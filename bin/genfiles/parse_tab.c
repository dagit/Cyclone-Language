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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 136
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 293 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 341
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
{struct _tuple10*_tmp80A;declarators=((_tmp80A=_region_malloc(r,sizeof(*_tmp80A)),((_tmp80A->tl=declarators,((_tmp80A->hd=_tmp1,_tmp80A))))));}{
struct Cyc_List_List*_tmp80B;exprs=((_tmp80B=_region_malloc(r,sizeof(*_tmp80B)),((_tmp80B->hd=_tmp2,((_tmp80B->tl=exprs,_tmp80B))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp80E;struct Cyc_Position_Exit_exn_struct*_tmp80D;(int)_throw((void*)((_tmp80D=_cycalloc_atomic(sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp80E.tag=Cyc_Position_Exit,_tmp80E)),_tmp80D)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp813;void*_tmp812[2];struct Cyc_String_pa_PrintArg_struct _tmp811;struct Cyc_String_pa_PrintArg_struct _tmp810;(_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp811.tag=0,((_tmp811.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp812[0]=& _tmp811,((_tmp812[1]=& _tmp810,Cyc_fprintf(Cyc_stderr,((_tmp813="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp813,sizeof(char),46))),_tag_dyneither(_tmp812,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp816;void*_tmp815;(_tmp815=0,Cyc_Tcutil_warn(loc,((_tmp816="qualifier on type is ignored",_tag_dyneither(_tmp816,sizeof(char),29))),_tag_dyneither(_tmp815,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp819;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp818;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp818=_cycalloc(sizeof(*_tmp818)),((_tmp818[0]=((_tmp819.tag=1,((_tmp819.f1=_tmp18,_tmp819)))),_tmp818)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp81A;return(_tmp81A.f1=nullable,((_tmp81A.f2=bound,((_tmp81A.f3=zeroterm,((_tmp81A.f4=rgn,_tmp81A)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp827;const char*_tmp826;void*_tmp825[1];struct Cyc_Int_pa_PrintArg_struct _tmp824;struct _tuple0*_tmp823;return(_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823->f1=Cyc_Absyn_Rel_n(0),((_tmp823->f2=(
(_tmp827=_cycalloc(sizeof(*_tmp827)),((_tmp827[0]=(struct _dyneither_ptr)((_tmp824.tag=1,((_tmp824.f1=(unsigned long)enum_counter ++,((_tmp825[0]=& _tmp824,Cyc_aprintf(((_tmp826="__anonymous_enum_%d__",_tag_dyneither(_tmp826,sizeof(char),22))),_tag_dyneither(_tmp825,sizeof(void*),1)))))))),_tmp827)))),_tmp823)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct _tuple15*f1;struct Cyc_Absyn_Exp*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple16*field_info){
# 246
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct _tuple16*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=_tmp23->f2;
if(_tmp27 != 0){
const char*_tmp828;Cyc_Parse_err(((_tmp828="bad type params in struct field",_tag_dyneither(_tmp828,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp829;Cyc_Parse_err(((_tmp829="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp829,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp82A;return(_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A->name=(*_tmp24).f2,((_tmp82A->tq=_tmp25,((_tmp82A->type=_tmp26,((_tmp82A->width=_tmp29,((_tmp82A->attributes=_tmp28,_tmp82A)))))))))));};}
# 255
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp82B;return(_tmp82B.Signed_spec=0,((_tmp82B.Unsigned_spec=0,((_tmp82B.Short_spec=0,((_tmp82B.Long_spec=0,((_tmp82B.Long_Long_spec=0,((_tmp82B.Valid_type_spec=0,((_tmp82B.Type_spec=Cyc_Absyn_sint_typ,((_tmp82B.loc=loc,_tmp82B)))))))))))))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp82E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82D;return Cyc_Absyn_starb_typ(_tmp35,
argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp36,
# 301
_tmp37 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp82D=_cycalloc(sizeof(*_tmp82D)),((_tmp82D[0]=((_tmp82E.tag=1,((_tmp82E.f1=_tmp37,_tmp82E)))),_tmp82D)))),_tmp38);}_LL14:;_LL15:
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
struct Cyc_String_pa_PrintArg_struct _tmp836;void*_tmp835[1];const char*_tmp834;struct _dyneither_ptr*_tmp833;struct _dyneither_ptr*nm=(_tmp833=_cycalloc(sizeof(*_tmp833)),((_tmp833[0]=(struct _dyneither_ptr)((_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E),((_tmp835[0]=& _tmp836,Cyc_aprintf(((_tmp834="`%s",_tag_dyneither(_tmp834,sizeof(char),4))),_tag_dyneither(_tmp835,sizeof(void*),1)))))))),_tmp833)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp845;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp844;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp843;struct Cyc_Absyn_Tvar*_tmp842;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp841;*_tmp47=(void*)((_tmp841=_cycalloc(sizeof(*_tmp841)),((_tmp841[0]=((_tmp845.tag=2,((_tmp845.f1=((_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842->name=nm,((_tmp842->identity=- 1,((_tmp842->kind=(void*)((_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844[0]=((_tmp843.tag=0,((_tmp843.f1=& Cyc_Tcutil_ik,_tmp843)))),_tmp844)))),_tmp842)))))))),_tmp845)))),_tmp841))));}
goto _LL1D;}_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 331
{struct _tuple17*_tmp848;struct Cyc_List_List*_tmp847;_tmp3C=((_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847->hd=((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848->f1=_tmp3E,((_tmp848->f2=_tmp40,_tmp848)))))),((_tmp847->tl=_tmp3C,_tmp847))))));}goto _LL16;_LL19: if(_tmp3D->f1 == 0)goto _LL1B;_tmp41=*_tmp3D->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3D->f3;if(_tmp42->tag != 15)goto _LL1B;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp42->f1);if(_tmp43->tag != 1)goto _LL1B;else{_tmp44=(void**)((void**)& _tmp43->f2);}}};_LL1A: {
# 335
struct Cyc_String_pa_PrintArg_struct _tmp850;void*_tmp84F[1];const char*_tmp84E;struct _dyneither_ptr*_tmp84D;struct _dyneither_ptr*nm=(_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D[0]=(struct _dyneither_ptr)((_tmp850.tag=0,((_tmp850.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp84F[0]=& _tmp850,Cyc_aprintf(((_tmp84E="`%s",_tag_dyneither(_tmp84E,sizeof(char),4))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))))),_tmp84D)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp85F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp85E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp85D;struct Cyc_Absyn_Tvar*_tmp85C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp85B;*_tmp44=(void*)((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85F.tag=2,((_tmp85F.f1=((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C->name=nm,((_tmp85C->identity=- 1,((_tmp85C->kind=(void*)((_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp85D.tag=0,((_tmp85D.f1=& Cyc_Tcutil_rk,_tmp85D)))),_tmp85E)))),_tmp85C)))))))),_tmp85F)))),_tmp85B))));}
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
{struct _tuple17*_tmp862;struct Cyc_List_List*_tmp861;_tmp5C=((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->hd=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp862->f2=_tmp5F,_tmp862)))))),((_tmp861->tl=_tmp5C,_tmp861))))));}goto _LL22;_LL25:;_LL26:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp865;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp864;return Cyc_Absyn_new_exp((void*)((_tmp864=_cycalloc(sizeof(*_tmp864)),((_tmp864[0]=((_tmp865.tag=37,((_tmp865.f1=Cyc_Tcutil_copy_type(_tmp68),_tmp865)))),_tmp864)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp86B;struct Cyc_Absyn_ArrayInfo _tmp86A;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp869;return(void*)((_tmp869=_cycalloc(sizeof(*_tmp869)),((_tmp869[0]=((_tmp86B.tag=8,((_tmp86B.f1=((_tmp86A.elt_type=_tmp7A,((_tmp86A.tq=_tmp6E,((_tmp86A.num_elts=nelts2,((_tmp86A.zero_term=_tmp70,((_tmp86A.zt_loc=_tmp71,_tmp86A)))))))))),_tmp86B)))),_tmp869))));}
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B;if(_tmp72->tag != 5)goto _LL31;else{_tmp73=(_tmp72->f1).elt_typ;_tmp74=(_tmp72->f1).elt_tq;_tmp75=((_tmp72->f1).ptr_atts).rgn;_tmp76=((_tmp72->f1).ptr_atts).nullable;_tmp77=((_tmp72->f1).ptr_atts).bounds;_tmp78=((_tmp72->f1).ptr_atts).zero_term;_tmp79=((_tmp72->f1).ptr_atts).ptrloc;}}_LL30: {
# 386
void*_tmp7E=Cyc_Parse_substitute_tags(tags,_tmp73);
union Cyc_Absyn_Constraint*_tmp7F=_tmp77;
{union Cyc_Absyn_Constraint _tmp80=*_tmp77;struct Cyc_Absyn_Exp*_tmp82;_LL34: if((_tmp80.Eq_constr).tag != 1)goto _LL36;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp81=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp80.Eq_constr).val);if(_tmp81->tag != 1)goto _LL36;else{_tmp82=_tmp81->f1;}};_LL35:
# 390
 _tmp82=Cyc_Parse_substitute_tags_exp(tags,_tmp82);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp871;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp870;union Cyc_Absyn_Constraint*_tmp86F;_tmp7F=((_tmp86F=_cycalloc(sizeof(*_tmp86F)),(((_tmp86F->Eq_constr).val=(void*)((_tmp871=_cycalloc(sizeof(*_tmp871)),((_tmp871[0]=((_tmp870.tag=1,((_tmp870.f1=_tmp82,_tmp870)))),_tmp871)))),(((_tmp86F->Eq_constr).tag=1,_tmp86F))))));}
goto _LL33;_LL36:;_LL37:
 goto _LL33;_LL33:;}
# 395
if(_tmp7E != _tmp73  || _tmp7F != _tmp77){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp87B;struct Cyc_Absyn_PtrAtts _tmp87A;struct Cyc_Absyn_PtrInfo _tmp879;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp878;return(void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp87B.tag=5,((_tmp87B.f1=((_tmp879.elt_typ=_tmp7E,((_tmp879.elt_tq=_tmp74,((_tmp879.ptr_atts=((_tmp87A.rgn=_tmp75,((_tmp87A.nullable=_tmp76,((_tmp87A.bounds=_tmp7F,((_tmp87A.zero_term=_tmp78,((_tmp87A.ptrloc=_tmp79,_tmp87A)))))))))),_tmp879)))))),_tmp87B)))),_tmp878))));}
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
struct _tuple18*_tmp87C;return(_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C->f1=(*t).f2,((_tmp87C->f2=(*t).f3,_tmp87C)))));}
# 419
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp8B=d->r;struct Cyc_Absyn_Vardecl*_tmp8D;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL3B;else{_tmp8D=_tmp8C->f1;}}_LL3A:
 return _tmp8D;_LL3B:;_LL3C: {
const char*_tmp87F;void*_tmp87E;(_tmp87E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp87F="bad declaration in `forarray' statement",_tag_dyneither(_tmp87F,sizeof(char),40))),_tag_dyneither(_tmp87E,sizeof(void*),0)));}_LL38:;}
# 426
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp90=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp91=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp90;if(_tmp91->tag != 4)goto _LL40;}_LL3F:
 return 1;_LL40:;_LL41:
 return 0;_LL3D:;}
# 435
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp880;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp880="`H",_tag_dyneither(_tmp880,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp881;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp881="`U",_tag_dyneither(_tmp881,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp882;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp882="`RC",_tag_dyneither(_tmp882,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 443
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88C;struct _dyneither_ptr*_tmp88B;struct Cyc_Absyn_Tvar*_tmp88A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp889;return(void*)((_tmp889=_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((_tmp88C.tag=2,((_tmp88C.f1=((_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A->name=((_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B[0]=s,_tmp88B)))),((_tmp88A->identity=- 1,((_tmp88A->kind=k,_tmp88A)))))))),_tmp88C)))),_tmp889))));}}}}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp99=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL45;}_LL44:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp88F;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp88E;k=(void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=1,((_tmp88F.f1=0,_tmp88F)))),_tmp88E))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f2;}}_LL46:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp892;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp891;k=(void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((_tmp892.tag=2,((_tmp892.f1=0,((_tmp892.f2=_tmp9C,_tmp892)))))),_tmp891))));}goto _LL42;_LL47: _tmp9D=_tmp99;_LL48:
 k=_tmp9D;goto _LL42;_LL42:;}{
# 453
struct Cyc_Absyn_Tvar*_tmp893;return(_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893->name=t->name,((_tmp893->identity=- 1,((_tmp893->kind=k,_tmp893)))))));};}
# 460
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL4C;else{_tmpA5=_tmpA4->f1;}}_LL4B:
 return _tmpA5;_LL4C:;_LL4D: {
const char*_tmp896;void*_tmp895;(_tmp895=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp896="expecting a list of type variables, not types",_tag_dyneither(_tmp896,sizeof(char),46))),_tag_dyneither(_tmp895,sizeof(void*),0)));}_LL49:;}
# 466
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp899;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp898;return(void*)((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898[0]=((_tmp899.tag=2,((_tmp899.f1=pr,_tmp899)))),_tmp898))));}
# 471
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void**_tmpAC;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAA;if(_tmpAB->tag != 2)goto _LL51;else{_tmpAC=(void**)&(_tmpAB->f1)->kind;}}_LL50: {
# 474
void*_tmpAD=Cyc_Absyn_compress_kb(*_tmpAC);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LL56;}_LL55:
# 476
 if(!leq)*_tmpAC=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp89C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp89B;*_tmpAC=(void*)((_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89C.tag=2,((_tmp89C.f1=0,((_tmp89C.f2=k,_tmp89C)))))),_tmp89B))));}
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
{const char*_tmp89F;void*_tmp89E;(_tmp89E=0,Cyc_Tcutil_warn(loc,((_tmp89F="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp89F,sizeof(char),93))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}
# 509
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB6=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB4;if(_tmpB6->tag != 0)goto _LL5D;else{_tmpB7=_tmpB6->f1;}}_LL61:
# 511
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8A2;void*_tmp8A1;(_tmp8A1=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8A2="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8A2,sizeof(char),73))),_tag_dyneither(_tmp8A1,sizeof(void*),0)));}{
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
const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8A5="initializer found in parameter declaration",_tag_dyneither(_tmp8A5,sizeof(char),43))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC0->name)){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8A8="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8A8,sizeof(char),47))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}
{struct _tuple8*_tmp8AB;struct Cyc_List_List*_tmp8AA;rev_new_params=(
(_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA->hd=((_tmp8AB=_cycalloc(sizeof(*_tmp8AB)),((_tmp8AB->f1=(*_tmpC0->name).f2,((_tmp8AB->f2=_tmpC0->tq,((_tmp8AB->f3=_tmpC0->type,_tmp8AB)))))))),((_tmp8AA->tl=rev_new_params,_tmp8AA))))));}
# 531
goto L;_LL65:;_LL66: {
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8AE="nonvariable declaration in parameter type",_tag_dyneither(_tmp8AE,sizeof(char),42))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}_LL62:;}
# 535
L: if(_tmpBC == 0){
const char*_tmp8B2;void*_tmp8B1[1];struct Cyc_String_pa_PrintArg_struct _tmp8B0;(_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpB7->hd)),((_tmp8B1[0]=& _tmp8B0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B2="%s is not given a type",_tag_dyneither(_tmp8B2,sizeof(char),23))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))));}}{
# 538
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8C1;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8BF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8BE;struct Cyc_List_List*_tmp8BD;return
(_tmp8BD=_region_malloc(yy,sizeof(*_tmp8BD)),((_tmp8BD->hd=(void*)((_tmp8C1=_region_malloc(yy,sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8BE.tag=3,((_tmp8BE.f1=(void*)((_tmp8C0=_region_malloc(yy,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8BF.tag=1,((_tmp8BF.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8BF.f2=0,((_tmp8BF.f3=0,((_tmp8BF.f4=0,((_tmp8BF.f5=0,_tmp8BF)))))))))))),_tmp8C0)))),_tmp8BE)))),_tmp8C1)))),((_tmp8BD->tl=0,_tmp8BD)))));};};_LL5D:;}
# 545
goto _LL5C;_LL5B:;_LL5C: {
struct Cyc_List_List*_tmp8C2;return(_tmp8C2=_region_malloc(yy,sizeof(*_tmp8C2)),((_tmp8C2->hd=(void*)tms->hd,((_tmp8C2->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8C2)))));}_LL58:;};}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 557
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8C3;d=((_tmp8C3.id=d.id,((_tmp8C3.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8C3))));}{
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
{const char*_tmp8C4;Cyc_Parse_err(((_tmp8C4="bad storage class on function",_tag_dyneither(_tmp8C4,sizeof(char),30))),loc);}break;}}{
# 579
void*_tmpD4=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct Cyc_Absyn_Tqual _tmpD6;void*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD9;struct _tuple13 _tmpD5=Cyc_Parse_apply_tms(tq,_tmpD4,atts,d.tms);_tmpD6=_tmpD5.f1;_tmpD7=_tmpD5.f2;_tmpD8=_tmpD5.f3;_tmpD9=_tmpD5.f4;
# 584
if(_tmpD8 != 0){
# 587
const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,Cyc_Tcutil_warn(loc,((_tmp8C7="bad type params, ignoring",_tag_dyneither(_tmp8C7,sizeof(char),26))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}{
# 589
void*_tmpDC=_tmpD7;struct Cyc_List_List*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE1;struct Cyc_List_List*_tmpE2;int _tmpE3;struct Cyc_Absyn_VarargInfo*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDC;if(_tmpDD->tag != 9)goto _LL6E;else{_tmpDE=(_tmpDD->f1).tvars;_tmpDF=(_tmpDD->f1).effect;_tmpE0=(_tmpDD->f1).ret_tqual;_tmpE1=(_tmpDD->f1).ret_typ;_tmpE2=(_tmpDD->f1).args;_tmpE3=(_tmpDD->f1).c_varargs;_tmpE4=(_tmpDD->f1).cyc_varargs;_tmpE5=(_tmpDD->f1).rgn_po;_tmpE6=(_tmpDD->f1).attributes;}}_LL6D: {
# 592
struct Cyc_List_List*_tmpE7=0;
{struct Cyc_List_List*_tmpE8=_tmpE2;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEC;struct _tuple8*_tmpE9=(struct _tuple8*)_tmpE8->hd;_tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=_tmpE9->f3;
if(_tmpEA == 0){
{const char*_tmp8C8;Cyc_Parse_err(((_tmp8C8="missing argument variable in function prototype",_tag_dyneither(_tmp8C8,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8CB;struct Cyc_List_List*_tmp8CA;_tmpE7=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->hd=((_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8CB->f2=_tmpEB,((_tmp8CB->f3=_tmpEC,_tmp8CB)))))))),((_tmp8CA->tl=_tmpE7,_tmp8CA))))));};}else{
# 599
struct _tuple8*_tmp8CE;struct Cyc_List_List*_tmp8CD;_tmpE7=((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD->hd=((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->f1=_tmpEA,((_tmp8CE->f2=_tmpEB,((_tmp8CE->f3=_tmpEC,_tmp8CE)))))))),((_tmp8CD->tl=_tmpE7,_tmp8CD))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp8CF;return(_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF->sc=sc,((_tmp8CF->name=d.id,((_tmp8CF->tvs=_tmpDE,((_tmp8CF->is_inline=is_inline,((_tmp8CF->effect=_tmpDF,((_tmp8CF->ret_tqual=_tmpE0,((_tmp8CF->ret_type=_tmpE1,((_tmp8CF->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE7),((_tmp8CF->c_varargs=_tmpE3,((_tmp8CF->cyc_varargs=_tmpE4,((_tmp8CF->rgn_po=_tmpE5,((_tmp8CF->body=body,((_tmp8CF->cached_typ=0,((_tmp8CF->param_vardecls=0,((_tmp8CF->fn_vardecl=0,((_tmp8CF->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE6,_tmpD9),_tmp8CF)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
const char*_tmp8D2;void*_tmp8D1;(_tmp8D1=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8D2="declarator is not a function prototype",_tag_dyneither(_tmp8D2,sizeof(char),39))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}_LL6B:;};};};}static char _tmpF6[52]="at most one type may appear within a type specifier";
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
void*_tmp8D3;(_tmp8D3=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D3,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8D4;(_tmp8D4=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D4,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D5,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8D6;(_tmp8D6=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D6,sizeof(void*),0)));}
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
const char*_tmp8D9;void*_tmp8D8;(_tmp8D8=0,Cyc_Tcutil_warn(loc,((_tmp8D9="missing type within specifier",_tag_dyneither(_tmp8D9,sizeof(char),30))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 689
if(seen_sign){
void*_tmp100=t;enum Cyc_Absyn_Sign _tmp102;enum Cyc_Absyn_Size_of _tmp103;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 6)goto _LL73;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;}}_LL72:
# 692
 if(_tmp102 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp103);
goto _LL70;_LL73:;_LL74:
{const char*_tmp8DA;Cyc_Parse_err(((_tmp8DA="sign specification on non-integral type",_tag_dyneither(_tmp8DA,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 697
if(seen_size){
void*_tmp105=t;enum Cyc_Absyn_Sign _tmp107;enum Cyc_Absyn_Size_of _tmp108;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp105;if(_tmp106->tag != 6)goto _LL78;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}_LL77:
# 700
 if(_tmp108 != sz)
t=Cyc_Absyn_int_typ(_tmp107,sz);
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp109=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp105;if(_tmp109->tag != 7)goto _LL7A;}_LL79:
# 704
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
{const char*_tmp8DB;Cyc_Parse_err(((_tmp8DB="size qualifier on non-integral type",_tag_dyneither(_tmp8DB,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
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
struct _tuple15*_tmp8DE;struct Cyc_List_List*_tmp8DD;return(_tmp8DD=_region_malloc(r,sizeof(*_tmp8DD)),((_tmp8DD->hd=((_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE->f1=_tmp10B,((_tmp8DE->f2=_tmp10D,((_tmp8DE->f3=_tmp10E,((_tmp8DE->f4=_tmp10F,((_tmp8DE->f5=_tmp110,_tmp8DE)))))))))))),((_tmp8DD->tl=0,_tmp8DD)))));}else{
# 724
struct _tuple15*_tmp8E1;struct Cyc_List_List*_tmp8E0;return(_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0->hd=((_tmp8E1=_region_malloc(r,sizeof(*_tmp8E1)),((_tmp8E1->f1=_tmp10B,((_tmp8E1->f2=_tmp10D,((_tmp8E1->f3=_tmp10E,((_tmp8E1->f4=_tmp10F,((_tmp8E1->f5=_tmp110,_tmp8E1)))))))))))),((_tmp8E0->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8E0)))));}};}
# 728
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 731
if(tms == 0){struct _tuple13 _tmp8E2;return(_tmp8E2.f1=tq,((_tmp8E2.f2=t,((_tmp8E2.f3=0,((_tmp8E2.f4=atts,_tmp8E2)))))));}{
void*_tmp116=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp118;unsigned int _tmp119;struct Cyc_Absyn_Exp*_tmp11B;union Cyc_Absyn_Constraint*_tmp11C;unsigned int _tmp11D;void*_tmp11F;struct Cyc_List_List*_tmp121;unsigned int _tmp122;struct Cyc_Absyn_PtrAtts _tmp124;struct Cyc_Absyn_Tqual _tmp125;unsigned int _tmp127;struct Cyc_List_List*_tmp128;_LL7D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp117->tag != 0)goto _LL7F;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7E:
# 734
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp118,_tmp119),atts,tms->tl);_LL7F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11A->tag != 1)goto _LL81;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;_tmp11D=_tmp11A->f3;}}_LL80:
# 737
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp11B,_tmp11C,_tmp11D),atts,tms->tl);_LL81: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp11E=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11E->tag != 3)goto _LL83;else{_tmp11F=(void*)_tmp11E->f1;}}_LL82: {
# 740
void*_tmp129=_tmp11F;struct Cyc_List_List*_tmp12B;int _tmp12C;struct Cyc_Absyn_VarargInfo*_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;unsigned int _tmp131;_LL8A: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp12A->tag != 1)goto _LL8C;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=(void*)_tmp12A->f4;_tmp12F=_tmp12A->f5;}}_LL8B: {
# 742
struct Cyc_List_List*typvars=0;
# 744
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp8E3;fn_atts=((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->hd=(void*)as->hd,((_tmp8E3->tl=fn_atts,_tmp8E3))))));}else{
# 749
struct Cyc_List_List*_tmp8E4;new_atts=((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4->hd=(void*)as->hd,((_tmp8E4->tl=new_atts,_tmp8E4))))));}}}
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
struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Tqual _tmp13B;void**_tmp13C;struct _tuple8*_tmp139=(struct _tuple8*)_tmp138->hd;_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=(void**)& _tmp139->f3;
if(_tmp137 != 0)
*_tmp13C=Cyc_Parse_substitute_tags(_tmp137,*_tmp13C);
*_tmp13C=Cyc_Parse_array2ptr(*_tmp13C,1);}}
# 790
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp12E,tq,t,_tmp12B,_tmp12C,_tmp12D,_tmp12F,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL8C: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp130=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp130->tag != 0)goto _LL89;else{_tmp131=_tmp130->f2;}}_LL8D: {
# 796
const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp131,((_tmp8E7="function declaration without parameter types",_tag_dyneither(_tmp8E7,sizeof(char),45))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp120=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp120->tag != 4)goto _LL85;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL84:
# 803
 if(tms->tl == 0){
struct _tuple13 _tmp8E8;return(_tmp8E8.f1=tq,((_tmp8E8.f2=t,((_tmp8E8.f3=_tmp121,((_tmp8E8.f4=atts,_tmp8E8)))))));}{
# 808
const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp122,((_tmp8EB="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp8EB,sizeof(char),68))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp123=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp123->tag != 2)goto _LL87;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LL86: {
# 811
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8F1;struct Cyc_Absyn_PtrInfo _tmp8F0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8EF;return Cyc_Parse_apply_tms(_tmp125,(void*)((_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF[0]=((_tmp8F1.tag=5,((_tmp8F1.f1=((_tmp8F0.elt_typ=t,((_tmp8F0.elt_tq=tq,((_tmp8F0.ptr_atts=_tmp124,_tmp8F0)))))),_tmp8F1)))),_tmp8EF)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp126=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp126->tag != 5)goto _LL7C;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LL88:
# 816
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp128),tms->tl);_LL7C:;};}
# 822
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 831
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp146;struct Cyc_Absyn_Tqual _tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp14A;struct _tuple15*_tmp145=t;_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;_tmp148=_tmp145->f3;_tmp149=_tmp145->f4;_tmp14A=_tmp145->f5;
# 834
Cyc_Lex_register_typedef(_tmp146);{
# 836
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp14B=_tmp148;struct Cyc_Core_Opt*_tmp14D;_LL94: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14B;if(_tmp14C->tag != 1)goto _LL96;else{_tmp14D=_tmp14C->f1;}}_LL95:
# 840
 type=0;
if(_tmp14D == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp14D;}
goto _LL93;_LL96:;_LL97:
 kind=0;type=_tmp148;goto _LL93;_LL93:;}{
# 846
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8F7;struct Cyc_Absyn_Typedefdecl*_tmp8F6;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp8F5;return Cyc_Absyn_new_decl((void*)((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F7.tag=9,((_tmp8F7.f1=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->name=_tmp146,((_tmp8F6->tvs=_tmp149,((_tmp8F6->kind=kind,((_tmp8F6->defn=type,((_tmp8F6->atts=_tmp14A,((_tmp8F6->tq=_tmp147,_tmp8F6)))))))))))))),_tmp8F7)))),_tmp8F5)))),loc);};};}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp8FA;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp8F9;return Cyc_Absyn_new_stmt((void*)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FA.tag=12,((_tmp8FA.f1=d,((_tmp8FA.f2=s,_tmp8FA)))))),_tmp8F9)))),d->loc);}
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
const char*_tmp8FB;Cyc_Parse_err(((_tmp8FB="inline is allowed only on function definitions",_tag_dyneither(_tmp8FB,sizeof(char),47))),loc);}{
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
void*_tmp15A=_tmp159;struct Cyc_Absyn_Aggrdecl*_tmp15D;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Datatypedecl*_tmp163;enum Cyc_Absyn_AggrKind _tmp165;struct _tuple0*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Datatypedecl**_tmp169;struct _tuple0*_tmp16B;int _tmp16C;struct Cyc_List_List*_tmp16D;struct _tuple0*_tmp16F;struct Cyc_List_List*_tmp171;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15B->tag != 26)goto _LLA3;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp15C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp15B->f1)->r;if(_tmp15C->tag != 0)goto _LLA3;else{_tmp15D=_tmp15C->f1;}}}_LLA2:
# 912
 _tmp15D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp15D->attributes,_tmp157);{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp901;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp900;struct Cyc_List_List*_tmp8FF;struct Cyc_List_List*_tmp175=(_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->hd=Cyc_Absyn_new_decl((void*)((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((_tmp900.tag=6,((_tmp900.f1=_tmp15D,_tmp900)))),_tmp901)))),loc),((_tmp8FF->tl=0,_tmp8FF)))));_npop_handler(0);return _tmp175;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15E->tag != 26)goto _LLA5;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp15F=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp15E->f1)->r;if(_tmp15F->tag != 1)goto _LLA5;else{_tmp160=_tmp15F->f1;}}}_LLA4:
# 915
 if(_tmp157 != 0){const char*_tmp902;Cyc_Parse_err(((_tmp902="attributes on enum not supported",_tag_dyneither(_tmp902,sizeof(char),33))),loc);}{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp908;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp907;struct Cyc_List_List*_tmp906;struct Cyc_List_List*_tmp17A=(_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906->hd=Cyc_Absyn_new_decl((void*)((_tmp908=_cycalloc(sizeof(*_tmp908)),((_tmp908[0]=((_tmp907.tag=8,((_tmp907.f1=_tmp160,_tmp907)))),_tmp908)))),loc),((_tmp906->tl=0,_tmp906)))));_npop_handler(0);return _tmp17A;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp161->tag != 26)goto _LLA7;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp162=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp161->f1)->r;if(_tmp162->tag != 2)goto _LLA7;else{_tmp163=_tmp162->f1;}}}_LLA6:
