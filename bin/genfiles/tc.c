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
typedef void*Cyc_sarg_t;
# 174 "cycboot.h"
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 302
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 377
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
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
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 962
void*Cyc_Absyn_string_typ(void*rgn);
# 963
void*Cyc_Absyn_const_string_typ(void*rgn);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1066
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1152
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
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 115 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
# 116
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 121
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 126
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 182
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 210
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
# 211
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 91
int Cyc_Tcutil_is_function_type(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 174
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 204 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 206
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 214
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 218
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 295
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 316
int Cyc_Tcutil_supports_default(void*);
# 327
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 331
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 33
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
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
# 42
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
# 43
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
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";
# 46 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
# 47
static struct _dyneither_ptr*Cyc_Tc_tc_msg=(struct _dyneither_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
# 50
for(0;exports != 0;exports=exports->tl){
# 51
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
# 52
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 55
(*_tmp1).f3=1;
# 56
return 1;}}
# 59
return 0;}
# 62
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
# 63
struct _RegionHandle _tmp2=_new_region("temp");struct _RegionHandle*temp=& _tmp2;_push_region(temp);
# 64
{struct Cyc_List_List*init_params=0;
# 65
struct Cyc_List_List*nolive_unique_params=0;
# 66
struct Cyc_List_List*noconsume_params=0;
# 67
void*_tmp3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp5;struct Cyc_List_List*_tmp6;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3;if(_tmp4->tag != 9)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=_tmp5.attributes;}}_LL2:
# 69
 for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
# 70
void*_tmp7=(void*)_tmp6->hd;int _tmp9;int _tmpB;int _tmpD;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp7;if(_tmp8->tag != 20)goto _LL8;else{_tmp9=_tmp8->f1;}}_LL7:
# 72
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmp9)){
# 73
struct Cyc_Int_pa_PrintArg_struct _tmp2BC;void*_tmp2BB[1];const char*_tmp2BA;void*_tmp2B9;(_tmp2B9=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2BC.tag=1,((_tmp2BC.f1=(unsigned long)_tmp9,((_tmp2BB[0]=& _tmp2BC,Cyc_rprintf(temp,((_tmp2BA="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp2BA,sizeof(char),73))),_tag_dyneither(_tmp2BB,sizeof(void*),1)))))))),_tag_dyneither(_tmp2B9,sizeof(void*),0)));}
# 76
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmp9)){
# 77
struct Cyc_Int_pa_PrintArg_struct _tmp2C4;void*_tmp2C3[1];const char*_tmp2C2;void*_tmp2C1;(_tmp2C1=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2C4.tag=1,((_tmp2C4.f1=(unsigned long)_tmp9,((_tmp2C3[0]=& _tmp2C4,Cyc_rprintf(temp,((_tmp2C2="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2C2,sizeof(char),70))),_tag_dyneither(_tmp2C3,sizeof(void*),1)))))))),_tag_dyneither(_tmp2C1,sizeof(void*),0)));}
# 79
{struct Cyc_List_List*_tmp2C5;init_params=((_tmp2C5=_region_malloc(temp,sizeof(*_tmp2C5)),((_tmp2C5->hd=(void*)_tmp9,((_tmp2C5->tl=init_params,_tmp2C5))))));}goto _LL5;_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp7;if(_tmpA->tag != 21)goto _LLA;else{_tmpB=_tmpA->f1;}}_LL9:
# 81
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpB)){
# 82
struct Cyc_Int_pa_PrintArg_struct _tmp2CD;void*_tmp2CC[1];const char*_tmp2CB;void*_tmp2CA;(_tmp2CA=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2CD.tag=1,((_tmp2CD.f1=(unsigned long)_tmpB,((_tmp2CC[0]=& _tmp2CD,Cyc_rprintf(temp,((_tmp2CB="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp2CB,sizeof(char),73))),_tag_dyneither(_tmp2CC,sizeof(void*),1)))))))),_tag_dyneither(_tmp2CA,sizeof(void*),0)));}
# 85
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmpB)){
# 86
struct Cyc_Int_pa_PrintArg_struct _tmp2D5;void*_tmp2D4[1];const char*_tmp2D3;void*_tmp2D2;(_tmp2D2=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2D5.tag=1,((_tmp2D5.f1=(unsigned long)_tmpB,((_tmp2D4[0]=& _tmp2D5,Cyc_rprintf(temp,((_tmp2D3="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2D3,sizeof(char),71))),_tag_dyneither(_tmp2D4,sizeof(void*),1)))))))),_tag_dyneither(_tmp2D2,sizeof(void*),0)));}
# 88
{struct Cyc_List_List*_tmp2D6;nolive_unique_params=((_tmp2D6=_region_malloc(temp,sizeof(*_tmp2D6)),((_tmp2D6->hd=(void*)_tmpB,((_tmp2D6->tl=nolive_unique_params,_tmp2D6))))));}goto _LL5;_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpC=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp7;if(_tmpC->tag != 22)goto _LLC;else{_tmpD=_tmpC->f1;}}_LLB:
# 90
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpD)){
# 91
struct Cyc_Int_pa_PrintArg_struct _tmp2DE;void*_tmp2DD[1];const char*_tmp2DC;void*_tmp2DB;(_tmp2DB=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2DE.tag=1,((_tmp2DE.f1=(unsigned long)_tmpD,((_tmp2DD[0]=& _tmp2DE,Cyc_rprintf(temp,((_tmp2DC="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2DC,sizeof(char),70))),_tag_dyneither(_tmp2DD,sizeof(void*),1)))))))),_tag_dyneither(_tmp2DB,sizeof(void*),0)));}
# 94
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmpD)){
# 95
struct Cyc_Int_pa_PrintArg_struct _tmp2E6;void*_tmp2E5[1];const char*_tmp2E4;void*_tmp2E3;(_tmp2E3=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2E6.tag=1,((_tmp2E6.f1=(unsigned long)_tmpD,((_tmp2E5[0]=& _tmp2E6,Cyc_rprintf(temp,((_tmp2E4="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2E4,sizeof(char),71))),_tag_dyneither(_tmp2E5,sizeof(void*),1)))))))),_tag_dyneither(_tmp2E3,sizeof(void*),0)));}
# 97
{struct Cyc_List_List*_tmp2E7;noconsume_params=((_tmp2E7=_region_malloc(temp,sizeof(*_tmp2E7)),((_tmp2E7->hd=(void*)_tmpD,((_tmp2E7->tl=noconsume_params,_tmp2E7))))));}goto _LL5;_LLC:;_LLD:
# 98
 goto _LL5;_LL5:;}
# 101
goto _LL0;_LL3:;_LL4: {
# 103
const char*_tmp2EA;void*_tmp2E9;(_tmp2E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2EA="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2EA,sizeof(char),34))),_tag_dyneither(_tmp2E9,sizeof(void*),0)));}_LL0:;}
# 64
;_pop_region(temp);}struct _tuple13{void*f1;int f2;};
# 108
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 112
struct Cyc_Absyn_Vardecl _tmp2C;enum Cyc_Absyn_Scope _tmp2D;struct _tuple0*_tmp2E;struct _tuple0 _tmp2F;union Cyc_Absyn_Nmspace _tmp30;struct _dyneither_ptr*_tmp31;void*_tmp32;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_List_List*_tmp34;struct Cyc_Absyn_Vardecl*_tmp2B=vd;_tmp2C=*_tmp2B;_tmp2D=_tmp2C.sc;_tmp2E=_tmp2C.name;_tmp2F=*_tmp2E;_tmp30=_tmp2F.f1;_tmp31=_tmp2F.f2;_tmp32=_tmp2C.type;_tmp33=_tmp2C.initializer;_tmp34=_tmp2C.attributes;
# 118
{union Cyc_Absyn_Nmspace _tmp35=_tmp30;struct Cyc_List_List*_tmp36;_LLF: if((_tmp35.Rel_n).tag != 1)goto _LL11;_tmp36=(struct Cyc_List_List*)(_tmp35.Rel_n).val;_LL10:
# 121
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
# 122
goto _LLE;_LL11:;_LL12:
# 123
 goto _LLE;_LLE:;}
# 127
{void*_tmp37=Cyc_Tcutil_compress(_tmp32);struct Cyc_Absyn_ArrayInfo _tmp39;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;struct Cyc_Absyn_Exp*_tmp3C;union Cyc_Absyn_Constraint*_tmp3D;unsigned int _tmp3E;_LL14:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 8)goto _LL16;else{_tmp39=_tmp38->f1;_tmp3A=_tmp39.elt_type;_tmp3B=_tmp39.tq;_tmp3C=_tmp39.num_elts;if(_tmp3C != 0)goto _LL16;_tmp3D=_tmp39.zero_term;_tmp3E=_tmp39.zt_loc;}}if(!(_tmp33 != 0))goto _LL16;_LL15:
# 129
{void*_tmp3F=_tmp33->r;union Cyc_Absyn_Cnst _tmp41;struct _dyneither_ptr _tmp42;union Cyc_Absyn_Cnst _tmp44;struct _dyneither_ptr _tmp45;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_List_List*_tmp49;struct Cyc_List_List*_tmp4B;_LL19: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp40=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F;if(_tmp40->tag != 0)goto _LL1B;else{_tmp41=_tmp40->f1;if((_tmp41.String_c).tag != 8)goto _LL1B;_tmp42=(struct _dyneither_ptr)(_tmp41.String_c).val;}}_LL1A:
# 131
 _tmp32=(vd->type=Cyc_Absyn_array_typ(_tmp3A,_tmp3B,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp42,sizeof(char)),0),_tmp3D,_tmp3E));
# 132
goto _LL18;_LL1B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp43=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3F;if(_tmp43->tag != 0)goto _LL1D;else{_tmp44=_tmp43->f1;if((_tmp44.Wstring_c).tag != 9)goto _LL1D;_tmp45=(struct _dyneither_ptr)(_tmp44.Wstring_c).val;}}_LL1C:
# 134
 _tmp32=(vd->type=Cyc_Absyn_array_typ(_tmp3A,_tmp3B,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0),_tmp3D,_tmp3E));
# 135
goto _LL18;_LL1D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp46=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F;if(_tmp46->tag != 26)goto _LL1F;else{_tmp47=_tmp46->f2;}}_LL1E:
# 138
 _tmp32=(vd->type=Cyc_Absyn_array_typ(_tmp3A,_tmp3B,(struct Cyc_Absyn_Exp*)_tmp47,_tmp3D,_tmp3E));
# 139
goto _LL18;_LL1F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp48=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp3F;if(_tmp48->tag != 34)goto _LL21;else{_tmp49=_tmp48->f2;}}_LL20:
# 140
 _tmp4B=_tmp49;goto _LL22;_LL21: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F;if(_tmp4A->tag != 25)goto _LL23;else{_tmp4B=_tmp4A->f1;}}_LL22:
# 142
 _tmp32=(vd->type=Cyc_Absyn_array_typ(_tmp3A,_tmp3B,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B),0),_tmp3D,_tmp3E));
# 143
goto _LL18;_LL23:;_LL24:
# 144
 goto _LL18;_LL18:;}
# 146
goto _LL13;_LL16:;_LL17:
# 147
 goto _LL13;_LL13:;}
# 150
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp32);
# 152
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp32);
# 155
{void*_tmp4C=Cyc_Tcutil_compress(_tmp32);_LL26: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C;if(_tmp4D->tag != 8)goto _LL28;}_LL27:
# 156
 vd->escapes=0;goto _LL25;_LL28:;_LL29:
# 157
 vd->escapes=1;goto _LL25;_LL25:;}
# 160
if(Cyc_Tcutil_is_function_type(_tmp32)){
# 161
_tmp34=Cyc_Tcutil_transfer_fn_type_atts(_tmp32,_tmp34);
# 162
Cyc_Tc_fnTypeAttsOK(te,_tmp32,loc);}
# 165
if(_tmp2D == Cyc_Absyn_Extern  || _tmp2D == Cyc_Absyn_ExternC){
# 166
if(_tmp33 != 0){
# 167
const char*_tmp2ED;void*_tmp2EC;(_tmp2EC=0,Cyc_Tcutil_terr(loc,((_tmp2ED="extern declaration should not have initializer",_tag_dyneither(_tmp2ED,sizeof(char),47))),_tag_dyneither(_tmp2EC,sizeof(void*),0)));}}else{
# 168
if(!Cyc_Tcutil_is_function_type(_tmp32)){
# 172
for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
# 173
void*_tmp50=(void*)_tmp34->hd;_LL2B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp51=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp50;if(_tmp51->tag != 6)goto _LL2D;}_LL2C:
# 174
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp52=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp50;if(_tmp52->tag != 8)goto _LL2F;}_LL2E:
# 177
 goto _LL30;_LL2F: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp53=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp50;if(_tmp53->tag != 9)goto _LL31;}_LL30:
# 178
 goto _LL32;_LL31: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp54=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp50;if(_tmp54->tag != 10)goto _LL33;}_LL32:
# 179
 goto _LL34;_LL33: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp55=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp50;if(_tmp55->tag != 11)goto _LL35;}_LL34:
# 180
 goto _LL36;_LL35: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp56=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp50;if(_tmp56->tag != 12)goto _LL37;}_LL36:
# 181
 goto _LL38;_LL37: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp57=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp50;if(_tmp57->tag != 13)goto _LL39;}_LL38:
# 182
 goto _LL3A;_LL39: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp58=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp50;if(_tmp58->tag != 14)goto _LL3B;}_LL3A:
# 183
 continue;_LL3B:;_LL3C:
# 185
{const char*_tmp2F2;void*_tmp2F1[2];struct Cyc_String_pa_PrintArg_struct _tmp2F0;struct Cyc_String_pa_PrintArg_struct _tmp2EF;(_tmp2EF.tag=0,((_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 186
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2F0.tag=0,((_tmp2F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp34->hd)),((_tmp2F1[0]=& _tmp2F0,((_tmp2F1[1]=& _tmp2EF,Cyc_Tcutil_terr(loc,((_tmp2F2="bad attribute %s for variable %s",_tag_dyneither(_tmp2F2,sizeof(char),33))),_tag_dyneither(_tmp2F1,sizeof(void*),2)))))))))))));}
# 187
goto _LL2A;_LL2A:;}
# 190
if(_tmp33 == 0  || in_cinclude){
# 191
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp32)){
# 192
const char*_tmp2F7;void*_tmp2F6[2];struct Cyc_String_pa_PrintArg_struct _tmp2F5;struct Cyc_String_pa_PrintArg_struct _tmp2F4;(_tmp2F4.tag=0,((_tmp2F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 193
Cyc_Absynpp_typ2string(_tmp32)),((_tmp2F5.tag=0,((_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2F6[0]=& _tmp2F5,((_tmp2F6[1]=& _tmp2F4,Cyc_Tcutil_terr(loc,((_tmp2F7="initializer required for variable %s of type %s",_tag_dyneither(_tmp2F7,sizeof(char),48))),_tag_dyneither(_tmp2F6,sizeof(void*),2)))))))))))));}}else{
# 195
struct Cyc_Absyn_Exp*_tmp61=(struct Cyc_Absyn_Exp*)_tmp33;
# 197
void*_tmp62=Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp32,_tmp61);
# 198
if(!Cyc_Tcutil_coerce_assign(te,_tmp61,_tmp32)){
# 199
{const char*_tmp2FD;void*_tmp2FC[3];struct Cyc_String_pa_PrintArg_struct _tmp2FB;struct Cyc_String_pa_PrintArg_struct _tmp2FA;struct Cyc_String_pa_PrintArg_struct _tmp2F9;(_tmp2F9.tag=0,((_tmp2F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 200
Cyc_Absynpp_typ2string(_tmp62)),((_tmp2FA.tag=0,((_tmp2FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp32)),((_tmp2FB.tag=0,((_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2FC[0]=& _tmp2FB,((_tmp2FC[1]=& _tmp2FA,((_tmp2FC[2]=& _tmp2F9,Cyc_Tcutil_terr(loc,((_tmp2FD="%s declared with type \n%s\n but initialized with type \n%s",_tag_dyneither(_tmp2FD,sizeof(char),57))),_tag_dyneither(_tmp2FC,sizeof(void*),3)))))))))))))))))));}
# 201
Cyc_Tcutil_explain_failure();}
# 204
if(!Cyc_Tcutil_is_const_exp(_tmp61)){
# 205
const char*_tmp300;void*_tmp2FF;(_tmp2FF=0,Cyc_Tcutil_terr(loc,((_tmp300="initializer is not a constant expression",_tag_dyneither(_tmp300,sizeof(char),41))),_tag_dyneither(_tmp2FF,sizeof(void*),0)));}}}else{
# 209
for(0;_tmp34 != 0;_tmp34=_tmp34->tl){
# 210
void*_tmp6A=(void*)_tmp34->hd;_LL3E: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp6B=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp6B->tag != 0)goto _LL40;}_LL3F:
# 212
 goto _LL41;_LL40: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp6C=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp6C->tag != 1)goto _LL42;}_LL41:
# 213
 goto _LL43;_LL42: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp6D=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp6D->tag != 2)goto _LL44;}_LL43:
# 214
 goto _LL45;_LL44: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp6E=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp6E->tag != 3)goto _LL46;}_LL45:
# 215
 goto _LL47;_LL46: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6F=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp6F->tag != 4)goto _LL48;}_LL47:
# 216
 goto _LL49;_LL48: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp70=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp70->tag != 19)goto _LL4A;}_LL49:
# 217
 goto _LL4B;_LL4A: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp71=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp71->tag != 20)goto _LL4C;}_LL4B:
# 218
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp72=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp72->tag != 23)goto _LL4E;}_LL4D:
# 219
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp73=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp73->tag != 5)goto _LL50;}_LL4F: {
# 220
const char*_tmp303;void*_tmp302;(_tmp302=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp303="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp303,sizeof(char),45))),_tag_dyneither(_tmp302,sizeof(void*),0)));}_LL50: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp74=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp74->tag != 6)goto _LL52;}_LL51:
# 222
 goto _LL53;_LL52: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp75=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp6A;if(_tmp75->tag != 7)goto _LL54;}_LL53:
# 224
{const char*_tmp307;void*_tmp306[1];struct Cyc_String_pa_PrintArg_struct _tmp305;(_tmp305.tag=0,((_tmp305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 225
Cyc_Absyn_attribute2string((void*)_tmp34->hd)),((_tmp306[0]=& _tmp305,Cyc_Tcutil_terr(loc,((_tmp307="bad attribute %s in function declaration",_tag_dyneither(_tmp307,sizeof(char),41))),_tag_dyneither(_tmp306,sizeof(void*),1)))))));}
# 226
goto _LL3D;_LL54:;_LL55:
# 227
 continue;_LL3D:;}}}
# 232
{struct _handler_cons _tmp7B;_push_handler(& _tmp7B);{int _tmp7D=0;if(setjmp(_tmp7B.handler))_tmp7D=1;if(!_tmp7D){
# 233
{struct _tuple13*_tmp7E=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp31);
# 234
void*_tmp7F=(*_tmp7E).f1;void*_tmp81;_LL57: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp80=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL59;else{_tmp81=(void*)_tmp80->f1;}}_LL58: {
# 236
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp30A;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp309;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp86=(_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309[0]=((_tmp30A.tag=1,((_tmp30A.f1=vd,_tmp30A)))),_tmp309)));
# 237
void*_tmp87=Cyc_Tcdecl_merge_binding(_tmp81,(void*)_tmp86,loc,Cyc_Tc_tc_msg);
# 238
if(_tmp87 == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 240
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
# 241
if(_tmp87 != _tmp81  || (*_tmp7E).f2){
# 243
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp310;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp30F;struct _tuple13*_tmp30E;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp31,(struct _tuple13*)(
# 244
(_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E->f1=(void*)((_tmp310=_cycalloc(sizeof(*_tmp310)),((_tmp310[0]=((_tmp30F.tag=0,((_tmp30F.f1=(void*)_tmp87,_tmp30F)))),_tmp310)))),((_tmp30E->f2=1,_tmp30E)))))));}}
# 245
_npop_handler(0);return;}_LL59: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp82=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp7F;if(_tmp82->tag != 1)goto _LL5B;}_LL5A:
# 247
{const char*_tmp313;void*_tmp312;(_tmp312=0,Cyc_Tcutil_warn(loc,((_tmp313="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp313,sizeof(char),57))),_tag_dyneither(_tmp312,sizeof(void*),0)));}
# 248
goto _LL56;_LL5B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp83=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7F;if(_tmp83->tag != 2)goto _LL5D;}_LL5C:
# 250
{const char*_tmp316;void*_tmp315;(_tmp315=0,Cyc_Tcutil_warn(loc,((_tmp316="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp316,sizeof(char),59))),_tag_dyneither(_tmp315,sizeof(void*),0)));}
# 251
goto _LL56;_LL5D: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp84=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp7F;if(_tmp84->tag != 4)goto _LL5F;}_LL5E:
# 252
 goto _LL60;_LL5F: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp85=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp7F;if(_tmp85->tag != 3)goto _LL56;}_LL60:
# 254
{const char*_tmp319;void*_tmp318;(_tmp318=0,Cyc_Tcutil_warn(loc,((_tmp319="variable declaration shadows previous enum tag",_tag_dyneither(_tmp319,sizeof(char),47))),_tag_dyneither(_tmp318,sizeof(void*),0)));}
# 255
goto _LL56;_LL56:;}
# 233
;_pop_handler();}else{void*_tmp7C=(void*)_exn_thrown;void*_tmp94=_tmp7C;_LL62: {struct Cyc_Dict_Absent_exn_struct*_tmp95=(struct Cyc_Dict_Absent_exn_struct*)_tmp94;if(_tmp95->tag != Cyc_Dict_Absent)goto _LL64;}_LL63:
# 257
 goto _LL61;_LL64:;_LL65:(void)_throw(_tmp94);_LL61:;}};}
# 258
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
# 259
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp328;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp327;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp326;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp325;struct _tuple13*_tmp324;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp31,(struct _tuple13*)(
# 260
(_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324->f1=(void*)((_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328[0]=((_tmp325.tag=0,((_tmp325.f1=(void*)((void*)((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=((_tmp326.tag=1,((_tmp326.f1=vd,_tmp326)))),_tmp327))))),_tmp325)))),_tmp328)))),((_tmp324->f2=0,_tmp324)))))));}}
# 263
static int Cyc_Tc_is_main(struct _tuple0*n){
# 264
struct _tuple0 _tmp9C;union Cyc_Absyn_Nmspace _tmp9D;struct _dyneither_ptr*_tmp9E;struct _tuple0*_tmp9B=n;_tmp9C=*_tmp9B;_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;{
# 265
union Cyc_Absyn_Nmspace _tmp9F=_tmp9D;struct Cyc_List_List*_tmpA0;_LL67: if((_tmp9F.Abs_n).tag != 2)goto _LL69;_tmpA0=(struct Cyc_List_List*)(_tmp9F.Abs_n).val;if(_tmpA0 != 0)goto _LL69;_LL68: {
# 267
const char*_tmp329;return Cyc_strcmp((struct _dyneither_ptr)*_tmp9E,((_tmp329="main",_tag_dyneither(_tmp329,sizeof(char),5))))== 0;}_LL69:;_LL6A:
# 268
 return 0;_LL66:;};}
# 272
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 275
struct _dyneither_ptr*v=(*fd->name).f2;
# 279
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
# 280
const char*_tmp32C;void*_tmp32B;(_tmp32B=0,Cyc_Tcutil_terr(loc,((_tmp32C="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp32C,sizeof(char),54))),_tag_dyneither(_tmp32B,sizeof(void*),0)));}
# 282
{union Cyc_Absyn_Nmspace _tmpA4=(*fd->name).f1;struct Cyc_List_List*_tmpA5;_LL6C: if((_tmpA4.Rel_n).tag != 1)goto _LL6E;_tmpA5=(struct Cyc_List_List*)(_tmpA4.Rel_n).val;if(_tmpA5 != 0)goto _LL6E;_LL6D:
# 285
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
# 286
goto _LL6B;_LL6E:;_LL6F:
# 288
 goto _LL6B;_LL6B:;}
# 291
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 293
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 295
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
# 296
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 298
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
# 299
void*_tmpA6=(void*)atts->hd;_LL71: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpA7=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpA6;if(_tmpA7->tag != 7)goto _LL73;}_LL72:
# 300
 goto _LL74;_LL73: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA8=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpA6;if(_tmpA8->tag != 6)goto _LL75;}_LL74:
# 302
{const char*_tmp330;void*_tmp32F[1];struct Cyc_String_pa_PrintArg_struct _tmp32E;(_tmp32E.tag=0,((_tmp32E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp32F[0]=& _tmp32E,Cyc_Tcutil_terr(loc,((_tmp330="bad attribute %s for function",_tag_dyneither(_tmp330,sizeof(char),30))),_tag_dyneither(_tmp32F,sizeof(void*),1)))))));}
# 303
goto _LL70;_LL75:;_LL76:
# 304
 goto _LL70;_LL70:;}}
