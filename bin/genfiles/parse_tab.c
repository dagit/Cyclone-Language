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
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 116
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 121
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 143
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 308 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);struct Cyc_PP_Ppstate;
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
{struct _tuple10*_tmp826;declarators=((_tmp826=_region_malloc(r,sizeof(*_tmp826)),((_tmp826->tl=declarators,((_tmp826->hd=_tmp1,_tmp826))))));}{
struct Cyc_List_List*_tmp827;exprs=((_tmp827=_region_malloc(r,sizeof(*_tmp827)),((_tmp827->hd=_tmp2,((_tmp827->tl=exprs,_tmp827))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp82A;struct Cyc_Position_Exit_exn_struct*_tmp829;(int)_throw((void*)((_tmp829=_cycalloc_atomic(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=Cyc_Position_Exit,_tmp82A)),_tmp829)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp82F;void*_tmp82E[2];struct Cyc_String_pa_PrintArg_struct _tmp82D;struct Cyc_String_pa_PrintArg_struct _tmp82C;(_tmp82C.tag=0,((_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp82D.tag=0,((_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp82E[0]=& _tmp82D,((_tmp82E[1]=& _tmp82C,Cyc_fprintf(Cyc_stderr,((_tmp82F="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp82F,sizeof(char),46))),_tag_dyneither(_tmp82E,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_warn(loc,((_tmp832="qualifier on type is ignored",_tag_dyneither(_tmp832,sizeof(char),29))),_tag_dyneither(_tmp831,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp835;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp834;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((_tmp835.tag=1,((_tmp835.f1=_tmp18,_tmp835)))),_tmp834)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp836;return(_tmp836.f1=nullable,((_tmp836.f2=bound,((_tmp836.f3=zeroterm,((_tmp836.f4=rgn,_tmp836)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp843;const char*_tmp842;void*_tmp841[1];struct Cyc_Int_pa_PrintArg_struct _tmp840;struct _tuple0*_tmp83F;return(_tmp83F=_cycalloc(sizeof(*_tmp83F)),((_tmp83F->f1=Cyc_Absyn_Rel_n(0),((_tmp83F->f2=(
(_tmp843=_cycalloc(sizeof(*_tmp843)),((_tmp843[0]=(struct _dyneither_ptr)((_tmp840.tag=1,((_tmp840.f1=(unsigned long)enum_counter ++,((_tmp841[0]=& _tmp840,Cyc_aprintf(((_tmp842="__anonymous_enum_%d__",_tag_dyneither(_tmp842,sizeof(char),22))),_tag_dyneither(_tmp841,sizeof(void*),1)))))))),_tmp843)))),_tmp83F)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp2A;struct _tuple17*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=(_tmp23->f2)->f1;_tmp2A=(_tmp23->f2)->f2;
if(_tmp27 != 0){
const char*_tmp844;Cyc_Parse_err(((_tmp844="bad type params in struct field",_tag_dyneither(_tmp844,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp845;Cyc_Parse_err(((_tmp845="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp845,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp846;return(_tmp846=_cycalloc(sizeof(*_tmp846)),((_tmp846->name=(*_tmp24).f2,((_tmp846->tq=_tmp25,((_tmp846->type=_tmp26,((_tmp846->width=_tmp29,((_tmp846->attributes=_tmp28,((_tmp846->requires_clause=_tmp2A,_tmp846)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp847;return(_tmp847.Signed_spec=0,((_tmp847.Unsigned_spec=0,((_tmp847.Short_spec=0,((_tmp847.Long_spec=0,((_tmp847.Long_Long_spec=0,((_tmp847.Valid_type_spec=0,((_tmp847.Type_spec=Cyc_Absyn_sint_typ,((_tmp847.loc=loc,_tmp847)))))))))))))));}
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
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 311 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp34=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_stmttmp2=(struct _tuple8*)x->hd;struct _tuple8*_tmp35=_stmttmp2;struct _dyneither_ptr*_tmp36;void*_tmp38;struct _dyneither_ptr _tmp39;void**_tmp3C;_LL12: _tmp36=_tmp35->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35->f3;if(_tmp37->tag != 19)goto _LL14;else{_tmp38=(void*)_tmp37->f1;}};if(!(_tmp36 != 0))goto _LL14;_LL13:
# 316
{void*_tmp3D=_tmp38;void**_tmp3F;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL1B;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}_LL1A: {
# 320
struct Cyc_String_pa_PrintArg_struct _tmp84F;void*_tmp84E[1];const char*_tmp84D;struct _dyneither_ptr*_tmp84C;struct _dyneither_ptr*nm=(_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=(struct _dyneither_ptr)((_tmp84F.tag=0,((_tmp84F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp36),((_tmp84E[0]=& _tmp84F,Cyc_aprintf(((_tmp84D="`%s",_tag_dyneither(_tmp84D,sizeof(char),4))),_tag_dyneither(_tmp84E,sizeof(void*),1)))))))),_tmp84C)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp85E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp85D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp85C;struct Cyc_Absyn_Tvar*_tmp85B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp85A;*_tmp3F=(void*)((_tmp85A=_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85E.tag=2,((_tmp85E.f1=((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B->name=nm,((_tmp85B->identity=- 1,((_tmp85B->kind=(void*)((_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D[0]=((_tmp85C.tag=0,((_tmp85C.f1=& Cyc_Tcutil_ik,_tmp85C)))),_tmp85D)))),_tmp85B)))))))),_tmp85E)))),_tmp85A))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp861;struct Cyc_List_List*_tmp860;_tmp34=((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860->hd=((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->f1=_tmp36,((_tmp861->f2=_tmp38,_tmp861)))))),((_tmp860->tl=_tmp34,_tmp860))))));}goto _LL11;_LL14: if(_tmp35->f1 == 0)goto _LL16;_tmp39=*_tmp35->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35->f3;if(_tmp3A->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3A->f1);if(_tmp3B->tag != 1)goto _LL16;else{_tmp3C=(void**)((void**)& _tmp3B->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp869;void*_tmp868[1];const char*_tmp867;struct _dyneither_ptr*_tmp866;struct _dyneither_ptr*nm=(_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866[0]=(struct _dyneither_ptr)((_tmp869.tag=0,((_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39),((_tmp868[0]=& _tmp869,Cyc_aprintf(((_tmp867="`%s",_tag_dyneither(_tmp867,sizeof(char),4))),_tag_dyneither(_tmp868,sizeof(void*),1)))))))),_tmp866)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp878;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp877;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp876;struct Cyc_Absyn_Tvar*_tmp875;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp874;*_tmp3C=(void*)((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874[0]=((_tmp878.tag=2,((_tmp878.f1=((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875->name=nm,((_tmp875->identity=- 1,((_tmp875->kind=(void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp876.tag=0,((_tmp876.f1=& Cyc_Tcutil_rk,_tmp876)))),_tmp877)))),_tmp875)))))))),_tmp878)))),_tmp874))));}
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
{struct _tuple18*_tmp87B;struct Cyc_List_List*_tmp87A;_tmp54=((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A->hd=((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp87B->f2=_tmp57,_tmp87B)))))),((_tmp87A->tl=_tmp54,_tmp87A))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp87E;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp87D;return Cyc_Absyn_new_exp((void*)((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87E.tag=38,((_tmp87E.f1=Cyc_Tcutil_copy_type(_tmp60),_tmp87E)))),_tmp87D)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp884;struct Cyc_Absyn_ArrayInfo _tmp883;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp882;return(void*)((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=((_tmp884.tag=8,((_tmp884.f1=((_tmp883.elt_type=_tmp72,((_tmp883.tq=_tmp66,((_tmp883.num_elts=nelts2,((_tmp883.zero_term=_tmp68,((_tmp883.zt_loc=_tmp69,_tmp883)))))))))),_tmp884)))),_tmp882))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp6A->tag != 5)goto _LL2C;else{_tmp6B=(_tmp6A->f1).elt_typ;_tmp6C=(_tmp6A->f1).elt_tq;_tmp6D=((_tmp6A->f1).ptr_atts).rgn;_tmp6E=((_tmp6A->f1).ptr_atts).nullable;_tmp6F=((_tmp6A->f1).ptr_atts).bounds;_tmp70=((_tmp6A->f1).ptr_atts).zero_term;_tmp71=((_tmp6A->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp76=Cyc_Parse_substitute_tags(tags,_tmp6B);
union Cyc_Absyn_Constraint*_tmp77=_tmp6F;
{union Cyc_Absyn_Constraint _stmttmp6=*_tmp6F;union Cyc_Absyn_Constraint _tmp78=_stmttmp6;struct Cyc_Absyn_Exp*_tmp7A;_LL2F: if((_tmp78.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp79=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp78.Eq_constr).val);if(_tmp79->tag != 1)goto _LL31;else{_tmp7A=_tmp79->f1;}};_LL30:
# 384
 _tmp7A=Cyc_Parse_substitute_tags_exp(tags,_tmp7A);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp88A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp889;union Cyc_Absyn_Constraint*_tmp888;_tmp77=((_tmp888=_cycalloc(sizeof(*_tmp888)),(((_tmp888->Eq_constr).val=(void*)((_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp889.tag=1,((_tmp889.f1=_tmp7A,_tmp889)))),_tmp88A)))),(((_tmp888->Eq_constr).tag=1,_tmp888))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp76 != _tmp6B  || _tmp77 != _tmp6F){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp890;struct Cyc_Absyn_PtrInfo _tmp88F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp88E;return(void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp890.tag=5,((_tmp890.f1=((_tmp88F.elt_typ=_tmp76,((_tmp88F.elt_tq=_tmp6C,((_tmp88F.ptr_atts=(((_tmp88F.ptr_atts).rgn=_tmp6D,(((_tmp88F.ptr_atts).nullable=_tmp6E,(((_tmp88F.ptr_atts).bounds=_tmp77,(((_tmp88F.ptr_atts).zero_term=_tmp70,(((_tmp88F.ptr_atts).ptrloc=_tmp71,_tmp88F.ptr_atts)))))))))),_tmp88F)))))),_tmp890)))),_tmp88E))));}
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
struct _tuple19*_tmp891;return(_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->f1=(*t).f2,((_tmp891->f2=(*t).f3,_tmp891)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_stmttmp7=d->r;void*_tmp82=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp84;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp83=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp82;if(_tmp83->tag != 0)goto _LL36;else{_tmp84=_tmp83->f1;}}_LL35:
 return _tmp84;_LL36:;_LL37: {
const char*_tmp894;void*_tmp893;(_tmp893=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp894="bad declaration in `forarray' statement",_tag_dyneither(_tmp894,sizeof(char),40))),_tag_dyneither(_tmp893,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp87=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp88=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp87;if(_tmp88->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp895;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp895="`H",_tag_dyneither(_tmp895,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp896;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp896="`U",_tag_dyneither(_tmp896,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp897;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp897="`RC",_tag_dyneither(_tmp897,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A1;struct _dyneither_ptr*_tmp8A0;struct Cyc_Absyn_Tvar*_tmp89F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89E;return(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp8A1.tag=2,((_tmp8A1.f1=((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F->name=((_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0[0]=s,_tmp8A0)))),((_tmp89F->identity=- 1,((_tmp89F->kind=k,_tmp89F)))))))),_tmp8A1)))),_tmp89E))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_stmttmp8=Cyc_Absyn_compress_kb(t->kind);void*_tmp90=_stmttmp8;struct Cyc_Absyn_Kind*_tmp93;void*_tmp94;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp91=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp91->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8A4;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8A3;k=(void*)((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3[0]=((_tmp8A4.tag=1,((_tmp8A4.f1=0,_tmp8A4)))),_tmp8A3))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp92->tag != 2)goto _LL42;else{_tmp93=_tmp92->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8A7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A6;k=(void*)((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A7.tag=2,((_tmp8A7.f1=0,((_tmp8A7.f2=_tmp93,_tmp8A7)))))),_tmp8A6))));}goto _LL3D;_LL42: _tmp94=_tmp90;_LL43:
 k=_tmp94;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8A8;return(_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8->name=t->name,((_tmp8A8->identity=- 1,((_tmp8A8->kind=k,_tmp8A8)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp9A=t;struct Cyc_Absyn_Tvar*_tmp9C;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f1;}}_LL46:
 return _tmp9C;_LL47:;_LL48: {
const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8AB="expecting a list of type variables, not types",_tag_dyneither(_tmp8AB,sizeof(char),46))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8AE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AD;return(void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AE.tag=2,((_tmp8AE.f1=pr,_tmp8AE)))),_tmp8AD))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_stmttmp9=Cyc_Tcutil_compress(t);void*_tmpA1=_stmttmp9;void**_tmpA3;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA1;if(_tmpA2->tag != 2)goto _LL4C;else{_tmpA3=(void**)&(_tmpA2->f1)->kind;}}_LL4B: {
# 468
void*_stmttmpA=Cyc_Absyn_compress_kb(*_tmpA3);void*_tmpA4=_stmttmpA;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA4;if(_tmpA5->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpA3=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8B1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8B0;*_tmpA3=(void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=2,((_tmp8B1.f1=0,((_tmp8B1.f2=k,_tmp8B1)))))),_tmp8B0))));}
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
{const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,Cyc_Tcutil_warn(loc,((_tmp8B4="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8B4,sizeof(char),93))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpAD=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpAB;if(_tmpAD->tag != 0)goto _LL58;else{_tmpAE=_tmpAD->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8B7;void*_tmp8B6;(_tmp8B6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B7="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8B7,sizeof(char),73))),_tag_dyneither(_tmp8B6,sizeof(void*),0)));}{
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
const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BA="initializer found in parameter declaration",_tag_dyneither(_tmp8BA,sizeof(char),43))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpB7->name)){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BD="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8BD,sizeof(char),47))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}
{struct _tuple8*_tmp8C0;struct Cyc_List_List*_tmp8BF;rev_new_params=(
(_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF->hd=((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0->f1=(*_tmpB7->name).f2,((_tmp8C0->f2=_tmpB7->tq,((_tmp8C0->f3=_tmpB7->type,_tmp8C0)))))))),((_tmp8BF->tl=rev_new_params,_tmp8BF))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8C3="nonvariable declaration in parameter type",_tag_dyneither(_tmp8C3,sizeof(char),42))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpB3 == 0){
const char*_tmp8C7;void*_tmp8C6[1];struct Cyc_String_pa_PrintArg_struct _tmp8C5;(_tmp8C5.tag=0,((_tmp8C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpAE->hd)),((_tmp8C6[0]=& _tmp8C5,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8C7="%s is not given a type",_tag_dyneither(_tmp8C7,sizeof(char),23))),_tag_dyneither(_tmp8C6,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8D5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8D4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8D3;struct Cyc_List_List*_tmp8D2;return
(_tmp8D2=_region_malloc(yy,sizeof(*_tmp8D2)),((_tmp8D2->hd=(void*)((_tmp8D6=_region_malloc(yy,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D3.tag=3,((_tmp8D3.f1=(void*)((_tmp8D5=_region_malloc(yy,sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D4.tag=1,((_tmp8D4.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8D4.f2=0,((_tmp8D4.f3=0,((_tmp8D4.f4=0,((_tmp8D4.f5=0,_tmp8D4)))))))))))),_tmp8D5)))),_tmp8D3)))),_tmp8D6)))),((_tmp8D2->tl=0,_tmp8D2)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp8D7;return(_tmp8D7=_region_malloc(yy,sizeof(*_tmp8D7)),((_tmp8D7->hd=(void*)tms->hd,((_tmp8D7->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8D7)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8D8;d=((_tmp8D8.id=d.id,((_tmp8D8.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8D8))));}{
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
{const char*_tmp8D9;Cyc_Parse_err(((_tmp8D9="bad storage class on function",_tag_dyneither(_tmp8D9,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpCB=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _stmttmpE=Cyc_Parse_apply_tms(tq,_tmpCB,atts,d.tms);struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct _tuple13 _tmpCC=_stmttmpE;_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;
# 578
if(_tmpCF != 0){
# 581
const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,Cyc_Tcutil_warn(loc,((_tmp8DC="bad type params, ignoring",_tag_dyneither(_tmp8DC,sizeof(char),26))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}{
# 583
void*_tmpD3=_tmpCE;struct Cyc_List_List*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Tqual _tmpD7;void*_tmpD8;struct Cyc_List_List*_tmpD9;int _tmpDA;struct Cyc_Absyn_VarargInfo*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*_tmpDD;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD3;if(_tmpD4->tag != 9)goto _LL69;else{_tmpD5=(_tmpD4->f1).tvars;_tmpD6=(_tmpD4->f1).effect;_tmpD7=(_tmpD4->f1).ret_tqual;_tmpD8=(_tmpD4->f1).ret_typ;_tmpD9=(_tmpD4->f1).args;_tmpDA=(_tmpD4->f1).c_varargs;_tmpDB=(_tmpD4->f1).cyc_varargs;_tmpDC=(_tmpD4->f1).rgn_po;_tmpDD=(_tmpD4->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpDE=0;
{struct Cyc_List_List*_tmpDF=_tmpD9;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
struct _tuple8*_stmttmpF=(struct _tuple8*)_tmpDF->hd;struct _dyneither_ptr*_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;void*_tmpE3;struct _tuple8*_tmpE0=_stmttmpF;_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;_tmpE3=_tmpE0->f3;
if(_tmpE1 == 0){
{const char*_tmp8DD;Cyc_Parse_err(((_tmp8DD="missing argument variable in function prototype",_tag_dyneither(_tmp8DD,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8E0;struct Cyc_List_List*_tmp8DF;_tmpDE=((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF->hd=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8E0->f2=_tmpE2,((_tmp8E0->f3=_tmpE3,_tmp8E0)))))))),((_tmp8DF->tl=_tmpDE,_tmp8DF))))));};}else{
# 593
struct _tuple8*_tmp8E3;struct Cyc_List_List*_tmp8E2;_tmpDE=((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2->hd=((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->f1=_tmpE1,((_tmp8E3->f2=_tmpE2,((_tmp8E3->f3=_tmpE3,_tmp8E3)))))))),((_tmp8E2->tl=_tmpDE,_tmp8E2))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp8E4;return(_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4->sc=sc,((_tmp8E4->name=d.id,((_tmp8E4->tvs=_tmpD5,((_tmp8E4->is_inline=is_inline,((_tmp8E4->effect=_tmpD6,((_tmp8E4->ret_tqual=_tmpD7,((_tmp8E4->ret_type=_tmpD8,((_tmp8E4->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDE),((_tmp8E4->c_varargs=_tmpDA,((_tmp8E4->cyc_varargs=_tmpDB,((_tmp8E4->rgn_po=_tmpDC,((_tmp8E4->body=body,((_tmp8E4->cached_typ=0,((_tmp8E4->param_vardecls=0,((_tmp8E4->fn_vardecl=0,((_tmp8E4->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpDD,_tmpD0),_tmp8E4)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E7="declarator is not a function prototype",_tag_dyneither(_tmp8E7,sizeof(char),39))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}_LL66:;};};};}static char _tmpED[52]="at most one type may appear within a type specifier";
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
void*_tmp8E8;(_tmp8E8=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8E8,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8E9;(_tmp8E9=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8E9,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
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
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_warn(loc,((_tmp8EE="missing type within specifier",_tag_dyneither(_tmp8EE,sizeof(char),30))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmpF7=t;enum Cyc_Absyn_Sign _tmpF9;enum Cyc_Absyn_Size_of _tmpFA;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF7;if(_tmpF8->tag != 6)goto _LL6E;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LL6D:
# 686
 if(_tmpF9 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpFA);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp8EF;Cyc_Parse_err(((_tmp8EF="sign specification on non-integral type",_tag_dyneither(_tmp8EF,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmpFC=t;enum Cyc_Absyn_Sign _tmpFE;enum Cyc_Absyn_Size_of _tmpFF;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFC;if(_tmpFD->tag != 6)goto _LL73;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}}_LL72:
# 694
 if(_tmpFF != sz)
t=Cyc_Absyn_int_typ(_tmpFE,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFC;if(_tmp100->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp8F0;Cyc_Parse_err(((_tmp8F0="size qualifier on non-integral type",_tag_dyneither(_tmp8F0,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp8F3;struct Cyc_List_List*_tmp8F2;return(_tmp8F2=_region_malloc(r,sizeof(*_tmp8F2)),((_tmp8F2->hd=((_tmp8F3=_region_malloc(r,sizeof(*_tmp8F3)),((_tmp8F3->f1=_tmp102,((_tmp8F3->f2=_tmp104,((_tmp8F3->f3=_tmp105,((_tmp8F3->f4=_tmp106,((_tmp8F3->f5=_tmp107,_tmp8F3)))))))))))),((_tmp8F2->tl=0,_tmp8F2)))));}else{
# 718
struct _tuple15*_tmp8F6;struct Cyc_List_List*_tmp8F5;return(_tmp8F5=_region_malloc(r,sizeof(*_tmp8F5)),((_tmp8F5->hd=((_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6->f1=_tmp102,((_tmp8F6->f2=_tmp104,((_tmp8F6->f3=_tmp105,((_tmp8F6->f4=_tmp106,((_tmp8F6->f5=_tmp107,_tmp8F6)))))))))))),((_tmp8F5->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8F5)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp8F7;return(_tmp8F7.f1=tq,((_tmp8F7.f2=t,((_tmp8F7.f3=0,((_tmp8F7.f4=atts,_tmp8F7)))))));}{
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
struct Cyc_List_List*_tmp8F8;fn_atts=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->hd=(void*)as->hd,((_tmp8F8->tl=fn_atts,_tmp8F8))))));}else{
# 743
struct Cyc_List_List*_tmp8F9;new_atts=((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->hd=(void*)as->hd,((_tmp8F9->tl=new_atts,_tmp8F9))))));}}}
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
const char*_tmp8FC;void*_tmp8FB;(_tmp8FB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp128,((_tmp8FC="function declaration without parameter types",_tag_dyneither(_tmp8FC,sizeof(char),45))),_tag_dyneither(_tmp8FB,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp117->tag != 4)goto _LL80;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp8FD;return(_tmp8FD.f1=tq,((_tmp8FD.f2=t,((_tmp8FD.f3=_tmp118,((_tmp8FD.f4=atts,_tmp8FD)))))));}{
# 802
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp119,((_tmp900="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp900,sizeof(char),68))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11A->tag != 2)goto _LL82;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp906;struct Cyc_Absyn_PtrInfo _tmp905;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp904;return Cyc_Parse_apply_tms(_tmp11C,(void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp906.tag=5,((_tmp906.f1=((_tmp905.elt_typ=t,((_tmp905.elt_tq=tq,((_tmp905.ptr_atts=_tmp11B,_tmp905)))))),_tmp906)))),_tmp904)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp11D=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL77;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp90C;struct Cyc_Absyn_Typedefdecl*_tmp90B;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp90A;return Cyc_Absyn_new_decl((void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90C.tag=9,((_tmp90C.f1=((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B->name=_tmp13D,((_tmp90B->tvs=_tmp140,((_tmp90B->kind=kind,((_tmp90B->defn=type,((_tmp90B->atts=_tmp141,((_tmp90B->tq=_tmp13E,_tmp90B)))))))))))))),_tmp90C)))),_tmp90A)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp90F;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp90E;return Cyc_Absyn_new_stmt((void*)((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=12,((_tmp90F.f1=d,((_tmp90F.f2=s,_tmp90F)))))),_tmp90E)))),d->loc);}
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
const char*_tmp910;Cyc_Parse_err(((_tmp910="inline is allowed only on function definitions",_tag_dyneither(_tmp910,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp916;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp915;struct Cyc_List_List*_tmp914;struct Cyc_List_List*_tmp16C=(_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914->hd=Cyc_Absyn_new_decl((void*)((_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916[0]=((_tmp915.tag=6,((_tmp915.f1=_tmp154,_tmp915)))),_tmp916)))),loc),((_tmp914->tl=0,_tmp914)))));_npop_handler(0);return _tmp16C;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp155->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp156=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp155->f1)->r;if(_tmp156->tag != 1)goto _LLA0;else{_tmp157=_tmp156->f1;}}}_LL9F:
# 910
 if(_tmp14E != 0){const char*_tmp917;Cyc_Parse_err(((_tmp917="attributes on enum not supported",_tag_dyneither(_tmp917,sizeof(char),33))),loc);}
_tmp157->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp91D;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp91C;struct Cyc_List_List*_tmp91B;struct Cyc_List_List*_tmp171=(_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B->hd=Cyc_Absyn_new_decl((void*)((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91C.tag=8,((_tmp91C.f1=_tmp157,_tmp91C)))),_tmp91D)))),loc),((_tmp91B->tl=0,_tmp91B)))));_npop_handler(0);return _tmp171;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp158->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp159=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp158->f1)->r;if(_tmp159->tag != 2)goto _LLA2;else{_tmp15A=_tmp159->f1;}}}_LLA1:
# 914
 if(_tmp14E != 0){const char*_tmp91E;Cyc_Parse_err(((_tmp91E="attributes on datatypes not supported",_tag_dyneither(_tmp91E,sizeof(char),38))),loc);}
_tmp15A->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp924;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp923;struct Cyc_List_List*_tmp922;struct Cyc_List_List*_tmp176=(_tmp922=_cycalloc(sizeof(*_tmp922)),((_tmp922->hd=Cyc_Absyn_new_decl((void*)((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp923.tag=7,((_tmp923.f1=_tmp15A,_tmp923)))),_tmp924)))),loc),((_tmp922->tl=0,_tmp922)))));_npop_handler(0);return _tmp176;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp151;if(_tmp15B->tag != 11)goto _LLA4;else{if((((_tmp15B->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp15C=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f1;_tmp15D=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f2;_tmp15E=(_tmp15B->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15E);
struct Cyc_Absyn_Aggrdecl*_tmp925;struct Cyc_Absyn_Aggrdecl*_tmp178=(_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925->kind=_tmp15C,((_tmp925->sc=s,((_tmp925->name=_tmp15D,((_tmp925->tvs=_tmp177,((_tmp925->impl=0,((_tmp925->attributes=0,_tmp925)))))))))))));
if(_tmp14E != 0){const char*_tmp926;Cyc_Parse_err(((_tmp926="bad attributes on type declaration",_tag_dyneither(_tmp926,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp92C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp92B;struct Cyc_List_List*_tmp92A;struct Cyc_List_List*_tmp17D=(_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A->hd=Cyc_Absyn_new_decl((void*)((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92B.tag=6,((_tmp92B.f1=_tmp178,_tmp92B)))),_tmp92C)))),loc),((_tmp92A->tl=0,_tmp92A)))));_npop_handler(0);return _tmp17D;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp15F->tag != 3)goto _LLA6;else{if((((_tmp15F->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp160=(struct Cyc_Absyn_Datatypedecl**)(((_tmp15F->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp14E != 0){const char*_tmp92D;Cyc_Parse_err(((_tmp92D="bad attributes on datatype",_tag_dyneither(_tmp92D,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp933;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp932;struct Cyc_List_List*_tmp931;struct Cyc_List_List*_tmp183=(_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->hd=Cyc_Absyn_new_decl((void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp932.tag=7,((_tmp932.f1=*_tmp160,_tmp932)))),_tmp933)))),loc),((_tmp931->tl=0,_tmp931)))));_npop_handler(0);return _tmp183;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp161->tag != 3)goto _LLA8;else{if((((_tmp161->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp162=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).name;_tmp163=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp164=(_tmp161->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp164);
struct Cyc_Absyn_Decl*_tmp185=Cyc_Absyn_datatype_decl(s,_tmp162,_tmp184,0,_tmp163,loc);
if(_tmp14E != 0){const char*_tmp934;Cyc_Parse_err(((_tmp934="bad attributes on datatype",_tag_dyneither(_tmp934,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp935;struct Cyc_List_List*_tmp188=(_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935->hd=_tmp185,((_tmp935->tl=0,_tmp935)))));_npop_handler(0);return _tmp188;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp151;if(_tmp165->tag != 13)goto _LLAA;else{_tmp166=_tmp165->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp936;struct Cyc_Absyn_Enumdecl*_tmp189=(_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->sc=s,((_tmp936->name=_tmp166,((_tmp936->fields=0,_tmp936)))))));
if(_tmp14E != 0){const char*_tmp937;Cyc_Parse_err(((_tmp937="bad attributes on enum",_tag_dyneither(_tmp937,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp941;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp940;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp93F;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp18F=(_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E->hd=((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941->r=(void*)((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=((_tmp940.tag=8,((_tmp940.f1=_tmp189,_tmp940)))),_tmp93F)))),((_tmp941->loc=loc,_tmp941)))))),((_tmp93E->tl=0,_tmp93E)))));_npop_handler(0);return _tmp18F;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp151;if(_tmp167->tag != 14)goto _LLAC;else{_tmp168=_tmp167->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp944;struct Cyc_Absyn_Enumdecl*_tmp943;struct Cyc_Absyn_Enumdecl*_tmp191=(_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943->sc=s,((_tmp943->name=Cyc_Parse_gensym_enum(),((_tmp943->fields=((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944->v=_tmp168,_tmp944)))),_tmp943)))))));
if(_tmp14E != 0){const char*_tmp945;Cyc_Parse_err(((_tmp945="bad attributes on enum",_tag_dyneither(_tmp945,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp94F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp94E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_List_List*_tmp197=(_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->hd=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->r=(void*)((_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94E.tag=8,((_tmp94E.f1=_tmp191,_tmp94E)))),_tmp94D)))),((_tmp94F->loc=loc,_tmp94F)))))),((_tmp94C->tl=0,_tmp94C)))));_npop_handler(0);return _tmp197;};}_LLAC:;_LLAD:
{const char*_tmp950;Cyc_Parse_err(((_tmp950="missing declarator",_tag_dyneither(_tmp950,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp19B=0;_npop_handler(0);return _tmp19B;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp19C=Cyc_Parse_apply_tmss(mkrgn,_tmp14C,_tmp150,declarators,_tmp14E);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp951;Cyc_Parse_err(((_tmp951="initializer in typedef declaration",_tag_dyneither(_tmp951,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp19C);
struct Cyc_List_List*_tmp19E=decls;_npop_handler(0);return _tmp19E;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp19F=_tmp19C;for(0;_tmp19F != 0;(_tmp19F=_tmp19F->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_stmttmp15=(struct _tuple15*)_tmp19F->hd;struct _tuple0*_tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct _tuple15*_tmp1A0=_stmttmp15;_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;_tmp1A4=_tmp1A0->f4;_tmp1A5=_tmp1A0->f5;
if(_tmp1A4 != 0){
const char*_tmp954;void*_tmp953;(_tmp953=0,Cyc_Tcutil_warn(loc,((_tmp954="bad type params, ignoring",_tag_dyneither(_tmp954,sizeof(char),26))),_tag_dyneither(_tmp953,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp957;void*_tmp956;(_tmp956=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp957="unexpected NULL in parse!",_tag_dyneither(_tmp957,sizeof(char),26))),_tag_dyneither(_tmp956,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1AA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1AB=Cyc_Absyn_new_vardecl(_tmp1A1,_tmp1A3,_tmp1AA);
_tmp1AB->tq=_tmp1A2;
_tmp1AB->sc=s;
_tmp1AB->attributes=_tmp1A5;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp95D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp95C;struct Cyc_Absyn_Decl*_tmp95B;struct Cyc_Absyn_Decl*_tmp1AC=(_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B->r=(void*)((_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D[0]=((_tmp95C.tag=0,((_tmp95C.f1=_tmp1AB,_tmp95C)))),_tmp95D)))),((_tmp95B->loc=loc,_tmp95B)))));
struct Cyc_List_List*_tmp95E;decls=((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E->hd=_tmp1AC,((_tmp95E->tl=decls,_tmp95E))))));};};}}{
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
{const char*_tmp963;void*_tmp962[2];struct Cyc_String_pa_PrintArg_struct _tmp961;struct Cyc_Int_pa_PrintArg_struct _tmp960;Cyc_Parse_err((struct _dyneither_ptr)((_tmp960.tag=1,((_tmp960.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp961.tag=0,((_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp962[0]=& _tmp961,((_tmp962[1]=& _tmp960,Cyc_aprintf(((_tmp963="bad kind: %s; strlen=%d",_tag_dyneither(_tmp963,sizeof(char),24))),_tag_dyneither(_tmp962,sizeof(void*),2)))))))))))))),loc);}
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
{const char*_tmp964;Cyc_Parse_err(((_tmp964="cannot mix patterns and expressions in case",_tag_dyneither(_tmp964,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp965;return((_tmp965.Int_tok).val=yy1,(((_tmp965.Int_tok).tag=1,_tmp965)));}static char _tmp1D3[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.Char_tok).val=yy1,(((_tmp966.Char_tok).tag=2,_tmp966)));}static char _tmp1D8[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.String_tok).val=yy1,(((_tmp967.String_tok).tag=3,_tmp967)));}static char _tmp1DD[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.YY1).val=yy1,(((_tmp968.YY1).tag=7,_tmp968)));}static char _tmp1E2[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY2).val=yy1,(((_tmp969.YY2).tag=8,_tmp969)));}static char _tmp1E7[28]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY3).val=yy1,(((_tmp96A.YY3).tag=9,_tmp96A)));}static char _tmp1EC[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY4).val=yy1,(((_tmp96B.YY4).tag=10,_tmp96B)));}static char _tmp1F1[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY5).val=yy1,(((_tmp96C.YY5).tag=11,_tmp96C)));}static char _tmp1F6[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY6).val=yy1,(((_tmp96D.YY6).tag=12,_tmp96D)));}static char _tmp1FB[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY7).val=yy1,(((_tmp96E.YY7).tag=13,_tmp96E)));}static char _tmp200[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY8).val=yy1,(((_tmp96F.YY8).tag=14,_tmp96F)));}static char _tmp205[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY9).val=yy1,(((_tmp970.YY9).tag=15,_tmp970)));}static char _tmp20A[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.QualId_tok).val=yy1,(((_tmp971.QualId_tok).tag=5,_tmp971)));}static char _tmp20F[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY10).val=yy1,(((_tmp972.YY10).tag=16,_tmp972)));}static char _tmp214[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY11).val=yy1,(((_tmp973.YY11).tag=17,_tmp973)));}static char _tmp219[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY12).val=yy1,(((_tmp974.YY12).tag=18,_tmp974)));}static char _tmp21E[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY13).val=yy1,(((_tmp975.YY13).tag=19,_tmp975)));}static char _tmp223[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY14).val=yy1,(((_tmp976.YY14).tag=20,_tmp976)));}static char _tmp228[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY15).val=yy1,(((_tmp977.YY15).tag=21,_tmp977)));}static char _tmp22D[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY16).val=yy1,(((_tmp978.YY16).tag=22,_tmp978)));}static char _tmp232[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY17).val=yy1,(((_tmp979.YY17).tag=23,_tmp979)));}static char _tmp237[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY18).val=yy1,(((_tmp97A.YY18).tag=24,_tmp97A)));}static char _tmp23C[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY19).val=yy1,(((_tmp97B.YY19).tag=25,_tmp97B)));}static char _tmp241[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY20).val=yy1,(((_tmp97C.YY20).tag=26,_tmp97C)));}static char _tmp246[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY21).val=yy1,(((_tmp97D.YY21).tag=27,_tmp97D)));}static char _tmp24B[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY22).val=yy1,(((_tmp97E.YY22).tag=28,_tmp97E)));}static char _tmp250[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY23).val=yy1,(((_tmp97F.YY23).tag=29,_tmp97F)));}static char _tmp255[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY24).val=yy1,(((_tmp980.YY24).tag=30,_tmp980)));}static char _tmp25A[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY25).val=yy1,(((_tmp981.YY25).tag=31,_tmp981)));}static char _tmp25F[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY26).val=yy1,(((_tmp982.YY26).tag=32,_tmp982)));}static char _tmp264[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY27).val=yy1,(((_tmp983.YY27).tag=33,_tmp983)));}static char _tmp269[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY28).val=yy1,(((_tmp984.YY28).tag=34,_tmp984)));}static char _tmp26E[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY29).val=yy1,(((_tmp985.YY29).tag=35,_tmp985)));}static char _tmp273[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY30).val=yy1,(((_tmp986.YY30).tag=36,_tmp986)));}static char _tmp278[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
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
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY31).val=yy1,(((_tmp987.YY31).tag=37,_tmp987)));}static char _tmp27D[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
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
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY32).val=yy1,(((_tmp988.YY32).tag=38,_tmp988)));}static char _tmp282[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY33).val=yy1,(((_tmp989.YY33).tag=39,_tmp989)));}static char _tmp287[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY34).val=yy1,(((_tmp98A.YY34).tag=40,_tmp98A)));}static char _tmp28C[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY35).val=yy1,(((_tmp98B.YY35).tag=41,_tmp98B)));}static char _tmp291[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY36).val=yy1,(((_tmp98C.YY36).tag=42,_tmp98C)));}static char _tmp296[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY37).val=yy1,(((_tmp98D.YY37).tag=43,_tmp98D)));}static char _tmp29B[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.YY38).val=yy1,(((_tmp98E.YY38).tag=44,_tmp98E)));}static char _tmp2A0[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98F;return((_tmp98F.YY39).val=yy1,(((_tmp98F.YY39).tag=45,_tmp98F)));}static char _tmp2A5[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.YY40).val=yy1,(((_tmp990.YY40).tag=46,_tmp990)));}static char _tmp2AA[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.YY41).val=yy1,(((_tmp991.YY41).tag=47,_tmp991)));}static char _tmp2AF[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp992;return((_tmp992.YY42).val=yy1,(((_tmp992.YY42).tag=48,_tmp992)));}static char _tmp2B4[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp993;return((_tmp993.YY43).val=yy1,(((_tmp993.YY43).tag=49,_tmp993)));}static char _tmp2B9[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.YY44).val=yy1,(((_tmp994.YY44).tag=50,_tmp994)));}static char _tmp2BE[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.YY45).val=yy1,(((_tmp995.YY45).tag=51,_tmp995)));}static char _tmp2C3[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.YY46).val=yy1,(((_tmp996.YY46).tag=52,_tmp996)));}static char _tmp2C8[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY47).val=yy1,(((_tmp997.YY47).tag=53,_tmp997)));}static char _tmp2CD[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY48).val=yy1,(((_tmp998.YY48).tag=54,_tmp998)));}static char _tmp2D2[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY49).val=yy1,(((_tmp999.YY49).tag=55,_tmp999)));}static char _tmp2D7[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY50).val=yy1,(((_tmp99A.YY50).tag=56,_tmp99A)));}static char _tmp2DC[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY51).val=yy1,(((_tmp99B.YY51).tag=57,_tmp99B)));}static char _tmp2E1[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY52).val=yy1,(((_tmp99C.YY52).tag=58,_tmp99C)));}static char _tmp2E6[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY53).val=yy1,(((_tmp99D.YY53).tag=59,_tmp99D)));}static char _tmp2EB[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY54).val=yy1,(((_tmp99E.YY54).tag=60,_tmp99E)));}static char _tmp2F0[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY55).val=yy1,(((_tmp99F.YY55).tag=61,_tmp99F)));}static char _tmp2F5[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY56).val=yy1,(((_tmp9A0.YY56).tag=62,_tmp9A0)));}static char _tmp2FA[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY57).val=yy1,(((_tmp9A1.YY57).tag=63,_tmp9A1)));}static char _tmp2FF[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.Asm_tok).val=yy1,(((_tmp9A2.Asm_tok).tag=6,_tmp9A2)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1185
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1189
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9A3;return(_tmp9A3.timestamp=0,((_tmp9A3.first_line=0,((_tmp9A3.first_column=0,((_tmp9A3.last_line=0,((_tmp9A3.last_column=0,_tmp9A3)))))))));}
# 1192
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1203 "parse.y"
static short Cyc_yytranslate[375]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp304[2]="$";static char _tmp305[6]="error";static char _tmp306[12]="$undefined.";static char _tmp307[5]="AUTO";static char _tmp308[9]="REGISTER";static char _tmp309[7]="STATIC";static char _tmp30A[7]="EXTERN";static char _tmp30B[8]="TYPEDEF";static char _tmp30C[5]="VOID";static char _tmp30D[5]="CHAR";static char _tmp30E[6]="SHORT";static char _tmp30F[4]="INT";static char _tmp310[5]="LONG";static char _tmp311[6]="FLOAT";static char _tmp312[7]="DOUBLE";static char _tmp313[7]="SIGNED";static char _tmp314[9]="UNSIGNED";static char _tmp315[6]="CONST";static char _tmp316[9]="VOLATILE";static char _tmp317[9]="RESTRICT";static char _tmp318[7]="STRUCT";static char _tmp319[6]="UNION";static char _tmp31A[5]="CASE";static char _tmp31B[8]="DEFAULT";static char _tmp31C[7]="INLINE";static char _tmp31D[7]="SIZEOF";static char _tmp31E[9]="OFFSETOF";static char _tmp31F[3]="IF";static char _tmp320[5]="ELSE";static char _tmp321[7]="SWITCH";static char _tmp322[6]="WHILE";static char _tmp323[3]="DO";static char _tmp324[4]="FOR";static char _tmp325[5]="GOTO";static char _tmp326[9]="CONTINUE";static char _tmp327[6]="BREAK";static char _tmp328[7]="RETURN";static char _tmp329[5]="ENUM";static char _tmp32A[8]="NULL_kw";static char _tmp32B[4]="LET";static char _tmp32C[6]="THROW";static char _tmp32D[4]="TRY";static char _tmp32E[6]="CATCH";static char _tmp32F[7]="EXPORT";static char _tmp330[4]="NEW";static char _tmp331[9]="ABSTRACT";static char _tmp332[9]="FALLTHRU";static char _tmp333[6]="USING";static char _tmp334[10]="NAMESPACE";static char _tmp335[9]="DATATYPE";static char _tmp336[8]="XTUNION";static char _tmp337[7]="TUNION";static char _tmp338[7]="MALLOC";static char _tmp339[8]="RMALLOC";static char _tmp33A[7]="CALLOC";static char _tmp33B[8]="RCALLOC";static char _tmp33C[5]="SWAP";static char _tmp33D[9]="REGION_T";static char _tmp33E[6]="TAG_T";static char _tmp33F[7]="REGION";static char _tmp340[5]="RNEW";static char _tmp341[8]="REGIONS";static char _tmp342[13]="RESET_REGION";static char _tmp343[16]="NOZEROTERM_QUAL";static char _tmp344[14]="ZEROTERM_QUAL";static char _tmp345[12]="REGION_QUAL";static char _tmp346[7]="PORTON";static char _tmp347[8]="PORTOFF";static char _tmp348[12]="DYNREGION_T";static char _tmp349[6]="ALIAS";static char _tmp34A[8]="NUMELTS";static char _tmp34B[8]="VALUEOF";static char _tmp34C[10]="VALUEOF_T";static char _tmp34D[9]="TAGCHECK";static char _tmp34E[13]="NUMELTS_QUAL";static char _tmp34F[10]="THIN_QUAL";static char _tmp350[9]="FAT_QUAL";static char _tmp351[13]="NOTNULL_QUAL";static char _tmp352[14]="NULLABLE_QUAL";static char _tmp353[14]="REQUIRES_QUAL";static char _tmp354[12]="TAGGED_QUAL";static char _tmp355[16]="EXTENSIBLE_QUAL";static char _tmp356[15]="RESETABLE_QUAL";static char _tmp357[7]="PTR_OP";static char _tmp358[7]="INC_OP";static char _tmp359[7]="DEC_OP";static char _tmp35A[8]="LEFT_OP";static char _tmp35B[9]="RIGHT_OP";static char _tmp35C[6]="LE_OP";static char _tmp35D[6]="GE_OP";static char _tmp35E[6]="EQ_OP";static char _tmp35F[6]="NE_OP";static char _tmp360[7]="AND_OP";static char _tmp361[6]="OR_OP";static char _tmp362[11]="MUL_ASSIGN";static char _tmp363[11]="DIV_ASSIGN";static char _tmp364[11]="MOD_ASSIGN";static char _tmp365[11]="ADD_ASSIGN";static char _tmp366[11]="SUB_ASSIGN";static char _tmp367[12]="LEFT_ASSIGN";static char _tmp368[13]="RIGHT_ASSIGN";static char _tmp369[11]="AND_ASSIGN";static char _tmp36A[11]="XOR_ASSIGN";static char _tmp36B[10]="OR_ASSIGN";static char _tmp36C[9]="ELLIPSIS";static char _tmp36D[11]="LEFT_RIGHT";static char _tmp36E[12]="COLON_COLON";static char _tmp36F[11]="IDENTIFIER";static char _tmp370[17]="INTEGER_CONSTANT";static char _tmp371[7]="STRING";static char _tmp372[8]="WSTRING";static char _tmp373[19]="CHARACTER_CONSTANT";static char _tmp374[20]="WCHARACTER_CONSTANT";static char _tmp375[18]="FLOATING_CONSTANT";static char _tmp376[9]="TYPE_VAR";static char _tmp377[13]="TYPEDEF_NAME";static char _tmp378[16]="QUAL_IDENTIFIER";static char _tmp379[18]="QUAL_TYPEDEF_NAME";static char _tmp37A[10]="ATTRIBUTE";static char _tmp37B[4]="ASM";static char _tmp37C[4]="';'";static char _tmp37D[4]="'{'";static char _tmp37E[4]="'}'";static char _tmp37F[4]="','";static char _tmp380[4]="'='";static char _tmp381[4]="'<'";static char _tmp382[4]="'>'";static char _tmp383[4]="'('";static char _tmp384[4]="')'";static char _tmp385[4]="'_'";static char _tmp386[4]="'$'";static char _tmp387[4]="':'";static char _tmp388[4]="'.'";static char _tmp389[4]="'['";static char _tmp38A[4]="']'";static char _tmp38B[4]="'*'";static char _tmp38C[4]="'@'";static char _tmp38D[4]="'?'";static char _tmp38E[4]="'+'";static char _tmp38F[4]="'|'";static char _tmp390[4]="'^'";static char _tmp391[4]="'&'";static char _tmp392[4]="'-'";static char _tmp393[4]="'/'";static char _tmp394[4]="'%'";static char _tmp395[4]="'~'";static char _tmp396[4]="'!'";static char _tmp397[5]="prog";static char _tmp398[17]="translation_unit";static char _tmp399[12]="export_list";static char _tmp39A[19]="export_list_values";static char _tmp39B[21]="external_declaration";static char _tmp39C[20]="function_definition";static char _tmp39D[21]="function_definition2";static char _tmp39E[13]="using_action";static char _tmp39F[15]="unusing_action";static char _tmp3A0[17]="namespace_action";static char _tmp3A1[19]="unnamespace_action";static char _tmp3A2[12]="declaration";static char _tmp3A3[19]="resetable_qual_opt";static char _tmp3A4[17]="declaration_list";static char _tmp3A5[23]="declaration_specifiers";static char _tmp3A6[24]="storage_class_specifier";static char _tmp3A7[15]="attributes_opt";static char _tmp3A8[11]="attributes";static char _tmp3A9[15]="attribute_list";static char _tmp3AA[10]="attribute";static char _tmp3AB[15]="type_specifier";static char _tmp3AC[25]="type_specifier_notypedef";static char _tmp3AD[5]="kind";static char _tmp3AE[15]="type_qualifier";static char _tmp3AF[15]="enum_specifier";static char _tmp3B0[11]="enum_field";static char _tmp3B1[22]="enum_declaration_list";static char _tmp3B2[26]="struct_or_union_specifier";static char _tmp3B3[16]="type_params_opt";static char _tmp3B4[16]="struct_or_union";static char _tmp3B5[24]="struct_declaration_list";static char _tmp3B6[25]="struct_declaration_list0";static char _tmp3B7[21]="init_declarator_list";static char _tmp3B8[22]="init_declarator_list0";static char _tmp3B9[16]="init_declarator";static char _tmp3BA[19]="struct_declaration";static char _tmp3BB[25]="specifier_qualifier_list";static char _tmp3BC[35]="notypedef_specifier_qualifier_list";static char _tmp3BD[23]="struct_declarator_list";static char _tmp3BE[24]="struct_declarator_list0";static char _tmp3BF[18]="struct_declarator";static char _tmp3C0[19]="datatype_specifier";static char _tmp3C1[14]="qual_datatype";static char _tmp3C2[19]="datatypefield_list";static char _tmp3C3[20]="datatypefield_scope";static char _tmp3C4[14]="datatypefield";static char _tmp3C5[11]="declarator";static char _tmp3C6[23]="declarator_withtypedef";static char _tmp3C7[18]="direct_declarator";static char _tmp3C8[30]="direct_declarator_withtypedef";static char _tmp3C9[8]="pointer";static char _tmp3CA[12]="one_pointer";static char _tmp3CB[14]="pointer_quals";static char _tmp3CC[13]="pointer_qual";static char _tmp3CD[23]="pointer_null_and_bound";static char _tmp3CE[14]="pointer_bound";static char _tmp3CF[18]="zeroterm_qual_opt";static char _tmp3D0[8]="rgn_opt";static char _tmp3D1[11]="tqual_list";static char _tmp3D2[20]="parameter_type_list";static char _tmp3D3[9]="type_var";static char _tmp3D4[16]="optional_effect";static char _tmp3D5[19]="optional_rgn_order";static char _tmp3D6[10]="rgn_order";static char _tmp3D7[16]="optional_inject";static char _tmp3D8[11]="effect_set";static char _tmp3D9[14]="atomic_effect";static char _tmp3DA[11]="region_set";static char _tmp3DB[15]="parameter_list";static char _tmp3DC[22]="parameter_declaration";static char _tmp3DD[16]="identifier_list";static char _tmp3DE[17]="identifier_list0";static char _tmp3DF[12]="initializer";static char _tmp3E0[18]="array_initializer";static char _tmp3E1[17]="initializer_list";static char _tmp3E2[12]="designation";static char _tmp3E3[16]="designator_list";static char _tmp3E4[11]="designator";static char _tmp3E5[10]="type_name";static char _tmp3E6[14]="any_type_name";static char _tmp3E7[15]="type_name_list";static char _tmp3E8[20]="abstract_declarator";static char _tmp3E9[27]="direct_abstract_declarator";static char _tmp3EA[10]="statement";static char _tmp3EB[13]="open_exp_opt";static char _tmp3EC[18]="labeled_statement";static char _tmp3ED[21]="expression_statement";static char _tmp3EE[19]="compound_statement";static char _tmp3EF[16]="block_item_list";static char _tmp3F0[20]="selection_statement";static char _tmp3F1[15]="switch_clauses";static char _tmp3F2[20]="iteration_statement";static char _tmp3F3[15]="jump_statement";static char _tmp3F4[12]="exp_pattern";static char _tmp3F5[20]="conditional_pattern";static char _tmp3F6[19]="logical_or_pattern";static char _tmp3F7[20]="logical_and_pattern";static char _tmp3F8[21]="inclusive_or_pattern";static char _tmp3F9[21]="exclusive_or_pattern";static char _tmp3FA[12]="and_pattern";static char _tmp3FB[17]="equality_pattern";static char _tmp3FC[19]="relational_pattern";static char _tmp3FD[14]="shift_pattern";static char _tmp3FE[17]="additive_pattern";static char _tmp3FF[23]="multiplicative_pattern";static char _tmp400[13]="cast_pattern";static char _tmp401[14]="unary_pattern";static char _tmp402[16]="postfix_pattern";static char _tmp403[16]="primary_pattern";static char _tmp404[8]="pattern";static char _tmp405[19]="tuple_pattern_list";static char _tmp406[20]="tuple_pattern_list0";static char _tmp407[14]="field_pattern";static char _tmp408[19]="field_pattern_list";static char _tmp409[20]="field_pattern_list0";static char _tmp40A[11]="expression";static char _tmp40B[22]="assignment_expression";static char _tmp40C[20]="assignment_operator";static char _tmp40D[23]="conditional_expression";static char _tmp40E[20]="constant_expression";static char _tmp40F[22]="logical_or_expression";static char _tmp410[23]="logical_and_expression";static char _tmp411[24]="inclusive_or_expression";static char _tmp412[24]="exclusive_or_expression";static char _tmp413[15]="and_expression";static char _tmp414[20]="equality_expression";static char _tmp415[22]="relational_expression";static char _tmp416[17]="shift_expression";static char _tmp417[20]="additive_expression";static char _tmp418[26]="multiplicative_expression";static char _tmp419[16]="cast_expression";static char _tmp41A[17]="unary_expression";static char _tmp41B[15]="unary_operator";static char _tmp41C[19]="postfix_expression";static char _tmp41D[17]="field_expression";static char _tmp41E[19]="primary_expression";static char _tmp41F[25]="argument_expression_list";static char _tmp420[26]="argument_expression_list0";static char _tmp421[9]="constant";static char _tmp422[20]="qual_opt_identifier";static char _tmp423[17]="qual_opt_typedef";static char _tmp424[18]="struct_union_name";static char _tmp425[11]="field_name";static char _tmp426[12]="right_angle";
# 1550 "parse.y"
static struct _dyneither_ptr Cyc_yytname[291]={{_tmp304,_tmp304,_tmp304 + 2},{_tmp305,_tmp305,_tmp305 + 6},{_tmp306,_tmp306,_tmp306 + 12},{_tmp307,_tmp307,_tmp307 + 5},{_tmp308,_tmp308,_tmp308 + 9},{_tmp309,_tmp309,_tmp309 + 7},{_tmp30A,_tmp30A,_tmp30A + 7},{_tmp30B,_tmp30B,_tmp30B + 8},{_tmp30C,_tmp30C,_tmp30C + 5},{_tmp30D,_tmp30D,_tmp30D + 5},{_tmp30E,_tmp30E,_tmp30E + 6},{_tmp30F,_tmp30F,_tmp30F + 4},{_tmp310,_tmp310,_tmp310 + 5},{_tmp311,_tmp311,_tmp311 + 6},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,_tmp313,_tmp313 + 7},{_tmp314,_tmp314,_tmp314 + 9},{_tmp315,_tmp315,_tmp315 + 6},{_tmp316,_tmp316,_tmp316 + 9},{_tmp317,_tmp317,_tmp317 + 9},{_tmp318,_tmp318,_tmp318 + 7},{_tmp319,_tmp319,_tmp319 + 6},{_tmp31A,_tmp31A,_tmp31A + 5},{_tmp31B,_tmp31B,_tmp31B + 8},{_tmp31C,_tmp31C,_tmp31C + 7},{_tmp31D,_tmp31D,_tmp31D + 7},{_tmp31E,_tmp31E,_tmp31E + 9},{_tmp31F,_tmp31F,_tmp31F + 3},{_tmp320,_tmp320,_tmp320 + 5},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 6},{_tmp323,_tmp323,_tmp323 + 3},{_tmp324,_tmp324,_tmp324 + 4},{_tmp325,_tmp325,_tmp325 + 5},{_tmp326,_tmp326,_tmp326 + 9},{_tmp327,_tmp327,_tmp327 + 6},{_tmp328,_tmp328,_tmp328 + 7},{_tmp329,_tmp329,_tmp329 + 5},{_tmp32A,_tmp32A,_tmp32A + 8},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 4},{_tmp32E,_tmp32E,_tmp32E + 6},{_tmp32F,_tmp32F,_tmp32F + 7},{_tmp330,_tmp330,_tmp330 + 4},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 9},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 10},{_tmp335,_tmp335,_tmp335 + 9},{_tmp336,_tmp336,_tmp336 + 8},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 7},{_tmp33B,_tmp33B,_tmp33B + 8},{_tmp33C,_tmp33C,_tmp33C + 5},{_tmp33D,_tmp33D,_tmp33D + 9},{_tmp33E,_tmp33E,_tmp33E + 6},{_tmp33F,_tmp33F,_tmp33F + 7},{_tmp340,_tmp340,_tmp340 + 5},{_tmp341,_tmp341,_tmp341 + 8},{_tmp342,_tmp342,_tmp342 + 13},{_tmp343,_tmp343,_tmp343 + 16},{_tmp344,_tmp344,_tmp344 + 14},{_tmp345,_tmp345,_tmp345 + 12},{_tmp346,_tmp346,_tmp346 + 7},{_tmp347,_tmp347,_tmp347 + 8},{_tmp348,_tmp348,_tmp348 + 12},{_tmp349,_tmp349,_tmp349 + 6},{_tmp34A,_tmp34A,_tmp34A + 8},{_tmp34B,_tmp34B,_tmp34B + 8},{_tmp34C,_tmp34C,_tmp34C + 10},{_tmp34D,_tmp34D,_tmp34D + 9},{_tmp34E,_tmp34E,_tmp34E + 13},{_tmp34F,_tmp34F,_tmp34F + 10},{_tmp350,_tmp350,_tmp350 + 9},{_tmp351,_tmp351,_tmp351 + 13},{_tmp352,_tmp352,_tmp352 + 14},{_tmp353,_tmp353,_tmp353 + 14},{_tmp354,_tmp354,_tmp354 + 12},{_tmp355,_tmp355,_tmp355 + 16},{_tmp356,_tmp356,_tmp356 + 15},{_tmp357,_tmp357,_tmp357 + 7},{_tmp358,_tmp358,_tmp358 + 7},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 8},{_tmp35B,_tmp35B,_tmp35B + 9},{_tmp35C,_tmp35C,_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 6},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 6},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 11},{_tmp365,_tmp365,_tmp365 + 11},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 12},{_tmp368,_tmp368,_tmp368 + 13},{_tmp369,_tmp369,_tmp369 + 11},{_tmp36A,_tmp36A,_tmp36A + 11},{_tmp36B,_tmp36B,_tmp36B + 10},{_tmp36C,_tmp36C,_tmp36C + 9},{_tmp36D,_tmp36D,_tmp36D + 11},{_tmp36E,_tmp36E,_tmp36E + 12},{_tmp36F,_tmp36F,_tmp36F + 11},{_tmp370,_tmp370,_tmp370 + 17},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 8},{_tmp373,_tmp373,_tmp373 + 19},{_tmp374,_tmp374,_tmp374 + 20},{_tmp375,_tmp375,_tmp375 + 18},{_tmp376,_tmp376,_tmp376 + 9},{_tmp377,_tmp377,_tmp377 + 13},{_tmp378,_tmp378,_tmp378 + 16},{_tmp379,_tmp379,_tmp379 + 18},{_tmp37A,_tmp37A,_tmp37A + 10},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 4},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 5},{_tmp398,_tmp398,_tmp398 + 17},{_tmp399,_tmp399,_tmp399 + 12},{_tmp39A,_tmp39A,_tmp39A + 19},{_tmp39B,_tmp39B,_tmp39B + 21},{_tmp39C,_tmp39C,_tmp39C + 20},{_tmp39D,_tmp39D,_tmp39D + 21},{_tmp39E,_tmp39E,_tmp39E + 13},{_tmp39F,_tmp39F,_tmp39F + 15},{_tmp3A0,_tmp3A0,_tmp3A0 + 17},{_tmp3A1,_tmp3A1,_tmp3A1 + 19},{_tmp3A2,_tmp3A2,_tmp3A2 + 12},{_tmp3A3,_tmp3A3,_tmp3A3 + 19},{_tmp3A4,_tmp3A4,_tmp3A4 + 17},{_tmp3A5,_tmp3A5,_tmp3A5 + 23},{_tmp3A6,_tmp3A6,_tmp3A6 + 24},{_tmp3A7,_tmp3A7,_tmp3A7 + 15},{_tmp3A8,_tmp3A8,_tmp3A8 + 11},{_tmp3A9,_tmp3A9,_tmp3A9 + 15},{_tmp3AA,_tmp3AA,_tmp3AA + 10},{_tmp3AB,_tmp3AB,_tmp3AB + 15},{_tmp3AC,_tmp3AC,_tmp3AC + 25},{_tmp3AD,_tmp3AD,_tmp3AD + 5},{_tmp3AE,_tmp3AE,_tmp3AE + 15},{_tmp3AF,_tmp3AF,_tmp3AF + 15},{_tmp3B0,_tmp3B0,_tmp3B0 + 11},{_tmp3B1,_tmp3B1,_tmp3B1 + 22},{_tmp3B2,_tmp3B2,_tmp3B2 + 26},{_tmp3B3,_tmp3B3,_tmp3B3 + 16},{_tmp3B4,_tmp3B4,_tmp3B4 + 16},{_tmp3B5,_tmp3B5,_tmp3B5 + 24},{_tmp3B6,_tmp3B6,_tmp3B6 + 25},{_tmp3B7,_tmp3B7,_tmp3B7 + 21},{_tmp3B8,_tmp3B8,_tmp3B8 + 22},{_tmp3B9,_tmp3B9,_tmp3B9 + 16},{_tmp3BA,_tmp3BA,_tmp3BA + 19},{_tmp3BB,_tmp3BB,_tmp3BB + 25},{_tmp3BC,_tmp3BC,_tmp3BC + 35},{_tmp3BD,_tmp3BD,_tmp3BD + 23},{_tmp3BE,_tmp3BE,_tmp3BE + 24},{_tmp3BF,_tmp3BF,_tmp3BF + 18},{_tmp3C0,_tmp3C0,_tmp3C0 + 19},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 19},{_tmp3C3,_tmp3C3,_tmp3C3 + 20},{_tmp3C4,_tmp3C4,_tmp3C4 + 14},{_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 23},{_tmp3C7,_tmp3C7,_tmp3C7 + 18},{_tmp3C8,_tmp3C8,_tmp3C8 + 30},{_tmp3C9,_tmp3C9,_tmp3C9 + 8},{_tmp3CA,_tmp3CA,_tmp3CA + 12},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 13},{_tmp3CD,_tmp3CD,_tmp3CD + 23},{_tmp3CE,_tmp3CE,_tmp3CE + 14},{_tmp3CF,_tmp3CF,_tmp3CF + 18},{_tmp3D0,_tmp3D0,_tmp3D0 + 8},{_tmp3D1,_tmp3D1,_tmp3D1 + 11},{_tmp3D2,_tmp3D2,_tmp3D2 + 20},{_tmp3D3,_tmp3D3,_tmp3D3 + 9},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 19},{_tmp3D6,_tmp3D6,_tmp3D6 + 10},{_tmp3D7,_tmp3D7,_tmp3D7 + 16},{_tmp3D8,_tmp3D8,_tmp3D8 + 11},{_tmp3D9,_tmp3D9,_tmp3D9 + 14},{_tmp3DA,_tmp3DA,_tmp3DA + 11},{_tmp3DB,_tmp3DB,_tmp3DB + 15},{_tmp3DC,_tmp3DC,_tmp3DC + 22},{_tmp3DD,_tmp3DD,_tmp3DD + 16},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 18},{_tmp3E1,_tmp3E1,_tmp3E1 + 17},{_tmp3E2,_tmp3E2,_tmp3E2 + 12},{_tmp3E3,_tmp3E3,_tmp3E3 + 16},{_tmp3E4,_tmp3E4,_tmp3E4 + 11},{_tmp3E5,_tmp3E5,_tmp3E5 + 10},{_tmp3E6,_tmp3E6,_tmp3E6 + 14},{_tmp3E7,_tmp3E7,_tmp3E7 + 15},{_tmp3E8,_tmp3E8,_tmp3E8 + 20},{_tmp3E9,_tmp3E9,_tmp3E9 + 27},{_tmp3EA,_tmp3EA,_tmp3EA + 10},{_tmp3EB,_tmp3EB,_tmp3EB + 13},{_tmp3EC,_tmp3EC,_tmp3EC + 18},{_tmp3ED,_tmp3ED,_tmp3ED + 21},{_tmp3EE,_tmp3EE,_tmp3EE + 19},{_tmp3EF,_tmp3EF,_tmp3EF + 16},{_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 15},{_tmp3F2,_tmp3F2,_tmp3F2 + 20},{_tmp3F3,_tmp3F3,_tmp3F3 + 15},{_tmp3F4,_tmp3F4,_tmp3F4 + 12},{_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 19},{_tmp3F7,_tmp3F7,_tmp3F7 + 20},{_tmp3F8,_tmp3F8,_tmp3F8 + 21},{_tmp3F9,_tmp3F9,_tmp3F9 + 21},{_tmp3FA,_tmp3FA,_tmp3FA + 12},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 19},{_tmp3FD,_tmp3FD,_tmp3FD + 14},{_tmp3FE,_tmp3FE,_tmp3FE + 17},{_tmp3FF,_tmp3FF,_tmp3FF + 23},{_tmp400,_tmp400,_tmp400 + 13},{_tmp401,_tmp401,_tmp401 + 14},{_tmp402,_tmp402,_tmp402 + 16},{_tmp403,_tmp403,_tmp403 + 16},{_tmp404,_tmp404,_tmp404 + 8},{_tmp405,_tmp405,_tmp405 + 19},{_tmp406,_tmp406,_tmp406 + 20},{_tmp407,_tmp407,_tmp407 + 14},{_tmp408,_tmp408,_tmp408 + 19},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 11},{_tmp40B,_tmp40B,_tmp40B + 22},{_tmp40C,_tmp40C,_tmp40C + 20},{_tmp40D,_tmp40D,_tmp40D + 23},{_tmp40E,_tmp40E,_tmp40E + 20},{_tmp40F,_tmp40F,_tmp40F + 22},{_tmp410,_tmp410,_tmp410 + 23},{_tmp411,_tmp411,_tmp411 + 24},{_tmp412,_tmp412,_tmp412 + 24},{_tmp413,_tmp413,_tmp413 + 15},{_tmp414,_tmp414,_tmp414 + 20},{_tmp415,_tmp415,_tmp415 + 22},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 20},{_tmp418,_tmp418,_tmp418 + 26},{_tmp419,_tmp419,_tmp419 + 16},{_tmp41A,_tmp41A,_tmp41A + 17},{_tmp41B,_tmp41B,_tmp41B + 15},{_tmp41C,_tmp41C,_tmp41C + 19},{_tmp41D,_tmp41D,_tmp41D + 17},{_tmp41E,_tmp41E,_tmp41E + 19},{_tmp41F,_tmp41F,_tmp41F + 25},{_tmp420,_tmp420,_tmp420 + 26},{_tmp421,_tmp421,_tmp421 + 9},{_tmp422,_tmp422,_tmp422 + 20},{_tmp423,_tmp423,_tmp423 + 17},{_tmp424,_tmp424,_tmp424 + 18},{_tmp425,_tmp425,_tmp425 + 11},{_tmp426,_tmp426,_tmp426 + 12}};
# 1604
static short Cyc_yyr1[521]={0,147,148,148,148,148,148,148,148,148,148,148,149,149,150,150,150,151,151,151,152,152,152,152,153,153,154,155,156,157,158,158,158,158,158,158,158,159,159,160,160,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,163,163,164,165,165,166,166,166,166,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,170,170,170,171,171,171,172,172,173,173,173,174,174,174,174,174,175,175,176,176,177,177,178,178,179,180,180,181,181,182,183,183,183,183,183,183,184,184,184,184,184,184,185,186,186,187,187,187,187,187,188,188,188,189,189,190,190,190,190,191,191,191,192,192,193,193,194,194,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,196,197,197,198,199,199,200,200,200,200,200,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,214,215,215,216,216,216,217,218,218,219,219,220,220,220,220,220,221,221,221,221,222,223,223,224,224,225,225,226,226,226,226,226,227,227,228,228,228,229,229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,230,231,231,232,233,233,234,234,235,235,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,238,238,238,238,238,238,238,238,238,238,238,238,238,238,239,239,239,239,239,239,239,239,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,247,248,248,248,248,248,249,249,249,250,250,250,251,251,251,251,252,252,253,253,253,253,253,253,254,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,257,257,257,258,258,259,259,260,260,260,261,261,262,262,263,263,263,264,264,264,264,264,264,264,264,264,264,264,265,265,265,265,265,265,265,266,267,267,268,268,269,269,270,270,271,271,272,272,272,273,273,273,273,273,274,274,274,275,275,275,276,276,276,276,277,277,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,281,281,281,281,282,282,282,282,282,282,282,282,282,282,283,284,284,285,285,285,285,285,286,286,287,287,288,288,289,289,290,290};
# 1659
static short Cyc_yyr2[521]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1714
static short Cyc_yydefact[1036]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,148,88,92,0,0,0,0,0,0,0,38,511,214,513,512,514,0,0,72,0,200,200,199,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,159,0,184,187,85,163,111,55,54,48,0,99,510,0,511,506,507,508,509,111,0,376,0,0,0,0,237,0,378,379,26,28,0,0,0,0,0,0,0,0,149,0,0,0,0,0,0,0,197,198,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,515,516,111,111,37,39,37,0,20,0,216,0,172,160,185,194,193,0,0,191,192,195,196,205,187,0,70,0,55,103,0,101,0,511,386,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,495,496,475,0,0,0,0,0,478,476,477,0,403,405,419,427,429,431,433,435,437,440,445,448,451,455,0,457,479,494,493,0,387,385,33,0,0,111,0,0,0,129,125,127,256,258,0,0,9,10,0,0,0,111,517,518,215,94,0,0,164,73,235,0,232,0,3,0,5,0,286,0,31,0,0,37,21,0,116,117,142,110,0,146,0,0,0,0,0,0,0,0,0,0,0,0,511,289,291,0,299,293,0,297,279,280,281,0,282,283,284,0,40,22,122,263,0,222,238,0,0,218,216,0,202,0,0,0,207,58,206,188,0,0,104,100,0,0,380,0,0,400,0,0,0,0,252,396,401,0,398,0,465,0,421,455,0,422,423,0,0,0,0,0,0,0,0,0,458,459,37,0,0,0,461,462,460,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,409,410,411,412,413,414,415,416,417,418,408,0,463,0,485,486,0,0,0,498,0,111,393,394,0,391,0,239,0,0,0,0,259,230,0,130,135,131,133,126,128,216,0,265,257,266,520,519,0,87,91,0,89,37,93,109,64,63,0,61,165,216,234,161,265,236,173,174,0,86,201,27,0,29,0,0,0,0,121,123,241,240,23,106,118,0,0,0,137,138,140,0,111,111,154,0,0,0,0,0,511,0,37,0,328,329,330,0,0,332,0,0,37,0,300,294,122,298,292,290,0,171,223,0,0,0,229,217,224,168,0,0,0,218,170,204,203,166,202,0,0,208,59,112,0,105,426,102,98,389,390,255,0,397,251,253,384,0,0,0,0,242,247,0,0,0,0,0,0,0,0,0,0,0,0,0,497,504,0,503,404,428,0,430,432,434,436,438,439,443,444,441,442,446,447,449,450,452,453,454,407,406,484,481,0,483,0,0,0,381,0,388,32,0,382,0,0,260,136,132,134,0,218,0,202,0,267,0,216,0,278,262,0,36,111,0,0,0,129,0,111,0,216,0,183,162,233,4,6,0,35,0,0,141,124,142,0,0,218,147,156,155,0,0,150,0,0,0,307,0,0,0,0,0,0,327,331,0,0,0,0,295,288,37,24,264,216,0,226,0,0,219,0,169,222,210,167,190,189,208,186,0,7,0,254,399,402,464,0,0,243,0,248,467,0,0,0,0,471,474,0,0,0,0,0,456,500,0,0,482,480,0,0,392,395,383,261,231,274,0,268,269,202,0,0,218,202,0,90,218,0,0,0,60,62,0,175,0,0,218,0,202,0,0,0,139,144,143,115,145,157,154,154,0,0,0,0,0,0,0,0,0,0,0,0,0,307,333,0,0,296,25,218,0,227,225,0,218,0,209,0,8,490,0,489,0,244,249,0,0,0,0,424,425,484,483,502,0,505,420,499,501,0,273,271,277,276,0,270,202,115,0,67,65,66,176,182,179,0,181,177,202,0,34,0,0,153,152,301,307,0,0,0,0,0,0,335,336,338,340,342,344,346,348,351,356,359,362,366,368,374,375,0,37,304,313,0,0,0,0,0,0,0,0,0,0,334,285,212,228,220,211,216,13,0,14,466,0,0,250,468,0,0,473,472,487,0,275,272,0,0,180,178,287,108,0,0,0,307,0,372,0,0,369,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,370,308,0,315,0,0,0,323,0,0,0,0,0,0,306,0,218,12,15,0,492,491,0,0,0,488,107,0,158,302,303,0,0,0,0,307,309,339,0,341,343,345,347,349,350,354,355,352,353,357,358,360,361,363,364,365,0,314,316,317,0,325,324,0,319,0,0,0,221,213,16,0,0,0,0,0,305,371,0,367,310,0,37,318,326,320,321,0,246,245,469,0,68,0,337,307,311,322,0,373,312,470,0,0,0};
# 1821
static short Cyc_yydefgoto[144]={1033,50,689,880,51,52,287,53,458,54,460,55,56,141,57,58,527,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,648,649,650,68,476,69,449,70,71,158,159,72,117,523,316,686,607,73,608,517,677,509,513,514,420,309,251,94,95,548,466,549,331,332,333,232,302,303,609,431,290,462,291,292,293,294,295,764,296,297,841,842,843,844,845,846,847,848,849,850,851,852,853,854,855,856,334,410,411,335,336,337,298,199,397,200,533,201,202,203,204,205,206,207,208,209,210,211,212,213,214,789,215,564,565,216,217,75,881,244,435};
# 1839
static short Cyc_yypact[1036]={2685,- -32768,- -32768,- -32768,- -32768,43,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3428,170,1236,- -32768,24,87,- -32768,50,71,118,153,91,174,196,391,276,- -32768,- -32768,235,- -32768,- -32768,- -32768,221,693,274,307,304,304,- -32768,- -32768,2547,- -32768,341,513,- -32768,377,169,3428,3428,3428,- -32768,3428,- -32768,- -32768,620,- -32768,24,3263,182,88,639,1121,- -32768,- -32768,321,331,380,- -32768,24,370,- -32768,3115,217,- -32768,- -32768,- -32768,- -32768,321,6085,- -32768,378,411,3115,406,420,445,- -32768,16,- -32768,- -32768,3576,3576,2547,2547,3576,471,6085,236,- -32768,96,468,564,473,96,3798,6085,- -32768,- -32768,- -32768,2547,2822,2547,2822,- 33,- -32768,487,504,- -32768,3180,- -32768,- -32768,- -32768,- -32768,3798,- -32768,- -32768,321,129,1546,- -32768,3263,169,- -32768,3576,3502,4438,- -32768,182,- -32768,- -32768,- -32768,535,550,- -32768,- -32768,- -32768,- -32768,- 72,1121,3576,- -32768,2822,- -32768,561,565,569,24,82,- -32768,3115,68,3007,6322,579,6085,6164,605,611,613,629,631,633,645,652,6401,6401,- -32768,- -32768,- -32768,2261,657,6480,6480,6480,- -32768,- -32768,- -32768,- 62,- -32768,- -32768,- 2,685,647,649,660,421,- 48,695,253,159,- -32768,799,6480,251,- 12,- -32768,698,851,716,- -32768,- -32768,725,6085,321,851,739,316,3798,3872,3798,747,- -32768,- 55,- 55,- -32768,- -32768,121,749,222,321,- -32768,- -32768,- -32768,- -32768,8,751,- -32768,- -32768,895,317,- -32768,755,- -32768,759,- -32768,764,767,774,- -32768,564,4537,3263,- -32768,802,3798,- -32768,966,795,24,797,811,119,812,3943,813,838,822,826,4636,3943,202,823,820,- -32768,- -32768,833,1690,1690,169,1690,- -32768,- -32768,- -32768,831,- -32768,- -32768,- -32768,158,- -32768,- -32768,842,836,105,862,- -32768,- 8,845,844,459,848,729,853,3576,6085,- -32768,861,- -32768,- -32768,105,860,24,- -32768,6085,866,- -32768,864,867,- -32768,96,6085,3115,237,- -32768,- -32768,- -32768,874,877,2261,- -32768,3798,- -32768,- -32768,4042,- -32768,904,6085,6085,6085,6085,6085,6085,3798,760,2261,- -32768,- -32768,1834,875,349,6085,- -32768,- -32768,- -32768,6085,- -32768,6480,6085,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6085,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6085,- -32768,96,- -32768,- -32768,4732,96,6085,- -32768,879,321,- -32768,- -32768,878,882,3115,- -32768,575,3007,881,3576,- -32768,884,892,- -32768,3872,3872,3872,- -32768,- -32768,2971,4828,288,- -32768,254,- -32768,- -32768,- 8,- -32768,- -32768,3576,- -32768,3346,- -32768,896,- -32768,889,891,897,- -32768,2897,- -32768,396,293,- -32768,- -32768,- -32768,3798,- -32768,- -32768,- -32768,2547,- -32768,2547,916,905,901,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,931,6085,909,910,- -32768,- 44,290,321,321,791,6085,6085,907,914,6085,908,1006,2122,920,- -32768,- -32768,- -32768,584,999,- -32768,4927,6085,1978,2400,- -32768,- -32768,3180,- -32768,- -32768,- -32768,3576,- -32768,- -32768,3798,915,337,- -32768,- -32768,906,- -32768,- 8,917,3724,844,- -32768,- -32768,- -32768,- -32768,729,125,923,835,- -32768,- -32768,1204,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,918,- -32768,- -32768,- -32768,- -32768,3830,926,937,936,- -32768,- -32768,681,4537,933,940,942,946,367,943,948,570,950,957,6243,- -32768,- -32768,952,947,- -32768,685,333,647,649,660,421,- 48,- 48,695,695,695,695,253,253,159,159,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,958,- -32768,49,3576,4339,- -32768,1046,- -32768,- -32768,963,- -32768,190,974,- -32768,- -32768,- -32768,- -32768,961,844,967,729,964,254,3576,3650,5023,- -32768,- -32768,- 55,- -32768,321,796,971,8,3045,972,321,3576,3502,5122,- -32768,396,- -32768,- -32768,- -32768,977,- -32768,998,1010,- -32768,- -32768,966,6085,6085,844,- -32768,- -32768,- -32768,984,24,594,381,399,6085,794,418,3943,982,5221,5317,600,- -32768,- -32768,989,992,985,449,1690,- -32768,3263,- -32768,836,994,3576,- -32768,993,- 8,- -32768,995,- -32768,57,- -32768,- -32768,- -32768,- -32768,835,- -32768,1001,- -32768,2547,- -32768,- -32768,- -32768,1002,150,1003,- -32768,4141,- -32768,- -32768,6085,1095,6085,6164,- -32768,- -32768,96,96,1002,1005,4339,- -32768,- -32768,6085,6085,- -32768,- -32768,105,704,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1008,- -32768,- -32768,729,105,1011,844,729,996,- -32768,844,466,1012,1013,- -32768,- -32768,1014,- -32768,105,1016,844,1020,729,997,6085,1009,- -32768,- -32768,- -32768,3798,- -32768,1022,33,791,3943,1030,1024,6610,1029,1039,3943,6085,5413,626,5509,632,5605,794,- -32768,1043,1045,- -32768,- -32768,844,227,- -32768,- -32768,1052,844,3798,- -32768,804,- -32768,- -32768,10,- -32768,6085,- -32768,- -32768,4537,1041,1044,1049,- -32768,904,1050,1054,- -32768,734,- -32768,- -32768,- -32768,- -32768,4339,- -32768,- -32768,- -32768,- -32768,1060,- -32768,729,3798,1066,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1061,- -32768,- -32768,729,480,- -32768,1055,3798,- -32768,- -32768,1152,794,1070,6559,1065,2400,6480,1069,- -32768,14,- -32768,1109,1063,722,817,195,827,402,298,- -32768,- -32768,- -32768,- -32768,1111,6480,1834,- -32768,- -32768,482,3943,507,5701,3943,514,5797,5893,648,1084,- -32768,- -32768,- -32768,- -32768,1103,- -32768,994,- -32768,1105,650,- -32768,260,351,- -32768,- -32768,3798,1206,- -32768,- -32768,- -32768,4240,- -32768,- -32768,1107,1110,- -32768,- -32768,- -32768,- -32768,537,3943,1112,794,2261,- -32768,3798,1104,- -32768,1402,6480,6085,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6480,6085,- -32768,- -32768,1115,- -32768,3943,3943,558,- -32768,3943,3943,560,3943,593,5989,- -32768,- 8,844,- -32768,- -32768,236,- -32768,- -32768,2400,1108,1113,- -32768,- -32768,1130,- -32768,- -32768,- -32768,1117,1114,1122,6480,794,- -32768,685,356,647,649,649,421,- 48,- 48,695,695,695,695,253,253,159,159,- -32768,- -32768,- -32768,369,- -32768,- -32768,- -32768,3943,- -32768,- -32768,3943,- -32768,3943,3943,616,- -32768,- -32768,- -32768,1124,810,1116,3798,1120,- -32768,1002,150,- -32768,- -32768,6085,1402,- -32768,- -32768,- -32768,- -32768,3943,- -32768,- -32768,- -32768,1129,- -32768,520,- -32768,794,- -32768,- -32768,1131,- -32768,- -32768,- -32768,1250,1258,- -32768};
# 1946
static short Cyc_yypgoto[144]={- -32768,102,- -32768,310,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 45,- -32768,- 126,111,- -32768,- -32768,0,641,- -32768,98,- 153,1151,5,- -32768,- -32768,- 140,- -32768,142,1231,- 699,- -32768,- -32768,- -32768,1007,1017,303,506,- -32768,- -32768,627,- -32768,- -32768,177,- -32768,- -32768,38,- 139,1205,- 398,107,- -32768,1118,- -32768,- -32768,1233,- 496,- -32768,598,- 131,- 152,- 144,- 434,342,599,614,- 426,- 491,- 106,- 421,- 127,- -32768,- 224,- 164,- 563,- 339,- -32768,955,- 154,212,- 117,- 119,- 380,59,- -32768,- -32768,- -32768,4,- 271,- -32768,- 636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,23,1064,- -32768,748,880,- -32768,325,362,- -32768,- 162,- 296,- 161,- 364,- 358,- 324,922,- 349,- 343,- 275,- 331,- 300,20,465,531,941,287,- -32768,- 378,- -32768,393,494,- 58,- 54,- 240,- 47};
# 1968
static short Cyc_yytable[6757]={59,308,567,262,550,62,317,136,617,250,569,137,344,341,307,345,500,501,310,503,675,59,572,140,590,442,62,324,682,573,574,718,432,632,538,642,358,465,646,647,374,375,39,319,112,570,96,579,580,612,136,59,631,510,240,830,62,315,59,59,59,364,59,62,62,62,365,62,59,147,612,433,143,62,257,419,423,376,377,631,581,582,113,434,140,681,560,643,672,537,678,366,258,405,288,129,299,632,60,575,576,577,578,59,59,169,39,911,62,62,448,723,430,511,727,443,220,895,665,60,59,59,59,59,406,62,62,62,62,59,451,38,78,263,62,367,871,224,882,59,41,59,883,225,62,300,62,803,147,60,246,912,76,119,512,- 151,60,60,60,588,60,532,59,591,508,519,60,62,532,130,131,132,364,133,725,101,639,306,149,142,301,531,326,716,544,- 216,545,436,- 216,170,438,453,432,325,100,38,102,327,557,903,559,60,60,241,41,235,236,171,432,453,754,242,361,362,363,44,105,299,60,60,60,60,253,254,255,256,38,60,506,422,172,433,810,398,424,41,814,60,103,60,142,409,288,288,437,288,482,433,409,483,289,826,142,683,160,550,507,241,788,434,60,269,59,434,320,242,468,62,962,423,423,423,529,104,967,761,38,38,505,268,270,364,512,919,920,41,41,59,59,125,59,79,62,62,382,62,44,813,301,106,43,816,383,384,47,48,49,144,625,145,532,824,288,233,234,146,528,237,722,894,921,922,495,107,170,109,698,502,434,496,1011,898,625,487,399,400,401,- 238,429,494,- 238,110,171,38,874,364,752,753,111,877,440,40,41,42,539,875,450,59,794,419,60,540,62,878,512,434,415,241,951,329,330,550,43,142,477,242,669,1027,402,613,114,614,441,403,404,60,60,615,60,626,1031,380,453,250,619,381,776,38,289,289,38,289,583,584,585,40,41,42,40,41,42,19,20,453,198,427,97,471,249,453,447,428,422,422,422,116,428,424,424,424,39,616,239,927,115,597,124,267,418,59,454,928,929,659,62,455,160,624,249,630,419,39,162,667,790,60,364,140,59,674,59,120,121,62,714,62,800,801,289,794,731,638,364,793,364,97,717,562,252,364,532,532,953,730,745,741,97,1012,59,163,364,167,364,62,74,703,729,744,59,741,1013,746,59,62,364,218,670,62,312,759,743,372,373,1000,43,359,80,98,219,99,678,627,364,628,512,525,221,760,778,629,59,421,685,426,429,62,783,60,74,798,925,364,799,222,926,74,765,968,414,594,142,74,805,794,450,970,60,668,60,135,633,138,634,97,74,97,626,973,267,223,885,734,364,166,974,975,98,775,477,306,453,711,518,453,793,238,60,98,932,817,971,972,980,981,818,245,60,74,74,142,60,247,135,364,493,364,74,259,899,142,933,97,616,142,74,74,74,74,97,720,644,645,288,467,299,982,983,260,60,601,364,630,688,122,123,935,74,364,339,966,342,342,939,952,976,977,978,979,1030,618,355,356,883,706,400,401,74,342,342,342,454,166,313,359,98,959,98,59,793,59,806,38,62,777,62,555,526,314,342,359,41,364,811,364,321,908,991,322,994,59,685,44,568,323,62,598,822,402,364,47,48,49,707,404,662,467,340,364,551,552,553,554,98,556,757,668,364,758,671,98,771,996,563,364,97,901,566,38,136,592,249,477,346,954,947,40,41,42,347,364,348,134,1026,452,1018,477,865,586,477,364,249,963,868,964,74,364,349,249,350,587,351,452,735,479,563,60,484,60,944,790,949,364,352,950,47,48,49,368,289,353,142,378,379,74,360,857,369,60,342,370,611,787,521,522,512,342,646,647,81,1002,38,371,249,696,697,97,651,652,97,41,655,43,249,660,288,166,762,763,834,407,44,249,666,412,861,98,807,808,47,48,49,342,413,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,1025,1022,16,17,18,385,891,892,563,59,909,563,915,916,62,288,417,38,84,187,188,85,86,87,427,439,41,456,931,446,428,457,47,48,49,779,459,354,463,81,191,461,136,386,387,388,389,390,391,392,393,394,395,736,737,738,98,917,918,98,59,38,467,923,924,62,478,249,480,40,41,42,934,396,469,938,879,421,604,605,606,249,1020,364,832,833,97,342,481,485,488,452,490,82,452,489,491,984,985,986,497,498,74,504,74,408,467,60,168,84,499,960,85,86,87,452,261,41,288,508,289,452,88,515,434,516,520,733,89,43,90,91,530,768,770,1010,92,524,534,97,535,748,93,536,989,990,542,366,992,993,543,995,38,561,593,596,595,602,60,600,40,41,42,59,603,563,621,620,62,622,623,289,447,635,74,636,342,637,428,640,47,48,49,641,653,654,657,98,38,656,661,663,673,695,676,679,40,41,42,43,1014,684,690,1015,693,1016,1017,267,471,467,694,699,795,700,797,701,47,48,49,702,713,704,467,38,828,804,705,1028,708,709,712,40,41,42,81,721,715,249,39,724,98,862,864,471,867,726,870,472,728,739,742,47,48,49,749,750,755,342,342,766,772,60,773,774,306,780,884,38,452,267,796,782,786,710,289,40,41,42,791,829,815,827,452,802,249,452,809,471,82,812,819,820,821,756,823,47,48,49,825,831,719,835,836,168,84,97,467,85,86,87,859,860,41,872,198,873,876,88,342,886,467,887,888,89,896,90,91,900,889,342,902,92,890,74,150,151,152,93,893,897,937,904,907,941,943,153,154,155,156,157,910,913,914,930,- 11,1,945,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,946,948,21,957,359,955,965,958,961,988,1004,969,1006,1007,1005,22,1008,23,1021,1009,1019,687,1023,24,1034,25,26,27,467,987,98,1029,1035,1032,1001,28,29,- 37,740,248,108,464,751,998,30,31,32,33,81,148,34,318,1003,784,135,118,470,785,35,36,37,541,999,416,781,692,571,858,558,599,1024,0,0,0,0,0,906,0,0,342,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,342,0,0,- 11,0,82,0,0,44,0,45,46,0,0,0,0,47,48,49,0,83,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,342,93,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,762,763,21,173,174,271,342,272,273,274,275,276,277,278,279,22,81,23,175,280,135,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,342,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 307,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,658,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,0,45,46,28,29,428,0,47,48,49,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,328,0,32,168,84,0,34,85,86,87,0,0,41,0,35,36,0,88,0,0,0,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,0,0,0,38,81,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,168,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,680,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,81,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,82,0,0,0,0,45,46,28,29,0,0,0,691,0,0,168,84,0,32,85,86,87,34,0,41,0,0,0,0,88,35,36,0,0,0,89,0,90,91,0,329,330,0,92,0,0,173,174,271,93,272,273,274,275,276,277,278,279,0,81,0,175,280,0,39,176,0,281,43,0,0,0,0,177,178,179,180,0,0,45,46,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,546,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,547,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,792,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,956,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,492,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,589,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,610,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,664,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,732,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,747,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,767,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,769,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,863,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,866,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,869,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,940,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,942,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,997,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,710,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,338,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,354,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,837,838,0,0,0,0,0,0,185,186,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,905,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,82,0,196,197,0,0,0,0,0,0,0,0,0,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,839,0,90,91,0,0,0,0,92,0,0,840,0,0,93,195,0,0,196,197};
# 2647
static short Cyc_yycheck[6757]={0,145,366,129,343,0,158,65,434,115,368,65,176,175,145,176,287,288,145,290,511,21,371,68,402,17,21,167,524,372,373,594,87,454,330,79,190,261,5,6,88,89,114,160,44,369,23,378,379,429,108,51,450,61,108,754,51,129,58,59,60,123,62,58,59,60,128,62,68,69,450,126,68,68,107,227,229,125,126,477,380,381,44,138,129,519,357,131,509,329,516,93,125,105,139,57,141,518,0,374,375,376,377,103,104,82,114,93,103,104,249,602,231,121,610,107,93,816,496,21,120,121,122,123,136,120,121,122,123,129,249,107,21,129,129,137,772,121,128,139,116,141,132,127,139,141,141,710,148,51,112,137,109,51,306,122,58,59,60,399,62,323,162,403,107,309,68,162,330,58,59,60,123,62,608,125,472,120,71,68,142,321,114,134,338,128,340,234,131,107,237,249,87,170,107,107,125,129,352,835,354,103,104,107,116,103,104,125,87,267,644,115,192,193,194,127,125,262,120,121,122,123,120,121,122,123,107,129,123,229,88,126,728,213,229,116,732,139,120,141,129,218,287,288,123,290,127,126,225,130,139,747,141,128,125,594,303,107,108,138,162,132,262,138,162,115,262,262,904,422,423,424,319,120,910,653,107,107,120,137,138,123,434,88,89,116,116,287,288,120,290,121,287,288,135,290,127,731,260,125,118,735,143,144,135,136,137,125,447,127,472,745,357,101,102,133,316,105,128,815,125,126,120,127,107,49,550,289,138,127,966,827,471,274,83,84,85,120,231,280,123,106,125,107,778,123,642,643,127,783,128,115,116,117,331,128,249,357,697,511,262,124,357,784,516,138,224,107,108,132,133,710,118,262,267,115,502,1013,127,125,106,127,240,132,133,287,288,133,290,447,1026,138,450,499,439,142,667,107,287,288,107,290,382,383,384,115,116,117,115,116,117,20,21,471,89,127,23,127,115,477,127,133,422,423,424,121,133,422,423,424,114,431,107,135,127,412,59,134,122,439,123,143,144,488,439,128,125,447,145,449,602,114,121,498,694,357,123,502,458,122,460,120,121,458,131,460,706,707,357,808,614,471,123,697,123,82,593,128,116,123,642,643,131,614,628,624,93,131,488,109,123,121,123,488,0,128,613,628,498,638,131,628,502,498,123,127,502,502,146,128,627,90,91,947,118,190,22,23,107,25,946,125,123,127,676,313,120,128,672,133,530,228,527,230,427,530,680,439,44,703,138,123,703,123,142,51,128,911,223,407,439,57,714,892,447,913,458,498,460,65,458,67,460,170,70,172,624,916,265,124,794,618,123,79,917,918,82,128,471,120,638,561,123,641,808,114,488,93,859,123,914,915,923,924,128,127,498,103,104,488,502,128,108,123,279,123,112,120,128,498,128,218,612,502,120,121,122,123,225,596,478,479,667,261,669,925,926,123,530,417,123,631,530,120,121,128,142,123,173,910,175,176,128,883,919,920,921,922,128,437,185,186,132,83,84,85,162,192,193,194,123,167,127,338,170,128,172,667,892,669,717,107,667,669,669,350,314,127,213,354,116,123,729,123,123,839,128,122,128,689,685,127,367,124,689,120,743,127,123,135,136,137,132,133,120,343,127,123,346,347,348,349,218,351,120,656,123,123,506,225,120,128,360,123,331,831,364,107,786,404,427,624,127,887,878,115,116,117,127,123,127,121,1013,249,128,638,120,385,641,123,447,905,120,907,260,123,127,454,127,397,127,267,620,269,402,667,272,669,120,1009,120,123,127,123,135,136,137,92,667,127,669,86,87,289,127,762,139,689,323,140,428,689,63,64,946,330,5,6,38,953,107,141,499,122,123,412,481,482,415,116,485,118,509,488,859,321,22,23,759,121,127,518,497,107,765,331,122,123,135,136,137,366,107,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,1012,1005,17,18,19,56,122,123,496,859,840,499,140,141,859,910,127,107,108,109,110,111,112,113,127,126,116,122,858,128,133,122,135,136,137,673,122,127,114,38,130,124,950,94,95,96,97,98,99,100,101,102,103,107,108,109,412,90,91,415,910,107,550,86,87,910,121,614,121,115,116,117,863,124,122,866,122,624,422,423,424,628,122,123,757,758,543,472,127,127,127,447,120,92,450,107,120,927,928,929,127,131,458,122,460,104,594,859,107,108,127,902,111,112,113,471,124,116,1013,107,859,477,121,128,138,131,128,615,127,118,129,130,122,658,659,965,135,134,122,596,126,629,141,126,935,936,122,93,939,940,123,942,107,128,125,123,128,123,910,128,115,116,117,1013,122,653,127,121,1013,128,123,910,127,107,530,120,561,126,133,120,135,136,137,123,127,121,30,543,107,131,120,42,127,107,138,128,115,116,117,118,991,128,134,994,128,996,997,754,127,697,123,128,700,123,702,123,135,136,137,123,123,128,710,107,749,713,128,1018,128,122,128,115,116,117,38,122,128,784,114,128,596,766,767,127,769,128,771,131,134,128,128,135,136,137,127,107,122,642,643,127,121,1013,120,128,120,122,791,107,624,816,25,126,121,121,1013,115,116,117,125,120,134,134,638,128,831,641,128,127,92,128,128,128,128,649,128,135,136,137,128,127,104,121,128,107,108,762,794,111,112,113,131,122,116,120,839,120,114,121,703,128,808,127,123,127,108,129,130,122,128,714,28,135,128,689,63,64,65,141,128,128,865,121,127,868,869,74,75,76,77,78,131,92,139,92,0,1,122,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,123,122,24,122,905,25,128,123,122,120,128,912,108,122,127,37,128,39,128,123,122,43,128,45,0,47,48,49,892,930,762,128,0,128,950,57,58,59,623,114,35,260,641,944,66,67,68,69,38,70,72,159,953,680,786,48,265,685,80,81,82,332,946,225,676,543,370,762,353,415,1009,- 1,- 1,- 1,- 1,- 1,837,- 1,- 1,840,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,858,- 1,- 1,122,- 1,92,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,911,141,913,914,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,965,29,30,31,32,33,34,35,36,37,38,39,40,41,950,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,1012,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,37,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,49,- 1,- 1,127,- 1,129,130,- 1,57,58,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,- 1,- 1,114,115,116,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,- 1,129,130,57,58,133,- 1,135,136,137,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,92,129,130,57,58,133,- 1,135,136,137,- 1,- 1,104,- 1,68,107,108,- 1,72,111,112,113,- 1,- 1,116,- 1,80,81,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,132,133,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,- 1,- 1,107,38,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,107,108,- 1,45,111,112,113,49,- 1,116,- 1,- 1,- 1,- 1,121,57,58,59,- 1,- 1,127,- 1,129,130,- 1,- 1,68,69,135,- 1,72,- 1,- 1,- 1,141,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,124,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,- 1,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,129,130,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,38,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,92,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,68,111,112,113,72,- 1,116,- 1,- 1,- 1,- 1,121,80,81,- 1,- 1,- 1,127,- 1,129,130,- 1,132,133,- 1,135,- 1,- 1,25,26,27,141,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,114,44,- 1,46,118,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,129,130,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,32,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,92,- 1,145,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E0[8]="stdcall";static char _tmp4E1[6]="cdecl";static char _tmp4E2[9]="fastcall";static char _tmp4E3[9]="noreturn";static char _tmp4E4[6]="const";static char _tmp4E5[8]="aligned";static char _tmp4E6[7]="packed";static char _tmp4E7[7]="shared";static char _tmp4E8[7]="unused";static char _tmp4E9[5]="weak";static char _tmp4EA[10]="dllimport";static char _tmp4EB[10]="dllexport";static char _tmp4EC[23]="no_instrument_function";static char _tmp4ED[12]="constructor";static char _tmp4EE[11]="destructor";static char _tmp4EF[22]="no_check_memory_usage";static char _tmp4F0[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9AB(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9AA,unsigned int*_tmp9A9,union Cyc_YYSTYPE**_tmp9A8){for(*_tmp9AA=0;*_tmp9AA < *_tmp9A9;(*_tmp9AA)++){(*_tmp9A8)[*_tmp9AA]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9B0(unsigned int*_tmp9AF,unsigned int*_tmp9AE,struct Cyc_Yyltype**_tmp9AD){for(*_tmp9AF=0;*_tmp9AF < *_tmp9AE;(*_tmp9AF)++){(*_tmp9AD)[*_tmp9AF]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9B7(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9B6,unsigned int*_tmp9B5,short**_tmp9B3){for(*_tmp9B6=0;*_tmp9B6 < *_tmp9B5;(*_tmp9B6)++){(*_tmp9B3)[*_tmp9B6]=(short)(
# 216
*_tmp9B6 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9B6)): 0);}}static void _tmp9BD(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9BC,unsigned int*_tmp9BB,union Cyc_YYSTYPE**_tmp9B9){for(*_tmp9BC=0;*_tmp9BC < *_tmp9BB;(*_tmp9BC)++){(*_tmp9B9)[*_tmp9BC]=(union Cyc_YYSTYPE)(
# 219
*_tmp9BC <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9BC)):*yylval);}}static void _tmp9C3(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9C2,unsigned int*_tmp9C1,struct Cyc_Yyltype**_tmp9BF){for(*_tmp9C2=0;*_tmp9C2 < *_tmp9C1;(*_tmp9C2)++){(*_tmp9BF)[*_tmp9C2]=(struct Cyc_Yyltype)(
# 222
*_tmp9C2 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9C2)):
 Cyc_yynewloc());}}static void _tmpEB1(unsigned int*_tmpEB0,unsigned int*_tmpEAF,char**_tmpEAD){for(*_tmpEB0=0;*_tmpEB0 < *_tmpEAF;(*_tmpEB0)++){(*_tmpEAD)[*_tmpEB0]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp428=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp428;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9A4;union Cyc_YYSTYPE yylval=((_tmp9A4.YYINITIALSVAL).val=0,(((_tmp9A4.YYINITIALSVAL).tag=64,_tmp9A4)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9A6;unsigned int _tmp9A5;struct _dyneither_ptr yyss=(_tmp9A5=200,((_tmp9A6=_region_calloc(yyregion,sizeof(short),_tmp9A5),_tag_dyneither(_tmp9A6,sizeof(short),_tmp9A5))));
# 158
int yyvsp_offset;
unsigned int _tmp9AA;unsigned int _tmp9A9;union Cyc_YYSTYPE*_tmp9A8;unsigned int _tmp9A7;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9A7=(unsigned int)200,((_tmp9A8=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A7)),((((_tmp9A9=_tmp9A7,_tmp9AB(& yylval,& _tmp9AA,& _tmp9A9,& _tmp9A8))),_tmp9A8)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9AF;unsigned int _tmp9AE;struct Cyc_Yyltype*_tmp9AD;unsigned int _tmp9AC;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9AC=(unsigned int)200,((_tmp9AD=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9AC)),((((_tmp9AE=_tmp9AC,_tmp9B0(& _tmp9AF,& _tmp9AE,& _tmp9AD))),_tmp9AD)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9B1;Cyc_yyerror(((_tmp9B1="parser stack overflow",_tag_dyneither(_tmp9B1,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9B6;unsigned int _tmp9B5;struct _dyneither_ptr _tmp9B4;short*_tmp9B3;unsigned int _tmp9B2;struct _dyneither_ptr yyss1=(_tmp9B2=(unsigned int)yystacksize,((_tmp9B3=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9B2)),((_tmp9B4=_tag_dyneither(_tmp9B3,sizeof(short),_tmp9B2),((((_tmp9B5=_tmp9B2,_tmp9B7(& yyssp_offset,& yyss,& _tmp9B6,& _tmp9B5,& _tmp9B3))),_tmp9B4)))))));
# 217
unsigned int _tmp9BC;unsigned int _tmp9BB;struct _dyneither_ptr _tmp9BA;union Cyc_YYSTYPE*_tmp9B9;unsigned int _tmp9B8;struct _dyneither_ptr yyvs1=
(_tmp9B8=(unsigned int)yystacksize,((_tmp9B9=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9B8)),((_tmp9BA=_tag_dyneither(_tmp9B9,sizeof(union Cyc_YYSTYPE),_tmp9B8),((((_tmp9BB=_tmp9B8,_tmp9BD(& yyvs,& yyssp_offset,& yylval,& _tmp9BC,& _tmp9BB,& _tmp9B9))),_tmp9BA)))))));
# 221
unsigned int _tmp9C2;unsigned int _tmp9C1;struct _dyneither_ptr _tmp9C0;struct Cyc_Yyltype*_tmp9BF;unsigned int _tmp9BE;struct _dyneither_ptr yyls1=(_tmp9BE=(unsigned int)yystacksize,((_tmp9BF=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9BE)),((_tmp9C0=_tag_dyneither(_tmp9BF,sizeof(struct Cyc_Yyltype),_tmp9BE),((((_tmp9C1=_tmp9BE,_tmp9C3(& yyssp_offset,& yyls,& _tmp9C2,& _tmp9C1,& _tmp9BF))),_tmp9C0)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1036,yystate)];
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
yychar1=yychar > 0  && yychar <= 374?(int)Cyc_yytranslate[_check_known_subscript_notnull(375,yychar)]: 291;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6756) || Cyc_yycheck[_check_known_subscript_notnull(6757,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6757,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1035){
int _tmp436=0;_npop_handler(0);return _tmp436;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1036,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(521,yyn)];
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
{struct _handler_cons _tmp437;_push_handler(& _tmp437);{int _tmp439=0;if(setjmp(_tmp437.handler))_tmp439=1;if(!_tmp439){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp438=(void*)_exn_thrown;void*_tmp43B=_tmp438;void*_tmp43D;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp43C=(struct Cyc_Core_Failure_exn_struct*)_tmp43B;if(_tmp43C->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1179
 x=0;goto _LL212;_LL215: _tmp43D=_tmp43B;_LL216:(void)_throw(_tmp43D);_LL212:;}};}
# 1181
{struct _handler_cons _tmp43E;_push_handler(& _tmp43E);{int _tmp440=0;if(setjmp(_tmp43E.handler))_tmp440=1;if(!_tmp440){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp43F=(void*)_exn_thrown;void*_tmp442=_tmp43F;void*_tmp444;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp443=(struct Cyc_Core_Failure_exn_struct*)_tmp442;if(_tmp443->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1185
 y=0;goto _LL217;_LL21A: _tmp444=_tmp442;_LL21B:(void)_throw(_tmp444);_LL217:;}};}
# 1187
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1189
break;}case 3: _LL211: {
# 1191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1192 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9CD;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9CC;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9CB;struct Cyc_List_List*_tmp9CA;yyval=Cyc_YY19(((_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA->hd=((_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((_tmp9CD->r=(void*)((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CC.tag=11,((_tmp9CC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9CC.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9CC)))))),_tmp9CB)))),((_tmp9CD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9CD)))))),((_tmp9CA->tl=0,_tmp9CA)))))));}
Cyc_Lex_leave_using();
# 1195
break;}case 4: _LL21C: {
# 1197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1196 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9D7;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9D6;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9D5;struct Cyc_List_List*_tmp9D4;yyval=Cyc_YY19(((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4->hd=((_tmp9D7=_cycalloc(sizeof(*_tmp9D7)),((_tmp9D7->r=(void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D6.tag=11,((_tmp9D6.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9D6.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D6)))))),_tmp9D5)))),((_tmp9D7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D7)))))),((_tmp9D4->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9D4)))))));}
break;}case 5: _LL21D: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1199 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9E6;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9E5;struct _dyneither_ptr*_tmp9E4;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E3;struct Cyc_List_List*_tmp9E2;yyval=Cyc_YY19(((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2->hd=((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6->r=(void*)((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E5.tag=10,((_tmp9E5.f1=((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9E4)))),((_tmp9E5.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9E5)))))),_tmp9E3)))),((_tmp9E6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E6)))))),((_tmp9E2->tl=0,_tmp9E2)))))));}
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
{struct _handler_cons _tmp452;_push_handler(& _tmp452);{int _tmp454=0;if(setjmp(_tmp452.handler))_tmp454=1;if(!_tmp454){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp453=(void*)_exn_thrown;void*_tmp456=_tmp453;void*_tmp458;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp457=(struct Cyc_Core_Failure_exn_struct*)_tmp456;if(_tmp457->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmp9E7;nspace=((_tmp9E7="",_tag_dyneither(_tmp9E7,sizeof(char),1)));}goto _LL220;_LL223: _tmp458=_tmp456;_LL224:(void)_throw(_tmp458);_LL220:;}};}
# 1209
{struct _handler_cons _tmp45A;_push_handler(& _tmp45A);{int _tmp45C=0;if(setjmp(_tmp45A.handler))_tmp45C=1;if(!_tmp45C){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp45B=(void*)_exn_thrown;void*_tmp45E=_tmp45B;void*_tmp460;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp45F=(struct Cyc_Core_Failure_exn_struct*)_tmp45E;if(_tmp45F->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp460=_tmp45E;_LL229:(void)_throw(_tmp460);_LL225:;}};}
# 1212
{struct _handler_cons _tmp461;_push_handler(& _tmp461);{int _tmp463=0;if(setjmp(_tmp461.handler))_tmp463=1;if(!_tmp463){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp462=(void*)_exn_thrown;void*_tmp465=_tmp462;void*_tmp467;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp466=(struct Cyc_Core_Failure_exn_struct*)_tmp465;if(_tmp466->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp467=_tmp465;_LL22E:(void)_throw(_tmp467);_LL22A:;}};}
# 1215
{struct Cyc_Absyn_Decl*_tmp9F6;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9F5;struct _dyneither_ptr*_tmp9F4;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F3;struct Cyc_List_List*_tmp9F2;yyval=Cyc_YY19(((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2->hd=((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6->r=(void*)((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F5.tag=10,((_tmp9F5.f1=((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=nspace,_tmp9F4)))),((_tmp9F5.f2=x,_tmp9F5)))))),_tmp9F3)))),((_tmp9F6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F6)))))),((_tmp9F2->tl=y,_tmp9F2)))))));}
# 1217
break;}case 7: _LL21F: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1218 "parse.y"
struct _dyneither_ptr _tmp46D=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9F7;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46D,((_tmp9F7="C",_tag_dyneither(_tmp9F7,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA01;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA00;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9FF;struct Cyc_List_List*_tmp9FE;yyval=Cyc_YY19(((_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->hd=((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01->r=(void*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=12,((_tmpA00.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA00)))),_tmp9FF)))),((_tmpA01->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA01)))))),((_tmp9FE->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9FE)))))));}else{
# 1222
{const char*_tmpA02;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46D,((_tmpA02="C include",_tag_dyneither(_tmpA02,sizeof(char),10))))!= 0){
const char*_tmpA03;Cyc_Parse_err(((_tmpA03="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA03,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1225
struct Cyc_Absyn_Decl*_tmpA0D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA0C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA0B;struct Cyc_List_List*_tmpA0A;yyval=Cyc_YY19(((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->hd=((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->r=(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=13,((_tmpA0C.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA0C.f2=0,_tmpA0C)))))),_tmpA0B)))),((_tmpA0D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0D)))))),((_tmpA0A->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA0A)))))));};}}
# 1228
break;}case 8: _LL22F: {
# 1230
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1232
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1229 "parse.y"
{const char*_tmpA0E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA0E="C include",_tag_dyneither(_tmpA0E,sizeof(char),10))))!= 0){
const char*_tmpA0F;Cyc_Parse_err(((_tmpA0F="expecting \"C include\"",_tag_dyneither(_tmpA0F,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA19;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA18;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA17;struct Cyc_List_List*_tmpA16;yyval=Cyc_YY19(((_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16->hd=((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19->r=(void*)((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17[0]=((_tmpA18.tag=13,((_tmpA18.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA18.f2=exs,_tmpA18)))))),_tmpA17)))),((_tmpA19->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA19)))))),((_tmpA16->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA16)))))));}
# 1234
break;};}case 9: _LL230: {
# 1236
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1238
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1235 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1C;struct Cyc_List_List*_tmpA1B;yyval=Cyc_YY19(((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->hd=((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA1C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1C)))))),((_tmpA1B->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1B)))))));}
break;}case 10: _LL231: {
# 1238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1237 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1F;struct Cyc_List_List*_tmpA1E;yyval=Cyc_YY19(((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->hd=((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA1F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1F)))))),((_tmpA1E->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1E)))))));}
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
{struct _tuple27*_tmpA22;struct Cyc_List_List*_tmpA21;yyval=Cyc_YY55(((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->hd=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA22->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA22->f3=0,_tmpA22)))))))),((_tmpA21->tl=0,_tmpA21)))))));}
break;}case 15: _LL236: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1248 "parse.y"
{struct _tuple27*_tmpA25;struct Cyc_List_List*_tmpA24;yyval=Cyc_YY55(((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24->hd=((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA25->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA25->f3=0,_tmpA25)))))))),((_tmpA24->tl=0,_tmpA24)))))));}
break;}case 16: _LL237: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct _tuple27*_tmpA28;struct Cyc_List_List*_tmpA27;yyval=Cyc_YY55(((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27->hd=((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA28->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA28->f3=0,_tmpA28)))))))),((_tmpA27->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA27)))))));}
break;}case 17: _LL238: {
# 1253
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1254 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA2E;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA2D;struct Cyc_List_List*_tmpA2C;yyval=Cyc_YY19(((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->hd=Cyc_Absyn_new_decl((void*)((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2D.tag=1,((_tmpA2D.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA2D)))),_tmpA2E)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA2C->tl=0,_tmpA2C)))))));}
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
break;}case 20: _LL23B: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1261 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 21: _LL23C: {
# 1264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1263 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48C=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48C,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 22: _LL23D: {
# 1267
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1276 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL23E: {
# 1279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1278 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48D=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48D,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23F: {
# 1282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1286 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48E=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48E,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL240: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1289 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp48F=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp48F,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL241: {
# 1293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1294 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 27: _LL242: {
# 1297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1297 "parse.y"
Cyc_Lex_leave_using();
break;}case 28: _LL243: {
# 1300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1300 "parse.y"
{struct _dyneither_ptr*_tmpA2F;Cyc_Lex_enter_namespace(((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA2F)))));}yyval=yyyyvsp[1];
break;}case 29: _LL244: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1303 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 30: _LL245: {
# 1306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp491=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp491,(unsigned int)_tmp491));
break;}case 31: _LL246: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1312 "parse.y"
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 32: _LL247: {
# 1315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1315 "parse.y"
{struct Cyc_List_List*_tmpA30;yyval=Cyc_YY19(((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA30->tl=0,_tmpA30)))))));}
break;}case 33: _LL248: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1317 "parse.y"
struct Cyc_List_List*_tmp493=0;
{struct Cyc_List_List*_tmp494=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp494 != 0;_tmp494=_tmp494->tl){
struct _dyneither_ptr*_tmp495=(struct _dyneither_ptr*)_tmp494->hd;
struct _tuple0*_tmpA31;struct _tuple0*qv=(_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->f1=Cyc_Absyn_Rel_n(0),((_tmpA31->f2=_tmp495,_tmpA31)))));
struct Cyc_Absyn_Vardecl*_tmp496=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA32;_tmp493=((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->hd=_tmp496,((_tmpA32->tl=_tmp493,_tmpA32))))));}}
# 1324
_tmp493=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp493);
{struct Cyc_List_List*_tmpA33;yyval=Cyc_YY19(((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->hd=Cyc_Absyn_letv_decl(_tmp493,(unsigned int)(yyyylsp[0]).first_line),((_tmpA33->tl=0,_tmpA33)))))));}
# 1327
break;}case 34: _LL249: {
# 1329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1330 "parse.y"
struct _dyneither_ptr _tmp49A=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA34;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49A,((_tmpA34="`H",_tag_dyneither(_tmpA34,sizeof(char),3))))== 0){
const char*_tmpA37;void*_tmpA36;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA36=0,Cyc_aprintf(((_tmpA37="bad occurrence of heap region",_tag_dyneither(_tmpA37,sizeof(char),30))),_tag_dyneither(_tmpA36,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA38;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49A,((_tmpA38="`U",_tag_dyneither(_tmpA38,sizeof(char),3))))== 0){
const char*_tmpA3B;void*_tmpA3A;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA3A=0,Cyc_aprintf(((_tmpA3B="bad occurrence of unique region",_tag_dyneither(_tmpA3B,sizeof(char),32))),_tag_dyneither(_tmpA3A,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA3E;struct Cyc_Absyn_Tvar*_tmpA3D;struct Cyc_Absyn_Tvar*tv=(_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->name=((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=_tmp49A,_tmpA3E)))),((_tmpA3D->identity=- 1,((_tmpA3D->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA3D)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA41;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA40;void*t=(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=2,((_tmpA41.f1=tv,_tmpA41)))),_tmpA40))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA46;struct _dyneither_ptr*_tmpA44;struct _tuple0*_tmpA43;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->f1=Cyc_Absyn_Loc_n,((_tmpA43->f2=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA44)))),_tmpA43)))))),(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=15,((_tmpA47.f1=t,_tmpA47)))),_tmpA46)))),0);
{struct Cyc_List_List*_tmpA48;yyval=Cyc_YY19(((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA48->tl=0,_tmpA48)))))));}
# 1340
break;};}case 35: _LL24A: {
# 1342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1342 "parse.y"
int _tmp4AA=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4AB=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA49;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AB,((_tmpA49="H",_tag_dyneither(_tmpA49,sizeof(char),2))))== 0){
const char*_tmpA4A;Cyc_Parse_err(((_tmpA4A="bad occurrence of heap region `H",_tag_dyneither(_tmpA4A,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA4B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AB,((_tmpA4B="U",_tag_dyneither(_tmpA4B,sizeof(char),2))))== 0){
const char*_tmpA4C;Cyc_Parse_err(((_tmpA4C="bad occurrence of unique region `U",_tag_dyneither(_tmpA4C,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4AA  && _tmp4AC != 0){
const char*_tmpA4D;Cyc_Parse_err(((_tmpA4D="open regions cannot be @resetable",_tag_dyneither(_tmpA4D,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA5A;const char*_tmpA59;void*_tmpA58[1];struct Cyc_String_pa_PrintArg_struct _tmpA57;struct Cyc_Absyn_Tvar*_tmpA56;struct Cyc_Absyn_Tvar*tv=(_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->name=((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A[0]=(struct _dyneither_ptr)((_tmpA57.tag=0,((_tmpA57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4AB),((_tmpA58[0]=& _tmpA57,Cyc_aprintf(((_tmpA59="`%s",_tag_dyneither(_tmpA59,sizeof(char),4))),_tag_dyneither(_tmpA58,sizeof(void*),1)))))))),_tmpA5A)))),((_tmpA56->identity=- 1,((_tmpA56->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA56)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA5D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA5C;void*t=(void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=2,((_tmpA5D.f1=tv,_tmpA5D)))),_tmpA5C))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA63;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA62;struct _dyneither_ptr*_tmpA60;struct _tuple0*_tmpA5F;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->f1=Cyc_Absyn_Loc_n,((_tmpA5F->f2=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60[0]=_tmp4AB,_tmpA60)))),_tmpA5F)))))),(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=15,((_tmpA63.f1=t,_tmpA63)))),_tmpA62)))),0);
# 1356
{struct Cyc_List_List*_tmpA64;yyval=Cyc_YY19(((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4AA,_tmp4AC,(unsigned int)(yyyylsp[0]).first_line),((_tmpA64->tl=0,_tmpA64)))))));}
# 1358
break;};}case 36: _LL24B: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1360 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6C;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA6B;struct _dyneither_ptr*_tmpA6A;struct Cyc_Absyn_Tvar*_tmpA69;struct Cyc_Absyn_Tvar*tv=(_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->name=((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA6A)))),((_tmpA69->identity=- 1,((_tmpA69->kind=(void*)((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6B.tag=0,((_tmpA6B.f1=& Cyc_Tcutil_rk,_tmpA6B)))),_tmpA6C)))),_tmpA69)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA6F;void*_tmpA6E;(_tmpA6E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA6F="too many declarations in alias",_tag_dyneither(_tmpA6F,sizeof(char),31))),_tag_dyneither(_tmpA6E,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_stmttmp35=d->r;void*_tmp4C0=_stmttmp35;struct Cyc_Absyn_Vardecl*_tmp4C2;_LL24E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C1=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C1->tag != 0)goto _LL250;else{_tmp4C2=_tmp4C1->f1;}}_LL24F:
# 1367
{struct Cyc_List_List*_tmpA70;yyval=Cyc_YY19(((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->hd=Cyc_Absyn_alias_decl(tv,_tmp4C2,(unsigned int)(yyyylsp[0]).first_line),((_tmpA70->tl=0,_tmpA70)))))));}
goto _LL24D;_LL250:;_LL251:
# 1370
{const char*_tmpA71;Cyc_Parse_err(((_tmpA71="expecting variable declaration",_tag_dyneither(_tmpA71,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY19(0);_LL24D:;}
# 1374
break;};}case 37: _LL24C:
# 1376
 yyval=Cyc_YY34(0);
break;case 38: _LL252: {
# 1379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1378 "parse.y"
yyval=Cyc_YY34(1);
break;}case 39: _LL253: {
# 1381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1383 "parse.y"
yyval=yyyyvsp[0];
break;}case 40: _LL254: {
# 1386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1385 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 41: _LL255: {
# 1388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpA72;yyval=Cyc_YY20(((_tmpA72.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA72.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA72.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA72.is_inline=0,((_tmpA72.attributes=0,_tmpA72)))))))))));}
break;}case 42: _LL256: {
# 1395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1394 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CA=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4CA.sc != 0){
const char*_tmpA75;void*_tmpA74;(_tmpA74=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA75="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA75,sizeof(char),51))),_tag_dyneither(_tmpA74,sizeof(void*),0)));}
# 1398
{struct Cyc_Parse_Declaration_spec _tmpA76;yyval=Cyc_YY20(((_tmpA76.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA76.tq=_tmp4CA.tq,((_tmpA76.type_specs=_tmp4CA.type_specs,((_tmpA76.is_inline=_tmp4CA.is_inline,((_tmpA76.attributes=_tmp4CA.attributes,_tmpA76)))))))))));}
# 1402
break;}case 43: _LL257: {
# 1404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1403 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA77;yyval=Cyc_YY20(((_tmpA77.sc=0,((_tmpA77.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA77.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA77.is_inline=0,((_tmpA77.attributes=0,_tmpA77)))))))))));}
break;}case 44: _LL258: {
# 1407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1406 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA78;yyval=Cyc_YY20(((_tmpA78.sc=_tmp4CF.sc,((_tmpA78.tq=_tmp4CF.tq,((_tmpA78.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4CF.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA78.is_inline=_tmp4CF.is_inline,((_tmpA78.attributes=_tmp4CF.attributes,_tmpA78)))))))))));}
# 1412
break;}case 45: _LL259: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1413 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA79;yyval=Cyc_YY20(((_tmpA79.sc=0,((_tmpA79.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA79.type_specs=Cyc_Parse_empty_spec(0),((_tmpA79.is_inline=0,((_tmpA79.attributes=0,_tmpA79)))))))))));}
break;}case 46: _LL25A: {
# 1416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1415 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7A;yyval=Cyc_YY20(((_tmpA7A.sc=_tmp4D2.sc,((_tmpA7A.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4D2.tq),((_tmpA7A.type_specs=_tmp4D2.type_specs,((_tmpA7A.is_inline=_tmp4D2.is_inline,((_tmpA7A.attributes=_tmp4D2.attributes,_tmpA7A)))))))))));}
# 1420
break;}case 47: _LL25B: {
# 1422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1421 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7B;yyval=Cyc_YY20(((_tmpA7B.sc=0,((_tmpA7B.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7B.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7B.is_inline=1,((_tmpA7B.attributes=0,_tmpA7B)))))))))));}
break;}case 48: _LL25C: {
# 1425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1424 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7C;yyval=Cyc_YY20(((_tmpA7C.sc=_tmp4D5.sc,((_tmpA7C.tq=_tmp4D5.tq,((_tmpA7C.type_specs=_tmp4D5.type_specs,((_tmpA7C.is_inline=1,((_tmpA7C.attributes=_tmp4D5.attributes,_tmpA7C)))))))))));}
# 1428
break;}case 49: _LL25D: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1429 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7D;yyval=Cyc_YY20(((_tmpA7D.sc=0,((_tmpA7D.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7D.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7D.is_inline=0,((_tmpA7D.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpA7D)))))))))));}
break;}case 50: _LL25E: {
# 1433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1432 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D8=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7E;yyval=Cyc_YY20(((_tmpA7E.sc=_tmp4D8.sc,((_tmpA7E.tq=_tmp4D8.tq,((_tmpA7E.type_specs=_tmp4D8.type_specs,((_tmpA7E.is_inline=_tmp4D8.is_inline,((_tmpA7E.attributes=
# 1435
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4D8.attributes),_tmpA7E)))))))))));}
break;}case 51: _LL25F: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1439 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 52: _LL260: {
# 1443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1441 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 53: _LL261: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1443 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 54: _LL262: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1445 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 55: _LL263: {
# 1449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1448 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA7F;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA7F="C",_tag_dyneither(_tmpA7F,sizeof(char),2))))!= 0){
const char*_tmpA80;Cyc_Parse_err(((_tmpA80="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA80,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1453
break;}case 56: _LL264: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1453 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 57: _LL265: {
# 1457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1456 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 58: _LL266:
# 1460
 yyval=Cyc_YY48(0);
break;case 59: _LL267: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1463 "parse.y"
yyval=yyyyvsp[0];
break;}case 60: _LL268: {
# 1466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1468 "parse.y"
yyval=yyyyvsp[3];
break;}case 61: _LL269: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1472 "parse.y"
{struct Cyc_List_List*_tmpA81;yyval=Cyc_YY48(((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA81->tl=0,_tmpA81)))))));}
break;}case 62: _LL26A: {
# 1475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1473 "parse.y"
{struct Cyc_List_List*_tmpA82;yyval=Cyc_YY48(((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA82->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpA82)))))));}
break;}case 63: _LL26B: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1478 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4E0,_tmp4E0,_tmp4E0 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E1,_tmp4E1,_tmp4E1 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 8},(void*)& att_aligned},{{_tmp4E6,_tmp4E6,_tmp4E6 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E7,_tmp4E7,_tmp4E7 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1498
struct _dyneither_ptr _tmp4DE=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1500
if((((_get_dyneither_size(_tmp4DE,sizeof(char))> 4  && ((const char*)_tmp4DE.curr)[0]== '_') && ((const char*)_tmp4DE.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DE,sizeof(char),(int)(_get_dyneither_size(_tmp4DE,sizeof(char))- 3)))== '_')
# 1502
_tmp4DE=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DE,2,_get_dyneither_size(_tmp4DE,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DE,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1509
if(i == 17){
{const char*_tmpA83;Cyc_Parse_err(((_tmpA83="unrecognized attribute",_tag_dyneither(_tmpA83,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1514
break;};}case 64: _LL26C: {
# 1516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1514 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL26D: {
# 1517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1516 "parse.y"
struct _dyneither_ptr _tmp4F2=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _stmttmp34=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp4F4;struct _tuple5 _tmp4F3=_stmttmp34;_tmp4F4=_tmp4F3.f2;{
void*a;
{const char*_tmpA85;const char*_tmpA84;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA85="regparm",_tag_dyneither(_tmpA85,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA84="__regparm__",_tag_dyneither(_tmpA84,sizeof(char),12))))== 0){
if(_tmp4F4 < 0  || _tmp4F4 > 3){
const char*_tmpA86;Cyc_Parse_err(((_tmpA86="regparm requires value between 0 and 3",_tag_dyneither(_tmpA86,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA89;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA88;a=(void*)((_tmpA88=_cycalloc_atomic(sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=0,((_tmpA89.f1=_tmp4F4,_tmpA89)))),_tmpA88))));};}else{
const char*_tmpA8B;const char*_tmpA8A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA8B="aligned",_tag_dyneither(_tmpA8B,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA8A="__aligned__",_tag_dyneither(_tmpA8A,sizeof(char),12))))== 0){
if(_tmp4F4 < 0){const char*_tmpA8C;Cyc_Parse_err(((_tmpA8C="aligned requires positive power of two",_tag_dyneither(_tmpA8C,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4F4;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA8D;Cyc_Parse_err(((_tmpA8D="aligned requires positive power of two",_tag_dyneither(_tmpA8D,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA90;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA8F;a=(void*)((_tmpA8F=_cycalloc_atomic(sizeof(*_tmpA8F)),((_tmpA8F[0]=((_tmpA90.tag=6,((_tmpA90.f1=_tmp4F4,_tmpA90)))),_tmpA8F))));};};}else{
const char*_tmpA92;const char*_tmpA91;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA92="initializes",_tag_dyneither(_tmpA92,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA91="__initializes__",_tag_dyneither(_tmpA91,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA95;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA94;a=(void*)((_tmpA94=_cycalloc_atomic(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=20,((_tmpA95.f1=_tmp4F4,_tmpA95)))),_tmpA94))));}else{
const char*_tmpA97;const char*_tmpA96;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA97="noliveunique",_tag_dyneither(_tmpA97,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA96="__noliveunique__",_tag_dyneither(_tmpA96,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA9A;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA99;a=(void*)((_tmpA99=_cycalloc_atomic(sizeof(*_tmpA99)),((_tmpA99[0]=((_tmpA9A.tag=21,((_tmpA9A.f1=_tmp4F4,_tmpA9A)))),_tmpA99))));}else{
const char*_tmpA9C;const char*_tmpA9B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA9C="noconsume",_tag_dyneither(_tmpA9C,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F2,((_tmpA9B="__noconsume__",_tag_dyneither(_tmpA9B,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA9F;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA9E;a=(void*)((_tmpA9E=_cycalloc_atomic(sizeof(*_tmpA9E)),((_tmpA9E[0]=((_tmpA9F.tag=22,((_tmpA9F.f1=_tmp4F4,_tmpA9F)))),_tmpA9E))));}else{
# 1538
{const char*_tmpAA0;Cyc_Parse_err(((_tmpAA0="unrecognized attribute",_tag_dyneither(_tmpAA0,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1541
yyval=Cyc_YY49(a);
# 1543
break;};}case 66: _LL26E: {
# 1545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1544 "parse.y"
struct _dyneither_ptr _tmp50D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAA2;const char*_tmpAA1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,((_tmpAA2="section",_tag_dyneither(_tmpAA2,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50D,((_tmpAA1="__section__",_tag_dyneither(_tmpAA1,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAA5;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAA4;a=(void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA5.tag=8,((_tmpAA5.f1=_tmp50E,_tmpAA5)))),_tmpAA4))));}else{
# 1550
{const char*_tmpAA6;Cyc_Parse_err(((_tmpAA6="unrecognized attribute",_tag_dyneither(_tmpAA6,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1553
yyval=Cyc_YY49(a);
# 1555
break;}case 67: _LL26F: {
# 1557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1556 "parse.y"
struct _dyneither_ptr _tmp514=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp515=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1560
{const char*_tmpAA7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp514,((_tmpAA7="__mode__",_tag_dyneither(_tmpAA7,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAAA;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAA9;a=(void*)((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=24,((_tmpAAA.f1=_tmp515,_tmpAAA)))),_tmpAA9))));}else{
# 1563
{const char*_tmpAAB;Cyc_Parse_err(((_tmpAAB="unrecognized attribute",_tag_dyneither(_tmpAAB,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1566
yyval=Cyc_YY49(a);
# 1568
break;}case 68: _LL270: {
# 1570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1569 "parse.y"
struct _dyneither_ptr _tmp51A=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51B=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _stmttmp32=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp51D;struct _tuple5 _tmp51C=_stmttmp32;_tmp51D=_tmp51C.f2;{
struct _tuple5 _stmttmp33=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp51F;struct _tuple5 _tmp51E=_stmttmp33;_tmp51F=_tmp51E.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAAD;const char*_tmpAAC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51A,((_tmpAAD="format",_tag_dyneither(_tmpAAD,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51A,((_tmpAAC="__format__",_tag_dyneither(_tmpAAC,sizeof(char),11))))== 0){
const char*_tmpAAE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAAE="printf",_tag_dyneither(_tmpAAE,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB1;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB0;a=(void*)((_tmpAB0=_cycalloc_atomic(sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB1.tag=19,((_tmpAB1.f1=Cyc_Absyn_Printf_ft,((_tmpAB1.f2=_tmp51D,((_tmpAB1.f3=_tmp51F,_tmpAB1)))))))),_tmpAB0))));}else{
const char*_tmpAB2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAB2="scanf",_tag_dyneither(_tmpAB2,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB5;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB4;a=(void*)((_tmpAB4=_cycalloc_atomic(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=19,((_tmpAB5.f1=Cyc_Absyn_Scanf_ft,((_tmpAB5.f2=_tmp51D,((_tmpAB5.f3=_tmp51F,_tmpAB5)))))))),_tmpAB4))));}else{
# 1580
const char*_tmpAB6;Cyc_Parse_err(((_tmpAB6="unrecognized format type",_tag_dyneither(_tmpAB6,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1582
const char*_tmpAB7;Cyc_Parse_err(((_tmpAB7="unrecognized attribute",_tag_dyneither(_tmpAB7,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1585
break;};};}case 69: _LL271: {
# 1587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1594 "parse.y"
yyval=yyyyvsp[0];
break;}case 70: _LL272: {
# 1597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1596 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpABA;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAB9;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=17,((_tmpABA.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpABA.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpABA.f3=0,((_tmpABA.f4=0,_tmpABA)))))))))),_tmpAB9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 71: _LL273: {
# 1599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1600 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 72: _LL274: {
# 1603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1601 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL275: {
# 1604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1603 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL276: {
# 1606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL277: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL278: {
# 1608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL279: {
# 1609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL27A: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL27B: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL27C: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL27D: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL27E: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612 "parse.y"
yyval=yyyyvsp[0];
break;}case 83: _LL27F: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=yyyyvsp[0];
break;}case 84: _LL280: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL281: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 86: _LL282: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1620 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpABD;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpABC;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABD.tag=10,((_tmpABD.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpABD)))),_tmpABC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1623
break;}case 87: _LL283: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1624 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC0;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpABF;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=15,((_tmpAC0.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAC0)))),_tmpABF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL284: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1626 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC3;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=15,((_tmpAC3.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAC3)))),_tmpAC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1628
break;}case 89: _LL285: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1629 "parse.y"
void*_tmp532=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAC6;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAC5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC6.tag=16,((_tmpAC6.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAC6.f2=_tmp532,_tmpAC6)))))),_tmpAC5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1632
break;}case 90: _LL286: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1633 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAC9;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAC8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=16,((_tmpAC9.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAC9.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpAC9)))))),_tmpAC8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL287: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1635 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpACC;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpACB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=((_tmpACC.tag=19,((_tmpACC.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpACC)))),_tmpACB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL288: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1637 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpACF;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpACE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=19,((_tmpACF.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpACF)))),_tmpACE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL289: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1639 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAD2;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD2.tag=18,((_tmpAD2.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAD2)))),_tmpAD1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL28A: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 95: _LL28B: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1648 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAD3;yyval=Cyc_YY26(((_tmpAD3.print_const=1,((_tmpAD3.q_volatile=0,((_tmpAD3.q_restrict=0,((_tmpAD3.real_const=1,((_tmpAD3.loc=loc,_tmpAD3)))))))))));}
break;}case 96: _LL28C: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1650 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD4;yyval=Cyc_YY26(((_tmpAD4.print_const=0,((_tmpAD4.q_volatile=1,((_tmpAD4.q_restrict=0,((_tmpAD4.real_const=0,((_tmpAD4.loc=0,_tmpAD4)))))))))));}
break;}case 97: _LL28D: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD5;yyval=Cyc_YY26(((_tmpAD5.print_const=0,((_tmpAD5.q_volatile=0,((_tmpAD5.q_restrict=1,((_tmpAD5.real_const=0,((_tmpAD5.loc=0,_tmpAD5)))))))))));}
break;}case 98: _LL28E: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAE4;struct Cyc_Absyn_Enumdecl*_tmpAE3;struct Cyc_Core_Opt*_tmpAE2;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAE1;struct Cyc_Absyn_TypeDecl*_tmpAE0;struct Cyc_Absyn_TypeDecl*_tmp540=(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->r=(void*)((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4[0]=((_tmpAE1.tag=1,((_tmpAE1.f1=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->sc=Cyc_Absyn_Public,((_tmpAE3->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAE3->fields=((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpAE2)))),_tmpAE3)))))))),_tmpAE1)))),_tmpAE4)))),((_tmpAE0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAE0)))));
# 1659
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE7;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE7.tag=26,((_tmpAE7.f1=_tmp540,((_tmpAE7.f2=0,_tmpAE7)))))),_tmpAE6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1661
break;}case 99: _LL28F: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1662 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAEA;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAE9;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9[0]=((_tmpAEA.tag=13,((_tmpAEA.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAEA.f2=0,_tmpAEA)))))),_tmpAE9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL290: {
# 1665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1664 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAED;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAEC;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAED.tag=14,((_tmpAED.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAED)))),_tmpAEC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL291: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpAEE;yyval=Cyc_YY50(((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAEE->tag=0,((_tmpAEE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAEE)))))))));}
break;}case 102: _LL292: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1672 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpAEF;yyval=Cyc_YY50(((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAEF->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpAEF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAEF)))))))));}
break;}case 103: _LL293: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1676 "parse.y"
{struct Cyc_List_List*_tmpAF0;yyval=Cyc_YY51(((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF0->tl=0,_tmpAF0)))))));}
break;}case 104: _LL294: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1677 "parse.y"
{struct Cyc_List_List*_tmpAF1;yyval=Cyc_YY51(((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF1->tl=0,_tmpAF1)))))));}
break;}case 105: _LL295: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1678 "parse.y"
{struct Cyc_List_List*_tmpAF2;yyval=Cyc_YY51(((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF2->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAF2)))))));}
break;}case 106: _LL296: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAF5;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAF4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4[0]=((_tmpAF5.tag=12,((_tmpAF5.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAF5.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAF5)))))),_tmpAF4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 107: _LL297: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1690 "parse.y"
struct Cyc_List_List*_tmp553=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp555=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp553,
Cyc_Absyn_aggrdecl_impl(_tmp554,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1695
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAF8;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAF7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7[0]=((_tmpAF8.tag=26,((_tmpAF8.f1=_tmp555,((_tmpAF8.f2=0,_tmpAF8)))))),_tmpAF7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1697
break;}case 108: _LL298: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1701 "parse.y"
struct Cyc_List_List*_tmp558=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp55A=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp558,
Cyc_Absyn_aggrdecl_impl(_tmp559,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1706
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAFB;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAFA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=26,((_tmpAFB.f1=_tmp55A,((_tmpAFB.f2=0,_tmpAFB)))))),_tmpAFA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1708
break;}case 109: _LL299: {
# 1710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1709 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB05;struct Cyc_Core_Opt*_tmpB04;struct Cyc_Absyn_AggrInfo _tmpB03;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB02;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02[0]=((_tmpB05.tag=11,((_tmpB05.f1=((_tmpB03.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB04=_cycalloc_atomic(sizeof(*_tmpB04)),((_tmpB04->v=(void*)1,_tmpB04))))),((_tmpB03.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB03)))),_tmpB05)))),_tmpB02)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1712
break;}case 110: _LL29A: {
# 1714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1713 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB0B;struct Cyc_Absyn_AggrInfo _tmpB0A;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB09;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0B.tag=11,((_tmpB0B.f1=((_tmpB0A.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB0A.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB0A)))),_tmpB0B)))),_tmpB09)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1716
break;}case 111: _LL29B:
# 1718
 yyval=Cyc_YY43(0);
