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
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 228 "core.h"
void Cyc_Core_rethrow(void*);
# 256
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 750 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 922
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 925
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 965
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 987
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1069
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
# 119 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 123
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 128
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 196
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 224
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_is_function_type(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 242
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 314
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
int Cyc_Tcutil_supports_default(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 78
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 47 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 50
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 56
(*_tmp1).f3=1;
return 1;}}
# 60
return 0;}
# 63
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
{const char*_tmp28B;void*_tmp28A[1];struct Cyc_String_pa_PrintArg_struct _tmp289;(_tmp289.tag=0,((_tmp289.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att)),((_tmp28A[0]=& _tmp289,Cyc_Tcutil_terr(loc,((_tmp28B="%s has an out-of-range index",_tag_dyneither(_tmp28B,sizeof(char),29))),_tag_dyneither(_tmp28A,sizeof(void*),1)))))));}
return 0;}
# 68
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 70
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 73
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i)){
const char*_tmp291;void*_tmp290[3];struct Cyc_String_pa_PrintArg_struct _tmp28F;struct Cyc_String_pa_PrintArg_struct _tmp28E;struct Cyc_Int_pa_PrintArg_struct _tmp28D;(_tmp28D.tag=1,((_tmp28D.f1=(unsigned long)i,((_tmp28E.tag=0,((_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2),((_tmp28F.tag=0,((_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2),((_tmp290[0]=& _tmp28F,((_tmp290[1]=& _tmp28E,((_tmp290[2]=& _tmp28D,Cyc_Tcutil_terr(loc,((_tmp291="incompatible %s() and %s() attributes on parameter %d",_tag_dyneither(_tmp291,sizeof(char),54))),_tag_dyneither(_tmp290,sizeof(void*),3)))))))))))))))))));}}
# 77
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{const char*_tmp294;struct _tuple13 _tmp293;struct _tuple13 init_params=(_tmp293.f1=0,((_tmp293.f2=((_tmp294="initializes",_tag_dyneither(_tmp294,sizeof(char),12))),_tmp293)));
const char*_tmp297;struct _tuple13 _tmp296;struct _tuple13 nolive_unique_params=(_tmp296.f1=0,((_tmp296.f2=((_tmp297="noliveunique",_tag_dyneither(_tmp297,sizeof(char),13))),_tmp296)));
const char*_tmp29A;struct _tuple13 _tmp299;struct _tuple13 noconsume_params=(_tmp299.f1=0,((_tmp299.f2=((_tmp29A="noconsume",_tag_dyneither(_tmp29A,sizeof(char),10))),_tmp299)));
void*_stmttmp0=Cyc_Tcutil_compress(t);void*_tmpB=_stmttmp0;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB;if(_tmpC->tag != 9)goto _LL3;else{_tmpD=(_tmpC->f1).attributes;_tmpE=(_tmpC->f1).args;}}_LL2: {
# 84
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE);
for(0;_tmpD != 0;_tmpD=_tmpD->tl){
void*_stmttmp1=(void*)_tmpD->hd;void*_tmpF=_stmttmp1;int _tmp11;int _tmp13;int _tmp15;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp10=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpF;if(_tmp10->tag != 20)goto _LL8;else{_tmp11=_tmp10->f1;}}_LL7:
# 88
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp11,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp11,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp11,init_params,noconsume_params);{
struct _tuple8*_stmttmp4=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp11 - 1);void*_tmp17;struct _tuple8*_tmp16=_stmttmp4;_tmp17=_tmp16->f3;{
const char*_tmp29B;struct _dyneither_ptr s=(_tmp29B="initializes attribute allowed only on",_tag_dyneither(_tmp29B,sizeof(char),38));
{void*_stmttmp5=Cyc_Tcutil_compress(_tmp17);void*_tmp18=_stmttmp5;void*_tmp1A;union Cyc_Absyn_Constraint*_tmp1B;union Cyc_Absyn_Constraint*_tmp1C;union Cyc_Absyn_Constraint*_tmp1D;_LLF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18;if(_tmp19->tag != 5)goto _LL11;else{_tmp1A=(_tmp19->f1).elt_typ;_tmp1B=((_tmp19->f1).ptr_atts).nullable;_tmp1C=((_tmp19->f1).ptr_atts).bounds;_tmp1D=((_tmp19->f1).ptr_atts).zero_term;}}_LL10:
# 95
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1B)){
const char*_tmp29F;void*_tmp29E[1];struct Cyc_String_pa_PrintArg_struct _tmp29D;(_tmp29D.tag=0,((_tmp29D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp29E[0]=& _tmp29D,Cyc_Tcutil_terr(loc,((_tmp29F="%s non-null pointers",_tag_dyneither(_tmp29F,sizeof(char),21))),_tag_dyneither(_tmp29E,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_bound_one(_tmp1C)){
const char*_tmp2A3;void*_tmp2A2[1];struct Cyc_String_pa_PrintArg_struct _tmp2A1;(_tmp2A1.tag=0,((_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2A2[0]=& _tmp2A1,Cyc_Tcutil_terr(loc,((_tmp2A3="%s pointers of size 1",_tag_dyneither(_tmp2A3,sizeof(char),22))),_tag_dyneither(_tmp2A2,sizeof(void*),1)))))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1D)){
const char*_tmp2A7;void*_tmp2A6[1];struct Cyc_String_pa_PrintArg_struct _tmp2A5;(_tmp2A5.tag=0,((_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2A6[0]=& _tmp2A5,Cyc_Tcutil_terr(loc,((_tmp2A7="%s pointers to non-zero-terminated arrays",_tag_dyneither(_tmp2A7,sizeof(char),42))),_tag_dyneither(_tmp2A6,sizeof(void*),1)))))));}
goto _LLE;_LL11:;_LL12: {
# 103
const char*_tmp2AB;void*_tmp2AA[1];struct Cyc_String_pa_PrintArg_struct _tmp2A9;(_tmp2A9.tag=0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2AA[0]=& _tmp2A9,Cyc_Tcutil_terr(loc,((_tmp2AB="%s pointers",_tag_dyneither(_tmp2AB,sizeof(char),12))),_tag_dyneither(_tmp2AA,sizeof(void*),1)))))));}_LLE:;}
# 105
{struct Cyc_List_List*_tmp2AC;init_params.f1=((_tmp2AC=_region_malloc(temp,sizeof(*_tmp2AC)),((_tmp2AC->hd=(void*)_tmp11,((_tmp2AC->tl=init_params.f1,_tmp2AC))))));}
goto _LL5;};};_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp12=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpF;if(_tmp12->tag != 21)goto _LLA;else{_tmp13=_tmp12->f1;}}_LL9:
# 108
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp13,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,nolive_unique_params,noconsume_params);{
struct _tuple8*_stmttmp3=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp13 - 1);void*_tmp2D;struct _tuple8*_tmp2C=_stmttmp3;_tmp2D=_tmp2C->f3;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2D,0)){
const char*_tmp2AF;void*_tmp2AE;(_tmp2AE=0,Cyc_Tcutil_terr(loc,((_tmp2AF="noliveunique attribute allowed only on unique pointers",_tag_dyneither(_tmp2AF,sizeof(char),55))),_tag_dyneither(_tmp2AE,sizeof(void*),0)));}
{struct Cyc_List_List*_tmp2B0;nolive_unique_params.f1=((_tmp2B0=_region_malloc(temp,sizeof(*_tmp2B0)),((_tmp2B0->hd=(void*)_tmp13,((_tmp2B0->tl=nolive_unique_params.f1,_tmp2B0))))));}
goto _LL5;};_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp14=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpF;if(_tmp14->tag != 22)goto _LLC;else{_tmp15=_tmp14->f1;}}_LLB:
# 117
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp15,nargs,(void*)_tmpD->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,noconsume_params,nolive_unique_params);{
struct _tuple8*_stmttmp2=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpE,_tmp15 - 1);void*_tmp32;struct _tuple8*_tmp31=_stmttmp2;_tmp32=_tmp31->f3;
# 122
{struct Cyc_List_List*_tmp2B1;noconsume_params.f1=((_tmp2B1=_region_malloc(temp,sizeof(*_tmp2B1)),((_tmp2B1->hd=(void*)_tmp15,((_tmp2B1->tl=noconsume_params.f1,_tmp2B1))))));}
goto _LL5;};_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 127
goto _LL0;}_LL3:;_LL4: {
const char*_tmp2B4;void*_tmp2B3;(_tmp2B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2B4="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2B4,sizeof(char),34))),_tag_dyneither(_tmp2B3,sizeof(void*),0)));}_LL0:;}
# 79
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 133
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 137
enum Cyc_Absyn_Scope _tmp3D;union Cyc_Absyn_Nmspace _tmp3E;struct _dyneither_ptr*_tmp3F;void*_tmp40;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_List_List*_tmp42;struct Cyc_Absyn_Vardecl*_tmp3C=vd;_tmp3D=_tmp3C->sc;_tmp3E=(_tmp3C->name)->f1;_tmp3F=(_tmp3C->name)->f2;_tmp40=_tmp3C->type;_tmp41=_tmp3C->initializer;_tmp42=_tmp3C->attributes;
# 143
{union Cyc_Absyn_Nmspace _tmp43=_tmp3E;_LL14: if((_tmp43.Rel_n).tag != 1)goto _LL16;_LL15:
# 146
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 152
{void*_stmttmp6=Cyc_Tcutil_compress(_tmp40);void*_tmp44=_stmttmp6;void*_tmp46;struct Cyc_Absyn_Tqual _tmp47;union Cyc_Absyn_Constraint*_tmp48;unsigned int _tmp49;_LL19:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp45=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44;if(_tmp45->tag != 8)goto _LL1B;else{_tmp46=(_tmp45->f1).elt_type;_tmp47=(_tmp45->f1).tq;if((_tmp45->f1).num_elts != 0)goto _LL1B;_tmp48=(_tmp45->f1).zero_term;_tmp49=(_tmp45->f1).zt_loc;}}if(!(_tmp41 != 0))goto _LL1B;_LL1A:
# 154
{void*_stmttmp7=_tmp41->r;void*_tmp4A=_stmttmp7;struct _dyneither_ptr _tmp4C;struct _dyneither_ptr _tmp4E;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_List_List*_tmp54;struct Cyc_List_List*_tmp56;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 0)goto _LL20;else{if(((_tmp4B->f1).String_c).tag != 8)goto _LL20;_tmp4C=(struct _dyneither_ptr)((_tmp4B->f1).String_c).val;}}_LL1F:
# 156
 _tmp40=(vd->type=Cyc_Absyn_array_typ(_tmp46,_tmp47,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp4C,sizeof(char)),0),_tmp48,_tmp49));
goto _LL1D;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4D->tag != 0)goto _LL22;else{if(((_tmp4D->f1).Wstring_c).tag != 9)goto _LL22;_tmp4E=(struct _dyneither_ptr)((_tmp4D->f1).Wstring_c).val;}}_LL21:
# 159
 _tmp40=(vd->type=Cyc_Absyn_array_typ(_tmp46,_tmp47,Cyc_Absyn_uint_exp(1,0),_tmp48,_tmp49));
goto _LL1D;_LL22: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4F->tag != 26)goto _LL24;else{_tmp50=_tmp4F->f2;}}_LL23:
 _tmp52=_tmp50;goto _LL25;_LL24: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp51=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp51->tag != 27)goto _LL26;else{_tmp52=_tmp51->f1;}}_LL25:
# 164
 _tmp40=(vd->type=Cyc_Absyn_array_typ(_tmp46,_tmp47,_tmp52,_tmp48,_tmp49));
goto _LL1D;_LL26: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp53=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp53->tag != 35)goto _LL28;else{_tmp54=_tmp53->f2;}}_LL27:
 _tmp56=_tmp54;goto _LL29;_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp55=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp55->tag != 25)goto _LL2A;else{_tmp56=_tmp55->f1;}}_LL29:
# 168
 _tmp40=(vd->type=Cyc_Absyn_array_typ(_tmp46,_tmp47,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56),0),_tmp48,_tmp49));
goto _LL1D;_LL2A:;_LL2B:
 goto _LL1D;_LL1D:;}
