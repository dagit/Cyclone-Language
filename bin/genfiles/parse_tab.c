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
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp85E;declarators=((_tmp85E=_region_malloc(r,sizeof(*_tmp85E)),((_tmp85E->tl=declarators,((_tmp85E->hd=_tmp2,_tmp85E))))));}{
struct Cyc_List_List*_tmp85F;exprs=((_tmp85F=_region_malloc(r,sizeof(*_tmp85F)),((_tmp85F->hd=_tmp3,((_tmp85F->tl=exprs,_tmp85F))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp862;struct Cyc_Position_Exit_exn_struct*_tmp861;(int)_throw((void*)((_tmp861=_cycalloc_atomic(sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=Cyc_Position_Exit,_tmp862)),_tmp861)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp867;void*_tmp866[2];struct Cyc_String_pa_PrintArg_struct _tmp865;struct Cyc_String_pa_PrintArg_struct _tmp864;(_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp866[0]=& _tmp865,((_tmp866[1]=& _tmp864,Cyc_fprintf(Cyc_stderr,((_tmp867="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp867,sizeof(char),46))),_tag_dyneither(_tmp866,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp86A;void*_tmp869;(_tmp869=0,Cyc_Tcutil_warn(loc,((_tmp86A="qualifier on type is ignored",_tag_dyneither(_tmp86A,sizeof(char),29))),_tag_dyneither(_tmp869,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp86D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86C;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86D.tag=1,((_tmp86D.f1=_tmp1A,_tmp86D)))),_tmp86C)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 226
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp86E;return(_tmp86E.f1=nullable,((_tmp86E.f2=bound,((_tmp86E.f3=zeroterm,((_tmp86E.f4=rgn,_tmp86E)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp87B;const char*_tmp87A;void*_tmp879[1];struct Cyc_Int_pa_PrintArg_struct _tmp878;struct _tuple0*_tmp877;return(_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877->f1=Cyc_Absyn_Rel_n(0),((_tmp877->f2=(
(_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B[0]=(struct _dyneither_ptr)((_tmp878.tag=1,((_tmp878.f1=(unsigned long)enum_counter ++,((_tmp879[0]=& _tmp878,Cyc_aprintf(((_tmp87A="__anonymous_enum_%d__",_tag_dyneither(_tmp87A,sizeof(char),22))),_tag_dyneither(_tmp879,sizeof(void*),1)))))))),_tmp87B)))),_tmp877)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp87C;Cyc_Parse_err(((_tmp87C="bad type params in struct field",_tag_dyneither(_tmp87C,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp87D;Cyc_Parse_err(((_tmp87D="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp87D,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp87E;return(_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E->name=(*_tmp26).f2,((_tmp87E->tq=_tmp27,((_tmp87E->type=_tmp28,((_tmp87E->width=_tmp2B,((_tmp87E->attributes=_tmp2A,((_tmp87E->requires_clause=_tmp2C,_tmp87E)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp87F;return(_tmp87F.Signed_spec=0,((_tmp87F.Unsigned_spec=0,((_tmp87F.Short_spec=0,((_tmp87F.Long_spec=0,((_tmp87F.Long_Long_spec=0,((_tmp87F.Valid_type_spec=0,((_tmp87F.Type_spec=Cyc_Absyn_sint_typ,((_tmp87F.loc=loc,_tmp87F)))))))))))))));}
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
struct Cyc_String_pa_PrintArg_struct _tmp887;void*_tmp886[1];const char*_tmp885;struct _dyneither_ptr*_tmp884;struct _dyneither_ptr*nm=(_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=(struct _dyneither_ptr)((_tmp887.tag=0,((_tmp887.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp886[0]=& _tmp887,Cyc_aprintf(((_tmp885="`%s",_tag_dyneither(_tmp885,sizeof(char),4))),_tag_dyneither(_tmp886,sizeof(void*),1)))))))),_tmp884)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp896;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp895;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp894;struct Cyc_Absyn_Tvar*_tmp893;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp892;*_tmp42=(void*)((_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892[0]=((_tmp896.tag=2,((_tmp896.f1=((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893->name=nm,((_tmp893->identity=- 1,((_tmp893->kind=(void*)((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp894.tag=0,((_tmp894.f1=& Cyc_Tcutil_ik,_tmp894)))),_tmp895)))),_tmp893)))))))),_tmp896)))),_tmp892))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp899;struct Cyc_List_List*_tmp898;_tmp36=((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898->hd=((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899->f1=_tmp39,((_tmp899->f2=_tmp3B,_tmp899)))))),((_tmp898->tl=_tmp36,_tmp898))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp8A1;void*_tmp8A0[1];const char*_tmp89F;struct _dyneither_ptr*_tmp89E;struct _dyneither_ptr*nm=(_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=(struct _dyneither_ptr)((_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8A0[0]=& _tmp8A1,Cyc_aprintf(((_tmp89F="`%s",_tag_dyneither(_tmp89F,sizeof(char),4))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))))),_tmp89E)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8B0;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8AF;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8AE;struct Cyc_Absyn_Tvar*_tmp8AD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8AC;*_tmp3F=(void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8B0.tag=2,((_tmp8B0.f1=((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD->name=nm,((_tmp8AD->identity=- 1,((_tmp8AD->kind=(void*)((_tmp8AF=_cycalloc(sizeof(*_tmp8AF)),((_tmp8AF[0]=((_tmp8AE.tag=0,((_tmp8AE.f1=& Cyc_Tcutil_rk,_tmp8AE)))),_tmp8AF)))),_tmp8AD)))))))),_tmp8B0)))),_tmp8AC))));}
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
{struct _tuple18*_tmp8B3;struct Cyc_List_List*_tmp8B2;_tmp57=((_tmp8B2=_cycalloc(sizeof(*_tmp8B2)),((_tmp8B2->hd=((_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8B3->f2=_tmp5B,_tmp8B3)))))),((_tmp8B2->tl=_tmp57,_tmp8B2))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8B6;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8B5;return Cyc_Absyn_new_exp((void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B6.tag=38,((_tmp8B6.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8B6)))),_tmp8B5)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8BC;struct Cyc_Absyn_ArrayInfo _tmp8BB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8BA;return(void*)((_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA[0]=((_tmp8BC.tag=8,((_tmp8BC.f1=((_tmp8BB.elt_type=_tmp78,((_tmp8BB.tq=_tmp6C,((_tmp8BB.num_elts=nelts2,((_tmp8BB.zero_term=_tmp6E,((_tmp8BB.zt_loc=_tmp6F,_tmp8BB)))))))))),_tmp8BC)))),_tmp8BA))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 384
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8C2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8C1;union Cyc_Absyn_Constraint*_tmp8C0;_tmp7D=((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),(((_tmp8C0->Eq_constr).val=(void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C1.tag=1,((_tmp8C1.f1=_tmp81,_tmp8C1)))),_tmp8C2)))),(((_tmp8C0->Eq_constr).tag=1,_tmp8C0))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8C8;struct Cyc_Absyn_PtrInfo _tmp8C7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C6;return(void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C8.tag=5,((_tmp8C8.f1=((_tmp8C7.elt_typ=_tmp7C,((_tmp8C7.elt_tq=_tmp72,((_tmp8C7.ptr_atts=(((_tmp8C7.ptr_atts).rgn=_tmp73,(((_tmp8C7.ptr_atts).nullable=_tmp74,(((_tmp8C7.ptr_atts).bounds=_tmp7D,(((_tmp8C7.ptr_atts).zero_term=_tmp76,(((_tmp8C7.ptr_atts).ptrloc=_tmp77,_tmp8C7.ptr_atts)))))))))),_tmp8C7)))))),_tmp8C8)))),_tmp8C6))));}
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
struct _tuple19*_tmp8C9;return(_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9->f1=(*t).f2,((_tmp8C9->f2=(*t).f3,_tmp8C9)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8CC;void*_tmp8CB;(_tmp8CB=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8CC="bad declaration in `forarray' statement",_tag_dyneither(_tmp8CC,sizeof(char),40))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8CD;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CD="`H",_tag_dyneither(_tmp8CD,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8CE;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CE="`U",_tag_dyneither(_tmp8CE,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8CF;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CF="`RC",_tag_dyneither(_tmp8CF,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D9;struct _dyneither_ptr*_tmp8D8;struct Cyc_Absyn_Tvar*_tmp8D7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D6;return(void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D9.tag=2,((_tmp8D9.f1=((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7->name=((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=s,_tmp8D8)))),((_tmp8D7->identity=- 1,((_tmp8D7->kind=k,_tmp8D7)))))))),_tmp8D9)))),_tmp8D6))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8DC;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8DB;k=(void*)((_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DC.tag=1,((_tmp8DC.f1=0,_tmp8DC)))),_tmp8DB))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8DF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8DE;k=(void*)((_tmp8DE=_cycalloc(sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=2,((_tmp8DF.f1=0,((_tmp8DF.f2=_tmp9C,_tmp8DF)))))),_tmp8DE))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8E0;return(_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->name=t->name,((_tmp8E0->identity=- 1,((_tmp8E0->kind=k,_tmp8E0)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8E3="expecting a list of type variables, not types",_tag_dyneither(_tmp8E3,sizeof(char),46))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E6;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E5;return(void*)((_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5[0]=((_tmp8E6.tag=2,((_tmp8E6.f1=pr,_tmp8E6)))),_tmp8E5))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 468
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8E9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E8;*_tmpAD=(void*)((_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8E9.tag=2,((_tmp8E9.f1=0,((_tmp8E9.f2=k,_tmp8E9)))))),_tmp8E8))));}
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
{const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_warn(loc,((_tmp8EC="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8EC,sizeof(char),93))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8EF="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8EF,sizeof(char),73))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}{
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
const char*_tmp8F2;void*_tmp8F1;(_tmp8F1=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F2="initializer found in parameter declaration",_tag_dyneither(_tmp8F2,sizeof(char),43))),_tag_dyneither(_tmp8F1,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F5="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8F5,sizeof(char),47))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}
{struct _tuple8*_tmp8F8;struct Cyc_List_List*_tmp8F7;rev_new_params=(
(_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7->hd=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->f1=(*_tmpC4->name).f2,((_tmp8F8->f2=_tmpC4->tq,((_tmp8F8->f3=_tmpC4->type,_tmp8F8)))))))),((_tmp8F7->tl=rev_new_params,_tmp8F7))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8FB="nonvariable declaration in parameter type",_tag_dyneither(_tmp8FB,sizeof(char),42))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpBF == 0){
const char*_tmp8FF;void*_tmp8FE[1];struct Cyc_String_pa_PrintArg_struct _tmp8FD;(_tmp8FD.tag=0,((_tmp8FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp8FE[0]=& _tmp8FD,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8FF="%s is not given a type",_tag_dyneither(_tmp8FF,sizeof(char),23))),_tag_dyneither(_tmp8FE,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp90E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp90D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp90C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp90B;struct Cyc_List_List*_tmp90A;return
(_tmp90A=_region_malloc(yy,sizeof(*_tmp90A)),((_tmp90A->hd=(void*)((_tmp90E=_region_malloc(yy,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90B.tag=3,((_tmp90B.f1=(void*)((_tmp90D=_region_malloc(yy,sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90C.tag=1,((_tmp90C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp90C.f2=0,((_tmp90C.f3=0,((_tmp90C.f4=0,((_tmp90C.f5=0,_tmp90C)))))))))))),_tmp90D)))),_tmp90B)))),_tmp90E)))),((_tmp90A->tl=0,_tmp90A)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp90F;return(_tmp90F=_region_malloc(yy,sizeof(*_tmp90F)),((_tmp90F->hd=(void*)tms->hd,((_tmp90F->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp90F)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp910;d=((_tmp910.id=d.id,((_tmp910.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp910))));}{
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
{const char*_tmp911;Cyc_Parse_err(((_tmp911="bad storage class on function",_tag_dyneither(_tmp911,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 578
if(_tmpDE != 0){
# 581
const char*_tmp914;void*_tmp913;(_tmp913=0,Cyc_Tcutil_warn(loc,((_tmp914="bad type params, ignoring",_tag_dyneither(_tmp914,sizeof(char),26))),_tag_dyneither(_tmp913,sizeof(void*),0)));}{
# 583
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpED=0;
{struct Cyc_List_List*_tmpEE=_tmpE8;for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)_tmpEE->hd;struct _dyneither_ptr*_tmpF1;struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct _tuple8*_tmpF0=_tmpEF;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;
if(_tmpF1 == 0){
{const char*_tmp915;Cyc_Parse_err(((_tmp915="missing argument variable in function prototype",_tag_dyneither(_tmp915,sizeof(char),48))),loc);}{
struct _tuple8*_tmp918;struct Cyc_List_List*_tmp917;_tmpED=((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917->hd=((_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp918->f2=_tmpF2,((_tmp918->f3=_tmpF3,_tmp918)))))))),((_tmp917->tl=_tmpED,_tmp917))))));};}else{
# 593
struct _tuple8*_tmp91B;struct Cyc_List_List*_tmp91A;_tmpED=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->hd=((_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B->f1=_tmpF1,((_tmp91B->f2=_tmpF2,((_tmp91B->f3=_tmpF3,_tmp91B)))))))),((_tmp91A->tl=_tmpED,_tmp91A))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp91C;return(_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C->sc=sc,((_tmp91C->name=d.id,((_tmp91C->tvs=_tmpE4,((_tmp91C->is_inline=is_inline,((_tmp91C->effect=_tmpE5,((_tmp91C->ret_tqual=_tmpE6,((_tmp91C->ret_type=_tmpE7,((_tmp91C->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpED),((_tmp91C->c_varargs=_tmpE9,((_tmp91C->cyc_varargs=_tmpEA,((_tmp91C->rgn_po=_tmpEB,((_tmp91C->body=body,((_tmp91C->cached_typ=0,((_tmp91C->param_vardecls=0,((_tmp91C->fn_vardecl=0,((_tmp91C->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),_tmp91C)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp91F;void*_tmp91E;(_tmp91E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91F="declarator is not a function prototype",_tag_dyneither(_tmp91F,sizeof(char),39))),_tag_dyneither(_tmp91E,sizeof(void*),0)));}_LL66:;};};};}static char _tmpFD[52]="at most one type may appear within a type specifier";
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
void*_tmp920;(_tmp920=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp920,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp921;(_tmp921=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp921,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp922;(_tmp922=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp922,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp923;(_tmp923=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp923,sizeof(void*),0)));}
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
const char*_tmp926;void*_tmp925;(_tmp925=0,Cyc_Tcutil_warn(loc,((_tmp926="missing type within specifier",_tag_dyneither(_tmp926,sizeof(char),30))),_tag_dyneither(_tmp925,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmp107=t;enum Cyc_Absyn_Sign _tmp109;enum Cyc_Absyn_Size_of _tmp10A;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp107;if(_tmp108->tag != 6)goto _LL6E;else{_tmp109=_tmp108->f1;_tmp10A=_tmp108->f2;}}_LL6D:
# 686
 if(_tmp109 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10A);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp927;Cyc_Parse_err(((_tmp927="sign specification on non-integral type",_tag_dyneither(_tmp927,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmp10C=t;enum Cyc_Absyn_Sign _tmp10E;enum Cyc_Absyn_Size_of _tmp10F;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10C;if(_tmp10D->tag != 6)goto _LL73;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;}}_LL72:
# 694
 if(_tmp10F != sz)
t=Cyc_Absyn_int_typ(_tmp10E,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp110=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp10C;if(_tmp110->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp928;Cyc_Parse_err(((_tmp928="size qualifier on non-integral type",_tag_dyneither(_tmp928,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp92B;struct Cyc_List_List*_tmp92A;return(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->hd=((_tmp92B=_region_malloc(r,sizeof(*_tmp92B)),((_tmp92B->f1=_tmp112,((_tmp92B->f2=_tmp115,((_tmp92B->f3=_tmp116,((_tmp92B->f4=_tmp117,((_tmp92B->f5=_tmp118,_tmp92B)))))))))))),((_tmp92A->tl=0,_tmp92A)))));}else{
# 718
struct _tuple15*_tmp92E;struct Cyc_List_List*_tmp92D;return(_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D->hd=((_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E->f1=_tmp112,((_tmp92E->f2=_tmp115,((_tmp92E->f3=_tmp116,((_tmp92E->f4=_tmp117,((_tmp92E->f5=_tmp118,_tmp92E)))))))))))),((_tmp92D->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp92D)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp92F;return(_tmp92F.f1=tq,((_tmp92F.f2=t,((_tmp92F.f3=0,((_tmp92F.f4=atts,_tmp92F)))))));}{
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
struct Cyc_List_List*_tmp930;fn_atts=((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930->hd=(void*)as->hd,((_tmp930->tl=fn_atts,_tmp930))))));}else{
# 743
struct Cyc_List_List*_tmp931;new_atts=((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->hd=(void*)as->hd,((_tmp931->tl=new_atts,_tmp931))))));}}}
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
const char*_tmp934;void*_tmp933;(_tmp933=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp13A,((_tmp934="function declaration without parameter types",_tag_dyneither(_tmp934,sizeof(char),45))),_tag_dyneither(_tmp933,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp129=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp129->tag != 4)goto _LL80;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp935;return(_tmp935.f1=tq,((_tmp935.f2=t,((_tmp935.f3=_tmp12A,((_tmp935.f4=atts,_tmp935)))))));}{
# 802
const char*_tmp938;void*_tmp937;(_tmp937=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12B,((_tmp938="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp938,sizeof(char),68))),_tag_dyneither(_tmp937,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp12C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12C->tag != 2)goto _LL82;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp93E;struct Cyc_Absyn_PtrInfo _tmp93D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp93C;return Cyc_Parse_apply_tms(_tmp12E,(void*)((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C[0]=((_tmp93E.tag=5,((_tmp93E.f1=((_tmp93D.elt_typ=t,((_tmp93D.elt_tq=tq,((_tmp93D.ptr_atts=_tmp12D,_tmp93D)))))),_tmp93E)))),_tmp93C)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp12F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12F->tag != 5)goto _LL77;else{_tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp944;struct Cyc_Absyn_Typedefdecl*_tmp943;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp942;return Cyc_Absyn_new_decl((void*)((_tmp942=_cycalloc(sizeof(*_tmp942)),((_tmp942[0]=((_tmp944.tag=9,((_tmp944.f1=((_tmp943=_cycalloc(sizeof(*_tmp943)),((_tmp943->name=_tmp151,((_tmp943->tvs=_tmp154,((_tmp943->kind=kind,((_tmp943->defn=type,((_tmp943->atts=_tmp155,((_tmp943->tq=_tmp152,_tmp943)))))))))))))),_tmp944)))),_tmp942)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp947;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp946;return Cyc_Absyn_new_stmt((void*)((_tmp946=_cycalloc(sizeof(*_tmp946)),((_tmp946[0]=((_tmp947.tag=12,((_tmp947.f1=d,((_tmp947.f2=s,_tmp947)))))),_tmp946)))),d->loc);}
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
const char*_tmp948;Cyc_Parse_err(((_tmp948="inline is allowed only on function definitions",_tag_dyneither(_tmp948,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp94E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_List_List*_tmp181=(_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C->hd=Cyc_Absyn_new_decl((void*)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp94D.tag=6,((_tmp94D.f1=_tmp169,_tmp94D)))),_tmp94E)))),loc),((_tmp94C->tl=0,_tmp94C)))));_npop_handler(0);return _tmp181;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16A->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp16B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp16A->f1)->r;if(_tmp16B->tag != 1)goto _LLA0;else{_tmp16C=_tmp16B->f1;}}}_LL9F:
# 910
 if(_tmp162 != 0){const char*_tmp94F;Cyc_Parse_err(((_tmp94F="attributes on enum not supported",_tag_dyneither(_tmp94F,sizeof(char),33))),loc);}
_tmp16C->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp955;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp954;struct Cyc_List_List*_tmp953;struct Cyc_List_List*_tmp186=(_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953->hd=Cyc_Absyn_new_decl((void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=((_tmp954.tag=8,((_tmp954.f1=_tmp16C,_tmp954)))),_tmp955)))),loc),((_tmp953->tl=0,_tmp953)))));_npop_handler(0);return _tmp186;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16D->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 2)goto _LLA2;else{_tmp16F=_tmp16E->f1;}}}_LLA1:
# 914
 if(_tmp162 != 0){const char*_tmp956;Cyc_Parse_err(((_tmp956="attributes on datatypes not supported",_tag_dyneither(_tmp956,sizeof(char),38))),loc);}
_tmp16F->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp95C;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp95B;struct Cyc_List_List*_tmp95A;struct Cyc_List_List*_tmp18B=(_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A->hd=Cyc_Absyn_new_decl((void*)((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95B.tag=7,((_tmp95B.f1=_tmp16F,_tmp95B)))),_tmp95C)))),loc),((_tmp95A->tl=0,_tmp95A)))));_npop_handler(0);return _tmp18B;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166;if(_tmp170->tag != 11)goto _LLA4;else{if((((_tmp170->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp171=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f1;_tmp172=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f2;_tmp173=(_tmp170->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp173);
struct Cyc_Absyn_Aggrdecl*_tmp95D;struct Cyc_Absyn_Aggrdecl*_tmp18D=(_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D->kind=_tmp171,((_tmp95D->sc=s,((_tmp95D->name=_tmp172,((_tmp95D->tvs=_tmp18C,((_tmp95D->impl=0,((_tmp95D->attributes=0,_tmp95D)))))))))))));
if(_tmp162 != 0){const char*_tmp95E;Cyc_Parse_err(((_tmp95E="bad attributes on type declaration",_tag_dyneither(_tmp95E,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp964;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp963;struct Cyc_List_List*_tmp962;struct Cyc_List_List*_tmp192=(_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962->hd=Cyc_Absyn_new_decl((void*)((_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964[0]=((_tmp963.tag=6,((_tmp963.f1=_tmp18D,_tmp963)))),_tmp964)))),loc),((_tmp962->tl=0,_tmp962)))));_npop_handler(0);return _tmp192;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp174->tag != 3)goto _LLA6;else{if((((_tmp174->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp175=(struct Cyc_Absyn_Datatypedecl**)(((_tmp174->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp162 != 0){const char*_tmp965;Cyc_Parse_err(((_tmp965="bad attributes on datatype",_tag_dyneither(_tmp965,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp96B;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp96A;struct Cyc_List_List*_tmp969;struct Cyc_List_List*_tmp198=(_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->hd=Cyc_Absyn_new_decl((void*)((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96A.tag=7,((_tmp96A.f1=*_tmp175,_tmp96A)))),_tmp96B)))),loc),((_tmp969->tl=0,_tmp969)))));_npop_handler(0);return _tmp198;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp176->tag != 3)goto _LLA8;else{if((((_tmp176->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp177=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).name;_tmp178=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp179=(_tmp176->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Decl*_tmp19A=Cyc_Absyn_datatype_decl(s,_tmp177,_tmp199,0,_tmp178,loc);
if(_tmp162 != 0){const char*_tmp96C;Cyc_Parse_err(((_tmp96C="bad attributes on datatype",_tag_dyneither(_tmp96C,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp96D;struct Cyc_List_List*_tmp19D=(_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D->hd=_tmp19A,((_tmp96D->tl=0,_tmp96D)))));_npop_handler(0);return _tmp19D;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp166;if(_tmp17A->tag != 13)goto _LLAA;else{_tmp17B=_tmp17A->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp96E;struct Cyc_Absyn_Enumdecl*_tmp19E=(_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E->sc=s,((_tmp96E->name=_tmp17B,((_tmp96E->fields=0,_tmp96E)))))));
if(_tmp162 != 0){const char*_tmp96F;Cyc_Parse_err(((_tmp96F="bad attributes on enum",_tag_dyneither(_tmp96F,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp979;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp978;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp977;struct Cyc_List_List*_tmp976;struct Cyc_List_List*_tmp1A4=(_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->hd=((_tmp979=_cycalloc(sizeof(*_tmp979)),((_tmp979->r=(void*)((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp978.tag=8,((_tmp978.f1=_tmp19E,_tmp978)))),_tmp977)))),((_tmp979->loc=loc,_tmp979)))))),((_tmp976->tl=0,_tmp976)))));_npop_handler(0);return _tmp1A4;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp166;if(_tmp17C->tag != 14)goto _LLAC;else{_tmp17D=_tmp17C->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp97C;struct Cyc_Absyn_Enumdecl*_tmp97B;struct Cyc_Absyn_Enumdecl*_tmp1A6=(_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B->sc=s,((_tmp97B->name=Cyc_Parse_gensym_enum(),((_tmp97B->fields=((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->v=_tmp17D,_tmp97C)))),_tmp97B)))))));
if(_tmp162 != 0){const char*_tmp97D;Cyc_Parse_err(((_tmp97D="bad attributes on enum",_tag_dyneither(_tmp97D,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp987;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp986;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp985;struct Cyc_List_List*_tmp984;struct Cyc_List_List*_tmp1AC=(_tmp984=_cycalloc(sizeof(*_tmp984)),((_tmp984->hd=((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->r=(void*)((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=8,((_tmp986.f1=_tmp1A6,_tmp986)))),_tmp985)))),((_tmp987->loc=loc,_tmp987)))))),((_tmp984->tl=0,_tmp984)))));_npop_handler(0);return _tmp1AC;};}_LLAC:;_LLAD:
{const char*_tmp988;Cyc_Parse_err(((_tmp988="missing declarator",_tag_dyneither(_tmp988,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B0=0;_npop_handler(0);return _tmp1B0;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp1B1=Cyc_Parse_apply_tmss(mkrgn,_tmp160,_tmp165,declarators,_tmp162);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp989;Cyc_Parse_err(((_tmp989="initializer in typedef declaration",_tag_dyneither(_tmp989,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B1);
struct Cyc_List_List*_tmp1B3=decls;_npop_handler(0);return _tmp1B3;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1B4=_tmp1B1;for(0;_tmp1B4 != 0;(_tmp1B4=_tmp1B4->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1B5=(struct _tuple15*)_tmp1B4->hd;struct _tuple0*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct _tuple15*_tmp1B6=_tmp1B5;_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;_tmp1B9=_tmp1B6->f3;_tmp1BA=_tmp1B6->f4;_tmp1BB=_tmp1B6->f5;
if(_tmp1BA != 0){
const char*_tmp98C;void*_tmp98B;(_tmp98B=0,Cyc_Tcutil_warn(loc,((_tmp98C="bad type params, ignoring",_tag_dyneither(_tmp98C,sizeof(char),26))),_tag_dyneither(_tmp98B,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp98F;void*_tmp98E;(_tmp98E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp98F="unexpected NULL in parse!",_tag_dyneither(_tmp98F,sizeof(char),26))),_tag_dyneither(_tmp98E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C1=Cyc_Absyn_new_vardecl(_tmp1B7,_tmp1B9,_tmp1C0);
_tmp1C1->tq=_tmp1B8;
_tmp1C1->sc=s;
_tmp1C1->attributes=_tmp1BB;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp995;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp994;struct Cyc_Absyn_Decl*_tmp993;struct Cyc_Absyn_Decl*_tmp1C2=(_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993->r=(void*)((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995[0]=((_tmp994.tag=0,((_tmp994.f1=_tmp1C1,_tmp994)))),_tmp995)))),((_tmp993->loc=loc,_tmp993)))));
struct Cyc_List_List*_tmp996;decls=((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996->hd=_tmp1C2,((_tmp996->tl=decls,_tmp996))))));};};}}{
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
{const char*_tmp99B;void*_tmp99A[2];struct Cyc_String_pa_PrintArg_struct _tmp999;struct Cyc_Int_pa_PrintArg_struct _tmp998;Cyc_Parse_err((struct _dyneither_ptr)((_tmp998.tag=1,((_tmp998.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp999.tag=0,((_tmp999.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp99A[0]=& _tmp999,((_tmp99A[1]=& _tmp998,Cyc_aprintf(((_tmp99B="bad kind: %s; strlen=%d",_tag_dyneither(_tmp99B,sizeof(char),24))),_tag_dyneither(_tmp99A,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1CF=p->r;void*_tmp1D0=_tmp1CF;struct _tuple0*_tmp1D2;struct Cyc_Absyn_Vardecl*_tmp1D4;struct Cyc_Absyn_Pat*_tmp1D7;enum Cyc_Absyn_Sign _tmp1DA;int _tmp1DB;char _tmp1DD;struct _dyneither_ptr _tmp1DF;int _tmp1E0;struct _tuple0*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E5;_LLC5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D1->tag != 14)goto _LLC7;else{_tmp1D2=_tmp1D1->f1;}}_LLC6:
 return Cyc_Absyn_unknownid_exp(_tmp1D2,p->loc);_LLC7: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D3->tag != 2)goto _LLC9;else{_tmp1D4=_tmp1D3->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1D3->f2)->r;if(_tmp1D5->tag != 0)goto _LLC9;};}}_LLC8:
# 1015
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1D4->name,p->loc),p->loc);_LLC9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D6->tag != 5)goto _LLCB;else{_tmp1D7=_tmp1D6->f1;}}_LLCA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1D7),p->loc);_LLCB: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D8->tag != 8)goto _LLCD;}_LLCC:
 return Cyc_Absyn_null_exp(p->loc);_LLCD: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D9->tag != 9)goto _LLCF;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LLCE:
 return Cyc_Absyn_int_exp(_tmp1DA,_tmp1DB,p->loc);_LLCF: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1DC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DC->tag != 10)goto _LLD1;else{_tmp1DD=_tmp1DC->f1;}}_LLD0:
 return Cyc_Absyn_char_exp(_tmp1DD,p->loc);_LLD1: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1DE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DE->tag != 11)goto _LLD3;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f2;}}_LLD2:
 return Cyc_Absyn_float_exp(_tmp1DF,_tmp1E0,p->loc);_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E1=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E1->tag != 15)goto _LLD5;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;if(_tmp1E1->f3 != 0)goto _LLD5;}}_LLD4: {
# 1022
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1E2,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1E3);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLD5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1E4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E4->tag != 16)goto _LLD7;else{_tmp1E5=_tmp1E4->f1;}}_LLD6:
 return _tmp1E5;_LLD7:;_LLD8:
# 1027
{const char*_tmp99C;Cyc_Parse_err(((_tmp99C="cannot mix patterns and expressions in case",_tag_dyneither(_tmp99C,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC4:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1E8[14]="$(sign_t,int)";
# 1079 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 14}};
# 1081
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1E9=yy1;struct _tuple5 _tmp1EA;_LLDA: if((_tmp1E9.Int_tok).tag != 1)goto _LLDC;_tmp1EA=(struct _tuple5)(_tmp1E9.Int_tok).val;_LLDB:
# 1085
 yyzzz=_tmp1EA;
goto _LLD9;_LLDC:;_LLDD:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD9:;}
# 1089
return yyzzz;}
# 1091
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.Int_tok).val=yy1,(((_tmp99D.Int_tok).tag=1,_tmp99D)));}static char _tmp1ED[5]="char";
# 1080 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 5}};
# 1082
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;char _tmp1EF;_LLDF: if((_tmp1EE.Char_tok).tag != 2)goto _LLE1;_tmp1EF=(char)(_tmp1EE.Char_tok).val;_LLE0:
# 1086
 yyzzz=_tmp1EF;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDE:;}
# 1090
return yyzzz;}
# 1092
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.Char_tok).val=yy1,(((_tmp99E.Char_tok).tag=2,_tmp99E)));}static char _tmp1F2[13]="string_t<`H>";
# 1081 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 13}};
# 1083
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct _dyneither_ptr _tmp1F4;_LLE4: if((_tmp1F3.String_tok).tag != 3)goto _LLE6;_tmp1F4=(struct _dyneither_ptr)(_tmp1F3.String_tok).val;_LLE5:
# 1087
 yyzzz=_tmp1F4;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE3:;}
# 1091
return yyzzz;}
# 1093
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.String_tok).val=yy1,(((_tmp99F.String_tok).tag=3,_tmp99F)));}static char _tmp1F7[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1084 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 56}};
# 1086
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1F8=yy1;struct _tuple21*_tmp1F9;_LLE9: if((_tmp1F8.YY1).tag != 7)goto _LLEB;_tmp1F9=(struct _tuple21*)(_tmp1F8.YY1).val;_LLEA:
# 1090
 yyzzz=_tmp1F9;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLE8:;}
