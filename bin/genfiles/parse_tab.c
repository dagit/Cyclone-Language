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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 887
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 914
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 922
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 932
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 937
void*Cyc_Absyn_compress_kb(void*);
# 942
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 944
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 947
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 949
extern void*Cyc_Absyn_sint_typ;
# 951
void*Cyc_Absyn_float_typ(int);
# 999
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1048
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
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1097
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
# 1106
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1108
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1115
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1124
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1129
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1135
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1158
int Cyc_Absyn_fntype_att(void*a);
# 1178
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
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp85C;declarators=((_tmp85C=_region_malloc(r,sizeof(*_tmp85C)),((_tmp85C->tl=declarators,((_tmp85C->hd=_tmp2,_tmp85C))))));}{
struct Cyc_List_List*_tmp85D;exprs=((_tmp85D=_region_malloc(r,sizeof(*_tmp85D)),((_tmp85D->hd=_tmp3,((_tmp85D->tl=exprs,_tmp85D))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp860;struct Cyc_Position_Exit_exn_struct*_tmp85F;(int)_throw((void*)((_tmp85F=_cycalloc_atomic(sizeof(*_tmp85F)),((_tmp85F[0]=((_tmp860.tag=Cyc_Position_Exit,_tmp860)),_tmp85F)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp865;void*_tmp864[2];struct Cyc_String_pa_PrintArg_struct _tmp863;struct Cyc_String_pa_PrintArg_struct _tmp862;(_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp863.tag=0,((_tmp863.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp864[0]=& _tmp863,((_tmp864[1]=& _tmp862,Cyc_fprintf(Cyc_stderr,((_tmp865="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp865,sizeof(char),46))),_tag_dyneither(_tmp864,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp868;void*_tmp867;(_tmp867=0,Cyc_Tcutil_warn(loc,((_tmp868="qualifier on type is ignored",_tag_dyneither(_tmp868,sizeof(char),29))),_tag_dyneither(_tmp867,sizeof(void*),0)));};}
# 199
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 202
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 208
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;struct Cyc_Absyn_Exp*_tmp1A;void*_tmp1C;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp13->tag != 4)goto _LL3;}_LL2:
# 212
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp14->tag != 5)goto _LL5;}_LL4:
# 214
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp15->tag != 7)goto _LL7;}_LL6:
# 216
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp16->tag != 6)goto _LL9;}_LL8:
# 218
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp17->tag != 3)goto _LLB;}_LLA:
# 220
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp18->tag != 2)goto _LLD;}_LLC:
# 222
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp19->tag != 0)goto _LLF;else{_tmp1A=_tmp19->f1;}}_LLE:
# 224
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp86B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86A;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86B.tag=1,((_tmp86B.f1=_tmp1A,_tmp86B)))),_tmp86A)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 226
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp86C;return(_tmp86C.f1=nullable,((_tmp86C.f2=bound,((_tmp86C.f3=zeroterm,((_tmp86C.f4=rgn,_tmp86C)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp879;const char*_tmp878;void*_tmp877[1];struct Cyc_Int_pa_PrintArg_struct _tmp876;struct _tuple0*_tmp875;return(_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875->f1=Cyc_Absyn_Rel_n(0),((_tmp875->f2=(
(_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=(struct _dyneither_ptr)((_tmp876.tag=1,((_tmp876.f1=(unsigned long)enum_counter ++,((_tmp877[0]=& _tmp876,Cyc_aprintf(((_tmp878="__anonymous_enum_%d__",_tag_dyneither(_tmp878,sizeof(char),22))),_tag_dyneither(_tmp877,sizeof(void*),1)))))))),_tmp879)))),_tmp875)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp87A;Cyc_Parse_err(((_tmp87A="bad type params in struct field",_tag_dyneither(_tmp87A,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp87B;Cyc_Parse_err(((_tmp87B="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp87B,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp87C;return(_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C->name=(*_tmp26).f2,((_tmp87C->tq=_tmp27,((_tmp87C->type=_tmp28,((_tmp87C->width=_tmp2B,((_tmp87C->attributes=_tmp2A,((_tmp87C->requires_clause=_tmp2C,_tmp87C)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp87D;return(_tmp87D.Signed_spec=0,((_tmp87D.Unsigned_spec=0,((_tmp87D.Short_spec=0,((_tmp87D.Long_spec=0,((_tmp87D.Long_Long_spec=0,((_tmp87D.Valid_type_spec=0,((_tmp87D.Type_spec=Cyc_Absyn_sint_typ,((_tmp87D.loc=loc,_tmp87D)))))))))))))));}
# 268
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Type_spec=t;
_tmp31.Valid_type_spec=1;
return _tmp31;}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Signed_spec=1;
return _tmp32;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Unsigned_spec=1;
return _tmp33;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
_tmp34.Short_spec=1;
return _tmp34;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
_tmp35.Long_spec=1;
return _tmp35;}
# 296
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 298
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 311 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp36=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp37=(struct _tuple8*)x->hd;struct _tuple8*_tmp38=_tmp37;struct _dyneither_ptr*_tmp39;void*_tmp3B;struct _dyneither_ptr _tmp3C;void**_tmp3F;_LL12: _tmp39=_tmp38->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3A->tag != 19)goto _LL14;else{_tmp3B=(void*)_tmp3A->f1;}};if(!(_tmp39 != 0))goto _LL14;_LL13:
# 316
{void*_tmp40=_tmp3B;void**_tmp42;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL1B;else{_tmp42=(void**)((void**)& _tmp41->f2);}}_LL1A: {
# 320
struct Cyc_String_pa_PrintArg_struct _tmp885;void*_tmp884[1];const char*_tmp883;struct _dyneither_ptr*_tmp882;struct _dyneither_ptr*nm=(_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882[0]=(struct _dyneither_ptr)((_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp884[0]=& _tmp885,Cyc_aprintf(((_tmp883="`%s",_tag_dyneither(_tmp883,sizeof(char),4))),_tag_dyneither(_tmp884,sizeof(void*),1)))))))),_tmp882)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp894;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp893;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp892;struct Cyc_Absyn_Tvar*_tmp891;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp890;*_tmp42=(void*)((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp894.tag=2,((_tmp894.f1=((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->name=nm,((_tmp891->identity=- 1,((_tmp891->kind=(void*)((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893[0]=((_tmp892.tag=0,((_tmp892.f1=& Cyc_Tcutil_ik,_tmp892)))),_tmp893)))),_tmp891)))))))),_tmp894)))),_tmp890))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp897;struct Cyc_List_List*_tmp896;_tmp36=((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896->hd=((_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897->f1=_tmp39,((_tmp897->f2=_tmp3B,_tmp897)))))),((_tmp896->tl=_tmp36,_tmp896))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp89F;void*_tmp89E[1];const char*_tmp89D;struct _dyneither_ptr*_tmp89C;struct _dyneither_ptr*nm=(_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C[0]=(struct _dyneither_ptr)((_tmp89F.tag=0,((_tmp89F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp89E[0]=& _tmp89F,Cyc_aprintf(((_tmp89D="`%s",_tag_dyneither(_tmp89D,sizeof(char),4))),_tag_dyneither(_tmp89E,sizeof(void*),1)))))))),_tmp89C)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8AE;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8AD;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8AC;struct Cyc_Absyn_Tvar*_tmp8AB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AA;*_tmp3F=(void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AE.tag=2,((_tmp8AE.f1=((_tmp8AB=_cycalloc(sizeof(*_tmp8AB)),((_tmp8AB->name=nm,((_tmp8AB->identity=- 1,((_tmp8AB->kind=(void*)((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD[0]=((_tmp8AC.tag=0,((_tmp8AC.f1=& Cyc_Tcutil_rk,_tmp8AC)))),_tmp8AD)))),_tmp8AB)))))))),_tmp8AE)))),_tmp8AA))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 335
return _tmp36;}
# 339
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp57=0;
for(0;x != 0;x=x->tl){
void*_tmp58=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp59=_tmp58;void*_tmp5B;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59;if(_tmp5A->tag != 19)goto _LL20;else{_tmp5B=(void*)_tmp5A->f1;}}_LL1F:
# 344
{struct _tuple18*_tmp8B1;struct Cyc_List_List*_tmp8B0;_tmp57=((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0->hd=((_tmp8B1=_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8B1->f2=_tmp5B,_tmp8B1)))))),((_tmp8B0->tl=_tmp57,_tmp8B0))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 348
return _tmp57;}
# 352
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct _dyneither_ptr*_tmp61;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL25;else{if((((_tmp60->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp60->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp61=(_tmp60->f1)->f2;}}_LL24:
# 355
{struct Cyc_List_List*_tmp62=tags;for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct _tuple18*_tmp63=(struct _tuple18*)_tmp62->hd;struct _dyneither_ptr*_tmp65;void*_tmp66;struct _tuple18*_tmp64=_tmp63;_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;
if(Cyc_strptrcmp(_tmp65,_tmp61)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8B4;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8B3;return Cyc_Absyn_new_exp((void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B4.tag=38,((_tmp8B4.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8B4)))),_tmp8B3)))),e->loc);}}}
# 360
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 363
return e;}
# 368
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp69=t;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;unsigned int _tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69;if(_tmp6A->tag != 8)goto _LL2A;else{_tmp6B=(_tmp6A->f1).elt_type;_tmp6C=(_tmp6A->f1).tq;_tmp6D=(_tmp6A->f1).num_elts;_tmp6E=(_tmp6A->f1).zero_term;_tmp6F=(_tmp6A->f1).zt_loc;}}_LL29: {
# 371
struct Cyc_Absyn_Exp*nelts2=_tmp6D;
if(_tmp6D != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6D);{
# 375
void*_tmp78=Cyc_Parse_substitute_tags(tags,_tmp6B);
if(_tmp6D != nelts2  || _tmp6B != _tmp78){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8BA;struct Cyc_Absyn_ArrayInfo _tmp8B9;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B8;return(void*)((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8BA.tag=8,((_tmp8BA.f1=((_tmp8B9.elt_type=_tmp78,((_tmp8B9.tq=_tmp6C,((_tmp8B9.num_elts=nelts2,((_tmp8B9.zero_term=_tmp6E,((_tmp8B9.zt_loc=_tmp6F,_tmp8B9)))))))))),_tmp8BA)))),_tmp8B8))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 384
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8C0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8BF;union Cyc_Absyn_Constraint*_tmp8BE;_tmp7D=((_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),(((_tmp8BE->Eq_constr).val=(void*)((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8BF.tag=1,((_tmp8BF.f1=_tmp81,_tmp8BF)))),_tmp8C0)))),(((_tmp8BE->Eq_constr).tag=1,_tmp8BE))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8C6;struct Cyc_Absyn_PtrInfo _tmp8C5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C4;return(void*)((_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C6.tag=5,((_tmp8C6.f1=((_tmp8C5.elt_typ=_tmp7C,((_tmp8C5.elt_tq=_tmp72,((_tmp8C5.ptr_atts=(((_tmp8C5.ptr_atts).rgn=_tmp73,(((_tmp8C5.ptr_atts).nullable=_tmp74,(((_tmp8C5.ptr_atts).bounds=_tmp7D,(((_tmp8C5.ptr_atts).zero_term=_tmp76,(((_tmp8C5.ptr_atts).ptrloc=_tmp77,_tmp8C5.ptr_atts)))))))))),_tmp8C5)))))),_tmp8C6)))),_tmp8C4))));}
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
struct _tuple19*_tmp8C7;return(_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->f1=(*t).f2,((_tmp8C7->f2=(*t).f3,_tmp8C7)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8CA="bad declaration in `forarray' statement",_tag_dyneither(_tmp8CA,sizeof(char),40))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8CB;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CB="`H",_tag_dyneither(_tmp8CB,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8CC;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CC="`U",_tag_dyneither(_tmp8CC,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8CD;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CD="`RC",_tag_dyneither(_tmp8CD,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D7;struct _dyneither_ptr*_tmp8D6;struct Cyc_Absyn_Tvar*_tmp8D5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D4;return(void*)((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4[0]=((_tmp8D7.tag=2,((_tmp8D7.f1=((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5->name=((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=s,_tmp8D6)))),((_tmp8D5->identity=- 1,((_tmp8D5->kind=k,_tmp8D5)))))))),_tmp8D7)))),_tmp8D4))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8DA;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8D9;k=(void*)((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=1,((_tmp8DA.f1=0,_tmp8DA)))),_tmp8D9))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8DD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8DC;k=(void*)((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=2,((_tmp8DD.f1=0,((_tmp8DD.f2=_tmp9C,_tmp8DD)))))),_tmp8DC))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8DE;return(_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE->name=t->name,((_tmp8DE->identity=- 1,((_tmp8DE->kind=k,_tmp8DE)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp8E1;void*_tmp8E0;(_tmp8E0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E1="expecting a list of type variables, not types",_tag_dyneither(_tmp8E1,sizeof(char),46))),_tag_dyneither(_tmp8E0,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E3;return(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=2,((_tmp8E4.f1=pr,_tmp8E4)))),_tmp8E3))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 468
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8E7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E6;*_tmpAD=(void*)((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=2,((_tmp8E7.f1=0,((_tmp8E7.f2=k,_tmp8E7)))))),_tmp8E6))));}
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
void*_tmpB3=(void*)tms->hd;void*_tmpB4=_tmpB3;void*_tmpB6;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB4;if(_tmpB5->tag != 3)goto _LL56;else{_tmpB6=(void*)_tmpB5->f1;}}_LL55:
# 496
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 499
void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL5B;}_LL5A:
# 501
{const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,Cyc_Tcutil_warn(loc,((_tmp8EA="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8EA,sizeof(char),93))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8ED="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8ED,sizeof(char),73))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}{
# 509
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
struct Cyc_List_List*_tmpBF=tds;
for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Decl*_tmpC0=(struct Cyc_Absyn_Decl*)_tmpBF->hd;
void*_tmpC1=_tmpC0->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC4;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL60;else{_tmpC4=_tmpC3->f1;}}_LL5F:
# 516
 if(Cyc_zstrptrcmp((*_tmpC4->name).f2,(struct _dyneither_ptr*)_tmpBA->hd)!= 0)
continue;
if(_tmpC4->initializer != 0){
const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F0="initializer found in parameter declaration",_tag_dyneither(_tmp8F0,sizeof(char),43))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F3="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8F3,sizeof(char),47))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
{struct _tuple8*_tmp8F6;struct Cyc_List_List*_tmp8F5;rev_new_params=(
(_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->hd=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->f1=(*_tmpC4->name).f2,((_tmp8F6->f2=_tmpC4->tq,((_tmp8F6->f3=_tmpC4->type,_tmp8F6)))))))),((_tmp8F5->tl=rev_new_params,_tmp8F5))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8F9;void*_tmp8F8;(_tmp8F8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F9="nonvariable declaration in parameter type",_tag_dyneither(_tmp8F9,sizeof(char),42))),_tag_dyneither(_tmp8F8,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpBF == 0){
const char*_tmp8FD;void*_tmp8FC[1];struct Cyc_String_pa_PrintArg_struct _tmp8FB;(_tmp8FB.tag=0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp8FC[0]=& _tmp8FB,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8FD="%s is not given a type",_tag_dyneither(_tmp8FD,sizeof(char),23))),_tag_dyneither(_tmp8FC,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp90C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp90B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp90A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp909;struct Cyc_List_List*_tmp908;return
(_tmp908=_region_malloc(yy,sizeof(*_tmp908)),((_tmp908->hd=(void*)((_tmp90C=_region_malloc(yy,sizeof(*_tmp90C)),((_tmp90C[0]=((_tmp909.tag=3,((_tmp909.f1=(void*)((_tmp90B=_region_malloc(yy,sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90A.tag=1,((_tmp90A.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp90A.f2=0,((_tmp90A.f3=0,((_tmp90A.f4=0,((_tmp90A.f5=0,_tmp90A)))))))))))),_tmp90B)))),_tmp909)))),_tmp90C)))),((_tmp908->tl=0,_tmp908)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp90D;return(_tmp90D=_region_malloc(yy,sizeof(*_tmp90D)),((_tmp90D->hd=(void*)tms->hd,((_tmp90D->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp90D)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp90E;d=((_tmp90E.id=d.id,((_tmp90E.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp90E))));}{
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
enum Cyc_Parse_Storage_class _tmpD7=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_tmpD7){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp90F;Cyc_Parse_err(((_tmp90F="bad storage class on function",_tag_dyneither(_tmp90F,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 578
if(_tmpDE != 0){
# 581
const char*_tmp912;void*_tmp911;(_tmp911=0,Cyc_Tcutil_warn(loc,((_tmp912="bad type params, ignoring",_tag_dyneither(_tmp912,sizeof(char),26))),_tag_dyneither(_tmp911,sizeof(void*),0)));}{
# 583
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpED=0;
{struct Cyc_List_List*_tmpEE=_tmpE8;for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)_tmpEE->hd;struct _dyneither_ptr*_tmpF1;struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct _tuple8*_tmpF0=_tmpEF;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;
if(_tmpF1 == 0){
{const char*_tmp913;Cyc_Parse_err(((_tmp913="missing argument variable in function prototype",_tag_dyneither(_tmp913,sizeof(char),48))),loc);}{
struct _tuple8*_tmp916;struct Cyc_List_List*_tmp915;_tmpED=((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=((_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp916->f2=_tmpF2,((_tmp916->f3=_tmpF3,_tmp916)))))))),((_tmp915->tl=_tmpED,_tmp915))))));};}else{
# 593
struct _tuple8*_tmp919;struct Cyc_List_List*_tmp918;_tmpED=((_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->hd=((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->f1=_tmpF1,((_tmp919->f2=_tmpF2,((_tmp919->f3=_tmpF3,_tmp919)))))))),((_tmp918->tl=_tmpED,_tmp918))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp91A;return(_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->sc=sc,((_tmp91A->name=d.id,((_tmp91A->tvs=_tmpE4,((_tmp91A->is_inline=is_inline,((_tmp91A->effect=_tmpE5,((_tmp91A->ret_tqual=_tmpE6,((_tmp91A->ret_type=_tmpE7,((_tmp91A->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpED),((_tmp91A->c_varargs=_tmpE9,((_tmp91A->cyc_varargs=_tmpEA,((_tmp91A->rgn_po=_tmpEB,((_tmp91A->body=body,((_tmp91A->cached_typ=0,((_tmp91A->param_vardecls=0,((_tmp91A->fn_vardecl=0,((_tmp91A->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),_tmp91A)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp91D;void*_tmp91C;(_tmp91C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91D="declarator is not a function prototype",_tag_dyneither(_tmp91D,sizeof(char),39))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}_LL66:;};};};}static char _tmpFD[52]="at most one type may appear within a type specifier";
# 612
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpFD,_tmpFD,_tmpFD + 52};static char _tmpFE[63]="const or volatile may appear only once within a type specifier";
# 614
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpFE,_tmpFE,_tmpFE + 63};static char _tmpFF[50]="type specifier includes more than one declaration";
# 616
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpFF,_tmpFF,_tmpFF + 50};static char _tmp100[60]="sign specifier may appear only once within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp100,_tmp100,_tmp100 + 60};
# 625
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 628
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp91E;(_tmp91E=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91E,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp91F;(_tmp91F=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91F,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp920;(_tmp920=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp920,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp921,sizeof(void*),0)));}
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
const char*_tmp924;void*_tmp923;(_tmp923=0,Cyc_Tcutil_warn(loc,((_tmp924="missing type within specifier",_tag_dyneither(_tmp924,sizeof(char),30))),_tag_dyneither(_tmp923,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmp107=t;enum Cyc_Absyn_Sign _tmp109;enum Cyc_Absyn_Size_of _tmp10A;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp107;if(_tmp108->tag != 6)goto _LL6E;else{_tmp109=_tmp108->f1;_tmp10A=_tmp108->f2;}}_LL6D:
# 686
 if(_tmp109 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10A);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp925;Cyc_Parse_err(((_tmp925="sign specification on non-integral type",_tag_dyneither(_tmp925,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmp10C=t;enum Cyc_Absyn_Sign _tmp10E;enum Cyc_Absyn_Size_of _tmp10F;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10C;if(_tmp10D->tag != 6)goto _LL73;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;}}_LL72:
# 694
 if(_tmp10F != sz)
t=Cyc_Absyn_int_typ(_tmp10E,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp110=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp10C;if(_tmp110->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp926;Cyc_Parse_err(((_tmp926="size qualifier on non-integral type",_tag_dyneither(_tmp926,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 702
return t;}
# 705
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 709
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp112=d.id;
struct _tuple13 _tmp113=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct _tuple13 _tmp114=_tmp113;_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;_tmp117=_tmp114.f3;_tmp118=_tmp114.f4;
# 715
if(ds->tl == 0){
struct _tuple15*_tmp929;struct Cyc_List_List*_tmp928;return(_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928->hd=((_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929->f1=_tmp112,((_tmp929->f2=_tmp115,((_tmp929->f3=_tmp116,((_tmp929->f4=_tmp117,((_tmp929->f5=_tmp118,_tmp929)))))))))))),((_tmp928->tl=0,_tmp928)))));}else{
# 718
struct _tuple15*_tmp92C;struct Cyc_List_List*_tmp92B;return(_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B->hd=((_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C->f1=_tmp112,((_tmp92C->f2=_tmp115,((_tmp92C->f3=_tmp116,((_tmp92C->f4=_tmp117,((_tmp92C->f5=_tmp118,_tmp92C)))))))))))),((_tmp92B->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp92B)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp92D;return(_tmp92D.f1=tq,((_tmp92D.f2=t,((_tmp92D.f3=0,((_tmp92D.f4=atts,_tmp92D)))))));}{
void*_tmp11E=(void*)tms->hd;void*_tmp11F=_tmp11E;union Cyc_Absyn_Constraint*_tmp121;unsigned int _tmp122;struct Cyc_Absyn_Exp*_tmp124;union Cyc_Absyn_Constraint*_tmp125;unsigned int _tmp126;void*_tmp128;struct Cyc_List_List*_tmp12A;unsigned int _tmp12B;struct Cyc_Absyn_PtrAtts _tmp12D;struct Cyc_Absyn_Tqual _tmp12E;unsigned int _tmp130;struct Cyc_List_List*_tmp131;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp120=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LL7A;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL79:
# 728
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp121,_tmp122),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp123=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp123->tag != 1)goto _LL7C;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;_tmp126=_tmp123->f3;}}_LL7B:
# 731
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp124,_tmp125,_tmp126),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp127=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp127->tag != 3)goto _LL7E;else{_tmp128=(void*)_tmp127->f1;}}_LL7D: {
# 734
void*_tmp132=_tmp128;struct Cyc_List_List*_tmp134;int _tmp135;struct Cyc_Absyn_VarargInfo*_tmp136;void*_tmp137;struct Cyc_List_List*_tmp138;unsigned int _tmp13A;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp133=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp132;if(_tmp133->tag != 1)goto _LL87;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;_tmp136=_tmp133->f3;_tmp137=(void*)_tmp133->f4;_tmp138=_tmp133->f5;}}_LL86: {
# 736
struct Cyc_List_List*typvars=0;
# 738
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp92E;fn_atts=((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E->hd=(void*)as->hd,((_tmp92E->tl=fn_atts,_tmp92E))))));}else{
# 743
struct Cyc_List_List*_tmp92F;new_atts=((_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F->hd=(void*)as->hd,((_tmp92F->tl=new_atts,_tmp92F))))));}}}
# 746
if(tms->tl != 0){
void*_tmp13D=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp13E=_tmp13D;struct Cyc_List_List*_tmp140;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp13F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp13E;if(_tmp13F->tag != 4)goto _LL8C;else{_tmp140=_tmp13F->f1;}}_LL8B:
# 749
 typvars=_tmp140;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 753
 goto _LL89;_LL89:;}
# 757
if(((((!_tmp135  && _tmp136 == 0) && _tmp134 != 0) && _tmp134->tl == 0) && (*((struct _tuple8*)_tmp134->hd)).f1 == 0) && (*((struct _tuple8*)_tmp134->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 762
_tmp134=0;{
# 765
struct Cyc_List_List*_tmp141=Cyc_Parse_get_arg_tags(_tmp134);
# 767
if(_tmp141 != 0)
t=Cyc_Parse_substitute_tags(_tmp141,t);
t=Cyc_Parse_array2ptr(t,0);
# 772
{struct Cyc_List_List*_tmp142=_tmp134;for(0;_tmp142 != 0;_tmp142=_tmp142->tl){
struct _tuple8*_tmp143=(struct _tuple8*)_tmp142->hd;struct _dyneither_ptr*_tmp145;struct Cyc_Absyn_Tqual _tmp146;void**_tmp147;struct _tuple8*_tmp144=_tmp143;_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;_tmp147=(void**)& _tmp144->f3;
if(_tmp141 != 0)
*_tmp147=Cyc_Parse_substitute_tags(_tmp141,*_tmp147);
*_tmp147=Cyc_Parse_array2ptr(*_tmp147,1);}}
# 784
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp137,tq,t,_tmp134,_tmp135,_tmp136,_tmp138,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp139=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp132;if(_tmp139->tag != 0)goto _LL84;else{_tmp13A=_tmp139->f2;}}_LL88: {
# 790
const char*_tmp932;void*_tmp931;(_tmp931=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp13A,((_tmp932="function declaration without parameter types",_tag_dyneither(_tmp932,sizeof(char),45))),_tag_dyneither(_tmp931,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp129=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp129->tag != 4)goto _LL80;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp933;return(_tmp933.f1=tq,((_tmp933.f2=t,((_tmp933.f3=_tmp12A,((_tmp933.f4=atts,_tmp933)))))));}{
# 802
const char*_tmp936;void*_tmp935;(_tmp935=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12B,((_tmp936="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp936,sizeof(char),68))),_tag_dyneither(_tmp935,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp12C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12C->tag != 2)goto _LL82;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp93C;struct Cyc_Absyn_PtrInfo _tmp93B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp93A;return Cyc_Parse_apply_tms(_tmp12E,(void*)((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp93C.tag=5,((_tmp93C.f1=((_tmp93B.elt_typ=t,((_tmp93B.elt_tq=tq,((_tmp93B.ptr_atts=_tmp12D,_tmp93B)))))),_tmp93C)))),_tmp93A)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp12F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12F->tag != 5)goto _LL77;else{_tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;}}_LL83:
# 810
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp131),tms->tl);_LL77:;};}
# 816
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 825
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp151;struct Cyc_Absyn_Tqual _tmp152;void*_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp155;struct _tuple15*_tmp150=t;_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;_tmp153=_tmp150->f3;_tmp154=_tmp150->f4;_tmp155=_tmp150->f5;
# 828
Cyc_Lex_register_typedef(_tmp151);{
# 830
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp156=_tmp153;struct Cyc_Core_Opt*_tmp158;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 1)goto _LL91;else{_tmp158=_tmp157->f1;}}_LL90:
# 834
 type=0;
if(_tmp158 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp158;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp153;goto _LL8E;_LL8E:;}{
# 840
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp942;struct Cyc_Absyn_Typedefdecl*_tmp941;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp940;return Cyc_Absyn_new_decl((void*)((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940[0]=((_tmp942.tag=8,((_tmp942.f1=((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941->name=_tmp151,((_tmp941->tvs=_tmp154,((_tmp941->kind=kind,((_tmp941->defn=type,((_tmp941->atts=_tmp155,((_tmp941->tq=_tmp152,_tmp941)))))))))))))),_tmp942)))),_tmp940)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp945;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp944;return Cyc_Absyn_new_stmt((void*)((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=12,((_tmp945.f1=d,((_tmp945.f2=s,_tmp945)))))),_tmp944)))),d->loc);}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 862
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 866
struct _RegionHandle _tmp15E=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp15E;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp160;struct Cyc_Parse_Type_specifier _tmp161;struct Cyc_List_List*_tmp162;struct Cyc_Parse_Declaration_spec _tmp15F=ds;_tmp160=_tmp15F.tq;_tmp161=_tmp15F.type_specs;_tmp162=_tmp15F.attributes;
if(_tmp160.loc == 0)_tmp160.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp946;Cyc_Parse_err(((_tmp946="inline is allowed only on function definitions",_tag_dyneither(_tmp946,sizeof(char),47))),loc);}{
# 872
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp164=*ds.sc;switch(_tmp164){case Cyc_Parse_Typedef_sc: _LL93:
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
void*_tmp165=Cyc_Parse_collapse_type_specifiers(_tmp161,loc);
if(declarators == 0){
# 904
void*_tmp166=_tmp165;struct Cyc_Absyn_Aggrdecl*_tmp169;struct Cyc_Absyn_Enumdecl*_tmp16C;struct Cyc_Absyn_Datatypedecl*_tmp16F;enum Cyc_Absyn_AggrKind _tmp171;struct _tuple0*_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Datatypedecl**_tmp175;struct _tuple0*_tmp177;int _tmp178;struct Cyc_List_List*_tmp179;struct _tuple0*_tmp17B;struct Cyc_List_List*_tmp17D;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp167->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp168=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp167->f1)->r;if(_tmp168->tag != 0)goto _LL9E;else{_tmp169=_tmp168->f1;}}}_LL9D:
# 906
 _tmp169->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp169->attributes,_tmp162);
_tmp169->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp94C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp94B;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp181=(_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->hd=Cyc_Absyn_new_decl((void*)((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94B.tag=5,((_tmp94B.f1=_tmp169,_tmp94B)))),_tmp94C)))),loc),((_tmp94A->tl=0,_tmp94A)))));_npop_handler(0);return _tmp181;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16A->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp16B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp16A->f1)->r;if(_tmp16B->tag != 1)goto _LLA0;else{_tmp16C=_tmp16B->f1;}}}_LL9F:
# 910
 if(_tmp162 != 0){const char*_tmp94D;Cyc_Parse_err(((_tmp94D="attributes on enum not supported",_tag_dyneither(_tmp94D,sizeof(char),33))),loc);}
_tmp16C->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp953;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp952;struct Cyc_List_List*_tmp951;struct Cyc_List_List*_tmp186=(_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->hd=Cyc_Absyn_new_decl((void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp952.tag=7,((_tmp952.f1=_tmp16C,_tmp952)))),_tmp953)))),loc),((_tmp951->tl=0,_tmp951)))));_npop_handler(0);return _tmp186;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16D->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 2)goto _LLA2;else{_tmp16F=_tmp16E->f1;}}}_LLA1:
# 914
 if(_tmp162 != 0){const char*_tmp954;Cyc_Parse_err(((_tmp954="attributes on datatypes not supported",_tag_dyneither(_tmp954,sizeof(char),38))),loc);}