# 172
goto _LL18;_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 176
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp40);
# 178
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp40);
# 181
{void*_stmttmp8=Cyc_Tcutil_compress(_tmp40);void*_tmp57=_stmttmp8;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57;if(_tmp58->tag != 8)goto _LL2F;}_LL2E:
 vd->escapes=0;goto _LL2C;_LL2F:;_LL30:
 vd->escapes=1;goto _LL2C;_LL2C:;}
# 186
if(Cyc_Tcutil_is_function_type(_tmp40)){
_tmp42=Cyc_Tcutil_transfer_fn_type_atts(_tmp40,_tmp42);
Cyc_Tc_fnTypeAttsOK(te,_tmp40,loc);}
# 191
if(_tmp3D == Cyc_Absyn_Extern  || _tmp3D == Cyc_Absyn_ExternC){
if(_tmp41 != 0){
const char*_tmp2B7;void*_tmp2B6;(_tmp2B6=0,Cyc_Tcutil_terr(loc,((_tmp2B7="extern declaration should not have initializer",_tag_dyneither(_tmp2B7,sizeof(char),47))),_tag_dyneither(_tmp2B6,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp40)){
# 198
for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
void*_stmttmp9=(void*)_tmp42->hd;void*_tmp5B=_stmttmp9;_LL32: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp5C=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp5C->tag != 6)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp5D=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp5D->tag != 8)goto _LL36;}_LL35:
# 203
 goto _LL37;_LL36: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp5E=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp5E->tag != 9)goto _LL38;}_LL37:
 goto _LL39;_LL38: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp5F=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp5F->tag != 10)goto _LL3A;}_LL39:
 goto _LL3B;_LL3A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp60=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp60->tag != 11)goto _LL3C;}_LL3B:
 goto _LL3D;_LL3C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp61=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp61->tag != 12)goto _LL3E;}_LL3D:
 goto _LL3F;_LL3E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp62=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp62->tag != 13)goto _LL40;}_LL3F:
 goto _LL41;_LL40: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp63=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp5B;if(_tmp63->tag != 14)goto _LL42;}_LL41:
 continue;_LL42:;_LL43:
# 211
{const char*_tmp2BC;void*_tmp2BB[2];struct Cyc_String_pa_PrintArg_struct _tmp2BA;struct Cyc_String_pa_PrintArg_struct _tmp2B9;(_tmp2B9.tag=0,((_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2BA.tag=0,((_tmp2BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp42->hd)),((_tmp2BB[0]=& _tmp2BA,((_tmp2BB[1]=& _tmp2B9,Cyc_Tcutil_terr(loc,((_tmp2BC="bad attribute %s for variable %s",_tag_dyneither(_tmp2BC,sizeof(char),33))),_tag_dyneither(_tmp2BB,sizeof(void*),2)))))))))))));}
goto _LL31;_LL31:;}
# 216
if(_tmp41 == 0  || in_cinclude){
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp40)){
const char*_tmp2C1;void*_tmp2C0[2];struct Cyc_String_pa_PrintArg_struct _tmp2BF;struct Cyc_String_pa_PrintArg_struct _tmp2BE;(_tmp2BE.tag=0,((_tmp2BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp40)),((_tmp2BF.tag=0,((_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2C0[0]=& _tmp2BF,((_tmp2C0[1]=& _tmp2BE,Cyc_Tcutil_terr(loc,((_tmp2C1="initializer required for variable %s of type %s",_tag_dyneither(_tmp2C1,sizeof(char),48))),_tag_dyneither(_tmp2C0,sizeof(void*),2)))))))))))));}}else{
# 221
struct Cyc_Absyn_Exp*_tmp6C=_tmp41;
# 225
struct _handler_cons _tmp6D;_push_handler(& _tmp6D);{int _tmp6F=0;if(setjmp(_tmp6D.handler))_tmp6F=1;if(!_tmp6F){
{void*_tmp70=Cyc_Tcexp_tcExpInitializer(te,& _tmp40,_tmp6C);
if(!Cyc_Tcutil_coerce_assign(te,_tmp6C,_tmp40)){
struct _dyneither_ptr _tmp71=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp72=" declared with type ";
struct _dyneither_ptr _tmp73=Cyc_Absynpp_typ2string(_tmp40);
const char*_tmp74=" but initialized with type ";
struct _dyneither_ptr _tmp75=Cyc_Absynpp_typ2string(_tmp70);
{const char*_tmp2C3;const char*_tmp2C2;if((((Cyc_strlen((struct _dyneither_ptr)_tmp71)+ Cyc_strlen(((_tmp2C2=_tmp72,_tag_dyneither(_tmp2C2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C2,21))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp73))+ Cyc_strlen(((_tmp2C3=_tmp74,_tag_dyneither(_tmp2C3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C3,28))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp75)> 70){
const char*_tmp2CF;void*_tmp2CE[5];struct Cyc_String_pa_PrintArg_struct _tmp2CD;const char*_tmp2CC;struct Cyc_String_pa_PrintArg_struct _tmp2CB;struct Cyc_String_pa_PrintArg_struct _tmp2CA;const char*_tmp2C9;struct Cyc_String_pa_PrintArg_struct _tmp2C8;struct Cyc_String_pa_PrintArg_struct _tmp2C7;(_tmp2C7.tag=0,((_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp75),((_tmp2C8.tag=0,((_tmp2C8.f1=(struct _dyneither_ptr)((_tmp2C9=_tmp74,_tag_dyneither(_tmp2C9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C9,28)))),((_tmp2CA.tag=0,((_tmp2CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp73),((_tmp2CB.tag=0,((_tmp2CB.f1=(struct _dyneither_ptr)((_tmp2CC=_tmp72,_tag_dyneither(_tmp2CC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2CC,21)))),((_tmp2CD.tag=0,((_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp71),((_tmp2CE[0]=& _tmp2CD,((_tmp2CE[1]=& _tmp2CB,((_tmp2CE[2]=& _tmp2CA,((_tmp2CE[3]=& _tmp2C8,((_tmp2CE[4]=& _tmp2C7,Cyc_Tcutil_terr(loc,((_tmp2CF="%s%s\n\t%s\n%s\n\t%s",_tag_dyneither(_tmp2CF,sizeof(char),16))),_tag_dyneither(_tmp2CE,sizeof(void*),5)))))))))))))))))))))))))))))));}else{
# 236
const char*_tmp2DB;void*_tmp2DA[5];struct Cyc_String_pa_PrintArg_struct _tmp2D9;const char*_tmp2D8;struct Cyc_String_pa_PrintArg_struct _tmp2D7;struct Cyc_String_pa_PrintArg_struct _tmp2D6;const char*_tmp2D5;struct Cyc_String_pa_PrintArg_struct _tmp2D4;struct Cyc_String_pa_PrintArg_struct _tmp2D3;(_tmp2D3.tag=0,((_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp75),((_tmp2D4.tag=0,((_tmp2D4.f1=(struct _dyneither_ptr)((_tmp2D5=_tmp74,_tag_dyneither(_tmp2D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D5,28)))),((_tmp2D6.tag=0,((_tmp2D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp73),((_tmp2D7.tag=0,((_tmp2D7.f1=(struct _dyneither_ptr)((_tmp2D8=_tmp72,_tag_dyneither(_tmp2D8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D8,21)))),((_tmp2D9.tag=0,((_tmp2D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp71),((_tmp2DA[0]=& _tmp2D9,((_tmp2DA[1]=& _tmp2D7,((_tmp2DA[2]=& _tmp2D6,((_tmp2DA[3]=& _tmp2D4,((_tmp2DA[4]=& _tmp2D3,Cyc_Tcutil_terr(loc,((_tmp2DB="%s%s%s%s%s",_tag_dyneither(_tmp2DB,sizeof(char),11))),_tag_dyneither(_tmp2DA,sizeof(void*),5)))))))))))))))))))))))))))))));}}
Cyc_Tcutil_explain_failure();
# 239
if(!Cyc_Tcutil_is_const_exp(_tmp6C)){
const char*_tmp2DE;void*_tmp2DD;(_tmp2DD=0,Cyc_Tcutil_terr(loc,((_tmp2DE="initializer is not a constant expression",_tag_dyneither(_tmp2DE,sizeof(char),41))),_tag_dyneither(_tmp2DD,sizeof(void*),0)));}}}
# 226
;_pop_handler();}else{void*_tmp6E=(void*)_exn_thrown;void*_tmp8D=_tmp6E;void*_tmp8F;_LL45: {struct Cyc_Tcenv_Env_error_exn_struct*_tmp8E=(struct Cyc_Tcenv_Env_error_exn_struct*)_tmp8D;if(_tmp8E->tag != Cyc_Tcenv_Env_error)goto _LL47;}_LL46:
# 244
{const char*_tmp2E1;void*_tmp2E0;(_tmp2E0=0,Cyc_Tcutil_terr(loc,((_tmp2E1="initializer is not a constant expression",_tag_dyneither(_tmp2E1,sizeof(char),41))),_tag_dyneither(_tmp2E0,sizeof(void*),0)));}
goto _LL44;_LL47: _tmp8F=_tmp8D;_LL48:(void)_throw(_tmp8F);_LL44:;}};}}else{
# 250
for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
void*_stmttmpA=(void*)_tmp42->hd;void*_tmp92=_stmttmpA;_LL4A: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp93=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp92;if(_tmp93->tag != 0)goto _LL4C;}_LL4B:
# 253
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp94=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp92;if(_tmp94->tag != 1)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp95=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp92;if(_tmp95->tag != 2)goto _LL50;}_LL4F:
 goto _LL51;_LL50: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp96=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp92;if(_tmp96->tag != 3)goto _LL52;}_LL51:
 goto _LL53;_LL52: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp97=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp92;if(_tmp97->tag != 4)goto _LL54;}_LL53:
 goto _LL55;_LL54: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp98=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp92;if(_tmp98->tag != 19)goto _LL56;}_LL55:
 goto _LL57;_LL56: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp99=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp92;if(_tmp99->tag != 20)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp9A=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp92;if(_tmp9A->tag != 23)goto _LL5A;}_LL59:
 goto _LL5B;_LL5A: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp9B=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp92;if(_tmp9B->tag != 5)goto _LL5C;}_LL5B: {
const char*_tmp2E4;void*_tmp2E3;(_tmp2E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2E4="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2E4,sizeof(char),45))),_tag_dyneither(_tmp2E3,sizeof(void*),0)));}_LL5C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9C=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp92;if(_tmp9C->tag != 6)goto _LL5E;}_LL5D:
# 263
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9D=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp92;if(_tmp9D->tag != 7)goto _LL60;}_LL5F:
# 265
{const char*_tmp2E8;void*_tmp2E7[1];struct Cyc_String_pa_PrintArg_struct _tmp2E6;(_tmp2E6.tag=0,((_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp42->hd)),((_tmp2E7[0]=& _tmp2E6,Cyc_Tcutil_terr(loc,((_tmp2E8="bad attribute %s in function declaration",_tag_dyneither(_tmp2E8,sizeof(char),41))),_tag_dyneither(_tmp2E7,sizeof(void*),1)))))));}
goto _LL49;_LL60:;_LL61:
 continue;_LL49:;}}}
