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
# 128
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 192
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 220
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 66
void*Cyc_Tcutil_compress(void*t);
# 69
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 96
int Cyc_Tcutil_is_function_type(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 139
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 153
void Cyc_Tcutil_explain_failure();
# 155
int Cyc_Tcutil_unify(void*,void*);
# 182
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 222 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 224
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 232
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 236
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 278
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 313
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 324
int Cyc_Tcutil_bits_only(void*t);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 337
int Cyc_Tcutil_supports_default(void*);
# 348
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 352
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
const char*_tmp262;void*_tmp261[1];struct Cyc_Int_pa_PrintArg_struct _tmp260;(_tmp260.tag=1,((_tmp260.f1=(unsigned long)_tmp9,((_tmp261[0]=& _tmp260,Cyc_Tcutil_terr(loc,((_tmp262="initializes attribute: index %d out of range",_tag_dyneither(_tmp262,sizeof(char),45))),_tag_dyneither(_tmp261,sizeof(void*),1)))))));}
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmp9)){
struct Cyc_Int_pa_PrintArg_struct _tmp26A;void*_tmp269[1];const char*_tmp268;void*_tmp267;(_tmp267=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp26A.tag=1,((_tmp26A.f1=(unsigned long)_tmp9,((_tmp269[0]=& _tmp26A,Cyc_rprintf(temp,((_tmp268="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp268,sizeof(char),73))),_tag_dyneither(_tmp269,sizeof(void*),1)))))))),_tag_dyneither(_tmp267,sizeof(void*),0)));}
# 80
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmp9)){
struct Cyc_Int_pa_PrintArg_struct _tmp272;void*_tmp271[1];const char*_tmp270;void*_tmp26F;(_tmp26F=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp272.tag=1,((_tmp272.f1=(unsigned long)_tmp9,((_tmp271[0]=& _tmp272,Cyc_rprintf(temp,((_tmp270="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp270,sizeof(char),70))),_tag_dyneither(_tmp271,sizeof(void*),1)))))))),_tag_dyneither(_tmp26F,sizeof(void*),0)));}
# 83
{struct Cyc_List_List*_tmp273;init_params=((_tmp273=_region_malloc(temp,sizeof(*_tmp273)),((_tmp273->hd=(void*)_tmp9,((_tmp273->tl=init_params,_tmp273))))));}goto _LL5;_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpA=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp7;if(_tmpA->tag != 21)goto _LLA;else{_tmpB=_tmpA->f1;}}_LL9:
# 85
 if(_tmpB < 1  || _tmpB > nargs){
const char*_tmp277;void*_tmp276[1];struct Cyc_Int_pa_PrintArg_struct _tmp275;(_tmp275.tag=1,((_tmp275.f1=(unsigned long)_tmpB,((_tmp276[0]=& _tmp275,Cyc_Tcutil_terr(loc,((_tmp277="noliveunique attribute: index %d out of range",_tag_dyneither(_tmp277,sizeof(char),46))),_tag_dyneither(_tmp276,sizeof(void*),1)))))));}
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpB)){
struct Cyc_Int_pa_PrintArg_struct _tmp27F;void*_tmp27E[1];const char*_tmp27D;void*_tmp27C;(_tmp27C=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp27F.tag=1,((_tmp27F.f1=(unsigned long)_tmpB,((_tmp27E[0]=& _tmp27F,Cyc_rprintf(temp,((_tmp27D="incompatible initializes() and noliveunique() attributes on parameter %d",_tag_dyneither(_tmp27D,sizeof(char),73))),_tag_dyneither(_tmp27E,sizeof(void*),1)))))))),_tag_dyneither(_tmp27C,sizeof(void*),0)));}
# 91
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,_tmpB)){
struct Cyc_Int_pa_PrintArg_struct _tmp287;void*_tmp286[1];const char*_tmp285;void*_tmp284;(_tmp284=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp287.tag=1,((_tmp287.f1=(unsigned long)_tmpB,((_tmp286[0]=& _tmp287,Cyc_rprintf(temp,((_tmp285="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp285,sizeof(char),71))),_tag_dyneither(_tmp286,sizeof(void*),1)))))))),_tag_dyneither(_tmp284,sizeof(void*),0)));}
# 94
{struct Cyc_List_List*_tmp288;nolive_unique_params=((_tmp288=_region_malloc(temp,sizeof(*_tmp288)),((_tmp288->hd=(void*)_tmpB,((_tmp288->tl=nolive_unique_params,_tmp288))))));}goto _LL5;_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpC=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp7;if(_tmpC->tag != 22)goto _LLC;else{_tmpD=_tmpC->f1;}}_LLB:
# 96
 if(_tmpD < 1  || _tmpD > nargs){
{const char*_tmp28C;void*_tmp28B[1];struct Cyc_Int_pa_PrintArg_struct _tmp28A;(_tmp28A.tag=1,((_tmp28A.f1=(unsigned long)_tmpD,((_tmp28B[0]=& _tmp28A,Cyc_Tcutil_terr(loc,((_tmp28C="noconsume attribute: index %d out of range",_tag_dyneither(_tmp28C,sizeof(char),43))),_tag_dyneither(_tmp28B,sizeof(void*),1)))))));}
continue;}
# 100
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,_tmpD)){
struct Cyc_Int_pa_PrintArg_struct _tmp294;void*_tmp293[1];const char*_tmp292;void*_tmp291;(_tmp291=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp294.tag=1,((_tmp294.f1=(unsigned long)_tmpD,((_tmp293[0]=& _tmp294,Cyc_rprintf(temp,((_tmp292="incompatible initializes() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp292,sizeof(char),70))),_tag_dyneither(_tmp293,sizeof(void*),1)))))))),_tag_dyneither(_tmp291,sizeof(void*),0)));}
# 104
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,_tmpD)){
struct Cyc_Int_pa_PrintArg_struct _tmp29C;void*_tmp29B[1];const char*_tmp29A;void*_tmp299;(_tmp299=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)((_tmp29C.tag=1,((_tmp29C.f1=(unsigned long)_tmpD,((_tmp29B[0]=& _tmp29C,Cyc_rprintf(temp,((_tmp29A="incompatible noliveunique() and noconsume() attributes on parameter %d",_tag_dyneither(_tmp29A,sizeof(char),71))),_tag_dyneither(_tmp29B,sizeof(void*),1)))))))),_tag_dyneither(_tmp299,sizeof(void*),0)));}{
# 107
struct _tuple8*_stmttmp2=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6,_tmpD - 1);void*_tmp32;struct _tuple8*_tmp31=_stmttmp2;_tmp32=_tmp31->f3;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp32,0)){
const char*_tmp29F;void*_tmp29E;(_tmp29E=0,Cyc_Tcutil_terr(loc,((_tmp29F="noconsume attribute only allowed on unique pointers",_tag_dyneither(_tmp29F,sizeof(char),52))),_tag_dyneither(_tmp29E,sizeof(void*),0)));}
{struct Cyc_List_List*_tmp2A0;noconsume_params=((_tmp2A0=_region_malloc(temp,sizeof(*_tmp2A0)),((_tmp2A0->hd=(void*)_tmpD,((_tmp2A0->tl=noconsume_params,_tmp2A0))))));}goto _LL5;};_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 114
goto _LL0;}_LL3:;_LL4: {
# 116
const char*_tmp2A3;void*_tmp2A2;(_tmp2A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2A3="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2A3,sizeof(char),34))),_tag_dyneither(_tmp2A2,sizeof(void*),0)));}_LL0:;}
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
const char*_tmp2A6;void*_tmp2A5;(_tmp2A5=0,Cyc_Tcutil_terr(loc,((_tmp2A6="extern declaration should not have initializer",_tag_dyneither(_tmp2A6,sizeof(char),47))),_tag_dyneither(_tmp2A5,sizeof(void*),0)));}}else{
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
{const char*_tmp2AB;void*_tmp2AA[2];struct Cyc_String_pa_PrintArg_struct _tmp2A9;struct Cyc_String_pa_PrintArg_struct _tmp2A8;(_tmp2A8.tag=0,((_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2A9.tag=0,((_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp3E->hd)),((_tmp2AA[0]=& _tmp2A9,((_tmp2AA[1]=& _tmp2A8,Cyc_Tcutil_terr(loc,((_tmp2AB="bad attribute %s for variable %s",_tag_dyneither(_tmp2AB,sizeof(char),33))),_tag_dyneither(_tmp2AA,sizeof(void*),2)))))))))))));}
goto _LL2C;_LL2C:;}
# 204
if(_tmp3D == 0  || in_cinclude){
if(check_var_init  && !Cyc_Tcutil_supports_default(_tmp3C)){
const char*_tmp2B0;void*_tmp2AF[2];struct Cyc_String_pa_PrintArg_struct _tmp2AE;struct Cyc_String_pa_PrintArg_struct _tmp2AD;(_tmp2AD.tag=0,((_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp3C)),((_tmp2AE.tag=0,((_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2AF[0]=& _tmp2AE,((_tmp2AF[1]=& _tmp2AD,Cyc_Tcutil_terr(loc,((_tmp2B0="initializer required for variable %s of type %s",_tag_dyneither(_tmp2B0,sizeof(char),48))),_tag_dyneither(_tmp2AF,sizeof(void*),2)))))))))))));}}else{
# 209
struct Cyc_Absyn_Exp*_tmp68=_tmp3D;
# 211
void*_tmp69=Cyc_Tcexp_tcExpInitializer(te,& _tmp3C,_tmp68);
if(!Cyc_Tcutil_coerce_assign(te,_tmp68,_tmp3C)){
{const char*_tmp2B6;void*_tmp2B5[3];struct Cyc_String_pa_PrintArg_struct _tmp2B4;struct Cyc_String_pa_PrintArg_struct _tmp2B3;struct Cyc_String_pa_PrintArg_struct _tmp2B2;(_tmp2B2.tag=0,((_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp69)),((_tmp2B3.tag=0,((_tmp2B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3C)),((_tmp2B4.tag=0,((_tmp2B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2B5[0]=& _tmp2B4,((_tmp2B5[1]=& _tmp2B3,((_tmp2B5[2]=& _tmp2B2,Cyc_Tcutil_terr(loc,((_tmp2B6="%s declared with type \n%s\n but initialized with type \n%s",_tag_dyneither(_tmp2B6,sizeof(char),57))),_tag_dyneither(_tmp2B5,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}
# 218
if(!Cyc_Tcutil_is_const_exp(_tmp68)){
const char*_tmp2B9;void*_tmp2B8;(_tmp2B8=0,Cyc_Tcutil_terr(loc,((_tmp2B9="initializer is not a constant expression",_tag_dyneither(_tmp2B9,sizeof(char),41))),_tag_dyneither(_tmp2B8,sizeof(void*),0)));}}}else{
# 223
for(0;_tmp3E != 0;_tmp3E=_tmp3E->tl){
void*_stmttmp7=(void*)_tmp3E->hd;void*_tmp71=_stmttmp7;_LL40: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp72=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL42;}_LL41:
# 226
 goto _LL43;_LL42: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp73=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp71;if(_tmp73->tag != 1)goto _LL44;}_LL43:
 goto _LL45;_LL44: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp74=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp71;if(_tmp74->tag != 2)goto _LL46;}_LL45:
 goto _LL47;_LL46: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp75=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp71;if(_tmp75->tag != 3)goto _LL48;}_LL47:
 goto _LL49;_LL48: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp76=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp71;if(_tmp76->tag != 4)goto _LL4A;}_LL49:
 goto _LL4B;_LL4A: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp77=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp71;if(_tmp77->tag != 19)goto _LL4C;}_LL4B:
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp78=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp71;if(_tmp78->tag != 20)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp79=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp71;if(_tmp79->tag != 23)goto _LL50;}_LL4F:
 goto _LL51;_LL50: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp7A=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp71;if(_tmp7A->tag != 5)goto _LL52;}_LL51: {
const char*_tmp2BC;void*_tmp2BB;(_tmp2BB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2BC="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp2BC,sizeof(char),45))),_tag_dyneither(_tmp2BB,sizeof(void*),0)));}_LL52: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp7B=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp71;if(_tmp7B->tag != 6)goto _LL54;}_LL53:
# 236
 goto _LL55;_LL54: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp7C=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp71;if(_tmp7C->tag != 7)goto _LL56;}_LL55:
# 238
{const char*_tmp2C0;void*_tmp2BF[1];struct Cyc_String_pa_PrintArg_struct _tmp2BE;(_tmp2BE.tag=0,((_tmp2BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp3E->hd)),((_tmp2BF[0]=& _tmp2BE,Cyc_Tcutil_terr(loc,((_tmp2C0="bad attribute %s in function declaration",_tag_dyneither(_tmp2C0,sizeof(char),41))),_tag_dyneither(_tmp2BF,sizeof(void*),1)))))));}
goto _LL3F;_LL56:;_LL57:
 continue;_LL3F:;}}}
# 246
{struct _handler_cons _tmp82;_push_handler(& _tmp82);{int _tmp84=0;if(setjmp(_tmp82.handler))_tmp84=1;if(!_tmp84){
{struct _tuple13*_tmp85=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp3B);
void*_stmttmp8=(*_tmp85).f1;void*_tmp86=_stmttmp8;void*_tmp88;_LL59: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp87=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp86;if(_tmp87->tag != 0)goto _LL5B;else{_tmp88=(void*)_tmp87->f1;}}_LL5A: {
# 250
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2C3;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2C2;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8D=(_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C3.tag=1,((_tmp2C3.f1=vd,_tmp2C3)))),_tmp2C2)));
void*_tmp8E=Cyc_Tcdecl_merge_binding(_tmp88,(void*)_tmp8D,loc,Cyc_Tc_tc_msg);
if(_tmp8E == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 254
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp8E != _tmp88  || (*_tmp85).f2){
# 257
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2C9;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2C8;struct _tuple13*_tmp2C7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3B,(
(_tmp2C7=_cycalloc(sizeof(*_tmp2C7)),((_tmp2C7->f1=(void*)((_tmp2C9=_cycalloc(sizeof(*_tmp2C9)),((_tmp2C9[0]=((_tmp2C8.tag=0,((_tmp2C8.f1=_tmp8E,_tmp2C8)))),_tmp2C9)))),((_tmp2C7->f2=1,_tmp2C7)))))));}}
_npop_handler(0);return;}_LL5B: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp89=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp86;if(_tmp89->tag != 1)goto _LL5D;}_LL5C:
# 261
{const char*_tmp2CC;void*_tmp2CB;(_tmp2CB=0,Cyc_Tcutil_warn(loc,((_tmp2CC="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp2CC,sizeof(char),57))),_tag_dyneither(_tmp2CB,sizeof(void*),0)));}
goto _LL58;_LL5D: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp8A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp86;if(_tmp8A->tag != 2)goto _LL5F;}_LL5E:
# 264
{const char*_tmp2CF;void*_tmp2CE;(_tmp2CE=0,Cyc_Tcutil_warn(loc,((_tmp2CF="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp2CF,sizeof(char),59))),_tag_dyneither(_tmp2CE,sizeof(void*),0)));}
goto _LL58;_LL5F: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp8B=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp86;if(_tmp8B->tag != 4)goto _LL61;}_LL60:
 goto _LL62;_LL61: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp8C=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp86;if(_tmp8C->tag != 3)goto _LL58;}_LL62:
# 268
{const char*_tmp2D2;void*_tmp2D1;(_tmp2D1=0,Cyc_Tcutil_warn(loc,((_tmp2D2="variable declaration shadows previous enum tag",_tag_dyneither(_tmp2D2,sizeof(char),47))),_tag_dyneither(_tmp2D1,sizeof(void*),0)));}
goto _LL58;_LL58:;}
# 247
;_pop_handler();}else{void*_tmp83=(void*)_exn_thrown;void*_tmp9B=_tmp83;void*_tmp9D;_LL64: {struct Cyc_Dict_Absent_exn_struct*_tmp9C=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B;if(_tmp9C->tag != Cyc_Dict_Absent)goto _LL66;}_LL65:
# 271
 goto _LL63;_LL66: _tmp9D=_tmp9B;_LL67:(void)_throw(_tmp9D);_LL63:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2E1;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2E0;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp2DF;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2DE;struct _tuple13*_tmp2DD;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp3B,(
(_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((_tmp2DD->f1=(void*)((_tmp2E1=_cycalloc(sizeof(*_tmp2E1)),((_tmp2E1[0]=((_tmp2DE.tag=0,((_tmp2DE.f1=(void*)((_tmp2E0=_cycalloc(sizeof(*_tmp2E0)),((_tmp2E0[0]=((_tmp2DF.tag=1,((_tmp2DF.f1=vd,_tmp2DF)))),_tmp2E0)))),_tmp2DE)))),_tmp2E1)))),((_tmp2DD->f2=0,_tmp2DD)))))));}}
# 277
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmpA4;struct _dyneither_ptr*_tmpA5;struct _tuple0*_tmpA3=n;_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;{
union Cyc_Absyn_Nmspace _tmpA6=_tmpA4;_LL69: if((_tmpA6.Abs_n).tag != 2)goto _LL6B;if((struct Cyc_List_List*)(_tmpA6.Abs_n).val != 0)goto _LL6B;_LL6A: {
# 281
const char*_tmp2E2;return Cyc_strcmp((struct _dyneither_ptr)*_tmpA5,((_tmp2E2="main",_tag_dyneither(_tmp2E2,sizeof(char),5))))== 0;}_LL6B:;_LL6C:
 return 0;_LL68:;};}
# 286
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 289
struct _dyneither_ptr*v=(*fd->name).f2;
# 293
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp2E5;void*_tmp2E4;(_tmp2E4=0,Cyc_Tcutil_terr(loc,((_tmp2E5="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp2E5,sizeof(char),54))),_tag_dyneither(_tmp2E4,sizeof(void*),0)));}
# 296
{union Cyc_Absyn_Nmspace _stmttmp9=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpAA=_stmttmp9;_LL6E: if((_tmpAA.Rel_n).tag != 1)goto _LL70;if((struct Cyc_List_List*)(_tmpAA.Rel_n).val != 0)goto _LL70;_LL6F:
# 299
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL6D;_LL70:;_LL71:
# 302
 goto _LL6D;_LL6D:;}
# 305
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 307
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 309
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 312
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmpA=(void*)atts->hd;void*_tmpAB=_stmttmpA;_LL73: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpAC=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAC->tag != 7)goto _LL75;}_LL74:
 goto _LL76;_LL75: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAD=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAD->tag != 6)goto _LL77;}_LL76:
# 316
{const char*_tmp2E9;void*_tmp2E8[1];struct Cyc_String_pa_PrintArg_struct _tmp2E7;(_tmp2E7.tag=0,((_tmp2E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp2E8[0]=& _tmp2E7,Cyc_Tcutil_terr(loc,((_tmp2E9="bad attribute %s for function",_tag_dyneither(_tmp2E9,sizeof(char),30))),_tag_dyneither(_tmp2E8,sizeof(void*),1)))))));}
goto _LL72;_LL77:;_LL78:
 goto _LL72;_LL72:;}}
# 322
{struct _handler_cons _tmpB1;_push_handler(& _tmpB1);{int _tmpB3=0;if(setjmp(_tmpB1.handler))_tmpB3=1;if(!_tmpB3){
{struct _tuple13*_tmpB4=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_stmttmpB=(*_tmpB4).f1;void*_tmpB5=_stmttmpB;void*_tmpB7;_LL7A: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB5;if(_tmpB6->tag != 0)goto _LL7C;else{_tmpB7=(void*)_tmpB6->f1;}}_LL7B: {
# 326
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2EC;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2EB;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBC=(_tmp2EB=_cycalloc(sizeof(*_tmp2EB)),((_tmp2EB[0]=((_tmp2EC.tag=2,((_tmp2EC.f1=fd,_tmp2EC)))),_tmp2EB)));
void*_tmpBD=Cyc_Tcdecl_merge_binding(_tmpB7,(void*)_tmpBC,loc,Cyc_Tc_tc_msg);
if(_tmpBD == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL79;
# 330
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpBD == _tmpB7  && (*_tmpB4).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp2F2;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp2F1;struct _tuple13*_tmp2F0;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp2F0=_cycalloc(sizeof(*_tmp2F0)),((_tmp2F0->f1=(void*)((_tmp2F2=_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2[0]=((_tmp2F1.tag=0,((_tmp2F1.f1=_tmpBD,_tmp2F1)))),_tmp2F2)))),((_tmp2F0->f2=1,_tmp2F0)))))));}}
goto _LL79;}_LL7C: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB8=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB5;if(_tmpB8->tag != 1)goto _LL7E;}_LL7D:
# 336
{const char*_tmp2F5;void*_tmp2F4;(_tmp2F4=0,Cyc_Tcutil_warn(loc,((_tmp2F5="function declaration shadows previous type declaration",_tag_dyneither(_tmp2F5,sizeof(char),55))),_tag_dyneither(_tmp2F4,sizeof(void*),0)));}
goto _LL79;_LL7E: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB5;if(_tmpB9->tag != 2)goto _LL80;}_LL7F:
# 339
{const char*_tmp2F8;void*_tmp2F7;(_tmp2F7=0,Cyc_Tcutil_warn(loc,((_tmp2F8="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp2F8,sizeof(char),59))),_tag_dyneither(_tmp2F7,sizeof(void*),0)));}
goto _LL79;_LL80: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpBA=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB5;if(_tmpBA->tag != 4)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpBB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB5;if(_tmpBB->tag != 3)goto _LL79;}_LL83:
# 343
{const char*_tmp2FB;void*_tmp2FA;(_tmp2FA=0,Cyc_Tcutil_warn(loc,((_tmp2FB="function declaration shadows previous enum tag",_tag_dyneither(_tmp2FB,sizeof(char),47))),_tag_dyneither(_tmp2FA,sizeof(void*),0)));}
goto _LL79;_LL79:;}
# 323
;_pop_handler();}else{void*_tmpB2=(void*)_exn_thrown;void*_tmpCA=_tmpB2;void*_tmpCC;_LL85: {struct Cyc_Dict_Absent_exn_struct*_tmpCB=(struct Cyc_Dict_Absent_exn_struct*)_tmpCA;if(_tmpCB->tag != Cyc_Dict_Absent)goto _LL87;}_LL86:
# 347
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp2FE;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp2FD;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpCD=(_tmp2FD=_cycalloc(sizeof(*_tmp2FD)),((_tmp2FD[0]=((_tmp2FE.tag=2,((_tmp2FE.f1=fd,_tmp2FE)))),_tmp2FD)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp30D;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp30C;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp30B;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp30A;struct _tuple13*_tmp309;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp309=_cycalloc(sizeof(*_tmp309)),((_tmp309->f1=(void*)((_tmp30D=_cycalloc(sizeof(*_tmp30D)),((_tmp30D[0]=((_tmp30A.tag=0,((_tmp30A.f1=(void*)((_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C[0]=((_tmp30B.tag=2,((_tmp30B.f1=fd,_tmp30B)))),_tmp30C)))),_tmp30A)))),_tmp30D)))),((_tmp309->f2=0,_tmp309)))))));}
# 352
goto _LL84;_LL87: _tmpCC=_tmpCA;_LL88:(void)_throw(_tmpCC);_LL84:;}};}
# 356
if(in_cinclude)return;{
# 361
struct _RegionHandle _tmpD5=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpD5;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmpD6=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp30E;struct Cyc_Tcenv_Tenv*_tmpD7=(_tmp30E=_region_malloc(fnrgn,sizeof(*_tmp30E)),((_tmp30E->ns=te->ns,((_tmp30E->ae=te->ae,((_tmp30E->le=_tmpD6,((_tmp30E->allow_valueof=0,_tmp30E)))))))));
# 365
Cyc_Tcstmt_tcStmt(_tmpD7,fd->body,0);
# 368
Cyc_Tcenv_check_delayed_effects(_tmpD7);
Cyc_Tcenv_check_delayed_constraints(_tmpD7);
# 372
if(!Cyc_Tcenv_all_labels_resolved(_tmpD7)){
const char*_tmp311;void*_tmp310;(_tmp310=0,Cyc_Tcutil_terr(loc,((_tmp311="function has goto statements to undefined labels",_tag_dyneither(_tmp311,sizeof(char),49))),_tag_dyneither(_tmp310,sizeof(void*),0)));}}
# 376
if(Cyc_Tc_is_main(fd->name)){
# 378
{void*_stmttmpC=Cyc_Tcutil_compress(fd->ret_type);void*_tmpDB=_stmttmpC;enum Cyc_Absyn_Size_of _tmpDE;_LL8A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 0)goto _LL8C;}_LL8B:
{const char*_tmp314;void*_tmp313;(_tmp313=0,Cyc_Tcutil_warn(loc,((_tmp314="main declared with return type void",_tag_dyneither(_tmp314,sizeof(char),36))),_tag_dyneither(_tmp313,sizeof(void*),0)));}goto _LL89;_LL8C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDB;if(_tmpDD->tag != 6)goto _LL8E;else{_tmpDE=_tmpDD->f2;}}_LL8D:
 goto _LL89;_LL8E:;_LL8F:
