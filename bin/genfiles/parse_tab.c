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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 885
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 912
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 935
void*Cyc_Absyn_compress_kb(void*);
# 940
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 942
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 945
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 947
extern void*Cyc_Absyn_sint_typ;
# 949
void*Cyc_Absyn_float_typ(int);
# 972
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 997
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1046
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
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1095
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1106
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1113
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1122
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1127
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1133
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1156
int Cyc_Absyn_fntype_att(void*a);
# 1174
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
# 296 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 344
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
{struct _tuple10*_tmp814;declarators=((_tmp814=_region_malloc(r,sizeof(*_tmp814)),((_tmp814->tl=declarators,((_tmp814->hd=_tmp1,_tmp814))))));}{
struct Cyc_List_List*_tmp815;exprs=((_tmp815=_region_malloc(r,sizeof(*_tmp815)),((_tmp815->hd=_tmp2,((_tmp815->tl=exprs,_tmp815))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp818;struct Cyc_Position_Exit_exn_struct*_tmp817;(int)_throw((void*)((_tmp817=_cycalloc_atomic(sizeof(*_tmp817)),((_tmp817[0]=((_tmp818.tag=Cyc_Position_Exit,_tmp818)),_tmp817)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp81D;void*_tmp81C[2];struct Cyc_String_pa_PrintArg_struct _tmp81B;struct Cyc_String_pa_PrintArg_struct _tmp81A;(_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp81B.tag=0,((_tmp81B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp81C[0]=& _tmp81B,((_tmp81C[1]=& _tmp81A,Cyc_fprintf(Cyc_stderr,((_tmp81D="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp81D,sizeof(char),46))),_tag_dyneither(_tmp81C,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_warn(loc,((_tmp820="qualifier on type is ignored",_tag_dyneither(_tmp820,sizeof(char),29))),_tag_dyneither(_tmp81F,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp823;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp822;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=((_tmp823.tag=1,((_tmp823.f1=_tmp18,_tmp823)))),_tmp822)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp824;return(_tmp824.f1=nullable,((_tmp824.f2=bound,((_tmp824.f3=zeroterm,((_tmp824.f4=rgn,_tmp824)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp831;const char*_tmp830;void*_tmp82F[1];struct Cyc_Int_pa_PrintArg_struct _tmp82E;struct _tuple0*_tmp82D;return(_tmp82D=_cycalloc(sizeof(*_tmp82D)),((_tmp82D->f1=Cyc_Absyn_Rel_n(0),((_tmp82D->f2=(
(_tmp831=_cycalloc(sizeof(*_tmp831)),((_tmp831[0]=(struct _dyneither_ptr)((_tmp82E.tag=1,((_tmp82E.f1=(unsigned long)enum_counter ++,((_tmp82F[0]=& _tmp82E,Cyc_aprintf(((_tmp830="__anonymous_enum_%d__",_tag_dyneither(_tmp830,sizeof(char),22))),_tag_dyneither(_tmp82F,sizeof(void*),1)))))))),_tmp831)))),_tmp82D)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct _tuple15*f1;struct Cyc_Absyn_Exp*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple16*field_info){
# 246
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct _tuple16*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=_tmp23->f2;
if(_tmp27 != 0){
const char*_tmp832;Cyc_Parse_err(((_tmp832="bad type params in struct field",_tag_dyneither(_tmp832,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp833;Cyc_Parse_err(((_tmp833="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp833,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp834;return(_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->name=(*_tmp24).f2,((_tmp834->tq=_tmp25,((_tmp834->type=_tmp26,((_tmp834->width=_tmp29,((_tmp834->attributes=_tmp28,_tmp834)))))))))));};}
# 255
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp835;return(_tmp835.Signed_spec=0,((_tmp835.Unsigned_spec=0,((_tmp835.Short_spec=0,((_tmp835.Long_spec=0,((_tmp835.Long_Long_spec=0,((_tmp835.Valid_type_spec=0,((_tmp835.Type_spec=Cyc_Absyn_sint_typ,((_tmp835.loc=loc,_tmp835)))))))))))))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp838;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp837;return Cyc_Absyn_starb_typ(_tmp35,
argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp36,
# 301
_tmp37 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837[0]=((_tmp838.tag=1,((_tmp838.f1=_tmp37,_tmp838)))),_tmp837)))),_tmp38);}_LL14:;_LL15:
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
struct Cyc_String_pa_PrintArg_struct _tmp840;void*_tmp83F[1];const char*_tmp83E;struct _dyneither_ptr*_tmp83D;struct _dyneither_ptr*nm=(_tmp83D=_cycalloc(sizeof(*_tmp83D)),((_tmp83D[0]=(struct _dyneither_ptr)((_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E),((_tmp83F[0]=& _tmp840,Cyc_aprintf(((_tmp83E="`%s",_tag_dyneither(_tmp83E,sizeof(char),4))),_tag_dyneither(_tmp83F,sizeof(void*),1)))))))),_tmp83D)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp84F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp84D;struct Cyc_Absyn_Tvar*_tmp84C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp84B;*_tmp47=(void*)((_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp84F.tag=2,((_tmp84F.f1=((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C->name=nm,((_tmp84C->identity=- 1,((_tmp84C->kind=(void*)((_tmp84E=_cycalloc(sizeof(*_tmp84E)),((_tmp84E[0]=((_tmp84D.tag=0,((_tmp84D.f1=& Cyc_Tcutil_ik,_tmp84D)))),_tmp84E)))),_tmp84C)))))))),_tmp84F)))),_tmp84B))));}
goto _LL1D;}_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 331
{struct _tuple17*_tmp852;struct Cyc_List_List*_tmp851;_tmp3C=((_tmp851=_cycalloc(sizeof(*_tmp851)),((_tmp851->hd=((_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852->f1=_tmp3E,((_tmp852->f2=_tmp40,_tmp852)))))),((_tmp851->tl=_tmp3C,_tmp851))))));}goto _LL16;_LL19: if(_tmp3D->f1 == 0)goto _LL1B;_tmp41=*_tmp3D->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3D->f3;if(_tmp42->tag != 15)goto _LL1B;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp42->f1);if(_tmp43->tag != 1)goto _LL1B;else{_tmp44=(void**)((void**)& _tmp43->f2);}}};_LL1A: {
# 335
struct Cyc_String_pa_PrintArg_struct _tmp85A;void*_tmp859[1];const char*_tmp858;struct _dyneither_ptr*_tmp857;struct _dyneither_ptr*nm=(_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857[0]=(struct _dyneither_ptr)((_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41),((_tmp859[0]=& _tmp85A,Cyc_aprintf(((_tmp858="`%s",_tag_dyneither(_tmp858,sizeof(char),4))),_tag_dyneither(_tmp859,sizeof(void*),1)))))))),_tmp857)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp869;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp868;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp867;struct Cyc_Absyn_Tvar*_tmp866;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp865;*_tmp44=(void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp869.tag=2,((_tmp869.f1=((_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866->name=nm,((_tmp866->identity=- 1,((_tmp866->kind=(void*)((_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((_tmp867.tag=0,((_tmp867.f1=& Cyc_Tcutil_rk,_tmp867)))),_tmp868)))),_tmp866)))))))),_tmp869)))),_tmp865))));}
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
{struct _tuple17*_tmp86C;struct Cyc_List_List*_tmp86B;_tmp5C=((_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B->hd=((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp86C->f2=_tmp5F,_tmp86C)))))),((_tmp86B->tl=_tmp5C,_tmp86B))))));}goto _LL22;_LL25:;_LL26:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp86F;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp86E;return Cyc_Absyn_new_exp((void*)((_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp86F.tag=38,((_tmp86F.f1=Cyc_Tcutil_copy_type(_tmp68),_tmp86F)))),_tmp86E)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp875;struct Cyc_Absyn_ArrayInfo _tmp874;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp873;return(void*)((_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873[0]=((_tmp875.tag=8,((_tmp875.f1=((_tmp874.elt_type=_tmp7A,((_tmp874.tq=_tmp6E,((_tmp874.num_elts=nelts2,((_tmp874.zero_term=_tmp70,((_tmp874.zt_loc=_tmp71,_tmp874)))))))))),_tmp875)))),_tmp873))));}
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B;if(_tmp72->tag != 5)goto _LL31;else{_tmp73=(_tmp72->f1).elt_typ;_tmp74=(_tmp72->f1).elt_tq;_tmp75=((_tmp72->f1).ptr_atts).rgn;_tmp76=((_tmp72->f1).ptr_atts).nullable;_tmp77=((_tmp72->f1).ptr_atts).bounds;_tmp78=((_tmp72->f1).ptr_atts).zero_term;_tmp79=((_tmp72->f1).ptr_atts).ptrloc;}}_LL30: {
# 386
void*_tmp7E=Cyc_Parse_substitute_tags(tags,_tmp73);
union Cyc_Absyn_Constraint*_tmp7F=_tmp77;
{union Cyc_Absyn_Constraint _tmp80=*_tmp77;struct Cyc_Absyn_Exp*_tmp82;_LL34: if((_tmp80.Eq_constr).tag != 1)goto _LL36;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp81=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp80.Eq_constr).val);if(_tmp81->tag != 1)goto _LL36;else{_tmp82=_tmp81->f1;}};_LL35:
# 390
 _tmp82=Cyc_Parse_substitute_tags_exp(tags,_tmp82);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp87B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp87A;union Cyc_Absyn_Constraint*_tmp879;_tmp7F=((_tmp879=_cycalloc(sizeof(*_tmp879)),(((_tmp879->Eq_constr).val=(void*)((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp87A.tag=1,((_tmp87A.f1=_tmp82,_tmp87A)))),_tmp87B)))),(((_tmp879->Eq_constr).tag=1,_tmp879))))));}
goto _LL33;_LL36:;_LL37:
 goto _LL33;_LL33:;}
# 395
if(_tmp7E != _tmp73  || _tmp7F != _tmp77){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp881;struct Cyc_Absyn_PtrInfo _tmp880;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87F;return(void*)((_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=((_tmp881.tag=5,((_tmp881.f1=((_tmp880.elt_typ=_tmp7E,((_tmp880.elt_tq=_tmp74,((_tmp880.ptr_atts=(((_tmp880.ptr_atts).rgn=_tmp75,(((_tmp880.ptr_atts).nullable=_tmp76,(((_tmp880.ptr_atts).bounds=_tmp7F,(((_tmp880.ptr_atts).zero_term=_tmp78,(((_tmp880.ptr_atts).ptrloc=_tmp79,_tmp880.ptr_atts)))))))))),_tmp880)))))),_tmp881)))),_tmp87F))));}
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
struct _tuple18*_tmp882;return(_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882->f1=(*t).f2,((_tmp882->f2=(*t).f3,_tmp882)))));}
# 419
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp8A=d->r;struct Cyc_Absyn_Vardecl*_tmp8C;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL3B;else{_tmp8C=_tmp8B->f1;}}_LL3A:
 return _tmp8C;_LL3B:;_LL3C: {
const char*_tmp885;void*_tmp884;(_tmp884=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp885="bad declaration in `forarray' statement",_tag_dyneither(_tmp885,sizeof(char),40))),_tag_dyneither(_tmp884,sizeof(void*),0)));}_LL38:;}
# 426
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL40;}_LL3F:
 return 1;_LL40:;_LL41:
 return 0;_LL3D:;}
# 435
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp886;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp886="`H",_tag_dyneither(_tmp886,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp887;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp887="`U",_tag_dyneither(_tmp887,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp888;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp888="`RC",_tag_dyneither(_tmp888,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 443
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp892;struct _dyneither_ptr*_tmp891;struct Cyc_Absyn_Tvar*_tmp890;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp88F;return(void*)((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp892.tag=2,((_tmp892.f1=((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890->name=((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=s,_tmp891)))),((_tmp890->identity=- 1,((_tmp890->kind=k,_tmp890)))))))),_tmp892)))),_tmp88F))));}}}}
# 446
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmp9B;void*_tmp9C;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp99=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98;if(_tmp99->tag != 1)goto _LL45;}_LL44:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp895;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp894;k=(void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((_tmp895.tag=1,((_tmp895.f1=0,_tmp895)))),_tmp894))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98;if(_tmp9A->tag != 2)goto _LL47;else{_tmp9B=_tmp9A->f2;}}_LL46:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp898;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp897;k=(void*)((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897[0]=((_tmp898.tag=2,((_tmp898.f1=0,((_tmp898.f2=_tmp9B,_tmp898)))))),_tmp897))));}goto _LL42;_LL47: _tmp9C=_tmp98;_LL48:
 k=_tmp9C;goto _LL42;_LL42:;}{
# 453
struct Cyc_Absyn_Tvar*_tmp899;return(_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899->name=t->name,((_tmp899->identity=- 1,((_tmp899->kind=k,_tmp899)))))));};}
# 460
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA2=t;struct Cyc_Absyn_Tvar*_tmpA4;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA2;if(_tmpA3->tag != 2)goto _LL4C;else{_tmpA4=_tmpA3->f1;}}_LL4B:
 return _tmpA4;_LL4C:;_LL4D: {
const char*_tmp89C;void*_tmp89B;(_tmp89B=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp89C="expecting a list of type variables, not types",_tag_dyneither(_tmp89C,sizeof(char),46))),_tag_dyneither(_tmp89B,sizeof(void*),0)));}_LL49:;}
# 466
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp89F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89E;return(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp89F.tag=2,((_tmp89F.f1=pr,_tmp89F)))),_tmp89E))));}
# 471
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpA9=Cyc_Tcutil_compress(t);void**_tmpAB;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA9;if(_tmpAA->tag != 2)goto _LL51;else{_tmpAB=(void**)&(_tmpAA->f1)->kind;}}_LL50: {
# 474
void*_tmpAC=Cyc_Absyn_compress_kb(*_tmpAB);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAD=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAC;if(_tmpAD->tag != 1)goto _LL56;}_LL55:
# 476
 if(!leq)*_tmpAB=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8A2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A1;*_tmpAB=(void*)((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A2.tag=2,((_tmp8A2.f1=0,((_tmp8A2.f2=k,_tmp8A2)))))),_tmp8A1))));}
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
{const char*_tmp8A5;void*_tmp8A4;(_tmp8A4=0,Cyc_Tcutil_warn(loc,((_tmp8A5="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8A5,sizeof(char),93))),_tag_dyneither(_tmp8A4,sizeof(void*),0)));}
# 509
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB5=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB3;if(_tmpB5->tag != 0)goto _LL5D;else{_tmpB6=_tmpB5->f1;}}_LL61:
# 511
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8A8="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8A8,sizeof(char),73))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}{
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
const char*_tmp8AB;void*_tmp8AA;(_tmp8AA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8AB="initializer found in parameter declaration",_tag_dyneither(_tmp8AB,sizeof(char),43))),_tag_dyneither(_tmp8AA,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpBF->name)){
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8AE="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8AE,sizeof(char),47))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}
{struct _tuple8*_tmp8B1;struct Cyc_List_List*_tmp8B0;rev_new_params=(
(_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0->hd=((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1->f1=(*_tmpBF->name).f2,((_tmp8B1->f2=_tmpBF->tq,((_tmp8B1->f3=_tmpBF->type,_tmp8B1)))))))),((_tmp8B0->tl=rev_new_params,_tmp8B0))))));}
# 531
goto L;_LL65:;_LL66: {
const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBC->loc,((_tmp8B4="nonvariable declaration in parameter type",_tag_dyneither(_tmp8B4,sizeof(char),42))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}_LL62:;}
# 535
L: if(_tmpBB == 0){
const char*_tmp8B8;void*_tmp8B7[1];struct Cyc_String_pa_PrintArg_struct _tmp8B6;(_tmp8B6.tag=0,((_tmp8B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpB6->hd)),((_tmp8B7[0]=& _tmp8B6,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B8="%s is not given a type",_tag_dyneither(_tmp8B8,sizeof(char),23))),_tag_dyneither(_tmp8B7,sizeof(void*),1)))))));}}{
# 538
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8C7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8C5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8C4;struct Cyc_List_List*_tmp8C3;return
(_tmp8C3=_region_malloc(yy,sizeof(*_tmp8C3)),((_tmp8C3->hd=(void*)((_tmp8C7=_region_malloc(yy,sizeof(*_tmp8C7)),((_tmp8C7[0]=((_tmp8C4.tag=3,((_tmp8C4.f1=(void*)((_tmp8C6=_region_malloc(yy,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C5.tag=1,((_tmp8C5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8C5.f2=0,((_tmp8C5.f3=0,((_tmp8C5.f4=0,((_tmp8C5.f5=0,_tmp8C5)))))))))))),_tmp8C6)))),_tmp8C4)))),_tmp8C7)))),((_tmp8C3->tl=0,_tmp8C3)))));};};_LL5D:;}
# 545
goto _LL5C;_LL5B:;_LL5C: {
struct Cyc_List_List*_tmp8C8;return(_tmp8C8=_region_malloc(yy,sizeof(*_tmp8C8)),((_tmp8C8->hd=(void*)tms->hd,((_tmp8C8->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8C8)))));}_LL58:;};}
# 553
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 557
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8C9;d=((_tmp8C9.id=d.id,((_tmp8C9.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8C9))));}{
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
{const char*_tmp8CA;Cyc_Parse_err(((_tmp8CA="bad storage class on function",_tag_dyneither(_tmp8CA,sizeof(char),30))),loc);}break;}}{
# 579
void*_tmpD3=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct Cyc_Absyn_Tqual _tmpD5;void*_tmpD6;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple13 _tmpD4=Cyc_Parse_apply_tms(tq,_tmpD3,atts,d.tms);_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;_tmpD7=_tmpD4.f3;_tmpD8=_tmpD4.f4;
# 584
if(_tmpD7 != 0){
# 587
const char*_tmp8CD;void*_tmp8CC;(_tmp8CC=0,Cyc_Tcutil_warn(loc,((_tmp8CD="bad type params, ignoring",_tag_dyneither(_tmp8CD,sizeof(char),26))),_tag_dyneither(_tmp8CC,sizeof(void*),0)));}{
# 589
void*_tmpDB=_tmpD6;struct Cyc_List_List*_tmpDD;void*_tmpDE;struct Cyc_Absyn_Tqual _tmpDF;void*_tmpE0;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*_tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 9)goto _LL6E;else{_tmpDD=(_tmpDC->f1).tvars;_tmpDE=(_tmpDC->f1).effect;_tmpDF=(_tmpDC->f1).ret_tqual;_tmpE0=(_tmpDC->f1).ret_typ;_tmpE1=(_tmpDC->f1).args;_tmpE2=(_tmpDC->f1).c_varargs;_tmpE3=(_tmpDC->f1).cyc_varargs;_tmpE4=(_tmpDC->f1).rgn_po;_tmpE5=(_tmpDC->f1).attributes;}}_LL6D: {
# 592
struct Cyc_List_List*_tmpE6=0;
{struct Cyc_List_List*_tmpE7=_tmpE1;for(0;_tmpE7 != 0;_tmpE7=_tmpE7->tl){
struct _dyneither_ptr*_tmpE9;struct Cyc_Absyn_Tqual _tmpEA;void*_tmpEB;struct _tuple8*_tmpE8=(struct _tuple8*)_tmpE7->hd;_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;_tmpEB=_tmpE8->f3;
if(_tmpE9 == 0){
{const char*_tmp8CE;Cyc_Parse_err(((_tmp8CE="missing argument variable in function prototype",_tag_dyneither(_tmp8CE,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8D1;struct Cyc_List_List*_tmp8D0;_tmpE6=((_tmp8D0=_cycalloc(sizeof(*_tmp8D0)),((_tmp8D0->hd=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8D1->f2=_tmpEA,((_tmp8D1->f3=_tmpEB,_tmp8D1)))))))),((_tmp8D0->tl=_tmpE6,_tmp8D0))))));};}else{
# 599
struct _tuple8*_tmp8D4;struct Cyc_List_List*_tmp8D3;_tmpE6=((_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3->hd=((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4->f1=_tmpE9,((_tmp8D4->f2=_tmpEA,((_tmp8D4->f3=_tmpEB,_tmp8D4)))))))),((_tmp8D3->tl=_tmpE6,_tmp8D3))))));}}}{
# 603
struct Cyc_Absyn_Fndecl*_tmp8D5;return(_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5->sc=sc,((_tmp8D5->name=d.id,((_tmp8D5->tvs=_tmpDD,((_tmp8D5->is_inline=is_inline,((_tmp8D5->effect=_tmpDE,((_tmp8D5->ret_tqual=_tmpDF,((_tmp8D5->ret_type=_tmpE0,((_tmp8D5->args=
# 606
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE6),((_tmp8D5->c_varargs=_tmpE2,((_tmp8D5->cyc_varargs=_tmpE3,((_tmp8D5->rgn_po=_tmpE4,((_tmp8D5->body=body,((_tmp8D5->cached_typ=0,((_tmp8D5->param_vardecls=0,((_tmp8D5->fn_vardecl=0,((_tmp8D5->attributes=
# 613
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE5,_tmpD8),_tmp8D5)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
const char*_tmp8D8;void*_tmp8D7;(_tmp8D7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8D8="declarator is not a function prototype",_tag_dyneither(_tmp8D8,sizeof(char),39))),_tag_dyneither(_tmp8D7,sizeof(void*),0)));}_LL6B:;};};};}static char _tmpF5[52]="at most one type may appear within a type specifier";
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
void*_tmp8D9;(_tmp8D9=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8D9,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8DA;(_tmp8DA=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8DA,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8DB;(_tmp8DB=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8DB,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8DC;(_tmp8DC=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8DC,sizeof(void*),0)));}
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
const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,Cyc_Tcutil_warn(loc,((_tmp8DF="missing type within specifier",_tag_dyneither(_tmp8DF,sizeof(char),30))),_tag_dyneither(_tmp8DE,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 689
if(seen_sign){
void*_tmpFF=t;enum Cyc_Absyn_Sign _tmp101;enum Cyc_Absyn_Size_of _tmp102;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFF;if(_tmp100->tag != 6)goto _LL73;else{_tmp101=_tmp100->f1;_tmp102=_tmp100->f2;}}_LL72:
# 692
 if(_tmp101 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp102);
goto _LL70;_LL73:;_LL74:
{const char*_tmp8E0;Cyc_Parse_err(((_tmp8E0="sign specification on non-integral type",_tag_dyneither(_tmp8E0,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 697
if(seen_size){
void*_tmp104=t;enum Cyc_Absyn_Sign _tmp106;enum Cyc_Absyn_Size_of _tmp107;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp105=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp104;if(_tmp105->tag != 6)goto _LL78;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;}}_LL77:
# 700
 if(_tmp107 != sz)
t=Cyc_Absyn_int_typ(_tmp106,sz);
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp104;if(_tmp108->tag != 7)goto _LL7A;}_LL79:
# 704
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
{const char*_tmp8E1;Cyc_Parse_err(((_tmp8E1="size qualifier on non-integral type",_tag_dyneither(_tmp8E1,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
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
struct _tuple15*_tmp8E4;struct Cyc_List_List*_tmp8E3;return(_tmp8E3=_region_malloc(r,sizeof(*_tmp8E3)),((_tmp8E3->hd=((_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4->f1=_tmp10A,((_tmp8E4->f2=_tmp10C,((_tmp8E4->f3=_tmp10D,((_tmp8E4->f4=_tmp10E,((_tmp8E4->f5=_tmp10F,_tmp8E4)))))))))))),((_tmp8E3->tl=0,_tmp8E3)))));}else{
# 724
struct _tuple15*_tmp8E7;struct Cyc_List_List*_tmp8E6;return(_tmp8E6=_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6->hd=((_tmp8E7=_region_malloc(r,sizeof(*_tmp8E7)),((_tmp8E7->f1=_tmp10A,((_tmp8E7->f2=_tmp10C,((_tmp8E7->f3=_tmp10D,((_tmp8E7->f4=_tmp10E,((_tmp8E7->f5=_tmp10F,_tmp8E7)))))))))))),((_tmp8E6->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8E6)))));}};}
# 728
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 731
if(tms == 0){struct _tuple13 _tmp8E8;return(_tmp8E8.f1=tq,((_tmp8E8.f2=t,((_tmp8E8.f3=0,((_tmp8E8.f4=atts,_tmp8E8)))))));}{
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
struct Cyc_List_List*_tmp8E9;fn_atts=((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9->hd=(void*)as->hd,((_tmp8E9->tl=fn_atts,_tmp8E9))))));}else{
# 749
struct Cyc_List_List*_tmp8EA;new_atts=((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->hd=(void*)as->hd,((_tmp8EA->tl=new_atts,_tmp8EA))))));}}}
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
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp130,((_tmp8ED="function declaration without parameter types",_tag_dyneither(_tmp8ED,sizeof(char),45))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp11F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp11F->tag != 4)goto _LL85;else{_tmp120=_tmp11F->f1;_tmp121=_tmp11F->f2;}}_LL84:
# 803
 if(tms->tl == 0){
struct _tuple13 _tmp8EE;return(_tmp8EE.f1=tq,((_tmp8EE.f2=t,((_tmp8EE.f3=_tmp120,((_tmp8EE.f4=atts,_tmp8EE)))))));}{
# 808
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp121,((_tmp8F1="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp8F1,sizeof(char),68))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp122=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp122->tag != 2)goto _LL87;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;}}_LL86: {
# 811
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8F7;struct Cyc_Absyn_PtrInfo _tmp8F6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F5;return Cyc_Parse_apply_tms(_tmp124,(void*)((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5[0]=((_tmp8F7.tag=5,((_tmp8F7.f1=((_tmp8F6.elt_typ=t,((_tmp8F6.elt_tq=tq,((_tmp8F6.ptr_atts=_tmp123,_tmp8F6)))))),_tmp8F7)))),_tmp8F5)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp125=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp115;if(_tmp125->tag != 5)goto _LL7C;else{_tmp126=_tmp125->f1;_tmp127=_tmp125->f2;}}_LL88:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp8FD;struct Cyc_Absyn_Typedefdecl*_tmp8FC;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp8FB;return Cyc_Absyn_new_decl((void*)((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FD.tag=9,((_tmp8FD.f1=((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC->name=_tmp145,((_tmp8FC->tvs=_tmp148,((_tmp8FC->kind=kind,((_tmp8FC->defn=type,((_tmp8FC->atts=_tmp149,((_tmp8FC->tq=_tmp146,_tmp8FC)))))))))))))),_tmp8FD)))),_tmp8FB)))),loc);};};}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp900;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp8FF;return Cyc_Absyn_new_stmt((void*)((_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=12,((_tmp900.f1=d,((_tmp900.f2=s,_tmp900)))))),_tmp8FF)))),d->loc);}
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
const char*_tmp901;Cyc_Parse_err(((_tmp901="inline is allowed only on function definitions",_tag_dyneither(_tmp901,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp907;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp906;struct Cyc_List_List*_tmp905;struct Cyc_List_List*_tmp174=(_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905->hd=Cyc_Absyn_new_decl((void*)((_tmp907=_cycalloc(sizeof(*_tmp907)),((_tmp907[0]=((_tmp906.tag=6,((_tmp906.f1=_tmp15C,_tmp906)))),_tmp907)))),loc),((_tmp905->tl=0,_tmp905)))));_npop_handler(0);return _tmp174;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp15D->tag != 26)goto _LLA5;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp15E=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp15D->f1)->r;if(_tmp15E->tag != 1)goto _LLA5;else{_tmp15F=_tmp15E->f1;}}}_LLA4:
# 916
 if(_tmp156 != 0){const char*_tmp908;Cyc_Parse_err(((_tmp908="attributes on enum not supported",_tag_dyneither(_tmp908,sizeof(char),33))),loc);}
_tmp15F->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp90E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp90D;struct Cyc_List_List*_tmp90C;struct Cyc_List_List*_tmp179=(_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C->hd=Cyc_Absyn_new_decl((void*)((_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90D.tag=8,((_tmp90D.f1=_tmp15F,_tmp90D)))),_tmp90E)))),loc),((_tmp90C->tl=0,_tmp90C)))));_npop_handler(0);return _tmp179;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159;if(_tmp160->tag != 26)goto _LLA7;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp161=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp160->f1)->r;if(_tmp161->tag != 2)goto _LLA7;else{_tmp162=_tmp161->f1;}}}_LLA6:
# 920
 if(_tmp156 != 0){const char*_tmp90F;Cyc_Parse_err(((_tmp90F="attributes on datatypes not supported",_tag_dyneither(_tmp90F,sizeof(char),38))),loc);}