# 918
 if(_tmp157 != 0){const char*_tmp909;Cyc_Parse_err(((_tmp909="attributes on datatypes not supported",_tag_dyneither(_tmp909,sizeof(char),38))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp90F;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp90E;struct Cyc_List_List*_tmp90D;struct Cyc_List_List*_tmp17F=(_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D->hd=Cyc_Absyn_new_decl((void*)((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp90E.tag=7,((_tmp90E.f1=_tmp163,_tmp90E)))),_tmp90F)))),loc),((_tmp90D->tl=0,_tmp90D)))));_npop_handler(0);return _tmp17F;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15A;if(_tmp164->tag != 11)goto _LLA9;else{if((((_tmp164->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA9;_tmp165=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f1;_tmp166=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f2;_tmp167=(_tmp164->f1).targs;}}_LLA8: {
# 921
struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp167);
struct Cyc_Absyn_Aggrdecl*_tmp910;struct Cyc_Absyn_Aggrdecl*_tmp181=(_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910->kind=_tmp165,((_tmp910->sc=s,((_tmp910->name=_tmp166,((_tmp910->tvs=_tmp180,((_tmp910->impl=0,((_tmp910->attributes=0,_tmp910)))))))))))));
if(_tmp157 != 0){const char*_tmp911;Cyc_Parse_err(((_tmp911="bad attributes on type declaration",_tag_dyneither(_tmp911,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp917;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp916;struct Cyc_List_List*_tmp915;struct Cyc_List_List*_tmp186=(_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=Cyc_Absyn_new_decl((void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp916.tag=6,((_tmp916.f1=_tmp181,_tmp916)))),_tmp917)))),loc),((_tmp915->tl=0,_tmp915)))));_npop_handler(0);return _tmp186;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp168->tag != 3)goto _LLAB;else{if((((_tmp168->f1).datatype_info).KnownDatatype).tag != 2)goto _LLAB;_tmp169=(struct Cyc_Absyn_Datatypedecl**)(((_tmp168->f1).datatype_info).KnownDatatype).val;}}_LLAA:
# 926
 if(_tmp157 != 0){const char*_tmp918;Cyc_Parse_err(((_tmp918="bad attributes on datatype",_tag_dyneither(_tmp918,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp91E;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp91D;struct Cyc_List_List*_tmp91C;struct Cyc_List_List*_tmp18C=(_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->hd=Cyc_Absyn_new_decl((void*)((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91D.tag=7,((_tmp91D.f1=*_tmp169,_tmp91D)))),_tmp91E)))),loc),((_tmp91C->tl=0,_tmp91C)))));_npop_handler(0);return _tmp18C;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp16A->tag != 3)goto _LLAD;else{if((((_tmp16A->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLAD;_tmp16B=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).name;_tmp16C=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp16D=(_tmp16A->f1).targs;}}_LLAC: {
# 929
struct Cyc_List_List*_tmp18D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp16D);
struct Cyc_Absyn_Decl*_tmp18E=Cyc_Absyn_datatype_decl(s,_tmp16B,_tmp18D,0,_tmp16C,loc);
if(_tmp157 != 0){const char*_tmp91F;Cyc_Parse_err(((_tmp91F="bad attributes on datatype",_tag_dyneither(_tmp91F,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp920;struct Cyc_List_List*_tmp191=(_tmp920=_cycalloc(sizeof(*_tmp920)),((_tmp920->hd=_tmp18E,((_tmp920->tl=0,_tmp920)))));_npop_handler(0);return _tmp191;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15A;if(_tmp16E->tag != 13)goto _LLAF;else{_tmp16F=_tmp16E->f1;}}_LLAE: {
# 934
struct Cyc_Absyn_Enumdecl*_tmp921;struct Cyc_Absyn_Enumdecl*_tmp192=(_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921->sc=s,((_tmp921->name=_tmp16F,((_tmp921->fields=0,_tmp921)))))));
if(_tmp157 != 0){const char*_tmp922;Cyc_Parse_err(((_tmp922="bad attributes on enum",_tag_dyneither(_tmp922,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp92C;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp92B;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp92A;struct Cyc_List_List*_tmp929;struct Cyc_List_List*_tmp198=(_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929->hd=((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->r=(void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp92B.tag=8,((_tmp92B.f1=_tmp192,_tmp92B)))),_tmp92A)))),((_tmp92C->loc=loc,_tmp92C)))))),((_tmp929->tl=0,_tmp929)))));_npop_handler(0);return _tmp198;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15A;if(_tmp170->tag != 14)goto _LLB1;else{_tmp171=_tmp170->f1;}}_LLB0: {
# 940
struct Cyc_Core_Opt*_tmp92F;struct Cyc_Absyn_Enumdecl*_tmp92E;struct Cyc_Absyn_Enumdecl*_tmp19A=(_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E->sc=s,((_tmp92E->name=Cyc_Parse_gensym_enum(),((_tmp92E->fields=((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->v=_tmp171,_tmp92F)))),_tmp92E)))))));
if(_tmp157 != 0){const char*_tmp930;Cyc_Parse_err(((_tmp930="bad attributes on enum",_tag_dyneither(_tmp930,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp93A;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp939;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp938;struct Cyc_List_List*_tmp937;struct Cyc_List_List*_tmp1A0=(_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->hd=((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->r=(void*)((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=8,((_tmp939.f1=_tmp19A,_tmp939)))),_tmp938)))),((_tmp93A->loc=loc,_tmp93A)))))),((_tmp937->tl=0,_tmp937)))));_npop_handler(0);return _tmp1A0;};}_LLB1:;_LLB2:
{const char*_tmp93B;Cyc_Parse_err(((_tmp93B="missing declarator",_tag_dyneither(_tmp93B,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1A4=0;_npop_handler(0);return _tmp1A4;};_LLA0:;}else{
# 947
struct Cyc_List_List*_tmp1A5=Cyc_Parse_apply_tmss(mkrgn,_tmp155,_tmp159,declarators,_tmp157);
if(istypedef){
# 952
if(!exps_empty){
const char*_tmp93C;Cyc_Parse_err(((_tmp93C="initializer in typedef declaration",_tag_dyneither(_tmp93C,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1A5);
struct Cyc_List_List*_tmp1A7=decls;_npop_handler(0);return _tmp1A7;};}else{
# 958
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1A8=_tmp1A5;for(0;_tmp1A8 != 0;(_tmp1A8=_tmp1A8->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple0*_tmp1AA;struct Cyc_Absyn_Tqual _tmp1AB;void*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct _tuple15*_tmp1A9=(struct _tuple15*)_tmp1A8->hd;_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;_tmp1AC=_tmp1A9->f3;_tmp1AD=_tmp1A9->f4;_tmp1AE=_tmp1A9->f5;
if(_tmp1AD != 0){
const char*_tmp93F;void*_tmp93E;(_tmp93E=0,Cyc_Tcutil_warn(loc,((_tmp93F="bad type params, ignoring",_tag_dyneither(_tmp93F,sizeof(char),26))),_tag_dyneither(_tmp93E,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp942;void*_tmp941;(_tmp941=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp942="unexpected NULL in parse!",_tag_dyneither(_tmp942,sizeof(char),26))),_tag_dyneither(_tmp941,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1B3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1B4=Cyc_Absyn_new_vardecl(_tmp1AA,_tmp1AC,_tmp1B3);
_tmp1B4->tq=_tmp1AB;
_tmp1B4->sc=s;
_tmp1B4->attributes=_tmp1AE;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp948;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp947;struct Cyc_Absyn_Decl*_tmp946;struct Cyc_Absyn_Decl*_tmp1B5=(_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946->r=(void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948[0]=((_tmp947.tag=0,((_tmp947.f1=_tmp1B4,_tmp947)))),_tmp948)))),((_tmp946->loc=loc,_tmp946)))));
struct Cyc_List_List*_tmp949;decls=((_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->hd=_tmp1B5,((_tmp949->tl=decls,_tmp949))))));};};}}{
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
{const char*_tmp94E;void*_tmp94D[2];struct Cyc_String_pa_PrintArg_struct _tmp94C;struct Cyc_Int_pa_PrintArg_struct _tmp94B;Cyc_Parse_err((struct _dyneither_ptr)((_tmp94B.tag=1,((_tmp94B.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp94D[0]=& _tmp94C,((_tmp94D[1]=& _tmp94B,Cyc_aprintf(((_tmp94E="bad kind: %s; strlen=%d",_tag_dyneither(_tmp94E,sizeof(char),24))),_tag_dyneither(_tmp94D,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1014
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BF=p->r;struct _tuple0*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C3;struct Cyc_Absyn_Pat*_tmp1C6;enum Cyc_Absyn_Sign _tmp1C9;int _tmp1CA;char _tmp1CC;struct _dyneither_ptr _tmp1CE;int _tmp1CF;struct _tuple0*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;_LLCA: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1C0=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C0->tag != 14)goto _LLCC;else{_tmp1C1=_tmp1C0->f1;}}_LLCB:
 return Cyc_Absyn_unknownid_exp(_tmp1C1,p->loc);_LLCC: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C2->tag != 2)goto _LLCE;else{_tmp1C3=_tmp1C2->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1C2->f2)->r;if(_tmp1C4->tag != 0)goto _LLCE;};}}_LLCD:
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
{const char*_tmp94F;Cyc_Parse_err(((_tmp94F="cannot mix patterns and expressions in case",_tag_dyneither(_tmp94F,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp950;return((_tmp950.Int_tok).val=yy1,(((_tmp950.Int_tok).tag=1,_tmp950)));}static char _tmp1DC[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp951;return((_tmp951.Char_tok).val=yy1,(((_tmp951.Char_tok).tag=2,_tmp951)));}static char _tmp1E1[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp952;return((_tmp952.String_tok).val=yy1,(((_tmp952.String_tok).tag=3,_tmp952)));}static char _tmp1E6[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp953;return((_tmp953.YY1).val=yy1,(((_tmp953.YY1).tag=7,_tmp953)));}static char _tmp1EB[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp954;return((_tmp954.YY2).val=yy1,(((_tmp954.YY2).tag=8,_tmp954)));}static char _tmp1F0[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp955;return((_tmp955.YY3).val=yy1,(((_tmp955.YY3).tag=9,_tmp955)));}static char _tmp1F5[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp956;return((_tmp956.YY4).val=yy1,(((_tmp956.YY4).tag=10,_tmp956)));}static char _tmp1FA[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp957;return((_tmp957.YY5).val=yy1,(((_tmp957.YY5).tag=11,_tmp957)));}static char _tmp1FF[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp958;return((_tmp958.YY6).val=yy1,(((_tmp958.YY6).tag=12,_tmp958)));}static char _tmp204[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp959;return((_tmp959.YY7).val=yy1,(((_tmp959.YY7).tag=13,_tmp959)));}static char _tmp209[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp95A;return((_tmp95A.YY8).val=yy1,(((_tmp95A.YY8).tag=14,_tmp95A)));}static char _tmp20E[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp95B;return((_tmp95B.QualId_tok).val=yy1,(((_tmp95B.QualId_tok).tag=5,_tmp95B)));}static char _tmp213[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp95C;return((_tmp95C.YY9).val=yy1,(((_tmp95C.YY9).tag=15,_tmp95C)));}static char _tmp218[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95D;return((_tmp95D.YY10).val=yy1,(((_tmp95D.YY10).tag=16,_tmp95D)));}static char _tmp21D[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp95E;return((_tmp95E.YY11).val=yy1,(((_tmp95E.YY11).tag=17,_tmp95E)));}static char _tmp222[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY12(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp95F;return((_tmp95F.YY12).val=yy1,(((_tmp95F.YY12).tag=18,_tmp95F)));}static char _tmp227[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp960;return((_tmp960.YY13).val=yy1,(((_tmp960.YY13).tag=19,_tmp960)));}static char _tmp22C[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp961;return((_tmp961.YY14).val=yy1,(((_tmp961.YY14).tag=20,_tmp961)));}static char _tmp231[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp962;return((_tmp962.YY15).val=yy1,(((_tmp962.YY15).tag=21,_tmp962)));}static char _tmp236[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY16(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp963;return((_tmp963.YY16).val=yy1,(((_tmp963.YY16).tag=22,_tmp963)));}static char _tmp23B[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp964;return((_tmp964.YY17).val=yy1,(((_tmp964.YY17).tag=23,_tmp964)));}static char _tmp240[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp965;return((_tmp965.YY18).val=yy1,(((_tmp965.YY18).tag=24,_tmp965)));}static char _tmp245[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.YY19).val=yy1,(((_tmp966.YY19).tag=25,_tmp966)));}static char _tmp24A[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY20(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.YY20).val=yy1,(((_tmp967.YY20).tag=26,_tmp967)));}static char _tmp24F[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY21(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.YY21).val=yy1,(((_tmp968.YY21).tag=27,_tmp968)));}static char _tmp254[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY22).val=yy1,(((_tmp969.YY22).tag=28,_tmp969)));}static char _tmp259[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY23).val=yy1,(((_tmp96A.YY23).tag=29,_tmp96A)));}static char _tmp25E[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY24).val=yy1,(((_tmp96B.YY24).tag=30,_tmp96B)));}static char _tmp263[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY25).val=yy1,(((_tmp96C.YY25).tag=31,_tmp96C)));}static char _tmp268[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY26).val=yy1,(((_tmp96D.YY26).tag=32,_tmp96D)));}static char _tmp26D[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY27).val=yy1,(((_tmp96E.YY27).tag=33,_tmp96E)));}static char _tmp272[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY28).val=yy1,(((_tmp96F.YY28).tag=34,_tmp96F)));}static char _tmp277[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY29).val=yy1,(((_tmp970.YY29).tag=35,_tmp970)));}static char _tmp27C[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY30).val=yy1,(((_tmp971.YY30).tag=36,_tmp971)));}static char _tmp281[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY31(int yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY31).val=yy1,(((_tmp972.YY31).tag=37,_tmp972)));}static char _tmp286[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY32).val=yy1,(((_tmp973.YY32).tag=38,_tmp973)));}static char _tmp28B[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY33).val=yy1,(((_tmp974.YY33).tag=39,_tmp974)));}static char _tmp290[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY34).val=yy1,(((_tmp975.YY34).tag=40,_tmp975)));}static char _tmp295[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY35(struct _tuple23 yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY35).val=yy1,(((_tmp976.YY35).tag=41,_tmp976)));}static char _tmp29A[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY36).val=yy1,(((_tmp977.YY36).tag=42,_tmp977)));}static char _tmp29F[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY37).val=yy1,(((_tmp978.YY37).tag=43,_tmp978)));}static char _tmp2A4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY38).val=yy1,(((_tmp979.YY38).tag=44,_tmp979)));}static char _tmp2A9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY39).val=yy1,(((_tmp97A.YY39).tag=45,_tmp97A)));}static char _tmp2AE[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY40).val=yy1,(((_tmp97B.YY40).tag=46,_tmp97B)));}static char _tmp2B3[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY41).val=yy1,(((_tmp97C.YY41).tag=47,_tmp97C)));}static char _tmp2B8[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY42).val=yy1,(((_tmp97D.YY42).tag=48,_tmp97D)));}static char _tmp2BD[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY43).val=yy1,(((_tmp97E.YY43).tag=49,_tmp97E)));}static char _tmp2C2[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY44(void*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY44).val=yy1,(((_tmp97F.YY44).tag=50,_tmp97F)));}static char _tmp2C7[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY45).val=yy1,(((_tmp980.YY45).tag=51,_tmp980)));}static char _tmp2CC[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY46).val=yy1,(((_tmp981.YY46).tag=52,_tmp981)));}static char _tmp2D1[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY47).val=yy1,(((_tmp982.YY47).tag=53,_tmp982)));}static char _tmp2D6[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY48).val=yy1,(((_tmp983.YY48).tag=54,_tmp983)));}static char _tmp2DB[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY49).val=yy1,(((_tmp984.YY49).tag=55,_tmp984)));}static char _tmp2E0[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY50).val=yy1,(((_tmp985.YY50).tag=56,_tmp985)));}static char _tmp2E5[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY51).val=yy1,(((_tmp986.YY51).tag=57,_tmp986)));}static char _tmp2EA[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY52).val=yy1,(((_tmp987.YY52).tag=58,_tmp987)));}static char _tmp2EF[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY53).val=yy1,(((_tmp988.YY53).tag=59,_tmp988)));}static char _tmp2F4[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY54).val=yy1,(((_tmp989.YY54).tag=60,_tmp989)));}static char _tmp2F9[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple19 yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.Asm_tok).val=yy1,(((_tmp98A.Asm_tok).tag=6,_tmp98A)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1187
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1191
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp98B;return(_tmp98B.timestamp=0,((_tmp98B.first_line=0,((_tmp98B.first_column=0,((_tmp98B.last_line=0,((_tmp98B.last_column=0,_tmp98B)))))))));}
# 1194
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1205 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp2FE[2]="$";static char _tmp2FF[6]="error";static char _tmp300[12]="$undefined.";static char _tmp301[5]="AUTO";static char _tmp302[9]="REGISTER";static char _tmp303[7]="STATIC";static char _tmp304[7]="EXTERN";static char _tmp305[8]="TYPEDEF";static char _tmp306[5]="VOID";static char _tmp307[5]="CHAR";static char _tmp308[6]="SHORT";static char _tmp309[4]="INT";static char _tmp30A[5]="LONG";static char _tmp30B[6]="FLOAT";static char _tmp30C[7]="DOUBLE";static char _tmp30D[7]="SIGNED";static char _tmp30E[9]="UNSIGNED";static char _tmp30F[6]="CONST";static char _tmp310[9]="VOLATILE";static char _tmp311[9]="RESTRICT";static char _tmp312[7]="STRUCT";static char _tmp313[6]="UNION";static char _tmp314[5]="CASE";static char _tmp315[8]="DEFAULT";static char _tmp316[7]="INLINE";static char _tmp317[7]="SIZEOF";static char _tmp318[9]="OFFSETOF";static char _tmp319[3]="IF";static char _tmp31A[5]="ELSE";static char _tmp31B[7]="SWITCH";static char _tmp31C[6]="WHILE";static char _tmp31D[3]="DO";static char _tmp31E[4]="FOR";static char _tmp31F[5]="GOTO";static char _tmp320[9]="CONTINUE";static char _tmp321[6]="BREAK";static char _tmp322[7]="RETURN";static char _tmp323[5]="ENUM";static char _tmp324[8]="NULL_kw";static char _tmp325[4]="LET";static char _tmp326[6]="THROW";static char _tmp327[4]="TRY";static char _tmp328[6]="CATCH";static char _tmp329[7]="EXPORT";static char _tmp32A[4]="NEW";static char _tmp32B[9]="ABSTRACT";static char _tmp32C[9]="FALLTHRU";static char _tmp32D[6]="USING";static char _tmp32E[10]="NAMESPACE";static char _tmp32F[9]="DATATYPE";static char _tmp330[8]="XTUNION";static char _tmp331[7]="TUNION";static char _tmp332[7]="MALLOC";static char _tmp333[8]="RMALLOC";static char _tmp334[7]="CALLOC";static char _tmp335[8]="RCALLOC";static char _tmp336[5]="SWAP";static char _tmp337[9]="REGION_T";static char _tmp338[6]="TAG_T";static char _tmp339[7]="REGION";static char _tmp33A[5]="RNEW";static char _tmp33B[8]="REGIONS";static char _tmp33C[13]="RESET_REGION";static char _tmp33D[16]="NOZEROTERM_QUAL";static char _tmp33E[14]="ZEROTERM_QUAL";static char _tmp33F[12]="REGION_QUAL";static char _tmp340[7]="PORTON";static char _tmp341[8]="PORTOFF";static char _tmp342[12]="DYNREGION_T";static char _tmp343[6]="ALIAS";static char _tmp344[8]="NUMELTS";static char _tmp345[8]="VALUEOF";static char _tmp346[10]="VALUEOF_T";static char _tmp347[9]="TAGCHECK";static char _tmp348[13]="NUMELTS_QUAL";static char _tmp349[10]="THIN_QUAL";static char _tmp34A[9]="FAT_QUAL";static char _tmp34B[13]="NOTNULL_QUAL";static char _tmp34C[14]="NULLABLE_QUAL";static char _tmp34D[12]="TAGGED_QUAL";static char _tmp34E[16]="EXTENSIBLE_QUAL";static char _tmp34F[15]="RESETABLE_QUAL";static char _tmp350[7]="PTR_OP";static char _tmp351[7]="INC_OP";static char _tmp352[7]="DEC_OP";static char _tmp353[8]="LEFT_OP";static char _tmp354[9]="RIGHT_OP";static char _tmp355[6]="LE_OP";static char _tmp356[6]="GE_OP";static char _tmp357[6]="EQ_OP";static char _tmp358[6]="NE_OP";static char _tmp359[7]="AND_OP";static char _tmp35A[6]="OR_OP";static char _tmp35B[11]="MUL_ASSIGN";static char _tmp35C[11]="DIV_ASSIGN";static char _tmp35D[11]="MOD_ASSIGN";static char _tmp35E[11]="ADD_ASSIGN";static char _tmp35F[11]="SUB_ASSIGN";static char _tmp360[12]="LEFT_ASSIGN";static char _tmp361[13]="RIGHT_ASSIGN";static char _tmp362[11]="AND_ASSIGN";static char _tmp363[11]="XOR_ASSIGN";static char _tmp364[10]="OR_ASSIGN";static char _tmp365[9]="ELLIPSIS";static char _tmp366[11]="LEFT_RIGHT";static char _tmp367[12]="COLON_COLON";static char _tmp368[11]="IDENTIFIER";static char _tmp369[17]="INTEGER_CONSTANT";static char _tmp36A[7]="STRING";static char _tmp36B[8]="WSTRING";static char _tmp36C[19]="CHARACTER_CONSTANT";static char _tmp36D[20]="WCHARACTER_CONSTANT";static char _tmp36E[18]="FLOATING_CONSTANT";static char _tmp36F[9]="TYPE_VAR";static char _tmp370[13]="TYPEDEF_NAME";static char _tmp371[16]="QUAL_IDENTIFIER";static char _tmp372[18]="QUAL_TYPEDEF_NAME";static char _tmp373[10]="ATTRIBUTE";static char _tmp374[4]="ASM";static char _tmp375[4]="';'";static char _tmp376[4]="'{'";static char _tmp377[4]="'}'";static char _tmp378[4]="','";static char _tmp379[4]="'='";static char _tmp37A[4]="'<'";static char _tmp37B[4]="'>'";static char _tmp37C[4]="'('";static char _tmp37D[4]="')'";static char _tmp37E[4]="'_'";static char _tmp37F[4]="'$'";static char _tmp380[4]="':'";static char _tmp381[4]="'.'";static char _tmp382[4]="'['";static char _tmp383[4]="']'";static char _tmp384[4]="'*'";static char _tmp385[4]="'@'";static char _tmp386[4]="'?'";static char _tmp387[4]="'+'";static char _tmp388[4]="'|'";static char _tmp389[4]="'^'";static char _tmp38A[4]="'&'";static char _tmp38B[4]="'-'";static char _tmp38C[4]="'/'";static char _tmp38D[4]="'%'";static char _tmp38E[4]="'~'";static char _tmp38F[4]="'!'";static char _tmp390[5]="prog";static char _tmp391[17]="translation_unit";static char _tmp392[12]="export_list";static char _tmp393[19]="export_list_values";static char _tmp394[21]="external_declaration";static char _tmp395[20]="function_definition";static char _tmp396[21]="function_definition2";static char _tmp397[13]="using_action";static char _tmp398[15]="unusing_action";static char _tmp399[17]="namespace_action";static char _tmp39A[19]="unnamespace_action";static char _tmp39B[12]="declaration";static char _tmp39C[19]="resetable_qual_opt";static char _tmp39D[17]="declaration_list";static char _tmp39E[23]="declaration_specifiers";static char _tmp39F[24]="storage_class_specifier";static char _tmp3A0[15]="attributes_opt";static char _tmp3A1[11]="attributes";static char _tmp3A2[15]="attribute_list";static char _tmp3A3[10]="attribute";static char _tmp3A4[15]="type_specifier";static char _tmp3A5[25]="type_specifier_notypedef";static char _tmp3A6[5]="kind";static char _tmp3A7[15]="type_qualifier";static char _tmp3A8[15]="enum_specifier";static char _tmp3A9[11]="enum_field";static char _tmp3AA[22]="enum_declaration_list";static char _tmp3AB[26]="struct_or_union_specifier";static char _tmp3AC[16]="type_params_opt";static char _tmp3AD[16]="struct_or_union";static char _tmp3AE[24]="struct_declaration_list";static char _tmp3AF[25]="struct_declaration_list0";static char _tmp3B0[21]="init_declarator_list";static char _tmp3B1[22]="init_declarator_list0";static char _tmp3B2[16]="init_declarator";static char _tmp3B3[19]="struct_declaration";static char _tmp3B4[25]="specifier_qualifier_list";static char _tmp3B5[35]="notypedef_specifier_qualifier_list";static char _tmp3B6[23]="struct_declarator_list";static char _tmp3B7[24]="struct_declarator_list0";static char _tmp3B8[18]="struct_declarator";static char _tmp3B9[19]="datatype_specifier";static char _tmp3BA[14]="qual_datatype";static char _tmp3BB[19]="datatypefield_list";static char _tmp3BC[20]="datatypefield_scope";static char _tmp3BD[14]="datatypefield";static char _tmp3BE[11]="declarator";static char _tmp3BF[23]="declarator_withtypedef";static char _tmp3C0[18]="direct_declarator";static char _tmp3C1[30]="direct_declarator_withtypedef";static char _tmp3C2[8]="pointer";static char _tmp3C3[12]="one_pointer";static char _tmp3C4[14]="pointer_quals";static char _tmp3C5[13]="pointer_qual";static char _tmp3C6[23]="pointer_null_and_bound";static char _tmp3C7[14]="pointer_bound";static char _tmp3C8[18]="zeroterm_qual_opt";static char _tmp3C9[8]="rgn_opt";static char _tmp3CA[11]="tqual_list";static char _tmp3CB[20]="parameter_type_list";static char _tmp3CC[9]="type_var";static char _tmp3CD[16]="optional_effect";static char _tmp3CE[19]="optional_rgn_order";static char _tmp3CF[10]="rgn_order";static char _tmp3D0[16]="optional_inject";static char _tmp3D1[11]="effect_set";static char _tmp3D2[14]="atomic_effect";static char _tmp3D3[11]="region_set";static char _tmp3D4[15]="parameter_list";static char _tmp3D5[22]="parameter_declaration";static char _tmp3D6[16]="identifier_list";static char _tmp3D7[17]="identifier_list0";static char _tmp3D8[12]="initializer";static char _tmp3D9[18]="array_initializer";static char _tmp3DA[17]="initializer_list";static char _tmp3DB[12]="designation";static char _tmp3DC[16]="designator_list";static char _tmp3DD[11]="designator";static char _tmp3DE[10]="type_name";static char _tmp3DF[14]="any_type_name";static char _tmp3E0[15]="type_name_list";static char _tmp3E1[20]="abstract_declarator";static char _tmp3E2[27]="direct_abstract_declarator";static char _tmp3E3[10]="statement";static char _tmp3E4[13]="open_exp_opt";static char _tmp3E5[18]="labeled_statement";static char _tmp3E6[21]="expression_statement";static char _tmp3E7[19]="compound_statement";static char _tmp3E8[16]="block_item_list";static char _tmp3E9[20]="selection_statement";static char _tmp3EA[15]="switch_clauses";static char _tmp3EB[20]="iteration_statement";static char _tmp3EC[15]="jump_statement";static char _tmp3ED[12]="exp_pattern";static char _tmp3EE[20]="conditional_pattern";static char _tmp3EF[19]="logical_or_pattern";static char _tmp3F0[20]="logical_and_pattern";static char _tmp3F1[21]="inclusive_or_pattern";static char _tmp3F2[21]="exclusive_or_pattern";static char _tmp3F3[12]="and_pattern";static char _tmp3F4[17]="equality_pattern";static char _tmp3F5[19]="relational_pattern";static char _tmp3F6[14]="shift_pattern";static char _tmp3F7[17]="additive_pattern";static char _tmp3F8[23]="multiplicative_pattern";static char _tmp3F9[13]="cast_pattern";static char _tmp3FA[14]="unary_pattern";static char _tmp3FB[16]="postfix_pattern";static char _tmp3FC[16]="primary_pattern";static char _tmp3FD[8]="pattern";static char _tmp3FE[19]="tuple_pattern_list";static char _tmp3FF[20]="tuple_pattern_list0";static char _tmp400[14]="field_pattern";static char _tmp401[19]="field_pattern_list";static char _tmp402[20]="field_pattern_list0";static char _tmp403[11]="expression";static char _tmp404[22]="assignment_expression";static char _tmp405[20]="assignment_operator";static char _tmp406[23]="conditional_expression";static char _tmp407[20]="constant_expression";static char _tmp408[22]="logical_or_expression";static char _tmp409[23]="logical_and_expression";static char _tmp40A[24]="inclusive_or_expression";static char _tmp40B[24]="exclusive_or_expression";static char _tmp40C[15]="and_expression";static char _tmp40D[20]="equality_expression";static char _tmp40E[22]="relational_expression";static char _tmp40F[17]="shift_expression";static char _tmp410[20]="additive_expression";static char _tmp411[26]="multiplicative_expression";static char _tmp412[16]="cast_expression";static char _tmp413[17]="unary_expression";static char _tmp414[15]="unary_operator";static char _tmp415[19]="postfix_expression";static char _tmp416[19]="primary_expression";static char _tmp417[25]="argument_expression_list";static char _tmp418[26]="argument_expression_list0";static char _tmp419[9]="constant";static char _tmp41A[20]="qual_opt_identifier";static char _tmp41B[17]="qual_opt_typedef";static char _tmp41C[18]="struct_union_name";static char _tmp41D[11]="field_name";static char _tmp41E[12]="right_angle";
# 1550 "parse.y"
static struct _dyneither_ptr Cyc_yytname[289]={{_tmp2FE,_tmp2FE,_tmp2FE + 2},{_tmp2FF,_tmp2FF,_tmp2FF + 6},{_tmp300,_tmp300,_tmp300 + 12},{_tmp301,_tmp301,_tmp301 + 5},{_tmp302,_tmp302,_tmp302 + 9},{_tmp303,_tmp303,_tmp303 + 7},{_tmp304,_tmp304,_tmp304 + 7},{_tmp305,_tmp305,_tmp305 + 8},{_tmp306,_tmp306,_tmp306 + 5},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 6},{_tmp309,_tmp309,_tmp309 + 4},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,_tmp30B,_tmp30B + 6},{_tmp30C,_tmp30C,_tmp30C + 7},{_tmp30D,_tmp30D,_tmp30D + 7},{_tmp30E,_tmp30E,_tmp30E + 9},{_tmp30F,_tmp30F,_tmp30F + 6},{_tmp310,_tmp310,_tmp310 + 9},{_tmp311,_tmp311,_tmp311 + 9},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,_tmp313,_tmp313 + 6},{_tmp314,_tmp314,_tmp314 + 5},{_tmp315,_tmp315,_tmp315 + 8},{_tmp316,_tmp316,_tmp316 + 7},{_tmp317,_tmp317,_tmp317 + 7},{_tmp318,_tmp318,_tmp318 + 9},{_tmp319,_tmp319,_tmp319 + 3},{_tmp31A,_tmp31A,_tmp31A + 5},{_tmp31B,_tmp31B,_tmp31B + 7},{_tmp31C,_tmp31C,_tmp31C + 6},{_tmp31D,_tmp31D,_tmp31D + 3},{_tmp31E,_tmp31E,_tmp31E + 4},{_tmp31F,_tmp31F,_tmp31F + 5},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,_tmp321 + 6},{_tmp322,_tmp322,_tmp322 + 7},{_tmp323,_tmp323,_tmp323 + 5},{_tmp324,_tmp324,_tmp324 + 8},{_tmp325,_tmp325,_tmp325 + 4},{_tmp326,_tmp326,_tmp326 + 6},{_tmp327,_tmp327,_tmp327 + 4},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 7},{_tmp32A,_tmp32A,_tmp32A + 4},{_tmp32B,_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C,_tmp32C + 9},{_tmp32D,_tmp32D,_tmp32D + 6},{_tmp32E,_tmp32E,_tmp32E + 10},{_tmp32F,_tmp32F,_tmp32F + 9},{_tmp330,_tmp330,_tmp330 + 8},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 8},{_tmp334,_tmp334,_tmp334 + 7},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 5},{_tmp337,_tmp337,_tmp337 + 9},{_tmp338,_tmp338,_tmp338 + 6},{_tmp339,_tmp339,_tmp339 + 7},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 13},{_tmp33D,_tmp33D,_tmp33D + 16},{_tmp33E,_tmp33E,_tmp33E + 14},{_tmp33F,_tmp33F,_tmp33F + 12},{_tmp340,_tmp340,_tmp340 + 7},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 12},{_tmp343,_tmp343,_tmp343 + 6},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 8},{_tmp346,_tmp346,_tmp346 + 10},{_tmp347,_tmp347,_tmp347 + 9},{_tmp348,_tmp348,_tmp348 + 13},{_tmp349,_tmp349,_tmp349 + 10},{_tmp34A,_tmp34A,_tmp34A + 9},{_tmp34B,_tmp34B,_tmp34B + 13},{_tmp34C,_tmp34C,_tmp34C + 14},{_tmp34D,_tmp34D,_tmp34D + 12},{_tmp34E,_tmp34E,_tmp34E + 16},{_tmp34F,_tmp34F,_tmp34F + 15},{_tmp350,_tmp350,_tmp350 + 7},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 9},{_tmp355,_tmp355,_tmp355 + 6},{_tmp356,_tmp356,_tmp356 + 6},{_tmp357,_tmp357,_tmp357 + 6},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 11},{_tmp35C,_tmp35C,_tmp35C + 11},{_tmp35D,_tmp35D,_tmp35D + 11},{_tmp35E,_tmp35E,_tmp35E + 11},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 12},{_tmp361,_tmp361,_tmp361 + 13},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 10},{_tmp365,_tmp365,_tmp365 + 9},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 12},{_tmp368,_tmp368,_tmp368 + 11},{_tmp369,_tmp369,_tmp369 + 17},{_tmp36A,_tmp36A,_tmp36A + 7},{_tmp36B,_tmp36B,_tmp36B + 8},{_tmp36C,_tmp36C,_tmp36C + 19},{_tmp36D,_tmp36D,_tmp36D + 20},{_tmp36E,_tmp36E,_tmp36E + 18},{_tmp36F,_tmp36F,_tmp36F + 9},{_tmp370,_tmp370,_tmp370 + 13},{_tmp371,_tmp371,_tmp371 + 16},{_tmp372,_tmp372,_tmp372 + 18},{_tmp373,_tmp373,_tmp373 + 10},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 5},{_tmp391,_tmp391,_tmp391 + 17},{_tmp392,_tmp392,_tmp392 + 12},{_tmp393,_tmp393,_tmp393 + 19},{_tmp394,_tmp394,_tmp394 + 21},{_tmp395,_tmp395,_tmp395 + 20},{_tmp396,_tmp396,_tmp396 + 21},{_tmp397,_tmp397,_tmp397 + 13},{_tmp398,_tmp398,_tmp398 + 15},{_tmp399,_tmp399,_tmp399 + 17},{_tmp39A,_tmp39A,_tmp39A + 19},{_tmp39B,_tmp39B,_tmp39B + 12},{_tmp39C,_tmp39C,_tmp39C + 19},{_tmp39D,_tmp39D,_tmp39D + 17},{_tmp39E,_tmp39E,_tmp39E + 23},{_tmp39F,_tmp39F,_tmp39F + 24},{_tmp3A0,_tmp3A0,_tmp3A0 + 15},{_tmp3A1,_tmp3A1,_tmp3A1 + 11},{_tmp3A2,_tmp3A2,_tmp3A2 + 15},{_tmp3A3,_tmp3A3,_tmp3A3 + 10},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 25},{_tmp3A6,_tmp3A6,_tmp3A6 + 5},{_tmp3A7,_tmp3A7,_tmp3A7 + 15},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 11},{_tmp3AA,_tmp3AA,_tmp3AA + 22},{_tmp3AB,_tmp3AB,_tmp3AB + 26},{_tmp3AC,_tmp3AC,_tmp3AC + 16},{_tmp3AD,_tmp3AD,_tmp3AD + 16},{_tmp3AE,_tmp3AE,_tmp3AE + 24},{_tmp3AF,_tmp3AF,_tmp3AF + 25},{_tmp3B0,_tmp3B0,_tmp3B0 + 21},{_tmp3B1,_tmp3B1,_tmp3B1 + 22},{_tmp3B2,_tmp3B2,_tmp3B2 + 16},{_tmp3B3,_tmp3B3,_tmp3B3 + 19},{_tmp3B4,_tmp3B4,_tmp3B4 + 25},{_tmp3B5,_tmp3B5,_tmp3B5 + 35},{_tmp3B6,_tmp3B6,_tmp3B6 + 23},{_tmp3B7,_tmp3B7,_tmp3B7 + 24},{_tmp3B8,_tmp3B8,_tmp3B8 + 18},{_tmp3B9,_tmp3B9,_tmp3B9 + 19},{_tmp3BA,_tmp3BA,_tmp3BA + 14},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 20},{_tmp3BD,_tmp3BD,_tmp3BD + 14},{_tmp3BE,_tmp3BE,_tmp3BE + 11},{_tmp3BF,_tmp3BF,_tmp3BF + 23},{_tmp3C0,_tmp3C0,_tmp3C0 + 18},{_tmp3C1,_tmp3C1,_tmp3C1 + 30},{_tmp3C2,_tmp3C2,_tmp3C2 + 8},{_tmp3C3,_tmp3C3,_tmp3C3 + 12},{_tmp3C4,_tmp3C4,_tmp3C4 + 14},{_tmp3C5,_tmp3C5,_tmp3C5 + 13},{_tmp3C6,_tmp3C6,_tmp3C6 + 23},{_tmp3C7,_tmp3C7,_tmp3C7 + 14},{_tmp3C8,_tmp3C8,_tmp3C8 + 18},{_tmp3C9,_tmp3C9,_tmp3C9 + 8},{_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 20},{_tmp3CC,_tmp3CC,_tmp3CC + 9},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 19},{_tmp3CF,_tmp3CF,_tmp3CF + 10},{_tmp3D0,_tmp3D0,_tmp3D0 + 16},{_tmp3D1,_tmp3D1,_tmp3D1 + 11},{_tmp3D2,_tmp3D2,_tmp3D2 + 14},{_tmp3D3,_tmp3D3,_tmp3D3 + 11},{_tmp3D4,_tmp3D4,_tmp3D4 + 15},{_tmp3D5,_tmp3D5,_tmp3D5 + 22},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{_tmp3D7,_tmp3D7,_tmp3D7 + 17},{_tmp3D8,_tmp3D8,_tmp3D8 + 12},{_tmp3D9,_tmp3D9,_tmp3D9 + 18},{_tmp3DA,_tmp3DA,_tmp3DA + 17},{_tmp3DB,_tmp3DB,_tmp3DB + 12},{_tmp3DC,_tmp3DC,_tmp3DC + 16},{_tmp3DD,_tmp3DD,_tmp3DD + 11},{_tmp3DE,_tmp3DE,_tmp3DE + 10},{_tmp3DF,_tmp3DF,_tmp3DF + 14},{_tmp3E0,_tmp3E0,_tmp3E0 + 15},{_tmp3E1,_tmp3E1,_tmp3E1 + 20},{_tmp3E2,_tmp3E2,_tmp3E2 + 27},{_tmp3E3,_tmp3E3,_tmp3E3 + 10},{_tmp3E4,_tmp3E4,_tmp3E4 + 13},{_tmp3E5,_tmp3E5,_tmp3E5 + 18},{_tmp3E6,_tmp3E6,_tmp3E6 + 21},{_tmp3E7,_tmp3E7,_tmp3E7 + 19},{_tmp3E8,_tmp3E8,_tmp3E8 + 16},{_tmp3E9,_tmp3E9,_tmp3E9 + 20},{_tmp3EA,_tmp3EA,_tmp3EA + 15},{_tmp3EB,_tmp3EB,_tmp3EB + 20},{_tmp3EC,_tmp3EC,_tmp3EC + 15},{_tmp3ED,_tmp3ED,_tmp3ED + 12},{_tmp3EE,_tmp3EE,_tmp3EE + 20},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 21},{_tmp3F2,_tmp3F2,_tmp3F2 + 21},{_tmp3F3,_tmp3F3,_tmp3F3 + 12},{_tmp3F4,_tmp3F4,_tmp3F4 + 17},{_tmp3F5,_tmp3F5,_tmp3F5 + 19},{_tmp3F6,_tmp3F6,_tmp3F6 + 14},{_tmp3F7,_tmp3F7,_tmp3F7 + 17},{_tmp3F8,_tmp3F8,_tmp3F8 + 23},{_tmp3F9,_tmp3F9,_tmp3F9 + 13},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 16},{_tmp3FC,_tmp3FC,_tmp3FC + 16},{_tmp3FD,_tmp3FD,_tmp3FD + 8},{_tmp3FE,_tmp3FE,_tmp3FE + 19},{_tmp3FF,_tmp3FF,_tmp3FF + 20},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 19},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 11},{_tmp404,_tmp404,_tmp404 + 22},{_tmp405,_tmp405,_tmp405 + 20},{_tmp406,_tmp406,_tmp406 + 23},{_tmp407,_tmp407,_tmp407 + 20},{_tmp408,_tmp408,_tmp408 + 22},{_tmp409,_tmp409,_tmp409 + 23},{_tmp40A,_tmp40A,_tmp40A + 24},{_tmp40B,_tmp40B,_tmp40B + 24},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,_tmp40E + 22},{_tmp40F,_tmp40F,_tmp40F + 17},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 26},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 15},{_tmp415,_tmp415,_tmp415 + 19},{_tmp416,_tmp416,_tmp416 + 19},{_tmp417,_tmp417,_tmp417 + 25},{_tmp418,_tmp418,_tmp418 + 26},{_tmp419,_tmp419,_tmp419 + 9},{_tmp41A,_tmp41A,_tmp41A + 20},{_tmp41B,_tmp41B,_tmp41B + 17},{_tmp41C,_tmp41C,_tmp41C + 18},{_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,_tmp41E,_tmp41E + 12}};
# 1603
static short Cyc_yyr1[515]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,183,183,183,184,185,185,186,186,186,186,187,187,187,188,188,189,189,189,189,190,190,190,191,191,192,192,193,193,194,194,194,194,194,194,194,194,194,194,195,195,195,195,195,195,195,195,195,195,195,196,196,197,198,198,199,199,199,199,199,199,199,199,200,200,200,201,201,202,202,202,203,203,203,204,204,205,205,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,212,212,212,213,213,214,214,215,215,215,216,217,217,218,218,219,219,219,219,220,220,220,220,221,222,222,223,223,224,224,225,225,225,225,225,226,226,227,227,227,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,229,230,230,231,232,232,233,233,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,280,280,280,280,280,280,281,282,282,283,283,283,283,283,284,284,285,285,286,286,287,287,288,288};
# 1658
static short Cyc_yyr2[515]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1713
static short Cyc_yydefact[1027]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,0,0,0,0,0,0,0,38,505,213,507,506,508,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,158,0,183,186,85,162,111,55,54,48,0,99,504,0,505,500,501,502,503,111,0,373,0,0,0,0,236,0,375,376,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,196,197,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,509,510,111,111,37,39,37,0,20,0,215,0,171,159,184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,505,383,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,489,490,473,0,0,0,0,0,476,474,475,0,400,402,416,424,426,428,430,432,434,437,442,445,448,452,0,454,477,488,487,0,384,382,33,0,0,111,0,0,0,129,125,127,254,256,0,0,9,10,0,0,0,111,511,512,214,94,0,0,163,73,234,0,231,0,3,0,5,0,284,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,0,0,0,0,0,505,287,289,0,295,291,0,293,277,278,279,0,280,281,282,0,40,22,122,261,0,221,237,0,0,217,215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,377,0,0,397,0,0,0,0,250,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,0,0,0,0,0,0,455,456,37,0,0,0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,406,407,408,409,410,411,412,413,414,415,405,0,460,0,483,484,0,0,0,492,0,111,390,391,0,388,0,238,0,0,0,0,257,229,0,130,135,131,133,126,128,215,0,263,255,264,514,513,0,87,91,0,89,37,93,109,64,63,0,61,164,215,233,160,263,235,172,173,0,86,200,27,0,29,0,0,0,0,121,123,240,239,23,106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,324,325,326,0,0,328,0,0,0,0,296,292,122,294,290,288,0,170,222,0,0,0,228,216,223,167,0,0,0,217,169,203,202,165,201,0,0,207,59,112,0,105,423,102,98,386,387,253,0,394,249,251,381,0,0,0,0,241,245,0,0,0,0,0,0,0,0,0,0,0,0,0,491,498,0,497,401,425,0,427,429,431,433,435,436,440,441,438,439,443,444,446,447,449,450,451,404,403,482,479,0,481,0,0,0,378,0,385,32,0,379,0,0,258,136,132,134,0,217,0,201,0,265,0,215,0,276,260,0,36,111,0,0,0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,146,155,154,0,0,149,0,0,0,303,0,0,0,0,0,323,327,0,0,0,0,286,37,24,262,215,0,225,0,0,218,0,168,221,209,166,189,188,207,185,0,7,0,252,396,399,461,0,0,242,0,246,465,0,0,0,0,469,472,0,0,0,0,0,453,494,0,0,480,478,0,0,389,392,380,259,230,272,0,266,267,201,0,0,217,201,0,90,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,115,144,156,153,153,0,0,0,0,0,0,0,0,0,0,0,0,0,303,329,0,0,25,217,0,226,224,0,217,0,208,0,8,0,0,0,243,247,0,0,0,0,421,422,482,481,496,0,499,417,493,495,0,271,269,275,274,0,268,201,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,297,303,0,0,0,0,0,0,331,332,334,336,338,340,342,344,347,352,355,358,362,364,371,372,0,37,300,309,0,0,0,0,0,0,0,0,0,0,330,283,211,227,219,210,215,13,0,14,464,463,0,248,466,0,0,471,470,485,0,273,270,0,0,179,177,285,108,0,0,0,303,0,368,0,0,365,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,366,304,0,311,0,0,0,319,0,0,0,0,0,0,302,0,217,12,15,0,0,0,0,486,107,0,157,298,299,0,0,0,0,303,305,335,0,337,339,341,343,345,346,350,351,348,349,353,354,356,357,359,360,361,0,310,312,313,0,321,320,0,315,0,0,0,220,212,16,0,0,0,0,301,367,0,363,306,0,37,314,322,316,317,0,244,467,0,68,0,0,333,303,307,318,0,370,369,308,468,0,0,0};
# 1819
static short Cyc_yydefgoto[143]={1024,50,685,873,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,646,647,648,68,476,69,449,70,71,158,159,72,117,522,316,682,606,73,607,516,673,508,512,513,420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,293,294,295,759,296,297,834,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,563,564,216,217,75,874,244,435};
# 1837
static short Cyc_yypact[1027]={2663,- -32768,- -32768,- -32768,- -32768,8,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3398,234,3089,- -32768,58,- 45,- -32768,28,51,- 38,90,53,74,95,255,190,- -32768,- -32768,154,- -32768,- -32768,- -32768,138,522,195,206,210,210,- -32768,- -32768,2526,- -32768,239,471,- -32768,286,688,3398,3398,3398,- -32768,3398,- -32768,- -32768,406,- -32768,58,3235,250,- 49,697,710,- -32768,- -32768,244,273,303,- -32768,58,298,- -32768,4601,220,- -32768,- -32768,- -32768,- -32768,244,5988,- -32768,294,328,4601,317,334,327,- -32768,137,- -32768,- -32768,3544,3544,2526,2526,3544,373,5988,583,- -32768,24,384,588,401,24,3763,5988,- -32768,- -32768,- -32768,2526,2799,2526,2799,30,- -32768,414,422,- -32768,3153,- -32768,- -32768,- -32768,- -32768,3763,- -32768,- -32768,244,102,1675,- -32768,3235,688,- -32768,3544,3471,4358,- -32768,250,- -32768,- -32768,- -32768,447,455,- -32768,- -32768,- -32768,- -32768,- 27,710,3544,- -32768,2799,- -32768,441,530,518,58,72,- -32768,4601,112,1002,6115,542,5988,1375,554,591,595,599,602,614,638,643,6164,6164,- -32768,- -32768,- -32768,2242,656,6213,6213,6213,- -32768,- -32768,- -32768,164,- -32768,- -32768,- 43,701,657,663,666,536,222,662,148,260,- -32768,949,6213,189,- 52,- -32768,692,1167,713,- -32768,- -32768,715,5988,244,1167,717,204,3763,1143,3763,341,- -32768,79,79,- -32768,- -32768,54,711,221,244,- -32768,- -32768,- -32768,- -32768,29,724,- -32768,- -32768,623,380,- -32768,735,- -32768,737,- -32768,739,740,759,- -32768,588,4456,3235,- -32768,763,3763,- -32768,675,760,58,765,761,155,764,3868,767,783,776,780,4554,3868,116,779,772,- -32768,- -32768,781,1818,1818,688,1818,- -32768,- -32768,- -32768,788,- -32768,- -32768,- -32768,192,- -32768,- -32768,787,769,106,805,- -32768,11,786,782,311,791,703,789,3544,5988,- -32768,797,- -32768,- -32768,106,799,58,- -32768,5988,800,- -32768,790,798,- -32768,24,5988,4601,322,- -32768,- -32768,- -32768,804,823,2242,- -32768,3763,- -32768,- -32768,3966,- -32768,854,5988,5988,5988,5988,5988,5988,3763,866,2242,- -32768,- -32768,1961,820,382,5988,- -32768,- -32768,- -32768,5988,- -32768,6213,5988,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,5988,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5988,- -32768,24,- -32768,- -32768,4649,24,5988,- -32768,824,244,- -32768,- -32768,822,830,4601,- -32768,374,1002,826,3544,- -32768,833,829,- -32768,1143,1143,1143,- -32768,- -32768,2946,4744,175,- -32768,529,- -32768,- -32768,11,- -32768,- -32768,3544,- -32768,3317,- -32768,836,- -32768,834,835,839,- -32768,2873,- -32768,646,293,- -32768,- -32768,- -32768,3763,- -32768,- -32768,- -32768,2526,- -32768,2526,857,845,840,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,952,5988,847,848,- -32768,837,399,244,244,774,5988,5988,843,851,5988,957,2104,863,- -32768,- -32768,- -32768,453,954,- -32768,4842,5988,3868,2380,- -32768,- -32768,3153,- -32768,- -32768,- -32768,3544,- -32768,- -32768,3763,865,215,- -32768,- -32768,861,- -32768,11,873,3690,782,- -32768,- -32768,- -32768,- -32768,703,78,875,828,- -32768,- -32768,1305,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,879,- -32768,- -32768,- -32768,- -32768,1135,886,892,909,- -32768,- -32768,676,4456,889,895,896,898,434,894,897,240,899,902,6037,- -32768,- -32768,900,903,- -32768,701,211,657,663,666,536,222,222,662,662,662,662,148,148,260,260,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,901,- -32768,- 22,3544,4260,- -32768,2982,- -32768,- -32768,910,- -32768,128,919,- -32768,- -32768,- -32768,- -32768,906,782,908,703,904,529,3544,3617,4937,- -32768,- -32768,79,- -32768,244,742,914,29,3019,926,244,3544,3471,5035,- -32768,646,- -32768,- -32768,- -32768,928,- -32768,930,1082,- -32768,- -32768,675,5988,782,- -32768,- -32768,- -32768,935,58,479,437,443,5988,794,475,931,5133,5228,497,- -32768,- -32768,939,941,934,478,- -32768,3235,- -32768,769,944,3544,- -32768,943,11,- -32768,945,- -32768,80,- -32768,- -32768,- -32768,- -32768,828,- -32768,951,- -32768,2526,- -32768,- -32768,- -32768,953,284,950,- -32768,4064,- -32768,- -32768,5988,1050,5988,1375,- -32768,- -32768,24,24,953,955,4260,- -32768,- -32768,5988,5988,- -32768,- -32768,106,704,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,956,- -32768,- -32768,703,106,958,782,703,946,- -32768,782,482,962,963,- -32768,- -32768,964,- -32768,106,965,782,968,703,947,5988,977,- -32768,- -32768,3763,- -32768,971,22,774,3868,961,972,3718,974,980,3868,5988,5323,573,5418,574,5513,794,- -32768,979,983,- -32768,782,131,- -32768,- -32768,997,782,3763,- -32768,508,- -32768,984,989,5988,- -32768,- -32768,4456,992,994,999,- -32768,854,996,998,- -32768,707,- -32768,- -32768,- -32768,- -32768,4260,- -32768,- -32768,- -32768,- -32768,1000,- -32768,703,3763,969,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1010,- -32768,- -32768,703,486,- -32768,1003,3763,- -32768,- -32768,1056,794,1006,6262,1012,2380,6213,1009,- -32768,- 42,- -32768,1038,1007,700,777,364,785,224,336,- -32768,- -32768,- -32768,- -32768,1049,6213,1961,- -32768,- -32768,505,3868,520,5608,3868,523,5703,5798,593,1020,- -32768,- -32768,- -32768,- -32768,1024,- -32768,944,- -32768,1026,594,- -32768,- -32768,233,- -32768,- -32768,3763,1123,- -32768,- -32768,- -32768,4162,- -32768,- -32768,1028,1043,- -32768,- -32768,- -32768,- -32768,540,3868,1045,794,2242,- -32768,3763,1042,- -32768,1532,6213,5988,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,6213,5988,- -32768,- -32768,1051,- -32768,3868,3868,544,- -32768,3868,3868,550,3868,552,5893,- -32768,11,782,- -32768,- -32768,583,5988,1044,1048,- -32768,- -32768,1069,- -32768,- -32768,- -32768,1057,1052,1055,6213,794,- -32768,701,258,657,663,663,536,222,222,662,662,662,662,148,148,260,260,- -32768,- -32768,- -32768,369,- -32768,- -32768,- -32768,3868,- -32768,- -32768,3868,- -32768,3868,3868,556,- -32768,- -32768,- -32768,755,1054,3763,1058,- -32768,953,446,- -32768,- -32768,5988,1532,- -32768,- -32768,- -32768,- -32768,3868,- -32768,- -32768,1060,- -32768,1064,1066,- -32768,794,- -32768,- -32768,1067,- -32768,- -32768,- -32768,- -32768,1183,1184,- -32768};
# 1943
static short Cyc_yypgoto[143]={- -32768,176,- -32768,259,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 114,16,- -32768,- -32768,0,581,- -32768,91,- 175,1090,5,- -32768,- -32768,- 136,- -32768,181,1171,- 694,- -32768,- -32768,- -32768,959,942,366,431,- -32768,- -32768,569,- -32768,- -32768,126,- -32768,- -32768,75,- 211,1140,- 379,- 30,- -32768,1053,- -32768,- -32768,1165,- 416,- -32768,533,- 144,- 140,- 129,227,281,545,553,- 423,- 468,- 113,- 418,- 132,- -32768,- 247,- 153,- 545,- 337,- -32768,905,- 180,149,- 23,- 47,- 333,- 91,- -32768,- -32768,- -32768,- 59,- 268,- -32768,- 620,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,20,1004,- -32768,682,812,- -32768,245,637,- -32768,- 167,- 291,- 141,- 359,- 356,- 366,858,- 354,- 343,- 206,- 299,- 275,- 160,558,474,880,- -32768,- 377,- -32768,159,415,- 61,- 20,- 289,- 37};
# 1965
static short Cyc_yytable[6408]={59,307,250,569,136,62,549,566,341,143,358,616,568,310,465,262,308,571,317,499,500,59,502,344,140,589,62,644,645,572,573,324,361,362,363,345,631,78,448,537,536,149,671,96,112,137,442,136,714,366,904,59,405,398,423,823,62,38,59,59,59,100,59,62,62,62,41,62,59,147,263,630,509,62,130,131,132,44,133,578,579,103,300,406,142,140,39,419,240,559,668,60,674,367,905,288,611,299,630,631,364,315,169,59,59,580,581,678,62,62,587,712,60,220,590,888,76,611,662,113,59,59,59,59,39,62,62,62,62,59,241,510,129,719,62,443,257,319,242,59,432,59,60,- 150,62,142,62,864,147,60,60,60,101,60,258,289,531,142,543,60,544,796,59,531,38,432,511,62,574,575,576,577,556,41,558,102,437,105,170,433,518,638,97,486,430,530,507,246,453,493,325,434,432,723,60,60,171,436,106,306,438,429,451,468,433,679,453,- 215,896,104,- 215,60,60,60,60,434,434,301,299,450,60,107,582,583,584,326,160,119,505,422,60,433,60,269,424,494,624,477,409,109,327,97,495,288,288,409,288,423,423,423,233,234,97,60,237,718,549,224,868,110,624,38,59,225,111,434,506,62,434,172,41,399,400,401,756,19,20,953,142,235,236,482,528,958,483,380,364,59,59,381,59,365,62,62,511,62,253,254,255,256,114,427,694,289,289,531,289,428,803,374,375,504,807,288,364,402,527,39,268,270,403,404,702,400,401,418,170,819,39,97,116,97,115,364,198,301,670,1001,320,- 237,38,710,- 237,364,171,124,376,377,440,41,748,538,239,60,79,364,787,59,120,121,871,918,62,944,501,919,402,43,160,549,419,703,404,289,144,511,145,97,60,60,364,60,146,1017,97,250,625,665,1002,453,241,782,887,162,382,618,1022,429,242,38,707,783,383,384,891,415,664,40,41,42,453,163,737,793,794,74,453,450,167,447,218,441,422,422,422,428,737,424,424,424,306,615,596,517,219,359,221,80,98,59,99,477,666,656,62,539,786,623,60,629,223,912,913,329,330,142,222,140,59,74,59,419,524,62,787,62,74,427,414,726,920,637,74,428,531,47,48,49,921,922,135,249,138,740,727,74,238,59,914,915,97,364,62,597,166,742,364,98,741,1003,267,59,454,142,364,38,62,455,98,561,245,249,38,40,41,42,674,142,74,74,40,41,42,135,492,471,134,74,247,59,60,681,511,259,62,74,74,74,74,772,878,962,963,798,260,959,791,777,787,60,961,60,241,1013,786,964,364,74,792,364,242,699,625,321,754,364,600,965,966,713,755,97,659,313,97,364,453,74,60,453,730,314,166,359,925,98,617,98,593,725,122,123,60,477,421,554,426,364,752,359,364,753,760,739,810,770,771,477,364,811,477,615,567,892,38,716,766,971,972,364,60,299,40,41,42,372,373,364,38,872,629,267,926,98,632,957,633,41,786,43,98,323,364,973,974,364,43,928,44,591,932,322,901,612,667,613,47,48,49,642,643,614,454,827,452,59,364,950,340,854,62,982,364,902,364,74,799,985,364,987,346,142,452,1008,479,59,681,484,804,38,62,924,858,861,38,364,364,40,41,42,945,97,815,41,74,684,967,968,969,970,1014,894,937,942,44,364,943,347,954,136,955,348,47,48,49,349,649,650,350,38,653,339,657,342,342,1016,166,40,41,42,351,663,940,355,356,677,98,378,379,447,342,342,342,252,97,428,60,47,48,49,975,976,977,43,352,927,520,521,931,353,626,342,627,150,151,152,60,850,628,644,645,38,360,312,153,154,155,156,157,40,41,42,368,249,38,369,1000,692,693,511,731,471,370,41,951,472,371,125,288,47,48,49,407,249,44,1011,757,758,773,412,249,413,47,48,49,800,801,98,884,885,98,47,48,49,721,1015,439,980,981,908,909,983,984,417,986,16,17,18,732,733,734,446,59,603,604,605,456,62,457,288,459,781,452,461,249,452,910,911,289,749,916,917,463,74,249,74,1009,364,825,826,478,342,136,249,469,480,452,481,342,488,485,1004,452,487,1005,489,1006,1007,467,490,763,765,497,59,81,496,434,498,62,503,261,507,515,514,43,534,97,1018,519,289,529,533,523,535,342,541,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,60,74,542,366,560,592,594,602,525,595,599,806,601,619,98,809,288,620,622,621,634,635,636,639,641,817,651,640,652,38,84,187,188,85,86,87,249,467,41,658,550,551,552,553,654,555,421,821,669,354,249,60,191,660,562,672,867,675,565,680,59,870,385,855,857,62,860,98,863,686,689,690,691,695,696,697,289,698,700,585,705,701,709,704,708,711,877,342,717,39,720,586,722,746,724,452,562,81,735,386,387,388,389,390,391,392,393,394,395,452,738,745,452,750,761,38,767,768,769,751,306,774,610,40,41,42,43,776,780,396,706,784,789,889,198,471,808,820,828,795,802,895,805,47,48,49,812,813,814,816,82,60,818,822,824,865,829,74,853,866,930,852,328,934,936,168,84,869,875,85,86,87,267,876,41,342,879,880,881,88,882,893,883,897,886,89,906,90,91,562,329,330,562,92,890,900,903,923,938,93,359,249,907,939,941,946,948,960,7,8,9,10,11,12,13,14,15,16,17,18,19,20,949,952,991,978,956,979,994,98,81,995,267,996,999,997,998,22,1010,989,1025,1026,1012,467,1019,38,993,249,1020,27,1021,1023,135,40,41,42,342,28,29,992,736,248,81,108,470,471,747,148,32,318,118,779,34,47,48,49,464,990,778,35,36,688,775,82,598,570,416,467,851,0,557,0,0,0,540,687,0,0,168,84,0,0,85,86,87,0,0,41,729,0,0,0,88,39,342,82,0,43,89,0,90,91,744,329,330,342,92,408,45,46,168,84,93,0,85,86,87,0,0,41,0,0,0,0,88,562,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,- 11,1,93,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,467,0,0,788,0,790,0,0,0,0,0,0,22,467,23,0,797,0,683,0,24,0,25,26,27,0,0,0,135,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,899,0,0,342,0,0,0,0,0,0,0,0,173,174,0,0,0,0,0,0,0,342,0,38,0,81,0,0,0,0,39,40,41,42,43,0,467,0,- 11,177,178,179,180,44,0,45,46,0,0,0,467,47,48,49,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,0,0,0,190,0,0,191,0,0,0,0,192,0,0,193,0,342,194,195,0,0,196,197,0,467,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,757,758,21,173,174,271,342,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 303,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,0,45,46,28,29,428,0,47,48,49,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,715,0,32,168,84,0,34,85,86,87,0,0,41,35,36,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,0,0,38,0,81,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,830,831,45,46,28,29,0,0,0,0,0,0,0,81,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,82,0,0,27,0,0,0,0,0,45,46,28,29,0,0,168,84,0,0,85,86,87,32,0,41,0,34,0,0,88,0,0,0,35,36,832,0,90,91,0,0,0,0,92,0,0,833,0,0,93,195,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,39,40,0,42,43,0,0,0,0,0,0,0,0,0,0,45,46,173,174,271,0,272,273,274,275,276,277,278,279,0,81,0,175,280,0,0,176,0,281,0,0,0,0,0,177,178,179,180,0,0,0,0,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,545,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,546,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,785,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,947,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,82,176,194,195,0,0,196,197,0,177,178,179,180,0,0,168,84,181,0,85,86,87,0,0,41,0,0,182,183,88,184,0,0,0,0,89,0,90,91,0,185,186,0,92,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,743,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,762,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,764,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,856,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,859,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,862,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,929,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,933,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,935,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,988,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,898,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197};
# 2609
static short Cyc_yycheck[6408]={0,145,115,369,65,0,343,366,175,68,190,434,368,145,261,129,145,371,158,287,288,21,290,176,68,402,21,5,6,372,373,167,192,193,194,176,454,21,249,330,329,71,510,23,44,65,17,108,593,92,92,51,104,213,229,749,51,106,58,59,60,106,62,58,59,60,115,62,68,69,129,450,61,68,58,59,60,126,62,378,379,119,141,135,68,129,113,227,108,357,508,0,515,136,136,139,429,141,477,517,122,128,82,103,104,380,381,523,103,104,399,133,21,93,403,809,108,450,495,44,120,121,122,123,113,120,121,122,123,129,106,120,57,601,129,106,106,160,114,139,86,141,51,121,139,129,141,767,148,58,59,60,124,62,124,139,323,141,338,68,340,706,162,330,106,86,306,162,374,375,376,377,352,115,354,124,122,124,106,125,309,472,23,274,231,321,106,112,249,280,170,137,86,609,103,104,124,234,124,119,237,231,249,262,125,127,267,127,828,119,130,120,121,122,123,137,137,142,262,249,129,126,382,383,384,113,124,51,122,229,139,125,141,131,229,119,447,267,218,49,128,82,126,287,288,225,290,422,423,424,101,102,93,162,105,127,593,120,127,105,471,106,262,126,126,137,303,262,137,88,115,82,83,84,651,20,21,897,262,103,104,126,319,903,129,137,122,287,288,141,290,127,287,288,434,290,120,121,122,123,105,126,549,287,288,472,290,132,724,87,88,119,728,357,122,126,316,113,137,138,131,132,82,83,84,121,106,743,113,170,120,172,126,122,89,260,121,957,162,119,106,130,122,122,124,59,124,125,127,115,641,331,107,262,120,122,693,357,119,120,778,137,357,130,289,141,126,117,124,706,510,131,132,357,124,515,126,218,287,288,122,290,132,1003,225,498,447,501,130,450,106,107,808,120,134,439,1016,427,114,106,560,690,142,143,820,224,497,114,115,116,471,108,623,702,703,0,477,447,120,126,126,240,422,423,424,132,637,422,423,424,119,431,412,122,106,190,119,22,23,439,25,471,501,487,439,123,693,447,357,449,123,87,88,131,132,439,122,501,458,44,460,601,313,458,801,460,51,126,223,613,134,471,57,132,641,134,135,136,142,143,65,115,67,627,613,70,113,487,124,125,331,122,487,119,79,627,122,82,627,130,134,501,122,487,122,106,501,127,93,127,126,145,106,114,115,116,939,501,103,104,114,115,116,108,279,126,120,112,127,529,439,526,672,119,529,120,121,122,123,668,787,907,908,710,122,904,699,676,885,458,906,460,106,107,801,909,122,142,699,122,114,127,623,122,127,122,417,910,911,592,127,412,119,126,415,122,637,162,487,640,617,126,167,338,852,170,437,172,407,612,119,120,501,623,228,350,230,122,119,354,122,122,127,626,122,127,665,637,122,127,640,611,367,127,106,595,119,916,917,122,529,665,114,115,116,89,90,122,106,121,630,265,127,218,458,903,460,115,885,117,225,123,122,918,919,122,117,127,126,404,127,121,832,124,505,126,134,135,136,478,479,132,122,754,249,665,122,127,126,760,665,127,122,833,122,260,713,127,122,127,126,665,267,127,269,685,681,272,725,106,685,851,119,119,106,122,122,114,115,116,880,542,739,115,289,529,912,913,914,915,999,824,119,119,126,122,122,126,898,780,900,126,134,135,136,126,481,482,126,106,485,173,487,175,176,1003,321,114,115,116,126,496,871,185,186,518,331,85,86,126,192,193,194,116,595,132,665,134,135,136,920,921,922,117,126,856,63,64,859,126,124,213,126,63,64,65,685,757,132,5,6,106,126,146,74,75,76,77,78,114,115,116,91,427,106,138,956,121,122,939,619,126,139,115,895,130,140,119,852,134,135,136,120,447,126,995,22,23,669,106,454,106,134,135,136,121,122,412,121,122,415,134,135,136,607,1002,125,928,929,139,140,932,933,126,935,17,18,19,106,107,108,127,852,422,423,424,121,852,121,903,121,685,447,123,498,450,89,90,852,642,85,86,113,458,508,460,121,122,752,753,120,323,943,517,121,120,471,126,330,106,126,982,477,126,985,119,987,988,261,119,655,656,130,903,38,126,137,126,903,121,123,106,130,127,117,125,757,1008,127,903,121,121,133,125,366,121,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,852,529,122,92,127,124,127,121,314,122,127,727,122,120,542,731,1003,126,122,127,106,119,125,119,130,741,126,122,120,106,107,108,109,110,111,112,613,343,115,119,346,347,348,349,30,351,623,745,126,126,627,903,129,42,360,137,772,127,364,127,1003,777,56,761,762,1003,764,595,766,133,127,122,106,127,122,122,1003,122,127,385,121,127,122,127,127,127,784,472,121,113,127,397,127,106,133,623,402,38,127,93,94,95,96,97,98,99,100,101,102,637,127,126,640,121,126,106,120,119,127,647,119,121,428,114,115,116,117,125,120,123,120,124,25,107,832,126,133,133,120,127,127,28,127,134,135,136,127,127,127,127,91,1003,127,119,126,119,127,685,121,119,858,130,103,861,862,106,107,113,127,110,111,112,749,127,115,560,127,126,122,120,127,121,127,120,127,126,91,128,129,495,131,132,498,134,127,126,130,91,121,140,898,778,138,122,121,25,121,905,8,9,10,11,12,13,14,15,16,17,18,19,20,21,122,121,940,923,127,119,127,757,38,126,809,107,122,121,127,37,127,937,0,0,127,549,127,106,944,824,127,49,127,127,780,114,115,116,641,57,58,943,622,114,38,35,265,126,640,70,68,159,48,681,72,134,135,136,260,939,676,79,80,542,672,91,415,370,225,593,757,- 1,353,- 1,- 1,- 1,332,103,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,614,- 1,- 1,- 1,120,113,699,91,- 1,117,126,- 1,128,129,628,131,132,710,134,103,128,129,106,107,140,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,651,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,0,1,140,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,693,- 1,- 1,696,- 1,698,- 1,- 1,- 1,- 1,- 1,- 1,37,706,39,- 1,709,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,943,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,830,- 1,- 1,833,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,851,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,787,- 1,121,52,53,54,55,126,- 1,128,129,- 1,- 1,- 1,801,134,135,136,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,904,- 1,906,907,908,909,910,911,912,913,914,915,916,917,918,919,920,921,922,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,956,140,141,- 1,- 1,144,145,- 1,885,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1002,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,- 1,72,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,- 1,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,25,26,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,91,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,106,107,- 1,- 1,110,111,112,68,- 1,115,- 1,72,- 1,- 1,120,- 1,- 1,- 1,79,80,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,129,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,32,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,91,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,106,107,60,- 1,110,111,112,- 1,- 1,115,- 1,- 1,70,71,120,73,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,83,84,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple25{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple26{struct _dyneither_ptr f1;void*f2;};static char _tmp4D8[8]="stdcall";static char _tmp4D9[6]="cdecl";static char _tmp4DA[9]="fastcall";static char _tmp4DB[9]="noreturn";static char _tmp4DC[6]="const";static char _tmp4DD[8]="aligned";static char _tmp4DE[7]="packed";static char _tmp4DF[7]="shared";static char _tmp4E0[7]="unused";static char _tmp4E1[5]="weak";static char _tmp4E2[10]="dllimport";static char _tmp4E3[10]="dllexport";static char _tmp4E4[23]="no_instrument_function";static char _tmp4E5[12]="constructor";static char _tmp4E6[11]="destructor";static char _tmp4E7[22]="no_check_memory_usage";static char _tmp4E8[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp993(union Cyc_YYSTYPE*yylval,unsigned int*_tmp992,unsigned int*_tmp991,union Cyc_YYSTYPE**_tmp990){for(*_tmp992=0;*_tmp992 < *_tmp991;(*_tmp992)++){(*_tmp990)[*_tmp992]=*yylval;}}static void _tmp998(unsigned int*_tmp997,unsigned int*_tmp996,struct Cyc_Yyltype**_tmp995){for(*_tmp997=0;*_tmp997 < *_tmp996;(*_tmp997)++){(*_tmp995)[*_tmp997]=
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp99F(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp99E,unsigned int*_tmp99D,short**_tmp99B){for(*_tmp99E=0;*_tmp99E < *_tmp99D;(*_tmp99E)++){(*_tmp99B)[*_tmp99E]=
# 216
*_tmp99E <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp99E)): 0;}}static void _tmp9A5(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9A4,unsigned int*_tmp9A3,union Cyc_YYSTYPE**_tmp9A1){for(*_tmp9A4=0;*_tmp9A4 < *_tmp9A3;(*_tmp9A4)++){(*_tmp9A1)[*_tmp9A4]=
# 219
*_tmp9A4 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9A4)):*yylval;}}static void _tmp9AB(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9AA,unsigned int*_tmp9A9,struct Cyc_Yyltype**_tmp9A7){for(*_tmp9AA=0;*_tmp9AA < *_tmp9A9;(*_tmp9AA)++){(*_tmp9A7)[*_tmp9AA]=
# 222
*_tmp9AA <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9AA)):
 Cyc_yynewloc();}}static void _tmpE77(unsigned int*_tmpE76,unsigned int*_tmpE75,char**_tmpE73){for(*_tmpE76=0;*_tmpE76 < *_tmpE75;(*_tmpE76)++){(*_tmpE73)[*_tmpE76]='\000';}}
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
union Cyc_YYSTYPE _tmp98C;union Cyc_YYSTYPE yylval=((_tmp98C.YYINITIALSVAL).val=0,(((_tmp98C.YYINITIALSVAL).tag=61,_tmp98C)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp98E;unsigned int _tmp98D;struct _dyneither_ptr yyss=(_tmp98D=200,((_tmp98E=_region_calloc(yyregion,sizeof(short),_tmp98D),_tag_dyneither(_tmp98E,sizeof(short),_tmp98D))));
# 158
int yyvsp_offset;
unsigned int _tmp992;unsigned int _tmp991;union Cyc_YYSTYPE*_tmp990;unsigned int _tmp98F;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp98F=(unsigned int)200,((_tmp990=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp98F)),((((_tmp991=_tmp98F,_tmp993(& yylval,& _tmp992,& _tmp991,& _tmp990))),_tmp990)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp997;unsigned int _tmp996;struct Cyc_Yyltype*_tmp995;unsigned int _tmp994;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp994=(unsigned int)200,((_tmp995=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp994)),((((_tmp996=_tmp994,_tmp998(& _tmp997,& _tmp996,& _tmp995))),_tmp995)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp999;Cyc_yyerror(((_tmp999="parser stack overflow",_tag_dyneither(_tmp999,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp99E;unsigned int _tmp99D;struct _dyneither_ptr _tmp99C;short*_tmp99B;unsigned int _tmp99A;struct _dyneither_ptr yyss1=(_tmp99A=(unsigned int)yystacksize,((_tmp99B=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp99A)),((_tmp99C=_tag_dyneither(_tmp99B,sizeof(short),_tmp99A),((((_tmp99D=_tmp99A,_tmp99F(& yyssp_offset,& yyss,& _tmp99E,& _tmp99D,& _tmp99B))),_tmp99C)))))));
# 217
unsigned int _tmp9A4;unsigned int _tmp9A3;struct _dyneither_ptr _tmp9A2;union Cyc_YYSTYPE*_tmp9A1;unsigned int _tmp9A0;struct _dyneither_ptr yyvs1=
(_tmp9A0=(unsigned int)yystacksize,((_tmp9A1=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A0)),((_tmp9A2=_tag_dyneither(_tmp9A1,sizeof(union Cyc_YYSTYPE),_tmp9A0),((((_tmp9A3=_tmp9A0,_tmp9A5(& yyvs,& yyssp_offset,& yylval,& _tmp9A4,& _tmp9A3,& _tmp9A1))),_tmp9A2)))))));
# 221
unsigned int _tmp9AA;unsigned int _tmp9A9;struct _dyneither_ptr _tmp9A8;struct Cyc_Yyltype*_tmp9A7;unsigned int _tmp9A6;struct _dyneither_ptr yyls1=(_tmp9A6=(unsigned int)yystacksize,((_tmp9A7=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9A6)),((_tmp9A8=_tag_dyneither(_tmp9A7,sizeof(struct Cyc_Yyltype),_tmp9A6),((((_tmp9A9=_tmp9A6,_tmp9AB(& yyssp_offset,& yyls,& _tmp9AA,& _tmp9A9,& _tmp9A7))),_tmp9A8)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,yystate)];
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
if((yyn < 0  || yyn > 6407) || Cyc_yycheck[_check_known_subscript_notnull(6408,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6408,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1026){
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1027,yystate)];
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
{struct Cyc_Absyn_Decl*_tmp9B5;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9B4;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9B3;struct Cyc_List_List*_tmp9B2;yyval=Cyc_YY18(((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2->hd=((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5->r=(void*)((_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=11,((_tmp9B4.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9B4.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9B4)))))),_tmp9B3)))),((_tmp9B5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9B5)))))),((_tmp9B2->tl=0,_tmp9B2)))))));}
Cyc_Lex_leave_using();
# 1201
break;}case 4: _LL212: {
# 1198 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1202
{struct Cyc_Absyn_Decl*_tmp9BF;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9BE;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9BD;struct Cyc_List_List*_tmp9BC;yyval=Cyc_YY18(((_tmp9BC=_cycalloc(sizeof(*_tmp9BC)),((_tmp9BC->hd=((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->r=(void*)((_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BE.tag=11,((_tmp9BE.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9BE.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9BE)))))),_tmp9BD)))),((_tmp9BF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9BF)))))),((_tmp9BC->tl=Cyc_yyget_YY18(yyyyvsp[4]),_tmp9BC)))))));}
break;}case 5: _LL213: {
# 1201 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1205
{struct Cyc_Absyn_Decl*_tmp9CE;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9CD;struct _dyneither_ptr*_tmp9CC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9CB;struct Cyc_List_List*_tmp9CA;yyval=Cyc_YY18(((_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA->hd=((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE->r=(void*)((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CD.tag=10,((_tmp9CD.f1=((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9CC)))),((_tmp9CD.f2=Cyc_yyget_YY18(yyyyvsp[2]),_tmp9CD)))))),_tmp9CB)))),((_tmp9CE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9CE)))))),((_tmp9CA->tl=0,_tmp9CA)))))));}
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
{const char*_tmp9CF;nspace=((_tmp9CF="",_tag_dyneither(_tmp9CF,sizeof(char),1)));}goto _LL216;_LL219: _tmp450=_tmp44E;_LL21A:(void)_throw(_tmp450);_LL216:;}};}
# 1215
{struct _handler_cons _tmp452;_push_handler(& _tmp452);{int _tmp454=0;if(setjmp(_tmp452.handler))_tmp454=1;if(!_tmp454){x=Cyc_yyget_YY18(yyyyvsp[2]);;_pop_handler();}else{void*_tmp453=(void*)_exn_thrown;void*_tmp456=_tmp453;void*_tmp458;_LL21C: {struct Cyc_Core_Failure_exn_struct*_tmp457=(struct Cyc_Core_Failure_exn_struct*)_tmp456;if(_tmp457->tag != Cyc_Core_Failure)goto _LL21E;}_LL21D:
 x=0;goto _LL21B;_LL21E: _tmp458=_tmp456;_LL21F:(void)_throw(_tmp458);_LL21B:;}};}