_tmp16F->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp95A;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp959;struct Cyc_List_List*_tmp958;struct Cyc_List_List*_tmp18B=(_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->hd=Cyc_Absyn_new_decl((void*)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp959.tag=6,((_tmp959.f1=_tmp16F,_tmp959)))),_tmp95A)))),loc),((_tmp958->tl=0,_tmp958)))));_npop_handler(0);return _tmp18B;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166;if(_tmp170->tag != 11)goto _LLA4;else{if((((_tmp170->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp171=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f1;_tmp172=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f2;_tmp173=(_tmp170->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp173);
struct Cyc_Absyn_Aggrdecl*_tmp95B;struct Cyc_Absyn_Aggrdecl*_tmp18D=(_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B->kind=_tmp171,((_tmp95B->sc=s,((_tmp95B->name=_tmp172,((_tmp95B->tvs=_tmp18C,((_tmp95B->impl=0,((_tmp95B->attributes=0,_tmp95B)))))))))))));
if(_tmp162 != 0){const char*_tmp95C;Cyc_Parse_err(((_tmp95C="bad attributes on type declaration",_tag_dyneither(_tmp95C,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp962;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp961;struct Cyc_List_List*_tmp960;struct Cyc_List_List*_tmp192=(_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960->hd=Cyc_Absyn_new_decl((void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((_tmp961.tag=5,((_tmp961.f1=_tmp18D,_tmp961)))),_tmp962)))),loc),((_tmp960->tl=0,_tmp960)))));_npop_handler(0);return _tmp192;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp174->tag != 3)goto _LLA6;else{if((((_tmp174->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp175=(struct Cyc_Absyn_Datatypedecl**)(((_tmp174->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp162 != 0){const char*_tmp963;Cyc_Parse_err(((_tmp963="bad attributes on datatype",_tag_dyneither(_tmp963,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp969;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp968;struct Cyc_List_List*_tmp967;struct Cyc_List_List*_tmp198=(_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->hd=Cyc_Absyn_new_decl((void*)((_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969[0]=((_tmp968.tag=6,((_tmp968.f1=*_tmp175,_tmp968)))),_tmp969)))),loc),((_tmp967->tl=0,_tmp967)))));_npop_handler(0);return _tmp198;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp176->tag != 3)goto _LLA8;else{if((((_tmp176->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp177=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).name;_tmp178=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp179=(_tmp176->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Decl*_tmp19A=Cyc_Absyn_datatype_decl(s,_tmp177,_tmp199,0,_tmp178,loc);
if(_tmp162 != 0){const char*_tmp96A;Cyc_Parse_err(((_tmp96A="bad attributes on datatype",_tag_dyneither(_tmp96A,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp96B;struct Cyc_List_List*_tmp19D=(_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B->hd=_tmp19A,((_tmp96B->tl=0,_tmp96B)))));_npop_handler(0);return _tmp19D;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp166;if(_tmp17A->tag != 13)goto _LLAA;else{_tmp17B=_tmp17A->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp96C;struct Cyc_Absyn_Enumdecl*_tmp19E=(_tmp96C=_cycalloc(sizeof(*_tmp96C)),((_tmp96C->sc=s,((_tmp96C->name=_tmp17B,((_tmp96C->fields=0,_tmp96C)))))));
if(_tmp162 != 0){const char*_tmp96D;Cyc_Parse_err(((_tmp96D="bad attributes on enum",_tag_dyneither(_tmp96D,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp977;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp976;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp975;struct Cyc_List_List*_tmp974;struct Cyc_List_List*_tmp1A4=(_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974->hd=((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977->r=(void*)((_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=((_tmp976.tag=7,((_tmp976.f1=_tmp19E,_tmp976)))),_tmp975)))),((_tmp977->loc=loc,_tmp977)))))),((_tmp974->tl=0,_tmp974)))));_npop_handler(0);return _tmp1A4;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp166;if(_tmp17C->tag != 14)goto _LLAC;else{_tmp17D=_tmp17C->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp97A;struct Cyc_Absyn_Enumdecl*_tmp979;struct Cyc_Absyn_Enumdecl*_tmp1A6=(_tmp979=_cycalloc(sizeof(*_tmp979)),((_tmp979->sc=s,((_tmp979->name=Cyc_Parse_gensym_enum(),((_tmp979->fields=((_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A->v=_tmp17D,_tmp97A)))),_tmp979)))))));
if(_tmp162 != 0){const char*_tmp97B;Cyc_Parse_err(((_tmp97B="bad attributes on enum",_tag_dyneither(_tmp97B,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp985;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp984;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp983;struct Cyc_List_List*_tmp982;struct Cyc_List_List*_tmp1AC=(_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->hd=((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985->r=(void*)((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp984.tag=7,((_tmp984.f1=_tmp1A6,_tmp984)))),_tmp983)))),((_tmp985->loc=loc,_tmp985)))))),((_tmp982->tl=0,_tmp982)))));_npop_handler(0);return _tmp1AC;};}_LLAC:;_LLAD:
{const char*_tmp986;Cyc_Parse_err(((_tmp986="missing declarator",_tag_dyneither(_tmp986,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B0=0;_npop_handler(0);return _tmp1B0;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp1B1=Cyc_Parse_apply_tmss(mkrgn,_tmp160,_tmp165,declarators,_tmp162);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp987;Cyc_Parse_err(((_tmp987="initializer in typedef declaration",_tag_dyneither(_tmp987,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B1);
struct Cyc_List_List*_tmp1B3=decls;_npop_handler(0);return _tmp1B3;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1B4=_tmp1B1;for(0;_tmp1B4 != 0;(_tmp1B4=_tmp1B4->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1B5=(struct _tuple15*)_tmp1B4->hd;struct _tuple0*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct _tuple15*_tmp1B6=_tmp1B5;_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;_tmp1B9=_tmp1B6->f3;_tmp1BA=_tmp1B6->f4;_tmp1BB=_tmp1B6->f5;
if(_tmp1BA != 0){
const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_Tcutil_warn(loc,((_tmp98A="bad type params, ignoring",_tag_dyneither(_tmp98A,sizeof(char),26))),_tag_dyneither(_tmp989,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp98D;void*_tmp98C;(_tmp98C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp98D="unexpected NULL in parse!",_tag_dyneither(_tmp98D,sizeof(char),26))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C1=Cyc_Absyn_new_vardecl(_tmp1B7,_tmp1B9,_tmp1C0);
_tmp1C1->tq=_tmp1B8;
_tmp1C1->sc=s;
_tmp1C1->attributes=_tmp1BB;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp993;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp992;struct Cyc_Absyn_Decl*_tmp991;struct Cyc_Absyn_Decl*_tmp1C2=(_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991->r=(void*)((_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993[0]=((_tmp992.tag=0,((_tmp992.f1=_tmp1C1,_tmp992)))),_tmp993)))),((_tmp991->loc=loc,_tmp991)))));
struct Cyc_List_List*_tmp994;decls=((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994->hd=_tmp1C2,((_tmp994->tl=decls,_tmp994))))));};};}}{
# 970
struct Cyc_List_List*_tmp1C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1C7;};}}};};};};}
# 867
;_pop_region(mkrgn);}
# 977
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp1C8=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_tmp1C8){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 987
{char _tmp1C9=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1C9){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 994
break;case 'T': _LLB5:
# 996
{char _tmp1CA=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1CA){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1003
break;default: _LLBC:
 break;}}
# 1006
{const char*_tmp999;void*_tmp998[2];struct Cyc_String_pa_PrintArg_struct _tmp997;struct Cyc_Int_pa_PrintArg_struct _tmp996;Cyc_Parse_err((struct _dyneither_ptr)((_tmp996.tag=1,((_tmp996.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp997.tag=0,((_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp998[0]=& _tmp997,((_tmp998[1]=& _tmp996,Cyc_aprintf(((_tmp999="bad kind: %s; strlen=%d",_tag_dyneither(_tmp999,sizeof(char),24))),_tag_dyneither(_tmp998,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1CF=p->r;void*_tmp1D0=_tmp1CF;struct _tuple0*_tmp1D2;struct Cyc_Absyn_Vardecl*_tmp1D4;struct Cyc_Absyn_Pat*_tmp1D7;enum Cyc_Absyn_Sign _tmp1DA;int _tmp1DB;char _tmp1DD;struct _dyneither_ptr _tmp1DF;int _tmp1E0;struct _tuple0*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E5;_LLC5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D1->tag != 15)goto _LLC7;else{_tmp1D2=_tmp1D1->f1;}}_LLC6:
 return Cyc_Absyn_unknownid_exp(_tmp1D2,p->loc);_LLC7: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D3->tag != 3)goto _LLC9;else{_tmp1D4=_tmp1D3->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1D3->f2)->r;if(_tmp1D5->tag != 0)goto _LLC9;};}}_LLC8:
# 1015
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1D4->name,p->loc),p->loc);_LLC9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D6->tag != 6)goto _LLCB;else{_tmp1D7=_tmp1D6->f1;}}_LLCA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1D7),p->loc);_LLCB: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D8->tag != 9)goto _LLCD;}_LLCC:
 return Cyc_Absyn_null_exp(p->loc);_LLCD: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D9->tag != 10)goto _LLCF;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LLCE:
 return Cyc_Absyn_int_exp(_tmp1DA,_tmp1DB,p->loc);_LLCF: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1DC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DC->tag != 11)goto _LLD1;else{_tmp1DD=_tmp1DC->f1;}}_LLD0:
 return Cyc_Absyn_char_exp(_tmp1DD,p->loc);_LLD1: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1DE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DE->tag != 12)goto _LLD3;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f2;}}_LLD2:
 return Cyc_Absyn_float_exp(_tmp1DF,_tmp1E0,p->loc);_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E1=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E1->tag != 16)goto _LLD5;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;if(_tmp1E1->f3 != 0)goto _LLD5;}}_LLD4: {
# 1022
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1E2,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1E3);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLD5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1E4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E4->tag != 17)goto _LLD7;else{_tmp1E5=_tmp1E4->f1;}}_LLD6:
 return _tmp1E5;_LLD7:;_LLD8:
# 1027
{const char*_tmp99A;Cyc_Parse_err(((_tmp99A="cannot mix patterns and expressions in case",_tag_dyneither(_tmp99A,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC4:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1E8[14]="$(sign_t,int)";
# 1080 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 14}};
# 1082
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1E9=yy1;struct _tuple5 _tmp1EA;_LLDA: if((_tmp1E9.Int_tok).tag != 1)goto _LLDC;_tmp1EA=(struct _tuple5)(_tmp1E9.Int_tok).val;_LLDB:
# 1086
 yyzzz=_tmp1EA;
goto _LLD9;_LLDC:;_LLDD:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD9:;}
# 1090
return yyzzz;}
# 1092
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.Int_tok).val=yy1,(((_tmp99B.Int_tok).tag=1,_tmp99B)));}static char _tmp1ED[5]="char";
# 1081 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 5}};
# 1083
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;char _tmp1EF;_LLDF: if((_tmp1EE.Char_tok).tag != 2)goto _LLE1;_tmp1EF=(char)(_tmp1EE.Char_tok).val;_LLE0:
# 1087
 yyzzz=_tmp1EF;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDE:;}
# 1091
return yyzzz;}
# 1093
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.Char_tok).val=yy1,(((_tmp99C.Char_tok).tag=2,_tmp99C)));}static char _tmp1F2[13]="string_t<`H>";
# 1082 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 13}};
# 1084
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct _dyneither_ptr _tmp1F4;_LLE4: if((_tmp1F3.String_tok).tag != 3)goto _LLE6;_tmp1F4=(struct _dyneither_ptr)(_tmp1F3.String_tok).val;_LLE5:
# 1088
 yyzzz=_tmp1F4;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE3:;}
# 1092
return yyzzz;}
# 1094
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.String_tok).val=yy1,(((_tmp99D.String_tok).tag=3,_tmp99D)));}static char _tmp1F7[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1085 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 56}};
# 1087
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1F8=yy1;struct _tuple21*_tmp1F9;_LLE9: if((_tmp1F8.YY1).tag != 7)goto _LLEB;_tmp1F9=(struct _tuple21*)(_tmp1F8.YY1).val;_LLEA:
# 1091
 yyzzz=_tmp1F9;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLE8:;}
# 1095
return yyzzz;}
# 1097
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY1).val=yy1,(((_tmp99E.YY1).tag=7,_tmp99E)));}static char _tmp1FC[19]="conref_t<bounds_t>";
# 1086 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 19}};
# 1088
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;union Cyc_Absyn_Constraint*_tmp1FE;_LLEE: if((_tmp1FD.YY2).tag != 8)goto _LLF0;_tmp1FE=(union Cyc_Absyn_Constraint*)(_tmp1FD.YY2).val;_LLEF:
# 1092
 yyzzz=_tmp1FE;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLED:;}
# 1096
return yyzzz;}
# 1098
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY2).val=yy1,(((_tmp99F.YY2).tag=8,_tmp99F)));}static char _tmp201[28]="list_t<offsetof_field_t,`H>";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 28}};
# 1089
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;_LLF3: if((_tmp202.YY3).tag != 9)goto _LLF5;_tmp203=(struct Cyc_List_List*)(_tmp202.YY3).val;_LLF4:
# 1093
 yyzzz=_tmp203;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF2:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY3).val=yy1,(((_tmp9A0.YY3).tag=9,_tmp9A0)));}static char _tmp206[6]="exp_t";
# 1088 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 6}};
# 1090
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_Absyn_Exp*_tmp208;_LLF8: if((_tmp207.YY4).tag != 10)goto _LLFA;_tmp208=(struct Cyc_Absyn_Exp*)(_tmp207.YY4).val;_LLF9:
# 1094
 yyzzz=_tmp208;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLF7:;}
# 1098
return yyzzz;}
# 1100
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY4).val=yy1,(((_tmp9A1.YY4).tag=10,_tmp9A1)));}static char _tmp20B[10]="exp_opt_t";
# 1096 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 10}};
# 1098
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Absyn_Exp*_tmp20D;_LLFD: if((_tmp20C.YY5).tag != 11)goto _LLFF;_tmp20D=(struct Cyc_Absyn_Exp*)(_tmp20C.YY5).val;_LLFE:
# 1102
 yyzzz=_tmp20D;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY5);_LLFC:;}
# 1106
return yyzzz;}
# 1108
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY5).val=yy1,(((_tmp9A2.YY5).tag=11,_tmp9A2)));}static char _tmp210[17]="list_t<exp_t,`H>";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 17}};
# 1099
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct Cyc_List_List*_tmp212;_LL102: if((_tmp211.YY6).tag != 12)goto _LL104;_tmp212=(struct Cyc_List_List*)(_tmp211.YY6).val;_LL103:
# 1103
 yyzzz=_tmp212;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL101:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY6).val=yy1,(((_tmp9A3.YY6).tag=12,_tmp9A3)));}static char _tmp215[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 47}};
# 1100
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;struct Cyc_List_List*_tmp217;_LL107: if((_tmp216.YY7).tag != 13)goto _LL109;_tmp217=(struct Cyc_List_List*)(_tmp216.YY7).val;_LL108:
# 1104
 yyzzz=_tmp217;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL106:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.YY7).val=yy1,(((_tmp9A4.YY7).tag=13,_tmp9A4)));}static char _tmp21A[9]="primop_t";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 9}};
# 1101
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;enum Cyc_Absyn_Primop _tmp21C;_LL10C: if((_tmp21B.YY8).tag != 14)goto _LL10E;_tmp21C=(enum Cyc_Absyn_Primop)(_tmp21B.YY8).val;_LL10D:
# 1105
 yyzzz=_tmp21C;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL10B:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY8).val=yy1,(((_tmp9A5.YY8).tag=14,_tmp9A5)));}static char _tmp21F[19]="opt_t<primop_t,`H>";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 19}};
# 1102
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_Core_Opt*_tmp221;_LL111: if((_tmp220.YY9).tag != 15)goto _LL113;_tmp221=(struct Cyc_Core_Opt*)(_tmp220.YY9).val;_LL112:
# 1106
 yyzzz=_tmp221;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL110:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY9).val=yy1,(((_tmp9A6.YY9).tag=15,_tmp9A6)));}static char _tmp224[7]="qvar_t";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 7}};
# 1103
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct _tuple0*_tmp226;_LL116: if((_tmp225.QualId_tok).tag != 5)goto _LL118;_tmp226=(struct _tuple0*)(_tmp225.QualId_tok).val;_LL117:
# 1107
 yyzzz=_tmp226;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.QualId_tok).val=yy1,(((_tmp9A7.QualId_tok).tag=5,_tmp9A7)));}static char _tmp229[7]="stmt_t";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 7}};
# 1106
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_Absyn_Stmt*_tmp22B;_LL11B: if((_tmp22A.YY10).tag != 16)goto _LL11D;_tmp22B=(struct Cyc_Absyn_Stmt*)(_tmp22A.YY10).val;_LL11C:
# 1110
 yyzzz=_tmp22B;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11A:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.YY10).val=yy1,(((_tmp9A8.YY10).tag=16,_tmp9A8)));}static char _tmp22E[27]="list_t<switch_clause_t,`H>";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 27}};
# 1110
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_List_List*_tmp230;_LL120: if((_tmp22F.YY11).tag != 17)goto _LL122;_tmp230=(struct Cyc_List_List*)(_tmp22F.YY11).val;_LL121:
# 1114
 yyzzz=_tmp230;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL11F:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.YY11).val=yy1,(((_tmp9A9.YY11).tag=17,_tmp9A9)));}static char _tmp233[6]="pat_t";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 6}};
# 1111
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Absyn_Pat*_tmp235;_LL125: if((_tmp234.YY12).tag != 18)goto _LL127;_tmp235=(struct Cyc_Absyn_Pat*)(_tmp234.YY12).val;_LL126:
# 1115
 yyzzz=_tmp235;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL124:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.YY12).val=yy1,(((_tmp9AA.YY12).tag=18,_tmp9AA)));}static char _tmp238[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 28}};
# 1116
struct _tuple22*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple22*_tmp23A;_LL12A: if((_tmp239.YY13).tag != 19)goto _LL12C;_tmp23A=(struct _tuple22*)(_tmp239.YY13).val;_LL12B:
# 1120
 yyzzz=_tmp23A;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL129:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.YY13).val=yy1,(((_tmp9AB.YY13).tag=19,_tmp9AB)));}static char _tmp23D[17]="list_t<pat_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 17}};
# 1117
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_List_List*_tmp23F;_LL12F: if((_tmp23E.YY14).tag != 20)goto _LL131;_tmp23F=(struct Cyc_List_List*)(_tmp23E.YY14).val;_LL130:
# 1121
 yyzzz=_tmp23F;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL12E:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY14).val=yy1,(((_tmp9AC.YY14).tag=20,_tmp9AC)));}static char _tmp242[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 36}};
# 1118
struct _tuple23*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;struct _tuple23*_tmp244;_LL134: if((_tmp243.YY15).tag != 21)goto _LL136;_tmp244=(struct _tuple23*)(_tmp243.YY15).val;_LL135:
# 1122
 yyzzz=_tmp244;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL133:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY15).val=yy1,(((_tmp9AD.YY15).tag=21,_tmp9AD)));}static char _tmp247[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 47}};
# 1119
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*_tmp249;_LL139: if((_tmp248.YY16).tag != 22)goto _LL13B;_tmp249=(struct Cyc_List_List*)(_tmp248.YY16).val;_LL13A:
# 1123
 yyzzz=_tmp249;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL138:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY16).val=yy1,(((_tmp9AE.YY16).tag=22,_tmp9AE)));}static char _tmp24C[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 58}};
# 1120
struct _tuple22*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL13E: if((_tmp24D.YY17).tag != 23)goto _LL140;_tmp24E=(struct _tuple22*)(_tmp24D.YY17).val;_LL13F:
# 1124
 yyzzz=_tmp24E;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL13D:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY17).val=yy1,(((_tmp9AF.YY17).tag=23,_tmp9AF)));}static char _tmp251[9]="fndecl_t";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 9}};
# 1121
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_Absyn_Fndecl*_tmp253;_LL143: if((_tmp252.YY18).tag != 24)goto _LL145;_tmp253=(struct Cyc_Absyn_Fndecl*)(_tmp252.YY18).val;_LL144:
# 1125
 yyzzz=_tmp253;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL142:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY18).val=yy1,(((_tmp9B0.YY18).tag=24,_tmp9B0)));}static char _tmp256[18]="list_t<decl_t,`H>";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 18}};
# 1122
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct Cyc_List_List*_tmp258;_LL148: if((_tmp257.YY19).tag != 25)goto _LL14A;_tmp258=(struct Cyc_List_List*)(_tmp257.YY19).val;_LL149:
# 1126
 yyzzz=_tmp258;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL147:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY19).val=yy1,(((_tmp9B1.YY19).tag=25,_tmp9B1)));}static char _tmp25B[12]="decl_spec_t";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 12}};
# 1124
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_Parse_Declaration_spec _tmp25D;_LL14D: if((_tmp25C.YY20).tag != 26)goto _LL14F;_tmp25D=(struct Cyc_Parse_Declaration_spec)(_tmp25C.YY20).val;_LL14E:
# 1128
 yyzzz=_tmp25D;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL14C:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY20).val=yy1,(((_tmp9B2.YY20).tag=26,_tmp9B2)));}static char _tmp260[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 31}};
# 1125
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple11 _tmp262;_LL152: if((_tmp261.YY21).tag != 27)goto _LL154;_tmp262=(struct _tuple11)(_tmp261.YY21).val;_LL153:
# 1129
 yyzzz=_tmp262;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL151:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY21).val=yy1,(((_tmp9B3.YY21).tag=27,_tmp9B3)));}static char _tmp265[23]="declarator_list_t<`yy>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 23}};
# 1126
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct _tuple12*_tmp267;_LL157: if((_tmp266.YY22).tag != 28)goto _LL159;_tmp267=(struct _tuple12*)(_tmp266.YY22).val;_LL158:
# 1130
 yyzzz=_tmp267;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL156:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY22).val=yy1,(((_tmp9B4.YY22).tag=28,_tmp9B4)));}static char _tmp26A[19]="storage_class_t@`H";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 19}};
# 1127
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;enum Cyc_Parse_Storage_class*_tmp26C;_LL15C: if((_tmp26B.YY23).tag != 29)goto _LL15E;_tmp26C=(enum Cyc_Parse_Storage_class*)(_tmp26B.YY23).val;_LL15D:
# 1131
 yyzzz=_tmp26C;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL15B:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY23).val=yy1,(((_tmp9B5.YY23).tag=29,_tmp9B5)));}static char _tmp26F[17]="type_specifier_t";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 17}};
# 1128
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_Parse_Type_specifier _tmp271;_LL161: if((_tmp270.YY24).tag != 30)goto _LL163;_tmp271=(struct Cyc_Parse_Type_specifier)(_tmp270.YY24).val;_LL162:
# 1132
 yyzzz=_tmp271;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL160:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY24).val=yy1,(((_tmp9B6.YY24).tag=30,_tmp9B6)));}static char _tmp274[12]="aggr_kind_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 12}};
# 1130
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;enum Cyc_Absyn_AggrKind _tmp276;_LL166: if((_tmp275.YY25).tag != 31)goto _LL168;_tmp276=(enum Cyc_Absyn_AggrKind)(_tmp275.YY25).val;_LL167:
# 1134
 yyzzz=_tmp276;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL165:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY25).val=yy1,(((_tmp9B7.YY25).tag=31,_tmp9B7)));}static char _tmp279[8]="tqual_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 8}};
# 1131
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;struct Cyc_Absyn_Tqual _tmp27B;_LL16B: if((_tmp27A.YY26).tag != 32)goto _LL16D;_tmp27B=(struct Cyc_Absyn_Tqual)(_tmp27A.YY26).val;_LL16C:
# 1135
 yyzzz=_tmp27B;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16A:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY26).val=yy1,(((_tmp9B8.YY26).tag=32,_tmp9B8)));}static char _tmp27E[23]="list_t<aggrfield_t,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 23}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;struct Cyc_List_List*_tmp280;_LL170: if((_tmp27F.YY27).tag != 33)goto _LL172;_tmp280=(struct Cyc_List_List*)(_tmp27F.YY27).val;_LL171:
# 1136
 yyzzz=_tmp280;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL16F:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY27).val=yy1,(((_tmp9B9.YY27).tag=33,_tmp9B9)));}static char _tmp283[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 34}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_List_List*_tmp285;_LL175: if((_tmp284.YY28).tag != 34)goto _LL177;_tmp285=(struct Cyc_List_List*)(_tmp284.YY28).val;_LL176:
# 1137
 yyzzz=_tmp285;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL174:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY28).val=yy1,(((_tmp9BA.YY28).tag=34,_tmp9BA)));}static char _tmp288[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 33}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;struct Cyc_List_List*_tmp28A;_LL17A: if((_tmp289.YY29).tag != 35)goto _LL17C;_tmp28A=(struct Cyc_List_List*)(_tmp289.YY29).val;_LL17B:
# 1138
 yyzzz=_tmp28A;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL179:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY29).val=yy1,(((_tmp9BB.YY29).tag=35,_tmp9BB)));}static char _tmp28D[18]="declarator_t<`yy>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 18}};
# 1135
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_Parse_Declarator _tmp28F;_LL17F: if((_tmp28E.YY30).tag != 36)goto _LL181;_tmp28F=(struct Cyc_Parse_Declarator)(_tmp28E.YY30).val;_LL180:
# 1139
 yyzzz=_tmp28F;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL17E:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY30).val=yy1,(((_tmp9BC.YY30).tag=36,_tmp9BC)));}static char _tmp292[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 45}};
# 1136
struct _tuple24*Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct _tuple24*_tmp294;_LL184: if((_tmp293.YY31).tag != 37)goto _LL186;_tmp294=(struct _tuple24*)(_tmp293.YY31).val;_LL185:
# 1140
 yyzzz=_tmp294;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL183:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY31).val=yy1,(((_tmp9BD.YY31).tag=37,_tmp9BD)));}static char _tmp297[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 57}};
# 1137
struct Cyc_List_List*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL189: if((_tmp298.YY32).tag != 38)goto _LL18B;_tmp299=(struct Cyc_List_List*)(_tmp298.YY32).val;_LL18A:
# 1141
 yyzzz=_tmp299;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL188:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY32).val=yy1,(((_tmp9BE.YY32).tag=38,_tmp9BE)));}static char _tmp29C[26]="abstractdeclarator_t<`yy>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 26}};
# 1138
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_Parse_Abstractdeclarator _tmp29E;_LL18E: if((_tmp29D.YY33).tag != 39)goto _LL190;_tmp29E=(struct Cyc_Parse_Abstractdeclarator)(_tmp29D.YY33).val;_LL18F:
# 1142
 yyzzz=_tmp29E;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL18D:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY33).val=yy1,(((_tmp9BF.YY33).tag=39,_tmp9BF)));}static char _tmp2A1[5]="bool";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 5}};
# 1139
int Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;int _tmp2A3;_LL193: if((_tmp2A2.YY34).tag != 40)goto _LL195;_tmp2A3=(int)(_tmp2A2.YY34).val;_LL194:
# 1143
 yyzzz=_tmp2A3;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL192:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY34).val=yy1,(((_tmp9C0.YY34).tag=40,_tmp9C0)));}static char _tmp2A6[8]="scope_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 8}};
# 1140
enum Cyc_Absyn_Scope Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;enum Cyc_Absyn_Scope _tmp2A8;_LL198: if((_tmp2A7.YY35).tag != 41)goto _LL19A;_tmp2A8=(enum Cyc_Absyn_Scope)(_tmp2A7.YY35).val;_LL199:
# 1144
 yyzzz=_tmp2A8;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL197:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY35).val=yy1,(((_tmp9C1.YY35).tag=41,_tmp9C1)));}static char _tmp2AB[16]="datatypefield_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 16}};
# 1141
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_Absyn_Datatypefield*_tmp2AD;_LL19D: if((_tmp2AC.YY36).tag != 42)goto _LL19F;_tmp2AD=(struct Cyc_Absyn_Datatypefield*)(_tmp2AC.YY36).val;_LL19E:
# 1145
 yyzzz=_tmp2AD;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL19C:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY36).val=yy1,(((_tmp9C2.YY36).tag=42,_tmp9C2)));}static char _tmp2B0[27]="list_t<datatypefield_t,`H>";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 27}};
# 1142
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;struct Cyc_List_List*_tmp2B2;_LL1A2: if((_tmp2B1.YY37).tag != 43)goto _LL1A4;_tmp2B2=(struct Cyc_List_List*)(_tmp2B1.YY37).val;_LL1A3:
# 1146
 yyzzz=_tmp2B2;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A1:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY37).val=yy1,(((_tmp9C3.YY37).tag=43,_tmp9C3)));}static char _tmp2B5[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 41}};
# 1143
struct _tuple25 Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;struct _tuple25 _tmp2B7;_LL1A7: if((_tmp2B6.YY38).tag != 44)goto _LL1A9;_tmp2B7=(struct _tuple25)(_tmp2B6.YY38).val;_LL1A8:
# 1147
 yyzzz=_tmp2B7;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1A6:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY38).val=yy1,(((_tmp9C4.YY38).tag=44,_tmp9C4)));}static char _tmp2BA[17]="list_t<var_t,`H>";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 17}};
# 1144
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;struct Cyc_List_List*_tmp2BC;_LL1AC: if((_tmp2BB.YY39).tag != 45)goto _LL1AE;_tmp2BC=(struct Cyc_List_List*)(_tmp2BB.YY39).val;_LL1AD:
# 1148
 yyzzz=_tmp2BC;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1AB:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY39).val=yy1,(((_tmp9C5.YY39).tag=45,_tmp9C5)));}static char _tmp2BF[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 31}};
# 1145
struct _tuple8*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct _tuple8*_tmp2C1;_LL1B1: if((_tmp2C0.YY40).tag != 46)goto _LL1B3;_tmp2C1=(struct _tuple8*)(_tmp2C0.YY40).val;_LL1B2:
# 1149
 yyzzz=_tmp2C1;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B0:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY40).val=yy1,(((_tmp9C6.YY40).tag=46,_tmp9C6)));}static char _tmp2C4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 42}};
# 1146
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL1B6: if((_tmp2C5.YY41).tag != 47)goto _LL1B8;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY41).val;_LL1B7:
# 1150
 yyzzz=_tmp2C6;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1B5:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY41).val=yy1,(((_tmp9C7.YY41).tag=47,_tmp9C7)));}static char _tmp2C9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 115}};
# 1147
struct _tuple26*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct _tuple26*_tmp2CB;_LL1BB: if((_tmp2CA.YY42).tag != 48)goto _LL1BD;_tmp2CB=(struct _tuple26*)(_tmp2CA.YY42).val;_LL1BC:
# 1151
 yyzzz=_tmp2CB;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BA:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY42).val=yy1,(((_tmp9C8.YY42).tag=48,_tmp9C8)));}static char _tmp2CE[18]="list_t<type_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 18}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;_LL1C0: if((_tmp2CF.YY43).tag != 49)goto _LL1C2;_tmp2D0=(struct Cyc_List_List*)(_tmp2CF.YY43).val;_LL1C1:
# 1152
 yyzzz=_tmp2D0;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1BF:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY43).val=yy1,(((_tmp9C9.YY43).tag=49,_tmp9C9)));}static char _tmp2D3[24]="list_t<designator_t,`H>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 24}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;struct Cyc_List_List*_tmp2D5;_LL1C5: if((_tmp2D4.YY44).tag != 50)goto _LL1C7;_tmp2D5=(struct Cyc_List_List*)(_tmp2D4.YY44).val;_LL1C6:
# 1154
 yyzzz=_tmp2D5;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C4:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY44).val=yy1,(((_tmp9CA.YY44).tag=50,_tmp9CA)));}static char _tmp2D8[13]="designator_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 13}};
# 1151
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;void*_tmp2DA;_LL1CA: if((_tmp2D9.YY45).tag != 51)goto _LL1CC;_tmp2DA=(void*)(_tmp2D9.YY45).val;_LL1CB:
# 1155
 yyzzz=_tmp2DA;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C9:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY45).val=yy1,(((_tmp9CB.YY45).tag=51,_tmp9CB)));}static char _tmp2DD[7]="kind_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 7}};
# 1152
struct Cyc_Absyn_Kind*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;struct Cyc_Absyn_Kind*_tmp2DF;_LL1CF: if((_tmp2DE.YY46).tag != 52)goto _LL1D1;_tmp2DF=(struct Cyc_Absyn_Kind*)(_tmp2DE.YY46).val;_LL1D0:
# 1156
 yyzzz=_tmp2DF;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1CE:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY46).val=yy1,(((_tmp9CC.YY46).tag=52,_tmp9CC)));}static char _tmp2E2[7]="type_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 7}};
# 1153
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;void*_tmp2E4;_LL1D4: if((_tmp2E3.YY47).tag != 53)goto _LL1D6;_tmp2E4=(void*)(_tmp2E3.YY47).val;_LL1D5:
# 1157
 yyzzz=_tmp2E4;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D3:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY47).val=yy1,(((_tmp9CD.YY47).tag=53,_tmp9CD)));}static char _tmp2E7[23]="list_t<attribute_t,`H>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 23}};
# 1154
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;struct Cyc_List_List*_tmp2E9;_LL1D9: if((_tmp2E8.YY48).tag != 54)goto _LL1DB;_tmp2E9=(struct Cyc_List_List*)(_tmp2E8.YY48).val;_LL1DA:
# 1158
 yyzzz=_tmp2E9;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1D8:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY48).val=yy1,(((_tmp9CE.YY48).tag=54,_tmp9CE)));}static char _tmp2EC[12]="attribute_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 12}};
# 1155
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;void*_tmp2EE;_LL1DE: if((_tmp2ED.YY49).tag != 55)goto _LL1E0;_tmp2EE=(void*)(_tmp2ED.YY49).val;_LL1DF:
# 1159
 yyzzz=_tmp2EE;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1DD:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY49).val=yy1,(((_tmp9CF.YY49).tag=55,_tmp9CF)));}static char _tmp2F1[12]="enumfield_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 12}};
# 1156
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct Cyc_Absyn_Enumfield*_tmp2F3;_LL1E3: if((_tmp2F2.YY50).tag != 56)goto _LL1E5;_tmp2F3=(struct Cyc_Absyn_Enumfield*)(_tmp2F2.YY50).val;_LL1E4:
# 1160
 yyzzz=_tmp2F3;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E2:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY50).val=yy1,(((_tmp9D0.YY50).tag=56,_tmp9D0)));}static char _tmp2F6[23]="list_t<enumfield_t,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 23}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_List_List*_tmp2F8;_LL1E8: if((_tmp2F7.YY51).tag != 57)goto _LL1EA;_tmp2F8=(struct Cyc_List_List*)(_tmp2F7.YY51).val;_LL1E9:
# 1161
 yyzzz=_tmp2F8;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1E7:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY51).val=yy1,(((_tmp9D1.YY51).tag=57,_tmp9D1)));}static char _tmp2FB[11]="type_opt_t";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 11}};
# 1158
void*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2FC=yy1;void*_tmp2FD;_LL1ED: if((_tmp2FC.YY52).tag != 58)goto _LL1EF;_tmp2FD=(void*)(_tmp2FC.YY52).val;_LL1EE:
# 1162
 yyzzz=_tmp2FD;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1EC:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY52).val=yy1,(((_tmp9D2.YY52).tag=58,_tmp9D2)));}static char _tmp300[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 31}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp301=yy1;struct Cyc_List_List*_tmp302;_LL1F2: if((_tmp301.YY53).tag != 59)goto _LL1F4;_tmp302=(struct Cyc_List_List*)(_tmp301.YY53).val;_LL1F3:
# 1163
 yyzzz=_tmp302;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F1:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY53).val=yy1,(((_tmp9D3.YY53).tag=59,_tmp9D3)));}static char _tmp305[15]="conref_t<bool>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 15}};
# 1160
union Cyc_Absyn_Constraint*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp306=yy1;union Cyc_Absyn_Constraint*_tmp307;_LL1F7: if((_tmp306.YY54).tag != 60)goto _LL1F9;_tmp307=(union Cyc_Absyn_Constraint*)(_tmp306.YY54).val;_LL1F8:
# 1164
 yyzzz=_tmp307;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1F6:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY54).val=yy1,(((_tmp9D4.YY54).tag=60,_tmp9D4)));}static char _tmp30A[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 45}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30B=yy1;struct Cyc_List_List*_tmp30C;_LL1FC: if((_tmp30B.YY55).tag != 61)goto _LL1FE;_tmp30C=(struct Cyc_List_List*)(_tmp30B.YY55).val;_LL1FD:
# 1165
 yyzzz=_tmp30C;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL1FB:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY55).val=yy1,(((_tmp9D5.YY55).tag=61,_tmp9D5)));}static char _tmp30F[20]="pointer_qual_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY56={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 20}};
# 1162
void*Cyc_yyget_YY56(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp310=yy1;void*_tmp311;_LL201: if((_tmp310.YY56).tag != 62)goto _LL203;_tmp311=(void*)(_tmp310.YY56).val;_LL202:
# 1166
 yyzzz=_tmp311;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY56);_LL200:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY56).val=yy1,(((_tmp9D6.YY56).tag=62,_tmp9D6)));}static char _tmp314[21]="pointer_quals_t<`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY57={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 21}};
# 1163
struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp315=yy1;struct Cyc_List_List*_tmp316;_LL206: if((_tmp315.YY57).tag != 63)goto _LL208;_tmp316=(struct Cyc_List_List*)(_tmp315.YY57).val;_LL207:
# 1167
 yyzzz=_tmp316;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_YY57);_LL205:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY57).val=yy1,(((_tmp9D7.YY57).tag=63,_tmp9D7)));}static char _tmp319[21]="$(bool,string_t<`H>)";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 21}};
# 1164
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp31A=yy1;struct _tuple20 _tmp31B;_LL20B: if((_tmp31A.Asm_tok).tag != 6)goto _LL20D;_tmp31B=(struct _tuple20)(_tmp31A.Asm_tok).val;_LL20C:
# 1168
 yyzzz=_tmp31B;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL20A:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.Asm_tok).val=yy1,(((_tmp9D8.Asm_tok).tag=6,_tmp9D8)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1186
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1190
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9D9;return(_tmp9D9.timestamp=0,((_tmp9D9.first_line=0,((_tmp9D9.first_column=0,((_tmp9D9.last_line=0,((_tmp9D9.last_column=0,_tmp9D9)))))))));}
# 1193
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1204 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp31E[2]="$";static char _tmp31F[6]="error";static char _tmp320[12]="$undefined.";static char _tmp321[5]="AUTO";static char _tmp322[9]="REGISTER";static char _tmp323[7]="STATIC";static char _tmp324[7]="EXTERN";static char _tmp325[8]="TYPEDEF";static char _tmp326[5]="VOID";static char _tmp327[5]="CHAR";static char _tmp328[6]="SHORT";static char _tmp329[4]="INT";static char _tmp32A[5]="LONG";static char _tmp32B[6]="FLOAT";static char _tmp32C[7]="DOUBLE";static char _tmp32D[7]="SIGNED";static char _tmp32E[9]="UNSIGNED";static char _tmp32F[6]="CONST";static char _tmp330[9]="VOLATILE";static char _tmp331[9]="RESTRICT";static char _tmp332[7]="STRUCT";static char _tmp333[6]="UNION";static char _tmp334[5]="CASE";static char _tmp335[8]="DEFAULT";static char _tmp336[7]="INLINE";static char _tmp337[7]="SIZEOF";static char _tmp338[9]="OFFSETOF";static char _tmp339[3]="IF";static char _tmp33A[5]="ELSE";static char _tmp33B[7]="SWITCH";static char _tmp33C[6]="WHILE";static char _tmp33D[3]="DO";static char _tmp33E[4]="FOR";static char _tmp33F[5]="GOTO";static char _tmp340[9]="CONTINUE";static char _tmp341[6]="BREAK";static char _tmp342[7]="RETURN";static char _tmp343[5]="ENUM";static char _tmp344[8]="NULL_kw";static char _tmp345[4]="LET";static char _tmp346[6]="THROW";static char _tmp347[4]="TRY";static char _tmp348[6]="CATCH";static char _tmp349[7]="EXPORT";static char _tmp34A[4]="NEW";static char _tmp34B[9]="ABSTRACT";static char _tmp34C[9]="FALLTHRU";static char _tmp34D[6]="USING";static char _tmp34E[10]="NAMESPACE";static char _tmp34F[9]="DATATYPE";static char _tmp350[8]="XTUNION";static char _tmp351[7]="TUNION";static char _tmp352[7]="MALLOC";static char _tmp353[8]="RMALLOC";static char _tmp354[7]="CALLOC";static char _tmp355[8]="RCALLOC";static char _tmp356[5]="SWAP";static char _tmp357[9]="REGION_T";static char _tmp358[6]="TAG_T";static char _tmp359[7]="REGION";static char _tmp35A[5]="RNEW";static char _tmp35B[8]="REGIONS";static char _tmp35C[13]="RESET_REGION";static char _tmp35D[16]="NOZEROTERM_QUAL";static char _tmp35E[14]="ZEROTERM_QUAL";static char _tmp35F[12]="REGION_QUAL";static char _tmp360[7]="PORTON";static char _tmp361[8]="PORTOFF";static char _tmp362[12]="DYNREGION_T";static char _tmp363[8]="NUMELTS";static char _tmp364[8]="VALUEOF";static char _tmp365[10]="VALUEOF_T";static char _tmp366[9]="TAGCHECK";static char _tmp367[13]="NUMELTS_QUAL";static char _tmp368[10]="THIN_QUAL";static char _tmp369[9]="FAT_QUAL";static char _tmp36A[13]="NOTNULL_QUAL";static char _tmp36B[14]="NULLABLE_QUAL";static char _tmp36C[14]="REQUIRES_QUAL";static char _tmp36D[12]="TAGGED_QUAL";static char _tmp36E[16]="EXTENSIBLE_QUAL";static char _tmp36F[15]="RESETABLE_QUAL";static char _tmp370[7]="PTR_OP";static char _tmp371[7]="INC_OP";static char _tmp372[7]="DEC_OP";static char _tmp373[8]="LEFT_OP";static char _tmp374[9]="RIGHT_OP";static char _tmp375[6]="LE_OP";static char _tmp376[6]="GE_OP";static char _tmp377[6]="EQ_OP";static char _tmp378[6]="NE_OP";static char _tmp379[7]="AND_OP";static char _tmp37A[6]="OR_OP";static char _tmp37B[11]="MUL_ASSIGN";static char _tmp37C[11]="DIV_ASSIGN";static char _tmp37D[11]="MOD_ASSIGN";static char _tmp37E[11]="ADD_ASSIGN";static char _tmp37F[11]="SUB_ASSIGN";static char _tmp380[12]="LEFT_ASSIGN";static char _tmp381[13]="RIGHT_ASSIGN";static char _tmp382[11]="AND_ASSIGN";static char _tmp383[11]="XOR_ASSIGN";static char _tmp384[10]="OR_ASSIGN";static char _tmp385[9]="ELLIPSIS";static char _tmp386[11]="LEFT_RIGHT";static char _tmp387[12]="COLON_COLON";static char _tmp388[11]="IDENTIFIER";static char _tmp389[17]="INTEGER_CONSTANT";static char _tmp38A[7]="STRING";static char _tmp38B[8]="WSTRING";static char _tmp38C[19]="CHARACTER_CONSTANT";static char _tmp38D[20]="WCHARACTER_CONSTANT";static char _tmp38E[18]="FLOATING_CONSTANT";static char _tmp38F[9]="TYPE_VAR";static char _tmp390[13]="TYPEDEF_NAME";static char _tmp391[16]="QUAL_IDENTIFIER";static char _tmp392[18]="QUAL_TYPEDEF_NAME";static char _tmp393[10]="ATTRIBUTE";static char _tmp394[4]="ASM";static char _tmp395[4]="';'";static char _tmp396[4]="'{'";static char _tmp397[4]="'}'";static char _tmp398[4]="','";static char _tmp399[4]="'='";static char _tmp39A[4]="'<'";static char _tmp39B[4]="'>'";static char _tmp39C[4]="'('";static char _tmp39D[4]="')'";static char _tmp39E[4]="'_'";static char _tmp39F[4]="'$'";static char _tmp3A0[4]="':'";static char _tmp3A1[4]="'.'";static char _tmp3A2[4]="'['";static char _tmp3A3[4]="']'";static char _tmp3A4[4]="'*'";static char _tmp3A5[4]="'@'";static char _tmp3A6[4]="'?'";static char _tmp3A7[4]="'+'";static char _tmp3A8[4]="'|'";static char _tmp3A9[4]="'^'";static char _tmp3AA[4]="'&'";static char _tmp3AB[4]="'-'";static char _tmp3AC[4]="'/'";static char _tmp3AD[4]="'%'";static char _tmp3AE[4]="'~'";static char _tmp3AF[4]="'!'";static char _tmp3B0[5]="prog";static char _tmp3B1[17]="translation_unit";static char _tmp3B2[12]="export_list";static char _tmp3B3[19]="export_list_values";static char _tmp3B4[21]="external_declaration";static char _tmp3B5[15]="optional_comma";static char _tmp3B6[20]="function_definition";static char _tmp3B7[21]="function_definition2";static char _tmp3B8[13]="using_action";static char _tmp3B9[15]="unusing_action";static char _tmp3BA[17]="namespace_action";static char _tmp3BB[19]="unnamespace_action";static char _tmp3BC[12]="declaration";static char _tmp3BD[19]="resetable_qual_opt";static char _tmp3BE[17]="declaration_list";static char _tmp3BF[23]="declaration_specifiers";static char _tmp3C0[24]="storage_class_specifier";static char _tmp3C1[15]="attributes_opt";static char _tmp3C2[11]="attributes";static char _tmp3C3[15]="attribute_list";static char _tmp3C4[10]="attribute";static char _tmp3C5[15]="type_specifier";static char _tmp3C6[25]="type_specifier_notypedef";static char _tmp3C7[5]="kind";static char _tmp3C8[15]="type_qualifier";static char _tmp3C9[15]="enum_specifier";static char _tmp3CA[11]="enum_field";static char _tmp3CB[22]="enum_declaration_list";static char _tmp3CC[26]="struct_or_union_specifier";static char _tmp3CD[16]="type_params_opt";static char _tmp3CE[16]="struct_or_union";static char _tmp3CF[24]="struct_declaration_list";static char _tmp3D0[25]="struct_declaration_list0";static char _tmp3D1[21]="init_declarator_list";static char _tmp3D2[22]="init_declarator_list0";static char _tmp3D3[16]="init_declarator";static char _tmp3D4[19]="struct_declaration";static char _tmp3D5[25]="specifier_qualifier_list";static char _tmp3D6[35]="notypedef_specifier_qualifier_list";static char _tmp3D7[23]="struct_declarator_list";static char _tmp3D8[24]="struct_declarator_list0";static char _tmp3D9[18]="struct_declarator";static char _tmp3DA[19]="datatype_specifier";static char _tmp3DB[14]="qual_datatype";static char _tmp3DC[19]="datatypefield_list";static char _tmp3DD[20]="datatypefield_scope";static char _tmp3DE[14]="datatypefield";static char _tmp3DF[11]="declarator";static char _tmp3E0[23]="declarator_withtypedef";static char _tmp3E1[18]="direct_declarator";static char _tmp3E2[30]="direct_declarator_withtypedef";static char _tmp3E3[8]="pointer";static char _tmp3E4[12]="one_pointer";static char _tmp3E5[14]="pointer_quals";static char _tmp3E6[13]="pointer_qual";static char _tmp3E7[23]="pointer_null_and_bound";static char _tmp3E8[14]="pointer_bound";static char _tmp3E9[18]="zeroterm_qual_opt";static char _tmp3EA[8]="rgn_opt";static char _tmp3EB[11]="tqual_list";static char _tmp3EC[20]="parameter_type_list";static char _tmp3ED[9]="type_var";static char _tmp3EE[16]="optional_effect";static char _tmp3EF[19]="optional_rgn_order";static char _tmp3F0[10]="rgn_order";static char _tmp3F1[16]="optional_inject";static char _tmp3F2[11]="effect_set";static char _tmp3F3[14]="atomic_effect";static char _tmp3F4[11]="region_set";static char _tmp3F5[15]="parameter_list";static char _tmp3F6[22]="parameter_declaration";static char _tmp3F7[16]="identifier_list";static char _tmp3F8[17]="identifier_list0";static char _tmp3F9[12]="initializer";static char _tmp3FA[18]="array_initializer";static char _tmp3FB[17]="initializer_list";static char _tmp3FC[12]="designation";static char _tmp3FD[16]="designator_list";static char _tmp3FE[11]="designator";static char _tmp3FF[10]="type_name";static char _tmp400[14]="any_type_name";static char _tmp401[15]="type_name_list";static char _tmp402[20]="abstract_declarator";static char _tmp403[27]="direct_abstract_declarator";static char _tmp404[10]="statement";static char _tmp405[13]="open_exp_opt";static char _tmp406[18]="labeled_statement";static char _tmp407[21]="expression_statement";static char _tmp408[19]="compound_statement";static char _tmp409[16]="block_item_list";static char _tmp40A[20]="selection_statement";static char _tmp40B[15]="switch_clauses";static char _tmp40C[20]="iteration_statement";static char _tmp40D[15]="jump_statement";static char _tmp40E[12]="exp_pattern";static char _tmp40F[20]="conditional_pattern";static char _tmp410[19]="logical_or_pattern";static char _tmp411[20]="logical_and_pattern";static char _tmp412[21]="inclusive_or_pattern";static char _tmp413[21]="exclusive_or_pattern";static char _tmp414[12]="and_pattern";static char _tmp415[17]="equality_pattern";static char _tmp416[19]="relational_pattern";static char _tmp417[14]="shift_pattern";static char _tmp418[17]="additive_pattern";static char _tmp419[23]="multiplicative_pattern";static char _tmp41A[13]="cast_pattern";static char _tmp41B[14]="unary_pattern";static char _tmp41C[16]="postfix_pattern";static char _tmp41D[16]="primary_pattern";static char _tmp41E[8]="pattern";static char _tmp41F[19]="tuple_pattern_list";static char _tmp420[20]="tuple_pattern_list0";static char _tmp421[14]="field_pattern";static char _tmp422[19]="field_pattern_list";static char _tmp423[20]="field_pattern_list0";static char _tmp424[11]="expression";static char _tmp425[22]="assignment_expression";static char _tmp426[20]="assignment_operator";static char _tmp427[23]="conditional_expression";static char _tmp428[20]="constant_expression";static char _tmp429[22]="logical_or_expression";static char _tmp42A[23]="logical_and_expression";static char _tmp42B[24]="inclusive_or_expression";static char _tmp42C[24]="exclusive_or_expression";static char _tmp42D[15]="and_expression";static char _tmp42E[20]="equality_expression";static char _tmp42F[22]="relational_expression";static char _tmp430[17]="shift_expression";static char _tmp431[20]="additive_expression";static char _tmp432[26]="multiplicative_expression";static char _tmp433[16]="cast_expression";static char _tmp434[17]="unary_expression";static char _tmp435[15]="unary_operator";static char _tmp436[19]="postfix_expression";static char _tmp437[17]="field_expression";static char _tmp438[19]="primary_expression";static char _tmp439[25]="argument_expression_list";static char _tmp43A[26]="argument_expression_list0";static char _tmp43B[9]="constant";static char _tmp43C[20]="qual_opt_identifier";static char _tmp43D[17]="qual_opt_typedef";static char _tmp43E[18]="struct_union_name";static char _tmp43F[11]="field_name";static char _tmp440[12]="right_angle";
# 1554 "parse.y"
static struct _dyneither_ptr Cyc_yytname[291]={{_tmp31E,_tmp31E,_tmp31E + 2},{_tmp31F,_tmp31F,_tmp31F + 6},{_tmp320,_tmp320,_tmp320 + 12},{_tmp321,_tmp321,_tmp321 + 5},{_tmp322,_tmp322,_tmp322 + 9},{_tmp323,_tmp323,_tmp323 + 7},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 7},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 6},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 3},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 3},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 9},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 4},{_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 4},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 7},{_tmp34A,_tmp34A,_tmp34A + 4},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 9},{_tmp34D,_tmp34D,_tmp34D + 6},{_tmp34E,_tmp34E,_tmp34E + 10},{_tmp34F,_tmp34F,_tmp34F + 9},{_tmp350,_tmp350,_tmp350 + 8},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 8},{_tmp356,_tmp356,_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 9},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 8},{_tmp35C,_tmp35C,_tmp35C + 13},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 8},{_tmp362,_tmp362,_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 8},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,_tmp365 + 10},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 13},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 13},{_tmp36B,_tmp36B,_tmp36B + 14},{_tmp36C,_tmp36C,_tmp36C + 14},{_tmp36D,_tmp36D,_tmp36D + 12},{_tmp36E,_tmp36E,_tmp36E + 16},{_tmp36F,_tmp36F,_tmp36F + 15},{_tmp370,_tmp370,_tmp370 + 7},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 7},{_tmp373,_tmp373,_tmp373 + 8},{_tmp374,_tmp374,_tmp374 + 9},{_tmp375,_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,_tmp377 + 6},{_tmp378,_tmp378,_tmp378 + 6},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 11},{_tmp37C,_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D,_tmp37D + 11},{_tmp37E,_tmp37E,_tmp37E + 11},{_tmp37F,_tmp37F,_tmp37F + 11},{_tmp380,_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 13},{_tmp382,_tmp382,_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 11},{_tmp384,_tmp384,_tmp384 + 10},{_tmp385,_tmp385,_tmp385 + 9},{_tmp386,_tmp386,_tmp386 + 11},{_tmp387,_tmp387,_tmp387 + 12},{_tmp388,_tmp388,_tmp388 + 11},{_tmp389,_tmp389,_tmp389 + 17},{_tmp38A,_tmp38A,_tmp38A + 7},{_tmp38B,_tmp38B,_tmp38B + 8},{_tmp38C,_tmp38C,_tmp38C + 19},{_tmp38D,_tmp38D,_tmp38D + 20},{_tmp38E,_tmp38E,_tmp38E + 18},{_tmp38F,_tmp38F,_tmp38F + 9},{_tmp390,_tmp390,_tmp390 + 13},{_tmp391,_tmp391,_tmp391 + 16},{_tmp392,_tmp392,_tmp392 + 18},{_tmp393,_tmp393,_tmp393 + 10},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 4},{_tmp398,_tmp398,_tmp398 + 4},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 4},{_tmp3A1,_tmp3A1,_tmp3A1 + 4},{_tmp3A2,_tmp3A2,_tmp3A2 + 4},{_tmp3A3,_tmp3A3,_tmp3A3 + 4},{_tmp3A4,_tmp3A4,_tmp3A4 + 4},{_tmp3A5,_tmp3A5,_tmp3A5 + 4},{_tmp3A6,_tmp3A6,_tmp3A6 + 4},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 5},{_tmp3B1,_tmp3B1,_tmp3B1 + 17},{_tmp3B2,_tmp3B2,_tmp3B2 + 12},{_tmp3B3,_tmp3B3,_tmp3B3 + 19},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 15},{_tmp3B6,_tmp3B6,_tmp3B6 + 20},{_tmp3B7,_tmp3B7,_tmp3B7 + 21},{_tmp3B8,_tmp3B8,_tmp3B8 + 13},{_tmp3B9,_tmp3B9,_tmp3B9 + 15},{_tmp3BA,_tmp3BA,_tmp3BA + 17},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 12},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 17},{_tmp3BF,_tmp3BF,_tmp3BF + 23},{_tmp3C0,_tmp3C0,_tmp3C0 + 24},{_tmp3C1,_tmp3C1,_tmp3C1 + 15},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 15},{_tmp3C4,_tmp3C4,_tmp3C4 + 10},{_tmp3C5,_tmp3C5,_tmp3C5 + 15},{_tmp3C6,_tmp3C6,_tmp3C6 + 25},{_tmp3C7,_tmp3C7,_tmp3C7 + 5},{_tmp3C8,_tmp3C8,_tmp3C8 + 15},{_tmp3C9,_tmp3C9,_tmp3C9 + 15},{_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 22},{_tmp3CC,_tmp3CC,_tmp3CC + 26},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 16},{_tmp3CF,_tmp3CF,_tmp3CF + 24},{_tmp3D0,_tmp3D0,_tmp3D0 + 25},{_tmp3D1,_tmp3D1,_tmp3D1 + 21},{_tmp3D2,_tmp3D2,_tmp3D2 + 22},{_tmp3D3,_tmp3D3,_tmp3D3 + 16},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 25},{_tmp3D6,_tmp3D6,_tmp3D6 + 35},{_tmp3D7,_tmp3D7,_tmp3D7 + 23},{_tmp3D8,_tmp3D8,_tmp3D8 + 24},{_tmp3D9,_tmp3D9,_tmp3D9 + 18},{_tmp3DA,_tmp3DA,_tmp3DA + 19},{_tmp3DB,_tmp3DB,_tmp3DB + 14},{_tmp3DC,_tmp3DC,_tmp3DC + 19},{_tmp3DD,_tmp3DD,_tmp3DD + 20},{_tmp3DE,_tmp3DE,_tmp3DE + 14},{_tmp3DF,_tmp3DF,_tmp3DF + 11},{_tmp3E0,_tmp3E0,_tmp3E0 + 23},{_tmp3E1,_tmp3E1,_tmp3E1 + 18},{_tmp3E2,_tmp3E2,_tmp3E2 + 30},{_tmp3E3,_tmp3E3,_tmp3E3 + 8},{_tmp3E4,_tmp3E4,_tmp3E4 + 12},{_tmp3E5,_tmp3E5,_tmp3E5 + 14},{_tmp3E6,_tmp3E6,_tmp3E6 + 13},{_tmp3E7,_tmp3E7,_tmp3E7 + 23},{_tmp3E8,_tmp3E8,_tmp3E8 + 14},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 8},{_tmp3EB,_tmp3EB,_tmp3EB + 11},{_tmp3EC,_tmp3EC,_tmp3EC + 20},{_tmp3ED,_tmp3ED,_tmp3ED + 9},{_tmp3EE,_tmp3EE,_tmp3EE + 16},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 10},{_tmp3F1,_tmp3F1,_tmp3F1 + 16},{_tmp3F2,_tmp3F2,_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 14},{_tmp3F4,_tmp3F4,_tmp3F4 + 11},{_tmp3F5,_tmp3F5,_tmp3F5 + 15},{_tmp3F6,_tmp3F6,_tmp3F6 + 22},{_tmp3F7,_tmp3F7,_tmp3F7 + 16},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 12},{_tmp3FA,_tmp3FA,_tmp3FA + 18},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 12},{_tmp3FD,_tmp3FD,_tmp3FD + 16},{_tmp3FE,_tmp3FE,_tmp3FE + 11},{_tmp3FF,_tmp3FF,_tmp3FF + 10},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 15},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 27},{_tmp404,_tmp404,_tmp404 + 10},{_tmp405,_tmp405,_tmp405 + 13},{_tmp406,_tmp406,_tmp406 + 18},{_tmp407,_tmp407,_tmp407 + 21},{_tmp408,_tmp408,_tmp408 + 19},{_tmp409,_tmp409,_tmp409 + 16},{_tmp40A,_tmp40A,_tmp40A + 20},{_tmp40B,_tmp40B,_tmp40B + 15},{_tmp40C,_tmp40C,_tmp40C + 20},{_tmp40D,_tmp40D,_tmp40D + 15},{_tmp40E,_tmp40E,_tmp40E + 12},{_tmp40F,_tmp40F,_tmp40F + 20},{_tmp410,_tmp410,_tmp410 + 19},{_tmp411,_tmp411,_tmp411 + 20},{_tmp412,_tmp412,_tmp412 + 21},{_tmp413,_tmp413,_tmp413 + 21},{_tmp414,_tmp414,_tmp414 + 12},{_tmp415,_tmp415,_tmp415 + 17},{_tmp416,_tmp416,_tmp416 + 19},{_tmp417,_tmp417,_tmp417 + 14},{_tmp418,_tmp418,_tmp418 + 17},{_tmp419,_tmp419,_tmp419 + 23},{_tmp41A,_tmp41A,_tmp41A + 13},{_tmp41B,_tmp41B,_tmp41B + 14},{_tmp41C,_tmp41C,_tmp41C + 16},{_tmp41D,_tmp41D,_tmp41D + 16},{_tmp41E,_tmp41E,_tmp41E + 8},{_tmp41F,_tmp41F,_tmp41F + 19},{_tmp420,_tmp420,_tmp420 + 20},{_tmp421,_tmp421,_tmp421 + 14},{_tmp422,_tmp422,_tmp422 + 19},{_tmp423,_tmp423,_tmp423 + 20},{_tmp424,_tmp424,_tmp424 + 11},{_tmp425,_tmp425,_tmp425 + 22},{_tmp426,_tmp426,_tmp426 + 20},{_tmp427,_tmp427,_tmp427 + 23},{_tmp428,_tmp428,_tmp428 + 20},{_tmp429,_tmp429,_tmp429 + 22},{_tmp42A,_tmp42A,_tmp42A + 23},{_tmp42B,_tmp42B,_tmp42B + 24},{_tmp42C,_tmp42C,_tmp42C + 24},{_tmp42D,_tmp42D,_tmp42D + 15},{_tmp42E,_tmp42E,_tmp42E + 20},{_tmp42F,_tmp42F,_tmp42F + 22},{_tmp430,_tmp430,_tmp430 + 17},{_tmp431,_tmp431,_tmp431 + 20},{_tmp432,_tmp432,_tmp432 + 26},{_tmp433,_tmp433,_tmp433 + 16},{_tmp434,_tmp434,_tmp434 + 17},{_tmp435,_tmp435,_tmp435 + 15},{_tmp436,_tmp436,_tmp436 + 19},{_tmp437,_tmp437,_tmp437 + 17},{_tmp438,_tmp438,_tmp438 + 19},{_tmp439,_tmp439,_tmp439 + 25},{_tmp43A,_tmp43A,_tmp43A + 26},{_tmp43B,_tmp43B,_tmp43B + 9},{_tmp43C,_tmp43C,_tmp43C + 20},{_tmp43D,_tmp43D,_tmp43D + 17},{_tmp43E,_tmp43E,_tmp43E + 18},{_tmp43F,_tmp43F,_tmp43F + 11},{_tmp440,_tmp440,_tmp440 + 12}};
# 1609
static short Cyc_yyr1[523]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,152,152,152,152,153,153,154,155,156,157,158,158,158,158,158,158,159,159,160,160,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,163,163,164,165,165,166,166,166,166,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,170,170,170,171,171,171,172,172,173,173,173,174,174,174,174,174,175,175,176,176,177,177,178,178,179,180,180,181,181,182,183,183,183,183,183,183,184,184,184,184,184,184,185,186,186,187,187,187,187,187,188,188,188,189,189,190,190,190,190,191,191,191,192,192,193,193,194,194,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,196,197,197,198,199,199,200,200,200,200,200,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,214,215,215,216,216,216,217,218,218,219,219,220,220,220,220,220,221,221,221,221,222,223,223,224,224,225,225,226,226,226,226,226,227,227,228,228,228,229,229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,230,231,231,232,233,233,234,234,235,235,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,238,238,238,238,238,238,238,238,238,238,238,238,238,238,239,239,239,239,239,239,239,239,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,247,248,248,248,248,248,249,249,249,250,250,250,251,251,251,251,252,252,253,253,253,253,253,253,254,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,257,257,257,258,258,259,259,260,260,260,261,261,262,262,263,263,263,264,264,264,264,264,264,264,264,264,264,264,265,265,265,265,265,265,265,266,267,267,268,268,269,269,270,270,271,271,272,272,272,273,273,273,273,273,274,274,274,275,275,275,276,276,276,276,277,277,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,281,281,281,281,282,282,282,282,282,282,282,282,282,282,283,284,284,285,285,285,285,285,286,286,287,287,288,288,289,289,290,290};
# 1665
static short Cyc_yyr2[523]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1721
static short Cyc_yydefact[1038]={0,19,52,53,54,55,57,72,75,76,77,78,79,80,81,82,96,97,98,114,115,48,0,0,58,0,0,149,89,93,0,0,0,0,0,0,39,513,215,515,514,516,0,0,73,0,201,201,200,1,0,17,0,0,18,0,0,42,50,44,70,46,83,84,0,85,0,38,160,0,185,188,86,164,112,56,55,49,0,100,512,0,513,508,509,510,511,112,0,377,0,0,0,0,238,0,379,380,28,30,0,0,0,0,0,0,0,150,0,0,0,0,0,0,0,198,199,2,0,0,0,0,0,32,0,120,121,123,43,51,45,47,116,517,518,112,112,38,40,38,0,21,0,217,0,173,161,186,195,194,0,0,192,193,196,197,206,188,0,71,0,56,104,0,102,0,513,388,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,498,477,0,0,0,0,0,480,478,479,0,405,407,421,429,431,433,435,437,439,442,447,450,453,457,0,459,481,496,495,0,389,387,35,0,0,112,0,0,0,130,126,128,257,259,0,0,9,10,0,0,112,519,520,216,95,0,0,165,74,236,0,233,0,3,0,5,0,287,0,33,0,0,38,21,0,117,118,143,111,0,147,0,0,0,0,0,0,0,0,0,0,0,0,513,290,292,0,300,294,0,298,280,281,282,0,283,284,285,0,41,21,123,20,22,264,0,223,239,0,0,219,217,0,203,0,0,0,208,59,207,189,0,0,105,101,0,0,381,0,0,402,0,0,0,0,253,398,403,0,400,0,467,0,423,457,0,424,425,0,0,0,0,0,0,0,0,0,460,461,38,0,0,0,463,464,462,0,378,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,411,412,413,414,415,416,417,418,419,420,410,0,465,0,487,488,0,0,0,500,0,112,395,396,0,393,0,240,0,0,0,0,260,231,0,131,136,132,134,127,129,217,0,266,258,267,522,521,0,88,92,0,90,94,110,65,64,0,62,166,217,235,162,266,237,174,175,0,87,202,29,0,31,0,0,0,0,122,124,242,241,21,23,107,119,0,0,0,138,139,141,0,112,112,155,0,0,0,0,0,513,0,38,0,329,330,331,0,0,333,0,0,38,0,301,295,123,299,293,291,24,0,172,224,0,0,0,230,218,225,169,0,0,0,219,171,205,204,167,203,0,0,209,60,113,0,106,428,103,99,391,392,256,0,399,252,254,386,0,0,0,0,243,248,0,0,0,0,0,0,0,0,0,0,0,0,0,499,506,0,505,406,430,0,432,434,436,438,440,441,445,446,443,444,448,449,451,452,454,455,456,409,408,486,483,0,485,0,0,0,383,0,390,34,0,384,0,0,261,137,133,135,0,219,0,203,0,268,0,217,0,279,263,0,112,0,0,0,130,0,112,0,217,0,184,163,234,4,6,0,37,0,25,0,142,125,143,0,0,219,148,157,156,0,0,151,0,0,0,308,0,0,0,0,0,0,328,332,0,0,0,0,296,289,38,26,265,217,0,227,0,0,220,0,170,223,211,168,191,190,209,187,0,7,0,0,255,401,404,466,0,0,244,0,249,469,0,0,0,0,473,476,0,0,0,0,0,458,502,0,0,484,482,0,0,394,397,385,262,232,275,0,269,270,203,0,0,219,203,0,91,219,0,0,0,61,63,0,176,0,0,219,0,203,0,0,0,140,145,144,116,146,158,155,155,0,0,0,0,0,0,0,0,0,0,0,0,0,308,334,0,0,297,27,219,0,228,226,0,219,0,210,0,8,382,492,0,491,0,245,250,0,0,0,0,426,427,486,485,504,0,507,422,501,503,0,274,272,278,277,0,271,203,116,0,68,66,67,177,183,180,0,182,178,203,0,36,0,0,154,153,302,308,0,0,0,0,0,0,336,337,339,341,343,345,347,349,352,357,360,363,367,369,375,376,0,38,305,314,0,0,0,0,0,0,0,0,0,0,335,286,213,229,221,212,217,13,0,14,468,0,0,251,470,0,0,475,474,489,0,276,273,0,0,181,179,288,109,0,0,0,308,0,373,0,0,370,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,371,309,0,316,0,0,0,324,0,0,0,0,0,0,307,0,219,12,15,0,494,493,0,0,0,490,108,0,159,303,304,0,0,0,0,308,310,340,0,342,344,346,348,350,351,355,356,353,354,358,359,361,362,364,365,366,0,315,317,318,0,326,325,0,320,0,0,0,222,214,16,0,0,0,0,0,306,372,0,368,311,0,38,319,327,321,322,0,247,246,471,0,69,0,338,308,312,323,0,374,313,472,0,0,0};
# 1828
static short Cyc_yydefgoto[145]={1035,49,689,882,50,300,51,284,52,456,53,458,54,55,139,56,57,527,226,442,443,227,60,240,228,62,162,163,63,159,64,261,262,124,125,126,263,229,424,472,473,474,65,66,648,649,650,67,475,68,447,69,70,156,157,71,115,523,315,686,607,72,608,517,677,509,513,514,419,308,248,93,94,548,464,549,330,331,332,230,301,302,609,430,287,460,288,289,290,291,292,765,293,294,843,844,845,846,847,848,849,850,851,852,853,854,855,856,857,858,333,409,410,334,335,336,295,197,396,198,533,199,200,201,202,203,204,205,206,207,208,209,210,211,212,791,213,564,565,214,215,74,883,241,434};
# 1846
static short Cyc_yypact[1038]={2837,- -32768,- -32768,- -32768,- -32768,- 29,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3490,162,717,- -32768,215,- 25,- -32768,- 10,100,150,161,198,263,359,292,- -32768,- -32768,310,- -32768,- -32768,- -32768,271,473,334,273,342,342,- -32768,- -32768,2700,- -32768,94,408,- -32768,412,752,3490,3490,3490,- -32768,3490,- -32768,- -32768,943,- -32768,215,3409,201,- 14,315,961,- -32768,- -32768,356,362,381,- -32768,215,390,- -32768,3263,287,- -32768,- -32768,- -32768,- -32768,356,6020,- -32768,370,428,3263,425,437,438,- -32768,299,- -32768,- -32768,3636,3636,2700,2700,3636,6020,634,- -32768,167,448,837,455,167,3855,6020,- -32768,- -32768,- -32768,2700,2973,2700,2973,71,- -32768,465,479,- -32768,3327,- -32768,- -32768,- -32768,- -32768,3855,- -32768,- -32768,356,216,1706,- -32768,3409,752,496,3636,3563,4390,- -32768,201,- -32768,- -32768,- -32768,503,513,- -32768,- -32768,- -32768,- -32768,84,961,3636,- -32768,2973,- -32768,540,550,552,215,116,- -32768,3263,113,979,6254,547,6020,6098,554,565,567,579,585,587,600,612,6332,6332,- -32768,- -32768,- -32768,2416,633,6410,6410,6410,- -32768,- -32768,- -32768,313,- -32768,- -32768,- 27,640,623,629,650,655,85,698,337,262,- -32768,1064,6410,566,- 62,- -32768,677,3156,693,- -32768,- -32768,695,6020,356,3156,681,129,3855,3928,3855,699,- -32768,62,62,- -32768,- -32768,135,316,356,- -32768,- -32768,- -32768,- -32768,52,690,- -32768,- -32768,223,389,- -32768,715,- -32768,723,- -32768,728,696,729,- -32768,837,4488,3409,496,739,3855,- -32768,749,742,215,750,746,168,763,3998,771,762,755,782,4586,3998,92,780,783,- -32768,- -32768,789,1849,1849,752,1849,- -32768,- -32768,- -32768,820,- -32768,- -32768,- -32768,398,- -32768,496,832,- -32768,- -32768,822,95,855,- -32768,- 9,842,834,539,848,748,845,3636,6020,- -32768,865,- -32768,- -32768,95,863,215,- -32768,6020,879,- -32768,881,882,- -32768,167,6020,3263,382,- -32768,- -32768,- -32768,888,868,2416,- -32768,3855,- -32768,- -32768,4096,- -32768,900,6020,6020,6020,6020,6020,6020,3855,665,2416,- -32768,- -32768,1992,883,415,6020,- -32768,- -32768,- -32768,6020,- -32768,6410,6020,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6020,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6020,- -32768,167,- -32768,- -32768,4681,167,6020,- -32768,887,356,- -32768,- -32768,889,891,3263,- -32768,562,979,892,3636,- -32768,896,893,- -32768,3928,3928,3928,- -32768,- -32768,3120,4776,176,- -32768,320,- -32768,- -32768,- 9,- -32768,- -32768,3636,- -32768,- -32768,901,- -32768,897,902,905,- -32768,3047,- -32768,367,260,- -32768,- -32768,- -32768,3855,- -32768,- -32768,- -32768,2700,- -32768,2700,922,912,907,- -32768,- -32768,- -32768,- -32768,496,- -32768,- -32768,- -32768,851,6020,920,919,- -32768,- 8,604,356,356,871,6020,6020,916,924,6020,918,1016,2278,933,- -32768,- -32768,- -32768,593,1011,- -32768,4874,6020,2135,2554,- -32768,- -32768,3327,- -32768,- -32768,- -32768,- -32768,3636,- -32768,- -32768,3855,929,193,- -32768,- -32768,923,- -32768,- 9,934,3782,834,- -32768,- -32768,- -32768,- -32768,748,64,937,886,- -32768,- -32768,1201,- -32768,- -32768,- -32768,- -32768,3855,- -32768,- -32768,932,- -32768,- -32768,- -32768,- -32768,1263,939,945,950,- -32768,- -32768,760,4488,941,947,953,957,426,954,956,721,960,951,6176,- -32768,- -32768,965,962,- -32768,640,220,623,629,650,655,85,85,698,698,698,698,337,337,262,262,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,966,- -32768,- 45,3636,4292,- -32768,6572,- -32768,- -32768,967,- -32768,121,982,- -32768,- -32768,- -32768,- -32768,970,834,971,748,968,320,3636,3709,4969,- -32768,- -32768,62,356,802,973,52,3193,975,356,3636,3563,5067,- -32768,367,- -32768,- -32768,- -32768,977,- -32768,998,- -32768,508,- -32768,- -32768,749,6020,6020,834,- -32768,- -32768,- -32768,985,215,648,433,436,6020,869,489,3998,983,5165,5260,666,- -32768,- -32768,994,996,989,504,1849,- -32768,3409,- -32768,822,999,3636,- -32768,1000,- 9,- -32768,992,- -32768,182,- -32768,- -32768,- -32768,- -32768,886,- -32768,1002,- -32768,2700,1018,- -32768,- -32768,- -32768,1005,294,1003,- -32768,1379,- -32768,- -32768,6020,1101,6020,6098,- -32768,- -32768,167,167,1005,1001,4292,- -32768,- -32768,6020,6020,- -32768,- -32768,95,818,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1004,- -32768,- -32768,748,95,1006,834,748,997,- -32768,834,519,1009,1010,- -32768,- -32768,1012,- -32768,95,1013,834,1015,748,1017,6020,1024,- -32768,- -32768,- -32768,3855,- -32768,1019,149,871,3998,1026,1021,6536,1022,1030,3998,6020,5355,687,5450,694,5545,869,- -32768,1034,1035,- -32768,- -32768,834,139,- -32768,- -32768,1042,834,3855,- -32768,230,- -32768,- -32768,- -32768,376,- -32768,6020,- -32768,- -32768,4488,1041,1043,1048,- -32768,900,1044,1045,- -32768,825,- -32768,- -32768,- -32768,- -32768,4292,- -32768,- -32768,- -32768,- -32768,1046,- -32768,748,3855,1067,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1050,- -32768,- -32768,748,530,- -32768,1054,3855,- -32768,- -32768,1150,869,1059,6488,1055,2554,6410,1052,- -32768,72,- -32768,1092,1047,810,864,106,895,456,462,- -32768,- -32768,- -32768,- -32768,1093,6410,1992,- -32768,- -32768,532,3998,573,5640,3998,580,5735,5830,719,1065,- -32768,- -32768,- -32768,- -32768,1066,- -32768,999,- -32768,1069,737,- -32768,521,238,- -32768,- -32768,3855,1166,- -32768,- -32768,- -32768,4194,- -32768,- -32768,1071,1073,- -32768,- -32768,- -32768,- -32768,582,3998,1075,869,2416,- -32768,3855,1070,- -32768,1563,6410,6020,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6410,6020,- -32768,- -32768,1079,- -32768,3998,3998,605,- -32768,3998,3998,606,3998,638,5925,- -32768,- 9,834,- -32768,- -32768,634,- -32768,- -32768,2554,1076,1074,- -32768,- -32768,1116,- -32768,- -32768,- -32768,1103,1099,1105,6410,869,- -32768,640,240,623,629,629,655,85,85,698,698,698,698,337,337,262,262,- -32768,- -32768,- -32768,242,- -32768,- -32768,- -32768,3998,- -32768,- -32768,3998,- -32768,3998,3998,660,- -32768,- -32768,- -32768,1107,867,1102,3855,1104,- -32768,1005,294,- -32768,- -32768,6020,1563,- -32768,- -32768,- -32768,- -32768,3998,- -32768,- -32768,- -32768,1106,- -32768,486,- -32768,869,- -32768,- -32768,1108,- -32768,- -32768,- -32768,1230,1232,- -32768};
# 1953
static short Cyc_yypgoto[145]={- -32768,133,- -32768,282,- -32768,- 223,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 22,- -32768,- 91,39,- -32768,- -32768,0,614,- -32768,32,- 176,1125,5,- -32768,- -32768,- 124,- -32768,98,1205,- 661,- -32768,- -32768,- -32768,984,980,553,515,- -32768,- -32768,610,- -32768,- -32768,244,- -32768,- -32768,146,- 170,1176,- 392,15,- -32768,1090,- -32768,- -32768,1206,- 381,- -32768,569,- 141,- 138,- 133,- 312,304,576,581,- 403,- 462,- 96,- 290,- 109,- -32768,- 239,- 173,- 550,- 327,- -32768,930,- 153,- 73,- 72,- 175,- 258,136,- -32768,- -32768,- -32768,- 44,- 276,- -32768,- 599,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,87,1039,- -32768,722,856,- -32768,28,649,- -32768,- 166,- 289,- 158,- 345,- 338,- 322,904,- 348,- 368,- 245,- 353,- 341,- 178,551,511,926,264,- -32768,- 354,- -32768,55,500,- 58,74,- 253,96};
# 1975
static short Cyc_yytable[6713]={58,343,306,573,574,61,134,340,499,500,307,502,360,361,362,550,344,247,316,463,567,58,572,141,579,580,61,231,232,569,617,235,59,397,309,357,259,467,581,582,538,323,404,110,719,138,570,590,134,675,58,422,510,59,429,61,630,58,58,58,77,58,61,61,61,365,61,58,145,440,642,449,61,405,505,537,446,363,96,75,560,99,59,260,630,147,318,418,717,59,59,59,37,59,832,297,128,129,130,59,131,40,58,58,38,138,140,61,61,366,95,511,43,678,100,285,196,296,58,58,58,58,643,61,61,61,61,58,575,576,577,578,61,236,59,59,96,58,135,58,724,665,61,682,61,588,145,96,431,591,59,59,59,59,646,647,532,897,441,59,58,805,631,532,913,61,140,512,167,59,612,59,373,374,873,519,286,254,140,218,237,431,639,117,544,170,545,432,451,111,612,683,59,921,922,255,531,38,557,433,559,433,127,583,584,585,451,681,914,375,376,494,314,118,119,466,358,506,495,672,432,431,168,96,101,96,325,421,631,728,923,924,423,265,267,233,234,296,905,525,169,326,38,637,428,422,422,422,723,413,417,250,251,252,253,324,243,436,433,58,432,448,285,285,61,285,877,550,37,102,- 152,96,433,238,37,624,433,40,96,476,103,239,78,40,58,58,298,58,508,61,61,59,61,319,481,512,726,482,140,762,624,305,426,408,492,532,38,964,427,- 217,408,699,- 217,969,674,528,59,59,42,59,414,37,104,286,286,142,286,143,435,37,40,437,755,144,285,439,37,39,40,41,158,107,363,601,39,40,41,266,812,445,715,881,816,753,754,427,58,46,47,48,363,61,363,618,363,358,37,828,955,1013,1014,796,1015,418,39,40,41,555,512,19,20,358,690,712,550,96,445,625,59,105,451,777,427,168,568,286,381,109,507,113,238,790,247,298,382,383,- 239,486,239,- 239,669,169,451,493,529,108,1029,539,451,222,815,421,421,421,818,223,423,423,423,1033,616,592,501,671,826,363,896,42,363,112,364,428,792,438,613,623,614,629,900,46,47,48,615,742,802,803,58,670,58,795,448,61,114,61,418,659,96,876,742,96,638,122,879,731,379,667,532,532,380,138,158,732,160,796,42,476,745,58,59,161,59,626,61,627,746,880,216,58,597,628,73,58,61,884,594,540,61,885,651,652,165,452,655,328,329,660,453,504,747,59,363,718,79,97,666,98,140,120,121,59,58,800,685,59,217,61,140,363,512,779,140,730,562,73,219,678,801,784,363,807,73,976,977,704,744,363,73,887,363,220,760,221,59,761,133,625,136,796,970,73,975,795,982,983,242,644,645,972,164,37,451,97,244,451,256,934,984,985,40,632,42,633,97,927,973,974,929,928,96,43,780,257,73,73,930,931,133,46,47,48,73,363,616,1032,37,299,766,885,73,73,73,73,39,40,41,778,363,238,953,312,629,776,954,668,470,239,968,1002,476,313,73,819,46,47,48,285,820,296,398,399,400,96,363,476,363,795,476,901,305,935,73,518,320,688,911,164,246,58,97,58,97,321,61,339,61,322,978,979,980,981,345,598,933,721,363,264,769,771,910,58,685,346,401,347,61,363,246,402,403,59,937,59,363,80,452,348,286,941,140,961,37,349,662,350,735,363,97,736,39,40,41,59,338,97,341,341,351,363,363,134,470,367,993,996,354,355,956,903,352,1028,37,341,341,341,371,372,450,949,39,40,41,986,987,988,965,80,966,73,792,359,363,368,341,249,450,998,478,758,369,483,759,37,83,185,186,84,85,86,830,420,40,425,363,377,378,772,73,1020,363,1012,370,353,668,311,189,864,866,406,869,411,872,412,1004,707,399,400,867,416,81,363,512,521,522,870,808,264,363,444,96,459,164,886,788,82,83,426,813,84,85,86,97,427,40,46,47,48,454,87,946,285,824,363,461,88,455,89,90,401,1027,457,859,91,708,403,1024,37,951,92,37,952,468,58,477,39,40,41,61,40,488,196,479,123,480,341,489,470,646,647,43,471,341,697,698,46,47,48,46,47,48,484,285,763,764,59,134,939,836,487,943,945,286,490,863,16,17,18,496,465,737,738,739,97,58,497,97,498,341,61,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,358,604,605,606,809,810,503,971,37,59,450,893,894,450,917,918,286,40,919,920,258,73,37,73,433,989,508,526,43,516,39,40,41,42,515,450,46,47,48,1000,520,450,470,524,246,925,926,42,1005,530,46,47,48,1022,363,543,465,365,285,551,552,553,554,246,556,534,936,834,835,940,246,535,536,563,542,561,593,566,596,603,58,595,80,602,600,61,619,341,620,148,149,150,622,634,621,73,635,636,586,151,152,153,154,155,640,962,641,653,97,654,587,657,59,656,37,563,246,661,663,286,673,696,39,40,41,676,679,246,132,684,691,694,695,700,701,81,246,710,991,992,702,611,994,995,703,997,705,327,706,714,166,83,709,722,84,85,86,713,716,40,38,97,725,727,87,740,729,743,750,751,88,756,89,90,767,328,329,341,91,773,774,775,783,305,92,384,781,787,450,789,711,798,793,804,1016,817,811,1017,814,1018,1019,821,822,450,823,825,450,827,831,563,833,837,563,838,757,829,862,861,874,875,878,1030,385,386,387,388,389,390,391,392,393,394,246,888,889,890,891,892,895,898,902,420,899,904,906,246,909,912,915,932,916,947,395,948,73,950,957,959,341,341,960,963,967,990,465,1007,- 11,1,1006,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,1008,1009,21,1010,1011,1021,1023,1036,1025,1037,1031,1003,1034,741,245,22,106,23,462,469,465,687,146,24,317,25,26,27,752,1001,116,786,341,785,782,28,29,- 38,541,415,97,734,693,341,30,31,32,599,0,33,571,860,1026,0,749,558,0,34,35,36,0,0,0,0,133,0,0,0,0,0,0,0,0,0,0,0,0,0,80,563,0,0,0,0,37,264,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,246,0,0,0,0,0,0,0,0,465,0,0,797,0,799,0,81,0,0,0,0,0,465,0,0,806,0,0,692,0,0,166,83,264,0,84,85,86,0,0,40,0,0,0,0,87,0,0,246,0,0,88,908,89,90,341,328,329,0,91,0,0,0,0,0,92,171,172,0,0,0,0,0,341,0,0,0,0,0,80,0,173,0,0,0,174,0,0,0,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,465,0,0,180,181,0,182,133,0,0,0,0,0,0,465,0,0,183,184,341,0,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,794,0,0,0,0,188,0,0,189,0,328,329,0,190,0,0,191,0,341,192,193,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,465,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,341,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,763,764,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,- 308,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,282,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,- 38,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,0,0,0,0,0,0,22,80,23,173,0,0,0,174,24,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,658,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,356,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,22,0,38,39,40,41,42,0,0,0,- 11,0,27,0,0,43,0,44,45,0,28,29,0,46,47,48,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,37,0,0,0,22,0,0,38,39,40,41,42,0,305,0,0,27,0,0,0,445,0,44,45,28,29,427,0,46,47,48,0,0,0,0,32,0,0,33,0,0,80,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,426,81,44,45,28,29,427,0,46,47,48,0,0,407,0,32,166,83,33,0,84,85,86,0,0,40,34,35,0,0,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,0,0,37,0,80,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,470,0,44,45,0,0,0,0,46,47,48,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,81,0,0,0,0,0,0,0,0,0,22,0,23,0,0,166,83,0,24,84,85,86,27,0,40,0,0,0,0,87,28,29,- 38,0,0,88,0,89,90,0,0,32,0,91,33,0,0,0,0,92,0,0,34,35,36,0,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,38,39,0,41,42,0,22,137,23,0,258,0,0,0,24,44,45,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,137,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,304,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,224,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,225,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,680,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,268,0,269,270,271,272,273,274,275,276,0,80,0,173,277,0,38,174,0,278,42,0,0,0,0,175,176,177,178,0,0,44,45,179,0,279,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,83,185,186,84,85,86,0,0,40,0,0,187,281,137,0,0,171,172,0,188,0,0,189,546,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,547,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,958,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,310,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,491,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,589,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,610,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,664,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,733,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,748,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,768,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,770,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,865,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,868,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,871,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,938,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,942,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,944,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,999,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,342,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,711,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,337,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,353,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,839,840,0,0,0,0,0,0,0,0,183,184,0,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,80,0,0,0,907,0,0,189,0,0,0,0,190,0,0,191,0,81,192,193,0,0,194,195,0,0,0,0,0,0,0,0,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,0,0,0,0,841,81,89,90,0,0,0,0,91,0,0,842,0,720,92,193,166,83,194,195,84,85,86,0,0,40,0,0,0,0,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};
# 2650
static short Cyc_yycheck[6713]={0,174,143,371,372,0,64,173,284,285,143,287,190,191,192,342,174,113,156,258,365,21,370,67,377,378,21,100,101,367,433,104,0,211,143,188,127,260,379,380,329,165,104,43,594,67,368,401,106,511,50,227,61,21,229,50,448,57,58,59,21,61,57,58,59,92,61,67,68,17,78,246,67,135,297,328,246,122,23,108,356,106,50,127,476,70,158,225,133,57,58,59,106,61,755,139,57,58,59,67,61,115,102,103,113,127,67,102,103,136,23,120,126,516,124,137,88,139,118,119,120,121,130,118,119,120,121,127,373,374,375,376,127,105,102,103,81,137,64,139,602,495,137,524,139,398,146,92,86,402,118,119,120,121,5,6,322,818,106,127,160,711,452,329,92,160,127,305,81,137,428,139,87,88,773,308,137,106,139,92,106,86,471,50,337,87,339,125,246,43,448,127,160,87,88,124,320,113,351,137,353,137,56,381,382,383,264,519,136,124,125,119,128,119,120,259,188,122,126,509,125,86,106,168,124,170,113,227,518,610,124,125,227,135,136,102,103,259,837,312,124,128,113,466,229,421,422,423,127,221,121,118,119,120,121,168,110,122,137,259,125,246,284,285,259,287,127,594,106,119,121,216,137,106,106,445,137,115,223,264,119,114,120,115,284,285,140,287,106,284,285,259,287,160,126,433,608,129,259,653,470,119,126,216,276,471,113,906,132,127,223,550,130,912,121,315,284,285,117,287,222,106,124,284,285,124,287,126,232,106,115,235,644,132,356,237,106,114,115,116,124,49,122,416,114,115,116,131,729,126,130,121,733,642,643,132,356,134,135,136,122,356,122,436,122,337,106,748,130,968,130,698,130,511,114,115,116,349,516,20,21,353,535,561,711,330,126,445,356,126,448,667,132,106,366,356,134,126,302,126,106,107,498,257,142,143,119,271,114,122,501,124,470,277,318,105,1015,330,476,120,732,421,422,423,736,126,421,422,423,1028,430,403,286,506,746,122,817,117,122,105,127,426,695,127,124,445,126,447,829,134,135,136,132,623,707,708,456,501,458,698,445,456,120,458,602,487,411,779,638,414,470,59,784,614,137,497,642,643,141,501,124,614,120,810,117,470,627,487,456,108,458,124,487,126,627,785,126,497,411,132,0,501,497,127,406,123,501,131,480,481,120,122,484,131,132,487,127,119,627,487,122,593,22,23,496,25,487,119,120,497,530,704,527,501,106,530,497,122,676,672,501,613,127,43,119,948,704,680,122,715,50,919,920,127,626,122,56,796,122,122,127,123,530,127,64,623,66,894,913,69,918,810,925,926,126,477,478,915,78,106,638,81,127,641,119,861,927,928,115,456,117,458,92,137,916,917,134,141,543,126,673,122,102,103,142,143,106,134,135,136,110,122,612,127,106,119,127,131,118,119,120,121,114,115,116,669,122,106,107,126,630,127,885,497,126,114,912,949,623,126,140,122,134,135,136,667,127,669,82,83,84,596,122,638,122,894,641,127,119,127,160,122,122,530,842,165,113,667,168,669,170,121,667,126,669,123,921,922,923,924,126,119,860,596,122,132,658,659,841,689,685,126,126,126,689,122,143,131,132,667,127,669,122,38,122,126,667,127,669,127,106,126,119,126,618,122,216,619,114,115,116,689,171,223,173,174,126,122,122,787,126,91,127,127,183,184,889,833,126,1015,106,190,191,192,89,90,246,880,114,115,116,929,930,931,907,38,909,257,1011,126,122,138,211,114,264,127,266,119,139,269,122,106,107,108,109,110,111,112,750,226,115,228,122,85,86,119,286,127,122,967,140,126,656,144,129,767,768,120,770,106,772,106,955,82,83,84,119,126,91,122,948,63,64,119,718,262,122,127,763,123,320,793,689,106,107,126,730,110,111,112,330,132,115,134,135,136,121,120,119,861,744,122,113,126,121,128,129,126,1014,121,763,134,131,132,1007,106,119,140,106,122,121,861,120,114,115,116,861,115,106,841,120,119,126,322,119,126,5,6,126,130,329,121,122,134,135,136,134,135,136,126,912,22,23,861,952,867,760,126,870,871,861,119,766,17,18,19,126,258,106,107,108,411,912,130,414,126,365,912,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,907,421,422,423,121,122,121,914,106,912,445,121,122,448,139,140,912,115,89,90,123,456,106,458,137,932,106,313,126,130,114,115,116,117,127,470,134,135,136,946,127,476,126,133,426,85,86,117,955,121,134,135,136,121,122,122,342,92,1015,345,346,347,348,445,350,121,865,758,759,868,452,125,125,359,121,127,124,363,122,121,1015,127,38,122,127,1015,120,471,126,63,64,65,122,106,127,530,119,125,384,73,74,75,76,77,119,904,122,126,543,120,396,30,1015,130,106,401,498,119,42,1015,126,106,114,115,116,137,127,509,120,127,133,127,122,127,122,91,518,121,937,938,122,427,941,942,122,944,127,103,127,122,106,107,127,121,110,111,112,127,127,115,113,596,127,127,120,127,133,127,126,106,126,121,128,129,126,131,132,561,134,120,119,127,125,119,140,56,121,120,623,106,120,25,124,127,993,133,127,996,127,998,999,127,127,638,127,127,641,127,119,495,126,120,498,127,649,133,121,130,119,119,113,1020,93,94,95,96,97,98,99,100,101,102,614,127,126,122,127,127,127,107,121,623,127,28,120,627,126,130,91,91,138,121,123,122,689,121,25,121,642,643,122,121,127,119,550,126,0,1,127,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,121,24,127,122,121,127,0,127,0,127,952,127,622,112,37,34,39,257,262,594,43,69,45,157,47,48,49,641,948,47,685,704,680,676,57,58,59,331,223,763,615,543,715,66,67,68,414,- 1,71,369,763,1011,- 1,628,352,- 1,79,80,81,- 1,- 1,- 1,- 1,787,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,653,- 1,- 1,- 1,- 1,106,755,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,785,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,698,- 1,- 1,701,- 1,703,- 1,91,- 1,- 1,- 1,- 1,- 1,711,- 1,- 1,714,- 1,- 1,103,- 1,- 1,106,107,818,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,833,- 1,- 1,126,839,128,129,842,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,25,26,- 1,- 1,- 1,- 1,- 1,860,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,796,- 1,- 1,69,70,- 1,72,952,- 1,- 1,- 1,- 1,- 1,- 1,810,- 1,- 1,83,84,913,- 1,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,930,931,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,- 1,137,- 1,967,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,894,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1014,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,71,- 1,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,- 1,134,71,- 1,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,113,44,- 1,46,117,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,128,129,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,32,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,38,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,91,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,91,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4F0[8]="stdcall";static char _tmp4F1[6]="cdecl";static char _tmp4F2[9]="fastcall";static char _tmp4F3[9]="noreturn";static char _tmp4F4[6]="const";static char _tmp4F5[8]="aligned";static char _tmp4F6[7]="packed";static char _tmp4F7[7]="shared";static char _tmp4F8[7]="unused";static char _tmp4F9[5]="weak";static char _tmp4FA[10]="dllimport";static char _tmp4FB[10]="dllexport";static char _tmp4FC[23]="no_instrument_function";static char _tmp4FD[12]="constructor";static char _tmp4FE[11]="destructor";static char _tmp4FF[22]="no_check_memory_usage";static char _tmp500[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9E1(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9E0,unsigned int*_tmp9DF,union Cyc_YYSTYPE**_tmp9DE){for(*_tmp9E0=0;*_tmp9E0 < *_tmp9DF;(*_tmp9E0)++){(*_tmp9DE)[*_tmp9E0]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9E6(unsigned int*_tmp9E5,unsigned int*_tmp9E4,struct Cyc_Yyltype**_tmp9E3){for(*_tmp9E5=0;*_tmp9E5 < *_tmp9E4;(*_tmp9E5)++){(*_tmp9E3)[*_tmp9E5]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9ED(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9EC,unsigned int*_tmp9EB,short**_tmp9E9){for(*_tmp9EC=0;*_tmp9EC < *_tmp9EB;(*_tmp9EC)++){(*_tmp9E9)[*_tmp9EC]=(short)(
# 216
*_tmp9EC <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9EC)): 0);}}static void _tmp9F3(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9F2,unsigned int*_tmp9F1,union Cyc_YYSTYPE**_tmp9EF){for(*_tmp9F2=0;*_tmp9F2 < *_tmp9F1;(*_tmp9F2)++){(*_tmp9EF)[*_tmp9F2]=(union Cyc_YYSTYPE)(
# 219
*_tmp9F2 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9F2)):*yylval);}}static void _tmp9F9(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9F8,unsigned int*_tmp9F7,struct Cyc_Yyltype**_tmp9F5){for(*_tmp9F8=0;*_tmp9F8 < *_tmp9F7;(*_tmp9F8)++){(*_tmp9F5)[*_tmp9F8]=(struct Cyc_Yyltype)(
# 222
*_tmp9F8 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9F8)):
 Cyc_yynewloc());}}static void _tmpEEA(unsigned int*_tmpEE9,unsigned int*_tmpEE8,char**_tmpEE6){for(*_tmpEE9=0;*_tmpEE9 < *_tmpEE8;(*_tmpEE9)++){(*_tmpEE6)[*_tmpEE9]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp442=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp442;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9DA;union Cyc_YYSTYPE yylval=((_tmp9DA.YYINITIALSVAL).val=0,(((_tmp9DA.YYINITIALSVAL).tag=64,_tmp9DA)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9DC;unsigned int _tmp9DB;struct _dyneither_ptr yyss=(_tmp9DB=200,((_tmp9DC=_region_calloc(yyregion,sizeof(short),_tmp9DB),_tag_dyneither(_tmp9DC,sizeof(short),_tmp9DB))));
# 158
int yyvsp_offset;
unsigned int _tmp9E0;unsigned int _tmp9DF;union Cyc_YYSTYPE*_tmp9DE;unsigned int _tmp9DD;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9DD=(unsigned int)200,((_tmp9DE=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9DD)),((((_tmp9DF=_tmp9DD,_tmp9E1(& yylval,& _tmp9E0,& _tmp9DF,& _tmp9DE))),_tmp9DE)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9E5;unsigned int _tmp9E4;struct Cyc_Yyltype*_tmp9E3;unsigned int _tmp9E2;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9E2=(unsigned int)200,((_tmp9E3=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9E2)),((((_tmp9E4=_tmp9E2,_tmp9E6(& _tmp9E5,& _tmp9E4,& _tmp9E3))),_tmp9E3)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9E7;Cyc_yyerror(((_tmp9E7="parser stack overflow",_tag_dyneither(_tmp9E7,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9EC;unsigned int _tmp9EB;struct _dyneither_ptr _tmp9EA;short*_tmp9E9;unsigned int _tmp9E8;struct _dyneither_ptr yyss1=(_tmp9E8=(unsigned int)yystacksize,((_tmp9E9=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9E8)),((_tmp9EA=_tag_dyneither(_tmp9E9,sizeof(short),_tmp9E8),((((_tmp9EB=_tmp9E8,_tmp9ED(& yyssp_offset,& yyss,& _tmp9EC,& _tmp9EB,& _tmp9E9))),_tmp9EA)))))));
# 217
unsigned int _tmp9F2;unsigned int _tmp9F1;struct _dyneither_ptr _tmp9F0;union Cyc_YYSTYPE*_tmp9EF;unsigned int _tmp9EE;struct _dyneither_ptr yyvs1=
(_tmp9EE=(unsigned int)yystacksize,((_tmp9EF=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9EE)),((_tmp9F0=_tag_dyneither(_tmp9EF,sizeof(union Cyc_YYSTYPE),_tmp9EE),((((_tmp9F1=_tmp9EE,_tmp9F3(& yyvs,& yyssp_offset,& yylval,& _tmp9F2,& _tmp9F1,& _tmp9EF))),_tmp9F0)))))));
# 221
unsigned int _tmp9F8;unsigned int _tmp9F7;struct _dyneither_ptr _tmp9F6;struct Cyc_Yyltype*_tmp9F5;unsigned int _tmp9F4;struct _dyneither_ptr yyls1=(_tmp9F4=(unsigned int)yystacksize,((_tmp9F5=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9F4)),((_tmp9F6=_tag_dyneither(_tmp9F5,sizeof(struct Cyc_Yyltype),_tmp9F4),((((_tmp9F7=_tmp9F4,_tmp9F9(& yyssp_offset,& yyls,& _tmp9F8,& _tmp9F7,& _tmp9F5))),_tmp9F6)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
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
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 291;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6712) || Cyc_yycheck[_check_known_subscript_notnull(6713,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6713,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1037){
int _tmp450=0;_npop_handler(0);return _tmp450;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1038,yystate)];
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
# 1169 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY19(yyyyvsp[0]);
# 1172
break;}case 2: _LL210: {
# 1174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1175 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp451;_push_handler(& _tmp451);{int _tmp453=0;if(setjmp(_tmp451.handler))_tmp453=1;if(!_tmp453){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp452=(void*)_exn_thrown;void*_tmp455=_tmp452;void*_tmp457;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp456=(struct Cyc_Core_Failure_exn_struct*)_tmp455;if(_tmp456->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1180
 x=0;goto _LL212;_LL215: _tmp457=_tmp455;_LL216:(void)_throw(_tmp457);_LL212:;}};}
# 1182
{struct _handler_cons _tmp458;_push_handler(& _tmp458);{int _tmp45A=0;if(setjmp(_tmp458.handler))_tmp45A=1;if(!_tmp45A){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp459=(void*)_exn_thrown;void*_tmp45C=_tmp459;void*_tmp45E;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp45D=(struct Cyc_Core_Failure_exn_struct*)_tmp45C;if(_tmp45D->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1186
 y=0;goto _LL217;_LL21A: _tmp45E=_tmp45C;_LL21B:(void)_throw(_tmp45E);_LL217:;}};}
# 1188
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1190
break;}case 3: _LL211: {
# 1192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1193 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA03;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA02;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA01;struct Cyc_List_List*_tmpA00;yyval=Cyc_YY19(((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->hd=((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03->r=(void*)((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=10,((_tmpA02.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA02.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA02)))))),_tmpA01)))),((_tmpA03->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA03)))))),((_tmpA00->tl=0,_tmpA00)))))));}
Cyc_Lex_leave_using();
# 1196
break;}case 4: _LL21C: {
# 1198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1197 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA0D;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA0C;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA0B;struct Cyc_List_List*_tmpA0A;yyval=Cyc_YY19(((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->hd=((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D->r=(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=10,((_tmpA0C.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0C.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA0C)))))),_tmpA0B)))),((_tmpA0D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0D)))))),((_tmpA0A->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmpA0A)))))));}
break;}case 5: _LL21D: {
# 1200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1200 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA1B;struct _dyneither_ptr*_tmpA1A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA19;struct Cyc_List_List*_tmpA18;yyval=Cyc_YY19(((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->r=(void*)((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1B.tag=9,((_tmpA1B.f1=((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA1A)))),((_tmpA1B.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1B)))))),_tmpA19)))),((_tmpA1C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1C)))))),((_tmpA18->tl=0,_tmpA18)))))));}
Cyc_Lex_leave_namespace();
# 1203
break;}case 6: _LL21E: {
# 1205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1205 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp46C;_push_handler(& _tmp46C);{int _tmp46E=0;if(setjmp(_tmp46C.handler))_tmp46E=1;if(!_tmp46E){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp46D=(void*)_exn_thrown;void*_tmp470=_tmp46D;void*_tmp472;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp471=(struct Cyc_Core_Failure_exn_struct*)_tmp470;if(_tmp471->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmpA1D;nspace=((_tmpA1D="",_tag_dyneither(_tmpA1D,sizeof(char),1)));}goto _LL220;_LL223: _tmp472=_tmp470;_LL224:(void)_throw(_tmp472);_LL220:;}};}
# 1210
{struct _handler_cons _tmp474;_push_handler(& _tmp474);{int _tmp476=0;if(setjmp(_tmp474.handler))_tmp476=1;if(!_tmp476){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp475=(void*)_exn_thrown;void*_tmp478=_tmp475;void*_tmp47A;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp479=(struct Cyc_Core_Failure_exn_struct*)_tmp478;if(_tmp479->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp47A=_tmp478;_LL229:(void)_throw(_tmp47A);_LL225:;}};}
# 1213
{struct _handler_cons _tmp47B;_push_handler(& _tmp47B);{int _tmp47D=0;if(setjmp(_tmp47B.handler))_tmp47D=1;if(!_tmp47D){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp47C=(void*)_exn_thrown;void*_tmp47F=_tmp47C;void*_tmp481;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp480=(struct Cyc_Core_Failure_exn_struct*)_tmp47F;if(_tmp480->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp481=_tmp47F;_LL22E:(void)_throw(_tmp481);_LL22A:;}};}
# 1216
{struct Cyc_Absyn_Decl*_tmpA2C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA2B;struct _dyneither_ptr*_tmpA2A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA29;struct Cyc_List_List*_tmpA28;yyval=Cyc_YY19(((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->hd=((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->r=(void*)((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA2B.tag=9,((_tmpA2B.f1=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A[0]=nspace,_tmpA2A)))),((_tmpA2B.f2=x,_tmpA2B)))))),_tmpA29)))),((_tmpA2C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2C)))))),((_tmpA28->tl=y,_tmpA28)))))));}
# 1218
break;}case 7: _LL21F: {
# 1220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1219 "parse.y"
struct _dyneither_ptr _tmp487=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmpA2D;if(Cyc_strcmp((struct _dyneither_ptr)_tmp487,((_tmpA2D="C",_tag_dyneither(_tmpA2D,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA37;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA36;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA35;struct Cyc_List_List*_tmpA34;yyval=Cyc_YY19(((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->r=(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA36.tag=11,((_tmpA36.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA36)))),_tmpA35)))),((_tmpA37->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA37)))))),((_tmpA34->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA34)))))));}else{
# 1223
{const char*_tmpA38;if(Cyc_strcmp((struct _dyneither_ptr)_tmp487,((_tmpA38="C include",_tag_dyneither(_tmpA38,sizeof(char),10))))!= 0){
const char*_tmpA39;Cyc_Parse_err(((_tmpA39="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA39,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1226
struct Cyc_Absyn_Decl*_tmpA43;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA42;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA41;struct Cyc_List_List*_tmpA40;yyval=Cyc_YY19(((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->hd=((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->r=(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=12,((_tmpA42.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA42.f2=0,_tmpA42)))))),_tmpA41)))),((_tmpA43->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA43)))))),((_tmpA40->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA40)))))));};}}
# 1229
break;}case 8: _LL22F: {
# 1231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1230 "parse.y"
{const char*_tmpA44;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA44="C include",_tag_dyneither(_tmpA44,sizeof(char),10))))!= 0){
const char*_tmpA45;Cyc_Parse_err(((_tmpA45="expecting \"C include\"",_tag_dyneither(_tmpA45,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA4F;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA4E;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA4D;struct Cyc_List_List*_tmpA4C;yyval=Cyc_YY19(((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->hd=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->r=(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=12,((_tmpA4E.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA4E.f2=exs,_tmpA4E)))))),_tmpA4D)))),((_tmpA4F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4F)))))),((_tmpA4C->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA4C)))))));}
# 1235
break;};}case 9: _LL230: {
# 1237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1236 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA52;struct Cyc_List_List*_tmpA51;yyval=Cyc_YY19(((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA52->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA52)))))),((_tmpA51->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA51)))))));}
break;}case 10: _LL231: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1238 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA55;struct Cyc_List_List*_tmpA54;yyval=Cyc_YY19(((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->hd=((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA55->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA55)))))),((_tmpA54->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA54)))))));}
break;}case 11: _LL232:
# 1241
 yyval=Cyc_YY19(0);
