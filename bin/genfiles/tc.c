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
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 922
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
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
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 116
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 144
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 158
void Cyc_Tcutil_explain_failure();
# 160
int Cyc_Tcutil_unify(void*,void*);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 197
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 227 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 229
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 237
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 241
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 283
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 318
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 329
int Cyc_Tcutil_bits_only(void*t);
# 332
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 342
int Cyc_Tcutil_supports_default(void*);
# 353
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 357
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
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
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmp2=_new_region("temp");struct _RegionHandle*temp=& _tmp2;_push_region(temp);
{struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
void*_stmttmp0=Cyc_Tcutil_compress(t);void*_tmp3=_stmttmp0;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp6;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3;if(_tmp4->tag != 9)goto _LL3;else{_tmp5=(_tmp4->f1).attributes;_tmp6=(_tmp4->f1).args;}}_LL2: {
# 70
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6);
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
void*_stmttmp1=(void*)_tmp5->hd;void*_tmp7=_stmttmp1;int _tmp9;int _tmpB;int _tmpD;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp7;if(_tmp8->tag != 20)goto _LL8;else{_tmp9=_tmp8->f1;}}_LL7:
# 74
 if(_tmp9 < 1  || _tmp9 > nargs){
const char*_tmp287;void*_tmp286[1];struct Cyc_Int_pa_PrintArg_struct _tmp285;(_tmp285.tag=1,((_tmp285.f1=(unsigned long)_tmp9,((_tmp286[0]=& _tmp285,Cyc_Tcutil_terr(loc,((_tmp287="initializes attribute: index %d out of range",_tag_dyneither(_tmp287,sizeof(char),45))),_tag_dyneither(_tmp286,sizeof(void*),1)))))));}
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmp9)){
struct Cyc_Int_pa_PrintArg_struct _tmp28F;void*_tmp28E[1];const char*_tmp28D;void*_tmp28C;(_tmp28C=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp28F.tag=1,((_tmp28F.f1=(unsigned long)_tmp9,((_tmp28E[0]=& _tmp28F,Cyc_rprintf(temp,((_tmp28D="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp28D,sizeof(char),73))),_tag_dyneither(_tmp28E,sizeof(void*),1)))))))),_tag_dyneither(_tmp28C,sizeof(void*),0)));}
# 80
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmp9)){
struct Cyc_Int_pa_PrintArg_struct _tmp297;void*_tmp296[1];const char*_tmp295;void*_tmp294;(_tmp294=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp297.tag=1,((_tmp297.f1=(unsigned long)_tmp9,((_tmp296[0]=& _tmp297,Cyc_rprintf(temp,((_tmp295="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp295,sizeof(char),70))),_tag_dyneither(_tmp296,sizeof(void*),1)))))))),_tag_dyneither(_tmp294,sizeof(void*),0)));}
# 83
{struct Cyc_List_List*_tmp298;init_params=((_tmp298=_region_malloc(temp,sizeof(*_tmp298)),((_tmp298->hd=(void*)_tmp9,((_tmp298->tl=init_params,_tmp298))))));}goto _LL5;_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp7;if(_tmpA->tag != 21)goto _LLA;else{_tmpB=_tmpA->f1;}}_LL9:
# 85
 if(_tmpB < 1  || _tmpB > nargs){
const char*_tmp29C;void*_tmp29B[1];struct Cyc_Int_pa_PrintArg_struct _tmp29A;(_tmp29A.tag=1,((_tmp29A.f1=(unsigned long)_tmpB,((_tmp29B[0]=& _tmp29A,Cyc_Tcutil_terr(loc,((_tmp29C="noliveunique attribute: index %d out of range",_tag_dyneither(_tmp29C,sizeof(char),46))),_tag_dyneither(_tmp29B,sizeof(void*),1)))))));}
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpB)){
struct Cyc_Int_pa_PrintArg_struct _tmp2A4;void*_tmp2A3[1];const char*_tmp2A2;void*_tmp2A1;(_tmp2A1=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2A4.tag=1,((_tmp2A4.f1=(unsigned long)_tmpB,((_tmp2A3[0]=& _tmp2A4,Cyc_rprintf(temp,((_tmp2A2="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp2A2,sizeof(char),73))),_tag_dyneither(_tmp2A3,sizeof(void*),1)))))))),_tag_dyneither(_tmp2A1,sizeof(void*),0)));}
# 91
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmpB)){
struct Cyc_Int_pa_PrintArg_struct _tmp2AC;void*_tmp2AB[1];const char*_tmp2AA;void*_tmp2A9;(_tmp2A9=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2AC.tag=1,((_tmp2AC.f1=(unsigned long)_tmpB,((_tmp2AB[0]=& _tmp2AC,Cyc_rprintf(temp,((_tmp2AA="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2AA,sizeof(char),71))),_tag_dyneither(_tmp2AB,sizeof(void*),1)))))))),_tag_dyneither(_tmp2A9,sizeof(void*),0)));}
# 94
{struct Cyc_List_List*_tmp2AD;nolive_unique_params=((_tmp2AD=_region_malloc(temp,sizeof(*_tmp2AD)),((_tmp2AD->hd=(void*)_tmpB,((_tmp2AD->tl=nolive_unique_params,_tmp2AD))))));}goto _LL5;_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpC=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp7;if(_tmpC->tag != 22)goto _LLC;else{_tmpD=_tmpC->f1;}}_LLB:
# 96
 if(_tmpD < 1  || _tmpD > nargs){
{const char*_tmp2B1;void*_tmp2B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp2AF;(_tmp2AF.tag=1,((_tmp2AF.f1=(unsigned long)_tmpD,((_tmp2B0[0]=& _tmp2AF,Cyc_Tcutil_terr(loc,((_tmp2B1="noconsume attribute: index %d out of range",_tag_dyneither(_tmp2B1,sizeof(char),43))),_tag_dyneither(_tmp2B0,sizeof(void*),1)))))));}
continue;}
# 100
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpD)){
struct Cyc_Int_pa_PrintArg_struct _tmp2B9;void*_tmp2B8[1];const char*_tmp2B7;void*_tmp2B6;(_tmp2B6=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2B9.tag=1,((_tmp2B9.f1=(unsigned long)_tmpD,((_tmp2B8[0]=& _tmp2B9,Cyc_rprintf(temp,((_tmp2B7="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2B7,sizeof(char),70))),_tag_dyneither(_tmp2B8,sizeof(void*),1)))))))),_tag_dyneither(_tmp2B6,sizeof(void*),0)));}
# 104
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmpD)){
struct Cyc_Int_pa_PrintArg_struct _tmp2C1;void*_tmp2C0[1];const char*_tmp2BF;void*_tmp2BE;(_tmp2BE=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp2C1.tag=1,((_tmp2C1.f1=(unsigned long)_tmpD,((_tmp2C0[0]=& _tmp2C1,Cyc_rprintf(temp,((_tmp2BF="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp2BF,sizeof(char),71))),_tag_dyneither(_tmp2C0,sizeof(void*),1)))))))),_tag_dyneither(_tmp2BE,sizeof(void*),0)));}{
# 107
struct _tuple8*_stmttmp2=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6,_tmpD - 1);void*_tmp32;struct _tuple8*_tmp31=_stmttmp2;_tmp32=_tmp31->f3;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp32,0)){
const char*_tmp2C4;void*_tmp2C3;(_tmp2C3=0,Cyc_Tcutil_terr(loc,((_tmp2C4="noconsume attribute only allowed on unique pointers",_tag_dyneither(_tmp2C4,sizeof(char),52))),_tag_dyneither(_tmp2C3,sizeof(void*),0)));}
{struct Cyc_List_List*_tmp2C5;noconsume_params=((_tmp2C5=_region_malloc(temp,sizeof(*_tmp2C5)),((_tmp2C5->hd=(void*)_tmpD,((_tmp2C5->tl=noconsume_params,_tmp2C5))))));}goto _LL5;};_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 114
goto _LL0;}_LL3:;_LL4: {
# 116
const char*_tmp2C8;void*_tmp2C7;(_tmp2C7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2C8="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2C8,sizeof(char),34))),_tag_dyneither(_tmp2C7,sizeof(void*),0)));}_LL0:;}
# 65
;_pop_region(temp);}struct _tuple13{void*f1;int f2;};
# 121
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 125
enum Cyc_Absyn_Scope _tmp39;union Cyc_Absyn_Nmspace _tmp3A;struct _dyneither_ptr*_tmp3B;void*_tmp3C;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_List_List*_tmp3E;struct Cyc_Absyn_Vardecl*_tmp38=vd;_tmp39=_tmp38->sc;_tmp3A=(_tmp38->name)->f1;_tmp3B=(_tmp38->name)->f2;_tmp3C=_tmp38->type;_tmp3D=_tmp38->initializer;_tmp3E=_tmp38->attributes;
# 131
{union Cyc_Absyn_Nmspace _tmp3F=_tmp3A;_LLF: if((_tmp3F.Rel_n).tag != 1)goto _LL11;_LL10:
# 134
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LLE;_LL11:;_LL12:
 goto _LLE;_LLE:;}
# 140
{void*_stmttmp3=Cyc_Tcutil_compress(_tmp3C);void*_tmp40=_stmttmp3;void*_tmp42;struct Cyc_Absyn_Tqual _tmp43;union Cyc_Absyn_Constraint*_tmp44;unsigned int _tmp45;_LL14:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 8)goto _LL16;else{_tmp42=(_tmp41->f1).elt_type;_tmp43=(_tmp41->f1).tq;if((_tmp41->f1).num_elts != 0)goto _LL16;_tmp44=(_tmp41->f1).zero_term;_tmp45=(_tmp41->f1).zt_loc;}}if(!(_tmp3D != 0))goto _LL16;_LL15:
# 142
{void*_stmttmp4=_tmp3D->r;void*_tmp46=_stmttmp4;struct _dyneither_ptr _tmp48;struct _dyneither_ptr _tmp4A;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp52;_LL19: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp47=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp47->tag != 0)goto _LL1B;else{if(((_tmp47->f1).String_c).tag != 8)goto _LL1B;_tmp48=(struct _dyneither_ptr)((_tmp47->f1).String_c).val;}}_LL1A:
# 144
 _tmp3C=(vd->type=Cyc_Absyn_array_typ(_tmp42,_tmp43,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp48,sizeof(char)),0),_tmp44,_tmp45));
goto _LL18;_LL1B: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp49=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp49->tag != 0)goto _LL1D;else{if(((_tmp49->f1).Wstring_c).tag != 9)goto _LL1D;_tmp4A=(struct _dyneither_ptr)((_tmp49->f1).Wstring_c).val;}}_LL1C:
# 147
 _tmp3C=(vd->type=Cyc_Absyn_array_typ(_tmp42,_tmp43,Cyc_Absyn_uint_exp(1,0),_tmp44,_tmp45));
goto _LL18;_LL1D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp4B->tag != 26)goto _LL1F;else{_tmp4C=_tmp4B->f2;}}_LL1E:
 _tmp4E=_tmp4C;goto _LL20;_LL1F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp4D->tag != 27)goto _LL21;else{_tmp4E=_tmp4D->f1;}}_LL20:
# 152
 _tmp3C=(vd->type=Cyc_Absyn_array_typ(_tmp42,_tmp43,_tmp4E,_tmp44,_tmp45));
goto _LL18;_LL21: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp4F->tag != 35)goto _LL23;else{_tmp50=_tmp4F->f2;}}_LL22:
 _tmp52=_tmp50;goto _LL24;_LL23: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp51=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp46;if(_tmp51->tag != 25)goto _LL25;else{_tmp52=_tmp51->f1;}}_LL24:
# 156
 _tmp3C=(vd->type=Cyc_Absyn_array_typ(_tmp42,_tmp43,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp52),0),_tmp44,_tmp45));
goto _LL18;_LL25:;_LL26:
 goto _LL18;_LL18:;}
# 160
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 164
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp3C);
# 166
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp3C);
# 169
{void*_stmttmp5=Cyc_Tcutil_compress(_tmp3C);void*_tmp53=_stmttmp5;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53;if(_tmp54->tag != 8)goto _LL2A;}_LL29:
 vd->escapes=0;goto _LL27;_LL2A:;_LL2B:
 vd->escapes=1;goto _LL27;_LL27:;}
# 174
if(Cyc_Tcutil_is_function_type(_tmp3C)){
_tmp3E=Cyc_Tcutil_transfer_fn_type_atts(_tmp3C,_tmp3E);
Cyc_Tc_fnTypeAttsOK(te,_tmp3C,loc);}
# 179
if(_tmp39 == Cyc_Absyn_Extern  || _tmp39 == Cyc_Absyn_ExternC){
if(_tmp3D != 0){
const char*_tmp2CB;void*_tmp2CA;(_tmp2CA=0,Cyc_Tcutil_terr(loc,((_tmp2CB="extern declaration should not have initializer",_tag_dyneither(_tmp2CB,sizeof(char),47))),_tag_dyneither(_tmp2CA,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp3C)){
# 186
for(0;_tmp3E != 0;_tmp3E=_tmp3E->tl){
void*_stmttmp6=(void*)_tmp3E->hd;void*_tmp57=_stmttmp6;_LL2D: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp58=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp57;if(_tmp58->tag != 6)goto _LL2F;}_LL2E:
 goto _LL30;_LL2F: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp59=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp57;if(_tmp59->tag != 8)goto _LL31;}_LL30:
# 191
 goto _LL32;_LL31: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp5A=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5A->tag != 9)goto _LL33;}_LL32:
 goto _LL34;_LL33: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp5B=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5B->tag != 10)goto _LL35;}_LL34:
 goto _LL36;_LL35: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp5C=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5C->tag != 11)goto _LL37;}_LL36:
 goto _LL38;_LL37: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp5D=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5D->tag != 12)goto _LL39;}_LL38:
 goto _LL3A;_LL39: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp5E=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5E->tag != 13)goto _LL3B;}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp5F=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp57;if(_tmp5F->tag != 14)goto _LL3D;}_LL3C:
 continue;_LL3D:;_LL3E:
# 199
{const char*_tmp2D0;void*_tmp2CF[2];struct Cyc_String_pa_PrintArg_struct _tmp2CE;struct Cyc_String_pa_PrintArg_struct _tmp2CD;(_tmp2CD.tag=0,((_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2CE.tag=0,((_tmp2CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp3E->hd)),((_tmp2CF[0]=& _tmp2CE,((_tmp2CF[1]=& _tmp2CD,Cyc_Tcutil_terr(loc,((_tmp2D0="bad attribute %s for variable %s",_tag_dyneither(_tmp2D0,sizeof(char),33))),_tag_dyneither(_tmp2CF,sizeof(void*),2)))))))))))));}
goto _LL2C;_LL2C:;}
# 204
if(_tmp3D == 0  || in_cinclude){
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp3C)){
const char*_tmp2D5;void*_tmp2D4[2];struct Cyc_String_pa_PrintArg_struct _tmp2D3;struct Cyc_String_pa_PrintArg_struct _tmp2D2;(_tmp2D2.tag=0,((_tmp2D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp3C)),((_tmp2D3.tag=0,((_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2D4[0]=& _tmp2D3,((_tmp2D4[1]=& _tmp2D2,Cyc_Tcutil_terr(loc,((_tmp2D5="initializer required for variable %s of type %s",_tag_dyneither(_tmp2D5,sizeof(char),48))),_tag_dyneither(_tmp2D4,sizeof(void*),2)))))))))))));}}else{
# 209
struct Cyc_Absyn_Exp*_tmp68=_tmp3D;
# 213
struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){
{void*_tmp6C=Cyc_Tcexp_tcExpInitializer(te,& _tmp3C,_tmp68);
if(!Cyc_Tcutil_coerce_assign(te,_tmp68,_tmp3C)){
struct _dyneither_ptr _tmp6D=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp6E=" declared with type ";
struct _dyneither_ptr _tmp6F=Cyc_Absynpp_typ2string(_tmp3C);
const char*_tmp70=" but initialized with type ";
struct _dyneither_ptr _tmp71=Cyc_Absynpp_typ2string(_tmp6C);
{const char*_tmp2D7;const char*_tmp2D6;if((((Cyc_strlen((struct _dyneither_ptr)_tmp6D)+ Cyc_strlen(((_tmp2D6=_tmp6E,_tag_dyneither(_tmp2D6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D6,21))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp6F))+ Cyc_strlen(((_tmp2D7=_tmp70,_tag_dyneither(_tmp2D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D7,28))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp71)> 70){
const char*_tmp2E3;void*_tmp2E2[5];struct Cyc_String_pa_PrintArg_struct _tmp2E1;const char*_tmp2E0;struct Cyc_String_pa_PrintArg_struct _tmp2DF;struct Cyc_String_pa_PrintArg_struct _tmp2DE;const char*_tmp2DD;struct Cyc_String_pa_PrintArg_struct _tmp2DC;struct Cyc_String_pa_PrintArg_struct _tmp2DB;(_tmp2DB.tag=0,((_tmp2DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp71),((_tmp2DC.tag=0,((_tmp2DC.f1=(struct _dyneither_ptr)((_tmp2DD=_tmp70,_tag_dyneither(_tmp2DD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2DD,28)))),((_tmp2DE.tag=0,((_tmp2DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6F),((_tmp2DF.tag=0,((_tmp2DF.f1=(struct _dyneither_ptr)((_tmp2E0=_tmp6E,_tag_dyneither(_tmp2E0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E0,21)))),((_tmp2E1.tag=0,((_tmp2E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6D),((_tmp2E2[0]=& _tmp2E1,((_tmp2E2[1]=& _tmp2DF,((_tmp2E2[2]=& _tmp2DE,((_tmp2E2[3]=& _tmp2DC,((_tmp2E2[4]=& _tmp2DB,Cyc_Tcutil_terr(loc,((_tmp2E3="%s%s\n\t%s\n%s\n\t%s",_tag_dyneither(_tmp2E3,sizeof(char),16))),_tag_dyneither(_tmp2E2,sizeof(void*),5)))))))))))))))))))))))))))))));}else{
# 224
const char*_tmp2EF;void*_tmp2EE[5];struct Cyc_String_pa_PrintArg_struct _tmp2ED;const char*_tmp2EC;struct Cyc_String_pa_PrintArg_struct _tmp2EB;struct Cyc_String_pa_PrintArg_struct _tmp2EA;const char*_tmp2E9;struct Cyc_String_pa_PrintArg_struct _tmp2E8;struct Cyc_String_pa_PrintArg_struct _tmp2E7;(_tmp2E7.tag=0,((_tmp2E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp71),((_tmp2E8.tag=0,((_tmp2E8.f1=(struct _dyneither_ptr)((_tmp2E9=_tmp70,_tag_dyneither(_tmp2E9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E9,28)))),((_tmp2EA.tag=0,((_tmp2EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6F),((_tmp2EB.tag=0,((_tmp2EB.f1=(struct _dyneither_ptr)((_tmp2EC=_tmp6E,_tag_dyneither(_tmp2EC,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EC,21)))),((_tmp2ED.tag=0,((_tmp2ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6D),((_tmp2EE[0]=& _tmp2ED,((_tmp2EE[1]=& _tmp2EB,((_tmp2EE[2]=& _tmp2EA,((_tmp2EE[3]=& _tmp2E8,((_tmp2EE[4]=& _tmp2E7,Cyc_Tcutil_terr(loc,((_tmp2EF="%s%s%s%s%s",_tag_dyneither(_tmp2EF,sizeof(char),11))),_tag_dyneither(_tmp2EE,sizeof(void*),5)))))))))))))))))))))))))))))));}}
Cyc_Tcutil_explain_failure();
# 227
if(!Cyc_Tcutil_is_const_exp(_tmp68)){
const char*_tmp2F2;void*_tmp2F1;(_tmp2F1=0,Cyc_Tcutil_terr(loc,((_tmp2F2="initializer is not a constant expression",_tag_dyneither(_tmp2F2,sizeof(char),41))),_tag_dyneither(_tmp2F1,sizeof(void*),0)));}}}
# 214
;_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp89=_tmp6A;void*_tmp8B;_LL40: {struct Cyc_Tcenv_Env_error_exn_struct*_tmp8A=(struct Cyc_Tcenv_Env_error_exn_struct*)_tmp89;if(_tmp8A->tag != Cyc_Tcenv_Env_error)goto _LL42;}_LL41:
# 232
{const char*_tmp2F5;void*_tmp2F4;(_tmp2F4=0,Cyc_Tcutil_terr(loc,((_tmp2F5="initializer is not a constant expression",_tag_dyneither(_tmp2F5,sizeof(char),41))),_tag_dyneither(_tmp2F4,sizeof(void*),0)));}
goto _LL3F;_LL42: _tmp8B=_tmp89;_LL43:(void)_throw(_tmp8B);_LL3F:;}};}}else{
# 238
for(0;_tmp3E != 0;_tmp3E=_tmp3E->tl){
void*_stmttmp7=(void*)_tmp3E->hd;void*_tmp8E=_stmttmp7;_LL45: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp8F=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp8F->tag != 0)goto _LL47;}_LL46:
# 241
 goto _LL48;_LL47: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp90=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp90->tag != 1)goto _LL49;}_LL48:
 goto _LL4A;_LL49: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp91=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp91->tag != 2)goto _LL4B;}_LL4A:
 goto _LL4C;_LL4B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp92=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp92->tag != 3)goto _LL4D;}_LL4C:
 goto _LL4E;_LL4D: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp93=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp93->tag != 4)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp94=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp94->tag != 19)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp95=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp95->tag != 20)goto _LL53;}_LL52:
 goto _LL54;_LL53: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp96=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp96->tag != 23)goto _LL55;}_LL54:
 goto _LL56;_LL55: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp97=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp97->tag != 5)goto _LL57;}_LL56: {
const char*_tmp2F8;void*_tmp2F7;(_tmp2F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2F8="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2F8,sizeof(char),45))),_tag_dyneither(_tmp2F7,sizeof(void*),0)));}_LL57: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp98=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp98->tag != 6)goto _LL59;}_LL58:
# 251
 goto _LL5A;_LL59: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp99=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8E;if(_tmp99->tag != 7)goto _LL5B;}_LL5A:
# 253
{const char*_tmp2FC;void*_tmp2FB[1];struct Cyc_String_pa_PrintArg_struct _tmp2FA;(_tmp2FA.tag=0,((_tmp2FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp3E->hd)),((_tmp2FB[0]=& _tmp2FA,Cyc_Tcutil_terr(loc,((_tmp2FC="bad attribute %s in function declaration",_tag_dyneither(_tmp2FC,sizeof(char),41))),_tag_dyneither(_tmp2FB,sizeof(void*),1)))))));}
goto _LL44;_LL5B:;_LL5C:
 continue;_LL44:;}}}
