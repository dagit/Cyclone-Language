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
struct Cyc_Tcenv_Genv*_tmp36F;return(_tmp36F=_region_malloc(r,sizeof(*_tmp36F)),((_tmp36F->grgn=r,((_tmp36F->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp36F->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp36F->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp36F->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp36F->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp36F->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp36F->availables=0,_tmp36F)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 252
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 254
{struct Cyc_Core_Opt*_tmp370;Cyc_Tcutil_empty_var_set=((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp370))));}{
# 256
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp371;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371[0]=Cyc_Absyn_exn_tud,_tmp371)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp377;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp376;struct _tuple12*_tmp375;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp375=_region_malloc(r,sizeof(*_tmp375)),((_tmp375->f1=(void*)(
(_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377[0]=((_tmp376.tag=2,((_tmp376.f1=Cyc_Absyn_exn_tud,((_tmp376.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp376)))))),_tmp377)))),((_tmp375->f2=1,_tmp375)))))));}}{
# 263
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp378;return(_tmp378=_region_malloc(r,sizeof(*_tmp378)),((_tmp378->ns=0,((_tmp378->ae=ae,((_tmp378->le=0,((_tmp378->allow_valueof=0,_tmp378)))))))));};};}
# 269
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 271
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 275
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp379;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp379="outer_namespace",_tag_dyneither(_tmp379,sizeof(char),16))));}
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
const char*_tmp37D;void*_tmp37C[1];struct Cyc_String_pa_PrintArg_struct _tmp37B;(_tmp37B.tag=0,((_tmp37B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp37C[0]=& _tmp37B,Cyc_Tcutil_terr(loc,((_tmp37D="%s is ambiguous",_tag_dyneither(_tmp37D,sizeof(char),16))),_tag_dyneither(_tmp37C,sizeof(void*),1)))))));};
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
struct Cyc_Dict_Absent_exn_struct _tmp380;struct Cyc_Dict_Absent_exn_struct*_tmp37F;(int)_throw((void*)((_tmp37F=_cycalloc_atomic(sizeof(*_tmp37F)),((_tmp37F[0]=((_tmp380.tag=Cyc_Dict_Absent,_tmp380)),_tmp37F)))));}
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
struct Cyc_List_List*_tmp383;struct Cyc_List_List*_tmp382;_tmp38=((_tmp382=_region_malloc(temp,sizeof(*_tmp382)),((_tmp382->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383->hd=n,((_tmp383->tl=ns,_tmp383))))))),((_tmp382->tl=_tmp38,_tmp382))))));}}}
# 376
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp386;struct Cyc_List_List*_tmp385;_tmp38=((_tmp385=_region_malloc(temp,sizeof(*_tmp385)),((_tmp385->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->hd=n,((_tmp386->tl=ns,_tmp386))))))),((_tmp385->tl=_tmp38,_tmp385))))));}
# 382
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp38A;void*_tmp389[1];struct Cyc_String_pa_PrintArg_struct _tmp388;(_tmp388.tag=0,((_tmp388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp389[0]=& _tmp388,Cyc_Tcutil_terr(loc,((_tmp38A="%s is ambiguous",_tag_dyneither(_tmp38A,sizeof(char),16))),_tag_dyneither(_tmp389,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 387
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp38D;struct Cyc_Dict_Absent_exn_struct*_tmp38C;(int)_throw((void*)((_tmp38C=_cycalloc_atomic(sizeof(*_tmp38C)),((_tmp38C[0]=((_tmp38D.tag=Cyc_Dict_Absent,_tmp38D)),_tmp38C)))));}
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
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp38E;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp38E=_region_malloc(r,sizeof(*_tmp38E)),((_tmp38E[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp38E)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_throw(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 451
{const char*_tmp391;void*_tmp390;(_tmp390=0,Cyc_Tcutil_terr(loc,((_tmp391="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp391,sizeof(char),33))),_tag_dyneither(_tmp390,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 458
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 462
{const char*_tmp394;void*_tmp393;(_tmp393=0,Cyc_Tcutil_terr(loc,((_tmp394="bad qualified name for @extensible datatype",_tag_dyneither(_tmp394,sizeof(char),44))),_tag_dyneither(_tmp393,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp397;struct Cyc_Dict_Absent_exn_struct*_tmp396;(int)_throw((void*)((_tmp396=_cycalloc_atomic(sizeof(*_tmp396)),((_tmp396[0]=((_tmp397.tag=Cyc_Dict_Absent,_tmp397)),_tmp396)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_throw(_tmp77);_LL40:;}};}{
# 465
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp398;return(_tmp398=_region_malloc(r,sizeof(*_tmp398)),((_tmp398[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp398)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 469
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp399;return(_tmp399=_region_malloc(r,sizeof(*_tmp399)),((_tmp399[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp399)));}_LL30:;};}
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
struct Cyc_Tcenv_Fenv*_stmttmp0=_tmp97;struct _RegionHandle*_tmp99;struct Cyc_Tcenv_Fenv*_tmp98=_stmttmp0;_tmp99=_tmp98->fnrgn;
return _tmp99;}
# 532
return Cyc_Tcenv_coerce_heap_region();}
# 535
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp39A;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp39A="put_fenv",_tag_dyneither(_tmp39A,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp39B;return(_tmp39B=_region_malloc(l,sizeof(*_tmp39B)),((_tmp39B->ns=te->ns,((_tmp39B->ae=te->ae,((_tmp39B->le=fe,((_tmp39B->allow_valueof=te->allow_valueof,_tmp39B)))))))));};}
# 540
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp39C;return(_tmp39C=_region_malloc(l,sizeof(*_tmp39C)),((_tmp39C->ns=te->ns,((_tmp39C->ae=te->ae,((_tmp39C->le=0,((_tmp39C->allow_valueof=te->allow_valueof,_tmp39C)))))))));}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_Fenv _stmttmp1=*f;struct Cyc_Tcenv_SharedFenv*_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_RgnOrder_RgnPO*_tmpA0;const struct Cyc_Tcenv_Bindings*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA4;void*_tmpA5;struct Cyc_Tcenv_FenvFlags _tmpA6;struct _RegionHandle*_tmpA7;struct Cyc_Tcenv_Fenv _tmp9D=_stmttmp1;_tmp9E=_tmp9D.shared;_tmp9F=_tmp9D.type_vars;_tmpA0=_tmp9D.region_order;_tmpA1=_tmp9D.locals;_tmpA2=_tmp9D.encloser;_tmpA3=_tmp9D.ctrl_env;_tmpA4=_tmp9D.capability;_tmpA5=_tmp9D.curr_rgn;_tmpA6=_tmp9D.flags;_tmpA7=_tmp9D.fnrgn;{
# 548
struct Cyc_Tcenv_Fenv*_tmp39D;return(_tmp39D=_region_malloc(l,sizeof(*_tmp39D)),((_tmp39D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9E,((_tmp39D->type_vars=(struct Cyc_List_List*)_tmp9F,((_tmp39D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA0,((_tmp39D->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA1,((_tmp39D->encloser=(struct Cyc_Absyn_Stmt*)_tmpA2,((_tmp39D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA3,((_tmp39D->capability=(void*)_tmpA4,((_tmp39D->curr_rgn=(void*)_tmpA5,((_tmp39D->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA6,((_tmp39D->fnrgn=(struct _RegionHandle*)l,_tmp39D)))))))))))))))))))));};}
# 552
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 554
struct Cyc_Tcenv_Fenv _stmttmp2=*f;struct Cyc_Tcenv_SharedFenv*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_RgnOrder_RgnPO*_tmpAC;const struct Cyc_Tcenv_Bindings*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Tcenv_CtrlEnv*_tmpAF;void*_tmpB0;void*_tmpB1;struct Cyc_Tcenv_FenvFlags _tmpB2;struct Cyc_Tcenv_Fenv _tmpA9=_stmttmp2;_tmpAA=_tmpA9.shared;_tmpAB=_tmpA9.type_vars;_tmpAC=_tmpA9.region_order;_tmpAD=_tmpA9.locals;_tmpAE=_tmpA9.encloser;_tmpAF=_tmpA9.ctrl_env;_tmpB0=_tmpA9.capability;_tmpB1=_tmpA9.curr_rgn;_tmpB2=_tmpA9.flags;{
# 557
struct Cyc_Tcenv_CtrlEnv _stmttmp3=*_tmpAF;struct _RegionHandle*_tmpB4;void*_tmpB5;void*_tmpB6;const struct _tuple10*_tmpB7;void*_tmpB8;int _tmpB9;struct Cyc_Tcenv_CtrlEnv _tmpB3=_stmttmp3;_tmpB4=_tmpB3.ctrl_rgn;_tmpB5=_tmpB3.continue_stmt;_tmpB6=_tmpB3.break_stmt;_tmpB7=_tmpB3.fallthru_clause;_tmpB8=_tmpB3.next_stmt;_tmpB9=_tmpB3.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp39E;struct Cyc_Tcenv_CtrlEnv*_tmpBA=
(_tmp39E=_region_malloc(l,sizeof(*_tmp39E)),((_tmp39E->ctrl_rgn=_tmpB4,((_tmp39E->continue_stmt=_tmpB5,((_tmp39E->break_stmt=_tmpB6,((_tmp39E->fallthru_clause=_tmpB7,((_tmp39E->next_stmt=_tmpB8,((_tmp39E->try_depth=_tmpB9,_tmp39E)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp39F;return(_tmp39F=_region_malloc(l,sizeof(*_tmp39F)),((_tmp39F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAA,((_tmp39F->type_vars=(struct Cyc_List_List*)_tmpAB,((_tmp39F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAC,((_tmp39F->locals=(const struct Cyc_Tcenv_Bindings*)_tmpAD,((_tmp39F->encloser=(struct Cyc_Absyn_Stmt*)_tmpAE,((_tmp39F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBA,((_tmp39F->capability=(void*)_tmpB0,((_tmp39F->curr_rgn=(void*)_tmpB1,((_tmp39F->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB2,((_tmp39F->fnrgn=(struct _RegionHandle*)l,_tmp39F)))))))))))))))))))));};};}
# 566
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3A0;struct Cyc_Tcenv_Fenv*_stmttmp4=Cyc_Tcenv_get_fenv(te,((_tmp3A0="return_typ",_tag_dyneither(_tmp3A0,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpBE;struct Cyc_Tcenv_Fenv*_tmpBD=_stmttmp4;_tmpBE=_tmpBD->shared;{
struct Cyc_Tcenv_SharedFenv _stmttmp5=*_tmpBE;void*_tmpC0;struct Cyc_Tcenv_SharedFenv _tmpBF=_stmttmp5;_tmpC0=_tmpBF.return_typ;
return _tmpC0;};}
# 572
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC2=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _stmttmp6=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC2));struct Cyc_List_List*_tmpC4;struct Cyc_Tcenv_Fenv _tmpC3=_stmttmp6;_tmpC4=_tmpC3.type_vars;
return _tmpC4;};}
# 579
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp3A1;return(_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3A1)));}
# 583
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 585
const char*_tmp3A2;struct Cyc_Tcenv_Fenv*_stmttmp7=
# 587
Cyc_Tcenv_get_fenv(te,((_tmp3A2="add_type_vars",_tag_dyneither(_tmp3A2,sizeof(char),14))));
# 585
struct Cyc_Tcenv_SharedFenv*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_RgnOrder_RgnPO*_tmpC9;const struct Cyc_Tcenv_Bindings*_tmpCA;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Tcenv_CtrlEnv*_tmpCC;void*_tmpCD;void*_tmpCE;struct Cyc_Tcenv_FenvFlags _tmpCF;struct Cyc_Tcenv_Fenv*_tmpC6=_stmttmp7;_tmpC7=_tmpC6->shared;_tmpC8=_tmpC6->type_vars;_tmpC9=_tmpC6->region_order;_tmpCA=_tmpC6->locals;_tmpCB=_tmpC6->encloser;_tmpCC=_tmpC6->ctrl_env;_tmpCD=_tmpC6->capability;_tmpCE=_tmpC6->curr_rgn;_tmpCF=_tmpC6->flags;
# 589
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC8);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD0);{
struct Cyc_Tcenv_Fenv*_tmp3A3;struct Cyc_Tcenv_Fenv*_tmpD1=(_tmp3A3=_region_malloc(r,sizeof(*_tmp3A3)),((_tmp3A3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC7,((_tmp3A3->type_vars=(struct Cyc_List_List*)_tmpD0,((_tmp3A3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC9,((_tmp3A3->locals=(const struct Cyc_Tcenv_Bindings*)_tmpCA,((_tmp3A3->encloser=(struct Cyc_Absyn_Stmt*)_tmpCB,((_tmp3A3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCC,((_tmp3A3->capability=(void*)_tmpCD,((_tmp3A3->curr_rgn=(void*)_tmpCE,((_tmp3A3->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCF,((_tmp3A3->fnrgn=(struct _RegionHandle*)r,_tmp3A3)))))))))))))))))))));
# 594
return Cyc_Tcenv_put_fenv(r,te,_tmpD1);};};}
# 597
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpD4=te->le;
if(_tmpD4 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 602
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpD4);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 609
struct _dyneither_ptr*_tmpD5=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _stmttmp8=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpD6=_stmttmp8;_LL5C: if((_tmpD6.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 614
struct Cyc_Core_Impossible_exn_struct _tmp3A9;const char*_tmp3A8;struct Cyc_Core_Impossible_exn_struct*_tmp3A7;(int)_throw((void*)((_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7[0]=((_tmp3A9.tag=Cyc_Core_Impossible,((_tmp3A9.f1=((_tmp3A8="add_local_var: called with Rel_n",_tag_dyneither(_tmp3A8,sizeof(char),33))),_tmp3A9)))),_tmp3A7)))));}_LL5B:;}
# 616
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpDA;_push_handler(& _tmpDA);{int _tmpDC=0;if(setjmp(_tmpDA.handler))_tmpDC=1;if(!_tmpDC){
{void*_stmttmp9=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpDD=_stmttmp9;_LL61: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpDE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL63;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpDE->f1);if(_tmpDF->tag != 4)goto _LL63;}}_LL62:
# 620
{const char*_tmp3AC;void*_tmp3AB;(_tmp3AB=0,Cyc_Tcutil_warn(loc,((_tmp3AC="declaration hides local",_tag_dyneither(_tmp3AC,sizeof(char),24))),_tag_dyneither(_tmp3AB,sizeof(void*),0)));}goto _LL60;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE0=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE0->tag != 0)goto _LL65;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpE0->f1);if(_tmpE1->tag != 1)goto _LL65;}}_LL64:
{const char*_tmp3AF;void*_tmp3AE;(_tmp3AE=0,Cyc_Tcutil_warn(loc,((_tmp3AF="declaration hides global",_tag_dyneither(_tmp3AF,sizeof(char),25))),_tag_dyneither(_tmp3AE,sizeof(void*),0)));}goto _LL60;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE2=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE2->tag != 0)goto _LL67;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpE2->f1);if(_tmpE3->tag != 5)goto _LL67;}}_LL66:
{const char*_tmp3B2;void*_tmp3B1;(_tmp3B1=0,Cyc_Tcutil_warn(loc,((_tmp3B2="declaration hides pattern variable",_tag_dyneither(_tmp3B2,sizeof(char),35))),_tag_dyneither(_tmp3B1,sizeof(void*),0)));}goto _LL60;_LL67: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE4->tag != 0)goto _LL69;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpE4->f1);if(_tmpE5->tag != 3)goto _LL69;}}_LL68:
{const char*_tmp3B5;void*_tmp3B4;(_tmp3B4=0,Cyc_Tcutil_warn(loc,((_tmp3B5="declaration hides parameter",_tag_dyneither(_tmp3B5,sizeof(char),28))),_tag_dyneither(_tmp3B4,sizeof(void*),0)));}goto _LL60;_LL69:;_LL6A:
 goto _LL60;_LL60:;}
# 618
;_pop_handler();}else{void*_tmpDB=(void*)_exn_thrown;void*_tmpEF=_tmpDB;void*_tmpF1;_LL6C: {struct Cyc_Dict_Absent_exn_struct*_tmpF0=(struct Cyc_Dict_Absent_exn_struct*)_tmpEF;if(_tmpF0->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D:
# 626
 goto _LL6B;_LL6E: _tmpF1=_tmpEF;_LL6F:(void)_throw(_tmpF1);_LL6B:;}};}{
const char*_tmp3B6;struct Cyc_Tcenv_Fenv*_stmttmpA=
# 629
Cyc_Tcenv_get_fenv(te,((_tmp3B6="add_local_var",_tag_dyneither(_tmp3B6,sizeof(char),14))));
# 627
struct Cyc_Tcenv_SharedFenv*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_RgnOrder_RgnPO*_tmpF5;const struct Cyc_Tcenv_Bindings*_tmpF6;struct Cyc_Absyn_Stmt*_tmpF7;struct Cyc_Tcenv_CtrlEnv*_tmpF8;void*_tmpF9;void*_tmpFA;struct Cyc_Tcenv_FenvFlags _tmpFB;struct Cyc_Tcenv_Fenv*_tmpF2=_stmttmpA;_tmpF3=_tmpF2->shared;_tmpF4=_tmpF2->type_vars;_tmpF5=_tmpF2->region_order;_tmpF6=_tmpF2->locals;_tmpF7=_tmpF2->encloser;_tmpF8=_tmpF2->ctrl_env;_tmpF9=_tmpF2->capability;_tmpFA=_tmpF2->curr_rgn;_tmpFB=_tmpF2->flags;{
# 630
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp3B9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3B8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFC=(_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=((_tmp3B9.tag=4,((_tmp3B9.f1=vd,_tmp3B9)))),_tmp3B8)));
struct Cyc_Tcenv_Bindings*_tmp3BA;struct Cyc_Tcenv_Bindings*_tmpFD=(_tmp3BA=_region_malloc(r,sizeof(*_tmp3BA)),((_tmp3BA->v=_tmpD5,((_tmp3BA->b=(void*)_tmpFC,((_tmp3BA->tl=_tmpF6,_tmp3BA)))))));
struct Cyc_Tcenv_Fenv*_tmp3BB;struct Cyc_Tcenv_Fenv*_tmpFE=(_tmp3BB=_region_malloc(r,sizeof(*_tmp3BB)),((_tmp3BB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF3,((_tmp3BB->type_vars=(struct Cyc_List_List*)_tmpF4,((_tmp3BB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF5,((_tmp3BB->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpFD),((_tmp3BB->encloser=(struct Cyc_Absyn_Stmt*)_tmpF7,((_tmp3BB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF8,((_tmp3BB->capability=(void*)_tmpF9,((_tmp3BB->curr_rgn=(void*)_tmpFA,((_tmp3BB->flags=(struct Cyc_Tcenv_FenvFlags)_tmpFB,((_tmp3BB->fnrgn=(struct _RegionHandle*)r,_tmp3BB)))))))))))))))))))));
# 634
return Cyc_Tcenv_put_fenv(r,te,_tmpFE);};};}
# 637
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 639
struct Cyc_Tcenv_Fenv*_tmp104=te->le;
if(_tmp104 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpB=*_tmp104;struct Cyc_Tcenv_SharedFenv*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_RgnOrder_RgnPO*_tmp108;const struct Cyc_Tcenv_Bindings*_tmp109;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Tcenv_CtrlEnv*_tmp10B;void*_tmp10C;void*_tmp10D;int _tmp10E;int _tmp10F;int _tmp110;struct Cyc_Tcenv_Fenv _tmp105=_stmttmpB;_tmp106=_tmp105.shared;_tmp107=_tmp105.type_vars;_tmp108=_tmp105.region_order;_tmp109=_tmp105.locals;_tmp10A=_tmp105.encloser;_tmp10B=_tmp105.ctrl_env;_tmp10C=_tmp105.capability;_tmp10D=_tmp105.curr_rgn;_tmp10E=(_tmp105.flags).in_notreadctxt;_tmp10F=(_tmp105.flags).in_lhs;_tmp110=(_tmp105.flags).abstract_ok;{
# 644
struct Cyc_Tcenv_Fenv*_tmp3BC;struct Cyc_Tcenv_Fenv*_tmp111=(_tmp3BC=_region_malloc(r,sizeof(*_tmp3BC)),((_tmp3BC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp106,((_tmp3BC->type_vars=(struct Cyc_List_List*)_tmp107,((_tmp3BC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp108,((_tmp3BC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp109,((_tmp3BC->encloser=(struct Cyc_Absyn_Stmt*)_tmp10A,((_tmp3BC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10B,((_tmp3BC->capability=(void*)_tmp10C,((_tmp3BC->curr_rgn=(void*)_tmp10D,((_tmp3BC->flags=(struct Cyc_Tcenv_FenvFlags)(
# 646
((_tmp3BC->flags).in_notreadctxt=_tmp10E,(((_tmp3BC->flags).in_lhs=_tmp10F,(((_tmp3BC->flags).in_new=(int)status,(((_tmp3BC->flags).abstract_ok=_tmp110,_tmp3BC->flags)))))))),((_tmp3BC->fnrgn=(struct _RegionHandle*)r,_tmp3BC)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp111);};};}
# 650
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp113=te->le;
if(_tmp113 == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_stmttmpC=_tmp113;int _tmp115;struct Cyc_Tcenv_Fenv*_tmp114=_stmttmpC;_tmp115=(_tmp114->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp115;};}
# 657
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp116=te->le;
if(_tmp116 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmpD=_tmp116;int _tmp118;struct Cyc_Tcenv_Fenv*_tmp117=_stmttmpD;_tmp118=(_tmp117->flags).abstract_ok;
return _tmp118;};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp119=te->le;
if(_tmp119 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpE=*_tmp119;struct Cyc_Tcenv_SharedFenv*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_RgnOrder_RgnPO*_tmp11D;const struct Cyc_Tcenv_Bindings*_tmp11E;struct Cyc_Absyn_Stmt*_tmp11F;struct Cyc_Tcenv_CtrlEnv*_tmp120;void*_tmp121;void*_tmp122;int _tmp123;int _tmp124;int _tmp125;struct Cyc_Tcenv_Fenv _tmp11A=_stmttmpE;_tmp11B=_tmp11A.shared;_tmp11C=_tmp11A.type_vars;_tmp11D=_tmp11A.region_order;_tmp11E=_tmp11A.locals;_tmp11F=_tmp11A.encloser;_tmp120=_tmp11A.ctrl_env;_tmp121=_tmp11A.capability;_tmp122=_tmp11A.curr_rgn;_tmp123=(_tmp11A.flags).in_notreadctxt;_tmp124=(_tmp11A.flags).in_lhs;_tmp125=(_tmp11A.flags).in_new;{
# 671
struct Cyc_Tcenv_Fenv*_tmp3BD;struct Cyc_Tcenv_Fenv*_tmp126=(_tmp3BD=_region_malloc(r,sizeof(*_tmp3BD)),((_tmp3BD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11B,((_tmp3BD->type_vars=(struct Cyc_List_List*)_tmp11C,((_tmp3BD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp11D,((_tmp3BD->locals=(const struct Cyc_Tcenv_Bindings*)_tmp11E,((_tmp3BD->encloser=(struct Cyc_Absyn_Stmt*)_tmp11F,((_tmp3BD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp120,((_tmp3BD->capability=(void*)_tmp121,((_tmp3BD->curr_rgn=(void*)_tmp122,((_tmp3BD->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp3BD->flags).in_notreadctxt=_tmp123,(((_tmp3BD->flags).in_lhs=_tmp124,(((_tmp3BD->flags).in_new=_tmp125,(((_tmp3BD->flags).abstract_ok=1,_tmp3BD->flags)))))))),((_tmp3BD->fnrgn=(struct _RegionHandle*)r,_tmp3BD)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp126);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 679
struct Cyc_Tcenv_Fenv*_tmp128=te->le;
if(_tmp128 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpF=*_tmp128;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_RgnOrder_RgnPO*_tmp12C;const struct Cyc_Tcenv_Bindings*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Tcenv_CtrlEnv*_tmp12F;void*_tmp130;void*_tmp131;int _tmp132;int _tmp133;int _tmp134;struct Cyc_Tcenv_Fenv _tmp129=_stmttmpF;_tmp12A=_tmp129.shared;_tmp12B=_tmp129.type_vars;_tmp12C=_tmp129.region_order;_tmp12D=_tmp129.locals;_tmp12E=_tmp129.encloser;_tmp12F=_tmp129.ctrl_env;_tmp130=_tmp129.capability;_tmp131=_tmp129.curr_rgn;_tmp132=(_tmp129.flags).in_notreadctxt;_tmp133=(_tmp129.flags).in_lhs;_tmp134=(_tmp129.flags).in_new;{
# 684
struct Cyc_Tcenv_Fenv*_tmp3BE;struct Cyc_Tcenv_Fenv*_tmp135=(_tmp3BE=_region_malloc(r,sizeof(*_tmp3BE)),((_tmp3BE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A,((_tmp3BE->type_vars=(struct Cyc_List_List*)_tmp12B,((_tmp3BE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12C,((_tmp3BE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12D,((_tmp3BE->encloser=(struct Cyc_Absyn_Stmt*)_tmp12E,((_tmp3BE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12F,((_tmp3BE->capability=(void*)_tmp130,((_tmp3BE->curr_rgn=(void*)_tmp131,((_tmp3BE->flags=(struct Cyc_Tcenv_FenvFlags)(
# 686
((_tmp3BE->flags).in_notreadctxt=_tmp132,(((_tmp3BE->flags).in_lhs=_tmp133,(((_tmp3BE->flags).in_new=_tmp134,(((_tmp3BE->flags).abstract_ok=0,_tmp3BE->flags)))))))),((_tmp3BE->fnrgn=(struct _RegionHandle*)r,_tmp3BE)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};}
# 690
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp137=te->le;
if(_tmp137 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp10=*_tmp137;struct Cyc_Tcenv_SharedFenv*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_RgnOrder_RgnPO*_tmp13B;const struct Cyc_Tcenv_Bindings*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13D;struct Cyc_Tcenv_CtrlEnv*_tmp13E;void*_tmp13F;void*_tmp140;int _tmp141;int _tmp142;int _tmp143;struct Cyc_Tcenv_Fenv _tmp138=_stmttmp10;_tmp139=_tmp138.shared;_tmp13A=_tmp138.type_vars;_tmp13B=_tmp138.region_order;_tmp13C=_tmp138.locals;_tmp13D=_tmp138.encloser;_tmp13E=_tmp138.ctrl_env;_tmp13F=_tmp138.capability;_tmp140=_tmp138.curr_rgn;_tmp141=(_tmp138.flags).in_lhs;_tmp142=(_tmp138.flags).in_new;_tmp143=(_tmp138.flags).abstract_ok;{
# 696
struct Cyc_Tcenv_Fenv*_tmp3BF;struct Cyc_Tcenv_Fenv*_tmp144=(_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF)),((_tmp3BF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp139,((_tmp3BF->type_vars=(struct Cyc_List_List*)_tmp13A,((_tmp3BF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13B,((_tmp3BF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13C,((_tmp3BF->encloser=(struct Cyc_Absyn_Stmt*)_tmp13D,((_tmp3BF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13E,((_tmp3BF->capability=(void*)_tmp13F,((_tmp3BF->curr_rgn=(void*)_tmp140,((_tmp3BF->flags=(struct Cyc_Tcenv_FenvFlags)(
# 698
((_tmp3BF->flags).in_notreadctxt=1,(((_tmp3BF->flags).in_lhs=_tmp141,(((_tmp3BF->flags).in_new=_tmp142,(((_tmp3BF->flags).abstract_ok=_tmp143,_tmp3BF->flags)))))))),((_tmp3BF->fnrgn=(struct _RegionHandle*)r,_tmp3BF)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp144);};};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp146=te->le;
if(_tmp146 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp11=*_tmp146;struct Cyc_Tcenv_SharedFenv*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_RgnOrder_RgnPO*_tmp14A;const struct Cyc_Tcenv_Bindings*_tmp14B;struct Cyc_Absyn_Stmt*_tmp14C;struct Cyc_Tcenv_CtrlEnv*_tmp14D;void*_tmp14E;void*_tmp14F;int _tmp150;int _tmp151;int _tmp152;struct Cyc_Tcenv_Fenv _tmp147=_stmttmp11;_tmp148=_tmp147.shared;_tmp149=_tmp147.type_vars;_tmp14A=_tmp147.region_order;_tmp14B=_tmp147.locals;_tmp14C=_tmp147.encloser;_tmp14D=_tmp147.ctrl_env;_tmp14E=_tmp147.capability;_tmp14F=_tmp147.curr_rgn;_tmp150=(_tmp147.flags).in_lhs;_tmp151=(_tmp147.flags).in_new;_tmp152=(_tmp147.flags).abstract_ok;{
# 708
struct Cyc_Tcenv_Fenv*_tmp3C0;struct Cyc_Tcenv_Fenv*_tmp153=(_tmp3C0=_region_malloc(r,sizeof(*_tmp3C0)),((_tmp3C0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp148,((_tmp3C0->type_vars=(struct Cyc_List_List*)_tmp149,((_tmp3C0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14A,((_tmp3C0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14B,((_tmp3C0->encloser=(struct Cyc_Absyn_Stmt*)_tmp14C,((_tmp3C0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14D,((_tmp3C0->capability=(void*)_tmp14E,((_tmp3C0->curr_rgn=(void*)_tmp14F,((_tmp3C0->flags=(struct Cyc_Tcenv_FenvFlags)(
# 710
((_tmp3C0->flags).in_notreadctxt=0,(((_tmp3C0->flags).in_lhs=_tmp150,(((_tmp3C0->flags).in_new=_tmp151,(((_tmp3C0->flags).abstract_ok=_tmp152,_tmp3C0->flags)))))))),((_tmp3C0->fnrgn=(struct _RegionHandle*)r,_tmp3C0)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp153);};};}
# 714
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp155=te->le;
if(_tmp155 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp12=_tmp155;int _tmp157;struct Cyc_Tcenv_Fenv*_tmp156=_stmttmp12;_tmp157=(_tmp156->flags).in_notreadctxt;
return _tmp157;};}
# 721
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp158=te->le;
if(_tmp158 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp13=*_tmp158;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;struct Cyc_Tcenv_Fenv _tmp159=_stmttmp13;_tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_notreadctxt;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;{
# 727
struct Cyc_Tcenv_Fenv*_tmp3C1;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp3C1=_region_malloc(r,sizeof(*_tmp3C1)),((_tmp3C1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp3C1->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp3C1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp3C1->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp3C1->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp3C1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp3C1->capability=(void*)_tmp160,((_tmp3C1->curr_rgn=(void*)_tmp161,((_tmp3C1->flags=(struct Cyc_Tcenv_FenvFlags)(
# 729
((_tmp3C1->flags).in_notreadctxt=_tmp162,(((_tmp3C1->flags).in_lhs=1,(((_tmp3C1->flags).in_new=_tmp163,(((_tmp3C1->flags).abstract_ok=_tmp164,_tmp3C1->flags)))))))),((_tmp3C1->fnrgn=(struct _RegionHandle*)r,_tmp3C1)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 733
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp14=*_tmp167;struct Cyc_Tcenv_SharedFenv*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_RgnOrder_RgnPO*_tmp16B;const struct Cyc_Tcenv_Bindings*_tmp16C;struct Cyc_Absyn_Stmt*_tmp16D;struct Cyc_Tcenv_CtrlEnv*_tmp16E;void*_tmp16F;void*_tmp170;int _tmp171;int _tmp172;int _tmp173;struct Cyc_Tcenv_Fenv _tmp168=_stmttmp14;_tmp169=_tmp168.shared;_tmp16A=_tmp168.type_vars;_tmp16B=_tmp168.region_order;_tmp16C=_tmp168.locals;_tmp16D=_tmp168.encloser;_tmp16E=_tmp168.ctrl_env;_tmp16F=_tmp168.capability;_tmp170=_tmp168.curr_rgn;_tmp171=(_tmp168.flags).in_notreadctxt;_tmp172=(_tmp168.flags).in_new;_tmp173=(_tmp168.flags).abstract_ok;{
# 739
struct Cyc_Tcenv_Fenv*_tmp3C2;struct Cyc_Tcenv_Fenv*_tmp174=(_tmp3C2=_region_malloc(r,sizeof(*_tmp3C2)),((_tmp3C2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp169,((_tmp3C2->type_vars=(struct Cyc_List_List*)_tmp16A,((_tmp3C2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16B,((_tmp3C2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp16C,((_tmp3C2->encloser=(struct Cyc_Absyn_Stmt*)_tmp16D,((_tmp3C2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16E,((_tmp3C2->capability=(void*)_tmp16F,((_tmp3C2->curr_rgn=(void*)_tmp170,((_tmp3C2->flags=(struct Cyc_Tcenv_FenvFlags)(
# 741
((_tmp3C2->flags).in_notreadctxt=_tmp171,(((_tmp3C2->flags).in_lhs=0,(((_tmp3C2->flags).in_new=_tmp172,(((_tmp3C2->flags).abstract_ok=_tmp173,_tmp3C2->flags)))))))),((_tmp3C2->fnrgn=(struct _RegionHandle*)r,_tmp3C2)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp174);};};}
# 745
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp176=te->le;
if(_tmp176 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_stmttmp15=_tmp176;int _tmp178;struct Cyc_Tcenv_Fenv*_tmp177=_stmttmp15;_tmp178=(_tmp177->flags).in_lhs;
return _tmp178;};}
# 753
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 755
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp179;_push_handler(& _tmp179);{int _tmp17B=0;if(setjmp(_tmp179.handler))_tmp17B=1;if(!_tmp17B){
{void*_stmttmp16=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp17C=_stmttmp16;_LL71: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp17D=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp17D->tag != 0)goto _LL73;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17D->f1);if(_tmp17E->tag != 4)goto _LL73;}}_LL72:
# 759
{const char*_tmp3C5;void*_tmp3C4;(_tmp3C4=0,Cyc_Tcutil_warn(loc,((_tmp3C5="pattern variable hides local",_tag_dyneither(_tmp3C5,sizeof(char),29))),_tag_dyneither(_tmp3C4,sizeof(void*),0)));}goto _LL70;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp17F=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp17F->tag != 0)goto _LL75;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp180=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp17F->f1);if(_tmp180->tag != 1)goto _LL75;}}_LL74:
{const char*_tmp3C8;void*_tmp3C7;(_tmp3C7=0,Cyc_Tcutil_warn(loc,((_tmp3C8="pattern variable hides global",_tag_dyneither(_tmp3C8,sizeof(char),30))),_tag_dyneither(_tmp3C7,sizeof(void*),0)));}goto _LL70;_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp181=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp181->tag != 0)goto _LL77;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp182=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp181->f1);if(_tmp182->tag != 5)goto _LL77;}}_LL76:
{const char*_tmp3CB;void*_tmp3CA;(_tmp3CA=0,Cyc_Tcutil_warn(loc,((_tmp3CB="pattern variable hides pattern variable",_tag_dyneither(_tmp3CB,sizeof(char),40))),_tag_dyneither(_tmp3CA,sizeof(void*),0)));}goto _LL70;_LL77: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp183=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp183->tag != 0)goto _LL79;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp184=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp183->f1);if(_tmp184->tag != 3)goto _LL79;}}_LL78:
{const char*_tmp3CE;void*_tmp3CD;(_tmp3CD=0,Cyc_Tcutil_warn(loc,((_tmp3CE="pattern variable hides parameter",_tag_dyneither(_tmp3CE,sizeof(char),33))),_tag_dyneither(_tmp3CD,sizeof(void*),0)));}goto _LL70;_LL79:;_LL7A:
 goto _LL70;_LL70:;}
# 757
;_pop_handler();}else{void*_tmp17A=(void*)_exn_thrown;void*_tmp18E=_tmp17A;void*_tmp190;_LL7C: {struct Cyc_Dict_Absent_exn_struct*_tmp18F=(struct Cyc_Dict_Absent_exn_struct*)_tmp18E;if(_tmp18F->tag != Cyc_Dict_Absent)goto _LL7E;}_LL7D:
# 765
 goto _LL7B;_LL7E: _tmp190=_tmp18E;_LL7F:(void)_throw(_tmp190);_LL7B:;}};}{
struct _dyneither_ptr*_tmp191=(*vd->name).f2;
const char*_tmp3CF;struct Cyc_Tcenv_Fenv*_stmttmp17=
# 769
Cyc_Tcenv_get_fenv(te,((_tmp3CF="add_pat_var",_tag_dyneither(_tmp3CF,sizeof(char),12))));
# 767
struct Cyc_Tcenv_SharedFenv*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_RgnOrder_RgnPO*_tmp195;const struct Cyc_Tcenv_Bindings*_tmp196;struct Cyc_Absyn_Stmt*_tmp197;struct Cyc_Tcenv_CtrlEnv*_tmp198;void*_tmp199;void*_tmp19A;struct Cyc_Tcenv_FenvFlags _tmp19B;struct Cyc_Tcenv_Fenv*_tmp192=_stmttmp17;_tmp193=_tmp192->shared;_tmp194=_tmp192->type_vars;_tmp195=_tmp192->region_order;_tmp196=_tmp192->locals;_tmp197=_tmp192->encloser;_tmp198=_tmp192->ctrl_env;_tmp199=_tmp192->capability;_tmp19A=_tmp192->curr_rgn;_tmp19B=_tmp192->flags;{
# 770
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp3D2;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3D1;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19C=(_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=((_tmp3D2.tag=5,((_tmp3D2.f1=vd,_tmp3D2)))),_tmp3D1)));
struct Cyc_Tcenv_Bindings*_tmp3D3;struct Cyc_Tcenv_Bindings*_tmp19D=(_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3->v=_tmp191,((_tmp3D3->b=(void*)_tmp19C,((_tmp3D3->tl=_tmp196,_tmp3D3)))))));
struct Cyc_Tcenv_Fenv*_tmp3D4;struct Cyc_Tcenv_Fenv*_tmp19E=(_tmp3D4=_region_malloc(r,sizeof(*_tmp3D4)),((_tmp3D4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp193,((_tmp3D4->type_vars=(struct Cyc_List_List*)_tmp194,((_tmp3D4->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp195,((_tmp3D4->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp19D),((_tmp3D4->encloser=(struct Cyc_Absyn_Stmt*)_tmp197,((_tmp3D4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp198,((_tmp3D4->capability=(void*)_tmp199,((_tmp3D4->curr_rgn=(void*)_tmp19A,((_tmp3D4->flags=(struct Cyc_Tcenv_FenvFlags)_tmp19B,((_tmp3D4->fnrgn=(struct _RegionHandle*)r,_tmp3D4)))))))))))))))))))));
# 774
return Cyc_Tcenv_put_fenv(r,te,_tmp19E);};};}
# 777
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp1A4=te->le;
union Cyc_Absyn_Nmspace _tmp1A6;struct _dyneither_ptr*_tmp1A7;struct _tuple0*_tmp1A5=q;_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;{
union Cyc_Absyn_Nmspace _tmp1A8=_tmp1A6;_LL81: if((_tmp1A8.Loc_n).tag != 4)goto _LL83;_LL82:
# 782
 if(_tmp1A4 == 0){struct Cyc_Dict_Absent_exn_struct _tmp3D7;struct Cyc_Dict_Absent_exn_struct*_tmp3D6;(int)_throw((void*)((_tmp3D6=_cycalloc_atomic(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3D7.tag=Cyc_Dict_Absent,_tmp3D7)),_tmp3D6)))));}
goto _LL84;_LL83: if((_tmp1A8.Rel_n).tag != 1)goto _LL85;if((struct Cyc_List_List*)(_tmp1A8.Rel_n).val != 0)goto _LL85;if(!(_tmp1A4 != 0))goto _LL85;_LL84: {
# 785
struct Cyc_Tcenv_Fenv*_stmttmp18=_tmp1A4;const struct Cyc_Tcenv_Bindings*_tmp1AC;struct Cyc_Tcenv_Fenv*_tmp1AB=_stmttmp18;_tmp1AC=_tmp1AB->locals;{
# 787
struct _handler_cons _tmp1AD;_push_handler(& _tmp1AD);{int _tmp1AF=0;if(setjmp(_tmp1AD.handler))_tmp1AF=1;if(!_tmp1AF){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3DA;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3D9;void*_tmp1B2=(void*)((_tmp3D9=_region_malloc(r,sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DA.tag=0,((_tmp3DA.f1=Cyc_Tcenv_lookup_binding(_tmp1AC,_tmp1A7),_tmp3DA)))),_tmp3D9))));_npop_handler(0);return _tmp1B2;};_pop_handler();}else{void*_tmp1AE=(void*)_exn_thrown;void*_tmp1B4=_tmp1AE;void*_tmp1B6;_LL88: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp1B5=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1B4;if(_tmp1B5->tag != Cyc_Tcenv_NoBinding)goto _LL8A;}_LL89:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL8A: _tmp1B6=_tmp1B4;_LL8B:(void)_throw(_tmp1B6);_LL87:;}};};}_LL85:;_LL86: {
# 791
struct _handler_cons _tmp1B7;_push_handler(& _tmp1B7);{int _tmp1B9=0;if(setjmp(_tmp1B7.handler))_tmp1B9=1;if(!_tmp1B9){{void*_tmp1BA=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1BA;};_pop_handler();}else{void*_tmp1B8=(void*)_exn_thrown;void*_tmp1BC=_tmp1B8;void*_tmp1BE;_LL8D: {struct Cyc_Dict_Absent_exn_struct*_tmp1BD=(struct Cyc_Dict_Absent_exn_struct*)_tmp1BC;if(_tmp1BD->tag != Cyc_Dict_Absent)goto _LL8F;}_LL8E: {
# 795
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3DD;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3DC;return(void*)((_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC[0]=((_tmp3DD.tag=0,((_tmp3DD.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp3DD)))),_tmp3DC))));}_LL8F: _tmp1BE=_tmp1BC;_LL90:(void)_throw(_tmp1BE);_LL8C:;}};}_LL80:;};}
# 800
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3DE;struct Cyc_Tcenv_Fenv*_stmttmp19=
Cyc_Tcenv_get_fenv(te,((_tmp3DE="process_continue",_tag_dyneither(_tmp3DE,sizeof(char),17))));
# 801
struct Cyc_Tcenv_CtrlEnv*_tmp1C2;struct Cyc_Tcenv_Fenv*_tmp1C1=_stmttmp19;_tmp1C2=_tmp1C1->ctrl_env;{
# 803
void*_stmttmp1A=_tmp1C2->continue_stmt;void*_tmp1C3=_stmttmp1A;struct Cyc_Absyn_Stmt*_tmp1C5;_LL92: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1C4=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C4->tag != 3)goto _LL94;else{_tmp1C5=_tmp1C4->f1;}}_LL93:
# 805
{struct Cyc_List_List*_tmp3DF;_tmp1C5->non_local_preds=((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->hd=s,((_tmp3DF->tl=_tmp1C5->non_local_preds,_tmp3DF))))));}
*sopt=_tmp1C5;
return;_LL94: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1C6=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C6->tag != 0)goto _LL96;}_LL95:
{const char*_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_Tcutil_terr(s->loc,((_tmp3E2="continue not in a loop",_tag_dyneither(_tmp3E2,sizeof(char),23))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}return;_LL96: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1C7=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C7->tag != 1)goto _LL98;}_LL97:
 goto _LL99;_LL98: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1C8=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C8->tag != 2)goto _LL91;}_LL99: {
const char*_tmp3E3;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3E3="bad continue destination",_tag_dyneither(_tmp3E3,sizeof(char),25))));}_LL91:;};}
# 813
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3E4;struct Cyc_Tcenv_Fenv*_stmttmp1B=
Cyc_Tcenv_get_fenv(te,((_tmp3E4="process_break",_tag_dyneither(_tmp3E4,sizeof(char),14))));
# 814
struct Cyc_Tcenv_CtrlEnv*_tmp1CF;struct Cyc_Tcenv_SharedFenv*_tmp1D0;struct Cyc_Tcenv_Fenv*_tmp1CE=_stmttmp1B;_tmp1CF=_tmp1CE->ctrl_env;_tmp1D0=_tmp1CE->shared;{
# 816
void*_stmttmp1C=_tmp1CF->break_stmt;void*_tmp1D1=_stmttmp1C;struct Cyc_Absyn_Stmt*_tmp1D3;_LL9B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1D2=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D2->tag != 3)goto _LL9D;else{_tmp1D3=_tmp1D2->f1;}}_LL9C:
# 818
{struct Cyc_List_List*_tmp3E5;_tmp1D3->non_local_preds=((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5->hd=s,((_tmp3E5->tl=_tmp1D3->non_local_preds,_tmp3E5))))));}
*sopt=_tmp1D3;
return;_LL9D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1D4=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D4->tag != 0)goto _LL9F;}_LL9E:
# 822
{const char*_tmp3E8;void*_tmp3E7;(_tmp3E7=0,Cyc_Tcutil_terr(s->loc,((_tmp3E8="break not in a loop or switch",_tag_dyneither(_tmp3E8,sizeof(char),30))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}
return;_LL9F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1D5=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D5->tag != 2)goto _LLA1;}_LLA0:
# 842 "tcenv.cyc"
 return;_LLA1: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1D6=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D6->tag != 1)goto _LL9A;}_LLA2:
# 847
{const char*_tmp3EB;void*_tmp3EA;(_tmp3EA=0,Cyc_Tcutil_terr(s->loc,((_tmp3EB="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3EB,sizeof(char),56))),_tag_dyneither(_tmp3EA,sizeof(void*),0)));}
return;_LL9A:;};}
# 851
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3EC;struct Cyc_Tcenv_Fenv*_stmttmp1D=Cyc_Tcenv_get_fenv(te,((_tmp3EC="process_goto",_tag_dyneither(_tmp3EC,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1DE;struct Cyc_Tcenv_Fenv*_tmp1DD=_stmttmp1D;_tmp1DE=_tmp1DD->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1DE->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1DF=_tmp1DE->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1DF,l);
struct _RegionHandle*frgn=_tmp1DE->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3ED;slopt=((_tmp3ED=_region_malloc(frgn,sizeof(*_tmp3ED)),((_tmp3ED[0]=0,_tmp3ED))));}{
struct Cyc_List_List*_tmp3EE;struct Cyc_List_List*new_needed=(_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE->hd=s,((_tmp3EE->tl=*slopt,_tmp3EE)))));
_tmp1DE->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1DF,l,new_needed);};}else{
# 863
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3EF;s->non_local_preds=((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF->hd=s,((_tmp3EF->tl=s->non_local_preds,_tmp3EF))))));}
*sopt=s;}};}
# 869
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 871
const char*_tmp3F0;struct Cyc_Tcenv_Fenv*_stmttmp1E=Cyc_Tcenv_get_fenv(te,((_tmp3F0="process_fallthru",_tag_dyneither(_tmp3F0,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp1E5;struct Cyc_Tcenv_Fenv*_tmp1E4=_stmttmp1E;_tmp1E5=_tmp1E4->ctrl_env;{
const struct _tuple10*_tmp1E6=_tmp1E5->fallthru_clause;
if(_tmp1E6 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3F1;(((*_tmp1E6).f1)->body)->non_local_preds=(
(_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1->hd=s,((_tmp3F1->tl=(((*_tmp1E6).f1)->body)->non_local_preds,_tmp3F1))))));}{
struct Cyc_Absyn_Switch_clause**_tmp3F2;*clauseopt=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=(*_tmp1E6).f1,_tmp3F2))));};}
# 878
return _tmp1E6;};}
# 881
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 885
const char*_tmp3F3;struct Cyc_Tcenv_Fenv*_stmttmp1F=
# 887
Cyc_Tcenv_get_fenv(te,((_tmp3F3="set_fallthru",_tag_dyneither(_tmp3F3,sizeof(char),13))));
# 885
struct Cyc_Tcenv_SharedFenv*_tmp1EB;struct Cyc_List_List*_tmp1EC;struct Cyc_RgnOrder_RgnPO*_tmp1ED;const struct Cyc_Tcenv_Bindings*_tmp1EE;struct Cyc_Absyn_Stmt*_tmp1EF;struct Cyc_Tcenv_CtrlEnv*_tmp1F0;void*_tmp1F1;void*_tmp1F2;struct Cyc_Tcenv_FenvFlags _tmp1F3;struct Cyc_Tcenv_Fenv*_tmp1EA=_stmttmp1F;_tmp1EB=_tmp1EA->shared;_tmp1EC=_tmp1EA->type_vars;_tmp1ED=_tmp1EA->region_order;_tmp1EE=_tmp1EA->locals;_tmp1EF=_tmp1EA->encloser;_tmp1F0=_tmp1EA->ctrl_env;_tmp1F1=_tmp1EA->capability;_tmp1F2=_tmp1EA->curr_rgn;_tmp1F3=_tmp1EA->flags;{
# 888
struct _RegionHandle*_tmp1F5;void*_tmp1F6;void*_tmp1F7;const struct _tuple10*_tmp1F8;void*_tmp1F9;int _tmp1FA;struct Cyc_Tcenv_CtrlEnv*_tmp1F4=_tmp1F0;_tmp1F5=_tmp1F4->ctrl_rgn;_tmp1F6=_tmp1F4->continue_stmt;_tmp1F7=_tmp1F4->break_stmt;_tmp1F8=_tmp1F4->fallthru_clause;_tmp1F9=_tmp1F4->next_stmt;_tmp1FA=_tmp1F4->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3F4;ft_typ=((_tmp3F4=_region_malloc(_tmp1F5,sizeof(*_tmp3F4)),((_tmp3F4->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3F4->tl=ft_typ,_tmp3F4))))));}{
const struct Cyc_Tcenv_CList*_tmp1FC=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp3F7;struct Cyc_Tcenv_CtrlEnv*_tmp3F6;struct Cyc_Tcenv_CtrlEnv*_tmp1FD=
(_tmp3F6=_region_malloc(r,sizeof(*_tmp3F6)),((_tmp3F6->ctrl_rgn=_tmp1F5,((_tmp3F6->continue_stmt=_tmp1F6,((_tmp3F6->break_stmt=_tmp1F7,((_tmp3F6->fallthru_clause=(const struct _tuple10*)((_tmp3F7=_region_malloc(_tmp1F5,sizeof(*_tmp3F7)),((_tmp3F7->f1=clause,((_tmp3F7->f2=new_tvs,((_tmp3F7->f3=_tmp1FC,_tmp3F7)))))))),((_tmp3F6->next_stmt=_tmp1F9,((_tmp3F6->try_depth=_tmp1FA,_tmp3F6)))))))))))));
# 896
struct Cyc_Tcenv_Fenv*_tmp3F8;struct Cyc_Tcenv_Fenv*_tmp1FE=(_tmp3F8=_region_malloc(r,sizeof(*_tmp3F8)),((_tmp3F8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1EB,((_tmp3F8->type_vars=(struct Cyc_List_List*)_tmp1EC,((_tmp3F8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1ED,((_tmp3F8->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1EE,((_tmp3F8->encloser=(struct Cyc_Absyn_Stmt*)_tmp1EF,((_tmp3F8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1FD,((_tmp3F8->capability=(void*)_tmp1F1,((_tmp3F8->curr_rgn=(void*)_tmp1F2,((_tmp3F8->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1F3,((_tmp3F8->fnrgn=(struct _RegionHandle*)r,_tmp3F8)))))))))))))))))))));
# 900
return Cyc_Tcenv_put_fenv(r,te,_tmp1FE);};};};}
# 903
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3F9;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3F9="clear_fallthru",_tag_dyneither(_tmp3F9,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp204;struct Cyc_Tcenv_Fenv*_tmp203=fe;_tmp204=_tmp203->ctrl_env;
_tmp204->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 910
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 912
const char*_tmp3FA;struct Cyc_Tcenv_Fenv*_stmttmp20=
# 914
Cyc_Tcenv_get_fenv(te,((_tmp3FA="set_in_loop",_tag_dyneither(_tmp3FA,sizeof(char),12))));
# 912
struct Cyc_Tcenv_SharedFenv*_tmp207;struct Cyc_List_List*_tmp208;struct Cyc_RgnOrder_RgnPO*_tmp209;const struct Cyc_Tcenv_Bindings*_tmp20A;struct Cyc_Absyn_Stmt*_tmp20B;struct Cyc_Tcenv_CtrlEnv*_tmp20C;void*_tmp20D;void*_tmp20E;struct Cyc_Tcenv_FenvFlags _tmp20F;struct Cyc_Tcenv_Fenv*_tmp206=_stmttmp20;_tmp207=_tmp206->shared;_tmp208=_tmp206->type_vars;_tmp209=_tmp206->region_order;_tmp20A=_tmp206->locals;_tmp20B=_tmp206->encloser;_tmp20C=_tmp206->ctrl_env;_tmp20D=_tmp206->capability;_tmp20E=_tmp206->curr_rgn;_tmp20F=_tmp206->flags;{
# 915
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp405;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp404;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp403;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp402;struct Cyc_Tcenv_CtrlEnv*_tmp401;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp401=_region_malloc(r,sizeof(*_tmp401)),((_tmp401->ctrl_rgn=r,((_tmp401->continue_stmt=(void*)(
(_tmp403=_region_malloc(r,sizeof(*_tmp403)),((_tmp403[0]=((_tmp402.tag=3,((_tmp402.f1=continue_dest,_tmp402)))),_tmp403)))),((_tmp401->break_stmt=_tmp20C->next_stmt,((_tmp401->next_stmt=(void*)(
# 919
(_tmp405=_region_malloc(r,sizeof(*_tmp405)),((_tmp405[0]=((_tmp404.tag=3,((_tmp404.f1=continue_dest,_tmp404)))),_tmp405)))),((_tmp401->fallthru_clause=_tmp20C->fallthru_clause,((_tmp401->try_depth=_tmp20C->try_depth,_tmp401)))))))))))));
# 922
struct Cyc_Tcenv_Fenv*_tmp406;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp406=_region_malloc(r,sizeof(*_tmp406)),((_tmp406->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp207,((_tmp406->type_vars=(struct Cyc_List_List*)_tmp208,((_tmp406->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp209,((_tmp406->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20A,((_tmp406->encloser=(struct Cyc_Absyn_Stmt*)_tmp20B,((_tmp406->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp406->capability=(void*)_tmp20D,((_tmp406->curr_rgn=(void*)_tmp20E,((_tmp406->flags=(struct Cyc_Tcenv_FenvFlags)_tmp20F,((_tmp406->fnrgn=(struct _RegionHandle*)r,_tmp406)))))))))))))))))))));
# 925
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 928
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp407;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp407="enter_try",_tag_dyneither(_tmp407,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp218;struct Cyc_Tcenv_Fenv*_tmp217=fe;_tmp218=_tmp217->ctrl_env;
++ _tmp218->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 934
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp408;struct Cyc_Tcenv_Fenv*_stmttmp21=Cyc_Tcenv_get_fenv(te,((_tmp408="get_try_depth",_tag_dyneither(_tmp408,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp21B;struct Cyc_Tcenv_Fenv*_tmp21A=_stmttmp21;_tmp21B=_tmp21A->ctrl_env;
return _tmp21B->try_depth;}
# 941
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp409;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp409="set_in_switch",_tag_dyneither(_tmp409,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp21E;struct Cyc_Tcenv_Fenv*_tmp21D=fe;_tmp21E=_tmp21D->ctrl_env;
_tmp21E->break_stmt=_tmp21E->next_stmt;
_tmp21E->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 949
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 951
const char*_tmp40A;struct Cyc_Tcenv_Fenv _stmttmp22=*
# 953
Cyc_Tcenv_get_fenv(te,((_tmp40A="set_next",_tag_dyneither(_tmp40A,sizeof(char),9))));
# 951
struct Cyc_Tcenv_SharedFenv*_tmp221;struct Cyc_List_List*_tmp222;struct Cyc_RgnOrder_RgnPO*_tmp223;const struct Cyc_Tcenv_Bindings*_tmp224;struct Cyc_Absyn_Stmt*_tmp225;struct Cyc_Tcenv_CtrlEnv*_tmp226;void*_tmp227;void*_tmp228;struct Cyc_Tcenv_FenvFlags _tmp229;struct Cyc_Tcenv_Fenv _tmp220=_stmttmp22;_tmp221=_tmp220.shared;_tmp222=_tmp220.type_vars;_tmp223=_tmp220.region_order;_tmp224=_tmp220.locals;_tmp225=_tmp220.encloser;_tmp226=_tmp220.ctrl_env;_tmp227=_tmp220.capability;_tmp228=_tmp220.curr_rgn;_tmp229=_tmp220.flags;{
# 954
struct Cyc_Tcenv_CtrlEnv*_tmp40B;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp40B=_region_malloc(r,sizeof(*_tmp40B)),((_tmp40B->ctrl_rgn=r,((_tmp40B->continue_stmt=_tmp226->continue_stmt,((_tmp40B->break_stmt=_tmp226->break_stmt,((_tmp40B->next_stmt=j,((_tmp40B->fallthru_clause=_tmp226->fallthru_clause,((_tmp40B->try_depth=_tmp226->try_depth,_tmp40B)))))))))))));
# 962
struct Cyc_Tcenv_Fenv*_tmp40C;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp40C=_region_malloc(r,sizeof(*_tmp40C)),((_tmp40C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp221,((_tmp40C->type_vars=(struct Cyc_List_List*)_tmp222,((_tmp40C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp223,((_tmp40C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp224,((_tmp40C->encloser=(struct Cyc_Absyn_Stmt*)_tmp225,((_tmp40C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp40C->capability=(void*)_tmp227,((_tmp40C->curr_rgn=(void*)_tmp228,((_tmp40C->flags=(struct Cyc_Tcenv_FenvFlags)_tmp229,((_tmp40C->fnrgn=(struct _RegionHandle*)r,_tmp40C)))))))))))))))))))));
# 965
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 968
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp40D;struct Cyc_Tcenv_Fenv*_stmttmp23=Cyc_Tcenv_get_fenv(te,((_tmp40D="add_label",_tag_dyneither(_tmp40D,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp22E;struct Cyc_Tcenv_Fenv*_tmp22D=_stmttmp23;_tmp22E=_tmp22D->shared;{
struct Cyc_Dict_Dict needed=_tmp22E->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp22E->frgn;
if(sl_opt != 0){
_tmp22E->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 976
struct Cyc_List_List*_tmp22F=*sl_opt;
s->non_local_preds=_tmp22F;
for(0;_tmp22F != 0;_tmp22F=_tmp22F->tl){
void*_stmttmp24=((struct Cyc_Absyn_Stmt*)_tmp22F->hd)->r;void*_tmp230=_stmttmp24;struct Cyc_Absyn_Stmt**_tmp232;_LLA4: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp231=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp230;if(_tmp231->tag != 8)goto _LLA6;else{_tmp232=(struct Cyc_Absyn_Stmt**)& _tmp231->f2;}}_LLA5:
*_tmp232=s;goto _LLA3;_LLA6:;_LLA7:
{const char*_tmp40E;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp40E="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp40E,sizeof(char),42))));}goto _LLA3;_LLA3:;}};}
# 984
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp22E->seen_labels,v)){
const char*_tmp412;void*_tmp411[1];struct Cyc_String_pa_PrintArg_struct _tmp410;(_tmp410.tag=0,((_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp411[0]=& _tmp410,Cyc_Tcutil_terr(s->loc,((_tmp412="Repeated label: %s",_tag_dyneither(_tmp412,sizeof(char),19))),_tag_dyneither(_tmp411,sizeof(void*),1)))))));}
_tmp22E->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp22E->seen_labels,v,s);
return te;};}
# 990
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp413;struct Cyc_Tcenv_Fenv*_stmttmp25=Cyc_Tcenv_get_fenv(te,((_tmp413="all_labels_resolved",_tag_dyneither(_tmp413,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp239;struct Cyc_Tcenv_Fenv*_tmp238=_stmttmp25;_tmp239=_tmp238->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp239->needed_labels);}
# 995
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp414;struct Cyc_Tcenv_Fenv*_stmttmp26=Cyc_Tcenv_get_fenv(te,((_tmp414="get_encloser",_tag_dyneither(_tmp414,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp23C;struct Cyc_Tcenv_Fenv*_tmp23B=_stmttmp26;_tmp23C=_tmp23B->encloser;
return _tmp23C;}
# 1000
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp415;return(_tmp415=_region_malloc(r2,sizeof(*_tmp415)),((_tmp415->ns=te->ns,((_tmp415->ae=te->ae,((_tmp415->le=0,((_tmp415->allow_valueof=1,_tmp415)))))))));}{
struct Cyc_Tcenv_Fenv _stmttmp27=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp240;struct Cyc_List_List*_tmp241;struct Cyc_RgnOrder_RgnPO*_tmp242;const struct Cyc_Tcenv_Bindings*_tmp243;struct Cyc_Absyn_Stmt*_tmp244;struct Cyc_Tcenv_CtrlEnv*_tmp245;void*_tmp246;void*_tmp247;struct Cyc_Tcenv_FenvFlags _tmp248;struct Cyc_Tcenv_Fenv _tmp23F=_stmttmp27;_tmp240=_tmp23F.shared;_tmp241=_tmp23F.type_vars;_tmp242=_tmp23F.region_order;_tmp243=_tmp23F.locals;_tmp244=_tmp23F.encloser;_tmp245=_tmp23F.ctrl_env;_tmp246=_tmp23F.capability;_tmp247=_tmp23F.curr_rgn;_tmp248=_tmp23F.flags;{
struct Cyc_Tcenv_Fenv*_tmp416;struct Cyc_Tcenv_Fenv*_tmp249=(_tmp416=_region_malloc(r2,sizeof(*_tmp416)),((_tmp416->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp240,((_tmp416->type_vars=(struct Cyc_List_List*)_tmp241,((_tmp416->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp242,((_tmp416->locals=(const struct Cyc_Tcenv_Bindings*)_tmp243,((_tmp416->encloser=(struct Cyc_Absyn_Stmt*)_tmp244,((_tmp416->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp245,((_tmp416->capability=(void*)_tmp246,((_tmp416->curr_rgn=(void*)_tmp247,((_tmp416->flags=(struct Cyc_Tcenv_FenvFlags)_tmp248,((_tmp416->fnrgn=(struct _RegionHandle*)r2,_tmp416)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp417;return(_tmp417=_region_malloc(r2,sizeof(*_tmp417)),((_tmp417->ns=te->ns,((_tmp417->ae=te->ae,((_tmp417->le=_tmp249,((_tmp417->allow_valueof=1,_tmp417)))))))));};};}
# 1008
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1010
const char*_tmp418;struct Cyc_Tcenv_Fenv _stmttmp28=*
# 1012
Cyc_Tcenv_get_fenv(te,((_tmp418="set_encloser",_tag_dyneither(_tmp418,sizeof(char),13))));
# 1010
struct Cyc_Tcenv_SharedFenv*_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_RgnOrder_RgnPO*_tmp24F;const struct Cyc_Tcenv_Bindings*_tmp250;struct Cyc_Absyn_Stmt*_tmp251;struct Cyc_Tcenv_CtrlEnv*_tmp252;void*_tmp253;void*_tmp254;struct Cyc_Tcenv_FenvFlags _tmp255;struct Cyc_Tcenv_Fenv _tmp24C=_stmttmp28;_tmp24D=_tmp24C.shared;_tmp24E=_tmp24C.type_vars;_tmp24F=_tmp24C.region_order;_tmp250=_tmp24C.locals;_tmp251=_tmp24C.encloser;_tmp252=_tmp24C.ctrl_env;_tmp253=_tmp24C.capability;_tmp254=_tmp24C.curr_rgn;_tmp255=_tmp24C.flags;{
# 1013
struct Cyc_Tcenv_Fenv*_tmp419;struct Cyc_Tcenv_Fenv*_tmp256=
(_tmp419=_region_malloc(r,sizeof(*_tmp419)),((_tmp419->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp24D,((_tmp419->type_vars=(struct Cyc_List_List*)_tmp24E,((_tmp419->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24F,((_tmp419->locals=(const struct Cyc_Tcenv_Bindings*)_tmp250,((_tmp419->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp419->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp252,((_tmp419->capability=(void*)_tmp253,((_tmp419->curr_rgn=(void*)_tmp254,((_tmp419->flags=(struct Cyc_Tcenv_FenvFlags)_tmp255,((_tmp419->fnrgn=(struct _RegionHandle*)r,_tmp419)))))))))))))))))))));
# 1016
return Cyc_Tcenv_put_fenv(r,te,_tmp256);};}
# 1019
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1022
struct Cyc_Absyn_Tvar*tv;
{void*_stmttmp29=Cyc_Tcutil_compress(rgn);void*_tmp259=_stmttmp29;struct Cyc_Absyn_Tvar*_tmp25B;_LLA9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp259;if(_tmp25A->tag != 2)goto _LLAB;else{_tmp25B=_tmp25A->f1;}}_LLAA:
 tv=_tmp25B;goto _LLA8;_LLAB:;_LLAC:
{const char*_tmp41A;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp41A="bad add region",_tag_dyneither(_tmp41A,sizeof(char),15))));}goto _LLA8;_LLA8:;}{
# 1027
const char*_tmp41B;struct Cyc_Tcenv_Fenv _stmttmp2A=*
# 1029
Cyc_Tcenv_get_fenv(te,((_tmp41B="add_region",_tag_dyneither(_tmp41B,sizeof(char),11))));
# 1027
struct Cyc_Tcenv_SharedFenv*_tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_RgnOrder_RgnPO*_tmp260;const struct Cyc_Tcenv_Bindings*_tmp261;struct Cyc_Absyn_Stmt*_tmp262;struct Cyc_Tcenv_CtrlEnv*_tmp263;void*_tmp264;void*_tmp265;struct Cyc_Tcenv_FenvFlags _tmp266;struct Cyc_Tcenv_Fenv _tmp25D=_stmttmp2A;_tmp25E=_tmp25D.shared;_tmp25F=_tmp25D.type_vars;_tmp260=_tmp25D.region_order;_tmp261=_tmp25D.locals;_tmp262=_tmp25D.encloser;_tmp263=_tmp25D.ctrl_env;_tmp264=_tmp25D.capability;_tmp265=_tmp25D.curr_rgn;_tmp266=_tmp25D.flags;
# 1030
_tmp260=Cyc_RgnOrder_add_youngest(_tmp25E->frgn,_tmp260,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp42E;struct Cyc_List_List*_tmp42D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp42C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp42B;struct Cyc_List_List*_tmp42A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp429;_tmp264=(void*)((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429[0]=((_tmp42E.tag=24,((_tmp42E.f1=((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A->hd=(void*)((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=((_tmp42B.tag=23,((_tmp42B.f1=rgn,_tmp42B)))),_tmp42C)))),((_tmp42A->tl=(
(_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->hd=_tmp264,((_tmp42D->tl=0,_tmp42D)))))),_tmp42A)))))),_tmp42E)))),_tmp429))));}{
struct Cyc_Tcenv_Fenv*_tmp42F;struct Cyc_Tcenv_Fenv*_tmp26D=
(_tmp42F=_region_malloc(r,sizeof(*_tmp42F)),((_tmp42F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25E,((_tmp42F->type_vars=(struct Cyc_List_List*)_tmp25F,((_tmp42F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp260,((_tmp42F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp261,((_tmp42F->encloser=(struct Cyc_Absyn_Stmt*)_tmp262,((_tmp42F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp263,((_tmp42F->capability=(void*)_tmp264,((_tmp42F->curr_rgn=(void*)_tmp265,((_tmp42F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp266,((_tmp42F->fnrgn=(struct _RegionHandle*)r,_tmp42F)))))))))))))))))))));
# 1036
return Cyc_Tcenv_put_fenv(r,te,_tmp26D);};};}
# 1039
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1041
const char*_tmp430;struct Cyc_Tcenv_Fenv _stmttmp2B=*
# 1043
Cyc_Tcenv_get_fenv(te,((_tmp430="new_named_block",_tag_dyneither(_tmp430,sizeof(char),16))));
# 1041
struct Cyc_Tcenv_SharedFenv*_tmp271;struct Cyc_List_List*_tmp272;struct Cyc_RgnOrder_RgnPO*_tmp273;const struct Cyc_Tcenv_Bindings*_tmp274;struct Cyc_Absyn_Stmt*_tmp275;struct Cyc_Tcenv_CtrlEnv*_tmp276;void*_tmp277;void*_tmp278;struct Cyc_Tcenv_FenvFlags _tmp279;struct Cyc_Tcenv_Fenv _tmp270=_stmttmp2B;_tmp271=_tmp270.shared;_tmp272=_tmp270.type_vars;_tmp273=_tmp270.region_order;_tmp274=_tmp270.locals;_tmp275=_tmp270.encloser;_tmp276=_tmp270.ctrl_env;_tmp277=_tmp270.capability;_tmp278=_tmp270.curr_rgn;_tmp279=_tmp270.flags;{
# 1045
const char*_tmp431;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp431="new_block",_tag_dyneither(_tmp431,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp434;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp433;void*block_typ=(void*)((_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433[0]=((_tmp434.tag=2,((_tmp434.f1=block_rgn,_tmp434)))),_tmp433))));
{struct Cyc_List_List*_tmp435;_tmp272=((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435->hd=block_rgn,((_tmp435->tl=_tmp272,_tmp435))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp272);
_tmp273=Cyc_RgnOrder_add_youngest(_tmp271->frgn,_tmp273,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp448;struct Cyc_List_List*_tmp447;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp446;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp445;struct Cyc_List_List*_tmp444;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp443;_tmp277=(void*)((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443[0]=((_tmp448.tag=24,((_tmp448.f1=((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444->hd=(void*)((_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446[0]=((_tmp445.tag=23,((_tmp445.f1=block_typ,_tmp445)))),_tmp446)))),((_tmp444->tl=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->hd=_tmp277,((_tmp447->tl=0,_tmp447)))))),_tmp444)))))),_tmp448)))),_tmp443))));}
_tmp278=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp449;struct Cyc_Tcenv_Fenv*_tmp281=
(_tmp449=_region_malloc(r,sizeof(*_tmp449)),((_tmp449->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp271,((_tmp449->type_vars=(struct Cyc_List_List*)_tmp272,((_tmp449->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp273,((_tmp449->locals=(const struct Cyc_Tcenv_Bindings*)_tmp274,((_tmp449->encloser=(struct Cyc_Absyn_Stmt*)_tmp275,((_tmp449->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp276,((_tmp449->capability=(void*)_tmp277,((_tmp449->curr_rgn=(void*)_tmp278,((_tmp449->flags=(struct Cyc_Tcenv_FenvFlags)_tmp279,((_tmp449->fnrgn=(struct _RegionHandle*)r,_tmp449)))))))))))))))))))));
# 1055
return Cyc_Tcenv_put_fenv(r,te,_tmp281);};};}
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
const char*_tmp44A;struct Cyc_Tcenv_Fenv _stmttmp2C=*
# 1074
Cyc_Tcenv_get_fenv(te,((_tmp44A="new_outlives_constraints",_tag_dyneither(_tmp44A,sizeof(char),25))));
# 1072
struct Cyc_Tcenv_SharedFenv*_tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_RgnOrder_RgnPO*_tmp28B;const struct Cyc_Tcenv_Bindings*_tmp28C;struct Cyc_Absyn_Stmt*_tmp28D;struct Cyc_Tcenv_CtrlEnv*_tmp28E;void*_tmp28F;void*_tmp290;struct Cyc_Tcenv_FenvFlags _tmp291;struct Cyc_Tcenv_Fenv _tmp288=_stmttmp2C;_tmp289=_tmp288.shared;_tmp28A=_tmp288.type_vars;_tmp28B=_tmp288.region_order;_tmp28C=_tmp288.locals;_tmp28D=_tmp288.encloser;_tmp28E=_tmp288.ctrl_env;_tmp28F=_tmp288.capability;_tmp290=_tmp288.curr_rgn;_tmp291=_tmp288.flags;
# 1075
for(0;cs != 0;cs=cs->tl){
_tmp28B=
Cyc_RgnOrder_add_outlives_constraint(_tmp289->frgn,_tmp28B,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1080
struct Cyc_Tcenv_Fenv*_tmp44B;struct Cyc_Tcenv_Fenv*_tmp292=
(_tmp44B=_region_malloc(r,sizeof(*_tmp44B)),((_tmp44B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp289,((_tmp44B->type_vars=(struct Cyc_List_List*)_tmp28A,((_tmp44B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp28B,((_tmp44B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp28C,((_tmp44B->encloser=(struct Cyc_Absyn_Stmt*)_tmp28D,((_tmp44B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp28E,((_tmp44B->capability=(void*)_tmp28F,((_tmp44B->curr_rgn=(void*)_tmp290,((_tmp44B->flags=(struct Cyc_Tcenv_FenvFlags)_tmp291,((_tmp44B->fnrgn=(struct _RegionHandle*)r,_tmp44B)))))))))))))))))))));
# 1083
return Cyc_Tcenv_put_fenv(r,te,_tmp292);};}
# 1086
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1094
void*_tmp295=Cyc_Tcutil_compress(r1);
void*_tmp296=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp297=Cyc_Tcutil_typ_kind(_tmp295);
struct Cyc_Absyn_Kind*_tmp298=Cyc_Tcutil_typ_kind(_tmp296);
# 1100
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp297,_tmp298);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp298,_tmp297);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp452;void*_tmp451[4];struct Cyc_String_pa_PrintArg_struct _tmp450;struct Cyc_String_pa_PrintArg_struct _tmp44F;struct Cyc_String_pa_PrintArg_struct _tmp44E;struct Cyc_String_pa_PrintArg_struct _tmp44D;(_tmp44D.tag=0,((_tmp44D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp298)),((_tmp44E.tag=0,((_tmp44E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp297)),((_tmp44F.tag=0,((_tmp44F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp296)),((_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp295)),((_tmp451[0]=& _tmp450,((_tmp451[1]=& _tmp44F,((_tmp451[2]=& _tmp44E,((_tmp451[3]=& _tmp44D,Cyc_Tcutil_terr(loc,((_tmp452="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp452,sizeof(char),82))),_tag_dyneither(_tmp451,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1107
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _stmttmp2D=Cyc_Tcutil_swap_kind(_tmp296,Cyc_Tcutil_kind_to_bound(_tmp297));struct Cyc_Absyn_Tvar*_tmp2A0;void*_tmp2A1;struct _tuple11 _tmp29F=_stmttmp2D;_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;{
struct _tuple11*_tmp453;*oldtv=((_tmp453=_region_malloc(r,sizeof(*_tmp453)),((_tmp453->f1=_tmp2A0,((_tmp453->f2=_tmp2A1,_tmp453))))));};}else{
# 1111
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _stmttmp2E=Cyc_Tcutil_swap_kind(_tmp295,Cyc_Tcutil_kind_to_bound(_tmp298));struct Cyc_Absyn_Tvar*_tmp2A4;void*_tmp2A5;struct _tuple11 _tmp2A3=_stmttmp2E;_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;{
struct _tuple11*_tmp454;*oldtv=((_tmp454=_region_malloc(r,sizeof(*_tmp454)),((_tmp454->f1=_tmp2A4,((_tmp454->f2=_tmp2A5,_tmp454))))));};}}}{
# 1117
struct _RegionHandle*_tmp2A7=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2A8=0;
if((_tmp295 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp295 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp295 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp457;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp456;void*eff1=(void*)((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456[0]=((_tmp457.tag=23,((_tmp457.f1=_tmp295,_tmp457)))),_tmp456))));
struct _tuple13*_tmp45A;struct Cyc_List_List*_tmp459;_tmp2A8=((_tmp459=_region_malloc(_tmp2A7,sizeof(*_tmp459)),((_tmp459->hd=((_tmp45A=_region_malloc(_tmp2A7,sizeof(*_tmp45A)),((_tmp45A->f1=eff1,((_tmp45A->f2=_tmp296,_tmp45A)))))),((_tmp459->tl=_tmp2A8,_tmp459))))));}
# 1123
if((_tmp296 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp296 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp296 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp45D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp45C;void*eff2=(void*)((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C[0]=((_tmp45D.tag=23,((_tmp45D.f1=_tmp296,_tmp45D)))),_tmp45C))));
struct _tuple13*_tmp460;struct Cyc_List_List*_tmp45F;_tmp2A8=((_tmp45F=_region_malloc(_tmp2A7,sizeof(*_tmp45F)),((_tmp45F->hd=((_tmp460=_region_malloc(_tmp2A7,sizeof(*_tmp460)),((_tmp460->f1=eff2,((_tmp460->f2=_tmp295,_tmp460)))))),((_tmp45F->tl=_tmp2A8,_tmp45F))))));}
# 1128
return Cyc_Tcenv_new_outlives_constraints(_tmp2A7,te,_tmp2A8,loc);};}
# 1131
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B1=te->le;
if(_tmp2B1 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2B1;
void*_tmp2B3;struct Cyc_Tcenv_Fenv*_tmp2B2=fe;_tmp2B3=_tmp2B2->curr_rgn;
return _tmp2B3;};}
# 1142
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp461;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp461="check_rgn_accessible",_tag_dyneither(_tmp461,sizeof(char),21))));
void*_tmp2B5;struct Cyc_RgnOrder_RgnPO*_tmp2B6;struct Cyc_Tcenv_Fenv*_tmp2B4=fe;_tmp2B5=_tmp2B4->capability;_tmp2B6=_tmp2B4->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2B5) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2B5))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp464;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp463;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2B6,(void*)((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=((_tmp464.tag=23,((_tmp464.f1=rgn,_tmp464)))),_tmp463)))),_tmp2B5))
return;}{
const char*_tmp468;void*_tmp467[1];struct Cyc_String_pa_PrintArg_struct _tmp466;(_tmp466.tag=0,((_tmp466.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp467[0]=& _tmp466,Cyc_Tcutil_terr(loc,((_tmp468="Expression accesses unavailable region %s",_tag_dyneither(_tmp468,sizeof(char),42))),_tag_dyneither(_tmp467,sizeof(void*),1)))))));};}
# 1154
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp469;struct Cyc_Tcenv_Fenv*_stmttmp2F=
Cyc_Tcenv_get_fenv(te,((_tmp469="check_rgn_resetable",_tag_dyneither(_tmp469,sizeof(char),20))));
# 1156
struct Cyc_RgnOrder_RgnPO*_tmp2BE;struct Cyc_Tcenv_Fenv*_tmp2BD=_stmttmp2F;_tmp2BE=_tmp2BD->region_order;{
# 1158
void*_stmttmp30=Cyc_Tcutil_compress(rgn);void*_tmp2BF=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp2C1;_LLAE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BF;if(_tmp2C0->tag != 2)goto _LLB0;else{_tmp2C1=_tmp2C0->f1;}}_LLAF:
# 1160
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2BE,_tmp2C1)){
const char*_tmp46D;void*_tmp46C[1];struct Cyc_String_pa_PrintArg_struct _tmp46B;(_tmp46B.tag=0,((_tmp46B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp46C[0]=& _tmp46B,Cyc_Tcutil_terr(loc,((_tmp46D="Region %s is not resetable",_tag_dyneither(_tmp46D,sizeof(char),27))),_tag_dyneither(_tmp46C,sizeof(void*),1)))))));}
return;_LLB0:;_LLB1: {
# 1164
const char*_tmp470;void*_tmp46F;(_tmp46F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp470="check_rgn_resetable",_tag_dyneither(_tmp470,sizeof(char),20))),_tag_dyneither(_tmp46F,sizeof(void*),0)));}_LLAD:;};};}
# 1171
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2C8=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1176
if(_tmp2C8 == 0){
void*_tmp2C9=rt_a;_LLB3: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2CA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2C9;if(_tmp2CA->tag != 22)goto _LLB5;}_LLB4:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LLB5: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2CB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2C9;if(_tmp2CB->tag != 21)goto _LLB7;}_LLB6:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLB7: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2C9;if(_tmp2CC->tag != 20)goto _LLB9;}_LLB8:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLB9:;_LLBA:
 return 0;_LLB2:;}{
# 1184
struct Cyc_Tcenv_Fenv*fe=_tmp2C8;
struct Cyc_RgnOrder_RgnPO*_tmp2CE;struct Cyc_Tcenv_Fenv*_tmp2CD=fe;_tmp2CE=_tmp2CD->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp473;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp472;int _tmp2CF=Cyc_RgnOrder_effect_outlives(_tmp2CE,(void*)((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472[0]=((_tmp473.tag=23,((_tmp473.f1=rt_a,_tmp473)))),_tmp472)))),rt_b);
# 1190
return _tmp2CF;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1195
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp474;struct Cyc_Tcenv_Fenv*_stmttmp31=
Cyc_Tcenv_get_fenv(te,((_tmp474="check_effect_accessible",_tag_dyneither(_tmp474,sizeof(char),24))));
# 1196
void*_tmp2D3;struct Cyc_RgnOrder_RgnPO*_tmp2D4;struct Cyc_Tcenv_SharedFenv*_tmp2D5;struct Cyc_Tcenv_Fenv*_tmp2D2=_stmttmp31;_tmp2D3=_tmp2D2->capability;_tmp2D4=_tmp2D2->region_order;_tmp2D5=_tmp2D2->shared;
# 1198
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2D3))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2D4,eff,_tmp2D3))
return;{
struct _RegionHandle*frgn=_tmp2D5->frgn;
struct _tuple14*_tmp477;struct Cyc_List_List*_tmp476;_tmp2D5->delayed_effect_checks=(
(_tmp476=_region_malloc(frgn,sizeof(*_tmp476)),((_tmp476->hd=((_tmp477=_region_malloc(frgn,sizeof(*_tmp477)),((_tmp477->f1=_tmp2D3,((_tmp477->f2=eff,((_tmp477->f3=_tmp2D4,((_tmp477->f4=loc,_tmp477)))))))))),((_tmp476->tl=_tmp2D5->delayed_effect_checks,_tmp476))))));};}
# 1207
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp478;struct Cyc_Tcenv_Fenv*_stmttmp32=
Cyc_Tcenv_get_fenv(te,((_tmp478="check_delayed_effects",_tag_dyneither(_tmp478,sizeof(char),22))));
# 1208
struct Cyc_Tcenv_SharedFenv*_tmp2DA;struct Cyc_Tcenv_Fenv*_tmp2D9=_stmttmp32;_tmp2DA=_tmp2D9->shared;{
# 1210
struct Cyc_List_List*_tmp2DB=_tmp2DA->delayed_effect_checks;
for(0;_tmp2DB != 0;_tmp2DB=_tmp2DB->tl){
struct _tuple14*_stmttmp33=(struct _tuple14*)_tmp2DB->hd;void*_tmp2DD;void*_tmp2DE;struct Cyc_RgnOrder_RgnPO*_tmp2DF;unsigned int _tmp2E0;struct _tuple14*_tmp2DC=_stmttmp33;_tmp2DD=_tmp2DC->f1;_tmp2DE=_tmp2DC->f2;_tmp2DF=_tmp2DC->f3;_tmp2E0=_tmp2DC->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2DE,_tmp2DD))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2DF,_tmp2DE,_tmp2DD))
continue;{
const char*_tmp47D;void*_tmp47C[2];struct Cyc_String_pa_PrintArg_struct _tmp47B;struct Cyc_String_pa_PrintArg_struct _tmp47A;(_tmp47A.tag=0,((_tmp47A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2DE)),((_tmp47B.tag=0,((_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DD)),((_tmp47C[0]=& _tmp47B,((_tmp47C[1]=& _tmp47A,Cyc_Tcutil_terr(_tmp2E0,((_tmp47D="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp47D,sizeof(char),51))),_tag_dyneither(_tmp47C,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1226
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1228
struct Cyc_Tcenv_Fenv*_tmp2E6=te->le;
if(_tmp2E6 == 0){
# 1231
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp480;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp47F;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp480.tag=23,((_tmp480.f1=(*((struct _tuple13*)po->hd)).f2,_tmp480)))),_tmp47F)))),Cyc_Absyn_empty_effect)){
const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_Tcutil_terr(loc,((_tmp483="the required region ordering is not satisfied here",_tag_dyneither(_tmp483,sizeof(char),51))),_tag_dyneither(_tmp482,sizeof(void*),0)));}}
return;}{
# 1237
struct Cyc_Tcenv_Fenv*_stmttmp34=_tmp2E6;struct Cyc_RgnOrder_RgnPO*_tmp2EC;struct Cyc_Tcenv_SharedFenv*_tmp2ED;struct Cyc_Tcenv_Fenv*_tmp2EB=_stmttmp34;_tmp2EC=_tmp2EB->region_order;_tmp2ED=_tmp2EB->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2EC,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp486;struct Cyc_List_List*_tmp485;_tmp2ED->delayed_constraint_checks=(
(_tmp485=_region_malloc(_tmp2ED->frgn,sizeof(*_tmp485)),((_tmp485->hd=((_tmp486=_region_malloc(_tmp2ED->frgn,sizeof(*_tmp486)),((_tmp486->f1=_tmp2EC,((_tmp486->f2=po,((_tmp486->f3=loc,_tmp486)))))))),((_tmp485->tl=_tmp2ED->delayed_constraint_checks,_tmp485))))));}};}
# 1244
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp487;struct Cyc_Tcenv_Fenv*_stmttmp35=
Cyc_Tcenv_get_fenv(te,((_tmp487="check_delayed_constraints",_tag_dyneither(_tmp487,sizeof(char),26))));
# 1245
struct Cyc_Tcenv_SharedFenv*_tmp2F1;struct Cyc_Tcenv_Fenv*_tmp2F0=_stmttmp35;_tmp2F1=_tmp2F0->shared;{
# 1247
struct Cyc_List_List*_tmp2F2=_tmp2F1->delayed_constraint_checks;
for(0;_tmp2F2 != 0;_tmp2F2=_tmp2F2->tl){
struct _tuple15*_stmttmp36=(struct _tuple15*)_tmp2F2->hd;struct Cyc_RgnOrder_RgnPO*_tmp2F4;struct Cyc_List_List*_tmp2F5;unsigned int _tmp2F6;struct _tuple15*_tmp2F3=_stmttmp36;_tmp2F4=_tmp2F3->f1;_tmp2F5=_tmp2F3->f2;_tmp2F6=_tmp2F3->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2F4,_tmp2F5,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp48A;void*_tmp489;(_tmp489=0,Cyc_Tcutil_terr(_tmp2F6,((_tmp48A="the required region ordering is not satisfied here",_tag_dyneither(_tmp48A,sizeof(char),51))),_tag_dyneither(_tmp489,sizeof(void*),0)));}}};}
# 1255
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp497;const char*_tmp496;void*_tmp495[1];struct Cyc_String_pa_PrintArg_struct _tmp494;struct Cyc_Absyn_Tvar*_tmp493;struct Cyc_Absyn_Tvar*rgn0=
(_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->name=((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497[0]=(struct _dyneither_ptr)((_tmp494.tag=0,((_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp495[0]=& _tmp494,Cyc_aprintf(((_tmp496="`%s",_tag_dyneither(_tmp496,sizeof(char),4))),_tag_dyneither(_tmp495,sizeof(void*),1)))))))),_tmp497)))),((_tmp493->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp493->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp493)))))));
struct Cyc_List_List*_tmp498;struct Cyc_List_List*_tmp2FA=(_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->hd=rgn0,((_tmp498->tl=fd->tvs,_tmp498)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp2FA);{
# 1263
struct Cyc_RgnOrder_RgnPO*_tmp2FB=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1265
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp49B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49A;void*param_rgn=(void*)((_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp49B.tag=2,((_tmp49B.f1=rgn0,_tmp49B)))),_tmp49A))));
struct Cyc_List_List*_tmp2FC=0;
{struct Cyc_List_List*_tmp2FD=fd->args;for(0;_tmp2FD != 0;_tmp2FD=_tmp2FD->tl){
struct Cyc_Absyn_Vardecl _tmp4A1;struct _tuple0*_tmp4A0;struct Cyc_Absyn_Vardecl*_tmp49F;struct Cyc_Absyn_Vardecl*_tmp2FE=(_tmp49F=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp49F[0]=(struct Cyc_Absyn_Vardecl)((_tmp4A1.sc=Cyc_Absyn_Public,((_tmp4A1.name=(
(_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->f1=Cyc_Absyn_Loc_n,((_tmp4A0->f2=(*((struct _tuple8*)_tmp2FD->hd)).f1,_tmp4A0)))))),((_tmp4A1.tq=(*((struct _tuple8*)_tmp2FD->hd)).f2,((_tmp4A1.type=(*((struct _tuple8*)_tmp2FD->hd)).f3,((_tmp4A1.initializer=0,((_tmp4A1.rgn=param_rgn,((_tmp4A1.attributes=0,((_tmp4A1.escapes=0,_tmp4A1)))))))))))))))),_tmp49F)));
# 1275
{struct Cyc_List_List _tmp4A4;struct Cyc_List_List*_tmp4A3;_tmp2FC=((_tmp4A3=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4A3[0]=(struct Cyc_List_List)((_tmp4A4.hd=_tmp2FE,((_tmp4A4.tl=_tmp2FC,_tmp4A4)))),_tmp4A3))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4A7;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4A6;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp301=(_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A7.tag=3,((_tmp4A7.f1=_tmp2FE,_tmp4A7)))),_tmp4A6)));
struct _dyneither_ptr*_tmp302=(*((struct _tuple8*)_tmp2FD->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4A8;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4A8=_region_malloc(r,sizeof(*_tmp4A8)),((_tmp4A8->v=_tmp302,((_tmp4A8->b=(void*)_tmp301,((_tmp4A8->tl=locals,_tmp4A8))))))));};}}
# 1280
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp37=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp30A;struct Cyc_Absyn_Tqual _tmp30B;void*_tmp30C;int _tmp30D;struct Cyc_Absyn_VarargInfo _tmp309=_stmttmp37;_tmp30A=_tmp309.name;_tmp30B=_tmp309.tq;_tmp30C=_tmp309.type;_tmp30D=_tmp309.inject;
if(_tmp30A != 0){
void*_tmp30E=Cyc_Absyn_dyneither_typ(_tmp30C,param_rgn,_tmp30B,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4AE;struct _tuple0*_tmp4AD;struct Cyc_Absyn_Vardecl*_tmp4AC;struct Cyc_Absyn_Vardecl*_tmp30F=(_tmp4AC=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4AC[0]=(struct Cyc_Absyn_Vardecl)((_tmp4AE.sc=Cyc_Absyn_Public,((_tmp4AE.name=(
(_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD->f1=Cyc_Absyn_Loc_n,((_tmp4AD->f2=_tmp30A,_tmp4AD)))))),((_tmp4AE.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4AE.type=_tmp30E,((_tmp4AE.initializer=0,((_tmp4AE.rgn=param_rgn,((_tmp4AE.attributes=0,((_tmp4AE.escapes=0,_tmp4AE)))))))))))))))),_tmp4AC)));
# 1291
{struct Cyc_List_List*_tmp4AF;_tmp2FC=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->hd=_tmp30F,((_tmp4AF->tl=_tmp2FC,_tmp4AF))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4B2;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4B1;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp311=(_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B2.tag=3,((_tmp4B2.f1=_tmp30F,_tmp4B2)))),_tmp4B1)));
struct _dyneither_ptr*_tmp312=_tmp30A;
struct Cyc_Tcenv_Bindings*_tmp4B3;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3)),((_tmp4B3->v=_tmp312,((_tmp4B3->b=(void*)_tmp311,((_tmp4B3->tl=locals,_tmp4B3))))))));};}else{
# 1296
const char*_tmp4B6;void*_tmp4B5;(_tmp4B5=0,Cyc_Tcutil_terr(loc,((_tmp4B6="missing name for varargs",_tag_dyneither(_tmp4B6,sizeof(char),25))),_tag_dyneither(_tmp4B5,sizeof(void*),0)));}}
# 1298
{struct Cyc_Core_Opt _tmp4B9;struct Cyc_Core_Opt*_tmp4B8;fd->param_vardecls=((_tmp4B8=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4B8[0]=(struct Cyc_Core_Opt)((_tmp4B9.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2FC),_tmp4B9)),_tmp4B8))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4D7;struct Cyc_List_List*_tmp4D6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4D5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D4;struct Cyc_List_List*_tmp4D3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4D2;struct Cyc_Tcenv_CtrlEnv*_tmp4D1;struct Cyc_Tcenv_SharedFenv*_tmp4D0;struct Cyc_Tcenv_Fenv*_tmp4CF;return(_tmp4CF=_region_malloc(r,sizeof(*_tmp4CF)),((_tmp4CF->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0->frgn=r,((_tmp4D0->return_typ=fd->ret_type,((_tmp4D0->seen_labels=
# 1303
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4D0->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4D0->delayed_effect_checks=0,((_tmp4D0->delayed_constraint_checks=0,_tmp4D0)))))))))))))),((_tmp4CF->type_vars=(struct Cyc_List_List*)_tmp2FA,((_tmp4CF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2FB,((_tmp4CF->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp4CF->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4CF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1312
(_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1)),((_tmp4D1->ctrl_rgn=r,((_tmp4D1->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4D1->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4D1->fallthru_clause=0,((_tmp4D1->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4D1->try_depth=0,_tmp4D1)))))))))))))),((_tmp4CF->capability=(void*)((void*)(
(_tmp4D7=_cycalloc(sizeof(*_tmp4D7)),((_tmp4D7[0]=((_tmp4D2.tag=24,((_tmp4D2.f1=((_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6->hd=(void*)((_tmp4D4=_cycalloc(sizeof(*_tmp4D4)),((_tmp4D4[0]=((_tmp4D5.tag=23,((_tmp4D5.f1=param_rgn,_tmp4D5)))),_tmp4D4)))),((_tmp4D6->tl=(
(_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3->hd=(void*)_check_null(fd->effect),((_tmp4D3->tl=0,_tmp4D3)))))),_tmp4D6)))))),_tmp4D2)))),_tmp4D7))))),((_tmp4CF->curr_rgn=(void*)param_rgn,((_tmp4CF->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1316
((_tmp4CF->flags).in_notreadctxt=0,(((_tmp4CF->flags).in_lhs=0,(((_tmp4CF->flags).in_new=0,(((_tmp4CF->flags).abstract_ok=0,_tmp4CF->flags)))))))),((_tmp4CF->fnrgn=(struct _RegionHandle*)r,_tmp4CF)))))))))))))))))))));};};}
# 1321
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp32F;struct Cyc_RgnOrder_RgnPO*_tmp330;struct Cyc_List_List*_tmp331;struct Cyc_Tcenv_SharedFenv*_tmp332;struct _RegionHandle*_tmp333;struct Cyc_Tcenv_Fenv*_tmp32E=old_fenv;_tmp32F=_tmp32E->locals;_tmp330=_tmp32E->region_order;_tmp331=_tmp32E->type_vars;_tmp332=_tmp32E->shared;_tmp333=_tmp32E->fnrgn;{
# 1324
struct _RegionHandle*_tmp334=_tmp332->frgn;
const struct Cyc_Tcenv_Bindings*_tmp335=_tmp32F;
struct _dyneither_ptr*_tmp4E4;const char*_tmp4E3;void*_tmp4E2[1];struct Cyc_String_pa_PrintArg_struct _tmp4E1;struct Cyc_Absyn_Tvar*_tmp4E0;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4E0=_cycalloc(sizeof(*_tmp4E0)),((_tmp4E0->name=((_tmp4E4=_cycalloc(sizeof(*_tmp4E4)),((_tmp4E4[0]=(struct _dyneither_ptr)((_tmp4E1.tag=0,((_tmp4E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4E2[0]=& _tmp4E1,Cyc_aprintf(((_tmp4E3="`%s",_tag_dyneither(_tmp4E3,sizeof(char),4))),_tag_dyneither(_tmp4E2,sizeof(void*),1)))))))),_tmp4E4)))),((_tmp4E0->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4E0->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4E0)))))));
# 1330
{struct Cyc_List_List*_tmp336=fd->tvs;for(0;_tmp336 != 0;_tmp336=_tmp336->tl){
struct Cyc_Absyn_Kind*_stmttmp38=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp336->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp338;enum Cyc_Absyn_AliasQual _tmp339;struct Cyc_Absyn_Kind*_tmp337=_stmttmp38;_tmp338=_tmp337->kind;_tmp339=_tmp337->aliasqual;
if(_tmp338 == Cyc_Absyn_RgnKind){
if(_tmp339 == Cyc_Absyn_Aliasable)
_tmp330=Cyc_RgnOrder_add_youngest(_tmp334,_tmp330,(struct Cyc_Absyn_Tvar*)_tmp336->hd,0,0);else{
# 1336
const char*_tmp4E7;void*_tmp4E6;(_tmp4E6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E7="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4E7,sizeof(char),39))),_tag_dyneither(_tmp4E6,sizeof(void*),0)));}}}}
# 1338
_tmp330=Cyc_RgnOrder_add_youngest(_tmp334,_tmp330,rgn0,0,0);{
struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*_tmp33C=(_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8->hd=rgn0,((_tmp4E8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp331),_tmp4E8)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp33C);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4EB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4EA;void*param_rgn=(void*)((_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA[0]=((_tmp4EB.tag=2,((_tmp4EB.f1=rgn0,_tmp4EB)))),_tmp4EA))));
struct Cyc_List_List*_tmp33D=0;
{struct Cyc_List_List*_tmp33E=fd->args;for(0;_tmp33E != 0;_tmp33E=_tmp33E->tl){
struct Cyc_Absyn_Vardecl _tmp4F1;struct _tuple0*_tmp4F0;struct Cyc_Absyn_Vardecl*_tmp4EF;struct Cyc_Absyn_Vardecl*_tmp33F=(_tmp4EF=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4EF[0]=(struct Cyc_Absyn_Vardecl)((_tmp4F1.sc=Cyc_Absyn_Public,((_tmp4F1.name=(
(_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0->f1=Cyc_Absyn_Loc_n,((_tmp4F0->f2=(*((struct _tuple8*)_tmp33E->hd)).f1,_tmp4F0)))))),((_tmp4F1.tq=(*((struct _tuple8*)_tmp33E->hd)).f2,((_tmp4F1.type=(*((struct _tuple8*)_tmp33E->hd)).f3,((_tmp4F1.initializer=0,((_tmp4F1.rgn=param_rgn,((_tmp4F1.attributes=0,((_tmp4F1.escapes=0,_tmp4F1)))))))))))))))),_tmp4EF)));
# 1351
{struct Cyc_List_List _tmp4F4;struct Cyc_List_List*_tmp4F3;_tmp33D=((_tmp4F3=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4F3[0]=(struct Cyc_List_List)((_tmp4F4.hd=_tmp33F,((_tmp4F4.tl=_tmp33D,_tmp4F4)))),_tmp4F3))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4F7;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4F6;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp342=(_tmp4F6=_cycalloc(sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F7.tag=3,((_tmp4F7.f1=_tmp33F,_tmp4F7)))),_tmp4F6)));
struct _dyneither_ptr*_tmp343=(*((struct _tuple8*)_tmp33E->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4F8;_tmp335=(const struct Cyc_Tcenv_Bindings*)((_tmp4F8=_region_malloc(_tmp334,sizeof(*_tmp4F8)),((_tmp4F8->v=_tmp343,((_tmp4F8->b=(void*)_tmp342,((_tmp4F8->tl=_tmp335,_tmp4F8))))))));};}}
# 1356
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp39=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp34B;struct Cyc_Absyn_Tqual _tmp34C;void*_tmp34D;int _tmp34E;struct Cyc_Absyn_VarargInfo _tmp34A=_stmttmp39;_tmp34B=_tmp34A.name;_tmp34C=_tmp34A.tq;_tmp34D=_tmp34A.type;_tmp34E=_tmp34A.inject;
if(_tmp34B != 0){
void*_tmp34F=Cyc_Absyn_dyneither_typ(_tmp34D,param_rgn,_tmp34C,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4FE;struct _tuple0*_tmp4FD;struct Cyc_Absyn_Vardecl*_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp350=(_tmp4FC=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4FC[0]=(struct Cyc_Absyn_Vardecl)((_tmp4FE.sc=Cyc_Absyn_Public,((_tmp4FE.name=(
(_tmp4FD=_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD->f1=Cyc_Absyn_Loc_n,((_tmp4FD->f2=_tmp34B,_tmp4FD)))))),((_tmp4FE.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4FE.type=_tmp34F,((_tmp4FE.initializer=0,((_tmp4FE.rgn=param_rgn,((_tmp4FE.attributes=0,((_tmp4FE.escapes=0,_tmp4FE)))))))))))))))),_tmp4FC)));
# 1367
{struct Cyc_List_List*_tmp4FF;_tmp33D=((_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF->hd=_tmp350,((_tmp4FF->tl=_tmp33D,_tmp4FF))))));}{
struct _dyneither_ptr*_tmp352=_tmp34B;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp502;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp501;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp353=(_tmp501=_cycalloc(sizeof(*_tmp501)),((_tmp501[0]=((_tmp502.tag=3,((_tmp502.f1=_tmp350,_tmp502)))),_tmp501)));
struct Cyc_Tcenv_Bindings*_tmp503;_tmp335=(const struct Cyc_Tcenv_Bindings*)((_tmp503=_region_malloc(_tmp334,sizeof(*_tmp503)),((_tmp503->v=_tmp352,((_tmp503->b=(void*)_tmp353,((_tmp503->tl=_tmp335,_tmp503))))))));};}else{
# 1372
const char*_tmp506;void*_tmp505;(_tmp505=0,Cyc_Tcutil_terr(loc,((_tmp506="missing name for varargs",_tag_dyneither(_tmp506,sizeof(char),25))),_tag_dyneither(_tmp505,sizeof(void*),0)));}}
# 1374
{struct Cyc_Core_Opt _tmp509;struct Cyc_Core_Opt*_tmp508;fd->param_vardecls=((_tmp508=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp508[0]=(struct Cyc_Core_Opt)((_tmp509.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33D),_tmp509)),_tmp508))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp527;struct Cyc_List_List*_tmp526;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp525;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp524;struct Cyc_List_List*_tmp523;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp522;struct Cyc_Tcenv_CtrlEnv*_tmp521;struct Cyc_Tcenv_SharedFenv*_tmp520;struct Cyc_Tcenv_Fenv*_tmp51F;return(_tmp51F=_region_malloc(_tmp334,sizeof(*_tmp51F)),((_tmp51F->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp520=_region_malloc(_tmp334,sizeof(*_tmp520)),((_tmp520->frgn=_tmp334,((_tmp520->return_typ=fd->ret_type,((_tmp520->seen_labels=
# 1379
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp334,Cyc_strptrcmp),((_tmp520->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp334,Cyc_strptrcmp),((_tmp520->delayed_effect_checks=0,((_tmp520->delayed_constraint_checks=0,_tmp520)))))))))))))),((_tmp51F->type_vars=(struct Cyc_List_List*)_tmp33C,((_tmp51F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp330,((_tmp51F->locals=(const struct Cyc_Tcenv_Bindings*)_tmp335,((_tmp51F->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp51F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1388
(_tmp521=_region_malloc(_tmp334,sizeof(*_tmp521)),((_tmp521->ctrl_rgn=_tmp334,((_tmp521->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp521->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp521->fallthru_clause=0,((_tmp521->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp521->try_depth=0,_tmp521)))))))))))))),((_tmp51F->capability=(void*)((void*)(
(_tmp527=_cycalloc(sizeof(*_tmp527)),((_tmp527[0]=((_tmp522.tag=24,((_tmp522.f1=((_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526->hd=(void*)((_tmp524=_cycalloc(sizeof(*_tmp524)),((_tmp524[0]=((_tmp525.tag=23,((_tmp525.f1=param_rgn,_tmp525)))),_tmp524)))),((_tmp526->tl=(
(_tmp523=_cycalloc(sizeof(*_tmp523)),((_tmp523->hd=(void*)_check_null(fd->effect),((_tmp523->tl=0,_tmp523)))))),_tmp526)))))),_tmp522)))),_tmp527))))),((_tmp51F->curr_rgn=(void*)param_rgn,((_tmp51F->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1392
((_tmp51F->flags).in_notreadctxt=0,(((_tmp51F->flags).in_lhs=0,(((_tmp51F->flags).in_new=0,(((_tmp51F->flags).abstract_ok=0,_tmp51F->flags)))))))),((_tmp51F->fnrgn=(struct _RegionHandle*)_tmp333,_tmp51F)))))))))))))))))))));};};};};}