# 1218
{struct _handler_cons _tmp459;_push_handler(& _tmp459);{int _tmp45B=0;if(setjmp(_tmp459.handler))_tmp45B=1;if(!_tmp45B){y=Cyc_yyget_YY18(yyyyvsp[4]);;_pop_handler();}else{void*_tmp45A=(void*)_exn_thrown;void*_tmp45D=_tmp45A;void*_tmp45F;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp45E=(struct Cyc_Core_Failure_exn_struct*)_tmp45D;if(_tmp45E->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
 y=0;goto _LL220;_LL223: _tmp45F=_tmp45D;_LL224:(void)_throw(_tmp45F);_LL220:;}};}
# 1221
{struct Cyc_Absyn_Decl*_tmp9DE;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9DD;struct _dyneither_ptr*_tmp9DC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9DB;struct Cyc_List_List*_tmp9DA;yyval=Cyc_YY18(((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->hd=((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE->r=(void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DD.tag=10,((_tmp9DD.f1=((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=nspace,_tmp9DC)))),((_tmp9DD.f2=x,_tmp9DD)))))),_tmp9DB)))),((_tmp9DE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9DE)))))),((_tmp9DA->tl=y,_tmp9DA)))))));}
# 1223
break;}case 7: _LL215: {
# 1220 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1224
struct _dyneither_ptr _tmp465=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9DF;if(Cyc_strcmp((struct _dyneither_ptr)_tmp465,((_tmp9DF="C",_tag_dyneither(_tmp9DF,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmp9E9;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp9E8;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9E7;struct Cyc_List_List*_tmp9E6;yyval=Cyc_YY18(((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6->hd=((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9->r=(void*)((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=12,((_tmp9E8.f1=Cyc_yyget_YY18(yyyyvsp[3]),_tmp9E8)))),_tmp9E7)))),((_tmp9E9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E9)))))),((_tmp9E6->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmp9E6)))))));}else{
# 1228
{const char*_tmp9EA;if(Cyc_strcmp((struct _dyneither_ptr)_tmp465,((_tmp9EA="C include",_tag_dyneither(_tmp9EA,sizeof(char),10))))!= 0){
const char*_tmp9EB;Cyc_Parse_err(((_tmp9EB="expecting \"C\" or \"C include\"",_tag_dyneither(_tmp9EB,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1231
struct Cyc_Absyn_Decl*_tmp9F5;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp9F4;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9F3;struct Cyc_List_List*_tmp9F2;yyval=Cyc_YY18(((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2->hd=((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5->r=(void*)((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F4.tag=13,((_tmp9F4.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmp9F4.f2=0,_tmp9F4)))))),_tmp9F3)))),((_tmp9F5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F5)))))),((_tmp9F2->tl=Cyc_yyget_YY18(yyyyvsp[5]),_tmp9F2)))))));};}}
# 1234
break;}case 8: _LL225: {
# 1231 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1235
{const char*_tmp9F6;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmp9F6="C include",_tag_dyneither(_tmp9F6,sizeof(char),10))))!= 0){
const char*_tmp9F7;Cyc_Parse_err(((_tmp9F7="expecting \"C include\"",_tag_dyneither(_tmp9F7,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA01;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA00;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9FF;struct Cyc_List_List*_tmp9FE;yyval=Cyc_YY18(((_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->hd=((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01->r=(void*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=13,((_tmpA00.f1=Cyc_yyget_YY18(yyyyvsp[3]),((_tmpA00.f2=exs,_tmpA00)))))),_tmp9FF)))),((_tmpA01->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA01)))))),((_tmp9FE->tl=Cyc_yyget_YY18(yyyyvsp[6]),_tmp9FE)))))));}
# 1240
break;};}case 9: _LL226: {
# 1237 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1241
{struct Cyc_Absyn_Decl*_tmpA04;struct Cyc_List_List*_tmpA03;yyval=Cyc_YY18(((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->hd=((_tmpA04=_cycalloc(sizeof(*_tmpA04)),((_tmpA04->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA04->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA04)))))),((_tmpA03->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA03)))))));}
break;}case 10: _LL227: {
# 1239 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1243
{struct Cyc_Absyn_Decl*_tmpA07;struct Cyc_List_List*_tmpA06;yyval=Cyc_YY18(((_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->hd=((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA07->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA07)))))),((_tmpA06->tl=Cyc_yyget_YY18(yyyyvsp[2]),_tmpA06)))))));}
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
{struct _tuple25*_tmpA0A;struct Cyc_List_List*_tmpA09;yyval=Cyc_YY52(((_tmpA09=_cycalloc(sizeof(*_tmpA09)),((_tmpA09->hd=((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA0A->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0A->f3=0,_tmpA0A)))))))),((_tmpA09->tl=0,_tmpA09)))))));}
break;}case 15: _LL22C: {
# 1250 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1254
{struct _tuple25*_tmpA0D;struct Cyc_List_List*_tmpA0C;yyval=Cyc_YY52(((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->hd=((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA0D->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0D->f3=0,_tmpA0D)))))))),((_tmpA0C->tl=0,_tmpA0C)))))));}
break;}case 16: _LL22D: {
# 1252 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1256
{struct _tuple25*_tmpA10;struct Cyc_List_List*_tmpA0F;yyval=Cyc_YY52(((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F->hd=((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA10->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA10->f3=0,_tmpA10)))))))),((_tmpA0F->tl=Cyc_yyget_YY52(yyyyvsp[2]),_tmpA0F)))))));}
break;}case 17: _LL22E: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1260
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA16;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA15;struct Cyc_List_List*_tmpA14;yyval=Cyc_YY18(((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->hd=Cyc_Absyn_new_decl((void*)((_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA15.tag=1,((_tmpA15.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpA15)))),_tmpA16)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA14->tl=0,_tmpA14)))))));}
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
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,& _tmp484,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
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
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,& _tmp485,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL235: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1292
struct Cyc_Parse_Declaration_spec _tmp486=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,& _tmp486,Cyc_yyget_YY29(yyyyvsp[1]),0,Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL236: {
# 1291 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1295
struct Cyc_Parse_Declaration_spec _tmp487=Cyc_yyget_YY19(yyyyvsp[0]);
yyval=Cyc_YY17(Cyc_Parse_make_function(yyr,& _tmp487,Cyc_yyget_YY29(yyyyvsp[1]),Cyc_yyget_YY18(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
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
{struct _dyneither_ptr*_tmpA17;Cyc_Lex_enter_namespace(((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA17)))));}yyval=yyyyvsp[1];
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
{struct Cyc_List_List*_tmpA18;yyval=Cyc_YY18(((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA18->tl=0,_tmpA18)))))));}
break;}case 33: _LL23E: {
# 1319 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323
struct Cyc_List_List*_tmp48B=0;
{struct Cyc_List_List*_tmp48C=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp48C != 0;_tmp48C=_tmp48C->tl){
struct _dyneither_ptr*_tmp48D=(struct _dyneither_ptr*)_tmp48C->hd;
struct _tuple0*_tmpA19;struct _tuple0*qv=(_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19->f1=Cyc_Absyn_Rel_n(0),((_tmpA19->f2=_tmp48D,_tmpA19)))));
struct Cyc_Absyn_Vardecl*_tmp48E=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA1A;_tmp48B=((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A->hd=_tmp48E,((_tmpA1A->tl=_tmp48B,_tmpA1A))))));}}
# 1330
_tmp48B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48B);
{struct Cyc_List_List*_tmpA1B;yyval=Cyc_YY18(((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->hd=Cyc_Absyn_letv_decl(_tmp48B,(unsigned int)(yyyylsp[0]).first_line),((_tmpA1B->tl=0,_tmpA1B)))))));}
# 1333
break;}case 34: _LL23F: {
# 1332 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336
struct _dyneither_ptr _tmp492=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA1C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,((_tmpA1C="`H",_tag_dyneither(_tmpA1C,sizeof(char),3))))== 0){
const char*_tmpA1F;void*_tmpA1E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA1E=0,Cyc_aprintf(((_tmpA1F="bad occurrence of heap region",_tag_dyneither(_tmpA1F,sizeof(char),30))),_tag_dyneither(_tmpA1E,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA20;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp492,((_tmpA20="`U",_tag_dyneither(_tmpA20,sizeof(char),3))))== 0){
const char*_tmpA23;void*_tmpA22;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA22=0,Cyc_aprintf(((_tmpA23="bad occurrence of unique region",_tag_dyneither(_tmpA23,sizeof(char),32))),_tag_dyneither(_tmpA22,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA26;struct Cyc_Absyn_Tvar*_tmpA25;struct Cyc_Absyn_Tvar*tv=(_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->name=((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26[0]=_tmp492,_tmpA26)))),((_tmpA25->identity=- 1,((_tmpA25->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA25)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA29;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA28;void*t=(void*)((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=2,((_tmpA29.f1=tv,_tmpA29)))),_tmpA28))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA2F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA2E;struct _dyneither_ptr*_tmpA2C;struct _tuple0*_tmpA2B;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->f1=Cyc_Absyn_Loc_n,((_tmpA2B->f2=((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA2C)))),_tmpA2B)))))),(void*)((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=15,((_tmpA2F.f1=t,_tmpA2F)))),_tmpA2E)))),0);
{struct Cyc_List_List*_tmpA30;yyval=Cyc_YY18(((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA30->tl=0,_tmpA30)))))));}
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
{const char*_tmpA31;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA31="H",_tag_dyneither(_tmpA31,sizeof(char),2))))== 0){
const char*_tmpA32;Cyc_Parse_err(((_tmpA32="bad occurrence of heap region `H",_tag_dyneither(_tmpA32,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA33;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA33="U",_tag_dyneither(_tmpA33,sizeof(char),2))))== 0){
const char*_tmpA34;Cyc_Parse_err(((_tmpA34="bad occurrence of unique region `U",_tag_dyneither(_tmpA34,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4A2  && _tmp4A4 != 0){
const char*_tmpA35;Cyc_Parse_err(((_tmpA35="open regions cannot be @resetable",_tag_dyneither(_tmpA35,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA42;const char*_tmpA41;void*_tmpA40[1];struct Cyc_String_pa_PrintArg_struct _tmpA3F;struct Cyc_Absyn_Tvar*_tmpA3E;struct Cyc_Absyn_Tvar*tv=(_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E->name=((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=(struct _dyneither_ptr)((_tmpA3F.tag=0,((_tmpA3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A3),((_tmpA40[0]=& _tmpA3F,Cyc_aprintf(((_tmpA41="`%s",_tag_dyneither(_tmpA41,sizeof(char),4))),_tag_dyneither(_tmpA40,sizeof(void*),1)))))))),_tmpA42)))),((_tmpA3E->identity=- 1,((_tmpA3E->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA3E)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA45;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA44;void*t=(void*)((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=2,((_tmpA45.f1=tv,_tmpA45)))),_tmpA44))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA4B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA4A;struct _dyneither_ptr*_tmpA48;struct _tuple0*_tmpA47;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47->f1=Cyc_Absyn_Loc_n,((_tmpA47->f2=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=_tmp4A3,_tmpA48)))),_tmpA47)))))),(void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=15,((_tmpA4B.f1=t,_tmpA4B)))),_tmpA4A)))),0);
# 1362
{struct Cyc_List_List*_tmpA4C;yyval=Cyc_YY18(((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4A2,_tmp4A4,(unsigned int)(yyyylsp[0]).first_line),((_tmpA4C->tl=0,_tmpA4C)))))));}
# 1364
break;};}case 36: _LL241: {
# 1362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1366
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA54;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA53;struct _dyneither_ptr*_tmpA52;struct Cyc_Absyn_Tvar*_tmpA51;struct Cyc_Absyn_Tvar*tv=(_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->name=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA52)))),((_tmpA51->identity=- 1,((_tmpA51->kind=(void*)((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA53.tag=0,((_tmpA53.f1=& Cyc_Tcutil_rk,_tmpA53)))),_tmpA54)))),_tmpA51)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY18(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA57;void*_tmpA56;(_tmpA56=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA57="too many declarations in alias",_tag_dyneither(_tmpA57,sizeof(char),31))),_tag_dyneither(_tmpA56,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_tmp4B8=d->r;struct Cyc_Absyn_Vardecl*_tmp4BA;_LL244: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4B9=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4B8;if(_tmp4B9->tag != 0)goto _LL246;else{_tmp4BA=_tmp4B9->f1;}}_LL245:
# 1373
{struct Cyc_List_List*_tmpA58;yyval=Cyc_YY18(((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->hd=Cyc_Absyn_alias_decl(tv,_tmp4BA,(unsigned int)(yyyylsp[0]).first_line),((_tmpA58->tl=0,_tmpA58)))))));}
goto _LL243;_LL246:;_LL247:
# 1376
{const char*_tmpA59;Cyc_Parse_err(((_tmpA59="expecting variable declaration",_tag_dyneither(_tmpA59,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY18(0);_LL243:;}
# 1380
break;};}case 37: _LL242:
# 1379 "parse.y"
 yyval=Cyc_YY31(0);
break;case 38: _LL248: {
# 1380 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1384
yyval=Cyc_YY31(1);
break;}case 39: _LL249: {
# 1385 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1389
yyval=yyyyvsp[0];
break;}case 40: _LL24A: {
# 1387 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1391
yyval=Cyc_YY18(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY18(yyyyvsp[1])));
break;}case 41: _LL24B: {
# 1393 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1397
{struct Cyc_Parse_Declaration_spec _tmpA5A;yyval=Cyc_YY19(((_tmpA5A.sc=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpA5A.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA5A.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA5A.is_inline=0,((_tmpA5A.attributes=0,_tmpA5A)))))))))));}
break;}case 42: _LL24C: {
# 1396 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1400
struct Cyc_Parse_Declaration_spec _tmp4C2=Cyc_yyget_YY19(yyyyvsp[1]);
if(_tmp4C2.sc != 0){
const char*_tmpA5D;void*_tmpA5C;(_tmpA5C=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA5D="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA5D,sizeof(char),51))),_tag_dyneither(_tmpA5C,sizeof(void*),0)));}
# 1404
{struct Cyc_Parse_Declaration_spec _tmpA5E;yyval=Cyc_YY19(((_tmpA5E.sc=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpA5E.tq=_tmp4C2.tq,((_tmpA5E.type_specs=_tmp4C2.type_specs,((_tmpA5E.is_inline=_tmp4C2.is_inline,((_tmpA5E.attributes=_tmp4C2.attributes,_tmpA5E)))))))))));}
# 1408
break;}case 43: _LL24D: {
# 1405 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1409
{struct Cyc_Parse_Declaration_spec _tmpA5F;yyval=Cyc_YY19(((_tmpA5F.sc=0,((_tmpA5F.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA5F.type_specs=
Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA5F.is_inline=0,((_tmpA5F.attributes=0,_tmpA5F)))))))))));}
break;}case 44: _LL24E: {
# 1408 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1412
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA60;yyval=Cyc_YY19(((_tmpA60.sc=_tmp4C7.sc,((_tmpA60.tq=_tmp4C7.tq,((_tmpA60.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4C7.type_specs,Cyc_yyget_YY23(yyyyvsp[0])),((_tmpA60.is_inline=_tmp4C7.is_inline,((_tmpA60.attributes=_tmp4C7.attributes,_tmpA60)))))))))));}
# 1418
break;}case 45: _LL24F: {
# 1415 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1419
{struct Cyc_Parse_Declaration_spec _tmpA61;yyval=Cyc_YY19(((_tmpA61.sc=0,((_tmpA61.tq=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpA61.type_specs=Cyc_Parse_empty_spec(0),((_tmpA61.is_inline=0,((_tmpA61.attributes=0,_tmpA61)))))))))));}
break;}case 46: _LL250: {
# 1417 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1421
struct Cyc_Parse_Declaration_spec _tmp4CA=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA62;yyval=Cyc_YY19(((_tmpA62.sc=_tmp4CA.sc,((_tmpA62.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp4CA.tq),((_tmpA62.type_specs=_tmp4CA.type_specs,((_tmpA62.is_inline=_tmp4CA.is_inline,((_tmpA62.attributes=_tmp4CA.attributes,_tmpA62)))))))))));}
# 1426
break;}case 47: _LL251: {
# 1423 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1427
{struct Cyc_Parse_Declaration_spec _tmpA63;yyval=Cyc_YY19(((_tmpA63.sc=0,((_tmpA63.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA63.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA63.is_inline=1,((_tmpA63.attributes=0,_tmpA63)))))))))));}
break;}case 48: _LL252: {
# 1426 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1430
struct Cyc_Parse_Declaration_spec _tmp4CD=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA64;yyval=Cyc_YY19(((_tmpA64.sc=_tmp4CD.sc,((_tmpA64.tq=_tmp4CD.tq,((_tmpA64.type_specs=_tmp4CD.type_specs,((_tmpA64.is_inline=1,((_tmpA64.attributes=_tmp4CD.attributes,_tmpA64)))))))))));}
# 1434
break;}case 49: _LL253: {
# 1431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1435
{struct Cyc_Parse_Declaration_spec _tmpA65;yyval=Cyc_YY19(((_tmpA65.sc=0,((_tmpA65.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA65.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA65.is_inline=0,((_tmpA65.attributes=Cyc_yyget_YY45(yyyyvsp[0]),_tmpA65)))))))))));}
break;}case 50: _LL254: {
# 1434 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1438
struct Cyc_Parse_Declaration_spec _tmp4D0=Cyc_yyget_YY19(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA66;yyval=Cyc_YY19(((_tmpA66.sc=_tmp4D0.sc,((_tmpA66.tq=_tmp4D0.tq,((_tmpA66.type_specs=_tmp4D0.type_specs,((_tmpA66.is_inline=_tmp4D0.is_inline,((_tmpA66.attributes=
# 1441
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp4D0.attributes),_tmpA66)))))))))));}
break;}case 51: _LL255: {
# 1441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1445
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY22(& auto_sc);
break;}case 52: _LL256: {
# 1443 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1447
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY22(& register_sc);
break;}case 53: _LL257: {
# 1445 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY22(& static_sc);
break;}case 54: _LL258: {
# 1447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1451
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY22(& extern_sc);
break;}case 55: _LL259: {
# 1450 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1454
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA67;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA67="C",_tag_dyneither(_tmpA67,sizeof(char),2))))!= 0){
const char*_tmpA68;Cyc_Parse_err(((_tmpA68="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA68,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY22(& externC_sc);
# 1459
break;}case 56: _LL25A: {
# 1455 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1459
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY22(& typedef_sc);
break;}case 57: _LL25B: {
# 1458 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1462
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY22(& abstract_sc);
break;}case 58: _LL25C:
# 1464 "parse.y"
 yyval=Cyc_YY45(0);
break;case 59: _LL25D: {
# 1465 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1469
yyval=yyyyvsp[0];
break;}case 60: _LL25E: {
# 1470 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1474
yyval=yyyyvsp[3];
break;}case 61: _LL25F: {
# 1474 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1478
{struct Cyc_List_List*_tmpA69;yyval=Cyc_YY45(((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpA69->tl=0,_tmpA69)))))));}
break;}case 62: _LL260: {
# 1475 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1479
{struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY45(((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=Cyc_yyget_YY46(yyyyvsp[0]),((_tmpA6A->tl=Cyc_yyget_YY45(yyyyvsp[2]),_tmpA6A)))))));}
break;}case 63: _LL261: {
# 1480 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1484
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple26 att_map[17]={{{_tmp4D8,_tmp4D8,_tmp4D8 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4D9,_tmp4D9,_tmp4D9 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4DA,_tmp4DA,_tmp4DA + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4DB,_tmp4DB,_tmp4DB + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4DC,_tmp4DC,_tmp4DC + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4DD,_tmp4DD,_tmp4DD + 8},(void*)& att_aligned},{{_tmp4DE,_tmp4DE,_tmp4DE + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4DF,_tmp4DF,_tmp4DF + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E0,_tmp4E0,_tmp4E0 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1504
struct _dyneither_ptr _tmp4D6=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1506
if((((_get_dyneither_size(_tmp4D6,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4D6,sizeof(char),(int)(_get_dyneither_size(_tmp4D6,sizeof(char))- 3)))== '_')
# 1508
_tmp4D6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4D6,2,_get_dyneither_size(_tmp4D6,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4D6,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY46((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1515
if(i == 17){
{const char*_tmpA6B;Cyc_Parse_err(((_tmpA6B="unrecognized attribute",_tag_dyneither(_tmpA6B,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1520
break;};}case 64: _LL262: {
# 1516 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1520
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL263: {
# 1518 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1522
struct _dyneither_ptr _tmp4EA=Cyc_yyget_String_tok(yyyyvsp[0]);
int _tmp4EC;struct _tuple5 _tmp4EB=Cyc_yyget_Int_tok(yyyyvsp[2]);_tmp4EC=_tmp4EB.f2;{
void*a;
{const char*_tmpA6D;const char*_tmpA6C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA6D="regparm",_tag_dyneither(_tmpA6D,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA6C="__regparm__",_tag_dyneither(_tmpA6C,sizeof(char),12))))== 0){
if(_tmp4EC < 0  || _tmp4EC > 3){
const char*_tmpA6E;Cyc_Parse_err(((_tmpA6E="regparm requires value between 0 and 3",_tag_dyneither(_tmpA6E,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA71;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA70;a=(void*)((_tmpA70=_cycalloc_atomic(sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=0,((_tmpA71.f1=_tmp4EC,_tmpA71)))),_tmpA70))));};}else{
const char*_tmpA73;const char*_tmpA72;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA73="aligned",_tag_dyneither(_tmpA73,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA72="__aligned__",_tag_dyneither(_tmpA72,sizeof(char),12))))== 0){
if(_tmp4EC < 0){const char*_tmpA74;Cyc_Parse_err(((_tmpA74="aligned requires positive power of two",_tag_dyneither(_tmpA74,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4EC;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA75;Cyc_Parse_err(((_tmpA75="aligned requires positive power of two",_tag_dyneither(_tmpA75,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA78;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA77;a=(void*)((_tmpA77=_cycalloc_atomic(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=6,((_tmpA78.f1=_tmp4EC,_tmpA78)))),_tmpA77))));};};}else{
const char*_tmpA7A;const char*_tmpA79;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA7A="initializes",_tag_dyneither(_tmpA7A,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA79="__initializes__",_tag_dyneither(_tmpA79,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA7D;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA7C;a=(void*)((_tmpA7C=_cycalloc_atomic(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=20,((_tmpA7D.f1=_tmp4EC,_tmpA7D)))),_tmpA7C))));}else{
const char*_tmpA7F;const char*_tmpA7E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA7F="noliveunique",_tag_dyneither(_tmpA7F,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA7E="__noliveunique__",_tag_dyneither(_tmpA7E,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA82;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA81;a=(void*)((_tmpA81=_cycalloc_atomic(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=21,((_tmpA82.f1=_tmp4EC,_tmpA82)))),_tmpA81))));}else{
const char*_tmpA84;const char*_tmpA83;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA84="noconsume",_tag_dyneither(_tmpA84,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EA,((_tmpA83="__noconsume__",_tag_dyneither(_tmpA83,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA87;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA86;a=(void*)((_tmpA86=_cycalloc_atomic(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=22,((_tmpA87.f1=_tmp4EC,_tmpA87)))),_tmpA86))));}else{
# 1544
{const char*_tmpA88;Cyc_Parse_err(((_tmpA88="unrecognized attribute",_tag_dyneither(_tmpA88,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1547
yyval=Cyc_YY46(a);
# 1549
break;};}case 66: _LL264: {
# 1546 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1550
struct _dyneither_ptr _tmp505=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp506=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpA8A;const char*_tmpA89;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp505,((_tmpA8A="section",_tag_dyneither(_tmpA8A,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp505,((_tmpA89="__section__",_tag_dyneither(_tmpA89,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA8D;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpA8C;a=(void*)((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8D.tag=8,((_tmpA8D.f1=_tmp506,_tmpA8D)))),_tmpA8C))));}else{
# 1556
{const char*_tmpA8E;Cyc_Parse_err(((_tmpA8E="unrecognized attribute",_tag_dyneither(_tmpA8E,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1559
yyval=Cyc_YY46(a);
# 1561
break;}case 67: _LL265: {
# 1558 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1562
struct _dyneither_ptr _tmp50C=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1566
{const char*_tmpA8F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50C,((_tmpA8F="__mode__",_tag_dyneither(_tmpA8F,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA92;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpA91;a=(void*)((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=24,((_tmpA92.f1=_tmp50D,_tmpA92)))),_tmpA91))));}else{
# 1569
{const char*_tmpA93;Cyc_Parse_err(((_tmpA93="unrecognized attribute",_tag_dyneither(_tmpA93,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1572
yyval=Cyc_YY46(a);
# 1574
break;}case 68: _LL266: {
# 1571 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1573
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1575
struct _dyneither_ptr _tmp512=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp513=Cyc_yyget_String_tok(yyyyvsp[2]);
int _tmp515;struct _tuple5 _tmp514=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp515=_tmp514.f2;{
int _tmp517;struct _tuple5 _tmp516=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp517=_tmp516.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpA95;const char*_tmpA94;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp512,((_tmpA95="format",_tag_dyneither(_tmpA95,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp512,((_tmpA94="__format__",_tag_dyneither(_tmpA94,sizeof(char),11))))== 0){
const char*_tmpA96;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp513,((_tmpA96="printf",_tag_dyneither(_tmpA96,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA99;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA98;a=(void*)((_tmpA98=_cycalloc_atomic(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA99.tag=19,((_tmpA99.f1=Cyc_Absyn_Printf_ft,((_tmpA99.f2=_tmp515,((_tmpA99.f3=_tmp517,_tmpA99)))))))),_tmpA98))));}else{
const char*_tmpA9A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp513,((_tmpA9A="scanf",_tag_dyneither(_tmpA9A,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpA9D;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA9C;a=(void*)((_tmpA9C=_cycalloc_atomic(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=19,((_tmpA9D.f1=Cyc_Absyn_Scanf_ft,((_tmpA9D.f2=_tmp515,((_tmpA9D.f3=_tmp517,_tmpA9D)))))))),_tmpA9C))));}else{
# 1586
const char*_tmpA9E;Cyc_Parse_err(((_tmpA9E="unrecognized format type",_tag_dyneither(_tmpA9E,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1588
const char*_tmpA9F;Cyc_Parse_err(((_tmpA9F="unrecognized attribute",_tag_dyneither(_tmpA9F,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY46(a);
# 1591
break;};};}case 69: _LL267: {
# 1596 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1600
yyval=yyyyvsp[0];
break;}case 70: _LL268: {
# 1598 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1602
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAA2;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAA1;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=((_tmpAA2.tag=17,((_tmpAA2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAA2.f2=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpAA2.f3=0,((_tmpAA2.f4=0,_tmpAA2)))))))))),_tmpAA1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 71: _LL269: {
# 1602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 72: _LL26A: {
# 1603 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL26B: {
# 1605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1609
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL26C: {
# 1606 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL26D: {
# 1607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611
yyval=Cyc_YY23(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL26E: {
# 1608 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL26F: {
# 1609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613
yyval=Cyc_YY23(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL270: {
# 1610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL271: {
# 1611 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL272: {
# 1612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616
yyval=Cyc_YY23(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL273: {
# 1613 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617
yyval=Cyc_YY23(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL274: {
# 1614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618
yyval=yyyyvsp[0];
break;}case 83: _LL275: {
# 1615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619
yyval=yyyyvsp[0];
break;}case 84: _LL276: {
# 1617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1621
yyval=yyyyvsp[0];
break;}case 85: _LL277: {
# 1619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1623
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 86: _LL278: {
# 1622 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1626
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAA5;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAA4;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA5.tag=10,((_tmpAA5.f1=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2]))),_tmpAA5)))),_tmpAA4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1629
break;}case 87: _LL279: {
# 1626 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1630
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA8;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA7;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAA8.tag=15,((_tmpAA8.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpAA8)))),_tmpAA7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL27A: {
# 1628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1632
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAAB;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAAA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=15,((_tmpAAB.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAAB)))),_tmpAAA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1634
break;}case 89: _LL27B: {
# 1631 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1635
void*_tmp52A=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAAE;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAAD;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAE.tag=16,((_tmpAAE.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAAE.f2=_tmp52A,_tmpAAE)))))),_tmpAAD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1638
break;}case 90: _LL27C: {
# 1635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1639
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAB1;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB0;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB1.tag=16,((_tmpAB1.f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpAB1.f2=Cyc_yyget_YY44(yyyyvsp[4]),_tmpAB1)))))),_tmpAB0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL27D: {
# 1637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1641
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAB4;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAB3;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB4.tag=19,((_tmpAB4.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpAB4)))),_tmpAB3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL27E: {
# 1639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1643
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAB7;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAB6;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=((_tmpAB7.tag=19,((_tmpAB7.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAB7)))),_tmpAB6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL27F: {
# 1641 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1645
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpABA;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAB9;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=18,((_tmpABA.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpABA)))),_tmpAB9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL280: {
# 1646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650
yyval=Cyc_YY43(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 95: _LL281: {
# 1650 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpABB;yyval=Cyc_YY25(((_tmpABB.print_const=1,((_tmpABB.q_volatile=0,((_tmpABB.q_restrict=0,((_tmpABB.real_const=1,((_tmpABB.loc=loc,_tmpABB)))))))))));}
break;}case 96: _LL282: {
# 1652 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656
{struct Cyc_Absyn_Tqual _tmpABC;yyval=Cyc_YY25(((_tmpABC.print_const=0,((_tmpABC.q_volatile=1,((_tmpABC.q_restrict=0,((_tmpABC.real_const=0,((_tmpABC.loc=0,_tmpABC)))))))))));}
break;}case 97: _LL283: {
# 1653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657
{struct Cyc_Absyn_Tqual _tmpABD;yyval=Cyc_YY25(((_tmpABD.print_const=0,((_tmpABD.q_volatile=0,((_tmpABD.q_restrict=1,((_tmpABD.real_const=0,((_tmpABD.loc=0,_tmpABD)))))))))));}
break;}case 98: _LL284: {
# 1659 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1663
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpACC;struct Cyc_Absyn_Enumdecl*_tmpACB;struct Cyc_Core_Opt*_tmpACA;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAC9;struct Cyc_Absyn_TypeDecl*_tmpAC8;struct Cyc_Absyn_TypeDecl*_tmp538=(_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8->r=(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpAC9.tag=1,((_tmpAC9.f1=((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB->sc=Cyc_Absyn_Public,((_tmpACB->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpACB->fields=((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->v=Cyc_yyget_YY48(yyyyvsp[3]),_tmpACA)))),_tmpACB)))))))),_tmpAC9)))),_tmpACC)))),((_tmpAC8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAC8)))));
# 1665
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpACF;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpACE;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=26,((_tmpACF.f1=_tmp538,((_tmpACF.f2=0,_tmpACF)))))),_tmpACE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1667
break;}case 99: _LL285: {
# 1664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1668
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAD2;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAD1;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD2.tag=13,((_tmpAD2.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAD2.f2=0,_tmpAD2)))))),_tmpAD1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL286: {
# 1666 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1670
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAD5;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAD4;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD5.tag=14,((_tmpAD5.f1=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAD5)))),_tmpAD4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL287: {
# 1672 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1676
{struct Cyc_Absyn_Enumfield*_tmpAD6;yyval=Cyc_YY47(((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAD6->tag=0,((_tmpAD6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAD6)))))))));}
break;}case 102: _LL288: {
# 1674 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1678
{struct Cyc_Absyn_Enumfield*_tmpAD7;yyval=Cyc_YY47(((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAD7->tag=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpAD7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAD7)))))))));}
break;}case 103: _LL289: {
# 1678 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1682
{struct Cyc_List_List*_tmpAD8;yyval=Cyc_YY48(((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpAD8->tl=0,_tmpAD8)))))));}
break;}case 104: _LL28A: {
# 1679 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1683
{struct Cyc_List_List*_tmpAD9;yyval=Cyc_YY48(((_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpAD9->tl=0,_tmpAD9)))))));}
break;}case 105: _LL28B: {
# 1680 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1684
{struct Cyc_List_List*_tmpADA;yyval=Cyc_YY48(((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpADA->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpADA)))))));}
break;}case 106: _LL28C: {
# 1686 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1690
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpADD;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpADC;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADD.tag=12,((_tmpADD.f1=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpADD.f2=Cyc_yyget_YY26(yyyyvsp[2]),_tmpADD)))))),_tmpADC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 107: _LL28D: {
# 1691 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1696
struct Cyc_List_List*_tmp54B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY40(yyyyvsp[3]));
struct Cyc_List_List*_tmp54C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY40(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp54D=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp54B,
Cyc_Absyn_aggrdecl_impl(_tmp54C,Cyc_yyget_YY50(yyyyvsp[6]),Cyc_yyget_YY26(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1701
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE0;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpADF;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF[0]=((_tmpAE0.tag=26,((_tmpAE0.f1=_tmp54D,((_tmpAE0.f2=0,_tmpAE0)))))),_tmpADF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1703
break;}case 108: _LL28E: {
# 1702 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1707
struct Cyc_List_List*_tmp550=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_List_List*_tmp551=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY40(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp552=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp550,
Cyc_Absyn_aggrdecl_impl(_tmp551,Cyc_yyget_YY50(yyyyvsp[5]),Cyc_yyget_YY26(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1712
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE3;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE2;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE3.tag=26,((_tmpAE3.f1=_tmp552,((_tmpAE3.f2=0,_tmpAE3)))))),_tmpAE2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1714
break;}case 109: _LL28F: {
# 1711 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1715
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAED;struct Cyc_Core_Opt*_tmpAEC;struct Cyc_Absyn_AggrInfo _tmpAEB;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAEA;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAED.tag=11,((_tmpAED.f1=((_tmpAEB.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpAEC=_cycalloc_atomic(sizeof(*_tmpAEC)),((_tmpAEC->v=(void*)1,_tmpAEC))))),((_tmpAEB.targs=Cyc_yyget_YY40(yyyyvsp[3]),_tmpAEB)))),_tmpAED)))),_tmpAEA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1718
break;}case 110: _LL290: {
# 1715 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1719
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAF3;struct Cyc_Absyn_AggrInfo _tmpAF2;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAF1;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF3.tag=11,((_tmpAF3.f1=((_tmpAF2.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpAF2.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpAF2)))),_tmpAF3)))),_tmpAF1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1722
break;}case 111: _LL291:
# 1722 "parse.y"
 yyval=Cyc_YY40(0);