# 261
{struct _handler_cons _tmp9F;_push_handler(& _tmp9F);{int _tmpA1=0;if(setjmp(_tmp9F.handler))_tmpA1=1;if(!_tmpA1){
{struct _tuple13*_tmpA2=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp3B);
void*_stmttmp8=(*_tmpA2).f1;void*_tmpA3=_stmttmp8;void*_tmpA5;_LL5E: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL60;else{_tmpA5=(void*)_tmpA4->f1;}}_LL5F: {
# 265
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2FF;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2FE;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpAA=(_tmp2FE=_cycalloc(sizeof(*_tmp2FE)),((_tmp2FE[0]=((_tmp2FF.tag=1,((_tmp2FF.f1=vd,_tmp2FF)))),_tmp2FE)));
void*_tmpAB=Cyc_Tcdecl_merge_binding(_tmpA5,(void*)_tmpAA,loc,Cyc_Tc_tc_msg);
if(_tmpAB == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 269
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmpAB != _tmpA5  || (*_tmpA2).f2){
# 272
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp305;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp304;struct _tuple13*_tmp303;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3B,(
(_tmp303=_cycalloc(sizeof(*_tmp303)),((_tmp303->f1=(void*)((_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=((_tmp304.tag=0,((_tmp304.f1=_tmpAB,_tmp304)))),_tmp305)))),((_tmp303->f2=1,_tmp303)))))));}}
_npop_handler(0);return;}_LL60: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 1)goto _LL62;}_LL61:
# 276
{const char*_tmp308;void*_tmp307;(_tmp307=0,Cyc_Tcutil_warn(loc,((_tmp308="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp308,sizeof(char),57))),_tag_dyneither(_tmp307,sizeof(void*),0)));}
goto _LL5D;_LL62: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA7=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA7->tag != 2)goto _LL64;}_LL63:
# 279
{const char*_tmp30B;void*_tmp30A;(_tmp30A=0,Cyc_Tcutil_warn(loc,((_tmp30B="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp30B,sizeof(char),59))),_tag_dyneither(_tmp30A,sizeof(void*),0)));}
goto _LL5D;_LL64: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 4)goto _LL66;}_LL65:
 goto _LL67;_LL66: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA9=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA9->tag != 3)goto _LL5D;}_LL67:
# 283
{const char*_tmp30E;void*_tmp30D;(_tmp30D=0,Cyc_Tcutil_warn(loc,((_tmp30E="variable declaration shadows previous enum tag",_tag_dyneither(_tmp30E,sizeof(char),47))),_tag_dyneither(_tmp30D,sizeof(void*),0)));}
goto _LL5D;_LL5D:;}
# 262
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpB8=_tmpA0;void*_tmpBA;_LL69: {struct Cyc_Dict_Absent_exn_struct*_tmpB9=(struct Cyc_Dict_Absent_exn_struct*)_tmpB8;if(_tmpB9->tag != Cyc_Dict_Absent)goto _LL6B;}_LL6A:
# 286
 goto _LL68;_LL6B: _tmpBA=_tmpB8;_LL6C:(void)_throw(_tmpBA);_LL68:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp31D;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp31C;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp31B;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp31A;struct _tuple13*_tmp319;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3B,(
(_tmp319=_cycalloc(sizeof(*_tmp319)),((_tmp319->f1=(void*)((_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=((_tmp31A.tag=0,((_tmp31A.f1=(void*)((_tmp31C=_cycalloc(sizeof(*_tmp31C)),((_tmp31C[0]=((_tmp31B.tag=1,((_tmp31B.f1=vd,_tmp31B)))),_tmp31C)))),_tmp31A)))),_tmp31D)))),((_tmp319->f2=0,_tmp319)))))));}}
# 292
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmpC1;struct _dyneither_ptr*_tmpC2;struct _tuple0*_tmpC0=n;_tmpC1=_tmpC0->f1;_tmpC2=_tmpC0->f2;{
union Cyc_Absyn_Nmspace _tmpC3=_tmpC1;_LL6E: if((_tmpC3.Abs_n).tag != 2)goto _LL70;if((struct Cyc_List_List*)(_tmpC3.Abs_n).val != 0)goto _LL70;_LL6F: {
# 296
const char*_tmp31E;return Cyc_strcmp((struct _dyneither_ptr)*_tmpC2,((_tmp31E="main",_tag_dyneither(_tmp31E,sizeof(char),5))))== 0;}_LL70:;_LL71:
 return 0;_LL6D:;};}