break;case 112: _LL29C: {
# 1721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1722 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 113: _LL29D: {
# 1725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1726 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 114: _LL29E: {
# 1729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1727 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 115: _LL29F:
# 1730
 yyval=Cyc_YY27(0);
break;case 116: _LL2A0: {
# 1733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1734 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp564=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp564 != 0;_tmp564=_tmp564->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp564->hd,decls);}}{
# 1738
struct Cyc_List_List*_tmp565=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp565 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp565,decls);
yyval=Cyc_YY27(decls);
# 1743
break;};}case 117: _LL2A1: {
# 1745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB0C;yyval=Cyc_YY28(((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB0C->tl=0,_tmpB0C)))))));}
break;}case 118: _LL2A2: {
# 1751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1750 "parse.y"
{struct Cyc_List_List*_tmpB0D;yyval=Cyc_YY28(((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB0D->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB0D)))))));}
break;}case 119: _LL2A3: {
# 1753
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1754 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 120: _LL2A4: {
# 1757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB0E;yyval=Cyc_YY22(((_tmpB0E=_region_malloc(yyr,sizeof(*_tmpB0E)),((_tmpB0E->tl=0,((_tmpB0E->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB0E)))))));}
break;}case 121: _LL2A5: {
# 1763
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1765
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1762 "parse.y"
{struct _tuple12*_tmpB0F;yyval=Cyc_YY22(((_tmpB0F=_region_malloc(yyr,sizeof(*_tmpB0F)),((_tmpB0F->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB0F->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB0F)))))));}
break;}case 122: _LL2A6: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1767 "parse.y"
{struct _tuple11 _tmpB10;yyval=Cyc_YY21(((_tmpB10.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB10.f2=0,_tmpB10)))));}
break;}case 123: _LL2A7: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1769 "parse.y"
{struct _tuple11 _tmpB11;yyval=Cyc_YY21(((_tmpB11.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB11.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB11)))));}
break;}case 124: _LL2A8: {
# 1772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1775 "parse.y"
struct _RegionHandle _tmp56C=_new_region("temp");struct _RegionHandle*temp=& _tmp56C;_push_region(temp);{
struct _tuple25 _stmttmp30=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp56E;struct Cyc_Parse_Type_specifier _tmp56F;struct Cyc_List_List*_tmp570;struct _tuple25 _tmp56D=_stmttmp30;_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;_tmp570=_tmp56D.f3;
if(_tmp56E.loc == 0)_tmp56E.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp571=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp571 != 0;_tmp571=_tmp571->tl){
struct _tuple24*_stmttmp31=(struct _tuple24*)_tmp571->hd;struct Cyc_Parse_Declarator _tmp573;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp575;struct _tuple24*_tmp572=_stmttmp31;_tmp573=_tmp572->f1;_tmp574=_tmp572->f2;_tmp575=_tmp572->f3;
{struct _tuple10*_tmpB12;decls=((_tmpB12=_region_malloc(temp,sizeof(*_tmpB12)),((_tmpB12->tl=decls,((_tmpB12->hd=_tmp573,_tmpB12))))));}{
struct _tuple16*_tmpB15;struct Cyc_List_List*_tmpB14;widths_and_reqs=(
(_tmpB14=_region_malloc(temp,sizeof(*_tmpB14)),((_tmpB14->hd=((_tmpB15=_region_malloc(temp,sizeof(*_tmpB15)),((_tmpB15->f1=_tmp574,((_tmpB15->f2=_tmp575,_tmpB15)))))),((_tmpB14->tl=widths_and_reqs,_tmpB14))))));};}}
# 1786
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp579=Cyc_Parse_speclist2typ(_tmp56F,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp56E,_tmp579,decls,_tmp570),widths_and_reqs);
# 1792
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57A));};};}
# 1795
_npop_handler(0);break;
# 1775
;_pop_region(temp);}case 125: _LL2A9: {
# 1797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1803 "parse.y"
{struct _tuple25 _tmpB16;yyval=Cyc_YY38(((_tmpB16.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB16.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB16.f3=0,_tmpB16)))))));}
break;}case 126: _LL2AA: {
# 1806
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1805 "parse.y"
struct _tuple25 _tmp57C=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB17;yyval=Cyc_YY38(((_tmpB17.f1=_tmp57C.f1,((_tmpB17.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp57C.f2),((_tmpB17.f3=_tmp57C.f3,_tmpB17)))))));}
break;}case 127: _LL2AB: {
# 1808
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1807 "parse.y"
{struct _tuple25 _tmpB18;yyval=Cyc_YY38(((_tmpB18.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB18.f2=Cyc_Parse_empty_spec(0),((_tmpB18.f3=0,_tmpB18)))))));}
break;}case 128: _LL2AC: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1809 "parse.y"
struct _tuple25 _tmp57F=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB19;yyval=Cyc_YY38(((_tmpB19.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp57F.f1),((_tmpB19.f2=_tmp57F.f2,((_tmpB19.f3=_tmp57F.f3,_tmpB19)))))));}
break;}case 129: _LL2AD: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812 "parse.y"
{struct _tuple25 _tmpB1A;yyval=Cyc_YY38(((_tmpB1A.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1A.f2=Cyc_Parse_empty_spec(0),((_tmpB1A.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB1A)))))));}
break;}case 130: _LL2AE: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814 "parse.y"
struct _tuple25 _tmp582=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1B;yyval=Cyc_YY38(((_tmpB1B.f1=_tmp582.f1,((_tmpB1B.f2=_tmp582.f2,((_tmpB1B.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp582.f3),_tmpB1B)))))));}
break;}case 131: _LL2AF: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB1C;yyval=Cyc_YY38(((_tmpB1C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1C.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB1C.f3=0,_tmpB1C)))))));}
break;}case 132: _LL2B0: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1822 "parse.y"
struct _tuple25 _tmp585=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1D;yyval=Cyc_YY38(((_tmpB1D.f1=_tmp585.f1,((_tmpB1D.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp585.f2),((_tmpB1D.f3=_tmp585.f3,_tmpB1D)))))));}
break;}case 133: _LL2B1: {
# 1825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1824 "parse.y"
{struct _tuple25 _tmpB1E;yyval=Cyc_YY38(((_tmpB1E.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB1E.f2=Cyc_Parse_empty_spec(0),((_tmpB1E.f3=0,_tmpB1E)))))));}
break;}case 134: _LL2B2: {
# 1827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1826 "parse.y"
struct _tuple25 _tmp588=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB1F;yyval=Cyc_YY38(((_tmpB1F.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp588.f1),((_tmpB1F.f2=_tmp588.f2,((_tmpB1F.f3=_tmp588.f3,_tmpB1F)))))));}
break;}case 135: _LL2B3: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829 "parse.y"
{struct _tuple25 _tmpB20;yyval=Cyc_YY38(((_tmpB20.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB20.f2=Cyc_Parse_empty_spec(0),((_tmpB20.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB20)))))));}
break;}case 136: _LL2B4: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831 "parse.y"
struct _tuple25 _tmp58B=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB21;yyval=Cyc_YY38(((_tmpB21.f1=_tmp58B.f1,((_tmpB21.f2=_tmp58B.f2,((_tmpB21.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp58B.f3),_tmpB21)))))));}
break;}case 137: _LL2B5: {
# 1834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1835 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 138: _LL2B6: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB22;yyval=Cyc_YY32(((_tmpB22=_region_malloc(yyr,sizeof(*_tmpB22)),((_tmpB22->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB22->tl=0,_tmpB22)))))));}
break;}case 139: _LL2B7: {
# 1844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1843 "parse.y"
{struct Cyc_List_List*_tmpB23;yyval=Cyc_YY32(((_tmpB23=_region_malloc(yyr,sizeof(*_tmpB23)),((_tmpB23->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB23->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB23)))))));}
break;}case 140: _LL2B8: {
# 1846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1848 "parse.y"
{struct _tuple24*_tmpB24;yyval=Cyc_YY31(((_tmpB24=_region_malloc(yyr,sizeof(*_tmpB24)),((_tmpB24->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB24->f2=0,((_tmpB24->f3=0,_tmpB24)))))))));}
break;}case 141: _LL2B9: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1852 "parse.y"
{struct Cyc_Parse_Declarator _tmpB2A;struct _tuple0*_tmpB29;struct _tuple24*_tmpB28;yyval=Cyc_YY31(((_tmpB28=_region_malloc(yyr,sizeof(*_tmpB28)),((_tmpB28->f1=((_tmpB2A.id=((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->f1=Cyc_Absyn_Rel_n(0),((_tmpB29->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB29)))))),((_tmpB2A.tms=0,_tmpB2A)))),((_tmpB28->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB28->f3=0,_tmpB28)))))))));}
# 1854
break;}case 142: _LL2BA:
# 1858
{struct Cyc_Parse_Declarator _tmpB30;struct _tuple0*_tmpB2F;struct _tuple24*_tmpB2E;yyval=Cyc_YY31(((_tmpB2E=_region_malloc(yyr,sizeof(*_tmpB2E)),((_tmpB2E->f1=((_tmpB30.id=((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->f1=Cyc_Absyn_Rel_n(0),((_tmpB2F->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB2F)))))),((_tmpB30.tms=0,_tmpB30)))),((_tmpB2E->f2=0,((_tmpB2E->f3=0,_tmpB2E)))))))));}
# 1860
break;case 143: _LL2BB: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1860 "parse.y"
{struct _tuple24*_tmpB31;yyval=Cyc_YY31(((_tmpB31=_region_malloc(yyr,sizeof(*_tmpB31)),((_tmpB31->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB31->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB31->f3=0,_tmpB31)))))))));}
break;}case 144: _LL2BC: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1862 "parse.y"
{struct _tuple24*_tmpB32;yyval=Cyc_YY31(((_tmpB32=_region_malloc(yyr,sizeof(*_tmpB32)),((_tmpB32->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB32->f2=0,((_tmpB32->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB32)))))))));}
break;}case 145: _LL2BD: {
# 1865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp59A=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp59B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB33;struct Cyc_Absyn_TypeDecl*_tmp59C=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp59B,((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB33)))),_tmp59A,(unsigned int)(yyyylsp[0]).first_line);
# 1872
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB35;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=26,((_tmpB36.f1=_tmp59C,((_tmpB36.f2=0,_tmpB36)))))),_tmpB35)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1874
break;}case 146: _LL2BE: {
# 1876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1875 "parse.y"
int _tmp5A0=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB40;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB3F;struct Cyc_Absyn_DatatypeInfo _tmpB3E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB3D;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D[0]=((_tmpB40.tag=3,((_tmpB40.f1=((_tmpB3E.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB3F.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB3F.is_extensible=_tmp5A0,_tmpB3F))))),((_tmpB3E.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB3E)))),_tmpB40)))),_tmpB3D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1878
break;}case 147: _LL2BF: {
# 1880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1879 "parse.y"
int _tmp5A5=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB4A;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB49;struct Cyc_Absyn_DatatypeFieldInfo _tmpB48;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB47;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB4A.tag=4,((_tmpB4A.f1=((_tmpB48.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB49.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB49.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB49.is_extensible=_tmp5A5,_tmpB49))))))),((_tmpB48.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB48)))),_tmpB4A)))),_tmpB47)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1884
break;}case 148: _LL2C0: {
# 1886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1887 "parse.y"
yyval=Cyc_YY34(0);
break;}case 149: _LL2C1: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1888 "parse.y"
yyval=Cyc_YY34(1);
break;}case 150: _LL2C2: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1892 "parse.y"
{struct Cyc_List_List*_tmpB4B;yyval=Cyc_YY37(((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4B->tl=0,_tmpB4B)))))));}
break;}case 151: _LL2C3: {
# 1895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1893 "parse.y"
{struct Cyc_List_List*_tmpB4C;yyval=Cyc_YY37(((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4C->tl=0,_tmpB4C)))))));}
break;}case 152: _LL2C4: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1894 "parse.y"
{struct Cyc_List_List*_tmpB4D;yyval=Cyc_YY37(((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4D->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB4D)))))));}
break;}case 153: _LL2C5: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1895 "parse.y"
{struct Cyc_List_List*_tmpB4E;yyval=Cyc_YY37(((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4E->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB4E)))))));}
break;}case 154: _LL2C6:
# 1898
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 155: _LL2C7: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1900 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 156: _LL2C8: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1901 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 157: _LL2C9: {
# 1904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1905 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB4F;yyval=Cyc_YY36(((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB4F->typs=0,((_tmpB4F->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB4F->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB4F)))))))))));}
break;}case 158: _LL2CA: {
# 1908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1907 "parse.y"
struct Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB50;yyval=Cyc_YY36(((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB50->typs=_tmp5AF,((_tmpB50->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB50->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB50)))))))))));}
break;}case 159: _LL2CB: {
# 1911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1913 "parse.y"
yyval=yyyyvsp[0];
break;}case 160: _LL2CC: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1915 "parse.y"
struct Cyc_Parse_Declarator _tmp5B1=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB51;yyval=Cyc_YY30(((_tmpB51.id=_tmp5B1.id,((_tmpB51.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B1.tms),_tmpB51)))));}
break;}case 161: _LL2CD: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 162: _LL2CE: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1924 "parse.y"
struct Cyc_Parse_Declarator _tmp5B3=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB52;yyval=Cyc_YY30(((_tmpB52.id=_tmp5B3.id,((_tmpB52.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B3.tms),_tmpB52)))));}
break;}case 163: _LL2CF: {
# 1928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1930 "parse.y"
{struct Cyc_Parse_Declarator _tmpB53;yyval=Cyc_YY30(((_tmpB53.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB53.tms=0,_tmpB53)))));}
break;}case 164: _LL2D0: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1932 "parse.y"
yyval=yyyyvsp[1];
break;}case 165: _LL2D1: {
# 1935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1936 "parse.y"
struct Cyc_Parse_Declarator _tmp5B6=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB59;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB58;struct Cyc_List_List*_tmpB57;_tmp5B6.tms=((_tmpB57=_region_malloc(yyr,sizeof(*_tmpB57)),((_tmpB57->hd=(void*)((_tmpB59=_region_malloc(yyr,sizeof(*_tmpB59)),((_tmpB59[0]=((_tmpB58.tag=5,((_tmpB58.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB58.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB58)))))),_tmpB59)))),((_tmpB57->tl=_tmp5B6.tms,_tmpB57))))));}
yyval=yyyyvsp[2];
# 1940
break;}case 166: _LL2D2: {
# 1942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1941 "parse.y"
{struct Cyc_List_List*_tmpB63;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB62;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB61;struct Cyc_Parse_Declarator _tmpB60;yyval=Cyc_YY30(((_tmpB60.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB60.tms=((_tmpB63=_region_malloc(yyr,sizeof(*_tmpB63)),((_tmpB63->hd=(void*)((_tmpB61=_region_malloc(yyr,sizeof(*_tmpB61)),((_tmpB61[0]=((_tmpB62.tag=0,((_tmpB62.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB62.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB62)))))),_tmpB61)))),((_tmpB63->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB63)))))),_tmpB60)))));}
break;}case 167: _LL2D3: {
# 1944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1943 "parse.y"
{struct Cyc_List_List*_tmpB6D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB6C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB6B;struct Cyc_Parse_Declarator _tmpB6A;yyval=Cyc_YY30(((_tmpB6A.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB6A.tms=(
(_tmpB6D=_region_malloc(yyr,sizeof(*_tmpB6D)),((_tmpB6D->hd=(void*)((_tmpB6B=_region_malloc(yyr,sizeof(*_tmpB6B)),((_tmpB6B[0]=((_tmpB6C.tag=1,((_tmpB6C.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB6C.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB6C.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB6C)))))))),_tmpB6B)))),((_tmpB6D->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB6D)))))),_tmpB6A)))));}
break;}case 168: _LL2D4: {
# 1947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1946 "parse.y"
struct _tuple26*_stmttmp2F=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5C3;int _tmp5C4;struct Cyc_Absyn_VarargInfo*_tmp5C5;void*_tmp5C6;struct Cyc_List_List*_tmp5C7;struct _tuple26*_tmp5C2=_stmttmp2F;_tmp5C3=_tmp5C2->f1;_tmp5C4=_tmp5C2->f2;_tmp5C5=_tmp5C2->f3;_tmp5C6=_tmp5C2->f4;_tmp5C7=_tmp5C2->f5;
{struct Cyc_List_List*_tmpB82;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB81;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB80;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB7F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB7E;struct Cyc_Parse_Declarator _tmpB7D;yyval=Cyc_YY30(((_tmpB7D.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB7D.tms=((_tmpB82=_region_malloc(yyr,sizeof(*_tmpB82)),((_tmpB82->hd=(void*)((_tmpB7E=_region_malloc(yyr,sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB81.tag=3,((_tmpB81.f1=(void*)((_tmpB7F=_region_malloc(yyr,sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB80.tag=1,((_tmpB80.f1=_tmp5C3,((_tmpB80.f2=_tmp5C4,((_tmpB80.f3=_tmp5C5,((_tmpB80.f4=_tmp5C6,((_tmpB80.f5=_tmp5C7,_tmpB80)))))))))))),_tmpB7F)))),_tmpB81)))),_tmpB7E)))),((_tmpB82->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB82)))))),_tmpB7D)))));}
# 1949
break;}case 169: _LL2D5: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1950 "parse.y"
{struct Cyc_List_List*_tmpB97;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB96;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB95;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB94;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB93;struct Cyc_Parse_Declarator _tmpB92;yyval=Cyc_YY30(((_tmpB92.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB92.tms=(
(_tmpB97=_region_malloc(yyr,sizeof(*_tmpB97)),((_tmpB97->hd=(void*)((_tmpB93=_region_malloc(yyr,sizeof(*_tmpB93)),((_tmpB93[0]=((_tmpB96.tag=3,((_tmpB96.f1=(void*)((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB95.tag=1,((_tmpB95.f1=0,((_tmpB95.f2=0,((_tmpB95.f3=0,((_tmpB95.f4=
# 1953
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpB95.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpB95)))))))))))),_tmpB94)))),_tmpB96)))),_tmpB93)))),((_tmpB97->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB97)))))),_tmpB92)))));}
# 1956
break;}case 170: _LL2D6: {
# 1958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1957 "parse.y"
{struct Cyc_List_List*_tmpBAC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAB;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBAA;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBA9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBA8;struct Cyc_Parse_Declarator _tmpBA7;yyval=Cyc_YY30(((_tmpBA7.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA7.tms=((_tmpBAC=_region_malloc(yyr,sizeof(*_tmpBAC)),((_tmpBAC->hd=(void*)((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBAB.tag=3,((_tmpBAB.f1=(void*)((_tmpBA9=_region_malloc(yyr,sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBAA.tag=0,((_tmpBAA.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBAA.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBAA)))))),_tmpBA9)))),_tmpBAB)))),_tmpBA8)))),((_tmpBAC->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBAC)))))),_tmpBA7)))));}
break;}case 171: _LL2D7: {
# 1960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1960 "parse.y"
struct Cyc_List_List*_tmp5DA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBB6;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBB5;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBB4;struct Cyc_Parse_Declarator _tmpBB3;yyval=Cyc_YY30(((_tmpBB3.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB3.tms=((_tmpBB6=_region_malloc(yyr,sizeof(*_tmpBB6)),((_tmpBB6->hd=(void*)((_tmpBB4=_region_malloc(yyr,sizeof(*_tmpBB4)),((_tmpBB4[0]=((_tmpBB5.tag=4,((_tmpBB5.f1=_tmp5DA,((_tmpBB5.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB5.f3=0,_tmpBB5)))))))),_tmpBB4)))),((_tmpBB6->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBB6)))))),_tmpBB3)))));}
# 1963
break;}case 172: _LL2D8: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1964 "parse.y"
{struct Cyc_List_List*_tmpBC0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBBF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBBE;struct Cyc_Parse_Declarator _tmpBBD;yyval=Cyc_YY30(((_tmpBBD.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBBD.tms=((_tmpBC0=_region_malloc(yyr,sizeof(*_tmpBC0)),((_tmpBC0->hd=(void*)((_tmpBBE=_region_malloc(yyr,sizeof(*_tmpBBE)),((_tmpBBE[0]=((_tmpBBF.tag=5,((_tmpBBF.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBBF.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBBF)))))),_tmpBBE)))),((_tmpBC0->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBC0)))))),_tmpBBD)))));}
# 1967
break;}case 173: _LL2D9: {
# 1969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1978 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC1;yyval=Cyc_YY30(((_tmpBC1.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC1.tms=0,_tmpBC1)))));}
break;}case 174: _LL2DA: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1980 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC2;yyval=Cyc_YY30(((_tmpBC2.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC2.tms=0,_tmpBC2)))));}
break;}case 175: _LL2DB: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1982 "parse.y"
yyval=yyyyvsp[1];
break;}case 176: _LL2DC: {
# 1985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1986 "parse.y"
struct Cyc_Parse_Declarator _tmp5E5=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBC8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC7;struct Cyc_List_List*_tmpBC6;_tmp5E5.tms=((_tmpBC6=_region_malloc(yyr,sizeof(*_tmpBC6)),((_tmpBC6->hd=(void*)((_tmpBC8=_region_malloc(yyr,sizeof(*_tmpBC8)),((_tmpBC8[0]=((_tmpBC7.tag=5,((_tmpBC7.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC7.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC7)))))),_tmpBC8)))),((_tmpBC6->tl=_tmp5E5.tms,_tmpBC6))))));}
yyval=yyyyvsp[2];
# 1990
break;}case 177: _LL2DD: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1991 "parse.y"
struct Cyc_Parse_Declarator _tmp5E9=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD2;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBD0;struct Cyc_Parse_Declarator _tmpBCF;yyval=Cyc_YY30(((_tmpBCF.id=_tmp5E9.id,((_tmpBCF.tms=(
(_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2->hd=(void*)((_tmpBD0=_region_malloc(yyr,sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD1.tag=0,((_tmpBD1.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBD1.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBD1)))))),_tmpBD0)))),((_tmpBD2->tl=_tmp5E9.tms,_tmpBD2)))))),_tmpBCF)))));}
break;}case 178: _LL2DE: {
# 1996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1995 "parse.y"
struct Cyc_Parse_Declarator _tmp5EE=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBDC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBDB;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBDA;struct Cyc_Parse_Declarator _tmpBD9;yyval=Cyc_YY30(((_tmpBD9.id=_tmp5EE.id,((_tmpBD9.tms=(
(_tmpBDC=_region_malloc(yyr,sizeof(*_tmpBDC)),((_tmpBDC->hd=(void*)((_tmpBDA=_region_malloc(yyr,sizeof(*_tmpBDA)),((_tmpBDA[0]=((_tmpBDB.tag=1,((_tmpBDB.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBDB.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBDB.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBDB)))))))),_tmpBDA)))),((_tmpBDC->tl=_tmp5EE.tms,_tmpBDC)))))),_tmpBD9)))));}
# 1999
break;}case 179: _LL2DF: {
# 2001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2000 "parse.y"
struct _tuple26*_stmttmp2E=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5F4;int _tmp5F5;struct Cyc_Absyn_VarargInfo*_tmp5F6;void*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct _tuple26*_tmp5F3=_stmttmp2E;_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;_tmp5F6=_tmp5F3->f3;_tmp5F7=_tmp5F3->f4;_tmp5F8=_tmp5F3->f5;{
struct Cyc_Parse_Declarator _tmp5F9=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBEF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBEE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBED;struct Cyc_Parse_Declarator _tmpBEC;yyval=Cyc_YY30(((_tmpBEC.id=_tmp5F9.id,((_tmpBEC.tms=((_tmpBF1=_region_malloc(yyr,sizeof(*_tmpBF1)),((_tmpBF1->hd=(void*)((_tmpBED=_region_malloc(yyr,sizeof(*_tmpBED)),((_tmpBED[0]=((_tmpBF0.tag=3,((_tmpBF0.f1=(void*)((_tmpBEE=_region_malloc(yyr,sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBEF.tag=1,((_tmpBEF.f1=_tmp5F4,((_tmpBEF.f2=_tmp5F5,((_tmpBEF.f3=_tmp5F6,((_tmpBEF.f4=_tmp5F7,((_tmpBEF.f5=_tmp5F8,_tmpBEF)))))))))))),_tmpBEE)))),_tmpBF0)))),_tmpBED)))),((_tmpBF1->tl=_tmp5F9.tms,_tmpBF1)))))),_tmpBEC)))));}
# 2004
break;};}case 180: _LL2E0: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2005 "parse.y"
struct Cyc_Parse_Declarator _tmp600=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC06;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC05;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC04;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC03;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC02;struct Cyc_Parse_Declarator _tmpC01;yyval=Cyc_YY30(((_tmpC01.id=_tmp600.id,((_tmpC01.tms=(
(_tmpC06=_region_malloc(yyr,sizeof(*_tmpC06)),((_tmpC06->hd=(void*)((_tmpC02=_region_malloc(yyr,sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC05.tag=3,((_tmpC05.f1=(void*)((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=1,((_tmpC04.f1=0,((_tmpC04.f2=0,((_tmpC04.f3=0,((_tmpC04.f4=
# 2009
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC04.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC04)))))))))))),_tmpC03)))),_tmpC05)))),_tmpC02)))),((_tmpC06->tl=_tmp600.tms,_tmpC06)))))),_tmpC01)))));}
# 2012
break;}case 181: _LL2E1: {
# 2014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2013 "parse.y"
struct Cyc_Parse_Declarator _tmp607=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC1B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC1A;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC19;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC18;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC17;struct Cyc_Parse_Declarator _tmpC16;yyval=Cyc_YY30(((_tmpC16.id=_tmp607.id,((_tmpC16.tms=((_tmpC1B=_region_malloc(yyr,sizeof(*_tmpC1B)),((_tmpC1B->hd=(void*)((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC1A.tag=3,((_tmpC1A.f1=(void*)((_tmpC18=_region_malloc(yyr,sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC19.tag=0,((_tmpC19.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC19.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC19)))))),_tmpC18)))),_tmpC1A)))),_tmpC17)))),((_tmpC1B->tl=_tmp607.tms,_tmpC1B)))))),_tmpC16)))));}
break;}case 182: _LL2E2: {
# 2017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2017 "parse.y"
struct Cyc_List_List*_tmp60E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp60F=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC25;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC24;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC23;struct Cyc_Parse_Declarator _tmpC22;yyval=Cyc_YY30(((_tmpC22.id=_tmp60F.id,((_tmpC22.tms=((_tmpC25=_region_malloc(yyr,sizeof(*_tmpC25)),((_tmpC25->hd=(void*)((_tmpC23=_region_malloc(yyr,sizeof(*_tmpC23)),((_tmpC23[0]=((_tmpC24.tag=4,((_tmpC24.f1=_tmp60E,((_tmpC24.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC24.f3=0,_tmpC24)))))))),_tmpC23)))),((_tmpC25->tl=_tmp60F.tms,_tmpC25)))))),_tmpC22)))));}
# 2021
break;}case 183: _LL2E3: {
# 2023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2022 "parse.y"
struct Cyc_Parse_Declarator _tmp614=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC2E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC2D;struct Cyc_Parse_Declarator _tmpC2C;yyval=Cyc_YY30(((_tmpC2C.id=_tmp614.id,((_tmpC2C.tms=((_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)((_tmpC2D=_region_malloc(yyr,sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2E.tag=5,((_tmpC2E.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC2E.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC2E)))))),_tmpC2D)))),((_tmpC2F->tl=_tmp614.tms,_tmpC2F)))))),_tmpC2C)))));}
# 2025
break;}case 184: _LL2E4: {
# 2027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2035 "parse.y"
yyval=yyyyvsp[0];
break;}case 185: _LL2E5: {
# 2038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2036 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 186: _LL2E6: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2040 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC35;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC34;struct Cyc_List_List*_tmpC33;ans=((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33->hd=(void*)((_tmpC35=_region_malloc(yyr,sizeof(*_tmpC35)),((_tmpC35[0]=((_tmpC34.tag=5,((_tmpC34.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC34.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC34)))))),_tmpC35)))),((_tmpC33->tl=ans,_tmpC33))))));}{
# 2044
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _stmttmp2C=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp61D;union Cyc_Absyn_Constraint*_tmp61E;union Cyc_Absyn_Constraint*_tmp61F;struct _tuple21 _tmp61C=_stmttmp2C;_tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;_tmp61F=_tmp61C.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC36;ptrloc=((_tmpC36=_cycalloc_atomic(sizeof(*_tmpC36)),((_tmpC36->ptr_loc=_tmp61D,((_tmpC36->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC36->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC36))))))));}{
# 2049
struct _tuple14 _stmttmp2D=Cyc_Parse_collapse_pointer_quals(_tmp61D,_tmp61E,_tmp61F,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp622;union Cyc_Absyn_Constraint*_tmp623;union Cyc_Absyn_Constraint*_tmp624;void*_tmp625;struct _tuple14 _tmp621=_stmttmp2D;_tmp622=_tmp621.f1;_tmp623=_tmp621.f2;_tmp624=_tmp621.f3;_tmp625=_tmp621.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC40;struct Cyc_Absyn_PtrAtts _tmpC3F;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC3E;struct Cyc_List_List*_tmpC3D;ans=((_tmpC3D=_region_malloc(yyr,sizeof(*_tmpC3D)),((_tmpC3D->hd=(void*)((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC3E.tag=2,((_tmpC3E.f1=((_tmpC3F.rgn=_tmp625,((_tmpC3F.nullable=_tmp622,((_tmpC3F.bounds=_tmp623,((_tmpC3F.zero_term=_tmp624,((_tmpC3F.ptrloc=ptrloc,_tmpC3F)))))))))),((_tmpC3E.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC3E)))))),_tmpC40)))),((_tmpC3D->tl=ans,_tmpC3D))))));}
yyval=Cyc_YY29(ans);
# 2053
break;};};}case 187: _LL2E7:
# 2055
 yyval=Cyc_YY57(0);
