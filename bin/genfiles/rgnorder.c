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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 432
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 910 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 946
extern void*Cyc_Absyn_uint_typ;
# 952
extern void*Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 66
void*Cyc_Tcutil_compress(void*t);
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 155
int Cyc_Tcutil_unify(void*,void*);
# 305 "tcutil.h"
void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 38 "rgnorder.cyc"
static struct _dyneither_ptr Cyc_RgnOrder_tvar2string(struct Cyc_Absyn_Tvar*t){
return*t->name;}
# 41
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn);
static void*Cyc_RgnOrder_filter_effect(void*eff,struct Cyc_Absyn_Kind*k);struct Cyc_RgnOrder_RgnInfo{void*these_outlive;int resetable;};struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*youngest_unresettable;};
# 74 "rgnorder.cyc"
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
struct Cyc_Absyn_Kind*_stmttmp0=Cyc_Tcutil_typ_kind(rgn);enum Cyc_Absyn_KindQual _tmp1;enum Cyc_Absyn_AliasQual _tmp2;struct Cyc_Absyn_Kind*_tmp0=_stmttmp0;_tmp1=_tmp0->kind;_tmp2=_tmp0->aliasqual;
if(_tmp1 != Cyc_Absyn_RgnKind){
const char*_tmpDE;void*_tmpDD[1];struct Cyc_String_pa_PrintArg_struct _tmpDC;(_tmpDC.tag=0,((_tmpDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(rgn)),((_tmpDD[0]=& _tmpDC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDE="bad region type |%s| passed to add_outlives_constraint",_tag_dyneither(_tmpDE,sizeof(char),55))),_tag_dyneither(_tmpDD,sizeof(void*),1)))))));}{
void*_stmttmp1=Cyc_Tcutil_compress(eff);void*_tmp6=_stmttmp1;struct Cyc_List_List*_tmp8;void*_tmpC;void*_tmpE;_LL1: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6;if(_tmp7->tag != 24)goto _LL3;else{_tmp8=_tmp7->f1;}}_LL2:
# 81
 for(0;_tmp8 != 0;_tmp8=_tmp8->tl){
if(!Cyc_RgnOrder_valid_constraint((void*)_tmp8->hd,rgn))return 0;}
return 1;_LL3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6;if(_tmp9->tag != 1)goto _LL5;}_LL4:
# 85
 goto _LL6;_LL5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6;if(_tmpA->tag != 2)goto _LL7;}_LL6:
 return 1;_LL7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6;if(_tmpB->tag != 25)goto _LL9;else{_tmpC=(void*)_tmpB->f1;}}_LL8: {
# 90
struct Cyc_Absyn_Kind*_stmttmp3=Cyc_Tcutil_typ_kind(_tmpC);enum Cyc_Absyn_KindQual _tmp10;enum Cyc_Absyn_AliasQual _tmp11;struct Cyc_Absyn_Kind*_tmpF=_stmttmp3;_tmp10=_tmpF->kind;_tmp11=_tmpF->aliasqual;
return _tmp2 == Cyc_Absyn_Aliasable  || _tmp11 == _tmp2;}_LL9: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6;if(_tmpD->tag != 23)goto _LLB;else{_tmpE=(void*)_tmpD->f1;}}_LLA: {
# 95
struct Cyc_Absyn_Kind*_stmttmp2=Cyc_Tcutil_typ_kind(_tmpE);enum Cyc_Absyn_KindQual _tmp13;enum Cyc_Absyn_AliasQual _tmp14;struct Cyc_Absyn_Kind*_tmp12=_stmttmp2;_tmp13=_tmp12->kind;_tmp14=_tmp12->aliasqual;
return _tmp1 == Cyc_Absyn_RgnKind  && (_tmp2 == Cyc_Absyn_Aliasable  || _tmp14 == _tmp2);}_LLB:;_LLC: {
# 98
const char*_tmpE2;void*_tmpE1[1];struct Cyc_String_pa_PrintArg_struct _tmpE0;(_tmpE0.tag=0,((_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(eff)),((_tmpE1[0]=& _tmpE0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE2="bad effect |%s| passed to add_outlives_constraint",_tag_dyneither(_tmpE2,sizeof(char),50))),_tag_dyneither(_tmpE1,sizeof(void*),1)))))));}_LL0:;};}
# 104
static void*Cyc_RgnOrder_filter_effect(void*eff,struct Cyc_Absyn_Kind*k){
void*_tmp18=Cyc_Tcutil_compress(eff);
void*_tmp19=_tmp18;struct Cyc_List_List*_tmp1B;void*_tmp1F;_LLE: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp19;if(_tmp1A->tag != 24)goto _LL10;else{_tmp1B=_tmp1A->f1;}}_LLF: {
# 108
struct Cyc_List_List*effs=0;
for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
void*_tmp20=Cyc_RgnOrder_filter_effect((void*)_tmp1B->hd,k);
if(_tmp20 != (void*)& Cyc_Absyn_VoidType_val){struct Cyc_List_List*_tmpE3;effs=((_tmpE3=_cycalloc(sizeof(*_tmpE3)),((_tmpE3->hd=_tmp20,((_tmpE3->tl=effs,_tmpE3))))));}}{
# 113
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE5;return(void*)((_tmpE5=_cycalloc(sizeof(*_tmpE5)),((_tmpE5[0]=((_tmpE6.tag=24,((_tmpE6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effs),_tmpE6)))),_tmpE5))));};}_LL10: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19;if(_tmp1C->tag != 1)goto _LL12;}_LL11:
 goto _LL13;_LL12: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp19;if(_tmp1D->tag != 25)goto _LL14;}_LL13:
 return _tmp18;_LL14: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp19;if(_tmp1E->tag != 23)goto _LL16;else{_tmp1F=(void*)_tmp1E->f1;}}_LL15:
 if(Cyc_Tcutil_typ_kind(_tmp1F)!= k)return _tmp18;else{return(void*)& Cyc_Absyn_VoidType_val;}_LL16:;_LL17: {
const char*_tmpE9;void*_tmpE8;(_tmpE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE9="bad effect passed to filter_effect",_tag_dyneither(_tmpE9,sizeof(char),35))),_tag_dyneither(_tmpE8,sizeof(void*),0)));}_LLD:;}
# 124
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc){
# 127
eff=Cyc_Tcutil_normalize_effect(eff);{
struct Cyc_RgnOrder_RgnPO*_tmpEA;struct Cyc_RgnOrder_RgnPO*_tmp26=(_tmpEA=_region_malloc(r,sizeof(*_tmpEA)),((_tmpEA[0]=*po,_tmpEA)));
# 130
if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){
{const char*_tmpED;void*_tmpEC;(_tmpEC=0,Cyc_Tcutil_terr(loc,((_tmpED="Invalid region ordering constraint; kind mismatch",_tag_dyneither(_tmpED,sizeof(char),50))),_tag_dyneither(_tmpEC,sizeof(void*),0)));}
return _tmp26;}{
# 135
void*_stmttmp4=Cyc_Tcutil_compress(rgn);void*_tmp29=_stmttmp4;struct Cyc_Absyn_Tvar*_tmp2B;_LL19: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29;if(_tmp2A->tag != 2)goto _LL1B;else{_tmp2B=_tmp2A->f1;}}_LL1A: {
# 137
struct Cyc_Dict_Dict _tmp30=po->d;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(_tmp30,_tmp2B)){
void*_tmp31=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp30,_tmp2B))->these_outlive;
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF7;void*_tmpF6[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF5;struct Cyc_RgnOrder_RgnInfo*_tmpF4;_tmp30=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(_tmp30,_tmp2B,((_tmpF4=_region_malloc(r,sizeof(*_tmpF4)),((_tmpF4->these_outlive=(void*)((_tmpF7=_cycalloc(sizeof(*_tmpF7)),((_tmpF7[0]=((_tmpF5.tag=24,((_tmpF5.f1=((_tmpF6[1]=_tmp31,((_tmpF6[0]=eff,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF6,sizeof(void*),2)))))),_tmpF5)))),_tmpF7)))),((_tmpF4->resetable=0,_tmpF4)))))));}else{
# 142
struct Cyc_RgnOrder_RgnInfo*_tmpF8;_tmp30=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(_tmp30,_tmp2B,((_tmpF8=_region_malloc(r,sizeof(*_tmpF8)),((_tmpF8->these_outlive=eff,((_tmpF8->resetable=0,_tmpF8)))))));}
_tmp26->d=_tmp30;
return _tmp26;}_LL1B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29;if(_tmp2C->tag != 1)goto _LL1D;}_LL1C:
# 146
 Cyc_Tcutil_unify(rgn,(void*)& Cyc_Absyn_HeapRgn_val);
