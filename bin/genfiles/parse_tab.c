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
{struct _tuple10*_tmp827;declarators=((_tmp827=_region_malloc(r,sizeof(*_tmp827)),((_tmp827->tl=declarators,((_tmp827->hd=_tmp1,_tmp827))))));}{
struct Cyc_List_List*_tmp828;exprs=((_tmp828=_region_malloc(r,sizeof(*_tmp828)),((_tmp828->hd=_tmp2,((_tmp828->tl=exprs,_tmp828))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp82B;struct Cyc_Position_Exit_exn_struct*_tmp82A;(int)_throw((void*)((_tmp82A=_cycalloc_atomic(sizeof(*_tmp82A)),((_tmp82A[0]=((_tmp82B.tag=Cyc_Position_Exit,_tmp82B)),_tmp82A)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp830;void*_tmp82F[2];struct Cyc_String_pa_PrintArg_struct _tmp82E;struct Cyc_String_pa_PrintArg_struct _tmp82D;(_tmp82D.tag=0,((_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp82E.tag=0,((_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp82F[0]=& _tmp82E,((_tmp82F[1]=& _tmp82D,Cyc_fprintf(Cyc_stderr,((_tmp830="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp830,sizeof(char),46))),_tag_dyneither(_tmp82F,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp833;void*_tmp832;(_tmp832=0,Cyc_Tcutil_warn(loc,((_tmp833="qualifier on type is ignored",_tag_dyneither(_tmp833,sizeof(char),29))),_tag_dyneither(_tmp832,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp836;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp835;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp835=_cycalloc(sizeof(*_tmp835)),((_tmp835[0]=((_tmp836.tag=1,((_tmp836.f1=_tmp18,_tmp836)))),_tmp835)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp837;return(_tmp837.f1=nullable,((_tmp837.f2=bound,((_tmp837.f3=zeroterm,((_tmp837.f4=rgn,_tmp837)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp844;const char*_tmp843;void*_tmp842[1];struct Cyc_Int_pa_PrintArg_struct _tmp841;struct _tuple0*_tmp840;return(_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840->f1=Cyc_Absyn_Rel_n(0),((_tmp840->f2=(
(_tmp844=_cycalloc(sizeof(*_tmp844)),((_tmp844[0]=(struct _dyneither_ptr)((_tmp841.tag=1,((_tmp841.f1=(unsigned long)enum_counter ++,((_tmp842[0]=& _tmp841,Cyc_aprintf(((_tmp843="__anonymous_enum_%d__",_tag_dyneither(_tmp843,sizeof(char),22))),_tag_dyneither(_tmp842,sizeof(void*),1)))))))),_tmp844)))),_tmp840)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp2A;struct _tuple17*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=(_tmp23->f2)->f1;_tmp2A=(_tmp23->f2)->f2;
if(_tmp27 != 0){
const char*_tmp845;Cyc_Parse_err(((_tmp845="bad type params in struct field",_tag_dyneither(_tmp845,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp846;Cyc_Parse_err(((_tmp846="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp846,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp847;return(_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847->name=(*_tmp24).f2,((_tmp847->tq=_tmp25,((_tmp847->type=_tmp26,((_tmp847->width=_tmp29,((_tmp847->attributes=_tmp28,((_tmp847->requires_clause=_tmp2A,_tmp847)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp848;return(_tmp848.Signed_spec=0,((_tmp848.Unsigned_spec=0,((_tmp848.Short_spec=0,((_tmp848.Long_spec=0,((_tmp848.Long_Long_spec=0,((_tmp848.Valid_type_spec=0,((_tmp848.Type_spec=Cyc_Absyn_sint_typ,((_tmp848.loc=loc,_tmp848)))))))))))))));}
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
struct Cyc_String_pa_PrintArg_struct _tmp850;void*_tmp84F[1];const char*_tmp84E;struct _dyneither_ptr*_tmp84D;struct _dyneither_ptr*nm=(_tmp84D=_cycalloc(sizeof(*_tmp84D)),((_tmp84D[0]=(struct _dyneither_ptr)((_tmp850.tag=0,((_tmp850.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp36),((_tmp84F[0]=& _tmp850,Cyc_aprintf(((_tmp84E="`%s",_tag_dyneither(_tmp84E,sizeof(char),4))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))))),_tmp84D)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp85F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp85E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp85D;struct Cyc_Absyn_Tvar*_tmp85C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp85B;*_tmp3F=(void*)((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85F.tag=2,((_tmp85F.f1=((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C->name=nm,((_tmp85C->identity=- 1,((_tmp85C->kind=(void*)((_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp85D.tag=0,((_tmp85D.f1=& Cyc_Tcutil_ik,_tmp85D)))),_tmp85E)))),_tmp85C)))))))),_tmp85F)))),_tmp85B))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp862;struct Cyc_List_List*_tmp861;_tmp34=((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->hd=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->f1=_tmp36,((_tmp862->f2=_tmp38,_tmp862)))))),((_tmp861->tl=_tmp34,_tmp861))))));}goto _LL11;_LL14: if(_tmp35->f1 == 0)goto _LL16;_tmp39=*_tmp35->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp35->f3;if(_tmp3A->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3A->f1);if(_tmp3B->tag != 1)goto _LL16;else{_tmp3C=(void**)((void**)& _tmp3B->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp86A;void*_tmp869[1];const char*_tmp868;struct _dyneither_ptr*_tmp867;struct _dyneither_ptr*nm=(_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=(struct _dyneither_ptr)((_tmp86A.tag=0,((_tmp86A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39),((_tmp869[0]=& _tmp86A,Cyc_aprintf(((_tmp868="`%s",_tag_dyneither(_tmp868,sizeof(char),4))),_tag_dyneither(_tmp869,sizeof(void*),1)))))))),_tmp867)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp879;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp878;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp877;struct Cyc_Absyn_Tvar*_tmp876;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp875;*_tmp3C=(void*)((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875[0]=((_tmp879.tag=2,((_tmp879.f1=((_tmp876=_cycalloc(sizeof(*_tmp876)),((_tmp876->name=nm,((_tmp876->identity=- 1,((_tmp876->kind=(void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp877.tag=0,((_tmp877.f1=& Cyc_Tcutil_rk,_tmp877)))),_tmp878)))),_tmp876)))))))),_tmp879)))),_tmp875))));}
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
{struct _tuple18*_tmp87C;struct Cyc_List_List*_tmp87B;_tmp54=((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B->hd=((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp87C->f2=_tmp57,_tmp87C)))))),((_tmp87B->tl=_tmp54,_tmp87B))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp87F;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp87E;return Cyc_Absyn_new_exp((void*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp87F.tag=38,((_tmp87F.f1=Cyc_Tcutil_copy_type(_tmp60),_tmp87F)))),_tmp87E)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp885;struct Cyc_Absyn_ArrayInfo _tmp884;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp883;return(void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp885.tag=8,((_tmp885.f1=((_tmp884.elt_type=_tmp72,((_tmp884.tq=_tmp66,((_tmp884.num_elts=nelts2,((_tmp884.zero_term=_tmp68,((_tmp884.zt_loc=_tmp69,_tmp884)))))))))),_tmp885)))),_tmp883))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63;if(_tmp6A->tag != 5)goto _LL2C;else{_tmp6B=(_tmp6A->f1).elt_typ;_tmp6C=(_tmp6A->f1).elt_tq;_tmp6D=((_tmp6A->f1).ptr_atts).rgn;_tmp6E=((_tmp6A->f1).ptr_atts).nullable;_tmp6F=((_tmp6A->f1).ptr_atts).bounds;_tmp70=((_tmp6A->f1).ptr_atts).zero_term;_tmp71=((_tmp6A->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp76=Cyc_Parse_substitute_tags(tags,_tmp6B);
union Cyc_Absyn_Constraint*_tmp77=_tmp6F;
{union Cyc_Absyn_Constraint _stmttmp6=*_tmp6F;union Cyc_Absyn_Constraint _tmp78=_stmttmp6;struct Cyc_Absyn_Exp*_tmp7A;_LL2F: if((_tmp78.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp79=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp78.Eq_constr).val);if(_tmp79->tag != 1)goto _LL31;else{_tmp7A=_tmp79->f1;}};_LL30:
# 384
 _tmp7A=Cyc_Parse_substitute_tags_exp(tags,_tmp7A);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp88B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp88A;union Cyc_Absyn_Constraint*_tmp889;_tmp77=((_tmp889=_cycalloc(sizeof(*_tmp889)),(((_tmp889->Eq_constr).val=(void*)((_tmp88B=_cycalloc(sizeof(*_tmp88B)),((_tmp88B[0]=((_tmp88A.tag=1,((_tmp88A.f1=_tmp7A,_tmp88A)))),_tmp88B)))),(((_tmp889->Eq_constr).tag=1,_tmp889))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp76 != _tmp6B  || _tmp77 != _tmp6F){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp891;struct Cyc_Absyn_PtrInfo _tmp890;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp88F;return(void*)((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp891.tag=5,((_tmp891.f1=((_tmp890.elt_typ=_tmp76,((_tmp890.elt_tq=_tmp6C,((_tmp890.ptr_atts=(((_tmp890.ptr_atts).rgn=_tmp6D,(((_tmp890.ptr_atts).nullable=_tmp6E,(((_tmp890.ptr_atts).bounds=_tmp77,(((_tmp890.ptr_atts).zero_term=_tmp70,(((_tmp890.ptr_atts).ptrloc=_tmp71,_tmp890.ptr_atts)))))))))),_tmp890)))))),_tmp891)))),_tmp88F))));}
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
struct _tuple19*_tmp892;return(_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892->f1=(*t).f2,((_tmp892->f2=(*t).f3,_tmp892)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_stmttmp7=d->r;void*_tmp82=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp84;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp83=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp82;if(_tmp83->tag != 0)goto _LL36;else{_tmp84=_tmp83->f1;}}_LL35:
 return _tmp84;_LL36:;_LL37: {
const char*_tmp895;void*_tmp894;(_tmp894=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp895="bad declaration in `forarray' statement",_tag_dyneither(_tmp895,sizeof(char),40))),_tag_dyneither(_tmp894,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp87=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp88=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp87;if(_tmp88->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp896;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp896="`H",_tag_dyneither(_tmp896,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp897;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp897="`U",_tag_dyneither(_tmp897,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp898;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp898="`RC",_tag_dyneither(_tmp898,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A2;struct _dyneither_ptr*_tmp8A1;struct Cyc_Absyn_Tvar*_tmp8A0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89F;return(void*)((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F[0]=((_tmp8A2.tag=2,((_tmp8A2.f1=((_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0->name=((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=s,_tmp8A1)))),((_tmp8A0->identity=- 1,((_tmp8A0->kind=k,_tmp8A0)))))))),_tmp8A2)))),_tmp89F))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_stmttmp8=Cyc_Absyn_compress_kb(t->kind);void*_tmp90=_stmttmp8;struct Cyc_Absyn_Kind*_tmp93;void*_tmp94;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp91=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp91->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8A5;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8A4;k=(void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A5.tag=1,((_tmp8A5.f1=0,_tmp8A5)))),_tmp8A4))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90;if(_tmp92->tag != 2)goto _LL42;else{_tmp93=_tmp92->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8A8;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8A7;k=(void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8A8.tag=2,((_tmp8A8.f1=0,((_tmp8A8.f2=_tmp93,_tmp8A8)))))),_tmp8A7))));}goto _LL3D;_LL42: _tmp94=_tmp90;_LL43:
 k=_tmp94;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8A9;return(_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9->name=t->name,((_tmp8A9->identity=- 1,((_tmp8A9->kind=k,_tmp8A9)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp9A=t;struct Cyc_Absyn_Tvar*_tmp9C;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f1;}}_LL46:
 return _tmp9C;_LL47:;_LL48: {
const char*_tmp8AC;void*_tmp8AB;(_tmp8AB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8AC="expecting a list of type variables, not types",_tag_dyneither(_tmp8AC,sizeof(char),46))),_tag_dyneither(_tmp8AB,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8AF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AE;return(void*)((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8AF.tag=2,((_tmp8AF.f1=pr,_tmp8AF)))),_tmp8AE))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_stmttmp9=Cyc_Tcutil_compress(t);void*_tmpA1=_stmttmp9;void**_tmpA3;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA1;if(_tmpA2->tag != 2)goto _LL4C;else{_tmpA3=(void**)&(_tmpA2->f1)->kind;}}_LL4B: {
# 468
void*_stmttmpA=Cyc_Absyn_compress_kb(*_tmpA3);void*_tmpA4=_stmttmpA;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA4;if(_tmpA5->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpA3=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8B2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8B1;*_tmpA3=(void*)((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1[0]=((_tmp8B2.tag=2,((_tmp8B2.f1=0,((_tmp8B2.f2=k,_tmp8B2)))))),_tmp8B1))));}
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
{const char*_tmp8B5;void*_tmp8B4;(_tmp8B4=0,Cyc_Tcutil_warn(loc,((_tmp8B5="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8B5,sizeof(char),93))),_tag_dyneither(_tmp8B4,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpAD=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpAB;if(_tmpAD->tag != 0)goto _LL58;else{_tmpAE=_tmpAD->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8B8;void*_tmp8B7;(_tmp8B7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B8="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8B8,sizeof(char),73))),_tag_dyneither(_tmp8B7,sizeof(void*),0)));}{
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
const char*_tmp8BB;void*_tmp8BA;(_tmp8BA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BB="initializer found in parameter declaration",_tag_dyneither(_tmp8BB,sizeof(char),43))),_tag_dyneither(_tmp8BA,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpB7->name)){
const char*_tmp8BE;void*_tmp8BD;(_tmp8BD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8BE="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8BE,sizeof(char),47))),_tag_dyneither(_tmp8BD,sizeof(void*),0)));}
{struct _tuple8*_tmp8C1;struct Cyc_List_List*_tmp8C0;rev_new_params=(
(_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0->hd=((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->f1=(*_tmpB7->name).f2,((_tmp8C1->f2=_tmpB7->tq,((_tmp8C1->f3=_tmpB7->type,_tmp8C1)))))))),((_tmp8C0->tl=rev_new_params,_tmp8C0))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8C4;void*_tmp8C3;(_tmp8C3=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpB4->loc,((_tmp8C4="nonvariable declaration in parameter type",_tag_dyneither(_tmp8C4,sizeof(char),42))),_tag_dyneither(_tmp8C3,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpB3 == 0){
const char*_tmp8C8;void*_tmp8C7[1];struct Cyc_String_pa_PrintArg_struct _tmp8C6;(_tmp8C6.tag=0,((_tmp8C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpAE->hd)),((_tmp8C7[0]=& _tmp8C6,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8C8="%s is not given a type",_tag_dyneither(_tmp8C8,sizeof(char),23))),_tag_dyneither(_tmp8C7,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8D6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8D5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8D4;struct Cyc_List_List*_tmp8D3;return
(_tmp8D3=_region_malloc(yy,sizeof(*_tmp8D3)),((_tmp8D3->hd=(void*)((_tmp8D7=_region_malloc(yy,sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D4.tag=3,((_tmp8D4.f1=(void*)((_tmp8D6=_region_malloc(yy,sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D5.tag=1,((_tmp8D5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8D5.f2=0,((_tmp8D5.f3=0,((_tmp8D5.f4=0,((_tmp8D5.f5=0,_tmp8D5)))))))))))),_tmp8D6)))),_tmp8D4)))),_tmp8D7)))),((_tmp8D3->tl=0,_tmp8D3)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp8D8;return(_tmp8D8=_region_malloc(yy,sizeof(*_tmp8D8)),((_tmp8D8->hd=(void*)tms->hd,((_tmp8D8->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8D8)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8D9;d=((_tmp8D9.id=d.id,((_tmp8D9.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8D9))));}{
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
{const char*_tmp8DA;Cyc_Parse_err(((_tmp8DA="bad storage class on function",_tag_dyneither(_tmp8DA,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpCB=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _stmttmpE=Cyc_Parse_apply_tms(tq,_tmpCB,atts,d.tms);struct Cyc_Absyn_Tqual _tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct _tuple13 _tmpCC=_stmttmpE;_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;_tmpCF=_tmpCC.f3;_tmpD0=_tmpCC.f4;
# 578
if(_tmpCF != 0){
# 581
const char*_tmp8DD;void*_tmp8DC;(_tmp8DC=0,Cyc_Tcutil_warn(loc,((_tmp8DD="bad type params, ignoring",_tag_dyneither(_tmp8DD,sizeof(char),26))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}{
# 583
void*_tmpD3=_tmpCE;struct Cyc_List_List*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Tqual _tmpD7;void*_tmpD8;struct Cyc_List_List*_tmpD9;int _tmpDA;struct Cyc_Absyn_VarargInfo*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*_tmpDD;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD3;if(_tmpD4->tag != 9)goto _LL69;else{_tmpD5=(_tmpD4->f1).tvars;_tmpD6=(_tmpD4->f1).effect;_tmpD7=(_tmpD4->f1).ret_tqual;_tmpD8=(_tmpD4->f1).ret_typ;_tmpD9=(_tmpD4->f1).args;_tmpDA=(_tmpD4->f1).c_varargs;_tmpDB=(_tmpD4->f1).cyc_varargs;_tmpDC=(_tmpD4->f1).rgn_po;_tmpDD=(_tmpD4->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpDE=0;
{struct Cyc_List_List*_tmpDF=_tmpD9;for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
struct _tuple8*_stmttmpF=(struct _tuple8*)_tmpDF->hd;struct _dyneither_ptr*_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;void*_tmpE3;struct _tuple8*_tmpE0=_stmttmpF;_tmpE1=_tmpE0->f1;_tmpE2=_tmpE0->f2;_tmpE3=_tmpE0->f3;
if(_tmpE1 == 0){
{const char*_tmp8DE;Cyc_Parse_err(((_tmp8DE="missing argument variable in function prototype",_tag_dyneither(_tmp8DE,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8E1;struct Cyc_List_List*_tmp8E0;_tmpDE=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->hd=((_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8E1->f2=_tmpE2,((_tmp8E1->f3=_tmpE3,_tmp8E1)))))))),((_tmp8E0->tl=_tmpDE,_tmp8E0))))));};}else{
# 593
struct _tuple8*_tmp8E4;struct Cyc_List_List*_tmp8E3;_tmpDE=((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->hd=((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4->f1=_tmpE1,((_tmp8E4->f2=_tmpE2,((_tmp8E4->f3=_tmpE3,_tmp8E4)))))))),((_tmp8E3->tl=_tmpDE,_tmp8E3))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp8E5;return(_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->sc=sc,((_tmp8E5->name=d.id,((_tmp8E5->tvs=_tmpD5,((_tmp8E5->is_inline=is_inline,((_tmp8E5->effect=_tmpD6,((_tmp8E5->ret_tqual=_tmpD7,((_tmp8E5->ret_type=_tmpD8,((_tmp8E5->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDE),((_tmp8E5->c_varargs=_tmpDA,((_tmp8E5->cyc_varargs=_tmpDB,((_tmp8E5->rgn_po=_tmpDC,((_tmp8E5->body=body,((_tmp8E5->cached_typ=0,((_tmp8E5->param_vardecls=0,((_tmp8E5->fn_vardecl=0,((_tmp8E5->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpDD,_tmpD0),_tmp8E5)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E8="declarator is not a function prototype",_tag_dyneither(_tmp8E8,sizeof(char),39))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}_LL66:;};};};}static char _tmpED[52]="at most one type may appear within a type specifier";
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
void*_tmp8E9;(_tmp8E9=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8E9,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8EC;(_tmp8EC=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EC,sizeof(void*),0)));}
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
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,Cyc_Tcutil_warn(loc,((_tmp8EF="missing type within specifier",_tag_dyneither(_tmp8EF,sizeof(char),30))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmpF7=t;enum Cyc_Absyn_Sign _tmpF9;enum Cyc_Absyn_Size_of _tmpFA;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpF7;if(_tmpF8->tag != 6)goto _LL6E;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LL6D:
# 686
 if(_tmpF9 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmpFA);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp8F0;Cyc_Parse_err(((_tmp8F0="sign specification on non-integral type",_tag_dyneither(_tmp8F0,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmpFC=t;enum Cyc_Absyn_Sign _tmpFE;enum Cyc_Absyn_Size_of _tmpFF;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpFD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpFC;if(_tmpFD->tag != 6)goto _LL73;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}}_LL72:
# 694
 if(_tmpFF != sz)
t=Cyc_Absyn_int_typ(_tmpFE,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpFC;if(_tmp100->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp8F1;Cyc_Parse_err(((_tmp8F1="size qualifier on non-integral type",_tag_dyneither(_tmp8F1,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp8F4;struct Cyc_List_List*_tmp8F3;return(_tmp8F3=_region_malloc(r,sizeof(*_tmp8F3)),((_tmp8F3->hd=((_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=_tmp102,((_tmp8F4->f2=_tmp104,((_tmp8F4->f3=_tmp105,((_tmp8F4->f4=_tmp106,((_tmp8F4->f5=_tmp107,_tmp8F4)))))))))))),((_tmp8F3->tl=0,_tmp8F3)))));}else{
# 718
struct _tuple15*_tmp8F7;struct Cyc_List_List*_tmp8F6;return(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6->hd=((_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7->f1=_tmp102,((_tmp8F7->f2=_tmp104,((_tmp8F7->f3=_tmp105,((_tmp8F7->f4=_tmp106,((_tmp8F7->f5=_tmp107,_tmp8F7)))))))))))),((_tmp8F6->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8F6)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp8F8;return(_tmp8F8.f1=tq,((_tmp8F8.f2=t,((_tmp8F8.f3=0,((_tmp8F8.f4=atts,_tmp8F8)))))));}{
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
struct Cyc_List_List*_tmp8F9;fn_atts=((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->hd=(void*)as->hd,((_tmp8F9->tl=fn_atts,_tmp8F9))))));}else{
# 743
struct Cyc_List_List*_tmp8FA;new_atts=((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA->hd=(void*)as->hd,((_tmp8FA->tl=new_atts,_tmp8FA))))));}}}
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
const char*_tmp8FD;void*_tmp8FC;(_tmp8FC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp128,((_tmp8FD="function declaration without parameter types",_tag_dyneither(_tmp8FD,sizeof(char),45))),_tag_dyneither(_tmp8FC,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp117->tag != 4)goto _LL80;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp8FE;return(_tmp8FE.f1=tq,((_tmp8FE.f2=t,((_tmp8FE.f3=_tmp118,((_tmp8FE.f4=atts,_tmp8FE)))))));}{
# 802
const char*_tmp901;void*_tmp900;(_tmp900=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp119,((_tmp901="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp901,sizeof(char),68))),_tag_dyneither(_tmp900,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11A->tag != 2)goto _LL82;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp907;struct Cyc_Absyn_PtrInfo _tmp906;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp905;return Cyc_Parse_apply_tms(_tmp11C,(void*)((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905[0]=((_tmp907.tag=5,((_tmp907.f1=((_tmp906.elt_typ=t,((_tmp906.elt_tq=tq,((_tmp906.ptr_atts=_tmp11B,_tmp906)))))),_tmp907)))),_tmp905)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp11D=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp10D;if(_tmp11D->tag != 5)goto _LL77;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp90D;struct Cyc_Absyn_Typedefdecl*_tmp90C;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp90B;return Cyc_Absyn_new_decl((void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=9,((_tmp90D.f1=((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C->name=_tmp13D,((_tmp90C->tvs=_tmp140,((_tmp90C->kind=kind,((_tmp90C->defn=type,((_tmp90C->atts=_tmp141,((_tmp90C->tq=_tmp13E,_tmp90C)))))))))))))),_tmp90D)))),_tmp90B)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp910;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp90F;return Cyc_Absyn_new_stmt((void*)((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp910.tag=12,((_tmp910.f1=d,((_tmp910.f2=s,_tmp910)))))),_tmp90F)))),d->loc);}
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
const char*_tmp911;Cyc_Parse_err(((_tmp911="inline is allowed only on function definitions",_tag_dyneither(_tmp911,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp917;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp916;struct Cyc_List_List*_tmp915;struct Cyc_List_List*_tmp16C=(_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=Cyc_Absyn_new_decl((void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp916.tag=6,((_tmp916.f1=_tmp154,_tmp916)))),_tmp917)))),loc),((_tmp915->tl=0,_tmp915)))));_npop_handler(0);return _tmp16C;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp155->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp156=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp155->f1)->r;if(_tmp156->tag != 1)goto _LLA0;else{_tmp157=_tmp156->f1;}}}_LL9F:
# 910
 if(_tmp14E != 0){const char*_tmp918;Cyc_Parse_err(((_tmp918="attributes on enum not supported",_tag_dyneither(_tmp918,sizeof(char),33))),loc);}
_tmp157->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp91E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp91D;struct Cyc_List_List*_tmp91C;struct Cyc_List_List*_tmp171=(_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->hd=Cyc_Absyn_new_decl((void*)((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=((_tmp91D.tag=8,((_tmp91D.f1=_tmp157,_tmp91D)))),_tmp91E)))),loc),((_tmp91C->tl=0,_tmp91C)))));_npop_handler(0);return _tmp171;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp151;if(_tmp158->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp159=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp158->f1)->r;if(_tmp159->tag != 2)goto _LLA2;else{_tmp15A=_tmp159->f1;}}}_LLA1:
# 914
 if(_tmp14E != 0){const char*_tmp91F;Cyc_Parse_err(((_tmp91F="attributes on datatypes not supported",_tag_dyneither(_tmp91F,sizeof(char),38))),loc);}
_tmp15A->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp925;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp924;struct Cyc_List_List*_tmp923;struct Cyc_List_List*_tmp176=(_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923->hd=Cyc_Absyn_new_decl((void*)((_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925[0]=((_tmp924.tag=7,((_tmp924.f1=_tmp15A,_tmp924)))),_tmp925)))),loc),((_tmp923->tl=0,_tmp923)))));_npop_handler(0);return _tmp176;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp151;if(_tmp15B->tag != 11)goto _LLA4;else{if((((_tmp15B->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp15C=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f1;_tmp15D=((struct _tuple2)(((_tmp15B->f1).aggr_info).UnknownAggr).val).f2;_tmp15E=(_tmp15B->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15E);
struct Cyc_Absyn_Aggrdecl*_tmp926;struct Cyc_Absyn_Aggrdecl*_tmp178=(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->kind=_tmp15C,((_tmp926->sc=s,((_tmp926->name=_tmp15D,((_tmp926->tvs=_tmp177,((_tmp926->impl=0,((_tmp926->attributes=0,_tmp926)))))))))))));
if(_tmp14E != 0){const char*_tmp927;Cyc_Parse_err(((_tmp927="bad attributes on type declaration",_tag_dyneither(_tmp927,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp92D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp92C;struct Cyc_List_List*_tmp92B;struct Cyc_List_List*_tmp17D=(_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->hd=Cyc_Absyn_new_decl((void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92C.tag=6,((_tmp92C.f1=_tmp178,_tmp92C)))),_tmp92D)))),loc),((_tmp92B->tl=0,_tmp92B)))));_npop_handler(0);return _tmp17D;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp15F->tag != 3)goto _LLA6;else{if((((_tmp15F->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp160=(struct Cyc_Absyn_Datatypedecl**)(((_tmp15F->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp14E != 0){const char*_tmp92E;Cyc_Parse_err(((_tmp92E="bad attributes on datatype",_tag_dyneither(_tmp92E,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp934;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp933;struct Cyc_List_List*_tmp932;struct Cyc_List_List*_tmp183=(_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932->hd=Cyc_Absyn_new_decl((void*)((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=((_tmp933.tag=7,((_tmp933.f1=*_tmp160,_tmp933)))),_tmp934)))),loc),((_tmp932->tl=0,_tmp932)))));_npop_handler(0);return _tmp183;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp151;if(_tmp161->tag != 3)goto _LLA8;else{if((((_tmp161->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp162=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).name;_tmp163=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp161->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp164=(_tmp161->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp184=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp164);
struct Cyc_Absyn_Decl*_tmp185=Cyc_Absyn_datatype_decl(s,_tmp162,_tmp184,0,_tmp163,loc);
if(_tmp14E != 0){const char*_tmp935;Cyc_Parse_err(((_tmp935="bad attributes on datatype",_tag_dyneither(_tmp935,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp936;struct Cyc_List_List*_tmp188=(_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->hd=_tmp185,((_tmp936->tl=0,_tmp936)))));_npop_handler(0);return _tmp188;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp151;if(_tmp165->tag != 13)goto _LLAA;else{_tmp166=_tmp165->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp937;struct Cyc_Absyn_Enumdecl*_tmp189=(_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->sc=s,((_tmp937->name=_tmp166,((_tmp937->fields=0,_tmp937)))))));
if(_tmp14E != 0){const char*_tmp938;Cyc_Parse_err(((_tmp938="bad attributes on enum",_tag_dyneither(_tmp938,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp942;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp941;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp940;struct Cyc_List_List*_tmp93F;struct Cyc_List_List*_tmp18F=(_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->hd=((_tmp942=_cycalloc(sizeof(*_tmp942)),((_tmp942->r=(void*)((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=8,((_tmp941.f1=_tmp189,_tmp941)))),_tmp940)))),((_tmp942->loc=loc,_tmp942)))))),((_tmp93F->tl=0,_tmp93F)))));_npop_handler(0);return _tmp18F;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp151;if(_tmp167->tag != 14)goto _LLAC;else{_tmp168=_tmp167->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp945;struct Cyc_Absyn_Enumdecl*_tmp944;struct Cyc_Absyn_Enumdecl*_tmp191=(_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944->sc=s,((_tmp944->name=Cyc_Parse_gensym_enum(),((_tmp944->fields=((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945->v=_tmp168,_tmp945)))),_tmp944)))))));
if(_tmp14E != 0){const char*_tmp946;Cyc_Parse_err(((_tmp946="bad attributes on enum",_tag_dyneither(_tmp946,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp950;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp94F;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94E;struct Cyc_List_List*_tmp94D;struct Cyc_List_List*_tmp197=(_tmp94D=_cycalloc(sizeof(*_tmp94D)),((_tmp94D->hd=((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950->r=(void*)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp94F.tag=8,((_tmp94F.f1=_tmp191,_tmp94F)))),_tmp94E)))),((_tmp950->loc=loc,_tmp950)))))),((_tmp94D->tl=0,_tmp94D)))));_npop_handler(0);return _tmp197;};}_LLAC:;_LLAD:
{const char*_tmp951;Cyc_Parse_err(((_tmp951="missing declarator",_tag_dyneither(_tmp951,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp19B=0;_npop_handler(0);return _tmp19B;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp19C=Cyc_Parse_apply_tmss(mkrgn,_tmp14C,_tmp150,declarators,_tmp14E);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp952;Cyc_Parse_err(((_tmp952="initializer in typedef declaration",_tag_dyneither(_tmp952,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp19C);
struct Cyc_List_List*_tmp19E=decls;_npop_handler(0);return _tmp19E;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp19F=_tmp19C;for(0;_tmp19F != 0;(_tmp19F=_tmp19F->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_stmttmp15=(struct _tuple15*)_tmp19F->hd;struct _tuple0*_tmp1A1;struct Cyc_Absyn_Tqual _tmp1A2;void*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct _tuple15*_tmp1A0=_stmttmp15;_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;_tmp1A3=_tmp1A0->f3;_tmp1A4=_tmp1A0->f4;_tmp1A5=_tmp1A0->f5;
if(_tmp1A4 != 0){
const char*_tmp955;void*_tmp954;(_tmp954=0,Cyc_Tcutil_warn(loc,((_tmp955="bad type params, ignoring",_tag_dyneither(_tmp955,sizeof(char),26))),_tag_dyneither(_tmp954,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp958;void*_tmp957;(_tmp957=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp958="unexpected NULL in parse!",_tag_dyneither(_tmp958,sizeof(char),26))),_tag_dyneither(_tmp957,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1AA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1AB=Cyc_Absyn_new_vardecl(_tmp1A1,_tmp1A3,_tmp1AA);
_tmp1AB->tq=_tmp1A2;
_tmp1AB->sc=s;
_tmp1AB->attributes=_tmp1A5;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp95E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp95D;struct Cyc_Absyn_Decl*_tmp95C;struct Cyc_Absyn_Decl*_tmp1AC=(_tmp95C=_cycalloc(sizeof(*_tmp95C)),((_tmp95C->r=(void*)((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95D.tag=0,((_tmp95D.f1=_tmp1AB,_tmp95D)))),_tmp95E)))),((_tmp95C->loc=loc,_tmp95C)))));
struct Cyc_List_List*_tmp95F;decls=((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=_tmp1AC,((_tmp95F->tl=decls,_tmp95F))))));};};}}{
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
{const char*_tmp964;void*_tmp963[2];struct Cyc_String_pa_PrintArg_struct _tmp962;struct Cyc_Int_pa_PrintArg_struct _tmp961;Cyc_Parse_err((struct _dyneither_ptr)((_tmp961.tag=1,((_tmp961.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp963[0]=& _tmp962,((_tmp963[1]=& _tmp961,Cyc_aprintf(((_tmp964="bad kind: %s; strlen=%d",_tag_dyneither(_tmp964,sizeof(char),24))),_tag_dyneither(_tmp963,sizeof(void*),2)))))))))))))),loc);}
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
{const char*_tmp965;Cyc_Parse_err(((_tmp965="cannot mix patterns and expressions in case",_tag_dyneither(_tmp965,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp966;return((_tmp966.Int_tok).val=yy1,(((_tmp966.Int_tok).tag=1,_tmp966)));}static char _tmp1D3[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp967;return((_tmp967.Char_tok).val=yy1,(((_tmp967.Char_tok).tag=2,_tmp967)));}static char _tmp1D8[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp968;return((_tmp968.String_tok).val=yy1,(((_tmp968.String_tok).tag=3,_tmp968)));}static char _tmp1DD[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp969;return((_tmp969.YY1).val=yy1,(((_tmp969.YY1).tag=7,_tmp969)));}static char _tmp1E2[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp96A;return((_tmp96A.YY2).val=yy1,(((_tmp96A.YY2).tag=8,_tmp96A)));}static char _tmp1E7[28]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.YY3).val=yy1,(((_tmp96B.YY3).tag=9,_tmp96B)));}static char _tmp1EC[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.YY4).val=yy1,(((_tmp96C.YY4).tag=10,_tmp96C)));}static char _tmp1F1[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.YY5).val=yy1,(((_tmp96D.YY5).tag=11,_tmp96D)));}static char _tmp1F6[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY6).val=yy1,(((_tmp96E.YY6).tag=12,_tmp96E)));}static char _tmp1FB[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY7).val=yy1,(((_tmp96F.YY7).tag=13,_tmp96F)));}static char _tmp200[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY8).val=yy1,(((_tmp970.YY8).tag=14,_tmp970)));}static char _tmp205[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY9).val=yy1,(((_tmp971.YY9).tag=15,_tmp971)));}static char _tmp20A[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.QualId_tok).val=yy1,(((_tmp972.QualId_tok).tag=5,_tmp972)));}static char _tmp20F[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY10).val=yy1,(((_tmp973.YY10).tag=16,_tmp973)));}static char _tmp214[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY11).val=yy1,(((_tmp974.YY11).tag=17,_tmp974)));}static char _tmp219[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY12).val=yy1,(((_tmp975.YY12).tag=18,_tmp975)));}static char _tmp21E[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY13).val=yy1,(((_tmp976.YY13).tag=19,_tmp976)));}static char _tmp223[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.YY14).val=yy1,(((_tmp977.YY14).tag=20,_tmp977)));}static char _tmp228[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY15).val=yy1,(((_tmp978.YY15).tag=21,_tmp978)));}static char _tmp22D[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY16).val=yy1,(((_tmp979.YY16).tag=22,_tmp979)));}static char _tmp232[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY17).val=yy1,(((_tmp97A.YY17).tag=23,_tmp97A)));}static char _tmp237[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY18).val=yy1,(((_tmp97B.YY18).tag=24,_tmp97B)));}static char _tmp23C[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY19).val=yy1,(((_tmp97C.YY19).tag=25,_tmp97C)));}static char _tmp241[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY20).val=yy1,(((_tmp97D.YY20).tag=26,_tmp97D)));}static char _tmp246[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY21).val=yy1,(((_tmp97E.YY21).tag=27,_tmp97E)));}static char _tmp24B[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY22).val=yy1,(((_tmp97F.YY22).tag=28,_tmp97F)));}static char _tmp250[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY23).val=yy1,(((_tmp980.YY23).tag=29,_tmp980)));}static char _tmp255[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY24).val=yy1,(((_tmp981.YY24).tag=30,_tmp981)));}static char _tmp25A[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY25).val=yy1,(((_tmp982.YY25).tag=31,_tmp982)));}static char _tmp25F[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY26).val=yy1,(((_tmp983.YY26).tag=32,_tmp983)));}static char _tmp264[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY27).val=yy1,(((_tmp984.YY27).tag=33,_tmp984)));}static char _tmp269[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY28).val=yy1,(((_tmp985.YY28).tag=34,_tmp985)));}static char _tmp26E[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY29).val=yy1,(((_tmp986.YY29).tag=35,_tmp986)));}static char _tmp273[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY30).val=yy1,(((_tmp987.YY30).tag=36,_tmp987)));}static char _tmp278[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
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
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY31).val=yy1,(((_tmp988.YY31).tag=37,_tmp988)));}static char _tmp27D[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
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
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY32).val=yy1,(((_tmp989.YY32).tag=38,_tmp989)));}static char _tmp282[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY33).val=yy1,(((_tmp98A.YY33).tag=39,_tmp98A)));}static char _tmp287[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY34).val=yy1,(((_tmp98B.YY34).tag=40,_tmp98B)));}static char _tmp28C[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY35).val=yy1,(((_tmp98C.YY35).tag=41,_tmp98C)));}static char _tmp291[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY36).val=yy1,(((_tmp98D.YY36).tag=42,_tmp98D)));}static char _tmp296[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.YY37).val=yy1,(((_tmp98E.YY37).tag=43,_tmp98E)));}static char _tmp29B[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp98F;return((_tmp98F.YY38).val=yy1,(((_tmp98F.YY38).tag=44,_tmp98F)));}static char _tmp2A0[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.YY39).val=yy1,(((_tmp990.YY39).tag=45,_tmp990)));}static char _tmp2A5[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.YY40).val=yy1,(((_tmp991.YY40).tag=46,_tmp991)));}static char _tmp2AA[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp992;return((_tmp992.YY41).val=yy1,(((_tmp992.YY41).tag=47,_tmp992)));}static char _tmp2AF[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp993;return((_tmp993.YY42).val=yy1,(((_tmp993.YY42).tag=48,_tmp993)));}static char _tmp2B4[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.YY43).val=yy1,(((_tmp994.YY43).tag=49,_tmp994)));}static char _tmp2B9[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.YY44).val=yy1,(((_tmp995.YY44).tag=50,_tmp995)));}static char _tmp2BE[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.YY45).val=yy1,(((_tmp996.YY45).tag=51,_tmp996)));}static char _tmp2C3[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY46).val=yy1,(((_tmp997.YY46).tag=52,_tmp997)));}static char _tmp2C8[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY47).val=yy1,(((_tmp998.YY47).tag=53,_tmp998)));}static char _tmp2CD[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY48).val=yy1,(((_tmp999.YY48).tag=54,_tmp999)));}static char _tmp2D2[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY49).val=yy1,(((_tmp99A.YY49).tag=55,_tmp99A)));}static char _tmp2D7[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY50).val=yy1,(((_tmp99B.YY50).tag=56,_tmp99B)));}static char _tmp2DC[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY51).val=yy1,(((_tmp99C.YY51).tag=57,_tmp99C)));}static char _tmp2E1[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY52).val=yy1,(((_tmp99D.YY52).tag=58,_tmp99D)));}static char _tmp2E6[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY53).val=yy1,(((_tmp99E.YY53).tag=59,_tmp99E)));}static char _tmp2EB[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY54).val=yy1,(((_tmp99F.YY54).tag=60,_tmp99F)));}static char _tmp2F0[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY55).val=yy1,(((_tmp9A0.YY55).tag=61,_tmp9A0)));}static char _tmp2F5[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY56).val=yy1,(((_tmp9A1.YY56).tag=62,_tmp9A1)));}static char _tmp2FA[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY57).val=yy1,(((_tmp9A2.YY57).tag=63,_tmp9A2)));}static char _tmp2FF[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.Asm_tok).val=yy1,(((_tmp9A3.Asm_tok).tag=6,_tmp9A3)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1185
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1189
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9A4;return(_tmp9A4.timestamp=0,((_tmp9A4.first_line=0,((_tmp9A4.first_column=0,((_tmp9A4.last_line=0,((_tmp9A4.last_column=0,_tmp9A4)))))))));}
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
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E1[8]="stdcall";static char _tmp4E2[6]="cdecl";static char _tmp4E3[9]="fastcall";static char _tmp4E4[9]="noreturn";static char _tmp4E5[6]="const";static char _tmp4E6[8]="aligned";static char _tmp4E7[7]="packed";static char _tmp4E8[7]="shared";static char _tmp4E9[7]="unused";static char _tmp4EA[5]="weak";static char _tmp4EB[10]="dllimport";static char _tmp4EC[10]="dllexport";static char _tmp4ED[23]="no_instrument_function";static char _tmp4EE[12]="constructor";static char _tmp4EF[11]="destructor";static char _tmp4F0[22]="no_check_memory_usage";static char _tmp4F1[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9AC(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9AB,unsigned int*_tmp9AA,union Cyc_YYSTYPE**_tmp9A9){for(*_tmp9AB=0;*_tmp9AB < *_tmp9AA;(*_tmp9AB)++){(*_tmp9A9)[*_tmp9AB]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9B1(unsigned int*_tmp9B0,unsigned int*_tmp9AF,struct Cyc_Yyltype**_tmp9AE){for(*_tmp9B0=0;*_tmp9B0 < *_tmp9AF;(*_tmp9B0)++){(*_tmp9AE)[*_tmp9B0]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9B8(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9B7,unsigned int*_tmp9B6,short**_tmp9B4){for(*_tmp9B7=0;*_tmp9B7 < *_tmp9B6;(*_tmp9B7)++){(*_tmp9B4)[*_tmp9B7]=(short)(
# 216
*_tmp9B7 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9B7)): 0);}}static void _tmp9BE(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9BD,unsigned int*_tmp9BC,union Cyc_YYSTYPE**_tmp9BA){for(*_tmp9BD=0;*_tmp9BD < *_tmp9BC;(*_tmp9BD)++){(*_tmp9BA)[*_tmp9BD]=(union Cyc_YYSTYPE)(
# 219
*_tmp9BD <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9BD)):*yylval);}}static void _tmp9C4(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9C3,unsigned int*_tmp9C2,struct Cyc_Yyltype**_tmp9C0){for(*_tmp9C3=0;*_tmp9C3 < *_tmp9C2;(*_tmp9C3)++){(*_tmp9C0)[*_tmp9C3]=(struct Cyc_Yyltype)(
# 222
*_tmp9C3 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9C3)):
 Cyc_yynewloc());}}static void _tmpEB2(unsigned int*_tmpEB1,unsigned int*_tmpEB0,char**_tmpEAE){for(*_tmpEB1=0;*_tmpEB1 < *_tmpEB0;(*_tmpEB1)++){(*_tmpEAE)[*_tmpEB1]=(char)'\000';}}
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
union Cyc_YYSTYPE _tmp9A5;union Cyc_YYSTYPE yylval=((_tmp9A5.YYINITIALSVAL).val=0,(((_tmp9A5.YYINITIALSVAL).tag=64,_tmp9A5)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9A7;unsigned int _tmp9A6;struct _dyneither_ptr yyss=(_tmp9A6=200,((_tmp9A7=_region_calloc(yyregion,sizeof(short),_tmp9A6),_tag_dyneither(_tmp9A7,sizeof(short),_tmp9A6))));
# 158
int yyvsp_offset;
unsigned int _tmp9AB;unsigned int _tmp9AA;union Cyc_YYSTYPE*_tmp9A9;unsigned int _tmp9A8;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9A8=(unsigned int)200,((_tmp9A9=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9A8)),((((_tmp9AA=_tmp9A8,_tmp9AC(& yylval,& _tmp9AB,& _tmp9AA,& _tmp9A9))),_tmp9A9)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9B0;unsigned int _tmp9AF;struct Cyc_Yyltype*_tmp9AE;unsigned int _tmp9AD;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9AD=(unsigned int)200,((_tmp9AE=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9AD)),((((_tmp9AF=_tmp9AD,_tmp9B1(& _tmp9B0,& _tmp9AF,& _tmp9AE))),_tmp9AE)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9B2;Cyc_yyerror(((_tmp9B2="parser stack overflow",_tag_dyneither(_tmp9B2,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9B7;unsigned int _tmp9B6;struct _dyneither_ptr _tmp9B5;short*_tmp9B4;unsigned int _tmp9B3;struct _dyneither_ptr yyss1=(_tmp9B3=(unsigned int)yystacksize,((_tmp9B4=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9B3)),((_tmp9B5=_tag_dyneither(_tmp9B4,sizeof(short),_tmp9B3),((((_tmp9B6=_tmp9B3,_tmp9B8(& yyssp_offset,& yyss,& _tmp9B7,& _tmp9B6,& _tmp9B4))),_tmp9B5)))))));
# 217
unsigned int _tmp9BD;unsigned int _tmp9BC;struct _dyneither_ptr _tmp9BB;union Cyc_YYSTYPE*_tmp9BA;unsigned int _tmp9B9;struct _dyneither_ptr yyvs1=
(_tmp9B9=(unsigned int)yystacksize,((_tmp9BA=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9B9)),((_tmp9BB=_tag_dyneither(_tmp9BA,sizeof(union Cyc_YYSTYPE),_tmp9B9),((((_tmp9BC=_tmp9B9,_tmp9BE(& yyvs,& yyssp_offset,& yylval,& _tmp9BD,& _tmp9BC,& _tmp9BA))),_tmp9BB)))))));
# 221
unsigned int _tmp9C3;unsigned int _tmp9C2;struct _dyneither_ptr _tmp9C1;struct Cyc_Yyltype*_tmp9C0;unsigned int _tmp9BF;struct _dyneither_ptr yyls1=(_tmp9BF=(unsigned int)yystacksize,((_tmp9C0=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9BF)),((_tmp9C1=_tag_dyneither(_tmp9C0,sizeof(struct Cyc_Yyltype),_tmp9BF),((((_tmp9C2=_tmp9BF,_tmp9C4(& yyssp_offset,& yyls,& _tmp9C3,& _tmp9C2,& _tmp9C0))),_tmp9C1)))))));
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
{struct Cyc_Absyn_Decl*_tmp9CE;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9CD;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9CC;struct Cyc_List_List*_tmp9CB;yyval=Cyc_YY19(((_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((_tmp9CB->hd=((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE->r=(void*)((_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=11,((_tmp9CD.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9CD.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9CD)))))),_tmp9CC)))),((_tmp9CE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9CE)))))),((_tmp9CB->tl=0,_tmp9CB)))))));}
Cyc_Lex_leave_using();
# 1195
break;}case 4: _LL21C: {
# 1197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1196 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9D8;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9D7;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9D6;struct Cyc_List_List*_tmp9D5;yyval=Cyc_YY19(((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->hd=((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8->r=(void*)((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=11,((_tmp9D7.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9D7.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D7)))))),_tmp9D6)))),((_tmp9D8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D8)))))),((_tmp9D5->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9D5)))))));}
break;}case 5: _LL21D: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1199 "parse.y"
{struct Cyc_Absyn_Decl*_tmp9E7;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9E6;struct _dyneither_ptr*_tmp9E5;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E4;struct Cyc_List_List*_tmp9E3;yyval=Cyc_YY19(((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3->hd=((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7->r=(void*)((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E6.tag=10,((_tmp9E6.f1=((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9E5)))),((_tmp9E6.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9E6)))))),_tmp9E4)))),((_tmp9E7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9E7)))))),((_tmp9E3->tl=0,_tmp9E3)))))));}
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
{const char*_tmp9E8;nspace=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1)));}goto _LL220;_LL223: _tmp459=_tmp457;_LL224:(void)_throw(_tmp459);_LL220:;}};}
# 1209
{struct _handler_cons _tmp45B;_push_handler(& _tmp45B);{int _tmp45D=0;if(setjmp(_tmp45B.handler))_tmp45D=1;if(!_tmp45D){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp45C=(void*)_exn_thrown;void*_tmp45F=_tmp45C;void*_tmp461;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp460=(struct Cyc_Core_Failure_exn_struct*)_tmp45F;if(_tmp460->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp461=_tmp45F;_LL229:(void)_throw(_tmp461);_LL225:;}};}
# 1212
{struct _handler_cons _tmp462;_push_handler(& _tmp462);{int _tmp464=0;if(setjmp(_tmp462.handler))_tmp464=1;if(!_tmp464){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp463=(void*)_exn_thrown;void*_tmp466=_tmp463;void*_tmp468;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp467=(struct Cyc_Core_Failure_exn_struct*)_tmp466;if(_tmp467->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp468=_tmp466;_LL22E:(void)_throw(_tmp468);_LL22A:;}};}
# 1215
{struct Cyc_Absyn_Decl*_tmp9F7;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9F6;struct _dyneither_ptr*_tmp9F5;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F4;struct Cyc_List_List*_tmp9F3;yyval=Cyc_YY19(((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3->hd=((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7->r=(void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F6.tag=10,((_tmp9F6.f1=((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=nspace,_tmp9F5)))),((_tmp9F6.f2=x,_tmp9F6)))))),_tmp9F4)))),((_tmp9F7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9F7)))))),((_tmp9F3->tl=y,_tmp9F3)))))));}
# 1217
break;}case 7: _LL21F: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1218 "parse.y"
struct _dyneither_ptr _tmp46E=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9F8;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46E,((_tmp9F8="C",_tag_dyneither(_tmp9F8,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA02;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA01;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA00;struct Cyc_List_List*_tmp9FF;yyval=Cyc_YY19(((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF->hd=((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->r=(void*)((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=12,((_tmpA01.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA01)))),_tmpA00)))),((_tmpA02->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA02)))))),((_tmp9FF->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmp9FF)))))));}else{
# 1222
{const char*_tmpA03;if(Cyc_strcmp((struct _dyneither_ptr)_tmp46E,((_tmpA03="C include",_tag_dyneither(_tmpA03,sizeof(char),10))))!= 0){
const char*_tmpA04;Cyc_Parse_err(((_tmpA04="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA04,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1225
struct Cyc_Absyn_Decl*_tmpA0E;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA0D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA0C;struct Cyc_List_List*_tmpA0B;yyval=Cyc_YY19(((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->hd=((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->r=(void*)((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0D.tag=13,((_tmpA0D.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA0D.f2=0,_tmpA0D)))))),_tmpA0C)))),((_tmpA0E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0E)))))),((_tmpA0B->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA0B)))))));};}}
# 1228
break;}case 8: _LL22F: {
# 1230
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1232
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1229 "parse.y"
{const char*_tmpA0F;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA0F="C include",_tag_dyneither(_tmpA0F,sizeof(char),10))))!= 0){
const char*_tmpA10;Cyc_Parse_err(((_tmpA10="expecting \"C include\"",_tag_dyneither(_tmpA10,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA1A;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA19;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA18;struct Cyc_List_List*_tmpA17;yyval=Cyc_YY19(((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17->hd=((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A->r=(void*)((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA19.tag=13,((_tmpA19.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA19.f2=exs,_tmpA19)))))),_tmpA18)))),((_tmpA1A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1A)))))),((_tmpA17->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA17)))))));}
# 1234
break;};}case 9: _LL230: {
# 1236
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1238
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1235 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1D;struct Cyc_List_List*_tmpA1C;yyval=Cyc_YY19(((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->hd=((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA1D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1D)))))),((_tmpA1C->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1C)))))));}
break;}case 10: _LL231: {
# 1238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1237 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA20;struct Cyc_List_List*_tmpA1F;yyval=Cyc_YY19(((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->hd=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA20->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA20)))))),((_tmpA1F->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1F)))))));}
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
{struct _tuple27*_tmpA23;struct Cyc_List_List*_tmpA22;yyval=Cyc_YY55(((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->hd=((_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA23->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA23->f3=0,_tmpA23)))))))),((_tmpA22->tl=0,_tmpA22)))))));}
break;}case 15: _LL236: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1248 "parse.y"
{struct _tuple27*_tmpA26;struct Cyc_List_List*_tmpA25;yyval=Cyc_YY55(((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->hd=((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA26->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA26->f3=0,_tmpA26)))))))),((_tmpA25->tl=0,_tmpA25)))))));}
break;}case 16: _LL237: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct _tuple27*_tmpA29;struct Cyc_List_List*_tmpA28;yyval=Cyc_YY55(((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->hd=((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA29->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA29->f3=0,_tmpA29)))))))),((_tmpA28->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA28)))))));}
break;}case 17: _LL238: {
# 1253
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1254 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA2F;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA2E;struct Cyc_List_List*_tmpA2D;yyval=Cyc_YY19(((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->hd=Cyc_Absyn_new_decl((void*)((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA2E.tag=1,((_tmpA2E.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA2E)))),_tmpA2F)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA2D->tl=0,_tmpA2D)))))));}
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
{struct _dyneither_ptr*_tmpA30;Cyc_Lex_enter_namespace(((_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA30)))));}yyval=yyyyvsp[1];
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
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 34: _LL247: {
# 1319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1319 "parse.y"
{struct Cyc_List_List*_tmpA31;yyval=Cyc_YY19(((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA31->tl=0,_tmpA31)))))));}
break;}case 35: _LL248: {
# 1322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1321 "parse.y"
struct Cyc_List_List*_tmp494=0;
{struct Cyc_List_List*_tmp495=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp495 != 0;_tmp495=_tmp495->tl){
struct _dyneither_ptr*_tmp496=(struct _dyneither_ptr*)_tmp495->hd;
struct _tuple0*_tmpA32;struct _tuple0*qv=(_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->f1=Cyc_Absyn_Rel_n(0),((_tmpA32->f2=_tmp496,_tmpA32)))));
struct Cyc_Absyn_Vardecl*_tmp497=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA33;_tmp494=((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33->hd=_tmp497,((_tmpA33->tl=_tmp494,_tmpA33))))));}}
# 1328
_tmp494=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp494);
{struct Cyc_List_List*_tmpA34;yyval=Cyc_YY19(((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=Cyc_Absyn_letv_decl(_tmp494,(unsigned int)(yyyylsp[0]).first_line),((_tmpA34->tl=0,_tmpA34)))))));}
# 1331
break;}case 36: _LL249: {
# 1333
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1335
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1334 "parse.y"
struct _dyneither_ptr _tmp49B=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA35;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49B,((_tmpA35="`H",_tag_dyneither(_tmpA35,sizeof(char),3))))== 0){
const char*_tmpA38;void*_tmpA37;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA37=0,Cyc_aprintf(((_tmpA38="bad occurrence of heap region",_tag_dyneither(_tmpA38,sizeof(char),30))),_tag_dyneither(_tmpA37,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA39;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp49B,((_tmpA39="`U",_tag_dyneither(_tmpA39,sizeof(char),3))))== 0){
const char*_tmpA3C;void*_tmpA3B;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA3B=0,Cyc_aprintf(((_tmpA3C="bad occurrence of unique region",_tag_dyneither(_tmpA3C,sizeof(char),32))),_tag_dyneither(_tmpA3B,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA3F;struct Cyc_Absyn_Tvar*_tmpA3E;struct Cyc_Absyn_Tvar*tv=(_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E->name=((_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=_tmp49B,_tmpA3F)))),((_tmpA3E->identity=- 1,((_tmpA3E->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA3E)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA42;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA41;void*t=(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=2,((_tmpA42.f1=tv,_tmpA42)))),_tmpA41))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA48;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA47;struct _dyneither_ptr*_tmpA45;struct _tuple0*_tmpA44;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->f1=Cyc_Absyn_Loc_n,((_tmpA44->f2=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA45)))),_tmpA44)))))),(void*)((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=15,((_tmpA48.f1=t,_tmpA48)))),_tmpA47)))),0);
{struct Cyc_List_List*_tmpA49;yyval=Cyc_YY19(((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA49->tl=0,_tmpA49)))))));}
# 1344
break;};}case 37: _LL24A: {
# 1346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1346 "parse.y"
int _tmp4AB=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4AC=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA4A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AC,((_tmpA4A="H",_tag_dyneither(_tmpA4A,sizeof(char),2))))== 0){
const char*_tmpA4B;Cyc_Parse_err(((_tmpA4B="bad occurrence of heap region `H",_tag_dyneither(_tmpA4B,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA4C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4AC,((_tmpA4C="U",_tag_dyneither(_tmpA4C,sizeof(char),2))))== 0){
const char*_tmpA4D;Cyc_Parse_err(((_tmpA4D="bad occurrence of unique region `U",_tag_dyneither(_tmpA4D,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4AB  && _tmp4AD != 0){
const char*_tmpA4E;Cyc_Parse_err(((_tmpA4E="open regions cannot be @resetable",_tag_dyneither(_tmpA4E,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA5B;const char*_tmpA5A;void*_tmpA59[1];struct Cyc_String_pa_PrintArg_struct _tmpA58;struct Cyc_Absyn_Tvar*_tmpA57;struct Cyc_Absyn_Tvar*tv=(_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->name=((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=(struct _dyneither_ptr)((_tmpA58.tag=0,((_tmpA58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4AC),((_tmpA59[0]=& _tmpA58,Cyc_aprintf(((_tmpA5A="`%s",_tag_dyneither(_tmpA5A,sizeof(char),4))),_tag_dyneither(_tmpA59,sizeof(void*),1)))))))),_tmpA5B)))),((_tmpA57->identity=- 1,((_tmpA57->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA57)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA5E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA5D;void*t=(void*)((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5E.tag=2,((_tmpA5E.f1=tv,_tmpA5E)))),_tmpA5D))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA64;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA63;struct _dyneither_ptr*_tmpA61;struct _tuple0*_tmpA60;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->f1=Cyc_Absyn_Loc_n,((_tmpA60->f2=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=_tmp4AC,_tmpA61)))),_tmpA60)))))),(void*)((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=15,((_tmpA64.f1=t,_tmpA64)))),_tmpA63)))),0);
# 1360
{struct Cyc_List_List*_tmpA65;yyval=Cyc_YY19(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4AB,_tmp4AD,(unsigned int)(yyyylsp[0]).first_line),((_tmpA65->tl=0,_tmpA65)))))));}
# 1362
break;};}case 38: _LL24B: {
# 1364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1364 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA6C;struct _dyneither_ptr*_tmpA6B;struct Cyc_Absyn_Tvar*_tmpA6A;struct Cyc_Absyn_Tvar*tv=(_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->name=((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA6B)))),((_tmpA6A->identity=- 1,((_tmpA6A->kind=(void*)((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6C.tag=0,((_tmpA6C.f1=& Cyc_Tcutil_rk,_tmpA6C)))),_tmpA6D)))),_tmpA6A)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA70;void*_tmpA6F;(_tmpA6F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA70="too many declarations in alias",_tag_dyneither(_tmpA70,sizeof(char),31))),_tag_dyneither(_tmpA6F,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_stmttmp35=d->r;void*_tmp4C1=_stmttmp35;struct Cyc_Absyn_Vardecl*_tmp4C3;_LL24E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4C2->tag != 0)goto _LL250;else{_tmp4C3=_tmp4C2->f1;}}_LL24F:
# 1371
{struct Cyc_List_List*_tmpA71;yyval=Cyc_YY19(((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71->hd=Cyc_Absyn_alias_decl(tv,_tmp4C3,(unsigned int)(yyyylsp[0]).first_line),((_tmpA71->tl=0,_tmpA71)))))));}
goto _LL24D;_LL250:;_LL251:
# 1374
{const char*_tmpA72;Cyc_Parse_err(((_tmpA72="expecting variable declaration",_tag_dyneither(_tmpA72,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY19(0);_LL24D:;}
# 1378
break;};}case 39: _LL24C:
# 1380
 yyval=Cyc_YY34(0);
break;case 40: _LL252: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1382 "parse.y"
yyval=Cyc_YY34(1);
break;}case 41: _LL253: {
# 1385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1387 "parse.y"
yyval=yyyyvsp[0];
break;}case 42: _LL254: {
# 1390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1389 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 43: _LL255: {
# 1392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpA73;yyval=Cyc_YY20(((_tmpA73.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA73.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA73.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA73.is_inline=0,((_tmpA73.attributes=0,_tmpA73)))))))))));}
break;}case 44: _LL256: {
# 1399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1398 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4CB=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4CB.sc != 0){
const char*_tmpA76;void*_tmpA75;(_tmpA75=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA76="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA76,sizeof(char),51))),_tag_dyneither(_tmpA75,sizeof(void*),0)));}
# 1402
{struct Cyc_Parse_Declaration_spec _tmpA77;yyval=Cyc_YY20(((_tmpA77.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA77.tq=_tmp4CB.tq,((_tmpA77.type_specs=_tmp4CB.type_specs,((_tmpA77.is_inline=_tmp4CB.is_inline,((_tmpA77.attributes=_tmp4CB.attributes,_tmpA77)))))))))));}
# 1406
break;}case 45: _LL257: {
# 1408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1407 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA78;yyval=Cyc_YY20(((_tmpA78.sc=0,((_tmpA78.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA78.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA78.is_inline=0,((_tmpA78.attributes=0,_tmpA78)))))))))));}
break;}case 46: _LL258: {
# 1411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1410 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D0=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA79;yyval=Cyc_YY20(((_tmpA79.sc=_tmp4D0.sc,((_tmpA79.tq=_tmp4D0.tq,((_tmpA79.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4D0.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA79.is_inline=_tmp4D0.is_inline,((_tmpA79.attributes=_tmp4D0.attributes,_tmpA79)))))))))));}
# 1416
break;}case 47: _LL259: {
# 1418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1417 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7A;yyval=Cyc_YY20(((_tmpA7A.sc=0,((_tmpA7A.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA7A.type_specs=Cyc_Parse_empty_spec(0),((_tmpA7A.is_inline=0,((_tmpA7A.attributes=0,_tmpA7A)))))))))));}
break;}case 48: _LL25A: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D3=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7B;yyval=Cyc_YY20(((_tmpA7B.sc=_tmp4D3.sc,((_tmpA7B.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4D3.tq),((_tmpA7B.type_specs=_tmp4D3.type_specs,((_tmpA7B.is_inline=_tmp4D3.is_inline,((_tmpA7B.attributes=_tmp4D3.attributes,_tmpA7B)))))))))));}
# 1424
break;}case 49: _LL25B: {
# 1426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1425 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7C;yyval=Cyc_YY20(((_tmpA7C.sc=0,((_tmpA7C.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7C.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7C.is_inline=1,((_tmpA7C.attributes=0,_tmpA7C)))))))))));}
break;}case 50: _LL25C: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1428 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D6=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7D;yyval=Cyc_YY20(((_tmpA7D.sc=_tmp4D6.sc,((_tmpA7D.tq=_tmp4D6.tq,((_tmpA7D.type_specs=_tmp4D6.type_specs,((_tmpA7D.is_inline=1,((_tmpA7D.attributes=_tmp4D6.attributes,_tmpA7D)))))))))));}
# 1432
break;}case 51: _LL25D: {
# 1434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1433 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA7E;yyval=Cyc_YY20(((_tmpA7E.sc=0,((_tmpA7E.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7E.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA7E.is_inline=0,((_tmpA7E.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpA7E)))))))))));}
break;}case 52: _LL25E: {
# 1437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1436 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D9=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7F;yyval=Cyc_YY20(((_tmpA7F.sc=_tmp4D9.sc,((_tmpA7F.tq=_tmp4D9.tq,((_tmpA7F.type_specs=_tmp4D9.type_specs,((_tmpA7F.is_inline=_tmp4D9.is_inline,((_tmpA7F.attributes=
# 1439
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4D9.attributes),_tmpA7F)))))))))));}
break;}case 53: _LL25F: {
# 1442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1443 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 54: _LL260: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1445 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 55: _LL261: {
# 1449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1447 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 56: _LL262: {
# 1451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1449 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 57: _LL263: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1452 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA80;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA80="C",_tag_dyneither(_tmpA80,sizeof(char),2))))!= 0){
const char*_tmpA81;Cyc_Parse_err(((_tmpA81="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA81,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1457
break;}case 58: _LL264: {
# 1459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1457 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 59: _LL265: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1460 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 60: _LL266:
# 1464
 yyval=Cyc_YY48(0);
break;case 61: _LL267: {
# 1467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1467 "parse.y"
yyval=yyyyvsp[0];
break;}case 62: _LL268: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1472 "parse.y"
yyval=yyyyvsp[3];
break;}case 63: _LL269: {
# 1475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1476 "parse.y"
{struct Cyc_List_List*_tmpA82;yyval=Cyc_YY48(((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA82->tl=0,_tmpA82)))))));}
break;}case 64: _LL26A: {
# 1479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1477 "parse.y"
{struct Cyc_List_List*_tmpA83;yyval=Cyc_YY48(((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA83->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpA83)))))));}
break;}case 65: _LL26B: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1482 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4E1,_tmp4E1,_tmp4E1 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4E2,_tmp4E2,_tmp4E2 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4E3,_tmp4E3,_tmp4E3 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4E4,_tmp4E4,_tmp4E4 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4E5,_tmp4E5,_tmp4E5 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4E6,_tmp4E6,_tmp4E6 + 8},(void*)& att_aligned},{{_tmp4E7,_tmp4E7,_tmp4E7 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4E8,_tmp4E8,_tmp4E8 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4E9,_tmp4E9,_tmp4E9 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1502
struct _dyneither_ptr _tmp4DF=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1504
if((((_get_dyneither_size(_tmp4DF,sizeof(char))> 4  && ((const char*)_tmp4DF.curr)[0]== '_') && ((const char*)_tmp4DF.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4DF,sizeof(char),(int)(_get_dyneither_size(_tmp4DF,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4DF,sizeof(char),(int)(_get_dyneither_size(_tmp4DF,sizeof(char))- 3)))== '_')
# 1506
_tmp4DF=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4DF,2,_get_dyneither_size(_tmp4DF,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4DF,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1513
if(i == 17){
{const char*_tmpA84;Cyc_Parse_err(((_tmpA84="unrecognized attribute",_tag_dyneither(_tmpA84,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1518
break;};}case 66: _LL26C: {
# 1520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1518 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 67: _LL26D: {
# 1521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1520 "parse.y"
struct _dyneither_ptr _tmp4F3=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _stmttmp34=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp4F5;struct _tuple5 _tmp4F4=_stmttmp34;_tmp4F5=_tmp4F4.f2;{
void*a;
{const char*_tmpA86;const char*_tmpA85;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA86="regparm",_tag_dyneither(_tmpA86,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA85="__regparm__",_tag_dyneither(_tmpA85,sizeof(char),12))))== 0){
if(_tmp4F5 < 0  || _tmp4F5 > 3){
const char*_tmpA87;Cyc_Parse_err(((_tmpA87="regparm requires value between 0 and 3",_tag_dyneither(_tmpA87,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA8A;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA89;a=(void*)((_tmpA89=_cycalloc_atomic(sizeof(*_tmpA89)),((_tmpA89[0]=((_tmpA8A.tag=0,((_tmpA8A.f1=_tmp4F5,_tmpA8A)))),_tmpA89))));};}else{
const char*_tmpA8C;const char*_tmpA8B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA8C="aligned",_tag_dyneither(_tmpA8C,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA8B="__aligned__",_tag_dyneither(_tmpA8B,sizeof(char),12))))== 0){
if(_tmp4F5 < 0){const char*_tmpA8D;Cyc_Parse_err(((_tmpA8D="aligned requires positive power of two",_tag_dyneither(_tmpA8D,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4F5;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA8E;Cyc_Parse_err(((_tmpA8E="aligned requires positive power of two",_tag_dyneither(_tmpA8E,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA91;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA90;a=(void*)((_tmpA90=_cycalloc_atomic(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=6,((_tmpA91.f1=_tmp4F5,_tmpA91)))),_tmpA90))));};};}else{
const char*_tmpA93;const char*_tmpA92;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA93="initializes",_tag_dyneither(_tmpA93,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA92="__initializes__",_tag_dyneither(_tmpA92,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA96;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA95;a=(void*)((_tmpA95=_cycalloc_atomic(sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA96.tag=20,((_tmpA96.f1=_tmp4F5,_tmpA96)))),_tmpA95))));}else{
const char*_tmpA98;const char*_tmpA97;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA98="noliveunique",_tag_dyneither(_tmpA98,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA97="__noliveunique__",_tag_dyneither(_tmpA97,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpA9B;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA9A;a=(void*)((_tmpA9A=_cycalloc_atomic(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=21,((_tmpA9B.f1=_tmp4F5,_tmpA9B)))),_tmpA9A))));}else{
const char*_tmpA9D;const char*_tmpA9C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA9D="noconsume",_tag_dyneither(_tmpA9D,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4F3,((_tmpA9C="__noconsume__",_tag_dyneither(_tmpA9C,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAA0;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpA9F;a=(void*)((_tmpA9F=_cycalloc_atomic(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=22,((_tmpAA0.f1=_tmp4F5,_tmpAA0)))),_tmpA9F))));}else{
# 1542
{const char*_tmpAA1;Cyc_Parse_err(((_tmpAA1="unrecognized attribute",_tag_dyneither(_tmpAA1,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1545
yyval=Cyc_YY49(a);
# 1547
break;};}case 68: _LL26E: {
# 1549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1548 "parse.y"
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp50F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAA3;const char*_tmpAA2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpAA3="section",_tag_dyneither(_tmpAA3,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50E,((_tmpAA2="__section__",_tag_dyneither(_tmpAA2,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAA6;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAA5;a=(void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=8,((_tmpAA6.f1=_tmp50F,_tmpAA6)))),_tmpAA5))));}else{
# 1554
{const char*_tmpAA7;Cyc_Parse_err(((_tmpAA7="unrecognized attribute",_tag_dyneither(_tmpAA7,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1557
yyval=Cyc_YY49(a);
# 1559
break;}case 69: _LL26F: {
# 1561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1560 "parse.y"
struct _dyneither_ptr _tmp515=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp516=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1564
{const char*_tmpAA8;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp515,((_tmpAA8="__mode__",_tag_dyneither(_tmpAA8,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAAB;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAAA;a=(void*)((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=24,((_tmpAAB.f1=_tmp516,_tmpAAB)))),_tmpAAA))));}else{
# 1567
{const char*_tmpAAC;Cyc_Parse_err(((_tmpAAC="unrecognized attribute",_tag_dyneither(_tmpAAC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1570
yyval=Cyc_YY49(a);
# 1572
break;}case 70: _LL270: {
# 1574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1573 "parse.y"
struct _dyneither_ptr _tmp51B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51C=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _stmttmp32=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp51E;struct _tuple5 _tmp51D=_stmttmp32;_tmp51E=_tmp51D.f2;{
struct _tuple5 _stmttmp33=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp520;struct _tuple5 _tmp51F=_stmttmp33;_tmp520=_tmp51F.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAAE;const char*_tmpAAD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAAE="format",_tag_dyneither(_tmpAAE,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAAD="__format__",_tag_dyneither(_tmpAAD,sizeof(char),11))))== 0){
const char*_tmpAAF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51C,((_tmpAAF="printf",_tag_dyneither(_tmpAAF,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB2;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB1;a=(void*)((_tmpAB1=_cycalloc_atomic(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=19,((_tmpAB2.f1=Cyc_Absyn_Printf_ft,((_tmpAB2.f2=_tmp51E,((_tmpAB2.f3=_tmp520,_tmpAB2)))))))),_tmpAB1))));}else{
const char*_tmpAB3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51C,((_tmpAB3="scanf",_tag_dyneither(_tmpAB3,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB6;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB5;a=(void*)((_tmpAB5=_cycalloc_atomic(sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB6.tag=19,((_tmpAB6.f1=Cyc_Absyn_Scanf_ft,((_tmpAB6.f2=_tmp51E,((_tmpAB6.f3=_tmp520,_tmpAB6)))))))),_tmpAB5))));}else{
# 1584
const char*_tmpAB7;Cyc_Parse_err(((_tmpAB7="unrecognized format type",_tag_dyneither(_tmpAB7,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1586
const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="unrecognized attribute",_tag_dyneither(_tmpAB8,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1589
break;};};}case 71: _LL271: {
# 1591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1598 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL272: {
# 1601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1600 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpABB;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpABA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=17,((_tmpABB.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpABB.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpABB.f3=0,((_tmpABB.f4=0,_tmpABB)))))))))),_tmpABA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL273: {
# 1603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL274: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL275: {
# 1608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1607 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL276: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL277: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL278: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL279: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL27A: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL27B: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL27C: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL27D: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 84: _LL27E: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27F: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL280: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL281: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1621 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL282: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1624 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpABE;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpABD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=10,((_tmpABE.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpABE)))),_tmpABD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1627
break;}case 89: _LL283: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1628 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC1;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC0;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=15,((_tmpAC1.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAC1)))),_tmpAC0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL284: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1630 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC4;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC3;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3[0]=((_tmpAC4.tag=15,((_tmpAC4.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAC4)))),_tmpAC3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1632
break;}case 91: _LL285: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1633 "parse.y"
void*_tmp533=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAC7;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAC6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=((_tmpAC7.tag=16,((_tmpAC7.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAC7.f2=_tmp533,_tmpAC7)))))),_tmpAC6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1636
break;}case 92: _LL286: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1637 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpACA;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAC9;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=16,((_tmpACA.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpACA.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpACA)))))),_tmpAC9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL287: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1639 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpACD;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpACC;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpACD.tag=19,((_tmpACD.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpACD)))),_tmpACC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL288: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1641 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAD0;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpACF;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF[0]=((_tmpAD0.tag=19,((_tmpAD0.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAD0)))),_tmpACF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL289: {
# 1644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1643 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAD3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=18,((_tmpAD3.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAD3)))),_tmpAD2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL28A: {
# 1646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1648 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 97: _LL28B: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAD4;yyval=Cyc_YY26(((_tmpAD4.print_const=1,((_tmpAD4.q_volatile=0,((_tmpAD4.q_restrict=0,((_tmpAD4.real_const=1,((_tmpAD4.loc=loc,_tmpAD4)))))))))));}
break;}case 98: _LL28C: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1654 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD5;yyval=Cyc_YY26(((_tmpAD5.print_const=0,((_tmpAD5.q_volatile=1,((_tmpAD5.q_restrict=0,((_tmpAD5.real_const=0,((_tmpAD5.loc=0,_tmpAD5)))))))))));}
break;}case 99: _LL28D: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAD6;yyval=Cyc_YY26(((_tmpAD6.print_const=0,((_tmpAD6.q_volatile=0,((_tmpAD6.q_restrict=1,((_tmpAD6.real_const=0,((_tmpAD6.loc=0,_tmpAD6)))))))))));}
break;}case 100: _LL28E: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAE5;struct Cyc_Absyn_Enumdecl*_tmpAE4;struct Cyc_Core_Opt*_tmpAE3;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAE2;struct Cyc_Absyn_TypeDecl*_tmpAE1;struct Cyc_Absyn_TypeDecl*_tmp541=(_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->r=(void*)((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=((_tmpAE2.tag=1,((_tmpAE2.f1=((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->sc=Cyc_Absyn_Public,((_tmpAE4->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAE4->fields=((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpAE3)))),_tmpAE4)))))))),_tmpAE2)))),_tmpAE5)))),((_tmpAE1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAE1)))));
# 1663
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAE8;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAE7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=26,((_tmpAE8.f1=_tmp541,((_tmpAE8.f2=0,_tmpAE8)))))),_tmpAE7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1665
break;}case 101: _LL28F: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1666 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAEB;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAEA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=13,((_tmpAEB.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAEB.f2=0,_tmpAEB)))))),_tmpAEA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL290: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1668 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAED;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=14,((_tmpAEE.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAEE)))),_tmpAED)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL291: {
# 1671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpAEF;yyval=Cyc_YY50(((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAEF->tag=0,((_tmpAEF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAEF)))))))));}
break;}case 104: _LL292: {
# 1677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1676 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpAF0;yyval=Cyc_YY50(((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF0->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpAF0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAF0)))))))));}
break;}case 105: _LL293: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1680 "parse.y"
{struct Cyc_List_List*_tmpAF1;yyval=Cyc_YY51(((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF1->tl=0,_tmpAF1)))))));}
break;}case 106: _LL294: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1681 "parse.y"
{struct Cyc_List_List*_tmpAF2;yyval=Cyc_YY51(((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF2->tl=0,_tmpAF2)))))));}
break;}case 107: _LL295: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1682 "parse.y"
{struct Cyc_List_List*_tmpAF3;yyval=Cyc_YY51(((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF3->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAF3)))))));}
break;}case 108: _LL296: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAF6;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAF5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5[0]=((_tmpAF6.tag=12,((_tmpAF6.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAF6.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAF6)))))),_tmpAF5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL297: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1694 "parse.y"
struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp556=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp554,
Cyc_Absyn_aggrdecl_impl(_tmp555,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1699
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAF9;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAF8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8[0]=((_tmpAF9.tag=26,((_tmpAF9.f1=_tmp556,((_tmpAF9.f2=0,_tmpAF9)))))),_tmpAF8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1701
break;}case 110: _LL298: {
# 1703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1705 "parse.y"
struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp55B=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp559,
Cyc_Absyn_aggrdecl_impl(_tmp55A,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1710
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAFC;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAFB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=((_tmpAFC.tag=26,((_tmpAFC.f1=_tmp55B,((_tmpAFC.f2=0,_tmpAFC)))))),_tmpAFB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1712
break;}case 111: _LL299: {
# 1714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1713 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB06;struct Cyc_Core_Opt*_tmpB05;struct Cyc_Absyn_AggrInfo _tmpB04;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB03;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB06.tag=11,((_tmpB06.f1=((_tmpB04.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB05=_cycalloc_atomic(sizeof(*_tmpB05)),((_tmpB05->v=(void*)1,_tmpB05))))),((_tmpB04.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB04)))),_tmpB06)))),_tmpB03)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1716
break;}case 112: _LL29A: {
# 1718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1717 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB0C;struct Cyc_Absyn_AggrInfo _tmpB0B;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB0A;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB0C.tag=11,((_tmpB0C.f1=((_tmpB0B.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB0B.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB0B)))),_tmpB0C)))),_tmpB0A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1720
break;}case 113: _LL29B:
# 1722
 yyval=Cyc_YY43(0);
break;case 114: _LL29C: {
# 1725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1726 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 115: _LL29D: {
# 1729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1730 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 116: _LL29E: {
# 1733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1731 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 117: _LL29F:
# 1734
 yyval=Cyc_YY27(0);
break;case 118: _LL2A0: {
# 1737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1738 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp565=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp565 != 0;_tmp565=_tmp565->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp565->hd,decls);}}{
# 1742
struct Cyc_List_List*_tmp566=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp566 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp566,decls);
yyval=Cyc_YY27(decls);
# 1747
break;};}case 119: _LL2A1: {
# 1749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB0D;yyval=Cyc_YY28(((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB0D->tl=0,_tmpB0D)))))));}
break;}case 120: _LL2A2: {
# 1755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1754 "parse.y"
{struct Cyc_List_List*_tmpB0E;yyval=Cyc_YY28(((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB0E->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB0E)))))));}
break;}case 121: _LL2A3: {
# 1757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1758 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 122: _LL2A4: {
# 1761
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB0F;yyval=Cyc_YY22(((_tmpB0F=_region_malloc(yyr,sizeof(*_tmpB0F)),((_tmpB0F->tl=0,((_tmpB0F->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB0F)))))));}
break;}case 123: _LL2A5: {
# 1767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1766 "parse.y"
{struct _tuple12*_tmpB10;yyval=Cyc_YY22(((_tmpB10=_region_malloc(yyr,sizeof(*_tmpB10)),((_tmpB10->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB10->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB10)))))));}
break;}case 124: _LL2A6: {
# 1769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1771 "parse.y"
{struct _tuple11 _tmpB11;yyval=Cyc_YY21(((_tmpB11.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB11.f2=0,_tmpB11)))));}
break;}case 125: _LL2A7: {
# 1774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1773 "parse.y"
{struct _tuple11 _tmpB12;yyval=Cyc_YY21(((_tmpB12.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB12.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB12)))));}
break;}case 126: _LL2A8: {
# 1776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1779 "parse.y"
struct _RegionHandle _tmp56D=_new_region("temp");struct _RegionHandle*temp=& _tmp56D;_push_region(temp);{
struct _tuple25 _stmttmp30=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp56F;struct Cyc_Parse_Type_specifier _tmp570;struct Cyc_List_List*_tmp571;struct _tuple25 _tmp56E=_stmttmp30;_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;_tmp571=_tmp56E.f3;
if(_tmp56F.loc == 0)_tmp56F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp572=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp572 != 0;_tmp572=_tmp572->tl){
struct _tuple24*_stmttmp31=(struct _tuple24*)_tmp572->hd;struct Cyc_Parse_Declarator _tmp574;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp576;struct _tuple24*_tmp573=_stmttmp31;_tmp574=_tmp573->f1;_tmp575=_tmp573->f2;_tmp576=_tmp573->f3;
{struct _tuple10*_tmpB13;decls=((_tmpB13=_region_malloc(temp,sizeof(*_tmpB13)),((_tmpB13->tl=decls,((_tmpB13->hd=_tmp574,_tmpB13))))));}{
struct _tuple16*_tmpB16;struct Cyc_List_List*_tmpB15;widths_and_reqs=(
(_tmpB15=_region_malloc(temp,sizeof(*_tmpB15)),((_tmpB15->hd=((_tmpB16=_region_malloc(temp,sizeof(*_tmpB16)),((_tmpB16->f1=_tmp575,((_tmpB16->f2=_tmp576,_tmpB16)))))),((_tmpB15->tl=widths_and_reqs,_tmpB15))))));};}}
# 1790
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp57A=Cyc_Parse_speclist2typ(_tmp570,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp56F,_tmp57A,decls,_tmp571),widths_and_reqs);
# 1796
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp57B));};};}
# 1799
_npop_handler(0);break;
# 1779
;_pop_region(temp);}case 127: _LL2A9: {
# 1801
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1807 "parse.y"
{struct _tuple25 _tmpB17;yyval=Cyc_YY38(((_tmpB17.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB17.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB17.f3=0,_tmpB17)))))));}
break;}case 128: _LL2AA: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1809 "parse.y"
struct _tuple25 _tmp57D=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB18;yyval=Cyc_YY38(((_tmpB18.f1=_tmp57D.f1,((_tmpB18.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp57D.f2),((_tmpB18.f3=_tmp57D.f3,_tmpB18)))))));}
break;}case 129: _LL2AB: {
# 1812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1811 "parse.y"
{struct _tuple25 _tmpB19;yyval=Cyc_YY38(((_tmpB19.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB19.f2=Cyc_Parse_empty_spec(0),((_tmpB19.f3=0,_tmpB19)))))));}
break;}case 130: _LL2AC: {
# 1814
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1813 "parse.y"
struct _tuple25 _tmp580=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB1A;yyval=Cyc_YY38(((_tmpB1A.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp580.f1),((_tmpB1A.f2=_tmp580.f2,((_tmpB1A.f3=_tmp580.f3,_tmpB1A)))))));}
break;}case 131: _LL2AD: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1816 "parse.y"
{struct _tuple25 _tmpB1B;yyval=Cyc_YY38(((_tmpB1B.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1B.f2=Cyc_Parse_empty_spec(0),((_tmpB1B.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB1B)))))));}
break;}case 132: _LL2AE: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1818 "parse.y"
struct _tuple25 _tmp583=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1C;yyval=Cyc_YY38(((_tmpB1C.f1=_tmp583.f1,((_tmpB1C.f2=_tmp583.f2,((_tmpB1C.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp583.f3),_tmpB1C)))))));}
break;}case 133: _LL2AF: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB1D;yyval=Cyc_YY38(((_tmpB1D.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1D.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB1D.f3=0,_tmpB1D)))))));}
break;}case 134: _LL2B0: {
# 1827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1826 "parse.y"
struct _tuple25 _tmp586=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1E;yyval=Cyc_YY38(((_tmpB1E.f1=_tmp586.f1,((_tmpB1E.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp586.f2),((_tmpB1E.f3=_tmp586.f3,_tmpB1E)))))));}
break;}case 135: _LL2B1: {
# 1829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1828 "parse.y"
{struct _tuple25 _tmpB1F;yyval=Cyc_YY38(((_tmpB1F.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB1F.f2=Cyc_Parse_empty_spec(0),((_tmpB1F.f3=0,_tmpB1F)))))));}
break;}case 136: _LL2B2: {
# 1831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1830 "parse.y"
struct _tuple25 _tmp589=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB20;yyval=Cyc_YY38(((_tmpB20.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp589.f1),((_tmpB20.f2=_tmp589.f2,((_tmpB20.f3=_tmp589.f3,_tmpB20)))))));}
break;}case 137: _LL2B3: {
# 1834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1833 "parse.y"
{struct _tuple25 _tmpB21;yyval=Cyc_YY38(((_tmpB21.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB21.f2=Cyc_Parse_empty_spec(0),((_tmpB21.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB21)))))));}
break;}case 138: _LL2B4: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1835 "parse.y"
struct _tuple25 _tmp58C=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB22;yyval=Cyc_YY38(((_tmpB22.f1=_tmp58C.f1,((_tmpB22.f2=_tmp58C.f2,((_tmpB22.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp58C.f3),_tmpB22)))))));}
break;}case 139: _LL2B5: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1839 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 140: _LL2B6: {
# 1842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB23;yyval=Cyc_YY32(((_tmpB23=_region_malloc(yyr,sizeof(*_tmpB23)),((_tmpB23->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB23->tl=0,_tmpB23)))))));}
break;}case 141: _LL2B7: {
# 1848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1847 "parse.y"
{struct Cyc_List_List*_tmpB24;yyval=Cyc_YY32(((_tmpB24=_region_malloc(yyr,sizeof(*_tmpB24)),((_tmpB24->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB24->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB24)))))));}
break;}case 142: _LL2B8: {
# 1850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1852 "parse.y"
{struct _tuple24*_tmpB25;yyval=Cyc_YY31(((_tmpB25=_region_malloc(yyr,sizeof(*_tmpB25)),((_tmpB25->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB25->f2=0,((_tmpB25->f3=0,_tmpB25)))))))));}
break;}case 143: _LL2B9: {
# 1855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1856 "parse.y"
{struct Cyc_Parse_Declarator _tmpB2B;struct _tuple0*_tmpB2A;struct _tuple24*_tmpB29;yyval=Cyc_YY31(((_tmpB29=_region_malloc(yyr,sizeof(*_tmpB29)),((_tmpB29->f1=((_tmpB2B.id=((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->f1=Cyc_Absyn_Rel_n(0),((_tmpB2A->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB2A)))))),((_tmpB2B.tms=0,_tmpB2B)))),((_tmpB29->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB29->f3=0,_tmpB29)))))))));}
# 1858
break;}case 144: _LL2BA:
# 1862
{struct Cyc_Parse_Declarator _tmpB31;struct _tuple0*_tmpB30;struct _tuple24*_tmpB2F;yyval=Cyc_YY31(((_tmpB2F=_region_malloc(yyr,sizeof(*_tmpB2F)),((_tmpB2F->f1=((_tmpB31.id=((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30->f1=Cyc_Absyn_Rel_n(0),((_tmpB30->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB30)))))),((_tmpB31.tms=0,_tmpB31)))),((_tmpB2F->f2=0,((_tmpB2F->f3=0,_tmpB2F)))))))));}
# 1864
break;case 145: _LL2BB: {
# 1866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1864 "parse.y"
{struct _tuple24*_tmpB32;yyval=Cyc_YY31(((_tmpB32=_region_malloc(yyr,sizeof(*_tmpB32)),((_tmpB32->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB32->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB32->f3=0,_tmpB32)))))))));}
break;}case 146: _LL2BC: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1866 "parse.y"
{struct _tuple24*_tmpB33;yyval=Cyc_YY31(((_tmpB33=_region_malloc(yyr,sizeof(*_tmpB33)),((_tmpB33->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB33->f2=0,((_tmpB33->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB33)))))))));}
break;}case 147: _LL2BD: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp59B=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp59C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB34;struct Cyc_Absyn_TypeDecl*_tmp59D=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp59C,((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB34)))),_tmp59B,(unsigned int)(yyyylsp[0]).first_line);
# 1876
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB37;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB36;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36[0]=((_tmpB37.tag=26,((_tmpB37.f1=_tmp59D,((_tmpB37.f2=0,_tmpB37)))))),_tmpB36)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1878
break;}case 148: _LL2BE: {
# 1880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1879 "parse.y"
int _tmp5A1=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB41;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB40;struct Cyc_Absyn_DatatypeInfo _tmpB3F;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB3E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E[0]=((_tmpB41.tag=3,((_tmpB41.f1=((_tmpB3F.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB40.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB40.is_extensible=_tmp5A1,_tmpB40))))),((_tmpB3F.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB3F)))),_tmpB41)))),_tmpB3E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1882
break;}case 149: _LL2BF: {
# 1884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1883 "parse.y"
int _tmp5A6=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB4B;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB4A;struct Cyc_Absyn_DatatypeFieldInfo _tmpB49;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB48;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48[0]=((_tmpB4B.tag=4,((_tmpB4B.f1=((_tmpB49.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB4A.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB4A.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB4A.is_extensible=_tmp5A6,_tmpB4A))))))),((_tmpB49.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB49)))),_tmpB4B)))),_tmpB48)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1888
break;}case 150: _LL2C0: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1891 "parse.y"
yyval=Cyc_YY34(0);
break;}case 151: _LL2C1: {
# 1894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1892 "parse.y"
yyval=Cyc_YY34(1);
break;}case 152: _LL2C2: {
# 1895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1896 "parse.y"
{struct Cyc_List_List*_tmpB4C;yyval=Cyc_YY37(((_tmpB4C=_cycalloc(sizeof(*_tmpB4C)),((_tmpB4C->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4C->tl=0,_tmpB4C)))))));}
break;}case 153: _LL2C3: {
# 1899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1897 "parse.y"
{struct Cyc_List_List*_tmpB4D;yyval=Cyc_YY37(((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4D->tl=0,_tmpB4D)))))));}
break;}case 154: _LL2C4: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1898 "parse.y"
{struct Cyc_List_List*_tmpB4E;yyval=Cyc_YY37(((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4E->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB4E)))))));}
break;}case 155: _LL2C5: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpB4F;yyval=Cyc_YY37(((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB4F->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB4F)))))));}
break;}case 156: _LL2C6:
# 1902
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 157: _LL2C7: {
# 1905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1904 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 158: _LL2C8: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1905 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 159: _LL2C9: {
# 1908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1909 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB50;yyval=Cyc_YY36(((_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB50->typs=0,((_tmpB50->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB50->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB50)))))))))));}
break;}case 160: _LL2CA: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1911 "parse.y"
struct Cyc_List_List*_tmp5B0=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB51;yyval=Cyc_YY36(((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB51->typs=_tmp5B0,((_tmpB51->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB51->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB51)))))))))));}
break;}case 161: _LL2CB: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1917 "parse.y"
yyval=yyyyvsp[0];
break;}case 162: _LL2CC: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1919 "parse.y"
struct Cyc_Parse_Declarator _tmp5B2=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB52;yyval=Cyc_YY30(((_tmpB52.id=_tmp5B2.id,((_tmpB52.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B2.tms),_tmpB52)))));}
break;}case 163: _LL2CD: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 164: _LL2CE: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1928 "parse.y"
struct Cyc_Parse_Declarator _tmp5B4=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB53;yyval=Cyc_YY30(((_tmpB53.id=_tmp5B4.id,((_tmpB53.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5B4.tms),_tmpB53)))));}
break;}case 165: _LL2CF: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1934 "parse.y"
{struct Cyc_Parse_Declarator _tmpB54;yyval=Cyc_YY30(((_tmpB54.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB54.tms=0,_tmpB54)))));}
break;}case 166: _LL2D0: {
# 1937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1936 "parse.y"
yyval=yyyyvsp[1];
break;}case 167: _LL2D1: {
# 1939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1940 "parse.y"
struct Cyc_Parse_Declarator _tmp5B7=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB5A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB59;struct Cyc_List_List*_tmpB58;_tmp5B7.tms=((_tmpB58=_region_malloc(yyr,sizeof(*_tmpB58)),((_tmpB58->hd=(void*)((_tmpB5A=_region_malloc(yyr,sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB59.tag=5,((_tmpB59.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB59.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB59)))))),_tmpB5A)))),((_tmpB58->tl=_tmp5B7.tms,_tmpB58))))));}
yyval=yyyyvsp[2];
# 1944
break;}case 168: _LL2D2: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1945 "parse.y"
{struct Cyc_List_List*_tmpB64;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB63;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB62;struct Cyc_Parse_Declarator _tmpB61;yyval=Cyc_YY30(((_tmpB61.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB61.tms=((_tmpB64=_region_malloc(yyr,sizeof(*_tmpB64)),((_tmpB64->hd=(void*)((_tmpB62=_region_malloc(yyr,sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=0,((_tmpB63.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB63.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB63)))))),_tmpB62)))),((_tmpB64->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB64)))))),_tmpB61)))));}
break;}case 169: _LL2D3: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1947 "parse.y"
{struct Cyc_List_List*_tmpB6E;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB6D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB6C;struct Cyc_Parse_Declarator _tmpB6B;yyval=Cyc_YY30(((_tmpB6B.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB6B.tms=(
(_tmpB6E=_region_malloc(yyr,sizeof(*_tmpB6E)),((_tmpB6E->hd=(void*)((_tmpB6C=_region_malloc(yyr,sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6D.tag=1,((_tmpB6D.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB6D.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB6D.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB6D)))))))),_tmpB6C)))),((_tmpB6E->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB6E)))))),_tmpB6B)))));}
break;}case 170: _LL2D4: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1950 "parse.y"
struct _tuple26*_stmttmp2F=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5C4;int _tmp5C5;struct Cyc_Absyn_VarargInfo*_tmp5C6;void*_tmp5C7;struct Cyc_List_List*_tmp5C8;struct _tuple26*_tmp5C3=_stmttmp2F;_tmp5C4=_tmp5C3->f1;_tmp5C5=_tmp5C3->f2;_tmp5C6=_tmp5C3->f3;_tmp5C7=_tmp5C3->f4;_tmp5C8=_tmp5C3->f5;
{struct Cyc_List_List*_tmpB83;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB81;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB80;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB7F;struct Cyc_Parse_Declarator _tmpB7E;yyval=Cyc_YY30(((_tmpB7E.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB7E.tms=((_tmpB83=_region_malloc(yyr,sizeof(*_tmpB83)),((_tmpB83->hd=(void*)((_tmpB7F=_region_malloc(yyr,sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB82.tag=3,((_tmpB82.f1=(void*)((_tmpB80=_region_malloc(yyr,sizeof(*_tmpB80)),((_tmpB80[0]=((_tmpB81.tag=1,((_tmpB81.f1=_tmp5C4,((_tmpB81.f2=_tmp5C5,((_tmpB81.f3=_tmp5C6,((_tmpB81.f4=_tmp5C7,((_tmpB81.f5=_tmp5C8,_tmpB81)))))))))))),_tmpB80)))),_tmpB82)))),_tmpB7F)))),((_tmpB83->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB83)))))),_tmpB7E)))));}
# 1953
break;}case 171: _LL2D5: {
# 1955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1954 "parse.y"
{struct Cyc_List_List*_tmpB98;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB97;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB96;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB95;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB94;struct Cyc_Parse_Declarator _tmpB93;yyval=Cyc_YY30(((_tmpB93.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB93.tms=(
(_tmpB98=_region_malloc(yyr,sizeof(*_tmpB98)),((_tmpB98->hd=(void*)((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB97.tag=3,((_tmpB97.f1=(void*)((_tmpB95=_region_malloc(yyr,sizeof(*_tmpB95)),((_tmpB95[0]=((_tmpB96.tag=1,((_tmpB96.f1=0,((_tmpB96.f2=0,((_tmpB96.f3=0,((_tmpB96.f4=
# 1957
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpB96.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpB96)))))))))))),_tmpB95)))),_tmpB97)))),_tmpB94)))),((_tmpB98->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB98)))))),_tmpB93)))));}
# 1960
break;}case 172: _LL2D6: {
# 1962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1961 "parse.y"
{struct Cyc_List_List*_tmpBAD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAC;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBAB;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBAA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBA9;struct Cyc_Parse_Declarator _tmpBA8;yyval=Cyc_YY30(((_tmpBA8.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA8.tms=((_tmpBAD=_region_malloc(yyr,sizeof(*_tmpBAD)),((_tmpBAD->hd=(void*)((_tmpBA9=_region_malloc(yyr,sizeof(*_tmpBA9)),((_tmpBA9[0]=((_tmpBAC.tag=3,((_tmpBAC.f1=(void*)((_tmpBAA=_region_malloc(yyr,sizeof(*_tmpBAA)),((_tmpBAA[0]=((_tmpBAB.tag=0,((_tmpBAB.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBAB.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBAB)))))),_tmpBAA)))),_tmpBAC)))),_tmpBA9)))),((_tmpBAD->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBAD)))))),_tmpBA8)))));}
break;}case 173: _LL2D7: {
# 1964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1964 "parse.y"
struct Cyc_List_List*_tmp5DB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBB7;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBB6;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBB5;struct Cyc_Parse_Declarator _tmpBB4;yyval=Cyc_YY30(((_tmpBB4.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB4.tms=((_tmpBB7=_region_malloc(yyr,sizeof(*_tmpBB7)),((_tmpBB7->hd=(void*)((_tmpBB5=_region_malloc(yyr,sizeof(*_tmpBB5)),((_tmpBB5[0]=((_tmpBB6.tag=4,((_tmpBB6.f1=_tmp5DB,((_tmpBB6.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBB6.f3=0,_tmpBB6)))))))),_tmpBB5)))),((_tmpBB7->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBB7)))))),_tmpBB4)))));}
# 1967
break;}case 174: _LL2D8: {
# 1969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1968 "parse.y"
{struct Cyc_List_List*_tmpBC1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBBF;struct Cyc_Parse_Declarator _tmpBBE;yyval=Cyc_YY30(((_tmpBBE.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBBE.tms=((_tmpBC1=_region_malloc(yyr,sizeof(*_tmpBC1)),((_tmpBC1->hd=(void*)((_tmpBBF=_region_malloc(yyr,sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC0.tag=5,((_tmpBC0.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC0.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC0)))))),_tmpBBF)))),((_tmpBC1->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBC1)))))),_tmpBBE)))));}
# 1971
break;}case 175: _LL2D9: {
# 1973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1982 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC2;yyval=Cyc_YY30(((_tmpBC2.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC2.tms=0,_tmpBC2)))));}
break;}case 176: _LL2DA: {
# 1985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1984 "parse.y"
{struct Cyc_Parse_Declarator _tmpBC3;yyval=Cyc_YY30(((_tmpBC3.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC3.tms=0,_tmpBC3)))));}
break;}case 177: _LL2DB: {
# 1987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1986 "parse.y"
yyval=yyyyvsp[1];
break;}case 178: _LL2DC: {
# 1989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1990 "parse.y"
struct Cyc_Parse_Declarator _tmp5E6=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBC9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC8;struct Cyc_List_List*_tmpBC7;_tmp5E6.tms=((_tmpBC7=_region_malloc(yyr,sizeof(*_tmpBC7)),((_tmpBC7->hd=(void*)((_tmpBC9=_region_malloc(yyr,sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBC8.tag=5,((_tmpBC8.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC8.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC8)))))),_tmpBC9)))),((_tmpBC7->tl=_tmp5E6.tms,_tmpBC7))))));}
yyval=yyyyvsp[2];
# 1994
break;}case 179: _LL2DD: {
# 1996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1995 "parse.y"
struct Cyc_Parse_Declarator _tmp5EA=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD3;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD2;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBD1;struct Cyc_Parse_Declarator _tmpBD0;yyval=Cyc_YY30(((_tmpBD0.id=_tmp5EA.id,((_tmpBD0.tms=(
(_tmpBD3=_region_malloc(yyr,sizeof(*_tmpBD3)),((_tmpBD3->hd=(void*)((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD2.tag=0,((_tmpBD2.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBD2.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBD2)))))),_tmpBD1)))),((_tmpBD3->tl=_tmp5EA.tms,_tmpBD3)))))),_tmpBD0)))));}
break;}case 180: _LL2DE: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1999 "parse.y"
struct Cyc_Parse_Declarator _tmp5EF=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBDD;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBDC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBDB;struct Cyc_Parse_Declarator _tmpBDA;yyval=Cyc_YY30(((_tmpBDA.id=_tmp5EF.id,((_tmpBDA.tms=(
(_tmpBDD=_region_malloc(yyr,sizeof(*_tmpBDD)),((_tmpBDD->hd=(void*)((_tmpBDB=_region_malloc(yyr,sizeof(*_tmpBDB)),((_tmpBDB[0]=((_tmpBDC.tag=1,((_tmpBDC.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBDC.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBDC.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBDC)))))))),_tmpBDB)))),((_tmpBDD->tl=_tmp5EF.tms,_tmpBDD)))))),_tmpBDA)))));}
# 2003
break;}case 181: _LL2DF: {
# 2005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2004 "parse.y"
struct _tuple26*_stmttmp2E=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5F5;int _tmp5F6;struct Cyc_Absyn_VarargInfo*_tmp5F7;void*_tmp5F8;struct Cyc_List_List*_tmp5F9;struct _tuple26*_tmp5F4=_stmttmp2E;_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;_tmp5F7=_tmp5F4->f3;_tmp5F8=_tmp5F4->f4;_tmp5F9=_tmp5F4->f5;{
struct Cyc_Parse_Declarator _tmp5FA=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF1;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBF0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBEF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBEE;struct Cyc_Parse_Declarator _tmpBED;yyval=Cyc_YY30(((_tmpBED.id=_tmp5FA.id,((_tmpBED.tms=((_tmpBF2=_region_malloc(yyr,sizeof(*_tmpBF2)),((_tmpBF2->hd=(void*)((_tmpBEE=_region_malloc(yyr,sizeof(*_tmpBEE)),((_tmpBEE[0]=((_tmpBF1.tag=3,((_tmpBF1.f1=(void*)((_tmpBEF=_region_malloc(yyr,sizeof(*_tmpBEF)),((_tmpBEF[0]=((_tmpBF0.tag=1,((_tmpBF0.f1=_tmp5F5,((_tmpBF0.f2=_tmp5F6,((_tmpBF0.f3=_tmp5F7,((_tmpBF0.f4=_tmp5F8,((_tmpBF0.f5=_tmp5F9,_tmpBF0)))))))))))),_tmpBEF)))),_tmpBF1)))),_tmpBEE)))),((_tmpBF2->tl=_tmp5FA.tms,_tmpBF2)))))),_tmpBED)))));}
# 2008
break;};}case 182: _LL2E0: {
# 2010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp601=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC07;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC06;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC05;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC04;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC03;struct Cyc_Parse_Declarator _tmpC02;yyval=Cyc_YY30(((_tmpC02.id=_tmp601.id,((_tmpC02.tms=(
(_tmpC07=_region_malloc(yyr,sizeof(*_tmpC07)),((_tmpC07->hd=(void*)((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC06.tag=3,((_tmpC06.f1=(void*)((_tmpC04=_region_malloc(yyr,sizeof(*_tmpC04)),((_tmpC04[0]=((_tmpC05.tag=1,((_tmpC05.f1=0,((_tmpC05.f2=0,((_tmpC05.f3=0,((_tmpC05.f4=
# 2013
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC05.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC05)))))))))))),_tmpC04)))),_tmpC06)))),_tmpC03)))),((_tmpC07->tl=_tmp601.tms,_tmpC07)))))),_tmpC02)))));}
# 2016
break;}case 183: _LL2E1: {
# 2018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2017 "parse.y"
struct Cyc_Parse_Declarator _tmp608=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC1C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC1B;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC1A;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC19;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC18;struct Cyc_Parse_Declarator _tmpC17;yyval=Cyc_YY30(((_tmpC17.id=_tmp608.id,((_tmpC17.tms=((_tmpC1C=_region_malloc(yyr,sizeof(*_tmpC1C)),((_tmpC1C->hd=(void*)((_tmpC18=_region_malloc(yyr,sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC1B.tag=3,((_tmpC1B.f1=(void*)((_tmpC19=_region_malloc(yyr,sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC1A.tag=0,((_tmpC1A.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC1A.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC1A)))))),_tmpC19)))),_tmpC1B)))),_tmpC18)))),((_tmpC1C->tl=_tmp608.tms,_tmpC1C)))))),_tmpC17)))));}
break;}case 184: _LL2E2: {
# 2021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2021 "parse.y"
struct Cyc_List_List*_tmp60F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp610=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC26;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC25;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC24;struct Cyc_Parse_Declarator _tmpC23;yyval=Cyc_YY30(((_tmpC23.id=_tmp610.id,((_tmpC23.tms=((_tmpC26=_region_malloc(yyr,sizeof(*_tmpC26)),((_tmpC26->hd=(void*)((_tmpC24=_region_malloc(yyr,sizeof(*_tmpC24)),((_tmpC24[0]=((_tmpC25.tag=4,((_tmpC25.f1=_tmp60F,((_tmpC25.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC25.f3=0,_tmpC25)))))))),_tmpC24)))),((_tmpC26->tl=_tmp610.tms,_tmpC26)))))),_tmpC23)))));}
# 2025
break;}case 185: _LL2E3: {
# 2027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2026 "parse.y"
struct Cyc_Parse_Declarator _tmp615=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC30;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC2F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC2E;struct Cyc_Parse_Declarator _tmpC2D;yyval=Cyc_YY30(((_tmpC2D.id=_tmp615.id,((_tmpC2D.tms=((_tmpC30=_region_malloc(yyr,sizeof(*_tmpC30)),((_tmpC30->hd=(void*)((_tmpC2E=_region_malloc(yyr,sizeof(*_tmpC2E)),((_tmpC2E[0]=((_tmpC2F.tag=5,((_tmpC2F.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC2F.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC2F)))))),_tmpC2E)))),((_tmpC30->tl=_tmp615.tms,_tmpC30)))))),_tmpC2D)))));}
# 2029
break;}case 186: _LL2E4: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2039 "parse.y"
yyval=yyyyvsp[0];
break;}case 187: _LL2E5: {
# 2042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2040 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 188: _LL2E6: {
# 2043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2044 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC36;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC35;struct Cyc_List_List*_tmpC34;ans=((_tmpC34=_region_malloc(yyr,sizeof(*_tmpC34)),((_tmpC34->hd=(void*)((_tmpC36=_region_malloc(yyr,sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC35.tag=5,((_tmpC35.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC35.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC35)))))),_tmpC36)))),((_tmpC34->tl=ans,_tmpC34))))));}{
# 2048
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _stmttmp2C=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp61E;union Cyc_Absyn_Constraint*_tmp61F;union Cyc_Absyn_Constraint*_tmp620;struct _tuple21 _tmp61D=_stmttmp2C;_tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;_tmp620=_tmp61D.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC37;ptrloc=((_tmpC37=_cycalloc_atomic(sizeof(*_tmpC37)),((_tmpC37->ptr_loc=_tmp61E,((_tmpC37->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC37->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC37))))))));}{
# 2053
struct _tuple14 _stmttmp2D=Cyc_Parse_collapse_pointer_quals(_tmp61E,_tmp61F,_tmp620,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp623;union Cyc_Absyn_Constraint*_tmp624;union Cyc_Absyn_Constraint*_tmp625;void*_tmp626;struct _tuple14 _tmp622=_stmttmp2D;_tmp623=_tmp622.f1;_tmp624=_tmp622.f2;_tmp625=_tmp622.f3;_tmp626=_tmp622.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC41;struct Cyc_Absyn_PtrAtts _tmpC40;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC3F;struct Cyc_List_List*_tmpC3E;ans=((_tmpC3E=_region_malloc(yyr,sizeof(*_tmpC3E)),((_tmpC3E->hd=(void*)((_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC3F.tag=2,((_tmpC3F.f1=((_tmpC40.rgn=_tmp626,((_tmpC40.nullable=_tmp623,((_tmpC40.bounds=_tmp624,((_tmpC40.zero_term=_tmp625,((_tmpC40.ptrloc=ptrloc,_tmpC40)))))))))),((_tmpC3F.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC3F)))))),_tmpC41)))),((_tmpC3E->tl=ans,_tmpC3E))))));}
yyval=Cyc_YY29(ans);
# 2057
break;};};}case 189: _LL2E7:
# 2059
 yyval=Cyc_YY57(0);
