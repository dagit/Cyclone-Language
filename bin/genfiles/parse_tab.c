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
# 973
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
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
# 307 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
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
{struct _tuple10*_tmp829;declarators=((_tmp829=_region_malloc(r,sizeof(*_tmp829)),((_tmp829->tl=declarators,((_tmp829->hd=_tmp1,_tmp829))))));}{
struct Cyc_List_List*_tmp82A;exprs=((_tmp82A=_region_malloc(r,sizeof(*_tmp82A)),((_tmp82A->hd=_tmp2,((_tmp82A->tl=exprs,_tmp82A))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp82D;struct Cyc_Position_Exit_exn_struct*_tmp82C;(int)_throw((void*)((_tmp82C=_cycalloc_atomic(sizeof(*_tmp82C)),((_tmp82C[0]=((_tmp82D.tag=Cyc_Position_Exit,_tmp82D)),_tmp82C)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp832;void*_tmp831[2];struct Cyc_String_pa_PrintArg_struct _tmp830;struct Cyc_String_pa_PrintArg_struct _tmp82F;(_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp831[0]=& _tmp830,((_tmp831[1]=& _tmp82F,Cyc_fprintf(Cyc_stderr,((_tmp832="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp832,sizeof(char),46))),_tag_dyneither(_tmp831,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpC;void*_tmpD;struct _tuple8*_tmpB=tqt;_tmpC=_tmpB->f2;_tmpD=_tmpB->f3;
if((_tmpC.print_const  || _tmpC.q_volatile) || _tmpC.q_restrict){
if(_tmpC.loc != 0)loc=_tmpC.loc;{
const char*_tmp835;void*_tmp834;(_tmp834=0,Cyc_Tcutil_warn(loc,((_tmp835="qualifier on type is ignored",_tag_dyneither(_tmp835,sizeof(char),29))),_tag_dyneither(_tmp834,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp838;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp837;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp837=_cycalloc(sizeof(*_tmp837)),((_tmp837[0]=((_tmp838.tag=1,((_tmp838.f1=_tmp18,_tmp838)))),_tmp837)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp10;if(_tmp19->tag != 1)goto _LL0;else{_tmp1A=(void*)_tmp19->f1;}}_LL10:
# 226
 rgn=_tmp1A;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp839;return(_tmp839.f1=nullable,((_tmp839.f2=bound,((_tmp839.f3=zeroterm,((_tmp839.f4=rgn,_tmp839)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp846;const char*_tmp845;void*_tmp844[1];struct Cyc_Int_pa_PrintArg_struct _tmp843;struct _tuple0*_tmp842;return(_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842->f1=Cyc_Absyn_Rel_n(0),((_tmp842->f2=(
(_tmp846=_cycalloc(sizeof(*_tmp846)),((_tmp846[0]=(struct _dyneither_ptr)((_tmp843.tag=1,((_tmp843.f1=(unsigned long)enum_counter ++,((_tmp844[0]=& _tmp843,Cyc_aprintf(((_tmp845="__anonymous_enum_%d__",_tag_dyneither(_tmp845,sizeof(char),22))),_tag_dyneither(_tmp844,sizeof(void*),1)))))))),_tmp846)))),_tmp842)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp24;struct Cyc_Absyn_Tqual _tmp25;void*_tmp26;struct Cyc_List_List*_tmp27;struct Cyc_List_List*_tmp28;struct Cyc_Absyn_Exp*_tmp29;struct Cyc_Absyn_Exp*_tmp2A;struct _tuple17*_tmp23=field_info;_tmp24=(_tmp23->f1)->f1;_tmp25=(_tmp23->f1)->f2;_tmp26=(_tmp23->f1)->f3;_tmp27=(_tmp23->f1)->f4;_tmp28=(_tmp23->f1)->f5;_tmp29=(_tmp23->f2)->f1;_tmp2A=(_tmp23->f2)->f2;
if(_tmp27 != 0){
const char*_tmp847;Cyc_Parse_err(((_tmp847="bad type params in struct field",_tag_dyneither(_tmp847,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp24)){
const char*_tmp848;Cyc_Parse_err(((_tmp848="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp848,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp849;return(_tmp849=_cycalloc(sizeof(*_tmp849)),((_tmp849->name=(*_tmp24).f2,((_tmp849->tq=_tmp25,((_tmp849->type=_tmp26,((_tmp849->width=_tmp29,((_tmp849->attributes=_tmp28,((_tmp849->requires_clause=_tmp2A,_tmp849)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp84A;return(_tmp84A.Signed_spec=0,((_tmp84A.Unsigned_spec=0,((_tmp84A.Short_spec=0,((_tmp84A.Long_spec=0,((_tmp84A.Long_Long_spec=0,((_tmp84A.Valid_type_spec=0,((_tmp84A.Type_spec=Cyc_Absyn_sint_typ,((_tmp84A.loc=loc,_tmp84A)))))))))))))));}
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
void*_tmp34=t;void*_tmp36;struct Cyc_Absyn_Tqual _tmp37;struct Cyc_Absyn_Exp*_tmp38;union Cyc_Absyn_Constraint*_tmp39;unsigned int _tmp3A;_LL12: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34;if(_tmp35->tag != 8)goto _LL14;else{_tmp36=(_tmp35->f1).elt_type;_tmp37=(_tmp35->f1).tq;_tmp38=(_tmp35->f1).num_elts;_tmp39=(_tmp35->f1).zero_term;_tmp3A=(_tmp35->f1).zt_loc;}}_LL13: {
# 300
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp84D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp84C;return Cyc_Absyn_starb_typ(_tmp36,
argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp37,
# 303
_tmp38 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C[0]=((_tmp84D.tag=1,((_tmp84D.f1=_tmp38,_tmp84D)))),_tmp84C)))),_tmp39);}_LL14:;_LL15:
# 306
 return t;_LL11:;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 319 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3D=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp3E=(struct _tuple8*)x->hd;struct _dyneither_ptr*_tmp3F;void*_tmp41;struct _dyneither_ptr _tmp42;void**_tmp45;_LL17: _tmp3F=_tmp3E->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E->f3;if(_tmp40->tag != 19)goto _LL19;else{_tmp41=(void*)_tmp40->f1;}};if(!(_tmp3F != 0))goto _LL19;_LL18:
# 324
{void*_tmp46=_tmp41;void**_tmp48;_LL1E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp47=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp46;if(_tmp47->tag != 1)goto _LL20;else{_tmp48=(void**)((void**)& _tmp47->f2);}}_LL1F: {
# 328
struct Cyc_String_pa_PrintArg_struct _tmp855;void*_tmp854[1];const char*_tmp853;struct _dyneither_ptr*_tmp852;struct _dyneither_ptr*nm=(_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852[0]=(struct _dyneither_ptr)((_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F),((_tmp854[0]=& _tmp855,Cyc_aprintf(((_tmp853="`%s",_tag_dyneither(_tmp853,sizeof(char),4))),_tag_dyneither(_tmp854,sizeof(void*),1)))))))),_tmp852)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp864;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp863;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp862;struct Cyc_Absyn_Tvar*_tmp861;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp860;*_tmp48=(void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp864.tag=2,((_tmp864.f1=((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->name=nm,((_tmp861->identity=- 1,((_tmp861->kind=(void*)((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((_tmp862.tag=0,((_tmp862.f1=& Cyc_Tcutil_ik,_tmp862)))),_tmp863)))),_tmp861)))))))),_tmp864)))),_tmp860))));}
goto _LL1D;}_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 333
{struct _tuple18*_tmp867;struct Cyc_List_List*_tmp866;_tmp3D=((_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866->hd=((_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867->f1=_tmp3F,((_tmp867->f2=_tmp41,_tmp867)))))),((_tmp866->tl=_tmp3D,_tmp866))))));}goto _LL16;_LL19: if(_tmp3E->f1 == 0)goto _LL1B;_tmp42=*_tmp3E->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E->f3;if(_tmp43->tag != 15)goto _LL1B;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp44=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp43->f1);if(_tmp44->tag != 1)goto _LL1B;else{_tmp45=(void**)((void**)& _tmp44->f2);}}};_LL1A: {
# 337
struct Cyc_String_pa_PrintArg_struct _tmp86F;void*_tmp86E[1];const char*_tmp86D;struct _dyneither_ptr*_tmp86C;struct _dyneither_ptr*nm=(_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=(struct _dyneither_ptr)((_tmp86F.tag=0,((_tmp86F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp42),((_tmp86E[0]=& _tmp86F,Cyc_aprintf(((_tmp86D="`%s",_tag_dyneither(_tmp86D,sizeof(char),4))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),_tmp86C)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp87E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp87D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp87C;struct Cyc_Absyn_Tvar*_tmp87B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp87A;*_tmp45=(void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87E.tag=2,((_tmp87E.f1=((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B->name=nm,((_tmp87B->identity=- 1,((_tmp87B->kind=(void*)((_tmp87D=_cycalloc(sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87C.tag=0,((_tmp87C.f1=& Cyc_Tcutil_rk,_tmp87C)))),_tmp87D)))),_tmp87B)))))))),_tmp87E)))),_tmp87A))));}
goto _LL16;}_LL1B:;_LL1C:
 goto _LL16;_LL16:;}
# 343
return _tmp3D;}
# 347
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp5D=0;
for(0;x != 0;x=x->tl){
void*_tmp5E=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp60;_LL23: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5E;if(_tmp5F->tag != 19)goto _LL25;else{_tmp60=(void*)_tmp5F->f1;}}_LL24:
# 352
{struct _tuple18*_tmp881;struct Cyc_List_List*_tmp880;_tmp5D=((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880->hd=((_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp881->f2=_tmp60,_tmp881)))))),((_tmp880->tl=_tmp5D,_tmp880))))));}goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 356
return _tmp5D;}
# 360
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp63=e->r;struct _dyneither_ptr*_tmp65;_LL28: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp64=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp63;if(_tmp64->tag != 1)goto _LL2A;else{if((((_tmp64->f1)->f1).Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(((_tmp64->f1)->f1).Rel_n).val != 0)goto _LL2A;_tmp65=(_tmp64->f1)->f2;}}_LL29:
# 363
{struct Cyc_List_List*_tmp66=tags;for(0;_tmp66 != 0;_tmp66=_tmp66->tl){
struct _dyneither_ptr*_tmp68;void*_tmp69;struct _tuple18*_tmp67=(struct _tuple18*)_tmp66->hd;_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;
if(Cyc_strptrcmp(_tmp68,_tmp65)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp884;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp883;return Cyc_Absyn_new_exp((void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp884.tag=38,((_tmp884.f1=Cyc_Tcutil_copy_type(_tmp69),_tmp884)))),_tmp883)))),e->loc);}}}
# 368
goto _LL27;_LL2A:;_LL2B:
 goto _LL27;_LL27:;}
# 371
return e;}
# 376
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp6C=t;void*_tmp6E;struct Cyc_Absyn_Tqual _tmp6F;struct Cyc_Absyn_Exp*_tmp70;union Cyc_Absyn_Constraint*_tmp71;unsigned int _tmp72;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;void*_tmp76;union Cyc_Absyn_Constraint*_tmp77;union Cyc_Absyn_Constraint*_tmp78;union Cyc_Absyn_Constraint*_tmp79;struct Cyc_Absyn_PtrLoc*_tmp7A;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C;if(_tmp6D->tag != 8)goto _LL2F;else{_tmp6E=(_tmp6D->f1).elt_type;_tmp6F=(_tmp6D->f1).tq;_tmp70=(_tmp6D->f1).num_elts;_tmp71=(_tmp6D->f1).zero_term;_tmp72=(_tmp6D->f1).zt_loc;}}_LL2E: {
# 379
struct Cyc_Absyn_Exp*nelts2=_tmp70;
if(_tmp70 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp70);{
# 383
void*_tmp7B=Cyc_Parse_substitute_tags(tags,_tmp6E);
if(_tmp70 != nelts2  || _tmp6E != _tmp7B){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp88A;struct Cyc_Absyn_ArrayInfo _tmp889;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp888;return(void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp88A.tag=8,((_tmp88A.f1=((_tmp889.elt_type=_tmp7B,((_tmp889.tq=_tmp6F,((_tmp889.num_elts=nelts2,((_tmp889.zero_term=_tmp71,((_tmp889.zt_loc=_tmp72,_tmp889)))))))))),_tmp88A)))),_tmp888))));}
goto _LL2C;};}_LL2F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C;if(_tmp73->tag != 5)goto _LL31;else{_tmp74=(_tmp73->f1).elt_typ;_tmp75=(_tmp73->f1).elt_tq;_tmp76=((_tmp73->f1).ptr_atts).rgn;_tmp77=((_tmp73->f1).ptr_atts).nullable;_tmp78=((_tmp73->f1).ptr_atts).bounds;_tmp79=((_tmp73->f1).ptr_atts).zero_term;_tmp7A=((_tmp73->f1).ptr_atts).ptrloc;}}_LL30: {
# 388
void*_tmp7F=Cyc_Parse_substitute_tags(tags,_tmp74);
union Cyc_Absyn_Constraint*_tmp80=_tmp78;
{union Cyc_Absyn_Constraint _tmp81=*_tmp78;struct Cyc_Absyn_Exp*_tmp83;_LL34: if((_tmp81.Eq_constr).tag != 1)goto _LL36;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp81.Eq_constr).val);if(_tmp82->tag != 1)goto _LL36;else{_tmp83=_tmp82->f1;}};_LL35:
# 392
 _tmp83=Cyc_Parse_substitute_tags_exp(tags,_tmp83);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp890;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp88F;union Cyc_Absyn_Constraint*_tmp88E;_tmp80=((_tmp88E=_cycalloc(sizeof(*_tmp88E)),(((_tmp88E->Eq_constr).val=(void*)((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp88F.tag=1,((_tmp88F.f1=_tmp83,_tmp88F)))),_tmp890)))),(((_tmp88E->Eq_constr).tag=1,_tmp88E))))));}
goto _LL33;_LL36:;_LL37:
 goto _LL33;_LL33:;}
# 397
if(_tmp7F != _tmp74  || _tmp80 != _tmp78){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp896;struct Cyc_Absyn_PtrInfo _tmp895;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp894;return(void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((_tmp896.tag=5,((_tmp896.f1=((_tmp895.elt_typ=_tmp7F,((_tmp895.elt_tq=_tmp75,((_tmp895.ptr_atts=(((_tmp895.ptr_atts).rgn=_tmp76,(((_tmp895.ptr_atts).nullable=_tmp77,(((_tmp895.ptr_atts).bounds=_tmp80,(((_tmp895.ptr_atts).zero_term=_tmp79,(((_tmp895.ptr_atts).ptrloc=_tmp7A,_tmp895.ptr_atts)))))))))),_tmp895)))))),_tmp896)))),_tmp894))));}
goto _LL2C;}_LL31:;_LL32:
# 402
 goto _LL2C;_LL2C:;}
# 404
return t;}
# 409
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 416
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 418
struct _tuple19*_tmp897;return(_tmp897=_cycalloc(sizeof(*_tmp897)),((_tmp897->f1=(*t).f2,((_tmp897->f2=(*t).f3,_tmp897)))));}
# 421
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp8B=d->r;struct Cyc_Absyn_Vardecl*_tmp8D;_LL39: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL3B;else{_tmp8D=_tmp8C->f1;}}_LL3A:
 return _tmp8D;_LL3B:;_LL3C: {
const char*_tmp89A;void*_tmp899;(_tmp899=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp89A="bad declaration in `forarray' statement",_tag_dyneither(_tmp89A,sizeof(char),40))),_tag_dyneither(_tmp899,sizeof(void*),0)));}_LL38:;}
# 428
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp90=tm;_LL3E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp91=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp90;if(_tmp91->tag != 4)goto _LL40;}_LL3F:
 return 1;_LL40:;_LL41:
 return 0;_LL3D:;}
# 437
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp89B;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp89B="`H",_tag_dyneither(_tmp89B,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp89C;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp89C="`U",_tag_dyneither(_tmp89C,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp89D;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp89D="`RC",_tag_dyneither(_tmp89D,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 445
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A7;struct _dyneither_ptr*_tmp8A6;struct Cyc_Absyn_Tvar*_tmp8A5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8A4;return(void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A7.tag=2,((_tmp8A7.f1=((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->name=((_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6[0]=s,_tmp8A6)))),((_tmp8A5->identity=- 1,((_tmp8A5->kind=k,_tmp8A5)))))))),_tmp8A7)))),_tmp8A4))));}}}}
# 448
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp99=Cyc_Absyn_compress_kb(t->kind);struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL43: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL45;}_LL44:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8AA;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8A9;k=(void*)((_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=((_tmp8AA.tag=1,((_tmp8AA.f1=0,_tmp8AA)))),_tmp8A9))));}goto _LL42;_LL45: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL47;else{_tmp9C=_tmp9B->f2;}}_LL46:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8AD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8AC;k=(void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AD.tag=2,((_tmp8AD.f1=0,((_tmp8AD.f2=_tmp9C,_tmp8AD)))))),_tmp8AC))));}goto _LL42;_LL47: _tmp9D=_tmp99;_LL48:
 k=_tmp9D;goto _LL42;_LL42:;}{
# 455
struct Cyc_Absyn_Tvar*_tmp8AE;return(_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->name=t->name,((_tmp8AE->identity=- 1,((_tmp8AE->kind=k,_tmp8AE)))))));};}
# 462
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL4C;else{_tmpA5=_tmpA4->f1;}}_LL4B:
 return _tmpA5;_LL4C:;_LL4D: {
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8B1="expecting a list of type variables, not types",_tag_dyneither(_tmp8B1,sizeof(char),46))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}_LL49:;}
# 468
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8B4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8B3;return(void*)((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B4.tag=2,((_tmp8B4.f1=pr,_tmp8B4)))),_tmp8B3))));}
# 473
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void**_tmpAC;_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAA;if(_tmpAB->tag != 2)goto _LL51;else{_tmpAC=(void**)&(_tmpAB->f1)->kind;}}_LL50: {
# 476
void*_tmpAD=Cyc_Absyn_compress_kb(*_tmpAC);_LL54: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LL56;}_LL55:
# 478
 if(!leq)*_tmpAC=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8B7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8B6;*_tmpAC=(void*)((_tmp8B6=_cycalloc(sizeof(*_tmp8B6)),((_tmp8B6[0]=((_tmp8B7.tag=2,((_tmp8B7.f1=0,((_tmp8B7.f2=k,_tmp8B7)))))),_tmp8B6))));}
return;_LL56:;_LL57:
 return;_LL53:;}_LL51:;_LL52:
# 483
 return;_LL4E:;}
# 488
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 494
if(tds == 0)return tms;
# 499
if(tms == 0)return 0;{
# 501
void*_tmpB1=(void*)tms->hd;void*_tmpB3;_LL59: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB2=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB1;if(_tmpB2->tag != 3)goto _LL5B;else{_tmpB3=(void*)_tmpB2->f1;}}_LL5A:
# 504
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 507
void*_tmpB4=_tmpB3;struct Cyc_List_List*_tmpB7;_LL5E: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB4;if(_tmpB5->tag != 1)goto _LL60;}_LL5F:
# 509
{const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,Cyc_Tcutil_warn(loc,((_tmp8BA="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8BA,sizeof(char),93))),_tag_dyneither(_tmp8B9,sizeof(void*),0)));}
# 511
return tms;_LL60: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB6=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB4;if(_tmpB6->tag != 0)goto _LL5D;else{_tmpB7=_tmpB6->f1;}}_LL61:
# 513
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8BD="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8BD,sizeof(char),73))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{
# 517
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpB7 != 0;_tmpB7=_tmpB7->tl){
struct Cyc_List_List*_tmpBC=tds;
for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){
struct Cyc_Absyn_Decl*_tmpBD=(struct Cyc_Absyn_Decl*)_tmpBC->hd;
void*_tmpBE=_tmpBD->r;struct Cyc_Absyn_Vardecl*_tmpC0;_LL63: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpBF->tag != 0)goto _LL65;else{_tmpC0=_tmpBF->f1;}}_LL64:
# 524
 if(Cyc_zstrptrcmp((*_tmpC0->name).f2,(struct _dyneither_ptr*)_tmpB7->hd)!= 0)
continue;
if(_tmpC0->initializer != 0){
const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8C0="initializer found in parameter declaration",_tag_dyneither(_tmp8C0,sizeof(char),43))),_tag_dyneither(_tmp8BF,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC0->name)){
const char*_tmp8C3;void*_tmp8C2;(_tmp8C2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8C3="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8C3,sizeof(char),47))),_tag_dyneither(_tmp8C2,sizeof(void*),0)));}
{struct _tuple8*_tmp8C6;struct Cyc_List_List*_tmp8C5;rev_new_params=(
(_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5->hd=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->f1=(*_tmpC0->name).f2,((_tmp8C6->f2=_tmpC0->tq,((_tmp8C6->f3=_tmpC0->type,_tmp8C6)))))))),((_tmp8C5->tl=rev_new_params,_tmp8C5))))));}
# 533
goto L;_LL65:;_LL66: {
const char*_tmp8C9;void*_tmp8C8;(_tmp8C8=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpBD->loc,((_tmp8C9="nonvariable declaration in parameter type",_tag_dyneither(_tmp8C9,sizeof(char),42))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}_LL62:;}
# 537
L: if(_tmpBC == 0){
const char*_tmp8CD;void*_tmp8CC[1];struct Cyc_String_pa_PrintArg_struct _tmp8CB;(_tmp8CB.tag=0,((_tmp8CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpB7->hd)),((_tmp8CC[0]=& _tmp8CB,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8CD="%s is not given a type",_tag_dyneither(_tmp8CD,sizeof(char),23))),_tag_dyneither(_tmp8CC,sizeof(void*),1)))))));}}{
# 540
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8DC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8DB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp8DA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp8D9;struct Cyc_List_List*_tmp8D8;return
(_tmp8D8=_region_malloc(yy,sizeof(*_tmp8D8)),((_tmp8D8->hd=(void*)((_tmp8DC=_region_malloc(yy,sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8D9.tag=3,((_tmp8D9.f1=(void*)((_tmp8DB=_region_malloc(yy,sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DA.tag=1,((_tmp8DA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp8DA.f2=0,((_tmp8DA.f3=0,((_tmp8DA.f4=0,((_tmp8DA.f5=0,_tmp8DA)))))))))))),_tmp8DB)))),_tmp8D9)))),_tmp8DC)))),((_tmp8D8->tl=0,_tmp8D8)))));};};_LL5D:;}
# 547
goto _LL5C;_LL5B:;_LL5C: {
struct Cyc_List_List*_tmp8DD;return(_tmp8DD=_region_malloc(yy,sizeof(*_tmp8DD)),((_tmp8DD->hd=(void*)tms->hd,((_tmp8DD->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp8DD)))));}_LL58:;};}
# 555
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 559
if(tds != 0){
struct Cyc_Parse_Declarator _tmp8DE;d=((_tmp8DE.id=d.id,((_tmp8DE.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp8DE))));}{
# 562
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 568
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 574
if(dso->sc != 0)
switch(*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc))){case Cyc_Parse_Extern_sc: _LL67:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL68:
 sc=Cyc_Absyn_Static;break;default: _LL69:
{const char*_tmp8DF;Cyc_Parse_err(((_tmp8DF="bad storage class on function",_tag_dyneither(_tmp8DF,sizeof(char),30))),loc);}break;}}{
# 581
void*_tmpD4=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct Cyc_Absyn_Tqual _tmpD6;void*_tmpD7;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD9;struct _tuple13 _tmpD5=Cyc_Parse_apply_tms(tq,_tmpD4,atts,d.tms);_tmpD6=_tmpD5.f1;_tmpD7=_tmpD5.f2;_tmpD8=_tmpD5.f3;_tmpD9=_tmpD5.f4;
# 586
if(_tmpD8 != 0){
# 589
const char*_tmp8E2;void*_tmp8E1;(_tmp8E1=0,Cyc_Tcutil_warn(loc,((_tmp8E2="bad type params, ignoring",_tag_dyneither(_tmp8E2,sizeof(char),26))),_tag_dyneither(_tmp8E1,sizeof(void*),0)));}{
# 591
void*_tmpDC=_tmpD7;struct Cyc_List_List*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE1;struct Cyc_List_List*_tmpE2;int _tmpE3;struct Cyc_Absyn_VarargInfo*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*_tmpE6;_LL6C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDC;if(_tmpDD->tag != 9)goto _LL6E;else{_tmpDE=(_tmpDD->f1).tvars;_tmpDF=(_tmpDD->f1).effect;_tmpE0=(_tmpDD->f1).ret_tqual;_tmpE1=(_tmpDD->f1).ret_typ;_tmpE2=(_tmpDD->f1).args;_tmpE3=(_tmpDD->f1).c_varargs;_tmpE4=(_tmpDD->f1).cyc_varargs;_tmpE5=(_tmpDD->f1).rgn_po;_tmpE6=(_tmpDD->f1).attributes;}}_LL6D: {
# 594
struct Cyc_List_List*_tmpE7=0;
{struct Cyc_List_List*_tmpE8=_tmpE2;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpEA;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEC;struct _tuple8*_tmpE9=(struct _tuple8*)_tmpE8->hd;_tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=_tmpE9->f3;
if(_tmpEA == 0){
{const char*_tmp8E3;Cyc_Parse_err(((_tmp8E3="missing argument variable in function prototype",_tag_dyneither(_tmp8E3,sizeof(char),48))),loc);}{
struct _tuple8*_tmp8E6;struct Cyc_List_List*_tmp8E5;_tmpE7=((_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->hd=((_tmp8E6=_cycalloc(sizeof(*_tmp8E6)),((_tmp8E6->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp8E6->f2=_tmpEB,((_tmp8E6->f3=_tmpEC,_tmp8E6)))))))),((_tmp8E5->tl=_tmpE7,_tmp8E5))))));};}else{
# 601
struct _tuple8*_tmp8E9;struct Cyc_List_List*_tmp8E8;_tmpE7=((_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8->hd=((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9->f1=_tmpEA,((_tmp8E9->f2=_tmpEB,((_tmp8E9->f3=_tmpEC,_tmp8E9)))))))),((_tmp8E8->tl=_tmpE7,_tmp8E8))))));}}}{
# 605
struct Cyc_Absyn_Fndecl*_tmp8EA;return(_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->sc=sc,((_tmp8EA->name=d.id,((_tmp8EA->tvs=_tmpDE,((_tmp8EA->is_inline=is_inline,((_tmp8EA->effect=_tmpDF,((_tmp8EA->ret_tqual=_tmpE0,((_tmp8EA->ret_type=_tmpE1,((_tmp8EA->args=
# 608
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE7),((_tmp8EA->c_varargs=_tmpE3,((_tmp8EA->cyc_varargs=_tmpE4,((_tmp8EA->rgn_po=_tmpE5,((_tmp8EA->body=body,((_tmp8EA->cached_typ=0,((_tmp8EA->param_vardecls=0,((_tmp8EA->fn_vardecl=0,((_tmp8EA->attributes=
# 615
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpE6,_tmpD9),_tmp8EA)))))))))))))))))))))))))))))))));};}_LL6E:;_LL6F: {
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8ED="declarator is not a function prototype",_tag_dyneither(_tmp8ED,sizeof(char),39))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}_LL6B:;};};};}static char _tmpF6[52]="at most one type may appear within a type specifier";
# 620
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpF6,_tmpF6,_tmpF6 + 52};static char _tmpF7[63]="const or volatile may appear only once within a type specifier";
# 622
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpF7,_tmpF7,_tmpF7 + 63};static char _tmpF8[50]="type specifier includes more than one declaration";
# 624
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpF8,_tmpF8,_tmpF8 + 50};static char _tmpF9[60]="sign specifier may appear only once within a type specifier";
# 626
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpF9,_tmpF9,_tmpF9 + 60};
# 633
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 636
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp8EE;(_tmp8EE=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EE,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp8EF;(_tmp8EF=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp8F0;(_tmp8F0=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8F0,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp8F1;(_tmp8F1=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp8F1,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 658
return s1;}
# 664
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 667
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 674
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 686
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp8F4;void*_tmp8F3;(_tmp8F3=0,Cyc_Tcutil_warn(loc,((_tmp8F4="missing type within specifier",_tag_dyneither(_tmp8F4,sizeof(char),30))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 691
if(seen_sign){
void*_tmp100=t;enum Cyc_Absyn_Sign _tmp102;enum Cyc_Absyn_Size_of _tmp103;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 6)goto _LL73;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;}}_LL72:
# 694
 if(_tmp102 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp103);
goto _LL70;_LL73:;_LL74:
{const char*_tmp8F5;Cyc_Parse_err(((_tmp8F5="sign specification on non-integral type",_tag_dyneither(_tmp8F5,sizeof(char),40))),loc);}goto _LL70;_LL70:;}
# 699
if(seen_size){
void*_tmp105=t;enum Cyc_Absyn_Sign _tmp107;enum Cyc_Absyn_Size_of _tmp108;_LL76: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp105;if(_tmp106->tag != 6)goto _LL78;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}_LL77:
# 702
 if(_tmp108 != sz)
t=Cyc_Absyn_int_typ(_tmp107,sz);
goto _LL75;_LL78: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp109=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp105;if(_tmp109->tag != 7)goto _LL7A;}_LL79:
# 706
 t=Cyc_Absyn_float_typ(2);goto _LL75;_LL7A:;_LL7B:
{const char*_tmp8F6;Cyc_Parse_err(((_tmp8F6="size qualifier on non-integral type",_tag_dyneither(_tmp8F6,sizeof(char),36))),loc);}goto _LL75;_LL75:;}}
# 710
return t;}
# 713
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 717
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp10B=d.id;
struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10E;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp110;struct _tuple13 _tmp10C=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);_tmp10D=_tmp10C.f1;_tmp10E=_tmp10C.f2;_tmp10F=_tmp10C.f3;_tmp110=_tmp10C.f4;
# 723
if(ds->tl == 0){
struct _tuple15*_tmp8F9;struct Cyc_List_List*_tmp8F8;return(_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8->hd=((_tmp8F9=_region_malloc(r,sizeof(*_tmp8F9)),((_tmp8F9->f1=_tmp10B,((_tmp8F9->f2=_tmp10D,((_tmp8F9->f3=_tmp10E,((_tmp8F9->f4=_tmp10F,((_tmp8F9->f5=_tmp110,_tmp8F9)))))))))))),((_tmp8F8->tl=0,_tmp8F8)))));}else{
# 726
struct _tuple15*_tmp8FC;struct Cyc_List_List*_tmp8FB;return(_tmp8FB=_region_malloc(r,sizeof(*_tmp8FB)),((_tmp8FB->hd=((_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC->f1=_tmp10B,((_tmp8FC->f2=_tmp10D,((_tmp8FC->f3=_tmp10E,((_tmp8FC->f4=_tmp10F,((_tmp8FC->f5=_tmp110,_tmp8FC)))))))))))),((_tmp8FB->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp8FB)))));}};}
# 730
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 733
if(tms == 0){struct _tuple13 _tmp8FD;return(_tmp8FD.f1=tq,((_tmp8FD.f2=t,((_tmp8FD.f3=0,((_tmp8FD.f4=atts,_tmp8FD)))))));}{
void*_tmp116=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp118;unsigned int _tmp119;struct Cyc_Absyn_Exp*_tmp11B;union Cyc_Absyn_Constraint*_tmp11C;unsigned int _tmp11D;void*_tmp11F;struct Cyc_List_List*_tmp121;unsigned int _tmp122;struct Cyc_Absyn_PtrAtts _tmp124;struct Cyc_Absyn_Tqual _tmp125;unsigned int _tmp127;struct Cyc_List_List*_tmp128;_LL7D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp117=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp117->tag != 0)goto _LL7F;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LL7E:
# 736
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp118,_tmp119),atts,tms->tl);_LL7F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp11A=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11A->tag != 1)goto _LL81;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;_tmp11D=_tmp11A->f3;}}_LL80:
# 739
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp11B,_tmp11C,_tmp11D),atts,tms->tl);_LL81: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp11E=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp11E->tag != 3)goto _LL83;else{_tmp11F=(void*)_tmp11E->f1;}}_LL82: {
# 742
void*_tmp129=_tmp11F;struct Cyc_List_List*_tmp12B;int _tmp12C;struct Cyc_Absyn_VarargInfo*_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;unsigned int _tmp131;_LL8A: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp12A=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp12A->tag != 1)goto _LL8C;else{_tmp12B=_tmp12A->f1;_tmp12C=_tmp12A->f2;_tmp12D=_tmp12A->f3;_tmp12E=(void*)_tmp12A->f4;_tmp12F=_tmp12A->f5;}}_LL8B: {
# 744
struct Cyc_List_List*typvars=0;
# 746
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp8FE;fn_atts=((_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((_tmp8FE->hd=(void*)as->hd,((_tmp8FE->tl=fn_atts,_tmp8FE))))));}else{
# 751
struct Cyc_List_List*_tmp8FF;new_atts=((_tmp8FF=_cycalloc(sizeof(*_tmp8FF)),((_tmp8FF->hd=(void*)as->hd,((_tmp8FF->tl=new_atts,_tmp8FF))))));}}}
# 754
if(tms->tl != 0){
void*_tmp134=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp136;_LL8F: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp135=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp134;if(_tmp135->tag != 4)goto _LL91;else{_tmp136=_tmp135->f1;}}_LL90:
# 757
 typvars=_tmp136;
tms=tms->tl;
goto _LL8E;_LL91:;_LL92:
# 761
 goto _LL8E;_LL8E:;}
# 765
if(((((!_tmp12C  && _tmp12D == 0) && _tmp12B != 0) && _tmp12B->tl == 0) && (*((struct _tuple8*)_tmp12B->hd)).f1 == 0) && (*((struct _tuple8*)_tmp12B->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 770
_tmp12B=0;{
# 773
struct Cyc_List_List*_tmp137=Cyc_Parse_get_arg_tags(_tmp12B);
# 775
if(_tmp137 != 0)
t=Cyc_Parse_substitute_tags(_tmp137,t);
t=Cyc_Parse_array2ptr(t,0);
# 780
{struct Cyc_List_List*_tmp138=_tmp12B;for(0;_tmp138 != 0;_tmp138=_tmp138->tl){
struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Tqual _tmp13B;void**_tmp13C;struct _tuple8*_tmp139=(struct _tuple8*)_tmp138->hd;_tmp13A=_tmp139->f1;_tmp13B=_tmp139->f2;_tmp13C=(void**)& _tmp139->f3;
if(_tmp137 != 0)
*_tmp13C=Cyc_Parse_substitute_tags(_tmp137,*_tmp13C);
*_tmp13C=Cyc_Parse_array2ptr(*_tmp13C,1);}}
# 792
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp12E,tq,t,_tmp12B,_tmp12C,_tmp12D,_tmp12F,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL8C: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp130=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp129;if(_tmp130->tag != 0)goto _LL89;else{_tmp131=_tmp130->f2;}}_LL8D: {
# 798
const char*_tmp902;void*_tmp901;(_tmp901=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp131,((_tmp902="function declaration without parameter types",_tag_dyneither(_tmp902,sizeof(char),45))),_tag_dyneither(_tmp901,sizeof(void*),0)));}_LL89:;}_LL83: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp120=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp120->tag != 4)goto _LL85;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL84:
# 805
 if(tms->tl == 0){
struct _tuple13 _tmp903;return(_tmp903.f1=tq,((_tmp903.f2=t,((_tmp903.f3=_tmp121,((_tmp903.f4=atts,_tmp903)))))));}{
# 810
const char*_tmp906;void*_tmp905;(_tmp905=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp122,((_tmp906="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp906,sizeof(char),68))),_tag_dyneither(_tmp905,sizeof(void*),0)));};_LL85: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp123=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp123->tag != 2)goto _LL87;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LL86: {
# 813
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp90C;struct Cyc_Absyn_PtrInfo _tmp90B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp90A;return Cyc_Parse_apply_tms(_tmp125,(void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90C.tag=5,((_tmp90C.f1=((_tmp90B.elt_typ=t,((_tmp90B.elt_tq=tq,((_tmp90B.ptr_atts=_tmp124,_tmp90B)))))),_tmp90C)))),_tmp90A)))),atts,tms->tl);}_LL87: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp126=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp116;if(_tmp126->tag != 5)goto _LL7C;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LL88:
# 818
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp128),tms->tl);_LL7C:;};}
# 824
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 833
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp146;struct Cyc_Absyn_Tqual _tmp147;void*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_List_List*_tmp14A;struct _tuple15*_tmp145=t;_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;_tmp148=_tmp145->f3;_tmp149=_tmp145->f4;_tmp14A=_tmp145->f5;
# 836
Cyc_Lex_register_typedef(_tmp146);{
# 838
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp14B=_tmp148;struct Cyc_Core_Opt*_tmp14D;_LL94: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14B;if(_tmp14C->tag != 1)goto _LL96;else{_tmp14D=_tmp14C->f1;}}_LL95:
# 842
 type=0;