# 301
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 304
struct _dyneither_ptr*v=(*fd->name).f2;
# 308
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp321;void*_tmp320;(_tmp320=0,Cyc_Tcutil_terr(loc,((_tmp321="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp321,sizeof(char),54))),_tag_dyneither(_tmp320,sizeof(void*),0)));}
# 311
{union Cyc_Absyn_Nmspace _stmttmp9=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpC7=_stmttmp9;_LL73: if((_tmpC7.Rel_n).tag != 1)goto _LL75;if((struct Cyc_List_List*)(_tmpC7.Rel_n).val != 0)goto _LL75;_LL74:
# 314
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL72;_LL75:;_LL76:
# 317
 goto _LL72;_LL72:;}
# 320
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 322
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 324
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 327
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmpA=(void*)atts->hd;void*_tmpC8=_stmttmpA;_LL78: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpC9=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpC8;if(_tmpC9->tag != 7)goto _LL7A;}_LL79:
 goto _LL7B;_LL7A: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpCA=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpC8;if(_tmpCA->tag != 6)goto _LL7C;}_LL7B:
# 331
{const char*_tmp325;void*_tmp324[1];struct Cyc_String_pa_PrintArg_struct _tmp323;(_tmp323.tag=0,((_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp324[0]=& _tmp323,Cyc_Tcutil_terr(loc,((_tmp325="bad attribute %s for function",_tag_dyneither(_tmp325,sizeof(char),30))),_tag_dyneither(_tmp324,sizeof(void*),1)))))));}
goto _LL77;_LL7C:;_LL7D:
 goto _LL77;_LL77:;}}
# 337
{struct _handler_cons _tmpCE;_push_handler(& _tmpCE);{int _tmpD0=0;if(setjmp(_tmpCE.handler))_tmpD0=1;if(!_tmpD0){
{struct _tuple13*_tmpD1=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_stmttmpB=(*_tmpD1).f1;void*_tmpD2=_stmttmpB;void*_tmpD4;_LL7F: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD3=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpD2;if(_tmpD3->tag != 0)goto _LL81;else{_tmpD4=(void*)_tmpD3->f1;}}_LL80: {
# 341
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp328;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp327;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD9=(_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=((_tmp328.tag=2,((_tmp328.f1=fd,_tmp328)))),_tmp327)));
void*_tmpDA=Cyc_Tcdecl_merge_binding(_tmpD4,(void*)_tmpD9,loc,Cyc_Tc_tc_msg);
if(_tmpDA == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL7E;
# 345
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpDA == _tmpD4  && (*_tmpD1).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp32E;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp32D;struct _tuple13*_tmp32C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C->f1=(void*)((_tmp32E=_cycalloc(sizeof(*_tmp32E)),((_tmp32E[0]=((_tmp32D.tag=0,((_tmp32D.f1=_tmpDA,_tmp32D)))),_tmp32E)))),((_tmp32C->f2=1,_tmp32C)))))));}}
goto _LL7E;}_LL81: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpD5=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpD2;if(_tmpD5->tag != 1)goto _LL83;}_LL82:
# 351
{const char*_tmp331;void*_tmp330;(_tmp330=0,Cyc_Tcutil_warn(loc,((_tmp331="function declaration shadows previous type declaration",_tag_dyneither(_tmp331,sizeof(char),55))),_tag_dyneither(_tmp330,sizeof(void*),0)));}
goto _LL7E;_LL83: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpD6=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpD2;if(_tmpD6->tag != 2)goto _LL85;}_LL84:
# 354
{const char*_tmp334;void*_tmp333;(_tmp333=0,Cyc_Tcutil_warn(loc,((_tmp334="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp334,sizeof(char),59))),_tag_dyneither(_tmp333,sizeof(void*),0)));}
goto _LL7E;_LL85: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpD7=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpD2;if(_tmpD7->tag != 4)goto _LL87;}_LL86:
 goto _LL88;_LL87: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpD8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpD2;if(_tmpD8->tag != 3)goto _LL7E;}_LL88:
# 358
{const char*_tmp337;void*_tmp336;(_tmp336=0,Cyc_Tcutil_warn(loc,((_tmp337="function declaration shadows previous enum tag",_tag_dyneither(_tmp337,sizeof(char),47))),_tag_dyneither(_tmp336,sizeof(void*),0)));}
goto _LL7E;_LL7E:;}
# 338
;_pop_handler();}else{void*_tmpCF=(void*)_exn_thrown;void*_tmpE7=_tmpCF;void*_tmpE9;_LL8A: {struct Cyc_Dict_Absent_exn_struct*_tmpE8=(struct Cyc_Dict_Absent_exn_struct*)_tmpE7;if(_tmpE8->tag != Cyc_Dict_Absent)goto _LL8C;}_LL8B:
# 362
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp33A;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp339;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpEA=(_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339[0]=((_tmp33A.tag=2,((_tmp33A.f1=fd,_tmp33A)))),_tmp339)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp349;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp348;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp347;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp346;struct _tuple13*_tmp345;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->f1=(void*)((_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349[0]=((_tmp346.tag=0,((_tmp346.f1=(void*)((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=((_tmp347.tag=2,((_tmp347.f1=fd,_tmp347)))),_tmp348)))),_tmp346)))),_tmp349)))),((_tmp345->f2=0,_tmp345)))))));}
# 367
goto _LL89;_LL8C: _tmpE9=_tmpE7;_LL8D:(void)_throw(_tmpE9);_LL89:;}};}
# 371
if(in_cinclude)return;{
# 376
struct _RegionHandle _tmpF2=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpF2;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmpF3=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp34A;struct Cyc_Tcenv_Tenv*_tmpF4=(_tmp34A=_region_malloc(fnrgn,sizeof(*_tmp34A)),((_tmp34A->ns=te->ns,((_tmp34A->ae=te->ae,((_tmp34A->le=_tmpF3,((_tmp34A->allow_valueof=0,_tmp34A)))))))));
# 380
struct _handler_cons _tmpF5;_push_handler(& _tmpF5);{int _tmpF7=0;if(setjmp(_tmpF5.handler))_tmpF7=1;if(!_tmpF7){
Cyc_Tcstmt_tcStmt(_tmpF4,fd->body,0);
# 384
Cyc_Tcenv_check_delayed_effects(_tmpF4);
Cyc_Tcenv_check_delayed_constraints(_tmpF4);
# 388
if(!Cyc_Tcenv_all_labels_resolved(_tmpF4)){
const char*_tmp34D;void*_tmp34C;(_tmp34C=0,Cyc_Tcutil_terr(loc,((_tmp34D="function has goto statements to undefined labels",_tag_dyneither(_tmp34D,sizeof(char),49))),_tag_dyneither(_tmp34C,sizeof(void*),0)));}
# 381
;_pop_handler();}else{void*_tmpF6=(void*)_exn_thrown;void*_tmpFB=_tmpF6;void*_tmpFD;void*_tmpFE;_LL8F: {struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmpFC=(struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmpFB;if(_tmpFC->tag != Cyc_Tcutil_AbortTypeCheckingFunction)goto _LL91;}_LL90:
# 391
 goto _LL8E;_LL91: _tmpFD=_tmpFB;_LL92:
 Cyc_Core_rethrow(_tmpFD);_LL93: _tmpFE=_tmpFB;_LL94:(void)_throw(_tmpFE);_LL8E:;}};}
# 396
if(Cyc_Tc_is_main(fd->name)){
# 398
{void*_stmttmpC=Cyc_Tcutil_compress(fd->ret_type);void*_tmp100=_stmttmpC;enum Cyc_Absyn_Size_of _tmp103;_LL96: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp101=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp100;if(_tmp101->tag != 0)goto _LL98;}_LL97:
{const char*_tmp350;void*_tmp34F;(_tmp34F=0,Cyc_Tcutil_warn(loc,((_tmp350="main declared with return type void",_tag_dyneither(_tmp350,sizeof(char),36))),_tag_dyneither(_tmp34F,sizeof(void*),0)));}goto _LL95;_LL98: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp102=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp100;if(_tmp102->tag != 6)goto _LL9A;else{_tmp103=_tmp102->f2;}}_LL99:
 goto _LL95;_LL9A:;_LL9B:
# 402
{const char*_tmp354;void*_tmp353[1];struct Cyc_String_pa_PrintArg_struct _tmp352;(_tmp352.tag=0,((_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp353[0]=& _tmp352,Cyc_Tcutil_terr(loc,((_tmp354="main declared with return type %s instead of int or void",_tag_dyneither(_tmp354,sizeof(char),57))),_tag_dyneither(_tmp353,sizeof(void*),1)))))));}
goto _LL95;_LL95:;}
# 406
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp357;void*_tmp356;(_tmp356=0,Cyc_Tcutil_terr(loc,((_tmp357="main declared with varargs",_tag_dyneither(_tmp357,sizeof(char),27))),_tag_dyneither(_tmp356,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp10B=fd->args;
if(_tmp10B != 0){
struct _tuple8*_stmttmpD=(struct _tuple8*)_tmp10B->hd;void*_tmp10D;struct _tuple8*_tmp10C=_stmttmpD;_tmp10D=_tmp10C->f3;
{void*_stmttmpE=Cyc_Tcutil_compress(_tmp10D);void*_tmp10E=_stmttmpE;enum Cyc_Absyn_Size_of _tmp110;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10E;if(_tmp10F->tag != 6)goto _LL9F;else{_tmp110=_tmp10F->f2;}}_LL9E:
 goto _LL9C;_LL9F:;_LLA0:
# 414
{const char*_tmp35B;void*_tmp35A[1];struct Cyc_String_pa_PrintArg_struct _tmp359;(_tmp359.tag=0,((_tmp359.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp10D)),((_tmp35A[0]=& _tmp359,Cyc_Tcutil_terr(loc,((_tmp35B="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp35B,sizeof(char),60))),_tag_dyneither(_tmp35A,sizeof(void*),1)))))));}
goto _LL9C;_LL9C:;}
# 418
_tmp10B=_tmp10B->tl;
if(_tmp10B != 0){
struct _tuple8*_stmttmpF=(struct _tuple8*)_tmp10B->hd;void*_tmp115;struct _tuple8*_tmp114=_stmttmpF;_tmp115=_tmp114->f3;
_tmp10B=_tmp10B->tl;
if(_tmp10B != 0){
const char*_tmp35E;void*_tmp35D;(_tmp35D=0,Cyc_Tcutil_terr(loc,((_tmp35E="main declared with too many arguments",_tag_dyneither(_tmp35E,sizeof(char),38))),_tag_dyneither(_tmp35D,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp35F;struct Cyc_Core_Opt*tvs=(_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F->v=fd->tvs,_tmp35F)));
struct Cyc_Core_Opt*_tmp367;struct Cyc_Core_Opt*_tmp366;struct Cyc_Core_Opt*_tmp365;struct Cyc_Core_Opt*_tmp364;struct Cyc_Core_Opt*_tmp363;struct Cyc_Core_Opt*_tmp362;struct Cyc_Core_Opt*_tmp361;struct Cyc_Core_Opt*_tmp360;if(((!Cyc_Tcutil_unify(_tmp115,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp366=_cycalloc(sizeof(*_tmp366)),((_tmp366->v=& Cyc_Tcutil_rk,_tmp366)))),tvs)),
Cyc_Absyn_new_evar(((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367->v=& Cyc_Tcutil_rk,_tmp367)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmp115,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364->v=& Cyc_Tcutil_rk,_tmp364)))),tvs)),
# 430
Cyc_Absyn_new_evar(((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->v=& Cyc_Tcutil_rk,_tmp365)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp115,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362->v=& Cyc_Tcutil_rk,_tmp362)))),tvs)),
Cyc_Absyn_new_evar(((_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363->v=& Cyc_Tcutil_rk,_tmp363)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp115,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360->v=& Cyc_Tcutil_rk,_tmp360)))),tvs)),
# 437
Cyc_Absyn_new_evar(((_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361->v=& Cyc_Tcutil_rk,_tmp361)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp36B;void*_tmp36A[1];struct Cyc_String_pa_PrintArg_struct _tmp369;(_tmp369.tag=0,((_tmp369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp115)),((_tmp36A[0]=& _tmp369,Cyc_Tcutil_terr(loc,((_tmp36B="second argument of main has type %s instead of char??",_tag_dyneither(_tmp36B,sizeof(char),54))),_tag_dyneither(_tmp36A,sizeof(void*),1)))))));}};}}};}
# 376
;_pop_region(fnrgn);};};}
# 447
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 453
{union Cyc_Absyn_Nmspace _stmttmp10=(*td->name).f1;union Cyc_Absyn_Nmspace _tmp124=_stmttmp10;_LLA2: if((_tmp124.Rel_n).tag != 1)goto _LLA4;_LLA3:
# 456
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LLA1;_LLA4:;_LLA5:
 goto _LLA1;_LLA1:;}