break;case 188: _LL2E8: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2056 "parse.y"
{struct Cyc_List_List*_tmpC41;yyval=Cyc_YY57(((_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC41->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC41)))))));}
break;}case 189: _LL2E9: {
# 2059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2061 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC44;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC43;yyval=Cyc_YY56((void*)((_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43[0]=((_tmpC44.tag=0,((_tmpC44.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC44)))),_tmpC43)))));}
break;}case 190: _LL2EA: {
# 2064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2063 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC47;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC46;yyval=Cyc_YY56((void*)((_tmpC46=_region_malloc(yyr,sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC47.tag=1,((_tmpC47.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC47)))),_tmpC46)))));}
break;}case 191: _LL2EB: {
# 2066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2065 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC4A;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC49;yyval=Cyc_YY56((void*)((_tmpC49=_region_malloc(yyr,sizeof(*_tmpC49)),((_tmpC49[0]=((_tmpC4A.tag=2,_tmpC4A)),_tmpC49)))));}
break;}case 192: _LL2EC: {
# 2068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2067 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC4D;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC4C;yyval=Cyc_YY56((void*)((_tmpC4C=_region_malloc(yyr,sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=3,_tmpC4D)),_tmpC4C)))));}
break;}case 193: _LL2ED: {
# 2070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2069 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC50;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC4F;yyval=Cyc_YY56((void*)((_tmpC4F=_region_malloc(yyr,sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=4,_tmpC50)),_tmpC4F)))));}
break;}case 194: _LL2EE: {
# 2072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2071 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC53;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC52;yyval=Cyc_YY56((void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=5,_tmpC53)),_tmpC52)))));}
break;}case 195: _LL2EF: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2073 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC56;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC55;yyval=Cyc_YY56((void*)((_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=6,_tmpC56)),_tmpC55)))));}
break;}case 196: _LL2F0: {
# 2076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2075 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC59;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC58;yyval=Cyc_YY56((void*)((_tmpC58=_region_malloc(yyr,sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=7,_tmpC59)),_tmpC58)))));}
break;}case 197: _LL2F1: {
# 2078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2081 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5A;yyval=Cyc_YY1(((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->f1=loc,((_tmpC5A->f2=Cyc_Absyn_true_conref,((_tmpC5A->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5A)))))))));}
# 2084
break;}case 198: _LL2F2: {
# 2086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2085 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5B;yyval=Cyc_YY1(((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->f1=loc,((_tmpC5B->f2=Cyc_Absyn_false_conref,((_tmpC5B->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5B)))))))));}
# 2088
break;}case 199: _LL2F3: {
# 2090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2089 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5C;yyval=Cyc_YY1(((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C->f1=loc,((_tmpC5C->f2=Cyc_Absyn_true_conref,((_tmpC5C->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC5C)))))))));}
# 2092
break;}case 200: _LL2F4:
# 2094
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 201: _LL2F5: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2095 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC5F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC5E;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E[0]=((_tmpC5F.tag=1,((_tmpC5F.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC5F)))),_tmpC5E))))));}
break;}case 202: _LL2F6:
# 2098
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 203: _LL2F7: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2099 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 204: _LL2F8: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2100 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 205: _LL2F9:
# 2103
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 206: _LL2FA: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2106 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 207: _LL2FB: {
# 2109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2107 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 208: _LL2FC:
# 2110
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 209: _LL2FD: {
# 2113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2112 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 210: _LL2FE: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2117 "parse.y"
{struct _tuple26*_tmpC60;yyval=Cyc_YY42(((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC60->f2=0,((_tmpC60->f3=0,((_tmpC60->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC60->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC60)))))))))))));}
break;}case 211: _LL2FF: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2119 "parse.y"
{struct _tuple26*_tmpC61;yyval=Cyc_YY42(((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC61->f2=1,((_tmpC61->f3=0,((_tmpC61->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC61->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC61)))))))))))));}
break;}case 212: _LL300: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2122 "parse.y"
struct _tuple8*_stmttmp2B=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp643;struct Cyc_Absyn_Tqual _tmp644;void*_tmp645;struct _tuple8*_tmp642=_stmttmp2B;_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=_tmp642->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC62;struct Cyc_Absyn_VarargInfo*_tmp646=(_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->name=_tmp643,((_tmpC62->tq=_tmp644,((_tmpC62->type=_tmp645,((_tmpC62->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC62)))))))));
{struct _tuple26*_tmpC63;yyval=Cyc_YY42(((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->f1=0,((_tmpC63->f2=0,((_tmpC63->f3=_tmp646,((_tmpC63->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC63->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC63)))))))))))));}
# 2126
break;};}case 213: _LL301: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2128 "parse.y"
struct _tuple8*_stmttmp2A=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp64A;struct Cyc_Absyn_Tqual _tmp64B;void*_tmp64C;struct _tuple8*_tmp649=_stmttmp2A;_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;_tmp64C=_tmp649->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC64;struct Cyc_Absyn_VarargInfo*_tmp64D=(_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->name=_tmp64A,((_tmpC64->tq=_tmp64B,((_tmpC64->type=_tmp64C,((_tmpC64->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC64)))))))));
{struct _tuple26*_tmpC65;yyval=Cyc_YY42(((_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC65->f2=0,((_tmpC65->f3=_tmp64D,((_tmpC65->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC65->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC65)))))))))))));}
# 2132
break;};}case 214: _LL302: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2136 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC68;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC67;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67[0]=((_tmpC68.tag=1,((_tmpC68.f1=0,_tmpC68)))),_tmpC67))))));}
break;}case 215: _LL303: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2137 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 216: _LL304:
# 2140
 yyval=Cyc_YY52(0);