if(_tmp14D == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp14D;}
goto _LL93;_LL96:;_LL97:
 kind=0;type=_tmp148;goto _LL93;_LL93:;}{
# 848
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp912;struct Cyc_Absyn_Typedefdecl*_tmp911;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp910;return Cyc_Absyn_new_decl((void*)((_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910[0]=((_tmp912.tag=9,((_tmp912.f1=((_tmp911=_cycalloc(sizeof(*_tmp911)),((_tmp911->name=_tmp146,((_tmp911->tvs=_tmp149,((_tmp911->kind=kind,((_tmp911->defn=type,((_tmp911->atts=_tmp14A,((_tmp911->tq=_tmp147,_tmp911)))))))))))))),_tmp912)))),_tmp910)))),loc);};};}
# 855
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp915;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp914;return Cyc_Absyn_new_stmt((void*)((_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=12,((_tmp915.f1=d,((_tmp915.f2=s,_tmp915)))))),_tmp914)))),d->loc);}
# 861
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 870
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 874
struct _RegionHandle _tmp153=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp153;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp155;struct Cyc_Parse_Type_specifier _tmp156;struct Cyc_List_List*_tmp157;struct Cyc_Parse_Declaration_spec _tmp154=ds;_tmp155=_tmp154.tq;_tmp156=_tmp154.type_specs;_tmp157=_tmp154.attributes;
if(_tmp155.loc == 0)_tmp155.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp916;Cyc_Parse_err(((_tmp916="inline is allowed only on function definitions",_tag_dyneither(_tmp916,sizeof(char),47))),loc);}{
# 880
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
# 896
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 900
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 908
void*_tmp159=Cyc_Parse_collapse_type_specifiers(_tmp156,loc);
if(declarators == 0){
# 912
void*_tmp15A=_tmp159;struct Cyc_Absyn_Aggrdecl*_tmp15D;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Datatypedecl*_tmp163;enum Cyc_Absyn_AggrKind _tmp165;struct _tuple0*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Datatypedecl**_tmp169;struct _tuple0*_tmp16B;int _tmp16C;struct Cyc_List_List*_tmp16D;struct _tuple0*_tmp16F;struct Cyc_List_List*_tmp171;_LLA1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15B->tag != 26)goto _LLA3;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp15C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp15B->f1)->r;if(_tmp15C->tag != 0)goto _LLA3;else{_tmp15D=_tmp15C->f1;}}}_LLA2:
# 914
 _tmp15D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp15D->attributes,_tmp157);
_tmp15D->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp91C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp91B;struct Cyc_List_List*_tmp91A;struct Cyc_List_List*_tmp175=(_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->hd=Cyc_Absyn_new_decl((void*)((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91B.tag=6,((_tmp91B.f1=_tmp15D,_tmp91B)))),_tmp91C)))),loc),((_tmp91A->tl=0,_tmp91A)))));_npop_handler(0);return _tmp175;};_LLA3: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp15E->tag != 26)goto _LLA5;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp15F=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp15E->f1)->r;if(_tmp15F->tag != 1)goto _LLA5;else{_tmp160=_tmp15F->f1;}}}_LLA4:
# 918
 if(_tmp157 != 0){const char*_tmp91D;Cyc_Parse_err(((_tmp91D="attributes on enum not supported",_tag_dyneither(_tmp91D,sizeof(char),33))),loc);}
_tmp160->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp923;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp922;struct Cyc_List_List*_tmp921;struct Cyc_List_List*_tmp17A=(_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921->hd=Cyc_Absyn_new_decl((void*)((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923[0]=((_tmp922.tag=8,((_tmp922.f1=_tmp160,_tmp922)))),_tmp923)))),loc),((_tmp921->tl=0,_tmp921)))));_npop_handler(0);return _tmp17A;};_LLA5: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15A;if(_tmp161->tag != 26)goto _LLA7;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp162=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp161->f1)->r;if(_tmp162->tag != 2)goto _LLA7;else{_tmp163=_tmp162->f1;}}}_LLA6:
# 922
 if(_tmp157 != 0){const char*_tmp924;Cyc_Parse_err(((_tmp924="attributes on datatypes not supported",_tag_dyneither(_tmp924,sizeof(char),38))),loc);}
_tmp163->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp92A;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp929;struct Cyc_List_List*_tmp928;struct Cyc_List_List*_tmp17F=(_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928->hd=Cyc_Absyn_new_decl((void*)((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A[0]=((_tmp929.tag=7,((_tmp929.f1=_tmp163,_tmp929)))),_tmp92A)))),loc),((_tmp928->tl=0,_tmp928)))));_npop_handler(0);return _tmp17F;};_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15A;if(_tmp164->tag != 11)goto _LLA9;else{if((((_tmp164->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA9;_tmp165=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f1;_tmp166=((struct _tuple2)(((_tmp164->f1).aggr_info).UnknownAggr).val).f2;_tmp167=(_tmp164->f1).targs;}}_LLA8: {
# 926
struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp167);
struct Cyc_Absyn_Aggrdecl*_tmp92B;struct Cyc_Absyn_Aggrdecl*_tmp181=(_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->kind=_tmp165,((_tmp92B->sc=s,((_tmp92B->name=_tmp166,((_tmp92B->tvs=_tmp180,((_tmp92B->impl=0,((_tmp92B->attributes=0,_tmp92B)))))))))))));
if(_tmp157 != 0){const char*_tmp92C;Cyc_Parse_err(((_tmp92C="bad attributes on type declaration",_tag_dyneither(_tmp92C,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp932;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp931;struct Cyc_List_List*_tmp930;struct Cyc_List_List*_tmp186=(_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930->hd=Cyc_Absyn_new_decl((void*)((_tmp932=_cycalloc(sizeof(*_tmp932)),((_tmp932[0]=((_tmp931.tag=6,((_tmp931.f1=_tmp181,_tmp931)))),_tmp932)))),loc),((_tmp930->tl=0,_tmp930)))));_npop_handler(0);return _tmp186;};}_LLA9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp168->tag != 3)goto _LLAB;else{if((((_tmp168->f1).datatype_info).KnownDatatype).tag != 2)goto _LLAB;_tmp169=(struct Cyc_Absyn_Datatypedecl**)(((_tmp168->f1).datatype_info).KnownDatatype).val;}}_LLAA:
# 931
 if(_tmp157 != 0){const char*_tmp933;Cyc_Parse_err(((_tmp933="bad attributes on datatype",_tag_dyneither(_tmp933,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp939;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp938;struct Cyc_List_List*_tmp937;struct Cyc_List_List*_tmp18C=(_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937->hd=Cyc_Absyn_new_decl((void*)((_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[0]=((_tmp938.tag=7,((_tmp938.f1=*_tmp169,_tmp938)))),_tmp939)))),loc),((_tmp937->tl=0,_tmp937)))));_npop_handler(0);return _tmp18C;};_LLAB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15A;if(_tmp16A->tag != 3)goto _LLAD;else{if((((_tmp16A->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLAD;_tmp16B=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).name;_tmp16C=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp16A->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp16D=(_tmp16A->f1).targs;}}_LLAC: {
# 934
struct Cyc_List_List*_tmp18D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp16D);
struct Cyc_Absyn_Decl*_tmp18E=Cyc_Absyn_datatype_decl(s,_tmp16B,_tmp18D,0,_tmp16C,loc);
if(_tmp157 != 0){const char*_tmp93A;Cyc_Parse_err(((_tmp93A="bad attributes on datatype",_tag_dyneither(_tmp93A,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp93B;struct Cyc_List_List*_tmp191=(_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B->hd=_tmp18E,((_tmp93B->tl=0,_tmp93B)))));_npop_handler(0);return _tmp191;};}_LLAD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15A;if(_tmp16E->tag != 13)goto _LLAF;else{_tmp16F=_tmp16E->f1;}}_LLAE: {
# 939
struct Cyc_Absyn_Enumdecl*_tmp93C;struct Cyc_Absyn_Enumdecl*_tmp192=(_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C->sc=s,((_tmp93C->name=_tmp16F,((_tmp93C->fields=0,_tmp93C)))))));
if(_tmp157 != 0){const char*_tmp93D;Cyc_Parse_err(((_tmp93D="bad attributes on enum",_tag_dyneither(_tmp93D,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp947;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp946;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp945;struct Cyc_List_List*_tmp944;struct Cyc_List_List*_tmp198=(_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944->hd=((_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947->r=(void*)((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=8,((_tmp946.f1=_tmp192,_tmp946)))),_tmp945)))),((_tmp947->loc=loc,_tmp947)))))),((_tmp944->tl=0,_tmp944)))));_npop_handler(0);return _tmp198;};}_LLAF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15A;if(_tmp170->tag != 14)goto _LLB1;else{_tmp171=_tmp170->f1;}}_LLB0: {
# 945
struct Cyc_Core_Opt*_tmp94A;struct Cyc_Absyn_Enumdecl*_tmp949;struct Cyc_Absyn_Enumdecl*_tmp19A=(_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949->sc=s,((_tmp949->name=Cyc_Parse_gensym_enum(),((_tmp949->fields=((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->v=_tmp171,_tmp94A)))),_tmp949)))))));
if(_tmp157 != 0){const char*_tmp94B;Cyc_Parse_err(((_tmp94B="bad attributes on enum",_tag_dyneither(_tmp94B,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp955;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp954;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp953;struct Cyc_List_List*_tmp952;struct Cyc_List_List*_tmp1A0=(_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952->hd=((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->r=(void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=8,((_tmp954.f1=_tmp19A,_tmp954)))),_tmp953)))),((_tmp955->loc=loc,_tmp955)))))),((_tmp952->tl=0,_tmp952)))));_npop_handler(0);return _tmp1A0;};}_LLB1:;_LLB2:
{const char*_tmp956;Cyc_Parse_err(((_tmp956="missing declarator",_tag_dyneither(_tmp956,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1A4=0;_npop_handler(0);return _tmp1A4;};_LLA0:;}else{
# 952
struct Cyc_List_List*_tmp1A5=Cyc_Parse_apply_tmss(mkrgn,_tmp155,_tmp159,declarators,_tmp157);
if(istypedef){
# 957
if(!exps_empty){
const char*_tmp957;Cyc_Parse_err(((_tmp957="initializer in typedef declaration",_tag_dyneither(_tmp957,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1A5);
struct Cyc_List_List*_tmp1A7=decls;_npop_handler(0);return _tmp1A7;};}else{
# 963
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1A8=_tmp1A5;for(0;_tmp1A8 != 0;(_tmp1A8=_tmp1A8->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple0*_tmp1AA;struct Cyc_Absyn_Tqual _tmp1AB;void*_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_List_List*_tmp1AE;struct _tuple15*_tmp1A9=(struct _tuple15*)_tmp1A8->hd;_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;_tmp1AC=_tmp1A9->f3;_tmp1AD=_tmp1A9->f4;_tmp1AE=_tmp1A9->f5;
if(_tmp1AD != 0){
const char*_tmp95A;void*_tmp959;(_tmp959=0,Cyc_Tcutil_warn(loc,((_tmp95A="bad type params, ignoring",_tag_dyneither(_tmp95A,sizeof(char),26))),_tag_dyneither(_tmp959,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp95D;void*_tmp95C;(_tmp95C=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp95D="unexpected NULL in parse!",_tag_dyneither(_tmp95D,sizeof(char),26))),_tag_dyneither(_tmp95C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1B3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1B4=Cyc_Absyn_new_vardecl(_tmp1AA,_tmp1AC,_tmp1B3);
_tmp1B4->tq=_tmp1AB;
_tmp1B4->sc=s;
_tmp1B4->attributes=_tmp1AE;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp963;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp962;struct Cyc_Absyn_Decl*_tmp961;struct Cyc_Absyn_Decl*_tmp1B5=(_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961->r=(void*)((_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963[0]=((_tmp962.tag=0,((_tmp962.f1=_tmp1B4,_tmp962)))),_tmp963)))),((_tmp961->loc=loc,_tmp961)))));
struct Cyc_List_List*_tmp964;decls=((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->hd=_tmp1B5,((_tmp964->tl=decls,_tmp964))))));};};}}{
# 978
struct Cyc_List_List*_tmp1BA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1BA;};}}};};};};}
# 875
;_pop_region(mkrgn);}
# 985
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)
switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB3:
 return& Cyc_Tcutil_ak;case 'M': _LLB4:
 return& Cyc_Tcutil_mk;case 'B': _LLB5:
 return& Cyc_Tcutil_bk;case 'R': _LLB6:
 return& Cyc_Tcutil_rk;case 'E': _LLB7:
 return& Cyc_Tcutil_ek;case 'I': _LLB8:
 return& Cyc_Tcutil_ik;case 'U': _LLB9:
# 995
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLBB:
 return& Cyc_Tcutil_urk;case 'A': _LLBC:
 return& Cyc_Tcutil_uak;case 'M': _LLBD:
 return& Cyc_Tcutil_umk;case 'B': _LLBE:
 return& Cyc_Tcutil_ubk;default: _LLBF:
 break;}
# 1002
break;case 'T': _LLBA:
# 1004
 switch(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))){case 'R': _LLC2:
 return& Cyc_Tcutil_trk;case 'A': _LLC3:
 return& Cyc_Tcutil_tak;case 'M': _LLC4:
 return& Cyc_Tcutil_tmk;case 'B': _LLC5:
 return& Cyc_Tcutil_tbk;default: _LLC6:
 break;}
# 1011
break;default: _LLC1:
 break;}
# 1014
{const char*_tmp969;void*_tmp968[2];struct Cyc_String_pa_PrintArg_struct _tmp967;struct Cyc_Int_pa_PrintArg_struct _tmp966;Cyc_Parse_err((struct _dyneither_ptr)((_tmp966.tag=1,((_tmp966.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp968[0]=& _tmp967,((_tmp968[1]=& _tmp966,Cyc_aprintf(((_tmp969="bad kind: %s; strlen=%d",_tag_dyneither(_tmp969,sizeof(char),24))),_tag_dyneither(_tmp968,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1BF=p->r;struct _tuple0*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C3;struct Cyc_Absyn_Pat*_tmp1C6;enum Cyc_Absyn_Sign _tmp1C9;int _tmp1CA;char _tmp1CC;struct _dyneither_ptr _tmp1CE;int _tmp1CF;struct _tuple0*_tmp1D1;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;_LLCA: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1C0=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C0->tag != 14)goto _LLCC;else{_tmp1C1=_tmp1C0->f1;}}_LLCB:
 return Cyc_Absyn_unknownid_exp(_tmp1C1,p->loc);_LLCC: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C2->tag != 2)goto _LLCE;else{_tmp1C3=_tmp1C2->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1C2->f2)->r;if(_tmp1C4->tag != 0)goto _LLCE;};}}_LLCD:
# 1023
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1C3->name,p->loc),p->loc);_LLCE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1C5=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C5->tag != 5)goto _LLD0;else{_tmp1C6=_tmp1C5->f1;}}_LLCF:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1C6),p->loc);_LLD0: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C7->tag != 8)goto _LLD2;}_LLD1:
 return Cyc_Absyn_null_exp(p->loc);_LLD2: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1C8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1C8->tag != 9)goto _LLD4;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C8->f2;}}_LLD3:
 return Cyc_Absyn_int_exp(_tmp1C9,_tmp1CA,p->loc);_LLD4: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1CB=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1CB->tag != 10)goto _LLD6;else{_tmp1CC=_tmp1CB->f1;}}_LLD5:
 return Cyc_Absyn_char_exp(_tmp1CC,p->loc);_LLD6: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1CD=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1CD->tag != 11)goto _LLD8;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}_LLD7:
 return Cyc_Absyn_float_exp(_tmp1CE,_tmp1CF,p->loc);_LLD8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1D0->tag != 15)goto _LLDA;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;if(_tmp1D0->f3 != 0)goto _LLDA;}}_LLD9: {
# 1030
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1D1,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1D2);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLDA: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1BF;if(_tmp1D3->tag != 16)goto _LLDC;else{_tmp1D4=_tmp1D3->f1;}}_LLDB:
 return _tmp1D4;_LLDC:;_LLDD:
# 1035
{const char*_tmp96A;Cyc_Parse_err(((_tmp96A="cannot mix patterns and expressions in case",_tag_dyneither(_tmp96A,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC9:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1D7[14]="$(sign_t,int)";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 14}};
# 1089
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1D8=yy1;struct _tuple5 _tmp1D9;_LLDF: if((_tmp1D8.Int_tok).tag != 1)goto _LLE1;_tmp1D9=(struct _tuple5)(_tmp1D8.Int_tok).val;_LLE0:
# 1093
 yyzzz=_tmp1D9;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLDE:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp96B;return((_tmp96B.Int_tok).val=yy1,(((_tmp96B.Int_tok).tag=1,_tmp96B)));}static char _tmp1DC[5]="char";
# 1088 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1DC,_tmp1DC,_tmp1DC + 5}};
# 1090
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1DD=yy1;char _tmp1DE;_LLE4: if((_tmp1DD.Char_tok).tag != 2)goto _LLE6;_tmp1DE=(char)(_tmp1DD.Char_tok).val;_LLE5:
# 1094
 yyzzz=_tmp1DE;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLE3:;}
# 1098
return yyzzz;}
# 1100
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp96C;return((_tmp96C.Char_tok).val=yy1,(((_tmp96C.Char_tok).tag=2,_tmp96C)));}static char _tmp1E1[13]="string_t<`H>";
# 1089 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1E1,_tmp1E1,_tmp1E1 + 13}};
# 1091
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1E2=yy1;struct _dyneither_ptr _tmp1E3;_LLE9: if((_tmp1E2.String_tok).tag != 3)goto _LLEB;_tmp1E3=(struct _dyneither_ptr)(_tmp1E2.String_tok).val;_LLEA:
# 1095
 yyzzz=_tmp1E3;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE8:;}
# 1099
return yyzzz;}
# 1101
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp96D;return((_tmp96D.String_tok).val=yy1,(((_tmp96D.String_tok).tag=3,_tmp96D)));}static char _tmp1E6[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1092 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 56}};
# 1094
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1E7=yy1;struct _tuple21*_tmp1E8;_LLEE: if((_tmp1E7.YY1).tag != 7)goto _LLF0;_tmp1E8=(struct _tuple21*)(_tmp1E7.YY1).val;_LLEF:
# 1098
 yyzzz=_tmp1E8;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLED:;}
# 1102
return yyzzz;}
# 1104
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp96E;return((_tmp96E.YY1).val=yy1,(((_tmp96E.YY1).tag=7,_tmp96E)));}static char _tmp1EB[19]="conref_t<bounds_t>";
# 1093 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1EB,_tmp1EB,_tmp1EB + 19}};
# 1095
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1EC=yy1;union Cyc_Absyn_Constraint*_tmp1ED;_LLF3: if((_tmp1EC.YY2).tag != 8)goto _LLF5;_tmp1ED=(union Cyc_Absyn_Constraint*)(_tmp1EC.YY2).val;_LLF4:
# 1099
 yyzzz=_tmp1ED;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLF2:;}
# 1103
return yyzzz;}
# 1105
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp96F;return((_tmp96F.YY2).val=yy1,(((_tmp96F.YY2).tag=8,_tmp96F)));}static char _tmp1F0[28]="list_t<offsetof_field_t,`H>";
# 1094 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1F0,_tmp1F0,_tmp1F0 + 28}};
# 1096
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F1=yy1;struct Cyc_List_List*_tmp1F2;_LLF8: if((_tmp1F1.YY3).tag != 9)goto _LLFA;_tmp1F2=(struct Cyc_List_List*)(_tmp1F1.YY3).val;_LLF9:
# 1100
 yyzzz=_tmp1F2;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF7:;}
# 1104
return yyzzz;}
# 1106
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp970;return((_tmp970.YY3).val=yy1,(((_tmp970.YY3).tag=9,_tmp970)));}static char _tmp1F5[6]="exp_t";
# 1095 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 6}};
# 1097
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1F6=yy1;struct Cyc_Absyn_Exp*_tmp1F7;_LLFD: if((_tmp1F6.YY4).tag != 10)goto _LLFF;_tmp1F7=(struct Cyc_Absyn_Exp*)(_tmp1F6.YY4).val;_LLFE:
# 1101
 yyzzz=_tmp1F7;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLFC:;}
# 1105
return yyzzz;}
# 1107
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp971;return((_tmp971.YY4).val=yy1,(((_tmp971.YY4).tag=10,_tmp971)));}static char _tmp1FA[10]="exp_opt_t";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1FA,_tmp1FA,_tmp1FA + 10}};
# 1105
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1FB=yy1;struct Cyc_Absyn_Exp*_tmp1FC;_LL102: if((_tmp1FB.YY5).tag != 11)goto _LL104;_tmp1FC=(struct Cyc_Absyn_Exp*)(_tmp1FB.YY5).val;_LL103:
# 1109
 yyzzz=_tmp1FC;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY5);_LL101:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp972;return((_tmp972.YY5).val=yy1,(((_tmp972.YY5).tag=11,_tmp972)));}static char _tmp1FF[17]="list_t<exp_t,`H>";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 17}};
# 1106
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp200=yy1;struct Cyc_List_List*_tmp201;_LL107: if((_tmp200.YY6).tag != 12)goto _LL109;_tmp201=(struct Cyc_List_List*)(_tmp200.YY6).val;_LL108:
# 1110
 yyzzz=_tmp201;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL106:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp973;return((_tmp973.YY6).val=yy1,(((_tmp973.YY6).tag=12,_tmp973)));}static char _tmp204[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 47}};
# 1107
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp205=yy1;struct Cyc_List_List*_tmp206;_LL10C: if((_tmp205.YY7).tag != 13)goto _LL10E;_tmp206=(struct Cyc_List_List*)(_tmp205.YY7).val;_LL10D:
# 1111
 yyzzz=_tmp206;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL10B:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp974;return((_tmp974.YY7).val=yy1,(((_tmp974.YY7).tag=13,_tmp974)));}static char _tmp209[9]="primop_t";
# 1106 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp209,_tmp209,_tmp209 + 9}};
# 1108
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp20A=yy1;enum Cyc_Absyn_Primop _tmp20B;_LL111: if((_tmp20A.YY8).tag != 14)goto _LL113;_tmp20B=(enum Cyc_Absyn_Primop)(_tmp20A.YY8).val;_LL112:
# 1112
 yyzzz=_tmp20B;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL110:;}
# 1116
return yyzzz;}
# 1118
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp975;return((_tmp975.YY8).val=yy1,(((_tmp975.YY8).tag=14,_tmp975)));}static char _tmp20E[19]="opt_t<primop_t,`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 19}};
# 1109
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp20F=yy1;struct Cyc_Core_Opt*_tmp210;_LL116: if((_tmp20F.YY9).tag != 15)goto _LL118;_tmp210=(struct Cyc_Core_Opt*)(_tmp20F.YY9).val;_LL117:
# 1113
 yyzzz=_tmp210;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL115:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp976;return((_tmp976.YY9).val=yy1,(((_tmp976.YY9).tag=15,_tmp976)));}static char _tmp213[7]="qvar_t";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 7}};
# 1110
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp214=yy1;struct _tuple0*_tmp215;_LL11B: if((_tmp214.QualId_tok).tag != 5)goto _LL11D;_tmp215=(struct _tuple0*)(_tmp214.QualId_tok).val;_LL11C:
# 1114
 yyzzz=_tmp215;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL11A:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp977;return((_tmp977.QualId_tok).val=yy1,(((_tmp977.QualId_tok).tag=5,_tmp977)));}static char _tmp218[7]="stmt_t";
# 1111 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp218,_tmp218,_tmp218 + 7}};
# 1113
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp219=yy1;struct Cyc_Absyn_Stmt*_tmp21A;_LL120: if((_tmp219.YY10).tag != 16)goto _LL122;_tmp21A=(struct Cyc_Absyn_Stmt*)(_tmp219.YY10).val;_LL121:
# 1117
 yyzzz=_tmp21A;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11F:;}
# 1121
return yyzzz;}
# 1123
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp978;return((_tmp978.YY10).val=yy1,(((_tmp978.YY10).tag=16,_tmp978)));}static char _tmp21D[27]="list_t<switch_clause_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp21D,_tmp21D,_tmp21D + 27}};
# 1117
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21E=yy1;struct Cyc_List_List*_tmp21F;_LL125: if((_tmp21E.YY11).tag != 17)goto _LL127;_tmp21F=(struct Cyc_List_List*)(_tmp21E.YY11).val;_LL126:
# 1121
 yyzzz=_tmp21F;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL124:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp979;return((_tmp979.YY11).val=yy1,(((_tmp979.YY11).tag=17,_tmp979)));}static char _tmp222[6]="pat_t";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 6}};
# 1118
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp223=yy1;struct Cyc_Absyn_Pat*_tmp224;_LL12A: if((_tmp223.YY12).tag != 18)goto _LL12C;_tmp224=(struct Cyc_Absyn_Pat*)(_tmp223.YY12).val;_LL12B:
# 1122
 yyzzz=_tmp224;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL129:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp97A;return((_tmp97A.YY12).val=yy1,(((_tmp97A.YY12).tag=18,_tmp97A)));}static char _tmp227[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp227,_tmp227,_tmp227 + 28}};
# 1123
struct _tuple22*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp228=yy1;struct _tuple22*_tmp229;_LL12F: if((_tmp228.YY13).tag != 19)goto _LL131;_tmp229=(struct _tuple22*)(_tmp228.YY13).val;_LL130:
# 1127
 yyzzz=_tmp229;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL12E:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp97B;return((_tmp97B.YY13).val=yy1,(((_tmp97B.YY13).tag=19,_tmp97B)));}static char _tmp22C[17]="list_t<pat_t,`H>";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp22C,_tmp22C,_tmp22C + 17}};
# 1124
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22D=yy1;struct Cyc_List_List*_tmp22E;_LL134: if((_tmp22D.YY14).tag != 20)goto _LL136;_tmp22E=(struct Cyc_List_List*)(_tmp22D.YY14).val;_LL135:
# 1128
 yyzzz=_tmp22E;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL133:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97C;return((_tmp97C.YY14).val=yy1,(((_tmp97C.YY14).tag=20,_tmp97C)));}static char _tmp231[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp231,_tmp231,_tmp231 + 36}};
# 1125
struct _tuple23*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp232=yy1;struct _tuple23*_tmp233;_LL139: if((_tmp232.YY15).tag != 21)goto _LL13B;_tmp233=(struct _tuple23*)(_tmp232.YY15).val;_LL13A:
# 1129
 yyzzz=_tmp233;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL138:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp97D;return((_tmp97D.YY15).val=yy1,(((_tmp97D.YY15).tag=21,_tmp97D)));}static char _tmp236[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp236,_tmp236,_tmp236 + 47}};
# 1126
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp237=yy1;struct Cyc_List_List*_tmp238;_LL13E: if((_tmp237.YY16).tag != 22)goto _LL140;_tmp238=(struct Cyc_List_List*)(_tmp237.YY16).val;_LL13F:
# 1130
 yyzzz=_tmp238;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL13D:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp97E;return((_tmp97E.YY16).val=yy1,(((_tmp97E.YY16).tag=22,_tmp97E)));}static char _tmp23B[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 58}};
# 1127
struct _tuple22*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp23C=yy1;struct _tuple22*_tmp23D;_LL143: if((_tmp23C.YY17).tag != 23)goto _LL145;_tmp23D=(struct _tuple22*)(_tmp23C.YY17).val;_LL144:
# 1131
 yyzzz=_tmp23D;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL142:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp97F;return((_tmp97F.YY17).val=yy1,(((_tmp97F.YY17).tag=23,_tmp97F)));}static char _tmp240[9]="fndecl_t";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 9}};
# 1128
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp241=yy1;struct Cyc_Absyn_Fndecl*_tmp242;_LL148: if((_tmp241.YY18).tag != 24)goto _LL14A;_tmp242=(struct Cyc_Absyn_Fndecl*)(_tmp241.YY18).val;_LL149:
# 1132
 yyzzz=_tmp242;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL147:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp980;return((_tmp980.YY18).val=yy1,(((_tmp980.YY18).tag=24,_tmp980)));}static char _tmp245[18]="list_t<decl_t,`H>";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp245,_tmp245,_tmp245 + 18}};
# 1129
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp246=yy1;struct Cyc_List_List*_tmp247;_LL14D: if((_tmp246.YY19).tag != 25)goto _LL14F;_tmp247=(struct Cyc_List_List*)(_tmp246.YY19).val;_LL14E:
# 1133
 yyzzz=_tmp247;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL14C:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp981;return((_tmp981.YY19).val=yy1,(((_tmp981.YY19).tag=25,_tmp981)));}static char _tmp24A[12]="decl_spec_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp24A,_tmp24A,_tmp24A + 12}};
# 1131
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp24B=yy1;struct Cyc_Parse_Declaration_spec _tmp24C;_LL152: if((_tmp24B.YY20).tag != 26)goto _LL154;_tmp24C=(struct Cyc_Parse_Declaration_spec)(_tmp24B.YY20).val;_LL153:
# 1135
 yyzzz=_tmp24C;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL151:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp982;return((_tmp982.YY20).val=yy1,(((_tmp982.YY20).tag=26,_tmp982)));}static char _tmp24F[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 31}};
# 1132
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp250=yy1;struct _tuple11 _tmp251;_LL157: if((_tmp250.YY21).tag != 27)goto _LL159;_tmp251=(struct _tuple11)(_tmp250.YY21).val;_LL158:
# 1136
 yyzzz=_tmp251;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL156:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp983;return((_tmp983.YY21).val=yy1,(((_tmp983.YY21).tag=27,_tmp983)));}static char _tmp254[23]="declarator_list_t<`yy>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp254,_tmp254,_tmp254 + 23}};
# 1133
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp255=yy1;struct _tuple12*_tmp256;_LL15C: if((_tmp255.YY22).tag != 28)goto _LL15E;_tmp256=(struct _tuple12*)(_tmp255.YY22).val;_LL15D:
# 1137
 yyzzz=_tmp256;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL15B:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp984;return((_tmp984.YY22).val=yy1,(((_tmp984.YY22).tag=28,_tmp984)));}static char _tmp259[19]="storage_class_t@`H";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp259,_tmp259,_tmp259 + 19}};
# 1134
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp25A=yy1;enum Cyc_Parse_Storage_class*_tmp25B;_LL161: if((_tmp25A.YY23).tag != 29)goto _LL163;_tmp25B=(enum Cyc_Parse_Storage_class*)(_tmp25A.YY23).val;_LL162:
# 1138
 yyzzz=_tmp25B;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL160:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp985;return((_tmp985.YY23).val=yy1,(((_tmp985.YY23).tag=29,_tmp985)));}static char _tmp25E[17]="type_specifier_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp25E,_tmp25E,_tmp25E + 17}};
# 1135
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp25F=yy1;struct Cyc_Parse_Type_specifier _tmp260;_LL166: if((_tmp25F.YY24).tag != 30)goto _LL168;_tmp260=(struct Cyc_Parse_Type_specifier)(_tmp25F.YY24).val;_LL167:
# 1139
 yyzzz=_tmp260;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL165:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp986;return((_tmp986.YY24).val=yy1,(((_tmp986.YY24).tag=30,_tmp986)));}static char _tmp263[12]="aggr_kind_t";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp263,_tmp263,_tmp263 + 12}};
# 1137
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp264=yy1;enum Cyc_Absyn_AggrKind _tmp265;_LL16B: if((_tmp264.YY25).tag != 31)goto _LL16D;_tmp265=(enum Cyc_Absyn_AggrKind)(_tmp264.YY25).val;_LL16C:
# 1141
 yyzzz=_tmp265;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL16A:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp987;return((_tmp987.YY25).val=yy1,(((_tmp987.YY25).tag=31,_tmp987)));}static char _tmp268[8]="tqual_t";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp268,_tmp268,_tmp268 + 8}};
# 1138
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp269=yy1;struct Cyc_Absyn_Tqual _tmp26A;_LL170: if((_tmp269.YY26).tag != 32)goto _LL172;_tmp26A=(struct Cyc_Absyn_Tqual)(_tmp269.YY26).val;_LL171:
# 1142
 yyzzz=_tmp26A;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16F:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp988;return((_tmp988.YY26).val=yy1,(((_tmp988.YY26).tag=32,_tmp988)));}static char _tmp26D[23]="list_t<aggrfield_t,`H>";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp26D,_tmp26D,_tmp26D + 23}};
# 1139
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp26E=yy1;struct Cyc_List_List*_tmp26F;_LL175: if((_tmp26E.YY27).tag != 33)goto _LL177;_tmp26F=(struct Cyc_List_List*)(_tmp26E.YY27).val;_LL176:
# 1143
 yyzzz=_tmp26F;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL174:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp989;return((_tmp989.YY27).val=yy1,(((_tmp989.YY27).tag=33,_tmp989)));}static char _tmp272[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp272,_tmp272,_tmp272 + 34}};
# 1140
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp273=yy1;struct Cyc_List_List*_tmp274;_LL17A: if((_tmp273.YY28).tag != 34)goto _LL17C;_tmp274=(struct Cyc_List_List*)(_tmp273.YY28).val;_LL17B:
# 1144
 yyzzz=_tmp274;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL179:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98A;return((_tmp98A.YY28).val=yy1,(((_tmp98A.YY28).tag=34,_tmp98A)));}static char _tmp277[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp277,_tmp277,_tmp277 + 33}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp278=yy1;struct Cyc_List_List*_tmp279;_LL17F: if((_tmp278.YY29).tag != 35)goto _LL181;_tmp279=(struct Cyc_List_List*)(_tmp278.YY29).val;_LL180:
# 1145
 yyzzz=_tmp279;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL17E:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98B;return((_tmp98B.YY29).val=yy1,(((_tmp98B.YY29).tag=35,_tmp98B)));}static char _tmp27C[18]="declarator_t<`yy>";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp27C,_tmp27C,_tmp27C + 18}};
# 1142
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp27D=yy1;struct Cyc_Parse_Declarator _tmp27E;_LL184: if((_tmp27D.YY30).tag != 36)goto _LL186;_tmp27E=(struct Cyc_Parse_Declarator)(_tmp27D.YY30).val;_LL185:
# 1146
 yyzzz=_tmp27E;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL183:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp98C;return((_tmp98C.YY30).val=yy1,(((_tmp98C.YY30).tag=36,_tmp98C)));}static char _tmp281[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp281,_tmp281,_tmp281 + 45}};
# 1143
struct _tuple24*Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp282=yy1;struct _tuple24*_tmp283;_LL189: if((_tmp282.YY31).tag != 37)goto _LL18B;_tmp283=(struct _tuple24*)(_tmp282.YY31).val;_LL18A:
# 1147
 yyzzz=_tmp283;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL188:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp98D;return((_tmp98D.YY31).val=yy1,(((_tmp98D.YY31).tag=37,_tmp98D)));}static char _tmp286[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp286,_tmp286,_tmp286 + 57}};
# 1144
struct Cyc_List_List*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp287=yy1;struct Cyc_List_List*_tmp288;_LL18E: if((_tmp287.YY32).tag != 38)goto _LL190;_tmp288=(struct Cyc_List_List*)(_tmp287.YY32).val;_LL18F:
# 1148
 yyzzz=_tmp288;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL18D:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp98E;return((_tmp98E.YY32).val=yy1,(((_tmp98E.YY32).tag=38,_tmp98E)));}static char _tmp28B[26]="abstractdeclarator_t<`yy>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp28B,_tmp28B,_tmp28B + 26}};
# 1145
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp28C=yy1;struct Cyc_Parse_Abstractdeclarator _tmp28D;_LL193: if((_tmp28C.YY33).tag != 39)goto _LL195;_tmp28D=(struct Cyc_Parse_Abstractdeclarator)(_tmp28C.YY33).val;_LL194:
# 1149
 yyzzz=_tmp28D;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL192:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp98F;return((_tmp98F.YY33).val=yy1,(((_tmp98F.YY33).tag=39,_tmp98F)));}static char _tmp290[5]="bool";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp290,_tmp290,_tmp290 + 5}};
# 1146
int Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp291=yy1;int _tmp292;_LL198: if((_tmp291.YY34).tag != 40)goto _LL19A;_tmp292=(int)(_tmp291.YY34).val;_LL199:
# 1150
 yyzzz=_tmp292;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL197:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp990;return((_tmp990.YY34).val=yy1,(((_tmp990.YY34).tag=40,_tmp990)));}static char _tmp295[8]="scope_t";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp295,_tmp295,_tmp295 + 8}};
# 1147
enum Cyc_Absyn_Scope Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp296=yy1;enum Cyc_Absyn_Scope _tmp297;_LL19D: if((_tmp296.YY35).tag != 41)goto _LL19F;_tmp297=(enum Cyc_Absyn_Scope)(_tmp296.YY35).val;_LL19E:
# 1151
 yyzzz=_tmp297;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL19C:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp991;return((_tmp991.YY35).val=yy1,(((_tmp991.YY35).tag=41,_tmp991)));}static char _tmp29A[16]="datatypefield_t";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp29A,_tmp29A,_tmp29A + 16}};
# 1148
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp29B=yy1;struct Cyc_Absyn_Datatypefield*_tmp29C;_LL1A2: if((_tmp29B.YY36).tag != 42)goto _LL1A4;_tmp29C=(struct Cyc_Absyn_Datatypefield*)(_tmp29B.YY36).val;_LL1A3:
# 1152
 yyzzz=_tmp29C;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL1A1:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp992;return((_tmp992.YY36).val=yy1,(((_tmp992.YY36).tag=42,_tmp992)));}static char _tmp29F[27]="list_t<datatypefield_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp29F,_tmp29F,_tmp29F + 27}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2A0=yy1;struct Cyc_List_List*_tmp2A1;_LL1A7: if((_tmp2A0.YY37).tag != 43)goto _LL1A9;_tmp2A1=(struct Cyc_List_List*)(_tmp2A0.YY37).val;_LL1A8:
# 1153
 yyzzz=_tmp2A1;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A6:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp993;return((_tmp993.YY37).val=yy1,(((_tmp993.YY37).tag=43,_tmp993)));}static char _tmp2A4[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2A4,_tmp2A4,_tmp2A4 + 41}};
