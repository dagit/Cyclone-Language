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
# 119
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 121
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 128
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 133
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 135
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 138
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 141
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 147
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 151
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 158
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 169
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 175
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 180
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 184
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 188
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 191
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 194
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 202
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 204
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 206
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 208
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 210
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 214
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 217
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 220
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
# 59
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 66
void*Cyc_Tcutil_compress(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 139
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 142
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 176
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 180
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 236 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 310
int Cyc_Tcutil_new_tvar_id();
# 312
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10]="Env_error";struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};
# 88 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
{const char*_tmp34D;void*_tmp34C[1];struct Cyc_String_pa_PrintArg_struct _tmp34B;(_tmp34B.tag=0,((_tmp34B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp34C[0]=& _tmp34B,Cyc_fprintf(Cyc_stderr,((_tmp34D="Internal error in tcenv: %s\n",_tag_dyneither(_tmp34D,sizeof(char),29))),_tag_dyneither(_tmp34C,sizeof(void*),1)))))));}
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
struct Cyc_Tcenv_Genv*_tmp34E;return(_tmp34E=_region_malloc(r,sizeof(*_tmp34E)),((_tmp34E->grgn=r,((_tmp34E->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp34E->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp34E->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp34E->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp34E->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp34E->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp34E->availables=0,_tmp34E)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 250
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 252
{struct Cyc_Core_Opt*_tmp34F;Cyc_Tcutil_empty_var_set=((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp34F))));}{
# 254
struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp350;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp350=_cycalloc(sizeof(*_tmp350)),((_tmp350[0]=Cyc_Absyn_exn_tud,_tmp350)))));}
{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Datatypedecl*)_check_null(Cyc_Absyn_exn_tud))->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp356;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp355;struct _tuple12*_tmp354;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,(
(_tmp354=_region_malloc(r,sizeof(*_tmp354)),((_tmp354->f1=(void*)(
(_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=((_tmp355.tag=2,((_tmp355.f1=Cyc_Absyn_exn_tud,((_tmp355.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp355)))))),_tmp356)))),((_tmp354->f2=1,_tmp354)))))));}}{
# 261
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmpA);
struct Cyc_Tcenv_Tenv*_tmp357;return(_tmp357=_region_malloc(r,sizeof(*_tmp357)),((_tmp357->ns=0,((_tmp357->ae=ae,((_tmp357->le=0,((_tmp357->allow_valueof=0,_tmp357)))))))));};};}
# 267
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 269
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 273
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp358;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp358="outer_namespace",_tag_dyneither(_tmp358,sizeof(char),16))));}
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
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))_tmp14=1;if(!_tmp14){
lookup(ge2,v,0);{
const char*_tmp35C;void*_tmp35B[1];struct Cyc_String_pa_PrintArg_struct _tmp35A;(_tmp35A.tag=0,((_tmp35A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp35B[0]=& _tmp35A,Cyc_Tcutil_terr(loc,((_tmp35C="%s is ambiguous",_tag_dyneither(_tmp35C,sizeof(char),16))),_tag_dyneither(_tmp35B,sizeof(void*),1)))))));};
# 301
;_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;void*_tmp1B;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp1A=(struct Cyc_Dict_Absent_exn_struct*)_tmp19;if(_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 303
 goto _LL0;_LL3: _tmp1B=_tmp19;_LL4:(void)_throw(_tmp1B);_LL0:;}};}}
# 305
return;}
# 308
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 313
{struct _handler_cons _tmp1C;_push_handler(& _tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1F=result;_npop_handler(0);return _tmp1F;};}
# 314
;_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;void*_tmp23;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp22=(struct Cyc_Dict_Absent_exn_struct*)_tmp21;if(_tmp22->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 317
 goto _LL5;_LL8: _tmp23=_tmp21;_LL9:(void)_throw(_tmp23);_LL5:;}};}
# 319
{struct Cyc_List_List*_tmp24=((struct Cyc_Tcenv_Genv*)_check_null(ge))->availables;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp24->hd);
struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp24->hd,_tmp24->tl);{
void*_tmp28=result;_npop_handler(0);return _tmp28;};}
# 322
;_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp2A=_tmp26;void*_tmp2C;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp2B=(struct Cyc_Dict_Absent_exn_struct*)_tmp2A;if(_tmp2B->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 325
 goto _LLA;_LLD: _tmp2C=_tmp2A;_LLE:(void)_throw(_tmp2C);_LLA:;}};}}
# 327
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp35F;struct Cyc_Dict_Absent_exn_struct*_tmp35E;(int)_throw((void*)((_tmp35E=_cycalloc_atomic(sizeof(*_tmp35E)),((_tmp35E[0]=((_tmp35F.tag=Cyc_Dict_Absent,_tmp35F)),_tmp35E)))));}
# 330
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 334
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2F=((struct Cyc_Tcenv_Genv*)_check_null(ge))->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2F,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 341
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp31;struct _dyneither_ptr*_tmp32;struct _tuple0*_tmp30=q;_tmp31=_tmp30->f1;_tmp32=_tmp30->f2;{
union Cyc_Absyn_Nmspace _tmp33=_tmp31;struct _dyneither_ptr*_tmp34;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp37;_LL10: if((_tmp33.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp33.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL14;_LL13:
# 346
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp32,is_use);_LL14: if((_tmp33.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp33.Rel_n).val == 0)goto _LL16;_tmp34=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp33.Rel_n).val)->hd;_tmp35=((struct Cyc_List_List*)(_tmp33.Rel_n).val)->tl;_LL15: {
# 348
struct Cyc_Tcenv_Genv*_tmp38=Cyc_Tcenv_lookup_namespace(te,loc,_tmp34,_tmp35);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp38,_tmp32,is_use);}_LL16: if((_tmp33.C_n).tag != 3)goto _LL18;_tmp36=(struct Cyc_List_List*)(_tmp33.C_n).val;_LL17:
 _tmp37=_tmp36;goto _LL19;_LL18: if((_tmp33.Abs_n).tag != 2)goto _LLF;_tmp37=(struct Cyc_List_List*)(_tmp33.Abs_n).val;_LL19:
# 352
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37),_tmp32,is_use);_LLF:;};}
# 357
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 359
struct Cyc_List_List*_tmp39=((struct Cyc_Tcenv_Tenv*)_check_null(te))->ns;
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_List_List*_tmp3B=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp3C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp39);
struct Cyc_List_List*_tmp3D=((struct Cyc_Tcenv_Genv*)_check_null(_tmp3C))->availables;
struct Cyc_Set_Set*_tmp3E=((struct Cyc_Tcenv_Genv*)_check_null(_tmp3C))->namespaces;
# 367
{struct Cyc_List_List*_tmp3F=_tmp3D;for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){
struct Cyc_Set_Set*_tmp40=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,(struct Cyc_List_List*)((struct Cyc_List_List*)_check_null(_tmp3F))->hd)))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){
struct Cyc_List_List*_tmp362;struct Cyc_List_List*_tmp361;_tmp3B=((_tmp361=_region_malloc(temp,sizeof(*_tmp361)),((_tmp361->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362->hd=n,((_tmp362->tl=ns,_tmp362))))))),((_tmp361->tl=_tmp3B,_tmp361))))));}}}
# 374
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3E,n)){
struct Cyc_List_List*_tmp365;struct Cyc_List_List*_tmp364;_tmp3B=((_tmp364=_region_malloc(temp,sizeof(*_tmp364)),((_tmp364->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->hd=n,((_tmp365->tl=ns,_tmp365))))))),((_tmp364->tl=_tmp3B,_tmp364))))));}
# 380
if(_tmp3B != 0){
if(_tmp3B->tl != 0){
const char*_tmp369;void*_tmp368[1];struct Cyc_String_pa_PrintArg_struct _tmp367;(_tmp367.tag=0,((_tmp367.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp368[0]=& _tmp367,Cyc_Tcutil_terr(loc,((_tmp369="%s is ambiguous",_tag_dyneither(_tmp369,sizeof(char),16))),_tag_dyneither(_tmp368,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp48=(struct Cyc_List_List*)_tmp3B->hd;_npop_handler(0);return _tmp48;};}
# 385
if(_tmp39 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp36C;struct Cyc_Dict_Absent_exn_struct*_tmp36B;(int)_throw((void*)((_tmp36B=_cycalloc_atomic(sizeof(*_tmp36B)),((_tmp36B[0]=((_tmp36C.tag=Cyc_Dict_Absent,_tmp36C)),_tmp36B)))));}
# 388
_tmp39=Cyc_Tcenv_outer_namespace(_tmp39);}}
# 361
;_pop_region(temp);}
# 396
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4B=((struct Cyc_Tcenv_Genv*)_check_null(ge))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,v);}
# 400
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4D;struct _dyneither_ptr*_tmp4E;struct _tuple0*_tmp4C=q;_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;{
union Cyc_Absyn_Nmspace _tmp4F=_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;struct _dyneither_ptr*_tmp52;struct Cyc_List_List*_tmp53;_LL1B: if((_tmp4F.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4F.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val != 0)goto _LL1F;_LL1E:
# 405
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4E,0);_LL1F: if((_tmp4F.C_n).tag != 3)goto _LL21;_tmp50=(struct Cyc_List_List*)(_tmp4F.C_n).val;_LL20:
 _tmp51=_tmp50;goto _LL22;_LL21: if((_tmp4F.Abs_n).tag != 2)goto _LL23;_tmp51=(struct Cyc_List_List*)(_tmp4F.Abs_n).val;_LL22: {
# 408
struct Cyc_Dict_Dict _tmp54=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp51)))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4E);}_LL23: if((_tmp4F.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val == 0)goto _LL1A;_tmp52=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->hd;_tmp53=((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->tl;_LL24: {
# 411
struct Cyc_Dict_Dict _tmp55=((struct Cyc_Tcenv_Genv*)_check_null(Cyc_Tcenv_lookup_namespace(te,loc,_tmp52,_tmp53)))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp55,_tmp4E);}_LL1A:;};}
# 416
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp56=((struct Cyc_Tcenv_Genv*)_check_null(ge))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,v);}
# 420
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp59;struct _tuple0*_tmp57=q;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;{
union Cyc_Absyn_Nmspace _tmp5A=_tmp58;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;_LL26: if((_tmp5A.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp5A.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val != 0)goto _LL2A;_LL29:
# 425
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp59,0);_LL2A: if((_tmp5A.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val == 0)goto _LL2C;_tmp5B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->hd;_tmp5C=((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->tl;_LL2B: {
# 427
struct Cyc_Dict_Dict _tmp5F=((struct Cyc_Tcenv_Genv*)_check_null(Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C)))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5F,_tmp59);}_LL2C: if((_tmp5A.C_n).tag != 3)goto _LL2E;_tmp5D=(struct Cyc_List_List*)(_tmp5A.C_n).val;_LL2D:
 _tmp5E=_tmp5D;goto _LL2F;_LL2E: if((_tmp5A.Abs_n).tag != 2)goto _LL25;_tmp5E=(struct Cyc_List_List*)(_tmp5A.Abs_n).val;_LL2F: {
# 431
struct Cyc_Dict_Dict _tmp60=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp5E)))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp60,_tmp59);}_LL25:;};}
# 436
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 441
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp62;struct _dyneither_ptr*_tmp63;struct _tuple0*_tmp61=q;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
union Cyc_Absyn_Nmspace _tmp64=_tmp62;struct _dyneither_ptr*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp68;_LL31: if((_tmp64.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp64.Rel_n).val != 0)goto _LL33;_LL32: {
# 446
struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){{struct Cyc_Absyn_Datatypedecl***_tmp36D;struct Cyc_Absyn_Datatypedecl***_tmp6D=(_tmp36D=_region_malloc(r,sizeof(*_tmp36D)),((_tmp36D[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp63,0),_tmp36D)));_npop_handler(0);return _tmp6D;};_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6F=_tmp6A;void*_tmp71;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp70=(struct Cyc_Dict_Absent_exn_struct*)_tmp6F;if(_tmp70->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp71=_tmp6F;_LL3F:(void)_throw(_tmp71);_LL3B:;}};}_LL33: if((_tmp64.Loc_n).tag != 4)goto _LL35;_LL34:
# 449
{const char*_tmp370;void*_tmp36F;(_tmp36F=0,Cyc_Tcutil_terr(loc,((_tmp370="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp370,sizeof(char),33))),_tag_dyneither(_tmp36F,sizeof(void*),0)));}
return 0;_LL35: if((_tmp64.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp64.Rel_n).val == 0)goto _LL37;_tmp65=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp64.Rel_n).val)->hd;_tmp66=((struct Cyc_List_List*)(_tmp64.Rel_n).val)->tl;_LL36: {
# 456
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))_tmp76=1;if(!_tmp76){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp65,_tmp66);;_pop_handler();}else{void*_tmp75=(void*)_exn_thrown;void*_tmp78=_tmp75;void*_tmp7A;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 460
{const char*_tmp373;void*_tmp372;(_tmp372=0,Cyc_Tcutil_terr(loc,((_tmp373="bad qualified name for @extensible datatype",_tag_dyneither(_tmp373,sizeof(char),44))),_tag_dyneither(_tmp372,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp376;struct Cyc_Dict_Absent_exn_struct*_tmp375;(int)_throw((void*)((_tmp375=_cycalloc_atomic(sizeof(*_tmp375)),((_tmp375[0]=((_tmp376.tag=Cyc_Dict_Absent,_tmp376)),_tmp375)))));};_LL43: _tmp7A=_tmp78;_LL44:(void)_throw(_tmp7A);_LL40:;}};}{
# 463
struct Cyc_Dict_Dict _tmp7F=((struct Cyc_Tcenv_Genv*)_check_null(ge))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp377;return(_tmp377=_region_malloc(r,sizeof(*_tmp377)),((_tmp377[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp63),_tmp377)));};}_LL37: if((_tmp64.C_n).tag != 3)goto _LL39;_tmp67=(struct Cyc_List_List*)(_tmp64.C_n).val;_LL38:
 _tmp68=_tmp67;goto _LL3A;_LL39: if((_tmp64.Abs_n).tag != 2)goto _LL30;_tmp68=(struct Cyc_List_List*)(_tmp64.Abs_n).val;_LL3A: {
# 467
struct Cyc_Dict_Dict _tmp81=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp68)))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp378;return(_tmp378=_region_malloc(r,sizeof(*_tmp378)),((_tmp378[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,_tmp63),_tmp378)));}_LL30:;};}
# 472
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp83=((struct Cyc_Tcenv_Genv*)_check_null(ge))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp83,v);}
# 476
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp84=q;_tmp85=_tmp84->f1;_tmp86=_tmp84->f2;{
union Cyc_Absyn_Nmspace _tmp87=_tmp85;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;_LL46: if((_tmp87.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp87.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp87.Rel_n).val != 0)goto _LL4A;_LL49:
# 481
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp86,0);_LL4A: if((_tmp87.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp87.Rel_n).val == 0)goto _LL4C;_tmp88=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp87.Rel_n).val)->hd;_tmp89=((struct Cyc_List_List*)(_tmp87.Rel_n).val)->tl;_LL4B: {
# 483
struct Cyc_Dict_Dict _tmp8C=((struct Cyc_Tcenv_Genv*)_check_null(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89)))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8C,_tmp86);}_LL4C: if((_tmp87.C_n).tag != 3)goto _LL4E;_tmp8A=(struct Cyc_List_List*)(_tmp87.C_n).val;_LL4D:
 _tmp8B=_tmp8A;goto _LL4F;_LL4E: if((_tmp87.Abs_n).tag != 2)goto _LL45;_tmp8B=(struct Cyc_List_List*)(_tmp87.Abs_n).val;_LL4F: {
# 487
struct Cyc_Dict_Dict _tmp8D=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp8B)))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,_tmp86);}_LL45:;};}
# 492
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8E=((struct Cyc_Tcenv_Genv*)_check_null(ge))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8E,v);}
# 496
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*_tmp8F=q;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{
union Cyc_Absyn_Nmspace _tmp92=_tmp90;struct _dyneither_ptr*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;_LL51: if((_tmp92.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp92.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp92.Rel_n).val != 0)goto _LL55;_LL54:
# 501
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp91,0);_LL55: if((_tmp92.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp92.Rel_n).val == 0)goto _LL57;_tmp93=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp92.Rel_n).val)->hd;_tmp94=((struct Cyc_List_List*)(_tmp92.Rel_n).val)->tl;_LL56: {
# 503
struct Cyc_Dict_Dict _tmp97=((struct Cyc_Tcenv_Genv*)_check_null(Cyc_Tcenv_lookup_namespace(te,loc,_tmp93,_tmp94)))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp97,_tmp91);}_LL57: if((_tmp92.C_n).tag != 3)goto _LL59;_tmp95=(struct Cyc_List_List*)(_tmp92.C_n).val;_LL58:
 _tmp96=_tmp95;goto _LL5A;_LL59: if((_tmp92.Abs_n).tag != 2)goto _LL50;_tmp96=(struct Cyc_List_List*)(_tmp92.Abs_n).val;_LL5A: {
# 507
struct Cyc_Dict_Dict _tmp98=((struct Cyc_Tcenv_Genv*)_check_null(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Tcenv_Tenv*)_check_null(te))->ae,_tmp96)))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp98,_tmp91);}_LL50:;};}
# 514
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp99=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp99);}
# 520
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 524
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp9A=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp9A != 0){
struct Cyc_Tcenv_Fenv*_stmttmp0=_tmp9A;struct _RegionHandle*_tmp9C;struct Cyc_Tcenv_Fenv*_tmp9B=_stmttmp0;_tmp9C=_tmp9B->fnrgn;
return _tmp9C;}
# 530
return Cyc_Tcenv_coerce_heap_region();}
# 533
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp379;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp379="put_fenv",_tag_dyneither(_tmp379,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp37A;return(_tmp37A=_region_malloc(l,sizeof(*_tmp37A)),((_tmp37A->ns=te->ns,((_tmp37A->ae=te->ae,((_tmp37A->le=fe,((_tmp37A->allow_valueof=te->allow_valueof,_tmp37A)))))))));};}
# 538
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp37B;return(_tmp37B=_region_malloc(l,sizeof(*_tmp37B)),((_tmp37B->ns=te->ns,((_tmp37B->ae=te->ae,((_tmp37B->le=0,((_tmp37B->allow_valueof=te->allow_valueof,_tmp37B)))))))));}
# 542
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 544
struct Cyc_Tcenv_Fenv _stmttmp1=*f;struct Cyc_Tcenv_SharedFenv*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_RgnOrder_RgnPO*_tmpA3;const struct Cyc_Tcenv_Bindings*_tmpA4;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Tcenv_CtrlEnv*_tmpA6;void*_tmpA7;void*_tmpA8;struct Cyc_Tcenv_FenvFlags _tmpA9;struct _RegionHandle*_tmpAA;struct Cyc_Tcenv_Fenv _tmpA0=_stmttmp1;_tmpA1=_tmpA0.shared;_tmpA2=_tmpA0.type_vars;_tmpA3=_tmpA0.region_order;_tmpA4=_tmpA0.locals;_tmpA5=_tmpA0.encloser;_tmpA6=_tmpA0.ctrl_env;_tmpA7=_tmpA0.capability;_tmpA8=_tmpA0.curr_rgn;_tmpA9=_tmpA0.flags;_tmpAA=_tmpA0.fnrgn;{
# 546
struct Cyc_Tcenv_Fenv*_tmp37C;return(_tmp37C=_region_malloc(l,sizeof(*_tmp37C)),((_tmp37C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA1,((_tmp37C->type_vars=(struct Cyc_List_List*)_tmpA2,((_tmp37C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA3,((_tmp37C->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA4,((_tmp37C->encloser=(struct Cyc_Absyn_Stmt*)_tmpA5,((_tmp37C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA6,((_tmp37C->capability=(void*)_tmpA7,((_tmp37C->curr_rgn=(void*)_tmpA8,((_tmp37C->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA9,((_tmp37C->fnrgn=(struct _RegionHandle*)l,_tmp37C)))))))))))))))))))));};}
# 550
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 552
struct Cyc_Tcenv_Fenv _stmttmp2=*f;struct Cyc_Tcenv_SharedFenv*_tmpAD;struct Cyc_List_List*_tmpAE;struct Cyc_RgnOrder_RgnPO*_tmpAF;const struct Cyc_Tcenv_Bindings*_tmpB0;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_Tcenv_CtrlEnv*_tmpB2;void*_tmpB3;void*_tmpB4;struct Cyc_Tcenv_FenvFlags _tmpB5;struct Cyc_Tcenv_Fenv _tmpAC=_stmttmp2;_tmpAD=_tmpAC.shared;_tmpAE=_tmpAC.type_vars;_tmpAF=_tmpAC.region_order;_tmpB0=_tmpAC.locals;_tmpB1=_tmpAC.encloser;_tmpB2=_tmpAC.ctrl_env;_tmpB3=_tmpAC.capability;_tmpB4=_tmpAC.curr_rgn;_tmpB5=_tmpAC.flags;{
# 555
struct Cyc_Tcenv_CtrlEnv _stmttmp3=*_tmpB2;struct _RegionHandle*_tmpB7;void*_tmpB8;void*_tmpB9;const struct _tuple10*_tmpBA;void*_tmpBB;int _tmpBC;struct Cyc_Tcenv_CtrlEnv _tmpB6=_stmttmp3;_tmpB7=_tmpB6.ctrl_rgn;_tmpB8=_tmpB6.continue_stmt;_tmpB9=_tmpB6.break_stmt;_tmpBA=_tmpB6.fallthru_clause;_tmpBB=_tmpB6.next_stmt;_tmpBC=_tmpB6.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp37D;struct Cyc_Tcenv_CtrlEnv*_tmpBD=
(_tmp37D=_region_malloc(l,sizeof(*_tmp37D)),((_tmp37D->ctrl_rgn=_tmpB7,((_tmp37D->continue_stmt=_tmpB8,((_tmp37D->break_stmt=_tmpB9,((_tmp37D->fallthru_clause=_tmpBA,((_tmp37D->next_stmt=_tmpBB,((_tmp37D->try_depth=_tmpBC,_tmp37D)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp37E;return(_tmp37E=_region_malloc(l,sizeof(*_tmp37E)),((_tmp37E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAD,((_tmp37E->type_vars=(struct Cyc_List_List*)_tmpAE,((_tmp37E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAF,((_tmp37E->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB0,((_tmp37E->encloser=(struct Cyc_Absyn_Stmt*)_tmpB1,((_tmp37E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBD,((_tmp37E->capability=(void*)_tmpB3,((_tmp37E->curr_rgn=(void*)_tmpB4,((_tmp37E->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB5,((_tmp37E->fnrgn=(struct _RegionHandle*)l,_tmp37E)))))))))))))))))))));};};}
# 564
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp37F;struct Cyc_Tcenv_Fenv*_stmttmp4=Cyc_Tcenv_get_fenv(te,((_tmp37F="return_typ",_tag_dyneither(_tmp37F,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpC1;struct Cyc_Tcenv_Fenv*_tmpC0=_stmttmp4;_tmpC1=_tmpC0->shared;{
struct Cyc_Tcenv_SharedFenv _stmttmp5=*_tmpC1;void*_tmpC3;struct Cyc_Tcenv_SharedFenv _tmpC2=_stmttmp5;_tmpC3=_tmpC2.return_typ;
return _tmpC3;};}
# 570
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC5=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _stmttmp6=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC5));struct Cyc_List_List*_tmpC7;struct Cyc_Tcenv_Fenv _tmpC6=_stmttmp6;_tmpC7=_tmpC6.type_vars;
return _tmpC7;};}
# 577
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp380;return(_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380->v=Cyc_Tcenv_lookup_type_vars(te),_tmp380)));}
# 581
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 583
const char*_tmp381;struct Cyc_Tcenv_Fenv*_stmttmp7=
# 585
Cyc_Tcenv_get_fenv(te,((_tmp381="add_type_vars",_tag_dyneither(_tmp381,sizeof(char),14))));
# 583
struct Cyc_Tcenv_SharedFenv*_tmpCA;struct Cyc_List_List*_tmpCB;struct Cyc_RgnOrder_RgnPO*_tmpCC;const struct Cyc_Tcenv_Bindings*_tmpCD;struct Cyc_Absyn_Stmt*_tmpCE;struct Cyc_Tcenv_CtrlEnv*_tmpCF;void*_tmpD0;void*_tmpD1;struct Cyc_Tcenv_FenvFlags _tmpD2;struct Cyc_Tcenv_Fenv*_tmpC9=_stmttmp7;_tmpCA=_tmpC9->shared;_tmpCB=_tmpC9->type_vars;_tmpCC=_tmpC9->region_order;_tmpCD=_tmpC9->locals;_tmpCE=_tmpC9->encloser;_tmpCF=_tmpC9->ctrl_env;_tmpD0=_tmpC9->capability;_tmpD1=_tmpC9->curr_rgn;_tmpD2=_tmpC9->flags;
# 587
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpCB);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD3);{
struct Cyc_Tcenv_Fenv*_tmp382;struct Cyc_Tcenv_Fenv*_tmpD4=(_tmp382=_region_malloc(r,sizeof(*_tmp382)),((_tmp382->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCA,((_tmp382->type_vars=(struct Cyc_List_List*)_tmpD3,((_tmp382->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpCC,((_tmp382->locals=(const struct Cyc_Tcenv_Bindings*)_tmpCD,((_tmp382->encloser=(struct Cyc_Absyn_Stmt*)_tmpCE,((_tmp382->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCF,((_tmp382->capability=(void*)_tmpD0,((_tmp382->curr_rgn=(void*)_tmpD1,((_tmp382->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD2,((_tmp382->fnrgn=(struct _RegionHandle*)r,_tmp382)))))))))))))))))))));
# 592
return Cyc_Tcenv_put_fenv(r,te,_tmpD4);};};}
# 595
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD7=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmpD7 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 600
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpD7);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 605
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 607
struct _dyneither_ptr*_tmpD8=(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name))).f2;
{union Cyc_Absyn_Nmspace _stmttmp8=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD9=_stmttmp8;_LL5C: if((_tmpD9.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 612
struct Cyc_Core_Impossible_exn_struct _tmp388;const char*_tmp387;struct Cyc_Core_Impossible_exn_struct*_tmp386;(int)_throw((void*)((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386[0]=((_tmp388.tag=Cyc_Core_Impossible,((_tmp388.f1=((_tmp387="add_local_var: called with Rel_n",_tag_dyneither(_tmp387,sizeof(char),33))),_tmp388)))),_tmp386)))));}_LL5B:;}{
# 614
const char*_tmp389;struct Cyc_Tcenv_Fenv*_stmttmp9=
# 616
Cyc_Tcenv_get_fenv(te,((_tmp389="add_local_var",_tag_dyneither(_tmp389,sizeof(char),14))));
# 614
struct Cyc_Tcenv_SharedFenv*_tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_RgnOrder_RgnPO*_tmpE0;const struct Cyc_Tcenv_Bindings*_tmpE1;struct Cyc_Absyn_Stmt*_tmpE2;struct Cyc_Tcenv_CtrlEnv*_tmpE3;void*_tmpE4;void*_tmpE5;struct Cyc_Tcenv_FenvFlags _tmpE6;struct Cyc_Tcenv_Fenv*_tmpDD=_stmttmp9;_tmpDE=_tmpDD->shared;_tmpDF=_tmpDD->type_vars;_tmpE0=_tmpDD->region_order;_tmpE1=_tmpDD->locals;_tmpE2=_tmpDD->encloser;_tmpE3=_tmpDD->ctrl_env;_tmpE4=_tmpDD->capability;_tmpE5=_tmpDD->curr_rgn;_tmpE6=_tmpDD->flags;{
# 617
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp38C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp38B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE7=(_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=((_tmp38C.tag=4,((_tmp38C.f1=vd,_tmp38C)))),_tmp38B)));
struct Cyc_Tcenv_Bindings*_tmp38D;struct Cyc_Tcenv_Bindings*_tmpE8=(_tmp38D=_region_malloc(r,sizeof(*_tmp38D)),((_tmp38D->v=_tmpD8,((_tmp38D->b=(void*)_tmpE7,((_tmp38D->tl=_tmpE1,_tmp38D)))))));
struct Cyc_Tcenv_Fenv*_tmp38E;struct Cyc_Tcenv_Fenv*_tmpE9=(_tmp38E=_region_malloc(r,sizeof(*_tmp38E)),((_tmp38E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpDE,((_tmp38E->type_vars=(struct Cyc_List_List*)_tmpDF,((_tmp38E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE0,((_tmp38E->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpE8),((_tmp38E->encloser=(struct Cyc_Absyn_Stmt*)_tmpE2,((_tmp38E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpE3,((_tmp38E->capability=(void*)_tmpE4,((_tmp38E->curr_rgn=(void*)_tmpE5,((_tmp38E->flags=(struct Cyc_Tcenv_FenvFlags)_tmpE6,((_tmp38E->fnrgn=(struct _RegionHandle*)r,_tmp38E)))))))))))))))))))));
# 621
return Cyc_Tcenv_put_fenv(r,te,_tmpE9);};};}
# 624
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 626
struct Cyc_Tcenv_Fenv*_tmpEF=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmpEF == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpA=*_tmpEF;struct Cyc_Tcenv_SharedFenv*_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_RgnOrder_RgnPO*_tmpF3;const struct Cyc_Tcenv_Bindings*_tmpF4;struct Cyc_Absyn_Stmt*_tmpF5;struct Cyc_Tcenv_CtrlEnv*_tmpF6;void*_tmpF7;void*_tmpF8;int _tmpF9;int _tmpFA;int _tmpFB;struct Cyc_Tcenv_Fenv _tmpF0=_stmttmpA;_tmpF1=_tmpF0.shared;_tmpF2=_tmpF0.type_vars;_tmpF3=_tmpF0.region_order;_tmpF4=_tmpF0.locals;_tmpF5=_tmpF0.encloser;_tmpF6=_tmpF0.ctrl_env;_tmpF7=_tmpF0.capability;_tmpF8=_tmpF0.curr_rgn;_tmpF9=(_tmpF0.flags).in_notreadctxt;_tmpFA=(_tmpF0.flags).in_lhs;_tmpFB=(_tmpF0.flags).abstract_ok;{
# 631
struct Cyc_Tcenv_Fenv*_tmp38F;struct Cyc_Tcenv_Fenv*_tmpFC=(_tmp38F=_region_malloc(r,sizeof(*_tmp38F)),((_tmp38F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF1,((_tmp38F->type_vars=(struct Cyc_List_List*)_tmpF2,((_tmp38F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF3,((_tmp38F->locals=(const struct Cyc_Tcenv_Bindings*)_tmpF4,((_tmp38F->encloser=(struct Cyc_Absyn_Stmt*)_tmpF5,((_tmp38F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF6,((_tmp38F->capability=(void*)_tmpF7,((_tmp38F->curr_rgn=(void*)_tmpF8,((_tmp38F->flags=(struct Cyc_Tcenv_FenvFlags)(
# 633
((_tmp38F->flags).in_notreadctxt=_tmpF9,(((_tmp38F->flags).in_lhs=_tmpFA,(((_tmp38F->flags).in_new=(int)status,(((_tmp38F->flags).abstract_ok=_tmpFB,_tmp38F->flags)))))))),((_tmp38F->fnrgn=(struct _RegionHandle*)r,_tmp38F)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmpFC);};};}
# 637
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpFE=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmpFE == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_stmttmpB=_tmpFE;int _tmp100;struct Cyc_Tcenv_Fenv*_tmpFF=_stmttmpB;_tmp100=(_tmpFF->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp100;};}
# 644
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp101=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp101 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmpC=_tmp101;int _tmp103;struct Cyc_Tcenv_Fenv*_tmp102=_stmttmpC;_tmp103=(_tmp102->flags).abstract_ok;
return _tmp103;};}
# 651
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 653
struct Cyc_Tcenv_Fenv*_tmp104=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp104 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpD=*_tmp104;struct Cyc_Tcenv_SharedFenv*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_RgnOrder_RgnPO*_tmp108;const struct Cyc_Tcenv_Bindings*_tmp109;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Tcenv_CtrlEnv*_tmp10B;void*_tmp10C;void*_tmp10D;int _tmp10E;int _tmp10F;int _tmp110;struct Cyc_Tcenv_Fenv _tmp105=_stmttmpD;_tmp106=_tmp105.shared;_tmp107=_tmp105.type_vars;_tmp108=_tmp105.region_order;_tmp109=_tmp105.locals;_tmp10A=_tmp105.encloser;_tmp10B=_tmp105.ctrl_env;_tmp10C=_tmp105.capability;_tmp10D=_tmp105.curr_rgn;_tmp10E=(_tmp105.flags).in_notreadctxt;_tmp10F=(_tmp105.flags).in_lhs;_tmp110=(_tmp105.flags).in_new;{
# 658
struct Cyc_Tcenv_Fenv*_tmp390;struct Cyc_Tcenv_Fenv*_tmp111=(_tmp390=_region_malloc(r,sizeof(*_tmp390)),((_tmp390->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp106,((_tmp390->type_vars=(struct Cyc_List_List*)_tmp107,((_tmp390->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp108,((_tmp390->locals=(const struct Cyc_Tcenv_Bindings*)_tmp109,((_tmp390->encloser=(struct Cyc_Absyn_Stmt*)_tmp10A,((_tmp390->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10B,((_tmp390->capability=(void*)_tmp10C,((_tmp390->curr_rgn=(void*)_tmp10D,((_tmp390->flags=(struct Cyc_Tcenv_FenvFlags)(
# 660
((_tmp390->flags).in_notreadctxt=_tmp10E,(((_tmp390->flags).in_lhs=_tmp10F,(((_tmp390->flags).in_new=_tmp110,(((_tmp390->flags).abstract_ok=1,_tmp390->flags)))))))),((_tmp390->fnrgn=(struct _RegionHandle*)r,_tmp390)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp111);};};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp113=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp113 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpE=*_tmp113;struct Cyc_Tcenv_SharedFenv*_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_RgnOrder_RgnPO*_tmp117;const struct Cyc_Tcenv_Bindings*_tmp118;struct Cyc_Absyn_Stmt*_tmp119;struct Cyc_Tcenv_CtrlEnv*_tmp11A;void*_tmp11B;void*_tmp11C;int _tmp11D;int _tmp11E;int _tmp11F;struct Cyc_Tcenv_Fenv _tmp114=_stmttmpE;_tmp115=_tmp114.shared;_tmp116=_tmp114.type_vars;_tmp117=_tmp114.region_order;_tmp118=_tmp114.locals;_tmp119=_tmp114.encloser;_tmp11A=_tmp114.ctrl_env;_tmp11B=_tmp114.capability;_tmp11C=_tmp114.curr_rgn;_tmp11D=(_tmp114.flags).in_notreadctxt;_tmp11E=(_tmp114.flags).in_lhs;_tmp11F=(_tmp114.flags).in_new;{
# 671
struct Cyc_Tcenv_Fenv*_tmp391;struct Cyc_Tcenv_Fenv*_tmp120=(_tmp391=_region_malloc(r,sizeof(*_tmp391)),((_tmp391->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp115,((_tmp391->type_vars=(struct Cyc_List_List*)_tmp116,((_tmp391->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp117,((_tmp391->locals=(const struct Cyc_Tcenv_Bindings*)_tmp118,((_tmp391->encloser=(struct Cyc_Absyn_Stmt*)_tmp119,((_tmp391->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11A,((_tmp391->capability=(void*)_tmp11B,((_tmp391->curr_rgn=(void*)_tmp11C,((_tmp391->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp391->flags).in_notreadctxt=_tmp11D,(((_tmp391->flags).in_lhs=_tmp11E,(((_tmp391->flags).in_new=_tmp11F,(((_tmp391->flags).abstract_ok=0,_tmp391->flags)))))))),((_tmp391->fnrgn=(struct _RegionHandle*)r,_tmp391)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp120);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp122=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp122 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpF=*_tmp122;struct Cyc_Tcenv_SharedFenv*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_RgnOrder_RgnPO*_tmp126;const struct Cyc_Tcenv_Bindings*_tmp127;struct Cyc_Absyn_Stmt*_tmp128;struct Cyc_Tcenv_CtrlEnv*_tmp129;void*_tmp12A;void*_tmp12B;int _tmp12C;int _tmp12D;int _tmp12E;struct Cyc_Tcenv_Fenv _tmp123=_stmttmpF;_tmp124=_tmp123.shared;_tmp125=_tmp123.type_vars;_tmp126=_tmp123.region_order;_tmp127=_tmp123.locals;_tmp128=_tmp123.encloser;_tmp129=_tmp123.ctrl_env;_tmp12A=_tmp123.capability;_tmp12B=_tmp123.curr_rgn;_tmp12C=(_tmp123.flags).in_lhs;_tmp12D=(_tmp123.flags).in_new;_tmp12E=(_tmp123.flags).abstract_ok;{
# 683
struct Cyc_Tcenv_Fenv*_tmp392;struct Cyc_Tcenv_Fenv*_tmp12F=(_tmp392=_region_malloc(r,sizeof(*_tmp392)),((_tmp392->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp124,((_tmp392->type_vars=(struct Cyc_List_List*)_tmp125,((_tmp392->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp126,((_tmp392->locals=(const struct Cyc_Tcenv_Bindings*)_tmp127,((_tmp392->encloser=(struct Cyc_Absyn_Stmt*)_tmp128,((_tmp392->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp129,((_tmp392->capability=(void*)_tmp12A,((_tmp392->curr_rgn=(void*)_tmp12B,((_tmp392->flags=(struct Cyc_Tcenv_FenvFlags)(
# 685
((_tmp392->flags).in_notreadctxt=1,(((_tmp392->flags).in_lhs=_tmp12C,(((_tmp392->flags).in_new=_tmp12D,(((_tmp392->flags).abstract_ok=_tmp12E,_tmp392->flags)))))))),((_tmp392->fnrgn=(struct _RegionHandle*)r,_tmp392)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp12F);};};}
# 689
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp131=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp131 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp10=*_tmp131;struct Cyc_Tcenv_SharedFenv*_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_RgnOrder_RgnPO*_tmp135;const struct Cyc_Tcenv_Bindings*_tmp136;struct Cyc_Absyn_Stmt*_tmp137;struct Cyc_Tcenv_CtrlEnv*_tmp138;void*_tmp139;void*_tmp13A;int _tmp13B;int _tmp13C;int _tmp13D;struct Cyc_Tcenv_Fenv _tmp132=_stmttmp10;_tmp133=_tmp132.shared;_tmp134=_tmp132.type_vars;_tmp135=_tmp132.region_order;_tmp136=_tmp132.locals;_tmp137=_tmp132.encloser;_tmp138=_tmp132.ctrl_env;_tmp139=_tmp132.capability;_tmp13A=_tmp132.curr_rgn;_tmp13B=(_tmp132.flags).in_lhs;_tmp13C=(_tmp132.flags).in_new;_tmp13D=(_tmp132.flags).abstract_ok;{
# 695
struct Cyc_Tcenv_Fenv*_tmp393;struct Cyc_Tcenv_Fenv*_tmp13E=(_tmp393=_region_malloc(r,sizeof(*_tmp393)),((_tmp393->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp133,((_tmp393->type_vars=(struct Cyc_List_List*)_tmp134,((_tmp393->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp135,((_tmp393->locals=(const struct Cyc_Tcenv_Bindings*)_tmp136,((_tmp393->encloser=(struct Cyc_Absyn_Stmt*)_tmp137,((_tmp393->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp138,((_tmp393->capability=(void*)_tmp139,((_tmp393->curr_rgn=(void*)_tmp13A,((_tmp393->flags=(struct Cyc_Tcenv_FenvFlags)(
# 697
((_tmp393->flags).in_notreadctxt=0,(((_tmp393->flags).in_lhs=_tmp13B,(((_tmp393->flags).in_new=_tmp13C,(((_tmp393->flags).abstract_ok=_tmp13D,_tmp393->flags)))))))),((_tmp393->fnrgn=(struct _RegionHandle*)r,_tmp393)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp13E);};};}
# 701
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp140=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp140 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp11=_tmp140;int _tmp142;struct Cyc_Tcenv_Fenv*_tmp141=_stmttmp11;_tmp142=(_tmp141->flags).in_notreadctxt;
return _tmp142;};}
# 708
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp143=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp143 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp12=*_tmp143;struct Cyc_Tcenv_SharedFenv*_tmp145;struct Cyc_List_List*_tmp146;struct Cyc_RgnOrder_RgnPO*_tmp147;const struct Cyc_Tcenv_Bindings*_tmp148;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Tcenv_CtrlEnv*_tmp14A;void*_tmp14B;void*_tmp14C;int _tmp14D;int _tmp14E;int _tmp14F;struct Cyc_Tcenv_Fenv _tmp144=_stmttmp12;_tmp145=_tmp144.shared;_tmp146=_tmp144.type_vars;_tmp147=_tmp144.region_order;_tmp148=_tmp144.locals;_tmp149=_tmp144.encloser;_tmp14A=_tmp144.ctrl_env;_tmp14B=_tmp144.capability;_tmp14C=_tmp144.curr_rgn;_tmp14D=(_tmp144.flags).in_notreadctxt;_tmp14E=(_tmp144.flags).in_new;_tmp14F=(_tmp144.flags).abstract_ok;{
# 714
struct Cyc_Tcenv_Fenv*_tmp394;struct Cyc_Tcenv_Fenv*_tmp150=(_tmp394=_region_malloc(r,sizeof(*_tmp394)),((_tmp394->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp145,((_tmp394->type_vars=(struct Cyc_List_List*)_tmp146,((_tmp394->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp147,((_tmp394->locals=(const struct Cyc_Tcenv_Bindings*)_tmp148,((_tmp394->encloser=(struct Cyc_Absyn_Stmt*)_tmp149,((_tmp394->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14A,((_tmp394->capability=(void*)_tmp14B,((_tmp394->curr_rgn=(void*)_tmp14C,((_tmp394->flags=(struct Cyc_Tcenv_FenvFlags)(
# 716
((_tmp394->flags).in_notreadctxt=_tmp14D,(((_tmp394->flags).in_lhs=1,(((_tmp394->flags).in_new=_tmp14E,(((_tmp394->flags).abstract_ok=_tmp14F,_tmp394->flags)))))))),((_tmp394->fnrgn=(struct _RegionHandle*)r,_tmp394)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp150);};};}
# 720
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp152=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp152 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp13=*_tmp152;struct Cyc_Tcenv_SharedFenv*_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_RgnOrder_RgnPO*_tmp156;const struct Cyc_Tcenv_Bindings*_tmp157;struct Cyc_Absyn_Stmt*_tmp158;struct Cyc_Tcenv_CtrlEnv*_tmp159;void*_tmp15A;void*_tmp15B;int _tmp15C;int _tmp15D;int _tmp15E;struct Cyc_Tcenv_Fenv _tmp153=_stmttmp13;_tmp154=_tmp153.shared;_tmp155=_tmp153.type_vars;_tmp156=_tmp153.region_order;_tmp157=_tmp153.locals;_tmp158=_tmp153.encloser;_tmp159=_tmp153.ctrl_env;_tmp15A=_tmp153.capability;_tmp15B=_tmp153.curr_rgn;_tmp15C=(_tmp153.flags).in_notreadctxt;_tmp15D=(_tmp153.flags).in_new;_tmp15E=(_tmp153.flags).abstract_ok;{
# 726
struct Cyc_Tcenv_Fenv*_tmp395;struct Cyc_Tcenv_Fenv*_tmp15F=(_tmp395=_region_malloc(r,sizeof(*_tmp395)),((_tmp395->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp154,((_tmp395->type_vars=(struct Cyc_List_List*)_tmp155,((_tmp395->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp156,((_tmp395->locals=(const struct Cyc_Tcenv_Bindings*)_tmp157,((_tmp395->encloser=(struct Cyc_Absyn_Stmt*)_tmp158,((_tmp395->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp159,((_tmp395->capability=(void*)_tmp15A,((_tmp395->curr_rgn=(void*)_tmp15B,((_tmp395->flags=(struct Cyc_Tcenv_FenvFlags)(
# 728
((_tmp395->flags).in_notreadctxt=_tmp15C,(((_tmp395->flags).in_lhs=0,(((_tmp395->flags).in_new=_tmp15D,(((_tmp395->flags).abstract_ok=_tmp15E,_tmp395->flags)))))))),((_tmp395->fnrgn=(struct _RegionHandle*)r,_tmp395)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp15F);};};}
# 732
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp161=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp161 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp14=_tmp161;int _tmp163;struct Cyc_Tcenv_Fenv*_tmp162=_stmttmp14;_tmp163=(_tmp162->flags).in_lhs;
return _tmp163;};}
# 740
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 742
struct _dyneither_ptr*_tmp164=(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null(vd))->name))).f2;
const char*_tmp396;struct Cyc_Tcenv_Fenv*_stmttmp15=
# 745
Cyc_Tcenv_get_fenv(te,((_tmp396="add_pat_var",_tag_dyneither(_tmp396,sizeof(char),12))));
# 743
struct Cyc_Tcenv_SharedFenv*_tmp166;struct Cyc_List_List*_tmp167;struct Cyc_RgnOrder_RgnPO*_tmp168;const struct Cyc_Tcenv_Bindings*_tmp169;struct Cyc_Absyn_Stmt*_tmp16A;struct Cyc_Tcenv_CtrlEnv*_tmp16B;void*_tmp16C;void*_tmp16D;struct Cyc_Tcenv_FenvFlags _tmp16E;struct Cyc_Tcenv_Fenv*_tmp165=_stmttmp15;_tmp166=_tmp165->shared;_tmp167=_tmp165->type_vars;_tmp168=_tmp165->region_order;_tmp169=_tmp165->locals;_tmp16A=_tmp165->encloser;_tmp16B=_tmp165->ctrl_env;_tmp16C=_tmp165->capability;_tmp16D=_tmp165->curr_rgn;_tmp16E=_tmp165->flags;{
# 746
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp399;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp398;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp16F=(_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398[0]=((_tmp399.tag=5,((_tmp399.f1=vd,_tmp399)))),_tmp398)));
struct Cyc_Tcenv_Bindings*_tmp39A;struct Cyc_Tcenv_Bindings*_tmp170=(_tmp39A=_region_malloc(r,sizeof(*_tmp39A)),((_tmp39A->v=_tmp164,((_tmp39A->b=(void*)_tmp16F,((_tmp39A->tl=_tmp169,_tmp39A)))))));
struct Cyc_Tcenv_Fenv*_tmp39B;struct Cyc_Tcenv_Fenv*_tmp171=(_tmp39B=_region_malloc(r,sizeof(*_tmp39B)),((_tmp39B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp166,((_tmp39B->type_vars=(struct Cyc_List_List*)_tmp167,((_tmp39B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp168,((_tmp39B->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp170),((_tmp39B->encloser=(struct Cyc_Absyn_Stmt*)_tmp16A,((_tmp39B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16B,((_tmp39B->capability=(void*)_tmp16C,((_tmp39B->curr_rgn=(void*)_tmp16D,((_tmp39B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp16E,((_tmp39B->fnrgn=(struct _RegionHandle*)r,_tmp39B)))))))))))))))))))));
# 750
return Cyc_Tcenv_put_fenv(r,te,_tmp171);};}
# 753
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp177=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
union Cyc_Absyn_Nmspace _tmp179;struct _dyneither_ptr*_tmp17A;struct _tuple0*_tmp178=q;_tmp179=_tmp178->f1;_tmp17A=_tmp178->f2;{
union Cyc_Absyn_Nmspace _tmp17B=_tmp179;_LL61: if((_tmp17B.Loc_n).tag != 4)goto _LL63;_LL62:
# 758
 if(_tmp177 == 0){struct Cyc_Dict_Absent_exn_struct _tmp39E;struct Cyc_Dict_Absent_exn_struct*_tmp39D;(int)_throw((void*)((_tmp39D=_cycalloc_atomic(sizeof(*_tmp39D)),((_tmp39D[0]=((_tmp39E.tag=Cyc_Dict_Absent,_tmp39E)),_tmp39D)))));}
goto _LL64;_LL63: if((_tmp17B.Rel_n).tag != 1)goto _LL65;if((struct Cyc_List_List*)(_tmp17B.Rel_n).val != 0)goto _LL65;if(!(_tmp177 != 0))goto _LL65;_LL64: {
# 761
struct Cyc_Tcenv_Fenv*_stmttmp16=_tmp177;const struct Cyc_Tcenv_Bindings*_tmp17F;struct Cyc_Tcenv_Fenv*_tmp17E=_stmttmp16;_tmp17F=_tmp17E->locals;{
# 763
struct _handler_cons _tmp180;_push_handler(& _tmp180);{int _tmp182=0;if(setjmp(_tmp180.handler))_tmp182=1;if(!_tmp182){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3A1;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3A0;void*_tmp185=(void*)((_tmp3A0=_region_malloc(r,sizeof(*_tmp3A0)),((_tmp3A0[0]=((_tmp3A1.tag=0,((_tmp3A1.f1=Cyc_Tcenv_lookup_binding(_tmp17F,_tmp17A),_tmp3A1)))),_tmp3A0))));_npop_handler(0);return _tmp185;};_pop_handler();}else{void*_tmp181=(void*)_exn_thrown;void*_tmp187=_tmp181;void*_tmp189;_LL68: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp188=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp187;if(_tmp188->tag != Cyc_Tcenv_NoBinding)goto _LL6A;}_LL69:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL6A: _tmp189=_tmp187;_LL6B:(void)_throw(_tmp189);_LL67:;}};};}_LL65:;_LL66: {
# 767
struct _handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){{void*_tmp18D=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp18D;};_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*_tmp18F=_tmp18B;void*_tmp191;_LL6D: {struct Cyc_Dict_Absent_exn_struct*_tmp190=(struct Cyc_Dict_Absent_exn_struct*)_tmp18F;if(_tmp190->tag != Cyc_Dict_Absent)goto _LL6F;}_LL6E: {
# 771
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3A4;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3A3;return(void*)((_tmp3A3=_region_malloc(r,sizeof(*_tmp3A3)),((_tmp3A3[0]=((_tmp3A4.tag=0,((_tmp3A4.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp3A4)))),_tmp3A3))));}_LL6F: _tmp191=_tmp18F;_LL70:(void)_throw(_tmp191);_LL6C:;}};}_LL60:;};}
# 776
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3A5;struct Cyc_Tcenv_Fenv*_stmttmp17=
Cyc_Tcenv_get_fenv(te,((_tmp3A5="process_continue",_tag_dyneither(_tmp3A5,sizeof(char),17))));
# 777
struct Cyc_Tcenv_CtrlEnv*_tmp195;struct Cyc_Tcenv_Fenv*_tmp194=_stmttmp17;_tmp195=_tmp194->ctrl_env;{
# 779
void*_stmttmp18=_tmp195->continue_stmt;void*_tmp196=_stmttmp18;struct Cyc_Absyn_Stmt*_tmp198;_LL72: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp197=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp196;if(_tmp197->tag != 3)goto _LL74;else{_tmp198=_tmp197->f1;}}_LL73:
# 781
{struct Cyc_List_List*_tmp3A6;_tmp198->non_local_preds=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->hd=s,((_tmp3A6->tl=_tmp198->non_local_preds,_tmp3A6))))));}
*sopt=_tmp198;
return;_LL74: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp199=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp196;if(_tmp199->tag != 0)goto _LL76;}_LL75:
{const char*_tmp3A9;void*_tmp3A8;(_tmp3A8=0,Cyc_Tcutil_terr(s->loc,((_tmp3A9="continue not in a loop",_tag_dyneither(_tmp3A9,sizeof(char),23))),_tag_dyneither(_tmp3A8,sizeof(void*),0)));}return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp19A=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp196;if(_tmp19A->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp19B=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp196;if(_tmp19B->tag != 2)goto _LL71;}_LL79: {
const char*_tmp3AA;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3AA="bad continue destination",_tag_dyneither(_tmp3AA,sizeof(char),25))));}_LL71:;};}
# 789
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3AB;struct Cyc_Tcenv_Fenv*_stmttmp19=
Cyc_Tcenv_get_fenv(te,((_tmp3AB="process_break",_tag_dyneither(_tmp3AB,sizeof(char),14))));
# 790
struct Cyc_Tcenv_CtrlEnv*_tmp1A2;struct Cyc_Tcenv_SharedFenv*_tmp1A3;struct Cyc_Tcenv_Fenv*_tmp1A1=_stmttmp19;_tmp1A2=_tmp1A1->ctrl_env;_tmp1A3=_tmp1A1->shared;{
# 792
void*_stmttmp1A=_tmp1A2->break_stmt;void*_tmp1A4=_stmttmp1A;struct Cyc_Absyn_Stmt*_tmp1A6;_LL7B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1A5=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1A4;if(_tmp1A5->tag != 3)goto _LL7D;else{_tmp1A6=_tmp1A5->f1;}}_LL7C:
# 794
{struct Cyc_List_List*_tmp3AC;_tmp1A6->non_local_preds=((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->hd=s,((_tmp3AC->tl=_tmp1A6->non_local_preds,_tmp3AC))))));}
*sopt=_tmp1A6;
return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1A7=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1A4;if(_tmp1A7->tag != 0)goto _LL7F;}_LL7E:
# 798
{const char*_tmp3AF;void*_tmp3AE;(_tmp3AE=0,Cyc_Tcutil_terr(s->loc,((_tmp3AF="break not in a loop or switch",_tag_dyneither(_tmp3AF,sizeof(char),30))),_tag_dyneither(_tmp3AE,sizeof(void*),0)));}
return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1A8=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1A4;if(_tmp1A8->tag != 2)goto _LL81;}_LL80:
# 804
{void*_stmttmp1B=Cyc_Tcutil_compress(_tmp1A3->return_typ);void*_tmp1AD=_stmttmp1B;_LL84: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1AD;if(_tmp1AE->tag != 0)goto _LL86;}_LL85:
 goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1AD;if(_tmp1AF->tag != 7)goto _LL88;}_LL87:
 goto _LL89;_LL88: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1AD;if(_tmp1B0->tag != 6)goto _LL8A;}_LL89:
# 808
{const char*_tmp3B2;void*_tmp3B1;(_tmp3B1=0,Cyc_Tcutil_warn(s->loc,((_tmp3B2="break may cause function not to return a value",_tag_dyneither(_tmp3B2,sizeof(char),47))),_tag_dyneither(_tmp3B1,sizeof(void*),0)));}
goto _LL83;_LL8A:;_LL8B:
# 811
{const char*_tmp3B5;void*_tmp3B4;(_tmp3B4=0,Cyc_Tcutil_terr(s->loc,((_tmp3B5="break may cause function not to return a value",_tag_dyneither(_tmp3B5,sizeof(char),47))),_tag_dyneither(_tmp3B4,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 814
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1A9=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1A4;if(_tmp1A9->tag != 1)goto _LL7A;}_LL82:
# 817
{const char*_tmp3B8;void*_tmp3B7;(_tmp3B7=0,Cyc_Tcutil_terr(s->loc,((_tmp3B8="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3B8,sizeof(char),56))),_tag_dyneither(_tmp3B7,sizeof(void*),0)));}
return;_LL7A:;};}
# 821
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3B9;struct Cyc_Tcenv_Fenv*_stmttmp1C=Cyc_Tcenv_get_fenv(te,((_tmp3B9="process_goto",_tag_dyneither(_tmp3B9,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1B9;struct Cyc_Tcenv_Fenv*_tmp1B8=_stmttmp1C;_tmp1B9=_tmp1B8->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1B9->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1BA=((struct Cyc_Tcenv_SharedFenv*)_check_null(_tmp1B9))->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1BA,l);
struct _RegionHandle*frgn=_tmp1B9->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3BA;slopt=((_tmp3BA=_region_malloc(frgn,sizeof(*_tmp3BA)),((_tmp3BA[0]=0,_tmp3BA))));}{
struct Cyc_List_List*_tmp3BB;struct Cyc_List_List*new_needed=(_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->hd=s,((_tmp3BB->tl=*slopt,_tmp3BB)))));
_tmp1B9->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1BA,l,new_needed);};}else{
# 833
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3BC;s->non_local_preds=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->hd=s,((_tmp3BC->tl=s->non_local_preds,_tmp3BC))))));}
*sopt=s;}};}
# 839
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 841
const char*_tmp3BD;struct Cyc_Tcenv_Fenv*_stmttmp1D=Cyc_Tcenv_get_fenv(te,((_tmp3BD="process_fallthru",_tag_dyneither(_tmp3BD,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp1C0;struct Cyc_Tcenv_Fenv*_tmp1BF=_stmttmp1D;_tmp1C0=_tmp1BF->ctrl_env;{
const struct _tuple10*_tmp1C1=((struct Cyc_Tcenv_CtrlEnv*)_check_null(_tmp1C0))->fallthru_clause;
if(_tmp1C1 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3BE;(((*_tmp1C1).f1)->body)->non_local_preds=(
(_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE->hd=s,((_tmp3BE->tl=(((*_tmp1C1).f1)->body)->non_local_preds,_tmp3BE))))));}{
struct Cyc_Absyn_Switch_clause**_tmp3BF;*clauseopt=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=(*_tmp1C1).f1,_tmp3BF))));};}
# 848
return _tmp1C1;};}
# 851
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 855
const char*_tmp3C0;struct Cyc_Tcenv_Fenv*_stmttmp1E=
# 857
Cyc_Tcenv_get_fenv(te,((_tmp3C0="set_fallthru",_tag_dyneither(_tmp3C0,sizeof(char),13))));
# 855
struct Cyc_Tcenv_SharedFenv*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_RgnOrder_RgnPO*_tmp1C8;const struct Cyc_Tcenv_Bindings*_tmp1C9;struct Cyc_Absyn_Stmt*_tmp1CA;struct Cyc_Tcenv_CtrlEnv*_tmp1CB;void*_tmp1CC;void*_tmp1CD;struct Cyc_Tcenv_FenvFlags _tmp1CE;struct Cyc_Tcenv_Fenv*_tmp1C5=_stmttmp1E;_tmp1C6=_tmp1C5->shared;_tmp1C7=_tmp1C5->type_vars;_tmp1C8=_tmp1C5->region_order;_tmp1C9=_tmp1C5->locals;_tmp1CA=_tmp1C5->encloser;_tmp1CB=_tmp1C5->ctrl_env;_tmp1CC=_tmp1C5->capability;_tmp1CD=_tmp1C5->curr_rgn;_tmp1CE=_tmp1C5->flags;{
# 858
struct _RegionHandle*_tmp1D0;void*_tmp1D1;void*_tmp1D2;const struct _tuple10*_tmp1D3;void*_tmp1D4;int _tmp1D5;struct Cyc_Tcenv_CtrlEnv*_tmp1CF=_tmp1CB;_tmp1D0=_tmp1CF->ctrl_rgn;_tmp1D1=_tmp1CF->continue_stmt;_tmp1D2=_tmp1CF->break_stmt;_tmp1D3=_tmp1CF->fallthru_clause;_tmp1D4=_tmp1CF->next_stmt;_tmp1D5=_tmp1CF->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3C1;ft_typ=((_tmp3C1=_region_malloc(_tmp1D0,sizeof(*_tmp3C1)),((_tmp3C1->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3C1->tl=ft_typ,_tmp3C1))))));}{
const struct Cyc_Tcenv_CList*_tmp1D7=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp3C4;struct Cyc_Tcenv_CtrlEnv*_tmp3C3;struct Cyc_Tcenv_CtrlEnv*_tmp1D8=
(_tmp3C3=_region_malloc(r,sizeof(*_tmp3C3)),((_tmp3C3->ctrl_rgn=_tmp1D0,((_tmp3C3->continue_stmt=_tmp1D1,((_tmp3C3->break_stmt=_tmp1D2,((_tmp3C3->fallthru_clause=(const struct _tuple10*)((_tmp3C4=_region_malloc(_tmp1D0,sizeof(*_tmp3C4)),((_tmp3C4->f1=clause,((_tmp3C4->f2=new_tvs,((_tmp3C4->f3=_tmp1D7,_tmp3C4)))))))),((_tmp3C3->next_stmt=_tmp1D4,((_tmp3C3->try_depth=_tmp1D5,_tmp3C3)))))))))))));
# 866
struct Cyc_Tcenv_Fenv*_tmp3C5;struct Cyc_Tcenv_Fenv*_tmp1D9=(_tmp3C5=_region_malloc(r,sizeof(*_tmp3C5)),((_tmp3C5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1C6,((_tmp3C5->type_vars=(struct Cyc_List_List*)_tmp1C7,((_tmp3C5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1C8,((_tmp3C5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1C9,((_tmp3C5->encloser=(struct Cyc_Absyn_Stmt*)_tmp1CA,((_tmp3C5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1D8,((_tmp3C5->capability=(void*)_tmp1CC,((_tmp3C5->curr_rgn=(void*)_tmp1CD,((_tmp3C5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1CE,((_tmp3C5->fnrgn=(struct _RegionHandle*)r,_tmp3C5)))))))))))))))))))));
# 870
return Cyc_Tcenv_put_fenv(r,te,_tmp1D9);};};};}
# 873
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3C6;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3C6="clear_fallthru",_tag_dyneither(_tmp3C6,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1DF;struct Cyc_Tcenv_Fenv*_tmp1DE=fe;_tmp1DF=_tmp1DE->ctrl_env;
_tmp1DF->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 880
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 882
const char*_tmp3C7;struct Cyc_Tcenv_Fenv*_stmttmp1F=
# 884
Cyc_Tcenv_get_fenv(te,((_tmp3C7="set_in_loop",_tag_dyneither(_tmp3C7,sizeof(char),12))));
# 882
struct Cyc_Tcenv_SharedFenv*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_RgnOrder_RgnPO*_tmp1E4;const struct Cyc_Tcenv_Bindings*_tmp1E5;struct Cyc_Absyn_Stmt*_tmp1E6;struct Cyc_Tcenv_CtrlEnv*_tmp1E7;void*_tmp1E8;void*_tmp1E9;struct Cyc_Tcenv_FenvFlags _tmp1EA;struct Cyc_Tcenv_Fenv*_tmp1E1=_stmttmp1F;_tmp1E2=_tmp1E1->shared;_tmp1E3=_tmp1E1->type_vars;_tmp1E4=_tmp1E1->region_order;_tmp1E5=_tmp1E1->locals;_tmp1E6=_tmp1E1->encloser;_tmp1E7=_tmp1E1->ctrl_env;_tmp1E8=_tmp1E1->capability;_tmp1E9=_tmp1E1->curr_rgn;_tmp1EA=_tmp1E1->flags;{
# 885
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3D2;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3D1;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3D0;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3CF;struct Cyc_Tcenv_CtrlEnv*_tmp3CE;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3CE=_region_malloc(r,sizeof(*_tmp3CE)),((_tmp3CE->ctrl_rgn=r,((_tmp3CE->continue_stmt=(void*)(
(_tmp3D0=_region_malloc(r,sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3CF.tag=3,((_tmp3CF.f1=continue_dest,_tmp3CF)))),_tmp3D0)))),((_tmp3CE->break_stmt=_tmp1E7->next_stmt,((_tmp3CE->next_stmt=(void*)(
# 889
(_tmp3D2=_region_malloc(r,sizeof(*_tmp3D2)),((_tmp3D2[0]=((_tmp3D1.tag=3,((_tmp3D1.f1=continue_dest,_tmp3D1)))),_tmp3D2)))),((_tmp3CE->fallthru_clause=_tmp1E7->fallthru_clause,((_tmp3CE->try_depth=_tmp1E7->try_depth,_tmp3CE)))))))))))));
# 892
struct Cyc_Tcenv_Fenv*_tmp3D3;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E2,((_tmp3D3->type_vars=(struct Cyc_List_List*)_tmp1E3,((_tmp3D3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1E4,((_tmp3D3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1E5,((_tmp3D3->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E6,((_tmp3D3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3D3->capability=(void*)_tmp1E8,((_tmp3D3->curr_rgn=(void*)_tmp1E9,((_tmp3D3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1EA,((_tmp3D3->fnrgn=(struct _RegionHandle*)r,_tmp3D3)))))))))))))))))))));
# 895
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 898
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3D4;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3D4="enter_try",_tag_dyneither(_tmp3D4,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1F3;struct Cyc_Tcenv_Fenv*_tmp1F2=fe;_tmp1F3=_tmp1F2->ctrl_env;
++ _tmp1F3->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 904
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3D5;struct Cyc_Tcenv_Fenv*_stmttmp20=Cyc_Tcenv_get_fenv(te,((_tmp3D5="get_try_depth",_tag_dyneither(_tmp3D5,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp1F6;struct Cyc_Tcenv_Fenv*_tmp1F5=_stmttmp20;_tmp1F6=_tmp1F5->ctrl_env;
return _tmp1F6->try_depth;}
# 911
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3D6;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3D6="set_in_switch",_tag_dyneither(_tmp3D6,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1F9;struct Cyc_Tcenv_Fenv*_tmp1F8=fe;_tmp1F9=_tmp1F8->ctrl_env;
_tmp1F9->break_stmt=_tmp1F9->next_stmt;
_tmp1F9->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 919
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 921
const char*_tmp3D7;struct Cyc_Tcenv_Fenv _stmttmp21=*
# 923
Cyc_Tcenv_get_fenv(te,((_tmp3D7="set_next",_tag_dyneither(_tmp3D7,sizeof(char),9))));
# 921
struct Cyc_Tcenv_SharedFenv*_tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_RgnOrder_RgnPO*_tmp1FE;const struct Cyc_Tcenv_Bindings*_tmp1FF;struct Cyc_Absyn_Stmt*_tmp200;struct Cyc_Tcenv_CtrlEnv*_tmp201;void*_tmp202;void*_tmp203;struct Cyc_Tcenv_FenvFlags _tmp204;struct Cyc_Tcenv_Fenv _tmp1FB=_stmttmp21;_tmp1FC=_tmp1FB.shared;_tmp1FD=_tmp1FB.type_vars;_tmp1FE=_tmp1FB.region_order;_tmp1FF=_tmp1FB.locals;_tmp200=_tmp1FB.encloser;_tmp201=_tmp1FB.ctrl_env;_tmp202=_tmp1FB.capability;_tmp203=_tmp1FB.curr_rgn;_tmp204=_tmp1FB.flags;{
# 924
struct Cyc_Tcenv_CtrlEnv*_tmp3D8;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3D8=_region_malloc(r,sizeof(*_tmp3D8)),((_tmp3D8->ctrl_rgn=r,((_tmp3D8->continue_stmt=_tmp201->continue_stmt,((_tmp3D8->break_stmt=_tmp201->break_stmt,((_tmp3D8->next_stmt=j,((_tmp3D8->fallthru_clause=_tmp201->fallthru_clause,((_tmp3D8->try_depth=_tmp201->try_depth,_tmp3D8)))))))))))));
# 932
struct Cyc_Tcenv_Fenv*_tmp3D9;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3D9=_region_malloc(r,sizeof(*_tmp3D9)),((_tmp3D9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FC,((_tmp3D9->type_vars=(struct Cyc_List_List*)_tmp1FD,((_tmp3D9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FE,((_tmp3D9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FF,((_tmp3D9->encloser=(struct Cyc_Absyn_Stmt*)_tmp200,((_tmp3D9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3D9->capability=(void*)_tmp202,((_tmp3D9->curr_rgn=(void*)_tmp203,((_tmp3D9->flags=(struct Cyc_Tcenv_FenvFlags)_tmp204,((_tmp3D9->fnrgn=(struct _RegionHandle*)r,_tmp3D9)))))))))))))))))))));
# 935
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 938
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp3DA;struct Cyc_Tcenv_Fenv*_stmttmp22=Cyc_Tcenv_get_fenv(te,((_tmp3DA="add_label",_tag_dyneither(_tmp3DA,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp209;struct Cyc_Tcenv_Fenv*_tmp208=_stmttmp22;_tmp209=_tmp208->shared;{
struct Cyc_Dict_Dict needed=_tmp209->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp209->frgn;
if(sl_opt != 0){
_tmp209->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 946
struct Cyc_List_List*_tmp20A=*((struct Cyc_List_List**)_check_null(sl_opt));
s->non_local_preds=_tmp20A;
for(0;_tmp20A != 0;_tmp20A=_tmp20A->tl){
void*_stmttmp23=((struct Cyc_Absyn_Stmt*)_tmp20A->hd)->r;void*_tmp20B=_stmttmp23;struct Cyc_Absyn_Stmt**_tmp20D;_LL8D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp20C=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp20B;if(_tmp20C->tag != 8)goto _LL8F;else{_tmp20D=(struct Cyc_Absyn_Stmt**)& _tmp20C->f2;}}_LL8E:
*_tmp20D=s;goto _LL8C;_LL8F:;_LL90:
{const char*_tmp3DB;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3DB="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp3DB,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}
# 954
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp209->seen_labels,v)){
const char*_tmp3DF;void*_tmp3DE[1];struct Cyc_String_pa_PrintArg_struct _tmp3DD;(_tmp3DD.tag=0,((_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3DE[0]=& _tmp3DD,Cyc_Tcutil_terr(s->loc,((_tmp3DF="Repeated label: %s",_tag_dyneither(_tmp3DF,sizeof(char),19))),_tag_dyneither(_tmp3DE,sizeof(void*),1)))))));}
_tmp209->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp209->seen_labels,v,s);
return te;};}
# 960
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3E0;struct Cyc_Tcenv_Fenv*_stmttmp24=Cyc_Tcenv_get_fenv(te,((_tmp3E0="all_labels_resolved",_tag_dyneither(_tmp3E0,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp214;struct Cyc_Tcenv_Fenv*_tmp213=_stmttmp24;_tmp214=_tmp213->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp214->needed_labels);}
# 965
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3E1;struct Cyc_Tcenv_Fenv*_stmttmp25=Cyc_Tcenv_get_fenv(te,((_tmp3E1="get_encloser",_tag_dyneither(_tmp3E1,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp217;struct Cyc_Tcenv_Fenv*_tmp216=_stmttmp25;_tmp217=_tmp216->encloser;
return _tmp217;}
# 970
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp3E2;return(_tmp3E2=_region_malloc(r2,sizeof(*_tmp3E2)),((_tmp3E2->ns=te->ns,((_tmp3E2->ae=te->ae,((_tmp3E2->le=0,((_tmp3E2->allow_valueof=1,_tmp3E2)))))))));}{
struct Cyc_Tcenv_Fenv _stmttmp26=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_RgnOrder_RgnPO*_tmp21D;const struct Cyc_Tcenv_Bindings*_tmp21E;struct Cyc_Absyn_Stmt*_tmp21F;struct Cyc_Tcenv_CtrlEnv*_tmp220;void*_tmp221;void*_tmp222;struct Cyc_Tcenv_FenvFlags _tmp223;struct Cyc_Tcenv_Fenv _tmp21A=_stmttmp26;_tmp21B=_tmp21A.shared;_tmp21C=_tmp21A.type_vars;_tmp21D=_tmp21A.region_order;_tmp21E=_tmp21A.locals;_tmp21F=_tmp21A.encloser;_tmp220=_tmp21A.ctrl_env;_tmp221=_tmp21A.capability;_tmp222=_tmp21A.curr_rgn;_tmp223=_tmp21A.flags;{
struct Cyc_Tcenv_Fenv*_tmp3E3;struct Cyc_Tcenv_Fenv*_tmp224=(_tmp3E3=_region_malloc(r2,sizeof(*_tmp3E3)),((_tmp3E3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp21B,((_tmp3E3->type_vars=(struct Cyc_List_List*)_tmp21C,((_tmp3E3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21D,((_tmp3E3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21E,((_tmp3E3->encloser=(struct Cyc_Absyn_Stmt*)_tmp21F,((_tmp3E3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp220,((_tmp3E3->capability=(void*)_tmp221,((_tmp3E3->curr_rgn=(void*)_tmp222,((_tmp3E3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp223,((_tmp3E3->fnrgn=(struct _RegionHandle*)r2,_tmp3E3)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp3E4;return(_tmp3E4=_region_malloc(r2,sizeof(*_tmp3E4)),((_tmp3E4->ns=te->ns,((_tmp3E4->ae=te->ae,((_tmp3E4->le=_tmp224,((_tmp3E4->allow_valueof=1,_tmp3E4)))))))));};};}
# 978
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 980
const char*_tmp3E5;struct Cyc_Tcenv_Fenv _stmttmp27=*
# 982
Cyc_Tcenv_get_fenv(te,((_tmp3E5="set_encloser",_tag_dyneither(_tmp3E5,sizeof(char),13))));
# 980
struct Cyc_Tcenv_SharedFenv*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_RgnOrder_RgnPO*_tmp22A;const struct Cyc_Tcenv_Bindings*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Tcenv_CtrlEnv*_tmp22D;void*_tmp22E;void*_tmp22F;struct Cyc_Tcenv_FenvFlags _tmp230;struct Cyc_Tcenv_Fenv _tmp227=_stmttmp27;_tmp228=_tmp227.shared;_tmp229=_tmp227.type_vars;_tmp22A=_tmp227.region_order;_tmp22B=_tmp227.locals;_tmp22C=_tmp227.encloser;_tmp22D=_tmp227.ctrl_env;_tmp22E=_tmp227.capability;_tmp22F=_tmp227.curr_rgn;_tmp230=_tmp227.flags;{
# 983
struct Cyc_Tcenv_Fenv*_tmp3E6;struct Cyc_Tcenv_Fenv*_tmp231=
(_tmp3E6=_region_malloc(r,sizeof(*_tmp3E6)),((_tmp3E6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp228,((_tmp3E6->type_vars=(struct Cyc_List_List*)_tmp229,((_tmp3E6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22A,((_tmp3E6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22B,((_tmp3E6->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp3E6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp22D,((_tmp3E6->capability=(void*)_tmp22E,((_tmp3E6->curr_rgn=(void*)_tmp22F,((_tmp3E6->flags=(struct Cyc_Tcenv_FenvFlags)_tmp230,((_tmp3E6->fnrgn=(struct _RegionHandle*)r,_tmp3E6)))))))))))))))))))));
# 986
return Cyc_Tcenv_put_fenv(r,te,_tmp231);};}
# 989
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 992
struct Cyc_Absyn_Tvar*tv;
{void*_stmttmp28=Cyc_Tcutil_compress(rgn);void*_tmp234=_stmttmp28;struct Cyc_Absyn_Tvar*_tmp236;_LL92: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp234;if(_tmp235->tag != 2)goto _LL94;else{_tmp236=_tmp235->f1;}}_LL93:
 tv=_tmp236;goto _LL91;_LL94:;_LL95:
{const char*_tmp3E7;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3E7="bad add region",_tag_dyneither(_tmp3E7,sizeof(char),15))));}goto _LL91;_LL91:;}{
# 997
const char*_tmp3E8;struct Cyc_Tcenv_Fenv _stmttmp29=*
# 999
Cyc_Tcenv_get_fenv(te,((_tmp3E8="add_region",_tag_dyneither(_tmp3E8,sizeof(char),11))));
# 997
struct Cyc_Tcenv_SharedFenv*_tmp239;struct Cyc_List_List*_tmp23A;struct Cyc_RgnOrder_RgnPO*_tmp23B;const struct Cyc_Tcenv_Bindings*_tmp23C;struct Cyc_Absyn_Stmt*_tmp23D;struct Cyc_Tcenv_CtrlEnv*_tmp23E;void*_tmp23F;void*_tmp240;struct Cyc_Tcenv_FenvFlags _tmp241;struct Cyc_Tcenv_Fenv _tmp238=_stmttmp29;_tmp239=_tmp238.shared;_tmp23A=_tmp238.type_vars;_tmp23B=_tmp238.region_order;_tmp23C=_tmp238.locals;_tmp23D=_tmp238.encloser;_tmp23E=_tmp238.ctrl_env;_tmp23F=_tmp238.capability;_tmp240=_tmp238.curr_rgn;_tmp241=_tmp238.flags;
# 1000
_tmp23B=Cyc_RgnOrder_add_youngest(_tmp239->frgn,_tmp23B,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp3FB;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3F9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3F8;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3F6;_tmp23F=(void*)((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3FB.tag=24,((_tmp3FB.f1=((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7->hd=(void*)((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3F8.tag=23,((_tmp3F8.f1=rgn,_tmp3F8)))),_tmp3F9)))),((_tmp3F7->tl=(
(_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA->hd=_tmp23F,((_tmp3FA->tl=0,_tmp3FA)))))),_tmp3F7)))))),_tmp3FB)))),_tmp3F6))));}{
struct Cyc_Tcenv_Fenv*_tmp3FC;struct Cyc_Tcenv_Fenv*_tmp248=
(_tmp3FC=_region_malloc(r,sizeof(*_tmp3FC)),((_tmp3FC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp239,((_tmp3FC->type_vars=(struct Cyc_List_List*)_tmp23A,((_tmp3FC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23B,((_tmp3FC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp23C,((_tmp3FC->encloser=(struct Cyc_Absyn_Stmt*)_tmp23D,((_tmp3FC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp23E,((_tmp3FC->capability=(void*)_tmp23F,((_tmp3FC->curr_rgn=(void*)_tmp240,((_tmp3FC->flags=(struct Cyc_Tcenv_FenvFlags)_tmp241,((_tmp3FC->fnrgn=(struct _RegionHandle*)r,_tmp3FC)))))))))))))))))))));
# 1006
return Cyc_Tcenv_put_fenv(r,te,_tmp248);};};}
# 1009
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1011
const char*_tmp3FD;struct Cyc_Tcenv_Fenv _stmttmp2A=*
# 1013
Cyc_Tcenv_get_fenv(te,((_tmp3FD="new_named_block",_tag_dyneither(_tmp3FD,sizeof(char),16))));
# 1011
struct Cyc_Tcenv_SharedFenv*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_RgnOrder_RgnPO*_tmp24E;const struct Cyc_Tcenv_Bindings*_tmp24F;struct Cyc_Absyn_Stmt*_tmp250;struct Cyc_Tcenv_CtrlEnv*_tmp251;void*_tmp252;void*_tmp253;struct Cyc_Tcenv_FenvFlags _tmp254;struct Cyc_Tcenv_Fenv _tmp24B=_stmttmp2A;_tmp24C=_tmp24B.shared;_tmp24D=_tmp24B.type_vars;_tmp24E=_tmp24B.region_order;_tmp24F=_tmp24B.locals;_tmp250=_tmp24B.encloser;_tmp251=_tmp24B.ctrl_env;_tmp252=_tmp24B.capability;_tmp253=_tmp24B.curr_rgn;_tmp254=_tmp24B.flags;{
# 1015
const char*_tmp3FE;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3FE="new_block",_tag_dyneither(_tmp3FE,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp401;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp400;void*block_typ=(void*)((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400[0]=((_tmp401.tag=2,((_tmp401.f1=block_rgn,_tmp401)))),_tmp400))));
{struct Cyc_List_List*_tmp402;_tmp24D=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->hd=block_rgn,((_tmp402->tl=_tmp24D,_tmp402))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp24D);
_tmp24E=Cyc_RgnOrder_add_youngest(_tmp24C->frgn,_tmp24E,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp415;struct Cyc_List_List*_tmp414;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp413;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp412;struct Cyc_List_List*_tmp411;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp410;_tmp252=(void*)((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=((_tmp415.tag=24,((_tmp415.f1=((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->hd=(void*)((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413[0]=((_tmp412.tag=23,((_tmp412.f1=block_typ,_tmp412)))),_tmp413)))),((_tmp411->tl=((_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414->hd=_tmp252,((_tmp414->tl=0,_tmp414)))))),_tmp411)))))),_tmp415)))),_tmp410))));}
_tmp253=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp416;struct Cyc_Tcenv_Fenv*_tmp25C=
(_tmp416=_region_malloc(r,sizeof(*_tmp416)),((_tmp416->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp24C,((_tmp416->type_vars=(struct Cyc_List_List*)_tmp24D,((_tmp416->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24E,((_tmp416->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24F,((_tmp416->encloser=(struct Cyc_Absyn_Stmt*)_tmp250,((_tmp416->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp251,((_tmp416->capability=(void*)_tmp252,((_tmp416->curr_rgn=(void*)_tmp253,((_tmp416->flags=(struct Cyc_Tcenv_FenvFlags)_tmp254,((_tmp416->fnrgn=(struct _RegionHandle*)r,_tmp416)))))))))))))))))))));
# 1025
return Cyc_Tcenv_put_fenv(r,te,_tmp25C);};};}
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
const char*_tmp417;struct Cyc_Tcenv_Fenv _stmttmp2B=*
# 1044
Cyc_Tcenv_get_fenv(te,((_tmp417="new_outlives_constraints",_tag_dyneither(_tmp417,sizeof(char),25))));
# 1042
struct Cyc_Tcenv_SharedFenv*_tmp264;struct Cyc_List_List*_tmp265;struct Cyc_RgnOrder_RgnPO*_tmp266;const struct Cyc_Tcenv_Bindings*_tmp267;struct Cyc_Absyn_Stmt*_tmp268;struct Cyc_Tcenv_CtrlEnv*_tmp269;void*_tmp26A;void*_tmp26B;struct Cyc_Tcenv_FenvFlags _tmp26C;struct Cyc_Tcenv_Fenv _tmp263=_stmttmp2B;_tmp264=_tmp263.shared;_tmp265=_tmp263.type_vars;_tmp266=_tmp263.region_order;_tmp267=_tmp263.locals;_tmp268=_tmp263.encloser;_tmp269=_tmp263.ctrl_env;_tmp26A=_tmp263.capability;_tmp26B=_tmp263.curr_rgn;_tmp26C=_tmp263.flags;
# 1045
for(0;cs != 0;cs=cs->tl){
_tmp266=
Cyc_RgnOrder_add_outlives_constraint(_tmp264->frgn,_tmp266,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1050
struct Cyc_Tcenv_Fenv*_tmp418;struct Cyc_Tcenv_Fenv*_tmp26D=
(_tmp418=_region_malloc(r,sizeof(*_tmp418)),((_tmp418->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp264,((_tmp418->type_vars=(struct Cyc_List_List*)_tmp265,((_tmp418->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp266,((_tmp418->locals=(const struct Cyc_Tcenv_Bindings*)_tmp267,((_tmp418->encloser=(struct Cyc_Absyn_Stmt*)_tmp268,((_tmp418->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp269,((_tmp418->capability=(void*)_tmp26A,((_tmp418->curr_rgn=(void*)_tmp26B,((_tmp418->flags=(struct Cyc_Tcenv_FenvFlags)_tmp26C,((_tmp418->fnrgn=(struct _RegionHandle*)r,_tmp418)))))))))))))))))))));
# 1053
return Cyc_Tcenv_put_fenv(r,te,_tmp26D);};}
# 1056
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1064
void*_tmp270=Cyc_Tcutil_compress(r1);
void*_tmp271=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp272=Cyc_Tcutil_typ_kind(_tmp270);
struct Cyc_Absyn_Kind*_tmp273=Cyc_Tcutil_typ_kind(_tmp271);
# 1070
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp272,_tmp273);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp273,_tmp272);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp41F;void*_tmp41E[4];struct Cyc_String_pa_PrintArg_struct _tmp41D;struct Cyc_String_pa_PrintArg_struct _tmp41C;struct Cyc_String_pa_PrintArg_struct _tmp41B;struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp273)),((_tmp41B.tag=0,((_tmp41B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp272)),((_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp271)),((_tmp41D.tag=0,((_tmp41D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp270)),((_tmp41E[0]=& _tmp41D,((_tmp41E[1]=& _tmp41C,((_tmp41E[2]=& _tmp41B,((_tmp41E[3]=& _tmp41A,Cyc_Tcutil_terr(loc,((_tmp41F="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp41F,sizeof(char),82))),_tag_dyneither(_tmp41E,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1077
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _stmttmp2C=Cyc_Tcutil_swap_kind(_tmp271,Cyc_Tcutil_kind_to_bound(_tmp272));struct Cyc_Absyn_Tvar*_tmp27B;void*_tmp27C;struct _tuple11 _tmp27A=_stmttmp2C;_tmp27B=_tmp27A.f1;_tmp27C=_tmp27A.f2;{
struct _tuple11*_tmp420;*oldtv=((_tmp420=_region_malloc(r,sizeof(*_tmp420)),((_tmp420->f1=_tmp27B,((_tmp420->f2=_tmp27C,_tmp420))))));};}else{
# 1081
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _stmttmp2D=Cyc_Tcutil_swap_kind(_tmp270,Cyc_Tcutil_kind_to_bound(_tmp273));struct Cyc_Absyn_Tvar*_tmp27F;void*_tmp280;struct _tuple11 _tmp27E=_stmttmp2D;_tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;{
struct _tuple11*_tmp421;*oldtv=((_tmp421=_region_malloc(r,sizeof(*_tmp421)),((_tmp421->f1=_tmp27F,((_tmp421->f2=_tmp280,_tmp421))))));};}}}{
# 1087
struct _RegionHandle*_tmp282=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp283=0;
if((_tmp270 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp270 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp270 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp424;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp423;void*eff1=(void*)((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423[0]=((_tmp424.tag=23,((_tmp424.f1=_tmp270,_tmp424)))),_tmp423))));
struct _tuple13*_tmp427;struct Cyc_List_List*_tmp426;_tmp283=((_tmp426=_region_malloc(_tmp282,sizeof(*_tmp426)),((_tmp426->hd=((_tmp427=_region_malloc(_tmp282,sizeof(*_tmp427)),((_tmp427->f1=eff1,((_tmp427->f2=_tmp271,_tmp427)))))),((_tmp426->tl=_tmp283,_tmp426))))));}
# 1093
if((_tmp271 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp271 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp271 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp42A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp429;void*eff2=(void*)((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429[0]=((_tmp42A.tag=23,((_tmp42A.f1=_tmp271,_tmp42A)))),_tmp429))));
struct _tuple13*_tmp42D;struct Cyc_List_List*_tmp42C;_tmp283=((_tmp42C=_region_malloc(_tmp282,sizeof(*_tmp42C)),((_tmp42C->hd=((_tmp42D=_region_malloc(_tmp282,sizeof(*_tmp42D)),((_tmp42D->f1=eff2,((_tmp42D->f2=_tmp270,_tmp42D)))))),((_tmp42C->tl=_tmp283,_tmp42C))))));}
# 1098
return Cyc_Tcenv_new_outlives_constraints(_tmp282,te,_tmp283,loc);};}
# 1101
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp28C=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp28C == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp28C;
void*_tmp28E;struct Cyc_Tcenv_Fenv*_tmp28D=fe;_tmp28E=_tmp28D->curr_rgn;
return _tmp28E;};}
# 1112
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp42E;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp42E="check_rgn_accessible",_tag_dyneither(_tmp42E,sizeof(char),21))));
void*_tmp290;struct Cyc_RgnOrder_RgnPO*_tmp291;struct Cyc_Tcenv_Fenv*_tmp28F=fe;_tmp290=_tmp28F->capability;_tmp291=_tmp28F->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp290) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp290))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp431;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp430;if(Cyc_RgnOrder_eff_outlives_eff(_tmp291,(void*)((_tmp430=_cycalloc(sizeof(*_tmp430)),((_tmp430[0]=((_tmp431.tag=23,((_tmp431.f1=rgn,_tmp431)))),_tmp430)))),_tmp290))
return;}{
const char*_tmp435;void*_tmp434[1];struct Cyc_String_pa_PrintArg_struct _tmp433;(_tmp433.tag=0,((_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp434[0]=& _tmp433,Cyc_Tcutil_terr(loc,((_tmp435="Expression accesses unavailable region %s",_tag_dyneither(_tmp435,sizeof(char),42))),_tag_dyneither(_tmp434,sizeof(void*),1)))))));};}
# 1124
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp436;struct Cyc_Tcenv_Fenv*_stmttmp2E=
Cyc_Tcenv_get_fenv(te,((_tmp436="check_rgn_resetable",_tag_dyneither(_tmp436,sizeof(char),20))));
# 1126
struct Cyc_RgnOrder_RgnPO*_tmp299;struct Cyc_Tcenv_Fenv*_tmp298=_stmttmp2E;_tmp299=_tmp298->region_order;{
# 1128
void*_stmttmp2F=Cyc_Tcutil_compress(rgn);void*_tmp29A=_stmttmp2F;struct Cyc_Absyn_Tvar*_tmp29C;_LL97: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp29B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp29A;if(_tmp29B->tag != 2)goto _LL99;else{_tmp29C=_tmp29B->f1;}}_LL98:
# 1130
 if(!Cyc_RgnOrder_is_region_resetable(_tmp299,_tmp29C)){
const char*_tmp43A;void*_tmp439[1];struct Cyc_String_pa_PrintArg_struct _tmp438;(_tmp438.tag=0,((_tmp438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp439[0]=& _tmp438,Cyc_Tcutil_terr(loc,((_tmp43A="Region %s is not resetable",_tag_dyneither(_tmp43A,sizeof(char),27))),_tag_dyneither(_tmp439,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {
# 1134
const char*_tmp43D;void*_tmp43C;(_tmp43C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp43D="check_rgn_resetable",_tag_dyneither(_tmp43D,sizeof(char),20))),_tag_dyneither(_tmp43C,sizeof(void*),0)));}_LL96:;};};}
# 1141
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2A3=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1146
if(_tmp2A3 == 0){
void*_tmp2A4=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2A5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2A4;if(_tmp2A5->tag != 22)goto _LL9E;}_LL9D:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2A6=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2A4;if(_tmp2A6->tag != 21)goto _LLA0;}_LL9F:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2A7=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2A4;if(_tmp2A7->tag != 20)goto _LLA2;}_LLA1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3:
 return 0;_LL9B:;}{
# 1154
struct Cyc_Tcenv_Fenv*fe=_tmp2A3;
struct Cyc_RgnOrder_RgnPO*_tmp2A9;struct Cyc_Tcenv_Fenv*_tmp2A8=fe;_tmp2A9=_tmp2A8->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp440;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp43F;int _tmp2AA=Cyc_RgnOrder_effect_outlives(_tmp2A9,(void*)((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp440.tag=23,((_tmp440.f1=rt_a,_tmp440)))),_tmp43F)))),rt_b);
# 1160
return _tmp2AA;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1165
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp441;struct Cyc_Tcenv_Fenv*_stmttmp30=
Cyc_Tcenv_get_fenv(te,((_tmp441="check_effect_accessible",_tag_dyneither(_tmp441,sizeof(char),24))));
# 1166
void*_tmp2AE;struct Cyc_RgnOrder_RgnPO*_tmp2AF;struct Cyc_Tcenv_SharedFenv*_tmp2B0;struct Cyc_Tcenv_Fenv*_tmp2AD=_stmttmp30;_tmp2AE=_tmp2AD->capability;_tmp2AF=_tmp2AD->region_order;_tmp2B0=_tmp2AD->shared;
# 1168
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2AE))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2AF,eff,_tmp2AE))
return;{
struct _RegionHandle*frgn=_tmp2B0->frgn;
struct _tuple14*_tmp444;struct Cyc_List_List*_tmp443;_tmp2B0->delayed_effect_checks=(
(_tmp443=_region_malloc(frgn,sizeof(*_tmp443)),((_tmp443->hd=((_tmp444=_region_malloc(frgn,sizeof(*_tmp444)),((_tmp444->f1=_tmp2AE,((_tmp444->f2=eff,((_tmp444->f3=_tmp2AF,((_tmp444->f4=loc,_tmp444)))))))))),((_tmp443->tl=_tmp2B0->delayed_effect_checks,_tmp443))))));};}
# 1177
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp445;struct Cyc_Tcenv_Fenv*_stmttmp31=
Cyc_Tcenv_get_fenv(te,((_tmp445="check_delayed_effects",_tag_dyneither(_tmp445,sizeof(char),22))));
# 1178
struct Cyc_Tcenv_SharedFenv*_tmp2B5;struct Cyc_Tcenv_Fenv*_tmp2B4=_stmttmp31;_tmp2B5=_tmp2B4->shared;{
# 1180
struct Cyc_List_List*_tmp2B6=((struct Cyc_Tcenv_SharedFenv*)_check_null(_tmp2B5))->delayed_effect_checks;
for(0;_tmp2B6 != 0;_tmp2B6=_tmp2B6->tl){
struct _tuple14*_stmttmp32=(struct _tuple14*)_tmp2B6->hd;void*_tmp2B8;void*_tmp2B9;struct Cyc_RgnOrder_RgnPO*_tmp2BA;unsigned int _tmp2BB;struct _tuple14*_tmp2B7=_stmttmp32;_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;_tmp2BA=_tmp2B7->f3;_tmp2BB=_tmp2B7->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2B9,_tmp2B8))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2BA,_tmp2B9,_tmp2B8))
continue;{
const char*_tmp44A;void*_tmp449[2];struct Cyc_String_pa_PrintArg_struct _tmp448;struct Cyc_String_pa_PrintArg_struct _tmp447;(_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2B9)),((_tmp448.tag=0,((_tmp448.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2B8)),((_tmp449[0]=& _tmp448,((_tmp449[1]=& _tmp447,Cyc_Tcutil_terr(_tmp2BB,((_tmp44A="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp44A,sizeof(char),51))),_tag_dyneither(_tmp449,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1196
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1198
struct Cyc_Tcenv_Fenv*_tmp2C1=((struct Cyc_Tcenv_Tenv*)_check_null(te))->le;
if(_tmp2C1 == 0){
# 1201
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp44D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp44C;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C[0]=((_tmp44D.tag=23,((_tmp44D.f1=(*((struct _tuple13*)po->hd)).f2,_tmp44D)))),_tmp44C)))),Cyc_Absyn_empty_effect)){
const char*_tmp450;void*_tmp44F;(_tmp44F=0,Cyc_Tcutil_terr(loc,((_tmp450="the required region ordering is not satisfied here",_tag_dyneither(_tmp450,sizeof(char),51))),_tag_dyneither(_tmp44F,sizeof(void*),0)));}}
return;}{
# 1207
struct Cyc_Tcenv_Fenv*_stmttmp33=_tmp2C1;struct Cyc_RgnOrder_RgnPO*_tmp2C7;struct Cyc_Tcenv_SharedFenv*_tmp2C8;struct Cyc_Tcenv_Fenv*_tmp2C6=_stmttmp33;_tmp2C7=_tmp2C6->region_order;_tmp2C8=_tmp2C6->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2C7,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp453;struct Cyc_List_List*_tmp452;_tmp2C8->delayed_constraint_checks=(
(_tmp452=_region_malloc(_tmp2C8->frgn,sizeof(*_tmp452)),((_tmp452->hd=((_tmp453=_region_malloc(_tmp2C8->frgn,sizeof(*_tmp453)),((_tmp453->f1=_tmp2C7,((_tmp453->f2=po,((_tmp453->f3=loc,_tmp453)))))))),((_tmp452->tl=_tmp2C8->delayed_constraint_checks,_tmp452))))));}};}
# 1214
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp454;struct Cyc_Tcenv_Fenv*_stmttmp34=
Cyc_Tcenv_get_fenv(te,((_tmp454="check_delayed_constraints",_tag_dyneither(_tmp454,sizeof(char),26))));
# 1215
struct Cyc_Tcenv_SharedFenv*_tmp2CC;struct Cyc_Tcenv_Fenv*_tmp2CB=_stmttmp34;_tmp2CC=_tmp2CB->shared;{
# 1217
struct Cyc_List_List*_tmp2CD=((struct Cyc_Tcenv_SharedFenv*)_check_null(_tmp2CC))->delayed_constraint_checks;
for(0;_tmp2CD != 0;_tmp2CD=_tmp2CD->tl){
struct _tuple15*_stmttmp35=(struct _tuple15*)_tmp2CD->hd;struct Cyc_RgnOrder_RgnPO*_tmp2CF;struct Cyc_List_List*_tmp2D0;unsigned int _tmp2D1;struct _tuple15*_tmp2CE=_stmttmp35;_tmp2CF=_tmp2CE->f1;_tmp2D0=_tmp2CE->f2;_tmp2D1=_tmp2CE->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2CF,_tmp2D0,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp457;void*_tmp456;(_tmp456=0,Cyc_Tcutil_terr(_tmp2D1,((_tmp457="the required region ordering is not satisfied here",_tag_dyneither(_tmp457,sizeof(char),51))),_tag_dyneither(_tmp456,sizeof(void*),0)));}}};}
# 1225
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp464;const char*_tmp463;void*_tmp462[1];struct Cyc_String_pa_PrintArg_struct _tmp461;struct Cyc_Absyn_Tvar*_tmp460;struct Cyc_Absyn_Tvar*rgn0=
(_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460->name=((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464[0]=(struct _dyneither_ptr)((_tmp461.tag=0,((_tmp461.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp462[0]=& _tmp461,Cyc_aprintf(((_tmp463="`%s",_tag_dyneither(_tmp463,sizeof(char),4))),_tag_dyneither(_tmp462,sizeof(void*),1)))))))),_tmp464)))),((_tmp460->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp460->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp460)))))));
struct Cyc_List_List*_tmp465;struct Cyc_List_List*_tmp2D5=(_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465->hd=rgn0,((_tmp465->tl=((struct Cyc_Absyn_Fndecl*)_check_null(fd))->tvs,_tmp465)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2D5);{
# 1233
struct Cyc_RgnOrder_RgnPO*_tmp2D6=Cyc_RgnOrder_initial_fn_po(r,((struct Cyc_Absyn_Fndecl*)_check_null(fd))->tvs,((struct Cyc_Absyn_Fndecl*)_check_null(fd))->rgn_po,(void*)_check_null(((struct Cyc_Absyn_Fndecl*)_check_null(fd))->effect),rgn0,loc);
# 1235
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp468;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp467;void*param_rgn=(void*)((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467[0]=((_tmp468.tag=2,((_tmp468.f1=rgn0,_tmp468)))),_tmp467))));
struct Cyc_List_List*_tmp2D7=0;
{struct Cyc_List_List*_tmp2D8=((struct Cyc_Absyn_Fndecl*)_check_null(fd))->args;for(0;_tmp2D8 != 0;_tmp2D8=_tmp2D8->tl){
struct Cyc_Absyn_Vardecl _tmp46E;struct _tuple0*_tmp46D;struct Cyc_Absyn_Vardecl*_tmp46C;struct Cyc_Absyn_Vardecl*_tmp2D9=(_tmp46C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp46C[0]=(struct Cyc_Absyn_Vardecl)((_tmp46E.sc=Cyc_Absyn_Public,((_tmp46E.name=(
(_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D->f1=Cyc_Absyn_Loc_n,((_tmp46D->f2=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd))).f1,_tmp46D)))))),((_tmp46E.tq=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd))).f2,((_tmp46E.type=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd))).f3,((_tmp46E.initializer=0,((_tmp46E.rgn=param_rgn,((_tmp46E.attributes=0,((_tmp46E.escapes=0,_tmp46E)))))))))))))))),_tmp46C)));
# 1245
{struct Cyc_List_List _tmp471;struct Cyc_List_List*_tmp470;_tmp2D7=((_tmp470=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp470[0]=(struct Cyc_List_List)((_tmp471.hd=_tmp2D9,((_tmp471.tl=_tmp2D7,_tmp471)))),_tmp470))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp474;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp473;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2DC=(_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473[0]=((_tmp474.tag=3,((_tmp474.f1=_tmp2D9,_tmp474)))),_tmp473)));
struct _dyneither_ptr*_tmp2DD=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp2D8))->hd))).f1;
struct Cyc_Tcenv_Bindings*_tmp475;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp475=_region_malloc(r,sizeof(*_tmp475)),((_tmp475->v=_tmp2DD,((_tmp475->b=(void*)_tmp2DC,((_tmp475->tl=locals,_tmp475))))))));};}}
# 1250
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp36=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp2E5;struct Cyc_Absyn_Tqual _tmp2E6;void*_tmp2E7;int _tmp2E8;struct Cyc_Absyn_VarargInfo _tmp2E4=_stmttmp36;_tmp2E5=_tmp2E4.name;_tmp2E6=_tmp2E4.tq;_tmp2E7=_tmp2E4.type;_tmp2E8=_tmp2E4.inject;
if(_tmp2E5 != 0){
void*_tmp2E9=Cyc_Absyn_dyneither_typ(_tmp2E7,param_rgn,_tmp2E6,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp47B;struct _tuple0*_tmp47A;struct Cyc_Absyn_Vardecl*_tmp479;struct Cyc_Absyn_Vardecl*_tmp2EA=(_tmp479=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp479[0]=(struct Cyc_Absyn_Vardecl)((_tmp47B.sc=Cyc_Absyn_Public,((_tmp47B.name=(
(_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->f1=Cyc_Absyn_Loc_n,((_tmp47A->f2=(struct _dyneither_ptr*)_check_null(_tmp2E5),_tmp47A)))))),((_tmp47B.tq=
Cyc_Absyn_empty_tqual(0),((_tmp47B.type=_tmp2E9,((_tmp47B.initializer=0,((_tmp47B.rgn=param_rgn,((_tmp47B.attributes=0,((_tmp47B.escapes=0,_tmp47B)))))))))))))))),_tmp479)));
# 1261
{struct Cyc_List_List*_tmp47C;_tmp2D7=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->hd=_tmp2EA,((_tmp47C->tl=_tmp2D7,_tmp47C))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp47F;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp47E;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2EC=(_tmp47E=_cycalloc(sizeof(*_tmp47E)),((_tmp47E[0]=((_tmp47F.tag=3,((_tmp47F.f1=_tmp2EA,_tmp47F)))),_tmp47E)));
struct _dyneither_ptr*_tmp2ED=(struct _dyneither_ptr*)_check_null(_tmp2E5);
struct Cyc_Tcenv_Bindings*_tmp480;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp480=_region_malloc(r,sizeof(*_tmp480)),((_tmp480->v=_tmp2ED,((_tmp480->b=(void*)_tmp2EC,((_tmp480->tl=locals,_tmp480))))))));};}else{
# 1266
const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_Tcutil_terr(loc,((_tmp483="missing name for varargs",_tag_dyneither(_tmp483,sizeof(char),25))),_tag_dyneither(_tmp482,sizeof(void*),0)));}}
# 1268
{struct Cyc_Core_Opt _tmp486;struct Cyc_Core_Opt*_tmp485;fd->param_vardecls=((_tmp485=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp485[0]=(struct Cyc_Core_Opt)((_tmp486.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D7),_tmp486)),_tmp485))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4A4;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4A2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4A1;struct Cyc_List_List*_tmp4A0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp49F;struct Cyc_Tcenv_CtrlEnv*_tmp49E;struct Cyc_Tcenv_SharedFenv*_tmp49D;struct Cyc_Tcenv_Fenv*_tmp49C;return(_tmp49C=_region_malloc(r,sizeof(*_tmp49C)),((_tmp49C->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D->frgn=r,((_tmp49D->return_typ=fd->ret_type,((_tmp49D->seen_labels=
# 1273
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp49D->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp49D->delayed_effect_checks=0,((_tmp49D->delayed_constraint_checks=0,_tmp49D)))))))))))))),((_tmp49C->type_vars=(struct Cyc_List_List*)_tmp2D5,((_tmp49C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2D6,((_tmp49C->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp49C->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp49C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1282
(_tmp49E=_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E->ctrl_rgn=r,((_tmp49E->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp49E->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp49E->fallthru_clause=0,((_tmp49E->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp49E->try_depth=0,_tmp49E)))))))))))))),((_tmp49C->capability=(void*)((void*)(
(_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp49F.tag=24,((_tmp49F.f1=((_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->hd=(void*)((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1[0]=((_tmp4A2.tag=23,((_tmp4A2.f1=param_rgn,_tmp4A2)))),_tmp4A1)))),((_tmp4A3->tl=(
(_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->hd=(void*)_check_null(fd->effect),((_tmp4A0->tl=0,_tmp4A0)))))),_tmp4A3)))))),_tmp49F)))),_tmp4A4))))),((_tmp49C->curr_rgn=(void*)param_rgn,((_tmp49C->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1286
((_tmp49C->flags).in_notreadctxt=0,(((_tmp49C->flags).in_lhs=0,(((_tmp49C->flags).in_new=0,(((_tmp49C->flags).abstract_ok=0,_tmp49C->flags)))))))),((_tmp49C->fnrgn=(struct _RegionHandle*)r,_tmp49C)))))))))))))))))))));};};}
# 1291
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp30A;struct Cyc_RgnOrder_RgnPO*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Tcenv_SharedFenv*_tmp30D;struct _RegionHandle*_tmp30E;struct Cyc_Tcenv_Fenv*_tmp309=old_fenv;_tmp30A=_tmp309->locals;_tmp30B=_tmp309->region_order;_tmp30C=_tmp309->type_vars;_tmp30D=_tmp309->shared;_tmp30E=_tmp309->fnrgn;{
# 1294
struct _RegionHandle*_tmp30F=((struct Cyc_Tcenv_SharedFenv*)_check_null(_tmp30D))->frgn;
const struct Cyc_Tcenv_Bindings*_tmp310=_tmp30A;
struct _dyneither_ptr*_tmp4B1;const char*_tmp4B0;void*_tmp4AF[1];struct Cyc_String_pa_PrintArg_struct _tmp4AE;struct Cyc_Absyn_Tvar*_tmp4AD;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD->name=((_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1[0]=(struct _dyneither_ptr)((_tmp4AE.tag=0,((_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4AF[0]=& _tmp4AE,Cyc_aprintf(((_tmp4B0="`%s",_tag_dyneither(_tmp4B0,sizeof(char),4))),_tag_dyneither(_tmp4AF,sizeof(void*),1)))))))),_tmp4B1)))),((_tmp4AD->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4AD->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4AD)))))));
# 1300
{struct Cyc_List_List*_tmp311=((struct Cyc_Absyn_Fndecl*)_check_null(fd))->tvs;for(0;_tmp311 != 0;_tmp311=_tmp311->tl){
struct Cyc_Absyn_Kind*_stmttmp37=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp311->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp313;enum Cyc_Absyn_AliasQual _tmp314;struct Cyc_Absyn_Kind*_tmp312=_stmttmp37;_tmp313=_tmp312->kind;_tmp314=_tmp312->aliasqual;
if(_tmp313 == Cyc_Absyn_RgnKind){
if(_tmp314 == Cyc_Absyn_Aliasable)
_tmp30B=Cyc_RgnOrder_add_youngest(_tmp30F,_tmp30B,(struct Cyc_Absyn_Tvar*)_tmp311->hd,0,0);else{
# 1306
const char*_tmp4B4;void*_tmp4B3;(_tmp4B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B4="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4B4,sizeof(char),39))),_tag_dyneither(_tmp4B3,sizeof(void*),0)));}}}}
# 1308
_tmp30B=Cyc_RgnOrder_add_youngest(_tmp30F,_tmp30B,rgn0,0,0);{
struct Cyc_List_List*_tmp4B5;struct Cyc_List_List*_tmp317=(_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5->hd=rgn0,((_tmp4B5->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(((struct Cyc_Absyn_Fndecl*)_check_null(fd))->tvs,_tmp30C),_tmp4B5)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp317);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4B8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4B7;void*param_rgn=(void*)((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((_tmp4B7[0]=((_tmp4B8.tag=2,((_tmp4B8.f1=rgn0,_tmp4B8)))),_tmp4B7))));
struct Cyc_List_List*_tmp318=0;
{struct Cyc_List_List*_tmp319=((struct Cyc_Absyn_Fndecl*)_check_null(fd))->args;for(0;_tmp319 != 0;_tmp319=_tmp319->tl){
struct Cyc_Absyn_Vardecl _tmp4BE;struct _tuple0*_tmp4BD;struct Cyc_Absyn_Vardecl*_tmp4BC;struct Cyc_Absyn_Vardecl*_tmp31A=(_tmp4BC=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4BC[0]=(struct Cyc_Absyn_Vardecl)((_tmp4BE.sc=Cyc_Absyn_Public,((_tmp4BE.name=(
(_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD->f1=Cyc_Absyn_Loc_n,((_tmp4BD->f2=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp319))->hd))).f1,_tmp4BD)))))),((_tmp4BE.tq=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp319))->hd))).f2,((_tmp4BE.type=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp319))->hd))).f3,((_tmp4BE.initializer=0,((_tmp4BE.rgn=param_rgn,((_tmp4BE.attributes=0,((_tmp4BE.escapes=0,_tmp4BE)))))))))))))))),_tmp4BC)));
# 1321
{struct Cyc_List_List _tmp4C1;struct Cyc_List_List*_tmp4C0;_tmp318=((_tmp4C0=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4C0[0]=(struct Cyc_List_List)((_tmp4C1.hd=_tmp31A,((_tmp4C1.tl=_tmp318,_tmp4C1)))),_tmp4C0))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4C4;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4C3;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp31D=(_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C4.tag=3,((_tmp4C4.f1=_tmp31A,_tmp4C4)))),_tmp4C3)));
struct _dyneither_ptr*_tmp31E=(*((struct _tuple8*)_check_null((struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp319))->hd))).f1;
struct Cyc_Tcenv_Bindings*_tmp4C5;_tmp310=(const struct Cyc_Tcenv_Bindings*)((_tmp4C5=_region_malloc(_tmp30F,sizeof(*_tmp4C5)),((_tmp4C5->v=_tmp31E,((_tmp4C5->b=(void*)_tmp31D,((_tmp4C5->tl=_tmp310,_tmp4C5))))))));};}}
# 1326
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp38=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp326;struct Cyc_Absyn_Tqual _tmp327;void*_tmp328;int _tmp329;struct Cyc_Absyn_VarargInfo _tmp325=_stmttmp38;_tmp326=_tmp325.name;_tmp327=_tmp325.tq;_tmp328=_tmp325.type;_tmp329=_tmp325.inject;
if(_tmp326 != 0){
void*_tmp32A=Cyc_Absyn_dyneither_typ(_tmp328,param_rgn,_tmp327,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4CB;struct _tuple0*_tmp4CA;struct Cyc_Absyn_Vardecl*_tmp4C9;struct Cyc_Absyn_Vardecl*_tmp32B=(_tmp4C9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4C9[0]=(struct Cyc_Absyn_Vardecl)((_tmp4CB.sc=Cyc_Absyn_Public,((_tmp4CB.name=(
(_tmp4CA=_cycalloc(sizeof(*_tmp4CA)),((_tmp4CA->f1=Cyc_Absyn_Loc_n,((_tmp4CA->f2=(struct _dyneither_ptr*)_check_null(_tmp326),_tmp4CA)))))),((_tmp4CB.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4CB.type=_tmp32A,((_tmp4CB.initializer=0,((_tmp4CB.rgn=param_rgn,((_tmp4CB.attributes=0,((_tmp4CB.escapes=0,_tmp4CB)))))))))))))))),_tmp4C9)));
# 1337
{struct Cyc_List_List*_tmp4CC;_tmp318=((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((_tmp4CC->hd=_tmp32B,((_tmp4CC->tl=_tmp318,_tmp4CC))))));}{
struct _dyneither_ptr*_tmp32D=(struct _dyneither_ptr*)_check_null(_tmp326);
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4CF;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4CE;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp32E=(_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE[0]=((_tmp4CF.tag=3,((_tmp4CF.f1=_tmp32B,_tmp4CF)))),_tmp4CE)));
struct Cyc_Tcenv_Bindings*_tmp4D0;_tmp310=(const struct Cyc_Tcenv_Bindings*)((_tmp4D0=_region_malloc(_tmp30F,sizeof(*_tmp4D0)),((_tmp4D0->v=_tmp32D,((_tmp4D0->b=(void*)_tmp32E,((_tmp4D0->tl=_tmp310,_tmp4D0))))))));};}else{
# 1342
const char*_tmp4D3;void*_tmp4D2;(_tmp4D2=0,Cyc_Tcutil_terr(loc,((_tmp4D3="missing name for varargs",_tag_dyneither(_tmp4D3,sizeof(char),25))),_tag_dyneither(_tmp4D2,sizeof(void*),0)));}}
# 1344
{struct Cyc_Core_Opt _tmp4D6;struct Cyc_Core_Opt*_tmp4D5;fd->param_vardecls=((_tmp4D5=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4D5[0]=(struct Cyc_Core_Opt)((_tmp4D6.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp318),_tmp4D6)),_tmp4D5))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F4;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4F2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F1;struct Cyc_List_List*_tmp4F0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4EF;struct Cyc_Tcenv_CtrlEnv*_tmp4EE;struct Cyc_Tcenv_SharedFenv*_tmp4ED;struct Cyc_Tcenv_Fenv*_tmp4EC;return(_tmp4EC=_region_malloc(_tmp30F,sizeof(*_tmp4EC)),((_tmp4EC->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4ED=_region_malloc(_tmp30F,sizeof(*_tmp4ED)),((_tmp4ED->frgn=_tmp30F,((_tmp4ED->return_typ=fd->ret_type,((_tmp4ED->seen_labels=
# 1349
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp30F,Cyc_strptrcmp),((_tmp4ED->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp30F,Cyc_strptrcmp),((_tmp4ED->delayed_effect_checks=0,((_tmp4ED->delayed_constraint_checks=0,_tmp4ED)))))))))))))),((_tmp4EC->type_vars=(struct Cyc_List_List*)_tmp317,((_tmp4EC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp30B,((_tmp4EC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp310,((_tmp4EC->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4EC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1358
(_tmp4EE=_region_malloc(_tmp30F,sizeof(*_tmp4EE)),((_tmp4EE->ctrl_rgn=_tmp30F,((_tmp4EE->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4EE->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4EE->fallthru_clause=0,((_tmp4EE->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4EE->try_depth=0,_tmp4EE)))))))))))))),((_tmp4EC->capability=(void*)((void*)(
(_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4EF.tag=24,((_tmp4EF.f1=((_tmp4F3=_cycalloc(sizeof(*_tmp4F3)),((_tmp4F3->hd=(void*)((_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F2.tag=23,((_tmp4F2.f1=param_rgn,_tmp4F2)))),_tmp4F1)))),((_tmp4F3->tl=(
(_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0->hd=(void*)_check_null(fd->effect),((_tmp4F0->tl=0,_tmp4F0)))))),_tmp4F3)))))),_tmp4EF)))),_tmp4F4))))),((_tmp4EC->curr_rgn=(void*)param_rgn,((_tmp4EC->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1362
((_tmp4EC->flags).in_notreadctxt=0,(((_tmp4EC->flags).in_lhs=0,(((_tmp4EC->flags).in_new=0,(((_tmp4EC->flags).abstract_ok=0,_tmp4EC->flags)))))))),((_tmp4EC->fnrgn=(struct _RegionHandle*)_tmp30E,_tmp4EC)))))))))))))))))))));};};};};}