# 308
{struct _handler_cons _tmpAC;_push_handler(& _tmpAC);{int _tmpAE=0;if(setjmp(_tmpAC.handler))_tmpAE=1;if(!_tmpAE){
# 309
{struct _tuple13*_tmpAF=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
# 310
void*_tmpB0=(*_tmpAF).f1;void*_tmpB2;_LL78: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB0;if(_tmpB1->tag != 0)goto _LL7A;else{_tmpB2=(void*)_tmpB1->f1;}}_LL79: {
# 312
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp333;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp332;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB7=(_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332[0]=((_tmp333.tag=2,((_tmp333.f1=fd,_tmp333)))),_tmp332)));
# 313
void*_tmpB8=Cyc_Tcdecl_merge_binding(_tmpB2,(void*)_tmpB7,loc,Cyc_Tc_tc_msg);
# 314
if(_tmpB8 == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL77;
# 316
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
# 317
if(!(_tmpB8 == _tmpB2  && (*_tmpAF).f2)){
# 318
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp339;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp338;struct _tuple13*_tmp337;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple13*)(
# 319
(_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337->f1=(void*)((_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339[0]=((_tmp338.tag=0,((_tmp338.f1=(void*)_tmpB8,_tmp338)))),_tmp339)))),((_tmp337->f2=1,_tmp337)))))));}}
# 320
goto _LL77;}_LL7A: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB3=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB0;if(_tmpB3->tag != 1)goto _LL7C;}_LL7B:
# 322
{const char*_tmp33C;void*_tmp33B;(_tmp33B=0,Cyc_Tcutil_warn(loc,((_tmp33C="function declaration shadows previous type declaration",_tag_dyneither(_tmp33C,sizeof(char),55))),_tag_dyneither(_tmp33B,sizeof(void*),0)));}
# 323
goto _LL77;_LL7C: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB4=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB0;if(_tmpB4->tag != 2)goto _LL7E;}_LL7D:
# 325
{const char*_tmp33F;void*_tmp33E;(_tmp33E=0,Cyc_Tcutil_warn(loc,((_tmp33F="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp33F,sizeof(char),59))),_tag_dyneither(_tmp33E,sizeof(void*),0)));}
# 326
goto _LL77;_LL7E: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB5=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB0;if(_tmpB5->tag != 4)goto _LL80;}_LL7F:
# 327
 goto _LL81;_LL80: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB0;if(_tmpB6->tag != 3)goto _LL77;}_LL81:
# 329
{const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_Tcutil_warn(loc,((_tmp342="function declaration shadows previous enum tag",_tag_dyneither(_tmp342,sizeof(char),47))),_tag_dyneither(_tmp341,sizeof(void*),0)));}
# 330
goto _LL77;_LL77:;}
# 309
;_pop_handler();}else{void*_tmpAD=(void*)_exn_thrown;void*_tmpC5=_tmpAD;_LL83: {struct Cyc_Dict_Absent_exn_struct*_tmpC6=(struct Cyc_Dict_Absent_exn_struct*)_tmpC5;if(_tmpC6->tag != Cyc_Dict_Absent)goto _LL85;}_LL84:
# 333
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
# 334
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp345;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp344;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC7=(_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=((_tmp345.tag=2,((_tmp345.f1=fd,_tmp345)))),_tmp344)));
# 335
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp354;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp353;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp352;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp351;struct _tuple13*_tmp350;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(struct _tuple13*)(
# 336
(_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350->f1=(void*)((_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354[0]=((_tmp351.tag=0,((_tmp351.f1=(void*)((void*)((_tmp353=_cycalloc(sizeof(*_tmp353)),((_tmp353[0]=((_tmp352.tag=2,((_tmp352.f1=fd,_tmp352)))),_tmp353))))),_tmp351)))),_tmp354)))),((_tmp350->f2=0,_tmp350)))))));}
# 338
goto _LL82;_LL85:;_LL86:(void)_throw(_tmpC5);_LL82:;}};}
# 342
if(in_cinclude)return;{
# 347
struct _RegionHandle _tmpCF=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpCF;_push_region(fnrgn);{
# 348
struct Cyc_Tcenv_Fenv*_tmpD0=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
# 349
struct Cyc_Tcenv_Tenv*_tmp355;struct Cyc_Tcenv_Tenv*_tmpD1=(_tmp355=_region_malloc(fnrgn,sizeof(*_tmp355)),((_tmp355->ns=te->ns,((_tmp355->ae=te->ae,((_tmp355->le=(struct Cyc_Tcenv_Fenv*)_tmpD0,((_tmp355->allow_valueof=0,_tmp355)))))))));
# 351
Cyc_Tcstmt_tcStmt(_tmpD1,fd->body,0);
# 354
Cyc_Tcenv_check_delayed_effects(_tmpD1);
# 355
Cyc_Tcenv_check_delayed_constraints(_tmpD1);
# 358
if(!Cyc_Tcenv_all_labels_resolved(_tmpD1)){
# 359
const char*_tmp358;void*_tmp357;(_tmp357=0,Cyc_Tcutil_terr(loc,((_tmp358="function has goto statements to undefined labels",_tag_dyneither(_tmp358,sizeof(char),49))),_tag_dyneither(_tmp357,sizeof(void*),0)));}}
# 362
if(Cyc_Tc_is_main(fd->name)){
# 364
{void*_tmpD5=Cyc_Tcutil_compress(fd->ret_type);enum Cyc_Absyn_Size_of _tmpD8;_LL88: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD6=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD5;if(_tmpD6->tag != 0)goto _LL8A;}_LL89:
# 365
{const char*_tmp35B;void*_tmp35A;(_tmp35A=0,Cyc_Tcutil_warn(loc,((_tmp35B="main declared with return type void",_tag_dyneither(_tmp35B,sizeof(char),36))),_tag_dyneither(_tmp35A,sizeof(void*),0)));}goto _LL87;_LL8A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD5;if(_tmpD7->tag != 6)goto _LL8C;else{_tmpD8=_tmpD7->f2;}}_LL8B:
# 366
 goto _LL87;_LL8C:;_LL8D:
# 368
{const char*_tmp35F;void*_tmp35E[1];struct Cyc_String_pa_PrintArg_struct _tmp35D;(_tmp35D.tag=0,((_tmp35D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 369
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp35E[0]=& _tmp35D,Cyc_Tcutil_terr(loc,((_tmp35F="main declared with return type %s instead of int or void",_tag_dyneither(_tmp35F,sizeof(char),57))),_tag_dyneither(_tmp35E,sizeof(void*),1)))))));}
# 370
goto _LL87;_LL87:;}
# 372
if(fd->c_varargs  || fd->cyc_varargs != 0){
# 373
const char*_tmp362;void*_tmp361;(_tmp361=0,Cyc_Tcutil_terr(loc,((_tmp362="main declared with varargs",_tag_dyneither(_tmp362,sizeof(char),27))),_tag_dyneither(_tmp361,sizeof(void*),0)));}{
# 374
struct Cyc_List_List*_tmpE0=fd->args;
# 375
if(_tmpE0 != 0){
# 376
struct _tuple8 _tmpE2;void*_tmpE3;struct _tuple8*_tmpE1=(struct _tuple8*)_tmpE0->hd;_tmpE2=*_tmpE1;_tmpE3=_tmpE2.f3;
# 377
{void*_tmpE4=Cyc_Tcutil_compress(_tmpE3);enum Cyc_Absyn_Size_of _tmpE6;_LL8F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE4;if(_tmpE5->tag != 6)goto _LL91;else{_tmpE6=_tmpE5->f2;}}_LL90:
# 378
 goto _LL8E;_LL91:;_LL92:
# 380
{const char*_tmp366;void*_tmp365[1];struct Cyc_String_pa_PrintArg_struct _tmp364;(_tmp364.tag=0,((_tmp364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 381
Cyc_Absynpp_typ2string(_tmpE3)),((_tmp365[0]=& _tmp364,Cyc_Tcutil_terr(loc,((_tmp366="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp366,sizeof(char),60))),_tag_dyneither(_tmp365,sizeof(void*),1)))))));}
# 382
goto _LL8E;_LL8E:;}
# 384
_tmpE0=_tmpE0->tl;
# 385
if(_tmpE0 != 0){
# 386
struct _tuple8 _tmpEB;void*_tmpEC;struct _tuple8*_tmpEA=(struct _tuple8*)_tmpE0->hd;_tmpEB=*_tmpEA;_tmpEC=_tmpEB.f3;
# 387
_tmpE0=_tmpE0->tl;
# 388
if(_tmpE0 != 0){
# 389
const char*_tmp369;void*_tmp368;(_tmp368=0,Cyc_Tcutil_terr(loc,((_tmp369="main declared with too many arguments",_tag_dyneither(_tmp369,sizeof(char),38))),_tag_dyneither(_tmp368,sizeof(void*),0)));}{
# 390
struct Cyc_Core_Opt*_tmp36A;struct Cyc_Core_Opt*tvs=(_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A->v=fd->tvs,_tmp36A)));
# 391
struct Cyc_Core_Opt*_tmp372;struct Cyc_Core_Opt*_tmp371;struct Cyc_Core_Opt*_tmp370;struct Cyc_Core_Opt*_tmp36F;struct Cyc_Core_Opt*_tmp36E;struct Cyc_Core_Opt*_tmp36D;struct Cyc_Core_Opt*_tmp36C;struct Cyc_Core_Opt*_tmp36B;if(((!Cyc_Tcutil_unify(_tmpEC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371->v=& Cyc_Tcutil_rk,_tmp371)))),tvs)),
# 392
Cyc_Absyn_new_evar(((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372->v=& Cyc_Tcutil_rk,_tmp372)))),tvs),
# 393
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
# 394
Cyc_Tcutil_unify(_tmpEC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->v=& Cyc_Tcutil_rk,_tmp36F)))),tvs)),
# 396
Cyc_Absyn_new_evar(((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->v=& Cyc_Tcutil_rk,_tmp370)))),tvs),
# 397
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
# 398
Cyc_Tcutil_unify(_tmpEC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D->v=& Cyc_Tcutil_rk,_tmp36D)))),tvs)),
# 399
Cyc_Absyn_new_evar(((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E->v=& Cyc_Tcutil_rk,_tmp36E)))),tvs),
# 400
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
# 401
Cyc_Tcutil_unify(_tmpEC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B->v=& Cyc_Tcutil_rk,_tmp36B)))),tvs)),
# 403
Cyc_Absyn_new_evar(((_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C->v=& Cyc_Tcutil_rk,_tmp36C)))),tvs),
# 404
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
# 405
const char*_tmp376;void*_tmp375[1];struct Cyc_String_pa_PrintArg_struct _tmp374;(_tmp374.tag=0,((_tmp374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 406
Cyc_Absynpp_typ2string(_tmpEC)),((_tmp375[0]=& _tmp374,Cyc_Tcutil_terr(loc,((_tmp376="second argument of main has type %s instead of char??",_tag_dyneither(_tmp376,sizeof(char),54))),_tag_dyneither(_tmp375,sizeof(void*),1)))))));}};}}};}
# 347
;_pop_region(fnrgn);};};}
# 413
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
# 414
struct _dyneither_ptr*v=(*td->name).f2;
# 419
{union Cyc_Absyn_Nmspace _tmpFB=(*td->name).f1;struct Cyc_List_List*_tmpFC;_LL94: if((_tmpFB.Rel_n).tag != 1)goto _LL96;_tmpFC=(struct Cyc_List_List*)(_tmpFB.Rel_n).val;_LL95:
# 422
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
# 423
goto _LL93;_LL96:;_LL97:
# 424
 goto _LL93;_LL93:;}
# 427
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
# 428
{const char*_tmp37A;void*_tmp379[1];struct Cyc_String_pa_PrintArg_struct _tmp378;(_tmp378.tag=0,((_tmp378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp379[0]=& _tmp378,Cyc_Tcutil_terr(loc,((_tmp37A="redeclaration of typedef %s",_tag_dyneither(_tmp37A,sizeof(char),28))),_tag_dyneither(_tmp379,sizeof(void*),1)))))));}
# 429
return;}
# 432
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
# 433
Cyc_Tcutil_add_tvar_identities(td->tvs);
# 434
if(td->defn != 0){
# 435
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
# 436
(td->tq).real_const=
# 437
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 441
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
# 442
void*_tmp100=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt**_tmp103;struct Cyc_Core_Opt*_tmp105;struct Cyc_Core_Opt**_tmp106;struct Cyc_Absyn_Kind*_tmp107;_LL99: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp101=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp100;if(_tmp101->tag != 1)goto _LL9B;else{_tmp102=_tmp101->f1;_tmp103=(struct Cyc_Core_Opt**)& _tmp101->f1;}}_LL9A:
# 444
 if(td->defn != 0){
# 445
const char*_tmp37E;void*_tmp37D[1];struct Cyc_String_pa_PrintArg_struct _tmp37C;(_tmp37C.tag=0,((_tmp37C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp37D[0]=& _tmp37C,Cyc_Tcutil_warn(loc,((_tmp37E="type variable %s is not used in typedef definition",_tag_dyneither(_tmp37E,sizeof(char),51))),_tag_dyneither(_tmp37D,sizeof(void*),1)))))));}
# 447
{struct Cyc_Core_Opt*_tmp37F;*_tmp103=((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp37F))));}goto _LL98;_LL9B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp104=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp100;if(_tmp104->tag != 2)goto _LL9D;else{_tmp105=_tmp104->f1;_tmp106=(struct Cyc_Core_Opt**)& _tmp104->f1;_tmp107=_tmp104->f2;}}_LL9C:
# 454
{struct Cyc_Core_Opt*_tmp380;*_tmp106=((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->v=Cyc_Tcutil_kind_to_bound(_tmp107),_tmp380))));}
# 455
goto _LL98;_LL9D:;_LL9E:
# 456
 continue;_LL98:;}}