# 1150
struct _tuple25 Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2A5=yy1;struct _tuple25 _tmp2A6;_LL1AC: if((_tmp2A5.YY38).tag != 44)goto _LL1AE;_tmp2A6=(struct _tuple25)(_tmp2A5.YY38).val;_LL1AD:
# 1154
 yyzzz=_tmp2A6;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1AB:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp994;return((_tmp994.YY38).val=yy1,(((_tmp994.YY38).tag=44,_tmp994)));}static char _tmp2A9[17]="list_t<var_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2A9,_tmp2A9,_tmp2A9 + 17}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2AA=yy1;struct Cyc_List_List*_tmp2AB;_LL1B1: if((_tmp2AA.YY39).tag != 45)goto _LL1B3;_tmp2AB=(struct Cyc_List_List*)(_tmp2AA.YY39).val;_LL1B2:
# 1155
 yyzzz=_tmp2AB;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1B0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp995;return((_tmp995.YY39).val=yy1,(((_tmp995.YY39).tag=45,_tmp995)));}static char _tmp2AE[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2AE,_tmp2AE,_tmp2AE + 31}};
# 1152
struct _tuple8*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2AF=yy1;struct _tuple8*_tmp2B0;_LL1B6: if((_tmp2AF.YY40).tag != 46)goto _LL1B8;_tmp2B0=(struct _tuple8*)(_tmp2AF.YY40).val;_LL1B7:
# 1156
 yyzzz=_tmp2B0;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B5:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp996;return((_tmp996.YY40).val=yy1,(((_tmp996.YY40).tag=46,_tmp996)));}static char _tmp2B3[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2B3,_tmp2B3,_tmp2B3 + 42}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B4=yy1;struct Cyc_List_List*_tmp2B5;_LL1BB: if((_tmp2B4.YY41).tag != 47)goto _LL1BD;_tmp2B5=(struct Cyc_List_List*)(_tmp2B4.YY41).val;_LL1BC:
# 1157
 yyzzz=_tmp2B5;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1BA:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp997;return((_tmp997.YY41).val=yy1,(((_tmp997.YY41).tag=47,_tmp997)));}static char _tmp2B8[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2B8,_tmp2B8,_tmp2B8 + 115}};
# 1154
struct _tuple26*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2B9=yy1;struct _tuple26*_tmp2BA;_LL1C0: if((_tmp2B9.YY42).tag != 48)goto _LL1C2;_tmp2BA=(struct _tuple26*)(_tmp2B9.YY42).val;_LL1C1:
# 1158
 yyzzz=_tmp2BA;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BF:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.YY42).val=yy1,(((_tmp998.YY42).tag=48,_tmp998)));}static char _tmp2BD[18]="list_t<type_t,`H>";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2BD,_tmp2BD,_tmp2BD + 18}};
# 1155
struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BE=yy1;struct Cyc_List_List*_tmp2BF;_LL1C5: if((_tmp2BE.YY43).tag != 49)goto _LL1C7;_tmp2BF=(struct Cyc_List_List*)(_tmp2BE.YY43).val;_LL1C6:
# 1159
 yyzzz=_tmp2BF;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1C4:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.YY43).val=yy1,(((_tmp999.YY43).tag=49,_tmp999)));}static char _tmp2C2[24]="list_t<designator_t,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2C2,_tmp2C2,_tmp2C2 + 24}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C3=yy1;struct Cyc_List_List*_tmp2C4;_LL1CA: if((_tmp2C3.YY44).tag != 50)goto _LL1CC;_tmp2C4=(struct Cyc_List_List*)(_tmp2C3.YY44).val;_LL1CB:
# 1161
 yyzzz=_tmp2C4;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C9:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.YY44).val=yy1,(((_tmp99A.YY44).tag=50,_tmp99A)));}static char _tmp2C7[13]="designator_t";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2C7,_tmp2C7,_tmp2C7 + 13}};
# 1158
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2C8=yy1;void*_tmp2C9;_LL1CF: if((_tmp2C8.YY45).tag != 51)goto _LL1D1;_tmp2C9=(void*)(_tmp2C8.YY45).val;_LL1D0:
# 1162
 yyzzz=_tmp2C9;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1CE:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY45).val=yy1,(((_tmp99B.YY45).tag=51,_tmp99B)));}static char _tmp2CC[7]="kind_t";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2CC,_tmp2CC,_tmp2CC + 7}};
# 1159
struct Cyc_Absyn_Kind*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2CD=yy1;struct Cyc_Absyn_Kind*_tmp2CE;_LL1D4: if((_tmp2CD.YY46).tag != 52)goto _LL1D6;_tmp2CE=(struct Cyc_Absyn_Kind*)(_tmp2CD.YY46).val;_LL1D5:
# 1163
 yyzzz=_tmp2CE;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1D3:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY46).val=yy1,(((_tmp99C.YY46).tag=52,_tmp99C)));}static char _tmp2D1[7]="type_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2D1,_tmp2D1,_tmp2D1 + 7}};
# 1160
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D2=yy1;void*_tmp2D3;_LL1D9: if((_tmp2D2.YY47).tag != 53)goto _LL1DB;_tmp2D3=(void*)(_tmp2D2.YY47).val;_LL1DA:
# 1164
 yyzzz=_tmp2D3;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D8:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY47).val=yy1,(((_tmp99D.YY47).tag=53,_tmp99D)));}static char _tmp2D6[23]="list_t<attribute_t,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2D6,_tmp2D6,_tmp2D6 + 23}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D7=yy1;struct Cyc_List_List*_tmp2D8;_LL1DE: if((_tmp2D7.YY48).tag != 54)goto _LL1E0;_tmp2D8=(struct Cyc_List_List*)(_tmp2D7.YY48).val;_LL1DF:
# 1165
 yyzzz=_tmp2D8;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1DD:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY48).val=yy1,(((_tmp99E.YY48).tag=54,_tmp99E)));}static char _tmp2DB[12]="attribute_t";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2DB,_tmp2DB,_tmp2DB + 12}};
# 1162
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2DC=yy1;void*_tmp2DD;_LL1E3: if((_tmp2DC.YY49).tag != 55)goto _LL1E5;_tmp2DD=(void*)(_tmp2DC.YY49).val;_LL1E4:
# 1166
 yyzzz=_tmp2DD;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1E2:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY49).val=yy1,(((_tmp99F.YY49).tag=55,_tmp99F)));}static char _tmp2E0[12]="enumfield_t";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2E0,_tmp2E0,_tmp2E0 + 12}};
# 1163
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2E1=yy1;struct Cyc_Absyn_Enumfield*_tmp2E2;_LL1E8: if((_tmp2E1.YY50).tag != 56)goto _LL1EA;_tmp2E2=(struct Cyc_Absyn_Enumfield*)(_tmp2E1.YY50).val;_LL1E9:
# 1167
 yyzzz=_tmp2E2;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E7:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY50).val=yy1,(((_tmp9A0.YY50).tag=56,_tmp9A0)));}static char _tmp2E5[23]="list_t<enumfield_t,`H>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2E5,_tmp2E5,_tmp2E5 + 23}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E6=yy1;struct Cyc_List_List*_tmp2E7;_LL1ED: if((_tmp2E6.YY51).tag != 57)goto _LL1EF;_tmp2E7=(struct Cyc_List_List*)(_tmp2E6.YY51).val;_LL1EE:
# 1168
 yyzzz=_tmp2E7;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1EC:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY51).val=yy1,(((_tmp9A1.YY51).tag=57,_tmp9A1)));}static char _tmp2EA[11]="type_opt_t";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2EA,_tmp2EA,_tmp2EA + 11}};
# 1165
void*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2EB=yy1;void*_tmp2EC;_LL1F2: if((_tmp2EB.YY52).tag != 58)goto _LL1F4;_tmp2EC=(void*)(_tmp2EB.YY52).val;_LL1F3:
# 1169
 yyzzz=_tmp2EC;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1F1:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY52).val=yy1,(((_tmp9A2.YY52).tag=58,_tmp9A2)));}static char _tmp2EF[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp2EF,_tmp2EF,_tmp2EF + 31}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F0=yy1;struct Cyc_List_List*_tmp2F1;_LL1F7: if((_tmp2F0.YY53).tag != 59)goto _LL1F9;_tmp2F1=(struct Cyc_List_List*)(_tmp2F0.YY53).val;_LL1F8:
# 1170
 yyzzz=_tmp2F1;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F6:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY53).val=yy1,(((_tmp9A3.YY53).tag=59,_tmp9A3)));}static char _tmp2F4[15]="conref_t<bool>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2F4,_tmp2F4,_tmp2F4 + 15}};
# 1167
union Cyc_Absyn_Constraint*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp2F5=yy1;union Cyc_Absyn_Constraint*_tmp2F6;_LL1FC: if((_tmp2F5.YY54).tag != 60)goto _LL1FE;_tmp2F6=(union Cyc_Absyn_Constraint*)(_tmp2F5.YY54).val;_LL1FD:
# 1171
 yyzzz=_tmp2F6;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1FB:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.YY54).val=yy1,(((_tmp9A4.YY54).tag=60,_tmp9A4)));}static char _tmp2F9[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp2F9,_tmp2F9,_tmp2F9 + 45}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2FA=yy1;struct Cyc_List_List*_tmp2FB;_LL201: if((_tmp2FA.YY55).tag != 61)goto _LL203;_tmp2FB=(struct Cyc_List_List*)(_tmp2FA.YY55).val;_LL202:
# 1172
 yyzzz=_tmp2FB;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL200:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY55).val=yy1,(((_tmp9A5.YY55).tag=61,_tmp9A5)));}static char _tmp2FE[20]="pointer_qual_t<`yy>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY56={Cyc_Core_Failure,{_tmp2FE,_tmp2FE,_tmp2FE + 20}};
# 1169
void*Cyc_yyget_YY56(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2FF=yy1;void*_tmp300;_LL206: if((_tmp2FF.YY56).tag != 62)goto _LL208;_tmp300=(void*)(_tmp2FF.YY56).val;_LL207:
# 1173
 yyzzz=_tmp300;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_YY56);_LL205:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY56).val=yy1,(((_tmp9A6.YY56).tag=62,_tmp9A6)));}static char _tmp303[21]="pointer_quals_t<`yy>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY57={Cyc_Core_Failure,{_tmp303,_tmp303,_tmp303 + 21}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp304=yy1;struct Cyc_List_List*_tmp305;_LL20B: if((_tmp304.YY57).tag != 63)goto _LL20D;_tmp305=(struct Cyc_List_List*)(_tmp304.YY57).val;_LL20C:
# 1174
 yyzzz=_tmp305;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_YY57);_LL20A:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.YY57).val=yy1,(((_tmp9A7.YY57).tag=63,_tmp9A7)));}static char _tmp308[21]="$(bool,string_t<`H>)";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp308,_tmp308,_tmp308 + 21}};
# 1171
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp309=yy1;struct _tuple20 _tmp30A;_LL210: if((_tmp309.Asm_tok).tag != 6)goto _LL212;_tmp30A=(struct _tuple20)(_tmp309.Asm_tok).val;_LL211:
# 1175
 yyzzz=_tmp30A;
