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
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
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
# 134 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 146 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 254 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 321
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;
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
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void _tmp17C(struct _dyneither_ptr**msg1,struct _dyneither_ptr*ap,unsigned int*_tmp17B,unsigned int*_tmp17A,void***_tmp178){for(*_tmp17B=0;*_tmp17B < *_tmp17A;(*_tmp17B)++){struct Cyc_String_pa_PrintArg_struct _tmp176;struct Cyc_String_pa_PrintArg_struct*_tmp175;(*_tmp178)[*_tmp17B]=(void*)(
# 57
*_tmp17B == 0?(void*)((_tmp175=_cycalloc(sizeof(*_tmp175)),((_tmp175[0]=((_tmp176.tag=0,((_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),_tmp176)))),_tmp175)))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(int)(*_tmp17B - 1))));}}
# 45
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 49
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(Cyc_strlen((struct _dyneither_ptr)*msg1)== 0){
const char*_tmp172;fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp172="%s",_tag_dyneither(_tmp172,sizeof(char),3))),(struct _dyneither_ptr)fmt);}else{
# 55
const char*_tmp173;fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp173="%s ",_tag_dyneither(_tmp173,sizeof(char),4))),(struct _dyneither_ptr)fmt);}{
unsigned int _tmp17B;unsigned int _tmp17A;struct _dyneither_ptr _tmp179;void**_tmp178;unsigned int _tmp177;struct _dyneither_ptr ap2=(_tmp177=_get_dyneither_size(ap,sizeof(void*))+ 1,((_tmp178=(void**)_cycalloc(_check_times(sizeof(void*),_tmp177)),((_tmp179=_tag_dyneither(_tmp178,sizeof(void*),_tmp177),((((_tmp17A=_tmp177,_tmp17C(& msg1,& ap,& _tmp17B,& _tmp17A,& _tmp178))),_tmp179)))))));
# 58
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)
Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};};}
# 64
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 66
const char*_tmp17D;struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?(_tmp17D="public ",_tag_dyneither(_tmp17D,sizeof(char),8)): Cyc_Absynpp_scope2string(s0);
const char*_tmp17E;struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?(_tmp17E="public ",_tag_dyneither(_tmp17E,sizeof(char),8)): Cyc_Absynpp_scope2string(s1);
const char*_tmp185;void*_tmp184[4];struct Cyc_String_pa_PrintArg_struct _tmp183;struct Cyc_String_pa_PrintArg_struct _tmp182;struct Cyc_String_pa_PrintArg_struct _tmp181;struct Cyc_String_pa_PrintArg_struct _tmp180;(_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str),((_tmp181.tag=0,((_tmp181.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str),((_tmp182.tag=0,((_tmp182.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp183.tag=0,((_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp184[0]=& _tmp183,((_tmp184[1]=& _tmp182,((_tmp184[2]=& _tmp181,((_tmp184[3]=& _tmp180,Cyc_Tcdecl_merr(loc,msg,((_tmp185="%s %s is %sbut expected scope is %s",_tag_dyneither(_tmp185,sizeof(char),36))),_tag_dyneither(_tmp184,sizeof(void*),4)))))))))))))))))))))))));}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 72
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 74
{struct _tuple11 _tmp186;struct _tuple11 _stmttmp0=(_tmp186.f1=s0,((_tmp186.f2=s1,_tmp186)));struct _tuple11 _tmp11=_stmttmp0;_LL1: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL3;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL3;_LL2:
# 76
 goto _LL0;_LL3: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL5;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL5;_LL4:
# 78
 goto _LL6;_LL5: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LL7;if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL7;_LL6:
 goto _LL8;_LL7: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL9;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LL9;_LL8:
 goto _LLA;_LL9: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LLB;if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLB;_LLA:
 if(externCmerge)goto _LL0;
goto _LLC;_LLB: if(_tmp11.f1 != Cyc_Absyn_ExternC)goto _LLD;_LLC:
 goto _LLE;_LLD: if(_tmp11.f2 != Cyc_Absyn_ExternC)goto _LLF;_LLE:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp187;return(_tmp187.f1=s1,((_tmp187.f2=0,_tmp187)));};_LLF: if(_tmp11.f2 != Cyc_Absyn_Extern)goto _LL11;_LL10:
# 86
 s1=s0;goto _LL0;_LL11: if(_tmp11.f1 != Cyc_Absyn_Extern)goto _LL13;_LL12:
 goto _LL0;_LL13: if(_tmp11.f1 != Cyc_Absyn_Static)goto _LL15;if(_tmp11.f2 != Cyc_Absyn_Static)goto _LL15;_LL14:
# 89
 goto _LL16;_LL15: if(_tmp11.f1 != Cyc_Absyn_Public)goto _LL17;if(_tmp11.f2 != Cyc_Absyn_Public)goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp11.f1 != Cyc_Absyn_Abstract)goto _LL19;if(_tmp11.f2 != Cyc_Absyn_Abstract)goto _LL19;_LL18:
 goto _LL0;_LL19: if(_tmp11.f1 != Cyc_Absyn_Register)goto _LL1B;if(_tmp11.f2 != Cyc_Absyn_Register)goto _LL1B;_LL1A:
 goto _LL0;_LL1B:;_LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple10 _tmp188;return(_tmp188.f1=s1,((_tmp188.f2=0,_tmp188)));};_LL0:;}{
# 95
struct _tuple10 _tmp189;return(_tmp189.f1=s1,((_tmp189.f2=1,_tmp189)));};}
# 98
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 100
return Cyc_Tcutil_unify(t0,t1);}
# 104
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_stmttmp1=e->r;void*_tmp16=_stmttmp1;int _tmp18;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp17=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16;if(_tmp17->tag != 0)goto _LL20;else{if(((_tmp17->f1).Int_c).tag != 5)goto _LL20;_tmp18=((struct _tuple5)((_tmp17->f1).Int_c).val).f2;}}_LL1F:
 return(unsigned int)_tmp18;_LL20:;_LL21: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp18F;const char*_tmp18E;struct Cyc_Core_Invalid_argument_exn_struct*_tmp18D;(int)_throw((void*)((_tmp18D=_cycalloc(sizeof(*_tmp18D)),((_tmp18D[0]=((_tmp18F.tag=Cyc_Core_Invalid_argument,((_tmp18F.f1=((_tmp18E="Tcdecl::get_uint_const_value",_tag_dyneither(_tmp18E,sizeof(char),29))),_tmp18F)))),_tmp18D)))));}_LL1D:;}