# 273
{struct _handler_cons _tmpA3;_push_handler(& _tmpA3);{int _tmpA5=0;if(setjmp(_tmpA3.handler))_tmpA5=1;if(!_tmpA5){
{struct _tuple14*_tmpA6=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp3F);
void*_stmttmpB=(*_tmpA6).f1;void*_tmpA7=_stmttmpB;void*_tmpA9;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpA8->tag != 0)goto _LL65;else{_tmpA9=(void*)_tmpA8->f1;}}_LL64: {
# 277
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2EB;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2EA;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpAE=(_tmp2EA=_cycalloc(sizeof(*_tmp2EA)),((_tmp2EA[0]=((_tmp2EB.tag=1,((_tmp2EB.f1=vd,_tmp2EB)))),_tmp2EA)));
void*_tmpAF=Cyc_Tcdecl_merge_binding(_tmpA9,(void*)_tmpAE,loc,Cyc_Tc_tc_msg);
if(_tmpAF == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 281
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmpAF != _tmpA9  || (*_tmpA6).f2){
# 284
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2F1;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2F0;struct _tuple14*_tmp2EF;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3F,(
(_tmp2EF=_cycalloc(sizeof(*_tmp2EF)),((_tmp2EF->f1=(void*)((_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1[0]=((_tmp2F0.tag=0,((_tmp2F0.f1=_tmpAF,_tmp2F0)))),_tmp2F1)))),((_tmp2EF->f2=1,_tmp2EF)))))));}}
_npop_handler(0);return;}_LL65: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpAA=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAA->tag != 1)goto _LL67;}_LL66:
# 288
{const char*_tmp2F4;void*_tmp2F3;(_tmp2F3=0,Cyc_Tcutil_warn(loc,((_tmp2F4="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp2F4,sizeof(char),57))),_tag_dyneither(_tmp2F3,sizeof(void*),0)));}
goto _LL62;_LL67: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAB->tag != 2)goto _LL69;}_LL68:
# 291
{const char*_tmp2F7;void*_tmp2F6;(_tmp2F6=0,Cyc_Tcutil_warn(loc,((_tmp2F7="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp2F7,sizeof(char),59))),_tag_dyneither(_tmp2F6,sizeof(void*),0)));}
goto _LL62;_LL69: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAC=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAC->tag != 4)goto _LL6B;}_LL6A:
 goto _LL6C;_LL6B: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAD=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAD->tag != 3)goto _LL62;}_LL6C:
# 295
{const char*_tmp2FA;void*_tmp2F9;(_tmp2F9=0,Cyc_Tcutil_warn(loc,((_tmp2FA="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2FA,sizeof(char),47))),_tag_dyneither(_tmp2F9,sizeof(void*),0)));}
goto _LL62;_LL62:;}
# 274
;_pop_handler();}else{void*_tmpA4=(void*)_exn_thrown;void*_tmpBC=_tmpA4;void*_tmpBE;_LL6E: {struct Cyc_Dict_Absent_exn_struct*_tmpBD=(struct Cyc_Dict_Absent_exn_struct*)_tmpBC;if(_tmpBD->tag != Cyc_Dict_Absent)goto _LL70;}_LL6F:
# 298
 goto _LL6D;_LL70: _tmpBE=_tmpBC;_LL71:(void)_throw(_tmpBE);_LL6D:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp309;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp308;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp307;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp306;struct _tuple14*_tmp305;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3F,(
(_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305->f1=(void*)((_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309[0]=((_tmp306.tag=0,((_tmp306.f1=(void*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308[0]=((_tmp307.tag=1,((_tmp307.f1=vd,_tmp307)))),_tmp308)))),_tmp306)))),_tmp309)))),((_tmp305->f2=0,_tmp305)))))));}}
# 304
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmpC5;struct _dyneither_ptr*_tmpC6;struct _tuple0*_tmpC4=n;_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;{
union Cyc_Absyn_Nmspace _tmpC7=_tmpC5;_LL73: if((_tmpC7.Abs_n).tag != 2)goto _LL75;if((struct Cyc_List_List*)(_tmpC7.Abs_n).val != 0)goto _LL75;_LL74: {
# 308
const char*_tmp30A;return Cyc_strcmp((struct _dyneither_ptr)*_tmpC6,((_tmp30A="main",_tag_dyneither(_tmp30A,sizeof(char),5))))== 0;}_LL75:;_LL76:
 return 0;_LL72:;};}
# 313
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 316
struct _dyneither_ptr*v=(*fd->name).f2;
# 320
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp30D;void*_tmp30C;(_tmp30C=0,Cyc_Tcutil_terr(loc,((_tmp30D="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp30D,sizeof(char),54))),_tag_dyneither(_tmp30C,sizeof(void*),0)));}
# 323
{union Cyc_Absyn_Nmspace _stmttmpC=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpCB=_stmttmpC;_LL78: if((_tmpCB.Rel_n).tag != 1)goto _LL7A;if((struct Cyc_List_List*)(_tmpCB.Rel_n).val != 0)goto _LL7A;_LL79:
# 326
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL77;_LL7A:;_LL7B:
# 329
 goto _LL77;_LL77:;}
# 332
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 334
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 336
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 339
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmpD=(void*)atts->hd;void*_tmpCC=_stmttmpD;_LL7D: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpCD=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpCC;if(_tmpCD->tag != 7)goto _LL7F;}_LL7E:
 goto _LL80;_LL7F: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpCE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpCC;if(_tmpCE->tag != 6)goto _LL81;}_LL80:
# 343
{const char*_tmp311;void*_tmp310[1];struct Cyc_String_pa_PrintArg_struct _tmp30F;(_tmp30F.tag=0,((_tmp30F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp310[0]=& _tmp30F,Cyc_Tcutil_terr(loc,((_tmp311="bad attribute %s for function",_tag_dyneither(_tmp311,sizeof(char),30))),_tag_dyneither(_tmp310,sizeof(void*),1)))))));}
goto _LL7C;_LL81:;_LL82:
 goto _LL7C;_LL7C:;}}
# 349
{struct _handler_cons _tmpD2;_push_handler(& _tmpD2);{int _tmpD4=0;if(setjmp(_tmpD2.handler))_tmpD4=1;if(!_tmpD4){
{struct _tuple14*_tmpD5=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_stmttmpE=(*_tmpD5).f1;void*_tmpD6=_stmttmpE;void*_tmpD8;_LL84: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD7=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD6;if(_tmpD7->tag != 0)goto _LL86;else{_tmpD8=(void*)_tmpD7->f1;}}_LL85: {
# 353
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp314;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp313;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpDD=(_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=((_tmp314.tag=2,((_tmp314.f1=fd,_tmp314)))),_tmp313)));
void*_tmpDE=Cyc_Tcdecl_merge_binding(_tmpD8,(void*)_tmpDD,loc,Cyc_Tc_tc_msg);
if(_tmpDE == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL83;
# 357
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpDE == _tmpD8  && (*_tmpD5).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp31A;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp319;struct _tuple14*_tmp318;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318->f1=(void*)((_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=((_tmp319.tag=0,((_tmp319.f1=_tmpDE,_tmp319)))),_tmp31A)))),((_tmp318->f2=1,_tmp318)))))));}}
goto _LL83;}_LL86: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpD9=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpD6;if(_tmpD9->tag != 1)goto _LL88;}_LL87:
# 363
{const char*_tmp31D;void*_tmp31C;(_tmp31C=0,Cyc_Tcutil_warn(loc,((_tmp31D="function declaration shadows previous type declaration",_tag_dyneither(_tmp31D,sizeof(char),55))),_tag_dyneither(_tmp31C,sizeof(void*),0)));}
goto _LL83;_LL88: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpDA=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpD6;if(_tmpDA->tag != 2)goto _LL8A;}_LL89:
# 366
{const char*_tmp320;void*_tmp31F;(_tmp31F=0,Cyc_Tcutil_warn(loc,((_tmp320="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp320,sizeof(char),59))),_tag_dyneither(_tmp31F,sizeof(void*),0)));}
goto _LL83;_LL8A: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpDB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpD6;if(_tmpDB->tag != 4)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpDC=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpD6;if(_tmpDC->tag != 3)goto _LL83;}_LL8D:
# 370
{const char*_tmp323;void*_tmp322;(_tmp322=0,Cyc_Tcutil_warn(loc,((_tmp323="function declaration shadows previous enum tag",_tag_dyneither(_tmp323,sizeof(char),47))),_tag_dyneither(_tmp322,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 350
;_pop_handler();}else{void*_tmpD3=(void*)_exn_thrown;void*_tmpEB=_tmpD3;void*_tmpED;_LL8F: {struct Cyc_Dict_Absent_exn_struct*_tmpEC=(struct Cyc_Dict_Absent_exn_struct*)_tmpEB;if(_tmpEC->tag != Cyc_Dict_Absent)goto _LL91;}_LL90:
# 374
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp326;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp325;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpEE=(_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325[0]=((_tmp326.tag=2,((_tmp326.f1=fd,_tmp326)))),_tmp325)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp335;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp334;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp333;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp332;struct _tuple14*_tmp331;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp331=_cycalloc(sizeof(*_tmp331)),((_tmp331->f1=(void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp332.tag=0,((_tmp332.f1=(void*)((_tmp334=_cycalloc(sizeof(*_tmp334)),((_tmp334[0]=((_tmp333.tag=2,((_tmp333.f1=fd,_tmp333)))),_tmp334)))),_tmp332)))),_tmp335)))),((_tmp331->f2=0,_tmp331)))))));}
# 379
goto _LL8E;_LL91: _tmpED=_tmpEB;_LL92:(void)_throw(_tmpED);_LL8E:;}};}
# 383
if(in_cinclude)return;{
# 388
struct _RegionHandle _tmpF6=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpF6;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmpF7=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp336;struct Cyc_Tcenv_Tenv*_tmpF8=(_tmp336=_region_malloc(fnrgn,sizeof(*_tmp336)),((_tmp336->ns=te->ns,((_tmp336->ae=te->ae,((_tmp336->le=_tmpF7,((_tmp336->allow_valueof=0,_tmp336)))))))));
# 392
struct _handler_cons _tmpF9;_push_handler(& _tmpF9);{int _tmpFB=0;if(setjmp(_tmpF9.handler))_tmpFB=1;if(!_tmpFB){
Cyc_Tcstmt_tcStmt(_tmpF8,fd->body,0);
# 396
Cyc_Tcenv_check_delayed_effects(_tmpF8);
Cyc_Tcenv_check_delayed_constraints(_tmpF8);
# 400
if(!Cyc_Tcenv_all_labels_resolved(_tmpF8)){
const char*_tmp339;void*_tmp338;(_tmp338=0,Cyc_Tcutil_terr(loc,((_tmp339="function has goto statements to undefined labels",_tag_dyneither(_tmp339,sizeof(char),49))),_tag_dyneither(_tmp338,sizeof(void*),0)));}
# 393
;_pop_handler();}else{void*_tmpFA=(void*)_exn_thrown;void*_tmpFF=_tmpFA;void*_tmp101;void*_tmp102;_LL94: {struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp100=(struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmpFF;if(_tmp100->tag != Cyc_Tcutil_AbortTypeCheckingFunction)goto _LL96;}_LL95:
# 403
 goto _LL93;_LL96: _tmp101=_tmpFF;_LL97:
 Cyc_Core_rethrow(_tmp101);_LL98: _tmp102=_tmpFF;_LL99:(void)_throw(_tmp102);_LL93:;}};}
# 408
if(Cyc_Tc_is_main(fd->name)){
# 410
{void*_stmttmpF=Cyc_Tcutil_compress(fd->ret_type);void*_tmp104=_stmttmpF;enum Cyc_Absyn_Size_of _tmp107;_LL9B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp105=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp104;if(_tmp105->tag != 0)goto _LL9D;}_LL9C:
{const char*_tmp33C;void*_tmp33B;(_tmp33B=0,Cyc_Tcutil_warn(loc,((_tmp33C="main declared with return type void",_tag_dyneither(_tmp33C,sizeof(char),36))),_tag_dyneither(_tmp33B,sizeof(void*),0)));}goto _LL9A;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp106=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp104;if(_tmp106->tag != 6)goto _LL9F;else{_tmp107=_tmp106->f2;}}_LL9E:
 goto _LL9A;_LL9F:;_LLA0:
# 414
{const char*_tmp340;void*_tmp33F[1];struct Cyc_String_pa_PrintArg_struct _tmp33E;(_tmp33E.tag=0,((_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp33F[0]=& _tmp33E,Cyc_Tcutil_terr(loc,((_tmp340="main declared with return type %s instead of int or void",_tag_dyneither(_tmp340,sizeof(char),57))),_tag_dyneither(_tmp33F,sizeof(void*),1)))))));}
goto _LL9A;_LL9A:;}
# 418
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp343;void*_tmp342;(_tmp342=0,Cyc_Tcutil_terr(loc,((_tmp343="main declared with varargs",_tag_dyneither(_tmp343,sizeof(char),27))),_tag_dyneither(_tmp342,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp10F=fd->args;
if(_tmp10F != 0){
struct _tuple8*_stmttmp10=(struct _tuple8*)_tmp10F->hd;void*_tmp111;struct _tuple8*_tmp110=_stmttmp10;_tmp111=_tmp110->f3;
{void*_stmttmp11=Cyc_Tcutil_compress(_tmp111);void*_tmp112=_stmttmp11;enum Cyc_Absyn_Size_of _tmp114;_LLA2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp113=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp112;if(_tmp113->tag != 6)goto _LLA4;else{_tmp114=_tmp113->f2;}}_LLA3:
 goto _LLA1;_LLA4:;_LLA5:
# 426
{const char*_tmp347;void*_tmp346[1];struct Cyc_String_pa_PrintArg_struct _tmp345;(_tmp345.tag=0,((_tmp345.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp111)),((_tmp346[0]=& _tmp345,Cyc_Tcutil_terr(loc,((_tmp347="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp347,sizeof(char),60))),_tag_dyneither(_tmp346,sizeof(void*),1)))))));}
goto _LLA1;_LLA1:;}
# 430
_tmp10F=_tmp10F->tl;
if(_tmp10F != 0){
struct _tuple8*_stmttmp12=(struct _tuple8*)_tmp10F->hd;void*_tmp119;struct _tuple8*_tmp118=_stmttmp12;_tmp119=_tmp118->f3;
_tmp10F=_tmp10F->tl;
if(_tmp10F != 0){
const char*_tmp34A;void*_tmp349;(_tmp349=0,Cyc_Tcutil_terr(loc,((_tmp34A="main declared with too many arguments",_tag_dyneither(_tmp34A,sizeof(char),38))),_tag_dyneither(_tmp349,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp34B;struct Cyc_Core_Opt*tvs=(_tmp34B=_cycalloc(sizeof(*_tmp34B)),((_tmp34B->v=fd->tvs,_tmp34B)));
struct Cyc_Core_Opt*_tmp353;struct Cyc_Core_Opt*_tmp352;struct Cyc_Core_Opt*_tmp351;struct Cyc_Core_Opt*_tmp350;struct Cyc_Core_Opt*_tmp34F;struct Cyc_Core_Opt*_tmp34E;struct Cyc_Core_Opt*_tmp34D;struct Cyc_Core_Opt*_tmp34C;if(((!Cyc_Tcutil_unify(_tmp119,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352->v=& Cyc_Tcutil_rk,_tmp352)))),tvs)),
Cyc_Absyn_new_evar(((_tmp353=_cycalloc(sizeof(*_tmp353)),((_tmp353->v=& Cyc_Tcutil_rk,_tmp353)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmp119,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350->v=& Cyc_Tcutil_rk,_tmp350)))),tvs)),
# 442
Cyc_Absyn_new_evar(((_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351->v=& Cyc_Tcutil_rk,_tmp351)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp119,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E->v=& Cyc_Tcutil_rk,_tmp34E)))),tvs)),
Cyc_Absyn_new_evar(((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F->v=& Cyc_Tcutil_rk,_tmp34F)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp119,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C->v=& Cyc_Tcutil_rk,_tmp34C)))),tvs)),
# 449
Cyc_Absyn_new_evar(((_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->v=& Cyc_Tcutil_rk,_tmp34D)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp357;void*_tmp356[1];struct Cyc_String_pa_PrintArg_struct _tmp355;(_tmp355.tag=0,((_tmp355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp119)),((_tmp356[0]=& _tmp355,Cyc_Tcutil_terr(loc,((_tmp357="second argument of main has type %s instead of char??",_tag_dyneither(_tmp357,sizeof(char),54))),_tag_dyneither(_tmp356,sizeof(void*),1)))))));}};}}};}
# 388
;_pop_region(fnrgn);};};}
# 459
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 465
{union Cyc_Absyn_Nmspace _stmttmp13=(*td->name).f1;union Cyc_Absyn_Nmspace _tmp128=_stmttmp13;_LLA7: if((_tmp128.Rel_n).tag != 1)goto _LLA9;_LLA8:
# 468
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LLA6;_LLA9:;_LLAA:
 goto _LLA6;_LLA6:;}