goto _LL20F;_LL212:;_LL213:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL20F:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.Asm_tok).val=yy1,(((_tmp9A8.Asm_tok).tag=6,_tmp9A8)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1193
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1197
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9A9;return(_tmp9A9.timestamp=0,((_tmp9A9.first_line=0,((_tmp9A9.first_column=0,((_tmp9A9.last_line=0,((_tmp9A9.last_column=0,_tmp9A9)))))))));}
# 1200
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1211 "parse.y"
static short Cyc_yytranslate[375]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp30D[2]="$";static char _tmp30E[6]="error";static char _tmp30F[12]="$undefined.";static char _tmp310[5]="AUTO";static char _tmp311[9]="REGISTER";static char _tmp312[7]="STATIC";static char _tmp313[7]="EXTERN";static char _tmp314[8]="TYPEDEF";static char _tmp315[5]="VOID";static char _tmp316[5]="CHAR";static char _tmp317[6]="SHORT";static char _tmp318[4]="INT";static char _tmp319[5]="LONG";static char _tmp31A[6]="FLOAT";static char _tmp31B[7]="DOUBLE";static char _tmp31C[7]="SIGNED";static char _tmp31D[9]="UNSIGNED";static char _tmp31E[6]="CONST";static char _tmp31F[9]="VOLATILE";static char _tmp320[9]="RESTRICT";static char _tmp321[7]="STRUCT";static char _tmp322[6]="UNION";static char _tmp323[5]="CASE";static char _tmp324[8]="DEFAULT";static char _tmp325[7]="INLINE";static char _tmp326[7]="SIZEOF";static char _tmp327[9]="OFFSETOF";static char _tmp328[3]="IF";static char _tmp329[5]="ELSE";static char _tmp32A[7]="SWITCH";static char _tmp32B[6]="WHILE";static char _tmp32C[3]="DO";static char _tmp32D[4]="FOR";static char _tmp32E[5]="GOTO";static char _tmp32F[9]="CONTINUE";static char _tmp330[6]="BREAK";static char _tmp331[7]="RETURN";static char _tmp332[5]="ENUM";static char _tmp333[8]="NULL_kw";static char _tmp334[4]="LET";static char _tmp335[6]="THROW";static char _tmp336[4]="TRY";static char _tmp337[6]="CATCH";static char _tmp338[7]="EXPORT";static char _tmp339[4]="NEW";static char _tmp33A[9]="ABSTRACT";static char _tmp33B[9]="FALLTHRU";static char _tmp33C[6]="USING";static char _tmp33D[10]="NAMESPACE";static char _tmp33E[9]="DATATYPE";static char _tmp33F[8]="XTUNION";static char _tmp340[7]="TUNION";static char _tmp341[7]="MALLOC";static char _tmp342[8]="RMALLOC";static char _tmp343[7]="CALLOC";static char _tmp344[8]="RCALLOC";static char _tmp345[5]="SWAP";static char _tmp346[9]="REGION_T";static char _tmp347[6]="TAG_T";static char _tmp348[7]="REGION";static char _tmp349[5]="RNEW";static char _tmp34A[8]="REGIONS";static char _tmp34B[13]="RESET_REGION";static char _tmp34C[16]="NOZEROTERM_QUAL";static char _tmp34D[14]="ZEROTERM_QUAL";static char _tmp34E[12]="REGION_QUAL";static char _tmp34F[7]="PORTON";static char _tmp350[8]="PORTOFF";static char _tmp351[12]="DYNREGION_T";static char _tmp352[6]="ALIAS";static char _tmp353[8]="NUMELTS";static char _tmp354[8]="VALUEOF";static char _tmp355[10]="VALUEOF_T";static char _tmp356[9]="TAGCHECK";static char _tmp357[13]="NUMELTS_QUAL";static char _tmp358[10]="THIN_QUAL";static char _tmp359[9]="FAT_QUAL";static char _tmp35A[13]="NOTNULL_QUAL";static char _tmp35B[14]="NULLABLE_QUAL";static char _tmp35C[14]="REQUIRES_QUAL";static char _tmp35D[12]="TAGGED_QUAL";static char _tmp35E[16]="EXTENSIBLE_QUAL";static char _tmp35F[15]="RESETABLE_QUAL";static char _tmp360[7]="PTR_OP";static char _tmp361[7]="INC_OP";static char _tmp362[7]="DEC_OP";static char _tmp363[8]="LEFT_OP";static char _tmp364[9]="RIGHT_OP";static char _tmp365[6]="LE_OP";static char _tmp366[6]="GE_OP";static char _tmp367[6]="EQ_OP";static char _tmp368[6]="NE_OP";static char _tmp369[7]="AND_OP";static char _tmp36A[6]="OR_OP";static char _tmp36B[11]="MUL_ASSIGN";static char _tmp36C[11]="DIV_ASSIGN";static char _tmp36D[11]="MOD_ASSIGN";static char _tmp36E[11]="ADD_ASSIGN";static char _tmp36F[11]="SUB_ASSIGN";static char _tmp370[12]="LEFT_ASSIGN";static char _tmp371[13]="RIGHT_ASSIGN";static char _tmp372[11]="AND_ASSIGN";static char _tmp373[11]="XOR_ASSIGN";static char _tmp374[10]="OR_ASSIGN";static char _tmp375[9]="ELLIPSIS";static char _tmp376[11]="LEFT_RIGHT";static char _tmp377[12]="COLON_COLON";static char _tmp378[11]="IDENTIFIER";static char _tmp379[17]="INTEGER_CONSTANT";static char _tmp37A[7]="STRING";static char _tmp37B[8]="WSTRING";static char _tmp37C[19]="CHARACTER_CONSTANT";static char _tmp37D[20]="WCHARACTER_CONSTANT";static char _tmp37E[18]="FLOATING_CONSTANT";static char _tmp37F[9]="TYPE_VAR";static char _tmp380[13]="TYPEDEF_NAME";static char _tmp381[16]="QUAL_IDENTIFIER";static char _tmp382[18]="QUAL_TYPEDEF_NAME";static char _tmp383[10]="ATTRIBUTE";static char _tmp384[4]="ASM";static char _tmp385[4]="';'";static char _tmp386[4]="'{'";static char _tmp387[4]="'}'";static char _tmp388[4]="','";static char _tmp389[4]="'='";static char _tmp38A[4]="'<'";static char _tmp38B[4]="'>'";static char _tmp38C[4]="'('";static char _tmp38D[4]="')'";static char _tmp38E[4]="'_'";static char _tmp38F[4]="'$'";static char _tmp390[4]="':'";static char _tmp391[4]="'.'";static char _tmp392[4]="'['";static char _tmp393[4]="']'";static char _tmp394[4]="'*'";static char _tmp395[4]="'@'";static char _tmp396[4]="'?'";static char _tmp397[4]="'+'";static char _tmp398[4]="'|'";static char _tmp399[4]="'^'";static char _tmp39A[4]="'&'";static char _tmp39B[4]="'-'";static char _tmp39C[4]="'/'";static char _tmp39D[4]="'%'";static char _tmp39E[4]="'~'";static char _tmp39F[4]="'!'";static char _tmp3A0[5]="prog";static char _tmp3A1[17]="translation_unit";static char _tmp3A2[12]="export_list";static char _tmp3A3[19]="export_list_values";static char _tmp3A4[21]="external_declaration";static char _tmp3A5[20]="function_definition";static char _tmp3A6[21]="function_definition2";static char _tmp3A7[13]="using_action";static char _tmp3A8[15]="unusing_action";static char _tmp3A9[17]="namespace_action";static char _tmp3AA[19]="unnamespace_action";static char _tmp3AB[12]="declaration";static char _tmp3AC[19]="resetable_qual_opt";static char _tmp3AD[17]="declaration_list";static char _tmp3AE[23]="declaration_specifiers";static char _tmp3AF[24]="storage_class_specifier";static char _tmp3B0[15]="attributes_opt";static char _tmp3B1[11]="attributes";static char _tmp3B2[15]="attribute_list";static char _tmp3B3[10]="attribute";static char _tmp3B4[15]="type_specifier";static char _tmp3B5[25]="type_specifier_notypedef";static char _tmp3B6[5]="kind";static char _tmp3B7[15]="type_qualifier";static char _tmp3B8[15]="enum_specifier";static char _tmp3B9[11]="enum_field";static char _tmp3BA[22]="enum_declaration_list";static char _tmp3BB[26]="struct_or_union_specifier";static char _tmp3BC[16]="type_params_opt";static char _tmp3BD[16]="struct_or_union";static char _tmp3BE[24]="struct_declaration_list";static char _tmp3BF[25]="struct_declaration_list0";static char _tmp3C0[21]="init_declarator_list";static char _tmp3C1[22]="init_declarator_list0";static char _tmp3C2[16]="init_declarator";static char _tmp3C3[19]="struct_declaration";static char _tmp3C4[25]="specifier_qualifier_list";static char _tmp3C5[35]="notypedef_specifier_qualifier_list";static char _tmp3C6[23]="struct_declarator_list";static char _tmp3C7[24]="struct_declarator_list0";static char _tmp3C8[18]="struct_declarator";static char _tmp3C9[19]="datatype_specifier";static char _tmp3CA[14]="qual_datatype";static char _tmp3CB[19]="datatypefield_list";static char _tmp3CC[20]="datatypefield_scope";static char _tmp3CD[14]="datatypefield";static char _tmp3CE[11]="declarator";static char _tmp3CF[23]="declarator_withtypedef";static char _tmp3D0[18]="direct_declarator";static char _tmp3D1[30]="direct_declarator_withtypedef";static char _tmp3D2[8]="pointer";static char _tmp3D3[12]="one_pointer";static char _tmp3D4[14]="pointer_quals";static char _tmp3D5[13]="pointer_qual";static char _tmp3D6[23]="pointer_null_and_bound";static char _tmp3D7[14]="pointer_bound";static char _tmp3D8[18]="zeroterm_qual_opt";static char _tmp3D9[8]="rgn_opt";static char _tmp3DA[11]="tqual_list";static char _tmp3DB[20]="parameter_type_list";static char _tmp3DC[9]="type_var";static char _tmp3DD[16]="optional_effect";static char _tmp3DE[19]="optional_rgn_order";static char _tmp3DF[10]="rgn_order";static char _tmp3E0[16]="optional_inject";static char _tmp3E1[11]="effect_set";static char _tmp3E2[14]="atomic_effect";static char _tmp3E3[11]="region_set";static char _tmp3E4[15]="parameter_list";static char _tmp3E5[22]="parameter_declaration";static char _tmp3E6[16]="identifier_list";static char _tmp3E7[17]="identifier_list0";static char _tmp3E8[12]="initializer";static char _tmp3E9[18]="array_initializer";static char _tmp3EA[17]="initializer_list";static char _tmp3EB[12]="designation";static char _tmp3EC[16]="designator_list";static char _tmp3ED[11]="designator";static char _tmp3EE[10]="type_name";static char _tmp3EF[14]="any_type_name";static char _tmp3F0[15]="type_name_list";static char _tmp3F1[20]="abstract_declarator";static char _tmp3F2[27]="direct_abstract_declarator";static char _tmp3F3[10]="statement";static char _tmp3F4[13]="open_exp_opt";static char _tmp3F5[18]="labeled_statement";static char _tmp3F6[21]="expression_statement";static char _tmp3F7[19]="compound_statement";static char _tmp3F8[16]="block_item_list";static char _tmp3F9[20]="selection_statement";static char _tmp3FA[15]="switch_clauses";static char _tmp3FB[20]="iteration_statement";static char _tmp3FC[15]="jump_statement";static char _tmp3FD[12]="exp_pattern";static char _tmp3FE[20]="conditional_pattern";static char _tmp3FF[19]="logical_or_pattern";static char _tmp400[20]="logical_and_pattern";static char _tmp401[21]="inclusive_or_pattern";static char _tmp402[21]="exclusive_or_pattern";static char _tmp403[12]="and_pattern";static char _tmp404[17]="equality_pattern";static char _tmp405[19]="relational_pattern";static char _tmp406[14]="shift_pattern";static char _tmp407[17]="additive_pattern";static char _tmp408[23]="multiplicative_pattern";static char _tmp409[13]="cast_pattern";static char _tmp40A[14]="unary_pattern";static char _tmp40B[16]="postfix_pattern";static char _tmp40C[16]="primary_pattern";static char _tmp40D[8]="pattern";static char _tmp40E[19]="tuple_pattern_list";static char _tmp40F[20]="tuple_pattern_list0";static char _tmp410[14]="field_pattern";static char _tmp411[19]="field_pattern_list";static char _tmp412[20]="field_pattern_list0";static char _tmp413[11]="expression";static char _tmp414[22]="assignment_expression";static char _tmp415[20]="assignment_operator";static char _tmp416[23]="conditional_expression";static char _tmp417[20]="constant_expression";static char _tmp418[22]="logical_or_expression";static char _tmp419[23]="logical_and_expression";static char _tmp41A[24]="inclusive_or_expression";static char _tmp41B[24]="exclusive_or_expression";static char _tmp41C[15]="and_expression";static char _tmp41D[20]="equality_expression";static char _tmp41E[22]="relational_expression";static char _tmp41F[17]="shift_expression";static char _tmp420[20]="additive_expression";static char _tmp421[26]="multiplicative_expression";static char _tmp422[16]="cast_expression";static char _tmp423[17]="unary_expression";static char _tmp424[15]="unary_operator";static char _tmp425[19]="postfix_expression";static char _tmp426[17]="field_expression";static char _tmp427[19]="primary_expression";static char _tmp428[25]="argument_expression_list";static char _tmp429[26]="argument_expression_list0";static char _tmp42A[9]="constant";static char _tmp42B[20]="qual_opt_identifier";static char _tmp42C[17]="qual_opt_typedef";static char _tmp42D[18]="struct_union_name";static char _tmp42E[11]="field_name";static char _tmp42F[12]="right_angle";
# 1557 "parse.y"
static struct _dyneither_ptr Cyc_yytname[291]={{_tmp30D,_tmp30D,_tmp30D + 2},{_tmp30E,_tmp30E,_tmp30E + 6},{_tmp30F,_tmp30F,_tmp30F + 12},{_tmp310,_tmp310,_tmp310 + 5},{_tmp311,_tmp311,_tmp311 + 9},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,_tmp313,_tmp313 + 7},{_tmp314,_tmp314,_tmp314 + 8},{_tmp315,_tmp315,_tmp315 + 5},{_tmp316,_tmp316,_tmp316 + 5},{_tmp317,_tmp317,_tmp317 + 6},{_tmp318,_tmp318,_tmp318 + 4},{_tmp319,_tmp319,_tmp319 + 5},{_tmp31A,_tmp31A,_tmp31A + 6},{_tmp31B,_tmp31B,_tmp31B + 7},{_tmp31C,_tmp31C,_tmp31C + 7},{_tmp31D,_tmp31D,_tmp31D + 9},{_tmp31E,_tmp31E,_tmp31E + 6},{_tmp31F,_tmp31F,_tmp31F + 9},{_tmp320,_tmp320,_tmp320 + 9},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 6},{_tmp323,_tmp323,_tmp323 + 5},{_tmp324,_tmp324,_tmp324 + 8},{_tmp325,_tmp325,_tmp325 + 7},{_tmp326,_tmp326,_tmp326 + 7},{_tmp327,_tmp327,_tmp327 + 9},{_tmp328,_tmp328,_tmp328 + 3},{_tmp329,_tmp329,_tmp329 + 5},{_tmp32A,_tmp32A,_tmp32A + 7},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 3},{_tmp32D,_tmp32D,_tmp32D + 4},{_tmp32E,_tmp32E,_tmp32E + 5},{_tmp32F,_tmp32F,_tmp32F + 9},{_tmp330,_tmp330,_tmp330 + 6},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 5},{_tmp333,_tmp333,_tmp333 + 8},{_tmp334,_tmp334,_tmp334 + 4},{_tmp335,_tmp335,_tmp335 + 6},{_tmp336,_tmp336,_tmp336 + 4},{_tmp337,_tmp337,_tmp337 + 6},{_tmp338,_tmp338,_tmp338 + 7},{_tmp339,_tmp339,_tmp339 + 4},{_tmp33A,_tmp33A,_tmp33A + 9},{_tmp33B,_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 10},{_tmp33E,_tmp33E,_tmp33E + 9},{_tmp33F,_tmp33F,_tmp33F + 8},{_tmp340,_tmp340,_tmp340 + 7},{_tmp341,_tmp341,_tmp341 + 7},{_tmp342,_tmp342,_tmp342 + 8},{_tmp343,_tmp343,_tmp343 + 7},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 5},{_tmp346,_tmp346,_tmp346 + 9},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,_tmp348 + 7},{_tmp349,_tmp349,_tmp349 + 5},{_tmp34A,_tmp34A,_tmp34A + 8},{_tmp34B,_tmp34B,_tmp34B + 13},{_tmp34C,_tmp34C,_tmp34C + 16},{_tmp34D,_tmp34D,_tmp34D + 14},{_tmp34E,_tmp34E,_tmp34E + 12},{_tmp34F,_tmp34F,_tmp34F + 7},{_tmp350,_tmp350,_tmp350 + 8},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 6},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 8},{_tmp355,_tmp355,_tmp355 + 10},{_tmp356,_tmp356,_tmp356 + 9},{_tmp357,_tmp357,_tmp357 + 13},{_tmp358,_tmp358,_tmp358 + 10},{_tmp359,_tmp359,_tmp359 + 9},{_tmp35A,_tmp35A,_tmp35A + 13},{_tmp35B,_tmp35B,_tmp35B + 14},{_tmp35C,_tmp35C,_tmp35C + 14},{_tmp35D,_tmp35D,_tmp35D + 12},{_tmp35E,_tmp35E,_tmp35E + 16},{_tmp35F,_tmp35F,_tmp35F + 15},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 7},{_tmp362,_tmp362,_tmp362 + 7},{_tmp363,_tmp363,_tmp363 + 8},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,_tmp365,_tmp365 + 6},{_tmp366,_tmp366,_tmp366 + 6},{_tmp367,_tmp367,_tmp367 + 6},{_tmp368,_tmp368,_tmp368 + 6},{_tmp369,_tmp369,_tmp369 + 7},{_tmp36A,_tmp36A,_tmp36A + 6},{_tmp36B,_tmp36B,_tmp36B + 11},{_tmp36C,_tmp36C,_tmp36C + 11},{_tmp36D,_tmp36D,_tmp36D + 11},{_tmp36E,_tmp36E,_tmp36E + 11},{_tmp36F,_tmp36F,_tmp36F + 11},{_tmp370,_tmp370,_tmp370 + 12},{_tmp371,_tmp371,_tmp371 + 13},{_tmp372,_tmp372,_tmp372 + 11},{_tmp373,_tmp373,_tmp373 + 11},{_tmp374,_tmp374,_tmp374 + 10},{_tmp375,_tmp375,_tmp375 + 9},{_tmp376,_tmp376,_tmp376 + 11},{_tmp377,_tmp377,_tmp377 + 12},{_tmp378,_tmp378,_tmp378 + 11},{_tmp379,_tmp379,_tmp379 + 17},{_tmp37A,_tmp37A,_tmp37A + 7},{_tmp37B,_tmp37B,_tmp37B + 8},{_tmp37C,_tmp37C,_tmp37C + 19},{_tmp37D,_tmp37D,_tmp37D + 20},{_tmp37E,_tmp37E,_tmp37E + 18},{_tmp37F,_tmp37F,_tmp37F + 9},{_tmp380,_tmp380,_tmp380 + 13},{_tmp381,_tmp381,_tmp381 + 16},{_tmp382,_tmp382,_tmp382 + 18},{_tmp383,_tmp383,_tmp383 + 10},{_tmp384,_tmp384,_tmp384 + 4},{_tmp385,_tmp385,_tmp385 + 4},{_tmp386,_tmp386,_tmp386 + 4},{_tmp387,_tmp387,_tmp387 + 4},{_tmp388,_tmp388,_tmp388 + 4},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,_tmp38A + 4},{_tmp38B,_tmp38B,_tmp38B + 4},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,_tmp38D,_tmp38D + 4},{_tmp38E,_tmp38E,_tmp38E + 4},{_tmp38F,_tmp38F,_tmp38F + 4},{_tmp390,_tmp390,_tmp390 + 4},{_tmp391,_tmp391,_tmp391 + 4},{_tmp392,_tmp392,_tmp392 + 4},{_tmp393,_tmp393,_tmp393 + 4},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 4},{_tmp398,_tmp398,_tmp398 + 4},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 5},{_tmp3A1,_tmp3A1,_tmp3A1 + 17},{_tmp3A2,_tmp3A2,_tmp3A2 + 12},{_tmp3A3,_tmp3A3,_tmp3A3 + 19},{_tmp3A4,_tmp3A4,_tmp3A4 + 21},{_tmp3A5,_tmp3A5,_tmp3A5 + 20},{_tmp3A6,_tmp3A6,_tmp3A6 + 21},{_tmp3A7,_tmp3A7,_tmp3A7 + 13},{_tmp3A8,_tmp3A8,_tmp3A8 + 15},{_tmp3A9,_tmp3A9,_tmp3A9 + 17},{_tmp3AA,_tmp3AA,_tmp3AA + 19},{_tmp3AB,_tmp3AB,_tmp3AB + 12},{_tmp3AC,_tmp3AC,_tmp3AC + 19},{_tmp3AD,_tmp3AD,_tmp3AD + 17},{_tmp3AE,_tmp3AE,_tmp3AE + 23},{_tmp3AF,_tmp3AF,_tmp3AF + 24},{_tmp3B0,_tmp3B0,_tmp3B0 + 15},{_tmp3B1,_tmp3B1,_tmp3B1 + 11},{_tmp3B2,_tmp3B2,_tmp3B2 + 15},{_tmp3B3,_tmp3B3,_tmp3B3 + 10},{_tmp3B4,_tmp3B4,_tmp3B4 + 15},{_tmp3B5,_tmp3B5,_tmp3B5 + 25},{_tmp3B6,_tmp3B6,_tmp3B6 + 5},{_tmp3B7,_tmp3B7,_tmp3B7 + 15},{_tmp3B8,_tmp3B8,_tmp3B8 + 15},{_tmp3B9,_tmp3B9,_tmp3B9 + 11},{_tmp3BA,_tmp3BA,_tmp3BA + 22},{_tmp3BB,_tmp3BB,_tmp3BB + 26},{_tmp3BC,_tmp3BC,_tmp3BC + 16},{_tmp3BD,_tmp3BD,_tmp3BD + 16},{_tmp3BE,_tmp3BE,_tmp3BE + 24},{_tmp3BF,_tmp3BF,_tmp3BF + 25},{_tmp3C0,_tmp3C0,_tmp3C0 + 21},{_tmp3C1,_tmp3C1,_tmp3C1 + 22},{_tmp3C2,_tmp3C2,_tmp3C2 + 16},{_tmp3C3,_tmp3C3,_tmp3C3 + 19},{_tmp3C4,_tmp3C4,_tmp3C4 + 25},{_tmp3C5,_tmp3C5,_tmp3C5 + 35},{_tmp3C6,_tmp3C6,_tmp3C6 + 23},{_tmp3C7,_tmp3C7,_tmp3C7 + 24},{_tmp3C8,_tmp3C8,_tmp3C8 + 18},{_tmp3C9,_tmp3C9,_tmp3C9 + 19},{_tmp3CA,_tmp3CA,_tmp3CA + 14},{_tmp3CB,_tmp3CB,_tmp3CB + 19},{_tmp3CC,_tmp3CC,_tmp3CC + 20},{_tmp3CD,_tmp3CD,_tmp3CD + 14},{_tmp3CE,_tmp3CE,_tmp3CE + 11},{_tmp3CF,_tmp3CF,_tmp3CF + 23},{_tmp3D0,_tmp3D0,_tmp3D0 + 18},{_tmp3D1,_tmp3D1,_tmp3D1 + 30},{_tmp3D2,_tmp3D2,_tmp3D2 + 8},{_tmp3D3,_tmp3D3,_tmp3D3 + 12},{_tmp3D4,_tmp3D4,_tmp3D4 + 14},{_tmp3D5,_tmp3D5,_tmp3D5 + 13},{_tmp3D6,_tmp3D6,_tmp3D6 + 23},{_tmp3D7,_tmp3D7,_tmp3D7 + 14},{_tmp3D8,_tmp3D8,_tmp3D8 + 18},{_tmp3D9,_tmp3D9,_tmp3D9 + 8},{_tmp3DA,_tmp3DA,_tmp3DA + 11},{_tmp3DB,_tmp3DB,_tmp3DB + 20},{_tmp3DC,_tmp3DC,_tmp3DC + 9},{_tmp3DD,_tmp3DD,_tmp3DD + 16},{_tmp3DE,_tmp3DE,_tmp3DE + 19},{_tmp3DF,_tmp3DF,_tmp3DF + 10},{_tmp3E0,_tmp3E0,_tmp3E0 + 16},{_tmp3E1,_tmp3E1,_tmp3E1 + 11},{_tmp3E2,_tmp3E2,_tmp3E2 + 14},{_tmp3E3,_tmp3E3,_tmp3E3 + 11},{_tmp3E4,_tmp3E4,_tmp3E4 + 15},{_tmp3E5,_tmp3E5,_tmp3E5 + 22},{_tmp3E6,_tmp3E6,_tmp3E6 + 16},{_tmp3E7,_tmp3E7,_tmp3E7 + 17},{_tmp3E8,_tmp3E8,_tmp3E8 + 12},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 17},{_tmp3EB,_tmp3EB,_tmp3EB + 12},{_tmp3EC,_tmp3EC,_tmp3EC + 16},{_tmp3ED,_tmp3ED,_tmp3ED + 11},{_tmp3EE,_tmp3EE,_tmp3EE + 10},{_tmp3EF,_tmp3EF,_tmp3EF + 14},{_tmp3F0,_tmp3F0,_tmp3F0 + 15},{_tmp3F1,_tmp3F1,_tmp3F1 + 20},{_tmp3F2,_tmp3F2,_tmp3F2 + 27},{_tmp3F3,_tmp3F3,_tmp3F3 + 10},{_tmp3F4,_tmp3F4,_tmp3F4 + 13},{_tmp3F5,_tmp3F5,_tmp3F5 + 18},{_tmp3F6,_tmp3F6,_tmp3F6 + 21},{_tmp3F7,_tmp3F7,_tmp3F7 + 19},{_tmp3F8,_tmp3F8,_tmp3F8 + 16},{_tmp3F9,_tmp3F9,_tmp3F9 + 20},{_tmp3FA,_tmp3FA,_tmp3FA + 15},{_tmp3FB,_tmp3FB,_tmp3FB + 20},{_tmp3FC,_tmp3FC,_tmp3FC + 15},{_tmp3FD,_tmp3FD,_tmp3FD + 12},{_tmp3FE,_tmp3FE,_tmp3FE + 20},{_tmp3FF,_tmp3FF,_tmp3FF + 19},{_tmp400,_tmp400,_tmp400 + 20},{_tmp401,_tmp401,_tmp401 + 21},{_tmp402,_tmp402,_tmp402 + 21},{_tmp403,_tmp403,_tmp403 + 12},{_tmp404,_tmp404,_tmp404 + 17},{_tmp405,_tmp405,_tmp405 + 19},{_tmp406,_tmp406,_tmp406 + 14},{_tmp407,_tmp407,_tmp407 + 17},{_tmp408,_tmp408,_tmp408 + 23},{_tmp409,_tmp409,_tmp409 + 13},{_tmp40A,_tmp40A,_tmp40A + 14},{_tmp40B,_tmp40B,_tmp40B + 16},{_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 8},{_tmp40E,_tmp40E,_tmp40E + 19},{_tmp40F,_tmp40F,_tmp40F + 20},{_tmp410,_tmp410,_tmp410 + 14},{_tmp411,_tmp411,_tmp411 + 19},{_tmp412,_tmp412,_tmp412 + 20},{_tmp413,_tmp413,_tmp413 + 11},{_tmp414,_tmp414,_tmp414 + 22},{_tmp415,_tmp415,_tmp415 + 20},{_tmp416,_tmp416,_tmp416 + 23},{_tmp417,_tmp417,_tmp417 + 20},{_tmp418,_tmp418,_tmp418 + 22},{_tmp419,_tmp419,_tmp419 + 23},{_tmp41A,_tmp41A,_tmp41A + 24},{_tmp41B,_tmp41B,_tmp41B + 24},{_tmp41C,_tmp41C,_tmp41C + 15},{_tmp41D,_tmp41D,_tmp41D + 20},{_tmp41E,_tmp41E,_tmp41E + 22},{_tmp41F,_tmp41F,_tmp41F + 17},{_tmp420,_tmp420,_tmp420 + 20},{_tmp421,_tmp421,_tmp421 + 26},{_tmp422,_tmp422,_tmp422 + 16},{_tmp423,_tmp423,_tmp423 + 17},{_tmp424,_tmp424,_tmp424 + 15},{_tmp425,_tmp425,_tmp425 + 19},{_tmp426,_tmp426,_tmp426 + 17},{_tmp427,_tmp427,_tmp427 + 19},{_tmp428,_tmp428,_tmp428 + 25},{_tmp429,_tmp429,_tmp429 + 26},{_tmp42A,_tmp42A,_tmp42A + 9},{_tmp42B,_tmp42B,_tmp42B + 20},{_tmp42C,_tmp42C,_tmp42C + 17},{_tmp42D,_tmp42D,_tmp42D + 18},{_tmp42E,_tmp42E,_tmp42E + 11},{_tmp42F,_tmp42F,_tmp42F + 12}};
# 1611
static short Cyc_yyr1[519]={0,147,148,148,148,148,148,148,148,148,148,148,149,149,150,150,150,151,151,151,152,152,152,152,153,153,154,155,156,157,158,158,158,158,158,158,158,159,159,160,160,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,163,163,164,165,165,166,166,166,166,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,170,170,170,171,171,171,172,172,173,173,173,174,174,174,174,174,175,175,176,176,177,177,178,178,179,180,180,181,181,182,183,183,183,183,183,183,184,184,184,184,184,184,185,186,186,187,187,187,187,187,188,188,188,189,189,190,190,190,190,191,191,191,192,192,193,193,194,194,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,196,197,197,198,199,199,200,200,200,200,200,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,214,215,215,216,216,216,217,218,218,219,219,220,220,220,220,220,221,221,221,221,222,223,223,224,224,225,225,226,226,226,226,226,227,227,228,228,228,229,229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,230,231,231,232,233,233,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,238,238,238,238,238,238,238,238,238,238,238,238,238,238,239,239,239,239,239,239,239,239,240,241,241,242,242,243,243,244,244,245,245,246,246,247,247,247,248,248,248,248,248,249,249,249,250,250,250,251,251,251,251,252,252,253,253,253,253,253,253,254,255,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,257,257,257,258,258,259,259,260,260,260,261,261,262,262,263,263,263,264,264,264,264,264,264,264,264,264,264,264,265,265,265,265,265,265,265,266,267,267,268,268,269,269,270,270,271,271,272,272,272,273,273,273,273,273,274,274,274,275,275,275,276,276,276,276,277,277,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,281,281,281,281,282,282,282,282,282,282,282,282,282,282,283,284,284,285,285,285,285,285,286,286,287,287,288,288,289,289,290,290};
# 1666
static short Cyc_yyr2[519]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,5,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1721
static short Cyc_yydefact[1032]={0,19,51,52,53,54,56,71,74,75,76,77,78,79,80,81,95,96,97,113,114,47,0,0,57,0,0,148,88,92,0,0,0,0,0,0,0,38,509,214,511,510,512,0,0,72,0,200,200,199,1,0,17,0,0,18,0,0,41,49,43,69,45,82,83,0,84,0,37,159,0,184,187,85,163,111,55,54,48,0,99,508,0,509,504,505,506,507,111,0,374,0,0,0,0,237,0,376,377,26,28,0,0,0,0,0,0,0,0,149,0,0,0,0,0,0,0,197,198,2,0,0,0,0,0,30,0,119,120,122,42,50,44,46,115,513,514,111,111,37,39,37,0,20,0,216,0,172,160,185,194,193,0,0,191,192,195,196,205,187,0,70,0,55,103,0,101,0,509,384,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,493,494,473,0,0,0,0,0,476,474,475,0,401,403,417,425,427,429,431,433,435,438,443,446,449,453,0,455,477,492,491,0,385,383,33,0,0,111,0,0,0,129,125,127,256,258,0,0,9,10,0,0,0,111,515,516,215,94,0,0,164,73,235,0,232,0,3,0,5,0,286,0,31,0,0,37,21,0,116,117,142,110,0,146,0,0,0,0,0,0,0,0,0,0,0,0,509,289,291,0,297,293,0,295,279,280,281,0,282,283,284,0,40,22,122,263,0,222,238,0,0,218,216,0,202,0,0,0,207,58,206,188,0,0,104,100,0,0,378,0,0,398,0,0,0,0,252,394,399,0,396,0,463,0,419,453,0,420,421,0,0,0,0,0,0,0,0,0,456,457,37,0,0,0,459,460,458,0,375,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,407,408,409,410,411,412,413,414,415,416,406,0,461,0,483,484,0,0,0,496,0,111,391,392,0,389,0,239,0,0,0,0,259,230,0,130,135,131,133,126,128,216,0,265,257,266,518,517,0,87,91,0,89,37,93,109,64,63,0,61,165,216,234,161,265,236,173,174,0,86,201,27,0,29,0,0,0,0,121,123,241,240,23,106,118,0,0,0,137,138,140,0,111,111,154,0,0,0,0,0,0,37,0,326,327,328,0,0,330,0,0,0,0,298,294,122,296,292,290,0,171,223,0,0,0,229,217,224,168,0,0,0,218,170,204,203,166,202,0,0,208,59,112,0,105,424,102,98,387,388,255,0,395,251,253,382,0,0,0,0,242,247,0,0,0,0,0,0,0,0,0,0,0,0,0,495,502,0,501,402,426,0,428,430,432,434,436,437,441,442,439,440,444,445,447,448,450,451,452,405,404,482,479,0,481,0,0,0,379,0,386,32,0,380,0,0,260,136,132,134,0,218,0,202,0,267,0,216,0,278,262,0,36,111,0,0,0,129,0,111,0,216,0,183,162,233,4,6,0,35,0,0,141,124,142,0,0,218,147,156,155,0,0,150,0,0,0,305,0,0,0,0,0,325,329,0,0,0,0,288,37,24,264,216,0,226,0,0,219,0,169,222,210,167,190,189,208,186,0,7,0,254,397,400,462,0,0,243,0,248,465,0,0,0,0,469,472,0,0,0,0,0,454,498,0,0,480,478,0,0,390,393,381,261,231,274,0,268,269,202,0,0,218,202,0,90,218,0,0,0,60,62,0,175,0,0,218,0,202,0,0,0,139,144,143,115,145,157,154,154,0,0,0,0,0,0,0,0,0,0,0,0,0,305,331,0,0,25,218,0,227,225,0,218,0,209,0,8,488,0,487,0,244,249,0,0,0,0,422,423,482,481,500,0,503,418,497,499,0,273,271,277,276,0,270,202,115,0,67,65,66,176,182,179,0,181,177,202,0,34,0,0,153,152,299,305,0,0,0,0,0,0,333,334,336,338,340,342,344,346,349,354,357,360,364,366,372,373,0,37,302,311,0,0,0,0,0,0,0,0,0,0,332,285,212,228,220,211,216,13,0,14,464,0,0,250,466,0,0,471,470,485,0,275,272,0,0,180,178,287,108,0,0,0,305,0,370,0,0,367,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,368,306,0,313,0,0,0,321,0,0,0,0,0,0,304,0,218,12,15,0,490,489,0,0,0,486,107,0,158,300,301,0,0,0,0,305,307,337,0,339,341,343,345,347,348,352,353,350,351,355,356,358,359,361,362,363,0,312,314,315,0,323,322,0,317,0,0,0,221,213,16,0,0,0,0,0,303,369,0,365,308,0,37,316,324,318,319,0,246,245,467,0,68,0,335,305,309,320,0,371,310,468,0,0,0};
# 1828
static short Cyc_yydefgoto[144]={1029,50,686,876,51,52,287,53,458,54,460,55,56,141,57,58,526,228,444,445,229,61,243,230,63,164,165,64,161,65,264,265,126,127,128,266,231,425,473,474,475,66,67,647,648,649,68,476,69,449,70,71,158,159,72,117,522,316,683,606,73,607,516,674,508,512,513,420,309,251,94,95,547,466,548,331,332,333,232,302,303,608,431,290,462,291,292,293,294,295,761,296,297,837,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,334,410,411,335,336,337,298,199,397,200,532,201,202,203,204,205,206,207,208,209,210,211,212,213,214,785,215,563,564,216,217,75,877,244,435};
# 1846
static short Cyc_yypact[1032]={2698,- -32768,- -32768,- -32768,- -32768,- 43,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3367,178,827,- -32768,- 73,10,- -32768,57,115,118,195,196,272,218,186,357,- -32768,- -32768,330,- -32768,- -32768,- -32768,326,671,358,375,364,364,- -32768,- -32768,2560,- -32768,219,421,- -32768,407,868,3367,3367,3367,- -32768,3367,- -32768,- -32768,352,- -32768,- 73,3202,139,- 35,553,620,- -32768,- -32768,401,391,413,- -32768,- 73,429,- -32768,1063,156,- -32768,- -32768,- -32768,- -32768,401,6024,- -32768,460,518,1063,441,476,495,- -32768,81,- -32768,- -32768,3515,3515,2560,2560,3515,525,6024,487,- -32768,64,519,345,542,64,3737,6024,- -32768,- -32768,- -32768,2560,2835,2560,2835,65,- -32768,556,589,- -32768,3119,- -32768,- -32768,- -32768,- -32768,3737,- -32768,- -32768,401,41,1565,- -32768,3202,868,- -32768,3515,3441,4377,- -32768,139,- -32768,- -32768,- -32768,600,612,- -32768,- -32768,- -32768,- -32768,46,620,3515,- -32768,2835,- -32768,621,633,634,- 73,73,- -32768,1063,113,1164,6261,649,6024,6103,654,668,696,716,732,736,750,752,6340,6340,- -32768,- -32768,- -32768,2136,758,6419,6419,6419,- -32768,- -32768,- -32768,105,- -32768,- -32768,21,788,706,747,748,570,12,629,160,217,- -32768,718,6419,199,- 34,- -32768,769,952,784,- -32768,- -32768,785,6024,401,952,767,203,3737,3811,3737,814,- -32768,9,9,- -32768,- -32768,77,772,191,401,- -32768,- -32768,- -32768,- -32768,62,775,- -32768,- -32768,737,262,- -32768,778,- -32768,783,- -32768,787,786,793,- -32768,345,4476,3202,- -32768,791,3737,- -32768,904,794,- 73,797,796,89,800,3882,802,817,810,813,4575,3882,148,809,811,- -32768,- -32768,818,1709,1709,868,1709,- -32768,- -32768,- -32768,822,- -32768,- -32768,- -32768,284,- -32768,- -32768,828,815,78,848,- -32768,22,831,829,314,833,655,830,3515,6024,- -32768,849,- -32768,- -32768,78,844,- 73,- -32768,6024,847,- -32768,846,850,- -32768,64,6024,1063,269,- -32768,- -32768,- -32768,852,854,2136,- -32768,3737,- -32768,- -32768,3981,- -32768,877,6024,6024,6024,6024,6024,6024,3737,996,2136,- -32768,- -32768,1853,851,263,6024,- -32768,- -32768,- -32768,6024,- -32768,6419,6024,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6024,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6024,- -32768,64,- -32768,- -32768,4671,64,6024,- -32768,853,401,- -32768,- -32768,857,858,1063,- -32768,440,1164,859,3515,- -32768,863,860,- -32768,3811,3811,3811,- -32768,- -32768,2984,4767,138,- -32768,250,- -32768,- -32768,22,- -32768,- -32768,3515,- -32768,3285,- -32768,870,- -32768,862,864,871,- -32768,2910,- -32768,604,490,- -32768,- -32768,- -32768,3737,- -32768,- -32768,- -32768,2560,- -32768,2560,873,878,867,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1123,6024,880,884,- -32768,- 37,231,401,401,744,6024,6024,874,887,6024,980,1997,892,- -32768,- -32768,- -32768,447,971,- -32768,4866,6024,3882,2275,- -32768,- -32768,3119,- -32768,- -32768,- -32768,3515,- -32768,- -32768,3737,888,220,- -32768,- -32768,879,- -32768,22,890,3663,829,- -32768,- -32768,- -32768,- -32768,655,107,894,753,- -32768,- -32768,2422,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,893,- -32768,- -32768,- -32768,- -32768,2946,901,909,929,- -32768,- -32768,630,4476,917,914,923,925,323,922,924,239,926,931,6182,- -32768,- -32768,927,928,- -32768,788,257,706,747,748,570,12,12,629,629,629,629,160,160,217,217,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,930,- -32768,173,3515,4278,- -32768,3769,- -32768,- -32768,935,- -32768,109,947,- -32768,- -32768,- -32768,- -32768,934,829,938,655,933,250,3515,3589,4962,- -32768,- -32768,9,- -32768,401,694,941,62,1206,942,401,3515,3441,5061,- -32768,604,- -32768,- -32768,- -32768,953,- -32768,967,1191,- -32768,- -32768,904,6024,6024,829,- -32768,- -32768,- -32768,961,- 73,463,371,385,6024,738,387,958,5160,5256,492,- -32768,- -32768,965,968,962,397,- -32768,3202,- -32768,815,969,3515,- -32768,970,22,- -32768,973,- -32768,213,- -32768,- -32768,- -32768,- -32768,753,- -32768,975,- -32768,2560,- -32768,- -32768,- -32768,989,248,986,- -32768,4080,- -32768,- -32768,6024,1088,6024,6103,- -32768,- -32768,64,64,989,987,4278,- -32768,- -32768,6024,6024,- -32768,- -32768,78,645,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,988,- -32768,- -32768,655,78,990,829,655,983,- -32768,829,409,991,992,- -32768,- -32768,994,- -32768,78,1001,829,1003,655,1000,6024,1015,- -32768,- -32768,- -32768,3737,- -32768,1009,48,744,3882,993,1010,6549,1006,1017,3882,6024,5352,539,5448,554,5544,738,- -32768,1021,1022,- -32768,829,131,- -32768,- -32768,1029,829,3737,- -32768,618,- -32768,- -32768,180,- -32768,6024,- -32768,- -32768,4476,1016,1018,1023,- -32768,877,1019,1020,- -32768,661,- -32768,- -32768,- -32768,- -32768,4278,- -32768,- -32768,- -32768,- -32768,1024,- -32768,655,3737,1042,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1025,- -32768,- -32768,655,436,- -32768,1035,3737,- -32768,- -32768,1130,738,1030,6498,1032,2275,6419,1031,- -32768,37,- -32768,1068,1026,669,757,100,764,346,512,- -32768,- -32768,- -32768,- -32768,1072,6419,1853,- -32768,- -32768,457,3882,467,5640,3882,485,5736,5832,587,1044,- -32768,- -32768,- -32768,- -32768,1046,- -32768,969,- -32768,1050,588,- -32768,302,307,- -32768,- -32768,3737,1148,- -32768,- -32768,- -32768,4179,- -32768,- -32768,1055,1057,- -32768,- -32768,- -32768,- -32768,505,3882,1056,738,2136,- -32768,3737,1053,- -32768,1421,6419,6024,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6419,6024,- -32768,- -32768,1062,- -32768,3882,3882,517,- -32768,3882,3882,550,3882,576,5928,- -32768,22,829,- -32768,- -32768,487,- -32768,- -32768,2275,1060,1058,- -32768,- -32768,1081,- -32768,- -32768,- -32768,1069,1066,1073,6419,738,- -32768,788,310,706,747,747,570,12,12,629,629,629,629,160,160,217,217,- -32768,- -32768,- -32768,339,- -32768,- -32768,- -32768,3882,- -32768,- -32768,3882,- -32768,3882,3882,577,- -32768,- -32768,- -32768,1075,745,1067,3737,1071,- -32768,989,248,- -32768,- -32768,6024,1421,- -32768,- -32768,- -32768,- -32768,3882,- -32768,- -32768,- -32768,1077,- -32768,363,- -32768,738,- -32768,- -32768,1078,- -32768,- -32768,- -32768,1201,1203,- -32768};
# 1953
static short Cyc_yypgoto[144]={- -32768,207,- -32768,261,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 44,- -32768,- 119,16,- -32768,- -32768,0,586,- -32768,91,- 168,1096,5,- -32768,- -32768,- 144,- -32768,564,1176,- 702,- -32768,- -32768,- -32768,977,963,146,434,- -32768,- -32768,572,- -32768,- -32768,121,- -32768,- -32768,75,- 211,1161,- 370,69,- -32768,1074,- -32768,- -32768,1187,- 91,- -32768,562,- 127,- 145,- 123,- 334,304,571,574,- 428,- 433,- 114,- 384,- 120,- -32768,- 252,- 173,- 553,- 307,- -32768,919,- 155,- 12,- 103,- 39,- 298,140,- -32768,- -32768,- -32768,- 23,- 271,- -32768,- 101,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,23,1027,- -32768,707,838,- -32768,260,535,- -32768,- 167,- 291,- 165,- 364,- 356,- 339,891,- 367,- 358,- 125,- 351,- 333,- 161,456,498,912,249,- -32768,- 382,- -32768,6,454,- 58,- 10,- 288,40};
# 1975
static short Cyc_yytable[6696]={59,250,566,344,571,62,616,136,341,465,262,345,568,317,572,573,499,500,307,502,589,59,308,324,140,310,62,578,579,97,569,361,362,363,38,358,549,78,448,537,715,536,641,41,112,143,96,580,581,826,136,59,398,645,646,137,62,319,59,59,59,423,59,62,62,62,76,62,59,147,631,405,38,62,130,131,132,672,133,442,630,41,419,509,142,140,559,675,97,233,234,60,44,237,642,288,432,299,240,97,374,375,406,59,59,169,263,630,62,62,891,587,60,663,366,590,220,100,300,113,59,59,59,59,669,62,62,62,62,59,907,611,129,631,62,433,39,376,377,59,149,59,60,510,62,142,62,434,147,60,60,60,611,60,799,289,531,142,367,60,39,511,59,531,432,432,160,62,720,443,- 151,241,257,269,908,315,97,530,97,242,170,638,101,543,678,544,518,246,915,916,258,453,430,325,60,60,38,556,171,558,437,505,224,433,433,41,19,20,225,453,451,60,60,60,60,434,482,301,299,483,60,582,583,584,97,917,918,326,364,422,60,97,60,365,424,680,624,719,103,468,102,409,327,288,288,434,288,434,409,574,575,576,577,60,423,423,423,43,119,871,624,249,59,170,144,427,145,62,494,434,758,428,146,722,436,495,- 238,438,142,- 238,267,171,399,400,401,38,549,59,59,511,59,249,62,62,41,62,364,695,380,79,429,524,381,289,289,531,289,713,878,751,235,236,879,288,364,104,527,39,450,440,507,105,703,400,401,418,402,253,254,255,256,403,404,306,39,301,477,97,38,120,121,- 216,671,506,- 216,107,40,41,42,198,749,750,382,60,538,241,784,59,471,528,383,384,62,242,501,419,402,239,43,320,511,704,404,289,421,612,426,613,60,60,364,60,666,614,250,454,364,790,711,625,455,561,453,539,809,618,874,106,812,708,549,329,330,786,504,600,109,364,820,241,947,267,738,453,486,796,797,242,97,453,493,97,422,422,422,617,738,424,424,424,364,615,679,364,306,596,110,517,949,59,870,1008,789,657,62,873,364,623,60,629,359,700,38,111,74,142,419,140,59,38,59,41,364,62,114,62,124,40,41,42,1009,637,44,134,531,531,80,98,667,99,47,48,49,414,921,116,727,59,922,714,728,1026,62,668,364,879,429,790,74,756,741,59,115,142,742,74,62,743,364,726,364,74,162,757,675,762,450,142,724,135,364,138,163,740,74,772,160,794,511,59,60,682,813,166,62,795,98,814,881,492,477,122,123,964,801,969,774,98,97,60,167,60,789,966,779,970,971,74,74,364,597,221,135,364,895,625,74,660,976,977,364,967,968,249,74,74,74,74,60,453,364,790,453,754,928,929,755,218,978,979,364,948,60,249,38,931,74,38,359,222,249,97,40,41,42,40,41,42,364,996,554,615,768,935,359,364,74,447,717,223,60,166,299,428,98,219,98,567,454,339,629,342,342,955,806,962,789,665,810,238,364,355,356,773,249,987,245,923,342,342,342,252,172,822,249,924,925,731,775,861,372,373,364,249,591,632,59,633,867,342,247,62,98,364,864,905,259,364,990,98,904,312,142,150,151,152,59,682,47,48,49,62,477,927,153,154,155,156,157,364,364,268,270,452,992,1014,477,940,945,477,364,946,260,897,74,378,379,786,520,521,890,452,43,479,136,38,484,313,950,626,899,627,894,40,41,42,685,628,1022,314,875,650,651,74,321,654,959,658,960,645,646,943,693,694,802,322,664,60,323,249,759,760,980,981,982,97,807,803,804,421,16,17,18,249,385,166,340,60,38,342,818,346,853,887,888,98,342,41,415,43,972,973,974,975,998,347,467,511,44,958,1006,733,734,735,441,963,47,48,49,911,912,288,386,387,388,389,390,391,392,393,394,395,342,348,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,1021,396,349,38,369,1018,913,914,525,919,920,40,41,42,59,603,604,605,350,62,1007,288,351,447,81,98,1016,364,98,428,289,47,48,49,828,829,352,467,353,368,550,551,552,553,360,555,370,136,371,407,412,413,783,417,562,830,267,439,565,456,452,857,446,452,457,59,463,1023,459,461,62,74,469,74,478,765,767,480,82,585,1027,289,481,488,452,249,485,342,487,489,452,586,490,83,84,496,562,85,86,87,427,497,41,503,498,60,428,88,47,48,49,261,434,89,507,90,91,267,514,515,519,92,610,523,288,529,43,93,533,366,593,534,249,541,38,535,542,592,560,634,595,602,74,41,594,601,599,125,620,81,619,621,636,622,44,98,60,635,930,639,652,934,47,48,49,824,640,653,59,655,38,659,661,62,670,342,673,676,40,41,42,681,858,860,289,863,687,866,690,562,471,691,562,81,472,692,697,956,47,48,49,643,644,82,696,698,880,699,98,701,710,702,706,705,709,408,718,712,168,84,39,721,85,86,87,723,725,41,736,739,985,986,88,747,988,989,452,991,89,746,90,91,752,467,763,769,92,770,306,771,452,776,93,452,198,782,342,342,778,60,81,753,38,84,187,188,85,86,87,707,787,41,792,831,798,805,811,808,815,816,933,817,354,937,939,191,1010,467,819,1011,821,1012,1013,823,825,827,855,832,856,74,868,869,872,882,883,884,885,886,730,892,900,889,893,1024,82,342,896,898,903,909,359,906,745,926,910,941,342,965,942,168,84,944,951,85,86,87,953,957,41,954,961,984,732,88,1001,983,562,1000,1002,89,1003,90,91,1004,1017,1005,1015,92,1019,994,1030,81,1031,93,1025,1028,997,737,999,248,108,748,98,7,8,9,10,11,12,13,14,15,16,17,18,19,20,470,467,38,148,791,318,793,118,135,464,40,41,42,43,467,22,781,800,995,777,780,689,471,540,416,598,1020,27,82,854,47,48,49,570,0,28,29,557,0,0,328,0,0,168,84,0,32,85,86,87,34,0,41,0,0,0,0,88,35,36,0,902,0,89,342,90,91,0,329,330,38,92,0,0,0,0,0,93,40,41,42,0,342,0,0,38,0,0,0,0,471,0,39,40,41,42,43,467,47,48,49,0,0,0,0,471,0,45,46,0,0,467,0,47,48,49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,135,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,0,0,0,0,0,467,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,759,760,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,342,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,- 305,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,285,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,- 37,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,271,0,272,273,274,275,276,277,278,279,22,81,23,175,280,0,0,176,24,281,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,282,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,39,40,41,42,43,189,284,139,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,0,0,0,0,0,0,0,0,0,22,81,23,175,0,0,0,176,24,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,656,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,357,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,81,0,175,0,0,0,176,0,0,0,0,27,0,0,177,178,179,180,0,28,29,0,181,0,0,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,39,40,41,42,43,189,0,0,0,0,0,0,0,190,0,45,286,0,0,0,0,192,0,0,193,0,0,194,195,0,0,196,197,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,684,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,39,40,41,42,43,0,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,22,0,39,40,41,42,43,0,0,0,- 11,0,27,0,0,44,0,45,46,0,28,29,0,47,48,49,0,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,38,0,0,0,22,0,0,39,40,41,42,43,0,306,0,0,27,0,0,0,447,82,45,46,28,29,428,0,47,48,49,0,0,688,0,32,168,84,0,34,85,86,87,0,0,41,0,35,36,0,88,0,0,0,0,0,89,0,90,91,0,329,330,0,92,0,0,0,0,0,93,304,0,0,0,0,0,0,0,0,0,39,40,0,42,43,0,306,0,0,0,0,0,0,427,0,45,46,0,0,428,0,47,48,49,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,0,0,0,0,0,0,0,28,29,- 37,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,261,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,39,40,0,42,43,0,22,139,23,0,0,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,0,35,36,37,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,0,0,0,0,0,0,0,24,0,45,46,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,305,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,226,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,227,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,304,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,306,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,677,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,0,0,0,0,0,45,46,28,29,0,0,0,0,0,0,0,0,0,32,0,81,0,34,0,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,39,40,0,42,43,0,0,0,0,27,82,0,0,0,0,45,46,28,29,0,0,0,716,0,0,168,84,0,32,85,86,87,34,0,41,0,0,0,0,88,35,36,0,0,0,89,0,90,91,0,0,0,0,92,0,0,173,174,271,93,272,273,274,275,276,277,278,279,0,81,0,175,280,0,39,176,0,281,43,0,0,0,0,177,178,179,180,0,0,45,46,181,0,282,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,283,84,187,188,85,86,87,0,0,41,0,0,189,284,139,0,0,173,174,0,190,0,0,191,545,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,546,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,788,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,952,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,329,330,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,311,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,343,0,0,173,174,0,190,0,0,191,0,0,0,0,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,491,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,588,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,609,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,662,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,729,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,173,174,0,190,0,0,191,0,0,0,744,192,0,81,193,175,0,194,195,176,0,196,197,0,0,0,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,764,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,766,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,859,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,862,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,865,173,174,0,0,0,0,190,0,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,932,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,936,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,938,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,173,174,0,0,0,0,190,993,0,191,0,0,0,81,192,175,0,193,0,176,194,195,0,0,196,197,0,177,178,179,180,0,0,0,0,181,0,0,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,343,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,707,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,338,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,354,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,173,174,0,38,84,187,188,85,86,87,0,0,41,81,0,189,0,0,0,0,0,0,0,190,0,0,191,177,178,179,180,192,0,0,193,0,0,194,195,0,0,196,197,0,0,182,183,0,184,0,0,833,834,0,0,0,0,0,0,185,186,0,0,0,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,188,85,86,87,0,0,41,0,0,189,0,0,0,0,0,0,0,901,0,0,191,0,0,0,0,192,0,0,193,0,0,194,195,82,0,196,197,0,0,0,0,0,0,0,0,0,0,0,168,84,0,0,85,86,87,0,0,41,0,0,0,0,88,0,0,0,0,0,835,0,90,91,0,0,0,0,92,0,0,836,0,0,93,195,0,0,196,197};
# 2648
static short Cyc_yycheck[6696]={0,115,366,176,371,0,434,65,175,261,129,176,368,158,372,373,287,288,145,290,402,21,145,167,68,145,21,378,379,23,369,192,193,194,107,190,343,21,249,330,593,329,79,116,44,68,23,380,381,751,108,51,213,5,6,65,51,160,58,59,60,229,62,58,59,60,109,62,68,69,454,105,107,68,58,59,60,510,62,17,450,116,227,61,68,129,357,515,82,101,102,0,127,105,131,139,87,141,108,93,88,89,136,103,104,82,129,477,103,104,812,399,21,495,93,403,93,107,141,44,120,121,122,123,508,120,121,122,123,129,93,429,57,517,129,126,114,125,126,139,71,141,51,121,139,129,141,138,148,58,59,60,450,62,707,139,323,141,137,68,114,306,162,330,87,87,125,162,601,107,122,107,107,132,137,129,170,321,172,115,107,472,125,338,518,340,309,112,88,89,125,249,231,170,103,104,107,352,125,354,123,123,121,126,126,116,20,21,127,267,249,120,121,122,123,138,127,142,262,130,129,382,383,384,218,125,126,114,123,229,139,225,141,128,229,128,447,128,120,262,125,218,129,287,288,138,290,138,225,374,375,376,377,162,422,423,424,118,51,128,471,115,262,107,125,127,127,262,120,138,652,133,133,607,234,127,120,237,262,123,134,125,83,84,85,107,593,287,288,434,290,145,287,288,116,290,123,549,138,121,231,313,142,287,288,472,290,134,128,643,103,104,132,357,123,120,316,114,249,128,107,125,83,84,85,122,127,120,121,122,123,132,133,120,114,260,267,331,107,120,121,128,122,303,131,127,115,116,117,89,641,642,135,262,331,107,108,357,127,319,143,144,357,115,289,510,127,107,118,162,515,132,133,357,228,125,230,127,287,288,123,290,501,133,498,123,123,694,131,447,128,128,450,124,728,439,780,125,732,560,707,132,133,691,120,417,49,123,742,107,108,265,623,471,274,703,704,115,412,477,280,415,422,423,424,437,637,422,423,424,123,431,523,123,120,412,106,123,131,439,774,131,694,487,439,779,123,447,357,449,190,128,107,127,0,439,601,501,458,107,460,116,123,458,106,460,59,115,116,117,131,471,127,121,641,642,22,23,501,25,135,136,137,223,138,121,613,487,142,592,613,128,487,505,123,132,427,804,44,128,627,501,127,487,627,51,501,627,123,612,123,57,121,128,942,128,447,501,609,65,123,67,109,626,70,128,125,700,673,529,439,526,123,79,529,700,82,128,790,279,471,120,121,907,711,912,669,93,542,458,121,460,804,909,677,913,914,103,104,123,120,120,108,123,128,623,112,120,919,920,123,910,911,427,120,121,122,123,487,637,123,888,640,120,855,128,123,127,921,922,123,879,501,447,107,128,142,107,338,123,454,595,115,116,117,115,116,117,123,943,350,611,120,128,354,123,162,127,595,124,529,167,666,133,170,107,172,367,123,173,630,175,176,128,725,906,888,497,729,114,123,185,186,666,498,128,127,135,192,193,194,116,88,744,508,143,144,617,670,120,90,91,123,517,404,458,666,460,769,213,128,666,218,123,120,836,120,123,128,225,835,146,666,63,64,65,686,682,135,136,137,686,623,854,74,75,76,77,78,123,123,137,138,249,128,128,637,120,120,640,123,123,123,827,260,86,87,1005,63,64,811,267,118,269,782,107,272,127,883,125,831,127,823,115,116,117,529,133,1009,127,122,481,482,289,123,485,901,487,903,5,6,874,122,123,714,122,496,666,124,613,22,23,923,924,925,759,726,122,123,623,17,18,19,627,56,321,127,686,107,323,740,127,759,122,123,331,330,116,224,118,915,916,917,918,949,127,261,942,127,900,961,107,108,109,240,906,135,136,137,140,141,855,94,95,96,97,98,99,100,101,102,103,366,127,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,1008,124,127,107,139,1001,90,91,314,86,87,115,116,117,855,422,423,424,127,855,962,906,127,127,38,412,122,123,415,133,855,135,136,137,754,755,127,343,127,92,346,347,348,349,127,351,140,946,141,121,107,107,686,127,360,756,751,126,364,122,447,762,128,450,122,906,114,1009,122,124,906,458,122,460,121,656,657,121,92,385,1022,906,127,107,471,780,127,472,127,120,477,397,120,107,108,127,402,111,112,113,127,131,116,122,127,855,133,121,135,136,137,124,138,127,107,129,130,812,128,131,128,135,428,134,1009,122,118,141,122,93,407,126,827,122,107,126,123,125,128,107,123,122,529,116,128,123,128,120,127,38,121,128,126,123,127,542,906,120,859,120,127,862,135,136,137,746,123,121,1009,30,107,120,42,1009,127,560,138,128,115,116,117,128,763,764,1009,766,134,768,128,495,127,123,498,38,131,107,123,898,135,136,137,478,479,92,128,123,787,123,595,128,123,128,122,128,128,104,122,128,107,108,114,128,111,112,113,128,134,116,128,128,931,932,121,107,935,936,623,938,127,127,129,130,122,549,127,121,135,120,120,128,637,122,141,640,835,121,641,642,126,1009,38,648,107,108,109,110,111,112,113,121,125,116,25,121,128,128,134,128,128,128,861,128,127,864,865,130,987,593,128,990,128,992,993,134,120,127,131,128,122,686,120,120,114,128,127,123,128,128,614,108,121,128,128,1014,92,700,122,28,127,92,901,131,628,92,139,122,711,908,123,107,108,122,25,111,112,113,122,122,116,123,128,120,619,121,127,926,652,128,108,127,122,129,130,128,128,123,122,135,128,940,0,38,0,141,128,128,946,622,949,114,35,640,759,8,9,10,11,12,13,14,15,16,17,18,19,20,21,265,694,107,70,697,159,699,48,782,260,115,116,117,118,707,37,682,710,942,673,677,542,127,332,225,415,1005,49,92,759,135,136,137,370,- 1,57,58,353,- 1,- 1,104,- 1,- 1,107,108,- 1,68,111,112,113,72,- 1,116,- 1,- 1,- 1,- 1,121,80,81,- 1,833,- 1,127,836,129,130,- 1,132,133,107,135,- 1,- 1,- 1,- 1,- 1,141,115,116,117,- 1,854,- 1,- 1,107,- 1,- 1,- 1,- 1,127,- 1,114,115,116,117,118,790,135,136,137,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,804,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,907,- 1,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,946,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,961,- 1,- 1,- 1,- 1,- 1,888,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,1008,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,37,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,49,- 1,- 1,127,- 1,129,130,- 1,57,58,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,- 1,- 1,114,115,116,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,127,92,129,130,57,58,133,- 1,135,136,137,- 1,- 1,104,- 1,68,107,108,- 1,72,111,112,113,- 1,- 1,116,- 1,80,81,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,132,133,- 1,135,- 1,- 1,- 1,- 1,- 1,141,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,133,- 1,135,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,124,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,37,121,39,- 1,- 1,- 1,- 1,- 1,45,129,130,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,129,130,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,38,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,114,115,- 1,117,118,- 1,- 1,- 1,- 1,49,92,- 1,- 1,- 1,- 1,129,130,57,58,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,68,111,112,113,72,- 1,116,- 1,- 1,- 1,- 1,121,80,81,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,25,26,27,141,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,114,44,- 1,46,118,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,129,130,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,32,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,122,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,25,26,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,38,138,40,- 1,141,142,44,- 1,145,146,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,38,135,40,- 1,138,- 1,44,141,142,- 1,- 1,145,146,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,107,108,109,110,111,112,113,- 1,- 1,116,38,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,52,53,54,55,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,70,71,- 1,73,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,92,- 1,145,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4E9[8]="stdcall";static char _tmp4EA[6]="cdecl";static char _tmp4EB[9]="fastcall";static char _tmp4EC[9]="noreturn";static char _tmp4ED[6]="const";static char _tmp4EE[8]="aligned";static char _tmp4EF[7]="packed";static char _tmp4F0[7]="shared";static char _tmp4F1[7]="unused";static char _tmp4F2[5]="weak";static char _tmp4F3[10]="dllimport";static char _tmp4F4[10]="dllexport";static char _tmp4F5[23]="no_instrument_function";static char _tmp4F6[12]="constructor";static char _tmp4F7[11]="destructor";static char _tmp4F8[22]="no_check_memory_usage";static char _tmp4F9[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9B1(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9B0,unsigned int*_tmp9AF,union Cyc_YYSTYPE**_tmp9AE){for(*_tmp9B0=0;*_tmp9B0 < *_tmp9AF;(*_tmp9B0)++){(*_tmp9AE)[*_tmp9B0]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9B6(unsigned int*_tmp9B5,unsigned int*_tmp9B4,struct Cyc_Yyltype**_tmp9B3){for(*_tmp9B5=0;*_tmp9B5 < *_tmp9B4;(*_tmp9B5)++){(*_tmp9B3)[*_tmp9B5]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9BD(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9BC,unsigned int*_tmp9BB,short**_tmp9B9){for(*_tmp9BC=0;*_tmp9BC < *_tmp9BB;(*_tmp9BC)++){(*_tmp9B9)[*_tmp9BC]=(short)(
# 216
*_tmp9BC <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9BC)): 0);}}static void _tmp9C3(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9C2,unsigned int*_tmp9C1,union Cyc_YYSTYPE**_tmp9BF){for(*_tmp9C2=0;*_tmp9C2 < *_tmp9C1;(*_tmp9C2)++){(*_tmp9BF)[*_tmp9C2]=(union Cyc_YYSTYPE)(
# 219
*_tmp9C2 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9C2)):*yylval);}}static void _tmp9C9(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9C8,unsigned int*_tmp9C7,struct Cyc_Yyltype**_tmp9C5){for(*_tmp9C8=0;*_tmp9C8 < *_tmp9C7;(*_tmp9C8)++){(*_tmp9C5)[*_tmp9C8]=(struct Cyc_Yyltype)(
# 222
*_tmp9C8 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9C8)):
 Cyc_yynewloc());}}static void _tmpEAB(unsigned int*_tmpEAA,unsigned int*_tmpEA9,char**_tmpEA7){for(*_tmpEAA=0;*_tmpEAA < *_tmpEA9;(*_tmpEAA)++){(*_tmpEA7)[*_tmpEAA]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp431=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp431;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9AA;union Cyc_YYSTYPE yylval=((_tmp9AA.YYINITIALSVAL).val=0,(((_tmp9AA.YYINITIALSVAL).tag=64,_tmp9AA)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9AC;unsigned int _tmp9AB;struct _dyneither_ptr yyss=(_tmp9AB=200,((_tmp9AC=_region_calloc(yyregion,sizeof(short),_tmp9AB),_tag_dyneither(_tmp9AC,sizeof(short),_tmp9AB))));
# 158
int yyvsp_offset;
unsigned int _tmp9B0;unsigned int _tmp9AF;union Cyc_YYSTYPE*_tmp9AE;unsigned int _tmp9AD;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9AD=(unsigned int)200,((_tmp9AE=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9AD)),((((_tmp9AF=_tmp9AD,_tmp9B1(& yylval,& _tmp9B0,& _tmp9AF,& _tmp9AE))),_tmp9AE)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9B5;unsigned int _tmp9B4;struct Cyc_Yyltype*_tmp9B3;unsigned int _tmp9B2;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9B2=(unsigned int)200,((_tmp9B3=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9B2)),((((_tmp9B4=_tmp9B2,_tmp9B6(& _tmp9B5,& _tmp9B4,& _tmp9B3))),_tmp9B3)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9B7;Cyc_yyerror(((_tmp9B7="parser stack overflow",_tag_dyneither(_tmp9B7,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9BC;unsigned int _tmp9BB;struct _dyneither_ptr _tmp9BA;short*_tmp9B9;unsigned int _tmp9B8;struct _dyneither_ptr yyss1=(_tmp9B8=(unsigned int)yystacksize,((_tmp9B9=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9B8)),((_tmp9BA=_tag_dyneither(_tmp9B9,sizeof(short),_tmp9B8),((((_tmp9BB=_tmp9B8,_tmp9BD(& yyssp_offset,& yyss,& _tmp9BC,& _tmp9BB,& _tmp9B9))),_tmp9BA)))))));
# 217
unsigned int _tmp9C2;unsigned int _tmp9C1;struct _dyneither_ptr _tmp9C0;union Cyc_YYSTYPE*_tmp9BF;unsigned int _tmp9BE;struct _dyneither_ptr yyvs1=
(_tmp9BE=(unsigned int)yystacksize,((_tmp9BF=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9BE)),((_tmp9C0=_tag_dyneither(_tmp9BF,sizeof(union Cyc_YYSTYPE),_tmp9BE),((((_tmp9C1=_tmp9BE,_tmp9C3(& yyvs,& yyssp_offset,& yylval,& _tmp9C2,& _tmp9C1,& _tmp9BF))),_tmp9C0)))))));
# 221
unsigned int _tmp9C8;unsigned int _tmp9C7;struct _dyneither_ptr _tmp9C6;struct Cyc_Yyltype*_tmp9C5;unsigned int _tmp9C4;struct _dyneither_ptr yyls1=(_tmp9C4=(unsigned int)yystacksize,((_tmp9C5=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9C4)),((_tmp9C6=_tag_dyneither(_tmp9C5,sizeof(struct Cyc_Yyltype),_tmp9C4),((((_tmp9C7=_tmp9C4,_tmp9C9(& yyssp_offset,& yyls,& _tmp9C8,& _tmp9C7,& _tmp9C5))),_tmp9C6)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
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
if((yyn < 0  || yyn > 6695) || Cyc_yycheck[_check_known_subscript_notnull(6696,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6696,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1031){
int _tmp43F=0;_npop_handler(0);return _tmp43F;}
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1032,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(519,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL214: {
# 1176 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1180
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY19(yyyyvsp[0]);
# 1183
break;}case 2: _LL215: {
# 1182 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1186
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp440;_push_handler(& _tmp440);{int _tmp442=0;if(setjmp(_tmp440.handler))_tmp442=1;if(!_tmp442){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp441=(void*)_exn_thrown;void*_tmp444=_tmp441;void*_tmp446;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp445=(struct Cyc_Core_Failure_exn_struct*)_tmp444;if(_tmp445->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1191
 x=0;goto _LL217;_LL21A: _tmp446=_tmp444;_LL21B:(void)_throw(_tmp446);_LL217:;}};}
# 1193
{struct _handler_cons _tmp447;_push_handler(& _tmp447);{int _tmp449=0;if(setjmp(_tmp447.handler))_tmp449=1;if(!_tmp449){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp448=(void*)_exn_thrown;void*_tmp44B=_tmp448;void*_tmp44D;_LL21D: {struct Cyc_Core_Failure_exn_struct*_tmp44C=(struct Cyc_Core_Failure_exn_struct*)_tmp44B;if(_tmp44C->tag != Cyc_Core_Failure)goto _LL21F;}_LL21E:
# 1197
 y=0;goto _LL21C;_LL21F: _tmp44D=_tmp44B;_LL220:(void)_throw(_tmp44D);_LL21C:;}};}
# 1199
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1201
break;}case 3: _LL216: {
# 1200 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1204
{struct Cyc_Absyn_Decl*_tmp9D3;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9D2;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9D1;struct Cyc_List_List*_tmp9D0;yyval=Cyc_YY19(((_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((_tmp9D0->hd=((_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3->r=(void*)((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=((_tmp9D2.tag=11,((_tmp9D2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9D2.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9D2)))))),_tmp9D1)))),((_tmp9D3->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9D3)))))),((_tmp9D0->tl=0,_tmp9D0)))))));}
Cyc_Lex_leave_using();
# 1207
break;}case 4: _LL221: {
# 1204 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1208
{struct Cyc_Absyn_Decl*_tmp9DD;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9DC;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9DB;struct Cyc_List_List*_tmp9DA;yyval=Cyc_YY19(((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA->hd=((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD->r=(void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DC.tag=11,((_tmp9DC.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9DC.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9DC)))))),_tmp9DB)))),((_tmp9DD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9DD)))))),((_tmp9DA->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmp9DA)))))));}
break;}case 5: _LL222: {
# 1207 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1211
{struct Cyc_Absyn_Decl*_tmp9EC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9EB;struct _dyneither_ptr*_tmp9EA;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9E9;struct Cyc_List_List*_tmp9E8;yyval=Cyc_YY19(((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8->hd=((_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC->r=(void*)((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EB.tag=10,((_tmp9EB.f1=((_tmp9EA=_cycalloc(sizeof(*_tmp9EA)),((_tmp9EA[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmp9EA)))),((_tmp9EB.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9EB)))))),_tmp9E9)))),((_tmp9EC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9EC)))))),((_tmp9E8->tl=0,_tmp9E8)))))));}
Cyc_Lex_leave_namespace();
# 1214
break;}case 6: _LL223: {
# 1211 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1216
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp45B;_push_handler(& _tmp45B);{int _tmp45D=0;if(setjmp(_tmp45B.handler))_tmp45D=1;if(!_tmp45D){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp45C=(void*)_exn_thrown;void*_tmp45F=_tmp45C;void*_tmp461;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp460=(struct Cyc_Core_Failure_exn_struct*)_tmp45F;if(_tmp460->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
{const char*_tmp9ED;nspace=((_tmp9ED="",_tag_dyneither(_tmp9ED,sizeof(char),1)));}goto _LL225;_LL228: _tmp461=_tmp45F;_LL229:(void)_throw(_tmp461);_LL225:;}};}
# 1221
{struct _handler_cons _tmp463;_push_handler(& _tmp463);{int _tmp465=0;if(setjmp(_tmp463.handler))_tmp465=1;if(!_tmp465){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp464=(void*)_exn_thrown;void*_tmp467=_tmp464;void*_tmp469;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp468=(struct Cyc_Core_Failure_exn_struct*)_tmp467;if(_tmp468->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 x=0;goto _LL22A;_LL22D: _tmp469=_tmp467;_LL22E:(void)_throw(_tmp469);_LL22A:;}};}
# 1224
{struct _handler_cons _tmp46A;_push_handler(& _tmp46A);{int _tmp46C=0;if(setjmp(_tmp46A.handler))_tmp46C=1;if(!_tmp46C){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp46B=(void*)_exn_thrown;void*_tmp46E=_tmp46B;void*_tmp470;_LL230: {struct Cyc_Core_Failure_exn_struct*_tmp46F=(struct Cyc_Core_Failure_exn_struct*)_tmp46E;if(_tmp46F->tag != Cyc_Core_Failure)goto _LL232;}_LL231:
 y=0;goto _LL22F;_LL232: _tmp470=_tmp46E;_LL233:(void)_throw(_tmp470);_LL22F:;}};}
# 1227
{struct Cyc_Absyn_Decl*_tmp9FC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmp9FB;struct _dyneither_ptr*_tmp9FA;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F9;struct Cyc_List_List*_tmp9F8;yyval=Cyc_YY19(((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->hd=((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC->r=(void*)((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9[0]=((_tmp9FB.tag=10,((_tmp9FB.f1=((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA[0]=nspace,_tmp9FA)))),((_tmp9FB.f2=x,_tmp9FB)))))),_tmp9F9)))),((_tmp9FC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmp9FC)))))),((_tmp9F8->tl=y,_tmp9F8)))))));}
# 1229
break;}case 7: _LL224: {
# 1226 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1228
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1230
struct _dyneither_ptr _tmp476=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmp9FD;if(Cyc_strcmp((struct _dyneither_ptr)_tmp476,((_tmp9FD="C",_tag_dyneither(_tmp9FD,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA07;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA06;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA05;struct Cyc_List_List*_tmpA04;yyval=Cyc_YY19(((_tmpA04=_cycalloc(sizeof(*_tmpA04)),((_tmpA04->hd=((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->r=(void*)((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA06.tag=12,((_tmpA06.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA06)))),_tmpA05)))),((_tmpA07->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA07)))))),((_tmpA04->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA04)))))));}else{
# 1234
{const char*_tmpA08;if(Cyc_strcmp((struct _dyneither_ptr)_tmp476,((_tmpA08="C include",_tag_dyneither(_tmpA08,sizeof(char),10))))!= 0){
const char*_tmpA09;Cyc_Parse_err(((_tmpA09="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA09,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1237
struct Cyc_Absyn_Decl*_tmpA13;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA12;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA11;struct Cyc_List_List*_tmpA10;yyval=Cyc_YY19(((_tmpA10=_cycalloc(sizeof(*_tmpA10)),((_tmpA10->hd=((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13->r=(void*)((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA12.tag=13,((_tmpA12.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA12.f2=0,_tmpA12)))))),_tmpA11)))),((_tmpA13->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA13)))))),((_tmpA10->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA10)))))));};}}
# 1240
break;}case 8: _LL234: {
# 1237 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1241
{const char*_tmpA14;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA14="C include",_tag_dyneither(_tmpA14,sizeof(char),10))))!= 0){
const char*_tmpA15;Cyc_Parse_err(((_tmpA15="expecting \"C include\"",_tag_dyneither(_tmpA15,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA1F;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA1E;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA1D;struct Cyc_List_List*_tmpA1C;yyval=Cyc_YY19(((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C->hd=((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->r=(void*)((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1E.tag=13,((_tmpA1E.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA1E.f2=exs,_tmpA1E)))))),_tmpA1D)))),((_tmpA1F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1F)))))),((_tmpA1C->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA1C)))))));}
# 1246
break;};}case 9: _LL235: {
# 1243 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1245
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1247
{struct Cyc_Absyn_Decl*_tmpA22;struct Cyc_List_List*_tmpA21;yyval=Cyc_YY19(((_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->hd=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA22->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA22)))))),((_tmpA21->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA21)))))));}
break;}case 10: _LL236: {
# 1245 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1249
{struct Cyc_Absyn_Decl*_tmpA25;struct Cyc_List_List*_tmpA24;yyval=Cyc_YY19(((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24->hd=((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA25->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA25)))))),((_tmpA24->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA24)))))));}
break;}case 11: _LL237:
# 1246 "parse.y"
 yyval=Cyc_YY19(0);
