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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 123
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 307
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 310
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 382
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
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
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 907
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 935
void*Cyc_Absyn_compress_kb(void*);
# 940
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 964
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 986
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 997
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1068
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1154
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
# 115 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 119
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 124
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 186
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 214
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
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
# 95
int Cyc_Tcutil_is_function_type(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 180
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 210 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 212
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 220
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 224
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 301
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 312
int Cyc_Tcutil_bits_only(void*t);
# 315
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 322
int Cyc_Tcutil_supports_default(void*);
# 333
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 337
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 344
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
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
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);static char _tmp0[1]="";
# 46 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 49
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 55
(*_tmp1).f3=1;
return 1;}}
# 59
return 0;}
# 62
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmp2=_new_region("temp");struct _RegionHandle*temp=& _tmp2;_push_region(temp);
{struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
void*_tmp3=Cyc_Tcutil_compress(t);struct Cyc_List_List*_tmp5;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3;if(_tmp4->tag != 9)goto _LL3;else{_tmp5=(_tmp4->f1).attributes;}}_LL2:
# 69
 for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
void*_tmp6=(void*)_tmp5->hd;int _tmp8;int _tmpA;int _tmpC;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp7=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6;if(_tmp7->tag != 20)goto _LL8;else{_tmp8=_tmp7->f1;}}_LL7:
# 72
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmp8)){
struct Cyc_Int_pa_PrintArg_struct _tmp245;void*_tmp244[1];const char*_tmp243;void*_tmp242;(_tmp242=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp245.tag=1,((_tmp245.f1=(unsigned long)_tmp8,((_tmp244[0]=& _tmp245,Cyc_rprintf(temp,((_tmp243="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp243,sizeof(char),73))),_tag_dyneither(_tmp244,sizeof(void*),1)))))))),_tag_dyneither(_tmp242,sizeof(void*),0)));}
# 76
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmp8)){
struct Cyc_Int_pa_PrintArg_struct _tmp24D;void*_tmp24C[1];const char*_tmp24B;void*_tmp24A;(_tmp24A=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp24D.tag=1,((_tmp24D.f1=(unsigned long)_tmp8,((_tmp24C[0]=& _tmp24D,Cyc_rprintf(temp,((_tmp24B="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp24B,sizeof(char),70))),_tag_dyneither(_tmp24C,sizeof(void*),1)))))))),_tag_dyneither(_tmp24A,sizeof(void*),0)));}
# 79
{struct Cyc_List_List*_tmp24E;init_params=((_tmp24E=_region_malloc(temp,sizeof(*_tmp24E)),((_tmp24E->hd=(void*)_tmp8,((_tmp24E->tl=init_params,_tmp24E))))));}goto _LL5;_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp9=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6;if(_tmp9->tag != 21)goto _LLA;else{_tmpA=_tmp9->f1;}}_LL9:
# 81
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpA)){
struct Cyc_Int_pa_PrintArg_struct _tmp256;void*_tmp255[1];const char*_tmp254;void*_tmp253;(_tmp253=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp256.tag=1,((_tmp256.f1=(unsigned long)_tmpA,((_tmp255[0]=& _tmp256,Cyc_rprintf(temp,((_tmp254="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp254,sizeof(char),73))),_tag_dyneither(_tmp255,sizeof(void*),1)))))))),_tag_dyneither(_tmp253,sizeof(void*),0)));}
# 85
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmpA)){
struct Cyc_Int_pa_PrintArg_struct _tmp25E;void*_tmp25D[1];const char*_tmp25C;void*_tmp25B;(_tmp25B=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp25E.tag=1,((_tmp25E.f1=(unsigned long)_tmpA,((_tmp25D[0]=& _tmp25E,Cyc_rprintf(temp,((_tmp25C="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp25C,sizeof(char),71))),_tag_dyneither(_tmp25D,sizeof(void*),1)))))))),_tag_dyneither(_tmp25B,sizeof(void*),0)));}
# 88
{struct Cyc_List_List*_tmp25F;nolive_unique_params=((_tmp25F=_region_malloc(temp,sizeof(*_tmp25F)),((_tmp25F->hd=(void*)_tmpA,((_tmp25F->tl=nolive_unique_params,_tmp25F))))));}goto _LL5;_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpB=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6;if(_tmpB->tag != 22)goto _LLC;else{_tmpC=_tmpB->f1;}}_LLB:
# 90
 if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpC)){
struct Cyc_Int_pa_PrintArg_struct _tmp267;void*_tmp266[1];const char*_tmp265;void*_tmp264;(_tmp264=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp267.tag=1,((_tmp267.f1=(unsigned long)_tmpC,((_tmp266[0]=& _tmp267,Cyc_rprintf(temp,((_tmp265="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp265,sizeof(char),70))),_tag_dyneither(_tmp266,sizeof(void*),1)))))))),_tag_dyneither(_tmp264,sizeof(void*),0)));}
# 94
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmpC)){
struct Cyc_Int_pa_PrintArg_struct _tmp26F;void*_tmp26E[1];const char*_tmp26D;void*_tmp26C;(_tmp26C=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp26F.tag=1,((_tmp26F.f1=(unsigned long)_tmpC,((_tmp26E[0]=& _tmp26F,Cyc_rprintf(temp,((_tmp26D="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp26D,sizeof(char),71))),_tag_dyneither(_tmp26E,sizeof(void*),1)))))))),_tag_dyneither(_tmp26C,sizeof(void*),0)));}
# 97
{struct Cyc_List_List*_tmp270;noconsume_params=((_tmp270=_region_malloc(temp,sizeof(*_tmp270)),((_tmp270->hd=(void*)_tmpC,((_tmp270->tl=noconsume_params,_tmp270))))));}goto _LL5;_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 101
goto _LL0;_LL3:;_LL4: {
# 103
const char*_tmp273;void*_tmp272;(_tmp272=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp273="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp273,sizeof(char),34))),_tag_dyneither(_tmp272,sizeof(void*),0)));}_LL0:;}
# 64
;_pop_region(temp);}struct _tuple13{void*f1;int f2;};
# 108
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 112
enum Cyc_Absyn_Scope _tmp2B;union Cyc_Absyn_Nmspace _tmp2C;struct _dyneither_ptr*_tmp2D;void*_tmp2E;struct Cyc_Absyn_Exp*_tmp2F;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Vardecl*_tmp2A=vd;_tmp2B=_tmp2A->sc;_tmp2C=(_tmp2A->name)->f1;_tmp2D=(_tmp2A->name)->f2;_tmp2E=_tmp2A->type;_tmp2F=_tmp2A->initializer;_tmp30=_tmp2A->attributes;
# 118
{union Cyc_Absyn_Nmspace _tmp31=_tmp2C;_LLF: if((_tmp31.Rel_n).tag != 1)goto _LL11;_LL10:
# 121
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LLE;_LL11:;_LL12:
 goto _LLE;_LLE:;}
# 127
{void*_tmp32=Cyc_Tcutil_compress(_tmp2E);void*_tmp34;struct Cyc_Absyn_Tqual _tmp35;union Cyc_Absyn_Constraint*_tmp36;unsigned int _tmp37;_LL14:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp33=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32;if(_tmp33->tag != 8)goto _LL16;else{_tmp34=(_tmp33->f1).elt_type;_tmp35=(_tmp33->f1).tq;if((_tmp33->f1).num_elts != 0)goto _LL16;_tmp36=(_tmp33->f1).zero_term;_tmp37=(_tmp33->f1).zt_loc;}}if(!(_tmp2F != 0))goto _LL16;_LL15:
# 129
{void*_tmp38=_tmp2F->r;struct _dyneither_ptr _tmp3A;struct _dyneither_ptr _tmp3C;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp44;_LL19: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp39=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp39->tag != 0)goto _LL1B;else{if(((_tmp39->f1).String_c).tag != 8)goto _LL1B;_tmp3A=(struct _dyneither_ptr)((_tmp39->f1).String_c).val;}}_LL1A:
# 131
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp3A,sizeof(char)),0),_tmp36,_tmp37));
goto _LL18;_LL1B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3B->tag != 0)goto _LL1D;else{if(((_tmp3B->f1).Wstring_c).tag != 9)goto _LL1D;_tmp3C=(struct _dyneither_ptr)((_tmp3B->f1).Wstring_c).val;}}_LL1C:
# 134
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp(1,0),_tmp36,_tmp37));
goto _LL18;_LL1D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3D->tag != 26)goto _LL1F;else{_tmp3E=_tmp3D->f2;}}_LL1E:
 _tmp40=_tmp3E;goto _LL20;_LL1F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp3F->tag != 27)goto _LL21;else{_tmp40=_tmp3F->f1;}}_LL20:
# 139
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,_tmp40,_tmp36,_tmp37));
goto _LL18;_LL21: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp41=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp41->tag != 35)goto _LL23;else{_tmp42=_tmp41->f2;}}_LL22:
 _tmp44=_tmp42;goto _LL24;_LL23: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp43=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38;if(_tmp43->tag != 25)goto _LL25;else{_tmp44=_tmp43->f1;}}_LL24:
# 143
 _tmp2E=(vd->type=Cyc_Absyn_array_typ(_tmp34,_tmp35,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp44),0),_tmp36,_tmp37));
goto _LL18;_LL25:;_LL26:
 goto _LL18;_LL18:;}
# 147
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 151
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp2E);
# 153
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp2E);
# 156
{void*_tmp45=Cyc_Tcutil_compress(_tmp2E);_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45;if(_tmp46->tag != 8)goto _LL2A;}_LL29:
 vd->escapes=0;goto _LL27;_LL2A:;_LL2B:
 vd->escapes=1;goto _LL27;_LL27:;}
# 161
if(Cyc_Tcutil_is_function_type(_tmp2E)){
_tmp30=Cyc_Tcutil_transfer_fn_type_atts(_tmp2E,_tmp30);
Cyc_Tc_fnTypeAttsOK(te,_tmp2E,loc);}
# 166
if(_tmp2B == Cyc_Absyn_Extern  || _tmp2B == Cyc_Absyn_ExternC){
if(_tmp2F != 0){
const char*_tmp276;void*_tmp275;(_tmp275=0,Cyc_Tcutil_terr(loc,((_tmp276="extern declaration should not have initializer",_tag_dyneither(_tmp276,sizeof(char),47))),_tag_dyneither(_tmp275,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp2E)){
# 173
for(0;_tmp30 != 0;_tmp30=_tmp30->tl){
void*_tmp49=(void*)_tmp30->hd;_LL2D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp4A=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4A->tag != 6)goto _LL2F;}_LL2E:
 goto _LL30;_LL2F: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp4B=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4B->tag != 8)goto _LL31;}_LL30:
# 178
 goto _LL32;_LL31: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp4C=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4C->tag != 9)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp4D=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4D->tag != 10)goto _LL35;}_LL34:
 goto _LL36;_LL35: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp4E=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4E->tag != 11)goto _LL37;}_LL36:
 goto _LL38;_LL37: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp4F=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp49;if(_tmp4F->tag != 12)goto _LL39;}_LL38:
 goto _LL3A;_LL39: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp50=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp49;if(_tmp50->tag != 13)goto _LL3B;}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp51=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp49;if(_tmp51->tag != 14)goto _LL3D;}_LL3C:
 continue;_LL3D:;_LL3E:
# 186
{const char*_tmp27B;void*_tmp27A[2];struct Cyc_String_pa_PrintArg_struct _tmp279;struct Cyc_String_pa_PrintArg_struct _tmp278;(_tmp278.tag=0,((_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp279.tag=0,((_tmp279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp30->hd)),((_tmp27A[0]=& _tmp279,((_tmp27A[1]=& _tmp278,Cyc_Tcutil_terr(loc,((_tmp27B="bad attribute %s for variable %s",_tag_dyneither(_tmp27B,sizeof(char),33))),_tag_dyneither(_tmp27A,sizeof(void*),2)))))))))))));}
goto _LL2C;_LL2C:;}
# 191
if(_tmp2F == 0  || in_cinclude){
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp2E)){
const char*_tmp280;void*_tmp27F[2];struct Cyc_String_pa_PrintArg_struct _tmp27E;struct Cyc_String_pa_PrintArg_struct _tmp27D;(_tmp27D.tag=0,((_tmp27D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2E)),((_tmp27E.tag=0,((_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp27F[0]=& _tmp27E,((_tmp27F[1]=& _tmp27D,Cyc_Tcutil_terr(loc,((_tmp280="initializer required for variable %s of type %s",_tag_dyneither(_tmp280,sizeof(char),48))),_tag_dyneither(_tmp27F,sizeof(void*),2)))))))))))));}}else{
# 196
struct Cyc_Absyn_Exp*_tmp5A=_tmp2F;
# 198
void*_tmp5B=Cyc_Tcexp_tcExpInitializer(te,& _tmp2E,_tmp5A);
if(!Cyc_Tcutil_coerce_assign(te,_tmp5A,_tmp2E)){
{const char*_tmp286;void*_tmp285[3];struct Cyc_String_pa_PrintArg_struct _tmp284;struct Cyc_String_pa_PrintArg_struct _tmp283;struct Cyc_String_pa_PrintArg_struct _tmp282;(_tmp282.tag=0,((_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp5B)),((_tmp283.tag=0,((_tmp283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E)),((_tmp284.tag=0,((_tmp284.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp285[0]=& _tmp284,((_tmp285[1]=& _tmp283,((_tmp285[2]=& _tmp282,Cyc_Tcutil_terr(loc,((_tmp286="%s declared with type \n%s\n but initialized with type \n%s",_tag_dyneither(_tmp286,sizeof(char),57))),_tag_dyneither(_tmp285,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}
# 205
if(!Cyc_Tcutil_is_const_exp(_tmp5A)){
const char*_tmp289;void*_tmp288;(_tmp288=0,Cyc_Tcutil_terr(loc,((_tmp289="initializer is not a constant expression",_tag_dyneither(_tmp289,sizeof(char),41))),_tag_dyneither(_tmp288,sizeof(void*),0)));}}}else{
# 210
for(0;_tmp30 != 0;_tmp30=_tmp30->tl){
void*_tmp63=(void*)_tmp30->hd;_LL40: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp64=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp63;if(_tmp64->tag != 0)goto _LL42;}_LL41:
# 213
 goto _LL43;_LL42: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp65=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp63;if(_tmp65->tag != 1)goto _LL44;}_LL43:
 goto _LL45;_LL44: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp66=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp63;if(_tmp66->tag != 2)goto _LL46;}_LL45:
 goto _LL47;_LL46: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp67=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp63;if(_tmp67->tag != 3)goto _LL48;}_LL47:
 goto _LL49;_LL48: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp68=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp63;if(_tmp68->tag != 4)goto _LL4A;}_LL49:
 goto _LL4B;_LL4A: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp69=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp63;if(_tmp69->tag != 19)goto _LL4C;}_LL4B:
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6A=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp63;if(_tmp6A->tag != 20)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp63;if(_tmp6B->tag != 23)goto _LL50;}_LL4F:
 goto _LL51;_LL50: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp6C=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp63;if(_tmp6C->tag != 5)goto _LL52;}_LL51: {
const char*_tmp28C;void*_tmp28B;(_tmp28B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp28C="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp28C,sizeof(char),45))),_tag_dyneither(_tmp28B,sizeof(void*),0)));}_LL52: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp6D=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp63;if(_tmp6D->tag != 6)goto _LL54;}_LL53:
# 223
 goto _LL55;_LL54: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp6E=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp63;if(_tmp6E->tag != 7)goto _LL56;}_LL55:
# 225
{const char*_tmp290;void*_tmp28F[1];struct Cyc_String_pa_PrintArg_struct _tmp28E;(_tmp28E.tag=0,((_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp30->hd)),((_tmp28F[0]=& _tmp28E,Cyc_Tcutil_terr(loc,((_tmp290="bad attribute %s in function declaration",_tag_dyneither(_tmp290,sizeof(char),41))),_tag_dyneither(_tmp28F,sizeof(void*),1)))))));}
goto _LL3F;_LL56:;_LL57:
 continue;_LL3F:;}}}
# 233
{struct _handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))_tmp76=1;if(!_tmp76){
{struct _tuple13*_tmp77=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp2D);
void*_tmp78=(*_tmp77).f1;void*_tmp7A;_LL59: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp79=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp78;if(_tmp79->tag != 0)goto _LL5B;else{_tmp7A=(void*)_tmp79->f1;}}_LL5A: {
# 237
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp293;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp292;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp7F=(_tmp292=_cycalloc(sizeof(*_tmp292)),((_tmp292[0]=((_tmp293.tag=1,((_tmp293.f1=vd,_tmp293)))),_tmp292)));
void*_tmp80=Cyc_Tcdecl_merge_binding(_tmp7A,(void*)_tmp7F,loc,Cyc_Tc_tc_msg);
if(_tmp80 == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 241
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp80 != _tmp7A  || (*_tmp77).f2){
# 244
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp299;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp298;struct _tuple13*_tmp297;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp2D,(
(_tmp297=_cycalloc(sizeof(*_tmp297)),((_tmp297->f1=(void*)((_tmp299=_cycalloc(sizeof(*_tmp299)),((_tmp299[0]=((_tmp298.tag=0,((_tmp298.f1=_tmp80,_tmp298)))),_tmp299)))),((_tmp297->f2=1,_tmp297)))))));}}
_npop_handler(0);return;}_LL5B: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp7B=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp78;if(_tmp7B->tag != 1)goto _LL5D;}_LL5C:
# 248
{const char*_tmp29C;void*_tmp29B;(_tmp29B=0,Cyc_Tcutil_warn(loc,((_tmp29C="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp29C,sizeof(char),57))),_tag_dyneither(_tmp29B,sizeof(void*),0)));}
goto _LL58;_LL5D: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp7C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp78;if(_tmp7C->tag != 2)goto _LL5F;}_LL5E:
# 251
{const char*_tmp29F;void*_tmp29E;(_tmp29E=0,Cyc_Tcutil_warn(loc,((_tmp29F="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp29F,sizeof(char),59))),_tag_dyneither(_tmp29E,sizeof(void*),0)));}
goto _LL58;_LL5F: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp7D=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp78;if(_tmp7D->tag != 4)goto _LL61;}_LL60:
 goto _LL62;_LL61: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp7E=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp78;if(_tmp7E->tag != 3)goto _LL58;}_LL62:
# 255
{const char*_tmp2A2;void*_tmp2A1;(_tmp2A1=0,Cyc_Tcutil_warn(loc,((_tmp2A2="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2A2,sizeof(char),47))),_tag_dyneither(_tmp2A1,sizeof(void*),0)));}
goto _LL58;_LL58:;}
# 234
;_pop_handler();}else{void*_tmp75=(void*)_exn_thrown;void*_tmp8D=_tmp75;void*_tmp8F;_LL64: {struct Cyc_Dict_Absent_exn_struct*_tmp8E=(struct Cyc_Dict_Absent_exn_struct*)_tmp8D;if(_tmp8E->tag != Cyc_Dict_Absent)goto _LL66;}_LL65:
# 258
 goto _LL63;_LL66: _tmp8F=_tmp8D;_LL67:(void)_throw(_tmp8F);_LL63:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2B1;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2B0;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2AF;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2AE;struct _tuple13*_tmp2AD;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp2D,(
(_tmp2AD=_cycalloc(sizeof(*_tmp2AD)),((_tmp2AD->f1=(void*)((_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((_tmp2B1[0]=((_tmp2AE.tag=0,((_tmp2AE.f1=(void*)((_tmp2B0=_cycalloc(sizeof(*_tmp2B0)),((_tmp2B0[0]=((_tmp2AF.tag=1,((_tmp2AF.f1=vd,_tmp2AF)))),_tmp2B0)))),_tmp2AE)))),_tmp2B1)))),((_tmp2AD->f2=0,_tmp2AD)))))));}}
# 264
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmp96;struct _dyneither_ptr*_tmp97;struct _tuple0*_tmp95=n;_tmp96=_tmp95->f1;_tmp97=_tmp95->f2;{
union Cyc_Absyn_Nmspace _tmp98=_tmp96;_LL69: if((_tmp98.Abs_n).tag != 2)goto _LL6B;if((struct Cyc_List_List*)(_tmp98.Abs_n).val != 0)goto _LL6B;_LL6A: {
# 268
const char*_tmp2B2;return Cyc_strcmp((struct _dyneither_ptr)*_tmp97,((_tmp2B2="main",_tag_dyneither(_tmp2B2,sizeof(char),5))))== 0;}_LL6B:;_LL6C:
 return 0;_LL68:;};}
# 273
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 276
struct _dyneither_ptr*v=(*fd->name).f2;
# 280
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp2B5;void*_tmp2B4;(_tmp2B4=0,Cyc_Tcutil_terr(loc,((_tmp2B5="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp2B5,sizeof(char),54))),_tag_dyneither(_tmp2B4,sizeof(void*),0)));}
# 283
{union Cyc_Absyn_Nmspace _tmp9C=(*fd->name).f1;_LL6E: if((_tmp9C.Rel_n).tag != 1)goto _LL70;if((struct Cyc_List_List*)(_tmp9C.Rel_n).val != 0)goto _LL70;_LL6F:
# 286
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL6D;_LL70:;_LL71:
# 289
 goto _LL6D;_LL6D:;}
# 292
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 294
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 296
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 299
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9D=(void*)atts->hd;_LL73: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9E=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9E->tag != 7)goto _LL75;}_LL74:
 goto _LL76;_LL75: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9F=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9F->tag != 6)goto _LL77;}_LL76:
# 303
{const char*_tmp2B9;void*_tmp2B8[1];struct Cyc_String_pa_PrintArg_struct _tmp2B7;(_tmp2B7.tag=0,((_tmp2B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp2B8[0]=& _tmp2B7,Cyc_Tcutil_terr(loc,((_tmp2B9="bad attribute %s for function",_tag_dyneither(_tmp2B9,sizeof(char),30))),_tag_dyneither(_tmp2B8,sizeof(void*),1)))))));}
goto _LL72;_LL77:;_LL78:
 goto _LL72;_LL72:;}}
# 309
{struct _handler_cons _tmpA3;_push_handler(& _tmpA3);{int _tmpA5=0;if(setjmp(_tmpA3.handler))_tmpA5=1;if(!_tmpA5){
{struct _tuple13*_tmpA6=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpA7=(*_tmpA6).f1;void*_tmpA9;_LL7A: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpA8->tag != 0)goto _LL7C;else{_tmpA9=(void*)_tmpA8->f1;}}_LL7B: {
# 313
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2BC;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2BB;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAE=(_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((_tmp2BB[0]=((_tmp2BC.tag=2,((_tmp2BC.f1=fd,_tmp2BC)))),_tmp2BB)));
void*_tmpAF=Cyc_Tcdecl_merge_binding(_tmpA9,(void*)_tmpAE,loc,Cyc_Tc_tc_msg);
if(_tmpAF == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL79;
# 317
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpAF == _tmpA9  && (*_tmpA6).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2C2;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2C1;struct _tuple13*_tmp2C0;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp2C0=_cycalloc(sizeof(*_tmp2C0)),((_tmp2C0->f1=(void*)((_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C1.tag=0,((_tmp2C1.f1=_tmpAF,_tmp2C1)))),_tmp2C2)))),((_tmp2C0->f2=1,_tmp2C0)))))));}}
goto _LL79;}_LL7C: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpAA=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAA->tag != 1)goto _LL7E;}_LL7D:
# 323
{const char*_tmp2C5;void*_tmp2C4;(_tmp2C4=0,Cyc_Tcutil_warn(loc,((_tmp2C5="function declaration shadows previous type declaration",_tag_dyneither(_tmp2C5,sizeof(char),55))),_tag_dyneither(_tmp2C4,sizeof(void*),0)));}
goto _LL79;_LL7E: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAB->tag != 2)goto _LL80;}_LL7F:
# 326
{const char*_tmp2C8;void*_tmp2C7;(_tmp2C7=0,Cyc_Tcutil_warn(loc,((_tmp2C8="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp2C8,sizeof(char),59))),_tag_dyneither(_tmp2C7,sizeof(void*),0)));}
goto _LL79;_LL80: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAC=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAC->tag != 4)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAD=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA7;if(_tmpAD->tag != 3)goto _LL79;}_LL83:
# 330
{const char*_tmp2CB;void*_tmp2CA;(_tmp2CA=0,Cyc_Tcutil_warn(loc,((_tmp2CB="function declaration shadows previous enum tag",_tag_dyneither(_tmp2CB,sizeof(char),47))),_tag_dyneither(_tmp2CA,sizeof(void*),0)));}
goto _LL79;_LL79:;}
# 310
;_pop_handler();}else{void*_tmpA4=(void*)_exn_thrown;void*_tmpBC=_tmpA4;void*_tmpBE;_LL85: {struct Cyc_Dict_Absent_exn_struct*_tmpBD=(struct Cyc_Dict_Absent_exn_struct*)_tmpBC;if(_tmpBD->tag != Cyc_Dict_Absent)goto _LL87;}_LL86:
# 334
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2CE;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2CD;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBF=(_tmp2CD=_cycalloc(sizeof(*_tmp2CD)),((_tmp2CD[0]=((_tmp2CE.tag=2,((_tmp2CE.f1=fd,_tmp2CE)))),_tmp2CD)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2DD;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2DC;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2DB;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2DA;struct _tuple13*_tmp2D9;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp2D9=_cycalloc(sizeof(*_tmp2D9)),((_tmp2D9->f1=(void*)((_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((_tmp2DD[0]=((_tmp2DA.tag=0,((_tmp2DA.f1=(void*)((_tmp2DC=_cycalloc(sizeof(*_tmp2DC)),((_tmp2DC[0]=((_tmp2DB.tag=2,((_tmp2DB.f1=fd,_tmp2DB)))),_tmp2DC)))),_tmp2DA)))),_tmp2DD)))),((_tmp2D9->f2=0,_tmp2D9)))))));}
# 339
goto _LL84;_LL87: _tmpBE=_tmpBC;_LL88:(void)_throw(_tmpBE);_LL84:;}};}
# 343
if(in_cinclude)return;{
# 348
struct _RegionHandle _tmpC7=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpC7;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmpC8=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp2DE;struct Cyc_Tcenv_Tenv*_tmpC9=(_tmp2DE=_region_malloc(fnrgn,sizeof(*_tmp2DE)),((_tmp2DE->ns=te->ns,((_tmp2DE->ae=te->ae,((_tmp2DE->le=_tmpC8,((_tmp2DE->allow_valueof=0,_tmp2DE)))))))));
# 352
Cyc_Tcstmt_tcStmt(_tmpC9,fd->body,0);
# 355
Cyc_Tcenv_check_delayed_effects(_tmpC9);
Cyc_Tcenv_check_delayed_constraints(_tmpC9);
# 359
if(!Cyc_Tcenv_all_labels_resolved(_tmpC9)){
const char*_tmp2E1;void*_tmp2E0;(_tmp2E0=0,Cyc_Tcutil_terr(loc,((_tmp2E1="function has goto statements to undefined labels",_tag_dyneither(_tmp2E1,sizeof(char),49))),_tag_dyneither(_tmp2E0,sizeof(void*),0)));}}
# 363
if(Cyc_Tc_is_main(fd->name)){
# 365
{void*_tmpCD=Cyc_Tcutil_compress(fd->ret_type);enum Cyc_Absyn_Size_of _tmpD0;_LL8A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCD;if(_tmpCE->tag != 0)goto _LL8C;}_LL8B:
{const char*_tmp2E4;void*_tmp2E3;(_tmp2E3=0,Cyc_Tcutil_warn(loc,((_tmp2E4="main declared with return type void",_tag_dyneither(_tmp2E4,sizeof(char),36))),_tag_dyneither(_tmp2E3,sizeof(void*),0)));}goto _LL89;_LL8C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCD;if(_tmpCF->tag != 6)goto _LL8E;else{_tmpD0=_tmpCF->f2;}}_LL8D:
 goto _LL89;_LL8E:;_LL8F:
# 369
{const char*_tmp2E8;void*_tmp2E7[1];struct Cyc_String_pa_PrintArg_struct _tmp2E6;(_tmp2E6.tag=0,((_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp2E7[0]=& _tmp2E6,Cyc_Tcutil_terr(loc,((_tmp2E8="main declared with return type %s instead of int or void",_tag_dyneither(_tmp2E8,sizeof(char),57))),_tag_dyneither(_tmp2E7,sizeof(void*),1)))))));}
goto _LL89;_LL89:;}
# 373
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp2EB;void*_tmp2EA;(_tmp2EA=0,Cyc_Tcutil_terr(loc,((_tmp2EB="main declared with varargs",_tag_dyneither(_tmp2EB,sizeof(char),27))),_tag_dyneither(_tmp2EA,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpD8=fd->args;
if(_tmpD8 != 0){
void*_tmpDA;struct _tuple8*_tmpD9=(struct _tuple8*)_tmpD8->hd;_tmpDA=_tmpD9->f3;
{void*_tmpDB=Cyc_Tcutil_compress(_tmpDA);enum Cyc_Absyn_Size_of _tmpDD;_LL91: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 6)goto _LL93;else{_tmpDD=_tmpDC->f2;}}_LL92:
 goto _LL90;_LL93:;_LL94:
# 381
{const char*_tmp2EF;void*_tmp2EE[1];struct Cyc_String_pa_PrintArg_struct _tmp2ED;(_tmp2ED.tag=0,((_tmp2ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpDA)),((_tmp2EE[0]=& _tmp2ED,Cyc_Tcutil_terr(loc,((_tmp2EF="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp2EF,sizeof(char),60))),_tag_dyneither(_tmp2EE,sizeof(void*),1)))))));}
goto _LL90;_LL90:;}
# 385
_tmpD8=_tmpD8->tl;
if(_tmpD8 != 0){
void*_tmpE2;struct _tuple8*_tmpE1=(struct _tuple8*)_tmpD8->hd;_tmpE2=_tmpE1->f3;
_tmpD8=_tmpD8->tl;
if(_tmpD8 != 0){
const char*_tmp2F2;void*_tmp2F1;(_tmp2F1=0,Cyc_Tcutil_terr(loc,((_tmp2F2="main declared with too many arguments",_tag_dyneither(_tmp2F2,sizeof(char),38))),_tag_dyneither(_tmp2F1,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp2F3;struct Cyc_Core_Opt*tvs=(_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3->v=fd->tvs,_tmp2F3)));
struct Cyc_Core_Opt*_tmp2FB;struct Cyc_Core_Opt*_tmp2FA;struct Cyc_Core_Opt*_tmp2F9;struct Cyc_Core_Opt*_tmp2F8;struct Cyc_Core_Opt*_tmp2F7;struct Cyc_Core_Opt*_tmp2F6;struct Cyc_Core_Opt*_tmp2F5;struct Cyc_Core_Opt*_tmp2F4;if(((!Cyc_Tcutil_unify(_tmpE2,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((_tmp2FA->v=& Cyc_Tcutil_rk,_tmp2FA)))),tvs)),
Cyc_Absyn_new_evar(((_tmp2FB=_cycalloc(sizeof(*_tmp2FB)),((_tmp2FB->v=& Cyc_Tcutil_rk,_tmp2FB)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpE2,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp2F8=_cycalloc(sizeof(*_tmp2F8)),((_tmp2F8->v=& Cyc_Tcutil_rk,_tmp2F8)))),tvs)),
# 397
Cyc_Absyn_new_evar(((_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9->v=& Cyc_Tcutil_rk,_tmp2F9)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpE2,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6->v=& Cyc_Tcutil_rk,_tmp2F6)))),tvs)),
Cyc_Absyn_new_evar(((_tmp2F7=_cycalloc(sizeof(*_tmp2F7)),((_tmp2F7->v=& Cyc_Tcutil_rk,_tmp2F7)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpE2,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4->v=& Cyc_Tcutil_rk,_tmp2F4)))),tvs)),
# 404
Cyc_Absyn_new_evar(((_tmp2F5=_cycalloc(sizeof(*_tmp2F5)),((_tmp2F5->v=& Cyc_Tcutil_rk,_tmp2F5)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp2FF;void*_tmp2FE[1];struct Cyc_String_pa_PrintArg_struct _tmp2FD;(_tmp2FD.tag=0,((_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpE2)),((_tmp2FE[0]=& _tmp2FD,Cyc_Tcutil_terr(loc,((_tmp2FF="second argument of main has type %s instead of char??",_tag_dyneither(_tmp2FF,sizeof(char),54))),_tag_dyneither(_tmp2FE,sizeof(void*),1)))))));}};}}};}
# 348
;_pop_region(fnrgn);};};}
# 414
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 420
{union Cyc_Absyn_Nmspace _tmpF1=(*td->name).f1;_LL96: if((_tmpF1.Rel_n).tag != 1)goto _LL98;_LL97:
# 423
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LL95;_LL98:;_LL99:
 goto _LL95;_LL95:;}
