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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};
# 35 "splay.h"
typedef int Cyc_Splay_dummy_t;struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
# 49
int Cyc_Splay_splay(int(*f)(void*,void*),void*,void*);struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};
# 49
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*cmp)(void*,void*));
# 56
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);
# 60
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*k);
# 64
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 69
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*k,void*v);
# 73
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*l);
# 77
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 81
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*k);
# 84
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*k);
# 89
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*k);
# 94
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*k);
# 98
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum);
# 102
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d,void*accum);
# 106
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 110
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 114
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d);
# 116
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d);
# 120
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d);
# 125
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d);struct _tuple0{void*f1;void*f2;};
# 129
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d);
# 134
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d);char Cyc_SlowDict_Absent[7]="Absent";char Cyc_SlowDict_Present[8]="Present";
# 32 "slowdict.cyc"
struct Cyc_SlowDict_Absent_exn_struct Cyc_SlowDict_Absent_val={Cyc_SlowDict_Absent};
struct Cyc_SlowDict_Present_exn_struct Cyc_SlowDict_Present_val={Cyc_SlowDict_Present};struct Cyc_SlowDict_Dict{int(*reln)(void*,void*);void*tree;};
# 41
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*comp)(void*,void*)){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmp86;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp85;void*t=(void*)((_tmp85=_cycalloc_atomic(sizeof(*_tmp85)),((_tmp85[0]=((_tmp86.tag=0,((_tmp86.f1=0,_tmp86)))),_tmp85))));
struct Cyc_SlowDict_Dict*_tmp87;return(_tmp87=_cycalloc(sizeof(*_tmp87)),((_tmp87->reln=comp,((_tmp87->tree=t,_tmp87)))));}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){
void*_stmttmp0=d->tree;void*_tmp5=_stmttmp0;_LL1: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp6=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp5;if(_tmp6->tag != 0)goto _LL3;}_LL2:
 return 1;_LL3: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp7=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5;if(_tmp7->tag != 1)goto _LL0;}_LL4:
 return 0;_LL0:;}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){
return Cyc_Splay_splay(d->reln,key,d->tree);}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmp8A;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp89;void*leaf=(void*)((_tmp89=_cycalloc_atomic(sizeof(*_tmp89)),((_tmp89[0]=((_tmp8A.tag=0,((_tmp8A.f1=0,_tmp8A)))),_tmp89))));
void*newleft=leaf;void*newright=leaf;
if(Cyc_Splay_splay(d->reln,key,d->tree)){
# 65
void*_stmttmp1=d->tree;void*_tmp8=_stmttmp1;struct Cyc_Splay_noderef*_tmpA;_LL6: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp9=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp8;if(_tmp9->tag != 1)goto _LL8;else{_tmpA=_tmp9->f1;}}_LL7:
# 67
 newleft=(_tmpA->v)->left;