# 461
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp36F;void*_tmp36E[1];struct Cyc_String_pa_PrintArg_struct _tmp36D;(_tmp36D.tag=0,((_tmp36D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp36E[0]=& _tmp36D,Cyc_Tcutil_terr(loc,((_tmp36F="redeclaration of typedef %s",_tag_dyneither(_tmp36F,sizeof(char),28))),_tag_dyneither(_tmp36E,sizeof(void*),1)))))));}
return;}
# 466
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 475
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_stmttmp11=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp128=_stmttmp11;struct Cyc_Core_Opt**_tmp12A;struct Cyc_Core_Opt**_tmp12C;struct Cyc_Absyn_Kind*_tmp12D;_LLA7: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp129=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp128;if(_tmp129->tag != 1)goto _LLA9;else{_tmp12A=(struct Cyc_Core_Opt**)& _tmp129->f1;}}_LLA8:
# 478
 if(td->defn != 0){
const char*_tmp373;void*_tmp372[1];struct Cyc_String_pa_PrintArg_struct _tmp371;(_tmp371.tag=0,((_tmp371.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp372[0]=& _tmp371,Cyc_Tcutil_warn(loc,((_tmp373="type variable %s is not used in typedef definition",_tag_dyneither(_tmp373,sizeof(char),51))),_tag_dyneither(_tmp372,sizeof(void*),1)))))));}
# 481
{struct Cyc_Core_Opt*_tmp374;*_tmp12A=((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp374))));}goto _LLA6;_LLA9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp128;if(_tmp12B->tag != 2)goto _LLAB;else{_tmp12C=(struct Cyc_Core_Opt**)& _tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LLAA:
# 488
{struct Cyc_Core_Opt*_tmp375;*_tmp12C=((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375->v=Cyc_Tcutil_kind_to_bound(_tmp12D),_tmp375))));}
goto _LLA6;_LLAB:;_LLAC:
 continue;_LLA6:;}}