# 1094
return yyzzz;}
# 1096
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY1).val=yy1,(((_tmp9A0.YY1).tag=7,_tmp9A0)));}static char _tmp1FC[19]="conref_t<bounds_t>";
# 1085 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 19}};
# 1087
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;union Cyc_Absyn_Constraint*_tmp1FE;_LLEE: if((_tmp1FD.YY2).tag != 8)goto _LLF0;_tmp1FE=(union Cyc_Absyn_Constraint*)(_tmp1FD.YY2).val;_LLEF:
# 1091
 yyzzz=_tmp1FE;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLED:;}
# 1095
return yyzzz;}
# 1097
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY2).val=yy1,(((_tmp9A1.YY2).tag=8,_tmp9A1)));}static char _tmp201[28]="list_t<offsetof_field_t,`H>";
# 1086 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 28}};
# 1088
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;_LLF3: if((_tmp202.YY3).tag != 9)goto _LLF5;_tmp203=(struct Cyc_List_List*)(_tmp202.YY3).val;_LLF4:
# 1092
 yyzzz=_tmp203;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF2:;}
# 1096
return yyzzz;}
# 1098
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY3).val=yy1,(((_tmp9A2.YY3).tag=9,_tmp9A2)));}static char _tmp206[6]="exp_t";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 6}};
# 1089
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_Absyn_Exp*_tmp208;_LLF8: if((_tmp207.YY4).tag != 10)goto _LLFA;_tmp208=(struct Cyc_Absyn_Exp*)(_tmp207.YY4).val;_LLF9:
# 1093
 yyzzz=_tmp208;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLF7:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY4).val=yy1,(((_tmp9A3.YY4).tag=10,_tmp9A3)));}static char _tmp20B[10]="exp_opt_t";
# 1095 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 10}};
# 1097
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Absyn_Exp*_tmp20D;_LLFD: if((_tmp20C.YY5).tag != 11)goto _LLFF;_tmp20D=(struct Cyc_Absyn_Exp*)(_tmp20C.YY5).val;_LLFE:
# 1101
 yyzzz=_tmp20D;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY5);_LLFC:;}
# 1105
return yyzzz;}
# 1107
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.YY5).val=yy1,(((_tmp9A4.YY5).tag=11,_tmp9A4)));}static char _tmp210[17]="list_t<exp_t,`H>";
# 1096 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 17}};
# 1098
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct Cyc_List_List*_tmp212;_LL102: if((_tmp211.YY6).tag != 12)goto _LL104;_tmp212=(struct Cyc_List_List*)(_tmp211.YY6).val;_LL103:
# 1102
 yyzzz=_tmp212;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL101:;}
# 1106
return yyzzz;}
# 1108
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY6).val=yy1,(((_tmp9A5.YY6).tag=12,_tmp9A5)));}static char _tmp215[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 47}};
# 1099
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;struct Cyc_List_List*_tmp217;_LL107: if((_tmp216.YY7).tag != 13)goto _LL109;_tmp217=(struct Cyc_List_List*)(_tmp216.YY7).val;_LL108:
# 1103
 yyzzz=_tmp217;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL106:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY7).val=yy1,(((_tmp9A6.YY7).tag=13,_tmp9A6)));}static char _tmp21A[9]="primop_t";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 9}};
# 1100
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;enum Cyc_Absyn_Primop _tmp21C;_LL10C: if((_tmp21B.YY8).tag != 14)goto _LL10E;_tmp21C=(enum Cyc_Absyn_Primop)(_tmp21B.YY8).val;_LL10D:
# 1104
 yyzzz=_tmp21C;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL10B:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.YY8).val=yy1,(((_tmp9A7.YY8).tag=14,_tmp9A7)));}static char _tmp21F[19]="opt_t<primop_t,`H>";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 19}};
# 1101
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_Core_Opt*_tmp221;_LL111: if((_tmp220.YY9).tag != 15)goto _LL113;_tmp221=(struct Cyc_Core_Opt*)(_tmp220.YY9).val;_LL112:
# 1105
 yyzzz=_tmp221;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL110:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.YY9).val=yy1,(((_tmp9A8.YY9).tag=15,_tmp9A8)));}static char _tmp224[7]="qvar_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 7}};
# 1102
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct _tuple0*_tmp226;_LL116: if((_tmp225.QualId_tok).tag != 5)goto _LL118;_tmp226=(struct _tuple0*)(_tmp225.QualId_tok).val;_LL117:
# 1106
 yyzzz=_tmp226;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.QualId_tok).val=yy1,(((_tmp9A9.QualId_tok).tag=5,_tmp9A9)));}static char _tmp229[7]="stmt_t";
# 1103 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 7}};
# 1105
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_Absyn_Stmt*_tmp22B;_LL11B: if((_tmp22A.YY10).tag != 16)goto _LL11D;_tmp22B=(struct Cyc_Absyn_Stmt*)(_tmp22A.YY10).val;_LL11C:
# 1109
 yyzzz=_tmp22B;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11A:;}
# 1113
return yyzzz;}
# 1115
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.YY10).val=yy1,(((_tmp9AA.YY10).tag=16,_tmp9AA)));}static char _tmp22E[27]="list_t<switch_clause_t,`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 27}};
# 1109
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_List_List*_tmp230;_LL120: if((_tmp22F.YY11).tag != 17)goto _LL122;_tmp230=(struct Cyc_List_List*)(_tmp22F.YY11).val;_LL121:
# 1113
 yyzzz=_tmp230;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL11F:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.YY11).val=yy1,(((_tmp9AB.YY11).tag=17,_tmp9AB)));}static char _tmp233[6]="pat_t";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 6}};
# 1110
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Absyn_Pat*_tmp235;_LL125: if((_tmp234.YY12).tag != 18)goto _LL127;_tmp235=(struct Cyc_Absyn_Pat*)(_tmp234.YY12).val;_LL126:
# 1114
 yyzzz=_tmp235;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL124:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY12).val=yy1,(((_tmp9AC.YY12).tag=18,_tmp9AC)));}static char _tmp238[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 28}};
# 1115
struct _tuple22*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple22*_tmp23A;_LL12A: if((_tmp239.YY13).tag != 19)goto _LL12C;_tmp23A=(struct _tuple22*)(_tmp239.YY13).val;_LL12B:
# 1119
 yyzzz=_tmp23A;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL129:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY13).val=yy1,(((_tmp9AD.YY13).tag=19,_tmp9AD)));}static char _tmp23D[17]="list_t<pat_t,`H>";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 17}};
# 1116
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_List_List*_tmp23F;_LL12F: if((_tmp23E.YY14).tag != 20)goto _LL131;_tmp23F=(struct Cyc_List_List*)(_tmp23E.YY14).val;_LL130:
# 1120
 yyzzz=_tmp23F;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL12E:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY14).val=yy1,(((_tmp9AE.YY14).tag=20,_tmp9AE)));}static char _tmp242[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 36}};
# 1117
struct _tuple23*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;struct _tuple23*_tmp244;_LL134: if((_tmp243.YY15).tag != 21)goto _LL136;_tmp244=(struct _tuple23*)(_tmp243.YY15).val;_LL135:
# 1121
 yyzzz=_tmp244;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL133:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY15).val=yy1,(((_tmp9AF.YY15).tag=21,_tmp9AF)));}static char _tmp247[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 47}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*_tmp249;_LL139: if((_tmp248.YY16).tag != 22)goto _LL13B;_tmp249=(struct Cyc_List_List*)(_tmp248.YY16).val;_LL13A:
# 1122
 yyzzz=_tmp249;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL138:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY16).val=yy1,(((_tmp9B0.YY16).tag=22,_tmp9B0)));}static char _tmp24C[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 58}};
# 1119
struct _tuple22*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL13E: if((_tmp24D.YY17).tag != 23)goto _LL140;_tmp24E=(struct _tuple22*)(_tmp24D.YY17).val;_LL13F:
# 1123
 yyzzz=_tmp24E;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL13D:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY17).val=yy1,(((_tmp9B1.YY17).tag=23,_tmp9B1)));}static char _tmp251[9]="fndecl_t";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 9}};
# 1120
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_Absyn_Fndecl*_tmp253;_LL143: if((_tmp252.YY18).tag != 24)goto _LL145;_tmp253=(struct Cyc_Absyn_Fndecl*)(_tmp252.YY18).val;_LL144:
# 1124
 yyzzz=_tmp253;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL142:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY18).val=yy1,(((_tmp9B2.YY18).tag=24,_tmp9B2)));}static char _tmp256[18]="list_t<decl_t,`H>";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 18}};
# 1121
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct Cyc_List_List*_tmp258;_LL148: if((_tmp257.YY19).tag != 25)goto _LL14A;_tmp258=(struct Cyc_List_List*)(_tmp257.YY19).val;_LL149:
# 1125
 yyzzz=_tmp258;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL147:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY19).val=yy1,(((_tmp9B3.YY19).tag=25,_tmp9B3)));}static char _tmp25B[12]="decl_spec_t";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 12}};
# 1123
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_Parse_Declaration_spec _tmp25D;_LL14D: if((_tmp25C.YY20).tag != 26)goto _LL14F;_tmp25D=(struct Cyc_Parse_Declaration_spec)(_tmp25C.YY20).val;_LL14E:
# 1127
 yyzzz=_tmp25D;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL14C:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY20).val=yy1,(((_tmp9B4.YY20).tag=26,_tmp9B4)));}static char _tmp260[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 31}};
# 1124
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple11 _tmp262;_LL152: if((_tmp261.YY21).tag != 27)goto _LL154;_tmp262=(struct _tuple11)(_tmp261.YY21).val;_LL153:
# 1128
 yyzzz=_tmp262;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL151:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY21).val=yy1,(((_tmp9B5.YY21).tag=27,_tmp9B5)));}static char _tmp265[23]="declarator_list_t<`yy>";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 23}};
# 1125
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct _tuple12*_tmp267;_LL157: if((_tmp266.YY22).tag != 28)goto _LL159;_tmp267=(struct _tuple12*)(_tmp266.YY22).val;_LL158:
# 1129
 yyzzz=_tmp267;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL156:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY22).val=yy1,(((_tmp9B6.YY22).tag=28,_tmp9B6)));}static char _tmp26A[19]="storage_class_t@`H";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 19}};
# 1126
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;enum Cyc_Parse_Storage_class*_tmp26C;_LL15C: if((_tmp26B.YY23).tag != 29)goto _LL15E;_tmp26C=(enum Cyc_Parse_Storage_class*)(_tmp26B.YY23).val;_LL15D:
# 1130
 yyzzz=_tmp26C;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL15B:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY23).val=yy1,(((_tmp9B7.YY23).tag=29,_tmp9B7)));}static char _tmp26F[17]="type_specifier_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 17}};
# 1127
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_Parse_Type_specifier _tmp271;_LL161: if((_tmp270.YY24).tag != 30)goto _LL163;_tmp271=(struct Cyc_Parse_Type_specifier)(_tmp270.YY24).val;_LL162:
# 1131
 yyzzz=_tmp271;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL160:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY24).val=yy1,(((_tmp9B8.YY24).tag=30,_tmp9B8)));}static char _tmp274[12]="aggr_kind_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 12}};
# 1129
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;enum Cyc_Absyn_AggrKind _tmp276;_LL166: if((_tmp275.YY25).tag != 31)goto _LL168;_tmp276=(enum Cyc_Absyn_AggrKind)(_tmp275.YY25).val;_LL167:
# 1133
 yyzzz=_tmp276;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL165:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY25).val=yy1,(((_tmp9B9.YY25).tag=31,_tmp9B9)));}static char _tmp279[8]="tqual_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 8}};
# 1130
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;struct Cyc_Absyn_Tqual _tmp27B;_LL16B: if((_tmp27A.YY26).tag != 32)goto _LL16D;_tmp27B=(struct Cyc_Absyn_Tqual)(_tmp27A.YY26).val;_LL16C:
# 1134
 yyzzz=_tmp27B;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16A:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY26).val=yy1,(((_tmp9BA.YY26).tag=32,_tmp9BA)));}static char _tmp27E[23]="list_t<aggrfield_t,`H>";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 23}};
# 1131
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;struct Cyc_List_List*_tmp280;_LL170: if((_tmp27F.YY27).tag != 33)goto _LL172;_tmp280=(struct Cyc_List_List*)(_tmp27F.YY27).val;_LL171:
# 1135
 yyzzz=_tmp280;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL16F:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY27).val=yy1,(((_tmp9BB.YY27).tag=33,_tmp9BB)));}static char _tmp283[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 34}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_List_List*_tmp285;_LL175: if((_tmp284.YY28).tag != 34)goto _LL177;_tmp285=(struct Cyc_List_List*)(_tmp284.YY28).val;_LL176:
# 1136
 yyzzz=_tmp285;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL174:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY28).val=yy1,(((_tmp9BC.YY28).tag=34,_tmp9BC)));}static char _tmp288[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 33}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;struct Cyc_List_List*_tmp28A;_LL17A: if((_tmp289.YY29).tag != 35)goto _LL17C;_tmp28A=(struct Cyc_List_List*)(_tmp289.YY29).val;_LL17B:
# 1137
 yyzzz=_tmp28A;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL179:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY29).val=yy1,(((_tmp9BD.YY29).tag=35,_tmp9BD)));}static char _tmp28D[18]="declarator_t<`yy>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 18}};
# 1134
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_Parse_Declarator _tmp28F;_LL17F: if((_tmp28E.YY30).tag != 36)goto _LL181;_tmp28F=(struct Cyc_Parse_Declarator)(_tmp28E.YY30).val;_LL180:
# 1138
 yyzzz=_tmp28F;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL17E:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY30).val=yy1,(((_tmp9BE.YY30).tag=36,_tmp9BE)));}static char _tmp292[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 45}};
# 1135
struct _tuple24*Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct _tuple24*_tmp294;_LL184: if((_tmp293.YY31).tag != 37)goto _LL186;_tmp294=(struct _tuple24*)(_tmp293.YY31).val;_LL185:
# 1139
 yyzzz=_tmp294;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL183:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY31).val=yy1,(((_tmp9BF.YY31).tag=37,_tmp9BF)));}static char _tmp297[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 57}};
# 1136
struct Cyc_List_List*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL189: if((_tmp298.YY32).tag != 38)goto _LL18B;_tmp299=(struct Cyc_List_List*)(_tmp298.YY32).val;_LL18A:
# 1140
 yyzzz=_tmp299;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL188:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY32).val=yy1,(((_tmp9C0.YY32).tag=38,_tmp9C0)));}static char _tmp29C[26]="abstractdeclarator_t<`yy>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 26}};
# 1137
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_Parse_Abstractdeclarator _tmp29E;_LL18E: if((_tmp29D.YY33).tag != 39)goto _LL190;_tmp29E=(struct Cyc_Parse_Abstractdeclarator)(_tmp29D.YY33).val;_LL18F:
# 1141
 yyzzz=_tmp29E;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL18D:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY33).val=yy1,(((_tmp9C1.YY33).tag=39,_tmp9C1)));}static char _tmp2A1[5]="bool";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 5}};
# 1138
int Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;int _tmp2A3;_LL193: if((_tmp2A2.YY34).tag != 40)goto _LL195;_tmp2A3=(int)(_tmp2A2.YY34).val;_LL194:
# 1142
 yyzzz=_tmp2A3;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL192:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY34).val=yy1,(((_tmp9C2.YY34).tag=40,_tmp9C2)));}static char _tmp2A6[8]="scope_t";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 8}};
# 1139
enum Cyc_Absyn_Scope Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;enum Cyc_Absyn_Scope _tmp2A8;_LL198: if((_tmp2A7.YY35).tag != 41)goto _LL19A;_tmp2A8=(enum Cyc_Absyn_Scope)(_tmp2A7.YY35).val;_LL199:
# 1143
 yyzzz=_tmp2A8;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL197:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY35).val=yy1,(((_tmp9C3.YY35).tag=41,_tmp9C3)));}static char _tmp2AB[16]="datatypefield_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 16}};
# 1140
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_Absyn_Datatypefield*_tmp2AD;_LL19D: if((_tmp2AC.YY36).tag != 42)goto _LL19F;_tmp2AD=(struct Cyc_Absyn_Datatypefield*)(_tmp2AC.YY36).val;_LL19E:
# 1144
 yyzzz=_tmp2AD;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL19C:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY36).val=yy1,(((_tmp9C4.YY36).tag=42,_tmp9C4)));}static char _tmp2B0[27]="list_t<datatypefield_t,`H>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 27}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;struct Cyc_List_List*_tmp2B2;_LL1A2: if((_tmp2B1.YY37).tag != 43)goto _LL1A4;_tmp2B2=(struct Cyc_List_List*)(_tmp2B1.YY37).val;_LL1A3:
# 1145
 yyzzz=_tmp2B2;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A1:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY37).val=yy1,(((_tmp9C5.YY37).tag=43,_tmp9C5)));}static char _tmp2B5[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 41}};
# 1142
struct _tuple25 Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;struct _tuple25 _tmp2B7;_LL1A7: if((_tmp2B6.YY38).tag != 44)goto _LL1A9;_tmp2B7=(struct _tuple25)(_tmp2B6.YY38).val;_LL1A8:
# 1146
 yyzzz=_tmp2B7;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1A6:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY38).val=yy1,(((_tmp9C6.YY38).tag=44,_tmp9C6)));}static char _tmp2BA[17]="list_t<var_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 17}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;struct Cyc_List_List*_tmp2BC;_LL1AC: if((_tmp2BB.YY39).tag != 45)goto _LL1AE;_tmp2BC=(struct Cyc_List_List*)(_tmp2BB.YY39).val;_LL1AD:
# 1147
 yyzzz=_tmp2BC;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1AB:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY39).val=yy1,(((_tmp9C7.YY39).tag=45,_tmp9C7)));}static char _tmp2BF[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 31}};
# 1144
struct _tuple8*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct _tuple8*_tmp2C1;_LL1B1: if((_tmp2C0.YY40).tag != 46)goto _LL1B3;_tmp2C1=(struct _tuple8*)(_tmp2C0.YY40).val;_LL1B2:
# 1148
 yyzzz=_tmp2C1;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY40).val=yy1,(((_tmp9C8.YY40).tag=46,_tmp9C8)));}static char _tmp2C4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 42}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL1B6: if((_tmp2C5.YY41).tag != 47)goto _LL1B8;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY41).val;_LL1B7:
# 1149
 yyzzz=_tmp2C6;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1B5:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY41).val=yy1,(((_tmp9C9.YY41).tag=47,_tmp9C9)));}static char _tmp2C9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 115}};
# 1146
struct _tuple26*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct _tuple26*_tmp2CB;_LL1BB: if((_tmp2CA.YY42).tag != 48)goto _LL1BD;_tmp2CB=(struct _tuple26*)(_tmp2CA.YY42).val;_LL1BC:
# 1150
 yyzzz=_tmp2CB;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BA:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY42).val=yy1,(((_tmp9CA.YY42).tag=48,_tmp9CA)));}static char _tmp2CE[18]="list_t<type_t,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 18}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;_LL1C0: if((_tmp2CF.YY43).tag != 49)goto _LL1C2;_tmp2D0=(struct Cyc_List_List*)(_tmp2CF.YY43).val;_LL1C1:
# 1151
 yyzzz=_tmp2D0;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1BF:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY43).val=yy1,(((_tmp9CB.YY43).tag=49,_tmp9CB)));}static char _tmp2D3[24]="list_t<designator_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 24}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;struct Cyc_List_List*_tmp2D5;_LL1C5: if((_tmp2D4.YY44).tag != 50)goto _LL1C7;_tmp2D5=(struct Cyc_List_List*)(_tmp2D4.YY44).val;_LL1C6:
# 1153
 yyzzz=_tmp2D5;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C4:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY44).val=yy1,(((_tmp9CC.YY44).tag=50,_tmp9CC)));}static char _tmp2D8[13]="designator_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 13}};
# 1150
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;void*_tmp2DA;_LL1CA: if((_tmp2D9.YY45).tag != 51)goto _LL1CC;_tmp2DA=(void*)(_tmp2D9.YY45).val;_LL1CB:
# 1154
 yyzzz=_tmp2DA;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C9:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY45).val=yy1,(((_tmp9CD.YY45).tag=51,_tmp9CD)));}static char _tmp2DD[7]="kind_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 7}};
# 1151
struct Cyc_Absyn_Kind*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;struct Cyc_Absyn_Kind*_tmp2DF;_LL1CF: if((_tmp2DE.YY46).tag != 52)goto _LL1D1;_tmp2DF=(struct Cyc_Absyn_Kind*)(_tmp2DE.YY46).val;_LL1D0:
# 1155
 yyzzz=_tmp2DF;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1CE:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY46).val=yy1,(((_tmp9CE.YY46).tag=52,_tmp9CE)));}static char _tmp2E2[7]="type_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 7}};
# 1152
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;void*_tmp2E4;_LL1D4: if((_tmp2E3.YY47).tag != 53)goto _LL1D6;_tmp2E4=(void*)(_tmp2E3.YY47).val;_LL1D5:
# 1156
 yyzzz=_tmp2E4;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D3:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY47).val=yy1,(((_tmp9CF.YY47).tag=53,_tmp9CF)));}static char _tmp2E7[23]="list_t<attribute_t,`H>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 23}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;struct Cyc_List_List*_tmp2E9;_LL1D9: if((_tmp2E8.YY48).tag != 54)goto _LL1DB;_tmp2E9=(struct Cyc_List_List*)(_tmp2E8.YY48).val;_LL1DA:
# 1157
 yyzzz=_tmp2E9;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1D8:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY48).val=yy1,(((_tmp9D0.YY48).tag=54,_tmp9D0)));}static char _tmp2EC[12]="attribute_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 12}};
