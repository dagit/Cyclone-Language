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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 750 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 951
extern void*Cyc_Absyn_empty_effect;
# 953
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 986
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
# 164
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 168
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 224 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 296
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 298
int Cyc_Tcutil_new_tvar_id();
# 300
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10]="Env_error";struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};
# 88 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 90
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
{const char*_tmp37A;void*_tmp379[1];struct Cyc_String_pa_PrintArg_struct _tmp378;(_tmp378.tag=0,((_tmp378.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp379[0]=& _tmp378,Cyc_fprintf(Cyc_stderr,((_tmp37A="Internal error in tcenv: %s\n",_tag_dyneither(_tmp37A,sizeof(char),29))),_tag_dyneither(_tmp379,sizeof(void*),1)))))));}
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
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 222 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 225
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 229
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 234
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp37B;return(_tmp37B=_region_malloc(r,sizeof(*_tmp37B)),((_tmp37B->grgn=r,((_tmp37B->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp37B->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp37B->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp37B->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp37B->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp37B->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp37B->availables=0,_tmp37B)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 245
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 247
{struct Cyc_Core_Opt*_tmp37C;Cyc_Tcutil_empty_var_set=((_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp37C))));}{
# 249
struct Cyc_Tcenv_Genv*_tmpA=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp37D;_tmpA->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmpA->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=Cyc_Absyn_exn_tud,_tmp37D)))));}
{struct Cyc_List_List*_tmpC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmpC != 0;_tmpC=_tmpC->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp383;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp382;struct _tuple12*_tmp381;_tmpA->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmpA->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmpC->hd)->name).f2,(
(_tmp381=_region_malloc(r,sizeof(*_tmp381)),((_tmp381->f1=(void*)(
(_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=((_tmp382.tag=2,((_tmp382.f1=Cyc_Absyn_exn_tud,((_tmp382.f2=(struct Cyc_Absyn_Datatypefield*)_tmpC->hd,_tmp382)))))),_tmp383)))),((_tmp381->f2=1,_tmp381)))))));}}{
# 256
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmpA);
struct Cyc_Tcenv_Tenv*_tmp384;return(_tmp384=_region_malloc(r,sizeof(*_tmp384)),((_tmp384->ns=0,((_tmp384->ae=ae,((_tmp384->le=0,((_tmp384->allow_valueof=0,_tmp384)))))))));};};}
# 262
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 264
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 268
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp385;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp385="outer_namespace",_tag_dyneither(_tmp385,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 274
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 281
if(n2 != 0)return 0;
return 1;}
# 287
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 292
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmp12;_push_handler(& _tmp12);{int _tmp14=0;if(setjmp(_tmp12.handler))_tmp14=1;if(!_tmp14){
lookup(ge2,v,0);{
const char*_tmp389;void*_tmp388[1];struct Cyc_String_pa_PrintArg_struct _tmp387;(_tmp387.tag=0,((_tmp387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp388[0]=& _tmp387,Cyc_Tcutil_terr(loc,((_tmp389="%s is ambiguous",_tag_dyneither(_tmp389,sizeof(char),16))),_tag_dyneither(_tmp388,sizeof(void*),1)))))));};
# 296
;_pop_handler();}else{void*_tmp13=(void*)_exn_thrown;void*_tmp19=_tmp13;void*_tmp1B;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp1A=(struct Cyc_Dict_Absent_exn_struct*)_tmp19;if(_tmp1A->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 298
 goto _LL0;_LL3: _tmp1B=_tmp19;_LL4:(void)_throw(_tmp1B);_LL0:;}};}}
# 300
return;}
# 303
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 308
{struct _handler_cons _tmp1C;_push_handler(& _tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1F=result;_npop_handler(0);return _tmp1F;};}
# 309
;_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;void*_tmp23;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp22=(struct Cyc_Dict_Absent_exn_struct*)_tmp21;if(_tmp22->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 312
 goto _LL5;_LL8: _tmp23=_tmp21;_LL9:(void)_throw(_tmp23);_LL5:;}};}
# 314
{struct Cyc_List_List*_tmp24=ge->availables;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp24->hd);
struct _handler_cons _tmp25;_push_handler(& _tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp24->hd,_tmp24->tl);{
void*_tmp28=result;_npop_handler(0);return _tmp28;};}
# 317
;_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp2A=_tmp26;void*_tmp2C;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp2B=(struct Cyc_Dict_Absent_exn_struct*)_tmp2A;if(_tmp2B->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 320
 goto _LLA;_LLD: _tmp2C=_tmp2A;_LLE:(void)_throw(_tmp2C);_LLA:;}};}}
# 322
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp38C;struct Cyc_Dict_Absent_exn_struct*_tmp38B;(int)_throw((void*)((_tmp38B=_cycalloc_atomic(sizeof(*_tmp38B)),((_tmp38B[0]=((_tmp38C.tag=Cyc_Dict_Absent,_tmp38C)),_tmp38B)))));}
# 325
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 329
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2F=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2F,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 336
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp31;struct _dyneither_ptr*_tmp32;struct _tuple0*_tmp30=q;_tmp31=_tmp30->f1;_tmp32=_tmp30->f2;{
union Cyc_Absyn_Nmspace _tmp33=_tmp31;struct _dyneither_ptr*_tmp34;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp37;_LL10: if((_tmp33.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp33.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL14;_LL13:
# 341
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp32,is_use);_LL14: if((_tmp33.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp33.Rel_n).val == 0)goto _LL16;_tmp34=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp33.Rel_n).val)->hd;_tmp35=((struct Cyc_List_List*)(_tmp33.Rel_n).val)->tl;_LL15: {
# 343
struct Cyc_Tcenv_Genv*_tmp38=Cyc_Tcenv_lookup_namespace(te,loc,_tmp34,_tmp35);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp38,_tmp32,is_use);}_LL16: if((_tmp33.C_n).tag != 3)goto _LL18;_tmp36=(struct Cyc_List_List*)(_tmp33.C_n).val;_LL17:
 _tmp37=_tmp36;goto _LL19;_LL18: if((_tmp33.Abs_n).tag != 2)goto _LLF;_tmp37=(struct Cyc_List_List*)(_tmp33.Abs_n).val;_LL19:
# 347
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp37),_tmp32,is_use);_LLF:;};}
# 352
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 354
struct Cyc_List_List*_tmp39=te->ns;
struct _RegionHandle _tmp3A=_new_region("temp");struct _RegionHandle*temp=& _tmp3A;_push_region(temp);
{struct Cyc_List_List*_tmp3B=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp3C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp39);
struct Cyc_List_List*_tmp3D=_tmp3C->availables;
struct Cyc_Set_Set*_tmp3E=_tmp3C->namespaces;
# 362
{struct Cyc_List_List*_tmp3F=_tmp3D;for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){
struct Cyc_Set_Set*_tmp40=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3F->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp40,n)){
struct Cyc_List_List*_tmp38F;struct Cyc_List_List*_tmp38E;_tmp3B=((_tmp38E=_region_malloc(temp,sizeof(*_tmp38E)),((_tmp38E->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->hd=n,((_tmp38F->tl=ns,_tmp38F))))))),((_tmp38E->tl=_tmp3B,_tmp38E))))));}}}
# 369
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3E,n)){
struct Cyc_List_List*_tmp392;struct Cyc_List_List*_tmp391;_tmp3B=((_tmp391=_region_malloc(temp,sizeof(*_tmp391)),((_tmp391->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp39,((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392->hd=n,((_tmp392->tl=ns,_tmp392))))))),((_tmp391->tl=_tmp3B,_tmp391))))));}
# 375
if(_tmp3B != 0){
if(_tmp3B->tl != 0){
const char*_tmp396;void*_tmp395[1];struct Cyc_String_pa_PrintArg_struct _tmp394;(_tmp394.tag=0,((_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp395[0]=& _tmp394,Cyc_Tcutil_terr(loc,((_tmp396="%s is ambiguous",_tag_dyneither(_tmp396,sizeof(char),16))),_tag_dyneither(_tmp395,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp48=(struct Cyc_List_List*)_tmp3B->hd;_npop_handler(0);return _tmp48;};}
# 380
if(_tmp39 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp399;struct Cyc_Dict_Absent_exn_struct*_tmp398;(int)_throw((void*)((_tmp398=_cycalloc_atomic(sizeof(*_tmp398)),((_tmp398[0]=((_tmp399.tag=Cyc_Dict_Absent,_tmp399)),_tmp398)))));}
# 383
_tmp39=Cyc_Tcenv_outer_namespace(_tmp39);}}
# 356
;_pop_region(temp);}
# 391
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp4B=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp4B,v);}
# 395
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4D;struct _dyneither_ptr*_tmp4E;struct _tuple0*_tmp4C=q;_tmp4D=_tmp4C->f1;_tmp4E=_tmp4C->f2;{
union Cyc_Absyn_Nmspace _tmp4F=_tmp4D;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;struct _dyneither_ptr*_tmp52;struct Cyc_List_List*_tmp53;_LL1B: if((_tmp4F.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4F.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val != 0)goto _LL1F;_LL1E:
# 400
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4E,0);_LL1F: if((_tmp4F.C_n).tag != 3)goto _LL21;_tmp50=(struct Cyc_List_List*)(_tmp4F.C_n).val;_LL20:
 _tmp51=_tmp50;goto _LL22;_LL21: if((_tmp4F.Abs_n).tag != 2)goto _LL23;_tmp51=(struct Cyc_List_List*)(_tmp4F.Abs_n).val;_LL22: {
# 403
struct Cyc_Dict_Dict _tmp54=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp51))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp54,_tmp4E);}_LL23: if((_tmp4F.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4F.Rel_n).val == 0)goto _LL1A;_tmp52=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->hd;_tmp53=((struct Cyc_List_List*)(_tmp4F.Rel_n).val)->tl;_LL24: {
# 406
struct Cyc_Dict_Dict _tmp55=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp52,_tmp53))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp55,_tmp4E);}_LL1A:;};}
# 411
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp56=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp56,v);}
# 415
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp58;struct _dyneither_ptr*_tmp59;struct _tuple0*_tmp57=q;_tmp58=_tmp57->f1;_tmp59=_tmp57->f2;{
union Cyc_Absyn_Nmspace _tmp5A=_tmp58;struct _dyneither_ptr*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct Cyc_List_List*_tmp5E;_LL26: if((_tmp5A.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp5A.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val != 0)goto _LL2A;_LL29:
# 420
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp59,0);_LL2A: if((_tmp5A.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp5A.Rel_n).val == 0)goto _LL2C;_tmp5B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->hd;_tmp5C=((struct Cyc_List_List*)(_tmp5A.Rel_n).val)->tl;_LL2B: {
# 422
struct Cyc_Dict_Dict _tmp5F=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp5B,_tmp5C))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5F,_tmp59);}_LL2C: if((_tmp5A.C_n).tag != 3)goto _LL2E;_tmp5D=(struct Cyc_List_List*)(_tmp5A.C_n).val;_LL2D:
 _tmp5E=_tmp5D;goto _LL2F;_LL2E: if((_tmp5A.Abs_n).tag != 2)goto _LL25;_tmp5E=(struct Cyc_List_List*)(_tmp5A.Abs_n).val;_LL2F: {
# 426
struct Cyc_Dict_Dict _tmp60=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5E))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp60,_tmp59);}_LL25:;};}
# 431
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 436
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp62;struct _dyneither_ptr*_tmp63;struct _tuple0*_tmp61=q;_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;{
union Cyc_Absyn_Nmspace _tmp64=_tmp62;struct _dyneither_ptr*_tmp65;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp67;struct Cyc_List_List*_tmp68;_LL31: if((_tmp64.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp64.Rel_n).val != 0)goto _LL33;_LL32: {
# 441
struct _handler_cons _tmp69;_push_handler(& _tmp69);{int _tmp6B=0;if(setjmp(_tmp69.handler))_tmp6B=1;if(!_tmp6B){{struct Cyc_Absyn_Datatypedecl***_tmp39A;struct Cyc_Absyn_Datatypedecl***_tmp6D=(_tmp39A=_region_malloc(r,sizeof(*_tmp39A)),((_tmp39A[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp63,0),_tmp39A)));_npop_handler(0);return _tmp6D;};_pop_handler();}else{void*_tmp6A=(void*)_exn_thrown;void*_tmp6F=_tmp6A;void*_tmp71;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp70=(struct Cyc_Dict_Absent_exn_struct*)_tmp6F;if(_tmp70->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp71=_tmp6F;_LL3F:(void)_throw(_tmp71);_LL3B:;}};}_LL33: if((_tmp64.Loc_n).tag != 4)goto _LL35;_LL34:
# 444
{const char*_tmp39D;void*_tmp39C;(_tmp39C=0,Cyc_Tcutil_terr(loc,((_tmp39D="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp39D,sizeof(char),33))),_tag_dyneither(_tmp39C,sizeof(void*),0)));}
return 0;_LL35: if((_tmp64.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp64.Rel_n).val == 0)goto _LL37;_tmp65=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp64.Rel_n).val)->hd;_tmp66=((struct Cyc_List_List*)(_tmp64.Rel_n).val)->tl;_LL36: {
# 451
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp74;_push_handler(& _tmp74);{int _tmp76=0;if(setjmp(_tmp74.handler))_tmp76=1;if(!_tmp76){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp65,_tmp66);;_pop_handler();}else{void*_tmp75=(void*)_exn_thrown;void*_tmp78=_tmp75;void*_tmp7A;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 455
{const char*_tmp3A0;void*_tmp39F;(_tmp39F=0,Cyc_Tcutil_terr(loc,((_tmp3A0="bad qualified name for @extensible datatype",_tag_dyneither(_tmp3A0,sizeof(char),44))),_tag_dyneither(_tmp39F,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp3A3;struct Cyc_Dict_Absent_exn_struct*_tmp3A2;(int)_throw((void*)((_tmp3A2=_cycalloc_atomic(sizeof(*_tmp3A2)),((_tmp3A2[0]=((_tmp3A3.tag=Cyc_Dict_Absent,_tmp3A3)),_tmp3A2)))));};_LL43: _tmp7A=_tmp78;_LL44:(void)_throw(_tmp7A);_LL40:;}};}{
# 458
struct Cyc_Dict_Dict _tmp7F=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3A4;return(_tmp3A4=_region_malloc(r,sizeof(*_tmp3A4)),((_tmp3A4[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7F,_tmp63),_tmp3A4)));};}_LL37: if((_tmp64.C_n).tag != 3)goto _LL39;_tmp67=(struct Cyc_List_List*)(_tmp64.C_n).val;_LL38:
 _tmp68=_tmp67;goto _LL3A;_LL39: if((_tmp64.Abs_n).tag != 2)goto _LL30;_tmp68=(struct Cyc_List_List*)(_tmp64.Abs_n).val;_LL3A: {
# 462
struct Cyc_Dict_Dict _tmp81=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp68))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3A5;return(_tmp3A5=_region_malloc(r,sizeof(*_tmp3A5)),((_tmp3A5[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp81,_tmp63),_tmp3A5)));}_LL30:;};}
# 467
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp83=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp83,v);}
# 471
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp85;struct _dyneither_ptr*_tmp86;struct _tuple0*_tmp84=q;_tmp85=_tmp84->f1;_tmp86=_tmp84->f2;{
union Cyc_Absyn_Nmspace _tmp87=_tmp85;struct _dyneither_ptr*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp8B;_LL46: if((_tmp87.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp87.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp87.Rel_n).val != 0)goto _LL4A;_LL49:
# 476
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp86,0);_LL4A: if((_tmp87.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp87.Rel_n).val == 0)goto _LL4C;_tmp88=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp87.Rel_n).val)->hd;_tmp89=((struct Cyc_List_List*)(_tmp87.Rel_n).val)->tl;_LL4B: {
# 478
struct Cyc_Dict_Dict _tmp8C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp88,_tmp89))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8C,_tmp86);}_LL4C: if((_tmp87.C_n).tag != 3)goto _LL4E;_tmp8A=(struct Cyc_List_List*)(_tmp87.C_n).val;_LL4D:
 _tmp8B=_tmp8A;goto _LL4F;_LL4E: if((_tmp87.Abs_n).tag != 2)goto _LL45;_tmp8B=(struct Cyc_List_List*)(_tmp87.Abs_n).val;_LL4F: {
# 482
struct Cyc_Dict_Dict _tmp8D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp8B))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8D,_tmp86);}_LL45:;};}
# 487
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8E=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8E,v);}
# 491
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp90;struct _dyneither_ptr*_tmp91;struct _tuple0*_tmp8F=q;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{
union Cyc_Absyn_Nmspace _tmp92=_tmp90;struct _dyneither_ptr*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_List_List*_tmp95;struct Cyc_List_List*_tmp96;_LL51: if((_tmp92.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp92.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp92.Rel_n).val != 0)goto _LL55;_LL54:
# 496
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp91,0);_LL55: if((_tmp92.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp92.Rel_n).val == 0)goto _LL57;_tmp93=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp92.Rel_n).val)->hd;_tmp94=((struct Cyc_List_List*)(_tmp92.Rel_n).val)->tl;_LL56: {
# 498
struct Cyc_Dict_Dict _tmp97=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp93,_tmp94))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp97,_tmp91);}_LL57: if((_tmp92.C_n).tag != 3)goto _LL59;_tmp95=(struct Cyc_List_List*)(_tmp92.C_n).val;_LL58:
 _tmp96=_tmp95;goto _LL5A;_LL59: if((_tmp92.Abs_n).tag != 2)goto _LL50;_tmp96=(struct Cyc_List_List*)(_tmp92.Abs_n).val;_LL5A: {
# 502
struct Cyc_Dict_Dict _tmp98=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp96))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp98,_tmp91);}_LL50:;};}
# 509
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp99=te->le;
if(te == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp99);}
# 515
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 519
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp9A=te->le;
if(_tmp9A != 0){
struct _RegionHandle*_tmp9C;struct Cyc_Tcenv_Fenv*_tmp9B=_tmp9A;_tmp9C=_tmp9B->fnrgn;
return _tmp9C;}
# 525
return Cyc_Tcenv_coerce_heap_region();}
# 528
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp3A6;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3A6="put_fenv",_tag_dyneither(_tmp3A6,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp3A7;return(_tmp3A7=_region_malloc(l,sizeof(*_tmp3A7)),((_tmp3A7->ns=te->ns,((_tmp3A7->ae=te->ae,((_tmp3A7->le=fe,((_tmp3A7->allow_valueof=te->allow_valueof,_tmp3A7)))))))));};}
# 533
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp3A8;return(_tmp3A8=_region_malloc(l,sizeof(*_tmp3A8)),((_tmp3A8->ns=te->ns,((_tmp3A8->ae=te->ae,((_tmp3A8->le=0,((_tmp3A8->allow_valueof=te->allow_valueof,_tmp3A8)))))))));}
# 537
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 539
struct Cyc_Tcenv_SharedFenv*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_RgnOrder_RgnPO*_tmpA3;const struct Cyc_Tcenv_Bindings*_tmpA4;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Tcenv_CtrlEnv*_tmpA6;void*_tmpA7;void*_tmpA8;int _tmpA9;int _tmpAA;int _tmpAB;int _tmpAC;struct _RegionHandle*_tmpAD;struct Cyc_Tcenv_Fenv _tmpA0=*f;_tmpA1=_tmpA0.shared;_tmpA2=_tmpA0.type_vars;_tmpA3=_tmpA0.region_order;_tmpA4=_tmpA0.locals;_tmpA5=_tmpA0.encloser;_tmpA6=_tmpA0.ctrl_env;_tmpA7=_tmpA0.capability;_tmpA8=_tmpA0.curr_rgn;_tmpA9=_tmpA0.in_notreadctxt;_tmpAA=_tmpA0.in_lhs;_tmpAB=_tmpA0.in_new;_tmpAC=_tmpA0.abstract_ok;_tmpAD=_tmpA0.fnrgn;{
# 541
struct Cyc_Tcenv_Fenv*_tmp3A9;return(_tmp3A9=_region_malloc(l,sizeof(*_tmp3A9)),((_tmp3A9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA1,((_tmp3A9->type_vars=(struct Cyc_List_List*)_tmpA2,((_tmp3A9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA3,((_tmp3A9->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA4,((_tmp3A9->encloser=(struct Cyc_Absyn_Stmt*)_tmpA5,((_tmp3A9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA6,((_tmp3A9->capability=(void*)_tmpA7,((_tmp3A9->curr_rgn=(void*)_tmpA8,((_tmp3A9->in_notreadctxt=(int)_tmpA9,((_tmp3A9->in_lhs=(int)_tmpAA,((_tmp3A9->in_new=(int)_tmpAB,((_tmp3A9->abstract_ok=(int)_tmpAC,((_tmp3A9->fnrgn=(struct _RegionHandle*)l,_tmp3A9)))))))))))))))))))))))))));};}
# 546
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 548
struct Cyc_Tcenv_SharedFenv*_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_RgnOrder_RgnPO*_tmpB2;const struct Cyc_Tcenv_Bindings*_tmpB3;struct Cyc_Absyn_Stmt*_tmpB4;struct Cyc_Tcenv_CtrlEnv*_tmpB5;void*_tmpB6;void*_tmpB7;int _tmpB8;int _tmpB9;int _tmpBA;int _tmpBB;struct Cyc_Tcenv_Fenv _tmpAF=*f;_tmpB0=_tmpAF.shared;_tmpB1=_tmpAF.type_vars;_tmpB2=_tmpAF.region_order;_tmpB3=_tmpAF.locals;_tmpB4=_tmpAF.encloser;_tmpB5=_tmpAF.ctrl_env;_tmpB6=_tmpAF.capability;_tmpB7=_tmpAF.curr_rgn;_tmpB8=_tmpAF.in_notreadctxt;_tmpB9=_tmpAF.in_lhs;_tmpBA=_tmpAF.in_new;_tmpBB=_tmpAF.abstract_ok;{
# 551
struct _RegionHandle*_tmpBD;void*_tmpBE;void*_tmpBF;const struct _tuple10*_tmpC0;void*_tmpC1;int _tmpC2;struct Cyc_Tcenv_CtrlEnv _tmpBC=*_tmpB5;_tmpBD=_tmpBC.ctrl_rgn;_tmpBE=_tmpBC.continue_stmt;_tmpBF=_tmpBC.break_stmt;_tmpC0=_tmpBC.fallthru_clause;_tmpC1=_tmpBC.next_stmt;_tmpC2=_tmpBC.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp3AA;struct Cyc_Tcenv_CtrlEnv*_tmpC3=
(_tmp3AA=_region_malloc(l,sizeof(*_tmp3AA)),((_tmp3AA->ctrl_rgn=_tmpBD,((_tmp3AA->continue_stmt=_tmpBE,((_tmp3AA->break_stmt=_tmpBF,((_tmp3AA->fallthru_clause=_tmpC0,((_tmp3AA->next_stmt=_tmpC1,((_tmp3AA->try_depth=_tmpC2,_tmp3AA)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp3AB;return(_tmp3AB=_region_malloc(l,sizeof(*_tmp3AB)),((_tmp3AB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpB0,((_tmp3AB->type_vars=(struct Cyc_List_List*)_tmpB1,((_tmp3AB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpB2,((_tmp3AB->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB3,((_tmp3AB->encloser=(struct Cyc_Absyn_Stmt*)_tmpB4,((_tmp3AB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpC3,((_tmp3AB->capability=(void*)_tmpB6,((_tmp3AB->curr_rgn=(void*)_tmpB7,((_tmp3AB->in_notreadctxt=(int)_tmpB8,((_tmp3AB->in_lhs=(int)_tmpB9,((_tmp3AB->in_new=(int)_tmpBA,((_tmp3AB->abstract_ok=(int)_tmpBB,((_tmp3AB->fnrgn=(struct _RegionHandle*)l,_tmp3AB)))))))))))))))))))))))))));};};}
# 561
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmpC8;const char*_tmp3AC;struct Cyc_Tcenv_Fenv*_tmpC7=Cyc_Tcenv_get_fenv(te,((_tmp3AC="return_typ",_tag_dyneither(_tmp3AC,sizeof(char),11))));_tmpC8=_tmpC7->shared;{
void*_tmpCA;struct Cyc_Tcenv_SharedFenv _tmpC9=*_tmpC8;_tmpCA=_tmpC9.return_typ;
return _tmpCA;};}
# 567
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpCB=te->le;
if(te->le == 0)return 0;{
struct Cyc_List_List*_tmpCD;struct Cyc_Tcenv_Fenv _tmpCC=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpCB));_tmpCD=_tmpCC.type_vars;
return _tmpCD;};}
# 574
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp3AD;return(_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3AD)));}
# 578
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 580
struct Cyc_Tcenv_SharedFenv*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_RgnOrder_RgnPO*_tmpD3;const struct Cyc_Tcenv_Bindings*_tmpD4;struct Cyc_Absyn_Stmt*_tmpD5;struct Cyc_Tcenv_CtrlEnv*_tmpD6;void*_tmpD7;void*_tmpD8;int _tmpD9;int _tmpDA;int _tmpDB;int _tmpDC;const char*_tmp3AE;struct Cyc_Tcenv_Fenv*_tmpD0=
# 582
Cyc_Tcenv_get_fenv(te,((_tmp3AE="add_type_vars",_tag_dyneither(_tmp3AE,sizeof(char),14))));_tmpD1=_tmpD0->shared;_tmpD2=_tmpD0->type_vars;_tmpD3=_tmpD0->region_order;_tmpD4=_tmpD0->locals;_tmpD5=_tmpD0->encloser;_tmpD6=_tmpD0->ctrl_env;_tmpD7=_tmpD0->capability;_tmpD8=_tmpD0->curr_rgn;_tmpD9=_tmpD0->in_notreadctxt;_tmpDA=_tmpD0->in_lhs;_tmpDB=_tmpD0->in_new;_tmpDC=_tmpD0->abstract_ok;
# 584
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD2);
Cyc_Tcutil_check_unique_tvars(loc,_tmpDD);{
struct Cyc_Tcenv_Fenv*_tmp3AF;struct Cyc_Tcenv_Fenv*_tmpDE=(_tmp3AF=_region_malloc(r,sizeof(*_tmp3AF)),((_tmp3AF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpD1,((_tmp3AF->type_vars=(struct Cyc_List_List*)_tmpDD,((_tmp3AF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD3,((_tmp3AF->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD4,((_tmp3AF->encloser=(struct Cyc_Absyn_Stmt*)_tmpD5,((_tmp3AF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD6,((_tmp3AF->capability=(void*)_tmpD7,((_tmp3AF->curr_rgn=(void*)_tmpD8,((_tmp3AF->in_notreadctxt=(int)_tmpD9,((_tmp3AF->in_lhs=(int)_tmpDA,((_tmp3AF->in_new=(int)_tmpDB,((_tmp3AF->abstract_ok=(int)_tmpDC,((_tmp3AF->fnrgn=(struct _RegionHandle*)r,_tmp3AF)))))))))))))))))))))))))));
# 590
return Cyc_Tcenv_put_fenv(r,te,_tmpDE);};};}
# 593
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpE0=te->le;
if(_tmpE0 == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 598
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpE0);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 603
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 605
struct _dyneither_ptr*_tmpE1=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpE2=(*vd->name).f1;_LL5C: if((_tmpE2.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 610
struct Cyc_Core_Impossible_exn_struct _tmp3B5;const char*_tmp3B4;struct Cyc_Core_Impossible_exn_struct*_tmp3B3;(int)_throw((void*)((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3[0]=((_tmp3B5.tag=Cyc_Core_Impossible,((_tmp3B5.f1=((_tmp3B4="add_local_var: called with Rel_n",_tag_dyneither(_tmp3B4,sizeof(char),33))),_tmp3B5)))),_tmp3B3)))));}_LL5B:;}{
# 612
struct Cyc_Tcenv_SharedFenv*_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_RgnOrder_RgnPO*_tmpEA;const struct Cyc_Tcenv_Bindings*_tmpEB;struct Cyc_Absyn_Stmt*_tmpEC;struct Cyc_Tcenv_CtrlEnv*_tmpED;void*_tmpEE;void*_tmpEF;int _tmpF0;int _tmpF1;int _tmpF2;int _tmpF3;const char*_tmp3B6;struct Cyc_Tcenv_Fenv*_tmpE7=
# 614
Cyc_Tcenv_get_fenv(te,((_tmp3B6="add_local_var",_tag_dyneither(_tmp3B6,sizeof(char),14))));_tmpE8=_tmpE7->shared;_tmpE9=_tmpE7->type_vars;_tmpEA=_tmpE7->region_order;_tmpEB=_tmpE7->locals;_tmpEC=_tmpE7->encloser;_tmpED=_tmpE7->ctrl_env;_tmpEE=_tmpE7->capability;_tmpEF=_tmpE7->curr_rgn;_tmpF0=_tmpE7->in_notreadctxt;_tmpF1=_tmpE7->in_lhs;_tmpF2=_tmpE7->in_new;_tmpF3=_tmpE7->abstract_ok;{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp3B9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3B8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpF4=(_tmp3B8=_cycalloc(sizeof(*_tmp3B8)),((_tmp3B8[0]=((_tmp3B9.tag=4,((_tmp3B9.f1=vd,_tmp3B9)))),_tmp3B8)));
struct Cyc_Tcenv_Bindings*_tmp3BA;struct Cyc_Tcenv_Bindings*_tmpF5=(_tmp3BA=_region_malloc(r,sizeof(*_tmp3BA)),((_tmp3BA->v=_tmpE1,((_tmp3BA->b=(void*)_tmpF4,((_tmp3BA->tl=_tmpEB,_tmp3BA)))))));
struct Cyc_Tcenv_Fenv*_tmp3BB;struct Cyc_Tcenv_Fenv*_tmpF6=(_tmp3BB=_region_malloc(r,sizeof(*_tmp3BB)),((_tmp3BB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpE8,((_tmp3BB->type_vars=(struct Cyc_List_List*)_tmpE9,((_tmp3BB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpEA,((_tmp3BB->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpF5),((_tmp3BB->encloser=(struct Cyc_Absyn_Stmt*)_tmpEC,((_tmp3BB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpED,((_tmp3BB->capability=(void*)_tmpEE,((_tmp3BB->curr_rgn=(void*)_tmpEF,((_tmp3BB->in_notreadctxt=(int)_tmpF0,((_tmp3BB->in_lhs=(int)_tmpF1,((_tmp3BB->in_new=(int)_tmpF2,((_tmp3BB->abstract_ok=(int)_tmpF3,((_tmp3BB->fnrgn=(struct _RegionHandle*)r,_tmp3BB)))))))))))))))))))))))))));
# 620
return Cyc_Tcenv_put_fenv(r,te,_tmpF6);};};}
# 623
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 625
struct Cyc_Tcenv_Fenv*_tmpFB=te->le;
if(_tmpFB == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_RgnOrder_RgnPO*_tmpFF;const struct Cyc_Tcenv_Bindings*_tmp100;struct Cyc_Absyn_Stmt*_tmp101;struct Cyc_Tcenv_CtrlEnv*_tmp102;void*_tmp103;void*_tmp104;int _tmp105;int _tmp106;int _tmp107;struct Cyc_Tcenv_Fenv _tmpFC=*_tmpFB;_tmpFD=_tmpFC.shared;_tmpFE=_tmpFC.type_vars;_tmpFF=_tmpFC.region_order;_tmp100=_tmpFC.locals;_tmp101=_tmpFC.encloser;_tmp102=_tmpFC.ctrl_env;_tmp103=_tmpFC.capability;_tmp104=_tmpFC.curr_rgn;_tmp105=_tmpFC.in_notreadctxt;_tmp106=_tmpFC.in_lhs;_tmp107=_tmpFC.abstract_ok;{
# 629
struct Cyc_Tcenv_Fenv*_tmp3BC;struct Cyc_Tcenv_Fenv*_tmp108=(_tmp3BC=_region_malloc(r,sizeof(*_tmp3BC)),((_tmp3BC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFD,((_tmp3BC->type_vars=(struct Cyc_List_List*)_tmpFE,((_tmp3BC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFF,((_tmp3BC->locals=(const struct Cyc_Tcenv_Bindings*)_tmp100,((_tmp3BC->encloser=(struct Cyc_Absyn_Stmt*)_tmp101,((_tmp3BC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp102,((_tmp3BC->capability=(void*)_tmp103,((_tmp3BC->curr_rgn=(void*)_tmp104,((_tmp3BC->in_notreadctxt=(int)_tmp105,((_tmp3BC->in_lhs=(int)_tmp106,((_tmp3BC->in_new=(int)((int)status),((_tmp3BC->abstract_ok=(int)_tmp107,((_tmp3BC->fnrgn=(struct _RegionHandle*)r,_tmp3BC)))))))))))))))))))))))))));
# 632
return Cyc_Tcenv_put_fenv(r,te,_tmp108);};};}
# 635
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10A=te->le;
if(_tmp10A == 0)return Cyc_Tcenv_NoneNew;{
int _tmp10C;struct Cyc_Tcenv_Fenv*_tmp10B=_tmp10A;_tmp10C=_tmp10B->in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp10C;};}
# 642
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp10D=te->le;
if(_tmp10D == 0)return 0;{
int _tmp10F;struct Cyc_Tcenv_Fenv*_tmp10E=_tmp10D;_tmp10F=_tmp10E->abstract_ok;
return _tmp10F;};}
# 649
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 651
struct Cyc_Tcenv_Fenv*_tmp110=te->le;
if(_tmp110 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp114;const struct Cyc_Tcenv_Bindings*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp118;void*_tmp119;int _tmp11A;int _tmp11B;int _tmp11C;int _tmp11D;struct Cyc_Tcenv_Fenv _tmp111=*_tmp110;_tmp112=_tmp111.shared;_tmp113=_tmp111.type_vars;_tmp114=_tmp111.region_order;_tmp115=_tmp111.locals;_tmp116=_tmp111.encloser;_tmp117=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp119=_tmp111.curr_rgn;_tmp11A=_tmp111.in_notreadctxt;_tmp11B=_tmp111.in_lhs;_tmp11C=_tmp111.in_new;_tmp11D=_tmp111.abstract_ok;{
# 655
struct Cyc_Tcenv_Fenv*_tmp3BD;struct Cyc_Tcenv_Fenv*_tmp11E=(_tmp3BD=_region_malloc(r,sizeof(*_tmp3BD)),((_tmp3BD->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112,((_tmp3BD->type_vars=(struct Cyc_List_List*)_tmp113,((_tmp3BD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp114,((_tmp3BD->locals=(const struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp3BD->encloser=(struct Cyc_Absyn_Stmt*)_tmp116,((_tmp3BD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117,((_tmp3BD->capability=(void*)_tmp118,((_tmp3BD->curr_rgn=(void*)_tmp119,((_tmp3BD->in_notreadctxt=(int)_tmp11A,((_tmp3BD->in_lhs=(int)_tmp11B,((_tmp3BD->in_new=(int)_tmp11C,((_tmp3BD->abstract_ok=(int)1,((_tmp3BD->fnrgn=(struct _RegionHandle*)r,_tmp3BD)))))))))))))))))))))))))));
# 658
return Cyc_Tcenv_put_fenv(r,te,_tmp11E);};};}
# 661
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 663
struct Cyc_Tcenv_Fenv*_tmp120=te->le;
if(_tmp120 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_RgnOrder_RgnPO*_tmp124;const struct Cyc_Tcenv_Bindings*_tmp125;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Tcenv_CtrlEnv*_tmp127;void*_tmp128;void*_tmp129;int _tmp12A;int _tmp12B;int _tmp12C;int _tmp12D;struct Cyc_Tcenv_Fenv _tmp121=*_tmp120;_tmp122=_tmp121.shared;_tmp123=_tmp121.type_vars;_tmp124=_tmp121.region_order;_tmp125=_tmp121.locals;_tmp126=_tmp121.encloser;_tmp127=_tmp121.ctrl_env;_tmp128=_tmp121.capability;_tmp129=_tmp121.curr_rgn;_tmp12A=_tmp121.in_notreadctxt;_tmp12B=_tmp121.in_lhs;_tmp12C=_tmp121.in_new;_tmp12D=_tmp121.abstract_ok;{
# 667
struct Cyc_Tcenv_Fenv*_tmp3BE;struct Cyc_Tcenv_Fenv*_tmp12E=(_tmp3BE=_region_malloc(r,sizeof(*_tmp3BE)),((_tmp3BE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp122,((_tmp3BE->type_vars=(struct Cyc_List_List*)_tmp123,((_tmp3BE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp124,((_tmp3BE->locals=(const struct Cyc_Tcenv_Bindings*)_tmp125,((_tmp3BE->encloser=(struct Cyc_Absyn_Stmt*)_tmp126,((_tmp3BE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp127,((_tmp3BE->capability=(void*)_tmp128,((_tmp3BE->curr_rgn=(void*)_tmp129,((_tmp3BE->in_notreadctxt=(int)_tmp12A,((_tmp3BE->in_lhs=(int)_tmp12B,((_tmp3BE->in_new=(int)_tmp12C,((_tmp3BE->abstract_ok=(int)0,((_tmp3BE->fnrgn=(struct _RegionHandle*)r,_tmp3BE)))))))))))))))))))))))))));
# 670
return Cyc_Tcenv_put_fenv(r,te,_tmp12E);};};}
# 673
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp130=te->le;
if(_tmp130 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_RgnOrder_RgnPO*_tmp134;const struct Cyc_Tcenv_Bindings*_tmp135;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Tcenv_CtrlEnv*_tmp137;void*_tmp138;void*_tmp139;int _tmp13A;int _tmp13B;int _tmp13C;int _tmp13D;struct Cyc_Tcenv_Fenv _tmp131=*_tmp130;_tmp132=_tmp131.shared;_tmp133=_tmp131.type_vars;_tmp134=_tmp131.region_order;_tmp135=_tmp131.locals;_tmp136=_tmp131.encloser;_tmp137=_tmp131.ctrl_env;_tmp138=_tmp131.capability;_tmp139=_tmp131.curr_rgn;_tmp13A=_tmp131.in_notreadctxt;_tmp13B=_tmp131.in_lhs;_tmp13C=_tmp131.in_new;_tmp13D=_tmp131.abstract_ok;{
# 678
struct Cyc_Tcenv_Fenv*_tmp3BF;struct Cyc_Tcenv_Fenv*_tmp13E=(_tmp3BF=_region_malloc(r,sizeof(*_tmp3BF)),((_tmp3BF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp132,((_tmp3BF->type_vars=(struct Cyc_List_List*)_tmp133,((_tmp3BF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp134,((_tmp3BF->locals=(const struct Cyc_Tcenv_Bindings*)_tmp135,((_tmp3BF->encloser=(struct Cyc_Absyn_Stmt*)_tmp136,((_tmp3BF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp137,((_tmp3BF->capability=(void*)_tmp138,((_tmp3BF->curr_rgn=(void*)_tmp139,((_tmp3BF->in_notreadctxt=(int)1,((_tmp3BF->in_lhs=(int)_tmp13B,((_tmp3BF->in_new=(int)_tmp13C,((_tmp3BF->abstract_ok=(int)_tmp13D,((_tmp3BF->fnrgn=(struct _RegionHandle*)r,_tmp3BF)))))))))))))))))))))))))));
# 681
return Cyc_Tcenv_put_fenv(r,te,_tmp13E);};};}
# 684
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp140=te->le;
if(_tmp140 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_RgnOrder_RgnPO*_tmp144;const struct Cyc_Tcenv_Bindings*_tmp145;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Tcenv_CtrlEnv*_tmp147;void*_tmp148;void*_tmp149;int _tmp14A;int _tmp14B;int _tmp14C;int _tmp14D;struct Cyc_Tcenv_Fenv _tmp141=*_tmp140;_tmp142=_tmp141.shared;_tmp143=_tmp141.type_vars;_tmp144=_tmp141.region_order;_tmp145=_tmp141.locals;_tmp146=_tmp141.encloser;_tmp147=_tmp141.ctrl_env;_tmp148=_tmp141.capability;_tmp149=_tmp141.curr_rgn;_tmp14A=_tmp141.in_notreadctxt;_tmp14B=_tmp141.in_lhs;_tmp14C=_tmp141.in_new;_tmp14D=_tmp141.abstract_ok;{
# 689
struct Cyc_Tcenv_Fenv*_tmp3C0;struct Cyc_Tcenv_Fenv*_tmp14E=(_tmp3C0=_region_malloc(r,sizeof(*_tmp3C0)),((_tmp3C0->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp142,((_tmp3C0->type_vars=(struct Cyc_List_List*)_tmp143,((_tmp3C0->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp144,((_tmp3C0->locals=(const struct Cyc_Tcenv_Bindings*)_tmp145,((_tmp3C0->encloser=(struct Cyc_Absyn_Stmt*)_tmp146,((_tmp3C0->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp147,((_tmp3C0->capability=(void*)_tmp148,((_tmp3C0->curr_rgn=(void*)_tmp149,((_tmp3C0->in_notreadctxt=(int)0,((_tmp3C0->in_lhs=(int)_tmp14B,((_tmp3C0->in_new=(int)_tmp14C,((_tmp3C0->abstract_ok=(int)_tmp14D,((_tmp3C0->fnrgn=(struct _RegionHandle*)r,_tmp3C0)))))))))))))))))))))))))));
# 692
return Cyc_Tcenv_put_fenv(r,te,_tmp14E);};};}
# 695
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp150=te->le;
if(_tmp150 == 0)return 0;{
int _tmp152;struct Cyc_Tcenv_Fenv*_tmp151=_tmp150;_tmp152=_tmp151->in_notreadctxt;
return _tmp152;};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp153=te->le;
if(_tmp153 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp155;struct Cyc_List_List*_tmp156;struct Cyc_RgnOrder_RgnPO*_tmp157;const struct Cyc_Tcenv_Bindings*_tmp158;struct Cyc_Absyn_Stmt*_tmp159;struct Cyc_Tcenv_CtrlEnv*_tmp15A;void*_tmp15B;void*_tmp15C;int _tmp15D;int _tmp15E;int _tmp15F;int _tmp160;struct Cyc_Tcenv_Fenv _tmp154=*_tmp153;_tmp155=_tmp154.shared;_tmp156=_tmp154.type_vars;_tmp157=_tmp154.region_order;_tmp158=_tmp154.locals;_tmp159=_tmp154.encloser;_tmp15A=_tmp154.ctrl_env;_tmp15B=_tmp154.capability;_tmp15C=_tmp154.curr_rgn;_tmp15D=_tmp154.in_notreadctxt;_tmp15E=_tmp154.in_lhs;_tmp15F=_tmp154.in_new;_tmp160=_tmp154.abstract_ok;{
# 707
struct Cyc_Tcenv_Fenv*_tmp3C1;struct Cyc_Tcenv_Fenv*_tmp161=(_tmp3C1=_region_malloc(r,sizeof(*_tmp3C1)),((_tmp3C1->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp155,((_tmp3C1->type_vars=(struct Cyc_List_List*)_tmp156,((_tmp3C1->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp157,((_tmp3C1->locals=(const struct Cyc_Tcenv_Bindings*)_tmp158,((_tmp3C1->encloser=(struct Cyc_Absyn_Stmt*)_tmp159,((_tmp3C1->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15A,((_tmp3C1->capability=(void*)_tmp15B,((_tmp3C1->curr_rgn=(void*)_tmp15C,((_tmp3C1->in_notreadctxt=(int)_tmp15D,((_tmp3C1->in_lhs=(int)1,((_tmp3C1->in_new=(int)_tmp15F,((_tmp3C1->abstract_ok=(int)_tmp160,((_tmp3C1->fnrgn=(struct _RegionHandle*)r,_tmp3C1)))))))))))))))))))))))))));
# 710
return Cyc_Tcenv_put_fenv(r,te,_tmp161);};};}
# 713
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp163=te->le;
if(_tmp163 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_SharedFenv*_tmp165;struct Cyc_List_List*_tmp166;struct Cyc_RgnOrder_RgnPO*_tmp167;const struct Cyc_Tcenv_Bindings*_tmp168;struct Cyc_Absyn_Stmt*_tmp169;struct Cyc_Tcenv_CtrlEnv*_tmp16A;void*_tmp16B;void*_tmp16C;int _tmp16D;int _tmp16E;int _tmp16F;int _tmp170;struct Cyc_Tcenv_Fenv _tmp164=*_tmp163;_tmp165=_tmp164.shared;_tmp166=_tmp164.type_vars;_tmp167=_tmp164.region_order;_tmp168=_tmp164.locals;_tmp169=_tmp164.encloser;_tmp16A=_tmp164.ctrl_env;_tmp16B=_tmp164.capability;_tmp16C=_tmp164.curr_rgn;_tmp16D=_tmp164.in_notreadctxt;_tmp16E=_tmp164.in_lhs;_tmp16F=_tmp164.in_new;_tmp170=_tmp164.abstract_ok;{
# 718
struct Cyc_Tcenv_Fenv*_tmp3C2;struct Cyc_Tcenv_Fenv*_tmp171=(_tmp3C2=_region_malloc(r,sizeof(*_tmp3C2)),((_tmp3C2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp165,((_tmp3C2->type_vars=(struct Cyc_List_List*)_tmp166,((_tmp3C2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp167,((_tmp3C2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp168,((_tmp3C2->encloser=(struct Cyc_Absyn_Stmt*)_tmp169,((_tmp3C2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16A,((_tmp3C2->capability=(void*)_tmp16B,((_tmp3C2->curr_rgn=(void*)_tmp16C,((_tmp3C2->in_notreadctxt=(int)_tmp16D,((_tmp3C2->in_lhs=(int)0,((_tmp3C2->in_new=(int)_tmp16F,((_tmp3C2->abstract_ok=(int)_tmp170,((_tmp3C2->fnrgn=(struct _RegionHandle*)r,_tmp3C2)))))))))))))))))))))))))));
# 721
return Cyc_Tcenv_put_fenv(r,te,_tmp171);};};}
# 724
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp173=te->le;
if(_tmp173 == 0)return 0;{
int _tmp175;struct Cyc_Tcenv_Fenv*_tmp174=_tmp173;_tmp175=_tmp174->in_lhs;
return _tmp175;};}
# 732
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 734
struct _dyneither_ptr*_tmp176=(*vd->name).f2;
struct Cyc_Tcenv_SharedFenv*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_RgnOrder_RgnPO*_tmp17B;const struct Cyc_Tcenv_Bindings*_tmp17C;struct Cyc_Absyn_Stmt*_tmp17D;struct Cyc_Tcenv_CtrlEnv*_tmp17E;void*_tmp17F;void*_tmp180;int _tmp181;int _tmp182;int _tmp183;int _tmp184;const char*_tmp3C3;struct Cyc_Tcenv_Fenv*_tmp178=
# 737
Cyc_Tcenv_get_fenv(te,((_tmp3C3="add_pat_var",_tag_dyneither(_tmp3C3,sizeof(char),12))));_tmp179=_tmp178->shared;_tmp17A=_tmp178->type_vars;_tmp17B=_tmp178->region_order;_tmp17C=_tmp178->locals;_tmp17D=_tmp178->encloser;_tmp17E=_tmp178->ctrl_env;_tmp17F=_tmp178->capability;_tmp180=_tmp178->curr_rgn;_tmp181=_tmp178->in_notreadctxt;_tmp182=_tmp178->in_lhs;_tmp183=_tmp178->in_new;_tmp184=_tmp178->abstract_ok;{
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp3C6;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3C5;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp185=(_tmp3C5=_cycalloc(sizeof(*_tmp3C5)),((_tmp3C5[0]=((_tmp3C6.tag=5,((_tmp3C6.f1=vd,_tmp3C6)))),_tmp3C5)));
struct Cyc_Tcenv_Bindings*_tmp3C7;struct Cyc_Tcenv_Bindings*_tmp186=(_tmp3C7=_region_malloc(r,sizeof(*_tmp3C7)),((_tmp3C7->v=_tmp176,((_tmp3C7->b=(void*)_tmp185,((_tmp3C7->tl=_tmp17C,_tmp3C7)))))));
struct Cyc_Tcenv_Fenv*_tmp3C8;struct Cyc_Tcenv_Fenv*_tmp187=(_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp179,((_tmp3C8->type_vars=(struct Cyc_List_List*)_tmp17A,((_tmp3C8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp17B,((_tmp3C8->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp186),((_tmp3C8->encloser=(struct Cyc_Absyn_Stmt*)_tmp17D,((_tmp3C8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp17E,((_tmp3C8->capability=(void*)_tmp17F,((_tmp3C8->curr_rgn=(void*)_tmp180,((_tmp3C8->in_notreadctxt=(int)_tmp181,((_tmp3C8->in_lhs=(int)_tmp182,((_tmp3C8->in_new=(int)_tmp183,((_tmp3C8->abstract_ok=(int)_tmp184,((_tmp3C8->fnrgn=(struct _RegionHandle*)r,_tmp3C8)))))))))))))))))))))))))));
# 743
return Cyc_Tcenv_put_fenv(r,te,_tmp187);};}
# 746
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp18C=te->le;
union Cyc_Absyn_Nmspace _tmp18E;struct _dyneither_ptr*_tmp18F;struct _tuple0*_tmp18D=q;_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;{
union Cyc_Absyn_Nmspace _tmp190=_tmp18E;_LL61: if((_tmp190.Loc_n).tag != 4)goto _LL63;_LL62:
# 751
 if(_tmp18C == 0){struct Cyc_Dict_Absent_exn_struct _tmp3CB;struct Cyc_Dict_Absent_exn_struct*_tmp3CA;(int)_throw((void*)((_tmp3CA=_cycalloc_atomic(sizeof(*_tmp3CA)),((_tmp3CA[0]=((_tmp3CB.tag=Cyc_Dict_Absent,_tmp3CB)),_tmp3CA)))));}
goto _LL64;_LL63: if((_tmp190.Rel_n).tag != 1)goto _LL65;if((struct Cyc_List_List*)(_tmp190.Rel_n).val != 0)goto _LL65;if(!(_tmp18C != 0))goto _LL65;_LL64: {
# 754
const struct Cyc_Tcenv_Bindings*_tmp194;struct Cyc_Tcenv_Fenv*_tmp193=_tmp18C;_tmp194=_tmp193->locals;{
# 756
struct _handler_cons _tmp195;_push_handler(& _tmp195);{int _tmp197=0;if(setjmp(_tmp195.handler))_tmp197=1;if(!_tmp197){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3CE;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3CD;void*_tmp19A=(void*)((_tmp3CD=_region_malloc(r,sizeof(*_tmp3CD)),((_tmp3CD[0]=((_tmp3CE.tag=0,((_tmp3CE.f1=Cyc_Tcenv_lookup_binding(_tmp194,_tmp18F),_tmp3CE)))),_tmp3CD))));_npop_handler(0);return _tmp19A;};_pop_handler();}else{void*_tmp196=(void*)_exn_thrown;void*_tmp19C=_tmp196;void*_tmp19E;_LL68: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp19D=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp19C;if(_tmp19D->tag != Cyc_Tcenv_NoBinding)goto _LL6A;}_LL69:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL6A: _tmp19E=_tmp19C;_LL6B:(void)_throw(_tmp19E);_LL67:;}};};}_LL65:;_LL66: {
# 760
struct _handler_cons _tmp19F;_push_handler(& _tmp19F);{int _tmp1A1=0;if(setjmp(_tmp19F.handler))_tmp1A1=1;if(!_tmp1A1){{void*_tmp1A2=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1A2;};_pop_handler();}else{void*_tmp1A0=(void*)_exn_thrown;void*_tmp1A4=_tmp1A0;void*_tmp1A6;_LL6D: {struct Cyc_Dict_Absent_exn_struct*_tmp1A5=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A4;if(_tmp1A5->tag != Cyc_Dict_Absent)goto _LL6F;}_LL6E: {
# 764
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3D1;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3D0;return(void*)((_tmp3D0=_region_malloc(r,sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D1.tag=0,((_tmp3D1.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp3D1)))),_tmp3D0))));}_LL6F: _tmp1A6=_tmp1A4;_LL70:(void)_throw(_tmp1A6);_LL6C:;}};}_LL60:;};}
# 769
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp1AB;const char*_tmp3D2;struct Cyc_Tcenv_Fenv*_tmp1AA=
Cyc_Tcenv_get_fenv(te,((_tmp3D2="process_continue",_tag_dyneither(_tmp3D2,sizeof(char),17))));_tmp1AB=_tmp1AA->ctrl_env;{
void*_tmp1AC=_tmp1AB->continue_stmt;struct Cyc_Absyn_Stmt*_tmp1AE;_LL72: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1AD=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1AC;if(_tmp1AD->tag != 3)goto _LL74;else{_tmp1AE=_tmp1AD->f1;}}_LL73:
# 774
{struct Cyc_List_List*_tmp3D3;_tmp1AE->non_local_preds=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->hd=s,((_tmp3D3->tl=_tmp1AE->non_local_preds,_tmp3D3))))));}
*sopt=_tmp1AE;
return;_LL74: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1AF=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1AC;if(_tmp1AF->tag != 0)goto _LL76;}_LL75:
{const char*_tmp3D6;void*_tmp3D5;(_tmp3D5=0,Cyc_Tcutil_terr(s->loc,((_tmp3D6="continue not in a loop",_tag_dyneither(_tmp3D6,sizeof(char),23))),_tag_dyneither(_tmp3D5,sizeof(void*),0)));}return;_LL76: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1B0=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1AC;if(_tmp1B0->tag != 1)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1B1=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1AC;if(_tmp1B1->tag != 2)goto _LL71;}_LL79: {
const char*_tmp3D7;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D7="bad continue destination",_tag_dyneither(_tmp3D7,sizeof(char),25))));}_LL71:;};}
# 782
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_CtrlEnv*_tmp1B8;struct Cyc_Tcenv_SharedFenv*_tmp1B9;const char*_tmp3D8;struct Cyc_Tcenv_Fenv*_tmp1B7=
Cyc_Tcenv_get_fenv(te,((_tmp3D8="process_break",_tag_dyneither(_tmp3D8,sizeof(char),14))));_tmp1B8=_tmp1B7->ctrl_env;_tmp1B9=_tmp1B7->shared;{
void*_tmp1BA=_tmp1B8->break_stmt;struct Cyc_Absyn_Stmt*_tmp1BC;_LL7B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1BB=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1BA;if(_tmp1BB->tag != 3)goto _LL7D;else{_tmp1BC=_tmp1BB->f1;}}_LL7C:
# 787
{struct Cyc_List_List*_tmp3D9;_tmp1BC->non_local_preds=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->hd=s,((_tmp3D9->tl=_tmp1BC->non_local_preds,_tmp3D9))))));}
*sopt=_tmp1BC;
return;_LL7D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1BD=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1BA;if(_tmp1BD->tag != 0)goto _LL7F;}_LL7E:
# 791
{const char*_tmp3DC;void*_tmp3DB;(_tmp3DB=0,Cyc_Tcutil_terr(s->loc,((_tmp3DC="break not in a loop or switch",_tag_dyneither(_tmp3DC,sizeof(char),30))),_tag_dyneither(_tmp3DB,sizeof(void*),0)));}
return;_LL7F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1BE=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1BA;if(_tmp1BE->tag != 2)goto _LL81;}_LL80:
# 797
{void*_tmp1C3=Cyc_Tcutil_compress(_tmp1B9->return_typ);_LL84: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1C4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1C3;if(_tmp1C4->tag != 0)goto _LL86;}_LL85:
 goto _LL83;_LL86: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1C5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1C3;if(_tmp1C5->tag != 7)goto _LL88;}_LL87:
 goto _LL89;_LL88: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1C3;if(_tmp1C6->tag != 6)goto _LL8A;}_LL89:
# 801
{const char*_tmp3DF;void*_tmp3DE;(_tmp3DE=0,Cyc_Tcutil_warn(s->loc,((_tmp3DF="break may cause function not to return a value",_tag_dyneither(_tmp3DF,sizeof(char),47))),_tag_dyneither(_tmp3DE,sizeof(void*),0)));}
goto _LL83;_LL8A:;_LL8B:
# 804
{const char*_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_Tcutil_terr(s->loc,((_tmp3E2="break may cause function not to return a value",_tag_dyneither(_tmp3E2,sizeof(char),47))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 807
return;_LL81: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1BF=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1BA;if(_tmp1BF->tag != 1)goto _LL7A;}_LL82:
# 810
{const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(s->loc,((_tmp3E5="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3E5,sizeof(char),56))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}
return;_LL7A:;};}
# 814
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
struct Cyc_Tcenv_SharedFenv*_tmp1CF;const char*_tmp3E6;struct Cyc_Tcenv_Fenv*_tmp1CE=Cyc_Tcenv_get_fenv(te,((_tmp3E6="process_goto",_tag_dyneither(_tmp3E6,sizeof(char),13))));_tmp1CF=_tmp1CE->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1CF->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1D0=_tmp1CF->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1D0,l);
struct _RegionHandle*frgn=_tmp1CF->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3E7;slopt=((_tmp3E7=_region_malloc(frgn,sizeof(*_tmp3E7)),((_tmp3E7[0]=0,_tmp3E7))));}{
struct Cyc_List_List*_tmp3E8;struct Cyc_List_List*new_needed=(_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8->hd=s,((_tmp3E8->tl=*slopt,_tmp3E8)))));
_tmp1CF->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1D0,l,new_needed);};}else{
# 826
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3E9;s->non_local_preds=((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9->hd=s,((_tmp3E9->tl=s->non_local_preds,_tmp3E9))))));}
*sopt=s;}};}
# 832
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 834
struct Cyc_Tcenv_CtrlEnv*_tmp1D6;const char*_tmp3EA;struct Cyc_Tcenv_Fenv*_tmp1D5=Cyc_Tcenv_get_fenv(te,((_tmp3EA="process_fallthru",_tag_dyneither(_tmp3EA,sizeof(char),17))));_tmp1D6=_tmp1D5->ctrl_env;{
const struct _tuple10*_tmp1D7=_tmp1D6->fallthru_clause;
if(_tmp1D7 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3EB;(((*_tmp1D7).f1)->body)->non_local_preds=(
(_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB->hd=s,((_tmp3EB->tl=(((*_tmp1D7).f1)->body)->non_local_preds,_tmp3EB))))));}{
struct Cyc_Absyn_Switch_clause**_tmp3EC;*clauseopt=((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=(*_tmp1D7).f1,_tmp3EC))));};}
# 841
return _tmp1D7;};}
# 844
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 848
struct Cyc_Tcenv_SharedFenv*_tmp1DC;struct Cyc_List_List*_tmp1DD;struct Cyc_RgnOrder_RgnPO*_tmp1DE;const struct Cyc_Tcenv_Bindings*_tmp1DF;struct Cyc_Absyn_Stmt*_tmp1E0;struct Cyc_Tcenv_CtrlEnv*_tmp1E1;void*_tmp1E2;void*_tmp1E3;int _tmp1E4;int _tmp1E5;int _tmp1E6;int _tmp1E7;const char*_tmp3ED;struct Cyc_Tcenv_Fenv*_tmp1DB=
# 850
Cyc_Tcenv_get_fenv(te,((_tmp3ED="set_fallthru",_tag_dyneither(_tmp3ED,sizeof(char),13))));_tmp1DC=_tmp1DB->shared;_tmp1DD=_tmp1DB->type_vars;_tmp1DE=_tmp1DB->region_order;_tmp1DF=_tmp1DB->locals;_tmp1E0=_tmp1DB->encloser;_tmp1E1=_tmp1DB->ctrl_env;_tmp1E2=_tmp1DB->capability;_tmp1E3=_tmp1DB->curr_rgn;_tmp1E4=_tmp1DB->in_notreadctxt;_tmp1E5=_tmp1DB->in_lhs;_tmp1E6=_tmp1DB->in_new;_tmp1E7=_tmp1DB->abstract_ok;{
struct _RegionHandle*_tmp1E9;void*_tmp1EA;void*_tmp1EB;const struct _tuple10*_tmp1EC;void*_tmp1ED;int _tmp1EE;struct Cyc_Tcenv_CtrlEnv*_tmp1E8=_tmp1E1;_tmp1E9=_tmp1E8->ctrl_rgn;_tmp1EA=_tmp1E8->continue_stmt;_tmp1EB=_tmp1E8->break_stmt;_tmp1EC=_tmp1E8->fallthru_clause;_tmp1ED=_tmp1E8->next_stmt;_tmp1EE=_tmp1E8->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp3EE;ft_typ=((_tmp3EE=_region_malloc(_tmp1E9,sizeof(*_tmp3EE)),((_tmp3EE->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp3EE->tl=ft_typ,_tmp3EE))))));}{
const struct Cyc_Tcenv_CList*_tmp1F0=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp3F1;struct Cyc_Tcenv_CtrlEnv*_tmp3F0;struct Cyc_Tcenv_CtrlEnv*_tmp1F1=
(_tmp3F0=_region_malloc(r,sizeof(*_tmp3F0)),((_tmp3F0->ctrl_rgn=_tmp1E9,((_tmp3F0->continue_stmt=_tmp1EA,((_tmp3F0->break_stmt=_tmp1EB,((_tmp3F0->fallthru_clause=(const struct _tuple10*)((_tmp3F1=_region_malloc(_tmp1E9,sizeof(*_tmp3F1)),((_tmp3F1->f1=clause,((_tmp3F1->f2=new_tvs,((_tmp3F1->f3=_tmp1F0,_tmp3F1)))))))),((_tmp3F0->next_stmt=_tmp1ED,((_tmp3F0->try_depth=_tmp1EE,_tmp3F0)))))))))))));
# 859
struct Cyc_Tcenv_Fenv*_tmp3F2;struct Cyc_Tcenv_Fenv*_tmp1F2=(_tmp3F2=_region_malloc(r,sizeof(*_tmp3F2)),((_tmp3F2->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1DC,((_tmp3F2->type_vars=(struct Cyc_List_List*)_tmp1DD,((_tmp3F2->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1DE,((_tmp3F2->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1DF,((_tmp3F2->encloser=(struct Cyc_Absyn_Stmt*)_tmp1E0,((_tmp3F2->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1F1,((_tmp3F2->capability=(void*)_tmp1E2,((_tmp3F2->curr_rgn=(void*)_tmp1E3,((_tmp3F2->in_notreadctxt=(int)_tmp1E4,((_tmp3F2->in_lhs=(int)_tmp1E5,((_tmp3F2->in_new=(int)_tmp1E6,((_tmp3F2->abstract_ok=(int)_tmp1E7,((_tmp3F2->fnrgn=(struct _RegionHandle*)r,_tmp3F2)))))))))))))))))))))))))));
# 863
return Cyc_Tcenv_put_fenv(r,te,_tmp1F2);};};};}
# 866
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp3F3;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp3F3="clear_fallthru",_tag_dyneither(_tmp3F3,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp1F7;struct Cyc_Tcenv_Fenv*_tmp1F6=fe;_tmp1F7=_tmp1F6->ctrl_env;
_tmp1F7->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 873
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 875
struct Cyc_Tcenv_SharedFenv*_tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_RgnOrder_RgnPO*_tmp1FD;const struct Cyc_Tcenv_Bindings*_tmp1FE;struct Cyc_Absyn_Stmt*_tmp1FF;struct Cyc_Tcenv_CtrlEnv*_tmp200;void*_tmp201;void*_tmp202;int _tmp203;int _tmp204;int _tmp205;int _tmp206;const char*_tmp3F4;struct Cyc_Tcenv_Fenv*_tmp1FA=
# 877
Cyc_Tcenv_get_fenv(te,((_tmp3F4="set_in_loop",_tag_dyneither(_tmp3F4,sizeof(char),12))));_tmp1FB=_tmp1FA->shared;_tmp1FC=_tmp1FA->type_vars;_tmp1FD=_tmp1FA->region_order;_tmp1FE=_tmp1FA->locals;_tmp1FF=_tmp1FA->encloser;_tmp200=_tmp1FA->ctrl_env;_tmp201=_tmp1FA->capability;_tmp202=_tmp1FA->curr_rgn;_tmp203=_tmp1FA->in_notreadctxt;_tmp204=_tmp1FA->in_lhs;_tmp205=_tmp1FA->in_new;_tmp206=_tmp1FA->abstract_ok;{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3FF;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3FE;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp3FD;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp3FC;struct Cyc_Tcenv_CtrlEnv*_tmp3FB;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp3FB=_region_malloc(r,sizeof(*_tmp3FB)),((_tmp3FB->ctrl_rgn=r,((_tmp3FB->continue_stmt=(void*)(
(_tmp3FD=_region_malloc(r,sizeof(*_tmp3FD)),((_tmp3FD[0]=((_tmp3FC.tag=3,((_tmp3FC.f1=continue_dest,_tmp3FC)))),_tmp3FD)))),((_tmp3FB->break_stmt=_tmp200->next_stmt,((_tmp3FB->next_stmt=(void*)(
# 882
(_tmp3FF=_region_malloc(r,sizeof(*_tmp3FF)),((_tmp3FF[0]=((_tmp3FE.tag=3,((_tmp3FE.f1=continue_dest,_tmp3FE)))),_tmp3FF)))),((_tmp3FB->fallthru_clause=_tmp200->fallthru_clause,((_tmp3FB->try_depth=_tmp200->try_depth,_tmp3FB)))))))))))));
# 885
struct Cyc_Tcenv_Fenv*_tmp400;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp400=_region_malloc(r,sizeof(*_tmp400)),((_tmp400->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1FB,((_tmp400->type_vars=(struct Cyc_List_List*)_tmp1FC,((_tmp400->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1FD,((_tmp400->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1FE,((_tmp400->encloser=(struct Cyc_Absyn_Stmt*)_tmp1FF,((_tmp400->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp400->capability=(void*)_tmp201,((_tmp400->curr_rgn=(void*)_tmp202,((_tmp400->in_notreadctxt=(int)_tmp203,((_tmp400->in_lhs=(int)_tmp204,((_tmp400->in_new=(int)_tmp205,((_tmp400->abstract_ok=(int)_tmp206,((_tmp400->fnrgn=(struct _RegionHandle*)r,_tmp400)))))))))))))))))))))))))));
# 889
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 892
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp401;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp401="enter_try",_tag_dyneither(_tmp401,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp20E;struct Cyc_Tcenv_Fenv*_tmp20D=fe;_tmp20E=_tmp20D->ctrl_env;
++ _tmp20E->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 898
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_CtrlEnv*_tmp212;const char*_tmp402;struct Cyc_Tcenv_Fenv*_tmp211=Cyc_Tcenv_get_fenv(te,((_tmp402="get_try_depth",_tag_dyneither(_tmp402,sizeof(char),14))));_tmp212=_tmp211->ctrl_env;
return _tmp212->try_depth;}
# 905
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp403;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp403="set_in_switch",_tag_dyneither(_tmp403,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp214;struct Cyc_Tcenv_Fenv*_tmp213=fe;_tmp214=_tmp213->ctrl_env;
_tmp214->break_stmt=_tmp214->next_stmt;
_tmp214->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 913
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 915
struct Cyc_Tcenv_SharedFenv*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_RgnOrder_RgnPO*_tmp21A;const struct Cyc_Tcenv_Bindings*_tmp21B;struct Cyc_Absyn_Stmt*_tmp21C;struct Cyc_Tcenv_CtrlEnv*_tmp21D;void*_tmp21E;void*_tmp21F;int _tmp220;int _tmp221;int _tmp222;int _tmp223;const char*_tmp404;struct Cyc_Tcenv_Fenv _tmp217=*
# 917
Cyc_Tcenv_get_fenv(te,((_tmp404="set_next",_tag_dyneither(_tmp404,sizeof(char),9))));_tmp218=_tmp217.shared;_tmp219=_tmp217.type_vars;_tmp21A=_tmp217.region_order;_tmp21B=_tmp217.locals;_tmp21C=_tmp217.encloser;_tmp21D=_tmp217.ctrl_env;_tmp21E=_tmp217.capability;_tmp21F=_tmp217.curr_rgn;_tmp220=_tmp217.in_notreadctxt;_tmp221=_tmp217.in_lhs;_tmp222=_tmp217.in_new;_tmp223=_tmp217.abstract_ok;{
struct Cyc_Tcenv_CtrlEnv*_tmp405;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp405=_region_malloc(r,sizeof(*_tmp405)),((_tmp405->ctrl_rgn=r,((_tmp405->continue_stmt=_tmp21D->continue_stmt,((_tmp405->break_stmt=_tmp21D->break_stmt,((_tmp405->next_stmt=j,((_tmp405->fallthru_clause=_tmp21D->fallthru_clause,((_tmp405->try_depth=_tmp21D->try_depth,_tmp405)))))))))))));
# 926
struct Cyc_Tcenv_Fenv*_tmp406;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp406=_region_malloc(r,sizeof(*_tmp406)),((_tmp406->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp218,((_tmp406->type_vars=(struct Cyc_List_List*)_tmp219,((_tmp406->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp21A,((_tmp406->locals=(const struct Cyc_Tcenv_Bindings*)_tmp21B,((_tmp406->encloser=(struct Cyc_Absyn_Stmt*)_tmp21C,((_tmp406->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp406->capability=(void*)_tmp21E,((_tmp406->curr_rgn=(void*)_tmp21F,((_tmp406->in_notreadctxt=(int)_tmp220,((_tmp406->in_lhs=(int)_tmp221,((_tmp406->in_new=(int)_tmp222,((_tmp406->abstract_ok=(int)_tmp223,((_tmp406->fnrgn=(struct _RegionHandle*)r,_tmp406)))))))))))))))))))))))))));
# 930
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 933
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Tcenv_SharedFenv*_tmp228;const char*_tmp407;struct Cyc_Tcenv_Fenv*_tmp227=Cyc_Tcenv_get_fenv(te,((_tmp407="add_label",_tag_dyneither(_tmp407,sizeof(char),10))));_tmp228=_tmp227->shared;{
struct Cyc_Dict_Dict needed=_tmp228->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp228->frgn;
if(sl_opt != 0){
_tmp228->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 941
struct Cyc_List_List*_tmp229=*sl_opt;
s->non_local_preds=_tmp229;
for(0;_tmp229 != 0;_tmp229=_tmp229->tl){
void*_tmp22A=((struct Cyc_Absyn_Stmt*)_tmp229->hd)->r;struct Cyc_Absyn_Stmt**_tmp22C;_LL8D: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp22B=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp22A;if(_tmp22B->tag != 8)goto _LL8F;else{_tmp22C=(struct Cyc_Absyn_Stmt**)& _tmp22B->f2;}}_LL8E:
*_tmp22C=s;goto _LL8C;_LL8F:;_LL90:
{const char*_tmp408;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp408="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp408,sizeof(char),42))));}goto _LL8C;_LL8C:;}};}
# 949
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp228->seen_labels,v)){
const char*_tmp40C;void*_tmp40B[1];struct Cyc_String_pa_PrintArg_struct _tmp40A;(_tmp40A.tag=0,((_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp40B[0]=& _tmp40A,Cyc_Tcutil_terr(s->loc,((_tmp40C="Repeated label: %s",_tag_dyneither(_tmp40C,sizeof(char),19))),_tag_dyneither(_tmp40B,sizeof(void*),1)))))));}
_tmp228->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp228->seen_labels,v,s);
return te;};}
# 955
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp233;const char*_tmp40D;struct Cyc_Tcenv_Fenv*_tmp232=Cyc_Tcenv_get_fenv(te,((_tmp40D="all_labels_resolved",_tag_dyneither(_tmp40D,sizeof(char),20))));_tmp233=_tmp232->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp233->needed_labels);}
# 960
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Stmt*_tmp236;const char*_tmp40E;struct Cyc_Tcenv_Fenv*_tmp235=Cyc_Tcenv_get_fenv(te,((_tmp40E="get_encloser",_tag_dyneither(_tmp40E,sizeof(char),13))));_tmp236=_tmp235->encloser;
return _tmp236;}
# 965
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp40F;return(_tmp40F=_region_malloc(r2,sizeof(*_tmp40F)),((_tmp40F->ns=te->ns,((_tmp40F->ae=te->ae,((_tmp40F->le=0,((_tmp40F->allow_valueof=1,_tmp40F)))))))));}{
struct Cyc_Tcenv_SharedFenv*_tmp239;struct Cyc_List_List*_tmp23A;struct Cyc_RgnOrder_RgnPO*_tmp23B;const struct Cyc_Tcenv_Bindings*_tmp23C;struct Cyc_Absyn_Stmt*_tmp23D;struct Cyc_Tcenv_CtrlEnv*_tmp23E;void*_tmp23F;void*_tmp240;int _tmp241;int _tmp242;int _tmp243;int _tmp244;struct Cyc_Tcenv_Fenv _tmp238=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp239=_tmp238.shared;_tmp23A=_tmp238.type_vars;_tmp23B=_tmp238.region_order;_tmp23C=_tmp238.locals;_tmp23D=_tmp238.encloser;_tmp23E=_tmp238.ctrl_env;_tmp23F=_tmp238.capability;_tmp240=_tmp238.curr_rgn;_tmp241=_tmp238.in_notreadctxt;_tmp242=_tmp238.in_lhs;_tmp243=_tmp238.in_new;_tmp244=_tmp238.abstract_ok;{
struct Cyc_Tcenv_Fenv*_tmp410;struct Cyc_Tcenv_Fenv*_tmp245=(_tmp410=_region_malloc(r2,sizeof(*_tmp410)),((_tmp410->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp239,((_tmp410->type_vars=(struct Cyc_List_List*)_tmp23A,((_tmp410->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp23B,((_tmp410->locals=(const struct Cyc_Tcenv_Bindings*)_tmp23C,((_tmp410->encloser=(struct Cyc_Absyn_Stmt*)_tmp23D,((_tmp410->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp23E,((_tmp410->capability=(void*)_tmp23F,((_tmp410->curr_rgn=(void*)_tmp240,((_tmp410->in_notreadctxt=(int)_tmp241,((_tmp410->in_lhs=(int)_tmp242,((_tmp410->in_new=(int)_tmp243,((_tmp410->abstract_ok=(int)_tmp244,((_tmp410->fnrgn=(struct _RegionHandle*)r2,_tmp410)))))))))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp411;return(_tmp411=_region_malloc(r2,sizeof(*_tmp411)),((_tmp411->ns=te->ns,((_tmp411->ae=te->ae,((_tmp411->le=_tmp245,((_tmp411->allow_valueof=1,_tmp411)))))))));};};}
# 973
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 975
struct Cyc_Tcenv_SharedFenv*_tmp24A;struct Cyc_List_List*_tmp24B;struct Cyc_RgnOrder_RgnPO*_tmp24C;const struct Cyc_Tcenv_Bindings*_tmp24D;struct Cyc_Absyn_Stmt*_tmp24E;struct Cyc_Tcenv_CtrlEnv*_tmp24F;void*_tmp250;void*_tmp251;int _tmp252;int _tmp253;int _tmp254;int _tmp255;const char*_tmp412;struct Cyc_Tcenv_Fenv _tmp249=*
# 977
Cyc_Tcenv_get_fenv(te,((_tmp412="set_encloser",_tag_dyneither(_tmp412,sizeof(char),13))));_tmp24A=_tmp249.shared;_tmp24B=_tmp249.type_vars;_tmp24C=_tmp249.region_order;_tmp24D=_tmp249.locals;_tmp24E=_tmp249.encloser;_tmp24F=_tmp249.ctrl_env;_tmp250=_tmp249.capability;_tmp251=_tmp249.curr_rgn;_tmp252=_tmp249.in_notreadctxt;_tmp253=_tmp249.in_lhs;_tmp254=_tmp249.in_new;_tmp255=_tmp249.abstract_ok;{
struct Cyc_Tcenv_Fenv*_tmp413;struct Cyc_Tcenv_Fenv*_tmp256=
(_tmp413=_region_malloc(r,sizeof(*_tmp413)),((_tmp413->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp24A,((_tmp413->type_vars=(struct Cyc_List_List*)_tmp24B,((_tmp413->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24C,((_tmp413->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24D,((_tmp413->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp413->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp24F,((_tmp413->capability=(void*)_tmp250,((_tmp413->curr_rgn=(void*)_tmp251,((_tmp413->in_notreadctxt=(int)_tmp252,((_tmp413->in_lhs=(int)_tmp253,((_tmp413->in_new=(int)_tmp254,((_tmp413->abstract_ok=(int)_tmp255,((_tmp413->fnrgn=(struct _RegionHandle*)r,_tmp413)))))))))))))))))))))))))));
# 982
return Cyc_Tcenv_put_fenv(r,te,_tmp256);};}
# 985
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 988
struct Cyc_Absyn_Tvar*tv;
{void*_tmp258=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp25A;_LL92: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp258;if(_tmp259->tag != 2)goto _LL94;else{_tmp25A=_tmp259->f1;}}_LL93:
 tv=_tmp25A;goto _LL91;_LL94:;_LL95:
{const char*_tmp414;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp414="bad add region",_tag_dyneither(_tmp414,sizeof(char),15))));}goto _LL91;_LL91:;}{
# 993
struct Cyc_Tcenv_SharedFenv*_tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_RgnOrder_RgnPO*_tmp260;const struct Cyc_Tcenv_Bindings*_tmp261;struct Cyc_Absyn_Stmt*_tmp262;struct Cyc_Tcenv_CtrlEnv*_tmp263;void*_tmp264;void*_tmp265;int _tmp266;int _tmp267;int _tmp268;int _tmp269;const char*_tmp415;struct Cyc_Tcenv_Fenv _tmp25D=*
# 995
Cyc_Tcenv_get_fenv(te,((_tmp415="add_region",_tag_dyneither(_tmp415,sizeof(char),11))));_tmp25E=_tmp25D.shared;_tmp25F=_tmp25D.type_vars;_tmp260=_tmp25D.region_order;_tmp261=_tmp25D.locals;_tmp262=_tmp25D.encloser;_tmp263=_tmp25D.ctrl_env;_tmp264=_tmp25D.capability;_tmp265=_tmp25D.curr_rgn;_tmp266=_tmp25D.in_notreadctxt;_tmp267=_tmp25D.in_lhs;_tmp268=_tmp25D.in_new;_tmp269=_tmp25D.abstract_ok;
_tmp260=Cyc_RgnOrder_add_youngest(_tmp25E->frgn,_tmp260,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp428;struct Cyc_List_List*_tmp427;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp426;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp425;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp423;_tmp264=(void*)((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423[0]=((_tmp428.tag=24,((_tmp428.f1=((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->hd=(void*)((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426[0]=((_tmp425.tag=23,((_tmp425.f1=rgn,_tmp425)))),_tmp426)))),((_tmp424->tl=(
(_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427->hd=_tmp264,((_tmp427->tl=0,_tmp427)))))),_tmp424)))))),_tmp428)))),_tmp423))));}{
struct Cyc_Tcenv_Fenv*_tmp429;struct Cyc_Tcenv_Fenv*_tmp270=
(_tmp429=_region_malloc(r,sizeof(*_tmp429)),((_tmp429->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp25E,((_tmp429->type_vars=(struct Cyc_List_List*)_tmp25F,((_tmp429->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp260,((_tmp429->locals=(const struct Cyc_Tcenv_Bindings*)_tmp261,((_tmp429->encloser=(struct Cyc_Absyn_Stmt*)_tmp262,((_tmp429->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp263,((_tmp429->capability=(void*)_tmp264,((_tmp429->curr_rgn=(void*)_tmp265,((_tmp429->in_notreadctxt=(int)_tmp266,((_tmp429->in_lhs=(int)_tmp267,((_tmp429->in_new=(int)_tmp268,((_tmp429->abstract_ok=(int)_tmp269,((_tmp429->fnrgn=(struct _RegionHandle*)r,_tmp429)))))))))))))))))))))))))));
# 1003
return Cyc_Tcenv_put_fenv(r,te,_tmp270);};};}
# 1006
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1008
struct Cyc_Tcenv_SharedFenv*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_RgnOrder_RgnPO*_tmp276;const struct Cyc_Tcenv_Bindings*_tmp277;struct Cyc_Absyn_Stmt*_tmp278;struct Cyc_Tcenv_CtrlEnv*_tmp279;void*_tmp27A;void*_tmp27B;int _tmp27C;int _tmp27D;int _tmp27E;int _tmp27F;const char*_tmp42A;struct Cyc_Tcenv_Fenv _tmp273=*
# 1010
Cyc_Tcenv_get_fenv(te,((_tmp42A="new_named_block",_tag_dyneither(_tmp42A,sizeof(char),16))));_tmp274=_tmp273.shared;_tmp275=_tmp273.type_vars;_tmp276=_tmp273.region_order;_tmp277=_tmp273.locals;_tmp278=_tmp273.encloser;_tmp279=_tmp273.ctrl_env;_tmp27A=_tmp273.capability;_tmp27B=_tmp273.curr_rgn;_tmp27C=_tmp273.in_notreadctxt;_tmp27D=_tmp273.in_lhs;_tmp27E=_tmp273.in_new;_tmp27F=_tmp273.abstract_ok;{
# 1012
const char*_tmp42B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp42B="new_block",_tag_dyneither(_tmp42B,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp42E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp42D;void*block_typ=(void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp42E.tag=2,((_tmp42E.f1=block_rgn,_tmp42E)))),_tmp42D))));
{struct Cyc_List_List*_tmp42F;_tmp275=((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F->hd=block_rgn,((_tmp42F->tl=_tmp275,_tmp42F))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp275);
_tmp276=Cyc_RgnOrder_add_youngest(_tmp274->frgn,_tmp276,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp442;struct Cyc_List_List*_tmp441;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp440;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp43F;struct Cyc_List_List*_tmp43E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp43D;_tmp27A=(void*)((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp442.tag=24,((_tmp442.f1=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E->hd=(void*)((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440[0]=((_tmp43F.tag=23,((_tmp43F.f1=block_typ,_tmp43F)))),_tmp440)))),((_tmp43E->tl=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441->hd=_tmp27A,((_tmp441->tl=0,_tmp441)))))),_tmp43E)))))),_tmp442)))),_tmp43D))));}
_tmp27B=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp443;struct Cyc_Tcenv_Fenv*_tmp287=
(_tmp443=_region_malloc(r,sizeof(*_tmp443)),((_tmp443->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp274,((_tmp443->type_vars=(struct Cyc_List_List*)_tmp275,((_tmp443->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp276,((_tmp443->locals=(const struct Cyc_Tcenv_Bindings*)_tmp277,((_tmp443->encloser=(struct Cyc_Absyn_Stmt*)_tmp278,((_tmp443->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp279,((_tmp443->capability=(void*)_tmp27A,((_tmp443->curr_rgn=(void*)_tmp27B,((_tmp443->in_notreadctxt=(int)_tmp27C,((_tmp443->in_lhs=(int)_tmp27D,((_tmp443->in_new=(int)_tmp27E,((_tmp443->abstract_ok=(int)_tmp27F,((_tmp443->fnrgn=(struct _RegionHandle*)r,_tmp443)))))))))))))))))))))))))));
# 1023
return Cyc_Tcenv_put_fenv(r,te,_tmp287);};};}
# 1026
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1028
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1030
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1036
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1040
struct Cyc_Tcenv_SharedFenv*_tmp28F;struct Cyc_List_List*_tmp290;struct Cyc_RgnOrder_RgnPO*_tmp291;const struct Cyc_Tcenv_Bindings*_tmp292;struct Cyc_Absyn_Stmt*_tmp293;struct Cyc_Tcenv_CtrlEnv*_tmp294;void*_tmp295;void*_tmp296;int _tmp297;int _tmp298;int _tmp299;int _tmp29A;const char*_tmp444;struct Cyc_Tcenv_Fenv _tmp28E=*
# 1042
Cyc_Tcenv_get_fenv(te,((_tmp444="new_outlives_constraints",_tag_dyneither(_tmp444,sizeof(char),25))));_tmp28F=_tmp28E.shared;_tmp290=_tmp28E.type_vars;_tmp291=_tmp28E.region_order;_tmp292=_tmp28E.locals;_tmp293=_tmp28E.encloser;_tmp294=_tmp28E.ctrl_env;_tmp295=_tmp28E.capability;_tmp296=_tmp28E.curr_rgn;_tmp297=_tmp28E.in_notreadctxt;_tmp298=_tmp28E.in_lhs;_tmp299=_tmp28E.in_new;_tmp29A=_tmp28E.abstract_ok;
for(0;cs != 0;cs=cs->tl){
_tmp291=
Cyc_RgnOrder_add_outlives_constraint(_tmp28F->frgn,_tmp291,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1048
struct Cyc_Tcenv_Fenv*_tmp445;struct Cyc_Tcenv_Fenv*_tmp29B=
(_tmp445=_region_malloc(r,sizeof(*_tmp445)),((_tmp445->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp28F,((_tmp445->type_vars=(struct Cyc_List_List*)_tmp290,((_tmp445->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp291,((_tmp445->locals=(const struct Cyc_Tcenv_Bindings*)_tmp292,((_tmp445->encloser=(struct Cyc_Absyn_Stmt*)_tmp293,((_tmp445->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp294,((_tmp445->capability=(void*)_tmp295,((_tmp445->curr_rgn=(void*)_tmp296,((_tmp445->in_notreadctxt=(int)_tmp297,((_tmp445->in_lhs=(int)_tmp298,((_tmp445->in_new=(int)_tmp299,((_tmp445->abstract_ok=(int)_tmp29A,((_tmp445->fnrgn=(struct _RegionHandle*)r,_tmp445)))))))))))))))))))))))))));
# 1052
return Cyc_Tcenv_put_fenv(r,te,_tmp29B);};}
# 1055
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1063
void*_tmp29D=Cyc_Tcutil_compress(r1);
void*_tmp29E=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp29F=Cyc_Tcutil_typ_kind(_tmp29D);
struct Cyc_Absyn_Kind*_tmp2A0=Cyc_Tcutil_typ_kind(_tmp29E);
# 1069
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp29F,_tmp2A0);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2A0,_tmp29F);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp44C;void*_tmp44B[4];struct Cyc_String_pa_PrintArg_struct _tmp44A;struct Cyc_String_pa_PrintArg_struct _tmp449;struct Cyc_String_pa_PrintArg_struct _tmp448;struct Cyc_String_pa_PrintArg_struct _tmp447;(_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2A0)),((_tmp448.tag=0,((_tmp448.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp29F)),((_tmp449.tag=0,((_tmp449.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29E)),((_tmp44A.tag=0,((_tmp44A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29D)),((_tmp44B[0]=& _tmp44A,((_tmp44B[1]=& _tmp449,((_tmp44B[2]=& _tmp448,((_tmp44B[3]=& _tmp447,Cyc_Tcutil_terr(loc,((_tmp44C="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp44C,sizeof(char),82))),_tag_dyneither(_tmp44B,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1076
if(r1_le_r2  && !r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp2A8;void*_tmp2A9;struct _tuple11 _tmp2A7=Cyc_Tcutil_swap_kind(_tmp29E,Cyc_Tcutil_kind_to_bound(_tmp29F));_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;{
struct _tuple11*_tmp44D;*oldtv=((_tmp44D=_region_malloc(r,sizeof(*_tmp44D)),((_tmp44D->f1=_tmp2A8,((_tmp44D->f2=_tmp2A9,_tmp44D))))));};}else{
# 1080
if(!r1_le_r2  && r2_le_r1){
struct Cyc_Absyn_Tvar*_tmp2AC;void*_tmp2AD;struct _tuple11 _tmp2AB=Cyc_Tcutil_swap_kind(_tmp29D,Cyc_Tcutil_kind_to_bound(_tmp2A0));_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;{
struct _tuple11*_tmp44E;*oldtv=((_tmp44E=_region_malloc(r,sizeof(*_tmp44E)),((_tmp44E->f1=_tmp2AC,((_tmp44E->f2=_tmp2AD,_tmp44E))))));};}}}{
# 1086
struct _RegionHandle*_tmp2AF=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2B0=0;
if((_tmp29D != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29D != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29D != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp451;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp450;void*eff1=(void*)((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450[0]=((_tmp451.tag=23,((_tmp451.f1=_tmp29D,_tmp451)))),_tmp450))));
struct _tuple13*_tmp454;struct Cyc_List_List*_tmp453;_tmp2B0=((_tmp453=_region_malloc(_tmp2AF,sizeof(*_tmp453)),((_tmp453->hd=((_tmp454=_region_malloc(_tmp2AF,sizeof(*_tmp454)),((_tmp454->f1=eff1,((_tmp454->f2=_tmp29E,_tmp454)))))),((_tmp453->tl=_tmp2B0,_tmp453))))));}
# 1092
if((_tmp29E != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29E != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29E != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp457;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp456;void*eff2=(void*)((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456[0]=((_tmp457.tag=23,((_tmp457.f1=_tmp29E,_tmp457)))),_tmp456))));
struct _tuple13*_tmp45A;struct Cyc_List_List*_tmp459;_tmp2B0=((_tmp459=_region_malloc(_tmp2AF,sizeof(*_tmp459)),((_tmp459->hd=((_tmp45A=_region_malloc(_tmp2AF,sizeof(*_tmp45A)),((_tmp45A->f1=eff2,((_tmp45A->f2=_tmp29D,_tmp45A)))))),((_tmp459->tl=_tmp2B0,_tmp459))))));}
# 1097
return Cyc_Tcenv_new_outlives_constraints(_tmp2AF,te,_tmp2B0,loc);};}
# 1100
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B9=te->le;
if(_tmp2B9 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2B9;
void*_tmp2BB;struct Cyc_Tcenv_Fenv*_tmp2BA=fe;_tmp2BB=_tmp2BA->curr_rgn;
return _tmp2BB;};}
# 1111
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp45B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp45B="check_rgn_accessible",_tag_dyneither(_tmp45B,sizeof(char),21))));
void*_tmp2BD;struct Cyc_RgnOrder_RgnPO*_tmp2BE;struct Cyc_Tcenv_Fenv*_tmp2BC=fe;_tmp2BD=_tmp2BC->capability;_tmp2BE=_tmp2BC->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2BD) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2BD))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp45E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp45D;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2BE,(void*)((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45E.tag=23,((_tmp45E.f1=rgn,_tmp45E)))),_tmp45D)))),_tmp2BD))
return;}{
const char*_tmp462;void*_tmp461[1];struct Cyc_String_pa_PrintArg_struct _tmp460;(_tmp460.tag=0,((_tmp460.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp461[0]=& _tmp460,Cyc_Tcutil_terr(loc,((_tmp462="Expression accesses unavailable region %s",_tag_dyneither(_tmp462,sizeof(char),42))),_tag_dyneither(_tmp461,sizeof(void*),1)))))));};}
# 1123
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
struct Cyc_RgnOrder_RgnPO*_tmp2C7;const char*_tmp463;struct Cyc_Tcenv_Fenv*_tmp2C6=
Cyc_Tcenv_get_fenv(te,((_tmp463="check_rgn_resetable",_tag_dyneither(_tmp463,sizeof(char),20))));_tmp2C7=_tmp2C6->region_order;{
void*_tmp2C8=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp2CA;_LL97: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C8;if(_tmp2C9->tag != 2)goto _LL99;else{_tmp2CA=_tmp2C9->f1;}}_LL98:
# 1129
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2C7,_tmp2CA)){
const char*_tmp467;void*_tmp466[1];struct Cyc_String_pa_PrintArg_struct _tmp465;(_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp466[0]=& _tmp465,Cyc_Tcutil_terr(loc,((_tmp467="Region %s is not resetable",_tag_dyneither(_tmp467,sizeof(char),27))),_tag_dyneither(_tmp466,sizeof(void*),1)))))));}
return;_LL99:;_LL9A: {
# 1133
const char*_tmp46A;void*_tmp469;(_tmp469=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp46A="check_rgn_resetable",_tag_dyneither(_tmp46A,sizeof(char),20))),_tag_dyneither(_tmp469,sizeof(void*),0)));}_LL96:;};};}
# 1140
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2D0=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1145
if(_tmp2D0 == 0){
void*_tmp2D1=rt_a;_LL9C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D2->tag != 22)goto _LL9E;}_LL9D:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LL9E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D3->tag != 21)goto _LLA0;}_LL9F:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLA0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D4->tag != 20)goto _LLA2;}_LLA1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLA2:;_LLA3:
 return 0;_LL9B:;}{
# 1153
struct Cyc_Tcenv_Fenv*fe=_tmp2D0;
struct Cyc_RgnOrder_RgnPO*_tmp2D6;struct Cyc_Tcenv_Fenv*_tmp2D5=fe;_tmp2D6=_tmp2D5->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp46D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46C;int _tmp2D7=Cyc_RgnOrder_effect_outlives(_tmp2D6,(void*)((_tmp46C=_cycalloc(sizeof(*_tmp46C)),((_tmp46C[0]=((_tmp46D.tag=23,((_tmp46D.f1=rt_a,_tmp46D)))),_tmp46C)))),rt_b);
# 1159
return _tmp2D7;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1164
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
void*_tmp2DC;struct Cyc_RgnOrder_RgnPO*_tmp2DD;struct Cyc_Tcenv_SharedFenv*_tmp2DE;const char*_tmp46E;struct Cyc_Tcenv_Fenv*_tmp2DB=
Cyc_Tcenv_get_fenv(te,((_tmp46E="check_effect_accessible",_tag_dyneither(_tmp46E,sizeof(char),24))));_tmp2DC=_tmp2DB->capability;_tmp2DD=_tmp2DB->region_order;_tmp2DE=_tmp2DB->shared;
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2DC))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2DD,eff,_tmp2DC))
return;{
struct _RegionHandle*frgn=_tmp2DE->frgn;
struct _tuple14*_tmp471;struct Cyc_List_List*_tmp470;_tmp2DE->delayed_effect_checks=(
(_tmp470=_region_malloc(frgn,sizeof(*_tmp470)),((_tmp470->hd=((_tmp471=_region_malloc(frgn,sizeof(*_tmp471)),((_tmp471->f1=_tmp2DC,((_tmp471->f2=eff,((_tmp471->f3=_tmp2DD,((_tmp471->f4=loc,_tmp471)))))))))),((_tmp470->tl=_tmp2DE->delayed_effect_checks,_tmp470))))));};}
# 1176
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp2E3;const char*_tmp472;struct Cyc_Tcenv_Fenv*_tmp2E2=
Cyc_Tcenv_get_fenv(te,((_tmp472="check_delayed_effects",_tag_dyneither(_tmp472,sizeof(char),22))));_tmp2E3=_tmp2E2->shared;{
struct Cyc_List_List*_tmp2E4=_tmp2E3->delayed_effect_checks;
for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){
void*_tmp2E6;void*_tmp2E7;struct Cyc_RgnOrder_RgnPO*_tmp2E8;unsigned int _tmp2E9;struct _tuple14*_tmp2E5=(struct _tuple14*)_tmp2E4->hd;_tmp2E6=_tmp2E5->f1;_tmp2E7=_tmp2E5->f2;_tmp2E8=_tmp2E5->f3;_tmp2E9=_tmp2E5->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2E7,_tmp2E6))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E8,_tmp2E7,_tmp2E6))
continue;{
const char*_tmp477;void*_tmp476[2];struct Cyc_String_pa_PrintArg_struct _tmp475;struct Cyc_String_pa_PrintArg_struct _tmp474;(_tmp474.tag=0,((_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2E7)),((_tmp475.tag=0,((_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E6)),((_tmp476[0]=& _tmp475,((_tmp476[1]=& _tmp474,Cyc_Tcutil_terr(_tmp2E9,((_tmp477="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp477,sizeof(char),51))),_tag_dyneither(_tmp476,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1195
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1197
struct Cyc_Tcenv_Fenv*_tmp2EE=te->le;
if(_tmp2EE == 0){
# 1200
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp47A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp479;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479[0]=((_tmp47A.tag=23,((_tmp47A.f1=(*((struct _tuple13*)po->hd)).f2,_tmp47A)))),_tmp479)))),Cyc_Absyn_empty_effect)){
const char*_tmp47D;void*_tmp47C;(_tmp47C=0,Cyc_Tcutil_terr(loc,((_tmp47D="the required region ordering is not satisfied here",_tag_dyneither(_tmp47D,sizeof(char),51))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}}
return;}{
# 1206
struct Cyc_RgnOrder_RgnPO*_tmp2F4;struct Cyc_Tcenv_SharedFenv*_tmp2F5;struct Cyc_Tcenv_Fenv*_tmp2F3=_tmp2EE;_tmp2F4=_tmp2F3->region_order;_tmp2F5=_tmp2F3->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2F4,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp480;struct Cyc_List_List*_tmp47F;_tmp2F5->delayed_constraint_checks=(
(_tmp47F=_region_malloc(_tmp2F5->frgn,sizeof(*_tmp47F)),((_tmp47F->hd=((_tmp480=_region_malloc(_tmp2F5->frgn,sizeof(*_tmp480)),((_tmp480->f1=_tmp2F4,((_tmp480->f2=po,((_tmp480->f3=loc,_tmp480)))))))),((_tmp47F->tl=_tmp2F5->delayed_constraint_checks,_tmp47F))))));}};}
# 1213
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_SharedFenv*_tmp2FA;const char*_tmp481;struct Cyc_Tcenv_Fenv*_tmp2F9=
Cyc_Tcenv_get_fenv(te,((_tmp481="check_delayed_constraints",_tag_dyneither(_tmp481,sizeof(char),26))));_tmp2FA=_tmp2F9->shared;{
struct Cyc_List_List*_tmp2FB=_tmp2FA->delayed_constraint_checks;
for(0;_tmp2FB != 0;_tmp2FB=_tmp2FB->tl){
struct Cyc_RgnOrder_RgnPO*_tmp2FD;struct Cyc_List_List*_tmp2FE;unsigned int _tmp2FF;struct _tuple15*_tmp2FC=(struct _tuple15*)_tmp2FB->hd;_tmp2FD=_tmp2FC->f1;_tmp2FE=_tmp2FC->f2;_tmp2FF=_tmp2FC->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2FD,_tmp2FE,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp484;void*_tmp483;(_tmp483=0,Cyc_Tcutil_terr(_tmp2FF,((_tmp484="the required region ordering is not satisfied here",_tag_dyneither(_tmp484,sizeof(char),51))),_tag_dyneither(_tmp483,sizeof(void*),0)));}}};}
# 1224
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp491;const char*_tmp490;void*_tmp48F[1];struct Cyc_String_pa_PrintArg_struct _tmp48E;struct Cyc_Absyn_Tvar*_tmp48D;struct Cyc_Absyn_Tvar*rgn0=
(_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D->name=((_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=(struct _dyneither_ptr)((_tmp48E.tag=0,((_tmp48E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp48F[0]=& _tmp48E,Cyc_aprintf(((_tmp490="`%s",_tag_dyneither(_tmp490,sizeof(char),4))),_tag_dyneither(_tmp48F,sizeof(void*),1)))))))),_tmp491)))),((_tmp48D->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp48D->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp48D)))))));
struct Cyc_List_List*_tmp492;struct Cyc_List_List*_tmp302=(_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492->hd=rgn0,((_tmp492->tl=fd->tvs,_tmp492)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp302);{
# 1232
struct Cyc_RgnOrder_RgnPO*_tmp303=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1234
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp495;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp494;void*param_rgn=(void*)((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494[0]=((_tmp495.tag=2,((_tmp495.f1=rgn0,_tmp495)))),_tmp494))));
struct Cyc_List_List*_tmp304=0;
{struct Cyc_List_List*_tmp305=fd->args;for(0;_tmp305 != 0;_tmp305=_tmp305->tl){
struct Cyc_Absyn_Vardecl _tmp49B;struct _tuple0*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp499;struct Cyc_Absyn_Vardecl*_tmp306=(_tmp499=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp499[0]=(struct Cyc_Absyn_Vardecl)((_tmp49B.sc=Cyc_Absyn_Public,((_tmp49B.name=(
(_tmp49A=_cycalloc(sizeof(*_tmp49A)),((_tmp49A->f1=Cyc_Absyn_Loc_n,((_tmp49A->f2=(*((struct _tuple8*)_tmp305->hd)).f1,_tmp49A)))))),((_tmp49B.tq=(*((struct _tuple8*)_tmp305->hd)).f2,((_tmp49B.type=(*((struct _tuple8*)_tmp305->hd)).f3,((_tmp49B.initializer=0,((_tmp49B.rgn=param_rgn,((_tmp49B.attributes=0,((_tmp49B.escapes=0,_tmp49B)))))))))))))))),_tmp499)));
# 1244
{struct Cyc_List_List _tmp49E;struct Cyc_List_List*_tmp49D;_tmp304=((_tmp49D=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp49D[0]=(struct Cyc_List_List)((_tmp49E.hd=_tmp306,((_tmp49E.tl=_tmp304,_tmp49E)))),_tmp49D))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4A1;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4A0;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp309=(_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0[0]=((_tmp4A1.tag=3,((_tmp4A1.f1=_tmp306,_tmp4A1)))),_tmp4A0)));
struct _dyneither_ptr*_tmp30A=(*((struct _tuple8*)_tmp305->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4A2;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4A2=_region_malloc(r,sizeof(*_tmp4A2)),((_tmp4A2->v=_tmp30A,((_tmp4A2->b=(void*)_tmp309,((_tmp4A2->tl=locals,_tmp4A2))))))));};}}
# 1249
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp312;struct Cyc_Absyn_Tqual _tmp313;void*_tmp314;int _tmp315;struct Cyc_Absyn_VarargInfo _tmp311=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp312=_tmp311.name;_tmp313=_tmp311.tq;_tmp314=_tmp311.type;_tmp315=_tmp311.inject;
if(_tmp312 != 0){
void*_tmp316=Cyc_Absyn_dyneither_typ(_tmp314,param_rgn,_tmp313,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4A8;struct _tuple0*_tmp4A7;struct Cyc_Absyn_Vardecl*_tmp4A6;struct Cyc_Absyn_Vardecl*_tmp317=(_tmp4A6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4A6[0]=(struct Cyc_Absyn_Vardecl)((_tmp4A8.sc=Cyc_Absyn_Public,((_tmp4A8.name=(
(_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7->f1=Cyc_Absyn_Loc_n,((_tmp4A7->f2=_tmp312,_tmp4A7)))))),((_tmp4A8.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4A8.type=_tmp316,((_tmp4A8.initializer=0,((_tmp4A8.rgn=param_rgn,((_tmp4A8.attributes=0,((_tmp4A8.escapes=0,_tmp4A8)))))))))))))))),_tmp4A6)));
# 1260
{struct Cyc_List_List*_tmp4A9;_tmp304=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9->hd=_tmp317,((_tmp4A9->tl=_tmp304,_tmp4A9))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4AC;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4AB;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp319=(_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB[0]=((_tmp4AC.tag=3,((_tmp4AC.f1=_tmp317,_tmp4AC)))),_tmp4AB)));
struct _dyneither_ptr*_tmp31A=_tmp312;
struct Cyc_Tcenv_Bindings*_tmp4AD;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD->v=_tmp31A,((_tmp4AD->b=(void*)_tmp319,((_tmp4AD->tl=locals,_tmp4AD))))))));};}else{
# 1265
const char*_tmp4B0;void*_tmp4AF;(_tmp4AF=0,Cyc_Tcutil_terr(loc,((_tmp4B0="missing name for varargs",_tag_dyneither(_tmp4B0,sizeof(char),25))),_tag_dyneither(_tmp4AF,sizeof(void*),0)));}}
# 1267
{struct Cyc_Core_Opt _tmp4B3;struct Cyc_Core_Opt*_tmp4B2;fd->param_vardecls=((_tmp4B2=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4B2[0]=(struct Cyc_Core_Opt)((_tmp4B3.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp304),_tmp4B3)),_tmp4B2))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4D1;struct Cyc_List_List*_tmp4D0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4CF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4CE;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4CC;struct Cyc_Tcenv_CtrlEnv*_tmp4CB;struct Cyc_Tcenv_SharedFenv*_tmp4CA;struct Cyc_Tcenv_Fenv*_tmp4C9;return(_tmp4C9=_region_malloc(r,sizeof(*_tmp4C9)),((_tmp4C9->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA)),((_tmp4CA->frgn=r,((_tmp4CA->return_typ=fd->ret_type,((_tmp4CA->seen_labels=
# 1272
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4CA->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4CA->delayed_effect_checks=0,((_tmp4CA->delayed_constraint_checks=0,_tmp4CA)))))))))))))),((_tmp4C9->type_vars=(struct Cyc_List_List*)_tmp302,((_tmp4C9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp303,((_tmp4C9->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp4C9->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4C9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1281
(_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB)),((_tmp4CB->ctrl_rgn=r,((_tmp4CB->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4CB->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4CB->fallthru_clause=0,((_tmp4CB->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4CB->try_depth=0,_tmp4CB)))))))))))))),((_tmp4C9->capability=(void*)((void*)(
(_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=((_tmp4CC.tag=24,((_tmp4CC.f1=((_tmp4D0=_cycalloc(sizeof(*_tmp4D0)),((_tmp4D0->hd=(void*)((_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE[0]=((_tmp4CF.tag=23,((_tmp4CF.f1=param_rgn,_tmp4CF)))),_tmp4CE)))),((_tmp4D0->tl=(
(_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD->hd=(void*)_check_null(fd->effect),((_tmp4CD->tl=0,_tmp4CD)))))),_tmp4D0)))))),_tmp4CC)))),_tmp4D1))))),((_tmp4C9->curr_rgn=(void*)param_rgn,((_tmp4C9->in_notreadctxt=(int)0,((_tmp4C9->in_lhs=(int)0,((_tmp4C9->in_new=(int)0,((_tmp4C9->abstract_ok=(int)0,((_tmp4C9->fnrgn=(struct _RegionHandle*)r,_tmp4C9)))))))))))))))))))))))))));};};}
# 1293
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp337;struct Cyc_RgnOrder_RgnPO*_tmp338;struct Cyc_List_List*_tmp339;struct Cyc_Tcenv_SharedFenv*_tmp33A;struct _RegionHandle*_tmp33B;struct Cyc_Tcenv_Fenv*_tmp336=old_fenv;_tmp337=_tmp336->locals;_tmp338=_tmp336->region_order;_tmp339=_tmp336->type_vars;_tmp33A=_tmp336->shared;_tmp33B=_tmp336->fnrgn;{
# 1296
struct _RegionHandle*_tmp33C=_tmp33A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp33D=_tmp337;
struct _dyneither_ptr*_tmp4DE;const char*_tmp4DD;void*_tmp4DC[1];struct Cyc_String_pa_PrintArg_struct _tmp4DB;struct Cyc_Absyn_Tvar*_tmp4DA;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4DA=_cycalloc(sizeof(*_tmp4DA)),((_tmp4DA->name=((_tmp4DE=_cycalloc(sizeof(*_tmp4DE)),((_tmp4DE[0]=(struct _dyneither_ptr)((_tmp4DB.tag=0,((_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4DC[0]=& _tmp4DB,Cyc_aprintf(((_tmp4DD="`%s",_tag_dyneither(_tmp4DD,sizeof(char),4))),_tag_dyneither(_tmp4DC,sizeof(void*),1)))))))),_tmp4DE)))),((_tmp4DA->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4DA->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4DA)))))));
# 1302
{struct Cyc_List_List*_tmp33E=fd->tvs;for(0;_tmp33E != 0;_tmp33E=_tmp33E->tl){
enum Cyc_Absyn_KindQual _tmp340;enum Cyc_Absyn_AliasQual _tmp341;struct Cyc_Absyn_Kind*_tmp33F=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp33E->hd,& Cyc_Tcutil_bk);_tmp340=_tmp33F->kind;_tmp341=_tmp33F->aliasqual;
if(_tmp340 == Cyc_Absyn_RgnKind){
if(_tmp341 == Cyc_Absyn_Aliasable)
_tmp338=Cyc_RgnOrder_add_youngest(_tmp33C,_tmp338,(struct Cyc_Absyn_Tvar*)_tmp33E->hd,0,0);else{
# 1308
const char*_tmp4E1;void*_tmp4E0;(_tmp4E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E1="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4E1,sizeof(char),39))),_tag_dyneither(_tmp4E0,sizeof(void*),0)));}}}}
# 1310
_tmp338=Cyc_RgnOrder_add_youngest(_tmp33C,_tmp338,rgn0,0,0);{
struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*_tmp344=(_tmp4E2=_cycalloc(sizeof(*_tmp4E2)),((_tmp4E2->hd=rgn0,((_tmp4E2->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp339),_tmp4E2)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp344);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E4;void*param_rgn=(void*)((_tmp4E4=_cycalloc(sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E5.tag=2,((_tmp4E5.f1=rgn0,_tmp4E5)))),_tmp4E4))));
struct Cyc_List_List*_tmp345=0;
{struct Cyc_List_List*_tmp346=fd->args;for(0;_tmp346 != 0;_tmp346=_tmp346->tl){
struct Cyc_Absyn_Vardecl _tmp4EB;struct _tuple0*_tmp4EA;struct Cyc_Absyn_Vardecl*_tmp4E9;struct Cyc_Absyn_Vardecl*_tmp347=(_tmp4E9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4E9[0]=(struct Cyc_Absyn_Vardecl)((_tmp4EB.sc=Cyc_Absyn_Public,((_tmp4EB.name=(
(_tmp4EA=_cycalloc(sizeof(*_tmp4EA)),((_tmp4EA->f1=Cyc_Absyn_Loc_n,((_tmp4EA->f2=(*((struct _tuple8*)_tmp346->hd)).f1,_tmp4EA)))))),((_tmp4EB.tq=(*((struct _tuple8*)_tmp346->hd)).f2,((_tmp4EB.type=(*((struct _tuple8*)_tmp346->hd)).f3,((_tmp4EB.initializer=0,((_tmp4EB.rgn=param_rgn,((_tmp4EB.attributes=0,((_tmp4EB.escapes=0,_tmp4EB)))))))))))))))),_tmp4E9)));
# 1323
{struct Cyc_List_List _tmp4EE;struct Cyc_List_List*_tmp4ED;_tmp345=((_tmp4ED=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4ED[0]=(struct Cyc_List_List)((_tmp4EE.hd=_tmp347,((_tmp4EE.tl=_tmp345,_tmp4EE)))),_tmp4ED))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4F1;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4F0;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34A=(_tmp4F0=_cycalloc(sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4F1.tag=3,((_tmp4F1.f1=_tmp347,_tmp4F1)))),_tmp4F0)));
struct _dyneither_ptr*_tmp34B=(*((struct _tuple8*)_tmp346->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4F2;_tmp33D=(const struct Cyc_Tcenv_Bindings*)((_tmp4F2=_region_malloc(_tmp33C,sizeof(*_tmp4F2)),((_tmp4F2->v=_tmp34B,((_tmp4F2->b=(void*)_tmp34A,((_tmp4F2->tl=_tmp33D,_tmp4F2))))))));};}}
# 1328
if(fd->cyc_varargs != 0){
struct _dyneither_ptr*_tmp353;struct Cyc_Absyn_Tqual _tmp354;void*_tmp355;int _tmp356;struct Cyc_Absyn_VarargInfo _tmp352=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));_tmp353=_tmp352.name;_tmp354=_tmp352.tq;_tmp355=_tmp352.type;_tmp356=_tmp352.inject;
if(_tmp353 != 0){
void*_tmp357=Cyc_Absyn_dyneither_typ(_tmp355,param_rgn,_tmp354,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4F8;struct _tuple0*_tmp4F7;struct Cyc_Absyn_Vardecl*_tmp4F6;struct Cyc_Absyn_Vardecl*_tmp358=(_tmp4F6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4F6[0]=(struct Cyc_Absyn_Vardecl)((_tmp4F8.sc=Cyc_Absyn_Public,((_tmp4F8.name=(
(_tmp4F7=_cycalloc(sizeof(*_tmp4F7)),((_tmp4F7->f1=Cyc_Absyn_Loc_n,((_tmp4F7->f2=_tmp353,_tmp4F7)))))),((_tmp4F8.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4F8.type=_tmp357,((_tmp4F8.initializer=0,((_tmp4F8.rgn=param_rgn,((_tmp4F8.attributes=0,((_tmp4F8.escapes=0,_tmp4F8)))))))))))))))),_tmp4F6)));
# 1339
{struct Cyc_List_List*_tmp4F9;_tmp345=((_tmp4F9=_cycalloc(sizeof(*_tmp4F9)),((_tmp4F9->hd=_tmp358,((_tmp4F9->tl=_tmp345,_tmp4F9))))));}{
struct _dyneither_ptr*_tmp35A=_tmp353;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4FC;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4FB;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp35B=(_tmp4FB=_cycalloc(sizeof(*_tmp4FB)),((_tmp4FB[0]=((_tmp4FC.tag=3,((_tmp4FC.f1=_tmp358,_tmp4FC)))),_tmp4FB)));
struct Cyc_Tcenv_Bindings*_tmp4FD;_tmp33D=(const struct Cyc_Tcenv_Bindings*)((_tmp4FD=_region_malloc(_tmp33C,sizeof(*_tmp4FD)),((_tmp4FD->v=_tmp35A,((_tmp4FD->b=(void*)_tmp35B,((_tmp4FD->tl=_tmp33D,_tmp4FD))))))));};}else{
# 1344
const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_Tcutil_terr(loc,((_tmp500="missing name for varargs",_tag_dyneither(_tmp500,sizeof(char),25))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}}
# 1346
{struct Cyc_Core_Opt _tmp503;struct Cyc_Core_Opt*_tmp502;fd->param_vardecls=((_tmp502=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp502[0]=(struct Cyc_Core_Opt)((_tmp503.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp345),_tmp503)),_tmp502))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp521;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp51F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp51E;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp51C;struct Cyc_Tcenv_CtrlEnv*_tmp51B;struct Cyc_Tcenv_SharedFenv*_tmp51A;struct Cyc_Tcenv_Fenv*_tmp519;return(_tmp519=_region_malloc(_tmp33C,sizeof(*_tmp519)),((_tmp519->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp51A=_region_malloc(_tmp33C,sizeof(*_tmp51A)),((_tmp51A->frgn=_tmp33C,((_tmp51A->return_typ=fd->ret_type,((_tmp51A->seen_labels=
# 1351
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp33C,Cyc_strptrcmp),((_tmp51A->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp33C,Cyc_strptrcmp),((_tmp51A->delayed_effect_checks=0,((_tmp51A->delayed_constraint_checks=0,_tmp51A)))))))))))))),((_tmp519->type_vars=(struct Cyc_List_List*)_tmp344,((_tmp519->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp338,((_tmp519->locals=(const struct Cyc_Tcenv_Bindings*)_tmp33D,((_tmp519->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp519->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1360
(_tmp51B=_region_malloc(_tmp33C,sizeof(*_tmp51B)),((_tmp51B->ctrl_rgn=_tmp33C,((_tmp51B->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp51B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp51B->fallthru_clause=0,((_tmp51B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp51B->try_depth=0,_tmp51B)))))))))))))),((_tmp519->capability=(void*)((void*)(
(_tmp521=_cycalloc(sizeof(*_tmp521)),((_tmp521[0]=((_tmp51C.tag=24,((_tmp51C.f1=((_tmp520=_cycalloc(sizeof(*_tmp520)),((_tmp520->hd=(void*)((_tmp51E=_cycalloc(sizeof(*_tmp51E)),((_tmp51E[0]=((_tmp51F.tag=23,((_tmp51F.f1=param_rgn,_tmp51F)))),_tmp51E)))),((_tmp520->tl=(
(_tmp51D=_cycalloc(sizeof(*_tmp51D)),((_tmp51D->hd=(void*)_check_null(fd->effect),((_tmp51D->tl=0,_tmp51D)))))),_tmp520)))))),_tmp51C)))),_tmp521))))),((_tmp519->curr_rgn=(void*)param_rgn,((_tmp519->in_notreadctxt=(int)0,((_tmp519->in_lhs=(int)0,((_tmp519->in_new=(int)0,((_tmp519->abstract_ok=(int)0,((_tmp519->fnrgn=(struct _RegionHandle*)_tmp33B,_tmp519)))))))))))))))))))))))))));};};};};}