break;case 217: _LL305: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2141 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC6B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC6A;yyval=Cyc_YY52((void*)((_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=24,((_tmpC6B.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC6B)))),_tmpC6A)))));}
break;}case 218: _LL306:
# 2144
 yyval=Cyc_YY53(0);
break;case 219: _LL307: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2146 "parse.y"
yyval=yyyyvsp[1];
break;}case 220: _LL308: {
# 2149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2154 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC6E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp654=(_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=2,((_tmpC6E.f1=0,((_tmpC6E.f2=& Cyc_Tcutil_trk,_tmpC6E)))))),_tmpC6D)));
struct _dyneither_ptr _tmp655=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp656=Cyc_Parse_id2type(_tmp655,(void*)_tmp654);
{struct _tuple29*_tmpC78;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC77;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC76;struct Cyc_List_List*_tmpC75;yyval=Cyc_YY53(((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->hd=((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78->f1=(void*)((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76[0]=((_tmpC77.tag=24,((_tmpC77.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC77)))),_tmpC76)))),((_tmpC78->f2=_tmp656,_tmpC78)))))),((_tmpC75->tl=0,_tmpC75)))))));}
# 2159
break;}case 221: _LL309: {
# 2161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2161 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC7B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC7A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65D=(_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A[0]=((_tmpC7B.tag=2,((_tmpC7B.f1=0,((_tmpC7B.f2=& Cyc_Tcutil_trk,_tmpC7B)))))),_tmpC7A)));
struct _dyneither_ptr _tmp65E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp65F=Cyc_Parse_id2type(_tmp65E,(void*)_tmp65D);
{struct _tuple29*_tmpC85;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC84;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC83;struct Cyc_List_List*_tmpC82;yyval=Cyc_YY53(((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->hd=((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->f1=(void*)((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83[0]=((_tmpC84.tag=24,((_tmpC84.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC84)))),_tmpC83)))),((_tmpC85->f2=_tmp65F,_tmpC85)))))),((_tmpC82->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC82)))))));}
# 2166
break;}case 222: _LL30A:
# 2168
 yyval=Cyc_YY34(0);