# 1154
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;void*_tmp2EE;_LL1DE: if((_tmp2ED.YY49).tag != 55)goto _LL1E0;_tmp2EE=(void*)(_tmp2ED.YY49).val;_LL1DF:
# 1158
 yyzzz=_tmp2EE;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1DD:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY49).val=yy1,(((_tmp9D1.YY49).tag=55,_tmp9D1)));}static char _tmp2F1[12]="enumfield_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 12}};
# 1155
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct Cyc_Absyn_Enumfield*_tmp2F3;_LL1E3: if((_tmp2F2.YY50).tag != 56)goto _LL1E5;_tmp2F3=(struct Cyc_Absyn_Enumfield*)(_tmp2F2.YY50).val;_LL1E4:
# 1159
 yyzzz=_tmp2F3;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E2:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY50).val=yy1,(((_tmp9D2.YY50).tag=56,_tmp9D2)));}static char _tmp2F6[23]="list_t<enumfield_t,`H>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 23}};
# 1156
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_List_List*_tmp2F8;_LL1E8: if((_tmp2F7.YY51).tag != 57)goto _LL1EA;_tmp2F8=(struct Cyc_List_List*)(_tmp2F7.YY51).val;_LL1E9:
# 1160
 yyzzz=_tmp2F8;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1E7:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY51).val=yy1,(((_tmp9D3.YY51).tag=57,_tmp9D3)));}static char _tmp2FB[11]="type_opt_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 11}};
# 1157
void*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2FC=yy1;void*_tmp2FD;_LL1ED: if((_tmp2FC.YY52).tag != 58)goto _LL1EF;_tmp2FD=(void*)(_tmp2FC.YY52).val;_LL1EE:
# 1161
 yyzzz=_tmp2FD;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1EC:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY52).val=yy1,(((_tmp9D4.YY52).tag=58,_tmp9D4)));}static char _tmp300[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 31}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp301=yy1;struct Cyc_List_List*_tmp302;_LL1F2: if((_tmp301.YY53).tag != 59)goto _LL1F4;_tmp302=(struct Cyc_List_List*)(_tmp301.YY53).val;_LL1F3:
# 1162
 yyzzz=_tmp302;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F1:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY53).val=yy1,(((_tmp9D5.YY53).tag=59,_tmp9D5)));}static char _tmp305[15]="conref_t<bool>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 15}};
# 1159
union Cyc_Absyn_Constraint*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp306=yy1;union Cyc_Absyn_Constraint*_tmp307;_LL1F7: if((_tmp306.YY54).tag != 60)goto _LL1F9;_tmp307=(union Cyc_Absyn_Constraint*)(_tmp306.YY54).val;_LL1F8:
# 1163
 yyzzz=_tmp307;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1F6:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY54).val=yy1,(((_tmp9D6.YY54).tag=60,_tmp9D6)));}static char _tmp30A[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 45}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30B=yy1;struct Cyc_List_List*_tmp30C;_LL1FC: if((_tmp30B.YY55).tag != 61)goto _LL1FE;_tmp30C=(struct Cyc_List_List*)(_tmp30B.YY55).val;_LL1FD:
# 1164
 yyzzz=_tmp30C;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL1FB:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY55).val=yy1,(((_tmp9D7.YY55).tag=61,_tmp9D7)));}static char _tmp30F[20]="pointer_qual_t<`yy>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY56={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 20}};
# 1161
void*Cyc_yyget_YY56(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp310=yy1;void*_tmp311;_LL201: if((_tmp310.YY56).tag != 62)goto _LL203;_tmp311=(void*)(_tmp310.YY56).val;_LL202:
# 1165
 yyzzz=_tmp311;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY56);_LL200:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY56).val=yy1,(((_tmp9D8.YY56).tag=62,_tmp9D8)));}static char _tmp314[21]="pointer_quals_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY57={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 21}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp315=yy1;struct Cyc_List_List*_tmp316;_LL206: if((_tmp315.YY57).tag != 63)goto _LL208;_tmp316=(struct Cyc_List_List*)(_tmp315.YY57).val;_LL207:
# 1166
 yyzzz=_tmp316;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_YY57);_LL205:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY57).val=yy1,(((_tmp9D9.YY57).tag=63,_tmp9D9)));}static char _tmp319[21]="$(bool,string_t<`H>)";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 21}};