goto _LL1E;_LL1D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp29;if(_tmp2D->tag != 20)goto _LL1F;}_LL1E:
# 149
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFE;void*_tmpFD[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFC;_tmp26->these_outlive_heap=(void*)((_tmpFC=_cycalloc(sizeof(*_tmpFC)),((_tmpFC[0]=((_tmpFE.tag=24,((_tmpFE.f1=((_tmpFD[1]=po->these_outlive_heap,((_tmpFD[0]=eff,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFD,sizeof(void*),2)))))),_tmpFE)))),_tmpFC))));}
return _tmp26;_LL1F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp29;if(_tmp2E->tag != 21)goto _LL21;}_LL20:
# 152
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp104;void*_tmp103[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp102;_tmp26->these_outlive_unique=(void*)((_tmp102=_cycalloc(sizeof(*_tmp102)),((_tmp102[0]=((_tmp104.tag=24,((_tmp104.f1=((_tmp103[1]=po->these_outlive_unique,((_tmp103[0]=eff,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp103,sizeof(void*),2)))))),_tmp104)))),_tmp102))));}
return _tmp26;_LL21: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp29;if(_tmp2F->tag != 22)goto _LL23;}_LL22: {
# 155
const char*_tmp107;void*_tmp106;(_tmp106=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp107="RgnOrder::add_outlives_constraint can't outlive RC for now",_tag_dyneither(_tmp107,sizeof(char),59))),_tag_dyneither(_tmp106,sizeof(void*),0)));}_LL23:;_LL24: {
const char*_tmp10A;void*_tmp109;(_tmp109=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A="RgnOrder::add_outlives_constraint passed a bad region",_tag_dyneither(_tmp10A,sizeof(char),54))),_tag_dyneither(_tmp109,sizeof(void*),0)));}_LL18:;};};}
# 160
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*r,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resettable,int opened){
# 162
if(!opened  && ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,rgn)){
const char*_tmp10D;void*_tmp10C;(_tmp10C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D="RgnOrder::add_youngest: repeated region",_tag_dyneither(_tmp10D,sizeof(char),40))),_tag_dyneither(_tmp10C,sizeof(void*),0)));}{
struct Cyc_RgnOrder_RgnPO*_tmp10E;struct Cyc_RgnOrder_RgnPO*_tmp44=(_tmp10E=_region_malloc(r,sizeof(*_tmp10E)),((_tmp10E[0]=*po,_tmp10E)));
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp118;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp117;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp116;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp115;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp45=(_tmp115=_cycalloc(sizeof(*_tmp115)),((_tmp115[0]=((_tmp118.tag=23,((_tmp118.f1=(void*)((_tmp116=_cycalloc(sizeof(*_tmp116)),((_tmp116[0]=((_tmp117.tag=2,((_tmp117.f1=po->youngest_unresettable,_tmp117)))),_tmp116)))),_tmp118)))),_tmp115)));
{struct Cyc_RgnOrder_RgnInfo*_tmp119;_tmp44->d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(po->d,rgn,((_tmp119=_region_malloc(r,sizeof(*_tmp119)),((_tmp119->these_outlive=(void*)_tmp45,((_tmp119->resetable=resettable,_tmp119)))))));}
if(!resettable);
_tmp44->youngest_unresettable=rgn;
return _tmp44;};}
# 172
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int loc){
# 176
struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_rempty)(r,Cyc_Absyn_tvar_cmp);
{struct Cyc_List_List*_tmp4C=tvs;for(0;_tmp4C != 0;_tmp4C=_tmp4C->tl){
if((Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp4C->hd,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_RgnKind){
# 181
struct Cyc_RgnOrder_RgnInfo*_tmp11A;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,(struct Cyc_Absyn_Tvar*)_tmp4C->hd,((_tmp11A=_region_malloc(r,sizeof(*_tmp11A)),((_tmp11A->these_outlive=Cyc_Absyn_empty_effect,((_tmp11A->resetable=0,_tmp11A)))))));}}}
# 185
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11C;if(!Cyc_RgnOrder_valid_constraint(effect,(void*)((_tmp11C=_cycalloc(sizeof(*_tmp11C)),((_tmp11C[0]=((_tmp11D.tag=2,((_tmp11D.f1=fst_rgn,_tmp11D)))),_tmp11C)))))){
# 187
{const char*_tmp120;void*_tmp11F;(_tmp11F=0,Cyc_Tcutil_terr(loc,((_tmp120="Invalid region ordering constraint; kind mismatch",_tag_dyneither(_tmp120,sizeof(char),50))),_tag_dyneither(_tmp11F,sizeof(void*),0)));}{
struct Cyc_RgnOrder_RgnPO*_tmp121;return(_tmp121=_region_malloc(r,sizeof(*_tmp121)),((_tmp121->d=d,((_tmp121->these_outlive_heap=Cyc_Absyn_empty_effect,((_tmp121->these_outlive_unique=Cyc_Absyn_empty_effect,((_tmp121->youngest_unresettable=fst_rgn,_tmp121)))))))));};}}
# 193
{struct Cyc_RgnOrder_RgnInfo*_tmp122;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,fst_rgn,((_tmp122=_region_malloc(r,sizeof(*_tmp122)),((_tmp122->these_outlive=effect,((_tmp122->resetable=0,_tmp122)))))));}{
# 195
struct Cyc_RgnOrder_RgnPO*_tmp123;struct Cyc_RgnOrder_RgnPO*_tmp54=(_tmp123=_region_malloc(r,sizeof(*_tmp123)),((_tmp123->d=d,((_tmp123->these_outlive_heap=Cyc_Absyn_empty_effect,((_tmp123->these_outlive_unique=Cyc_Absyn_empty_effect,((_tmp123->youngest_unresettable=fst_rgn,_tmp123)))))))));
for(0;po != 0;po=po->tl){
_tmp54=Cyc_RgnOrder_add_outlives_constraint(r,_tmp54,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}
# 199
return _tmp54;};}
# 224 "rgnorder.cyc"
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r){
return(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,r))->resetable;}
# 229
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_stmttmp5=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(eff));void*_tmp56=_stmttmp5;void*_tmp58;struct Cyc_List_List*_tmp5A;_LL26: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 25)goto _LL28;else{_tmp58=(void*)_tmp57->f1;}}_LL27: {
# 232
void*_stmttmp6=Cyc_Tcutil_compress(_tmp58);void*_tmp5B=_stmttmp6;struct Cyc_Absyn_Tvar*_tmp5D;_LL2D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B;if(_tmp5C->tag != 2)goto _LL2F;else{_tmp5D=_tmp5C->f1;}}_LL2E:
 return Cyc_Absyn_tvar_cmp(_tmp5D,rgns_of_var)== 0;_LL2F:;_LL30:
 return 0;_LL2C:;}_LL28: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp59=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp56;if(_tmp59->tag != 24)goto _LL2A;else{_tmp5A=_tmp59->f1;}}_LL29:
# 237
 for(0;_tmp5A != 0;_tmp5A=_tmp5A->tl){
if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)_tmp5A->hd))
return 1;}
return 0;_LL2A:;_LL2B:
 return 0;_LL25:;}struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};
# 251
static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){
void*_stmttmp7=Cyc_Tcutil_compress(eff);void*_tmp5E=_stmttmp7;struct Cyc_Absyn_Tvar*_tmp61;struct Cyc_List_List*_tmp63;_LL32: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E;if(_tmp5F->tag != 23)goto _LL34;else{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)((void*)_tmp5F->f1);if(_tmp60->tag != 2)goto _LL34;else{_tmp61=_tmp60->f1;}}}_LL33:
# 254
{struct Cyc_List_List*_tmp64=env->seen;for(0;_tmp64 != 0;_tmp64=_tmp64->tl){
if(Cyc_Absyn_tvar_cmp(_tmp61,(struct Cyc_Absyn_Tvar*)_tmp64->hd)== 0)
return;}}
{struct Cyc_List_List*_tmp124;env->seen=((_tmp124=_region_malloc(env->r,sizeof(*_tmp124)),((_tmp124->hd=_tmp61,((_tmp124->tl=env->seen,_tmp124))))));}
{struct Cyc_List_List*_tmp125;env->todo=((_tmp125=_region_malloc(env->r,sizeof(*_tmp125)),((_tmp125->hd=_tmp61,((_tmp125->tl=env->todo,_tmp125))))));}
return;_LL34: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E;if(_tmp62->tag != 24)goto _LL36;else{_tmp63=_tmp62->f1;}}_LL35:
# 261
 for(0;_tmp63 != 0;_tmp63=_tmp63->tl){
Cyc_RgnOrder_add_to_search(env,(void*)_tmp63->hd);}
return;_LL36:;_LL37:
 return;_LL31:;}