break;case 12: _LL233: {
# 1244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1243 "parse.y"
yyval=yyyyvsp[2];
break;}case 13: _LL234: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1244 "parse.y"
yyval=Cyc_YY55(0);
break;}case 14: _LL235: {
# 1247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1248 "parse.y"
{struct _tuple27*_tmpA58;struct Cyc_List_List*_tmpA57;yyval=Cyc_YY55(((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->hd=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA58->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA58->f3=0,_tmpA58)))))))),((_tmpA57->tl=0,_tmpA57)))))));}
break;}case 15: _LL236: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1249 "parse.y"
{struct _tuple27*_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY55(((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA5B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA5B->f3=0,_tmpA5B)))))))),((_tmpA5A->tl=0,_tmpA5A)))))));}
break;}case 16: _LL237: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1251 "parse.y"
{struct _tuple27*_tmpA5E;struct Cyc_List_List*_tmpA5D;yyval=Cyc_YY55(((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->hd=((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA5E->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA5E->f3=0,_tmpA5E)))))))),((_tmpA5D->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA5D)))))));}
break;}case 17: _LL238: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1255 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA64;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA63;struct Cyc_List_List*_tmpA62;yyval=Cyc_YY19(((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62->hd=Cyc_Absyn_new_decl((void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA63.tag=1,((_tmpA63.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA63)))),_tmpA64)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA62->tl=0,_tmpA62)))))));}
break;}case 18: _LL239: {
# 1258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1256 "parse.y"
yyval=yyyyvsp[0];
break;}case 19: _LL23A: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1257 "parse.y"
yyval=Cyc_YY19(0);
break;}case 22: _LL23B: {
# 1260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1266 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL23C: {
# 1269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1268 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A6=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A6,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23D: {
# 1272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1281 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL23E: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1283 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A7=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A7,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23F: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1291 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A8=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A8,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL240: {
# 1295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1294 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A9=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A9,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL241: {
# 1298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1299 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 29: _LL242: {
# 1302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1302 "parse.y"
Cyc_Lex_leave_using();
break;}case 30: _LL243: {
# 1305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1305 "parse.y"
{struct _dyneither_ptr*_tmpA65;Cyc_Lex_enter_namespace(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA65)))));}yyval=yyyyvsp[1];
break;}case 31: _LL244: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1308 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 32: _LL245: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4AB=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp4AB,(unsigned int)_tmp4AB));
break;}case 33: _LL246: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1317 "parse.y"
int _tmp4AC=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4AC));
break;}case 34: _LL247: {
# 1321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1321 "parse.y"
{struct Cyc_List_List*_tmpA66;yyval=Cyc_YY19(((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA66->tl=0,_tmpA66)))))));}
break;}case 35: _LL248: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323 "parse.y"
struct Cyc_List_List*_tmp4AE=0;
{struct Cyc_List_List*_tmp4AF=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp4AF != 0;_tmp4AF=_tmp4AF->tl){
struct _dyneither_ptr*_tmp4B0=(struct _dyneither_ptr*)_tmp4AF->hd;
struct _tuple0*_tmpA67;struct _tuple0*qv=(_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67->f1=Cyc_Absyn_Rel_n(0),((_tmpA67->f2=_tmp4B0,_tmpA67)))));
struct Cyc_Absyn_Vardecl*_tmp4B1=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA68;_tmp4AE=((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68->hd=_tmp4B1,((_tmpA68->tl=_tmp4AE,_tmpA68))))));}}
# 1330
_tmp4AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AE);
{struct Cyc_List_List*_tmpA69;yyval=Cyc_YY19(((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->hd=Cyc_Absyn_letv_decl(_tmp4AE,(unsigned int)(yyyylsp[0]).first_line),((_tmpA69->tl=0,_tmpA69)))))));}
# 1333
break;}case 36: _LL249: {
# 1335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336 "parse.y"
struct _dyneither_ptr _tmp4B5=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA6A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B5,((_tmpA6A="`H",_tag_dyneither(_tmpA6A,sizeof(char),3))))== 0){
const char*_tmpA6D;void*_tmpA6C;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA6C=0,Cyc_aprintf(((_tmpA6D="bad occurrence of heap region",_tag_dyneither(_tmpA6D,sizeof(char),30))),_tag_dyneither(_tmpA6C,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA6E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B5,((_tmpA6E="`U",_tag_dyneither(_tmpA6E,sizeof(char),3))))== 0){
const char*_tmpA71;void*_tmpA70;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA70=0,Cyc_aprintf(((_tmpA71="bad occurrence of unique region",_tag_dyneither(_tmpA71,sizeof(char),32))),_tag_dyneither(_tmpA70,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA74;struct Cyc_Absyn_Tvar*_tmpA73;struct Cyc_Absyn_Tvar*tv=(_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73->name=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=_tmp4B5,_tmpA74)))),((_tmpA73->identity=- 1,((_tmpA73->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA73)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA77;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA76;void*t=(void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=2,((_tmpA77.f1=tv,_tmpA77)))),_tmpA76))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA7D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA7C;struct _dyneither_ptr*_tmpA7A;struct _tuple0*_tmpA79;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79->f1=Cyc_Absyn_Loc_n,((_tmpA79->f2=((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA7A)))),_tmpA79)))))),(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=15,((_tmpA7D.f1=t,_tmpA7D)))),_tmpA7C)))),0);
{struct Cyc_List_List*_tmpA7E;yyval=Cyc_YY19(((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA7E->tl=0,_tmpA7E)))))));}
# 1346
break;};}case 37: _LL24A: {
# 1348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1348 "parse.y"
int _tmp4C5=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4C6=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA7F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C6,((_tmpA7F="H",_tag_dyneither(_tmpA7F,sizeof(char),2))))== 0){
const char*_tmpA80;Cyc_Parse_err(((_tmpA80="bad occurrence of heap region `H",_tag_dyneither(_tmpA80,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA81;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C6,((_tmpA81="U",_tag_dyneither(_tmpA81,sizeof(char),2))))== 0){
const char*_tmpA82;Cyc_Parse_err(((_tmpA82="bad occurrence of unique region `U",_tag_dyneither(_tmpA82,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4C5  && _tmp4C7 != 0){
const char*_tmpA83;Cyc_Parse_err(((_tmpA83="open regions cannot be @resetable",_tag_dyneither(_tmpA83,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA90;const char*_tmpA8F;void*_tmpA8E[1];struct Cyc_String_pa_PrintArg_struct _tmpA8D;struct Cyc_Absyn_Tvar*_tmpA8C;struct Cyc_Absyn_Tvar*tv=(_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->name=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=(struct _dyneither_ptr)((_tmpA8D.tag=0,((_tmpA8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C6),((_tmpA8E[0]=& _tmpA8D,Cyc_aprintf(((_tmpA8F="`%s",_tag_dyneither(_tmpA8F,sizeof(char),4))),_tag_dyneither(_tmpA8E,sizeof(void*),1)))))))),_tmpA90)))),((_tmpA8C->identity=- 1,((_tmpA8C->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA8C)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA93;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA92;void*t=(void*)((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA93.tag=2,((_tmpA93.f1=tv,_tmpA93)))),_tmpA92))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA99;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA98;struct _dyneither_ptr*_tmpA96;struct _tuple0*_tmpA95;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->f1=Cyc_Absyn_Loc_n,((_tmpA95->f2=((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=_tmp4C6,_tmpA96)))),_tmpA95)))))),(void*)((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=((_tmpA99.tag=15,((_tmpA99.f1=t,_tmpA99)))),_tmpA98)))),0);
# 1362
{struct Cyc_List_List*_tmpA9A;yyval=Cyc_YY19(((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4C5,_tmp4C7,(unsigned int)(yyyylsp[0]).first_line),((_tmpA9A->tl=0,_tmpA9A)))))));}
# 1364
break;};}case 38: _LL24B:
# 1366
 yyval=Cyc_YY34(0);