break;case 190: _LL2E8: {
# 2062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2060 "parse.y"
{struct Cyc_List_List*_tmpC42;yyval=Cyc_YY57(((_tmpC42=_region_malloc(yyr,sizeof(*_tmpC42)),((_tmpC42->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC42->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC42)))))));}
break;}case 191: _LL2E9: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2065 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC45;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC44;yyval=Cyc_YY56((void*)((_tmpC44=_region_malloc(yyr,sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC45.tag=0,((_tmpC45.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC45)))),_tmpC44)))));}
break;}case 192: _LL2EA: {
# 2068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2067 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC48;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC47;yyval=Cyc_YY56((void*)((_tmpC47=_region_malloc(yyr,sizeof(*_tmpC47)),((_tmpC47[0]=((_tmpC48.tag=1,((_tmpC48.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC48)))),_tmpC47)))));}
break;}case 193: _LL2EB: {
# 2070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2069 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC4B;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC4A;yyval=Cyc_YY56((void*)((_tmpC4A=_region_malloc(yyr,sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4B.tag=2,_tmpC4B)),_tmpC4A)))));}
break;}case 194: _LL2EC: {
# 2072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2071 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC4E;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC4D;yyval=Cyc_YY56((void*)((_tmpC4D=_region_malloc(yyr,sizeof(*_tmpC4D)),((_tmpC4D[0]=((_tmpC4E.tag=3,_tmpC4E)),_tmpC4D)))));}
break;}case 195: _LL2ED: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2073 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC51;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC50;yyval=Cyc_YY56((void*)((_tmpC50=_region_malloc(yyr,sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=4,_tmpC51)),_tmpC50)))));}
break;}case 196: _LL2EE: {
# 2076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2075 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC54;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC53;yyval=Cyc_YY56((void*)((_tmpC53=_region_malloc(yyr,sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=5,_tmpC54)),_tmpC53)))));}
break;}case 197: _LL2EF: {
# 2078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2077 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC57;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC56;yyval=Cyc_YY56((void*)((_tmpC56=_region_malloc(yyr,sizeof(*_tmpC56)),((_tmpC56[0]=((_tmpC57.tag=6,_tmpC57)),_tmpC56)))));}
break;}case 198: _LL2F0: {
# 2080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2079 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC5A;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC59;yyval=Cyc_YY56((void*)((_tmpC59=_region_malloc(yyr,sizeof(*_tmpC59)),((_tmpC59[0]=((_tmpC5A.tag=7,_tmpC5A)),_tmpC59)))));}
break;}case 199: _LL2F1: {
# 2082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2085 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5B;yyval=Cyc_YY1(((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->f1=loc,((_tmpC5B->f2=Cyc_Absyn_true_conref,((_tmpC5B->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5B)))))))));}
# 2088
break;}case 200: _LL2F2: {
# 2090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2089 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5C;yyval=Cyc_YY1(((_tmpC5C=_cycalloc(sizeof(*_tmpC5C)),((_tmpC5C->f1=loc,((_tmpC5C->f2=Cyc_Absyn_false_conref,((_tmpC5C->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC5C)))))))));}
# 2092
break;}case 201: _LL2F3: {
# 2094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2093 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC5D;yyval=Cyc_YY1(((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D->f1=loc,((_tmpC5D->f2=Cyc_Absyn_true_conref,((_tmpC5D->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC5D)))))))));}
# 2096
break;}case 202: _LL2F4:
# 2098
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 203: _LL2F5: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2099 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC60;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC5F;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC5F=_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((_tmpC60.tag=1,((_tmpC60.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC60)))),_tmpC5F))))));}
break;}case 204: _LL2F6:
# 2102
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 205: _LL2F7: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2103 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 206: _LL2F8: {
# 2106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2104 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 207: _LL2F9:
# 2107
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 208: _LL2FA: {
# 2110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2110 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 209: _LL2FB: {
# 2113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2111 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 210: _LL2FC:
# 2114
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 211: _LL2FD: {
# 2117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2116 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 212: _LL2FE: {
# 2119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2121 "parse.y"
{struct _tuple26*_tmpC61;yyval=Cyc_YY42(((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC61->f2=0,((_tmpC61->f3=0,((_tmpC61->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC61->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC61)))))))))))));}
break;}case 213: _LL2FF: {
# 2124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2123 "parse.y"
{struct _tuple26*_tmpC62;yyval=Cyc_YY42(((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC62->f2=1,((_tmpC62->f3=0,((_tmpC62->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC62->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC62)))))))))))));}
break;}case 214: _LL300: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2126 "parse.y"
struct _tuple8*_stmttmp2B=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp644;struct Cyc_Absyn_Tqual _tmp645;void*_tmp646;struct _tuple8*_tmp643=_stmttmp2B;_tmp644=_tmp643->f1;_tmp645=_tmp643->f2;_tmp646=_tmp643->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC63;struct Cyc_Absyn_VarargInfo*_tmp647=(_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->name=_tmp644,((_tmpC63->tq=_tmp645,((_tmpC63->type=_tmp646,((_tmpC63->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC63)))))))));
{struct _tuple26*_tmpC64;yyval=Cyc_YY42(((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->f1=0,((_tmpC64->f2=0,((_tmpC64->f3=_tmp647,((_tmpC64->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC64->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC64)))))))))))));}
# 2130
break;};}case 215: _LL301: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2132 "parse.y"
struct _tuple8*_stmttmp2A=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp64B;struct Cyc_Absyn_Tqual _tmp64C;void*_tmp64D;struct _tuple8*_tmp64A=_stmttmp2A;_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;_tmp64D=_tmp64A->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC65;struct Cyc_Absyn_VarargInfo*_tmp64E=(_tmpC65=_cycalloc(sizeof(*_tmpC65)),((_tmpC65->name=_tmp64B,((_tmpC65->tq=_tmp64C,((_tmpC65->type=_tmp64D,((_tmpC65->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC65)))))))));
{struct _tuple26*_tmpC66;yyval=Cyc_YY42(((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC66->f2=0,((_tmpC66->f3=_tmp64E,((_tmpC66->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC66->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC66)))))))))))));}
# 2136
break;};}case 216: _LL302: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2140 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC69;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC68;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC69.tag=1,((_tmpC69.f1=0,_tmpC69)))),_tmpC68))))));}
break;}case 217: _LL303: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2141 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 218: _LL304:
# 2144
 yyval=Cyc_YY52(0);