break;case 223: _LL30B: {
# 2171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2172 "parse.y"
{const char*_tmpC86;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC86="inject",_tag_dyneither(_tmpC86,sizeof(char),7))))!= 0){
const char*_tmpC87;Cyc_Parse_err(((_tmpC87="missing type in function declaration",_tag_dyneither(_tmpC87,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2176
break;}case 224: _LL30C: {
# 2178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2179 "parse.y"
yyval=yyyyvsp[0];
break;}case 225: _LL30D: {
# 2182
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2180 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 226: _LL30E: {
# 2183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2184 "parse.y"
yyval=Cyc_YY43(0);
break;}case 227: _LL30F: {
# 2187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2185 "parse.y"
yyval=yyyyvsp[1];
break;}case 228: _LL310: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2187 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC8D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC8C;struct Cyc_List_List*_tmpC8B;yyval=Cyc_YY43(((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->hd=(void*)((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8C.tag=25,((_tmpC8C.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC8C)))),_tmpC8D)))),((_tmpC8B->tl=0,_tmpC8B)))))));}
break;}case 229: _LL311: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2189 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC8E;yyval=Cyc_YY43(((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpC8E->tl=0,_tmpC8E)))))));}
# 2192
break;}case 230: _LL312: {
# 2194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC94;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC93;struct Cyc_List_List*_tmpC92;yyval=Cyc_YY43(((_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->hd=(void*)((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94[0]=((_tmpC93.tag=23,((_tmpC93.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC93)))),_tmpC94)))),((_tmpC92->tl=0,_tmpC92)))))));}
# 2200
break;}case 231: _LL313: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2201 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC9A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC99;struct Cyc_List_List*_tmpC98;yyval=Cyc_YY43(((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=(void*)((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC99.tag=23,((_tmpC99.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC99)))),_tmpC9A)))),((_tmpC98->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpC98)))))));}
# 2204
break;}case 232: _LL314: {
# 2206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpC9B;yyval=Cyc_YY41(((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpC9B->tl=0,_tmpC9B)))))));}
break;}case 233: _LL315: {
# 2212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2211 "parse.y"
{struct Cyc_List_List*_tmpC9C;yyval=Cyc_YY41(((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpC9C->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC9C)))))));}
break;}case 234: _LL316: {
# 2214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _stmttmp27=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp675;struct Cyc_Parse_Type_specifier _tmp676;struct Cyc_List_List*_tmp677;struct _tuple25 _tmp674=_stmttmp27;_tmp675=_tmp674.f1;_tmp676=_tmp674.f2;_tmp677=_tmp674.f3;
if(_tmp675.loc == 0)_tmp675.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _stmttmp28=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp679;struct Cyc_List_List*_tmp67A;struct Cyc_Parse_Declarator _tmp678=_stmttmp28;_tmp679=_tmp678.id;_tmp67A=_tmp678.tms;{
void*_tmp67B=Cyc_Parse_speclist2typ(_tmp676,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _stmttmp29=Cyc_Parse_apply_tms(_tmp675,_tmp67B,_tmp677,_tmp67A);struct Cyc_Absyn_Tqual _tmp67D;void*_tmp67E;struct Cyc_List_List*_tmp67F;struct Cyc_List_List*_tmp680;struct _tuple13 _tmp67C=_stmttmp29;_tmp67D=_tmp67C.f1;_tmp67E=_tmp67C.f2;_tmp67F=_tmp67C.f3;_tmp680=_tmp67C.f4;
if(_tmp67F != 0){
const char*_tmpC9D;Cyc_Parse_err(((_tmpC9D="parameter with bad type params",_tag_dyneither(_tmpC9D,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp679)){
const char*_tmpC9E;Cyc_Parse_err(((_tmpC9E="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC9E,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp679).f2;
if(_tmp680 != 0){
const char*_tmpCA1;void*_tmpCA0;(_tmpCA0=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA1="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCA1,sizeof(char),40))),_tag_dyneither(_tmpCA0,sizeof(void*),0)));}
{struct _tuple8*_tmpCA2;yyval=Cyc_YY40(((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->f1=idopt,((_tmpCA2->f2=_tmp67D,((_tmpCA2->f3=_tmp67E,_tmpCA2)))))))));}
# 2231
break;};};};}case 235: _LL317: {
# 2233
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2235
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2232 "parse.y"
struct _tuple25 _stmttmp26=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp687;struct Cyc_Parse_Type_specifier _tmp688;struct Cyc_List_List*_tmp689;struct _tuple25 _tmp686=_stmttmp26;_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;_tmp689=_tmp686.f3;
if(_tmp687.loc == 0)_tmp687.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp68A=Cyc_Parse_speclist2typ(_tmp688,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp689 != 0){
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA5="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCA5,sizeof(char),38))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}
{struct _tuple8*_tmpCA6;yyval=Cyc_YY40(((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6->f1=0,((_tmpCA6->f2=_tmp687,((_tmpCA6->f3=_tmp68A,_tmpCA6)))))))));}
# 2239
break;};}case 236: _LL318: {
# 2241
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2240 "parse.y"
struct _tuple25 _stmttmp24=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp68F;struct Cyc_Parse_Type_specifier _tmp690;struct Cyc_List_List*_tmp691;struct _tuple25 _tmp68E=_stmttmp24;_tmp68F=_tmp68E.f1;_tmp690=_tmp68E.f2;_tmp691=_tmp68E.f3;
if(_tmp68F.loc == 0)_tmp68F.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp692=Cyc_Parse_speclist2typ(_tmp690,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp693=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _stmttmp25=Cyc_Parse_apply_tms(_tmp68F,_tmp692,_tmp691,_tmp693);struct Cyc_Absyn_Tqual _tmp695;void*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_List_List*_tmp698;struct _tuple13 _tmp694=_stmttmp25;_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;_tmp697=_tmp694.f3;_tmp698=_tmp694.f4;
if(_tmp697 != 0){
const char*_tmpCA9;void*_tmpCA8;(_tmpCA8=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA9="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCA9,sizeof(char),49))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}
# 2248
if(_tmp698 != 0){
const char*_tmpCAC;void*_tmpCAB;(_tmpCAB=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAC="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCAC,sizeof(char),38))),_tag_dyneither(_tmpCAB,sizeof(void*),0)));}
{struct _tuple8*_tmpCAD;yyval=Cyc_YY40(((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->f1=0,((_tmpCAD->f2=_tmp695,((_tmpCAD->f3=_tmp696,_tmpCAD)))))))));}
# 2252
break;};}case 237: _LL319: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2256 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 238: _LL31A: {
# 2259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2260 "parse.y"
{struct _dyneither_ptr*_tmpCB0;struct Cyc_List_List*_tmpCAF;yyval=Cyc_YY39(((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->hd=((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCB0)))),((_tmpCAF->tl=0,_tmpCAF)))))));}
break;}case 239: _LL31B: {
# 2263
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2262 "parse.y"
{struct _dyneither_ptr*_tmpCB3;struct Cyc_List_List*_tmpCB2;yyval=Cyc_YY39(((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->hd=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCB3)))),((_tmpCB2->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCB2)))))));}
break;}case 240: _LL31C: {
# 2265
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2266 "parse.y"
yyval=yyyyvsp[0];
break;}case 241: _LL31D: {
# 2269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2267 "parse.y"
yyval=yyyyvsp[0];
break;}case 242: _LL31E: {
# 2270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2272 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCB6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCB5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=35,((_tmpCB6.f1=0,((_tmpCB6.f2=0,_tmpCB6)))))),_tmpCB5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL31F: {
# 2275
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2274 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCB9;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCB8;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=35,((_tmpCB9.f1=0,((_tmpCB9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCB9)))))),_tmpCB8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL320: {
# 2277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2276 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBC;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=35,((_tmpCBC.f1=0,((_tmpCBC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBC)))))),_tmpCBB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL321: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2278 "parse.y"
struct _dyneither_ptr*_tmpCBF;struct _tuple0*_tmpCBE;struct Cyc_Absyn_Vardecl*_tmp6A8=Cyc_Absyn_new_vardecl(((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->f1=Cyc_Absyn_Loc_n,((_tmpCBE->f2=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCBF)))),_tmpCBE)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2281
(_tmp6A8->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCC2;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCC1;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=26,((_tmpCC2.f1=_tmp6A8,((_tmpCC2.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC2.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCC2.f4=0,_tmpCC2)))))))))),_tmpCC1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2284
break;}case 246: _LL322: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2285 "parse.y"
void*_tmp6AD=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCC5;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCC4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=27,((_tmpCC5.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC5.f2=_tmp6AD,((_tmpCC5.f3=0,_tmpCC5)))))))),_tmpCC4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2288
break;}case 247: _LL323: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpCC8;struct Cyc_List_List*_tmpCC7;yyval=Cyc_YY7(((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->hd=((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->f1=0,((_tmpCC8->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCC8)))))),((_tmpCC7->tl=0,_tmpCC7)))))));}
break;}case 248: _LL324: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2295 "parse.y"
{struct _tuple30*_tmpCCB;struct Cyc_List_List*_tmpCCA;yyval=Cyc_YY7(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->hd=((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCCB->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCCB)))))),((_tmpCCA->tl=0,_tmpCCA)))))));}
break;}case 249: _LL325: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2297 "parse.y"
{struct _tuple30*_tmpCCE;struct Cyc_List_List*_tmpCCD;yyval=Cyc_YY7(((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->hd=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=0,((_tmpCCE->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCCE)))))),((_tmpCCD->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCCD)))))));}
break;}case 250: _LL326: {
# 2300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2299 "parse.y"
{struct _tuple30*_tmpCD1;struct Cyc_List_List*_tmpCD0;yyval=Cyc_YY7(((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD1->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCD1)))))),((_tmpCD0->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCD0)))))));}
break;}case 251: _LL327: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2303 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 252: _LL328: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2308 "parse.y"
{struct Cyc_List_List*_tmpCD2;yyval=Cyc_YY44(((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCD2->tl=0,_tmpCD2)))))));}
break;}case 253: _LL329: {
# 2311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2309 "parse.y"
{struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY44(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCD3->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCD3)))))));}
break;}case 254: _LL32A: {
# 2312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2313 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCD6;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCD5;yyval=Cyc_YY45((void*)((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5[0]=((_tmpCD6.tag=0,((_tmpCD6.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCD6)))),_tmpCD5)))));}
break;}case 255: _LL32B: {
# 2316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2314 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCDC;struct _dyneither_ptr*_tmpCDB;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCDA;yyval=Cyc_YY45((void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDC.tag=1,((_tmpCDC.f1=((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCDB)))),_tmpCDC)))),_tmpCDA)))));}
break;}case 256: _LL32C: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2319 "parse.y"
struct _tuple25 _stmttmp23=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C0;struct Cyc_Parse_Type_specifier _tmp6C1;struct Cyc_List_List*_tmp6C2;struct _tuple25 _tmp6BF=_stmttmp23;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;_tmp6C2=_tmp6BF.f3;{
void*_tmp6C3=Cyc_Parse_speclist2typ(_tmp6C1,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6C2 != 0){
const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCDF="ignoring attributes in type",_tag_dyneither(_tmpCDF,sizeof(char),28))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}
{struct _tuple8*_tmpCE0;yyval=Cyc_YY40(((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->f1=0,((_tmpCE0->f2=_tmp6C0,((_tmpCE0->f3=_tmp6C3,_tmpCE0)))))))));}
# 2325
break;};}case 257: _LL32D: {
# 2327
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2326 "parse.y"
struct _tuple25 _stmttmp22=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C8;struct Cyc_Parse_Type_specifier _tmp6C9;struct Cyc_List_List*_tmp6CA;struct _tuple25 _tmp6C7=_stmttmp22;_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;_tmp6CA=_tmp6C7.f3;{
void*_tmp6CB=Cyc_Parse_speclist2typ(_tmp6C9,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6CC=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6CD=Cyc_Parse_apply_tms(_tmp6C8,_tmp6CB,_tmp6CA,_tmp6CC);
if(_tmp6CD.f3 != 0){
# 2332
const char*_tmpCE3;void*_tmpCE2;(_tmpCE2=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE3="bad type params, ignoring",_tag_dyneither(_tmpCE3,sizeof(char),26))),_tag_dyneither(_tmpCE2,sizeof(void*),0)));}
if(_tmp6CD.f4 != 0){
const char*_tmpCE6;void*_tmpCE5;(_tmpCE5=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE6="bad specifiers, ignoring",_tag_dyneither(_tmpCE6,sizeof(char),25))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}
{struct _tuple8*_tmpCE7;yyval=Cyc_YY40(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->f1=0,((_tmpCE7->f2=_tmp6CD.f1,((_tmpCE7->f3=_tmp6CD.f2,_tmpCE7)))))))));}
# 2337
break;};}case 258: _LL32E: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2340 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 259: _LL32F: {
# 2343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2341 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCEA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCE9;yyval=Cyc_YY47((void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCEA.tag=24,((_tmpCEA.f1=0,_tmpCEA)))),_tmpCE9)))));}
break;}case 260: _LL330: {
# 2344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2342 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCED;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEC;yyval=Cyc_YY47((void*)((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=24,((_tmpCED.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCED)))),_tmpCEC)))));}
break;}case 261: _LL331: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2343 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCEF;yyval=Cyc_YY47((void*)((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=25,((_tmpCF0.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCF0)))),_tmpCEF)))));}
break;}case 262: _LL332: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2344 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF6;struct Cyc_List_List*_tmpCF5;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF4;yyval=Cyc_YY47((void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4[0]=((_tmpCF6.tag=24,((_tmpCF6.f1=((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF5->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCF5)))))),_tmpCF6)))),_tmpCF4)))));}
break;}case 263: _LL333: {
# 2347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCF7;yyval=Cyc_YY43(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF7->tl=0,_tmpCF7)))))));}
break;}case 264: _LL334: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2351 "parse.y"
{struct Cyc_List_List*_tmpCF8;yyval=Cyc_YY43(((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpCF8->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCF8)))))));}
break;}case 265: _LL335: {
# 2354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2356 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCF9;yyval=Cyc_YY33(((_tmpCF9.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCF9)));}
break;}case 266: _LL336: {
# 2359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2358 "parse.y"
yyval=yyyyvsp[0];
break;}case 267: _LL337: {
# 2361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2360 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCFA;yyval=Cyc_YY33(((_tmpCFA.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpCFA)));}
break;}case 268: _LL338: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2365 "parse.y"
yyval=yyyyvsp[1];
break;}case 269: _LL339: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2367 "parse.y"
{struct Cyc_List_List*_tmpD04;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD03;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD02;struct Cyc_Parse_Abstractdeclarator _tmpD01;yyval=Cyc_YY33(((_tmpD01.tms=((_tmpD04=_region_malloc(yyr,sizeof(*_tmpD04)),((_tmpD04->hd=(void*)((_tmpD02=_region_malloc(yyr,sizeof(*_tmpD02)),((_tmpD02[0]=((_tmpD03.tag=0,((_tmpD03.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD03.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD03)))))),_tmpD02)))),((_tmpD04->tl=0,_tmpD04)))))),_tmpD01)));}
break;}case 270: _LL33A: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2369 "parse.y"
{struct Cyc_List_List*_tmpD0E;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD0D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD0C;struct Cyc_Parse_Abstractdeclarator _tmpD0B;yyval=Cyc_YY33(((_tmpD0B.tms=((_tmpD0E=_region_malloc(yyr,sizeof(*_tmpD0E)),((_tmpD0E->hd=(void*)((_tmpD0C=_region_malloc(yyr,sizeof(*_tmpD0C)),((_tmpD0C[0]=((_tmpD0D.tag=0,((_tmpD0D.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD0D.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD0D)))))),_tmpD0C)))),((_tmpD0E->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD0E)))))),_tmpD0B)));}
break;}case 271: _LL33B: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2371 "parse.y"
{struct Cyc_List_List*_tmpD18;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD17;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD16;struct Cyc_Parse_Abstractdeclarator _tmpD15;yyval=Cyc_YY33(((_tmpD15.tms=((_tmpD18=_region_malloc(yyr,sizeof(*_tmpD18)),((_tmpD18->hd=(void*)((_tmpD16=_region_malloc(yyr,sizeof(*_tmpD16)),((_tmpD16[0]=((_tmpD17.tag=1,((_tmpD17.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD17.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD17.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD17)))))))),_tmpD16)))),((_tmpD18->tl=0,_tmpD18)))))),_tmpD15)));}
break;}case 272: _LL33C: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2373 "parse.y"
{struct Cyc_List_List*_tmpD22;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD21;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD20;struct Cyc_Parse_Abstractdeclarator _tmpD1F;yyval=Cyc_YY33(((_tmpD1F.tms=((_tmpD22=_region_malloc(yyr,sizeof(*_tmpD22)),((_tmpD22->hd=(void*)((_tmpD20=_region_malloc(yyr,sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD21.tag=1,((_tmpD21.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD21.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD21.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD21)))))))),_tmpD20)))),((_tmpD22->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD22)))))),_tmpD1F)));}
# 2376
break;}case 273: _LL33D: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2377 "parse.y"
{struct Cyc_List_List*_tmpD37;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD36;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD35;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD34;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD33;struct Cyc_Parse_Abstractdeclarator _tmpD32;yyval=Cyc_YY33(((_tmpD32.tms=((_tmpD37=_region_malloc(yyr,sizeof(*_tmpD37)),((_tmpD37->hd=(void*)((_tmpD33=_region_malloc(yyr,sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD36.tag=3,((_tmpD36.f1=(void*)((_tmpD34=_region_malloc(yyr,sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD35.tag=1,((_tmpD35.f1=0,((_tmpD35.f2=0,((_tmpD35.f3=0,((_tmpD35.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD35.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD35)))))))))))),_tmpD34)))),_tmpD36)))),_tmpD33)))),((_tmpD37->tl=0,_tmpD37)))))),_tmpD32)));}
# 2379
break;}case 274: _LL33E: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2380 "parse.y"
struct _tuple26*_stmttmp21=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp6F7;int _tmp6F8;struct Cyc_Absyn_VarargInfo*_tmp6F9;void*_tmp6FA;struct Cyc_List_List*_tmp6FB;struct _tuple26*_tmp6F6=_stmttmp21;_tmp6F7=_tmp6F6->f1;_tmp6F8=_tmp6F6->f2;_tmp6F9=_tmp6F6->f3;_tmp6FA=_tmp6F6->f4;_tmp6FB=_tmp6F6->f5;
{struct Cyc_List_List*_tmpD4C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD4B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD4A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD49;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD48;struct Cyc_Parse_Abstractdeclarator _tmpD47;yyval=Cyc_YY33(((_tmpD47.tms=((_tmpD4C=_region_malloc(yyr,sizeof(*_tmpD4C)),((_tmpD4C->hd=(void*)((_tmpD48=_region_malloc(yyr,sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD4B.tag=3,((_tmpD4B.f1=(void*)((_tmpD49=_region_malloc(yyr,sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=1,((_tmpD4A.f1=_tmp6F7,((_tmpD4A.f2=_tmp6F8,((_tmpD4A.f3=_tmp6F9,((_tmpD4A.f4=_tmp6FA,((_tmpD4A.f5=_tmp6FB,_tmpD4A)))))))))))),_tmpD49)))),_tmpD4B)))),_tmpD48)))),((_tmpD4C->tl=0,_tmpD4C)))))),_tmpD47)));}
# 2383
break;}case 275: _LL33F: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2384 "parse.y"
{struct Cyc_List_List*_tmpD61;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD60;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD5F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5D;struct Cyc_Parse_Abstractdeclarator _tmpD5C;yyval=Cyc_YY33(((_tmpD5C.tms=((_tmpD61=_region_malloc(yyr,sizeof(*_tmpD61)),((_tmpD61->hd=(void*)((_tmpD5D=_region_malloc(yyr,sizeof(*_tmpD5D)),((_tmpD5D[0]=((_tmpD60.tag=3,((_tmpD60.f1=(void*)((_tmpD5E=_region_malloc(yyr,sizeof(*_tmpD5E)),((_tmpD5E[0]=((_tmpD5F.tag=1,((_tmpD5F.f1=0,((_tmpD5F.f2=0,((_tmpD5F.f3=0,((_tmpD5F.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD5F.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD5F)))))))))))),_tmpD5E)))),_tmpD60)))),_tmpD5D)))),((_tmpD61->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD61)))))),_tmpD5C)));}
# 2387
break;}case 276: _LL340: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2388 "parse.y"
struct _tuple26*_stmttmp20=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp709;int _tmp70A;struct Cyc_Absyn_VarargInfo*_tmp70B;void*_tmp70C;struct Cyc_List_List*_tmp70D;struct _tuple26*_tmp708=_stmttmp20;_tmp709=_tmp708->f1;_tmp70A=_tmp708->f2;_tmp70B=_tmp708->f3;_tmp70C=_tmp708->f4;_tmp70D=_tmp708->f5;
{struct Cyc_List_List*_tmpD76;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD75;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD74;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD73;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD72;struct Cyc_Parse_Abstractdeclarator _tmpD71;yyval=Cyc_YY33(((_tmpD71.tms=((_tmpD76=_region_malloc(yyr,sizeof(*_tmpD76)),((_tmpD76->hd=(void*)((_tmpD72=_region_malloc(yyr,sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD75.tag=3,((_tmpD75.f1=(void*)((_tmpD73=_region_malloc(yyr,sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=1,((_tmpD74.f1=_tmp709,((_tmpD74.f2=_tmp70A,((_tmpD74.f3=_tmp70B,((_tmpD74.f4=_tmp70C,((_tmpD74.f5=_tmp70D,_tmpD74)))))))))))),_tmpD73)))),_tmpD75)))),_tmpD72)))),((_tmpD76->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD76)))))),_tmpD71)));}
# 2392
break;}case 277: _LL341: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2394 "parse.y"
struct Cyc_List_List*_tmp714=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD80;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD7F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD7E;struct Cyc_Parse_Abstractdeclarator _tmpD7D;yyval=Cyc_YY33(((_tmpD7D.tms=((_tmpD80=_region_malloc(yyr,sizeof(*_tmpD80)),((_tmpD80->hd=(void*)((_tmpD7E=_region_malloc(yyr,sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7F.tag=4,((_tmpD7F.f1=_tmp714,((_tmpD7F.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD7F.f3=0,_tmpD7F)))))))),_tmpD7E)))),((_tmpD80->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD80)))))),_tmpD7D)));}
# 2398
break;}case 278: _LL342: {
# 2400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2399 "parse.y"
{struct Cyc_List_List*_tmpD8A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD89;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD88;struct Cyc_Parse_Abstractdeclarator _tmpD87;yyval=Cyc_YY33(((_tmpD87.tms=((_tmpD8A=_region_malloc(yyr,sizeof(*_tmpD8A)),((_tmpD8A->hd=(void*)((_tmpD88=_region_malloc(yyr,sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=5,((_tmpD89.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD89.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpD89)))))),_tmpD88)))),((_tmpD8A->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD8A)))))),_tmpD87)));}
# 2401
break;}case 279: _LL343: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2405 "parse.y"
yyval=yyyyvsp[0];
break;}case 280: _LL344: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2406 "parse.y"
yyval=yyyyvsp[0];
break;}case 281: _LL345: {
# 2409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2407 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL346: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2408 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL347: {
# 2411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2409 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL348: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2410 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL349: {
# 2413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2413 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD8D;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD8C;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C[0]=((_tmpD8D.tag=16,((_tmpD8D.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD8D)))),_tmpD8C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 286: _LL34A:
# 2416
 yyval=Cyc_YY5(0);
break;case 287: _LL34B: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2419 "parse.y"
{const char*_tmpD8E;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD8E="open",_tag_dyneither(_tmpD8E,sizeof(char),5))))!= 0){
const char*_tmpD8F;Cyc_Parse_err(((_tmpD8F="expecting `open'",_tag_dyneither(_tmpD8F,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2423
break;}case 288: _LL34C: {
# 2425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD95;struct _dyneither_ptr*_tmpD94;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD93;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93[0]=((_tmpD95.tag=13,((_tmpD95.f1=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD94)))),((_tmpD95.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD95)))))),_tmpD93)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 289: _LL34D: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2432 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL34E: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2433 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 291: _LL34F: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2438 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL350: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2439 "parse.y"
yyval=yyyyvsp[1];
break;}case 293: _LL351: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2444 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 294: _LL352: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2445 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 295: _LL353: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2446 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD9B;struct _dyneither_ptr*_tmpD9A;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD99;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9B.tag=13,((_tmpD9B.f1=((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD9A)))),((_tmpD9B.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpD9B)))))),_tmpD99)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2448
break;}case 296: _LL354: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2448 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDA1;struct _dyneither_ptr*_tmpDA0;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD9F;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpDA1.tag=13,((_tmpDA1.f1=((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDA0)))),((_tmpDA1.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDA1)))))),_tmpD9F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2450
break;}case 297: _LL355: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2450 "parse.y"
yyval=yyyyvsp[0];
break;}case 298: _LL356: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2451 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL357: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2452 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA4;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA3;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3[0]=((_tmpDA4.tag=1,((_tmpDA4.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA4)))),_tmpDA3)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 300: _LL358: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2455 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA7;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA6;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6[0]=((_tmpDA7.tag=1,((_tmpDA7.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA7)))),_tmpDA6)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 301: _LL359: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2460 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 302: _LL35A: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2462 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL35B: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL35C: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2471 "parse.y"
struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp72E,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL35D: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2475 "parse.y"
struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp72F,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2478
break;}case 306: _LL35E: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2481 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL35F:
# 2484
 yyval=Cyc_YY11(0);