# 428
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp303;void*_tmp302[1];struct Cyc_String_pa_PrintArg_struct _tmp301;(_tmp301.tag=0,((_tmp301.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp302[0]=& _tmp301,Cyc_Tcutil_terr(loc,((_tmp303="redeclaration of typedef %s",_tag_dyneither(_tmp303,sizeof(char),28))),_tag_dyneither(_tmp302,sizeof(void*),1)))))));}
return;}
# 433
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 442
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpF5=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt**_tmpF7;struct Cyc_Core_Opt**_tmpF9;struct Cyc_Absyn_Kind*_tmpFA;_LL9B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpF6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpF5;if(_tmpF6->tag != 1)goto _LL9D;else{_tmpF7=(struct Cyc_Core_Opt**)& _tmpF6->f1;}}_LL9C:
# 445
 if(td->defn != 0){
const char*_tmp307;void*_tmp306[1];struct Cyc_String_pa_PrintArg_struct _tmp305;(_tmp305.tag=0,((_tmp305.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp306[0]=& _tmp305,Cyc_Tcutil_warn(loc,((_tmp307="type variable %s is not used in typedef definition",_tag_dyneither(_tmp307,sizeof(char),51))),_tag_dyneither(_tmp306,sizeof(void*),1)))))));}
# 448
{struct Cyc_Core_Opt*_tmp308;*_tmpF7=((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp308))));}goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF5;if(_tmpF8->tag != 2)goto _LL9F;else{_tmpF9=(struct Cyc_Core_Opt**)& _tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LL9E:
# 455
{struct Cyc_Core_Opt*_tmp309;*_tmpF9=((_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309->v=Cyc_Tcutil_kind_to_bound(_tmpFA),_tmp309))));}
goto _LL9A;_LL9F:;_LLA0:
 continue;_LL9A:;}}