newright=(_tmpA->v)->right;
goto _LL5;_LL8:;_LL9:
# 71
 goto _LL5;_LL5:;}else{
# 75
void*_stmttmp2=d->tree;void*_tmpB=_stmttmp2;struct Cyc_Splay_noderef*_tmpD;_LLB: {struct Cyc_Splay_Node_Splay_tree_struct*_tmpC=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmpB;if(_tmpC->tag != 1)goto _LLD;else{_tmpD=_tmpC->f1;}}_LLC: {
# 77
struct Cyc_Splay_node*_tmpF=_tmpD->v;
if((d->reln)(key,_tmpF->key)< 0){
newleft=_tmpF->left;{
struct Cyc_Splay_Node_Splay_tree_struct _tmp94;struct Cyc_Splay_node*_tmp93;struct Cyc_Splay_noderef*_tmp92;struct Cyc_Splay_Node_Splay_tree_struct*_tmp91;newright=(void*)((_tmp91=_cycalloc(sizeof(*_tmp91)),((_tmp91[0]=((_tmp94.tag=1,((_tmp94.f1=((_tmp92=_cycalloc(sizeof(*_tmp92)),((_tmp92->v=((_tmp93=_cycalloc(sizeof(*_tmp93)),((_tmp93->key=_tmpF->key,((_tmp93->data=_tmpF->data,((_tmp93->left=leaf,((_tmp93->right=_tmpF->right,_tmp93)))))))))),_tmp92)))),_tmp94)))),_tmp91))));};}else{
# 84
{struct Cyc_Splay_Node_Splay_tree_struct _tmp9E;struct Cyc_Splay_node*_tmp9D;struct Cyc_Splay_noderef*_tmp9C;struct Cyc_Splay_Node_Splay_tree_struct*_tmp9B;newleft=(void*)((_tmp9B=_cycalloc(sizeof(*_tmp9B)),((_tmp9B[0]=((_tmp9E.tag=1,((_tmp9E.f1=((_tmp9C=_cycalloc(sizeof(*_tmp9C)),((_tmp9C->v=((_tmp9D=_cycalloc(sizeof(*_tmp9D)),((_tmp9D->key=_tmpF->key,((_tmp9D->data=_tmpF->data,((_tmp9D->left=_tmpF->left,((_tmp9D->right=leaf,_tmp9D)))))))))),_tmp9C)))),_tmp9E)))),_tmp9B))));}
# 86
newright=_tmpF->right;}
# 88
goto _LLA;}_LLD: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpE=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmpB;if(_tmpE->tag != 0)goto _LLA;}_LLE:
# 90
 goto _LLA;_LLA:;}{
# 93
struct Cyc_Splay_Node_Splay_tree_struct*_tmpAD;struct Cyc_Splay_noderef*_tmpAC;struct Cyc_Splay_node*_tmpAB;struct Cyc_Splay_Node_Splay_tree_struct _tmpAA;struct Cyc_SlowDict_Dict*_tmpA9;return(_tmpA9=_cycalloc(sizeof(*_tmpA9)),((_tmpA9->reln=d->reln,((_tmpA9->tree=(void*)((_tmpAD=_cycalloc(sizeof(*_tmpAD)),((_tmpAD[0]=((_tmpAA.tag=1,((_tmpAA.f1=((_tmpAC=_cycalloc(sizeof(*_tmpAC)),((_tmpAC->v=(
(_tmpAB=_cycalloc(sizeof(*_tmpAB)),((_tmpAB->key=key,((_tmpAB->data=data,((_tmpAB->left=newleft,((_tmpAB->right=newright,_tmpAB)))))))))),_tmpAC)))),_tmpAA)))),_tmpAD)))),_tmpA9)))));};}
# 98
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){
# 100
if(Cyc_Splay_splay(d->reln,key,d->tree))
(int)_throw((void*)& Cyc_SlowDict_Present_val);
return Cyc_SlowDict_insert(d,key,data);}
# 105
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*kds){
# 107
for(0;kds != 0;kds=kds->tl){
d=Cyc_SlowDict_insert(d,(*((struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}
return d;}
# 112
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpB0;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpAF;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp1F=(_tmpAF=_cycalloc_atomic(sizeof(*_tmpAF)),((_tmpAF[0]=((_tmpB0.tag=0,((_tmpB0.f1=0,_tmpB0)))),_tmpAF)));
struct Cyc_Splay_Node_Splay_tree_struct*_tmpBF;struct Cyc_Splay_noderef*_tmpBE;struct Cyc_Splay_node*_tmpBD;struct Cyc_Splay_Node_Splay_tree_struct _tmpBC;struct Cyc_SlowDict_Dict*_tmpBB;return(_tmpBB=_cycalloc(sizeof(*_tmpBB)),((_tmpBB->reln=comp,((_tmpBB->tree=(void*)((_tmpBF=_cycalloc(sizeof(*_tmpBF)),((_tmpBF[0]=((_tmpBC.tag=1,((_tmpBC.f1=((_tmpBE=_cycalloc(sizeof(*_tmpBE)),((_tmpBE->v=((_tmpBD=_cycalloc(sizeof(*_tmpBD)),((_tmpBD->key=key,((_tmpBD->data=data,((_tmpBD->left=(void*)_tmp1F,((_tmpBD->right=(void*)_tmp1F,_tmpBD)))))))))),_tmpBE)))),_tmpBC)))),_tmpBF)))),_tmpBB)))));}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp3=d->tree;void*_tmp27=_stmttmp3;struct Cyc_Splay_noderef*_tmp29;_LL10: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp28=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp27;if(_tmp28->tag != 1)goto _LL12;else{_tmp29=_tmp28->f1;}}_LL11:
 return(_tmp29->v)->data;_LL12: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp2A=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp27;if(_tmp2A->tag != 0)goto _LLF;}_LL13: {
struct Cyc_Core_Impossible_exn_struct _tmpC5;const char*_tmpC4;struct Cyc_Core_Impossible_exn_struct*_tmpC3;(int)_throw((void*)((_tmpC3=_cycalloc(sizeof(*_tmpC3)),((_tmpC3[0]=((_tmpC5.tag=Cyc_Core_Impossible,((_tmpC5.f1=((_tmpC4="Dict::lookup",_tag_dyneither(_tmpC4,sizeof(char),13))),_tmpC5)))),_tmpC3)))));}_LLF:;}
# 123
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp4=d->tree;void*_tmp2E=_stmttmp4;struct Cyc_Splay_noderef*_tmp30;_LL15: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp2F=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp2E;if(_tmp2F->tag != 1)goto _LL17;else{_tmp30=_tmp2F->f1;}}_LL16: {
struct Cyc_Core_Opt*_tmpC6;return(_tmpC6=_cycalloc(sizeof(*_tmpC6)),((_tmpC6->v=(_tmp30->v)->data,_tmpC6)));}_LL17: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp31=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp2E;if(_tmp31->tag != 0)goto _LL14;}_LL18: {
struct Cyc_Core_Impossible_exn_struct _tmpCC;const char*_tmpCB;struct Cyc_Core_Impossible_exn_struct*_tmpCA;(int)_throw((void*)((_tmpCA=_cycalloc(sizeof(*_tmpCA)),((_tmpCA[0]=((_tmpCC.tag=Cyc_Core_Impossible,((_tmpCC.f1=((_tmpCB="Dict::lookup",_tag_dyneither(_tmpCB,sizeof(char),13))),_tmpCC)))),_tmpCA)))));}_LL14:;}
# 132
return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp5=d->tree;void*_tmp36=_stmttmp5;struct Cyc_Splay_noderef*_tmp39;_LL1A: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp37=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp36;if(_tmp37->tag != 0)goto _LL1C;}_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpD2;const char*_tmpD1;struct Cyc_Core_Impossible_exn_struct*_tmpD0;(int)_throw((void*)((_tmpD0=_cycalloc(sizeof(*_tmpD0)),((_tmpD0[0]=((_tmpD2.tag=Cyc_Core_Impossible,((_tmpD2.f1=((_tmpD1="Dict::lookup",_tag_dyneither(_tmpD1,sizeof(char),13))),_tmpD2)))),_tmpD0)))));}_LL1C: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp38=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp36;if(_tmp38->tag != 1)goto _LL19;else{_tmp39=_tmp38->f1;}}_LL1D: {
# 142
struct Cyc_Splay_node*n=_tmp39->v;
void*_stmttmp6=n->left;void*_tmp3D=_stmttmp6;struct Cyc_Splay_noderef*_tmp40;_LL1F: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp3E=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp3D;if(_tmp3E->tag != 0)goto _LL21;}_LL20: {
struct Cyc_SlowDict_Dict*_tmpD3;return(_tmpD3=_cycalloc(sizeof(*_tmpD3)),((_tmpD3->reln=d->reln,((_tmpD3->tree=n->right,_tmpD3)))));}_LL21: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp3F=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3D;if(_tmp3F->tag != 1)goto _LL1E;else{_tmp40=_tmp3F->f1;}}_LL22: {
# 146
void*_stmttmp7=n->right;void*_tmp42=_stmttmp7;struct Cyc_Splay_noderef*_tmp45;_LL24: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp43=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp42;if(_tmp43->tag != 0)goto _LL26;}_LL25: {
struct Cyc_SlowDict_Dict*_tmpD4;return(_tmpD4=_cycalloc(sizeof(*_tmpD4)),((_tmpD4->reln=d->reln,((_tmpD4->tree=n->left,_tmpD4)))));}_LL26: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp44=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp42;if(_tmp44->tag != 1)goto _LL23;else{_tmp45=_tmp44->f1;}}_LL27:
# 149
 Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,n->left);{
struct Cyc_Splay_node*newtop=_tmp40->v;
struct Cyc_Splay_Node_Splay_tree_struct*_tmpE3;struct Cyc_Splay_noderef*_tmpE2;struct Cyc_Splay_node*_tmpE1;struct Cyc_Splay_Node_Splay_tree_struct _tmpE0;struct Cyc_SlowDict_Dict*_tmpDF;return(_tmpDF=_cycalloc(sizeof(*_tmpDF)),((_tmpDF->reln=d->reln,((_tmpDF->tree=(void*)(
(_tmpE3=_cycalloc(sizeof(*_tmpE3)),((_tmpE3[0]=((_tmpE0.tag=1,((_tmpE0.f1=((_tmpE2=_cycalloc(sizeof(*_tmpE2)),((_tmpE2->v=((_tmpE1=_cycalloc(sizeof(*_tmpE1)),((_tmpE1->key=newtop->key,((_tmpE1->data=newtop->data,((_tmpE1->left=newtop->left,((_tmpE1->right=n->right,_tmpE1)))))))))),_tmpE2)))),_tmpE0)))),_tmpE3)))),_tmpDF)))));};_LL23:;}_LL1E:;}_LL19:;}else{
# 158
return d;}}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){
struct Cyc_SlowDict_Dict*_tmp4C=Cyc_SlowDict_delete(d,key);
if(d == _tmp4C)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);
return _tmp4C;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){
void*_tmp4D=t;struct Cyc_Splay_noderef*_tmp50;_LL29: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp4E=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp4D;if(_tmp4E->tag != 0)goto _LL2B;}_LL2A:
# 171
 return accum;_LL2B: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp4F=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp4D;if(_tmp4F->tag != 1)goto _LL28;else{_tmp50=_tmp4F->f1;}}_LL2C: {
# 173
struct Cyc_Splay_node*n=_tmp50->v;
return f(n->key,n->data,Cyc_SlowDict_fold_tree(f,n->left,
Cyc_SlowDict_fold_tree(f,n->right,accum)));}_LL28:;}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){
return Cyc_SlowDict_fold_tree(f,d->tree,accum);}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){
void*_tmp51=t;struct Cyc_Splay_noderef*_tmp54;_LL2E: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp52=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp51;if(_tmp52->tag != 0)goto _LL30;}_LL2F:
# 185
 return accum;_LL30: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp53=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp51;if(_tmp53->tag != 1)goto _LL2D;else{_tmp54=_tmp53->f1;}}_LL31: {
# 187
struct Cyc_Splay_node*n=_tmp54->v;
return f(env,n->key,n->data,
Cyc_SlowDict_fold_tree_c(f,env,n->left,Cyc_SlowDict_fold_tree_c(f,env,n->right,accum)));}_LL2D:;}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){
return Cyc_SlowDict_fold_tree_c(f,env,dict->tree,accum);}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){
void*_tmp55=t;struct Cyc_Splay_noderef*_tmp58;_LL33: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp56=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp55;if(_tmp56->tag != 0)goto _LL35;}_LL34:
 goto _LL32;_LL35: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp57=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp55;if(_tmp57->tag != 1)goto _LL32;else{_tmp58=_tmp57->f1;}}_LL36: {
# 200
struct Cyc_Splay_node*_tmp59=_tmp58->v;
Cyc_SlowDict_app_tree(f,_tmp59->left);
f(_tmp59->key,_tmp59->data);
Cyc_SlowDict_app_tree(f,_tmp59->right);
goto _LL32;}_LL32:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,d->tree);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){
void*_tmp5A=t;struct Cyc_Splay_noderef*_tmp5D;_LL38: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp5B=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp5A;if(_tmp5B->tag != 0)goto _LL3A;}_LL39:
 goto _LL37;_LL3A: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp5C=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5A;if(_tmp5C->tag != 1)goto _LL37;else{_tmp5D=_tmp5C->f1;}}_LL3B: {
# 215
struct Cyc_Splay_node*n=_tmp5D->v;
Cyc_SlowDict_iter_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_iter_tree(f,n->right);
goto _LL37;}_LL37:;}
# 222
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree(f,d->tree);}
# 226
static void Cyc_SlowDict_app_tree_c(void*(*f)(void*,void*,void*),void*env,void*t){
void*_tmp5E=t;struct Cyc_Splay_noderef*_tmp61;_LL3D: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp5F=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp5E;if(_tmp5F->tag != 0)goto _LL3F;}_LL3E:
 goto _LL3C;_LL3F: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp60=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5E;if(_tmp60->tag != 1)goto _LL3C;else{_tmp61=_tmp60->f1;}}_LL40: {
# 230
struct Cyc_Splay_node*n=_tmp61->v;
Cyc_SlowDict_app_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_app_tree_c(f,env,n->right);
goto _LL3C;}_LL3C:;}
# 237
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree_c(f,env,d->tree);}
# 241
static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),void*env,void*t){
void*_tmp62=t;struct Cyc_Splay_noderef*_tmp65;_LL42: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp63=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp62;if(_tmp63->tag != 0)goto _LL44;}_LL43:
 goto _LL41;_LL44: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp64=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp62;if(_tmp64->tag != 1)goto _LL41;else{_tmp65=_tmp64->f1;}}_LL45: {
# 245
struct Cyc_Splay_node*n=_tmp65->v;
Cyc_SlowDict_iter_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_iter_tree_c(f,env,n->right);
goto _LL41;}_LL41:;}
# 252
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree_c(f,env,d->tree);}
# 256
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){
void*_tmp66=t;struct Cyc_Splay_noderef*_tmp69;_LL47: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp67=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp66;if(_tmp67->tag != 0)goto _LL49;}_LL48: {
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpE6;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpE5;return(void*)((_tmpE5=_cycalloc_atomic(sizeof(*_tmpE5)),((_tmpE5[0]=((_tmpE6.tag=0,((_tmpE6.f1=0,_tmpE6)))),_tmpE5))));}_LL49: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp68=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp66;if(_tmp68->tag != 1)goto _LL46;else{_tmp69=_tmp68->f1;}}_LL4A: {
# 260
struct Cyc_Splay_node*_tmp6C=_tmp69->v;
struct Cyc_Splay_Node_Splay_tree_struct _tmpF0;struct Cyc_Splay_node*_tmpEF;struct Cyc_Splay_noderef*_tmpEE;struct Cyc_Splay_Node_Splay_tree_struct*_tmpED;return(void*)((_tmpED=_cycalloc(sizeof(*_tmpED)),((_tmpED[0]=((_tmpF0.tag=1,((_tmpF0.f1=((_tmpEE=_cycalloc(sizeof(*_tmpEE)),((_tmpEE->v=((_tmpEF=_cycalloc(sizeof(*_tmpEF)),((_tmpEF->key=_tmp6C->key,((_tmpEF->data=
f(_tmp6C->data),((_tmpEF->left=
Cyc_SlowDict_map_tree(f,_tmp6C->left),((_tmpEF->right=
Cyc_SlowDict_map_tree(f,_tmp6C->right),_tmpEF)))))))))),_tmpEE)))),_tmpF0)))),_tmpED))));}_LL46:;}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){
struct Cyc_SlowDict_Dict*_tmpF1;return(_tmpF1=_cycalloc(sizeof(*_tmpF1)),((_tmpF1->reln=d->reln,((_tmpF1->tree=Cyc_SlowDict_map_tree(f,d->tree),_tmpF1)))));}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){
void*_tmp72=t;struct Cyc_Splay_noderef*_tmp75;_LL4C: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp73=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp72;if(_tmp73->tag != 0)goto _LL4E;}_LL4D: {
struct Cyc_Splay_Leaf_Splay_tree_struct _tmpF4;struct Cyc_Splay_Leaf_Splay_tree_struct*_tmpF3;return(void*)((_tmpF3=_cycalloc_atomic(sizeof(*_tmpF3)),((_tmpF3[0]=((_tmpF4.tag=0,((_tmpF4.f1=0,_tmpF4)))),_tmpF3))));}_LL4E: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp74=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp72;if(_tmp74->tag != 1)goto _LL4B;else{_tmp75=_tmp74->f1;}}_LL4F: {
# 275
struct Cyc_Splay_node*n=_tmp75->v;
struct Cyc_Splay_Node_Splay_tree_struct _tmpFE;struct Cyc_Splay_node*_tmpFD;struct Cyc_Splay_noderef*_tmpFC;struct Cyc_Splay_Node_Splay_tree_struct*_tmpFB;return(void*)((_tmpFB=_cycalloc(sizeof(*_tmpFB)),((_tmpFB[0]=((_tmpFE.tag=1,((_tmpFE.f1=((_tmpFC=_cycalloc(sizeof(*_tmpFC)),((_tmpFC->v=((_tmpFD=_cycalloc(sizeof(*_tmpFD)),((_tmpFD->key=n->key,((_tmpFD->data=f(env,n->data),((_tmpFD->left=
Cyc_SlowDict_map_tree_c(f,env,n->left),((_tmpFD->right=
Cyc_SlowDict_map_tree_c(f,env,n->right),_tmpFD)))))))))),_tmpFC)))),_tmpFE)))),_tmpFB))));}_LL4B:;}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
struct Cyc_SlowDict_Dict*_tmpFF;return(_tmpFF=_cycalloc(sizeof(*_tmpFF)),((_tmpFF->reln=d->reln,((_tmpFF->tree=Cyc_SlowDict_map_tree_c(f,env,d->tree),_tmpFF)))));}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){
void*_stmttmp8=d->tree;void*_tmp7D=_stmttmp8;struct Cyc_Splay_noderef*_tmp80;_LL51: {struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp7E=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_tmp7D;if(_tmp7E->tag != 0)goto _LL53;}_LL52:
(int)_throw((void*)& Cyc_SlowDict_Absent_val);_LL53: {struct Cyc_Splay_Node_Splay_tree_struct*_tmp7F=(struct Cyc_Splay_Node_Splay_tree_struct*)_tmp7D;if(_tmp7F->tag != 1)goto _LL50;else{_tmp80=_tmp7F->f1;}}_LL54: {
struct _tuple0*_tmp100;return(_tmp100=_cycalloc(sizeof(*_tmp100)),((_tmp100->f1=(_tmp80->v)->key,((_tmp100->f2=(_tmp80->v)->data,_tmp100)))));}_LL50:;}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){
struct _tuple0*_tmp103;struct Cyc_List_List*_tmp102;return(_tmp102=_cycalloc(sizeof(*_tmp102)),((_tmp102->hd=((_tmp103=_cycalloc(sizeof(*_tmp103)),((_tmp103->f1=k,((_tmp103->f2=v,_tmp103)))))),((_tmp102->tl=accum,_tmp102)))));}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*accum))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}