break;case 219: _LL305: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2145 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC6C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC6B;yyval=Cyc_YY52((void*)((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6C.tag=24,((_tmpC6C.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC6C)))),_tmpC6B)))));}
break;}case 220: _LL306:
# 2148
 yyval=Cyc_YY53(0);
break;case 221: _LL307: {
# 2151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2150 "parse.y"
yyval=yyyyvsp[1];
break;}case 222: _LL308: {
# 2153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2158 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC6F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC6E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp655=(_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E[0]=((_tmpC6F.tag=2,((_tmpC6F.f1=0,((_tmpC6F.f2=& Cyc_Tcutil_trk,_tmpC6F)))))),_tmpC6E)));
struct _dyneither_ptr _tmp656=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp657=Cyc_Parse_id2type(_tmp656,(void*)_tmp655);
{struct _tuple29*_tmpC79;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC78;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC77;struct Cyc_List_List*_tmpC76;yyval=Cyc_YY53(((_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->f1=(void*)((_tmpC77=_cycalloc(sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC78.tag=24,((_tmpC78.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC78)))),_tmpC77)))),((_tmpC79->f2=_tmp657,_tmpC79)))))),((_tmpC76->tl=0,_tmpC76)))))));}
# 2163
break;}case 223: _LL309: {
# 2165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2165 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC7C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC7B;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65E=(_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7C.tag=2,((_tmpC7C.f1=0,((_tmpC7C.f2=& Cyc_Tcutil_trk,_tmpC7C)))))),_tmpC7B)));
struct _dyneither_ptr _tmp65F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp660=Cyc_Parse_id2type(_tmp65F,(void*)_tmp65E);
{struct _tuple29*_tmpC86;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC85;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC84;struct Cyc_List_List*_tmpC83;yyval=Cyc_YY53(((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->f1=(void*)((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84[0]=((_tmpC85.tag=24,((_tmpC85.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC85)))),_tmpC84)))),((_tmpC86->f2=_tmp660,_tmpC86)))))),((_tmpC83->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC83)))))));}
# 2170
break;}case 224: _LL30A:
# 2172
 yyval=Cyc_YY34(0);