# 1163
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp31A=yy1;struct _tuple20 _tmp31B;_LL20B: if((_tmp31A.Asm_tok).tag != 6)goto _LL20D;_tmp31B=(struct _tuple20)(_tmp31A.Asm_tok).val;_LL20C:
# 1167
 yyzzz=_tmp31B;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL20A:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.Asm_tok).val=yy1,(((_tmp9DA.Asm_tok).tag=6,_tmp9DA)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1185
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1189
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9DB;return(_tmp9DB.timestamp=0,((_tmp9DB.first_line=0,((_tmp9DB.first_column=0,((_tmp9DB.last_line=0,((_tmp9DB.last_column=0,_tmp9DB)))))))));}
# 1192
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1203 "parse.y"
static short Cyc_yytranslate[375]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp31E[2]="$";static char _tmp31F[6]="error";static char _tmp320[12]="$undefined.";static char _tmp321[5]="AUTO";static char _tmp322[9]="REGISTER";static char _tmp323[7]="STATIC";static char _tmp324[7]="EXTERN";static char _tmp325[8]="TYPEDEF";static char _tmp326[5]="VOID";static char _tmp327[5]="CHAR";static char _tmp328[6]="SHORT";static char _tmp329[4]="INT";static char _tmp32A[5]="LONG";static char _tmp32B[6]="FLOAT";static char _tmp32C[7]="DOUBLE";static char _tmp32D[7]="SIGNED";static char _tmp32E[9]="UNSIGNED";static char _tmp32F[6]="CONST";static char _tmp330[9]="VOLATILE";static char _tmp331[9]="RESTRICT";static char _tmp332[7]="STRUCT";static char _tmp333[6]="UNION";static char _tmp334[5]="CASE";static char _tmp335[8]="DEFAULT";static char _tmp336[7]="INLINE";static char _tmp337[7]="SIZEOF";static char _tmp338[9]="OFFSETOF";static char _tmp339[3]="IF";static char _tmp33A[5]="ELSE";static char _tmp33B[7]="SWITCH";static char _tmp33C[6]="WHILE";static char _tmp33D[3]="DO";static char _tmp33E[4]="FOR";static char _tmp33F[5]="GOTO";static char _tmp340[9]="CONTINUE";static char _tmp341[6]="BREAK";static char _tmp342[7]="RETURN";static char _tmp343[5]="ENUM";static char _tmp344[8]="NULL_kw";static char _tmp345[4]="LET";static char _tmp346[6]="THROW";static char _tmp347[4]="TRY";static char _tmp348[6]="CATCH";static char _tmp349[7]="EXPORT";static char _tmp34A[4]="NEW";static char _tmp34B[9]="ABSTRACT";static char _tmp34C[9]="FALLTHRU";static char _tmp34D[6]="USING";static char _tmp34E[10]="NAMESPACE";static char _tmp34F[9]="DATATYPE";static char _tmp350[8]="XTUNION";static char _tmp351[7]="TUNION";static char _tmp352[7]="MALLOC";static char _tmp353[8]="RMALLOC";static char _tmp354[7]="CALLOC";static char _tmp355[8]="RCALLOC";static char _tmp356[5]="SWAP";static char _tmp357[9]="REGION_T";static char _tmp358[6]="TAG_T";static char _tmp359[7]="REGION";static char _tmp35A[5]="RNEW";static char _tmp35B[8]="REGIONS";static char _tmp35C[13]="RESET_REGION";static char _tmp35D[16]="NOZEROTERM_QUAL";static char _tmp35E[14]="ZEROTERM_QUAL";static char _tmp35F[12]="REGION_QUAL";static char _tmp360[7]="PORTON";static char _tmp361[8]="PORTOFF";static char _tmp362[12]="DYNREGION_T";static char _tmp363[6]="ALIAS";static char _tmp364[8]="NUMELTS";static char _tmp365[8]="VALUEOF";static char _tmp366[10]="VALUEOF_T";static char _tmp367[9]="TAGCHECK";static char _tmp368[13]="NUMELTS_QUAL";static char _tmp369[10]="THIN_QUAL";static char _tmp36A[9]="FAT_QUAL";static char _tmp36B[13]="NOTNULL_QUAL";static char _tmp36C[14]="NULLABLE_QUAL";static char _tmp36D[14]="REQUIRES_QUAL";static char _tmp36E[12]="TAGGED_QUAL";static char _tmp36F[16]="EXTENSIBLE_QUAL";static char _tmp370[15]="RESETABLE_QUAL";static char _tmp371[7]="PTR_OP";static char _tmp372[7]="INC_OP";static char _tmp373[7]="DEC_OP";static char _tmp374[8]="LEFT_OP";static char _tmp375[9]="RIGHT_OP";static char _tmp376[6]="LE_OP";static char _tmp377[6]="GE_OP";static char _tmp378[6]="EQ_OP";static char _tmp379[6]="NE_OP";static char _tmp37A[7]="AND_OP";static char _tmp37B[6]="OR_OP";static char _tmp37C[11]="MUL_ASSIGN";static char _tmp37D[11]="DIV_ASSIGN";static char _tmp37E[11]="MOD_ASSIGN";static char _tmp37F[11]="ADD_ASSIGN";static char _tmp380[11]="SUB_ASSIGN";static char _tmp381[12]="LEFT_ASSIGN";static char _tmp382[13]="RIGHT_ASSIGN";static char _tmp383[11]="AND_ASSIGN";static char _tmp384[11]="XOR_ASSIGN";static char _tmp385[10]="OR_ASSIGN";static char _tmp386[9]="ELLIPSIS";static char _tmp387[11]="LEFT_RIGHT";static char _tmp388[12]="COLON_COLON";static char _tmp389[11]="IDENTIFIER";static char _tmp38A[17]="INTEGER_CONSTANT";static char _tmp38B[7]="STRING";static char _tmp38C[8]="WSTRING";static char _tmp38D[19]="CHARACTER_CONSTANT";static char _tmp38E[20]="WCHARACTER_CONSTANT";static char _tmp38F[18]="FLOATING_CONSTANT";static char _tmp390[9]="TYPE_VAR";static char _tmp391[13]="TYPEDEF_NAME";static char _tmp392[16]="QUAL_IDENTIFIER";static char _tmp393[18]="QUAL_TYPEDEF_NAME";static char _tmp394[10]="ATTRIBUTE";static char _tmp395[4]="ASM";static char _tmp396[4]="';'";static char _tmp397[4]="'{'";static char _tmp398[4]="'}'";static char _tmp399[4]="','";static char _tmp39A[4]="'='";static char _tmp39B[4]="'<'";static char _tmp39C[4]="'>'";static char _tmp39D[4]="'('";static char _tmp39E[4]="')'";static char _tmp39F[4]="'_'";static char _tmp3A0[4]="'$'";static char _tmp3A1[4]="':'";static char _tmp3A2[4]="'.'";static char _tmp3A3[4]="'['";static char _tmp3A4[4]="']'";static char _tmp3A5[4]="'*'";static char _tmp3A6[4]="'@'";static char _tmp3A7[4]="'?'";static char _tmp3A8[4]="'+'";static char _tmp3A9[4]="'|'";static char _tmp3AA[4]="'^'";static char _tmp3AB[4]="'&'";static char _tmp3AC[4]="'-'";static char _tmp3AD[4]="'/'";static char _tmp3AE[4]="'%'";static char _tmp3AF[4]="'~'";static char _tmp3B0[4]="'!'";static char _tmp3B1[5]="prog";static char _tmp3B2[17]="translation_unit";static char _tmp3B3[12]="export_list";static char _tmp3B4[19]="export_list_values";static char _tmp3B5[21]="external_declaration";static char _tmp3B6[15]="optional_comma";static char _tmp3B7[20]="function_definition";static char _tmp3B8[21]="function_definition2";static char _tmp3B9[13]="using_action";static char _tmp3BA[15]="unusing_action";static char _tmp3BB[17]="namespace_action";static char _tmp3BC[19]="unnamespace_action";static char _tmp3BD[12]="declaration";static char _tmp3BE[19]="resetable_qual_opt";static char _tmp3BF[17]="declaration_list";static char _tmp3C0[23]="declaration_specifiers";static char _tmp3C1[24]="storage_class_specifier";static char _tmp3C2[15]="attributes_opt";static char _tmp3C3[11]="attributes";static char _tmp3C4[15]="attribute_list";static char _tmp3C5[10]="attribute";static char _tmp3C6[15]="type_specifier";static char _tmp3C7[25]="type_specifier_notypedef";static char _tmp3C8[5]="kind";static char _tmp3C9[15]="type_qualifier";static char _tmp3CA[15]="enum_specifier";static char _tmp3CB[11]="enum_field";static char _tmp3CC[22]="enum_declaration_list";static char _tmp3CD[26]="struct_or_union_specifier";static char _tmp3CE[16]="type_params_opt";static char _tmp3CF[16]="struct_or_union";static char _tmp3D0[24]="struct_declaration_list";static char _tmp3D1[25]="struct_declaration_list0";static char _tmp3D2[21]="init_declarator_list";static char _tmp3D3[22]="init_declarator_list0";static char _tmp3D4[16]="init_declarator";static char _tmp3D5[19]="struct_declaration";static char _tmp3D6[25]="specifier_qualifier_list";static char _tmp3D7[35]="notypedef_specifier_qualifier_list";static char _tmp3D8[23]="struct_declarator_list";static char _tmp3D9[24]="struct_declarator_list0";static char _tmp3DA[18]="struct_declarator";static char _tmp3DB[19]="datatype_specifier";static char _tmp3DC[14]="qual_datatype";static char _tmp3DD[19]="datatypefield_list";static char _tmp3DE[20]="datatypefield_scope";static char _tmp3DF[14]="datatypefield";static char _tmp3E0[11]="declarator";static char _tmp3E1[23]="declarator_withtypedef";static char _tmp3E2[18]="direct_declarator";static char _tmp3E3[30]="direct_declarator_withtypedef";static char _tmp3E4[8]="pointer";static char _tmp3E5[12]="one_pointer";static char _tmp3E6[14]="pointer_quals";static char _tmp3E7[13]="pointer_qual";static char _tmp3E8[23]="pointer_null_and_bound";static char _tmp3E9[14]="pointer_bound";static char _tmp3EA[18]="zeroterm_qual_opt";static char _tmp3EB[8]="rgn_opt";static char _tmp3EC[11]="tqual_list";static char _tmp3ED[20]="parameter_type_list";static char _tmp3EE[9]="type_var";static char _tmp3EF[16]="optional_effect";static char _tmp3F0[19]="optional_rgn_order";static char _tmp3F1[10]="rgn_order";static char _tmp3F2[16]="optional_inject";static char _tmp3F3[11]="effect_set";static char _tmp3F4[14]="atomic_effect";static char _tmp3F5[11]="region_set";static char _tmp3F6[15]="parameter_list";static char _tmp3F7[22]="parameter_declaration";static char _tmp3F8[16]="identifier_list";static char _tmp3F9[17]="identifier_list0";static char _tmp3FA[12]="initializer";static char _tmp3FB[18]="array_initializer";static char _tmp3FC[17]="initializer_list";static char _tmp3FD[12]="designation";static char _tmp3FE[16]="designator_list";static char _tmp3FF[11]="designator";static char _tmp400[10]="type_name";static char _tmp401[14]="any_type_name";static char _tmp402[15]="type_name_list";static char _tmp403[20]="abstract_declarator";static char _tmp404[27]="direct_abstract_declarator";static char _tmp405[10]="statement";static char _tmp406[13]="open_exp_opt";static char _tmp407[18]="labeled_statement";static char _tmp408[21]="expression_statement";static char _tmp409[19]="compound_statement";static char _tmp40A[16]="block_item_list";static char _tmp40B[20]="selection_statement";static char _tmp40C[15]="switch_clauses";static char _tmp40D[20]="iteration_statement";static char _tmp40E[15]="jump_statement";static char _tmp40F[12]="exp_pattern";static char _tmp410[20]="conditional_pattern";static char _tmp411[19]="logical_or_pattern";static char _tmp412[20]="logical_and_pattern";static char _tmp413[21]="inclusive_or_pattern";static char _tmp414[21]="exclusive_or_pattern";static char _tmp415[12]="and_pattern";static char _tmp416[17]="equality_pattern";static char _tmp417[19]="relational_pattern";static char _tmp418[14]="shift_pattern";static char _tmp419[17]="additive_pattern";static char _tmp41A[23]="multiplicative_pattern";static char _tmp41B[13]="cast_pattern";static char _tmp41C[14]="unary_pattern";static char _tmp41D[16]="postfix_pattern";static char _tmp41E[16]="primary_pattern";static char _tmp41F[8]="pattern";static char _tmp420[19]="tuple_pattern_list";static char _tmp421[20]="tuple_pattern_list0";static char _tmp422[14]="field_pattern";static char _tmp423[19]="field_pattern_list";static char _tmp424[20]="field_pattern_list0";static char _tmp425[11]="expression";static char _tmp426[22]="assignment_expression";static char _tmp427[20]="assignment_operator";static char _tmp428[23]="conditional_expression";static char _tmp429[20]="constant_expression";static char _tmp42A[22]="logical_or_expression";static char _tmp42B[23]="logical_and_expression";static char _tmp42C[24]="inclusive_or_expression";static char _tmp42D[24]="exclusive_or_expression";static char _tmp42E[15]="and_expression";static char _tmp42F[20]="equality_expression";static char _tmp430[22]="relational_expression";static char _tmp431[17]="shift_expression";static char _tmp432[20]="additive_expression";static char _tmp433[26]="multiplicative_expression";static char _tmp434[16]="cast_expression";static char _tmp435[17]="unary_expression";static char _tmp436[15]="unary_operator";static char _tmp437[19]="postfix_expression";static char _tmp438[17]="field_expression";static char _tmp439[19]="primary_expression";static char _tmp43A[25]="argument_expression_list";static char _tmp43B[26]="argument_expression_list0";static char _tmp43C[9]="constant";static char _tmp43D[20]="qual_opt_identifier";static char _tmp43E[17]="qual_opt_typedef";static char _tmp43F[18]="struct_union_name";static char _tmp440[11]="field_name";static char _tmp441[12]="right_angle";
# 1553 "parse.y"
static struct _dyneither_ptr Cyc_yytname[292]={{_tmp31E,_tmp31E,_tmp31E + 2},{_tmp31F,_tmp31F,_tmp31F + 6},{_tmp320,_tmp320,_tmp320 + 12},{_tmp321,_tmp321,_tmp321 + 5},{_tmp322,_tmp322,_tmp322 + 9},{_tmp323,_tmp323,_tmp323 + 7},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 7},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 6},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 3},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 3},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 9},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 4},{_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 4},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 7},{_tmp34A,_tmp34A,_tmp34A + 4},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 9},{_tmp34D,_tmp34D,_tmp34D + 6},{_tmp34E,_tmp34E,_tmp34E + 10},{_tmp34F,_tmp34F,_tmp34F + 9},{_tmp350,_tmp350,_tmp350 + 8},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 8},{_tmp356,_tmp356,_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 9},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 8},{_tmp35C,_tmp35C,_tmp35C + 13},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 8},{_tmp362,_tmp362,_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 6},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,_tmp365 + 8},{_tmp366,_tmp366,_tmp366 + 10},{_tmp367,_tmp367,_tmp367 + 9},{_tmp368,_tmp368,_tmp368 + 13},{_tmp369,_tmp369,_tmp369 + 10},{_tmp36A,_tmp36A,_tmp36A + 9},{_tmp36B,_tmp36B,_tmp36B + 13},{_tmp36C,_tmp36C,_tmp36C + 14},{_tmp36D,_tmp36D,_tmp36D + 14},{_tmp36E,_tmp36E,_tmp36E + 12},{_tmp36F,_tmp36F,_tmp36F + 16},{_tmp370,_tmp370,_tmp370 + 15},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 7},{_tmp373,_tmp373,_tmp373 + 7},{_tmp374,_tmp374,_tmp374 + 8},{_tmp375,_tmp375,_tmp375 + 9},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,_tmp377 + 6},{_tmp378,_tmp378,_tmp378 + 6},{_tmp379,_tmp379,_tmp379 + 6},{_tmp37A,_tmp37A,_tmp37A + 7},{_tmp37B,_tmp37B,_tmp37B + 6},{_tmp37C,_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D,_tmp37D + 11},{_tmp37E,_tmp37E,_tmp37E + 11},{_tmp37F,_tmp37F,_tmp37F + 11},{_tmp380,_tmp380,_tmp380 + 11},{_tmp381,_tmp381,_tmp381 + 12},{_tmp382,_tmp382,_tmp382 + 13},{_tmp383,_tmp383,_tmp383 + 11},{_tmp384,_tmp384,_tmp384 + 11},{_tmp385,_tmp385,_tmp385 + 10},{_tmp386,_tmp386,_tmp386 + 9},{_tmp387,_tmp387,_tmp387 + 11},{_tmp388,_tmp388,_tmp388 + 12},{_tmp389,_tmp389,_tmp389 + 11},{_tmp38A,_tmp38A,_tmp38A + 17},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 8},{_tmp38D,_tmp38D,_tmp38D + 19},{_tmp38E,_tmp38E,_tmp38E + 20},{_tmp38F,_tmp38F,_tmp38F + 18},{_tmp390,_tmp390,_tmp390 + 9},{_tmp391,_tmp391,_tmp391 + 13},{_tmp392,_tmp392,_tmp392 + 16},{_tmp393,_tmp393,_tmp393 + 18},{_tmp394,_tmp394,_tmp394 + 10},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 4},{_tmp398,_tmp398,_tmp398 + 4},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 4},{_tmp3A1,_tmp3A1,_tmp3A1 + 4},{_tmp3A2,_tmp3A2,_tmp3A2 + 4},{_tmp3A3,_tmp3A3,_tmp3A3 + 4},{_tmp3A4,_tmp3A4,_tmp3A4 + 4},{_tmp3A5,_tmp3A5,_tmp3A5 + 4},{_tmp3A6,_tmp3A6,_tmp3A6 + 4},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 4},{_tmp3B1,_tmp3B1,_tmp3B1 + 5},{_tmp3B2,_tmp3B2,_tmp3B2 + 17},{_tmp3B3,_tmp3B3,_tmp3B3 + 12},{_tmp3B4,_tmp3B4,_tmp3B4 + 19},{_tmp3B5,_tmp3B5,_tmp3B5 + 21},{_tmp3B6,_tmp3B6,_tmp3B6 + 15},{_tmp3B7,_tmp3B7,_tmp3B7 + 20},{_tmp3B8,_tmp3B8,_tmp3B8 + 21},{_tmp3B9,_tmp3B9,_tmp3B9 + 13},{_tmp3BA,_tmp3BA,_tmp3BA + 15},{_tmp3BB,_tmp3BB,_tmp3BB + 17},{_tmp3BC,_tmp3BC,_tmp3BC + 19},{_tmp3BD,_tmp3BD,_tmp3BD + 12},{_tmp3BE,_tmp3BE,_tmp3BE + 19},{_tmp3BF,_tmp3BF,_tmp3BF + 17},{_tmp3C0,_tmp3C0,_tmp3C0 + 23},{_tmp3C1,_tmp3C1,_tmp3C1 + 24},{_tmp3C2,_tmp3C2,_tmp3C2 + 15},{_tmp3C3,_tmp3C3,_tmp3C3 + 11},{_tmp3C4,_tmp3C4,_tmp3C4 + 15},{_tmp3C5,_tmp3C5,_tmp3C5 + 10},{_tmp3C6,_tmp3C6,_tmp3C6 + 15},{_tmp3C7,_tmp3C7,_tmp3C7 + 25},{_tmp3C8,_tmp3C8,_tmp3C8 + 5},{_tmp3C9,_tmp3C9,_tmp3C9 + 15},{_tmp3CA,_tmp3CA,_tmp3CA + 15},{_tmp3CB,_tmp3CB,_tmp3CB + 11},{_tmp3CC,_tmp3CC,_tmp3CC + 22},{_tmp3CD,_tmp3CD,_tmp3CD + 26},{_tmp3CE,_tmp3CE,_tmp3CE + 16},{_tmp3CF,_tmp3CF,_tmp3CF + 16},{_tmp3D0,_tmp3D0,_tmp3D0 + 24},{_tmp3D1,_tmp3D1,_tmp3D1 + 25},{_tmp3D2,_tmp3D2,_tmp3D2 + 21},{_tmp3D3,_tmp3D3,_tmp3D3 + 22},{_tmp3D4,_tmp3D4,_tmp3D4 + 16},{_tmp3D5,_tmp3D5,_tmp3D5 + 19},{_tmp3D6,_tmp3D6,_tmp3D6 + 25},{_tmp3D7,_tmp3D7,_tmp3D7 + 35},{_tmp3D8,_tmp3D8,_tmp3D8 + 23},{_tmp3D9,_tmp3D9,_tmp3D9 + 24},{_tmp3DA,_tmp3DA,_tmp3DA + 18},{_tmp3DB,_tmp3DB,_tmp3DB + 19},{_tmp3DC,_tmp3DC,_tmp3DC + 14},{_tmp3DD,_tmp3DD,_tmp3DD + 19},{_tmp3DE,_tmp3DE,_tmp3DE + 20},{_tmp3DF,_tmp3DF,_tmp3DF + 14},{_tmp3E0,_tmp3E0,_tmp3E0 + 11},{_tmp3E1,_tmp3E1,_tmp3E1 + 23},{_tmp3E2,_tmp3E2,_tmp3E2 + 18},{_tmp3E3,_tmp3E3,_tmp3E3 + 30},{_tmp3E4,_tmp3E4,_tmp3E4 + 8},{_tmp3E5,_tmp3E5,_tmp3E5 + 12},{_tmp3E6,_tmp3E6,_tmp3E6 + 14},{_tmp3E7,_tmp3E7,_tmp3E7 + 13},{_tmp3E8,_tmp3E8,_tmp3E8 + 23},{_tmp3E9,_tmp3E9,_tmp3E9 + 14},{_tmp3EA,_tmp3EA,_tmp3EA + 18},{_tmp3EB,_tmp3EB,_tmp3EB + 8},{_tmp3EC,_tmp3EC,_tmp3EC + 11},{_tmp3ED,_tmp3ED,_tmp3ED + 20},{_tmp3EE,_tmp3EE,_tmp3EE + 9},{_tmp3EF,_tmp3EF,_tmp3EF + 16},{_tmp3F0,_tmp3F0,_tmp3F0 + 19},{_tmp3F1,_tmp3F1,_tmp3F1 + 10},{_tmp3F2,_tmp3F2,_tmp3F2 + 16},{_tmp3F3,_tmp3F3,_tmp3F3 + 11},{_tmp3F4,_tmp3F4,_tmp3F4 + 14},{_tmp3F5,_tmp3F5,_tmp3F5 + 11},{_tmp3F6,_tmp3F6,_tmp3F6 + 15},{_tmp3F7,_tmp3F7,_tmp3F7 + 22},{_tmp3F8,_tmp3F8,_tmp3F8 + 16},{_tmp3F9,_tmp3F9,_tmp3F9 + 17},{_tmp3FA,_tmp3FA,_tmp3FA + 12},{_tmp3FB,_tmp3FB,_tmp3FB + 18},{_tmp3FC,_tmp3FC,_tmp3FC + 17},{_tmp3FD,_tmp3FD,_tmp3FD + 12},{_tmp3FE,_tmp3FE,_tmp3FE + 16},{_tmp3FF,_tmp3FF,_tmp3FF + 11},{_tmp400,_tmp400,_tmp400 + 10},{_tmp401,_tmp401,_tmp401 + 14},{_tmp402,_tmp402,_tmp402 + 15},{_tmp403,_tmp403,_tmp403 + 20},{_tmp404,_tmp404,_tmp404 + 27},{_tmp405,_tmp405,_tmp405 + 10},{_tmp406,_tmp406,_tmp406 + 13},{_tmp407,_tmp407,_tmp407 + 18},{_tmp408,_tmp408,_tmp408 + 21},{_tmp409,_tmp409,_tmp409 + 19},{_tmp40A,_tmp40A,_tmp40A + 16},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 20},{_tmp40E,_tmp40E,_tmp40E + 15},{_tmp40F,_tmp40F,_tmp40F + 12},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 19},{_tmp412,_tmp412,_tmp412 + 20},{_tmp413,_tmp413,_tmp413 + 21},{_tmp414,_tmp414,_tmp414 + 21},{_tmp415,_tmp415,_tmp415 + 12},{_tmp416,_tmp416,_tmp416 + 17},{_tmp417,_tmp417,_tmp417 + 19},{_tmp418,_tmp418,_tmp418 + 14},{_tmp419,_tmp419,_tmp419 + 17},{_tmp41A,_tmp41A,_tmp41A + 23},{_tmp41B,_tmp41B,_tmp41B + 13},{_tmp41C,_tmp41C,_tmp41C + 14},{_tmp41D,_tmp41D,_tmp41D + 16},{_tmp41E,_tmp41E,_tmp41E + 16},{_tmp41F,_tmp41F,_tmp41F + 8},{_tmp420,_tmp420,_tmp420 + 19},{_tmp421,_tmp421,_tmp421 + 20},{_tmp422,_tmp422,_tmp422 + 14},{_tmp423,_tmp423,_tmp423 + 19},{_tmp424,_tmp424,_tmp424 + 20},{_tmp425,_tmp425,_tmp425 + 11},{_tmp426,_tmp426,_tmp426 + 22},{_tmp427,_tmp427,_tmp427 + 20},{_tmp428,_tmp428,_tmp428 + 23},{_tmp429,_tmp429,_tmp429 + 20},{_tmp42A,_tmp42A,_tmp42A + 22},{_tmp42B,_tmp42B,_tmp42B + 23},{_tmp42C,_tmp42C,_tmp42C + 24},{_tmp42D,_tmp42D,_tmp42D + 24},{_tmp42E,_tmp42E,_tmp42E + 15},{_tmp42F,_tmp42F,_tmp42F + 20},{_tmp430,_tmp430,_tmp430 + 22},{_tmp431,_tmp431,_tmp431 + 17},{_tmp432,_tmp432,_tmp432 + 20},{_tmp433,_tmp433,_tmp433 + 26},{_tmp434,_tmp434,_tmp434 + 16},{_tmp435,_tmp435,_tmp435 + 17},{_tmp436,_tmp436,_tmp436 + 15},{_tmp437,_tmp437,_tmp437 + 19},{_tmp438,_tmp438,_tmp438 + 17},{_tmp439,_tmp439,_tmp439 + 19},{_tmp43A,_tmp43A,_tmp43A + 25},{_tmp43B,_tmp43B,_tmp43B + 26},{_tmp43C,_tmp43C,_tmp43C + 9},{_tmp43D,_tmp43D,_tmp43D + 20},{_tmp43E,_tmp43E,_tmp43E + 17},{_tmp43F,_tmp43F,_tmp43F + 18},{_tmp440,_tmp440,_tmp440 + 11},{_tmp441,_tmp441,_tmp441 + 12}};
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
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4FD[8]="stdcall";static char _tmp4FE[6]="cdecl";static char _tmp4FF[9]="fastcall";static char _tmp500[9]="noreturn";static char _tmp501[6]="const";static char _tmp502[8]="aligned";static char _tmp503[7]="packed";static char _tmp504[7]="shared";static char _tmp505[7]="unused";static char _tmp506[5]="weak";static char _tmp507[10]="dllimport";static char _tmp508[10]="dllexport";static char _tmp509[23]="no_instrument_function";static char _tmp50A[12]="constructor";static char _tmp50B[11]="destructor";static char _tmp50C[22]="no_check_memory_usage";static char _tmp50D[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9E3(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9E2,unsigned int*_tmp9E1,union Cyc_YYSTYPE**_tmp9E0){for(*_tmp9E2=0;*_tmp9E2 < *_tmp9E1;(*_tmp9E2)++){(*_tmp9E0)[*_tmp9E2]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9E8(unsigned int*_tmp9E7,unsigned int*_tmp9E6,struct Cyc_Yyltype**_tmp9E5){for(*_tmp9E7=0;*_tmp9E7 < *_tmp9E6;(*_tmp9E7)++){(*_tmp9E5)[*_tmp9E7]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9EF(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9EE,unsigned int*_tmp9ED,short**_tmp9EB){for(*_tmp9EE=0;*_tmp9EE < *_tmp9ED;(*_tmp9EE)++){(*_tmp9EB)[*_tmp9EE]=(short)(
# 216
*_tmp9EE <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9EE)): 0);}}static void _tmp9F5(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9F4,unsigned int*_tmp9F3,union Cyc_YYSTYPE**_tmp9F1){for(*_tmp9F4=0;*_tmp9F4 < *_tmp9F3;(*_tmp9F4)++){(*_tmp9F1)[*_tmp9F4]=(union Cyc_YYSTYPE)(
# 219
*_tmp9F4 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9F4)):*yylval);}}static void _tmp9FB(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9FA,unsigned int*_tmp9F9,struct Cyc_Yyltype**_tmp9F7){for(*_tmp9FA=0;*_tmp9FA < *_tmp9F9;(*_tmp9FA)++){(*_tmp9F7)[*_tmp9FA]=(struct Cyc_Yyltype)(
# 222
*_tmp9FA <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9FA)):
 Cyc_yynewloc());}}static void _tmpEE9(unsigned int*_tmpEE8,unsigned int*_tmpEE7,char**_tmpEE5){for(*_tmpEE8=0;*_tmpEE8 < *_tmpEE7;(*_tmpEE8)++){(*_tmpEE5)[*_tmpEE8]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp443=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp443;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9DC;union Cyc_YYSTYPE yylval=((_tmp9DC.YYINITIALSVAL).val=0,(((_tmp9DC.YYINITIALSVAL).tag=64,_tmp9DC)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9DE;unsigned int _tmp9DD;struct _dyneither_ptr yyss=(_tmp9DD=200,((_tmp9DE=_region_calloc(yyregion,sizeof(short),_tmp9DD),_tag_dyneither(_tmp9DE,sizeof(short),_tmp9DD))));
# 158
int yyvsp_offset;
unsigned int _tmp9E2;unsigned int _tmp9E1;union Cyc_YYSTYPE*_tmp9E0;unsigned int _tmp9DF;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9DF=(unsigned int)200,((_tmp9E0=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9DF)),((((_tmp9E1=_tmp9DF,_tmp9E3(& yylval,& _tmp9E2,& _tmp9E1,& _tmp9E0))),_tmp9E0)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9E7;unsigned int _tmp9E6;struct Cyc_Yyltype*_tmp9E5;unsigned int _tmp9E4;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9E4=(unsigned int)200,((_tmp9E5=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9E4)),((((_tmp9E6=_tmp9E4,_tmp9E8(& _tmp9E7,& _tmp9E6,& _tmp9E5))),_tmp9E5)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9E9;Cyc_yyerror(((_tmp9E9="parser stack overflow",_tag_dyneither(_tmp9E9,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9EE;unsigned int _tmp9ED;struct _dyneither_ptr _tmp9EC;short*_tmp9EB;unsigned int _tmp9EA;struct _dyneither_ptr yyss1=(_tmp9EA=(unsigned int)yystacksize,((_tmp9EB=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9EA)),((_tmp9EC=_tag_dyneither(_tmp9EB,sizeof(short),_tmp9EA),((((_tmp9ED=_tmp9EA,_tmp9EF(& yyssp_offset,& yyss,& _tmp9EE,& _tmp9ED,& _tmp9EB))),_tmp9EC)))))));
# 217
unsigned int _tmp9F4;unsigned int _tmp9F3;struct _dyneither_ptr _tmp9F2;union Cyc_YYSTYPE*_tmp9F1;unsigned int _tmp9F0;struct _dyneither_ptr yyvs1=
(_tmp9F0=(unsigned int)yystacksize,((_tmp9F1=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9F0)),((_tmp9F2=_tag_dyneither(_tmp9F1,sizeof(union Cyc_YYSTYPE),_tmp9F0),((((_tmp9F3=_tmp9F0,_tmp9F5(& yyvs,& yyssp_offset,& yylval,& _tmp9F4,& _tmp9F3,& _tmp9F1))),_tmp9F2)))))));
# 221
unsigned int _tmp9FA;unsigned int _tmp9F9;struct _dyneither_ptr _tmp9F8;struct Cyc_Yyltype*_tmp9F7;unsigned int _tmp9F6;struct _dyneither_ptr yyls1=(_tmp9F6=(unsigned int)yystacksize,((_tmp9F7=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9F6)),((_tmp9F8=_tag_dyneither(_tmp9F7,sizeof(struct Cyc_Yyltype),_tmp9F6),((((_tmp9F9=_tmp9F6,_tmp9FB(& yyssp_offset,& yyls,& _tmp9FA,& _tmp9F9,& _tmp9F7))),_tmp9F8)))))));
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
int _tmp451=0;_npop_handler(0);return _tmp451;}
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
{struct _handler_cons _tmp452;_push_handler(& _tmp452);{int _tmp454=0;if(setjmp(_tmp452.handler))_tmp454=1;if(!_tmp454){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp453=(void*)_exn_thrown;void*_tmp456=_tmp453;void*_tmp458;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp457=(struct Cyc_Core_Failure_exn_struct*)_tmp456;if(_tmp457->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1179
 x=0;goto _LL212;_LL215: _tmp458=_tmp456;_LL216:(void)_throw(_tmp458);_LL212:;}};}
# 1181
{struct _handler_cons _tmp459;_push_handler(& _tmp459);{int _tmp45B=0;if(setjmp(_tmp459.handler))_tmp45B=1;if(!_tmp45B){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp45A=(void*)_exn_thrown;void*_tmp45D=_tmp45A;void*_tmp45F;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp45E=(struct Cyc_Core_Failure_exn_struct*)_tmp45D;if(_tmp45E->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1185
 y=0;goto _LL217;_LL21A: _tmp45F=_tmp45D;_LL21B:(void)_throw(_tmp45F);_LL217:;}};}
# 1187
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1189
break;}case 3: _LL211: {
# 1191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1192 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA05;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA04;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA03;struct Cyc_List_List*_tmpA02;yyval=Cyc_YY19(((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->hd=((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->r=(void*)((_tmpA03=_cycalloc(sizeof(*_tmpA03)),((_tmpA03[0]=((_tmpA04.tag=11,((_tmpA04.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA04.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA04)))))),_tmpA03)))),((_tmpA05->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA05)))))),((_tmpA02->tl=0,_tmpA02)))))));}
Cyc_Lex_leave_using();
# 1195
break;}case 4: _LL21C: {
# 1197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1196 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA0F;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA0E;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA0D;struct Cyc_List_List*_tmpA0C;yyval=Cyc_YY19(((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C->hd=((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F->r=(void*)((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0E.tag=11,((_tmpA0E.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA0E.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA0E)))))),_tmpA0D)))),((_tmpA0F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0F)))))),((_tmpA0C->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmpA0C)))))));}
break;}case 5: _LL21D: {
# 1199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1199 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA1D;struct _dyneither_ptr*_tmpA1C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA1B;struct Cyc_List_List*_tmpA1A;yyval=Cyc_YY19(((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A->hd=((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E->r=(void*)((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B[0]=((_tmpA1D.tag=10,((_tmpA1D.f1=((_tmpA1C=_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA1C)))),((_tmpA1D.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1D)))))),_tmpA1B)))),((_tmpA1E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1E)))))),((_tmpA1A->tl=0,_tmpA1A)))))));}
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
{struct _handler_cons _tmp46D;_push_handler(& _tmp46D);{int _tmp46F=0;if(setjmp(_tmp46D.handler))_tmp46F=1;if(!_tmp46F){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp46E=(void*)_exn_thrown;void*_tmp471=_tmp46E;void*_tmp473;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp472=(struct Cyc_Core_Failure_exn_struct*)_tmp471;if(_tmp472->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmpA1F;nspace=((_tmpA1F="",_tag_dyneither(_tmpA1F,sizeof(char),1)));}goto _LL220;_LL223: _tmp473=_tmp471;_LL224:(void)_throw(_tmp473);_LL220:;}};}
# 1209
{struct _handler_cons _tmp475;_push_handler(& _tmp475);{int _tmp477=0;if(setjmp(_tmp475.handler))_tmp477=1;if(!_tmp477){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp476=(void*)_exn_thrown;void*_tmp479=_tmp476;void*_tmp47B;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp47A=(struct Cyc_Core_Failure_exn_struct*)_tmp479;if(_tmp47A->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp47B=_tmp479;_LL229:(void)_throw(_tmp47B);_LL225:;}};}
# 1212
{struct _handler_cons _tmp47C;_push_handler(& _tmp47C);{int _tmp47E=0;if(setjmp(_tmp47C.handler))_tmp47E=1;if(!_tmp47E){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp47D=(void*)_exn_thrown;void*_tmp480=_tmp47D;void*_tmp482;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp481=(struct Cyc_Core_Failure_exn_struct*)_tmp480;if(_tmp481->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp482=_tmp480;_LL22E:(void)_throw(_tmp482);_LL22A:;}};}
# 1215
{struct Cyc_Absyn_Decl*_tmpA2E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA2D;struct _dyneither_ptr*_tmpA2C;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA2B;struct Cyc_List_List*_tmpA2A;yyval=Cyc_YY19(((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->hd=((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->r=(void*)((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2D.tag=10,((_tmpA2D.f1=((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C[0]=nspace,_tmpA2C)))),((_tmpA2D.f2=x,_tmpA2D)))))),_tmpA2B)))),((_tmpA2E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2E)))))),((_tmpA2A->tl=y,_tmpA2A)))))));}
# 1217
break;}case 7: _LL21F: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1218 "parse.y"
struct _dyneither_ptr _tmp488=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmpA2F;if(Cyc_strcmp((struct _dyneither_ptr)_tmp488,((_tmpA2F="C",_tag_dyneither(_tmpA2F,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA39;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA38;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA37;struct Cyc_List_List*_tmpA36;yyval=Cyc_YY19(((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39->r=(void*)((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA38.tag=12,((_tmpA38.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA38)))),_tmpA37)))),((_tmpA39->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA39)))))),((_tmpA36->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA36)))))));}else{
# 1222
{const char*_tmpA3A;if(Cyc_strcmp((struct _dyneither_ptr)_tmp488,((_tmpA3A="C include",_tag_dyneither(_tmpA3A,sizeof(char),10))))!= 0){
const char*_tmpA3B;Cyc_Parse_err(((_tmpA3B="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA3B,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1225
struct Cyc_Absyn_Decl*_tmpA45;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA44;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY19(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->r=(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=13,((_tmpA44.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA44.f2=0,_tmpA44)))))),_tmpA43)))),((_tmpA45->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA45)))))),((_tmpA42->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA42)))))));};}}
# 1228
break;}case 8: _LL22F: {
# 1230
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1232
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1229 "parse.y"
{const char*_tmpA46;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA46="C include",_tag_dyneither(_tmpA46,sizeof(char),10))))!= 0){
const char*_tmpA47;Cyc_Parse_err(((_tmpA47="expecting \"C include\"",_tag_dyneither(_tmpA47,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA51;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA50;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA4F;struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY19(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->r=(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=13,((_tmpA50.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA50.f2=exs,_tmpA50)))))),_tmpA4F)))),((_tmpA51->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA51)))))),((_tmpA4E->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA4E)))))));}
# 1234
break;};}case 9: _LL230: {
# 1236
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1238
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1235 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA54;struct Cyc_List_List*_tmpA53;yyval=Cyc_YY19(((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53->hd=((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA54->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA54)))))),((_tmpA53->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA53)))))));}
break;}case 10: _LL231: {
# 1238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1237 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA57;struct Cyc_List_List*_tmpA56;yyval=Cyc_YY19(((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56->hd=((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA57->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA57)))))),((_tmpA56->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA56)))))));}
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
{struct _tuple27*_tmpA5A;struct Cyc_List_List*_tmpA59;yyval=Cyc_YY55(((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->hd=((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA5A->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA5A->f3=0,_tmpA5A)))))))),((_tmpA59->tl=0,_tmpA59)))))));}
break;}case 15: _LL236: {
# 1250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1248 "parse.y"
{struct _tuple27*_tmpA5D;struct Cyc_List_List*_tmpA5C;yyval=Cyc_YY55(((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->hd=((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA5D->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA5D->f3=0,_tmpA5D)))))))),((_tmpA5C->tl=0,_tmpA5C)))))));}
break;}case 16: _LL237: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1250 "parse.y"
{struct _tuple27*_tmpA60;struct Cyc_List_List*_tmpA5F;yyval=Cyc_YY55(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA60->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA60->f3=0,_tmpA60)))))))),((_tmpA5F->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA5F)))))));}
break;}case 17: _LL238: {
# 1253
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1255
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1254 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA66;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA65;struct Cyc_List_List*_tmpA64;yyval=Cyc_YY19(((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->hd=Cyc_Absyn_new_decl((void*)((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA65.tag=1,((_tmpA65.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA65)))),_tmpA66)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA64->tl=0,_tmpA64)))))));}
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
struct Cyc_Parse_Declaration_spec _tmp4A7=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A7,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
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
struct Cyc_Parse_Declaration_spec _tmp4A8=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A8,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23F: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1290 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A9=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A9,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL240: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1293 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4AA=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4AA,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
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
{struct _dyneither_ptr*_tmpA67;Cyc_Lex_enter_namespace(((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA67)))));}yyval=yyyyvsp[1];
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
int _tmp4AC=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp4AC,(unsigned int)_tmp4AC));
break;}case 33: _LL246: {
# 1317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1316 "parse.y"
int _tmp4AD=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4AD));
break;}case 34: _LL247: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1320 "parse.y"
{struct Cyc_List_List*_tmpA68;yyval=Cyc_YY19(((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA68->tl=0,_tmpA68)))))));}
break;}case 35: _LL248: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1322 "parse.y"
struct Cyc_List_List*_tmp4AF=0;
{struct Cyc_List_List*_tmp4B0=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp4B0 != 0;_tmp4B0=_tmp4B0->tl){
struct _dyneither_ptr*_tmp4B1=(struct _dyneither_ptr*)_tmp4B0->hd;
struct _tuple0*_tmpA69;struct _tuple0*qv=(_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->f1=Cyc_Absyn_Rel_n(0),((_tmpA69->f2=_tmp4B1,_tmpA69)))));
struct Cyc_Absyn_Vardecl*_tmp4B2=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA6A;_tmp4AF=((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A->hd=_tmp4B2,((_tmpA6A->tl=_tmp4AF,_tmpA6A))))));}}
# 1329
_tmp4AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AF);
{struct Cyc_List_List*_tmpA6B;yyval=Cyc_YY19(((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->hd=Cyc_Absyn_letv_decl(_tmp4AF,(unsigned int)(yyyylsp[0]).first_line),((_tmpA6B->tl=0,_tmpA6B)))))));}
# 1332
break;}case 36: _LL249: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1335 "parse.y"
struct _dyneither_ptr _tmp4B6=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA6C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B6,((_tmpA6C="`H",_tag_dyneither(_tmpA6C,sizeof(char),3))))== 0){
const char*_tmpA6F;void*_tmpA6E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA6E=0,Cyc_aprintf(((_tmpA6F="bad occurrence of heap region",_tag_dyneither(_tmpA6F,sizeof(char),30))),_tag_dyneither(_tmpA6E,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA70;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B6,((_tmpA70="`U",_tag_dyneither(_tmpA70,sizeof(char),3))))== 0){
const char*_tmpA73;void*_tmpA72;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA72=0,Cyc_aprintf(((_tmpA73="bad occurrence of unique region",_tag_dyneither(_tmpA73,sizeof(char),32))),_tag_dyneither(_tmpA72,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA76;struct Cyc_Absyn_Tvar*_tmpA75;struct Cyc_Absyn_Tvar*tv=(_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->name=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=_tmp4B6,_tmpA76)))),((_tmpA75->identity=- 1,((_tmpA75->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA75)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA79;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA78;void*t=(void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=2,((_tmpA79.f1=tv,_tmpA79)))),_tmpA78))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA7F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA7E;struct _dyneither_ptr*_tmpA7C;struct _tuple0*_tmpA7B;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->f1=Cyc_Absyn_Loc_n,((_tmpA7B->f2=((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA7C)))),_tmpA7B)))))),(void*)((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=15,((_tmpA7F.f1=t,_tmpA7F)))),_tmpA7E)))),0);
{struct Cyc_List_List*_tmpA80;yyval=Cyc_YY19(((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA80->tl=0,_tmpA80)))))));}
# 1345
break;};}case 37: _LL24A: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1347 "parse.y"
int _tmp4C6=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4C7=Cyc_yyget_String_tok(yyyyvsp[2]);
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_yyget_YY5(yyyyvsp[3]);
{const char*_tmpA81;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C7,((_tmpA81="H",_tag_dyneither(_tmpA81,sizeof(char),2))))== 0){
const char*_tmpA82;Cyc_Parse_err(((_tmpA82="bad occurrence of heap region `H",_tag_dyneither(_tmpA82,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA83;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C7,((_tmpA83="U",_tag_dyneither(_tmpA83,sizeof(char),2))))== 0){
const char*_tmpA84;Cyc_Parse_err(((_tmpA84="bad occurrence of unique region `U",_tag_dyneither(_tmpA84,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}
if(_tmp4C6  && _tmp4C8 != 0){
const char*_tmpA85;Cyc_Parse_err(((_tmpA85="open regions cannot be @resetable",_tag_dyneither(_tmpA85,sizeof(char),34))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*_tmpA92;const char*_tmpA91;void*_tmpA90[1];struct Cyc_String_pa_PrintArg_struct _tmpA8F;struct Cyc_Absyn_Tvar*_tmpA8E;struct Cyc_Absyn_Tvar*tv=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->name=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=(struct _dyneither_ptr)((_tmpA8F.tag=0,((_tmpA8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C7),((_tmpA90[0]=& _tmpA8F,Cyc_aprintf(((_tmpA91="`%s",_tag_dyneither(_tmpA91,sizeof(char),4))),_tag_dyneither(_tmpA90,sizeof(void*),1)))))))),_tmpA92)))),((_tmpA8E->identity=- 1,((_tmpA8E->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA8E)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA95;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA94;void*t=(void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=2,((_tmpA95.f1=tv,_tmpA95)))),_tmpA94))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA9B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA9A;struct _dyneither_ptr*_tmpA98;struct _tuple0*_tmpA97;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->f1=Cyc_Absyn_Loc_n,((_tmpA97->f2=((_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98[0]=_tmp4C7,_tmpA98)))),_tmpA97)))))),(void*)((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=15,((_tmpA9B.f1=t,_tmpA9B)))),_tmpA9A)))),0);
# 1361
{struct Cyc_List_List*_tmpA9C;yyval=Cyc_YY19(((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4C6,_tmp4C8,(unsigned int)(yyyylsp[0]).first_line),((_tmpA9C->tl=0,_tmpA9C)))))));}
# 1363
break;};}case 38: _LL24B: {
# 1365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1365 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAA4;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpAA3;struct _dyneither_ptr*_tmpAA2;struct Cyc_Absyn_Tvar*_tmpAA1;struct Cyc_Absyn_Tvar*tv=(_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->name=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpAA2)))),((_tmpAA1->identity=- 1,((_tmpAA1->kind=(void*)((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=((_tmpAA3.tag=0,((_tmpAA3.f1=& Cyc_Tcutil_rk,_tmpAA3)))),_tmpAA4)))),_tmpAA1)))))));
struct Cyc_List_List*ds=Cyc_yyget_YY19(yyyyvsp[4]);
if(ds == 0  || ds->tl != 0){
const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)((unsigned int)(yyyylsp[0]).first_line,((_tmpAA7="too many declarations in alias",_tag_dyneither(_tmpAA7,sizeof(char),31))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
{void*_tmp4DC=d->r;void*_tmp4DD=_tmp4DC;struct Cyc_Absyn_Vardecl*_tmp4DF;_LL24E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4DE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4DD;if(_tmp4DE->tag != 0)goto _LL250;else{_tmp4DF=_tmp4DE->f1;}}_LL24F:
# 1372
{struct Cyc_List_List*_tmpAA8;yyval=Cyc_YY19(((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->hd=Cyc_Absyn_alias_decl(tv,_tmp4DF,(unsigned int)(yyyylsp[0]).first_line),((_tmpAA8->tl=0,_tmpAA8)))))));}
goto _LL24D;_LL250:;_LL251:
# 1375
{const char*_tmpAA9;Cyc_Parse_err(((_tmpAA9="expecting variable declaration",_tag_dyneither(_tmpAA9,sizeof(char),31))),(unsigned int)(yyyylsp[4]).first_line);}
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
{struct Cyc_Parse_Declaration_spec _tmpAAA;yyval=Cyc_YY20(((_tmpAAA.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAAA.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAAA.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAAA.is_inline=0,((_tmpAAA.attributes=0,_tmpAAA)))))))))));}
break;}case 44: _LL256: {
# 1400
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1402
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1399 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E7=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4E7.sc != 0){
const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAAD="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpAAD,sizeof(char),51))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));}
# 1403
{struct Cyc_Parse_Declaration_spec _tmpAAE;yyval=Cyc_YY20(((_tmpAAE.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAAE.tq=_tmp4E7.tq,((_tmpAAE.type_specs=_tmp4E7.type_specs,((_tmpAAE.is_inline=_tmp4E7.is_inline,((_tmpAAE.attributes=_tmp4E7.attributes,_tmpAAE)))))))))));}
# 1407
break;}case 45: _LL257: {
# 1409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1408 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAAF;yyval=Cyc_YY20(((_tmpAAF.sc=0,((_tmpAAF.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAAF.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpAAF.is_inline=0,((_tmpAAF.attributes=0,_tmpAAF)))))))))));}
break;}case 46: _LL258: {
# 1412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1411 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4EC=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB0;yyval=Cyc_YY20(((_tmpAB0.sc=_tmp4EC.sc,((_tmpAB0.tq=_tmp4EC.tq,((_tmpAB0.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4EC.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpAB0.is_inline=_tmp4EC.is_inline,((_tmpAB0.attributes=_tmp4EC.attributes,_tmpAB0)))))))))));}
# 1417
break;}case 47: _LL259: {
# 1419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1418 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAB1;yyval=Cyc_YY20(((_tmpAB1.sc=0,((_tmpAB1.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpAB1.type_specs=Cyc_Parse_empty_spec(0),((_tmpAB1.is_inline=0,((_tmpAB1.attributes=0,_tmpAB1)))))))))));}
break;}case 48: _LL25A: {
# 1421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1420 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4EF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB2;yyval=Cyc_YY20(((_tmpAB2.sc=_tmp4EF.sc,((_tmpAB2.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4EF.tq),((_tmpAB2.type_specs=_tmp4EF.type_specs,((_tmpAB2.is_inline=_tmp4EF.is_inline,((_tmpAB2.attributes=_tmp4EF.attributes,_tmpAB2)))))))))));}
# 1425
break;}case 49: _LL25B: {
# 1427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1426 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAB3;yyval=Cyc_YY20(((_tmpAB3.sc=0,((_tmpAB3.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAB3.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAB3.is_inline=1,((_tmpAB3.attributes=0,_tmpAB3)))))))))));}
break;}case 50: _LL25C: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1429 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB4;yyval=Cyc_YY20(((_tmpAB4.sc=_tmp4F2.sc,((_tmpAB4.tq=_tmp4F2.tq,((_tmpAB4.type_specs=_tmp4F2.type_specs,((_tmpAB4.is_inline=1,((_tmpAB4.attributes=_tmp4F2.attributes,_tmpAB4)))))))))));}
# 1433
break;}case 51: _LL25D: {
# 1435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1434 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAB5;yyval=Cyc_YY20(((_tmpAB5.sc=0,((_tmpAB5.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAB5.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAB5.is_inline=0,((_tmpAB5.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpAB5)))))))))));}
break;}case 52: _LL25E: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1437 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAB6;yyval=Cyc_YY20(((_tmpAB6.sc=_tmp4F5.sc,((_tmpAB6.tq=_tmp4F5.tq,((_tmpAB6.type_specs=_tmp4F5.type_specs,((_tmpAB6.is_inline=_tmp4F5.is_inline,((_tmpAB6.attributes=
# 1440
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4F5.attributes),_tmpAB6)))))))))));}
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
{const char*_tmpAB7;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAB7="C",_tag_dyneither(_tmpAB7,sizeof(char),2))))!= 0){
const char*_tmpAB8;Cyc_Parse_err(((_tmpAB8="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAB8,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_List_List*_tmpAB9;yyval=Cyc_YY48(((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAB9->tl=0,_tmpAB9)))))));}
break;}case 64: _LL26A: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1478 "parse.y"
{struct Cyc_List_List*_tmpABA;yyval=Cyc_YY48(((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpABA->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpABA)))))));}
break;}case 65: _LL26B: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1483 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4FD,_tmp4FD,_tmp4FD + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4FE,_tmp4FE,_tmp4FE + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4FF,_tmp4FF,_tmp4FF + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp500,_tmp500,_tmp500 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp501,_tmp501,_tmp501 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp502,_tmp502,_tmp502 + 8},(void*)& att_aligned},{{_tmp503,_tmp503,_tmp503 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp504,_tmp504,_tmp504 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp505,_tmp505,_tmp505 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp506,_tmp506,_tmp506 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp507,_tmp507,_tmp507 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp508,_tmp508,_tmp508 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp509,_tmp509,_tmp509 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp50A,_tmp50A,_tmp50A + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp50B,_tmp50B,_tmp50B + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp50C,_tmp50C,_tmp50C + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp50D,_tmp50D,_tmp50D + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1503
struct _dyneither_ptr _tmp4FB=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1505
if((((_get_dyneither_size(_tmp4FB,sizeof(char))> 4  && ((const char*)_tmp4FB.curr)[0]== '_') && ((const char*)_tmp4FB.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4FB,sizeof(char),(int)(_get_dyneither_size(_tmp4FB,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4FB,sizeof(char),(int)(_get_dyneither_size(_tmp4FB,sizeof(char))- 3)))== '_')
# 1507
_tmp4FB=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4FB,2,_get_dyneither_size(_tmp4FB,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4FB,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1514
if(i == 17){
{const char*_tmpABB;Cyc_Parse_err(((_tmpABB="unrecognized attribute",_tag_dyneither(_tmpABB,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp50F=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _tmp510=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp512;struct _tuple5 _tmp511=_tmp510;_tmp512=_tmp511.f2;{
void*a;
{const char*_tmpABD;const char*_tmpABC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpABD="regparm",_tag_dyneither(_tmpABD,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpABC="__regparm__",_tag_dyneither(_tmpABC,sizeof(char),12))))== 0){
if(_tmp512 < 0  || _tmp512 > 3){
const char*_tmpABE;Cyc_Parse_err(((_tmpABE="regparm requires value between 0 and 3",_tag_dyneither(_tmpABE,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAC1;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAC0;a=(void*)((_tmpAC0=_cycalloc_atomic(sizeof(*_tmpAC0)),((_tmpAC0[0]=((_tmpAC1.tag=0,((_tmpAC1.f1=_tmp512,_tmpAC1)))),_tmpAC0))));};}else{
const char*_tmpAC3;const char*_tmpAC2;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAC3="aligned",_tag_dyneither(_tmpAC3,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAC2="__aligned__",_tag_dyneither(_tmpAC2,sizeof(char),12))))== 0){
if(_tmp512 < 0){const char*_tmpAC4;Cyc_Parse_err(((_tmpAC4="aligned requires positive power of two",_tag_dyneither(_tmpAC4,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp512;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpAC5;Cyc_Parse_err(((_tmpAC5="aligned requires positive power of two",_tag_dyneither(_tmpAC5,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAC8;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAC7;a=(void*)((_tmpAC7=_cycalloc_atomic(sizeof(*_tmpAC7)),((_tmpAC7[0]=((_tmpAC8.tag=6,((_tmpAC8.f1=_tmp512,_tmpAC8)))),_tmpAC7))));};};}else{
const char*_tmpACA;const char*_tmpAC9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpACA="initializes",_tag_dyneither(_tmpACA,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAC9="__initializes__",_tag_dyneither(_tmpAC9,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpACD;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpACC;a=(void*)((_tmpACC=_cycalloc_atomic(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpACD.tag=20,((_tmpACD.f1=_tmp512,_tmpACD)))),_tmpACC))));}else{
const char*_tmpACF;const char*_tmpACE;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpACF="noliveunique",_tag_dyneither(_tmpACF,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpACE="__noliveunique__",_tag_dyneither(_tmpACE,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAD2;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAD1;a=(void*)((_tmpAD1=_cycalloc_atomic(sizeof(*_tmpAD1)),((_tmpAD1[0]=((_tmpAD2.tag=21,((_tmpAD2.f1=_tmp512,_tmpAD2)))),_tmpAD1))));}else{
const char*_tmpAD4;const char*_tmpAD3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAD4="noconsume",_tag_dyneither(_tmpAD4,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp50F,((_tmpAD3="__noconsume__",_tag_dyneither(_tmpAD3,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAD7;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAD6;a=(void*)((_tmpAD6=_cycalloc_atomic(sizeof(*_tmpAD6)),((_tmpAD6[0]=((_tmpAD7.tag=22,((_tmpAD7.f1=_tmp512,_tmpAD7)))),_tmpAD6))));}else{
# 1543
{const char*_tmpAD8;Cyc_Parse_err(((_tmpAD8="unrecognized attribute",_tag_dyneither(_tmpAD8,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp52C=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpADA;const char*_tmpAD9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpADA="section",_tag_dyneither(_tmpADA,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,((_tmpAD9="__section__",_tag_dyneither(_tmpAD9,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpADD;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpADC;a=(void*)((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADD.tag=8,((_tmpADD.f1=_tmp52C,_tmpADD)))),_tmpADC))));}else{
# 1555
{const char*_tmpADE;Cyc_Parse_err(((_tmpADE="unrecognized attribute",_tag_dyneither(_tmpADE,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp532=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp533=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1565
{const char*_tmpADF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp532,((_tmpADF="__mode__",_tag_dyneither(_tmpADF,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAE2;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAE1;a=(void*)((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1[0]=((_tmpAE2.tag=24,((_tmpAE2.f1=_tmp533,_tmpAE2)))),_tmpAE1))));}else{
# 1568
{const char*_tmpAE3;Cyc_Parse_err(((_tmpAE3="unrecognized attribute",_tag_dyneither(_tmpAE3,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
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
struct _dyneither_ptr _tmp538=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp539=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _tmp53A=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp53C;struct _tuple5 _tmp53B=_tmp53A;_tmp53C=_tmp53B.f2;{
struct _tuple5 _tmp53D=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp53F;struct _tuple5 _tmp53E=_tmp53D;_tmp53F=_tmp53E.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAE5;const char*_tmpAE4;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp538,((_tmpAE5="format",_tag_dyneither(_tmpAE5,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp538,((_tmpAE4="__format__",_tag_dyneither(_tmpAE4,sizeof(char),11))))== 0){
const char*_tmpAE6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpAE6="printf",_tag_dyneither(_tmpAE6,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAE9;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAE8;a=(void*)((_tmpAE8=_cycalloc_atomic(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=19,((_tmpAE9.f1=Cyc_Absyn_Printf_ft,((_tmpAE9.f2=_tmp53C,((_tmpAE9.f3=_tmp53F,_tmpAE9)))))))),_tmpAE8))));}else{
const char*_tmpAEA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpAEA="scanf",_tag_dyneither(_tmpAEA,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAED;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAEC;a=(void*)((_tmpAEC=_cycalloc_atomic(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAED.tag=19,((_tmpAED.f1=Cyc_Absyn_Scanf_ft,((_tmpAED.f2=_tmp53C,((_tmpAED.f3=_tmp53F,_tmpAED)))))))),_tmpAEC))));}else{
# 1585
const char*_tmpAEE;Cyc_Parse_err(((_tmpAEE="unrecognized format type",_tag_dyneither(_tmpAEE,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1587
const char*_tmpAEF;Cyc_Parse_err(((_tmpAEF="unrecognized attribute",_tag_dyneither(_tmpAEF,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAF2;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF2.tag=17,((_tmpAF2.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpAF2.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpAF2.f3=0,((_tmpAF2.f4=0,_tmpAF2)))))))))),_tmpAF1)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAF5;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAF4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4[0]=((_tmpAF5.tag=10,((_tmpAF5.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpAF5)))),_tmpAF4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1628
break;}case 89: _LL283: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1629 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAF8;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAF7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7[0]=((_tmpAF8.tag=15,((_tmpAF8.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAF8)))),_tmpAF7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL284: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1631 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAFB;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAFA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=((_tmpAFB.tag=15,((_tmpAFB.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAFB)))),_tmpAFA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1633
break;}case 91: _LL285: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1634 "parse.y"
void*_tmp552=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAFE;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAFD;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAFD=_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD[0]=((_tmpAFE.tag=16,((_tmpAFE.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAFE.f2=_tmp552,_tmpAFE)))))),_tmpAFD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1637
break;}case 92: _LL286: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1638 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB01;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB00;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=16,((_tmpB01.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpB01.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpB01)))))),_tmpB00)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL287: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1640 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB04;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB03;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=19,((_tmpB04.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpB04)))),_tmpB03)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL288: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1642 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB07;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB06;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB07.tag=19,((_tmpB07.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB07)))),_tmpB06)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL289: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1644 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB0A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB09;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=18,((_tmpB0A.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB0A)))),_tmpB09)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_Tqual _tmpB0B;yyval=Cyc_YY26(((_tmpB0B.print_const=1,((_tmpB0B.q_volatile=0,((_tmpB0B.q_restrict=0,((_tmpB0B.real_const=1,((_tmpB0B.loc=loc,_tmpB0B)))))))))));}
break;}case 98: _LL28C: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1655 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB0C;yyval=Cyc_YY26(((_tmpB0C.print_const=0,((_tmpB0C.q_volatile=1,((_tmpB0C.q_restrict=0,((_tmpB0C.real_const=0,((_tmpB0C.loc=0,_tmpB0C)))))))))));}
break;}case 99: _LL28D: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1656 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB0D;yyval=Cyc_YY26(((_tmpB0D.print_const=0,((_tmpB0D.q_volatile=0,((_tmpB0D.q_restrict=1,((_tmpB0D.real_const=0,((_tmpB0D.loc=0,_tmpB0D)))))))))));}
break;}case 100: _LL28E: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB1C;struct Cyc_Absyn_Enumdecl*_tmpB1B;struct Cyc_Core_Opt*_tmpB1A;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB19;struct Cyc_Absyn_TypeDecl*_tmpB18;struct Cyc_Absyn_TypeDecl*_tmp560=(_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18->r=(void*)((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB19.tag=1,((_tmpB19.f1=((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->sc=Cyc_Absyn_Public,((_tmpB1B->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB1B->fields=((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB1A)))),_tmpB1B)))))))),_tmpB19)))),_tmpB1C)))),((_tmpB18->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB18)))));
# 1664
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB1F;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=((_tmpB1F.tag=26,((_tmpB1F.f1=_tmp560,((_tmpB1F.f2=0,_tmpB1F)))))),_tmpB1E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1666
break;}case 101: _LL28F: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1667 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB22;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB21;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB22.tag=13,((_tmpB22.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB22.f2=0,_tmpB22)))))),_tmpB21)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL290: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1669 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB25;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB24;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=((_tmpB25.tag=14,((_tmpB25.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB25)))),_tmpB24)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL291: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB26;yyval=Cyc_YY50(((_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB26->tag=0,((_tmpB26->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB26)))))))));}
break;}case 104: _LL292: {
# 1678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1677 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB27;yyval=Cyc_YY50(((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB27->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB27->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB27)))))))));}
break;}case 105: _LL293: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1681 "parse.y"
{struct Cyc_List_List*_tmpB28;yyval=Cyc_YY51(((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB28->tl=0,_tmpB28)))))));}
break;}case 106: _LL294: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1682 "parse.y"
{struct Cyc_List_List*_tmpB29;yyval=Cyc_YY51(((_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB29->tl=0,_tmpB29)))))));}
break;}case 107: _LL295: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1683 "parse.y"
{struct Cyc_List_List*_tmpB2A;yyval=Cyc_YY51(((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB2A->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB2A)))))));}
break;}case 108: _LL296: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB2D;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB2C;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C[0]=((_tmpB2D.tag=12,((_tmpB2D.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB2D.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpB2D)))))),_tmpB2C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL297: {
# 1692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1695 "parse.y"
struct Cyc_List_List*_tmp573=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp575=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp573,
Cyc_Absyn_aggrdecl_impl(_tmp574,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1700
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB30;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB2F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F[0]=((_tmpB30.tag=26,((_tmpB30.f1=_tmp575,((_tmpB30.f2=0,_tmpB30)))))),_tmpB2F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1702
break;}case 110: _LL298: {
# 1704
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1706
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1706 "parse.y"
struct Cyc_List_List*_tmp578=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp579=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp57A=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp578,
Cyc_Absyn_aggrdecl_impl(_tmp579,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1711
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB33;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB32;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=((_tmpB33.tag=26,((_tmpB33.f1=_tmp57A,((_tmpB33.f2=0,_tmpB33)))))),_tmpB32)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1713
break;}case 111: _LL299: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1714 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB3D;struct Cyc_Core_Opt*_tmpB3C;struct Cyc_Absyn_AggrInfo _tmpB3B;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB3A;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A[0]=((_tmpB3D.tag=11,((_tmpB3D.f1=((_tmpB3B.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB3C=_cycalloc_atomic(sizeof(*_tmpB3C)),((_tmpB3C->v=(void*)1,_tmpB3C))))),((_tmpB3B.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB3B)))),_tmpB3D)))),_tmpB3A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1717
break;}case 112: _LL29A: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1718 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB43;struct Cyc_Absyn_AggrInfo _tmpB42;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB41;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB43.tag=11,((_tmpB43.f1=((_tmpB42.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB42.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB42)))),_tmpB43)))),_tmpB41)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmp584=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp584 != 0;_tmp584=_tmp584->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp584->hd,decls);}}{
# 1743
struct Cyc_List_List*_tmp585=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp585 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp585,decls);
yyval=Cyc_YY27(decls);
# 1748
break;};}case 119: _LL2A1: {
# 1750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB44;yyval=Cyc_YY28(((_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB44->tl=0,_tmpB44)))))));}
break;}case 120: _LL2A2: {
# 1756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1755 "parse.y"
{struct Cyc_List_List*_tmpB45;yyval=Cyc_YY28(((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB45->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB45)))))));}
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
{struct _tuple12*_tmpB46;yyval=Cyc_YY22(((_tmpB46=_region_malloc(yyr,sizeof(*_tmpB46)),((_tmpB46->tl=0,((_tmpB46->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB46)))))));}
break;}case 123: _LL2A5: {
# 1768
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1770
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1767 "parse.y"
{struct _tuple12*_tmpB47;yyval=Cyc_YY22(((_tmpB47=_region_malloc(yyr,sizeof(*_tmpB47)),((_tmpB47->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB47->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB47)))))));}
break;}case 124: _LL2A6: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1772 "parse.y"
{struct _tuple11 _tmpB48;yyval=Cyc_YY21(((_tmpB48.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB48.f2=0,_tmpB48)))));}
break;}case 125: _LL2A7: {
# 1775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1774 "parse.y"
{struct _tuple11 _tmpB49;yyval=Cyc_YY21(((_tmpB49.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB49.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB49)))));}
break;}case 126: _LL2A8: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1780 "parse.y"
struct _RegionHandle _tmp58C=_new_region("temp");struct _RegionHandle*temp=& _tmp58C;_push_region(temp);{
struct _tuple25 _tmp58D=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp58F;struct Cyc_Parse_Type_specifier _tmp590;struct Cyc_List_List*_tmp591;struct _tuple25 _tmp58E=_tmp58D;_tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;_tmp591=_tmp58E.f3;
if(_tmp58F.loc == 0)_tmp58F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp592=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp592 != 0;_tmp592=_tmp592->tl){
struct _tuple24*_tmp593=(struct _tuple24*)_tmp592->hd;struct Cyc_Parse_Declarator _tmp595;struct Cyc_Absyn_Exp*_tmp596;struct Cyc_Absyn_Exp*_tmp597;struct _tuple24*_tmp594=_tmp593;_tmp595=_tmp594->f1;_tmp596=_tmp594->f2;_tmp597=_tmp594->f3;
{struct _tuple10*_tmpB4A;decls=((_tmpB4A=_region_malloc(temp,sizeof(*_tmpB4A)),((_tmpB4A->tl=decls,((_tmpB4A->hd=_tmp595,_tmpB4A))))));}{
struct _tuple16*_tmpB4D;struct Cyc_List_List*_tmpB4C;widths_and_reqs=(
(_tmpB4C=_region_malloc(temp,sizeof(*_tmpB4C)),((_tmpB4C->hd=((_tmpB4D=_region_malloc(temp,sizeof(*_tmpB4D)),((_tmpB4D->f1=_tmp596,((_tmpB4D->f2=_tmp597,_tmpB4D)))))),((_tmpB4C->tl=widths_and_reqs,_tmpB4C))))));};}}
# 1791
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp59B=Cyc_Parse_speclist2typ(_tmp590,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp59C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp58F,_tmp59B,decls,_tmp591),widths_and_reqs);
# 1797
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp59C));};};}
# 1800
_npop_handler(0);break;
# 1780
;_pop_region(temp);}case 127: _LL2A9: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1808 "parse.y"
{struct _tuple25 _tmpB4E;yyval=Cyc_YY38(((_tmpB4E.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB4E.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB4E.f3=0,_tmpB4E)))))));}
break;}case 128: _LL2AA: {
# 1811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1810 "parse.y"
struct _tuple25 _tmp59E=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB4F;yyval=Cyc_YY38(((_tmpB4F.f1=_tmp59E.f1,((_tmpB4F.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp59E.f2),((_tmpB4F.f3=_tmp59E.f3,_tmpB4F)))))));}
break;}case 129: _LL2AB: {
# 1813
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1815
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1812 "parse.y"
{struct _tuple25 _tmpB50;yyval=Cyc_YY38(((_tmpB50.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB50.f2=Cyc_Parse_empty_spec(0),((_tmpB50.f3=0,_tmpB50)))))));}
break;}case 130: _LL2AC: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1814 "parse.y"
struct _tuple25 _tmp5A1=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB51;yyval=Cyc_YY38(((_tmpB51.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5A1.f1),((_tmpB51.f2=_tmp5A1.f2,((_tmpB51.f3=_tmp5A1.f3,_tmpB51)))))));}
break;}case 131: _LL2AD: {
# 1818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1817 "parse.y"
{struct _tuple25 _tmpB52;yyval=Cyc_YY38(((_tmpB52.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB52.f2=Cyc_Parse_empty_spec(0),((_tmpB52.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB52)))))));}
break;}case 132: _LL2AE: {
# 1820
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1822
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1819 "parse.y"
struct _tuple25 _tmp5A4=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB53;yyval=Cyc_YY38(((_tmpB53.f1=_tmp5A4.f1,((_tmpB53.f2=_tmp5A4.f2,((_tmpB53.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp5A4.f3),_tmpB53)))))));}
break;}case 133: _LL2AF: {
# 1822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB54;yyval=Cyc_YY38(((_tmpB54.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB54.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB54.f3=0,_tmpB54)))))));}
break;}case 134: _LL2B0: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1827 "parse.y"
struct _tuple25 _tmp5A7=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB55;yyval=Cyc_YY38(((_tmpB55.f1=_tmp5A7.f1,((_tmpB55.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp5A7.f2),((_tmpB55.f3=_tmp5A7.f3,_tmpB55)))))));}
break;}case 135: _LL2B1: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1829 "parse.y"
{struct _tuple25 _tmpB56;yyval=Cyc_YY38(((_tmpB56.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB56.f2=Cyc_Parse_empty_spec(0),((_tmpB56.f3=0,_tmpB56)))))));}
break;}case 136: _LL2B2: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1831 "parse.y"
struct _tuple25 _tmp5AA=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB57;yyval=Cyc_YY38(((_tmpB57.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5AA.f1),((_tmpB57.f2=_tmp5AA.f2,((_tmpB57.f3=_tmp5AA.f3,_tmpB57)))))));}
break;}case 137: _LL2B3: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1834 "parse.y"
{struct _tuple25 _tmpB58;yyval=Cyc_YY38(((_tmpB58.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB58.f2=Cyc_Parse_empty_spec(0),((_tmpB58.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB58)))))));}
break;}case 138: _LL2B4: {
# 1837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1836 "parse.y"
struct _tuple25 _tmp5AD=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB59;yyval=Cyc_YY38(((_tmpB59.f1=_tmp5AD.f1,((_tmpB59.f2=_tmp5AD.f2,((_tmpB59.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp5AD.f3),_tmpB59)))))));}
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
{struct Cyc_List_List*_tmpB5A;yyval=Cyc_YY32(((_tmpB5A=_region_malloc(yyr,sizeof(*_tmpB5A)),((_tmpB5A->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB5A->tl=0,_tmpB5A)))))));}
break;}case 141: _LL2B7: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1848 "parse.y"
{struct Cyc_List_List*_tmpB5B;yyval=Cyc_YY32(((_tmpB5B=_region_malloc(yyr,sizeof(*_tmpB5B)),((_tmpB5B->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB5B->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB5B)))))));}
break;}case 142: _LL2B8: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1853 "parse.y"
{struct _tuple24*_tmpB5C;yyval=Cyc_YY31(((_tmpB5C=_region_malloc(yyr,sizeof(*_tmpB5C)),((_tmpB5C->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB5C->f2=0,((_tmpB5C->f3=0,_tmpB5C)))))))));}
break;}case 143: _LL2B9: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1857 "parse.y"
{struct Cyc_Parse_Declarator _tmpB62;struct _tuple0*_tmpB61;struct _tuple24*_tmpB60;yyval=Cyc_YY31(((_tmpB60=_region_malloc(yyr,sizeof(*_tmpB60)),((_tmpB60->f1=((_tmpB62.id=((_tmpB61=_cycalloc(sizeof(*_tmpB61)),((_tmpB61->f1=Cyc_Absyn_Rel_n(0),((_tmpB61->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB61)))))),((_tmpB62.tms=0,_tmpB62)))),((_tmpB60->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB60->f3=0,_tmpB60)))))))));}
# 1859
break;}case 144: _LL2BA:
# 1863
{struct Cyc_Parse_Declarator _tmpB68;struct _tuple0*_tmpB67;struct _tuple24*_tmpB66;yyval=Cyc_YY31(((_tmpB66=_region_malloc(yyr,sizeof(*_tmpB66)),((_tmpB66->f1=((_tmpB68.id=((_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->f1=Cyc_Absyn_Rel_n(0),((_tmpB67->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB67)))))),((_tmpB68.tms=0,_tmpB68)))),((_tmpB66->f2=0,((_tmpB66->f3=0,_tmpB66)))))))));}
# 1865
break;case 145: _LL2BB: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1865 "parse.y"
{struct _tuple24*_tmpB69;yyval=Cyc_YY31(((_tmpB69=_region_malloc(yyr,sizeof(*_tmpB69)),((_tmpB69->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB69->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB69->f3=0,_tmpB69)))))))));}
break;}case 146: _LL2BC: {
# 1868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1867 "parse.y"
{struct _tuple24*_tmpB6A;yyval=Cyc_YY31(((_tmpB6A=_region_malloc(yyr,sizeof(*_tmpB6A)),((_tmpB6A->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB6A->f2=0,((_tmpB6A->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB6A)))))))));}
break;}case 147: _LL2BD: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp5BC=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp5BD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB6B;struct Cyc_Absyn_TypeDecl*_tmp5BE=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5BD,((_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB6B)))),_tmp5BC,(unsigned int)(yyyylsp[0]).first_line);
# 1877
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB6E;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB6D;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((_tmpB6D[0]=((_tmpB6E.tag=26,((_tmpB6E.f1=_tmp5BE,((_tmpB6E.f2=0,_tmpB6E)))))),_tmpB6D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1879
break;}case 148: _LL2BE: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1880 "parse.y"
int _tmp5C2=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB78;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB77;struct Cyc_Absyn_DatatypeInfo _tmpB76;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB75;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75[0]=((_tmpB78.tag=3,((_tmpB78.f1=((_tmpB76.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB77.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB77.is_extensible=_tmp5C2,_tmpB77))))),((_tmpB76.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB76)))),_tmpB78)))),_tmpB75)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1883
break;}case 149: _LL2BF: {
# 1885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1884 "parse.y"
int _tmp5C7=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB82;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB81;struct Cyc_Absyn_DatatypeFieldInfo _tmpB80;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB7F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB82.tag=4,((_tmpB82.f1=((_tmpB80.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB81.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB81.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB81.is_extensible=_tmp5C7,_tmpB81))))))),((_tmpB80.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB80)))),_tmpB82)))),_tmpB7F)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpB83;yyval=Cyc_YY37(((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB83->tl=0,_tmpB83)))))));}
break;}case 153: _LL2C3: {
# 1900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1898 "parse.y"
{struct Cyc_List_List*_tmpB84;yyval=Cyc_YY37(((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB84->tl=0,_tmpB84)))))));}
break;}case 154: _LL2C4: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1899 "parse.y"
{struct Cyc_List_List*_tmpB85;yyval=Cyc_YY37(((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB85->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB85)))))));}
break;}case 155: _LL2C5: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1900 "parse.y"
{struct Cyc_List_List*_tmpB86;yyval=Cyc_YY37(((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB86->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB86)))))));}
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
{struct Cyc_Absyn_Datatypefield*_tmpB87;yyval=Cyc_YY36(((_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB87->typs=0,((_tmpB87->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB87->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB87)))))))))));}
break;}case 160: _LL2CA: {
# 1913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1912 "parse.y"
struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB88;yyval=Cyc_YY36(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB88->typs=_tmp5D1,((_tmpB88->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB88->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB88)))))))))));}
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
struct Cyc_Parse_Declarator _tmp5D3=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB89;yyval=Cyc_YY30(((_tmpB89.id=_tmp5D3.id,((_tmpB89.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5D3.tms),_tmpB89)))));}
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
struct Cyc_Parse_Declarator _tmp5D5=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB8A;yyval=Cyc_YY30(((_tmpB8A.id=_tmp5D5.id,((_tmpB8A.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5D5.tms),_tmpB8A)))));}
break;}case 165: _LL2CF: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1935 "parse.y"
{struct Cyc_Parse_Declarator _tmpB8B;yyval=Cyc_YY30(((_tmpB8B.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB8B.tms=0,_tmpB8B)))));}
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
struct Cyc_Parse_Declarator _tmp5D8=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB91;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB90;struct Cyc_List_List*_tmpB8F;_tmp5D8.tms=((_tmpB8F=_region_malloc(yyr,sizeof(*_tmpB8F)),((_tmpB8F->hd=(void*)((_tmpB91=_region_malloc(yyr,sizeof(*_tmpB91)),((_tmpB91[0]=((_tmpB90.tag=5,((_tmpB90.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB90.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB90)))))),_tmpB91)))),((_tmpB8F->tl=_tmp5D8.tms,_tmpB8F))))));}
yyval=yyyyvsp[2];
# 1945
break;}case 168: _LL2D2: {
# 1947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1946 "parse.y"
{struct Cyc_List_List*_tmpB9B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB9A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB99;struct Cyc_Parse_Declarator _tmpB98;yyval=Cyc_YY30(((_tmpB98.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB98.tms=((_tmpB9B=_region_malloc(yyr,sizeof(*_tmpB9B)),((_tmpB9B->hd=(void*)((_tmpB99=_region_malloc(yyr,sizeof(*_tmpB99)),((_tmpB99[0]=((_tmpB9A.tag=0,((_tmpB9A.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB9A.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB9A)))))),_tmpB99)))),((_tmpB9B->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB9B)))))),_tmpB98)))));}
break;}case 169: _LL2D3: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1948 "parse.y"
{struct Cyc_List_List*_tmpBA5;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBA4;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBA3;struct Cyc_Parse_Declarator _tmpBA2;yyval=Cyc_YY30(((_tmpBA2.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA2.tms=(
(_tmpBA5=_region_malloc(yyr,sizeof(*_tmpBA5)),((_tmpBA5->hd=(void*)((_tmpBA3=_region_malloc(yyr,sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA4.tag=1,((_tmpBA4.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBA4.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBA4.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBA4)))))))),_tmpBA3)))),((_tmpBA5->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBA5)))))),_tmpBA2)))));}
break;}case 170: _LL2D4: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1951 "parse.y"
struct _tuple26*_tmp5E4=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5E6;int _tmp5E7;struct Cyc_Absyn_VarargInfo*_tmp5E8;void*_tmp5E9;struct Cyc_List_List*_tmp5EA;struct _tuple26*_tmp5E5=_tmp5E4;_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E5->f2;_tmp5E8=_tmp5E5->f3;_tmp5E9=_tmp5E5->f4;_tmp5EA=_tmp5E5->f5;
{struct Cyc_List_List*_tmpBBA;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBB9;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBB8;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBB7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBB6;struct Cyc_Parse_Declarator _tmpBB5;yyval=Cyc_YY30(((_tmpBB5.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB5.tms=((_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA->hd=(void*)((_tmpBB6=_region_malloc(yyr,sizeof(*_tmpBB6)),((_tmpBB6[0]=((_tmpBB9.tag=3,((_tmpBB9.f1=(void*)((_tmpBB7=_region_malloc(yyr,sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=1,((_tmpBB8.f1=_tmp5E6,((_tmpBB8.f2=_tmp5E7,((_tmpBB8.f3=_tmp5E8,((_tmpBB8.f4=_tmp5E9,((_tmpBB8.f5=_tmp5EA,_tmpBB8)))))))))))),_tmpBB7)))),_tmpBB9)))),_tmpBB6)))),((_tmpBBA->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBBA)))))),_tmpBB5)))));}
# 1954
break;}case 171: _LL2D5: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1955 "parse.y"
{struct Cyc_List_List*_tmpBCF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBCE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBCD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBCC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBCB;struct Cyc_Parse_Declarator _tmpBCA;yyval=Cyc_YY30(((_tmpBCA.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBCA.tms=(
(_tmpBCF=_region_malloc(yyr,sizeof(*_tmpBCF)),((_tmpBCF->hd=(void*)((_tmpBCB=_region_malloc(yyr,sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCE.tag=3,((_tmpBCE.f1=(void*)((_tmpBCC=_region_malloc(yyr,sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCD.tag=1,((_tmpBCD.f1=0,((_tmpBCD.f2=0,((_tmpBCD.f3=0,((_tmpBCD.f4=
# 1958
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpBCD.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpBCD)))))))))))),_tmpBCC)))),_tmpBCE)))),_tmpBCB)))),((_tmpBCF->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBCF)))))),_tmpBCA)))));}
# 1961
break;}case 172: _LL2D6: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1962 "parse.y"
{struct Cyc_List_List*_tmpBE4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE3;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBE2;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBE1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE0;struct Cyc_Parse_Declarator _tmpBDF;yyval=Cyc_YY30(((_tmpBDF.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBDF.tms=((_tmpBE4=_region_malloc(yyr,sizeof(*_tmpBE4)),((_tmpBE4->hd=(void*)((_tmpBE0=_region_malloc(yyr,sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE3.tag=3,((_tmpBE3.f1=(void*)((_tmpBE1=_region_malloc(yyr,sizeof(*_tmpBE1)),((_tmpBE1[0]=((_tmpBE2.tag=0,((_tmpBE2.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBE2.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBE2)))))),_tmpBE1)))),_tmpBE3)))),_tmpBE0)))),((_tmpBE4->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBE4)))))),_tmpBDF)))));}
break;}case 173: _LL2D7: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1965 "parse.y"
struct Cyc_List_List*_tmp5FD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBEE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBED;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBEC;struct Cyc_Parse_Declarator _tmpBEB;yyval=Cyc_YY30(((_tmpBEB.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBEB.tms=((_tmpBEE=_region_malloc(yyr,sizeof(*_tmpBEE)),((_tmpBEE->hd=(void*)((_tmpBEC=_region_malloc(yyr,sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBED.tag=4,((_tmpBED.f1=_tmp5FD,((_tmpBED.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBED.f3=0,_tmpBED)))))))),_tmpBEC)))),((_tmpBEE->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBEE)))))),_tmpBEB)))));}
# 1968
break;}case 174: _LL2D8: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1969 "parse.y"
{struct Cyc_List_List*_tmpBF8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBF7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBF6;struct Cyc_Parse_Declarator _tmpBF5;yyval=Cyc_YY30(((_tmpBF5.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBF5.tms=((_tmpBF8=_region_malloc(yyr,sizeof(*_tmpBF8)),((_tmpBF8->hd=(void*)((_tmpBF6=_region_malloc(yyr,sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF7.tag=5,((_tmpBF7.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBF7.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBF7)))))),_tmpBF6)))),((_tmpBF8->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBF8)))))),_tmpBF5)))));}
# 1972
break;}case 175: _LL2D9: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1983 "parse.y"
{struct Cyc_Parse_Declarator _tmpBF9;yyval=Cyc_YY30(((_tmpBF9.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBF9.tms=0,_tmpBF9)))));}
break;}case 176: _LL2DA: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1985 "parse.y"
{struct Cyc_Parse_Declarator _tmpBFA;yyval=Cyc_YY30(((_tmpBFA.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBFA.tms=0,_tmpBFA)))));}
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
struct Cyc_Parse_Declarator _tmp608=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC00;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBFF;struct Cyc_List_List*_tmpBFE;_tmp608.tms=((_tmpBFE=_region_malloc(yyr,sizeof(*_tmpBFE)),((_tmpBFE->hd=(void*)((_tmpC00=_region_malloc(yyr,sizeof(*_tmpC00)),((_tmpC00[0]=((_tmpBFF.tag=5,((_tmpBFF.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBFF.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBFF)))))),_tmpC00)))),((_tmpBFE->tl=_tmp608.tms,_tmpBFE))))));}
yyval=yyyyvsp[2];
# 1995
break;}case 179: _LL2DD: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1996 "parse.y"
struct Cyc_Parse_Declarator _tmp60C=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC0A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC09;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC08;struct Cyc_Parse_Declarator _tmpC07;yyval=Cyc_YY30(((_tmpC07.id=_tmp60C.id,((_tmpC07.tms=(
(_tmpC0A=_region_malloc(yyr,sizeof(*_tmpC0A)),((_tmpC0A->hd=(void*)((_tmpC08=_region_malloc(yyr,sizeof(*_tmpC08)),((_tmpC08[0]=((_tmpC09.tag=0,((_tmpC09.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpC09.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC09)))))),_tmpC08)))),((_tmpC0A->tl=_tmp60C.tms,_tmpC0A)))))),_tmpC07)))));}
break;}case 180: _LL2DE: {
# 2001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp611=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC14;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC13;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC12;struct Cyc_Parse_Declarator _tmpC11;yyval=Cyc_YY30(((_tmpC11.id=_tmp611.id,((_tmpC11.tms=(
(_tmpC14=_region_malloc(yyr,sizeof(*_tmpC14)),((_tmpC14->hd=(void*)((_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC13.tag=1,((_tmpC13.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpC13.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpC13.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC13)))))))),_tmpC12)))),((_tmpC14->tl=_tmp611.tms,_tmpC14)))))),_tmpC11)))));}
# 2004
break;}case 181: _LL2DF: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2005 "parse.y"
struct _tuple26*_tmp616=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp618;int _tmp619;struct Cyc_Absyn_VarargInfo*_tmp61A;void*_tmp61B;struct Cyc_List_List*_tmp61C;struct _tuple26*_tmp617=_tmp616;_tmp618=_tmp617->f1;_tmp619=_tmp617->f2;_tmp61A=_tmp617->f3;_tmp61B=_tmp617->f4;_tmp61C=_tmp617->f5;{
struct Cyc_Parse_Declarator _tmp61D=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC29;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC28;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC27;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC26;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC25;struct Cyc_Parse_Declarator _tmpC24;yyval=Cyc_YY30(((_tmpC24.id=_tmp61D.id,((_tmpC24.tms=((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29->hd=(void*)((_tmpC25=_region_malloc(yyr,sizeof(*_tmpC25)),((_tmpC25[0]=((_tmpC28.tag=3,((_tmpC28.f1=(void*)((_tmpC26=_region_malloc(yyr,sizeof(*_tmpC26)),((_tmpC26[0]=((_tmpC27.tag=1,((_tmpC27.f1=_tmp618,((_tmpC27.f2=_tmp619,((_tmpC27.f3=_tmp61A,((_tmpC27.f4=_tmp61B,((_tmpC27.f5=_tmp61C,_tmpC27)))))))))))),_tmpC26)))),_tmpC28)))),_tmpC25)))),((_tmpC29->tl=_tmp61D.tms,_tmpC29)))))),_tmpC24)))));}
# 2009
break;};}case 182: _LL2E0: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2010 "parse.y"
struct Cyc_Parse_Declarator _tmp624=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC3E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC3D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC3C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC3B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC3A;struct Cyc_Parse_Declarator _tmpC39;yyval=Cyc_YY30(((_tmpC39.id=_tmp624.id,((_tmpC39.tms=(
(_tmpC3E=_region_malloc(yyr,sizeof(*_tmpC3E)),((_tmpC3E->hd=(void*)((_tmpC3A=_region_malloc(yyr,sizeof(*_tmpC3A)),((_tmpC3A[0]=((_tmpC3D.tag=3,((_tmpC3D.f1=(void*)((_tmpC3B=_region_malloc(yyr,sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3C.tag=1,((_tmpC3C.f1=0,((_tmpC3C.f2=0,((_tmpC3C.f3=0,((_tmpC3C.f4=
# 2014
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC3C.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC3C)))))))))))),_tmpC3B)))),_tmpC3D)))),_tmpC3A)))),((_tmpC3E->tl=_tmp624.tms,_tmpC3E)))))),_tmpC39)))));}
# 2017
break;}case 183: _LL2E1: {
# 2019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2018 "parse.y"
struct Cyc_Parse_Declarator _tmp62B=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC53;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC52;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC51;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC50;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC4F;struct Cyc_Parse_Declarator _tmpC4E;yyval=Cyc_YY30(((_tmpC4E.id=_tmp62B.id,((_tmpC4E.tms=((_tmpC53=_region_malloc(yyr,sizeof(*_tmpC53)),((_tmpC53->hd=(void*)((_tmpC4F=_region_malloc(yyr,sizeof(*_tmpC4F)),((_tmpC4F[0]=((_tmpC52.tag=3,((_tmpC52.f1=(void*)((_tmpC50=_region_malloc(yyr,sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=0,((_tmpC51.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC51.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC51)))))),_tmpC50)))),_tmpC52)))),_tmpC4F)))),((_tmpC53->tl=_tmp62B.tms,_tmpC53)))))),_tmpC4E)))));}
break;}case 184: _LL2E2: {
# 2022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2022 "parse.y"
struct Cyc_List_List*_tmp632=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp633=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC5D;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC5C;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC5B;struct Cyc_Parse_Declarator _tmpC5A;yyval=Cyc_YY30(((_tmpC5A.id=_tmp633.id,((_tmpC5A.tms=((_tmpC5D=_region_malloc(yyr,sizeof(*_tmpC5D)),((_tmpC5D->hd=(void*)((_tmpC5B=_region_malloc(yyr,sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5C.tag=4,((_tmpC5C.f1=_tmp632,((_tmpC5C.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC5C.f3=0,_tmpC5C)))))))),_tmpC5B)))),((_tmpC5D->tl=_tmp633.tms,_tmpC5D)))))),_tmpC5A)))));}
# 2026
break;}case 185: _LL2E3: {
# 2028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2027 "parse.y"
struct Cyc_Parse_Declarator _tmp638=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC67;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC66;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC65;struct Cyc_Parse_Declarator _tmpC64;yyval=Cyc_YY30(((_tmpC64.id=_tmp638.id,((_tmpC64.tms=((_tmpC67=_region_malloc(yyr,sizeof(*_tmpC67)),((_tmpC67->hd=(void*)((_tmpC65=_region_malloc(yyr,sizeof(*_tmpC65)),((_tmpC65[0]=((_tmpC66.tag=5,((_tmpC66.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC66.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC66)))))),_tmpC65)))),((_tmpC67->tl=_tmp638.tms,_tmpC67)))))),_tmpC64)))));}
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
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC6D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC6C;struct Cyc_List_List*_tmpC6B;ans=((_tmpC6B=_region_malloc(yyr,sizeof(*_tmpC6B)),((_tmpC6B->hd=(void*)((_tmpC6D=_region_malloc(yyr,sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6C.tag=5,((_tmpC6C.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC6C.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC6C)))))),_tmpC6D)))),((_tmpC6B->tl=ans,_tmpC6B))))));}{
# 2049
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp640=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;struct _tuple21 _tmp641=_tmp640;_tmp642=_tmp641.f1;_tmp643=_tmp641.f2;_tmp644=_tmp641.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC6E;ptrloc=((_tmpC6E=_cycalloc_atomic(sizeof(*_tmpC6E)),((_tmpC6E->ptr_loc=_tmp642,((_tmpC6E->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC6E->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC6E))))))));}{
# 2054
struct _tuple14 _tmp646=Cyc_Parse_collapse_pointer_quals(_tmp642,_tmp643,_tmp644,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;void*_tmp64B;struct _tuple14 _tmp647=_tmp646;_tmp648=_tmp647.f1;_tmp649=_tmp647.f2;_tmp64A=_tmp647.f3;_tmp64B=_tmp647.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC78;struct Cyc_Absyn_PtrAtts _tmpC77;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC76;struct Cyc_List_List*_tmpC75;ans=((_tmpC75=_region_malloc(yyr,sizeof(*_tmpC75)),((_tmpC75->hd=(void*)((_tmpC78=_region_malloc(yyr,sizeof(*_tmpC78)),((_tmpC78[0]=((_tmpC76.tag=2,((_tmpC76.f1=((_tmpC77.rgn=_tmp64B,((_tmpC77.nullable=_tmp648,((_tmpC77.bounds=_tmp649,((_tmpC77.zero_term=_tmp64A,((_tmpC77.ptrloc=ptrloc,_tmpC77)))))))))),((_tmpC76.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC76)))))),_tmpC78)))),((_tmpC75->tl=ans,_tmpC75))))));}
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
{struct Cyc_List_List*_tmpC79;yyval=Cyc_YY57(((_tmpC79=_region_malloc(yyr,sizeof(*_tmpC79)),((_tmpC79->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC79->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC79)))))));}
break;}case 191: _LL2E9: {
# 2064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2066 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC7C;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC7B;yyval=Cyc_YY56((void*)((_tmpC7B=_region_malloc(yyr,sizeof(*_tmpC7B)),((_tmpC7B[0]=((_tmpC7C.tag=0,((_tmpC7C.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC7C)))),_tmpC7B)))));}
break;}case 192: _LL2EA: {
# 2069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2068 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC7F;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC7E;yyval=Cyc_YY56((void*)((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=1,((_tmpC7F.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC7F)))),_tmpC7E)))));}
break;}case 193: _LL2EB: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2070 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC82;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC81;yyval=Cyc_YY56((void*)((_tmpC81=_region_malloc(yyr,sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC82.tag=2,_tmpC82)),_tmpC81)))));}
break;}case 194: _LL2EC: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC85;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC84;yyval=Cyc_YY56((void*)((_tmpC84=_region_malloc(yyr,sizeof(*_tmpC84)),((_tmpC84[0]=((_tmpC85.tag=3,_tmpC85)),_tmpC84)))));}
break;}case 195: _LL2ED: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC88;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC87;yyval=Cyc_YY56((void*)((_tmpC87=_region_malloc(yyr,sizeof(*_tmpC87)),((_tmpC87[0]=((_tmpC88.tag=4,_tmpC88)),_tmpC87)))));}
break;}case 196: _LL2EE: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC8B;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC8A;yyval=Cyc_YY56((void*)((_tmpC8A=_region_malloc(yyr,sizeof(*_tmpC8A)),((_tmpC8A[0]=((_tmpC8B.tag=5,_tmpC8B)),_tmpC8A)))));}
break;}case 197: _LL2EF: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2078 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC8E;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC8D;yyval=Cyc_YY56((void*)((_tmpC8D=_region_malloc(yyr,sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=6,_tmpC8E)),_tmpC8D)))));}
break;}case 198: _LL2F0: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2080 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC91;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC90;yyval=Cyc_YY56((void*)((_tmpC90=_region_malloc(yyr,sizeof(*_tmpC90)),((_tmpC90[0]=((_tmpC91.tag=7,_tmpC91)),_tmpC90)))));}
break;}case 199: _LL2F1: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2086 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC92;yyval=Cyc_YY1(((_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->f1=loc,((_tmpC92->f2=Cyc_Absyn_true_conref,((_tmpC92->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC92)))))))));}
# 2089
break;}case 200: _LL2F2: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2090 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC93;yyval=Cyc_YY1(((_tmpC93=_cycalloc(sizeof(*_tmpC93)),((_tmpC93->f1=loc,((_tmpC93->f2=Cyc_Absyn_false_conref,((_tmpC93->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC93)))))))));}
# 2093
break;}case 201: _LL2F3: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2094 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC94;yyval=Cyc_YY1(((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->f1=loc,((_tmpC94->f2=Cyc_Absyn_true_conref,((_tmpC94->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC94)))))))));}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC97;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC96;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96[0]=((_tmpC97.tag=1,((_tmpC97.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC97)))),_tmpC96))))));}
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
{struct _tuple26*_tmpC98;yyval=Cyc_YY42(((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC98->f2=0,((_tmpC98->f3=0,((_tmpC98->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC98->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC98)))))))))))));}
break;}case 213: _LL2FF: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2124 "parse.y"
{struct _tuple26*_tmpC99;yyval=Cyc_YY42(((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC99->f2=1,((_tmpC99->f3=0,((_tmpC99->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC99->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC99)))))))))))));}
break;}case 214: _LL300: {
# 2127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2127 "parse.y"
struct _tuple8*_tmp668=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp66A;struct Cyc_Absyn_Tqual _tmp66B;void*_tmp66C;struct _tuple8*_tmp669=_tmp668;_tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;_tmp66C=_tmp669->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC9A;struct Cyc_Absyn_VarargInfo*_tmp66D=(_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->name=_tmp66A,((_tmpC9A->tq=_tmp66B,((_tmpC9A->type=_tmp66C,((_tmpC9A->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC9A)))))))));
{struct _tuple26*_tmpC9B;yyval=Cyc_YY42(((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->f1=0,((_tmpC9B->f2=0,((_tmpC9B->f3=_tmp66D,((_tmpC9B->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC9B->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC9B)))))))))))));}
# 2131
break;};}case 215: _LL301: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2133 "parse.y"
struct _tuple8*_tmp670=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp672;struct Cyc_Absyn_Tqual _tmp673;void*_tmp674;struct _tuple8*_tmp671=_tmp670;_tmp672=_tmp671->f1;_tmp673=_tmp671->f2;_tmp674=_tmp671->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC9C;struct Cyc_Absyn_VarargInfo*_tmp675=(_tmpC9C=_cycalloc(sizeof(*_tmpC9C)),((_tmpC9C->name=_tmp672,((_tmpC9C->tq=_tmp673,((_tmpC9C->type=_tmp674,((_tmpC9C->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC9C)))))))));
{struct _tuple26*_tmpC9D;yyval=Cyc_YY42(((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC9D->f2=0,((_tmpC9D->f3=_tmp675,((_tmpC9D->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC9D->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC9D)))))))))))));}
# 2137
break;};}case 216: _LL302: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2141 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCA0;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC9F;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=1,((_tmpCA0.f1=0,_tmpCA0)))),_tmpC9F))))));}
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
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCA3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCA2;yyval=Cyc_YY52((void*)((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2[0]=((_tmpCA3.tag=24,((_tmpCA3.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCA3)))),_tmpCA2)))));}
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
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCA6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCA5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp67C=(_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=2,((_tmpCA6.f1=0,((_tmpCA6.f2=& Cyc_Tcutil_trk,_tmpCA6)))))),_tmpCA5)));
struct _dyneither_ptr _tmp67D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp67E=Cyc_Parse_id2type(_tmp67D,(void*)_tmp67C);
{struct _tuple29*_tmpCB0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCAF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCAE;struct Cyc_List_List*_tmpCAD;yyval=Cyc_YY53(((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->f1=(void*)((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE[0]=((_tmpCAF.tag=24,((_tmpCAF.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCAF)))),_tmpCAE)))),((_tmpCB0->f2=_tmp67E,_tmpCB0)))))),((_tmpCAD->tl=0,_tmpCAD)))))));}
# 2164
break;}case 223: _LL309: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2166 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCB3;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp685=(_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB3.tag=2,((_tmpCB3.f1=0,((_tmpCB3.f2=& Cyc_Tcutil_trk,_tmpCB3)))))),_tmpCB2)));
struct _dyneither_ptr _tmp686=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp687=Cyc_Parse_id2type(_tmp686,(void*)_tmp685);
{struct _tuple29*_tmpCBD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCBC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCBB;struct Cyc_List_List*_tmpCBA;yyval=Cyc_YY53(((_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->hd=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->f1=(void*)((_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB[0]=((_tmpCBC.tag=24,((_tmpCBC.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCBC)))),_tmpCBB)))),((_tmpCBD->f2=_tmp687,_tmpCBD)))))),((_tmpCBA->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCBA)))))));}
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
{const char*_tmpCBE;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCBE="inject",_tag_dyneither(_tmpCBE,sizeof(char),7))))!= 0){
const char*_tmpCBF;Cyc_Parse_err(((_tmpCBF="missing type in function declaration",_tag_dyneither(_tmpCBF,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
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
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCC5;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCC4;struct Cyc_List_List*_tmpCC3;yyval=Cyc_YY43(((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->hd=(void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC4.tag=25,((_tmpCC4.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCC4)))),_tmpCC5)))),((_tmpCC3->tl=0,_tmpCC3)))))));}
break;}case 231: _LL311: {
# 2195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2194 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCC6;yyval=Cyc_YY43(((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCC6->tl=0,_tmpCC6)))))));}
# 2197
break;}case 232: _LL312: {
# 2199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCCC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCCB;struct Cyc_List_List*_tmpCCA;yyval=Cyc_YY43(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->hd=(void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCB.tag=23,((_tmpCCB.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpCCB)))),_tmpCCC)))),((_tmpCCA->tl=0,_tmpCCA)))))));}
# 2205
break;}case 233: _LL313: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2206 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCD2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCD1;struct Cyc_List_List*_tmpCD0;yyval=Cyc_YY43(((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=(void*)((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD1.tag=23,((_tmpCD1.f1=Cyc_yyget_YY47(yyyyvsp[0]),_tmpCD1)))),_tmpCD2)))),((_tmpCD0->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCD0)))))));}
# 2209
break;}case 234: _LL314: {
# 2211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCD3;yyval=Cyc_YY41(((_tmpCD3=_cycalloc(sizeof(*_tmpCD3)),((_tmpCD3->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpCD3->tl=0,_tmpCD3)))))));}
break;}case 235: _LL315: {
# 2217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2216 "parse.y"
{struct Cyc_List_List*_tmpCD4;yyval=Cyc_YY41(((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpCD4->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCD4)))))));}
break;}case 236: _LL316: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp69C=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp69E;struct Cyc_Parse_Type_specifier _tmp69F;struct Cyc_List_List*_tmp6A0;struct _tuple25 _tmp69D=_tmp69C;_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;_tmp6A0=_tmp69D.f3;
if(_tmp69E.loc == 0)_tmp69E.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6A1=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp6A3;struct Cyc_List_List*_tmp6A4;struct Cyc_Parse_Declarator _tmp6A2=_tmp6A1;_tmp6A3=_tmp6A2.id;_tmp6A4=_tmp6A2.tms;{
void*_tmp6A5=Cyc_Parse_speclist2typ(_tmp69F,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6A6=Cyc_Parse_apply_tms(_tmp69E,_tmp6A5,_tmp6A0,_tmp6A4);struct Cyc_Absyn_Tqual _tmp6A8;void*_tmp6A9;struct Cyc_List_List*_tmp6AA;struct Cyc_List_List*_tmp6AB;struct _tuple13 _tmp6A7=_tmp6A6;_tmp6A8=_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;_tmp6AA=_tmp6A7.f3;_tmp6AB=_tmp6A7.f4;
if(_tmp6AA != 0){
const char*_tmpCD5;Cyc_Parse_err(((_tmpCD5="parameter with bad type params",_tag_dyneither(_tmpCD5,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6A3)){
const char*_tmpCD6;Cyc_Parse_err(((_tmpCD6="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCD6,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6A3).f2;
if(_tmp6AB != 0){
const char*_tmpCD9;void*_tmpCD8;(_tmpCD8=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCD9="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCD9,sizeof(char),40))),_tag_dyneither(_tmpCD8,sizeof(void*),0)));}
{struct _tuple8*_tmpCDA;yyval=Cyc_YY40(((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->f1=idopt,((_tmpCDA->f2=_tmp6A8,((_tmpCDA->f3=_tmp6A9,_tmpCDA)))))))));}
# 2236
break;};};};}case 237: _LL317: {
# 2238
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2240
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2237 "parse.y"
struct _tuple25 _tmp6B1=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6B3;struct Cyc_Parse_Type_specifier _tmp6B4;struct Cyc_List_List*_tmp6B5;struct _tuple25 _tmp6B2=_tmp6B1;_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B2.f3;
if(_tmp6B3.loc == 0)_tmp6B3.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6B6=Cyc_Parse_speclist2typ(_tmp6B4,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6B5 != 0){
const char*_tmpCDD;void*_tmpCDC;(_tmpCDC=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCDD="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCDD,sizeof(char),38))),_tag_dyneither(_tmpCDC,sizeof(void*),0)));}
{struct _tuple8*_tmpCDE;yyval=Cyc_YY40(((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->f1=0,((_tmpCDE->f2=_tmp6B3,((_tmpCDE->f3=_tmp6B6,_tmpCDE)))))))));}
# 2244
break;};}case 238: _LL318: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2245 "parse.y"
struct _tuple25 _tmp6BA=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6BC;struct Cyc_Parse_Type_specifier _tmp6BD;struct Cyc_List_List*_tmp6BE;struct _tuple25 _tmp6BB=_tmp6BA;_tmp6BC=_tmp6BB.f1;_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;
if(_tmp6BC.loc == 0)_tmp6BC.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6BF=Cyc_Parse_speclist2typ(_tmp6BD,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6C0=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6C1=Cyc_Parse_apply_tms(_tmp6BC,_tmp6BF,_tmp6BE,_tmp6C0);struct Cyc_Absyn_Tqual _tmp6C3;void*_tmp6C4;struct Cyc_List_List*_tmp6C5;struct Cyc_List_List*_tmp6C6;struct _tuple13 _tmp6C2=_tmp6C1;_tmp6C3=_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_tmp6C5=_tmp6C2.f3;_tmp6C6=_tmp6C2.f4;
if(_tmp6C5 != 0){
const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE1="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCE1,sizeof(char),49))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}
# 2253
if(_tmp6C6 != 0){
const char*_tmpCE4;void*_tmpCE3;(_tmpCE3=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCE4="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCE4,sizeof(char),38))),_tag_dyneither(_tmpCE3,sizeof(void*),0)));}
{struct _tuple8*_tmpCE5;yyval=Cyc_YY40(((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->f1=0,((_tmpCE5->f2=_tmp6C3,((_tmpCE5->f3=_tmp6C4,_tmpCE5)))))))));}
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
{struct _dyneither_ptr*_tmpCE8;struct Cyc_List_List*_tmpCE7;yyval=Cyc_YY39(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCE8)))),((_tmpCE7->tl=0,_tmpCE7)))))));}
break;}case 241: _LL31B: {
# 2268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2267 "parse.y"
{struct _dyneither_ptr*_tmpCEB;struct Cyc_List_List*_tmpCEA;yyval=Cyc_YY39(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCEB)))),((_tmpCEA->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCEA)))))));}
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
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCEE;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCED;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED[0]=((_tmpCEE.tag=35,((_tmpCEE.f1=0,((_tmpCEE.f2=0,_tmpCEE)))))),_tmpCED)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31F: {
# 2280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2279 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCF1;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCF0;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0[0]=((_tmpCF1.tag=35,((_tmpCF1.f1=0,((_tmpCF1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCF1)))))),_tmpCF0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL320: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2281 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCF4;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCF3;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3[0]=((_tmpCF4.tag=35,((_tmpCF4.f1=0,((_tmpCF4.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCF4)))))),_tmpCF3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 247: _LL321: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2283 "parse.y"
struct _dyneither_ptr*_tmpCF7;struct _tuple0*_tmpCF6;struct Cyc_Absyn_Vardecl*_tmp6D6=Cyc_Absyn_new_vardecl(((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=Cyc_Absyn_Loc_n,((_tmpCF6->f2=((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCF7)))),_tmpCF6)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2286
(_tmp6D6->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCFA;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCF9;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9[0]=((_tmpCFA.tag=26,((_tmpCFA.f1=_tmp6D6,((_tmpCFA.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCFA.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCFA.f4=0,_tmpCFA)))))))))),_tmpCF9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2289
break;}case 248: _LL322: {
# 2291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2290 "parse.y"
void*_tmp6DB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCFD;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCFC;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC[0]=((_tmpCFD.tag=27,((_tmpCFD.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCFD.f2=_tmp6DB,((_tmpCFD.f3=0,_tmpCFD)))))))),_tmpCFC)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2293
break;}case 249: _LL323: {
# 2295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD00;struct Cyc_List_List*_tmpCFF;yyval=Cyc_YY7(((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->hd=((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->f1=0,((_tmpD00->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpD00)))))),((_tmpCFF->tl=0,_tmpCFF)))))));}
break;}case 250: _LL324: {
# 2301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2300 "parse.y"
{struct _tuple30*_tmpD03;struct Cyc_List_List*_tmpD02;yyval=Cyc_YY7(((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->hd=((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD03->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpD03)))))),((_tmpD02->tl=0,_tmpD02)))))));}
break;}case 251: _LL325: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2302 "parse.y"
{struct _tuple30*_tmpD06;struct Cyc_List_List*_tmpD05;yyval=Cyc_YY7(((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->f1=0,((_tmpD06->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD06)))))),((_tmpD05->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpD05)))))));}
break;}case 252: _LL326: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2304 "parse.y"
{struct _tuple30*_tmpD09;struct Cyc_List_List*_tmpD08;yyval=Cyc_YY7(((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD09->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpD09)))))),((_tmpD08->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpD08)))))));}
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
{struct Cyc_List_List*_tmpD0A;yyval=Cyc_YY44(((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpD0A->tl=0,_tmpD0A)))))));}
break;}case 255: _LL329: {
# 2316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2314 "parse.y"
{struct Cyc_List_List*_tmpD0B;yyval=Cyc_YY44(((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpD0B->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpD0B)))))));}
break;}case 256: _LL32A: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2318 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD0E;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD0D;yyval=Cyc_YY45((void*)((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD0E.tag=0,((_tmpD0E.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpD0E)))),_tmpD0D)))));}
break;}case 257: _LL32B: {
# 2321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2319 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD14;struct _dyneither_ptr*_tmpD13;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD12;yyval=Cyc_YY45((void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD14.tag=1,((_tmpD14.f1=((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD13)))),_tmpD14)))),_tmpD12)))));}
break;}case 258: _LL32C: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2324 "parse.y"
struct _tuple25 _tmp6ED=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6EF;struct Cyc_Parse_Type_specifier _tmp6F0;struct Cyc_List_List*_tmp6F1;struct _tuple25 _tmp6EE=_tmp6ED;_tmp6EF=_tmp6EE.f1;_tmp6F0=_tmp6EE.f2;_tmp6F1=_tmp6EE.f3;{
void*_tmp6F2=Cyc_Parse_speclist2typ(_tmp6F0,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6F1 != 0){
const char*_tmpD17;void*_tmpD16;(_tmpD16=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD17="ignoring attributes in type",_tag_dyneither(_tmpD17,sizeof(char),28))),_tag_dyneither(_tmpD16,sizeof(void*),0)));}
{struct _tuple8*_tmpD18;yyval=Cyc_YY40(((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->f1=0,((_tmpD18->f2=_tmp6EF,((_tmpD18->f3=_tmp6F2,_tmpD18)))))))));}
# 2330
break;};}case 259: _LL32D: {
# 2332
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2331 "parse.y"
struct _tuple25 _tmp6F6=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6F8;struct Cyc_Parse_Type_specifier _tmp6F9;struct Cyc_List_List*_tmp6FA;struct _tuple25 _tmp6F7=_tmp6F6;_tmp6F8=_tmp6F7.f1;_tmp6F9=_tmp6F7.f2;_tmp6FA=_tmp6F7.f3;{
void*_tmp6FB=Cyc_Parse_speclist2typ(_tmp6F9,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6FC=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6FD=Cyc_Parse_apply_tms(_tmp6F8,_tmp6FB,_tmp6FA,_tmp6FC);
if(_tmp6FD.f3 != 0){
# 2337
const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD1B="bad type params, ignoring",_tag_dyneither(_tmpD1B,sizeof(char),26))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}
if(_tmp6FD.f4 != 0){
const char*_tmpD1E;void*_tmpD1D;(_tmpD1D=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD1E="bad specifiers, ignoring",_tag_dyneither(_tmpD1E,sizeof(char),25))),_tag_dyneither(_tmpD1D,sizeof(void*),0)));}
{struct _tuple8*_tmpD1F;yyval=Cyc_YY40(((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->f1=0,((_tmpD1F->f2=_tmp6FD.f1,((_tmpD1F->f3=_tmp6FD.f2,_tmpD1F)))))))));}
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
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD22;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD21;yyval=Cyc_YY47((void*)((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD22.tag=24,((_tmpD22.f1=0,_tmpD22)))),_tmpD21)))));}
break;}case 262: _LL330: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2347 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD25;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD24;yyval=Cyc_YY47((void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD25.tag=24,((_tmpD25.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpD25)))),_tmpD24)))));}
break;}case 263: _LL331: {
# 2350
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2352
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2348 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD28;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD27;yyval=Cyc_YY47((void*)((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=((_tmpD28.tag=25,((_tmpD28.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpD28)))),_tmpD27)))));}
break;}case 264: _LL332: {
# 2351
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2353
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2349 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD2E;struct Cyc_List_List*_tmpD2D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD2C;yyval=Cyc_YY47((void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2E.tag=24,((_tmpD2E.f1=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD2D->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpD2D)))))),_tmpD2E)))),_tmpD2C)))));}
break;}case 265: _LL333: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD2F;yyval=Cyc_YY43(((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD2F->tl=0,_tmpD2F)))))));}
break;}case 266: _LL334: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2356 "parse.y"
{struct Cyc_List_List*_tmpD30;yyval=Cyc_YY43(((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpD30->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpD30)))))));}
break;}case 267: _LL335: {
# 2359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2361 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD31;yyval=Cyc_YY33(((_tmpD31.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpD31)));}
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
{struct Cyc_Parse_Abstractdeclarator _tmpD32;yyval=Cyc_YY33(((_tmpD32.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpD32)));}
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
{struct Cyc_List_List*_tmpD3C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD3B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD3A;struct Cyc_Parse_Abstractdeclarator _tmpD39;yyval=Cyc_YY33(((_tmpD39.tms=((_tmpD3C=_region_malloc(yyr,sizeof(*_tmpD3C)),((_tmpD3C->hd=(void*)((_tmpD3A=_region_malloc(yyr,sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD3B.tag=0,((_tmpD3B.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD3B.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD3B)))))),_tmpD3A)))),((_tmpD3C->tl=0,_tmpD3C)))))),_tmpD39)));}
break;}case 272: _LL33A: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2374 "parse.y"
{struct Cyc_List_List*_tmpD46;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD45;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD44;struct Cyc_Parse_Abstractdeclarator _tmpD43;yyval=Cyc_YY33(((_tmpD43.tms=((_tmpD46=_region_malloc(yyr,sizeof(*_tmpD46)),((_tmpD46->hd=(void*)((_tmpD44=_region_malloc(yyr,sizeof(*_tmpD44)),((_tmpD44[0]=((_tmpD45.tag=0,((_tmpD45.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD45.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD45)))))),_tmpD44)))),((_tmpD46->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD46)))))),_tmpD43)));}
break;}case 273: _LL33B: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2376 "parse.y"
{struct Cyc_List_List*_tmpD50;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD4F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD4E;struct Cyc_Parse_Abstractdeclarator _tmpD4D;yyval=Cyc_YY33(((_tmpD4D.tms=((_tmpD50=_region_malloc(yyr,sizeof(*_tmpD50)),((_tmpD50->hd=(void*)((_tmpD4E=_region_malloc(yyr,sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=1,((_tmpD4F.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD4F.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD4F.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD4F)))))))),_tmpD4E)))),((_tmpD50->tl=0,_tmpD50)))))),_tmpD4D)));}
break;}case 274: _LL33C: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2378 "parse.y"
{struct Cyc_List_List*_tmpD5A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD59;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD58;struct Cyc_Parse_Abstractdeclarator _tmpD57;yyval=Cyc_YY33(((_tmpD57.tms=((_tmpD5A=_region_malloc(yyr,sizeof(*_tmpD5A)),((_tmpD5A->hd=(void*)((_tmpD58=_region_malloc(yyr,sizeof(*_tmpD58)),((_tmpD58[0]=((_tmpD59.tag=1,((_tmpD59.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD59.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD59.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD59)))))))),_tmpD58)))),((_tmpD5A->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD5A)))))),_tmpD57)));}
# 2381
break;}case 275: _LL33D: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2382 "parse.y"
{struct Cyc_List_List*_tmpD6F;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD6E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD6D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD6B;struct Cyc_Parse_Abstractdeclarator _tmpD6A;yyval=Cyc_YY33(((_tmpD6A.tms=((_tmpD6F=_region_malloc(yyr,sizeof(*_tmpD6F)),((_tmpD6F->hd=(void*)((_tmpD6B=_region_malloc(yyr,sizeof(*_tmpD6B)),((_tmpD6B[0]=((_tmpD6E.tag=3,((_tmpD6E.f1=(void*)((_tmpD6C=_region_malloc(yyr,sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=1,((_tmpD6D.f1=0,((_tmpD6D.f2=0,((_tmpD6D.f3=0,((_tmpD6D.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD6D.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD6D)))))))))))),_tmpD6C)))),_tmpD6E)))),_tmpD6B)))),((_tmpD6F->tl=0,_tmpD6F)))))),_tmpD6A)));}
# 2384
break;}case 276: _LL33E: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2385 "parse.y"
struct _tuple26*_tmp726=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp728;int _tmp729;struct Cyc_Absyn_VarargInfo*_tmp72A;void*_tmp72B;struct Cyc_List_List*_tmp72C;struct _tuple26*_tmp727=_tmp726;_tmp728=_tmp727->f1;_tmp729=_tmp727->f2;_tmp72A=_tmp727->f3;_tmp72B=_tmp727->f4;_tmp72C=_tmp727->f5;
{struct Cyc_List_List*_tmpD84;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD83;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD80;struct Cyc_Parse_Abstractdeclarator _tmpD7F;yyval=Cyc_YY33(((_tmpD7F.tms=((_tmpD84=_region_malloc(yyr,sizeof(*_tmpD84)),((_tmpD84->hd=(void*)((_tmpD80=_region_malloc(yyr,sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD83.tag=3,((_tmpD83.f1=(void*)((_tmpD81=_region_malloc(yyr,sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=1,((_tmpD82.f1=_tmp728,((_tmpD82.f2=_tmp729,((_tmpD82.f3=_tmp72A,((_tmpD82.f4=_tmp72B,((_tmpD82.f5=_tmp72C,_tmpD82)))))))))))),_tmpD81)))),_tmpD83)))),_tmpD80)))),((_tmpD84->tl=0,_tmpD84)))))),_tmpD7F)));}
# 2388
break;}case 277: _LL33F: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2389 "parse.y"
{struct Cyc_List_List*_tmpD99;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD98;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD97;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD96;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD95;struct Cyc_Parse_Abstractdeclarator _tmpD94;yyval=Cyc_YY33(((_tmpD94.tms=((_tmpD99=_region_malloc(yyr,sizeof(*_tmpD99)),((_tmpD99->hd=(void*)((_tmpD95=_region_malloc(yyr,sizeof(*_tmpD95)),((_tmpD95[0]=((_tmpD98.tag=3,((_tmpD98.f1=(void*)((_tmpD96=_region_malloc(yyr,sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=1,((_tmpD97.f1=0,((_tmpD97.f2=0,((_tmpD97.f3=0,((_tmpD97.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD97.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD97)))))))))))),_tmpD96)))),_tmpD98)))),_tmpD95)))),((_tmpD99->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD99)))))),_tmpD94)));}
# 2392
break;}case 278: _LL340: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2393 "parse.y"
struct _tuple26*_tmp739=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp73B;int _tmp73C;struct Cyc_Absyn_VarargInfo*_tmp73D;void*_tmp73E;struct Cyc_List_List*_tmp73F;struct _tuple26*_tmp73A=_tmp739;_tmp73B=_tmp73A->f1;_tmp73C=_tmp73A->f2;_tmp73D=_tmp73A->f3;_tmp73E=_tmp73A->f4;_tmp73F=_tmp73A->f5;
{struct Cyc_List_List*_tmpDAE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDAD;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDAC;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDAB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDAA;struct Cyc_Parse_Abstractdeclarator _tmpDA9;yyval=Cyc_YY33(((_tmpDA9.tms=((_tmpDAE=_region_malloc(yyr,sizeof(*_tmpDAE)),((_tmpDAE->hd=(void*)((_tmpDAA=_region_malloc(yyr,sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDAD.tag=3,((_tmpDAD.f1=(void*)((_tmpDAB=_region_malloc(yyr,sizeof(*_tmpDAB)),((_tmpDAB[0]=((_tmpDAC.tag=1,((_tmpDAC.f1=_tmp73B,((_tmpDAC.f2=_tmp73C,((_tmpDAC.f3=_tmp73D,((_tmpDAC.f4=_tmp73E,((_tmpDAC.f5=_tmp73F,_tmpDAC)))))))))))),_tmpDAB)))),_tmpDAD)))),_tmpDAA)))),((_tmpDAE->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDAE)))))),_tmpDA9)));}
# 2397
break;}case 279: _LL341: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2399 "parse.y"
struct Cyc_List_List*_tmp746=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDB8;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDB7;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDB6;struct Cyc_Parse_Abstractdeclarator _tmpDB5;yyval=Cyc_YY33(((_tmpDB5.tms=((_tmpDB8=_region_malloc(yyr,sizeof(*_tmpDB8)),((_tmpDB8->hd=(void*)((_tmpDB6=_region_malloc(yyr,sizeof(*_tmpDB6)),((_tmpDB6[0]=((_tmpDB7.tag=4,((_tmpDB7.f1=_tmp746,((_tmpDB7.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDB7.f3=0,_tmpDB7)))))))),_tmpDB6)))),((_tmpDB8->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDB8)))))),_tmpDB5)));}
# 2403
break;}case 280: _LL342: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2404 "parse.y"
{struct Cyc_List_List*_tmpDC2;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDC1;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDC0;struct Cyc_Parse_Abstractdeclarator _tmpDBF;yyval=Cyc_YY33(((_tmpDBF.tms=((_tmpDC2=_region_malloc(yyr,sizeof(*_tmpDC2)),((_tmpDC2->hd=(void*)((_tmpDC0=_region_malloc(yyr,sizeof(*_tmpDC0)),((_tmpDC0[0]=((_tmpDC1.tag=5,((_tmpDC1.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDC1.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpDC1)))))),_tmpDC0)))),((_tmpDC2->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDC2)))))),_tmpDBF)));}
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
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDC5;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDC4;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC5.tag=16,((_tmpDC5.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpDC5)))),_tmpDC4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 288: _LL34A:
# 2421
 yyval=Cyc_YY5(0);