# 473
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp35B;void*_tmp35A[1];struct Cyc_String_pa_PrintArg_struct _tmp359;(_tmp359.tag=0,((_tmp359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp35A[0]=& _tmp359,Cyc_Tcutil_terr(loc,((_tmp35B="redeclaration of typedef %s",_tag_dyneither(_tmp35B,sizeof(char),28))),_tag_dyneither(_tmp35A,sizeof(void*),1)))))));}
return;}
# 478
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 487
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_stmttmp14=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp12C=_stmttmp14;struct Cyc_Core_Opt**_tmp12E;struct Cyc_Core_Opt**_tmp130;struct Cyc_Absyn_Kind*_tmp131;_LLAC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp12D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp12C;if(_tmp12D->tag != 1)goto _LLAE;else{_tmp12E=(struct Cyc_Core_Opt**)& _tmp12D->f1;}}_LLAD:
# 490
 if(td->defn != 0){
const char*_tmp35F;void*_tmp35E[1];struct Cyc_String_pa_PrintArg_struct _tmp35D;(_tmp35D.tag=0,((_tmp35D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp35E[0]=& _tmp35D,Cyc_Tcutil_warn(loc,((_tmp35F="type variable %s is not used in typedef definition",_tag_dyneither(_tmp35F,sizeof(char),51))),_tag_dyneither(_tmp35E,sizeof(void*),1)))))));}
# 493
{struct Cyc_Core_Opt*_tmp360;*_tmp12E=((_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp360))));}goto _LLAB;_LLAE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12C;if(_tmp12F->tag != 2)goto _LLB0;else{_tmp130=(struct Cyc_Core_Opt**)& _tmp12F->f1;_tmp131=_tmp12F->f2;}}_LLAF:
# 500
{struct Cyc_Core_Opt*_tmp361;*_tmp130=((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361->v=Cyc_Tcutil_kind_to_bound(_tmp131),_tmp361))));}
goto _LLAB;_LLB0:;_LLB1:
 continue;_LLAB:;}}
