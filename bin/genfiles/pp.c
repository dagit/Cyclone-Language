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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 129
void*Cyc_Core_identity(void*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 54 "string.h"
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 43 "fn.h"
typedef struct Cyc_Fn_Function*Cyc_Fn_fn_t;
# 48
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);
# 51
struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));
# 54
void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _dyneither_ptr f1;struct Cyc_List_List*f2;};
# 56
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 88
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
# 100
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 126
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};
# 39 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 41
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 43
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 46
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 49
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 67
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 43 "pp.cyc"
typedef int Cyc_PP_dummy_t;struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 50
typedef void*Cyc_PP_alist_t;
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0,0};struct _tuple1{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple1 _tmpAE;struct _tuple1 _tmp3=(_tmpAE.f1=a1,((_tmpAE.f2=a2,_tmpAE)));_LL1: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp4=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f1;if(_tmp4->tag != 0)goto _LL3;}_LL2:
 return a2;_LL3: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp5=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f2;if(_tmp5->tag != 0)goto _LL5;}_LL4:
 return a1;_LL5:;_LL6: {
struct Cyc_PP_Append_PP_Alist_struct _tmpB1;struct Cyc_PP_Append_PP_Alist_struct*_tmpB0;return(void*)((_tmpB0=_cycalloc(sizeof(*_tmpB0)),((_tmpB0[0]=((_tmpB1.tag=2,((_tmpB1.f1=(void*)a1,((_tmpB1.f2=(void*)a2,_tmpB1)))))),_tmpB0))));}_LL0:;}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp8=y;void*_tmpB;void*_tmpD;void*_tmpE;_LL8: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp9=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp8;if(_tmp9->tag != 0)goto _LLA;}_LL9:
 return l;_LLA: {struct Cyc_PP_Single_PP_Alist_struct*_tmpA=(struct Cyc_PP_Single_PP_Alist_struct*)_tmp8;if(_tmpA->tag != 1)goto _LLC;else{_tmpB=(void*)_tmpA->f1;}}_LLB: {
struct Cyc_List_List*_tmpB2;return(_tmpB2=_cycalloc(sizeof(*_tmpB2)),((_tmpB2->hd=(void*)_tmpB,((_tmpB2->tl=l,_tmpB2)))));}_LLC: {struct Cyc_PP_Append_PP_Alist_struct*_tmpC=(struct Cyc_PP_Append_PP_Alist_struct*)_tmp8;if(_tmpC->tag != 2)goto _LL7;else{_tmpD=(void*)_tmpC->f1;_tmpE=(void*)_tmpC->f2;}}_LLD:
 return Cyc_PP_list_of_alist_f(_tmpD,Cyc_PP_list_of_alist_f(_tmpE,l));_LL7:;}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmp10=((struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create)(16,al);
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp10,al);{
int last=0;
while(last >= 0){
void*_tmp11=((void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp10,last);struct _dyneither_ptr*_tmp14;void*_tmp16;void*_tmp17;_LLF: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp12=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp11;if(_tmp12->tag != 0)goto _LL11;}_LL10:
 -- last;goto _LLE;_LL11: {struct Cyc_PP_Single_PP_Alist_struct*_tmp13=(struct Cyc_PP_Single_PP_Alist_struct*)_tmp11;if(_tmp13->tag != 1)goto _LL13;else{_tmp14=(struct _dyneither_ptr*)_tmp13->f1;}}_LL12:
 -- last;{const char*_tmpB6;void*_tmpB5[1];struct Cyc_String_pa_PrintArg_struct _tmpB4;(_tmpB4.tag=0,((_tmpB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14),((_tmpB5[0]=& _tmpB4,Cyc_fprintf(f,((_tmpB6="%s",_tag_dyneither(_tmpB6,sizeof(char),3))),_tag_dyneither(_tmpB5,sizeof(void*),1)))))));}goto _LLE;_LL13: {struct Cyc_PP_Append_PP_Alist_struct*_tmp15=(struct Cyc_PP_Append_PP_Alist_struct*)_tmp11;if(_tmp15->tag != 2)goto _LLE;else{_tmp16=(void*)_tmp15->f1;_tmp17=(void*)_tmp15->f2;}}_LL14:
# 107
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp10,last,_tmp17);
if(last == ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp10)- 1)
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp10,_tmp16);else{
# 111
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp10,last + 1,_tmp16);}
++ last;
goto _LLE;_LLE:;}};}
# 119
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Ppstate*_tmpB7;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpB7=_cycalloc_atomic(sizeof(*_tmpB7)),((_tmpB7->ci=0,((_tmpB7->cc=0,((_tmpB7->cl=1,((_tmpB7->pw=w,((_tmpB7->epw=w,_tmpB7)))))))))))));
Cyc_PP_dump_out(f,o->ppout);}
# 125
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpB8;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpB8=_cycalloc_atomic(sizeof(*_tmpB8)),((_tmpB8->ci=0,((_tmpB8->cc=0,((_tmpB8->cl=1,((_tmpB8->pw=w,((_tmpB8->epw=w,_tmpB8)))))))))))));
const char*_tmpB9;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpB9="",_tag_dyneither(_tmpB9,sizeof(char),1))));}
# 132
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpBA;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpBA=_cycalloc_atomic(sizeof(*_tmpBA)),((_tmpBA->ci=0,((_tmpBA->cc=0,((_tmpBA->cl=1,((_tmpBA->pw=w,((_tmpBA->epw=w,_tmpBA)))))))))))));
const char*_tmpBD;struct _tuple0*_tmpBC;return(_tmpBC=_cycalloc(sizeof(*_tmpBC)),((_tmpBC->f1=(struct _dyneither_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpBD="",_tag_dyneither(_tmpBD,sizeof(char),1)))),((_tmpBC->f2=
((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->links),_tmpBC)))));}
# 139
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 141
int Cyc_PP_tex_output=0;
# 143
struct _dyneither_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0){
struct Cyc_Core_Opt*_tmpBE;Cyc_PP_bhashtbl=((_tmpBE=_cycalloc(sizeof(*_tmpBE)),((_tmpBE->v=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity),_tmpBE))));}
if(i < 0){struct Cyc_Core_Failure_exn_struct _tmpC4;const char*_tmpC3;struct Cyc_Core_Failure_exn_struct*_tmpC2;(int)_throw((void*)((_tmpC2=_cycalloc(sizeof(*_tmpC2)),((_tmpC2[0]=((_tmpC4.tag=Cyc_Core_Failure,((_tmpC4.f1=((_tmpC3="nlblanks",_tag_dyneither(_tmpC3,sizeof(char),9))),_tmpC4)))),_tmpC2)))));}{
struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){
{struct _dyneither_ptr _tmp28=*((struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0);return _tmp28;};_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp2A=_tmp26;void*_tmp2C;_LL16: {struct Cyc_Core_Not_found_exn_struct*_tmp2B=(struct Cyc_Core_Not_found_exn_struct*)_tmp2A;if(_tmp2B->tag != Cyc_Core_Not_found)goto _LL18;}_LL17:
# 151
 if(!Cyc_PP_tex_output){
int _tmp2D=i / 8;
int _tmp2E=i % 8;
int _tmp2F=(2 + _tmp2D)+ _tmp2E;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp2F);
{char _tmpC7;char _tmpC6;struct _dyneither_ptr _tmpC5;(_tmpC5=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpC6=*((char*)_check_dyneither_subscript(_tmpC5,sizeof(char),0)),((_tmpC7='\n',((_get_dyneither_size(_tmpC5,sizeof(char))== 1  && (_tmpC6 == '\000'  && _tmpC7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC5.curr)=_tmpC7)))))));}
{int j=0;for(0;j < _tmp2D;++ j){
char _tmpCA;char _tmpC9;struct _dyneither_ptr _tmpC8;(_tmpC8=_dyneither_ptr_plus(nlb,sizeof(char),j + 1),((_tmpC9=*((char*)_check_dyneither_subscript(_tmpC8,sizeof(char),0)),((_tmpCA='\t',((_get_dyneither_size(_tmpC8,sizeof(char))== 1  && (_tmpC9 == '\000'  && _tmpCA != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC8.curr)=_tmpCA)))))));}}
{int j=0;for(0;j < _tmp2E;++ j){
char _tmpCD;char _tmpCC;struct _dyneither_ptr _tmpCB;(_tmpCB=_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp2D),((_tmpCC=*((char*)_check_dyneither_subscript(_tmpCB,sizeof(char),0)),((_tmpCD=' ',((_get_dyneither_size(_tmpCB,sizeof(char))== 1  && (_tmpCC == '\000'  && _tmpCD != '\000')?_throw_arraybounds(): 1,*((char*)_tmpCB.curr)=_tmpCD)))))));}}
{struct _dyneither_ptr*_tmpCE;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpCE=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpCE[0]=(struct _dyneither_ptr)nlb,_tmpCE)))));}
return(struct _dyneither_ptr)nlb;}else{
# 167
int _tmp3A=3 + i;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)(_tmp3A + 1));
{char _tmpD1;char _tmpD0;struct _dyneither_ptr _tmpCF;(_tmpCF=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpD0=*((char*)_check_dyneither_subscript(_tmpCF,sizeof(char),0)),((_tmpD1='\\',((_get_dyneither_size(_tmpCF,sizeof(char))== 1  && (_tmpD0 == '\000'  && _tmpD1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpCF.curr)=_tmpD1)))))));}
{char _tmpD4;char _tmpD3;struct _dyneither_ptr _tmpD2;(_tmpD2=_dyneither_ptr_plus(nlb,sizeof(char),1),((_tmpD3=*((char*)_check_dyneither_subscript(_tmpD2,sizeof(char),0)),((_tmpD4='\\',((_get_dyneither_size(_tmpD2,sizeof(char))== 1  && (_tmpD3 == '\000'  && _tmpD4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD2.curr)=_tmpD4)))))));}
{char _tmpD7;char _tmpD6;struct _dyneither_ptr _tmpD5;(_tmpD5=_dyneither_ptr_plus(nlb,sizeof(char),2),((_tmpD6=*((char*)_check_dyneither_subscript(_tmpD5,sizeof(char),0)),((_tmpD7='\n',((_get_dyneither_size(_tmpD5,sizeof(char))== 1  && (_tmpD6 == '\000'  && _tmpD7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD5.curr)=_tmpD7)))))));}
{int j=3;for(0;j < _tmp3A;++ j){
char _tmpDA;char _tmpD9;struct _dyneither_ptr _tmpD8;(_tmpD8=_dyneither_ptr_plus(nlb,sizeof(char),j),((_tmpD9=*((char*)_check_dyneither_subscript(_tmpD8,sizeof(char),0)),((_tmpDA='~',((_get_dyneither_size(_tmpD8,sizeof(char))== 1  && (_tmpD9 == '\000'  && _tmpDA != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD8.curr)=_tmpDA)))))));}}
{struct _dyneither_ptr*_tmpDB;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpDB=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDB[0]=(struct _dyneither_ptr)nlb,_tmpDB)))));}
return(struct _dyneither_ptr)nlb;}_LL18: _tmp2C=_tmp2A;_LL19:(void)_throw(_tmp2C);_LL15:;}};};}
# 181
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 183
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _dyneither_ptr f2;};
# 185
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
int _tmp49;struct _dyneither_ptr _tmp4A;struct _tuple2*_tmp48=clo;_tmp49=(*_tmp48).f1;_tmp4A=(*_tmp48).f2;{
struct Cyc_PP_Single_PP_Alist_struct*_tmpE5;struct _dyneither_ptr*_tmpE4;struct Cyc_PP_Single_PP_Alist_struct _tmpE3;struct Cyc_PP_Out*_tmpE2;return(_tmpE2=_cycalloc(sizeof(*_tmpE2)),((_tmpE2->newcc=st->cc + _tmp49,((_tmpE2->newcl=st->cl,((_tmpE2->ppout=(void*)(
# 189
(_tmpE5=_cycalloc(sizeof(*_tmpE5)),((_tmpE5[0]=((_tmpE3.tag=1,((_tmpE3.f1=(void*)((_tmpE4=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpE4[0]=_tmp4A,_tmpE4)))),_tmpE3)))),_tmpE5)))),((_tmpE2->links=(void*)& Cyc_PP_Empty_link,_tmpE2)))))))));};}
# 192
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)s);
struct _tuple2*_tmpE8;struct Cyc_PP_Doc*_tmpE7;return
(_tmpE7=_cycalloc(sizeof(*_tmpE7)),((_tmpE7->mwo=slen,((_tmpE7->mw=Cyc_PP_infinity,((_tmpE7->f=
# 197
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 199
(_tmpE8=_cycalloc(sizeof(*_tmpE8)),((_tmpE8->f1=slen,((_tmpE8->f2=s,_tmpE8))))))),_tmpE7)))))));}
# 201
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpE9;Cyc_PP_str_hashtbl=((_tmpE9=_cycalloc(sizeof(*_tmpE9)),((_tmpE9->v=t,_tmpE9))));};}else{
# 207
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 209
struct _handler_cons _tmp52;_push_handler(& _tmp52);{int _tmp54=0;if(setjmp(_tmp52.handler))_tmp54=1;if(!_tmp54){
{struct _dyneither_ptr*_tmpEA;struct Cyc_PP_Doc*_tmp56=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpEA=_cycalloc(sizeof(*_tmpEA)),((_tmpEA[0]=s,_tmpEA)))));_npop_handler(0);return _tmp56;};_pop_handler();}else{void*_tmp53=(void*)_exn_thrown;void*_tmp58=_tmp53;void*_tmp5A;_LL1B: {struct Cyc_Core_Not_found_exn_struct*_tmp59=(struct Cyc_Core_Not_found_exn_struct*)_tmp58;if(_tmp59->tag != Cyc_Core_Not_found)goto _LL1D;}_LL1C: {
# 213
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
{struct _dyneither_ptr*_tmpEB;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpEB=_cycalloc(sizeof(*_tmpEB)),((_tmpEB[0]=s,_tmpEB)))),d);}
return d;}_LL1D: _tmp5A=_tmp58;_LL1E:(void)_throw(_tmp5A);_LL1A:;}};};}
# 218
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return Cyc_PP_text(*s);}
# 223
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _dyneither_ptr s,int slen){
struct _tuple2*_tmpEE;struct Cyc_PP_Doc*_tmpED;return
(_tmpED=_cycalloc(sizeof(*_tmpED)),((_tmpED->mwo=slen,((_tmpED->mw=Cyc_PP_infinity,((_tmpED->f=
# 227
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 229
(_tmpEE=_cycalloc(sizeof(*_tmpEE)),((_tmpEE->f1=slen,((_tmpEE->f2=s,_tmpEE))))))),_tmpED)))))));}
# 231
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpEF;Cyc_PP_str_hashtbl2=((_tmpEF=_cycalloc(sizeof(*_tmpEF)),((_tmpEF->v=t,_tmpEF))));};}else{
# 238
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 240
struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct _dyneither_ptr*_tmpF0;struct Cyc_PP_Doc*_tmp63=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpF0=_cycalloc(sizeof(*_tmpF0)),((_tmpF0[0]=s,_tmpF0)))));_npop_handler(0);return _tmp63;};_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp65=_tmp60;void*_tmp67;_LL20: {struct Cyc_Core_Not_found_exn_struct*_tmp66=(struct Cyc_Core_Not_found_exn_struct*)_tmp65;if(_tmp66->tag != Cyc_Core_Not_found)goto _LL22;}_LL21: {
# 244
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
{struct _dyneither_ptr*_tmpF1;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpF1=_cycalloc(sizeof(*_tmpF1)),((_tmpF1[0]=s,_tmpF1)))),d);}
return d;}_LL22: _tmp67=_tmp65;_LL23:(void)_throw(_tmp67);_LL1F:;}};};}
# 250
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 256
struct Cyc_PP_Doc*Cyc_PP_nil_doc(){
if(Cyc_PP_nil_doc_opt == 0){
const char*_tmpF4;struct Cyc_Core_Opt*_tmpF3;Cyc_PP_nil_doc_opt=((_tmpF3=_cycalloc(sizeof(*_tmpF3)),((_tmpF3->v=Cyc_PP_text(((_tmpF4="",_tag_dyneither(_tmpF4,sizeof(char),1)))),_tmpF3))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 263
struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0){
const char*_tmpF7;struct Cyc_Core_Opt*_tmpF6;Cyc_PP_blank_doc_opt=((_tmpF6=_cycalloc(sizeof(*_tmpF6)),((_tmpF6->v=Cyc_PP_text(((_tmpF7="",_tag_dyneither(_tmpF7,sizeof(char),1)))),_tmpF6))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _dyneither_ptr f4;};
# 269
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
int _tmp6E;struct _dyneither_ptr _tmp6F;struct _dyneither_ptr _tmp70;struct _tuple3*_tmp6D=clo;_tmp6E=(*_tmp6D).f1;_tmp6F=(*_tmp6D).f2;_tmp70=(*_tmp6D).f3;{
struct Cyc_PP_Single_PP_Alist_struct*_tmp10A;struct _tuple4*_tmp109;struct Cyc_PP_Single_PP_Alist_struct _tmp108;struct Cyc_PP_Single_PP_Alist_struct*_tmp107;struct _dyneither_ptr*_tmp106;struct Cyc_PP_Single_PP_Alist_struct _tmp105;struct Cyc_PP_Out*_tmp104;return(_tmp104=_cycalloc(sizeof(*_tmp104)),((_tmp104->newcc=st->cc + _tmp6E,((_tmp104->newcl=st->cl,((_tmp104->ppout=(void*)(
# 273
(_tmp107=_cycalloc(sizeof(*_tmp107)),((_tmp107[0]=((_tmp105.tag=1,((_tmp105.f1=(void*)((_tmp106=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp106[0]=_tmp6F,_tmp106)))),_tmp105)))),_tmp107)))),((_tmp104->links=(void*)(
(_tmp10A=_cycalloc(sizeof(*_tmp10A)),((_tmp10A[0]=((_tmp108.tag=1,((_tmp108.f1=(void*)((_tmp109=_cycalloc(sizeof(*_tmp109)),((_tmp109->f1=st->cl,((_tmp109->f2=st->cc,((_tmp109->f3=_tmp6E,((_tmp109->f4=_tmp70,_tmp109)))))))))),_tmp108)))),_tmp10A)))),_tmp104)))))))));};}
# 276
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)shrt);
struct _tuple3*_tmp10D;struct Cyc_PP_Doc*_tmp10C;return(_tmp10C=_cycalloc(sizeof(*_tmp10C)),((_tmp10C->mwo=slen,((_tmp10C->mw=Cyc_PP_infinity,((_tmp10C->f=
# 280
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,((_tmp10D=_cycalloc(sizeof(*_tmp10D)),((_tmp10D->f1=slen,((_tmp10D->f2=shrt,((_tmp10D->f3=full,_tmp10D))))))))),_tmp10C)))))));}
# 284
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Single_PP_Alist_struct*_tmp117;struct _dyneither_ptr*_tmp116;struct Cyc_PP_Single_PP_Alist_struct _tmp115;struct Cyc_PP_Out*_tmp114;return(_tmp114=_cycalloc(sizeof(*_tmp114)),((_tmp114->newcc=st->ci,((_tmp114->newcl=st->cl + 1,((_tmp114->ppout=(void*)(
# 287
(_tmp117=_cycalloc(sizeof(*_tmp117)),((_tmp117[0]=((_tmp115.tag=1,((_tmp115.f1=(void*)((_tmp116=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp116[0]=Cyc_PP_nlblanks(st->ci),_tmp116)))),_tmp115)))),_tmp117)))),((_tmp114->links=(void*)& Cyc_PP_Empty_link,_tmp114)))))))));}
# 290
struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0){
struct Cyc_PP_Doc*_tmp11A;struct Cyc_Core_Opt*_tmp119;Cyc_PP_line_doc_opt=((_tmp119=_cycalloc(sizeof(*_tmp119)),((_tmp119->v=((_tmp11A=_cycalloc(sizeof(*_tmp11A)),((_tmp11A->mwo=0,((_tmp11A->mw=0,((_tmp11A->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(Cyc_PP_line_f),_tmp11A)))))))),_tmp119))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 296
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
int _tmp81;struct Cyc_PP_Doc*_tmp82;struct _tuple5*_tmp80=clo;_tmp81=(*_tmp80).f1;_tmp82=(*_tmp80).f2;{
struct Cyc_PP_Ppstate*_tmp11B;struct Cyc_PP_Ppstate*st2=
(_tmp11B=_cycalloc_atomic(sizeof(*_tmp11B)),((_tmp11B->ci=st->ci + _tmp81,((_tmp11B->cc=st->cc,((_tmp11B->cl=st->cl,((_tmp11B->pw=st->pw,((_tmp11B->epw=st->epw,_tmp11B)))))))))));
# 304
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp82->f,st2);};}
# 306
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
struct _tuple5*_tmp11E;struct Cyc_PP_Doc*_tmp11D;return(_tmp11D=_cycalloc(sizeof(*_tmp11D)),((_tmp11D->mwo=d->mwo,((_tmp11D->mw=d->mw,((_tmp11D->f=
# 309
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,(
# 311
(_tmp11E=_cycalloc(sizeof(*_tmp11E)),((_tmp11E->f1=k,((_tmp11E->f2=d,_tmp11E))))))),_tmp11D)))))));}
# 314
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 319
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 324
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Doc*_tmp87;struct Cyc_PP_Doc*_tmp88;struct _tuple6*_tmp86=clo;_tmp87=(*_tmp86).f1;_tmp88=(*_tmp86).f2;{
int epw2=Cyc_PP_max(st->pw - _tmp88->mw,st->epw - _tmp87->mwo);
struct Cyc_PP_Ppstate*_tmp11F;struct Cyc_PP_Ppstate*st2=(_tmp11F=_cycalloc_atomic(sizeof(*_tmp11F)),((_tmp11F->ci=st->ci,((_tmp11F->cc=st->cc,((_tmp11F->cl=st->cl,((_tmp11F->pw=st->pw,((_tmp11F->epw=epw2,_tmp11F)))))))))));
struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp87->f,st2);
struct Cyc_PP_Ppstate*_tmp120;struct Cyc_PP_Ppstate*st3=(_tmp120=_cycalloc_atomic(sizeof(*_tmp120)),((_tmp120->ci=st->ci,((_tmp120->cc=o1->newcc,((_tmp120->cl=o1->newcl,((_tmp120->pw=st->pw,((_tmp120->epw=epw2,_tmp120)))))))))));
struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp88->f,st3);
struct Cyc_PP_Out*_tmp121;struct Cyc_PP_Out*o3=(_tmp121=_cycalloc(sizeof(*_tmp121)),((_tmp121->newcc=o2->newcc,((_tmp121->newcl=o2->newcl,((_tmp121->ppout=
# 333
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->ppout,o2->ppout),((_tmp121->links=
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->links,o2->links),_tmp121)))))))));
return o3;};}
# 337
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp124;struct Cyc_PP_Doc*_tmp123;return(_tmp123=_cycalloc(sizeof(*_tmp123)),((_tmp123->mwo=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo),((_tmp123->mw=
Cyc_PP_min(d1->mw,d1->mwo + d2->mw),((_tmp123->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,((_tmp124=_cycalloc(sizeof(*_tmp124)),((_tmp124->f1=d1,((_tmp124->f2=d2,_tmp124))))))),_tmp123)))))));}
# 342
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l){
struct Cyc_PP_Doc*_tmp8E=Cyc_PP_nil_doc();
{int i=(int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
_tmp8E=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),_tmp8E);}}
# 347
return _tmp8E;}
# 352
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp8F=ds0;for(0;_tmp8F != 0;_tmp8F=_tmp8F->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_tmp8F->hd;
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp125;st=((_tmp125=_cycalloc_atomic(sizeof(*_tmp125)),((_tmp125->ci=st->ci,((_tmp125->cc=o->newcc,((_tmp125->cl=o->newcl,((_tmp125->pw=st->pw,((_tmp125->epw=st->epw - d->mwo,_tmp125))))))))))));}{
struct Cyc_List_List*_tmp126;os=((_tmp126=_cycalloc(sizeof(*_tmp126)),((_tmp126->hd=o,((_tmp126->tl=os,_tmp126))))));};}}{
# 360
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 368
struct Cyc_PP_Out*_tmp127;return(_tmp127=_cycalloc(sizeof(*_tmp127)),((_tmp127->newcc=newcc,((_tmp127->newcl=newcl,((_tmp127->ppout=s,((_tmp127->links=links,_tmp127)))))))));};};}
# 370
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 374
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 380
{struct Cyc_List_List*_tmp93=doclist;for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp93->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp93->hd)->mw);
mw=mw2;
mwo=mwo2;}}{
# 386
struct Cyc_PP_Doc*_tmp128;return(_tmp128=_cycalloc(sizeof(*_tmp128)),((_tmp128->mwo=mw,((_tmp128->mw=mwo,((_tmp128->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,orig),_tmp128)))))));};};}
# 389
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 393
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return Cyc_PP_concat((struct Cyc_PP_Doc*)doclist->hd,Cyc_PP_cats(doclist->tl));}}}}
# 397
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp129;st=((_tmp129=_cycalloc_atomic(sizeof(*_tmp129)),((_tmp129->ci=st->ci,((_tmp129->cc=o->newcc,((_tmp129->cl=o->newcl,((_tmp129->pw=st->pw,((_tmp129->epw=st->epw - d->mwo,_tmp129))))))))))));}{
struct Cyc_List_List*_tmp12A;os=((_tmp12A=_cycalloc(sizeof(*_tmp12A)),((_tmp12A->hd=o,((_tmp12A->tl=os,_tmp12A))))));};}}{
# 407
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 415
struct Cyc_PP_Out*_tmp12B;return(_tmp12B=_cycalloc(sizeof(*_tmp12B)),((_tmp12B->newcc=newcc,((_tmp12B->newcl=newcl,((_tmp12B->ppout=s,((_tmp12B->links=links,_tmp12B)))))))));};};}
# 418
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0){
struct Cyc_Core_Failure_exn_struct _tmp131;const char*_tmp130;struct Cyc_Core_Failure_exn_struct*_tmp12F;(int)_throw((void*)((_tmp12F=_cycalloc(sizeof(*_tmp12F)),((_tmp12F[0]=((_tmp131.tag=Cyc_Core_Failure,((_tmp131.f1=((_tmp130="cats_arr -- size zero array",_tag_dyneither(_tmp130,sizeof(char),28))),_tmp131)))),_tmp12F)))));}{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mw);
mw=mw2;
mwo=mwo2;}}{
# 431
struct _dyneither_ptr*_tmp134;struct Cyc_PP_Doc*_tmp133;return(_tmp133=_cycalloc(sizeof(*_tmp133)),((_tmp133->mwo=mw,((_tmp133->mw=mwo,((_tmp133->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,((_tmp134=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp134[0]=docs,_tmp134))))),_tmp133)))))));};};}
# 434
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Doc*_tmp9E;struct Cyc_PP_Doc*_tmp9F;struct _tuple6*_tmp9D=clo;_tmp9E=(*_tmp9D).f1;_tmp9F=(*_tmp9D).f2;{
int dfits=st->cc + _tmp9E->mwo <= st->epw  || st->cc + _tmp9E->mw <= st->pw;
# 438
if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp9E->f,st);else{
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp9F->f,st);}};}
# 441
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp137;struct Cyc_PP_Doc*_tmp136;return(_tmp136=_cycalloc(sizeof(*_tmp136)),((_tmp136->mwo=Cyc_PP_min(d->mwo,d2->mwo),((_tmp136->mw=
Cyc_PP_min(d->mw,d2->mw),((_tmp136->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,((_tmp137=_cycalloc(sizeof(*_tmp137)),((_tmp137->f1=d,((_tmp137->f2=d2,_tmp137))))))),_tmp136)))))));}
# 448
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}
# 452
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*_tmp138;struct Cyc_PP_Ppstate*st2=(_tmp138=_cycalloc_atomic(sizeof(*_tmp138)),((_tmp138->ci=st->cc,((_tmp138->cc=st->cc,((_tmp138->cl=st->cl,((_tmp138->pw=st->pw,((_tmp138->epw=st->epw,_tmp138)))))))))));
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}
# 456
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*_tmp139;struct Cyc_PP_Doc*d2=
(_tmp139=_cycalloc(sizeof(*_tmp139)),((_tmp139->mwo=d->mwo,((_tmp139->mw=d->mw,((_tmp139->f=
# 460
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d),_tmp139)))))));
return d2;}
# 466
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp((void*)l->hd);else{
struct Cyc_PP_Doc*_tmp13A[4];return(_tmp13A[3]=Cyc_PP_ppseq_f(pp,sep,l->tl),((_tmp13A[2]=Cyc_PP_oline_doc(),((_tmp13A[1]=Cyc_PP_text(sep),((_tmp13A[0]=pp((void*)l->hd),Cyc_PP_cat(_tag_dyneither(_tmp13A,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 472
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 476
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 480
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmpA5=l;
while(_tmpA5->tl != 0){
struct Cyc_List_List*_tmpA6=_tmpA5->tl;
{struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;_tmpA5->tl=((_tmp13C=_cycalloc(sizeof(*_tmp13C)),((_tmp13C->hd=sep2,((_tmp13C->tl=((_tmp13D=_cycalloc(sizeof(*_tmp13D)),((_tmp13D->hd=oline,((_tmp13D->tl=_tmpA6,_tmp13D)))))),_tmp13C))))));}
_tmpA5=_tmpA6;}
# 488
return Cyc_PP_cats(l);}}}
# 496
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 501
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp((void*)l->hd);else{
struct Cyc_PP_Doc*_tmp13E[4];return(_tmp13E[3]=Cyc_PP_ppseql_f(pp,sep,l->tl),((_tmp13E[2]=Cyc_PP_line_doc(),((_tmp13E[1]=Cyc_PP_text(sep),((_tmp13E[0]=pp((void*)l->hd),Cyc_PP_cat(_tag_dyneither(_tmp13E,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 506
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 510
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
struct Cyc_PP_Doc*_tmp13F[4];return(_tmp13F[3]=Cyc_PP_seql_f(sep,l->tl),((_tmp13F[2]=Cyc_PP_line_doc(),((_tmp13F[1]=Cyc_PP_text(sep),((_tmp13F[0]=(struct Cyc_PP_Doc*)l->hd,Cyc_PP_cat(_tag_dyneither(_tmp13F,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 515
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 520
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp140[3];return(_tmp140[2]=
# 523
Cyc_PP_text(stop),((_tmp140[1]=
# 522
Cyc_PP_seq(sep,ss),((_tmp140[0]=
# 521
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp140,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 527
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
struct Cyc_PP_Doc*_tmp141[3];return(_tmp141[2]=
# 531
Cyc_PP_text(stop),((_tmp141[1]=
# 530
Cyc_PP_seq(sep,ss),((_tmp141[0]=
# 529
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp141,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 534
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp142[3];return(_tmp142[2]=
# 537
Cyc_PP_text(stop),((_tmp142[1]=
# 536
Cyc_PP_seql(sep,ss),((_tmp142[0]=
# 535
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp142,sizeof(struct Cyc_PP_Doc*),3)))))));}