# 382
{const char*_tmp318;void*_tmp317[1];struct Cyc_String_pa_PrintArg_struct _tmp316;(_tmp316.tag=0,((_tmp316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp317[0]=& _tmp316,Cyc_Tcutil_terr(loc,((_tmp318="main declared with return type %s instead of int or void",_tag_dyneither(_tmp318,sizeof(char),57))),_tag_dyneither(_tmp317,sizeof(void*),1)))))));}
goto _LL89;_LL89:;}
# 386
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp31B;void*_tmp31A;(_tmp31A=0,Cyc_Tcutil_terr(loc,((_tmp31B="main declared with varargs",_tag_dyneither(_tmp31B,sizeof(char),27))),_tag_dyneither(_tmp31A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpE6=fd->args;
if(_tmpE6 != 0){
struct _tuple8*_stmttmpD=(struct _tuple8*)_tmpE6->hd;void*_tmpE8;struct _tuple8*_tmpE7=_stmttmpD;_tmpE8=_tmpE7->f3;
{void*_stmttmpE=Cyc_Tcutil_compress(_tmpE8);void*_tmpE9=_stmttmpE;enum Cyc_Absyn_Size_of _tmpEB;_LL91: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE9;if(_tmpEA->tag != 6)goto _LL93;else{_tmpEB=_tmpEA->f2;}}_LL92:
 goto _LL90;_LL93:;_LL94:
# 394
{const char*_tmp31F;void*_tmp31E[1];struct Cyc_String_pa_PrintArg_struct _tmp31D;(_tmp31D.tag=0,((_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpE8)),((_tmp31E[0]=& _tmp31D,Cyc_Tcutil_terr(loc,((_tmp31F="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp31F,sizeof(char),60))),_tag_dyneither(_tmp31E,sizeof(void*),1)))))));}
goto _LL90;_LL90:;}
# 398
_tmpE6=_tmpE6->tl;
if(_tmpE6 != 0){
struct _tuple8*_stmttmpF=(struct _tuple8*)_tmpE6->hd;void*_tmpF0;struct _tuple8*_tmpEF=_stmttmpF;_tmpF0=_tmpEF->f3;
_tmpE6=_tmpE6->tl;
if(_tmpE6 != 0){
const char*_tmp322;void*_tmp321;(_tmp321=0,Cyc_Tcutil_terr(loc,((_tmp322="main declared with too many arguments",_tag_dyneither(_tmp322,sizeof(char),38))),_tag_dyneither(_tmp321,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp323;struct Cyc_Core_Opt*tvs=(_tmp323=_cycalloc(sizeof(*_tmp323)),((_tmp323->v=fd->tvs,_tmp323)));
struct Cyc_Core_Opt*_tmp32B;struct Cyc_Core_Opt*_tmp32A;struct Cyc_Core_Opt*_tmp329;struct Cyc_Core_Opt*_tmp328;struct Cyc_Core_Opt*_tmp327;struct Cyc_Core_Opt*_tmp326;struct Cyc_Core_Opt*_tmp325;struct Cyc_Core_Opt*_tmp324;if(((!Cyc_Tcutil_unify(_tmpF0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A->v=& Cyc_Tcutil_rk,_tmp32A)))),tvs)),
Cyc_Absyn_new_evar(((_tmp32B=_cycalloc(sizeof(*_tmp32B)),((_tmp32B->v=& Cyc_Tcutil_rk,_tmp32B)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpF0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp328=_cycalloc(sizeof(*_tmp328)),((_tmp328->v=& Cyc_Tcutil_rk,_tmp328)))),tvs)),
# 410
Cyc_Absyn_new_evar(((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329->v=& Cyc_Tcutil_rk,_tmp329)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326->v=& Cyc_Tcutil_rk,_tmp326)))),tvs)),
Cyc_Absyn_new_evar(((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327->v=& Cyc_Tcutil_rk,_tmp327)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpF0,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324->v=& Cyc_Tcutil_rk,_tmp324)))),tvs)),
# 417
Cyc_Absyn_new_evar(((_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325->v=& Cyc_Tcutil_rk,_tmp325)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp32F;void*_tmp32E[1];struct Cyc_String_pa_PrintArg_struct _tmp32D;(_tmp32D.tag=0,((_tmp32D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpF0)),((_tmp32E[0]=& _tmp32D,Cyc_Tcutil_terr(loc,((_tmp32F="second argument of main has type %s instead of char??",_tag_dyneither(_tmp32F,sizeof(char),54))),_tag_dyneither(_tmp32E,sizeof(void*),1)))))));}};}}};}
# 361
;_pop_region(fnrgn);};};}
# 427
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 433
{union Cyc_Absyn_Nmspace _stmttmp10=(*td->name).f1;union Cyc_Absyn_Nmspace _tmpFF=_stmttmp10;_LL96: if((_tmpFF.Rel_n).tag != 1)goto _LL98;_LL97:
# 436
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LL95;_LL98:;_LL99:
 goto _LL95;_LL95:;}