# 268
static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){
# 270
struct Cyc_RgnOrder_OutlivesEnv _tmp126;struct Cyc_RgnOrder_OutlivesEnv _tmp67=(_tmp126.r=listrgn,((_tmp126.seen=0,((_tmp126.todo=0,_tmp126)))));
void*r=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Kind*_stmttmp8=Cyc_Tcutil_typ_kind(r);enum Cyc_Absyn_KindQual _tmp69;enum Cyc_Absyn_AliasQual _tmp6A;struct Cyc_Absyn_Kind*_tmp68=_stmttmp8;_tmp69=_tmp68->kind;_tmp6A=_tmp68->aliasqual;
if(_tmp69 == Cyc_Absyn_RgnKind)
switch(_tmp6A){case Cyc_Absyn_Aliasable: _LL38:
# 277
 Cyc_RgnOrder_add_to_search(& _tmp67,po->these_outlive_unique);
Cyc_RgnOrder_add_to_search(& _tmp67,po->these_outlive_heap);
break;case Cyc_Absyn_Unique: _LL39:
# 281
 Cyc_RgnOrder_add_to_search(& _tmp67,po->these_outlive_unique);break;case Cyc_Absyn_Top: _LL3A:
# 286
 break;}else{
# 290
const char*_tmp129;void*_tmp128;(_tmp128=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp129="RgnOrder: rgn not of correct kind",_tag_dyneither(_tmp129,sizeof(char),34))),_tag_dyneither(_tmp128,sizeof(void*),0)));}{
# 292
void*_tmp6D=r;struct Cyc_Absyn_Tvar*_tmp6F;_LL3D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D;if(_tmp6E->tag != 2)goto _LL3F;else{_tmp6F=_tmp6E->f1;}}_LL3E:
# 294
{struct Cyc_List_List*_tmp12A;_tmp67.seen=((_tmp12A=_region_malloc(listrgn,sizeof(*_tmp12A)),((_tmp12A->hd=_tmp6F,((_tmp12A->tl=_tmp67.seen,_tmp12A))))));}
{struct Cyc_List_List*_tmp12B;_tmp67.todo=((_tmp12B=_region_malloc(listrgn,sizeof(*_tmp12B)),((_tmp12B->hd=_tmp6F,((_tmp12B->tl=_tmp67.todo,_tmp12B))))));}
goto _LL40;_LL3F:;_LL40:
 return _tmp67;_LL3C:;};}
