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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 937
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;int f2;};
# 61 "tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);
# 66
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 73
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 82
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 130 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 139 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 157
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 247 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 319
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 74
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";
# 43 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp171(struct _dyneither_ptr**msg1,struct _dyneither_ptr*ap,unsigned int*_tmp170,unsigned int*_tmp16F,void***_tmp16D){for(*_tmp170=0;*_tmp170 < *_tmp16F;(*_tmp170)++){struct Cyc_String_pa_PrintArg_struct _tmp16B;struct Cyc_String_pa_PrintArg_struct*_tmp16A;(*_tmp16D)[*_tmp170]=(void*)(
# 53
*_tmp170 == 0?(void*)((_tmp16A=_cycalloc(sizeof(*_tmp16A)),((_tmp16A[0]=((_tmp16B.tag=0,((_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),_tmp16B)))),_tmp16A)))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(int)(*_tmp170 - 1))));}}
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
const char*_tmp168;struct _dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp168="%s ",_tag_dyneither(_tmp168,sizeof(char),4))),(struct _dyneither_ptr)fmt);
unsigned int _tmp170;unsigned int _tmp16F;struct _dyneither_ptr _tmp16E;void**_tmp16D;unsigned int _tmp16C;struct _dyneither_ptr ap2=(_tmp16C=_get_dyneither_size(ap,sizeof(void*))+ 1,((_tmp16D=(void**)_cycalloc(_check_times(sizeof(void*),_tmp16C)),((_tmp16E=_tag_dyneither(_tmp16D,sizeof(void*),_tmp16C),((((_tmp16F=_tmp16C,_tmp171(& msg1,& ap,& _tmp170,& _tmp16F,& _tmp16D))),_tmp16E)))))));
# 54
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};}
# 60
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 62
const char*_tmp172;struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?(_tmp172="public",_tag_dyneither(_tmp172,sizeof(char),7)): Cyc_Absynpp_scope2string(s0);
const char*_tmp173;struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?(_tmp173="public",_tag_dyneither(_tmp173,sizeof(char),7)): Cyc_Absynpp_scope2string(s1);
const char*_tmp17A;void*_tmp179[4];struct Cyc_String_pa_PrintArg_struct _tmp178;struct Cyc_String_pa_PrintArg_struct _tmp177;struct Cyc_String_pa_PrintArg_struct _tmp176;struct Cyc_String_pa_PrintArg_struct _tmp175;(_tmp175.tag=0,((_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_scope2string(s0)),((_tmp176.tag=0,((_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s1)),((_tmp177.tag=0,((_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp178.tag=0,((_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp179[0]=& _tmp178,((_tmp179[1]=& _tmp177,((_tmp179[2]=& _tmp176,((_tmp179[3]=& _tmp175,Cyc_Tcdecl_merr(loc,msg,((_tmp17A="%s %s is %s whereas expected scope is %s",_tag_dyneither(_tmp17A,sizeof(char),41))),_tag_dyneither(_tmp179,sizeof(void*),4)))))))))))))))))))))))));}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 68
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 70
{struct _tuple11 _tmp17B;struct _tuple11 _tmp11=(_tmp17B.f1=s0,((_tmp17B.f2=s1,_tmp17B)));_LL1: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL3;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL3;_LL2:
# 72
 goto _LL0;_LL3: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL5;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL5;_LL4:
# 74
 goto _LL6;_LL5: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL7;if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL7;_LL6:
 goto _LL8;_LL7: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL9;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL9;_LL8:
 goto _LLA;_LL9: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LLB;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLB;_LLA:
 if(externCmerge)goto _LL0;
goto _LLC;_LLB: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LLD;_LLC:
 goto _LLE;_LLD: if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLF;_LLE:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp17C;return(_tmp17C.f1=s1,((_tmp17C.f2=0,_tmp17C)));};_LLF: if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL11;_LL10:
# 82
 s1=s0;goto _LL0;_LL11: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LL13;_LL12:
 goto _LL0;_LL13: if(_tmp11.f1 != Cyc_Absyn_Static)goto _LL15;if(_tmp11.f2 != Cyc_Absyn_Static)goto _LL15;_LL14:
# 85
 goto _LL16;_LL15: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL17;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp11.f1 != Cyc_Absyn_Abstract)goto _LL19;if(_tmp11.f2 != Cyc_Absyn_Abstract)goto _LL19;_LL18:
 goto _LL0;_LL19: if(_tmp11.f1 != Cyc_Absyn_Register)goto _LL1B;if(_tmp11.f2 != Cyc_Absyn_Register)goto _LL1B;_LL1A:
 goto _LL0;_LL1B:;_LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp17D;return(_tmp17D.f1=s1,((_tmp17D.f2=0,_tmp17D)));};_LL0:;}{
# 91
struct _tuple10 _tmp17E;return(_tmp17E.f1=s1,((_tmp17E.f2=1,_tmp17E)));};}
# 94
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 96
return Cyc_Tcutil_unify(t0,t1);}
# 100
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmp15=e->r;int _tmp17;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp15;if(_tmp16->tag != 0)goto _LL20;else{if(((_tmp16->f1).Int_c).tag != 5)goto _LL20;_tmp17=((struct _tuple5)((_tmp16->f1).Int_c).val).f2;}}_LL1F:
 return(unsigned int)_tmp17;_LL20:;_LL21: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp184;const char*_tmp183;struct Cyc_Core_Invalid_argument_exn_struct*_tmp182;(int)_throw((void*)((_tmp182=_cycalloc(sizeof(*_tmp182)),((_tmp182[0]=((_tmp184.tag=Cyc_Core_Invalid_argument,((_tmp184.f1=((_tmp183="Tcdecl::get_uint_const_value",_tag_dyneither(_tmp183,sizeof(char),29))),_tmp184)))),_tmp182)))));}_LL1D:;}