_tmp162->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp915;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp914;struct Cyc_List_List*_tmp913;struct Cyc_List_List*_tmp17E=(_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->hd=Cyc_Absyn_new_decl((void*)((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915[0]=((_tmp914.tag=7,((_tmp914.f1=_tmp162,_tmp914)))),_tmp915)))),loc),((_tmp913->tl=0,_tmp913)))));_npop_handler(0);return _tmp17E;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp159;if(_tmp163->tag != 11)goto _LLA9;else{if((((_tmp163->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA9;_tmp164=((struct _tuple2)(((_tmp163->f1).aggr_info).UnknownAggr).val).f1;_tmp165=((struct _tuple2)(((_tmp163->f1).aggr_info).UnknownAggr).val).f2;_tmp166=(_tmp163->f1).targs;}}_LLA8: {
# 924
struct Cyc_List_List*_tmp17F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp166);
struct Cyc_Absyn_Aggrdecl*_tmp916;struct Cyc_Absyn_Aggrdecl*_tmp180=(_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->kind=_tmp164,((_tmp916->sc=s,((_tmp916->name=_tmp165,((_tmp916->tvs=_tmp17F,((_tmp916->impl=0,((_tmp916->attributes=0,_tmp916)))))))))))));
if(_tmp156 != 0){const char*_tmp917;Cyc_Parse_err(((_tmp917="bad attributes on type declaration",_tag_dyneither(_tmp917,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp91D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp91C;struct Cyc_List_List*_tmp91B;struct Cyc_List_List*_tmp185=(_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B->hd=Cyc_Absyn_new_decl((void*)((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91C.tag=6,((_tmp91C.f1=_tmp180,_tmp91C)))),_tmp91D)))),loc),((_tmp91B->tl=0,_tmp91B)))));_npop_handler(0);return _tmp185;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159;if(_tmp167->tag != 3)goto _LLAB;else{if((((_tmp167->f1).datatype_info).KnownDatatype).tag != 2)goto _LLAB;_tmp168=(struct Cyc_Absyn_Datatypedecl**)(((_tmp167->f1).datatype_info).KnownDatatype).val;}}_LLAA:
# 929
 if(_tmp156 != 0){const char*_tmp91E;Cyc_Parse_err(((_tmp91E="bad attributes on datatype",_tag_dyneither(_tmp91E,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp924;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp923;struct Cyc_List_List*_tmp922;struct Cyc_List_List*_tmp18B=(_tmp922=_cycalloc(sizeof(*_tmp922)),((_tmp922->hd=Cyc_Absyn_new_decl((void*)((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924[0]=((_tmp923.tag=7,((_tmp923.f1=*_tmp168,_tmp923)))),_tmp924)))),loc),((_tmp922->tl=0,_tmp922)))));_npop_handler(0);return _tmp18B;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp159;if(_tmp169->tag != 3)goto _LLAD;else{if((((_tmp169->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLAD;_tmp16A=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp169->f1).datatype_info).UnknownDatatype).val).name;_tmp16B=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp169->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp16C=(_tmp169->f1).targs;}}_LLAC: {
# 932
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp16C);
struct Cyc_Absyn_Decl*_tmp18D=Cyc_Absyn_datatype_decl(s,_tmp16A,_tmp18C,0,_tmp16B,loc);
if(_tmp156 != 0){const char*_tmp925;Cyc_Parse_err(((_tmp925="bad attributes on datatype",_tag_dyneither(_tmp925,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp926;struct Cyc_List_List*_tmp190=(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->hd=_tmp18D,((_tmp926->tl=0,_tmp926)))));_npop_handler(0);return _tmp190;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp159;if(_tmp16D->tag != 13)goto _LLAF;else{_tmp16E=_tmp16D->f1;}}_LLAE: {
# 937
struct Cyc_Absyn_Enumdecl*_tmp927;struct Cyc_Absyn_Enumdecl*_tmp191=(_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->sc=s,((_tmp927->name=_tmp16E,((_tmp927->fields=0,_tmp927)))))));
if(_tmp156 != 0){const char*_tmp928;Cyc_Parse_err(((_tmp928="bad attributes on enum",_tag_dyneither(_tmp928,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp932;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp931;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp930;struct Cyc_List_List*_tmp92F;struct Cyc_List_List*_tmp197=(_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->hd=((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->r=(void*)((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=8,((_tmp931.f1=_tmp191,_tmp931)))),_tmp930)))),((_tmp932->loc=loc,_tmp932)))))),((_tmp92F->tl=0,_tmp92F)))));_npop_handler(0);return _tmp197;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp159;if(_tmp16F->tag != 14)goto _LLB1;else{_tmp170=_tmp16F->f1;}}_LLB0: {
# 943
struct Cyc_Core_Opt*_tmp935;struct Cyc_Absyn_Enumdecl*_tmp934;struct Cyc_Absyn_Enumdecl*_tmp199=(_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934->sc=s,((_tmp934->name=Cyc_Parse_gensym_enum(),((_tmp934->fields=((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935->v=_tmp170,_tmp935)))),_tmp934)))))));
if(_tmp156 != 0){const char*_tmp936;Cyc_Parse_err(((_tmp936="bad attributes on enum",_tag_dyneither(_tmp936,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp940;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp93F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp93E;struct Cyc_List_List*_tmp93D;struct Cyc_List_List*_tmp19F=(_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->hd=((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940->r=(void*)((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=8,((_tmp93F.f1=_tmp199,_tmp93F)))),_tmp93E)))),((_tmp940->loc=loc,_tmp940)))))),((_tmp93D->tl=0,_tmp93D)))));_npop_handler(0);return _tmp19F;};}_LLB1:;_LLB2:
{const char*_tmp941;Cyc_Parse_err(((_tmp941="missing declarator",_tag_dyneither(_tmp941,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1A3=0;_npop_handler(0);return _tmp1A3;};_LLA0:;}else{
# 950
struct Cyc_List_List*_tmp1A4=Cyc_Parse_apply_tmss(mkrgn,_tmp154,_tmp158,declarators,_tmp156);
if(istypedef){
# 955
if(!exps_empty){
const char*_tmp942;Cyc_Parse_err(((_tmp942="initializer in typedef declaration",_tag_dyneither(_tmp942,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1A4);
struct Cyc_List_List*_tmp1A6=decls;_npop_handler(0);return _tmp1A6;};}else{
# 961
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1A7=_tmp1A4;for(0;_tmp1A7 != 0;(_tmp1A7=_tmp1A7->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple0*_tmp1A9;struct Cyc_Absyn_Tqual _tmp1AA;void*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple15*_tmp1A8=(struct _tuple15*)_tmp1A7->hd;_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;_tmp1AB=_tmp1A8->f3;_tmp1AC=_tmp1A8->f4;_tmp1AD=_tmp1A8->f5;
if(_tmp1AC != 0){
const char*_tmp945;void*_tmp944;(_tmp944=0,Cyc_Tcutil_warn(loc,((_tmp945="bad type params, ignoring",_tag_dyneither(_tmp945,sizeof(char),26))),_tag_dyneither(_tmp944,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp948;void*_tmp947;(_tmp947=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp948="unexpected NULL in parse!",_tag_dyneither(_tmp948,sizeof(char),26))),_tag_dyneither(_tmp947,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1B2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1B3=Cyc_Absyn_new_vardecl(_tmp1A9,_tmp1AB,_tmp1B2);
_tmp1B3->tq=_tmp1AA;
_tmp1B3->sc=s;
_tmp1B3->attributes=_tmp1AD;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp94E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp94D;struct Cyc_Absyn_Decl*_tmp94C;struct Cyc_Absyn_Decl*_tmp1B4=(_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->r=(void*)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp94D.tag=0,((_tmp94D.f1=_tmp1B3,_tmp94D)))),_tmp94E)))),((_tmp94C->loc=loc,_tmp94C)))));
struct Cyc_List_List*_tmp94F;decls=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->hd=_tmp1B4,((_tmp94F->tl=decls,_tmp94F))))));};};}}{
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
{const char*_tmp954;void*_tmp953[2];struct Cyc_String_pa_PrintArg_struct _tmp952;struct Cyc_Int_pa_PrintArg_struct _tmp951;Cyc_Parse_err((struct _dyneither_ptr)((_tmp951.tag=1,((_tmp951.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp952.tag=0,((_tmp952.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp953[0]=& _tmp952,((_tmp953[1]=& _tmp951,Cyc_aprintf(((_tmp954="bad kind: %s; strlen=%d",_tag_dyneither(_tmp954,sizeof(char),24))),_tag_dyneither(_tmp953,sizeof(void*),2)))))))))))))),loc);}
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
{const char*_tmp955;Cyc_Parse_err(((_tmp955="cannot mix patterns and expressions in case",_tag_dyneither(_tmp955,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp956;return((_tmp956.Int_tok).val=yy1,(((_tmp956.Int_tok).tag=1,_tmp956)));}static char _tmp1DB[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp957;return((_tmp957.Char_tok).val=yy1,(((_tmp957.Char_tok).tag=2,_tmp957)));}static char _tmp1E0[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp958;return((_tmp958.String_tok).val=yy1,(((_tmp958.String_tok).tag=3,_tmp958)));}static char _tmp1E5[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp959;return((_tmp959.YY1).val=yy1,(((_tmp959.YY1).tag=7,_tmp959)));}static char _tmp1EA[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp95A;return((_tmp95A.YY2).val=yy1,(((_tmp95A.YY2).tag=8,_tmp95A)));}static char _tmp1EF[28]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95B;return((_tmp95B.YY3).val=yy1,(((_tmp95B.YY3).tag=9,_tmp95B)));}static char _tmp1F4[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp95C;return((_tmp95C.YY4).val=yy1,(((_tmp95C.YY4).tag=10,_tmp95C)));}static char _tmp1F9[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp95D;return((_tmp95D.YY5).val=yy1,(((_tmp95D.YY5).tag=11,_tmp95D)));}static char _tmp1FE[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95E;return((_tmp95E.YY6).val=yy1,(((_tmp95E.YY6).tag=12,_tmp95E)));}static char _tmp203[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp95F;return((_tmp95F.YY7).val=yy1,(((_tmp95F.YY7).tag=13,_tmp95F)));}static char _tmp208[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp960;return((_tmp960.YY8).val=yy1,(((_tmp960.YY8).tag=14,_tmp960)));}static char _tmp20D[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp961;return((_tmp961.YY9).val=yy1,(((_tmp961.YY9).tag=15,_tmp961)));}static char _tmp212[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp962;return((_tmp962.QualId_tok).val=yy1,(((_tmp962.QualId_tok).tag=5,_tmp962)));}static char _tmp217[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp963;return((_tmp963.YY10).val=yy1,(((_tmp963.YY10).tag=16,_tmp963)));}static char _tmp21C[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp964;return((_tmp964.YY11).val=yy1,(((_tmp964.YY11).tag=17,_tmp964)));}static char _tmp221[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp965;return((_tmp965.YY12).val=yy1,(((_tmp965.YY12).tag=18,_tmp965)));}static char _tmp226[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY13(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.YY13).val=yy1,(((_tmp966.YY13).tag=19,_tmp966)));}static char _tmp22B[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.YY14).val=yy1,(((_tmp967.YY14).tag=20,_tmp967)));}static char _tmp230[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY15(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.YY15).val=yy1,(((_tmp968.YY15).tag=21,_tmp968)));}static char _tmp235[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY16).val=yy1,(((_tmp969.YY16).tag=22,_tmp969)));}static char _tmp23A[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY17(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY17).val=yy1,(((_tmp96A.YY17).tag=23,_tmp96A)));}static char _tmp23F[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY18).val=yy1,(((_tmp96B.YY18).tag=24,_tmp96B)));}static char _tmp244[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY19).val=yy1,(((_tmp96C.YY19).tag=25,_tmp96C)));}static char _tmp249[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY20).val=yy1,(((_tmp96D.YY20).tag=26,_tmp96D)));}static char _tmp24E[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY21).val=yy1,(((_tmp96E.YY21).tag=27,_tmp96E)));}static char _tmp253[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY22).val=yy1,(((_tmp96F.YY22).tag=28,_tmp96F)));}static char _tmp258[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY23).val=yy1,(((_tmp970.YY23).tag=29,_tmp970)));}static char _tmp25D[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY24).val=yy1,(((_tmp971.YY24).tag=30,_tmp971)));}static char _tmp262[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY25).val=yy1,(((_tmp972.YY25).tag=31,_tmp972)));}static char _tmp267[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY26).val=yy1,(((_tmp973.YY26).tag=32,_tmp973)));}static char _tmp26C[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY27).val=yy1,(((_tmp974.YY27).tag=33,_tmp974)));}static char _tmp271[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY28).val=yy1,(((_tmp975.YY28).tag=34,_tmp975)));}static char _tmp276[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY29).val=yy1,(((_tmp976.YY29).tag=35,_tmp976)));}static char _tmp27B[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY30).val=yy1,(((_tmp977.YY30).tag=36,_tmp977)));}static char _tmp280[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY31).val=yy1,(((_tmp978.YY31).tag=37,_tmp978)));}static char _tmp285[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY32).val=yy1,(((_tmp979.YY32).tag=38,_tmp979)));}static char _tmp28A[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY33).val=yy1,(((_tmp97A.YY33).tag=39,_tmp97A)));}static char _tmp28F[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY34).val=yy1,(((_tmp97B.YY34).tag=40,_tmp97B)));}static char _tmp294[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY35).val=yy1,(((_tmp97C.YY35).tag=41,_tmp97C)));}static char _tmp299[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY36(struct _tuple23 yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY36).val=yy1,(((_tmp97D.YY36).tag=42,_tmp97D)));}static char _tmp29E[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY37).val=yy1,(((_tmp97E.YY37).tag=43,_tmp97E)));}static char _tmp2A3[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY38).val=yy1,(((_tmp97F.YY38).tag=44,_tmp97F)));}static char _tmp2A8[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY39).val=yy1,(((_tmp980.YY39).tag=45,_tmp980)));}static char _tmp2AD[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY40(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY40).val=yy1,(((_tmp981.YY40).tag=46,_tmp981)));}static char _tmp2B2[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY41).val=yy1,(((_tmp982.YY41).tag=47,_tmp982)));}static char _tmp2B7[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY42).val=yy1,(((_tmp983.YY42).tag=48,_tmp983)));}static char _tmp2BC[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY43).val=yy1,(((_tmp984.YY43).tag=49,_tmp984)));}static char _tmp2C1[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY44).val=yy1,(((_tmp985.YY44).tag=50,_tmp985)));}static char _tmp2C6[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY45).val=yy1,(((_tmp986.YY45).tag=51,_tmp986)));}static char _tmp2CB[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY46).val=yy1,(((_tmp987.YY46).tag=52,_tmp987)));}static char _tmp2D0[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY47).val=yy1,(((_tmp988.YY47).tag=53,_tmp988)));}static char _tmp2D5[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY48).val=yy1,(((_tmp989.YY48).tag=54,_tmp989)));}static char _tmp2DA[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY49).val=yy1,(((_tmp98A.YY49).tag=55,_tmp98A)));}static char _tmp2DF[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY50).val=yy1,(((_tmp98B.YY50).tag=56,_tmp98B)));}static char _tmp2E4[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY51).val=yy1,(((_tmp98C.YY51).tag=57,_tmp98C)));}static char _tmp2E9[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY52(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY52).val=yy1,(((_tmp98D.YY52).tag=58,_tmp98D)));}static char _tmp2EE[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.YY53).val=yy1,(((_tmp98E.YY53).tag=59,_tmp98E)));}static char _tmp2F3[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _tmp98F;return((_tmp98F.YY54).val=yy1,(((_tmp98F.YY54).tag=60,_tmp98F)));}static char _tmp2F8[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.YY55).val=yy1,(((_tmp990.YY55).tag=61,_tmp990)));}static char _tmp2FD[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple19 yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.Asm_tok).val=yy1,(((_tmp991.Asm_tok).tag=6,_tmp991)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1191
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1195
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp992;return(_tmp992.timestamp=0,((_tmp992.first_line=0,((_tmp992.first_column=0,((_tmp992.last_line=0,((_tmp992.last_column=0,_tmp992)))))))));}
# 1198
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1209 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp302[2]="$";static char _tmp303[6]="error";static char _tmp304[12]="$undefined.";static char _tmp305[5]="AUTO";static char _tmp306[9]="REGISTER";static char _tmp307[7]="STATIC";static char _tmp308[7]="EXTERN";static char _tmp309[8]="TYPEDEF";static char _tmp30A[5]="VOID";static char _tmp30B[5]="CHAR";static char _tmp30C[6]="SHORT";static char _tmp30D[4]="INT";static char _tmp30E[5]="LONG";static char _tmp30F[6]="FLOAT";static char _tmp310[7]="DOUBLE";static char _tmp311[7]="SIGNED";static char _tmp312[9]="UNSIGNED";static char _tmp313[6]="CONST";static char _tmp314[9]="VOLATILE";static char _tmp315[9]="RESTRICT";static char _tmp316[7]="STRUCT";static char _tmp317[6]="UNION";static char _tmp318[5]="CASE";static char _tmp319[8]="DEFAULT";static char _tmp31A[7]="INLINE";static char _tmp31B[7]="SIZEOF";static char _tmp31C[9]="OFFSETOF";static char _tmp31D[3]="IF";static char _tmp31E[5]="ELSE";static char _tmp31F[7]="SWITCH";static char _tmp320[6]="WHILE";static char _tmp321[3]="DO";static char _tmp322[4]="FOR";static char _tmp323[5]="GOTO";static char _tmp324[9]="CONTINUE";static char _tmp325[6]="BREAK";static char _tmp326[7]="RETURN";static char _tmp327[5]="ENUM";static char _tmp328[8]="NULL_kw";static char _tmp329[4]="LET";static char _tmp32A[6]="THROW";static char _tmp32B[4]="TRY";static char _tmp32C[6]="CATCH";static char _tmp32D[7]="EXPORT";static char _tmp32E[4]="NEW";static char _tmp32F[9]="ABSTRACT";static char _tmp330[9]="FALLTHRU";static char _tmp331[6]="USING";static char _tmp332[10]="NAMESPACE";static char _tmp333[9]="DATATYPE";static char _tmp334[8]="XTUNION";static char _tmp335[7]="TUNION";static char _tmp336[7]="MALLOC";static char _tmp337[8]="RMALLOC";static char _tmp338[7]="CALLOC";static char _tmp339[8]="RCALLOC";static char _tmp33A[5]="SWAP";static char _tmp33B[9]="REGION_T";static char _tmp33C[6]="TAG_T";static char _tmp33D[7]="REGION";static char _tmp33E[5]="RNEW";static char _tmp33F[8]="REGIONS";static char _tmp340[13]="RESET_REGION";static char _tmp341[16]="NOZEROTERM_QUAL";static char _tmp342[14]="ZEROTERM_QUAL";static char _tmp343[12]="REGION_QUAL";static char _tmp344[7]="PORTON";static char _tmp345[8]="PORTOFF";static char _tmp346[12]="DYNREGION_T";static char _tmp347[6]="ALIAS";static char _tmp348[8]="NUMELTS";static char _tmp349[8]="VALUEOF";static char _tmp34A[10]="VALUEOF_T";static char _tmp34B[9]="TAGCHECK";static char _tmp34C[13]="NUMELTS_QUAL";static char _tmp34D[10]="THIN_QUAL";static char _tmp34E[9]="FAT_QUAL";static char _tmp34F[13]="NOTNULL_QUAL";static char _tmp350[14]="NULLABLE_QUAL";static char _tmp351[12]="TAGGED_QUAL";static char _tmp352[16]="EXTENSIBLE_QUAL";static char _tmp353[15]="RESETABLE_QUAL";static char _tmp354[7]="PTR_OP";static char _tmp355[7]="INC_OP";static char _tmp356[7]="DEC_OP";static char _tmp357[8]="LEFT_OP";static char _tmp358[9]="RIGHT_OP";static char _tmp359[6]="LE_OP";static char _tmp35A[6]="GE_OP";static char _tmp35B[6]="EQ_OP";static char _tmp35C[6]="NE_OP";static char _tmp35D[7]="AND_OP";static char _tmp35E[6]="OR_OP";static char _tmp35F[11]="MUL_ASSIGN";static char _tmp360[11]="DIV_ASSIGN";static char _tmp361[11]="MOD_ASSIGN";static char _tmp362[11]="ADD_ASSIGN";static char _tmp363[11]="SUB_ASSIGN";static char _tmp364[12]="LEFT_ASSIGN";static char _tmp365[13]="RIGHT_ASSIGN";static char _tmp366[11]="AND_ASSIGN";static char _tmp367[11]="XOR_ASSIGN";static char _tmp368[10]="OR_ASSIGN";static char _tmp369[9]="ELLIPSIS";static char _tmp36A[11]="LEFT_RIGHT";static char _tmp36B[12]="COLON_COLON";static char _tmp36C[11]="IDENTIFIER";static char _tmp36D[17]="INTEGER_CONSTANT";static char _tmp36E[7]="STRING";static char _tmp36F[8]="WSTRING";static char _tmp370[19]="CHARACTER_CONSTANT";static char _tmp371[20]="WCHARACTER_CONSTANT";static char _tmp372[18]="FLOATING_CONSTANT";static char _tmp373[9]="TYPE_VAR";static char _tmp374[13]="TYPEDEF_NAME";static char _tmp375[16]="QUAL_IDENTIFIER";static char _tmp376[18]="QUAL_TYPEDEF_NAME";static char _tmp377[10]="ATTRIBUTE";static char _tmp378[4]="ASM";static char _tmp379[4]="';'";static char _tmp37A[4]="'{'";static char _tmp37B[4]="'}'";static char _tmp37C[4]="','";static char _tmp37D[4]="'='";static char _tmp37E[4]="'<'";static char _tmp37F[4]="'>'";static char _tmp380[4]="'('";static char _tmp381[4]="')'";static char _tmp382[4]="'_'";static char _tmp383[4]="'$'";static char _tmp384[4]="':'";static char _tmp385[4]="'.'";static char _tmp386[4]="'['";static char _tmp387[4]="']'";static char _tmp388[4]="'*'";static char _tmp389[4]="'@'";static char _tmp38A[4]="'?'";static char _tmp38B[4]="'+'";static char _tmp38C[4]="'|'";static char _tmp38D[4]="'^'";static char _tmp38E[4]="'&'";static char _tmp38F[4]="'-'";static char _tmp390[4]="'/'";static char _tmp391[4]="'%'";static char _tmp392[4]="'~'";static char _tmp393[4]="'!'";static char _tmp394[5]="prog";static char _tmp395[17]="translation_unit";static char _tmp396[12]="export_list";static char _tmp397[19]="export_list_values";static char _tmp398[21]="external_declaration";static char _tmp399[20]="function_definition";static char _tmp39A[21]="function_definition2";static char _tmp39B[13]="using_action";static char _tmp39C[15]="unusing_action";static char _tmp39D[17]="namespace_action";static char _tmp39E[19]="unnamespace_action";static char _tmp39F[12]="declaration";static char _tmp3A0[19]="resetable_qual_opt";static char _tmp3A1[17]="declaration_list";static char _tmp3A2[23]="declaration_specifiers";static char _tmp3A3[24]="storage_class_specifier";static char _tmp3A4[15]="attributes_opt";static char _tmp3A5[11]="attributes";static char _tmp3A6[15]="attribute_list";static char _tmp3A7[10]="attribute";static char _tmp3A8[15]="type_specifier";static char _tmp3A9[25]="type_specifier_notypedef";static char _tmp3AA[5]="kind";static char _tmp3AB[15]="type_qualifier";static char _tmp3AC[15]="enum_specifier";static char _tmp3AD[11]="enum_field";static char _tmp3AE[22]="enum_declaration_list";static char _tmp3AF[26]="struct_or_union_specifier";static char _tmp3B0[16]="type_params_opt";static char _tmp3B1[16]="struct_or_union";static char _tmp3B2[24]="struct_declaration_list";static char _tmp3B3[25]="struct_declaration_list0";static char _tmp3B4[21]="init_declarator_list";static char _tmp3B5[22]="init_declarator_list0";static char _tmp3B6[16]="init_declarator";static char _tmp3B7[19]="struct_declaration";static char _tmp3B8[25]="specifier_qualifier_list";static char _tmp3B9[35]="notypedef_specifier_qualifier_list";static char _tmp3BA[23]="struct_declarator_list";static char _tmp3BB[24]="struct_declarator_list0";static char _tmp3BC[18]="struct_declarator";static char _tmp3BD[19]="datatype_specifier";static char _tmp3BE[14]="qual_datatype";static char _tmp3BF[19]="datatypefield_list";static char _tmp3C0[20]="datatypefield_scope";static char _tmp3C1[14]="datatypefield";static char _tmp3C2[11]="declarator";static char _tmp3C3[23]="declarator_withtypedef";static char _tmp3C4[18]="direct_declarator";static char _tmp3C5[30]="direct_declarator_withtypedef";static char _tmp3C6[8]="pointer";static char _tmp3C7[12]="one_pointer";static char _tmp3C8[14]="pointer_quals";static char _tmp3C9[13]="pointer_qual";static char _tmp3CA[23]="pointer_null_and_bound";static char _tmp3CB[14]="pointer_bound";static char _tmp3CC[18]="zeroterm_qual_opt";static char _tmp3CD[8]="rgn_opt";static char _tmp3CE[11]="tqual_list";static char _tmp3CF[20]="parameter_type_list";static char _tmp3D0[9]="type_var";static char _tmp3D1[16]="optional_effect";static char _tmp3D2[19]="optional_rgn_order";static char _tmp3D3[10]="rgn_order";static char _tmp3D4[16]="optional_inject";static char _tmp3D5[11]="effect_set";static char _tmp3D6[14]="atomic_effect";static char _tmp3D7[11]="region_set";static char _tmp3D8[15]="parameter_list";static char _tmp3D9[22]="parameter_declaration";static char _tmp3DA[16]="identifier_list";static char _tmp3DB[17]="identifier_list0";static char _tmp3DC[12]="initializer";static char _tmp3DD[18]="array_initializer";static char _tmp3DE[17]="initializer_list";static char _tmp3DF[12]="designation";static char _tmp3E0[16]="designator_list";static char _tmp3E1[11]="designator";static char _tmp3E2[10]="type_name";static char _tmp3E3[14]="any_type_name";static char _tmp3E4[15]="type_name_list";static char _tmp3E5[20]="abstract_declarator";static char _tmp3E6[27]="direct_abstract_declarator";static char _tmp3E7[10]="statement";static char _tmp3E8[13]="open_exp_opt";static char _tmp3E9[18]="labeled_statement";static char _tmp3EA[21]="expression_statement";static char _tmp3EB[19]="compound_statement";static char _tmp3EC[16]="block_item_list";static char _tmp3ED[20]="selection_statement";static char _tmp3EE[15]="switch_clauses";static char _tmp3EF[20]="iteration_statement";static char _tmp3F0[15]="jump_statement";static char _tmp3F1[12]="exp_pattern";static char _tmp3F2[20]="conditional_pattern";static char _tmp3F3[19]="logical_or_pattern";static char _tmp3F4[20]="logical_and_pattern";static char _tmp3F5[21]="inclusive_or_pattern";static char _tmp3F6[21]="exclusive_or_pattern";static char _tmp3F7[12]="and_pattern";static char _tmp3F8[17]="equality_pattern";static char _tmp3F9[19]="relational_pattern";static char _tmp3FA[14]="shift_pattern";static char _tmp3FB[17]="additive_pattern";static char _tmp3FC[23]="multiplicative_pattern";static char _tmp3FD[13]="cast_pattern";static char _tmp3FE[14]="unary_pattern";static char _tmp3FF[16]="postfix_pattern";static char _tmp400[16]="primary_pattern";static char _tmp401[8]="pattern";static char _tmp402[19]="tuple_pattern_list";static char _tmp403[20]="tuple_pattern_list0";static char _tmp404[14]="field_pattern";static char _tmp405[19]="field_pattern_list";static char _tmp406[20]="field_pattern_list0";static char _tmp407[11]="expression";static char _tmp408[22]="assignment_expression";static char _tmp409[20]="assignment_operator";static char _tmp40A[23]="conditional_expression";static char _tmp40B[20]="constant_expression";static char _tmp40C[22]="logical_or_expression";static char _tmp40D[23]="logical_and_expression";static char _tmp40E[24]="inclusive_or_expression";static char _tmp40F[24]="exclusive_or_expression";static char _tmp410[15]="and_expression";static char _tmp411[20]="equality_expression";static char _tmp412[22]="relational_expression";static char _tmp413[17]="shift_expression";static char _tmp414[20]="additive_expression";static char _tmp415[26]="multiplicative_expression";static char _tmp416[16]="cast_expression";static char _tmp417[17]="unary_expression";static char _tmp418[15]="unary_operator";static char _tmp419[19]="postfix_expression";static char _tmp41A[17]="field_expression";static char _tmp41B[19]="primary_expression";static char _tmp41C[25]="argument_expression_list";static char _tmp41D[26]="argument_expression_list0";static char _tmp41E[9]="constant";static char _tmp41F[20]="qual_opt_identifier";static char _tmp420[17]="qual_opt_typedef";static char _tmp421[18]="struct_union_name";static char _tmp422[11]="field_name";static char _tmp423[12]="right_angle";
# 1555 "parse.y"
static struct _dyneither_ptr Cyc_yytname[290]={{_tmp302,_tmp302,_tmp302 + 2},{_tmp303,_tmp303,_tmp303 + 6},{_tmp304,_tmp304,_tmp304 + 12},{_tmp305,_tmp305,_tmp305 + 5},{_tmp306,_tmp306,_tmp306 + 9},{_tmp307,_tmp307,_tmp307 + 7},{_tmp308,_tmp308,_tmp308 + 7},{_tmp309,_tmp309,_tmp309 + 8},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,_tmp30B,_tmp30B + 5},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 4},{_tmp30E,_tmp30E,_tmp30E + 5},{_tmp30F,_tmp30F,_tmp30F + 6},{_tmp310,_tmp310,_tmp310 + 7},{_tmp311,_tmp311,_tmp311 + 7},{_tmp312,_tmp312,_tmp312 + 9},{_tmp313,_tmp313,_tmp313 + 6},{_tmp314,_tmp314,_tmp314 + 9},{_tmp315,_tmp315,_tmp315 + 9},{_tmp316,_tmp316,_tmp316 + 7},{_tmp317,_tmp317,_tmp317 + 6},{_tmp318,_tmp318,_tmp318 + 5},{_tmp319,_tmp319,_tmp319 + 8},{_tmp31A,_tmp31A,_tmp31A + 7},{_tmp31B,_tmp31B,_tmp31B + 7},{_tmp31C,_tmp31C,_tmp31C + 9},{_tmp31D,_tmp31D,_tmp31D + 3},{_tmp31E,_tmp31E,_tmp31E + 5},{_tmp31F,_tmp31F,_tmp31F + 7},{_tmp320,_tmp320,_tmp320 + 6},{_tmp321,_tmp321,_tmp321 + 3},{_tmp322,_tmp322,_tmp322 + 4},{_tmp323,_tmp323,_tmp323 + 5},{_tmp324,_tmp324,_tmp324 + 9},{_tmp325,_tmp325,_tmp325 + 6},{_tmp326,_tmp326,_tmp326 + 7},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 8},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 6},{_tmp32B,_tmp32B,_tmp32B + 4},{_tmp32C,_tmp32C,_tmp32C + 6},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 4},{_tmp32F,_tmp32F,_tmp32F + 9},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 6},{_tmp332,_tmp332,_tmp332 + 10},{_tmp333,_tmp333,_tmp333 + 9},{_tmp334,_tmp334,_tmp334 + 8},{_tmp335,_tmp335,_tmp335 + 7},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 8},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 8},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 7},{_tmp33E,_tmp33E,_tmp33E + 5},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 13},{_tmp341,_tmp341,_tmp341 + 16},{_tmp342,_tmp342,_tmp342 + 14},{_tmp343,_tmp343,_tmp343 + 12},{_tmp344,_tmp344,_tmp344 + 7},{_tmp345,_tmp345,_tmp345 + 8},{_tmp346,_tmp346,_tmp346 + 12},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 8},{_tmp349,_tmp349,_tmp349 + 8},{_tmp34A,_tmp34A,_tmp34A + 10},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 13},{_tmp34D,_tmp34D,_tmp34D + 10},{_tmp34E,_tmp34E,_tmp34E + 9},{_tmp34F,_tmp34F,_tmp34F + 13},{_tmp350,_tmp350,_tmp350 + 14},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 16},{_tmp353,_tmp353,_tmp353 + 15},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 7},{_tmp356,_tmp356,_tmp356 + 7},{_tmp357,_tmp357,_tmp357 + 8},{_tmp358,_tmp358,_tmp358 + 9},{_tmp359,_tmp359,_tmp359 + 6},{_tmp35A,_tmp35A,_tmp35A + 6},{_tmp35B,_tmp35B,_tmp35B + 6},{_tmp35C,_tmp35C,_tmp35C + 6},{_tmp35D,_tmp35D,_tmp35D + 7},{_tmp35E,_tmp35E,_tmp35E + 6},{_tmp35F,_tmp35F,_tmp35F + 11},{_tmp360,_tmp360,_tmp360 + 11},{_tmp361,_tmp361,_tmp361 + 11},{_tmp362,_tmp362,_tmp362 + 11},{_tmp363,_tmp363,_tmp363 + 11},{_tmp364,_tmp364,_tmp364 + 12},{_tmp365,_tmp365,_tmp365 + 13},{_tmp366,_tmp366,_tmp366 + 11},{_tmp367,_tmp367,_tmp367 + 11},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 11},{_tmp36B,_tmp36B,_tmp36B + 12},{_tmp36C,_tmp36C,_tmp36C + 11},{_tmp36D,_tmp36D,_tmp36D + 17},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 8},{_tmp370,_tmp370,_tmp370 + 19},{_tmp371,_tmp371,_tmp371 + 20},{_tmp372,_tmp372,_tmp372 + 18},{_tmp373,_tmp373,_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 13},{_tmp375,_tmp375,_tmp375 + 16},{_tmp376,_tmp376,_tmp376 + 18},{_tmp377,_tmp377,_tmp377 + 10},{_tmp378,_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 4},{_tmp37F,_tmp37F,_tmp37F + 4},{_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 4},{_tmp382,_tmp382,_tmp382 + 4},{_tmp383,_tmp383,_tmp383 + 4},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 4},{_tmp394,_tmp394,_tmp394 + 5},{_tmp395,_tmp395,_tmp395 + 17},{_tmp396,_tmp396,_tmp396 + 12},{_tmp397,_tmp397,_tmp397 + 19},{_tmp398,_tmp398,_tmp398 + 21},{_tmp399,_tmp399,_tmp399 + 20},{_tmp39A,_tmp39A,_tmp39A + 21},{_tmp39B,_tmp39B,_tmp39B + 13},{_tmp39C,_tmp39C,_tmp39C + 15},{_tmp39D,_tmp39D,_tmp39D + 17},{_tmp39E,_tmp39E,_tmp39E + 19},{_tmp39F,_tmp39F,_tmp39F + 12},{_tmp3A0,_tmp3A0,_tmp3A0 + 19},{_tmp3A1,_tmp3A1,_tmp3A1 + 17},{_tmp3A2,_tmp3A2,_tmp3A2 + 23},{_tmp3A3,_tmp3A3,_tmp3A3 + 24},{_tmp3A4,_tmp3A4,_tmp3A4 + 15},{_tmp3A5,_tmp3A5,_tmp3A5 + 11},{_tmp3A6,_tmp3A6,_tmp3A6 + 15},{_tmp3A7,_tmp3A7,_tmp3A7 + 10},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 25},{_tmp3AA,_tmp3AA,_tmp3AA + 5},{_tmp3AB,_tmp3AB,_tmp3AB + 15},{_tmp3AC,_tmp3AC,_tmp3AC + 15},{_tmp3AD,_tmp3AD,_tmp3AD + 11},{_tmp3AE,_tmp3AE,_tmp3AE + 22},{_tmp3AF,_tmp3AF,_tmp3AF + 26},{_tmp3B0,_tmp3B0,_tmp3B0 + 16},{_tmp3B1,_tmp3B1,_tmp3B1 + 16},{_tmp3B2,_tmp3B2,_tmp3B2 + 24},{_tmp3B3,_tmp3B3,_tmp3B3 + 25},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 22},{_tmp3B6,_tmp3B6,_tmp3B6 + 16},{_tmp3B7,_tmp3B7,_tmp3B7 + 19},{_tmp3B8,_tmp3B8,_tmp3B8 + 25},{_tmp3B9,_tmp3B9,_tmp3B9 + 35},{_tmp3BA,_tmp3BA,_tmp3BA + 23},{_tmp3BB,_tmp3BB,_tmp3BB + 24},{_tmp3BC,_tmp3BC,_tmp3BC + 18},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 14},{_tmp3BF,_tmp3BF,_tmp3BF + 19},{_tmp3C0,_tmp3C0,_tmp3C0 + 20},{_tmp3C1,_tmp3C1,_tmp3C1 + 14},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 23},{_tmp3C4,_tmp3C4,_tmp3C4 + 18},{_tmp3C5,_tmp3C5,_tmp3C5 + 30},{_tmp3C6,_tmp3C6,_tmp3C6 + 8},{_tmp3C7,_tmp3C7,_tmp3C7 + 12},{_tmp3C8,_tmp3C8,_tmp3C8 + 14},{_tmp3C9,_tmp3C9,_tmp3C9 + 13},{_tmp3CA,_tmp3CA,_tmp3CA + 23},{_tmp3CB,_tmp3CB,_tmp3CB + 14},{_tmp3CC,_tmp3CC,_tmp3CC + 18},{_tmp3CD,_tmp3CD,_tmp3CD + 8},{_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 9},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,_tmp3D2,_tmp3D2 + 19},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 11},{_tmp3D6,_tmp3D6,_tmp3D6 + 14},{_tmp3D7,_tmp3D7,_tmp3D7 + 11},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,_tmp3D9 + 22},{_tmp3DA,_tmp3DA,_tmp3DA + 16},{_tmp3DB,_tmp3DB,_tmp3DB + 17},{_tmp3DC,_tmp3DC,_tmp3DC + 12},{_tmp3DD,_tmp3DD,_tmp3DD + 18},{_tmp3DE,_tmp3DE,_tmp3DE + 17},{_tmp3DF,_tmp3DF,_tmp3DF + 12},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 11},{_tmp3E2,_tmp3E2,_tmp3E2 + 10},{_tmp3E3,_tmp3E3,_tmp3E3 + 14},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 20},{_tmp3E6,_tmp3E6,_tmp3E6 + 27},{_tmp3E7,_tmp3E7,_tmp3E7 + 10},{_tmp3E8,_tmp3E8,_tmp3E8 + 13},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 21},{_tmp3EB,_tmp3EB,_tmp3EB + 19},{_tmp3EC,_tmp3EC,_tmp3EC + 16},{_tmp3ED,_tmp3ED,_tmp3ED + 20},{_tmp3EE,_tmp3EE,_tmp3EE + 15},{_tmp3EF,_tmp3EF,_tmp3EF + 20},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,_tmp3F1 + 12},{_tmp3F2,_tmp3F2,_tmp3F2 + 20},{_tmp3F3,_tmp3F3,_tmp3F3 + 19},{_tmp3F4,_tmp3F4,_tmp3F4 + 20},{_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 21},{_tmp3F7,_tmp3F7,_tmp3F7 + 12},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{_tmp3FA,_tmp3FA,_tmp3FA + 14},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 23},{_tmp3FD,_tmp3FD,_tmp3FD + 13},{_tmp3FE,_tmp3FE,_tmp3FE + 14},{_tmp3FF,_tmp3FF,_tmp3FF + 16},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,_tmp401 + 8},{_tmp402,_tmp402,_tmp402 + 19},{_tmp403,_tmp403,_tmp403 + 20},{_tmp404,_tmp404,_tmp404 + 14},{_tmp405,_tmp405,_tmp405 + 19},{_tmp406,_tmp406,_tmp406 + 20},{_tmp407,_tmp407,_tmp407 + 11},{_tmp408,_tmp408,_tmp408 + 22},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 23},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 22},{_tmp40D,_tmp40D,_tmp40D + 23},{_tmp40E,_tmp40E,_tmp40E + 24},{_tmp40F,_tmp40F,_tmp40F + 24},{_tmp410,_tmp410,_tmp410 + 15},{_tmp411,_tmp411,_tmp411 + 20},{_tmp412,_tmp412,_tmp412 + 22},{_tmp413,_tmp413,_tmp413 + 17},{_tmp414,_tmp414,_tmp414 + 20},{_tmp415,_tmp415,_tmp415 + 26},{_tmp416,_tmp416,_tmp416 + 16},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 15},{_tmp419,_tmp419,_tmp419 + 19},{_tmp41A,_tmp41A,_tmp41A + 17},{_tmp41B,_tmp41B,_tmp41B + 19},{_tmp41C,_tmp41C,_tmp41C + 25},{_tmp41D,_tmp41D,_tmp41D + 26},{_tmp41E,_tmp41E,_tmp41E + 9},{_tmp41F,_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 17},{_tmp421,_tmp421,_tmp421 + 18},{_tmp422,_tmp422,_tmp422 + 11},{_tmp423,_tmp423,_tmp423 + 12}};
# 1609
static short Cyc_yyr1[518]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,151,151,152,152,153,154,155,156,157,157,157,157,157,157,157,158,158,159,159,160,160,160,160,160,160,160,160,160,160,161,161,161,161,161,161,161,162,162,163,164,164,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,168,169,169,169,170,170,170,171,171,172,172,172,173,173,173,173,173,174,174,175,175,176,176,177,177,178,179,179,180,180,181,182,182,182,182,182,182,183,183,183,183,183,183,184,185,185,186,186,186,186,187,187,187,188,188,189,189,189,189,190,190,190,191,191,192,192,193,193,194,194,194,194,194,194,194,194,194,194,195,195,195,195,195,195,195,195,195,195,195,196,196,197,198,198,199,199,199,199,199,199,199,199,200,200,200,201,201,202,202,202,203,203,203,204,204,205,205,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,212,212,212,213,213,214,214,215,215,215,216,217,217,218,218,219,219,219,219,219,220,220,220,220,221,222,222,223,223,224,224,225,225,225,225,225,226,226,227,227,227,228,228,228,228,228,228,228,228,228,228,228,229,229,229,229,229,229,229,230,230,231,232,232,233,233,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,281,281,281,281,281,281,281,281,281,281,282,283,283,284,284,284,284,284,285,285,286,286,287,287,288,288,289,289};
# 1664
static short Cyc_yyr2[518]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1719
static short Cyc_yydefact[1030]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,147,88,92,0,0,0,0,0,0,0,38,508,213,510,509,511,0,0,72,0,199,199,198,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,158,0,183,186,85,162,111,55,54,48,0,99,507,0,508,503,504,505,506,111,0,373,0,0,0,0,236,0,375,376,26,28,0,0,0,0,0,0,0,0,148,0,0,0,0,0,0,0,196,197,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,512,513,111,111,37,39,37,0,20,0,215,0,171,159,184,193,192,0,0,190,191,194,195,204,186,0,70,0,55,103,0,101,0,508,383,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,492,493,472,0,0,0,0,0,475,473,474,0,400,402,416,424,426,428,430,432,434,437,442,445,448,452,0,454,476,491,490,0,384,382,33,0,0,111,0,0,0,129,125,127,255,257,0,0,9,10,0,0,0,111,514,515,214,94,0,0,163,73,234,0,231,0,3,0,5,0,285,0,31,0,0,37,21,0,116,117,142,110,0,145,0,0,0,0,0,0,0,0,0,0,0,0,508,288,290,0,296,292,0,294,278,279,280,0,281,282,283,0,40,22,122,262,0,221,237,0,0,217,215,0,201,0,0,0,206,58,205,187,0,0,104,100,0,0,377,0,0,397,0,0,0,0,251,393,398,0,395,0,462,0,418,452,0,419,420,0,0,0,0,0,0,0,0,0,455,456,37,0,0,0,458,459,457,0,374,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,406,407,408,409,410,411,412,413,414,415,405,0,460,0,482,483,0,0,0,495,0,111,390,391,0,388,0,238,0,0,0,0,258,229,0,130,135,131,133,126,128,215,0,264,256,265,517,516,0,87,91,0,89,37,93,109,64,63,0,61,164,215,233,160,264,235,172,173,0,86,200,27,0,29,0,0,0,0,121,123,240,239,23,106,118,0,0,0,137,138,140,0,111,111,153,0,0,0,0,0,0,37,0,325,326,327,0,0,329,0,0,0,0,297,293,122,295,291,289,0,170,222,0,0,0,228,216,223,167,0,0,0,217,169,203,202,165,201,0,0,207,59,112,0,105,423,102,98,386,387,254,0,394,250,252,381,0,0,0,0,241,246,0,0,0,0,0,0,0,0,0,0,0,0,0,494,501,0,500,401,425,0,427,429,431,433,435,436,440,441,438,439,443,444,446,447,449,450,451,404,403,481,478,0,480,0,0,0,378,0,385,32,0,379,0,0,259,136,132,134,0,217,0,201,0,266,0,215,0,277,261,0,36,111,0,0,0,129,0,111,0,215,0,182,161,232,4,6,0,35,0,0,141,124,142,0,217,146,155,154,0,0,149,0,0,0,304,0,0,0,0,0,324,328,0,0,0,0,287,37,24,263,215,0,225,0,0,218,0,168,221,209,166,189,188,207,185,0,7,0,253,396,399,461,0,0,242,0,247,464,0,0,0,0,468,471,0,0,0,0,0,453,497,0,0,479,477,0,0,389,392,380,260,230,273,0,267,268,201,0,0,217,201,0,90,217,0,0,0,60,62,0,174,0,0,217,0,201,0,0,0,139,143,115,144,156,153,153,0,0,0,0,0,0,0,0,0,0,0,0,0,304,330,0,0,25,217,0,226,224,0,217,0,208,0,8,487,0,486,0,243,248,0,0,0,0,421,422,481,480,499,0,502,417,496,498,0,272,270,276,275,0,269,201,115,0,67,65,66,175,181,178,0,180,176,201,0,34,0,0,152,151,298,304,0,0,0,0,0,0,332,333,335,337,339,341,343,345,348,353,356,359,363,365,371,372,0,37,301,310,0,0,0,0,0,0,0,0,0,0,331,284,211,227,219,210,215,13,0,14,463,0,0,249,465,0,0,470,469,484,0,274,271,0,0,179,177,286,108,0,0,0,304,0,369,0,0,366,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,367,305,0,312,0,0,0,320,0,0,0,0,0,0,303,0,217,12,15,0,489,488,0,0,0,485,107,0,157,299,300,0,0,0,0,304,306,336,0,338,340,342,344,346,347,351,352,349,350,354,355,357,358,360,361,362,0,311,313,314,0,322,321,0,316,0,0,0,220,212,16,0,0,0,0,0,302,368,0,364,307,0,37,315,323,317,318,0,245,244,466,0,68,0,334,304,308,319,0,370,309,467,0,0,0};
# 1825
static short Cyc_yydefgoto[144]={1027,50,685,874,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,646,647,648,68,476,69,449,70,71,158,159,72,117,522,316,682,606,73,607,516,673,508,512,513,420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,293,294,295,759,296,297,835,836,837,838,839,840,841,842,843,844,845,846,847,848,849,850,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,212,213,214,783,215,563,564,216,217,75,875,244,435};
# 1843
static short Cyc_yypact[1030]={2770,- -32768,- -32768,- -32768,- -32768,2,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3505,265,3196,- -32768,- 29,99,- -32768,83,117,133,152,187,196,212,61,305,- -32768,- -32768,247,- -32768,- -32768,- -32768,257,595,288,280,312,312,- -32768,- -32768,2633,- -32768,166,178,- -32768,408,692,3505,3505,3505,- -32768,3505,- -32768,- -32768,827,- -32768,- 29,3342,202,122,521,990,- -32768,- -32768,357,387,385,- -32768,- 29,394,- -32768,4610,236,- -32768,- -32768,- -32768,- -32768,357,5997,- -32768,412,474,4610,428,460,486,- -32768,130,- -32768,- -32768,3651,3651,2633,2633,3651,506,5997,561,- -32768,119,542,907,553,119,3870,5997,- -32768,- -32768,- -32768,2633,2906,2633,2906,78,- -32768,565,575,- -32768,3260,- -32768,- -32768,- -32768,- -32768,3870,- -32768,- -32768,357,160,1645,- -32768,3342,692,- -32768,3651,3578,4367,- -32768,202,- -32768,- -32768,- -32768,573,589,- -32768,- -32768,- -32768,- -32768,45,990,3651,- -32768,2906,- -32768,597,607,609,- 29,136,- -32768,4610,103,1097,6202,614,5997,6046,637,646,649,660,669,671,675,677,6251,6251,- -32768,- -32768,- -32768,2212,679,6300,6300,6300,- -32768,- -32768,- -32768,34,- -32768,- -32768,- 46,653,681,683,680,93,151,356,- 43,258,- -32768,828,6300,198,- 34,- -32768,705,1358,723,- -32768,- -32768,725,5997,357,1358,726,313,3870,1073,3870,822,- -32768,85,85,- -32768,- -32768,66,734,283,357,- -32768,- -32768,- -32768,- -32768,63,736,- -32768,- -32768,590,293,- -32768,740,- -32768,747,- -32768,754,757,764,- -32768,907,4465,3342,- -32768,762,3870,- -32768,756,765,- 29,768,763,235,767,3975,769,790,779,780,4563,3975,263,777,771,- -32768,- -32768,784,1788,1788,692,1788,- -32768,- -32768,- -32768,791,- -32768,- -32768,- -32768,146,- -32768,- -32768,792,776,101,810,- -32768,72,804,788,218,812,304,786,3651,5997,- -32768,823,- -32768,- -32768,101,825,- 29,- -32768,5997,829,- -32768,830,834,- -32768,119,5997,4610,286,- -32768,- -32768,- -32768,832,846,2212,- -32768,3870,- -32768,- -32768,1223,- -32768,878,5997,5997,5997,5997,5997,5997,3870,673,2212,- -32768,- -32768,1931,845,361,5997,- -32768,- -32768,- -32768,5997,- -32768,6300,5997,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,5997,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5997,- -32768,119,- -32768,- -32768,4658,119,5997,- -32768,853,357,- -32768,- -32768,854,861,4610,- -32768,404,1097,864,3651,- -32768,871,873,- -32768,1073,1073,1073,- -32768,- -32768,3053,4753,183,- -32768,352,- -32768,- -32768,72,- -32768,- -32768,3651,- -32768,3424,- -32768,872,- -32768,875,887,876,- -32768,2980,- -32768,365,698,- -32768,- -32768,- -32768,3870,- -32768,- -32768,- -32768,2633,- -32768,2633,891,880,877,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,870,5997,897,895,- -32768,889,547,357,357,585,5997,5997,898,901,5997,995,2074,908,- -32768,- -32768,- -32768,426,986,- -32768,4851,5997,3975,2350,- -32768,- -32768,3260,- -32768,- -32768,- -32768,3651,- -32768,- -32768,3870,903,351,- -32768,- -32768,893,- -32768,72,905,3797,788,- -32768,- -32768,- -32768,- -32768,304,185,910,775,- -32768,- -32768,2496,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,902,- -32768,- -32768,- -32768,- -32768,1302,911,912,933,- -32768,- -32768,332,4465,913,923,925,926,373,922,924,217,929,931,6124,- -32768,- -32768,930,936,- -32768,653,355,681,683,680,93,151,151,356,356,356,356,- 43,- 43,258,258,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,934,- -32768,- 16,3651,4269,- -32768,3089,- -32768,- -32768,939,- -32768,209,949,- -32768,- -32768,- -32768,- -32768,942,788,943,304,938,352,3651,3724,4946,- -32768,- -32768,85,- -32768,357,799,945,63,3126,946,357,3651,3578,5044,- -32768,365,- -32768,- -32768,- -32768,948,- -32768,969,874,- -32768,- -32768,756,5997,788,- -32768,- -32768,- -32768,955,- 29,457,382,414,5997,578,448,952,5142,5237,494,- -32768,- -32768,959,976,973,476,- -32768,3342,- -32768,776,977,3651,- -32768,980,72,- -32768,978,- -32768,139,- -32768,- -32768,- -32768,- -32768,775,- -32768,982,- -32768,2633,- -32768,- -32768,- -32768,984,345,981,- -32768,4073,- -32768,- -32768,5997,1081,5997,6046,- -32768,- -32768,119,119,984,985,4269,- -32768,- -32768,5997,5997,- -32768,- -32768,101,636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,987,- -32768,- -32768,304,101,988,788,304,975,- -32768,788,480,991,993,- -32768,- -32768,994,- -32768,101,996,788,997,304,999,5997,992,- -32768,- -32768,3870,- -32768,983,49,585,3975,1007,1001,3825,1003,1008,3975,5997,5332,514,5427,524,5522,578,- -32768,998,1015,- -32768,788,239,- -32768,- -32768,1023,788,3870,- -32768,282,- -32768,- -32768,238,- -32768,5997,- -32768,- -32768,4465,1010,1012,1018,- -32768,878,1016,1017,- -32768,639,- -32768,- -32768,- -32768,- -32768,4269,- -32768,- -32768,- -32768,- -32768,1019,- -32768,304,3870,1040,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1021,- -32768,- -32768,304,490,- -32768,1029,3870,- -32768,- -32768,1123,578,1034,6349,1030,2350,6300,1025,- -32768,4,- -32768,1066,1020,626,688,220,730,284,303,- -32768,- -32768,- -32768,- -32768,1068,6300,1931,- -32768,- -32768,507,3975,513,5617,3975,523,5712,5807,619,1039,- -32768,- -32768,- -32768,- -32768,1041,- -32768,977,- -32768,1043,633,- -32768,459,368,- -32768,- -32768,3870,1136,- -32768,- -32768,- -32768,4171,- -32768,- -32768,1045,1046,- -32768,- -32768,- -32768,- -32768,556,3975,1048,578,2212,- -32768,3870,1035,- -32768,1502,6300,5997,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,6300,5997,- -32768,- -32768,1052,- -32768,3975,3975,571,- -32768,3975,3975,581,3975,587,5902,- -32768,72,788,- -32768,- -32768,561,- -32768,- -32768,2350,1047,1049,- -32768,- -32768,1065,- -32768,- -32768,- -32768,1056,1051,1057,6300,578,- -32768,653,383,681,683,683,93,151,151,356,356,356,356,- 43,- 43,258,258,- -32768,- -32768,- -32768,441,- -32768,- -32768,- -32768,3975,- -32768,- -32768,3975,- -32768,3975,3975,596,- -32768,- -32768,- -32768,1060,733,1055,3870,1058,- -32768,984,345,- -32768,- -32768,5997,1502,- -32768,- -32768,- -32768,- -32768,3975,- -32768,- -32768,- -32768,1064,- -32768,427,- -32768,578,- -32768,- -32768,1067,- -32768,- -32768,- -32768,1173,1183,- -32768};
# 1949
static short Cyc_yypgoto[144]={- -32768,15,- -32768,240,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 94,16,- -32768,- -32768,0,570,- -32768,91,- 150,1079,5,- -32768,- -32768,- 128,- -32768,275,1160,- 678,- -32768,- -32768,- -32768,937,940,672,512,- -32768,- -32768,558,- -32768,- -32768,113,- -32768,- -32768,67,- 207,1126,- 226,440,- -32768,1054,- -32768,- -32768,1151,- 410,- -32768,525,- 117,- 140,- 133,- 337,270,538,543,- 400,- 470,- 112,- 401,- 88,- -32768,- 260,- 166,- 541,- 334,- -32768,884,- 91,- 86,- 156,- 59,- 286,- 242,- -32768,- -32768,- -32768,- 27,- 257,- -32768,- 724,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 10,1002,- -32768,676,805,- -32768,112,618,- -32768,- 155,- 294,- 162,- 355,- 343,- 352,849,- 342,- 350,- 327,- 290,- 288,- 186,466,464,869,221,- -32768,- 341,- -32768,107,502,- 63,98,- 182,20};
# 1971
static short Cyc_yytable[6495]={59,465,136,250,319,62,361,362,363,549,344,566,308,96,345,233,234,569,317,237,341,59,572,573,140,568,62,398,307,571,499,500,486,502,616,262,537,78,493,324,671,143,448,865,112,136,366,574,575,576,577,59,714,631,644,645,62,310,59,59,59,589,59,62,62,62,119,62,59,147,405,824,169,62,130,131,132,38,133,423,442,19,20,220,142,140,41,419,578,579,367,60,580,581,380,288,905,299,381,358,559,406,263,59,59,897,364,668,62,62,76,113,60,678,300,674,631,712,235,236,59,59,59,59,129,62,62,62,62,59,97,719,889,509,62,253,254,255,256,59,906,59,60,611,62,142,62,536,147,60,60,60,432,60,662,289,364,142,39,60,325,365,59,137,611,797,511,62,531,443,- 150,432,430,315,956,531,518,320,638,246,961,677,372,373,257,39,453,432,437,97,451,433,510,530,60,60,582,583,584,723,97,198,258,434,453,100,240,101,409,301,433,60,60,60,60,409,326,587,299,239,60,590,434,505,630,241,433,524,38,422,60,327,60,242,424,468,1005,41,374,375,624,102,170,288,288,507,288,543,44,544,224,630,103,60,436,664,225,438,306,549,171,556,59,558,624,504,- 215,62,364,- 215,721,104,423,423,423,376,377,97,142,97,399,400,401,1021,160,120,121,59,59,694,59,269,62,62,511,62,1025,122,123,702,400,401,359,289,289,749,289,913,914,427,756,105,679,288,804,428,527,531,808,43,106,538,434,506,402,97,144,301,145,403,404,600,97,820,146,414,718,306,107,528,517,38,170,402,915,916,434,748,703,404,41,617,110,60,109,- 237,501,59,- 237,788,171,482,62,172,483,876,869,520,521,877,419,38,549,289,707,511,434,872,60,60,41,60,494,111,625,79,250,453,38,495,807,492,382,114,810,618,40,41,42,888,383,384,596,873,818,364,115,665,453,539,440,892,268,270,453,454,737,329,330,667,455,919,422,422,422,920,39,424,424,424,737,615,116,787,418,868,713,921,97,59,871,378,379,656,62,922,923,623,60,629,359,241,782,692,693,142,725,140,59,242,59,419,554,62,39,62,359,124,788,43,739,637,670,632,666,633,612,364,613,567,727,160,43,364,614,710,531,59,561,626,364,627,62,163,741,364,726,628,947,415,699,59,74,142,364,364,62,162,784,754,740,149,828,1006,167,441,591,142,855,97,794,795,97,597,80,98,364,99,879,59,60,681,511,792,62,772,364,793,218,742,674,755,787,777,684,659,74,221,364,60,962,60,788,74,1024,799,965,966,877,74,625,968,969,364,964,241,945,135,967,138,364,1007,74,242,453,760,752,453,60,753,219,166,222,773,98,716,970,971,972,973,644,645,60,649,650,98,926,653,364,657,757,758,811,770,994,74,74,812,663,223,135,615,364,766,74,928,364,893,932,238,60,299,74,74,74,74,787,974,975,364,629,976,977,859,927,364,364,730,771,339,929,342,342,862,74,364,364,960,903,97,933,355,356,38,954,47,48,49,342,342,342,40,41,42,74,59,925,38,245,166,62,429,98,471,98,40,41,42,454,342,247,142,593,953,259,59,681,983,984,450,62,986,987,364,989,946,38,260,985,313,781,38,97,364,40,41,42,477,988,364,41,81,43,895,990,314,447,136,364,321,98,44,428,1012,47,48,49,98,322,47,48,49,323,800,252,978,979,980,938,941,340,364,902,1008,368,805,1009,851,1010,1011,1020,452,943,642,643,944,60,801,802,816,885,886,74,346,312,909,910,763,765,452,1022,479,347,1004,484,348,60,911,912,38,84,187,188,85,86,87,349,249,41,342,948,74,16,17,18,350,342,351,38,354,511,352,191,353,38,360,267,41,957,288,958,125,40,41,42,917,918,249,44,369,371,784,370,166,447,407,47,48,49,412,428,413,342,98,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,1019,417,59,1014,364,996,822,62,439,288,456,38,446,97,826,827,429,457,289,40,41,42,856,858,459,861,463,864,467,461,136,471,469,385,478,472,450,480,481,47,48,49,485,731,487,488,878,489,490,421,497,426,496,59,732,733,734,1016,62,498,477,503,434,98,261,507,98,515,523,289,386,387,388,389,390,391,392,393,394,395,514,525,38,603,604,605,267,342,519,43,40,41,42,60,198,529,134,427,452,533,396,452,541,428,534,47,48,49,535,74,467,74,288,550,551,552,553,542,555,366,931,560,452,935,937,38,592,562,452,38,594,565,595,40,41,42,43,40,41,42,599,619,601,602,60,471,634,622,635,471,620,636,585,47,48,49,59,47,48,49,359,62,38,621,586,639,640,963,641,562,652,41,289,651,654,342,658,660,669,672,74,675,44,690,686,981,680,689,691,695,47,48,49,98,696,610,697,698,700,992,701,705,150,151,152,704,708,709,997,717,711,39,477,153,154,155,156,157,720,722,724,735,738,745,746,750,477,761,767,477,7,8,9,10,11,12,13,14,15,16,17,18,19,20,768,306,98,60,249,769,774,780,776,706,785,790,342,809,825,22,823,796,562,803,806,562,866,813,249,814,815,27,817,819,452,249,829,830,854,28,29,821,853,867,81,870,880,881,452,882,32,452,883,884,34,887,890,891,751,894,896,35,36,898,904,901,907,908,924,939,949,959,940,942,342,951,467,952,955,249,982,1000,1028,998,999,342,1001,1002,1003,249,1013,1015,1029,995,1017,39,74,82,249,43,1023,736,248,1026,108,148,464,747,118,328,45,46,168,84,470,779,85,86,87,993,467,41,318,778,775,540,88,688,570,598,852,557,89,1018,90,91,416,329,330,0,92,729,0,0,0,0,93,0,0,0,0,0,0,0,0,744,0,173,174,0,0,0,0,0,545,0,0,0,98,0,81,0,175,0,0,0,176,0,562,0,0,0,0,0,177,178,179,180,0,0,0,135,181,0,249,0,0,0,0,0,0,0,182,183,421,184,900,0,249,342,0,0,0,0,0,185,186,0,0,0,467,0,0,789,0,791,0,342,0,0,0,0,0,467,0,0,798,0,38,84,187,188,85,86,87,0,0,41,0,81,189,0,343,546,0,0,0,0,190,0,0,191,0,329,330,0,192,0,0,193,0,0,194,195,0,0,196,197,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,82,0,0,81,0,0,0,0,0,0,0,0,687,467,0,168,84,0,0,85,86,87,0,0,41,0,0,467,267,88,0,0,342,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,0,0,0,135,0,0,82,249,0,0,0,0,0,0,0,0,0,0,408,0,0,168,84,0,0,85,86,87,0,342,41,0,0,0,0,88,0,0,0,267,0,89,0,90,91,0,0,0,0,92,0,0,0,0,249,93,0,0,0,0,0,467,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,757,758,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 304,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,655,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,0,45,46,28,29,428,0,47,48,49,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,427,82,45,46,28,29,428,0,47,48,49,0,0,715,0,32,168,84,0,34,85,86,87,0,0,41,35,36,0,0,88,0,0,0,0,0,89,0,90,91,0,0,0,0,92,0,0,0,0,0,93,0,0,38,0,81,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,471,0,45,46,0,0,0,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,82,0,0,0,0,0,0,0,0,0,22,0,23,0,0,83,84,0,24,85,86,87,27,0,41,0,0,0,0,88,28,29,- 37,0,0,89,0,90,91,0,0,32,33,92,0,34,0,0,0,93,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,831,832,45,46,28,29,0,0,0,0,0,0,0,81,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,676,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,82,0,0,27,0,0,0,0,0,45,46,28,29,0,0,168,84,0,0,85,86,87,32,0,41,0,34,0,0,88,0,0,0,35,36,833,0,90,91,0,0,0,0,92,0,0,834,0,0,93,195,0,0,196,197,0,0,0,0,0,0,0,0,0,0,0,0,39,40,0,42,43,0,0,0,0,0,0,0,0,0,0,45,46,173,174,271,0,272,273,274,275,276,277,278,279,0,81,0,175,280,0,0,176,0,281,0,0,0,0,0,177,178,179,180,0,0,0,0,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,786,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,950,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,82,176,194,195,0,0,196,197,0,177,178,179,180,0,0,168,84,181,0,85,86,87,0,0,41,0,0,182,183,88,184,0,0,0,0,89,0,90,91,0,185,186,0,92,0,0,0,0,0,93,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,661,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,728,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,743,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,762,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,764,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,857,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,860,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,863,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,930,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,934,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,991,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,706,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,173,174,0,0,0,0,0,0,0,185,186,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,338,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,173,174,0,354,0,0,191,0,0,185,186,192,0,81,193,0,0,194,195,0,0,196,197,0,0,0,0,177,178,179,180,0,38,84,187,188,85,86,87,0,0,41,0,0,189,182,183,0,184,0,0,0,190,0,0,191,0,0,185,186,192,0,0,193,0,0,194,195,0,0,196,197,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,899,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197};
# 2624
static short Cyc_yycheck[6495]={0,261,65,115,160,0,192,193,194,343,176,366,145,23,176,101,102,369,158,105,175,21,372,373,68,368,21,213,145,371,287,288,274,290,434,129,330,21,280,167,510,68,249,767,44,108,92,374,375,376,377,51,593,454,5,6,51,145,58,59,60,402,62,58,59,60,51,62,68,69,104,749,82,68,58,59,60,106,62,229,17,20,21,93,68,129,115,227,378,379,136,0,380,381,137,139,92,141,141,190,357,135,129,103,104,829,122,508,103,104,108,44,21,523,141,515,517,133,103,104,120,121,122,123,57,120,121,122,123,129,23,601,810,61,129,120,121,122,123,139,136,141,51,429,139,129,141,329,148,58,59,60,86,62,495,139,122,141,113,68,170,127,162,65,450,706,306,162,323,106,121,86,231,128,898,330,309,162,472,112,904,518,89,90,106,113,249,86,122,82,249,125,120,321,103,104,382,383,384,609,93,89,124,137,267,106,108,124,218,142,125,120,121,122,123,225,113,399,262,107,129,403,137,122,450,106,125,313,106,229,139,128,141,114,229,262,960,115,87,88,447,124,106,287,288,106,290,338,126,340,120,477,119,162,234,497,126,237,119,593,124,352,262,354,471,119,127,262,122,130,607,119,422,423,424,124,125,170,262,172,82,83,84,1007,124,119,120,287,288,549,290,131,287,288,434,290,1020,119,120,82,83,84,190,287,288,642,290,87,88,126,651,124,127,357,724,132,316,472,728,117,124,331,137,303,126,218,124,260,126,131,132,417,225,743,132,223,127,119,126,319,122,106,106,126,124,125,137,641,131,132,115,437,105,262,49,119,289,357,122,693,124,126,357,88,129,127,127,63,64,131,510,106,706,357,560,515,137,778,287,288,115,290,119,126,447,120,498,450,106,126,727,279,134,105,731,439,114,115,116,809,142,143,412,121,741,122,126,501,471,123,127,821,137,138,477,122,623,131,132,505,127,137,422,423,424,141,113,422,423,424,637,431,120,693,121,772,592,134,331,439,777,85,86,487,439,142,143,447,357,449,338,106,107,121,122,439,612,501,458,114,460,601,350,458,113,460,354,59,802,117,626,471,121,458,501,460,124,122,126,367,613,124,117,122,132,130,641,487,127,124,122,126,487,108,627,122,613,132,130,224,127,501,0,487,122,122,501,120,690,127,627,71,754,130,120,240,404,501,760,412,702,703,415,119,22,23,122,25,788,529,439,526,672,699,529,668,122,699,126,627,940,127,802,676,529,119,44,119,122,458,905,460,886,51,127,710,908,909,131,57,623,911,912,122,907,106,107,65,910,67,122,130,70,114,637,127,119,640,487,122,106,79,122,669,82,595,913,914,915,916,5,6,501,481,482,93,853,485,122,487,22,23,122,127,941,103,104,127,496,123,108,611,122,119,112,857,122,127,860,113,529,665,120,121,122,123,886,917,918,122,630,919,920,119,127,122,122,617,665,173,127,175,176,119,142,122,122,904,834,542,127,185,186,106,896,134,135,136,192,193,194,114,115,116,162,665,852,106,126,167,665,231,170,126,172,114,115,116,122,213,127,665,407,127,119,685,681,929,930,249,685,933,934,122,936,877,106,122,127,126,685,106,595,122,114,115,116,267,127,122,115,38,117,825,127,126,126,780,122,122,218,126,132,127,134,135,136,225,121,134,135,136,123,713,116,921,922,923,119,872,126,122,833,985,91,725,988,757,990,991,1007,249,119,478,479,122,665,121,122,739,121,122,260,126,146,139,140,655,656,267,1012,269,126,959,272,126,685,89,90,106,107,108,109,110,111,112,126,115,115,323,881,289,17,18,19,126,330,126,106,126,940,126,129,126,106,126,134,115,899,853,901,119,114,115,116,85,86,145,126,138,140,1003,139,321,126,120,134,135,136,106,132,106,366,331,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,1006,126,853,121,122,947,745,853,125,904,121,106,127,757,752,753,427,121,853,114,115,116,761,762,121,764,113,766,261,123,944,126,121,56,120,130,447,120,126,134,135,136,126,619,126,106,785,119,119,228,130,230,126,904,106,107,108,999,904,126,471,121,137,412,123,106,415,130,133,904,93,94,95,96,97,98,99,100,101,102,127,314,106,422,423,424,265,472,127,117,114,115,116,853,833,121,120,126,447,121,123,450,121,132,125,134,135,136,125,458,343,460,1007,346,347,348,349,122,351,92,859,127,471,862,863,106,124,360,477,106,127,364,122,114,115,116,117,114,115,116,127,120,122,121,904,126,106,122,119,126,126,125,385,134,135,136,1007,134,135,136,899,1007,106,127,397,119,122,906,130,402,120,115,1007,126,30,560,119,42,126,137,529,127,126,122,133,924,127,127,106,127,134,135,136,542,122,428,122,122,127,938,127,121,63,64,65,127,127,122,947,121,127,113,623,74,75,76,77,78,127,127,133,127,127,126,106,121,637,126,120,640,8,9,10,11,12,13,14,15,16,17,18,19,20,21,119,119,595,1007,427,127,121,120,125,120,124,25,641,133,126,37,119,127,495,127,127,498,119,127,447,127,127,49,127,127,623,454,120,127,121,57,58,133,130,119,38,113,127,126,637,122,68,640,127,127,72,127,107,127,647,121,28,79,80,120,130,126,91,138,91,121,25,127,122,121,699,121,549,122,121,498,119,107,0,127,126,710,121,127,122,508,121,127,0,944,127,113,685,91,517,117,127,622,114,127,35,70,260,640,48,103,128,129,106,107,265,681,110,111,112,940,593,115,159,676,672,332,120,542,370,415,757,353,126,1003,128,129,225,131,132,- 1,134,614,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,628,- 1,25,26,- 1,- 1,- 1,- 1,- 1,32,- 1,- 1,- 1,757,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,651,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,780,60,- 1,613,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,623,73,831,- 1,627,834,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,693,- 1,- 1,696,- 1,698,- 1,852,- 1,- 1,- 1,- 1,- 1,706,- 1,- 1,709,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,38,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,905,- 1,907,908,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,- 1,- 1,- 1,91,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,788,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,802,749,120,- 1,- 1,959,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,944,- 1,- 1,91,778,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,1006,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,810,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,825,140,- 1,- 1,- 1,- 1,- 1,886,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,- 1,72,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,69,134,- 1,72,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,- 1,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,25,26,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,91,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,106,107,- 1,- 1,110,111,112,68,- 1,115,- 1,72,- 1,- 1,120,- 1,- 1,- 1,79,80,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,129,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,91,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,106,107,60,- 1,110,111,112,- 1,- 1,115,- 1,- 1,70,71,120,73,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,83,84,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,70,71,- 1,73,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,25,26,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,38,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,70,71,- 1,73,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,83,84,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple25{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple26{struct _dyneither_ptr f1;void*f2;};static char _tmp4DD[8]="stdcall";static char _tmp4DE[6]="cdecl";static char _tmp4DF[9]="fastcall";static char _tmp4E0[9]="noreturn";static char _tmp4E1[6]="const";static char _tmp4E2[8]="aligned";static char _tmp4E3[7]="packed";static char _tmp4E4[7]="shared";static char _tmp4E5[7]="unused";static char _tmp4E6[5]="weak";static char _tmp4E7[10]="dllimport";static char _tmp4E8[10]="dllexport";static char _tmp4E9[23]="no_instrument_function";static char _tmp4EA[12]="constructor";static char _tmp4EB[11]="destructor";static char _tmp4EC[22]="no_check_memory_usage";static char _tmp4ED[5]="pure";struct _tuple27{void*f1;void*f2;};struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp99A(union Cyc_YYSTYPE*yylval,unsigned int*_tmp999,unsigned int*_tmp998,union Cyc_YYSTYPE**_tmp997){for(*_tmp999=0;*_tmp999 < *_tmp998;(*_tmp999)++){(*_tmp997)[*_tmp999]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp99F(unsigned int*_tmp99E,unsigned int*_tmp99D,struct Cyc_Yyltype**_tmp99C){for(*_tmp99E=0;*_tmp99E < *_tmp99D;(*_tmp99E)++){(*_tmp99C)[*_tmp99E]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9A6(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9A5,unsigned int*_tmp9A4,short**_tmp9A2){for(*_tmp9A5=0;*_tmp9A5 < *_tmp9A4;(*_tmp9A5)++){(*_tmp9A2)[*_tmp9A5]=(short)(
# 216
*_tmp9A5 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9A5)): 0);}}static void _tmp9AC(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9AB,unsigned int*_tmp9AA,union Cyc_YYSTYPE**_tmp9A8){for(*_tmp9AB=0;*_tmp9AB < *_tmp9AA;(*_tmp9AB)++){(*_tmp9A8)[*_tmp9AB]=(union Cyc_YYSTYPE)(
# 219
*_tmp9AB <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9AB)):*yylval);}}static void _tmp9B2(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9B1,unsigned int*_tmp9B0,struct Cyc_Yyltype**_tmp9AE){for(*_tmp9B1=0;*_tmp9B1 < *_tmp9B0;(*_tmp9B1)++){(*_tmp9AE)[*_tmp9B1]=(struct Cyc_Yyltype)(
# 222
*_tmp9B1 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9B1)):
 Cyc_yynewloc());}}static void _tmpE8F(unsigned int*_tmpE8E,unsigned int*_tmpE8D,char**_tmpE8B){for(*_tmpE8E=0;*_tmpE8E < *_tmpE8D;(*_tmpE8E)++){(*_tmpE8B)[*_tmpE8E]=(char)'\000';}}
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
union Cyc_YYSTYPE _tmp993;union Cyc_YYSTYPE yylval=((_tmp993.YYINITIALSVAL).val=0,(((_tmp993.YYINITIALSVAL).tag=62,_tmp993)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp995;unsigned int _tmp994;struct _dyneither_ptr yyss=(_tmp994=200,((_tmp995=_region_calloc(yyregion,sizeof(short),_tmp994),_tag_dyneither(_tmp995,sizeof(short),_tmp994))));
# 158
int yyvsp_offset;
unsigned int _tmp999;unsigned int _tmp998;union Cyc_YYSTYPE*_tmp997;unsigned int _tmp996;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp996=(unsigned int)200,((_tmp997=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp996)),((((_tmp998=_tmp996,_tmp99A(& yylval,& _tmp999,& _tmp998,& _tmp997))),_tmp997)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp99E;unsigned int _tmp99D;struct Cyc_Yyltype*_tmp99C;unsigned int _tmp99B;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp99B=(unsigned int)200,((_tmp99C=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp99B)),((((_tmp99D=_tmp99B,_tmp99F(& _tmp99E,& _tmp99D,& _tmp99C))),_tmp99C)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9A0;Cyc_yyerror(((_tmp9A0="parser stack overflow",_tag_dyneither(_tmp9A0,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9A5;unsigned int _tmp9A4;struct _dyneither_ptr _tmp9A3;short*_tmp9A2;unsigned int _tmp9A1;struct _dyneither_ptr yyss1=(_tmp9A1=(unsigned int)yystacksize,((_tmp9A2=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9A1)),((_tmp9A3=_tag_dyneither(_tmp9A2,sizeof(short),_tmp9A1),((((_tmp9A4=_tmp9A1,_tmp9A6(& yyssp_offset,& yyss,& _tmp9A5,& _tmp9A4,& _tmp9A2))),_tmp9A3)))))));
# 217
unsigned int _tmp9AB;unsigned int _tmp9AA;struct _dyneither_ptr _tmp9A9;union Cyc_YYSTYPE*_tmp9A8;unsigned int _tmp9A7;struct _dyneither_ptr yyvs1=
(_tmp9A7=(unsigned int)yystacksize,((_tmp9A8=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A7)),((_tmp9A9=_tag_dyneither(_tmp9A8,sizeof(union Cyc_YYSTYPE),_tmp9A7),((((_tmp9AA=_tmp9A7,_tmp9AC(& yyvs,& yyssp_offset,& yylval,& _tmp9AB,& _tmp9AA,& _tmp9A8))),_tmp9A9)))))));
# 221
unsigned int _tmp9B1;unsigned int _tmp9B0;struct _dyneither_ptr _tmp9AF;struct Cyc_Yyltype*_tmp9AE;unsigned int _tmp9AD;struct _dyneither_ptr yyls1=(_tmp9AD=(unsigned int)yystacksize,((_tmp9AE=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9AD)),((_tmp9AF=_tag_dyneither(_tmp9AE,sizeof(struct Cyc_Yyltype),_tmp9AD),((((_tmp9B0=_tmp9AD,_tmp9B2(& yyssp_offset,& yyls,& _tmp9B1,& _tmp9B0,& _tmp9AE))),_tmp9AF)))))));
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
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 290;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6494) || Cyc_yycheck[_check_known_subscript_notnull(6495,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6495,yyn)];
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1030,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(518,yyn)];
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
{struct Cyc_Absyn_Decl*_tmp9BC;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9BB;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9BA;struct Cyc_List_List*_tmp9B9;yyval=Cyc_YY19(((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->hd=((_tmp9BC=_cycalloc(sizeof(*_tmp9BC)),((_tmp9BC->r=(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=11,((_tmp9BB.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9BB.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9BB)))))),_tmp9BA)))),((_tmp9BC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9BC)))))),((_tmp9B9->tl=0,_tmp9B9)))))));}
Cyc_Lex_leave_using();
# 1205
break;}case 4: _LL217: {
# 1202 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1206
{struct Cyc_Absyn_Decl*_tmp9C6;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9C5;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9C4;struct Cyc_List_List*_tmp9C3;yyval=Cyc_YY19(((_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3->hd=((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->r=(void*)((_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=11,((_tmp9C5.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9C5.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9C5)))))),_tmp9C4)))),((_tmp9C6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9C6)))))),((_tmp9C3->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9C3)))))));}
break;}case 5: _LL218: {
# 1205 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1209
{struct Cyc_Absyn_Decl*_tmp9D5;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9D4;struct _dyneither_ptr*_tmp9D3;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9D2;struct Cyc_List_List*_tmp9D1;yyval=Cyc_YY19(((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1->hd=((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->r=(void*)((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D4.tag=10,((_tmp9D4.f1=((_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9D3)))),((_tmp9D4.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D4)))))),_tmp9D2)))),((_tmp9D5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D5)))))),((_tmp9D1->tl=0,_tmp9D1)))))));}
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
{const char*_tmp9D6;nspace=((_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1)));}goto _LL21B;_LL21E: _tmp455=_tmp453;_LL21F:(void)_throw(_tmp455);_LL21B:;}};}
# 1219
{struct _handler_cons _tmp457;_push_handler(& _tmp457);{int _tmp459=0;if(setjmp(_tmp457.handler))_tmp459=1;if(!_tmp459){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp458=(void*)_exn_thrown;void*_tmp45B=_tmp458;void*_tmp45D;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp45C=(struct Cyc_Core_Failure_exn_struct*)_tmp45B;if(_tmp45C->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
 x=0;goto _LL220;_LL223: _tmp45D=_tmp45B;_LL224:(void)_throw(_tmp45D);_LL220:;}};}
# 1222
{struct _handler_cons _tmp45E;_push_handler(& _tmp45E);{int _tmp460=0;if(setjmp(_tmp45E.handler))_tmp460=1;if(!_tmp460){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp45F=(void*)_exn_thrown;void*_tmp462=_tmp45F;void*_tmp464;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp463=(struct Cyc_Core_Failure_exn_struct*)_tmp462;if(_tmp463->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 y=0;goto _LL225;_LL228: _tmp464=_tmp462;_LL229:(void)_throw(_tmp464);_LL225:;}};}
# 1225
{struct Cyc_Absyn_Decl*_tmp9E5;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9E4;struct _dyneither_ptr*_tmp9E3;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E2;struct Cyc_List_List*_tmp9E1;yyval=Cyc_YY19(((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1->hd=((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5->r=(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E4.tag=10,((_tmp9E4.f1=((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=nspace,_tmp9E3)))),((_tmp9E4.f2=x,_tmp9E4)))))),_tmp9E2)))),((_tmp9E5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E5)))))),((_tmp9E1->tl=y,_tmp9E1)))))));}
# 1227
break;}case 7: _LL21A: {
# 1224 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1228
struct _dyneither_ptr _tmp46A=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9E6;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46A,((_tmp9E6="C",_tag_dyneither(_tmp9E6,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmp9F0;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmp9EF;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9EE;struct Cyc_List_List*_tmp9ED;yyval=Cyc_YY19(((_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED->hd=((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0->r=(void*)((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=12,((_tmp9EF.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmp9EF)))),_tmp9EE)))),((_tmp9F0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F0)))))),((_tmp9ED->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9ED)))))));}else{
# 1232
{const char*_tmp9F1;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46A,((_tmp9F1="C include",_tag_dyneither(_tmp9F1,sizeof(char),10))))!= 0){
const char*_tmp9F2;Cyc_Parse_err(((_tmp9F2="expecting \"C\" or \"C include\"",_tag_dyneither(_tmp9F2,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1235
struct Cyc_Absyn_Decl*_tmp9FC;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmp9FB;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9FA;struct Cyc_List_List*_tmp9F9;yyval=Cyc_YY19(((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9->hd=((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC->r=(void*)((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=13,((_tmp9FB.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmp9FB.f2=0,_tmp9FB)))))),_tmp9FA)))),((_tmp9FC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9FC)))))),((_tmp9F9->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9F9)))))));};}}
# 1238
break;}case 8: _LL22A: {
# 1235 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1239
{const char*_tmp9FD;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmp9FD="C include",_tag_dyneither(_tmp9FD,sizeof(char),10))))!= 0){
const char*_tmp9FE;Cyc_Parse_err(((_tmp9FE="expecting \"C include\"",_tag_dyneither(_tmp9FE,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY53(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA08;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA07;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA06;struct Cyc_List_List*_tmpA05;yyval=Cyc_YY19(((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->hd=((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08->r=(void*)((_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06[0]=((_tmpA07.tag=13,((_tmpA07.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA07.f2=exs,_tmpA07)))))),_tmpA06)))),((_tmpA08->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA08)))))),((_tmpA05->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA05)))))));}
# 1244
break;};}case 9: _LL22B: {
# 1241 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1245
{struct Cyc_Absyn_Decl*_tmpA0B;struct Cyc_List_List*_tmpA0A;yyval=Cyc_YY19(((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->hd=((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA0B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0B)))))),((_tmpA0A->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA0A)))))));}
break;}case 10: _LL22C: {
# 1243 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1247
{struct Cyc_Absyn_Decl*_tmpA0E;struct Cyc_List_List*_tmpA0D;yyval=Cyc_YY19(((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->hd=((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA0E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0E)))))),((_tmpA0D->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA0D)))))));}
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
{struct _tuple25*_tmpA11;struct Cyc_List_List*_tmpA10;yyval=Cyc_YY53(((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA11->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA11->f3=0,_tmpA11)))))))),((_tmpA10->tl=0,_tmpA10)))))));}
break;}case 15: _LL231: {
# 1254 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1258
{struct _tuple25*_tmpA14;struct Cyc_List_List*_tmpA13;yyval=Cyc_YY53(((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->hd=((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA14->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA14->f3=0,_tmpA14)))))))),((_tmpA13->tl=0,_tmpA13)))))));}
break;}case 16: _LL232: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1260
{struct _tuple25*_tmpA17;struct Cyc_List_List*_tmpA16;yyval=Cyc_YY53(((_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16->hd=((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA17->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA17->f3=0,_tmpA17)))))))),((_tmpA16->tl=Cyc_yyget_YY53(yyyyvsp[2]),_tmpA16)))))));}
break;}case 17: _LL233: {
# 1260 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1264
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA1D;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA1C;struct Cyc_List_List*_tmpA1B;yyval=Cyc_YY19(((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->hd=Cyc_Absyn_new_decl((void*)((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1C.tag=1,((_tmpA1C.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA1C)))),_tmpA1D)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA1B->tl=0,_tmpA1B)))))));}
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
{struct _dyneither_ptr*_tmpA1E;Cyc_Lex_enter_namespace(((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA1E)))));}yyval=yyyyvsp[1];
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
{struct Cyc_List_List*_tmpA1F;yyval=Cyc_YY19(((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA1F->tl=0,_tmpA1F)))))));}
break;}case 33: _LL243: {
# 1323 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1327
struct Cyc_List_List*_tmp490=0;
{struct Cyc_List_List*_tmp491=Cyc_yyget_YY37(yyyyvsp[1]);for(0;_tmp491 != 0;_tmp491=_tmp491->tl){
struct _dyneither_ptr*_tmp492=(struct _dyneither_ptr*)_tmp491->hd;
struct _tuple0*_tmpA20;struct _tuple0*qv=(_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->f1=Cyc_Absyn_Rel_n(0),((_tmpA20->f2=_tmp492,_tmpA20)))));
struct Cyc_Absyn_Vardecl*_tmp493=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA21;_tmp490=((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->hd=_tmp493,((_tmpA21->tl=_tmp490,_tmpA21))))));}}
# 1334
_tmp490=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp490);
{struct Cyc_List_List*_tmpA22;yyval=Cyc_YY19(((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->hd=Cyc_Absyn_letv_decl(_tmp490,(unsigned int)(yyyylsp[0]).first_line),((_tmpA22->tl=0,_tmpA22)))))));}
# 1337
break;}case 34: _LL244: {
# 1336 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1340
struct _dyneither_ptr _tmp497=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA23;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,((_tmpA23="`H",_tag_dyneither(_tmpA23,sizeof(char),3))))== 0){
const char*_tmpA26;void*_tmpA25;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA25=0,Cyc_aprintf(((_tmpA26="bad occurrence of heap region",_tag_dyneither(_tmpA26,sizeof(char),30))),_tag_dyneither(_tmpA25,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA27;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp497,((_tmpA27="`U",_tag_dyneither(_tmpA27,sizeof(char),3))))== 0){
const char*_tmpA2A;void*_tmpA29;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA29=0,Cyc_aprintf(((_tmpA2A="bad occurrence of unique region",_tag_dyneither(_tmpA2A,sizeof(char),32))),_tag_dyneither(_tmpA29,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA2D;struct Cyc_Absyn_Tvar*_tmpA2C;struct Cyc_Absyn_Tvar*tv=(_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->name=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=_tmp497,_tmpA2D)))),((_tmpA2C->identity=- 1,((_tmpA2C->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA2C)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA30;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA2F;void*t=(void*)((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=2,((_tmpA30.f1=tv,_tmpA30)))),_tmpA2F))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA36;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA35;struct _dyneither_ptr*_tmpA33;struct _tuple0*_tmpA32;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->f1=Cyc_Absyn_Loc_n,((_tmpA32->f2=((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA33)))),_tmpA32)))))),(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=15,((_tmpA36.f1=t,_tmpA36)))),_tmpA35)))),0);
{struct Cyc_List_List*_tmpA37;yyval=Cyc_YY19(((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY32(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA37->tl=0,_tmpA37)))))));}
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
{const char*_tmpA38;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A8,((_tmpA38="H",_tag_dyneither(_tmpA38,sizeof(char),2))))== 0){
const char*_tmpA39;Cyc_Parse_err(((_tmpA39="bad occurrence of heap region `H",_tag_dyneither(_tmpA39,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA3A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A8,((_tmpA3A="U",_tag_dyneither(_tmpA3A,sizeof(char),2))))== 0){
const char*_tmpA3B;Cyc_Parse_err(((_tmpA3B="bad occurrence of unique region `U",_tag_dyneither(_tmpA3B,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4A7  && _tmp4A9 != 0){
const char*_tmpA3C;Cyc_Parse_err(((_tmpA3C="open regions cannot be @resetable",_tag_dyneither(_tmpA3C,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA49;const char*_tmpA48;void*_tmpA47[1];struct Cyc_String_pa_PrintArg_struct _tmpA46;struct Cyc_Absyn_Tvar*_tmpA45;struct Cyc_Absyn_Tvar*tv=(_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->name=((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=(struct _dyneither_ptr)((_tmpA46.tag=0,((_tmpA46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4A8),((_tmpA47[0]=& _tmpA46,Cyc_aprintf(((_tmpA48="`%s",_tag_dyneither(_tmpA48,sizeof(char),4))),_tag_dyneither(_tmpA47,sizeof(void*),1)))))))),_tmpA49)))),((_tmpA45->identity=- 1,((_tmpA45->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA45)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA4C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4B;void*t=(void*)((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=2,((_tmpA4C.f1=tv,_tmpA4C)))),_tmpA4B))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA52;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA51;struct _dyneither_ptr*_tmpA4F;struct _tuple0*_tmpA4E;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->f1=Cyc_Absyn_Loc_n,((_tmpA4E->f2=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=_tmp4A8,_tmpA4F)))),_tmpA4E)))))),(void*)((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51[0]=((_tmpA52.tag=15,((_tmpA52.f1=t,_tmpA52)))),_tmpA51)))),0);
# 1366
{struct Cyc_List_List*_tmpA53;yyval=Cyc_YY19(((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4A7,_tmp4A9,(unsigned int)(yyyylsp[0]).first_line),((_tmpA53->tl=0,_tmpA53)))))));}
# 1368
break;};}case 36: _LL246: {
# 1366 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1370
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA5B;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA5A;struct _dyneither_ptr*_tmpA59;struct Cyc_Absyn_Tvar*_tmpA58;struct Cyc_Absyn_Tvar*tv=(_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->name=((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA59)))),((_tmpA58->identity=- 1,((_tmpA58->kind=(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5A.tag=0,((_tmpA5A.f1=& Cyc_Tcutil_rk,_tmpA5A)))),_tmpA5B)))),_tmpA58)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA5E;void*_tmpA5D;(_tmpA5D=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA5E="too many declarations in alias",_tag_dyneither(_tmpA5E,sizeof(char),31))),_tag_dyneither(_tmpA5D,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_tmp4BD=d->r;struct Cyc_Absyn_Vardecl*_tmp4BF;_LL249: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4BE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4BD;if(_tmp4BE->tag != 0)goto _LL24B;else{_tmp4BF=_tmp4BE->f1;}}_LL24A:
# 1377
{struct Cyc_List_List*_tmpA5F;yyval=Cyc_YY19(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=Cyc_Absyn_alias_decl(tv,_tmp4BF,(unsigned int)(yyyylsp[0]).first_line),((_tmpA5F->tl=0,_tmpA5F)))))));}
goto _LL248;_LL24B:;_LL24C:
# 1380
{const char*_tmpA60;Cyc_Parse_err(((_tmpA60="expecting variable declaration",_tag_dyneither(_tmpA60,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
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
{struct Cyc_Parse_Declaration_spec _tmpA61;yyval=Cyc_YY20(((_tmpA61.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA61.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA61.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA61.is_inline=0,((_tmpA61.attributes=0,_tmpA61)))))))))));}
break;}case 42: _LL251: {
# 1400 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1404
struct Cyc_Parse_Declaration_spec _tmp4C7=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4C7.sc != 0){
const char*_tmpA64;void*_tmpA63;(_tmpA63=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA64="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA64,sizeof(char),51))),_tag_dyneither(_tmpA63,sizeof(void*),0)));}
# 1408
{struct Cyc_Parse_Declaration_spec _tmpA65;yyval=Cyc_YY20(((_tmpA65.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA65.tq=_tmp4C7.tq,((_tmpA65.type_specs=_tmp4C7.type_specs,((_tmpA65.is_inline=_tmp4C7.is_inline,((_tmpA65.attributes=_tmp4C7.attributes,_tmpA65)))))))))));}
# 1412
break;}case 43: _LL252: {
# 1409 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1413
{struct Cyc_Parse_Declaration_spec _tmpA66;yyval=Cyc_YY20(((_tmpA66.sc=0,((_tmpA66.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA66.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA66.is_inline=0,((_tmpA66.attributes=0,_tmpA66)))))))))));}
break;}case 44: _LL253: {
# 1412 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1416
struct Cyc_Parse_Declaration_spec _tmp4CC=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA67;yyval=Cyc_YY20(((_tmpA67.sc=_tmp4CC.sc,((_tmpA67.tq=_tmp4CC.tq,((_tmpA67.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4CC.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA67.is_inline=_tmp4CC.is_inline,((_tmpA67.attributes=_tmp4CC.attributes,_tmpA67)))))))))));}
# 1422
break;}case 45: _LL254: {
# 1419 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1423
{struct Cyc_Parse_Declaration_spec _tmpA68;yyval=Cyc_YY20(((_tmpA68.sc=0,((_tmpA68.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA68.type_specs=Cyc_Parse_empty_spec(0),((_tmpA68.is_inline=0,((_tmpA68.attributes=0,_tmpA68)))))))))));}
break;}case 46: _LL255: {
# 1421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1425
struct Cyc_Parse_Declaration_spec _tmp4CF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA69;yyval=Cyc_YY20(((_tmpA69.sc=_tmp4CF.sc,((_tmpA69.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4CF.tq),((_tmpA69.type_specs=_tmp4CF.type_specs,((_tmpA69.is_inline=_tmp4CF.is_inline,((_tmpA69.attributes=_tmp4CF.attributes,_tmpA69)))))))))));}
# 1430
break;}case 47: _LL256: {
# 1427 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1431
{struct Cyc_Parse_Declaration_spec _tmpA6A;yyval=Cyc_YY20(((_tmpA6A.sc=0,((_tmpA6A.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA6A.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA6A.is_inline=1,((_tmpA6A.attributes=0,_tmpA6A)))))))))));}
break;}case 48: _LL257: {
# 1430 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1434
struct Cyc_Parse_Declaration_spec _tmp4D2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA6B;yyval=Cyc_YY20(((_tmpA6B.sc=_tmp4D2.sc,((_tmpA6B.tq=_tmp4D2.tq,((_tmpA6B.type_specs=_tmp4D2.type_specs,((_tmpA6B.is_inline=1,((_tmpA6B.attributes=_tmp4D2.attributes,_tmpA6B)))))))))));}
# 1438
break;}case 49: _LL258: {
# 1435 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1439
{struct Cyc_Parse_Declaration_spec _tmpA6C;yyval=Cyc_YY20(((_tmpA6C.sc=0,((_tmpA6C.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA6C.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA6C.is_inline=0,((_tmpA6C.attributes=Cyc_yyget_YY46(yyyyvsp[0]),_tmpA6C)))))))))));}
break;}case 50: _LL259: {
# 1438 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1442
struct Cyc_Parse_Declaration_spec _tmp4D5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA6D;yyval=Cyc_YY20(((_tmpA6D.sc=_tmp4D5.sc,((_tmpA6D.tq=_tmp4D5.tq,((_tmpA6D.type_specs=_tmp4D5.type_specs,((_tmpA6D.is_inline=_tmp4D5.is_inline,((_tmpA6D.attributes=
# 1445
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp4D5.attributes),_tmpA6D)))))))))));}
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
{const char*_tmpA6E;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA6E="C",_tag_dyneither(_tmpA6E,sizeof(char),2))))!= 0){
const char*_tmpA6F;Cyc_Parse_err(((_tmpA6F="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA6F,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_List_List*_tmpA70;yyval=Cyc_YY46(((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpA70->tl=0,_tmpA70)))))));}
break;}case 62: _LL265: {
# 1479 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1483
{struct Cyc_List_List*_tmpA71;yyval=Cyc_YY46(((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpA71->tl=Cyc_yyget_YY46(yyyyvsp[2]),_tmpA71)))))));}
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
{const char*_tmpA72;Cyc_Parse_err(((_tmpA72="unrecognized attribute",_tag_dyneither(_tmpA72,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpA74;const char*_tmpA73;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA74="regparm",_tag_dyneither(_tmpA74,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA73="__regparm__",_tag_dyneither(_tmpA73,sizeof(char),12))))== 0){
if(_tmp4F1 < 0  || _tmp4F1 > 3){
const char*_tmpA75;Cyc_Parse_err(((_tmpA75="regparm requires value between 0 and 3",_tag_dyneither(_tmpA75,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA78;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA77;a=(void*)((_tmpA77=_cycalloc_atomic(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=0,((_tmpA78.f1=_tmp4F1,_tmpA78)))),_tmpA77))));};}else{
const char*_tmpA7A;const char*_tmpA79;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA7A="aligned",_tag_dyneither(_tmpA7A,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA79="__aligned__",_tag_dyneither(_tmpA79,sizeof(char),12))))== 0){
if(_tmp4F1 < 0){const char*_tmpA7B;Cyc_Parse_err(((_tmpA7B="aligned requires positive power of two",_tag_dyneither(_tmpA7B,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4F1;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA7C;Cyc_Parse_err(((_tmpA7C="aligned requires positive power of two",_tag_dyneither(_tmpA7C,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA7F;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA7E;a=(void*)((_tmpA7E=_cycalloc_atomic(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=6,((_tmpA7F.f1=_tmp4F1,_tmpA7F)))),_tmpA7E))));};};}else{
const char*_tmpA81;const char*_tmpA80;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA81="initializes",_tag_dyneither(_tmpA81,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA80="__initializes__",_tag_dyneither(_tmpA80,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA84;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA83;a=(void*)((_tmpA83=_cycalloc_atomic(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=20,((_tmpA84.f1=_tmp4F1,_tmpA84)))),_tmpA83))));}else{
const char*_tmpA86;const char*_tmpA85;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA86="noliveunique",_tag_dyneither(_tmpA86,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA85="__noliveunique__",_tag_dyneither(_tmpA85,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA89;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA88;a=(void*)((_tmpA88=_cycalloc_atomic(sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA89.tag=21,((_tmpA89.f1=_tmp4F1,_tmpA89)))),_tmpA88))));}else{
const char*_tmpA8B;const char*_tmpA8A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA8B="noconsume",_tag_dyneither(_tmpA8B,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4EF,((_tmpA8A="__noconsume__",_tag_dyneither(_tmpA8A,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpA8E;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA8D;a=(void*)((_tmpA8D=_cycalloc_atomic(sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=22,((_tmpA8E.f1=_tmp4F1,_tmpA8E)))),_tmpA8D))));}else{
# 1548
{const char*_tmpA8F;Cyc_Parse_err(((_tmpA8F="unrecognized attribute",_tag_dyneither(_tmpA8F,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpA91;const char*_tmpA90;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50A,((_tmpA91="section",_tag_dyneither(_tmpA91,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50A,((_tmpA90="__section__",_tag_dyneither(_tmpA90,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpA94;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpA93;a=(void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=8,((_tmpA94.f1=_tmp50B,_tmpA94)))),_tmpA93))));}else{
# 1560
{const char*_tmpA95;Cyc_Parse_err(((_tmpA95="unrecognized attribute",_tag_dyneither(_tmpA95,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpA96;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp511,((_tmpA96="__mode__",_tag_dyneither(_tmpA96,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpA99;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpA98;a=(void*)((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA99.tag=24,((_tmpA99.f1=_tmp512,_tmpA99)))),_tmpA98))));}else{
# 1573
{const char*_tmpA9A;Cyc_Parse_err(((_tmpA9A="unrecognized attribute",_tag_dyneither(_tmpA9A,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
{const char*_tmpA9C;const char*_tmpA9B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpA9C="format",_tag_dyneither(_tmpA9C,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp517,((_tmpA9B="__format__",_tag_dyneither(_tmpA9B,sizeof(char),11))))== 0){
const char*_tmpA9D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,((_tmpA9D="printf",_tag_dyneither(_tmpA9D,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAA0;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA9F;a=(void*)((_tmpA9F=_cycalloc_atomic(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=19,((_tmpAA0.f1=Cyc_Absyn_Printf_ft,((_tmpAA0.f2=_tmp51A,((_tmpAA0.f3=_tmp51C,_tmpAA0)))))))),_tmpA9F))));}else{
const char*_tmpAA1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp518,((_tmpAA1="scanf",_tag_dyneither(_tmpAA1,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAA4;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAA3;a=(void*)((_tmpAA3=_cycalloc_atomic(sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=19,((_tmpAA4.f1=Cyc_Absyn_Scanf_ft,((_tmpAA4.f2=_tmp51A,((_tmpAA4.f3=_tmp51C,_tmpAA4)))))))),_tmpAA3))));}else{
# 1590
const char*_tmpAA5;Cyc_Parse_err(((_tmpAA5="unrecognized format type",_tag_dyneither(_tmpAA5,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1592
const char*_tmpAA6;Cyc_Parse_err(((_tmpAA6="unrecognized attribute",_tag_dyneither(_tmpAA6,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAA9;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAA8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=17,((_tmpAA9.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAA9.f2=Cyc_yyget_YY41(yyyyvsp[1]),((_tmpAA9.f3=0,((_tmpAA9.f4=0,_tmpAA9)))))))))),_tmpAA8)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAAC;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAAB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAC.tag=10,((_tmpAAC.f1=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[2]))),_tmpAAC)))),_tmpAAB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1633
break;}case 87: _LL27E: {
# 1630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1634
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAAF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAAE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=15,((_tmpAAF.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpAAF)))),_tmpAAE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL27F: {
# 1632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1636
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAB2;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAB1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=15,((_tmpAB2.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAB2)))),_tmpAB1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1638
break;}case 89: _LL280: {
# 1635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1639
void*_tmp52F=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAB5;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=16,((_tmpAB5.f1=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpAB5.f2=_tmp52F,_tmpAB5)))))),_tmpAB4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1642
break;}case 90: _LL281: {
# 1639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1643
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAB8;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=((_tmpAB8.tag=16,((_tmpAB8.f1=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpAB8.f2=Cyc_yyget_YY45(yyyyvsp[4]),_tmpAB8)))))),_tmpAB7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL282: {
# 1641 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1645
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpABB;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpABA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=19,((_tmpABB.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpABB)))),_tmpABA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL283: {
# 1643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1647
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpABE;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpABD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=19,((_tmpABE.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpABE)))),_tmpABD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL284: {
# 1645 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1649
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAC1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAC0;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=18,((_tmpAC1.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAC1)))),_tmpAC0)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_Tqual _tmpAC2;yyval=Cyc_YY26(((_tmpAC2.print_const=1,((_tmpAC2.q_volatile=0,((_tmpAC2.q_restrict=0,((_tmpAC2.real_const=1,((_tmpAC2.loc=loc,_tmpAC2)))))))))));}
break;}case 96: _LL287: {
# 1656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660
{struct Cyc_Absyn_Tqual _tmpAC3;yyval=Cyc_YY26(((_tmpAC3.print_const=0,((_tmpAC3.q_volatile=1,((_tmpAC3.q_restrict=0,((_tmpAC3.real_const=0,((_tmpAC3.loc=0,_tmpAC3)))))))))));}
break;}case 97: _LL288: {
# 1657 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1661
{struct Cyc_Absyn_Tqual _tmpAC4;yyval=Cyc_YY26(((_tmpAC4.print_const=0,((_tmpAC4.q_volatile=0,((_tmpAC4.q_restrict=1,((_tmpAC4.real_const=0,((_tmpAC4.loc=0,_tmpAC4)))))))))));}
break;}case 98: _LL289: {
# 1663 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1667
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAD3;struct Cyc_Absyn_Enumdecl*_tmpAD2;struct Cyc_Core_Opt*_tmpAD1;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAD0;struct Cyc_Absyn_TypeDecl*_tmpACF;struct Cyc_Absyn_TypeDecl*_tmp53D=(_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->r=(void*)((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3[0]=((_tmpAD0.tag=1,((_tmpAD0.f1=((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->sc=Cyc_Absyn_Public,((_tmpAD2->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAD2->fields=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->v=Cyc_yyget_YY49(yyyyvsp[3]),_tmpAD1)))),_tmpAD2)))))))),_tmpAD0)))),_tmpAD3)))),((_tmpACF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpACF)))));
# 1669
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAD6;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAD5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD6.tag=26,((_tmpAD6.f1=_tmp53D,((_tmpAD6.f2=0,_tmpAD6)))))),_tmpAD5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1671
break;}case 99: _LL28A: {
# 1668 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1672
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAD9;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAD8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=((_tmpAD9.tag=13,((_tmpAD9.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAD9.f2=0,_tmpAD9)))))),_tmpAD8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL28B: {
# 1670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1674
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpADC;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpADB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB[0]=((_tmpADC.tag=14,((_tmpADC.f1=Cyc_yyget_YY49(yyyyvsp[2]),_tmpADC)))),_tmpADB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL28C: {
# 1676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1680
{struct Cyc_Absyn_Enumfield*_tmpADD;yyval=Cyc_YY48(((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpADD->tag=0,((_tmpADD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpADD)))))))));}
break;}case 102: _LL28D: {
# 1678 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1682
{struct Cyc_Absyn_Enumfield*_tmpADE;yyval=Cyc_YY48(((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpADE->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpADE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpADE)))))))));}
break;}case 103: _LL28E: {
# 1682 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1686
{struct Cyc_List_List*_tmpADF;yyval=Cyc_YY49(((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpADF->tl=0,_tmpADF)))))));}
break;}case 104: _LL28F: {
# 1683 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1687
{struct Cyc_List_List*_tmpAE0;yyval=Cyc_YY49(((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpAE0->tl=0,_tmpAE0)))))));}
break;}case 105: _LL290: {
# 1684 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1688
{struct Cyc_List_List*_tmpAE1;yyval=Cyc_YY49(((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->hd=Cyc_yyget_YY48(yyyyvsp[0]),((_tmpAE1->tl=Cyc_yyget_YY49(yyyyvsp[2]),_tmpAE1)))))));}
break;}case 106: _LL291: {
# 1690 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1694
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAE4;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE3;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=((_tmpAE4.tag=12,((_tmpAE4.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAE4.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAE4)))))),_tmpAE3)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE7;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE7.tag=26,((_tmpAE7.f1=_tmp552,((_tmpAE7.f2=0,_tmpAE7)))))),_tmpAE6)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAEA;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE9;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9[0]=((_tmpAEA.tag=26,((_tmpAEA.f1=_tmp557,((_tmpAEA.f2=0,_tmpAEA)))))),_tmpAE9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1718
break;}case 109: _LL294: {
# 1715 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1719
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAF4;struct Cyc_Core_Opt*_tmpAF3;struct Cyc_Absyn_AggrInfo _tmpAF2;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAF1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF4.tag=11,((_tmpAF4.f1=((_tmpAF2.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpAF3=_cycalloc_atomic(sizeof(*_tmpAF3)),((_tmpAF3->v=(void*)1,_tmpAF3))))),((_tmpAF2.targs=Cyc_yyget_YY41(yyyyvsp[3]),_tmpAF2)))),_tmpAF4)))),_tmpAF1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1722
break;}case 110: _LL295: {
# 1719 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1723
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpAFA;struct Cyc_Absyn_AggrInfo _tmpAF9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAF8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8[0]=((_tmpAFA.tag=11,((_tmpAFA.f1=((_tmpAF9.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpAF9.targs=Cyc_yyget_YY41(yyyyvsp[2]),_tmpAF9)))),_tmpAFA)))),_tmpAF8)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpAFB;yyval=Cyc_YY28(((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpAFB->tl=0,_tmpAFB)))))));}
break;}case 118: _LL29D: {
# 1756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1760
{struct Cyc_List_List*_tmpAFC;yyval=Cyc_YY28(((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpAFC->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpAFC)))))));}
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
{struct _tuple12*_tmpAFD;yyval=Cyc_YY22(((_tmpAFD=_region_malloc(yyr,sizeof(*_tmpAFD)),((_tmpAFD->tl=0,((_tmpAFD->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpAFD)))))));}
break;}case 121: _LL2A0: {
# 1768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1772
{struct _tuple12*_tmpAFE;yyval=Cyc_YY22(((_tmpAFE=_region_malloc(yyr,sizeof(*_tmpAFE)),((_tmpAFE->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpAFE->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpAFE)))))));}
break;}case 122: _LL2A1: {
# 1773 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1777
{struct _tuple11 _tmpAFF;yyval=Cyc_YY21(((_tmpAFF.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpAFF.f2=0,_tmpAFF)))));}
break;}case 123: _LL2A2: {
# 1775 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1779
{struct _tuple11 _tmpB00;yyval=Cyc_YY21(((_tmpB00.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB00.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB00)))));}
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
{struct _tuple23 _tmpB01;yyval=Cyc_YY36(((_tmpB01.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB01.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB01.f3=0,_tmpB01)))))));}
break;}case 126: _LL2A5: {
# 1810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814
struct _tuple23 _tmp571=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB02;yyval=Cyc_YY36(((_tmpB02.f1=_tmp571.f1,((_tmpB02.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp571.f2),((_tmpB02.f3=_tmp571.f3,_tmpB02)))))));}
break;}case 127: _LL2A6: {
# 1812 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1816
{struct _tuple23 _tmpB03;yyval=Cyc_YY36(((_tmpB03.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB03.f2=Cyc_Parse_empty_spec(0),((_tmpB03.f3=0,_tmpB03)))))));}
break;}case 128: _LL2A7: {
# 1814 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1818
struct _tuple23 _tmp574=Cyc_yyget_YY36(yyyyvsp[1]);
{struct _tuple23 _tmpB04;yyval=Cyc_YY36(((_tmpB04.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp574.f1),((_tmpB04.f2=_tmp574.f2,((_tmpB04.f3=_tmp574.f3,_tmpB04)))))));}
break;}case 129: _LL2A8: {
# 1817 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1821
{struct _tuple23 _tmpB05;yyval=Cyc_YY36(((_tmpB05.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB05.f2=Cyc_Parse_empty_spec(0),((_tmpB05.f3=Cyc_yyget_YY46(yyyyvsp[0]),_tmpB05)))))));}
break;}case 130: _LL2A9: {
# 1819 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1823
struct _tuple23 _tmp577=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB06;yyval=Cyc_YY36(((_tmpB06.f1=_tmp577.f1,((_tmpB06.f2=_tmp577.f2,((_tmpB06.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp577.f3),_tmpB06)))))));}
break;}case 131: _LL2AA: {
# 1825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829
{struct _tuple23 _tmpB07;yyval=Cyc_YY36(((_tmpB07.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB07.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB07.f3=0,_tmpB07)))))));}
break;}case 132: _LL2AB: {
# 1827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831
struct _tuple23 _tmp57A=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB08;yyval=Cyc_YY36(((_tmpB08.f1=_tmp57A.f1,((_tmpB08.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp57A.f2),((_tmpB08.f3=_tmp57A.f3,_tmpB08)))))));}
break;}case 133: _LL2AC: {
# 1829 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1833
{struct _tuple23 _tmpB09;yyval=Cyc_YY36(((_tmpB09.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB09.f2=Cyc_Parse_empty_spec(0),((_tmpB09.f3=0,_tmpB09)))))));}
break;}case 134: _LL2AD: {
# 1831 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1835
struct _tuple23 _tmp57D=Cyc_yyget_YY36(yyyyvsp[1]);
{struct _tuple23 _tmpB0A;yyval=Cyc_YY36(((_tmpB0A.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp57D.f1),((_tmpB0A.f2=_tmp57D.f2,((_tmpB0A.f3=_tmp57D.f3,_tmpB0A)))))));}
break;}case 135: _LL2AE: {
# 1834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1838
{struct _tuple23 _tmpB0B;yyval=Cyc_YY36(((_tmpB0B.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB0B.f2=Cyc_Parse_empty_spec(0),((_tmpB0B.f3=Cyc_yyget_YY46(yyyyvsp[0]),_tmpB0B)))))));}
break;}case 136: _LL2AF: {
# 1836 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1840
struct _tuple23 _tmp580=Cyc_yyget_YY36(yyyyvsp[1]);{struct _tuple23 _tmpB0C;yyval=Cyc_YY36(((_tmpB0C.f1=_tmp580.f1,((_tmpB0C.f2=_tmp580.f2,((_tmpB0C.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY46(yyyyvsp[0]),_tmp580.f3),_tmpB0C)))))));}
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
{struct _tuple12*_tmpB0D;yyval=Cyc_YY22(((_tmpB0D=_region_malloc(yyr,sizeof(*_tmpB0D)),((_tmpB0D->tl=0,((_tmpB0D->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB0D)))))));}
break;}case 139: _LL2B2: {
# 1848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1852
{struct _tuple12*_tmpB0E;yyval=Cyc_YY22(((_tmpB0E=_region_malloc(yyr,sizeof(*_tmpB0E)),((_tmpB0E->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB0E->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB0E)))))));}
break;}case 140: _LL2B3: {
# 1853 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1857
{struct _tuple11 _tmpB0F;yyval=Cyc_YY21(((_tmpB0F.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB0F.f2=0,_tmpB0F)))));}
break;}case 141: _LL2B4: {
# 1855 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1861
{struct Cyc_Parse_Declarator _tmpB15;struct _tuple0*_tmpB14;struct _tuple11 _tmpB13;yyval=Cyc_YY21(((_tmpB13.f1=((_tmpB15.id=((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->f1=Cyc_Absyn_Rel_n(0),((_tmpB14->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB14)))))),((_tmpB15.tms=0,_tmpB15)))),((_tmpB13.f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpB13)))));}
# 1863
break;}case 142: _LL2B5:
# 1862 "parse.y"
{struct Cyc_Parse_Declarator _tmpB1B;struct _tuple0*_tmpB1A;struct _tuple11 _tmpB19;yyval=Cyc_YY21(((_tmpB19.f1=((_tmpB1B.id=((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->f1=Cyc_Absyn_Rel_n(0),((_tmpB1A->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB1A)))))),((_tmpB1B.tms=0,_tmpB1B)))),((_tmpB19.f2=0,_tmpB19)))));}
# 1864
break;case 143: _LL2B6: {
# 1865 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1869
{struct _tuple11 _tmpB1C;yyval=Cyc_YY21(((_tmpB1C.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB1C.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB1C)))));}
break;}case 144: _LL2B7: {
# 1871 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1875
int _tmp58E=Cyc_yyget_YY32(yyyyvsp[0]);
struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY41(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB1D;struct Cyc_Absyn_TypeDecl*_tmp590=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp58F,((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->v=Cyc_yyget_YY35(yyyyvsp[4]),_tmpB1D)))),_tmp58E,(unsigned int)(yyyylsp[0]).first_line);
# 1879
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB20;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=26,((_tmpB20.f1=_tmp590,((_tmpB20.f2=0,_tmpB20)))))),_tmpB1F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1881
break;}case 145: _LL2B8: {
# 1878 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1882
int _tmp594=Cyc_yyget_YY32(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB2A;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB29;struct Cyc_Absyn_DatatypeInfo _tmpB28;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB27;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB2A.tag=3,((_tmpB2A.f1=((_tmpB28.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB29.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB29.is_extensible=_tmp594,_tmpB29))))),((_tmpB28.targs=Cyc_yyget_YY41(yyyyvsp[2]),_tmpB28)))),_tmpB2A)))),_tmpB27)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1885
break;}case 146: _LL2B9: {
# 1882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1886
int _tmp599=Cyc_yyget_YY32(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB34;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB33;struct Cyc_Absyn_DatatypeFieldInfo _tmpB32;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB31;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((_tmpB34.tag=4,((_tmpB34.f1=((_tmpB32.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB33.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB33.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB33.is_extensible=_tmp599,_tmpB33))))))),((_tmpB32.targs=Cyc_yyget_YY41(yyyyvsp[4]),_tmpB32)))),_tmpB34)))),_tmpB31)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpB35;yyval=Cyc_YY35(((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB35->tl=0,_tmpB35)))))));}
break;}case 150: _LL2BD: {
# 1896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1900
{struct Cyc_List_List*_tmpB36;yyval=Cyc_YY35(((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB36->tl=0,_tmpB36)))))));}
break;}case 151: _LL2BE: {
# 1897 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1901
{struct Cyc_List_List*_tmpB37;yyval=Cyc_YY35(((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB37->tl=Cyc_yyget_YY35(yyyyvsp[2]),_tmpB37)))))));}
break;}case 152: _LL2BF: {
# 1898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1902
{struct Cyc_List_List*_tmpB38;yyval=Cyc_YY35(((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->hd=Cyc_yyget_YY34(yyyyvsp[0]),((_tmpB38->tl=Cyc_yyget_YY35(yyyyvsp[2]),_tmpB38)))))));}
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
{struct Cyc_Absyn_Datatypefield*_tmpB39;yyval=Cyc_YY34(((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB39->typs=0,((_tmpB39->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB39->sc=Cyc_yyget_YY33(yyyyvsp[0]),_tmpB39)))))))))));}
break;}case 157: _LL2C4: {
# 1910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1914
struct Cyc_List_List*_tmp5A3=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB3A;yyval=Cyc_YY34(((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB3A->typs=_tmp5A3,((_tmpB3A->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB3A->sc=Cyc_yyget_YY33(yyyyvsp[0]),_tmpB3A)))))))))));}
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
{struct Cyc_Parse_Declarator _tmpB3B;yyval=Cyc_YY30(((_tmpB3B.id=_tmp5A5.id,((_tmpB3B.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5A5.tms),_tmpB3B)))));}
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
{struct Cyc_Parse_Declarator _tmpB3C;yyval=Cyc_YY30(((_tmpB3C.id=_tmp5A7.id,((_tmpB3C.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5A7.tms),_tmpB3C)))));}
break;}case 162: _LL2C9: {
# 1933 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1937
{struct Cyc_Parse_Declarator _tmpB3D;yyval=Cyc_YY30(((_tmpB3D.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB3D.tms=0,_tmpB3D)))));}
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
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB43;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB42;struct Cyc_List_List*_tmpB41;_tmp5AA.tms=((_tmpB41=_region_malloc(yyr,sizeof(*_tmpB41)),((_tmpB41->hd=(void*)((_tmpB43=_region_malloc(yyr,sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB42.tag=5,((_tmpB42.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB42.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpB42)))))),_tmpB43)))),((_tmpB41->tl=_tmp5AA.tms,_tmpB41))))));}
yyval=yyyyvsp[2];
# 1947
break;}case 165: _LL2CC: {
# 1944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1948
{struct Cyc_List_List*_tmpB4D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB4C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB4B;struct Cyc_Parse_Declarator _tmpB4A;yyval=Cyc_YY30(((_tmpB4A.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB4A.tms=((_tmpB4D=_region_malloc(yyr,sizeof(*_tmpB4D)),((_tmpB4D->hd=(void*)((_tmpB4B=_region_malloc(yyr,sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=0,((_tmpB4C.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpB4C.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB4C)))))),_tmpB4B)))),((_tmpB4D->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB4D)))))),_tmpB4A)))));}
break;}case 166: _LL2CD: {
# 1946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1950
{struct Cyc_List_List*_tmpB57;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB56;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB55;struct Cyc_Parse_Declarator _tmpB54;yyval=Cyc_YY30(((_tmpB54.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB54.tms=(
(_tmpB57=_region_malloc(yyr,sizeof(*_tmpB57)),((_tmpB57->hd=(void*)((_tmpB55=_region_malloc(yyr,sizeof(*_tmpB55)),((_tmpB55[0]=((_tmpB56.tag=1,((_tmpB56.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB56.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpB56.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB56)))))))),_tmpB55)))),((_tmpB57->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB57)))))),_tmpB54)))));}
break;}case 167: _LL2CE: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953
struct Cyc_List_List*_tmp5B7;int _tmp5B8;struct Cyc_Absyn_VarargInfo*_tmp5B9;void*_tmp5BA;struct Cyc_List_List*_tmp5BB;struct _tuple24*_tmp5B6=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;_tmp5B9=_tmp5B6->f3;_tmp5BA=_tmp5B6->f4;_tmp5BB=_tmp5B6->f5;
{struct Cyc_List_List*_tmpB6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB6B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB6A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB69;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB68;struct Cyc_Parse_Declarator _tmpB67;yyval=Cyc_YY30(((_tmpB67.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB67.tms=((_tmpB6C=_region_malloc(yyr,sizeof(*_tmpB6C)),((_tmpB6C->hd=(void*)((_tmpB68=_region_malloc(yyr,sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB6B.tag=3,((_tmpB6B.f1=(void*)((_tmpB69=_region_malloc(yyr,sizeof(*_tmpB69)),((_tmpB69[0]=((_tmpB6A.tag=1,((_tmpB6A.f1=_tmp5B7,((_tmpB6A.f2=_tmp5B8,((_tmpB6A.f3=_tmp5B9,((_tmpB6A.f4=_tmp5BA,((_tmpB6A.f5=_tmp5BB,_tmpB6A)))))))))))),_tmpB69)))),_tmpB6B)))),_tmpB68)))),((_tmpB6C->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB6C)))))),_tmpB67)))));}
# 1956
break;}case 168: _LL2CF: {
# 1953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1957
{struct Cyc_List_List*_tmpB81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB80;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB7F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB7E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB7D;struct Cyc_Parse_Declarator _tmpB7C;yyval=Cyc_YY30(((_tmpB7C.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB7C.tms=(
(_tmpB81=_region_malloc(yyr,sizeof(*_tmpB81)),((_tmpB81->hd=(void*)((_tmpB7D=_region_malloc(yyr,sizeof(*_tmpB7D)),((_tmpB7D[0]=((_tmpB80.tag=3,((_tmpB80.f1=(void*)((_tmpB7E=_region_malloc(yyr,sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7F.tag=1,((_tmpB7F.f1=0,((_tmpB7F.f2=0,((_tmpB7F.f3=0,((_tmpB7F.f4=
# 1960
Cyc_yyget_YY50(yyyyvsp[2]),((_tmpB7F.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB7F)))))))))))),_tmpB7E)))),_tmpB80)))),_tmpB7D)))),((_tmpB81->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB81)))))),_tmpB7C)))));}
# 1963
break;}case 169: _LL2D0: {
# 1960 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1964
{struct Cyc_List_List*_tmpB96;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB95;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpB94;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB93;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB92;struct Cyc_Parse_Declarator _tmpB91;yyval=Cyc_YY30(((_tmpB91.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB91.tms=((_tmpB96=_region_malloc(yyr,sizeof(*_tmpB96)),((_tmpB96->hd=(void*)((_tmpB92=_region_malloc(yyr,sizeof(*_tmpB92)),((_tmpB92[0]=((_tmpB95.tag=3,((_tmpB95.f1=(void*)((_tmpB93=_region_malloc(yyr,sizeof(*_tmpB93)),((_tmpB93[0]=((_tmpB94.tag=0,((_tmpB94.f1=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpB94.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpB94)))))),_tmpB93)))),_tmpB95)))),_tmpB92)))),((_tmpB96->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB96)))))),_tmpB91)))));}
break;}case 170: _LL2D1: {
# 1963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1967
struct Cyc_List_List*_tmp5CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBA0;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpB9F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpB9E;struct Cyc_Parse_Declarator _tmpB9D;yyval=Cyc_YY30(((_tmpB9D.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB9D.tms=((_tmpBA0=_region_malloc(yyr,sizeof(*_tmpBA0)),((_tmpBA0->hd=(void*)((_tmpB9E=_region_malloc(yyr,sizeof(*_tmpB9E)),((_tmpB9E[0]=((_tmpB9F.tag=4,((_tmpB9F.f1=_tmp5CE,((_tmpB9F.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpB9F.f3=0,_tmpB9F)))))))),_tmpB9E)))),((_tmpBA0->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBA0)))))),_tmpB9D)))));}
# 1970
break;}case 171: _LL2D2: {
# 1967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1971
{struct Cyc_List_List*_tmpBAA;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBA9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBA8;struct Cyc_Parse_Declarator _tmpBA7;yyval=Cyc_YY30(((_tmpBA7.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA7.tms=((_tmpBAA=_region_malloc(yyr,sizeof(*_tmpBAA)),((_tmpBAA->hd=(void*)((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA9.tag=5,((_tmpBA9.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBA9.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpBA9)))))),_tmpBA8)))),((_tmpBAA->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBAA)))))),_tmpBA7)))));}
# 1974
break;}case 172: _LL2D3: {
# 1981 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1985
{struct Cyc_Parse_Declarator _tmpBAB;yyval=Cyc_YY30(((_tmpBAB.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBAB.tms=0,_tmpBAB)))));}
break;}case 173: _LL2D4: {
# 1983 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1987
{struct Cyc_Parse_Declarator _tmpBAC;yyval=Cyc_YY30(((_tmpBAC.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBAC.tms=0,_tmpBAC)))));}
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
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBB2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBB1;struct Cyc_List_List*_tmpBB0;_tmp5D9.tms=((_tmpBB0=_region_malloc(yyr,sizeof(*_tmpBB0)),((_tmpBB0->hd=(void*)((_tmpBB2=_region_malloc(yyr,sizeof(*_tmpBB2)),((_tmpBB2[0]=((_tmpBB1.tag=5,((_tmpBB1.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBB1.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpBB1)))))),_tmpBB2)))),((_tmpBB0->tl=_tmp5D9.tms,_tmpBB0))))));}
yyval=yyyyvsp[2];
# 1997
break;}case 176: _LL2D7: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1998
struct Cyc_Parse_Declarator _tmp5DD=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBBC;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBBB;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBBA;struct Cyc_Parse_Declarator _tmpBB9;yyval=Cyc_YY30(((_tmpBB9.id=_tmp5DD.id,((_tmpBB9.tms=(
(_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)((_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=0,((_tmpBBB.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpBBB.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBBB)))))),_tmpBBA)))),((_tmpBBC->tl=_tmp5DD.tms,_tmpBBC)))))),_tmpBB9)))));}
break;}case 177: _LL2D8: {
# 1998 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2002
struct Cyc_Parse_Declarator _tmp5E2=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBC6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBC5;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBC4;struct Cyc_Parse_Declarator _tmpBC3;yyval=Cyc_YY30(((_tmpBC3.id=_tmp5E2.id,((_tmpBC3.tms=(
(_tmpBC6=_region_malloc(yyr,sizeof(*_tmpBC6)),((_tmpBC6->hd=(void*)((_tmpBC4=_region_malloc(yyr,sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC5.tag=1,((_tmpBC5.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBC5.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpBC5.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBC5)))))))),_tmpBC4)))),((_tmpBC6->tl=_tmp5E2.tms,_tmpBC6)))))),_tmpBC3)))));}
# 2006
break;}case 178: _LL2D9: {
# 2003 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2007
struct Cyc_List_List*_tmp5E8;int _tmp5E9;struct Cyc_Absyn_VarargInfo*_tmp5EA;void*_tmp5EB;struct Cyc_List_List*_tmp5EC;struct _tuple24*_tmp5E7=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;_tmp5EA=_tmp5E7->f3;_tmp5EB=_tmp5E7->f4;_tmp5EC=_tmp5E7->f5;{
struct Cyc_Parse_Declarator _tmp5ED=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBDB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBDA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBD9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBD8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBD7;struct Cyc_Parse_Declarator _tmpBD6;yyval=Cyc_YY30(((_tmpBD6.id=_tmp5ED.id,((_tmpBD6.tms=((_tmpBDB=_region_malloc(yyr,sizeof(*_tmpBDB)),((_tmpBDB->hd=(void*)((_tmpBD7=_region_malloc(yyr,sizeof(*_tmpBD7)),((_tmpBD7[0]=((_tmpBDA.tag=3,((_tmpBDA.f1=(void*)((_tmpBD8=_region_malloc(yyr,sizeof(*_tmpBD8)),((_tmpBD8[0]=((_tmpBD9.tag=1,((_tmpBD9.f1=_tmp5E8,((_tmpBD9.f2=_tmp5E9,((_tmpBD9.f3=_tmp5EA,((_tmpBD9.f4=_tmp5EB,((_tmpBD9.f5=_tmp5EC,_tmpBD9)))))))))))),_tmpBD8)))),_tmpBDA)))),_tmpBD7)))),((_tmpBDB->tl=_tmp5ED.tms,_tmpBDB)))))),_tmpBD6)))));}
# 2011
break;};}case 179: _LL2DA: {
# 2008 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2012
struct Cyc_Parse_Declarator _tmp5F4=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBEF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBEE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBED;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBEC;struct Cyc_Parse_Declarator _tmpBEB;yyval=Cyc_YY30(((_tmpBEB.id=_tmp5F4.id,((_tmpBEB.tms=(
(_tmpBF0=_region_malloc(yyr,sizeof(*_tmpBF0)),((_tmpBF0->hd=(void*)((_tmpBEC=_region_malloc(yyr,sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBEF.tag=3,((_tmpBEF.f1=(void*)((_tmpBED=_region_malloc(yyr,sizeof(*_tmpBED)),((_tmpBED[0]=((_tmpBEE.tag=1,((_tmpBEE.f1=0,((_tmpBEE.f2=0,((_tmpBEE.f3=0,((_tmpBEE.f4=
# 2016
Cyc_yyget_YY50(yyyyvsp[2]),((_tmpBEE.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpBEE)))))))))))),_tmpBED)))),_tmpBEF)))),_tmpBEC)))),((_tmpBF0->tl=_tmp5F4.tms,_tmpBF0)))))),_tmpBEB)))));}
# 2019
break;}case 180: _LL2DB: {
# 2016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2020
struct Cyc_Parse_Declarator _tmp5FB=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC05;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC04;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC03;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC02;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC01;struct Cyc_Parse_Declarator _tmpC00;yyval=Cyc_YY30(((_tmpC00.id=_tmp5FB.id,((_tmpC00.tms=((_tmpC05=_region_malloc(yyr,sizeof(*_tmpC05)),((_tmpC05->hd=(void*)((_tmpC01=_region_malloc(yyr,sizeof(*_tmpC01)),((_tmpC01[0]=((_tmpC04.tag=3,((_tmpC04.f1=(void*)((_tmpC02=_region_malloc(yyr,sizeof(*_tmpC02)),((_tmpC02[0]=((_tmpC03.tag=0,((_tmpC03.f1=Cyc_yyget_YY37(yyyyvsp[2]),((_tmpC03.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC03)))))),_tmpC02)))),_tmpC04)))),_tmpC01)))),((_tmpC05->tl=_tmp5FB.tms,_tmpC05)))))),_tmpC00)))));}
break;}case 181: _LL2DC: {
# 2020 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2024
struct Cyc_List_List*_tmp602=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp603=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0F;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC0E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC0D;struct Cyc_Parse_Declarator _tmpC0C;yyval=Cyc_YY30(((_tmpC0C.id=_tmp603.id,((_tmpC0C.tms=((_tmpC0F=_region_malloc(yyr,sizeof(*_tmpC0F)),((_tmpC0F->hd=(void*)((_tmpC0D=_region_malloc(yyr,sizeof(*_tmpC0D)),((_tmpC0D[0]=((_tmpC0E.tag=4,((_tmpC0E.f1=_tmp602,((_tmpC0E.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC0E.f3=0,_tmpC0E)))))))),_tmpC0D)))),((_tmpC0F->tl=_tmp603.tms,_tmpC0F)))))),_tmpC0C)))));}
# 2028
break;}case 182: _LL2DD: {
# 2025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2029
struct Cyc_Parse_Declarator _tmp608=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC19;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC18;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC17;struct Cyc_Parse_Declarator _tmpC16;yyval=Cyc_YY30(((_tmpC16.id=_tmp608.id,((_tmpC16.tms=((_tmpC19=_region_malloc(yyr,sizeof(*_tmpC19)),((_tmpC19->hd=(void*)((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=5,((_tmpC18.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC18.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpC18)))))),_tmpC17)))),((_tmpC19->tl=_tmp608.tms,_tmpC19)))))),_tmpC16)))));}
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
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC1F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC1E;struct Cyc_List_List*_tmpC1D;ans=((_tmpC1D=_region_malloc(yyr,sizeof(*_tmpC1D)),((_tmpC1D->hd=(void*)((_tmpC1F=_region_malloc(yyr,sizeof(*_tmpC1F)),((_tmpC1F[0]=((_tmpC1E.tag=5,((_tmpC1E.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC1E.f2=Cyc_yyget_YY46(yyyyvsp[3]),_tmpC1E)))))),_tmpC1F)))),((_tmpC1D->tl=ans,_tmpC1D))))));}{
# 2051
struct Cyc_Absyn_PtrLoc*ptrloc=0;
unsigned int _tmp611;union Cyc_Absyn_Constraint*_tmp612;union Cyc_Absyn_Constraint*_tmp613;struct _tuple20 _tmp610=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp611=_tmp610.f1;_tmp612=_tmp610.f2;_tmp613=_tmp610.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC20;ptrloc=((_tmpC20=_cycalloc_atomic(sizeof(*_tmpC20)),((_tmpC20->ptr_loc=_tmp611,((_tmpC20->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC20->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC20))))))));}{
# 2056
union Cyc_Absyn_Constraint*_tmp616;union Cyc_Absyn_Constraint*_tmp617;union Cyc_Absyn_Constraint*_tmp618;void*_tmp619;struct _tuple14 _tmp615=Cyc_Parse_collapse_pointer_quals(_tmp611,_tmp612,_tmp613,Cyc_yyget_YY45(yyyyvsp[2]),Cyc_yyget_YY55(yyyyvsp[1]));_tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_tmp618=_tmp615.f3;_tmp619=_tmp615.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC2A;struct Cyc_Absyn_PtrAtts _tmpC29;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC28;struct Cyc_List_List*_tmpC27;ans=((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27->hd=(void*)((_tmpC2A=_region_malloc(yyr,sizeof(*_tmpC2A)),((_tmpC2A[0]=((_tmpC28.tag=2,((_tmpC28.f1=((_tmpC29.rgn=_tmp619,((_tmpC29.nullable=_tmp616,((_tmpC29.bounds=_tmp617,((_tmpC29.zero_term=_tmp618,((_tmpC29.ptrloc=ptrloc,_tmpC29)))))))))),((_tmpC28.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC28)))))),_tmpC2A)))),((_tmpC27->tl=ans,_tmpC27))))));}
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
{struct Cyc_List_List*_tmpC2B;yyval=Cyc_YY55(((_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B->hd=Cyc_yyget_YY54(yyyyvsp[0]),((_tmpC2B->tl=Cyc_yyget_YY55(yyyyvsp[1]),_tmpC2B)))))));}
break;}case 188: _LL2E3: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2068
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC2E;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC2D;yyval=Cyc_YY54((void*)((_tmpC2D=_region_malloc(yyr,sizeof(*_tmpC2D)),((_tmpC2D[0]=((_tmpC2E.tag=0,((_tmpC2E.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC2E)))),_tmpC2D)))));}
break;}case 189: _LL2E4: {
# 2066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2070
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC31;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC30;yyval=Cyc_YY54((void*)((_tmpC30=_region_malloc(yyr,sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC31.tag=1,((_tmpC31.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpC31)))),_tmpC30)))));}
break;}case 190: _LL2E5: {
# 2068 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC34;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC33;yyval=Cyc_YY54((void*)((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=2,_tmpC34)),_tmpC33)))));}
break;}case 191: _LL2E6: {
# 2070 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC37;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC36;yyval=Cyc_YY54((void*)((_tmpC36=_region_malloc(yyr,sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC37.tag=3,_tmpC37)),_tmpC36)))));}
break;}case 192: _LL2E7: {
# 2072 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC3A;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC39;yyval=Cyc_YY54((void*)((_tmpC39=_region_malloc(yyr,sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3A.tag=4,_tmpC3A)),_tmpC39)))));}
break;}case 193: _LL2E8: {
# 2074 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC3D;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC3C;yyval=Cyc_YY54((void*)((_tmpC3C=_region_malloc(yyr,sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3D.tag=5,_tmpC3D)),_tmpC3C)))));}
break;}case 194: _LL2E9: {
# 2076 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2080
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC40;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC3F;yyval=Cyc_YY54((void*)((_tmpC3F=_region_malloc(yyr,sizeof(*_tmpC3F)),((_tmpC3F[0]=((_tmpC40.tag=6,_tmpC40)),_tmpC3F)))));}
break;}case 195: _LL2EA: {
# 2078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2082
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC43;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC42;yyval=Cyc_YY54((void*)((_tmpC42=_region_malloc(yyr,sizeof(*_tmpC42)),((_tmpC42[0]=((_tmpC43.tag=7,_tmpC43)),_tmpC42)))));}
break;}case 196: _LL2EB: {
# 2083 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2088
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC44;yyval=Cyc_YY1(((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44->f1=loc,((_tmpC44->f2=Cyc_Absyn_true_conref,((_tmpC44->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC44)))))))));}
# 2091
break;}case 197: _LL2EC: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2092
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC45;yyval=Cyc_YY1(((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->f1=loc,((_tmpC45->f2=Cyc_Absyn_false_conref,((_tmpC45->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC45)))))))));}
# 2095
break;}case 198: _LL2ED: {
# 2092 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2096
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple20*_tmpC46;yyval=Cyc_YY1(((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->f1=loc,((_tmpC46->f2=Cyc_Absyn_true_conref,((_tmpC46->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC46)))))))));}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC49;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC48;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=1,((_tmpC49.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC49)))),_tmpC48))))));}
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
{struct _tuple24*_tmpC4A;yyval=Cyc_YY40(((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC4A->f2=0,((_tmpC4A->f3=0,((_tmpC4A->f4=Cyc_yyget_YY50(yyyyvsp[1]),((_tmpC4A->f5=Cyc_yyget_YY51(yyyyvsp[2]),_tmpC4A)))))))))))));}
break;}case 210: _LL2F9: {
# 2122 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2126
{struct _tuple24*_tmpC4B;yyval=Cyc_YY40(((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC4B->f2=1,((_tmpC4B->f3=0,((_tmpC4B->f4=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC4B->f5=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC4B)))))))))))));}
break;}case 211: _LL2FA: {
# 2125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2129
struct _dyneither_ptr*_tmp637;struct Cyc_Absyn_Tqual _tmp638;void*_tmp639;struct _tuple8*_tmp636=Cyc_yyget_YY38(yyyyvsp[2]);_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;_tmp639=_tmp636->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC4C;struct Cyc_Absyn_VarargInfo*_tmp63A=(_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C->name=_tmp637,((_tmpC4C->tq=_tmp638,((_tmpC4C->type=_tmp639,((_tmpC4C->inject=Cyc_yyget_YY32(yyyyvsp[1]),_tmpC4C)))))))));
{struct _tuple24*_tmpC4D;yyval=Cyc_YY40(((_tmpC4D=_cycalloc(sizeof(*_tmpC4D)),((_tmpC4D->f1=0,((_tmpC4D->f2=0,((_tmpC4D->f3=_tmp63A,((_tmpC4D->f4=Cyc_yyget_YY50(yyyyvsp[3]),((_tmpC4D->f5=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC4D)))))))))))));}
# 2133
break;};}case 212: _LL2FB: {
# 2131 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2135
struct _dyneither_ptr*_tmp63E;struct Cyc_Absyn_Tqual _tmp63F;void*_tmp640;struct _tuple8*_tmp63D=Cyc_yyget_YY38(yyyyvsp[4]);_tmp63E=_tmp63D->f1;_tmp63F=_tmp63D->f2;_tmp640=_tmp63D->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC4E;struct Cyc_Absyn_VarargInfo*_tmp641=(_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->name=_tmp63E,((_tmpC4E->tq=_tmp63F,((_tmpC4E->type=_tmp640,((_tmpC4E->inject=Cyc_yyget_YY32(yyyyvsp[3]),_tmpC4E)))))))));
{struct _tuple24*_tmpC4F;yyval=Cyc_YY40(((_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])),((_tmpC4F->f2=0,((_tmpC4F->f3=_tmp641,((_tmpC4F->f4=Cyc_yyget_YY50(yyyyvsp[5]),((_tmpC4F->f5=Cyc_yyget_YY51(yyyyvsp[6]),_tmpC4F)))))))))))));}
# 2139
break;};}case 213: _LL2FC: {
# 2139 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2143
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC52;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC51;yyval=Cyc_YY45(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC52.tag=1,((_tmpC52.f1=0,_tmpC52)))),_tmpC51))))));}
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
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC55;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC54;yyval=Cyc_YY50((void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=24,((_tmpC55.f1=Cyc_yyget_YY41(yyyyvsp[1]),_tmpC55)))),_tmpC54)))));}
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
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC58;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC57;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp648=(_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC58.tag=2,((_tmpC58.f1=0,((_tmpC58.f2=& Cyc_Tcutil_trk,_tmpC58)))))),_tmpC57)));
struct _dyneither_ptr _tmp649=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp64A=Cyc_Parse_id2type(_tmp649,(void*)_tmp648);
{struct _tuple27*_tmpC62;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC61;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC60;struct Cyc_List_List*_tmpC5F;yyval=Cyc_YY51(((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F->hd=((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->f1=(void*)((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC61.tag=24,((_tmpC61.f1=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC61)))),_tmpC60)))),((_tmpC62->f2=_tmp64A,_tmpC62)))))),((_tmpC5F->tl=0,_tmpC5F)))))));}
# 2166
break;}case 220: _LL303: {
# 2163 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2165
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2168
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC65;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC64;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp651=(_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC65.tag=2,((_tmpC65.f1=0,((_tmpC65.f2=& Cyc_Tcutil_trk,_tmpC65)))))),_tmpC64)));
struct _dyneither_ptr _tmp652=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp653=Cyc_Parse_id2type(_tmp652,(void*)_tmp651);
{struct _tuple27*_tmpC6F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC6E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC6D;struct Cyc_List_List*_tmpC6C;yyval=Cyc_YY51(((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=((_tmpC6F=_cycalloc(sizeof(*_tmpC6F)),((_tmpC6F->f1=(void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=24,((_tmpC6E.f1=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC6E)))),_tmpC6D)))),((_tmpC6F->f2=_tmp653,_tmpC6F)))))),((_tmpC6C->tl=Cyc_yyget_YY51(yyyyvsp[4]),_tmpC6C)))))));}
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
{const char*_tmpC70;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC70="inject",_tag_dyneither(_tmpC70,sizeof(char),7))))!= 0){
const char*_tmpC71;Cyc_Parse_err(((_tmpC71="missing type in function declaration",_tag_dyneither(_tmpC71,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC77;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC76;struct Cyc_List_List*_tmpC75;yyval=Cyc_YY41(((_tmpC75=_cycalloc(sizeof(*_tmpC75)),((_tmpC75->hd=(void*)((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC76.tag=25,((_tmpC76.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpC76)))),_tmpC77)))),((_tmpC75->tl=0,_tmpC75)))))));}
break;}case 228: _LL30B: {
# 2192 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2196
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC78;yyval=Cyc_YY41(((_tmpC78=_cycalloc(sizeof(*_tmpC78)),((_tmpC78->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpC78->tl=0,_tmpC78)))))));}
# 2199
break;}case 229: _LL30C: {
# 2200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2204
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC7E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC7D;struct Cyc_List_List*_tmpC7C;yyval=Cyc_YY41(((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C->hd=(void*)((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7D.tag=23,((_tmpC7D.f1=Cyc_yyget_YY45(yyyyvsp[0]),_tmpC7D)))),_tmpC7E)))),((_tmpC7C->tl=0,_tmpC7C)))))));}
# 2207
break;}case 230: _LL30D: {
# 2204 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2208
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC84;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC83;struct Cyc_List_List*_tmpC82;yyval=Cyc_YY41(((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->hd=(void*)((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84[0]=((_tmpC83.tag=23,((_tmpC83.f1=Cyc_yyget_YY45(yyyyvsp[0]),_tmpC83)))),_tmpC84)))),((_tmpC82->tl=Cyc_yyget_YY41(yyyyvsp[2]),_tmpC82)))))));}
# 2211
break;}case 231: _LL30E: {
# 2212 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2216
{struct Cyc_List_List*_tmpC85;yyval=Cyc_YY39(((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->hd=Cyc_yyget_YY38(yyyyvsp[0]),((_tmpC85->tl=0,_tmpC85)))))));}
break;}case 232: _LL30F: {
# 2214 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2218
{struct Cyc_List_List*_tmpC86;yyval=Cyc_YY39(((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->hd=Cyc_yyget_YY38(yyyyvsp[2]),((_tmpC86->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpC86)))))));}
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
const char*_tmpC87;Cyc_Parse_err(((_tmpC87="parameter with bad type params",_tag_dyneither(_tmpC87,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp66D)){
const char*_tmpC88;Cyc_Parse_err(((_tmpC88="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC88,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp66D).f2;
if(_tmp674 != 0){
const char*_tmpC8B;void*_tmpC8A;(_tmpC8A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8B="extra attributes on parameter, ignoring",_tag_dyneither(_tmpC8B,sizeof(char),40))),_tag_dyneither(_tmpC8A,sizeof(void*),0)));}
{struct _tuple8*_tmpC8C;yyval=Cyc_YY38(((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->f1=idopt,((_tmpC8C->f2=_tmp671,((_tmpC8C->f3=_tmp672,_tmpC8C)))))))));}
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
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC8F="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC8F,sizeof(char),38))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}
{struct _tuple8*_tmpC90;yyval=Cyc_YY38(((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->f1=0,((_tmpC90->f2=_tmp67B,((_tmpC90->f3=_tmp67E,_tmpC90)))))))));}
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
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC93="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpC93,sizeof(char),49))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}
# 2255
if(_tmp68C != 0){
const char*_tmpC96;void*_tmpC95;(_tmpC95=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpC96="bad attributes on parameter, ignoring",_tag_dyneither(_tmpC96,sizeof(char),38))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}
{struct _tuple8*_tmpC97;yyval=Cyc_YY38(((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->f1=0,((_tmpC97->f2=_tmp689,((_tmpC97->f3=_tmp68A,_tmpC97)))))))));}
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
{struct _dyneither_ptr*_tmpC9A;struct Cyc_List_List*_tmpC99;yyval=Cyc_YY37(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpC9A)))),((_tmpC99->tl=0,_tmpC99)))))));}
break;}case 238: _LL315: {
# 2265 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2269
{struct _dyneither_ptr*_tmpC9D;struct Cyc_List_List*_tmpC9C;yyval=Cyc_YY37(((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->hd=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpC9D)))),((_tmpC9C->tl=Cyc_yyget_YY37(yyyyvsp[0]),_tmpC9C)))))));}
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
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCA0;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpC9F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=35,((_tmpCA0.f1=0,((_tmpCA0.f2=0,_tmpCA0)))))),_tmpC9F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 242: _LL319: {
# 2277 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2281
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCA3;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCA2;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=35,((_tmpCA3.f1=0,((_tmpCA3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCA3)))))),_tmpCA2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL31A: {
# 2279 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2283
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCA6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCA5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=35,((_tmpCA6.f1=0,((_tmpCA6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCA6)))))),_tmpCA5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL31B: {
# 2281 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2285
struct _dyneither_ptr*_tmpCA9;struct _tuple0*_tmpCA8;struct Cyc_Absyn_Vardecl*_tmp69C=Cyc_Absyn_new_vardecl(((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=Cyc_Absyn_Loc_n,((_tmpCA8->f2=((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCA9)))),_tmpCA8)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2288
(_tmp69C->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCAC;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCAB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB[0]=((_tmpCAC.tag=26,((_tmpCAC.f1=_tmp69C,((_tmpCAC.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCAC.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCAC.f4=0,_tmpCAC)))))))))),_tmpCAB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2291
break;}case 245: _LL31C: {
# 2288 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2292
void*_tmp6A1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCAF;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCAE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAF.tag=27,((_tmpCAF.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCAF.f2=_tmp6A1,((_tmpCAF.f3=0,_tmpCAF)))))))),_tmpCAE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2295
break;}case 246: _LL31D: {
# 2296 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2300
{struct _tuple28*_tmpCB2;struct Cyc_List_List*_tmpCB1;yyval=Cyc_YY7(((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->f1=0,((_tmpCB2->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCB2)))))),((_tmpCB1->tl=0,_tmpCB1)))))));}
break;}case 247: _LL31E: {
# 2298 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2302
{struct _tuple28*_tmpCB5;struct Cyc_List_List*_tmpCB4;yyval=Cyc_YY7(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->f1=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpCB5->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCB5)))))),((_tmpCB4->tl=0,_tmpCB4)))))));}
break;}case 248: _LL31F: {
# 2300 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2304
{struct _tuple28*_tmpCB8;struct Cyc_List_List*_tmpCB7;yyval=Cyc_YY7(((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->hd=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->f1=0,((_tmpCB8->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCB8)))))),((_tmpCB7->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCB7)))))));}
break;}case 249: _LL320: {
# 2302 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2306
{struct _tuple28*_tmpCBB;struct Cyc_List_List*_tmpCBA;yyval=Cyc_YY7(((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->hd=((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->f1=Cyc_yyget_YY42(yyyyvsp[2]),((_tmpCBB->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCBB)))))),((_tmpCBA->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCBA)))))));}
break;}case 250: _LL321: {
# 2306 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2310
yyval=Cyc_YY42(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY42(yyyyvsp[0])));
break;}case 251: _LL322: {
# 2311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2315
{struct Cyc_List_List*_tmpCBC;yyval=Cyc_YY42(((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC->hd=Cyc_yyget_YY43(yyyyvsp[0]),((_tmpCBC->tl=0,_tmpCBC)))))));}
break;}case 252: _LL323: {
# 2312 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2316
{struct Cyc_List_List*_tmpCBD;yyval=Cyc_YY42(((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->hd=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpCBD->tl=Cyc_yyget_YY42(yyyyvsp[0]),_tmpCBD)))))));}
break;}case 253: _LL324: {
# 2316 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2320
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCC0;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCBF;yyval=Cyc_YY43((void*)((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC0.tag=0,((_tmpCC0.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCC0)))),_tmpCBF)))));}
break;}case 254: _LL325: {
# 2317 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2321
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCC6;struct _dyneither_ptr*_tmpCC5;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCC4;yyval=Cyc_YY43((void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC6.tag=1,((_tmpCC6.f1=((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCC5)))),_tmpCC6)))),_tmpCC4)))));}
break;}case 255: _LL326: {
# 2322 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2326
struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_Parse_Type_specifier _tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple23 _tmp6B3=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B3.f3;{
void*_tmp6B7=Cyc_Parse_speclist2typ(_tmp6B5,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6B6 != 0){
const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCC9="ignoring attributes in type",_tag_dyneither(_tmpCC9,sizeof(char),28))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}
{struct _tuple8*_tmpCCA;yyval=Cyc_YY38(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=0,((_tmpCCA->f2=_tmp6B4,((_tmpCCA->f3=_tmp6B7,_tmpCCA)))))))));}
# 2332
break;};}case 256: _LL327: {
# 2329 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2333
struct Cyc_Absyn_Tqual _tmp6BC;struct Cyc_Parse_Type_specifier _tmp6BD;struct Cyc_List_List*_tmp6BE;struct _tuple23 _tmp6BB=Cyc_yyget_YY36(yyyyvsp[0]);_tmp6BC=_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;{
void*_tmp6BF=Cyc_Parse_speclist2typ(_tmp6BD,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6C0=(Cyc_yyget_YY31(yyyyvsp[1])).tms;
struct _tuple13 _tmp6C1=Cyc_Parse_apply_tms(_tmp6BC,_tmp6BF,_tmp6BE,_tmp6C0);
if(_tmp6C1.f3 != 0){
# 2339
const char*_tmpCCD;void*_tmpCCC;(_tmpCCC=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCCD="bad type params, ignoring",_tag_dyneither(_tmpCCD,sizeof(char),26))),_tag_dyneither(_tmpCCC,sizeof(void*),0)));}
if(_tmp6C1.f4 != 0){
const char*_tmpCD0;void*_tmpCCF;(_tmpCCF=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCD0="bad specifiers, ignoring",_tag_dyneither(_tmpCD0,sizeof(char),25))),_tag_dyneither(_tmpCCF,sizeof(void*),0)));}
{struct _tuple8*_tmpCD1;yyval=Cyc_YY38(((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=0,((_tmpCD1->f2=_tmp6C1.f1,((_tmpCD1->f3=_tmp6C1.f2,_tmpCD1)))))))));}
# 2344
break;};}case 257: _LL328: {
# 2343 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2347
yyval=Cyc_YY45(Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 258: _LL329: {
# 2344 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2348
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD3;yyval=Cyc_YY45((void*)((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3[0]=((_tmpCD4.tag=24,((_tmpCD4.f1=0,_tmpCD4)))),_tmpCD3)))));}
break;}case 259: _LL32A: {
# 2345 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2349
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD6;yyval=Cyc_YY45((void*)((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD7.tag=24,((_tmpCD7.f1=Cyc_yyget_YY41(yyyyvsp[1]),_tmpCD7)))),_tmpCD6)))));}
break;}case 260: _LL32B: {
# 2346 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2350
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCDA;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCD9;yyval=Cyc_YY45((void*)((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=((_tmpCDA.tag=25,((_tmpCDA.f1=Cyc_yyget_YY45(yyyyvsp[2]),_tmpCDA)))),_tmpCD9)))));}
break;}case 261: _LL32C: {
# 2347 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2351
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCE0;struct Cyc_List_List*_tmpCDF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCDE;yyval=Cyc_YY45((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCE0.tag=24,((_tmpCE0.f1=((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((_tmpCDF->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCDF->tl=Cyc_yyget_YY41(yyyyvsp[2]),_tmpCDF)))))),_tmpCE0)))),_tmpCDE)))));}
break;}case 262: _LL32D: {
# 2353 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2357
{struct Cyc_List_List*_tmpCE1;yyval=Cyc_YY41(((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCE1->tl=0,_tmpCE1)))))));}
break;}case 263: _LL32E: {
# 2354 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2358
{struct Cyc_List_List*_tmpCE2;yyval=Cyc_YY41(((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->hd=Cyc_yyget_YY45(yyyyvsp[2]),((_tmpCE2->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCE2)))))));}
break;}case 264: _LL32F: {
# 2359 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2363
{struct Cyc_Parse_Abstractdeclarator _tmpCE3;yyval=Cyc_YY31(((_tmpCE3.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCE3)));}
break;}case 265: _LL330: {
# 2361 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2365
yyval=yyyyvsp[0];
break;}case 266: _LL331: {
# 2363 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2367
{struct Cyc_Parse_Abstractdeclarator _tmpCE4;yyval=Cyc_YY31(((_tmpCE4.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY31(yyyyvsp[1])).tms),_tmpCE4)));}
break;}case 267: _LL332: {
# 2368 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2372
yyval=yyyyvsp[1];
break;}case 268: _LL333: {
# 2370 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2374
{struct Cyc_List_List*_tmpCEE;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCED;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCEC;struct Cyc_Parse_Abstractdeclarator _tmpCEB;yyval=Cyc_YY31(((_tmpCEB.tms=((_tmpCEE=_region_malloc(yyr,sizeof(*_tmpCEE)),((_tmpCEE->hd=(void*)((_tmpCEC=_region_malloc(yyr,sizeof(*_tmpCEC)),((_tmpCEC[0]=((_tmpCED.tag=0,((_tmpCED.f1=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpCED.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpCED)))))),_tmpCEC)))),((_tmpCEE->tl=0,_tmpCEE)))))),_tmpCEB)));}
break;}case 269: _LL334: {
# 2372 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376
{struct Cyc_List_List*_tmpCF8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpCF7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpCF6;struct Cyc_Parse_Abstractdeclarator _tmpCF5;yyval=Cyc_YY31(((_tmpCF5.tms=((_tmpCF8=_region_malloc(yyr,sizeof(*_tmpCF8)),((_tmpCF8->hd=(void*)((_tmpCF6=_region_malloc(yyr,sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF7.tag=0,((_tmpCF7.f1=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpCF7.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpCF7)))))),_tmpCF6)))),((_tmpCF8->tl=(Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpCF8)))))),_tmpCF5)));}
break;}case 270: _LL335: {
# 2374 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2378
{struct Cyc_List_List*_tmpD02;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD01;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD00;struct Cyc_Parse_Abstractdeclarator _tmpCFF;yyval=Cyc_YY31(((_tmpCFF.tms=((_tmpD02=_region_malloc(yyr,sizeof(*_tmpD02)),((_tmpD02->hd=(void*)((_tmpD00=_region_malloc(yyr,sizeof(*_tmpD00)),((_tmpD00[0]=((_tmpD01.tag=1,((_tmpD01.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD01.f2=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpD01.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD01)))))))),_tmpD00)))),((_tmpD02->tl=0,_tmpD02)))))),_tmpCFF)));}
break;}case 271: _LL336: {
# 2376 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2380
{struct Cyc_List_List*_tmpD0C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD0B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD0A;struct Cyc_Parse_Abstractdeclarator _tmpD09;yyval=Cyc_YY31(((_tmpD09.tms=((_tmpD0C=_region_malloc(yyr,sizeof(*_tmpD0C)),((_tmpD0C->hd=(void*)((_tmpD0A=_region_malloc(yyr,sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=1,((_tmpD0B.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD0B.f2=Cyc_yyget_YY52(yyyyvsp[4]),((_tmpD0B.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD0B)))))))),_tmpD0A)))),((_tmpD0C->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD0C)))))),_tmpD09)));}
# 2383
break;}case 272: _LL337: {
# 2380 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2384
{struct Cyc_List_List*_tmpD21;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD20;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD1F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD1E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD1D;struct Cyc_Parse_Abstractdeclarator _tmpD1C;yyval=Cyc_YY31(((_tmpD1C.tms=((_tmpD21=_region_malloc(yyr,sizeof(*_tmpD21)),((_tmpD21->hd=(void*)((_tmpD1D=_region_malloc(yyr,sizeof(*_tmpD1D)),((_tmpD1D[0]=((_tmpD20.tag=3,((_tmpD20.f1=(void*)((_tmpD1E=_region_malloc(yyr,sizeof(*_tmpD1E)),((_tmpD1E[0]=((_tmpD1F.tag=1,((_tmpD1F.f1=0,((_tmpD1F.f2=0,((_tmpD1F.f3=0,((_tmpD1F.f4=Cyc_yyget_YY50(yyyyvsp[1]),((_tmpD1F.f5=Cyc_yyget_YY51(yyyyvsp[2]),_tmpD1F)))))))))))),_tmpD1E)))),_tmpD20)))),_tmpD1D)))),((_tmpD21->tl=0,_tmpD21)))))),_tmpD1C)));}
# 2386
break;}case 273: _LL338: {
# 2383 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2387
struct Cyc_List_List*_tmp6EB;int _tmp6EC;struct Cyc_Absyn_VarargInfo*_tmp6ED;void*_tmp6EE;struct Cyc_List_List*_tmp6EF;struct _tuple24*_tmp6EA=Cyc_yyget_YY40(yyyyvsp[1]);_tmp6EB=_tmp6EA->f1;_tmp6EC=_tmp6EA->f2;_tmp6ED=_tmp6EA->f3;_tmp6EE=_tmp6EA->f4;_tmp6EF=_tmp6EA->f5;
{struct Cyc_List_List*_tmpD36;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD35;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD34;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD33;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD32;struct Cyc_Parse_Abstractdeclarator _tmpD31;yyval=Cyc_YY31(((_tmpD31.tms=((_tmpD36=_region_malloc(yyr,sizeof(*_tmpD36)),((_tmpD36->hd=(void*)((_tmpD32=_region_malloc(yyr,sizeof(*_tmpD32)),((_tmpD32[0]=((_tmpD35.tag=3,((_tmpD35.f1=(void*)((_tmpD33=_region_malloc(yyr,sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=1,((_tmpD34.f1=_tmp6EB,((_tmpD34.f2=_tmp6EC,((_tmpD34.f3=_tmp6ED,((_tmpD34.f4=_tmp6EE,((_tmpD34.f5=_tmp6EF,_tmpD34)))))))))))),_tmpD33)))),_tmpD35)))),_tmpD32)))),((_tmpD36->tl=0,_tmpD36)))))),_tmpD31)));}
# 2390
break;}case 274: _LL339: {
# 2387 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2391
{struct Cyc_List_List*_tmpD4B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD4A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD49;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD48;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD47;struct Cyc_Parse_Abstractdeclarator _tmpD46;yyval=Cyc_YY31(((_tmpD46.tms=((_tmpD4B=_region_malloc(yyr,sizeof(*_tmpD4B)),((_tmpD4B->hd=(void*)((_tmpD47=_region_malloc(yyr,sizeof(*_tmpD47)),((_tmpD47[0]=((_tmpD4A.tag=3,((_tmpD4A.f1=(void*)((_tmpD48=_region_malloc(yyr,sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD49.tag=1,((_tmpD49.f1=0,((_tmpD49.f2=0,((_tmpD49.f3=0,((_tmpD49.f4=Cyc_yyget_YY50(yyyyvsp[2]),((_tmpD49.f5=Cyc_yyget_YY51(yyyyvsp[3]),_tmpD49)))))))))))),_tmpD48)))),_tmpD4A)))),_tmpD47)))),((_tmpD4B->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD4B)))))),_tmpD46)));}
# 2394
break;}case 275: _LL33A: {
# 2391 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2395
struct Cyc_List_List*_tmp6FD;int _tmp6FE;struct Cyc_Absyn_VarargInfo*_tmp6FF;void*_tmp700;struct Cyc_List_List*_tmp701;struct _tuple24*_tmp6FC=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6FD=_tmp6FC->f1;_tmp6FE=_tmp6FC->f2;_tmp6FF=_tmp6FC->f3;_tmp700=_tmp6FC->f4;_tmp701=_tmp6FC->f5;
{struct Cyc_List_List*_tmpD60;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD5F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD5E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5C;struct Cyc_Parse_Abstractdeclarator _tmpD5B;yyval=Cyc_YY31(((_tmpD5B.tms=((_tmpD60=_region_malloc(yyr,sizeof(*_tmpD60)),((_tmpD60->hd=(void*)((_tmpD5C=_region_malloc(yyr,sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5F.tag=3,((_tmpD5F.f1=(void*)((_tmpD5D=_region_malloc(yyr,sizeof(*_tmpD5D)),((_tmpD5D[0]=((_tmpD5E.tag=1,((_tmpD5E.f1=_tmp6FD,((_tmpD5E.f2=_tmp6FE,((_tmpD5E.f3=_tmp6FF,((_tmpD5E.f4=_tmp700,((_tmpD5E.f5=_tmp701,_tmpD5E)))))))))))),_tmpD5D)))),_tmpD5F)))),_tmpD5C)))),((_tmpD60->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD60)))))),_tmpD5B)));}
# 2399
break;}case 276: _LL33B: {
# 2397 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2401
struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD6A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD69;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD68;struct Cyc_Parse_Abstractdeclarator _tmpD67;yyval=Cyc_YY31(((_tmpD67.tms=((_tmpD6A=_region_malloc(yyr,sizeof(*_tmpD6A)),((_tmpD6A->hd=(void*)((_tmpD68=_region_malloc(yyr,sizeof(*_tmpD68)),((_tmpD68[0]=((_tmpD69.tag=4,((_tmpD69.f1=_tmp708,((_tmpD69.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD69.f3=0,_tmpD69)))))))),_tmpD68)))),((_tmpD6A->tl=(
Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD6A)))))),_tmpD67)));}
# 2405
break;}case 277: _LL33C: {
# 2402 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2406
{struct Cyc_List_List*_tmpD74;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD73;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD72;struct Cyc_Parse_Abstractdeclarator _tmpD71;yyval=Cyc_YY31(((_tmpD71.tms=((_tmpD74=_region_malloc(yyr,sizeof(*_tmpD74)),((_tmpD74->hd=(void*)((_tmpD72=_region_malloc(yyr,sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=5,((_tmpD73.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD73.f2=Cyc_yyget_YY46(yyyyvsp[1]),_tmpD73)))))),_tmpD72)))),((_tmpD74->tl=(Cyc_yyget_YY31(yyyyvsp[0])).tms,_tmpD74)))))),_tmpD71)));}
# 2408
break;}case 278: _LL33D: {
# 2408 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2412
yyval=yyyyvsp[0];
break;}case 279: _LL33E: {
# 2409 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413
yyval=yyyyvsp[0];
break;}case 280: _LL33F: {
# 2410 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2414
yyval=yyyyvsp[0];
break;}case 281: _LL340: {
# 2411 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2415
yyval=yyyyvsp[0];
break;}case 282: _LL341: {
# 2412 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2416
yyval=yyyyvsp[0];
break;}case 283: _LL342: {
# 2413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417
yyval=yyyyvsp[0];
break;}case 284: _LL343: {
# 2416 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2420
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD77;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD76;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=((_tmpD77.tag=16,((_tmpD77.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD77)))),_tmpD76)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 285: _LL344:
# 2420 "parse.y"
 yyval=Cyc_YY5(0);