# 303
static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
struct _RegionHandle _tmp73=_new_region("listrgn");struct _RegionHandle*listrgn=& _tmp73;_push_region(listrgn);
{void*_stmttmp9=Cyc_Tcutil_compress(eff);void*_tmp74=_stmttmp9;void*_tmp76;void*_tmp78;_LL42: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp74;if(_tmp75->tag != 23)goto _LL44;else{_tmp76=(void*)_tmp75->f1;}}_LL43: {
# 308
void*_stmttmpC=Cyc_Tcutil_compress(_tmp76);void*_tmp79=_stmttmpC;struct Cyc_Absyn_Tvar*_tmp7E;_LL49: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp7A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp79;if(_tmp7A->tag != 21)goto _LL4B;}_LL4A: {
# 311
struct Cyc_Absyn_Kind*_stmttmpE=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));enum Cyc_Absyn_KindQual _tmp80;enum Cyc_Absyn_AliasQual _tmp81;struct Cyc_Absyn_Kind*_tmp7F=_stmttmpE;_tmp80=_tmp7F->kind;_tmp81=_tmp7F->aliasqual;{
int _tmp82=_tmp80 == Cyc_Absyn_RgnKind  && _tmp81 != Cyc_Absyn_Top;_npop_handler(0);return _tmp82;};}_LL4B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp79;if(_tmp7B->tag != 22)goto _LL4D;}_LL4C:
# 316
 if(Cyc_Tcutil_compress(rgn)== (void*)& Cyc_Absyn_RefCntRgn_val){int _tmp83=1;_npop_handler(0);return _tmp83;}