# 506
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 509
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 516
struct _RegionHandle _tmp137=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp137;_push_region(uprev_rgn);
# 518
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_stmttmp15=(struct _tuple10*)rpo->hd;void*_tmp139;void*_tmp13A;struct _tuple10*_tmp138=_stmttmp15;_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp139);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp13A);}{
# 524
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 527
struct Cyc_List_List*_tmp13B=fields;for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp16=(struct Cyc_Absyn_Aggrfield*)_tmp13B->hd;struct _dyneither_ptr*_tmp13D;struct Cyc_Absyn_Tqual _tmp13E;void*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_List_List*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Aggrfield*_tmp13C=_stmttmp16;_tmp13D=_tmp13C->name;_tmp13E=_tmp13C->tq;_tmp13F=_tmp13C->type;_tmp140=_tmp13C->width;_tmp141=_tmp13C->attributes;_tmp142=_tmp13C->requires_clause;
# 530
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp13D)){
const char*_tmp365;void*_tmp364[1];struct Cyc_String_pa_PrintArg_struct _tmp363;(_tmp363.tag=0,((_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp13D),((_tmp364[0]=& _tmp363,Cyc_Tcutil_terr(loc,((_tmp365="duplicate member %s",_tag_dyneither(_tmp365,sizeof(char),20))),_tag_dyneither(_tmp364,sizeof(void*),1)))))));}
# 534
{const char*_tmp366;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp13D,((_tmp366="",_tag_dyneither(_tmp366,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp367;prev_fields=((_tmp367=_region_malloc(uprev_rgn,sizeof(*_tmp367)),((_tmp367->hd=_tmp13D,((_tmp367->tl=prev_fields,_tmp367))))));}}{
# 537
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 541
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp13B->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp13F);
# 546
(((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp13B->hd)->tq).print_const,_tmp13F);
# 549
Cyc_Tcutil_check_bitfield(loc,te,_tmp13F,_tmp140,_tmp13D);
# 551
if((unsigned int)_tmp142){
if(str_or_union != Cyc_Absyn_UnionA){
const char*_tmp36A;void*_tmp369;(_tmp369=0,Cyc_Tcutil_terr(loc,((_tmp36A="@requires clauses are only allowed on union members",_tag_dyneither(_tmp36A,sizeof(char),52))),_tag_dyneither(_tmp369,sizeof(void*),0)));}
{struct _RegionHandle _tmp14A=_new_region("temp");struct _RegionHandle*temp=& _tmp14A;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp14B=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp14B,0,_tmp142);}
# 555
;_pop_region(temp);}
# 558
if(!Cyc_Tcutil_is_integral(_tmp142)){
const char*_tmp36E;void*_tmp36D[1];struct Cyc_String_pa_PrintArg_struct _tmp36C;(_tmp36C.tag=0,((_tmp36C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 561
Cyc_Absynpp_typ2string((void*)_check_null(_tmp142->topt))),((_tmp36D[0]=& _tmp36C,Cyc_Tcutil_terr(_tmp142->loc,((_tmp36E="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp36E,sizeof(char),54))),_tag_dyneither(_tmp36D,sizeof(void*),1)))))));}else{
# 563
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp371;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp370;Cyc_Tcutil_check_type(_tmp142->loc,te,tvs,& Cyc_Tcutil_ik,0,(void*)((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370[0]=((_tmp371.tag=18,((_tmp371.f1=_tmp142,_tmp371)))),_tmp370)))));}{
# 565
struct Cyc_List_List*_tmp151=Cyc_Relations_exp2relns(uprev_rgn,_tmp142);
# 572
if(!Cyc_Relations_consistent_relations(_tmp151)){
const char*_tmp374;void*_tmp373;(_tmp373=0,Cyc_Tcutil_terr(_tmp142->loc,((_tmp374="@requires clause may be unsatisfiable",_tag_dyneither(_tmp374,sizeof(char),38))),_tag_dyneither(_tmp373,sizeof(void*),0)));}
# 578
{struct Cyc_List_List*_tmp154=prev_relations;for(0;_tmp154 != 0;_tmp154=_tmp154->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp151,(struct Cyc_List_List*)_tmp154->hd))){
const char*_tmp377;void*_tmp376;(_tmp376=0,Cyc_Tcutil_terr(_tmp142->loc,((_tmp377="@requires clause may overlap with previous clauses",_tag_dyneither(_tmp377,sizeof(char),51))),_tag_dyneither(_tmp376,sizeof(void*),0)));}}}{
# 583
struct Cyc_List_List*_tmp378;prev_relations=((_tmp378=_region_malloc(uprev_rgn,sizeof(*_tmp378)),((_tmp378->hd=_tmp151,((_tmp378->tl=prev_relations,_tmp378))))));};};}}else{
# 586
if(prev_relations != 0){
const char*_tmp37B;void*_tmp37A;(_tmp37A=0,Cyc_Tcutil_terr(loc,((_tmp37B="if one field has a @requires clause, they all must",_tag_dyneither(_tmp37B,sizeof(char),51))),_tag_dyneither(_tmp37A,sizeof(void*),0)));}}};}};
# 518
;_pop_region(uprev_rgn);}
# 593
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 595
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp17=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp15A=_stmttmp17;struct Cyc_Core_Opt**_tmp15C;struct Cyc_Core_Opt**_tmp15E;enum Cyc_Absyn_AliasQual _tmp15F;struct Cyc_Core_Opt**_tmp161;struct Cyc_Core_Opt**_tmp163;struct Cyc_Absyn_Kind*_tmp164;enum Cyc_Absyn_AliasQual _tmp166;_LLB3: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp15B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp15A;if(_tmp15B->tag != 1)goto _LLB5;else{_tmp15C=(struct Cyc_Core_Opt**)& _tmp15B->f1;}}_LLB4:
# 598
{struct Cyc_Core_Opt*_tmp37C;*_tmp15C=((_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp37C))));}continue;_LLB5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15A;if(_tmp15D->tag != 2)goto _LLB7;else{_tmp15E=(struct Cyc_Core_Opt**)& _tmp15D->f1;if((_tmp15D->f2)->kind != Cyc_Absyn_MemKind)goto _LLB7;_tmp15F=(_tmp15D->f2)->aliasqual;}}_LLB6:
# 600
 if(constrain_top_kind  && _tmp15F == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp37F;struct Cyc_Core_Opt*_tmp37E;*_tmp15E=((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E->v=Cyc_Tcutil_kind_to_bound(((_tmp37F=_cycalloc_atomic(sizeof(*_tmp37F)),((_tmp37F->kind=Cyc_Absyn_BoxKind,((_tmp37F->aliasqual=Cyc_Absyn_Aliasable,_tmp37F))))))),_tmp37E))));}else{
# 603
struct Cyc_Absyn_Kind*_tmp382;struct Cyc_Core_Opt*_tmp381;*_tmp15E=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->v=Cyc_Tcutil_kind_to_bound(((_tmp382=_cycalloc_atomic(sizeof(*_tmp382)),((_tmp382->kind=Cyc_Absyn_BoxKind,((_tmp382->aliasqual=_tmp15F,_tmp382))))))),_tmp381))));}
continue;_LLB7:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp160=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15A;if(_tmp160->tag != 2)goto _LLB9;else{_tmp161=(struct Cyc_Core_Opt**)& _tmp160->f1;if((_tmp160->f2)->kind != Cyc_Absyn_BoxKind)goto _LLB9;if((_tmp160->f2)->aliasqual != Cyc_Absyn_Top)goto _LLB9;}}if(!constrain_top_kind)goto _LLB9;_LLB8:
# 606
{struct Cyc_Absyn_Kind*_tmp385;struct Cyc_Core_Opt*_tmp384;*_tmp161=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->v=Cyc_Tcutil_kind_to_bound(((_tmp385=_cycalloc_atomic(sizeof(*_tmp385)),((_tmp385->kind=Cyc_Absyn_BoxKind,((_tmp385->aliasqual=Cyc_Absyn_Aliasable,_tmp385))))))),_tmp384))));}
continue;_LLB9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp162=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp15A;if(_tmp162->tag != 2)goto _LLBB;else{_tmp163=(struct Cyc_Core_Opt**)& _tmp162->f1;_tmp164=_tmp162->f2;}}_LLBA:
# 609
{struct Cyc_Core_Opt*_tmp386;*_tmp163=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->v=Cyc_Tcutil_kind_to_bound(_tmp164),_tmp386))));}continue;_LLBB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp165=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp15A;if(_tmp165->tag != 0)goto _LLBD;else{if((_tmp165->f1)->kind != Cyc_Absyn_MemKind)goto _LLBD;_tmp166=(_tmp165->f1)->aliasqual;}}_LLBC:
# 611
{const char*_tmp38E;void*_tmp38D[3];struct Cyc_String_pa_PrintArg_struct _tmp38C;struct Cyc_String_pa_PrintArg_struct _tmp38B;struct Cyc_Absyn_Kind*_tmp38A;struct Cyc_String_pa_PrintArg_struct _tmp389;(_tmp389.tag=0,((_tmp389.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp38A=_cycalloc_atomic(sizeof(*_tmp38A)),((_tmp38A->kind=Cyc_Absyn_MemKind,((_tmp38A->aliasqual=_tmp166,_tmp38A)))))))),((_tmp38B.tag=0,((_tmp38B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp38C.tag=0,((_tmp38C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp38D[0]=& _tmp38C,((_tmp38D[1]=& _tmp38B,((_tmp38D[2]=& _tmp389,Cyc_Tcutil_terr(loc,((_tmp38E="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp38E,sizeof(char),57))),_tag_dyneither(_tmp38D,sizeof(void*),3)))))))))))))))))));}
continue;_LLBD:;_LLBE:
 continue;_LLB2:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 619
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp175=(*ad->name).f2;
# 626
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmp18=(void*)atts->hd;void*_tmp176=_stmttmp18;_LLC0: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp177=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp176;if(_tmp177->tag != 7)goto _LLC2;}_LLC1:
 goto _LLC3;_LLC2: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp178=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp176;if(_tmp178->tag != 6)goto _LLC4;}_LLC3:
 continue;_LLC4:;_LLC5:
# 631
{const char*_tmp393;void*_tmp392[2];struct Cyc_String_pa_PrintArg_struct _tmp391;struct Cyc_String_pa_PrintArg_struct _tmp390;(_tmp390.tag=0,((_tmp390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp175),((_tmp391.tag=0,((_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp392[0]=& _tmp391,((_tmp392[1]=& _tmp390,Cyc_Tcutil_terr(loc,((_tmp393="bad attribute %s in %s definition",_tag_dyneither(_tmp393,sizeof(char),34))),_tag_dyneither(_tmp392,sizeof(void*),2)))))))))))));}
goto _LLBF;_LLBF:;}}{
# 636
struct Cyc_List_List*_tmp17D=ad->tvs;
# 638
{union Cyc_Absyn_Nmspace _stmttmp19=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp17E=_stmttmp19;_LLC7: if((_tmp17E.Rel_n).tag != 1)goto _LLC9;if((struct Cyc_List_List*)(_tmp17E.Rel_n).val != 0)goto _LLC9;_LLC8:
# 641
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLC6;_LLC9:;_LLCA:
# 644
 goto _LLC6;_LLC6:;}
# 648
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 650
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 654
{struct _tuple15 _tmp394;struct _tuple15 _stmttmp1A=(_tmp394.f1=ad->impl,((_tmp394.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp175),_tmp394)));struct _tuple15 _tmp17F=_stmttmp1A;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;int _tmp183;struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp186;int _tmp187;struct Cyc_Absyn_Aggrdecl**_tmp188;struct Cyc_Absyn_Aggrdecl**_tmp189;_LLCC: if(_tmp17F.f1 != 0)goto _LLCE;if(_tmp17F.f2 != 0)goto _LLCE;_LLCD:
# 657
 Cyc_Tc_rule_out_memkind(loc,_tmp175,_tmp17D,0);
# 659
{struct Cyc_Absyn_Aggrdecl**_tmp395;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp175,((_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395[0]=ad,_tmp395)))));}
goto _LLCB;_LLCE: if(_tmp17F.f1 == 0)goto _LLD0;_tmp180=(_tmp17F.f1)->exist_vars;_tmp181=(_tmp17F.f1)->rgn_po;_tmp182=(_tmp17F.f1)->fields;_tmp183=(_tmp17F.f1)->tagged;if(_tmp17F.f2 != 0)goto _LLD0;_LLCF: {
# 664
struct Cyc_Absyn_Aggrdecl*_tmp398;struct Cyc_Absyn_Aggrdecl**_tmp397;struct Cyc_Absyn_Aggrdecl**_tmp18B=(_tmp397=_cycalloc(sizeof(*_tmp397)),((_tmp397[0]=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398->kind=ad->kind,((_tmp398->sc=Cyc_Absyn_Extern,((_tmp398->name=ad->name,((_tmp398->tvs=_tmp17D,((_tmp398->impl=0,((_tmp398->attributes=ad->attributes,_tmp398)))))))))))))),_tmp397)));
# 666
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp175,_tmp18B);
# 671
Cyc_Tcutil_check_unique_tvars(loc,_tmp180);
# 673
Cyc_Tcutil_add_tvar_identities(_tmp180);
# 676
if(_tmp183  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp39B;void*_tmp39A;(_tmp39A=0,Cyc_Tcutil_terr(loc,((_tmp39B="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp39B,sizeof(char),56))),_tag_dyneither(_tmp39A,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp17D,_tmp180),_tmp181,_tmp182);
# 680
Cyc_Tc_rule_out_memkind(loc,_tmp175,_tmp17D,0);
# 683
Cyc_Tc_rule_out_memkind(loc,_tmp175,_tmp180,1);
# 685
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp183){
# 688
struct Cyc_List_List*f=_tmp182;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0){
const char*_tmp3A1;void*_tmp3A0[3];struct Cyc_String_pa_PrintArg_struct _tmp39F;struct Cyc_String_pa_PrintArg_struct _tmp39E;struct Cyc_String_pa_PrintArg_struct _tmp39D;(_tmp39D.tag=0,((_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp39E.tag=0,((_tmp39E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp175),((_tmp39F.tag=0,((_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp3A0[0]=& _tmp39F,((_tmp3A0[1]=& _tmp39E,((_tmp3A0[2]=& _tmp39D,Cyc_Tcutil_warn(loc,((_tmp3A1="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp3A1,sizeof(char),98))),_tag_dyneither(_tmp3A0,sizeof(void*),3)))))))))))))))))));}}}
# 693
*_tmp18B=ad;
goto _LLCB;}_LLD0: if(_tmp17F.f1 == 0)goto _LLD2;_tmp184=(_tmp17F.f1)->exist_vars;_tmp185=(_tmp17F.f1)->rgn_po;_tmp186=(_tmp17F.f1)->fields;_tmp187=(_tmp17F.f1)->tagged;if(_tmp17F.f2 == 0)goto _LLD2;_tmp188=*_tmp17F.f2;_LLD1:
# 697
 if(ad->kind != (*_tmp188)->kind){
const char*_tmp3A4;void*_tmp3A3;(_tmp3A3=0,Cyc_Tcutil_terr(loc,((_tmp3A4="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp3A4,sizeof(char),52))),_tag_dyneither(_tmp3A3,sizeof(void*),0)));}{
# 700
struct Cyc_Absyn_Aggrdecl*_tmp197=*_tmp188;
# 702
{struct Cyc_Absyn_Aggrdecl*_tmp3A5;*_tmp188=((_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5->kind=ad->kind,((_tmp3A5->sc=Cyc_Absyn_Extern,((_tmp3A5->name=ad->name,((_tmp3A5->tvs=_tmp17D,((_tmp3A5->impl=0,((_tmp3A5->attributes=ad->attributes,_tmp3A5))))))))))))));}
# 708
Cyc_Tcutil_check_unique_tvars(loc,_tmp184);
# 710
Cyc_Tcutil_add_tvar_identities(_tmp184);
# 712
if(_tmp187  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_terr(loc,((_tmp3A8="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3A8,sizeof(char),56))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp17D,_tmp184),_tmp185,_tmp186);
# 716
*_tmp188=_tmp197;
_tmp189=_tmp188;goto _LLD3;};_LLD2: if(_tmp17F.f1 != 0)goto _LLCB;if(_tmp17F.f2 == 0)goto _LLCB;_tmp189=*_tmp17F.f2;_LLD3: {
# 720
struct Cyc_Absyn_Aggrdecl*_tmp19B=Cyc_Tcdecl_merge_aggrdecl(*_tmp189,ad,loc,Cyc_Tc_tc_msg);
if(_tmp19B == 0)
return;else{
# 724
Cyc_Tc_rule_out_memkind(loc,_tmp175,_tmp17D,0);
# 727
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp175,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 730
*_tmp189=_tmp19B;
ad=_tmp19B;
goto _LLCB;}}_LLCB:;}{
# 736
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3AE;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp3AD;struct _tuple14*_tmp3AC;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp175,(
(_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->f1=(void*)((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE[0]=((_tmp3AD.tag=1,((_tmp3AD.f1=ad,_tmp3AD)))),_tmp3AE)))),((_tmp3AC->f2=1,_tmp3AC)))))));};};}
# 740
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp1B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp1A0=_stmttmp1B;struct Cyc_Core_Opt**_tmp1A2;struct Cyc_Core_Opt**_tmp1A4;struct Cyc_Core_Opt**_tmp1A6;struct Cyc_Core_Opt**_tmp1A8;struct Cyc_Core_Opt**_tmp1AA;struct Cyc_Core_Opt**_tmp1AC;struct Cyc_Core_Opt**_tmp1AF;enum Cyc_Absyn_KindQual _tmp1B0;enum Cyc_Absyn_KindQual _tmp1B2;enum Cyc_Absyn_AliasQual _tmp1B4;_LLD5: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1A1=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1A1->tag != 1)goto _LLD7;else{_tmp1A2=(struct Cyc_Core_Opt**)& _tmp1A1->f1;}}_LLD6:
 _tmp1A4=_tmp1A2;goto _LLD8;_LLD7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1A3->tag != 2)goto _LLD9;else{_tmp1A4=(struct Cyc_Core_Opt**)& _tmp1A3->f1;if((_tmp1A3->f2)->kind != Cyc_Absyn_MemKind)goto _LLD9;if((_tmp1A3->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD9;}}_LLD8:
# 745
 _tmp1A6=_tmp1A4;goto _LLDA;_LLD9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1A5->tag != 2)goto _LLDB;else{_tmp1A6=(struct Cyc_Core_Opt**)& _tmp1A5->f1;if((_tmp1A5->f2)->kind != Cyc_Absyn_MemKind)goto _LLDB;if((_tmp1A5->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDB;}}_LLDA:
# 747
{struct Cyc_Core_Opt*_tmp3AF;*_tmp1A6=((_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3AF))));}goto _LLD4;_LLDB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1A7->tag != 2)goto _LLDD;else{_tmp1A8=(struct Cyc_Core_Opt**)& _tmp1A7->f1;if((_tmp1A7->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDD;if((_tmp1A7->f2)->aliasqual != Cyc_Absyn_Top)goto _LLDD;}}_LLDC:
 _tmp1AA=_tmp1A8;goto _LLDE;_LLDD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1A9->tag != 2)goto _LLDF;else{_tmp1AA=(struct Cyc_Core_Opt**)& _tmp1A9->f1;if((_tmp1A9->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDF;if((_tmp1A9->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDF;}}_LLDE:
# 750
{struct Cyc_Core_Opt*_tmp3B0;*_tmp1AA=((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp3B0))));}goto _LLD4;_LLDF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1AB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1AB->tag != 2)goto _LLE1;else{_tmp1AC=(struct Cyc_Core_Opt**)& _tmp1AB->f1;if((_tmp1AB->f2)->kind != Cyc_Absyn_RgnKind)goto _LLE1;if((_tmp1AB->f2)->aliasqual != Cyc_Absyn_Top)goto _LLE1;}}_LLE0:
# 752
{struct Cyc_Core_Opt*_tmp3B1;*_tmp1AC=((_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp3B1))));}goto _LLD4;_LLE1: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1AD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1AD->tag != 0)goto _LLE3;else{if((_tmp1AD->f1)->kind != Cyc_Absyn_RgnKind)goto _LLE3;if((_tmp1AD->f1)->aliasqual != Cyc_Absyn_Top)goto _LLE3;}}_LLE2:
# 754
{const char*_tmp3B6;void*_tmp3B5[2];struct Cyc_String_pa_PrintArg_struct _tmp3B4;struct Cyc_String_pa_PrintArg_struct _tmp3B3;(_tmp3B3.tag=0,((_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3B4.tag=0,((_tmp3B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B5[0]=& _tmp3B4,((_tmp3B5[1]=& _tmp3B3,Cyc_Tcutil_terr(loc,((_tmp3B6="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp3B6,sizeof(char),57))),_tag_dyneither(_tmp3B5,sizeof(void*),2)))))))))))));}
goto _LLD4;_LLE3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1AE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1AE->tag != 2)goto _LLE5;else{_tmp1AF=(struct Cyc_Core_Opt**)& _tmp1AE->f1;_tmp1B0=(_tmp1AE->f2)->kind;if((_tmp1AE->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLE5;}}_LLE4:
 _tmp1B2=_tmp1B0;goto _LLE6;_LLE5: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1B1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1B1->tag != 0)goto _LLE7;else{_tmp1B2=(_tmp1B1->f1)->kind;if((_tmp1B1->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLE7;}}_LLE6:
# 758
{const char*_tmp3BE;void*_tmp3BD[3];struct Cyc_String_pa_PrintArg_struct _tmp3BC;struct Cyc_String_pa_PrintArg_struct _tmp3BB;struct Cyc_Absyn_Kind*_tmp3BA;struct Cyc_String_pa_PrintArg_struct _tmp3B9;(_tmp3B9.tag=0,((_tmp3B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 760
Cyc_Absynpp_kind2string(((_tmp3BA=_cycalloc_atomic(sizeof(*_tmp3BA)),((_tmp3BA->kind=_tmp1B2,((_tmp3BA->aliasqual=Cyc_Absyn_Unique,_tmp3BA)))))))),((_tmp3BB.tag=0,((_tmp3BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3BC.tag=0,((_tmp3BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3BD[0]=& _tmp3BC,((_tmp3BD[1]=& _tmp3BB,((_tmp3BD[2]=& _tmp3B9,Cyc_Tcutil_terr(loc,((_tmp3BE="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3BE,sizeof(char),57))),_tag_dyneither(_tmp3BD,sizeof(void*),3)))))))))))))))))));}goto _LLD4;_LLE7: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1B3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1A0;if(_tmp1B3->tag != 0)goto _LLE9;else{if((_tmp1B3->f1)->kind != Cyc_Absyn_MemKind)goto _LLE9;_tmp1B4=(_tmp1B3->f1)->aliasqual;}}_LLE8:
# 762
{const char*_tmp3C6;void*_tmp3C5[3];struct Cyc_String_pa_PrintArg_struct _tmp3C4;struct Cyc_String_pa_PrintArg_struct _tmp3C3;struct Cyc_Absyn_Kind*_tmp3C2;struct Cyc_String_pa_PrintArg_struct _tmp3C1;(_tmp3C1.tag=0,((_tmp3C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 764
Cyc_Absynpp_kind2string(((_tmp3C2=_cycalloc_atomic(sizeof(*_tmp3C2)),((_tmp3C2->kind=Cyc_Absyn_MemKind,((_tmp3C2->aliasqual=_tmp1B4,_tmp3C2)))))))),((_tmp3C3.tag=0,((_tmp3C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3C4.tag=0,((_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3C5[0]=& _tmp3C4,((_tmp3C5[1]=& _tmp3C3,((_tmp3C5[2]=& _tmp3C1,Cyc_Tcutil_terr(loc,((_tmp3C6="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3C6,sizeof(char),57))),_tag_dyneither(_tmp3C5,sizeof(void*),3)))))))))))))))))));}goto _LLD4;_LLE9:;_LLEA:
 goto _LLD4;_LLD4:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 771
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 780
{struct Cyc_List_List*_tmp1C8=fields;for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct Cyc_Absyn_Datatypefield*_tmp1C9=(struct Cyc_Absyn_Datatypefield*)_tmp1C8->hd;
# 783
{struct Cyc_List_List*typs=_tmp1C9->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp1C9->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple16*)typs->hd)).f2);{
# 786
struct _RegionHandle _tmp1CA=_new_region("temp");struct _RegionHandle*temp=& _tmp1CA;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2)){
const char*_tmp3CA;void*_tmp3C9[1];struct Cyc_String_pa_PrintArg_struct _tmp3C8;(_tmp3C8.tag=0,((_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 790
Cyc_Absynpp_qvar2string(_tmp1C9->name)),((_tmp3C9[0]=& _tmp3C8,Cyc_Tcutil_terr(_tmp1C9->loc,((_tmp3CA="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3CA,sizeof(char),51))),_tag_dyneither(_tmp3C9,sizeof(void*),1)))))));}
# 792
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp1C9->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 786
;_pop_region(temp);};}}{
# 797
union Cyc_Absyn_Nmspace _stmttmp1C=(*_tmp1C9->name).f1;union Cyc_Absyn_Nmspace _tmp1CE=_stmttmp1C;_LLEC: if((_tmp1CE.Rel_n).tag != 1)goto _LLEE;if((struct Cyc_List_List*)(_tmp1CE.Rel_n).val != 0)goto _LLEE;_LLED:
# 799
 if(is_extensible)
(*_tmp1C9->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 802
(*_tmp1C9->name).f1=(*name).f1;}
goto _LLEB;_LLEE: if((_tmp1CE.Rel_n).tag != 1)goto _LLF0;_LLEF:
# 805
{const char*_tmp3CE;void*_tmp3CD[1];struct Cyc_String_pa_PrintArg_struct _tmp3CC;(_tmp3CC.tag=0,((_tmp3CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1C9->name)),((_tmp3CD[0]=& _tmp3CC,Cyc_Tcutil_terr(_tmp1C9->loc,((_tmp3CE="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3CE,sizeof(char),58))),_tag_dyneither(_tmp3CD,sizeof(void*),1)))))));}
goto _LLEB;_LLF0: if((_tmp1CE.C_n).tag != 3)goto _LLF2;_LLF1:
 goto _LLEB;_LLF2: if((_tmp1CE.Abs_n).tag != 2)goto _LLF4;_LLF3:
 goto _LLEB;_LLF4: if((_tmp1CE.Loc_n).tag != 4)goto _LLEB;_LLF5: {
const char*_tmp3D1;void*_tmp3D0;(_tmp3D0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3D1="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3D1,sizeof(char),24))),_tag_dyneither(_tmp3D0,sizeof(void*),0)));}_LLEB:;};}}{
# 814
struct Cyc_List_List*fields2;
if(is_extensible){
# 817
int _tmp1D4=1;
struct Cyc_List_List*_tmp1D5=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1D4,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1D4)
fields2=_tmp1D5;else{
# 822
fields2=0;}}else{
# 824
struct _RegionHandle _tmp1D6=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1D6;_push_region(uprev_rgn);
# 826
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1D7=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1D7->name).f2)){
const char*_tmp3D6;void*_tmp3D5[2];struct Cyc_String_pa_PrintArg_struct _tmp3D4;struct Cyc_String_pa_PrintArg_struct _tmp3D3;(_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3D4.tag=0,((_tmp3D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D7->name).f2),((_tmp3D5[0]=& _tmp3D4,((_tmp3D5[1]=& _tmp3D3,Cyc_Tcutil_terr(_tmp1D7->loc,((_tmp3D6="duplicate field name %s in %s",_tag_dyneither(_tmp3D6,sizeof(char),30))),_tag_dyneither(_tmp3D5,sizeof(void*),2)))))))))))));}else{
# 832
struct Cyc_List_List*_tmp3D7;prev_fields=((_tmp3D7=_region_malloc(uprev_rgn,sizeof(*_tmp3D7)),((_tmp3D7->hd=(*_tmp1D7->name).f2,((_tmp3D7->tl=prev_fields,_tmp3D7))))));}
# 834
if(_tmp1D7->sc != Cyc_Absyn_Public){
{const char*_tmp3DB;void*_tmp3DA[1];struct Cyc_String_pa_PrintArg_struct _tmp3D9;(_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D7->name).f2),((_tmp3DA[0]=& _tmp3D9,Cyc_Tcutil_warn(loc,((_tmp3DB="ignoring scope of field %s",_tag_dyneither(_tmp3DB,sizeof(char),27))),_tag_dyneither(_tmp3DA,sizeof(void*),1)))))));}
_tmp1D7->sc=Cyc_Absyn_Public;}}}
# 839
fields2=fields;}
# 826
;_pop_region(uprev_rgn);}
# 845
{struct Cyc_List_List*_tmp1E0=fields;for(0;_tmp1E0 != 0;_tmp1E0=_tmp1E0->tl){
struct Cyc_Absyn_Datatypefield*_tmp1E1=(struct Cyc_Absyn_Datatypefield*)_tmp1E0->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3E1;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3E0;struct _tuple14*_tmp3DF;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1E1->name).f2,(
(_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->f1=(void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E0.tag=2,((_tmp3E0.f1=tudres,((_tmp3E0.f2=_tmp1E1,_tmp3E0)))))),_tmp3E1)))),((_tmp3DF->f2=1,_tmp3DF)))))));}}
# 851
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 854
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp3E3;const char*_tmp3E2;struct _dyneither_ptr obj=tud->is_extensible?(_tmp3E3="@extensible datatype",_tag_dyneither(_tmp3E3,sizeof(char),21)):((_tmp3E2="datatype",_tag_dyneither(_tmp3E2,sizeof(char),9)));
# 861
struct Cyc_List_List*tvs=tud->tvs;
# 864
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 866
Cyc_Tcutil_add_tvar_identities(tvs);{
# 871
struct _RegionHandle _tmp1E5=_new_region("temp");struct _RegionHandle*temp=& _tmp1E5;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 881 "tc.cyc"
{struct _handler_cons _tmp1E6;_push_handler(& _tmp1E6);{int _tmp1E8=0;if(setjmp(_tmp1E6.handler))_tmp1E8=1;if(!_tmp1E8){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp3E6;struct Cyc_Dict_Absent_exn_struct*_tmp3E5;(int)_throw((void*)((_tmp3E5=_cycalloc_atomic(sizeof(*_tmp3E5)),((_tmp3E5[0]=((_tmp3E6.tag=Cyc_Dict_Absent,_tmp3E6)),_tmp3E5)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 887
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 882
;_pop_handler();}else{void*_tmp1E7=(void*)_exn_thrown;void*_tmp1EC=_tmp1E7;void*_tmp1EE;_LLF7: {struct Cyc_Dict_Absent_exn_struct*_tmp1ED=(struct Cyc_Dict_Absent_exn_struct*)_tmp1EC;if(_tmp1ED->tag != Cyc_Dict_Absent)goto _LLF9;}_LLF8:
# 891
{union Cyc_Absyn_Nmspace _stmttmp1D=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp1EF=_stmttmp1D;_LLFC: if((_tmp1EF.Rel_n).tag != 1)goto _LLFE;if((struct Cyc_List_List*)(_tmp1EF.Rel_n).val != 0)goto _LLFE;_LLFD:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLFB;_LLFE:;_LLFF:
 goto _LLFB;_LLFB:;}{
# 895
struct Cyc_Absyn_Datatypedecl***_tmp1F0=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp3E7;tud_opt=(unsigned int)_tmp1F0?(_tmp3E7=_region_malloc(temp,sizeof(*_tmp3E7)),((_tmp3E7[0]=*_tmp1F0,_tmp3E7))): 0;}
goto _LLF6;};_LLF9: _tmp1EE=_tmp1EC;_LLFA:(void)_throw(_tmp1EE);_LLF6:;}};}{
# 902
struct _tuple17 _tmp3E8;struct _tuple17 _stmttmp1E=(_tmp3E8.f1=tud->fields,((_tmp3E8.f2=tud_opt,_tmp3E8)));struct _tuple17 _tmp1F2=_stmttmp1E;struct Cyc_List_List**_tmp1F3;struct Cyc_List_List**_tmp1F4;struct Cyc_Absyn_Datatypedecl**_tmp1F5;struct Cyc_Absyn_Datatypedecl**_tmp1F6;_LL101: if(_tmp1F2.f1 != 0)goto _LL103;if(_tmp1F2.f2 != 0)goto _LL103;_LL102:
# 905
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp3E9;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=tud,_tmp3E9)))));}
goto _LL100;_LL103: if(_tmp1F2.f1 == 0)goto _LL105;_tmp1F3=(struct Cyc_List_List**)&(_tmp1F2.f1)->v;if(_tmp1F2.f2 != 0)goto _LL105;_LL104: {
# 910
struct Cyc_Absyn_Datatypedecl*_tmp3EC;struct Cyc_Absyn_Datatypedecl**_tmp3EB;struct Cyc_Absyn_Datatypedecl**_tmp1F8=(_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC->sc=Cyc_Absyn_Extern,((_tmp3EC->name=tud->name,((_tmp3EC->tvs=tvs,((_tmp3EC->fields=0,((_tmp3EC->is_extensible=tud->is_extensible,_tmp3EC)))))))))))),_tmp3EB)));
# 912
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1F8);
# 915
*_tmp1F3=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1F3,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1F8=tud;
goto _LL100;}_LL105: if(_tmp1F2.f1 == 0)goto _LL107;_tmp1F4=(struct Cyc_List_List**)&(_tmp1F2.f1)->v;if(_tmp1F2.f2 == 0)goto _LL107;_tmp1F5=*_tmp1F2.f2;_LL106: {
# 920
struct Cyc_Absyn_Datatypedecl*_tmp1FB=*_tmp1F5;
# 923
if((!tud->is_extensible  && (unsigned int)_tmp1FB) && _tmp1FB->is_extensible)
tud->is_extensible=1;
# 926
{struct Cyc_Absyn_Datatypedecl*_tmp3ED;*_tmp1F5=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->sc=Cyc_Absyn_Extern,((_tmp3ED->name=tud->name,((_tmp3ED->tvs=tvs,((_tmp3ED->fields=0,((_tmp3ED->is_extensible=tud->is_extensible,_tmp3ED))))))))))));}
# 930
*_tmp1F4=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1F4,tvs,tud);
# 934
*_tmp1F5=_tmp1FB;
_tmp1F6=_tmp1F5;goto _LL108;}_LL107: if(_tmp1F2.f1 != 0)goto _LL100;if(_tmp1F2.f2 == 0)goto _LL100;_tmp1F6=*_tmp1F2.f2;_LL108: {
# 938
struct Cyc_Absyn_Datatypedecl*_tmp1FD=Cyc_Tcdecl_merge_datatypedecl(*_tmp1F6,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1FD == 0){
_npop_handler(0);return;}else{
# 943
*_tmp1F6=_tmp1FD;
goto _LL100;}}_LL100:;};}
# 872 "tc.cyc"
;_pop_region(temp);};}
# 950 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 957
{union Cyc_Absyn_Nmspace _stmttmp1F=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp201=_stmttmp1F;_LL10A: if((_tmp201.Rel_n).tag != 1)goto _LL10C;if((struct Cyc_List_List*)(_tmp201.Rel_n).val != 0)goto _LL10C;_LL10B:
# 960
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LL109;_LL10C:;_LL10D:
 goto _LL109;_LL109:;}
# 966
if(ed->fields != 0){struct _RegionHandle _tmp202=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp202;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp203=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 972
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp203->name).f2)){
const char*_tmp3F1;void*_tmp3F0[1];struct Cyc_String_pa_PrintArg_struct _tmp3EF;(_tmp3EF.tag=0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp203->name).f2),((_tmp3F0[0]=& _tmp3EF,Cyc_Tcutil_terr(_tmp203->loc,((_tmp3F1="duplicate enum constructor %s",_tag_dyneither(_tmp3F1,sizeof(char),30))),_tag_dyneither(_tmp3F0,sizeof(void*),1)))))));}else{
# 975
struct Cyc_List_List*_tmp3F2;prev_fields=((_tmp3F2=_region_malloc(uprev_rgn,sizeof(*_tmp3F2)),((_tmp3F2->hd=(*_tmp203->name).f2,((_tmp3F2->tl=prev_fields,_tmp3F2))))));}
# 977
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp203->name).f2)!= 0){
const char*_tmp3F6;void*_tmp3F5[1];struct Cyc_String_pa_PrintArg_struct _tmp3F4;(_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp203->name).f2),((_tmp3F5[0]=& _tmp3F4,Cyc_Tcutil_terr(_tmp203->loc,((_tmp3F6="enum field name %s shadows global name",_tag_dyneither(_tmp3F6,sizeof(char),39))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}
# 980
if(_tmp203->tag == 0){
_tmp203->tag=Cyc_Absyn_uint_exp(tag_count,_tmp203->loc);
++ tag_count;}else{
# 985
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp203->tag))){
struct _tuple11 _stmttmp20=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp203->tag));unsigned int _tmp20C;int _tmp20D;struct _tuple11 _tmp20B=_stmttmp20;_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;
if(_tmp20D)tag_count=_tmp20C + 1;}}
# 989
(*_tmp203->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 967
;_pop_region(uprev_rgn);}
# 994
{struct _handler_cons _tmp20E;_push_handler(& _tmp20E);{int _tmp210=0;if(setjmp(_tmp20E.handler))_tmp210=1;if(!_tmp210){
{struct Cyc_Absyn_Enumdecl**_tmp211=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp212=Cyc_Tcdecl_merge_enumdecl(*_tmp211,ed,loc,Cyc_Tc_tc_msg);
if(_tmp212 == 0){_npop_handler(0);return;}
*_tmp211=_tmp212;}
# 995
;_pop_handler();}else{void*_tmp20F=(void*)_exn_thrown;void*_tmp214=_tmp20F;void*_tmp216;_LL10F: {struct Cyc_Dict_Absent_exn_struct*_tmp215=(struct Cyc_Dict_Absent_exn_struct*)_tmp214;if(_tmp215->tag != Cyc_Dict_Absent)goto _LL111;}_LL110: {
# 1000
struct Cyc_Absyn_Enumdecl**_tmp3F7;struct Cyc_Absyn_Enumdecl**_tmp217=(_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7[0]=ed,_tmp3F7)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp217);
goto _LL10E;}_LL111: _tmp216=_tmp214;_LL112:(void)_throw(_tmp216);_LL10E:;}};}
# 1004
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp219=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3FD;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp3FC;struct _tuple14*_tmp3FB;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp219->name).f2,(
(_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->f1=(void*)((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=((_tmp3FC.tag=3,((_tmp3FC.f1=ed,((_tmp3FC.f2=_tmp219,_tmp3FC)))))),_tmp3FD)))),((_tmp3FB->f2=1,_tmp3FB)))))));}}
# 1013
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp21D=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp21D->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp21D->tag))){
const char*_tmp402;void*_tmp401[2];struct Cyc_String_pa_PrintArg_struct _tmp400;struct Cyc_String_pa_PrintArg_struct _tmp3FF;(_tmp3FF.tag=0,((_tmp3FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp21D->name).f2),((_tmp400.tag=0,((_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp401[0]=& _tmp400,((_tmp401[1]=& _tmp3FF,Cyc_Tcutil_terr(loc,((_tmp402="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp402,sizeof(char),46))),_tag_dyneither(_tmp401,sizeof(void*),2)))))))))))));}}}}
# 1023
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL113:
# 1026
 if(!in_include){
const char*_tmp405;void*_tmp404;(_tmp404=0,Cyc_Tcutil_warn(loc,((_tmp405="static declaration nested within extern \"C\"",_tag_dyneither(_tmp405,sizeof(char),44))),_tag_dyneither(_tmp404,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL114:
# 1030
{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_warn(loc,((_tmp408="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp408,sizeof(char),46))),_tag_dyneither(_tmp407,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL115:
 goto _LL116;case Cyc_Absyn_Register: _LL116:
 goto _LL117;case Cyc_Absyn_Extern: _LL117:
 return 1;case Cyc_Absyn_ExternC: _LL118:
# 1036
{const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_Tcutil_warn(loc,((_tmp40B="nested extern \"C\" declaration",_tag_dyneither(_tmp40B,sizeof(char),30))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
return 1;}}
# 1041
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _stmttmp21=*exp;unsigned int _tmp229;struct _tuple0*_tmp22A;struct _tuple12 _tmp228=_stmttmp21;_tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;{
union Cyc_Absyn_Nmspace _tmp22C;struct _dyneither_ptr*_tmp22D;struct _tuple0*_tmp22B=_tmp22A;_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;{
union Cyc_Absyn_Nmspace _tmp22E=_tmp22C;_LL11B: if((_tmp22E.Rel_n).tag != 1)goto _LL11D;if((struct Cyc_List_List*)(_tmp22E.Rel_n).val != 0)goto _LL11D;_LL11C:
# 1047
(*_tmp22A).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL11A;_LL11D:;_LL11E:
 goto _LL11A;_LL11A:;};};}
# 1058
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1065
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1068
struct Cyc_List_List*_tmp22F=ds0;for(0;_tmp22F != 0;(last=_tmp22F,_tmp22F=_tmp22F->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp22F->hd;
unsigned int loc=d->loc;
# 1072
void*_stmttmp22=d->r;void*_tmp230=_stmttmp22;struct Cyc_Absyn_Vardecl*_tmp236;struct Cyc_Absyn_Fndecl*_tmp238;struct Cyc_Absyn_Typedefdecl*_tmp23A;struct Cyc_Absyn_Aggrdecl*_tmp23C;struct Cyc_Absyn_Datatypedecl*_tmp23E;struct Cyc_Absyn_Enumdecl*_tmp240;struct _dyneither_ptr*_tmp244;struct Cyc_List_List*_tmp245;union Cyc_Absyn_Nmspace _tmp247;struct _dyneither_ptr*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_List_List*_tmp24B;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24E;_LL120: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp231=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp231->tag != 2)goto _LL122;}_LL121:
 goto _LL123;_LL122: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp232=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp232->tag != 3)goto _LL124;}_LL123:
# 1075
{const char*_tmp40E;void*_tmp40D;(_tmp40D=0,Cyc_Tcutil_terr(loc,((_tmp40E="top level let-declarations are not implemented",_tag_dyneither(_tmp40E,sizeof(char),47))),_tag_dyneither(_tmp40D,sizeof(void*),0)));}
goto _LL11F;_LL124: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp233=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp233->tag != 4)goto _LL126;}_LL125:
# 1078
{const char*_tmp411;void*_tmp410;(_tmp410=0,Cyc_Tcutil_terr(loc,((_tmp411="top level region declarations are not implemented",_tag_dyneither(_tmp411,sizeof(char),50))),_tag_dyneither(_tmp410,sizeof(void*),0)));}
goto _LL11F;_LL126: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp234=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp234->tag != 5)goto _LL128;}_LL127:
# 1081
{const char*_tmp414;void*_tmp413;(_tmp413=0,Cyc_Tcutil_terr(loc,((_tmp414="top level alias declarations are not implemented",_tag_dyneither(_tmp414,sizeof(char),49))),_tag_dyneither(_tmp413,sizeof(void*),0)));}
goto _LL11F;_LL128: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp235=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp235->tag != 0)goto _LL12A;else{_tmp236=_tmp235->f1;}}_LL129:
# 1084
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp236->sc,in_externCinclude))
_tmp236->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp236,check_var_init,in_externCinclude,exports);
goto _LL11F;_LL12A: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp237=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp237->tag != 1)goto _LL12C;else{_tmp238=_tmp237->f1;}}_LL12B:
# 1089
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp238->sc,in_externCinclude))
_tmp238->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp238,in_externCinclude,exports);
goto _LL11F;_LL12C: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp239=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp239->tag != 9)goto _LL12E;else{_tmp23A=_tmp239->f1;}}_LL12D:
# 1094
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp23A);
goto _LL11F;_LL12E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp23B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp23B->tag != 6)goto _LL130;else{_tmp23C=_tmp23B->f1;}}_LL12F:
# 1097
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23C->sc,in_externCinclude))
_tmp23C->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp23C);
goto _LL11F;_LL130: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp23D=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp23D->tag != 7)goto _LL132;else{_tmp23E=_tmp23D->f1;}}_LL131:
# 1102
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23E->sc,in_externCinclude))
_tmp23E->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp23E);
goto _LL11F;_LL132: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp23F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp23F->tag != 8)goto _LL134;else{_tmp240=_tmp23F->f1;}}_LL133:
# 1107
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp240->sc,in_externCinclude))
_tmp240->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp240);
goto _LL11F;_LL134: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp241=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp241->tag != 14)goto _LL136;}_LL135:
# 1112
{const char*_tmp417;void*_tmp416;(_tmp416=0,Cyc_Tcutil_warn(d->loc,((_tmp417="spurious __cyclone_port_on__",_tag_dyneither(_tmp417,sizeof(char),29))),_tag_dyneither(_tmp416,sizeof(void*),0)));}
goto _LL11F;_LL136: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp242=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp242->tag != 15)goto _LL138;}_LL137:
# 1115
 goto _LL11F;_LL138: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp243=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp243->tag != 10)goto _LL13A;else{_tmp244=_tmp243->f1;_tmp245=_tmp243->f2;}}_LL139: {
# 1118
struct Cyc_List_List*_tmp257=te->ns;
struct Cyc_List_List*_tmp418;struct Cyc_List_List*_tmp258=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp257,((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=_tmp244,((_tmp418->tl=0,_tmp418)))))));
# 1122
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp244)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp244);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp258,Cyc_Tcenv_empty_genv(grgn));}
# 1127
te->ns=_tmp258;
Cyc_Tc_tc_decls(te,_tmp245,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp257;
goto _LL11F;}_LL13A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp246=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp246->tag != 11)goto _LL13C;else{_tmp247=(_tmp246->f1)->f1;_tmp248=(_tmp246->f1)->f2;_tmp249=_tmp246->f2;}}_LL13B: {
# 1134
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp25A=_tmp247;struct _dyneither_ptr*_tmp25B;struct Cyc_List_List*_tmp25C;struct _dyneither_ptr*_tmp25D;struct Cyc_List_List*_tmp25E;struct _dyneither_ptr*_tmp25F;struct Cyc_List_List*_tmp260;_LL141: if((_tmp25A.Loc_n).tag != 4)goto _LL143;_LL142:
 goto _LL144;_LL143: if((_tmp25A.Rel_n).tag != 1)goto _LL145;if((struct Cyc_List_List*)(_tmp25A.Rel_n).val != 0)goto _LL145;_LL144:
 goto _LL146;_LL145: if((_tmp25A.C_n).tag != 3)goto _LL147;if((struct Cyc_List_List*)(_tmp25A.C_n).val != 0)goto _LL147;_LL146:
 goto _LL148;_LL147: if((_tmp25A.Abs_n).tag != 2)goto _LL149;if((struct Cyc_List_List*)(_tmp25A.Abs_n).val != 0)goto _LL149;_LL148:
# 1141
 first=_tmp248;rest=0;goto _LL140;_LL149: if((_tmp25A.Rel_n).tag != 1)goto _LL14B;if((struct Cyc_List_List*)(_tmp25A.Rel_n).val == 0)goto _LL14B;_tmp25B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp25A.Rel_n).val)->hd;_tmp25C=((struct Cyc_List_List*)(_tmp25A.Rel_n).val)->tl;_LL14A:
 _tmp25D=_tmp25B;_tmp25E=_tmp25C;goto _LL14C;_LL14B: if((_tmp25A.C_n).tag != 3)goto _LL14D;if((struct Cyc_List_List*)(_tmp25A.C_n).val == 0)goto _LL14D;_tmp25D=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp25A.C_n).val)->hd;_tmp25E=((struct Cyc_List_List*)(_tmp25A.C_n).val)->tl;_LL14C:
 _tmp25F=_tmp25D;_tmp260=_tmp25E;goto _LL14E;_LL14D: if((_tmp25A.Abs_n).tag != 2)goto _LL140;if((struct Cyc_List_List*)(_tmp25A.Abs_n).val == 0)goto _LL140;_tmp25F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp25A.Abs_n).val)->hd;_tmp260=((struct Cyc_List_List*)(_tmp25A.Abs_n).val)->tl;_LL14E:
# 1145
 first=_tmp25F;{struct Cyc_List_List*_tmp419;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp260,((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->hd=_tmp248,((_tmp419->tl=0,_tmp419)))))));}goto _LL140;_LL140:;}{
# 1147
struct Cyc_List_List*_tmp262=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp41A;ge->availables=((_tmp41A=_cycalloc(sizeof(*_tmp41A)),((_tmp41A->hd=_tmp262,((_tmp41A->tl=ge->availables,_tmp41A))))));}
Cyc_Tc_tc_decls(te,_tmp249,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL11F;};}_LL13C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp24A=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp24A->tag != 12)goto _LL13E;else{_tmp24B=_tmp24A->f1;}}_LL13D:
# 1154
 Cyc_Tc_tc_decls(te,_tmp24B,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL11F;_LL13E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp24C=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp230;if(_tmp24C->tag != 13)goto _LL11F;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;}}_LL13F:
# 1158
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp24E);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp24E,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp24D,1,1,check_var_init,grgn,& newexs);
# 1163
for(0;_tmp24E != 0;_tmp24E=_tmp24E->tl){
struct _tuple12*_tmp264=(struct _tuple12*)_tmp24E->hd;
if(!(*_tmp264).f3){
const char*_tmp41E;void*_tmp41D[1];struct Cyc_String_pa_PrintArg_struct _tmp41C;(_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp264).f2)),((_tmp41D[0]=& _tmp41C,Cyc_Tcutil_warn((*_tmp264).f1,((_tmp41E="%s is exported but not defined",_tag_dyneither(_tmp41E,sizeof(char),31))),_tag_dyneither(_tmp41D,sizeof(void*),1)))))));}}
# 1169
goto _LL11F;};_LL11F:;}}
# 1174
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1176
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1193 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1195
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1197
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple18{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1201
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_stmttmp23=d->r;void*_tmp268=_stmttmp23;struct Cyc_Absyn_Vardecl*_tmp26A;struct Cyc_List_List**_tmp26C;struct Cyc_List_List**_tmp26E;struct Cyc_List_List**_tmp270;struct Cyc_List_List**_tmp272;_LL150: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp269=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp268;if(_tmp269->tag != 0)goto _LL152;else{_tmp26A=_tmp269->f1;}}_LL151:
# 1206
 if((env->in_cinclude  || _tmp26A->sc != Cyc_Absyn_Extern  && _tmp26A->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp26A->name,Cyc_Absyn_uniquergn_qvar))