# 107
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 110
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){
{const char*_tmp189;void*_tmp188[2];struct Cyc_String_pa_PrintArg_struct _tmp187;struct Cyc_String_pa_PrintArg_struct _tmp186;(_tmp186.tag=0,((_tmp186.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp187.tag=0,((_tmp187.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp188[0]=& _tmp187,((_tmp188[1]=& _tmp186,Cyc_Tcdecl_merr(loc,msg,((_tmp189="%s %s has a different number of type parameters",_tag_dyneither(_tmp189,sizeof(char),48))),_tag_dyneither(_tmp188,sizeof(void*),2)))))))))))));}
return 0;}{
# 114
struct Cyc_List_List*_tmp1F=tvs0;
struct Cyc_List_List*_tmp20=tvs1;
for(0;_tmp1F != 0;(_tmp1F=_tmp1F->tl,_tmp20=_tmp20->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp20))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp21=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->kind);
struct Cyc_Absyn_Kind*_tmp22=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp20->hd)->kind);
if(_tmp21 == _tmp22)continue;
{const char*_tmp191;void*_tmp190[5];struct Cyc_String_pa_PrintArg_struct _tmp18F;struct Cyc_String_pa_PrintArg_struct _tmp18E;struct Cyc_String_pa_PrintArg_struct _tmp18D;struct Cyc_String_pa_PrintArg_struct _tmp18C;struct Cyc_String_pa_PrintArg_struct _tmp18B;(_tmp18B.tag=0,((_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp22)),((_tmp18C.tag=0,((_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp1F->hd)->name),((_tmp18D.tag=0,((_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp21)),((_tmp18E.tag=0,((_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp18F.tag=0,((_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp190[0]=& _tmp18F,((_tmp190[1]=& _tmp18E,((_tmp190[2]=& _tmp18D,((_tmp190[3]=& _tmp18C,((_tmp190[4]=& _tmp18B,Cyc_Tcdecl_merr(loc,msg,((_tmp191="%s %s has a different kind (%s) for type parameter %s (%s)",_tag_dyneither(_tmp191,sizeof(char),59))),_tag_dyneither(_tmp190,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;};}
# 125
return 1;};}
# 128
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 130
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp196;void*_tmp195[2];struct Cyc_String_pa_PrintArg_struct _tmp194;struct Cyc_String_pa_PrintArg_struct _tmp193;(_tmp193.tag=0,((_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp194.tag=0,((_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp195[0]=& _tmp194,((_tmp195[1]=& _tmp193,Cyc_Tcdecl_merr(loc,msg,((_tmp196="%s %s has different attributes",_tag_dyneither(_tmp196,sizeof(char),31))),_tag_dyneither(_tmp195,sizeof(void*),2)))))))))))));}
return 0;}
# 134
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 137
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 139
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
struct _tuple12*_tmp1A0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp19F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19E;struct Cyc_List_List*_tmp19D;inst=((_tmp19D=_cycalloc(sizeof(*_tmp19D)),((_tmp19D->hd=((_tmp1A0=_cycalloc(sizeof(*_tmp1A0)),((_tmp1A0->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,((_tmp1A0->f2=(void*)((_tmp19E=_cycalloc(sizeof(*_tmp19E)),((_tmp19E[0]=((_tmp19F.tag=2,((_tmp19F.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp19F)))),_tmp19E)))),_tmp1A0)))))),((_tmp19D->tl=inst,_tmp19D))))));}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 145
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 147
struct _dyneither_ptr _tmp32=Cyc_Absynpp_qvar2string(d0->name);
int _tmp33=1;
# 153
if(!(d0->kind == d1->kind))return 0;
# 156
{const char*_tmp1A1;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp1A1="type",_tag_dyneither(_tmp1A1,sizeof(char),5))),_tmp32,loc,msg))return 0;}{
# 159
enum Cyc_Absyn_Scope _tmp37;int _tmp38;const char*_tmp1A2;struct _tuple10 _tmp36=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp1A2="type",_tag_dyneither(_tmp1A2,sizeof(char),5))),_tmp32,loc,msg,1);_tmp37=_tmp36.f1;_tmp38=_tmp36.f2;
if(!_tmp38)_tmp33=0;
# 164
{const char*_tmp1A3;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,((_tmp1A3="type",_tag_dyneither(_tmp1A3,sizeof(char),5))),_tmp32,loc,msg))_tmp33=0;}{
# 166
struct Cyc_Absyn_Aggrdecl*d2;
# 168
{struct _tuple13 _tmp1A4;struct _tuple13 _tmp3B=(_tmp1A4.f1=d0->impl,((_tmp1A4.f2=d1->impl,_tmp1A4)));struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3E;int _tmp3F;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp42;int _tmp43;_LL23: if(_tmp3B.f2 != 0)goto _LL25;_LL24:
 d2=d0;goto _LL22;_LL25: if(_tmp3B.f1 != 0)goto _LL27;_LL26:
 d2=d1;goto _LL22;_LL27: if(_tmp3B.f1 == 0)goto _LL22;_tmp3C=(_tmp3B.f1)->exist_vars;_tmp3D=(_tmp3B.f1)->rgn_po;_tmp3E=(_tmp3B.f1)->fields;_tmp3F=(_tmp3B.f1)->tagged;if(_tmp3B.f2 == 0)goto _LL22;_tmp40=(_tmp3B.f2)->exist_vars;_tmp41=(_tmp3B.f2)->rgn_po;_tmp42=(_tmp3B.f2)->fields;_tmp43=(_tmp3B.f2)->tagged;_LL28:
# 175
{const char*_tmp1A5;if(!Cyc_Tcdecl_check_tvs(_tmp3C,_tmp40,((_tmp1A5="type",_tag_dyneither(_tmp1A5,sizeof(char),5))),_tmp32,loc,msg))
return 0;}{
# 179
struct Cyc_List_List*_tmp45=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp3C),
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp40));
# 184
for(0;_tmp3D != 0  && _tmp41 != 0;
(_tmp3D=_tmp3D->tl,_tmp41=_tmp41->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3D->hd)).f1,(*((struct _tuple14*)_tmp41->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3D->hd)).f2,(*((struct _tuple14*)_tmp41->hd)).f2);}
# 191
for(0;_tmp3E != 0  && _tmp42 != 0;(_tmp3E=_tmp3E->tl,_tmp42=_tmp42->tl)){
struct _dyneither_ptr*_tmp47;struct Cyc_Absyn_Tqual _tmp48;void*_tmp49;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_List_List*_tmp4B;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Aggrfield*_tmp46=(struct Cyc_Absyn_Aggrfield*)_tmp3E->hd;_tmp47=_tmp46->name;_tmp48=_tmp46->tq;_tmp49=_tmp46->type;_tmp4A=_tmp46->width;_tmp4B=_tmp46->attributes;_tmp4C=_tmp46->requires_clause;{
struct _dyneither_ptr*_tmp4E;struct Cyc_Absyn_Tqual _tmp4F;void*_tmp50;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Aggrfield*_tmp4D=(struct Cyc_Absyn_Aggrfield*)_tmp42->hd;_tmp4E=_tmp4D->name;_tmp4F=_tmp4D->tq;_tmp50=_tmp4D->type;_tmp51=_tmp4D->width;_tmp52=_tmp4D->attributes;_tmp53=_tmp4D->requires_clause;
# 195
if(Cyc_strptrcmp(_tmp47,_tmp4E)!= 0){
{const char*_tmp1AE;void*_tmp1AD[4];const char*_tmp1AC;struct Cyc_String_pa_PrintArg_struct _tmp1AB;struct Cyc_String_pa_PrintArg_struct _tmp1AA;struct Cyc_String_pa_PrintArg_struct _tmp1A9;struct Cyc_String_pa_PrintArg_struct _tmp1A8;(_tmp1A8.tag=0,((_tmp1A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E),((_tmp1A9.tag=0,((_tmp1A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1AA.tag=0,((_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1AB.tag=0,((_tmp1AB.f1=(struct _dyneither_ptr)((_tmp1AC="type",_tag_dyneither(_tmp1AC,sizeof(char),5))),((_tmp1AD[0]=& _tmp1AB,((_tmp1AD[1]=& _tmp1AA,((_tmp1AD[2]=& _tmp1A9,((_tmp1AD[3]=& _tmp1A8,Cyc_Tcdecl_merr(loc,msg,((_tmp1AE="%s %s : field name mismatch %s != %s",_tag_dyneither(_tmp1AE,sizeof(char),37))),_tag_dyneither(_tmp1AD,sizeof(void*),4)))))))))))))))))))))))));}
# 198
return 0;}
# 200
if(!Cyc_Tcutil_same_atts(_tmp4B,_tmp52)){
{const char*_tmp1B6;void*_tmp1B5[3];const char*_tmp1B4;struct Cyc_String_pa_PrintArg_struct _tmp1B3;struct Cyc_String_pa_PrintArg_struct _tmp1B2;struct Cyc_String_pa_PrintArg_struct _tmp1B1;(_tmp1B1.tag=0,((_tmp1B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1B2.tag=0,((_tmp1B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1B3.tag=0,((_tmp1B3.f1=(struct _dyneither_ptr)((_tmp1B4="type",_tag_dyneither(_tmp1B4,sizeof(char),5))),((_tmp1B5[0]=& _tmp1B3,((_tmp1B5[1]=& _tmp1B2,((_tmp1B5[2]=& _tmp1B1,Cyc_Tcdecl_merr(loc,msg,((_tmp1B6="%s %s : attribute mismatch on field %s",_tag_dyneither(_tmp1B6,sizeof(char),39))),_tag_dyneither(_tmp1B5,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}
# 204
if(!Cyc_Tcutil_equal_tqual(_tmp48,_tmp4F)){
{const char*_tmp1BE;void*_tmp1BD[3];const char*_tmp1BC;struct Cyc_String_pa_PrintArg_struct _tmp1BB;struct Cyc_String_pa_PrintArg_struct _tmp1BA;struct Cyc_String_pa_PrintArg_struct _tmp1B9;(_tmp1B9.tag=0,((_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1BA.tag=0,((_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1BB.tag=0,((_tmp1BB.f1=(struct _dyneither_ptr)((_tmp1BC="type",_tag_dyneither(_tmp1BC,sizeof(char),5))),((_tmp1BD[0]=& _tmp1BB,((_tmp1BD[1]=& _tmp1BA,((_tmp1BD[2]=& _tmp1B9,Cyc_Tcdecl_merr(loc,msg,((_tmp1BE="%s %s : qualifier mismatch on field %s",_tag_dyneither(_tmp1BE,sizeof(char),39))),_tag_dyneither(_tmp1BD,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}
# 208
if(((_tmp4A != 0  && _tmp51 != 0) && 
Cyc_Tcdecl_get_uint_const_value(_tmp4A)!= 
Cyc_Tcdecl_get_uint_const_value(_tmp51) || 
_tmp4A == 0  && _tmp51 != 0) || 
_tmp4A != 0  && _tmp51 == 0){
{const char*_tmp1C6;void*_tmp1C5[3];const char*_tmp1C4;struct Cyc_String_pa_PrintArg_struct _tmp1C3;struct Cyc_String_pa_PrintArg_struct _tmp1C2;struct Cyc_String_pa_PrintArg_struct _tmp1C1;(_tmp1C1.tag=0,((_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1C2.tag=0,((_tmp1C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dyneither_ptr)((_tmp1C4="type",_tag_dyneither(_tmp1C4,sizeof(char),5))),((_tmp1C5[0]=& _tmp1C3,((_tmp1C5[1]=& _tmp1C2,((_tmp1C5[2]=& _tmp1C1,Cyc_Tcdecl_merr(loc,msg,((_tmp1C6="%s %s : bitfield mismatch on field %s",_tag_dyneither(_tmp1C6,sizeof(char),38))),_tag_dyneither(_tmp1C5,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}
# 216
if(_tmp4C != 0  && _tmp53 != 0){
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1C9;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C8;void*subst_w1=Cyc_Tcutil_substitute(_tmp45,(void*)((_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8[0]=((_tmp1C9.tag=18,((_tmp1C9.f1=_tmp53,_tmp1C9)))),_tmp1C8)))));
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1CC;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1CB;if(!Cyc_Tcdecl_check_type((void*)((_tmp1CB=_cycalloc(sizeof(*_tmp1CB)),((_tmp1CB[0]=((_tmp1CC.tag=18,((_tmp1CC.f1=_tmp4C,_tmp1CC)))),_tmp1CB)))),subst_w1)){
{const char*_tmp1D2;void*_tmp1D1[3];struct Cyc_String_pa_PrintArg_struct _tmp1D0;struct Cyc_String_pa_PrintArg_struct _tmp1CF;struct Cyc_String_pa_PrintArg_struct _tmp1CE;(_tmp1CE.tag=0,((_tmp1CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp53)),((_tmp1CF.tag=0,((_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp4C)),((_tmp1D0.tag=0,((_tmp1D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1D1[0]=& _tmp1D0,((_tmp1D1[1]=& _tmp1CF,((_tmp1D1[2]=& _tmp1CE,Cyc_Tcdecl_merr(loc,msg,((_tmp1D2="type %s : mismatch on @requires clauses: %s != %s",_tag_dyneither(_tmp1D2,sizeof(char),50))),_tag_dyneither(_tmp1D1,sizeof(void*),3)))))))))))))))))));}
_tmp33=0;}}else{
# 223
if(_tmp4C == 0  && _tmp53 != 0  || 
_tmp4C != 0  && _tmp53 == 0){
{const char*_tmp1D6;void*_tmp1D5[1];struct Cyc_String_pa_PrintArg_struct _tmp1D4;(_tmp1D4.tag=0,((_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1D5[0]=& _tmp1D4,Cyc_Tcdecl_merr(loc,msg,((_tmp1D6="type %s : mismatch on @requires clauses",_tag_dyneither(_tmp1D6,sizeof(char),40))),_tag_dyneither(_tmp1D5,sizeof(void*),1)))))));}
_tmp33=0;}}{
# 229
void*subst_t1=Cyc_Tcutil_substitute(_tmp45,_tmp50);
if(!Cyc_Tcdecl_check_type(_tmp49,subst_t1)){
{const char*_tmp1DD;void*_tmp1DC[4];struct Cyc_String_pa_PrintArg_struct _tmp1DB;struct Cyc_String_pa_PrintArg_struct _tmp1DA;struct Cyc_String_pa_PrintArg_struct _tmp1D9;struct Cyc_String_pa_PrintArg_struct _tmp1D8;(_tmp1D8.tag=0,((_tmp1D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp49)),((_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47),((_tmp1DB.tag=0,((_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1DC[0]=& _tmp1DB,((_tmp1DC[1]=& _tmp1DA,((_tmp1DC[2]=& _tmp1D9,((_tmp1DC[3]=& _tmp1D8,Cyc_Tcdecl_merr(loc,msg,((_tmp1DD="type %s : type mismatch on field %s: %s != %s",_tag_dyneither(_tmp1DD,sizeof(char),46))),_tag_dyneither(_tmp1DC,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp33=0;}};};}
# 238
if(_tmp3E != 0){
{const char*_tmp1E2;void*_tmp1E1[2];struct Cyc_String_pa_PrintArg_struct _tmp1E0;struct Cyc_String_pa_PrintArg_struct _tmp1DF;(_tmp1DF.tag=0,((_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp3E->hd)->name),((_tmp1E0.tag=0,((_tmp1E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1E1[0]=& _tmp1E0,((_tmp1E1[1]=& _tmp1DF,Cyc_Tcdecl_merr(loc,msg,((_tmp1E2="type %s is missing field %s",_tag_dyneither(_tmp1E2,sizeof(char),28))),_tag_dyneither(_tmp1E1,sizeof(void*),2)))))))))))));}
_tmp33=0;}
# 242
if(_tmp42 != 0){
{const char*_tmp1E7;void*_tmp1E6[2];struct Cyc_String_pa_PrintArg_struct _tmp1E5;struct Cyc_String_pa_PrintArg_struct _tmp1E4;(_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp42->hd)->name),((_tmp1E5.tag=0,((_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1E6[0]=& _tmp1E5,((_tmp1E6[1]=& _tmp1E4,Cyc_Tcdecl_merr(loc,msg,((_tmp1E7="type %s has extra field %s",_tag_dyneither(_tmp1E7,sizeof(char),27))),_tag_dyneither(_tmp1E6,sizeof(void*),2)))))))))))));}
_tmp33=0;}
# 247
if(_tmp3F != _tmp43){
{const char*_tmp1EB;void*_tmp1EA[1];struct Cyc_String_pa_PrintArg_struct _tmp1E9;(_tmp1E9.tag=0,((_tmp1E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32),((_tmp1EA[0]=& _tmp1E9,Cyc_Tcdecl_merr(loc,msg,((_tmp1EB="%s : previous declaration disagrees with @tagged qualifier",_tag_dyneither(_tmp1EB,sizeof(char),59))),_tag_dyneither(_tmp1EA,sizeof(void*),1)))))));}
_tmp33=0;}
# 252
d2=d0;
goto _LL22;};_LL22:;}
# 256
if(!_tmp33)return 0;
# 258
if(_tmp37 == d2->sc)
return d2;else{
# 261
{struct Cyc_Absyn_Aggrdecl*_tmp1EC;d2=((_tmp1EC=_cycalloc(sizeof(*_tmp1EC)),((_tmp1EC[0]=*d2,_tmp1EC))));}
d2->sc=_tmp37;
return d2;}};};}
# 267
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
const char*_tmp1EE;const char*_tmp1ED;return is_x?(_tmp1EE="@extensible datatype",_tag_dyneither(_tmp1EE,sizeof(char),21)):((_tmp1ED="datatype",_tag_dyneither(_tmp1ED,sizeof(char),9)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 273
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 277
unsigned int loc=f1->loc;
# 280
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
{const char*_tmp1F5;void*_tmp1F4[4];struct Cyc_String_pa_PrintArg_struct _tmp1F3;struct Cyc_String_pa_PrintArg_struct _tmp1F2;struct Cyc_String_pa_PrintArg_struct _tmp1F1;struct Cyc_String_pa_PrintArg_struct _tmp1F0;(_tmp1F0.tag=0,((_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp1F1.tag=0,((_tmp1F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp1F2.tag=0,((_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1F3.tag=0,((_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1F4[0]=& _tmp1F3,((_tmp1F4[1]=& _tmp1F2,((_tmp1F4[2]=& _tmp1F1,((_tmp1F4[3]=& _tmp1F0,Cyc_Tcdecl_merr(loc,msg,((_tmp1F5="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp1F5,sizeof(char),36))),_tag_dyneither(_tmp1F4,sizeof(void*),4)))))))))))))))))))))))));}
# 283
return 0;}{
# 286
struct _dyneither_ptr _tmp93=*(*f0->name).f2;
# 289
enum Cyc_Absyn_Scope _tmp99;int _tmp9A;const char*_tmp1FA;void*_tmp1F9[2];struct Cyc_String_pa_PrintArg_struct _tmp1F8;struct Cyc_String_pa_PrintArg_struct _tmp1F7;struct _tuple10 _tmp98=
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)((_tmp1F7.tag=0,((_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1F8.tag=0,((_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1F9[0]=& _tmp1F8,((_tmp1F9[1]=& _tmp1F7,Cyc_aprintf(((_tmp1FA="in %s %s, field",_tag_dyneither(_tmp1FA,sizeof(char),16))),_tag_dyneither(_tmp1F9,sizeof(void*),2)))))))))))))),_tmp93,loc,msg,0);_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;{
# 293
struct Cyc_List_List*_tmp9B=f0->typs;
struct Cyc_List_List*_tmp9C=f1->typs;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9C)){
{const char*_tmp200;void*_tmp1FF[3];struct Cyc_String_pa_PrintArg_struct _tmp1FE;struct Cyc_String_pa_PrintArg_struct _tmp1FD;struct Cyc_String_pa_PrintArg_struct _tmp1FC;(_tmp1FC.tag=0,((_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp93),((_tmp1FD.tag=0,((_tmp1FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1FF[0]=& _tmp1FE,((_tmp1FF[1]=& _tmp1FD,((_tmp1FF[2]=& _tmp1FC,Cyc_Tcdecl_merr(loc,msg,((_tmp200="%s %s, field %s: parameter number mismatch",_tag_dyneither(_tmp200,sizeof(char),43))),_tag_dyneither(_tmp1FF,sizeof(void*),3)))))))))))))))))));}
_tmp9A=0;}
# 300
for(0;_tmp9B != 0;(_tmp9B=_tmp9B->tl,_tmp9C=_tmp9C->tl)){
# 302
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp9B->hd)).f1,(*((struct _tuple15*)((struct Cyc_List_List*)_check_null(_tmp9C))->hd)).f1)){
{const char*_tmp206;void*_tmp205[3];struct Cyc_String_pa_PrintArg_struct _tmp204;struct Cyc_String_pa_PrintArg_struct _tmp203;struct Cyc_String_pa_PrintArg_struct _tmp202;(_tmp202.tag=0,((_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp93),((_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp204.tag=0,((_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp205[0]=& _tmp204,((_tmp205[1]=& _tmp203,((_tmp205[2]=& _tmp202,Cyc_Tcdecl_merr(loc,msg,((_tmp206="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp206,sizeof(char),37))),_tag_dyneither(_tmp205,sizeof(void*),3)))))))))))))))))));}
_tmp9A=0;}{
# 306
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp9C->hd)).f2);
# 308
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp9B->hd)).f2,subst_t1)){
{const char*_tmp20E;void*_tmp20D[5];struct Cyc_String_pa_PrintArg_struct _tmp20C;struct Cyc_String_pa_PrintArg_struct _tmp20B;struct Cyc_String_pa_PrintArg_struct _tmp20A;struct Cyc_String_pa_PrintArg_struct _tmp209;struct Cyc_String_pa_PrintArg_struct _tmp208;(_tmp208.tag=0,((_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp209.tag=0,((_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp9B->hd)).f2)),((_tmp20A.tag=0,((_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp93),((_tmp20B.tag=0,((_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp20C.tag=0,((_tmp20C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp20D[0]=& _tmp20C,((_tmp20D[1]=& _tmp20B,((_tmp20D[2]=& _tmp20A,((_tmp20D[3]=& _tmp209,((_tmp20D[4]=& _tmp208,Cyc_Tcdecl_merr(loc,msg,((_tmp20E="%s %s, field %s: parameter type mismatch %s != %s",_tag_dyneither(_tmp20E,sizeof(char),50))),_tag_dyneither(_tmp20D,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp9A=0;}};}
# 316
if(!_tmp9A)return 0;
if(f0->sc != _tmp99){
struct Cyc_Absyn_Datatypefield*_tmp20F;struct Cyc_Absyn_Datatypefield*_tmpAE=(_tmp20F=_cycalloc(sizeof(*_tmp20F)),((_tmp20F[0]=*f0,_tmp20F)));
((struct Cyc_Absyn_Datatypefield*)_check_null(_tmpAE))->sc=_tmp99;
return _tmpAE;}else{
return f0;}};};}
# 324
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 327
struct Cyc_Absyn_Tqual _tmpB1;void*_tmpB2;struct _tuple15*_tmpB0=x;_tmpB1=_tmpB0->f1;_tmpB2=_tmpB0->f2;{
struct _tuple15*_tmp210;return(_tmp210=_cycalloc(sizeof(*_tmp210)),((_tmp210->f1=_tmpB1,((_tmp210->f2=Cyc_Tcutil_substitute(inst,_tmpB2),_tmp210)))));};}
# 333
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 335
struct Cyc_Absyn_Datatypefield*_tmp211;struct Cyc_Absyn_Datatypefield*_tmpB4=(_tmp211=_cycalloc(sizeof(*_tmp211)),((_tmp211[0]=*f1,_tmp211)));
_tmpB4->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 338
return _tmpB4;}
# 345
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 354
struct Cyc_List_List**_tmp212;struct Cyc_List_List**f2sp=(_tmp212=_cycalloc(sizeof(*_tmp212)),((_tmp212[0]=0,_tmp212)));
struct Cyc_List_List**_tmpB6=f2sp;
int cmp=-1;
# 358
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmpB7=(_tmp213=_cycalloc(sizeof(*_tmp213)),((_tmp213->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp213->tl=0,_tmp213)))));
*_tmpB6=_tmpB7;
_tmpB6=&((struct Cyc_List_List*)_check_null(_tmpB7))->tl;
f0s=f0s->tl;}
# 365
if(f0s == 0  || cmp > 0){
# 367
*incl=0;{
struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmpB9=(_tmp214=_cycalloc(sizeof(*_tmp214)),((_tmp214->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp214->tl=0,_tmp214)))));
*_tmpB6=_tmpB9;
_tmpB6=&((struct Cyc_List_List*)_check_null(_tmpB9))->tl;};}else{
# 373
struct Cyc_Absyn_Datatypefield*_tmpBB=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpBB != 0){
if(_tmpBB != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmpBC=(_tmp215=_cycalloc(sizeof(*_tmp215)),((_tmp215->hd=_tmpBB,((_tmp215->tl=0,_tmp215)))));
*_tmpB6=_tmpBC;
_tmpB6=&((struct Cyc_List_List*)_check_null(_tmpBC))->tl;};}else{
# 380
*res=0;}
# 382
f0s=f0s->tl;}}
# 387
if(f1s != 0){
*incl=0;
*_tmpB6=f1s;}else{
# 391
*_tmpB6=f0s;}
# 394
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 398
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0){struct _tuple16 _tmp216;return(_tmp216.f1=0,((_tmp216.f2=0,_tmp216)));}
if(f->tl == 0){struct _tuple16 _tmp217;return(_tmp217.f1=f,((_tmp217.f2=0,_tmp217)));}{
struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC3;struct _tuple16 _tmpC1=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);_tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;{
struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21B;struct _tuple16 _tmp21A;return(_tmp21A.f1=((_tmp21B=_cycalloc(sizeof(*_tmp21B)),((_tmp21B->hd=f->hd,((_tmp21B->tl=_tmpC2,_tmp21B)))))),((_tmp21A.f2=((_tmp21C=_cycalloc(sizeof(*_tmp21C)),((_tmp21C->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,((_tmp21C->tl=_tmpC3,_tmp21C)))))),_tmp21A)));};};}
# 407
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 410
struct Cyc_List_List*_tmpC8;struct Cyc_List_List*_tmpC9;struct _tuple16 _tmpC7=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpC8=_tmpC7.f1;_tmpC9=_tmpC7.f2;
if(_tmpC8 != 0  && _tmpC8->tl != 0)
_tmpC8=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC8,res,v,loc,msg);
if(_tmpC9 != 0  && _tmpC9->tl != 0)
_tmpC9=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC9,res,v,loc,msg);{
const char*_tmp21E;int*_tmp21D;return Cyc_Tcdecl_merge_xdatatype_fields(_tmpC8,_tmpC9,0,0,0,res,((_tmp21D=_cycalloc_atomic(sizeof(*_tmp21D)),((_tmp21D[0]=1,_tmp21D)))),(
(_tmp21E="@extensible datatype",_tag_dyneither(_tmp21E,sizeof(char),21))),*v,loc,msg);};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 419
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 421
struct _dyneither_ptr _tmpCC=Cyc_Absynpp_qvar2string(d0->name);
const char*_tmp21F;struct _dyneither_ptr t=(_tmp21F="datatype",_tag_dyneither(_tmp21F,sizeof(char),9));
int _tmpCD=1;
# 428
if(d0->is_extensible != d1->is_extensible){
{const char*_tmp225;void*_tmp224[3];struct Cyc_String_pa_PrintArg_struct _tmp223;struct Cyc_String_pa_PrintArg_struct _tmp222;struct Cyc_String_pa_PrintArg_struct _tmp221;(_tmp221.tag=0,((_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible)),((_tmp222.tag=0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible)),((_tmp223.tag=0,((_tmp223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCC),((_tmp224[0]=& _tmp223,((_tmp224[1]=& _tmp222,((_tmp224[2]=& _tmp221,Cyc_Tcdecl_merr(loc,msg,((_tmp225="expected %s to be a %s instead of a %s",_tag_dyneither(_tmp225,sizeof(char),39))),_tag_dyneither(_tmp224,sizeof(void*),3)))))))))))))))))));}
_tmpCD=0;}else{
# 433
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 437
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpCC,loc,msg))return 0;{
# 440
enum Cyc_Absyn_Scope _tmpD4;int _tmpD5;struct _tuple10 _tmpD3=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpCC,loc,msg,0);_tmpD4=_tmpD3.f1;_tmpD5=_tmpD3.f2;
if(!_tmpD5)_tmpCD=0;{
# 443
struct Cyc_Absyn_Datatypedecl*d2;
# 446
{struct _tuple17 _tmp226;struct _tuple17 _tmpD7=(_tmp226.f1=d0->fields,((_tmp226.f2=d1->fields,_tmp226)));struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD9;_LL2A: if(_tmpD7.f2 != 0)goto _LL2C;_LL2B:
 d2=d0;goto _LL29;_LL2C: if(_tmpD7.f1 != 0)goto _LL2E;_LL2D:
 d2=d1;goto _LL29;_LL2E: if(_tmpD7.f1 == 0)goto _LL29;_tmpD8=(struct Cyc_List_List*)(_tmpD7.f1)->v;if(_tmpD7.f2 == 0)goto _LL29;_tmpD9=(struct Cyc_List_List*)(_tmpD7.f2)->v;_LL2F: {
# 460 "tcdecl.cyc"
struct Cyc_List_List*_tmpDA=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 463
if(d0->is_extensible){
# 467
int _tmpDB=1;
struct Cyc_List_List*_tmpDC=Cyc_Tcdecl_merge_xdatatype_fields(_tmpD8,_tmpD9,_tmpDA,d0->tvs,d1->tvs,& _tmpCD,& _tmpDB,t,_tmpCC,loc,msg);
# 471
if(_tmpDB)
d2=d0;else{
# 474
{struct Cyc_Absyn_Datatypedecl*_tmp227;d2=((_tmp227=_cycalloc(sizeof(*_tmp227)),((_tmp227[0]=*d0,_tmp227))));}
d2->sc=_tmpD4;{
struct Cyc_Core_Opt*_tmp228;d2->fields=((_tmp228=_cycalloc(sizeof(*_tmp228)),((_tmp228->v=_tmpDC,_tmp228))));};}}else{
# 480
for(0;_tmpD8 != 0  && _tmpD9 != 0;(_tmpD8=_tmpD8->tl,_tmpD9=_tmpD9->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpD8->hd,(struct Cyc_Absyn_Datatypefield*)_tmpD9->hd,_tmpDA,t,_tmpCC,msg);}
# 483
if(_tmpD8 != 0){
{const char*_tmp22E;void*_tmp22D[3];struct Cyc_String_pa_PrintArg_struct _tmp22C;struct Cyc_String_pa_PrintArg_struct _tmp22B;struct Cyc_String_pa_PrintArg_struct _tmp22A;(_tmp22A.tag=0,((_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmpD9))->hd)->name).f2),((_tmp22B.tag=0,((_tmp22B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCC),((_tmp22C.tag=0,((_tmp22C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp22D[0]=& _tmp22C,((_tmp22D[1]=& _tmp22B,((_tmp22D[2]=& _tmp22A,Cyc_Tcdecl_merr(loc,msg,((_tmp22E="%s %s has extra field %s",_tag_dyneither(_tmp22E,sizeof(char),25))),_tag_dyneither(_tmp22D,sizeof(void*),3)))))))))))))))))));}
_tmpCD=0;}
# 487
if(_tmpD9 != 0){
{const char*_tmp234;void*_tmp233[3];struct Cyc_String_pa_PrintArg_struct _tmp232;struct Cyc_String_pa_PrintArg_struct _tmp231;struct Cyc_String_pa_PrintArg_struct _tmp230;(_tmp230.tag=0,((_tmp230.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpD9->hd)->name).f2),((_tmp231.tag=0,((_tmp231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCC),((_tmp232.tag=0,((_tmp232.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp233[0]=& _tmp232,((_tmp233[1]=& _tmp231,((_tmp233[2]=& _tmp230,Cyc_Tcdecl_merr(loc,msg,((_tmp234="%s %s is missing field %s",_tag_dyneither(_tmp234,sizeof(char),26))),_tag_dyneither(_tmp233,sizeof(void*),3)))))))))))))))))));}
_tmpCD=0;}
# 491
d2=d0;}
# 493
goto _LL29;}_LL29:;}
# 496
if(!_tmpCD)return 0;
# 498
if(_tmpD4 == d2->sc)
return d2;else{
# 501
{struct Cyc_Absyn_Datatypedecl*_tmp235;d2=((_tmp235=_cycalloc(sizeof(*_tmp235)),((_tmp235[0]=*d2,_tmp235))));}
d2->sc=_tmpD4;
return d2;}};};}
# 507
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 509
struct _dyneither_ptr _tmpEB=Cyc_Absynpp_qvar2string(d0->name);
int _tmpEC=1;
# 513
enum Cyc_Absyn_Scope _tmpEF;int _tmpF0;const char*_tmp236;struct _tuple10 _tmpEE=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp236="enum",_tag_dyneither(_tmp236,sizeof(char),5))),_tmpEB,loc,msg,1);_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;
if(!_tmpF0)_tmpEC=0;{
# 516
struct Cyc_Absyn_Enumdecl*d2;
# 519
{struct _tuple17 _tmp237;struct _tuple17 _tmpF2=(_tmp237.f1=d0->fields,((_tmp237.f2=d1->fields,_tmp237)));struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF4;_LL31: if(_tmpF2.f2 != 0)goto _LL33;_LL32:
 d2=d0;goto _LL30;_LL33: if(_tmpF2.f1 != 0)goto _LL35;_LL34:
 d2=d1;goto _LL30;_LL35: if(_tmpF2.f1 == 0)goto _LL30;_tmpF3=(struct Cyc_List_List*)(_tmpF2.f1)->v;if(_tmpF2.f2 == 0)goto _LL30;_tmpF4=(struct Cyc_List_List*)(_tmpF2.f2)->v;_LL36:
# 524
 for(0;_tmpF3 != 0  && _tmpF4 != 0;(_tmpF3=_tmpF3->tl,_tmpF4=_tmpF4->tl)){
struct _dyneither_ptr*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;unsigned int _tmpF8;struct Cyc_Absyn_Enumfield*_tmpF5=(struct Cyc_Absyn_Enumfield*)_tmpF3->hd;_tmpF6=(_tmpF5->name)->f2;_tmpF7=_tmpF5->tag;_tmpF8=_tmpF5->loc;{
struct _dyneither_ptr*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;unsigned int _tmpFC;struct Cyc_Absyn_Enumfield*_tmpF9=(struct Cyc_Absyn_Enumfield*)_tmpF4->hd;_tmpFA=(_tmpF9->name)->f2;_tmpFB=_tmpF9->tag;_tmpFC=_tmpF9->loc;
# 529
if(Cyc_strptrcmp(_tmpF6,_tmpFA)!= 0){
{const char*_tmp23D;void*_tmp23C[3];struct Cyc_String_pa_PrintArg_struct _tmp23B;struct Cyc_String_pa_PrintArg_struct _tmp23A;struct Cyc_String_pa_PrintArg_struct _tmp239;(_tmp239.tag=0,((_tmp239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA),((_tmp23A.tag=0,((_tmp23A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF6),((_tmp23B.tag=0,((_tmp23B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEB),((_tmp23C[0]=& _tmp23B,((_tmp23C[1]=& _tmp23A,((_tmp23C[2]=& _tmp239,Cyc_Tcdecl_merr(loc,msg,((_tmp23D="enum %s: field name mismatch %s != %s",_tag_dyneither(_tmp23D,sizeof(char),38))),_tag_dyneither(_tmp23C,sizeof(void*),3)))))))))))))))))));}
_tmpEC=0;}
# 534
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpF7))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpFB))){
{const char*_tmp242;void*_tmp241[2];struct Cyc_String_pa_PrintArg_struct _tmp240;struct Cyc_String_pa_PrintArg_struct _tmp23F;(_tmp23F.tag=0,((_tmp23F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA),((_tmp240.tag=0,((_tmp240.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEB),((_tmp241[0]=& _tmp240,((_tmp241[1]=& _tmp23F,Cyc_Tcdecl_merr(loc,msg,((_tmp242="enum %s, field %s, value mismatch",_tag_dyneither(_tmp242,sizeof(char),34))),_tag_dyneither(_tmp241,sizeof(void*),2)))))))))))));}
_tmpEC=0;}};}
# 539
d2=d0;
goto _LL30;_LL30:;}
# 543
if(!_tmpEC)return 0;
# 545
if(d2->sc == _tmpEF)
return d2;else{
# 548
{struct Cyc_Absyn_Enumdecl*_tmp243;d2=((_tmp243=_cycalloc(sizeof(*_tmp243)),((_tmp243[0]=*d2,_tmp243))));}
d2->sc=_tmpEF;
return d2;}};}
# 554
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 558
int _tmp107=1;
# 561
enum Cyc_Absyn_Scope _tmp109;int _tmp10A;struct _tuple10 _tmp108=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);_tmp109=_tmp108.f1;_tmp10A=_tmp108.f2;
if(!_tmp10A)_tmp107=0;
# 565
if(!Cyc_Tcdecl_check_type(t0,t1)){
{const char*_tmp24A;void*_tmp249[4];struct Cyc_String_pa_PrintArg_struct _tmp248;struct Cyc_String_pa_PrintArg_struct _tmp247;struct Cyc_String_pa_PrintArg_struct _tmp246;struct Cyc_String_pa_PrintArg_struct _tmp245;(_tmp245.tag=0,((_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t0)),((_tmp246.tag=0,((_tmp246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp247.tag=0,((_tmp247.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp248.tag=0,((_tmp248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp249[0]=& _tmp248,((_tmp249[1]=& _tmp247,((_tmp249[2]=& _tmp246,((_tmp249[3]=& _tmp245,Cyc_Tcdecl_merr(loc,msg,((_tmp24A="%s %s has type \n%s\n instead of \n%s\n",_tag_dyneither(_tmp24A,sizeof(char),36))),_tag_dyneither(_tmp249,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp107=0;}
# 573
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
{const char*_tmp24F;void*_tmp24E[2];struct Cyc_String_pa_PrintArg_struct _tmp24D;struct Cyc_String_pa_PrintArg_struct _tmp24C;(_tmp24C.tag=0,((_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp24D.tag=0,((_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp24E[0]=& _tmp24D,((_tmp24E[1]=& _tmp24C,Cyc_Tcdecl_merr(loc,msg,((_tmp24F="%s %s has different type qualifiers",_tag_dyneither(_tmp24F,sizeof(char),36))),_tag_dyneither(_tmp24E,sizeof(void*),2)))))))))))));}
_tmp107=0;}
# 579
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp254;void*_tmp253[2];struct Cyc_String_pa_PrintArg_struct _tmp252;struct Cyc_String_pa_PrintArg_struct _tmp251;(_tmp251.tag=0,((_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp252.tag=0,((_tmp252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp253[0]=& _tmp252,((_tmp253[1]=& _tmp251,Cyc_Tcdecl_merr(loc,msg,((_tmp254="%s %s has different attributes",_tag_dyneither(_tmp254,sizeof(char),31))),_tag_dyneither(_tmp253,sizeof(void*),2)))))))))))));}
{const char*_tmp257;void*_tmp256;(_tmp256=0,Cyc_fprintf(Cyc_stderr,((_tmp257="previous attributes: ",_tag_dyneither(_tmp257,sizeof(char),22))),_tag_dyneither(_tmp256,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){
const char*_tmp25B;void*_tmp25A[1];struct Cyc_String_pa_PrintArg_struct _tmp259;(_tmp259.tag=0,((_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp25A[0]=& _tmp259,Cyc_fprintf(Cyc_stderr,((_tmp25B="%s ",_tag_dyneither(_tmp25B,sizeof(char),4))),_tag_dyneither(_tmp25A,sizeof(void*),1)))))));}
{const char*_tmp25E;void*_tmp25D;(_tmp25D=0,Cyc_fprintf(Cyc_stderr,((_tmp25E="\ncurrent attributes: ",_tag_dyneither(_tmp25E,sizeof(char),22))),_tag_dyneither(_tmp25D,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){
const char*_tmp262;void*_tmp261[1];struct Cyc_String_pa_PrintArg_struct _tmp260;(_tmp260.tag=0,((_tmp260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp261[0]=& _tmp260,Cyc_fprintf(Cyc_stderr,((_tmp262="%s ",_tag_dyneither(_tmp262,sizeof(char),4))),_tag_dyneither(_tmp261,sizeof(void*),1)))))));}
{const char*_tmp265;void*_tmp264;(_tmp264=0,Cyc_fprintf(Cyc_stderr,((_tmp265="\n",_tag_dyneither(_tmp265,sizeof(char),2))),_tag_dyneither(_tmp264,sizeof(void*),0)));}
_tmp107=0;}{
# 591
struct _tuple10 _tmp266;return(_tmp266.f1=_tmp109,((_tmp266.f2=_tmp107,_tmp266)));};}
# 594
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 596
struct _dyneither_ptr _tmp126=Cyc_Absynpp_qvar2string(d0->name);
# 599
enum Cyc_Absyn_Scope _tmp129;int _tmp12A;const char*_tmp267;struct _tuple10 _tmp128=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,(
# 602
(_tmp267="variable",_tag_dyneither(_tmp267,sizeof(char),9))),_tmp126,loc,msg);_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
if(!_tmp12A)return 0;
# 605
if(d0->sc == _tmp129)
return d0;else{
# 608
struct Cyc_Absyn_Vardecl*_tmp268;struct Cyc_Absyn_Vardecl*_tmp12B=(_tmp268=_cycalloc(sizeof(*_tmp268)),((_tmp268[0]=*d0,_tmp268)));
((struct Cyc_Absyn_Vardecl*)_check_null(_tmp12B))->sc=_tmp129;
return _tmp12B;}}
# 614
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 616
struct _dyneither_ptr _tmp12D=Cyc_Absynpp_qvar2string(d0->name);
# 619
{const char*_tmp269;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp269="typedef",_tag_dyneither(_tmp269,sizeof(char),8))),_tmp12D,loc,msg))return 0;}{
# 622
struct Cyc_List_List*_tmp12F=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 625
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp12F,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
{const char*_tmp26F;void*_tmp26E[3];struct Cyc_String_pa_PrintArg_struct _tmp26D;struct Cyc_String_pa_PrintArg_struct _tmp26C;struct Cyc_String_pa_PrintArg_struct _tmp26B;(_tmp26B.tag=0,((_tmp26B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn))),((_tmp26C.tag=0,((_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((_tmp26D.tag=0,((_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp12D),((_tmp26E[0]=& _tmp26D,((_tmp26E[1]=& _tmp26C,((_tmp26E[2]=& _tmp26B,Cyc_Tcdecl_merr(loc,msg,((_tmp26F="typedef %s does not refer to the same type: %s != %s",_tag_dyneither(_tmp26F,sizeof(char),53))),_tag_dyneither(_tmp26E,sizeof(void*),3)))))))))))))))))));}
return 0;}}
# 633
return d0;};}
# 636
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp270;struct _tuple14 _tmp136=(_tmp270.f1=b0,((_tmp270.f2=b1,_tmp270)));struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Fndecl*_tmp140;struct Cyc_Absyn_Fndecl*_tmp143;struct Cyc_Absyn_Fndecl*_tmp145;struct Cyc_Absyn_Vardecl*_tmp147;_LL38:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp137=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp136.f1;if(_tmp137->tag != 0)goto _LL3A;}{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp138=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp136.f2;if(_tmp138->tag != 0)goto _LL3A;};_LL39:
 return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3A:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp139=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp136.f1;if(_tmp139->tag != 1)goto _LL3C;else{_tmp13A=_tmp139->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp13B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp136.f2;if(_tmp13B->tag != 1)goto _LL3C;else{_tmp13C=_tmp13B->f1;}};_LL3B: {
# 640
struct Cyc_Absyn_Vardecl*_tmp148=Cyc_Tcdecl_merge_vardecl(_tmp13A,_tmp13C,loc,msg);
if(_tmp148 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp148 == _tmp13A)return b0;
if(_tmp148 == _tmp13C)return b1;{
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp273;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp272;return(void*)((_tmp272=_cycalloc(sizeof(*_tmp272)),((_tmp272[0]=((_tmp273.tag=1,((_tmp273.f1=_tmp148,_tmp273)))),_tmp272))));};}_LL3C:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp13D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp136.f1;if(_tmp13D->tag != 1)goto _LL3E;else{_tmp13E=_tmp13D->f1;}}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp13F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp136.f2;if(_tmp13F->tag != 2)goto _LL3E;else{_tmp140=_tmp13F->f1;}};_LL3D: {
# 646
int _tmp14D;const char*_tmp274;struct _tuple10 _tmp14C=Cyc_Tcdecl_check_var_or_fn_decl(_tmp13E->sc,_tmp13E->type,_tmp13E->tq,_tmp13E->attributes,_tmp140->sc,(void*)_check_null(_tmp140->cached_typ),
Cyc_Absyn_empty_tqual(0),_tmp140->attributes,(
(_tmp274="function",_tag_dyneither(_tmp274,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp13E->name),loc,msg);_tmp14D=_tmp14C.f2;
return _tmp14D?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3E:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp141=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp136.f1;if(_tmp141->tag != 2)goto _LL40;}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp142=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp136.f2;if(_tmp142->tag != 2)goto _LL40;else{_tmp143=_tmp142->f1;}};_LL3F:
# 652
{const char*_tmp278;void*_tmp277[1];struct Cyc_String_pa_PrintArg_struct _tmp276;(_tmp276.tag=0,((_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp143->name)),((_tmp277[0]=& _tmp276,Cyc_Tcdecl_merr(loc,msg,((_tmp278="redefinition of function %s",_tag_dyneither(_tmp278,sizeof(char),28))),_tag_dyneither(_tmp277,sizeof(void*),1)))))));}
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL40:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp144=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp136.f1;if(_tmp144->tag != 2)goto _LL42;else{_tmp145=_tmp144->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp146=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp136.f2;if(_tmp146->tag != 1)goto _LL42;else{_tmp147=_tmp146->f1;}};_LL41: {
# 655
int _tmp153;const char*_tmp279;struct _tuple10 _tmp152=Cyc_Tcdecl_check_var_or_fn_decl(_tmp145->sc,(void*)_check_null(_tmp145->cached_typ),Cyc_Absyn_empty_tqual(0),_tmp145->attributes,_tmp147->sc,_tmp147->type,_tmp147->tq,_tmp147->attributes,(
# 657
(_tmp279="variable",_tag_dyneither(_tmp279,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp145->name),loc,msg);_tmp153=_tmp152.f2;
return _tmp153?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL42:;_LL43: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp27F;const char*_tmp27E;struct Cyc_Core_Invalid_argument_exn_struct*_tmp27D;(int)_throw((void*)((_tmp27D=_cycalloc(sizeof(*_tmp27D)),((_tmp27D[0]=((_tmp27F.tag=Cyc_Core_Invalid_argument,((_tmp27F.f1=((_tmp27E="Tcdecl::merge_binding",_tag_dyneither(_tmp27E,sizeof(char),22))),_tmp27F)))),_tmp27D)))));}_LL37:;}
# 664
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 667
struct Cyc_Absyn_Datatypedecl*_tmp158;struct Cyc_Absyn_Datatypefield*_tmp159;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp157=d0;_tmp158=_tmp157->base;_tmp159=_tmp157->field;{
struct Cyc_Absyn_Datatypedecl*_tmp15B;struct Cyc_Absyn_Datatypefield*_tmp15C;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp15A=d1;_tmp15B=_tmp15A->base;_tmp15C=_tmp15A->field;{
struct _dyneither_ptr _tmp15D=Cyc_Absynpp_qvar2string(_tmp159->name);
# 672
if(Cyc_Absyn_qvar_cmp(_tmp158->name,_tmp15B->name)!= 0){
{const char*_tmp285;void*_tmp284[3];struct Cyc_String_pa_PrintArg_struct _tmp283;struct Cyc_String_pa_PrintArg_struct _tmp282;struct Cyc_String_pa_PrintArg_struct _tmp281;(_tmp281.tag=0,((_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15D),((_tmp282.tag=0,((_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp15B->name)),((_tmp283.tag=0,((_tmp283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp158->name)),((_tmp284[0]=& _tmp283,((_tmp284[1]=& _tmp282,((_tmp284[2]=& _tmp281,Cyc_Tcdecl_merr(loc,msg,((_tmp285="@extensible datatype %s and %s have both a field named %s",_tag_dyneither(_tmp285,sizeof(char),58))),_tag_dyneither(_tmp284,sizeof(void*),3)))))))))))))))))));}
return 0;}
# 679
{const char*_tmp286;if(!Cyc_Tcdecl_check_tvs(_tmp158->tvs,_tmp15B->tvs,((_tmp286="@extensible datatype",_tag_dyneither(_tmp286,sizeof(char),21))),Cyc_Absynpp_qvar2string(_tmp158->name),loc,msg))
return 0;}{
# 682
struct Cyc_List_List*_tmp164=Cyc_Tcdecl_build_tvs_map(_tmp158->tvs,_tmp15B->tvs);
# 685
const char*_tmp287;struct Cyc_Absyn_Datatypefield*_tmp165=Cyc_Tcdecl_merge_datatypefield(_tmp159,_tmp15C,_tmp164,((_tmp287="@extensible datatype field",_tag_dyneither(_tmp287,sizeof(char),27))),_tmp15D,msg);
if(_tmp165 == 0)return 0;
if(_tmp165 == _tmp159)return d0;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp288;return(_tmp288=_cycalloc(sizeof(*_tmp288)),((_tmp288->base=_tmp158,((_tmp288->field=_tmp165,_tmp288)))));};};};};}