goto _LL4E;_LL4D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp79;if(_tmp7C->tag != 20)goto _LL4F;}_LL4E: {
# 320
struct Cyc_Absyn_Kind*_stmttmpD=Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn));enum Cyc_Absyn_KindQual _tmp85;enum Cyc_Absyn_AliasQual _tmp86;struct Cyc_Absyn_Kind*_tmp84=_stmttmpD;_tmp85=_tmp84->kind;_tmp86=_tmp84->aliasqual;{
int _tmp87=_tmp85 == Cyc_Absyn_RgnKind  && _tmp86 == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp87;};}_LL4F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp79;if(_tmp7D->tag != 2)goto _LL51;else{_tmp7E=_tmp7D->f1;}}_LL50: {
# 330
struct Cyc_RgnOrder_OutlivesEnv _tmp88=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
while(_tmp88.todo != 0){
struct Cyc_Absyn_Tvar*_tmp89=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp88.todo))->hd;
_tmp88.todo=((struct Cyc_List_List*)_check_null(_tmp88.todo))->tl;
# 336
if(Cyc_Absyn_tvar_cmp(_tmp89,_tmp7E)== 0){
# 338
int _tmp8A=1;_npop_handler(0);return _tmp8A;}
# 344
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp89))
# 348
Cyc_RgnOrder_add_to_search(& _tmp88,(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp89))->these_outlive);else{
# 350
struct Cyc_String_pa_PrintArg_struct _tmp133;void*_tmp132[1];const char*_tmp131;void*_tmp130;(_tmp130=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp133.tag=0,((_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7E->name),((_tmp132[0]=& _tmp133,Cyc_aprintf(((_tmp131="RgnOrder: type variable %s not in dict",_tag_dyneither(_tmp131,sizeof(char),39))),_tag_dyneither(_tmp132,sizeof(void*),1)))))))),_tag_dyneither(_tmp130,sizeof(void*),0)));}}{
# 352
int _tmp8F=0;_npop_handler(0);return _tmp8F;};}_LL51:;_LL52: {
int _tmp90=0;_npop_handler(0);return _tmp90;}_LL48:;}_LL44: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp74;if(_tmp77->tag != 25)goto _LL46;else{_tmp78=(void*)_tmp77->f1;}}_LL45: {
# 356
void*_stmttmpA=Cyc_Tcutil_compress(_tmp78);void*_tmp91=_stmttmpA;struct Cyc_Absyn_Tvar*_tmp93;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp91;if(_tmp92->tag != 2)goto _LL56;else{_tmp93=_tmp92->f1;}}_LL55: {
# 362
struct Cyc_RgnOrder_OutlivesEnv _tmp94=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
struct Cyc_Absyn_Kind*_stmttmpB=Cyc_Tcutil_typ_kind(rgn);enum Cyc_Absyn_KindQual _tmp96;enum Cyc_Absyn_AliasQual _tmp97;struct Cyc_Absyn_Kind*_tmp95=_stmttmpB;_tmp96=_tmp95->kind;_tmp97=_tmp95->aliasqual;
# 365
if(_tmp96 == Cyc_Absyn_RgnKind){
if(_tmp97 == Cyc_Absyn_Aliasable){
if(Cyc_RgnOrder_contains_rgnseff(_tmp93,po->these_outlive_heap) || 
Cyc_RgnOrder_contains_rgnseff(_tmp93,po->these_outlive_unique)){
int _tmp98=1;_npop_handler(0);return _tmp98;}}else{
# 371
if(_tmp97 == Cyc_Absyn_Unique){
if(Cyc_RgnOrder_contains_rgnseff(_tmp93,po->these_outlive_unique)){
int _tmp99=1;_npop_handler(0);return _tmp99;}}}}
# 376
while(_tmp94.todo != 0){
struct Cyc_Absyn_Tvar*_tmp9A=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp94.todo))->hd;
_tmp94.todo=((struct Cyc_List_List*)_check_null(_tmp94.todo))->tl;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp9A)){
void*_tmp9B=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp9A))->these_outlive;
if(Cyc_RgnOrder_contains_rgnseff(_tmp93,_tmp9B)){
int _tmp9C=1;_npop_handler(0);return _tmp9C;}
Cyc_RgnOrder_add_to_search(& _tmp94,_tmp9B);}else{
const char*_tmp136;void*_tmp135;(_tmp135=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136="RgnOrder: type variable not in dict",_tag_dyneither(_tmp136,sizeof(char),36))),_tag_dyneither(_tmp135,sizeof(void*),0)));}}{
# 386
int _tmp9F=0;_npop_handler(0);return _tmp9F;};}_LL56:;_LL57: {
int _tmpA0=0;_npop_handler(0);return _tmpA0;}_LL53:;}_LL46:;_LL47: {
# 389
int _tmpA1=0;_npop_handler(0);return _tmpA1;}_LL41:;}
# 305
;_pop_region(listrgn);}
# 396
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_stmttmpF=Cyc_Tcutil_compress(eff);void*_tmpA2=_stmttmpF;struct Cyc_List_List*_tmpA4;_LL59: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpA2;if(_tmpA3->tag != 24)goto _LL5B;else{_tmpA4=_tmpA3->f1;}}_LL5A:
# 400
 for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