break;case 12: _LL238: {
# 1250 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1254
yyval=yyyyvsp[2];
break;}case 13: _LL239: {
# 1251 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1255
yyval=Cyc_YY55(0);
break;}case 14: _LL23A: {
# 1255 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1257
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1259
{struct _tuple27*_tmpA28;struct Cyc_List_List*_tmpA27;yyval=Cyc_YY55(((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27->hd=((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA28->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA28->f3=0,_tmpA28)))))))),((_tmpA27->tl=0,_tmpA27)))))));}
break;}case 15: _LL23B: {
# 1256 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1260
{struct _tuple27*_tmpA2B;struct Cyc_List_List*_tmpA2A;yyval=Cyc_YY55(((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->hd=((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA2B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA2B->f3=0,_tmpA2B)))))))),((_tmpA2A->tl=0,_tmpA2A)))))));}
break;}case 16: _LL23C: {
# 1258 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1262
{struct _tuple27*_tmpA2E;struct Cyc_List_List*_tmpA2D;yyval=Cyc_YY55(((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->hd=((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA2E->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA2E->f3=0,_tmpA2E)))))))),((_tmpA2D->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA2D)))))));}
break;}case 17: _LL23D: {
# 1262 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1266
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA34;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA33;struct Cyc_List_List*_tmpA32;yyval=Cyc_YY19(((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->hd=Cyc_Absyn_new_decl((void*)((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA33.tag=1,((_tmpA33.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA33)))),_tmpA34)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA32->tl=0,_tmpA32)))))));}
break;}case 18: _LL23E: {
# 1263 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1267
yyval=yyyyvsp[0];
break;}case 19: _LL23F: {
# 1264 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1268
yyval=Cyc_YY19(0);
break;}case 20: _LL240: {
# 1269 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1273
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 21: _LL241: {
# 1271 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1273
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1275
struct Cyc_Parse_Declaration_spec _tmp495=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp495,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 22: _LL242: {
# 1284 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1288
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL243: {
# 1286 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1290
struct Cyc_Parse_Declaration_spec _tmp496=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp496,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL244: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1298
struct Cyc_Parse_Declaration_spec _tmp497=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp497,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL245: {
# 1297 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1301
struct Cyc_Parse_Declaration_spec _tmp498=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp498,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL246: {
# 1302 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1306
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 27: _LL247: {
# 1305 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1309
Cyc_Lex_leave_using();
break;}case 28: _LL248: {
# 1308 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1312
{struct _dyneither_ptr*_tmpA35;Cyc_Lex_enter_namespace(((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA35)))));}yyval=yyyyvsp[1];
break;}case 29: _LL249: {
# 1311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1315
Cyc_Lex_leave_namespace();
break;}case 30: _LL24A: {
# 1317 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1321
int _tmp49A=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp49A,(unsigned int)_tmp49A));
break;}case 31: _LL24B: {
# 1320 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1324
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)(yyyylsp[0]).first_line));
break;}case 32: _LL24C: {
# 1323 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1327
{struct Cyc_List_List*_tmpA36;yyval=Cyc_YY19(((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA36->tl=0,_tmpA36)))))));}
break;}case 33: _LL24D: {
# 1325 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1329
struct Cyc_List_List*_tmp49C=0;
{struct Cyc_List_List*_tmp49D=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp49D != 0;_tmp49D=_tmp49D->tl){
struct _dyneither_ptr*_tmp49E=(struct _dyneither_ptr*)_tmp49D->hd;
struct _tuple0*_tmpA37;struct _tuple0*qv=(_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37->f1=Cyc_Absyn_Rel_n(0),((_tmpA37->f2=_tmp49E,_tmpA37)))));
struct Cyc_Absyn_Vardecl*_tmp49F=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA38;_tmp49C=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->hd=_tmp49F,((_tmpA38->tl=_tmp49C,_tmpA38))))));}}
# 1336
_tmp49C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp49C);
{struct Cyc_List_List*_tmpA39;yyval=Cyc_YY19(((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39->hd=Cyc_Absyn_letv_decl(_tmp49C,(unsigned int)(yyyylsp[0]).first_line),((_tmpA39->tl=0,_tmpA39)))))));}
# 1339
break;}case 34: _LL24E: {
# 1338 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1342
struct _dyneither_ptr _tmp4A3=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA3A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA3A="`H",_tag_dyneither(_tmpA3A,sizeof(char),3))))== 0){
const char*_tmpA3D;void*_tmpA3C;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA3C=0,Cyc_aprintf(((_tmpA3D="bad occurrence of heap region",_tag_dyneither(_tmpA3D,sizeof(char),30))),_tag_dyneither(_tmpA3C,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA3E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4A3,((_tmpA3E="`U",_tag_dyneither(_tmpA3E,sizeof(char),3))))== 0){
const char*_tmpA41;void*_tmpA40;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA40=0,Cyc_aprintf(((_tmpA41="bad occurrence of unique region",_tag_dyneither(_tmpA41,sizeof(char),32))),_tag_dyneither(_tmpA40,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA44;struct Cyc_Absyn_Tvar*_tmpA43;struct Cyc_Absyn_Tvar*tv=(_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->name=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=_tmp4A3,_tmpA44)))),((_tmpA43->identity=- 1,((_tmpA43->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA43)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA46;void*t=(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=2,((_tmpA47.f1=tv,_tmpA47)))),_tmpA46))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA4D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA4C;struct _dyneither_ptr*_tmpA4A;struct _tuple0*_tmpA49;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->f1=Cyc_Absyn_Loc_n,((_tmpA49->f2=((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA4A)))),_tmpA49)))))),(void*)((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=15,((_tmpA4D.f1=t,_tmpA4D)))),_tmpA4C)))),0);
{struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY19(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA4E->tl=0,_tmpA4E)))))));}
# 1352
break;};}case 35: _LL24F: {
# 1350 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1354
int _tmp4B3=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4B4=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4B5=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA4F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA4F="H",_tag_dyneither(_tmpA4F,sizeof(char),2))))== 0){
const char*_tmpA50;Cyc_Parse_err(((_tmpA50="bad occurrence of heap region `H",_tag_dyneither(_tmpA50,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA51;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA51="U",_tag_dyneither(_tmpA51,sizeof(char),2))))== 0){
const char*_tmpA52;Cyc_Parse_err(((_tmpA52="bad occurrence of unique region `U",_tag_dyneither(_tmpA52,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4B3  && _tmp4B5 != 0){
const char*_tmpA53;Cyc_Parse_err(((_tmpA53="open regions cannot be @resetable",_tag_dyneither(_tmpA53,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA60;const char*_tmpA5F;void*_tmpA5E[1];struct Cyc_String_pa_PrintArg_struct _tmpA5D;struct Cyc_Absyn_Tvar*_tmpA5C;struct Cyc_Absyn_Tvar*tv=(_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->name=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60[0]=(struct _dyneither_ptr)((_tmpA5D.tag=0,((_tmpA5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4B4),((_tmpA5E[0]=& _tmpA5D,Cyc_aprintf(((_tmpA5F="`%s",_tag_dyneither(_tmpA5F,sizeof(char),4))),_tag_dyneither(_tmpA5E,sizeof(void*),1)))))))),_tmpA60)))),((_tmpA5C->identity=- 1,((_tmpA5C->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA5C)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA63;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA62;void*t=(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=2,((_tmpA63.f1=tv,_tmpA63)))),_tmpA62))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA69;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA68;struct _dyneither_ptr*_tmpA66;struct _tuple0*_tmpA65;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->f1=Cyc_Absyn_Loc_n,((_tmpA65->f2=((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=_tmp4B4,_tmpA66)))),_tmpA65)))))),(void*)((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA69.tag=15,((_tmpA69.f1=t,_tmpA69)))),_tmpA68)))),0);
# 1368
{struct Cyc_List_List*_tmpA6A;yyval=Cyc_YY19(((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4B3,_tmp4B5,(unsigned int)(yyyylsp[0]).first_line),((_tmpA6A->tl=0,_tmpA6A)))))));}
# 1370
break;};}case 36: _LL250: {
# 1368 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1372
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA72;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpA71;struct _dyneither_ptr*_tmpA70;struct Cyc_Absyn_Tvar*_tmpA6F;struct Cyc_Absyn_Tvar*tv=(_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->name=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpA70)))),((_tmpA6F->identity=- 1,((_tmpA6F->kind=(void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA71.tag=0,((_tmpA71.f1=& Cyc_Tcutil_rk,_tmpA71)))),_tmpA72)))),_tmpA6F)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpA75;void*_tmpA74;(_tmpA74=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpA75="too many declarations in alias",_tag_dyneither(_tmpA75,sizeof(char),31))),_tag_dyneither(_tmpA74,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_tmp4C9=d->r;struct Cyc_Absyn_Vardecl*_tmp4CB;_LL253: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4CA=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C9;if(_tmp4CA->tag != 0)goto _LL255;else{_tmp4CB=_tmp4CA->f1;}}_LL254:
# 1379
{struct Cyc_List_List*_tmpA76;yyval=Cyc_YY19(((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=Cyc_Absyn_alias_decl(tv,_tmp4CB,(unsigned int)(yyyylsp[0]).first_line),((_tmpA76->tl=0,_tmpA76)))))));}
goto _LL252;_LL255:;_LL256:
# 1382
{const char*_tmpA77;Cyc_Parse_err(((_tmpA77="expecting variable declaration",_tag_dyneither(_tmpA77,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
yyval=Cyc_YY19(0);_LL252:;}
# 1386
break;};}case 37: _LL251:
# 1385 "parse.y"
 yyval=Cyc_YY34(0);
break;case 38: _LL257: {
# 1386 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1390
yyval=Cyc_YY34(1);
break;}case 39: _LL258: {
# 1391 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1395
yyval=yyyyvsp[0];
break;}case 40: _LL259: {
# 1393 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1397
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 41: _LL25A: {
# 1399 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1403
{struct Cyc_Parse_Declaration_spec _tmpA78;yyval=Cyc_YY20(((_tmpA78.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA78.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA78.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA78.is_inline=0,((_tmpA78.attributes=0,_tmpA78)))))))))));}
break;}case 42: _LL25B: {
# 1402 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1406
struct Cyc_Parse_Declaration_spec _tmp4D3=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4D3.sc != 0){
const char*_tmpA7B;void*_tmpA7A;(_tmpA7A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA7B="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA7B,sizeof(char),51))),_tag_dyneither(_tmpA7A,sizeof(void*),0)));}
# 1410
{struct Cyc_Parse_Declaration_spec _tmpA7C;yyval=Cyc_YY20(((_tmpA7C.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA7C.tq=_tmp4D3.tq,((_tmpA7C.type_specs=_tmp4D3.type_specs,((_tmpA7C.is_inline=_tmp4D3.is_inline,((_tmpA7C.attributes=_tmp4D3.attributes,_tmpA7C)))))))))));}
# 1414
break;}case 43: _LL25C: {
# 1411 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1413
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1415
{struct Cyc_Parse_Declaration_spec _tmpA7D;yyval=Cyc_YY20(((_tmpA7D.sc=0,((_tmpA7D.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA7D.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA7D.is_inline=0,((_tmpA7D.attributes=0,_tmpA7D)))))))))));}
break;}case 44: _LL25D: {
# 1414 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1418
struct Cyc_Parse_Declaration_spec _tmp4D8=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA7E;yyval=Cyc_YY20(((_tmpA7E.sc=_tmp4D8.sc,((_tmpA7E.tq=_tmp4D8.tq,((_tmpA7E.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4D8.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA7E.is_inline=_tmp4D8.is_inline,((_tmpA7E.attributes=_tmp4D8.attributes,_tmpA7E)))))))))));}
# 1424
break;}case 45: _LL25E: {
# 1421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1425
{struct Cyc_Parse_Declaration_spec _tmpA7F;yyval=Cyc_YY20(((_tmpA7F.sc=0,((_tmpA7F.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA7F.type_specs=Cyc_Parse_empty_spec(0),((_tmpA7F.is_inline=0,((_tmpA7F.attributes=0,_tmpA7F)))))))))));}
break;}case 46: _LL25F: {
# 1423 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1427
struct Cyc_Parse_Declaration_spec _tmp4DB=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA80;yyval=Cyc_YY20(((_tmpA80.sc=_tmp4DB.sc,((_tmpA80.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4DB.tq),((_tmpA80.type_specs=_tmp4DB.type_specs,((_tmpA80.is_inline=_tmp4DB.is_inline,((_tmpA80.attributes=_tmp4DB.attributes,_tmpA80)))))))))));}
# 1432
break;}case 47: _LL260: {
# 1429 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1433
{struct Cyc_Parse_Declaration_spec _tmpA81;yyval=Cyc_YY20(((_tmpA81.sc=0,((_tmpA81.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA81.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA81.is_inline=1,((_tmpA81.attributes=0,_tmpA81)))))))))));}
break;}case 48: _LL261: {
# 1432 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1436
struct Cyc_Parse_Declaration_spec _tmp4DE=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA82;yyval=Cyc_YY20(((_tmpA82.sc=_tmp4DE.sc,((_tmpA82.tq=_tmp4DE.tq,((_tmpA82.type_specs=_tmp4DE.type_specs,((_tmpA82.is_inline=1,((_tmpA82.attributes=_tmp4DE.attributes,_tmpA82)))))))))));}
# 1440
break;}case 49: _LL262: {
# 1437 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1441
{struct Cyc_Parse_Declaration_spec _tmpA83;yyval=Cyc_YY20(((_tmpA83.sc=0,((_tmpA83.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA83.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA83.is_inline=0,((_tmpA83.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpA83)))))))))));}
break;}case 50: _LL263: {
# 1440 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1444
struct Cyc_Parse_Declaration_spec _tmp4E1=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA84;yyval=Cyc_YY20(((_tmpA84.sc=_tmp4E1.sc,((_tmpA84.tq=_tmp4E1.tq,((_tmpA84.type_specs=_tmp4E1.type_specs,((_tmpA84.is_inline=_tmp4E1.is_inline,((_tmpA84.attributes=
# 1447
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4E1.attributes),_tmpA84)))))))))));}
break;}case 51: _LL264: {
# 1447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1451
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 52: _LL265: {
# 1449 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1453
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 53: _LL266: {
# 1451 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1455
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 54: _LL267: {
# 1453 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1457
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 55: _LL268: {
# 1456 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1460
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpA85;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA85="C",_tag_dyneither(_tmpA85,sizeof(char),2))))!= 0){
const char*_tmpA86;Cyc_Parse_err(((_tmpA86="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA86,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1465
break;}case 56: _LL269: {
# 1461 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1465
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 57: _LL26A: {
# 1464 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1468
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 58: _LL26B:
# 1470 "parse.y"
 yyval=Cyc_YY48(0);
break;case 59: _LL26C: {
# 1471 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1475
yyval=yyyyvsp[0];
break;}case 60: _LL26D: {
# 1476 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1480
yyval=yyyyvsp[3];
break;}case 61: _LL26E: {
# 1480 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1484
{struct Cyc_List_List*_tmpA87;yyval=Cyc_YY48(((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA87->tl=0,_tmpA87)))))));}
break;}case 62: _LL26F: {
# 1481 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1485
{struct Cyc_List_List*_tmpA88;yyval=Cyc_YY48(((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpA88->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpA88)))))));}
break;}case 63: _LL270: {
# 1486 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1490
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4E9,_tmp4E9,_tmp4E9 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4EA,_tmp4EA,_tmp4EA + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4EB,_tmp4EB,_tmp4EB + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4EC,_tmp4EC,_tmp4EC + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4ED,_tmp4ED,_tmp4ED + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 8},(void*)& att_aligned},{{_tmp4EF,_tmp4EF,_tmp4EF + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4F3,_tmp4F3,_tmp4F3 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4F4,_tmp4F4,_tmp4F4 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4F5,_tmp4F5,_tmp4F5 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4F6,_tmp4F6,_tmp4F6 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4F7,_tmp4F7,_tmp4F7 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4F8,_tmp4F8,_tmp4F8 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4F9,_tmp4F9,_tmp4F9 + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1510
struct _dyneither_ptr _tmp4E7=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1512
if((((_get_dyneither_size(_tmp4E7,sizeof(char))> 4  && ((const char*)_tmp4E7.curr)[0]== '_') && ((const char*)_tmp4E7.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4E7,sizeof(char),(int)(_get_dyneither_size(_tmp4E7,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4E7,sizeof(char),(int)(_get_dyneither_size(_tmp4E7,sizeof(char))- 3)))== '_')
# 1514
_tmp4E7=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4E7,2,_get_dyneither_size(_tmp4E7,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4E7,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1521
if(i == 17){
{const char*_tmpA89;Cyc_Parse_err(((_tmpA89="unrecognized attribute",_tag_dyneither(_tmpA89,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1526
break;};}case 64: _LL271: {
# 1522 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1526
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 65: _LL272: {
# 1524 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1526
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1528
struct _dyneither_ptr _tmp4FB=Cyc_yyget_String_tok(yyyyvsp[0]);
int _tmp4FD;struct _tuple5 _tmp4FC=Cyc_yyget_Int_tok(yyyyvsp[2]);_tmp4FD=_tmp4FC.f2;{
void*a;
{const char*_tmpA8B;const char*_tmpA8A;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA8B="regparm",_tag_dyneither(_tmpA8B,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA8A="__regparm__",_tag_dyneither(_tmpA8A,sizeof(char),12))))== 0){
if(_tmp4FD < 0  || _tmp4FD > 3){
const char*_tmpA8C;Cyc_Parse_err(((_tmpA8C="regparm requires value between 0 and 3",_tag_dyneither(_tmpA8C,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpA8F;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpA8E;a=(void*)((_tmpA8E=_cycalloc_atomic(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=0,((_tmpA8F.f1=_tmp4FD,_tmpA8F)))),_tmpA8E))));};}else{
const char*_tmpA91;const char*_tmpA90;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA91="aligned",_tag_dyneither(_tmpA91,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA90="__aligned__",_tag_dyneither(_tmpA90,sizeof(char),12))))== 0){
if(_tmp4FD < 0){const char*_tmpA92;Cyc_Parse_err(((_tmpA92="aligned requires positive power of two",_tag_dyneither(_tmpA92,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp4FD;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpA93;Cyc_Parse_err(((_tmpA93="aligned requires positive power of two",_tag_dyneither(_tmpA93,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpA96;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA95;a=(void*)((_tmpA95=_cycalloc_atomic(sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA96.tag=6,((_tmpA96.f1=_tmp4FD,_tmpA96)))),_tmpA95))));};};}else{
const char*_tmpA98;const char*_tmpA97;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA98="initializes",_tag_dyneither(_tmpA98,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA97="__initializes__",_tag_dyneither(_tmpA97,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpA9B;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA9A;a=(void*)((_tmpA9A=_cycalloc_atomic(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=20,((_tmpA9B.f1=_tmp4FD,_tmpA9B)))),_tmpA9A))));}else{
const char*_tmpA9D;const char*_tmpA9C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA9D="noliveunique",_tag_dyneither(_tmpA9D,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpA9C="__noliveunique__",_tag_dyneither(_tmpA9C,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAA0;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA9F;a=(void*)((_tmpA9F=_cycalloc_atomic(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=21,((_tmpAA0.f1=_tmp4FD,_tmpAA0)))),_tmpA9F))));}else{
const char*_tmpAA2;const char*_tmpAA1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpAA2="noconsume",_tag_dyneither(_tmpAA2,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FB,((_tmpAA1="__noconsume__",_tag_dyneither(_tmpAA1,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAA5;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAA4;a=(void*)((_tmpAA4=_cycalloc_atomic(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA5.tag=22,((_tmpAA5.f1=_tmp4FD,_tmpAA5)))),_tmpAA4))));}else{
# 1550
{const char*_tmpAA6;Cyc_Parse_err(((_tmpAA6="unrecognized attribute",_tag_dyneither(_tmpAA6,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1553
yyval=Cyc_YY49(a);
# 1555
break;};}case 66: _LL273: {
# 1552 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1556
struct _dyneither_ptr _tmp516=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAA8;const char*_tmpAA7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp516,((_tmpAA8="section",_tag_dyneither(_tmpAA8,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp516,((_tmpAA7="__section__",_tag_dyneither(_tmpAA7,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAAB;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAAA;a=(void*)((_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA[0]=((_tmpAAB.tag=8,((_tmpAAB.f1=_tmp517,_tmpAAB)))),_tmpAAA))));}else{
# 1562
{const char*_tmpAAC;Cyc_Parse_err(((_tmpAAC="unrecognized attribute",_tag_dyneither(_tmpAAC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1565
yyval=Cyc_YY49(a);
# 1567
break;}case 67: _LL274: {
# 1564 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1568
struct _dyneither_ptr _tmp51D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1572
{const char*_tmpAAD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51D,((_tmpAAD="__mode__",_tag_dyneither(_tmpAAD,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAB0;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAAF;a=(void*)((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=24,((_tmpAB0.f1=_tmp51E,_tmpAB0)))),_tmpAAF))));}else{
# 1575
{const char*_tmpAB1;Cyc_Parse_err(((_tmpAB1="unrecognized attribute",_tag_dyneither(_tmpAB1,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1578
yyval=Cyc_YY49(a);
# 1580
break;}case 68: _LL275: {
# 1577 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1581
struct _dyneither_ptr _tmp523=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp524=Cyc_yyget_String_tok(yyyyvsp[2]);
int _tmp526;struct _tuple5 _tmp525=Cyc_yyget_Int_tok(yyyyvsp[4]);_tmp526=_tmp525.f2;{
int _tmp528;struct _tuple5 _tmp527=Cyc_yyget_Int_tok(yyyyvsp[6]);_tmp528=_tmp527.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAB3;const char*_tmpAB2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp523,((_tmpAB3="format",_tag_dyneither(_tmpAB3,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp523,((_tmpAB2="__format__",_tag_dyneither(_tmpAB2,sizeof(char),11))))== 0){
const char*_tmpAB4;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp524,((_tmpAB4="printf",_tag_dyneither(_tmpAB4,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAB7;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAB6;a=(void*)((_tmpAB6=_cycalloc_atomic(sizeof(*_tmpAB6)),((_tmpAB6[0]=((_tmpAB7.tag=19,((_tmpAB7.f1=Cyc_Absyn_Printf_ft,((_tmpAB7.f2=_tmp526,((_tmpAB7.f3=_tmp528,_tmpAB7)))))))),_tmpAB6))));}else{
const char*_tmpAB8;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp524,((_tmpAB8="scanf",_tag_dyneither(_tmpAB8,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpABB;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpABA;a=(void*)((_tmpABA=_cycalloc_atomic(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABB.tag=19,((_tmpABB.f1=Cyc_Absyn_Scanf_ft,((_tmpABB.f2=_tmp526,((_tmpABB.f3=_tmp528,_tmpABB)))))))),_tmpABA))));}else{
# 1592
const char*_tmpABC;Cyc_Parse_err(((_tmpABC="unrecognized format type",_tag_dyneither(_tmpABC,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1594
const char*_tmpABD;Cyc_Parse_err(((_tmpABD="unrecognized attribute",_tag_dyneither(_tmpABD,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1597
break;};};}case 69: _LL276: {
# 1602 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606
yyval=yyyyvsp[0];
break;}case 70: _LL277: {
# 1604 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1608
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAC0;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpABF;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpAC0.tag=17,((_tmpAC0.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAC0.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpAC0.f3=0,((_tmpAC0.f4=0,_tmpAC0)))))))))),_tmpABF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 71: _LL278: {
# 1608 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 72: _LL279: {
# 1609 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL27A: {
# 1611 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1615
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL27B: {
# 1612 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL27C: {
# 1613 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1617
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL27D: {
# 1614 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL27E: {
# 1615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1619
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL27F: {
# 1616 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1620
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL280: {
# 1617 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1621
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL281: {
# 1618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1622
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL282: {
# 1619 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1623
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL283: {
# 1620 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1624
yyval=yyyyvsp[0];
break;}case 83: _LL284: {
# 1621 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1625
yyval=yyyyvsp[0];
break;}case 84: _LL285: {
# 1623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1627
yyval=yyyyvsp[0];
break;}case 85: _LL286: {
# 1625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1629
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 86: _LL287: {
# 1628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1632
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAC3;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAC2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2[0]=((_tmpAC3.tag=10,((_tmpAC3.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpAC3)))),_tmpAC2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1635
break;}case 87: _LL288: {
# 1632 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1636
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC5;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5[0]=((_tmpAC6.tag=15,((_tmpAC6.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAC6)))),_tmpAC5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 88: _LL289: {
# 1634 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1638
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAC9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAC8;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC9.tag=15,((_tmpAC9.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAC9)))),_tmpAC8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1640
break;}case 89: _LL28A: {
# 1637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1641
void*_tmp53B=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpACC;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpACB;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=((_tmpACC.tag=16,((_tmpACC.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpACC.f2=_tmp53B,_tmpACC)))))),_tmpACB)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1644
break;}case 90: _LL28B: {
# 1641 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1645
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpACF;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpACE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=16,((_tmpACF.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpACF.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpACF)))))),_tmpACE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 91: _LL28C: {
# 1643 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1647
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAD2;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD2.tag=19,((_tmpAD2.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAD2)))),_tmpAD1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL28D: {
# 1645 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAD5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAD4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4[0]=((_tmpAD5.tag=19,((_tmpAD5.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAD5)))),_tmpAD4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL28E: {
# 1647 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1651
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAD8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=((_tmpAD8.tag=18,((_tmpAD8.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAD8)))),_tmpAD7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL28F: {
# 1652 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 95: _LL290: {
# 1656 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1660
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAD9;yyval=Cyc_YY26(((_tmpAD9.print_const=1,((_tmpAD9.q_volatile=0,((_tmpAD9.q_restrict=0,((_tmpAD9.real_const=1,((_tmpAD9.loc=loc,_tmpAD9)))))))))));}
break;}case 96: _LL291: {
# 1658 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1662
{struct Cyc_Absyn_Tqual _tmpADA;yyval=Cyc_YY26(((_tmpADA.print_const=0,((_tmpADA.q_volatile=1,((_tmpADA.q_restrict=0,((_tmpADA.real_const=0,((_tmpADA.loc=0,_tmpADA)))))))))));}
break;}case 97: _LL292: {
# 1659 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1663
{struct Cyc_Absyn_Tqual _tmpADB;yyval=Cyc_YY26(((_tmpADB.print_const=0,((_tmpADB.q_volatile=0,((_tmpADB.q_restrict=1,((_tmpADB.real_const=0,((_tmpADB.loc=0,_tmpADB)))))))))));}
break;}case 98: _LL293: {
# 1665 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1669
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpAEA;struct Cyc_Absyn_Enumdecl*_tmpAE9;struct Cyc_Core_Opt*_tmpAE8;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpAE7;struct Cyc_Absyn_TypeDecl*_tmpAE6;struct Cyc_Absyn_TypeDecl*_tmp549=(_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->r=(void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAE7.tag=1,((_tmpAE7.f1=((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->sc=Cyc_Absyn_Public,((_tmpAE9->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAE9->fields=((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpAE8)))),_tmpAE9)))))))),_tmpAE7)))),_tmpAEA)))),((_tmpAE6->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAE6)))));
# 1671
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAED;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAEC;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAED.tag=26,((_tmpAED.f1=_tmp549,((_tmpAED.f2=0,_tmpAED)))))),_tmpAEC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1673
break;}case 99: _LL294: {
# 1670 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1674
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpAF0;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpAEF;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=((_tmpAF0.tag=13,((_tmpAF0.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpAF0.f2=0,_tmpAF0)))))),_tmpAEF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 100: _LL295: {
# 1672 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1676
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpAF3;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpAF2;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=14,((_tmpAF3.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAF3)))),_tmpAF2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL296: {
# 1678 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1682
{struct Cyc_Absyn_Enumfield*_tmpAF4;yyval=Cyc_YY50(((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF4->tag=0,((_tmpAF4->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAF4)))))))));}
break;}case 102: _LL297: {
# 1680 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1684
{struct Cyc_Absyn_Enumfield*_tmpAF5;yyval=Cyc_YY50(((_tmpAF5=_cycalloc(sizeof(*_tmpAF5)),((_tmpAF5->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF5->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpAF5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpAF5)))))))));}
break;}case 103: _LL298: {
# 1684 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1688
{struct Cyc_List_List*_tmpAF6;yyval=Cyc_YY51(((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF6->tl=0,_tmpAF6)))))));}
break;}case 104: _LL299: {
# 1685 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1689
{struct Cyc_List_List*_tmpAF7;yyval=Cyc_YY51(((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF7->tl=0,_tmpAF7)))))));}
break;}case 105: _LL29A: {
# 1686 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1690
{struct Cyc_List_List*_tmpAF8;yyval=Cyc_YY51(((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpAF8->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpAF8)))))));}
break;}case 106: _LL29B: {
# 1692 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1696
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAFB;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAFA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=12,((_tmpAFB.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpAFB.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpAFB)))))),_tmpAFA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 107: _LL29C: {
# 1697 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1702
struct Cyc_List_List*_tmp55C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp55D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp55E=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp55C,
Cyc_Absyn_aggrdecl_impl(_tmp55D,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1707
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpAFE;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAFD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFE.tag=26,((_tmpAFE.f1=_tmp55E,((_tmpAFE.f2=0,_tmpAFE)))))),_tmpAFD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1709
break;}case 108: _LL29D: {
# 1708 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1713
struct Cyc_List_List*_tmp561=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp562=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp563=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp561,
Cyc_Absyn_aggrdecl_impl(_tmp562,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1718
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB01;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB00;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=26,((_tmpB01.f1=_tmp563,((_tmpB01.f2=0,_tmpB01)))))),_tmpB00)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1720
break;}case 109: _LL29E: {
# 1717 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1721
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB0B;struct Cyc_Core_Opt*_tmpB0A;struct Cyc_Absyn_AggrInfo _tmpB09;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB08;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB0B.tag=11,((_tmpB0B.f1=((_tmpB09.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB0A=_cycalloc_atomic(sizeof(*_tmpB0A)),((_tmpB0A->v=(void*)1,_tmpB0A))))),((_tmpB09.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB09)))),_tmpB0B)))),_tmpB08)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1724
break;}case 110: _LL29F: {
# 1721 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1725
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB11;struct Cyc_Absyn_AggrInfo _tmpB10;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB0F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB11.tag=11,((_tmpB11.f1=((_tmpB10.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB10.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB10)))),_tmpB11)))),_tmpB0F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1728
break;}case 111: _LL2A0:
# 1728 "parse.y"
 yyval=Cyc_YY43(0);
