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
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 279 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 54
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
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
# 909
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 952
extern void*Cyc_Absyn_empty_effect;
# 954
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 987
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
# 86
extern struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val;
extern struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val;
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 102
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 118 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 123
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 125
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 139
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 145
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 155
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 162
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 173
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 184
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 188
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 195
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 212
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 214
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 218
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 221
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 224
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 144
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 147
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 181
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 185
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 241 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 310
int Cyc_Tcutil_new_tvar_id();
# 312
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);char Cyc_Tcenv_Env_error[10]="Env_error";
# 88 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 93
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 145
struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val={0};
struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val={1};
struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val={2};
# 150
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;int try_depth;};
# 164
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 192 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 227 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 230
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 234
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 239
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp347;return(_tmp347=_region_malloc(r,sizeof(*_tmp347)),((_tmp347->grgn=r,((_tmp347->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp347->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp347->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp347->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp347->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp347->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp347->availables=0,_tmp347)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 250
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 252
{struct Cyc_Core_Opt*_tmp348;Cyc_Tcutil_empty_var_set=((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp348))));}{
# 254
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp349;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349[0]=Cyc_Absyn_exn_tud,_tmp349)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp34F;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp34E;struct _tuple12*_tmp34D;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp34D=_region_malloc(r,sizeof(*_tmp34D)),((_tmp34D->f1=(void*)(
(_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F[0]=((_tmp34E.tag=2,((_tmp34E.f1=Cyc_Absyn_exn_tud,((_tmp34E.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp34E)))))),_tmp34F)))),((_tmp34D->f2=1,_tmp34D)))))));}}{
# 261
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp350;return(_tmp350=_region_malloc(r,sizeof(*_tmp350)),((_tmp350->ns=0,((_tmp350->ae=ae,((_tmp350->le=0,((_tmp350->allow_valueof=0,_tmp350)))))))));};};}
# 267
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 269
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 273
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp351;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp351="outer_namespace",_tag_dyneither(_tmp351,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 279
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 286
if(n2 != 0)return 0;
return 1;}
# 292
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 297
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){
lookup(ge2,v,0);{
const char*_tmp355;void*_tmp354[1];struct Cyc_String_pa_PrintArg_struct _tmp353;(_tmp353.tag=0,((_tmp353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp354[0]=& _tmp353,Cyc_Tcutil_terr(loc,((_tmp355="%s is ambiguous",_tag_dyneither(_tmp355,sizeof(char),16))),_tag_dyneither(_tmp354,sizeof(void*),1)))))));};
# 301
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp16=_tmp10;void*_tmp18;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp17=(struct Cyc_Dict_Absent_exn_struct*)_tmp16;if(_tmp17->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 303
 goto _LL0;_LL3: _tmp18=_tmp16;_LL4:(void)_throw(_tmp18);_LL0:;}};}}
# 305
return;}
# 308
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 313
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1C=result;_npop_handler(0);return _tmp1C;};}
# 314
;_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1E=_tmp1A;void*_tmp20;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp1F=(struct Cyc_Dict_Absent_exn_struct*)_tmp1E;if(_tmp1F->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 317
 goto _LL5;_LL8: _tmp20=_tmp1E;_LL9:(void)_throw(_tmp20);_LL5:;}};}
# 319
{struct Cyc_List_List*_tmp21=ge->availables;for(0;_tmp21 != 0;_tmp21=_tmp21->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp21->hd);
struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp21->hd,_tmp21->tl);{
void*_tmp25=result;_npop_handler(0);return _tmp25;};}
# 322
;_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp27=_tmp23;void*_tmp29;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp28=(struct Cyc_Dict_Absent_exn_struct*)_tmp27;if(_tmp28->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 325
 goto _LLA;_LLD: _tmp29=_tmp27;_LLE:(void)_throw(_tmp29);_LLA:;}};}}
# 327
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp358;struct Cyc_Dict_Absent_exn_struct*_tmp357;(int)_throw((void*)((_tmp357=_cycalloc_atomic(sizeof(*_tmp357)),((_tmp357[0]=((_tmp358.tag=Cyc_Dict_Absent,_tmp358)),_tmp357)))));}
# 330
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 334
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2C,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 341
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp2E;struct _dyneither_ptr*_tmp2F;struct _tuple0*_tmp2D=q;_tmp2E=_tmp2D->f1;_tmp2F=_tmp2D->f2;{
union Cyc_Absyn_Nmspace _tmp30=_tmp2E;struct _dyneither_ptr*_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp34;_LL10: if((_tmp30.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp30.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL14;_LL13:
# 346
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp2F,is_use);_LL14: if((_tmp30.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp30.Rel_n).val == 0)goto _LL16;_tmp31=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp30.Rel_n).val)->hd;_tmp32=((struct Cyc_List_List*)(_tmp30.Rel_n).val)->tl;_LL15: {
# 348
struct Cyc_Tcenv_Genv*_tmp35=Cyc_Tcenv_lookup_namespace(te,loc,_tmp31,_tmp32);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp35,_tmp2F,is_use);}_LL16: if((_tmp30.C_n).tag != 3)goto _LL18;_tmp33=(struct Cyc_List_List*)(_tmp30.C_n).val;_LL17:
 _tmp34=_tmp33;goto _LL19;_LL18: if((_tmp30.Abs_n).tag != 2)goto _LLF;_tmp34=(struct Cyc_List_List*)(_tmp30.Abs_n).val;_LL19:
# 352
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp34),_tmp2F,is_use);_LLF:;};}
# 357
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 359
struct Cyc_List_List*_tmp36=te->ns;
struct _RegionHandle _tmp37=_new_region("temp");struct _RegionHandle*temp=& _tmp37;_push_region(temp);
{struct Cyc_List_List*_tmp38=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp39=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp36);
struct Cyc_List_List*_tmp3A=_tmp39->availables;
struct Cyc_Set_Set*_tmp3B=_tmp39->namespaces;
# 367
{struct Cyc_List_List*_tmp3C=_tmp3A;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
struct Cyc_Set_Set*_tmp3D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3C->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3D,n)){
struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35A;_tmp38=((_tmp35A=_region_malloc(temp,sizeof(*_tmp35A)),((_tmp35A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B->hd=n,((_tmp35B->tl=ns,_tmp35B))))))),((_tmp35A->tl=_tmp38,_tmp35A))))));}}}
# 374
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp35E;struct Cyc_List_List*_tmp35D;_tmp38=((_tmp35D=_region_malloc(temp,sizeof(*_tmp35D)),((_tmp35D->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp35E=_cycalloc(sizeof(*_tmp35E)),((_tmp35E->hd=n,((_tmp35E->tl=ns,_tmp35E))))))),((_tmp35D->tl=_tmp38,_tmp35D))))));}
# 380
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp362;void*_tmp361[1];struct Cyc_String_pa_PrintArg_struct _tmp360;(_tmp360.tag=0,((_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp361[0]=& _tmp360,Cyc_Tcutil_terr(loc,((_tmp362="%s is ambiguous",_tag_dyneither(_tmp362,sizeof(char),16))),_tag_dyneither(_tmp361,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 385
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp365;struct Cyc_Dict_Absent_exn_struct*_tmp364;(int)_throw((void*)((_tmp364=_cycalloc_atomic(sizeof(*_tmp364)),((_tmp364[0]=((_tmp365.tag=Cyc_Dict_Absent,_tmp365)),_tmp364)))));}
# 388
_tmp36=Cyc_Tcenv_outer_namespace(_tmp36);}}
# 361
;_pop_region(temp);}
# 396
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp48=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,v);}
# 400
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4A;struct _dyneither_ptr*_tmp4B;struct _tuple0*_tmp49=q;_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;{
union Cyc_Absyn_Nmspace _tmp4C=_tmp4A;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;struct _dyneither_ptr*_tmp4F;struct Cyc_List_List*_tmp50;_LL1B: if((_tmp4C.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4C.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val != 0)goto _LL1F;_LL1E:
# 405
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4B,0);_LL1F: if((_tmp4C.C_n).tag != 3)goto _LL21;_tmp4D=(struct Cyc_List_List*)(_tmp4C.C_n).val;_LL20:
 _tmp4E=_tmp4D;goto _LL22;_LL21: if((_tmp4C.Abs_n).tag != 2)goto _LL23;_tmp4E=(struct Cyc_List_List*)(_tmp4C.Abs_n).val;_LL22: {
# 408
struct Cyc_Dict_Dict _tmp51=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4E))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp51,_tmp4B);}_LL23: if((_tmp4C.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val == 0)goto _LL1A;_tmp4F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->hd;_tmp50=((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->tl;_LL24: {
# 411
struct Cyc_Dict_Dict _tmp52=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp4F,_tmp50))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp52,_tmp4B);}_LL1A:;};}
# 416
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp53=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,v);}
# 420
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp55;struct _dyneither_ptr*_tmp56;struct _tuple0*_tmp54=q;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
union Cyc_Absyn_Nmspace _tmp57=_tmp55;struct _dyneither_ptr*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp5B;_LL26: if((_tmp57.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp57.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp57.Rel_n).val != 0)goto _LL2A;_LL29:
# 425
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp56,0);_LL2A: if((_tmp57.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp57.Rel_n).val == 0)goto _LL2C;_tmp58=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp57.Rel_n).val)->hd;_tmp59=((struct Cyc_List_List*)(_tmp57.Rel_n).val)->tl;_LL2B: {
# 427
struct Cyc_Dict_Dict _tmp5C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp58,_tmp59))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5C,_tmp56);}_LL2C: if((_tmp57.C_n).tag != 3)goto _LL2E;_tmp5A=(struct Cyc_List_List*)(_tmp57.C_n).val;_LL2D:
 _tmp5B=_tmp5A;goto _LL2F;_LL2E: if((_tmp57.Abs_n).tag != 2)goto _LL25;_tmp5B=(struct Cyc_List_List*)(_tmp57.Abs_n).val;_LL2F: {
# 431
struct Cyc_Dict_Dict _tmp5D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5B))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5D,_tmp56);}_LL25:;};}
# 436
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 441
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp60;struct _tuple0*_tmp5E=q;_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;{
union Cyc_Absyn_Nmspace _tmp61=_tmp5F;struct _dyneither_ptr*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp65;_LL31: if((_tmp61.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp61.Rel_n).val != 0)goto _LL33;_LL32: {
# 446
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp366;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp366=_region_malloc(r,sizeof(*_tmp366)),((_tmp366[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp366)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_throw(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 449
{const char*_tmp369;void*_tmp368;(_tmp368=0,Cyc_Tcutil_terr(loc,((_tmp369="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp369,sizeof(char),33))),_tag_dyneither(_tmp368,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 456
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 460
{const char*_tmp36C;void*_tmp36B;(_tmp36B=0,Cyc_Tcutil_terr(loc,((_tmp36C="bad qualified name for @extensible datatype",_tag_dyneither(_tmp36C,sizeof(char),44))),_tag_dyneither(_tmp36B,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp36F;struct Cyc_Dict_Absent_exn_struct*_tmp36E;(int)_throw((void*)((_tmp36E=_cycalloc_atomic(sizeof(*_tmp36E)),((_tmp36E[0]=((_tmp36F.tag=Cyc_Dict_Absent,_tmp36F)),_tmp36E)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_throw(_tmp77);_LL40:;}};}{
# 463
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp370;return(_tmp370=_region_malloc(r,sizeof(*_tmp370)),((_tmp370[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp370)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 467
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp371;return(_tmp371=_region_malloc(r,sizeof(*_tmp371)),((_tmp371[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp371)));}_LL30:;};}
# 472
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp80=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,v);}
# 476
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct _tuple0*_tmp81=q;_tmp82=_tmp81->f1;_tmp83=_tmp81->f2;{
union Cyc_Absyn_Nmspace _tmp84=_tmp82;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp88;_LL46: if((_tmp84.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp84.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp84.Rel_n).val != 0)goto _LL4A;_LL49:
# 481
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp83,0);_LL4A: if((_tmp84.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp84.Rel_n).val == 0)goto _LL4C;_tmp85=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp84.Rel_n).val)->hd;_tmp86=((struct Cyc_List_List*)(_tmp84.Rel_n).val)->tl;_LL4B: {
# 483
struct Cyc_Dict_Dict _tmp89=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp89,_tmp83);}_LL4C: if((_tmp84.C_n).tag != 3)goto _LL4E;_tmp87=(struct Cyc_List_List*)(_tmp84.C_n).val;_LL4D:
 _tmp88=_tmp87;goto _LL4F;_LL4E: if((_tmp84.Abs_n).tag != 2)goto _LL45;_tmp88=(struct Cyc_List_List*)(_tmp84.Abs_n).val;_LL4F: {
# 487
struct Cyc_Dict_Dict _tmp8A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp88))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp83);}_LL45:;};}
# 492
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8B=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,v);}
# 496
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp8D;struct _dyneither_ptr*_tmp8E;struct _tuple0*_tmp8C=q;_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;{
union Cyc_Absyn_Nmspace _tmp8F=_tmp8D;struct _dyneither_ptr*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;_LL51: if((_tmp8F.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp8F.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val != 0)goto _LL55;_LL54:
# 501
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8E,0);_LL55: if((_tmp8F.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val == 0)goto _LL57;_tmp90=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->hd;_tmp91=((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->tl;_LL56: {
# 503
struct Cyc_Dict_Dict _tmp94=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp90,_tmp91))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8E);}_LL57: if((_tmp8F.C_n).tag != 3)goto _LL59;_tmp92=(struct Cyc_List_List*)(_tmp8F.C_n).val;_LL58:
 _tmp93=_tmp92;goto _LL5A;_LL59: if((_tmp8F.Abs_n).tag != 2)goto _LL50;_tmp93=(struct Cyc_List_List*)(_tmp8F.Abs_n).val;_LL5A: {
# 507
struct Cyc_Dict_Dict _tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8E);}_LL50:;};}
# 514
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp96=te->le;
if(_tmp96 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp96;}
# 520
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 524
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp97=te->le;
if(_tmp97 != 0){
struct Cyc_Tcenv_Fenv*_stmttmp0=_tmp97;struct _RegionHandle*_tmp99;struct Cyc_Tcenv_Fenv*_tmp98=_stmttmp0;_tmp99=_tmp98->fnrgn;
return _tmp99;}
# 530
return Cyc_Tcenv_coerce_heap_region();}
# 533
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp372;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp372="put_fenv",_tag_dyneither(_tmp372,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp373;return(_tmp373=_region_malloc(l,sizeof(*_tmp373)),((_tmp373->ns=te->ns,((_tmp373->ae=te->ae,((_tmp373->le=fe,((_tmp373->allow_valueof=te->allow_valueof,_tmp373)))))))));};}
# 538
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp374;return(_tmp374=_region_malloc(l,sizeof(*_tmp374)),((_tmp374->ns=te->ns,((_tmp374->ae=te->ae,((_tmp374->le=0,((_tmp374->allow_valueof=te->allow_valueof,_tmp374)))))))));}
# 542
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 544
struct Cyc_Tcenv_Fenv _stmttmp1=*f;struct Cyc_Tcenv_SharedFenv*_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_RgnOrder_RgnPO*_tmpA0;const struct Cyc_Tcenv_Bindings*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA4;void*_tmpA5;struct Cyc_Tcenv_FenvFlags _tmpA6;struct _RegionHandle*_tmpA7;struct Cyc_Tcenv_Fenv _tmp9D=_stmttmp1;_tmp9E=_tmp9D.shared;_tmp9F=_tmp9D.type_vars;_tmpA0=_tmp9D.region_order;_tmpA1=_tmp9D.locals;_tmpA2=_tmp9D.encloser;_tmpA3=_tmp9D.ctrl_env;_tmpA4=_tmp9D.capability;_tmpA5=_tmp9D.curr_rgn;_tmpA6=_tmp9D.flags;_tmpA7=_tmp9D.fnrgn;{
# 546
struct Cyc_Tcenv_Fenv*_tmp375;return(_tmp375=_region_malloc(l,sizeof(*_tmp375)),((_tmp375->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9E,((_tmp375->type_vars=(struct Cyc_List_List*)_tmp9F,((_tmp375->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA0,((_tmp375->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA1,((_tmp375->encloser=(struct Cyc_Absyn_Stmt*)_tmpA2,((_tmp375->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA3,((_tmp375->capability=(void*)_tmpA4,((_tmp375->curr_rgn=(void*)_tmpA5,((_tmp375->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA6,((_tmp375->fnrgn=(struct _RegionHandle*)l,_tmp375)))))))))))))))))))));};}
# 550
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 552
struct Cyc_Tcenv_Fenv _stmttmp2=*f;struct Cyc_Tcenv_SharedFenv*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_RgnOrder_RgnPO*_tmpAC;const struct Cyc_Tcenv_Bindings*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Tcenv_CtrlEnv*_tmpAF;void*_tmpB0;void*_tmpB1;struct Cyc_Tcenv_FenvFlags _tmpB2;struct Cyc_Tcenv_Fenv _tmpA9=_stmttmp2;_tmpAA=_tmpA9.shared;_tmpAB=_tmpA9.type_vars;_tmpAC=_tmpA9.region_order;_tmpAD=_tmpA9.locals;_tmpAE=_tmpA9.encloser;_tmpAF=_tmpA9.ctrl_env;_tmpB0=_tmpA9.capability;_tmpB1=_tmpA9.curr_rgn;_tmpB2=_tmpA9.flags;{
# 555
struct Cyc_Tcenv_CtrlEnv _stmttmp3=*_tmpAF;struct _RegionHandle*_tmpB4;void*_tmpB5;void*_tmpB6;const struct _tuple10*_tmpB7;void*_tmpB8;int _tmpB9;struct Cyc_Tcenv_CtrlEnv _tmpB3=_stmttmp3;_tmpB4=_tmpB3.ctrl_rgn;_tmpB5=_tmpB3.continue_stmt;_tmpB6=_tmpB3.break_stmt;_tmpB7=_tmpB3.fallthru_clause;_tmpB8=_tmpB3.next_stmt;_tmpB9=_tmpB3.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp376;struct Cyc_Tcenv_CtrlEnv*_tmpBA=
(_tmp376=_region_malloc(l,sizeof(*_tmp376)),((_tmp376->ctrl_rgn=_tmpB4,((_tmp376->continue_stmt=_tmpB5,((_tmp376->break_stmt=_tmpB6,((_tmp376->fallthru_clause=_tmpB7,((_tmp376->next_stmt=_tmpB8,((_tmp376->try_depth=_tmpB9,_tmp376)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp377;return(_tmp377=_region_malloc(l,sizeof(*_tmp377)),((_tmp377->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAA,((_tmp377->type_vars=(struct Cyc_List_List*)_tmpAB,((_tmp377->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAC,((_tmp377->locals=(const struct Cyc_Tcenv_Bindings*)_tmpAD,((_tmp377->encloser=(struct Cyc_Absyn_Stmt*)_tmpAE,((_tmp377->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBA,((_tmp377->capability=(void*)_tmpB0,((_tmp377->curr_rgn=(void*)_tmpB1,((_tmp377->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB2,((_tmp377->fnrgn=(struct _RegionHandle*)l,_tmp377)))))))))))))))))))));};};}
# 564
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp378;struct Cyc_Tcenv_Fenv*_stmttmp4=Cyc_Tcenv_get_fenv(te,((_tmp378="return_typ",_tag_dyneither(_tmp378,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpBE;struct Cyc_Tcenv_Fenv*_tmpBD=_stmttmp4;_tmpBE=_tmpBD->shared;{
struct Cyc_Tcenv_SharedFenv _stmttmp5=*_tmpBE;void*_tmpC0;struct Cyc_Tcenv_SharedFenv _tmpBF=_stmttmp5;_tmpC0=_tmpBF.return_typ;
return _tmpC0;};}
# 570
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC2=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _stmttmp6=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC2));struct Cyc_List_List*_tmpC4;struct Cyc_Tcenv_Fenv _tmpC3=_stmttmp6;_tmpC4=_tmpC3.type_vars;
return _tmpC4;};}
# 577
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp379;return(_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379->v=Cyc_Tcenv_lookup_type_vars(te),_tmp379)));}
# 581
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 583
const char*_tmp37A;struct Cyc_Tcenv_Fenv*_stmttmp7=
# 585
Cyc_Tcenv_get_fenv(te,((_tmp37A="add_type_vars",_tag_dyneither(_tmp37A,sizeof(char),14))));
# 583
struct Cyc_Tcenv_SharedFenv*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_RgnOrder_RgnPO*_tmpC9;const struct Cyc_Tcenv_Bindings*_tmpCA;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Tcenv_CtrlEnv*_tmpCC;void*_tmpCD;void*_tmpCE;struct Cyc_Tcenv_FenvFlags _tmpCF;struct Cyc_Tcenv_Fenv*_tmpC6=_stmttmp7;_tmpC7=_tmpC6->shared;_tmpC8=_tmpC6->type_vars;_tmpC9=_tmpC6->region_order;_tmpCA=_tmpC6->locals;_tmpCB=_tmpC6->encloser;_tmpCC=_tmpC6->ctrl_env;_tmpCD=_tmpC6->capability;_tmpCE=_tmpC6->curr_rgn;_tmpCF=_tmpC6->flags;
# 587
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC8);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD0);{
struct Cyc_Tcenv_Fenv*_tmp37B;struct Cyc_Tcenv_Fenv*_tmpD1=(_tmp37B=_region_malloc(r,sizeof(*_tmp37B)),((_tmp37B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC7,((_tmp37B->type_vars=(struct Cyc_List_List*)_tmpD0,((_tmp37B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC9,((_tmp37B->locals=(const struct Cyc_Tcenv_Bindings*)_tmpCA,((_tmp37B->encloser=(struct Cyc_Absyn_Stmt*)_tmpCB,((_tmp37B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCC,((_tmp37B->capability=(void*)_tmpCD,((_tmp37B->curr_rgn=(void*)_tmpCE,((_tmp37B->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCF,((_tmp37B->fnrgn=(struct _RegionHandle*)r,_tmp37B)))))))))))))))))))));
# 592
return Cyc_Tcenv_put_fenv(r,te,_tmpD1);};};}
# 595
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD4=te->le;
if(_tmpD4 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 600
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpD4);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 605
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 607
struct _dyneither_ptr*_tmpD5=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _stmttmp8=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD6=_stmttmp8;_LL5C: if((_tmpD6.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 612
struct Cyc_Core_Impossible_exn_struct _tmp381;const char*_tmp380;struct Cyc_Core_Impossible_exn_struct*_tmp37F;(int)_throw((void*)((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F[0]=((_tmp381.tag=Cyc_Core_Impossible,((_tmp381.f1=((_tmp380="add_local_var: called with Rel_n",_tag_dyneither(_tmp380,sizeof(char),33))),_tmp381)))),_tmp37F)))));}_LL5B:;}{
# 614
const char*_tmp382;struct Cyc_Tcenv_Fenv*_stmttmp9=
# 616
Cyc_Tcenv_get_fenv(te,((_tmp382="add_local_var",_tag_dyneither(_tmp382,sizeof(char),14))));
# 614
struct Cyc_Tcenv_SharedFenv*_tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_RgnOrder_RgnPO*_tmpDD;const struct Cyc_Tcenv_Bindings*_tmpDE;struct Cyc_Absyn_Stmt*_tmpDF;struct Cyc_Tcenv_CtrlEnv*_tmpE0;void*_tmpE1;void*_tmpE2;struct Cyc_Tcenv_FenvFlags _tmpE3;struct Cyc_Tcenv_Fenv*_tmpDA=_stmttmp9;_tmpDB=_tmpDA->shared;_tmpDC=_tmpDA->type_vars;_tmpDD=_tmpDA->region_order;_tmpDE=_tmpDA->locals;_tmpDF=_tmpDA->encloser;_tmpE0=_tmpDA->ctrl_env;_tmpE1=_tmpDA->capability;_tmpE2=_tmpDA->curr_rgn;_tmpE3=_tmpDA->flags;{
# 617
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp385;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp384;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE4=(_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384[0]=((_tmp385.tag=4,((_tmp385.f1=vd,_tmp385)))),_tmp384)));
struct Cyc_Tcenv_Bindings*_tmp386;struct Cyc_Tcenv_Bindings*_tmpE5=(_tmp386=_region_malloc(r,sizeof(*_tmp386)),((_tmp386->v=_tmpD5,((_tmp386->b=(void*)_tmpE4,((_tmp386->tl=_tmpDE,_tmp386)))))));
struct Cyc_Tcenv_Fenv*_tmp387;struct Cyc_Tcenv_Fenv*_tmpE6=(_tmp387=_region_malloc(r,sizeof(*_tmp387)),((_tmp387->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDB,((_tmp387->type_vars=(struct Cyc_List_List*)_tmpDC,((_tmp387->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpDD,((_tmp387->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpE5),((_tmp387->encloser=(struct Cyc_Absyn_Stmt*)_tmpDF,((_tmp387->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE0,((_tmp387->capability=(void*)_tmpE1,((_tmp387->curr_rgn=(void*)_tmpE2,((_tmp387->flags=(struct Cyc_Tcenv_FenvFlags)_tmpE3,((_tmp387->fnrgn=(struct _RegionHandle*)r,_tmp387)))))))))))))))))))));
# 621
return Cyc_Tcenv_put_fenv(r,te,_tmpE6);};};}
# 624
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 626
struct Cyc_Tcenv_Fenv*_tmpEC=te->le;
if(_tmpEC == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpA=*_tmpEC;struct Cyc_Tcenv_SharedFenv*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_RgnOrder_RgnPO*_tmpF0;const struct Cyc_Tcenv_Bindings*_tmpF1;struct Cyc_Absyn_Stmt*_tmpF2;struct Cyc_Tcenv_CtrlEnv*_tmpF3;void*_tmpF4;void*_tmpF5;int _tmpF6;int _tmpF7;int _tmpF8;struct Cyc_Tcenv_Fenv _tmpED=_stmttmpA;_tmpEE=_tmpED.shared;_tmpEF=_tmpED.type_vars;_tmpF0=_tmpED.region_order;_tmpF1=_tmpED.locals;_tmpF2=_tmpED.encloser;_tmpF3=_tmpED.ctrl_env;_tmpF4=_tmpED.capability;_tmpF5=_tmpED.curr_rgn;_tmpF6=(_tmpED.flags).in_notreadctxt;_tmpF7=(_tmpED.flags).in_lhs;_tmpF8=(_tmpED.flags).abstract_ok;{
# 631
struct Cyc_Tcenv_Fenv*_tmp388;struct Cyc_Tcenv_Fenv*_tmpF9=(_tmp388=_region_malloc(r,sizeof(*_tmp388)),((_tmp388->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEE,((_tmp388->type_vars=(struct Cyc_List_List*)_tmpEF,((_tmp388->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF0,((_tmp388->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF1,((_tmp388->encloser=(struct Cyc_Absyn_Stmt*)_tmpF2,((_tmp388->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF3,((_tmp388->capability=(void*)_tmpF4,((_tmp388->curr_rgn=(void*)_tmpF5,((_tmp388->flags=(struct Cyc_Tcenv_FenvFlags)(
# 633
((_tmp388->flags).in_notreadctxt=_tmpF6,(((_tmp388->flags).in_lhs=_tmpF7,(((_tmp388->flags).in_new=(int)status,(((_tmp388->flags).abstract_ok=_tmpF8,_tmp388->flags)))))))),((_tmp388->fnrgn=(struct _RegionHandle*)r,_tmp388)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmpF9);};};}
# 637
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFB=te->le;
if(_tmpFB == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_stmttmpB=_tmpFB;int _tmpFD;struct Cyc_Tcenv_Fenv*_tmpFC=_stmttmpB;_tmpFD=(_tmpFC->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmpFD;};}
# 644
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFE=te->le;
if(_tmpFE == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmpC=_tmpFE;int _tmp100;struct Cyc_Tcenv_Fenv*_tmpFF=_stmttmpC;_tmp100=(_tmpFF->flags).abstract_ok;
return _tmp100;};}
# 651
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 653
struct Cyc_Tcenv_Fenv*_tmp101=te->le;
if(_tmp101 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpD=*_tmp101;struct Cyc_Tcenv_SharedFenv*_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_RgnOrder_RgnPO*_tmp105;const struct Cyc_Tcenv_Bindings*_tmp106;struct Cyc_Absyn_Stmt*_tmp107;struct Cyc_Tcenv_CtrlEnv*_tmp108;void*_tmp109;void*_tmp10A;int _tmp10B;int _tmp10C;int _tmp10D;struct Cyc_Tcenv_Fenv _tmp102=_stmttmpD;_tmp103=_tmp102.shared;_tmp104=_tmp102.type_vars;_tmp105=_tmp102.region_order;_tmp106=_tmp102.locals;_tmp107=_tmp102.encloser;_tmp108=_tmp102.ctrl_env;_tmp109=_tmp102.capability;_tmp10A=_tmp102.curr_rgn;_tmp10B=(_tmp102.flags).in_notreadctxt;_tmp10C=(_tmp102.flags).in_lhs;_tmp10D=(_tmp102.flags).in_new;{
# 658
struct Cyc_Tcenv_Fenv*_tmp389;struct Cyc_Tcenv_Fenv*_tmp10E=(_tmp389=_region_malloc(r,sizeof(*_tmp389)),((_tmp389->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp103,((_tmp389->type_vars=(struct Cyc_List_List*)_tmp104,((_tmp389->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp105,((_tmp389->locals=(const struct Cyc_Tcenv_Bindings*)_tmp106,((_tmp389->encloser=(struct Cyc_Absyn_Stmt*)_tmp107,((_tmp389->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp108,((_tmp389->capability=(void*)_tmp109,((_tmp389->curr_rgn=(void*)_tmp10A,((_tmp389->flags=(struct Cyc_Tcenv_FenvFlags)(
# 660
((_tmp389->flags).in_notreadctxt=_tmp10B,(((_tmp389->flags).in_lhs=_tmp10C,(((_tmp389->flags).in_new=_tmp10D,(((_tmp389->flags).abstract_ok=1,_tmp389->flags)))))))),((_tmp389->fnrgn=(struct _RegionHandle*)r,_tmp389)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp10E);};};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp110=te->le;
if(_tmp110 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpE=*_tmp110;struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp114;const struct Cyc_Tcenv_Bindings*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp118;void*_tmp119;int _tmp11A;int _tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp111=_stmttmpE;_tmp112=_tmp111.shared;_tmp113=_tmp111.type_vars;_tmp114=_tmp111.region_order;_tmp115=_tmp111.locals;_tmp116=_tmp111.encloser;_tmp117=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp119=_tmp111.curr_rgn;_tmp11A=(_tmp111.flags).in_notreadctxt;_tmp11B=(_tmp111.flags).in_lhs;_tmp11C=(_tmp111.flags).in_new;{
# 671
struct Cyc_Tcenv_Fenv*_tmp38A;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp38A=_region_malloc(r,sizeof(*_tmp38A)),((_tmp38A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112,((_tmp38A->type_vars=(struct Cyc_List_List*)_tmp113,((_tmp38A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp114,((_tmp38A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp38A->encloser=(struct Cyc_Absyn_Stmt*)_tmp116,((_tmp38A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117,((_tmp38A->capability=(void*)_tmp118,((_tmp38A->curr_rgn=(void*)_tmp119,((_tmp38A->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp38A->flags).in_notreadctxt=_tmp11A,(((_tmp38A->flags).in_lhs=_tmp11B,(((_tmp38A->flags).in_new=_tmp11C,(((_tmp38A->flags).abstract_ok=0,_tmp38A->flags)))))))),((_tmp38A->fnrgn=(struct _RegionHandle*)r,_tmp38A)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11F=te->le;
if(_tmp11F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpF=*_tmp11F;struct Cyc_Tcenv_SharedFenv*_tmp121;struct Cyc_List_List*_tmp122;struct Cyc_RgnOrder_RgnPO*_tmp123;const struct Cyc_Tcenv_Bindings*_tmp124;struct Cyc_Absyn_Stmt*_tmp125;struct Cyc_Tcenv_CtrlEnv*_tmp126;void*_tmp127;void*_tmp128;int _tmp129;int _tmp12A;int _tmp12B;struct Cyc_Tcenv_Fenv _tmp120=_stmttmpF;_tmp121=_tmp120.shared;_tmp122=_tmp120.type_vars;_tmp123=_tmp120.region_order;_tmp124=_tmp120.locals;_tmp125=_tmp120.encloser;_tmp126=_tmp120.ctrl_env;_tmp127=_tmp120.capability;_tmp128=_tmp120.curr_rgn;_tmp129=(_tmp120.flags).in_lhs;_tmp12A=(_tmp120.flags).in_new;_tmp12B=(_tmp120.flags).abstract_ok;{
# 683
struct Cyc_Tcenv_Fenv*_tmp38B;struct Cyc_Tcenv_Fenv*_tmp12C=(_tmp38B=_region_malloc(r,sizeof(*_tmp38B)),((_tmp38B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp121,((_tmp38B->type_vars=(struct Cyc_List_List*)_tmp122,((_tmp38B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp123,((_tmp38B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp124,((_tmp38B->encloser=(struct Cyc_Absyn_Stmt*)_tmp125,((_tmp38B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp126,((_tmp38B->capability=(void*)_tmp127,((_tmp38B->curr_rgn=(void*)_tmp128,((_tmp38B->flags=(struct Cyc_Tcenv_FenvFlags)(
# 685
((_tmp38B->flags).in_notreadctxt=1,(((_tmp38B->flags).in_lhs=_tmp129,(((_tmp38B->flags).in_new=_tmp12A,(((_tmp38B->flags).abstract_ok=_tmp12B,_tmp38B->flags)))))))),((_tmp38B->fnrgn=(struct _RegionHandle*)r,_tmp38B)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp12C);};};}
# 689
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp12E=te->le;
if(_tmp12E == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp10=*_tmp12E;struct Cyc_Tcenv_SharedFenv*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_RgnOrder_RgnPO*_tmp132;const struct Cyc_Tcenv_Bindings*_tmp133;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Tcenv_CtrlEnv*_tmp135;void*_tmp136;void*_tmp137;int _tmp138;int _tmp139;int _tmp13A;struct Cyc_Tcenv_Fenv _tmp12F=_stmttmp10;_tmp130=_tmp12F.shared;_tmp131=_tmp12F.type_vars;_tmp132=_tmp12F.region_order;_tmp133=_tmp12F.locals;_tmp134=_tmp12F.encloser;_tmp135=_tmp12F.ctrl_env;_tmp136=_tmp12F.capability;_tmp137=_tmp12F.curr_rgn;_tmp138=(_tmp12F.flags).in_lhs;_tmp139=(_tmp12F.flags).in_new;_tmp13A=(_tmp12F.flags).abstract_ok;{
# 695
struct Cyc_Tcenv_Fenv*_tmp38C;struct Cyc_Tcenv_Fenv*_tmp13B=(_tmp38C=_region_malloc(r,sizeof(*_tmp38C)),((_tmp38C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp130,((_tmp38C->type_vars=(struct Cyc_List_List*)_tmp131,((_tmp38C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp132,((_tmp38C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp133,((_tmp38C->encloser=(struct Cyc_Absyn_Stmt*)_tmp134,((_tmp38C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp135,((_tmp38C->capability=(void*)_tmp136,((_tmp38C->curr_rgn=(void*)_tmp137,((_tmp38C->flags=(struct Cyc_Tcenv_FenvFlags)(
# 697
((_tmp38C->flags).in_notreadctxt=0,(((_tmp38C->flags).in_lhs=_tmp138,(((_tmp38C->flags).in_new=_tmp139,(((_tmp38C->flags).abstract_ok=_tmp13A,_tmp38C->flags)))))))),((_tmp38C->fnrgn=(struct _RegionHandle*)r,_tmp38C)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp13B);};};}
# 701
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp13D=te->le;
if(_tmp13D == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp11=_tmp13D;int _tmp13F;struct Cyc_Tcenv_Fenv*_tmp13E=_stmttmp11;_tmp13F=(_tmp13E->flags).in_notreadctxt;
return _tmp13F;};}
# 708
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp140=te->le;
if(_tmp140 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp12=*_tmp140;struct Cyc_Tcenv_SharedFenv*_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_RgnOrder_RgnPO*_tmp144;const struct Cyc_Tcenv_Bindings*_tmp145;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Tcenv_CtrlEnv*_tmp147;void*_tmp148;void*_tmp149;int _tmp14A;int _tmp14B;int _tmp14C;struct Cyc_Tcenv_Fenv _tmp141=_stmttmp12;_tmp142=_tmp141.shared;_tmp143=_tmp141.type_vars;_tmp144=_tmp141.region_order;_tmp145=_tmp141.locals;_tmp146=_tmp141.encloser;_tmp147=_tmp141.ctrl_env;_tmp148=_tmp141.capability;_tmp149=_tmp141.curr_rgn;_tmp14A=(_tmp141.flags).in_notreadctxt;_tmp14B=(_tmp141.flags).in_new;_tmp14C=(_tmp141.flags).abstract_ok;{
# 714
struct Cyc_Tcenv_Fenv*_tmp38D;struct Cyc_Tcenv_Fenv*_tmp14D=(_tmp38D=_region_malloc(r,sizeof(*_tmp38D)),((_tmp38D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp142,((_tmp38D->type_vars=(struct Cyc_List_List*)_tmp143,((_tmp38D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp144,((_tmp38D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp145,((_tmp38D->encloser=(struct Cyc_Absyn_Stmt*)_tmp146,((_tmp38D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp147,((_tmp38D->capability=(void*)_tmp148,((_tmp38D->curr_rgn=(void*)_tmp149,((_tmp38D->flags=(struct Cyc_Tcenv_FenvFlags)(
# 716
((_tmp38D->flags).in_notreadctxt=_tmp14A,(((_tmp38D->flags).in_lhs=1,(((_tmp38D->flags).in_new=_tmp14B,(((_tmp38D->flags).abstract_ok=_tmp14C,_tmp38D->flags)))))))),((_tmp38D->fnrgn=(struct _RegionHandle*)r,_tmp38D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp14D);};};}
# 720
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp14F=te->le;
if(_tmp14F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp13=*_tmp14F;struct Cyc_Tcenv_SharedFenv*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_RgnOrder_RgnPO*_tmp153;const struct Cyc_Tcenv_Bindings*_tmp154;struct Cyc_Absyn_Stmt*_tmp155;struct Cyc_Tcenv_CtrlEnv*_tmp156;void*_tmp157;void*_tmp158;int _tmp159;int _tmp15A;int _tmp15B;struct Cyc_Tcenv_Fenv _tmp150=_stmttmp13;_tmp151=_tmp150.shared;_tmp152=_tmp150.type_vars;_tmp153=_tmp150.region_order;_tmp154=_tmp150.locals;_tmp155=_tmp150.encloser;_tmp156=_tmp150.ctrl_env;_tmp157=_tmp150.capability;_tmp158=_tmp150.curr_rgn;_tmp159=(_tmp150.flags).in_notreadctxt;_tmp15A=(_tmp150.flags).in_new;_tmp15B=(_tmp150.flags).abstract_ok;{
# 726
struct Cyc_Tcenv_Fenv*_tmp38E;struct Cyc_Tcenv_Fenv*_tmp15C=(_tmp38E=_region_malloc(r,sizeof(*_tmp38E)),((_tmp38E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp151,((_tmp38E->type_vars=(struct Cyc_List_List*)_tmp152,((_tmp38E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp153,((_tmp38E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp154,((_tmp38E->encloser=(struct Cyc_Absyn_Stmt*)_tmp155,((_tmp38E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp156,((_tmp38E->capability=(void*)_tmp157,((_tmp38E->curr_rgn=(void*)_tmp158,((_tmp38E->flags=(struct Cyc_Tcenv_FenvFlags)(
# 728
((_tmp38E->flags).in_notreadctxt=_tmp159,(((_tmp38E->flags).in_lhs=0,(((_tmp38E->flags).in_new=_tmp15A,(((_tmp38E->flags).abstract_ok=_tmp15B,_tmp38E->flags)))))))),((_tmp38E->fnrgn=(struct _RegionHandle*)r,_tmp38E)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp15C);};};}
# 732
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp15E=te->le;
if(_tmp15E == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp14=_tmp15E;int _tmp160;struct Cyc_Tcenv_Fenv*_tmp15F=_stmttmp14;_tmp160=(_tmp15F->flags).in_lhs;
return _tmp160;};}
# 740
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 742
struct _dyneither_ptr*_tmp161=(*vd->name).f2;
const char*_tmp38F;struct Cyc_Tcenv_Fenv*_stmttmp15=
# 745
Cyc_Tcenv_get_fenv(te,((_tmp38F="add_pat_var",_tag_dyneither(_tmp38F,sizeof(char),12))));
# 743
struct Cyc_Tcenv_SharedFenv*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_RgnOrder_RgnPO*_tmp165;const struct Cyc_Tcenv_Bindings*_tmp166;struct Cyc_Absyn_Stmt*_tmp167;struct Cyc_Tcenv_CtrlEnv*_tmp168;void*_tmp169;void*_tmp16A;struct Cyc_Tcenv_FenvFlags _tmp16B;struct Cyc_Tcenv_Fenv*_tmp162=_stmttmp15;_tmp163=_tmp162->shared;_tmp164=_tmp162->type_vars;_tmp165=_tmp162->region_order;_tmp166=_tmp162->locals;_tmp167=_tmp162->encloser;_tmp168=_tmp162->ctrl_env;_tmp169=_tmp162->capability;_tmp16A=_tmp162->curr_rgn;_tmp16B=_tmp162->flags;{
# 746
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp392;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp391;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp16C=(_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391[0]=((_tmp392.tag=5,((_tmp392.f1=vd,_tmp392)))),_tmp391)));
struct Cyc_Tcenv_Bindings*_tmp393;struct Cyc_Tcenv_Bindings*_tmp16D=(_tmp393=_region_malloc(r,sizeof(*_tmp393)),((_tmp393->v=_tmp161,((_tmp393->b=(void*)_tmp16C,((_tmp393->tl=_tmp166,_tmp393)))))));
struct Cyc_Tcenv_Fenv*_tmp394;struct Cyc_Tcenv_Fenv*_tmp16E=(_tmp394=_region_malloc(r,sizeof(*_tmp394)),((_tmp394->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp163,((_tmp394->type_vars=(struct Cyc_List_List*)_tmp164,((_tmp394->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp165,((_tmp394->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp16D),((_tmp394->encloser=(struct Cyc_Absyn_Stmt*)_tmp167,((_tmp394->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp168,((_tmp394->capability=(void*)_tmp169,((_tmp394->curr_rgn=(void*)_tmp16A,((_tmp394->flags=(struct Cyc_Tcenv_FenvFlags)_tmp16B,((_tmp394->fnrgn=(struct _RegionHandle*)r,_tmp394)))))))))))))))))))));
# 750
return Cyc_Tcenv_put_fenv(r,te,_tmp16E);};}
# 753
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp174=te->le;
union Cyc_Absyn_Nmspace _tmp176;struct _dyneither_ptr*_tmp177;struct _tuple0*_tmp175=q;_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;{
union Cyc_Absyn_Nmspace _tmp178=_tmp176;_LL61: if((_tmp178.Loc_n).tag != 4)goto _LL63;_LL62:
# 758
 if(_tmp174 == 0){struct Cyc_Dict_Absent_exn_struct _tmp397;struct Cyc_Dict_Absent_exn_struct*_tmp396;(int)_throw((void*)((_tmp396=_cycalloc_atomic(sizeof(*_tmp396)),((_tmp396[0]=((_tmp397.tag=Cyc_Dict_Absent,_tmp397)),_tmp396)))));}
goto _LL64;_LL63: if((_tmp178.Rel_n).tag != 1)goto _LL65;if((struct Cyc_List_List*)(_tmp178.Rel_n).val != 0)goto _LL65;if(!(_tmp174 != 0))goto _LL65;_LL64: {
# 761
struct Cyc_Tcenv_Fenv*_stmttmp16=_tmp174;const struct Cyc_Tcenv_Bindings*_tmp17C;struct Cyc_Tcenv_Fenv*_tmp17B=_stmttmp16;_tmp17C=_tmp17B->locals;{
# 763
struct _handler_cons _tmp17D;_push_handler(& _tmp17D);{int _tmp17F=0;if(setjmp(_tmp17D.handler))_tmp17F=1;if(!_tmp17F){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp39A;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp399;void*_tmp182=(void*)((_tmp399=_region_malloc(r,sizeof(*_tmp399)),((_tmp399[0]=((_tmp39A.tag=0,((_tmp39A.f1=Cyc_Tcenv_lookup_binding(_tmp17C,_tmp177),_tmp39A)))),_tmp399))));_npop_handler(0);return _tmp182;};_pop_handler();}else{void*_tmp17E=(void*)_exn_thrown;void*_tmp184=_tmp17E;void*_tmp186;_LL68: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp185=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp184;if(_tmp185->tag != Cyc_Tcenv_NoBinding)goto _LL6A;}_LL69:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL6A: _tmp186=_tmp184;_LL6B:(void)_throw(_tmp186);_LL67:;}};};}_LL65:;_LL66: {
# 767
struct _handler_cons _tmp187;_push_handler(& _tmp187);{int _tmp189=0;if(setjmp(_tmp187.handler))_tmp189=1;if(!_tmp189){{void*_tmp18A=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp18A;};_pop_handler();}else{void*_tmp188=(void*)_exn_thrown;void*_tmp18C=_tmp188;void*_tmp18E;_LL6D: {struct Cyc_Dict_Absent_exn_struct*_tmp18D=(struct Cyc_Dict_Absent_exn_struct*)_tmp18C;if(_tmp18D->tag != Cyc_Dict_Absent)goto _LL6F;}_LL6E: {
# 771
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp39D;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp39C;return(void*)((_tmp39C=_region_malloc(r,sizeof(*_tmp39C)),((_tmp39C[0]=((_tmp39D.tag=0,((_tmp39D.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp39D)))),_tmp39C))));}_LL6F: _tmp18E=_tmp18C;_LL70:(void)_throw(_tmp18E);_LL6C:;}};}_LL60:;};}
# 776
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp39E;struct Cyc_Tcenv_Fenv*_stmttmp17=
Cyc_Tcenv_get_fenv(te,((_tmp39E="process_continue",_tag_dyneither(_tmp39E,sizeof(char),17))));
# 777
struct Cyc_Tcenv_CtrlEnv*_tmp192;struct Cyc_Tcenv_Fenv*_tmp191=_stmttmp17;_tmp192=_tmp191->ctrl_env;{
# 779
void*_stmttmp18=_tmp192->continue_stmt;void*_tmp193=_stmttmp18;struct Cyc_Absyn_Stmt*_tmp195;_LL72: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp194=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp193;if(_tmp194->tag != 3)goto _LL74;else{_tmp195=_tmp194->f1;}}_LL73:
# 781
{struct Cyc_List_List*_tmp39F;_tmp195->non_local_preds=((_tmp39F=_cycalloc(sizeof(*_tmp39F)),((_tmp39F->hd=s,((_tmp39F->tl=_tmp195->non_local_preds,_tmp39F))))));}
*sopt=_tmp195;
return;_LL74: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp196=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp193;if(_tmp196->tag != 0)goto _LL76;}_LL75:
{const char*_tmp3A2;void*_tmp3A1;(_tmp3A1=0,Cyc_Tcutil_terr(s->loc,((_tmp3A2="continue not in a loop",_tag_dyneither(_tmp3A2,sizeof(char),23))),_tag_dyneither(_tmp3A1,sizeof(void*),0)));}return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp197=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp193;if(_tmp197->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp198=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp193;if(_tmp198->tag != 2)goto _LL71;}_LL79: {
const char*_tmp3A3;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3A3="bad continue destination",_tag_dyneither(_tmp3A3,sizeof(char),25))));}_LL71:;};}
# 789
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3A4;struct Cyc_Tcenv_Fenv*_stmttmp19=
Cyc_Tcenv_get_fenv(te,((_tmp3A4="process_break",_tag_dyneither(_tmp3A4,sizeof(char),14))));
# 790
struct Cyc_Tcenv_CtrlEnv*_tmp19F;struct Cyc_Tcenv_SharedFenv*_tmp1A0;struct Cyc_Tcenv_Fenv*_tmp19E=_stmttmp19;_tmp19F=_tmp19E->ctrl_env;_tmp1A0=_tmp19E->shared;{
# 792
void*_stmttmp1A=_tmp19F->break_stmt;void*_tmp1A1=_stmttmp1A;struct Cyc_Absyn_Stmt*_tmp1A3;_LL7B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1A2=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1A1;if(_tmp1A2->tag != 3)goto _LL7D;else{_tmp1A3=_tmp1A2->f1;}}_LL7C:
# 794
{struct Cyc_List_List*_tmp3A5;_tmp1A3->non_local_preds=((_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5->hd=s,((_tmp3A5->tl=_tmp1A3->non_local_preds,_tmp3A5))))));}
*sopt=_tmp1A3;
return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1A4=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1A1;if(_tmp1A4->tag != 0)goto _LL7F;}_LL7E:
# 798
{const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_terr(s->loc,((_tmp3A8="break not in a loop or switch",_tag_dyneither(_tmp3A8,sizeof(char),30))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1A5=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1A1;if(_tmp1A5->tag != 2)goto _LL81;}_LL80:
# 804
{void*_stmttmp1B=Cyc_Tcutil_compress(_tmp1A0->return_typ);void*_tmp1AA=_stmttmp1B;_LL84: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1AA;if(_tmp1AB->tag != 0)goto _LL86;}_LL85:
 goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1AC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1AA;if(_tmp1AC->tag != 7)goto _LL88;}_LL87:
 goto _LL89;_LL88: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1AA;if(_tmp1AD->tag != 6)goto _LL8A;}_LL89:
# 808
{const char*_tmp3AB;void*_tmp3AA;(_tmp3AA=0,Cyc_Tcutil_warn(s->loc,((_tmp3AB="break may cause function not to return a value",_tag_dyneither(_tmp3AB,sizeof(char),47))),_tag_dyneither(_tmp3AA,sizeof(void*),0)));}
goto _LL83;_LL8A:;_LL8B:
# 811
{const char*_tmp3AE;void*_tmp3AD;(_tmp3AD=0,Cyc_Tcutil_terr(s->loc,((_tmp3AE="break may cause function not to return a value",_tag_dyneither(_tmp3AE,sizeof(char),47))),_tag_dyneither(_tmp3AD,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 814
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1A6=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1A1;if(_tmp1A6->tag != 1)goto _LL7A;}_LL82:
# 817
{const char*_tmp3B1;void*_tmp3B0;(_tmp3B0=0,Cyc_Tcutil_terr(s->loc,((_tmp3B1="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3B1,sizeof(char),56))),_tag_dyneither(_tmp3B0,sizeof(void*),0)));}
return;_LL7A:;};}
# 821
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3B2;struct Cyc_Tcenv_Fenv*_stmttmp1C=Cyc_Tcenv_get_fenv(te,((_tmp3B2="process_goto",_tag_dyneither(_tmp3B2,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1B6;struct Cyc_Tcenv_Fenv*_tmp1B5=_stmttmp1C;_tmp1B6=_tmp1B5->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1B6->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1B7=_tmp1B6->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1B7,l);
struct _RegionHandle*frgn=_tmp1B6->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3B3;slopt=((_tmp3B3=_region_malloc(frgn,sizeof(*_tmp3B3)),((_tmp3B3[0]=0,_tmp3B3))));}{
struct Cyc_List_List*_tmp3B4;struct Cyc_List_List*new_needed=(_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4->hd=s,((_tmp3B4->tl=*slopt,_tmp3B4)))));
_tmp1B6->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1B7,l,new_needed);};}else{
# 833
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3B5;s->non_local_preds=((_tmp3B5=_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5->hd=s,((_tmp3B5->tl=s->non_local_preds,_tmp3B5))))));}
*sopt=s;}};}
# 839
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 841
const char*_tmp3B6;struct Cyc_Tcenv_Fenv*_stmttmp1D=Cyc_Tcenv_get_fenv(te,((_tmp3B6="process_fallthru",_tag_dyneither(_tmp3B6,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp1BD;struct Cyc_Tcenv_Fenv*_tmp1BC=_stmttmp1D;_tmp1BD=_tmp1BC->ctrl_env;{
const struct _tuple10*_tmp1BE=_tmp1BD->fallthru_clause;
if(_tmp1BE != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3B7;(((*_tmp1BE).f1)->body)->non_local_preds=(
(_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->hd=s,((_tmp3B7->tl=(((*_tmp1BE).f1)->body)->non_local_preds,_tmp3B7))))));}{
struct Cyc_Absyn_Switch_clause**_tmp3B8;*clauseopt=((_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=(*_tmp1BE).f1,_tmp3B8))));};}
# 848
return _tmp1BE;};}
# 851
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 855
const char*_tmp3B9;struct Cyc_Tcenv_Fenv*_stmttmp1E=
# 857
Cyc_Tcenv_get_fenv(te,((_tmp3B9="set_fallthru",_tag_dyneither(_tmp3B9,sizeof(char),13))));
# 855
struct Cyc_Tcenv_SharedFenv*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_RgnOrder_RgnPO*_tmp1C5;const struct Cyc_Tcenv_Bindings*_tmp1C6;struct Cyc_Absyn_Stmt*_tmp1C7;struct Cyc_Tcenv_CtrlEnv*_tmp1C8;void*_tmp1C9;void*_tmp1CA;struct Cyc_Tcenv_FenvFlags _tmp1CB;struct Cyc_Tcenv_Fenv*_tmp1C2=_stmttmp1E;_tmp1C3=_tmp1C2->shared;_tmp1C4=_tmp1C2->type_vars;_tmp1C5=_tmp1C2->region_order;_tmp1C6=_tmp1C2->locals;_tmp1C7=_tmp1C2->encloser;_tmp1C8=_tmp1C2->ctrl_env;_tmp1C9=_tmp1C2->capability;_tmp1CA=_tmp1C2->curr_rgn;_tmp1CB=_tmp1C2->flags;{
# 858
struct _RegionHandle*_tmp1CD;void*_tmp1CE;void*_tmp1CF;const struct _tuple10*_tmp1D0;void*_tmp1D1;int _tmp1D2;struct Cyc_Tcenv_CtrlEnv*_tmp1CC=_tmp1C8;_tmp1CD=_tmp1CC->ctrl_rgn;_tmp1CE=_tmp1CC->continue_stmt;_tmp1CF=_tmp1CC->break_stmt;_tmp1D0=_tmp1CC->fallthru_clause;_tmp1D1=_tmp1CC->next_stmt;_tmp1D2=_tmp1CC->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3BA;ft_typ=((_tmp3BA=_region_malloc(_tmp1CD,sizeof(*_tmp3BA)),((_tmp3BA->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3BA->tl=ft_typ,_tmp3BA))))));}{
const struct Cyc_Tcenv_CList*_tmp1D4=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp3BD;struct Cyc_Tcenv_CtrlEnv*_tmp3BC;struct Cyc_Tcenv_CtrlEnv*_tmp1D5=
(_tmp3BC=_region_malloc(r,sizeof(*_tmp3BC)),((_tmp3BC->ctrl_rgn=_tmp1CD,((_tmp3BC->continue_stmt=_tmp1CE,((_tmp3BC->break_stmt=_tmp1CF,((_tmp3BC->fallthru_clause=(const struct _tuple10*)((_tmp3BD=_region_malloc(_tmp1CD,sizeof(*_tmp3BD)),((_tmp3BD->f1=clause,((_tmp3BD->f2=new_tvs,((_tmp3BD->f3=_tmp1D4,_tmp3BD)))))))),((_tmp3BC->next_stmt=_tmp1D1,((_tmp3BC->try_depth=_tmp1D2,_tmp3BC)))))))))))));
# 866
struct Cyc_Tcenv_Fenv*_tmp3BE;struct Cyc_Tcenv_Fenv*_tmp1D6=(_tmp3BE=_region_malloc(r,sizeof(*_tmp3BE)),((_tmp3BE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1C3,((_tmp3BE->type_vars=(struct Cyc_List_List*)_tmp1C4,((_tmp3BE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C5,((_tmp3BE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C6,((_tmp3BE->encloser=(struct Cyc_Absyn_Stmt*)_tmp1C7,((_tmp3BE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1D5,((_tmp3BE->capability=(void*)_tmp1C9,((_tmp3BE->curr_rgn=(void*)_tmp1CA,((_tmp3BE->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1CB,((_tmp3BE->fnrgn=(struct _RegionHandle*)r,_tmp3BE)))))))))))))))))))));
# 870
return Cyc_Tcenv_put_fenv(r,te,_tmp1D6);};};};}
# 873
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3BF;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3BF="clear_fallthru",_tag_dyneither(_tmp3BF,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1DC;struct Cyc_Tcenv_Fenv*_tmp1DB=fe;_tmp1DC=_tmp1DB->ctrl_env;
_tmp1DC->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 880
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 882
const char*_tmp3C0;struct Cyc_Tcenv_Fenv*_stmttmp1F=
# 884
Cyc_Tcenv_get_fenv(te,((_tmp3C0="set_in_loop",_tag_dyneither(_tmp3C0,sizeof(char),12))));
# 882
struct Cyc_Tcenv_SharedFenv*_tmp1DF;struct Cyc_List_List*_tmp1E0;struct Cyc_RgnOrder_RgnPO*_tmp1E1;const struct Cyc_Tcenv_Bindings*_tmp1E2;struct Cyc_Absyn_Stmt*_tmp1E3;struct Cyc_Tcenv_CtrlEnv*_tmp1E4;void*_tmp1E5;void*_tmp1E6;struct Cyc_Tcenv_FenvFlags _tmp1E7;struct Cyc_Tcenv_Fenv*_tmp1DE=_stmttmp1F;_tmp1DF=_tmp1DE->shared;_tmp1E0=_tmp1DE->type_vars;_tmp1E1=_tmp1DE->region_order;_tmp1E2=_tmp1DE->locals;_tmp1E3=_tmp1DE->encloser;_tmp1E4=_tmp1DE->ctrl_env;_tmp1E5=_tmp1DE->capability;_tmp1E6=_tmp1DE->curr_rgn;_tmp1E7=_tmp1DE->flags;{
# 885
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3CB;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3CA;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3C9;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3C8;struct Cyc_Tcenv_CtrlEnv*_tmp3C7;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3C7=_region_malloc(r,sizeof(*_tmp3C7)),((_tmp3C7->ctrl_rgn=r,((_tmp3C7->continue_stmt=(void*)(
(_tmp3C9=_region_malloc(r,sizeof(*_tmp3C9)),((_tmp3C9[0]=((_tmp3C8.tag=3,((_tmp3C8.f1=continue_dest,_tmp3C8)))),_tmp3C9)))),((_tmp3C7->break_stmt=_tmp1E4->next_stmt,((_tmp3C7->next_stmt=(void*)(
# 889
(_tmp3CB=_region_malloc(r,sizeof(*_tmp3CB)),((_tmp3CB[0]=((_tmp3CA.tag=3,((_tmp3CA.f1=continue_dest,_tmp3CA)))),_tmp3CB)))),((_tmp3C7->fallthru_clause=_tmp1E4->fallthru_clause,((_tmp3C7->try_depth=_tmp1E4->try_depth,_tmp3C7)))))))))))));
# 892
struct Cyc_Tcenv_Fenv*_tmp3CC;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3CC=_region_malloc(r,sizeof(*_tmp3CC)),((_tmp3CC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DF,((_tmp3CC->type_vars=(struct Cyc_List_List*)_tmp1E0,((_tmp3CC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E1,((_tmp3CC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E2,((_tmp3CC->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E3,((_tmp3CC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3CC->capability=(void*)_tmp1E5,((_tmp3CC->curr_rgn=(void*)_tmp1E6,((_tmp3CC->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1E7,((_tmp3CC->fnrgn=(struct _RegionHandle*)r,_tmp3CC)))))))))))))))))))));
# 895
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 898
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3CD;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3CD="enter_try",_tag_dyneither(_tmp3CD,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1F0;struct Cyc_Tcenv_Fenv*_tmp1EF=fe;_tmp1F0=_tmp1EF->ctrl_env;
++ _tmp1F0->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 904
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3CE;struct Cyc_Tcenv_Fenv*_stmttmp20=Cyc_Tcenv_get_fenv(te,((_tmp3CE="get_try_depth",_tag_dyneither(_tmp3CE,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp1F3;struct Cyc_Tcenv_Fenv*_tmp1F2=_stmttmp20;_tmp1F3=_tmp1F2->ctrl_env;
return _tmp1F3->try_depth;}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3CF;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3CF="set_in_switch",_tag_dyneither(_tmp3CF,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1F6;struct Cyc_Tcenv_Fenv*_tmp1F5=fe;_tmp1F6=_tmp1F5->ctrl_env;
_tmp1F6->break_stmt=_tmp1F6->next_stmt;
_tmp1F6->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 919
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 921
const char*_tmp3D0;struct Cyc_Tcenv_Fenv _stmttmp21=*
# 923
Cyc_Tcenv_get_fenv(te,((_tmp3D0="set_next",_tag_dyneither(_tmp3D0,sizeof(char),9))));
# 921
struct Cyc_Tcenv_SharedFenv*_tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_RgnOrder_RgnPO*_tmp1FB;const struct Cyc_Tcenv_Bindings*_tmp1FC;struct Cyc_Absyn_Stmt*_tmp1FD;struct Cyc_Tcenv_CtrlEnv*_tmp1FE;void*_tmp1FF;void*_tmp200;struct Cyc_Tcenv_FenvFlags _tmp201;struct Cyc_Tcenv_Fenv _tmp1F8=_stmttmp21;_tmp1F9=_tmp1F8.shared;_tmp1FA=_tmp1F8.type_vars;_tmp1FB=_tmp1F8.region_order;_tmp1FC=_tmp1F8.locals;_tmp1FD=_tmp1F8.encloser;_tmp1FE=_tmp1F8.ctrl_env;_tmp1FF=_tmp1F8.capability;_tmp200=_tmp1F8.curr_rgn;_tmp201=_tmp1F8.flags;{
# 924
struct Cyc_Tcenv_CtrlEnv*_tmp3D1;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3D1=_region_malloc(r,sizeof(*_tmp3D1)),((_tmp3D1->ctrl_rgn=r,((_tmp3D1->continue_stmt=_tmp1FE->continue_stmt,((_tmp3D1->break_stmt=_tmp1FE->break_stmt,((_tmp3D1->next_stmt=j,((_tmp3D1->fallthru_clause=_tmp1FE->fallthru_clause,((_tmp3D1->try_depth=_tmp1FE->try_depth,_tmp3D1)))))))))))));
# 932
struct Cyc_Tcenv_Fenv*_tmp3D2;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3D2=_region_malloc(r,sizeof(*_tmp3D2)),((_tmp3D2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F9,((_tmp3D2->type_vars=(struct Cyc_List_List*)_tmp1FA,((_tmp3D2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FB,((_tmp3D2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FC,((_tmp3D2->encloser=(struct Cyc_Absyn_Stmt*)_tmp1FD,((_tmp3D2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3D2->capability=(void*)_tmp1FF,((_tmp3D2->curr_rgn=(void*)_tmp200,((_tmp3D2->flags=(struct Cyc_Tcenv_FenvFlags)_tmp201,((_tmp3D2->fnrgn=(struct _RegionHandle*)r,_tmp3D2)))))))))))))))))))));
# 935
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 938
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp3D3;struct Cyc_Tcenv_Fenv*_stmttmp22=Cyc_Tcenv_get_fenv(te,((_tmp3D3="add_label",_tag_dyneither(_tmp3D3,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp206;struct Cyc_Tcenv_Fenv*_tmp205=_stmttmp22;_tmp206=_tmp205->shared;{
struct Cyc_Dict_Dict needed=_tmp206->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp206->frgn;
if(sl_opt != 0){
_tmp206->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 946
struct Cyc_List_List*_tmp207=*sl_opt;
s->non_local_preds=_tmp207;
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
void*_stmttmp23=((struct Cyc_Absyn_Stmt*)_tmp207->hd)->r;void*_tmp208=_stmttmp23;struct Cyc_Absyn_Stmt**_tmp20A;_LL8D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp209=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp208;if(_tmp209->tag != 8)goto _LL8F;else{_tmp20A=(struct Cyc_Absyn_Stmt**)& _tmp209->f2;}}_LL8E:
*_tmp20A=s;goto _LL8C;_LL8F:;_LL90:
{const char*_tmp3D4;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D4="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp3D4,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}
# 954
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp206->seen_labels,v)){
const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_PrintArg_struct _tmp3D6;(_tmp3D6.tag=0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D7[0]=& _tmp3D6,Cyc_Tcutil_terr(s->loc,((_tmp3D8="Repeated label: %s",_tag_dyneither(_tmp3D8,sizeof(char),19))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))));}
_tmp206->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp206->seen_labels,v,s);
return te;};}
# 960
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3D9;struct Cyc_Tcenv_Fenv*_stmttmp24=Cyc_Tcenv_get_fenv(te,((_tmp3D9="all_labels_resolved",_tag_dyneither(_tmp3D9,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp211;struct Cyc_Tcenv_Fenv*_tmp210=_stmttmp24;_tmp211=_tmp210->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp211->needed_labels);}
# 965
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3DA;struct Cyc_Tcenv_Fenv*_stmttmp25=Cyc_Tcenv_get_fenv(te,((_tmp3DA="get_encloser",_tag_dyneither(_tmp3DA,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp214;struct Cyc_Tcenv_Fenv*_tmp213=_stmttmp25;_tmp214=_tmp213->encloser;
return _tmp214;}
# 970
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp3DB;return(_tmp3DB=_region_malloc(r2,sizeof(*_tmp3DB)),((_tmp3DB->ns=te->ns,((_tmp3DB->ae=te->ae,((_tmp3DB->le=0,((_tmp3DB->allow_valueof=1,_tmp3DB)))))))));}{
struct Cyc_Tcenv_Fenv _stmttmp26=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_RgnOrder_RgnPO*_tmp21A;const struct Cyc_Tcenv_Bindings*_tmp21B;struct Cyc_Absyn_Stmt*_tmp21C;struct Cyc_Tcenv_CtrlEnv*_tmp21D;void*_tmp21E;void*_tmp21F;struct Cyc_Tcenv_FenvFlags _tmp220;struct Cyc_Tcenv_Fenv _tmp217=_stmttmp26;_tmp218=_tmp217.shared;_tmp219=_tmp217.type_vars;_tmp21A=_tmp217.region_order;_tmp21B=_tmp217.locals;_tmp21C=_tmp217.encloser;_tmp21D=_tmp217.ctrl_env;_tmp21E=_tmp217.capability;_tmp21F=_tmp217.curr_rgn;_tmp220=_tmp217.flags;{
struct Cyc_Tcenv_Fenv*_tmp3DC;struct Cyc_Tcenv_Fenv*_tmp221=(_tmp3DC=_region_malloc(r2,sizeof(*_tmp3DC)),((_tmp3DC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp218,((_tmp3DC->type_vars=(struct Cyc_List_List*)_tmp219,((_tmp3DC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21A,((_tmp3DC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21B,((_tmp3DC->encloser=(struct Cyc_Absyn_Stmt*)_tmp21C,((_tmp3DC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21D,((_tmp3DC->capability=(void*)_tmp21E,((_tmp3DC->curr_rgn=(void*)_tmp21F,((_tmp3DC->flags=(struct Cyc_Tcenv_FenvFlags)_tmp220,((_tmp3DC->fnrgn=(struct _RegionHandle*)r2,_tmp3DC)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp3DD;return(_tmp3DD=_region_malloc(r2,sizeof(*_tmp3DD)),((_tmp3DD->ns=te->ns,((_tmp3DD->ae=te->ae,((_tmp3DD->le=_tmp221,((_tmp3DD->allow_valueof=1,_tmp3DD)))))))));};};}
# 978
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 980
const char*_tmp3DE;struct Cyc_Tcenv_Fenv _stmttmp27=*
# 982
Cyc_Tcenv_get_fenv(te,((_tmp3DE="set_encloser",_tag_dyneither(_tmp3DE,sizeof(char),13))));
# 980
struct Cyc_Tcenv_SharedFenv*_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_RgnOrder_RgnPO*_tmp227;const struct Cyc_Tcenv_Bindings*_tmp228;struct Cyc_Absyn_Stmt*_tmp229;struct Cyc_Tcenv_CtrlEnv*_tmp22A;void*_tmp22B;void*_tmp22C;struct Cyc_Tcenv_FenvFlags _tmp22D;struct Cyc_Tcenv_Fenv _tmp224=_stmttmp27;_tmp225=_tmp224.shared;_tmp226=_tmp224.type_vars;_tmp227=_tmp224.region_order;_tmp228=_tmp224.locals;_tmp229=_tmp224.encloser;_tmp22A=_tmp224.ctrl_env;_tmp22B=_tmp224.capability;_tmp22C=_tmp224.curr_rgn;_tmp22D=_tmp224.flags;{
# 983
struct Cyc_Tcenv_Fenv*_tmp3DF;struct Cyc_Tcenv_Fenv*_tmp22E=
(_tmp3DF=_region_malloc(r,sizeof(*_tmp3DF)),((_tmp3DF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp225,((_tmp3DF->type_vars=(struct Cyc_List_List*)_tmp226,((_tmp3DF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp227,((_tmp3DF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp228,((_tmp3DF->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp3DF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp22A,((_tmp3DF->capability=(void*)_tmp22B,((_tmp3DF->curr_rgn=(void*)_tmp22C,((_tmp3DF->flags=(struct Cyc_Tcenv_FenvFlags)_tmp22D,((_tmp3DF->fnrgn=(struct _RegionHandle*)r,_tmp3DF)))))))))))))))))))));
# 986
return Cyc_Tcenv_put_fenv(r,te,_tmp22E);};}
# 989
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 992
struct Cyc_Absyn_Tvar*tv;
{void*_stmttmp28=Cyc_Tcutil_compress(rgn);void*_tmp231=_stmttmp28;struct Cyc_Absyn_Tvar*_tmp233;_LL92: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp231;if(_tmp232->tag != 2)goto _LL94;else{_tmp233=_tmp232->f1;}}_LL93:
 tv=_tmp233;goto _LL91;_LL94:;_LL95:
{const char*_tmp3E0;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3E0="bad add region",_tag_dyneither(_tmp3E0,sizeof(char),15))));}goto _LL91;_LL91:;}{
# 997
const char*_tmp3E1;struct Cyc_Tcenv_Fenv _stmttmp29=*
# 999
Cyc_Tcenv_get_fenv(te,((_tmp3E1="add_region",_tag_dyneither(_tmp3E1,sizeof(char),11))));
# 997
struct Cyc_Tcenv_SharedFenv*_tmp236;struct Cyc_List_List*_tmp237;struct Cyc_RgnOrder_RgnPO*_tmp238;const struct Cyc_Tcenv_Bindings*_tmp239;struct Cyc_Absyn_Stmt*_tmp23A;struct Cyc_Tcenv_CtrlEnv*_tmp23B;void*_tmp23C;void*_tmp23D;struct Cyc_Tcenv_FenvFlags _tmp23E;struct Cyc_Tcenv_Fenv _tmp235=_stmttmp29;_tmp236=_tmp235.shared;_tmp237=_tmp235.type_vars;_tmp238=_tmp235.region_order;_tmp239=_tmp235.locals;_tmp23A=_tmp235.encloser;_tmp23B=_tmp235.ctrl_env;_tmp23C=_tmp235.capability;_tmp23D=_tmp235.curr_rgn;_tmp23E=_tmp235.flags;
# 1000
_tmp238=Cyc_RgnOrder_add_youngest(_tmp236->frgn,_tmp238,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp3F4;struct Cyc_List_List*_tmp3F3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3F2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3F1;struct Cyc_List_List*_tmp3F0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3EF;_tmp23C=(void*)((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3F4.tag=24,((_tmp3F4.f1=((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=(void*)((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=((_tmp3F1.tag=23,((_tmp3F1.f1=rgn,_tmp3F1)))),_tmp3F2)))),((_tmp3F0->tl=(
(_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3->hd=_tmp23C,((_tmp3F3->tl=0,_tmp3F3)))))),_tmp3F0)))))),_tmp3F4)))),_tmp3EF))));}{
struct Cyc_Tcenv_Fenv*_tmp3F5;struct Cyc_Tcenv_Fenv*_tmp245=
(_tmp3F5=_region_malloc(r,sizeof(*_tmp3F5)),((_tmp3F5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp236,((_tmp3F5->type_vars=(struct Cyc_List_List*)_tmp237,((_tmp3F5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp238,((_tmp3F5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp239,((_tmp3F5->encloser=(struct Cyc_Absyn_Stmt*)_tmp23A,((_tmp3F5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp23B,((_tmp3F5->capability=(void*)_tmp23C,((_tmp3F5->curr_rgn=(void*)_tmp23D,((_tmp3F5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp23E,((_tmp3F5->fnrgn=(struct _RegionHandle*)r,_tmp3F5)))))))))))))))))))));
# 1006
return Cyc_Tcenv_put_fenv(r,te,_tmp245);};};}
# 1009
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1011
const char*_tmp3F6;struct Cyc_Tcenv_Fenv _stmttmp2A=*
# 1013
Cyc_Tcenv_get_fenv(te,((_tmp3F6="new_named_block",_tag_dyneither(_tmp3F6,sizeof(char),16))));
# 1011
struct Cyc_Tcenv_SharedFenv*_tmp249;struct Cyc_List_List*_tmp24A;struct Cyc_RgnOrder_RgnPO*_tmp24B;const struct Cyc_Tcenv_Bindings*_tmp24C;struct Cyc_Absyn_Stmt*_tmp24D;struct Cyc_Tcenv_CtrlEnv*_tmp24E;void*_tmp24F;void*_tmp250;struct Cyc_Tcenv_FenvFlags _tmp251;struct Cyc_Tcenv_Fenv _tmp248=_stmttmp2A;_tmp249=_tmp248.shared;_tmp24A=_tmp248.type_vars;_tmp24B=_tmp248.region_order;_tmp24C=_tmp248.locals;_tmp24D=_tmp248.encloser;_tmp24E=_tmp248.ctrl_env;_tmp24F=_tmp248.capability;_tmp250=_tmp248.curr_rgn;_tmp251=_tmp248.flags;{
# 1015
const char*_tmp3F7;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3F7="new_block",_tag_dyneither(_tmp3F7,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3FA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F9;void*block_typ=(void*)((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3FA.tag=2,((_tmp3FA.f1=block_rgn,_tmp3FA)))),_tmp3F9))));
{struct Cyc_List_List*_tmp3FB;_tmp24A=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=block_rgn,((_tmp3FB->tl=_tmp24A,_tmp3FB))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp24A);
_tmp24B=Cyc_RgnOrder_add_youngest(_tmp249->frgn,_tmp24B,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp40E;struct Cyc_List_List*_tmp40D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp40C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp40B;struct Cyc_List_List*_tmp40A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp409;_tmp24F=(void*)((_tmp409=_cycalloc(sizeof(*_tmp409)),((_tmp409[0]=((_tmp40E.tag=24,((_tmp40E.f1=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A->hd=(void*)((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=((_tmp40B.tag=23,((_tmp40B.f1=block_typ,_tmp40B)))),_tmp40C)))),((_tmp40A->tl=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->hd=_tmp24F,((_tmp40D->tl=0,_tmp40D)))))),_tmp40A)))))),_tmp40E)))),_tmp409))));}
_tmp250=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp40F;struct Cyc_Tcenv_Fenv*_tmp259=
(_tmp40F=_region_malloc(r,sizeof(*_tmp40F)),((_tmp40F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp249,((_tmp40F->type_vars=(struct Cyc_List_List*)_tmp24A,((_tmp40F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24B,((_tmp40F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24C,((_tmp40F->encloser=(struct Cyc_Absyn_Stmt*)_tmp24D,((_tmp40F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp24E,((_tmp40F->capability=(void*)_tmp24F,((_tmp40F->curr_rgn=(void*)_tmp250,((_tmp40F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp251,((_tmp40F->fnrgn=(struct _RegionHandle*)r,_tmp40F)))))))))))))))))))));
# 1025
return Cyc_Tcenv_put_fenv(r,te,_tmp259);};};}
# 1028
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1030
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1032
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1038
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1042
const char*_tmp410;struct Cyc_Tcenv_Fenv _stmttmp2B=*
# 1044
Cyc_Tcenv_get_fenv(te,((_tmp410="new_outlives_constraints",_tag_dyneither(_tmp410,sizeof(char),25))));
# 1042
struct Cyc_Tcenv_SharedFenv*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_RgnOrder_RgnPO*_tmp263;const struct Cyc_Tcenv_Bindings*_tmp264;struct Cyc_Absyn_Stmt*_tmp265;struct Cyc_Tcenv_CtrlEnv*_tmp266;void*_tmp267;void*_tmp268;struct Cyc_Tcenv_FenvFlags _tmp269;struct Cyc_Tcenv_Fenv _tmp260=_stmttmp2B;_tmp261=_tmp260.shared;_tmp262=_tmp260.type_vars;_tmp263=_tmp260.region_order;_tmp264=_tmp260.locals;_tmp265=_tmp260.encloser;_tmp266=_tmp260.ctrl_env;_tmp267=_tmp260.capability;_tmp268=_tmp260.curr_rgn;_tmp269=_tmp260.flags;
# 1045
for(0;cs != 0;cs=cs->tl){
_tmp263=
Cyc_RgnOrder_add_outlives_constraint(_tmp261->frgn,_tmp263,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1050
struct Cyc_Tcenv_Fenv*_tmp411;struct Cyc_Tcenv_Fenv*_tmp26A=
(_tmp411=_region_malloc(r,sizeof(*_tmp411)),((_tmp411->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp261,((_tmp411->type_vars=(struct Cyc_List_List*)_tmp262,((_tmp411->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp263,((_tmp411->locals=(const struct Cyc_Tcenv_Bindings*)_tmp264,((_tmp411->encloser=(struct Cyc_Absyn_Stmt*)_tmp265,((_tmp411->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp266,((_tmp411->capability=(void*)_tmp267,((_tmp411->curr_rgn=(void*)_tmp268,((_tmp411->flags=(struct Cyc_Tcenv_FenvFlags)_tmp269,((_tmp411->fnrgn=(struct _RegionHandle*)r,_tmp411)))))))))))))))))))));
# 1053
return Cyc_Tcenv_put_fenv(r,te,_tmp26A);};}
# 1056
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1064
void*_tmp26D=Cyc_Tcutil_compress(r1);
void*_tmp26E=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp26F=Cyc_Tcutil_typ_kind(_tmp26D);
struct Cyc_Absyn_Kind*_tmp270=Cyc_Tcutil_typ_kind(_tmp26E);
# 1070
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp26F,_tmp270);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp270,_tmp26F);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp418;void*_tmp417[4];struct Cyc_String_pa_PrintArg_struct _tmp416;struct Cyc_String_pa_PrintArg_struct _tmp415;struct Cyc_String_pa_PrintArg_struct _tmp414;struct Cyc_String_pa_PrintArg_struct _tmp413;(_tmp413.tag=0,((_tmp413.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp270)),((_tmp414.tag=0,((_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp26F)),((_tmp415.tag=0,((_tmp415.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp26E)),((_tmp416.tag=0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp26D)),((_tmp417[0]=& _tmp416,((_tmp417[1]=& _tmp415,((_tmp417[2]=& _tmp414,((_tmp417[3]=& _tmp413,Cyc_Tcutil_terr(loc,((_tmp418="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp418,sizeof(char),82))),_tag_dyneither(_tmp417,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1077
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _stmttmp2C=Cyc_Tcutil_swap_kind(_tmp26E,Cyc_Tcutil_kind_to_bound(_tmp26F));struct Cyc_Absyn_Tvar*_tmp278;void*_tmp279;struct _tuple11 _tmp277=_stmttmp2C;_tmp278=_tmp277.f1;_tmp279=_tmp277.f2;{
struct _tuple11*_tmp419;*oldtv=((_tmp419=_region_malloc(r,sizeof(*_tmp419)),((_tmp419->f1=_tmp278,((_tmp419->f2=_tmp279,_tmp419))))));};}else{
# 1081
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _stmttmp2D=Cyc_Tcutil_swap_kind(_tmp26D,Cyc_Tcutil_kind_to_bound(_tmp270));struct Cyc_Absyn_Tvar*_tmp27C;void*_tmp27D;struct _tuple11 _tmp27B=_stmttmp2D;_tmp27C=_tmp27B.f1;_tmp27D=_tmp27B.f2;{
struct _tuple11*_tmp41A;*oldtv=((_tmp41A=_region_malloc(r,sizeof(*_tmp41A)),((_tmp41A->f1=_tmp27C,((_tmp41A->f2=_tmp27D,_tmp41A))))));};}}}{
# 1087
struct _RegionHandle*_tmp27F=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp280=0;
if((_tmp26D != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp26D != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp26D != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp41D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp41C;void*eff1=(void*)((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C[0]=((_tmp41D.tag=23,((_tmp41D.f1=_tmp26D,_tmp41D)))),_tmp41C))));
struct _tuple13*_tmp420;struct Cyc_List_List*_tmp41F;_tmp280=((_tmp41F=_region_malloc(_tmp27F,sizeof(*_tmp41F)),((_tmp41F->hd=((_tmp420=_region_malloc(_tmp27F,sizeof(*_tmp420)),((_tmp420->f1=eff1,((_tmp420->f2=_tmp26E,_tmp420)))))),((_tmp41F->tl=_tmp280,_tmp41F))))));}
# 1093
if((_tmp26E != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp26E != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp26E != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp423;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp422;void*eff2=(void*)((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=((_tmp423.tag=23,((_tmp423.f1=_tmp26E,_tmp423)))),_tmp422))));
struct _tuple13*_tmp426;struct Cyc_List_List*_tmp425;_tmp280=((_tmp425=_region_malloc(_tmp27F,sizeof(*_tmp425)),((_tmp425->hd=((_tmp426=_region_malloc(_tmp27F,sizeof(*_tmp426)),((_tmp426->f1=eff2,((_tmp426->f2=_tmp26D,_tmp426)))))),((_tmp425->tl=_tmp280,_tmp425))))));}
# 1098
return Cyc_Tcenv_new_outlives_constraints(_tmp27F,te,_tmp280,loc);};}
# 1101
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp289=te->le;
if(_tmp289 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp289;
void*_tmp28B;struct Cyc_Tcenv_Fenv*_tmp28A=fe;_tmp28B=_tmp28A->curr_rgn;
return _tmp28B;};}
# 1112
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp427;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp427="check_rgn_accessible",_tag_dyneither(_tmp427,sizeof(char),21))));
void*_tmp28D;struct Cyc_RgnOrder_RgnPO*_tmp28E;struct Cyc_Tcenv_Fenv*_tmp28C=fe;_tmp28D=_tmp28C->capability;_tmp28E=_tmp28C->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp28D) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp28D))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp42A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp429;if(Cyc_RgnOrder_eff_outlives_eff(_tmp28E,(void*)((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429[0]=((_tmp42A.tag=23,((_tmp42A.f1=rgn,_tmp42A)))),_tmp429)))),_tmp28D))
return;}{
const char*_tmp42E;void*_tmp42D[1];struct Cyc_String_pa_PrintArg_struct _tmp42C;(_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp42D[0]=& _tmp42C,Cyc_Tcutil_terr(loc,((_tmp42E="Expression accesses unavailable region %s",_tag_dyneither(_tmp42E,sizeof(char),42))),_tag_dyneither(_tmp42D,sizeof(void*),1)))))));};}
# 1124
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp42F;struct Cyc_Tcenv_Fenv*_stmttmp2E=
Cyc_Tcenv_get_fenv(te,((_tmp42F="check_rgn_resetable",_tag_dyneither(_tmp42F,sizeof(char),20))));
# 1126
struct Cyc_RgnOrder_RgnPO*_tmp296;struct Cyc_Tcenv_Fenv*_tmp295=_stmttmp2E;_tmp296=_tmp295->region_order;{
# 1128
void*_stmttmp2F=Cyc_Tcutil_compress(rgn);void*_tmp297=_stmttmp2F;struct Cyc_Absyn_Tvar*_tmp299;_LL97: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp298=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp297;if(_tmp298->tag != 2)goto _LL99;else{_tmp299=_tmp298->f1;}}_LL98:
# 1130
 if(!Cyc_RgnOrder_is_region_resetable(_tmp296,_tmp299)){
const char*_tmp433;void*_tmp432[1];struct Cyc_String_pa_PrintArg_struct _tmp431;(_tmp431.tag=0,((_tmp431.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp432[0]=& _tmp431,Cyc_Tcutil_terr(loc,((_tmp433="Region %s is not resetable",_tag_dyneither(_tmp433,sizeof(char),27))),_tag_dyneither(_tmp432,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {
# 1134
const char*_tmp436;void*_tmp435;(_tmp435=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp436="check_rgn_resetable",_tag_dyneither(_tmp436,sizeof(char),20))),_tag_dyneither(_tmp435,sizeof(void*),0)));}_LL96:;};};}
# 1141
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2A0=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1146
if(_tmp2A0 == 0){
void*_tmp2A1=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2A2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2A1;if(_tmp2A2->tag != 22)goto _LL9E;}_LL9D:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2A3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2A1;if(_tmp2A3->tag != 21)goto _LLA0;}_LL9F:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2A4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2A1;if(_tmp2A4->tag != 20)goto _LLA2;}_LLA1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3:
 return 0;_LL9B:;}{
# 1154
struct Cyc_Tcenv_Fenv*fe=_tmp2A0;
struct Cyc_RgnOrder_RgnPO*_tmp2A6;struct Cyc_Tcenv_Fenv*_tmp2A5=fe;_tmp2A6=_tmp2A5->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp439;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp438;int _tmp2A7=Cyc_RgnOrder_effect_outlives(_tmp2A6,(void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp439.tag=23,((_tmp439.f1=rt_a,_tmp439)))),_tmp438)))),rt_b);
# 1160
return _tmp2A7;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1165
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp43A;struct Cyc_Tcenv_Fenv*_stmttmp30=
Cyc_Tcenv_get_fenv(te,((_tmp43A="check_effect_accessible",_tag_dyneither(_tmp43A,sizeof(char),24))));
# 1166
void*_tmp2AB;struct Cyc_RgnOrder_RgnPO*_tmp2AC;struct Cyc_Tcenv_SharedFenv*_tmp2AD;struct Cyc_Tcenv_Fenv*_tmp2AA=_stmttmp30;_tmp2AB=_tmp2AA->capability;_tmp2AC=_tmp2AA->region_order;_tmp2AD=_tmp2AA->shared;
# 1168
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2AB))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2AC,eff,_tmp2AB))
return;{
struct _RegionHandle*frgn=_tmp2AD->frgn;
struct _tuple14*_tmp43D;struct Cyc_List_List*_tmp43C;_tmp2AD->delayed_effect_checks=(
(_tmp43C=_region_malloc(frgn,sizeof(*_tmp43C)),((_tmp43C->hd=((_tmp43D=_region_malloc(frgn,sizeof(*_tmp43D)),((_tmp43D->f1=_tmp2AB,((_tmp43D->f2=eff,((_tmp43D->f3=_tmp2AC,((_tmp43D->f4=loc,_tmp43D)))))))))),((_tmp43C->tl=_tmp2AD->delayed_effect_checks,_tmp43C))))));};}
# 1177
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp43E;struct Cyc_Tcenv_Fenv*_stmttmp31=
Cyc_Tcenv_get_fenv(te,((_tmp43E="check_delayed_effects",_tag_dyneither(_tmp43E,sizeof(char),22))));
# 1178
struct Cyc_Tcenv_SharedFenv*_tmp2B2;struct Cyc_Tcenv_Fenv*_tmp2B1=_stmttmp31;_tmp2B2=_tmp2B1->shared;{
# 1180
struct Cyc_List_List*_tmp2B3=_tmp2B2->delayed_effect_checks;
for(0;_tmp2B3 != 0;_tmp2B3=_tmp2B3->tl){
struct _tuple14*_stmttmp32=(struct _tuple14*)_tmp2B3->hd;void*_tmp2B5;void*_tmp2B6;struct Cyc_RgnOrder_RgnPO*_tmp2B7;unsigned int _tmp2B8;struct _tuple14*_tmp2B4=_stmttmp32;_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;_tmp2B7=_tmp2B4->f3;_tmp2B8=_tmp2B4->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2B6,_tmp2B5))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2B7,_tmp2B6,_tmp2B5))
continue;{
const char*_tmp443;void*_tmp442[2];struct Cyc_String_pa_PrintArg_struct _tmp441;struct Cyc_String_pa_PrintArg_struct _tmp440;(_tmp440.tag=0,((_tmp440.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2B6)),((_tmp441.tag=0,((_tmp441.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2B5)),((_tmp442[0]=& _tmp441,((_tmp442[1]=& _tmp440,Cyc_Tcutil_terr(_tmp2B8,((_tmp443="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp443,sizeof(char),51))),_tag_dyneither(_tmp442,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1196
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1198
struct Cyc_Tcenv_Fenv*_tmp2BE=te->le;
if(_tmp2BE == 0){
# 1201
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp446;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp445;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445[0]=((_tmp446.tag=23,((_tmp446.f1=(*((struct _tuple13*)po->hd)).f2,_tmp446)))),_tmp445)))),Cyc_Absyn_empty_effect)){
const char*_tmp449;void*_tmp448;(_tmp448=0,Cyc_Tcutil_terr(loc,((_tmp449="the required region ordering is not satisfied here",_tag_dyneither(_tmp449,sizeof(char),51))),_tag_dyneither(_tmp448,sizeof(void*),0)));}}
return;}{
# 1207
struct Cyc_Tcenv_Fenv*_stmttmp33=_tmp2BE;struct Cyc_RgnOrder_RgnPO*_tmp2C4;struct Cyc_Tcenv_SharedFenv*_tmp2C5;struct Cyc_Tcenv_Fenv*_tmp2C3=_stmttmp33;_tmp2C4=_tmp2C3->region_order;_tmp2C5=_tmp2C3->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2C4,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp44C;struct Cyc_List_List*_tmp44B;_tmp2C5->delayed_constraint_checks=(
(_tmp44B=_region_malloc(_tmp2C5->frgn,sizeof(*_tmp44B)),((_tmp44B->hd=((_tmp44C=_region_malloc(_tmp2C5->frgn,sizeof(*_tmp44C)),((_tmp44C->f1=_tmp2C4,((_tmp44C->f2=po,((_tmp44C->f3=loc,_tmp44C)))))))),((_tmp44B->tl=_tmp2C5->delayed_constraint_checks,_tmp44B))))));}};}
# 1214
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp44D;struct Cyc_Tcenv_Fenv*_stmttmp34=
Cyc_Tcenv_get_fenv(te,((_tmp44D="check_delayed_constraints",_tag_dyneither(_tmp44D,sizeof(char),26))));
# 1215
struct Cyc_Tcenv_SharedFenv*_tmp2C9;struct Cyc_Tcenv_Fenv*_tmp2C8=_stmttmp34;_tmp2C9=_tmp2C8->shared;{
# 1217
struct Cyc_List_List*_tmp2CA=_tmp2C9->delayed_constraint_checks;
for(0;_tmp2CA != 0;_tmp2CA=_tmp2CA->tl){
struct _tuple15*_stmttmp35=(struct _tuple15*)_tmp2CA->hd;struct Cyc_RgnOrder_RgnPO*_tmp2CC;struct Cyc_List_List*_tmp2CD;unsigned int _tmp2CE;struct _tuple15*_tmp2CB=_stmttmp35;_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;_tmp2CE=_tmp2CB->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2CC,_tmp2CD,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp450;void*_tmp44F;(_tmp44F=0,Cyc_Tcutil_terr(_tmp2CE,((_tmp450="the required region ordering is not satisfied here",_tag_dyneither(_tmp450,sizeof(char),51))),_tag_dyneither(_tmp44F,sizeof(void*),0)));}}};}
# 1225
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp45D;const char*_tmp45C;void*_tmp45B[1];struct Cyc_String_pa_PrintArg_struct _tmp45A;struct Cyc_Absyn_Tvar*_tmp459;struct Cyc_Absyn_Tvar*rgn0=
(_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459->name=((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=(struct _dyneither_ptr)((_tmp45A.tag=0,((_tmp45A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp45B[0]=& _tmp45A,Cyc_aprintf(((_tmp45C="`%s",_tag_dyneither(_tmp45C,sizeof(char),4))),_tag_dyneither(_tmp45B,sizeof(void*),1)))))))),_tmp45D)))),((_tmp459->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp459->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp459)))))));
struct Cyc_List_List*_tmp45E;struct Cyc_List_List*_tmp2D2=(_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E->hd=rgn0,((_tmp45E->tl=fd->tvs,_tmp45E)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2D2);{
# 1233
struct Cyc_RgnOrder_RgnPO*_tmp2D3=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1235
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp461;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp460;void*param_rgn=(void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp461.tag=2,((_tmp461.f1=rgn0,_tmp461)))),_tmp460))));
struct Cyc_List_List*_tmp2D4=0;
{struct Cyc_List_List*_tmp2D5=fd->args;for(0;_tmp2D5 != 0;_tmp2D5=_tmp2D5->tl){
struct Cyc_Absyn_Vardecl _tmp467;struct _tuple0*_tmp466;struct Cyc_Absyn_Vardecl*_tmp465;struct Cyc_Absyn_Vardecl*_tmp2D6=(_tmp465=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp465[0]=(struct Cyc_Absyn_Vardecl)((_tmp467.sc=Cyc_Absyn_Public,((_tmp467.name=(
(_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466->f1=Cyc_Absyn_Loc_n,((_tmp466->f2=(*((struct _tuple8*)_tmp2D5->hd)).f1,_tmp466)))))),((_tmp467.tq=(*((struct _tuple8*)_tmp2D5->hd)).f2,((_tmp467.type=(*((struct _tuple8*)_tmp2D5->hd)).f3,((_tmp467.initializer=0,((_tmp467.rgn=param_rgn,((_tmp467.attributes=0,((_tmp467.escapes=0,_tmp467)))))))))))))))),_tmp465)));
# 1245
{struct Cyc_List_List _tmp46A;struct Cyc_List_List*_tmp469;_tmp2D4=((_tmp469=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp469[0]=(struct Cyc_List_List)((_tmp46A.hd=_tmp2D6,((_tmp46A.tl=_tmp2D4,_tmp46A)))),_tmp469))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp46D;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp46C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2D9=(_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C[0]=((_tmp46D.tag=3,((_tmp46D.f1=_tmp2D6,_tmp46D)))),_tmp46C)));
struct _dyneither_ptr*_tmp2DA=(*((struct _tuple8*)_tmp2D5->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp46E;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp46E=_region_malloc(r,sizeof(*_tmp46E)),((_tmp46E->v=_tmp2DA,((_tmp46E->b=(void*)_tmp2D9,((_tmp46E->tl=locals,_tmp46E))))))));};}}
# 1250
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp36=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp2E2;struct Cyc_Absyn_Tqual _tmp2E3;void*_tmp2E4;int _tmp2E5;struct Cyc_Absyn_VarargInfo _tmp2E1=_stmttmp36;_tmp2E2=_tmp2E1.name;_tmp2E3=_tmp2E1.tq;_tmp2E4=_tmp2E1.type;_tmp2E5=_tmp2E1.inject;
if(_tmp2E2 != 0){
void*_tmp2E6=Cyc_Absyn_dyneither_typ(_tmp2E4,param_rgn,_tmp2E3,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp474;struct _tuple0*_tmp473;struct Cyc_Absyn_Vardecl*_tmp472;struct Cyc_Absyn_Vardecl*_tmp2E7=(_tmp472=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp472[0]=(struct Cyc_Absyn_Vardecl)((_tmp474.sc=Cyc_Absyn_Public,((_tmp474.name=(
(_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473->f1=Cyc_Absyn_Loc_n,((_tmp473->f2=_tmp2E2,_tmp473)))))),((_tmp474.tq=
Cyc_Absyn_empty_tqual(0),((_tmp474.type=_tmp2E6,((_tmp474.initializer=0,((_tmp474.rgn=param_rgn,((_tmp474.attributes=0,((_tmp474.escapes=0,_tmp474)))))))))))))))),_tmp472)));
# 1261
{struct Cyc_List_List*_tmp475;_tmp2D4=((_tmp475=_cycalloc(sizeof(*_tmp475)),((_tmp475->hd=_tmp2E7,((_tmp475->tl=_tmp2D4,_tmp475))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp478;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp477;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E9=(_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477[0]=((_tmp478.tag=3,((_tmp478.f1=_tmp2E7,_tmp478)))),_tmp477)));
struct _dyneither_ptr*_tmp2EA=_tmp2E2;
struct Cyc_Tcenv_Bindings*_tmp479;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp479=_region_malloc(r,sizeof(*_tmp479)),((_tmp479->v=_tmp2EA,((_tmp479->b=(void*)_tmp2E9,((_tmp479->tl=locals,_tmp479))))))));};}else{
# 1266
const char*_tmp47C;void*_tmp47B;(_tmp47B=0,Cyc_Tcutil_terr(loc,((_tmp47C="missing name for varargs",_tag_dyneither(_tmp47C,sizeof(char),25))),_tag_dyneither(_tmp47B,sizeof(void*),0)));}}
# 1268
{struct Cyc_Core_Opt _tmp47F;struct Cyc_Core_Opt*_tmp47E;fd->param_vardecls=((_tmp47E=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp47E[0]=(struct Cyc_Core_Opt)((_tmp47F.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D4),_tmp47F)),_tmp47E))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49D;struct Cyc_List_List*_tmp49C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp49B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp498;struct Cyc_Tcenv_CtrlEnv*_tmp497;struct Cyc_Tcenv_SharedFenv*_tmp496;struct Cyc_Tcenv_Fenv*_tmp495;return(_tmp495=_region_malloc(r,sizeof(*_tmp495)),((_tmp495->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp496=_region_malloc(r,sizeof(*_tmp496)),((_tmp496->frgn=r,((_tmp496->return_typ=fd->ret_type,((_tmp496->seen_labels=
# 1273
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp496->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp496->delayed_effect_checks=0,((_tmp496->delayed_constraint_checks=0,_tmp496)))))))))))))),((_tmp495->type_vars=(struct Cyc_List_List*)_tmp2D2,((_tmp495->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2D3,((_tmp495->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp495->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp495->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1282
(_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497->ctrl_rgn=r,((_tmp497->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp497->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp497->fallthru_clause=0,((_tmp497->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp497->try_depth=0,_tmp497)))))))))))))),((_tmp495->capability=(void*)((void*)(
(_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp498.tag=24,((_tmp498.f1=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C->hd=(void*)((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp49B.tag=23,((_tmp49B.f1=param_rgn,_tmp49B)))),_tmp49A)))),((_tmp49C->tl=(
(_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499->hd=(void*)_check_null(fd->effect),((_tmp499->tl=0,_tmp499)))))),_tmp49C)))))),_tmp498)))),_tmp49D))))),((_tmp495->curr_rgn=(void*)param_rgn,((_tmp495->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1286
((_tmp495->flags).in_notreadctxt=0,(((_tmp495->flags).in_lhs=0,(((_tmp495->flags).in_new=0,(((_tmp495->flags).abstract_ok=0,_tmp495->flags)))))))),((_tmp495->fnrgn=(struct _RegionHandle*)r,_tmp495)))))))))))))))))))));};};}
# 1291
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp307;struct Cyc_RgnOrder_RgnPO*_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_Tcenv_SharedFenv*_tmp30A;struct _RegionHandle*_tmp30B;struct Cyc_Tcenv_Fenv*_tmp306=old_fenv;_tmp307=_tmp306->locals;_tmp308=_tmp306->region_order;_tmp309=_tmp306->type_vars;_tmp30A=_tmp306->shared;_tmp30B=_tmp306->fnrgn;{
# 1294
struct _RegionHandle*_tmp30C=_tmp30A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp30D=_tmp307;
struct _dyneither_ptr*_tmp4AA;const char*_tmp4A9;void*_tmp4A8[1];struct Cyc_String_pa_PrintArg_struct _tmp4A7;struct Cyc_Absyn_Tvar*_tmp4A6;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6->name=((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA[0]=(struct _dyneither_ptr)((_tmp4A7.tag=0,((_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4A8[0]=& _tmp4A7,Cyc_aprintf(((_tmp4A9="`%s",_tag_dyneither(_tmp4A9,sizeof(char),4))),_tag_dyneither(_tmp4A8,sizeof(void*),1)))))))),_tmp4AA)))),((_tmp4A6->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4A6->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4A6)))))));
# 1300
{struct Cyc_List_List*_tmp30E=fd->tvs;for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct Cyc_Absyn_Kind*_stmttmp37=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp30E->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp310;enum Cyc_Absyn_AliasQual _tmp311;struct Cyc_Absyn_Kind*_tmp30F=_stmttmp37;_tmp310=_tmp30F->kind;_tmp311=_tmp30F->aliasqual;
if(_tmp310 == Cyc_Absyn_RgnKind){
if(_tmp311 == Cyc_Absyn_Aliasable)
_tmp308=Cyc_RgnOrder_add_youngest(_tmp30C,_tmp308,(struct Cyc_Absyn_Tvar*)_tmp30E->hd,0,0);else{
# 1306
const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AD="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4AD,sizeof(char),39))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}}}}
# 1308
_tmp308=Cyc_RgnOrder_add_youngest(_tmp30C,_tmp308,rgn0,0,0);{
struct Cyc_List_List*_tmp4AE;struct Cyc_List_List*_tmp314=(_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->hd=rgn0,((_tmp4AE->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp309),_tmp4AE)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp314);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4B0;void*param_rgn=(void*)((_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=2,((_tmp4B1.f1=rgn0,_tmp4B1)))),_tmp4B0))));
struct Cyc_List_List*_tmp315=0;
{struct Cyc_List_List*_tmp316=fd->args;for(0;_tmp316 != 0;_tmp316=_tmp316->tl){
struct Cyc_Absyn_Vardecl _tmp4B7;struct _tuple0*_tmp4B6;struct Cyc_Absyn_Vardecl*_tmp4B5;struct Cyc_Absyn_Vardecl*_tmp317=(_tmp4B5=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4B5[0]=(struct Cyc_Absyn_Vardecl)((_tmp4B7.sc=Cyc_Absyn_Public,((_tmp4B7.name=(
(_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6->f1=Cyc_Absyn_Loc_n,((_tmp4B6->f2=(*((struct _tuple8*)_tmp316->hd)).f1,_tmp4B6)))))),((_tmp4B7.tq=(*((struct _tuple8*)_tmp316->hd)).f2,((_tmp4B7.type=(*((struct _tuple8*)_tmp316->hd)).f3,((_tmp4B7.initializer=0,((_tmp4B7.rgn=param_rgn,((_tmp4B7.attributes=0,((_tmp4B7.escapes=0,_tmp4B7)))))))))))))))),_tmp4B5)));
# 1321
{struct Cyc_List_List _tmp4BA;struct Cyc_List_List*_tmp4B9;_tmp315=((_tmp4B9=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4B9[0]=(struct Cyc_List_List)((_tmp4BA.hd=_tmp317,((_tmp4BA.tl=_tmp315,_tmp4BA)))),_tmp4B9))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4BD;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4BC;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31A=(_tmp4BC=_cycalloc(sizeof(*_tmp4BC)),((_tmp4BC[0]=((_tmp4BD.tag=3,((_tmp4BD.f1=_tmp317,_tmp4BD)))),_tmp4BC)));
struct _dyneither_ptr*_tmp31B=(*((struct _tuple8*)_tmp316->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4BE;_tmp30D=(const struct Cyc_Tcenv_Bindings*)((_tmp4BE=_region_malloc(_tmp30C,sizeof(*_tmp4BE)),((_tmp4BE->v=_tmp31B,((_tmp4BE->b=(void*)_tmp31A,((_tmp4BE->tl=_tmp30D,_tmp4BE))))))));};}}
# 1326
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp38=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp323;struct Cyc_Absyn_Tqual _tmp324;void*_tmp325;int _tmp326;struct Cyc_Absyn_VarargInfo _tmp322=_stmttmp38;_tmp323=_tmp322.name;_tmp324=_tmp322.tq;_tmp325=_tmp322.type;_tmp326=_tmp322.inject;
if(_tmp323 != 0){
void*_tmp327=Cyc_Absyn_dyneither_typ(_tmp325,param_rgn,_tmp324,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4C4;struct _tuple0*_tmp4C3;struct Cyc_Absyn_Vardecl*_tmp4C2;struct Cyc_Absyn_Vardecl*_tmp328=(_tmp4C2=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4C2[0]=(struct Cyc_Absyn_Vardecl)((_tmp4C4.sc=Cyc_Absyn_Public,((_tmp4C4.name=(
(_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->f1=Cyc_Absyn_Loc_n,((_tmp4C3->f2=_tmp323,_tmp4C3)))))),((_tmp4C4.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4C4.type=_tmp327,((_tmp4C4.initializer=0,((_tmp4C4.rgn=param_rgn,((_tmp4C4.attributes=0,((_tmp4C4.escapes=0,_tmp4C4)))))))))))))))),_tmp4C2)));
# 1337
{struct Cyc_List_List*_tmp4C5;_tmp315=((_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5->hd=_tmp328,((_tmp4C5->tl=_tmp315,_tmp4C5))))));}{
struct _dyneither_ptr*_tmp32A=_tmp323;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4C8;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4C7;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp32B=(_tmp4C7=_cycalloc(sizeof(*_tmp4C7)),((_tmp4C7[0]=((_tmp4C8.tag=3,((_tmp4C8.f1=_tmp328,_tmp4C8)))),_tmp4C7)));
struct Cyc_Tcenv_Bindings*_tmp4C9;_tmp30D=(const struct Cyc_Tcenv_Bindings*)((_tmp4C9=_region_malloc(_tmp30C,sizeof(*_tmp4C9)),((_tmp4C9->v=_tmp32A,((_tmp4C9->b=(void*)_tmp32B,((_tmp4C9->tl=_tmp30D,_tmp4C9))))))));};}else{
# 1342
const char*_tmp4CC;void*_tmp4CB;(_tmp4CB=0,Cyc_Tcutil_terr(loc,((_tmp4CC="missing name for varargs",_tag_dyneither(_tmp4CC,sizeof(char),25))),_tag_dyneither(_tmp4CB,sizeof(void*),0)));}}
# 1344
{struct Cyc_Core_Opt _tmp4CF;struct Cyc_Core_Opt*_tmp4CE;fd->param_vardecls=((_tmp4CE=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4CE[0]=(struct Cyc_Core_Opt)((_tmp4CF.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp315),_tmp4CF)),_tmp4CE))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4ED;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4EB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4EA;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4E8;struct Cyc_Tcenv_CtrlEnv*_tmp4E7;struct Cyc_Tcenv_SharedFenv*_tmp4E6;struct Cyc_Tcenv_Fenv*_tmp4E5;return(_tmp4E5=_region_malloc(_tmp30C,sizeof(*_tmp4E5)),((_tmp4E5->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4E6=_region_malloc(_tmp30C,sizeof(*_tmp4E6)),((_tmp4E6->frgn=_tmp30C,((_tmp4E6->return_typ=fd->ret_type,((_tmp4E6->seen_labels=
# 1349
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp30C,Cyc_strptrcmp),((_tmp4E6->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp30C,Cyc_strptrcmp),((_tmp4E6->delayed_effect_checks=0,((_tmp4E6->delayed_constraint_checks=0,_tmp4E6)))))))))))))),((_tmp4E5->type_vars=(struct Cyc_List_List*)_tmp314,((_tmp4E5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp308,((_tmp4E5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp30D,((_tmp4E5->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4E5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1358
(_tmp4E7=_region_malloc(_tmp30C,sizeof(*_tmp4E7)),((_tmp4E7->ctrl_rgn=_tmp30C,((_tmp4E7->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4E7->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4E7->fallthru_clause=0,((_tmp4E7->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4E7->try_depth=0,_tmp4E7)))))))))))))),((_tmp4E5->capability=(void*)((void*)(
(_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED[0]=((_tmp4E8.tag=24,((_tmp4E8.f1=((_tmp4EC=_cycalloc(sizeof(*_tmp4EC)),((_tmp4EC->hd=(void*)((_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EB.tag=23,((_tmp4EB.f1=param_rgn,_tmp4EB)))),_tmp4EA)))),((_tmp4EC->tl=(
(_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->hd=(void*)_check_null(fd->effect),((_tmp4E9->tl=0,_tmp4E9)))))),_tmp4EC)))))),_tmp4E8)))),_tmp4ED))))),((_tmp4E5->curr_rgn=(void*)param_rgn,((_tmp4E5->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1362
((_tmp4E5->flags).in_notreadctxt=0,(((_tmp4E5->flags).in_lhs=0,(((_tmp4E5->flags).in_new=0,(((_tmp4E5->flags).abstract_ok=0,_tmp4E5->flags)))))))),((_tmp4E5->fnrgn=(struct _RegionHandle*)_tmp30B,_tmp4E5)))))))))))))))))))));};};};};}