# 494
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 497
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 504
struct _RegionHandle _tmp133=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp133;_push_region(uprev_rgn);
# 506
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_stmttmp12=(struct _tuple10*)rpo->hd;void*_tmp135;void*_tmp136;struct _tuple10*_tmp134=_stmttmp12;_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp135);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp136);}{
# 512
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 515
struct Cyc_List_List*_tmp137=fields;for(0;_tmp137 != 0;_tmp137=_tmp137->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp13=(struct Cyc_Absyn_Aggrfield*)_tmp137->hd;struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Tqual _tmp13A;void*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Aggrfield*_tmp138=_stmttmp13;_tmp139=_tmp138->name;_tmp13A=_tmp138->tq;_tmp13B=_tmp138->type;_tmp13C=_tmp138->width;_tmp13D=_tmp138->attributes;_tmp13E=_tmp138->requires_clause;
# 518
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp139)){
const char*_tmp379;void*_tmp378[1];struct Cyc_String_pa_PrintArg_struct _tmp377;(_tmp377.tag=0,((_tmp377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp139),((_tmp378[0]=& _tmp377,Cyc_Tcutil_terr(loc,((_tmp379="duplicate member %s",_tag_dyneither(_tmp379,sizeof(char),20))),_tag_dyneither(_tmp378,sizeof(void*),1)))))));}
# 522
{const char*_tmp37A;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp139,((_tmp37A="",_tag_dyneither(_tmp37A,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp37B;prev_fields=((_tmp37B=_region_malloc(uprev_rgn,sizeof(*_tmp37B)),((_tmp37B->hd=_tmp139,((_tmp37B->tl=prev_fields,_tmp37B))))));}}{
# 525
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 529
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp137->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp13B);
# 534
(((struct Cyc_Absyn_Aggrfield*)_tmp137->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp137->hd)->tq).print_const,_tmp13B);
# 537
Cyc_Tcutil_check_bitfield(loc,te,_tmp13B,_tmp13C,_tmp139);
# 539
if((unsigned int)_tmp13E){
if(str_or_union != Cyc_Absyn_UnionA){
const char*_tmp37E;void*_tmp37D;(_tmp37D=0,Cyc_Tcutil_terr(loc,((_tmp37E="@requires clauses are only allowed on union members",_tag_dyneither(_tmp37E,sizeof(char),52))),_tag_dyneither(_tmp37D,sizeof(void*),0)));}
{struct _RegionHandle _tmp146=_new_region("temp");struct _RegionHandle*temp=& _tmp146;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp147=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp147,0,_tmp13E);}
# 543
;_pop_region(temp);}
# 546
if(!Cyc_Tcutil_is_integral(_tmp13E)){
const char*_tmp382;void*_tmp381[1];struct Cyc_String_pa_PrintArg_struct _tmp380;(_tmp380.tag=0,((_tmp380.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 549
Cyc_Absynpp_typ2string((void*)_check_null(_tmp13E->topt))),((_tmp381[0]=& _tmp380,Cyc_Tcutil_terr(_tmp13E->loc,((_tmp382="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp382,sizeof(char),54))),_tag_dyneither(_tmp381,sizeof(void*),1)))))));}else{
# 551
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp385;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp384;Cyc_Tcutil_check_type(_tmp13E->loc,te,tvs,& Cyc_Tcutil_ik,0,(void*)((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384[0]=((_tmp385.tag=18,((_tmp385.f1=_tmp13E,_tmp385)))),_tmp384)))));}{
# 553
struct Cyc_List_List*_tmp14D=Cyc_Relations_exp2relns(uprev_rgn,_tmp13E);
# 560
if(!Cyc_Relations_consistent_relations(_tmp14D)){
const char*_tmp388;void*_tmp387;(_tmp387=0,Cyc_Tcutil_terr(_tmp13E->loc,((_tmp388="@requires clause may be unsatisfiable",_tag_dyneither(_tmp388,sizeof(char),38))),_tag_dyneither(_tmp387,sizeof(void*),0)));}
# 566
{struct Cyc_List_List*_tmp150=prev_relations;for(0;_tmp150 != 0;_tmp150=_tmp150->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp14D,(struct Cyc_List_List*)_tmp150->hd))){
const char*_tmp38B;void*_tmp38A;(_tmp38A=0,Cyc_Tcutil_terr(_tmp13E->loc,((_tmp38B="@requires clause may overlap with previous clauses",_tag_dyneither(_tmp38B,sizeof(char),51))),_tag_dyneither(_tmp38A,sizeof(void*),0)));}}}{
# 571
struct Cyc_List_List*_tmp38C;prev_relations=((_tmp38C=_region_malloc(uprev_rgn,sizeof(*_tmp38C)),((_tmp38C->hd=_tmp14D,((_tmp38C->tl=prev_relations,_tmp38C))))));};};}}else{
# 574
if(prev_relations != 0){
const char*_tmp38F;void*_tmp38E;(_tmp38E=0,Cyc_Tcutil_terr(loc,((_tmp38F="if one field has a @requires clause, they all must",_tag_dyneither(_tmp38F,sizeof(char),51))),_tag_dyneither(_tmp38E,sizeof(void*),0)));}}};}};
# 506
;_pop_region(uprev_rgn);}
# 581
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 583
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp14=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp156=_stmttmp14;struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt**_tmp15A;enum Cyc_Absyn_AliasQual _tmp15B;struct Cyc_Core_Opt**_tmp15D;struct Cyc_Core_Opt**_tmp15F;struct Cyc_Absyn_Kind*_tmp160;enum Cyc_Absyn_AliasQual _tmp162;_LLAE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp157=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp157->tag != 1)goto _LLB0;else{_tmp158=(struct Cyc_Core_Opt**)& _tmp157->f1;}}_LLAF:
# 586
{struct Cyc_Core_Opt*_tmp390;*_tmp158=((_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp390))));}continue;_LLB0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp159=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp159->tag != 2)goto _LLB2;else{_tmp15A=(struct Cyc_Core_Opt**)& _tmp159->f1;if((_tmp159->f2)->kind != Cyc_Absyn_MemKind)goto _LLB2;_tmp15B=(_tmp159->f2)->aliasqual;}}_LLB1:
# 588
 if(constrain_top_kind  && _tmp15B == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp393;struct Cyc_Core_Opt*_tmp392;*_tmp15A=((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392->v=Cyc_Tcutil_kind_to_bound(((_tmp393=_cycalloc_atomic(sizeof(*_tmp393)),((_tmp393->kind=Cyc_Absyn_BoxKind,((_tmp393->aliasqual=Cyc_Absyn_Aliasable,_tmp393))))))),_tmp392))));}else{
# 591
struct Cyc_Absyn_Kind*_tmp396;struct Cyc_Core_Opt*_tmp395;*_tmp15A=((_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395->v=Cyc_Tcutil_kind_to_bound(((_tmp396=_cycalloc_atomic(sizeof(*_tmp396)),((_tmp396->kind=Cyc_Absyn_BoxKind,((_tmp396->aliasqual=_tmp15B,_tmp396))))))),_tmp395))));}
continue;_LLB2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp15C->tag != 2)goto _LLB4;else{_tmp15D=(struct Cyc_Core_Opt**)& _tmp15C->f1;if((_tmp15C->f2)->kind != Cyc_Absyn_BoxKind)goto _LLB4;if((_tmp15C->f2)->aliasqual != Cyc_Absyn_Top)goto _LLB4;}}if(!constrain_top_kind)goto _LLB4;_LLB3:
# 594
{struct Cyc_Absyn_Kind*_tmp399;struct Cyc_Core_Opt*_tmp398;*_tmp15D=((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398->v=Cyc_Tcutil_kind_to_bound(((_tmp399=_cycalloc_atomic(sizeof(*_tmp399)),((_tmp399->kind=Cyc_Absyn_BoxKind,((_tmp399->aliasqual=Cyc_Absyn_Aliasable,_tmp399))))))),_tmp398))));}
continue;_LLB4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp15E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp15E->tag != 2)goto _LLB6;else{_tmp15F=(struct Cyc_Core_Opt**)& _tmp15E->f1;_tmp160=_tmp15E->f2;}}_LLB5:
# 597
{struct Cyc_Core_Opt*_tmp39A;*_tmp15F=((_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A->v=Cyc_Tcutil_kind_to_bound(_tmp160),_tmp39A))));}continue;_LLB6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp161=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp156;if(_tmp161->tag != 0)goto _LLB8;else{if((_tmp161->f1)->kind != Cyc_Absyn_MemKind)goto _LLB8;_tmp162=(_tmp161->f1)->aliasqual;}}_LLB7:
# 599
{const char*_tmp3A2;void*_tmp3A1[3];struct Cyc_String_pa_PrintArg_struct _tmp3A0;struct Cyc_String_pa_PrintArg_struct _tmp39F;struct Cyc_Absyn_Kind*_tmp39E;struct Cyc_String_pa_PrintArg_struct _tmp39D;(_tmp39D.tag=0,((_tmp39D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp39E=_cycalloc_atomic(sizeof(*_tmp39E)),((_tmp39E->kind=Cyc_Absyn_MemKind,((_tmp39E->aliasqual=_tmp162,_tmp39E)))))))),((_tmp39F.tag=0,((_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3A1[0]=& _tmp3A0,((_tmp3A1[1]=& _tmp39F,((_tmp3A1[2]=& _tmp39D,Cyc_Tcutil_terr(loc,((_tmp3A2="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3A2,sizeof(char),57))),_tag_dyneither(_tmp3A1,sizeof(void*),3)))))))))))))))))));}
continue;_LLB8:;_LLB9:
 continue;_LLAD:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 607
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp171=(*ad->name).f2;
# 614
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmp15=(void*)atts->hd;void*_tmp172=_stmttmp15;_LLBB: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp173=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp172;if(_tmp173->tag != 7)goto _LLBD;}_LLBC:
 goto _LLBE;_LLBD: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp174=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp172;if(_tmp174->tag != 6)goto _LLBF;}_LLBE:
 continue;_LLBF:;_LLC0:
# 619
{const char*_tmp3A7;void*_tmp3A6[2];struct Cyc_String_pa_PrintArg_struct _tmp3A5;struct Cyc_String_pa_PrintArg_struct _tmp3A4;(_tmp3A4.tag=0,((_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp171),((_tmp3A5.tag=0,((_tmp3A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp3A6[0]=& _tmp3A5,((_tmp3A6[1]=& _tmp3A4,Cyc_Tcutil_terr(loc,((_tmp3A7="bad attribute %s in %s definition",_tag_dyneither(_tmp3A7,sizeof(char),34))),_tag_dyneither(_tmp3A6,sizeof(void*),2)))))))))))));}
goto _LLBA;_LLBA:;}}{
# 624
struct Cyc_List_List*_tmp179=ad->tvs;
# 626
{union Cyc_Absyn_Nmspace _stmttmp16=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp17A=_stmttmp16;_LLC2: if((_tmp17A.Rel_n).tag != 1)goto _LLC4;if((struct Cyc_List_List*)(_tmp17A.Rel_n).val != 0)goto _LLC4;_LLC3:
# 629
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLC1;_LLC4:;_LLC5:
# 632
 goto _LLC1;_LLC1:;}
# 636
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 638
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 642
{struct _tuple14 _tmp3A8;struct _tuple14 _stmttmp17=(_tmp3A8.f1=ad->impl,((_tmp3A8.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp171),_tmp3A8)));struct _tuple14 _tmp17B=_stmttmp17;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17E;int _tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;int _tmp183;struct Cyc_Absyn_Aggrdecl**_tmp184;struct Cyc_Absyn_Aggrdecl**_tmp185;_LLC7: if(_tmp17B.f1 != 0)goto _LLC9;if(_tmp17B.f2 != 0)goto _LLC9;_LLC8:
# 645
 Cyc_Tc_rule_out_memkind(loc,_tmp171,_tmp179,0);
# 647
{struct Cyc_Absyn_Aggrdecl**_tmp3A9;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp171,((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=ad,_tmp3A9)))));}
goto _LLC6;_LLC9: if(_tmp17B.f1 == 0)goto _LLCB;_tmp17C=(_tmp17B.f1)->exist_vars;_tmp17D=(_tmp17B.f1)->rgn_po;_tmp17E=(_tmp17B.f1)->fields;_tmp17F=(_tmp17B.f1)->tagged;if(_tmp17B.f2 != 0)goto _LLCB;_LLCA: {
# 652
struct Cyc_Absyn_Aggrdecl*_tmp3AC;struct Cyc_Absyn_Aggrdecl**_tmp3AB;struct Cyc_Absyn_Aggrdecl**_tmp187=(_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB[0]=((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->kind=ad->kind,((_tmp3AC->sc=Cyc_Absyn_Extern,((_tmp3AC->name=ad->name,((_tmp3AC->tvs=_tmp179,((_tmp3AC->impl=0,((_tmp3AC->attributes=ad->attributes,_tmp3AC)))))))))))))),_tmp3AB)));
# 654
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp171,_tmp187);
# 659
Cyc_Tcutil_check_unique_tvars(loc,_tmp17C);
# 661
Cyc_Tcutil_add_tvar_identities(_tmp17C);
# 664
if(_tmp17F  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3AF;void*_tmp3AE;(_tmp3AE=0,Cyc_Tcutil_terr(loc,((_tmp3AF="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3AF,sizeof(char),56))),_tag_dyneither(_tmp3AE,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp179,_tmp17C),_tmp17D,_tmp17E);
# 668
Cyc_Tc_rule_out_memkind(loc,_tmp171,_tmp179,0);
# 671
Cyc_Tc_rule_out_memkind(loc,_tmp171,_tmp17C,1);
# 673
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp17F){
# 676
struct Cyc_List_List*f=_tmp17E;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0){
const char*_tmp3B5;void*_tmp3B4[3];struct Cyc_String_pa_PrintArg_struct _tmp3B3;struct Cyc_String_pa_PrintArg_struct _tmp3B2;struct Cyc_String_pa_PrintArg_struct _tmp3B1;(_tmp3B1.tag=0,((_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp3B2.tag=0,((_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp171),((_tmp3B3.tag=0,((_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp3B4[0]=& _tmp3B3,((_tmp3B4[1]=& _tmp3B2,((_tmp3B4[2]=& _tmp3B1,Cyc_Tcutil_warn(loc,((_tmp3B5="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp3B5,sizeof(char),98))),_tag_dyneither(_tmp3B4,sizeof(void*),3)))))))))))))))))));}}}
# 681
*_tmp187=ad;
goto _LLC6;}_LLCB: if(_tmp17B.f1 == 0)goto _LLCD;_tmp180=(_tmp17B.f1)->exist_vars;_tmp181=(_tmp17B.f1)->rgn_po;_tmp182=(_tmp17B.f1)->fields;_tmp183=(_tmp17B.f1)->tagged;if(_tmp17B.f2 == 0)goto _LLCD;_tmp184=*_tmp17B.f2;_LLCC:
# 685
 if(ad->kind != (*_tmp184)->kind){
const char*_tmp3B8;void*_tmp3B7;(_tmp3B7=0,Cyc_Tcutil_terr(loc,((_tmp3B8="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp3B8,sizeof(char),52))),_tag_dyneither(_tmp3B7,sizeof(void*),0)));}{
# 688
struct Cyc_Absyn_Aggrdecl*_tmp193=*_tmp184;
# 690
{struct Cyc_Absyn_Aggrdecl*_tmp3B9;*_tmp184=((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9->kind=ad->kind,((_tmp3B9->sc=Cyc_Absyn_Extern,((_tmp3B9->name=ad->name,((_tmp3B9->tvs=_tmp179,((_tmp3B9->impl=0,((_tmp3B9->attributes=ad->attributes,_tmp3B9))))))))))))));}
# 696
Cyc_Tcutil_check_unique_tvars(loc,_tmp180);
# 698
Cyc_Tcutil_add_tvar_identities(_tmp180);
# 700
if(_tmp183  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3BC;void*_tmp3BB;(_tmp3BB=0,Cyc_Tcutil_terr(loc,((_tmp3BC="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3BC,sizeof(char),56))),_tag_dyneither(_tmp3BB,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp179,_tmp180),_tmp181,_tmp182);
# 704
*_tmp184=_tmp193;
_tmp185=_tmp184;goto _LLCE;};_LLCD: if(_tmp17B.f1 != 0)goto _LLC6;if(_tmp17B.f2 == 0)goto _LLC6;_tmp185=*_tmp17B.f2;_LLCE: {
# 708
struct Cyc_Absyn_Aggrdecl*_tmp197=Cyc_Tcdecl_merge_aggrdecl(*_tmp185,ad,loc,Cyc_Tc_tc_msg);
if(_tmp197 == 0)
return;else{
# 712
Cyc_Tc_rule_out_memkind(loc,_tmp171,_tmp179,0);
# 715
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp171,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 718
*_tmp185=_tmp197;
ad=_tmp197;
goto _LLC6;}}_LLC6:;}{
# 724
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3C2;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp3C1;struct _tuple13*_tmp3C0;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp171,(
(_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0->f1=(void*)((_tmp3C2=_cycalloc(sizeof(*_tmp3C2)),((_tmp3C2[0]=((_tmp3C1.tag=1,((_tmp3C1.f1=ad,_tmp3C1)))),_tmp3C2)))),((_tmp3C0->f2=1,_tmp3C0)))))));};};}
# 728
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp18=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp19C=_stmttmp18;struct Cyc_Core_Opt**_tmp19E;struct Cyc_Core_Opt**_tmp1A0;struct Cyc_Core_Opt**_tmp1A2;struct Cyc_Core_Opt**_tmp1A4;struct Cyc_Core_Opt**_tmp1A6;struct Cyc_Core_Opt**_tmp1A8;struct Cyc_Core_Opt**_tmp1AB;enum Cyc_Absyn_KindQual _tmp1AC;enum Cyc_Absyn_KindQual _tmp1AE;enum Cyc_Absyn_AliasQual _tmp1B0;_LLD0: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp19D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp19D->tag != 1)goto _LLD2;else{_tmp19E=(struct Cyc_Core_Opt**)& _tmp19D->f1;}}_LLD1:
 _tmp1A0=_tmp19E;goto _LLD3;_LLD2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp19F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp19F->tag != 2)goto _LLD4;else{_tmp1A0=(struct Cyc_Core_Opt**)& _tmp19F->f1;if((_tmp19F->f2)->kind != Cyc_Absyn_MemKind)goto _LLD4;if((_tmp19F->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD4;}}_LLD3:
# 733
 _tmp1A2=_tmp1A0;goto _LLD5;_LLD4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1A1->tag != 2)goto _LLD6;else{_tmp1A2=(struct Cyc_Core_Opt**)& _tmp1A1->f1;if((_tmp1A1->f2)->kind != Cyc_Absyn_MemKind)goto _LLD6;if((_tmp1A1->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLD6;}}_LLD5:
# 735
{struct Cyc_Core_Opt*_tmp3C3;*_tmp1A2=((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3C3))));}goto _LLCF;_LLD6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1A3->tag != 2)goto _LLD8;else{_tmp1A4=(struct Cyc_Core_Opt**)& _tmp1A3->f1;if((_tmp1A3->f2)->kind != Cyc_Absyn_AnyKind)goto _LLD8;if((_tmp1A3->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD8;}}_LLD7:
 _tmp1A6=_tmp1A4;goto _LLD9;_LLD8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1A5->tag != 2)goto _LLDA;else{_tmp1A6=(struct Cyc_Core_Opt**)& _tmp1A5->f1;if((_tmp1A5->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDA;if((_tmp1A5->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDA;}}_LLD9:
# 738
{struct Cyc_Core_Opt*_tmp3C4;*_tmp1A6=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp3C4))));}goto _LLCF;_LLDA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1A7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1A7->tag != 2)goto _LLDC;else{_tmp1A8=(struct Cyc_Core_Opt**)& _tmp1A7->f1;if((_tmp1A7->f2)->kind != Cyc_Absyn_RgnKind)goto _LLDC;if((_tmp1A7->f2)->aliasqual != Cyc_Absyn_Top)goto _LLDC;}}_LLDB:
# 740
{struct Cyc_Core_Opt*_tmp3C5;*_tmp1A8=((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp3C5))));}goto _LLCF;_LLDC: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1A9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1A9->tag != 0)goto _LLDE;else{if((_tmp1A9->f1)->kind != Cyc_Absyn_RgnKind)goto _LLDE;if((_tmp1A9->f1)->aliasqual != Cyc_Absyn_Top)goto _LLDE;}}_LLDD:
# 742
{const char*_tmp3CA;void*_tmp3C9[2];struct Cyc_String_pa_PrintArg_struct _tmp3C8;struct Cyc_String_pa_PrintArg_struct _tmp3C7;(_tmp3C7.tag=0,((_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3C8.tag=0,((_tmp3C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3C9[0]=& _tmp3C8,((_tmp3C9[1]=& _tmp3C7,Cyc_Tcutil_terr(loc,((_tmp3CA="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp3CA,sizeof(char),57))),_tag_dyneither(_tmp3C9,sizeof(void*),2)))))))))))));}
goto _LLCF;_LLDE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1AA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1AA->tag != 2)goto _LLE0;else{_tmp1AB=(struct Cyc_Core_Opt**)& _tmp1AA->f1;_tmp1AC=(_tmp1AA->f2)->kind;if((_tmp1AA->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLE0;}}_LLDF:
 _tmp1AE=_tmp1AC;goto _LLE1;_LLE0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1AD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1AD->tag != 0)goto _LLE2;else{_tmp1AE=(_tmp1AD->f1)->kind;if((_tmp1AD->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLE2;}}_LLE1:
# 746
{const char*_tmp3D2;void*_tmp3D1[3];struct Cyc_String_pa_PrintArg_struct _tmp3D0;struct Cyc_String_pa_PrintArg_struct _tmp3CF;struct Cyc_Absyn_Kind*_tmp3CE;struct Cyc_String_pa_PrintArg_struct _tmp3CD;(_tmp3CD.tag=0,((_tmp3CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 748
Cyc_Absynpp_kind2string(((_tmp3CE=_cycalloc_atomic(sizeof(*_tmp3CE)),((_tmp3CE->kind=_tmp1AE,((_tmp3CE->aliasqual=Cyc_Absyn_Unique,_tmp3CE)))))))),((_tmp3CF.tag=0,((_tmp3CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3D0.tag=0,((_tmp3D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D1[0]=& _tmp3D0,((_tmp3D1[1]=& _tmp3CF,((_tmp3D1[2]=& _tmp3CD,Cyc_Tcutil_terr(loc,((_tmp3D2="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3D2,sizeof(char),57))),_tag_dyneither(_tmp3D1,sizeof(void*),3)))))))))))))))))));}goto _LLCF;_LLE2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1AF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp19C;if(_tmp1AF->tag != 0)goto _LLE4;else{if((_tmp1AF->f1)->kind != Cyc_Absyn_MemKind)goto _LLE4;_tmp1B0=(_tmp1AF->f1)->aliasqual;}}_LLE3:
# 750
{const char*_tmp3DA;void*_tmp3D9[3];struct Cyc_String_pa_PrintArg_struct _tmp3D8;struct Cyc_String_pa_PrintArg_struct _tmp3D7;struct Cyc_Absyn_Kind*_tmp3D6;struct Cyc_String_pa_PrintArg_struct _tmp3D5;(_tmp3D5.tag=0,((_tmp3D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 752
Cyc_Absynpp_kind2string(((_tmp3D6=_cycalloc_atomic(sizeof(*_tmp3D6)),((_tmp3D6->kind=Cyc_Absyn_MemKind,((_tmp3D6->aliasqual=_tmp1B0,_tmp3D6)))))))),((_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3D8.tag=0,((_tmp3D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D9[0]=& _tmp3D8,((_tmp3D9[1]=& _tmp3D7,((_tmp3D9[2]=& _tmp3D5,Cyc_Tcutil_terr(loc,((_tmp3DA="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3DA,sizeof(char),57))),_tag_dyneither(_tmp3D9,sizeof(void*),3)))))))))))))))))));}goto _LLCF;_LLE4:;_LLE5:
 goto _LLCF;_LLCF:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 759
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 768
{struct Cyc_List_List*_tmp1C4=fields;for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){
struct Cyc_Absyn_Datatypefield*_tmp1C5=(struct Cyc_Absyn_Datatypefield*)_tmp1C4->hd;
# 771
{struct Cyc_List_List*typs=_tmp1C5->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp1C5->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 774
struct _RegionHandle _tmp1C6=_new_region("temp");struct _RegionHandle*temp=& _tmp1C6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp3DE;void*_tmp3DD[1];struct Cyc_String_pa_PrintArg_struct _tmp3DC;(_tmp3DC.tag=0,((_tmp3DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 778
Cyc_Absynpp_qvar2string(_tmp1C5->name)),((_tmp3DD[0]=& _tmp3DC,Cyc_Tcutil_terr(_tmp1C5->loc,((_tmp3DE="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3DE,sizeof(char),51))),_tag_dyneither(_tmp3DD,sizeof(void*),1)))))));}
# 780
((*((struct _tuple15*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp1C5->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 774
;_pop_region(temp);};}}{
# 785
union Cyc_Absyn_Nmspace _stmttmp19=(*_tmp1C5->name).f1;union Cyc_Absyn_Nmspace _tmp1CA=_stmttmp19;_LLE7: if((_tmp1CA.Rel_n).tag != 1)goto _LLE9;if((struct Cyc_List_List*)(_tmp1CA.Rel_n).val != 0)goto _LLE9;_LLE8:
# 787
 if(is_extensible)
(*_tmp1C5->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 790
(*_tmp1C5->name).f1=(*name).f1;}
goto _LLE6;_LLE9: if((_tmp1CA.Rel_n).tag != 1)goto _LLEB;_LLEA:
# 793
{const char*_tmp3E2;void*_tmp3E1[1];struct Cyc_String_pa_PrintArg_struct _tmp3E0;(_tmp3E0.tag=0,((_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1C5->name)),((_tmp3E1[0]=& _tmp3E0,Cyc_Tcutil_terr(_tmp1C5->loc,((_tmp3E2="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3E2,sizeof(char),58))),_tag_dyneither(_tmp3E1,sizeof(void*),1)))))));}
goto _LLE6;_LLEB: if((_tmp1CA.C_n).tag != 3)goto _LLED;_LLEC:
 goto _LLE6;_LLED: if((_tmp1CA.Abs_n).tag != 2)goto _LLEF;_LLEE:
 goto _LLE6;_LLEF: if((_tmp1CA.Loc_n).tag != 4)goto _LLE6;_LLF0: {
const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3E5="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3E5,sizeof(char),24))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}_LLE6:;};}}{
# 802
struct Cyc_List_List*fields2;
if(is_extensible){
# 805
int _tmp1D0=1;
struct Cyc_List_List*_tmp1D1=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1D0,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1D0)
fields2=_tmp1D1;else{
# 810
fields2=0;}}else{
# 812
struct _RegionHandle _tmp1D2=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1D2;_push_region(uprev_rgn);
# 814
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1D3=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1D3->name).f2)){
const char*_tmp3EA;void*_tmp3E9[2];struct Cyc_String_pa_PrintArg_struct _tmp3E8;struct Cyc_String_pa_PrintArg_struct _tmp3E7;(_tmp3E7.tag=0,((_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3E8.tag=0,((_tmp3E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D3->name).f2),((_tmp3E9[0]=& _tmp3E8,((_tmp3E9[1]=& _tmp3E7,Cyc_Tcutil_terr(_tmp1D3->loc,((_tmp3EA="duplicate field name %s in %s",_tag_dyneither(_tmp3EA,sizeof(char),30))),_tag_dyneither(_tmp3E9,sizeof(void*),2)))))))))))));}else{
# 820
struct Cyc_List_List*_tmp3EB;prev_fields=((_tmp3EB=_region_malloc(uprev_rgn,sizeof(*_tmp3EB)),((_tmp3EB->hd=(*_tmp1D3->name).f2,((_tmp3EB->tl=prev_fields,_tmp3EB))))));}
# 822
if(_tmp1D3->sc != Cyc_Absyn_Public){
{const char*_tmp3EF;void*_tmp3EE[1];struct Cyc_String_pa_PrintArg_struct _tmp3ED;(_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1D3->name).f2),((_tmp3EE[0]=& _tmp3ED,Cyc_Tcutil_warn(loc,((_tmp3EF="ignoring scope of field %s",_tag_dyneither(_tmp3EF,sizeof(char),27))),_tag_dyneither(_tmp3EE,sizeof(void*),1)))))));}
_tmp1D3->sc=Cyc_Absyn_Public;}}}
# 827
fields2=fields;}
# 814
;_pop_region(uprev_rgn);}
# 833
{struct Cyc_List_List*_tmp1DC=fields;for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
struct Cyc_Absyn_Datatypefield*_tmp1DD=(struct Cyc_Absyn_Datatypefield*)_tmp1DC->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3F5;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3F4;struct _tuple13*_tmp3F3;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1DD->name).f2,(
(_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3->f1=(void*)((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F4.tag=2,((_tmp3F4.f1=tudres,((_tmp3F4.f2=_tmp1DD,_tmp3F4)))))),_tmp3F5)))),((_tmp3F3->f2=1,_tmp3F3)))))));}}
# 839
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 842
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp3F7;const char*_tmp3F6;struct _dyneither_ptr obj=tud->is_extensible?(_tmp3F7="@extensible datatype",_tag_dyneither(_tmp3F7,sizeof(char),21)):((_tmp3F6="datatype",_tag_dyneither(_tmp3F6,sizeof(char),9)));
# 849
struct Cyc_List_List*tvs=tud->tvs;
# 852
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 854
Cyc_Tcutil_add_tvar_identities(tvs);{
# 859
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 869 "tc.cyc"
{struct _handler_cons _tmp1E2;_push_handler(& _tmp1E2);{int _tmp1E4=0;if(setjmp(_tmp1E2.handler))_tmp1E4=1;if(!_tmp1E4){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp3FA;struct Cyc_Dict_Absent_exn_struct*_tmp3F9;(int)_throw((void*)((_tmp3F9=_cycalloc_atomic(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3FA.tag=Cyc_Dict_Absent,_tmp3FA)),_tmp3F9)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 875
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 870
;_pop_handler();}else{void*_tmp1E3=(void*)_exn_thrown;void*_tmp1E8=_tmp1E3;void*_tmp1EA;_LLF2: {struct Cyc_Dict_Absent_exn_struct*_tmp1E9=(struct Cyc_Dict_Absent_exn_struct*)_tmp1E8;if(_tmp1E9->tag != Cyc_Dict_Absent)goto _LLF4;}_LLF3:
# 879
{union Cyc_Absyn_Nmspace _stmttmp1A=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp1EB=_stmttmp1A;_LLF7: if((_tmp1EB.Rel_n).tag != 1)goto _LLF9;if((struct Cyc_List_List*)(_tmp1EB.Rel_n).val != 0)goto _LLF9;_LLF8:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLF6;_LLF9:;_LLFA:
 goto _LLF6;_LLF6:;}{
# 883
struct Cyc_Absyn_Datatypedecl***_tmp1EC=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp3FB;tud_opt=(unsigned int)_tmp1EC?(_tmp3FB=_region_malloc(temp,sizeof(*_tmp3FB)),((_tmp3FB[0]=*_tmp1EC,_tmp3FB))): 0;}
goto _LLF1;};_LLF4: _tmp1EA=_tmp1E8;_LLF5:(void)_throw(_tmp1EA);_LLF1:;}};}{
# 890
struct _tuple16 _tmp3FC;struct _tuple16 _stmttmp1B=(_tmp3FC.f1=tud->fields,((_tmp3FC.f2=tud_opt,_tmp3FC)));struct _tuple16 _tmp1EE=_stmttmp1B;struct Cyc_List_List**_tmp1EF;struct Cyc_List_List**_tmp1F0;struct Cyc_Absyn_Datatypedecl**_tmp1F1;struct Cyc_Absyn_Datatypedecl**_tmp1F2;_LLFC: if(_tmp1EE.f1 != 0)goto _LLFE;if(_tmp1EE.f2 != 0)goto _LLFE;_LLFD:
# 893
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp3FD;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=tud,_tmp3FD)))));}
goto _LLFB;_LLFE: if(_tmp1EE.f1 == 0)goto _LL100;_tmp1EF=(struct Cyc_List_List**)&(_tmp1EE.f1)->v;if(_tmp1EE.f2 != 0)goto _LL100;_LLFF: {
# 898
struct Cyc_Absyn_Datatypedecl*_tmp400;struct Cyc_Absyn_Datatypedecl**_tmp3FF;struct Cyc_Absyn_Datatypedecl**_tmp1F4=(_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF[0]=((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400->sc=Cyc_Absyn_Extern,((_tmp400->name=tud->name,((_tmp400->tvs=tvs,((_tmp400->fields=0,((_tmp400->is_extensible=tud->is_extensible,_tmp400)))))))))))),_tmp3FF)));
# 900
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1F4);
# 903
*_tmp1EF=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1EF,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1F4=tud;
goto _LLFB;}_LL100: if(_tmp1EE.f1 == 0)goto _LL102;_tmp1F0=(struct Cyc_List_List**)&(_tmp1EE.f1)->v;if(_tmp1EE.f2 == 0)goto _LL102;_tmp1F1=*_tmp1EE.f2;_LL101: {
# 908
struct Cyc_Absyn_Datatypedecl*_tmp1F7=*_tmp1F1;
# 911
if((!tud->is_extensible  && (unsigned int)_tmp1F7) && _tmp1F7->is_extensible)
tud->is_extensible=1;
# 914
{struct Cyc_Absyn_Datatypedecl*_tmp401;*_tmp1F1=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->sc=Cyc_Absyn_Extern,((_tmp401->name=tud->name,((_tmp401->tvs=tvs,((_tmp401->fields=0,((_tmp401->is_extensible=tud->is_extensible,_tmp401))))))))))));}
# 918
*_tmp1F0=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1F0,tvs,tud);
# 922
*_tmp1F1=_tmp1F7;
_tmp1F2=_tmp1F1;goto _LL103;}_LL102: if(_tmp1EE.f1 != 0)goto _LLFB;if(_tmp1EE.f2 == 0)goto _LLFB;_tmp1F2=*_tmp1EE.f2;_LL103: {
# 926
struct Cyc_Absyn_Datatypedecl*_tmp1F9=Cyc_Tcdecl_merge_datatypedecl(*_tmp1F2,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1F9 == 0){
_npop_handler(0);return;}else{
# 931
*_tmp1F2=_tmp1F9;
goto _LLFB;}}_LLFB:;};}
# 860 "tc.cyc"
;_pop_region(temp);};}
# 938 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 945
{union Cyc_Absyn_Nmspace _stmttmp1C=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp1FD=_stmttmp1C;_LL105: if((_tmp1FD.Rel_n).tag != 1)goto _LL107;if((struct Cyc_List_List*)(_tmp1FD.Rel_n).val != 0)goto _LL107;_LL106:
# 948
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LL104;_LL107:;_LL108:
 goto _LL104;_LL104:;}
# 954
if(ed->fields != 0){struct _RegionHandle _tmp1FE=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1FE;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1FF=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 960
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1FF->name).f2)){
const char*_tmp405;void*_tmp404[1];struct Cyc_String_pa_PrintArg_struct _tmp403;(_tmp403.tag=0,((_tmp403.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1FF->name).f2),((_tmp404[0]=& _tmp403,Cyc_Tcutil_terr(_tmp1FF->loc,((_tmp405="duplicate enum constructor %s",_tag_dyneither(_tmp405,sizeof(char),30))),_tag_dyneither(_tmp404,sizeof(void*),1)))))));}else{
# 963
struct Cyc_List_List*_tmp406;prev_fields=((_tmp406=_region_malloc(uprev_rgn,sizeof(*_tmp406)),((_tmp406->hd=(*_tmp1FF->name).f2,((_tmp406->tl=prev_fields,_tmp406))))));}
# 965
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1FF->name).f2)!= 0){
const char*_tmp40A;void*_tmp409[1];struct Cyc_String_pa_PrintArg_struct _tmp408;(_tmp408.tag=0,((_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1FF->name).f2),((_tmp409[0]=& _tmp408,Cyc_Tcutil_terr(_tmp1FF->loc,((_tmp40A="enum field name %s shadows global name",_tag_dyneither(_tmp40A,sizeof(char),39))),_tag_dyneither(_tmp409,sizeof(void*),1)))))));}
# 968
if(_tmp1FF->tag == 0){
_tmp1FF->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1FF->loc);
++ tag_count;}else{
# 973
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1FF->tag))){
struct _tuple11 _stmttmp1D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1FF->tag));unsigned int _tmp208;int _tmp209;struct _tuple11 _tmp207=_stmttmp1D;_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;
if(_tmp209)tag_count=_tmp208 + 1;}}
# 977
(*_tmp1FF->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 955
;_pop_region(uprev_rgn);}
# 982
{struct _handler_cons _tmp20A;_push_handler(& _tmp20A);{int _tmp20C=0;if(setjmp(_tmp20A.handler))_tmp20C=1;if(!_tmp20C){
{struct Cyc_Absyn_Enumdecl**_tmp20D=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp20E=Cyc_Tcdecl_merge_enumdecl(*_tmp20D,ed,loc,Cyc_Tc_tc_msg);
if(_tmp20E == 0){_npop_handler(0);return;}
*_tmp20D=_tmp20E;}
# 983
;_pop_handler();}else{void*_tmp20B=(void*)_exn_thrown;void*_tmp210=_tmp20B;void*_tmp212;_LL10A: {struct Cyc_Dict_Absent_exn_struct*_tmp211=(struct Cyc_Dict_Absent_exn_struct*)_tmp210;if(_tmp211->tag != Cyc_Dict_Absent)goto _LL10C;}_LL10B: {
# 988
struct Cyc_Absyn_Enumdecl**_tmp40B;struct Cyc_Absyn_Enumdecl**_tmp213=(_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=ed,_tmp40B)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp213);
goto _LL109;}_LL10C: _tmp212=_tmp210;_LL10D:(void)_throw(_tmp212);_LL109:;}};}
# 992
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp215=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp411;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp410;struct _tuple13*_tmp40F;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp215->name).f2,(
(_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F->f1=(void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp410.tag=3,((_tmp410.f1=ed,((_tmp410.f2=_tmp215,_tmp410)))))),_tmp411)))),((_tmp40F->f2=1,_tmp40F)))))));}}
# 1001
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp219=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp219->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp219->tag))){
const char*_tmp416;void*_tmp415[2];struct Cyc_String_pa_PrintArg_struct _tmp414;struct Cyc_String_pa_PrintArg_struct _tmp413;(_tmp413.tag=0,((_tmp413.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp219->name).f2),((_tmp414.tag=0,((_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp415[0]=& _tmp414,((_tmp415[1]=& _tmp413,Cyc_Tcutil_terr(loc,((_tmp416="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp416,sizeof(char),46))),_tag_dyneither(_tmp415,sizeof(void*),2)))))))))))));}}}}
# 1011
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL10E:
# 1014
 if(!in_include){
const char*_tmp419;void*_tmp418;(_tmp418=0,Cyc_Tcutil_warn(loc,((_tmp419="static declaration nested within extern \"C\"",_tag_dyneither(_tmp419,sizeof(char),44))),_tag_dyneither(_tmp418,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL10F:
# 1018
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_warn(loc,((_tmp41C="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp41C,sizeof(char),46))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL110:
 goto _LL111;case Cyc_Absyn_Register: _LL111:
 goto _LL112;case Cyc_Absyn_Extern: _LL112:
 return 1;case Cyc_Absyn_ExternC: _LL113:
# 1024
{const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_Tcutil_warn(loc,((_tmp41F="nested extern \"C\" declaration",_tag_dyneither(_tmp41F,sizeof(char),30))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}
return 1;}}
# 1029
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _stmttmp1E=*exp;unsigned int _tmp225;struct _tuple0*_tmp226;struct _tuple12 _tmp224=_stmttmp1E;_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;{
union Cyc_Absyn_Nmspace _tmp228;struct _dyneither_ptr*_tmp229;struct _tuple0*_tmp227=_tmp226;_tmp228=_tmp227->f1;_tmp229=_tmp227->f2;{
union Cyc_Absyn_Nmspace _tmp22A=_tmp228;_LL116: if((_tmp22A.Rel_n).tag != 1)goto _LL118;if((struct Cyc_List_List*)(_tmp22A.Rel_n).val != 0)goto _LL118;_LL117:
# 1035
(*_tmp226).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL115;_LL118:;_LL119:
 goto _LL115;_LL115:;};};}
# 1046
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1053
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1056
struct Cyc_List_List*_tmp22B=ds0;for(0;_tmp22B != 0;(last=_tmp22B,_tmp22B=_tmp22B->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp22B->hd;
unsigned int loc=d->loc;
# 1060
void*_stmttmp1F=d->r;void*_tmp22C=_stmttmp1F;struct Cyc_Absyn_Vardecl*_tmp232;struct Cyc_Absyn_Fndecl*_tmp234;struct Cyc_Absyn_Typedefdecl*_tmp236;struct Cyc_Absyn_Aggrdecl*_tmp238;struct Cyc_Absyn_Datatypedecl*_tmp23A;struct Cyc_Absyn_Enumdecl*_tmp23C;struct _dyneither_ptr*_tmp240;struct Cyc_List_List*_tmp241;union Cyc_Absyn_Nmspace _tmp243;struct _dyneither_ptr*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_List_List*_tmp247;struct Cyc_List_List*_tmp249;struct Cyc_List_List*_tmp24A;_LL11B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp22D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp22D->tag != 2)goto _LL11D;}_LL11C:
 goto _LL11E;_LL11D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp22E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp22E->tag != 3)goto _LL11F;}_LL11E:
# 1063
{const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_Tcutil_terr(loc,((_tmp422="top level let-declarations are not implemented",_tag_dyneither(_tmp422,sizeof(char),47))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
goto _LL11A;_LL11F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp22F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp22F->tag != 4)goto _LL121;}_LL120:
# 1066
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_Tcutil_terr(loc,((_tmp425="top level region declarations are not implemented",_tag_dyneither(_tmp425,sizeof(char),50))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
goto _LL11A;_LL121: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp230=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp230->tag != 5)goto _LL123;}_LL122:
# 1069
{const char*_tmp428;void*_tmp427;(_tmp427=0,Cyc_Tcutil_terr(loc,((_tmp428="top level alias declarations are not implemented",_tag_dyneither(_tmp428,sizeof(char),49))),_tag_dyneither(_tmp427,sizeof(void*),0)));}
goto _LL11A;_LL123: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp231=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp231->tag != 0)goto _LL125;else{_tmp232=_tmp231->f1;}}_LL124:
# 1072
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp232->sc,in_externCinclude))
_tmp232->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp232,check_var_init,in_externCinclude,exports);
goto _LL11A;_LL125: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp233=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp233->tag != 1)goto _LL127;else{_tmp234=_tmp233->f1;}}_LL126:
# 1077
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp234->sc,in_externCinclude))
_tmp234->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp234,in_externCinclude,exports);
goto _LL11A;_LL127: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp235=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp235->tag != 9)goto _LL129;else{_tmp236=_tmp235->f1;}}_LL128:
# 1082
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp236);
goto _LL11A;_LL129: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp237=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp237->tag != 6)goto _LL12B;else{_tmp238=_tmp237->f1;}}_LL12A:
# 1085
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp238->sc,in_externCinclude))
_tmp238->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp238);
goto _LL11A;_LL12B: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp239=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp239->tag != 7)goto _LL12D;else{_tmp23A=_tmp239->f1;}}_LL12C:
# 1090
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23A->sc,in_externCinclude))
_tmp23A->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp23A);
goto _LL11A;_LL12D: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp23B=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp23B->tag != 8)goto _LL12F;else{_tmp23C=_tmp23B->f1;}}_LL12E:
# 1095
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp23C->sc,in_externCinclude))
_tmp23C->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp23C);
goto _LL11A;_LL12F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp23D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp23D->tag != 14)goto _LL131;}_LL130:
# 1100
{const char*_tmp42B;void*_tmp42A;(_tmp42A=0,Cyc_Tcutil_warn(d->loc,((_tmp42B="spurious __cyclone_port_on__",_tag_dyneither(_tmp42B,sizeof(char),29))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}
goto _LL11A;_LL131: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp23E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp23E->tag != 15)goto _LL133;}_LL132:
# 1103
 goto _LL11A;_LL133: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp23F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp23F->tag != 10)goto _LL135;else{_tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;}}_LL134: {
# 1106
struct Cyc_List_List*_tmp253=te->ns;
struct Cyc_List_List*_tmp42C;struct Cyc_List_List*_tmp254=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp253,((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C->hd=_tmp240,((_tmp42C->tl=0,_tmp42C)))))));
# 1110
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp240)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp240);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp254,Cyc_Tcenv_empty_genv(grgn));}
# 1115
te->ns=_tmp254;
Cyc_Tc_tc_decls(te,_tmp241,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp253;
goto _LL11A;}_LL135: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp242=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp242->tag != 11)goto _LL137;else{_tmp243=(_tmp242->f1)->f1;_tmp244=(_tmp242->f1)->f2;_tmp245=_tmp242->f2;}}_LL136: {
# 1122
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp256=_tmp243;struct _dyneither_ptr*_tmp257;struct Cyc_List_List*_tmp258;struct _dyneither_ptr*_tmp259;struct Cyc_List_List*_tmp25A;struct _dyneither_ptr*_tmp25B;struct Cyc_List_List*_tmp25C;_LL13C: if((_tmp256.Loc_n).tag != 4)goto _LL13E;_LL13D:
 goto _LL13F;_LL13E: if((_tmp256.Rel_n).tag != 1)goto _LL140;if((struct Cyc_List_List*)(_tmp256.Rel_n).val != 0)goto _LL140;_LL13F:
 goto _LL141;_LL140: if((_tmp256.C_n).tag != 3)goto _LL142;if((struct Cyc_List_List*)(_tmp256.C_n).val != 0)goto _LL142;_LL141:
 goto _LL143;_LL142: if((_tmp256.Abs_n).tag != 2)goto _LL144;if((struct Cyc_List_List*)(_tmp256.Abs_n).val != 0)goto _LL144;_LL143:
# 1129
 first=_tmp244;rest=0;goto _LL13B;_LL144: if((_tmp256.Rel_n).tag != 1)goto _LL146;if((struct Cyc_List_List*)(_tmp256.Rel_n).val == 0)goto _LL146;_tmp257=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp256.Rel_n).val)->hd;_tmp258=((struct Cyc_List_List*)(_tmp256.Rel_n).val)->tl;_LL145:
 _tmp259=_tmp257;_tmp25A=_tmp258;goto _LL147;_LL146: if((_tmp256.C_n).tag != 3)goto _LL148;if((struct Cyc_List_List*)(_tmp256.C_n).val == 0)goto _LL148;_tmp259=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp256.C_n).val)->hd;_tmp25A=((struct Cyc_List_List*)(_tmp256.C_n).val)->tl;_LL147:
 _tmp25B=_tmp259;_tmp25C=_tmp25A;goto _LL149;_LL148: if((_tmp256.Abs_n).tag != 2)goto _LL13B;if((struct Cyc_List_List*)(_tmp256.Abs_n).val == 0)goto _LL13B;_tmp25B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp256.Abs_n).val)->hd;_tmp25C=((struct Cyc_List_List*)(_tmp256.Abs_n).val)->tl;_LL149:
# 1133
 first=_tmp25B;{struct Cyc_List_List*_tmp42D;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp25C,((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->hd=_tmp244,((_tmp42D->tl=0,_tmp42D)))))));}goto _LL13B;_LL13B:;}{
# 1135
struct Cyc_List_List*_tmp25E=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp42E;ge->availables=((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->hd=_tmp25E,((_tmp42E->tl=ge->availables,_tmp42E))))));}
Cyc_Tc_tc_decls(te,_tmp245,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL11A;};}_LL137: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp246=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp246->tag != 12)goto _LL139;else{_tmp247=_tmp246->f1;}}_LL138:
# 1142
 Cyc_Tc_tc_decls(te,_tmp247,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL11A;_LL139: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp248=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp22C;if(_tmp248->tag != 13)goto _LL11A;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LL13A:
# 1146
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp24A);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp24A,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp249,1,1,check_var_init,grgn,& newexs);
# 1151
for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){
struct _tuple12*_tmp260=(struct _tuple12*)_tmp24A->hd;
if(!(*_tmp260).f3){
const char*_tmp432;void*_tmp431[1];struct Cyc_String_pa_PrintArg_struct _tmp430;(_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp260).f2)),((_tmp431[0]=& _tmp430,Cyc_Tcutil_warn((*_tmp260).f1,((_tmp432="%s is exported but not defined",_tag_dyneither(_tmp432,sizeof(char),31))),_tag_dyneither(_tmp431,sizeof(void*),1)))))));}}
# 1157
goto _LL11A;};_LL11A:;}}
# 1162
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1164
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1181 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1183
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1185
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1189
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_stmttmp20=d->r;void*_tmp264=_stmttmp20;struct Cyc_Absyn_Vardecl*_tmp266;struct Cyc_List_List**_tmp268;struct Cyc_List_List**_tmp26A;struct Cyc_List_List**_tmp26C;struct Cyc_List_List**_tmp26E;_LL14B: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp265=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp265->tag != 0)goto _LL14D;else{_tmp266=_tmp265->f1;}}_LL14C:
# 1194
 if((env->in_cinclude  || _tmp266->sc != Cyc_Absyn_Extern  && _tmp266->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp266->name,Cyc_Absyn_uniquergn_qvar))