break;case 39: _LL24C: {
# 1369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1385 "parse.y"
yyval=Cyc_YY34(1);
break;}case 40: _LL24D: {
# 1388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1390 "parse.y"
yyval=yyyyvsp[0];
break;}case 41: _LL24E: {
# 1393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1392 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 42: _LL24F: {
# 1395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpA9B;yyval=Cyc_YY20(((_tmpA9B.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA9B.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA9B.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA9B.is_inline=0,((_tmpA9B.attributes=0,_tmpA9B)))))))))));}
break;}case 43: _LL250: {
# 1402
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1401 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4DA=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4DA.sc != 0){
const char*_tmpA9E;void*_tmpA9D;(_tmpA9D=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA9E="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA9E,sizeof(char),51))),_tag_dyneither(_tmpA9D,sizeof(void*),0)));}
# 1405
{struct Cyc_Parse_Declaration_spec _tmpA9F;yyval=Cyc_YY20(((_tmpA9F.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA9F.tq=_tmp4DA.tq,((_tmpA9F.type_specs=_tmp4DA.type_specs,((_tmpA9F.is_inline=_tmp4DA.is_inline,((_tmpA9F.attributes=_tmp4DA.attributes,_tmpA9F)))))))))));}
# 1409
break;}case 44: _LL251: {
# 1411
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1410 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA0;yyval=Cyc_YY20(((_tmpAA0.sc=0,((_tmpAA0.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA0.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpAA0.is_inline=0,((_tmpAA0.attributes=0,_tmpAA0)))))))))));}
break;}case 45: _LL252: {
# 1414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1413 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4DF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA1;yyval=Cyc_YY20(((_tmpAA1.sc=_tmp4DF.sc,((_tmpAA1.tq=_tmp4DF.tq,((_tmpAA1.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4DF.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpAA1.is_inline=_tmp4DF.is_inline,((_tmpAA1.attributes=_tmp4DF.attributes,_tmpAA1)))))))))));}
# 1419
break;}case 46: _LL253: {
# 1421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1420 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA2;yyval=Cyc_YY20(((_tmpAA2.sc=0,((_tmpAA2.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpAA2.type_specs=Cyc_Parse_empty_spec(0),((_tmpAA2.is_inline=0,((_tmpAA2.attributes=0,_tmpAA2)))))))))));}
break;}case 47: _LL254: {
# 1423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1422 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA3;yyval=Cyc_YY20(((_tmpAA3.sc=_tmp4E2.sc,((_tmpAA3.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4E2.tq),((_tmpAA3.type_specs=_tmp4E2.type_specs,((_tmpAA3.is_inline=_tmp4E2.is_inline,((_tmpAA3.attributes=_tmp4E2.attributes,_tmpAA3)))))))))));}
# 1427
break;}case 48: _LL255: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1428 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA4;yyval=Cyc_YY20(((_tmpAA4.sc=0,((_tmpAA4.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA4.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAA4.is_inline=1,((_tmpAA4.attributes=0,_tmpAA4)))))))))));}
break;}case 49: _LL256: {
# 1432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1431 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA5;yyval=Cyc_YY20(((_tmpAA5.sc=_tmp4E5.sc,((_tmpAA5.tq=_tmp4E5.tq,((_tmpAA5.type_specs=_tmp4E5.type_specs,((_tmpAA5.is_inline=1,((_tmpAA5.attributes=_tmp4E5.attributes,_tmpAA5)))))))))));}
# 1435
break;}case 50: _LL257: {
# 1437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1436 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA6;yyval=Cyc_YY20(((_tmpAA6.sc=0,((_tmpAA6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA6.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAA6.is_inline=0,((_tmpAA6.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpAA6)))))))))));}
break;}case 51: _LL258: {
# 1440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1439 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E8=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA7;yyval=Cyc_YY20(((_tmpAA7.sc=_tmp4E8.sc,((_tmpAA7.tq=_tmp4E8.tq,((_tmpAA7.type_specs=_tmp4E8.type_specs,((_tmpAA7.is_inline=_tmp4E8.is_inline,((_tmpAA7.attributes=
# 1442
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4E8.attributes),_tmpAA7)))))))))));}
break;}case 52: _LL259: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1446 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 53: _LL25A: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1448 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 54: _LL25B: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1450 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 55: _LL25C: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1452 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 56: _LL25D: {
# 1456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1455 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpAA8;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAA8="C",_tag_dyneither(_tmpAA8,sizeof(char),2))))!= 0){
const char*_tmpAA9;Cyc_Parse_err(((_tmpAA9="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAA9,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1460
break;}case 57: _LL25E: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1460 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 58: _LL25F: {
# 1464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1463 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 59: _LL260:
# 1467
 yyval=Cyc_YY48(0);
break;case 60: _LL261: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1470 "parse.y"
yyval=yyyyvsp[0];
break;}case 61: _LL262: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1475 "parse.y"
yyval=yyyyvsp[3];
break;}case 62: _LL263: {
# 1478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
{struct Cyc_List_List*_tmpAAA;yyval=Cyc_YY48(((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAAA->tl=0,_tmpAAA)))))));}
break;}case 63: _LL264: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1480 "parse.y"
{struct Cyc_List_List*_tmpAAB;yyval=Cyc_YY48(((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAAB->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAAB)))))));}
break;}case 64: _LL265: {
# 1483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1485 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4F0,_tmp4F0,_tmp4F0 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4F3,_tmp4F3,_tmp4F3 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4F4,_tmp4F4,_tmp4F4 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4F5,_tmp4F5,_tmp4F5 + 8},(void*)& att_aligned},{{_tmp4F6,_tmp4F6,_tmp4F6 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4F7,_tmp4F7,_tmp4F7 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4F8,_tmp4F8,_tmp4F8 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4F9,_tmp4F9,_tmp4F9 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4FA,_tmp4FA,_tmp4FA + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4FB,_tmp4FB,_tmp4FB + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4FC,_tmp4FC,_tmp4FC + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4FD,_tmp4FD,_tmp4FD + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4FE,_tmp4FE,_tmp4FE + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4FF,_tmp4FF,_tmp4FF + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp500,_tmp500,_tmp500 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1505
struct _dyneither_ptr _tmp4EE=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1507
if((((_get_dyneither_size(_tmp4EE,sizeof(char))> 4  && ((const char*)_tmp4EE.curr)[0]== '_') && ((const char*)_tmp4EE.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4EE,sizeof(char),(int)(_get_dyneither_size(_tmp4EE,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4EE,sizeof(char),(int)(_get_dyneither_size(_tmp4EE,sizeof(char))- 3)))== '_')
# 1509
_tmp4EE=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4EE,2,_get_dyneither_size(_tmp4EE,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4EE,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1516
if(i == 17){
{const char*_tmpAAC;Cyc_Parse_err(((_tmpAAC="unrecognized attribute",_tag_dyneither(_tmpAAC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1521
break;};}case 65: _LL266: {
# 1523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1521 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 66: _LL267: {
# 1524
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1523 "parse.y"
struct _dyneither_ptr _tmp502=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _tmp503=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp505;struct _tuple5 _tmp504=_tmp503;_tmp505=_tmp504.f2;{
void*a;
{const char*_tmpAAE;const char*_tmpAAD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAAE="regparm",_tag_dyneither(_tmpAAE,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAAD="__regparm__",_tag_dyneither(_tmpAAD,sizeof(char),12))))== 0){
if(_tmp505 < 0  || _tmp505 > 3){
const char*_tmpAAF;Cyc_Parse_err(((_tmpAAF="regparm requires value between 0 and 3",_tag_dyneither(_tmpAAF,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAB2;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAB1;a=(void*)((_tmpAB1=_cycalloc_atomic(sizeof(*_tmpAB1)),((_tmpAB1[0]=((_tmpAB2.tag=0,((_tmpAB2.f1=_tmp505,_tmpAB2)))),_tmpAB1))));};}else{
const char*_tmpAB4;const char*_tmpAB3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAB4="aligned",_tag_dyneither(_tmpAB4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAB3="__aligned__",_tag_dyneither(_tmpAB3,sizeof(char),12))))== 0){
if(_tmp505 < 0){const char*_tmpAB5;Cyc_Parse_err(((_tmpAB5="aligned requires positive power of two",_tag_dyneither(_tmpAB5,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp505;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpAB6;Cyc_Parse_err(((_tmpAB6="aligned requires positive power of two",_tag_dyneither(_tmpAB6,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAB9;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAB8;a=(void*)((_tmpAB8=_cycalloc_atomic(sizeof(*_tmpAB8)),((_tmpAB8[0]=((_tmpAB9.tag=6,((_tmpAB9.f1=_tmp505,_tmpAB9)))),_tmpAB8))));};};}else{
const char*_tmpABB;const char*_tmpABA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpABB="initializes",_tag_dyneither(_tmpABB,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpABA="__initializes__",_tag_dyneither(_tmpABA,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpABE;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpABD;a=(void*)((_tmpABD=_cycalloc_atomic(sizeof(*_tmpABD)),((_tmpABD[0]=((_tmpABE.tag=20,((_tmpABE.f1=_tmp505,_tmpABE)))),_tmpABD))));}else{
const char*_tmpAC0;const char*_tmpABF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAC0="noliveunique",_tag_dyneither(_tmpAC0,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpABF="__noliveunique__",_tag_dyneither(_tmpABF,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAC3;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAC2;a=(void*)((_tmpAC2=_cycalloc_atomic(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=21,((_tmpAC3.f1=_tmp505,_tmpAC3)))),_tmpAC2))));}else{
const char*_tmpAC5;const char*_tmpAC4;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAC5="noconsume",_tag_dyneither(_tmpAC5,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp502,((_tmpAC4="__noconsume__",_tag_dyneither(_tmpAC4,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAC8;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAC7;a=(void*)((_tmpAC7=_cycalloc_atomic(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=22,((_tmpAC8.f1=_tmp505,_tmpAC8)))),_tmpAC7))));}else{
# 1545
{const char*_tmpAC9;Cyc_Parse_err(((_tmpAC9="unrecognized attribute",_tag_dyneither(_tmpAC9,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1548
yyval=Cyc_YY49(a);
# 1550
break;};}case 67: _LL268: {
# 1552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1551 "parse.y"
struct _dyneither_ptr _tmp51E=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51F=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpACB;const char*_tmpACA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpACB="section",_tag_dyneither(_tmpACB,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51E,((_tmpACA="__section__",_tag_dyneither(_tmpACA,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpACE;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpACD;a=(void*)((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=((_tmpACE.tag=8,((_tmpACE.f1=_tmp51F,_tmpACE)))),_tmpACD))));}else{
# 1557
{const char*_tmpACF;Cyc_Parse_err(((_tmpACF="unrecognized attribute",_tag_dyneither(_tmpACF,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1560
yyval=Cyc_YY49(a);
# 1562
break;}case 68: _LL269: {
# 1564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1563 "parse.y"
struct _dyneither_ptr _tmp525=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp526=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1567
{const char*_tmpAD0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp525,((_tmpAD0="__mode__",_tag_dyneither(_tmpAD0,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAD3;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAD2;a=(void*)((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD3.tag=24,((_tmpAD3.f1=_tmp526,_tmpAD3)))),_tmpAD2))));}else{
# 1570
{const char*_tmpAD4;Cyc_Parse_err(((_tmpAD4="unrecognized attribute",_tag_dyneither(_tmpAD4,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1573
yyval=Cyc_YY49(a);
# 1575
break;}case 69: _LL26A: {
# 1577
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1576 "parse.y"
struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp52C=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _tmp52D=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp52F;struct _tuple5 _tmp52E=_tmp52D;_tmp52F=_tmp52E.f2;{
struct _tuple5 _tmp530=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp532;struct _tuple5 _tmp531=_tmp530;_tmp532=_tmp531.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAD6;const char*_tmpAD5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAD6="format",_tag_dyneither(_tmpAD6,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAD5="__format__",_tag_dyneither(_tmpAD5,sizeof(char),11))))== 0){
const char*_tmpAD7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpAD7="printf",_tag_dyneither(_tmpAD7,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpADA;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAD9;a=(void*)((_tmpAD9=_cycalloc_atomic(sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADA.tag=19,((_tmpADA.f1=Cyc_Absyn_Printf_ft,((_tmpADA.f2=_tmp52F,((_tmpADA.f3=_tmp532,_tmpADA)))))))),_tmpAD9))));}else{
const char*_tmpADB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpADB="scanf",_tag_dyneither(_tmpADB,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpADE;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpADD;a=(void*)((_tmpADD=_cycalloc_atomic(sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADE.tag=19,((_tmpADE.f1=Cyc_Absyn_Scanf_ft,((_tmpADE.f2=_tmp52F,((_tmpADE.f3=_tmp532,_tmpADE)))))))),_tmpADD))));}else{
# 1587
const char*_tmpADF;Cyc_Parse_err(((_tmpADF="unrecognized format type",_tag_dyneither(_tmpADF,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1589
const char*_tmpAE0;Cyc_Parse_err(((_tmpAE0="unrecognized attribute",_tag_dyneither(_tmpAE0,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1592
break;};};}case 70: _LL26B: {
# 1594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1601 "parse.y"
yyval=yyyyvsp[0];
break;}case 71: _LL26C: {
# 1604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1603 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAE3;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAE2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=((_tmpAE3.tag=17,((_tmpAE3.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAE3.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpAE3.f3=0,((_tmpAE3.f4=0,_tmpAE3)))))))))),_tmpAE2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 72: _LL26D: {
# 1606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL26E: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL26F: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1610 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL270: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL271: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL272: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL273: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL274: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1615 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL275: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL276: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL277: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL278: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619 "parse.y"
yyval=yyyyvsp[0];
break;}case 84: _LL279: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1620 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27A: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL27B: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1624 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 87: _LL27C: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1627 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAE6;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAE5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=((_tmpAE6.tag=10,((_tmpAE6.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpAE6)))),_tmpAE5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1630
break;}case 88: _LL27D: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1631 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAE9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAE8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=15,((_tmpAE9.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAE9)))),_tmpAE8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 89: _LL27E: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1633 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAEC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAEB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEC.tag=15,((_tmpAEC.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAEC)))),_tmpAEB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1635
break;}case 90: _LL27F: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1636 "parse.y"
void*_tmp545=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAEF;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAEE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE[0]=((_tmpAEF.tag=16,((_tmpAEF.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAEF.f2=_tmp545,_tmpAEF)))))),_tmpAEE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1639
break;}case 91: _LL280: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1640 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAF2;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAF1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=16,((_tmpAF2.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAF2.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpAF2)))))),_tmpAF1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL281: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1642 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAF5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4[0]=((_tmpAF5.tag=19,((_tmpAF5.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAF5)))),_tmpAF4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL282: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1644 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAF8;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7[0]=((_tmpAF8.tag=19,((_tmpAF8.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAF8)))),_tmpAF7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL283: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1646 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAFB;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAFA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=18,((_tmpAFB.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAFB)))),_tmpAFA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL284: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 96: _LL285: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAFC;yyval=Cyc_YY26(((_tmpAFC.print_const=1,((_tmpAFC.q_volatile=0,((_tmpAFC.q_restrict=0,((_tmpAFC.real_const=1,((_tmpAFC.loc=loc,_tmpAFC)))))))))));}
break;}case 97: _LL286: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1657 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAFD;yyval=Cyc_YY26(((_tmpAFD.print_const=0,((_tmpAFD.q_volatile=1,((_tmpAFD.q_restrict=0,((_tmpAFD.real_const=0,((_tmpAFD.loc=0,_tmpAFD)))))))))));}
break;}case 98: _LL287: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1658 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAFE;yyval=Cyc_YY26(((_tmpAFE.print_const=0,((_tmpAFE.q_volatile=0,((_tmpAFE.q_restrict=1,((_tmpAFE.real_const=0,((_tmpAFE.loc=0,_tmpAFE)))))))))));}
break;}case 99: _LL288: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB0D;struct Cyc_Absyn_Enumdecl*_tmpB0C;struct Cyc_Core_Opt*_tmpB0B;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB0A;struct Cyc_Absyn_TypeDecl*_tmpB09;struct Cyc_Absyn_TypeDecl*_tmp553=(_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->r=(void*)((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((_tmpB0A.tag=1,((_tmpB0A.f1=((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->sc=Cyc_Absyn_Public,((_tmpB0C->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB0C->fields=((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB0B)))),_tmpB0C)))))))),_tmpB0A)))),_tmpB0D)))),((_tmpB09->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB09)))));
# 1666
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB10;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB0F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=26,((_tmpB10.f1=_tmp553,((_tmpB10.f2=0,_tmpB10)))))),_tmpB0F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1668
break;}case 100: _LL289: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1669 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB13;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB12;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12[0]=((_tmpB13.tag=13,((_tmpB13.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB13.f2=0,_tmpB13)))))),_tmpB12)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL28A: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1671 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB16;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB15;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15[0]=((_tmpB16.tag=14,((_tmpB16.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB16)))),_tmpB15)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL28B: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB17;yyval=Cyc_YY50(((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB17->tag=0,((_tmpB17->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB17)))))))));}
break;}case 103: _LL28C: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1679 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB18;yyval=Cyc_YY50(((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB18->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB18->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB18)))))))));}
break;}case 104: _LL28D: {
# 1682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1683 "parse.y"
{struct Cyc_List_List*_tmpB19;yyval=Cyc_YY51(((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB19->tl=0,_tmpB19)))))));}
break;}case 105: _LL28E: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1684 "parse.y"
{struct Cyc_List_List*_tmpB1A;yyval=Cyc_YY51(((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB1A->tl=0,_tmpB1A)))))));}
break;}case 106: _LL28F: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1685 "parse.y"
{struct Cyc_List_List*_tmpB1B;yyval=Cyc_YY51(((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB1B->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB1B)))))));}
break;}case 107: _LL290: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB1E;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB1D;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=((_tmpB1E.tag=12,((_tmpB1E.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB1E.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpB1E)))))),_tmpB1D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 108: _LL291: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1697 "parse.y"
struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp568=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp566,
Cyc_Absyn_aggrdecl_impl(_tmp567,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1702
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB21;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB20;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB21.tag=26,((_tmpB21.f1=_tmp568,((_tmpB21.f2=0,_tmpB21)))))),_tmpB20)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1704
break;}case 109: _LL292: {
# 1706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1708 "parse.y"
struct Cyc_List_List*_tmp56B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp56C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp56D=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp56B,
Cyc_Absyn_aggrdecl_impl(_tmp56C,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1713
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB24;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB23;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23[0]=((_tmpB24.tag=26,((_tmpB24.f1=_tmp56D,((_tmpB24.f2=0,_tmpB24)))))),_tmpB23)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1715
break;}case 110: _LL293: {
# 1717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1716 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB2E;struct Cyc_Core_Opt*_tmpB2D;struct Cyc_Absyn_AggrInfo _tmpB2C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB2B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2E.tag=11,((_tmpB2E.f1=((_tmpB2C.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB2D=_cycalloc_atomic(sizeof(*_tmpB2D)),((_tmpB2D->v=(void*)1,_tmpB2D))))),((_tmpB2C.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB2C)))),_tmpB2E)))),_tmpB2B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1719
break;}case 111: _LL294: {
# 1721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1720 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB34;struct Cyc_Absyn_AggrInfo _tmpB33;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB32;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB34.tag=11,((_tmpB34.f1=((_tmpB33.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB33.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB33)))),_tmpB34)))),_tmpB32)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1723
break;}case 112: _LL295:
# 1725
 yyval=Cyc_YY43(0);