# 441
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp333;void*_tmp332[1];struct Cyc_String_pa_PrintArg_struct _tmp331;(_tmp331.tag=0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp332[0]=& _tmp331,Cyc_Tcutil_terr(loc,((_tmp333="redeclaration of typedef %s",_tag_dyneither(_tmp333,sizeof(char),28))),_tag_dyneither(_tmp332,sizeof(void*),1)))))));}
return;}
# 446
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 455
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_stmttmp11=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp103=_stmttmp11;struct Cyc_Core_Opt**_tmp105;struct Cyc_Core_Opt**_tmp107;struct Cyc_Absyn_Kind*_tmp108;_LL9B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp104=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp103;if(_tmp104->tag != 1)goto _LL9D;else{_tmp105=(struct Cyc_Core_Opt**)& _tmp104->f1;}}_LL9C:
# 458
 if(td->defn != 0){
const char*_tmp337;void*_tmp336[1];struct Cyc_String_pa_PrintArg_struct _tmp335;(_tmp335.tag=0,((_tmp335.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp336[0]=& _tmp335,Cyc_Tcutil_warn(loc,((_tmp337="type variable %s is not used in typedef definition",_tag_dyneither(_tmp337,sizeof(char),51))),_tag_dyneither(_tmp336,sizeof(void*),1)))))));}
# 461
{struct Cyc_Core_Opt*_tmp338;*_tmp105=((_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp338))));}goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp106=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp103;if(_tmp106->tag != 2)goto _LL9F;else{_tmp107=(struct Cyc_Core_Opt**)& _tmp106->f1;_tmp108=_tmp106->f2;}}_LL9E:
# 468
{struct Cyc_Core_Opt*_tmp339;*_tmp107=((_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339->v=Cyc_Tcutil_kind_to_bound(_tmp108),_tmp339))));}
goto _LL9A;_LL9F:;_LLA0:
 continue;_LL9A:;}}