# 460
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 463
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 470
struct _RegionHandle _tmp10D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp10D;_push_region(uprev_rgn);
# 472
for(0;rpo != 0;rpo=rpo->tl){
# 473
struct _tuple10 _tmp10F;void*_tmp110;void*_tmp111;struct _tuple10*_tmp10E=(struct _tuple10*)rpo->hd;_tmp10F=*_tmp10E;_tmp110=_tmp10F.f1;_tmp111=_tmp10F.f2;
# 474
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp110);
# 475
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp111);}{
# 478
struct Cyc_List_List*prev_fields=0;
# 480
struct Cyc_List_List*_tmp112=fields;for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
# 481
struct Cyc_Absyn_Aggrfield _tmp114;struct _dyneither_ptr*_tmp115;struct Cyc_Absyn_Tqual _tmp116;void*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Aggrfield*_tmp113=(struct Cyc_Absyn_Aggrfield*)_tmp112->hd;_tmp114=*_tmp113;_tmp115=_tmp114.name;_tmp116=_tmp114.tq;_tmp117=_tmp114.type;_tmp118=_tmp114.width;_tmp119=_tmp114.attributes;
# 483
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp115)){
# 484
const char*_tmp384;void*_tmp383[1];struct Cyc_String_pa_PrintArg_struct _tmp382;(_tmp382.tag=0,((_tmp382.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp115),((_tmp383[0]=& _tmp382,Cyc_Tcutil_terr(loc,((_tmp384="duplicate member %s",_tag_dyneither(_tmp384,sizeof(char),20))),_tag_dyneither(_tmp383,sizeof(void*),1)))))));}
# 487
{const char*_tmp385;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp115,((_tmp385="",_tag_dyneither(_tmp385,sizeof(char),1))))!= 0){
# 488
struct Cyc_List_List*_tmp386;prev_fields=((_tmp386=_region_malloc(uprev_rgn,sizeof(*_tmp386)),((_tmp386->hd=_tmp115,((_tmp386->tl=prev_fields,_tmp386))))));}}{
# 490
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 494
if(_tmp112->tl == 0  && str_or_union == Cyc_Absyn_StructA)
# 495
field_kind=& Cyc_Tcutil_tak;
# 496
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp117);
# 498
(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).print_const,_tmp117);
# 501
Cyc_Tcutil_check_bitfield(loc,te,_tmp117,_tmp118,_tmp115);};}};
# 472
;_pop_region(uprev_rgn);}
# 507
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 509
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 510
void*_tmp11F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp121;struct Cyc_Core_Opt**_tmp122;struct Cyc_Core_Opt*_tmp124;struct Cyc_Core_Opt**_tmp125;struct Cyc_Absyn_Kind*_tmp126;struct Cyc_Absyn_Kind _tmp127;enum Cyc_Absyn_KindQual _tmp128;enum Cyc_Absyn_AliasQual _tmp129;struct Cyc_Core_Opt*_tmp12B;struct Cyc_Core_Opt**_tmp12C;struct Cyc_Absyn_Kind*_tmp12D;struct Cyc_Absyn_Kind _tmp12E;enum Cyc_Absyn_KindQual _tmp12F;enum Cyc_Absyn_AliasQual _tmp130;struct Cyc_Core_Opt*_tmp132;struct Cyc_Core_Opt**_tmp133;struct Cyc_Absyn_Kind*_tmp134;struct Cyc_Absyn_Kind*_tmp136;struct Cyc_Absyn_Kind _tmp137;enum Cyc_Absyn_KindQual _tmp138;enum Cyc_Absyn_AliasQual _tmp139;_LLA0: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp120=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11F;if(_tmp120->tag != 1)goto _LLA2;else{_tmp121=_tmp120->f1;_tmp122=(struct Cyc_Core_Opt**)& _tmp120->f1;}}_LLA1:
# 512
{struct Cyc_Core_Opt*_tmp387;*_tmp122=((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp387))));}continue;_LLA2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp123=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F;if(_tmp123->tag != 2)goto _LLA4;else{_tmp124=_tmp123->f1;_tmp125=(struct Cyc_Core_Opt**)& _tmp123->f1;_tmp126=_tmp123->f2;_tmp127=*_tmp126;_tmp128=_tmp127.kind;if(_tmp128 != Cyc_Absyn_MemKind)goto _LLA4;_tmp129=_tmp127.aliasqual;}}_LLA3:
# 514
 if(constrain_top_kind  && _tmp129 == Cyc_Absyn_Top){
# 515
struct Cyc_Absyn_Kind*_tmp38A;struct Cyc_Core_Opt*_tmp389;*_tmp125=((_tmp389=_cycalloc(sizeof(*_tmp389)),((_tmp389->v=Cyc_Tcutil_kind_to_bound(((_tmp38A=_cycalloc_atomic(sizeof(*_tmp38A)),((_tmp38A->kind=Cyc_Absyn_BoxKind,((_tmp38A->aliasqual=Cyc_Absyn_Aliasable,_tmp38A))))))),_tmp389))));}else{
# 517
struct Cyc_Absyn_Kind*_tmp38D;struct Cyc_Core_Opt*_tmp38C;*_tmp125=((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C->v=Cyc_Tcutil_kind_to_bound(((_tmp38D=_cycalloc_atomic(sizeof(*_tmp38D)),((_tmp38D->kind=Cyc_Absyn_BoxKind,((_tmp38D->aliasqual=_tmp129,_tmp38D))))))),_tmp38C))));}
# 518
continue;_LLA4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F;if(_tmp12A->tag != 2)goto _LLA6;else{_tmp12B=_tmp12A->f1;_tmp12C=(struct Cyc_Core_Opt**)& _tmp12A->f1;_tmp12D=_tmp12A->f2;_tmp12E=*_tmp12D;_tmp12F=_tmp12E.kind;if(_tmp12F != Cyc_Absyn_BoxKind)goto _LLA6;_tmp130=_tmp12E.aliasqual;if(_tmp130 != Cyc_Absyn_Top)goto _LLA6;}}if(!constrain_top_kind)goto _LLA6;_LLA5:
# 520
{struct Cyc_Absyn_Kind*_tmp390;struct Cyc_Core_Opt*_tmp38F;*_tmp12C=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->v=Cyc_Tcutil_kind_to_bound(((_tmp390=_cycalloc_atomic(sizeof(*_tmp390)),((_tmp390->kind=Cyc_Absyn_BoxKind,((_tmp390->aliasqual=Cyc_Absyn_Aliasable,_tmp390))))))),_tmp38F))));}
# 521
continue;_LLA6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp131=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F;if(_tmp131->tag != 2)goto _LLA8;else{_tmp132=_tmp131->f1;_tmp133=(struct Cyc_Core_Opt**)& _tmp131->f1;_tmp134=_tmp131->f2;}}_LLA7:
# 523
{struct Cyc_Core_Opt*_tmp391;*_tmp133=((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391->v=Cyc_Tcutil_kind_to_bound(_tmp134),_tmp391))));}continue;_LLA8: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp135=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11F;if(_tmp135->tag != 0)goto _LLAA;else{_tmp136=_tmp135->f1;_tmp137=*_tmp136;_tmp138=_tmp137.kind;if(_tmp138 != Cyc_Absyn_MemKind)goto _LLAA;_tmp139=_tmp137.aliasqual;}}_LLA9:
# 525
{const char*_tmp399;void*_tmp398[3];struct Cyc_String_pa_PrintArg_struct _tmp397;struct Cyc_String_pa_PrintArg_struct _tmp396;struct Cyc_Absyn_Kind*_tmp395;struct Cyc_String_pa_PrintArg_struct _tmp394;(_tmp394.tag=0,((_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 526
Cyc_Absynpp_kind2string(((_tmp395=_cycalloc_atomic(sizeof(*_tmp395)),((_tmp395->kind=Cyc_Absyn_MemKind,((_tmp395->aliasqual=_tmp139,_tmp395)))))))),((_tmp396.tag=0,((_tmp396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp397.tag=0,((_tmp397.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp398[0]=& _tmp397,((_tmp398[1]=& _tmp396,((_tmp398[2]=& _tmp394,Cyc_Tcutil_terr(loc,((_tmp399="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp399,sizeof(char),57))),_tag_dyneither(_tmp398,sizeof(void*),3)))))))))))))))))));}
# 527
continue;_LLAA:;_LLAB:
# 528
 continue;_LL9F:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 533
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
# 534
struct _dyneither_ptr*_tmp148=(*ad->name).f2;
# 540
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
# 541
void*_tmp149=(void*)atts->hd;_LLAD: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp14A=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp149;if(_tmp14A->tag != 7)goto _LLAF;}_LLAE:
# 542
 goto _LLB0;_LLAF: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp14B=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp149;if(_tmp14B->tag != 6)goto _LLB1;}_LLB0:
# 543
 continue;_LLB1:;_LLB2:
# 545
{const char*_tmp39E;void*_tmp39D[2];struct Cyc_String_pa_PrintArg_struct _tmp39C;struct Cyc_String_pa_PrintArg_struct _tmp39B;(_tmp39B.tag=0,((_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp148),((_tmp39C.tag=0,((_tmp39C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp39D[0]=& _tmp39C,((_tmp39D[1]=& _tmp39B,Cyc_Tcutil_terr(loc,((_tmp39E="bad attribute %s in  %s definition",_tag_dyneither(_tmp39E,sizeof(char),35))),_tag_dyneither(_tmp39D,sizeof(void*),2)))))))))))));}
# 547
goto _LLAC;_LLAC:;}}{
# 550
struct Cyc_List_List*_tmp150=ad->tvs;
# 552
{union Cyc_Absyn_Nmspace _tmp151=(*ad->name).f1;struct Cyc_List_List*_tmp152;_LLB4: if((_tmp151.Rel_n).tag != 1)goto _LLB6;_tmp152=(struct Cyc_List_List*)(_tmp151.Rel_n).val;if(_tmp152 != 0)goto _LLB6;_LLB5:
# 555
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
# 556
goto _LLB3;_LLB6:;_LLB7:
# 558
 goto _LLB3;_LLB3:;}
# 562
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 564
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 568
{struct _tuple14 _tmp39F;struct _tuple14 _tmp154=(_tmp39F.f1=ad->impl,((_tmp39F.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp148),_tmp39F)));struct Cyc_Absyn_AggrdeclImpl*_tmp155;struct Cyc_Absyn_Aggrdecl***_tmp156;struct Cyc_Absyn_AggrdeclImpl*_tmp157;struct Cyc_Absyn_AggrdeclImpl _tmp158;struct Cyc_List_List*_tmp159;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp15B;int _tmp15C;struct Cyc_Absyn_Aggrdecl***_tmp15D;struct Cyc_Absyn_AggrdeclImpl*_tmp15E;struct Cyc_Absyn_AggrdeclImpl _tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp162;int _tmp163;struct Cyc_Absyn_Aggrdecl***_tmp164;struct Cyc_Absyn_Aggrdecl**_tmp165;struct Cyc_Absyn_AggrdeclImpl*_tmp166;struct Cyc_Absyn_Aggrdecl***_tmp167;struct Cyc_Absyn_Aggrdecl**_tmp168;_LLB9: _tmp155=_tmp154.f1;if(_tmp155 != 0)goto _LLBB;_tmp156=_tmp154.f2;if(_tmp156 != 0)goto _LLBB;_LLBA:
# 571
 Cyc_Tc_rule_out_memkind(loc,_tmp148,_tmp150,0);
# 572
{struct Cyc_Absyn_Aggrdecl**_tmp3A0;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp148,((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0[0]=ad,_tmp3A0)))));}
# 573
goto _LLB8;_LLBB: _tmp157=_tmp154.f1;if(_tmp157 == 0)goto _LLBD;_tmp158=*_tmp157;_tmp159=_tmp158.exist_vars;_tmp15A=_tmp158.rgn_po;_tmp15B=_tmp158.fields;_tmp15C=_tmp158.tagged;_tmp15D=_tmp154.f2;if(_tmp15D != 0)goto _LLBD;_LLBC: {
# 577
struct Cyc_Absyn_Aggrdecl*_tmp3A3;struct Cyc_Absyn_Aggrdecl**_tmp3A2;struct Cyc_Absyn_Aggrdecl**_tmp16A=(_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2[0]=((_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3->kind=ad->kind,((_tmp3A3->sc=Cyc_Absyn_Extern,((_tmp3A3->name=ad->name,((_tmp3A3->tvs=_tmp150,((_tmp3A3->impl=0,((_tmp3A3->attributes=ad->attributes,_tmp3A3)))))))))))))),_tmp3A2)));
# 579
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp148,_tmp16A);
# 582
Cyc_Tcutil_check_unique_tvars(loc,_tmp159);
# 584
Cyc_Tcutil_add_tvar_identities(_tmp159);
# 587
if(_tmp15C  && ad->kind == Cyc_Absyn_StructA){
# 588
const char*_tmp3A6;void*_tmp3A5;(_tmp3A5=0,Cyc_Tcutil_terr(loc,((_tmp3A6="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3A6,sizeof(char),56))),_tag_dyneither(_tmp3A5,sizeof(void*),0)));}
# 589
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp150,_tmp159),_tmp15A,_tmp15B);
# 591
Cyc_Tc_rule_out_memkind(loc,_tmp148,_tmp150,0);
# 594
Cyc_Tc_rule_out_memkind(loc,_tmp148,_tmp159,1);
# 596
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp15C){
# 599
struct Cyc_List_List*f=_tmp15B;for(0;f != 0;f=f->tl){
# 600
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){
# 601
const char*_tmp3AC;void*_tmp3AB[3];struct Cyc_String_pa_PrintArg_struct _tmp3AA;struct Cyc_String_pa_PrintArg_struct _tmp3A9;struct Cyc_String_pa_PrintArg_struct _tmp3A8;(_tmp3A8.tag=0,((_tmp3A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 602
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp3A9.tag=0,((_tmp3A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp148),((_tmp3AA.tag=0,((_tmp3AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp3AB[0]=& _tmp3AA,((_tmp3AB[1]=& _tmp3A9,((_tmp3AB[2]=& _tmp3A8,Cyc_Tcutil_warn(loc,((_tmp3AC="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp3AC,sizeof(char),98))),_tag_dyneither(_tmp3AB,sizeof(void*),3)))))))))))))))))));}}}
# 604
*_tmp16A=ad;
# 605
goto _LLB8;}_LLBD: _tmp15E=_tmp154.f1;if(_tmp15E == 0)goto _LLBF;_tmp15F=*_tmp15E;_tmp160=_tmp15F.exist_vars;_tmp161=_tmp15F.rgn_po;_tmp162=_tmp15F.fields;_tmp163=_tmp15F.tagged;_tmp164=_tmp154.f2;if(_tmp164 == 0)goto _LLBF;_tmp165=*_tmp164;_LLBE:
# 608
 if(ad->kind != (*_tmp165)->kind){
# 609
const char*_tmp3AF;void*_tmp3AE;(_tmp3AE=0,Cyc_Tcutil_terr(loc,((_tmp3AF="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp3AF,sizeof(char),52))),_tag_dyneither(_tmp3AE,sizeof(void*),0)));}{
# 611
struct Cyc_Absyn_Aggrdecl*_tmp176=*_tmp165;
# 613
{struct Cyc_Absyn_Aggrdecl*_tmp3B0;*_tmp165=((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->kind=ad->kind,((_tmp3B0->sc=Cyc_Absyn_Extern,((_tmp3B0->name=ad->name,((_tmp3B0->tvs=_tmp150,((_tmp3B0->impl=0,((_tmp3B0->attributes=ad->attributes,_tmp3B0))))))))))))));}
# 617
Cyc_Tcutil_check_unique_tvars(loc,_tmp160);
# 619
Cyc_Tcutil_add_tvar_identities(_tmp160);
# 621
if(_tmp163  && ad->kind == Cyc_Absyn_StructA){
# 622
const char*_tmp3B3;void*_tmp3B2;(_tmp3B2=0,Cyc_Tcutil_terr(loc,((_tmp3B3="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3B3,sizeof(char),56))),_tag_dyneither(_tmp3B2,sizeof(void*),0)));}
# 623
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp150,_tmp160),_tmp161,_tmp162);
# 625
*_tmp165=_tmp176;
# 626
_tmp168=_tmp165;goto _LLC0;};_LLBF: _tmp166=_tmp154.f1;if(_tmp166 != 0)goto _LLB8;_tmp167=_tmp154.f2;if(_tmp167 == 0)goto _LLB8;_tmp168=*_tmp167;_LLC0: {
# 629
struct Cyc_Absyn_Aggrdecl*_tmp17A=Cyc_Tcdecl_merge_aggrdecl(*_tmp168,ad,loc,Cyc_Tc_tc_msg);
# 630
if(_tmp17A == 0)
# 631
return;else{
# 633
Cyc_Tc_rule_out_memkind(loc,_tmp148,_tmp150,0);
# 636
if(ad->impl != 0)
# 637
Cyc_Tc_rule_out_memkind(loc,_tmp148,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 639
*_tmp168=(struct Cyc_Absyn_Aggrdecl*)_tmp17A;
# 640
ad=(struct Cyc_Absyn_Aggrdecl*)_tmp17A;
# 641
goto _LLB8;}}_LLB8:;}{
# 645
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3B9;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp3B8;struct _tuple13*_tmp3B7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp148,(struct _tuple13*)(
# 646
(_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->f1=(void*)((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3B8.tag=1,((_tmp3B8.f1=ad,_tmp3B8)))),_tmp3B9)))),((_tmp3B7->f2=1,_tmp3B7)))))));};};}
# 649
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
# 650
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 651
void*_tmp17E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt*_tmp180;struct Cyc_Core_Opt**_tmp181;struct Cyc_Core_Opt*_tmp183;struct Cyc_Core_Opt**_tmp184;struct Cyc_Absyn_Kind*_tmp185;struct Cyc_Absyn_Kind _tmp186;enum Cyc_Absyn_KindQual _tmp187;enum Cyc_Absyn_AliasQual _tmp188;struct Cyc_Core_Opt*_tmp18A;struct Cyc_Core_Opt**_tmp18B;struct Cyc_Absyn_Kind*_tmp18C;struct Cyc_Absyn_Kind _tmp18D;enum Cyc_Absyn_KindQual _tmp18E;enum Cyc_Absyn_AliasQual _tmp18F;struct Cyc_Core_Opt*_tmp191;struct Cyc_Core_Opt**_tmp192;struct Cyc_Absyn_Kind*_tmp193;struct Cyc_Absyn_Kind _tmp194;enum Cyc_Absyn_KindQual _tmp195;enum Cyc_Absyn_AliasQual _tmp196;struct Cyc_Core_Opt*_tmp198;struct Cyc_Core_Opt**_tmp199;struct Cyc_Absyn_Kind*_tmp19A;struct Cyc_Absyn_Kind _tmp19B;enum Cyc_Absyn_KindQual _tmp19C;enum Cyc_Absyn_AliasQual _tmp19D;struct Cyc_Core_Opt*_tmp19F;struct Cyc_Core_Opt**_tmp1A0;struct Cyc_Absyn_Kind*_tmp1A1;struct Cyc_Absyn_Kind _tmp1A2;enum Cyc_Absyn_KindQual _tmp1A3;enum Cyc_Absyn_AliasQual _tmp1A4;struct Cyc_Absyn_Kind*_tmp1A6;struct Cyc_Absyn_Kind _tmp1A7;enum Cyc_Absyn_KindQual _tmp1A8;enum Cyc_Absyn_AliasQual _tmp1A9;struct Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt**_tmp1AC;struct Cyc_Absyn_Kind*_tmp1AD;struct Cyc_Absyn_Kind _tmp1AE;enum Cyc_Absyn_KindQual _tmp1AF;enum Cyc_Absyn_AliasQual _tmp1B0;struct Cyc_Absyn_Kind*_tmp1B2;struct Cyc_Absyn_Kind _tmp1B3;enum Cyc_Absyn_KindQual _tmp1B4;enum Cyc_Absyn_AliasQual _tmp1B5;struct Cyc_Absyn_Kind*_tmp1B7;struct Cyc_Absyn_Kind _tmp1B8;enum Cyc_Absyn_KindQual _tmp1B9;enum Cyc_Absyn_AliasQual _tmp1BA;_LLC2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp17F=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp17F->tag != 1)goto _LLC4;else{_tmp180=_tmp17F->f1;_tmp181=(struct Cyc_Core_Opt**)& _tmp17F->f1;}}_LLC3:
# 652
 _tmp184=_tmp181;goto _LLC5;_LLC4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp182=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp182->tag != 2)goto _LLC6;else{_tmp183=_tmp182->f1;_tmp184=(struct Cyc_Core_Opt**)& _tmp182->f1;_tmp185=_tmp182->f2;_tmp186=*_tmp185;_tmp187=_tmp186.kind;if(_tmp187 != Cyc_Absyn_MemKind)goto _LLC6;_tmp188=_tmp186.aliasqual;if(_tmp188 != Cyc_Absyn_Top)goto _LLC6;}}_LLC5:
# 654
 _tmp18B=_tmp184;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp189=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp189->tag != 2)goto _LLC8;else{_tmp18A=_tmp189->f1;_tmp18B=(struct Cyc_Core_Opt**)& _tmp189->f1;_tmp18C=_tmp189->f2;_tmp18D=*_tmp18C;_tmp18E=_tmp18D.kind;if(_tmp18E != Cyc_Absyn_MemKind)goto _LLC8;_tmp18F=_tmp18D.aliasqual;if(_tmp18F != Cyc_Absyn_Aliasable)goto _LLC8;}}_LLC7:
# 656
{struct Cyc_Core_Opt*_tmp3BA;*_tmp18B=((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3BA))));}goto _LLC1;_LLC8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp190=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp190->tag != 2)goto _LLCA;else{_tmp191=_tmp190->f1;_tmp192=(struct Cyc_Core_Opt**)& _tmp190->f1;_tmp193=_tmp190->f2;_tmp194=*_tmp193;_tmp195=_tmp194.kind;if(_tmp195 != Cyc_Absyn_AnyKind)goto _LLCA;_tmp196=_tmp194.aliasqual;if(_tmp196 != Cyc_Absyn_Top)goto _LLCA;}}_LLC9:
# 657
 _tmp199=_tmp192;goto _LLCB;_LLCA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp197=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp197->tag != 2)goto _LLCC;else{_tmp198=_tmp197->f1;_tmp199=(struct Cyc_Core_Opt**)& _tmp197->f1;_tmp19A=_tmp197->f2;_tmp19B=*_tmp19A;_tmp19C=_tmp19B.kind;if(_tmp19C != Cyc_Absyn_AnyKind)goto _LLCC;_tmp19D=_tmp19B.aliasqual;if(_tmp19D != Cyc_Absyn_Aliasable)goto _LLCC;}}_LLCB:
# 659
{struct Cyc_Core_Opt*_tmp3BB;*_tmp199=((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp3BB))));}goto _LLC1;_LLCC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp19E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp19E->tag != 2)goto _LLCE;else{_tmp19F=_tmp19E->f1;_tmp1A0=(struct Cyc_Core_Opt**)& _tmp19E->f1;_tmp1A1=_tmp19E->f2;_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.kind;if(_tmp1A3 != Cyc_Absyn_RgnKind)goto _LLCE;_tmp1A4=_tmp1A2.aliasqual;if(_tmp1A4 != Cyc_Absyn_Top)goto _LLCE;}}_LLCD:
# 661
{struct Cyc_Core_Opt*_tmp3BC;*_tmp1A0=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp3BC))));}goto _LLC1;_LLCE: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1A5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp1A5->tag != 0)goto _LLD0;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=*_tmp1A6;_tmp1A8=_tmp1A7.kind;if(_tmp1A8 != Cyc_Absyn_RgnKind)goto _LLD0;_tmp1A9=_tmp1A7.aliasqual;if(_tmp1A9 != Cyc_Absyn_Top)goto _LLD0;}}_LLCF:
# 663
{const char*_tmp3C1;void*_tmp3C0[2];struct Cyc_String_pa_PrintArg_struct _tmp3BF;struct Cyc_String_pa_PrintArg_struct _tmp3BE;(_tmp3BE.tag=0,((_tmp3BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3BF.tag=0,((_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3C0[0]=& _tmp3BF,((_tmp3C0[1]=& _tmp3BE,Cyc_Tcutil_terr(loc,((_tmp3C1="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp3C1,sizeof(char),57))),_tag_dyneither(_tmp3C0,sizeof(void*),2)))))))))))));}
# 664
goto _LLC1;_LLD0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1AA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp1AA->tag != 2)goto _LLD2;else{_tmp1AB=_tmp1AA->f1;_tmp1AC=(struct Cyc_Core_Opt**)& _tmp1AA->f1;_tmp1AD=_tmp1AA->f2;_tmp1AE=*_tmp1AD;_tmp1AF=_tmp1AE.kind;_tmp1B0=_tmp1AE.aliasqual;if(_tmp1B0 != Cyc_Absyn_Unique)goto _LLD2;}}_LLD1:
# 665
 _tmp1B4=_tmp1AF;goto _LLD3;_LLD2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1B1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp1B1->tag != 0)goto _LLD4;else{_tmp1B2=_tmp1B1->f1;_tmp1B3=*_tmp1B2;_tmp1B4=_tmp1B3.kind;_tmp1B5=_tmp1B3.aliasqual;if(_tmp1B5 != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:
# 667
{const char*_tmp3C9;void*_tmp3C8[3];struct Cyc_String_pa_PrintArg_struct _tmp3C7;struct Cyc_String_pa_PrintArg_struct _tmp3C6;struct Cyc_Absyn_Kind*_tmp3C5;struct Cyc_String_pa_PrintArg_struct _tmp3C4;(_tmp3C4.tag=0,((_tmp3C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 669
Cyc_Absynpp_kind2string(((_tmp3C5=_cycalloc_atomic(sizeof(*_tmp3C5)),((_tmp3C5->kind=_tmp1B4,((_tmp3C5->aliasqual=Cyc_Absyn_Unique,_tmp3C5)))))))),((_tmp3C6.tag=0,((_tmp3C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3C7.tag=0,((_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3C8[0]=& _tmp3C7,((_tmp3C8[1]=& _tmp3C6,((_tmp3C8[2]=& _tmp3C4,Cyc_Tcutil_terr(loc,((_tmp3C9="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3C9,sizeof(char),57))),_tag_dyneither(_tmp3C8,sizeof(void*),3)))))))))))))))))));}goto _LLC1;_LLD4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1B6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17E;if(_tmp1B6->tag != 0)goto _LLD6;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.kind;if(_tmp1B9 != Cyc_Absyn_MemKind)goto _LLD6;_tmp1BA=_tmp1B8.aliasqual;}}_LLD5:
# 671
{const char*_tmp3D1;void*_tmp3D0[3];struct Cyc_String_pa_PrintArg_struct _tmp3CF;struct Cyc_String_pa_PrintArg_struct _tmp3CE;struct Cyc_Absyn_Kind*_tmp3CD;struct Cyc_String_pa_PrintArg_struct _tmp3CC;(_tmp3CC.tag=0,((_tmp3CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 673
Cyc_Absynpp_kind2string(((_tmp3CD=_cycalloc_atomic(sizeof(*_tmp3CD)),((_tmp3CD->kind=Cyc_Absyn_MemKind,((_tmp3CD->aliasqual=_tmp1BA,_tmp3CD)))))))),((_tmp3CE.tag=0,((_tmp3CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3CF.tag=0,((_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D0[0]=& _tmp3CF,((_tmp3D0[1]=& _tmp3CE,((_tmp3D0[2]=& _tmp3CC,Cyc_Tcutil_terr(loc,((_tmp3D1="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3D1,sizeof(char),57))),_tag_dyneither(_tmp3D0,sizeof(void*),3)))))))))))))))))));}goto _LLC1;_LLD6:;_LLD7:
# 674
 goto _LLC1;_LLC1:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 680
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 689
{struct Cyc_List_List*_tmp1CE=fields;for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
# 690
struct Cyc_Absyn_Datatypefield*_tmp1CF=(struct Cyc_Absyn_Datatypefield*)_tmp1CE->hd;
# 692
{struct Cyc_List_List*typs=_tmp1CF->typs;for(0;typs != 0;typs=typs->tl){
# 693
Cyc_Tcutil_check_type(_tmp1CF->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 695
struct _RegionHandle _tmp1D0=_new_region("temp");struct _RegionHandle*temp=& _tmp1D0;_push_region(temp);
# 696
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
# 697
const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;(_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 699
Cyc_Absynpp_qvar2string(_tmp1CF->name)),((_tmp3D4[0]=& _tmp3D3,Cyc_Tcutil_terr(_tmp1CF->loc,((_tmp3D5="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3D5,sizeof(char),51))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))));}
# 701
((*((struct _tuple15*)typs->hd)).f1).real_const=
# 702
Cyc_Tcutil_extract_const_from_typedef(_tmp1CF->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 695
;_pop_region(temp);};}}{
# 706
union Cyc_Absyn_Nmspace _tmp1D4=(*_tmp1CF->name).f1;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D6;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D8;int _tmp1D9;_LLD9: if((_tmp1D4.Rel_n).tag != 1)goto _LLDB;_tmp1D5=(struct Cyc_List_List*)(_tmp1D4.Rel_n).val;if(_tmp1D5 != 0)goto _LLDB;_LLDA:
# 708
 if(is_extensible)
# 709
(*_tmp1CF->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 711
(*_tmp1CF->name).f1=(*name).f1;}
# 712
goto _LLD8;_LLDB: if((_tmp1D4.Rel_n).tag != 1)goto _LLDD;_tmp1D6=(struct Cyc_List_List*)(_tmp1D4.Rel_n).val;_LLDC:
# 714
{const char*_tmp3D9;void*_tmp3D8[1];struct Cyc_String_pa_PrintArg_struct _tmp3D7;(_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 715
Cyc_Absynpp_qvar2string(_tmp1CF->name)),((_tmp3D8[0]=& _tmp3D7,Cyc_Tcutil_terr(_tmp1CF->loc,((_tmp3D9="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3D9,sizeof(char),58))),_tag_dyneither(_tmp3D8,sizeof(void*),1)))))));}
# 716
goto _LLD8;_LLDD: if((_tmp1D4.C_n).tag != 3)goto _LLDF;_tmp1D7=(struct Cyc_List_List*)(_tmp1D4.C_n).val;_LLDE:
# 717
 goto _LLD8;_LLDF: if((_tmp1D4.Abs_n).tag != 2)goto _LLE1;_tmp1D8=(struct Cyc_List_List*)(_tmp1D4.Abs_n).val;_LLE0:
# 718
 goto _LLD8;_LLE1: if((_tmp1D4.Loc_n).tag != 4)goto _LLD8;_tmp1D9=(int)(_tmp1D4.Loc_n).val;_LLE2: {
# 719
const char*_tmp3DC;void*_tmp3DB;(_tmp3DB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3DC="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3DC,sizeof(char),24))),_tag_dyneither(_tmp3DB,sizeof(void*),0)));}_LLD8:;};}}{
# 723
struct Cyc_List_List*fields2;
# 724
if(is_extensible){
# 726
int _tmp1DF=1;
# 727
struct Cyc_List_List*_tmp1E0=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1DF,(*name).f2,loc,Cyc_Tc_tc_msg);
# 728
if(_tmp1DF)
# 729
fields2=_tmp1E0;else{
# 731
fields2=0;}}else{
# 733
struct _RegionHandle _tmp1E1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1E1;_push_region(uprev_rgn);
# 735
{struct Cyc_List_List*prev_fields=0;
# 736
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
# 737
struct Cyc_Absyn_Datatypefield*_tmp1E2=(struct Cyc_Absyn_Datatypefield*)fs->hd;
# 738
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1E2->name).f2)){
# 739
const char*_tmp3E1;void*_tmp3E0[2];struct Cyc_String_pa_PrintArg_struct _tmp3DF;struct Cyc_String_pa_PrintArg_struct _tmp3DE;(_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1E2->name).f2),((_tmp3E0[0]=& _tmp3DF,((_tmp3E0[1]=& _tmp3DE,Cyc_Tcutil_terr(_tmp1E2->loc,((_tmp3E1="duplicate field name %s in %s",_tag_dyneither(_tmp3E1,sizeof(char),30))),_tag_dyneither(_tmp3E0,sizeof(void*),2)))))))))))));}else{
# 741
struct Cyc_List_List*_tmp3E2;prev_fields=((_tmp3E2=_region_malloc(uprev_rgn,sizeof(*_tmp3E2)),((_tmp3E2->hd=(*_tmp1E2->name).f2,((_tmp3E2->tl=prev_fields,_tmp3E2))))));}
# 743
if(_tmp1E2->sc != Cyc_Absyn_Public){
# 744
{const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_PrintArg_struct _tmp3E4;(_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1E2->name).f2),((_tmp3E5[0]=& _tmp3E4,Cyc_Tcutil_warn(loc,((_tmp3E6="ignoring scope of field %s",_tag_dyneither(_tmp3E6,sizeof(char),27))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))));}
# 745
_tmp1E2->sc=Cyc_Absyn_Public;}}}
# 748
fields2=fields;}
# 735
;_pop_region(uprev_rgn);}
# 754
{struct Cyc_List_List*_tmp1EB=fields;for(0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){
# 755
struct Cyc_Absyn_Datatypefield*_tmp1EC=(struct Cyc_Absyn_Datatypefield*)_tmp1EB->hd;
# 756
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3EC;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3EB;struct _tuple13*_tmp3EA;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1EC->name).f2,(struct _tuple13*)(
# 757
(_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA->f1=(void*)((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3EB.tag=2,((_tmp3EB.f1=tudres,((_tmp3EB.f2=_tmp1EC,_tmp3EB)))))),_tmp3EC)))),((_tmp3EA->f2=1,_tmp3EA)))))));}}
# 760
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 763
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
# 764
struct _dyneither_ptr*v=(*tud->name).f2;
# 765
const char*_tmp3EE;const char*_tmp3ED;struct _dyneither_ptr obj=tud->is_extensible?(_tmp3EE="@extensible datatype",_tag_dyneither(_tmp3EE,sizeof(char),21)):((_tmp3ED="datatype",_tag_dyneither(_tmp3ED,sizeof(char),9)));
# 770
struct Cyc_List_List*tvs=tud->tvs;
# 773
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 775
Cyc_Tcutil_add_tvar_identities(tvs);{
# 780
struct _RegionHandle _tmp1F0=_new_region("temp");struct _RegionHandle*temp=& _tmp1F0;_push_region(temp);
# 781
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 790 "tc.cyc"
{struct _handler_cons _tmp1F1;_push_handler(& _tmp1F1);{int _tmp1F3=0;if(setjmp(_tmp1F1.handler))_tmp1F3=1;if(!_tmp1F3){
# 791
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
# 792
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp3F1;struct Cyc_Dict_Absent_exn_struct*_tmp3F0;(int)_throw((void*)((_tmp3F0=_cycalloc_atomic(sizeof(*_tmp3F0)),((_tmp3F0[0]=((_tmp3F1.tag=Cyc_Dict_Absent,_tmp3F1)),_tmp3F0)))));}
# 793
if(tud_opt != 0)
# 794
tud->name=(*(*tud_opt))->name;else{
# 796
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 791
;_pop_handler();}else{void*_tmp1F2=(void*)_exn_thrown;void*_tmp1F7=_tmp1F2;_LLE4: {struct Cyc_Dict_Absent_exn_struct*_tmp1F8=(struct Cyc_Dict_Absent_exn_struct*)_tmp1F7;if(_tmp1F8->tag != Cyc_Dict_Absent)goto _LLE6;}_LLE5:
# 800
{union Cyc_Absyn_Nmspace _tmp1F9=(*tud->name).f1;struct Cyc_List_List*_tmp1FA;_LLE9: if((_tmp1F9.Rel_n).tag != 1)goto _LLEB;_tmp1FA=(struct Cyc_List_List*)(_tmp1F9.Rel_n).val;if(_tmp1FA != 0)goto _LLEB;_LLEA:
# 801
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLE8;_LLEB:;_LLEC:
# 802
 goto _LLE8;_LLE8:;}{
# 804
struct Cyc_Absyn_Datatypedecl***_tmp1FB=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
# 805
{struct Cyc_Absyn_Datatypedecl***_tmp3F2;tud_opt=(unsigned int)_tmp1FB?(_tmp3F2=_region_malloc(temp,sizeof(*_tmp3F2)),((_tmp3F2[0]=*_tmp1FB,_tmp3F2))): 0;}
# 806
goto _LLE3;};_LLE6:;_LLE7:(void)_throw(_tmp1F7);_LLE3:;}};}{
# 811
struct _tuple16 _tmp3F3;struct _tuple16 _tmp1FE=(_tmp3F3.f1=tud->fields,((_tmp3F3.f2=tud_opt,_tmp3F3)));struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Absyn_Datatypedecl***_tmp200;struct Cyc_Core_Opt*_tmp201;struct Cyc_Core_Opt _tmp202;struct Cyc_List_List*_tmp203;struct Cyc_List_List**_tmp204;struct Cyc_Absyn_Datatypedecl***_tmp205;struct Cyc_Core_Opt*_tmp206;struct Cyc_Core_Opt _tmp207;struct Cyc_List_List*_tmp208;struct Cyc_List_List**_tmp209;struct Cyc_Absyn_Datatypedecl***_tmp20A;struct Cyc_Absyn_Datatypedecl**_tmp20B;struct Cyc_Core_Opt*_tmp20C;struct Cyc_Absyn_Datatypedecl***_tmp20D;struct Cyc_Absyn_Datatypedecl**_tmp20E;_LLEE: _tmp1FF=_tmp1FE.f1;if(_tmp1FF != 0)goto _LLF0;_tmp200=_tmp1FE.f2;if(_tmp200 != 0)goto _LLF0;_LLEF:
# 814
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 815
{struct Cyc_Absyn_Datatypedecl**_tmp3F4;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=tud,_tmp3F4)))));}
# 816
goto _LLED;_LLF0: _tmp201=_tmp1FE.f1;if(_tmp201 == 0)goto _LLF2;_tmp202=*_tmp201;_tmp203=(struct Cyc_List_List*)_tmp202.v;_tmp204=(struct Cyc_List_List**)&(*_tmp1FE.f1).v;_tmp205=_tmp1FE.f2;if(_tmp205 != 0)goto _LLF2;_LLF1: {
# 819
struct Cyc_Absyn_Datatypedecl*_tmp3F7;struct Cyc_Absyn_Datatypedecl**_tmp3F6;struct Cyc_Absyn_Datatypedecl**_tmp210=(_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7->sc=Cyc_Absyn_Extern,((_tmp3F7->name=tud->name,((_tmp3F7->tvs=tvs,((_tmp3F7->fields=0,((_tmp3F7->is_extensible=tud->is_extensible,_tmp3F7)))))))))))),_tmp3F6)));
# 821
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp210);
# 824
*_tmp204=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp204,tvs,tud);
# 825
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 826
*_tmp210=tud;
# 827
goto _LLED;}_LLF2: _tmp206=_tmp1FE.f1;if(_tmp206 == 0)goto _LLF4;_tmp207=*_tmp206;_tmp208=(struct Cyc_List_List*)_tmp207.v;_tmp209=(struct Cyc_List_List**)&(*_tmp1FE.f1).v;_tmp20A=_tmp1FE.f2;if(_tmp20A == 0)goto _LLF4;_tmp20B=*_tmp20A;_LLF3: {
# 829
struct Cyc_Absyn_Datatypedecl*_tmp213=*_tmp20B;
# 832
if((!tud->is_extensible  && (unsigned int)_tmp213) && _tmp213->is_extensible)
# 833
tud->is_extensible=1;
# 835
{struct Cyc_Absyn_Datatypedecl*_tmp3F8;*_tmp20B=((_tmp3F8=_cycalloc(sizeof(*_tmp3F8)),((_tmp3F8->sc=Cyc_Absyn_Extern,((_tmp3F8->name=tud->name,((_tmp3F8->tvs=tvs,((_tmp3F8->fields=0,((_tmp3F8->is_extensible=tud->is_extensible,_tmp3F8))))))))))));}
# 839
*_tmp209=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp209,tvs,tud);
# 843
*_tmp20B=_tmp213;
# 844
_tmp20E=_tmp20B;goto _LLF5;}_LLF4: _tmp20C=_tmp1FE.f1;if(_tmp20C != 0)goto _LLED;_tmp20D=_tmp1FE.f2;if(_tmp20D == 0)goto _LLED;_tmp20E=*_tmp20D;_LLF5: {
# 847
struct Cyc_Absyn_Datatypedecl*_tmp215=Cyc_Tcdecl_merge_datatypedecl(*_tmp20E,tud,loc,Cyc_Tc_tc_msg);
# 848
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
# 849
if(_tmp215 == 0){
# 850
_npop_handler(0);return;}else{
# 852
*_tmp20E=(struct Cyc_Absyn_Datatypedecl*)_tmp215;
# 853
goto _LLED;}}_LLED:;};}
# 781 "tc.cyc"
;_pop_region(temp);};}
# 859 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
# 860
struct _dyneither_ptr*v=(*ed->name).f2;
# 866
{union Cyc_Absyn_Nmspace _tmp218=(*ed->name).f1;struct Cyc_List_List*_tmp219;_LLF7: if((_tmp218.Rel_n).tag != 1)goto _LLF9;_tmp219=(struct Cyc_List_List*)(_tmp218.Rel_n).val;if(_tmp219 != 0)goto _LLF9;_LLF8:
# 869
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
# 870
goto _LLF6;_LLF9:;_LLFA:
# 871
 goto _LLF6;_LLF6:;}
# 875
if(ed->fields != 0){struct _RegionHandle _tmp21A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp21A;_push_region(uprev_rgn);
# 876
{struct Cyc_List_List*prev_fields=0;
# 877
unsigned int tag_count=0;
# 878
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 879
struct Cyc_Absyn_Enumfield*_tmp21B=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 881
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp21B->name).f2)){
# 882
const char*_tmp3FC;void*_tmp3FB[1];struct Cyc_String_pa_PrintArg_struct _tmp3FA;(_tmp3FA.tag=0,((_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp21B->name).f2),((_tmp3FB[0]=& _tmp3FA,Cyc_Tcutil_terr(_tmp21B->loc,((_tmp3FC="duplicate field name %s",_tag_dyneither(_tmp3FC,sizeof(char),24))),_tag_dyneither(_tmp3FB,sizeof(void*),1)))))));}else{
# 884
struct Cyc_List_List*_tmp3FD;prev_fields=((_tmp3FD=_region_malloc(uprev_rgn,sizeof(*_tmp3FD)),((_tmp3FD->hd=(*_tmp21B->name).f2,((_tmp3FD->tl=prev_fields,_tmp3FD))))));}
# 886
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp21B->name).f2)!= 0){
# 887
const char*_tmp401;void*_tmp400[1];struct Cyc_String_pa_PrintArg_struct _tmp3FF;(_tmp3FF.tag=0,((_tmp3FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp21B->name).f2),((_tmp400[0]=& _tmp3FF,Cyc_Tcutil_terr(_tmp21B->loc,((_tmp401="enum field name %s shadows global name",_tag_dyneither(_tmp401,sizeof(char),39))),_tag_dyneither(_tmp400,sizeof(void*),1)))))));}
# 889
if(_tmp21B->tag == 0){
# 890
_tmp21B->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp21B->loc);
# 891
++ tag_count;}else{
# 894
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp21B->tag))){
# 895
unsigned int _tmp224;int _tmp225;struct _tuple11 _tmp223=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp21B->tag));_tmp224=_tmp223.f1;_tmp225=_tmp223.f2;
# 896
if(_tmp225)tag_count=_tmp224 + 1;}}
# 898
(*_tmp21B->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 876
;_pop_region(uprev_rgn);}
# 903
{struct _handler_cons _tmp226;_push_handler(& _tmp226);{int _tmp228=0;if(setjmp(_tmp226.handler))_tmp228=1;if(!_tmp228){
# 904
{struct Cyc_Absyn_Enumdecl**_tmp229=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
# 905
struct Cyc_Absyn_Enumdecl*_tmp22A=Cyc_Tcdecl_merge_enumdecl(*_tmp229,ed,loc,Cyc_Tc_tc_msg);
# 906
if(_tmp22A == 0){_npop_handler(0);return;}
# 907
*_tmp229=(struct Cyc_Absyn_Enumdecl*)_tmp22A;}
# 904
;_pop_handler();}else{void*_tmp227=(void*)_exn_thrown;void*_tmp22C=_tmp227;_LLFC: {struct Cyc_Dict_Absent_exn_struct*_tmp22D=(struct Cyc_Dict_Absent_exn_struct*)_tmp22C;if(_tmp22D->tag != Cyc_Dict_Absent)goto _LLFE;}_LLFD: {
# 909
struct Cyc_Absyn_Enumdecl**_tmp402;struct Cyc_Absyn_Enumdecl**_tmp22E=(_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=ed,_tmp402)));
# 910
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp22E);
# 911
goto _LLFB;}_LLFE:;_LLFF:(void)_throw(_tmp22C);_LLFB:;}};}
# 913
if(ed->fields != 0){
# 914
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 915
struct Cyc_Absyn_Enumfield*_tmp230=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 916
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp408;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp407;struct _tuple13*_tmp406;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp230->name).f2,(struct _tuple13*)(
# 917
(_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406->f1=(void*)((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=((_tmp407.tag=3,((_tmp407.f1=ed,((_tmp407.f2=_tmp230,_tmp407)))))),_tmp408)))),((_tmp406->f2=1,_tmp406)))))));}}
# 922
if(ed->fields != 0){
# 923
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
# 924
struct Cyc_Absyn_Enumfield*_tmp234=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 925
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp234->tag));
# 926
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp234->tag))){
# 927
const char*_tmp40D;void*_tmp40C[2];struct Cyc_String_pa_PrintArg_struct _tmp40B;struct Cyc_String_pa_PrintArg_struct _tmp40A;(_tmp40A.tag=0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp234->name).f2),((_tmp40B.tag=0,((_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp40C[0]=& _tmp40B,((_tmp40C[1]=& _tmp40A,Cyc_Tcutil_terr(loc,((_tmp40D="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp40D,sizeof(char),46))),_tag_dyneither(_tmp40C,sizeof(void*),2)))))))))))));}}}}
# 932
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
# 933
switch(sc){case Cyc_Absyn_Static: _LL100:
# 935
 if(!in_include){
# 936
const char*_tmp410;void*_tmp40F;(_tmp40F=0,Cyc_Tcutil_warn(loc,((_tmp410="static declaration nested within extern \"C\"",_tag_dyneither(_tmp410,sizeof(char),44))),_tag_dyneither(_tmp40F,sizeof(void*),0)));}
# 937
return 0;case Cyc_Absyn_Abstract: _LL101:
# 939
{const char*_tmp413;void*_tmp412;(_tmp412=0,Cyc_Tcutil_warn(loc,((_tmp413="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp413,sizeof(char),46))),_tag_dyneither(_tmp412,sizeof(void*),0)));}
# 940
return 0;case Cyc_Absyn_Public: _LL102:
# 941
 goto _LL103;case Cyc_Absyn_Register: _LL103:
# 942
 goto _LL104;case Cyc_Absyn_Extern: _LL104:
# 943
 return 1;case Cyc_Absyn_ExternC: _LL105:
# 945
{const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_Tcutil_warn(loc,((_tmp416="nested extern \"C\" declaration",_tag_dyneither(_tmp416,sizeof(char),30))),_tag_dyneither(_tmp415,sizeof(void*),0)));}
# 946
return 1;}}
# 950
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
# 951
unsigned int _tmp240;struct _tuple0*_tmp241;struct _tuple12 _tmp23F=*exp;_tmp240=_tmp23F.f1;_tmp241=_tmp23F.f2;{
# 952
struct _tuple0 _tmp243;union Cyc_Absyn_Nmspace _tmp244;struct _dyneither_ptr*_tmp245;struct _tuple0*_tmp242=_tmp241;_tmp243=*_tmp242;_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;{
# 953
union Cyc_Absyn_Nmspace _tmp246=_tmp244;struct Cyc_List_List*_tmp247;_LL108: if((_tmp246.Rel_n).tag != 1)goto _LL10A;_tmp247=(struct Cyc_List_List*)(_tmp246.Rel_n).val;if(_tmp247 != 0)goto _LL10A;_LL109:
# 956
(*_tmp241).f1=Cyc_Absyn_Abs_n(te->ns,1);
# 957
goto _LL107;_LL10A:;_LL10B:
# 958
 goto _LL107;_LL107:;};};}
# 967
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 974
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
# 975
struct Cyc_List_List*last=0;
# 977
struct Cyc_List_List*_tmp248=ds0;for(0;_tmp248 != 0;(last=_tmp248,_tmp248=_tmp248->tl)){
# 978
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp248->hd;
# 979
unsigned int loc=d->loc;
# 981
void*_tmp249=d->r;struct Cyc_Absyn_Vardecl*_tmp24F;struct Cyc_Absyn_Fndecl*_tmp251;struct Cyc_Absyn_Typedefdecl*_tmp253;struct Cyc_Absyn_Aggrdecl*_tmp255;struct Cyc_Absyn_Datatypedecl*_tmp257;struct Cyc_Absyn_Enumdecl*_tmp259;struct _dyneither_ptr*_tmp25D;struct Cyc_List_List*_tmp25E;struct _tuple0*_tmp260;struct _tuple0 _tmp261;union Cyc_Absyn_Nmspace _tmp262;struct _dyneither_ptr*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp269;_LL10D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp24A=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp24A->tag != 2)goto _LL10F;}_LL10E:
# 982
 goto _LL110;_LL10F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp24B=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp24B->tag != 3)goto _LL111;}_LL110:
# 984
{const char*_tmp419;void*_tmp418;(_tmp418=0,Cyc_Tcutil_terr(loc,((_tmp419="top level let-declarations are not implemented",_tag_dyneither(_tmp419,sizeof(char),47))),_tag_dyneither(_tmp418,sizeof(void*),0)));}
# 985
goto _LL10C;_LL111: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp24C=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp24C->tag != 4)goto _LL113;}_LL112:
# 987
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_terr(loc,((_tmp41C="top level region declarations are not implemented",_tag_dyneither(_tmp41C,sizeof(char),50))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}
# 988
goto _LL10C;_LL113: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp24D=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp24D->tag != 5)goto _LL115;}_LL114:
# 990
{const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_Tcutil_terr(loc,((_tmp41F="top level alias declarations are not implemented",_tag_dyneither(_tmp41F,sizeof(char),49))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}
# 991
goto _LL10C;_LL115: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp24E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp24E->tag != 0)goto _LL117;else{_tmp24F=_tmp24E->f1;}}_LL116:
# 993
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp24F->sc,in_externCinclude))
# 994
_tmp24F->sc=Cyc_Absyn_ExternC;
# 995
Cyc_Tc_tcVardecl(te,ge,loc,_tmp24F,check_var_init,in_externCinclude,exports);
# 996
goto _LL10C;_LL117: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp250=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp250->tag != 1)goto _LL119;else{_tmp251=_tmp250->f1;}}_LL118:
# 998
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp251->sc,in_externCinclude))
# 999
_tmp251->sc=Cyc_Absyn_ExternC;
# 1000
Cyc_Tc_tcFndecl(te,ge,loc,_tmp251,in_externCinclude,exports);
# 1001
goto _LL10C;_LL119: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp252=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp252->tag != 9)goto _LL11B;else{_tmp253=_tmp252->f1;}}_LL11A:
# 1003
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp253);
# 1004
goto _LL10C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp254=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp254->tag != 6)goto _LL11D;else{_tmp255=_tmp254->f1;}}_LL11C:
# 1006
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp255->sc,in_externCinclude))
# 1007
_tmp255->sc=Cyc_Absyn_ExternC;
# 1008
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp255);
# 1009
goto _LL10C;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp256=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp256->tag != 7)goto _LL11F;else{_tmp257=_tmp256->f1;}}_LL11E:
# 1011
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp257->sc,in_externCinclude))
# 1012
_tmp257->sc=Cyc_Absyn_ExternC;
# 1013
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp257);
# 1014
goto _LL10C;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp258=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp258->tag != 8)goto _LL121;else{_tmp259=_tmp258->f1;}}_LL120:
# 1016
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp259->sc,in_externCinclude))
# 1017
_tmp259->sc=Cyc_Absyn_ExternC;
# 1018
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp259);
# 1019
goto _LL10C;_LL121: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp25A=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp25A->tag != 14)goto _LL123;}_LL122:
# 1021
{const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_Tcutil_warn(d->loc,((_tmp422="spurious __cyclone_port_on__",_tag_dyneither(_tmp422,sizeof(char),29))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
# 1022
goto _LL10C;_LL123: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp25B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp25B->tag != 15)goto _LL125;}_LL124:
# 1024
 goto _LL10C;_LL125: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp25C=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp25C->tag != 10)goto _LL127;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}_LL126: {
# 1027
struct Cyc_List_List*_tmp272=te->ns;
# 1028
struct Cyc_List_List*_tmp423;struct Cyc_List_List*_tmp273=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp272,((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->hd=_tmp25D,((_tmp423->tl=0,_tmp423)))))));
# 1031
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp25D)){
# 1032
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp25D);
# 1033
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp273,Cyc_Tcenv_empty_genv(grgn));}
# 1036
te->ns=_tmp273;
# 1037
Cyc_Tc_tc_decls(te,_tmp25E,in_externC,in_externCinclude,check_var_init,grgn,exports);
# 1038
te->ns=_tmp272;
# 1039
goto _LL10C;}_LL127: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp25F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp25F->tag != 11)goto _LL129;else{_tmp260=_tmp25F->f1;_tmp261=*_tmp260;_tmp262=_tmp261.f1;_tmp263=_tmp261.f2;_tmp264=_tmp25F->f2;}}_LL128: {
# 1043
struct _dyneither_ptr*first;
# 1044
struct Cyc_List_List*rest;
# 1045
{union Cyc_Absyn_Nmspace _tmp275=_tmp262;int _tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List _tmp27B;struct _dyneither_ptr*_tmp27C;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27E;struct Cyc_List_List _tmp27F;struct _dyneither_ptr*_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_List_List _tmp283;struct _dyneither_ptr*_tmp284;struct Cyc_List_List*_tmp285;_LL12E: if((_tmp275.Loc_n).tag != 4)goto _LL130;_tmp276=(int)(_tmp275.Loc_n).val;_LL12F:
# 1046
 goto _LL131;_LL130: if((_tmp275.Rel_n).tag != 1)goto _LL132;_tmp277=(struct Cyc_List_List*)(_tmp275.Rel_n).val;if(_tmp277 != 0)goto _LL132;_LL131:
# 1047
 goto _LL133;_LL132: if((_tmp275.C_n).tag != 3)goto _LL134;_tmp278=(struct Cyc_List_List*)(_tmp275.C_n).val;if(_tmp278 != 0)goto _LL134;_LL133:
# 1048
 goto _LL135;_LL134: if((_tmp275.Abs_n).tag != 2)goto _LL136;_tmp279=(struct Cyc_List_List*)(_tmp275.Abs_n).val;if(_tmp279 != 0)goto _LL136;_LL135:
# 1050
 first=_tmp263;rest=0;goto _LL12D;_LL136: if((_tmp275.Rel_n).tag != 1)goto _LL138;_tmp27A=(struct Cyc_List_List*)(_tmp275.Rel_n).val;if(_tmp27A == 0)goto _LL138;_tmp27B=*_tmp27A;_tmp27C=(struct _dyneither_ptr*)_tmp27B.hd;_tmp27D=_tmp27B.tl;_LL137:
# 1051
 _tmp280=_tmp27C;_tmp281=_tmp27D;goto _LL139;_LL138: if((_tmp275.C_n).tag != 3)goto _LL13A;_tmp27E=(struct Cyc_List_List*)(_tmp275.C_n).val;if(_tmp27E == 0)goto _LL13A;_tmp27F=*_tmp27E;_tmp280=(struct _dyneither_ptr*)_tmp27F.hd;_tmp281=_tmp27F.tl;_LL139:
# 1052
 _tmp284=_tmp280;_tmp285=_tmp281;goto _LL13B;_LL13A: if((_tmp275.Abs_n).tag != 2)goto _LL12D;_tmp282=(struct Cyc_List_List*)(_tmp275.Abs_n).val;if(_tmp282 == 0)goto _LL12D;_tmp283=*_tmp282;_tmp284=(struct _dyneither_ptr*)_tmp283.hd;_tmp285=_tmp283.tl;_LL13B:
# 1054
 first=_tmp284;{struct Cyc_List_List*_tmp424;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp285,((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->hd=_tmp263,((_tmp424->tl=0,_tmp424)))))));}goto _LL12D;_LL12D:;}{
# 1056
struct Cyc_List_List*_tmp287=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
# 1057
{struct Cyc_List_List*_tmp425;ge->availables=(struct Cyc_List_List*)((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->hd=_tmp287,((_tmp425->tl=ge->availables,_tmp425))))));}
# 1058
Cyc_Tc_tc_decls(te,_tmp264,in_externC,in_externCinclude,check_var_init,grgn,exports);
# 1059
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
# 1060
goto _LL10C;};}_LL129: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp265=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp265->tag != 12)goto _LL12B;else{_tmp266=_tmp265->f1;}}_LL12A:
# 1063
 Cyc_Tc_tc_decls(te,_tmp266,1,in_externCinclude,check_var_init,grgn,exports);