break;case 308: _LL360: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2497 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAA;struct Cyc_List_List*_tmpDA9;yyval=Cyc_YY11(((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->hd=((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDAA->pat_vars=0,((_tmpDAA->where_clause=0,((_tmpDAA->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDAA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAA)))))))))))),((_tmpDA9->tl=0,_tmpDA9)))))));}
# 2500
break;}case 309: _LL361: {
# 2502
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2501 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAD;struct Cyc_List_List*_tmpDAC;yyval=Cyc_YY11(((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDAD->pat_vars=0,((_tmpDAD->where_clause=0,((_tmpDAD->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDAD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAD)))))))))))),((_tmpDAC->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDAC)))))));}
break;}case 310: _LL362: {
# 2506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2505 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB0;struct Cyc_List_List*_tmpDAF;yyval=Cyc_YY11(((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->hd=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB0->pat_vars=0,((_tmpDB0->where_clause=0,((_tmpDB0->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDB0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB0)))))))))))),((_tmpDAF->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDAF)))))));}
break;}case 311: _LL363: {
# 2508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2507 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB3;struct Cyc_List_List*_tmpDB2;yyval=Cyc_YY11(((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->hd=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB3->pat_vars=0,((_tmpDB3->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB3->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDB3->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB3)))))))))))),((_tmpDB2->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDB2)))))));}
break;}case 312: _LL364: {
# 2512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2511 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB6;struct Cyc_List_List*_tmpDB5;yyval=Cyc_YY11(((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->hd=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB6->pat_vars=0,((_tmpDB6->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB6->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDB6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB6)))))))))))),((_tmpDB5->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDB5)))))));}
break;}case 313: _LL365: {
# 2514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2518 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL366: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2522 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL367: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2526 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL368: {
# 2530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2529 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL369: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2532 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL36A: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2535 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL36B: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2538 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL36C: {
# 2542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2541 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL36D: {
# 2545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2544 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL36E: {
# 2548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2547 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL36F: {
# 2551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2550 "parse.y"
struct Cyc_List_List*_tmp73A=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73A,_tmp73B));
# 2555
break;}case 324: _LL370: {
# 2557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2556 "parse.y"
struct Cyc_List_List*_tmp73C=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73C,_tmp73D));
# 2561
break;}case 325: _LL371: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2562 "parse.y"
struct Cyc_List_List*_tmp73E=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73E,_tmp73F));
# 2567
break;}case 326: _LL372: {
# 2569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2568 "parse.y"
struct Cyc_List_List*_tmp740=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp741=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp740,_tmp741));
# 2573
break;}case 327: _LL373: {
# 2575
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2577
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDB7;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDB7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 328: _LL374: {
# 2581
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2579 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL375: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2580 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL376: {
# 2583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2581 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL377: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2582 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL378: {
# 2585
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2584 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL379: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2585 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL37A: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2587 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL37B: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2596 "parse.y"
yyval=yyyyvsp[0];
break;}case 336: _LL37C: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599 "parse.y"
yyval=yyyyvsp[0];
break;}case 337: _LL37D: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2601 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 338: _LL37E: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL37F: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL380: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL381: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL382: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL383: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL384: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2619 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL385: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2621 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL386: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2624 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL387: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2626 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL388: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2629 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL389: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2631 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL38A: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2633 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL38B: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2636 "parse.y"
yyval=yyyyvsp[0];
break;}case 352: _LL38C: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2638 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL38D: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2640 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL38E: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2642 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL38F: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2644 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL390: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2647 "parse.y"
yyval=yyyyvsp[0];
break;}case 357: _LL391: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2649 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL392: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2651 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL393: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2654 "parse.y"
yyval=yyyyvsp[0];
break;}case 360: _LL394: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2656 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL395: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2658 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL396: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2661 "parse.y"
yyval=yyyyvsp[0];
break;}case 363: _LL397: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2663 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL398: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2665 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL399: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2667 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL39A: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2670 "parse.y"
yyval=yyyyvsp[0];
break;}case 367: _LL39B: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2672 "parse.y"
void*_tmp743=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp743,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2675
break;}case 368: _LL39C: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2678 "parse.y"
yyval=yyyyvsp[0];
break;}case 369: _LL39D: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2681 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 370: _LL39E: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2683 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 371: _LL39F: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2685 "parse.y"
void*_tmp744=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp744,(unsigned int)(yyyylsp[0]).first_line)));
# 2688
break;}case 372: _LL3A0: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2689 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 373: _LL3A1: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2691 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 374: _LL3A2: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2696 "parse.y"
yyval=yyyyvsp[0];
break;}case 375: _LL3A3: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2705 "parse.y"
yyval=yyyyvsp[0];
break;}case 376: _LL3A4: {
# 2708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2710 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 377: _LL3A5: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2712 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 378: _LL3A6: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2714 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_stmttmp1F=e->r;void*_tmp745=_stmttmp1F;enum Cyc_Absyn_Sign _tmp747;char _tmp748;enum Cyc_Absyn_Sign _tmp74A;short _tmp74B;enum Cyc_Absyn_Sign _tmp74D;int _tmp74E;struct _dyneither_ptr _tmp750;int _tmp751;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp746=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp746->tag != 0)goto _LL3AB;else{if(((_tmp746->f1).Char_c).tag != 2)goto _LL3AB;_tmp747=((struct _tuple3)((_tmp746->f1).Char_c).val).f1;_tmp748=((struct _tuple3)((_tmp746->f1).Char_c).val).f2;}}_LL3AA:
# 2718
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDBA;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDB9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB9=_cycalloc_atomic(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=10,((_tmpDBA.f1=_tmp748,_tmpDBA)))),_tmpDB9)))),e->loc));}goto _LL3A8;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp749=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp749->tag != 0)goto _LL3AD;else{if(((_tmp749->f1).Short_c).tag != 4)goto _LL3AD;_tmp74A=((struct _tuple4)((_tmp749->f1).Short_c).val).f1;_tmp74B=((struct _tuple4)((_tmp749->f1).Short_c).val).f2;}}_LL3AC:
# 2720
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDBD;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDBC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBC=_cycalloc_atomic(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=9,((_tmpDBD.f1=_tmp74A,((_tmpDBD.f2=(int)_tmp74B,_tmpDBD)))))),_tmpDBC)))),e->loc));}goto _LL3A8;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp74C->tag != 0)goto _LL3AF;else{if(((_tmp74C->f1).Int_c).tag != 5)goto _LL3AF;_tmp74D=((struct _tuple5)((_tmp74C->f1).Int_c).val).f1;_tmp74E=((struct _tuple5)((_tmp74C->f1).Int_c).val).f2;}}_LL3AE:
# 2722
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDC0;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDBF;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBF=_cycalloc_atomic(sizeof(*_tmpDBF)),((_tmpDBF[0]=((_tmpDC0.tag=9,((_tmpDC0.f1=_tmp74D,((_tmpDC0.f2=_tmp74E,_tmpDC0)))))),_tmpDBF)))),e->loc));}goto _LL3A8;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp74F->tag != 0)goto _LL3B1;else{if(((_tmp74F->f1).Float_c).tag != 7)goto _LL3B1;_tmp750=((struct _tuple7)((_tmp74F->f1).Float_c).val).f1;_tmp751=((struct _tuple7)((_tmp74F->f1).Float_c).val).f2;}}_LL3B0:
# 2724
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDC3;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDC2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=11,((_tmpDC3.f1=_tmp750,((_tmpDC3.f2=_tmp751,_tmpDC3)))))),_tmpDC2)))),e->loc));}goto _LL3A8;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp752=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp752->tag != 0)goto _LL3B3;else{if(((_tmp752->f1).Null_c).tag != 1)goto _LL3B3;}}_LL3B2:
# 2726
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A8;_LL3B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp753=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp753->tag != 0)goto _LL3B5;else{if(((_tmp753->f1).String_c).tag != 8)goto _LL3B5;}}_LL3B4:
# 2728
{const char*_tmpDC4;Cyc_Parse_err(((_tmpDC4="strings cannot occur within patterns",_tag_dyneither(_tmpDC4,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp754=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp754->tag != 0)goto _LL3B7;else{if(((_tmp754->f1).Wstring_c).tag != 9)goto _LL3B7;}}_LL3B6:
# 2730
{const char*_tmpDC5;Cyc_Parse_err(((_tmpDC5="strings cannot occur within patterns",_tag_dyneither(_tmpDC5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp755=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp745;if(_tmp755->tag != 0)goto _LL3B9;else{if(((_tmp755->f1).LongLong_c).tag != 6)goto _LL3B9;}}_LL3B8:
# 2732
{const char*_tmpDC6;Cyc_Parse_unimp(((_tmpDC6="long long's in patterns",_tag_dyneither(_tmpDC6,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B9:;_LL3BA: {
# 2734
const char*_tmpDC7;Cyc_Parse_err(((_tmpDC7="bad constant in case",_tag_dyneither(_tmpDC7,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A8:;}
# 2737
break;}case 379: _LL3A7: {
# 2739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2738 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDCA;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDC9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=((_tmpDCA.tag=14,((_tmpDCA.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDCA)))),_tmpDC9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 380: _LL3BB: {
# 2741
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2740 "parse.y"
{const char*_tmpDCB;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDCB="as",_tag_dyneither(_tmpDCB,sizeof(char),3))))!= 0){
const char*_tmpDCC;Cyc_Parse_err(((_tmpDCC="expecting `as'",_tag_dyneither(_tmpDCC,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDD6;struct _dyneither_ptr*_tmpDD5;struct _tuple0*_tmpDD4;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDD3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD6.tag=1,((_tmpDD6.f1=Cyc_Absyn_new_vardecl(((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->f1=Cyc_Absyn_Loc_n,((_tmpDD4->f2=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD5)))),_tmpDD4)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD6.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDD6)))))),_tmpDD3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2745
break;}case 381: _LL3BC: {
# 2747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2746 "parse.y"
struct _tuple22 _stmttmp1E=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp76B;int _tmp76C;struct _tuple22 _tmp76A=_stmttmp1E;_tmp76B=_tmp76A.f1;_tmp76C=_tmp76A.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDD9;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDD8;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDD9.tag=4,((_tmpDD9.f1=_tmp76B,((_tmpDD9.f2=_tmp76C,_tmpDD9)))))),_tmpDD8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2749
break;}case 382: _LL3BD: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2750 "parse.y"
struct _tuple22 _stmttmp1D=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp770;int _tmp771;struct _tuple22 _tmp76F=_stmttmp1D;_tmp770=_tmp76F.f1;_tmp771=_tmp76F.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDDC;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDDB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=15,((_tmpDDC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDDC.f2=_tmp770,((_tmpDDC.f3=_tmp771,_tmpDDC)))))))),_tmpDDB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2753
break;}case 383: _LL3BE: {
# 2755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2754 "parse.y"
struct _tuple22 _stmttmp1C=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp775;int _tmp776;struct _tuple22 _tmp774=_stmttmp1C;_tmp775=_tmp774.f1;_tmp776=_tmp774.f2;{
struct Cyc_List_List*_tmp777=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE2;struct Cyc_Absyn_AggrInfo*_tmpDE1;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE0;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=((_tmpDE2.tag=6,((_tmpDE2.f1=((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDE1->targs=0,_tmpDE1)))))),((_tmpDE2.f2=_tmp777,((_tmpDE2.f3=_tmp775,((_tmpDE2.f4=_tmp776,_tmpDE2)))))))))),_tmpDE0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2759
break;};}case 384: _LL3BF: {
# 2761
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2760 "parse.y"
struct _tuple22 _stmttmp1B=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp77C;int _tmp77D;struct _tuple22 _tmp77B=_stmttmp1B;_tmp77C=_tmp77B.f1;_tmp77D=_tmp77B.f2;{
struct Cyc_List_List*_tmp77E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE5;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE4;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE5.tag=6,((_tmpDE5.f1=0,((_tmpDE5.f2=_tmp77E,((_tmpDE5.f3=_tmp77C,((_tmpDE5.f4=_tmp77D,_tmpDE5)))))))))),_tmpDE4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2764
break;};}case 385: _LL3C0: {
# 2766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2765 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDE8;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDE7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE8.tag=5,((_tmpDE8.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDE8)))),_tmpDE7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 386: _LL3C1: {
# 2768
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2767 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF2;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF1;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDF0;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDEF;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF2.tag=5,((_tmpDF2.f1=Cyc_Absyn_new_pat((void*)((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=((_tmpDF1.tag=5,((_tmpDF1.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDF1)))),_tmpDF0)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDF2)))),_tmpDEF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 387: _LL3C2: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2769 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDFC;struct _dyneither_ptr*_tmpDFB;struct _tuple0*_tmpDFA;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDF9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFC.tag=2,((_tmpDFC.f1=Cyc_Absyn_new_vardecl(((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->f1=Cyc_Absyn_Loc_n,((_tmpDFA->f2=((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDFB)))),_tmpDFA)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDFC.f2=
# 2771
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDFC)))))),_tmpDF9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2773
break;}case 388: _LL3C3: {
# 2775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2774 "parse.y"
{const char*_tmpDFD;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDFD="as",_tag_dyneither(_tmpDFD,sizeof(char),3))))!= 0){
const char*_tmpDFE;Cyc_Parse_err(((_tmpDFE="expecting `as'",_tag_dyneither(_tmpDFE,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE08;struct _dyneither_ptr*_tmpE07;struct _tuple0*_tmpE06;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE05;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE08.tag=2,((_tmpE08.f1=Cyc_Absyn_new_vardecl(((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->f1=Cyc_Absyn_Loc_n,((_tmpE06->f2=((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE07)))),_tmpE06)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE08.f2=
# 2778
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE08)))))),_tmpE05)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2780
break;}case 389: _LL3C4: {
# 2782
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2781 "parse.y"
void*_tmp791=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE19;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE18;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE17;struct _dyneither_ptr*_tmpE16;struct _tuple0*_tmpE15;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE14;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE19.tag=3,((_tmpE19.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp791),((_tmpE19.f2=
Cyc_Absyn_new_vardecl(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=Cyc_Absyn_Loc_n,((_tmpE15->f2=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE16)))),_tmpE15)))))),(void*)(
(_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=((_tmpE18.tag=19,((_tmpE18.f1=_tmp791,_tmpE18)))),_tmpE17)))),0),_tmpE19)))))),_tmpE14)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2786
break;}case 390: _LL3C5: {
# 2788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2787 "parse.y"
struct Cyc_Absyn_Tvar*_tmp798=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE35;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE34;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE33;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE32;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE31;struct _dyneither_ptr*_tmpE30;struct _tuple0*_tmpE2F;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE2E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E[0]=((_tmpE35.tag=3,((_tmpE35.f1=_tmp798,((_tmpE35.f2=
Cyc_Absyn_new_vardecl(((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->f1=Cyc_Absyn_Loc_n,((_tmpE2F->f2=((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE30)))),_tmpE2F)))))),(void*)(
(_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE34.tag=19,((_tmpE34.f1=(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE33.tag=2,((_tmpE33.f1=_tmp798,_tmpE33)))),_tmpE32)))),_tmpE34)))),_tmpE31)))),0),_tmpE35)))))),_tmpE2E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2792
break;}case 391: _LL3C6: {
# 2794
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2795 "parse.y"
{struct _tuple22*_tmpE36;yyval=Cyc_YY13(((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE36->f2=0,_tmpE36)))))));}
break;}case 392: _LL3C7: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2796 "parse.y"
{struct _tuple22*_tmpE37;yyval=Cyc_YY13(((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE37->f2=1,_tmpE37)))))));}
break;}case 393: _LL3C8: {
# 2799
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2797 "parse.y"
{struct _tuple22*_tmpE38;yyval=Cyc_YY13(((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->f1=0,((_tmpE38->f2=1,_tmpE38)))))));}
break;}case 394: _LL3C9: {
# 2800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2802 "parse.y"
{struct Cyc_List_List*_tmpE39;yyval=Cyc_YY14(((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE39->tl=0,_tmpE39)))))));}
break;}case 395: _LL3CA: {
# 2805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2804 "parse.y"
{struct Cyc_List_List*_tmpE3A;yyval=Cyc_YY14(((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE3A->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE3A)))))));}
break;}case 396: _LL3CB: {
# 2807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2809 "parse.y"
{struct _tuple23*_tmpE3B;yyval=Cyc_YY15(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->f1=0,((_tmpE3B->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE3B)))))));}
break;}case 397: _LL3CC: {
# 2812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2811 "parse.y"
{struct _tuple23*_tmpE3C;yyval=Cyc_YY15(((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE3C->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE3C)))))));}
break;}case 398: _LL3CD: {
# 2814
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2814 "parse.y"
{struct _tuple22*_tmpE3D;yyval=Cyc_YY17(((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE3D->f2=0,_tmpE3D)))))));}
break;}case 399: _LL3CE: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2815 "parse.y"
{struct _tuple22*_tmpE3E;yyval=Cyc_YY17(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE3E->f2=1,_tmpE3E)))))));}
break;}case 400: _LL3CF: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2816 "parse.y"
{struct _tuple22*_tmpE3F;yyval=Cyc_YY17(((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->f1=0,((_tmpE3F->f2=1,_tmpE3F)))))));}
break;}case 401: _LL3D0: {
# 2819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2821 "parse.y"
{struct Cyc_List_List*_tmpE40;yyval=Cyc_YY16(((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE40->tl=0,_tmpE40)))))));}
break;}case 402: _LL3D1: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2823 "parse.y"
{struct Cyc_List_List*_tmpE41;yyval=Cyc_YY16(((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE41->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE41)))))));}
break;}case 403: _LL3D2: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 404: _LL3D3: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2831 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 405: _LL3D4: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2836 "parse.y"
yyval=yyyyvsp[0];
break;}case 406: _LL3D5: {
# 2839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2838 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 407: _LL3D6: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2840 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 408: _LL3D7: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2844 "parse.y"
yyval=Cyc_YY9(0);
break;}case 409: _LL3D8: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2845 "parse.y"
{struct Cyc_Core_Opt*_tmpE42;yyval=Cyc_YY9(((_tmpE42=_cycalloc_atomic(sizeof(*_tmpE42)),((_tmpE42->v=(void*)Cyc_Absyn_Times,_tmpE42)))));}
break;}case 410: _LL3D9: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2846 "parse.y"
{struct Cyc_Core_Opt*_tmpE43;yyval=Cyc_YY9(((_tmpE43=_cycalloc_atomic(sizeof(*_tmpE43)),((_tmpE43->v=(void*)Cyc_Absyn_Div,_tmpE43)))));}
break;}case 411: _LL3DA: {
# 2849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2847 "parse.y"
{struct Cyc_Core_Opt*_tmpE44;yyval=Cyc_YY9(((_tmpE44=_cycalloc_atomic(sizeof(*_tmpE44)),((_tmpE44->v=(void*)Cyc_Absyn_Mod,_tmpE44)))));}
break;}case 412: _LL3DB: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848 "parse.y"
{struct Cyc_Core_Opt*_tmpE45;yyval=Cyc_YY9(((_tmpE45=_cycalloc_atomic(sizeof(*_tmpE45)),((_tmpE45->v=(void*)Cyc_Absyn_Plus,_tmpE45)))));}
break;}case 413: _LL3DC: {
# 2851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849 "parse.y"
{struct Cyc_Core_Opt*_tmpE46;yyval=Cyc_YY9(((_tmpE46=_cycalloc_atomic(sizeof(*_tmpE46)),((_tmpE46->v=(void*)Cyc_Absyn_Minus,_tmpE46)))));}
break;}case 414: _LL3DD: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850 "parse.y"
{struct Cyc_Core_Opt*_tmpE47;yyval=Cyc_YY9(((_tmpE47=_cycalloc_atomic(sizeof(*_tmpE47)),((_tmpE47->v=(void*)Cyc_Absyn_Bitlshift,_tmpE47)))));}
break;}case 415: _LL3DE: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851 "parse.y"
{struct Cyc_Core_Opt*_tmpE48;yyval=Cyc_YY9(((_tmpE48=_cycalloc_atomic(sizeof(*_tmpE48)),((_tmpE48->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE48)))));}
break;}case 416: _LL3DF: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
{struct Cyc_Core_Opt*_tmpE49;yyval=Cyc_YY9(((_tmpE49=_cycalloc_atomic(sizeof(*_tmpE49)),((_tmpE49->v=(void*)Cyc_Absyn_Bitand,_tmpE49)))));}
break;}case 417: _LL3E0: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853 "parse.y"
{struct Cyc_Core_Opt*_tmpE4A;yyval=Cyc_YY9(((_tmpE4A=_cycalloc_atomic(sizeof(*_tmpE4A)),((_tmpE4A->v=(void*)Cyc_Absyn_Bitxor,_tmpE4A)))));}
break;}case 418: _LL3E1: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_Core_Opt*_tmpE4B;yyval=Cyc_YY9(((_tmpE4B=_cycalloc_atomic(sizeof(*_tmpE4B)),((_tmpE4B->v=(void*)Cyc_Absyn_Bitor,_tmpE4B)))));}
break;}case 419: _LL3E2: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2859 "parse.y"
yyval=yyyyvsp[0];
break;}case 420: _LL3E3: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2861 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3E4: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2864 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3E5: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2867 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3E6: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2869 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E7: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2871 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E8: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2873 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E9: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2877 "parse.y"
yyval=yyyyvsp[0];
break;}case 427: _LL3EA: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
yyval=yyyyvsp[0];
break;}case 428: _LL3EB: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2883 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3EC: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2887 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3ED: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2889 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3EE: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2893 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3EF: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2895 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3F0: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2899 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3F1: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2901 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3F2: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2905 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3F3: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2907 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3F4: {
# 2910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2911 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3F5: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2913 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F6: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2915 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3F7: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2919 "parse.y"
yyval=yyyyvsp[0];
break;}case 441: _LL3F8: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2921 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F9: {
# 2924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2923 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL3FA: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2925 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3FB: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2927 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3FC: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2931 "parse.y"
yyval=yyyyvsp[0];
break;}case 446: _LL3FD: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2933 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3FE: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2935 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3FF: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2939 "parse.y"
yyval=yyyyvsp[0];
break;}case 449: _LL400: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2941 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL401: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2943 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL402: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2947 "parse.y"
yyval=yyyyvsp[0];
break;}case 452: _LL403: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2949 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL404: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2951 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL405: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2953 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL406: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2957 "parse.y"
yyval=yyyyvsp[0];
break;}case 456: _LL407: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2959 "parse.y"
void*_tmp7B7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7B7,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2962
break;}case 457: _LL408: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2965 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL409: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2966 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL40A: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2967 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL40B: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2968 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL40C: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2969 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL40D: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL40E: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL40F: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2973 "parse.y"
void*_tmp7B8=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7B8,(unsigned int)(yyyylsp[0]).first_line));
# 2976
break;}case 465: _LL410: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2976 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL411: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2978 "parse.y"
void*_tmp7B9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7B9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2981
break;}case 467: _LL412: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2983 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE51;struct Cyc_Absyn_MallocInfo _tmpE50;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE4F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE51.tag=33,((_tmpE51.f1=((_tmpE50.is_calloc=0,((_tmpE50.rgn=0,((_tmpE50.elt_type=0,((_tmpE50.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE50.fat_result=0,_tmpE50)))))))))),_tmpE51)))),_tmpE4F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2985
break;}case 468: _LL413: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2986 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE57;struct Cyc_Absyn_MallocInfo _tmpE56;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE55;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=((_tmpE57.tag=33,((_tmpE57.f1=((_tmpE56.is_calloc=0,((_tmpE56.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE56.elt_type=0,((_tmpE56.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE56.fat_result=0,_tmpE56)))))))))),_tmpE57)))),_tmpE55)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2988
break;}case 469: _LL414: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2989 "parse.y"
void*_tmp7C0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE61;void**_tmpE60;struct Cyc_Absyn_MallocInfo _tmpE5F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE5E;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE5E=_cycalloc(sizeof(*_tmpE5E)),((_tmpE5E[0]=((_tmpE61.tag=33,((_tmpE61.f1=((_tmpE5F.is_calloc=1,((_tmpE5F.rgn=0,((_tmpE5F.elt_type=((_tmpE60=_cycalloc(sizeof(*_tmpE60)),((_tmpE60[0]=_tmp7C0,_tmpE60)))),((_tmpE5F.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE5F.fat_result=0,_tmpE5F)))))))))),_tmpE61)))),_tmpE5E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2992
break;}case 470: _LL415: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2994 "parse.y"
void*_tmp7C5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE6B;void**_tmpE6A;struct Cyc_Absyn_MallocInfo _tmpE69;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE68;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE6B.tag=33,((_tmpE6B.f1=((_tmpE69.is_calloc=1,((_tmpE69.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE69.elt_type=((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=_tmp7C5,_tmpE6A)))),((_tmpE69.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE69.fat_result=0,_tmpE69)))))))))),_tmpE6B)))),_tmpE68)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2997
break;}case 471: _LL416: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2998 "parse.y"
{struct Cyc_Absyn_Exp*_tmpE6C[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE6C[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE6C,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 472: _LL417: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3000 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE72;struct _dyneither_ptr*_tmpE71;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE70;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=((_tmpE72.tag=37,((_tmpE72.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE72.f2=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE71)))),_tmpE72)))))),_tmpE70)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 473: _LL418: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3002 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE78;struct _dyneither_ptr*_tmpE77;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE76;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76[0]=((_tmpE78.tag=37,((_tmpE78.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE78.f2=((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE77)))),_tmpE78)))))),_tmpE76)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL419: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3004 "parse.y"
void*_tmp7D1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7D1,(unsigned int)(yyyylsp[0]).first_line));
break;}case 475: _LL41A: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3007 "parse.y"
struct _tuple20 _stmttmp1A=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp7D3;struct _dyneither_ptr _tmp7D4;struct _tuple20 _tmp7D2=_stmttmp1A;_tmp7D3=_tmp7D2.f1;_tmp7D4=_tmp7D2.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7D3,_tmp7D4,(unsigned int)(yyyylsp[0]).first_line));
break;}case 476: _LL41B: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3012 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 477: _LL41C: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3013 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 478: _LL41D: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3014 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 479: _LL41E: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3019 "parse.y"
yyval=yyyyvsp[0];
break;}case 480: _LL41F: {
# 3022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3021 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL420: {
# 3024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3023 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 482: _LL421: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3025 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL422: {
# 3028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3027 "parse.y"
{struct _dyneither_ptr*_tmpE79;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE79)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 484: _LL423: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3029 "parse.y"
{struct _dyneither_ptr*_tmpE7A;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 485: _LL424: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3031 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 486: _LL425: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3033 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 487: _LL426: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3035 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE7D;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE7C;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C[0]=((_tmpE7D.tag=24,((_tmpE7D.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE7D.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE7D)))))),_tmpE7C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 488: _LL427: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3037 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE80;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE7F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F[0]=((_tmpE80.tag=24,((_tmpE80.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE80.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE80)))))),_tmpE7F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 489: _LL428: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3042 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE8A;struct _dyneither_ptr*_tmpE89;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE88;struct Cyc_List_List*_tmpE87;yyval=Cyc_YY3(((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87->hd=(void*)((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A[0]=((_tmpE88.tag=0,((_tmpE88.f1=((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE89)))),_tmpE88)))),_tmpE8A)))),((_tmpE87->tl=0,_tmpE87)))))));}
break;}case 490: _LL429: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3045 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE90;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE8F;struct Cyc_List_List*_tmpE8E;yyval=Cyc_YY3(((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E->hd=(void*)((_tmpE90=_cycalloc_atomic(sizeof(*_tmpE90)),((_tmpE90[0]=((_tmpE8F.tag=1,((_tmpE8F.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE8F)))),_tmpE90)))),((_tmpE8E->tl=0,_tmpE8E)))))));}
break;}case 491: _LL42A: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3047 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE9A;struct _dyneither_ptr*_tmpE99;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE98;struct Cyc_List_List*_tmpE97;yyval=Cyc_YY3(((_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->hd=(void*)((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=((_tmpE98.tag=0,((_tmpE98.f1=((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE99)))),_tmpE98)))),_tmpE9A)))),((_tmpE97->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE97)))))));}
break;}case 492: _LL42B: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3050 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEA0;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE9F;struct Cyc_List_List*_tmpE9E;yyval=Cyc_YY3(((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->hd=(void*)((_tmpEA0=_cycalloc_atomic(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpE9F.tag=1,((_tmpE9F.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpE9F)))),_tmpEA0)))),((_tmpE9E->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE9E)))))));}
break;}case 493: _LL42C: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL42D: {
# 3059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3058 "parse.y"
yyval=yyyyvsp[0];
break;}case 495: _LL42E: {
# 3061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3060 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL42F: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3062 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL430: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3064 "parse.y"
yyval=yyyyvsp[1];
break;}case 498: _LL431: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3069 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL432: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3071 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 500: _LL433: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3074 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL434: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3077 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEA3;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEA2;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=((_tmpEA3.tag=28,((_tmpEA3.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEA3.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpEA3.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpEA3.f4=0,_tmpEA3)))))))))),_tmpEA2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 502: _LL435: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3080 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL436: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3084 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 504: _LL437: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpEA4;yyval=Cyc_YY6(((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEA4->tl=0,_tmpEA4)))))));}
break;}case 505: _LL438: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3092 "parse.y"
{struct Cyc_List_List*_tmpEA5;yyval=Cyc_YY6(((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA5->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEA5)))))));}
break;}case 506: _LL439: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp7ED=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7ED.f1,_tmp7ED.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL43A: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3099 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 508: _LL43B: {
# 3102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3100 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL43C: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3102 "parse.y"
struct _dyneither_ptr _tmp7EE=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7EE);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7EE,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3110
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7EE,i,(unsigned int)(yyyylsp[0]).first_line));
# 3112
break;}case 510: _LL43D: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3113 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL43E: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117 "parse.y"
{struct _dyneither_ptr*_tmpEA8;struct _tuple0*_tmpEA7;yyval=Cyc_QualId_tok(((_tmpEA7=_cycalloc(sizeof(*_tmpEA7)),((_tmpEA7->f1=Cyc_Absyn_Rel_n(0),((_tmpEA7->f2=((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA8)))),_tmpEA7)))))));}
break;}case 512: _LL43F: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3118 "parse.y"
yyval=yyyyvsp[0];
break;}case 513: _LL440: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121 "parse.y"
{struct _dyneither_ptr*_tmpEAB;struct _tuple0*_tmpEAA;yyval=Cyc_QualId_tok(((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA->f1=Cyc_Absyn_Rel_n(0),((_tmpEAA->f2=((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEAB)))),_tmpEAA)))))));}
break;}case 514: _LL441: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3122 "parse.y"
yyval=yyyyvsp[0];
break;}case 515: _LL442: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3127 "parse.y"
yyval=yyyyvsp[0];
break;}case 516: _LL443: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3128 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL444: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL445: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3132 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL446: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3137 "parse.y"
break;}case 520: _LL447: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3137 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL448:
# 3142
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(521,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 147)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6756) && Cyc_yycheck[_check_known_subscript_notnull(6757,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6757,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(144,yyn - 147)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1036,yystate)];
# 431
if(yyn > - 32768  && yyn < 6756){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6757,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)])+ 15,count ++);}
{unsigned int _tmpEB0;unsigned int _tmpEAF;struct _dyneither_ptr _tmpEAE;char*_tmpEAD;unsigned int _tmpEAC;msg=((_tmpEAC=(unsigned int)(sze + 15),((_tmpEAD=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEAC + 1)),((_tmpEAE=_tag_dyneither(_tmpEAD,sizeof(char),_tmpEAC + 1),((((_tmpEAF=_tmpEAC,((_tmpEB1(& _tmpEB0,& _tmpEAF,& _tmpEAD),_tmpEAD[_tmpEAF]=(char)0)))),_tmpEAE))))))));}
{const char*_tmpEB2;Cyc_strcpy(msg,((_tmpEB2="parse error",_tag_dyneither(_tmpEB2,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6757,x + yyn)]== x){
# 453
{const char*_tmpEB4;const char*_tmpEB3;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEB4=", expecting `",_tag_dyneither(_tmpEB4,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEB3=" or `",_tag_dyneither(_tmpEB3,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)]);
{const char*_tmpEB5;Cyc_strcat(msg,((_tmpEB5="'",_tag_dyneither(_tmpEB5,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEB6;Cyc_yyerror(((_tmpEB6="parse error",_tag_dyneither(_tmpEB6,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7FC=1;_npop_handler(0);return _tmp7FC;}
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
 if(yyssp_offset == 0){int _tmp7FD=1;_npop_handler(0);return _tmp7FD;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1036,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6756) || Cyc_yycheck[_check_known_subscript_notnull(6757,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6757,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1035){
int _tmp7FE=0;_npop_handler(0);return _tmp7FE;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3140 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp808=v;int _tmp809;char _tmp80A;struct _dyneither_ptr _tmp80B;union Cyc_Absyn_Nmspace _tmp80C;struct _dyneither_ptr*_tmp80D;_LL44B: if((_tmp808.Int_tok).tag != 1)goto _LL44D;_tmp809=((struct _tuple5)(_tmp808.Int_tok).val).f2;_LL44C:
{const char*_tmpEBA;void*_tmpEB9[1];struct Cyc_Int_pa_PrintArg_struct _tmpEB8;(_tmpEB8.tag=1,((_tmpEB8.f1=(unsigned long)_tmp809,((_tmpEB9[0]=& _tmpEB8,Cyc_fprintf(Cyc_stderr,((_tmpEBA="%d",_tag_dyneither(_tmpEBA,sizeof(char),3))),_tag_dyneither(_tmpEB9,sizeof(void*),1)))))));}goto _LL44A;_LL44D: if((_tmp808.Char_tok).tag != 2)goto _LL44F;_tmp80A=(char)(_tmp808.Char_tok).val;_LL44E:
{const char*_tmpEBE;void*_tmpEBD[1];struct Cyc_Int_pa_PrintArg_struct _tmpEBC;(_tmpEBC.tag=1,((_tmpEBC.f1=(unsigned long)((int)_tmp80A),((_tmpEBD[0]=& _tmpEBC,Cyc_fprintf(Cyc_stderr,((_tmpEBE="%c",_tag_dyneither(_tmpEBE,sizeof(char),3))),_tag_dyneither(_tmpEBD,sizeof(void*),1)))))));}goto _LL44A;_LL44F: if((_tmp808.String_tok).tag != 3)goto _LL451;_tmp80B=(struct _dyneither_ptr)(_tmp808.String_tok).val;_LL450:
{const char*_tmpEC2;void*_tmpEC1[1];struct Cyc_String_pa_PrintArg_struct _tmpEC0;(_tmpEC0.tag=0,((_tmpEC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp80B),((_tmpEC1[0]=& _tmpEC0,Cyc_fprintf(Cyc_stderr,((_tmpEC2="\"%s\"",_tag_dyneither(_tmpEC2,sizeof(char),5))),_tag_dyneither(_tmpEC1,sizeof(void*),1)))))));}goto _LL44A;_LL451: if((_tmp808.QualId_tok).tag != 5)goto _LL453;_tmp80C=((struct _tuple0*)(_tmp808.QualId_tok).val)->f1;_tmp80D=((struct _tuple0*)(_tmp808.QualId_tok).val)->f2;_LL452: {
# 3146
struct Cyc_List_List*_tmp817=0;
{union Cyc_Absyn_Nmspace _tmp818=_tmp80C;struct Cyc_List_List*_tmp819;struct Cyc_List_List*_tmp81A;struct Cyc_List_List*_tmp81B;_LL456: if((_tmp818.Rel_n).tag != 1)goto _LL458;_tmp819=(struct Cyc_List_List*)(_tmp818.Rel_n).val;_LL457:
 _tmp817=_tmp819;goto _LL455;_LL458: if((_tmp818.Abs_n).tag != 2)goto _LL45A;_tmp81A=(struct Cyc_List_List*)(_tmp818.Abs_n).val;_LL459:
 _tmp817=_tmp81A;goto _LL455;_LL45A: if((_tmp818.C_n).tag != 3)goto _LL45C;_tmp81B=(struct Cyc_List_List*)(_tmp818.C_n).val;_LL45B:
 _tmp817=_tmp81B;goto _LL455;_LL45C: if((_tmp818.Loc_n).tag != 4)goto _LL455;_LL45D:
 goto _LL455;_LL455:;}
# 3153
for(0;_tmp817 != 0;_tmp817=_tmp817->tl){
const char*_tmpEC6;void*_tmpEC5[1];struct Cyc_String_pa_PrintArg_struct _tmpEC4;(_tmpEC4.tag=0,((_tmpEC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp817->hd)),((_tmpEC5[0]=& _tmpEC4,Cyc_fprintf(Cyc_stderr,((_tmpEC6="%s::",_tag_dyneither(_tmpEC6,sizeof(char),5))),_tag_dyneither(_tmpEC5,sizeof(void*),1)))))));}
{const char*_tmpECA;void*_tmpEC9[1];struct Cyc_String_pa_PrintArg_struct _tmpEC8;(_tmpEC8.tag=0,((_tmpEC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp80D),((_tmpEC9[0]=& _tmpEC8,Cyc_fprintf(Cyc_stderr,((_tmpECA="%s::",_tag_dyneither(_tmpECA,sizeof(char),5))),_tag_dyneither(_tmpEC9,sizeof(void*),1)))))));}
goto _LL44A;}_LL453:;_LL454:
{const char*_tmpECD;void*_tmpECC;(_tmpECC=0,Cyc_fprintf(Cyc_stderr,((_tmpECD="?",_tag_dyneither(_tmpECD,sizeof(char),2))),_tag_dyneither(_tmpECC,sizeof(void*),0)));}goto _LL44A;_LL44A:;}
# 3162
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp824=_new_region("yyr");struct _RegionHandle*yyr=& _tmp824;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp825=Cyc_Parse_parse_result;_npop_handler(0);return _tmp825;};
# 3165
;_pop_region(yyr);};}