# 461
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 464
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 471
struct _RegionHandle _tmp100=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp100;_push_region(uprev_rgn);
# 473
for(0;rpo != 0;rpo=rpo->tl){
void*_tmp102;void*_tmp103;struct _tuple10*_tmp101=(struct _tuple10*)rpo->hd;_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp102);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp103);}{
# 479
struct Cyc_List_List*prev_fields=0;
# 481
struct Cyc_List_List*_tmp104=fields;for(0;_tmp104 != 0;_tmp104=_tmp104->tl){
struct _dyneither_ptr*_tmp106;struct Cyc_Absyn_Tqual _tmp107;void*_tmp108;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrfield*_tmp105=(struct Cyc_Absyn_Aggrfield*)_tmp104->hd;_tmp106=_tmp105->name;_tmp107=_tmp105->tq;_tmp108=_tmp105->type;_tmp109=_tmp105->width;_tmp10A=_tmp105->attributes;
# 484
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp106)){
const char*_tmp30D;void*_tmp30C[1];struct Cyc_String_pa_PrintArg_struct _tmp30B;(_tmp30B.tag=0,((_tmp30B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp106),((_tmp30C[0]=& _tmp30B,Cyc_Tcutil_terr(loc,((_tmp30D="duplicate member %s",_tag_dyneither(_tmp30D,sizeof(char),20))),_tag_dyneither(_tmp30C,sizeof(void*),1)))))));}
# 488
{const char*_tmp30E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp106,((_tmp30E="",_tag_dyneither(_tmp30E,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp30F;prev_fields=((_tmp30F=_region_malloc(uprev_rgn,sizeof(*_tmp30F)),((_tmp30F->hd=_tmp106,((_tmp30F->tl=prev_fields,_tmp30F))))));}}{
# 491
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 495
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp104->tl == 0  && str_or_union == Cyc_Absyn_StructA)
# 498
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp108);
# 501
(((struct Cyc_Absyn_Aggrfield*)_tmp104->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp104->hd)->tq).print_const,_tmp108);
# 504
Cyc_Tcutil_check_bitfield(loc,te,_tmp108,_tmp109,_tmp106);};}};
# 473
;_pop_region(uprev_rgn);}
# 510
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 512
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp110=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt**_tmp112;struct Cyc_Core_Opt**_tmp114;enum Cyc_Absyn_AliasQual _tmp115;struct Cyc_Core_Opt**_tmp117;struct Cyc_Core_Opt**_tmp119;struct Cyc_Absyn_Kind*_tmp11A;enum Cyc_Absyn_AliasQual _tmp11C;_LLA2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp111=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp110;if(_tmp111->tag != 1)goto _LLA4;else{_tmp112=(struct Cyc_Core_Opt**)& _tmp111->f1;}}_LLA3:
# 515
{struct Cyc_Core_Opt*_tmp310;*_tmp112=((_tmp310=_cycalloc(sizeof(*_tmp310)),((_tmp310->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp310))));}continue;_LLA4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110;if(_tmp113->tag != 2)goto _LLA6;else{_tmp114=(struct Cyc_Core_Opt**)& _tmp113->f1;if((_tmp113->f2)->kind != Cyc_Absyn_MemKind)goto _LLA6;_tmp115=(_tmp113->f2)->aliasqual;}}_LLA5:
# 517
 if(constrain_top_kind  && _tmp115 == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp313;struct Cyc_Core_Opt*_tmp312;*_tmp114=((_tmp312=_cycalloc(sizeof(*_tmp312)),((_tmp312->v=Cyc_Tcutil_kind_to_bound(((_tmp313=_cycalloc_atomic(sizeof(*_tmp313)),((_tmp313->kind=Cyc_Absyn_BoxKind,((_tmp313->aliasqual=Cyc_Absyn_Aliasable,_tmp313))))))),_tmp312))));}else{
# 520
struct Cyc_Absyn_Kind*_tmp316;struct Cyc_Core_Opt*_tmp315;*_tmp114=((_tmp315=_cycalloc(sizeof(*_tmp315)),((_tmp315->v=Cyc_Tcutil_kind_to_bound(((_tmp316=_cycalloc_atomic(sizeof(*_tmp316)),((_tmp316->kind=Cyc_Absyn_BoxKind,((_tmp316->aliasqual=_tmp115,_tmp316))))))),_tmp315))));}
continue;_LLA6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110;if(_tmp116->tag != 2)goto _LLA8;else{_tmp117=(struct Cyc_Core_Opt**)& _tmp116->f1;if((_tmp116->f2)->kind != Cyc_Absyn_BoxKind)goto _LLA8;if((_tmp116->f2)->aliasqual != Cyc_Absyn_Top)goto _LLA8;}}if(!constrain_top_kind)goto _LLA8;_LLA7:
# 523
{struct Cyc_Absyn_Kind*_tmp319;struct Cyc_Core_Opt*_tmp318;*_tmp117=((_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318->v=Cyc_Tcutil_kind_to_bound(((_tmp319=_cycalloc_atomic(sizeof(*_tmp319)),((_tmp319->kind=Cyc_Absyn_BoxKind,((_tmp319->aliasqual=Cyc_Absyn_Aliasable,_tmp319))))))),_tmp318))));}
continue;_LLA8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp118=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110;if(_tmp118->tag != 2)goto _LLAA;else{_tmp119=(struct Cyc_Core_Opt**)& _tmp118->f1;_tmp11A=_tmp118->f2;}}_LLA9:
# 526
{struct Cyc_Core_Opt*_tmp31A;*_tmp119=((_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A->v=Cyc_Tcutil_kind_to_bound(_tmp11A),_tmp31A))));}continue;_LLAA: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp11B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp110;if(_tmp11B->tag != 0)goto _LLAC;else{if((_tmp11B->f1)->kind != Cyc_Absyn_MemKind)goto _LLAC;_tmp11C=(_tmp11B->f1)->aliasqual;}}_LLAB:
# 528
{const char*_tmp322;void*_tmp321[3];struct Cyc_String_pa_PrintArg_struct _tmp320;struct Cyc_String_pa_PrintArg_struct _tmp31F;struct Cyc_Absyn_Kind*_tmp31E;struct Cyc_String_pa_PrintArg_struct _tmp31D;(_tmp31D.tag=0,((_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp31E=_cycalloc_atomic(sizeof(*_tmp31E)),((_tmp31E->kind=Cyc_Absyn_MemKind,((_tmp31E->aliasqual=_tmp11C,_tmp31E)))))))),((_tmp31F.tag=0,((_tmp31F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp320.tag=0,((_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp321[0]=& _tmp320,((_tmp321[1]=& _tmp31F,((_tmp321[2]=& _tmp31D,Cyc_Tcutil_terr(loc,((_tmp322="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp322,sizeof(char),57))),_tag_dyneither(_tmp321,sizeof(void*),3)))))))))))))))))));}
continue;_LLAC:;_LLAD:
 continue;_LLA1:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 536
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp12B=(*ad->name).f2;
# 543
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp12C=(void*)atts->hd;_LLAF: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp12D=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp12C;if(_tmp12D->tag != 7)goto _LLB1;}_LLB0:
 goto _LLB2;_LLB1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp12E=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp12C;if(_tmp12E->tag != 6)goto _LLB3;}_LLB2:
 continue;_LLB3:;_LLB4:
# 548
{const char*_tmp327;void*_tmp326[2];struct Cyc_String_pa_PrintArg_struct _tmp325;struct Cyc_String_pa_PrintArg_struct _tmp324;(_tmp324.tag=0,((_tmp324.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp12B),((_tmp325.tag=0,((_tmp325.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp326[0]=& _tmp325,((_tmp326[1]=& _tmp324,Cyc_Tcutil_terr(loc,((_tmp327="bad attribute %s in  %s definition",_tag_dyneither(_tmp327,sizeof(char),35))),_tag_dyneither(_tmp326,sizeof(void*),2)))))))))))));}
goto _LLAE;_LLAE:;}}{
# 553
struct Cyc_List_List*_tmp133=ad->tvs;
# 555
{union Cyc_Absyn_Nmspace _tmp134=(*ad->name).f1;_LLB6: if((_tmp134.Rel_n).tag != 1)goto _LLB8;if((struct Cyc_List_List*)(_tmp134.Rel_n).val != 0)goto _LLB8;_LLB7:
# 558
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLB5;_LLB8:;_LLB9:
# 561
 goto _LLB5;_LLB5:;}
# 565
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 567
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 571
{struct _tuple14 _tmp328;struct _tuple14 _tmp136=(_tmp328.f1=ad->impl,((_tmp328.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp12B),_tmp328)));struct Cyc_List_List*_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_List_List*_tmp139;int _tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13D;int _tmp13E;struct Cyc_Absyn_Aggrdecl**_tmp13F;struct Cyc_Absyn_Aggrdecl**_tmp140;_LLBB: if(_tmp136.f1 != 0)goto _LLBD;if(_tmp136.f2 != 0)goto _LLBD;_LLBC:
# 574
 Cyc_Tc_rule_out_memkind(loc,_tmp12B,_tmp133,0);
{struct Cyc_Absyn_Aggrdecl**_tmp329;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp12B,((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=ad,_tmp329)))));}
goto _LLBA;_LLBD: if(_tmp136.f1 == 0)goto _LLBF;_tmp137=(_tmp136.f1)->exist_vars;_tmp138=(_tmp136.f1)->rgn_po;_tmp139=(_tmp136.f1)->fields;_tmp13A=(_tmp136.f1)->tagged;if(_tmp136.f2 != 0)goto _LLBF;_LLBE: {
# 580
struct Cyc_Absyn_Aggrdecl*_tmp32C;struct Cyc_Absyn_Aggrdecl**_tmp32B;struct Cyc_Absyn_Aggrdecl**_tmp142=(_tmp32B=_cycalloc(sizeof(*_tmp32B)),((_tmp32B[0]=((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C->kind=ad->kind,((_tmp32C->sc=Cyc_Absyn_Extern,((_tmp32C->name=ad->name,((_tmp32C->tvs=_tmp133,((_tmp32C->impl=0,((_tmp32C->attributes=ad->attributes,_tmp32C)))))))))))))),_tmp32B)));
# 582
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp12B,_tmp142);
# 585
Cyc_Tcutil_check_unique_tvars(loc,_tmp137);
# 587
Cyc_Tcutil_add_tvar_identities(_tmp137);
# 590
if(_tmp13A  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp32F;void*_tmp32E;(_tmp32E=0,Cyc_Tcutil_terr(loc,((_tmp32F="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp32F,sizeof(char),56))),_tag_dyneither(_tmp32E,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp133,_tmp137),_tmp138,_tmp139);
# 594
Cyc_Tc_rule_out_memkind(loc,_tmp12B,_tmp133,0);
# 597
Cyc_Tc_rule_out_memkind(loc,_tmp12B,_tmp137,1);
# 599
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp13A){
# 602
struct Cyc_List_List*f=_tmp139;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)){
const char*_tmp335;void*_tmp334[3];struct Cyc_String_pa_PrintArg_struct _tmp333;struct Cyc_String_pa_PrintArg_struct _tmp332;struct Cyc_String_pa_PrintArg_struct _tmp331;(_tmp331.tag=0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp332.tag=0,((_tmp332.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp12B),((_tmp333.tag=0,((_tmp333.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp334[0]=& _tmp333,((_tmp334[1]=& _tmp332,((_tmp334[2]=& _tmp331,Cyc_Tcutil_warn(loc,((_tmp335="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp335,sizeof(char),98))),_tag_dyneither(_tmp334,sizeof(void*),3)))))))))))))))))));}}}
# 607
*_tmp142=ad;
goto _LLBA;}_LLBF: if(_tmp136.f1 == 0)goto _LLC1;_tmp13B=(_tmp136.f1)->exist_vars;_tmp13C=(_tmp136.f1)->rgn_po;_tmp13D=(_tmp136.f1)->fields;_tmp13E=(_tmp136.f1)->tagged;if(_tmp136.f2 == 0)goto _LLC1;_tmp13F=*_tmp136.f2;_LLC0:
# 611
 if(ad->kind != (*_tmp13F)->kind){
const char*_tmp338;void*_tmp337;(_tmp337=0,Cyc_Tcutil_terr(loc,((_tmp338="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp338,sizeof(char),52))),_tag_dyneither(_tmp337,sizeof(void*),0)));}{
# 614
struct Cyc_Absyn_Aggrdecl*_tmp14E=*_tmp13F;
# 616
{struct Cyc_Absyn_Aggrdecl*_tmp339;*_tmp13F=((_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339->kind=ad->kind,((_tmp339->sc=Cyc_Absyn_Extern,((_tmp339->name=ad->name,((_tmp339->tvs=_tmp133,((_tmp339->impl=0,((_tmp339->attributes=ad->attributes,_tmp339))))))))))))));}
# 620
Cyc_Tcutil_check_unique_tvars(loc,_tmp13B);
# 622
Cyc_Tcutil_add_tvar_identities(_tmp13B);
# 624
if(_tmp13E  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp33C;void*_tmp33B;(_tmp33B=0,Cyc_Tcutil_terr(loc,((_tmp33C="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp33C,sizeof(char),56))),_tag_dyneither(_tmp33B,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp133,_tmp13B),_tmp13C,_tmp13D);
# 628
*_tmp13F=_tmp14E;
_tmp140=_tmp13F;goto _LLC2;};_LLC1: if(_tmp136.f1 != 0)goto _LLBA;if(_tmp136.f2 == 0)goto _LLBA;_tmp140=*_tmp136.f2;_LLC2: {
# 632
struct Cyc_Absyn_Aggrdecl*_tmp152=Cyc_Tcdecl_merge_aggrdecl(*_tmp140,ad,loc,Cyc_Tc_tc_msg);
if(_tmp152 == 0)
return;else{
# 636
Cyc_Tc_rule_out_memkind(loc,_tmp12B,_tmp133,0);
# 639
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp12B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 642
*_tmp140=_tmp152;
ad=_tmp152;
goto _LLBA;}}_LLBA:;}{
# 648
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp342;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp341;struct _tuple13*_tmp340;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp12B,(
(_tmp340=_cycalloc(sizeof(*_tmp340)),((_tmp340->f1=(void*)((_tmp342=_cycalloc(sizeof(*_tmp342)),((_tmp342[0]=((_tmp341.tag=1,((_tmp341.f1=ad,_tmp341)))),_tmp342)))),((_tmp340->f2=1,_tmp340)))))));};};}
# 652
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp156=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp15C;struct Cyc_Core_Opt**_tmp15E;struct Cyc_Core_Opt**_tmp160;struct Cyc_Core_Opt**_tmp162;struct Cyc_Core_Opt**_tmp165;enum Cyc_Absyn_KindQual _tmp166;enum Cyc_Absyn_KindQual _tmp168;enum Cyc_Absyn_AliasQual _tmp16A;_LLC4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp157=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp157->tag != 1)goto _LLC6;else{_tmp158=(struct Cyc_Core_Opt**)& _tmp157->f1;}}_LLC5:
 _tmp15A=_tmp158;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp159=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp159->tag != 2)goto _LLC8;else{_tmp15A=(struct Cyc_Core_Opt**)& _tmp159->f1;if((_tmp159->f2)->kind != Cyc_Absyn_MemKind)goto _LLC8;if((_tmp159->f2)->aliasqual != Cyc_Absyn_Top)goto _LLC8;}}_LLC7:
# 657
 _tmp15C=_tmp15A;goto _LLC9;_LLC8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp15B->tag != 2)goto _LLCA;else{_tmp15C=(struct Cyc_Core_Opt**)& _tmp15B->f1;if((_tmp15B->f2)->kind != Cyc_Absyn_MemKind)goto _LLCA;if((_tmp15B->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLCA;}}_LLC9:
# 659
{struct Cyc_Core_Opt*_tmp343;*_tmp15C=((_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp343))));}goto _LLC3;_LLCA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp15D->tag != 2)goto _LLCC;else{_tmp15E=(struct Cyc_Core_Opt**)& _tmp15D->f1;if((_tmp15D->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCC;if((_tmp15D->f2)->aliasqual != Cyc_Absyn_Top)goto _LLCC;}}_LLCB:
 _tmp160=_tmp15E;goto _LLCD;_LLCC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp15F->tag != 2)goto _LLCE;else{_tmp160=(struct Cyc_Core_Opt**)& _tmp15F->f1;if((_tmp15F->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCE;if((_tmp15F->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLCE;}}_LLCD:
# 662
{struct Cyc_Core_Opt*_tmp344;*_tmp160=((_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp344))));}goto _LLC3;_LLCE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp161=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp161->tag != 2)goto _LLD0;else{_tmp162=(struct Cyc_Core_Opt**)& _tmp161->f1;if((_tmp161->f2)->kind != Cyc_Absyn_RgnKind)goto _LLD0;if((_tmp161->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD0;}}_LLCF:
# 664
{struct Cyc_Core_Opt*_tmp345;*_tmp162=((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp345))));}goto _LLC3;_LLD0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp163=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp163->tag != 0)goto _LLD2;else{if((_tmp163->f1)->kind != Cyc_Absyn_RgnKind)goto _LLD2;if((_tmp163->f1)->aliasqual != Cyc_Absyn_Top)goto _LLD2;}}_LLD1:
# 666
{const char*_tmp34A;void*_tmp349[2];struct Cyc_String_pa_PrintArg_struct _tmp348;struct Cyc_String_pa_PrintArg_struct _tmp347;(_tmp347.tag=0,((_tmp347.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp348.tag=0,((_tmp348.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp349[0]=& _tmp348,((_tmp349[1]=& _tmp347,Cyc_Tcutil_terr(loc,((_tmp34A="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp34A,sizeof(char),57))),_tag_dyneither(_tmp349,sizeof(void*),2)))))))))))));}
goto _LLC3;_LLD2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp164=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp164->tag != 2)goto _LLD4;else{_tmp165=(struct Cyc_Core_Opt**)& _tmp164->f1;_tmp166=(_tmp164->f2)->kind;if((_tmp164->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:
 _tmp168=_tmp166;goto _LLD5;_LLD4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp167=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp167->tag != 0)goto _LLD6;else{_tmp168=(_tmp167->f1)->kind;if((_tmp167->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLD6;}}_LLD5:
# 670
{const char*_tmp352;void*_tmp351[3];struct Cyc_String_pa_PrintArg_struct _tmp350;struct Cyc_String_pa_PrintArg_struct _tmp34F;struct Cyc_Absyn_Kind*_tmp34E;struct Cyc_String_pa_PrintArg_struct _tmp34D;(_tmp34D.tag=0,((_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 672
Cyc_Absynpp_kind2string(((_tmp34E=_cycalloc_atomic(sizeof(*_tmp34E)),((_tmp34E->kind=_tmp168,((_tmp34E->aliasqual=Cyc_Absyn_Unique,_tmp34E)))))))),((_tmp34F.tag=0,((_tmp34F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp350.tag=0,((_tmp350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp351[0]=& _tmp350,((_tmp351[1]=& _tmp34F,((_tmp351[2]=& _tmp34D,Cyc_Tcutil_terr(loc,((_tmp352="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp352,sizeof(char),57))),_tag_dyneither(_tmp351,sizeof(void*),3)))))))))))))))))));}goto _LLC3;_LLD6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp169=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp169->tag != 0)goto _LLD8;else{if((_tmp169->f1)->kind != Cyc_Absyn_MemKind)goto _LLD8;_tmp16A=(_tmp169->f1)->aliasqual;}}_LLD7:
# 674
{const char*_tmp35A;void*_tmp359[3];struct Cyc_String_pa_PrintArg_struct _tmp358;struct Cyc_String_pa_PrintArg_struct _tmp357;struct Cyc_Absyn_Kind*_tmp356;struct Cyc_String_pa_PrintArg_struct _tmp355;(_tmp355.tag=0,((_tmp355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 676
Cyc_Absynpp_kind2string(((_tmp356=_cycalloc_atomic(sizeof(*_tmp356)),((_tmp356->kind=Cyc_Absyn_MemKind,((_tmp356->aliasqual=_tmp16A,_tmp356)))))))),((_tmp357.tag=0,((_tmp357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp358.tag=0,((_tmp358.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp359[0]=& _tmp358,((_tmp359[1]=& _tmp357,((_tmp359[2]=& _tmp355,Cyc_Tcutil_terr(loc,((_tmp35A="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp35A,sizeof(char),57))),_tag_dyneither(_tmp359,sizeof(void*),3)))))))))))))))))));}goto _LLC3;_LLD8:;_LLD9:
 goto _LLC3;_LLC3:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 683
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 692
{struct Cyc_List_List*_tmp17E=fields;for(0;_tmp17E != 0;_tmp17E=_tmp17E->tl){
struct Cyc_Absyn_Datatypefield*_tmp17F=(struct Cyc_Absyn_Datatypefield*)_tmp17E->hd;
# 695
{struct Cyc_List_List*typs=_tmp17F->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp17F->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 698
struct _RegionHandle _tmp180=_new_region("temp");struct _RegionHandle*temp=& _tmp180;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp35E;void*_tmp35D[1];struct Cyc_String_pa_PrintArg_struct _tmp35C;(_tmp35C.tag=0,((_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 702
Cyc_Absynpp_qvar2string(_tmp17F->name)),((_tmp35D[0]=& _tmp35C,Cyc_Tcutil_terr(_tmp17F->loc,((_tmp35E="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp35E,sizeof(char),51))),_tag_dyneither(_tmp35D,sizeof(void*),1)))))));}
# 704
((*((struct _tuple15*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp17F->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 698
;_pop_region(temp);};}}{
# 709
union Cyc_Absyn_Nmspace _tmp184=(*_tmp17F->name).f1;_LLDB: if((_tmp184.Rel_n).tag != 1)goto _LLDD;if((struct Cyc_List_List*)(_tmp184.Rel_n).val != 0)goto _LLDD;_LLDC:
# 711
 if(is_extensible)
(*_tmp17F->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 714
(*_tmp17F->name).f1=(*name).f1;}
goto _LLDA;_LLDD: if((_tmp184.Rel_n).tag != 1)goto _LLDF;_LLDE:
# 717
{const char*_tmp362;void*_tmp361[1];struct Cyc_String_pa_PrintArg_struct _tmp360;(_tmp360.tag=0,((_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp17F->name)),((_tmp361[0]=& _tmp360,Cyc_Tcutil_terr(_tmp17F->loc,((_tmp362="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp362,sizeof(char),58))),_tag_dyneither(_tmp361,sizeof(void*),1)))))));}
goto _LLDA;_LLDF: if((_tmp184.C_n).tag != 3)goto _LLE1;_LLE0:
 goto _LLDA;_LLE1: if((_tmp184.Abs_n).tag != 2)goto _LLE3;_LLE2:
 goto _LLDA;_LLE3: if((_tmp184.Loc_n).tag != 4)goto _LLDA;_LLE4: {
const char*_tmp365;void*_tmp364;(_tmp364=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp365="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp365,sizeof(char),24))),_tag_dyneither(_tmp364,sizeof(void*),0)));}_LLDA:;};}}{
# 726
struct Cyc_List_List*fields2;
if(is_extensible){
# 729
int _tmp18A=1;
struct Cyc_List_List*_tmp18B=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp18A,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp18A)
fields2=_tmp18B;else{
# 734
fields2=0;}}else{
# 736
struct _RegionHandle _tmp18C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp18C;_push_region(uprev_rgn);
# 738
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp18D=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp18D->name).f2)){
const char*_tmp36A;void*_tmp369[2];struct Cyc_String_pa_PrintArg_struct _tmp368;struct Cyc_String_pa_PrintArg_struct _tmp367;(_tmp367.tag=0,((_tmp367.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp368.tag=0,((_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18D->name).f2),((_tmp369[0]=& _tmp368,((_tmp369[1]=& _tmp367,Cyc_Tcutil_terr(_tmp18D->loc,((_tmp36A="duplicate field name %s in %s",_tag_dyneither(_tmp36A,sizeof(char),30))),_tag_dyneither(_tmp369,sizeof(void*),2)))))))))))));}else{
# 744
struct Cyc_List_List*_tmp36B;prev_fields=((_tmp36B=_region_malloc(uprev_rgn,sizeof(*_tmp36B)),((_tmp36B->hd=(*_tmp18D->name).f2,((_tmp36B->tl=prev_fields,_tmp36B))))));}
# 746
if(_tmp18D->sc != Cyc_Absyn_Public){
{const char*_tmp36F;void*_tmp36E[1];struct Cyc_String_pa_PrintArg_struct _tmp36D;(_tmp36D.tag=0,((_tmp36D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18D->name).f2),((_tmp36E[0]=& _tmp36D,Cyc_Tcutil_warn(loc,((_tmp36F="ignoring scope of field %s",_tag_dyneither(_tmp36F,sizeof(char),27))),_tag_dyneither(_tmp36E,sizeof(void*),1)))))));}
_tmp18D->sc=Cyc_Absyn_Public;}}}
# 751
fields2=fields;}
# 738
;_pop_region(uprev_rgn);}
# 757
{struct Cyc_List_List*_tmp196=fields;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){
struct Cyc_Absyn_Datatypefield*_tmp197=(struct Cyc_Absyn_Datatypefield*)_tmp196->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp375;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp374;struct _tuple13*_tmp373;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp197->name).f2,(
(_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->f1=(void*)((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375[0]=((_tmp374.tag=2,((_tmp374.f1=tudres,((_tmp374.f2=_tmp197,_tmp374)))))),_tmp375)))),((_tmp373->f2=1,_tmp373)))))));}}
# 763
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 766
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp377;const char*_tmp376;struct _dyneither_ptr obj=tud->is_extensible?(_tmp377="@extensible datatype",_tag_dyneither(_tmp377,sizeof(char),21)):((_tmp376="datatype",_tag_dyneither(_tmp376,sizeof(char),9)));
# 773
struct Cyc_List_List*tvs=tud->tvs;
# 776
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 778
Cyc_Tcutil_add_tvar_identities(tvs);{
# 783
struct _RegionHandle _tmp19B=_new_region("temp");struct _RegionHandle*temp=& _tmp19B;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 793 "tc.cyc"
{struct _handler_cons _tmp19C;_push_handler(& _tmp19C);{int _tmp19E=0;if(setjmp(_tmp19C.handler))_tmp19E=1;if(!_tmp19E){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp37A;struct Cyc_Dict_Absent_exn_struct*_tmp379;(int)_throw((void*)((_tmp379=_cycalloc_atomic(sizeof(*_tmp379)),((_tmp379[0]=((_tmp37A.tag=Cyc_Dict_Absent,_tmp37A)),_tmp379)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 799
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 794
;_pop_handler();}else{void*_tmp19D=(void*)_exn_thrown;void*_tmp1A2=_tmp19D;void*_tmp1A4;_LLE6: {struct Cyc_Dict_Absent_exn_struct*_tmp1A3=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A2;if(_tmp1A3->tag != Cyc_Dict_Absent)goto _LLE8;}_LLE7:
# 803
{union Cyc_Absyn_Nmspace _tmp1A5=(*tud->name).f1;_LLEB: if((_tmp1A5.Rel_n).tag != 1)goto _LLED;if((struct Cyc_List_List*)(_tmp1A5.Rel_n).val != 0)goto _LLED;_LLEC:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLEA;_LLED:;_LLEE:
 goto _LLEA;_LLEA:;}{
# 807
struct Cyc_Absyn_Datatypedecl***_tmp1A6=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp37B;tud_opt=(unsigned int)_tmp1A6?(_tmp37B=_region_malloc(temp,sizeof(*_tmp37B)),((_tmp37B[0]=*_tmp1A6,_tmp37B))): 0;}
goto _LLE5;};_LLE8: _tmp1A4=_tmp1A2;_LLE9:(void)_throw(_tmp1A4);_LLE5:;}};}{
# 814
struct _tuple16 _tmp37C;struct _tuple16 _tmp1A9=(_tmp37C.f1=tud->fields,((_tmp37C.f2=tud_opt,_tmp37C)));struct Cyc_List_List**_tmp1AA;struct Cyc_List_List**_tmp1AB;struct Cyc_Absyn_Datatypedecl**_tmp1AC;struct Cyc_Absyn_Datatypedecl**_tmp1AD;_LLF0: if(_tmp1A9.f1 != 0)goto _LLF2;if(_tmp1A9.f2 != 0)goto _LLF2;_LLF1:
# 817
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp37D;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=tud,_tmp37D)))));}
goto _LLEF;_LLF2: if(_tmp1A9.f1 == 0)goto _LLF4;_tmp1AA=(struct Cyc_List_List**)&(_tmp1A9.f1)->v;if(_tmp1A9.f2 != 0)goto _LLF4;_LLF3: {
# 822
struct Cyc_Absyn_Datatypedecl*_tmp380;struct Cyc_Absyn_Datatypedecl**_tmp37F;struct Cyc_Absyn_Datatypedecl**_tmp1AF=(_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F[0]=((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->sc=Cyc_Absyn_Extern,((_tmp380->name=tud->name,((_tmp380->tvs=tvs,((_tmp380->fields=0,((_tmp380->is_extensible=tud->is_extensible,_tmp380)))))))))))),_tmp37F)));
# 824
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1AF);
# 827
*_tmp1AA=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1AA,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1AF=tud;
goto _LLEF;}_LLF4: if(_tmp1A9.f1 == 0)goto _LLF6;_tmp1AB=(struct Cyc_List_List**)&(_tmp1A9.f1)->v;if(_tmp1A9.f2 == 0)goto _LLF6;_tmp1AC=*_tmp1A9.f2;_LLF5: {
# 832
struct Cyc_Absyn_Datatypedecl*_tmp1B2=*_tmp1AC;
# 835
if((!tud->is_extensible  && (unsigned int)_tmp1B2) && _tmp1B2->is_extensible)
tud->is_extensible=1;
# 838
{struct Cyc_Absyn_Datatypedecl*_tmp381;*_tmp1AC=((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381->sc=Cyc_Absyn_Extern,((_tmp381->name=tud->name,((_tmp381->tvs=tvs,((_tmp381->fields=0,((_tmp381->is_extensible=tud->is_extensible,_tmp381))))))))))));}
# 842
*_tmp1AB=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1AB,tvs,tud);
# 846
*_tmp1AC=_tmp1B2;
_tmp1AD=_tmp1AC;goto _LLF7;}_LLF6: if(_tmp1A9.f1 != 0)goto _LLEF;if(_tmp1A9.f2 == 0)goto _LLEF;_tmp1AD=*_tmp1A9.f2;_LLF7: {
# 850
struct Cyc_Absyn_Datatypedecl*_tmp1B4=Cyc_Tcdecl_merge_datatypedecl(*_tmp1AD,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1B4 == 0){
_npop_handler(0);return;}else{
# 855
*_tmp1AD=_tmp1B4;
goto _LLEF;}}_LLEF:;};}
# 784 "tc.cyc"
;_pop_region(temp);};}
# 862 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 869
{union Cyc_Absyn_Nmspace _tmp1B7=(*ed->name).f1;_LLF9: if((_tmp1B7.Rel_n).tag != 1)goto _LLFB;if((struct Cyc_List_List*)(_tmp1B7.Rel_n).val != 0)goto _LLFB;_LLFA:
# 872
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LLF8;_LLFB:;_LLFC:
 goto _LLF8;_LLF8:;}
# 878
if(ed->fields != 0){struct _RegionHandle _tmp1B8=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1B8;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1B9=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 884
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1B9->name).f2)){
const char*_tmp385;void*_tmp384[1];struct Cyc_String_pa_PrintArg_struct _tmp383;(_tmp383.tag=0,((_tmp383.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B9->name).f2),((_tmp384[0]=& _tmp383,Cyc_Tcutil_terr(_tmp1B9->loc,((_tmp385="duplicate field name %s",_tag_dyneither(_tmp385,sizeof(char),24))),_tag_dyneither(_tmp384,sizeof(void*),1)))))));}else{
# 887
struct Cyc_List_List*_tmp386;prev_fields=((_tmp386=_region_malloc(uprev_rgn,sizeof(*_tmp386)),((_tmp386->hd=(*_tmp1B9->name).f2,((_tmp386->tl=prev_fields,_tmp386))))));}
# 889
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1B9->name).f2)!= 0){
const char*_tmp38A;void*_tmp389[1];struct Cyc_String_pa_PrintArg_struct _tmp388;(_tmp388.tag=0,((_tmp388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B9->name).f2),((_tmp389[0]=& _tmp388,Cyc_Tcutil_terr(_tmp1B9->loc,((_tmp38A="enum field name %s shadows global name",_tag_dyneither(_tmp38A,sizeof(char),39))),_tag_dyneither(_tmp389,sizeof(void*),1)))))));}
# 892
if(_tmp1B9->tag == 0){
_tmp1B9->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1B9->loc);
++ tag_count;}else{
# 897
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B9->tag))){
unsigned int _tmp1C2;int _tmp1C3;struct _tuple11 _tmp1C1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B9->tag));_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;
if(_tmp1C3)tag_count=_tmp1C2 + 1;}}
# 901
(*_tmp1B9->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 879
;_pop_region(uprev_rgn);}
# 906
{struct _handler_cons _tmp1C4;_push_handler(& _tmp1C4);{int _tmp1C6=0;if(setjmp(_tmp1C4.handler))_tmp1C6=1;if(!_tmp1C6){
{struct Cyc_Absyn_Enumdecl**_tmp1C7=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp1C8=Cyc_Tcdecl_merge_enumdecl(*_tmp1C7,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1C8 == 0){_npop_handler(0);return;}
*_tmp1C7=_tmp1C8;}
# 907
;_pop_handler();}else{void*_tmp1C5=(void*)_exn_thrown;void*_tmp1CA=_tmp1C5;void*_tmp1CC;_LLFE: {struct Cyc_Dict_Absent_exn_struct*_tmp1CB=(struct Cyc_Dict_Absent_exn_struct*)_tmp1CA;if(_tmp1CB->tag != Cyc_Dict_Absent)goto _LL100;}_LLFF: {
# 912
struct Cyc_Absyn_Enumdecl**_tmp38B;struct Cyc_Absyn_Enumdecl**_tmp1CD=(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=ed,_tmp38B)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp1CD);
goto _LLFD;}_LL100: _tmp1CC=_tmp1CA;_LL101:(void)_throw(_tmp1CC);_LLFD:;}};}
# 916
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1CF=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp391;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp390;struct _tuple13*_tmp38F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1CF->name).f2,(
(_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->f1=(void*)((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=((_tmp390.tag=3,((_tmp390.f1=ed,((_tmp390.f2=_tmp1CF,_tmp390)))))),_tmp391)))),((_tmp38F->f2=1,_tmp38F)))))));}}
# 925
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1D3=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1D3->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1D3->tag))){
const char*_tmp396;void*_tmp395[2];struct Cyc_String_pa_PrintArg_struct _tmp394;struct Cyc_String_pa_PrintArg_struct _tmp393;(_tmp393.tag=0,((_tmp393.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D3->name).f2),((_tmp394.tag=0,((_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp395[0]=& _tmp394,((_tmp395[1]=& _tmp393,Cyc_Tcutil_terr(loc,((_tmp396="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp396,sizeof(char),46))),_tag_dyneither(_tmp395,sizeof(void*),2)))))))))))));}}}}
# 935
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL102:
# 938
 if(!in_include){
const char*_tmp399;void*_tmp398;(_tmp398=0,Cyc_Tcutil_warn(loc,((_tmp399="static declaration nested within extern \"C\"",_tag_dyneither(_tmp399,sizeof(char),44))),_tag_dyneither(_tmp398,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL103:
# 942
{const char*_tmp39C;void*_tmp39B;(_tmp39B=0,Cyc_Tcutil_warn(loc,((_tmp39C="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp39C,sizeof(char),46))),_tag_dyneither(_tmp39B,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL104:
 goto _LL105;case Cyc_Absyn_Register: _LL105:
 goto _LL106;case Cyc_Absyn_Extern: _LL106:
 return 1;case Cyc_Absyn_ExternC: _LL107:
# 948
{const char*_tmp39F;void*_tmp39E;(_tmp39E=0,Cyc_Tcutil_warn(loc,((_tmp39F="nested extern \"C\" declaration",_tag_dyneither(_tmp39F,sizeof(char),30))),_tag_dyneither(_tmp39E,sizeof(void*),0)));}
return 1;}}
# 953
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
unsigned int _tmp1DF;struct _tuple0*_tmp1E0;struct _tuple12 _tmp1DE=*exp;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1DE.f2;{
union Cyc_Absyn_Nmspace _tmp1E2;struct _dyneither_ptr*_tmp1E3;struct _tuple0*_tmp1E1=_tmp1E0;_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;{
union Cyc_Absyn_Nmspace _tmp1E4=_tmp1E2;_LL10A: if((_tmp1E4.Rel_n).tag != 1)goto _LL10C;if((struct Cyc_List_List*)(_tmp1E4.Rel_n).val != 0)goto _LL10C;_LL10B:
# 959
(*_tmp1E0).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL109;_LL10C:;_LL10D:
 goto _LL109;_LL109:;};};}
# 970
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 977
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 980
struct Cyc_List_List*_tmp1E5=ds0;for(0;_tmp1E5 != 0;(last=_tmp1E5,_tmp1E5=_tmp1E5->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1E5->hd;
unsigned int loc=d->loc;
# 984
void*_tmp1E6=d->r;struct Cyc_Absyn_Vardecl*_tmp1EC;struct Cyc_Absyn_Fndecl*_tmp1EE;struct Cyc_Absyn_Typedefdecl*_tmp1F0;struct Cyc_Absyn_Aggrdecl*_tmp1F2;struct Cyc_Absyn_Datatypedecl*_tmp1F4;struct Cyc_Absyn_Enumdecl*_tmp1F6;struct _dyneither_ptr*_tmp1FA;struct Cyc_List_List*_tmp1FB;union Cyc_Absyn_Nmspace _tmp1FD;struct _dyneither_ptr*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp201;struct Cyc_List_List*_tmp203;struct Cyc_List_List*_tmp204;_LL10F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp1E7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1E7->tag != 2)goto _LL111;}_LL110:
 goto _LL112;_LL111: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp1E8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1E8->tag != 3)goto _LL113;}_LL112:
# 987
{const char*_tmp3A2;void*_tmp3A1;(_tmp3A1=0,Cyc_Tcutil_terr(loc,((_tmp3A2="top level let-declarations are not implemented",_tag_dyneither(_tmp3A2,sizeof(char),47))),_tag_dyneither(_tmp3A1,sizeof(void*),0)));}
goto _LL10E;_LL113: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp1E9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1E9->tag != 4)goto _LL115;}_LL114:
# 990
{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_Tcutil_terr(loc,((_tmp3A5="top level region declarations are not implemented",_tag_dyneither(_tmp3A5,sizeof(char),50))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
goto _LL10E;_LL115: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp1EA=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1EA->tag != 5)goto _LL117;}_LL116:
# 993
{const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_terr(loc,((_tmp3A8="top level alias declarations are not implemented",_tag_dyneither(_tmp3A8,sizeof(char),49))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
goto _LL10E;_LL117: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1EB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1EB->tag != 0)goto _LL119;else{_tmp1EC=_tmp1EB->f1;}}_LL118:
# 996
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EC->sc,in_externCinclude))
_tmp1EC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp1EC,check_var_init,in_externCinclude,exports);
goto _LL10E;_LL119: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1ED=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1ED->tag != 1)goto _LL11B;else{_tmp1EE=_tmp1ED->f1;}}_LL11A:
# 1001
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EE->sc,in_externCinclude))
_tmp1EE->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp1EE,in_externCinclude,exports);
goto _LL10E;_LL11B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1EF=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1EF->tag != 9)goto _LL11D;else{_tmp1F0=_tmp1EF->f1;}}_LL11C:
# 1006
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp1F0);
goto _LL10E;_LL11D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1F1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F1->tag != 6)goto _LL11F;else{_tmp1F2=_tmp1F1->f1;}}_LL11E:
# 1009
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F2->sc,in_externCinclude))
_tmp1F2->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp1F2);
goto _LL10E;_LL11F: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1F3=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F3->tag != 7)goto _LL121;else{_tmp1F4=_tmp1F3->f1;}}_LL120:
# 1014
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F4->sc,in_externCinclude))
_tmp1F4->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp1F4);
goto _LL10E;_LL121: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1F5=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F5->tag != 8)goto _LL123;else{_tmp1F6=_tmp1F5->f1;}}_LL122:
# 1019
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1F6->sc,in_externCinclude))
_tmp1F6->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp1F6);
goto _LL10E;_LL123: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp1F7=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F7->tag != 14)goto _LL125;}_LL124:
# 1024
{const char*_tmp3AB;void*_tmp3AA;(_tmp3AA=0,Cyc_Tcutil_warn(d->loc,((_tmp3AB="spurious __cyclone_port_on__",_tag_dyneither(_tmp3AB,sizeof(char),29))),_tag_dyneither(_tmp3AA,sizeof(void*),0)));}
goto _LL10E;_LL125: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp1F8=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F8->tag != 15)goto _LL127;}_LL126:
# 1027
 goto _LL10E;_LL127: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1F9=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1F9->tag != 10)goto _LL129;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;}}_LL128: {
# 1030
struct Cyc_List_List*_tmp20D=te->ns;
struct Cyc_List_List*_tmp3AC;struct Cyc_List_List*_tmp20E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20D,((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->hd=_tmp1FA,((_tmp3AC->tl=0,_tmp3AC)))))));
# 1034
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp1FA)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp1FA);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp20E,Cyc_Tcenv_empty_genv(grgn));}
# 1039
te->ns=_tmp20E;
Cyc_Tc_tc_decls(te,_tmp1FB,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp20D;
goto _LL10E;}_LL129: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp1FC=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp1FC->tag != 11)goto _LL12B;else{_tmp1FD=(_tmp1FC->f1)->f1;_tmp1FE=(_tmp1FC->f1)->f2;_tmp1FF=_tmp1FC->f2;}}_LL12A: {
# 1046
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp210=_tmp1FD;struct _dyneither_ptr*_tmp211;struct Cyc_List_List*_tmp212;struct _dyneither_ptr*_tmp213;struct Cyc_List_List*_tmp214;struct _dyneither_ptr*_tmp215;struct Cyc_List_List*_tmp216;_LL130: if((_tmp210.Loc_n).tag != 4)goto _LL132;_LL131:
 goto _LL133;_LL132: if((_tmp210.Rel_n).tag != 1)goto _LL134;if((struct Cyc_List_List*)(_tmp210.Rel_n).val != 0)goto _LL134;_LL133:
 goto _LL135;_LL134: if((_tmp210.C_n).tag != 3)goto _LL136;if((struct Cyc_List_List*)(_tmp210.C_n).val != 0)goto _LL136;_LL135:
 goto _LL137;_LL136: if((_tmp210.Abs_n).tag != 2)goto _LL138;if((struct Cyc_List_List*)(_tmp210.Abs_n).val != 0)goto _LL138;_LL137:
# 1053
 first=_tmp1FE;rest=0;goto _LL12F;_LL138: if((_tmp210.Rel_n).tag != 1)goto _LL13A;if((struct Cyc_List_List*)(_tmp210.Rel_n).val == 0)goto _LL13A;_tmp211=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp210.Rel_n).val)->hd;_tmp212=((struct Cyc_List_List*)(_tmp210.Rel_n).val)->tl;_LL139:
 _tmp213=_tmp211;_tmp214=_tmp212;goto _LL13B;_LL13A: if((_tmp210.C_n).tag != 3)goto _LL13C;if((struct Cyc_List_List*)(_tmp210.C_n).val == 0)goto _LL13C;_tmp213=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp210.C_n).val)->hd;_tmp214=((struct Cyc_List_List*)(_tmp210.C_n).val)->tl;_LL13B:
 _tmp215=_tmp213;_tmp216=_tmp214;goto _LL13D;_LL13C: if((_tmp210.Abs_n).tag != 2)goto _LL12F;if((struct Cyc_List_List*)(_tmp210.Abs_n).val == 0)goto _LL12F;_tmp215=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp210.Abs_n).val)->hd;_tmp216=((struct Cyc_List_List*)(_tmp210.Abs_n).val)->tl;_LL13D:
# 1057
 first=_tmp215;{struct Cyc_List_List*_tmp3AD;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp216,((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD->hd=_tmp1FE,((_tmp3AD->tl=0,_tmp3AD)))))));}goto _LL12F;_LL12F:;}{
# 1059
struct Cyc_List_List*_tmp218=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp3AE;ge->availables=((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->hd=_tmp218,((_tmp3AE->tl=ge->availables,_tmp3AE))))));}
Cyc_Tc_tc_decls(te,_tmp1FF,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL10E;};}_LL12B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp200=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp200->tag != 12)goto _LL12D;else{_tmp201=_tmp200->f1;}}_LL12C:
# 1066
 Cyc_Tc_tc_decls(te,_tmp201,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL10E;_LL12D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp202=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1E6;if(_tmp202->tag != 13)goto _LL10E;else{_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;}}_LL12E:
# 1070
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp204);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp204,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp203,1,1,check_var_init,grgn,& newexs);
# 1075
for(0;_tmp204 != 0;_tmp204=_tmp204->tl){
struct _tuple12*_tmp21A=(struct _tuple12*)_tmp204->hd;
if(!(*_tmp21A).f3){
const char*_tmp3B2;void*_tmp3B1[1];struct Cyc_String_pa_PrintArg_struct _tmp3B0;(_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp21A).f2)),((_tmp3B1[0]=& _tmp3B0,Cyc_Tcutil_warn((*_tmp21A).f1,((_tmp3B2="%s is exported but not defined",_tag_dyneither(_tmp3B2,sizeof(char),31))),_tag_dyneither(_tmp3B1,sizeof(void*),1)))))));}}
# 1081
goto _LL10E;};_LL10E:;}}
# 1086
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1088
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1105 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1107
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1109
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1113
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp21E=d->r;struct Cyc_Absyn_Vardecl*_tmp220;struct Cyc_List_List**_tmp222;struct Cyc_List_List**_tmp224;struct Cyc_List_List**_tmp226;struct Cyc_List_List**_tmp228;_LL13F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp21F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp21E;if(_tmp21F->tag != 0)goto _LL141;else{_tmp220=_tmp21F->f1;}}_LL140:
# 1118
 if((env->in_cinclude  || _tmp220->sc != Cyc_Absyn_Extern  && _tmp220->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp220->name,Cyc_Absyn_uniquergn_qvar))