if(!Cyc_RgnOrder_effect_outlives(po,(void*)_tmpA4->hd,rgn))
return 0;}
return 1;_LL5B:;_LL5C:
# 405
 return Cyc_RgnOrder_atomic_effect_outlives(po,eff,rgn);_LL58:;};}
# 409
static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_stmttmp10=Cyc_Tcutil_compress(eff);void*_tmpA5=_stmttmp10;struct Cyc_List_List*_tmpA7;void*_tmpA9;void*_tmpAB;_LL5E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpA5;if(_tmpA6->tag != 24)goto _LL60;else{_tmpA7=_tmpA6->f1;}}_LL5F:
# 413
 for(0;_tmpA7 != 0;_tmpA7=_tmpA7->tl){
Cyc_RgnOrder_pin_effect((void*)_tmpA7->hd,bd);}
return;_LL60: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpA5;if(_tmpA8->tag != 25)goto _LL62;else{_tmpA9=(void*)_tmpA8->f1;}}_LL61: {
# 417
void*_stmttmp12=Cyc_Tcutil_compress(_tmpA9);void*_tmpAD=_stmttmp12;_LL69: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LL6B;}_LL6A:
 Cyc_Tcutil_unify(_tmpA9,Cyc_Absyn_uint_typ);return;_LL6B:;_LL6C:
 return;_LL68:;}_LL62: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpA5;if(_tmpAA->tag != 23)goto _LL64;else{_tmpAB=(void*)_tmpAA->f1;}}_LL63: {
# 422
void*_stmttmp11=Cyc_Tcutil_compress(_tmpAB);void*_tmpAF=_stmttmp11;_LL6E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F:
 Cyc_Tcutil_unify(_tmpAB,bd);return;_LL70:;_LL71:
 return;_LL6D:;}_LL64: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpA5;if(_tmpAC->tag != 1)goto _LL66;}_LL65:
# 426
 Cyc_Tcutil_unify(eff,Cyc_Absyn_empty_effect);return;_LL66:;_LL67:
 return;_LL5D:;};}
# 441 "rgnorder.cyc"
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin){
# 444
{struct Cyc_List_List*_tmpB1=constraints;for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
struct _tuple0*_stmttmp13=(struct _tuple0*)_tmpB1->hd;void*_tmpB3;struct _tuple0*_tmpB2=_stmttmp13;_tmpB3=_tmpB2->f2;{
void*_stmttmp14=Cyc_Tcutil_compress(_tmpB3);void*_tmpB4=_stmttmp14;_LL73: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB4;if(_tmpB5->tag != 1)goto _LL75;}_LL74:
# 448
 if(do_pin)
Cyc_Tcutil_unify(_tmpB3,default_bound);
goto _LL72;_LL75:;_LL76:
 goto _LL72;_LL72:;};}}
# 454
{struct Cyc_List_List*_tmpB6=constraints;for(0;_tmpB6 != 0;_tmpB6=_tmpB6->tl){
struct _tuple0*_stmttmp15=(struct _tuple0*)_tmpB6->hd;void*_tmpB8;void*_tmpB9;struct _tuple0*_tmpB7=_stmttmp15;_tmpB8=_tmpB7->f1;_tmpB9=_tmpB7->f2;
if(do_pin)
Cyc_RgnOrder_pin_effect(_tmpB8,_tmpB9);
if(!Cyc_RgnOrder_effect_outlives(po,_tmpB8,_tmpB9))
return 0;}}
# 461
return 1;}
# 464
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
# 466
eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff2));{
void*_tmpBA=eff2;struct Cyc_List_List*_tmpBC;void*_tmpBE;void*_tmpC0;_LL78: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpBA;if(_tmpBB->tag != 24)goto _LL7A;else{_tmpBC=_tmpBB->f1;}}_LL79:
# 469
 for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)_tmpBC->hd))