# 1064
goto _LL10C;_LL12B: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp267=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp249;if(_tmp267->tag != 13)goto _LL10C;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}_LL12C:
# 1067
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp269);{
# 1068
struct Cyc_List_List*newexs=
# 1069
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp269,(unsigned int)exports?*exports: 0);
# 1070
Cyc_Tc_tc_decls(te,_tmp268,1,1,check_var_init,grgn,(struct Cyc_List_List**)& newexs);
# 1072
for(0;_tmp269 != 0;_tmp269=_tmp269->tl){
# 1073
struct _tuple12*_tmp289=(struct _tuple12*)_tmp269->hd;
# 1074
if(!(*_tmp289).f3){
# 1075
const char*_tmp429;void*_tmp428[1];struct Cyc_String_pa_PrintArg_struct _tmp427;(_tmp427.tag=0,((_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1076
Cyc_Absynpp_qvar2string((*_tmp289).f2)),((_tmp428[0]=& _tmp427,Cyc_Tcutil_warn((*_tmp289).f1,((_tmp429="%s is exported but not defined",_tag_dyneither(_tmp429,sizeof(char),31))),_tag_dyneither(_tmp428,sizeof(void*),1)))))));}}
# 1078
goto _LL10C;};_LL10C:;}}
# 1083
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1085
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
# 1086
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1102 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1104
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1106
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
# 1107
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1110
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
# 1111
void*_tmp28D=d->r;struct Cyc_Absyn_Vardecl*_tmp28F;struct Cyc_List_List*_tmp291;struct Cyc_List_List**_tmp292;struct Cyc_List_List*_tmp294;struct Cyc_List_List**_tmp295;struct Cyc_List_List*_tmp297;struct Cyc_List_List**_tmp298;struct Cyc_List_List*_tmp29A;struct Cyc_List_List**_tmp29B;_LL13D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28E=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp28D;if(_tmp28E->tag != 0)goto _LL13F;else{_tmp28F=_tmp28E->f1;}}_LL13E:
# 1115
 if((env->in_cinclude  || _tmp28F->sc != Cyc_Absyn_Extern  && _tmp28F->sc != Cyc_Absyn_ExternC) || !
# 1116
Cyc_Absyn_qvar_cmp(_tmp28F->name,Cyc_Absyn_uniquergn_qvar))
# 1118
return 1;{
# 1120
struct _tuple0 _tmp29D;union Cyc_Absyn_Nmspace _tmp29E;struct _dyneither_ptr*_tmp29F;struct _tuple0*_tmp29C=_tmp28F->name;_tmp29D=*_tmp29C;_tmp29E=_tmp29D.f1;_tmp29F=_tmp29D.f2;{
# 1121
struct Cyc_List_List*ns;
# 1122
{union Cyc_Absyn_Nmspace _tmp2A0=_tmp29E;int _tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*_tmp2A4;_LL14A: if((_tmp2A0.Loc_n).tag != 4)goto _LL14C;_tmp2A1=(int)(_tmp2A0.Loc_n).val;_LL14B:
# 1123
 ns=0;goto _LL149;_LL14C: if((_tmp2A0.Rel_n).tag != 1)goto _LL14E;_tmp2A2=(struct Cyc_List_List*)(_tmp2A0.Rel_n).val;_LL14D:
# 1124
 ns=_tmp2A2;goto _LL149;_LL14E: if((_tmp2A0.C_n).tag != 3)goto _LL150;_tmp2A3=(struct Cyc_List_List*)(_tmp2A0.C_n).val;_LL14F:
# 1125
 ns=_tmp2A3;goto _LL149;_LL150: if((_tmp2A0.Abs_n).tag != 2)goto _LL149;_tmp2A4=(struct Cyc_List_List*)(_tmp2A0.Abs_n).val;_LL151:
# 1126
 ns=_tmp2A4;goto _LL149;_LL149:;}{
# 1128
struct _tuple17*_tmp2A5=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
# 1129
struct Cyc_Tcenv_Genv*_tmp2A6=(*_tmp2A5).f1;
# 1130
int _tmp2A7=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2A6->ordinaries,_tmp29F)).f2;
# 1131
if(!_tmp2A7)
# 1132
(*_tmp2A5).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp2A5).f2,_tmp29F);
# 1136
return _tmp2A7;};};};_LL13F: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp290=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp28D;if(_tmp290->tag != 12)goto _LL141;else{_tmp291=_tmp290->f1;_tmp292=(struct Cyc_List_List**)& _tmp290->f1;}}_LL140:
# 1137
 _tmp295=_tmp292;goto _LL142;_LL141: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp293=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp28D;if(_tmp293->tag != 11)goto _LL143;else{_tmp294=_tmp293->f2;_tmp295=(struct Cyc_List_List**)& _tmp293->f2;}}_LL142:
# 1138
 _tmp298=_tmp295;goto _LL144;_LL143: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp296=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp28D;if(_tmp296->tag != 10)goto _LL145;else{_tmp297=_tmp296->f2;_tmp298=(struct Cyc_List_List**)& _tmp296->f2;}}_LL144:
# 1140
*_tmp298=Cyc_Tc_treeshake_f(env,*_tmp298);
# 1141
return 1;_LL145: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp299=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp28D;if(_tmp299->tag != 13)goto _LL147;else{_tmp29A=_tmp299->f1;_tmp29B=(struct Cyc_List_List**)& _tmp299->f1;}}_LL146: {
# 1143
int in_cinclude=env->in_cinclude;
# 1144
env->in_cinclude=1;
# 1145
*_tmp29B=Cyc_Tc_treeshake_f(env,*_tmp29B);
# 1146
env->in_cinclude=in_cinclude;
# 1147
return 1;}_LL147:;_LL148:
# 1148
 return 1;_LL13C:;}
# 1152
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
# 1153
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1156
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1158
struct _tuple17*_tmp42A;return(_tmp42A=_region_malloc(rgn,sizeof(*_tmp42A)),((_tmp42A->f1=ge,((_tmp42A->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp42A)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1161
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
# 1162
struct _RegionHandle _tmp2A9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A9;_push_region(rgn);
# 1163
{struct Cyc_Tc_TreeshakeEnv _tmp42B;struct Cyc_Tc_TreeshakeEnv _tmp2AA=(_tmp42B.rgn=rgn,((_tmp42B.in_cinclude=0,((_tmp42B.nsdict=
# 1164
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp42B)))));
# 1165
struct Cyc_List_List*_tmp2AB=Cyc_Tc_treeshake_f(& _tmp2AA,ds);
# 1166
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp2AA.nsdict)){
# 1167
struct Cyc_List_List*_tmp2AC=_tmp2AB;_npop_handler(0);return _tmp2AC;}{
# 1169
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp2AA.nsdict);
# 1170
struct _tuple18 _tmp2AD=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp2AA.nsdict);
# 1171
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp2AD)){
# 1172
struct _tuple17*_tmp2AF;struct _tuple17 _tmp2B0;struct Cyc_Tcenv_Genv*_tmp2B1;struct Cyc_Set_Set*_tmp2B2;struct _tuple18 _tmp2AE=_tmp2AD;_tmp2AF=_tmp2AE.f2;_tmp2B0=*_tmp2AF;_tmp2B1=_tmp2B0.f1;_tmp2B2=_tmp2B0.f2;
# 1173
_tmp2B1->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp2B1->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp2B2,_tmp2B1->ordinaries);}{
# 1175
struct Cyc_List_List*_tmp2B3=_tmp2AB;_npop_handler(0);return _tmp2B3;};};}
# 1163
;_pop_region(rgn);}
