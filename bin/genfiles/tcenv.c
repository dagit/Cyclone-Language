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
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244
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
# 49
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
# 122
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 279
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51
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
# 430
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748
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
# 114
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
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 152
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 163
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 169
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 174
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 178
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 182
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 185
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 188
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 196
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 200
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 202
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 204
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 208
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 211
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 214
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
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 138
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 141
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 162
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 166
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 222
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 294
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 296
int Cyc_Tcutil_new_tvar_id();
# 298
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);char Cyc_Tcenv_Env_error[10]="Env_error";struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};
# 88 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
{const char*_tmp345;void*_tmp344[1];struct Cyc_String_pa_PrintArg_struct _tmp343;(_tmp343.tag=0,((_tmp343.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp344[0]=& _tmp343,Cyc_fprintf(Cyc_stderr,((_tmp345="Internal error in tcenv: %s\n",_tag_dyneither(_tmp345,sizeof(char),29))),_tag_dyneither(_tmp344,sizeof(void*),1)))))));}
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
# 192
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;int in_lhs;int in_new;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 221
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 224
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 228
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 233
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp346;return(_tmp346=_region_malloc(r,sizeof(*_tmp346)),((_tmp346->grgn=r,((_tmp346->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp346->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp346->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp346->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp346->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp346->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp346->availables=0,_tmp346)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 244
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 246
{struct Cyc_Core_Opt*_tmp347;Cyc_Tcutil_empty_var_set=((_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp347))));}{
# 248
struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp348;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348[0]=Cyc_Absyn_exn_tud,_tmp348)))));}
{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp34E;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp34D;struct _tuple12*_tmp34C;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,(
(_tmp34C=_region_malloc(r,sizeof(*_tmp34C)),((_tmp34C->f1=(void*)(
(_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E[0]=((_tmp34D.tag=2,((_tmp34D.f1=Cyc_Absyn_exn_tud,((_tmp34D.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp34D)))))),_tmp34E)))),((_tmp34C->f2=1,_tmp34C)))))));}}{
# 255
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmpA);
struct Cyc_Tcenv_Tenv*_tmp34F;return(_tmp34F=_region_malloc(r,sizeof(*_tmp34F)),((_tmp34F->ns=0,((_tmp34F->ae=ae,((_tmp34F->le=0,((_tmp34F->allow_valueof=0,_tmp34F)))))))));};};}
# 261
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 263
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 267
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp350;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp350="outer_namespace",_tag_dyneither(_tmp350,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 273
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 280
if(n2 != 0)return 0;
return 1;}
# 286
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 291
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))_tmp14=1;if(!_tmp14){
lookup(ge2,v,0);{
const char*_tmp354;void*_tmp353[1];struct Cyc_String_pa_PrintArg_struct _tmp352;(_tmp352.tag=0,((_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp353[0]=& _tmp352,Cyc_Tcutil_terr(loc,((_tmp354="%s is ambiguous",_tag_dyneither(_tmp354,sizeof(char),16))),_tag_dyneither(_tmp353,sizeof(void*),1)))))));};
# 295
;_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;void*_tmp1B;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp1A=(struct Cyc_Dict_Absent_exn_struct*)_tmp19;if(_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 297
 goto _LL0;_LL3: _tmp1B=_tmp19;_LL4:(void)_throw(_tmp1B);_LL0:;}};}}
# 299
return;}
# 302
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 307
{struct _handler_cons _tmp1C;_push_handler(& _tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1F=result;_npop_handler(0);return _tmp1F;};}
# 308
;_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;void*_tmp23;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp22=(struct Cyc_Dict_Absent_exn_struct*)_tmp21;if(_tmp22->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 311
 goto _LL5;_LL8: _tmp23=_tmp21;_LL9:(void)_throw(_tmp23);_LL5:;}};}
# 313
{struct Cyc_List_List*_tmp24=ge->availables;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp24->hd);
struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp24->hd,_tmp24->tl);{
void*_tmp28=result;_npop_handler(0);return _tmp28;};}
# 316
;_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp2A=_tmp26;void*_tmp2C;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp2B=(struct Cyc_Dict_Absent_exn_struct*)_tmp2A;if(_tmp2B->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 319
 goto _LLA;_LLD: _tmp2C=_tmp2A;_LLE:(void)_throw(_tmp2C);_LLA:;}};}}
# 321
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp357;struct Cyc_Dict_Absent_exn_struct*_tmp356;(int)_throw((void*)((_tmp356=_cycalloc_atomic(sizeof(*_tmp356)),((_tmp356[0]=((_tmp357.tag=Cyc_Dict_Absent,_tmp357)),_tmp356)))));}
# 324
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 328
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2F=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2F,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 335
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp31;struct _dyneither_ptr*_tmp32;struct _tuple0*_tmp30=q;_tmp31=_tmp30->f1;_tmp32=_tmp30->f2;{
union Cyc_Absyn_Nmspace _tmp33=_tmp31;struct _dyneither_ptr*_tmp34;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp37;_LL10: if((_tmp33.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp33.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL14;_LL13:
# 340
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp32,is_use);_LL14: if((_tmp33.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp33.Rel_n).val == 0)goto _LL16;_tmp34=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp33.Rel_n).val)->hd;_tmp35=((struct Cyc_List_List*)(_tmp33.Rel_n).val)->tl;_LL15: {
# 342
struct Cyc_Tcenv_Genv*_tmp38=Cyc_Tcenv_lookup_namespace(te,loc,_tmp34,_tmp35);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp38,_tmp32,is_use);}_LL16: if((_tmp33.C_n).tag != 3)goto _LL18;_tmp36=(struct Cyc_List_List*)(_tmp33.C_n).val;_LL17:
 _tmp37=_tmp36;goto _LL19;_LL18: if((_tmp33.Abs_n).tag != 2)goto _LLF;_tmp37=(struct Cyc_List_List*)(_tmp33.Abs_n).val;_LL19:
# 346
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37),_tmp32,is_use);_LLF:;};}
# 351
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 353
struct Cyc_List_List*_tmp39=te->ns;
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_List_List*_tmp3B=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp3C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp39);
struct Cyc_List_List*_tmp3D=_tmp3C->availables;
struct Cyc_Set_Set*_tmp3E=_tmp3C->namespaces;
# 361
{struct Cyc_List_List*_tmp3F=_tmp3D;for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){
struct Cyc_Set_Set*_tmp40=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3F->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){
struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;_tmp3B=((_tmp359=_region_malloc(temp,sizeof(*_tmp359)),((_tmp359->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A->hd=n,((_tmp35A->tl=ns,_tmp35A))))))),((_tmp359->tl=_tmp3B,_tmp359))))));}}}
# 368
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3E,n)){
struct Cyc_List_List*_tmp35D;struct Cyc_List_List*_tmp35C;_tmp3B=((_tmp35C=_region_malloc(temp,sizeof(*_tmp35C)),((_tmp35C->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D->hd=n,((_tmp35D->tl=ns,_tmp35D))))))),((_tmp35C->tl=_tmp3B,_tmp35C))))));}
# 374
if(_tmp3B != 0){
if(_tmp3B->tl != 0){
const char*_tmp361;void*_tmp360[1];struct Cyc_String_pa_PrintArg_struct _tmp35F;(_tmp35F.tag=0,((_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp360[0]=& _tmp35F,Cyc_Tcutil_terr(loc,((_tmp361="%s is ambiguous",_tag_dyneither(_tmp361,sizeof(char),16))),_tag_dyneither(_tmp360,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp48=(struct Cyc_List_List*)_tmp3B->hd;_npop_handler(0);return _tmp48;};}
# 379
if(_tmp39 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp364;struct Cyc_Dict_Absent_exn_struct*_tmp363;(int)_throw((void*)((_tmp363=_cycalloc_atomic(sizeof(*_tmp363)),((_tmp363[0]=((_tmp364.tag=Cyc_Dict_Absent,_tmp364)),_tmp363)))));}
# 382
_tmp39=Cyc_Tcenv_outer_namespace(_tmp39);}}
# 355
;_pop_region(temp);}
# 390
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,v);}
# 394
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4D;struct _dyneither_ptr*_tmp4E;struct _tuple0*_tmp4C=q;_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;{
union Cyc_Absyn_Nmspace _tmp4F=_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;struct _dyneither_ptr*_tmp52;struct Cyc_List_List*_tmp53;_LL1B: if((_tmp4F.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4F.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val != 0)goto _LL1F;_LL1E:
# 399
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4E,0);_LL1F: if((_tmp4F.C_n).tag != 3)goto _LL21;_tmp50=(struct Cyc_List_List*)(_tmp4F.C_n).val;_LL20:
 _tmp51=_tmp50;goto _LL22;_LL21: if((_tmp4F.Abs_n).tag != 2)goto _LL23;_tmp51=(struct Cyc_List_List*)(_tmp4F.Abs_n).val;_LL22: {
# 402
struct Cyc_Dict_Dict _tmp54=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp51))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4E);}_LL23: if((_tmp4F.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val == 0)goto _LL1A;_tmp52=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->hd;_tmp53=((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->tl;_LL24: {
# 405
struct Cyc_Dict_Dict _tmp55=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp52,_tmp53))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp55,_tmp4E);}_LL1A:;};}
# 410
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp56=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,v);}
# 414
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp59;struct _tuple0*_tmp57=q;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;{
union Cyc_Absyn_Nmspace _tmp5A=_tmp58;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;_LL26: if((_tmp5A.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp5A.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val != 0)goto _LL2A;_LL29:
# 419
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp59,0);_LL2A: if((_tmp5A.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val == 0)goto _LL2C;_tmp5B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->hd;_tmp5C=((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->tl;_LL2B: {
# 421
struct Cyc_Dict_Dict _tmp5F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5F,_tmp59);}_LL2C: if((_tmp5A.C_n).tag != 3)goto _LL2E;_tmp5D=(struct Cyc_List_List*)(_tmp5A.C_n).val;_LL2D:
 _tmp5E=_tmp5D;goto _LL2F;_LL2E: if((_tmp5A.Abs_n).tag != 2)goto _LL25;_tmp5E=(struct Cyc_List_List*)(_tmp5A.Abs_n).val;_LL2F: {
# 425
struct Cyc_Dict_Dict _tmp60=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5E))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp60,_tmp59);}_LL25:;};}
# 430
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 435
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp62;struct _dyneither_ptr*_tmp63;struct _tuple0*_tmp61=q;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
union Cyc_Absyn_Nmspace _tmp64=_tmp62;struct _dyneither_ptr*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp68;_LL31: if((_tmp64.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp64.Rel_n).val != 0)goto _LL33;_LL32: {
# 440
struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){{struct Cyc_Absyn_Datatypedecl***_tmp365;struct Cyc_Absyn_Datatypedecl***_tmp6D=(_tmp365=_region_malloc(r,sizeof(*_tmp365)),((_tmp365[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp63,0),_tmp365)));_npop_handler(0);return _tmp6D;};_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6F=_tmp6A;void*_tmp71;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp70=(struct Cyc_Dict_Absent_exn_struct*)_tmp6F;if(_tmp70->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp71=_tmp6F;_LL3F:(void)_throw(_tmp71);_LL3B:;}};}_LL33: if((_tmp64.Loc_n).tag != 4)goto _LL35;_LL34:
# 443
{const char*_tmp368;void*_tmp367;(_tmp367=0,Cyc_Tcutil_terr(loc,((_tmp368="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp368,sizeof(char),33))),_tag_dyneither(_tmp367,sizeof(void*),0)));}
return 0;_LL35: if((_tmp64.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp64.Rel_n).val == 0)goto _LL37;_tmp65=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp64.Rel_n).val)->hd;_tmp66=((struct Cyc_List_List*)(_tmp64.Rel_n).val)->tl;_LL36: {
# 450
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))_tmp76=1;if(!_tmp76){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp65,_tmp66);;_pop_handler();}else{void*_tmp75=(void*)_exn_thrown;void*_tmp78=_tmp75;void*_tmp7A;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 454
{const char*_tmp36B;void*_tmp36A;(_tmp36A=0,Cyc_Tcutil_terr(loc,((_tmp36B="bad qualified name for @extensible datatype",_tag_dyneither(_tmp36B,sizeof(char),44))),_tag_dyneither(_tmp36A,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp36E;struct Cyc_Dict_Absent_exn_struct*_tmp36D;(int)_throw((void*)((_tmp36D=_cycalloc_atomic(sizeof(*_tmp36D)),((_tmp36D[0]=((_tmp36E.tag=Cyc_Dict_Absent,_tmp36E)),_tmp36D)))));};_LL43: _tmp7A=_tmp78;_LL44:(void)_throw(_tmp7A);_LL40:;}};}{
# 457
struct Cyc_Dict_Dict _tmp7F=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp36F;return(_tmp36F=_region_malloc(r,sizeof(*_tmp36F)),((_tmp36F[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp63),_tmp36F)));};}_LL37: if((_tmp64.C_n).tag != 3)goto _LL39;_tmp67=(struct Cyc_List_List*)(_tmp64.C_n).val;_LL38:
 _tmp68=_tmp67;goto _LL3A;_LL39: if((_tmp64.Abs_n).tag != 2)goto _LL30;_tmp68=(struct Cyc_List_List*)(_tmp64.Abs_n).val;_LL3A: {
# 461
struct Cyc_Dict_Dict _tmp81=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp68))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp370;return(_tmp370=_region_malloc(r,sizeof(*_tmp370)),((_tmp370[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,_tmp63),_tmp370)));}_LL30:;};}
# 466
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp83=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp83,v);}
# 470
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp84=q;_tmp85=_tmp84->f1;_tmp86=_tmp84->f2;{
union Cyc_Absyn_Nmspace _tmp87=_tmp85;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;_LL46: if((_tmp87.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp87.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp87.Rel_n).val != 0)goto _LL4A;_LL49:
# 475
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp86,0);_LL4A: if((_tmp87.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp87.Rel_n).val == 0)goto _LL4C;_tmp88=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp87.Rel_n).val)->hd;_tmp89=((struct Cyc_List_List*)(_tmp87.Rel_n).val)->tl;_LL4B: {
# 477
struct Cyc_Dict_Dict _tmp8C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8C,_tmp86);}_LL4C: if((_tmp87.C_n).tag != 3)goto _LL4E;_tmp8A=(struct Cyc_List_List*)(_tmp87.C_n).val;_LL4D:
 _tmp8B=_tmp8A;goto _LL4F;_LL4E: if((_tmp87.Abs_n).tag != 2)goto _LL45;_tmp8B=(struct Cyc_List_List*)(_tmp87.Abs_n).val;_LL4F: {
# 481
struct Cyc_Dict_Dict _tmp8D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp8B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,_tmp86);}_LL45:;};}
# 486
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8E,v);}
# 490
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*_tmp8F=q;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{
union Cyc_Absyn_Nmspace _tmp92=_tmp90;struct _dyneither_ptr*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;_LL51: if((_tmp92.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp92.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp92.Rel_n).val != 0)goto _LL55;_LL54:
# 495
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp91,0);_LL55: if((_tmp92.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp92.Rel_n).val == 0)goto _LL57;_tmp93=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp92.Rel_n).val)->hd;_tmp94=((struct Cyc_List_List*)(_tmp92.Rel_n).val)->tl;_LL56: {
# 497
struct Cyc_Dict_Dict _tmp97=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp93,_tmp94))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp97,_tmp91);}_LL57: if((_tmp92.C_n).tag != 3)goto _LL59;_tmp95=(struct Cyc_List_List*)(_tmp92.C_n).val;_LL58:
 _tmp96=_tmp95;goto _LL5A;_LL59: if((_tmp92.Abs_n).tag != 2)goto _LL50;_tmp96=(struct Cyc_List_List*)(_tmp92.Abs_n).val;_LL5A: {
# 501
struct Cyc_Dict_Dict _tmp98=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp96))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp98,_tmp91);}_LL50:;};}
# 508
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp99=te->le;
if(te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp99);}
# 514
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 518
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp9A=te->le;
if(_tmp9A != 0){
struct _RegionHandle*_tmp9C;struct Cyc_Tcenv_Fenv*_tmp9B=_tmp9A;_tmp9C=_tmp9B->fnrgn;
return _tmp9C;}
# 524
return Cyc_Tcenv_coerce_heap_region();}
# 527
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp371;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp371="put_fenv",_tag_dyneither(_tmp371,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp372;return(_tmp372=_region_malloc(l,sizeof(*_tmp372)),((_tmp372->ns=te->ns,((_tmp372->ae=te->ae,((_tmp372->le=fe,((_tmp372->allow_valueof=te->allow_valueof,_tmp372)))))))));};}
# 532
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp373;return(_tmp373=_region_malloc(l,sizeof(*_tmp373)),((_tmp373->ns=te->ns,((_tmp373->ae=te->ae,((_tmp373->le=0,((_tmp373->allow_valueof=te->allow_valueof,_tmp373)))))))));}
# 536
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 538
struct Cyc_Tcenv_SharedFenv*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_RgnOrder_RgnPO*_tmpA3;const struct Cyc_Tcenv_Bindings*_tmpA4;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Tcenv_CtrlEnv*_tmpA6;void*_tmpA7;void*_tmpA8;int _tmpA9;int _tmpAA;int _tmpAB;struct _RegionHandle*_tmpAC;struct Cyc_Tcenv_Fenv _tmpA0=*f;_tmpA1=_tmpA0.shared;_tmpA2=_tmpA0.type_vars;_tmpA3=_tmpA0.region_order;_tmpA4=_tmpA0.locals;_tmpA5=_tmpA0.encloser;_tmpA6=_tmpA0.ctrl_env;_tmpA7=_tmpA0.capability;_tmpA8=_tmpA0.curr_rgn;_tmpA9=_tmpA0.in_notreadctxt;_tmpAA=_tmpA0.in_lhs;_tmpAB=_tmpA0.in_new;_tmpAC=_tmpA0.fnrgn;{
# 540
struct Cyc_Tcenv_Fenv*_tmp374;return(_tmp374=_region_malloc(l,sizeof(*_tmp374)),((_tmp374->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA1,((_tmp374->type_vars=(struct Cyc_List_List*)_tmpA2,((_tmp374->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA3,((_tmp374->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA4,((_tmp374->encloser=(struct Cyc_Absyn_Stmt*)_tmpA5,((_tmp374->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA6,((_tmp374->capability=(void*)_tmpA7,((_tmp374->curr_rgn=(void*)_tmpA8,((_tmp374->in_notreadctxt=(int)_tmpA9,((_tmp374->in_lhs=(int)_tmpAA,((_tmp374->in_new=(int)_tmpAB,((_tmp374->fnrgn=(struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp374)))))))))))))))))))))))));};}
# 545
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 547
struct Cyc_Tcenv_SharedFenv*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_RgnOrder_RgnPO*_tmpB1;const struct Cyc_Tcenv_Bindings*_tmpB2;struct Cyc_Absyn_Stmt*_tmpB3;struct Cyc_Tcenv_CtrlEnv*_tmpB4;void*_tmpB5;void*_tmpB6;int _tmpB7;int _tmpB8;int _tmpB9;struct Cyc_Tcenv_Fenv _tmpAE=*f;_tmpAF=_tmpAE.shared;_tmpB0=_tmpAE.type_vars;_tmpB1=_tmpAE.region_order;_tmpB2=_tmpAE.locals;_tmpB3=_tmpAE.encloser;_tmpB4=_tmpAE.ctrl_env;_tmpB5=_tmpAE.capability;_tmpB6=_tmpAE.curr_rgn;_tmpB7=_tmpAE.in_notreadctxt;_tmpB8=_tmpAE.in_lhs;_tmpB9=_tmpAE.in_new;{
# 550
struct _RegionHandle*_tmpBB;void*_tmpBC;void*_tmpBD;const struct _tuple10*_tmpBE;void*_tmpBF;int _tmpC0;struct Cyc_Tcenv_CtrlEnv _tmpBA=*_tmpB4;_tmpBB=_tmpBA.ctrl_rgn;_tmpBC=_tmpBA.continue_stmt;_tmpBD=_tmpBA.break_stmt;_tmpBE=_tmpBA.fallthru_clause;_tmpBF=_tmpBA.next_stmt;_tmpC0=_tmpBA.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp375;struct Cyc_Tcenv_CtrlEnv*_tmpC1=
(_tmp375=_region_malloc(l,sizeof(*_tmp375)),((_tmp375->ctrl_rgn=_tmpBB,((_tmp375->continue_stmt=_tmpBC,((_tmp375->break_stmt=_tmpBD,((_tmp375->fallthru_clause=_tmpBE,((_tmp375->next_stmt=_tmpBF,((_tmp375->try_depth=_tmpC0,_tmp375)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp376;return(_tmp376=_region_malloc(l,sizeof(*_tmp376)),((_tmp376->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAF,((_tmp376->type_vars=(struct Cyc_List_List*)_tmpB0,((_tmp376->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB1,((_tmp376->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB2,((_tmp376->encloser=(struct Cyc_Absyn_Stmt*)_tmpB3,((_tmp376->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC1,((_tmp376->capability=(void*)_tmpB5,((_tmp376->curr_rgn=(void*)_tmpB6,((_tmp376->in_notreadctxt=(int)_tmpB7,((_tmp376->in_lhs=(int)_tmpB8,((_tmp376->in_new=(int)_tmpB9,((_tmp376->fnrgn=(struct _RegionHandle*)l,(struct Cyc_Tcenv_Fenv*)_tmp376)))))))))))))))))))))))));};};}
# 560
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmpC6;const char*_tmp377;struct Cyc_Tcenv_Fenv*_tmpC5=Cyc_Tcenv_get_fenv(te,((_tmp377="return_typ",_tag_dyneither(_tmp377,sizeof(char),11))));_tmpC6=_tmpC5->shared;{
void*_tmpC8;struct Cyc_Tcenv_SharedFenv _tmpC7=*_tmpC6;_tmpC8=_tmpC7.return_typ;
return _tmpC8;};}
# 566
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC9=te->le;
if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpCB;struct Cyc_Tcenv_Fenv _tmpCA=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC9));_tmpCB=_tmpCA.type_vars;
return _tmpCB;};}
# 573
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp378;return(_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378->v=Cyc_Tcenv_lookup_type_vars(te),_tmp378)));}
# 577
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 579
struct Cyc_Tcenv_SharedFenv*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_RgnOrder_RgnPO*_tmpD1;const struct Cyc_Tcenv_Bindings*_tmpD2;struct Cyc_Absyn_Stmt*_tmpD3;struct Cyc_Tcenv_CtrlEnv*_tmpD4;void*_tmpD5;void*_tmpD6;int _tmpD7;int _tmpD8;int _tmpD9;const char*_tmp379;struct Cyc_Tcenv_Fenv*_tmpCE=
# 581
Cyc_Tcenv_get_fenv(te,((_tmp379="add_type_vars",_tag_dyneither(_tmp379,sizeof(char),14))));_tmpCF=_tmpCE->shared;_tmpD0=_tmpCE->type_vars;_tmpD1=_tmpCE->region_order;_tmpD2=_tmpCE->locals;_tmpD3=_tmpCE->encloser;_tmpD4=_tmpCE->ctrl_env;_tmpD5=_tmpCE->capability;_tmpD6=_tmpCE->curr_rgn;_tmpD7=_tmpCE->in_notreadctxt;_tmpD8=_tmpCE->in_lhs;_tmpD9=_tmpCE->in_new;
# 583
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD0);
Cyc_Tcutil_check_unique_tvars(loc,_tmpDA);{
struct Cyc_Tcenv_Fenv*_tmp37A;struct Cyc_Tcenv_Fenv*_tmpDB=(_tmp37A=_region_malloc(r,sizeof(*_tmp37A)),((_tmp37A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCF,((_tmp37A->type_vars=(struct Cyc_List_List*)_tmpDA,((_tmp37A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD1,((_tmp37A->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD2,((_tmp37A->encloser=(struct Cyc_Absyn_Stmt*)_tmpD3,((_tmp37A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD4,((_tmp37A->capability=(void*)_tmpD5,((_tmp37A->curr_rgn=(void*)_tmpD6,((_tmp37A->in_notreadctxt=(int)_tmpD7,((_tmp37A->in_lhs=(int)_tmpD8,((_tmp37A->in_new=(int)_tmpD9,((_tmp37A->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp37A)))))))))))))))))))))))));
# 589
return Cyc_Tcenv_put_fenv(r,te,_tmpDB);};};}
# 592
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDD=te->le;
if(_tmpDD == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 597
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpDD);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 602
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 604
struct _dyneither_ptr*_tmpDE=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpDF=(*vd->name).f1;_LL5C: if((_tmpDF.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 609
struct Cyc_Core_Impossible_exn_struct _tmp380;const char*_tmp37F;struct Cyc_Core_Impossible_exn_struct*_tmp37E;(int)_throw((void*)((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E[0]=((_tmp380.tag=Cyc_Core_Impossible,((_tmp380.f1=((_tmp37F="add_local_var: called with Rel_n",_tag_dyneither(_tmp37F,sizeof(char),33))),_tmp380)))),_tmp37E)))));}_LL5B:;}{
# 611
struct Cyc_Tcenv_SharedFenv*_tmpE5;struct Cyc_List_List*_tmpE6;struct Cyc_RgnOrder_RgnPO*_tmpE7;const struct Cyc_Tcenv_Bindings*_tmpE8;struct Cyc_Absyn_Stmt*_tmpE9;struct Cyc_Tcenv_CtrlEnv*_tmpEA;void*_tmpEB;void*_tmpEC;int _tmpED;int _tmpEE;int _tmpEF;const char*_tmp381;struct Cyc_Tcenv_Fenv*_tmpE4=
# 613
Cyc_Tcenv_get_fenv(te,((_tmp381="add_local_var",_tag_dyneither(_tmp381,sizeof(char),14))));_tmpE5=_tmpE4->shared;_tmpE6=_tmpE4->type_vars;_tmpE7=_tmpE4->region_order;_tmpE8=_tmpE4->locals;_tmpE9=_tmpE4->encloser;_tmpEA=_tmpE4->ctrl_env;_tmpEB=_tmpE4->capability;_tmpEC=_tmpE4->curr_rgn;_tmpED=_tmpE4->in_notreadctxt;_tmpEE=_tmpE4->in_lhs;_tmpEF=_tmpE4->in_new;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp384;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp383;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF0=(_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=((_tmp384.tag=4,((_tmp384.f1=vd,_tmp384)))),_tmp383)));
struct Cyc_Tcenv_Bindings*_tmp385;struct Cyc_Tcenv_Bindings*_tmpF1=(_tmp385=_region_malloc(r,sizeof(*_tmp385)),((_tmp385->v=_tmpDE,((_tmp385->b=(void*)_tmpF0,((_tmp385->tl=_tmpE8,_tmp385)))))));
struct Cyc_Tcenv_Fenv*_tmp386;struct Cyc_Tcenv_Fenv*_tmpF2=(_tmp386=_region_malloc(r,sizeof(*_tmp386)),((_tmp386->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE5,((_tmp386->type_vars=(struct Cyc_List_List*)_tmpE6,((_tmp386->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpE7,((_tmp386->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpF1),((_tmp386->encloser=(struct Cyc_Absyn_Stmt*)_tmpE9,((_tmp386->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpEA,((_tmp386->capability=(void*)_tmpEB,((_tmp386->curr_rgn=(void*)_tmpEC,((_tmp386->in_notreadctxt=(int)_tmpED,((_tmp386->in_lhs=(int)_tmpEE,((_tmp386->in_new=(int)_tmpEF,((_tmp386->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp386)))))))))))))))))))))))));
# 619
return Cyc_Tcenv_put_fenv(r,te,_tmpF2);};};}
# 622
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 624
struct Cyc_Tcenv_Fenv*_tmpF7=te->le;
if(_tmpF7 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_RgnOrder_RgnPO*_tmpFB;const struct Cyc_Tcenv_Bindings*_tmpFC;struct Cyc_Absyn_Stmt*_tmpFD;struct Cyc_Tcenv_CtrlEnv*_tmpFE;void*_tmpFF;void*_tmp100;int _tmp101;int _tmp102;struct Cyc_Tcenv_Fenv _tmpF8=*_tmpF7;_tmpF9=_tmpF8.shared;_tmpFA=_tmpF8.type_vars;_tmpFB=_tmpF8.region_order;_tmpFC=_tmpF8.locals;_tmpFD=_tmpF8.encloser;_tmpFE=_tmpF8.ctrl_env;_tmpFF=_tmpF8.capability;_tmp100=_tmpF8.curr_rgn;_tmp101=_tmpF8.in_notreadctxt;_tmp102=_tmpF8.in_lhs;{
# 628
struct Cyc_Tcenv_Fenv*_tmp387;struct Cyc_Tcenv_Fenv*_tmp103=(_tmp387=_region_malloc(r,sizeof(*_tmp387)),((_tmp387->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF9,((_tmp387->type_vars=(struct Cyc_List_List*)_tmpFA,((_tmp387->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFB,((_tmp387->locals=(const struct Cyc_Tcenv_Bindings*)_tmpFC,((_tmp387->encloser=(struct Cyc_Absyn_Stmt*)_tmpFD,((_tmp387->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpFE,((_tmp387->capability=(void*)_tmpFF,((_tmp387->curr_rgn=(void*)_tmp100,((_tmp387->in_notreadctxt=(int)_tmp101,((_tmp387->in_lhs=(int)_tmp102,((_tmp387->in_new=(int)((int)status),((_tmp387->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp387)))))))))))))))))))))))));
# 631
return Cyc_Tcenv_put_fenv(r,te,_tmp103);};};}
# 634
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp105=te->le;
if(_tmp105 == 0)return(enum Cyc_Tcenv_NewStatus)0;{
int _tmp107;struct Cyc_Tcenv_Fenv*_tmp106=_tmp105;_tmp107=_tmp106->in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp107;};}
# 641
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp108=te->le;
if(_tmp108 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp10A;struct Cyc_List_List*_tmp10B;struct Cyc_RgnOrder_RgnPO*_tmp10C;const struct Cyc_Tcenv_Bindings*_tmp10D;struct Cyc_Absyn_Stmt*_tmp10E;struct Cyc_Tcenv_CtrlEnv*_tmp10F;void*_tmp110;void*_tmp111;int _tmp112;int _tmp113;int _tmp114;struct Cyc_Tcenv_Fenv _tmp109=*_tmp108;_tmp10A=_tmp109.shared;_tmp10B=_tmp109.type_vars;_tmp10C=_tmp109.region_order;_tmp10D=_tmp109.locals;_tmp10E=_tmp109.encloser;_tmp10F=_tmp109.ctrl_env;_tmp110=_tmp109.capability;_tmp111=_tmp109.curr_rgn;_tmp112=_tmp109.in_notreadctxt;_tmp113=_tmp109.in_lhs;_tmp114=_tmp109.in_new;{
# 646
struct Cyc_Tcenv_Fenv*_tmp388;struct Cyc_Tcenv_Fenv*_tmp115=(_tmp388=_region_malloc(r,sizeof(*_tmp388)),((_tmp388->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10A,((_tmp388->type_vars=(struct Cyc_List_List*)_tmp10B,((_tmp388->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp10C,((_tmp388->locals=(const struct Cyc_Tcenv_Bindings*)_tmp10D,((_tmp388->encloser=(struct Cyc_Absyn_Stmt*)_tmp10E,((_tmp388->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10F,((_tmp388->capability=(void*)_tmp110,((_tmp388->curr_rgn=(void*)_tmp111,((_tmp388->in_notreadctxt=(int)1,((_tmp388->in_lhs=(int)_tmp113,((_tmp388->in_new=(int)_tmp114,((_tmp388->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp388)))))))))))))))))))))))));
# 648
return Cyc_Tcenv_put_fenv(r,te,_tmp115);};};}
# 651
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp117=te->le;
if(_tmp117 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_RgnOrder_RgnPO*_tmp11B;const struct Cyc_Tcenv_Bindings*_tmp11C;struct Cyc_Absyn_Stmt*_tmp11D;struct Cyc_Tcenv_CtrlEnv*_tmp11E;void*_tmp11F;void*_tmp120;int _tmp121;int _tmp122;int _tmp123;struct Cyc_Tcenv_Fenv _tmp118=*_tmp117;_tmp119=_tmp118.shared;_tmp11A=_tmp118.type_vars;_tmp11B=_tmp118.region_order;_tmp11C=_tmp118.locals;_tmp11D=_tmp118.encloser;_tmp11E=_tmp118.ctrl_env;_tmp11F=_tmp118.capability;_tmp120=_tmp118.curr_rgn;_tmp121=_tmp118.in_notreadctxt;_tmp122=_tmp118.in_lhs;_tmp123=_tmp118.in_new;{
# 656
struct Cyc_Tcenv_Fenv*_tmp389;struct Cyc_Tcenv_Fenv*_tmp124=(_tmp389=_region_malloc(r,sizeof(*_tmp389)),((_tmp389->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp119,((_tmp389->type_vars=(struct Cyc_List_List*)_tmp11A,((_tmp389->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp11B,((_tmp389->locals=(const struct Cyc_Tcenv_Bindings*)_tmp11C,((_tmp389->encloser=(struct Cyc_Absyn_Stmt*)_tmp11D,((_tmp389->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp11E,((_tmp389->capability=(void*)_tmp11F,((_tmp389->curr_rgn=(void*)_tmp120,((_tmp389->in_notreadctxt=(int)0,((_tmp389->in_lhs=(int)_tmp122,((_tmp389->in_new=(int)_tmp123,((_tmp389->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp389)))))))))))))))))))))))));
# 658
return Cyc_Tcenv_put_fenv(r,te,_tmp124);};};}
# 661
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp126=te->le;
if(_tmp126 == 0)return 0;{
int _tmp128;struct Cyc_Tcenv_Fenv*_tmp127=_tmp126;_tmp128=_tmp127->in_notreadctxt;
return _tmp128;};}
# 668
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp129=te->le;
if(_tmp129 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_RgnOrder_RgnPO*_tmp12D;const struct Cyc_Tcenv_Bindings*_tmp12E;struct Cyc_Absyn_Stmt*_tmp12F;struct Cyc_Tcenv_CtrlEnv*_tmp130;void*_tmp131;void*_tmp132;int _tmp133;int _tmp134;int _tmp135;struct Cyc_Tcenv_Fenv _tmp12A=*_tmp129;_tmp12B=_tmp12A.shared;_tmp12C=_tmp12A.type_vars;_tmp12D=_tmp12A.region_order;_tmp12E=_tmp12A.locals;_tmp12F=_tmp12A.encloser;_tmp130=_tmp12A.ctrl_env;_tmp131=_tmp12A.capability;_tmp132=_tmp12A.curr_rgn;_tmp133=_tmp12A.in_notreadctxt;_tmp134=_tmp12A.in_lhs;_tmp135=_tmp12A.in_new;{
# 673
struct Cyc_Tcenv_Fenv*_tmp38A;struct Cyc_Tcenv_Fenv*_tmp136=(_tmp38A=_region_malloc(r,sizeof(*_tmp38A)),((_tmp38A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12B,((_tmp38A->type_vars=(struct Cyc_List_List*)_tmp12C,((_tmp38A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12D,((_tmp38A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12E,((_tmp38A->encloser=(struct Cyc_Absyn_Stmt*)_tmp12F,((_tmp38A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp130,((_tmp38A->capability=(void*)_tmp131,((_tmp38A->curr_rgn=(void*)_tmp132,((_tmp38A->in_notreadctxt=(int)_tmp133,((_tmp38A->in_lhs=(int)1,((_tmp38A->in_new=(int)_tmp135,((_tmp38A->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp38A)))))))))))))))))))))))));
# 676
return Cyc_Tcenv_put_fenv(r,te,_tmp136);};};}
# 679
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp138=te->le;
if(_tmp138 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_RgnOrder_RgnPO*_tmp13C;const struct Cyc_Tcenv_Bindings*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13F;void*_tmp140;void*_tmp141;int _tmp142;int _tmp143;int _tmp144;struct Cyc_Tcenv_Fenv _tmp139=*_tmp138;_tmp13A=_tmp139.shared;_tmp13B=_tmp139.type_vars;_tmp13C=_tmp139.region_order;_tmp13D=_tmp139.locals;_tmp13E=_tmp139.encloser;_tmp13F=_tmp139.ctrl_env;_tmp140=_tmp139.capability;_tmp141=_tmp139.curr_rgn;_tmp142=_tmp139.in_notreadctxt;_tmp143=_tmp139.in_lhs;_tmp144=_tmp139.in_new;{
# 684
struct Cyc_Tcenv_Fenv*_tmp38B;struct Cyc_Tcenv_Fenv*_tmp145=(_tmp38B=_region_malloc(r,sizeof(*_tmp38B)),((_tmp38B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp13A,((_tmp38B->type_vars=(struct Cyc_List_List*)_tmp13B,((_tmp38B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13C,((_tmp38B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13D,((_tmp38B->encloser=(struct Cyc_Absyn_Stmt*)_tmp13E,((_tmp38B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13F,((_tmp38B->capability=(void*)_tmp140,((_tmp38B->curr_rgn=(void*)_tmp141,((_tmp38B->in_notreadctxt=(int)_tmp142,((_tmp38B->in_lhs=(int)0,((_tmp38B->in_new=(int)_tmp144,((_tmp38B->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp38B)))))))))))))))))))))))));
# 687
return Cyc_Tcenv_put_fenv(r,te,_tmp145);};};}
# 690
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp147=te->le;
if(_tmp147 == 0)return 0;{
int _tmp149;struct Cyc_Tcenv_Fenv*_tmp148=_tmp147;_tmp149=_tmp148->in_lhs;
return _tmp149;};}
# 698
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 700
struct _dyneither_ptr*_tmp14A=(*vd->name).f2;
struct Cyc_Tcenv_SharedFenv*_tmp14D;struct Cyc_List_List*_tmp14E;struct Cyc_RgnOrder_RgnPO*_tmp14F;const struct Cyc_Tcenv_Bindings*_tmp150;struct Cyc_Absyn_Stmt*_tmp151;struct Cyc_Tcenv_CtrlEnv*_tmp152;void*_tmp153;void*_tmp154;int _tmp155;int _tmp156;int _tmp157;const char*_tmp38C;struct Cyc_Tcenv_Fenv*_tmp14C=
# 703
Cyc_Tcenv_get_fenv(te,((_tmp38C="add_pat_var",_tag_dyneither(_tmp38C,sizeof(char),12))));_tmp14D=_tmp14C->shared;_tmp14E=_tmp14C->type_vars;_tmp14F=_tmp14C->region_order;_tmp150=_tmp14C->locals;_tmp151=_tmp14C->encloser;_tmp152=_tmp14C->ctrl_env;_tmp153=_tmp14C->capability;_tmp154=_tmp14C->curr_rgn;_tmp155=_tmp14C->in_notreadctxt;_tmp156=_tmp14C->in_lhs;_tmp157=_tmp14C->in_new;{
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp38F;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp38E;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp158=(_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E[0]=((_tmp38F.tag=5,((_tmp38F.f1=vd,_tmp38F)))),_tmp38E)));
struct Cyc_Tcenv_Bindings*_tmp390;struct Cyc_Tcenv_Bindings*_tmp159=(_tmp390=_region_malloc(r,sizeof(*_tmp390)),((_tmp390->v=_tmp14A,((_tmp390->b=(void*)_tmp158,((_tmp390->tl=_tmp150,_tmp390)))))));
struct Cyc_Tcenv_Fenv*_tmp391;struct Cyc_Tcenv_Fenv*_tmp15A=(_tmp391=_region_malloc(r,sizeof(*_tmp391)),((_tmp391->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp14D,((_tmp391->type_vars=(struct Cyc_List_List*)_tmp14E,((_tmp391->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14F,((_tmp391->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp159),((_tmp391->encloser=(struct Cyc_Absyn_Stmt*)_tmp151,((_tmp391->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp152,((_tmp391->capability=(void*)_tmp153,((_tmp391->curr_rgn=(void*)_tmp154,((_tmp391->in_notreadctxt=(int)_tmp155,((_tmp391->in_lhs=(int)_tmp156,((_tmp391->in_new=(int)_tmp157,((_tmp391->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp391)))))))))))))))))))))))));
# 709
return Cyc_Tcenv_put_fenv(r,te,_tmp15A);};}
# 712
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp15F=te->le;
union Cyc_Absyn_Nmspace _tmp161;struct _dyneither_ptr*_tmp162;struct _tuple0*_tmp160=q;_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;{
union Cyc_Absyn_Nmspace _tmp163=_tmp161;_LL61: if((_tmp163.Loc_n).tag != 4)goto _LL63;_LL62:
# 717
 if(_tmp15F == 0){struct Cyc_Dict_Absent_exn_struct _tmp394;struct Cyc_Dict_Absent_exn_struct*_tmp393;(int)_throw((void*)((_tmp393=_cycalloc_atomic(sizeof(*_tmp393)),((_tmp393[0]=((_tmp394.tag=Cyc_Dict_Absent,_tmp394)),_tmp393)))));}
goto _LL64;_LL63: if((_tmp163.Rel_n).tag != 1)goto _LL65;if((struct Cyc_List_List*)(_tmp163.Rel_n).val != 0)goto _LL65;if(!(_tmp15F != 0))goto _LL65;_LL64: {
# 720
const struct Cyc_Tcenv_Bindings*_tmp167;struct Cyc_Tcenv_Fenv*_tmp166=_tmp15F;_tmp167=_tmp166->locals;{
# 722
struct _handler_cons _tmp168;_push_handler(& _tmp168);{int _tmp16A=0;if(setjmp(_tmp168.handler))_tmp16A=1;if(!_tmp16A){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp397;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp396;void*_tmp16D=(void*)((_tmp396=_region_malloc(r,sizeof(*_tmp396)),((_tmp396[0]=((_tmp397.tag=0,((_tmp397.f1=Cyc_Tcenv_lookup_binding(_tmp167,_tmp162),_tmp397)))),_tmp396))));_npop_handler(0);return _tmp16D;};_pop_handler();}else{void*_tmp169=(void*)_exn_thrown;void*_tmp16F=_tmp169;void*_tmp171;_LL68: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp170=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp16F;if(_tmp170->tag != Cyc_Tcenv_NoBinding)goto _LL6A;}_LL69:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL6A: _tmp171=_tmp16F;_LL6B:(void)_throw(_tmp171);_LL67:;}};};}_LL65:;_LL66: {
# 726
struct _handler_cons _tmp172;_push_handler(& _tmp172);{int _tmp174=0;if(setjmp(_tmp172.handler))_tmp174=1;if(!_tmp174){{void*_tmp175=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp175;};_pop_handler();}else{void*_tmp173=(void*)_exn_thrown;void*_tmp177=_tmp173;void*_tmp179;_LL6D: {struct Cyc_Dict_Absent_exn_struct*_tmp178=(struct Cyc_Dict_Absent_exn_struct*)_tmp177;if(_tmp178->tag != Cyc_Dict_Absent)goto _LL6F;}_LL6E: {
# 730
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp39A;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp399;return(void*)((_tmp399=_region_malloc(r,sizeof(*_tmp399)),((_tmp399[0]=((_tmp39A.tag=0,((_tmp39A.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp39A)))),_tmp399))));}_LL6F: _tmp179=_tmp177;_LL70:(void)_throw(_tmp179);_LL6C:;}};}_LL60:;};}
# 735
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp17E;const char*_tmp39B;struct Cyc_Tcenv_Fenv*_tmp17D=
Cyc_Tcenv_get_fenv(te,((_tmp39B="process_continue",_tag_dyneither(_tmp39B,sizeof(char),17))));_tmp17E=_tmp17D->ctrl_env;{
void*_tmp17F=_tmp17E->continue_stmt;struct Cyc_Absyn_Stmt*_tmp181;_LL72: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp180=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp17F;if(_tmp180->tag != 3)goto _LL74;else{_tmp181=_tmp180->f1;}}_LL73:
# 740
{struct Cyc_List_List*_tmp39C;_tmp181->non_local_preds=((_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->hd=s,((_tmp39C->tl=_tmp181->non_local_preds,_tmp39C))))));}
*sopt=_tmp181;
return;_LL74: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp182=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp17F;if(_tmp182->tag != 0)goto _LL76;}_LL75:
{const char*_tmp39F;void*_tmp39E;(_tmp39E=0,Cyc_Tcutil_terr(s->loc,((_tmp39F="continue not in a loop",_tag_dyneither(_tmp39F,sizeof(char),23))),_tag_dyneither(_tmp39E,sizeof(void*),0)));}return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp183=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp17F;if(_tmp183->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp184=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp17F;if(_tmp184->tag != 2)goto _LL71;}_LL79: {
const char*_tmp3A0;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3A0="bad continue destination",_tag_dyneither(_tmp3A0,sizeof(char),25))));}_LL71:;};}
# 748
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp18B;struct Cyc_Tcenv_SharedFenv*_tmp18C;const char*_tmp3A1;struct Cyc_Tcenv_Fenv*_tmp18A=
Cyc_Tcenv_get_fenv(te,((_tmp3A1="process_break",_tag_dyneither(_tmp3A1,sizeof(char),14))));_tmp18B=_tmp18A->ctrl_env;_tmp18C=_tmp18A->shared;{
void*_tmp18D=_tmp18B->break_stmt;struct Cyc_Absyn_Stmt*_tmp18F;_LL7B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp18E=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp18D;if(_tmp18E->tag != 3)goto _LL7D;else{_tmp18F=_tmp18E->f1;}}_LL7C:
# 753
{struct Cyc_List_List*_tmp3A2;_tmp18F->non_local_preds=((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2->hd=s,((_tmp3A2->tl=_tmp18F->non_local_preds,_tmp3A2))))));}
*sopt=_tmp18F;
return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp190=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp18D;if(_tmp190->tag != 0)goto _LL7F;}_LL7E:
# 757
{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_Tcutil_terr(s->loc,((_tmp3A5="break not in a loop or switch",_tag_dyneither(_tmp3A5,sizeof(char),30))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp191=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp18D;if(_tmp191->tag != 2)goto _LL81;}_LL80:
# 763
{void*_tmp196=Cyc_Tcutil_compress(_tmp18C->return_typ);_LL84: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp196;if(_tmp197->tag != 0)goto _LL86;}_LL85:
 goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp196;if(_tmp198->tag != 7)goto _LL88;}_LL87:
 goto _LL89;_LL88: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp196;if(_tmp199->tag != 6)goto _LL8A;}_LL89:
# 767
{const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_warn(s->loc,((_tmp3A8="break may cause function not to return a value",_tag_dyneither(_tmp3A8,sizeof(char),47))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
goto _LL83;_LL8A:;_LL8B:
# 770
{const char*_tmp3AB;void*_tmp3AA;(_tmp3AA=0,Cyc_Tcutil_terr(s->loc,((_tmp3AB="break may cause function not to return a value",_tag_dyneither(_tmp3AB,sizeof(char),47))),_tag_dyneither(_tmp3AA,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 773
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp192=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp18D;if(_tmp192->tag != 1)goto _LL7A;}_LL82:
# 776
{const char*_tmp3AE;void*_tmp3AD;(_tmp3AD=0,Cyc_Tcutil_terr(s->loc,((_tmp3AE="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3AE,sizeof(char),56))),_tag_dyneither(_tmp3AD,sizeof(void*),0)));}
return;_LL7A:;};}
# 780
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_SharedFenv*_tmp1A2;const char*_tmp3AF;struct Cyc_Tcenv_Fenv*_tmp1A1=Cyc_Tcenv_get_fenv(te,((_tmp3AF="process_goto",_tag_dyneither(_tmp3AF,sizeof(char),13))));_tmp1A2=_tmp1A1->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1A2->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1A3=_tmp1A2->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1A3,l);
struct _RegionHandle*frgn=_tmp1A2->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3B0;slopt=((_tmp3B0=_region_malloc(frgn,sizeof(*_tmp3B0)),((_tmp3B0[0]=0,_tmp3B0))));}{
struct Cyc_List_List*_tmp3B1;struct Cyc_List_List*new_needed=(_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1->hd=s,((_tmp3B1->tl=*slopt,_tmp3B1)))));
_tmp1A2->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1A3,l,new_needed);};}else{
# 792
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3B2;s->non_local_preds=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->hd=s,((_tmp3B2->tl=s->non_local_preds,_tmp3B2))))));}
*sopt=s;}};}
# 798
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 800
struct Cyc_Tcenv_CtrlEnv*_tmp1A9;const char*_tmp3B3;struct Cyc_Tcenv_Fenv*_tmp1A8=Cyc_Tcenv_get_fenv(te,((_tmp3B3="process_fallthru",_tag_dyneither(_tmp3B3,sizeof(char),17))));_tmp1A9=_tmp1A8->ctrl_env;{
const struct _tuple10*_tmp1AA=_tmp1A9->fallthru_clause;
if(_tmp1AA != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3B4;(((*_tmp1AA).f1)->body)->non_local_preds=(
(_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4->hd=s,((_tmp3B4->tl=(((*_tmp1AA).f1)->body)->non_local_preds,_tmp3B4))))));}{
struct Cyc_Absyn_Switch_clause**_tmp3B5;*clauseopt=((_tmp3B5=_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5[0]=(*_tmp1AA).f1,_tmp3B5))));};}
# 807
return _tmp1AA;};}
# 810
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 814
struct Cyc_Tcenv_SharedFenv*_tmp1AF;struct Cyc_List_List*_tmp1B0;struct Cyc_RgnOrder_RgnPO*_tmp1B1;const struct Cyc_Tcenv_Bindings*_tmp1B2;struct Cyc_Absyn_Stmt*_tmp1B3;struct Cyc_Tcenv_CtrlEnv*_tmp1B4;void*_tmp1B5;void*_tmp1B6;int _tmp1B7;int _tmp1B8;int _tmp1B9;const char*_tmp3B6;struct Cyc_Tcenv_Fenv*_tmp1AE=
Cyc_Tcenv_get_fenv(te,((_tmp3B6="set_fallthru",_tag_dyneither(_tmp3B6,sizeof(char),13))));_tmp1AF=_tmp1AE->shared;_tmp1B0=_tmp1AE->type_vars;_tmp1B1=_tmp1AE->region_order;_tmp1B2=_tmp1AE->locals;_tmp1B3=_tmp1AE->encloser;_tmp1B4=_tmp1AE->ctrl_env;_tmp1B5=_tmp1AE->capability;_tmp1B6=_tmp1AE->curr_rgn;_tmp1B7=_tmp1AE->in_notreadctxt;_tmp1B8=_tmp1AE->in_lhs;_tmp1B9=_tmp1AE->in_new;{
struct _RegionHandle*_tmp1BB;void*_tmp1BC;void*_tmp1BD;const struct _tuple10*_tmp1BE;void*_tmp1BF;int _tmp1C0;struct Cyc_Tcenv_CtrlEnv*_tmp1BA=_tmp1B4;_tmp1BB=_tmp1BA->ctrl_rgn;_tmp1BC=_tmp1BA->continue_stmt;_tmp1BD=_tmp1BA->break_stmt;_tmp1BE=_tmp1BA->fallthru_clause;_tmp1BF=_tmp1BA->next_stmt;_tmp1C0=_tmp1BA->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3B7;ft_typ=((_tmp3B7=_region_malloc(_tmp1BB,sizeof(*_tmp3B7)),((_tmp3B7->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3B7->tl=ft_typ,_tmp3B7))))));}{
const struct Cyc_Tcenv_CList*_tmp1C2=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp3BA;struct Cyc_Tcenv_CtrlEnv*_tmp3B9;struct Cyc_Tcenv_CtrlEnv*_tmp1C3=
(_tmp3B9=_region_malloc(r,sizeof(*_tmp3B9)),((_tmp3B9->ctrl_rgn=_tmp1BB,((_tmp3B9->continue_stmt=_tmp1BC,((_tmp3B9->break_stmt=_tmp1BD,((_tmp3B9->fallthru_clause=(const struct _tuple10*)((_tmp3BA=_region_malloc(_tmp1BB,sizeof(*_tmp3BA)),((_tmp3BA->f1=clause,((_tmp3BA->f2=new_tvs,((_tmp3BA->f3=_tmp1C2,_tmp3BA)))))))),((_tmp3B9->next_stmt=_tmp1BF,((_tmp3B9->try_depth=_tmp1C0,_tmp3B9)))))))))))));
# 824
struct Cyc_Tcenv_Fenv*_tmp3BB;struct Cyc_Tcenv_Fenv*_tmp1C4=(_tmp3BB=_region_malloc(r,sizeof(*_tmp3BB)),((_tmp3BB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AF,((_tmp3BB->type_vars=(struct Cyc_List_List*)_tmp1B0,((_tmp3BB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1B1,((_tmp3BB->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1B2,((_tmp3BB->encloser=(struct Cyc_Absyn_Stmt*)_tmp1B3,((_tmp3BB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1C3,((_tmp3BB->capability=(void*)_tmp1B5,((_tmp3BB->curr_rgn=(void*)_tmp1B6,((_tmp3BB->in_notreadctxt=(int)_tmp1B7,((_tmp3BB->in_lhs=(int)_tmp1B8,((_tmp3BB->in_new=(int)_tmp1B9,((_tmp3BB->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3BB)))))))))))))))))))))))));
# 828
return Cyc_Tcenv_put_fenv(r,te,_tmp1C4);};};};}
# 831
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3BC;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3BC="clear_fallthru",_tag_dyneither(_tmp3BC,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1C9;struct Cyc_Tcenv_Fenv*_tmp1C8=fe;_tmp1C9=_tmp1C8->ctrl_env;
_tmp1C9->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 838
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 840
struct Cyc_Tcenv_SharedFenv*_tmp1CD;struct Cyc_List_List*_tmp1CE;struct Cyc_RgnOrder_RgnPO*_tmp1CF;const struct Cyc_Tcenv_Bindings*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1D1;struct Cyc_Tcenv_CtrlEnv*_tmp1D2;void*_tmp1D3;void*_tmp1D4;int _tmp1D5;int _tmp1D6;int _tmp1D7;const char*_tmp3BD;struct Cyc_Tcenv_Fenv*_tmp1CC=
# 842
Cyc_Tcenv_get_fenv(te,((_tmp3BD="set_in_loop",_tag_dyneither(_tmp3BD,sizeof(char),12))));_tmp1CD=_tmp1CC->shared;_tmp1CE=_tmp1CC->type_vars;_tmp1CF=_tmp1CC->region_order;_tmp1D0=_tmp1CC->locals;_tmp1D1=_tmp1CC->encloser;_tmp1D2=_tmp1CC->ctrl_env;_tmp1D3=_tmp1CC->capability;_tmp1D4=_tmp1CC->curr_rgn;_tmp1D5=_tmp1CC->in_notreadctxt;_tmp1D6=_tmp1CC->in_lhs;_tmp1D7=_tmp1CC->in_new;{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3C8;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3C7;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3C6;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3C5;struct Cyc_Tcenv_CtrlEnv*_tmp3C4;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3C4=_region_malloc(r,sizeof(*_tmp3C4)),((_tmp3C4->ctrl_rgn=r,((_tmp3C4->continue_stmt=(void*)(
(_tmp3C6=_region_malloc(r,sizeof(*_tmp3C6)),((_tmp3C6[0]=((_tmp3C5.tag=3,((_tmp3C5.f1=continue_dest,_tmp3C5)))),_tmp3C6)))),((_tmp3C4->break_stmt=_tmp1D2->next_stmt,((_tmp3C4->next_stmt=(void*)(
# 847
(_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8[0]=((_tmp3C7.tag=3,((_tmp3C7.f1=continue_dest,_tmp3C7)))),_tmp3C8)))),((_tmp3C4->fallthru_clause=_tmp1D2->fallthru_clause,((_tmp3C4->try_depth=_tmp1D2->try_depth,_tmp3C4)))))))))))));
# 850
struct Cyc_Tcenv_Fenv*_tmp3C9;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3C9=_region_malloc(r,sizeof(*_tmp3C9)),((_tmp3C9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1CD,((_tmp3C9->type_vars=(struct Cyc_List_List*)_tmp1CE,((_tmp3C9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1CF,((_tmp3C9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1D0,((_tmp3C9->encloser=(struct Cyc_Absyn_Stmt*)_tmp1D1,((_tmp3C9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3C9->capability=(void*)_tmp1D3,((_tmp3C9->curr_rgn=(void*)_tmp1D4,((_tmp3C9->in_notreadctxt=(int)_tmp1D5,((_tmp3C9->in_lhs=(int)_tmp1D6,((_tmp3C9->in_new=(int)_tmp1D7,((_tmp3C9->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3C9)))))))))))))))))))))))));
# 853
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 856
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3CA;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3CA="enter_try",_tag_dyneither(_tmp3CA,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1DF;struct Cyc_Tcenv_Fenv*_tmp1DE=fe;_tmp1DF=_tmp1DE->ctrl_env;
++ _tmp1DF->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 862
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_CtrlEnv*_tmp1E3;const char*_tmp3CB;struct Cyc_Tcenv_Fenv*_tmp1E2=Cyc_Tcenv_get_fenv(te,((_tmp3CB="get_try_depth",_tag_dyneither(_tmp3CB,sizeof(char),14))));_tmp1E3=_tmp1E2->ctrl_env;
return _tmp1E3->try_depth;}
# 869
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3CC;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3CC="set_in_switch",_tag_dyneither(_tmp3CC,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1E5;struct Cyc_Tcenv_Fenv*_tmp1E4=fe;_tmp1E5=_tmp1E4->ctrl_env;
_tmp1E5->break_stmt=_tmp1E5->next_stmt;
_tmp1E5->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 877
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 879
struct Cyc_Tcenv_SharedFenv*_tmp1E9;struct Cyc_List_List*_tmp1EA;struct Cyc_RgnOrder_RgnPO*_tmp1EB;const struct Cyc_Tcenv_Bindings*_tmp1EC;struct Cyc_Absyn_Stmt*_tmp1ED;struct Cyc_Tcenv_CtrlEnv*_tmp1EE;void*_tmp1EF;void*_tmp1F0;int _tmp1F1;int _tmp1F2;int _tmp1F3;const char*_tmp3CD;struct Cyc_Tcenv_Fenv _tmp1E8=*
# 881
Cyc_Tcenv_get_fenv(te,((_tmp3CD="set_next",_tag_dyneither(_tmp3CD,sizeof(char),9))));_tmp1E9=_tmp1E8.shared;_tmp1EA=_tmp1E8.type_vars;_tmp1EB=_tmp1E8.region_order;_tmp1EC=_tmp1E8.locals;_tmp1ED=_tmp1E8.encloser;_tmp1EE=_tmp1E8.ctrl_env;_tmp1EF=_tmp1E8.capability;_tmp1F0=_tmp1E8.curr_rgn;_tmp1F1=_tmp1E8.in_notreadctxt;_tmp1F2=_tmp1E8.in_lhs;_tmp1F3=_tmp1E8.in_new;{
struct Cyc_Tcenv_CtrlEnv*_tmp3CE;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3CE=_region_malloc(r,sizeof(*_tmp3CE)),((_tmp3CE->ctrl_rgn=r,((_tmp3CE->continue_stmt=_tmp1EE->continue_stmt,((_tmp3CE->break_stmt=_tmp1EE->break_stmt,((_tmp3CE->next_stmt=j,((_tmp3CE->fallthru_clause=_tmp1EE->fallthru_clause,((_tmp3CE->try_depth=_tmp1EE->try_depth,_tmp3CE)))))))))))));
# 890
struct Cyc_Tcenv_Fenv*_tmp3CF;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp3CF=_region_malloc(r,sizeof(*_tmp3CF)),((_tmp3CF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1E9,((_tmp3CF->type_vars=(struct Cyc_List_List*)_tmp1EA,((_tmp3CF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1EB,((_tmp3CF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1EC,((_tmp3CF->encloser=(struct Cyc_Absyn_Stmt*)_tmp1ED,((_tmp3CF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp3CF->capability=(void*)_tmp1EF,((_tmp3CF->curr_rgn=(void*)_tmp1F0,((_tmp3CF->in_notreadctxt=(int)_tmp1F1,((_tmp3CF->in_lhs=(int)_tmp1F2,((_tmp3CF->in_new=(int)_tmp1F3,((_tmp3CF->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3CF)))))))))))))))))))))))));
# 893
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 896
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_SharedFenv*_tmp1F8;const char*_tmp3D0;struct Cyc_Tcenv_Fenv*_tmp1F7=Cyc_Tcenv_get_fenv(te,((_tmp3D0="add_label",_tag_dyneither(_tmp3D0,sizeof(char),10))));_tmp1F8=_tmp1F7->shared;{
struct Cyc_Dict_Dict needed=_tmp1F8->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1F8->frgn;
if(sl_opt != 0){
_tmp1F8->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 904
struct Cyc_List_List*_tmp1F9=*sl_opt;
s->non_local_preds=_tmp1F9;
for(0;_tmp1F9 != 0;_tmp1F9=_tmp1F9->tl){
void*_tmp1FA=((struct Cyc_Absyn_Stmt*)_tmp1F9->hd)->r;struct Cyc_Absyn_Stmt**_tmp1FC;_LL8D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp1FB=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp1FA;if(_tmp1FB->tag != 8)goto _LL8F;else{_tmp1FC=(struct Cyc_Absyn_Stmt**)& _tmp1FB->f2;}}_LL8E:
*_tmp1FC=s;goto _LL8C;_LL8F:;_LL90:
{const char*_tmp3D1;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D1="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp3D1,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}
# 912
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp1F8->seen_labels,v)){
const char*_tmp3D5;void*_tmp3D4[1];struct Cyc_String_pa_PrintArg_struct _tmp3D3;(_tmp3D3.tag=0,((_tmp3D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D4[0]=& _tmp3D3,Cyc_Tcutil_terr(s->loc,((_tmp3D5="Repeated label: %s",_tag_dyneither(_tmp3D5,sizeof(char),19))),_tag_dyneither(_tmp3D4,sizeof(void*),1)))))));}
_tmp1F8->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1F8->seen_labels,v,s);
return te;};}
# 918
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp203;const char*_tmp3D6;struct Cyc_Tcenv_Fenv*_tmp202=Cyc_Tcenv_get_fenv(te,((_tmp3D6="all_labels_resolved",_tag_dyneither(_tmp3D6,sizeof(char),20))));_tmp203=_tmp202->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp203->needed_labels);}
# 923
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Stmt*_tmp206;const char*_tmp3D7;struct Cyc_Tcenv_Fenv*_tmp205=Cyc_Tcenv_get_fenv(te,((_tmp3D7="get_encloser",_tag_dyneither(_tmp3D7,sizeof(char),13))));_tmp206=_tmp205->encloser;
return _tmp206;}
# 928
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp3D8;return(_tmp3D8=_region_malloc(r2,sizeof(*_tmp3D8)),((_tmp3D8->ns=te->ns,((_tmp3D8->ae=te->ae,((_tmp3D8->le=0,((_tmp3D8->allow_valueof=1,_tmp3D8)))))))));}{
struct Cyc_Tcenv_SharedFenv*_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_RgnOrder_RgnPO*_tmp20B;const struct Cyc_Tcenv_Bindings*_tmp20C;struct Cyc_Absyn_Stmt*_tmp20D;struct Cyc_Tcenv_CtrlEnv*_tmp20E;void*_tmp20F;void*_tmp210;int _tmp211;int _tmp212;int _tmp213;struct Cyc_Tcenv_Fenv _tmp208=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp209=_tmp208.shared;_tmp20A=_tmp208.type_vars;_tmp20B=_tmp208.region_order;_tmp20C=_tmp208.locals;_tmp20D=_tmp208.encloser;_tmp20E=_tmp208.ctrl_env;_tmp20F=_tmp208.capability;_tmp210=_tmp208.curr_rgn;_tmp211=_tmp208.in_notreadctxt;_tmp212=_tmp208.in_lhs;_tmp213=_tmp208.in_new;{
struct Cyc_Tcenv_Fenv*_tmp3D9;struct Cyc_Tcenv_Fenv*_tmp214=(_tmp3D9=_region_malloc(r2,sizeof(*_tmp3D9)),((_tmp3D9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp209,((_tmp3D9->type_vars=(struct Cyc_List_List*)_tmp20A,((_tmp3D9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp20B,((_tmp3D9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20C,((_tmp3D9->encloser=(struct Cyc_Absyn_Stmt*)_tmp20D,((_tmp3D9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp20E,((_tmp3D9->capability=(void*)_tmp20F,((_tmp3D9->curr_rgn=(void*)_tmp210,((_tmp3D9->in_notreadctxt=(int)_tmp211,((_tmp3D9->in_lhs=(int)_tmp212,((_tmp3D9->in_new=(int)_tmp213,((_tmp3D9->fnrgn=(struct _RegionHandle*)r2,(struct Cyc_Tcenv_Fenv*)_tmp3D9)))))))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp3DA;return(_tmp3DA=_region_malloc(r2,sizeof(*_tmp3DA)),((_tmp3DA->ns=te->ns,((_tmp3DA->ae=te->ae,((_tmp3DA->le=_tmp214,((_tmp3DA->allow_valueof=1,_tmp3DA)))))))));};};}
# 936
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 938
struct Cyc_Tcenv_SharedFenv*_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_RgnOrder_RgnPO*_tmp21B;const struct Cyc_Tcenv_Bindings*_tmp21C;struct Cyc_Absyn_Stmt*_tmp21D;struct Cyc_Tcenv_CtrlEnv*_tmp21E;void*_tmp21F;void*_tmp220;int _tmp221;int _tmp222;int _tmp223;const char*_tmp3DB;struct Cyc_Tcenv_Fenv _tmp218=*
# 940
Cyc_Tcenv_get_fenv(te,((_tmp3DB="set_encloser",_tag_dyneither(_tmp3DB,sizeof(char),13))));_tmp219=_tmp218.shared;_tmp21A=_tmp218.type_vars;_tmp21B=_tmp218.region_order;_tmp21C=_tmp218.locals;_tmp21D=_tmp218.encloser;_tmp21E=_tmp218.ctrl_env;_tmp21F=_tmp218.capability;_tmp220=_tmp218.curr_rgn;_tmp221=_tmp218.in_notreadctxt;_tmp222=_tmp218.in_lhs;_tmp223=_tmp218.in_new;{
struct Cyc_Tcenv_Fenv*_tmp3DC;struct Cyc_Tcenv_Fenv*_tmp224=
(_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp219,((_tmp3DC->type_vars=(struct Cyc_List_List*)_tmp21A,((_tmp3DC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21B,((_tmp3DC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21C,((_tmp3DC->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp3DC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21E,((_tmp3DC->capability=(void*)_tmp21F,((_tmp3DC->curr_rgn=(void*)_tmp220,((_tmp3DC->in_notreadctxt=(int)_tmp221,((_tmp3DC->in_lhs=(int)_tmp222,((_tmp3DC->in_new=(int)_tmp223,((_tmp3DC->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3DC)))))))))))))))))))))))));
# 944
return Cyc_Tcenv_put_fenv(r,te,_tmp224);};}
# 947
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 950
struct Cyc_Absyn_Tvar*tv;
{void*_tmp226=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp228;_LL92: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp226;if(_tmp227->tag != 2)goto _LL94;else{_tmp228=_tmp227->f1;}}_LL93:
 tv=_tmp228;goto _LL91;_LL94:;_LL95:
{const char*_tmp3DD;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3DD="bad add region",_tag_dyneither(_tmp3DD,sizeof(char),15))));}goto _LL91;_LL91:;}{
# 955
struct Cyc_Tcenv_SharedFenv*_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_RgnOrder_RgnPO*_tmp22E;const struct Cyc_Tcenv_Bindings*_tmp22F;struct Cyc_Absyn_Stmt*_tmp230;struct Cyc_Tcenv_CtrlEnv*_tmp231;void*_tmp232;void*_tmp233;int _tmp234;int _tmp235;int _tmp236;const char*_tmp3DE;struct Cyc_Tcenv_Fenv _tmp22B=*
# 957
Cyc_Tcenv_get_fenv(te,((_tmp3DE="add_region",_tag_dyneither(_tmp3DE,sizeof(char),11))));_tmp22C=_tmp22B.shared;_tmp22D=_tmp22B.type_vars;_tmp22E=_tmp22B.region_order;_tmp22F=_tmp22B.locals;_tmp230=_tmp22B.encloser;_tmp231=_tmp22B.ctrl_env;_tmp232=_tmp22B.capability;_tmp233=_tmp22B.curr_rgn;_tmp234=_tmp22B.in_notreadctxt;_tmp235=_tmp22B.in_lhs;_tmp236=_tmp22B.in_new;
_tmp22E=Cyc_RgnOrder_add_youngest(_tmp22C->frgn,_tmp22E,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp3F1;struct Cyc_List_List*_tmp3F0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3EF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp3EE;struct Cyc_List_List*_tmp3ED;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3EC;_tmp232=(void*)((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3F1.tag=24,((_tmp3F1.f1=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=(void*)((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3EE.tag=23,((_tmp3EE.f1=rgn,_tmp3EE)))),_tmp3EF)))),((_tmp3ED->tl=(
(_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=_tmp232,((_tmp3F0->tl=0,_tmp3F0)))))),_tmp3ED)))))),_tmp3F1)))),_tmp3EC))));}{
struct Cyc_Tcenv_Fenv*_tmp3F2;struct Cyc_Tcenv_Fenv*_tmp23D=
(_tmp3F2=_region_malloc(r,sizeof(*_tmp3F2)),((_tmp3F2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp22C,((_tmp3F2->type_vars=(struct Cyc_List_List*)_tmp22D,((_tmp3F2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22E,((_tmp3F2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22F,((_tmp3F2->encloser=(struct Cyc_Absyn_Stmt*)_tmp230,((_tmp3F2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp231,((_tmp3F2->capability=(void*)_tmp232,((_tmp3F2->curr_rgn=(void*)_tmp233,((_tmp3F2->in_notreadctxt=(int)_tmp234,((_tmp3F2->in_lhs=(int)_tmp235,((_tmp3F2->in_new=(int)_tmp236,((_tmp3F2->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp3F2)))))))))))))))))))))))));
# 964
return Cyc_Tcenv_put_fenv(r,te,_tmp23D);};};}
# 967
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 969
struct Cyc_Tcenv_SharedFenv*_tmp241;struct Cyc_List_List*_tmp242;struct Cyc_RgnOrder_RgnPO*_tmp243;const struct Cyc_Tcenv_Bindings*_tmp244;struct Cyc_Absyn_Stmt*_tmp245;struct Cyc_Tcenv_CtrlEnv*_tmp246;void*_tmp247;void*_tmp248;int _tmp249;int _tmp24A;int _tmp24B;const char*_tmp3F3;struct Cyc_Tcenv_Fenv _tmp240=*
# 971
Cyc_Tcenv_get_fenv(te,((_tmp3F3="new_named_block",_tag_dyneither(_tmp3F3,sizeof(char),16))));_tmp241=_tmp240.shared;_tmp242=_tmp240.type_vars;_tmp243=_tmp240.region_order;_tmp244=_tmp240.locals;_tmp245=_tmp240.encloser;_tmp246=_tmp240.ctrl_env;_tmp247=_tmp240.capability;_tmp248=_tmp240.curr_rgn;_tmp249=_tmp240.in_notreadctxt;_tmp24A=_tmp240.in_lhs;_tmp24B=_tmp240.in_new;{
# 973
const char*_tmp3F4;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3F4="new_block",_tag_dyneither(_tmp3F4,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp3F7;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F6;void*block_typ=(void*)((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=((_tmp3F7.tag=2,((_tmp3F7.f1=block_rgn,_tmp3F7)))),_tmp3F6))));
{struct Cyc_List_List*_tmp3F8;_tmp242=((_tmp3F8=_cycalloc(sizeof(*_tmp3F8)),((_tmp3F8->hd=block_rgn,((_tmp3F8->tl=_tmp242,_tmp3F8))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp242);
_tmp243=Cyc_RgnOrder_add_youngest(_tmp241->frgn,_tmp243,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp40B;struct Cyc_List_List*_tmp40A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp409;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp408;struct Cyc_List_List*_tmp407;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp406;_tmp247=(void*)((_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406[0]=((_tmp40B.tag=24,((_tmp40B.f1=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->hd=(void*)((_tmp409=_cycalloc(sizeof(*_tmp409)),((_tmp409[0]=((_tmp408.tag=23,((_tmp408.f1=block_typ,_tmp408)))),_tmp409)))),((_tmp407->tl=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A->hd=_tmp247,((_tmp40A->tl=0,_tmp40A)))))),_tmp407)))))),_tmp40B)))),_tmp406))));}
_tmp248=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp40C;struct Cyc_Tcenv_Fenv*_tmp253=
(_tmp40C=_region_malloc(r,sizeof(*_tmp40C)),((_tmp40C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp241,((_tmp40C->type_vars=(struct Cyc_List_List*)_tmp242,((_tmp40C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp243,((_tmp40C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp244,((_tmp40C->encloser=(struct Cyc_Absyn_Stmt*)_tmp245,((_tmp40C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp246,((_tmp40C->capability=(void*)_tmp247,((_tmp40C->curr_rgn=(void*)_tmp248,((_tmp40C->in_notreadctxt=(int)_tmp249,((_tmp40C->in_lhs=(int)_tmp24A,((_tmp40C->in_new=(int)_tmp24B,((_tmp40C->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp40C)))))))))))))))))))))))));
# 983
return Cyc_Tcenv_put_fenv(r,te,_tmp253);};};}
# 986
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 988
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 990
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 996
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1000
struct Cyc_Tcenv_SharedFenv*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_RgnOrder_RgnPO*_tmp25D;const struct Cyc_Tcenv_Bindings*_tmp25E;struct Cyc_Absyn_Stmt*_tmp25F;struct Cyc_Tcenv_CtrlEnv*_tmp260;void*_tmp261;void*_tmp262;int _tmp263;int _tmp264;int _tmp265;const char*_tmp40D;struct Cyc_Tcenv_Fenv _tmp25A=*
# 1002
Cyc_Tcenv_get_fenv(te,((_tmp40D="new_outlives_constraints",_tag_dyneither(_tmp40D,sizeof(char),25))));_tmp25B=_tmp25A.shared;_tmp25C=_tmp25A.type_vars;_tmp25D=_tmp25A.region_order;_tmp25E=_tmp25A.locals;_tmp25F=_tmp25A.encloser;_tmp260=_tmp25A.ctrl_env;_tmp261=_tmp25A.capability;_tmp262=_tmp25A.curr_rgn;_tmp263=_tmp25A.in_notreadctxt;_tmp264=_tmp25A.in_lhs;_tmp265=_tmp25A.in_new;
for(0;cs != 0;cs=cs->tl){
_tmp25D=
Cyc_RgnOrder_add_outlives_constraint(_tmp25B->frgn,_tmp25D,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1008
struct Cyc_Tcenv_Fenv*_tmp40E;struct Cyc_Tcenv_Fenv*_tmp266=
(_tmp40E=_region_malloc(r,sizeof(*_tmp40E)),((_tmp40E->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25B,((_tmp40E->type_vars=(struct Cyc_List_List*)_tmp25C,((_tmp40E->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp25D,((_tmp40E->locals=(const struct Cyc_Tcenv_Bindings*)_tmp25E,((_tmp40E->encloser=(struct Cyc_Absyn_Stmt*)_tmp25F,((_tmp40E->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp260,((_tmp40E->capability=(void*)_tmp261,((_tmp40E->curr_rgn=(void*)_tmp262,((_tmp40E->in_notreadctxt=(int)_tmp263,((_tmp40E->in_lhs=(int)_tmp264,((_tmp40E->in_new=(int)_tmp265,((_tmp40E->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp40E)))))))))))))))))))))))));
# 1011
return Cyc_Tcenv_put_fenv(r,te,_tmp266);};}
# 1014
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1022
void*_tmp268=Cyc_Tcutil_compress(r1);
void*_tmp269=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp26A=Cyc_Tcutil_typ_kind(_tmp268);
struct Cyc_Absyn_Kind*_tmp26B=Cyc_Tcutil_typ_kind(_tmp269);
# 1028
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp26A,_tmp26B);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp26B,_tmp26A);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp415;void*_tmp414[4];struct Cyc_String_pa_PrintArg_struct _tmp413;struct Cyc_String_pa_PrintArg_struct _tmp412;struct Cyc_String_pa_PrintArg_struct _tmp411;struct Cyc_String_pa_PrintArg_struct _tmp410;(_tmp410.tag=0,((_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp26B)),((_tmp411.tag=0,((_tmp411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp26A)),((_tmp412.tag=0,((_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp269)),((_tmp413.tag=0,((_tmp413.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp268)),((_tmp414[0]=& _tmp413,((_tmp414[1]=& _tmp412,((_tmp414[2]=& _tmp411,((_tmp414[3]=& _tmp410,Cyc_Tcutil_terr(loc,((_tmp415="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp415,sizeof(char),82))),_tag_dyneither(_tmp414,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1035
if(r1_le_r2  && !r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp273;void*_tmp274;struct _tuple11 _tmp272=Cyc_Tcutil_swap_kind(_tmp269,Cyc_Tcutil_kind_to_bound(_tmp26A));_tmp273=_tmp272.f1;_tmp274=_tmp272.f2;{
struct _tuple11*_tmp416;*oldtv=((_tmp416=_region_malloc(r,sizeof(*_tmp416)),((_tmp416->f1=_tmp273,((_tmp416->f2=_tmp274,_tmp416))))));};}else{
# 1039
if(!r1_le_r2  && r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp277;void*_tmp278;struct _tuple11 _tmp276=Cyc_Tcutil_swap_kind(_tmp268,Cyc_Tcutil_kind_to_bound(_tmp26B));_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;{
struct _tuple11*_tmp417;*oldtv=((_tmp417=_region_malloc(r,sizeof(*_tmp417)),((_tmp417->f1=_tmp277,((_tmp417->f2=_tmp278,_tmp417))))));};}}}{
# 1045
struct _RegionHandle*_tmp27A=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp27B=0;
if((_tmp268 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp268 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp268 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp41A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp419;void*eff1=(void*)((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419[0]=((_tmp41A.tag=23,((_tmp41A.f1=_tmp268,_tmp41A)))),_tmp419))));
struct _tuple13*_tmp41D;struct Cyc_List_List*_tmp41C;_tmp27B=((_tmp41C=_region_malloc(_tmp27A,sizeof(*_tmp41C)),((_tmp41C->hd=((_tmp41D=_region_malloc(_tmp27A,sizeof(*_tmp41D)),((_tmp41D->f1=eff1,((_tmp41D->f2=_tmp269,_tmp41D)))))),((_tmp41C->tl=_tmp27B,_tmp41C))))));}
# 1051
if((_tmp269 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp269 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp269 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp420;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp41F;void*eff2=(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp420.tag=23,((_tmp420.f1=_tmp269,_tmp420)))),_tmp41F))));
struct _tuple13*_tmp423;struct Cyc_List_List*_tmp422;_tmp27B=((_tmp422=_region_malloc(_tmp27A,sizeof(*_tmp422)),((_tmp422->hd=((_tmp423=_region_malloc(_tmp27A,sizeof(*_tmp423)),((_tmp423->f1=eff2,((_tmp423->f2=_tmp268,_tmp423)))))),((_tmp422->tl=_tmp27B,_tmp422))))));}
# 1056
return Cyc_Tcenv_new_outlives_constraints(_tmp27A,te,_tmp27B,loc);};}
# 1059
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp284=te->le;
if(_tmp284 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp284;
void*_tmp286;struct Cyc_Tcenv_Fenv*_tmp285=fe;_tmp286=_tmp285->curr_rgn;
return _tmp286;};}
# 1070
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp424;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp424="check_rgn_accessible",_tag_dyneither(_tmp424,sizeof(char),21))));
void*_tmp288;struct Cyc_RgnOrder_RgnPO*_tmp289;struct Cyc_Tcenv_Fenv*_tmp287=fe;_tmp288=_tmp287->capability;_tmp289=_tmp287->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp288) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp288))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp427;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp426;if(Cyc_RgnOrder_eff_outlives_eff(_tmp289,(void*)((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((_tmp427.tag=23,((_tmp427.f1=rgn,_tmp427)))),_tmp426)))),_tmp288))
return;}{
const char*_tmp42B;void*_tmp42A[1];struct Cyc_String_pa_PrintArg_struct _tmp429;(_tmp429.tag=0,((_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp42A[0]=& _tmp429,Cyc_Tcutil_terr(loc,((_tmp42B="Expression accesses unavailable region %s",_tag_dyneither(_tmp42B,sizeof(char),42))),_tag_dyneither(_tmp42A,sizeof(void*),1)))))));};}
# 1082
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_RgnOrder_RgnPO*_tmp292;const char*_tmp42C;struct Cyc_Tcenv_Fenv*_tmp291=
Cyc_Tcenv_get_fenv(te,((_tmp42C="check_rgn_resetable",_tag_dyneither(_tmp42C,sizeof(char),20))));_tmp292=_tmp291->region_order;{
void*_tmp293=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp295;_LL97: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp294=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp293;if(_tmp294->tag != 2)goto _LL99;else{_tmp295=_tmp294->f1;}}_LL98:
# 1088
 if(!Cyc_RgnOrder_is_region_resetable(_tmp292,_tmp295)){
const char*_tmp430;void*_tmp42F[1];struct Cyc_String_pa_PrintArg_struct _tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp42F[0]=& _tmp42E,Cyc_Tcutil_terr(loc,((_tmp430="Region %s is not resetable",_tag_dyneither(_tmp430,sizeof(char),27))),_tag_dyneither(_tmp42F,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {
# 1092
const char*_tmp433;void*_tmp432;(_tmp432=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp433="check_rgn_resetable",_tag_dyneither(_tmp433,sizeof(char),20))),_tag_dyneither(_tmp432,sizeof(void*),0)));}_LL96:;};};}
# 1099
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp29B=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1104
if(_tmp29B == 0){
void*_tmp29C=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp29D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp29C;if(_tmp29D->tag != 22)goto _LL9E;}_LL9D:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp29E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp29C;if(_tmp29E->tag != 21)goto _LLA0;}_LL9F:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp29F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp29C;if(_tmp29F->tag != 20)goto _LLA2;}_LLA1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3:
 return 0;_LL9B:;}{
# 1112
struct Cyc_Tcenv_Fenv*fe=_tmp29B;
struct Cyc_RgnOrder_RgnPO*_tmp2A1;struct Cyc_Tcenv_Fenv*_tmp2A0=fe;_tmp2A1=_tmp2A0->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp436;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp435;int _tmp2A2=Cyc_RgnOrder_effect_outlives(_tmp2A1,(void*)((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=((_tmp436.tag=23,((_tmp436.f1=rt_a,_tmp436)))),_tmp435)))),rt_b);
# 1118
return _tmp2A2;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1123
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
void*_tmp2A7;struct Cyc_RgnOrder_RgnPO*_tmp2A8;struct Cyc_Tcenv_SharedFenv*_tmp2A9;const char*_tmp437;struct Cyc_Tcenv_Fenv*_tmp2A6=
Cyc_Tcenv_get_fenv(te,((_tmp437="check_effect_accessible",_tag_dyneither(_tmp437,sizeof(char),24))));_tmp2A7=_tmp2A6->capability;_tmp2A8=_tmp2A6->region_order;_tmp2A9=_tmp2A6->shared;
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2A7))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2A8,eff,_tmp2A7))
return;{
struct _RegionHandle*frgn=_tmp2A9->frgn;
struct _tuple14*_tmp43A;struct Cyc_List_List*_tmp439;_tmp2A9->delayed_effect_checks=(
(_tmp439=_region_malloc(frgn,sizeof(*_tmp439)),((_tmp439->hd=((_tmp43A=_region_malloc(frgn,sizeof(*_tmp43A)),((_tmp43A->f1=_tmp2A7,((_tmp43A->f2=eff,((_tmp43A->f3=_tmp2A8,((_tmp43A->f4=loc,_tmp43A)))))))))),((_tmp439->tl=_tmp2A9->delayed_effect_checks,_tmp439))))));};}
# 1135
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp2AE;const char*_tmp43B;struct Cyc_Tcenv_Fenv*_tmp2AD=
Cyc_Tcenv_get_fenv(te,((_tmp43B="check_delayed_effects",_tag_dyneither(_tmp43B,sizeof(char),22))));_tmp2AE=_tmp2AD->shared;{
struct Cyc_List_List*_tmp2AF=_tmp2AE->delayed_effect_checks;
for(0;_tmp2AF != 0;_tmp2AF=_tmp2AF->tl){
void*_tmp2B1;void*_tmp2B2;struct Cyc_RgnOrder_RgnPO*_tmp2B3;unsigned int _tmp2B4;struct _tuple14*_tmp2B0=(struct _tuple14*)_tmp2AF->hd;_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;_tmp2B3=_tmp2B0->f3;_tmp2B4=_tmp2B0->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2B2,_tmp2B1))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2B3,_tmp2B2,_tmp2B1))
continue;{
const char*_tmp440;void*_tmp43F[2];struct Cyc_String_pa_PrintArg_struct _tmp43E;struct Cyc_String_pa_PrintArg_struct _tmp43D;(_tmp43D.tag=0,((_tmp43D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2B2)),((_tmp43E.tag=0,((_tmp43E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2B1)),((_tmp43F[0]=& _tmp43E,((_tmp43F[1]=& _tmp43D,Cyc_Tcutil_terr(_tmp2B4,((_tmp440="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp440,sizeof(char),51))),_tag_dyneither(_tmp43F,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1154
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1156
struct Cyc_Tcenv_Fenv*_tmp2B9=te->le;
if(_tmp2B9 == 0){
# 1159
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp443;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp442;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((_tmp443.tag=23,((_tmp443.f1=(*((struct _tuple13*)po->hd)).f2,_tmp443)))),_tmp442)))),Cyc_Absyn_empty_effect)){
const char*_tmp446;void*_tmp445;(_tmp445=0,Cyc_Tcutil_terr(loc,((_tmp446="the required region ordering is not satisfied here",_tag_dyneither(_tmp446,sizeof(char),51))),_tag_dyneither(_tmp445,sizeof(void*),0)));}}
return;}{
# 1165
struct Cyc_RgnOrder_RgnPO*_tmp2BF;struct Cyc_Tcenv_SharedFenv*_tmp2C0;struct Cyc_Tcenv_Fenv*_tmp2BE=_tmp2B9;_tmp2BF=_tmp2BE->region_order;_tmp2C0=_tmp2BE->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2BF,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp449;struct Cyc_List_List*_tmp448;_tmp2C0->delayed_constraint_checks=(
(_tmp448=_region_malloc(_tmp2C0->frgn,sizeof(*_tmp448)),((_tmp448->hd=((_tmp449=_region_malloc(_tmp2C0->frgn,sizeof(*_tmp449)),((_tmp449->f1=_tmp2BF,((_tmp449->f2=po,((_tmp449->f3=loc,_tmp449)))))))),((_tmp448->tl=_tmp2C0->delayed_constraint_checks,_tmp448))))));}};}
# 1172
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp2C5;const char*_tmp44A;struct Cyc_Tcenv_Fenv*_tmp2C4=
Cyc_Tcenv_get_fenv(te,((_tmp44A="check_delayed_constraints",_tag_dyneither(_tmp44A,sizeof(char),26))));_tmp2C5=_tmp2C4->shared;{
struct Cyc_List_List*_tmp2C6=_tmp2C5->delayed_constraint_checks;
for(0;_tmp2C6 != 0;_tmp2C6=_tmp2C6->tl){
struct Cyc_RgnOrder_RgnPO*_tmp2C8;struct Cyc_List_List*_tmp2C9;unsigned int _tmp2CA;struct _tuple15*_tmp2C7=(struct _tuple15*)_tmp2C6->hd;_tmp2C8=_tmp2C7->f1;_tmp2C9=_tmp2C7->f2;_tmp2CA=_tmp2C7->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2C8,_tmp2C9,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp44D;void*_tmp44C;(_tmp44C=0,Cyc_Tcutil_terr(_tmp2CA,((_tmp44D="the required region ordering is not satisfied here",_tag_dyneither(_tmp44D,sizeof(char),51))),_tag_dyneither(_tmp44C,sizeof(void*),0)));}}};}
# 1183
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp45A;const char*_tmp459;void*_tmp458[1];struct Cyc_String_pa_PrintArg_struct _tmp457;struct Cyc_Absyn_Tvar*_tmp456;struct Cyc_Absyn_Tvar*rgn0=
(_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456->name=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A[0]=(struct _dyneither_ptr)((_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp458[0]=& _tmp457,Cyc_aprintf(((_tmp459="`%s",_tag_dyneither(_tmp459,sizeof(char),4))),_tag_dyneither(_tmp458,sizeof(void*),1)))))))),_tmp45A)))),((_tmp456->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp456->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp456)))))));
struct Cyc_List_List*_tmp45B;struct Cyc_List_List*_tmp2CD=(_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B->hd=rgn0,((_tmp45B->tl=fd->tvs,_tmp45B)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2CD);{
# 1191
struct Cyc_RgnOrder_RgnPO*_tmp2CE=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1193
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp45E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45D;void*param_rgn=(void*)((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45E.tag=2,((_tmp45E.f1=rgn0,_tmp45E)))),_tmp45D))));
struct Cyc_List_List*_tmp2CF=0;
{struct Cyc_List_List*_tmp2D0=fd->args;for(0;_tmp2D0 != 0;_tmp2D0=_tmp2D0->tl){
struct Cyc_Absyn_Vardecl _tmp464;struct _tuple0*_tmp463;struct Cyc_Absyn_Vardecl*_tmp462;struct Cyc_Absyn_Vardecl*_tmp2D1=(_tmp462=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp462[0]=((_tmp464.sc=Cyc_Absyn_Public,((_tmp464.name=(
(_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463->f1=Cyc_Absyn_Loc_n,((_tmp463->f2=(*((struct _tuple8*)_tmp2D0->hd)).f1,_tmp463)))))),((_tmp464.tq=(*((struct _tuple8*)_tmp2D0->hd)).f2,((_tmp464.type=(*((struct _tuple8*)_tmp2D0->hd)).f3,((_tmp464.initializer=0,((_tmp464.rgn=param_rgn,((_tmp464.attributes=0,((_tmp464.escapes=0,_tmp464)))))))))))))))),_tmp462)));
# 1203
{struct Cyc_List_List _tmp467;struct Cyc_List_List*_tmp466;_tmp2CF=((_tmp466=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp466[0]=((_tmp467.hd=_tmp2D1,((_tmp467.tl=_tmp2CF,_tmp467)))),_tmp466))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp46A;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp469;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2D4=(_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469[0]=((_tmp46A.tag=3,((_tmp46A.f1=_tmp2D1,_tmp46A)))),_tmp469)));
struct _dyneither_ptr*_tmp2D5=(*((struct _tuple8*)_tmp2D0->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp46B;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp46B=_region_malloc(r,sizeof(*_tmp46B)),((_tmp46B->v=_tmp2D5,((_tmp46B->b=(void*)_tmp2D4,((_tmp46B->tl=locals,_tmp46B))))))));};}}
# 1208
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp2DD;struct Cyc_Absyn_Tqual _tmp2DE;void*_tmp2DF;int _tmp2E0;struct Cyc_Absyn_VarargInfo _tmp2DC=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp2DD=_tmp2DC.name;_tmp2DE=_tmp2DC.tq;_tmp2DF=_tmp2DC.type;_tmp2E0=_tmp2DC.inject;
if(_tmp2DD != 0){
void*_tmp2E1=Cyc_Absyn_dyneither_typ(_tmp2DF,param_rgn,_tmp2DE,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp471;struct _tuple0*_tmp470;struct Cyc_Absyn_Vardecl*_tmp46F;struct Cyc_Absyn_Vardecl*_tmp2E2=(_tmp46F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp46F[0]=((_tmp471.sc=Cyc_Absyn_Public,((_tmp471.name=(
(_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470->f1=Cyc_Absyn_Loc_n,((_tmp470->f2=_tmp2DD,_tmp470)))))),((_tmp471.tq=
Cyc_Absyn_empty_tqual(0),((_tmp471.type=_tmp2E1,((_tmp471.initializer=0,((_tmp471.rgn=param_rgn,((_tmp471.attributes=0,((_tmp471.escapes=0,_tmp471)))))))))))))))),_tmp46F)));
# 1219
{struct Cyc_List_List*_tmp472;_tmp2CF=((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472->hd=_tmp2E2,((_tmp472->tl=_tmp2CF,_tmp472))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp475;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp474;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2E4=(_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[0]=((_tmp475.tag=3,((_tmp475.f1=_tmp2E2,_tmp475)))),_tmp474)));
struct _dyneither_ptr*_tmp2E5=_tmp2DD;
struct Cyc_Tcenv_Bindings*_tmp476;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp476=_region_malloc(r,sizeof(*_tmp476)),((_tmp476->v=_tmp2E5,((_tmp476->b=(void*)_tmp2E4,((_tmp476->tl=locals,_tmp476))))))));};}else{
# 1224
const char*_tmp479;void*_tmp478;(_tmp478=0,Cyc_Tcutil_terr(loc,((_tmp479="missing name for varargs",_tag_dyneither(_tmp479,sizeof(char),25))),_tag_dyneither(_tmp478,sizeof(void*),0)));}}
# 1226
{struct Cyc_Core_Opt _tmp47C;struct Cyc_Core_Opt*_tmp47B;fd->param_vardecls=((_tmp47B=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp47B[0]=((_tmp47C.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2CF),_tmp47C)),_tmp47B))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp498;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp497;struct Cyc_List_List*_tmp496;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp495;struct Cyc_Tcenv_CtrlEnv*_tmp494;struct Cyc_Tcenv_SharedFenv*_tmp493;struct Cyc_Tcenv_Fenv*_tmp492;return(_tmp492=_region_malloc(r,sizeof(*_tmp492)),((_tmp492->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp493=_region_malloc(r,sizeof(*_tmp493)),((_tmp493->frgn=r,((_tmp493->return_typ=fd->ret_type,((_tmp493->seen_labels=
# 1231
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp493->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp493->delayed_effect_checks=0,((_tmp493->delayed_constraint_checks=0,_tmp493)))))))))))))),((_tmp492->type_vars=(struct Cyc_List_List*)_tmp2CD,((_tmp492->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2CE,((_tmp492->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp492->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp492->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1240
(_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494->ctrl_rgn=r,((_tmp494->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp494->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp494->fallthru_clause=0,((_tmp494->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp494->try_depth=0,_tmp494)))))))))))))),((_tmp492->capability=(void*)((void*)(
(_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp495.tag=24,((_tmp495.f1=((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499->hd=(void*)((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497[0]=((_tmp498.tag=23,((_tmp498.f1=param_rgn,_tmp498)))),_tmp497)))),((_tmp499->tl=(
(_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=(void*)_check_null(fd->effect),((_tmp496->tl=0,_tmp496)))))),_tmp499)))))),_tmp495)))),_tmp49A))))),((_tmp492->curr_rgn=(void*)param_rgn,((_tmp492->in_notreadctxt=(int)0,((_tmp492->in_lhs=(int)0,((_tmp492->in_new=(int)0,((_tmp492->fnrgn=(struct _RegionHandle*)r,(struct Cyc_Tcenv_Fenv*)_tmp492)))))))))))))))))))))))));};};}
# 1251
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp302;struct Cyc_RgnOrder_RgnPO*_tmp303;struct Cyc_List_List*_tmp304;struct Cyc_Tcenv_SharedFenv*_tmp305;struct _RegionHandle*_tmp306;struct Cyc_Tcenv_Fenv*_tmp301=old_fenv;_tmp302=_tmp301->locals;_tmp303=_tmp301->region_order;_tmp304=_tmp301->type_vars;_tmp305=_tmp301->shared;_tmp306=_tmp301->fnrgn;{
# 1254
struct _RegionHandle*_tmp307=_tmp305->frgn;
const struct Cyc_Tcenv_Bindings*_tmp308=_tmp302;
struct _dyneither_ptr*_tmp4A7;const char*_tmp4A6;void*_tmp4A5[1];struct Cyc_String_pa_PrintArg_struct _tmp4A4;struct Cyc_Absyn_Tvar*_tmp4A3;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->name=((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7[0]=(struct _dyneither_ptr)((_tmp4A4.tag=0,((_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4A5[0]=& _tmp4A4,Cyc_aprintf(((_tmp4A6="`%s",_tag_dyneither(_tmp4A6,sizeof(char),4))),_tag_dyneither(_tmp4A5,sizeof(void*),1)))))))),_tmp4A7)))),((_tmp4A3->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4A3->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4A3)))))));
# 1260
{struct Cyc_List_List*_tmp309=fd->tvs;for(0;_tmp309 != 0;_tmp309=_tmp309->tl){
enum Cyc_Absyn_KindQual _tmp30B;enum Cyc_Absyn_AliasQual _tmp30C;struct Cyc_Absyn_Kind*_tmp30A=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp309->hd,& Cyc_Tcutil_bk);_tmp30B=_tmp30A->kind;_tmp30C=_tmp30A->aliasqual;
if(_tmp30B == Cyc_Absyn_RgnKind){
if(_tmp30C == Cyc_Absyn_Aliasable)
_tmp303=Cyc_RgnOrder_add_youngest(_tmp307,_tmp303,(struct Cyc_Absyn_Tvar*)_tmp309->hd,0,0);else{
# 1266
const char*_tmp4AA;void*_tmp4A9;(_tmp4A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AA="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4AA,sizeof(char),39))),_tag_dyneither(_tmp4A9,sizeof(void*),0)));}}}}
# 1268
_tmp303=Cyc_RgnOrder_add_youngest(_tmp307,_tmp303,rgn0,0,0);{
struct Cyc_List_List*_tmp4AB;struct Cyc_List_List*_tmp30F=(_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB->hd=rgn0,((_tmp4AB->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp304),_tmp4AB)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp30F);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4AE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4AD;void*param_rgn=(void*)((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=2,((_tmp4AE.f1=rgn0,_tmp4AE)))),_tmp4AD))));
struct Cyc_List_List*_tmp310=0;
{struct Cyc_List_List*_tmp311=fd->args;for(0;_tmp311 != 0;_tmp311=_tmp311->tl){
struct Cyc_Absyn_Vardecl _tmp4B4;struct _tuple0*_tmp4B3;struct Cyc_Absyn_Vardecl*_tmp4B2;struct Cyc_Absyn_Vardecl*_tmp312=(_tmp4B2=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4B2[0]=((_tmp4B4.sc=Cyc_Absyn_Public,((_tmp4B4.name=(
(_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->f1=Cyc_Absyn_Loc_n,((_tmp4B3->f2=(*((struct _tuple8*)_tmp311->hd)).f1,_tmp4B3)))))),((_tmp4B4.tq=(*((struct _tuple8*)_tmp311->hd)).f2,((_tmp4B4.type=(*((struct _tuple8*)_tmp311->hd)).f3,((_tmp4B4.initializer=0,((_tmp4B4.rgn=param_rgn,((_tmp4B4.attributes=0,((_tmp4B4.escapes=0,_tmp4B4)))))))))))))))),_tmp4B2)));
# 1281
{struct Cyc_List_List _tmp4B7;struct Cyc_List_List*_tmp4B6;_tmp310=((_tmp4B6=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4B6[0]=((_tmp4B7.hd=_tmp312,((_tmp4B7.tl=_tmp310,_tmp4B7)))),_tmp4B6))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4BA;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4B9;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp315=(_tmp4B9=_cycalloc(sizeof(*_tmp4B9)),((_tmp4B9[0]=((_tmp4BA.tag=3,((_tmp4BA.f1=_tmp312,_tmp4BA)))),_tmp4B9)));
struct _dyneither_ptr*_tmp316=(*((struct _tuple8*)_tmp311->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4BB;_tmp308=(const struct Cyc_Tcenv_Bindings*)((_tmp4BB=_region_malloc(_tmp307,sizeof(*_tmp4BB)),((_tmp4BB->v=_tmp316,((_tmp4BB->b=(void*)_tmp315,((_tmp4BB->tl=_tmp308,_tmp4BB))))))));};}}
# 1286
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp31E;struct Cyc_Absyn_Tqual _tmp31F;void*_tmp320;int _tmp321;struct Cyc_Absyn_VarargInfo _tmp31D=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp31E=_tmp31D.name;_tmp31F=_tmp31D.tq;_tmp320=_tmp31D.type;_tmp321=_tmp31D.inject;
if(_tmp31E != 0){
void*_tmp322=Cyc_Absyn_dyneither_typ(_tmp320,param_rgn,_tmp31F,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4C1;struct _tuple0*_tmp4C0;struct Cyc_Absyn_Vardecl*_tmp4BF;struct Cyc_Absyn_Vardecl*_tmp323=(_tmp4BF=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4BF[0]=((_tmp4C1.sc=Cyc_Absyn_Public,((_tmp4C1.name=(
(_tmp4C0=_cycalloc(sizeof(*_tmp4C0)),((_tmp4C0->f1=Cyc_Absyn_Loc_n,((_tmp4C0->f2=_tmp31E,_tmp4C0)))))),((_tmp4C1.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4C1.type=_tmp322,((_tmp4C1.initializer=0,((_tmp4C1.rgn=param_rgn,((_tmp4C1.attributes=0,((_tmp4C1.escapes=0,_tmp4C1)))))))))))))))),_tmp4BF)));
# 1297
{struct Cyc_List_List*_tmp4C2;_tmp310=((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2->hd=_tmp323,((_tmp4C2->tl=_tmp310,_tmp4C2))))));}{
struct _dyneither_ptr*_tmp325=_tmp31E;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4C5;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4C4;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp326=(_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=((_tmp4C5.tag=3,((_tmp4C5.f1=_tmp323,_tmp4C5)))),_tmp4C4)));
struct Cyc_Tcenv_Bindings*_tmp4C6;_tmp308=(const struct Cyc_Tcenv_Bindings*)((_tmp4C6=_region_malloc(_tmp307,sizeof(*_tmp4C6)),((_tmp4C6->v=_tmp325,((_tmp4C6->b=(void*)_tmp326,((_tmp4C6->tl=_tmp308,_tmp4C6))))))));};}else{
# 1302
const char*_tmp4C9;void*_tmp4C8;(_tmp4C8=0,Cyc_Tcutil_terr(loc,((_tmp4C9="missing name for varargs",_tag_dyneither(_tmp4C9,sizeof(char),25))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}}
# 1304
{struct Cyc_Core_Opt _tmp4CC;struct Cyc_Core_Opt*_tmp4CB;fd->param_vardecls=((_tmp4CB=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4CB[0]=((_tmp4CC.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp310),_tmp4CC)),_tmp4CB))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4EA;struct Cyc_List_List*_tmp4E9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4E8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4E7;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4E5;struct Cyc_Tcenv_CtrlEnv*_tmp4E4;struct Cyc_Tcenv_SharedFenv*_tmp4E3;struct Cyc_Tcenv_Fenv*_tmp4E2;return(_tmp4E2=_region_malloc(_tmp307,sizeof(*_tmp4E2)),((_tmp4E2->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4E3=_region_malloc(_tmp307,sizeof(*_tmp4E3)),((_tmp4E3->frgn=_tmp307,((_tmp4E3->return_typ=fd->ret_type,((_tmp4E3->seen_labels=
# 1309
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp307,Cyc_strptrcmp),((_tmp4E3->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp307,Cyc_strptrcmp),((_tmp4E3->delayed_effect_checks=0,((_tmp4E3->delayed_constraint_checks=0,_tmp4E3)))))))))))))),((_tmp4E2->type_vars=(struct Cyc_List_List*)_tmp30F,((_tmp4E2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp303,((_tmp4E2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp308,((_tmp4E2->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4E2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1318
(_tmp4E4=_region_malloc(_tmp307,sizeof(*_tmp4E4)),((_tmp4E4->ctrl_rgn=_tmp307,((_tmp4E4->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4E4->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4E4->fallthru_clause=0,((_tmp4E4->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4E4->try_depth=0,_tmp4E4)))))))))))))),((_tmp4E2->capability=(void*)((void*)(
(_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4E5.tag=24,((_tmp4E5.f1=((_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9->hd=(void*)((_tmp4E7=_cycalloc(sizeof(*_tmp4E7)),((_tmp4E7[0]=((_tmp4E8.tag=23,((_tmp4E8.f1=param_rgn,_tmp4E8)))),_tmp4E7)))),((_tmp4E9->tl=(
(_tmp4E6=_cycalloc(sizeof(*_tmp4E6)),((_tmp4E6->hd=(void*)_check_null(fd->effect),((_tmp4E6->tl=0,_tmp4E6)))))),_tmp4E9)))))),_tmp4E5)))),_tmp4EA))))),((_tmp4E2->curr_rgn=(void*)param_rgn,((_tmp4E2->in_notreadctxt=(int)0,((_tmp4E2->in_lhs=(int)0,((_tmp4E2->in_new=(int)0,((_tmp4E2->fnrgn=(struct _RegionHandle*)_tmp306,(struct Cyc_Tcenv_Fenv*)_tmp4E2)))))))))))))))))))))))));};};};};}