# 474
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 477
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 484
struct _RegionHandle _tmp10E=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp10E;_push_region(uprev_rgn);
# 486
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_stmttmp12=(struct _tuple10*)rpo->hd;void*_tmp110;void*_tmp111;struct _tuple10*_tmp10F=_stmttmp12;_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp110);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp111);}{
# 492
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 495
struct Cyc_List_List*_tmp112=fields;for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp13=(struct Cyc_Absyn_Aggrfield*)_tmp112->hd;struct _dyneither_ptr*_tmp114;struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Aggrfield*_tmp113=_stmttmp13;_tmp114=_tmp113->name;_tmp115=_tmp113->tq;_tmp116=_tmp113->type;_tmp117=_tmp113->width;_tmp118=_tmp113->attributes;_tmp119=_tmp113->requires_clause;
# 498
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp114)){
const char*_tmp33D;void*_tmp33C[1];struct Cyc_String_pa_PrintArg_struct _tmp33B;(_tmp33B.tag=0,((_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp114),((_tmp33C[0]=& _tmp33B,Cyc_Tcutil_terr(loc,((_tmp33D="duplicate member %s",_tag_dyneither(_tmp33D,sizeof(char),20))),_tag_dyneither(_tmp33C,sizeof(void*),1)))))));}
# 502
{const char*_tmp33E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp114,((_tmp33E="",_tag_dyneither(_tmp33E,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp33F;prev_fields=((_tmp33F=_region_malloc(uprev_rgn,sizeof(*_tmp33F)),((_tmp33F->hd=_tmp114,((_tmp33F->tl=prev_fields,_tmp33F))))));}}{
# 505
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 509
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp112->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp116);
# 514
(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp112->hd)->tq).print_const,_tmp116);
# 517
Cyc_Tcutil_check_bitfield(loc,te,_tmp116,_tmp117,_tmp114);
# 519
if((unsigned int)_tmp119){
if(str_or_union != Cyc_Absyn_UnionA){
const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_Tcutil_terr(loc,((_tmp342="@requires clauses are only allowed on union members",_tag_dyneither(_tmp342,sizeof(char),52))),_tag_dyneither(_tmp341,sizeof(void*),0)));}
{struct _RegionHandle _tmp121=_new_region("temp");struct _RegionHandle*temp=& _tmp121;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp122=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp122,0,_tmp119);}
# 523
;_pop_region(temp);}
# 526
if(!Cyc_Tcutil_is_integral(_tmp119)){
const char*_tmp346;void*_tmp345[1];struct Cyc_String_pa_PrintArg_struct _tmp344;(_tmp344.tag=0,((_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 529
Cyc_Absynpp_typ2string((void*)_check_null(_tmp119->topt))),((_tmp345[0]=& _tmp344,Cyc_Tcutil_terr(_tmp119->loc,((_tmp346="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp346,sizeof(char),54))),_tag_dyneither(_tmp345,sizeof(void*),1)))))));}
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp349;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp348;Cyc_Tcutil_check_type(_tmp119->loc,te,tvs,& Cyc_Tcutil_ik,0,(void*)((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=((_tmp349.tag=18,((_tmp349.f1=_tmp119,_tmp349)))),_tmp348)))));}{
# 532
struct Cyc_List_List*_tmp128=Cyc_Relations_exp2relns(uprev_rgn,_tmp119);
# 539
if(!Cyc_Relations_consistent_relations(_tmp128)){
const char*_tmp34C;void*_tmp34B;(_tmp34B=0,Cyc_Tcutil_terr(_tmp119->loc,((_tmp34C="@requires clause may be unsatisfiable",_tag_dyneither(_tmp34C,sizeof(char),38))),_tag_dyneither(_tmp34B,sizeof(void*),0)));}
# 545
{struct Cyc_List_List*_tmp12B=prev_relations;for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp128,(struct Cyc_List_List*)_tmp12B->hd))){
const char*_tmp34F;void*_tmp34E;(_tmp34E=0,Cyc_Tcutil_terr(_tmp119->loc,((_tmp34F="@requires clause may overlap with previous clauses",_tag_dyneither(_tmp34F,sizeof(char),51))),_tag_dyneither(_tmp34E,sizeof(void*),0)));}}}{
# 550
struct Cyc_List_List*_tmp350;prev_relations=((_tmp350=_region_malloc(uprev_rgn,sizeof(*_tmp350)),((_tmp350->hd=_tmp128,((_tmp350->tl=prev_relations,_tmp350))))));};};}else{
# 552
if(prev_relations != 0){
const char*_tmp353;void*_tmp352;(_tmp352=0,Cyc_Tcutil_terr(loc,((_tmp353="if one field has a @requires clause, they all must",_tag_dyneither(_tmp353,sizeof(char),51))),_tag_dyneither(_tmp352,sizeof(void*),0)));}}};}};
# 486
;_pop_region(uprev_rgn);}
# 559
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 561
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp14=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp131=_stmttmp14;struct Cyc_Core_Opt**_tmp133;struct Cyc_Core_Opt**_tmp135;enum Cyc_Absyn_AliasQual _tmp136;struct Cyc_Core_Opt**_tmp138;struct Cyc_Core_Opt**_tmp13A;struct Cyc_Absyn_Kind*_tmp13B;enum Cyc_Absyn_AliasQual _tmp13D;_LLA2: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp132=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp131;if(_tmp132->tag != 1)goto _LLA4;else{_tmp133=(struct Cyc_Core_Opt**)& _tmp132->f1;}}_LLA3:
# 564
{struct Cyc_Core_Opt*_tmp354;*_tmp133=((_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp354))));}continue;_LLA4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp134=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp131;if(_tmp134->tag != 2)goto _LLA6;else{_tmp135=(struct Cyc_Core_Opt**)& _tmp134->f1;if((_tmp134->f2)->kind != Cyc_Absyn_MemKind)goto _LLA6;_tmp136=(_tmp134->f2)->aliasqual;}}_LLA5:
# 566
 if(constrain_top_kind  && _tmp136 == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp357;struct Cyc_Core_Opt*_tmp356;*_tmp135=((_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356->v=Cyc_Tcutil_kind_to_bound(((_tmp357=_cycalloc_atomic(sizeof(*_tmp357)),((_tmp357->kind=Cyc_Absyn_BoxKind,((_tmp357->aliasqual=Cyc_Absyn_Aliasable,_tmp357))))))),_tmp356))));}else{
# 569
struct Cyc_Absyn_Kind*_tmp35A;struct Cyc_Core_Opt*_tmp359;*_tmp135=((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359->v=Cyc_Tcutil_kind_to_bound(((_tmp35A=_cycalloc_atomic(sizeof(*_tmp35A)),((_tmp35A->kind=Cyc_Absyn_BoxKind,((_tmp35A->aliasqual=_tmp136,_tmp35A))))))),_tmp359))));}
continue;_LLA6:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp137=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp131;if(_tmp137->tag != 2)goto _LLA8;else{_tmp138=(struct Cyc_Core_Opt**)& _tmp137->f1;if((_tmp137->f2)->kind != Cyc_Absyn_BoxKind)goto _LLA8;if((_tmp137->f2)->aliasqual != Cyc_Absyn_Top)goto _LLA8;}}if(!constrain_top_kind)goto _LLA8;_LLA7:
# 572
{struct Cyc_Absyn_Kind*_tmp35D;struct Cyc_Core_Opt*_tmp35C;*_tmp138=((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C->v=Cyc_Tcutil_kind_to_bound(((_tmp35D=_cycalloc_atomic(sizeof(*_tmp35D)),((_tmp35D->kind=Cyc_Absyn_BoxKind,((_tmp35D->aliasqual=Cyc_Absyn_Aliasable,_tmp35D))))))),_tmp35C))));}
continue;_LLA8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp139=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp131;if(_tmp139->tag != 2)goto _LLAA;else{_tmp13A=(struct Cyc_Core_Opt**)& _tmp139->f1;_tmp13B=_tmp139->f2;}}_LLA9:
# 575
{struct Cyc_Core_Opt*_tmp35E;*_tmp13A=((_tmp35E=_cycalloc(sizeof(*_tmp35E)),((_tmp35E->v=Cyc_Tcutil_kind_to_bound(_tmp13B),_tmp35E))));}continue;_LLAA: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp13C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp131;if(_tmp13C->tag != 0)goto _LLAC;else{if((_tmp13C->f1)->kind != Cyc_Absyn_MemKind)goto _LLAC;_tmp13D=(_tmp13C->f1)->aliasqual;}}_LLAB:
# 577
{const char*_tmp366;void*_tmp365[3];struct Cyc_String_pa_PrintArg_struct _tmp364;struct Cyc_String_pa_PrintArg_struct _tmp363;struct Cyc_Absyn_Kind*_tmp362;struct Cyc_String_pa_PrintArg_struct _tmp361;(_tmp361.tag=0,((_tmp361.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp362=_cycalloc_atomic(sizeof(*_tmp362)),((_tmp362->kind=Cyc_Absyn_MemKind,((_tmp362->aliasqual=_tmp13D,_tmp362)))))))),((_tmp363.tag=0,((_tmp363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp364.tag=0,((_tmp364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp365[0]=& _tmp364,((_tmp365[1]=& _tmp363,((_tmp365[2]=& _tmp361,Cyc_Tcutil_terr(loc,((_tmp366="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp366,sizeof(char),57))),_tag_dyneither(_tmp365,sizeof(void*),3)))))))))))))))))));}
continue;_LLAC:;_LLAD:
 continue;_LLA1:;}}struct _tuple14{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 585
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp14C=(*ad->name).f2;
# 592
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_stmttmp15=(void*)atts->hd;void*_tmp14D=_stmttmp15;_LLAF: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp14E=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp14D;if(_tmp14E->tag != 7)goto _LLB1;}_LLB0:
 goto _LLB2;_LLB1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp14F=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp14D;if(_tmp14F->tag != 6)goto _LLB3;}_LLB2:
 continue;_LLB3:;_LLB4:
# 597
{const char*_tmp36B;void*_tmp36A[2];struct Cyc_String_pa_PrintArg_struct _tmp369;struct Cyc_String_pa_PrintArg_struct _tmp368;(_tmp368.tag=0,((_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14C),((_tmp369.tag=0,((_tmp369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp36A[0]=& _tmp369,((_tmp36A[1]=& _tmp368,Cyc_Tcutil_terr(loc,((_tmp36B="bad attribute %s in  %s definition",_tag_dyneither(_tmp36B,sizeof(char),35))),_tag_dyneither(_tmp36A,sizeof(void*),2)))))))))))));}
goto _LLAE;_LLAE:;}}{
# 602
struct Cyc_List_List*_tmp154=ad->tvs;
# 604
{union Cyc_Absyn_Nmspace _stmttmp16=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp155=_stmttmp16;_LLB6: if((_tmp155.Rel_n).tag != 1)goto _LLB8;if((struct Cyc_List_List*)(_tmp155.Rel_n).val != 0)goto _LLB8;_LLB7:
# 607
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLB5;_LLB8:;_LLB9:
# 610
 goto _LLB5;_LLB5:;}
# 614
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 616
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 620
{struct _tuple14 _tmp36C;struct _tuple14 _stmttmp17=(_tmp36C.f1=ad->impl,((_tmp36C.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp14C),_tmp36C)));struct _tuple14 _tmp156=_stmttmp17;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp158;struct Cyc_List_List*_tmp159;int _tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_List_List*_tmp15D;int _tmp15E;struct Cyc_Absyn_Aggrdecl**_tmp15F;struct Cyc_Absyn_Aggrdecl**_tmp160;_LLBB: if(_tmp156.f1 != 0)goto _LLBD;if(_tmp156.f2 != 0)goto _LLBD;_LLBC:
# 623
 Cyc_Tc_rule_out_memkind(loc,_tmp14C,_tmp154,0);
{struct Cyc_Absyn_Aggrdecl**_tmp36D;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp14C,((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=ad,_tmp36D)))));}
goto _LLBA;_LLBD: if(_tmp156.f1 == 0)goto _LLBF;_tmp157=(_tmp156.f1)->exist_vars;_tmp158=(_tmp156.f1)->rgn_po;_tmp159=(_tmp156.f1)->fields;_tmp15A=(_tmp156.f1)->tagged;if(_tmp156.f2 != 0)goto _LLBF;_LLBE: {
# 629
struct Cyc_Absyn_Aggrdecl*_tmp370;struct Cyc_Absyn_Aggrdecl**_tmp36F;struct Cyc_Absyn_Aggrdecl**_tmp162=(_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F[0]=((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->kind=ad->kind,((_tmp370->sc=Cyc_Absyn_Extern,((_tmp370->name=ad->name,((_tmp370->tvs=_tmp154,((_tmp370->impl=0,((_tmp370->attributes=ad->attributes,_tmp370)))))))))))))),_tmp36F)));
# 631
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp14C,_tmp162);
# 634
Cyc_Tcutil_check_unique_tvars(loc,_tmp157);
# 636
Cyc_Tcutil_add_tvar_identities(_tmp157);
# 639
if(_tmp15A  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp373;void*_tmp372;(_tmp372=0,Cyc_Tcutil_terr(loc,((_tmp373="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp373,sizeof(char),56))),_tag_dyneither(_tmp372,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp154,_tmp157),_tmp158,_tmp159);
# 643
Cyc_Tc_rule_out_memkind(loc,_tmp14C,_tmp154,0);
# 646
Cyc_Tc_rule_out_memkind(loc,_tmp14C,_tmp157,1);
# 648
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp15A){
# 651
struct Cyc_List_List*f=_tmp159;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0){
const char*_tmp379;void*_tmp378[3];struct Cyc_String_pa_PrintArg_struct _tmp377;struct Cyc_String_pa_PrintArg_struct _tmp376;struct Cyc_String_pa_PrintArg_struct _tmp375;(_tmp375.tag=0,((_tmp375.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp376.tag=0,((_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14C),((_tmp377.tag=0,((_tmp377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp378[0]=& _tmp377,((_tmp378[1]=& _tmp376,((_tmp378[2]=& _tmp375,Cyc_Tcutil_warn(loc,((_tmp379="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp379,sizeof(char),98))),_tag_dyneither(_tmp378,sizeof(void*),3)))))))))))))))))));}}}
# 656
*_tmp162=ad;
goto _LLBA;}_LLBF: if(_tmp156.f1 == 0)goto _LLC1;_tmp15B=(_tmp156.f1)->exist_vars;_tmp15C=(_tmp156.f1)->rgn_po;_tmp15D=(_tmp156.f1)->fields;_tmp15E=(_tmp156.f1)->tagged;if(_tmp156.f2 == 0)goto _LLC1;_tmp15F=*_tmp156.f2;_LLC0:
# 660
 if(ad->kind != (*_tmp15F)->kind){
const char*_tmp37C;void*_tmp37B;(_tmp37B=0,Cyc_Tcutil_terr(loc,((_tmp37C="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp37C,sizeof(char),52))),_tag_dyneither(_tmp37B,sizeof(void*),0)));}{
# 663
struct Cyc_Absyn_Aggrdecl*_tmp16E=*_tmp15F;
# 665
{struct Cyc_Absyn_Aggrdecl*_tmp37D;*_tmp15F=((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D->kind=ad->kind,((_tmp37D->sc=Cyc_Absyn_Extern,((_tmp37D->name=ad->name,((_tmp37D->tvs=_tmp154,((_tmp37D->impl=0,((_tmp37D->attributes=ad->attributes,_tmp37D))))))))))))));}
# 669
Cyc_Tcutil_check_unique_tvars(loc,_tmp15B);
# 671
Cyc_Tcutil_add_tvar_identities(_tmp15B);
# 673
if(_tmp15E  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp380;void*_tmp37F;(_tmp37F=0,Cyc_Tcutil_terr(loc,((_tmp380="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp380,sizeof(char),56))),_tag_dyneither(_tmp37F,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp154,_tmp15B),_tmp15C,_tmp15D);
# 677
*_tmp15F=_tmp16E;
_tmp160=_tmp15F;goto _LLC2;};_LLC1: if(_tmp156.f1 != 0)goto _LLBA;if(_tmp156.f2 == 0)goto _LLBA;_tmp160=*_tmp156.f2;_LLC2: {
# 681
struct Cyc_Absyn_Aggrdecl*_tmp172=Cyc_Tcdecl_merge_aggrdecl(*_tmp160,ad,loc,Cyc_Tc_tc_msg);
if(_tmp172 == 0)
return;else{
# 685
Cyc_Tc_rule_out_memkind(loc,_tmp14C,_tmp154,0);
# 688
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp14C,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 691
*_tmp160=_tmp172;
ad=_tmp172;
goto _LLBA;}}_LLBA:;}{
# 697
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp386;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp385;struct _tuple13*_tmp384;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,_tmp14C,(
(_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->f1=(void*)((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386[0]=((_tmp385.tag=1,((_tmp385.f1=ad,_tmp385)))),_tmp386)))),((_tmp384->f2=1,_tmp384)))))));};};}
# 701
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_stmttmp18=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp177=_stmttmp18;struct Cyc_Core_Opt**_tmp179;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Core_Opt**_tmp17D;struct Cyc_Core_Opt**_tmp17F;struct Cyc_Core_Opt**_tmp181;struct Cyc_Core_Opt**_tmp183;struct Cyc_Core_Opt**_tmp186;enum Cyc_Absyn_KindQual _tmp187;enum Cyc_Absyn_KindQual _tmp189;enum Cyc_Absyn_AliasQual _tmp18B;_LLC4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp178=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp178->tag != 1)goto _LLC6;else{_tmp179=(struct Cyc_Core_Opt**)& _tmp178->f1;}}_LLC5:
 _tmp17B=_tmp179;goto _LLC7;_LLC6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp17A->tag != 2)goto _LLC8;else{_tmp17B=(struct Cyc_Core_Opt**)& _tmp17A->f1;if((_tmp17A->f2)->kind != Cyc_Absyn_MemKind)goto _LLC8;if((_tmp17A->f2)->aliasqual != Cyc_Absyn_Top)goto _LLC8;}}_LLC7:
# 706
 _tmp17D=_tmp17B;goto _LLC9;_LLC8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp17C->tag != 2)goto _LLCA;else{_tmp17D=(struct Cyc_Core_Opt**)& _tmp17C->f1;if((_tmp17C->f2)->kind != Cyc_Absyn_MemKind)goto _LLCA;if((_tmp17C->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLCA;}}_LLC9:
# 708
{struct Cyc_Core_Opt*_tmp387;*_tmp17D=((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp387))));}goto _LLC3;_LLCA: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp17E->tag != 2)goto _LLCC;else{_tmp17F=(struct Cyc_Core_Opt**)& _tmp17E->f1;if((_tmp17E->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCC;if((_tmp17E->f2)->aliasqual != Cyc_Absyn_Top)goto _LLCC;}}_LLCB:
 _tmp181=_tmp17F;goto _LLCD;_LLCC: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp180=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp180->tag != 2)goto _LLCE;else{_tmp181=(struct Cyc_Core_Opt**)& _tmp180->f1;if((_tmp180->f2)->kind != Cyc_Absyn_AnyKind)goto _LLCE;if((_tmp180->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLCE;}}_LLCD:
# 711
{struct Cyc_Core_Opt*_tmp388;*_tmp181=((_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp388))));}goto _LLC3;_LLCE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp182=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp182->tag != 2)goto _LLD0;else{_tmp183=(struct Cyc_Core_Opt**)& _tmp182->f1;if((_tmp182->f2)->kind != Cyc_Absyn_RgnKind)goto _LLD0;if((_tmp182->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD0;}}_LLCF:
# 713
{struct Cyc_Core_Opt*_tmp389;*_tmp183=((_tmp389=_cycalloc(sizeof(*_tmp389)),((_tmp389->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp389))));}goto _LLC3;_LLD0: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp184=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp184->tag != 0)goto _LLD2;else{if((_tmp184->f1)->kind != Cyc_Absyn_RgnKind)goto _LLD2;if((_tmp184->f1)->aliasqual != Cyc_Absyn_Top)goto _LLD2;}}_LLD1:
# 715
{const char*_tmp38E;void*_tmp38D[2];struct Cyc_String_pa_PrintArg_struct _tmp38C;struct Cyc_String_pa_PrintArg_struct _tmp38B;(_tmp38B.tag=0,((_tmp38B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp38C.tag=0,((_tmp38C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp38D[0]=& _tmp38C,((_tmp38D[1]=& _tmp38B,Cyc_Tcutil_terr(loc,((_tmp38E="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp38E,sizeof(char),57))),_tag_dyneither(_tmp38D,sizeof(void*),2)))))))))))));}
goto _LLC3;_LLD2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp185=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp185->tag != 2)goto _LLD4;else{_tmp186=(struct Cyc_Core_Opt**)& _tmp185->f1;_tmp187=(_tmp185->f2)->kind;if((_tmp185->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLD4;}}_LLD3:
 _tmp189=_tmp187;goto _LLD5;_LLD4: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp188=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp188->tag != 0)goto _LLD6;else{_tmp189=(_tmp188->f1)->kind;if((_tmp188->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLD6;}}_LLD5:
# 719
{const char*_tmp396;void*_tmp395[3];struct Cyc_String_pa_PrintArg_struct _tmp394;struct Cyc_String_pa_PrintArg_struct _tmp393;struct Cyc_Absyn_Kind*_tmp392;struct Cyc_String_pa_PrintArg_struct _tmp391;(_tmp391.tag=0,((_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 721
Cyc_Absynpp_kind2string(((_tmp392=_cycalloc_atomic(sizeof(*_tmp392)),((_tmp392->kind=_tmp189,((_tmp392->aliasqual=Cyc_Absyn_Unique,_tmp392)))))))),((_tmp393.tag=0,((_tmp393.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp394.tag=0,((_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp395[0]=& _tmp394,((_tmp395[1]=& _tmp393,((_tmp395[2]=& _tmp391,Cyc_Tcutil_terr(loc,((_tmp396="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp396,sizeof(char),57))),_tag_dyneither(_tmp395,sizeof(void*),3)))))))))))))))))));}goto _LLC3;_LLD6: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp18A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp177;if(_tmp18A->tag != 0)goto _LLD8;else{if((_tmp18A->f1)->kind != Cyc_Absyn_MemKind)goto _LLD8;_tmp18B=(_tmp18A->f1)->aliasqual;}}_LLD7:
# 723
{const char*_tmp39E;void*_tmp39D[3];struct Cyc_String_pa_PrintArg_struct _tmp39C;struct Cyc_String_pa_PrintArg_struct _tmp39B;struct Cyc_Absyn_Kind*_tmp39A;struct Cyc_String_pa_PrintArg_struct _tmp399;(_tmp399.tag=0,((_tmp399.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 725
Cyc_Absynpp_kind2string(((_tmp39A=_cycalloc_atomic(sizeof(*_tmp39A)),((_tmp39A->kind=Cyc_Absyn_MemKind,((_tmp39A->aliasqual=_tmp18B,_tmp39A)))))))),((_tmp39B.tag=0,((_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp39C.tag=0,((_tmp39C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp39D[0]=& _tmp39C,((_tmp39D[1]=& _tmp39B,((_tmp39D[2]=& _tmp399,Cyc_Tcutil_terr(loc,((_tmp39E="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp39E,sizeof(char),57))),_tag_dyneither(_tmp39D,sizeof(void*),3)))))))))))))))))));}goto _LLC3;_LLD8:;_LLD9:
 goto _LLC3;_LLC3:;}}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 732
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 741
{struct Cyc_List_List*_tmp19F=fields;for(0;_tmp19F != 0;_tmp19F=_tmp19F->tl){
struct Cyc_Absyn_Datatypefield*_tmp1A0=(struct Cyc_Absyn_Datatypefield*)_tmp19F->hd;
# 744
{struct Cyc_List_List*typs=_tmp1A0->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp1A0->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple15*)typs->hd)).f2);{
# 747
struct _RegionHandle _tmp1A1=_new_region("temp");struct _RegionHandle*temp=& _tmp1A1;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple15*)typs->hd)).f2)){
const char*_tmp3A2;void*_tmp3A1[1];struct Cyc_String_pa_PrintArg_struct _tmp3A0;(_tmp3A0.tag=0,((_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 751
Cyc_Absynpp_qvar2string(_tmp1A0->name)),((_tmp3A1[0]=& _tmp3A0,Cyc_Tcutil_terr(_tmp1A0->loc,((_tmp3A2="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3A2,sizeof(char),51))),_tag_dyneither(_tmp3A1,sizeof(void*),1)))))));}
# 753
((*((struct _tuple15*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp1A0->loc,((*((struct _tuple15*)typs->hd)).f1).print_const,(*((struct _tuple15*)typs->hd)).f2);
# 747
;_pop_region(temp);};}}{
# 758
union Cyc_Absyn_Nmspace _stmttmp19=(*_tmp1A0->name).f1;union Cyc_Absyn_Nmspace _tmp1A5=_stmttmp19;_LLDB: if((_tmp1A5.Rel_n).tag != 1)goto _LLDD;if((struct Cyc_List_List*)(_tmp1A5.Rel_n).val != 0)goto _LLDD;_LLDC:
# 760
 if(is_extensible)
(*_tmp1A0->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 763
(*_tmp1A0->name).f1=(*name).f1;}
goto _LLDA;_LLDD: if((_tmp1A5.Rel_n).tag != 1)goto _LLDF;_LLDE:
# 766
{const char*_tmp3A6;void*_tmp3A5[1];struct Cyc_String_pa_PrintArg_struct _tmp3A4;(_tmp3A4.tag=0,((_tmp3A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1A0->name)),((_tmp3A5[0]=& _tmp3A4,Cyc_Tcutil_terr(_tmp1A0->loc,((_tmp3A6="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3A6,sizeof(char),58))),_tag_dyneither(_tmp3A5,sizeof(void*),1)))))));}
goto _LLDA;_LLDF: if((_tmp1A5.C_n).tag != 3)goto _LLE1;_LLE0:
 goto _LLDA;_LLE1: if((_tmp1A5.Abs_n).tag != 2)goto _LLE3;_LLE2:
 goto _LLDA;_LLE3: if((_tmp1A5.Loc_n).tag != 4)goto _LLDA;_LLE4: {
const char*_tmp3A9;void*_tmp3A8;(_tmp3A8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3A9="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3A9,sizeof(char),24))),_tag_dyneither(_tmp3A8,sizeof(void*),0)));}_LLDA:;};}}{
# 775
struct Cyc_List_List*fields2;
if(is_extensible){
# 778
int _tmp1AB=1;
struct Cyc_List_List*_tmp1AC=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1AB,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1AB)
fields2=_tmp1AC;else{
# 783
fields2=0;}}else{
# 785
struct _RegionHandle _tmp1AD=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1AD;_push_region(uprev_rgn);
# 787
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1AE=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1AE->name).f2)){
const char*_tmp3AE;void*_tmp3AD[2];struct Cyc_String_pa_PrintArg_struct _tmp3AC;struct Cyc_String_pa_PrintArg_struct _tmp3AB;(_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3AC.tag=0,((_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1AE->name).f2),((_tmp3AD[0]=& _tmp3AC,((_tmp3AD[1]=& _tmp3AB,Cyc_Tcutil_terr(_tmp1AE->loc,((_tmp3AE="duplicate field name %s in %s",_tag_dyneither(_tmp3AE,sizeof(char),30))),_tag_dyneither(_tmp3AD,sizeof(void*),2)))))))))))));}else{
# 793
struct Cyc_List_List*_tmp3AF;prev_fields=((_tmp3AF=_region_malloc(uprev_rgn,sizeof(*_tmp3AF)),((_tmp3AF->hd=(*_tmp1AE->name).f2,((_tmp3AF->tl=prev_fields,_tmp3AF))))));}
# 795
if(_tmp1AE->sc != Cyc_Absyn_Public){
{const char*_tmp3B3;void*_tmp3B2[1];struct Cyc_String_pa_PrintArg_struct _tmp3B1;(_tmp3B1.tag=0,((_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1AE->name).f2),((_tmp3B2[0]=& _tmp3B1,Cyc_Tcutil_warn(loc,((_tmp3B3="ignoring scope of field %s",_tag_dyneither(_tmp3B3,sizeof(char),27))),_tag_dyneither(_tmp3B2,sizeof(void*),1)))))));}
_tmp1AE->sc=Cyc_Absyn_Public;}}}
# 800
fields2=fields;}
# 787
;_pop_region(uprev_rgn);}
# 806
{struct Cyc_List_List*_tmp1B7=fields;for(0;_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){
struct Cyc_Absyn_Datatypefield*_tmp1B8=(struct Cyc_Absyn_Datatypefield*)_tmp1B7->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3B9;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3B8;struct _tuple13*_tmp3B7;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1B8->name).f2,(
(_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->f1=(void*)((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3B8.tag=2,((_tmp3B8.f1=tudres,((_tmp3B8.f2=_tmp1B8,_tmp3B8)))))),_tmp3B9)))),((_tmp3B7->f2=1,_tmp3B7)))))));}}
# 812
return fields2;};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 815
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp3BB;const char*_tmp3BA;struct _dyneither_ptr obj=tud->is_extensible?(_tmp3BB="@extensible datatype",_tag_dyneither(_tmp3BB,sizeof(char),21)):((_tmp3BA="datatype",_tag_dyneither(_tmp3BA,sizeof(char),9)));
# 822
struct Cyc_List_List*tvs=tud->tvs;
# 825
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 827
Cyc_Tcutil_add_tvar_identities(tvs);{
# 832
struct _RegionHandle _tmp1BC=_new_region("temp");struct _RegionHandle*temp=& _tmp1BC;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 842 "tc.cyc"
{struct _handler_cons _tmp1BD;_push_handler(& _tmp1BD);{int _tmp1BF=0;if(setjmp(_tmp1BD.handler))_tmp1BF=1;if(!_tmp1BF){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp3BE;struct Cyc_Dict_Absent_exn_struct*_tmp3BD;(int)_throw((void*)((_tmp3BD=_cycalloc_atomic(sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BE.tag=Cyc_Dict_Absent,_tmp3BE)),_tmp3BD)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 848
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 843
;_pop_handler();}else{void*_tmp1BE=(void*)_exn_thrown;void*_tmp1C3=_tmp1BE;void*_tmp1C5;_LLE6: {struct Cyc_Dict_Absent_exn_struct*_tmp1C4=(struct Cyc_Dict_Absent_exn_struct*)_tmp1C3;if(_tmp1C4->tag != Cyc_Dict_Absent)goto _LLE8;}_LLE7:
# 852
{union Cyc_Absyn_Nmspace _stmttmp1A=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp1C6=_stmttmp1A;_LLEB: if((_tmp1C6.Rel_n).tag != 1)goto _LLED;if((struct Cyc_List_List*)(_tmp1C6.Rel_n).val != 0)goto _LLED;_LLEC:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLEA;_LLED:;_LLEE:
 goto _LLEA;_LLEA:;}{
# 856
struct Cyc_Absyn_Datatypedecl***_tmp1C7=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp3BF;tud_opt=(unsigned int)_tmp1C7?(_tmp3BF=_region_malloc(temp,sizeof(*_tmp3BF)),((_tmp3BF[0]=*_tmp1C7,_tmp3BF))): 0;}
goto _LLE5;};_LLE8: _tmp1C5=_tmp1C3;_LLE9:(void)_throw(_tmp1C5);_LLE5:;}};}{
# 863
struct _tuple16 _tmp3C0;struct _tuple16 _stmttmp1B=(_tmp3C0.f1=tud->fields,((_tmp3C0.f2=tud_opt,_tmp3C0)));struct _tuple16 _tmp1C9=_stmttmp1B;struct Cyc_List_List**_tmp1CA;struct Cyc_List_List**_tmp1CB;struct Cyc_Absyn_Datatypedecl**_tmp1CC;struct Cyc_Absyn_Datatypedecl**_tmp1CD;_LLF0: if(_tmp1C9.f1 != 0)goto _LLF2;if(_tmp1C9.f2 != 0)goto _LLF2;_LLF1:
# 866
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp3C1;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=tud,_tmp3C1)))));}
goto _LLEF;_LLF2: if(_tmp1C9.f1 == 0)goto _LLF4;_tmp1CA=(struct Cyc_List_List**)&(_tmp1C9.f1)->v;if(_tmp1C9.f2 != 0)goto _LLF4;_LLF3: {
# 871
struct Cyc_Absyn_Datatypedecl*_tmp3C4;struct Cyc_Absyn_Datatypedecl**_tmp3C3;struct Cyc_Absyn_Datatypedecl**_tmp1CF=(_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->sc=Cyc_Absyn_Extern,((_tmp3C4->name=tud->name,((_tmp3C4->tvs=tvs,((_tmp3C4->fields=0,((_tmp3C4->is_extensible=tud->is_extensible,_tmp3C4)))))))))))),_tmp3C3)));
# 873
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp1CF);
# 876
*_tmp1CA=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1CA,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp1CF=tud;
goto _LLEF;}_LLF4: if(_tmp1C9.f1 == 0)goto _LLF6;_tmp1CB=(struct Cyc_List_List**)&(_tmp1C9.f1)->v;if(_tmp1C9.f2 == 0)goto _LLF6;_tmp1CC=*_tmp1C9.f2;_LLF5: {
# 881
struct Cyc_Absyn_Datatypedecl*_tmp1D2=*_tmp1CC;
# 884
if((!tud->is_extensible  && (unsigned int)_tmp1D2) && _tmp1D2->is_extensible)
tud->is_extensible=1;
# 887
{struct Cyc_Absyn_Datatypedecl*_tmp3C5;*_tmp1CC=((_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5->sc=Cyc_Absyn_Extern,((_tmp3C5->name=tud->name,((_tmp3C5->tvs=tvs,((_tmp3C5->fields=0,((_tmp3C5->is_extensible=tud->is_extensible,_tmp3C5))))))))))));}
# 891
*_tmp1CB=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp1CB,tvs,tud);
# 895
*_tmp1CC=_tmp1D2;
_tmp1CD=_tmp1CC;goto _LLF7;}_LLF6: if(_tmp1C9.f1 != 0)goto _LLEF;if(_tmp1C9.f2 == 0)goto _LLEF;_tmp1CD=*_tmp1C9.f2;_LLF7: {
# 899
struct Cyc_Absyn_Datatypedecl*_tmp1D4=Cyc_Tcdecl_merge_datatypedecl(*_tmp1CD,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp1D4 == 0){
_npop_handler(0);return;}else{
# 904
*_tmp1CD=_tmp1D4;
goto _LLEF;}}_LLEF:;};}
# 833 "tc.cyc"
;_pop_region(temp);};}
# 911 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 918
{union Cyc_Absyn_Nmspace _stmttmp1C=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp1D8=_stmttmp1C;_LLF9: if((_tmp1D8.Rel_n).tag != 1)goto _LLFB;if((struct Cyc_List_List*)(_tmp1D8.Rel_n).val != 0)goto _LLFB;_LLFA:
# 921
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LLF8;_LLFB:;_LLFC:
 goto _LLF8;_LLF8:;}
# 927
if(ed->fields != 0){struct _RegionHandle _tmp1D9=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1D9;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1DA=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 933
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1DA->name).f2)){
const char*_tmp3C9;void*_tmp3C8[1];struct Cyc_String_pa_PrintArg_struct _tmp3C7;(_tmp3C7.tag=0,((_tmp3C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1DA->name).f2),((_tmp3C8[0]=& _tmp3C7,Cyc_Tcutil_terr(_tmp1DA->loc,((_tmp3C9="duplicate field name %s",_tag_dyneither(_tmp3C9,sizeof(char),24))),_tag_dyneither(_tmp3C8,sizeof(void*),1)))))));}else{
# 936
struct Cyc_List_List*_tmp3CA;prev_fields=((_tmp3CA=_region_malloc(uprev_rgn,sizeof(*_tmp3CA)),((_tmp3CA->hd=(*_tmp1DA->name).f2,((_tmp3CA->tl=prev_fields,_tmp3CA))))));}
# 938
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp1DA->name).f2)!= 0){
const char*_tmp3CE;void*_tmp3CD[1];struct Cyc_String_pa_PrintArg_struct _tmp3CC;(_tmp3CC.tag=0,((_tmp3CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1DA->name).f2),((_tmp3CD[0]=& _tmp3CC,Cyc_Tcutil_terr(_tmp1DA->loc,((_tmp3CE="enum field name %s shadows global name",_tag_dyneither(_tmp3CE,sizeof(char),39))),_tag_dyneither(_tmp3CD,sizeof(void*),1)))))));}
# 941
if(_tmp1DA->tag == 0){
_tmp1DA->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1DA->loc);
++ tag_count;}else{
# 946
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1DA->tag))){
struct _tuple11 _stmttmp1D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1DA->tag));unsigned int _tmp1E3;int _tmp1E4;struct _tuple11 _tmp1E2=_stmttmp1D;_tmp1E3=_tmp1E2.f1;_tmp1E4=_tmp1E2.f2;
if(_tmp1E4)tag_count=_tmp1E3 + 1;}}
# 950
(*_tmp1DA->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 928
;_pop_region(uprev_rgn);}
# 955
{struct _handler_cons _tmp1E5;_push_handler(& _tmp1E5);{int _tmp1E7=0;if(setjmp(_tmp1E5.handler))_tmp1E7=1;if(!_tmp1E7){
{struct Cyc_Absyn_Enumdecl**_tmp1E8=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp1E9=Cyc_Tcdecl_merge_enumdecl(*_tmp1E8,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1E9 == 0){_npop_handler(0);return;}
*_tmp1E8=_tmp1E9;}
# 956
;_pop_handler();}else{void*_tmp1E6=(void*)_exn_thrown;void*_tmp1EB=_tmp1E6;void*_tmp1ED;_LLFE: {struct Cyc_Dict_Absent_exn_struct*_tmp1EC=(struct Cyc_Dict_Absent_exn_struct*)_tmp1EB;if(_tmp1EC->tag != Cyc_Dict_Absent)goto _LL100;}_LLFF: {
# 961
struct Cyc_Absyn_Enumdecl**_tmp3CF;struct Cyc_Absyn_Enumdecl**_tmp1EE=(_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF[0]=ed,_tmp3CF)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp1EE);
goto _LLFD;}_LL100: _tmp1ED=_tmp1EB;_LL101:(void)_throw(_tmp1ED);_LLFD:;}};}
# 965
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1F0=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3D5;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp3D4;struct _tuple13*_tmp3D3;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple13*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1F0->name).f2,(
(_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->f1=(void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=((_tmp3D4.tag=3,((_tmp3D4.f1=ed,((_tmp3D4.f2=_tmp1F0,_tmp3D4)))))),_tmp3D5)))),((_tmp3D3->f2=1,_tmp3D3)))))));}}
# 974
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1F4=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1F4->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1F4->tag))){
const char*_tmp3DA;void*_tmp3D9[2];struct Cyc_String_pa_PrintArg_struct _tmp3D8;struct Cyc_String_pa_PrintArg_struct _tmp3D7;(_tmp3D7.tag=0,((_tmp3D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F4->name).f2),((_tmp3D8.tag=0,((_tmp3D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D9[0]=& _tmp3D8,((_tmp3D9[1]=& _tmp3D7,Cyc_Tcutil_terr(loc,((_tmp3DA="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp3DA,sizeof(char),46))),_tag_dyneither(_tmp3D9,sizeof(void*),2)))))))))))));}}}}
# 984
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL102:
# 987
 if(!in_include){
const char*_tmp3DD;void*_tmp3DC;(_tmp3DC=0,Cyc_Tcutil_warn(loc,((_tmp3DD="static declaration nested within extern \"C\"",_tag_dyneither(_tmp3DD,sizeof(char),44))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL103:
# 991
{const char*_tmp3E0;void*_tmp3DF;(_tmp3DF=0,Cyc_Tcutil_warn(loc,((_tmp3E0="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp3E0,sizeof(char),46))),_tag_dyneither(_tmp3DF,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL104:
 goto _LL105;case Cyc_Absyn_Register: _LL105:
 goto _LL106;case Cyc_Absyn_Extern: _LL106:
 return 1;case Cyc_Absyn_ExternC: _LL107:
# 997
{const char*_tmp3E3;void*_tmp3E2;(_tmp3E2=0,Cyc_Tcutil_warn(loc,((_tmp3E3="nested extern \"C\" declaration",_tag_dyneither(_tmp3E3,sizeof(char),30))),_tag_dyneither(_tmp3E2,sizeof(void*),0)));}
return 1;}}
# 1002
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _stmttmp1E=*exp;unsigned int _tmp200;struct _tuple0*_tmp201;struct _tuple12 _tmp1FF=_stmttmp1E;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;{
union Cyc_Absyn_Nmspace _tmp203;struct _dyneither_ptr*_tmp204;struct _tuple0*_tmp202=_tmp201;_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;{
union Cyc_Absyn_Nmspace _tmp205=_tmp203;_LL10A: if((_tmp205.Rel_n).tag != 1)goto _LL10C;if((struct Cyc_List_List*)(_tmp205.Rel_n).val != 0)goto _LL10C;_LL10B:
# 1008
(*_tmp201).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL109;_LL10C:;_LL10D:
 goto _LL109;_LL109:;};};}
# 1019
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1026
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1029
struct Cyc_List_List*_tmp206=ds0;for(0;_tmp206 != 0;(last=_tmp206,_tmp206=_tmp206->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp206->hd;
unsigned int loc=d->loc;
# 1033
void*_stmttmp1F=d->r;void*_tmp207=_stmttmp1F;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Fndecl*_tmp20F;struct Cyc_Absyn_Typedefdecl*_tmp211;struct Cyc_Absyn_Aggrdecl*_tmp213;struct Cyc_Absyn_Datatypedecl*_tmp215;struct Cyc_Absyn_Enumdecl*_tmp217;struct _dyneither_ptr*_tmp21B;struct Cyc_List_List*_tmp21C;union Cyc_Absyn_Nmspace _tmp21E;struct _dyneither_ptr*_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmp222;struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp225;_LL10F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp208=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp208->tag != 2)goto _LL111;}_LL110:
 goto _LL112;_LL111: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp209=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp209->tag != 3)goto _LL113;}_LL112:
# 1036
{const char*_tmp3E6;void*_tmp3E5;(_tmp3E5=0,Cyc_Tcutil_terr(loc,((_tmp3E6="top level let-declarations are not implemented",_tag_dyneither(_tmp3E6,sizeof(char),47))),_tag_dyneither(_tmp3E5,sizeof(void*),0)));}
goto _LL10E;_LL113: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp20A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp20A->tag != 4)goto _LL115;}_LL114:
# 1039
{const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,Cyc_Tcutil_terr(loc,((_tmp3E9="top level region declarations are not implemented",_tag_dyneither(_tmp3E9,sizeof(char),50))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}
goto _LL10E;_LL115: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp20B=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp20B->tag != 5)goto _LL117;}_LL116:
# 1042
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_terr(loc,((_tmp3EC="top level alias declarations are not implemented",_tag_dyneither(_tmp3EC,sizeof(char),49))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}
goto _LL10E;_LL117: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp20C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp20C->tag != 0)goto _LL119;else{_tmp20D=_tmp20C->f1;}}_LL118:
# 1045
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp20D->sc,in_externCinclude))
_tmp20D->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp20D,check_var_init,in_externCinclude,exports);
goto _LL10E;_LL119: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp20E=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp20E->tag != 1)goto _LL11B;else{_tmp20F=_tmp20E->f1;}}_LL11A:
# 1050
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp20F->sc,in_externCinclude))
_tmp20F->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp20F,in_externCinclude,exports);
goto _LL10E;_LL11B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp210=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp210->tag != 9)goto _LL11D;else{_tmp211=_tmp210->f1;}}_LL11C:
# 1055
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp211);
goto _LL10E;_LL11D: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp212=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp212->tag != 6)goto _LL11F;else{_tmp213=_tmp212->f1;}}_LL11E:
# 1058
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp213->sc,in_externCinclude))
_tmp213->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp213);
goto _LL10E;_LL11F: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp214=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp214->tag != 7)goto _LL121;else{_tmp215=_tmp214->f1;}}_LL120:
# 1063
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp215->sc,in_externCinclude))
_tmp215->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp215);
goto _LL10E;_LL121: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp216=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp216->tag != 8)goto _LL123;else{_tmp217=_tmp216->f1;}}_LL122:
# 1068
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp217->sc,in_externCinclude))
_tmp217->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp217);
goto _LL10E;_LL123: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp218=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp218->tag != 14)goto _LL125;}_LL124:
# 1073
{const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_Tcutil_warn(d->loc,((_tmp3EF="spurious __cyclone_port_on__",_tag_dyneither(_tmp3EF,sizeof(char),29))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}
goto _LL10E;_LL125: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp219=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp219->tag != 15)goto _LL127;}_LL126:
# 1076
 goto _LL10E;_LL127: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp21A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp21A->tag != 10)goto _LL129;else{_tmp21B=_tmp21A->f1;_tmp21C=_tmp21A->f2;}}_LL128: {
# 1079
struct Cyc_List_List*_tmp22E=te->ns;
struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*_tmp22F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp22E,((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=_tmp21B,((_tmp3F0->tl=0,_tmp3F0)))))));
# 1083
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp21B)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp21B);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp22F,Cyc_Tcenv_empty_genv(grgn));}
# 1088
te->ns=_tmp22F;
Cyc_Tc_tc_decls(te,_tmp21C,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp22E;
goto _LL10E;}_LL129: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp21D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp21D->tag != 11)goto _LL12B;else{_tmp21E=(_tmp21D->f1)->f1;_tmp21F=(_tmp21D->f1)->f2;_tmp220=_tmp21D->f2;}}_LL12A: {
# 1095
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp231=_tmp21E;struct _dyneither_ptr*_tmp232;struct Cyc_List_List*_tmp233;struct _dyneither_ptr*_tmp234;struct Cyc_List_List*_tmp235;struct _dyneither_ptr*_tmp236;struct Cyc_List_List*_tmp237;_LL130: if((_tmp231.Loc_n).tag != 4)goto _LL132;_LL131:
 goto _LL133;_LL132: if((_tmp231.Rel_n).tag != 1)goto _LL134;if((struct Cyc_List_List*)(_tmp231.Rel_n).val != 0)goto _LL134;_LL133:
 goto _LL135;_LL134: if((_tmp231.C_n).tag != 3)goto _LL136;if((struct Cyc_List_List*)(_tmp231.C_n).val != 0)goto _LL136;_LL135:
 goto _LL137;_LL136: if((_tmp231.Abs_n).tag != 2)goto _LL138;if((struct Cyc_List_List*)(_tmp231.Abs_n).val != 0)goto _LL138;_LL137:
# 1102
 first=_tmp21F;rest=0;goto _LL12F;_LL138: if((_tmp231.Rel_n).tag != 1)goto _LL13A;if((struct Cyc_List_List*)(_tmp231.Rel_n).val == 0)goto _LL13A;_tmp232=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp231.Rel_n).val)->hd;_tmp233=((struct Cyc_List_List*)(_tmp231.Rel_n).val)->tl;_LL139:
 _tmp234=_tmp232;_tmp235=_tmp233;goto _LL13B;_LL13A: if((_tmp231.C_n).tag != 3)goto _LL13C;if((struct Cyc_List_List*)(_tmp231.C_n).val == 0)goto _LL13C;_tmp234=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp231.C_n).val)->hd;_tmp235=((struct Cyc_List_List*)(_tmp231.C_n).val)->tl;_LL13B:
 _tmp236=_tmp234;_tmp237=_tmp235;goto _LL13D;_LL13C: if((_tmp231.Abs_n).tag != 2)goto _LL12F;if((struct Cyc_List_List*)(_tmp231.Abs_n).val == 0)goto _LL12F;_tmp236=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp231.Abs_n).val)->hd;_tmp237=((struct Cyc_List_List*)(_tmp231.Abs_n).val)->tl;_LL13D:
# 1106
 first=_tmp236;{struct Cyc_List_List*_tmp3F1;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp237,((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1->hd=_tmp21F,((_tmp3F1->tl=0,_tmp3F1)))))));}goto _LL12F;_LL12F:;}{
# 1108
struct Cyc_List_List*_tmp239=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp3F2;ge->availables=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2->hd=_tmp239,((_tmp3F2->tl=ge->availables,_tmp3F2))))));}
Cyc_Tc_tc_decls(te,_tmp220,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL10E;};}_LL12B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp221=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp221->tag != 12)goto _LL12D;else{_tmp222=_tmp221->f1;}}_LL12C:
# 1115
 Cyc_Tc_tc_decls(te,_tmp222,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL10E;_LL12D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp223=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp207;if(_tmp223->tag != 13)goto _LL10E;else{_tmp224=_tmp223->f1;_tmp225=_tmp223->f2;}}_LL12E:
# 1119
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp225);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp225,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp224,1,1,check_var_init,grgn,& newexs);
# 1124
for(0;_tmp225 != 0;_tmp225=_tmp225->tl){
struct _tuple12*_tmp23B=(struct _tuple12*)_tmp225->hd;
if(!(*_tmp23B).f3){
const char*_tmp3F6;void*_tmp3F5[1];struct Cyc_String_pa_PrintArg_struct _tmp3F4;(_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp23B).f2)),((_tmp3F5[0]=& _tmp3F4,Cyc_Tcutil_warn((*_tmp23B).f1,((_tmp3F6="%s is exported but not defined",_tag_dyneither(_tmp3F6,sizeof(char),31))),_tag_dyneither(_tmp3F5,sizeof(void*),1)))))));}}
# 1130
goto _LL10E;};_LL10E:;}}
# 1135
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1137
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1154 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1156
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1158
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple17{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1162
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_stmttmp20=d->r;void*_tmp23F=_stmttmp20;struct Cyc_Absyn_Vardecl*_tmp241;struct Cyc_List_List**_tmp243;struct Cyc_List_List**_tmp245;struct Cyc_List_List**_tmp247;struct Cyc_List_List**_tmp249;_LL13F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp240=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp23F;if(_tmp240->tag != 0)goto _LL141;else{_tmp241=_tmp240->f1;}}_LL140:
# 1167
 if((env->in_cinclude  || _tmp241->sc != Cyc_Absyn_Extern  && _tmp241->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp241->name,Cyc_Absyn_uniquergn_qvar))