break;case 112: _LL292: {
# 1724 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1728
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
break;}case 113: _LL293: {
# 1728 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1732
yyval=Cyc_YY24(Cyc_Absyn_StructA);
break;}case 114: _LL294: {
# 1729 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1733
yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;}case 115: _LL295:
# 1734 "parse.y"
 yyval=Cyc_YY26(0);
break;case 116: _LL296: {
# 1736 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1740
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp55C=Cyc_yyget_YY27(yyyyvsp[0]);for(0;_tmp55C != 0;_tmp55C=_tmp55C->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp55C->hd,decls);}}{
# 1744
struct Cyc_List_List*_tmp55D=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp55D != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp55D,decls);
yyval=Cyc_YY26(decls);
# 1749
break;};}case 117: _LL297: {
# 1750 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1754
{struct Cyc_List_List*_tmpAF4;yyval=Cyc_YY27(((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->hd=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpAF4->tl=0,_tmpAF4)))))));}
break;}case 118: _LL298: {
# 1752 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1756
{struct Cyc_List_List*_tmpAF5;yyval=Cyc_YY27(((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->hd=Cyc_yyget_YY26(yyyyvsp[1]),((_tmpAF5->tl=Cyc_yyget_YY27(yyyyvsp[0]),_tmpAF5)))))));}
break;}case 119: _LL299: {
# 1756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1760
yyval=Cyc_YY21(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
break;}case 120: _LL29A: {
# 1762 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1766
{struct _tuple12*_tmpAF6;yyval=Cyc_YY21(((_tmpAF6=_region_malloc(yyr,sizeof(*_tmpAF6)),((_tmpAF6->tl=0,((_tmpAF6->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpAF6)))))));}
break;}case 121: _LL29B: {
# 1764 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1768
{struct _tuple12*_tmpAF7;yyval=Cyc_YY21(((_tmpAF7=_region_malloc(yyr,sizeof(*_tmpAF7)),((_tmpAF7->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpAF7->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpAF7)))))));}
break;}case 122: _LL29C: {
# 1769 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1773
{struct _tuple11 _tmpAF8;yyval=Cyc_YY20(((_tmpAF8.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpAF8.f2=0,_tmpAF8)))));}
break;}case 123: _LL29D: {
# 1771 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1775
{struct _tuple11 _tmpAF9;yyval=Cyc_YY20(((_tmpAF9.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpAF9.f2=Cyc_yyget_YY3(yyyyvsp[2]),_tmpAF9)))));}
break;}case 124: _LL29E: {
# 1776 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1788 "parse.y"
struct _RegionHandle _tmp564=_new_region("temp");struct _RegionHandle*temp=& _tmp564;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp566;struct Cyc_Parse_Type_specifier _tmp567;struct Cyc_List_List*_tmp568;struct _tuple23 _tmp565=Cyc_yyget_YY35(yyyyvsp[0]);_tmp566=_tmp565.f1;_tmp567=_tmp565.f2;_tmp568=_tmp565.f3;
if(_tmp566.loc == 0)_tmp566.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths=0;
Cyc_Parse_decl_split(temp,Cyc_yyget_YY21(yyyyvsp[1]),& decls,& widths);{
void*_tmp569=Cyc_Parse_speclist2typ(_tmp567,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp56A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp566,_tmp569,decls,_tmp568),widths);
yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple16*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp56A));};};}
# 1800
_npop_handler(0);break;
# 1788
;_pop_region(temp);}case 125: _LL29F: {
# 1804 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1808
{struct _tuple23 _tmpAFA;yyval=Cyc_YY35(((_tmpAFA.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFA.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAFA.f3=0,_tmpAFA)))))));}
break;}case 126: _LL2A0: {
# 1806 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1810
struct _tuple23 _tmp56C=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpAFB;yyval=Cyc_YY35(((_tmpAFB.f1=_tmp56C.f1,((_tmpAFB.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp56C.f2),((_tmpAFB.f3=_tmp56C.f3,_tmpAFB)))))));}
break;}case 127: _LL2A1: {
# 1808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812
{struct _tuple23 _tmpAFC;yyval=Cyc_YY35(((_tmpAFC.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAFC.f2=Cyc_Parse_empty_spec(0),((_tmpAFC.f3=0,_tmpAFC)))))));}
break;}case 128: _LL2A2: {
# 1810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814
struct _tuple23 _tmp56F=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple23 _tmpAFD;yyval=Cyc_YY35(((_tmpAFD.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp56F.f1),((_tmpAFD.f2=_tmp56F.f2,((_tmpAFD.f3=_tmp56F.f3,_tmpAFD)))))));}
break;}case 129: _LL2A3: {
# 1813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1817
{struct _tuple23 _tmpAFE;yyval=Cyc_YY35(((_tmpAFE.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAFE.f2=Cyc_Parse_empty_spec(0),((_tmpAFE.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpAFE)))))));}
break;}case 130: _LL2A4: {
# 1815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1819
struct _tuple23 _tmp572=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpAFF;yyval=Cyc_YY35(((_tmpAFF.f1=_tmp572.f1,((_tmpAFF.f2=_tmp572.f2,((_tmpAFF.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp572.f3),_tmpAFF)))))));}
break;}case 131: _LL2A5: {
# 1821 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1825
{struct _tuple23 _tmpB00;yyval=Cyc_YY35(((_tmpB00.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB00.f2=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpB00.f3=0,_tmpB00)))))));}
break;}case 132: _LL2A6: {
# 1823 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1827
struct _tuple23 _tmp575=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpB01;yyval=Cyc_YY35(((_tmpB01.f1=_tmp575.f1,((_tmpB01.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY23(yyyyvsp[0]),_tmp575.f2),((_tmpB01.f3=_tmp575.f3,_tmpB01)))))));}
break;}case 133: _LL2A7: {
# 1825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829
{struct _tuple23 _tmpB02;yyval=Cyc_YY35(((_tmpB02.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB02.f2=Cyc_Parse_empty_spec(0),((_tmpB02.f3=0,_tmpB02)))))));}
break;}case 134: _LL2A8: {
# 1827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831
struct _tuple23 _tmp578=Cyc_yyget_YY35(yyyyvsp[1]);
{struct _tuple23 _tmpB03;yyval=Cyc_YY35(((_tmpB03.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),_tmp578.f1),((_tmpB03.f2=_tmp578.f2,((_tmpB03.f3=_tmp578.f3,_tmpB03)))))));}
break;}case 135: _LL2A9: {
# 1830 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1834
{struct _tuple23 _tmpB04;yyval=Cyc_YY35(((_tmpB04.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB04.f2=Cyc_Parse_empty_spec(0),((_tmpB04.f3=Cyc_yyget_YY45(yyyyvsp[0]),_tmpB04)))))));}
break;}case 136: _LL2AA: {
# 1832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1836
struct _tuple23 _tmp57B=Cyc_yyget_YY35(yyyyvsp[1]);{struct _tuple23 _tmpB05;yyval=Cyc_YY35(((_tmpB05.f1=_tmp57B.f1,((_tmpB05.f2=_tmp57B.f2,((_tmpB05.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY45(yyyyvsp[0]),_tmp57B.f3),_tmpB05)))))));}
break;}case 137: _LL2AB: {
# 1836 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1840
yyval=Cyc_YY21(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY21(yyyyvsp[0])));
break;}case 138: _LL2AC: {
# 1842 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1846
{struct _tuple12*_tmpB06;yyval=Cyc_YY21(((_tmpB06=_region_malloc(yyr,sizeof(*_tmpB06)),((_tmpB06->tl=0,((_tmpB06->hd=Cyc_yyget_YY20(yyyyvsp[0]),_tmpB06)))))));}
break;}case 139: _LL2AD: {
# 1844 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1848
{struct _tuple12*_tmpB07;yyval=Cyc_YY21(((_tmpB07=_region_malloc(yyr,sizeof(*_tmpB07)),((_tmpB07->tl=Cyc_yyget_YY21(yyyyvsp[0]),((_tmpB07->hd=Cyc_yyget_YY20(yyyyvsp[2]),_tmpB07)))))));}
break;}case 140: _LL2AE: {
# 1849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1853
{struct _tuple11 _tmpB08;yyval=Cyc_YY20(((_tmpB08.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB08.f2=0,_tmpB08)))));}
break;}case 141: _LL2AF: {
# 1851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1857
{struct Cyc_Parse_Declarator _tmpB0E;struct _tuple0*_tmpB0D;struct _tuple11 _tmpB0C;yyval=Cyc_YY20(((_tmpB0C.f1=((_tmpB0E.id=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->f1=Cyc_Absyn_Rel_n(0),((_tmpB0D->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB0D)))))),((_tmpB0E.tms=0,_tmpB0E)))),((_tmpB0C.f2=Cyc_yyget_YY3(yyyyvsp[1]),_tmpB0C)))));}
# 1859
break;}case 142: _LL2B0:
# 1858 "parse.y"
{struct Cyc_Parse_Declarator _tmpB14;struct _tuple0*_tmpB13;struct _tuple11 _tmpB12;yyval=Cyc_YY20(((_tmpB12.f1=((_tmpB14.id=((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->f1=Cyc_Absyn_Rel_n(0),((_tmpB13->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB13)))))),((_tmpB14.tms=0,_tmpB14)))),((_tmpB12.f2=0,_tmpB12)))));}
# 1860
break;case 143: _LL2B1: {
# 1861 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1865
{struct _tuple11 _tmpB15;yyval=Cyc_YY20(((_tmpB15.f1=Cyc_yyget_YY29(yyyyvsp[0]),((_tmpB15.f2=Cyc_yyget_YY3(yyyyvsp[2]),_tmpB15)))));}
break;}case 144: _LL2B2: {
# 1867 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1871
int _tmp589=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp58A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB16;struct Cyc_Absyn_TypeDecl*_tmp58B=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp58A,((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->v=Cyc_yyget_YY34(yyyyvsp[4]),_tmpB16)))),_tmp589,(unsigned int)(yyyylsp[0]).first_line);
# 1875
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB19;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB18;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB19.tag=26,((_tmpB19.f1=_tmp58B,((_tmpB19.f2=0,_tmpB19)))))),_tmpB18)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1877
break;}case 145: _LL2B3: {
# 1874 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1878
int _tmp58F=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB23;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB22;struct Cyc_Absyn_DatatypeInfo _tmpB21;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB20;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB23.tag=3,((_tmpB23.f1=((_tmpB21.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB22.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB22.is_extensible=_tmp58F,_tmpB22))))),((_tmpB21.targs=Cyc_yyget_YY40(yyyyvsp[2]),_tmpB21)))),_tmpB23)))),_tmpB20)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1881
break;}case 146: _LL2B4: {
# 1878 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1882
int _tmp594=Cyc_yyget_YY31(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB2D;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB2C;struct Cyc_Absyn_DatatypeFieldInfo _tmpB2B;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB2A;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A[0]=((_tmpB2D.tag=4,((_tmpB2D.f1=((_tmpB2B.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB2C.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB2C.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB2C.is_extensible=_tmp594,_tmpB2C))))))),((_tmpB2B.targs=Cyc_yyget_YY40(yyyyvsp[4]),_tmpB2B)))),_tmpB2D)))),_tmpB2A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1887
break;}case 147: _LL2B5: {
# 1886 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1890
yyval=Cyc_YY31(0);
break;}case 148: _LL2B6: {
# 1887 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1891
yyval=Cyc_YY31(1);
break;}case 149: _LL2B7: {
# 1891 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1895
{struct Cyc_List_List*_tmpB2E;yyval=Cyc_YY34(((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB2E->tl=0,_tmpB2E)))))));}
break;}case 150: _LL2B8: {
# 1892 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1896
{struct Cyc_List_List*_tmpB2F;yyval=Cyc_YY34(((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB2F->tl=0,_tmpB2F)))))));}
break;}case 151: _LL2B9: {
# 1893 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1897
{struct Cyc_List_List*_tmpB30;yyval=Cyc_YY34(((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB30->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB30)))))));}
break;}case 152: _LL2BA: {
# 1894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1898
{struct Cyc_List_List*_tmpB31;yyval=Cyc_YY34(((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->hd=Cyc_yyget_YY33(yyyyvsp[0]),((_tmpB31->tl=Cyc_yyget_YY34(yyyyvsp[2]),_tmpB31)))))));}
break;}case 153: _LL2BB:
# 1898 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
break;case 154: _LL2BC: {
# 1899 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1903
yyval=Cyc_YY32(Cyc_Absyn_Extern);
break;}case 155: _LL2BD: {
# 1900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1904
yyval=Cyc_YY32(Cyc_Absyn_Static);
break;}case 156: _LL2BE: {
# 1904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1908
{struct Cyc_Absyn_Datatypefield*_tmpB32;yyval=Cyc_YY33(((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB32->typs=0,((_tmpB32->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB32->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB32)))))))))));}
break;}case 157: _LL2BF: {
# 1906 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1910
struct Cyc_List_List*_tmp59E=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB33;yyval=Cyc_YY33(((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB33->typs=_tmp59E,((_tmpB33->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB33->sc=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB33)))))))))));}
break;}case 158: _LL2C0: {
# 1912 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1916
yyval=yyyyvsp[0];
break;}case 159: _LL2C1: {
# 1914 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1918
struct Cyc_Parse_Declarator _tmp5A0=Cyc_yyget_YY29(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB34;yyval=Cyc_YY29(((_tmpB34.id=_tmp5A0.id,((_tmpB34.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp5A0.tms),_tmpB34)))));}
break;}case 160: _LL2C2: {
# 1921 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1925
yyval=yyyyvsp[0];
break;}case 161: _LL2C3: {
# 1923 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1927
struct Cyc_Parse_Declarator _tmp5A2=Cyc_yyget_YY29(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB35;yyval=Cyc_YY29(((_tmpB35.id=_tmp5A2.id,((_tmpB35.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),_tmp5A2.tms),_tmpB35)))));}
break;}case 162: _LL2C4: {
# 1929 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1933
{struct Cyc_Parse_Declarator _tmpB36;yyval=Cyc_YY29(((_tmpB36.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB36.tms=0,_tmpB36)))));}
break;}case 163: _LL2C5: {
# 1931 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1935
yyval=yyyyvsp[1];
break;}case 164: _LL2C6: {
# 1935 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1939
struct Cyc_Parse_Declarator _tmp5A5=Cyc_yyget_YY29(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB3C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB3B;struct Cyc_List_List*_tmpB3A;_tmp5A5.tms=((_tmpB3A=_region_malloc(yyr,sizeof(*_tmpB3A)),((_tmpB3A->hd=(void*)((_tmpB3C=_region_malloc(yyr,sizeof(*_tmpB3C)),((_tmpB3C[0]=((_tmpB3B.tag=5,((_tmpB3B.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB3B.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpB3B)))))),_tmpB3C)))),((_tmpB3A->tl=_tmp5A5.tms,_tmpB3A))))));}
yyval=yyyyvsp[2];
# 1943
break;}case 165: _LL2C7: {
# 1940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1944
{struct Cyc_List_List*_tmpB46;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB45;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB44;struct Cyc_Parse_Declarator _tmpB43;yyval=Cyc_YY29(((_tmpB43.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB43.tms=((_tmpB46=_region_malloc(yyr,sizeof(*_tmpB46)),((_tmpB46->hd=(void*)((_tmpB44=_region_malloc(yyr,sizeof(*_tmpB44)),((_tmpB44[0]=((_tmpB45.tag=0,((_tmpB45.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpB45.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB45)))))),_tmpB44)))),((_tmpB46->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB46)))))),_tmpB43)))));}
break;}case 166: _LL2C8: {
# 1942 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1946
{struct Cyc_List_List*_tmpB50;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB4F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB4E;struct Cyc_Parse_Declarator _tmpB4D;yyval=Cyc_YY29(((_tmpB4D.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB4D.tms=(
(_tmpB50=_region_malloc(yyr,sizeof(*_tmpB50)),((_tmpB50->hd=(void*)((_tmpB4E=_region_malloc(yyr,sizeof(*_tmpB4E)),((_tmpB4E[0]=((_tmpB4F.tag=1,((_tmpB4F.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpB4F.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpB4F.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB4F)))))))),_tmpB4E)))),((_tmpB50->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB50)))))),_tmpB4D)))));}
break;}case 167: _LL2C9: {
# 1945 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1949
struct Cyc_List_List*_tmp5B2;int _tmp5B3;struct Cyc_Absyn_VarargInfo*_tmp5B4;void*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct _tuple24*_tmp5B1=Cyc_yyget_YY39(yyyyvsp[2]);_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;_tmp5B4=_tmp5B1->f3;_tmp5B5=_tmp5B1->f4;_tmp5B6=_tmp5B1->f5;
{struct Cyc_List_List*_tmpB65;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB64;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB63;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB62;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB61;struct Cyc_Parse_Declarator _tmpB60;yyval=Cyc_YY29(((_tmpB60.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB60.tms=((_tmpB65=_region_malloc(yyr,sizeof(*_tmpB65)),((_tmpB65->hd=(void*)((_tmpB61=_region_malloc(yyr,sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB64.tag=3,((_tmpB64.f1=(void*)((_tmpB62=_region_malloc(yyr,sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=1,((_tmpB63.f1=_tmp5B2,((_tmpB63.f2=_tmp5B3,((_tmpB63.f3=_tmp5B4,((_tmpB63.f4=_tmp5B5,((_tmpB63.f5=_tmp5B6,_tmpB63)))))))))))),_tmpB62)))),_tmpB64)))),_tmpB61)))),((_tmpB65->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB65)))))),_tmpB60)))));}
# 1952
break;}case 168: _LL2CA: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1953
{struct Cyc_List_List*_tmpB7A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB79;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB78;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB77;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB76;struct Cyc_Parse_Declarator _tmpB75;yyval=Cyc_YY29(((_tmpB75.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB75.tms=(
(_tmpB7A=_region_malloc(yyr,sizeof(*_tmpB7A)),((_tmpB7A->hd=(void*)((_tmpB76=_region_malloc(yyr,sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB79.tag=3,((_tmpB79.f1=(void*)((_tmpB77=_region_malloc(yyr,sizeof(*_tmpB77)),((_tmpB77[0]=((_tmpB78.tag=1,((_tmpB78.f1=0,((_tmpB78.f2=0,((_tmpB78.f3=0,((_tmpB78.f4=
# 1956
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpB78.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpB78)))))))))))),_tmpB77)))),_tmpB79)))),_tmpB76)))),((_tmpB7A->tl=(
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB7A)))))),_tmpB75)))));}
# 1959
break;}case 169: _LL2CB: {
# 1956 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1960
{struct Cyc_List_List*_tmpB8F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB8E;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpB8D;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB8C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB8B;struct Cyc_Parse_Declarator _tmpB8A;yyval=Cyc_YY29(((_tmpB8A.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB8A.tms=((_tmpB8F=_region_malloc(yyr,sizeof(*_tmpB8F)),((_tmpB8F->hd=(void*)((_tmpB8B=_region_malloc(yyr,sizeof(*_tmpB8B)),((_tmpB8B[0]=((_tmpB8E.tag=3,((_tmpB8E.f1=(void*)((_tmpB8C=_region_malloc(yyr,sizeof(*_tmpB8C)),((_tmpB8C[0]=((_tmpB8D.tag=0,((_tmpB8D.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpB8D.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpB8D)))))),_tmpB8C)))),_tmpB8E)))),_tmpB8B)))),((_tmpB8F->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB8F)))))),_tmpB8A)))));}
break;}case 170: _LL2CC: {
# 1959 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1963
struct Cyc_List_List*_tmp5C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpB99;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpB98;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpB97;struct Cyc_Parse_Declarator _tmpB96;yyval=Cyc_YY29(((_tmpB96.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpB96.tms=((_tmpB99=_region_malloc(yyr,sizeof(*_tmpB99)),((_tmpB99->hd=(void*)((_tmpB97=_region_malloc(yyr,sizeof(*_tmpB97)),((_tmpB97[0]=((_tmpB98.tag=4,((_tmpB98.f1=_tmp5C9,((_tmpB98.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpB98.f3=0,_tmpB98)))))))),_tmpB97)))),((_tmpB99->tl=(Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpB99)))))),_tmpB96)))));}
# 1966
break;}case 171: _LL2CD: {
# 1963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1967
{struct Cyc_List_List*_tmpBA3;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBA2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBA1;struct Cyc_Parse_Declarator _tmpBA0;yyval=Cyc_YY29(((_tmpBA0.id=(Cyc_yyget_YY29(yyyyvsp[0])).id,((_tmpBA0.tms=((_tmpBA3=_region_malloc(yyr,sizeof(*_tmpBA3)),((_tmpBA3->hd=(void*)((_tmpBA1=_region_malloc(yyr,sizeof(*_tmpBA1)),((_tmpBA1[0]=((_tmpBA2.tag=5,((_tmpBA2.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBA2.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBA2)))))),_tmpBA1)))),((_tmpBA3->tl=(
Cyc_yyget_YY29(yyyyvsp[0])).tms,_tmpBA3)))))),_tmpBA0)))));}
# 1970
break;}case 172: _LL2CE: {
# 1977 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1981
{struct Cyc_Parse_Declarator _tmpBA4;yyval=Cyc_YY29(((_tmpBA4.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA4.tms=0,_tmpBA4)))));}
break;}case 173: _LL2CF: {
# 1979 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1983
{struct Cyc_Parse_Declarator _tmpBA5;yyval=Cyc_YY29(((_tmpBA5.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA5.tms=0,_tmpBA5)))));}
break;}case 174: _LL2D0: {
# 1981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1985
yyval=yyyyvsp[1];
break;}case 175: _LL2D1: {
# 1985 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1989
struct Cyc_Parse_Declarator _tmp5D4=Cyc_yyget_YY29(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBAB;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBAA;struct Cyc_List_List*_tmpBA9;_tmp5D4.tms=((_tmpBA9=_region_malloc(yyr,sizeof(*_tmpBA9)),((_tmpBA9->hd=(void*)((_tmpBAB=_region_malloc(yyr,sizeof(*_tmpBAB)),((_tmpBAB[0]=((_tmpBAA.tag=5,((_tmpBAA.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBAA.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpBAA)))))),_tmpBAB)))),((_tmpBA9->tl=_tmp5D4.tms,_tmpBA9))))));}
yyval=yyyyvsp[2];
# 1993
break;}case 176: _LL2D2: {
# 1990 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1994
struct Cyc_Parse_Declarator _tmp5D8=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBB5;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBB4;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBB3;struct Cyc_Parse_Declarator _tmpBB2;yyval=Cyc_YY29(((_tmpBB2.id=_tmp5D8.id,((_tmpBB2.tms=(
(_tmpBB5=_region_malloc(yyr,sizeof(*_tmpBB5)),((_tmpBB5->hd=(void*)((_tmpBB3=_region_malloc(yyr,sizeof(*_tmpBB3)),((_tmpBB3[0]=((_tmpBB4.tag=0,((_tmpBB4.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpBB4.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBB4)))))),_tmpBB3)))),((_tmpBB5->tl=_tmp5D8.tms,_tmpBB5)))))),_tmpBB2)))));}
break;}case 177: _LL2D3: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1998
struct Cyc_Parse_Declarator _tmp5DD=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBBF;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBBE;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBBD;struct Cyc_Parse_Declarator _tmpBBC;yyval=Cyc_YY29(((_tmpBBC.id=_tmp5DD.id,((_tmpBBC.tms=(
(_tmpBBF=_region_malloc(yyr,sizeof(*_tmpBBF)),((_tmpBBF->hd=(void*)((_tmpBBD=_region_malloc(yyr,sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=1,((_tmpBBE.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpBBE.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpBBE.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBBE)))))))),_tmpBBD)))),((_tmpBBF->tl=_tmp5DD.tms,_tmpBBF)))))),_tmpBBC)))));}
# 2002
break;}case 178: _LL2D4: {
# 1999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2003
struct Cyc_List_List*_tmp5E3;int _tmp5E4;struct Cyc_Absyn_VarargInfo*_tmp5E5;void*_tmp5E6;struct Cyc_List_List*_tmp5E7;struct _tuple24*_tmp5E2=Cyc_yyget_YY39(yyyyvsp[2]);_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;_tmp5E5=_tmp5E2->f3;_tmp5E6=_tmp5E2->f4;_tmp5E7=_tmp5E2->f5;{
struct Cyc_Parse_Declarator _tmp5E8=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBD2;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBD0;struct Cyc_Parse_Declarator _tmpBCF;yyval=Cyc_YY29(((_tmpBCF.id=_tmp5E8.id,((_tmpBCF.tms=((_tmpBD4=_region_malloc(yyr,sizeof(*_tmpBD4)),((_tmpBD4->hd=(void*)((_tmpBD0=_region_malloc(yyr,sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD3.tag=3,((_tmpBD3.f1=(void*)((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD2.tag=1,((_tmpBD2.f1=_tmp5E3,((_tmpBD2.f2=_tmp5E4,((_tmpBD2.f3=_tmp5E5,((_tmpBD2.f4=_tmp5E6,((_tmpBD2.f5=_tmp5E7,_tmpBD2)))))))))))),_tmpBD1)))),_tmpBD3)))),_tmpBD0)))),((_tmpBD4->tl=_tmp5E8.tms,_tmpBD4)))))),_tmpBCF)))));}
# 2007
break;};}case 179: _LL2D5: {
# 2004 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2008
struct Cyc_Parse_Declarator _tmp5EF=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBE9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBE6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE5;struct Cyc_Parse_Declarator _tmpBE4;yyval=Cyc_YY29(((_tmpBE4.id=_tmp5EF.id,((_tmpBE4.tms=(
(_tmpBE9=_region_malloc(yyr,sizeof(*_tmpBE9)),((_tmpBE9->hd=(void*)((_tmpBE5=_region_malloc(yyr,sizeof(*_tmpBE5)),((_tmpBE5[0]=((_tmpBE8.tag=3,((_tmpBE8.f1=(void*)((_tmpBE6=_region_malloc(yyr,sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=1,((_tmpBE7.f1=0,((_tmpBE7.f2=0,((_tmpBE7.f3=0,((_tmpBE7.f4=
# 2012
Cyc_yyget_YY49(yyyyvsp[2]),((_tmpBE7.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpBE7)))))))))))),_tmpBE6)))),_tmpBE8)))),_tmpBE5)))),((_tmpBE9->tl=_tmp5EF.tms,_tmpBE9)))))),_tmpBE4)))));}
# 2015
break;}case 180: _LL2D6: {
# 2012 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2016
struct Cyc_Parse_Declarator _tmp5F6=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBFE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBFD;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBFC;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBFB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBFA;struct Cyc_Parse_Declarator _tmpBF9;yyval=Cyc_YY29(((_tmpBF9.id=_tmp5F6.id,((_tmpBF9.tms=((_tmpBFE=_region_malloc(yyr,sizeof(*_tmpBFE)),((_tmpBFE->hd=(void*)((_tmpBFA=_region_malloc(yyr,sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFD.tag=3,((_tmpBFD.f1=(void*)((_tmpBFB=_region_malloc(yyr,sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=0,((_tmpBFC.f1=Cyc_yyget_YY36(yyyyvsp[2]),((_tmpBFC.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBFC)))))),_tmpBFB)))),_tmpBFD)))),_tmpBFA)))),((_tmpBFE->tl=_tmp5F6.tms,_tmpBFE)))))),_tmpBF9)))));}
break;}case 181: _LL2D7: {
# 2016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2020
struct Cyc_List_List*_tmp5FD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp5FE=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC08;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC07;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC06;struct Cyc_Parse_Declarator _tmpC05;yyval=Cyc_YY29(((_tmpC05.id=_tmp5FE.id,((_tmpC05.tms=((_tmpC08=_region_malloc(yyr,sizeof(*_tmpC08)),((_tmpC08->hd=(void*)((_tmpC06=_region_malloc(yyr,sizeof(*_tmpC06)),((_tmpC06[0]=((_tmpC07.tag=4,((_tmpC07.f1=_tmp5FD,((_tmpC07.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC07.f3=0,_tmpC07)))))))),_tmpC06)))),((_tmpC08->tl=_tmp5FE.tms,_tmpC08)))))),_tmpC05)))));}
# 2024
break;}case 182: _LL2D8: {
# 2021 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2025
struct Cyc_Parse_Declarator _tmp603=Cyc_yyget_YY29(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC12;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC11;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC10;struct Cyc_Parse_Declarator _tmpC0F;yyval=Cyc_YY29(((_tmpC0F.id=_tmp603.id,((_tmpC0F.tms=((_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12->hd=(void*)((_tmpC10=_region_malloc(yyr,sizeof(*_tmpC10)),((_tmpC10[0]=((_tmpC11.tag=5,((_tmpC11.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC11.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpC11)))))),_tmpC10)))),((_tmpC12->tl=_tmp603.tms,_tmpC12)))))),_tmpC0F)))));}
# 2028
break;}case 183: _LL2D9: {
# 2034 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2038
yyval=yyyyvsp[0];
break;}case 184: _LL2DA: {
# 2035 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2039
yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),Cyc_yyget_YY28(yyyyvsp[1])));
break;}case 185: _LL2DB: {
# 2039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2043
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC18;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC17;struct Cyc_List_List*_tmpC16;ans=((_tmpC16=_region_malloc(yyr,sizeof(*_tmpC16)),((_tmpC16->hd=(void*)((_tmpC18=_region_malloc(yyr,sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC17.tag=5,((_tmpC17.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC17.f2=Cyc_yyget_YY45(yyyyvsp[3]),_tmpC17)))))),_tmpC18)))),((_tmpC16->tl=ans,_tmpC16))))));}{
# 2047
struct Cyc_Absyn_PtrLoc*ptrloc=0;
unsigned int _tmp60C;union Cyc_Absyn_Constraint*_tmp60D;union Cyc_Absyn_Constraint*_tmp60E;struct _tuple20 _tmp60B=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp60C=_tmp60B.f1;_tmp60D=_tmp60B.f2;_tmp60E=_tmp60B.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC19;ptrloc=((_tmpC19=_cycalloc_atomic(sizeof(*_tmpC19)),((_tmpC19->ptr_loc=_tmp60C,((_tmpC19->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC19->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC19))))))));}{
# 2052
union Cyc_Absyn_Constraint*_tmp611;union Cyc_Absyn_Constraint*_tmp612;union Cyc_Absyn_Constraint*_tmp613;void*_tmp614;struct _tuple14 _tmp610=Cyc_Parse_collapse_pointer_quals(_tmp60C,_tmp60D,_tmp60E,Cyc_yyget_YY44(yyyyvsp[2]),Cyc_yyget_YY54(yyyyvsp[1]));_tmp611=_tmp610.f1;_tmp612=_tmp610.f2;_tmp613=_tmp610.f3;_tmp614=_tmp610.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC23;struct Cyc_Absyn_PtrAtts _tmpC22;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC21;struct Cyc_List_List*_tmpC20;ans=((_tmpC20=_region_malloc(yyr,sizeof(*_tmpC20)),((_tmpC20->hd=(void*)((_tmpC23=_region_malloc(yyr,sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC21.tag=2,((_tmpC21.f1=((_tmpC22.rgn=_tmp614,((_tmpC22.nullable=_tmp611,((_tmpC22.bounds=_tmp612,((_tmpC22.zero_term=_tmp613,((_tmpC22.ptrloc=ptrloc,_tmpC22)))))))))),((_tmpC21.f2=Cyc_yyget_YY25(yyyyvsp[4]),_tmpC21)))))),_tmpC23)))),((_tmpC20->tl=ans,_tmpC20))))));}
yyval=Cyc_YY28(ans);
# 2056
break;};};}case 186: _LL2DC:
# 2054 "parse.y"
 yyval=Cyc_YY54(0);