return 1;}
return 0;_LL7A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpBD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmpBA;if(_tmpBD->tag != 23)goto _LL7C;else{_tmpBE=(void*)_tmpBD->f1;}}_LL7B:
 return Cyc_RgnOrder_effect_outlives(po,eff1,_tmpBE);_LL7C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpBA;if(_tmpBF->tag != 25)goto _LL7E;else{_tmpC0=(void*)_tmpBF->f1;}}_LL7D: {
# 475
void*_tmpC1=eff1;void*_tmpC3;_LL81: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpC2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmpC1;if(_tmpC2->tag != 25)goto _LL83;else{_tmpC3=(void*)_tmpC2->f1;}}_LL82:
# 477
{struct _tuple0 _tmp137;struct _tuple0 _stmttmp16=(_tmp137.f1=Cyc_Tcutil_compress(_tmpC0),((_tmp137.f2=Cyc_Tcutil_compress(_tmpC3),_tmp137)));struct _tuple0 _tmpC4=_stmttmp16;struct Cyc_Absyn_Tvar*_tmpC6;struct Cyc_Absyn_Tvar*_tmpC8;_LL86:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC4.f1;if(_tmpC5->tag != 2)goto _LL88;else{_tmpC6=_tmpC5->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC4.f2;if(_tmpC7->tag != 2)goto _LL88;else{_tmpC8=_tmpC7->f1;}};_LL87:
 return Cyc_Absyn_tvar_cmp(_tmpC6,_tmpC8)== 0;_LL88:;_LL89:
 goto _LL85;_LL85:;}
# 481
goto _LL84;_LL83:;_LL84:
 return eff1 == (void*)& Cyc_Absyn_HeapRgn_val;_LL80:;}_LL7E:;_LL7F:
# 484
 return eff1 == (void*)& Cyc_Absyn_HeapRgn_val;_LL77:;};}
# 488
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff1));{
void*_tmpCA=eff1;struct Cyc_List_List*_tmpCC;_LL8B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmpCA;if(_tmpCB->tag != 24)goto _LL8D;else{_tmpCC=_tmpCB->f1;}}_LL8C:
# 492
 for(0;_tmpCC != 0;_tmpCC=_tmpCC->tl){
if(!Cyc_RgnOrder_eff_outlives_eff(po,(void*)_tmpCC->hd,eff2))
return 0;}
return 1;_LL8D:;_LL8E:
 return Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,eff2);_LL8A:;};}struct _tuple11{struct Cyc_Absyn_Tvar*f1;struct Cyc_RgnOrder_RgnInfo*f2;};
# 501
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){
struct _RegionHandle _tmpCD=_new_region("r");struct _RegionHandle*r=& _tmpCD;_push_region(r);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,po->d);
struct _tuple11 _tmpCE=*((struct _tuple11*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,po->d);
{const char*_tmp13A;void*_tmp139;(_tmp139=0,Cyc_fprintf(Cyc_stderr,((_tmp13A="region po:\n",_tag_dyneither(_tmp13A,sizeof(char),12))),_tag_dyneither(_tmp139,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple11*))Cyc_Iter_next)(iter,& _tmpCE)){
const char*_tmp13F;void*_tmp13E[2];struct Cyc_String_pa_PrintArg_struct _tmp13D;struct Cyc_String_pa_PrintArg_struct _tmp13C;(_tmp13C.tag=0,((_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((_tmpCE.f2)->these_outlive)),((_tmp13D.tag=0,((_tmp13D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(_tmpCE.f1)->name),((_tmp13E[0]=& _tmp13D,((_tmp13E[1]=& _tmp13C,Cyc_fprintf(Cyc_stderr,((_tmp13F="  %s outlived by %s\n",_tag_dyneither(_tmp13F,sizeof(char),21))),_tag_dyneither(_tmp13E,sizeof(void*),2)))))))))))));}
# 510
{const char*_tmp143;void*_tmp142[1];struct Cyc_String_pa_PrintArg_struct _tmp141;(_tmp141.tag=0,((_tmp141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(po->these_outlive_heap)),((_tmp142[0]=& _tmp141,Cyc_fprintf(Cyc_stderr,((_tmp143="  these outlive heap: %s\n",_tag_dyneither(_tmp143,sizeof(char),26))),_tag_dyneither(_tmp142,sizeof(void*),1)))))));}{
const char*_tmp147;void*_tmp146[1];struct Cyc_String_pa_PrintArg_struct _tmp145;(_tmp145.tag=0,((_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(po->these_outlive_unique)),((_tmp146[0]=& _tmp145,Cyc_fprintf(Cyc_stderr,((_tmp147="  these outlive unique: %s\n",_tag_dyneither(_tmp147,sizeof(char),28))),_tag_dyneither(_tmp146,sizeof(void*),1)))))));};}
# 503
;_pop_region(r);}