break;case 113: _LL296: {
# 1728
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1730
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1729 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 114: _LL297: {
# 1732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1733 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 115: _LL298: {
# 1736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1734 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 116: _LL299:
# 1737
 yyval=Cyc_YY27(0);
break;case 117: _LL29A: {
# 1740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1741 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp577=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp577 != 0;_tmp577=_tmp577->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp577->hd,decls);}}{
# 1745
struct Cyc_List_List*_tmp578=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp578 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp578,decls);
yyval=Cyc_YY27(decls);
# 1750
break;};}case 118: _LL29B: {
# 1752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB35;yyval=Cyc_YY28(((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB35->tl=0,_tmpB35)))))));}
break;}case 119: _LL29C: {
# 1758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1757 "parse.y"
{struct Cyc_List_List*_tmpB36;yyval=Cyc_YY28(((_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB36->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB36)))))));}
break;}case 120: _LL29D: {
# 1760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1761 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 121: _LL29E: {
# 1764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB37;yyval=Cyc_YY22(((_tmpB37=_region_malloc(yyr,sizeof(*_tmpB37)),((_tmpB37->tl=0,((_tmpB37->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB37)))))));}
break;}case 122: _LL29F: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1769 "parse.y"
{struct _tuple12*_tmpB38;yyval=Cyc_YY22(((_tmpB38=_region_malloc(yyr,sizeof(*_tmpB38)),((_tmpB38->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB38->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB38)))))));}
break;}case 123: _LL2A0: {
# 1772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1774 "parse.y"
{struct _tuple11 _tmpB39;yyval=Cyc_YY21(((_tmpB39.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB39.f2=0,_tmpB39)))));}
break;}case 124: _LL2A1: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1776 "parse.y"
{struct _tuple11 _tmpB3A;yyval=Cyc_YY21(((_tmpB3A.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB3A.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB3A)))));}
break;}case 125: _LL2A2: {
# 1779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1782 "parse.y"
struct _RegionHandle _tmp57F=_new_region("temp");struct _RegionHandle*temp=& _tmp57F;_push_region(temp);{
struct _tuple25 _tmp580=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp582;struct Cyc_Parse_Type_specifier _tmp583;struct Cyc_List_List*_tmp584;struct _tuple25 _tmp581=_tmp580;_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp584=_tmp581.f3;
if(_tmp582.loc == 0)_tmp582.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp585=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp585 != 0;_tmp585=_tmp585->tl){
struct _tuple24*_tmp586=(struct _tuple24*)_tmp585->hd;struct Cyc_Parse_Declarator _tmp588;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Exp*_tmp58A;struct _tuple24*_tmp587=_tmp586;_tmp588=_tmp587->f1;_tmp589=_tmp587->f2;_tmp58A=_tmp587->f3;
{struct _tuple10*_tmpB3B;decls=((_tmpB3B=_region_malloc(temp,sizeof(*_tmpB3B)),((_tmpB3B->tl=decls,((_tmpB3B->hd=_tmp588,_tmpB3B))))));}{
struct _tuple16*_tmpB3E;struct Cyc_List_List*_tmpB3D;widths_and_reqs=(
(_tmpB3D=_region_malloc(temp,sizeof(*_tmpB3D)),((_tmpB3D->hd=((_tmpB3E=_region_malloc(temp,sizeof(*_tmpB3E)),((_tmpB3E->f1=_tmp589,((_tmpB3E->f2=_tmp58A,_tmpB3E)))))),((_tmpB3D->tl=widths_and_reqs,_tmpB3D))))));};}}
# 1793
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp58E=Cyc_Parse_speclist2typ(_tmp583,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp582,_tmp58E,decls,_tmp584),widths_and_reqs);
# 1799
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp58F));};};}
# 1802
_npop_handler(0);break;
# 1782
;_pop_region(temp);}case 126: _LL2A3: {
# 1804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1810 "parse.y"
{struct _tuple25 _tmpB3F;yyval=Cyc_YY38(((_tmpB3F.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB3F.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB3F.f3=0,_tmpB3F)))))));}
break;}case 127: _LL2A4: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1812 "parse.y"
struct _tuple25 _tmp591=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB40;yyval=Cyc_YY38(((_tmpB40.f1=_tmp591.f1,((_tmpB40.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp591.f2),((_tmpB40.f3=_tmp591.f3,_tmpB40)))))));}
break;}case 128: _LL2A5: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1814 "parse.y"
{struct _tuple25 _tmpB41;yyval=Cyc_YY38(((_tmpB41.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB41.f2=Cyc_Parse_empty_spec(0),((_tmpB41.f3=0,_tmpB41)))))));}
break;}case 129: _LL2A6: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1816 "parse.y"
struct _tuple25 _tmp594=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB42;yyval=Cyc_YY38(((_tmpB42.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp594.f1),((_tmpB42.f2=_tmp594.f2,((_tmpB42.f3=_tmp594.f3,_tmpB42)))))));}
break;}case 130: _LL2A7: {
# 1820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1819 "parse.y"
{struct _tuple25 _tmpB43;yyval=Cyc_YY38(((_tmpB43.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB43.f2=Cyc_Parse_empty_spec(0),((_tmpB43.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB43)))))));}
break;}case 131: _LL2A8: {
# 1822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1821 "parse.y"
struct _tuple25 _tmp597=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB44;yyval=Cyc_YY38(((_tmpB44.f1=_tmp597.f1,((_tmpB44.f2=_tmp597.f2,((_tmpB44.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp597.f3),_tmpB44)))))));}
break;}case 132: _LL2A9: {
# 1824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB45;yyval=Cyc_YY38(((_tmpB45.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB45.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB45.f3=0,_tmpB45)))))));}
break;}case 133: _LL2AA: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1829 "parse.y"
struct _tuple25 _tmp59A=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB46;yyval=Cyc_YY38(((_tmpB46.f1=_tmp59A.f1,((_tmpB46.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp59A.f2),((_tmpB46.f3=_tmp59A.f3,_tmpB46)))))));}
break;}case 134: _LL2AB: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1831 "parse.y"
{struct _tuple25 _tmpB47;yyval=Cyc_YY38(((_tmpB47.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB47.f2=Cyc_Parse_empty_spec(0),((_tmpB47.f3=0,_tmpB47)))))));}
break;}case 135: _LL2AC: {
# 1834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1833 "parse.y"
struct _tuple25 _tmp59D=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB48;yyval=Cyc_YY38(((_tmpB48.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp59D.f1),((_tmpB48.f2=_tmp59D.f2,((_tmpB48.f3=_tmp59D.f3,_tmpB48)))))));}
break;}case 136: _LL2AD: {
# 1837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1836 "parse.y"
{struct _tuple25 _tmpB49;yyval=Cyc_YY38(((_tmpB49.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB49.f2=Cyc_Parse_empty_spec(0),((_tmpB49.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB49)))))));}
break;}case 137: _LL2AE: {
# 1839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1838 "parse.y"
struct _tuple25 _tmp5A0=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB4A;yyval=Cyc_YY38(((_tmpB4A.f1=_tmp5A0.f1,((_tmpB4A.f2=_tmp5A0.f2,((_tmpB4A.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp5A0.f3),_tmpB4A)))))));}
break;}case 138: _LL2AF: {
# 1841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1842 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 139: _LL2B0: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB4B;yyval=Cyc_YY32(((_tmpB4B=_region_malloc(yyr,sizeof(*_tmpB4B)),((_tmpB4B->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB4B->tl=0,_tmpB4B)))))));}
break;}case 140: _LL2B1: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1850 "parse.y"
{struct Cyc_List_List*_tmpB4C;yyval=Cyc_YY32(((_tmpB4C=_region_malloc(yyr,sizeof(*_tmpB4C)),((_tmpB4C->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB4C->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB4C)))))));}
break;}case 141: _LL2B2: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1855 "parse.y"
{struct _tuple24*_tmpB4D;yyval=Cyc_YY31(((_tmpB4D=_region_malloc(yyr,sizeof(*_tmpB4D)),((_tmpB4D->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB4D->f2=0,((_tmpB4D->f3=0,_tmpB4D)))))))));}
break;}case 142: _LL2B3: {
# 1858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1859 "parse.y"
{struct Cyc_Parse_Declarator _tmpB53;struct _tuple0*_tmpB52;struct _tuple24*_tmpB51;yyval=Cyc_YY31(((_tmpB51=_region_malloc(yyr,sizeof(*_tmpB51)),((_tmpB51->f1=((_tmpB53.id=((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->f1=Cyc_Absyn_Rel_n(0),((_tmpB52->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB52)))))),((_tmpB53.tms=0,_tmpB53)))),((_tmpB51->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB51->f3=0,_tmpB51)))))))));}
# 1861
break;}case 143: _LL2B4:
# 1865
{struct Cyc_Parse_Declarator _tmpB59;struct _tuple0*_tmpB58;struct _tuple24*_tmpB57;yyval=Cyc_YY31(((_tmpB57=_region_malloc(yyr,sizeof(*_tmpB57)),((_tmpB57->f1=((_tmpB59.id=((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->f1=Cyc_Absyn_Rel_n(0),((_tmpB58->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB58)))))),((_tmpB59.tms=0,_tmpB59)))),((_tmpB57->f2=0,((_tmpB57->f3=0,_tmpB57)))))))));}
# 1867
break;case 144: _LL2B5: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1867 "parse.y"
{struct _tuple24*_tmpB5A;yyval=Cyc_YY31(((_tmpB5A=_region_malloc(yyr,sizeof(*_tmpB5A)),((_tmpB5A->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB5A->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB5A->f3=0,_tmpB5A)))))))));}
break;}case 145: _LL2B6: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1869 "parse.y"
{struct _tuple24*_tmpB5B;yyval=Cyc_YY31(((_tmpB5B=_region_malloc(yyr,sizeof(*_tmpB5B)),((_tmpB5B->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB5B->f2=0,((_tmpB5B->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB5B)))))))));}
break;}case 146: _LL2B7: {
# 1872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp5AF=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp5B0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB5C;struct Cyc_Absyn_TypeDecl*_tmp5B1=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5B0,((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB5C)))),_tmp5AF,(unsigned int)(yyyylsp[0]).first_line);
# 1879
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB5F;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E[0]=((_tmpB5F.tag=26,((_tmpB5F.f1=_tmp5B1,((_tmpB5F.f2=0,_tmpB5F)))))),_tmpB5E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1881
break;}case 147: _LL2B8: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1882 "parse.y"
int _tmp5B5=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB69;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB68;struct Cyc_Absyn_DatatypeInfo _tmpB67;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB66;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB69.tag=3,((_tmpB69.f1=((_tmpB67.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB68.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB68.is_extensible=_tmp5B5,_tmpB68))))),((_tmpB67.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB67)))),_tmpB69)))),_tmpB66)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1885
break;}case 148: _LL2B9: {
# 1887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1886 "parse.y"
int _tmp5BA=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB73;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB72;struct Cyc_Absyn_DatatypeFieldInfo _tmpB71;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB70;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70[0]=((_tmpB73.tag=4,((_tmpB73.f1=((_tmpB71.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB72.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB72.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB72.is_extensible=_tmp5BA,_tmpB72))))))),((_tmpB71.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB71)))),_tmpB73)))),_tmpB70)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1891
break;}case 149: _LL2BA: {
# 1893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1894 "parse.y"
yyval=Cyc_YY34(0);
break;}case 150: _LL2BB: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1895 "parse.y"
yyval=Cyc_YY34(1);
break;}case 151: _LL2BC: {
# 1898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpB74;yyval=Cyc_YY37(((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB74->tl=0,_tmpB74)))))));}
break;}case 152: _LL2BD: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1900 "parse.y"
{struct Cyc_List_List*_tmpB75;yyval=Cyc_YY37(((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB75->tl=0,_tmpB75)))))));}
break;}case 153: _LL2BE: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1901 "parse.y"
{struct Cyc_List_List*_tmpB76;yyval=Cyc_YY37(((_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB76->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB76)))))));}
break;}case 154: _LL2BF: {
# 1904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1902 "parse.y"
{struct Cyc_List_List*_tmpB77;yyval=Cyc_YY37(((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB77->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB77)))))));}
break;}case 155: _LL2C0:
# 1905
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 156: _LL2C1: {
# 1908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1907 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 157: _LL2C2: {
# 1910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1908 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 158: _LL2C3: {
# 1911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1912 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB78;yyval=Cyc_YY36(((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB78->typs=0,((_tmpB78->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB78->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB78)))))))))));}
break;}case 159: _LL2C4: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1914 "parse.y"
struct Cyc_List_List*_tmp5C4=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB79;yyval=Cyc_YY36(((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB79->typs=_tmp5C4,((_tmpB79->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB79->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB79)))))))))));}
break;}case 160: _LL2C5: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1920 "parse.y"
yyval=yyyyvsp[0];
break;}case 161: _LL2C6: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1922 "parse.y"
struct Cyc_Parse_Declarator _tmp5C6=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB7A;yyval=Cyc_YY30(((_tmpB7A.id=_tmp5C6.id,((_tmpB7A.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5C6.tms),_tmpB7A)))));}
break;}case 162: _LL2C7: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 163: _LL2C8: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1931 "parse.y"
struct Cyc_Parse_Declarator _tmp5C8=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB7B;yyval=Cyc_YY30(((_tmpB7B.id=_tmp5C8.id,((_tmpB7B.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5C8.tms),_tmpB7B)))));}
break;}case 164: _LL2C9: {
# 1935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1937 "parse.y"
{struct Cyc_Parse_Declarator _tmpB7C;yyval=Cyc_YY30(((_tmpB7C.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB7C.tms=0,_tmpB7C)))));}
break;}case 165: _LL2CA: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1939 "parse.y"
yyval=yyyyvsp[1];
break;}case 166: _LL2CB: {
# 1942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1943 "parse.y"
struct Cyc_Parse_Declarator _tmp5CB=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB82;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB81;struct Cyc_List_List*_tmpB80;_tmp5CB.tms=((_tmpB80=_region_malloc(yyr,sizeof(*_tmpB80)),((_tmpB80->hd=(void*)((_tmpB82=_region_malloc(yyr,sizeof(*_tmpB82)),((_tmpB82[0]=((_tmpB81.tag=5,((_tmpB81.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB81.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB81)))))),_tmpB82)))),((_tmpB80->tl=_tmp5CB.tms,_tmpB80))))));}
yyval=yyyyvsp[2];
# 1947
break;}case 167: _LL2CC: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1948 "parse.y"
{struct Cyc_List_List*_tmpB8C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB8B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB8A;struct Cyc_Parse_Declarator _tmpB89;yyval=Cyc_YY30(((_tmpB89.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB89.tms=((_tmpB8C=_region_malloc(yyr,sizeof(*_tmpB8C)),((_tmpB8C->hd=(void*)((_tmpB8A=_region_malloc(yyr,sizeof(*_tmpB8A)),((_tmpB8A[0]=((_tmpB8B.tag=0,((_tmpB8B.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB8B.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB8B)))))),_tmpB8A)))),((_tmpB8C->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB8C)))))),_tmpB89)))));}
break;}case 168: _LL2CD: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1950 "parse.y"
{struct Cyc_List_List*_tmpB96;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB95;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB94;struct Cyc_Parse_Declarator _tmpB93;yyval=Cyc_YY30(((_tmpB93.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB93.tms=(
(_tmpB96=_region_malloc(yyr,sizeof(*_tmpB96)),((_tmpB96->hd=(void*)((_tmpB94=_region_malloc(yyr,sizeof(*_tmpB94)),((_tmpB94[0]=((_tmpB95.tag=1,((_tmpB95.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB95.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB95.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB95)))))))),_tmpB94)))),((_tmpB96->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB96)))))),_tmpB93)))));}
break;}case 169: _LL2CE: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953 "parse.y"
struct _tuple26*_tmp5D7=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5D9;int _tmp5DA;struct Cyc_Absyn_VarargInfo*_tmp5DB;void*_tmp5DC;struct Cyc_List_List*_tmp5DD;struct _tuple26*_tmp5D8=_tmp5D7;_tmp5D9=_tmp5D8->f1;_tmp5DA=_tmp5D8->f2;_tmp5DB=_tmp5D8->f3;_tmp5DC=_tmp5D8->f4;_tmp5DD=_tmp5D8->f5;
{struct Cyc_List_List*_tmpBAB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBAA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBA9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBA8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBA7;struct Cyc_Parse_Declarator _tmpBA6;yyval=Cyc_YY30(((_tmpBA6.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA6.tms=((_tmpBAB=_region_malloc(yyr,sizeof(*_tmpBAB)),((_tmpBAB->hd=(void*)((_tmpBA7=_region_malloc(yyr,sizeof(*_tmpBA7)),((_tmpBA7[0]=((_tmpBAA.tag=3,((_tmpBAA.f1=(void*)((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA9.tag=1,((_tmpBA9.f1=_tmp5D9,((_tmpBA9.f2=_tmp5DA,((_tmpBA9.f3=_tmp5DB,((_tmpBA9.f4=_tmp5DC,((_tmpBA9.f5=_tmp5DD,_tmpBA9)))))))))))),_tmpBA8)))),_tmpBAA)))),_tmpBA7)))),((_tmpBAB->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBAB)))))),_tmpBA6)))));}
# 1956
break;}case 170: _LL2CF: {
# 1958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1957 "parse.y"
{struct Cyc_List_List*_tmpBC0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBBF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBBE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBBD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBBC;struct Cyc_Parse_Declarator _tmpBBB;yyval=Cyc_YY30(((_tmpBBB.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBBB.tms=(
(_tmpBC0=_region_malloc(yyr,sizeof(*_tmpBC0)),((_tmpBC0->hd=(void*)((_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC[0]=((_tmpBBF.tag=3,((_tmpBBF.f1=(void*)((_tmpBBD=_region_malloc(yyr,sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBE.tag=1,((_tmpBBE.f1=0,((_tmpBBE.f2=0,((_tmpBBE.f3=0,((_tmpBBE.f4=
# 1960
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpBBE.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpBBE)))))))))))),_tmpBBD)))),_tmpBBF)))),_tmpBBC)))),((_tmpBC0->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBC0)))))),_tmpBBB)))));}
# 1963
break;}case 171: _LL2D0: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1964 "parse.y"
{struct Cyc_List_List*_tmpBD5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD4;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBD3;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBD2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBD1;struct Cyc_Parse_Declarator _tmpBD0;yyval=Cyc_YY30(((_tmpBD0.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBD0.tms=((_tmpBD5=_region_malloc(yyr,sizeof(*_tmpBD5)),((_tmpBD5->hd=(void*)((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD4.tag=3,((_tmpBD4.f1=(void*)((_tmpBD2=_region_malloc(yyr,sizeof(*_tmpBD2)),((_tmpBD2[0]=((_tmpBD3.tag=0,((_tmpBD3.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBD3.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBD3)))))),_tmpBD2)))),_tmpBD4)))),_tmpBD1)))),((_tmpBD5->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBD5)))))),_tmpBD0)))));}
break;}case 172: _LL2D1: {
# 1967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1967 "parse.y"
struct Cyc_List_List*_tmp5F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBDF;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBDE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBDD;struct Cyc_Parse_Declarator _tmpBDC;yyval=Cyc_YY30(((_tmpBDC.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBDC.tms=((_tmpBDF=_region_malloc(yyr,sizeof(*_tmpBDF)),((_tmpBDF->hd=(void*)((_tmpBDD=_region_malloc(yyr,sizeof(*_tmpBDD)),((_tmpBDD[0]=((_tmpBDE.tag=4,((_tmpBDE.f1=_tmp5F0,((_tmpBDE.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBDE.f3=0,_tmpBDE)))))))),_tmpBDD)))),((_tmpBDF->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBDF)))))),_tmpBDC)))));}
# 1970
break;}case 173: _LL2D2: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1971 "parse.y"
{struct Cyc_List_List*_tmpBE9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBE8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBE7;struct Cyc_Parse_Declarator _tmpBE6;yyval=Cyc_YY30(((_tmpBE6.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBE6.tms=((_tmpBE9=_region_malloc(yyr,sizeof(*_tmpBE9)),((_tmpBE9->hd=(void*)((_tmpBE7=_region_malloc(yyr,sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBE8.tag=5,((_tmpBE8.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBE8.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBE8)))))),_tmpBE7)))),((_tmpBE9->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBE9)))))),_tmpBE6)))));}
# 1974
break;}case 174: _LL2D3: {
# 1976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1985 "parse.y"
{struct Cyc_Parse_Declarator _tmpBEA;yyval=Cyc_YY30(((_tmpBEA.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBEA.tms=0,_tmpBEA)))));}
break;}case 175: _LL2D4: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1987 "parse.y"
{struct Cyc_Parse_Declarator _tmpBEB;yyval=Cyc_YY30(((_tmpBEB.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBEB.tms=0,_tmpBEB)))));}
break;}case 176: _LL2D5: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1989 "parse.y"
yyval=yyyyvsp[1];
break;}case 177: _LL2D6: {
# 1992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1993 "parse.y"
struct Cyc_Parse_Declarator _tmp5FB=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBF1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBF0;struct Cyc_List_List*_tmpBEF;_tmp5FB.tms=((_tmpBEF=_region_malloc(yyr,sizeof(*_tmpBEF)),((_tmpBEF->hd=(void*)((_tmpBF1=_region_malloc(yyr,sizeof(*_tmpBF1)),((_tmpBF1[0]=((_tmpBF0.tag=5,((_tmpBF0.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBF0.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBF0)))))),_tmpBF1)))),((_tmpBEF->tl=_tmp5FB.tms,_tmpBEF))))));}
yyval=yyyyvsp[2];
# 1997
break;}case 178: _LL2D7: {
# 1999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1998 "parse.y"
struct Cyc_Parse_Declarator _tmp5FF=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBFB;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBFA;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBF9;struct Cyc_Parse_Declarator _tmpBF8;yyval=Cyc_YY30(((_tmpBF8.id=_tmp5FF.id,((_tmpBF8.tms=(
(_tmpBFB=_region_malloc(yyr,sizeof(*_tmpBFB)),((_tmpBFB->hd=(void*)((_tmpBF9=_region_malloc(yyr,sizeof(*_tmpBF9)),((_tmpBF9[0]=((_tmpBFA.tag=0,((_tmpBFA.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBFA.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBFA)))))),_tmpBF9)))),((_tmpBFB->tl=_tmp5FF.tms,_tmpBFB)))))),_tmpBF8)))));}
break;}case 179: _LL2D8: {
# 2003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2002 "parse.y"
struct Cyc_Parse_Declarator _tmp604=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC05;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC04;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC03;struct Cyc_Parse_Declarator _tmpC02;yyval=Cyc_YY30(((_tmpC02.id=_tmp604.id,((_tmpC02.tms=(
(_tmpC05=_region_malloc(yyr,sizeof(*_tmpC05)),((_tmpC05->hd=(void*)((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=1,((_tmpC04.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpC04.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpC04.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC04)))))))),_tmpC03)))),((_tmpC05->tl=_tmp604.tms,_tmpC05)))))),_tmpC02)))));}
# 2006
break;}case 180: _LL2D9: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2007 "parse.y"
struct _tuple26*_tmp609=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp60B;int _tmp60C;struct Cyc_Absyn_VarargInfo*_tmp60D;void*_tmp60E;struct Cyc_List_List*_tmp60F;struct _tuple26*_tmp60A=_tmp609;_tmp60B=_tmp60A->f1;_tmp60C=_tmp60A->f2;_tmp60D=_tmp60A->f3;_tmp60E=_tmp60A->f4;_tmp60F=_tmp60A->f5;{
struct Cyc_Parse_Declarator _tmp610=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC1A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC19;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC18;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC17;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC16;struct Cyc_Parse_Declarator _tmpC15;yyval=Cyc_YY30(((_tmpC15.id=_tmp610.id,((_tmpC15.tms=((_tmpC1A=_region_malloc(yyr,sizeof(*_tmpC1A)),((_tmpC1A->hd=(void*)((_tmpC16=_region_malloc(yyr,sizeof(*_tmpC16)),((_tmpC16[0]=((_tmpC19.tag=3,((_tmpC19.f1=(void*)((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC18.tag=1,((_tmpC18.f1=_tmp60B,((_tmpC18.f2=_tmp60C,((_tmpC18.f3=_tmp60D,((_tmpC18.f4=_tmp60E,((_tmpC18.f5=_tmp60F,_tmpC18)))))))))))),_tmpC17)))),_tmpC19)))),_tmpC16)))),((_tmpC1A->tl=_tmp610.tms,_tmpC1A)))))),_tmpC15)))));}
# 2011
break;};}case 181: _LL2DA: {
# 2013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2012 "parse.y"
struct Cyc_Parse_Declarator _tmp617=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC2E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC2D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC2C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC2B;struct Cyc_Parse_Declarator _tmpC2A;yyval=Cyc_YY30(((_tmpC2A.id=_tmp617.id,((_tmpC2A.tms=(
(_tmpC2F=_region_malloc(yyr,sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)((_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B[0]=((_tmpC2E.tag=3,((_tmpC2E.f1=(void*)((_tmpC2C=_region_malloc(yyr,sizeof(*_tmpC2C)),((_tmpC2C[0]=((_tmpC2D.tag=1,((_tmpC2D.f1=0,((_tmpC2D.f2=0,((_tmpC2D.f3=0,((_tmpC2D.f4=
# 2016
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC2D.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC2D)))))))))))),_tmpC2C)))),_tmpC2E)))),_tmpC2B)))),((_tmpC2F->tl=_tmp617.tms,_tmpC2F)))))),_tmpC2A)))));}
# 2019
break;}case 182: _LL2DB: {
# 2021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2020 "parse.y"
struct Cyc_Parse_Declarator _tmp61E=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC44;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC43;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC42;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC41;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC40;struct Cyc_Parse_Declarator _tmpC3F;yyval=Cyc_YY30(((_tmpC3F.id=_tmp61E.id,((_tmpC3F.tms=((_tmpC44=_region_malloc(yyr,sizeof(*_tmpC44)),((_tmpC44->hd=(void*)((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC43.tag=3,((_tmpC43.f1=(void*)((_tmpC41=_region_malloc(yyr,sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=0,((_tmpC42.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC42.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC42)))))),_tmpC41)))),_tmpC43)))),_tmpC40)))),((_tmpC44->tl=_tmp61E.tms,_tmpC44)))))),_tmpC3F)))));}
break;}case 183: _LL2DC: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2024 "parse.y"
struct Cyc_List_List*_tmp625=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp626=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC4E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC4D;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC4C;struct Cyc_Parse_Declarator _tmpC4B;yyval=Cyc_YY30(((_tmpC4B.id=_tmp626.id,((_tmpC4B.tms=((_tmpC4E=_region_malloc(yyr,sizeof(*_tmpC4E)),((_tmpC4E->hd=(void*)((_tmpC4C=_region_malloc(yyr,sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=4,((_tmpC4D.f1=_tmp625,((_tmpC4D.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC4D.f3=0,_tmpC4D)))))))),_tmpC4C)))),((_tmpC4E->tl=_tmp626.tms,_tmpC4E)))))),_tmpC4B)))));}
# 2028
break;}case 184: _LL2DD: {
# 2030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2029 "parse.y"
struct Cyc_Parse_Declarator _tmp62B=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC58;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC57;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC56;struct Cyc_Parse_Declarator _tmpC55;yyval=Cyc_YY30(((_tmpC55.id=_tmp62B.id,((_tmpC55.tms=((_tmpC58=_region_malloc(yyr,sizeof(*_tmpC58)),((_tmpC58->hd=(void*)((_tmpC56=_region_malloc(yyr,sizeof(*_tmpC56)),((_tmpC56[0]=((_tmpC57.tag=5,((_tmpC57.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC57.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC57)))))),_tmpC56)))),((_tmpC58->tl=_tmp62B.tms,_tmpC58)))))),_tmpC55)))));}
# 2032
break;}case 185: _LL2DE: {
# 2034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2042 "parse.y"
yyval=yyyyvsp[0];
break;}case 186: _LL2DF: {
# 2045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2043 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 187: _LL2E0: {
# 2046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2047 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC5E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC5D;struct Cyc_List_List*_tmpC5C;ans=((_tmpC5C=_region_malloc(yyr,sizeof(*_tmpC5C)),((_tmpC5C->hd=(void*)((_tmpC5E=_region_malloc(yyr,sizeof(*_tmpC5E)),((_tmpC5E[0]=((_tmpC5D.tag=5,((_tmpC5D.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC5D.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC5D)))))),_tmpC5E)))),((_tmpC5C->tl=ans,_tmpC5C))))));}{
# 2051
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp633=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp635;union Cyc_Absyn_Constraint*_tmp636;union Cyc_Absyn_Constraint*_tmp637;struct _tuple21 _tmp634=_tmp633;_tmp635=_tmp634.f1;_tmp636=_tmp634.f2;_tmp637=_tmp634.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC5F;ptrloc=((_tmpC5F=_cycalloc_atomic(sizeof(*_tmpC5F)),((_tmpC5F->ptr_loc=_tmp635,((_tmpC5F->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC5F->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC5F))))))));}{
# 2056
struct _tuple14 _tmp639=Cyc_Parse_collapse_pointer_quals(_tmp635,_tmp636,_tmp637,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp63B;union Cyc_Absyn_Constraint*_tmp63C;union Cyc_Absyn_Constraint*_tmp63D;void*_tmp63E;struct _tuple14 _tmp63A=_tmp639;_tmp63B=_tmp63A.f1;_tmp63C=_tmp63A.f2;_tmp63D=_tmp63A.f3;_tmp63E=_tmp63A.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC69;struct Cyc_Absyn_PtrAtts _tmpC68;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC67;struct Cyc_List_List*_tmpC66;ans=((_tmpC66=_region_malloc(yyr,sizeof(*_tmpC66)),((_tmpC66->hd=(void*)((_tmpC69=_region_malloc(yyr,sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC67.tag=2,((_tmpC67.f1=((_tmpC68.rgn=_tmp63E,((_tmpC68.nullable=_tmp63B,((_tmpC68.bounds=_tmp63C,((_tmpC68.zero_term=_tmp63D,((_tmpC68.ptrloc=ptrloc,_tmpC68)))))))))),((_tmpC67.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC67)))))),_tmpC69)))),((_tmpC66->tl=ans,_tmpC66))))));}
yyval=Cyc_YY29(ans);
# 2060
break;};};}case 188: _LL2E1:
# 2062
 yyval=Cyc_YY57(0);
break;case 189: _LL2E2: {
# 2065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2063 "parse.y"
{struct Cyc_List_List*_tmpC6A;yyval=Cyc_YY57(((_tmpC6A=_region_malloc(yyr,sizeof(*_tmpC6A)),((_tmpC6A->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC6A->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC6A)))))));}
break;}case 190: _LL2E3: {
# 2066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2068 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC6D;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC6C;yyval=Cyc_YY56((void*)((_tmpC6C=_region_malloc(yyr,sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=0,((_tmpC6D.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC6D)))),_tmpC6C)))));}
break;}case 191: _LL2E4: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2070 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC70;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC6F;yyval=Cyc_YY56((void*)((_tmpC6F=_region_malloc(yyr,sizeof(*_tmpC6F)),((_tmpC6F[0]=((_tmpC70.tag=1,((_tmpC70.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC70)))),_tmpC6F)))));}
break;}case 192: _LL2E5: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC73;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC72;yyval=Cyc_YY56((void*)((_tmpC72=_region_malloc(yyr,sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=2,_tmpC73)),_tmpC72)))));}
break;}case 193: _LL2E6: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC76;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC75;yyval=Cyc_YY56((void*)((_tmpC75=_region_malloc(yyr,sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC76.tag=3,_tmpC76)),_tmpC75)))));}
break;}case 194: _LL2E7: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC79;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC78;yyval=Cyc_YY56((void*)((_tmpC78=_region_malloc(yyr,sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC79.tag=4,_tmpC79)),_tmpC78)))));}
break;}case 195: _LL2E8: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC7C;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC7B;yyval=Cyc_YY56((void*)((_tmpC7B=_region_malloc(yyr,sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7C.tag=5,_tmpC7C)),_tmpC7B)))));}
break;}case 196: _LL2E9: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2080 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC7F;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC7E;yyval=Cyc_YY56((void*)((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=6,_tmpC7F)),_tmpC7E)))));}
break;}case 197: _LL2EA: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2082 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC82;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC81;yyval=Cyc_YY56((void*)((_tmpC81=_region_malloc(yyr,sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=7,_tmpC82)),_tmpC81)))));}
break;}case 198: _LL2EB: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2088 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC83;yyval=Cyc_YY1(((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->f1=loc,((_tmpC83->f2=Cyc_Absyn_true_conref,((_tmpC83->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC83)))))))));}
# 2091
break;}case 199: _LL2EC: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2092 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC84;yyval=Cyc_YY1(((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->f1=loc,((_tmpC84->f2=Cyc_Absyn_false_conref,((_tmpC84->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC84)))))))));}
# 2095
break;}case 200: _LL2ED: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2096 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC85;yyval=Cyc_YY1(((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->f1=loc,((_tmpC85->f2=Cyc_Absyn_true_conref,((_tmpC85->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC85)))))))));}
# 2099
break;}case 201: _LL2EE:
# 2101
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 202: _LL2EF: {
# 2104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2102 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC88;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC87;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87[0]=((_tmpC88.tag=1,((_tmpC88.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC88)))),_tmpC87))))));}
break;}case 203: _LL2F0:
# 2105
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 204: _LL2F1: {
# 2108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2106 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 205: _LL2F2: {
# 2109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2107 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 206: _LL2F3:
# 2110
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 207: _LL2F4: {
# 2113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2113 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 208: _LL2F5: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2114 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 209: _LL2F6:
# 2117
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 210: _LL2F7: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2119 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 211: _LL2F8: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2124 "parse.y"
{struct _tuple26*_tmpC89;yyval=Cyc_YY42(((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC89->f2=0,((_tmpC89->f3=0,((_tmpC89->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC89->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC89)))))))))))));}
break;}case 212: _LL2F9: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2126 "parse.y"
{struct _tuple26*_tmpC8A;yyval=Cyc_YY42(((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC8A->f2=1,((_tmpC8A->f3=0,((_tmpC8A->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC8A->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC8A)))))))))))));}
break;}case 213: _LL2FA: {
# 2129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2129 "parse.y"
struct _tuple8*_tmp65B=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp65D;struct Cyc_Absyn_Tqual _tmp65E;void*_tmp65F;struct _tuple8*_tmp65C=_tmp65B;_tmp65D=_tmp65C->f1;_tmp65E=_tmp65C->f2;_tmp65F=_tmp65C->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC8B;struct Cyc_Absyn_VarargInfo*_tmp660=(_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->name=_tmp65D,((_tmpC8B->tq=_tmp65E,((_tmpC8B->type=_tmp65F,((_tmpC8B->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC8B)))))))));
{struct _tuple26*_tmpC8C;yyval=Cyc_YY42(((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->f1=0,((_tmpC8C->f2=0,((_tmpC8C->f3=_tmp660,((_tmpC8C->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC8C->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC8C)))))))))))));}
# 2133
break;};}case 214: _LL2FB: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2135 "parse.y"
struct _tuple8*_tmp663=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp665;struct Cyc_Absyn_Tqual _tmp666;void*_tmp667;struct _tuple8*_tmp664=_tmp663;_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;_tmp667=_tmp664->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC8D;struct Cyc_Absyn_VarargInfo*_tmp668=(_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->name=_tmp665,((_tmpC8D->tq=_tmp666,((_tmpC8D->type=_tmp667,((_tmpC8D->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC8D)))))))));
{struct _tuple26*_tmpC8E;yyval=Cyc_YY42(((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC8E->f2=0,((_tmpC8E->f3=_tmp668,((_tmpC8E->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC8E->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC8E)))))))))))));}
# 2139
break;};}case 215: _LL2FC: {
# 2141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2143 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC91;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC90;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90[0]=((_tmpC91.tag=1,((_tmpC91.f1=0,_tmpC91)))),_tmpC90))))));}
break;}case 216: _LL2FD: {
# 2146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2144 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 217: _LL2FE:
# 2147
 yyval=Cyc_YY52(0);
break;case 218: _LL2FF: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2148 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC94;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC93;yyval=Cyc_YY52((void*)((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC94.tag=24,((_tmpC94.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC94)))),_tmpC93)))));}
break;}case 219: _LL300:
# 2151
 yyval=Cyc_YY53(0);