break;case 225: _LL30B: {
# 2175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2176 "parse.y"
{const char*_tmpC87;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC87="inject",_tag_dyneither(_tmpC87,sizeof(char),7))))!= 0){
const char*_tmpC88;Cyc_Parse_err(((_tmpC88="missing type in function declaration",_tag_dyneither(_tmpC88,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2180
break;}case 226: _LL30C: {
# 2182
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2183 "parse.y"
yyval=yyyyvsp[0];
break;}case 227: _LL30D: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2184 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 228: _LL30E: {
# 2187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2188 "parse.y"
yyval=Cyc_YY43(0);
break;}case 229: _LL30F: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2189 "parse.y"
yyval=yyyyvsp[1];
break;}case 230: _LL310: {
# 2192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2191 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC8E;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC8D;struct Cyc_List_List*_tmpC8C;yyval=Cyc_YY43(((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=(void*)((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E[0]=((_tmpC8D.tag=25,((_tmpC8D.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC8D)))),_tmpC8E)))),((_tmpC8C->tl=0,_tmpC8C)))))));}
break;}case 231: _LL311: {
# 2194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2193 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC8F;yyval=Cyc_YY43(((_tmpC8F=_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpC8F->tl=0,_tmpC8F)))))));}
# 2196
break;}case 232: _LL312: {
# 2198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC95;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC94;struct Cyc_List_List*_tmpC93;yyval=Cyc_YY43(((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93->hd=(void*)((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC94.tag=23,((_tmpC94.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC94)))),_tmpC95)))),((_tmpC93->tl=0,_tmpC93)))))));}
# 2204
break;}case 233: _LL313: {
# 2206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2205 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC9B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC9A;struct Cyc_List_List*_tmpC99;yyval=Cyc_YY43(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=(void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9A.tag=23,((_tmpC9A.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC9A)))),_tmpC9B)))),((_tmpC99->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpC99)))))));}
# 2208
break;}case 234: _LL314: {
# 2210
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2212
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpC9C;yyval=Cyc_YY41(((_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpC9C->tl=0,_tmpC9C)))))));}
break;}case 235: _LL315: {
# 2216
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2218
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2215 "parse.y"
{struct Cyc_List_List*_tmpC9D;yyval=Cyc_YY41(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpC9D->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpC9D)))))));}
break;}case 236: _LL316: {
# 2218
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2220
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _stmttmp27=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp676;struct Cyc_Parse_Type_specifier _tmp677;struct Cyc_List_List*_tmp678;struct _tuple25 _tmp675=_stmttmp27;_tmp676=_tmp675.f1;_tmp677=_tmp675.f2;_tmp678=_tmp675.f3;
if(_tmp676.loc == 0)_tmp676.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _stmttmp28=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp67A;struct Cyc_List_List*_tmp67B;struct Cyc_Parse_Declarator _tmp679=_stmttmp28;_tmp67A=_tmp679.id;_tmp67B=_tmp679.tms;{
void*_tmp67C=Cyc_Parse_speclist2typ(_tmp677,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _stmttmp29=Cyc_Parse_apply_tms(_tmp676,_tmp67C,_tmp678,_tmp67B);struct Cyc_Absyn_Tqual _tmp67E;void*_tmp67F;struct Cyc_List_List*_tmp680;struct Cyc_List_List*_tmp681;struct _tuple13 _tmp67D=_stmttmp29;_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;_tmp680=_tmp67D.f3;_tmp681=_tmp67D.f4;
if(_tmp680 != 0){
const char*_tmpC9E;Cyc_Parse_err(((_tmpC9E="parameter with bad type params",_tag_dyneither(_tmpC9E,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp67A)){
const char*_tmpC9F;Cyc_Parse_err(((_tmpC9F="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpC9F,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp67A).f2;
if(_tmp681 != 0){
const char*_tmpCA2;void*_tmpCA1;(_tmpCA1=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA2="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCA2,sizeof(char),40))),_tag_dyneither(_tmpCA1,sizeof(void*),0)));}
{struct _tuple8*_tmpCA3;yyval=Cyc_YY40(((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3->f1=idopt,((_tmpCA3->f2=_tmp67E,((_tmpCA3->f3=_tmp67F,_tmpCA3)))))))));}
# 2235
break;};};};}case 237: _LL317: {
# 2237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2236 "parse.y"
struct _tuple25 _stmttmp26=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp688;struct Cyc_Parse_Type_specifier _tmp689;struct Cyc_List_List*_tmp68A;struct _tuple25 _tmp687=_stmttmp26;_tmp688=_tmp687.f1;_tmp689=_tmp687.f2;_tmp68A=_tmp687.f3;
if(_tmp688.loc == 0)_tmp688.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp68B=Cyc_Parse_speclist2typ(_tmp689,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp68A != 0){
const char*_tmpCA6;void*_tmpCA5;(_tmpCA5=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA6="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCA6,sizeof(char),38))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}
{struct _tuple8*_tmpCA7;yyval=Cyc_YY40(((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->f1=0,((_tmpCA7->f2=_tmp688,((_tmpCA7->f3=_tmp68B,_tmpCA7)))))))));}
# 2243
break;};}case 238: _LL318: {
# 2245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2244 "parse.y"
struct _tuple25 _stmttmp24=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp690;struct Cyc_Parse_Type_specifier _tmp691;struct Cyc_List_List*_tmp692;struct _tuple25 _tmp68F=_stmttmp24;_tmp690=_tmp68F.f1;_tmp691=_tmp68F.f2;_tmp692=_tmp68F.f3;
if(_tmp690.loc == 0)_tmp690.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp693=Cyc_Parse_speclist2typ(_tmp691,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp694=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _stmttmp25=Cyc_Parse_apply_tms(_tmp690,_tmp693,_tmp692,_tmp694);struct Cyc_Absyn_Tqual _tmp696;void*_tmp697;struct Cyc_List_List*_tmp698;struct Cyc_List_List*_tmp699;struct _tuple13 _tmp695=_stmttmp25;_tmp696=_tmp695.f1;_tmp697=_tmp695.f2;_tmp698=_tmp695.f3;_tmp699=_tmp695.f4;
if(_tmp698 != 0){
const char*_tmpCAA;void*_tmpCA9;(_tmpCA9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAA="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCAA,sizeof(char),49))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
# 2252
if(_tmp699 != 0){
const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAD="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCAD,sizeof(char),38))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}
{struct _tuple8*_tmpCAE;yyval=Cyc_YY40(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->f1=0,((_tmpCAE->f2=_tmp696,((_tmpCAE->f3=_tmp697,_tmpCAE)))))))));}
# 2256
break;};}case 239: _LL319: {
# 2258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2260 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 240: _LL31A: {
# 2263
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2264 "parse.y"
{struct _dyneither_ptr*_tmpCB1;struct Cyc_List_List*_tmpCB0;yyval=Cyc_YY39(((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->hd=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCB1)))),((_tmpCB0->tl=0,_tmpCB0)))))));}
break;}case 241: _LL31B: {
# 2267
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2269
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2266 "parse.y"
{struct _dyneither_ptr*_tmpCB4;struct Cyc_List_List*_tmpCB3;yyval=Cyc_YY39(((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCB4)))),((_tmpCB3->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCB3)))))));}
break;}case 242: _LL31C: {
# 2269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2270 "parse.y"
yyval=yyyyvsp[0];
break;}case 243: _LL31D: {
# 2273
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2275
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2271 "parse.y"
yyval=yyyyvsp[0];
break;}case 244: _LL31E: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2276 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCB7;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCB6;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=35,((_tmpCB7.f1=0,((_tmpCB7.f2=0,_tmpCB7)))))),_tmpCB6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31F: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2278 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBA;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCB9;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=35,((_tmpCBA.f1=0,((_tmpCBA.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBA)))))),_tmpCB9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL320: {
# 2281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2280 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBD;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBC;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=35,((_tmpCBD.f1=0,((_tmpCBD.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBD)))))),_tmpCBC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 247: _LL321: {
# 2283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2282 "parse.y"
struct _dyneither_ptr*_tmpCC0;struct _tuple0*_tmpCBF;struct Cyc_Absyn_Vardecl*_tmp6A9=Cyc_Absyn_new_vardecl(((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=Cyc_Absyn_Loc_n,((_tmpCBF->f2=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCC0)))),_tmpCBF)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2285
(_tmp6A9->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCC3;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCC2;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC3.tag=26,((_tmpCC3.f1=_tmp6A9,((_tmpCC3.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC3.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCC3.f4=0,_tmpCC3)))))))))),_tmpCC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2288
break;}case 248: _LL322: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2289 "parse.y"
void*_tmp6AE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCC6;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCC5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC6.tag=27,((_tmpCC6.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC6.f2=_tmp6AE,((_tmpCC6.f3=0,_tmpCC6)))))))),_tmpCC5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2292
break;}case 249: _LL323: {
# 2294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpCC9;struct Cyc_List_List*_tmpCC8;yyval=Cyc_YY7(((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8->hd=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9->f1=0,((_tmpCC9->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCC9)))))),((_tmpCC8->tl=0,_tmpCC8)))))));}
break;}case 250: _LL324: {
# 2300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2299 "parse.y"
{struct _tuple30*_tmpCCC;struct Cyc_List_List*_tmpCCB;yyval=Cyc_YY7(((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->hd=((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCCC->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCCC)))))),((_tmpCCB->tl=0,_tmpCCB)))))));}
break;}case 251: _LL325: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2301 "parse.y"
{struct _tuple30*_tmpCCF;struct Cyc_List_List*_tmpCCE;yyval=Cyc_YY7(((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->hd=((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=0,((_tmpCCF->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCCF)))))),((_tmpCCE->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCCE)))))));}
break;}case 252: _LL326: {
# 2304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2303 "parse.y"
{struct _tuple30*_tmpCD2;struct Cyc_List_List*_tmpCD1;yyval=Cyc_YY7(((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD2->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCD2)))))),((_tmpCD1->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCD1)))))));}
break;}case 253: _LL327: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2307 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 254: _LL328: {
# 2310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2312 "parse.y"
{struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY44(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCD3->tl=0,_tmpCD3)))))));}
break;}case 255: _LL329: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2313 "parse.y"
{struct Cyc_List_List*_tmpCD4;yyval=Cyc_YY44(((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCD4->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCD4)))))));}
break;}case 256: _LL32A: {
# 2316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2317 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCD7;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCD6;yyval=Cyc_YY45((void*)((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6[0]=((_tmpCD7.tag=0,((_tmpCD7.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCD7)))),_tmpCD6)))));}
break;}case 257: _LL32B: {
# 2320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2318 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCDD;struct _dyneither_ptr*_tmpCDC;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCDB;yyval=Cyc_YY45((void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDD.tag=1,((_tmpCDD.f1=((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCDC)))),_tmpCDD)))),_tmpCDB)))));}
break;}case 258: _LL32C: {
# 2321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2323 "parse.y"
struct _tuple25 _stmttmp23=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C1;struct Cyc_Parse_Type_specifier _tmp6C2;struct Cyc_List_List*_tmp6C3;struct _tuple25 _tmp6C0=_stmttmp23;_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;{
void*_tmp6C4=Cyc_Parse_speclist2typ(_tmp6C2,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6C3 != 0){
const char*_tmpCE0;void*_tmpCDF;(_tmpCDF=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE0="ignoring attributes in type",_tag_dyneither(_tmpCE0,sizeof(char),28))),_tag_dyneither(_tmpCDF,sizeof(void*),0)));}
{struct _tuple8*_tmpCE1;yyval=Cyc_YY40(((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=0,((_tmpCE1->f2=_tmp6C1,((_tmpCE1->f3=_tmp6C4,_tmpCE1)))))))));}
# 2329
break;};}case 259: _LL32D: {
# 2331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2330 "parse.y"
struct _tuple25 _stmttmp22=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6C9;struct Cyc_Parse_Type_specifier _tmp6CA;struct Cyc_List_List*_tmp6CB;struct _tuple25 _tmp6C8=_stmttmp22;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;_tmp6CB=_tmp6C8.f3;{
void*_tmp6CC=Cyc_Parse_speclist2typ(_tmp6CA,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6CD=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6CE=Cyc_Parse_apply_tms(_tmp6C9,_tmp6CC,_tmp6CB,_tmp6CD);
if(_tmp6CE.f3 != 0){
# 2336
const char*_tmpCE4;void*_tmpCE3;(_tmpCE3=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE4="bad type params, ignoring",_tag_dyneither(_tmpCE4,sizeof(char),26))),_tag_dyneither(_tmpCE3,sizeof(void*),0)));}
if(_tmp6CE.f4 != 0){
const char*_tmpCE7;void*_tmpCE6;(_tmpCE6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE7="bad specifiers, ignoring",_tag_dyneither(_tmpCE7,sizeof(char),25))),_tag_dyneither(_tmpCE6,sizeof(void*),0)));}
{struct _tuple8*_tmpCE8;yyval=Cyc_YY40(((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8->f1=0,((_tmpCE8->f2=_tmp6CE.f1,((_tmpCE8->f3=_tmp6CE.f2,_tmpCE8)))))))));}
# 2341
break;};}case 260: _LL32E: {
# 2343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2344 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 261: _LL32F: {
# 2347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2345 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCEB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEA;yyval=Cyc_YY47((void*)((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA[0]=((_tmpCEB.tag=24,((_tmpCEB.f1=0,_tmpCEB)))),_tmpCEA)))));}
break;}case 262: _LL330: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2346 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCEE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCED;yyval=Cyc_YY47((void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=24,((_tmpCEE.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCEE)))),_tmpCED)))));}
break;}case 263: _LL331: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2347 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF1;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCF0;yyval=Cyc_YY47((void*)((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF1.tag=25,((_tmpCF1.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCF1)))),_tmpCF0)))));}
break;}case 264: _LL332: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2348 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF7;struct Cyc_List_List*_tmpCF6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF5;yyval=Cyc_YY47((void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF7.tag=24,((_tmpCF7.f1=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF6->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCF6)))))),_tmpCF7)))),_tmpCF5)))));}
break;}case 265: _LL333: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCF8;yyval=Cyc_YY43(((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCF8->tl=0,_tmpCF8)))))));}
break;}case 266: _LL334: {
# 2357
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2355 "parse.y"
{struct Cyc_List_List*_tmpCF9;yyval=Cyc_YY43(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpCF9->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCF9)))))));}
break;}case 267: _LL335: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2360 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCFA;yyval=Cyc_YY33(((_tmpCFA.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCFA)));}
break;}case 268: _LL336: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2362 "parse.y"
yyval=yyyyvsp[0];
break;}case 269: _LL337: {
# 2365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2364 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpCFB;yyval=Cyc_YY33(((_tmpCFB.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpCFB)));}
break;}case 270: _LL338: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2369 "parse.y"
yyval=yyyyvsp[1];
break;}case 271: _LL339: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2371 "parse.y"
{struct Cyc_List_List*_tmpD05;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD04;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD03;struct Cyc_Parse_Abstractdeclarator _tmpD02;yyval=Cyc_YY33(((_tmpD02.tms=((_tmpD05=_region_malloc(yyr,sizeof(*_tmpD05)),((_tmpD05->hd=(void*)((_tmpD03=_region_malloc(yyr,sizeof(*_tmpD03)),((_tmpD03[0]=((_tmpD04.tag=0,((_tmpD04.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD04.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD04)))))),_tmpD03)))),((_tmpD05->tl=0,_tmpD05)))))),_tmpD02)));}
break;}case 272: _LL33A: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2373 "parse.y"
{struct Cyc_List_List*_tmpD0F;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD0E;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD0D;struct Cyc_Parse_Abstractdeclarator _tmpD0C;yyval=Cyc_YY33(((_tmpD0C.tms=((_tmpD0F=_region_malloc(yyr,sizeof(*_tmpD0F)),((_tmpD0F->hd=(void*)((_tmpD0D=_region_malloc(yyr,sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD0E.tag=0,((_tmpD0E.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD0E.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD0E)))))),_tmpD0D)))),((_tmpD0F->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD0F)))))),_tmpD0C)));}
break;}case 273: _LL33B: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2375 "parse.y"
{struct Cyc_List_List*_tmpD19;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD18;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD17;struct Cyc_Parse_Abstractdeclarator _tmpD16;yyval=Cyc_YY33(((_tmpD16.tms=((_tmpD19=_region_malloc(yyr,sizeof(*_tmpD19)),((_tmpD19->hd=(void*)((_tmpD17=_region_malloc(yyr,sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD18.tag=1,((_tmpD18.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD18.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD18.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD18)))))))),_tmpD17)))),((_tmpD19->tl=0,_tmpD19)))))),_tmpD16)));}
break;}case 274: _LL33C: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2377 "parse.y"
{struct Cyc_List_List*_tmpD23;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD22;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD21;struct Cyc_Parse_Abstractdeclarator _tmpD20;yyval=Cyc_YY33(((_tmpD20.tms=((_tmpD23=_region_malloc(yyr,sizeof(*_tmpD23)),((_tmpD23->hd=(void*)((_tmpD21=_region_malloc(yyr,sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD22.tag=1,((_tmpD22.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD22.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD22.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD22)))))))),_tmpD21)))),((_tmpD23->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD23)))))),_tmpD20)));}
# 2380
break;}case 275: _LL33D: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2381 "parse.y"
{struct Cyc_List_List*_tmpD38;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD37;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD36;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD35;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD34;struct Cyc_Parse_Abstractdeclarator _tmpD33;yyval=Cyc_YY33(((_tmpD33.tms=((_tmpD38=_region_malloc(yyr,sizeof(*_tmpD38)),((_tmpD38->hd=(void*)((_tmpD34=_region_malloc(yyr,sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD37.tag=3,((_tmpD37.f1=(void*)((_tmpD35=_region_malloc(yyr,sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD36.tag=1,((_tmpD36.f1=0,((_tmpD36.f2=0,((_tmpD36.f3=0,((_tmpD36.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD36.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD36)))))))))))),_tmpD35)))),_tmpD37)))),_tmpD34)))),((_tmpD38->tl=0,_tmpD38)))))),_tmpD33)));}
# 2383
break;}case 276: _LL33E: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2384 "parse.y"
struct _tuple26*_stmttmp21=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp6F8;int _tmp6F9;struct Cyc_Absyn_VarargInfo*_tmp6FA;void*_tmp6FB;struct Cyc_List_List*_tmp6FC;struct _tuple26*_tmp6F7=_stmttmp21;_tmp6F8=_tmp6F7->f1;_tmp6F9=_tmp6F7->f2;_tmp6FA=_tmp6F7->f3;_tmp6FB=_tmp6F7->f4;_tmp6FC=_tmp6F7->f5;
{struct Cyc_List_List*_tmpD4D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD4C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD4B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD4A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD49;struct Cyc_Parse_Abstractdeclarator _tmpD48;yyval=Cyc_YY33(((_tmpD48.tms=((_tmpD4D=_region_malloc(yyr,sizeof(*_tmpD4D)),((_tmpD4D->hd=(void*)((_tmpD49=_region_malloc(yyr,sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4C.tag=3,((_tmpD4C.f1=(void*)((_tmpD4A=_region_malloc(yyr,sizeof(*_tmpD4A)),((_tmpD4A[0]=((_tmpD4B.tag=1,((_tmpD4B.f1=_tmp6F8,((_tmpD4B.f2=_tmp6F9,((_tmpD4B.f3=_tmp6FA,((_tmpD4B.f4=_tmp6FB,((_tmpD4B.f5=_tmp6FC,_tmpD4B)))))))))))),_tmpD4A)))),_tmpD4C)))),_tmpD49)))),((_tmpD4D->tl=0,_tmpD4D)))))),_tmpD48)));}
# 2387
break;}case 277: _LL33F: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2388 "parse.y"
{struct Cyc_List_List*_tmpD62;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD61;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD60;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5E;struct Cyc_Parse_Abstractdeclarator _tmpD5D;yyval=Cyc_YY33(((_tmpD5D.tms=((_tmpD62=_region_malloc(yyr,sizeof(*_tmpD62)),((_tmpD62->hd=(void*)((_tmpD5E=_region_malloc(yyr,sizeof(*_tmpD5E)),((_tmpD5E[0]=((_tmpD61.tag=3,((_tmpD61.f1=(void*)((_tmpD5F=_region_malloc(yyr,sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD60.tag=1,((_tmpD60.f1=0,((_tmpD60.f2=0,((_tmpD60.f3=0,((_tmpD60.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD60.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD60)))))))))))),_tmpD5F)))),_tmpD61)))),_tmpD5E)))),((_tmpD62->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD62)))))),_tmpD5D)));}
# 2391
break;}case 278: _LL340: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2392 "parse.y"
struct _tuple26*_stmttmp20=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp70A;int _tmp70B;struct Cyc_Absyn_VarargInfo*_tmp70C;void*_tmp70D;struct Cyc_List_List*_tmp70E;struct _tuple26*_tmp709=_stmttmp20;_tmp70A=_tmp709->f1;_tmp70B=_tmp709->f2;_tmp70C=_tmp709->f3;_tmp70D=_tmp709->f4;_tmp70E=_tmp709->f5;
{struct Cyc_List_List*_tmpD77;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD76;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD75;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD74;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD73;struct Cyc_Parse_Abstractdeclarator _tmpD72;yyval=Cyc_YY33(((_tmpD72.tms=((_tmpD77=_region_malloc(yyr,sizeof(*_tmpD77)),((_tmpD77->hd=(void*)((_tmpD73=_region_malloc(yyr,sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD76.tag=3,((_tmpD76.f1=(void*)((_tmpD74=_region_malloc(yyr,sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=1,((_tmpD75.f1=_tmp70A,((_tmpD75.f2=_tmp70B,((_tmpD75.f3=_tmp70C,((_tmpD75.f4=_tmp70D,((_tmpD75.f5=_tmp70E,_tmpD75)))))))))))),_tmpD74)))),_tmpD76)))),_tmpD73)))),((_tmpD77->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD77)))))),_tmpD72)));}
# 2396
break;}case 279: _LL341: {
# 2398
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2398 "parse.y"
struct Cyc_List_List*_tmp715=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD81;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD80;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD7F;struct Cyc_Parse_Abstractdeclarator _tmpD7E;yyval=Cyc_YY33(((_tmpD7E.tms=((_tmpD81=_region_malloc(yyr,sizeof(*_tmpD81)),((_tmpD81->hd=(void*)((_tmpD7F=_region_malloc(yyr,sizeof(*_tmpD7F)),((_tmpD7F[0]=((_tmpD80.tag=4,((_tmpD80.f1=_tmp715,((_tmpD80.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD80.f3=0,_tmpD80)))))))),_tmpD7F)))),((_tmpD81->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD81)))))),_tmpD7E)));}
# 2402
break;}case 280: _LL342: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2403 "parse.y"
{struct Cyc_List_List*_tmpD8B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD8A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD89;struct Cyc_Parse_Abstractdeclarator _tmpD88;yyval=Cyc_YY33(((_tmpD88.tms=((_tmpD8B=_region_malloc(yyr,sizeof(*_tmpD8B)),((_tmpD8B->hd=(void*)((_tmpD89=_region_malloc(yyr,sizeof(*_tmpD89)),((_tmpD89[0]=((_tmpD8A.tag=5,((_tmpD8A.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD8A.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpD8A)))))),_tmpD89)))),((_tmpD8B->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD8B)))))),_tmpD88)));}
# 2405
break;}case 281: _LL343: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2409 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL344: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2410 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL345: {
# 2413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2411 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL346: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2412 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL347: {
# 2415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL348: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2414 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL349: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2417 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD8E;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD8D;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8E.tag=16,((_tmpD8E.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD8E)))),_tmpD8D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 288: _LL34A:
# 2420
 yyval=Cyc_YY5(0);
break;case 289: _LL34B: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2423 "parse.y"
{const char*_tmpD8F;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD8F="open",_tag_dyneither(_tmpD8F,sizeof(char),5))))!= 0){
const char*_tmpD90;Cyc_Parse_err(((_tmpD90="expecting `open'",_tag_dyneither(_tmpD90,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2427
break;}case 290: _LL34C: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD96;struct _dyneither_ptr*_tmpD95;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD94;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD96.tag=13,((_tmpD96.f1=((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD95)))),((_tmpD96.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD96)))))),_tmpD94)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 291: _LL34D: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2436 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL34E: {
# 2439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2437 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34F: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2442 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL350: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2443 "parse.y"
yyval=yyyyvsp[1];
break;}case 295: _LL351: {
# 2446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2448 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 296: _LL352: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2449 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 297: _LL353: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2450 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD9C;struct _dyneither_ptr*_tmpD9B;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD9A;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD9C.tag=13,((_tmpD9C.f1=((_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD9B)))),((_tmpD9C.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpD9C)))))),_tmpD9A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2452
break;}case 298: _LL354: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2452 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDA2;struct _dyneither_ptr*_tmpDA1;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDA0;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpDA2.tag=13,((_tmpDA2.f1=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDA1)))),((_tmpDA2.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDA2)))))),_tmpDA0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2454
break;}case 299: _LL355: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2454 "parse.y"
yyval=yyyyvsp[0];
break;}case 300: _LL356: {
# 2457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2455 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL357: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2456 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA5;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA4;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4[0]=((_tmpDA5.tag=1,((_tmpDA5.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA5)))),_tmpDA4)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 302: _LL358: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2459 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA8;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA7;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA8.tag=1,((_tmpDA8.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA8)))),_tmpDA7)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 303: _LL359: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2464 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL35A: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2466 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL35B: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL35C: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2475 "parse.y"
struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp72F,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL35D: {
# 2479
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2479 "parse.y"
struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp730,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2482
break;}case 308: _LL35E: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2485 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL35F:
# 2488
 yyval=Cyc_YY11(0);
break;case 310: _LL360: {
# 2491
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2493
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2501 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAB;struct Cyc_List_List*_tmpDAA;yyval=Cyc_YY11(((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=((_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDAB->pat_vars=0,((_tmpDAB->where_clause=0,((_tmpDAB->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDAB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAB)))))))))))),((_tmpDAA->tl=0,_tmpDAA)))))));}
# 2504
break;}case 311: _LL361: {
# 2506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2505 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDAE;struct Cyc_List_List*_tmpDAD;yyval=Cyc_YY11(((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->hd=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDAE->pat_vars=0,((_tmpDAE->where_clause=0,((_tmpDAE->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDAE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAE)))))))))))),((_tmpDAD->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDAD)))))));}
break;}case 312: _LL362: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2509 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB1;struct Cyc_List_List*_tmpDB0;yyval=Cyc_YY11(((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->hd=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB1->pat_vars=0,((_tmpDB1->where_clause=0,((_tmpDB1->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDB1->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB1)))))))))))),((_tmpDB0->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDB0)))))));}
break;}case 313: _LL363: {
# 2512
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2514
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2511 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB4;struct Cyc_List_List*_tmpDB3;yyval=Cyc_YY11(((_tmpDB3=_cycalloc(sizeof(*_tmpDB3)),((_tmpDB3->hd=((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB4->pat_vars=0,((_tmpDB4->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB4->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDB4->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB4)))))))))))),((_tmpDB3->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDB3)))))));}
break;}case 314: _LL364: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2515 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDB7;struct Cyc_List_List*_tmpDB6;yyval=Cyc_YY11(((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB7->pat_vars=0,((_tmpDB7->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB7->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDB7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB7)))))))))))),((_tmpDB6->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDB6)))))));}
break;}case 315: _LL365: {
# 2518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2522 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL366: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2526 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL367: {
# 2529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2530 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL368: {
# 2534
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2533 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL369: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2536 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL36A: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2539 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL36B: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2542 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL36C: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2545 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL36D: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2548 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL36E: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2551 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL36F: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2554 "parse.y"
struct Cyc_List_List*_tmp73B=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73B,_tmp73C));
# 2559
break;}case 326: _LL370: {
# 2561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2560 "parse.y"
struct Cyc_List_List*_tmp73D=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73D,_tmp73E));
# 2565
break;}case 327: _LL371: {
# 2567
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2566 "parse.y"
struct Cyc_List_List*_tmp73F=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp740=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73F,_tmp740));
# 2571
break;}case 328: _LL372: {
# 2573
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2575
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2572 "parse.y"
struct Cyc_List_List*_tmp741=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp742=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp741,_tmp742));
# 2577
break;}case 329: _LL373: {
# 2579
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2581
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDB8;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDB8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 330: _LL374: {
# 2585
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2583 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL375: {
# 2586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2584 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL376: {
# 2587
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2589
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2585 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL377: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2586 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL378: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2588 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL379: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2589 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL37A: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2591 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL37B: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2600 "parse.y"
yyval=yyyyvsp[0];
break;}case 338: _LL37C: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2603 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL37D: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2605 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL37E: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2608 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL37F: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2610 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL380: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2613 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL381: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2615 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL382: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2618 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL383: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2620 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL384: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2623 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL385: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2625 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL386: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2628 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL387: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2630 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL388: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2633 "parse.y"
yyval=yyyyvsp[0];
break;}case 351: _LL389: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2635 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL38A: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2637 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL38B: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2640 "parse.y"
yyval=yyyyvsp[0];
break;}case 354: _LL38C: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2642 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL38D: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2644 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL38E: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2646 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38F: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL390: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2651 "parse.y"
yyval=yyyyvsp[0];
break;}case 359: _LL391: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2653 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL392: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2655 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL393: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2658 "parse.y"
yyval=yyyyvsp[0];
break;}case 362: _LL394: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2660 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL395: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL396: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2665 "parse.y"
yyval=yyyyvsp[0];
break;}case 365: _LL397: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2667 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL398: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2669 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL399: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2671 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL39A: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2674 "parse.y"
yyval=yyyyvsp[0];
break;}case 369: _LL39B: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2676 "parse.y"
void*_tmp744=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp744,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2679
break;}case 370: _LL39C: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2682 "parse.y"
yyval=yyyyvsp[0];
break;}case 371: _LL39D: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2685 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 372: _LL39E: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2687 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 373: _LL39F: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2689 "parse.y"
void*_tmp745=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp745,(unsigned int)(yyyylsp[0]).first_line)));
# 2692
break;}case 374: _LL3A0: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2693 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL3A1: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2695 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL3A2: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2700 "parse.y"
yyval=yyyyvsp[0];
break;}case 377: _LL3A3: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2709 "parse.y"
yyval=yyyyvsp[0];
break;}case 378: _LL3A4: {
# 2712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2714 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 379: _LL3A5: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2716 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 380: _LL3A6: {
# 2719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2718 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_stmttmp1F=e->r;void*_tmp746=_stmttmp1F;enum Cyc_Absyn_Sign _tmp748;char _tmp749;enum Cyc_Absyn_Sign _tmp74B;short _tmp74C;enum Cyc_Absyn_Sign _tmp74E;int _tmp74F;struct _dyneither_ptr _tmp751;int _tmp752;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp747=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp747->tag != 0)goto _LL3AB;else{if(((_tmp747->f1).Char_c).tag != 2)goto _LL3AB;_tmp748=((struct _tuple3)((_tmp747->f1).Char_c).val).f1;_tmp749=((struct _tuple3)((_tmp747->f1).Char_c).val).f2;}}_LL3AA:
# 2722
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDBB;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDBA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBA=_cycalloc_atomic(sizeof(*_tmpDBA)),((_tmpDBA[0]=((_tmpDBB.tag=10,((_tmpDBB.f1=_tmp749,_tmpDBB)))),_tmpDBA)))),e->loc));}goto _LL3A8;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp74A->tag != 0)goto _LL3AD;else{if(((_tmp74A->f1).Short_c).tag != 4)goto _LL3AD;_tmp74B=((struct _tuple4)((_tmp74A->f1).Short_c).val).f1;_tmp74C=((struct _tuple4)((_tmp74A->f1).Short_c).val).f2;}}_LL3AC:
# 2724
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDBE;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDBD;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBD=_cycalloc_atomic(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBE.tag=9,((_tmpDBE.f1=_tmp74B,((_tmpDBE.f2=(int)_tmp74C,_tmpDBE)))))),_tmpDBD)))),e->loc));}goto _LL3A8;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp74D->tag != 0)goto _LL3AF;else{if(((_tmp74D->f1).Int_c).tag != 5)goto _LL3AF;_tmp74E=((struct _tuple5)((_tmp74D->f1).Int_c).val).f1;_tmp74F=((struct _tuple5)((_tmp74D->f1).Int_c).val).f2;}}_LL3AE:
# 2726
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDC1;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDC0;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC0=_cycalloc_atomic(sizeof(*_tmpDC0)),((_tmpDC0[0]=((_tmpDC1.tag=9,((_tmpDC1.f1=_tmp74E,((_tmpDC1.f2=_tmp74F,_tmpDC1)))))),_tmpDC0)))),e->loc));}goto _LL3A8;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp750=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp750->tag != 0)goto _LL3B1;else{if(((_tmp750->f1).Float_c).tag != 7)goto _LL3B1;_tmp751=((struct _tuple7)((_tmp750->f1).Float_c).val).f1;_tmp752=((struct _tuple7)((_tmp750->f1).Float_c).val).f2;}}_LL3B0:
# 2728
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDC4;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDC3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=11,((_tmpDC4.f1=_tmp751,((_tmpDC4.f2=_tmp752,_tmpDC4)))))),_tmpDC3)))),e->loc));}goto _LL3A8;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp753=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp753->tag != 0)goto _LL3B3;else{if(((_tmp753->f1).Null_c).tag != 1)goto _LL3B3;}}_LL3B2:
# 2730
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A8;_LL3B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp754=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp754->tag != 0)goto _LL3B5;else{if(((_tmp754->f1).String_c).tag != 8)goto _LL3B5;}}_LL3B4:
# 2732
{const char*_tmpDC5;Cyc_Parse_err(((_tmpDC5="strings cannot occur within patterns",_tag_dyneither(_tmpDC5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp755=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp755->tag != 0)goto _LL3B7;else{if(((_tmp755->f1).Wstring_c).tag != 9)goto _LL3B7;}}_LL3B6:
# 2734
{const char*_tmpDC6;Cyc_Parse_err(((_tmpDC6="strings cannot occur within patterns",_tag_dyneither(_tmpDC6,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp756=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp746;if(_tmp756->tag != 0)goto _LL3B9;else{if(((_tmp756->f1).LongLong_c).tag != 6)goto _LL3B9;}}_LL3B8:
# 2736
{const char*_tmpDC7;Cyc_Parse_unimp(((_tmpDC7="long long's in patterns",_tag_dyneither(_tmpDC7,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B9:;_LL3BA: {
# 2738
const char*_tmpDC8;Cyc_Parse_err(((_tmpDC8="bad constant in case",_tag_dyneither(_tmpDC8,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A8:;}
# 2741
break;}case 381: _LL3A7: {
# 2743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2742 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDCB;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDCA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA[0]=((_tmpDCB.tag=14,((_tmpDCB.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDCB)))),_tmpDCA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 382: _LL3BB: {
# 2745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2744 "parse.y"
{const char*_tmpDCC;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDCC="as",_tag_dyneither(_tmpDCC,sizeof(char),3))))!= 0){
const char*_tmpDCD;Cyc_Parse_err(((_tmpDCD="expecting `as'",_tag_dyneither(_tmpDCD,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDD7;struct _dyneither_ptr*_tmpDD6;struct _tuple0*_tmpDD5;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDD4;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD7.tag=1,((_tmpDD7.f1=Cyc_Absyn_new_vardecl(((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->f1=Cyc_Absyn_Loc_n,((_tmpDD5->f2=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD6)))),_tmpDD5)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD7.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDD7)))))),_tmpDD4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2749
break;}case 383: _LL3BC: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2750 "parse.y"
struct _tuple22 _stmttmp1E=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp76C;int _tmp76D;struct _tuple22 _tmp76B=_stmttmp1E;_tmp76C=_tmp76B.f1;_tmp76D=_tmp76B.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDDA;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDD9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDDA.tag=4,((_tmpDDA.f1=_tmp76C,((_tmpDDA.f2=_tmp76D,_tmpDDA)))))),_tmpDD9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2753
break;}case 384: _LL3BD: {
# 2755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2754 "parse.y"
struct _tuple22 _stmttmp1D=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp771;int _tmp772;struct _tuple22 _tmp770=_stmttmp1D;_tmp771=_tmp770.f1;_tmp772=_tmp770.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDDD;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDDC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDD.tag=15,((_tmpDDD.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDDD.f2=_tmp771,((_tmpDDD.f3=_tmp772,_tmpDDD)))))))),_tmpDDC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2757
break;}case 385: _LL3BE: {
# 2759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2758 "parse.y"
struct _tuple22 _stmttmp1C=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp776;int _tmp777;struct _tuple22 _tmp775=_stmttmp1C;_tmp776=_tmp775.f1;_tmp777=_tmp775.f2;{
struct Cyc_List_List*_tmp778=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE3;struct Cyc_Absyn_AggrInfo*_tmpDE2;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE3.tag=6,((_tmpDE3.f1=((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDE2->targs=0,_tmpDE2)))))),((_tmpDE3.f2=_tmp778,((_tmpDE3.f3=_tmp776,((_tmpDE3.f4=_tmp777,_tmpDE3)))))))))),_tmpDE1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2763
break;};}case 386: _LL3BF: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2764 "parse.y"
struct _tuple22 _stmttmp1B=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp77D;int _tmp77E;struct _tuple22 _tmp77C=_stmttmp1B;_tmp77D=_tmp77C.f1;_tmp77E=_tmp77C.f2;{
struct Cyc_List_List*_tmp77F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDE6;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDE5;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE6.tag=6,((_tmpDE6.f1=0,((_tmpDE6.f2=_tmp77F,((_tmpDE6.f3=_tmp77D,((_tmpDE6.f4=_tmp77E,_tmpDE6)))))))))),_tmpDE5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2768
break;};}case 387: _LL3C0: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2769 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDE9;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDE8;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE9.tag=5,((_tmpDE9.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDE9)))),_tmpDE8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3C1: {
# 2772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2771 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF3;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDF2;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDF1;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDF0;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0[0]=((_tmpDF3.tag=5,((_tmpDF3.f1=Cyc_Absyn_new_pat((void*)((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=5,((_tmpDF2.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDF2)))),_tmpDF1)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDF3)))),_tmpDF0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3C2: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2773 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDFD;struct _dyneither_ptr*_tmpDFC;struct _tuple0*_tmpDFB;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDFA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFD.tag=2,((_tmpDFD.f1=Cyc_Absyn_new_vardecl(((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->f1=Cyc_Absyn_Loc_n,((_tmpDFB->f2=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDFC)))),_tmpDFB)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDFD.f2=
# 2775
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDFD)))))),_tmpDFA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2777
break;}case 390: _LL3C3: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2778 "parse.y"
{const char*_tmpDFE;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDFE="as",_tag_dyneither(_tmpDFE,sizeof(char),3))))!= 0){
const char*_tmpDFF;Cyc_Parse_err(((_tmpDFF="expecting `as'",_tag_dyneither(_tmpDFF,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE09;struct _dyneither_ptr*_tmpE08;struct _tuple0*_tmpE07;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE06;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=((_tmpE09.tag=2,((_tmpE09.f1=Cyc_Absyn_new_vardecl(((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->f1=Cyc_Absyn_Loc_n,((_tmpE07->f2=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE08)))),_tmpE07)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE09.f2=
# 2782
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE09)))))),_tmpE06)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2784
break;}case 391: _LL3C4: {
# 2786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2785 "parse.y"
void*_tmp792=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE1A;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE19;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE18;struct _dyneither_ptr*_tmpE17;struct _tuple0*_tmpE16;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE15;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15[0]=((_tmpE1A.tag=3,((_tmpE1A.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp792),((_tmpE1A.f2=
Cyc_Absyn_new_vardecl(((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->f1=Cyc_Absyn_Loc_n,((_tmpE16->f2=((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE17)))),_tmpE16)))))),(void*)(
(_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE19.tag=19,((_tmpE19.f1=_tmp792,_tmpE19)))),_tmpE18)))),0),_tmpE1A)))))),_tmpE15)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2790
break;}case 392: _LL3C5: {
# 2792
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2791 "parse.y"
struct Cyc_Absyn_Tvar*_tmp799=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE36;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE35;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE34;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE33;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE32;struct _dyneither_ptr*_tmpE31;struct _tuple0*_tmpE30;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE2F;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE36.tag=3,((_tmpE36.f1=_tmp799,((_tmpE36.f2=
Cyc_Absyn_new_vardecl(((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->f1=Cyc_Absyn_Loc_n,((_tmpE30->f2=((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE31)))),_tmpE30)))))),(void*)(
(_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE35.tag=19,((_tmpE35.f1=(void*)((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE34.tag=2,((_tmpE34.f1=_tmp799,_tmpE34)))),_tmpE33)))),_tmpE35)))),_tmpE32)))),0),_tmpE36)))))),_tmpE2F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2796
break;}case 393: _LL3C6: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2799 "parse.y"
{struct _tuple22*_tmpE37;yyval=Cyc_YY13(((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE37->f2=0,_tmpE37)))))));}
break;}case 394: _LL3C7: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2800 "parse.y"
{struct _tuple22*_tmpE38;yyval=Cyc_YY13(((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE38->f2=1,_tmpE38)))))));}
break;}case 395: _LL3C8: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2801 "parse.y"
{struct _tuple22*_tmpE39;yyval=Cyc_YY13(((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->f1=0,((_tmpE39->f2=1,_tmpE39)))))));}
break;}case 396: _LL3C9: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2806 "parse.y"
{struct Cyc_List_List*_tmpE3A;yyval=Cyc_YY14(((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE3A->tl=0,_tmpE3A)))))));}
break;}case 397: _LL3CA: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2808 "parse.y"
{struct Cyc_List_List*_tmpE3B;yyval=Cyc_YY14(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE3B->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE3B)))))));}
break;}case 398: _LL3CB: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2813 "parse.y"
{struct _tuple23*_tmpE3C;yyval=Cyc_YY15(((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->f1=0,((_tmpE3C->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE3C)))))));}
break;}case 399: _LL3CC: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2815 "parse.y"
{struct _tuple23*_tmpE3D;yyval=Cyc_YY15(((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE3D->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE3D)))))));}
break;}case 400: _LL3CD: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2818 "parse.y"
{struct _tuple22*_tmpE3E;yyval=Cyc_YY17(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE3E->f2=0,_tmpE3E)))))));}
break;}case 401: _LL3CE: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2819 "parse.y"
{struct _tuple22*_tmpE3F;yyval=Cyc_YY17(((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE3F->f2=1,_tmpE3F)))))));}
break;}case 402: _LL3CF: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820 "parse.y"
{struct _tuple22*_tmpE40;yyval=Cyc_YY17(((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->f1=0,((_tmpE40->f2=1,_tmpE40)))))));}
break;}case 403: _LL3D0: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2825 "parse.y"
{struct Cyc_List_List*_tmpE41;yyval=Cyc_YY16(((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE41->tl=0,_tmpE41)))))));}
break;}case 404: _LL3D1: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2827 "parse.y"
{struct Cyc_List_List*_tmpE42;yyval=Cyc_YY16(((_tmpE42=_cycalloc(sizeof(*_tmpE42)),((_tmpE42->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE42->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE42)))))));}
break;}case 405: _LL3D2: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 406: _LL3D3: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2835 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 407: _LL3D4: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2840 "parse.y"
yyval=yyyyvsp[0];
break;}case 408: _LL3D5: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2842 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3D6: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2844 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3D7: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848 "parse.y"
yyval=Cyc_YY9(0);
break;}case 411: _LL3D8: {
# 2851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849 "parse.y"
{struct Cyc_Core_Opt*_tmpE43;yyval=Cyc_YY9(((_tmpE43=_cycalloc_atomic(sizeof(*_tmpE43)),((_tmpE43->v=(void*)Cyc_Absyn_Times,_tmpE43)))));}
break;}case 412: _LL3D9: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850 "parse.y"
{struct Cyc_Core_Opt*_tmpE44;yyval=Cyc_YY9(((_tmpE44=_cycalloc_atomic(sizeof(*_tmpE44)),((_tmpE44->v=(void*)Cyc_Absyn_Div,_tmpE44)))));}
break;}case 413: _LL3DA: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851 "parse.y"
{struct Cyc_Core_Opt*_tmpE45;yyval=Cyc_YY9(((_tmpE45=_cycalloc_atomic(sizeof(*_tmpE45)),((_tmpE45->v=(void*)Cyc_Absyn_Mod,_tmpE45)))));}
break;}case 414: _LL3DB: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
{struct Cyc_Core_Opt*_tmpE46;yyval=Cyc_YY9(((_tmpE46=_cycalloc_atomic(sizeof(*_tmpE46)),((_tmpE46->v=(void*)Cyc_Absyn_Plus,_tmpE46)))));}
break;}case 415: _LL3DC: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853 "parse.y"
{struct Cyc_Core_Opt*_tmpE47;yyval=Cyc_YY9(((_tmpE47=_cycalloc_atomic(sizeof(*_tmpE47)),((_tmpE47->v=(void*)Cyc_Absyn_Minus,_tmpE47)))));}
break;}case 416: _LL3DD: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_Core_Opt*_tmpE48;yyval=Cyc_YY9(((_tmpE48=_cycalloc_atomic(sizeof(*_tmpE48)),((_tmpE48->v=(void*)Cyc_Absyn_Bitlshift,_tmpE48)))));}
break;}case 417: _LL3DE: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855 "parse.y"
{struct Cyc_Core_Opt*_tmpE49;yyval=Cyc_YY9(((_tmpE49=_cycalloc_atomic(sizeof(*_tmpE49)),((_tmpE49->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE49)))));}
break;}case 418: _LL3DF: {
# 2858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856 "parse.y"
{struct Cyc_Core_Opt*_tmpE4A;yyval=Cyc_YY9(((_tmpE4A=_cycalloc_atomic(sizeof(*_tmpE4A)),((_tmpE4A->v=(void*)Cyc_Absyn_Bitand,_tmpE4A)))));}
break;}case 419: _LL3E0: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857 "parse.y"
{struct Cyc_Core_Opt*_tmpE4B;yyval=Cyc_YY9(((_tmpE4B=_cycalloc_atomic(sizeof(*_tmpE4B)),((_tmpE4B->v=(void*)Cyc_Absyn_Bitxor,_tmpE4B)))));}
break;}case 420: _LL3E1: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858 "parse.y"
{struct Cyc_Core_Opt*_tmpE4C;yyval=Cyc_YY9(((_tmpE4C=_cycalloc_atomic(sizeof(*_tmpE4C)),((_tmpE4C->v=(void*)Cyc_Absyn_Bitor,_tmpE4C)))));}
break;}case 421: _LL3E2: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2863 "parse.y"
yyval=yyyyvsp[0];
break;}case 422: _LL3E3: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2865 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3E4: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2868 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E5: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2871 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E6: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2873 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E7: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2875 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E8: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2877 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E9: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
yyval=yyyyvsp[0];
break;}case 429: _LL3EA: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2885 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3EB: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2887 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3EC: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2891 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3ED: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2893 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3EE: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2897 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3EF: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2899 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3F0: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2903 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3F1: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2905 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3F2: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2909 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3F3: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2911 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3F4: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL3F5: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F6: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2919 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F7: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL3F8: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2925 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F9: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2927 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3FA: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2929 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3FB: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2931 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3FC: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2935 "parse.y"
yyval=yyyyvsp[0];
break;}case 448: _LL3FD: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2937 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3FE: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2939 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FF: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2943 "parse.y"
yyval=yyyyvsp[0];
break;}case 451: _LL400: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2945 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL401: {
# 2948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2947 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL402: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2951 "parse.y"
yyval=yyyyvsp[0];
break;}case 454: _LL403: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2953 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL404: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2955 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL405: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2957 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL406: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2961 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL407: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2963 "parse.y"
void*_tmp7B8=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7B8,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2966
break;}case 459: _LL408: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2969 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL409: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL40A: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL40B: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2972 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL40C: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2973 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL40D: {
# 2976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2974 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL40E: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2975 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40F: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2977 "parse.y"
void*_tmp7B9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7B9,(unsigned int)(yyyylsp[0]).first_line));
# 2980
break;}case 467: _LL410: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2980 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL411: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2982 "parse.y"
void*_tmp7BA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7BA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2985
break;}case 469: _LL412: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2987 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE52;struct Cyc_Absyn_MallocInfo _tmpE51;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE50;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE52.tag=33,((_tmpE52.f1=((_tmpE51.is_calloc=0,((_tmpE51.rgn=0,((_tmpE51.elt_type=0,((_tmpE51.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE51.fat_result=0,_tmpE51)))))))))),_tmpE52)))),_tmpE50)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2989
break;}case 470: _LL413: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2990 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE58;struct Cyc_Absyn_MallocInfo _tmpE57;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE56;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56[0]=((_tmpE58.tag=33,((_tmpE58.f1=((_tmpE57.is_calloc=0,((_tmpE57.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE57.elt_type=0,((_tmpE57.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE57.fat_result=0,_tmpE57)))))))))),_tmpE58)))),_tmpE56)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2992
break;}case 471: _LL414: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2993 "parse.y"
void*_tmp7C1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE62;void**_tmpE61;struct Cyc_Absyn_MallocInfo _tmpE60;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE5F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE62.tag=33,((_tmpE62.f1=((_tmpE60.is_calloc=1,((_tmpE60.rgn=0,((_tmpE60.elt_type=((_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61[0]=_tmp7C1,_tmpE61)))),((_tmpE60.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE60.fat_result=0,_tmpE60)))))))))),_tmpE62)))),_tmpE5F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2996
break;}case 472: _LL415: {
# 2998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2998 "parse.y"
void*_tmp7C6=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE6C;void**_tmpE6B;struct Cyc_Absyn_MallocInfo _tmpE6A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE69;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((_tmpE6C.tag=33,((_tmpE6C.f1=((_tmpE6A.is_calloc=1,((_tmpE6A.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE6A.elt_type=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=_tmp7C6,_tmpE6B)))),((_tmpE6A.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE6A.fat_result=0,_tmpE6A)))))))))),_tmpE6C)))),_tmpE69)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3001
break;}case 473: _LL416: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3002 "parse.y"
{struct Cyc_Absyn_Exp*_tmpE6D[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE6D[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE6D,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL417: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3004 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE73;struct _dyneither_ptr*_tmpE72;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE71;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=((_tmpE73.tag=37,((_tmpE73.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE73.f2=((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE72)))),_tmpE73)))))),_tmpE71)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL418: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3006 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE79;struct _dyneither_ptr*_tmpE78;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE77;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77[0]=((_tmpE79.tag=37,((_tmpE79.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE79.f2=((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE78)))),_tmpE79)))))),_tmpE77)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL419: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3008 "parse.y"
void*_tmp7D2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7D2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL41A: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3011 "parse.y"
struct _tuple20 _stmttmp1A=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp7D4;struct _dyneither_ptr _tmp7D5;struct _tuple20 _tmp7D3=_stmttmp1A;_tmp7D4=_tmp7D3.f1;_tmp7D5=_tmp7D3.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7D4,_tmp7D5,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL41B: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3016 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 479: _LL41C: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3017 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 480: _LL41D: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 481: _LL41E: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3023 "parse.y"
yyval=yyyyvsp[0];
break;}case 482: _LL41F: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3025 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL420: {
# 3028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3027 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL421: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3029 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL422: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3031 "parse.y"
{struct _dyneither_ptr*_tmpE7A;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE7A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL423: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3033 "parse.y"
{struct _dyneither_ptr*_tmpE7B;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE7B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL424: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3035 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL425: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3037 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL426: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3039 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE7E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE7D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE7D=_cycalloc(sizeof(*_tmpE7D)),((_tmpE7D[0]=((_tmpE7E.tag=24,((_tmpE7E.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE7E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE7E)))))),_tmpE7D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL427: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3041 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE81;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE80;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=((_tmpE81.tag=24,((_tmpE81.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE81.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE81)))))),_tmpE80)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL428: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3046 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE8B;struct _dyneither_ptr*_tmpE8A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE89;struct Cyc_List_List*_tmpE88;yyval=Cyc_YY3(((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->hd=(void*)((_tmpE8B=_cycalloc(sizeof(*_tmpE8B)),((_tmpE8B[0]=((_tmpE89.tag=0,((_tmpE89.f1=((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE8A)))),_tmpE89)))),_tmpE8B)))),((_tmpE88->tl=0,_tmpE88)))))));}
break;}case 492: _LL429: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3049 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE91;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE90;struct Cyc_List_List*_tmpE8F;yyval=Cyc_YY3(((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F->hd=(void*)((_tmpE91=_cycalloc_atomic(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE90.tag=1,((_tmpE90.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE90)))),_tmpE91)))),((_tmpE8F->tl=0,_tmpE8F)))))));}
break;}case 493: _LL42A: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3051 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE9B;struct _dyneither_ptr*_tmpE9A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE99;struct Cyc_List_List*_tmpE98;yyval=Cyc_YY3(((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98->hd=(void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE99.tag=0,((_tmpE99.f1=((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE9A)))),_tmpE99)))),_tmpE9B)))),((_tmpE98->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE98)))))));}
break;}case 494: _LL42B: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3054 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEA1;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEA0;struct Cyc_List_List*_tmpE9F;yyval=Cyc_YY3(((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F->hd=(void*)((_tmpEA1=_cycalloc_atomic(sizeof(*_tmpEA1)),((_tmpEA1[0]=((_tmpEA0.tag=1,((_tmpEA0.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpEA0)))),_tmpEA1)))),((_tmpE9F->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE9F)))))));}
break;}case 495: _LL42C: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL42D: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3062 "parse.y"
yyval=yyyyvsp[0];
break;}case 497: _LL42E: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3064 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42F: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3066 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL430: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3068 "parse.y"
yyval=yyyyvsp[1];
break;}case 500: _LL431: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3073 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL432: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3075 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL433: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3078 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL434: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3081 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEA4;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEA3;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=((_tmpEA4.tag=28,((_tmpEA4.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEA4.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpEA4.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpEA4.f4=0,_tmpEA4)))))))))),_tmpEA3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 504: _LL435: {
# 3084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3084 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL436: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3088 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 506: _LL437: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpEA5;yyval=Cyc_YY6(((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEA5->tl=0,_tmpEA5)))))));}
break;}case 507: _LL438: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3096 "parse.y"
{struct Cyc_List_List*_tmpEA6;yyval=Cyc_YY6(((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA6->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEA6)))))));}
break;}case 508: _LL439: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp7EE=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7EE.f1,_tmp7EE.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL43A: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3103 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL43B: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3104 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL43C: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3106 "parse.y"
struct _dyneither_ptr _tmp7EF=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7EF);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7EF,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3114
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7EF,i,(unsigned int)(yyyylsp[0]).first_line));
# 3116
break;}case 512: _LL43D: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL43E: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121 "parse.y"
{struct _dyneither_ptr*_tmpEA9;struct _tuple0*_tmpEA8;yyval=Cyc_QualId_tok(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=Cyc_Absyn_Rel_n(0),((_tmpEA8->f2=((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA9)))),_tmpEA8)))))));}
break;}case 514: _LL43F: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3122 "parse.y"
yyval=yyyyvsp[0];
break;}case 515: _LL440: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125 "parse.y"
{struct _dyneither_ptr*_tmpEAC;struct _tuple0*_tmpEAB;yyval=Cyc_QualId_tok(((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB->f1=Cyc_Absyn_Rel_n(0),((_tmpEAB->f2=((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEAC)))),_tmpEAB)))))));}
break;}case 516: _LL441: {
# 3128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL442: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL443: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3132 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL444: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL445: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL446: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3141 "parse.y"
break;}case 522: _LL447: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3141 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL448:
# 3146
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
{unsigned int _tmpEB1;unsigned int _tmpEB0;struct _dyneither_ptr _tmpEAF;char*_tmpEAE;unsigned int _tmpEAD;msg=((_tmpEAD=(unsigned int)(sze + 15),((_tmpEAE=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEAD + 1)),((_tmpEAF=_tag_dyneither(_tmpEAE,sizeof(char),_tmpEAD + 1),((((_tmpEB0=_tmpEAD,((_tmpEB2(& _tmpEB1,& _tmpEB0,& _tmpEAE),_tmpEAE[_tmpEB0]=(char)0)))),_tmpEAF))))))));}
{const char*_tmpEB3;Cyc_strcpy(msg,((_tmpEB3="parse error",_tag_dyneither(_tmpEB3,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 292 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6876,x + yyn)]== x){
# 453
{const char*_tmpEB5;const char*_tmpEB4;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEB5=", expecting `",_tag_dyneither(_tmpEB5,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEB4=" or `",_tag_dyneither(_tmpEB4,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(292,x)]);
{const char*_tmpEB6;Cyc_strcat(msg,((_tmpEB6="'",_tag_dyneither(_tmpEB6,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEB7;Cyc_yyerror(((_tmpEB7="parse error",_tag_dyneither(_tmpEB7,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7FD=1;_npop_handler(0);return _tmp7FD;}
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
 if(yyssp_offset == 0){int _tmp7FE=1;_npop_handler(0);return _tmp7FE;}
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
int _tmp7FF=0;_npop_handler(0);return _tmp7FF;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3144 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp809=v;int _tmp80A;char _tmp80B;struct _dyneither_ptr _tmp80C;union Cyc_Absyn_Nmspace _tmp80D;struct _dyneither_ptr*_tmp80E;_LL44B: if((_tmp809.Int_tok).tag != 1)goto _LL44D;_tmp80A=((struct _tuple5)(_tmp809.Int_tok).val).f2;_LL44C:
{const char*_tmpEBB;void*_tmpEBA[1];struct Cyc_Int_pa_PrintArg_struct _tmpEB9;(_tmpEB9.tag=1,((_tmpEB9.f1=(unsigned long)_tmp80A,((_tmpEBA[0]=& _tmpEB9,Cyc_fprintf(Cyc_stderr,((_tmpEBB="%d",_tag_dyneither(_tmpEBB,sizeof(char),3))),_tag_dyneither(_tmpEBA,sizeof(void*),1)))))));}goto _LL44A;_LL44D: if((_tmp809.Char_tok).tag != 2)goto _LL44F;_tmp80B=(char)(_tmp809.Char_tok).val;_LL44E:
{const char*_tmpEBF;void*_tmpEBE[1];struct Cyc_Int_pa_PrintArg_struct _tmpEBD;(_tmpEBD.tag=1,((_tmpEBD.f1=(unsigned long)((int)_tmp80B),((_tmpEBE[0]=& _tmpEBD,Cyc_fprintf(Cyc_stderr,((_tmpEBF="%c",_tag_dyneither(_tmpEBF,sizeof(char),3))),_tag_dyneither(_tmpEBE,sizeof(void*),1)))))));}goto _LL44A;_LL44F: if((_tmp809.String_tok).tag != 3)goto _LL451;_tmp80C=(struct _dyneither_ptr)(_tmp809.String_tok).val;_LL450:
{const char*_tmpEC3;void*_tmpEC2[1];struct Cyc_String_pa_PrintArg_struct _tmpEC1;(_tmpEC1.tag=0,((_tmpEC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp80C),((_tmpEC2[0]=& _tmpEC1,Cyc_fprintf(Cyc_stderr,((_tmpEC3="\"%s\"",_tag_dyneither(_tmpEC3,sizeof(char),5))),_tag_dyneither(_tmpEC2,sizeof(void*),1)))))));}goto _LL44A;_LL451: if((_tmp809.QualId_tok).tag != 5)goto _LL453;_tmp80D=((struct _tuple0*)(_tmp809.QualId_tok).val)->f1;_tmp80E=((struct _tuple0*)(_tmp809.QualId_tok).val)->f2;_LL452: {
# 3150
struct Cyc_List_List*_tmp818=0;
{union Cyc_Absyn_Nmspace _tmp819=_tmp80D;struct Cyc_List_List*_tmp81A;struct Cyc_List_List*_tmp81B;struct Cyc_List_List*_tmp81C;_LL456: if((_tmp819.Rel_n).tag != 1)goto _LL458;_tmp81A=(struct Cyc_List_List*)(_tmp819.Rel_n).val;_LL457:
 _tmp818=_tmp81A;goto _LL455;_LL458: if((_tmp819.Abs_n).tag != 2)goto _LL45A;_tmp81B=(struct Cyc_List_List*)(_tmp819.Abs_n).val;_LL459:
 _tmp818=_tmp81B;goto _LL455;_LL45A: if((_tmp819.C_n).tag != 3)goto _LL45C;_tmp81C=(struct Cyc_List_List*)(_tmp819.C_n).val;_LL45B:
 _tmp818=_tmp81C;goto _LL455;_LL45C: if((_tmp819.Loc_n).tag != 4)goto _LL455;_LL45D:
 goto _LL455;_LL455:;}
# 3157
for(0;_tmp818 != 0;_tmp818=_tmp818->tl){
const char*_tmpEC7;void*_tmpEC6[1];struct Cyc_String_pa_PrintArg_struct _tmpEC5;(_tmpEC5.tag=0,((_tmpEC5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp818->hd)),((_tmpEC6[0]=& _tmpEC5,Cyc_fprintf(Cyc_stderr,((_tmpEC7="%s::",_tag_dyneither(_tmpEC7,sizeof(char),5))),_tag_dyneither(_tmpEC6,sizeof(void*),1)))))));}
{const char*_tmpECB;void*_tmpECA[1];struct Cyc_String_pa_PrintArg_struct _tmpEC9;(_tmpEC9.tag=0,((_tmpEC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp80E),((_tmpECA[0]=& _tmpEC9,Cyc_fprintf(Cyc_stderr,((_tmpECB="%s::",_tag_dyneither(_tmpECB,sizeof(char),5))),_tag_dyneither(_tmpECA,sizeof(void*),1)))))));}
goto _LL44A;}_LL453:;_LL454:
{const char*_tmpECE;void*_tmpECD;(_tmpECD=0,Cyc_fprintf(Cyc_stderr,((_tmpECE="?",_tag_dyneither(_tmpECE,sizeof(char),2))),_tag_dyneither(_tmpECD,sizeof(void*),0)));}goto _LL44A;_LL44A:;}
# 3166
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp825=_new_region("yyr");struct _RegionHandle*yyr=& _tmp825;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp826=Cyc_Parse_parse_result;_npop_handler(0);return _tmp826;};
# 3169
;_pop_region(yyr);};}