break;case 112: _LL2A1: {
# 1730 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1734
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 113: _LL2A2: {
# 1734 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1738
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 114: _LL2A3: {
# 1735 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1739
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 115: _LL2A4:
# 1740 "parse.y"
 yyval=Cyc_YY27(0);
break;case 116: _LL2A5: {
# 1742 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1746
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp56D=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp56D != 0;_tmp56D=_tmp56D->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp56D->hd,decls);}}{
# 1750
struct Cyc_List_List*_tmp56E=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp56E != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp56E,decls);
yyval=Cyc_YY27(decls);
# 1755
break;};}case 117: _LL2A6: {
# 1756 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1760
{struct Cyc_List_List*_tmpB12;yyval=Cyc_YY28(((_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB12->tl=0,_tmpB12)))))));}
break;}case 118: _LL2A7: {
# 1758 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1762
{struct Cyc_List_List*_tmpB13;yyval=Cyc_YY28(((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB13->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB13)))))));}
break;}case 119: _LL2A8: {
# 1762 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1764
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1766
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 120: _LL2A9: {
# 1768 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1772
{struct _tuple12*_tmpB14;yyval=Cyc_YY22(((_tmpB14=_region_malloc(yyr,sizeof(*_tmpB14)),((_tmpB14->tl=0,((_tmpB14->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB14)))))));}
break;}case 121: _LL2AA: {
# 1770 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1774
{struct _tuple12*_tmpB15;yyval=Cyc_YY22(((_tmpB15=_region_malloc(yyr,sizeof(*_tmpB15)),((_tmpB15->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB15->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB15)))))));}
break;}case 122: _LL2AB: {
# 1775 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1779
{struct _tuple11 _tmpB16;yyval=Cyc_YY21(((_tmpB16.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB16.f2=0,_tmpB16)))));}
break;}case 123: _LL2AC: {
# 1777 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1781
{struct _tuple11 _tmpB17;yyval=Cyc_YY21(((_tmpB17.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB17.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB17)))));}
break;}case 124: _LL2AD: {
# 1782 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1787
struct _RegionHandle _tmp575=_new_region("temp");struct _RegionHandle*temp=& _tmp575;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp577;struct Cyc_Parse_Type_specifier _tmp578;struct Cyc_List_List*_tmp579;struct _tuple25 _tmp576=Cyc_yyget_YY38(yyyyvsp[0]);_tmp577=_tmp576.f1;_tmp578=_tmp576.f2;_tmp579=_tmp576.f3;
if(_tmp577.loc == 0)_tmp577.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp57A=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp57A != 0;_tmp57A=_tmp57A->tl){
struct Cyc_Parse_Declarator _tmp57C;struct Cyc_Absyn_Exp*_tmp57D;struct Cyc_Absyn_Exp*_tmp57E;struct _tuple24*_tmp57B=(struct _tuple24*)_tmp57A->hd;_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;
{struct _tuple10*_tmpB18;decls=((_tmpB18=_region_malloc(temp,sizeof(*_tmpB18)),((_tmpB18->tl=decls,((_tmpB18->hd=_tmp57C,_tmpB18))))));}{
struct _tuple16*_tmpB1B;struct Cyc_List_List*_tmpB1A;widths_and_reqs=(
(_tmpB1A=_region_malloc(temp,sizeof(*_tmpB1A)),((_tmpB1A->hd=((_tmpB1B=_region_malloc(temp,sizeof(*_tmpB1B)),((_tmpB1B->f1=_tmp57D,((_tmpB1B->f2=_tmp57E,_tmpB1B)))))),((_tmpB1A->tl=widths_and_reqs,_tmpB1A))))));};}}
# 1798
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp582=Cyc_Parse_speclist2typ(_tmp578,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp583=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp577,_tmp582,decls,_tmp579),widths_and_reqs);
# 1804
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp583));};};}
# 1807
_npop_handler(0);break;
# 1787
;_pop_region(temp);}case 125: _LL2AE: {
# 1811 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1815
{struct _tuple25 _tmpB1C;yyval=Cyc_YY38(((_tmpB1C.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB1C.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB1C.f3=0,_tmpB1C)))))));}
break;}case 126: _LL2AF: {
# 1813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1817
struct _tuple25 _tmp585=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB1D;yyval=Cyc_YY38(((_tmpB1D.f1=_tmp585.f1,((_tmpB1D.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp585.f2),((_tmpB1D.f3=_tmp585.f3,_tmpB1D)))))));}
break;}case 127: _LL2B0: {
# 1815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1819
{struct _tuple25 _tmpB1E;yyval=Cyc_YY38(((_tmpB1E.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB1E.f2=Cyc_Parse_empty_spec(0),((_tmpB1E.f3=0,_tmpB1E)))))));}
break;}case 128: _LL2B1: {
# 1817 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1821
struct _tuple25 _tmp588=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB1F;yyval=Cyc_YY38(((_tmpB1F.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp588.f1),((_tmpB1F.f2=_tmp588.f2,((_tmpB1F.f3=_tmp588.f3,_tmpB1F)))))));}
break;}case 129: _LL2B2: {
# 1820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1824
{struct _tuple25 _tmpB20;yyval=Cyc_YY38(((_tmpB20.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB20.f2=Cyc_Parse_empty_spec(0),((_tmpB20.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB20)))))));}
break;}case 130: _LL2B3: {
# 1822 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1826
struct _tuple25 _tmp58B=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB21;yyval=Cyc_YY38(((_tmpB21.f1=_tmp58B.f1,((_tmpB21.f2=_tmp58B.f2,((_tmpB21.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp58B.f3),_tmpB21)))))));}
break;}case 131: _LL2B4: {
# 1828 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1832
{struct _tuple25 _tmpB22;yyval=Cyc_YY38(((_tmpB22.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB22.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB22.f3=0,_tmpB22)))))));}
break;}case 132: _LL2B5: {
# 1830 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1834
struct _tuple25 _tmp58E=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB23;yyval=Cyc_YY38(((_tmpB23.f1=_tmp58E.f1,((_tmpB23.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp58E.f2),((_tmpB23.f3=_tmp58E.f3,_tmpB23)))))));}
break;}case 133: _LL2B6: {
# 1832 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1836
{struct _tuple25 _tmpB24;yyval=Cyc_YY38(((_tmpB24.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB24.f2=Cyc_Parse_empty_spec(0),((_tmpB24.f3=0,_tmpB24)))))));}
break;}case 134: _LL2B7: {
# 1834 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1838
struct _tuple25 _tmp591=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB25;yyval=Cyc_YY38(((_tmpB25.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp591.f1),((_tmpB25.f2=_tmp591.f2,((_tmpB25.f3=_tmp591.f3,_tmpB25)))))));}
break;}case 135: _LL2B8: {
# 1837 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1841
{struct _tuple25 _tmpB26;yyval=Cyc_YY38(((_tmpB26.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB26.f2=Cyc_Parse_empty_spec(0),((_tmpB26.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB26)))))));}
break;}case 136: _LL2B9: {
# 1839 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1843
struct _tuple25 _tmp594=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB27;yyval=Cyc_YY38(((_tmpB27.f1=_tmp594.f1,((_tmpB27.f2=_tmp594.f2,((_tmpB27.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp594.f3),_tmpB27)))))));}
break;}case 137: _LL2BA: {
# 1843 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1847
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 138: _LL2BB: {
# 1849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1853
{struct Cyc_List_List*_tmpB28;yyval=Cyc_YY32(((_tmpB28=_region_malloc(yyr,sizeof(*_tmpB28)),((_tmpB28->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB28->tl=0,_tmpB28)))))));}
break;}case 139: _LL2BC: {
# 1851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1855
{struct Cyc_List_List*_tmpB29;yyval=Cyc_YY32(((_tmpB29=_region_malloc(yyr,sizeof(*_tmpB29)),((_tmpB29->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB29->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB29)))))));}
break;}case 140: _LL2BD: {
# 1856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1860
{struct _tuple24*_tmpB2A;yyval=Cyc_YY31(((_tmpB2A=_region_malloc(yyr,sizeof(*_tmpB2A)),((_tmpB2A->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB2A->f2=0,((_tmpB2A->f3=0,_tmpB2A)))))))));}
break;}case 141: _LL2BE: {
# 1858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1864
{struct Cyc_Parse_Declarator _tmpB30;struct _tuple0*_tmpB2F;struct _tuple24*_tmpB2E;yyval=Cyc_YY31(((_tmpB2E=_region_malloc(yyr,sizeof(*_tmpB2E)),((_tmpB2E->f1=((_tmpB30.id=((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->f1=Cyc_Absyn_Rel_n(0),((_tmpB2F->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB2F)))))),((_tmpB30.tms=0,_tmpB30)))),((_tmpB2E->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB2E->f3=0,_tmpB2E)))))))));}
# 1866
break;}case 142: _LL2BF:
# 1865 "parse.y"
{struct Cyc_Parse_Declarator _tmpB36;struct _tuple0*_tmpB35;struct _tuple24*_tmpB34;yyval=Cyc_YY31(((_tmpB34=_region_malloc(yyr,sizeof(*_tmpB34)),((_tmpB34->f1=((_tmpB36.id=((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35->f1=Cyc_Absyn_Rel_n(0),((_tmpB35->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB35)))))),((_tmpB36.tms=0,_tmpB36)))),((_tmpB34->f2=0,((_tmpB34->f3=0,_tmpB34)))))))));}
# 1867
break;case 143: _LL2C0: {
# 1868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1872
{struct _tuple24*_tmpB37;yyval=Cyc_YY31(((_tmpB37=_region_malloc(yyr,sizeof(*_tmpB37)),((_tmpB37->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB37->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB37->f3=0,_tmpB37)))))))));}
break;}case 144: _LL2C1: {
# 1870 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1874
{struct _tuple24*_tmpB38;yyval=Cyc_YY31(((_tmpB38=_region_malloc(yyr,sizeof(*_tmpB38)),((_tmpB38->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB38->f2=0,((_tmpB38->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB38)))))))));}
break;}case 145: _LL2C2: {
# 1876 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1880
int _tmp5A3=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp5A4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB39;struct Cyc_Absyn_TypeDecl*_tmp5A5=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5A4,((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB39)))),_tmp5A3,(unsigned int)(yyyylsp[0]).first_line);
# 1884
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB3B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=26,((_tmpB3C.f1=_tmp5A5,((_tmpB3C.f2=0,_tmpB3C)))))),_tmpB3B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1886
break;}case 146: _LL2C3: {
# 1883 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1887
int _tmp5A9=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB46;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB45;struct Cyc_Absyn_DatatypeInfo _tmpB44;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB43;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB46.tag=3,((_tmpB46.f1=((_tmpB44.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB45.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB45.is_extensible=_tmp5A9,_tmpB45))))),((_tmpB44.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB44)))),_tmpB46)))),_tmpB43)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1890
break;}case 147: _LL2C4: {
# 1887 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1891
int _tmp5AE=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB50;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB4F;struct Cyc_Absyn_DatatypeFieldInfo _tmpB4E;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB4D;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB50.tag=4,((_tmpB50.f1=((_tmpB4E.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB4F.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB4F.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB4F.is_extensible=_tmp5AE,_tmpB4F))))))),((_tmpB4E.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB4E)))),_tmpB50)))),_tmpB4D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1896
break;}case 148: _LL2C5: {
# 1895 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1899
yyval=Cyc_YY34(0);
break;}case 149: _LL2C6: {
# 1896 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1900
yyval=Cyc_YY34(1);
break;}case 150: _LL2C7: {
# 1900 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1904
{struct Cyc_List_List*_tmpB51;yyval=Cyc_YY37(((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB51->tl=0,_tmpB51)))))));}
break;}case 151: _LL2C8: {
# 1901 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1905
{struct Cyc_List_List*_tmpB52;yyval=Cyc_YY37(((_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB52->tl=0,_tmpB52)))))));}
break;}case 152: _LL2C9: {
# 1902 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1906
{struct Cyc_List_List*_tmpB53;yyval=Cyc_YY37(((_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB53->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB53)))))));}
break;}case 153: _LL2CA: {
# 1903 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1907
{struct Cyc_List_List*_tmpB54;yyval=Cyc_YY37(((_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB54->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB54)))))));}
break;}case 154: _LL2CB:
# 1907 "parse.y"
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 155: _LL2CC: {
# 1908 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1912
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 156: _LL2CD: {
# 1909 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1913
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 157: _LL2CE: {
# 1913 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1917
{struct Cyc_Absyn_Datatypefield*_tmpB55;yyval=Cyc_YY36(((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB55->typs=0,((_tmpB55->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB55->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB55)))))))))));}
break;}case 158: _LL2CF: {
# 1915 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1919
struct Cyc_List_List*_tmp5B8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB56;yyval=Cyc_YY36(((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((_tmpB56->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB56->typs=_tmp5B8,((_tmpB56->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB56->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB56)))))))))));}
break;}case 159: _LL2D0: {
# 1921 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1925
yyval=yyyyvsp[0];
break;}case 160: _LL2D1: {
# 1923 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1927
struct Cyc_Parse_Declarator _tmp5BA=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB57;yyval=Cyc_YY30(((_tmpB57.id=_tmp5BA.id,((_tmpB57.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5BA.tms),_tmpB57)))));}
break;}case 161: _LL2D2: {
# 1930 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1934
yyval=yyyyvsp[0];
break;}case 162: _LL2D3: {
# 1932 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1936
struct Cyc_Parse_Declarator _tmp5BC=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB58;yyval=Cyc_YY30(((_tmpB58.id=_tmp5BC.id,((_tmpB58.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5BC.tms),_tmpB58)))));}
break;}case 163: _LL2D4: {
# 1938 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1942
{struct Cyc_Parse_Declarator _tmpB59;yyval=Cyc_YY30(((_tmpB59.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB59.tms=0,_tmpB59)))));}
break;}case 164: _LL2D5: {
# 1940 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1944
yyval=yyyyvsp[1];
break;}case 165: _LL2D6: {
# 1944 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1948
struct Cyc_Parse_Declarator _tmp5BF=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB5F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB5E;struct Cyc_List_List*_tmpB5D;_tmp5BF.tms=((_tmpB5D=_region_malloc(yyr,sizeof(*_tmpB5D)),((_tmpB5D->hd=(void*)((_tmpB5F=_region_malloc(yyr,sizeof(*_tmpB5F)),((_tmpB5F[0]=((_tmpB5E.tag=5,((_tmpB5E.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB5E.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB5E)))))),_tmpB5F)))),((_tmpB5D->tl=_tmp5BF.tms,_tmpB5D))))));}
yyval=yyyyvsp[2];
# 1952
break;}case 166: _LL2D7: {
# 1949 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1953
{struct Cyc_List_List*_tmpB69;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB68;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB67;struct Cyc_Parse_Declarator _tmpB66;yyval=Cyc_YY30(((_tmpB66.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB66.tms=((_tmpB69=_region_malloc(yyr,sizeof(*_tmpB69)),((_tmpB69->hd=(void*)((_tmpB67=_region_malloc(yyr,sizeof(*_tmpB67)),((_tmpB67[0]=((_tmpB68.tag=0,((_tmpB68.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB68.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB68)))))),_tmpB67)))),((_tmpB69->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB69)))))),_tmpB66)))));}
break;}case 167: _LL2D8: {
# 1951 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1955
{struct Cyc_List_List*_tmpB73;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB72;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB71;struct Cyc_Parse_Declarator _tmpB70;yyval=Cyc_YY30(((_tmpB70.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB70.tms=(
(_tmpB73=_region_malloc(yyr,sizeof(*_tmpB73)),((_tmpB73->hd=(void*)((_tmpB71=_region_malloc(yyr,sizeof(*_tmpB71)),((_tmpB71[0]=((_tmpB72.tag=1,((_tmpB72.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB72.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB72.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB72)))))))),_tmpB71)))),((_tmpB73->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB73)))))),_tmpB70)))));}
break;}case 168: _LL2D9: {
# 1954 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1958
struct Cyc_List_List*_tmp5CC;int _tmp5CD;struct Cyc_Absyn_VarargInfo*_tmp5CE;void*_tmp5CF;struct Cyc_List_List*_tmp5D0;struct _tuple26*_tmp5CB=Cyc_yyget_YY42(yyyyvsp[2]);_tmp5CC=_tmp5CB->f1;_tmp5CD=_tmp5CB->f2;_tmp5CE=_tmp5CB->f3;_tmp5CF=_tmp5CB->f4;_tmp5D0=_tmp5CB->f5;
{struct Cyc_List_List*_tmpB88;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB87;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB86;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB85;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB84;struct Cyc_Parse_Declarator _tmpB83;yyval=Cyc_YY30(((_tmpB83.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB83.tms=((_tmpB88=_region_malloc(yyr,sizeof(*_tmpB88)),((_tmpB88->hd=(void*)((_tmpB84=_region_malloc(yyr,sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB87.tag=3,((_tmpB87.f1=(void*)((_tmpB85=_region_malloc(yyr,sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB86.tag=1,((_tmpB86.f1=_tmp5CC,((_tmpB86.f2=_tmp5CD,((_tmpB86.f3=_tmp5CE,((_tmpB86.f4=_tmp5CF,((_tmpB86.f5=_tmp5D0,_tmpB86)))))))))))),_tmpB85)))),_tmpB87)))),_tmpB84)))),((_tmpB88->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB88)))))),_tmpB83)))));}
# 1961
break;}case 169: _LL2DA: {
# 1958 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1962
{struct Cyc_List_List*_tmpB9D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpB9C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpB9B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB9A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB99;struct Cyc_Parse_Declarator _tmpB98;yyval=Cyc_YY30(((_tmpB98.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB98.tms=(
(_tmpB9D=_region_malloc(yyr,sizeof(*_tmpB9D)),((_tmpB9D->hd=(void*)((_tmpB99=_region_malloc(yyr,sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB9C.tag=3,((_tmpB9C.f1=(void*)((_tmpB9A=_region_malloc(yyr,sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB9B.tag=1,((_tmpB9B.f1=0,((_tmpB9B.f2=0,((_tmpB9B.f3=0,((_tmpB9B.f4=
# 1965
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpB9B.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpB9B)))))))))))),_tmpB9A)))),_tmpB9C)))),_tmpB99)))),((_tmpB9D->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB9D)))))),_tmpB98)))));}
# 1968
break;}case 170: _LL2DB: {
# 1965 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1969
{struct Cyc_List_List*_tmpBB2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBB1;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBB0;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBAF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBAE;struct Cyc_Parse_Declarator _tmpBAD;yyval=Cyc_YY30(((_tmpBAD.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBAD.tms=((_tmpBB2=_region_malloc(yyr,sizeof(*_tmpBB2)),((_tmpBB2->hd=(void*)((_tmpBAE=_region_malloc(yyr,sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBB1.tag=3,((_tmpBB1.f1=(void*)((_tmpBAF=_region_malloc(yyr,sizeof(*_tmpBAF)),((_tmpBAF[0]=((_tmpBB0.tag=0,((_tmpBB0.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBB0.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBB0)))))),_tmpBAF)))),_tmpBB1)))),_tmpBAE)))),((_tmpBB2->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBB2)))))),_tmpBAD)))));}
break;}case 171: _LL2DC: {
# 1968 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1972
struct Cyc_List_List*_tmp5E3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBBC;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBBB;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBBA;struct Cyc_Parse_Declarator _tmpBB9;yyval=Cyc_YY30(((_tmpBB9.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB9.tms=((_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)((_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=4,((_tmpBBB.f1=_tmp5E3,((_tmpBBB.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBBB.f3=0,_tmpBBB)))))))),_tmpBBA)))),((_tmpBBC->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBBC)))))),_tmpBB9)))));}
# 1975
break;}case 172: _LL2DD: {
# 1972 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1976
{struct Cyc_List_List*_tmpBC6;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBC5;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBC4;struct Cyc_Parse_Declarator _tmpBC3;yyval=Cyc_YY30(((_tmpBC3.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBC3.tms=((_tmpBC6=_region_malloc(yyr,sizeof(*_tmpBC6)),((_tmpBC6->hd=(void*)((_tmpBC4=_region_malloc(yyr,sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC5.tag=5,((_tmpBC5.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBC5.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBC5)))))),_tmpBC4)))),((_tmpBC6->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBC6)))))),_tmpBC3)))));}
# 1979
break;}case 173: _LL2DE: {
# 1986 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1990
{struct Cyc_Parse_Declarator _tmpBC7;yyval=Cyc_YY30(((_tmpBC7.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC7.tms=0,_tmpBC7)))));}
break;}case 174: _LL2DF: {
# 1988 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1992
{struct Cyc_Parse_Declarator _tmpBC8;yyval=Cyc_YY30(((_tmpBC8.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBC8.tms=0,_tmpBC8)))));}
break;}case 175: _LL2E0: {
# 1990 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1994
yyval=yyyyvsp[1];
break;}case 176: _LL2E1: {
# 1994 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1998
struct Cyc_Parse_Declarator _tmp5EE=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBCE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBCD;struct Cyc_List_List*_tmpBCC;_tmp5EE.tms=((_tmpBCC=_region_malloc(yyr,sizeof(*_tmpBCC)),((_tmpBCC->hd=(void*)((_tmpBCE=_region_malloc(yyr,sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBCD.tag=5,((_tmpBCD.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBCD.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBCD)))))),_tmpBCE)))),((_tmpBCC->tl=_tmp5EE.tms,_tmpBCC))))));}
yyval=yyyyvsp[2];
# 2002
break;}case 177: _LL2E2: {
# 1999 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2003
struct Cyc_Parse_Declarator _tmp5F2=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBD8;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBD7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBD6;struct Cyc_Parse_Declarator _tmpBD5;yyval=Cyc_YY30(((_tmpBD5.id=_tmp5F2.id,((_tmpBD5.tms=(
(_tmpBD8=_region_malloc(yyr,sizeof(*_tmpBD8)),((_tmpBD8->hd=(void*)((_tmpBD6=_region_malloc(yyr,sizeof(*_tmpBD6)),((_tmpBD6[0]=((_tmpBD7.tag=0,((_tmpBD7.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBD7.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBD7)))))),_tmpBD6)))),((_tmpBD8->tl=_tmp5F2.tms,_tmpBD8)))))),_tmpBD5)))));}
break;}case 178: _LL2E3: {
# 2003 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2007
struct Cyc_Parse_Declarator _tmp5F7=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBE2;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBE1;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBE0;struct Cyc_Parse_Declarator _tmpBDF;yyval=Cyc_YY30(((_tmpBDF.id=_tmp5F7.id,((_tmpBDF.tms=(
(_tmpBE2=_region_malloc(yyr,sizeof(*_tmpBE2)),((_tmpBE2->hd=(void*)((_tmpBE0=_region_malloc(yyr,sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE1.tag=1,((_tmpBE1.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBE1.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBE1.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBE1)))))))),_tmpBE0)))),((_tmpBE2->tl=_tmp5F7.tms,_tmpBE2)))))),_tmpBDF)))));}
# 2011
break;}case 179: _LL2E4: {
# 2008 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2012
struct Cyc_List_List*_tmp5FD;int _tmp5FE;struct Cyc_Absyn_VarargInfo*_tmp5FF;void*_tmp600;struct Cyc_List_List*_tmp601;struct _tuple26*_tmp5FC=Cyc_yyget_YY42(yyyyvsp[2]);_tmp5FD=_tmp5FC->f1;_tmp5FE=_tmp5FC->f2;_tmp5FF=_tmp5FC->f3;_tmp600=_tmp5FC->f4;_tmp601=_tmp5FC->f5;{
struct Cyc_Parse_Declarator _tmp602=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBF6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBF5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBF4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF3;struct Cyc_Parse_Declarator _tmpBF2;yyval=Cyc_YY30(((_tmpBF2.id=_tmp602.id,((_tmpBF2.tms=((_tmpBF7=_region_malloc(yyr,sizeof(*_tmpBF7)),((_tmpBF7->hd=(void*)((_tmpBF3=_region_malloc(yyr,sizeof(*_tmpBF3)),((_tmpBF3[0]=((_tmpBF6.tag=3,((_tmpBF6.f1=(void*)((_tmpBF4=_region_malloc(yyr,sizeof(*_tmpBF4)),((_tmpBF4[0]=((_tmpBF5.tag=1,((_tmpBF5.f1=_tmp5FD,((_tmpBF5.f2=_tmp5FE,((_tmpBF5.f3=_tmp5FF,((_tmpBF5.f4=_tmp600,((_tmpBF5.f5=_tmp601,_tmpBF5)))))))))))),_tmpBF4)))),_tmpBF6)))),_tmpBF3)))),((_tmpBF7->tl=_tmp602.tms,_tmpBF7)))))),_tmpBF2)))));}
# 2016
break;};}case 180: _LL2E5: {
# 2013 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2017
struct Cyc_Parse_Declarator _tmp609=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC0B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC0A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC09;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC08;struct Cyc_Parse_Declarator _tmpC07;yyval=Cyc_YY30(((_tmpC07.id=_tmp609.id,((_tmpC07.tms=(
(_tmpC0C=_region_malloc(yyr,sizeof(*_tmpC0C)),((_tmpC0C->hd=(void*)((_tmpC08=_region_malloc(yyr,sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC0B.tag=3,((_tmpC0B.f1=(void*)((_tmpC09=_region_malloc(yyr,sizeof(*_tmpC09)),((_tmpC09[0]=((_tmpC0A.tag=1,((_tmpC0A.f1=0,((_tmpC0A.f2=0,((_tmpC0A.f3=0,((_tmpC0A.f4=
# 2021
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC0A.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC0A)))))))))))),_tmpC09)))),_tmpC0B)))),_tmpC08)))),((_tmpC0C->tl=_tmp609.tms,_tmpC0C)))))),_tmpC07)))));}
# 2024
break;}case 181: _LL2E6: {
# 2021 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2025
struct Cyc_Parse_Declarator _tmp610=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC21;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC20;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC1F;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC1E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC1D;struct Cyc_Parse_Declarator _tmpC1C;yyval=Cyc_YY30(((_tmpC1C.id=_tmp610.id,((_tmpC1C.tms=((_tmpC21=_region_malloc(yyr,sizeof(*_tmpC21)),((_tmpC21->hd=(void*)((_tmpC1D=_region_malloc(yyr,sizeof(*_tmpC1D)),((_tmpC1D[0]=((_tmpC20.tag=3,((_tmpC20.f1=(void*)((_tmpC1E=_region_malloc(yyr,sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1F.tag=0,((_tmpC1F.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC1F.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC1F)))))),_tmpC1E)))),_tmpC20)))),_tmpC1D)))),((_tmpC21->tl=_tmp610.tms,_tmpC21)))))),_tmpC1C)))));}
break;}case 182: _LL2E7: {
# 2025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2029
struct Cyc_List_List*_tmp617=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp618=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC2A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC29;struct Cyc_Parse_Declarator _tmpC28;yyval=Cyc_YY30(((_tmpC28.id=_tmp618.id,((_tmpC28.tms=((_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=4,((_tmpC2A.f1=_tmp617,((_tmpC2A.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC2A.f3=0,_tmpC2A)))))))),_tmpC29)))),((_tmpC2B->tl=_tmp618.tms,_tmpC2B)))))),_tmpC28)))));}
# 2033
break;}case 183: _LL2E8: {
# 2030 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2034
struct Cyc_Parse_Declarator _tmp61D=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC35;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC34;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC33;struct Cyc_Parse_Declarator _tmpC32;yyval=Cyc_YY30(((_tmpC32.id=_tmp61D.id,((_tmpC32.tms=((_tmpC35=_region_malloc(yyr,sizeof(*_tmpC35)),((_tmpC35->hd=(void*)((_tmpC33=_region_malloc(yyr,sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC34.tag=5,((_tmpC34.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC34.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC34)))))),_tmpC33)))),((_tmpC35->tl=_tmp61D.tms,_tmpC35)))))),_tmpC32)))));}
# 2037
break;}case 184: _LL2E9: {
# 2043 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2047
yyval=yyyyvsp[0];
break;}case 185: _LL2EA: {
# 2044 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2048
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 186: _LL2EB: {
# 2048 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2052
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC3B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC3A;struct Cyc_List_List*_tmpC39;ans=((_tmpC39=_region_malloc(yyr,sizeof(*_tmpC39)),((_tmpC39->hd=(void*)((_tmpC3B=_region_malloc(yyr,sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3A.tag=5,((_tmpC3A.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC3A.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC3A)))))),_tmpC3B)))),((_tmpC39->tl=ans,_tmpC39))))));}{
# 2056
struct Cyc_Absyn_PtrLoc*ptrloc=0;
unsigned int _tmp626;union Cyc_Absyn_Constraint*_tmp627;union Cyc_Absyn_Constraint*_tmp628;struct _tuple21 _tmp625=*Cyc_yyget_YY1(yyyyvsp[0]);_tmp626=_tmp625.f1;_tmp627=_tmp625.f2;_tmp628=_tmp625.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC3C;ptrloc=((_tmpC3C=_cycalloc_atomic(sizeof(*_tmpC3C)),((_tmpC3C->ptr_loc=_tmp626,((_tmpC3C->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC3C->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC3C))))))));}{
# 2061
union Cyc_Absyn_Constraint*_tmp62B;union Cyc_Absyn_Constraint*_tmp62C;union Cyc_Absyn_Constraint*_tmp62D;void*_tmp62E;struct _tuple14 _tmp62A=Cyc_Parse_collapse_pointer_quals(_tmp626,_tmp627,_tmp628,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));_tmp62B=_tmp62A.f1;_tmp62C=_tmp62A.f2;_tmp62D=_tmp62A.f3;_tmp62E=_tmp62A.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC46;struct Cyc_Absyn_PtrAtts _tmpC45;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC44;struct Cyc_List_List*_tmpC43;ans=((_tmpC43=_region_malloc(yyr,sizeof(*_tmpC43)),((_tmpC43->hd=(void*)((_tmpC46=_region_malloc(yyr,sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC44.tag=2,((_tmpC44.f1=((_tmpC45.rgn=_tmp62E,((_tmpC45.nullable=_tmp62B,((_tmpC45.bounds=_tmp62C,((_tmpC45.zero_term=_tmp62D,((_tmpC45.ptrloc=ptrloc,_tmpC45)))))))))),((_tmpC44.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC44)))))),_tmpC46)))),((_tmpC43->tl=ans,_tmpC43))))));}
yyval=Cyc_YY29(ans);
# 2065
break;};};}case 187: _LL2EC:
# 2063 "parse.y"
 yyval=Cyc_YY57(0);
