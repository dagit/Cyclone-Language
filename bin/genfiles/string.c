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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 310 "cycboot.h"
int toupper(int);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 111
struct _dyneither_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 235 "core.h"
struct _dyneither_ptr wrap_Cbuffer_as_buffer(char*,unsigned long);
# 244
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 53
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 55
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 56
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 63
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 65
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*,struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 72
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 73
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 76
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr,unsigned long);
# 80
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
# 81
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
# 82
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
# 83
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n);
# 84
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n);
# 85
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n);
# 86
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n);
# 87
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz);
# 98 "string.h"
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz);
# 99
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
# 100
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz);
# 101
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz);
# 102
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 103
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*,struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 108
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*,struct _dyneither_ptr,int ofs,unsigned long n);
# 112
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
# 113
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
# 119
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c);
# 120
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s,char c);
# 121
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s,char c);
# 122
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
# 123
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
# 124
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
# 125
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
# 126
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
# 127
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
# 128
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim);
# 132
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s);
# 133
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct _dyneither_ptr s);
# 134
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);
# 25 "strings.h"
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 26
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);
# 39 "string.cyc"
unsigned long Cyc_strlen(struct _dyneither_ptr s){
# 40
unsigned long i;
# 41
unsigned int sz=_get_dyneither_size(s,sizeof(char));
# 42
for(i=0;i < sz;++ i){
# 43
if(((const char*)s.curr)[(int)i]== '\000')
# 44
return i;}
# 46
return i;}
# 52
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
# 53
if((char*)s1.curr == (char*)s2.curr)
# 54
return 0;{
# 55
int i=0;
# 56
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
# 57
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
# 58
unsigned int minsz=sz1 < sz2?sz1: sz2;
# 59
{const char*_tmp80;const char*_tmp7F;minsz <= _get_dyneither_size(s1,sizeof(char)) && minsz <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp7F="minsz <= numelts(s1) && minsz <= numelts(s2)",_tag_dyneither(_tmp7F,sizeof(char),45))),((_tmp80="string.cyc",_tag_dyneither(_tmp80,sizeof(char),11))),59);}
# 60
while(i < minsz){
# 61
char c1=((const char*)s1.curr)[i];
# 62
char c2=((const char*)s2.curr)[i];
# 63
if(c1 == '\000'){
# 64
if(c2 == '\000')return 0;else{
# 65
return - 1;}}else{
# 66
if(c2 == '\000')return 1;else{
# 68
int diff=c1 - c2;
# 69
if(diff != 0)return diff;}}
# 71
++ i;}
# 73
if(sz1 == sz2)return 0;
# 74
if(minsz < sz2){
# 75
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
# 76
return - 1;}}else{
# 78
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
# 79
return 1;}}};}
# 83
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2){
# 84
return Cyc_strcmp((struct _dyneither_ptr)*s1,(struct _dyneither_ptr)*s2);}
# 87
inline static int Cyc_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 90
if(n <= 0)return 0;{
# 92
unsigned long min_len=len1 > len2?len2: len1;
# 93
unsigned long bound=min_len > n?n: min_len;
# 95
{const char*_tmp82;const char*_tmp81;bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp81="bound <= numelts(s1) && bound <= numelts(s2)",_tag_dyneither(_tmp81,sizeof(char),45))),((_tmp82="string.cyc",_tag_dyneither(_tmp82,sizeof(char),11))),95);}
# 97
{int i=0;for(0;i < bound;++ i){
# 98
int retc;
# 99
if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])!= 0)
# 100
return retc;}}
# 102
if(len1 < n  || len2 < n)
# 103
return(int)len1 - (int)len2;
# 104
return 0;};}
# 109
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
# 110
unsigned long len1=Cyc_strlen(s1);
# 111
unsigned long len2=Cyc_strlen(s2);
# 112
return Cyc_ncmp(s1,len1,s2,len2,n);}
# 119
int Cyc_zstrcmp(struct _dyneither_ptr a,struct _dyneither_ptr b){
# 120
if((char*)a.curr == (char*)b.curr)
# 121
return 0;{
# 122
unsigned long as=_get_dyneither_size(a,sizeof(char));
# 123
unsigned long bs=_get_dyneither_size(b,sizeof(char));
# 124
unsigned long min_length=as < bs?as: bs;
# 125
int i=- 1;
# 127
{const char*_tmp84;const char*_tmp83;min_length <= _get_dyneither_size(a,sizeof(char)) && min_length <= _get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp83="min_length <= numelts(a) && min_length <= numelts(b)",_tag_dyneither(_tmp83,sizeof(char),53))),((_tmp84="string.cyc",_tag_dyneither(_tmp84,sizeof(char),11))),127);}
# 129
while((++ i,i < min_length)){
# 130
int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
# 131
if(diff != 0)
# 132
return diff;}
# 134
return(int)as - (int)bs;};}
# 137
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
# 138
if(n <= 0)return 0;{
# 140
unsigned long s1size=_get_dyneither_size(s1,sizeof(char));
# 141
unsigned long s2size=_get_dyneither_size(s2,sizeof(char));
# 142
unsigned long min_size=s1size > s2size?s2size: s1size;
# 143
unsigned long bound=min_size > n?n: min_size;
# 145
{const char*_tmp86;const char*_tmp85;bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp85="bound <= numelts(s1) && bound <= numelts(s2)",_tag_dyneither(_tmp85,sizeof(char),45))),((_tmp86="string.cyc",_tag_dyneither(_tmp86,sizeof(char),11))),145);}
# 147
{int i=0;for(0;i < bound;++ i){
# 148
if(((const char*)s1.curr)[i]< ((const char*)s2.curr)[i])
# 149
return - 1;else{
# 150
if(((const char*)s2.curr)[i]< ((const char*)s1.curr)[i])
# 151
return 1;}}}
# 153
if(min_size <= bound)
# 154
return 0;
# 155
if(s1size < s2size)
# 156
return - 1;else{
# 158
return 1;}};}
# 162
int Cyc_zstrptrcmp(struct _dyneither_ptr*a,struct _dyneither_ptr*b){
# 163
if((int)a == (int)b)return 0;
# 164
return Cyc_zstrcmp((struct _dyneither_ptr)*a,(struct _dyneither_ptr)*b);}
# 172
inline static struct _dyneither_ptr Cyc_int_strcato(struct _dyneither_ptr dest,struct _dyneither_ptr src){
# 173
int i;
# 174
unsigned long dsize;unsigned long slen;unsigned long dlen;
# 176
dsize=_get_dyneither_size(dest,sizeof(char));
# 177
dlen=Cyc_strlen((struct _dyneither_ptr)dest);
# 178
slen=Cyc_strlen(src);
# 180
if(slen + dlen <= dsize){
# 181
{const char*_tmp88;const char*_tmp87;slen <= _get_dyneither_size(src,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp87="slen <= numelts(src)",_tag_dyneither(_tmp87,sizeof(char),21))),((_tmp88="string.cyc",_tag_dyneither(_tmp88,sizeof(char),11))),181);}
# 182
for(i=0;i < slen;++ i){
# 183
char _tmp8B;char _tmp8A;struct _dyneither_ptr _tmp89;(_tmp89=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen)),((_tmp8A=*((char*)_check_dyneither_subscript(_tmp89,sizeof(char),0)),((_tmp8B=((const char*)src.curr)[i],((_get_dyneither_size(_tmp89,sizeof(char))== 1  && (_tmp8A == '\000'  && _tmp8B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp89.curr)=_tmp8B)))))));}
# 185
if(i != dsize){
# 186
char _tmp8E;char _tmp8D;struct _dyneither_ptr _tmp8C;(_tmp8C=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen)),((_tmp8D=*((char*)_check_dyneither_subscript(_tmp8C,sizeof(char),0)),((_tmp8E='\000',((_get_dyneither_size(_tmp8C,sizeof(char))== 1  && (_tmp8D == '\000'  && _tmp8E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8C.curr)=_tmp8E)))))));}}else{
# 189
struct Cyc_Core_Invalid_argument_exn_struct _tmp94;const char*_tmp93;struct Cyc_Core_Invalid_argument_exn_struct*_tmp92;(int)_throw((void*)((_tmp92=_cycalloc(sizeof(*_tmp92)),((_tmp92[0]=((_tmp94.tag=Cyc_Core_Invalid_argument,((_tmp94.f1=((_tmp93="strcat",_tag_dyneither(_tmp93,sizeof(char),7))),_tmp94)))),_tmp92)))));}
# 190
return dest;}
# 195
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src){
# 196
return Cyc_int_strcato(dest,src);}
# 200
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _dyneither_ptr a,struct _dyneither_ptr b){
# 201
unsigned long _tmp13=Cyc_strlen(a);
# 202
unsigned long _tmp14=Cyc_strlen(b);
# 203
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,(_tmp13 + _tmp14)+ 1);
# 204
int i;int j;
# 205
{const char*_tmp96;const char*_tmp95;_tmp13 <= _get_dyneither_size(ans,sizeof(char)) && _tmp13 <= _get_dyneither_size(a,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp95="alen <= numelts(ans) && alen <= numelts(a)",_tag_dyneither(_tmp95,sizeof(char),43))),((_tmp96="string.cyc",_tag_dyneither(_tmp96,sizeof(char),11))),205);}
# 206
for(i=0;i < _tmp13;++ i){char _tmp99;char _tmp98;struct _dyneither_ptr _tmp97;(_tmp97=_dyneither_ptr_plus(ans,sizeof(char),i),((_tmp98=*((char*)_check_dyneither_subscript(_tmp97,sizeof(char),0)),((_tmp99=((const char*)a.curr)[i],((_get_dyneither_size(_tmp97,sizeof(char))== 1  && (_tmp98 == '\000'  && _tmp99 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp97.curr)=_tmp99)))))));}
# 207
{const char*_tmp9B;const char*_tmp9A;_tmp14 <= _get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp9A="blen <= numelts(b)",_tag_dyneither(_tmp9A,sizeof(char),19))),((_tmp9B="string.cyc",_tag_dyneither(_tmp9B,sizeof(char),11))),207);}
# 208
for(j=0;j < _tmp14;++ j){char _tmp9E;char _tmp9D;struct _dyneither_ptr _tmp9C;(_tmp9C=_dyneither_ptr_plus(ans,sizeof(char),i + j),((_tmp9D=*((char*)_check_dyneither_subscript(_tmp9C,sizeof(char),0)),((_tmp9E=((const char*)b.curr)[j],((_get_dyneither_size(_tmp9C,sizeof(char))== 1  && (_tmp9D == '\000'  && _tmp9E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp9C.curr)=_tmp9E)))))));}
# 209
return ans;}
# 212
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr a,struct _dyneither_ptr b){
# 213
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}
# 217
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
# 219
unsigned long len;
# 220
unsigned long total_len=0;
# 221
struct _dyneither_ptr ans;
# 222
struct _RegionHandle _tmp1F=_new_region("temp");struct _RegionHandle*temp=& _tmp1F;_push_region(temp);{
# 223
struct Cyc_List_List*_tmp9F;struct Cyc_List_List*lens=
# 224
(_tmp9F=_region_malloc(temp,sizeof(*_tmp9F)),((_tmp9F->hd=(void*)((unsigned long)0),((_tmp9F->tl=0,_tmp9F)))));
# 226
struct Cyc_List_List*end=lens;
# 227
{struct Cyc_List_List*p=strs;for(0;p != 0;p=p->tl){
# 228
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)p->hd));
# 229
total_len +=len;
# 230
{struct Cyc_List_List*_tmpA0;((struct Cyc_List_List*)_check_null(end))->tl=((_tmpA0=_region_malloc(temp,sizeof(*_tmpA0)),((_tmpA0->hd=(void*)len,((_tmpA0->tl=0,_tmpA0))))));}
# 231
end=end->tl;}}
# 233
lens=lens->tl;
# 234
ans=Cyc_Core_rnew_string(r,total_len + 1);{
# 235
unsigned long i=0;
# 236
while(strs != 0){
# 237
struct _dyneither_ptr _tmp21=*((struct _dyneither_ptr*)strs->hd);
# 238
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
# 239
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)_tmp21,len);
# 240
i +=len;
# 241
strs=strs->tl;
# 242
lens=lens->tl;}};}{
# 245
struct _dyneither_ptr _tmp23=ans;_npop_handler(0);return _tmp23;};
# 222
;_pop_region(temp);}
# 248
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){
# 249
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}
# 254
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,struct _dyneither_ptr separator){
# 255
if(strs == 0)return Cyc_Core_rnew_string(r,0);
# 256
if(strs->tl == 0)return Cyc_rstrdup(r,(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd));{
# 257
struct Cyc_List_List*_tmp24=strs;
# 258
struct _RegionHandle _tmp25=_new_region("temp");struct _RegionHandle*temp=& _tmp25;_push_region(temp);
# 259
{struct Cyc_List_List*_tmpA1;struct Cyc_List_List*lens=(_tmpA1=_region_malloc(temp,sizeof(*_tmpA1)),((_tmpA1->hd=(void*)((unsigned long)0),((_tmpA1->tl=0,_tmpA1)))));
# 260
struct Cyc_List_List*end=lens;
# 261
unsigned long len;
# 262
unsigned long total_len=0;
# 263
unsigned long list_len=0;
# 264
for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
# 265
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp24->hd));
# 266
total_len +=len;
# 267
{struct Cyc_List_List*_tmpA2;((struct Cyc_List_List*)_check_null(end))->tl=((_tmpA2=_region_malloc(temp,sizeof(*_tmpA2)),((_tmpA2->hd=(void*)len,((_tmpA2->tl=0,_tmpA2))))));}
# 268
end=end->tl;
# 269
++ list_len;}
# 271
lens=lens->tl;{
# 272
unsigned long seplen=Cyc_strlen(separator);
# 273
total_len +=(list_len - 1)* seplen;{
# 274
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,total_len + 1);
# 275
unsigned long i=0;
# 276
while(strs->tl != 0){
# 277
struct _dyneither_ptr _tmp27=*((struct _dyneither_ptr*)strs->hd);
# 278
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
# 279
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)_tmp27,len);
# 280
i +=len;
# 281
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),separator,seplen);
# 282
i +=seplen;
# 283
strs=strs->tl;
# 284
lens=lens->tl;}
# 286
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd),(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd);{
# 287
struct _dyneither_ptr _tmp28=ans;_npop_handler(0);return _tmp28;};};};}
# 259
;_pop_region(temp);};}
# 291
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _dyneither_ptr separator){
# 292
return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
# 296
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
# 297
int i;
# 298
{const char*_tmpA4;const char*_tmpA3;n <= _get_dyneither_size(src,sizeof(char)) && n <= _get_dyneither_size(dest,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpA3="n <= numelts(src) && n <= numelts(dest)",_tag_dyneither(_tmpA3,sizeof(char),40))),((_tmpA4="string.cyc",_tag_dyneither(_tmpA4,sizeof(char),11))),298);}
# 299
for(i=0;i < n;++ i){
# 300
char _tmp2C=((const char*)src.curr)[i];
# 301
if(_tmp2C == '\000')break;
# 302
((char*)dest.curr)[i]=_tmp2C;}
# 304
for(0;i < n;++ i){
# 305
((char*)dest.curr)[i]='\000';}
# 307
return dest;}
# 311
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
# 312
{const char*_tmpA6;const char*_tmpA5;n <= _get_dyneither_size(dest,sizeof(char)) && n <= _get_dyneither_size(src,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpA5="n <= numelts(dest) && n <= numelts(src)",_tag_dyneither(_tmpA5,sizeof(char),40))),((_tmpA6="string.cyc",_tag_dyneither(_tmpA6,sizeof(char),11))),312);}{
# 313
int i;
# 314
for(i=0;i < n;++ i){
# 315
((char*)dest.curr)[i]=((const char*)src.curr)[i];}
# 316
return dest;};}
# 319
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src){
# 320
unsigned int ssz=_get_dyneither_size(src,sizeof(char));
# 321
unsigned int dsz=_get_dyneither_size(dest,sizeof(char));
# 322
if(ssz <= dsz){
# 323
unsigned int i;
# 324
for(i=0;i < ssz;++ i){
# 325
char _tmp2F=((const char*)src.curr)[(int)i];
# 326
{char _tmpA9;char _tmpA8;struct _dyneither_ptr _tmpA7;(_tmpA7=_dyneither_ptr_plus(dest,sizeof(char),(int)i),((_tmpA8=*((char*)_check_dyneither_subscript(_tmpA7,sizeof(char),0)),((_tmpA9=_tmp2F,((_get_dyneither_size(_tmpA7,sizeof(char))== 1  && (_tmpA8 == '\000'  && _tmpA9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA7.curr)=_tmpA9)))))));}
# 327
if(_tmp2F == '\000')break;}}else{
# 331
unsigned long len=Cyc_strlen(src);
# 332
Cyc_strncpy(_dyneither_ptr_decrease_size(dest,sizeof(char),1),src,len);
# 333
if(len < _get_dyneither_size(dest,sizeof(char))){
# 334
char _tmpAC;char _tmpAB;struct _dyneither_ptr _tmpAA;(_tmpAA=_dyneither_ptr_plus(dest,sizeof(char),(int)len),((_tmpAB=*((char*)_check_dyneither_subscript(_tmpAA,sizeof(char),0)),((_tmpAC='\000',((_get_dyneither_size(_tmpAA,sizeof(char))== 1  && (_tmpAB == '\000'  && _tmpAC != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAA.curr)=_tmpAC)))))));}}
# 336
return dest;}
# 342
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dyneither_ptr src){
# 343
unsigned long len;
# 344
struct _dyneither_ptr temp;
# 346
len=Cyc_strlen(src);
# 347
temp=Cyc_Core_rnew_string(r,len + 1);
# 348
Cyc_strncpy(_dyneither_ptr_decrease_size(temp,sizeof(char),1),src,len);
# 349
return temp;}
# 352
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src){
# 353
return Cyc_rstrdup(Cyc_Core_heap_region,src);}
# 356
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
# 357
struct _dyneither_ptr temp;
# 358
unsigned long slen;
# 360
slen=Cyc_strlen(s);
# 361
sz=sz > slen?sz: slen;
# 362
temp=Cyc_Core_rnew_string(r,sz);
# 364
Cyc_strncpy(_dyneither_ptr_decrease_size(temp,sizeof(char),1),s,slen);
# 366
if(slen != _get_dyneither_size(s,sizeof(char))){
# 367
char _tmpAF;char _tmpAE;struct _dyneither_ptr _tmpAD;(_tmpAD=_dyneither_ptr_plus(temp,sizeof(char),(int)slen),((_tmpAE=*((char*)_check_dyneither_subscript(_tmpAD,sizeof(char),0)),((_tmpAF='\000',((_get_dyneither_size(_tmpAD,sizeof(char))== 1  && (_tmpAE == '\000'  && _tmpAF != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAD.curr)=_tmpAF)))))));}
# 369
return temp;}
# 372
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz){
# 373
return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}
# 376
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz){
# 378
unsigned long maxsizeP=_get_dyneither_size(str,sizeof(char));
# 380
if(maxsizeP == 0){
# 381
maxsizeP=30 > sz?30: sz;
# 382
str=Cyc_Core_rnew_string(r,maxsizeP);{
# 383
char _tmpB2;char _tmpB1;struct _dyneither_ptr _tmpB0;(_tmpB0=_dyneither_ptr_plus(str,sizeof(char),0),((_tmpB1=*((char*)_check_dyneither_subscript(_tmpB0,sizeof(char),0)),((_tmpB2='\000',((_get_dyneither_size(_tmpB0,sizeof(char))== 1  && (_tmpB1 == '\000'  && _tmpB2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB0.curr)=_tmpB2)))))));};}else{
# 385
if(sz > maxsizeP){
# 386
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;
# 387
str=Cyc_rexpand(r,(struct _dyneither_ptr)str,maxsizeP);}}
# 390
return str;}
# 393
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz){
# 394
return Cyc_rrealloc_str(Cyc_Core_heap_region,str,sz);}
# 402
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _dyneither_ptr s,int start,unsigned long amt){
# 406
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,amt + 1);
# 407
s=_dyneither_ptr_plus(s,sizeof(char),start);
# 408
{const char*_tmpB4;const char*_tmpB3;amt < _get_dyneither_size(ans,sizeof(char)) && amt <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpB3="amt < numelts(ans) && amt <= numelts(s)",_tag_dyneither(_tmpB3,sizeof(char),40))),((_tmpB4="string.cyc",_tag_dyneither(_tmpB4,sizeof(char),11))),408);}
# 409
{unsigned long i=0;for(0;i < amt;++ i){
# 410
char _tmpB7;char _tmpB6;struct _dyneither_ptr _tmpB5;(_tmpB5=_dyneither_ptr_plus(ans,sizeof(char),(int)i),((_tmpB6=*((char*)_check_dyneither_subscript(_tmpB5,sizeof(char),0)),((_tmpB7=((const char*)s.curr)[(int)i],((_get_dyneither_size(_tmpB5,sizeof(char))== 1  && (_tmpB6 == '\000'  && _tmpB7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB5.curr)=_tmpB7)))))));}}
# 411
{char _tmpBA;char _tmpB9;struct _dyneither_ptr _tmpB8;(_tmpB8=_dyneither_ptr_plus(ans,sizeof(char),(int)amt),((_tmpB9=*((char*)_check_dyneither_subscript(_tmpB8,sizeof(char),0)),((_tmpBA='\000',((_get_dyneither_size(_tmpB8,sizeof(char))== 1  && (_tmpB9 == '\000'  && _tmpBA != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB8.curr)=_tmpBA)))))));}
# 412
return ans;}
# 415
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr s,int start,unsigned long amt){
# 416
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
# 421
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
# 422
unsigned long m=_get_dyneither_size(src,sizeof(char));
# 423
unsigned long n=_get_dyneither_size(curr_suffix,sizeof(char));
# 424
const char*_tmpBB;struct _dyneither_ptr err=(_tmpBB="replace_suffix",_tag_dyneither(_tmpBB,sizeof(char),15));
# 425
if(m < n){
# 426
struct Cyc_Core_Invalid_argument_exn_struct _tmpBE;struct Cyc_Core_Invalid_argument_exn_struct*_tmpBD;(int)_throw((void*)((_tmpBD=_cycalloc(sizeof(*_tmpBD)),((_tmpBD[0]=((_tmpBE.tag=Cyc_Core_Invalid_argument,((_tmpBE.f1=err,_tmpBE)))),_tmpBD)))));}
# 427
{unsigned long i=1;for(0;i <= n;++ i){
# 428
if(*((const char*)_check_dyneither_subscript(src,sizeof(char),(int)(m - i)))!= *((const char*)_check_dyneither_subscript(curr_suffix,sizeof(char),(int)(n - i)))){
# 429
struct Cyc_Core_Invalid_argument_exn_struct _tmpC1;struct Cyc_Core_Invalid_argument_exn_struct*_tmpC0;(int)_throw((void*)((_tmpC0=_cycalloc(sizeof(*_tmpC0)),((_tmpC0[0]=((_tmpC1.tag=Cyc_Core_Invalid_argument,((_tmpC1.f1=err,_tmpC1)))),_tmpC0)))));}}}{
# 430
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dyneither_size(new_suffix,sizeof(char)))+ 1);
# 431
Cyc_strncpy(_dyneither_ptr_decrease_size(ans,sizeof(char),1),src,m - n);
# 432
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1),new_suffix,_get_dyneither_size(new_suffix,sizeof(char)));
# 433
return ans;};}
# 436
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
# 437
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
# 443
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
# 444
int len=(int)_get_dyneither_size(s,sizeof(char));
# 445
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 446
char c;
# 447
unsigned int i;
# 448
for(i=0;i < len  && (c=((const char*)s.curr)[(int)i])!= 0;++ i){
# 449
unsigned int j=0;for(0;j < asize;++ j){
# 450
if(c == ((const char*)accept.curr)[(int)j])
# 451
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 453
return _tag_dyneither(0,0,0);}
# 456
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
# 457
int len=(int)_get_dyneither_size(s,sizeof(char));
# 458
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 459
char c;
# 460
unsigned int i;
# 461
for(i=0;i < len  && (c=((char*)s.curr)[(int)i])!= 0;++ i){
# 462
unsigned int j=0;for(0;j < asize;++ j){
# 463
if(c == ((const char*)accept.curr)[(int)j])
# 464
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 466
return _tag_dyneither(0,0,0);}
# 471
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c){
# 472
int len=(int)_get_dyneither_size(s,sizeof(char));
# 473
char c2;
# 474
unsigned int i;
# 476
for(i=0;i < len  && (c2=((char*)s.curr)[(int)i])!= 0;++ i){
# 477
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 479
return _tag_dyneither(0,0,0);}
# 482
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c){
# 483
int len=(int)_get_dyneither_size(s,sizeof(char));
# 484
char c2;
# 485
unsigned int i;
# 487
for(i=0;i < len  && (c2=((const char*)s.curr)[(int)i])!= 0;++ i){
# 488
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 490
return _tag_dyneither(0,0,0);}
# 495
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s0,char c){
# 496
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
# 497
int i=len - 1;
# 498
struct _dyneither_ptr s=s0;
# 499
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 501
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
# 502
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)
# 503
return(struct _dyneither_ptr)s;}
# 505
return _tag_dyneither(0,0,0);}
# 508
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s0,char c){
# 509
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
# 510
int i=len - 1;
# 511
struct _dyneither_ptr s=s0;
# 512
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 514
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
# 515
if(*((char*)_check_dyneither_subscript(s,sizeof(char),0))== c)
# 516
return(struct _dyneither_ptr)s;}
# 518
return _tag_dyneither(0,0,0);}
# 523
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
# 524
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr)){struct Cyc_Core_Invalid_argument_exn_struct _tmpC7;const char*_tmpC6;struct Cyc_Core_Invalid_argument_exn_struct*_tmpC5;(int)_throw((void*)((_tmpC5=_cycalloc(sizeof(*_tmpC5)),((_tmpC5[0]=((_tmpC7.tag=Cyc_Core_Invalid_argument,((_tmpC7.f1=((_tmpC6="strstr",_tag_dyneither(_tmpC6,sizeof(char),7))),_tmpC7)))),_tmpC5)))));}
# 525
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0))== '\000')return haystack;{
# 527
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
# 528
{struct _dyneither_ptr start=haystack;for(0;(char*)(start=
# 529
Cyc_strchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))).curr != (char*)(_tag_dyneither(0,0,0)).curr;start=
# 530
Cyc_strchr(_dyneither_ptr_plus(start,sizeof(char),1),*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))){
# 531
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
# 532
return start;}}
# 534
return _tag_dyneither(0,0,0);};}
# 537
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
# 538
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr)){struct Cyc_Core_Invalid_argument_exn_struct _tmpCD;const char*_tmpCC;struct Cyc_Core_Invalid_argument_exn_struct*_tmpCB;(int)_throw((void*)((_tmpCB=_cycalloc(sizeof(*_tmpCB)),((_tmpCB[0]=((_tmpCD.tag=Cyc_Core_Invalid_argument,((_tmpCD.f1=((_tmpCC="mstrstr",_tag_dyneither(_tmpCC,sizeof(char),8))),_tmpCD)))),_tmpCB)))));}
# 539
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0))== '\000')return haystack;{
# 541
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
# 542
{struct _dyneither_ptr start=haystack;for(0;(char*)(start=
# 543
Cyc_mstrchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))).curr != (char*)(_tag_dyneither(0,0,0)).curr;start=
# 544
Cyc_mstrchr(_dyneither_ptr_plus(start,sizeof(char),1),*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))){
# 545
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
# 546
return start;}}
# 548
return _tag_dyneither(0,0,0);};}
# 554
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
# 555
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
# 556
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 558
{const char*_tmpCF;const char*_tmpCE;len <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpCE="len <= numelts(s)",_tag_dyneither(_tmpCE,sizeof(char),18))),((_tmpCF="string.cyc",_tag_dyneither(_tmpCF,sizeof(char),11))),558);}
# 559
{unsigned long i=0;for(0;i < len;++ i){
# 560
int j;
# 561
for(j=0;j < asize;++ j){
# 562
if(((const char*)s.curr)[(int)i]== ((const char*)accept.curr)[j])
# 563
break;}
# 564
if(j == asize)
# 565
return i;}}
# 568
return len;}
# 574
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
# 575
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
# 576
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 578
{const char*_tmpD1;const char*_tmpD0;len <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpD0="len <= numelts(s)",_tag_dyneither(_tmpD0,sizeof(char),18))),((_tmpD1="string.cyc",_tag_dyneither(_tmpD1,sizeof(char),11))),578);}
# 579
{unsigned long i=0;for(0;i < len;++ i){
# 580
int j;
# 581
for(j=0;j < asize;++ j){
# 582
if(((const char*)s.curr)[(int)i]!= ((const char*)accept.curr)[j])
# 583
break;}
# 584
if(j == asize)
# 585
return i;}}
# 588
return len;}
# 595
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim){
# 597
static struct _dyneither_ptr olds={(void*)0,(void*)0,(void*)(0 + 0)};
# 598
struct _dyneither_ptr token;
# 600
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr){
# 601
if((char*)olds.curr == (char*)(_tag_dyneither(0,0,0)).curr)
# 602
return _tag_dyneither(0,0,0);
# 603
s=olds;}{
# 607
unsigned long inc=Cyc_strspn((struct _dyneither_ptr)s,delim);
# 608
if(inc >= _get_dyneither_size(s,sizeof(char)) || *((char*)_check_dyneither_subscript(_dyneither_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0))== '\000'){
# 610
olds=_tag_dyneither(0,0,0);
# 611
return _tag_dyneither(0,0,0);}else{
# 614
_dyneither_ptr_inplace_plus(& s,sizeof(char),(int)inc);}
# 617
token=s;
# 618
s=Cyc_mstrpbrk(token,(struct _dyneither_ptr)delim);
# 619
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)
# 621
olds=_tag_dyneither(0,0,0);else{
# 625
{char _tmpD4;char _tmpD3;struct _dyneither_ptr _tmpD2;(_tmpD2=s,((_tmpD3=*((char*)_check_dyneither_subscript(_tmpD2,sizeof(char),0)),((_tmpD4='\000',((_get_dyneither_size(_tmpD2,sizeof(char))== 1  && (_tmpD3 == '\000'  && _tmpD4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD2.curr)=_tmpD4)))))));}
# 626
olds=_dyneither_ptr_plus(s,sizeof(char),1);}
# 628
return token;};}
# 632
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dyneither_ptr s){
# 633
struct Cyc_List_List*result=0;
# 634
{int i=(int)(Cyc_strlen(s)- 1);for(0;i >= 0;-- i){
# 635
struct Cyc_List_List*_tmpD5;result=((_tmpD5=_region_malloc(r,sizeof(*_tmpD5)),((_tmpD5->hd=(void*)((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i))),((_tmpD5->tl=result,_tmpD5))))));}}
# 636
return result;}
# 639
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s){
# 640
return Cyc_rexplode(Cyc_Core_heap_region,s);}
# 643
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*chars){
# 644
struct _dyneither_ptr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(chars)+ 1));
# 645
unsigned long i=0;
# 646
while(chars != 0){
# 647
{char _tmpD8;char _tmpD7;struct _dyneither_ptr _tmpD6;(_tmpD6=_dyneither_ptr_plus(s,sizeof(char),(int)i ++),((_tmpD7=*((char*)_check_dyneither_subscript(_tmpD6,sizeof(char),0)),((_tmpD8=(char)((int)chars->hd),((_get_dyneither_size(_tmpD6,sizeof(char))== 1  && (_tmpD7 == '\000'  && _tmpD8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD6.curr)=_tmpD8)))))));}
# 648
chars=chars->tl;}
# 650
return s;}
# 654
inline static int Cyc_casecmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2){
# 657
unsigned long min_length=len1 < len2?len1: len2;
# 659
{const char*_tmpDA;const char*_tmpD9;min_length <= _get_dyneither_size(s1,sizeof(char)) && min_length <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpD9="min_length <= numelts(s1) && min_length <= numelts(s2)",_tag_dyneither(_tmpD9,sizeof(char),55))),((_tmpDA="string.cyc",_tag_dyneither(_tmpDA,sizeof(char),11))),659);}{
# 661
int i=- 1;
# 662
while((++ i,i < min_length)){
# 663
int diff=toupper((int)((const char*)s1.curr)[i])- toupper((int)((const char*)s2.curr)[i]);
# 664
if(diff != 0)
# 665
return diff;}
# 667
return(int)len1 - (int)len2;};}
# 670
int Cyc_strcasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
# 671
if((char*)s1.curr == (char*)s2.curr)
# 672
return 0;{
# 673
unsigned long len1=Cyc_strlen(s1);
# 674
unsigned long len2=Cyc_strlen(s2);
# 675
return Cyc_casecmp(s1,len1,s2,len2);};}
# 678
inline static int Cyc_caseless_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 681
if(n <= 0)return 0;{
# 683
unsigned long min_len=len1 > len2?len2: len1;
# 684
unsigned long bound=min_len > n?n: min_len;
# 686
{const char*_tmpDC;const char*_tmpDB;bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpDB="bound <= numelts(s1) && bound <= numelts(s2)",_tag_dyneither(_tmpDB,sizeof(char),45))),((_tmpDC="string.cyc",_tag_dyneither(_tmpDC,sizeof(char),11))),686);}
# 688
{int i=0;for(0;i < bound;++ i){
# 689
int retc;
# 690
if((retc=toupper((int)((const char*)s1.curr)[i])- toupper((int)((const char*)s2.curr)[i]))!= 0)
# 691
return retc;}}
# 693
if(len1 < n  || len2 < n)
# 694
return(int)len1 - (int)len2;
# 695
return 0;};}
# 699
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
# 700
unsigned long len1=Cyc_strlen(s1);
# 701
unsigned long len2=Cyc_strlen(s2);
# 702
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}
# 707
void*memcpy(void*,const void*,unsigned long n);
# 708
void*memmove(void*,const void*,unsigned long n);
# 709
int memcmp(const void*,const void*,unsigned long n);
# 710
char*memchr(const char*,char c,unsigned long n);
# 711
void*memset(void*,int c,unsigned long n);
# 712
void bcopy(const void*src,void*dest,unsigned long n);
# 713
void bzero(void*s,unsigned long n);
# 714
char*GC_realloc(char*,unsigned long n);
# 718
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr s,unsigned long n){
# 719
char*_tmp5E=GC_realloc((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),n);
# 720
return wrap_Cbuffer_as_buffer(_tmp5E,n);}
# 723
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
# 724
if((((void*)d.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || (void*)s.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n){
# 725
struct Cyc_Core_Invalid_argument_exn_struct _tmpE2;const char*_tmpE1;struct Cyc_Core_Invalid_argument_exn_struct*_tmpE0;(int)_throw((void*)((_tmpE0=_cycalloc(sizeof(*_tmpE0)),((_tmpE0[0]=((_tmpE2.tag=Cyc_Core_Invalid_argument,((_tmpE2.f1=((_tmpE1="memcpy",_tag_dyneither(_tmpE1,sizeof(char),7))),_tmpE2)))),_tmpE0)))));}
# 726
memcpy((void*)_check_null(_untag_dyneither_ptr(d,sizeof(void),1)),(const void*)_check_null(_untag_dyneither_ptr(s,sizeof(void),1)),n * sz);
# 727
return d;}
# 730
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
# 731
if((((void*)d.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || (void*)s.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n){
# 732
struct Cyc_Core_Invalid_argument_exn_struct _tmpE8;const char*_tmpE7;struct Cyc_Core_Invalid_argument_exn_struct*_tmpE6;(int)_throw((void*)((_tmpE6=_cycalloc(sizeof(*_tmpE6)),((_tmpE6[0]=((_tmpE8.tag=Cyc_Core_Invalid_argument,((_tmpE8.f1=((_tmpE7="memove",_tag_dyneither(_tmpE7,sizeof(char),7))),_tmpE8)))),_tmpE6)))));}
# 733
memmove((void*)_check_null(_untag_dyneither_ptr(d,sizeof(void),1)),(const void*)_check_null(_untag_dyneither_ptr(s,sizeof(void),1)),n * sz);
# 734
return d;}
# 737
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
# 738
if((((char*)s1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)s2.curr == (char*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s1,sizeof(char))>= n) || _get_dyneither_size(s2,sizeof(char))>= n){
# 739
struct Cyc_Core_Invalid_argument_exn_struct _tmpEE;const char*_tmpED;struct Cyc_Core_Invalid_argument_exn_struct*_tmpEC;(int)_throw((void*)((_tmpEC=_cycalloc(sizeof(*_tmpEC)),((_tmpEC[0]=((_tmpEE.tag=Cyc_Core_Invalid_argument,((_tmpEE.f1=((_tmpED="memcmp",_tag_dyneither(_tmpED,sizeof(char),7))),_tmpEE)))),_tmpEC)))));}
# 740
return memcmp((const void*)_check_null(_untag_dyneither_ptr(s1,sizeof(char),1)),(const void*)_check_null(_untag_dyneither_ptr(s2,sizeof(char),1)),n);}
# 743
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n){
# 744
unsigned int sz=_get_dyneither_size(s,sizeof(char));
# 745
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > sz){
# 746
struct Cyc_Core_Invalid_argument_exn_struct _tmpF4;const char*_tmpF3;struct Cyc_Core_Invalid_argument_exn_struct*_tmpF2;(int)_throw((void*)((_tmpF2=_cycalloc(sizeof(*_tmpF2)),((_tmpF2[0]=((_tmpF4.tag=Cyc_Core_Invalid_argument,((_tmpF4.f1=((_tmpF3="memchr",_tag_dyneither(_tmpF3,sizeof(char),7))),_tmpF4)))),_tmpF2)))));}{
# 747
char*_tmp6B=memchr((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),c,n);
# 748
if(_tmp6B == 0)return _tag_dyneither(0,0,0);{
# 749
unsigned int _tmp6C=(unsigned int)((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)));
# 750
unsigned int _tmp6D=(unsigned int)((const char*)_tmp6B);
# 751
unsigned int _tmp6E=_tmp6D - _tmp6C;
# 752
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp6E);};};}
# 755
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n){
# 756
unsigned int sz=_get_dyneither_size(s,sizeof(char));
# 757
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > sz){
# 758
struct Cyc_Core_Invalid_argument_exn_struct _tmpFA;const char*_tmpF9;struct Cyc_Core_Invalid_argument_exn_struct*_tmpF8;(int)_throw((void*)((_tmpF8=_cycalloc(sizeof(*_tmpF8)),((_tmpF8[0]=((_tmpFA.tag=Cyc_Core_Invalid_argument,((_tmpFA.f1=((_tmpF9="mmemchr",_tag_dyneither(_tmpF9,sizeof(char),8))),_tmpFA)))),_tmpF8)))));}{
# 759
char*_tmp72=memchr((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),c,n);
# 760
if(_tmp72 == 0)return _tag_dyneither(0,0,0);{
# 761
unsigned int _tmp73=(unsigned int)((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)));
# 762
unsigned int _tmp74=(unsigned int)_tmp72;
# 763
unsigned int _tmp75=_tmp74 - _tmp73;
# 764
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp75);};};}
# 767
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n){
# 768
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > _get_dyneither_size(s,sizeof(char))){
# 769
struct Cyc_Core_Invalid_argument_exn_struct _tmp100;const char*_tmpFF;struct Cyc_Core_Invalid_argument_exn_struct*_tmpFE;(int)_throw((void*)((_tmpFE=_cycalloc(sizeof(*_tmpFE)),((_tmpFE[0]=((_tmp100.tag=Cyc_Core_Invalid_argument,((_tmp100.f1=((_tmpFF="memset",_tag_dyneither(_tmpFF,sizeof(char),7))),_tmp100)))),_tmpFE)))));}
# 770
memset((void*)((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1))),(int)c,n);
# 771
return s;}
# 774
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n){
# 775
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(s,sizeof(char))< n){
# 776
struct Cyc_Core_Invalid_argument_exn_struct _tmp106;const char*_tmp105;struct Cyc_Core_Invalid_argument_exn_struct*_tmp104;(int)_throw((void*)((_tmp104=_cycalloc(sizeof(*_tmp104)),((_tmp104[0]=((_tmp106.tag=Cyc_Core_Invalid_argument,((_tmp106.f1=((_tmp105="bzero",_tag_dyneither(_tmp105,sizeof(char),6))),_tmp106)))),_tmp104)))));}
# 777
((void(*)(char*s,unsigned long n))bzero)((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),n);}
# 780
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz){
# 781
if((((void*)src.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(src,sizeof(void))< n) || (void*)dst.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(dst,sizeof(void))< n){
# 782
struct Cyc_Core_Invalid_argument_exn_struct _tmp10C;const char*_tmp10B;struct Cyc_Core_Invalid_argument_exn_struct*_tmp10A;(int)_throw((void*)((_tmp10A=_cycalloc(sizeof(*_tmp10A)),((_tmp10A[0]=((_tmp10C.tag=Cyc_Core_Invalid_argument,((_tmp10C.f1=((_tmp10B="bcopy",_tag_dyneither(_tmp10B,sizeof(char),6))),_tmp10C)))),_tmp10A)))));}
# 783
bcopy((const void*)_check_null(_untag_dyneither_ptr(src,sizeof(void),1)),(void*)_check_null(_untag_dyneither_ptr(dst,sizeof(void),1)),n * sz);}