break;case 289: _LL34B: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2424 "parse.y"
{const char*_tmpDC6;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDC6="open",_tag_dyneither(_tmpDC6,sizeof(char),5))))!= 0){
const char*_tmpDC7;Cyc_Parse_err(((_tmpDC7="expecting `open'",_tag_dyneither(_tmpDC7,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2428
break;}case 290: _LL34C: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDCD;struct _dyneither_ptr*_tmpDCC;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDCB;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCD.tag=13,((_tmpDCD.f1=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDCC)))),((_tmpDCD.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpDCD)))))),_tmpDCB)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDD3;struct _dyneither_ptr*_tmpDD2;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDD1;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1[0]=((_tmpDD3.tag=13,((_tmpDD3.f1=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD2)))),((_tmpDD3.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDD3)))))),_tmpDD1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2453
break;}case 298: _LL354: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2453 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDD9;struct _dyneither_ptr*_tmpDD8;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDD7;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDD9.tag=13,((_tmpDD9.f1=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDD8)))),((_tmpDD9.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDD9)))))),_tmpDD7)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDDC;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDDB;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=1,((_tmpDDC.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDDC)))),_tmpDDB)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 302: _LL358: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2460 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDDF;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDDE;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=1,((_tmpDDF.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDDF)))),_tmpDDE)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
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
struct Cyc_Absyn_Exp*_tmp760=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp760,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL35D: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2480 "parse.y"
struct Cyc_Absyn_Exp*_tmp761=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp761,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
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
{struct Cyc_Absyn_Switch_clause*_tmpDE2;struct Cyc_List_List*_tmpDE1;yyval=Cyc_YY11(((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->hd=((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDE2->pat_vars=0,((_tmpDE2->where_clause=0,((_tmpDE2->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDE2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDE2)))))))))))),((_tmpDE1->tl=0,_tmpDE1)))))));}
# 2505
break;}case 311: _LL361: {
# 2507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2506 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDE5;struct Cyc_List_List*_tmpDE4;yyval=Cyc_YY11(((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->hd=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDE5->pat_vars=0,((_tmpDE5->where_clause=0,((_tmpDE5->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDE5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDE5)))))))))))),((_tmpDE4->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDE4)))))));}
break;}case 312: _LL362: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2510 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDE8;struct Cyc_List_List*_tmpDE7;yyval=Cyc_YY11(((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDE8->pat_vars=0,((_tmpDE8->where_clause=0,((_tmpDE8->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDE8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDE8)))))))))))),((_tmpDE7->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDE7)))))));}
break;}case 313: _LL363: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2512 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDEB;struct Cyc_List_List*_tmpDEA;yyval=Cyc_YY11(((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->hd=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDEB->pat_vars=0,((_tmpDEB->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDEB->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDEB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDEB)))))))))))),((_tmpDEA->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDEA)))))));}
break;}case 314: _LL364: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2516 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDEE;struct Cyc_List_List*_tmpDED;yyval=Cyc_YY11(((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->hd=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDEE->pat_vars=0,((_tmpDEE->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDEE->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDEE->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDEE)))))))))))),((_tmpDED->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDED)))))));}
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
struct Cyc_List_List*_tmp76C=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp76D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp76C,_tmp76D));
# 2560
break;}case 326: _LL370: {
# 2562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2561 "parse.y"
struct Cyc_List_List*_tmp76E=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp76E,_tmp76F));
# 2566
break;}case 327: _LL371: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2567 "parse.y"
struct Cyc_List_List*_tmp770=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp771=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp770,_tmp771));
# 2572
break;}case 328: _LL372: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2573 "parse.y"
struct Cyc_List_List*_tmp772=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp773=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp772,_tmp773));
# 2578
break;}case 329: _LL373: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDEF;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDEF)))),(unsigned int)(yyyylsp[0]).first_line));}
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
void*_tmp775=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp775,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
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
void*_tmp776=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp776,(unsigned int)(yyyylsp[0]).first_line)));
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
{void*_tmp777=e->r;void*_tmp778=_tmp777;enum Cyc_Absyn_Sign _tmp77A;char _tmp77B;enum Cyc_Absyn_Sign _tmp77D;short _tmp77E;enum Cyc_Absyn_Sign _tmp780;int _tmp781;struct _dyneither_ptr _tmp783;int _tmp784;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp779=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp779->tag != 0)goto _LL3AB;else{if(((_tmp779->f1).Char_c).tag != 2)goto _LL3AB;_tmp77A=((struct _tuple3)((_tmp779->f1).Char_c).val).f1;_tmp77B=((struct _tuple3)((_tmp779->f1).Char_c).val).f2;}}_LL3AA:
# 2723
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDF2;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDF1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF1=_cycalloc_atomic(sizeof(*_tmpDF1)),((_tmpDF1[0]=((_tmpDF2.tag=10,((_tmpDF2.f1=_tmp77B,_tmpDF2)))),_tmpDF1)))),e->loc));}goto _LL3A8;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp77C->tag != 0)goto _LL3AD;else{if(((_tmp77C->f1).Short_c).tag != 4)goto _LL3AD;_tmp77D=((struct _tuple4)((_tmp77C->f1).Short_c).val).f1;_tmp77E=((struct _tuple4)((_tmp77C->f1).Short_c).val).f2;}}_LL3AC:
# 2725
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDF5;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDF4;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF4=_cycalloc_atomic(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF5.tag=9,((_tmpDF5.f1=_tmp77D,((_tmpDF5.f2=(int)_tmp77E,_tmpDF5)))))),_tmpDF4)))),e->loc));}goto _LL3A8;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp77F->tag != 0)goto _LL3AF;else{if(((_tmp77F->f1).Int_c).tag != 5)goto _LL3AF;_tmp780=((struct _tuple5)((_tmp77F->f1).Int_c).val).f1;_tmp781=((struct _tuple5)((_tmp77F->f1).Int_c).val).f2;}}_LL3AE:
# 2727
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDF8;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDF7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF7=_cycalloc_atomic(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=9,((_tmpDF8.f1=_tmp780,((_tmpDF8.f2=_tmp781,_tmpDF8)))))),_tmpDF7)))),e->loc));}goto _LL3A8;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp782=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp782->tag != 0)goto _LL3B1;else{if(((_tmp782->f1).Float_c).tag != 7)goto _LL3B1;_tmp783=((struct _tuple7)((_tmp782->f1).Float_c).val).f1;_tmp784=((struct _tuple7)((_tmp782->f1).Float_c).val).f2;}}_LL3B0:
# 2729
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDFB;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDFA;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFB.tag=11,((_tmpDFB.f1=_tmp783,((_tmpDFB.f2=_tmp784,_tmpDFB)))))),_tmpDFA)))),e->loc));}goto _LL3A8;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp785=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp785->tag != 0)goto _LL3B3;else{if(((_tmp785->f1).Null_c).tag != 1)goto _LL3B3;}}_LL3B2:
# 2731
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A8;_LL3B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp786=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp786->tag != 0)goto _LL3B5;else{if(((_tmp786->f1).String_c).tag != 8)goto _LL3B5;}}_LL3B4:
# 2733
{const char*_tmpDFC;Cyc_Parse_err(((_tmpDFC="strings cannot occur within patterns",_tag_dyneither(_tmpDFC,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp787=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp787->tag != 0)goto _LL3B7;else{if(((_tmp787->f1).Wstring_c).tag != 9)goto _LL3B7;}}_LL3B6:
# 2735
{const char*_tmpDFD;Cyc_Parse_err(((_tmpDFD="strings cannot occur within patterns",_tag_dyneither(_tmpDFD,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp788=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp788->tag != 0)goto _LL3B9;else{if(((_tmp788->f1).LongLong_c).tag != 6)goto _LL3B9;}}_LL3B8:
# 2737
{const char*_tmpDFE;Cyc_Parse_unimp(((_tmpDFE="long long's in patterns",_tag_dyneither(_tmpDFE,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A8;_LL3B9:;_LL3BA: {
# 2739
const char*_tmpDFF;Cyc_Parse_err(((_tmpDFF="bad constant in case",_tag_dyneither(_tmpDFF,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A8:;}
# 2742
break;}case 381: _LL3A7: {
# 2744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2743 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE02;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE01;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01[0]=((_tmpE02.tag=14,((_tmpE02.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE02)))),_tmpE01)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 382: _LL3BB: {
# 2746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2745 "parse.y"
{const char*_tmpE03;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE03="as",_tag_dyneither(_tmpE03,sizeof(char),3))))!= 0){
const char*_tmpE04;Cyc_Parse_err(((_tmpE04="expecting `as'",_tag_dyneither(_tmpE04,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE0E;struct _dyneither_ptr*_tmpE0D;struct _tuple0*_tmpE0C;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE0B;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0E.tag=1,((_tmpE0E.f1=Cyc_Absyn_new_vardecl(((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->f1=Cyc_Absyn_Loc_n,((_tmpE0C->f2=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE0D)))),_tmpE0C)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE0E.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpE0E)))))),_tmpE0B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2750
break;}case 383: _LL3BC: {
# 2752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2751 "parse.y"
struct _tuple22 _tmp79D=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp79F;int _tmp7A0;struct _tuple22 _tmp79E=_tmp79D;_tmp79F=_tmp79E.f1;_tmp7A0=_tmp79E.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE11;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE10;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=4,((_tmpE11.f1=_tmp79F,((_tmpE11.f2=_tmp7A0,_tmpE11)))))),_tmpE10)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2754
break;}case 384: _LL3BD: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2755 "parse.y"
struct _tuple22 _tmp7A3=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp7A5;int _tmp7A6;struct _tuple22 _tmp7A4=_tmp7A3;_tmp7A5=_tmp7A4.f1;_tmp7A6=_tmp7A4.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE14;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE13;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=((_tmpE14.tag=15,((_tmpE14.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE14.f2=_tmp7A5,((_tmpE14.f3=_tmp7A6,_tmpE14)))))))),_tmpE13)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2758
break;}case 385: _LL3BE: {
# 2760
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2762
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2759 "parse.y"
struct _tuple22 _tmp7A9=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp7AB;int _tmp7AC;struct _tuple22 _tmp7AA=_tmp7A9;_tmp7AB=_tmp7AA.f1;_tmp7AC=_tmp7AA.f2;{
struct Cyc_List_List*_tmp7AD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE1A;struct Cyc_Absyn_AggrInfo*_tmpE19;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE18;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE1A.tag=6,((_tmpE1A.f1=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE19->targs=0,_tmpE19)))))),((_tmpE1A.f2=_tmp7AD,((_tmpE1A.f3=_tmp7AB,((_tmpE1A.f4=_tmp7AC,_tmpE1A)))))))))),_tmpE18)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2764
break;};}case 386: _LL3BF: {
# 2766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2765 "parse.y"
struct _tuple22 _tmp7B1=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp7B3;int _tmp7B4;struct _tuple22 _tmp7B2=_tmp7B1;_tmp7B3=_tmp7B2.f1;_tmp7B4=_tmp7B2.f2;{
struct Cyc_List_List*_tmp7B5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE1D;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE1C;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=6,((_tmpE1D.f1=0,((_tmpE1D.f2=_tmp7B5,((_tmpE1D.f3=_tmp7B3,((_tmpE1D.f4=_tmp7B4,_tmpE1D)))))))))),_tmpE1C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2769
break;};}case 387: _LL3C0: {
# 2771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2770 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE20;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE1F;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE1F=_cycalloc(sizeof(*_tmpE1F)),((_tmpE1F[0]=((_tmpE20.tag=5,((_tmpE20.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE20)))),_tmpE1F)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3C1: {
# 2773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2772 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE2A;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE29;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE28;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE27;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE27=_cycalloc(sizeof(*_tmpE27)),((_tmpE27[0]=((_tmpE2A.tag=5,((_tmpE2A.f1=Cyc_Absyn_new_pat((void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE29.tag=5,((_tmpE29.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE29)))),_tmpE28)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE2A)))),_tmpE27)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3C2: {
# 2775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2774 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE34;struct _dyneither_ptr*_tmpE33;struct _tuple0*_tmpE32;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE31;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31[0]=((_tmpE34.tag=2,((_tmpE34.f1=Cyc_Absyn_new_vardecl(((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32->f1=Cyc_Absyn_Loc_n,((_tmpE32->f2=((_tmpE33=_cycalloc(sizeof(*_tmpE33)),((_tmpE33[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE33)))),_tmpE32)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE34.f2=
# 2776
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE34)))))),_tmpE31)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2778
break;}case 390: _LL3C3: {
# 2780
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2782
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2779 "parse.y"
{const char*_tmpE35;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE35="as",_tag_dyneither(_tmpE35,sizeof(char),3))))!= 0){
const char*_tmpE36;Cyc_Parse_err(((_tmpE36="expecting `as'",_tag_dyneither(_tmpE36,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE40;struct _dyneither_ptr*_tmpE3F;struct _tuple0*_tmpE3E;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE3D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE40.tag=2,((_tmpE40.f1=Cyc_Absyn_new_vardecl(((_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=Cyc_Absyn_Loc_n,((_tmpE3E->f2=((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE3F)))),_tmpE3E)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE40.f2=
# 2783
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE40)))))),_tmpE3D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2785
break;}case 391: _LL3C4: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2786 "parse.y"
void*_tmp7C8=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE51;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE50;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE4F;struct _dyneither_ptr*_tmpE4E;struct _tuple0*_tmpE4D;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE4C;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE51.tag=3,((_tmpE51.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7C8),((_tmpE51.f2=
Cyc_Absyn_new_vardecl(((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D->f1=Cyc_Absyn_Loc_n,((_tmpE4D->f2=((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE4E)))),_tmpE4D)))))),(void*)(
(_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=19,((_tmpE50.f1=_tmp7C8,_tmpE50)))),_tmpE4F)))),0),_tmpE51)))))),_tmpE4C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2791
break;}case 392: _LL3C5: {
# 2793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2792 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7CF=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE6D;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE6C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE6B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE6A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE69;struct _dyneither_ptr*_tmpE68;struct _tuple0*_tmpE67;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE66;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=((_tmpE6D.tag=3,((_tmpE6D.f1=_tmp7CF,((_tmpE6D.f2=
Cyc_Absyn_new_vardecl(((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67->f1=Cyc_Absyn_Loc_n,((_tmpE67->f2=((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE68)))),_tmpE67)))))),(void*)(
(_tmpE69=_cycalloc(sizeof(*_tmpE69)),((_tmpE69[0]=((_tmpE6C.tag=19,((_tmpE6C.f1=(void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((_tmpE6B.tag=2,((_tmpE6B.f1=_tmp7CF,_tmpE6B)))),_tmpE6A)))),_tmpE6C)))),_tmpE69)))),0),_tmpE6D)))))),_tmpE66)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2797
break;}case 393: _LL3C6: {
# 2799
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2801
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2800 "parse.y"
{struct _tuple22*_tmpE6E;yyval=Cyc_YY13(((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE6E->f2=0,_tmpE6E)))))));}
break;}case 394: _LL3C7: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2801 "parse.y"
{struct _tuple22*_tmpE6F;yyval=Cyc_YY13(((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE6F->f2=1,_tmpE6F)))))));}
break;}case 395: _LL3C8: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2802 "parse.y"
{struct _tuple22*_tmpE70;yyval=Cyc_YY13(((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->f1=0,((_tmpE70->f2=1,_tmpE70)))))));}
break;}case 396: _LL3C9: {
# 2805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2807 "parse.y"
{struct Cyc_List_List*_tmpE71;yyval=Cyc_YY14(((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE71->tl=0,_tmpE71)))))));}
break;}case 397: _LL3CA: {
# 2810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2809 "parse.y"
{struct Cyc_List_List*_tmpE72;yyval=Cyc_YY14(((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE72->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE72)))))));}
break;}case 398: _LL3CB: {
# 2812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2814 "parse.y"
{struct _tuple23*_tmpE73;yyval=Cyc_YY15(((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->f1=0,((_tmpE73->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE73)))))));}
break;}case 399: _LL3CC: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2816 "parse.y"
{struct _tuple23*_tmpE74;yyval=Cyc_YY15(((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE74->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE74)))))));}
break;}case 400: _LL3CD: {
# 2819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2819 "parse.y"
{struct _tuple22*_tmpE75;yyval=Cyc_YY17(((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE75->f2=0,_tmpE75)))))));}
break;}case 401: _LL3CE: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2820 "parse.y"
{struct _tuple22*_tmpE76;yyval=Cyc_YY17(((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE76->f2=1,_tmpE76)))))));}
break;}case 402: _LL3CF: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2821 "parse.y"
{struct _tuple22*_tmpE77;yyval=Cyc_YY17(((_tmpE77=_cycalloc(sizeof(*_tmpE77)),((_tmpE77->f1=0,((_tmpE77->f2=1,_tmpE77)))))));}
break;}case 403: _LL3D0: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2826 "parse.y"
{struct Cyc_List_List*_tmpE78;yyval=Cyc_YY16(((_tmpE78=_cycalloc(sizeof(*_tmpE78)),((_tmpE78->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE78->tl=0,_tmpE78)))))));}
break;}case 404: _LL3D1: {
# 2829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2828 "parse.y"
{struct Cyc_List_List*_tmpE79;yyval=Cyc_YY16(((_tmpE79=_cycalloc(sizeof(*_tmpE79)),((_tmpE79->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE79->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE79)))))));}
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
{struct Cyc_Core_Opt*_tmpE7A;yyval=Cyc_YY9(((_tmpE7A=_cycalloc_atomic(sizeof(*_tmpE7A)),((_tmpE7A->v=(void*)Cyc_Absyn_Times,_tmpE7A)))));}
break;}case 412: _LL3D9: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851 "parse.y"
{struct Cyc_Core_Opt*_tmpE7B;yyval=Cyc_YY9(((_tmpE7B=_cycalloc_atomic(sizeof(*_tmpE7B)),((_tmpE7B->v=(void*)Cyc_Absyn_Div,_tmpE7B)))));}
break;}case 413: _LL3DA: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
{struct Cyc_Core_Opt*_tmpE7C;yyval=Cyc_YY9(((_tmpE7C=_cycalloc_atomic(sizeof(*_tmpE7C)),((_tmpE7C->v=(void*)Cyc_Absyn_Mod,_tmpE7C)))));}
break;}case 414: _LL3DB: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853 "parse.y"
{struct Cyc_Core_Opt*_tmpE7D;yyval=Cyc_YY9(((_tmpE7D=_cycalloc_atomic(sizeof(*_tmpE7D)),((_tmpE7D->v=(void*)Cyc_Absyn_Plus,_tmpE7D)))));}
break;}case 415: _LL3DC: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_Core_Opt*_tmpE7E;yyval=Cyc_YY9(((_tmpE7E=_cycalloc_atomic(sizeof(*_tmpE7E)),((_tmpE7E->v=(void*)Cyc_Absyn_Minus,_tmpE7E)))));}
break;}case 416: _LL3DD: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855 "parse.y"
{struct Cyc_Core_Opt*_tmpE7F;yyval=Cyc_YY9(((_tmpE7F=_cycalloc_atomic(sizeof(*_tmpE7F)),((_tmpE7F->v=(void*)Cyc_Absyn_Bitlshift,_tmpE7F)))));}
break;}case 417: _LL3DE: {
# 2858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856 "parse.y"
{struct Cyc_Core_Opt*_tmpE80;yyval=Cyc_YY9(((_tmpE80=_cycalloc_atomic(sizeof(*_tmpE80)),((_tmpE80->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE80)))));}
break;}case 418: _LL3DF: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857 "parse.y"
{struct Cyc_Core_Opt*_tmpE81;yyval=Cyc_YY9(((_tmpE81=_cycalloc_atomic(sizeof(*_tmpE81)),((_tmpE81->v=(void*)Cyc_Absyn_Bitand,_tmpE81)))));}
break;}case 419: _LL3E0: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858 "parse.y"
{struct Cyc_Core_Opt*_tmpE82;yyval=Cyc_YY9(((_tmpE82=_cycalloc_atomic(sizeof(*_tmpE82)),((_tmpE82->v=(void*)Cyc_Absyn_Bitxor,_tmpE82)))));}
break;}case 420: _LL3E1: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2859 "parse.y"
{struct Cyc_Core_Opt*_tmpE83;yyval=Cyc_YY9(((_tmpE83=_cycalloc_atomic(sizeof(*_tmpE83)),((_tmpE83->v=(void*)Cyc_Absyn_Bitor,_tmpE83)))));}
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
void*_tmp7EE=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7EE,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
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
void*_tmp7EF=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7EF,(unsigned int)(yyyylsp[0]).first_line));
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
void*_tmp7F0=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7F0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2986
break;}case 469: _LL412: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2988 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE89;struct Cyc_Absyn_MallocInfo _tmpE88;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE87;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE87=_cycalloc(sizeof(*_tmpE87)),((_tmpE87[0]=((_tmpE89.tag=33,((_tmpE89.f1=((_tmpE88.is_calloc=0,((_tmpE88.rgn=0,((_tmpE88.elt_type=0,((_tmpE88.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE88.fat_result=0,_tmpE88)))))))))),_tmpE89)))),_tmpE87)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2990
break;}case 470: _LL413: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2991 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE8F;struct Cyc_Absyn_MallocInfo _tmpE8E;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE8D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D[0]=((_tmpE8F.tag=33,((_tmpE8F.f1=((_tmpE8E.is_calloc=0,((_tmpE8E.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE8E.elt_type=0,((_tmpE8E.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE8E.fat_result=0,_tmpE8E)))))))))),_tmpE8F)))),_tmpE8D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2993
break;}case 471: _LL414: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2994 "parse.y"
void*_tmp7F7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE99;void**_tmpE98;struct Cyc_Absyn_MallocInfo _tmpE97;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE96;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96[0]=((_tmpE99.tag=33,((_tmpE99.f1=((_tmpE97.is_calloc=1,((_tmpE97.rgn=0,((_tmpE97.elt_type=((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98[0]=_tmp7F7,_tmpE98)))),((_tmpE97.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE97.fat_result=0,_tmpE97)))))))))),_tmpE99)))),_tmpE96)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2997
break;}case 472: _LL415: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2999 "parse.y"
void*_tmp7FC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEA3;void**_tmpEA2;struct Cyc_Absyn_MallocInfo _tmpEA1;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEA0;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0[0]=((_tmpEA3.tag=33,((_tmpEA3.f1=((_tmpEA1.is_calloc=1,((_tmpEA1.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA1.elt_type=((_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2[0]=_tmp7FC,_tmpEA2)))),((_tmpEA1.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpEA1.fat_result=0,_tmpEA1)))))))))),_tmpEA3)))),_tmpEA0)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3002
break;}case 473: _LL416: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3003 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEA4[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEA4[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEA4,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL417: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3005 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEAA;struct _dyneither_ptr*_tmpEA9;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEA8;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=((_tmpEAA.tag=37,((_tmpEAA.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEAA.f2=((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEA9)))),_tmpEAA)))))),_tmpEA8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL418: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3007 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEB0;struct _dyneither_ptr*_tmpEAF;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEAE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=((_tmpEB0.tag=37,((_tmpEB0.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEB0.f2=((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEAF)))),_tmpEB0)))))),_tmpEAE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL419: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3009 "parse.y"
void*_tmp808=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp808,(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL41A: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3012 "parse.y"
struct _tuple20 _tmp809=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp80B;struct _dyneither_ptr _tmp80C;struct _tuple20 _tmp80A=_tmp809;_tmp80B=_tmp80A.f1;_tmp80C=_tmp80A.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp80B,_tmp80C,(unsigned int)(yyyylsp[0]).first_line));
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
{struct _dyneither_ptr*_tmpEB1;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEB1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL423: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3034 "parse.y"
{struct _dyneither_ptr*_tmpEB2;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEB2)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB5;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB5.tag=24,((_tmpEB5.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB5)))))),_tmpEB4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL427: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3042 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB8;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB7;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB7=_cycalloc(sizeof(*_tmpEB7)),((_tmpEB7[0]=((_tmpEB8.tag=24,((_tmpEB8.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB8.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB8)))))),_tmpEB7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL428: {
# 3045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3047 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEC2;struct _dyneither_ptr*_tmpEC1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEC0;struct Cyc_List_List*_tmpEBF;yyval=Cyc_YY3(((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF->hd=(void*)((_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((_tmpEC2[0]=((_tmpEC0.tag=0,((_tmpEC0.f1=((_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEC1)))),_tmpEC0)))),_tmpEC2)))),((_tmpEBF->tl=0,_tmpEBF)))))));}
break;}case 492: _LL429: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3050 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEC8;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEC7;struct Cyc_List_List*_tmpEC6;yyval=Cyc_YY3(((_tmpEC6=_cycalloc(sizeof(*_tmpEC6)),((_tmpEC6->hd=(void*)((_tmpEC8=_cycalloc_atomic(sizeof(*_tmpEC8)),((_tmpEC8[0]=((_tmpEC7.tag=1,((_tmpEC7.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpEC7)))),_tmpEC8)))),((_tmpEC6->tl=0,_tmpEC6)))))));}
break;}case 493: _LL42A: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3052 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpED2;struct _dyneither_ptr*_tmpED1;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpED0;struct Cyc_List_List*_tmpECF;yyval=Cyc_YY3(((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF->hd=(void*)((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED0.tag=0,((_tmpED0.f1=((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED1)))),_tmpED0)))),_tmpED2)))),((_tmpECF->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpECF)))))));}
break;}case 494: _LL42B: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3055 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpED8;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpED7;struct Cyc_List_List*_tmpED6;yyval=Cyc_YY3(((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6->hd=(void*)((_tmpED8=_cycalloc_atomic(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED7.tag=1,((_tmpED7.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpED7)))),_tmpED8)))),((_tmpED6->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpED6)))))));}
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
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpEDB;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEDA;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((_tmpEDB.tag=28,((_tmpEDB.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpEDB.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpEDB.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpEDB.f4=0,_tmpEDB)))))))))),_tmpEDA)))),(unsigned int)(yyyylsp[0]).first_line));}
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
{struct Cyc_List_List*_tmpEDC;yyval=Cyc_YY6(((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEDC->tl=0,_tmpEDC)))))));}
break;}case 507: _LL438: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3097 "parse.y"
{struct Cyc_List_List*_tmpEDD;yyval=Cyc_YY6(((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEDD->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEDD)))))));}
break;}case 508: _LL439: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp825=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp825.f1,_tmp825.f2,(unsigned int)(yyyylsp[0]).first_line));
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
struct _dyneither_ptr _tmp826=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp826);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp826,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3115
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp826,i,(unsigned int)(yyyylsp[0]).first_line));
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
{struct _dyneither_ptr*_tmpEE0;struct _tuple0*_tmpEDF;yyval=Cyc_QualId_tok(((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF->f1=Cyc_Absyn_Rel_n(0),((_tmpEDF->f2=((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE0)))),_tmpEDF)))))));}
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
{struct _dyneither_ptr*_tmpEE3;struct _tuple0*_tmpEE2;yyval=Cyc_QualId_tok(((_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2->f1=Cyc_Absyn_Rel_n(0),((_tmpEE2->f2=((_tmpEE3=_cycalloc(sizeof(*_tmpEE3)),((_tmpEE3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE3)))),_tmpEE2)))))));}
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
{unsigned int _tmpEE8;unsigned int _tmpEE7;struct _dyneither_ptr _tmpEE6;char*_tmpEE5;unsigned int _tmpEE4;msg=((_tmpEE4=(unsigned int)(sze + 15),((_tmpEE5=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEE4 + 1)),((_tmpEE6=_tag_dyneither(_tmpEE5,sizeof(char),_tmpEE4 + 1),((((_tmpEE7=_tmpEE4,((_tmpEE9(& _tmpEE8,& _tmpEE7,& _tmpEE5),_tmpEE5[_tmpEE7]=(char)0)))),_tmpEE6))))))));}
{const char*_tmpEEA;Cyc_strcpy(msg,((_tmpEEA="parse error",_tag_dyneither(_tmpEEA,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 292 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6876,x + yyn)]== x){
# 453
{const char*_tmpEEC;const char*_tmpEEB;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEEC=", expecting `",_tag_dyneither(_tmpEEC,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEEB=" or `",_tag_dyneither(_tmpEEB,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(292,x)]);
{const char*_tmpEED;Cyc_strcat(msg,((_tmpEED="'",_tag_dyneither(_tmpEED,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEEE;Cyc_yyerror(((_tmpEEE="parse error",_tag_dyneither(_tmpEEE,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp834=1;_npop_handler(0);return _tmp834;}
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
 if(yyssp_offset == 0){int _tmp835=1;_npop_handler(0);return _tmp835;}
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
int _tmp836=0;_npop_handler(0);return _tmp836;}
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
union Cyc_YYSTYPE _tmp840=v;int _tmp841;char _tmp842;struct _dyneither_ptr _tmp843;union Cyc_Absyn_Nmspace _tmp844;struct _dyneither_ptr*_tmp845;_LL44B: if((_tmp840.Int_tok).tag != 1)goto _LL44D;_tmp841=((struct _tuple5)(_tmp840.Int_tok).val).f2;_LL44C:
{const char*_tmpEF2;void*_tmpEF1[1];struct Cyc_Int_pa_PrintArg_struct _tmpEF0;(_tmpEF0.tag=1,((_tmpEF0.f1=(unsigned long)_tmp841,((_tmpEF1[0]=& _tmpEF0,Cyc_fprintf(Cyc_stderr,((_tmpEF2="%d",_tag_dyneither(_tmpEF2,sizeof(char),3))),_tag_dyneither(_tmpEF1,sizeof(void*),1)))))));}goto _LL44A;_LL44D: if((_tmp840.Char_tok).tag != 2)goto _LL44F;_tmp842=(char)(_tmp840.Char_tok).val;_LL44E:
{const char*_tmpEF6;void*_tmpEF5[1];struct Cyc_Int_pa_PrintArg_struct _tmpEF4;(_tmpEF4.tag=1,((_tmpEF4.f1=(unsigned long)((int)_tmp842),((_tmpEF5[0]=& _tmpEF4,Cyc_fprintf(Cyc_stderr,((_tmpEF6="%c",_tag_dyneither(_tmpEF6,sizeof(char),3))),_tag_dyneither(_tmpEF5,sizeof(void*),1)))))));}goto _LL44A;_LL44F: if((_tmp840.String_tok).tag != 3)goto _LL451;_tmp843=(struct _dyneither_ptr)(_tmp840.String_tok).val;_LL450:
{const char*_tmpEFA;void*_tmpEF9[1];struct Cyc_String_pa_PrintArg_struct _tmpEF8;(_tmpEF8.tag=0,((_tmpEF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp843),((_tmpEF9[0]=& _tmpEF8,Cyc_fprintf(Cyc_stderr,((_tmpEFA="\"%s\"",_tag_dyneither(_tmpEFA,sizeof(char),5))),_tag_dyneither(_tmpEF9,sizeof(void*),1)))))));}goto _LL44A;_LL451: if((_tmp840.QualId_tok).tag != 5)goto _LL453;_tmp844=((struct _tuple0*)(_tmp840.QualId_tok).val)->f1;_tmp845=((struct _tuple0*)(_tmp840.QualId_tok).val)->f2;_LL452: {
# 3151
struct Cyc_List_List*_tmp84F=0;
{union Cyc_Absyn_Nmspace _tmp850=_tmp844;struct Cyc_List_List*_tmp851;struct Cyc_List_List*_tmp852;struct Cyc_List_List*_tmp853;_LL456: if((_tmp850.Rel_n).tag != 1)goto _LL458;_tmp851=(struct Cyc_List_List*)(_tmp850.Rel_n).val;_LL457:
 _tmp84F=_tmp851;goto _LL455;_LL458: if((_tmp850.Abs_n).tag != 2)goto _LL45A;_tmp852=(struct Cyc_List_List*)(_tmp850.Abs_n).val;_LL459:
 _tmp84F=_tmp852;goto _LL455;_LL45A: if((_tmp850.C_n).tag != 3)goto _LL45C;_tmp853=(struct Cyc_List_List*)(_tmp850.C_n).val;_LL45B:
 _tmp84F=_tmp853;goto _LL455;_LL45C: if((_tmp850.Loc_n).tag != 4)goto _LL455;_LL45D:
 goto _LL455;_LL455:;}
# 3158
for(0;_tmp84F != 0;_tmp84F=_tmp84F->tl){
const char*_tmpEFE;void*_tmpEFD[1];struct Cyc_String_pa_PrintArg_struct _tmpEFC;(_tmpEFC.tag=0,((_tmpEFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp84F->hd)),((_tmpEFD[0]=& _tmpEFC,Cyc_fprintf(Cyc_stderr,((_tmpEFE="%s::",_tag_dyneither(_tmpEFE,sizeof(char),5))),_tag_dyneither(_tmpEFD,sizeof(void*),1)))))));}
{const char*_tmpF02;void*_tmpF01[1];struct Cyc_String_pa_PrintArg_struct _tmpF00;(_tmpF00.tag=0,((_tmpF00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp845),((_tmpF01[0]=& _tmpF00,Cyc_fprintf(Cyc_stderr,((_tmpF02="%s::",_tag_dyneither(_tmpF02,sizeof(char),5))),_tag_dyneither(_tmpF01,sizeof(void*),1)))))));}
goto _LL44A;}_LL453:;_LL454:
{const char*_tmpF05;void*_tmpF04;(_tmpF04=0,Cyc_fprintf(Cyc_stderr,((_tmpF05="?",_tag_dyneither(_tmpF05,sizeof(char),2))),_tag_dyneither(_tmpF04,sizeof(void*),0)));}goto _LL44A;_LL44A:;}
# 3167
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp85C=_new_region("yyr");struct _RegionHandle*yyr=& _tmp85C;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp85D=Cyc_Parse_parse_result;_npop_handler(0);return _tmp85D;};
# 3170
;_pop_region(yyr);};}