break;case 188: _LL2ED: {
# 2064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2068
{struct Cyc_List_List*_tmpC47;yyval=Cyc_YY57(((_tmpC47=_region_malloc(yyr,sizeof(*_tmpC47)),((_tmpC47->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC47->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC47)))))));}
break;}case 189: _LL2EE: {
# 2069 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2073
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC4A;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC49;yyval=Cyc_YY56((void*)((_tmpC49=_region_malloc(yyr,sizeof(*_tmpC49)),((_tmpC49[0]=((_tmpC4A.tag=0,((_tmpC4A.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC4A)))),_tmpC49)))));}
break;}case 190: _LL2EF: {
# 2071 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2075
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC4D;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC4C;yyval=Cyc_YY56((void*)((_tmpC4C=_region_malloc(yyr,sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=1,((_tmpC4D.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC4D)))),_tmpC4C)))));}
break;}case 191: _LL2F0: {
# 2073 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2077
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC50;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC4F;yyval=Cyc_YY56((void*)((_tmpC4F=_region_malloc(yyr,sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC50.tag=2,_tmpC50)),_tmpC4F)))));}
break;}case 192: _LL2F1: {
# 2075 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2079
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC53;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC52;yyval=Cyc_YY56((void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=3,_tmpC53)),_tmpC52)))));}
break;}case 193: _LL2F2: {
# 2077 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2081
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC56;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC55;yyval=Cyc_YY56((void*)((_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=4,_tmpC56)),_tmpC55)))));}
break;}case 194: _LL2F3: {
# 2079 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2083
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC59;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC58;yyval=Cyc_YY56((void*)((_tmpC58=_region_malloc(yyr,sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=5,_tmpC59)),_tmpC58)))));}
break;}case 195: _LL2F4: {
# 2081 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2085
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC5C;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC5B;yyval=Cyc_YY56((void*)((_tmpC5B=_region_malloc(yyr,sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5C.tag=6,_tmpC5C)),_tmpC5B)))));}
break;}case 196: _LL2F5: {
# 2083 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2087
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC5F;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC5E;yyval=Cyc_YY56((void*)((_tmpC5E=_region_malloc(yyr,sizeof(*_tmpC5E)),((_tmpC5E[0]=((_tmpC5F.tag=7,_tmpC5F)),_tmpC5E)))));}
break;}case 197: _LL2F6: {
# 2088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2093
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC60;yyval=Cyc_YY1(((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->f1=loc,((_tmpC60->f2=Cyc_Absyn_true_conref,((_tmpC60->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC60)))))))));}
# 2096
break;}case 198: _LL2F7: {
# 2093 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2097
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC61;yyval=Cyc_YY1(((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->f1=loc,((_tmpC61->f2=Cyc_Absyn_false_conref,((_tmpC61->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC61)))))))));}
# 2100
break;}case 199: _LL2F8: {
# 2097 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2101
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC62;yyval=Cyc_YY1(((_tmpC62=_cycalloc(sizeof(*_tmpC62)),((_tmpC62->f1=loc,((_tmpC62->f2=Cyc_Absyn_true_conref,((_tmpC62->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC62)))))))));}
# 2104
break;}case 200: _LL2F9:
# 2102 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 201: _LL2FA: {
# 2103 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2107
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC65;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC64;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64[0]=((_tmpC65.tag=1,((_tmpC65.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC65)))),_tmpC64))))));}
break;}case 202: _LL2FB:
# 2106 "parse.y"
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 203: _LL2FC: {
# 2107 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2111
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 204: _LL2FD: {
# 2108 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2112
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 205: _LL2FE:
# 2113 "parse.y"
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 206: _LL2FF: {
# 2114 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2118
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 207: _LL300: {
# 2115 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2119
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 208: _LL301:
# 2119 "parse.y"
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 209: _LL302: {
# 2120 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2124
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 210: _LL303: {
# 2125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2129
{struct _tuple26*_tmpC66;yyval=Cyc_YY42(((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC66->f2=0,((_tmpC66->f3=0,((_tmpC66->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC66->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC66)))))))))))));}
break;}case 211: _LL304: {
# 2127 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2131
{struct _tuple26*_tmpC67;yyval=Cyc_YY42(((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC67->f2=1,((_tmpC67->f3=0,((_tmpC67->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC67->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC67)))))))))))));}
break;}case 212: _LL305: {
# 2130 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2134
struct _dyneither_ptr*_tmp64C;struct Cyc_Absyn_Tqual _tmp64D;void*_tmp64E;struct _tuple8*_tmp64B=Cyc_yyget_YY40(yyyyvsp[2]);_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;_tmp64E=_tmp64B->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC68;struct Cyc_Absyn_VarargInfo*_tmp64F=(_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->name=_tmp64C,((_tmpC68->tq=_tmp64D,((_tmpC68->type=_tmp64E,((_tmpC68->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC68)))))))));
{struct _tuple26*_tmpC69;yyval=Cyc_YY42(((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->f1=0,((_tmpC69->f2=0,((_tmpC69->f3=_tmp64F,((_tmpC69->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC69->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC69)))))))))))));}
# 2138
break;};}case 213: _LL306: {
# 2136 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2140
struct _dyneither_ptr*_tmp653;struct Cyc_Absyn_Tqual _tmp654;void*_tmp655;struct _tuple8*_tmp652=Cyc_yyget_YY40(yyyyvsp[4]);_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=_tmp652->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC6A;struct Cyc_Absyn_VarargInfo*_tmp656=(_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->name=_tmp653,((_tmpC6A->tq=_tmp654,((_tmpC6A->type=_tmp655,((_tmpC6A->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC6A)))))))));
{struct _tuple26*_tmpC6B;yyval=Cyc_YY42(((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC6B->f2=0,((_tmpC6B->f3=_tmp656,((_tmpC6B->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC6B->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC6B)))))))))))));}
# 2144
break;};}case 214: _LL307: {
# 2144 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2148
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC6E;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC6D;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=1,((_tmpC6E.f1=0,_tmpC6E)))),_tmpC6D))))));}
break;}case 215: _LL308: {
# 2145 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2149
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 216: _LL309:
# 2148 "parse.y"
 yyval=Cyc_YY52(0);
break;case 217: _LL30A: {
# 2149 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2153
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC71;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC70;yyval=Cyc_YY52((void*)((_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC71.tag=24,((_tmpC71.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC71)))),_tmpC70)))));}
break;}case 218: _LL30B:
# 2153 "parse.y"
 yyval=Cyc_YY53(0);
break;case 219: _LL30C: {
# 2154 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2158
yyval=yyyyvsp[1];
break;}case 220: _LL30D: {
# 2159 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2166
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC74;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC73;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp65D=(_tmpC73=_cycalloc(sizeof(*_tmpC73)),((_tmpC73[0]=((_tmpC74.tag=2,((_tmpC74.f1=0,((_tmpC74.f2=& Cyc_Tcutil_trk,_tmpC74)))))),_tmpC73)));
struct _dyneither_ptr _tmp65E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp65F=Cyc_Parse_id2type(_tmp65E,(void*)_tmp65D);
{struct _tuple29*_tmpC7E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC7D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC7C;struct Cyc_List_List*_tmpC7B;yyval=Cyc_YY53(((_tmpC7B=_cycalloc(sizeof(*_tmpC7B)),((_tmpC7B->hd=((_tmpC7E=_cycalloc(sizeof(*_tmpC7E)),((_tmpC7E->f1=(void*)((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=24,((_tmpC7D.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC7D)))),_tmpC7C)))),((_tmpC7E->f2=_tmp65F,_tmpC7E)))))),((_tmpC7B->tl=0,_tmpC7B)))))));}
# 2171
break;}case 221: _LL30E: {
# 2168 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2173
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC81;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC80;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp666=(_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=2,((_tmpC81.f1=0,((_tmpC81.f2=& Cyc_Tcutil_trk,_tmpC81)))))),_tmpC80)));
struct _dyneither_ptr _tmp667=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp668=Cyc_Parse_id2type(_tmp667,(void*)_tmp666);
{struct _tuple29*_tmpC8B;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC8A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC89;struct Cyc_List_List*_tmpC88;yyval=Cyc_YY53(((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88->hd=((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->f1=(void*)((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89[0]=((_tmpC8A.tag=24,((_tmpC8A.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC8A)))),_tmpC89)))),((_tmpC8B->f2=_tmp668,_tmpC8B)))))),((_tmpC88->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC88)))))));}
# 2178
break;}case 222: _LL30F:
# 2178 "parse.y"
 yyval=Cyc_YY34(0);
break;case 223: _LL310: {
# 2180 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2184
{const char*_tmpC8C;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpC8C="inject",_tag_dyneither(_tmpC8C,sizeof(char),7))))!= 0){
const char*_tmpC8D;Cyc_Parse_err(((_tmpC8D="missing type in function declaration",_tag_dyneither(_tmpC8D,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2188
break;}case 224: _LL311: {
# 2187 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2191
yyval=yyyyvsp[0];
break;}case 225: _LL312: {
# 2188 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2192
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 226: _LL313: {
# 2192 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2196
yyval=Cyc_YY43(0);
break;}case 227: _LL314: {
# 2193 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2197
yyval=yyyyvsp[1];
break;}case 228: _LL315: {
# 2195 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2199
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC93;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC92;struct Cyc_List_List*_tmpC91;yyval=Cyc_YY43(((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91->hd=(void*)((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93[0]=((_tmpC92.tag=25,((_tmpC92.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC92)))),_tmpC93)))),((_tmpC91->tl=0,_tmpC91)))))));}
break;}case 229: _LL316: {
# 2197 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2201
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpC94;yyval=Cyc_YY43(((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpC94->tl=0,_tmpC94)))))));}
# 2204
break;}case 230: _LL317: {
# 2205 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2209
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpC9A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC99;struct Cyc_List_List*_tmpC98;yyval=Cyc_YY43(((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=(void*)((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A[0]=((_tmpC99.tag=23,((_tmpC99.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC99)))),_tmpC9A)))),((_tmpC98->tl=0,_tmpC98)))))));}
# 2212
break;}case 231: _LL318: {
# 2209 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2213
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCA0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC9F;struct Cyc_List_List*_tmpC9E;yyval=Cyc_YY43(((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->hd=(void*)((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0[0]=((_tmpC9F.tag=23,((_tmpC9F.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpC9F)))),_tmpCA0)))),((_tmpC9E->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpC9E)))))));}
# 2216
break;}case 232: _LL319: {
# 2217 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2221
{struct Cyc_List_List*_tmpCA1;yyval=Cyc_YY41(((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpCA1->tl=0,_tmpCA1)))))));}
break;}case 233: _LL31A: {
# 2219 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2223
{struct Cyc_List_List*_tmpCA2;yyval=Cyc_YY41(((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpCA2->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCA2)))))));}
break;}case 234: _LL31B: {
# 2225 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2229
struct Cyc_Absyn_Tqual _tmp67E;struct Cyc_Parse_Type_specifier _tmp67F;struct Cyc_List_List*_tmp680;struct _tuple25 _tmp67D=Cyc_yyget_YY38(yyyyvsp[0]);_tmp67E=_tmp67D.f1;_tmp67F=_tmp67D.f2;_tmp680=_tmp67D.f3;
if(_tmp67E.loc == 0)_tmp67E.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple0*_tmp682;struct Cyc_List_List*_tmp683;struct Cyc_Parse_Declarator _tmp681=Cyc_yyget_YY30(yyyyvsp[1]);_tmp682=_tmp681.id;_tmp683=_tmp681.tms;{
void*_tmp684=Cyc_Parse_speclist2typ(_tmp67F,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_Absyn_Tqual _tmp686;void*_tmp687;struct Cyc_List_List*_tmp688;struct Cyc_List_List*_tmp689;struct _tuple13 _tmp685=Cyc_Parse_apply_tms(_tmp67E,_tmp684,_tmp680,_tmp683);_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;_tmp688=_tmp685.f3;_tmp689=_tmp685.f4;
if(_tmp688 != 0){
const char*_tmpCA3;Cyc_Parse_err(((_tmpCA3="parameter with bad type params",_tag_dyneither(_tmpCA3,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp682)){
const char*_tmpCA4;Cyc_Parse_err(((_tmpCA4="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCA4,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp682).f2;
if(_tmp689 != 0){
const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCA7="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCA7,sizeof(char),40))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}
{struct _tuple8*_tmpCA8;yyval=Cyc_YY40(((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=idopt,((_tmpCA8->f2=_tmp686,((_tmpCA8->f3=_tmp687,_tmpCA8)))))))));}
# 2243
break;};};};}case 235: _LL31C: {
# 2240 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2242
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2244
struct Cyc_Absyn_Tqual _tmp690;struct Cyc_Parse_Type_specifier _tmp691;struct Cyc_List_List*_tmp692;struct _tuple25 _tmp68F=Cyc_yyget_YY38(yyyyvsp[0]);_tmp690=_tmp68F.f1;_tmp691=_tmp68F.f2;_tmp692=_tmp68F.f3;
if(_tmp690.loc == 0)_tmp690.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp693=Cyc_Parse_speclist2typ(_tmp691,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp692 != 0){
const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAB="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCAB,sizeof(char),38))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}
{struct _tuple8*_tmpCAC;yyval=Cyc_YY40(((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=0,((_tmpCAC->f2=_tmp690,((_tmpCAC->f3=_tmp693,_tmpCAC)))))))));}
# 2251
break;};}case 236: _LL31D: {
# 2248 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2252
struct Cyc_Absyn_Tqual _tmp698;struct Cyc_Parse_Type_specifier _tmp699;struct Cyc_List_List*_tmp69A;struct _tuple25 _tmp697=Cyc_yyget_YY38(yyyyvsp[0]);_tmp698=_tmp697.f1;_tmp699=_tmp697.f2;_tmp69A=_tmp697.f3;
if(_tmp698.loc == 0)_tmp698.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp69B=Cyc_Parse_speclist2typ(_tmp699,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp69C=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct Cyc_Absyn_Tqual _tmp69E;void*_tmp69F;struct Cyc_List_List*_tmp6A0;struct Cyc_List_List*_tmp6A1;struct _tuple13 _tmp69D=Cyc_Parse_apply_tms(_tmp698,_tmp69B,_tmp69A,_tmp69C);_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;_tmp6A0=_tmp69D.f3;_tmp6A1=_tmp69D.f4;
if(_tmp6A0 != 0){
const char*_tmpCAF;void*_tmpCAE;(_tmpCAE=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCAF="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCAF,sizeof(char),49))),_tag_dyneither(_tmpCAE,sizeof(void*),0)));}
# 2260
if(_tmp6A1 != 0){
const char*_tmpCB2;void*_tmpCB1;(_tmpCB1=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCB2="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCB2,sizeof(char),38))),_tag_dyneither(_tmpCB1,sizeof(void*),0)));}
{struct _tuple8*_tmpCB3;yyval=Cyc_YY40(((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->f1=0,((_tmpCB3->f2=_tmp69E,((_tmpCB3->f3=_tmp69F,_tmpCB3)))))))));}
# 2264
break;};}case 237: _LL31E: {
# 2264 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2268
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 238: _LL31F: {
# 2268 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2272
{struct _dyneither_ptr*_tmpCB6;struct Cyc_List_List*_tmpCB5;yyval=Cyc_YY39(((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->hd=((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCB6)))),((_tmpCB5->tl=0,_tmpCB5)))))));}
break;}case 239: _LL320: {
# 2270 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2274
{struct _dyneither_ptr*_tmpCB9;struct Cyc_List_List*_tmpCB8;yyval=Cyc_YY39(((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->hd=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCB9)))),((_tmpCB8->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCB8)))))));}
break;}case 240: _LL321: {
# 2274 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2278
yyval=yyyyvsp[0];
break;}case 241: _LL322: {
# 2275 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2277
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2279
yyval=yyyyvsp[0];
break;}case 242: _LL323: {
# 2280 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2284
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBC;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=35,((_tmpCBC.f1=0,((_tmpCBC.f2=0,_tmpCBC)))))),_tmpCBB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 243: _LL324: {
# 2282 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2286
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCBF;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCBE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=35,((_tmpCBF.f1=0,((_tmpCBF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCBF)))))),_tmpCBE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL325: {
# 2284 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2288
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCC2;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCC1;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1[0]=((_tmpCC2.tag=35,((_tmpCC2.f1=0,((_tmpCC2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCC2)))))),_tmpCC1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL326: {
# 2286 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2290
struct _dyneither_ptr*_tmpCC5;struct _tuple0*_tmpCC4;struct Cyc_Absyn_Vardecl*_tmp6B1=Cyc_Absyn_new_vardecl(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=Cyc_Absyn_Loc_n,((_tmpCC4->f2=((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCC5)))),_tmpCC4)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2293
(_tmp6B1->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCC8;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCC7;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=26,((_tmpCC8.f1=_tmp6B1,((_tmpCC8.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCC8.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCC8.f4=0,_tmpCC8)))))))))),_tmpCC7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2296
break;}case 246: _LL327: {
# 2293 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2297
void*_tmp6B6=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCCB;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCCA;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA[0]=((_tmpCCB.tag=27,((_tmpCCB.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCCB.f2=_tmp6B6,((_tmpCCB.f3=0,_tmpCCB)))))))),_tmpCCA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2300
break;}case 247: _LL328: {
# 2301 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2305
{struct _tuple30*_tmpCCE;struct Cyc_List_List*_tmpCCD;yyval=Cyc_YY7(((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD->hd=((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE->f1=0,((_tmpCCE->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCCE)))))),((_tmpCCD->tl=0,_tmpCCD)))))));}
break;}case 248: _LL329: {
# 2303 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2307
{struct _tuple30*_tmpCD1;struct Cyc_List_List*_tmpCD0;yyval=Cyc_YY7(((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCD1->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCD1)))))),((_tmpCD0->tl=0,_tmpCD0)))))));}
break;}case 249: _LL32A: {
# 2305 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2309
{struct _tuple30*_tmpCD4;struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY7(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->f1=0,((_tmpCD4->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCD4)))))),((_tmpCD3->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCD3)))))));}
break;}case 250: _LL32B: {
# 2307 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2311
{struct _tuple30*_tmpCD7;struct Cyc_List_List*_tmpCD6;yyval=Cyc_YY7(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->hd=((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCD7->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCD7)))))),((_tmpCD6->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCD6)))))));}
break;}case 251: _LL32C: {
# 2311 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2315
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 252: _LL32D: {
# 2316 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2320
{struct Cyc_List_List*_tmpCD8;yyval=Cyc_YY44(((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCD8->tl=0,_tmpCD8)))))));}
break;}case 253: _LL32E: {
# 2317 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2321
{struct Cyc_List_List*_tmpCD9;yyval=Cyc_YY44(((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCD9->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCD9)))))));}
break;}case 254: _LL32F: {
# 2321 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2325
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCDC;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCDB;yyval=Cyc_YY45((void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=0,((_tmpCDC.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCDC)))),_tmpCDB)))));}
break;}case 255: _LL330: {
# 2322 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2326
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCE2;struct _dyneither_ptr*_tmpCE1;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCE0;yyval=Cyc_YY45((void*)((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCE2.tag=1,((_tmpCE2.f1=((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpCE1)))),_tmpCE2)))),_tmpCE0)))));}
break;}case 256: _LL331: {
# 2327 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2331
struct Cyc_Absyn_Tqual _tmp6C9;struct Cyc_Parse_Type_specifier _tmp6CA;struct Cyc_List_List*_tmp6CB;struct _tuple25 _tmp6C8=Cyc_yyget_YY38(yyyyvsp[0]);_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;_tmp6CB=_tmp6C8.f3;{
void*_tmp6CC=Cyc_Parse_speclist2typ(_tmp6CA,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6CB != 0){
const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE5="ignoring attributes in type",_tag_dyneither(_tmpCE5,sizeof(char),28))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
{struct _tuple8*_tmpCE6;yyval=Cyc_YY40(((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6->f1=0,((_tmpCE6->f2=_tmp6C9,((_tmpCE6->f3=_tmp6CC,_tmpCE6)))))))));}
# 2337
break;};}case 257: _LL332: {
# 2334 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2338
struct Cyc_Absyn_Tqual _tmp6D1;struct Cyc_Parse_Type_specifier _tmp6D2;struct Cyc_List_List*_tmp6D3;struct _tuple25 _tmp6D0=Cyc_yyget_YY38(yyyyvsp[0]);_tmp6D1=_tmp6D0.f1;_tmp6D2=_tmp6D0.f2;_tmp6D3=_tmp6D0.f3;{
void*_tmp6D4=Cyc_Parse_speclist2typ(_tmp6D2,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6D5=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6D6=Cyc_Parse_apply_tms(_tmp6D1,_tmp6D4,_tmp6D3,_tmp6D5);
if(_tmp6D6.f3 != 0){
# 2344
const char*_tmpCE9;void*_tmpCE8;(_tmpCE8=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCE9="bad type params, ignoring",_tag_dyneither(_tmpCE9,sizeof(char),26))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}
if(_tmp6D6.f4 != 0){
const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpCEC="bad specifiers, ignoring",_tag_dyneither(_tmpCEC,sizeof(char),25))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}
{struct _tuple8*_tmpCED;yyval=Cyc_YY40(((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->f1=0,((_tmpCED->f2=_tmp6D6.f1,((_tmpCED->f3=_tmp6D6.f2,_tmpCED)))))))));}
# 2349
break;};}case 258: _LL333: {
# 2348 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2352
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 259: _LL334: {
# 2349 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2353
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCEF;yyval=Cyc_YY47((void*)((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=24,((_tmpCF0.f1=0,_tmpCF0)))),_tmpCEF)))));}
break;}case 260: _LL335: {
# 2350 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2354
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCF3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCF2;yyval=Cyc_YY47((void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF3.tag=24,((_tmpCF3.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCF3)))),_tmpCF2)))));}
break;}case 261: _LL336: {
# 2351 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2355
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCF6;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCF5;yyval=Cyc_YY47((void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=25,((_tmpCF6.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCF6)))),_tmpCF5)))));}
break;}case 262: _LL337: {
# 2352 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2356
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCFC;struct Cyc_List_List*_tmpCFB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCFA;yyval=Cyc_YY47((void*)((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFC.tag=24,((_tmpCFC.f1=((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCFB->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCFB)))))),_tmpCFC)))),_tmpCFA)))));}
break;}case 263: _LL338: {
# 2358 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2362
{struct Cyc_List_List*_tmpCFD;yyval=Cyc_YY43(((_tmpCFD=_cycalloc(sizeof(*_tmpCFD)),((_tmpCFD->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCFD->tl=0,_tmpCFD)))))));}
break;}case 264: _LL339: {
# 2359 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2363
{struct Cyc_List_List*_tmpCFE;yyval=Cyc_YY43(((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpCFE->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCFE)))))));}
break;}case 265: _LL33A: {
# 2364 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2368
{struct Cyc_Parse_Abstractdeclarator _tmpCFF;yyval=Cyc_YY33(((_tmpCFF.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpCFF)));}
break;}case 266: _LL33B: {
# 2366 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2370
yyval=yyyyvsp[0];
break;}case 267: _LL33C: {
# 2368 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2372
{struct Cyc_Parse_Abstractdeclarator _tmpD00;yyval=Cyc_YY33(((_tmpD00.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpD00)));}
break;}case 268: _LL33D: {
# 2373 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2377
yyval=yyyyvsp[1];
break;}case 269: _LL33E: {
# 2375 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2379
{struct Cyc_List_List*_tmpD0A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD09;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD08;struct Cyc_Parse_Abstractdeclarator _tmpD07;yyval=Cyc_YY33(((_tmpD07.tms=((_tmpD0A=_region_malloc(yyr,sizeof(*_tmpD0A)),((_tmpD0A->hd=(void*)((_tmpD08=_region_malloc(yyr,sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD09.tag=0,((_tmpD09.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD09.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD09)))))),_tmpD08)))),((_tmpD0A->tl=0,_tmpD0A)))))),_tmpD07)));}
break;}case 270: _LL33F: {
# 2377 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2381
{struct Cyc_List_List*_tmpD14;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD13;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD12;struct Cyc_Parse_Abstractdeclarator _tmpD11;yyval=Cyc_YY33(((_tmpD11.tms=((_tmpD14=_region_malloc(yyr,sizeof(*_tmpD14)),((_tmpD14->hd=(void*)((_tmpD12=_region_malloc(yyr,sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD13.tag=0,((_tmpD13.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD13.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD13)))))),_tmpD12)))),((_tmpD14->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD14)))))),_tmpD11)));}
break;}case 271: _LL340: {
# 2379 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2383
{struct Cyc_List_List*_tmpD1E;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD1D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD1C;struct Cyc_Parse_Abstractdeclarator _tmpD1B;yyval=Cyc_YY33(((_tmpD1B.tms=((_tmpD1E=_region_malloc(yyr,sizeof(*_tmpD1E)),((_tmpD1E->hd=(void*)((_tmpD1C=_region_malloc(yyr,sizeof(*_tmpD1C)),((_tmpD1C[0]=((_tmpD1D.tag=1,((_tmpD1D.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD1D.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD1D.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD1D)))))))),_tmpD1C)))),((_tmpD1E->tl=0,_tmpD1E)))))),_tmpD1B)));}
break;}case 272: _LL341: {
# 2381 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2385
{struct Cyc_List_List*_tmpD28;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD27;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD26;struct Cyc_Parse_Abstractdeclarator _tmpD25;yyval=Cyc_YY33(((_tmpD25.tms=((_tmpD28=_region_malloc(yyr,sizeof(*_tmpD28)),((_tmpD28->hd=(void*)((_tmpD26=_region_malloc(yyr,sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD27.tag=1,((_tmpD27.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD27.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD27.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD27)))))))),_tmpD26)))),((_tmpD28->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD28)))))),_tmpD25)));}
# 2388
break;}case 273: _LL342: {
# 2385 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2389
{struct Cyc_List_List*_tmpD3D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD3C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD3B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD3A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD39;struct Cyc_Parse_Abstractdeclarator _tmpD38;yyval=Cyc_YY33(((_tmpD38.tms=((_tmpD3D=_region_malloc(yyr,sizeof(*_tmpD3D)),((_tmpD3D->hd=(void*)((_tmpD39=_region_malloc(yyr,sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD3C.tag=3,((_tmpD3C.f1=(void*)((_tmpD3A=_region_malloc(yyr,sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=1,((_tmpD3B.f1=0,((_tmpD3B.f2=0,((_tmpD3B.f3=0,((_tmpD3B.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD3B.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD3B)))))))))))),_tmpD3A)))),_tmpD3C)))),_tmpD39)))),((_tmpD3D->tl=0,_tmpD3D)))))),_tmpD38)));}
# 2391
break;}case 274: _LL343: {
# 2388 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2392
struct Cyc_List_List*_tmp700;int _tmp701;struct Cyc_Absyn_VarargInfo*_tmp702;void*_tmp703;struct Cyc_List_List*_tmp704;struct _tuple26*_tmp6FF=Cyc_yyget_YY42(yyyyvsp[1]);_tmp700=_tmp6FF->f1;_tmp701=_tmp6FF->f2;_tmp702=_tmp6FF->f3;_tmp703=_tmp6FF->f4;_tmp704=_tmp6FF->f5;
{struct Cyc_List_List*_tmpD52;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD51;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD50;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD4F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4E;struct Cyc_Parse_Abstractdeclarator _tmpD4D;yyval=Cyc_YY33(((_tmpD4D.tms=((_tmpD52=_region_malloc(yyr,sizeof(*_tmpD52)),((_tmpD52->hd=(void*)((_tmpD4E=_region_malloc(yyr,sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD51.tag=3,((_tmpD51.f1=(void*)((_tmpD4F=_region_malloc(yyr,sizeof(*_tmpD4F)),((_tmpD4F[0]=((_tmpD50.tag=1,((_tmpD50.f1=_tmp700,((_tmpD50.f2=_tmp701,((_tmpD50.f3=_tmp702,((_tmpD50.f4=_tmp703,((_tmpD50.f5=_tmp704,_tmpD50)))))))))))),_tmpD4F)))),_tmpD51)))),_tmpD4E)))),((_tmpD52->tl=0,_tmpD52)))))),_tmpD4D)));}
# 2395
break;}case 275: _LL344: {
# 2392 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2396
{struct Cyc_List_List*_tmpD67;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD66;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD65;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD64;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD63;struct Cyc_Parse_Abstractdeclarator _tmpD62;yyval=Cyc_YY33(((_tmpD62.tms=((_tmpD67=_region_malloc(yyr,sizeof(*_tmpD67)),((_tmpD67->hd=(void*)((_tmpD63=_region_malloc(yyr,sizeof(*_tmpD63)),((_tmpD63[0]=((_tmpD66.tag=3,((_tmpD66.f1=(void*)((_tmpD64=_region_malloc(yyr,sizeof(*_tmpD64)),((_tmpD64[0]=((_tmpD65.tag=1,((_tmpD65.f1=0,((_tmpD65.f2=0,((_tmpD65.f3=0,((_tmpD65.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD65.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD65)))))))))))),_tmpD64)))),_tmpD66)))),_tmpD63)))),((_tmpD67->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD67)))))),_tmpD62)));}
# 2399
break;}case 276: _LL345: {
# 2396 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2400
struct Cyc_List_List*_tmp712;int _tmp713;struct Cyc_Absyn_VarargInfo*_tmp714;void*_tmp715;struct Cyc_List_List*_tmp716;struct _tuple26*_tmp711=Cyc_yyget_YY42(yyyyvsp[2]);_tmp712=_tmp711->f1;_tmp713=_tmp711->f2;_tmp714=_tmp711->f3;_tmp715=_tmp711->f4;_tmp716=_tmp711->f5;
{struct Cyc_List_List*_tmpD7C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD7B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD7A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD79;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD78;struct Cyc_Parse_Abstractdeclarator _tmpD77;yyval=Cyc_YY33(((_tmpD77.tms=((_tmpD7C=_region_malloc(yyr,sizeof(*_tmpD7C)),((_tmpD7C->hd=(void*)((_tmpD78=_region_malloc(yyr,sizeof(*_tmpD78)),((_tmpD78[0]=((_tmpD7B.tag=3,((_tmpD7B.f1=(void*)((_tmpD79=_region_malloc(yyr,sizeof(*_tmpD79)),((_tmpD79[0]=((_tmpD7A.tag=1,((_tmpD7A.f1=_tmp712,((_tmpD7A.f2=_tmp713,((_tmpD7A.f3=_tmp714,((_tmpD7A.f4=_tmp715,((_tmpD7A.f5=_tmp716,_tmpD7A)))))))))))),_tmpD79)))),_tmpD7B)))),_tmpD78)))),((_tmpD7C->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD7C)))))),_tmpD77)));}
# 2404
break;}case 277: _LL346: {
# 2402 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2406
struct Cyc_List_List*_tmp71D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpD86;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpD85;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD84;struct Cyc_Parse_Abstractdeclarator _tmpD83;yyval=Cyc_YY33(((_tmpD83.tms=((_tmpD86=_region_malloc(yyr,sizeof(*_tmpD86)),((_tmpD86->hd=(void*)((_tmpD84=_region_malloc(yyr,sizeof(*_tmpD84)),((_tmpD84[0]=((_tmpD85.tag=4,((_tmpD85.f1=_tmp71D,((_tmpD85.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpD85.f3=0,_tmpD85)))))))),_tmpD84)))),((_tmpD86->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD86)))))),_tmpD83)));}
# 2410
break;}case 278: _LL347: {
# 2407 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2411
{struct Cyc_List_List*_tmpD90;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpD8F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8E;struct Cyc_Parse_Abstractdeclarator _tmpD8D;yyval=Cyc_YY33(((_tmpD8D.tms=((_tmpD90=_region_malloc(yyr,sizeof(*_tmpD90)),((_tmpD90->hd=(void*)((_tmpD8E=_region_malloc(yyr,sizeof(*_tmpD8E)),((_tmpD8E[0]=((_tmpD8F.tag=5,((_tmpD8F.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpD8F.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpD8F)))))),_tmpD8E)))),((_tmpD90->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD90)))))),_tmpD8D)));}
# 2413
break;}case 279: _LL348: {
# 2413 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417
yyval=yyyyvsp[0];
break;}case 280: _LL349: {
# 2414 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2418
yyval=yyyyvsp[0];
break;}case 281: _LL34A: {
# 2415 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2419
yyval=yyyyvsp[0];
break;}case 282: _LL34B: {
# 2416 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2420
yyval=yyyyvsp[0];
break;}case 283: _LL34C: {
# 2417 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2421
yyval=yyyyvsp[0];
break;}case 284: _LL34D: {
# 2418 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2422
yyval=yyyyvsp[0];
break;}case 285: _LL34E: {
# 2421 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2425
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpD93;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpD92;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92[0]=((_tmpD93.tag=16,((_tmpD93.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD93)))),_tmpD92)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 286: _LL34F:
# 2425 "parse.y"
 yyval=Cyc_YY5(0);