# 1209
return 1;{
# 1211
struct _tuple0*_stmttmp24=_tmp26A->name;union Cyc_Absyn_Nmspace _tmp274;struct _dyneither_ptr*_tmp275;struct _tuple0*_tmp273=_stmttmp24;_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp276=_tmp274;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp279;_LL15D: if((_tmp276.Loc_n).tag != 4)goto _LL15F;_LL15E:
 ns=0;goto _LL15C;_LL15F: if((_tmp276.Rel_n).tag != 1)goto _LL161;_tmp277=(struct Cyc_List_List*)(_tmp276.Rel_n).val;_LL160:
 ns=_tmp277;goto _LL15C;_LL161: if((_tmp276.C_n).tag != 3)goto _LL163;_tmp278=(struct Cyc_List_List*)(_tmp276.C_n).val;_LL162:
 ns=_tmp278;goto _LL15C;_LL163: if((_tmp276.Abs_n).tag != 2)goto _LL15C;_tmp279=(struct Cyc_List_List*)(_tmp276.Abs_n).val;_LL164:
 ns=_tmp279;goto _LL15C;_LL15C:;}{
# 1219
struct _tuple18*_tmp27A=((struct _tuple18*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp27B=(*_tmp27A).f1;
int _tmp27C=(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp27B->ordinaries,_tmp275)).f2;
if(!_tmp27C)
(*_tmp27A).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp27A).f2,_tmp275);
# 1227
return _tmp27C;};};};_LL152: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp26B=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp268;if(_tmp26B->tag != 12)goto _LL154;else{_tmp26C=(struct Cyc_List_List**)& _tmp26B->f1;}}_LL153:
 _tmp26E=_tmp26C;goto _LL155;_LL154: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp26D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp268;if(_tmp26D->tag != 11)goto _LL156;else{_tmp26E=(struct Cyc_List_List**)& _tmp26D->f2;}}_LL155:
 _tmp270=_tmp26E;goto _LL157;_LL156: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp26F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp268;if(_tmp26F->tag != 10)goto _LL158;else{_tmp270=(struct Cyc_List_List**)& _tmp26F->f2;}}_LL157:
# 1231
*_tmp270=Cyc_Tc_treeshake_f(env,*_tmp270);
return 1;_LL158: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp271=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp268;if(_tmp271->tag != 13)goto _LL15A;else{_tmp272=(struct Cyc_List_List**)& _tmp271->f1;}}_LL159: {
# 1234
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp272=Cyc_Tc_treeshake_f(env,*_tmp272);
env->in_cinclude=in_cinclude;
return 1;}_LL15A:;_LL15B:
 return 1;_LL14F:;}
# 1243
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1247
static struct _tuple18*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1249
struct _tuple18*_tmp41F;return(_tmp41F=_region_malloc(rgn,sizeof(*_tmp41F)),((_tmp41F->f1=ge,((_tmp41F->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp41F)))));}struct _tuple19{struct Cyc_List_List*f1;struct _tuple18*f2;};
# 1252
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp27E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp27E;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp420;struct Cyc_Tc_TreeshakeEnv _tmp27F=(_tmp420.rgn=rgn,((_tmp420.in_cinclude=0,((_tmp420.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple18*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp420)))));
struct Cyc_List_List*_tmp280=Cyc_Tc_treeshake_f(& _tmp27F,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp27F.nsdict)){
struct Cyc_List_List*_tmp281=_tmp280;_npop_handler(0);return _tmp281;}{
# 1260
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp27F.nsdict);
struct _tuple19 _tmp282=*((struct _tuple19*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp27F.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple19*))Cyc_Iter_next)(iter,& _tmp282)){
struct Cyc_Tcenv_Genv*_tmp284;struct Cyc_Set_Set*_tmp285;struct _tuple19 _tmp283=_tmp282;_tmp284=(_tmp283.f2)->f1;_tmp285=(_tmp283.f2)->f2;
_tmp284->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple14*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp284->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple14*y))Cyc_Tc_treeshake_remove_f,_tmp285,_tmp284->ordinaries);}{
# 1266
struct Cyc_List_List*_tmp286=_tmp280;_npop_handler(0);return _tmp286;};};}
# 1254
;_pop_region(rgn);}
