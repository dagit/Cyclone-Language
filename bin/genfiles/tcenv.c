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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 906
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 949
extern void*Cyc_Absyn_empty_effect;
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 984
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
# 82
extern struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val;
extern struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val;
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 98
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 100
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 114 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 121
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 123
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 141
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 148
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 159
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 170
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 174
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 178
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 181
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 192
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 194
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 198
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 200
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 204
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 207
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 210
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 48
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 137
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 140
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 161
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 165
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 221 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 293
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 295
int Cyc_Tcutil_new_tvar_id();
# 297
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);char Cyc_Tcenv_Env_error[10]="Env_error";struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};
# 88 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
{const char*_tmp323;void*_tmp322[1];struct Cyc_String_pa_PrintArg_struct _tmp321;(_tmp321.tag=0,((_tmp321.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp322[0]=& _tmp321,Cyc_fprintf(Cyc_stderr,((_tmp323="Internal error in tcenv: %s\n",_tag_dyneither(_tmp323,sizeof(char),29))),_tag_dyneither(_tmp322,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
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
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;int in_lhs;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 220 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 223
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 227
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 232
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp324;return(_tmp324=_region_malloc(r,sizeof(*_tmp324)),((_tmp324->grgn=r,((_tmp324->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp324->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp324->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp324->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp324->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp324->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp324->availables=0,_tmp324)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 243
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 245
{struct Cyc_Core_Opt*_tmp325;Cyc_Tcutil_empty_var_set=((_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp325))));}{
# 247
struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp326;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326[0]=Cyc_Absyn_exn_tud,_tmp326)))));}
{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp32C;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp32B;struct _tuple12*_tmp32A;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,(
(_tmp32A=_region_malloc(r,sizeof(*_tmp32A)),((_tmp32A->f1=(void*)(
(_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32B.tag=2,((_tmp32B.f1=Cyc_Absyn_exn_tud,((_tmp32B.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp32B)))))),_tmp32C)))),((_tmp32A->f2=1,_tmp32A)))))));}}{
# 254
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmpA);
struct Cyc_Tcenv_Tenv*_tmp32D;return(_tmp32D=_region_malloc(r,sizeof(*_tmp32D)),((_tmp32D->ns=0,((_tmp32D->ae=ae,((_tmp32D->le=0,((_tmp32D->allow_valueof=0,_tmp32D)))))))));};};}
# 260
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 262
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 266
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp32E;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp32E="outer_namespace",_tag_dyneither(_tmp32E,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 272
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 279
if(n2 != 0)return 0;
return 1;}
# 285
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 290
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))_tmp14=1;if(!_tmp14){
lookup(ge2,v,0);{
const char*_tmp332;void*_tmp331[1];struct Cyc_String_pa_PrintArg_struct _tmp330;(_tmp330.tag=0,((_tmp330.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp331[0]=& _tmp330,Cyc_Tcutil_terr(loc,((_tmp332="%s is ambiguous",_tag_dyneither(_tmp332,sizeof(char),16))),_tag_dyneither(_tmp331,sizeof(void*),1)))))));};
# 294
;_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;void*_tmp1B;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp1A=(struct Cyc_Dict_Absent_exn_struct*)_tmp19;if(_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 296
 goto _LL0;_LL3: _tmp1B=_tmp19;_LL4:(void)_throw(_tmp1B);_LL0:;}};}}
# 298
return;}
# 301
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 306
{struct _handler_cons _tmp1C;_push_handler(& _tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1F=result;_npop_handler(0);return _tmp1F;};}
# 307
;_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;void*_tmp23;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp22=(struct Cyc_Dict_Absent_exn_struct*)_tmp21;if(_tmp22->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 310
 goto _LL5;_LL8: _tmp23=_tmp21;_LL9:(void)_throw(_tmp23);_LL5:;}};}
# 312
{struct Cyc_List_List*_tmp24=ge->availables;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp24->hd);
struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp24->hd,_tmp24->tl);{
void*_tmp28=result;_npop_handler(0);return _tmp28;};}
# 315
;_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp2A=_tmp26;void*_tmp2C;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp2B=(struct Cyc_Dict_Absent_exn_struct*)_tmp2A;if(_tmp2B->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 318
 goto _LLA;_LLD: _tmp2C=_tmp2A;_LLE:(void)_throw(_tmp2C);_LLA:;}};}}
# 320
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp335;struct Cyc_Dict_Absent_exn_struct*_tmp334;(int)_throw((void*)((_tmp334=_cycalloc_atomic(sizeof(*_tmp334)),((_tmp334[0]=((_tmp335.tag=Cyc_Dict_Absent,_tmp335)),_tmp334)))));}
# 323
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 327
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2F=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2F,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 334
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp31;struct _dyneither_ptr*_tmp32;struct _tuple0*_tmp30=q;_tmp31=_tmp30->f1;_tmp32=_tmp30->f2;{
union Cyc_Absyn_Nmspace _tmp33=_tmp31;struct _dyneither_ptr*_tmp34;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp37;_LL10: if((_tmp33.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp33.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL14;_LL13:
# 339
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp32,is_use);_LL14: if((_tmp33.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp33.Rel_n).val == 0)goto _LL16;_tmp34=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp33.Rel_n).val)->hd;_tmp35=((struct Cyc_List_List*)(_tmp33.Rel_n).val)->tl;_LL15: {
# 341
struct Cyc_Tcenv_Genv*_tmp38=Cyc_Tcenv_lookup_namespace(te,loc,_tmp34,_tmp35);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp38,_tmp32,is_use);}_LL16: if((_tmp33.C_n).tag != 3)goto _LL18;_tmp36=(struct Cyc_List_List*)(_tmp33.C_n).val;_LL17:
 _tmp37=_tmp36;goto _LL19;_LL18: if((_tmp33.Abs_n).tag != 2)goto _LLF;_tmp37=(struct Cyc_List_List*)(_tmp33.Abs_n).val;_LL19:
# 345
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37),_tmp32,is_use);_LLF:;};}
# 350
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 352
struct Cyc_List_List*_tmp39=te->ns;
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_List_List*_tmp3B=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp3C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp39);
struct Cyc_List_List*_tmp3D=_tmp3C->availables;
struct Cyc_Set_Set*_tmp3E=_tmp3C->namespaces;
# 360
{struct Cyc_List_List*_tmp3F=_tmp3D;for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){
struct Cyc_Set_Set*_tmp40=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3F->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){
struct Cyc_List_List*_tmp338;struct Cyc_List_List*_tmp337;_tmp3B=((_tmp337=_region_malloc(temp,sizeof(*_tmp337)),((_tmp337->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338->hd=n,((_tmp338->tl=ns,_tmp338))))))),((_tmp337->tl=_tmp3B,_tmp337))))));}}}
# 367
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3E,n)){
struct Cyc_List_List*_tmp33B;struct Cyc_List_List*_tmp33A;_tmp3B=((_tmp33A=_region_malloc(temp,sizeof(*_tmp33A)),((_tmp33A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B->hd=n,((_tmp33B->tl=ns,_tmp33B))))))),((_tmp33A->tl=_tmp3B,_tmp33A))))));}
# 373
if(_tmp3B != 0){
if(_tmp3B->tl != 0){
const char*_tmp33F;void*_tmp33E[1];struct Cyc_String_pa_PrintArg_struct _tmp33D;(_tmp33D.tag=0,((_tmp33D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp33E[0]=& _tmp33D,Cyc_Tcutil_terr(loc,((_tmp33F="%s is ambiguous",_tag_dyneither(_tmp33F,sizeof(char),16))),_tag_dyneither(_tmp33E,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp48=(struct Cyc_List_List*)_tmp3B->hd;_npop_handler(0);return _tmp48;};}
# 378
if(_tmp39 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp342;struct Cyc_Dict_Absent_exn_struct*_tmp341;(int)_throw((void*)((_tmp341=_cycalloc_atomic(sizeof(*_tmp341)),((_tmp341[0]=((_tmp342.tag=Cyc_Dict_Absent,_tmp342)),_tmp341)))));}
# 381
_tmp39=Cyc_Tcenv_outer_namespace(_tmp39);}}
# 354
;_pop_region(temp);}
# 389
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,v);}
# 393
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4D;struct _dyneither_ptr*_tmp4E;struct _tuple0*_tmp4C=q;_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;{
union Cyc_Absyn_Nmspace _tmp4F=_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;struct _dyneither_ptr*_tmp52;struct Cyc_List_List*_tmp53;_LL1B: if((_tmp4F.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4F.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val != 0)goto _LL1F;_LL1E:
# 398
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4E,0);_LL1F: if((_tmp4F.C_n).tag != 3)goto _LL21;_tmp50=(struct Cyc_List_List*)(_tmp4F.C_n).val;_LL20:
 _tmp51=_tmp50;goto _LL22;_LL21: if((_tmp4F.Abs_n).tag != 2)goto _LL23;_tmp51=(struct Cyc_List_List*)(_tmp4F.Abs_n).val;_LL22: {
# 401
struct Cyc_Dict_Dict _tmp54=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp51))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4E);}_LL23: if((_tmp4F.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val == 0)goto _LL1A;_tmp52=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->hd;_tmp53=((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->tl;_LL24: {
# 404
struct Cyc_Dict_Dict _tmp55=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp52,_tmp53))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp55,_tmp4E);}_LL1A:;};}
# 409
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp56=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,v);}
# 413
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp59;struct _tuple0*_tmp57=q;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;{
union Cyc_Absyn_Nmspace _tmp5A=_tmp58;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;_LL26: if((_tmp5A.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp5A.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val != 0)goto _LL2A;_LL29:
# 418
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp59,0);_LL2A: if((_tmp5A.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val == 0)goto _LL2C;_tmp5B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->hd;_tmp5C=((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->tl;_LL2B: {
# 420
struct Cyc_Dict_Dict _tmp5F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5F,_tmp59);}_LL2C: if((_tmp5A.C_n).tag != 3)goto _LL2E;_tmp5D=(struct Cyc_List_List*)(_tmp5A.C_n).val;_LL2D:
 _tmp5E=_tmp5D;goto _LL2F;_LL2E: if((_tmp5A.Abs_n).tag != 2)goto _LL25;_tmp5E=(struct Cyc_List_List*)(_tmp5A.Abs_n).val;_LL2F: {
# 424
struct Cyc_Dict_Dict _tmp60=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5E))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp60,_tmp59);}_LL25:;};}
# 429
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 434
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp62;struct _dyneither_ptr*_tmp63;struct _tuple0*_tmp61=q;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
union Cyc_Absyn_Nmspace _tmp64=_tmp62;struct _dyneither_ptr*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp68;_LL31: if((_tmp64.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp64.Rel_n).val != 0)goto _LL33;_LL32: {
# 439
struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){{struct Cyc_Absyn_Datatypedecl***_tmp343;struct Cyc_Absyn_Datatypedecl***_tmp6D=(_tmp343=_region_malloc(r,sizeof(*_tmp343)),((_tmp343[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp63,0),_tmp343)));_npop_handler(0);return _tmp6D;};_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6F=_tmp6A;void*_tmp71;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp70=(struct Cyc_Dict_Absent_exn_struct*)_tmp6F;if(_tmp70->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp71=_tmp6F;_LL3F:(void)_throw(_tmp71);_LL3B:;}};}_LL33: if((_tmp64.Loc_n).tag != 4)goto _LL35;_LL34:
# 442
{const char*_tmp346;void*_tmp345;(_tmp345=0,Cyc_Tcutil_terr(loc,((_tmp346="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp346,sizeof(char),33))),_tag_dyneither(_tmp345,sizeof(void*),0)));}
return 0;_LL35: if((_tmp64.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp64.Rel_n).val == 0)goto _LL37;_tmp65=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp64.Rel_n).val)->hd;_tmp66=((struct Cyc_List_List*)(_tmp64.Rel_n).val)->tl;_LL36: {
# 449
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))_tmp76=1;if(!_tmp76){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp65,_tmp66);;_pop_handler();}else{void*_tmp75=(void*)_exn_thrown;void*_tmp78=_tmp75;void*_tmp7A;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 453
{const char*_tmp349;void*_tmp348;(_tmp348=0,Cyc_Tcutil_terr(loc,((_tmp349="bad qualified name for @extensible datatype",_tag_dyneither(_tmp349,sizeof(char),44))),_tag_dyneither(_tmp348,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp34C;struct Cyc_Dict_Absent_exn_struct*_tmp34B;(int)_throw((void*)((_tmp34B=_cycalloc_atomic(sizeof(*_tmp34B)),((_tmp34B[0]=((_tmp34C.tag=Cyc_Dict_Absent,_tmp34C)),_tmp34B)))));};_LL43: _tmp7A=_tmp78;_LL44:(void)_throw(_tmp7A);_LL40:;}};}{
# 456
struct Cyc_Dict_Dict _tmp7F=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp34D;return(_tmp34D=_region_malloc(r,sizeof(*_tmp34D)),((_tmp34D[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp63),_tmp34D)));};}_LL37: if((_tmp64.C_n).tag != 3)goto _LL39;_tmp67=(struct Cyc_List_List*)(_tmp64.C_n).val;_LL38:
 _tmp68=_tmp67;goto _LL3A;_LL39: if((_tmp64.Abs_n).tag != 2)goto _LL30;_tmp68=(struct Cyc_List_List*)(_tmp64.Abs_n).val;_LL3A: {
# 460
struct Cyc_Dict_Dict _tmp81=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp68))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp34E;return(_tmp34E=_region_malloc(r,sizeof(*_tmp34E)),((_tmp34E[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,_tmp63),_tmp34E)));}_LL30:;};}
# 465
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp83=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp83,v);}
# 469
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp84=q;_tmp85=_tmp84->f1;_tmp86=_tmp84->f2;{
union Cyc_Absyn_Nmspace _tmp87=_tmp85;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;_LL46: if((_tmp87.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp87.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp87.Rel_n).val != 0)goto _LL4A;_LL49:
# 474
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp86,0);_LL4A: if((_tmp87.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp87.Rel_n).val == 0)goto _LL4C;_tmp88=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp87.Rel_n).val)->hd;_tmp89=((struct Cyc_List_List*)(_tmp87.Rel_n).val)->tl;_LL4B: {
# 476
struct Cyc_Dict_Dict _tmp8C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8C,_tmp86);}_LL4C: if((_tmp87.C_n).tag != 3)goto _LL4E;_tmp8A=(struct Cyc_List_List*)(_tmp87.C_n).val;_LL4D:
 _tmp8B=_tmp8A;goto _LL4F;_LL4E: if((_tmp87.Abs_n).tag != 2)goto _LL45;_tmp8B=(struct Cyc_List_List*)(_tmp87.Abs_n).val;_LL4F: {
# 480
struct Cyc_Dict_Dict _tmp8D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp8B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,_tmp86);}_LL45:;};}
# 485
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8E,v);}
# 489
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*_tmp8F=q;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{
union Cyc_Absyn_Nmspace _tmp92=_tmp90;struct _dyneither_ptr*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;_LL51: if((_tmp92.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp92.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp92.Rel_n).val != 0)goto _LL55;_LL54:
# 494
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp91,0);_LL55: if((_tmp92.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp92.Rel_n).val == 0)goto _LL57;_tmp93=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp92.Rel_n).val)->hd;_tmp94=((struct Cyc_List_List*)(_tmp92.Rel_n).val)->tl;_LL56: {
# 496
struct Cyc_Dict_Dict _tmp97=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp93,_tmp94))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp97,_tmp91);}_LL57: if((_tmp92.C_n).tag != 3)goto _LL59;_tmp95=(struct Cyc_List_List*)(_tmp92.C_n).val;_LL58:
 _tmp96=_tmp95;goto _LL5A;_LL59: if((_tmp92.Abs_n).tag != 2)goto _LL50;_tmp96=(struct Cyc_List_List*)(_tmp92.Abs_n).val;_LL5A: {
# 500
struct Cyc_Dict_Dict _tmp98=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp96))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp98,_tmp91);}_LL50:;};}
# 507
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp99=te->le;
if(te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp99);}
# 513
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 517
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp9A=te->le;
if(_tmp9A != 0){
struct _RegionHandle*_tmp9C;struct Cyc_Tcenv_Fenv*_tmp9B=_tmp9A;_tmp9C=_tmp9B->fnrgn;
return _tmp9C;}
# 523
return Cyc_Tcenv_coerce_heap_region();}
# 526
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp34F;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp34F="put_fenv",_tag_dyneither(_tmp34F,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp350;return(_tmp350=_region_malloc(l,sizeof(*_tmp350)),((_tmp350->ns=te->ns,((_tmp350->ae=te->ae,((_tmp350->le=fe,((_tmp350->allow_valueof=te->allow_valueof,_tmp350)))))))));};}
# 531
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp351;return(_tmp351=_region_malloc(l,sizeof(*_tmp351)),((_tmp351->ns=te->ns,((_tmp351->ae=te->ae,((_tmp351->le=0,((_tmp351->allow_valueof=te->allow_valueof,_tmp351)))))))));}
# 535
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 537
struct Cyc_Tcenv_SharedFenv*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_RgnOrder_RgnPO*_tmpA3;const struct Cyc_Tcenv_Bindings*_tmpA4;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Tcenv_CtrlEnv*_tmpA6;void*_tmpA7;void*_tmpA8;int _tmpA9;int _tmpAA;struct _RegionHandle*_tmpAB;struct Cyc_Tcenv_Fenv _tmpA0=*f;_tmpA1=_tmpA0.shared;_tmpA2=_tmpA0.type_vars;_tmpA3=_tmpA0.region_order;_tmpA4=_tmpA0.locals;_tmpA5=_tmpA0.encloser;_tmpA6=_tmpA0.ctrl_env;_tmpA7=_tmpA0.capability;_tmpA8=_tmpA0.curr_rgn;_tmpA9=_tmpA0.in_notreadctxt;_tmpAA=_tmpA0.in_lhs;_tmpAB=_tmpA0.fnrgn;{
# 539
struct Cyc_Tcenv_Fenv*_tmp352;return(_tmp352=_region_malloc(l,sizeof(*_tmp352)),((_tmp352->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA1,((_tmp352->type_vars=(struct Cyc_List_List*)_tmpA2,((_tmp352->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA3,((_tmp352->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA4,((_tmp352->encloser=(struct Cyc_Absyn_Stmt*)_tmpA5,((_tmp352->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA6,((_tmp352->capability=(void*)_tmpA7,((_tmp352->curr_rgn=(void*)_tmpA8,((_tmp352->in_notreadctxt=(int)_tmpA9,((_tmp352->in_lhs=(int)_tmpAA,((_tmp352->fnrgn=(struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp352)))))))))))))))))))))));};}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_SharedFenv*_tmpAE;struct Cyc_List_List*_tmpAF;struct Cyc_RgnOrder_RgnPO*_tmpB0;const struct Cyc_Tcenv_Bindings*_tmpB1;struct Cyc_Absyn_Stmt*_tmpB2;struct Cyc_Tcenv_CtrlEnv*_tmpB3;void*_tmpB4;void*_tmpB5;int _tmpB6;int _tmpB7;struct Cyc_Tcenv_Fenv _tmpAD=*f;_tmpAE=_tmpAD.shared;_tmpAF=_tmpAD.type_vars;_tmpB0=_tmpAD.region_order;_tmpB1=_tmpAD.locals;_tmpB2=_tmpAD.encloser;_tmpB3=_tmpAD.ctrl_env;_tmpB4=_tmpAD.capability;_tmpB5=_tmpAD.curr_rgn;_tmpB6=_tmpAD.in_notreadctxt;_tmpB7=_tmpAD.in_lhs;{
# 549
struct _RegionHandle*_tmpB9;void*_tmpBA;void*_tmpBB;const struct _tuple10*_tmpBC;void*_tmpBD;int _tmpBE;struct Cyc_Tcenv_CtrlEnv _tmpB8=*_tmpB3;_tmpB9=_tmpB8.ctrl_rgn;_tmpBA=_tmpB8.continue_stmt;_tmpBB=_tmpB8.break_stmt;_tmpBC=_tmpB8.fallthru_clause;_tmpBD=_tmpB8.next_stmt;_tmpBE=_tmpB8.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp353;struct Cyc_Tcenv_CtrlEnv*_tmpBF=
(_tmp353=_region_malloc(l,sizeof(*_tmp353)),((_tmp353->ctrl_rgn=_tmpB9,((_tmp353->continue_stmt=_tmpBA,((_tmp353->break_stmt=_tmpBB,((_tmp353->fallthru_clause=_tmpBC,((_tmp353->next_stmt=_tmpBD,((_tmp353->try_depth=_tmpBE,_tmp353)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp354;return(_tmp354=_region_malloc(l,sizeof(*_tmp354)),((_tmp354->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAE,((_tmp354->type_vars=(struct Cyc_List_List*)_tmpAF,((_tmp354->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB0,((_tmp354->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB1,((_tmp354->encloser=(struct Cyc_Absyn_Stmt*)_tmpB2,((_tmp354->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBF,((_tmp354->capability=(void*)_tmpB4,((_tmp354->curr_rgn=(void*)_tmpB5,((_tmp354->in_notreadctxt=(int)_tmpB6,((_tmp354->in_lhs=(int)_tmpB7,((_tmp354->fnrgn=(struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp354)))))))))))))))))))))));};};}
# 559
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmpC4;const char*_tmp355;struct Cyc_Tcenv_Fenv*_tmpC3=Cyc_Tcenv_get_fenv(te,((_tmp355="return_typ",_tag_dyneither(_tmp355,sizeof(char),11))));_tmpC4=_tmpC3->shared;{
void*_tmpC6;struct Cyc_Tcenv_SharedFenv _tmpC5=*_tmpC4;_tmpC6=_tmpC5.return_typ;
return _tmpC6;};}
# 565
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC7=te->le;
if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpC9;struct Cyc_Tcenv_Fenv _tmpC8=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC7));_tmpC9=_tmpC8.type_vars;
return _tmpC9;};}
# 572
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp356;return(_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356->v=Cyc_Tcenv_lookup_type_vars(te),_tmp356)));}
# 576
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 578
struct Cyc_Tcenv_SharedFenv*_tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_RgnOrder_RgnPO*_tmpCF;const struct Cyc_Tcenv_Bindings*_tmpD0;struct Cyc_Absyn_Stmt*_tmpD1;struct Cyc_Tcenv_CtrlEnv*_tmpD2;void*_tmpD3;void*_tmpD4;int _tmpD5;int _tmpD6;const char*_tmp357;struct Cyc_Tcenv_Fenv*_tmpCC=
# 580
Cyc_Tcenv_get_fenv(te,((_tmp357="add_type_vars",_tag_dyneither(_tmp357,sizeof(char),14))));_tmpCD=_tmpCC->shared;_tmpCE=_tmpCC->type_vars;_tmpCF=_tmpCC->region_order;_tmpD0=_tmpCC->locals;_tmpD1=_tmpCC->encloser;_tmpD2=_tmpCC->ctrl_env;_tmpD3=_tmpCC->capability;_tmpD4=_tmpCC->curr_rgn;_tmpD5=_tmpCC->in_notreadctxt;_tmpD6=_tmpCC->in_lhs;
# 582
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpCE);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD7);{
struct Cyc_Tcenv_Fenv*_tmp358;struct Cyc_Tcenv_Fenv*_tmpD8=(_tmp358=_region_malloc(r,sizeof(*_tmp358)),((_tmp358->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCD,((_tmp358->type_vars=(struct Cyc_List_List*)_tmpD7,((_tmp358->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpCF,((_tmp358->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD0,((_tmp358->encloser=(struct Cyc_Absyn_Stmt*)_tmpD1,((_tmp358->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD2,((_tmp358->capability=(void*)_tmpD3,((_tmp358->curr_rgn=(void*)_tmpD4,((_tmp358->in_notreadctxt=(int)_tmpD5,((_tmp358->in_lhs=(int)_tmpD6,((_tmp358->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp358)))))))))))))))))))))));
# 588
return Cyc_Tcenv_put_fenv(r,te,_tmpD8);};};}
# 591
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDA=te->le;
if(_tmpDA == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 596
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpDA);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 601
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 603
struct _dyneither_ptr*_tmpDB=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpDC=(*vd->name).f1;_LL5C: if((_tmpDC.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 608
struct Cyc_Core_Impossible_exn_struct _tmp35E;const char*_tmp35D;struct Cyc_Core_Impossible_exn_struct*_tmp35C;(int)_throw((void*)((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C[0]=((_tmp35E.tag=Cyc_Core_Impossible,((_tmp35E.f1=((_tmp35D="add_local_var: called with Rel_n",_tag_dyneither(_tmp35D,sizeof(char),33))),_tmp35E)))),_tmp35C)))));}_LL5B:;}{
# 610
struct Cyc_Tcenv_SharedFenv*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_RgnOrder_RgnPO*_tmpE4;const struct Cyc_Tcenv_Bindings*_tmpE5;struct Cyc_Absyn_Stmt*_tmpE6;struct Cyc_Tcenv_CtrlEnv*_tmpE7;void*_tmpE8;void*_tmpE9;int _tmpEA;int _tmpEB;const char*_tmp35F;struct Cyc_Tcenv_Fenv*_tmpE1=
# 612
Cyc_Tcenv_get_fenv(te,((_tmp35F="add_local_var",_tag_dyneither(_tmp35F,sizeof(char),14))));_tmpE2=_tmpE1->shared;_tmpE3=_tmpE1->type_vars;_tmpE4=_tmpE1->region_order;_tmpE5=_tmpE1->locals;_tmpE6=_tmpE1->encloser;_tmpE7=_tmpE1->ctrl_env;_tmpE8=_tmpE1->capability;_tmpE9=_tmpE1->curr_rgn;_tmpEA=_tmpE1->in_notreadctxt;_tmpEB=_tmpE1->in_lhs;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp362;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp361;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpEC=(_tmp361=_cycalloc(sizeof(*_tmp361)),((_tmp361[0]=((_tmp362.tag=4,((_tmp362.f1=vd,_tmp362)))),_tmp361)));
struct Cyc_Tcenv_Bindings*_tmp363;struct Cyc_Tcenv_Bindings*_tmpED=(_tmp363=_region_malloc(r,sizeof(*_tmp363)),((_tmp363->v=_tmpDB,((_tmp363->b=(void*)_tmpEC,((_tmp363->tl=_tmpE5,_tmp363)))))));
struct Cyc_Tcenv_Fenv*_tmp364;struct Cyc_Tcenv_Fenv*_tmpEE=(_tmp364=_region_malloc(r,sizeof(*_tmp364)),((_tmp364->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE2,((_tmp364->type_vars=(struct Cyc_List_List*)_tmpE3,((_tmp364->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE4,((_tmp364->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpED),((_tmp364->encloser=(struct Cyc_Absyn_Stmt*)_tmpE6,((_tmp364->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE7,((_tmp364->capability=(void*)_tmpE8,((_tmp364->curr_rgn=(void*)_tmpE9,((_tmp364->in_notreadctxt=(int)_tmpEA,((_tmp364->in_lhs=(int)_tmpEB,((_tmp364->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp364)))))))))))))))))))))));
# 618
return Cyc_Tcenv_put_fenv(r,te,_tmpEE);};};}
# 621
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpF3=te->le;
if(_tmpF3 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_RgnOrder_RgnPO*_tmpF7;const struct Cyc_Tcenv_Bindings*_tmpF8;struct Cyc_Absyn_Stmt*_tmpF9;struct Cyc_Tcenv_CtrlEnv*_tmpFA;void*_tmpFB;void*_tmpFC;int _tmpFD;int _tmpFE;struct Cyc_Tcenv_Fenv _tmpF4=*_tmpF3;_tmpF5=_tmpF4.shared;_tmpF6=_tmpF4.type_vars;_tmpF7=_tmpF4.region_order;_tmpF8=_tmpF4.locals;_tmpF9=_tmpF4.encloser;_tmpFA=_tmpF4.ctrl_env;_tmpFB=_tmpF4.capability;_tmpFC=_tmpF4.curr_rgn;_tmpFD=_tmpF4.in_notreadctxt;_tmpFE=_tmpF4.in_lhs;{
# 626
struct Cyc_Tcenv_Fenv*_tmp365;struct Cyc_Tcenv_Fenv*_tmpFF=(_tmp365=_region_malloc(r,sizeof(*_tmp365)),((_tmp365->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF5,((_tmp365->type_vars=(struct Cyc_List_List*)_tmpF6,((_tmp365->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF7,((_tmp365->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF8,((_tmp365->encloser=(struct Cyc_Absyn_Stmt*)_tmpF9,((_tmp365->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpFA,((_tmp365->capability=(void*)_tmpFB,((_tmp365->curr_rgn=(void*)_tmpFC,((_tmp365->in_notreadctxt=(int)1,((_tmp365->in_lhs=(int)_tmpFE,((_tmp365->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp365)))))))))))))))))))))));
# 628
return Cyc_Tcenv_put_fenv(r,te,_tmpFF);};};}
# 631
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp101=te->le;
if(_tmp101 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_RgnOrder_RgnPO*_tmp105;const struct Cyc_Tcenv_Bindings*_tmp106;struct Cyc_Absyn_Stmt*_tmp107;struct Cyc_Tcenv_CtrlEnv*_tmp108;void*_tmp109;void*_tmp10A;int _tmp10B;int _tmp10C;struct Cyc_Tcenv_Fenv _tmp102=*_tmp101;_tmp103=_tmp102.shared;_tmp104=_tmp102.type_vars;_tmp105=_tmp102.region_order;_tmp106=_tmp102.locals;_tmp107=_tmp102.encloser;_tmp108=_tmp102.ctrl_env;_tmp109=_tmp102.capability;_tmp10A=_tmp102.curr_rgn;_tmp10B=_tmp102.in_notreadctxt;_tmp10C=_tmp102.in_lhs;{
# 636
struct Cyc_Tcenv_Fenv*_tmp366;struct Cyc_Tcenv_Fenv*_tmp10D=(_tmp366=_region_malloc(r,sizeof(*_tmp366)),((_tmp366->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp103,((_tmp366->type_vars=(struct Cyc_List_List*)_tmp104,((_tmp366->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp105,((_tmp366->locals=(const struct Cyc_Tcenv_Bindings*)_tmp106,((_tmp366->encloser=(struct Cyc_Absyn_Stmt*)_tmp107,((_tmp366->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp108,((_tmp366->capability=(void*)_tmp109,((_tmp366->curr_rgn=(void*)_tmp10A,((_tmp366->in_notreadctxt=(int)0,((_tmp366->in_lhs=(int)_tmp10C,((_tmp366->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp366)))))))))))))))))))))));
# 638
return Cyc_Tcenv_put_fenv(r,te,_tmp10D);};};}
# 641
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return 0;{
int _tmp111;struct Cyc_Tcenv_Fenv*_tmp110=_tmp10F;_tmp111=_tmp110->in_notreadctxt;
return _tmp111;};}
# 648
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp112=te->le;
if(_tmp112 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp114;struct Cyc_List_List*_tmp115;struct Cyc_RgnOrder_RgnPO*_tmp116;const struct Cyc_Tcenv_Bindings*_tmp117;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Tcenv_CtrlEnv*_tmp119;void*_tmp11A;void*_tmp11B;int _tmp11C;int _tmp11D;struct Cyc_Tcenv_Fenv _tmp113=*_tmp112;_tmp114=_tmp113.shared;_tmp115=_tmp113.type_vars;_tmp116=_tmp113.region_order;_tmp117=_tmp113.locals;_tmp118=_tmp113.encloser;_tmp119=_tmp113.ctrl_env;_tmp11A=_tmp113.capability;_tmp11B=_tmp113.curr_rgn;_tmp11C=_tmp113.in_notreadctxt;_tmp11D=_tmp113.in_lhs;{
# 653
struct Cyc_Tcenv_Fenv*_tmp367;struct Cyc_Tcenv_Fenv*_tmp11E=(_tmp367=_region_malloc(r,sizeof(*_tmp367)),((_tmp367->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp114,((_tmp367->type_vars=(struct Cyc_List_List*)_tmp115,((_tmp367->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp116,((_tmp367->locals=(const struct Cyc_Tcenv_Bindings*)_tmp117,((_tmp367->encloser=(struct Cyc_Absyn_Stmt*)_tmp118,((_tmp367->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp119,((_tmp367->capability=(void*)_tmp11A,((_tmp367->curr_rgn=(void*)_tmp11B,((_tmp367->in_notreadctxt=(int)_tmp11C,((_tmp367->in_lhs=(int)1,((_tmp367->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp367)))))))))))))))))))))));
# 656
return Cyc_Tcenv_put_fenv(r,te,_tmp11E);};};}
# 659
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp120=te->le;
if(_tmp120 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_RgnOrder_RgnPO*_tmp124;const struct Cyc_Tcenv_Bindings*_tmp125;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Tcenv_CtrlEnv*_tmp127;void*_tmp128;void*_tmp129;int _tmp12A;int _tmp12B;struct Cyc_Tcenv_Fenv _tmp121=*_tmp120;_tmp122=_tmp121.shared;_tmp123=_tmp121.type_vars;_tmp124=_tmp121.region_order;_tmp125=_tmp121.locals;_tmp126=_tmp121.encloser;_tmp127=_tmp121.ctrl_env;_tmp128=_tmp121.capability;_tmp129=_tmp121.curr_rgn;_tmp12A=_tmp121.in_notreadctxt;_tmp12B=_tmp121.in_lhs;{
# 664
struct Cyc_Tcenv_Fenv*_tmp368;struct Cyc_Tcenv_Fenv*_tmp12C=(_tmp368=_region_malloc(r,sizeof(*_tmp368)),((_tmp368->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp122,((_tmp368->type_vars=(struct Cyc_List_List*)_tmp123,((_tmp368->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp124,((_tmp368->locals=(const struct Cyc_Tcenv_Bindings*)_tmp125,((_tmp368->encloser=(struct Cyc_Absyn_Stmt*)_tmp126,((_tmp368->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp127,((_tmp368->capability=(void*)_tmp128,((_tmp368->curr_rgn=(void*)_tmp129,((_tmp368->in_notreadctxt=(int)_tmp12A,((_tmp368->in_lhs=(int)0,((_tmp368->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp368)))))))))))))))))))))));
# 667
return Cyc_Tcenv_put_fenv(r,te,_tmp12C);};};}
# 670
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp12E=te->le;
if(_tmp12E == 0)return 0;{
int _tmp130;struct Cyc_Tcenv_Fenv*_tmp12F=_tmp12E;_tmp130=_tmp12F->in_lhs;
return _tmp130;};}
# 678
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 680
struct _dyneither_ptr*_tmp131=(*vd->name).f2;
struct Cyc_Tcenv_SharedFenv*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_RgnOrder_RgnPO*_tmp136;const struct Cyc_Tcenv_Bindings*_tmp137;struct Cyc_Absyn_Stmt*_tmp138;struct Cyc_Tcenv_CtrlEnv*_tmp139;void*_tmp13A;void*_tmp13B;int _tmp13C;int _tmp13D;const char*_tmp369;struct Cyc_Tcenv_Fenv*_tmp133=
# 683
Cyc_Tcenv_get_fenv(te,((_tmp369="add_pat_var",_tag_dyneither(_tmp369,sizeof(char),12))));_tmp134=_tmp133->shared;_tmp135=_tmp133->type_vars;_tmp136=_tmp133->region_order;_tmp137=_tmp133->locals;_tmp138=_tmp133->encloser;_tmp139=_tmp133->ctrl_env;_tmp13A=_tmp133->capability;_tmp13B=_tmp133->curr_rgn;_tmp13C=_tmp133->in_notreadctxt;_tmp13D=_tmp133->in_lhs;{
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp36C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp36B;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp13E=(_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B[0]=((_tmp36C.tag=5,((_tmp36C.f1=vd,_tmp36C)))),_tmp36B)));
struct Cyc_Tcenv_Bindings*_tmp36D;struct Cyc_Tcenv_Bindings*_tmp13F=(_tmp36D=_region_malloc(r,sizeof(*_tmp36D)),((_tmp36D->v=_tmp131,((_tmp36D->b=(void*)_tmp13E,((_tmp36D->tl=_tmp137,_tmp36D)))))));
struct Cyc_Tcenv_Fenv*_tmp36E;struct Cyc_Tcenv_Fenv*_tmp140=(_tmp36E=_region_malloc(r,sizeof(*_tmp36E)),((_tmp36E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp134,((_tmp36E->type_vars=(struct Cyc_List_List*)_tmp135,((_tmp36E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp136,((_tmp36E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp13F),((_tmp36E->encloser=(struct Cyc_Absyn_Stmt*)_tmp138,((_tmp36E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp139,((_tmp36E->capability=(void*)_tmp13A,((_tmp36E->curr_rgn=(void*)_tmp13B,((_tmp36E->in_notreadctxt=(int)_tmp13C,((_tmp36E->in_lhs=(int)_tmp13D,((_tmp36E->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp36E)))))))))))))))))))))));
# 689
return Cyc_Tcenv_put_fenv(r,te,_tmp140);};}
# 692
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp145=te->le;
union Cyc_Absyn_Nmspace _tmp147;struct _dyneither_ptr*_tmp148;struct _tuple0*_tmp146=q;_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;{
union Cyc_Absyn_Nmspace _tmp149=_tmp147;_LL61: if((_tmp149.Loc_n).tag != 4)goto _LL63;_LL62:
# 697
 if(_tmp145 == 0){struct Cyc_Dict_Absent_exn_struct _tmp371;struct Cyc_Dict_Absent_exn_struct*_tmp370;(int)_throw((void*)((_tmp370=_cycalloc_atomic(sizeof(*_tmp370)),((_tmp370[0]=((_tmp371.tag=Cyc_Dict_Absent,_tmp371)),_tmp370)))));}
goto _LL64;_LL63: if((_tmp149.Rel_n).tag != 1)goto _LL65;if((struct Cyc_List_List*)(_tmp149.Rel_n).val != 0)goto _LL65;if(!(_tmp145 != 0))goto _LL65;_LL64: {
# 700
const struct Cyc_Tcenv_Bindings*_tmp14D;struct Cyc_Tcenv_Fenv*_tmp14C=_tmp145;_tmp14D=_tmp14C->locals;{
# 702
struct _handler_cons _tmp14E;_push_handler(& _tmp14E);{int _tmp150=0;if(setjmp(_tmp14E.handler))_tmp150=1;if(!_tmp150){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp374;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp373;void*_tmp153=(void*)((_tmp373=_region_malloc(r,sizeof(*_tmp373)),((_tmp373[0]=((_tmp374.tag=0,((_tmp374.f1=Cyc_Tcenv_lookup_binding(_tmp14D,_tmp148),_tmp374)))),_tmp373))));_npop_handler(0);return _tmp153;};_pop_handler();}else{void*_tmp14F=(void*)_exn_thrown;void*_tmp155=_tmp14F;void*_tmp157;_LL68: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp156=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp155;if(_tmp156->tag != Cyc_Tcenv_NoBinding)goto _LL6A;}_LL69:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL6A: _tmp157=_tmp155;_LL6B:(void)_throw(_tmp157);_LL67:;}};};}_LL65:;_LL66: {
# 706
struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){{void*_tmp15B=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp15B;};_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15D=_tmp159;void*_tmp15F;_LL6D: {struct Cyc_Dict_Absent_exn_struct*_tmp15E=(struct Cyc_Dict_Absent_exn_struct*)_tmp15D;if(_tmp15E->tag != Cyc_Dict_Absent)goto _LL6F;}_LL6E: {
# 710
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp377;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp376;return(void*)((_tmp376=_region_malloc(r,sizeof(*_tmp376)),((_tmp376[0]=((_tmp377.tag=0,((_tmp377.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp377)))),_tmp376))));}_LL6F: _tmp15F=_tmp15D;_LL70:(void)_throw(_tmp15F);_LL6C:;}};}_LL60:;};}
# 715
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp164;const char*_tmp378;struct Cyc_Tcenv_Fenv*_tmp163=
Cyc_Tcenv_get_fenv(te,((_tmp378="process_continue",_tag_dyneither(_tmp378,sizeof(char),17))));_tmp164=_tmp163->ctrl_env;{
void*_tmp165=_tmp164->continue_stmt;struct Cyc_Absyn_Stmt*_tmp167;_LL72: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp166=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp165;if(_tmp166->tag != 3)goto _LL74;else{_tmp167=_tmp166->f1;}}_LL73:
# 720
{struct Cyc_List_List*_tmp379;_tmp167->non_local_preds=((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379->hd=s,((_tmp379->tl=_tmp167->non_local_preds,_tmp379))))));}
*sopt=_tmp167;
return;_LL74: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp168=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp165;if(_tmp168->tag != 0)goto _LL76;}_LL75:
{const char*_tmp37C;void*_tmp37B;(_tmp37B=0,Cyc_Tcutil_terr(s->loc,((_tmp37C="continue not in a loop",_tag_dyneither(_tmp37C,sizeof(char),23))),_tag_dyneither(_tmp37B,sizeof(void*),0)));}return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp169=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp165;if(_tmp169->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp16A=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp165;if(_tmp16A->tag != 2)goto _LL71;}_LL79: {
const char*_tmp37D;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp37D="bad continue destination",_tag_dyneither(_tmp37D,sizeof(char),25))));}_LL71:;};}
# 728
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp171;struct Cyc_Tcenv_SharedFenv*_tmp172;const char*_tmp37E;struct Cyc_Tcenv_Fenv*_tmp170=
Cyc_Tcenv_get_fenv(te,((_tmp37E="process_break",_tag_dyneither(_tmp37E,sizeof(char),14))));_tmp171=_tmp170->ctrl_env;_tmp172=_tmp170->shared;{
void*_tmp173=_tmp171->break_stmt;struct Cyc_Absyn_Stmt*_tmp175;_LL7B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp174=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp173;if(_tmp174->tag != 3)goto _LL7D;else{_tmp175=_tmp174->f1;}}_LL7C:
# 733
{struct Cyc_List_List*_tmp37F;_tmp175->non_local_preds=((_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F->hd=s,((_tmp37F->tl=_tmp175->non_local_preds,_tmp37F))))));}
*sopt=_tmp175;
return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp176=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp173;if(_tmp176->tag != 0)goto _LL7F;}_LL7E:
# 737
{const char*_tmp382;void*_tmp381;(_tmp381=0,Cyc_Tcutil_terr(s->loc,((_tmp382="break not in a loop or switch",_tag_dyneither(_tmp382,sizeof(char),30))),_tag_dyneither(_tmp381,sizeof(void*),0)));}
return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp177=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp173;if(_tmp177->tag != 2)goto _LL81;}_LL80:
# 743
{void*_tmp17C=Cyc_Tcutil_compress(_tmp172->return_typ);_LL84: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp17C;if(_tmp17D->tag != 0)goto _LL86;}_LL85:
 goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp17C;if(_tmp17E->tag != 7)goto _LL88;}_LL87:
 goto _LL89;_LL88: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp17C;if(_tmp17F->tag != 6)goto _LL8A;}_LL89:
# 747
{const char*_tmp385;void*_tmp384;(_tmp384=0,Cyc_Tcutil_warn(s->loc,((_tmp385="break may cause function not to return a value",_tag_dyneither(_tmp385,sizeof(char),47))),_tag_dyneither(_tmp384,sizeof(void*),0)));}
goto _LL83;_LL8A:;_LL8B:
# 750
{const char*_tmp388;void*_tmp387;(_tmp387=0,Cyc_Tcutil_terr(s->loc,((_tmp388="break may cause function not to return a value",_tag_dyneither(_tmp388,sizeof(char),47))),_tag_dyneither(_tmp387,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 753
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp178=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp173;if(_tmp178->tag != 1)goto _LL7A;}_LL82:
# 756
{const char*_tmp38B;void*_tmp38A;(_tmp38A=0,Cyc_Tcutil_terr(s->loc,((_tmp38B="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp38B,sizeof(char),56))),_tag_dyneither(_tmp38A,sizeof(void*),0)));}
return;_LL7A:;};}
# 760
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_SharedFenv*_tmp188;const char*_tmp38C;struct Cyc_Tcenv_Fenv*_tmp187=Cyc_Tcenv_get_fenv(te,((_tmp38C="process_goto",_tag_dyneither(_tmp38C,sizeof(char),13))));_tmp188=_tmp187->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp188->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp189=_tmp188->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp189,l);
struct _RegionHandle*frgn=_tmp188->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp38D;slopt=((_tmp38D=_region_malloc(frgn,sizeof(*_tmp38D)),((_tmp38D[0]=0,_tmp38D))));}{
struct Cyc_List_List*_tmp38E;struct Cyc_List_List*new_needed=(_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E->hd=s,((_tmp38E->tl=*slopt,_tmp38E)))));
_tmp188->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp189,l,new_needed);};}else{
# 772
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp38F;s->non_local_preds=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->hd=s,((_tmp38F->tl=s->non_local_preds,_tmp38F))))));}
*sopt=s;}};}
# 778
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 780
struct Cyc_Tcenv_CtrlEnv*_tmp18F;const char*_tmp390;struct Cyc_Tcenv_Fenv*_tmp18E=Cyc_Tcenv_get_fenv(te,((_tmp390="process_fallthru",_tag_dyneither(_tmp390,sizeof(char),17))));_tmp18F=_tmp18E->ctrl_env;{
const struct _tuple10*_tmp190=_tmp18F->fallthru_clause;
if(_tmp190 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp391;(((*_tmp190).f1)->body)->non_local_preds=(
(_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391->hd=s,((_tmp391->tl=(((*_tmp190).f1)->body)->non_local_preds,_tmp391))))));}{
struct Cyc_Absyn_Switch_clause**_tmp392;*clauseopt=((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392[0]=(*_tmp190).f1,_tmp392))));};}
# 787
return _tmp190;};}
# 790
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 794
struct Cyc_Tcenv_SharedFenv*_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_RgnOrder_RgnPO*_tmp197;const struct Cyc_Tcenv_Bindings*_tmp198;struct Cyc_Absyn_Stmt*_tmp199;struct Cyc_Tcenv_CtrlEnv*_tmp19A;void*_tmp19B;void*_tmp19C;int _tmp19D;int _tmp19E;const char*_tmp393;struct Cyc_Tcenv_Fenv*_tmp194=
Cyc_Tcenv_get_fenv(te,((_tmp393="set_fallthru",_tag_dyneither(_tmp393,sizeof(char),13))));_tmp195=_tmp194->shared;_tmp196=_tmp194->type_vars;_tmp197=_tmp194->region_order;_tmp198=_tmp194->locals;_tmp199=_tmp194->encloser;_tmp19A=_tmp194->ctrl_env;_tmp19B=_tmp194->capability;_tmp19C=_tmp194->curr_rgn;_tmp19D=_tmp194->in_notreadctxt;_tmp19E=_tmp194->in_lhs;{
struct _RegionHandle*_tmp1A0;void*_tmp1A1;void*_tmp1A2;const struct _tuple10*_tmp1A3;void*_tmp1A4;int _tmp1A5;struct Cyc_Tcenv_CtrlEnv*_tmp19F=_tmp19A;_tmp1A0=_tmp19F->ctrl_rgn;_tmp1A1=_tmp19F->continue_stmt;_tmp1A2=_tmp19F->break_stmt;_tmp1A3=_tmp19F->fallthru_clause;_tmp1A4=_tmp19F->next_stmt;_tmp1A5=_tmp19F->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp394;ft_typ=((_tmp394=_region_malloc(_tmp1A0,sizeof(*_tmp394)),((_tmp394->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp394->tl=ft_typ,_tmp394))))));}{
const struct Cyc_Tcenv_CList*_tmp1A7=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp397;struct Cyc_Tcenv_CtrlEnv*_tmp396;struct Cyc_Tcenv_CtrlEnv*_tmp1A8=
(_tmp396=_region_malloc(r,sizeof(*_tmp396)),((_tmp396->ctrl_rgn=_tmp1A0,((_tmp396->continue_stmt=_tmp1A1,((_tmp396->break_stmt=_tmp1A2,((_tmp396->fallthru_clause=(const struct _tuple10*)((_tmp397=_region_malloc(_tmp1A0,sizeof(*_tmp397)),((_tmp397->f1=clause,((_tmp397->f2=new_tvs,((_tmp397->f3=_tmp1A7,_tmp397)))))))),((_tmp396->next_stmt=_tmp1A4,((_tmp396->try_depth=_tmp1A5,_tmp396)))))))))))));
# 804
struct Cyc_Tcenv_Fenv*_tmp398;struct Cyc_Tcenv_Fenv*_tmp1A9=(_tmp398=_region_malloc(r,sizeof(*_tmp398)),((_tmp398->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp195,((_tmp398->type_vars=(struct Cyc_List_List*)_tmp196,((_tmp398->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp197,((_tmp398->locals=(const struct Cyc_Tcenv_Bindings*)_tmp198,((_tmp398->encloser=(struct Cyc_Absyn_Stmt*)_tmp199,((_tmp398->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1A8,((_tmp398->capability=(void*)_tmp19B,((_tmp398->curr_rgn=(void*)_tmp19C,((_tmp398->in_notreadctxt=(int)_tmp19D,((_tmp398->in_lhs=(int)_tmp19E,((_tmp398->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp398)))))))))))))))))))))));
# 808
return Cyc_Tcenv_put_fenv(r,te,_tmp1A9);};};};}
# 811
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp399;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp399="clear_fallthru",_tag_dyneither(_tmp399,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1AE;struct Cyc_Tcenv_Fenv*_tmp1AD=fe;_tmp1AE=_tmp1AD->ctrl_env;
_tmp1AE->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 818
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 820
struct Cyc_Tcenv_SharedFenv*_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_RgnOrder_RgnPO*_tmp1B4;const struct Cyc_Tcenv_Bindings*_tmp1B5;struct Cyc_Absyn_Stmt*_tmp1B6;struct Cyc_Tcenv_CtrlEnv*_tmp1B7;void*_tmp1B8;void*_tmp1B9;int _tmp1BA;int _tmp1BB;const char*_tmp39A;struct Cyc_Tcenv_Fenv*_tmp1B1=
# 822
Cyc_Tcenv_get_fenv(te,((_tmp39A="set_in_loop",_tag_dyneither(_tmp39A,sizeof(char),12))));_tmp1B2=_tmp1B1->shared;_tmp1B3=_tmp1B1->type_vars;_tmp1B4=_tmp1B1->region_order;_tmp1B5=_tmp1B1->locals;_tmp1B6=_tmp1B1->encloser;_tmp1B7=_tmp1B1->ctrl_env;_tmp1B8=_tmp1B1->capability;_tmp1B9=_tmp1B1->curr_rgn;_tmp1BA=_tmp1B1->in_notreadctxt;_tmp1BB=_tmp1B1->in_lhs;{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3A5;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3A4;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3A3;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3A2;struct Cyc_Tcenv_CtrlEnv*_tmp3A1;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3A1=_region_malloc(r,sizeof(*_tmp3A1)),((_tmp3A1->ctrl_rgn=r,((_tmp3A1->continue_stmt=(void*)(
(_tmp3A3=_region_malloc(r,sizeof(*_tmp3A3)),((_tmp3A3[0]=((_tmp3A2.tag=3,((_tmp3A2.f1=continue_dest,_tmp3A2)))),_tmp3A3)))),((_tmp3A1->break_stmt=_tmp1B7->next_stmt,((_tmp3A1->next_stmt=(void*)(
# 827
(_tmp3A5=_region_malloc(r,sizeof(*_tmp3A5)),((_tmp3A5[0]=((_tmp3A4.tag=3,((_tmp3A4.f1=continue_dest,_tmp3A4)))),_tmp3A5)))),((_tmp3A1->fallthru_clause=_tmp1B7->fallthru_clause,((_tmp3A1->try_depth=_tmp1B7->try_depth,_tmp3A1)))))))))))));
# 830
struct Cyc_Tcenv_Fenv*_tmp3A6;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3A6=_region_malloc(r,sizeof(*_tmp3A6)),((_tmp3A6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1B2,((_tmp3A6->type_vars=(struct Cyc_List_List*)_tmp1B3,((_tmp3A6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1B4,((_tmp3A6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1B5,((_tmp3A6->encloser=(struct Cyc_Absyn_Stmt*)_tmp1B6,((_tmp3A6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3A6->capability=(void*)_tmp1B8,((_tmp3A6->curr_rgn=(void*)_tmp1B9,((_tmp3A6->in_notreadctxt=(int)_tmp1BA,((_tmp3A6->in_lhs=(int)_tmp1BB,((_tmp3A6->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3A6)))))))))))))))))))))));
# 833
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 836
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3A7;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3A7="enter_try",_tag_dyneither(_tmp3A7,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1C3;struct Cyc_Tcenv_Fenv*_tmp1C2=fe;_tmp1C3=_tmp1C2->ctrl_env;
++ _tmp1C3->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 842
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_CtrlEnv*_tmp1C7;const char*_tmp3A8;struct Cyc_Tcenv_Fenv*_tmp1C6=Cyc_Tcenv_get_fenv(te,((_tmp3A8="get_try_depth",_tag_dyneither(_tmp3A8,sizeof(char),14))));_tmp1C7=_tmp1C6->ctrl_env;
return _tmp1C7->try_depth;}
# 849
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3A9;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3A9="set_in_switch",_tag_dyneither(_tmp3A9,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1C9;struct Cyc_Tcenv_Fenv*_tmp1C8=fe;_tmp1C9=_tmp1C8->ctrl_env;
_tmp1C9->break_stmt=_tmp1C9->next_stmt;
_tmp1C9->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 857
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 859
struct Cyc_Tcenv_SharedFenv*_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_RgnOrder_RgnPO*_tmp1CF;const struct Cyc_Tcenv_Bindings*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1D1;struct Cyc_Tcenv_CtrlEnv*_tmp1D2;void*_tmp1D3;void*_tmp1D4;int _tmp1D5;int _tmp1D6;const char*_tmp3AA;struct Cyc_Tcenv_Fenv _tmp1CC=*
# 861
Cyc_Tcenv_get_fenv(te,((_tmp3AA="set_next",_tag_dyneither(_tmp3AA,sizeof(char),9))));_tmp1CD=_tmp1CC.shared;_tmp1CE=_tmp1CC.type_vars;_tmp1CF=_tmp1CC.region_order;_tmp1D0=_tmp1CC.locals;_tmp1D1=_tmp1CC.encloser;_tmp1D2=_tmp1CC.ctrl_env;_tmp1D3=_tmp1CC.capability;_tmp1D4=_tmp1CC.curr_rgn;_tmp1D5=_tmp1CC.in_notreadctxt;_tmp1D6=_tmp1CC.in_lhs;{
struct Cyc_Tcenv_CtrlEnv*_tmp3AB;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3AB=_region_malloc(r,sizeof(*_tmp3AB)),((_tmp3AB->ctrl_rgn=r,((_tmp3AB->continue_stmt=_tmp1D2->continue_stmt,((_tmp3AB->break_stmt=_tmp1D2->break_stmt,((_tmp3AB->next_stmt=j,((_tmp3AB->fallthru_clause=_tmp1D2->fallthru_clause,((_tmp3AB->try_depth=_tmp1D2->try_depth,_tmp3AB)))))))))))));
# 870
struct Cyc_Tcenv_Fenv*_tmp3AC;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3AC=_region_malloc(r,sizeof(*_tmp3AC)),((_tmp3AC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1CD,((_tmp3AC->type_vars=(struct Cyc_List_List*)_tmp1CE,((_tmp3AC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1CF,((_tmp3AC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1D0,((_tmp3AC->encloser=(struct Cyc_Absyn_Stmt*)_tmp1D1,((_tmp3AC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3AC->capability=(void*)_tmp1D3,((_tmp3AC->curr_rgn=(void*)_tmp1D4,((_tmp3AC->in_notreadctxt=(int)_tmp1D5,((_tmp3AC->in_lhs=(int)_tmp1D6,((_tmp3AC->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3AC)))))))))))))))))))))));
# 873
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 876
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_SharedFenv*_tmp1DB;const char*_tmp3AD;struct Cyc_Tcenv_Fenv*_tmp1DA=Cyc_Tcenv_get_fenv(te,((_tmp3AD="add_label",_tag_dyneither(_tmp3AD,sizeof(char),10))));_tmp1DB=_tmp1DA->shared;{
struct Cyc_Dict_Dict needed=_tmp1DB->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1DB->frgn;
if(sl_opt != 0){
_tmp1DB->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 884
struct Cyc_List_List*_tmp1DC=*sl_opt;
s->non_local_preds=_tmp1DC;
for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
void*_tmp1DD=((struct Cyc_Absyn_Stmt*)_tmp1DC->hd)->r;struct Cyc_Absyn_Stmt**_tmp1DF;_LL8D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp1DE=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp1DD;if(_tmp1DE->tag != 8)goto _LL8F;else{_tmp1DF=(struct Cyc_Absyn_Stmt**)& _tmp1DE->f2;}}_LL8E:
*_tmp1DF=s;goto _LL8C;_LL8F:;_LL90:
{const char*_tmp3AE;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3AE="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp3AE,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}
# 892
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1DB->seen_labels,v)){
const char*_tmp3B2;void*_tmp3B1[1];struct Cyc_String_pa_PrintArg_struct _tmp3B0;(_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B1[0]=& _tmp3B0,Cyc_Tcutil_terr(s->loc,((_tmp3B2="Repeated label: %s",_tag_dyneither(_tmp3B2,sizeof(char),19))),_tag_dyneither(_tmp3B1,sizeof(void*),1)))))));}
_tmp1DB->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1DB->seen_labels,v,s);
return te;};}
# 898
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp1E6;const char*_tmp3B3;struct Cyc_Tcenv_Fenv*_tmp1E5=Cyc_Tcenv_get_fenv(te,((_tmp3B3="all_labels_resolved",_tag_dyneither(_tmp3B3,sizeof(char),20))));_tmp1E6=_tmp1E5->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1E6->needed_labels);}
# 903
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Stmt*_tmp1E9;const char*_tmp3B4;struct Cyc_Tcenv_Fenv*_tmp1E8=Cyc_Tcenv_get_fenv(te,((_tmp3B4="get_encloser",_tag_dyneither(_tmp3B4,sizeof(char),13))));_tmp1E9=_tmp1E8->encloser;
return _tmp1E9;}
# 908
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp3B5;return(_tmp3B5=_region_malloc(r2,sizeof(*_tmp3B5)),((_tmp3B5->ns=te->ns,((_tmp3B5->ae=te->ae,((_tmp3B5->le=0,((_tmp3B5->allow_valueof=1,_tmp3B5)))))))));}{
struct Cyc_Tcenv_SharedFenv*_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_RgnOrder_RgnPO*_tmp1EE;const struct Cyc_Tcenv_Bindings*_tmp1EF;struct Cyc_Absyn_Stmt*_tmp1F0;struct Cyc_Tcenv_CtrlEnv*_tmp1F1;void*_tmp1F2;void*_tmp1F3;int _tmp1F4;int _tmp1F5;struct Cyc_Tcenv_Fenv _tmp1EB=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp1EC=_tmp1EB.shared;_tmp1ED=_tmp1EB.type_vars;_tmp1EE=_tmp1EB.region_order;_tmp1EF=_tmp1EB.locals;_tmp1F0=_tmp1EB.encloser;_tmp1F1=_tmp1EB.ctrl_env;_tmp1F2=_tmp1EB.capability;_tmp1F3=_tmp1EB.curr_rgn;_tmp1F4=_tmp1EB.in_notreadctxt;_tmp1F5=_tmp1EB.in_lhs;{
struct Cyc_Tcenv_Fenv*_tmp3B6;struct Cyc_Tcenv_Fenv*_tmp1F6=(_tmp3B6=_region_malloc(r2,sizeof(*_tmp3B6)),((_tmp3B6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1EC,((_tmp3B6->type_vars=(struct Cyc_List_List*)_tmp1ED,((_tmp3B6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1EE,((_tmp3B6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1EF,((_tmp3B6->encloser=(struct Cyc_Absyn_Stmt*)_tmp1F0,((_tmp3B6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1F1,((_tmp3B6->capability=(void*)_tmp1F2,((_tmp3B6->curr_rgn=(void*)_tmp1F3,((_tmp3B6->in_notreadctxt=(int)_tmp1F4,((_tmp3B6->in_lhs=(int)_tmp1F5,((_tmp3B6->fnrgn=(struct _RegionHandle*)r2,(struct Cyc_Tcenv_Fenv*)_tmp3B6)))))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp3B7;return(_tmp3B7=_region_malloc(r2,sizeof(*_tmp3B7)),((_tmp3B7->ns=te->ns,((_tmp3B7->ae=te->ae,((_tmp3B7->le=_tmp1F6,((_tmp3B7->allow_valueof=1,_tmp3B7)))))))));};};}
# 916
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 918
struct Cyc_Tcenv_SharedFenv*_tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_RgnOrder_RgnPO*_tmp1FD;const struct Cyc_Tcenv_Bindings*_tmp1FE;struct Cyc_Absyn_Stmt*_tmp1FF;struct Cyc_Tcenv_CtrlEnv*_tmp200;void*_tmp201;void*_tmp202;int _tmp203;int _tmp204;const char*_tmp3B8;struct Cyc_Tcenv_Fenv _tmp1FA=*
# 920
Cyc_Tcenv_get_fenv(te,((_tmp3B8="set_encloser",_tag_dyneither(_tmp3B8,sizeof(char),13))));_tmp1FB=_tmp1FA.shared;_tmp1FC=_tmp1FA.type_vars;_tmp1FD=_tmp1FA.region_order;_tmp1FE=_tmp1FA.locals;_tmp1FF=_tmp1FA.encloser;_tmp200=_tmp1FA.ctrl_env;_tmp201=_tmp1FA.capability;_tmp202=_tmp1FA.curr_rgn;_tmp203=_tmp1FA.in_notreadctxt;_tmp204=_tmp1FA.in_lhs;{
struct Cyc_Tcenv_Fenv*_tmp3B9;struct Cyc_Tcenv_Fenv*_tmp205=
(_tmp3B9=_region_malloc(r,sizeof(*_tmp3B9)),((_tmp3B9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FB,((_tmp3B9->type_vars=(struct Cyc_List_List*)_tmp1FC,((_tmp3B9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FD,((_tmp3B9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FE,((_tmp3B9->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp3B9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp200,((_tmp3B9->capability=(void*)_tmp201,((_tmp3B9->curr_rgn=(void*)_tmp202,((_tmp3B9->in_notreadctxt=(int)_tmp203,((_tmp3B9->in_lhs=(int)_tmp204,((_tmp3B9->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3B9)))))))))))))))))))))));
# 924
return Cyc_Tcenv_put_fenv(r,te,_tmp205);};}
# 927
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 930
struct Cyc_Absyn_Tvar*tv;
{void*_tmp207=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp209;_LL92: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp207;if(_tmp208->tag != 2)goto _LL94;else{_tmp209=_tmp208->f1;}}_LL93:
 tv=_tmp209;goto _LL91;_LL94:;_LL95:
{const char*_tmp3BA;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3BA="bad add region",_tag_dyneither(_tmp3BA,sizeof(char),15))));}goto _LL91;_LL91:;}{
# 935
struct Cyc_Tcenv_SharedFenv*_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_RgnOrder_RgnPO*_tmp20F;const struct Cyc_Tcenv_Bindings*_tmp210;struct Cyc_Absyn_Stmt*_tmp211;struct Cyc_Tcenv_CtrlEnv*_tmp212;void*_tmp213;void*_tmp214;int _tmp215;int _tmp216;const char*_tmp3BB;struct Cyc_Tcenv_Fenv _tmp20C=*
# 937
Cyc_Tcenv_get_fenv(te,((_tmp3BB="add_region",_tag_dyneither(_tmp3BB,sizeof(char),11))));_tmp20D=_tmp20C.shared;_tmp20E=_tmp20C.type_vars;_tmp20F=_tmp20C.region_order;_tmp210=_tmp20C.locals;_tmp211=_tmp20C.encloser;_tmp212=_tmp20C.ctrl_env;_tmp213=_tmp20C.capability;_tmp214=_tmp20C.curr_rgn;_tmp215=_tmp20C.in_notreadctxt;_tmp216=_tmp20C.in_lhs;
_tmp20F=Cyc_RgnOrder_add_youngest(_tmp20D->frgn,_tmp20F,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3CC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3CB;struct Cyc_List_List*_tmp3CA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C9;_tmp213=(void*)((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9[0]=((_tmp3CE.tag=24,((_tmp3CE.f1=((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->hd=(void*)((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=((_tmp3CB.tag=23,((_tmp3CB.f1=rgn,_tmp3CB)))),_tmp3CC)))),((_tmp3CA->tl=(
(_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->hd=_tmp213,((_tmp3CD->tl=0,_tmp3CD)))))),_tmp3CA)))))),_tmp3CE)))),_tmp3C9))));}{
struct Cyc_Tcenv_Fenv*_tmp3CF;struct Cyc_Tcenv_Fenv*_tmp21D=
(_tmp3CF=_region_malloc(r,sizeof(*_tmp3CF)),((_tmp3CF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20D,((_tmp3CF->type_vars=(struct Cyc_List_List*)_tmp20E,((_tmp3CF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp20F,((_tmp3CF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp210,((_tmp3CF->encloser=(struct Cyc_Absyn_Stmt*)_tmp211,((_tmp3CF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp212,((_tmp3CF->capability=(void*)_tmp213,((_tmp3CF->curr_rgn=(void*)_tmp214,((_tmp3CF->in_notreadctxt=(int)_tmp215,((_tmp3CF->in_lhs=(int)_tmp216,((_tmp3CF->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3CF)))))))))))))))))))))));
# 944
return Cyc_Tcenv_put_fenv(r,te,_tmp21D);};};}
# 947
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 949
struct Cyc_Tcenv_SharedFenv*_tmp221;struct Cyc_List_List*_tmp222;struct Cyc_RgnOrder_RgnPO*_tmp223;const struct Cyc_Tcenv_Bindings*_tmp224;struct Cyc_Absyn_Stmt*_tmp225;struct Cyc_Tcenv_CtrlEnv*_tmp226;void*_tmp227;void*_tmp228;int _tmp229;int _tmp22A;const char*_tmp3D0;struct Cyc_Tcenv_Fenv _tmp220=*
# 951
Cyc_Tcenv_get_fenv(te,((_tmp3D0="new_named_block",_tag_dyneither(_tmp3D0,sizeof(char),16))));_tmp221=_tmp220.shared;_tmp222=_tmp220.type_vars;_tmp223=_tmp220.region_order;_tmp224=_tmp220.locals;_tmp225=_tmp220.encloser;_tmp226=_tmp220.ctrl_env;_tmp227=_tmp220.capability;_tmp228=_tmp220.curr_rgn;_tmp229=_tmp220.in_notreadctxt;_tmp22A=_tmp220.in_lhs;{
# 953
const char*_tmp3D1;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3D1="new_block",_tag_dyneither(_tmp3D1,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3D4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D3;void*block_typ=(void*)((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=((_tmp3D4.tag=2,((_tmp3D4.f1=block_rgn,_tmp3D4)))),_tmp3D3))));
{struct Cyc_List_List*_tmp3D5;_tmp222=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->hd=block_rgn,((_tmp3D5->tl=_tmp222,_tmp3D5))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp222);
_tmp223=Cyc_RgnOrder_add_youngest(_tmp221->frgn,_tmp223,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp3E8;struct Cyc_List_List*_tmp3E7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3E6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3E5;struct Cyc_List_List*_tmp3E4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3E3;_tmp227=(void*)((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=((_tmp3E8.tag=24,((_tmp3E8.f1=((_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((_tmp3E4->hd=(void*)((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E5.tag=23,((_tmp3E5.f1=block_typ,_tmp3E5)))),_tmp3E6)))),((_tmp3E4->tl=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=_tmp227,((_tmp3E7->tl=0,_tmp3E7)))))),_tmp3E4)))))),_tmp3E8)))),_tmp3E3))));}
_tmp228=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp3E9;struct Cyc_Tcenv_Fenv*_tmp232=
(_tmp3E9=_region_malloc(r,sizeof(*_tmp3E9)),((_tmp3E9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp221,((_tmp3E9->type_vars=(struct Cyc_List_List*)_tmp222,((_tmp3E9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp223,((_tmp3E9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp224,((_tmp3E9->encloser=(struct Cyc_Absyn_Stmt*)_tmp225,((_tmp3E9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp226,((_tmp3E9->capability=(void*)_tmp227,((_tmp3E9->curr_rgn=(void*)_tmp228,((_tmp3E9->in_notreadctxt=(int)_tmp229,((_tmp3E9->in_lhs=(int)_tmp22A,((_tmp3E9->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3E9)))))))))))))))))))))));
# 963
return Cyc_Tcenv_put_fenv(r,te,_tmp232);};};}
# 966
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 968
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 970
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 976
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 980
struct Cyc_Tcenv_SharedFenv*_tmp23A;struct Cyc_List_List*_tmp23B;struct Cyc_RgnOrder_RgnPO*_tmp23C;const struct Cyc_Tcenv_Bindings*_tmp23D;struct Cyc_Absyn_Stmt*_tmp23E;struct Cyc_Tcenv_CtrlEnv*_tmp23F;void*_tmp240;void*_tmp241;int _tmp242;int _tmp243;const char*_tmp3EA;struct Cyc_Tcenv_Fenv _tmp239=*
# 982
Cyc_Tcenv_get_fenv(te,((_tmp3EA="new_outlives_constraints",_tag_dyneither(_tmp3EA,sizeof(char),25))));_tmp23A=_tmp239.shared;_tmp23B=_tmp239.type_vars;_tmp23C=_tmp239.region_order;_tmp23D=_tmp239.locals;_tmp23E=_tmp239.encloser;_tmp23F=_tmp239.ctrl_env;_tmp240=_tmp239.capability;_tmp241=_tmp239.curr_rgn;_tmp242=_tmp239.in_notreadctxt;_tmp243=_tmp239.in_lhs;
for(0;cs != 0;cs=cs->tl){
_tmp23C=
Cyc_RgnOrder_add_outlives_constraint(_tmp23A->frgn,_tmp23C,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 988
struct Cyc_Tcenv_Fenv*_tmp3EB;struct Cyc_Tcenv_Fenv*_tmp244=
(_tmp3EB=_region_malloc(r,sizeof(*_tmp3EB)),((_tmp3EB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp23A,((_tmp3EB->type_vars=(struct Cyc_List_List*)_tmp23B,((_tmp3EB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23C,((_tmp3EB->locals=(const struct Cyc_Tcenv_Bindings*)_tmp23D,((_tmp3EB->encloser=(struct Cyc_Absyn_Stmt*)_tmp23E,((_tmp3EB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp23F,((_tmp3EB->capability=(void*)_tmp240,((_tmp3EB->curr_rgn=(void*)_tmp241,((_tmp3EB->in_notreadctxt=(int)_tmp242,((_tmp3EB->in_lhs=(int)_tmp243,((_tmp3EB->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3EB)))))))))))))))))))))));
# 991
return Cyc_Tcenv_put_fenv(r,te,_tmp244);};}
# 994
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1002
void*_tmp246=Cyc_Tcutil_compress(r1);
void*_tmp247=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp248=Cyc_Tcutil_typ_kind(_tmp246);
struct Cyc_Absyn_Kind*_tmp249=Cyc_Tcutil_typ_kind(_tmp247);
# 1008
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp248,_tmp249);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp249,_tmp248);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp3F2;void*_tmp3F1[4];struct Cyc_String_pa_PrintArg_struct _tmp3F0;struct Cyc_String_pa_PrintArg_struct _tmp3EF;struct Cyc_String_pa_PrintArg_struct _tmp3EE;struct Cyc_String_pa_PrintArg_struct _tmp3ED;(_tmp3ED.tag=0,((_tmp3ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp249)),((_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp248)),((_tmp3EF.tag=0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp247)),((_tmp3F0.tag=0,((_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp246)),((_tmp3F1[0]=& _tmp3F0,((_tmp3F1[1]=& _tmp3EF,((_tmp3F1[2]=& _tmp3EE,((_tmp3F1[3]=& _tmp3ED,Cyc_Tcutil_terr(loc,((_tmp3F2="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp3F2,sizeof(char),82))),_tag_dyneither(_tmp3F1,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1015
if(r1_le_r2  && !r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp251;void*_tmp252;struct _tuple11 _tmp250=Cyc_Tcutil_swap_kind(_tmp247,Cyc_Tcutil_kind_to_bound(_tmp248));_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;{
struct _tuple11*_tmp3F3;*oldtv=((_tmp3F3=_region_malloc(r,sizeof(*_tmp3F3)),((_tmp3F3->f1=_tmp251,((_tmp3F3->f2=_tmp252,_tmp3F3))))));};}else{
# 1019
if(!r1_le_r2  && r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp255;void*_tmp256;struct _tuple11 _tmp254=Cyc_Tcutil_swap_kind(_tmp246,Cyc_Tcutil_kind_to_bound(_tmp249));_tmp255=_tmp254.f1;_tmp256=_tmp254.f2;{
struct _tuple11*_tmp3F4;*oldtv=((_tmp3F4=_region_malloc(r,sizeof(*_tmp3F4)),((_tmp3F4->f1=_tmp255,((_tmp3F4->f2=_tmp256,_tmp3F4))))));};}}}{
# 1025
struct _RegionHandle*_tmp258=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp259=0;
if((_tmp246 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp246 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp246 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3F7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3F6;void*eff1=(void*)((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3F7.tag=23,((_tmp3F7.f1=_tmp246,_tmp3F7)))),_tmp3F6))));
struct _tuple13*_tmp3FA;struct Cyc_List_List*_tmp3F9;_tmp259=((_tmp3F9=_region_malloc(_tmp258,sizeof(*_tmp3F9)),((_tmp3F9->hd=((_tmp3FA=_region_malloc(_tmp258,sizeof(*_tmp3FA)),((_tmp3FA->f1=eff1,((_tmp3FA->f2=_tmp247,_tmp3FA)))))),((_tmp3F9->tl=_tmp259,_tmp3F9))))));}
# 1031
if((_tmp247 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp247 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp247 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3FD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3FC;void*eff2=(void*)((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=((_tmp3FD.tag=23,((_tmp3FD.f1=_tmp247,_tmp3FD)))),_tmp3FC))));
struct _tuple13*_tmp400;struct Cyc_List_List*_tmp3FF;_tmp259=((_tmp3FF=_region_malloc(_tmp258,sizeof(*_tmp3FF)),((_tmp3FF->hd=((_tmp400=_region_malloc(_tmp258,sizeof(*_tmp400)),((_tmp400->f1=eff2,((_tmp400->f2=_tmp246,_tmp400)))))),((_tmp3FF->tl=_tmp259,_tmp3FF))))));}
# 1036
return Cyc_Tcenv_new_outlives_constraints(_tmp258,te,_tmp259,loc);};}
# 1039
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp262=te->le;
if(_tmp262 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp262;
void*_tmp264;struct Cyc_Tcenv_Fenv*_tmp263=fe;_tmp264=_tmp263->curr_rgn;
return _tmp264;};}
# 1050
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp401;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp401="check_rgn_accessible",_tag_dyneither(_tmp401,sizeof(char),21))));
void*_tmp266;struct Cyc_RgnOrder_RgnPO*_tmp267;struct Cyc_Tcenv_Fenv*_tmp265=fe;_tmp266=_tmp265->capability;_tmp267=_tmp265->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp266) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp266))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp404;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp403;if(Cyc_RgnOrder_eff_outlives_eff(_tmp267,(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp404.tag=23,((_tmp404.f1=rgn,_tmp404)))),_tmp403)))),_tmp266))
return;}{
const char*_tmp408;void*_tmp407[1];struct Cyc_String_pa_PrintArg_struct _tmp406;(_tmp406.tag=0,((_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp407[0]=& _tmp406,Cyc_Tcutil_terr(loc,((_tmp408="Expression accesses unavailable region %s",_tag_dyneither(_tmp408,sizeof(char),42))),_tag_dyneither(_tmp407,sizeof(void*),1)))))));};}
# 1062
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_RgnOrder_RgnPO*_tmp270;const char*_tmp409;struct Cyc_Tcenv_Fenv*_tmp26F=
Cyc_Tcenv_get_fenv(te,((_tmp409="check_rgn_resetable",_tag_dyneither(_tmp409,sizeof(char),20))));_tmp270=_tmp26F->region_order;{
void*_tmp271=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp273;_LL97: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp272=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp271;if(_tmp272->tag != 2)goto _LL99;else{_tmp273=_tmp272->f1;}}_LL98:
# 1068
 if(!Cyc_RgnOrder_is_region_resetable(_tmp270,_tmp273)){
const char*_tmp40D;void*_tmp40C[1];struct Cyc_String_pa_PrintArg_struct _tmp40B;(_tmp40B.tag=0,((_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp40C[0]=& _tmp40B,Cyc_Tcutil_terr(loc,((_tmp40D="Region %s is not resetable",_tag_dyneither(_tmp40D,sizeof(char),27))),_tag_dyneither(_tmp40C,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {
# 1072
const char*_tmp410;void*_tmp40F;(_tmp40F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp410="check_rgn_resetable",_tag_dyneither(_tmp410,sizeof(char),20))),_tag_dyneither(_tmp40F,sizeof(void*),0)));}_LL96:;};};}
# 1079
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp279=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1084
if(_tmp279 == 0){
void*_tmp27A=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp27B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp27A;if(_tmp27B->tag != 22)goto _LL9E;}_LL9D:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp27C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp27A;if(_tmp27C->tag != 21)goto _LLA0;}_LL9F:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp27D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp27A;if(_tmp27D->tag != 20)goto _LLA2;}_LLA1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3:
 return 0;_LL9B:;}{
# 1092
struct Cyc_Tcenv_Fenv*fe=_tmp279;
struct Cyc_RgnOrder_RgnPO*_tmp27F;struct Cyc_Tcenv_Fenv*_tmp27E=fe;_tmp27F=_tmp27E->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp413;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp412;int _tmp280=Cyc_RgnOrder_effect_outlives(_tmp27F,(void*)((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=((_tmp413.tag=23,((_tmp413.f1=rt_a,_tmp413)))),_tmp412)))),rt_b);
# 1098
return _tmp280;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1103
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
void*_tmp285;struct Cyc_RgnOrder_RgnPO*_tmp286;struct Cyc_Tcenv_SharedFenv*_tmp287;const char*_tmp414;struct Cyc_Tcenv_Fenv*_tmp284=
Cyc_Tcenv_get_fenv(te,((_tmp414="check_effect_accessible",_tag_dyneither(_tmp414,sizeof(char),24))));_tmp285=_tmp284->capability;_tmp286=_tmp284->region_order;_tmp287=_tmp284->shared;
if(Cyc_Tcutil_subset_effect(0,eff,_tmp285))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp286,eff,_tmp285))
return;{
struct _RegionHandle*frgn=_tmp287->frgn;
struct _tuple14*_tmp417;struct Cyc_List_List*_tmp416;_tmp287->delayed_effect_checks=(
(_tmp416=_region_malloc(frgn,sizeof(*_tmp416)),((_tmp416->hd=((_tmp417=_region_malloc(frgn,sizeof(*_tmp417)),((_tmp417->f1=_tmp285,((_tmp417->f2=eff,((_tmp417->f3=_tmp286,((_tmp417->f4=loc,_tmp417)))))))))),((_tmp416->tl=_tmp287->delayed_effect_checks,_tmp416))))));};}
# 1115
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp28C;const char*_tmp418;struct Cyc_Tcenv_Fenv*_tmp28B=
Cyc_Tcenv_get_fenv(te,((_tmp418="check_delayed_effects",_tag_dyneither(_tmp418,sizeof(char),22))));_tmp28C=_tmp28B->shared;{
struct Cyc_List_List*_tmp28D=_tmp28C->delayed_effect_checks;
for(0;_tmp28D != 0;_tmp28D=_tmp28D->tl){
void*_tmp28F;void*_tmp290;struct Cyc_RgnOrder_RgnPO*_tmp291;unsigned int _tmp292;struct _tuple14*_tmp28E=(struct _tuple14*)_tmp28D->hd;_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;_tmp291=_tmp28E->f3;_tmp292=_tmp28E->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp290,_tmp28F))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp291,_tmp290,_tmp28F))
continue;{
const char*_tmp41D;void*_tmp41C[2];struct Cyc_String_pa_PrintArg_struct _tmp41B;struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp290)),((_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp28F)),((_tmp41C[0]=& _tmp41B,((_tmp41C[1]=& _tmp41A,Cyc_Tcutil_terr(_tmp292,((_tmp41D="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp41D,sizeof(char),51))),_tag_dyneither(_tmp41C,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1134
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1136
struct Cyc_Tcenv_Fenv*_tmp297=te->le;
if(_tmp297 == 0){
# 1139
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp420;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp41F;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp420.tag=23,((_tmp420.f1=(*((struct _tuple13*)po->hd)).f2,_tmp420)))),_tmp41F)))),Cyc_Absyn_empty_effect)){
const char*_tmp423;void*_tmp422;(_tmp422=0,Cyc_Tcutil_terr(loc,((_tmp423="the required region ordering is not satisfied here",_tag_dyneither(_tmp423,sizeof(char),51))),_tag_dyneither(_tmp422,sizeof(void*),0)));}}
return;}{
# 1145
struct Cyc_RgnOrder_RgnPO*_tmp29D;struct Cyc_Tcenv_SharedFenv*_tmp29E;struct Cyc_Tcenv_Fenv*_tmp29C=_tmp297;_tmp29D=_tmp29C->region_order;_tmp29E=_tmp29C->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp29D,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp426;struct Cyc_List_List*_tmp425;_tmp29E->delayed_constraint_checks=(
(_tmp425=_region_malloc(_tmp29E->frgn,sizeof(*_tmp425)),((_tmp425->hd=((_tmp426=_region_malloc(_tmp29E->frgn,sizeof(*_tmp426)),((_tmp426->f1=_tmp29D,((_tmp426->f2=po,((_tmp426->f3=loc,_tmp426)))))))),((_tmp425->tl=_tmp29E->delayed_constraint_checks,_tmp425))))));}};}
# 1152
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp2A3;const char*_tmp427;struct Cyc_Tcenv_Fenv*_tmp2A2=
Cyc_Tcenv_get_fenv(te,((_tmp427="check_delayed_constraints",_tag_dyneither(_tmp427,sizeof(char),26))));_tmp2A3=_tmp2A2->shared;{
struct Cyc_List_List*_tmp2A4=_tmp2A3->delayed_constraint_checks;
for(0;_tmp2A4 != 0;_tmp2A4=_tmp2A4->tl){
struct Cyc_RgnOrder_RgnPO*_tmp2A6;struct Cyc_List_List*_tmp2A7;unsigned int _tmp2A8;struct _tuple15*_tmp2A5=(struct _tuple15*)_tmp2A4->hd;_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;_tmp2A8=_tmp2A5->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2A6,_tmp2A7,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp42A;void*_tmp429;(_tmp429=0,Cyc_Tcutil_terr(_tmp2A8,((_tmp42A="the required region ordering is not satisfied here",_tag_dyneither(_tmp42A,sizeof(char),51))),_tag_dyneither(_tmp429,sizeof(void*),0)));}}};}
# 1163
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp437;const char*_tmp436;void*_tmp435[1];struct Cyc_String_pa_PrintArg_struct _tmp434;struct Cyc_Absyn_Tvar*_tmp433;struct Cyc_Absyn_Tvar*rgn0=
(_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->name=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=(struct _dyneither_ptr)((_tmp434.tag=0,((_tmp434.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp435[0]=& _tmp434,Cyc_aprintf(((_tmp436="`%s",_tag_dyneither(_tmp436,sizeof(char),4))),_tag_dyneither(_tmp435,sizeof(void*),1)))))))),_tmp437)))),((_tmp433->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp433->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp433)))))));
struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp2AB=(_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->hd=rgn0,((_tmp438->tl=fd->tvs,_tmp438)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2AB);{
# 1171
struct Cyc_RgnOrder_RgnPO*_tmp2AC=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1173
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp43B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43A;void*param_rgn=(void*)((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((_tmp43B.tag=2,((_tmp43B.f1=rgn0,_tmp43B)))),_tmp43A))));
struct Cyc_List_List*_tmp2AD=0;
{struct Cyc_List_List*_tmp2AE=fd->args;for(0;_tmp2AE != 0;_tmp2AE=_tmp2AE->tl){
struct Cyc_Absyn_Vardecl _tmp441;struct _tuple0*_tmp440;struct Cyc_Absyn_Vardecl*_tmp43F;struct Cyc_Absyn_Vardecl*_tmp2AF=(_tmp43F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp43F[0]=((_tmp441.sc=Cyc_Absyn_Public,((_tmp441.name=(
(_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->f1=Cyc_Absyn_Loc_n,((_tmp440->f2=(*((struct _tuple8*)_tmp2AE->hd)).f1,_tmp440)))))),((_tmp441.tq=(*((struct _tuple8*)_tmp2AE->hd)).f2,((_tmp441.type=(*((struct _tuple8*)_tmp2AE->hd)).f3,((_tmp441.initializer=0,((_tmp441.rgn=param_rgn,((_tmp441.attributes=0,((_tmp441.escapes=0,_tmp441)))))))))))))))),_tmp43F)));
# 1183
{struct Cyc_List_List _tmp444;struct Cyc_List_List*_tmp443;_tmp2AD=((_tmp443=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp443[0]=((_tmp444.hd=_tmp2AF,((_tmp444.tl=_tmp2AD,_tmp444)))),_tmp443))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp447;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp446;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2B2=(_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446[0]=((_tmp447.tag=3,((_tmp447.f1=_tmp2AF,_tmp447)))),_tmp446)));
struct _dyneither_ptr*_tmp2B3=(*((struct _tuple8*)_tmp2AE->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp448;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp448=_region_malloc(r,sizeof(*_tmp448)),((_tmp448->v=_tmp2B3,((_tmp448->b=(void*)_tmp2B2,((_tmp448->tl=locals,_tmp448))))))));};}}
# 1188
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp2BB;struct Cyc_Absyn_Tqual _tmp2BC;void*_tmp2BD;int _tmp2BE;struct Cyc_Absyn_VarargInfo _tmp2BA=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2BB=_tmp2BA.name;_tmp2BC=_tmp2BA.tq;_tmp2BD=_tmp2BA.type;_tmp2BE=_tmp2BA.inject;
if(_tmp2BB != 0){
void*_tmp2BF=Cyc_Absyn_dyneither_typ(_tmp2BD,param_rgn,_tmp2BC,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp44E;struct _tuple0*_tmp44D;struct Cyc_Absyn_Vardecl*_tmp44C;struct Cyc_Absyn_Vardecl*_tmp2C0=(_tmp44C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp44C[0]=((_tmp44E.sc=Cyc_Absyn_Public,((_tmp44E.name=(
(_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->f1=Cyc_Absyn_Loc_n,((_tmp44D->f2=_tmp2BB,_tmp44D)))))),((_tmp44E.tq=
Cyc_Absyn_empty_tqual(0),((_tmp44E.type=_tmp2BF,((_tmp44E.initializer=0,((_tmp44E.rgn=param_rgn,((_tmp44E.attributes=0,((_tmp44E.escapes=0,_tmp44E)))))))))))))))),_tmp44C)));
# 1199
{struct Cyc_List_List*_tmp44F;_tmp2AD=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->hd=_tmp2C0,((_tmp44F->tl=_tmp2AD,_tmp44F))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp452;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp451;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2C2=(_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=((_tmp452.tag=3,((_tmp452.f1=_tmp2C0,_tmp452)))),_tmp451)));
struct _dyneither_ptr*_tmp2C3=_tmp2BB;
struct Cyc_Tcenv_Bindings*_tmp453;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp453=_region_malloc(r,sizeof(*_tmp453)),((_tmp453->v=_tmp2C3,((_tmp453->b=(void*)_tmp2C2,((_tmp453->tl=locals,_tmp453))))))));};}else{
# 1204
const char*_tmp456;void*_tmp455;(_tmp455=0,Cyc_Tcutil_terr(loc,((_tmp456="missing name for varargs",_tag_dyneither(_tmp456,sizeof(char),25))),_tag_dyneither(_tmp455,sizeof(void*),0)));}}
# 1206
{struct Cyc_Core_Opt _tmp459;struct Cyc_Core_Opt*_tmp458;fd->param_vardecls=((_tmp458=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp458[0]=((_tmp459.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2AD),_tmp459)),_tmp458))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp477;struct Cyc_List_List*_tmp476;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp475;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp474;struct Cyc_List_List*_tmp473;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp472;struct Cyc_Tcenv_CtrlEnv*_tmp471;struct Cyc_Tcenv_SharedFenv*_tmp470;struct Cyc_Tcenv_Fenv*_tmp46F;return(_tmp46F=_region_malloc(r,sizeof(*_tmp46F)),((_tmp46F->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp470=_region_malloc(r,sizeof(*_tmp470)),((_tmp470->frgn=r,((_tmp470->return_typ=fd->ret_type,((_tmp470->seen_labels=
# 1211
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp470->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp470->delayed_effect_checks=0,((_tmp470->delayed_constraint_checks=0,_tmp470)))))))))))))),((_tmp46F->type_vars=(struct Cyc_List_List*)_tmp2AB,((_tmp46F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2AC,((_tmp46F->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp46F->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp46F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1220
(_tmp471=_region_malloc(r,sizeof(*_tmp471)),((_tmp471->ctrl_rgn=r,((_tmp471->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp471->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp471->fallthru_clause=0,((_tmp471->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp471->try_depth=0,_tmp471)))))))))))))),((_tmp46F->capability=(void*)((void*)(
(_tmp477=_cycalloc(sizeof(*_tmp477)),((_tmp477[0]=((_tmp472.tag=24,((_tmp472.f1=((_tmp476=_cycalloc(sizeof(*_tmp476)),((_tmp476->hd=(void*)((_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[0]=((_tmp475.tag=23,((_tmp475.f1=param_rgn,_tmp475)))),_tmp474)))),((_tmp476->tl=(
(_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473->hd=(void*)_check_null(fd->effect),((_tmp473->tl=0,_tmp473)))))),_tmp476)))))),_tmp472)))),_tmp477))))),((_tmp46F->curr_rgn=(void*)param_rgn,((_tmp46F->in_notreadctxt=(int)0,((_tmp46F->in_lhs=(int)0,((_tmp46F->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp46F)))))))))))))))))))))));};};}
# 1230
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp2E0;struct Cyc_RgnOrder_RgnPO*_tmp2E1;struct Cyc_List_List*_tmp2E2;struct Cyc_Tcenv_SharedFenv*_tmp2E3;struct _RegionHandle*_tmp2E4;struct Cyc_Tcenv_Fenv*_tmp2DF=old_fenv;_tmp2E0=_tmp2DF->locals;_tmp2E1=_tmp2DF->region_order;_tmp2E2=_tmp2DF->type_vars;_tmp2E3=_tmp2DF->shared;_tmp2E4=_tmp2DF->fnrgn;{
# 1233
struct _RegionHandle*_tmp2E5=_tmp2E3->frgn;
const struct Cyc_Tcenv_Bindings*_tmp2E6=_tmp2E0;
struct _dyneither_ptr*_tmp484;const char*_tmp483;void*_tmp482[1];struct Cyc_String_pa_PrintArg_struct _tmp481;struct Cyc_Absyn_Tvar*_tmp480;struct Cyc_Absyn_Tvar*rgn0=
(_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480->name=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=(struct _dyneither_ptr)((_tmp481.tag=0,((_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp482[0]=& _tmp481,Cyc_aprintf(((_tmp483="`%s",_tag_dyneither(_tmp483,sizeof(char),4))),_tag_dyneither(_tmp482,sizeof(void*),1)))))))),_tmp484)))),((_tmp480->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp480->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp480)))))));
# 1239
{struct Cyc_List_List*_tmp2E7=fd->tvs;for(0;_tmp2E7 != 0;_tmp2E7=_tmp2E7->tl){
enum Cyc_Absyn_KindQual _tmp2E9;enum Cyc_Absyn_AliasQual _tmp2EA;struct Cyc_Absyn_Kind*_tmp2E8=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2E7->hd,& Cyc_Tcutil_bk);_tmp2E9=_tmp2E8->kind;_tmp2EA=_tmp2E8->aliasqual;
if(_tmp2E9 == Cyc_Absyn_RgnKind){
if(_tmp2EA == Cyc_Absyn_Aliasable)
_tmp2E1=Cyc_RgnOrder_add_youngest(_tmp2E5,_tmp2E1,(struct Cyc_Absyn_Tvar*)_tmp2E7->hd,0,0);else{
# 1245
const char*_tmp487;void*_tmp486;(_tmp486=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp487="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp487,sizeof(char),39))),_tag_dyneither(_tmp486,sizeof(void*),0)));}}}}
# 1247
_tmp2E1=Cyc_RgnOrder_add_youngest(_tmp2E5,_tmp2E1,rgn0,0,0);{
struct Cyc_List_List*_tmp488;struct Cyc_List_List*_tmp2ED=(_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=rgn0,((_tmp488->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp2E2),_tmp488)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2ED);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp48B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp48A;void*param_rgn=(void*)((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp48B.tag=2,((_tmp48B.f1=rgn0,_tmp48B)))),_tmp48A))));
struct Cyc_List_List*_tmp2EE=0;
{struct Cyc_List_List*_tmp2EF=fd->args;for(0;_tmp2EF != 0;_tmp2EF=_tmp2EF->tl){
struct Cyc_Absyn_Vardecl _tmp491;struct _tuple0*_tmp490;struct Cyc_Absyn_Vardecl*_tmp48F;struct Cyc_Absyn_Vardecl*_tmp2F0=(_tmp48F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp48F[0]=((_tmp491.sc=Cyc_Absyn_Public,((_tmp491.name=(
(_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490->f1=Cyc_Absyn_Loc_n,((_tmp490->f2=(*((struct _tuple8*)_tmp2EF->hd)).f1,_tmp490)))))),((_tmp491.tq=(*((struct _tuple8*)_tmp2EF->hd)).f2,((_tmp491.type=(*((struct _tuple8*)_tmp2EF->hd)).f3,((_tmp491.initializer=0,((_tmp491.rgn=param_rgn,((_tmp491.attributes=0,((_tmp491.escapes=0,_tmp491)))))))))))))))),_tmp48F)));
# 1260
{struct Cyc_List_List _tmp494;struct Cyc_List_List*_tmp493;_tmp2EE=((_tmp493=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp493[0]=((_tmp494.hd=_tmp2F0,((_tmp494.tl=_tmp2EE,_tmp494)))),_tmp493))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp497;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp496;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F3=(_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496[0]=((_tmp497.tag=3,((_tmp497.f1=_tmp2F0,_tmp497)))),_tmp496)));
struct _dyneither_ptr*_tmp2F4=(*((struct _tuple8*)_tmp2EF->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp498;_tmp2E6=(const struct Cyc_Tcenv_Bindings*)((_tmp498=_region_malloc(_tmp2E5,sizeof(*_tmp498)),((_tmp498->v=_tmp2F4,((_tmp498->b=(void*)_tmp2F3,((_tmp498->tl=_tmp2E6,_tmp498))))))));};}}
# 1265
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp2FC;struct Cyc_Absyn_Tqual _tmp2FD;void*_tmp2FE;int _tmp2FF;struct Cyc_Absyn_VarargInfo _tmp2FB=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2FC=_tmp2FB.name;_tmp2FD=_tmp2FB.tq;_tmp2FE=_tmp2FB.type;_tmp2FF=_tmp2FB.inject;
if(_tmp2FC != 0){
void*_tmp300=Cyc_Absyn_dyneither_typ(_tmp2FE,param_rgn,_tmp2FD,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp49E;struct _tuple0*_tmp49D;struct Cyc_Absyn_Vardecl*_tmp49C;struct Cyc_Absyn_Vardecl*_tmp301=(_tmp49C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp49C[0]=((_tmp49E.sc=Cyc_Absyn_Public,((_tmp49E.name=(
(_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D->f1=Cyc_Absyn_Loc_n,((_tmp49D->f2=_tmp2FC,_tmp49D)))))),((_tmp49E.tq=
Cyc_Absyn_empty_tqual(0),((_tmp49E.type=_tmp300,((_tmp49E.initializer=0,((_tmp49E.rgn=param_rgn,((_tmp49E.attributes=0,((_tmp49E.escapes=0,_tmp49E)))))))))))))))),_tmp49C)));
# 1276
{struct Cyc_List_List*_tmp49F;_tmp2EE=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F->hd=_tmp301,((_tmp49F->tl=_tmp2EE,_tmp49F))))));}{
struct _dyneither_ptr*_tmp303=_tmp2FC;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4A2;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4A1;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp304=(_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1[0]=((_tmp4A2.tag=3,((_tmp4A2.f1=_tmp301,_tmp4A2)))),_tmp4A1)));
struct Cyc_Tcenv_Bindings*_tmp4A3;_tmp2E6=(const struct Cyc_Tcenv_Bindings*)((_tmp4A3=_region_malloc(_tmp2E5,sizeof(*_tmp4A3)),((_tmp4A3->v=_tmp303,((_tmp4A3->b=(void*)_tmp304,((_tmp4A3->tl=_tmp2E6,_tmp4A3))))))));};}else{
# 1281
const char*_tmp4A6;void*_tmp4A5;(_tmp4A5=0,Cyc_Tcutil_terr(loc,((_tmp4A6="missing name for varargs",_tag_dyneither(_tmp4A6,sizeof(char),25))),_tag_dyneither(_tmp4A5,sizeof(void*),0)));}}
# 1283
{struct Cyc_Core_Opt _tmp4A9;struct Cyc_Core_Opt*_tmp4A8;fd->param_vardecls=((_tmp4A8=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4A8[0]=((_tmp4A9.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2EE),_tmp4A9)),_tmp4A8))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C7;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4C5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C4;struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4C2;struct Cyc_Tcenv_CtrlEnv*_tmp4C1;struct Cyc_Tcenv_SharedFenv*_tmp4C0;struct Cyc_Tcenv_Fenv*_tmp4BF;return(_tmp4BF=_region_malloc(_tmp2E5,sizeof(*_tmp4BF)),((_tmp4BF->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4C0=_region_malloc(_tmp2E5,sizeof(*_tmp4C0)),((_tmp4C0->frgn=_tmp2E5,((_tmp4C0->return_typ=fd->ret_type,((_tmp4C0->seen_labels=
# 1288
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2E5,Cyc_strptrcmp),((_tmp4C0->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp2E5,Cyc_strptrcmp),((_tmp4C0->delayed_effect_checks=0,((_tmp4C0->delayed_constraint_checks=0,_tmp4C0)))))))))))))),((_tmp4BF->type_vars=(struct Cyc_List_List*)_tmp2ED,((_tmp4BF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2E1,((_tmp4BF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2E6,((_tmp4BF->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4BF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1297
(_tmp4C1=_region_malloc(_tmp2E5,sizeof(*_tmp4C1)),((_tmp4C1->ctrl_rgn=_tmp2E5,((_tmp4C1->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4C1->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4C1->fallthru_clause=0,((_tmp4C1->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4C1->try_depth=0,_tmp4C1)))))))))))))),((_tmp4BF->capability=(void*)((void*)(
(_tmp4C7=_cycalloc(sizeof(*_tmp4C7)),((_tmp4C7[0]=((_tmp4C2.tag=24,((_tmp4C2.f1=((_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6->hd=(void*)((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=((_tmp4C5.tag=23,((_tmp4C5.f1=param_rgn,_tmp4C5)))),_tmp4C4)))),((_tmp4C6->tl=(
(_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->hd=(void*)_check_null(fd->effect),((_tmp4C3->tl=0,_tmp4C3)))))),_tmp4C6)))))),_tmp4C2)))),_tmp4C7))))),((_tmp4BF->curr_rgn=(void*)param_rgn,((_tmp4BF->in_notreadctxt=(int)0,((_tmp4BF->in_lhs=(int)0,((_tmp4BF->fnrgn=(struct _RegionHandle*)_tmp2E4,(struct Cyc_Tcenv_Fenv*)_tmp4BF)))))))))))))))))))))));};};};};}