break;case 187: _LL2DD: {
# 2055 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2059
{struct Cyc_List_List*_tmpC24;yyval=Cyc_YY54(((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24->hd=Cyc_yyget_YY53(yyyyvsp[0]),((_tmpC24->tl=Cyc_yyget_YY54(yyyyvsp[1]),_tmpC24)))))));}
break;}case 188: _LL2DE: {
# 2060 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2064
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC27;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC26;yyval=Cyc_YY53((void*)((_tmpC26=_region_malloc(yyr,sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC27.tag=0,((_tmpC27.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpC27)))),_tmpC26)))));}
break;}case 189: _LL2DF: {
# 2062 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2066
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC2A;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC29;yyval=Cyc_YY53((void*)((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=1,((_tmpC2A.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpC2A)))),_tmpC29)))));}
break;}case 190: _LL2E0: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2068
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC2D;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC2C;yyval=Cyc_YY53((void*)((_tmpC2C=_region_malloc(yyr,sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=2,_tmpC2D)),_tmpC2C)))));}
break;}case 191: _LL2E1: {
# 2066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2070
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC30;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC2F;yyval=Cyc_YY53((void*)((_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC30.tag=3,_tmpC30)),_tmpC2F)))));}
break;}case 192: _LL2E2: {
# 2068 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC33;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC32;yyval=Cyc_YY53((void*)((_tmpC32=_region_malloc(yyr,sizeof(*_tmpC32)),((_tmpC32[0]=((_tmpC33.tag=4,_tmpC33)),_tmpC32)))));}
break;}case 193: _LL2E3: {
# 2070 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC36;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC35;yyval=Cyc_YY53((void*)((_tmpC35=_region_malloc(yyr,sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC36.tag=5,_tmpC36)),_tmpC35)))));}
break;}case 194: _LL2E4: {
# 2072 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC39;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC38;yyval=Cyc_YY53((void*)((_tmpC38=_region_malloc(yyr,sizeof(*_tmpC38)),((_tmpC38[0]=((_tmpC39.tag=6,_tmpC39)),_tmpC38)))));}
break;}case 195: _LL2E5: {
# 2074 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC3C;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC3B;yyval=Cyc_YY53((void*)((_tmpC3B=_region_malloc(yyr,sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3C.tag=7,_tmpC3C)),_tmpC3B)))));}
break;}case 196: _LL2E6: {
# 2079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2084
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC3D;yyval=Cyc_YY1(((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->f1=loc,((_tmpC3D->f2=Cyc_Absyn_true_conref,((_tmpC3D->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC3D)))))))));}
# 2087
break;}case 197: _LL2E7: {
# 2084 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2088
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC3E;yyval=Cyc_YY1(((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->f1=loc,((_tmpC3E->f2=Cyc_Absyn_false_conref,((_tmpC3E->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC3E)))))))));}
# 2091
break;}case 198: _LL2E8: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2092
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC3F;yyval=Cyc_YY1(((_tmpC3F=_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F->f1=loc,((_tmpC3F->f2=Cyc_Absyn_true_conref,((_tmpC3F->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC3F)))))))));}
# 2095
break;}case 199: _LL2E9:
# 2093 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 200: _LL2EA: {
# 2094 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2098
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC42;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC41;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=1,((_tmpC42.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpC42)))),_tmpC41))))));}
break;}case 201: _LL2EB:
# 2097 "parse.y"
 yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 202: _LL2EC: {
# 2098 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2102
yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;}case 203: _LL2ED: {
# 2099 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2103
yyval=Cyc_YY51(Cyc_Absyn_false_conref);
break;}case 204: _LL2EE:
# 2104 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 205: _LL2EF: {
# 2105 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2109
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 206: _LL2F0: {
# 2106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2110
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 207: _LL2F1:
# 2110 "parse.y"
 yyval=Cyc_YY25(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 208: _LL2F2: {
# 2111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2115
yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_YY25(yyyyvsp[1])));
break;}case 209: _LL2F3: {
# 2116 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2120
{struct _tuple24*_tmpC43;yyval=Cyc_YY39(((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC43->f2=0,((_tmpC43->f3=0,((_tmpC43->f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpC43->f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpC43)))))))))))));}
break;}case 210: _LL2F4: {
# 2118 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2122
{struct _tuple24*_tmpC44;yyval=Cyc_YY39(((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC44->f2=1,((_tmpC44->f3=0,((_tmpC44->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC44->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC44)))))))))))));}
break;}case 211: _LL2F5: {
# 2121 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2125
struct _dyneither_ptr*_tmp632;struct Cyc_Absyn_Tqual _tmp633;void*_tmp634;struct _tuple8*_tmp631=Cyc_yyget_YY37(yyyyvsp[2]);_tmp632=_tmp631->f1;_tmp633=_tmp631->f2;_tmp634=_tmp631->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC45;struct Cyc_Absyn_VarargInfo*_tmp635=(_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->name=_tmp632,((_tmpC45->tq=_tmp633,((_tmpC45->type=_tmp634,((_tmpC45->inject=Cyc_yyget_YY31(yyyyvsp[1]),_tmpC45)))))))));
{struct _tuple24*_tmpC46;yyval=Cyc_YY39(((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->f1=0,((_tmpC46->f2=0,((_tmpC46->f3=_tmp635,((_tmpC46->f4=Cyc_yyget_YY49(yyyyvsp[3]),((_tmpC46->f5=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC46)))))))))))));}
# 2129
break;};}case 212: _LL2F6: {
# 2127 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2131
struct _dyneither_ptr*_tmp639;struct Cyc_Absyn_Tqual _tmp63A;void*_tmp63B;struct _tuple8*_tmp638=Cyc_yyget_YY37(yyyyvsp[4]);_tmp639=_tmp638->f1;_tmp63A=_tmp638->f2;_tmp63B=_tmp638->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC47;struct Cyc_Absyn_VarargInfo*_tmp63C=(_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47->name=_tmp639,((_tmpC47->tq=_tmp63A,((_tmpC47->type=_tmp63B,((_tmpC47->inject=Cyc_yyget_YY31(yyyyvsp[3]),_tmpC47)))))))));
{struct _tuple24*_tmpC48;yyval=Cyc_YY39(((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0])),((_tmpC48->f2=0,((_tmpC48->f3=_tmp63C,((_tmpC48->f4=Cyc_yyget_YY49(yyyyvsp[5]),((_tmpC48->f5=Cyc_yyget_YY50(yyyyvsp[6]),_tmpC48)))))))))))));}
# 2135
break;};}case 213: _LL2F7: {
# 2135 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2139
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC4B;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC4A;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4B.tag=1,((_tmpC4B.f1=0,_tmpC4B)))),_tmpC4A))))));}
break;}case 214: _LL2F8: {
# 2136 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2140
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2]))));
break;}case 215: _LL2F9:
# 2139 "parse.y"
 yyval=Cyc_YY49(0);