break;case 287: _LL350: {
# 2427 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2431
{const char*_tmpD94;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpD94="open",_tag_dyneither(_tmpD94,sizeof(char),5))))!= 0){
const char*_tmpD95;Cyc_Parse_err(((_tmpD95="expecting `open'",_tag_dyneither(_tmpD95,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2435
break;}case 288: _LL351: {
# 2436 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2440
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpD9B;struct _dyneither_ptr*_tmpD9A;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpD99;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9B.tag=13,((_tmpD9B.f1=((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpD9A)))),((_tmpD9B.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpD9B)))))),_tmpD99)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 289: _LL352: {
# 2440 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2444
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 290: _LL353: {
# 2441 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2445
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 291: _LL354: {
# 2446 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2450
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL355: {
# 2447 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2451
yyval=yyyyvsp[1];
break;}case 293: _LL356: {
# 2452 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2456
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt(0)));
break;}case 294: _LL357: {
# 2453 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2457
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 295: _LL358: {
# 2454 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2458
yyval=yyyyvsp[0];
break;}case 296: _LL359: {
# 2455 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2459
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 297: _LL35A: {
# 2456 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2460
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpD9E;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD9D;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpD9E.tag=1,((_tmpD9E.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpD9E)))),_tmpD9D)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 298: _LL35B: {
# 2459 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2463
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDA1;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDA0;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpDA1.tag=1,((_tmpDA1.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDA1)))),_tmpDA0)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 299: _LL35C: {
# 2464 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2468
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL35D: {
# 2466 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2470
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL35E: {
# 2472 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2476
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 302: _LL35F: {
# 2475 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2479
struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp731,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL360: {
# 2479 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2481
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2483
struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp732,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2486
break;}case 304: _LL361: {
# 2485 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2489
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL362:
# 2499 "parse.y"
 yyval=Cyc_YY11(0);
break;case 306: _LL363: {
# 2501 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2505
{struct Cyc_Absyn_Switch_clause*_tmpDA4;struct Cyc_List_List*_tmpDA3;yyval=Cyc_YY11(((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->hd=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDA4->pat_vars=0,((_tmpDA4->where_clause=0,((_tmpDA4->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDA4->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDA4)))))))))))),((_tmpDA3->tl=0,_tmpDA3)))))));}
# 2508
break;}case 307: _LL364: {
# 2505 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2509
{struct Cyc_Absyn_Switch_clause*_tmpDA7;struct Cyc_List_List*_tmpDA6;yyval=Cyc_YY11(((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->hd=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDA7->pat_vars=0,((_tmpDA7->where_clause=0,((_tmpDA7->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDA7->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDA7)))))))))))),((_tmpDA6->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDA6)))))));}
break;}case 308: _LL365: {
# 2509 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2513
{struct Cyc_Absyn_Switch_clause*_tmpDAA;struct Cyc_List_List*_tmpDA9;yyval=Cyc_YY11(((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->hd=((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDAA->pat_vars=0,((_tmpDAA->where_clause=0,((_tmpDAA->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDAA->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAA)))))))))))),((_tmpDA9->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDA9)))))));}
break;}case 309: _LL366: {
# 2511 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2515
{struct Cyc_Absyn_Switch_clause*_tmpDAD;struct Cyc_List_List*_tmpDAC;yyval=Cyc_YY11(((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDAD->pat_vars=0,((_tmpDAD->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDAD->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDAD->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDAD)))))))))))),((_tmpDAC->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDAC)))))));}
break;}case 310: _LL367: {
# 2515 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2519
{struct Cyc_Absyn_Switch_clause*_tmpDB0;struct Cyc_List_List*_tmpDAF;yyval=Cyc_YY11(((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->hd=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDB0->pat_vars=0,((_tmpDB0->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDB0->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDB0->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDB0)))))))))))),((_tmpDAF->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDAF)))))));}
break;}case 311: _LL368: {
# 2522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2526
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 312: _LL369: {
# 2526 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2530
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 313: _LL36A: {
# 2530 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2534
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 314: _LL36B: {
# 2533 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2537
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL36C: {
# 2536 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2540
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL36D: {
# 2539 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2543
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL36E: {
# 2542 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2546
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL36F: {
# 2545 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2549
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL370: {
# 2548 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2552
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL371: {
# 2551 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2555
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL372: {
# 2554 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2556
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2558
struct Cyc_List_List*_tmp73D=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp73E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73D,_tmp73E));
# 2563
break;}case 322: _LL373: {
# 2560 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2564
struct Cyc_List_List*_tmp73F=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp740=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp73F,_tmp740));
# 2569
break;}case 323: _LL374: {
# 2566 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2570
struct Cyc_List_List*_tmp741=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp742=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp741,_tmp742));
# 2575
break;}case 324: _LL375: {
# 2572 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2576
struct Cyc_List_List*_tmp743=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp744=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp743,_tmp744));
# 2581
break;}case 325: _LL376: {
# 2582 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2586
{struct _dyneither_ptr*_tmpDB1;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDB1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 326: _LL377: {
# 2583 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2587
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 327: _LL378: {
# 2584 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2588
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 328: _LL379: {
# 2585 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2589
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 329: _LL37A: {
# 2586 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2590
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL37B: {
# 2588 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2592
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL37C: {
# 2589 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2593
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL37D: {
# 2591 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2595
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL37E: {
# 2600 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604
yyval=yyyyvsp[0];
break;}case 334: _LL37F: {
# 2603 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2607
yyval=yyyyvsp[0];
break;}case 335: _LL380: {
# 2605 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2609
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 336: _LL381: {
# 2608 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2612
yyval=yyyyvsp[0];
break;}case 337: _LL382: {
# 2610 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2614
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 338: _LL383: {
# 2613 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2617
yyval=yyyyvsp[0];
break;}case 339: _LL384: {
# 2615 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2619
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL385: {
# 2618 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2622
yyval=yyyyvsp[0];
break;}case 341: _LL386: {
# 2620 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2624
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL387: {
# 2623 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2627
yyval=yyyyvsp[0];
break;}case 343: _LL388: {
# 2625 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2629
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL389: {
# 2628 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2632
yyval=yyyyvsp[0];
break;}case 345: _LL38A: {
# 2630 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2634
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL38B: {
# 2633 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2637
yyval=yyyyvsp[0];
break;}case 347: _LL38C: {
# 2635 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2639
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL38D: {
# 2637 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2641
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL38E: {
# 2640 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2644
yyval=yyyyvsp[0];
break;}case 350: _LL38F: {
# 2642 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2646
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL390: {
# 2644 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2648
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL391: {
# 2646 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2650
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL392: {
# 2648 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2652
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL393: {
# 2651 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2655
yyval=yyyyvsp[0];
break;}case 355: _LL394: {
# 2653 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2657
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL395: {
# 2655 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2659
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL396: {
# 2658 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2662
yyval=yyyyvsp[0];
break;}case 358: _LL397: {
# 2660 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2664
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL398: {
# 2662 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2666
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL399: {
# 2665 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2669
yyval=yyyyvsp[0];
break;}case 361: _LL39A: {
# 2667 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2671
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL39B: {
# 2669 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2673
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL39C: {
# 2671 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2675
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL39D: {
# 2674 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2678
yyval=yyyyvsp[0];
break;}case 365: _LL39E: {
# 2676 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2680
void*_tmp746=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp746,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2683
break;}case 366: _LL39F: {
# 2682 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2686
yyval=yyyyvsp[0];
break;}case 367: _LL3A0: {
# 2685 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2689
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 368: _LL3A1: {
# 2687 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2691
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 369: _LL3A2: {
# 2689 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2693
void*_tmp747=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp747,(unsigned int)(yyyylsp[0]).first_line)));
# 2696
break;}case 370: _LL3A3: {
# 2693 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2697
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 371: _LL3A4: {
# 2695 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2699
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 372: _LL3A5: {
# 2700 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2704
yyval=yyyyvsp[0];
break;}case 373: _LL3A6: {
# 2709 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2713
yyval=yyyyvsp[0];
break;}case 374: _LL3A7: {
# 2714 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2718
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 375: _LL3A8: {
# 2716 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2720
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 376: _LL3A9: {
# 2718 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2722
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp748=e->r;enum Cyc_Absyn_Sign _tmp74A;char _tmp74B;enum Cyc_Absyn_Sign _tmp74D;short _tmp74E;enum Cyc_Absyn_Sign _tmp750;int _tmp751;struct _dyneither_ptr _tmp753;int _tmp754;_LL3AC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp749=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp749->tag != 0)goto _LL3AE;else{if(((_tmp749->f1).Char_c).tag != 2)goto _LL3AE;_tmp74A=((struct _tuple3)((_tmp749->f1).Char_c).val).f1;_tmp74B=((struct _tuple3)((_tmp749->f1).Char_c).val).f2;}}_LL3AD:
# 2726
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDB4;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDB3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB3=_cycalloc_atomic(sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB4.tag=10,((_tmpDB4.f1=_tmp74B,_tmpDB4)))),_tmpDB3)))),e->loc));}goto _LL3AB;_LL3AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp74C->tag != 0)goto _LL3B0;else{if(((_tmp74C->f1).Short_c).tag != 4)goto _LL3B0;_tmp74D=((struct _tuple4)((_tmp74C->f1).Short_c).val).f1;_tmp74E=((struct _tuple4)((_tmp74C->f1).Short_c).val).f2;}}_LL3AF:
# 2728
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDB7;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDB6;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB6=_cycalloc_atomic(sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=9,((_tmpDB7.f1=_tmp74D,((_tmpDB7.f2=(int)_tmp74E,_tmpDB7)))))),_tmpDB6)))),e->loc));}goto _LL3AB;_LL3B0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp74F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp74F->tag != 0)goto _LL3B2;else{if(((_tmp74F->f1).Int_c).tag != 5)goto _LL3B2;_tmp750=((struct _tuple5)((_tmp74F->f1).Int_c).val).f1;_tmp751=((struct _tuple5)((_tmp74F->f1).Int_c).val).f2;}}_LL3B1:
# 2730
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDBA;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDB9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDB9=_cycalloc_atomic(sizeof(*_tmpDB9)),((_tmpDB9[0]=((_tmpDBA.tag=9,((_tmpDBA.f1=_tmp750,((_tmpDBA.f2=_tmp751,_tmpDBA)))))),_tmpDB9)))),e->loc));}goto _LL3AB;_LL3B2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp752=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp752->tag != 0)goto _LL3B4;else{if(((_tmp752->f1).Float_c).tag != 7)goto _LL3B4;_tmp753=((struct _tuple7)((_tmp752->f1).Float_c).val).f1;_tmp754=((struct _tuple7)((_tmp752->f1).Float_c).val).f2;}}_LL3B3:
# 2732
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDBD;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDBC;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBD.tag=11,((_tmpDBD.f1=_tmp753,((_tmpDBD.f2=_tmp754,_tmpDBD)))))),_tmpDBC)))),e->loc));}goto _LL3AB;_LL3B4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp755=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp755->tag != 0)goto _LL3B6;else{if(((_tmp755->f1).Null_c).tag != 1)goto _LL3B6;}}_LL3B5:
# 2734
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3AB;_LL3B6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp756=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp756->tag != 0)goto _LL3B8;else{if(((_tmp756->f1).String_c).tag != 8)goto _LL3B8;}}_LL3B7:
# 2736
{const char*_tmpDBE;Cyc_Parse_err(((_tmpDBE="strings cannot occur within patterns",_tag_dyneither(_tmpDBE,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3AB;_LL3B8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp757=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp757->tag != 0)goto _LL3BA;else{if(((_tmp757->f1).Wstring_c).tag != 9)goto _LL3BA;}}_LL3B9:
# 2738
{const char*_tmpDBF;Cyc_Parse_err(((_tmpDBF="strings cannot occur within patterns",_tag_dyneither(_tmpDBF,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3AB;_LL3BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp758=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp748;if(_tmp758->tag != 0)goto _LL3BC;else{if(((_tmp758->f1).LongLong_c).tag != 6)goto _LL3BC;}}_LL3BB:
# 2740
{const char*_tmpDC0;Cyc_Parse_unimp(((_tmpDC0="long long's in patterns",_tag_dyneither(_tmpDC0,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3AB;_LL3BC:;_LL3BD: {
# 2742
const char*_tmpDC1;Cyc_Parse_err(((_tmpDC1="bad constant in case",_tag_dyneither(_tmpDC1,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3AB:;}
# 2745
break;}case 377: _LL3AA: {
# 2742 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2746
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDC4;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDC3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=14,((_tmpDC4.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDC4)))),_tmpDC3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 378: _LL3BE: {
# 2744 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2748
{const char*_tmpDC5;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDC5="as",_tag_dyneither(_tmpDC5,sizeof(char),3))))!= 0){
const char*_tmpDC6;Cyc_Parse_err(((_tmpDC6="expecting `as'",_tag_dyneither(_tmpDC6,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDD0;struct _dyneither_ptr*_tmpDCF;struct _tuple0*_tmpDCE;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDCD;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDD0.tag=1,((_tmpDD0.f1=Cyc_Absyn_new_vardecl(((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->f1=Cyc_Absyn_Loc_n,((_tmpDCE->f2=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDCF)))),_tmpDCE)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDD0.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDD0)))))),_tmpDCD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2753
break;}case 379: _LL3BF: {
# 2750 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2754
struct Cyc_List_List*_tmp76E;int _tmp76F;struct _tuple22 _tmp76D=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp76E=_tmp76D.f1;_tmp76F=_tmp76D.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpDD3;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpDD2;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD3.tag=4,((_tmpDD3.f1=_tmp76E,((_tmpDD3.f2=_tmp76F,_tmpDD3)))))),_tmpDD2)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2757
break;}case 380: _LL3C0: {
# 2754 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2758
struct Cyc_List_List*_tmp773;int _tmp774;struct _tuple22 _tmp772=*Cyc_yyget_YY13(yyyyvsp[2]);_tmp773=_tmp772.f1;_tmp774=_tmp772.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpDD6;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDD5;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5[0]=((_tmpDD6.tag=15,((_tmpDD6.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpDD6.f2=_tmp773,((_tmpDD6.f3=_tmp774,_tmpDD6)))))))),_tmpDD5)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2761
break;}case 381: _LL3C1: {
# 2758 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2762
struct Cyc_List_List*_tmp778;int _tmp779;struct _tuple22 _tmp777=*Cyc_yyget_YY17(yyyyvsp[3]);_tmp778=_tmp777.f1;_tmp779=_tmp777.f2;{
struct Cyc_List_List*_tmp77A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDDC;struct Cyc_Absyn_AggrInfo*_tmpDDB;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDDA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDDC.tag=6,((_tmpDDC.f1=((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpDDB->targs=0,_tmpDDB)))))),((_tmpDDC.f2=_tmp77A,((_tmpDDC.f3=_tmp778,((_tmpDDC.f4=_tmp779,_tmpDDC)))))))))),_tmpDDA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2767
break;};}case 382: _LL3C2: {
# 2764 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2768
struct Cyc_List_List*_tmp77F;int _tmp780;struct _tuple22 _tmp77E=*Cyc_yyget_YY17(yyyyvsp[2]);_tmp77F=_tmp77E.f1;_tmp780=_tmp77E.f2;{
struct Cyc_List_List*_tmp781=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpDDF;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpDDE;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=6,((_tmpDDF.f1=0,((_tmpDDF.f2=_tmp781,((_tmpDDF.f3=_tmp77F,((_tmpDDF.f4=_tmp780,_tmpDDF)))))))))),_tmpDDE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2772
break;};}case 383: _LL3C3: {
# 2769 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2773
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDE2;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDE1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE2.tag=5,((_tmpDE2.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDE2)))),_tmpDE1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 384: _LL3C4: {
# 2771 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2775
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDEC;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpDEB;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDEA;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpDE9;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEC.tag=5,((_tmpDEC.f1=Cyc_Absyn_new_pat((void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEB.tag=5,((_tmpDEB.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpDEB)))),_tmpDEA)))),(unsigned int)(yyyylsp[0]).first_line),_tmpDEC)))),_tmpDE9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 385: _LL3C5: {
# 2773 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2777
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpDF6;struct _dyneither_ptr*_tmpDF5;struct _tuple0*_tmpDF4;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDF3;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF6.tag=2,((_tmpDF6.f1=Cyc_Absyn_new_vardecl(((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->f1=Cyc_Absyn_Loc_n,((_tmpDF4->f2=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDF5)))),_tmpDF4)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDF6.f2=
# 2779
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpDF6)))))),_tmpDF3)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2781
break;}case 386: _LL3C6: {
# 2778 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2782
{const char*_tmpDF7;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpDF7="as",_tag_dyneither(_tmpDF7,sizeof(char),3))))!= 0){
const char*_tmpDF8;Cyc_Parse_err(((_tmpDF8="expecting `as'",_tag_dyneither(_tmpDF8,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE02;struct _dyneither_ptr*_tmpE01;struct _tuple0*_tmpE00;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpDFF;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpE02.tag=2,((_tmpE02.f1=Cyc_Absyn_new_vardecl(((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->f1=Cyc_Absyn_Loc_n,((_tmpE00->f2=((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE01)))),_tmpE00)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE02.f2=
# 2786
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE02)))))),_tmpDFF)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2788
break;}case 387: _LL3C7: {
# 2785 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2789
void*_tmp794=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE13;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE12;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE11;struct _dyneither_ptr*_tmpE10;struct _tuple0*_tmpE0F;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE0E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE13.tag=3,((_tmpE13.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp794),((_tmpE13.f2=
Cyc_Absyn_new_vardecl(((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->f1=Cyc_Absyn_Loc_n,((_tmpE0F->f2=((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE10)))),_tmpE0F)))))),(void*)(
(_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=19,((_tmpE12.f1=_tmp794,_tmpE12)))),_tmpE11)))),0),_tmpE13)))))),_tmpE0E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2794
break;}case 388: _LL3C8: {
# 2791 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2795
struct Cyc_Absyn_Tvar*_tmp79B=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE2F;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE2E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE2D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE2C;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE2B;struct _dyneither_ptr*_tmpE2A;struct _tuple0*_tmpE29;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE28;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE2F.tag=3,((_tmpE2F.f1=_tmp79B,((_tmpE2F.f2=
Cyc_Absyn_new_vardecl(((_tmpE29=_cycalloc(sizeof(*_tmpE29)),((_tmpE29->f1=Cyc_Absyn_Loc_n,((_tmpE29->f2=((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE2A)))),_tmpE29)))))),(void*)(
(_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B[0]=((_tmpE2E.tag=19,((_tmpE2E.f1=(void*)((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C[0]=((_tmpE2D.tag=2,((_tmpE2D.f1=_tmp79B,_tmpE2D)))),_tmpE2C)))),_tmpE2E)))),_tmpE2B)))),0),_tmpE2F)))))),_tmpE28)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2800
break;}case 389: _LL3C9: {
# 2799 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2803
{struct _tuple22*_tmpE30;yyval=Cyc_YY13(((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE30->f2=0,_tmpE30)))))));}
break;}case 390: _LL3CA: {
# 2800 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2804
{struct _tuple22*_tmpE31;yyval=Cyc_YY13(((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE31->f2=1,_tmpE31)))))));}
break;}case 391: _LL3CB: {
# 2801 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2803
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2805
{struct _tuple22*_tmpE32;yyval=Cyc_YY13(((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->f1=0,((_tmpE32->f2=1,_tmpE32)))))));}
break;}case 392: _LL3CC: {
# 2806 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2810
{struct Cyc_List_List*_tmpE33;yyval=Cyc_YY14(((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE33->tl=0,_tmpE33)))))));}
break;}case 393: _LL3CD: {
# 2808 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2810
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2812
{struct Cyc_List_List*_tmpE34;yyval=Cyc_YY14(((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE34->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE34)))))));}
break;}case 394: _LL3CE: {
# 2813 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2817
{struct _tuple23*_tmpE35;yyval=Cyc_YY15(((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((_tmpE35->f1=0,((_tmpE35->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE35)))))));}
break;}case 395: _LL3CF: {
# 2815 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2819
{struct _tuple23*_tmpE36;yyval=Cyc_YY15(((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE36->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE36)))))));}
break;}case 396: _LL3D0: {
# 2818 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2822
{struct _tuple22*_tmpE37;yyval=Cyc_YY17(((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE37->f2=0,_tmpE37)))))));}
break;}case 397: _LL3D1: {
# 2819 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2823
{struct _tuple22*_tmpE38;yyval=Cyc_YY17(((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE38->f2=1,_tmpE38)))))));}
break;}case 398: _LL3D2: {
# 2820 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2824
{struct _tuple22*_tmpE39;yyval=Cyc_YY17(((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->f1=0,((_tmpE39->f2=1,_tmpE39)))))));}
break;}case 399: _LL3D3: {
# 2825 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2829
{struct Cyc_List_List*_tmpE3A;yyval=Cyc_YY16(((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE3A->tl=0,_tmpE3A)))))));}
break;}case 400: _LL3D4: {
# 2827 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2831
{struct Cyc_List_List*_tmpE3B;yyval=Cyc_YY16(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE3B->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE3B)))))));}
break;}case 401: _LL3D5: {
# 2833 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2837
yyval=yyyyvsp[0];
break;}case 402: _LL3D6: {
# 2835 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2839
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 403: _LL3D7: {
# 2840 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2844
yyval=yyyyvsp[0];
break;}case 404: _LL3D8: {
# 2842 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2846
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 405: _LL3D9: {
# 2844 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2848
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 406: _LL3DA: {
# 2848 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852
yyval=Cyc_YY9(0);
break;}case 407: _LL3DB: {
# 2849 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853
{struct Cyc_Core_Opt*_tmpE3C;yyval=Cyc_YY9(((_tmpE3C=_cycalloc_atomic(sizeof(*_tmpE3C)),((_tmpE3C->v=(void*)Cyc_Absyn_Times,_tmpE3C)))));}
break;}case 408: _LL3DC: {
# 2850 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854
{struct Cyc_Core_Opt*_tmpE3D;yyval=Cyc_YY9(((_tmpE3D=_cycalloc_atomic(sizeof(*_tmpE3D)),((_tmpE3D->v=(void*)Cyc_Absyn_Div,_tmpE3D)))));}
break;}case 409: _LL3DD: {
# 2851 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855
{struct Cyc_Core_Opt*_tmpE3E;yyval=Cyc_YY9(((_tmpE3E=_cycalloc_atomic(sizeof(*_tmpE3E)),((_tmpE3E->v=(void*)Cyc_Absyn_Mod,_tmpE3E)))));}
break;}case 410: _LL3DE: {
# 2852 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856
{struct Cyc_Core_Opt*_tmpE3F;yyval=Cyc_YY9(((_tmpE3F=_cycalloc_atomic(sizeof(*_tmpE3F)),((_tmpE3F->v=(void*)Cyc_Absyn_Plus,_tmpE3F)))));}
break;}case 411: _LL3DF: {
# 2853 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857
{struct Cyc_Core_Opt*_tmpE40;yyval=Cyc_YY9(((_tmpE40=_cycalloc_atomic(sizeof(*_tmpE40)),((_tmpE40->v=(void*)Cyc_Absyn_Minus,_tmpE40)))));}
break;}case 412: _LL3E0: {
# 2854 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858
{struct Cyc_Core_Opt*_tmpE41;yyval=Cyc_YY9(((_tmpE41=_cycalloc_atomic(sizeof(*_tmpE41)),((_tmpE41->v=(void*)Cyc_Absyn_Bitlshift,_tmpE41)))));}
break;}case 413: _LL3E1: {
# 2855 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2859
{struct Cyc_Core_Opt*_tmpE42;yyval=Cyc_YY9(((_tmpE42=_cycalloc_atomic(sizeof(*_tmpE42)),((_tmpE42->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE42)))));}
break;}case 414: _LL3E2: {
# 2856 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2860
{struct Cyc_Core_Opt*_tmpE43;yyval=Cyc_YY9(((_tmpE43=_cycalloc_atomic(sizeof(*_tmpE43)),((_tmpE43->v=(void*)Cyc_Absyn_Bitand,_tmpE43)))));}
break;}case 415: _LL3E3: {
# 2857 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2861
{struct Cyc_Core_Opt*_tmpE44;yyval=Cyc_YY9(((_tmpE44=_cycalloc_atomic(sizeof(*_tmpE44)),((_tmpE44->v=(void*)Cyc_Absyn_Bitxor,_tmpE44)))));}
break;}case 416: _LL3E4: {
# 2858 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2862
{struct Cyc_Core_Opt*_tmpE45;yyval=Cyc_YY9(((_tmpE45=_cycalloc_atomic(sizeof(*_tmpE45)),((_tmpE45->v=(void*)Cyc_Absyn_Bitor,_tmpE45)))));}
break;}case 417: _LL3E5: {
# 2863 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2867
yyval=yyyyvsp[0];
break;}case 418: _LL3E6: {
# 2865 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2869
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 419: _LL3E7: {
# 2868 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2872
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 420: _LL3E8: {
# 2871 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2875
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 421: _LL3E9: {
# 2873 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2877
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 422: _LL3EA: {
# 2875 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2879
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3EB: {
# 2877 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2881
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3EC: {
# 2881 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2885
yyval=yyyyvsp[0];
break;}case 425: _LL3ED: {
# 2885 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2889
yyval=yyyyvsp[0];
break;}case 426: _LL3EE: {
# 2887 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2891
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3EF: {
# 2891 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2895
yyval=yyyyvsp[0];
break;}case 428: _LL3F0: {
# 2893 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2897
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3F1: {
# 2897 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2901
yyval=yyyyvsp[0];
break;}case 430: _LL3F2: {
# 2899 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2903
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3F3: {
# 2903 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2907
yyval=yyyyvsp[0];
break;}case 432: _LL3F4: {
# 2905 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2909
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3F5: {
# 2909 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2913
yyval=yyyyvsp[0];
break;}case 434: _LL3F6: {
# 2911 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2915
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3F7: {
# 2915 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2919
yyval=yyyyvsp[0];
break;}case 436: _LL3F8: {
# 2917 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2921
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3F9: {
# 2919 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2923
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 438: _LL3FA: {
# 2923 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2927
yyval=yyyyvsp[0];
break;}case 439: _LL3FB: {
# 2925 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2929
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3FC: {
# 2927 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2931
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3FD: {
# 2929 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2933
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3FE: {
# 2931 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2935
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL3FF: {
# 2935 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2939
yyval=yyyyvsp[0];
break;}case 444: _LL400: {
# 2937 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2941
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL401: {
# 2939 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2943
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL402: {
# 2943 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2947
yyval=yyyyvsp[0];
break;}case 447: _LL403: {
# 2945 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2949
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL404: {
# 2947 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2951
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL405: {
# 2951 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2955
yyval=yyyyvsp[0];
break;}case 450: _LL406: {
# 2953 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2957
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL407: {
# 2955 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2959
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL408: {
# 2957 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2961
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL409: {
# 2961 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2965
yyval=yyyyvsp[0];
break;}case 454: _LL40A: {
# 2963 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2967
void*_tmp7BA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7BA,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2970
break;}case 455: _LL40B: {
# 2969 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2973
yyval=yyyyvsp[0];
break;}case 456: _LL40C: {
# 2970 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2974
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL40D: {
# 2971 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2975
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL40E: {
# 2972 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2976
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 459: _LL40F: {
# 2973 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2977
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 460: _LL410: {
# 2974 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2978
yyval=yyyyvsp[1];
break;}case 461: _LL411: {
# 2975 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2979
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL412: {
# 2977 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2981
void*_tmp7BB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7BB,(unsigned int)(yyyylsp[0]).first_line));
# 2984
break;}case 463: _LL413: {
# 2980 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2984
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL414: {
# 2982 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2986
void*_tmp7BC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7BC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2989
break;}case 465: _LL415: {
# 2987 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2991
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE4B;struct Cyc_Absyn_MallocInfo _tmpE4A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE49;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4B.tag=33,((_tmpE4B.f1=((_tmpE4A.is_calloc=0,((_tmpE4A.rgn=0,((_tmpE4A.elt_type=0,((_tmpE4A.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE4A.fat_result=0,_tmpE4A)))))))))),_tmpE4B)))),_tmpE49)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2993
break;}case 466: _LL416: {
# 2990 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2994
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE51;struct Cyc_Absyn_MallocInfo _tmpE50;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE4F;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE51.tag=33,((_tmpE51.f1=((_tmpE50.is_calloc=0,((_tmpE50.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE50.elt_type=0,((_tmpE50.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE50.fat_result=0,_tmpE50)))))))))),_tmpE51)))),_tmpE4F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2996
break;}case 467: _LL417: {
# 2993 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2997
void*_tmp7C3=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE5B;void**_tmpE5A;struct Cyc_Absyn_MallocInfo _tmpE59;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE58;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=((_tmpE5B.tag=33,((_tmpE5B.f1=((_tmpE59.is_calloc=1,((_tmpE59.rgn=0,((_tmpE59.elt_type=((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=_tmp7C3,_tmpE5A)))),((_tmpE59.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE59.fat_result=0,_tmpE59)))))))))),_tmpE5B)))),_tmpE58)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3000
break;}case 468: _LL418: {
# 2998 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3002
void*_tmp7C8=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE65;void**_tmpE64;struct Cyc_Absyn_MallocInfo _tmpE63;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE62;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE65.tag=33,((_tmpE65.f1=((_tmpE63.is_calloc=1,((_tmpE63.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE63.elt_type=((_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64[0]=_tmp7C8,_tmpE64)))),((_tmpE63.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE63.fat_result=0,_tmpE63)))))))))),_tmpE65)))),_tmpE62)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3005
break;}case 469: _LL419: {
# 3002 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3006
{struct Cyc_Absyn_Exp*_tmpE66[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpE66[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE66,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 470: _LL41A: {
# 3004 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3008
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE6C;struct _dyneither_ptr*_tmpE6B;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE6A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6C.tag=37,((_tmpE6C.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE6C.f2=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE6B)))),_tmpE6C)))))),_tmpE6A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 471: _LL41B: {
# 3006 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3010
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE72;struct _dyneither_ptr*_tmpE71;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE70;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70[0]=((_tmpE72.tag=37,((_tmpE72.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpE72.f2=((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpE71)))),_tmpE72)))))),_tmpE70)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 472: _LL41C: {
# 3008 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3012
void*_tmp7D4=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp7D4,(unsigned int)(yyyylsp[0]).first_line));
break;}case 473: _LL41D: {
# 3011 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3015
int _tmp7D6;struct _dyneither_ptr _tmp7D7;struct _tuple20 _tmp7D5=Cyc_yyget_Asm_tok(yyyyvsp[0]);_tmp7D6=_tmp7D5.f1;_tmp7D7=_tmp7D5.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp7D6,_tmp7D7,(unsigned int)(yyyylsp[0]).first_line));
break;}case 474: _LL41E: {
# 3016 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3020
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 475: _LL41F: {
# 3017 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3021
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 476: _LL420: {
# 3018 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3022
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 477: _LL421: {
# 3023 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3027
yyval=yyyyvsp[0];
break;}case 478: _LL422: {
# 3025 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3029
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL423: {
# 3027 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3031
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 480: _LL424: {
# 3029 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3033
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 481: _LL425: {
# 3031 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3035
{struct _dyneither_ptr*_tmpE73;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE73)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 482: _LL426: {
# 3033 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3037
{struct _dyneither_ptr*_tmpE74;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE74)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 483: _LL427: {
# 3035 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3039
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL428: {
# 3037 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3041
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL429: {
# 3039 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3043
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE77;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE76;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76[0]=((_tmpE77.tag=24,((_tmpE77.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE77.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE77)))))),_tmpE76)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL42A: {
# 3041 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3045
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE7A;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE79;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79[0]=((_tmpE7A.tag=24,((_tmpE7A.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpE7A.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpE7A)))))),_tmpE79)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL42B: {
# 3046 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3050
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE84;struct _dyneither_ptr*_tmpE83;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE82;struct Cyc_List_List*_tmpE81;yyval=Cyc_YY3(((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->hd=(void*)((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84[0]=((_tmpE82.tag=0,((_tmpE82.f1=((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((_tmpE83[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE83)))),_tmpE82)))),_tmpE84)))),((_tmpE81->tl=0,_tmpE81)))))));}
break;}case 488: _LL42C: {
# 3049 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3053
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE8A;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE89;struct Cyc_List_List*_tmpE88;yyval=Cyc_YY3(((_tmpE88=_cycalloc(sizeof(*_tmpE88)),((_tmpE88->hd=(void*)((_tmpE8A=_cycalloc_atomic(sizeof(*_tmpE8A)),((_tmpE8A[0]=((_tmpE89.tag=1,((_tmpE89.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpE89)))),_tmpE8A)))),((_tmpE88->tl=0,_tmpE88)))))));}
break;}case 489: _LL42D: {
# 3051 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3055
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpE94;struct _dyneither_ptr*_tmpE93;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpE92;struct Cyc_List_List*_tmpE91;yyval=Cyc_YY3(((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91->hd=(void*)((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94[0]=((_tmpE92.tag=0,((_tmpE92.f1=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE93)))),_tmpE92)))),_tmpE94)))),((_tmpE91->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE91)))))));}
break;}case 490: _LL42E: {
# 3054 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3058
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpE9A;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpE99;struct Cyc_List_List*_tmpE98;yyval=Cyc_YY3(((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98->hd=(void*)((_tmpE9A=_cycalloc_atomic(sizeof(*_tmpE9A)),((_tmpE9A[0]=((_tmpE99.tag=1,((_tmpE99.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpE99)))),_tmpE9A)))),((_tmpE98->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpE98)))))));}
break;}case 491: _LL42F: {
# 3060 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3064
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 492: _LL430: {
# 3062 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3066
yyval=yyyyvsp[0];
break;}case 493: _LL431: {
# 3064 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3068
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 494: _LL432: {
# 3066 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3070
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 495: _LL433: {
# 3068 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3072
yyval=yyyyvsp[1];
break;}case 496: _LL434: {
# 3073 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3077
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL435: {
# 3075 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3079
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL436: {
# 3078 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3082
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL437: {
# 3081 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3085
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE9D;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE9C;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=((_tmpE9D.tag=28,((_tmpE9D.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE9D.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpE9D.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpE9D.f4=0,_tmpE9D)))))))))),_tmpE9C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 500: _LL438: {
# 3084 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3088
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL439: {
# 3088 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3092
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 502: _LL43A: {
# 3094 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3098
{struct Cyc_List_List*_tmpE9E;yyval=Cyc_YY6(((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpE9E->tl=0,_tmpE9E)))))));}
break;}case 503: _LL43B: {
# 3096 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3100
{struct Cyc_List_List*_tmpE9F;yyval=Cyc_YY6(((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE9F->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpE9F)))))));}
break;}case 504: _LL43C: {
# 3102 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3106
struct _tuple5 _tmp7F0=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp7F0.f1,_tmp7F0.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL43D: {
# 3103 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3107
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL43E: {
# 3104 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3108
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 507: _LL43F: {
# 3105 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3110
struct _dyneither_ptr _tmp7F1=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp7F1);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp7F1,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3118
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp7F1,i,(unsigned int)(yyyylsp[0]).first_line));
# 3120
break;}case 508: _LL440: {
# 3117 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL441: {
# 3121 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125
{struct _dyneither_ptr*_tmpEA2;struct _tuple0*_tmpEA1;yyval=Cyc_QualId_tok(((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1->f1=Cyc_Absyn_Rel_n(0),((_tmpEA1->f2=((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA2)))),_tmpEA1)))))));}
break;}case 510: _LL442: {
# 3122 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126
yyval=yyyyvsp[0];
break;}case 511: _LL443: {
# 3125 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3129
{struct _dyneither_ptr*_tmpEA5;struct _tuple0*_tmpEA4;yyval=Cyc_QualId_tok(((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4->f1=Cyc_Absyn_Rel_n(0),((_tmpEA4->f2=((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEA5)))),_tmpEA4)))))));}
break;}case 512: _LL444: {
# 3126 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3130
yyval=yyyyvsp[0];
break;}case 513: _LL445: {
# 3131 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135
yyval=yyyyvsp[0];
break;}case 514: _LL446: {
# 3132 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136
yyval=yyyyvsp[0];
break;}case 515: _LL447: {
# 3135 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3139
yyval=yyyyvsp[0];
break;}case 516: _LL448: {
# 3136 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3140
yyval=yyyyvsp[0];
break;}case 517: _LL449: {
# 3140 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3145
break;}case 518: _LL44A: {
# 3141 "parse.y"
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3145
yylex_buf->lex_curr_pos -=1;
break;}default: _LL44B:
# 3150
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(519,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 147)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6695) && Cyc_yycheck[_check_known_subscript_notnull(6696,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6696,yystate)];else{
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
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
# 431
if(yyn > - 32768  && yyn < 6695){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6696,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)])+ 15,count ++);}
{unsigned int _tmpEAA;unsigned int _tmpEA9;struct _dyneither_ptr _tmpEA8;char*_tmpEA7;unsigned int _tmpEA6;msg=((_tmpEA6=(unsigned int)(sze + 15),((_tmpEA7=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEA6 + 1)),((_tmpEA8=_tag_dyneither(_tmpEA7,sizeof(char),_tmpEA6 + 1),((((_tmpEA9=_tmpEA6,((_tmpEAB(& _tmpEAA,& _tmpEA9,& _tmpEA7),_tmpEA7[_tmpEA9]=(char)0)))),_tmpEA8))))))));}
{const char*_tmpEAC;Cyc_strcpy(msg,((_tmpEAC="parse error",_tag_dyneither(_tmpEAC,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 291 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6696,x + yyn)]== x){
# 453
{const char*_tmpEAE;const char*_tmpEAD;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEAE=", expecting `",_tag_dyneither(_tmpEAE,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEAD=" or `",_tag_dyneither(_tmpEAD,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(291,x)]);
{const char*_tmpEAF;Cyc_strcat(msg,((_tmpEAF="'",_tag_dyneither(_tmpEAF,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEB0;Cyc_yyerror(((_tmpEB0="parse error",_tag_dyneither(_tmpEB0,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp7FF=1;_npop_handler(0);return _tmp7FF;}
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
 if(yyssp_offset == 0){int _tmp800=1;_npop_handler(0);return _tmp800;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6695) || Cyc_yycheck[_check_known_subscript_notnull(6696,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6696,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1031){
int _tmp801=0;_npop_handler(0);return _tmp801;}
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
union Cyc_YYSTYPE _tmp80B=v;int _tmp80C;char _tmp80D;struct _dyneither_ptr _tmp80E;union Cyc_Absyn_Nmspace _tmp80F;struct _dyneither_ptr*_tmp810;_LL44E: if((_tmp80B.Int_tok).tag != 1)goto _LL450;_tmp80C=((struct _tuple5)(_tmp80B.Int_tok).val).f2;_LL44F:
{const char*_tmpEB4;void*_tmpEB3[1];struct Cyc_Int_pa_PrintArg_struct _tmpEB2;(_tmpEB2.tag=1,((_tmpEB2.f1=(unsigned long)_tmp80C,((_tmpEB3[0]=& _tmpEB2,Cyc_fprintf(Cyc_stderr,((_tmpEB4="%d",_tag_dyneither(_tmpEB4,sizeof(char),3))),_tag_dyneither(_tmpEB3,sizeof(void*),1)))))));}goto _LL44D;_LL450: if((_tmp80B.Char_tok).tag != 2)goto _LL452;_tmp80D=(char)(_tmp80B.Char_tok).val;_LL451:
{const char*_tmpEB8;void*_tmpEB7[1];struct Cyc_Int_pa_PrintArg_struct _tmpEB6;(_tmpEB6.tag=1,((_tmpEB6.f1=(unsigned long)((int)_tmp80D),((_tmpEB7[0]=& _tmpEB6,Cyc_fprintf(Cyc_stderr,((_tmpEB8="%c",_tag_dyneither(_tmpEB8,sizeof(char),3))),_tag_dyneither(_tmpEB7,sizeof(void*),1)))))));}goto _LL44D;_LL452: if((_tmp80B.String_tok).tag != 3)goto _LL454;_tmp80E=(struct _dyneither_ptr)(_tmp80B.String_tok).val;_LL453:
{const char*_tmpEBC;void*_tmpEBB[1];struct Cyc_String_pa_PrintArg_struct _tmpEBA;(_tmpEBA.tag=0,((_tmpEBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp80E),((_tmpEBB[0]=& _tmpEBA,Cyc_fprintf(Cyc_stderr,((_tmpEBC="\"%s\"",_tag_dyneither(_tmpEBC,sizeof(char),5))),_tag_dyneither(_tmpEBB,sizeof(void*),1)))))));}goto _LL44D;_LL454: if((_tmp80B.QualId_tok).tag != 5)goto _LL456;_tmp80F=((struct _tuple0*)(_tmp80B.QualId_tok).val)->f1;_tmp810=((struct _tuple0*)(_tmp80B.QualId_tok).val)->f2;_LL455: {
# 3150
struct Cyc_List_List*_tmp81A=0;
{union Cyc_Absyn_Nmspace _tmp81B=_tmp80F;struct Cyc_List_List*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_List_List*_tmp81E;_LL459: if((_tmp81B.Rel_n).tag != 1)goto _LL45B;_tmp81C=(struct Cyc_List_List*)(_tmp81B.Rel_n).val;_LL45A:
 _tmp81A=_tmp81C;goto _LL458;_LL45B: if((_tmp81B.Abs_n).tag != 2)goto _LL45D;_tmp81D=(struct Cyc_List_List*)(_tmp81B.Abs_n).val;_LL45C:
 _tmp81A=_tmp81D;goto _LL458;_LL45D: if((_tmp81B.C_n).tag != 3)goto _LL45F;_tmp81E=(struct Cyc_List_List*)(_tmp81B.C_n).val;_LL45E:
 _tmp81A=_tmp81E;goto _LL458;_LL45F: if((_tmp81B.Loc_n).tag != 4)goto _LL458;_LL460:
 goto _LL458;_LL458:;}
# 3157
for(0;_tmp81A != 0;_tmp81A=_tmp81A->tl){
const char*_tmpEC0;void*_tmpEBF[1];struct Cyc_String_pa_PrintArg_struct _tmpEBE;(_tmpEBE.tag=0,((_tmpEBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp81A->hd)),((_tmpEBF[0]=& _tmpEBE,Cyc_fprintf(Cyc_stderr,((_tmpEC0="%s::",_tag_dyneither(_tmpEC0,sizeof(char),5))),_tag_dyneither(_tmpEBF,sizeof(void*),1)))))));}
{const char*_tmpEC4;void*_tmpEC3[1];struct Cyc_String_pa_PrintArg_struct _tmpEC2;(_tmpEC2.tag=0,((_tmpEC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp810),((_tmpEC3[0]=& _tmpEC2,Cyc_fprintf(Cyc_stderr,((_tmpEC4="%s::",_tag_dyneither(_tmpEC4,sizeof(char),5))),_tag_dyneither(_tmpEC3,sizeof(void*),1)))))));}
goto _LL44D;}_LL456:;_LL457:
{const char*_tmpEC7;void*_tmpEC6;(_tmpEC6=0,Cyc_fprintf(Cyc_stderr,((_tmpEC7="?",_tag_dyneither(_tmpEC7,sizeof(char),2))),_tag_dyneither(_tmpEC6,sizeof(void*),0)));}goto _LL44D;_LL44D:;}
# 3166
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp827=_new_region("yyr");struct _RegionHandle*yyr=& _tmp827;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp828=Cyc_Parse_parse_result;_npop_handler(0);return _tmp828;};
# 3169
;_pop_region(yyr);};}