# 1170
return 1;{
# 1172
struct _tuple0*_stmttmp21=_tmp241->name;union Cyc_Absyn_Nmspace _tmp24B;struct _dyneither_ptr*_tmp24C;struct _tuple0*_tmp24A=_stmttmp21;_tmp24B=_tmp24A->f1;_tmp24C=_tmp24A->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp24D=_tmp24B;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp250;_LL14C: if((_tmp24D.Loc_n).tag != 4)goto _LL14E;_LL14D:
 ns=0;goto _LL14B;_LL14E: if((_tmp24D.Rel_n).tag != 1)goto _LL150;_tmp24E=(struct Cyc_List_List*)(_tmp24D.Rel_n).val;_LL14F:
 ns=_tmp24E;goto _LL14B;_LL150: if((_tmp24D.C_n).tag != 3)goto _LL152;_tmp24F=(struct Cyc_List_List*)(_tmp24D.C_n).val;_LL151:
 ns=_tmp24F;goto _LL14B;_LL152: if((_tmp24D.Abs_n).tag != 2)goto _LL14B;_tmp250=(struct Cyc_List_List*)(_tmp24D.Abs_n).val;_LL153:
 ns=_tmp250;goto _LL14B;_LL14B:;}{
# 1180
struct _tuple17*_tmp251=((struct _tuple17*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp252=(*_tmp251).f1;
int _tmp253=(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp252->ordinaries,_tmp24C)).f2;
if(!_tmp253)
(*_tmp251).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp251).f2,_tmp24C);
# 1188
return _tmp253;};};};_LL141: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp242=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp23F;if(_tmp242->tag != 12)goto _LL143;else{_tmp243=(struct Cyc_List_List**)& _tmp242->f1;}}_LL142:
 _tmp245=_tmp243;goto _LL144;_LL143: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp244=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp23F;if(_tmp244->tag != 11)goto _LL145;else{_tmp245=(struct Cyc_List_List**)& _tmp244->f2;}}_LL144:
 _tmp247=_tmp245;goto _LL146;_LL145: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp246=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp23F;if(_tmp246->tag != 10)goto _LL147;else{_tmp247=(struct Cyc_List_List**)& _tmp246->f2;}}_LL146:
# 1192
*_tmp247=Cyc_Tc_treeshake_f(env,*_tmp247);
return 1;_LL147: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp248=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp23F;if(_tmp248->tag != 13)goto _LL149;else{_tmp249=(struct Cyc_List_List**)& _tmp248->f1;}}_LL148: {
# 1195
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp249=Cyc_Tc_treeshake_f(env,*_tmp249);
env->in_cinclude=in_cinclude;
return 1;}_LL149:;_LL14A:
 return 1;_LL13E:;}
# 1204
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1208
static struct _tuple17*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1210
struct _tuple17*_tmp3F7;return(_tmp3F7=_region_malloc(rgn,sizeof(*_tmp3F7)),((_tmp3F7->f1=ge,((_tmp3F7->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp3F7)))));}struct _tuple18{struct Cyc_List_List*f1;struct _tuple17*f2;};
# 1213
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp255=_new_region("rgn");struct _RegionHandle*rgn=& _tmp255;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp3F8;struct Cyc_Tc_TreeshakeEnv _tmp256=(_tmp3F8.rgn=rgn,((_tmp3F8.in_cinclude=0,((_tmp3F8.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple17*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp3F8)))));
struct Cyc_List_List*_tmp257=Cyc_Tc_treeshake_f(& _tmp256,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp256.nsdict)){
struct Cyc_List_List*_tmp258=_tmp257;_npop_handler(0);return _tmp258;}{
# 1221
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp256.nsdict);
struct _tuple18 _tmp259=*((struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp256.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp259)){
struct Cyc_Tcenv_Genv*_tmp25B;struct Cyc_Set_Set*_tmp25C;struct _tuple18 _tmp25A=_tmp259;_tmp25B=(_tmp25A.f2)->f1;_tmp25C=(_tmp25A.f2)->f2;
_tmp25B->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple13*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp25B->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple13*y))Cyc_Tc_treeshake_remove_f,_tmp25C,_tmp25B->ordinaries);}{
# 1227
struct Cyc_List_List*_tmp25D=_tmp257;_npop_handler(0);return _tmp25D;};};}
# 1215
;_pop_region(rgn);}