break;case 220: _LL301: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2153 "parse.y"
yyval=yyyyvsp[1];
break;}case 221: _LL302: {
# 2156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2161 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC97;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC96;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66F=(_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=2,((_tmpC97.f1=0,((_tmpC97.f2=& Cyc_Tcutil_trk,_tmpC97)))))),_tmpC96)));
struct _dyneither_ptr _tmp670=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp671=Cyc_Parse_id2type(_tmp670,(void*)_tmp66F);
{struct _tuple29*_tmpCA1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCA0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC9F;struct Cyc_List_List*_tmpC9E;yyval=Cyc_YY53(((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->hd=((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=(void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=24,((_tmpCA0.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCA0)))),_tmpC9F)))),((_tmpCA1->f2=_tmp671,_tmpCA1)))))),((_tmpC9E->tl=0,_tmpC9E)))))));}
# 2166
break;}case 222: _LL303: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2168 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCA4;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCA3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp678=(_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3[0]=((_tmpCA4.tag=2,((_tmpCA4.f1=0,((_tmpCA4.f2=& Cyc_Tcutil_trk,_tmpCA4)))))),_tmpCA3)));
struct _dyneither_ptr _tmp679=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp67A=Cyc_Parse_id2type(_tmp679,(void*)_tmp678);
{struct _tuple29*_tmpCAE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCAD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCAC;struct Cyc_List_List*_tmpCAB;yyval=Cyc_YY53(((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->f1=(void*)((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC[0]=((_tmpCAD.tag=24,((_tmpCAD.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCAD)))),_tmpCAC)))),((_tmpCAE->f2=_tmp67A,_tmpCAE)))))),((_tmpCAB->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCAB)))))));}
# 2173
break;}case 223: _LL304:
# 2175
 yyval=Cyc_YY34(0);