break;case 216: _LL2FA: {
# 2140 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2144
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC4E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC4D;yyval=Cyc_YY49((void*)((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D[0]=((_tmpC4E.tag=24,((_tmpC4E.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpC4E)))),_tmpC4D)))));}
break;}case 217: _LL2FB:
# 2144 "parse.y"
 yyval=Cyc_YY50(0);
break;case 218: _LL2FC: {
# 2145 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2149
yyval=yyyyvsp[1];
break;}case 219: _LL2FD: {
# 2150 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2157
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC51;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC50;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp643=(_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=2,((_tmpC51.f1=0,((_tmpC51.f2=& Cyc_Tcutil_trk,_tmpC51)))))),_tmpC50)));
struct _dyneither_ptr _tmp644=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp645=Cyc_Parse_id2type(_tmp644,(void*)_tmp643);
{struct _tuple27*_tmpC5B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC59;struct Cyc_List_List*_tmpC58;yyval=Cyc_YY50(((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58->hd=((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->f1=(void*)((_tmpC59=_cycalloc(sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5A.tag=24,((_tmpC5A.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpC5A)))),_tmpC59)))),((_tmpC5B->f2=_tmp645,_tmpC5B)))))),((_tmpC58->tl=0,_tmpC58)))))));}
# 2162
break;}case 220: _LL2FE: {
# 2159 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2164
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC5E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC5D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp64C=(_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC5E.tag=2,((_tmpC5E.f1=0,((_tmpC5E.f2=& Cyc_Tcutil_trk,_tmpC5E)))))),_tmpC5D)));
struct _dyneither_ptr _tmp64D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp64E=Cyc_Parse_id2type(_tmp64D,(void*)_tmp64C);
{struct _tuple27*_tmpC68;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC67;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC66;struct Cyc_List_List*_tmpC65;yyval=Cyc_YY50(((_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65->hd=((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->f1=(void*)((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC67.tag=24,((_tmpC67.f1=Cyc_yyget_YY40(yyyyvsp[0]),_tmpC67)))),_tmpC66)))),((_tmpC68->f2=_tmp64E,_tmpC68)))))),((_tmpC65->tl=Cyc_yyget_YY50(yyyyvsp[4]),_tmpC65)))))));}
# 2169
break;}case 221: _LL2FF:
# 2169 "parse.y"
 yyval=Cyc_YY31(0);
