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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 751 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 910
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 919
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 933
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 953
extern void*Cyc_Absyn_empty_effect;
# 955
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 988
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
# 227
extern int Cyc_Tcenv_warn_override;
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
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 242 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 87 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 90
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 92
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 95
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 147
struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val={0};
struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val={1};
struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val={2};
# 152
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;int try_depth;};
# 166
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 194 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 229 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 232
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 236
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 241
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp3A9;return(_tmp3A9=_region_malloc(r,sizeof(*_tmp3A9)),((_tmp3A9->grgn=r,((_tmp3A9->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp3A9->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3A9->availables=0,_tmp3A9)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 252
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 254
{struct Cyc_Core_Opt*_tmp3AA;Cyc_Tcutil_empty_var_set=((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3AA))));}{
# 256
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp3AB;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB[0]=Cyc_Absyn_exn_tud,_tmp3AB)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3B1;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3B0;struct _tuple12*_tmp3AF;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp3AF=_region_malloc(r,sizeof(*_tmp3AF)),((_tmp3AF->f1=(void*)(
(_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1[0]=((_tmp3B0.tag=2,((_tmp3B0.f1=Cyc_Absyn_exn_tud,((_tmp3B0.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp3B0)))))),_tmp3B1)))),((_tmp3AF->f2=1,_tmp3AF)))))));}}{
# 263
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp3B2;return(_tmp3B2=_region_malloc(r,sizeof(*_tmp3B2)),((_tmp3B2->ns=0,((_tmp3B2->ae=ae,((_tmp3B2->le=0,((_tmp3B2->allow_valueof=0,_tmp3B2)))))))));};};}
# 269
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 271
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 275
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp3B3;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3B3="outer_namespace",_tag_dyneither(_tmp3B3,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 281
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 288
if(n2 != 0)return 0;
return 1;}
# 294
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 299
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){
lookup(ge2,v,0);{
const char*_tmp3B7;void*_tmp3B6[1];struct Cyc_String_pa_PrintArg_struct _tmp3B5;(_tmp3B5.tag=0,((_tmp3B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3B6[0]=& _tmp3B5,Cyc_Tcutil_terr(loc,((_tmp3B7="%s is ambiguous",_tag_dyneither(_tmp3B7,sizeof(char),16))),_tag_dyneither(_tmp3B6,sizeof(void*),1)))))));};
# 303
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp16=_tmp10;void*_tmp18;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp17=(struct Cyc_Dict_Absent_exn_struct*)_tmp16;if(_tmp17->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 305
 goto _LL0;_LL3: _tmp18=_tmp16;_LL4:(void)_throw(_tmp18);_LL0:;}};}}
# 307
return;}
# 310
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 315
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1C=result;_npop_handler(0);return _tmp1C;};}
# 316
;_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1E=_tmp1A;void*_tmp20;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp1F=(struct Cyc_Dict_Absent_exn_struct*)_tmp1E;if(_tmp1F->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 319
 goto _LL5;_LL8: _tmp20=_tmp1E;_LL9:(void)_throw(_tmp20);_LL5:;}};}
# 321
{struct Cyc_List_List*_tmp21=ge->availables;for(0;_tmp21 != 0;_tmp21=_tmp21->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp21->hd);
struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp21->hd,_tmp21->tl);{
void*_tmp25=result;_npop_handler(0);return _tmp25;};}
# 324
;_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp27=_tmp23;void*_tmp29;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp28=(struct Cyc_Dict_Absent_exn_struct*)_tmp27;if(_tmp28->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 327
 goto _LLA;_LLD: _tmp29=_tmp27;_LLE:(void)_throw(_tmp29);_LLA:;}};}}
# 329
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3BA;struct Cyc_Dict_Absent_exn_struct*_tmp3B9;(int)_throw((void*)((_tmp3B9=_cycalloc_atomic(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3BA.tag=Cyc_Dict_Absent,_tmp3BA)),_tmp3B9)))));}
# 332
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 336
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2C,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 343
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp2E;struct _dyneither_ptr*_tmp2F;struct _tuple0*_tmp2D=q;_tmp2E=_tmp2D->f1;_tmp2F=_tmp2D->f2;{
union Cyc_Absyn_Nmspace _tmp30=_tmp2E;struct _dyneither_ptr*_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp34;_LL10: if((_tmp30.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp30.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL14;_LL13:
# 348
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp2F,is_use);_LL14: if((_tmp30.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp30.Rel_n).val == 0)goto _LL16;_tmp31=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp30.Rel_n).val)->hd;_tmp32=((struct Cyc_List_List*)(_tmp30.Rel_n).val)->tl;_LL15: {
# 350
struct Cyc_Tcenv_Genv*_tmp35=Cyc_Tcenv_lookup_namespace(te,loc,_tmp31,_tmp32);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp35,_tmp2F,is_use);}_LL16: if((_tmp30.C_n).tag != 3)goto _LL18;_tmp33=(struct Cyc_List_List*)(_tmp30.C_n).val;_LL17:
 _tmp34=_tmp33;goto _LL19;_LL18: if((_tmp30.Abs_n).tag != 2)goto _LLF;_tmp34=(struct Cyc_List_List*)(_tmp30.Abs_n).val;_LL19:
# 354
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp34),_tmp2F,is_use);_LLF:;};}
# 359
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 361
struct Cyc_List_List*_tmp36=te->ns;
struct _RegionHandle _tmp37=_new_region("temp");struct _RegionHandle*temp=& _tmp37;_push_region(temp);
{struct Cyc_List_List*_tmp38=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp39=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp36);
struct Cyc_List_List*_tmp3A=_tmp39->availables;
struct Cyc_Set_Set*_tmp3B=_tmp39->namespaces;
# 369
{struct Cyc_List_List*_tmp3C=_tmp3A;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
struct Cyc_Set_Set*_tmp3D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3C->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3D,n)){
struct Cyc_List_List*_tmp3BD;struct Cyc_List_List*_tmp3BC;_tmp38=((_tmp3BC=_region_malloc(temp,sizeof(*_tmp3BC)),((_tmp3BC->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD->hd=n,((_tmp3BD->tl=ns,_tmp3BD))))))),((_tmp3BC->tl=_tmp38,_tmp3BC))))));}}}
# 376
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp3C0;struct Cyc_List_List*_tmp3BF;_tmp38=((_tmp3BF=_region_malloc(temp,sizeof(*_tmp3BF)),((_tmp3BF->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0->hd=n,((_tmp3C0->tl=ns,_tmp3C0))))))),((_tmp3BF->tl=_tmp38,_tmp3BF))))));}
# 382
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp3C4;void*_tmp3C3[1];struct Cyc_String_pa_PrintArg_struct _tmp3C2;(_tmp3C2.tag=0,((_tmp3C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3C3[0]=& _tmp3C2,Cyc_Tcutil_terr(loc,((_tmp3C4="%s is ambiguous",_tag_dyneither(_tmp3C4,sizeof(char),16))),_tag_dyneither(_tmp3C3,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 387
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3C7;struct Cyc_Dict_Absent_exn_struct*_tmp3C6;(int)_throw((void*)((_tmp3C6=_cycalloc_atomic(sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C7.tag=Cyc_Dict_Absent,_tmp3C7)),_tmp3C6)))));}
# 390
_tmp36=Cyc_Tcenv_outer_namespace(_tmp36);}}
# 363
;_pop_region(temp);}
# 398
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp48=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,v);}
# 402
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4A;struct _dyneither_ptr*_tmp4B;struct _tuple0*_tmp49=q;_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;{
union Cyc_Absyn_Nmspace _tmp4C=_tmp4A;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;struct _dyneither_ptr*_tmp4F;struct Cyc_List_List*_tmp50;_LL1B: if((_tmp4C.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4C.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val != 0)goto _LL1F;_LL1E:
# 407
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4B,0);_LL1F: if((_tmp4C.C_n).tag != 3)goto _LL21;_tmp4D=(struct Cyc_List_List*)(_tmp4C.C_n).val;_LL20:
 _tmp4E=_tmp4D;goto _LL22;_LL21: if((_tmp4C.Abs_n).tag != 2)goto _LL23;_tmp4E=(struct Cyc_List_List*)(_tmp4C.Abs_n).val;_LL22: {
# 410
struct Cyc_Dict_Dict _tmp51=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4E))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp51,_tmp4B);}_LL23: if((_tmp4C.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val == 0)goto _LL1A;_tmp4F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->hd;_tmp50=((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->tl;_LL24: {
# 413
struct Cyc_Dict_Dict _tmp52=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp4F,_tmp50))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp52,_tmp4B);}_LL1A:;};}
# 418
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp53=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,v);}
# 422
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp55;struct _dyneither_ptr*_tmp56;struct _tuple0*_tmp54=q;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
union Cyc_Absyn_Nmspace _tmp57=_tmp55;struct _dyneither_ptr*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp5B;_LL26: if((_tmp57.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp57.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp57.Rel_n).val != 0)goto _LL2A;_LL29:
# 427
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp56,0);_LL2A: if((_tmp57.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp57.Rel_n).val == 0)goto _LL2C;_tmp58=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp57.Rel_n).val)->hd;_tmp59=((struct Cyc_List_List*)(_tmp57.Rel_n).val)->tl;_LL2B: {
# 429
struct Cyc_Dict_Dict _tmp5C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp58,_tmp59))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5C,_tmp56);}_LL2C: if((_tmp57.C_n).tag != 3)goto _LL2E;_tmp5A=(struct Cyc_List_List*)(_tmp57.C_n).val;_LL2D:
 _tmp5B=_tmp5A;goto _LL2F;_LL2E: if((_tmp57.Abs_n).tag != 2)goto _LL25;_tmp5B=(struct Cyc_List_List*)(_tmp57.Abs_n).val;_LL2F: {
# 433
struct Cyc_Dict_Dict _tmp5D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5B))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5D,_tmp56);}_LL25:;};}
# 438
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 443
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp60;struct _tuple0*_tmp5E=q;_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;{
union Cyc_Absyn_Nmspace _tmp61=_tmp5F;struct _dyneither_ptr*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp65;_LL31: if((_tmp61.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp61.Rel_n).val != 0)goto _LL33;_LL32: {
# 448
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp3C8;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp3C8)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_throw(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 451
{const char*_tmp3CB;void*_tmp3CA;(_tmp3CA=0,Cyc_Tcutil_terr(loc,((_tmp3CB="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp3CB,sizeof(char),33))),_tag_dyneither(_tmp3CA,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 458
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 462
{const char*_tmp3CE;void*_tmp3CD;(_tmp3CD=0,Cyc_Tcutil_terr(loc,((_tmp3CE="bad qualified name for @extensible datatype",_tag_dyneither(_tmp3CE,sizeof(char),44))),_tag_dyneither(_tmp3CD,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp3D1;struct Cyc_Dict_Absent_exn_struct*_tmp3D0;(int)_throw((void*)((_tmp3D0=_cycalloc_atomic(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D1.tag=Cyc_Dict_Absent,_tmp3D1)),_tmp3D0)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_throw(_tmp77);_LL40:;}};}{
# 465
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3D2;return(_tmp3D2=_region_malloc(r,sizeof(*_tmp3D2)),((_tmp3D2[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp3D2)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 469
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3D3;return(_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp3D3)));}_LL30:;};}
# 474
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp80=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,v);}
# 478
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct _tuple0*_tmp81=q;_tmp82=_tmp81->f1;_tmp83=_tmp81->f2;{
union Cyc_Absyn_Nmspace _tmp84=_tmp82;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp88;_LL46: if((_tmp84.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp84.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp84.Rel_n).val != 0)goto _LL4A;_LL49:
# 483
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp83,0);_LL4A: if((_tmp84.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp84.Rel_n).val == 0)goto _LL4C;_tmp85=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp84.Rel_n).val)->hd;_tmp86=((struct Cyc_List_List*)(_tmp84.Rel_n).val)->tl;_LL4B: {
# 485
struct Cyc_Dict_Dict _tmp89=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp89,_tmp83);}_LL4C: if((_tmp84.C_n).tag != 3)goto _LL4E;_tmp87=(struct Cyc_List_List*)(_tmp84.C_n).val;_LL4D:
 _tmp88=_tmp87;goto _LL4F;_LL4E: if((_tmp84.Abs_n).tag != 2)goto _LL45;_tmp88=(struct Cyc_List_List*)(_tmp84.Abs_n).val;_LL4F: {
# 489
struct Cyc_Dict_Dict _tmp8A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp88))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp83);}_LL45:;};}
# 494
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8B=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,v);}
# 498
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp8D;struct _dyneither_ptr*_tmp8E;struct _tuple0*_tmp8C=q;_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;{
union Cyc_Absyn_Nmspace _tmp8F=_tmp8D;struct _dyneither_ptr*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;_LL51: if((_tmp8F.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp8F.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val != 0)goto _LL55;_LL54:
# 503
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8E,0);_LL55: if((_tmp8F.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val == 0)goto _LL57;_tmp90=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->hd;_tmp91=((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->tl;_LL56: {
# 505
struct Cyc_Dict_Dict _tmp94=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp90,_tmp91))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8E);}_LL57: if((_tmp8F.C_n).tag != 3)goto _LL59;_tmp92=(struct Cyc_List_List*)(_tmp8F.C_n).val;_LL58:
 _tmp93=_tmp92;goto _LL5A;_LL59: if((_tmp8F.Abs_n).tag != 2)goto _LL50;_tmp93=(struct Cyc_List_List*)(_tmp8F.Abs_n).val;_LL5A: {
# 509
struct Cyc_Dict_Dict _tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8E);}_LL50:;};}
# 516
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp96=te->le;
if(_tmp96 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp96;}
# 522
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 526
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp97=te->le;
if(_tmp97 != 0){
struct Cyc_Tcenv_Fenv*_tmp98=_tmp97;struct _RegionHandle*_tmp9A;struct Cyc_Tcenv_Fenv*_tmp99=_tmp98;_tmp9A=_tmp99->fnrgn;
return _tmp9A;}
# 532
return Cyc_Tcenv_coerce_heap_region();}
# 535
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp3D4;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D4="put_fenv",_tag_dyneither(_tmp3D4,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp3D5;return(_tmp3D5=_region_malloc(l,sizeof(*_tmp3D5)),((_tmp3D5->ns=te->ns,((_tmp3D5->ae=te->ae,((_tmp3D5->le=fe,((_tmp3D5->allow_valueof=te->allow_valueof,_tmp3D5)))))))));};}
# 540
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp3D6;return(_tmp3D6=_region_malloc(l,sizeof(*_tmp3D6)),((_tmp3D6->ns=te->ns,((_tmp3D6->ae=te->ae,((_tmp3D6->le=0,((_tmp3D6->allow_valueof=te->allow_valueof,_tmp3D6)))))))));}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_Fenv _tmp9E=*f;struct Cyc_Tcenv_SharedFenv*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_RgnOrder_RgnPO*_tmpA2;const struct Cyc_Tcenv_Bindings*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Tcenv_CtrlEnv*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_Tcenv_FenvFlags _tmpA8;struct _RegionHandle*_tmpA9;struct Cyc_Tcenv_Fenv _tmp9F=_tmp9E;_tmpA0=_tmp9F.shared;_tmpA1=_tmp9F.type_vars;_tmpA2=_tmp9F.region_order;_tmpA3=_tmp9F.locals;_tmpA4=_tmp9F.encloser;_tmpA5=_tmp9F.ctrl_env;_tmpA6=_tmp9F.capability;_tmpA7=_tmp9F.curr_rgn;_tmpA8=_tmp9F.flags;_tmpA9=_tmp9F.fnrgn;{
# 548
struct Cyc_Tcenv_Fenv*_tmp3D7;return(_tmp3D7=_region_malloc(l,sizeof(*_tmp3D7)),((_tmp3D7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA0,((_tmp3D7->type_vars=(struct Cyc_List_List*)_tmpA1,((_tmp3D7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA2,((_tmp3D7->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA3,((_tmp3D7->encloser=(struct Cyc_Absyn_Stmt*)_tmpA4,((_tmp3D7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA5,((_tmp3D7->capability=(void*)_tmpA6,((_tmp3D7->curr_rgn=(void*)_tmpA7,((_tmp3D7->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA8,((_tmp3D7->fnrgn=(struct _RegionHandle*)l,_tmp3D7)))))))))))))))))))));};}
# 552
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 554
struct Cyc_Tcenv_Fenv _tmpAB=*f;struct Cyc_Tcenv_SharedFenv*_tmpAD;struct Cyc_List_List*_tmpAE;struct Cyc_RgnOrder_RgnPO*_tmpAF;const struct Cyc_Tcenv_Bindings*_tmpB0;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_Tcenv_CtrlEnv*_tmpB2;void*_tmpB3;void*_tmpB4;struct Cyc_Tcenv_FenvFlags _tmpB5;struct Cyc_Tcenv_Fenv _tmpAC=_tmpAB;_tmpAD=_tmpAC.shared;_tmpAE=_tmpAC.type_vars;_tmpAF=_tmpAC.region_order;_tmpB0=_tmpAC.locals;_tmpB1=_tmpAC.encloser;_tmpB2=_tmpAC.ctrl_env;_tmpB3=_tmpAC.capability;_tmpB4=_tmpAC.curr_rgn;_tmpB5=_tmpAC.flags;{
# 557
struct Cyc_Tcenv_CtrlEnv _tmpB6=*_tmpB2;struct _RegionHandle*_tmpB8;void*_tmpB9;void*_tmpBA;const struct _tuple10*_tmpBB;void*_tmpBC;int _tmpBD;struct Cyc_Tcenv_CtrlEnv _tmpB7=_tmpB6;_tmpB8=_tmpB7.ctrl_rgn;_tmpB9=_tmpB7.continue_stmt;_tmpBA=_tmpB7.break_stmt;_tmpBB=_tmpB7.fallthru_clause;_tmpBC=_tmpB7.next_stmt;_tmpBD=_tmpB7.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp3D8;struct Cyc_Tcenv_CtrlEnv*_tmpBE=
(_tmp3D8=_region_malloc(l,sizeof(*_tmp3D8)),((_tmp3D8->ctrl_rgn=_tmpB8,((_tmp3D8->continue_stmt=_tmpB9,((_tmp3D8->break_stmt=_tmpBA,((_tmp3D8->fallthru_clause=_tmpBB,((_tmp3D8->next_stmt=_tmpBC,((_tmp3D8->try_depth=_tmpBD,_tmp3D8)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp3D9;return(_tmp3D9=_region_malloc(l,sizeof(*_tmp3D9)),((_tmp3D9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAD,((_tmp3D9->type_vars=(struct Cyc_List_List*)_tmpAE,((_tmp3D9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAF,((_tmp3D9->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB0,((_tmp3D9->encloser=(struct Cyc_Absyn_Stmt*)_tmpB1,((_tmp3D9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBE,((_tmp3D9->capability=(void*)_tmpB3,((_tmp3D9->curr_rgn=(void*)_tmpB4,((_tmp3D9->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB5,((_tmp3D9->fnrgn=(struct _RegionHandle*)l,_tmp3D9)))))))))))))))))))));};};}
# 566
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3DA;struct Cyc_Tcenv_Fenv*_tmpC1=Cyc_Tcenv_get_fenv(te,((_tmp3DA="return_typ",_tag_dyneither(_tmp3DA,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpC3;struct Cyc_Tcenv_Fenv*_tmpC2=_tmpC1;_tmpC3=_tmpC2->shared;{
struct Cyc_Tcenv_SharedFenv _tmpC4=*_tmpC3;void*_tmpC6;struct Cyc_Tcenv_SharedFenv _tmpC5=_tmpC4;_tmpC6=_tmpC5.return_typ;
return _tmpC6;};}
# 572
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC8=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpC9=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC8));struct Cyc_List_List*_tmpCB;struct Cyc_Tcenv_Fenv _tmpCA=_tmpC9;_tmpCB=_tmpCA.type_vars;
return _tmpCB;};}
# 579
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp3DB;return(_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3DB)));}
# 583
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 585
const char*_tmp3DC;struct Cyc_Tcenv_Fenv*_tmpCD=
# 587
Cyc_Tcenv_get_fenv(te,((_tmp3DC="add_type_vars",_tag_dyneither(_tmp3DC,sizeof(char),14))));
# 585
struct Cyc_Tcenv_SharedFenv*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_RgnOrder_RgnPO*_tmpD1;const struct Cyc_Tcenv_Bindings*_tmpD2;struct Cyc_Absyn_Stmt*_tmpD3;struct Cyc_Tcenv_CtrlEnv*_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_Tcenv_FenvFlags _tmpD7;struct Cyc_Tcenv_Fenv*_tmpCE=_tmpCD;_tmpCF=_tmpCE->shared;_tmpD0=_tmpCE->type_vars;_tmpD1=_tmpCE->region_order;_tmpD2=_tmpCE->locals;_tmpD3=_tmpCE->encloser;_tmpD4=_tmpCE->ctrl_env;_tmpD5=_tmpCE->capability;_tmpD6=_tmpCE->curr_rgn;_tmpD7=_tmpCE->flags;
# 589
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD0);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD8);{
struct Cyc_Tcenv_Fenv*_tmp3DD;struct Cyc_Tcenv_Fenv*_tmpD9=(_tmp3DD=_region_malloc(r,sizeof(*_tmp3DD)),((_tmp3DD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCF,((_tmp3DD->type_vars=(struct Cyc_List_List*)_tmpD8,((_tmp3DD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD1,((_tmp3DD->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD2,((_tmp3DD->encloser=(struct Cyc_Absyn_Stmt*)_tmpD3,((_tmp3DD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD4,((_tmp3DD->capability=(void*)_tmpD5,((_tmp3DD->curr_rgn=(void*)_tmpD6,((_tmp3DD->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD7,((_tmp3DD->fnrgn=(struct _RegionHandle*)r,_tmp3DD)))))))))))))))))))));
# 594
return Cyc_Tcenv_put_fenv(r,te,_tmpD9);};};}
# 597
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDC=te->le;
if(_tmpDC == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 602
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpDC);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 609
struct _dyneither_ptr*_tmpDD=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpDE=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpDF=_tmpDE;_LL5C: if((_tmpDF.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 614
struct Cyc_Core_Impossible_exn_struct _tmp3E3;const char*_tmp3E2;struct Cyc_Core_Impossible_exn_struct*_tmp3E1;(int)_throw((void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E3.tag=Cyc_Core_Impossible,((_tmp3E3.f1=((_tmp3E2="add_local_var: called with Rel_n",_tag_dyneither(_tmp3E2,sizeof(char),33))),_tmp3E3)))),_tmp3E1)))));}_LL5B:;}
# 616
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpE3;_push_handler(& _tmpE3);{int _tmpE5=0;if(setjmp(_tmpE3.handler))_tmpE5=1;if(!_tmpE5){
{void*_tmpE6=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpE7=_tmpE6;_LL61: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpE8->tag != 0)goto _LL63;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpE8->f1);if(_tmpE9->tag != 4)goto _LL63;}}_LL62:
# 620
{const char*_tmp3E6;void*_tmp3E5;(_tmp3E5=0,Cyc_Tcutil_warn(loc,((_tmp3E6="declaration hides local",_tag_dyneither(_tmp3E6,sizeof(char),24))),_tag_dyneither(_tmp3E5,sizeof(void*),0)));}goto _LL60;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEA=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEA->tag != 0)goto _LL65;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpEB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpEA->f1);if(_tmpEB->tag != 1)goto _LL65;}}_LL64:
{const char*_tmp3E9;void*_tmp3E8;(_tmp3E8=0,Cyc_Tcutil_warn(loc,((_tmp3E9="declaration hides global",_tag_dyneither(_tmp3E9,sizeof(char),25))),_tag_dyneither(_tmp3E8,sizeof(void*),0)));}goto _LL60;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEC->tag != 0)goto _LL67;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpED=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpEC->f1);if(_tmpED->tag != 5)goto _LL67;}}_LL66:
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_warn(loc,((_tmp3EC="declaration hides pattern variable",_tag_dyneither(_tmp3EC,sizeof(char),35))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}goto _LL60;_LL67: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEE->tag != 0)goto _LL69;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpEF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEE->f1);if(_tmpEF->tag != 3)goto _LL69;}}_LL68:
{const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_Tcutil_warn(loc,((_tmp3EF="declaration hides parameter",_tag_dyneither(_tmp3EF,sizeof(char),28))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}goto _LL60;_LL69:;_LL6A:
 goto _LL60;_LL60:;}
# 618
;_pop_handler();}else{void*_tmpE4=(void*)_exn_thrown;void*_tmpF9=_tmpE4;void*_tmpFB;_LL6C: {struct Cyc_Dict_Absent_exn_struct*_tmpFA=(struct Cyc_Dict_Absent_exn_struct*)_tmpF9;if(_tmpFA->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D:
# 626
 goto _LL6B;_LL6E: _tmpFB=_tmpF9;_LL6F:(void)_throw(_tmpFB);_LL6B:;}};}{
const char*_tmp3F0;struct Cyc_Tcenv_Fenv*_tmpFC=
# 629
Cyc_Tcenv_get_fenv(te,((_tmp3F0="add_local_var",_tag_dyneither(_tmp3F0,sizeof(char),14))));
# 627
struct Cyc_Tcenv_SharedFenv*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_RgnOrder_RgnPO*_tmp100;const struct Cyc_Tcenv_Bindings*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Tcenv_CtrlEnv*_tmp103;void*_tmp104;void*_tmp105;struct Cyc_Tcenv_FenvFlags _tmp106;struct Cyc_Tcenv_Fenv*_tmpFD=_tmpFC;_tmpFE=_tmpFD->shared;_tmpFF=_tmpFD->type_vars;_tmp100=_tmpFD->region_order;_tmp101=_tmpFD->locals;_tmp102=_tmpFD->encloser;_tmp103=_tmpFD->ctrl_env;_tmp104=_tmpFD->capability;_tmp105=_tmpFD->curr_rgn;_tmp106=_tmpFD->flags;{
# 630
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp3F3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3F2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp107=(_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=((_tmp3F3.tag=4,((_tmp3F3.f1=vd,_tmp3F3)))),_tmp3F2)));
struct Cyc_Tcenv_Bindings*_tmp3F4;struct Cyc_Tcenv_Bindings*_tmp108=(_tmp3F4=_region_malloc(r,sizeof(*_tmp3F4)),((_tmp3F4->v=_tmpDD,((_tmp3F4->b=(void*)_tmp107,((_tmp3F4->tl=_tmp101,_tmp3F4)))))));
struct Cyc_Tcenv_Fenv*_tmp3F5;struct Cyc_Tcenv_Fenv*_tmp109=(_tmp3F5=_region_malloc(r,sizeof(*_tmp3F5)),((_tmp3F5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFE,((_tmp3F5->type_vars=(struct Cyc_List_List*)_tmpFF,((_tmp3F5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp100,((_tmp3F5->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp108),((_tmp3F5->encloser=(struct Cyc_Absyn_Stmt*)_tmp102,((_tmp3F5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp103,((_tmp3F5->capability=(void*)_tmp104,((_tmp3F5->curr_rgn=(void*)_tmp105,((_tmp3F5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp106,((_tmp3F5->fnrgn=(struct _RegionHandle*)r,_tmp3F5)))))))))))))))))))));
# 634
return Cyc_Tcenv_put_fenv(r,te,_tmp109);};};}
# 637
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 639
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp110=*_tmp10F;struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp114;const struct Cyc_Tcenv_Bindings*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp118;void*_tmp119;int _tmp11A;int _tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp111=_tmp110;_tmp112=_tmp111.shared;_tmp113=_tmp111.type_vars;_tmp114=_tmp111.region_order;_tmp115=_tmp111.locals;_tmp116=_tmp111.encloser;_tmp117=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp119=_tmp111.curr_rgn;_tmp11A=(_tmp111.flags).in_notreadctxt;_tmp11B=(_tmp111.flags).in_lhs;_tmp11C=(_tmp111.flags).abstract_ok;{
# 644
struct Cyc_Tcenv_Fenv*_tmp3F6;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp3F6=_region_malloc(r,sizeof(*_tmp3F6)),((_tmp3F6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112,((_tmp3F6->type_vars=(struct Cyc_List_List*)_tmp113,((_tmp3F6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp114,((_tmp3F6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp3F6->encloser=(struct Cyc_Absyn_Stmt*)_tmp116,((_tmp3F6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117,((_tmp3F6->capability=(void*)_tmp118,((_tmp3F6->curr_rgn=(void*)_tmp119,((_tmp3F6->flags=(struct Cyc_Tcenv_FenvFlags)(
# 646
((_tmp3F6->flags).in_notreadctxt=_tmp11A,(((_tmp3F6->flags).in_lhs=_tmp11B,(((_tmp3F6->flags).in_new=(int)status,(((_tmp3F6->flags).abstract_ok=_tmp11C,_tmp3F6->flags)))))))),((_tmp3F6->fnrgn=(struct _RegionHandle*)r,_tmp3F6)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}
# 650
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11F=te->le;
if(_tmp11F == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp120=_tmp11F;int _tmp122;struct Cyc_Tcenv_Fenv*_tmp121=_tmp120;_tmp122=(_tmp121->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp122;};}
# 657
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp123=te->le;
if(_tmp123 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp124=_tmp123;int _tmp126;struct Cyc_Tcenv_Fenv*_tmp125=_tmp124;_tmp126=(_tmp125->flags).abstract_ok;
return _tmp126;};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp127=te->le;
if(_tmp127 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp128=*_tmp127;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_RgnOrder_RgnPO*_tmp12C;const struct Cyc_Tcenv_Bindings*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Tcenv_CtrlEnv*_tmp12F;void*_tmp130;void*_tmp131;int _tmp132;int _tmp133;int _tmp134;struct Cyc_Tcenv_Fenv _tmp129=_tmp128;_tmp12A=_tmp129.shared;_tmp12B=_tmp129.type_vars;_tmp12C=_tmp129.region_order;_tmp12D=_tmp129.locals;_tmp12E=_tmp129.encloser;_tmp12F=_tmp129.ctrl_env;_tmp130=_tmp129.capability;_tmp131=_tmp129.curr_rgn;_tmp132=(_tmp129.flags).in_notreadctxt;_tmp133=(_tmp129.flags).in_lhs;_tmp134=(_tmp129.flags).in_new;{
# 671
struct Cyc_Tcenv_Fenv*_tmp3F7;struct Cyc_Tcenv_Fenv*_tmp135=(_tmp3F7=_region_malloc(r,sizeof(*_tmp3F7)),((_tmp3F7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A,((_tmp3F7->type_vars=(struct Cyc_List_List*)_tmp12B,((_tmp3F7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12C,((_tmp3F7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12D,((_tmp3F7->encloser=(struct Cyc_Absyn_Stmt*)_tmp12E,((_tmp3F7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12F,((_tmp3F7->capability=(void*)_tmp130,((_tmp3F7->curr_rgn=(void*)_tmp131,((_tmp3F7->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp3F7->flags).in_notreadctxt=_tmp132,(((_tmp3F7->flags).in_lhs=_tmp133,(((_tmp3F7->flags).in_new=_tmp134,(((_tmp3F7->flags).abstract_ok=1,_tmp3F7->flags)))))))),((_tmp3F7->fnrgn=(struct _RegionHandle*)r,_tmp3F7)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 679
struct Cyc_Tcenv_Fenv*_tmp137=te->le;
if(_tmp137 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp138=*_tmp137;struct Cyc_Tcenv_SharedFenv*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_RgnOrder_RgnPO*_tmp13C;const struct Cyc_Tcenv_Bindings*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13F;void*_tmp140;void*_tmp141;int _tmp142;int _tmp143;int _tmp144;struct Cyc_Tcenv_Fenv _tmp139=_tmp138;_tmp13A=_tmp139.shared;_tmp13B=_tmp139.type_vars;_tmp13C=_tmp139.region_order;_tmp13D=_tmp139.locals;_tmp13E=_tmp139.encloser;_tmp13F=_tmp139.ctrl_env;_tmp140=_tmp139.capability;_tmp141=_tmp139.curr_rgn;_tmp142=(_tmp139.flags).in_notreadctxt;_tmp143=(_tmp139.flags).in_lhs;_tmp144=(_tmp139.flags).in_new;{
# 684
struct Cyc_Tcenv_Fenv*_tmp3F8;struct Cyc_Tcenv_Fenv*_tmp145=(_tmp3F8=_region_malloc(r,sizeof(*_tmp3F8)),((_tmp3F8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp13A,((_tmp3F8->type_vars=(struct Cyc_List_List*)_tmp13B,((_tmp3F8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13C,((_tmp3F8->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13D,((_tmp3F8->encloser=(struct Cyc_Absyn_Stmt*)_tmp13E,((_tmp3F8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13F,((_tmp3F8->capability=(void*)_tmp140,((_tmp3F8->curr_rgn=(void*)_tmp141,((_tmp3F8->flags=(struct Cyc_Tcenv_FenvFlags)(
# 686
((_tmp3F8->flags).in_notreadctxt=_tmp142,(((_tmp3F8->flags).in_lhs=_tmp143,(((_tmp3F8->flags).in_new=_tmp144,(((_tmp3F8->flags).abstract_ok=0,_tmp3F8->flags)))))))),((_tmp3F8->fnrgn=(struct _RegionHandle*)r,_tmp3F8)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp145);};};}
# 690
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp147=te->le;
if(_tmp147 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp148=*_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp14A;struct Cyc_List_List*_tmp14B;struct Cyc_RgnOrder_RgnPO*_tmp14C;const struct Cyc_Tcenv_Bindings*_tmp14D;struct Cyc_Absyn_Stmt*_tmp14E;struct Cyc_Tcenv_CtrlEnv*_tmp14F;void*_tmp150;void*_tmp151;int _tmp152;int _tmp153;int _tmp154;struct Cyc_Tcenv_Fenv _tmp149=_tmp148;_tmp14A=_tmp149.shared;_tmp14B=_tmp149.type_vars;_tmp14C=_tmp149.region_order;_tmp14D=_tmp149.locals;_tmp14E=_tmp149.encloser;_tmp14F=_tmp149.ctrl_env;_tmp150=_tmp149.capability;_tmp151=_tmp149.curr_rgn;_tmp152=(_tmp149.flags).in_lhs;_tmp153=(_tmp149.flags).in_new;_tmp154=(_tmp149.flags).abstract_ok;{
# 696
struct Cyc_Tcenv_Fenv*_tmp3F9;struct Cyc_Tcenv_Fenv*_tmp155=(_tmp3F9=_region_malloc(r,sizeof(*_tmp3F9)),((_tmp3F9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp14A,((_tmp3F9->type_vars=(struct Cyc_List_List*)_tmp14B,((_tmp3F9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14C,((_tmp3F9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14D,((_tmp3F9->encloser=(struct Cyc_Absyn_Stmt*)_tmp14E,((_tmp3F9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14F,((_tmp3F9->capability=(void*)_tmp150,((_tmp3F9->curr_rgn=(void*)_tmp151,((_tmp3F9->flags=(struct Cyc_Tcenv_FenvFlags)(
# 698
((_tmp3F9->flags).in_notreadctxt=1,(((_tmp3F9->flags).in_lhs=_tmp152,(((_tmp3F9->flags).in_new=_tmp153,(((_tmp3F9->flags).abstract_ok=_tmp154,_tmp3F9->flags)))))))),((_tmp3F9->fnrgn=(struct _RegionHandle*)r,_tmp3F9)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp155);};};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp157=te->le;
if(_tmp157 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp158=*_tmp157;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;struct Cyc_Tcenv_Fenv _tmp159=_tmp158;_tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_lhs;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;{
# 708
struct Cyc_Tcenv_Fenv*_tmp3FA;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp3FA=_region_malloc(r,sizeof(*_tmp3FA)),((_tmp3FA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp3FA->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp3FA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp3FA->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp3FA->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp3FA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp3FA->capability=(void*)_tmp160,((_tmp3FA->curr_rgn=(void*)_tmp161,((_tmp3FA->flags=(struct Cyc_Tcenv_FenvFlags)(
# 710
((_tmp3FA->flags).in_notreadctxt=0,(((_tmp3FA->flags).in_lhs=_tmp162,(((_tmp3FA->flags).in_new=_tmp163,(((_tmp3FA->flags).abstract_ok=_tmp164,_tmp3FA->flags)))))))),((_tmp3FA->fnrgn=(struct _RegionHandle*)r,_tmp3FA)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 714
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp168=_tmp167;int _tmp16A;struct Cyc_Tcenv_Fenv*_tmp169=_tmp168;_tmp16A=(_tmp169->flags).in_notreadctxt;
return _tmp16A;};}
# 721
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16B=te->le;
if(_tmp16B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp16C=*_tmp16B;struct Cyc_Tcenv_SharedFenv*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_RgnOrder_RgnPO*_tmp170;const struct Cyc_Tcenv_Bindings*_tmp171;struct Cyc_Absyn_Stmt*_tmp172;struct Cyc_Tcenv_CtrlEnv*_tmp173;void*_tmp174;void*_tmp175;int _tmp176;int _tmp177;int _tmp178;struct Cyc_Tcenv_Fenv _tmp16D=_tmp16C;_tmp16E=_tmp16D.shared;_tmp16F=_tmp16D.type_vars;_tmp170=_tmp16D.region_order;_tmp171=_tmp16D.locals;_tmp172=_tmp16D.encloser;_tmp173=_tmp16D.ctrl_env;_tmp174=_tmp16D.capability;_tmp175=_tmp16D.curr_rgn;_tmp176=(_tmp16D.flags).in_notreadctxt;_tmp177=(_tmp16D.flags).in_new;_tmp178=(_tmp16D.flags).abstract_ok;{
# 727
struct Cyc_Tcenv_Fenv*_tmp3FB;struct Cyc_Tcenv_Fenv*_tmp179=(_tmp3FB=_region_malloc(r,sizeof(*_tmp3FB)),((_tmp3FB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16E,((_tmp3FB->type_vars=(struct Cyc_List_List*)_tmp16F,((_tmp3FB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp170,((_tmp3FB->locals=(const struct Cyc_Tcenv_Bindings*)_tmp171,((_tmp3FB->encloser=(struct Cyc_Absyn_Stmt*)_tmp172,((_tmp3FB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp173,((_tmp3FB->capability=(void*)_tmp174,((_tmp3FB->curr_rgn=(void*)_tmp175,((_tmp3FB->flags=(struct Cyc_Tcenv_FenvFlags)(
# 729
((_tmp3FB->flags).in_notreadctxt=_tmp176,(((_tmp3FB->flags).in_lhs=1,(((_tmp3FB->flags).in_new=_tmp177,(((_tmp3FB->flags).abstract_ok=_tmp178,_tmp3FB->flags)))))))),((_tmp3FB->fnrgn=(struct _RegionHandle*)r,_tmp3FB)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp179);};};}
# 733
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17B=te->le;
if(_tmp17B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp17C=*_tmp17B;struct Cyc_Tcenv_SharedFenv*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*_tmp180;const struct Cyc_Tcenv_Bindings*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*_tmp183;void*_tmp184;void*_tmp185;int _tmp186;int _tmp187;int _tmp188;struct Cyc_Tcenv_Fenv _tmp17D=_tmp17C;_tmp17E=_tmp17D.shared;_tmp17F=_tmp17D.type_vars;_tmp180=_tmp17D.region_order;_tmp181=_tmp17D.locals;_tmp182=_tmp17D.encloser;_tmp183=_tmp17D.ctrl_env;_tmp184=_tmp17D.capability;_tmp185=_tmp17D.curr_rgn;_tmp186=(_tmp17D.flags).in_notreadctxt;_tmp187=(_tmp17D.flags).in_new;_tmp188=(_tmp17D.flags).abstract_ok;{
# 739
struct Cyc_Tcenv_Fenv*_tmp3FC;struct Cyc_Tcenv_Fenv*_tmp189=(_tmp3FC=_region_malloc(r,sizeof(*_tmp3FC)),((_tmp3FC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp17E,((_tmp3FC->type_vars=(struct Cyc_List_List*)_tmp17F,((_tmp3FC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp180,((_tmp3FC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp181,((_tmp3FC->encloser=(struct Cyc_Absyn_Stmt*)_tmp182,((_tmp3FC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp183,((_tmp3FC->capability=(void*)_tmp184,((_tmp3FC->curr_rgn=(void*)_tmp185,((_tmp3FC->flags=(struct Cyc_Tcenv_FenvFlags)(
# 741
((_tmp3FC->flags).in_notreadctxt=_tmp186,(((_tmp3FC->flags).in_lhs=0,(((_tmp3FC->flags).in_new=_tmp187,(((_tmp3FC->flags).abstract_ok=_tmp188,_tmp3FC->flags)))))))),((_tmp3FC->fnrgn=(struct _RegionHandle*)r,_tmp3FC)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp189);};};}
# 745
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18B=te->le;
if(_tmp18B == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp18C=_tmp18B;int _tmp18E;struct Cyc_Tcenv_Fenv*_tmp18D=_tmp18C;_tmp18E=(_tmp18D->flags).in_lhs;
return _tmp18E;};}
# 753
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 755
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp18F;_push_handler(& _tmp18F);{int _tmp191=0;if(setjmp(_tmp18F.handler))_tmp191=1;if(!_tmp191){
{void*_tmp192=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp193=_tmp192;_LL71: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp194=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp194->tag != 0)goto _LL73;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp195=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp194->f1);if(_tmp195->tag != 4)goto _LL73;}}_LL72:
# 759
{const char*_tmp3FF;void*_tmp3FE;(_tmp3FE=0,Cyc_Tcutil_warn(loc,((_tmp3FF="pattern variable hides local",_tag_dyneither(_tmp3FF,sizeof(char),29))),_tag_dyneither(_tmp3FE,sizeof(void*),0)));}goto _LL70;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp196=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp196->tag != 0)goto _LL75;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp197=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp196->f1);if(_tmp197->tag != 1)goto _LL75;}}_LL74:
{const char*_tmp402;void*_tmp401;(_tmp401=0,Cyc_Tcutil_warn(loc,((_tmp402="pattern variable hides global",_tag_dyneither(_tmp402,sizeof(char),30))),_tag_dyneither(_tmp401,sizeof(void*),0)));}goto _LL70;_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp198=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp198->tag != 0)goto _LL77;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp198->f1);if(_tmp199->tag != 5)goto _LL77;}}_LL76:
{const char*_tmp405;void*_tmp404;(_tmp404=0,Cyc_Tcutil_warn(loc,((_tmp405="pattern variable hides pattern variable",_tag_dyneither(_tmp405,sizeof(char),40))),_tag_dyneither(_tmp404,sizeof(void*),0)));}goto _LL70;_LL77: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp19A=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp19A->tag != 0)goto _LL79;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp19B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp19A->f1);if(_tmp19B->tag != 3)goto _LL79;}}_LL78:
{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_warn(loc,((_tmp408="pattern variable hides parameter",_tag_dyneither(_tmp408,sizeof(char),33))),_tag_dyneither(_tmp407,sizeof(void*),0)));}goto _LL70;_LL79:;_LL7A:
 goto _LL70;_LL70:;}
# 757
;_pop_handler();}else{void*_tmp190=(void*)_exn_thrown;void*_tmp1A5=_tmp190;void*_tmp1A7;_LL7C: {struct Cyc_Dict_Absent_exn_struct*_tmp1A6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A5;if(_tmp1A6->tag != Cyc_Dict_Absent)goto _LL7E;}_LL7D:
# 765
 goto _LL7B;_LL7E: _tmp1A7=_tmp1A5;_LL7F:(void)_throw(_tmp1A7);_LL7B:;}};}{
struct _dyneither_ptr*_tmp1A8=(*vd->name).f2;
const char*_tmp409;struct Cyc_Tcenv_Fenv*_tmp1A9=
# 769
Cyc_Tcenv_get_fenv(te,((_tmp409="add_pat_var",_tag_dyneither(_tmp409,sizeof(char),12))));
# 767
struct Cyc_Tcenv_SharedFenv*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_RgnOrder_RgnPO*_tmp1AD;const struct Cyc_Tcenv_Bindings*_tmp1AE;struct Cyc_Absyn_Stmt*_tmp1AF;struct Cyc_Tcenv_CtrlEnv*_tmp1B0;void*_tmp1B1;void*_tmp1B2;struct Cyc_Tcenv_FenvFlags _tmp1B3;struct Cyc_Tcenv_Fenv*_tmp1AA=_tmp1A9;_tmp1AB=_tmp1AA->shared;_tmp1AC=_tmp1AA->type_vars;_tmp1AD=_tmp1AA->region_order;_tmp1AE=_tmp1AA->locals;_tmp1AF=_tmp1AA->encloser;_tmp1B0=_tmp1AA->ctrl_env;_tmp1B1=_tmp1AA->capability;_tmp1B2=_tmp1AA->curr_rgn;_tmp1B3=_tmp1AA->flags;{
# 770
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp40C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp40B;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B4=(_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=((_tmp40C.tag=5,((_tmp40C.f1=vd,_tmp40C)))),_tmp40B)));
struct Cyc_Tcenv_Bindings*_tmp40D;struct Cyc_Tcenv_Bindings*_tmp1B5=(_tmp40D=_region_malloc(r,sizeof(*_tmp40D)),((_tmp40D->v=_tmp1A8,((_tmp40D->b=(void*)_tmp1B4,((_tmp40D->tl=_tmp1AE,_tmp40D)))))));
struct Cyc_Tcenv_Fenv*_tmp40E;struct Cyc_Tcenv_Fenv*_tmp1B6=(_tmp40E=_region_malloc(r,sizeof(*_tmp40E)),((_tmp40E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AB,((_tmp40E->type_vars=(struct Cyc_List_List*)_tmp1AC,((_tmp40E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1AD,((_tmp40E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp1B5),((_tmp40E->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AF,((_tmp40E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B0,((_tmp40E->capability=(void*)_tmp1B1,((_tmp40E->curr_rgn=(void*)_tmp1B2,((_tmp40E->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B3,((_tmp40E->fnrgn=(struct _RegionHandle*)r,_tmp40E)))))))))))))))))))));
# 774
return Cyc_Tcenv_put_fenv(r,te,_tmp1B6);};};}
# 777
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp1BC=te->le;
union Cyc_Absyn_Nmspace _tmp1BE;struct _dyneither_ptr*_tmp1BF;struct _tuple0*_tmp1BD=q;_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;{
union Cyc_Absyn_Nmspace _tmp1C0=_tmp1BE;_LL81: if((_tmp1C0.Loc_n).tag != 4)goto _LL83;_LL82:
# 782
 if(_tmp1BC == 0){struct Cyc_Dict_Absent_exn_struct _tmp411;struct Cyc_Dict_Absent_exn_struct*_tmp410;(int)_throw((void*)((_tmp410=_cycalloc_atomic(sizeof(*_tmp410)),((_tmp410[0]=((_tmp411.tag=Cyc_Dict_Absent,_tmp411)),_tmp410)))));}
goto _LL84;_LL83: if((_tmp1C0.Rel_n).tag != 1)goto _LL85;if((struct Cyc_List_List*)(_tmp1C0.Rel_n).val != 0)goto _LL85;if(!(_tmp1BC != 0))goto _LL85;_LL84: {
# 785
struct Cyc_Tcenv_Fenv*_tmp1C3=_tmp1BC;const struct Cyc_Tcenv_Bindings*_tmp1C5;struct Cyc_Tcenv_Fenv*_tmp1C4=_tmp1C3;_tmp1C5=_tmp1C4->locals;{
# 787
struct _handler_cons _tmp1C6;_push_handler(& _tmp1C6);{int _tmp1C8=0;if(setjmp(_tmp1C6.handler))_tmp1C8=1;if(!_tmp1C8){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp414;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp413;void*_tmp1CB=(void*)((_tmp413=_region_malloc(r,sizeof(*_tmp413)),((_tmp413[0]=((_tmp414.tag=0,((_tmp414.f1=Cyc_Tcenv_lookup_binding(_tmp1C5,_tmp1BF),_tmp414)))),_tmp413))));_npop_handler(0);return _tmp1CB;};_pop_handler();}else{void*_tmp1C7=(void*)_exn_thrown;void*_tmp1CD=_tmp1C7;void*_tmp1CF;_LL88: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp1CE=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1CD;if(_tmp1CE->tag != Cyc_Tcenv_NoBinding)goto _LL8A;}_LL89:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL8A: _tmp1CF=_tmp1CD;_LL8B:(void)_throw(_tmp1CF);_LL87:;}};};}_LL85:;_LL86: {
# 791
struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){{void*_tmp1D3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1D3;};_pop_handler();}else{void*_tmp1D1=(void*)_exn_thrown;void*_tmp1D5=_tmp1D1;void*_tmp1D7;_LL8D: {struct Cyc_Dict_Absent_exn_struct*_tmp1D6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1D5;if(_tmp1D6->tag != Cyc_Dict_Absent)goto _LL8F;}_LL8E: {
# 795
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp417;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp416;return(void*)((_tmp416=_region_malloc(r,sizeof(*_tmp416)),((_tmp416[0]=((_tmp417.tag=0,((_tmp417.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp417)))),_tmp416))));}_LL8F: _tmp1D7=_tmp1D5;_LL90:(void)_throw(_tmp1D7);_LL8C:;}};}_LL80:;};}
# 800
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp418;struct Cyc_Tcenv_Fenv*_tmp1DA=
Cyc_Tcenv_get_fenv(te,((_tmp418="process_continue",_tag_dyneither(_tmp418,sizeof(char),17))));
# 801
struct Cyc_Tcenv_CtrlEnv*_tmp1DC;struct Cyc_Tcenv_Fenv*_tmp1DB=_tmp1DA;_tmp1DC=_tmp1DB->ctrl_env;{
# 803
void*_tmp1DD=_tmp1DC->continue_stmt;void*_tmp1DE=_tmp1DD;struct Cyc_Absyn_Stmt*_tmp1E0;_LL92: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1DF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1DF->tag != 3)goto _LL94;else{_tmp1E0=_tmp1DF->f1;}}_LL93:
# 805
{struct Cyc_List_List*_tmp419;_tmp1E0->non_local_preds=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->hd=s,((_tmp419->tl=_tmp1E0->non_local_preds,_tmp419))))));}
*sopt=_tmp1E0;
return;_LL94: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1E1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E1->tag != 0)goto _LL96;}_LL95:
{const char*_tmp41C;void*_tmp41B;(_tmp41B=0,Cyc_Tcutil_terr(s->loc,((_tmp41C="continue not in a loop",_tag_dyneither(_tmp41C,sizeof(char),23))),_tag_dyneither(_tmp41B,sizeof(void*),0)));}return;_LL96: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1E2=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E2->tag != 1)goto _LL98;}_LL97:
 goto _LL99;_LL98: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1E3=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E3->tag != 2)goto _LL91;}_LL99: {
const char*_tmp41D;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp41D="bad continue destination",_tag_dyneither(_tmp41D,sizeof(char),25))));}_LL91:;};}
# 813
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp41E;struct Cyc_Tcenv_Fenv*_tmp1E9=
Cyc_Tcenv_get_fenv(te,((_tmp41E="process_break",_tag_dyneither(_tmp41E,sizeof(char),14))));
# 814
struct Cyc_Tcenv_CtrlEnv*_tmp1EB;struct Cyc_Tcenv_SharedFenv*_tmp1EC;struct Cyc_Tcenv_Fenv*_tmp1EA=_tmp1E9;_tmp1EB=_tmp1EA->ctrl_env;_tmp1EC=_tmp1EA->shared;{
# 816
void*_tmp1ED=_tmp1EB->break_stmt;void*_tmp1EE=_tmp1ED;struct Cyc_Absyn_Stmt*_tmp1F0;_LL9B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1EF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1EF->tag != 3)goto _LL9D;else{_tmp1F0=_tmp1EF->f1;}}_LL9C:
# 818
{struct Cyc_List_List*_tmp41F;_tmp1F0->non_local_preds=((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F->hd=s,((_tmp41F->tl=_tmp1F0->non_local_preds,_tmp41F))))));}
*sopt=_tmp1F0;
return;_LL9D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1F1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F1->tag != 0)goto _LL9F;}_LL9E:
# 822
{const char*_tmp422;void*_tmp421;(_tmp421=0,Cyc_Tcutil_terr(s->loc,((_tmp422="break not in a loop or switch",_tag_dyneither(_tmp422,sizeof(char),30))),_tag_dyneither(_tmp421,sizeof(void*),0)));}
return;_LL9F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1F2=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F2->tag != 2)goto _LLA1;}_LLA0:
# 842 "tcenv.cyc"
 return;_LLA1: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1F3=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F3->tag != 1)goto _LL9A;}_LLA2:
# 847
{const char*_tmp425;void*_tmp424;(_tmp424=0,Cyc_Tcutil_terr(s->loc,((_tmp425="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp425,sizeof(char),56))),_tag_dyneither(_tmp424,sizeof(void*),0)));}
return;_LL9A:;};}
# 851
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp426;struct Cyc_Tcenv_Fenv*_tmp1FA=Cyc_Tcenv_get_fenv(te,((_tmp426="process_goto",_tag_dyneither(_tmp426,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1FC;struct Cyc_Tcenv_Fenv*_tmp1FB=_tmp1FA;_tmp1FC=_tmp1FB->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FC->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1FD=_tmp1FC->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FD,l);
struct _RegionHandle*frgn=_tmp1FC->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp427;slopt=((_tmp427=_region_malloc(frgn,sizeof(*_tmp427)),((_tmp427[0]=0,_tmp427))));}{
struct Cyc_List_List*_tmp428;struct Cyc_List_List*new_needed=(_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428->hd=s,((_tmp428->tl=*slopt,_tmp428)))));
_tmp1FC->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1FD,l,new_needed);};}else{
# 863
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp429;s->non_local_preds=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->hd=s,((_tmp429->tl=s->non_local_preds,_tmp429))))));}
*sopt=s;}};}
# 869
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 871
const char*_tmp42A;struct Cyc_Tcenv_Fenv*_tmp202=Cyc_Tcenv_get_fenv(te,((_tmp42A="process_fallthru",_tag_dyneither(_tmp42A,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp204;struct Cyc_Tcenv_Fenv*_tmp203=_tmp202;_tmp204=_tmp203->ctrl_env;{
const struct _tuple10*_tmp205=_tmp204->fallthru_clause;
if(_tmp205 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp42B;(((*_tmp205).f1)->body)->non_local_preds=(
(_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B->hd=s,((_tmp42B->tl=(((*_tmp205).f1)->body)->non_local_preds,_tmp42B))))));}{
struct Cyc_Absyn_Switch_clause**_tmp42C;*clauseopt=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=(*_tmp205).f1,_tmp42C))));};}
# 878
return _tmp205;};}
# 881
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 885
const char*_tmp42D;struct Cyc_Tcenv_Fenv*_tmp209=
# 887
Cyc_Tcenv_get_fenv(te,((_tmp42D="set_fallthru",_tag_dyneither(_tmp42D,sizeof(char),13))));
# 885
struct Cyc_Tcenv_SharedFenv*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_RgnOrder_RgnPO*_tmp20D;const struct Cyc_Tcenv_Bindings*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Tcenv_CtrlEnv*_tmp210;void*_tmp211;void*_tmp212;struct Cyc_Tcenv_FenvFlags _tmp213;struct Cyc_Tcenv_Fenv*_tmp20A=_tmp209;_tmp20B=_tmp20A->shared;_tmp20C=_tmp20A->type_vars;_tmp20D=_tmp20A->region_order;_tmp20E=_tmp20A->locals;_tmp20F=_tmp20A->encloser;_tmp210=_tmp20A->ctrl_env;_tmp211=_tmp20A->capability;_tmp212=_tmp20A->curr_rgn;_tmp213=_tmp20A->flags;{
# 888
struct _RegionHandle*_tmp215;void*_tmp216;void*_tmp217;const struct _tuple10*_tmp218;void*_tmp219;int _tmp21A;struct Cyc_Tcenv_CtrlEnv*_tmp214=_tmp210;_tmp215=_tmp214->ctrl_rgn;_tmp216=_tmp214->continue_stmt;_tmp217=_tmp214->break_stmt;_tmp218=_tmp214->fallthru_clause;_tmp219=_tmp214->next_stmt;_tmp21A=_tmp214->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp42E;ft_typ=((_tmp42E=_region_malloc(_tmp215,sizeof(*_tmp42E)),((_tmp42E->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp42E->tl=ft_typ,_tmp42E))))));}{
const struct Cyc_Tcenv_CList*_tmp21C=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp431;struct Cyc_Tcenv_CtrlEnv*_tmp430;struct Cyc_Tcenv_CtrlEnv*_tmp21D=
(_tmp430=_region_malloc(r,sizeof(*_tmp430)),((_tmp430->ctrl_rgn=_tmp215,((_tmp430->continue_stmt=_tmp216,((_tmp430->break_stmt=_tmp217,((_tmp430->fallthru_clause=(const struct _tuple10*)((_tmp431=_region_malloc(_tmp215,sizeof(*_tmp431)),((_tmp431->f1=clause,((_tmp431->f2=new_tvs,((_tmp431->f3=_tmp21C,_tmp431)))))))),((_tmp430->next_stmt=_tmp219,((_tmp430->try_depth=_tmp21A,_tmp430)))))))))))));
# 896
struct Cyc_Tcenv_Fenv*_tmp432;struct Cyc_Tcenv_Fenv*_tmp21E=(_tmp432=_region_malloc(r,sizeof(*_tmp432)),((_tmp432->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20B,((_tmp432->type_vars=(struct Cyc_List_List*)_tmp20C,((_tmp432->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp20D,((_tmp432->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20E,((_tmp432->encloser=(struct Cyc_Absyn_Stmt*)_tmp20F,((_tmp432->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21D,((_tmp432->capability=(void*)_tmp211,((_tmp432->curr_rgn=(void*)_tmp212,((_tmp432->flags=(struct Cyc_Tcenv_FenvFlags)_tmp213,((_tmp432->fnrgn=(struct _RegionHandle*)r,_tmp432)))))))))))))))))))));
# 900
return Cyc_Tcenv_put_fenv(r,te,_tmp21E);};};};}
# 903
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp433;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp433="clear_fallthru",_tag_dyneither(_tmp433,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp224;struct Cyc_Tcenv_Fenv*_tmp223=fe;_tmp224=_tmp223->ctrl_env;
_tmp224->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 910
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 912
const char*_tmp434;struct Cyc_Tcenv_Fenv*_tmp226=
# 914
Cyc_Tcenv_get_fenv(te,((_tmp434="set_in_loop",_tag_dyneither(_tmp434,sizeof(char),12))));
# 912
struct Cyc_Tcenv_SharedFenv*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_RgnOrder_RgnPO*_tmp22A;const struct Cyc_Tcenv_Bindings*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Tcenv_CtrlEnv*_tmp22D;void*_tmp22E;void*_tmp22F;struct Cyc_Tcenv_FenvFlags _tmp230;struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;_tmp228=_tmp227->shared;_tmp229=_tmp227->type_vars;_tmp22A=_tmp227->region_order;_tmp22B=_tmp227->locals;_tmp22C=_tmp227->encloser;_tmp22D=_tmp227->ctrl_env;_tmp22E=_tmp227->capability;_tmp22F=_tmp227->curr_rgn;_tmp230=_tmp227->flags;{
# 915
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp43F;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp43E;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp43D;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp43C;struct Cyc_Tcenv_CtrlEnv*_tmp43B;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp43B=_region_malloc(r,sizeof(*_tmp43B)),((_tmp43B->ctrl_rgn=r,((_tmp43B->continue_stmt=(void*)(
(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp43C.tag=3,((_tmp43C.f1=continue_dest,_tmp43C)))),_tmp43D)))),((_tmp43B->break_stmt=_tmp22D->next_stmt,((_tmp43B->next_stmt=(void*)(
# 919
(_tmp43F=_region_malloc(r,sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp43E.tag=3,((_tmp43E.f1=continue_dest,_tmp43E)))),_tmp43F)))),((_tmp43B->fallthru_clause=_tmp22D->fallthru_clause,((_tmp43B->try_depth=_tmp22D->try_depth,_tmp43B)))))))))))));
# 922
struct Cyc_Tcenv_Fenv*_tmp440;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp440=_region_malloc(r,sizeof(*_tmp440)),((_tmp440->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp228,((_tmp440->type_vars=(struct Cyc_List_List*)_tmp229,((_tmp440->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22A,((_tmp440->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22B,((_tmp440->encloser=(struct Cyc_Absyn_Stmt*)_tmp22C,((_tmp440->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp440->capability=(void*)_tmp22E,((_tmp440->curr_rgn=(void*)_tmp22F,((_tmp440->flags=(struct Cyc_Tcenv_FenvFlags)_tmp230,((_tmp440->fnrgn=(struct _RegionHandle*)r,_tmp440)))))))))))))))))))));
# 925
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 928
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp441;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp441="enter_try",_tag_dyneither(_tmp441,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp239;struct Cyc_Tcenv_Fenv*_tmp238=fe;_tmp239=_tmp238->ctrl_env;
++ _tmp239->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 934
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp442;struct Cyc_Tcenv_Fenv*_tmp23B=Cyc_Tcenv_get_fenv(te,((_tmp442="get_try_depth",_tag_dyneither(_tmp442,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp23D;struct Cyc_Tcenv_Fenv*_tmp23C=_tmp23B;_tmp23D=_tmp23C->ctrl_env;
return _tmp23D->try_depth;}
# 941
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp443;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp443="set_in_switch",_tag_dyneither(_tmp443,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp240;struct Cyc_Tcenv_Fenv*_tmp23F=fe;_tmp240=_tmp23F->ctrl_env;
_tmp240->break_stmt=_tmp240->next_stmt;
_tmp240->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 949
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 951
const char*_tmp444;struct Cyc_Tcenv_Fenv _tmp242=*
# 953
Cyc_Tcenv_get_fenv(te,((_tmp444="set_next",_tag_dyneither(_tmp444,sizeof(char),9))));
# 951
struct Cyc_Tcenv_SharedFenv*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_RgnOrder_RgnPO*_tmp246;const struct Cyc_Tcenv_Bindings*_tmp247;struct Cyc_Absyn_Stmt*_tmp248;struct Cyc_Tcenv_CtrlEnv*_tmp249;void*_tmp24A;void*_tmp24B;struct Cyc_Tcenv_FenvFlags _tmp24C;struct Cyc_Tcenv_Fenv _tmp243=_tmp242;_tmp244=_tmp243.shared;_tmp245=_tmp243.type_vars;_tmp246=_tmp243.region_order;_tmp247=_tmp243.locals;_tmp248=_tmp243.encloser;_tmp249=_tmp243.ctrl_env;_tmp24A=_tmp243.capability;_tmp24B=_tmp243.curr_rgn;_tmp24C=_tmp243.flags;{
# 954
struct Cyc_Tcenv_CtrlEnv*_tmp445;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp445=_region_malloc(r,sizeof(*_tmp445)),((_tmp445->ctrl_rgn=r,((_tmp445->continue_stmt=_tmp249->continue_stmt,((_tmp445->break_stmt=_tmp249->break_stmt,((_tmp445->next_stmt=j,((_tmp445->fallthru_clause=_tmp249->fallthru_clause,((_tmp445->try_depth=_tmp249->try_depth,_tmp445)))))))))))));
# 962
struct Cyc_Tcenv_Fenv*_tmp446;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp446=_region_malloc(r,sizeof(*_tmp446)),((_tmp446->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp244,((_tmp446->type_vars=(struct Cyc_List_List*)_tmp245,((_tmp446->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp246,((_tmp446->locals=(const struct Cyc_Tcenv_Bindings*)_tmp247,((_tmp446->encloser=(struct Cyc_Absyn_Stmt*)_tmp248,((_tmp446->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp446->capability=(void*)_tmp24A,((_tmp446->curr_rgn=(void*)_tmp24B,((_tmp446->flags=(struct Cyc_Tcenv_FenvFlags)_tmp24C,((_tmp446->fnrgn=(struct _RegionHandle*)r,_tmp446)))))))))))))))))))));
# 965
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 968
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp447;struct Cyc_Tcenv_Fenv*_tmp250=Cyc_Tcenv_get_fenv(te,((_tmp447="add_label",_tag_dyneither(_tmp447,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp252;struct Cyc_Tcenv_Fenv*_tmp251=_tmp250;_tmp252=_tmp251->shared;{
struct Cyc_Dict_Dict needed=_tmp252->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp252->frgn;
if(sl_opt != 0){
_tmp252->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 976
struct Cyc_List_List*_tmp253=*sl_opt;
s->non_local_preds=_tmp253;
for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
void*_tmp254=((struct Cyc_Absyn_Stmt*)_tmp253->hd)->r;void*_tmp255=_tmp254;struct Cyc_Absyn_Stmt**_tmp257;_LLA4: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp256=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp255;if(_tmp256->tag != 8)goto _LLA6;else{_tmp257=(struct Cyc_Absyn_Stmt**)& _tmp256->f2;}}_LLA5:
*_tmp257=s;goto _LLA3;_LLA6:;_LLA7:
{const char*_tmp448;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp448="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp448,sizeof(char),42))));}goto _LLA3;_LLA3:;}};}
# 984
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp252->seen_labels,v)){
const char*_tmp44C;void*_tmp44B[1];struct Cyc_String_pa_PrintArg_struct _tmp44A;(_tmp44A.tag=0,((_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp44B[0]=& _tmp44A,Cyc_Tcutil_terr(s->loc,((_tmp44C="Repeated label: %s",_tag_dyneither(_tmp44C,sizeof(char),19))),_tag_dyneither(_tmp44B,sizeof(void*),1)))))));}
_tmp252->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp252->seen_labels,v,s);
return te;};}
# 990
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp44D;struct Cyc_Tcenv_Fenv*_tmp25D=Cyc_Tcenv_get_fenv(te,((_tmp44D="all_labels_resolved",_tag_dyneither(_tmp44D,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp25F;struct Cyc_Tcenv_Fenv*_tmp25E=_tmp25D;_tmp25F=_tmp25E->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp25F->needed_labels);}
# 995
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp44E;struct Cyc_Tcenv_Fenv*_tmp261=Cyc_Tcenv_get_fenv(te,((_tmp44E="get_encloser",_tag_dyneither(_tmp44E,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp263;struct Cyc_Tcenv_Fenv*_tmp262=_tmp261;_tmp263=_tmp262->encloser;
return _tmp263;}
# 1000
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp44F;return(_tmp44F=_region_malloc(r2,sizeof(*_tmp44F)),((_tmp44F->ns=te->ns,((_tmp44F->ae=te->ae,((_tmp44F->le=0,((_tmp44F->allow_valueof=1,_tmp44F)))))))));}{
struct Cyc_Tcenv_Fenv _tmp266=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_RgnOrder_RgnPO*_tmp26A;const struct Cyc_Tcenv_Bindings*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Tcenv_CtrlEnv*_tmp26D;void*_tmp26E;void*_tmp26F;struct Cyc_Tcenv_FenvFlags _tmp270;struct Cyc_Tcenv_Fenv _tmp267=_tmp266;_tmp268=_tmp267.shared;_tmp269=_tmp267.type_vars;_tmp26A=_tmp267.region_order;_tmp26B=_tmp267.locals;_tmp26C=_tmp267.encloser;_tmp26D=_tmp267.ctrl_env;_tmp26E=_tmp267.capability;_tmp26F=_tmp267.curr_rgn;_tmp270=_tmp267.flags;{
struct Cyc_Tcenv_Fenv*_tmp450;struct Cyc_Tcenv_Fenv*_tmp271=(_tmp450=_region_malloc(r2,sizeof(*_tmp450)),((_tmp450->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp268,((_tmp450->type_vars=(struct Cyc_List_List*)_tmp269,((_tmp450->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26A,((_tmp450->locals=(const struct Cyc_Tcenv_Bindings*)_tmp26B,((_tmp450->encloser=(struct Cyc_Absyn_Stmt*)_tmp26C,((_tmp450->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26D,((_tmp450->capability=(void*)_tmp26E,((_tmp450->curr_rgn=(void*)_tmp26F,((_tmp450->flags=(struct Cyc_Tcenv_FenvFlags)_tmp270,((_tmp450->fnrgn=(struct _RegionHandle*)r2,_tmp450)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp451;return(_tmp451=_region_malloc(r2,sizeof(*_tmp451)),((_tmp451->ns=te->ns,((_tmp451->ae=te->ae,((_tmp451->le=_tmp271,((_tmp451->allow_valueof=1,_tmp451)))))))));};};}
# 1008
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1010
const char*_tmp452;struct Cyc_Tcenv_Fenv _tmp274=*
# 1012
Cyc_Tcenv_get_fenv(te,((_tmp452="set_encloser",_tag_dyneither(_tmp452,sizeof(char),13))));
# 1010
struct Cyc_Tcenv_SharedFenv*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_RgnOrder_RgnPO*_tmp278;const struct Cyc_Tcenv_Bindings*_tmp279;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Tcenv_CtrlEnv*_tmp27B;void*_tmp27C;void*_tmp27D;struct Cyc_Tcenv_FenvFlags _tmp27E;struct Cyc_Tcenv_Fenv _tmp275=_tmp274;_tmp276=_tmp275.shared;_tmp277=_tmp275.type_vars;_tmp278=_tmp275.region_order;_tmp279=_tmp275.locals;_tmp27A=_tmp275.encloser;_tmp27B=_tmp275.ctrl_env;_tmp27C=_tmp275.capability;_tmp27D=_tmp275.curr_rgn;_tmp27E=_tmp275.flags;{
# 1013
struct Cyc_Tcenv_Fenv*_tmp453;struct Cyc_Tcenv_Fenv*_tmp27F=
(_tmp453=_region_malloc(r,sizeof(*_tmp453)),((_tmp453->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp276,((_tmp453->type_vars=(struct Cyc_List_List*)_tmp277,((_tmp453->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp278,((_tmp453->locals=(const struct Cyc_Tcenv_Bindings*)_tmp279,((_tmp453->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp453->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp27B,((_tmp453->capability=(void*)_tmp27C,((_tmp453->curr_rgn=(void*)_tmp27D,((_tmp453->flags=(struct Cyc_Tcenv_FenvFlags)_tmp27E,((_tmp453->fnrgn=(struct _RegionHandle*)r,_tmp453)))))))))))))))))))));
# 1016
return Cyc_Tcenv_put_fenv(r,te,_tmp27F);};}
# 1019
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1022
struct Cyc_Absyn_Tvar*tv;
{void*_tmp282=Cyc_Tcutil_compress(rgn);void*_tmp283=_tmp282;struct Cyc_Absyn_Tvar*_tmp285;_LLA9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp284=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp283;if(_tmp284->tag != 2)goto _LLAB;else{_tmp285=_tmp284->f1;}}_LLAA:
 tv=_tmp285;goto _LLA8;_LLAB:;_LLAC:
{const char*_tmp454;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp454="bad add region",_tag_dyneither(_tmp454,sizeof(char),15))));}goto _LLA8;_LLA8:;}{
# 1027
const char*_tmp455;struct Cyc_Tcenv_Fenv _tmp287=*
# 1029
Cyc_Tcenv_get_fenv(te,((_tmp455="add_region",_tag_dyneither(_tmp455,sizeof(char),11))));
# 1027
struct Cyc_Tcenv_SharedFenv*_tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_RgnOrder_RgnPO*_tmp28B;const struct Cyc_Tcenv_Bindings*_tmp28C;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Tcenv_CtrlEnv*_tmp28E;void*_tmp28F;void*_tmp290;struct Cyc_Tcenv_FenvFlags _tmp291;struct Cyc_Tcenv_Fenv _tmp288=_tmp287;_tmp289=_tmp288.shared;_tmp28A=_tmp288.type_vars;_tmp28B=_tmp288.region_order;_tmp28C=_tmp288.locals;_tmp28D=_tmp288.encloser;_tmp28E=_tmp288.ctrl_env;_tmp28F=_tmp288.capability;_tmp290=_tmp288.curr_rgn;_tmp291=_tmp288.flags;
# 1030
_tmp28B=Cyc_RgnOrder_add_youngest(_tmp289->frgn,_tmp28B,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp468;struct Cyc_List_List*_tmp467;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp466;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp465;struct Cyc_List_List*_tmp464;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp463;_tmp28F=(void*)((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=((_tmp468.tag=24,((_tmp468.f1=((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464->hd=(void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((_tmp465.tag=23,((_tmp465.f1=rgn,_tmp465)))),_tmp466)))),((_tmp464->tl=(
(_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->hd=_tmp28F,((_tmp467->tl=0,_tmp467)))))),_tmp464)))))),_tmp468)))),_tmp463))));}{
struct Cyc_Tcenv_Fenv*_tmp469;struct Cyc_Tcenv_Fenv*_tmp298=
(_tmp469=_region_malloc(r,sizeof(*_tmp469)),((_tmp469->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp289,((_tmp469->type_vars=(struct Cyc_List_List*)_tmp28A,((_tmp469->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp28B,((_tmp469->locals=(const struct Cyc_Tcenv_Bindings*)_tmp28C,((_tmp469->encloser=(struct Cyc_Absyn_Stmt*)_tmp28D,((_tmp469->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp28E,((_tmp469->capability=(void*)_tmp28F,((_tmp469->curr_rgn=(void*)_tmp290,((_tmp469->flags=(struct Cyc_Tcenv_FenvFlags)_tmp291,((_tmp469->fnrgn=(struct _RegionHandle*)r,_tmp469)))))))))))))))))))));
# 1036
return Cyc_Tcenv_put_fenv(r,te,_tmp298);};};}
# 1039
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1041
const char*_tmp46A;struct Cyc_Tcenv_Fenv _tmp29B=*
# 1043
Cyc_Tcenv_get_fenv(te,((_tmp46A="new_named_block",_tag_dyneither(_tmp46A,sizeof(char),16))));
# 1041
struct Cyc_Tcenv_SharedFenv*_tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_RgnOrder_RgnPO*_tmp29F;const struct Cyc_Tcenv_Bindings*_tmp2A0;struct Cyc_Absyn_Stmt*_tmp2A1;struct Cyc_Tcenv_CtrlEnv*_tmp2A2;void*_tmp2A3;void*_tmp2A4;struct Cyc_Tcenv_FenvFlags _tmp2A5;struct Cyc_Tcenv_Fenv _tmp29C=_tmp29B;_tmp29D=_tmp29C.shared;_tmp29E=_tmp29C.type_vars;_tmp29F=_tmp29C.region_order;_tmp2A0=_tmp29C.locals;_tmp2A1=_tmp29C.encloser;_tmp2A2=_tmp29C.ctrl_env;_tmp2A3=_tmp29C.capability;_tmp2A4=_tmp29C.curr_rgn;_tmp2A5=_tmp29C.flags;{
# 1045
const char*_tmp46B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp46B="new_block",_tag_dyneither(_tmp46B,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp46E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp46D;void*block_typ=(void*)((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp46E.tag=2,((_tmp46E.f1=block_rgn,_tmp46E)))),_tmp46D))));
{struct Cyc_List_List*_tmp46F;_tmp29E=((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F->hd=block_rgn,((_tmp46F->tl=_tmp29E,_tmp46F))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp29E);
_tmp29F=Cyc_RgnOrder_add_youngest(_tmp29D->frgn,_tmp29F,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp482;struct Cyc_List_List*_tmp481;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp480;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp47F;struct Cyc_List_List*_tmp47E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp47D;_tmp2A3=(void*)((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D[0]=((_tmp482.tag=24,((_tmp482.f1=((_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E->hd=(void*)((_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480[0]=((_tmp47F.tag=23,((_tmp47F.f1=block_typ,_tmp47F)))),_tmp480)))),((_tmp47E->tl=((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->hd=_tmp2A3,((_tmp481->tl=0,_tmp481)))))),_tmp47E)))))),_tmp482)))),_tmp47D))));}
_tmp2A4=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp483;struct Cyc_Tcenv_Fenv*_tmp2AD=
(_tmp483=_region_malloc(r,sizeof(*_tmp483)),((_tmp483->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp29D,((_tmp483->type_vars=(struct Cyc_List_List*)_tmp29E,((_tmp483->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp29F,((_tmp483->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2A0,((_tmp483->encloser=(struct Cyc_Absyn_Stmt*)_tmp2A1,((_tmp483->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2A2,((_tmp483->capability=(void*)_tmp2A3,((_tmp483->curr_rgn=(void*)_tmp2A4,((_tmp483->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2A5,((_tmp483->fnrgn=(struct _RegionHandle*)r,_tmp483)))))))))))))))))))));
# 1055
return Cyc_Tcenv_put_fenv(r,te,_tmp2AD);};};}
# 1058
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1060
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1062
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1068
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1072
const char*_tmp484;struct Cyc_Tcenv_Fenv _tmp2B4=*
# 1074
Cyc_Tcenv_get_fenv(te,((_tmp484="new_outlives_constraints",_tag_dyneither(_tmp484,sizeof(char),25))));
# 1072
struct Cyc_Tcenv_SharedFenv*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_RgnOrder_RgnPO*_tmp2B8;const struct Cyc_Tcenv_Bindings*_tmp2B9;struct Cyc_Absyn_Stmt*_tmp2BA;struct Cyc_Tcenv_CtrlEnv*_tmp2BB;void*_tmp2BC;void*_tmp2BD;struct Cyc_Tcenv_FenvFlags _tmp2BE;struct Cyc_Tcenv_Fenv _tmp2B5=_tmp2B4;_tmp2B6=_tmp2B5.shared;_tmp2B7=_tmp2B5.type_vars;_tmp2B8=_tmp2B5.region_order;_tmp2B9=_tmp2B5.locals;_tmp2BA=_tmp2B5.encloser;_tmp2BB=_tmp2B5.ctrl_env;_tmp2BC=_tmp2B5.capability;_tmp2BD=_tmp2B5.curr_rgn;_tmp2BE=_tmp2B5.flags;
# 1075
for(0;cs != 0;cs=cs->tl){
_tmp2B8=
Cyc_RgnOrder_add_outlives_constraint(_tmp2B6->frgn,_tmp2B8,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1080
struct Cyc_Tcenv_Fenv*_tmp485;struct Cyc_Tcenv_Fenv*_tmp2BF=
(_tmp485=_region_malloc(r,sizeof(*_tmp485)),((_tmp485->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp2B6,((_tmp485->type_vars=(struct Cyc_List_List*)_tmp2B7,((_tmp485->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2B8,((_tmp485->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2B9,((_tmp485->encloser=(struct Cyc_Absyn_Stmt*)_tmp2BA,((_tmp485->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2BB,((_tmp485->capability=(void*)_tmp2BC,((_tmp485->curr_rgn=(void*)_tmp2BD,((_tmp485->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2BE,((_tmp485->fnrgn=(struct _RegionHandle*)r,_tmp485)))))))))))))))))))));
# 1083
return Cyc_Tcenv_put_fenv(r,te,_tmp2BF);};}
# 1086
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1094
void*_tmp2C2=Cyc_Tcutil_compress(r1);
void*_tmp2C3=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp2C4=Cyc_Tcutil_typ_kind(_tmp2C2);
struct Cyc_Absyn_Kind*_tmp2C5=Cyc_Tcutil_typ_kind(_tmp2C3);
# 1100
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp2C4,_tmp2C5);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2C5,_tmp2C4);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp48C;void*_tmp48B[4];struct Cyc_String_pa_PrintArg_struct _tmp48A;struct Cyc_String_pa_PrintArg_struct _tmp489;struct Cyc_String_pa_PrintArg_struct _tmp488;struct Cyc_String_pa_PrintArg_struct _tmp487;(_tmp487.tag=0,((_tmp487.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2C5)),((_tmp488.tag=0,((_tmp488.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp2C4)),((_tmp489.tag=0,((_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C3)),((_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C2)),((_tmp48B[0]=& _tmp48A,((_tmp48B[1]=& _tmp489,((_tmp48B[2]=& _tmp488,((_tmp48B[3]=& _tmp487,Cyc_Tcutil_terr(loc,((_tmp48C="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp48C,sizeof(char),82))),_tag_dyneither(_tmp48B,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1107
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2CC=Cyc_Tcutil_swap_kind(_tmp2C3,Cyc_Tcutil_kind_to_bound(_tmp2C4));struct Cyc_Absyn_Tvar*_tmp2CE;void*_tmp2CF;struct _tuple11 _tmp2CD=_tmp2CC;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;{
struct _tuple11*_tmp48D;*oldtv=((_tmp48D=_region_malloc(r,sizeof(*_tmp48D)),((_tmp48D->f1=_tmp2CE,((_tmp48D->f2=_tmp2CF,_tmp48D))))));};}else{
# 1111
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2D1=Cyc_Tcutil_swap_kind(_tmp2C2,Cyc_Tcutil_kind_to_bound(_tmp2C5));struct Cyc_Absyn_Tvar*_tmp2D3;void*_tmp2D4;struct _tuple11 _tmp2D2=_tmp2D1;_tmp2D3=_tmp2D2.f1;_tmp2D4=_tmp2D2.f2;{
struct _tuple11*_tmp48E;*oldtv=((_tmp48E=_region_malloc(r,sizeof(*_tmp48E)),((_tmp48E->f1=_tmp2D3,((_tmp48E->f2=_tmp2D4,_tmp48E))))));};}}}{
# 1117
struct _RegionHandle*_tmp2D6=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2D7=0;
if((_tmp2C2 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2C2 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2C2 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp491;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp490;void*eff1=(void*)((_tmp490=_cycalloc(sizeof(*_tmp490)),((_tmp490[0]=((_tmp491.tag=23,((_tmp491.f1=_tmp2C2,_tmp491)))),_tmp490))));
struct _tuple13*_tmp494;struct Cyc_List_List*_tmp493;_tmp2D7=((_tmp493=_region_malloc(_tmp2D6,sizeof(*_tmp493)),((_tmp493->hd=((_tmp494=_region_malloc(_tmp2D6,sizeof(*_tmp494)),((_tmp494->f1=eff1,((_tmp494->f2=_tmp2C3,_tmp494)))))),((_tmp493->tl=_tmp2D7,_tmp493))))));}
# 1123
if((_tmp2C3 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2C3 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2C3 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp497;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp496;void*eff2=(void*)((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496[0]=((_tmp497.tag=23,((_tmp497.f1=_tmp2C3,_tmp497)))),_tmp496))));
struct _tuple13*_tmp49A;struct Cyc_List_List*_tmp499;_tmp2D7=((_tmp499=_region_malloc(_tmp2D6,sizeof(*_tmp499)),((_tmp499->hd=((_tmp49A=_region_malloc(_tmp2D6,sizeof(*_tmp49A)),((_tmp49A->f1=eff2,((_tmp49A->f2=_tmp2C2,_tmp49A)))))),((_tmp499->tl=_tmp2D7,_tmp499))))));}
# 1128
return Cyc_Tcenv_new_outlives_constraints(_tmp2D6,te,_tmp2D7,loc);};}
# 1131
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2E0=te->le;
if(_tmp2E0 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2E0;
void*_tmp2E2;struct Cyc_Tcenv_Fenv*_tmp2E1=fe;_tmp2E2=_tmp2E1->curr_rgn;
return _tmp2E2;};}
# 1142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp49B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp49B="check_rgn_accessible",_tag_dyneither(_tmp49B,sizeof(char),21))));
void*_tmp2E4;struct Cyc_RgnOrder_RgnPO*_tmp2E5;struct Cyc_Tcenv_Fenv*_tmp2E3=fe;_tmp2E4=_tmp2E3->capability;_tmp2E5=_tmp2E3->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2E4) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2E4))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp49E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp49D;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E5,(void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp49E.tag=23,((_tmp49E.f1=rgn,_tmp49E)))),_tmp49D)))),_tmp2E4))
return;}{
const char*_tmp4A2;void*_tmp4A1[1];struct Cyc_String_pa_PrintArg_struct _tmp4A0;(_tmp4A0.tag=0,((_tmp4A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4A1[0]=& _tmp4A0,Cyc_Tcutil_terr(loc,((_tmp4A2="Expression accesses unavailable region %s",_tag_dyneither(_tmp4A2,sizeof(char),42))),_tag_dyneither(_tmp4A1,sizeof(void*),1)))))));};}
# 1154
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp4A3;struct Cyc_Tcenv_Fenv*_tmp2EC=
Cyc_Tcenv_get_fenv(te,((_tmp4A3="check_rgn_resetable",_tag_dyneither(_tmp4A3,sizeof(char),20))));
# 1156
struct Cyc_RgnOrder_RgnPO*_tmp2EE;struct Cyc_Tcenv_Fenv*_tmp2ED=_tmp2EC;_tmp2EE=_tmp2ED->region_order;{
# 1158
void*_tmp2EF=Cyc_Tcutil_compress(rgn);void*_tmp2F0=_tmp2EF;struct Cyc_Absyn_Tvar*_tmp2F2;_LLAE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F1->tag != 2)goto _LLB0;else{_tmp2F2=_tmp2F1->f1;}}_LLAF:
# 1160
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2EE,_tmp2F2)){
const char*_tmp4A7;void*_tmp4A6[1];struct Cyc_String_pa_PrintArg_struct _tmp4A5;(_tmp4A5.tag=0,((_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4A6[0]=& _tmp4A5,Cyc_Tcutil_terr(loc,((_tmp4A7="Region %s is not resetable",_tag_dyneither(_tmp4A7,sizeof(char),27))),_tag_dyneither(_tmp4A6,sizeof(void*),1)))))));}
return;_LLB0:;_LLB1: {
# 1164
const char*_tmp4AA;void*_tmp4A9;(_tmp4A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AA="check_rgn_resetable",_tag_dyneither(_tmp4AA,sizeof(char),20))),_tag_dyneither(_tmp4A9,sizeof(void*),0)));}_LLAD:;};};}
# 1171
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2F9=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1176
if(_tmp2F9 == 0){
void*_tmp2FA=rt_a;_LLB3: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2FA;if(_tmp2FB->tag != 22)goto _LLB5;}_LLB4:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LLB5: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2FA;if(_tmp2FC->tag != 21)goto _LLB7;}_LLB6:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLB7: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2FA;if(_tmp2FD->tag != 20)goto _LLB9;}_LLB8:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLB9:;_LLBA:
 return 0;_LLB2:;}{
# 1184
struct Cyc_Tcenv_Fenv*fe=_tmp2F9;
struct Cyc_RgnOrder_RgnPO*_tmp2FF;struct Cyc_Tcenv_Fenv*_tmp2FE=fe;_tmp2FF=_tmp2FE->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4AD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4AC;int _tmp300=Cyc_RgnOrder_effect_outlives(_tmp2FF,(void*)((_tmp4AC=_cycalloc(sizeof(*_tmp4AC)),((_tmp4AC[0]=((_tmp4AD.tag=23,((_tmp4AD.f1=rt_a,_tmp4AD)))),_tmp4AC)))),rt_b);
# 1190
return _tmp300;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1195
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp4AE;struct Cyc_Tcenv_Fenv*_tmp303=
Cyc_Tcenv_get_fenv(te,((_tmp4AE="check_effect_accessible",_tag_dyneither(_tmp4AE,sizeof(char),24))));
# 1196
void*_tmp305;struct Cyc_RgnOrder_RgnPO*_tmp306;struct Cyc_Tcenv_SharedFenv*_tmp307;struct Cyc_Tcenv_Fenv*_tmp304=_tmp303;_tmp305=_tmp304->capability;_tmp306=_tmp304->region_order;_tmp307=_tmp304->shared;
# 1198
if(Cyc_Tcutil_subset_effect(0,eff,_tmp305))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp306,eff,_tmp305))
return;{
struct _RegionHandle*frgn=_tmp307->frgn;
struct _tuple14*_tmp4B1;struct Cyc_List_List*_tmp4B0;_tmp307->delayed_effect_checks=(
(_tmp4B0=_region_malloc(frgn,sizeof(*_tmp4B0)),((_tmp4B0->hd=((_tmp4B1=_region_malloc(frgn,sizeof(*_tmp4B1)),((_tmp4B1->f1=_tmp305,((_tmp4B1->f2=eff,((_tmp4B1->f3=_tmp306,((_tmp4B1->f4=loc,_tmp4B1)))))))))),((_tmp4B0->tl=_tmp307->delayed_effect_checks,_tmp4B0))))));};}
# 1207
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4B2;struct Cyc_Tcenv_Fenv*_tmp30B=
Cyc_Tcenv_get_fenv(te,((_tmp4B2="check_delayed_effects",_tag_dyneither(_tmp4B2,sizeof(char),22))));
# 1208
struct Cyc_Tcenv_SharedFenv*_tmp30D;struct Cyc_Tcenv_Fenv*_tmp30C=_tmp30B;_tmp30D=_tmp30C->shared;{
# 1210
struct Cyc_List_List*_tmp30E=_tmp30D->delayed_effect_checks;
for(0;_tmp30E != 0;_tmp30E=_tmp30E->tl){
struct _tuple14*_tmp30F=(struct _tuple14*)_tmp30E->hd;void*_tmp311;void*_tmp312;struct Cyc_RgnOrder_RgnPO*_tmp313;unsigned int _tmp314;struct _tuple14*_tmp310=_tmp30F;_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;_tmp313=_tmp310->f3;_tmp314=_tmp310->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp312,_tmp311))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp313,_tmp312,_tmp311))
continue;{
const char*_tmp4B7;void*_tmp4B6[2];struct Cyc_String_pa_PrintArg_struct _tmp4B5;struct Cyc_String_pa_PrintArg_struct _tmp4B4;(_tmp4B4.tag=0,((_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp312)),((_tmp4B5.tag=0,((_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp311)),((_tmp4B6[0]=& _tmp4B5,((_tmp4B6[1]=& _tmp4B4,Cyc_Tcutil_terr(_tmp314,((_tmp4B7="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp4B7,sizeof(char),51))),_tag_dyneither(_tmp4B6,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1226
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1228
struct Cyc_Tcenv_Fenv*_tmp31A=te->le;
if(_tmp31A == 0){
# 1231
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4BA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4B9;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=((_tmp4BA.tag=23,((_tmp4BA.f1=(*((struct _tuple13*)po->hd)).f2,_tmp4BA)))),_tmp4B9)))),Cyc_Absyn_empty_effect)){
const char*_tmp4BD;void*_tmp4BC;(_tmp4BC=0,Cyc_Tcutil_terr(loc,((_tmp4BD="the required region ordering is not satisfied here",_tag_dyneither(_tmp4BD,sizeof(char),51))),_tag_dyneither(_tmp4BC,sizeof(void*),0)));}}
return;}{
# 1237
struct Cyc_Tcenv_Fenv*_tmp31F=_tmp31A;struct Cyc_RgnOrder_RgnPO*_tmp321;struct Cyc_Tcenv_SharedFenv*_tmp322;struct Cyc_Tcenv_Fenv*_tmp320=_tmp31F;_tmp321=_tmp320->region_order;_tmp322=_tmp320->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp321,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp4C0;struct Cyc_List_List*_tmp4BF;_tmp322->delayed_constraint_checks=(
(_tmp4BF=_region_malloc(_tmp322->frgn,sizeof(*_tmp4BF)),((_tmp4BF->hd=((_tmp4C0=_region_malloc(_tmp322->frgn,sizeof(*_tmp4C0)),((_tmp4C0->f1=_tmp321,((_tmp4C0->f2=po,((_tmp4C0->f3=loc,_tmp4C0)))))))),((_tmp4BF->tl=_tmp322->delayed_constraint_checks,_tmp4BF))))));}};}
# 1244
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4C1;struct Cyc_Tcenv_Fenv*_tmp325=
Cyc_Tcenv_get_fenv(te,((_tmp4C1="check_delayed_constraints",_tag_dyneither(_tmp4C1,sizeof(char),26))));
# 1245
struct Cyc_Tcenv_SharedFenv*_tmp327;struct Cyc_Tcenv_Fenv*_tmp326=_tmp325;_tmp327=_tmp326->shared;{
# 1247
struct Cyc_List_List*_tmp328=_tmp327->delayed_constraint_checks;
for(0;_tmp328 != 0;_tmp328=_tmp328->tl){
struct _tuple15*_tmp329=(struct _tuple15*)_tmp328->hd;struct Cyc_RgnOrder_RgnPO*_tmp32B;struct Cyc_List_List*_tmp32C;unsigned int _tmp32D;struct _tuple15*_tmp32A=_tmp329;_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;_tmp32D=_tmp32A->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp32B,_tmp32C,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,Cyc_Tcutil_terr(_tmp32D,((_tmp4C4="the required region ordering is not satisfied here",_tag_dyneither(_tmp4C4,sizeof(char),51))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}}};}
# 1255
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp4D1;const char*_tmp4D0;void*_tmp4CF[1];struct Cyc_String_pa_PrintArg_struct _tmp4CE;struct Cyc_Absyn_Tvar*_tmp4CD;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD->name=((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=(struct _dyneither_ptr)((_tmp4CE.tag=0,((_tmp4CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4CF[0]=& _tmp4CE,Cyc_aprintf(((_tmp4D0="`%s",_tag_dyneither(_tmp4D0,sizeof(char),4))),_tag_dyneither(_tmp4CF,sizeof(void*),1)))))))),_tmp4D1)))),((_tmp4CD->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4CD->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4CD)))))));
struct Cyc_List_List*_tmp4D2;struct Cyc_List_List*_tmp331=(_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2->hd=rgn0,((_tmp4D2->tl=fd->tvs,_tmp4D2)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp331);{
# 1263
struct Cyc_RgnOrder_RgnPO*_tmp332=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1265
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D4;void*param_rgn=(void*)((_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4[0]=((_tmp4D5.tag=2,((_tmp4D5.f1=rgn0,_tmp4D5)))),_tmp4D4))));
struct Cyc_List_List*_tmp333=0;
{struct Cyc_List_List*_tmp334=fd->args;for(0;_tmp334 != 0;_tmp334=_tmp334->tl){
struct Cyc_Absyn_Vardecl _tmp4DB;struct _tuple0*_tmp4DA;struct Cyc_Absyn_Vardecl*_tmp4D9;struct Cyc_Absyn_Vardecl*_tmp335=(_tmp4D9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4D9[0]=(struct Cyc_Absyn_Vardecl)((_tmp4DB.sc=Cyc_Absyn_Public,((_tmp4DB.name=(
(_tmp4DA=_cycalloc(sizeof(*_tmp4DA)),((_tmp4DA->f1=Cyc_Absyn_Loc_n,((_tmp4DA->f2=(*((struct _tuple8*)_tmp334->hd)).f1,_tmp4DA)))))),((_tmp4DB.tq=(*((struct _tuple8*)_tmp334->hd)).f2,((_tmp4DB.type=(*((struct _tuple8*)_tmp334->hd)).f3,((_tmp4DB.initializer=0,((_tmp4DB.rgn=param_rgn,((_tmp4DB.attributes=0,((_tmp4DB.escapes=0,_tmp4DB)))))))))))))))),_tmp4D9)));
# 1275
{struct Cyc_List_List _tmp4DE;struct Cyc_List_List*_tmp4DD;_tmp333=((_tmp4DD=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4DD[0]=(struct Cyc_List_List)((_tmp4DE.hd=_tmp335,((_tmp4DE.tl=_tmp333,_tmp4DE)))),_tmp4DD))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4E1;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4E0;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp338=(_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0[0]=((_tmp4E1.tag=3,((_tmp4E1.f1=_tmp335,_tmp4E1)))),_tmp4E0)));
struct _dyneither_ptr*_tmp339=(*((struct _tuple8*)_tmp334->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4E2;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2)),((_tmp4E2->v=_tmp339,((_tmp4E2->b=(void*)_tmp338,((_tmp4E2->tl=locals,_tmp4E2))))))));};}}
# 1280
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp340=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp342;struct Cyc_Absyn_Tqual _tmp343;void*_tmp344;int _tmp345;struct Cyc_Absyn_VarargInfo _tmp341=_tmp340;_tmp342=_tmp341.name;_tmp343=_tmp341.tq;_tmp344=_tmp341.type;_tmp345=_tmp341.inject;
if(_tmp342 != 0){
void*_tmp346=Cyc_Absyn_dyneither_typ(_tmp344,param_rgn,_tmp343,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4E8;struct _tuple0*_tmp4E7;struct Cyc_Absyn_Vardecl*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp347=(_tmp4E6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4E6[0]=(struct Cyc_Absyn_Vardecl)((_tmp4E8.sc=Cyc_Absyn_Public,((_tmp4E8.name=(
(_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7->f1=Cyc_Absyn_Loc_n,((_tmp4E7->f2=_tmp342,_tmp4E7)))))),((_tmp4E8.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4E8.type=_tmp346,((_tmp4E8.initializer=0,((_tmp4E8.rgn=param_rgn,((_tmp4E8.attributes=0,((_tmp4E8.escapes=0,_tmp4E8)))))))))))))))),_tmp4E6)));
# 1291
{struct Cyc_List_List*_tmp4E9;_tmp333=((_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->hd=_tmp347,((_tmp4E9->tl=_tmp333,_tmp4E9))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4EC;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4EB;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp349=(_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB[0]=((_tmp4EC.tag=3,((_tmp4EC.f1=_tmp347,_tmp4EC)))),_tmp4EB)));
struct _dyneither_ptr*_tmp34A=_tmp342;
struct Cyc_Tcenv_Bindings*_tmp4ED;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED->v=_tmp34A,((_tmp4ED->b=(void*)_tmp349,((_tmp4ED->tl=locals,_tmp4ED))))))));};}else{
# 1296
const char*_tmp4F0;void*_tmp4EF;(_tmp4EF=0,Cyc_Tcutil_terr(loc,((_tmp4F0="missing name for varargs",_tag_dyneither(_tmp4F0,sizeof(char),25))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}}
# 1298
{struct Cyc_Core_Opt _tmp4F3;struct Cyc_Core_Opt*_tmp4F2;fd->param_vardecls=((_tmp4F2=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4F2[0]=(struct Cyc_Core_Opt)((_tmp4F3.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp333),_tmp4F3)),_tmp4F2))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp511;struct Cyc_List_List*_tmp510;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp50F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp50C;struct Cyc_Tcenv_CtrlEnv*_tmp50B;struct Cyc_Tcenv_SharedFenv*_tmp50A;struct Cyc_Tcenv_Fenv*_tmp509;return(_tmp509=_region_malloc(r,sizeof(*_tmp509)),((_tmp509->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A->frgn=r,((_tmp50A->return_typ=fd->ret_type,((_tmp50A->seen_labels=
# 1303
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp50A->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp50A->delayed_effect_checks=0,((_tmp50A->delayed_constraint_checks=0,_tmp50A)))))))))))))),((_tmp509->type_vars=(struct Cyc_List_List*)_tmp331,((_tmp509->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp332,((_tmp509->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp509->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp509->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1312
(_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B->ctrl_rgn=r,((_tmp50B->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp50B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp50B->fallthru_clause=0,((_tmp50B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp50B->try_depth=0,_tmp50B)))))))))))))),((_tmp509->capability=(void*)((void*)(
(_tmp511=_cycalloc(sizeof(*_tmp511)),((_tmp511[0]=((_tmp50C.tag=24,((_tmp50C.f1=((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510->hd=(void*)((_tmp50E=_cycalloc(sizeof(*_tmp50E)),((_tmp50E[0]=((_tmp50F.tag=23,((_tmp50F.f1=param_rgn,_tmp50F)))),_tmp50E)))),((_tmp510->tl=(
(_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D->hd=(void*)_check_null(fd->effect),((_tmp50D->tl=0,_tmp50D)))))),_tmp510)))))),_tmp50C)))),_tmp511))))),((_tmp509->curr_rgn=(void*)param_rgn,((_tmp509->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1316
((_tmp509->flags).in_notreadctxt=0,(((_tmp509->flags).in_lhs=0,(((_tmp509->flags).in_new=0,(((_tmp509->flags).abstract_ok=0,_tmp509->flags)))))))),((_tmp509->fnrgn=(struct _RegionHandle*)r,_tmp509)))))))))))))))))))));};};}
# 1321
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp367;struct Cyc_RgnOrder_RgnPO*_tmp368;struct Cyc_List_List*_tmp369;struct Cyc_Tcenv_SharedFenv*_tmp36A;struct _RegionHandle*_tmp36B;struct Cyc_Tcenv_Fenv*_tmp366=old_fenv;_tmp367=_tmp366->locals;_tmp368=_tmp366->region_order;_tmp369=_tmp366->type_vars;_tmp36A=_tmp366->shared;_tmp36B=_tmp366->fnrgn;{
# 1324
struct _RegionHandle*_tmp36C=_tmp36A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp36D=_tmp367;
struct _dyneither_ptr*_tmp51E;const char*_tmp51D;void*_tmp51C[1];struct Cyc_String_pa_PrintArg_struct _tmp51B;struct Cyc_Absyn_Tvar*_tmp51A;struct Cyc_Absyn_Tvar*rgn0=
(_tmp51A=_cycalloc(sizeof(*_tmp51A)),((_tmp51A->name=((_tmp51E=_cycalloc(sizeof(*_tmp51E)),((_tmp51E[0]=(struct _dyneither_ptr)((_tmp51B.tag=0,((_tmp51B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp51C[0]=& _tmp51B,Cyc_aprintf(((_tmp51D="`%s",_tag_dyneither(_tmp51D,sizeof(char),4))),_tag_dyneither(_tmp51C,sizeof(void*),1)))))))),_tmp51E)))),((_tmp51A->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp51A->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp51A)))))));
# 1330
{struct Cyc_List_List*_tmp36E=fd->tvs;for(0;_tmp36E != 0;_tmp36E=_tmp36E->tl){
struct Cyc_Absyn_Kind*_tmp36F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp36E->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp371;enum Cyc_Absyn_AliasQual _tmp372;struct Cyc_Absyn_Kind*_tmp370=_tmp36F;_tmp371=_tmp370->kind;_tmp372=_tmp370->aliasqual;
if(_tmp371 == Cyc_Absyn_RgnKind){
if(_tmp372 == Cyc_Absyn_Aliasable)
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,(struct Cyc_Absyn_Tvar*)_tmp36E->hd,0,0);else{
# 1336
const char*_tmp521;void*_tmp520;(_tmp520=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp521="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp521,sizeof(char),39))),_tag_dyneither(_tmp520,sizeof(void*),0)));}}}}
# 1338
_tmp368=Cyc_RgnOrder_add_youngest(_tmp36C,_tmp368,rgn0,0,0);{
struct Cyc_List_List*_tmp522;struct Cyc_List_List*_tmp375=(_tmp522=_cycalloc(sizeof(*_tmp522)),((_tmp522->hd=rgn0,((_tmp522->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp369),_tmp522)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp375);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp525;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp524;void*param_rgn=(void*)((_tmp524=_cycalloc(sizeof(*_tmp524)),((_tmp524[0]=((_tmp525.tag=2,((_tmp525.f1=rgn0,_tmp525)))),_tmp524))));
struct Cyc_List_List*_tmp376=0;
{struct Cyc_List_List*_tmp377=fd->args;for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct Cyc_Absyn_Vardecl _tmp52B;struct _tuple0*_tmp52A;struct Cyc_Absyn_Vardecl*_tmp529;struct Cyc_Absyn_Vardecl*_tmp378=(_tmp529=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp529[0]=(struct Cyc_Absyn_Vardecl)((_tmp52B.sc=Cyc_Absyn_Public,((_tmp52B.name=(
(_tmp52A=_cycalloc(sizeof(*_tmp52A)),((_tmp52A->f1=Cyc_Absyn_Loc_n,((_tmp52A->f2=(*((struct _tuple8*)_tmp377->hd)).f1,_tmp52A)))))),((_tmp52B.tq=(*((struct _tuple8*)_tmp377->hd)).f2,((_tmp52B.type=(*((struct _tuple8*)_tmp377->hd)).f3,((_tmp52B.initializer=0,((_tmp52B.rgn=param_rgn,((_tmp52B.attributes=0,((_tmp52B.escapes=0,_tmp52B)))))))))))))))),_tmp529)));
# 1351
{struct Cyc_List_List _tmp52E;struct Cyc_List_List*_tmp52D;_tmp376=((_tmp52D=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp52D[0]=(struct Cyc_List_List)((_tmp52E.hd=_tmp378,((_tmp52E.tl=_tmp376,_tmp52E)))),_tmp52D))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp531;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp530;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp37B=(_tmp530=_cycalloc(sizeof(*_tmp530)),((_tmp530[0]=((_tmp531.tag=3,((_tmp531.f1=_tmp378,_tmp531)))),_tmp530)));
struct _dyneither_ptr*_tmp37C=(*((struct _tuple8*)_tmp377->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp532;_tmp36D=(const struct Cyc_Tcenv_Bindings*)((_tmp532=_region_malloc(_tmp36C,sizeof(*_tmp532)),((_tmp532->v=_tmp37C,((_tmp532->b=(void*)_tmp37B,((_tmp532->tl=_tmp36D,_tmp532))))))));};}}
# 1356
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp383=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp385;struct Cyc_Absyn_Tqual _tmp386;void*_tmp387;int _tmp388;struct Cyc_Absyn_VarargInfo _tmp384=_tmp383;_tmp385=_tmp384.name;_tmp386=_tmp384.tq;_tmp387=_tmp384.type;_tmp388=_tmp384.inject;
if(_tmp385 != 0){
void*_tmp389=Cyc_Absyn_dyneither_typ(_tmp387,param_rgn,_tmp386,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp538;struct _tuple0*_tmp537;struct Cyc_Absyn_Vardecl*_tmp536;struct Cyc_Absyn_Vardecl*_tmp38A=(_tmp536=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp536[0]=(struct Cyc_Absyn_Vardecl)((_tmp538.sc=Cyc_Absyn_Public,((_tmp538.name=(
(_tmp537=_cycalloc(sizeof(*_tmp537)),((_tmp537->f1=Cyc_Absyn_Loc_n,((_tmp537->f2=_tmp385,_tmp537)))))),((_tmp538.tq=
Cyc_Absyn_empty_tqual(0),((_tmp538.type=_tmp389,((_tmp538.initializer=0,((_tmp538.rgn=param_rgn,((_tmp538.attributes=0,((_tmp538.escapes=0,_tmp538)))))))))))))))),_tmp536)));
# 1367
{struct Cyc_List_List*_tmp539;_tmp376=((_tmp539=_cycalloc(sizeof(*_tmp539)),((_tmp539->hd=_tmp38A,((_tmp539->tl=_tmp376,_tmp539))))));}{
struct _dyneither_ptr*_tmp38C=_tmp385;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp53C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp53B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp38D=(_tmp53B=_cycalloc(sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53C.tag=3,((_tmp53C.f1=_tmp38A,_tmp53C)))),_tmp53B)));
struct Cyc_Tcenv_Bindings*_tmp53D;_tmp36D=(const struct Cyc_Tcenv_Bindings*)((_tmp53D=_region_malloc(_tmp36C,sizeof(*_tmp53D)),((_tmp53D->v=_tmp38C,((_tmp53D->b=(void*)_tmp38D,((_tmp53D->tl=_tmp36D,_tmp53D))))))));};}else{
# 1372
const char*_tmp540;void*_tmp53F;(_tmp53F=0,Cyc_Tcutil_terr(loc,((_tmp540="missing name for varargs",_tag_dyneither(_tmp540,sizeof(char),25))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}}
# 1374
{struct Cyc_Core_Opt _tmp543;struct Cyc_Core_Opt*_tmp542;fd->param_vardecls=((_tmp542=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp542[0]=(struct Cyc_Core_Opt)((_tmp543.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp376),_tmp543)),_tmp542))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp561;struct Cyc_List_List*_tmp560;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp55F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp55E;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp55C;struct Cyc_Tcenv_CtrlEnv*_tmp55B;struct Cyc_Tcenv_SharedFenv*_tmp55A;struct Cyc_Tcenv_Fenv*_tmp559;return(_tmp559=_region_malloc(_tmp36C,sizeof(*_tmp559)),((_tmp559->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp55A=_region_malloc(_tmp36C,sizeof(*_tmp55A)),((_tmp55A->frgn=_tmp36C,((_tmp55A->return_typ=fd->ret_type,((_tmp55A->seen_labels=
# 1379
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp),((_tmp55A->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp36C,Cyc_strptrcmp),((_tmp55A->delayed_effect_checks=0,((_tmp55A->delayed_constraint_checks=0,_tmp55A)))))))))))))),((_tmp559->type_vars=(struct Cyc_List_List*)_tmp375,((_tmp559->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp368,((_tmp559->locals=(const struct Cyc_Tcenv_Bindings*)_tmp36D,((_tmp559->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp559->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1388
(_tmp55B=_region_malloc(_tmp36C,sizeof(*_tmp55B)),((_tmp55B->ctrl_rgn=_tmp36C,((_tmp55B->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp55B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp55B->fallthru_clause=0,((_tmp55B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp55B->try_depth=0,_tmp55B)))))))))))))),((_tmp559->capability=(void*)((void*)(
(_tmp561=_cycalloc(sizeof(*_tmp561)),((_tmp561[0]=((_tmp55C.tag=24,((_tmp55C.f1=((_tmp560=_cycalloc(sizeof(*_tmp560)),((_tmp560->hd=(void*)((_tmp55E=_cycalloc(sizeof(*_tmp55E)),((_tmp55E[0]=((_tmp55F.tag=23,((_tmp55F.f1=param_rgn,_tmp55F)))),_tmp55E)))),((_tmp560->tl=(
(_tmp55D=_cycalloc(sizeof(*_tmp55D)),((_tmp55D->hd=(void*)_check_null(fd->effect),((_tmp55D->tl=0,_tmp55D)))))),_tmp560)))))),_tmp55C)))),_tmp561))))),((_tmp559->curr_rgn=(void*)param_rgn,((_tmp559->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1392
((_tmp559->flags).in_notreadctxt=0,(((_tmp559->flags).in_lhs=0,(((_tmp559->flags).in_new=0,(((_tmp559->flags).abstract_ok=0,_tmp559->flags)))))))),((_tmp559->fnrgn=(struct _RegionHandle*)_tmp36B,_tmp559)))))))))))))))))))));};};};};}