# 111
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 114
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){
{const char*_tmp194;void*_tmp193[2];struct Cyc_String_pa_PrintArg_struct _tmp192;struct Cyc_String_pa_PrintArg_struct _tmp191;(_tmp191.tag=0,((_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp192.tag=0,((_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp193[0]=& _tmp192,((_tmp193[1]=& _tmp191,Cyc_Tcdecl_merr(loc,msg,((_tmp194="%s %s has a different number of type parameters",_tag_dyneither(_tmp194,sizeof(char),48))),_tag_dyneither(_tmp193,sizeof(void*),2)))))))))))));}
return 0;}{
# 118
struct Cyc_List_List*_tmp20=tvs0;
struct Cyc_List_List*_tmp21=tvs1;
for(0;_tmp20 != 0;(_tmp20=_tmp20->tl,_tmp21=_tmp21->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp20->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp21))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp22=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp20->hd)->kind);
struct Cyc_Absyn_Kind*_tmp23=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp21->hd)->kind);
if(_tmp22 == _tmp23)continue;
{const char*_tmp19C;void*_tmp19B[5];struct Cyc_String_pa_PrintArg_struct _tmp19A;struct Cyc_String_pa_PrintArg_struct _tmp199;struct Cyc_String_pa_PrintArg_struct _tmp198;struct Cyc_String_pa_PrintArg_struct _tmp197;struct Cyc_String_pa_PrintArg_struct _tmp196;(_tmp196.tag=0,((_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp23)),((_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp20->hd)->name),((_tmp198.tag=0,((_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp22)),((_tmp199.tag=0,((_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp19A.tag=0,((_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp19B[0]=& _tmp19A,((_tmp19B[1]=& _tmp199,((_tmp19B[2]=& _tmp198,((_tmp19B[3]=& _tmp197,((_tmp19B[4]=& _tmp196,Cyc_Tcdecl_merr(loc,msg,((_tmp19C="%s %s has a different kind (%s) for type parameter %s (%s)",_tag_dyneither(_tmp19C,sizeof(char),59))),_tag_dyneither(_tmp19B,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;};}
# 129
return 1;};}
# 132
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 134
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp1A1;void*_tmp1A0[2];struct Cyc_String_pa_PrintArg_struct _tmp19F;struct Cyc_String_pa_PrintArg_struct _tmp19E;(_tmp19E.tag=0,((_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp19F.tag=0,((_tmp19F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1A0[0]=& _tmp19F,((_tmp1A0[1]=& _tmp19E,Cyc_Tcdecl_merr(loc,msg,((_tmp1A1="%s %s has different attributes",_tag_dyneither(_tmp1A1,sizeof(char),31))),_tag_dyneither(_tmp1A0,sizeof(void*),2)))))))))))));}
return 0;}
# 138
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 141
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 143
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
struct _tuple12*_tmp1AB;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1AA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A9;struct Cyc_List_List*_tmp1A8;inst=((_tmp1A8=_cycalloc(sizeof(*_tmp1A8)),((_tmp1A8->hd=((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,((_tmp1AB->f2=(void*)((_tmp1A9=_cycalloc(sizeof(*_tmp1A9)),((_tmp1A9[0]=((_tmp1AA.tag=2,((_tmp1AA.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1AA)))),_tmp1A9)))),_tmp1AB)))))),((_tmp1A8->tl=inst,_tmp1A8))))));}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 149
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 151
struct _dyneither_ptr _tmp33=Cyc_Absynpp_qvar2string(d0->name);
int _tmp34=1;
# 157
if(!(d0->kind == d1->kind))return 0;
# 160
{const char*_tmp1AC;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp1AC="type",_tag_dyneither(_tmp1AC,sizeof(char),5))),_tmp33,loc,msg))return 0;}{
# 163
const char*_tmp1AD;struct _tuple10 _stmttmp2=
Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp1AD="type",_tag_dyneither(_tmp1AD,sizeof(char),5))),_tmp33,loc,msg,1);
# 163
enum Cyc_Absyn_Scope _tmp37;int _tmp38;struct _tuple10 _tmp36=_stmttmp2;_tmp37=_tmp36.f1;_tmp38=_tmp36.f2;
# 165
if(!_tmp38)_tmp34=0;
# 168
{const char*_tmp1AE;if(!Cyc_Tcdecl_check_atts(d0->attributes,d1->attributes,((_tmp1AE="type",_tag_dyneither(_tmp1AE,sizeof(char),5))),_tmp33,loc,msg))_tmp34=0;}{
# 170
struct Cyc_Absyn_Aggrdecl*d2;
# 172
{struct _tuple13 _tmp1AF;struct _tuple13 _stmttmp3=(_tmp1AF.f1=d0->impl,((_tmp1AF.f2=d1->impl,_tmp1AF)));struct _tuple13 _tmp3A=_stmttmp3;struct Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3C;struct Cyc_List_List*_tmp3D;int _tmp3E;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;int _tmp42;_LL23: if(_tmp3A.f2 != 0)goto _LL25;_LL24:
 d2=d0;goto _LL22;_LL25: if(_tmp3A.f1 != 0)goto _LL27;_LL26:
 d2=d1;goto _LL22;_LL27: if(_tmp3A.f1 == 0)goto _LL22;_tmp3B=(_tmp3A.f1)->exist_vars;_tmp3C=(_tmp3A.f1)->rgn_po;_tmp3D=(_tmp3A.f1)->fields;_tmp3E=(_tmp3A.f1)->tagged;if(_tmp3A.f2 == 0)goto _LL22;_tmp3F=(_tmp3A.f2)->exist_vars;_tmp40=(_tmp3A.f2)->rgn_po;_tmp41=(_tmp3A.f2)->fields;_tmp42=(_tmp3A.f2)->tagged;_LL28:
# 179
{const char*_tmp1B0;if(!Cyc_Tcdecl_check_tvs(_tmp3B,_tmp3F,((_tmp1B0="type",_tag_dyneither(_tmp1B0,sizeof(char),5))),_tmp33,loc,msg))
return 0;}{
# 183
struct Cyc_List_List*_tmp44=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp3B),
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp3F));
# 188
for(0;_tmp3C != 0  && _tmp40 != 0;
(_tmp3C=_tmp3C->tl,_tmp40=_tmp40->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3C->hd)).f1,(*((struct _tuple14*)_tmp40->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp3C->hd)).f2,(*((struct _tuple14*)_tmp40->hd)).f2);}
# 195
for(0;_tmp3D != 0  && _tmp41 != 0;(_tmp3D=_tmp3D->tl,_tmp41=_tmp41->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmp4=(struct Cyc_Absyn_Aggrfield*)_tmp3D->hd;struct _dyneither_ptr*_tmp46;struct Cyc_Absyn_Tqual _tmp47;void*_tmp48;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_List_List*_tmp4A;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Aggrfield*_tmp45=_stmttmp4;_tmp46=_tmp45->name;_tmp47=_tmp45->tq;_tmp48=_tmp45->type;_tmp49=_tmp45->width;_tmp4A=_tmp45->attributes;_tmp4B=_tmp45->requires_clause;{
struct Cyc_Absyn_Aggrfield*_stmttmp5=(struct Cyc_Absyn_Aggrfield*)_tmp41->hd;struct _dyneither_ptr*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;void*_tmp4F;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Aggrfield*_tmp4C=_stmttmp5;_tmp4D=_tmp4C->name;_tmp4E=_tmp4C->tq;_tmp4F=_tmp4C->type;_tmp50=_tmp4C->width;_tmp51=_tmp4C->attributes;_tmp52=_tmp4C->requires_clause;
# 199
if(Cyc_strptrcmp(_tmp46,_tmp4D)!= 0){
{const char*_tmp1B9;void*_tmp1B8[4];const char*_tmp1B7;struct Cyc_String_pa_PrintArg_struct _tmp1B6;struct Cyc_String_pa_PrintArg_struct _tmp1B5;struct Cyc_String_pa_PrintArg_struct _tmp1B4;struct Cyc_String_pa_PrintArg_struct _tmp1B3;(_tmp1B3.tag=0,((_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D),((_tmp1B4.tag=0,((_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1B5.tag=0,((_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1B6.tag=0,((_tmp1B6.f1=(struct _dyneither_ptr)((_tmp1B7="type",_tag_dyneither(_tmp1B7,sizeof(char),5))),((_tmp1B8[0]=& _tmp1B6,((_tmp1B8[1]=& _tmp1B5,((_tmp1B8[2]=& _tmp1B4,((_tmp1B8[3]=& _tmp1B3,Cyc_Tcdecl_merr(loc,msg,((_tmp1B9="%s %s : field name mismatch %s != %s",_tag_dyneither(_tmp1B9,sizeof(char),37))),_tag_dyneither(_tmp1B8,sizeof(void*),4)))))))))))))))))))))))));}
# 202
return 0;}
# 204
if(!Cyc_Tcutil_same_atts(_tmp4A,_tmp51)){
{const char*_tmp1C1;void*_tmp1C0[3];const char*_tmp1BF;struct Cyc_String_pa_PrintArg_struct _tmp1BE;struct Cyc_String_pa_PrintArg_struct _tmp1BD;struct Cyc_String_pa_PrintArg_struct _tmp1BC;(_tmp1BC.tag=0,((_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1BD.tag=0,((_tmp1BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1BE.tag=0,((_tmp1BE.f1=(struct _dyneither_ptr)((_tmp1BF="type",_tag_dyneither(_tmp1BF,sizeof(char),5))),((_tmp1C0[0]=& _tmp1BE,((_tmp1C0[1]=& _tmp1BD,((_tmp1C0[2]=& _tmp1BC,Cyc_Tcdecl_merr(loc,msg,((_tmp1C1="%s %s : attribute mismatch on field %s",_tag_dyneither(_tmp1C1,sizeof(char),39))),_tag_dyneither(_tmp1C0,sizeof(void*),3)))))))))))))))))));}
_tmp34=0;}
# 208
if(!Cyc_Tcutil_equal_tqual(_tmp47,_tmp4E)){
{const char*_tmp1C9;void*_tmp1C8[3];const char*_tmp1C7;struct Cyc_String_pa_PrintArg_struct _tmp1C6;struct Cyc_String_pa_PrintArg_struct _tmp1C5;struct Cyc_String_pa_PrintArg_struct _tmp1C4;(_tmp1C4.tag=0,((_tmp1C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1C5.tag=0,((_tmp1C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1C6.tag=0,((_tmp1C6.f1=(struct _dyneither_ptr)((_tmp1C7="type",_tag_dyneither(_tmp1C7,sizeof(char),5))),((_tmp1C8[0]=& _tmp1C6,((_tmp1C8[1]=& _tmp1C5,((_tmp1C8[2]=& _tmp1C4,Cyc_Tcdecl_merr(loc,msg,((_tmp1C9="%s %s : qualifier mismatch on field %s",_tag_dyneither(_tmp1C9,sizeof(char),39))),_tag_dyneither(_tmp1C8,sizeof(void*),3)))))))))))))))))));}
_tmp34=0;}
# 212
if(((_tmp49 != 0  && _tmp50 != 0) && 
Cyc_Tcdecl_get_uint_const_value(_tmp49)!= 
Cyc_Tcdecl_get_uint_const_value(_tmp50) || 
_tmp49 == 0  && _tmp50 != 0) || 
_tmp49 != 0  && _tmp50 == 0){
{const char*_tmp1D1;void*_tmp1D0[3];const char*_tmp1CF;struct Cyc_String_pa_PrintArg_struct _tmp1CE;struct Cyc_String_pa_PrintArg_struct _tmp1CD;struct Cyc_String_pa_PrintArg_struct _tmp1CC;(_tmp1CC.tag=0,((_tmp1CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1CD.tag=0,((_tmp1CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1CE.tag=0,((_tmp1CE.f1=(struct _dyneither_ptr)((_tmp1CF="type",_tag_dyneither(_tmp1CF,sizeof(char),5))),((_tmp1D0[0]=& _tmp1CE,((_tmp1D0[1]=& _tmp1CD,((_tmp1D0[2]=& _tmp1CC,Cyc_Tcdecl_merr(loc,msg,((_tmp1D1="%s %s : bitfield mismatch on field %s",_tag_dyneither(_tmp1D1,sizeof(char),38))),_tag_dyneither(_tmp1D0,sizeof(void*),3)))))))))))))))))));}
_tmp34=0;}
# 220
if(_tmp4B != 0  && _tmp52 != 0){
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1D4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D3;void*subst_w1=Cyc_Tcutil_substitute(_tmp44,(void*)((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3[0]=((_tmp1D4.tag=18,((_tmp1D4.f1=_tmp52,_tmp1D4)))),_tmp1D3)))));
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1D7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D6;if(!Cyc_Tcdecl_check_type((void*)((_tmp1D6=_cycalloc(sizeof(*_tmp1D6)),((_tmp1D6[0]=((_tmp1D7.tag=18,((_tmp1D7.f1=_tmp4B,_tmp1D7)))),_tmp1D6)))),subst_w1)){
{const char*_tmp1DD;void*_tmp1DC[3];struct Cyc_String_pa_PrintArg_struct _tmp1DB;struct Cyc_String_pa_PrintArg_struct _tmp1DA;struct Cyc_String_pa_PrintArg_struct _tmp1D9;(_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp52)),((_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp4B)),((_tmp1DB.tag=0,((_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1DC[0]=& _tmp1DB,((_tmp1DC[1]=& _tmp1DA,((_tmp1DC[2]=& _tmp1D9,Cyc_Tcdecl_merr(loc,msg,((_tmp1DD="type %s : mismatch on @requires clauses: %s != %s",_tag_dyneither(_tmp1DD,sizeof(char),50))),_tag_dyneither(_tmp1DC,sizeof(void*),3)))))))))))))))))));}
_tmp34=0;}}else{
# 227
if(_tmp4B == 0  && _tmp52 != 0  || 
_tmp4B != 0  && _tmp52 == 0){
{const char*_tmp1E1;void*_tmp1E0[1];struct Cyc_String_pa_PrintArg_struct _tmp1DF;(_tmp1DF.tag=0,((_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1E0[0]=& _tmp1DF,Cyc_Tcdecl_merr(loc,msg,((_tmp1E1="type %s : mismatch on @requires clauses",_tag_dyneither(_tmp1E1,sizeof(char),40))),_tag_dyneither(_tmp1E0,sizeof(void*),1)))))));}
_tmp34=0;}}{
# 233
void*subst_t1=Cyc_Tcutil_substitute(_tmp44,_tmp4F);
if(!Cyc_Tcdecl_check_type(_tmp48,subst_t1)){
{const char*_tmp1E8;void*_tmp1E7[4];struct Cyc_String_pa_PrintArg_struct _tmp1E6;struct Cyc_String_pa_PrintArg_struct _tmp1E5;struct Cyc_String_pa_PrintArg_struct _tmp1E4;struct Cyc_String_pa_PrintArg_struct _tmp1E3;(_tmp1E3.tag=0,((_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp48)),((_tmp1E5.tag=0,((_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46),((_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1E7[0]=& _tmp1E6,((_tmp1E7[1]=& _tmp1E5,((_tmp1E7[2]=& _tmp1E4,((_tmp1E7[3]=& _tmp1E3,Cyc_Tcdecl_merr(loc,msg,((_tmp1E8="type %s : type mismatch on field %s: %s != %s",_tag_dyneither(_tmp1E8,sizeof(char),46))),_tag_dyneither(_tmp1E7,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp34=0;}};};}
# 242
if(_tmp3D != 0){
{const char*_tmp1ED;void*_tmp1EC[2];struct Cyc_String_pa_PrintArg_struct _tmp1EB;struct Cyc_String_pa_PrintArg_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp3D->hd)->name),((_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1EC[0]=& _tmp1EB,((_tmp1EC[1]=& _tmp1EA,Cyc_Tcdecl_merr(loc,msg,((_tmp1ED="type %s is missing field %s",_tag_dyneither(_tmp1ED,sizeof(char),28))),_tag_dyneither(_tmp1EC,sizeof(void*),2)))))))))))));}
_tmp34=0;}
# 246
if(_tmp41 != 0){
{const char*_tmp1F2;void*_tmp1F1[2];struct Cyc_String_pa_PrintArg_struct _tmp1F0;struct Cyc_String_pa_PrintArg_struct _tmp1EF;(_tmp1EF.tag=0,((_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp41->hd)->name),((_tmp1F0.tag=0,((_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1F1[0]=& _tmp1F0,((_tmp1F1[1]=& _tmp1EF,Cyc_Tcdecl_merr(loc,msg,((_tmp1F2="type %s has extra field %s",_tag_dyneither(_tmp1F2,sizeof(char),27))),_tag_dyneither(_tmp1F1,sizeof(void*),2)))))))))))));}
_tmp34=0;}
# 251
if(_tmp3E != _tmp42){
{const char*_tmp1F6;void*_tmp1F5[1];struct Cyc_String_pa_PrintArg_struct _tmp1F4;(_tmp1F4.tag=0,((_tmp1F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33),((_tmp1F5[0]=& _tmp1F4,Cyc_Tcdecl_merr(loc,msg,((_tmp1F6="%s : previous declaration disagrees with @tagged qualifier",_tag_dyneither(_tmp1F6,sizeof(char),59))),_tag_dyneither(_tmp1F5,sizeof(void*),1)))))));}
_tmp34=0;}
# 256
d2=d0;
goto _LL22;};_LL22:;}
# 260
if(!_tmp34)return 0;
# 262
if(_tmp37 == d2->sc)
return d2;else{
# 265
{struct Cyc_Absyn_Aggrdecl*_tmp1F7;d2=((_tmp1F7=_cycalloc(sizeof(*_tmp1F7)),((_tmp1F7[0]=*d2,_tmp1F7))));}
d2->sc=_tmp37;
return d2;}};};}
# 271
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
const char*_tmp1F9;const char*_tmp1F8;return is_x?(_tmp1F9="@extensible datatype",_tag_dyneither(_tmp1F9,sizeof(char),21)):((_tmp1F8="datatype",_tag_dyneither(_tmp1F8,sizeof(char),9)));}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 277
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 281
unsigned int loc=f1->loc;
# 284
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
{const char*_tmp200;void*_tmp1FF[4];struct Cyc_String_pa_PrintArg_struct _tmp1FE;struct Cyc_String_pa_PrintArg_struct _tmp1FD;struct Cyc_String_pa_PrintArg_struct _tmp1FC;struct Cyc_String_pa_PrintArg_struct _tmp1FB;(_tmp1FB.tag=0,((_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp1FC.tag=0,((_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp1FD.tag=0,((_tmp1FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1FF[0]=& _tmp1FE,((_tmp1FF[1]=& _tmp1FD,((_tmp1FF[2]=& _tmp1FC,((_tmp1FF[3]=& _tmp1FB,Cyc_Tcdecl_merr(loc,msg,((_tmp200="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp200,sizeof(char),36))),_tag_dyneither(_tmp1FF,sizeof(void*),4)))))))))))))))))))))))));}
# 287
return 0;}{
# 290
struct _dyneither_ptr _tmp94=*(*f0->name).f2;
# 293
const char*_tmp205;void*_tmp204[2];struct Cyc_String_pa_PrintArg_struct _tmp203;struct Cyc_String_pa_PrintArg_struct _tmp202;struct _tuple10 _stmttmp6=
Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct _dyneither_ptr)((_tmp202.tag=0,((_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp204[0]=& _tmp203,((_tmp204[1]=& _tmp202,Cyc_aprintf(((_tmp205="in %s %s, field",_tag_dyneither(_tmp205,sizeof(char),16))),_tag_dyneither(_tmp204,sizeof(void*),2)))))))))))))),_tmp94,loc,msg,0);
# 293
enum Cyc_Absyn_Scope _tmp96;int _tmp97;struct _tuple10 _tmp95=_stmttmp6;_tmp96=_tmp95.f1;_tmp97=_tmp95.f2;{
# 297
struct Cyc_List_List*_tmp98=f0->typs;
struct Cyc_List_List*_tmp99=f1->typs;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp98)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp99)){
{const char*_tmp20B;void*_tmp20A[3];struct Cyc_String_pa_PrintArg_struct _tmp209;struct Cyc_String_pa_PrintArg_struct _tmp208;struct Cyc_String_pa_PrintArg_struct _tmp207;(_tmp207.tag=0,((_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp94),((_tmp208.tag=0,((_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp209.tag=0,((_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp20A[0]=& _tmp209,((_tmp20A[1]=& _tmp208,((_tmp20A[2]=& _tmp207,Cyc_Tcdecl_merr(loc,msg,((_tmp20B="%s %s, field %s: parameter number mismatch",_tag_dyneither(_tmp20B,sizeof(char),43))),_tag_dyneither(_tmp20A,sizeof(void*),3)))))))))))))))))));}
_tmp97=0;}
# 304
for(0;_tmp98 != 0  && _tmp99 != 0;(_tmp98=_tmp98->tl,_tmp99=_tmp99->tl)){
# 306
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp98->hd)).f1,(*((struct _tuple15*)_tmp99->hd)).f1)){
{const char*_tmp211;void*_tmp210[3];struct Cyc_String_pa_PrintArg_struct _tmp20F;struct Cyc_String_pa_PrintArg_struct _tmp20E;struct Cyc_String_pa_PrintArg_struct _tmp20D;(_tmp20D.tag=0,((_tmp20D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp94),((_tmp20E.tag=0,((_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp20F.tag=0,((_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp210[0]=& _tmp20F,((_tmp210[1]=& _tmp20E,((_tmp210[2]=& _tmp20D,Cyc_Tcdecl_merr(loc,msg,((_tmp211="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp211,sizeof(char),37))),_tag_dyneither(_tmp210,sizeof(void*),3)))))))))))))))))));}
_tmp97=0;}{
# 310
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp99->hd)).f2);
# 312
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp98->hd)).f2,subst_t1)){
{const char*_tmp219;void*_tmp218[5];struct Cyc_String_pa_PrintArg_struct _tmp217;struct Cyc_String_pa_PrintArg_struct _tmp216;struct Cyc_String_pa_PrintArg_struct _tmp215;struct Cyc_String_pa_PrintArg_struct _tmp214;struct Cyc_String_pa_PrintArg_struct _tmp213;(_tmp213.tag=0,((_tmp213.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1)),((_tmp214.tag=0,((_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp98->hd)).f2)),((_tmp215.tag=0,((_tmp215.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp94),((_tmp216.tag=0,((_tmp216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp217.tag=0,((_tmp217.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp218[0]=& _tmp217,((_tmp218[1]=& _tmp216,((_tmp218[2]=& _tmp215,((_tmp218[3]=& _tmp214,((_tmp218[4]=& _tmp213,Cyc_Tcdecl_merr(loc,msg,((_tmp219="%s %s, field %s: parameter type mismatch %s != %s",_tag_dyneither(_tmp219,sizeof(char),50))),_tag_dyneither(_tmp218,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();
_tmp97=0;}};}
# 320
if(!_tmp97)return 0;
if(f0->sc != _tmp96){
struct Cyc_Absyn_Datatypefield*_tmp21A;struct Cyc_Absyn_Datatypefield*_tmpAB=(_tmp21A=_cycalloc(sizeof(*_tmp21A)),((_tmp21A[0]=*f0,_tmp21A)));
_tmpAB->sc=_tmp96;
return _tmpAB;}else{
return f0;}};};}
# 328
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 331
struct Cyc_Absyn_Tqual _tmpB2;void*_tmpB3;struct _tuple15*_tmpB1=x;_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;{
struct _tuple15*_tmp21B;return(_tmp21B=_cycalloc(sizeof(*_tmp21B)),((_tmp21B->f1=_tmpB2,((_tmp21B->f2=Cyc_Tcutil_substitute(inst,_tmpB3),_tmp21B)))));};}
# 337
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 339
struct Cyc_Absyn_Datatypefield*_tmp21C;struct Cyc_Absyn_Datatypefield*_tmpB5=(_tmp21C=_cycalloc(sizeof(*_tmp21C)),((_tmp21C[0]=*f1,_tmp21C)));
_tmpB5->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);
# 342
return _tmpB5;}
# 349
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 358
struct Cyc_List_List**_tmp21D;struct Cyc_List_List**f2sp=(_tmp21D=_cycalloc(sizeof(*_tmp21D)),((_tmp21D[0]=0,_tmp21D)));
struct Cyc_List_List**_tmpB7=f2sp;
int cmp=-1;
# 362
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){
struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmpB8=(_tmp21E=_cycalloc(sizeof(*_tmp21E)),((_tmp21E->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp21E->tl=0,_tmp21E)))));
*_tmpB7=_tmpB8;
_tmpB7=& _tmpB8->tl;
f0s=f0s->tl;}
# 369
if(f0s == 0  || cmp > 0){
# 371
*incl=0;{
struct Cyc_List_List*_tmp21F;struct Cyc_List_List*_tmpBA=(_tmp21F=_cycalloc(sizeof(*_tmp21F)),((_tmp21F->hd=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp21F->tl=0,_tmp21F)))));
*_tmpB7=_tmpBA;
_tmpB7=& _tmpBA->tl;};}else{
# 377
struct Cyc_Absyn_Datatypefield*_tmpBC=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpBC != 0){
if(_tmpBC != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmpBD=(_tmp220=_cycalloc(sizeof(*_tmp220)),((_tmp220->hd=_tmpBC,((_tmp220->tl=0,_tmp220)))));
*_tmpB7=_tmpBD;
_tmpB7=& _tmpBD->tl;};}else{
# 384
*res=0;}
# 386
f0s=f0s->tl;}}
# 391
if(f1s != 0){
*incl=0;
*_tmpB7=f1s;}else{
# 395
*_tmpB7=f0s;}
# 398
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 402
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0){struct _tuple16 _tmp221;return(_tmp221.f1=0,((_tmp221.f2=0,_tmp221)));}
if(f->tl == 0){struct _tuple16 _tmp222;return(_tmp222.f1=f,((_tmp222.f2=0,_tmp222)));}{
struct _tuple16 _stmttmp7=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct _tuple16 _tmpC2=_stmttmp7;_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;{
struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp226;struct _tuple16 _tmp225;return(_tmp225.f1=((_tmp226=_cycalloc(sizeof(*_tmp226)),((_tmp226->hd=f->hd,((_tmp226->tl=_tmpC3,_tmp226)))))),((_tmp225.f2=((_tmp227=_cycalloc(sizeof(*_tmp227)),((_tmp227->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,((_tmp227->tl=_tmpC4,_tmp227)))))),_tmp225)));};};}
# 411
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 414
struct _tuple16 _stmttmp8=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpCA;struct _tuple16 _tmpC8=_stmttmp8;_tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;
if(_tmpC9 != 0  && _tmpC9->tl != 0)
_tmpC9=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC9,res,v,loc,msg);
if(_tmpCA != 0  && _tmpCA->tl != 0)
_tmpCA=Cyc_Tcdecl_sort_xdatatype_fields(_tmpCA,res,v,loc,msg);{
const char*_tmp229;int*_tmp228;return Cyc_Tcdecl_merge_xdatatype_fields(_tmpC9,_tmpCA,0,0,0,res,((_tmp228=_cycalloc_atomic(sizeof(*_tmp228)),((_tmp228[0]=1,_tmp228)))),(
(_tmp229="@extensible datatype",_tag_dyneither(_tmp229,sizeof(char),21))),*v,loc,msg);};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 423
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 425
struct _dyneither_ptr _tmpCD=Cyc_Absynpp_qvar2string(d0->name);
const char*_tmp22A;struct _dyneither_ptr t=(_tmp22A="datatype",_tag_dyneither(_tmp22A,sizeof(char),9));
int _tmpCE=1;
# 432
if(d0->is_extensible != d1->is_extensible){
{const char*_tmp230;void*_tmp22F[3];struct Cyc_String_pa_PrintArg_struct _tmp22E;struct Cyc_String_pa_PrintArg_struct _tmp22D;struct Cyc_String_pa_PrintArg_struct _tmp22C;(_tmp22C.tag=0,((_tmp22C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible)),((_tmp22D.tag=0,((_tmp22D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible)),((_tmp22E.tag=0,((_tmp22E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCD),((_tmp22F[0]=& _tmp22E,((_tmp22F[1]=& _tmp22D,((_tmp22F[2]=& _tmp22C,Cyc_Tcdecl_merr(loc,msg,((_tmp230="expected %s to be a %s instead of a %s",_tag_dyneither(_tmp230,sizeof(char),39))),_tag_dyneither(_tmp22F,sizeof(void*),3)))))))))))))))))));}
_tmpCE=0;}else{
# 437
t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}
# 441
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpCD,loc,msg))return 0;{
# 444
struct _tuple10 _stmttmp9=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpCD,loc,msg,0);enum Cyc_Absyn_Scope _tmpD5;int _tmpD6;struct _tuple10 _tmpD4=_stmttmp9;_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;
if(!_tmpD6)_tmpCE=0;{
# 447
struct Cyc_Absyn_Datatypedecl*d2;
# 450
{struct _tuple17 _tmp231;struct _tuple17 _stmttmpA=(_tmp231.f1=d0->fields,((_tmp231.f2=d1->fields,_tmp231)));struct _tuple17 _tmpD7=_stmttmpA;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD9;_LL2A: if(_tmpD7.f2 != 0)goto _LL2C;_LL2B:
 d2=d0;goto _LL29;_LL2C: if(_tmpD7.f1 != 0)goto _LL2E;_LL2D:
 d2=d1;goto _LL29;_LL2E: if(_tmpD7.f1 == 0)goto _LL29;_tmpD8=(struct Cyc_List_List*)(_tmpD7.f1)->v;if(_tmpD7.f2 == 0)goto _LL29;_tmpD9=(struct Cyc_List_List*)(_tmpD7.f2)->v;_LL2F: {
# 464 "tcdecl.cyc"
struct Cyc_List_List*_tmpDA=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 467
if(d0->is_extensible){
# 471
int _tmpDB=1;
struct Cyc_List_List*_tmpDC=Cyc_Tcdecl_merge_xdatatype_fields(_tmpD8,_tmpD9,_tmpDA,d0->tvs,d1->tvs,& _tmpCE,& _tmpDB,t,_tmpCD,loc,msg);
# 475
if(_tmpDB)
d2=d0;else{
# 478
{struct Cyc_Absyn_Datatypedecl*_tmp232;d2=((_tmp232=_cycalloc(sizeof(*_tmp232)),((_tmp232[0]=*d0,_tmp232))));}
d2->sc=_tmpD5;{
struct Cyc_Core_Opt*_tmp233;d2->fields=((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233->v=_tmpDC,_tmp233))));};}}else{
# 484
for(0;_tmpD8 != 0  && _tmpD9 != 0;(_tmpD8=_tmpD8->tl,_tmpD9=_tmpD9->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpD8->hd,(struct Cyc_Absyn_Datatypefield*)_tmpD9->hd,_tmpDA,t,_tmpCD,msg);}
# 487
if(_tmpD8 != 0){
{const char*_tmp239;void*_tmp238[3];struct Cyc_String_pa_PrintArg_struct _tmp237;struct Cyc_String_pa_PrintArg_struct _tmp236;struct Cyc_String_pa_PrintArg_struct _tmp235;(_tmp235.tag=0,((_tmp235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpD8->hd)->name).f2),((_tmp236.tag=0,((_tmp236.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCD),((_tmp237.tag=0,((_tmp237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp238[0]=& _tmp237,((_tmp238[1]=& _tmp236,((_tmp238[2]=& _tmp235,Cyc_Tcdecl_merr(loc,msg,((_tmp239="%s %s is missing field %s",_tag_dyneither(_tmp239,sizeof(char),26))),_tag_dyneither(_tmp238,sizeof(void*),3)))))))))))))))))));}
_tmpCE=0;}
# 491
if(_tmpD9 != 0){
{const char*_tmp23F;void*_tmp23E[3];struct Cyc_String_pa_PrintArg_struct _tmp23D;struct Cyc_String_pa_PrintArg_struct _tmp23C;struct Cyc_String_pa_PrintArg_struct _tmp23B;(_tmp23B.tag=0,((_tmp23B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpD9->hd)->name).f2),((_tmp23C.tag=0,((_tmp23C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpCD),((_tmp23D.tag=0,((_tmp23D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp23E[0]=& _tmp23D,((_tmp23E[1]=& _tmp23C,((_tmp23E[2]=& _tmp23B,Cyc_Tcdecl_merr(loc,msg,((_tmp23F="%s %s has extra field %s",_tag_dyneither(_tmp23F,sizeof(char),25))),_tag_dyneither(_tmp23E,sizeof(void*),3)))))))))))))))))));}
_tmpCE=0;}
# 495
d2=d0;}
# 497
goto _LL29;}_LL29:;}
# 500
if(!_tmpCE)return 0;
# 502
if(_tmpD5 == d2->sc)
return d2;else{
# 505
{struct Cyc_Absyn_Datatypedecl*_tmp240;d2=((_tmp240=_cycalloc(sizeof(*_tmp240)),((_tmp240[0]=*d2,_tmp240))));}
d2->sc=_tmpD5;
return d2;}};};}
# 511
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 513
struct _dyneither_ptr _tmpEC=Cyc_Absynpp_qvar2string(d0->name);
int _tmpED=1;
# 517
const char*_tmp241;struct _tuple10 _stmttmpB=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp241="enum",_tag_dyneither(_tmp241,sizeof(char),5))),_tmpEC,loc,msg,1);enum Cyc_Absyn_Scope _tmpEF;int _tmpF0;struct _tuple10 _tmpEE=_stmttmpB;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;
if(!_tmpF0)_tmpED=0;{
# 520
struct Cyc_Absyn_Enumdecl*d2;
# 523
{struct _tuple17 _tmp242;struct _tuple17 _stmttmpC=(_tmp242.f1=d0->fields,((_tmp242.f2=d1->fields,_tmp242)));struct _tuple17 _tmpF1=_stmttmpC;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF3;_LL31: if(_tmpF1.f2 != 0)goto _LL33;_LL32:
 d2=d0;goto _LL30;_LL33: if(_tmpF1.f1 != 0)goto _LL35;_LL34:
 d2=d1;goto _LL30;_LL35: if(_tmpF1.f1 == 0)goto _LL30;_tmpF2=(struct Cyc_List_List*)(_tmpF1.f1)->v;if(_tmpF1.f2 == 0)goto _LL30;_tmpF3=(struct Cyc_List_List*)(_tmpF1.f2)->v;_LL36:
# 528
 for(0;_tmpF2 != 0  && _tmpF3 != 0;(_tmpF2=_tmpF2->tl,_tmpF3=_tmpF3->tl)){
struct Cyc_Absyn_Enumfield*_stmttmpD=(struct Cyc_Absyn_Enumfield*)_tmpF2->hd;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;unsigned int _tmpF7;struct Cyc_Absyn_Enumfield*_tmpF4=_stmttmpD;_tmpF5=(_tmpF4->name)->f2;_tmpF6=_tmpF4->tag;_tmpF7=_tmpF4->loc;{
struct Cyc_Absyn_Enumfield*_stmttmpE=(struct Cyc_Absyn_Enumfield*)_tmpF3->hd;struct _dyneither_ptr*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;unsigned int _tmpFB;struct Cyc_Absyn_Enumfield*_tmpF8=_stmttmpE;_tmpF9=(_tmpF8->name)->f2;_tmpFA=_tmpF8->tag;_tmpFB=_tmpF8->loc;
# 533
if(Cyc_strptrcmp(_tmpF5,_tmpF9)!= 0){
{const char*_tmp248;void*_tmp247[3];struct Cyc_String_pa_PrintArg_struct _tmp246;struct Cyc_String_pa_PrintArg_struct _tmp245;struct Cyc_String_pa_PrintArg_struct _tmp244;(_tmp244.tag=0,((_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF9),((_tmp245.tag=0,((_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF5),((_tmp246.tag=0,((_tmp246.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEC),((_tmp247[0]=& _tmp246,((_tmp247[1]=& _tmp245,((_tmp247[2]=& _tmp244,Cyc_Tcdecl_merr(loc,msg,((_tmp248="enum %s: field name mismatch %s != %s",_tag_dyneither(_tmp248,sizeof(char),38))),_tag_dyneither(_tmp247,sizeof(void*),3)))))))))))))))))));}
_tmpED=0;}
# 538
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpF6))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpFA))){
{const char*_tmp24D;void*_tmp24C[2];struct Cyc_String_pa_PrintArg_struct _tmp24B;struct Cyc_String_pa_PrintArg_struct _tmp24A;(_tmp24A.tag=0,((_tmp24A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF9),((_tmp24B.tag=0,((_tmp24B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpEC),((_tmp24C[0]=& _tmp24B,((_tmp24C[1]=& _tmp24A,Cyc_Tcdecl_merr(loc,msg,((_tmp24D="enum %s, field %s, value mismatch",_tag_dyneither(_tmp24D,sizeof(char),34))),_tag_dyneither(_tmp24C,sizeof(void*),2)))))))))))));}
_tmpED=0;}};}
# 543
d2=d0;
goto _LL30;_LL30:;}
# 547
if(!_tmpED)return 0;
# 549
if(d2->sc == _tmpEF)
return d2;else{
# 552
{struct Cyc_Absyn_Enumdecl*_tmp24E;d2=((_tmp24E=_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=*d2,_tmp24E))));}
d2->sc=_tmpEF;
return d2;}};}
# 558
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 562
int _tmp108=1;
# 565
struct _tuple10 _stmttmpF=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);enum Cyc_Absyn_Scope _tmp10A;int _tmp10B;struct _tuple10 _tmp109=_stmttmpF;_tmp10A=_tmp109.f1;_tmp10B=_tmp109.f2;
if(!_tmp10B)_tmp108=0;
# 569
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp10C=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp10D=Cyc_Absynpp_typ2string(t1);
{const char*_tmp24F;if((((Cyc_strlen((struct _dyneither_ptr)_tmp10C)+ Cyc_strlen((struct _dyneither_ptr)_tmp10D))+ Cyc_strlen((struct _dyneither_ptr)t))+ Cyc_strlen((struct _dyneither_ptr)v))+ 
Cyc_strlen(((_tmp24F="  was declared  instead of ",_tag_dyneither(_tmp24F,sizeof(char),28))))> 70){
const char*_tmp256;void*_tmp255[4];struct Cyc_String_pa_PrintArg_struct _tmp254;struct Cyc_String_pa_PrintArg_struct _tmp253;struct Cyc_String_pa_PrintArg_struct _tmp252;struct Cyc_String_pa_PrintArg_struct _tmp251;(_tmp251.tag=0,((_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10C),((_tmp252.tag=0,((_tmp252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10D),((_tmp253.tag=0,((_tmp253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp254.tag=0,((_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp255[0]=& _tmp254,((_tmp255[1]=& _tmp253,((_tmp255[2]=& _tmp252,((_tmp255[3]=& _tmp251,Cyc_Tcdecl_merr(loc,msg,((_tmp256="%s %s was declared \n\t%s\n instead of \n\t%s",_tag_dyneither(_tmp256,sizeof(char),41))),_tag_dyneither(_tmp255,sizeof(void*),4)))))))))))))))))))))))));}else{
# 577
const char*_tmp25D;void*_tmp25C[4];struct Cyc_String_pa_PrintArg_struct _tmp25B;struct Cyc_String_pa_PrintArg_struct _tmp25A;struct Cyc_String_pa_PrintArg_struct _tmp259;struct Cyc_String_pa_PrintArg_struct _tmp258;(_tmp258.tag=0,((_tmp258.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10C),((_tmp259.tag=0,((_tmp259.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10D),((_tmp25A.tag=0,((_tmp25A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp25B.tag=0,((_tmp25B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp25C[0]=& _tmp25B,((_tmp25C[1]=& _tmp25A,((_tmp25C[2]=& _tmp259,((_tmp25C[3]=& _tmp258,Cyc_Tcdecl_merr(loc,msg,((_tmp25D="%s %s was declared %s instead of %s",_tag_dyneither(_tmp25D,sizeof(char),36))),_tag_dyneither(_tmp25C,sizeof(void*),4)))))))))))))))))))))))));}}
# 579
Cyc_Tcutil_explain_failure();
_tmp108=0;}
# 584
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
{const char*_tmp262;void*_tmp261[2];struct Cyc_String_pa_PrintArg_struct _tmp260;struct Cyc_String_pa_PrintArg_struct _tmp25F;(_tmp25F.tag=0,((_tmp25F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp260.tag=0,((_tmp260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp261[0]=& _tmp260,((_tmp261[1]=& _tmp25F,Cyc_Tcdecl_merr(loc,msg,((_tmp262="%s %s has different type qualifiers",_tag_dyneither(_tmp262,sizeof(char),36))),_tag_dyneither(_tmp261,sizeof(void*),2)))))))))))));}
_tmp108=0;}
# 590
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
{const char*_tmp267;void*_tmp266[2];struct Cyc_String_pa_PrintArg_struct _tmp265;struct Cyc_String_pa_PrintArg_struct _tmp264;(_tmp264.tag=0,((_tmp264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp265.tag=0,((_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp266[0]=& _tmp265,((_tmp266[1]=& _tmp264,Cyc_Tcdecl_merr(loc,msg,((_tmp267="%s %s has different attributes",_tag_dyneither(_tmp267,sizeof(char),31))),_tag_dyneither(_tmp266,sizeof(void*),2)))))))))))));}
{const char*_tmp26A;void*_tmp269;(_tmp269=0,Cyc_fprintf(Cyc_stderr,((_tmp26A="\tprevious attributes: ",_tag_dyneither(_tmp26A,sizeof(char),23))),_tag_dyneither(_tmp269,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){
const char*_tmp26E;void*_tmp26D[1];struct Cyc_String_pa_PrintArg_struct _tmp26C;(_tmp26C.tag=0,((_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp26D[0]=& _tmp26C,Cyc_fprintf(Cyc_stderr,((_tmp26E="%s ",_tag_dyneither(_tmp26E,sizeof(char),4))),_tag_dyneither(_tmp26D,sizeof(void*),1)))))));}
{const char*_tmp271;void*_tmp270;(_tmp270=0,Cyc_fprintf(Cyc_stderr,((_tmp271="\n\tcurrent attributes: ",_tag_dyneither(_tmp271,sizeof(char),23))),_tag_dyneither(_tmp270,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){
const char*_tmp275;void*_tmp274[1];struct Cyc_String_pa_PrintArg_struct _tmp273;(_tmp273.tag=0,((_tmp273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp274[0]=& _tmp273,Cyc_fprintf(Cyc_stderr,((_tmp275="%s ",_tag_dyneither(_tmp275,sizeof(char),4))),_tag_dyneither(_tmp274,sizeof(void*),1)))))));}
{const char*_tmp278;void*_tmp277;(_tmp277=0,Cyc_fprintf(Cyc_stderr,((_tmp278="\n",_tag_dyneither(_tmp278,sizeof(char),2))),_tag_dyneither(_tmp277,sizeof(void*),0)));}
_tmp108=0;}{
# 602
struct _tuple10 _tmp279;return(_tmp279.f1=_tmp10A,((_tmp279.f2=_tmp108,_tmp279)));};}
# 605
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 607
struct _dyneither_ptr _tmp130=Cyc_Absynpp_qvar2string(d0->name);
# 610
const char*_tmp27A;struct _tuple10 _stmttmp10=
Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,(
# 613
(_tmp27A="variable",_tag_dyneither(_tmp27A,sizeof(char),9))),_tmp130,loc,msg);
# 610
enum Cyc_Absyn_Scope _tmp132;int _tmp133;struct _tuple10 _tmp131=_stmttmp10;_tmp132=_tmp131.f1;_tmp133=_tmp131.f2;
# 614
if(!_tmp133)return 0;
# 616
if(d0->sc == _tmp132)
return d0;else{
# 619
struct Cyc_Absyn_Vardecl*_tmp27B;struct Cyc_Absyn_Vardecl*_tmp134=(_tmp27B=_cycalloc(sizeof(*_tmp27B)),((_tmp27B[0]=*d0,_tmp27B)));
_tmp134->sc=_tmp132;
return _tmp134;}}
# 625
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 627
struct _dyneither_ptr _tmp137=Cyc_Absynpp_qvar2string(d0->name);
# 630
{const char*_tmp27C;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp27C="typedef",_tag_dyneither(_tmp27C,sizeof(char),8))),_tmp137,loc,msg))return 0;}{
# 633
struct Cyc_List_List*_tmp139=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 636
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp139,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
{const char*_tmp282;void*_tmp281[3];struct Cyc_String_pa_PrintArg_struct _tmp280;struct Cyc_String_pa_PrintArg_struct _tmp27F;struct Cyc_String_pa_PrintArg_struct _tmp27E;(_tmp27E.tag=0,((_tmp27E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn))),((_tmp27F.tag=0,((_tmp27F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((_tmp280.tag=0,((_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp137),((_tmp281[0]=& _tmp280,((_tmp281[1]=& _tmp27F,((_tmp281[2]=& _tmp27E,Cyc_Tcdecl_merr(loc,msg,((_tmp282="typedef %s does not refer to the same type: %s != %s",_tag_dyneither(_tmp282,sizeof(char),53))),_tag_dyneither(_tmp281,sizeof(void*),3)))))))))))))))))));}
return 0;}}
# 644
return d0;};}
# 647
void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp283;struct _tuple14 _stmttmp11=(_tmp283.f1=b0,((_tmp283.f2=b1,_tmp283)));struct _tuple14 _tmp13F=_stmttmp11;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Vardecl*_tmp145;struct Cyc_Absyn_Vardecl*_tmp147;struct Cyc_Absyn_Fndecl*_tmp149;struct Cyc_Absyn_Fndecl*_tmp14C;struct Cyc_Absyn_Fndecl*_tmp14E;struct Cyc_Absyn_Vardecl*_tmp150;_LL38:{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp140=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp13F.f1;if(_tmp140->tag != 0)goto _LL3A;}{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp141=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp13F.f2;if(_tmp141->tag != 0)goto _LL3A;};_LL39:
 return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3A:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp142=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13F.f1;if(_tmp142->tag != 1)goto _LL3C;else{_tmp143=_tmp142->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp144=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13F.f2;if(_tmp144->tag != 1)goto _LL3C;else{_tmp145=_tmp144->f1;}};_LL3B: {
# 651
struct Cyc_Absyn_Vardecl*_tmp151=Cyc_Tcdecl_merge_vardecl(_tmp143,_tmp145,loc,msg);
if(_tmp151 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp151 == _tmp143)return b0;
if(_tmp151 == _tmp145)return b1;{
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp286;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp285;return(void*)((_tmp285=_cycalloc(sizeof(*_tmp285)),((_tmp285[0]=((_tmp286.tag=1,((_tmp286.f1=_tmp151,_tmp286)))),_tmp285))));};}_LL3C:{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp146=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13F.f1;if(_tmp146->tag != 1)goto _LL3E;else{_tmp147=_tmp146->f1;}}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp148=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13F.f2;if(_tmp148->tag != 2)goto _LL3E;else{_tmp149=_tmp148->f1;}};_LL3D: {
# 657
const char*_tmp287;struct _tuple10 _stmttmp13=Cyc_Tcdecl_check_var_or_fn_decl(_tmp147->sc,_tmp147->type,_tmp147->tq,_tmp147->attributes,_tmp149->sc,(void*)_check_null(_tmp149->cached_typ),
Cyc_Absyn_empty_tqual(0),_tmp149->attributes,(
(_tmp287="function",_tag_dyneither(_tmp287,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp147->name),loc,msg);
# 657
int _tmp155;struct _tuple10 _tmp154=_stmttmp13;_tmp155=_tmp154.f2;
# 661
return _tmp155?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3E:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp14A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13F.f1;if(_tmp14A->tag != 2)goto _LL40;}{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp14B=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13F.f2;if(_tmp14B->tag != 2)goto _LL40;else{_tmp14C=_tmp14B->f1;}};_LL3F:
# 663
{const char*_tmp28B;void*_tmp28A[1];struct Cyc_String_pa_PrintArg_struct _tmp289;(_tmp289.tag=0,((_tmp289.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14C->name)),((_tmp28A[0]=& _tmp289,Cyc_Tcdecl_merr(loc,msg,((_tmp28B="redefinition of function %s",_tag_dyneither(_tmp28B,sizeof(char),28))),_tag_dyneither(_tmp28A,sizeof(void*),1)))))));}
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL40:{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp14D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13F.f1;if(_tmp14D->tag != 2)goto _LL42;else{_tmp14E=_tmp14D->f1;}}{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp14F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13F.f2;if(_tmp14F->tag != 1)goto _LL42;else{_tmp150=_tmp14F->f1;}};_LL41: {
# 666
const char*_tmp28C;struct _tuple10 _stmttmp12=Cyc_Tcdecl_check_var_or_fn_decl(_tmp14E->sc,(void*)_check_null(_tmp14E->cached_typ),Cyc_Absyn_empty_tqual(0),_tmp14E->attributes,_tmp150->sc,_tmp150->type,_tmp150->tq,_tmp150->attributes,(
# 668
(_tmp28C="variable",_tag_dyneither(_tmp28C,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp14E->name),loc,msg);
# 666
int _tmp15B;struct _tuple10 _tmp15A=_stmttmp12;_tmp15B=_tmp15A.f2;
# 670
return _tmp15B?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL42:;_LL43: {
struct Cyc_Core_Invalid_argument_exn_struct _tmp292;const char*_tmp291;struct Cyc_Core_Invalid_argument_exn_struct*_tmp290;(int)_throw((void*)((_tmp290=_cycalloc(sizeof(*_tmp290)),((_tmp290[0]=((_tmp292.tag=Cyc_Core_Invalid_argument,((_tmp292.f1=((_tmp291="Tcdecl::merge_binding",_tag_dyneither(_tmp291,sizeof(char),22))),_tmp292)))),_tmp290)))));}_LL37:;}
# 675
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 678
struct Cyc_Absyn_Datatypedecl*_tmp162;struct Cyc_Absyn_Datatypefield*_tmp163;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp161=d0;_tmp162=_tmp161->base;_tmp163=_tmp161->field;{
struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp164=d1;_tmp165=_tmp164->base;_tmp166=_tmp164->field;{
struct _dyneither_ptr _tmp167=Cyc_Absynpp_qvar2string(_tmp163->name);
# 683
if(Cyc_Absyn_qvar_cmp(_tmp162->name,_tmp165->name)!= 0){
{const char*_tmp298;void*_tmp297[3];struct Cyc_String_pa_PrintArg_struct _tmp296;struct Cyc_String_pa_PrintArg_struct _tmp295;struct Cyc_String_pa_PrintArg_struct _tmp294;(_tmp294.tag=0,((_tmp294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp167),((_tmp295.tag=0,((_tmp295.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp165->name)),((_tmp296.tag=0,((_tmp296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp162->name)),((_tmp297[0]=& _tmp296,((_tmp297[1]=& _tmp295,((_tmp297[2]=& _tmp294,Cyc_Tcdecl_merr(loc,msg,((_tmp298="@extensible datatype %s and %s have both a field named %s",_tag_dyneither(_tmp298,sizeof(char),58))),_tag_dyneither(_tmp297,sizeof(void*),3)))))))))))))))))));}
return 0;}
# 690
{const char*_tmp299;if(!Cyc_Tcdecl_check_tvs(_tmp162->tvs,_tmp165->tvs,((_tmp299="@extensible datatype",_tag_dyneither(_tmp299,sizeof(char),21))),Cyc_Absynpp_qvar2string(_tmp162->name),loc,msg))
return 0;}{
# 693
struct Cyc_List_List*_tmp16E=Cyc_Tcdecl_build_tvs_map(_tmp162->tvs,_tmp165->tvs);
# 696
const char*_tmp29A;struct Cyc_Absyn_Datatypefield*_tmp16F=Cyc_Tcdecl_merge_datatypefield(_tmp163,_tmp166,_tmp16E,((_tmp29A="@extensible datatype field",_tag_dyneither(_tmp29A,sizeof(char),27))),_tmp167,msg);
if(_tmp16F == 0)return 0;
if(_tmp16F == _tmp163)return d0;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp29B;return(_tmp29B=_cycalloc(sizeof(*_tmp29B)),((_tmp29B->base=_tmp162,((_tmp29B->field=_tmp16F,_tmp29B)))));};};};};}