break;case 222: _LL300: {
# 2171 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2175
{const char*_tmpC69;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC69="inject",_tag_dyneither(_tmpC69,sizeof(char),7))))!= 0){
const char*_tmpC6A;Cyc_Parse_err(((_tmpC6A="missing type in function declaration",_tag_dyneither(_tmpC6A,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY31(1);
# 2179
break;}case 223: _LL301: {
# 2178 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2182
yyval=yyyyvsp[0];
break;}case 224: _LL302: {
# 2179 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2183
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(yyyyvsp[0]),Cyc_yyget_YY40(yyyyvsp[2])));
break;}case 225: _LL303: {
# 2183 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2187
yyval=Cyc_YY40(0);
break;}case 226: _LL304: {
# 2184 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2186
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2188
yyval=yyyyvsp[1];
break;}case 227: _LL305: {
# 2186 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2190
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC70;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC6F;struct Cyc_List_List*_tmpC6E;yyval=Cyc_YY40(((_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E->hd=(void*)((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC6F.tag=25,((_tmpC6F.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpC6F)))),_tmpC70)))),((_tmpC6E->tl=0,_tmpC6E)))))));}
break;}case 228: _LL306: {
# 2188 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2192
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC71;yyval=Cyc_YY40(((_tmpC71=_cycalloc(sizeof(*_tmpC71)),((_tmpC71->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpC71->tl=0,_tmpC71)))))));}
# 2195
break;}case 229: _LL307: {
# 2196 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2200
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC77;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC76;struct Cyc_List_List*_tmpC75;yyval=Cyc_YY40(((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->hd=(void*)((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC76.tag=23,((_tmpC76.f1=Cyc_yyget_YY44(yyyyvsp[0]),_tmpC76)))),_tmpC77)))),((_tmpC75->tl=0,_tmpC75)))))));}
# 2203
break;}case 230: _LL308: {
# 2200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2204
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC7D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC7C;struct Cyc_List_List*_tmpC7B;yyval=Cyc_YY40(((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->hd=(void*)((_tmpC7D=_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC7C.tag=23,((_tmpC7C.f1=Cyc_yyget_YY44(yyyyvsp[0]),_tmpC7C)))),_tmpC7D)))),((_tmpC7B->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpC7B)))))));}
# 2207
break;}case 231: _LL309: {
# 2208 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2210
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2212
{struct Cyc_List_List*_tmpC7E;yyval=Cyc_YY38(((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->hd=Cyc_yyget_YY37(yyyyvsp[0]),((_tmpC7E->tl=0,_tmpC7E)))))));}
break;}case 232: _LL30A: {
# 2210 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2212
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2214
{struct Cyc_List_List*_tmpC7F;yyval=Cyc_YY38(((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->hd=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpC7F->tl=Cyc_yyget_YY38(yyyyvsp[0]),_tmpC7F)))))));}
break;}case 233: _LL30B: {
# 2216 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2218
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2220
struct Cyc_Absyn_Tqual _tmp664;struct Cyc_Parse_Type_specifier _tmp665;struct Cyc_List_List*_tmp666;struct _tuple23 _tmp663=Cyc_yyget_YY35(yyyyvsp[0]);_tmp664=_tmp663.f1;_tmp665=_tmp663.f2;_tmp666=_tmp663.f3;
if(_tmp664.loc == 0)_tmp664.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple0*_tmp668;struct Cyc_List_List*_tmp669;struct Cyc_Parse_Declarator _tmp667=Cyc_yyget_YY29(yyyyvsp[1]);_tmp668=_tmp667.id;_tmp669=_tmp667.tms;{
void*_tmp66A=Cyc_Parse_speclist2typ(_tmp665,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_Absyn_Tqual _tmp66C;void*_tmp66D;struct Cyc_List_List*_tmp66E;struct Cyc_List_List*_tmp66F;struct _tuple13 _tmp66B=Cyc_Parse_apply_tms(_tmp664,_tmp66A,_tmp666,_tmp669);_tmp66C=_tmp66B.f1;_tmp66D=_tmp66B.f2;_tmp66E=_tmp66B.f3;_tmp66F=_tmp66B.f4;
if(_tmp66E != 0){
const char*_tmpC80;Cyc_Parse_err(((_tmpC80="parameter with bad type params",_tag_dyneither(_tmpC80,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp668)){
const char*_tmpC81;Cyc_Parse_err(((_tmpC81="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC81,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp668).f2;
if(_tmp66F != 0){
const char*_tmpC84;void*_tmpC83;(_tmpC83=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC84="extra attributes on parameter, ignoring",_tag_dyneither(_tmpC84,sizeof(char),40))),_tag_dyneither(_tmpC83,sizeof(void*),0)));}
{struct _tuple8*_tmpC85;yyval=Cyc_YY37(((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->f1=idopt,((_tmpC85->f2=_tmp66C,((_tmpC85->f3=_tmp66D,_tmpC85)))))))));}
# 2234
break;};};};}case 234: _LL30C: {
# 2231 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2235
struct Cyc_Absyn_Tqual _tmp676;struct Cyc_Parse_Type_specifier _tmp677;struct Cyc_List_List*_tmp678;struct _tuple23 _tmp675=Cyc_yyget_YY35(yyyyvsp[0]);_tmp676=_tmp675.f1;_tmp677=_tmp675.f2;_tmp678=_tmp675.f3;
if(_tmp676.loc == 0)_tmp676.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp679=Cyc_Parse_speclist2typ(_tmp677,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp678 != 0){
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC88="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC88,sizeof(char),38))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}
{struct _tuple8*_tmpC89;yyval=Cyc_YY37(((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->f1=0,((_tmpC89->f2=_tmp676,((_tmpC89->f3=_tmp679,_tmpC89)))))))));}
# 2242
break;};}case 235: _LL30D: {
# 2239 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2243
struct Cyc_Absyn_Tqual _tmp67E;struct Cyc_Parse_Type_specifier _tmp67F;struct Cyc_List_List*_tmp680;struct _tuple23 _tmp67D=Cyc_yyget_YY35(yyyyvsp[0]);_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;_tmp680=_tmp67D.f3;
if(_tmp67E.loc == 0)_tmp67E.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp681=Cyc_Parse_speclist2typ(_tmp67F,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp682=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;struct Cyc_List_List*_tmp686;struct Cyc_List_List*_tmp687;struct _tuple13 _tmp683=Cyc_Parse_apply_tms(_tmp67E,_tmp681,_tmp680,_tmp682);_tmp684=_tmp683.f1;_tmp685=_tmp683.f2;_tmp686=_tmp683.f3;_tmp687=_tmp683.f4;
if(_tmp686 != 0){
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8C="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpC8C,sizeof(char),49))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}
# 2251
if(_tmp687 != 0){
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8F="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC8F,sizeof(char),38))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}
{struct _tuple8*_tmpC90;yyval=Cyc_YY37(((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->f1=0,((_tmpC90->f2=_tmp684,((_tmpC90->f3=_tmp685,_tmpC90)))))))));}
# 2255
break;};}case 236: _LL30E: {
# 2255 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2259
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
break;}case 237: _LL30F: {
# 2259 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2263
{struct _dyneither_ptr*_tmpC93;struct Cyc_List_List*_tmpC92;yyval=Cyc_YY36(((_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpC93)))),((_tmpC92->tl=0,_tmpC92)))))));}
break;}case 238: _LL310: {
# 2261 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2265
{struct _dyneither_ptr*_tmpC96;struct Cyc_List_List*_tmpC95;yyval=Cyc_YY36(((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95->hd=((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpC96)))),((_tmpC95->tl=Cyc_yyget_YY36(yyyyvsp[0]),_tmpC95)))))));}
break;}case 239: _LL311: {
# 2265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2269
yyval=yyyyvsp[0];
break;}case 240: _LL312: {
# 2266 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2268
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2270
yyval=yyyyvsp[0];
break;}case 241: _LL313: {
# 2271 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2275
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC99;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC98;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98[0]=((_tmpC99.tag=34,((_tmpC99.f1=0,((_tmpC99.f2=0,_tmpC99)))))),_tmpC98)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 242: _LL314: {
# 2273 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2275
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2277
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC9B;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=34,((_tmpC9C.f1=0,((_tmpC9C.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpC9C)))))),_tmpC9B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL315: {
# 2275 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2279
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpC9F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC9E;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9F.tag=34,((_tmpC9F.f1=0,((_tmpC9F.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[1])),_tmpC9F)))))),_tmpC9E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL316: {
# 2277 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2281
struct _dyneither_ptr*_tmpCA2;struct _tuple0*_tmpCA1;struct Cyc_Absyn_Vardecl*_tmp697=Cyc_Absyn_new_vardecl(((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=Cyc_Absyn_Loc_n,((_tmpCA1->f2=((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCA2)))),_tmpCA1)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2284
(_tmp697->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCA5;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCA4;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=26,((_tmpCA5.f1=_tmp697,((_tmpCA5.f2=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpCA5.f3=Cyc_yyget_YY3(yyyyvsp[6]),((_tmpCA5.f4=0,_tmpCA5)))))))))),_tmpCA4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2287
break;}case 245: _LL317: {
# 2288 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2292
{struct _tuple28*_tmpCA8;struct Cyc_List_List*_tmpCA7;yyval=Cyc_YY6(((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=0,((_tmpCA8->f2=Cyc_yyget_YY3(yyyyvsp[0]),_tmpCA8)))))),((_tmpCA7->tl=0,_tmpCA7)))))));}
break;}case 246: _LL318: {
# 2290 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2294
{struct _tuple28*_tmpCAB;struct Cyc_List_List*_tmpCAA;yyval=Cyc_YY6(((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpCAB->f2=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCAB)))))),((_tmpCAA->tl=0,_tmpCAA)))))));}
break;}case 247: _LL319: {
# 2292 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2296
{struct _tuple28*_tmpCAE;struct Cyc_List_List*_tmpCAD;yyval=Cyc_YY6(((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->f1=0,((_tmpCAE->f2=Cyc_yyget_YY3(yyyyvsp[2]),_tmpCAE)))))),((_tmpCAD->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCAD)))))));}
break;}case 248: _LL31A: {
# 2294 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2298
{struct _tuple28*_tmpCB1;struct Cyc_List_List*_tmpCB0;yyval=Cyc_YY6(((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->hd=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->f1=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpCB1->f2=Cyc_yyget_YY3(yyyyvsp[3]),_tmpCB1)))))),((_tmpCB0->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpCB0)))))));}
break;}case 249: _LL31B: {
# 2298 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2302
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
break;}case 250: _LL31C: {
# 2303 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2307
{struct Cyc_List_List*_tmpCB2;yyval=Cyc_YY41(((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->hd=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpCB2->tl=0,_tmpCB2)))))));}
break;}case 251: _LL31D: {
# 2304 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2308
{struct Cyc_List_List*_tmpCB3;yyval=Cyc_YY41(((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=Cyc_yyget_YY42(yyyyvsp[1]),((_tmpCB3->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCB3)))))));}
break;}case 252: _LL31E: {
# 2308 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2312
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCB6;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCB5;yyval=Cyc_YY42((void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=0,((_tmpCB6.f1=Cyc_yyget_YY3(yyyyvsp[1]),_tmpCB6)))),_tmpCB5)))));}
break;}case 253: _LL31F: {
# 2309 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2313
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCBC;struct _dyneither_ptr*_tmpCBB;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCBA;yyval=Cyc_YY42((void*)((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA[0]=((_tmpCBC.tag=1,((_tmpCBC.f1=((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCBB)))),_tmpCBC)))),_tmpCBA)))));}
break;}case 254: _LL320: {
# 2314 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2318
struct Cyc_Absyn_Tqual _tmp6AC;struct Cyc_Parse_Type_specifier _tmp6AD;struct Cyc_List_List*_tmp6AE;struct _tuple23 _tmp6AB=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6AC=_tmp6AB.f1;_tmp6AD=_tmp6AB.f2;_tmp6AE=_tmp6AB.f3;{
void*_tmp6AF=Cyc_Parse_speclist2typ(_tmp6AD,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6AE != 0){
const char*_tmpCBF;void*_tmpCBE;(_tmpCBE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCBF="ignoring attributes in type",_tag_dyneither(_tmpCBF,sizeof(char),28))),_tag_dyneither(_tmpCBE,sizeof(void*),0)));}
{struct _tuple8*_tmpCC0;yyval=Cyc_YY37(((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->f1=0,((_tmpCC0->f2=_tmp6AC,((_tmpCC0->f3=_tmp6AF,_tmpCC0)))))))));}
# 2324
break;};}case 255: _LL321: {
# 2321 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2325
struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_Parse_Type_specifier _tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple23 _tmp6B3=Cyc_yyget_YY35(yyyyvsp[0]);_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B3.f3;{
void*_tmp6B7=Cyc_Parse_speclist2typ(_tmp6B5,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6B8=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp6B9=Cyc_Parse_apply_tms(_tmp6B4,_tmp6B7,_tmp6B6,_tmp6B8);
if(_tmp6B9.f3 != 0){
# 2331
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCC3="bad type params, ignoring",_tag_dyneither(_tmpCC3,sizeof(char),26))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}
if(_tmp6B9.f4 != 0){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCC6="bad specifiers, ignoring",_tag_dyneither(_tmpCC6,sizeof(char),25))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}
{struct _tuple8*_tmpCC7;yyval=Cyc_YY37(((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->f1=0,((_tmpCC7->f2=_tmp6B9.f1,((_tmpCC7->f3=_tmp6B9.f2,_tmpCC7)))))))));}
# 2336
break;};}case 256: _LL322: {
# 2335 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2339
yyval=Cyc_YY44(Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 257: _LL323: {
# 2336 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2340
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCCA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC9;yyval=Cyc_YY44((void*)((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9[0]=((_tmpCCA.tag=24,((_tmpCCA.f1=0,_tmpCCA)))),_tmpCC9)))));}
break;}case 258: _LL324: {
# 2337 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2341
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCCD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCCC;yyval=Cyc_YY44((void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCD.tag=24,((_tmpCCD.f1=Cyc_yyget_YY40(yyyyvsp[1]),_tmpCCD)))),_tmpCCC)))));}
break;}case 259: _LL325: {
# 2338 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2342
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCD0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCCF;yyval=Cyc_YY44((void*)((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF[0]=((_tmpCD0.tag=25,((_tmpCD0.f1=Cyc_yyget_YY44(yyyyvsp[2]),_tmpCD0)))),_tmpCCF)))));}
break;}case 260: _LL326: {
# 2339 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2343
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD6;struct Cyc_List_List*_tmpCD5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD4;yyval=Cyc_YY44((void*)((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4[0]=((_tmpCD6.tag=24,((_tmpCD6.f1=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCD5->tl=Cyc_yyget_YY40(yyyyvsp[2]),_tmpCD5)))))),_tmpCD6)))),_tmpCD4)))));}
break;}case 261: _LL327: {
# 2345 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2349
{struct Cyc_List_List*_tmpCD7;yyval=Cyc_YY40(((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCD7->tl=0,_tmpCD7)))))));}
break;}case 262: _LL328: {
# 2346 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2350
{struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY40(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD8->tl=Cyc_yyget_YY40(yyyyvsp[0]),_tmpCD8)))))));}
break;}case 263: _LL329: {
# 2351 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2355
{struct Cyc_Parse_Abstractdeclarator _tmpCD9;yyval=Cyc_YY30(((_tmpCD9.tms=Cyc_yyget_YY28(yyyyvsp[0]),_tmpCD9)));}
break;}case 264: _LL32A: {
# 2353 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2357
yyval=yyyyvsp[0];
break;}case 265: _LL32B: {
# 2355 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2359
{struct Cyc_Parse_Abstractdeclarator _tmpCDA;yyval=Cyc_YY30(((_tmpCDA.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyyyvsp[0]),(Cyc_yyget_YY30(yyyyvsp[1])).tms),_tmpCDA)));}
break;}case 266: _LL32C: {
# 2360 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2364
yyval=yyyyvsp[1];
break;}case 267: _LL32D: {
# 2362 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2366
{struct Cyc_List_List*_tmpCE4;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCE3;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCE2;struct Cyc_Parse_Abstractdeclarator _tmpCE1;yyval=Cyc_YY30(((_tmpCE1.tms=((_tmpCE4=_region_malloc(yyr,sizeof(*_tmpCE4)),((_tmpCE4->hd=(void*)((_tmpCE2=_region_malloc(yyr,sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE3.tag=0,((_tmpCE3.f1=Cyc_yyget_YY51(yyyyvsp[2]),((_tmpCE3.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpCE3)))))),_tmpCE2)))),((_tmpCE4->tl=0,_tmpCE4)))))),_tmpCE1)));}
break;}case 268: _LL32E: {
# 2364 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2368
{struct Cyc_List_List*_tmpCEE;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCED;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCEC;struct Cyc_Parse_Abstractdeclarator _tmpCEB;yyval=Cyc_YY30(((_tmpCEB.tms=((_tmpCEE=_region_malloc(yyr,sizeof(*_tmpCEE)),((_tmpCEE->hd=(void*)((_tmpCEC=_region_malloc(yyr,sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=0,((_tmpCED.f1=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpCED.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpCED)))))),_tmpCEC)))),((_tmpCEE->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpCEE)))))),_tmpCEB)));}
break;}case 269: _LL32F: {
# 2366 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2370
{struct Cyc_List_List*_tmpCF8;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpCF7;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpCF6;struct Cyc_Parse_Abstractdeclarator _tmpCF5;yyval=Cyc_YY30(((_tmpCF5.tms=((_tmpCF8=_region_malloc(yyr,sizeof(*_tmpCF8)),((_tmpCF8->hd=(void*)((_tmpCF6=_region_malloc(yyr,sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF7.tag=1,((_tmpCF7.f1=Cyc_yyget_YY3(yyyyvsp[1]),((_tmpCF7.f2=Cyc_yyget_YY51(yyyyvsp[3]),((_tmpCF7.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpCF7)))))))),_tmpCF6)))),((_tmpCF8->tl=0,_tmpCF8)))))),_tmpCF5)));}
break;}case 270: _LL330: {
# 2368 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2372
{struct Cyc_List_List*_tmpD02;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD01;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD00;struct Cyc_Parse_Abstractdeclarator _tmpCFF;yyval=Cyc_YY30(((_tmpCFF.tms=((_tmpD02=_region_malloc(yyr,sizeof(*_tmpD02)),((_tmpD02->hd=(void*)((_tmpD00=_region_malloc(yyr,sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpD01.tag=1,((_tmpD01.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpD01.f2=Cyc_yyget_YY51(yyyyvsp[4]),((_tmpD01.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD01)))))))),_tmpD00)))),((_tmpD02->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD02)))))),_tmpCFF)));}
# 2375
break;}case 271: _LL331: {
# 2372 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376
{struct Cyc_List_List*_tmpD17;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD16;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD15;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD14;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD13;struct Cyc_Parse_Abstractdeclarator _tmpD12;yyval=Cyc_YY30(((_tmpD12.tms=((_tmpD17=_region_malloc(yyr,sizeof(*_tmpD17)),((_tmpD17->hd=(void*)((_tmpD13=_region_malloc(yyr,sizeof(*_tmpD13)),((_tmpD13[0]=((_tmpD16.tag=3,((_tmpD16.f1=(void*)((_tmpD14=_region_malloc(yyr,sizeof(*_tmpD14)),((_tmpD14[0]=((_tmpD15.tag=1,((_tmpD15.f1=0,((_tmpD15.f2=0,((_tmpD15.f3=0,((_tmpD15.f4=Cyc_yyget_YY49(yyyyvsp[1]),((_tmpD15.f5=Cyc_yyget_YY50(yyyyvsp[2]),_tmpD15)))))))))))),_tmpD14)))),_tmpD16)))),_tmpD13)))),((_tmpD17->tl=0,_tmpD17)))))),_tmpD12)));}
# 2378
break;}case 272: _LL332: {
# 2375 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2379
struct Cyc_List_List*_tmp6E3;int _tmp6E4;struct Cyc_Absyn_VarargInfo*_tmp6E5;void*_tmp6E6;struct Cyc_List_List*_tmp6E7;struct _tuple24*_tmp6E2=Cyc_yyget_YY39(yyyyvsp[1]);_tmp6E3=_tmp6E2->f1;_tmp6E4=_tmp6E2->f2;_tmp6E5=_tmp6E2->f3;_tmp6E6=_tmp6E2->f4;_tmp6E7=_tmp6E2->f5;
{struct Cyc_List_List*_tmpD2C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD2B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD2A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD29;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD28;struct Cyc_Parse_Abstractdeclarator _tmpD27;yyval=Cyc_YY30(((_tmpD27.tms=((_tmpD2C=_region_malloc(yyr,sizeof(*_tmpD2C)),((_tmpD2C->hd=(void*)((_tmpD28=_region_malloc(yyr,sizeof(*_tmpD28)),((_tmpD28[0]=((_tmpD2B.tag=3,((_tmpD2B.f1=(void*)((_tmpD29=_region_malloc(yyr,sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2A.tag=1,((_tmpD2A.f1=_tmp6E3,((_tmpD2A.f2=_tmp6E4,((_tmpD2A.f3=_tmp6E5,((_tmpD2A.f4=_tmp6E6,((_tmpD2A.f5=_tmp6E7,_tmpD2A)))))))))))),_tmpD29)))),_tmpD2B)))),_tmpD28)))),((_tmpD2C->tl=0,_tmpD2C)))))),_tmpD27)));}
# 2382
break;}case 273: _LL333: {
# 2379 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2383
{struct Cyc_List_List*_tmpD41;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD40;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD3F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD3E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD3D;struct Cyc_Parse_Abstractdeclarator _tmpD3C;yyval=Cyc_YY30(((_tmpD3C.tms=((_tmpD41=_region_malloc(yyr,sizeof(*_tmpD41)),((_tmpD41->hd=(void*)((_tmpD3D=_region_malloc(yyr,sizeof(*_tmpD3D)),((_tmpD3D[0]=((_tmpD40.tag=3,((_tmpD40.f1=(void*)((_tmpD3E=_region_malloc(yyr,sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD3F.tag=1,((_tmpD3F.f1=0,((_tmpD3F.f2=0,((_tmpD3F.f3=0,((_tmpD3F.f4=Cyc_yyget_YY49(yyyyvsp[2]),((_tmpD3F.f5=Cyc_yyget_YY50(yyyyvsp[3]),_tmpD3F)))))))))))),_tmpD3E)))),_tmpD40)))),_tmpD3D)))),((_tmpD41->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD41)))))),_tmpD3C)));}
# 2386
break;}case 274: _LL334: {
# 2383 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2387
struct Cyc_List_List*_tmp6F5;int _tmp6F6;struct Cyc_Absyn_VarargInfo*_tmp6F7;void*_tmp6F8;struct Cyc_List_List*_tmp6F9;struct _tuple24*_tmp6F4=Cyc_yyget_YY39(yyyyvsp[2]);_tmp6F5=_tmp6F4->f1;_tmp6F6=_tmp6F4->f2;_tmp6F7=_tmp6F4->f3;_tmp6F8=_tmp6F4->f4;_tmp6F9=_tmp6F4->f5;
{struct Cyc_List_List*_tmpD56;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD55;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD54;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD53;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD52;struct Cyc_Parse_Abstractdeclarator _tmpD51;yyval=Cyc_YY30(((_tmpD51.tms=((_tmpD56=_region_malloc(yyr,sizeof(*_tmpD56)),((_tmpD56->hd=(void*)((_tmpD52=_region_malloc(yyr,sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD55.tag=3,((_tmpD55.f1=(void*)((_tmpD53=_region_malloc(yyr,sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD54.tag=1,((_tmpD54.f1=_tmp6F5,((_tmpD54.f2=_tmp6F6,((_tmpD54.f3=_tmp6F7,((_tmpD54.f4=_tmp6F8,((_tmpD54.f5=_tmp6F9,_tmpD54)))))))))))),_tmpD53)))),_tmpD55)))),_tmpD52)))),((_tmpD56->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD56)))))),_tmpD51)));}
# 2391
break;}case 275: _LL335: {
# 2389 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2393
struct Cyc_List_List*_tmp700=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD60;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD5F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD5E;struct Cyc_Parse_Abstractdeclarator _tmpD5D;yyval=Cyc_YY30(((_tmpD5D.tms=((_tmpD60=_region_malloc(yyr,sizeof(*_tmpD60)),((_tmpD60->hd=(void*)((_tmpD5E=_region_malloc(yyr,sizeof(*_tmpD5E)),((_tmpD5E[0]=((_tmpD5F.tag=4,((_tmpD5F.f1=_tmp700,((_tmpD5F.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD5F.f3=0,_tmpD5F)))))))),_tmpD5E)))),((_tmpD60->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD60)))))),_tmpD5D)));}
# 2397
break;}case 276: _LL336: {
# 2394 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2398
{struct Cyc_List_List*_tmpD6A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD69;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD68;struct Cyc_Parse_Abstractdeclarator _tmpD67;yyval=Cyc_YY30(((_tmpD67.tms=((_tmpD6A=_region_malloc(yyr,sizeof(*_tmpD6A)),((_tmpD6A->hd=(void*)((_tmpD68=_region_malloc(yyr,sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD69.tag=5,((_tmpD69.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD69.f2=Cyc_yyget_YY45(yyyyvsp[1]),_tmpD69)))))),_tmpD68)))),((_tmpD6A->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpD6A)))))),_tmpD67)));}
# 2400
break;}case 277: _LL337: {
# 2400 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2404
yyval=yyyyvsp[0];
break;}case 278: _LL338: {
# 2401 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2405
yyval=yyyyvsp[0];
break;}case 279: _LL339: {
# 2402 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2406
yyval=yyyyvsp[0];
break;}case 280: _LL33A: {
# 2403 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2407
yyval=yyyyvsp[0];
break;}case 281: _LL33B: {
# 2404 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2408
yyval=yyyyvsp[0];
break;}case 282: _LL33C: {
# 2405 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2409
yyval=yyyyvsp[0];
break;}case 283: _LL33D: {
# 2408 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2412
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD6D;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD6C;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=16,((_tmpD6D.f1=Cyc_yyget_YY3(yyyyvsp[2]),_tmpD6D)))),_tmpD6C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 284: _LL33E:
# 2412 "parse.y"
 yyval=Cyc_YY4(0);
break;case 285: _LL33F: {
# 2414 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2418
{const char*_tmpD6E;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD6E="open",_tag_dyneither(_tmpD6E,sizeof(char),5))))!= 0){
const char*_tmpD6F;Cyc_Parse_err(((_tmpD6F="expecting `open'",_tag_dyneither(_tmpD6F,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY4(Cyc_yyget_YY3(yyyyvsp[3]));
# 2422
break;}case 286: _LL340: {
# 2423 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2427
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD75;struct _dyneither_ptr*_tmpD74;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD73;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD75.tag=13,((_tmpD75.f1=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD74)))),((_tmpD75.f2=Cyc_yyget_YY9(yyyyvsp[2]),_tmpD75)))))),_tmpD73)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL341: {
# 2427 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2431
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 288: _LL342: {
# 2428 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2432
yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 289: _LL343: {
# 2433 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2437
yyval=Cyc_YY9(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL344: {
# 2434 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2438
yyval=yyyyvsp[1];
break;}case 291: _LL345: {
# 2439 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2443
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 292: _LL346: {
# 2440 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2444
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1])));
break;}case 293: _LL347: {
# 2441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2445
yyval=yyyyvsp[0];
break;}case 294: _LL348: {
# 2442 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2446
yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 295: _LL349: {
# 2443 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2447
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD78;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD77;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77[0]=((_tmpD78.tag=1,((_tmpD78.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpD78)))),_tmpD77)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 296: _LL34A: {
# 2446 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2450
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD7B;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD7A;yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7B.tag=1,((_tmpD7B.f1=Cyc_yyget_YY17(yyyyvsp[0]),_tmpD7B)))),_tmpD7A)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY9(yyyyvsp[1])));}
break;}case 297: _LL34B: {
# 2451 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2455
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 298: _LL34C: {
# 2453 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2457
yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL34D: {
# 2459 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2463
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL34E: {
# 2462 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2466
struct Cyc_Absyn_Exp*_tmp714=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp714,Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL34F: {
# 2466 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2470
struct Cyc_Absyn_Exp*_tmp715=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp715,Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2473
break;}case 302: _LL350: {
# 2472 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2476
yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL351:
# 2486 "parse.y"
 yyval=Cyc_YY10(0);
break;case 304: _LL352: {
# 2488 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2492
{struct Cyc_Absyn_Switch_clause*_tmpD7E;struct Cyc_List_List*_tmpD7D;yyval=Cyc_YY10(((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpD7E->pat_vars=0,((_tmpD7E->where_clause=0,((_tmpD7E->body=
Cyc_yyget_YY9(yyyyvsp[2]),((_tmpD7E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD7E)))))))))))),((_tmpD7D->tl=0,_tmpD7D)))))));}
# 2495
break;}case 305: _LL353: {
# 2492 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2496
{struct Cyc_Absyn_Switch_clause*_tmpD81;struct Cyc_List_List*_tmpD80;yyval=Cyc_YY10(((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->hd=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD81->pat_vars=0,((_tmpD81->where_clause=0,((_tmpD81->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpD81->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD81)))))))))))),((_tmpD80->tl=
Cyc_yyget_YY10(yyyyvsp[3]),_tmpD80)))))));}
break;}case 306: _LL354: {
# 2496 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2500
{struct Cyc_Absyn_Switch_clause*_tmpD84;struct Cyc_List_List*_tmpD83;yyval=Cyc_YY10(((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD84->pat_vars=0,((_tmpD84->where_clause=0,((_tmpD84->body=Cyc_yyget_YY9(yyyyvsp[3]),((_tmpD84->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD84)))))))))))),((_tmpD83->tl=Cyc_yyget_YY10(yyyyvsp[4]),_tmpD83)))))));}
break;}case 307: _LL355: {
# 2498 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2502
{struct Cyc_Absyn_Switch_clause*_tmpD87;struct Cyc_List_List*_tmpD86;yyval=Cyc_YY10(((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86->hd=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD87->pat_vars=0,((_tmpD87->where_clause=Cyc_yyget_YY3(yyyyvsp[3]),((_tmpD87->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpD87->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD87)))))))))))),((_tmpD86->tl=
Cyc_yyget_YY10(yyyyvsp[5]),_tmpD86)))))));}
break;}case 308: _LL356: {
# 2502 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2506
{struct Cyc_Absyn_Switch_clause*_tmpD8A;struct Cyc_List_List*_tmpD89;yyval=Cyc_YY10(((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->hd=((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->pattern=Cyc_yyget_YY11(yyyyvsp[1]),((_tmpD8A->pat_vars=0,((_tmpD8A->where_clause=Cyc_yyget_YY3(yyyyvsp[3]),((_tmpD8A->body=Cyc_yyget_YY9(yyyyvsp[5]),((_tmpD8A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD8A)))))))))))),((_tmpD89->tl=Cyc_yyget_YY10(yyyyvsp[6]),_tmpD89)))))));}
break;}case 309: _LL357: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2513
yyval=Cyc_YY9(Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY9(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 310: _LL358: {
# 2513 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2517
yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL359: {
# 2517 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2521
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL35A: {
# 2520 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2524
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL35B: {
# 2523 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2527
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL35C: {
# 2526 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2530
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL35D: {
# 2529 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2533
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL35E: {
# 2532 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2536
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL35F: {
# 2535 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2537
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2539
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL360: {
# 2538 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2542
yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),Cyc_yyget_YY3(yyyyvsp[6]),
Cyc_yyget_YY9(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL361: {
# 2541 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2545
struct Cyc_List_List*_tmp720=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp721=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp720,_tmp721));
# 2550
break;}case 320: _LL362: {
# 2547 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2549
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2551
struct Cyc_List_List*_tmp722=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp723=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp722,_tmp723));
# 2556
break;}case 321: _LL363: {
# 2553 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2555
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2557
struct Cyc_List_List*_tmp724=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp725=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyyyvsp[4]),
Cyc_yyget_YY9(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp724,_tmp725));
# 2562
break;}case 322: _LL364: {
# 2559 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2561
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2563
struct Cyc_List_List*_tmp726=Cyc_yyget_YY18(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp727=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyyyvsp[3]),Cyc_yyget_YY3(yyyyvsp[5]),
Cyc_yyget_YY9(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp726,_tmp727));
# 2568
break;}case 323: _LL365: {
# 2569 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2573
{struct _dyneither_ptr*_tmpD8B;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD8B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 324: _LL366: {
# 2570 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2574
yyval=Cyc_YY9(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL367: {
# 2571 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2573
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2575
yyval=Cyc_YY9(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL368: {
# 2572 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2576
yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL369: {
# 2573 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2575
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2577
yyval=Cyc_YY9(Cyc_Absyn_return_stmt(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL36A: {
# 2575 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2579
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL36B: {
# 2576 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2580
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL36C: {
# 2578 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2582
yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL36D: {
# 2587 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2591
yyval=yyyyvsp[0];
break;}case 332: _LL36E: {
# 2590 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2594
yyval=yyyyvsp[0];
break;}case 333: _LL36F: {
# 2592 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2596
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 334: _LL370: {
# 2595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599
yyval=yyyyvsp[0];
break;}case 335: _LL371: {
# 2597 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2601
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 336: _LL372: {
# 2600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604
yyval=yyyyvsp[0];
break;}case 337: _LL373: {
# 2602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 338: _LL374: {
# 2605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609
yyval=yyyyvsp[0];
break;}case 339: _LL375: {
# 2607 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL376: {
# 2610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614
yyval=yyyyvsp[0];
break;}case 341: _LL377: {
# 2612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL378: {
# 2615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2619
yyval=yyyyvsp[0];
break;}case 343: _LL379: {
# 2617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2621
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL37A: {
# 2620 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2624
yyval=yyyyvsp[0];
break;}case 345: _LL37B: {
# 2622 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2626
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL37C: {
# 2624 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2628
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL37D: {
# 2627 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2631
yyval=yyyyvsp[0];
break;}case 348: _LL37E: {
# 2629 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2633
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL37F: {
# 2631 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2635
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL380: {
# 2633 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2637
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL381: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2639
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL382: {
# 2638 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2642
yyval=yyyyvsp[0];
break;}case 353: _LL383: {
# 2640 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2644
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL384: {
# 2642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2646
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL385: {
# 2645 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2649
yyval=yyyyvsp[0];
break;}case 356: _LL386: {
# 2647 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2651
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL387: {
# 2649 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2653
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL388: {
# 2652 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2656
yyval=yyyyvsp[0];
break;}case 359: _LL389: {
# 2654 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2658
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL38A: {
# 2656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2660
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL38B: {
# 2658 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyyyvsp[0])),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL38C: {
# 2661 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2665
yyval=yyyyvsp[0];
break;}case 363: _LL38D: {
# 2663 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2667
void*_tmp729=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp729,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2670
break;}case 364: _LL38E: {
# 2669 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2673
yyval=yyyyvsp[0];
break;}case 365: _LL38F: {
# 2672 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2676
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
break;}case 366: _LL390: {
# 2674 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2678
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL391: {
# 2676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2680
void*_tmp72A=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp72A,(unsigned int)(yyyylsp[0]).first_line)));
# 2683
break;}case 368: _LL392: {
# 2680 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2684
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL393: {
# 2682 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2686
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpD91;struct _dyneither_ptr*_tmpD90;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpD8F;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyyyvsp[2])).f3,(void*)(
(_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F[0]=((_tmpD91.tag=0,((_tmpD91.f1=((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpD90)))),_tmpD91)))),_tmpD8F)))),(unsigned int)(yyyylsp[0]).first_line)));}
break;}case 370: _LL394: {
# 2686 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2690
void*_tmp72E=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpD94;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpD93;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp(_tmp72E,(void*)((_tmpD93=_cycalloc_atomic(sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD94.tag=1,((_tmpD94.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpD94)))),_tmpD93)))),(unsigned int)(yyyylsp[0]).first_line)));}
# 2693
break;}case 371: _LL395: {
# 2693 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2697
yyval=yyyyvsp[0];
break;}case 372: _LL396: {
# 2702 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2706
yyval=yyyyvsp[0];
break;}case 373: _LL397: {
# 2707 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2711
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 374: _LL398: {
# 2709 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2713
yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyyyvsp[1])));
break;}case 375: _LL399: {
# 2711 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2715
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyyyvsp[0]);
{void*_tmp731=e->r;enum Cyc_Absyn_Sign _tmp733;char _tmp734;enum Cyc_Absyn_Sign _tmp736;short _tmp737;enum Cyc_Absyn_Sign _tmp739;int _tmp73A;struct _dyneither_ptr _tmp73C;int _tmp73D;_LL39C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp732=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp732->tag != 0)goto _LL39E;else{if(((_tmp732->f1).Char_c).tag != 2)goto _LL39E;_tmp733=((struct _tuple3)((_tmp732->f1).Char_c).val).f1;_tmp734=((struct _tuple3)((_tmp732->f1).Char_c).val).f2;}}_LL39D:
# 2719
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpD97;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpD96;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD96=_cycalloc_atomic(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=10,((_tmpD97.f1=_tmp734,_tmpD97)))),_tmpD96)))),e->loc));}goto _LL39B;_LL39E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp735=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp735->tag != 0)goto _LL3A0;else{if(((_tmp735->f1).Short_c).tag != 4)goto _LL3A0;_tmp736=((struct _tuple4)((_tmp735->f1).Short_c).val).f1;_tmp737=((struct _tuple4)((_tmp735->f1).Short_c).val).f2;}}_LL39F:
# 2721
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD9A;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD99;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD99=_cycalloc_atomic(sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9A.tag=9,((_tmpD9A.f1=_tmp736,((_tmpD9A.f2=(int)_tmp737,_tmpD9A)))))),_tmpD99)))),e->loc));}goto _LL39B;_LL3A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp738=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp738->tag != 0)goto _LL3A2;else{if(((_tmp738->f1).Int_c).tag != 5)goto _LL3A2;_tmp739=((struct _tuple5)((_tmp738->f1).Int_c).val).f1;_tmp73A=((struct _tuple5)((_tmp738->f1).Int_c).val).f2;}}_LL3A1:
# 2723
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD9D;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD9C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD9C=_cycalloc_atomic(sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9D.tag=9,((_tmpD9D.f1=_tmp739,((_tmpD9D.f2=_tmp73A,_tmpD9D)))))),_tmpD9C)))),e->loc));}goto _LL39B;_LL3A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp73B->tag != 0)goto _LL3A4;else{if(((_tmp73B->f1).Float_c).tag != 7)goto _LL3A4;_tmp73C=((struct _tuple7)((_tmp73B->f1).Float_c).val).f1;_tmp73D=((struct _tuple7)((_tmp73B->f1).Float_c).val).f2;}}_LL3A3:
# 2725
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDA0;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpD9F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpDA0.tag=11,((_tmpDA0.f1=_tmp73C,((_tmpDA0.f2=_tmp73D,_tmpDA0)))))),_tmpD9F)))),e->loc));}goto _LL39B;_LL3A4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp73E->tag != 0)goto _LL3A6;else{if(((_tmp73E->f1).Null_c).tag != 1)goto _LL3A6;}}_LL3A5:
# 2727
 yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL39B;_LL3A6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp73F->tag != 0)goto _LL3A8;else{if(((_tmp73F->f1).String_c).tag != 8)goto _LL3A8;}}_LL3A7:
# 2729
{const char*_tmpDA1;Cyc_Parse_err(((_tmpDA1="strings cannot occur within patterns",_tag_dyneither(_tmpDA1,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39B;_LL3A8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp740=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp740->tag != 0)goto _LL3AA;else{if(((_tmp740->f1).Wstring_c).tag != 9)goto _LL3AA;}}_LL3A9:
# 2731
{const char*_tmpDA2;Cyc_Parse_err(((_tmpDA2="strings cannot occur within patterns",_tag_dyneither(_tmpDA2,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39B;_LL3AA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp741=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp731;if(_tmp741->tag != 0)goto _LL3AC;else{if(((_tmp741->f1).LongLong_c).tag != 6)goto _LL3AC;}}_LL3AB:
# 2733
{const char*_tmpDA3;Cyc_Parse_unimp(((_tmpDA3="long long's in patterns",_tag_dyneither(_tmpDA3,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL39B;_LL3AC:;_LL3AD: {
# 2735
const char*_tmpDA4;Cyc_Parse_err(((_tmpDA4="bad constant in case",_tag_dyneither(_tmpDA4,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL39B:;}
# 2738
break;}case 376: _LL39A: {
# 2735 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2739
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDA7;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDA6;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA7.tag=14,((_tmpDA7.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDA7)))),_tmpDA6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 377: _LL3AE: {
# 2737 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2741
{const char*_tmpDA8;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDA8="as",_tag_dyneither(_tmpDA8,sizeof(char),3))))!= 0){
const char*_tmpDA9;Cyc_Parse_err(((_tmpDA9="expecting `as'",_tag_dyneither(_tmpDA9,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDB3;struct _dyneither_ptr*_tmpDB2;struct _tuple0*_tmpDB1;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDB0;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDB3.tag=1,((_tmpDB3.f1=Cyc_Absyn_new_vardecl(((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->f1=Cyc_Absyn_Loc_n,((_tmpDB1->f2=((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDB2)))),_tmpDB1)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDB3.f2=
Cyc_yyget_YY11(yyyyvsp[2]),_tmpDB3)))))),_tmpDB0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2746
break;}case 378: _LL3AF: {
# 2743 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2747
struct Cyc_List_List*_tmp757;int _tmp758;struct _tuple21 _tmp756=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp757=_tmp756.f1;_tmp758=_tmp756.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDB6;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDB5;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=((_tmpDB6.tag=4,((_tmpDB6.f1=_tmp757,((_tmpDB6.f2=_tmp758,_tmpDB6)))))),_tmpDB5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2750
break;}case 379: _LL3B0: {
# 2747 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751
struct Cyc_List_List*_tmp75C;int _tmp75D;struct _tuple21 _tmp75B=*Cyc_yyget_YY12(yyyyvsp[2]);_tmp75C=_tmp75B.f1;_tmp75D=_tmp75B.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDB9;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDB8;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=15,((_tmpDB9.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDB9.f2=_tmp75C,((_tmpDB9.f3=_tmp75D,_tmpDB9)))))))),_tmpDB8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2754
break;}case 380: _LL3B1: {
# 2751 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2755
struct Cyc_List_List*_tmp761;int _tmp762;struct _tuple21 _tmp760=*Cyc_yyget_YY16(yyyyvsp[3]);_tmp761=_tmp760.f1;_tmp762=_tmp760.f2;{
struct Cyc_List_List*_tmp763=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY40(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDBF;struct Cyc_Absyn_AggrInfo*_tmpDBE;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDBD;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBF.tag=6,((_tmpDBF.f1=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDBE->targs=0,_tmpDBE)))))),((_tmpDBF.f2=_tmp763,((_tmpDBF.f3=_tmp761,((_tmpDBF.f4=_tmp762,_tmpDBF)))))))))),_tmpDBD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2760
break;};}case 381: _LL3B2: {
# 2757 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2761
struct Cyc_List_List*_tmp768;int _tmp769;struct _tuple21 _tmp767=*Cyc_yyget_YY16(yyyyvsp[2]);_tmp768=_tmp767.f1;_tmp769=_tmp767.f2;{
struct Cyc_List_List*_tmp76A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY40(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDC2;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDC1;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=6,((_tmpDC2.f1=0,((_tmpDC2.f2=_tmp76A,((_tmpDC2.f3=_tmp768,((_tmpDC2.f4=_tmp769,_tmpDC2)))))))))),_tmpDC1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2765
break;};}case 382: _LL3B3: {
# 2762 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2766
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC5;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC4;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=5,((_tmpDC5.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpDC5)))),_tmpDC4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3B4: {
# 2764 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2768
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDCF;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDCE;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDCD;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDCC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCF.tag=5,((_tmpDCF.f1=Cyc_Absyn_new_pat((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCE.tag=5,((_tmpDCE.f1=Cyc_yyget_YY11(yyyyvsp[1]),_tmpDCE)))),_tmpDCD)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDCF)))),_tmpDCC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 384: _LL3B5: {
# 2766 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2770
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDD9;struct _dyneither_ptr*_tmpDD8;struct _tuple0*_tmpDD7;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDD6;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD9.tag=2,((_tmpDD9.f1=Cyc_Absyn_new_vardecl(((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->f1=Cyc_Absyn_Loc_n,((_tmpDD7->f2=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDD8)))),_tmpDD7)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD9.f2=
# 2772
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDD9)))))),_tmpDD6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2774
break;}case 385: _LL3B6: {
# 2771 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2775
{const char*_tmpDDA;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDDA="as",_tag_dyneither(_tmpDDA,sizeof(char),3))))!= 0){
const char*_tmpDDB;Cyc_Parse_err(((_tmpDDB="expecting `as'",_tag_dyneither(_tmpDDB,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDE5;struct _dyneither_ptr*_tmpDE4;struct _tuple0*_tmpDE3;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDE2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE5.tag=2,((_tmpDE5.f1=Cyc_Absyn_new_vardecl(((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->f1=Cyc_Absyn_Loc_n,((_tmpDE3->f2=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDE4)))),_tmpDE3)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDE5.f2=
# 2779
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDE5)))))),_tmpDE2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2781
break;}case 386: _LL3B7: {
# 2778 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2782
void*_tmp77D=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpDF6;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDF5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDF4;struct _dyneither_ptr*_tmpDF3;struct _tuple0*_tmpDF2;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpDF1;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF6.tag=3,((_tmpDF6.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp77D),((_tmpDF6.f2=
Cyc_Absyn_new_vardecl(((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->f1=Cyc_Absyn_Loc_n,((_tmpDF2->f2=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF3)))),_tmpDF2)))))),(void*)(
(_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=19,((_tmpDF5.f1=_tmp77D,_tmpDF5)))),_tmpDF4)))),0),_tmpDF6)))))),_tmpDF1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2787
break;}case 387: _LL3B8: {
# 2784 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2788
struct Cyc_Absyn_Tvar*_tmp784=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE12;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE11;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE10;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE0F;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE0E;struct _dyneither_ptr*_tmpE0D;struct _tuple0*_tmpE0C;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE0B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE12.tag=3,((_tmpE12.f1=_tmp784,((_tmpE12.f2=
Cyc_Absyn_new_vardecl(((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->f1=Cyc_Absyn_Loc_n,((_tmpE0C->f2=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE0D)))),_tmpE0C)))))),(void*)(
(_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE11.tag=19,((_tmpE11.f1=(void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE10.tag=2,((_tmpE10.f1=_tmp784,_tmpE10)))),_tmpE0F)))),_tmpE11)))),_tmpE0E)))),0),_tmpE12)))))),_tmpE0B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2793
break;}case 388: _LL3B9: {
# 2792 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2796
{struct _tuple21*_tmpE13;yyval=Cyc_YY12(((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE13->f2=0,_tmpE13)))))));}
break;}case 389: _LL3BA: {
# 2793 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2797
{struct _tuple21*_tmpE14;yyval=Cyc_YY12(((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0])),((_tmpE14->f2=1,_tmpE14)))))));}
break;}case 390: _LL3BB: {
# 2794 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2798
{struct _tuple21*_tmpE15;yyval=Cyc_YY12(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=0,((_tmpE15->f2=1,_tmpE15)))))));}
break;}case 391: _LL3BC: {
# 2799 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2803
{struct Cyc_List_List*_tmpE16;yyval=Cyc_YY13(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->hd=Cyc_yyget_YY11(yyyyvsp[0]),((_tmpE16->tl=0,_tmpE16)))))));}
break;}case 392: _LL3BD: {
# 2801 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2805
{struct Cyc_List_List*_tmpE17;yyval=Cyc_YY13(((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->hd=Cyc_yyget_YY11(yyyyvsp[2]),((_tmpE17->tl=Cyc_yyget_YY13(yyyyvsp[0]),_tmpE17)))))));}
break;}case 393: _LL3BE: {
# 2806 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2810
{struct _tuple22*_tmpE18;yyval=Cyc_YY14(((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->f1=0,((_tmpE18->f2=Cyc_yyget_YY11(yyyyvsp[0]),_tmpE18)))))));}
break;}case 394: _LL3BF: {
# 2808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2812
{struct _tuple22*_tmpE19;yyval=Cyc_YY14(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->f1=Cyc_yyget_YY41(yyyyvsp[0]),((_tmpE19->f2=Cyc_yyget_YY11(yyyyvsp[1]),_tmpE19)))))));}
break;}case 395: _LL3C0: {
# 2811 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2815
{struct _tuple21*_tmpE1A;yyval=Cyc_YY16(((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE1A->f2=0,_tmpE1A)))))));}
break;}case 396: _LL3C1: {
# 2812 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2816
{struct _tuple21*_tmpE1B;yyval=Cyc_YY16(((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyyyvsp[0])),((_tmpE1B->f2=1,_tmpE1B)))))));}
break;}case 397: _LL3C2: {
# 2813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2817
{struct _tuple21*_tmpE1C;yyval=Cyc_YY16(((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->f1=0,((_tmpE1C->f2=1,_tmpE1C)))))));}
break;}case 398: _LL3C3: {
# 2818 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2822
{struct Cyc_List_List*_tmpE1D;yyval=Cyc_YY15(((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->hd=Cyc_yyget_YY14(yyyyvsp[0]),((_tmpE1D->tl=0,_tmpE1D)))))));}
break;}case 399: _LL3C4: {
# 2820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2824
{struct Cyc_List_List*_tmpE1E;yyval=Cyc_YY15(((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->hd=Cyc_yyget_YY14(yyyyvsp[2]),((_tmpE1E->tl=Cyc_yyget_YY15(yyyyvsp[0]),_tmpE1E)))))));}
break;}case 400: _LL3C5: {
# 2826 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2830
yyval=yyyyvsp[0];
break;}case 401: _LL3C6: {
# 2828 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2832
yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 402: _LL3C7: {
# 2833 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2837
yyval=yyyyvsp[0];
break;}case 403: _LL3C8: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2839
yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY8(yyyyvsp[1]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 404: _LL3C9: {
# 2837 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2841
yyval=Cyc_YY3(Cyc_Absyn_swap_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 405: _LL3CA: {
# 2841 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2845
yyval=Cyc_YY8(0);
break;}case 406: _LL3CB: {
# 2842 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2846
{struct Cyc_Core_Opt*_tmpE1F;yyval=Cyc_YY8(((_tmpE1F=_cycalloc_atomic(sizeof(*_tmpE1F)),((_tmpE1F->v=(void*)Cyc_Absyn_Times,_tmpE1F)))));}
break;}case 407: _LL3CC: {
# 2843 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2847
{struct Cyc_Core_Opt*_tmpE20;yyval=Cyc_YY8(((_tmpE20=_cycalloc_atomic(sizeof(*_tmpE20)),((_tmpE20->v=(void*)Cyc_Absyn_Div,_tmpE20)))));}
break;}case 408: _LL3CD: {
# 2844 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848
{struct Cyc_Core_Opt*_tmpE21;yyval=Cyc_YY8(((_tmpE21=_cycalloc_atomic(sizeof(*_tmpE21)),((_tmpE21->v=(void*)Cyc_Absyn_Mod,_tmpE21)))));}
break;}case 409: _LL3CE: {
# 2845 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849
{struct Cyc_Core_Opt*_tmpE22;yyval=Cyc_YY8(((_tmpE22=_cycalloc_atomic(sizeof(*_tmpE22)),((_tmpE22->v=(void*)Cyc_Absyn_Plus,_tmpE22)))));}
break;}case 410: _LL3CF: {
# 2846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850
{struct Cyc_Core_Opt*_tmpE23;yyval=Cyc_YY8(((_tmpE23=_cycalloc_atomic(sizeof(*_tmpE23)),((_tmpE23->v=(void*)Cyc_Absyn_Minus,_tmpE23)))));}
break;}case 411: _LL3D0: {
# 2847 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851
{struct Cyc_Core_Opt*_tmpE24;yyval=Cyc_YY8(((_tmpE24=_cycalloc_atomic(sizeof(*_tmpE24)),((_tmpE24->v=(void*)Cyc_Absyn_Bitlshift,_tmpE24)))));}
break;}case 412: _LL3D1: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852
{struct Cyc_Core_Opt*_tmpE25;yyval=Cyc_YY8(((_tmpE25=_cycalloc_atomic(sizeof(*_tmpE25)),((_tmpE25->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE25)))));}
break;}case 413: _LL3D2: {
# 2849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853
{struct Cyc_Core_Opt*_tmpE26;yyval=Cyc_YY8(((_tmpE26=_cycalloc_atomic(sizeof(*_tmpE26)),((_tmpE26->v=(void*)Cyc_Absyn_Bitand,_tmpE26)))));}
break;}case 414: _LL3D3: {
# 2850 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854
{struct Cyc_Core_Opt*_tmpE27;yyval=Cyc_YY8(((_tmpE27=_cycalloc_atomic(sizeof(*_tmpE27)),((_tmpE27->v=(void*)Cyc_Absyn_Bitxor,_tmpE27)))));}
break;}case 415: _LL3D4: {
# 2851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855
{struct Cyc_Core_Opt*_tmpE28;yyval=Cyc_YY8(((_tmpE28=_cycalloc_atomic(sizeof(*_tmpE28)),((_tmpE28->v=(void*)Cyc_Absyn_Bitor,_tmpE28)))));}
break;}case 416: _LL3D5: {
# 2856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2860
yyval=yyyyvsp[0];
break;}case 417: _LL3D6: {
# 2858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2862
yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 418: _LL3D7: {
# 2861 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2865
yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3D8: {
# 2864 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2868
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 420: _LL3D9: {
# 2866 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2870
yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3DA: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2872
yyval=Cyc_YY3(Cyc_Absyn_New_exp(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3DB: {
# 2870 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2874
yyval=Cyc_YY3(Cyc_Absyn_New_exp(Cyc_yyget_YY3(yyyyvsp[2]),Cyc_yyget_YY3(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3DC: {
# 2874 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2878
yyval=yyyyvsp[0];
break;}case 424: _LL3DD: {
# 2878 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2882
yyval=yyyyvsp[0];
break;}case 425: _LL3DE: {
# 2880 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2884
yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3DF: {
# 2884 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2888
yyval=yyyyvsp[0];
break;}case 427: _LL3E0: {
# 2886 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2890
yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E1: {
# 2890 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2894
yyval=yyyyvsp[0];
break;}case 429: _LL3E2: {
# 2892 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2896
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3E3: {
# 2896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2900
yyval=yyyyvsp[0];
break;}case 431: _LL3E4: {
# 2898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2902
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3E5: {
# 2902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2906
yyval=yyyyvsp[0];
break;}case 433: _LL3E6: {
# 2904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2908
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 434: _LL3E7: {
# 2908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2912
yyval=yyyyvsp[0];
break;}case 435: _LL3E8: {
# 2910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2914
yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3E9: {
# 2912 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2916
yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3EA: {
# 2916 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2920
yyval=yyyyvsp[0];
break;}case 438: _LL3EB: {
# 2918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2922
yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3EC: {
# 2920 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2924
yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3ED: {
# 2922 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926
yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3EE: {
# 2924 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2928
yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3EF: {
# 2928 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2932
yyval=yyyyvsp[0];
break;}case 443: _LL3F0: {
# 2930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2934
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F1: {
# 2932 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2936
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3F2: {
# 2936 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2940
yyval=yyyyvsp[0];
break;}case 446: _LL3F3: {
# 2938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2942
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F4: {
# 2940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2944
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3F5: {
# 2944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2948
yyval=yyyyvsp[0];
break;}case 449: _LL3F6: {
# 2946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2950
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3F7: {
# 2948 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2952
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3F8: {
# 2950 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2954
yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3F9: {
# 2954 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2958
yyval=yyyyvsp[0];
break;}case 453: _LL3FA: {
# 2956 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2960
void*_tmp7A3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_cast_exp(_tmp7A3,Cyc_yyget_YY3(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2963
break;}case 454: _LL3FB: {
# 2962 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2966
yyval=yyyyvsp[0];
break;}case 455: _LL3FC: {
# 2963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2967
yyval=Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL3FD: {
# 2964 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2968
yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL3FE: {
# 2965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2969
yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL3FF: {
# 2966 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970
yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL400: {
# 2967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971
yyval=yyyyvsp[1];
break;}case 460: _LL401: {
# 2968 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2972
yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL402: {
# 2970 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2974
void*_tmp7A4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp(_tmp7A4,(unsigned int)(yyyylsp[0]).first_line));
# 2977
break;}case 462: _LL403: {
# 2973 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2977
yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL404: {
# 2975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2979
void*_tmp7A5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE2E;struct _dyneither_ptr*_tmpE2D;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE2C;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7A5,(void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2E.tag=0,((_tmpE2E.f1=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE2D)))),_tmpE2E)))),_tmpE2C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2982
break;}case 464: _LL405: {
# 2980 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2984
void*_tmp7A9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE31;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE30;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp(_tmp7A9,(void*)((_tmpE30=_cycalloc_atomic(sizeof(*_tmpE30)),((_tmpE30[0]=((_tmpE31.tag=1,((_tmpE31.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[4])).f2,_tmpE31)))),_tmpE30)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2987
break;}case 465: _LL406: {
# 2985 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2989
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE37;struct Cyc_Absyn_MallocInfo _tmpE36;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE35;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35[0]=((_tmpE37.tag=32,((_tmpE37.f1=((_tmpE36.is_calloc=0,((_tmpE36.rgn=0,((_tmpE36.elt_type=0,((_tmpE36.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE36.fat_result=0,_tmpE36)))))))))),_tmpE37)))),_tmpE35)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2991
break;}case 466: _LL407: {
# 2988 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2992
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE3D;struct Cyc_Absyn_MallocInfo _tmpE3C;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE3B;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B[0]=((_tmpE3D.tag=32,((_tmpE3D.f1=((_tmpE3C.is_calloc=0,((_tmpE3C.rgn=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE3C.elt_type=0,((_tmpE3C.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE3C.fat_result=0,_tmpE3C)))))))))),_tmpE3D)))),_tmpE3B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2994
break;}case 467: _LL408: {
# 2991 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2995
void*_tmp7B2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE47;void**_tmpE46;struct Cyc_Absyn_MallocInfo _tmpE45;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE44;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE47.tag=32,((_tmpE47.f1=((_tmpE45.is_calloc=1,((_tmpE45.rgn=0,((_tmpE45.elt_type=((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=_tmp7B2,_tmpE46)))),((_tmpE45.num_elts=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE45.fat_result=0,_tmpE45)))))))))),_tmpE47)))),_tmpE44)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2998
break;}case 468: _LL409: {
# 2996 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3000
void*_tmp7B7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE51;void**_tmpE50;struct Cyc_Absyn_MallocInfo _tmpE4F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE4E;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE51.tag=32,((_tmpE51.f1=((_tmpE4F.is_calloc=1,((_tmpE4F.rgn=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE4F.elt_type=((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=_tmp7B7,_tmpE50)))),((_tmpE4F.num_elts=Cyc_yyget_YY3(yyyyvsp[4]),((_tmpE4F.fat_result=0,_tmpE4F)))))))))),_tmpE51)))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3003
break;}case 469: _LL40A: {
# 3000 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3004
{struct Cyc_Absyn_Exp*_tmpE52[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE52[0]=Cyc_yyget_YY3(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE52,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 470: _LL40B: {
# 3002 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3006
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE58;struct _dyneither_ptr*_tmpE57;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE56;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56[0]=((_tmpE58.tag=36,((_tmpE58.f1=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE58.f2=((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE57)))),_tmpE58)))))),_tmpE56)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 471: _LL40C: {
# 3004 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3008
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE5E;struct _dyneither_ptr*_tmpE5D;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE5C;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=((_tmpE5E.tag=36,((_tmpE5E.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE5E.f2=((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE5D)))),_tmpE5E)))))),_tmpE5C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 472: _LL40D: {
# 3006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3010
void*_tmp7C3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY37(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp7C3,(unsigned int)(yyyylsp[0]).first_line));
break;}case 473: _LL40E: {
# 3009 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3013
int _tmp7C5;struct _dyneither_ptr _tmp7C6;struct _tuple19 _tmp7C4=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp7C5=_tmp7C4.f1;_tmp7C6=_tmp7C4.f2;
yyval=Cyc_YY3(Cyc_Absyn_asm_exp(_tmp7C5,_tmp7C6,(unsigned int)(yyyylsp[0]).first_line));
break;}case 474: _LL40F: {
# 3014 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018
yyval=Cyc_YY7(Cyc_Absyn_Bitnot);
break;}case 475: _LL410: {
# 3015 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3019
yyval=Cyc_YY7(Cyc_Absyn_Not);
break;}case 476: _LL411: {
# 3016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3020
yyval=Cyc_YY7(Cyc_Absyn_Minus);
break;}case 477: _LL412: {
# 3021 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3025
yyval=yyyyvsp[0];
break;}case 478: _LL413: {
# 3023 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3027
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY3(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL414: {
# 3025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3029
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL415: {
# 3027 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3031
yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyyyvsp[0]),Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL416: {
# 3029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3033
{struct _dyneither_ptr*_tmpE5F;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE5F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 482: _LL417: {
# 3031 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3035
{struct _dyneither_ptr*_tmpE60;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE60)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 483: _LL418: {
# 3033 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3037
yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL419: {
# 3035 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3039
yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL41A: {
# 3037 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3041
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE63;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE62;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE63.tag=24,((_tmpE63.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpE63.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpE63)))))),_tmpE62)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL41B: {
# 3039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3043
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE66;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE65;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=((_tmpE66.tag=24,((_tmpE66.f1=Cyc_yyget_YY37(yyyyvsp[1]),((_tmpE66.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[4])),_tmpE66)))))),_tmpE65)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL41C: {
# 3045 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3049
yyval=Cyc_YY3(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL41D: {
# 3047 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3051
yyval=yyyyvsp[0];
break;}case 489: _LL41E: {
# 3049 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3053
yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL41F: {
# 3051 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3055
yyval=Cyc_YY3(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL420: {
# 3053 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3057
yyval=yyyyvsp[1];
break;}case 492: _LL421: {
# 3058 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3062
yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 493: _LL422: {
# 3060 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3064
yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL423: {
# 3063 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3067
yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 495: _LL424: {
# 3066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3070
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE69;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE68;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE69.tag=27,((_tmpE69.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE69.f2=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpE69.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[3])),((_tmpE69.f4=0,_tmpE69)))))))))),_tmpE68)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 496: _LL425: {
# 3069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3073
yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL426: {
# 3073 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3077
yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[0])));
break;}case 498: _LL427: {
# 3079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3083
{struct Cyc_List_List*_tmpE6A;yyval=Cyc_YY5(((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A->hd=Cyc_yyget_YY3(yyyyvsp[0]),((_tmpE6A->tl=0,_tmpE6A)))))));}
break;}case 499: _LL428: {
# 3081 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3085
{struct Cyc_List_List*_tmpE6B;yyval=Cyc_YY5(((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->hd=Cyc_yyget_YY3(yyyyvsp[2]),((_tmpE6B->tl=Cyc_yyget_YY5(yyyyvsp[0]),_tmpE6B)))))));}
break;}case 500: _LL429: {
# 3087 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3091
struct _tuple5 _tmp7D1=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY3(Cyc_Absyn_int_exp(_tmp7D1.f1,_tmp7D1.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL42A: {
# 3088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3092
yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL42B: {
# 3089 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3093
yyval=Cyc_YY3(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL42C: {
# 3090 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3095
struct _dyneither_ptr _tmp7D2=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7D2);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7D2,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3103
yyval=Cyc_YY3(Cyc_Absyn_float_exp(_tmp7D2,i,(unsigned int)(yyyylsp[0]).first_line));
# 3105
break;}case 504: _LL42D: {
# 3102 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3106
yyval=Cyc_YY3(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL42E: {
# 3106 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3110
{struct _dyneither_ptr*_tmpE6E;struct _tuple0*_tmpE6D;yyval=Cyc_QualId_tok(((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->f1=Cyc_Absyn_Rel_n(0),((_tmpE6D->f2=((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE6E)))),_tmpE6D)))))));}
break;}case 506: _LL42F: {
# 3107 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3111
yyval=yyyyvsp[0];
break;}case 507: _LL430: {
# 3110 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3114
{struct _dyneither_ptr*_tmpE71;struct _tuple0*_tmpE70;yyval=Cyc_QualId_tok(((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->f1=Cyc_Absyn_Rel_n(0),((_tmpE70->f2=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE71)))),_tmpE70)))))));}
break;}case 508: _LL431: {
# 3111 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3115
yyval=yyyyvsp[0];
break;}case 509: _LL432: {
# 3116 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120
yyval=yyyyvsp[0];
break;}case 510: _LL433: {
# 3117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121
yyval=yyyyvsp[0];
break;}case 511: _LL434: {
# 3120 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3124
yyval=yyyyvsp[0];
break;}case 512: _LL435: {
# 3121 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125
yyval=yyyyvsp[0];
break;}case 513: _LL436: {
# 3125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3130
break;}case 514: _LL437: {
# 3126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3130
yylex_buf->lex_curr_pos -=1;
break;}default: _LL438:
# 3135
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
if((yystate >= 0  && yystate <= 6407) && Cyc_yycheck[_check_known_subscript_notnull(6408,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6408,yystate)];else{
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
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,yystate)];
# 431
if(yyn > - 32768  && yyn < 6407){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6408,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)])+ 15,count ++);}
{unsigned int _tmpE76;unsigned int _tmpE75;struct _dyneither_ptr _tmpE74;char*_tmpE73;unsigned int _tmpE72;msg=((_tmpE72=(unsigned int)(sze + 15),((_tmpE73=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpE72 + 1)),((_tmpE74=_tag_dyneither(_tmpE73,sizeof(char),_tmpE72 + 1),((((_tmpE75=_tmpE72,((_tmpE77(& _tmpE76,& _tmpE75,& _tmpE73),_tmpE73[_tmpE75]=(char)0)))),_tmpE74))))))));}
{const char*_tmpE78;Cyc_strcpy(msg,((_tmpE78="parse error",_tag_dyneither(_tmpE78,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 289 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6408,x + yyn)]== x){
# 453
{const char*_tmpE7A;const char*_tmpE79;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpE7A=", expecting `",_tag_dyneither(_tmpE7A,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpE79=" or `",_tag_dyneither(_tmpE79,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(289,x)]);
{const char*_tmpE7B;Cyc_strcat(msg,((_tmpE7B="'",_tag_dyneither(_tmpE7B,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpE7C;Cyc_yyerror(((_tmpE7C="parse error",_tag_dyneither(_tmpE7C,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7E0=1;_npop_handler(0);return _tmp7E0;}
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
 if(yyssp_offset == 0){int _tmp7E1=1;_npop_handler(0);return _tmp7E1;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6407) || Cyc_yycheck[_check_known_subscript_notnull(6408,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6408,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1026){
int _tmp7E2=0;_npop_handler(0);return _tmp7E2;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3129 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp7EC=v;int _tmp7ED;char _tmp7EE;struct _dyneither_ptr _tmp7EF;union Cyc_Absyn_Nmspace _tmp7F0;struct _dyneither_ptr*_tmp7F1;_LL43B: if((_tmp7EC.Int_tok).tag != 1)goto _LL43D;_tmp7ED=((struct _tuple5)(_tmp7EC.Int_tok).val).f2;_LL43C:
{const char*_tmpE80;void*_tmpE7F[1];struct Cyc_Int_pa_PrintArg_struct _tmpE7E;(_tmpE7E.tag=1,((_tmpE7E.f1=(unsigned long)_tmp7ED,((_tmpE7F[0]=& _tmpE7E,Cyc_fprintf(Cyc_stderr,((_tmpE80="%d",_tag_dyneither(_tmpE80,sizeof(char),3))),_tag_dyneither(_tmpE7F,sizeof(void*),1)))))));}goto _LL43A;_LL43D: if((_tmp7EC.Char_tok).tag != 2)goto _LL43F;_tmp7EE=(char)(_tmp7EC.Char_tok).val;_LL43E:
{const char*_tmpE84;void*_tmpE83[1];struct Cyc_Int_pa_PrintArg_struct _tmpE82;(_tmpE82.tag=1,((_tmpE82.f1=(unsigned long)((int)_tmp7EE),((_tmpE83[0]=& _tmpE82,Cyc_fprintf(Cyc_stderr,((_tmpE84="%c",_tag_dyneither(_tmpE84,sizeof(char),3))),_tag_dyneither(_tmpE83,sizeof(void*),1)))))));}goto _LL43A;_LL43F: if((_tmp7EC.String_tok).tag != 3)goto _LL441;_tmp7EF=(struct _dyneither_ptr)(_tmp7EC.String_tok).val;_LL440:
{const char*_tmpE88;void*_tmpE87[1];struct Cyc_String_pa_PrintArg_struct _tmpE86;(_tmpE86.tag=0,((_tmpE86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7EF),((_tmpE87[0]=& _tmpE86,Cyc_fprintf(Cyc_stderr,((_tmpE88="\"%s\"",_tag_dyneither(_tmpE88,sizeof(char),5))),_tag_dyneither(_tmpE87,sizeof(void*),1)))))));}goto _LL43A;_LL441: if((_tmp7EC.QualId_tok).tag != 5)goto _LL443;_tmp7F0=((struct _tuple0*)(_tmp7EC.QualId_tok).val)->f1;_tmp7F1=((struct _tuple0*)(_tmp7EC.QualId_tok).val)->f2;_LL442: {
# 3135
struct Cyc_List_List*_tmp7FB=0;
{union Cyc_Absyn_Nmspace _tmp7FC=_tmp7F0;struct Cyc_List_List*_tmp7FD;struct Cyc_List_List*_tmp7FE;struct Cyc_List_List*_tmp7FF;_LL446: if((_tmp7FC.Rel_n).tag != 1)goto _LL448;_tmp7FD=(struct Cyc_List_List*)(_tmp7FC.Rel_n).val;_LL447:
 _tmp7FB=_tmp7FD;goto _LL445;_LL448: if((_tmp7FC.Abs_n).tag != 2)goto _LL44A;_tmp7FE=(struct Cyc_List_List*)(_tmp7FC.Abs_n).val;_LL449:
 _tmp7FB=_tmp7FE;goto _LL445;_LL44A: if((_tmp7FC.C_n).tag != 3)goto _LL44C;_tmp7FF=(struct Cyc_List_List*)(_tmp7FC.C_n).val;_LL44B:
 _tmp7FB=_tmp7FF;goto _LL445;_LL44C: if((_tmp7FC.Loc_n).tag != 4)goto _LL445;_LL44D:
 goto _LL445;_LL445:;}
# 3142
for(0;_tmp7FB != 0;_tmp7FB=_tmp7FB->tl){
const char*_tmpE8C;void*_tmpE8B[1];struct Cyc_String_pa_PrintArg_struct _tmpE8A;(_tmpE8A.tag=0,((_tmpE8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp7FB->hd)),((_tmpE8B[0]=& _tmpE8A,Cyc_fprintf(Cyc_stderr,((_tmpE8C="%s::",_tag_dyneither(_tmpE8C,sizeof(char),5))),_tag_dyneither(_tmpE8B,sizeof(void*),1)))))));}
{const char*_tmpE90;void*_tmpE8F[1];struct Cyc_String_pa_PrintArg_struct _tmpE8E;(_tmpE8E.tag=0,((_tmpE8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7F1),((_tmpE8F[0]=& _tmpE8E,Cyc_fprintf(Cyc_stderr,((_tmpE90="%s::",_tag_dyneither(_tmpE90,sizeof(char),5))),_tag_dyneither(_tmpE8F,sizeof(void*),1)))))));}
goto _LL43A;}_LL443:;_LL444:
{const char*_tmpE93;void*_tmpE92;(_tmpE92=0,Cyc_fprintf(Cyc_stderr,((_tmpE93="?",_tag_dyneither(_tmpE93,sizeof(char),2))),_tag_dyneither(_tmpE92,sizeof(void*),0)));}goto _LL43A;_LL43A:;}
# 3151
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp808=_new_region("yyr");struct _RegionHandle*yyr=& _tmp808;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp809=Cyc_Parse_parse_result;_npop_handler(0);return _tmp809;};
# 3154
;_pop_region(yyr);};}