# 1197
return 1;{
# 1199
struct _tuple0*_stmttmp21=_tmp266->name;union Cyc_Absyn_Nmspace _tmp270;struct _dyneither_ptr*_tmp271;struct _tuple0*_tmp26F=_stmttmp21;_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp272=_tmp270;struct Cyc_List_List*_tmp273;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp275;_LL158: if((_tmp272.Loc_n).tag != 4)goto _LL15A;_LL159:
 ns=0;goto _LL157;_LL15A: if((_tmp272.Rel_n).tag != 1)goto _LL15C;_tmp273=(struct Cyc_List_List*)(_tmp272.Rel_n).val;_LL15B:
 ns=_tmp273;goto _LL157;_LL15C: if((_tmp272.C_n).tag != 3)goto _LL15E;_tmp274=(struct Cyc_List_List*)(_tmp272.C_n).val;_LL15D:
 ns=_tmp274;goto _LL157;_LL15E: if((_tmp272.Abs_n).tag != 2)goto _LL157;_tmp275=(struct Cyc_List_List*)(_tmp272.Abs_n).val;_LL15F:
 ns=_tmp275;goto _LL157;_LL157:;}{
# 1207
struct _tuple17*_tmp276=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp277=(*_tmp276).f1;
int _tmp278=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp277->ordinaries,_tmp271)).f2;
if(!_tmp278)
(*_tmp276).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp276).f2,_tmp271);
# 1215
return _tmp278;};};};_LL14D: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp267=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp267->tag != 12)goto _LL14F;else{_tmp268=(struct Cyc_List_List**)& _tmp267->f1;}}_LL14E:
 _tmp26A=_tmp268;goto _LL150;_LL14F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp269=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp269->tag != 11)goto _LL151;else{_tmp26A=(struct Cyc_List_List**)& _tmp269->f2;}}_LL150:
 _tmp26C=_tmp26A;goto _LL152;_LL151: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp26B=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp26B->tag != 10)goto _LL153;else{_tmp26C=(struct Cyc_List_List**)& _tmp26B->f2;}}_LL152:
# 1219
*_tmp26C=Cyc_Tc_treeshake_f(env,*_tmp26C);
return 1;_LL153: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp26D=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp264;if(_tmp26D->tag != 13)goto _LL155;else{_tmp26E=(struct Cyc_List_List**)& _tmp26D->f1;}}_LL154: {
# 1222
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp26E=Cyc_Tc_treeshake_f(env,*_tmp26E);
env->in_cinclude=in_cinclude;
return 1;}_LL155:;_LL156:
 return 1;_LL14A:;}
# 1231
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1235
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1237
struct _tuple17*_tmp433;return(_tmp433=_region_malloc(rgn,sizeof(*_tmp433)),((_tmp433->f1=ge,((_tmp433->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp433)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1240
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp27A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp27A;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp434;struct Cyc_Tc_TreeshakeEnv _tmp27B=(_tmp434.rgn=rgn,((_tmp434.in_cinclude=0,((_tmp434.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp434)))));
struct Cyc_List_List*_tmp27C=Cyc_Tc_treeshake_f(& _tmp27B,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp27B.nsdict)){
struct Cyc_List_List*_tmp27D=_tmp27C;_npop_handler(0);return _tmp27D;}{
# 1248
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp27B.nsdict);
struct _tuple18 _tmp27E=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp27B.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp27E)){
struct Cyc_Tcenv_Genv*_tmp280;struct Cyc_Set_Set*_tmp281;struct _tuple18 _tmp27F=_tmp27E;_tmp280=(_tmp27F.f2)->f1;_tmp281=(_tmp27F.f2)->f2;
_tmp280->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp280->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp281,_tmp280->ordinaries);}{
# 1254
struct Cyc_List_List*_tmp282=_tmp27C;_npop_handler(0);return _tmp282;};};}
# 1242
;_pop_region(rgn);}