break;case 224: _LL305: {
# 2178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2179 "parse.y"
{const char*_tmpCAF;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCAF="inject",_tag_dyneither(_tmpCAF,sizeof(char),7))))!= 0){
const char*_tmpCB0;Cyc_Parse_err(((_tmpCB0="missing type in function declaration",_tag_dyneither(_tmpCB0,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2183
break;}case 225: _LL306: {
# 2185
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2187
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2186 "parse.y"
yyval=yyyyvsp[0];
break;}case 226: _LL307: {
# 2189
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2191
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2187 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 227: _LL308: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2191 "parse.y"
yyval=Cyc_YY43(0);
break;}case 228: _LL309: {
# 2194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2192 "parse.y"
yyval=yyyyvsp[1];
break;}case 229: _LL30A: {
# 2195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2194 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCB6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCB5;struct Cyc_List_List*_tmpCB4;yyval=Cyc_YY43(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=(void*)((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB5.tag=25,((_tmpCB5.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCB5)))),_tmpCB6)))),((_tmpCB4->tl=0,_tmpCB4)))))));}
break;}case 230: _LL30B: {
# 2197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2196 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCB7;yyval=Cyc_YY43(((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCB7->tl=0,_tmpCB7)))))));}
# 2199
break;}case 231: _LL30C: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCBD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCBC;struct Cyc_List_List*_tmpCBB;yyval=Cyc_YY43(((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->hd=(void*)((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBC.tag=23,((_tmpCBC.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpCBC)))),_tmpCBD)))),((_tmpCBB->tl=0,_tmpCBB)))))));}
# 2207
break;}case 232: _LL30D: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2208 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCC3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCC2;struct Cyc_List_List*_tmpCC1;yyval=Cyc_YY43(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=(void*)((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3[0]=((_tmpCC2.tag=23,((_tmpCC2.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpCC2)))),_tmpCC3)))),((_tmpCC1->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCC1)))))));}
# 2211
break;}case 233: _LL30E: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCC4;yyval=Cyc_YY41(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpCC4->tl=0,_tmpCC4)))))));}
break;}case 234: _LL30F: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2218 "parse.y"
{struct Cyc_List_List*_tmpCC5;yyval=Cyc_YY41(((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpCC5->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCC5)))))));}
break;}case 235: _LL310: {
# 2221
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp68F=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp691;struct Cyc_Parse_Type_specifier _tmp692;struct Cyc_List_List*_tmp693;struct _tuple25 _tmp690=_tmp68F;_tmp691=_tmp690.f1;_tmp692=_tmp690.f2;_tmp693=_tmp690.f3;
if(_tmp691.loc == 0)_tmp691.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp694=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp696;struct Cyc_List_List*_tmp697;struct Cyc_Parse_Declarator _tmp695=_tmp694;_tmp696=_tmp695.id;_tmp697=_tmp695.tms;{
void*_tmp698=Cyc_Parse_speclist2typ(_tmp692,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp699=Cyc_Parse_apply_tms(_tmp691,_tmp698,_tmp693,_tmp697);struct Cyc_Absyn_Tqual _tmp69B;void*_tmp69C;struct Cyc_List_List*_tmp69D;struct Cyc_List_List*_tmp69E;struct _tuple13 _tmp69A=_tmp699;_tmp69B=_tmp69A.f1;_tmp69C=_tmp69A.f2;_tmp69D=_tmp69A.f3;_tmp69E=_tmp69A.f4;
if(_tmp69D != 0){
const char*_tmpCC6;Cyc_Parse_err(((_tmpCC6="parameter with bad type params",_tag_dyneither(_tmpCC6,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp696)){
const char*_tmpCC7;Cyc_Parse_err(((_tmpCC7="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCC7,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp696).f2;
if(_tmp69E != 0){
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCA="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCCA,sizeof(char),40))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}
{struct _tuple8*_tmpCCB;yyval=Cyc_YY40(((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->f1=idopt,((_tmpCCB->f2=_tmp69B,((_tmpCCB->f3=_tmp69C,_tmpCCB)))))))));}
# 2238
break;};};};}case 236: _LL311: {
# 2240
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2242
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2239 "parse.y"
struct _tuple25 _tmp6A4=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6A6;struct Cyc_Parse_Type_specifier _tmp6A7;struct Cyc_List_List*_tmp6A8;struct _tuple25 _tmp6A5=_tmp6A4;_tmp6A6=_tmp6A5.f1;_tmp6A7=_tmp6A5.f2;_tmp6A8=_tmp6A5.f3;
if(_tmp6A6.loc == 0)_tmp6A6.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A9=Cyc_Parse_speclist2typ(_tmp6A7,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6A8 != 0){
const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCE="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCCE,sizeof(char),38))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}
{struct _tuple8*_tmpCCF;yyval=Cyc_YY40(((_tmpCCF=_cycalloc(sizeof(*_tmpCCF)),((_tmpCCF->f1=0,((_tmpCCF->f2=_tmp6A6,((_tmpCCF->f3=_tmp6A9,_tmpCCF)))))))));}
# 2246
break;};}case 237: _LL312: {
# 2248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2247 "parse.y"
struct _tuple25 _tmp6AD=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6AF;struct Cyc_Parse_Type_specifier _tmp6B0;struct Cyc_List_List*_tmp6B1;struct _tuple25 _tmp6AE=_tmp6AD;_tmp6AF=_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;_tmp6B1=_tmp6AE.f3;
if(_tmp6AF.loc == 0)_tmp6AF.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6B2=Cyc_Parse_speclist2typ(_tmp6B0,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6B3=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6B4=Cyc_Parse_apply_tms(_tmp6AF,_tmp6B2,_tmp6B1,_tmp6B3);struct Cyc_Absyn_Tqual _tmp6B6;void*_tmp6B7;struct Cyc_List_List*_tmp6B8;struct Cyc_List_List*_tmp6B9;struct _tuple13 _tmp6B5=_tmp6B4;_tmp6B6=_tmp6B5.f1;_tmp6B7=_tmp6B5.f2;_tmp6B8=_tmp6B5.f3;_tmp6B9=_tmp6B5.f4;
if(_tmp6B8 != 0){
const char*_tmpCD2;void*_tmpCD1;(_tmpCD1=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCD2="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCD2,sizeof(char),49))),_tag_dyneither(_tmpCD1,sizeof(void*),0)));}
# 2255
if(_tmp6B9 != 0){
const char*_tmpCD5;void*_tmpCD4;(_tmpCD4=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCD5="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCD5,sizeof(char),38))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}
{struct _tuple8*_tmpCD6;yyval=Cyc_YY40(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=0,((_tmpCD6->f2=_tmp6B6,((_tmpCD6->f3=_tmp6B7,_tmpCD6)))))))));}
# 2259
break;};}case 238: _LL313: {
# 2261
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2263 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 239: _LL314: {
# 2266
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2268
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2267 "parse.y"
{struct _dyneither_ptr*_tmpCD9;struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY39(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCD9)))),((_tmpCD8->tl=0,_tmpCD8)))))));}
break;}case 240: _LL315: {
# 2270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2269 "parse.y"
{struct _dyneither_ptr*_tmpCDC;struct Cyc_List_List*_tmpCDB;yyval=Cyc_YY39(((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->hd=((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCDC)))),((_tmpCDB->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCDB)))))));}
break;}case 241: _LL316: {
# 2272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2273 "parse.y"
yyval=yyyyvsp[0];
break;}case 242: _LL317: {
# 2276
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2278
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2274 "parse.y"
yyval=yyyyvsp[0];
break;}case 243: _LL318: {
# 2277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2279 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCDF;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCDE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDF.tag=35,((_tmpCDF.f1=0,((_tmpCDF.f2=0,_tmpCDF)))))),_tmpCDE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL319: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2281 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCE2;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCE1;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=35,((_tmpCE2.f1=0,((_tmpCE2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCE2)))))),_tmpCE1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31A: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2283 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCE5;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCE4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=((_tmpCE5.tag=35,((_tmpCE5.f1=0,((_tmpCE5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCE5)))))),_tmpCE4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL31B: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2285 "parse.y"
struct _dyneither_ptr*_tmpCE8;struct _tuple0*_tmpCE7;struct Cyc_Absyn_Vardecl*_tmp6C9=Cyc_Absyn_new_vardecl(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->f1=Cyc_Absyn_Loc_n,((_tmpCE7->f2=((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCE8)))),_tmpCE7)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2288
(_tmp6C9->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCEB;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCEA;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA[0]=((_tmpCEB.tag=26,((_tmpCEB.f1=_tmp6C9,((_tmpCEB.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCEB.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCEB.f4=0,_tmpCEB)))))))))),_tmpCEA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2291
break;}case 247: _LL31C: {
# 2293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2292 "parse.y"
void*_tmp6CE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCEE;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCED;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=27,((_tmpCEE.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCEE.f2=_tmp6CE,((_tmpCEE.f3=0,_tmpCEE)))))))),_tmpCED)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2295
break;}case 248: _LL31D: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpCF1;struct Cyc_List_List*_tmpCF0;yyval=Cyc_YY7(((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->hd=((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=0,((_tmpCF1->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCF1)))))),((_tmpCF0->tl=0,_tmpCF0)))))));}
break;}case 249: _LL31E: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2302 "parse.y"
{struct _tuple30*_tmpCF4;struct Cyc_List_List*_tmpCF3;yyval=Cyc_YY7(((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->hd=((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCF4->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCF4)))))),((_tmpCF3->tl=0,_tmpCF3)))))));}
break;}case 250: _LL31F: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2304 "parse.y"
{struct _tuple30*_tmpCF7;struct Cyc_List_List*_tmpCF6;yyval=Cyc_YY7(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->f1=0,((_tmpCF7->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCF7)))))),((_tmpCF6->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCF6)))))));}
break;}case 251: _LL320: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2306 "parse.y"
{struct _tuple30*_tmpCFA;struct Cyc_List_List*_tmpCF9;yyval=Cyc_YY7(((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9->hd=((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCFA->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCFA)))))),((_tmpCF9->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCF9)))))));}
break;}case 252: _LL321: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2310 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 253: _LL322: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2315 "parse.y"
{struct Cyc_List_List*_tmpCFB;yyval=Cyc_YY44(((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCFB->tl=0,_tmpCFB)))))));}
break;}case 254: _LL323: {
# 2318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2316 "parse.y"
{struct Cyc_List_List*_tmpCFC;yyval=Cyc_YY44(((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCFC->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCFC)))))));}
break;}case 255: _LL324: {
# 2319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2320 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCFF;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCFE;yyval=Cyc_YY45((void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=0,((_tmpCFF.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCFF)))),_tmpCFE)))));}
break;}case 256: _LL325: {
# 2323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2321 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD05;struct _dyneither_ptr*_tmpD04;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD03;yyval=Cyc_YY45((void*)((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03[0]=((_tmpD05.tag=1,((_tmpD05.f1=((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD04)))),_tmpD05)))),_tmpD03)))));}
break;}case 257: _LL326: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2326 "parse.y"
struct _tuple25 _tmp6E0=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6E2;struct Cyc_Parse_Type_specifier _tmp6E3;struct Cyc_List_List*_tmp6E4;struct _tuple25 _tmp6E1=_tmp6E0;_tmp6E2=_tmp6E1.f1;_tmp6E3=_tmp6E1.f2;_tmp6E4=_tmp6E1.f3;{
void*_tmp6E5=Cyc_Parse_speclist2typ(_tmp6E3,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6E4 != 0){
const char*_tmpD08;void*_tmpD07;(_tmpD07=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD08="ignoring attributes in type",_tag_dyneither(_tmpD08,sizeof(char),28))),_tag_dyneither(_tmpD07,sizeof(void*),0)));}
{struct _tuple8*_tmpD09;yyval=Cyc_YY40(((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->f1=0,((_tmpD09->f2=_tmp6E2,((_tmpD09->f3=_tmp6E5,_tmpD09)))))))));}
# 2332
break;};}case 258: _LL327: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2333 "parse.y"
struct _tuple25 _tmp6E9=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6EB;struct Cyc_Parse_Type_specifier _tmp6EC;struct Cyc_List_List*_tmp6ED;struct _tuple25 _tmp6EA=_tmp6E9;_tmp6EB=_tmp6EA.f1;_tmp6EC=_tmp6EA.f2;_tmp6ED=_tmp6EA.f3;{
void*_tmp6EE=Cyc_Parse_speclist2typ(_tmp6EC,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6EF=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6F0=Cyc_Parse_apply_tms(_tmp6EB,_tmp6EE,_tmp6ED,_tmp6EF);
if(_tmp6F0.f3 != 0){
# 2339
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD0C="bad type params, ignoring",_tag_dyneither(_tmpD0C,sizeof(char),26))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}
if(_tmp6F0.f4 != 0){
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD0F="bad specifiers, ignoring",_tag_dyneither(_tmpD0F,sizeof(char),25))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}
{struct _tuple8*_tmpD10;yyval=Cyc_YY40(((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->f1=0,((_tmpD10->f2=_tmp6F0.f1,((_tmpD10->f3=_tmp6F0.f2,_tmpD10)))))))));}
# 2344
break;};}case 259: _LL328: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2347 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 260: _LL329: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2348 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD13;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD12;yyval=Cyc_YY47((void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD13.tag=24,((_tmpD13.f1=0,_tmpD13)))),_tmpD12)))));}
break;}case 261: _LL32A: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2349 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD16;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD15;yyval=Cyc_YY47((void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD16.tag=24,((_tmpD16.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpD16)))),_tmpD15)))));}
break;}case 262: _LL32B: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2350 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD19;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD18;yyval=Cyc_YY47((void*)((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18[0]=((_tmpD19.tag=25,((_tmpD19.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpD19)))),_tmpD18)))));}
break;}case 263: _LL32C: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2351 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD1F;struct Cyc_List_List*_tmpD1E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD1D;yyval=Cyc_YY47((void*)((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D[0]=((_tmpD1F.tag=24,((_tmpD1F.f1=((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD1E->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpD1E)))))),_tmpD1F)))),_tmpD1D)))));}
break;}case 264: _LL32D: {
# 2354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD20;yyval=Cyc_YY43(((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD20->tl=0,_tmpD20)))))));}
break;}case 265: _LL32E: {
# 2360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2358 "parse.y"
{struct Cyc_List_List*_tmpD21;yyval=Cyc_YY43(((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpD21->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpD21)))))));}
break;}case 266: _LL32F: {
# 2361
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2363
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2363 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD22;yyval=Cyc_YY33(((_tmpD22.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpD22)));}
break;}case 267: _LL330: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2365 "parse.y"
yyval=yyyyvsp[0];
break;}case 268: _LL331: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2367 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD23;yyval=Cyc_YY33(((_tmpD23.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpD23)));}
break;}case 269: _LL332: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2372 "parse.y"
yyval=yyyyvsp[1];
break;}case 270: _LL333: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2374 "parse.y"
{struct Cyc_List_List*_tmpD2D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD2C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2B;struct Cyc_Parse_Abstractdeclarator _tmpD2A;yyval=Cyc_YY33(((_tmpD2A.tms=((_tmpD2D=_region_malloc(yyr,sizeof(*_tmpD2D)),((_tmpD2D->hd=(void*)((_tmpD2B=_region_malloc(yyr,sizeof(*_tmpD2B)),((_tmpD2B[0]=((_tmpD2C.tag=0,((_tmpD2C.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD2C.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD2C)))))),_tmpD2B)))),((_tmpD2D->tl=0,_tmpD2D)))))),_tmpD2A)));}
break;}case 271: _LL334: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376 "parse.y"
{struct Cyc_List_List*_tmpD37;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD36;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD35;struct Cyc_Parse_Abstractdeclarator _tmpD34;yyval=Cyc_YY33(((_tmpD34.tms=((_tmpD37=_region_malloc(yyr,sizeof(*_tmpD37)),((_tmpD37->hd=(void*)((_tmpD35=_region_malloc(yyr,sizeof(*_tmpD35)),((_tmpD35[0]=((_tmpD36.tag=0,((_tmpD36.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD36.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD36)))))),_tmpD35)))),((_tmpD37->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD37)))))),_tmpD34)));}
break;}case 272: _LL335: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2378 "parse.y"
{struct Cyc_List_List*_tmpD41;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD40;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3F;struct Cyc_Parse_Abstractdeclarator _tmpD3E;yyval=Cyc_YY33(((_tmpD3E.tms=((_tmpD41=_region_malloc(yyr,sizeof(*_tmpD41)),((_tmpD41->hd=(void*)((_tmpD3F=_region_malloc(yyr,sizeof(*_tmpD3F)),((_tmpD3F[0]=((_tmpD40.tag=1,((_tmpD40.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD40.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD40.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD40)))))))),_tmpD3F)))),((_tmpD41->tl=0,_tmpD41)))))),_tmpD3E)));}
break;}case 273: _LL336: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2380 "parse.y"
{struct Cyc_List_List*_tmpD4B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD4A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD49;struct Cyc_Parse_Abstractdeclarator _tmpD48;yyval=Cyc_YY33(((_tmpD48.tms=((_tmpD4B=_region_malloc(yyr,sizeof(*_tmpD4B)),((_tmpD4B->hd=(void*)((_tmpD49=_region_malloc(yyr,sizeof(*_tmpD49)),((_tmpD49[0]=((_tmpD4A.tag=1,((_tmpD4A.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD4A.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD4A.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD4A)))))))),_tmpD49)))),((_tmpD4B->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD4B)))))),_tmpD48)));}
# 2383
break;}case 274: _LL337: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2384 "parse.y"
{struct Cyc_List_List*_tmpD60;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD5F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD5E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5C;struct Cyc_Parse_Abstractdeclarator _tmpD5B;yyval=Cyc_YY33(((_tmpD5B.tms=((_tmpD60=_region_malloc(yyr,sizeof(*_tmpD60)),((_tmpD60->hd=(void*)((_tmpD5C=_region_malloc(yyr,sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5F.tag=3,((_tmpD5F.f1=(void*)((_tmpD5D=_region_malloc(yyr,sizeof(*_tmpD5D)),((_tmpD5D[0]=((_tmpD5E.tag=1,((_tmpD5E.f1=0,((_tmpD5E.f2=0,((_tmpD5E.f3=0,((_tmpD5E.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD5E.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD5E)))))))))))),_tmpD5D)))),_tmpD5F)))),_tmpD5C)))),((_tmpD60->tl=0,_tmpD60)))))),_tmpD5B)));}
# 2386
break;}case 275: _LL338: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2387 "parse.y"
struct _tuple26*_tmp719=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp71B;int _tmp71C;struct Cyc_Absyn_VarargInfo*_tmp71D;void*_tmp71E;struct Cyc_List_List*_tmp71F;struct _tuple26*_tmp71A=_tmp719;_tmp71B=_tmp71A->f1;_tmp71C=_tmp71A->f2;_tmp71D=_tmp71A->f3;_tmp71E=_tmp71A->f4;_tmp71F=_tmp71A->f5;
{struct Cyc_List_List*_tmpD75;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD74;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD73;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD72;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD71;struct Cyc_Parse_Abstractdeclarator _tmpD70;yyval=Cyc_YY33(((_tmpD70.tms=((_tmpD75=_region_malloc(yyr,sizeof(*_tmpD75)),((_tmpD75->hd=(void*)((_tmpD71=_region_malloc(yyr,sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD74.tag=3,((_tmpD74.f1=(void*)((_tmpD72=_region_malloc(yyr,sizeof(*_tmpD72)),((_tmpD72[0]=((_tmpD73.tag=1,((_tmpD73.f1=_tmp71B,((_tmpD73.f2=_tmp71C,((_tmpD73.f3=_tmp71D,((_tmpD73.f4=_tmp71E,((_tmpD73.f5=_tmp71F,_tmpD73)))))))))))),_tmpD72)))),_tmpD74)))),_tmpD71)))),((_tmpD75->tl=0,_tmpD75)))))),_tmpD70)));}
# 2390
break;}case 276: _LL339: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2391 "parse.y"
{struct Cyc_List_List*_tmpD8A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD89;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD88;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD87;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD86;struct Cyc_Parse_Abstractdeclarator _tmpD85;yyval=Cyc_YY33(((_tmpD85.tms=((_tmpD8A=_region_malloc(yyr,sizeof(*_tmpD8A)),((_tmpD8A->hd=(void*)((_tmpD86=_region_malloc(yyr,sizeof(*_tmpD86)),((_tmpD86[0]=((_tmpD89.tag=3,((_tmpD89.f1=(void*)((_tmpD87=_region_malloc(yyr,sizeof(*_tmpD87)),((_tmpD87[0]=((_tmpD88.tag=1,((_tmpD88.f1=0,((_tmpD88.f2=0,((_tmpD88.f3=0,((_tmpD88.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD88.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD88)))))))))))),_tmpD87)))),_tmpD89)))),_tmpD86)))),((_tmpD8A->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD8A)))))),_tmpD85)));}
# 2394
break;}case 277: _LL33A: {
# 2396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2395 "parse.y"
struct _tuple26*_tmp72C=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp72E;int _tmp72F;struct Cyc_Absyn_VarargInfo*_tmp730;void*_tmp731;struct Cyc_List_List*_tmp732;struct _tuple26*_tmp72D=_tmp72C;_tmp72E=_tmp72D->f1;_tmp72F=_tmp72D->f2;_tmp730=_tmp72D->f3;_tmp731=_tmp72D->f4;_tmp732=_tmp72D->f5;
{struct Cyc_List_List*_tmpD9F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD9E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD9D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD9C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD9B;struct Cyc_Parse_Abstractdeclarator _tmpD9A;yyval=Cyc_YY33(((_tmpD9A.tms=((_tmpD9F=_region_malloc(yyr,sizeof(*_tmpD9F)),((_tmpD9F->hd=(void*)((_tmpD9B=_region_malloc(yyr,sizeof(*_tmpD9B)),((_tmpD9B[0]=((_tmpD9E.tag=3,((_tmpD9E.f1=(void*)((_tmpD9C=_region_malloc(yyr,sizeof(*_tmpD9C)),((_tmpD9C[0]=((_tmpD9D.tag=1,((_tmpD9D.f1=_tmp72E,((_tmpD9D.f2=_tmp72F,((_tmpD9D.f3=_tmp730,((_tmpD9D.f4=_tmp731,((_tmpD9D.f5=_tmp732,_tmpD9D)))))))))))),_tmpD9C)))),_tmpD9E)))),_tmpD9B)))),((_tmpD9F->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD9F)))))),_tmpD9A)));}
# 2399
break;}case 278: _LL33B: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2401 "parse.y"
struct Cyc_List_List*_tmp739=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDA9;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDA8;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDA7;struct Cyc_Parse_Abstractdeclarator _tmpDA6;yyval=Cyc_YY33(((_tmpDA6.tms=((_tmpDA9=_region_malloc(yyr,sizeof(*_tmpDA9)),((_tmpDA9->hd=(void*)((_tmpDA7=_region_malloc(yyr,sizeof(*_tmpDA7)),((_tmpDA7[0]=((_tmpDA8.tag=4,((_tmpDA8.f1=_tmp739,((_tmpDA8.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDA8.f3=0,_tmpDA8)))))))),_tmpDA7)))),((_tmpDA9->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDA9)))))),_tmpDA6)));}
# 2405
break;}case 279: _LL33C: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2406 "parse.y"
{struct Cyc_List_List*_tmpDB3;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDB2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDB1;struct Cyc_Parse_Abstractdeclarator _tmpDB0;yyval=Cyc_YY33(((_tmpDB0.tms=((_tmpDB3=_region_malloc(yyr,sizeof(*_tmpDB3)),((_tmpDB3->hd=(void*)((_tmpDB1=_region_malloc(yyr,sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB2.tag=5,((_tmpDB2.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDB2.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpDB2)))))),_tmpDB1)))),((_tmpDB3->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDB3)))))),_tmpDB0)));}
# 2408
break;}case 280: _LL33D: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2412 "parse.y"
yyval=yyyyvsp[0];
break;}case 281: _LL33E: {
# 2415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2413 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL33F: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2414 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL340: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2415 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL341: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2416 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL342: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL343: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2420 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDB6;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDB5;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5[0]=((_tmpDB6.tag=16,((_tmpDB6.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpDB6)))),_tmpDB5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL344:
# 2423
 yyval=Cyc_YY5(0);
break;case 288: _LL345: {
# 2426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2426 "parse.y"
{const char*_tmpDB7;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDB7="open",_tag_dyneither(_tmpDB7,sizeof(char),5))))!= 0){
const char*_tmpDB8;Cyc_Parse_err(((_tmpDB8="expecting `open'",_tag_dyneither(_tmpDB8,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2430
break;}case 289: _LL346: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDBE;struct _dyneither_ptr*_tmpDBD;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDBC;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBE.tag=13,((_tmpDBE.f1=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDBD)))),((_tmpDBE.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpDBE)))))),_tmpDBC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 290: _LL347: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2439 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 291: _LL348: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2440 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL349: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2445 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34A: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2446 "parse.y"
yyval=yyyyvsp[1];
break;}case 294: _LL34B: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2451 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 295: _LL34C: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2452 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 296: _LL34D: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2453 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDC4;struct _dyneither_ptr*_tmpDC3;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDC2;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC4.tag=13,((_tmpDC4.f1=((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDC3)))),((_tmpDC4.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDC4)))))),_tmpDC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2455
break;}case 297: _LL34E: {
# 2457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2455 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDCA;struct _dyneither_ptr*_tmpDC9;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDC8;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDCA.tag=13,((_tmpDCA.f1=((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDC9)))),((_tmpDCA.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDCA)))))),_tmpDC8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2457
break;}case 298: _LL34F: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2457 "parse.y"
yyval=yyyyvsp[0];
break;}case 299: _LL350: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2458 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL351: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2459 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDCD;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDCC;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=((_tmpDCD.tag=1,((_tmpDCD.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDCD)))),_tmpDCC)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 301: _LL352: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2462 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDD0;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDCF;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF[0]=((_tmpDD0.tag=1,((_tmpDD0.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDD0)))),_tmpDCF)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 302: _LL353: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2467 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL354: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2469 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL355: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL356: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2478 "parse.y"
struct Cyc_Absyn_Exp*_tmp753=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp753,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL357: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2482 "parse.y"
struct Cyc_Absyn_Exp*_tmp754=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp754,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2485
break;}case 307: _LL358: {
# 2487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2488 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL359:
# 2491
 yyval=Cyc_YY11(0);
break;case 309: _LL35A: {
# 2494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2504 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD3;struct Cyc_List_List*_tmpDD2;yyval=Cyc_YY11(((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->hd=((_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDD3->pat_vars=0,((_tmpDD3->where_clause=0,((_tmpDD3->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDD3->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD3)))))))))))),((_tmpDD2->tl=0,_tmpDD2)))))));}
# 2507
break;}case 310: _LL35B: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2508 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD6;struct Cyc_List_List*_tmpDD5;yyval=Cyc_YY11(((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->hd=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDD6->pat_vars=0,((_tmpDD6->where_clause=0,((_tmpDD6->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDD6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD6)))))))))))),((_tmpDD5->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDD5)))))));}
break;}case 311: _LL35C: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2512 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD9;struct Cyc_List_List*_tmpDD8;yyval=Cyc_YY11(((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->hd=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDD9->pat_vars=0,((_tmpDD9->where_clause=0,((_tmpDD9->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDD9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD9)))))))))))),((_tmpDD8->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDD8)))))));}
break;}case 312: _LL35D: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2514 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDDC;struct Cyc_List_List*_tmpDDB;yyval=Cyc_YY11(((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->hd=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDDC->pat_vars=0,((_tmpDDC->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDDC->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDDC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDDC)))))))))))),((_tmpDDB->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDDB)))))));}
break;}case 313: _LL35E: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2518 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDDF;struct Cyc_List_List*_tmpDDE;yyval=Cyc_YY11(((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE->hd=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDDF->pat_vars=0,((_tmpDDF->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDDF->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDDF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDDF)))))))))))),((_tmpDDE->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDDE)))))));}
break;}case 314: _LL35F: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2525 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL360: {
# 2528
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2529 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL361: {
# 2532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2533 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL362: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2536 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL363: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2539 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL364: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2542 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL365: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2545 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL366: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2548 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL367: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2551 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL368: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2554 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL369: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2557 "parse.y"
struct Cyc_List_List*_tmp75F=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp760=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp75F,_tmp760));
# 2562
break;}case 325: _LL36A: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2563 "parse.y"
struct Cyc_List_List*_tmp761=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp762=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp761,_tmp762));
# 2568
break;}case 326: _LL36B: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2569 "parse.y"
struct Cyc_List_List*_tmp763=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp764=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp763,_tmp764));
# 2574
break;}case 327: _LL36C: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2575 "parse.y"
struct Cyc_List_List*_tmp765=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp766=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp765,_tmp766));
# 2580
break;}case 328: _LL36D: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDE0;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDE0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 329: _LL36E: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2586 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL36F: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2587 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL370: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2588 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL371: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2589 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL372: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2591 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL373: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2592 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL374: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2594 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL375: {
# 2597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2603 "parse.y"
yyval=yyyyvsp[0];
break;}case 337: _LL376: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2606 "parse.y"
yyval=yyyyvsp[0];
break;}case 338: _LL377: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2608 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 339: _LL378: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2611 "parse.y"
yyval=yyyyvsp[0];
break;}case 340: _LL379: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2613 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 341: _LL37A: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2616 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL37B: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2618 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL37C: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2621 "parse.y"
yyval=yyyyvsp[0];
break;}case 344: _LL37D: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2623 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL37E: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2626 "parse.y"
yyval=yyyyvsp[0];
break;}case 346: _LL37F: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2628 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL380: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2631 "parse.y"
yyval=yyyyvsp[0];
break;}case 348: _LL381: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2633 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL382: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2636 "parse.y"
yyval=yyyyvsp[0];
break;}case 350: _LL383: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2638 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL384: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2640 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL385: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2643 "parse.y"
yyval=yyyyvsp[0];
break;}case 353: _LL386: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2645 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL387: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2647 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL388: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2649 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL389: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2651 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38A: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2654 "parse.y"
yyval=yyyyvsp[0];
break;}case 358: _LL38B: {
# 2657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2656 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL38C: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2658 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL38D: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2661 "parse.y"
yyval=yyyyvsp[0];
break;}case 361: _LL38E: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2663 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL38F: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2665 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL390: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2668 "parse.y"
yyval=yyyyvsp[0];
break;}case 364: _LL391: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2670 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL392: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2672 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL393: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2674 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL394: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2677 "parse.y"
yyval=yyyyvsp[0];
break;}case 368: _LL395: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2679 "parse.y"
void*_tmp768=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp768,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2682
break;}case 369: _LL396: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2685 "parse.y"
yyval=yyyyvsp[0];
break;}case 370: _LL397: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2688 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 371: _LL398: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2690 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 372: _LL399: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2692 "parse.y"
void*_tmp769=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp769,(unsigned int)(yyyylsp[0]).first_line)));
# 2695
break;}case 373: _LL39A: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2696 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 374: _LL39B: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2698 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL39C: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2703 "parse.y"
yyval=yyyyvsp[0];
break;}case 376: _LL39D: {
# 2706
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2708
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2712 "parse.y"
yyval=yyyyvsp[0];
break;}case 377: _LL39E: {
# 2715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2717 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 378: _LL39F: {
# 2720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2719 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 379: _LL3A0: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2721 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp76A=e->r;void*_tmp76B=_tmp76A;enum Cyc_Absyn_Sign _tmp76D;char _tmp76E;enum Cyc_Absyn_Sign _tmp770;short _tmp771;enum Cyc_Absyn_Sign _tmp773;int _tmp774;struct _dyneither_ptr _tmp776;int _tmp777;_LL3A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp76C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp76C->tag != 0)goto _LL3A5;else{if(((_tmp76C->f1).Char_c).tag != 2)goto _LL3A5;_tmp76D=((struct _tuple3)((_tmp76C->f1).Char_c).val).f1;_tmp76E=((struct _tuple3)((_tmp76C->f1).Char_c).val).f2;}}_LL3A4:
# 2725
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDE3;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDE2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE2=_cycalloc_atomic(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=11,((_tmpDE3.f1=_tmp76E,_tmpDE3)))),_tmpDE2)))),e->loc));}goto _LL3A2;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp76F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp76F->tag != 0)goto _LL3A7;else{if(((_tmp76F->f1).Short_c).tag != 4)goto _LL3A7;_tmp770=((struct _tuple4)((_tmp76F->f1).Short_c).val).f1;_tmp771=((struct _tuple4)((_tmp76F->f1).Short_c).val).f2;}}_LL3A6:
# 2727
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDE6;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDE5;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE5=_cycalloc_atomic(sizeof(*_tmpDE5)),((_tmpDE5[0]=((_tmpDE6.tag=10,((_tmpDE6.f1=_tmp770,((_tmpDE6.f2=(int)_tmp771,_tmpDE6)))))),_tmpDE5)))),e->loc));}goto _LL3A2;_LL3A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp772=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp772->tag != 0)goto _LL3A9;else{if(((_tmp772->f1).Int_c).tag != 5)goto _LL3A9;_tmp773=((struct _tuple5)((_tmp772->f1).Int_c).val).f1;_tmp774=((struct _tuple5)((_tmp772->f1).Int_c).val).f2;}}_LL3A8:
# 2729
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDE9;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDE8;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE8=_cycalloc_atomic(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE9.tag=10,((_tmpDE9.f1=_tmp773,((_tmpDE9.f2=_tmp774,_tmpDE9)))))),_tmpDE8)))),e->loc));}goto _LL3A2;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp775=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp775->tag != 0)goto _LL3AB;else{if(((_tmp775->f1).Float_c).tag != 7)goto _LL3AB;_tmp776=((struct _tuple7)((_tmp775->f1).Float_c).val).f1;_tmp777=((struct _tuple7)((_tmp775->f1).Float_c).val).f2;}}_LL3AA:
# 2731
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDEC;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDEB;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDEC.tag=12,((_tmpDEC.f1=_tmp776,((_tmpDEC.f2=_tmp777,_tmpDEC)))))),_tmpDEB)))),e->loc));}goto _LL3A2;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp778=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp778->tag != 0)goto _LL3AD;else{if(((_tmp778->f1).Null_c).tag != 1)goto _LL3AD;}}_LL3AC:
# 2733
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A2;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp779=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp779->tag != 0)goto _LL3AF;else{if(((_tmp779->f1).String_c).tag != 8)goto _LL3AF;}}_LL3AE:
# 2735
{const char*_tmpDED;Cyc_Parse_err(((_tmpDED="strings cannot occur within patterns",_tag_dyneither(_tmpDED,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp77A->tag != 0)goto _LL3B1;else{if(((_tmp77A->f1).Wstring_c).tag != 9)goto _LL3B1;}}_LL3B0:
# 2737
{const char*_tmpDEE;Cyc_Parse_err(((_tmpDEE="strings cannot occur within patterns",_tag_dyneither(_tmpDEE,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp76B;if(_tmp77B->tag != 0)goto _LL3B3;else{if(((_tmp77B->f1).LongLong_c).tag != 6)goto _LL3B3;}}_LL3B2:
# 2739
{const char*_tmpDEF;Cyc_Parse_unimp(((_tmpDEF="long long's in patterns",_tag_dyneither(_tmpDEF,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3B3:;_LL3B4: {
# 2741
const char*_tmpDF0;Cyc_Parse_err(((_tmpDF0="bad constant in case",_tag_dyneither(_tmpDF0,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A2:;}
# 2744
break;}case 380: _LL3A1: {
# 2746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2745 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDF3;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDF2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF3.tag=15,((_tmpDF3.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDF3)))),_tmpDF2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 381: _LL3B5: {
# 2748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2747 "parse.y"
{const char*_tmpDF4;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDF4="as",_tag_dyneither(_tmpDF4,sizeof(char),3))))!= 0){
const char*_tmpDF5;Cyc_Parse_err(((_tmpDF5="expecting `as'",_tag_dyneither(_tmpDF5,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDFF;struct _dyneither_ptr*_tmpDFE;struct _tuple0*_tmpDFD;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDFC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=((_tmpDFF.tag=1,((_tmpDFF.f1=Cyc_Absyn_new_vardecl(((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->f1=Cyc_Absyn_Loc_n,((_tmpDFD->f2=((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDFE)))),_tmpDFD)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDFF.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDFF)))))),_tmpDFC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2752
break;}case 382: _LL3B6: {
# 2754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2753 "parse.y"
{const char*_tmpE00;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE00="alias",_tag_dyneither(_tmpE00,sizeof(char),6))))!= 0){
const char*_tmpE01;Cyc_Parse_err(((_tmpE01="expecting `alias'",_tag_dyneither(_tmpE01,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp792=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE09;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE08;struct _dyneither_ptr*_tmpE07;struct Cyc_Absyn_Tvar*_tmpE06;struct Cyc_Absyn_Tvar*tv=(_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->name=((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE07)))),((_tmpE06->identity=- 1,((_tmpE06->kind=(void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE08.tag=0,((_tmpE08.f1=& Cyc_Tcutil_rk,_tmpE08)))),_tmpE09)))),_tmpE06)))))));
struct _dyneither_ptr*_tmpE0C;struct _tuple0*_tmpE0B;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->f1=Cyc_Absyn_Loc_n,((_tmpE0B->f2=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE0C)))),_tmpE0B)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE0F;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE0E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=2,((_tmpE0F.f1=tv,((_tmpE0F.f2=vd,_tmpE0F)))))),_tmpE0E)))),(unsigned int)_tmp792));}
# 2761
break;};}case 383: _LL3B7: {
# 2763
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2765
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2762 "parse.y"
struct _tuple22 _tmp79B=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp79D;int _tmp79E;struct _tuple22 _tmp79C=_tmp79B;_tmp79D=_tmp79C.f1;_tmp79E=_tmp79C.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE12;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE11;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=5,((_tmpE12.f1=_tmp79D,((_tmpE12.f2=_tmp79E,_tmpE12)))))),_tmpE11)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2765
break;}case 384: _LL3B8: {
# 2767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2766 "parse.y"
struct _tuple22 _tmp7A1=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp7A3;int _tmp7A4;struct _tuple22 _tmp7A2=_tmp7A1;_tmp7A3=_tmp7A2.f1;_tmp7A4=_tmp7A2.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE15;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE14;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE15.tag=16,((_tmpE15.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE15.f2=_tmp7A3,((_tmpE15.f3=_tmp7A4,_tmpE15)))))))),_tmpE14)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2769
break;}case 385: _LL3B9: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2770 "parse.y"
struct _tuple22 _tmp7A7=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp7A9;int _tmp7AA;struct _tuple22 _tmp7A8=_tmp7A7;_tmp7A9=_tmp7A8.f1;_tmp7AA=_tmp7A8.f2;{
struct Cyc_List_List*_tmp7AB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE1B;struct Cyc_Absyn_AggrInfo*_tmpE1A;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE19;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1B.tag=7,((_tmpE1B.f1=((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE1A->targs=0,_tmpE1A)))))),((_tmpE1B.f2=_tmp7AB,((_tmpE1B.f3=_tmp7A9,((_tmpE1B.f4=_tmp7AA,_tmpE1B)))))))))),_tmpE19)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2775
break;};}case 386: _LL3BA: {
# 2777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2776 "parse.y"
struct _tuple22 _tmp7AF=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp7B1;int _tmp7B2;struct _tuple22 _tmp7B0=_tmp7AF;_tmp7B1=_tmp7B0.f1;_tmp7B2=_tmp7B0.f2;{
struct Cyc_List_List*_tmp7B3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE1E;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE1D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1E.tag=7,((_tmpE1E.f1=0,((_tmpE1E.f2=_tmp7B3,((_tmpE1E.f3=_tmp7B1,((_tmpE1E.f4=_tmp7B2,_tmpE1E)))))))))),_tmpE1D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2780
break;};}case 387: _LL3BB: {
# 2782
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2781 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE21;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE20;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=6,((_tmpE21.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE21)))),_tmpE20)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3BC: {
# 2784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2783 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE2B;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE2A;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE29;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE28;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE2B.tag=6,((_tmpE2B.f1=Cyc_Absyn_new_pat((void*)((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=6,((_tmpE2A.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE2A)))),_tmpE29)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE2B)))),_tmpE28)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3BD: {
# 2786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2785 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE35;struct _dyneither_ptr*_tmpE34;struct _tuple0*_tmpE33;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE32;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE35.tag=3,((_tmpE35.f1=Cyc_Absyn_new_vardecl(((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->f1=Cyc_Absyn_Loc_n,((_tmpE33->f2=((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE34)))),_tmpE33)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE35.f2=
# 2787
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE35)))))),_tmpE32)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2789
break;}case 390: _LL3BE: {
# 2791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2790 "parse.y"
{const char*_tmpE36;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE36="as",_tag_dyneither(_tmpE36,sizeof(char),3))))!= 0){
const char*_tmpE37;Cyc_Parse_err(((_tmpE37="expecting `as'",_tag_dyneither(_tmpE37,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE41;struct _dyneither_ptr*_tmpE40;struct _tuple0*_tmpE3F;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE3E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E[0]=((_tmpE41.tag=3,((_tmpE41.f1=Cyc_Absyn_new_vardecl(((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->f1=Cyc_Absyn_Loc_n,((_tmpE3F->f2=((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE40)))),_tmpE3F)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE41.f2=
# 2794
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE41)))))),_tmpE3E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2796
break;}case 391: _LL3BF: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2797 "parse.y"
void*_tmp7C6=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE52;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE51;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE50;struct _dyneither_ptr*_tmpE4F;struct _tuple0*_tmpE4E;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE4D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D[0]=((_tmpE52.tag=4,((_tmpE52.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7C6),((_tmpE52.f2=
Cyc_Absyn_new_vardecl(((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E->f1=Cyc_Absyn_Loc_n,((_tmpE4E->f2=((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE4F)))),_tmpE4E)))))),(void*)(
(_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE51.tag=19,((_tmpE51.f1=_tmp7C6,_tmpE51)))),_tmpE50)))),0),_tmpE52)))))),_tmpE4D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2802
break;}case 392: _LL3C0: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2803 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7CD=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE6E;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE6D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE6C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE6B;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE6A;struct _dyneither_ptr*_tmpE69;struct _tuple0*_tmpE68;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE67;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=((_tmpE6E.tag=4,((_tmpE6E.f1=_tmp7CD,((_tmpE6E.f2=
Cyc_Absyn_new_vardecl(((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68->f1=Cyc_Absyn_Loc_n,((_tmpE68->f2=((_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE69)))),_tmpE68)))))),(void*)(
(_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6D.tag=19,((_tmpE6D.f1=(void*)((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=((_tmpE6C.tag=2,((_tmpE6C.f1=_tmp7CD,_tmpE6C)))),_tmpE6B)))),_tmpE6D)))),_tmpE6A)))),0),_tmpE6E)))))),_tmpE67)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2808
break;}case 393: _LL3C1: {
# 2810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2811 "parse.y"
{struct _tuple22*_tmpE6F;yyval=Cyc_YY13(((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE6F->f2=0,_tmpE6F)))))));}
break;}case 394: _LL3C2: {
# 2814
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2812 "parse.y"
{struct _tuple22*_tmpE70;yyval=Cyc_YY13(((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE70->f2=1,_tmpE70)))))));}
break;}case 395: _LL3C3: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2813 "parse.y"
{struct _tuple22*_tmpE71;yyval=Cyc_YY13(((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->f1=0,((_tmpE71->f2=1,_tmpE71)))))));}
break;}case 396: _LL3C4: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2818 "parse.y"
{struct Cyc_List_List*_tmpE72;yyval=Cyc_YY14(((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE72->tl=0,_tmpE72)))))));}
break;}case 397: _LL3C5: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2820 "parse.y"
{struct Cyc_List_List*_tmpE73;yyval=Cyc_YY14(((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE73->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE73)))))));}
break;}case 398: _LL3C6: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2825 "parse.y"
{struct _tuple23*_tmpE74;yyval=Cyc_YY15(((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=0,((_tmpE74->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE74)))))));}
break;}case 399: _LL3C7: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2827 "parse.y"
{struct _tuple23*_tmpE75;yyval=Cyc_YY15(((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE75->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE75)))))));}
break;}case 400: _LL3C8: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2830 "parse.y"
{struct _tuple22*_tmpE76;yyval=Cyc_YY17(((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE76->f2=0,_tmpE76)))))));}
break;}case 401: _LL3C9: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2831 "parse.y"
{struct _tuple22*_tmpE77;yyval=Cyc_YY17(((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE77->f2=1,_tmpE77)))))));}
break;}case 402: _LL3CA: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2832 "parse.y"
{struct _tuple22*_tmpE78;yyval=Cyc_YY17(((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78->f1=0,((_tmpE78->f2=1,_tmpE78)))))));}
break;}case 403: _LL3CB: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2837 "parse.y"
{struct Cyc_List_List*_tmpE79;yyval=Cyc_YY16(((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE79->tl=0,_tmpE79)))))));}
break;}case 404: _LL3CC: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2839 "parse.y"
{struct Cyc_List_List*_tmpE7A;yyval=Cyc_YY16(((_tmpE7A=_cycalloc(sizeof(*_tmpE7A)),((_tmpE7A->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE7A->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE7A)))))));}
break;}case 405: _LL3CD: {
# 2842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 406: _LL3CE: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2847 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 407: _LL3CF: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
yyval=yyyyvsp[0];
break;}case 408: _LL3D0: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2854 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3D1: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2856 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3D2: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2860 "parse.y"
yyval=Cyc_YY9(0);
break;}case 411: _LL3D3: {
# 2863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2861 "parse.y"
{struct Cyc_Core_Opt*_tmpE7B;yyval=Cyc_YY9(((_tmpE7B=_cycalloc_atomic(sizeof(*_tmpE7B)),((_tmpE7B->v=(void*)Cyc_Absyn_Times,_tmpE7B)))));}
break;}case 412: _LL3D4: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2862 "parse.y"
{struct Cyc_Core_Opt*_tmpE7C;yyval=Cyc_YY9(((_tmpE7C=_cycalloc_atomic(sizeof(*_tmpE7C)),((_tmpE7C->v=(void*)Cyc_Absyn_Div,_tmpE7C)))));}
break;}case 413: _LL3D5: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2863 "parse.y"
{struct Cyc_Core_Opt*_tmpE7D;yyval=Cyc_YY9(((_tmpE7D=_cycalloc_atomic(sizeof(*_tmpE7D)),((_tmpE7D->v=(void*)Cyc_Absyn_Mod,_tmpE7D)))));}
break;}case 414: _LL3D6: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2864 "parse.y"
{struct Cyc_Core_Opt*_tmpE7E;yyval=Cyc_YY9(((_tmpE7E=_cycalloc_atomic(sizeof(*_tmpE7E)),((_tmpE7E->v=(void*)Cyc_Absyn_Plus,_tmpE7E)))));}
break;}case 415: _LL3D7: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2865 "parse.y"
{struct Cyc_Core_Opt*_tmpE7F;yyval=Cyc_YY9(((_tmpE7F=_cycalloc_atomic(sizeof(*_tmpE7F)),((_tmpE7F->v=(void*)Cyc_Absyn_Minus,_tmpE7F)))));}
break;}case 416: _LL3D8: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2866 "parse.y"
{struct Cyc_Core_Opt*_tmpE80;yyval=Cyc_YY9(((_tmpE80=_cycalloc_atomic(sizeof(*_tmpE80)),((_tmpE80->v=(void*)Cyc_Absyn_Bitlshift,_tmpE80)))));}
break;}case 417: _LL3D9: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2867 "parse.y"
{struct Cyc_Core_Opt*_tmpE81;yyval=Cyc_YY9(((_tmpE81=_cycalloc_atomic(sizeof(*_tmpE81)),((_tmpE81->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE81)))));}
break;}case 418: _LL3DA: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2868 "parse.y"
{struct Cyc_Core_Opt*_tmpE82;yyval=Cyc_YY9(((_tmpE82=_cycalloc_atomic(sizeof(*_tmpE82)),((_tmpE82->v=(void*)Cyc_Absyn_Bitand,_tmpE82)))));}
break;}case 419: _LL3DB: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2869 "parse.y"
{struct Cyc_Core_Opt*_tmpE83;yyval=Cyc_YY9(((_tmpE83=_cycalloc_atomic(sizeof(*_tmpE83)),((_tmpE83->v=(void*)Cyc_Absyn_Bitxor,_tmpE83)))));}
break;}case 420: _LL3DC: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2870 "parse.y"
{struct Cyc_Core_Opt*_tmpE84;yyval=Cyc_YY9(((_tmpE84=_cycalloc_atomic(sizeof(*_tmpE84)),((_tmpE84->v=(void*)Cyc_Absyn_Bitor,_tmpE84)))));}
break;}case 421: _LL3DD: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2875 "parse.y"
yyval=yyyyvsp[0];
break;}case 422: _LL3DE: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2877 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3DF: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2880 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E0: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2883 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E1: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2885 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E2: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2887 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E3: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2889 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E4: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2893 "parse.y"
yyval=yyyyvsp[0];
break;}case 429: _LL3E5: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2897 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3E6: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2899 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3E7: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2903 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3E8: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2905 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3E9: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2909 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3EA: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2911 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3EB: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3EC: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3ED: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2921 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3EE: {
# 2924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2923 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3EF: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL3F0: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2929 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F1: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2931 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F2: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2935 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL3F3: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2937 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F4: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2939 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3F5: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2941 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3F6: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2943 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F7: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2947 "parse.y"
yyval=yyyyvsp[0];
break;}case 448: _LL3F8: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2949 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3F9: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2951 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FA: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2955 "parse.y"
yyval=yyyyvsp[0];
break;}case 451: _LL3FB: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2957 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3FC: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2959 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL3FD: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2963 "parse.y"
yyval=yyyyvsp[0];
break;}case 454: _LL3FE: {
# 2966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2965 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL3FF: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2967 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL400: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2969 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL401: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2973 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL402: {
# 2976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2975 "parse.y"
void*_tmp7EC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7EC,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2978
break;}case 459: _LL403: {
# 2980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2981 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL404: {
# 2984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2982 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL405: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2983 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL406: {
# 2986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2984 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL407: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2985 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL408: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2986 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL409: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2987 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40A: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2989 "parse.y"
void*_tmp7ED=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7ED,(unsigned int)(yyyylsp[0]).first_line));
# 2992
break;}case 467: _LL40B: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2992 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL40C: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2994 "parse.y"
void*_tmp7EE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7EE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2997
break;}case 469: _LL40D: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2999 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE8A;struct Cyc_Absyn_MallocInfo _tmpE89;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE88;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88[0]=((_tmpE8A.tag=33,((_tmpE8A.f1=((_tmpE89.is_calloc=0,((_tmpE89.rgn=0,((_tmpE89.elt_type=0,((_tmpE89.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE89.fat_result=0,_tmpE89)))))))))),_tmpE8A)))),_tmpE88)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3001
break;}case 470: _LL40E: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3002 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE90;struct Cyc_Absyn_MallocInfo _tmpE8F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE8E;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E[0]=((_tmpE90.tag=33,((_tmpE90.f1=((_tmpE8F.is_calloc=0,((_tmpE8F.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE8F.elt_type=0,((_tmpE8F.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE8F.fat_result=0,_tmpE8F)))))))))),_tmpE90)))),_tmpE8E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3004
break;}case 471: _LL40F: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3005 "parse.y"
void*_tmp7F5=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE9A;void**_tmpE99;struct Cyc_Absyn_MallocInfo _tmpE98;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE97;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97[0]=((_tmpE9A.tag=33,((_tmpE9A.f1=((_tmpE98.is_calloc=1,((_tmpE98.rgn=0,((_tmpE98.elt_type=((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99[0]=_tmp7F5,_tmpE99)))),((_tmpE98.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE98.fat_result=0,_tmpE98)))))))))),_tmpE9A)))),_tmpE97)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3008
break;}case 472: _LL410: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3010 "parse.y"
void*_tmp7FA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEA4;void**_tmpEA3;struct Cyc_Absyn_MallocInfo _tmpEA2;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEA1;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1[0]=((_tmpEA4.tag=33,((_tmpEA4.f1=((_tmpEA2.is_calloc=1,((_tmpEA2.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA2.elt_type=((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=_tmp7FA,_tmpEA3)))),((_tmpEA2.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpEA2.fat_result=0,_tmpEA2)))))))))),_tmpEA4)))),_tmpEA1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3013
break;}case 473: _LL411: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3014 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEA5[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEA5[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEA5,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL412: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3016 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEAB;struct _dyneither_ptr*_tmpEAA;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEA9;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=((_tmpEAB.tag=37,((_tmpEAB.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEAB.f2=((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEAA)))),_tmpEAB)))))),_tmpEA9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL413: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3018 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEB1;struct _dyneither_ptr*_tmpEB0;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEAF;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB1.tag=37,((_tmpEB1.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEB1.f2=((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEB0)))),_tmpEB1)))))),_tmpEAF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL414: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3020 "parse.y"
void*_tmp806=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp806,(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL415: {
# 3024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3023 "parse.y"
struct _tuple20 _tmp807=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp809;struct _dyneither_ptr _tmp80A;struct _tuple20 _tmp808=_tmp807;_tmp809=_tmp808.f1;_tmp80A=_tmp808.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp809,_tmp80A,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL416: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3028 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 479: _LL417: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3029 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 480: _LL418: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3030 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 481: _LL419: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3035 "parse.y"
yyval=yyyyvsp[0];
break;}case 482: _LL41A: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3037 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL41B: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3039 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL41C: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3041 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL41D: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3043 "parse.y"
{struct _dyneither_ptr*_tmpEB2;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEB2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL41E: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3045 "parse.y"
{struct _dyneither_ptr*_tmpEB3;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEB3=_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEB3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL41F: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3047 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL420: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3049 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL421: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3051 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB6;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5[0]=((_tmpEB6.tag=24,((_tmpEB6.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB6)))))),_tmpEB5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL422: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3053 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB9;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB8;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8[0]=((_tmpEB9.tag=24,((_tmpEB9.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB9.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB9)))))),_tmpEB8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL423: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3058 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEC3;struct _dyneither_ptr*_tmpEC2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEC1;struct Cyc_List_List*_tmpEC0;yyval=Cyc_YY3(((_tmpEC0=_cycalloc(sizeof(*_tmpEC0)),((_tmpEC0->hd=(void*)((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3[0]=((_tmpEC1.tag=0,((_tmpEC1.f1=((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEC2)))),_tmpEC1)))),_tmpEC3)))),((_tmpEC0->tl=0,_tmpEC0)))))));}
break;}case 492: _LL424: {
# 3061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3061 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEC9;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEC8;struct Cyc_List_List*_tmpEC7;yyval=Cyc_YY3(((_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7->hd=(void*)((_tmpEC9=_cycalloc_atomic(sizeof(*_tmpEC9)),((_tmpEC9[0]=((_tmpEC8.tag=1,((_tmpEC8.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpEC8)))),_tmpEC9)))),((_tmpEC7->tl=0,_tmpEC7)))))));}
break;}case 493: _LL425: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3063 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpED3;struct _dyneither_ptr*_tmpED2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpED1;struct Cyc_List_List*_tmpED0;yyval=Cyc_YY3(((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0->hd=(void*)((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3[0]=((_tmpED1.tag=0,((_tmpED1.f1=((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED2)))),_tmpED1)))),_tmpED3)))),((_tmpED0->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpED0)))))));}
break;}case 494: _LL426: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3066 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpED9;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpED8;struct Cyc_List_List*_tmpED7;yyval=Cyc_YY3(((_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7->hd=(void*)((_tmpED9=_cycalloc_atomic(sizeof(*_tmpED9)),((_tmpED9[0]=((_tmpED8.tag=1,((_tmpED8.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpED8)))),_tmpED9)))),((_tmpED7->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpED7)))))));}
break;}case 495: _LL427: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL428: {
# 3075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3074 "parse.y"
yyval=yyyyvsp[0];
break;}case 497: _LL429: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3076 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42A: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3078 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL42B: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3080 "parse.y"
yyval=yyyyvsp[1];
break;}case 500: _LL42C: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3085 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL42D: {
# 3088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3087 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL42E: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3090 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL42F: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3093 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEDC;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEDB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=28,((_tmpEDC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEDC.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpEDC.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpEDC.f4=0,_tmpEDC)))))))))),_tmpEDB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 504: _LL430: {
# 3096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3096 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL431: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3100 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 506: _LL432: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpEDD;yyval=Cyc_YY6(((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEDD->tl=0,_tmpEDD)))))));}
break;}case 507: _LL433: {
# 3109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3108 "parse.y"
{struct Cyc_List_List*_tmpEDE;yyval=Cyc_YY6(((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEDE->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEDE)))))));}
break;}case 508: _LL434: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp823=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp823.f1,_tmp823.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL435: {
# 3117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3115 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL436: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3116 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL437: {
# 3119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3118 "parse.y"
struct _dyneither_ptr _tmp824=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp824);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp824,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3126
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp824,i,(unsigned int)(yyyylsp[0]).first_line));
# 3128
break;}case 512: _LL438: {
# 3130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3129 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL439: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3133 "parse.y"
{struct _dyneither_ptr*_tmpEE1;struct _tuple0*_tmpEE0;yyval=Cyc_QualId_tok(((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0->f1=Cyc_Absyn_Rel_n(0),((_tmpEE0->f2=((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE1)))),_tmpEE0)))))));}
break;}case 514: _LL43A: {
# 3136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3134 "parse.y"
yyval=yyyyvsp[0];
break;}case 515: _LL43B: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3137 "parse.y"
{struct _dyneither_ptr*_tmpEE4;struct _tuple0*_tmpEE3;yyval=Cyc_QualId_tok(((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3->f1=Cyc_Absyn_Rel_n(0),((_tmpEE3->f2=((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE4)))),_tmpEE3)))))));}
break;}case 516: _LL43C: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3138 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL43D: {
# 3141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3143 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL43E: {
# 3146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3144 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL43F: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3147 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL440: {
# 3150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3148 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL441: {
# 3151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3153 "parse.y"
break;}case 522: _LL442: {
# 3155
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3157
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3153 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL443:
# 3158
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
yystate=Cyc_yypgoto[_check_known_subscript_notnull(145,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6712) && Cyc_yycheck[_check_known_subscript_notnull(6713,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6713,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(145,yyn - 146)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
# 431
if(yyn > - 32768  && yyn < 6712){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6713,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)])+ 15,count ++);}
{unsigned int _tmpEE9;unsigned int _tmpEE8;struct _dyneither_ptr _tmpEE7;char*_tmpEE6;unsigned int _tmpEE5;msg=((_tmpEE5=(unsigned int)(sze + 15),((_tmpEE6=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEE5 + 1)),((_tmpEE7=_tag_dyneither(_tmpEE6,sizeof(char),_tmpEE5 + 1),((((_tmpEE8=_tmpEE5,((_tmpEEA(& _tmpEE9,& _tmpEE8,& _tmpEE6),_tmpEE6[_tmpEE8]=(char)0)))),_tmpEE7))))))));}
{const char*_tmpEEB;Cyc_strcpy(msg,((_tmpEEB="parse error",_tag_dyneither(_tmpEEB,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6713,x + yyn)]== x){
# 453
{const char*_tmpEED;const char*_tmpEEC;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEED=", expecting `",_tag_dyneither(_tmpEED,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEEC=" or `",_tag_dyneither(_tmpEEC,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)]);
{const char*_tmpEEE;Cyc_strcat(msg,((_tmpEEE="'",_tag_dyneither(_tmpEEE,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEEF;Cyc_yyerror(((_tmpEEF="parse error",_tag_dyneither(_tmpEEF,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp832=1;_npop_handler(0);return _tmp832;}
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
 if(yyssp_offset == 0){int _tmp833=1;_npop_handler(0);return _tmp833;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6712) || Cyc_yycheck[_check_known_subscript_notnull(6713,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6713,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1037){
int _tmp834=0;_npop_handler(0);return _tmp834;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3156 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp83E=v;int _tmp83F;char _tmp840;struct _dyneither_ptr _tmp841;union Cyc_Absyn_Nmspace _tmp842;struct _dyneither_ptr*_tmp843;_LL446: if((_tmp83E.Int_tok).tag != 1)goto _LL448;_tmp83F=((struct _tuple5)(_tmp83E.Int_tok).val).f2;_LL447:
{const char*_tmpEF3;void*_tmpEF2[1];struct Cyc_Int_pa_PrintArg_struct _tmpEF1;(_tmpEF1.tag=1,((_tmpEF1.f1=(unsigned long)_tmp83F,((_tmpEF2[0]=& _tmpEF1,Cyc_fprintf(Cyc_stderr,((_tmpEF3="%d",_tag_dyneither(_tmpEF3,sizeof(char),3))),_tag_dyneither(_tmpEF2,sizeof(void*),1)))))));}goto _LL445;_LL448: if((_tmp83E.Char_tok).tag != 2)goto _LL44A;_tmp840=(char)(_tmp83E.Char_tok).val;_LL449:
{const char*_tmpEF7;void*_tmpEF6[1];struct Cyc_Int_pa_PrintArg_struct _tmpEF5;(_tmpEF5.tag=1,((_tmpEF5.f1=(unsigned long)((int)_tmp840),((_tmpEF6[0]=& _tmpEF5,Cyc_fprintf(Cyc_stderr,((_tmpEF7="%c",_tag_dyneither(_tmpEF7,sizeof(char),3))),_tag_dyneither(_tmpEF6,sizeof(void*),1)))))));}goto _LL445;_LL44A: if((_tmp83E.String_tok).tag != 3)goto _LL44C;_tmp841=(struct _dyneither_ptr)(_tmp83E.String_tok).val;_LL44B:
{const char*_tmpEFB;void*_tmpEFA[1];struct Cyc_String_pa_PrintArg_struct _tmpEF9;(_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp841),((_tmpEFA[0]=& _tmpEF9,Cyc_fprintf(Cyc_stderr,((_tmpEFB="\"%s\"",_tag_dyneither(_tmpEFB,sizeof(char),5))),_tag_dyneither(_tmpEFA,sizeof(void*),1)))))));}goto _LL445;_LL44C: if((_tmp83E.QualId_tok).tag != 5)goto _LL44E;_tmp842=((struct _tuple0*)(_tmp83E.QualId_tok).val)->f1;_tmp843=((struct _tuple0*)(_tmp83E.QualId_tok).val)->f2;_LL44D: {
# 3162
struct Cyc_List_List*_tmp84D=0;
{union Cyc_Absyn_Nmspace _tmp84E=_tmp842;struct Cyc_List_List*_tmp84F;struct Cyc_List_List*_tmp850;struct Cyc_List_List*_tmp851;_LL451: if((_tmp84E.Rel_n).tag != 1)goto _LL453;_tmp84F=(struct Cyc_List_List*)(_tmp84E.Rel_n).val;_LL452:
 _tmp84D=_tmp84F;goto _LL450;_LL453: if((_tmp84E.Abs_n).tag != 2)goto _LL455;_tmp850=(struct Cyc_List_List*)(_tmp84E.Abs_n).val;_LL454:
 _tmp84D=_tmp850;goto _LL450;_LL455: if((_tmp84E.C_n).tag != 3)goto _LL457;_tmp851=(struct Cyc_List_List*)(_tmp84E.C_n).val;_LL456:
 _tmp84D=_tmp851;goto _LL450;_LL457: if((_tmp84E.Loc_n).tag != 4)goto _LL450;_LL458:
 goto _LL450;_LL450:;}
# 3169
for(0;_tmp84D != 0;_tmp84D=_tmp84D->tl){
const char*_tmpEFF;void*_tmpEFE[1];struct Cyc_String_pa_PrintArg_struct _tmpEFD;(_tmpEFD.tag=0,((_tmpEFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp84D->hd)),((_tmpEFE[0]=& _tmpEFD,Cyc_fprintf(Cyc_stderr,((_tmpEFF="%s::",_tag_dyneither(_tmpEFF,sizeof(char),5))),_tag_dyneither(_tmpEFE,sizeof(void*),1)))))));}
{const char*_tmpF03;void*_tmpF02[1];struct Cyc_String_pa_PrintArg_struct _tmpF01;(_tmpF01.tag=0,((_tmpF01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp843),((_tmpF02[0]=& _tmpF01,Cyc_fprintf(Cyc_stderr,((_tmpF03="%s::",_tag_dyneither(_tmpF03,sizeof(char),5))),_tag_dyneither(_tmpF02,sizeof(void*),1)))))));}
goto _LL445;}_LL44E:;_LL44F:
{const char*_tmpF06;void*_tmpF05;(_tmpF05=0,Cyc_fprintf(Cyc_stderr,((_tmpF06="?",_tag_dyneither(_tmpF06,sizeof(char),2))),_tag_dyneither(_tmpF05,sizeof(void*),0)));}goto _LL445;_LL445:;}
# 3178
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp85A=_new_region("yyr");struct _RegionHandle*yyr=& _tmp85A;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp85B=Cyc_Parse_parse_result;_npop_handler(0);return _tmp85B;};
# 3181
;_pop_region(yyr);};}