break;case 286: _LL345: {
# 2422 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2426
{const char*_tmpD78;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD78="open",_tag_dyneither(_tmpD78,sizeof(char),5))))!= 0){
const char*_tmpD79;Cyc_Parse_err(((_tmpD79="expecting `open'",_tag_dyneither(_tmpD79,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2430
break;}case 287: _LL346: {
# 2431 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2435
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD7F;struct _dyneither_ptr*_tmpD7E;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD7D;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7F.tag=13,((_tmpD7F.f1=((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD7E)))),((_tmpD7F.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD7F)))))),_tmpD7D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 288: _LL347: {
# 2435 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2439
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 289: _LL348: {
# 2436 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2440
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL349: {
# 2441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2445
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 291: _LL34A: {
# 2442 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2446
yyval=yyyyvsp[1];
break;}case 292: _LL34B: {
# 2447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2451
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 293: _LL34C: {
# 2448 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2452
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 294: _LL34D: {
# 2449 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2453
yyval=yyyyvsp[0];
break;}case 295: _LL34E: {
# 2450 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2454
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 296: _LL34F: {
# 2451 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2455
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD82;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD81;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=1,((_tmpD82.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpD82)))),_tmpD81)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 297: _LL350: {
# 2454 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2458
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD85;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD84;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84[0]=((_tmpD85.tag=1,((_tmpD85.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpD85)))),_tmpD84)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 298: _LL351: {
# 2459 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2463
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 299: _LL352: {
# 2461 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2465
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL353: {
# 2467 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2471
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL354: {
# 2470 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2474
struct Cyc_Absyn_Exp*_tmp71C=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp71C,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 302: _LL355: {
# 2474 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2478
struct Cyc_Absyn_Exp*_tmp71D=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp71D,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2481
break;}case 303: _LL356: {
# 2480 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2484
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL357:
# 2494 "parse.y"
 yyval=Cyc_YY11(0);
break;case 305: _LL358: {
# 2496 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2500
{struct Cyc_Absyn_Switch_clause*_tmpD88;struct Cyc_List_List*_tmpD87;yyval=Cyc_YY11(((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=((_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpD88->pat_vars=0,((_tmpD88->where_clause=0,((_tmpD88->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpD88->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD88)))))))))))),((_tmpD87->tl=0,_tmpD87)))))));}
# 2503
break;}case 306: _LL359: {
# 2500 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2504
{struct Cyc_Absyn_Switch_clause*_tmpD8B;struct Cyc_List_List*_tmpD8A;yyval=Cyc_YY11(((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->hd=((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD8B->pat_vars=0,((_tmpD8B->where_clause=0,((_tmpD8B->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpD8B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD8B)))))))))))),((_tmpD8A->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpD8A)))))));}
break;}case 307: _LL35A: {
# 2504 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2508
{struct Cyc_Absyn_Switch_clause*_tmpD8E;struct Cyc_List_List*_tmpD8D;yyval=Cyc_YY11(((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD8E->pat_vars=0,((_tmpD8E->where_clause=0,((_tmpD8E->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpD8E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD8E)))))))))))),((_tmpD8D->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpD8D)))))));}
break;}case 308: _LL35B: {
# 2506 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2510
{struct Cyc_Absyn_Switch_clause*_tmpD91;struct Cyc_List_List*_tmpD90;yyval=Cyc_YY11(((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->hd=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD91->pat_vars=0,((_tmpD91->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpD91->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpD91->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD91)))))))))))),((_tmpD90->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpD90)))))));}
break;}case 309: _LL35C: {
# 2510 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2514
{struct Cyc_Absyn_Switch_clause*_tmpD94;struct Cyc_List_List*_tmpD93;yyval=Cyc_YY11(((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpD94->pat_vars=0,((_tmpD94->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpD94->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpD94->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpD94)))))))))))),((_tmpD93->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpD93)))))));}
break;}case 310: _LL35D: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2521
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 311: _LL35E: {
# 2521 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2525
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL35F: {
# 2525 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2529
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL360: {
# 2528 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2532
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL361: {
# 2531 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2535
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL362: {
# 2534 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2538
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL363: {
# 2537 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2541
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL364: {
# 2540 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2544
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL365: {
# 2543 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2547
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL366: {
# 2546 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2550
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL367: {
# 2549 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2553
struct Cyc_List_List*_tmp728=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp729=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp728,_tmp729));
# 2558
break;}case 321: _LL368: {
# 2555 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2559
struct Cyc_List_List*_tmp72A=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp72B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp72A,_tmp72B));
# 2564
break;}case 322: _LL369: {
# 2561 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2565
struct Cyc_List_List*_tmp72C=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp72D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp72C,_tmp72D));
# 2570
break;}case 323: _LL36A: {
# 2567 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2571
struct Cyc_List_List*_tmp72E=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp72F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp72E,_tmp72F));
# 2576
break;}case 324: _LL36B: {
# 2577 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2581
{struct _dyneither_ptr*_tmpD95;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD95)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 325: _LL36C: {
# 2578 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2582
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 326: _LL36D: {
# 2579 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2581
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2583
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL36E: {
# 2580 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2584
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL36F: {
# 2581 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2583
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2585
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL370: {
# 2583 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2587
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL371: {
# 2584 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2588
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL372: {
# 2586 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2590
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL373: {
# 2595 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599
yyval=yyyyvsp[0];
break;}case 333: _LL374: {
# 2598 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2602
yyval=yyyyvsp[0];
break;}case 334: _LL375: {
# 2600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2604
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 335: _LL376: {
# 2603 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2607
yyval=yyyyvsp[0];
break;}case 336: _LL377: {
# 2605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2609
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 337: _LL378: {
# 2608 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2612
yyval=yyyyvsp[0];
break;}case 338: _LL379: {
# 2610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2614
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 339: _LL37A: {
# 2613 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2617
yyval=yyyyvsp[0];
break;}case 340: _LL37B: {
# 2615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2619
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 341: _LL37C: {
# 2618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2622
yyval=yyyyvsp[0];
break;}case 342: _LL37D: {
# 2620 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2624
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL37E: {
# 2623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2627
yyval=yyyyvsp[0];
break;}case 344: _LL37F: {
# 2625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2629
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL380: {
# 2628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2632
yyval=yyyyvsp[0];
break;}case 346: _LL381: {
# 2630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2634
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL382: {
# 2632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2636
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL383: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2639
yyval=yyyyvsp[0];
break;}case 349: _LL384: {
# 2637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2641
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL385: {
# 2639 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2643
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL386: {
# 2641 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2645
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL387: {
# 2643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2647
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL388: {
# 2646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2650
yyval=yyyyvsp[0];
break;}case 354: _LL389: {
# 2648 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL38A: {
# 2650 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2654
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL38B: {
# 2653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2657
yyval=yyyyvsp[0];
break;}case 357: _LL38C: {
# 2655 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2659
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL38D: {
# 2657 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2661
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL38E: {
# 2660 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2664
yyval=yyyyvsp[0];
break;}case 360: _LL38F: {
# 2662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2666
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL390: {
# 2664 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2668
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL391: {
# 2666 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2670
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL392: {
# 2669 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2673
yyval=yyyyvsp[0];
break;}case 364: _LL393: {
# 2671 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2675
void*_tmp731=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp731,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2678
break;}case 365: _LL394: {
# 2677 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2681
yyval=yyyyvsp[0];
break;}case 366: _LL395: {
# 2680 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2684
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 367: _LL396: {
# 2682 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2686
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL397: {
# 2684 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2688
void*_tmp732=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp732,(unsigned int)(yyyylsp[0]).first_line)));
# 2691
break;}case 369: _LL398: {
# 2688 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2692
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 370: _LL399: {
# 2690 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2694
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY38(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 371: _LL39A: {
# 2695 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2699
yyval=yyyyvsp[0];
break;}case 372: _LL39B: {
# 2704 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2708
yyval=yyyyvsp[0];
break;}case 373: _LL39C: {
# 2709 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2713
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 374: _LL39D: {
# 2711 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2715
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 375: _LL39E: {
# 2713 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2717
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp733=e->r;enum Cyc_Absyn_Sign _tmp735;char _tmp736;enum Cyc_Absyn_Sign _tmp738;short _tmp739;enum Cyc_Absyn_Sign _tmp73B;int _tmp73C;struct _dyneither_ptr _tmp73E;int _tmp73F;_LL3A1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp734=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp734->tag != 0)goto _LL3A3;else{if(((_tmp734->f1).Char_c).tag != 2)goto _LL3A3;_tmp735=((struct _tuple3)((_tmp734->f1).Char_c).val).f1;_tmp736=((struct _tuple3)((_tmp734->f1).Char_c).val).f2;}}_LL3A2:
# 2721
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpD98;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpD97;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD97=_cycalloc_atomic(sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD98.tag=10,((_tmpD98.f1=_tmp736,_tmpD98)))),_tmpD97)))),e->loc));}goto _LL3A0;_LL3A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp737=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp737->tag != 0)goto _LL3A5;else{if(((_tmp737->f1).Short_c).tag != 4)goto _LL3A5;_tmp738=((struct _tuple4)((_tmp737->f1).Short_c).val).f1;_tmp739=((struct _tuple4)((_tmp737->f1).Short_c).val).f2;}}_LL3A4:
# 2723
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD9B;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD9A;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD9A=_cycalloc_atomic(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9B.tag=9,((_tmpD9B.f1=_tmp738,((_tmpD9B.f2=(int)_tmp739,_tmpD9B)))))),_tmpD9A)))),e->loc));}goto _LL3A0;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp73A->tag != 0)goto _LL3A7;else{if(((_tmp73A->f1).Int_c).tag != 5)goto _LL3A7;_tmp73B=((struct _tuple5)((_tmp73A->f1).Int_c).val).f1;_tmp73C=((struct _tuple5)((_tmp73A->f1).Int_c).val).f2;}}_LL3A6:
# 2725
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpD9E;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpD9D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpD9D=_cycalloc_atomic(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9E.tag=9,((_tmpD9E.f1=_tmp73B,((_tmpD9E.f2=_tmp73C,_tmpD9E)))))),_tmpD9D)))),e->loc));}goto _LL3A0;_LL3A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp73D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp73D->tag != 0)goto _LL3A9;else{if(((_tmp73D->f1).Float_c).tag != 7)goto _LL3A9;_tmp73E=((struct _tuple7)((_tmp73D->f1).Float_c).val).f1;_tmp73F=((struct _tuple7)((_tmp73D->f1).Float_c).val).f2;}}_LL3A8:
# 2727
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDA1;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDA0;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpDA1.tag=11,((_tmpDA1.f1=_tmp73E,((_tmpDA1.f2=_tmp73F,_tmpDA1)))))),_tmpDA0)))),e->loc));}goto _LL3A0;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp740=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp740->tag != 0)goto _LL3AB;else{if(((_tmp740->f1).Null_c).tag != 1)goto _LL3AB;}}_LL3AA:
# 2729
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A0;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp741=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp741->tag != 0)goto _LL3AD;else{if(((_tmp741->f1).String_c).tag != 8)goto _LL3AD;}}_LL3AC:
# 2731
{const char*_tmpDA2;Cyc_Parse_err(((_tmpDA2="strings cannot occur within patterns",_tag_dyneither(_tmpDA2,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A0;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp742=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp742->tag != 0)goto _LL3AF;else{if(((_tmp742->f1).Wstring_c).tag != 9)goto _LL3AF;}}_LL3AE:
# 2733
{const char*_tmpDA3;Cyc_Parse_err(((_tmpDA3="strings cannot occur within patterns",_tag_dyneither(_tmpDA3,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A0;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp743=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp733;if(_tmp743->tag != 0)goto _LL3B1;else{if(((_tmp743->f1).LongLong_c).tag != 6)goto _LL3B1;}}_LL3B0:
# 2735
{const char*_tmpDA4;Cyc_Parse_unimp(((_tmpDA4="long long's in patterns",_tag_dyneither(_tmpDA4,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A0;_LL3B1:;_LL3B2: {
# 2737
const char*_tmpDA5;Cyc_Parse_err(((_tmpDA5="bad constant in case",_tag_dyneither(_tmpDA5,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A0:;}
# 2740
break;}case 376: _LL39F: {
# 2737 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2741
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDA8;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDA7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA8.tag=14,((_tmpDA8.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDA8)))),_tmpDA7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 377: _LL3B3: {
# 2739 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2743
{const char*_tmpDA9;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDA9="as",_tag_dyneither(_tmpDA9,sizeof(char),3))))!= 0){
const char*_tmpDAA;Cyc_Parse_err(((_tmpDAA="expecting `as'",_tag_dyneither(_tmpDAA,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDB4;struct _dyneither_ptr*_tmpDB3;struct _tuple0*_tmpDB2;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDB1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB4.tag=1,((_tmpDB4.f1=Cyc_Absyn_new_vardecl(((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->f1=Cyc_Absyn_Loc_n,((_tmpDB2->f2=((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDB3)))),_tmpDB2)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDB4.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDB4)))))),_tmpDB1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2748
break;}case 378: _LL3B4: {
# 2745 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2749
struct Cyc_List_List*_tmp759;int _tmp75A;struct _tuple21 _tmp758=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp759=_tmp758.f1;_tmp75A=_tmp758.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDB7;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDB6;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=4,((_tmpDB7.f1=_tmp759,((_tmpDB7.f2=_tmp75A,_tmpDB7)))))),_tmpDB6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2752
break;}case 379: _LL3B5: {
# 2749 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2753
struct Cyc_List_List*_tmp75E;int _tmp75F;struct _tuple21 _tmp75D=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp75E=_tmp75D.f1;_tmp75F=_tmp75D.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDBA;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDB9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=15,((_tmpDBA.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDBA.f2=_tmp75E,((_tmpDBA.f3=_tmp75F,_tmpDBA)))))))),_tmpDB9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2756
break;}case 380: _LL3B6: {
# 2753 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2757
struct Cyc_List_List*_tmp763;int _tmp764;struct _tuple21 _tmp762=*Cyc_yyget_YY17(yyyyvsp[3]);_tmp763=_tmp762.f1;_tmp764=_tmp762.f2;{
struct Cyc_List_List*_tmp765=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY41(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDC0;struct Cyc_Absyn_AggrInfo*_tmpDBF;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDBE;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDC0.tag=6,((_tmpDC0.f1=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDBF->targs=0,_tmpDBF)))))),((_tmpDC0.f2=_tmp765,((_tmpDC0.f3=_tmp763,((_tmpDC0.f4=_tmp764,_tmpDC0)))))))))),_tmpDBE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2762
break;};}case 381: _LL3B7: {
# 2759 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2763
struct Cyc_List_List*_tmp76A;int _tmp76B;struct _tuple21 _tmp769=*Cyc_yyget_YY17(yyyyvsp[2]);_tmp76A=_tmp769.f1;_tmp76B=_tmp769.f2;{
struct Cyc_List_List*_tmp76C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY41(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDC3;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDC2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=6,((_tmpDC3.f1=0,((_tmpDC3.f2=_tmp76C,((_tmpDC3.f3=_tmp76A,((_tmpDC3.f4=_tmp76B,_tmpDC3)))))))))),_tmpDC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2767
break;};}case 382: _LL3B8: {
# 2764 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2768
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDC6;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDC5;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=5,((_tmpDC6.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDC6)))),_tmpDC5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 383: _LL3B9: {
# 2766 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2770
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDD0;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDCF;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDCE;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDCD;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDD0.tag=5,((_tmpDD0.f1=Cyc_Absyn_new_pat((void*)((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCF.tag=5,((_tmpDCF.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDCF)))),_tmpDCE)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDD0)))),_tmpDCD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 384: _LL3BA: {
# 2768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2772
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDDA;struct _dyneither_ptr*_tmpDD9;struct _tuple0*_tmpDD8;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDD7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDDA.tag=2,((_tmpDDA.f1=Cyc_Absyn_new_vardecl(((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->f1=Cyc_Absyn_Loc_n,((_tmpDD8->f2=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDD9)))),_tmpDD8)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDDA.f2=
# 2774
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDDA)))))),_tmpDD7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2776
break;}case 385: _LL3BB: {
# 2773 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2777
{const char*_tmpDDB;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDDB="as",_tag_dyneither(_tmpDDB,sizeof(char),3))))!= 0){
const char*_tmpDDC;Cyc_Parse_err(((_tmpDDC="expecting `as'",_tag_dyneither(_tmpDDC,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDE6;struct _dyneither_ptr*_tmpDE5;struct _tuple0*_tmpDE4;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDE3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE6.tag=2,((_tmpDE6.f1=Cyc_Absyn_new_vardecl(((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->f1=Cyc_Absyn_Loc_n,((_tmpDE4->f2=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDE5)))),_tmpDE4)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDE6.f2=
# 2781
Cyc_yyget_YY12(yyyyvsp[3]),_tmpDE6)))))),_tmpDE3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2783
break;}case 386: _LL3BC: {
# 2780 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2784
void*_tmp77F=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpDF7;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpDF6;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDF5;struct _dyneither_ptr*_tmpDF4;struct _tuple0*_tmpDF3;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpDF2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF7.tag=3,((_tmpDF7.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp77F),((_tmpDF7.f2=
Cyc_Absyn_new_vardecl(((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3->f1=Cyc_Absyn_Loc_n,((_tmpDF3->f2=((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDF4)))),_tmpDF3)))))),(void*)(
(_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=((_tmpDF6.tag=19,((_tmpDF6.f1=_tmp77F,_tmpDF6)))),_tmpDF5)))),0),_tmpDF7)))))),_tmpDF2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2789
break;}case 387: _LL3BD: {
# 2786 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2790
struct Cyc_Absyn_Tvar*_tmp786=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE13;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE12;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE11;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE10;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE0F;struct _dyneither_ptr*_tmpE0E;struct _tuple0*_tmpE0D;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE0C;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE13.tag=3,((_tmpE13.f1=_tmp786,((_tmpE13.f2=
Cyc_Absyn_new_vardecl(((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->f1=Cyc_Absyn_Loc_n,((_tmpE0D->f2=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE0E)))),_tmpE0D)))))),(void*)(
(_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE12.tag=19,((_tmpE12.f1=(void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=2,((_tmpE11.f1=_tmp786,_tmpE11)))),_tmpE10)))),_tmpE12)))),_tmpE0F)))),0),_tmpE13)))))),_tmpE0C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2795
break;}case 388: _LL3BE: {
# 2794 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2798
{struct _tuple21*_tmpE14;yyval=Cyc_YY13(((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE14->f2=0,_tmpE14)))))));}
break;}case 389: _LL3BF: {
# 2795 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2797
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2799
{struct _tuple21*_tmpE15;yyval=Cyc_YY13(((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE15->f2=1,_tmpE15)))))));}
break;}case 390: _LL3C0: {
# 2796 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2800
{struct _tuple21*_tmpE16;yyval=Cyc_YY13(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->f1=0,((_tmpE16->f2=1,_tmpE16)))))));}
break;}case 391: _LL3C1: {
# 2801 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2805
{struct Cyc_List_List*_tmpE17;yyval=Cyc_YY14(((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE17->tl=0,_tmpE17)))))));}
break;}case 392: _LL3C2: {
# 2803 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2807
{struct Cyc_List_List*_tmpE18;yyval=Cyc_YY14(((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE18->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE18)))))));}
break;}case 393: _LL3C3: {
# 2808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2812
{struct _tuple22*_tmpE19;yyval=Cyc_YY15(((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->f1=0,((_tmpE19->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE19)))))));}
break;}case 394: _LL3C4: {
# 2810 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2814
{struct _tuple22*_tmpE1A;yyval=Cyc_YY15(((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->f1=Cyc_yyget_YY42(yyyyvsp[0]),((_tmpE1A->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE1A)))))));}
break;}case 395: _LL3C5: {
# 2813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2817
{struct _tuple21*_tmpE1B;yyval=Cyc_YY17(((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE1B->f2=0,_tmpE1B)))))));}
break;}case 396: _LL3C6: {
# 2814 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2818
{struct _tuple21*_tmpE1C;yyval=Cyc_YY17(((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE1C->f2=1,_tmpE1C)))))));}
break;}case 397: _LL3C7: {
# 2815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2819
{struct _tuple21*_tmpE1D;yyval=Cyc_YY17(((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->f1=0,((_tmpE1D->f2=1,_tmpE1D)))))));}
break;}case 398: _LL3C8: {
# 2820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2824
{struct Cyc_List_List*_tmpE1E;yyval=Cyc_YY16(((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE1E->tl=0,_tmpE1E)))))));}
break;}case 399: _LL3C9: {
# 2822 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2826
{struct Cyc_List_List*_tmpE1F;yyval=Cyc_YY16(((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE1F->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE1F)))))));}
break;}case 400: _LL3CA: {
# 2828 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2832
yyval=yyyyvsp[0];
break;}case 401: _LL3CB: {
# 2830 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2834
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 402: _LL3CC: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2839
yyval=yyyyvsp[0];
break;}case 403: _LL3CD: {
# 2837 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2841
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 404: _LL3CE: {
# 2839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2843
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 405: _LL3CF: {
# 2843 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2847
yyval=Cyc_YY9(0);
break;}case 406: _LL3D0: {
# 2844 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848
{struct Cyc_Core_Opt*_tmpE20;yyval=Cyc_YY9(((_tmpE20=_cycalloc_atomic(sizeof(*_tmpE20)),((_tmpE20->v=(void*)Cyc_Absyn_Times,_tmpE20)))));}
break;}case 407: _LL3D1: {
# 2845 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849
{struct Cyc_Core_Opt*_tmpE21;yyval=Cyc_YY9(((_tmpE21=_cycalloc_atomic(sizeof(*_tmpE21)),((_tmpE21->v=(void*)Cyc_Absyn_Div,_tmpE21)))));}
break;}case 408: _LL3D2: {
# 2846 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850
{struct Cyc_Core_Opt*_tmpE22;yyval=Cyc_YY9(((_tmpE22=_cycalloc_atomic(sizeof(*_tmpE22)),((_tmpE22->v=(void*)Cyc_Absyn_Mod,_tmpE22)))));}
break;}case 409: _LL3D3: {
# 2847 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851
{struct Cyc_Core_Opt*_tmpE23;yyval=Cyc_YY9(((_tmpE23=_cycalloc_atomic(sizeof(*_tmpE23)),((_tmpE23->v=(void*)Cyc_Absyn_Plus,_tmpE23)))));}
break;}case 410: _LL3D4: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852
{struct Cyc_Core_Opt*_tmpE24;yyval=Cyc_YY9(((_tmpE24=_cycalloc_atomic(sizeof(*_tmpE24)),((_tmpE24->v=(void*)Cyc_Absyn_Minus,_tmpE24)))));}
break;}case 411: _LL3D5: {
# 2849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853
{struct Cyc_Core_Opt*_tmpE25;yyval=Cyc_YY9(((_tmpE25=_cycalloc_atomic(sizeof(*_tmpE25)),((_tmpE25->v=(void*)Cyc_Absyn_Bitlshift,_tmpE25)))));}
break;}case 412: _LL3D6: {
# 2850 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854
{struct Cyc_Core_Opt*_tmpE26;yyval=Cyc_YY9(((_tmpE26=_cycalloc_atomic(sizeof(*_tmpE26)),((_tmpE26->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE26)))));}
break;}case 413: _LL3D7: {
# 2851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855
{struct Cyc_Core_Opt*_tmpE27;yyval=Cyc_YY9(((_tmpE27=_cycalloc_atomic(sizeof(*_tmpE27)),((_tmpE27->v=(void*)Cyc_Absyn_Bitand,_tmpE27)))));}
break;}case 414: _LL3D8: {
# 2852 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856
{struct Cyc_Core_Opt*_tmpE28;yyval=Cyc_YY9(((_tmpE28=_cycalloc_atomic(sizeof(*_tmpE28)),((_tmpE28->v=(void*)Cyc_Absyn_Bitxor,_tmpE28)))));}
break;}case 415: _LL3D9: {
# 2853 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857
{struct Cyc_Core_Opt*_tmpE29;yyval=Cyc_YY9(((_tmpE29=_cycalloc_atomic(sizeof(*_tmpE29)),((_tmpE29->v=(void*)Cyc_Absyn_Bitor,_tmpE29)))));}
break;}case 416: _LL3DA: {
# 2858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2862
yyval=yyyyvsp[0];
break;}case 417: _LL3DB: {
# 2860 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2864
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 418: _LL3DC: {
# 2863 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2867
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3DD: {
# 2866 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2870
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 420: _LL3DE: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2872
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3DF: {
# 2870 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2874
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3E0: {
# 2872 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2876
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3E1: {
# 2876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2880
yyval=yyyyvsp[0];
break;}case 424: _LL3E2: {
# 2880 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2884
yyval=yyyyvsp[0];
break;}case 425: _LL3E3: {
# 2882 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2886
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E4: {
# 2886 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2890
yyval=yyyyvsp[0];
break;}case 427: _LL3E5: {
# 2888 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2892
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E6: {
# 2892 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2896
yyval=yyyyvsp[0];
break;}case 429: _LL3E7: {
# 2894 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2898
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 430: _LL3E8: {
# 2898 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2902
yyval=yyyyvsp[0];
break;}case 431: _LL3E9: {
# 2900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2904
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3EA: {
# 2904 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2908
yyval=yyyyvsp[0];
break;}case 433: _LL3EB: {
# 2906 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2910
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 434: _LL3EC: {
# 2910 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2914
yyval=yyyyvsp[0];
break;}case 435: _LL3ED: {
# 2912 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2916
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3EE: {
# 2914 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2918
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3EF: {
# 2918 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2922
yyval=yyyyvsp[0];
break;}case 438: _LL3F0: {
# 2920 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2924
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F1: {
# 2922 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2926
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3F2: {
# 2924 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2928
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F3: {
# 2926 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2930
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F4: {
# 2930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2934
yyval=yyyyvsp[0];
break;}case 443: _LL3F5: {
# 2932 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2936
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F6: {
# 2934 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2938
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3F7: {
# 2938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2942
yyval=yyyyvsp[0];
break;}case 446: _LL3F8: {
# 2940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2944
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F9: {
# 2942 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2946
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3FA: {
# 2946 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2950
yyval=yyyyvsp[0];
break;}case 449: _LL3FB: {
# 2948 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2952
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FC: {
# 2950 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2954
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3FD: {
# 2952 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2956
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3FE: {
# 2956 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2960
yyval=yyyyvsp[0];
break;}case 453: _LL3FF: {
# 2958 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2962
void*_tmp7A5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7A5,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2965
break;}case 454: _LL400: {
# 2964 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2968
yyval=yyyyvsp[0];
break;}case 455: _LL401: {
# 2965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2969
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL402: {
# 2966 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL403: {
# 2967 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL404: {
# 2968 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2972
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL405: {
# 2969 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2973
yyval=yyyyvsp[1];
break;}case 460: _LL406: {
# 2970 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2974
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL407: {
# 2972 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2976
void*_tmp7A6=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7A6,(unsigned int)(yyyylsp[0]).first_line));
# 2979
break;}case 462: _LL408: {
# 2975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2979
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL409: {
# 2977 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2981
void*_tmp7A7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7A7,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2984
break;}case 464: _LL40A: {
# 2982 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2986
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE2F;struct Cyc_Absyn_MallocInfo _tmpE2E;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE2D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2F.tag=33,((_tmpE2F.f1=((_tmpE2E.is_calloc=0,((_tmpE2E.rgn=0,((_tmpE2E.elt_type=0,((_tmpE2E.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE2E.fat_result=0,_tmpE2E)))))))))),_tmpE2F)))),_tmpE2D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2988
break;}case 465: _LL40B: {
# 2985 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2989
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE35;struct Cyc_Absyn_MallocInfo _tmpE34;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE33;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE35.tag=33,((_tmpE35.f1=((_tmpE34.is_calloc=0,((_tmpE34.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE34.elt_type=0,((_tmpE34.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE34.fat_result=0,_tmpE34)))))))))),_tmpE35)))),_tmpE33)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2991
break;}case 466: _LL40C: {
# 2988 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2992
void*_tmp7AE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE3F;void**_tmpE3E;struct Cyc_Absyn_MallocInfo _tmpE3D;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE3C;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3F.tag=33,((_tmpE3F.f1=((_tmpE3D.is_calloc=1,((_tmpE3D.rgn=0,((_tmpE3D.elt_type=((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=_tmp7AE,_tmpE3E)))),((_tmpE3D.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE3D.fat_result=0,_tmpE3D)))))))))),_tmpE3F)))),_tmpE3C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2995
break;}case 467: _LL40D: {
# 2993 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2997
void*_tmp7B3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE49;void**_tmpE48;struct Cyc_Absyn_MallocInfo _tmpE47;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE46;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE49.tag=33,((_tmpE49.f1=((_tmpE47.is_calloc=1,((_tmpE47.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE47.elt_type=((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48[0]=_tmp7B3,_tmpE48)))),((_tmpE47.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE47.fat_result=0,_tmpE47)))))))))),_tmpE49)))),_tmpE46)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3000
break;}case 468: _LL40E: {
# 2997 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3001
{struct Cyc_Absyn_Exp*_tmpE4A[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE4A[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE4A,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 469: _LL40F: {
# 2999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3003
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE50;struct _dyneither_ptr*_tmpE4F;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE4E;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE50.tag=37,((_tmpE50.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE50.f2=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE4F)))),_tmpE50)))))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 470: _LL410: {
# 3001 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3005
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE56;struct _dyneither_ptr*_tmpE55;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE54;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54[0]=((_tmpE56.tag=37,((_tmpE56.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE56.f2=((_tmpE55=_cycalloc(sizeof(*_tmpE55)),((_tmpE55[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE55)))),_tmpE56)))))),_tmpE54)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 471: _LL411: {
# 3003 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3007
void*_tmp7BF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY38(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7BF,(unsigned int)(yyyylsp[0]).first_line));
break;}case 472: _LL412: {
# 3006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3010
int _tmp7C1;struct _dyneither_ptr _tmp7C2;struct _tuple19 _tmp7C0=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp7C1=_tmp7C0.f1;_tmp7C2=_tmp7C0.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7C1,_tmp7C2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 473: _LL413: {
# 3011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3015
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 474: _LL414: {
# 3012 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3016
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 475: _LL415: {
# 3013 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3017
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 476: _LL416: {
# 3018 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3022
yyval=yyyyvsp[0];
break;}case 477: _LL417: {
# 3020 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3024
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL418: {
# 3022 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3026
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL419: {
# 3024 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3028
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL41A: {
# 3026 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3030
{struct _dyneither_ptr*_tmpE57;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE57)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 481: _LL41B: {
# 3028 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3032
{struct _dyneither_ptr*_tmpE58;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE58)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 482: _LL41C: {
# 3030 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3034
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL41D: {
# 3032 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3036
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL41E: {
# 3034 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3038
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE5B;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE5A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=24,((_tmpE5B.f1=Cyc_yyget_YY38(yyyyvsp[1]),((_tmpE5B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE5B)))))),_tmpE5A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 485: _LL41F: {
# 3036 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3040
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE5E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE5D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE5D=_cycalloc(sizeof(*_tmpE5D)),((_tmpE5D[0]=((_tmpE5E.tag=24,((_tmpE5E.f1=Cyc_yyget_YY38(yyyyvsp[1]),((_tmpE5E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE5E)))))),_tmpE5D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL420: {
# 3041 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3045
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE68;struct _dyneither_ptr*_tmpE67;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE66;struct Cyc_List_List*_tmpE65;yyval=Cyc_YY3(((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65->hd=(void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE66.tag=0,((_tmpE66.f1=((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE67)))),_tmpE66)))),_tmpE68)))),((_tmpE65->tl=0,_tmpE65)))))));}
break;}case 487: _LL421: {
# 3044 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3048
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE6E;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE6D;struct Cyc_List_List*_tmpE6C;yyval=Cyc_YY3(((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->hd=(void*)((_tmpE6E=_cycalloc_atomic(sizeof(*_tmpE6E)),((_tmpE6E[0]=((_tmpE6D.tag=1,((_tmpE6D.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE6D)))),_tmpE6E)))),((_tmpE6C->tl=0,_tmpE6C)))))));}
break;}case 488: _LL422: {
# 3046 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3050
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE78;struct _dyneither_ptr*_tmpE77;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE76;struct Cyc_List_List*_tmpE75;yyval=Cyc_YY3(((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75->hd=(void*)((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=((_tmpE76.tag=0,((_tmpE76.f1=((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE77)))),_tmpE76)))),_tmpE78)))),((_tmpE75->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE75)))))));}
break;}case 489: _LL423: {
# 3049 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3053
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE7E;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE7D;struct Cyc_List_List*_tmpE7C;yyval=Cyc_YY3(((_tmpE7C=_cycalloc(sizeof(*_tmpE7C)),((_tmpE7C->hd=(void*)((_tmpE7E=_cycalloc_atomic(sizeof(*_tmpE7E)),((_tmpE7E[0]=((_tmpE7D.tag=1,((_tmpE7D.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpE7D)))),_tmpE7E)))),((_tmpE7C->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE7C)))))));}
break;}case 490: _LL424: {
# 3055 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3059
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 491: _LL425: {
# 3057 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3061
yyval=yyyyvsp[0];
break;}case 492: _LL426: {
# 3059 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3063
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 493: _LL427: {
# 3061 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3065
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL428: {
# 3063 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3067
yyval=yyyyvsp[1];
break;}case 495: _LL429: {
# 3068 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3072
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL42A: {
# 3070 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3074
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL42B: {
# 3073 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3077
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42C: {
# 3076 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3080
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE81;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE80;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=((_tmpE81.tag=28,((_tmpE81.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE81.f2=Cyc_yyget_YY41(yyyyvsp[2]),((_tmpE81.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpE81.f4=0,_tmpE81)))))))))),_tmpE80)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 499: _LL42D: {
# 3079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3083
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 500: _LL42E: {
# 3083 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3087
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 501: _LL42F: {
# 3089 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3093
{struct Cyc_List_List*_tmpE82;yyval=Cyc_YY6(((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE82->tl=0,_tmpE82)))))));}
break;}case 502: _LL430: {
# 3091 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3095
{struct Cyc_List_List*_tmpE83;yyval=Cyc_YY6(((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE83->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpE83)))))));}
break;}case 503: _LL431: {
# 3097 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3101
struct _tuple5 _tmp7DB=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7DB.f1,_tmp7DB.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL432: {
# 3098 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3102
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL433: {
# 3099 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3103
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL434: {
# 3100 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3105
struct _dyneither_ptr _tmp7DC=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7DC);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7DC,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3113
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7DC,i,(unsigned int)(yyyylsp[0]).first_line));
# 3115
break;}case 507: _LL435: {
# 3112 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3116
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 508: _LL436: {
# 3116 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120
{struct _dyneither_ptr*_tmpE86;struct _tuple0*_tmpE85;yyval=Cyc_QualId_tok(((_tmpE85=_cycalloc(sizeof(*_tmpE85)),((_tmpE85->f1=Cyc_Absyn_Rel_n(0),((_tmpE85->f2=((_tmpE86=_cycalloc(sizeof(*_tmpE86)),((_tmpE86[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE86)))),_tmpE85)))))));}
break;}case 509: _LL437: {
# 3117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121
yyval=yyyyvsp[0];
break;}case 510: _LL438: {
# 3120 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3124
{struct _dyneither_ptr*_tmpE89;struct _tuple0*_tmpE88;yyval=Cyc_QualId_tok(((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->f1=Cyc_Absyn_Rel_n(0),((_tmpE88->f2=((_tmpE89=_cycalloc(sizeof(*_tmpE89)),((_tmpE89[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE89)))),_tmpE88)))))));}
break;}case 511: _LL439: {
# 3121 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125
yyval=yyyyvsp[0];
break;}case 512: _LL43A: {
# 3126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3130
yyval=yyyyvsp[0];
break;}case 513: _LL43B: {
# 3127 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131
yyval=yyyyvsp[0];
break;}case 514: _LL43C: {
# 3130 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3134
yyval=yyyyvsp[0];
break;}case 515: _LL43D: {
# 3131 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135
yyval=yyyyvsp[0];
break;}case 516: _LL43E: {
# 3135 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3140
break;}case 517: _LL43F: {
# 3136 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3140
yylex_buf->lex_curr_pos -=1;
break;}default: _LL440:
# 3145
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(518,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6494) && Cyc_yycheck[_check_known_subscript_notnull(6495,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6495,yystate)];else{
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
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1030,yystate)];
# 431
if(yyn > - 32768  && yyn < 6494){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6495,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)])+ 15,count ++);}
{unsigned int _tmpE8E;unsigned int _tmpE8D;struct _dyneither_ptr _tmpE8C;char*_tmpE8B;unsigned int _tmpE8A;msg=((_tmpE8A=(unsigned int)(sze + 15),((_tmpE8B=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpE8A + 1)),((_tmpE8C=_tag_dyneither(_tmpE8B,sizeof(char),_tmpE8A + 1),((((_tmpE8D=_tmpE8A,((_tmpE8F(& _tmpE8E,& _tmpE8D,& _tmpE8B),_tmpE8B[_tmpE8D]=(char)0)))),_tmpE8C))))))));}
{const char*_tmpE90;Cyc_strcpy(msg,((_tmpE90="parse error",_tag_dyneither(_tmpE90,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6495,x + yyn)]== x){
# 453
{const char*_tmpE92;const char*_tmpE91;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpE92=", expecting `",_tag_dyneither(_tmpE92,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpE91=" or `",_tag_dyneither(_tmpE91,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)]);
{const char*_tmpE93;Cyc_strcat(msg,((_tmpE93="'",_tag_dyneither(_tmpE93,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpE94;Cyc_yyerror(((_tmpE94="parse error",_tag_dyneither(_tmpE94,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7EA=1;_npop_handler(0);return _tmp7EA;}
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
 if(yyssp_offset == 0){int _tmp7EB=1;_npop_handler(0);return _tmp7EB;}
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
if((yyn < 0  || yyn > 6494) || Cyc_yycheck[_check_known_subscript_notnull(6495,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6495,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1029){
int _tmp7EC=0;_npop_handler(0);return _tmp7EC;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3139 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp7F6=v;int _tmp7F7;char _tmp7F8;struct _dyneither_ptr _tmp7F9;union Cyc_Absyn_Nmspace _tmp7FA;struct _dyneither_ptr*_tmp7FB;_LL443: if((_tmp7F6.Int_tok).tag != 1)goto _LL445;_tmp7F7=((struct _tuple5)(_tmp7F6.Int_tok).val).f2;_LL444:
{const char*_tmpE98;void*_tmpE97[1];struct Cyc_Int_pa_PrintArg_struct _tmpE96;(_tmpE96.tag=1,((_tmpE96.f1=(unsigned long)_tmp7F7,((_tmpE97[0]=& _tmpE96,Cyc_fprintf(Cyc_stderr,((_tmpE98="%d",_tag_dyneither(_tmpE98,sizeof(char),3))),_tag_dyneither(_tmpE97,sizeof(void*),1)))))));}goto _LL442;_LL445: if((_tmp7F6.Char_tok).tag != 2)goto _LL447;_tmp7F8=(char)(_tmp7F6.Char_tok).val;_LL446:
{const char*_tmpE9C;void*_tmpE9B[1];struct Cyc_Int_pa_PrintArg_struct _tmpE9A;(_tmpE9A.tag=1,((_tmpE9A.f1=(unsigned long)((int)_tmp7F8),((_tmpE9B[0]=& _tmpE9A,Cyc_fprintf(Cyc_stderr,((_tmpE9C="%c",_tag_dyneither(_tmpE9C,sizeof(char),3))),_tag_dyneither(_tmpE9B,sizeof(void*),1)))))));}goto _LL442;_LL447: if((_tmp7F6.String_tok).tag != 3)goto _LL449;_tmp7F9=(struct _dyneither_ptr)(_tmp7F6.String_tok).val;_LL448:
{const char*_tmpEA0;void*_tmpE9F[1];struct Cyc_String_pa_PrintArg_struct _tmpE9E;(_tmpE9E.tag=0,((_tmpE9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F9),((_tmpE9F[0]=& _tmpE9E,Cyc_fprintf(Cyc_stderr,((_tmpEA0="\"%s\"",_tag_dyneither(_tmpEA0,sizeof(char),5))),_tag_dyneither(_tmpE9F,sizeof(void*),1)))))));}goto _LL442;_LL449: if((_tmp7F6.QualId_tok).tag != 5)goto _LL44B;_tmp7FA=((struct _tuple0*)(_tmp7F6.QualId_tok).val)->f1;_tmp7FB=((struct _tuple0*)(_tmp7F6.QualId_tok).val)->f2;_LL44A: {
# 3145
struct Cyc_List_List*_tmp805=0;
{union Cyc_Absyn_Nmspace _tmp806=_tmp7FA;struct Cyc_List_List*_tmp807;struct Cyc_List_List*_tmp808;struct Cyc_List_List*_tmp809;_LL44E: if((_tmp806.Rel_n).tag != 1)goto _LL450;_tmp807=(struct Cyc_List_List*)(_tmp806.Rel_n).val;_LL44F:
 _tmp805=_tmp807;goto _LL44D;_LL450: if((_tmp806.Abs_n).tag != 2)goto _LL452;_tmp808=(struct Cyc_List_List*)(_tmp806.Abs_n).val;_LL451:
 _tmp805=_tmp808;goto _LL44D;_LL452: if((_tmp806.C_n).tag != 3)goto _LL454;_tmp809=(struct Cyc_List_List*)(_tmp806.C_n).val;_LL453:
 _tmp805=_tmp809;goto _LL44D;_LL454: if((_tmp806.Loc_n).tag != 4)goto _LL44D;_LL455:
 goto _LL44D;_LL44D:;}
# 3152
for(0;_tmp805 != 0;_tmp805=_tmp805->tl){
const char*_tmpEA4;void*_tmpEA3[1];struct Cyc_String_pa_PrintArg_struct _tmpEA2;(_tmpEA2.tag=0,((_tmpEA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp805->hd)),((_tmpEA3[0]=& _tmpEA2,Cyc_fprintf(Cyc_stderr,((_tmpEA4="%s::",_tag_dyneither(_tmpEA4,sizeof(char),5))),_tag_dyneither(_tmpEA3,sizeof(void*),1)))))));}
{const char*_tmpEA8;void*_tmpEA7[1];struct Cyc_String_pa_PrintArg_struct _tmpEA6;(_tmpEA6.tag=0,((_tmpEA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7FB),((_tmpEA7[0]=& _tmpEA6,Cyc_fprintf(Cyc_stderr,((_tmpEA8="%s::",_tag_dyneither(_tmpEA8,sizeof(char),5))),_tag_dyneither(_tmpEA7,sizeof(void*),1)))))));}
goto _LL442;}_LL44B:;_LL44C:
{const char*_tmpEAB;void*_tmpEAA;(_tmpEAA=0,Cyc_fprintf(Cyc_stderr,((_tmpEAB="?",_tag_dyneither(_tmpEAB,sizeof(char),2))),_tag_dyneither(_tmpEAA,sizeof(void*),0)));}goto _LL442;_LL442:;}
# 3161
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp812=_new_region("yyr");struct _RegionHandle*yyr=& _tmp812;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp813=Cyc_Parse_parse_result;_npop_handler(0);return _tmp813;};
# 3164
;_pop_region(yyr);};}