# 1121
return 1;{
# 1123
union Cyc_Absyn_Nmspace _tmp22A;struct _dyneither_ptr*_tmp22B;struct _tuple0*_tmp229=_tmp220->name;_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp22C=_tmp22A;struct Cyc_List_List*_tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_List_List*_tmp22F;_LL14C: if((_tmp22C.Loc_n).tag != 4)goto _LL14E;_LL14D:
 ns=0;goto _LL14B;_LL14E: if((_tmp22C.Rel_n).tag != 1)goto _LL150;_tmp22D=(struct Cyc_List_List*)(_tmp22C.Rel_n).val;_LL14F:
 ns=_tmp22D;goto _LL14B;_LL150: if((_tmp22C.C_n).tag != 3)goto _LL152;_tmp22E=(struct Cyc_List_List*)(_tmp22C.C_n).val;_LL151:
 ns=_tmp22E;goto _LL14B;_LL152: if((_tmp22C.Abs_n).tag != 2)goto _LL14B;_tmp22F=(struct Cyc_List_List*)(_tmp22C.Abs_n).val;_LL153:
 ns=_tmp22F;goto _LL14B;_LL14B:;}{
# 1131
struct _tuple17*_tmp230=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp231=(*_tmp230).f1;
int _tmp232=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp231->ordinaries,_tmp22B)).f2;
if(!_tmp232)
(*_tmp230).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp230).f2,_tmp22B);
# 1139
return _tmp232;};};};_LL141: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp221=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp21E;if(_tmp221->tag != 12)goto _LL143;else{_tmp222=(struct Cyc_List_List**)& _tmp221->f1;}}_LL142:
 _tmp224=_tmp222;goto _LL144;_LL143: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp223=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp21E;if(_tmp223->tag != 11)goto _LL145;else{_tmp224=(struct Cyc_List_List**)& _tmp223->f2;}}_LL144:
 _tmp226=_tmp224;goto _LL146;_LL145: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp225=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp21E;if(_tmp225->tag != 10)goto _LL147;else{_tmp226=(struct Cyc_List_List**)& _tmp225->f2;}}_LL146:
# 1143
*_tmp226=Cyc_Tc_treeshake_f(env,*_tmp226);
return 1;_LL147: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp227=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp21E;if(_tmp227->tag != 13)goto _LL149;else{_tmp228=(struct Cyc_List_List**)& _tmp227->f1;}}_LL148: {
# 1146
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp228=Cyc_Tc_treeshake_f(env,*_tmp228);
env->in_cinclude=in_cinclude;
return 1;}_LL149:;_LL14A:
 return 1;_LL13E:;}
# 1155
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1159
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1161
struct _tuple17*_tmp3B3;return(_tmp3B3=_region_malloc(rgn,sizeof(*_tmp3B3)),((_tmp3B3->f1=ge,((_tmp3B3->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3B3)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1164
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp234=_new_region("rgn");struct _RegionHandle*rgn=& _tmp234;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp3B4;struct Cyc_Tc_TreeshakeEnv _tmp235=(_tmp3B4.rgn=rgn,((_tmp3B4.in_cinclude=0,((_tmp3B4.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3B4)))));
struct Cyc_List_List*_tmp236=Cyc_Tc_treeshake_f(& _tmp235,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp235.nsdict)){
struct Cyc_List_List*_tmp237=_tmp236;_npop_handler(0);return _tmp237;}{
# 1172
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp235.nsdict);
struct _tuple18 _tmp238=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp235.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp238)){
struct Cyc_Tcenv_Genv*_tmp23A;struct Cyc_Set_Set*_tmp23B;struct _tuple18 _tmp239=_tmp238;_tmp23A=(_tmp239.f2)->f1;_tmp23B=(_tmp239.f2)->f2;
_tmp23A->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp23A->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp23B,_tmp23A->ordinaries);}{
# 1178
struct Cyc_List_List*_tmp23C=_tmp236;_npop_handler(0);return _tmp23C;};};}
# 1166
;_pop_region(rgn);}
