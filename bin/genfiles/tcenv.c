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
struct Cyc_Tcenv_Genv*_tmp377;return(_tmp377=_region_malloc(r,sizeof(*_tmp377)),((_tmp377->grgn=r,((_tmp377->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp377->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp377->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp377->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp377->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp377->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp377->availables=0,_tmp377)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 252
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 254
{struct Cyc_Core_Opt*_tmp378;Cyc_Tcutil_empty_var_set=((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp378))));}{
# 256
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp379;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[0]=Cyc_Absyn_exn_tud,_tmp379)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp37F;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp37E;struct _tuple12*_tmp37D;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp37D=_region_malloc(r,sizeof(*_tmp37D)),((_tmp37D->f1=(void*)(
(_tmp37F=_cycalloc(sizeof(*_tmp37F)),((_tmp37F[0]=((_tmp37E.tag=2,((_tmp37E.f1=Cyc_Absyn_exn_tud,((_tmp37E.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp37E)))))),_tmp37F)))),((_tmp37D->f2=1,_tmp37D)))))));}}{
# 263
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp380;return(_tmp380=_region_malloc(r,sizeof(*_tmp380)),((_tmp380->ns=0,((_tmp380->ae=ae,((_tmp380->le=0,((_tmp380->allow_valueof=0,_tmp380)))))))));};};}
# 269
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 271
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 275
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp381;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp381="outer_namespace",_tag_dyneither(_tmp381,sizeof(char),16))));}
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
const char*_tmp385;void*_tmp384[1];struct Cyc_String_pa_PrintArg_struct _tmp383;(_tmp383.tag=0,((_tmp383.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp384[0]=& _tmp383,Cyc_Tcutil_terr(loc,((_tmp385="%s is ambiguous",_tag_dyneither(_tmp385,sizeof(char),16))),_tag_dyneither(_tmp384,sizeof(void*),1)))))));};
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
struct Cyc_Dict_Absent_exn_struct _tmp388;struct Cyc_Dict_Absent_exn_struct*_tmp387;(int)_throw((void*)((_tmp387=_cycalloc_atomic(sizeof(*_tmp387)),((_tmp387[0]=((_tmp388.tag=Cyc_Dict_Absent,_tmp388)),_tmp387)))));}
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
struct Cyc_List_List*_tmp38B;struct Cyc_List_List*_tmp38A;_tmp38=((_tmp38A=_region_malloc(temp,sizeof(*_tmp38A)),((_tmp38A->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->hd=n,((_tmp38B->tl=ns,_tmp38B))))))),((_tmp38A->tl=_tmp38,_tmp38A))))));}}}
# 376
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp38E;struct Cyc_List_List*_tmp38D;_tmp38=((_tmp38D=_region_malloc(temp,sizeof(*_tmp38D)),((_tmp38D->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E->hd=n,((_tmp38E->tl=ns,_tmp38E))))))),((_tmp38D->tl=_tmp38,_tmp38D))))));}
# 382
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp392;void*_tmp391[1];struct Cyc_String_pa_PrintArg_struct _tmp390;(_tmp390.tag=0,((_tmp390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp391[0]=& _tmp390,Cyc_Tcutil_terr(loc,((_tmp392="%s is ambiguous",_tag_dyneither(_tmp392,sizeof(char),16))),_tag_dyneither(_tmp391,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 387
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp395;struct Cyc_Dict_Absent_exn_struct*_tmp394;(int)_throw((void*)((_tmp394=_cycalloc_atomic(sizeof(*_tmp394)),((_tmp394[0]=((_tmp395.tag=Cyc_Dict_Absent,_tmp395)),_tmp394)))));}
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
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp396;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp396=_region_malloc(r,sizeof(*_tmp396)),((_tmp396[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp396)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_throw(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 451
{const char*_tmp399;void*_tmp398;(_tmp398=0,Cyc_Tcutil_terr(loc,((_tmp399="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp399,sizeof(char),33))),_tag_dyneither(_tmp398,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 458
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 462
{const char*_tmp39C;void*_tmp39B;(_tmp39B=0,Cyc_Tcutil_terr(loc,((_tmp39C="bad qualified name for @extensible datatype",_tag_dyneither(_tmp39C,sizeof(char),44))),_tag_dyneither(_tmp39B,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp39F;struct Cyc_Dict_Absent_exn_struct*_tmp39E;(int)_throw((void*)((_tmp39E=_cycalloc_atomic(sizeof(*_tmp39E)),((_tmp39E[0]=((_tmp39F.tag=Cyc_Dict_Absent,_tmp39F)),_tmp39E)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_throw(_tmp77);_LL40:;}};}{
# 465
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3A0;return(_tmp3A0=_region_malloc(r,sizeof(*_tmp3A0)),((_tmp3A0[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp3A0)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 469
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3A1;return(_tmp3A1=_region_malloc(r,sizeof(*_tmp3A1)),((_tmp3A1[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp3A1)));}_LL30:;};}
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
if(te->le == 0){const char*_tmp3A2;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3A2="put_fenv",_tag_dyneither(_tmp3A2,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp3A3;return(_tmp3A3=_region_malloc(l,sizeof(*_tmp3A3)),((_tmp3A3->ns=te->ns,((_tmp3A3->ae=te->ae,((_tmp3A3->le=fe,((_tmp3A3->allow_valueof=te->allow_valueof,_tmp3A3)))))))));};}
# 540
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp3A4;return(_tmp3A4=_region_malloc(l,sizeof(*_tmp3A4)),((_tmp3A4->ns=te->ns,((_tmp3A4->ae=te->ae,((_tmp3A4->le=0,((_tmp3A4->allow_valueof=te->allow_valueof,_tmp3A4)))))))));}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_Fenv _stmttmp1=*f;struct Cyc_Tcenv_SharedFenv*_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_RgnOrder_RgnPO*_tmpA0;const struct Cyc_Tcenv_Bindings*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA2;struct Cyc_Tcenv_CtrlEnv*_tmpA3;void*_tmpA4;void*_tmpA5;struct Cyc_Tcenv_FenvFlags _tmpA6;struct _RegionHandle*_tmpA7;struct Cyc_Tcenv_Fenv _tmp9D=_stmttmp1;_tmp9E=_tmp9D.shared;_tmp9F=_tmp9D.type_vars;_tmpA0=_tmp9D.region_order;_tmpA1=_tmp9D.locals;_tmpA2=_tmp9D.encloser;_tmpA3=_tmp9D.ctrl_env;_tmpA4=_tmp9D.capability;_tmpA5=_tmp9D.curr_rgn;_tmpA6=_tmp9D.flags;_tmpA7=_tmp9D.fnrgn;{
# 548
struct Cyc_Tcenv_Fenv*_tmp3A5;return(_tmp3A5=_region_malloc(l,sizeof(*_tmp3A5)),((_tmp3A5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp9E,((_tmp3A5->type_vars=(struct Cyc_List_List*)_tmp9F,((_tmp3A5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA0,((_tmp3A5->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA1,((_tmp3A5->encloser=(struct Cyc_Absyn_Stmt*)_tmpA2,((_tmp3A5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA3,((_tmp3A5->capability=(void*)_tmpA4,((_tmp3A5->curr_rgn=(void*)_tmpA5,((_tmp3A5->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA6,((_tmp3A5->fnrgn=(struct _RegionHandle*)l,_tmp3A5)))))))))))))))))))));};}
# 552
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 554
struct Cyc_Tcenv_Fenv _stmttmp2=*f;struct Cyc_Tcenv_SharedFenv*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_RgnOrder_RgnPO*_tmpAC;const struct Cyc_Tcenv_Bindings*_tmpAD;struct Cyc_Absyn_Stmt*_tmpAE;struct Cyc_Tcenv_CtrlEnv*_tmpAF;void*_tmpB0;void*_tmpB1;struct Cyc_Tcenv_FenvFlags _tmpB2;struct Cyc_Tcenv_Fenv _tmpA9=_stmttmp2;_tmpAA=_tmpA9.shared;_tmpAB=_tmpA9.type_vars;_tmpAC=_tmpA9.region_order;_tmpAD=_tmpA9.locals;_tmpAE=_tmpA9.encloser;_tmpAF=_tmpA9.ctrl_env;_tmpB0=_tmpA9.capability;_tmpB1=_tmpA9.curr_rgn;_tmpB2=_tmpA9.flags;{
# 557
struct Cyc_Tcenv_CtrlEnv _stmttmp3=*_tmpAF;struct _RegionHandle*_tmpB4;void*_tmpB5;void*_tmpB6;const struct _tuple10*_tmpB7;void*_tmpB8;int _tmpB9;struct Cyc_Tcenv_CtrlEnv _tmpB3=_stmttmp3;_tmpB4=_tmpB3.ctrl_rgn;_tmpB5=_tmpB3.continue_stmt;_tmpB6=_tmpB3.break_stmt;_tmpB7=_tmpB3.fallthru_clause;_tmpB8=_tmpB3.next_stmt;_tmpB9=_tmpB3.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp3A6;struct Cyc_Tcenv_CtrlEnv*_tmpBA=
(_tmp3A6=_region_malloc(l,sizeof(*_tmp3A6)),((_tmp3A6->ctrl_rgn=_tmpB4,((_tmp3A6->continue_stmt=_tmpB5,((_tmp3A6->break_stmt=_tmpB6,((_tmp3A6->fallthru_clause=_tmpB7,((_tmp3A6->next_stmt=_tmpB8,((_tmp3A6->try_depth=_tmpB9,_tmp3A6)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp3A7;return(_tmp3A7=_region_malloc(l,sizeof(*_tmp3A7)),((_tmp3A7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAA,((_tmp3A7->type_vars=(struct Cyc_List_List*)_tmpAB,((_tmp3A7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAC,((_tmp3A7->locals=(const struct Cyc_Tcenv_Bindings*)_tmpAD,((_tmp3A7->encloser=(struct Cyc_Absyn_Stmt*)_tmpAE,((_tmp3A7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBA,((_tmp3A7->capability=(void*)_tmpB0,((_tmp3A7->curr_rgn=(void*)_tmpB1,((_tmp3A7->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB2,((_tmp3A7->fnrgn=(struct _RegionHandle*)l,_tmp3A7)))))))))))))))))))));};};}
# 566
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3A8;struct Cyc_Tcenv_Fenv*_stmttmp4=Cyc_Tcenv_get_fenv(te,((_tmp3A8="return_typ",_tag_dyneither(_tmp3A8,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpBE;struct Cyc_Tcenv_Fenv*_tmpBD=_stmttmp4;_tmpBE=_tmpBD->shared;{
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
struct Cyc_Core_Opt*_tmp3A9;return(_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3A9)));}
# 583
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 585
const char*_tmp3AA;struct Cyc_Tcenv_Fenv*_stmttmp7=
# 587
Cyc_Tcenv_get_fenv(te,((_tmp3AA="add_type_vars",_tag_dyneither(_tmp3AA,sizeof(char),14))));
# 585
struct Cyc_Tcenv_SharedFenv*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_RgnOrder_RgnPO*_tmpC9;const struct Cyc_Tcenv_Bindings*_tmpCA;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Tcenv_CtrlEnv*_tmpCC;void*_tmpCD;void*_tmpCE;struct Cyc_Tcenv_FenvFlags _tmpCF;struct Cyc_Tcenv_Fenv*_tmpC6=_stmttmp7;_tmpC7=_tmpC6->shared;_tmpC8=_tmpC6->type_vars;_tmpC9=_tmpC6->region_order;_tmpCA=_tmpC6->locals;_tmpCB=_tmpC6->encloser;_tmpCC=_tmpC6->ctrl_env;_tmpCD=_tmpC6->capability;_tmpCE=_tmpC6->curr_rgn;_tmpCF=_tmpC6->flags;
# 589
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC8);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD0);{
struct Cyc_Tcenv_Fenv*_tmp3AB;struct Cyc_Tcenv_Fenv*_tmpD1=(_tmp3AB=_region_malloc(r,sizeof(*_tmp3AB)),((_tmp3AB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpC7,((_tmp3AB->type_vars=(struct Cyc_List_List*)_tmpD0,((_tmp3AB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpC9,((_tmp3AB->locals=(const struct Cyc_Tcenv_Bindings*)_tmpCA,((_tmp3AB->encloser=(struct Cyc_Absyn_Stmt*)_tmpCB,((_tmp3AB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpCC,((_tmp3AB->capability=(void*)_tmpCD,((_tmp3AB->curr_rgn=(void*)_tmpCE,((_tmp3AB->flags=(struct Cyc_Tcenv_FenvFlags)_tmpCF,((_tmp3AB->fnrgn=(struct _RegionHandle*)r,_tmp3AB)))))))))))))))))))));
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
struct Cyc_Core_Impossible_exn_struct _tmp3B1;const char*_tmp3B0;struct Cyc_Core_Impossible_exn_struct*_tmp3AF;(int)_throw((void*)((_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF[0]=((_tmp3B1.tag=Cyc_Core_Impossible,((_tmp3B1.f1=((_tmp3B0="add_local_var: called with Rel_n",_tag_dyneither(_tmp3B0,sizeof(char),33))),_tmp3B1)))),_tmp3AF)))));}_LL5B:;}
# 616
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpDA;_push_handler(& _tmpDA);{int _tmpDC=0;if(setjmp(_tmpDA.handler))_tmpDC=1;if(!_tmpDC){
{void*_stmttmp9=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpDD=_stmttmp9;_LL61: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpDE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL63;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpDE->f1);if(_tmpDF->tag != 4)goto _LL63;}}_LL62:
# 620
{const char*_tmp3B4;void*_tmp3B3;(_tmp3B3=0,Cyc_Tcutil_warn(loc,((_tmp3B4="declaration hides local",_tag_dyneither(_tmp3B4,sizeof(char),24))),_tag_dyneither(_tmp3B3,sizeof(void*),0)));}goto _LL60;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE0=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE0->tag != 0)goto _LL65;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpE0->f1);if(_tmpE1->tag != 1)goto _LL65;}}_LL64:
{const char*_tmp3B7;void*_tmp3B6;(_tmp3B6=0,Cyc_Tcutil_warn(loc,((_tmp3B7="declaration hides global",_tag_dyneither(_tmp3B7,sizeof(char),25))),_tag_dyneither(_tmp3B6,sizeof(void*),0)));}goto _LL60;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE2=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE2->tag != 0)goto _LL67;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpE2->f1);if(_tmpE3->tag != 5)goto _LL67;}}_LL66:
{const char*_tmp3BA;void*_tmp3B9;(_tmp3B9=0,Cyc_Tcutil_warn(loc,((_tmp3BA="declaration hides pattern variable",_tag_dyneither(_tmp3BA,sizeof(char),35))),_tag_dyneither(_tmp3B9,sizeof(void*),0)));}goto _LL60;_LL67: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpDD;if(_tmpE4->tag != 0)goto _LL69;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpE4->f1);if(_tmpE5->tag != 3)goto _LL69;}}_LL68:
{const char*_tmp3BD;void*_tmp3BC;(_tmp3BC=0,Cyc_Tcutil_warn(loc,((_tmp3BD="declaration hides parameter",_tag_dyneither(_tmp3BD,sizeof(char),28))),_tag_dyneither(_tmp3BC,sizeof(void*),0)));}goto _LL60;_LL69:;_LL6A:
 goto _LL60;_LL60:;}
# 618
;_pop_handler();}else{void*_tmpDB=(void*)_exn_thrown;void*_tmpEF=_tmpDB;void*_tmpF1;_LL6C: {struct Cyc_Dict_Absent_exn_struct*_tmpF0=(struct Cyc_Dict_Absent_exn_struct*)_tmpEF;if(_tmpF0->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D:
# 626
 goto _LL6B;_LL6E: _tmpF1=_tmpEF;_LL6F:(void)_throw(_tmpF1);_LL6B:;}};}{
const char*_tmp3BE;struct Cyc_Tcenv_Fenv*_stmttmpA=
# 629
Cyc_Tcenv_get_fenv(te,((_tmp3BE="add_local_var",_tag_dyneither(_tmp3BE,sizeof(char),14))));
# 627
struct Cyc_Tcenv_SharedFenv*_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_RgnOrder_RgnPO*_tmpF5;const struct Cyc_Tcenv_Bindings*_tmpF6;struct Cyc_Absyn_Stmt*_tmpF7;struct Cyc_Tcenv_CtrlEnv*_tmpF8;void*_tmpF9;void*_tmpFA;struct Cyc_Tcenv_FenvFlags _tmpFB;struct Cyc_Tcenv_Fenv*_tmpF2=_stmttmpA;_tmpF3=_tmpF2->shared;_tmpF4=_tmpF2->type_vars;_tmpF5=_tmpF2->region_order;_tmpF6=_tmpF2->locals;_tmpF7=_tmpF2->encloser;_tmpF8=_tmpF2->ctrl_env;_tmpF9=_tmpF2->capability;_tmpFA=_tmpF2->curr_rgn;_tmpFB=_tmpF2->flags;{
# 630
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp3C1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp3C0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFC=(_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0[0]=((_tmp3C1.tag=4,((_tmp3C1.f1=vd,_tmp3C1)))),_tmp3C0)));
struct Cyc_Tcenv_Bindings*_tmp3C2;struct Cyc_Tcenv_Bindings*_tmpFD=(_tmp3C2=_region_malloc(r,sizeof(*_tmp3C2)),((_tmp3C2->v=_tmpD5,((_tmp3C2->b=(void*)_tmpFC,((_tmp3C2->tl=_tmpF6,_tmp3C2)))))));
struct Cyc_Tcenv_Fenv*_tmp3C3;struct Cyc_Tcenv_Fenv*_tmpFE=(_tmp3C3=_region_malloc(r,sizeof(*_tmp3C3)),((_tmp3C3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF3,((_tmp3C3->type_vars=(struct Cyc_List_List*)_tmpF4,((_tmp3C3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpF5,((_tmp3C3->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmpFD),((_tmp3C3->encloser=(struct Cyc_Absyn_Stmt*)_tmpF7,((_tmp3C3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpF8,((_tmp3C3->capability=(void*)_tmpF9,((_tmp3C3->curr_rgn=(void*)_tmpFA,((_tmp3C3->flags=(struct Cyc_Tcenv_FenvFlags)_tmpFB,((_tmp3C3->fnrgn=(struct _RegionHandle*)r,_tmp3C3)))))))))))))))))))));
# 634
return Cyc_Tcenv_put_fenv(r,te,_tmpFE);};};}
# 637
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 639
struct Cyc_Tcenv_Fenv*_tmp104=te->le;
if(_tmp104 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpB=*_tmp104;struct Cyc_Tcenv_SharedFenv*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_RgnOrder_RgnPO*_tmp108;const struct Cyc_Tcenv_Bindings*_tmp109;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Tcenv_CtrlEnv*_tmp10B;void*_tmp10C;void*_tmp10D;int _tmp10E;int _tmp10F;int _tmp110;struct Cyc_Tcenv_Fenv _tmp105=_stmttmpB;_tmp106=_tmp105.shared;_tmp107=_tmp105.type_vars;_tmp108=_tmp105.region_order;_tmp109=_tmp105.locals;_tmp10A=_tmp105.encloser;_tmp10B=_tmp105.ctrl_env;_tmp10C=_tmp105.capability;_tmp10D=_tmp105.curr_rgn;_tmp10E=(_tmp105.flags).in_notreadctxt;_tmp10F=(_tmp105.flags).in_lhs;_tmp110=(_tmp105.flags).abstract_ok;{
# 644
struct Cyc_Tcenv_Fenv*_tmp3C4;struct Cyc_Tcenv_Fenv*_tmp111=(_tmp3C4=_region_malloc(r,sizeof(*_tmp3C4)),((_tmp3C4->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp106,((_tmp3C4->type_vars=(struct Cyc_List_List*)_tmp107,((_tmp3C4->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp108,((_tmp3C4->locals=(const struct Cyc_Tcenv_Bindings*)_tmp109,((_tmp3C4->encloser=(struct Cyc_Absyn_Stmt*)_tmp10A,((_tmp3C4->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp10B,((_tmp3C4->capability=(void*)_tmp10C,((_tmp3C4->curr_rgn=(void*)_tmp10D,((_tmp3C4->flags=(struct Cyc_Tcenv_FenvFlags)(
# 646
((_tmp3C4->flags).in_notreadctxt=_tmp10E,(((_tmp3C4->flags).in_lhs=_tmp10F,(((_tmp3C4->flags).in_new=(int)status,(((_tmp3C4->flags).abstract_ok=_tmp110,_tmp3C4->flags)))))))),((_tmp3C4->fnrgn=(struct _RegionHandle*)r,_tmp3C4)))))))))))))))))))));
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
struct Cyc_Tcenv_Fenv*_tmp3C5;struct Cyc_Tcenv_Fenv*_tmp126=(_tmp3C5=_region_malloc(r,sizeof(*_tmp3C5)),((_tmp3C5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp11B,((_tmp3C5->type_vars=(struct Cyc_List_List*)_tmp11C,((_tmp3C5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp11D,((_tmp3C5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp11E,((_tmp3C5->encloser=(struct Cyc_Absyn_Stmt*)_tmp11F,((_tmp3C5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp120,((_tmp3C5->capability=(void*)_tmp121,((_tmp3C5->curr_rgn=(void*)_tmp122,((_tmp3C5->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp3C5->flags).in_notreadctxt=_tmp123,(((_tmp3C5->flags).in_lhs=_tmp124,(((_tmp3C5->flags).in_new=_tmp125,(((_tmp3C5->flags).abstract_ok=1,_tmp3C5->flags)))))))),((_tmp3C5->fnrgn=(struct _RegionHandle*)r,_tmp3C5)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp126);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 679
struct Cyc_Tcenv_Fenv*_tmp128=te->le;
if(_tmp128 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmpF=*_tmp128;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_RgnOrder_RgnPO*_tmp12C;const struct Cyc_Tcenv_Bindings*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Tcenv_CtrlEnv*_tmp12F;void*_tmp130;void*_tmp131;int _tmp132;int _tmp133;int _tmp134;struct Cyc_Tcenv_Fenv _tmp129=_stmttmpF;_tmp12A=_tmp129.shared;_tmp12B=_tmp129.type_vars;_tmp12C=_tmp129.region_order;_tmp12D=_tmp129.locals;_tmp12E=_tmp129.encloser;_tmp12F=_tmp129.ctrl_env;_tmp130=_tmp129.capability;_tmp131=_tmp129.curr_rgn;_tmp132=(_tmp129.flags).in_notreadctxt;_tmp133=(_tmp129.flags).in_lhs;_tmp134=(_tmp129.flags).in_new;{
# 684
struct Cyc_Tcenv_Fenv*_tmp3C6;struct Cyc_Tcenv_Fenv*_tmp135=(_tmp3C6=_region_malloc(r,sizeof(*_tmp3C6)),((_tmp3C6->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A,((_tmp3C6->type_vars=(struct Cyc_List_List*)_tmp12B,((_tmp3C6->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12C,((_tmp3C6->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12D,((_tmp3C6->encloser=(struct Cyc_Absyn_Stmt*)_tmp12E,((_tmp3C6->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12F,((_tmp3C6->capability=(void*)_tmp130,((_tmp3C6->curr_rgn=(void*)_tmp131,((_tmp3C6->flags=(struct Cyc_Tcenv_FenvFlags)(
# 686
((_tmp3C6->flags).in_notreadctxt=_tmp132,(((_tmp3C6->flags).in_lhs=_tmp133,(((_tmp3C6->flags).in_new=_tmp134,(((_tmp3C6->flags).abstract_ok=0,_tmp3C6->flags)))))))),((_tmp3C6->fnrgn=(struct _RegionHandle*)r,_tmp3C6)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};}
# 690
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp137=te->le;
if(_tmp137 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp10=*_tmp137;struct Cyc_Tcenv_SharedFenv*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_RgnOrder_RgnPO*_tmp13B;const struct Cyc_Tcenv_Bindings*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13D;struct Cyc_Tcenv_CtrlEnv*_tmp13E;void*_tmp13F;void*_tmp140;int _tmp141;int _tmp142;int _tmp143;struct Cyc_Tcenv_Fenv _tmp138=_stmttmp10;_tmp139=_tmp138.shared;_tmp13A=_tmp138.type_vars;_tmp13B=_tmp138.region_order;_tmp13C=_tmp138.locals;_tmp13D=_tmp138.encloser;_tmp13E=_tmp138.ctrl_env;_tmp13F=_tmp138.capability;_tmp140=_tmp138.curr_rgn;_tmp141=(_tmp138.flags).in_lhs;_tmp142=(_tmp138.flags).in_new;_tmp143=(_tmp138.flags).abstract_ok;{
# 696
struct Cyc_Tcenv_Fenv*_tmp3C7;struct Cyc_Tcenv_Fenv*_tmp144=(_tmp3C7=_region_malloc(r,sizeof(*_tmp3C7)),((_tmp3C7->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp139,((_tmp3C7->type_vars=(struct Cyc_List_List*)_tmp13A,((_tmp3C7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13B,((_tmp3C7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13C,((_tmp3C7->encloser=(struct Cyc_Absyn_Stmt*)_tmp13D,((_tmp3C7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13E,((_tmp3C7->capability=(void*)_tmp13F,((_tmp3C7->curr_rgn=(void*)_tmp140,((_tmp3C7->flags=(struct Cyc_Tcenv_FenvFlags)(
# 698
((_tmp3C7->flags).in_notreadctxt=1,(((_tmp3C7->flags).in_lhs=_tmp141,(((_tmp3C7->flags).in_new=_tmp142,(((_tmp3C7->flags).abstract_ok=_tmp143,_tmp3C7->flags)))))))),((_tmp3C7->fnrgn=(struct _RegionHandle*)r,_tmp3C7)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp144);};};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp146=te->le;
if(_tmp146 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp11=*_tmp146;struct Cyc_Tcenv_SharedFenv*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_RgnOrder_RgnPO*_tmp14A;const struct Cyc_Tcenv_Bindings*_tmp14B;struct Cyc_Absyn_Stmt*_tmp14C;struct Cyc_Tcenv_CtrlEnv*_tmp14D;void*_tmp14E;void*_tmp14F;int _tmp150;int _tmp151;int _tmp152;struct Cyc_Tcenv_Fenv _tmp147=_stmttmp11;_tmp148=_tmp147.shared;_tmp149=_tmp147.type_vars;_tmp14A=_tmp147.region_order;_tmp14B=_tmp147.locals;_tmp14C=_tmp147.encloser;_tmp14D=_tmp147.ctrl_env;_tmp14E=_tmp147.capability;_tmp14F=_tmp147.curr_rgn;_tmp150=(_tmp147.flags).in_lhs;_tmp151=(_tmp147.flags).in_new;_tmp152=(_tmp147.flags).abstract_ok;{
# 708
struct Cyc_Tcenv_Fenv*_tmp3C8;struct Cyc_Tcenv_Fenv*_tmp153=(_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp148,((_tmp3C8->type_vars=(struct Cyc_List_List*)_tmp149,((_tmp3C8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14A,((_tmp3C8->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14B,((_tmp3C8->encloser=(struct Cyc_Absyn_Stmt*)_tmp14C,((_tmp3C8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14D,((_tmp3C8->capability=(void*)_tmp14E,((_tmp3C8->curr_rgn=(void*)_tmp14F,((_tmp3C8->flags=(struct Cyc_Tcenv_FenvFlags)(
# 710
((_tmp3C8->flags).in_notreadctxt=0,(((_tmp3C8->flags).in_lhs=_tmp150,(((_tmp3C8->flags).in_new=_tmp151,(((_tmp3C8->flags).abstract_ok=_tmp152,_tmp3C8->flags)))))))),((_tmp3C8->fnrgn=(struct _RegionHandle*)r,_tmp3C8)))))))))))))))))))));
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
struct Cyc_Tcenv_Fenv*_tmp3C9;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp3C9=_region_malloc(r,sizeof(*_tmp3C9)),((_tmp3C9->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp3C9->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp3C9->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp3C9->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp3C9->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp3C9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp3C9->capability=(void*)_tmp160,((_tmp3C9->curr_rgn=(void*)_tmp161,((_tmp3C9->flags=(struct Cyc_Tcenv_FenvFlags)(
# 729
((_tmp3C9->flags).in_notreadctxt=_tmp162,(((_tmp3C9->flags).in_lhs=1,(((_tmp3C9->flags).in_new=_tmp163,(((_tmp3C9->flags).abstract_ok=_tmp164,_tmp3C9->flags)))))))),((_tmp3C9->fnrgn=(struct _RegionHandle*)r,_tmp3C9)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 733
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _stmttmp14=*_tmp167;struct Cyc_Tcenv_SharedFenv*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_RgnOrder_RgnPO*_tmp16B;const struct Cyc_Tcenv_Bindings*_tmp16C;struct Cyc_Absyn_Stmt*_tmp16D;struct Cyc_Tcenv_CtrlEnv*_tmp16E;void*_tmp16F;void*_tmp170;int _tmp171;int _tmp172;int _tmp173;struct Cyc_Tcenv_Fenv _tmp168=_stmttmp14;_tmp169=_tmp168.shared;_tmp16A=_tmp168.type_vars;_tmp16B=_tmp168.region_order;_tmp16C=_tmp168.locals;_tmp16D=_tmp168.encloser;_tmp16E=_tmp168.ctrl_env;_tmp16F=_tmp168.capability;_tmp170=_tmp168.curr_rgn;_tmp171=(_tmp168.flags).in_notreadctxt;_tmp172=(_tmp168.flags).in_new;_tmp173=(_tmp168.flags).abstract_ok;{
# 739
struct Cyc_Tcenv_Fenv*_tmp3CA;struct Cyc_Tcenv_Fenv*_tmp174=(_tmp3CA=_region_malloc(r,sizeof(*_tmp3CA)),((_tmp3CA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp169,((_tmp3CA->type_vars=(struct Cyc_List_List*)_tmp16A,((_tmp3CA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp16B,((_tmp3CA->locals=(const struct Cyc_Tcenv_Bindings*)_tmp16C,((_tmp3CA->encloser=(struct Cyc_Absyn_Stmt*)_tmp16D,((_tmp3CA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp16E,((_tmp3CA->capability=(void*)_tmp16F,((_tmp3CA->curr_rgn=(void*)_tmp170,((_tmp3CA->flags=(struct Cyc_Tcenv_FenvFlags)(
# 741
((_tmp3CA->flags).in_notreadctxt=_tmp171,(((_tmp3CA->flags).in_lhs=0,(((_tmp3CA->flags).in_new=_tmp172,(((_tmp3CA->flags).abstract_ok=_tmp173,_tmp3CA->flags)))))))),((_tmp3CA->fnrgn=(struct _RegionHandle*)r,_tmp3CA)))))))))))))))))))));
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
{const char*_tmp3CD;void*_tmp3CC;(_tmp3CC=0,Cyc_Tcutil_warn(loc,((_tmp3CD="pattern variable hides local",_tag_dyneither(_tmp3CD,sizeof(char),29))),_tag_dyneither(_tmp3CC,sizeof(void*),0)));}goto _LL70;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp17F=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp17F->tag != 0)goto _LL75;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp180=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp17F->f1);if(_tmp180->tag != 1)goto _LL75;}}_LL74:
{const char*_tmp3D0;void*_tmp3CF;(_tmp3CF=0,Cyc_Tcutil_warn(loc,((_tmp3D0="pattern variable hides global",_tag_dyneither(_tmp3D0,sizeof(char),30))),_tag_dyneither(_tmp3CF,sizeof(void*),0)));}goto _LL70;_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp181=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp181->tag != 0)goto _LL77;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp182=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp181->f1);if(_tmp182->tag != 5)goto _LL77;}}_LL76:
{const char*_tmp3D3;void*_tmp3D2;(_tmp3D2=0,Cyc_Tcutil_warn(loc,((_tmp3D3="pattern variable hides pattern variable",_tag_dyneither(_tmp3D3,sizeof(char),40))),_tag_dyneither(_tmp3D2,sizeof(void*),0)));}goto _LL70;_LL77: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp183=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp17C;if(_tmp183->tag != 0)goto _LL79;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp184=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp183->f1);if(_tmp184->tag != 3)goto _LL79;}}_LL78:
{const char*_tmp3D6;void*_tmp3D5;(_tmp3D5=0,Cyc_Tcutil_warn(loc,((_tmp3D6="pattern variable hides parameter",_tag_dyneither(_tmp3D6,sizeof(char),33))),_tag_dyneither(_tmp3D5,sizeof(void*),0)));}goto _LL70;_LL79:;_LL7A:
 goto _LL70;_LL70:;}
# 757
;_pop_handler();}else{void*_tmp17A=(void*)_exn_thrown;void*_tmp18E=_tmp17A;void*_tmp190;_LL7C: {struct Cyc_Dict_Absent_exn_struct*_tmp18F=(struct Cyc_Dict_Absent_exn_struct*)_tmp18E;if(_tmp18F->tag != Cyc_Dict_Absent)goto _LL7E;}_LL7D:
# 765
 goto _LL7B;_LL7E: _tmp190=_tmp18E;_LL7F:(void)_throw(_tmp190);_LL7B:;}};}{
struct _dyneither_ptr*_tmp191=(*vd->name).f2;
const char*_tmp3D7;struct Cyc_Tcenv_Fenv*_stmttmp17=
# 769
Cyc_Tcenv_get_fenv(te,((_tmp3D7="add_pat_var",_tag_dyneither(_tmp3D7,sizeof(char),12))));
# 767
struct Cyc_Tcenv_SharedFenv*_tmp193;struct Cyc_List_List*_tmp194;struct Cyc_RgnOrder_RgnPO*_tmp195;const struct Cyc_Tcenv_Bindings*_tmp196;struct Cyc_Absyn_Stmt*_tmp197;struct Cyc_Tcenv_CtrlEnv*_tmp198;void*_tmp199;void*_tmp19A;struct Cyc_Tcenv_FenvFlags _tmp19B;struct Cyc_Tcenv_Fenv*_tmp192=_stmttmp17;_tmp193=_tmp192->shared;_tmp194=_tmp192->type_vars;_tmp195=_tmp192->region_order;_tmp196=_tmp192->locals;_tmp197=_tmp192->encloser;_tmp198=_tmp192->ctrl_env;_tmp199=_tmp192->capability;_tmp19A=_tmp192->curr_rgn;_tmp19B=_tmp192->flags;{
# 770
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp3DA;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp3D9;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19C=(_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DA.tag=5,((_tmp3DA.f1=vd,_tmp3DA)))),_tmp3D9)));
struct Cyc_Tcenv_Bindings*_tmp3DB;struct Cyc_Tcenv_Bindings*_tmp19D=(_tmp3DB=_region_malloc(r,sizeof(*_tmp3DB)),((_tmp3DB->v=_tmp191,((_tmp3DB->b=(void*)_tmp19C,((_tmp3DB->tl=_tmp196,_tmp3DB)))))));
struct Cyc_Tcenv_Fenv*_tmp3DC;struct Cyc_Tcenv_Fenv*_tmp19E=(_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp193,((_tmp3DC->type_vars=(struct Cyc_List_List*)_tmp194,((_tmp3DC->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp195,((_tmp3DC->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp19D),((_tmp3DC->encloser=(struct Cyc_Absyn_Stmt*)_tmp197,((_tmp3DC->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp198,((_tmp3DC->capability=(void*)_tmp199,((_tmp3DC->curr_rgn=(void*)_tmp19A,((_tmp3DC->flags=(struct Cyc_Tcenv_FenvFlags)_tmp19B,((_tmp3DC->fnrgn=(struct _RegionHandle*)r,_tmp3DC)))))))))))))))))))));
# 774
return Cyc_Tcenv_put_fenv(r,te,_tmp19E);};};}
# 777
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp1A4=te->le;
union Cyc_Absyn_Nmspace _tmp1A6;struct _dyneither_ptr*_tmp1A7;struct _tuple0*_tmp1A5=q;_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;{
union Cyc_Absyn_Nmspace _tmp1A8=_tmp1A6;_LL81: if((_tmp1A8.Loc_n).tag != 4)goto _LL83;_LL82:
# 782
 if(_tmp1A4 == 0){struct Cyc_Dict_Absent_exn_struct _tmp3DF;struct Cyc_Dict_Absent_exn_struct*_tmp3DE;(int)_throw((void*)((_tmp3DE=_cycalloc_atomic(sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3DF.tag=Cyc_Dict_Absent,_tmp3DF)),_tmp3DE)))));}
goto _LL84;_LL83: if((_tmp1A8.Rel_n).tag != 1)goto _LL85;if((struct Cyc_List_List*)(_tmp1A8.Rel_n).val != 0)goto _LL85;if(!(_tmp1A4 != 0))goto _LL85;_LL84: {
# 785
struct Cyc_Tcenv_Fenv*_stmttmp18=_tmp1A4;const struct Cyc_Tcenv_Bindings*_tmp1AC;struct Cyc_Tcenv_Fenv*_tmp1AB=_stmttmp18;_tmp1AC=_tmp1AB->locals;{
# 787
struct _handler_cons _tmp1AD;_push_handler(& _tmp1AD);{int _tmp1AF=0;if(setjmp(_tmp1AD.handler))_tmp1AF=1;if(!_tmp1AF){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3E2;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3E1;void*_tmp1B2=(void*)((_tmp3E1=_region_malloc(r,sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E2.tag=0,((_tmp3E2.f1=Cyc_Tcenv_lookup_binding(_tmp1AC,_tmp1A7),_tmp3E2)))),_tmp3E1))));_npop_handler(0);return _tmp1B2;};_pop_handler();}else{void*_tmp1AE=(void*)_exn_thrown;void*_tmp1B4=_tmp1AE;void*_tmp1B6;_LL88: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp1B5=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1B4;if(_tmp1B5->tag != Cyc_Tcenv_NoBinding)goto _LL8A;}_LL89:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL8A: _tmp1B6=_tmp1B4;_LL8B:(void)_throw(_tmp1B6);_LL87:;}};};}_LL85:;_LL86: {
# 791
struct _handler_cons _tmp1B7;_push_handler(& _tmp1B7);{int _tmp1B9=0;if(setjmp(_tmp1B7.handler))_tmp1B9=1;if(!_tmp1B9){{void*_tmp1BA=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1BA;};_pop_handler();}else{void*_tmp1B8=(void*)_exn_thrown;void*_tmp1BC=_tmp1B8;void*_tmp1BE;_LL8D: {struct Cyc_Dict_Absent_exn_struct*_tmp1BD=(struct Cyc_Dict_Absent_exn_struct*)_tmp1BC;if(_tmp1BD->tag != Cyc_Dict_Absent)goto _LL8F;}_LL8E: {
# 795
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp3E5;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp3E4;return(void*)((_tmp3E4=_region_malloc(r,sizeof(*_tmp3E4)),((_tmp3E4[0]=((_tmp3E5.tag=0,((_tmp3E5.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp3E5)))),_tmp3E4))));}_LL8F: _tmp1BE=_tmp1BC;_LL90:(void)_throw(_tmp1BE);_LL8C:;}};}_LL80:;};}
# 800
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3E6;struct Cyc_Tcenv_Fenv*_stmttmp19=
Cyc_Tcenv_get_fenv(te,((_tmp3E6="process_continue",_tag_dyneither(_tmp3E6,sizeof(char),17))));
# 801
struct Cyc_Tcenv_CtrlEnv*_tmp1C2;struct Cyc_Tcenv_Fenv*_tmp1C1=_stmttmp19;_tmp1C2=_tmp1C1->ctrl_env;{
# 803
void*_stmttmp1A=_tmp1C2->continue_stmt;void*_tmp1C3=_stmttmp1A;struct Cyc_Absyn_Stmt*_tmp1C5;_LL92: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1C4=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C4->tag != 3)goto _LL94;else{_tmp1C5=_tmp1C4->f1;}}_LL93:
# 805
{struct Cyc_List_List*_tmp3E7;_tmp1C5->non_local_preds=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=s,((_tmp3E7->tl=_tmp1C5->non_local_preds,_tmp3E7))))));}
*sopt=_tmp1C5;
return;_LL94: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1C6=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C6->tag != 0)goto _LL96;}_LL95:
{const char*_tmp3EA;void*_tmp3E9;(_tmp3E9=0,Cyc_Tcutil_terr(s->loc,((_tmp3EA="continue not in a loop",_tag_dyneither(_tmp3EA,sizeof(char),23))),_tag_dyneither(_tmp3E9,sizeof(void*),0)));}return;_LL96: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1C7=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C7->tag != 1)goto _LL98;}_LL97:
 goto _LL99;_LL98: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1C8=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1C3;if(_tmp1C8->tag != 2)goto _LL91;}_LL99: {
const char*_tmp3EB;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3EB="bad continue destination",_tag_dyneither(_tmp3EB,sizeof(char),25))));}_LL91:;};}
# 813
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3EC;struct Cyc_Tcenv_Fenv*_stmttmp1B=
Cyc_Tcenv_get_fenv(te,((_tmp3EC="process_break",_tag_dyneither(_tmp3EC,sizeof(char),14))));
# 814
struct Cyc_Tcenv_CtrlEnv*_tmp1CF;struct Cyc_Tcenv_SharedFenv*_tmp1D0;struct Cyc_Tcenv_Fenv*_tmp1CE=_stmttmp1B;_tmp1CF=_tmp1CE->ctrl_env;_tmp1D0=_tmp1CE->shared;{
# 816
void*_stmttmp1C=_tmp1CF->break_stmt;void*_tmp1D1=_stmttmp1C;struct Cyc_Absyn_Stmt*_tmp1D3;_LL9B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1D2=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D2->tag != 3)goto _LL9D;else{_tmp1D3=_tmp1D2->f1;}}_LL9C:
# 818
{struct Cyc_List_List*_tmp3ED;_tmp1D3->non_local_preds=((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=s,((_tmp3ED->tl=_tmp1D3->non_local_preds,_tmp3ED))))));}
*sopt=_tmp1D3;
return;_LL9D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1D4=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D4->tag != 0)goto _LL9F;}_LL9E:
# 822
{const char*_tmp3F0;void*_tmp3EF;(_tmp3EF=0,Cyc_Tcutil_terr(s->loc,((_tmp3F0="break not in a loop or switch",_tag_dyneither(_tmp3F0,sizeof(char),30))),_tag_dyneither(_tmp3EF,sizeof(void*),0)));}
return;_LL9F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1D5=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D5->tag != 2)goto _LLA1;}_LLA0:
# 828
{void*_stmttmp1D=Cyc_Tcutil_compress(_tmp1D0->return_typ);void*_tmp1DA=_stmttmp1D;_LLA4: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1DA;if(_tmp1DB->tag != 0)goto _LLA6;}_LLA5:
 goto _LLA3;_LLA6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1DA;if(_tmp1DC->tag != 7)goto _LLA8;}_LLA7:
 goto _LLA9;_LLA8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1DA;if(_tmp1DD->tag != 6)goto _LLAA;}_LLA9:
# 832
{const char*_tmp3F3;void*_tmp3F2;(_tmp3F2=0,Cyc_Tcutil_warn(s->loc,((_tmp3F3="break may cause function not to return a value",_tag_dyneither(_tmp3F3,sizeof(char),47))),_tag_dyneither(_tmp3F2,sizeof(void*),0)));}
goto _LLA3;_LLAA:;_LLAB:
# 835
{const char*_tmp3F6;void*_tmp3F5;(_tmp3F5=0,Cyc_Tcutil_terr(s->loc,((_tmp3F6="break may cause function not to return a value",_tag_dyneither(_tmp3F6,sizeof(char),47))),_tag_dyneither(_tmp3F5,sizeof(void*),0)));}
goto _LLA3;_LLA3:;}
# 838
return;_LLA1: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1D6=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1D1;if(_tmp1D6->tag != 1)goto _LL9A;}_LLA2:
# 841
{const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,Cyc_Tcutil_terr(s->loc,((_tmp3F9="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp3F9,sizeof(char),56))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}
return;_LL9A:;};}
# 845
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp3FA;struct Cyc_Tcenv_Fenv*_stmttmp1E=Cyc_Tcenv_get_fenv(te,((_tmp3FA="process_goto",_tag_dyneither(_tmp3FA,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1E6;struct Cyc_Tcenv_Fenv*_tmp1E5=_stmttmp1E;_tmp1E6=_tmp1E5->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1E6->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1E7=_tmp1E6->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1E7,l);
struct _RegionHandle*frgn=_tmp1E6->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp3FB;slopt=((_tmp3FB=_region_malloc(frgn,sizeof(*_tmp3FB)),((_tmp3FB[0]=0,_tmp3FB))));}{
struct Cyc_List_List*_tmp3FC;struct Cyc_List_List*new_needed=(_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->hd=s,((_tmp3FC->tl=*slopt,_tmp3FC)))));
_tmp1E6->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1E7,l,new_needed);};}else{
# 857
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp3FD;s->non_local_preds=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD->hd=s,((_tmp3FD->tl=s->non_local_preds,_tmp3FD))))));}
*sopt=s;}};}
# 863
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 865
const char*_tmp3FE;struct Cyc_Tcenv_Fenv*_stmttmp1F=Cyc_Tcenv_get_fenv(te,((_tmp3FE="process_fallthru",_tag_dyneither(_tmp3FE,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp1ED;struct Cyc_Tcenv_Fenv*_tmp1EC=_stmttmp1F;_tmp1ED=_tmp1EC->ctrl_env;{
const struct _tuple10*_tmp1EE=_tmp1ED->fallthru_clause;
if(_tmp1EE != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp3FF;(((*_tmp1EE).f1)->body)->non_local_preds=(
(_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF->hd=s,((_tmp3FF->tl=(((*_tmp1EE).f1)->body)->non_local_preds,_tmp3FF))))));}{
struct Cyc_Absyn_Switch_clause**_tmp400;*clauseopt=((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400[0]=(*_tmp1EE).f1,_tmp400))));};}
# 872
return _tmp1EE;};}
# 875
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 879
const char*_tmp401;struct Cyc_Tcenv_Fenv*_stmttmp20=
# 881
Cyc_Tcenv_get_fenv(te,((_tmp401="set_fallthru",_tag_dyneither(_tmp401,sizeof(char),13))));
# 879
struct Cyc_Tcenv_SharedFenv*_tmp1F3;struct Cyc_List_List*_tmp1F4;struct Cyc_RgnOrder_RgnPO*_tmp1F5;const struct Cyc_Tcenv_Bindings*_tmp1F6;struct Cyc_Absyn_Stmt*_tmp1F7;struct Cyc_Tcenv_CtrlEnv*_tmp1F8;void*_tmp1F9;void*_tmp1FA;struct Cyc_Tcenv_FenvFlags _tmp1FB;struct Cyc_Tcenv_Fenv*_tmp1F2=_stmttmp20;_tmp1F3=_tmp1F2->shared;_tmp1F4=_tmp1F2->type_vars;_tmp1F5=_tmp1F2->region_order;_tmp1F6=_tmp1F2->locals;_tmp1F7=_tmp1F2->encloser;_tmp1F8=_tmp1F2->ctrl_env;_tmp1F9=_tmp1F2->capability;_tmp1FA=_tmp1F2->curr_rgn;_tmp1FB=_tmp1F2->flags;{
# 882
struct _RegionHandle*_tmp1FD;void*_tmp1FE;void*_tmp1FF;const struct _tuple10*_tmp200;void*_tmp201;int _tmp202;struct Cyc_Tcenv_CtrlEnv*_tmp1FC=_tmp1F8;_tmp1FD=_tmp1FC->ctrl_rgn;_tmp1FE=_tmp1FC->continue_stmt;_tmp1FF=_tmp1FC->break_stmt;_tmp200=_tmp1FC->fallthru_clause;_tmp201=_tmp1FC->next_stmt;_tmp202=_tmp1FC->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp402;ft_typ=((_tmp402=_region_malloc(_tmp1FD,sizeof(*_tmp402)),((_tmp402->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp402->tl=ft_typ,_tmp402))))));}{
const struct Cyc_Tcenv_CList*_tmp204=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp405;struct Cyc_Tcenv_CtrlEnv*_tmp404;struct Cyc_Tcenv_CtrlEnv*_tmp205=
(_tmp404=_region_malloc(r,sizeof(*_tmp404)),((_tmp404->ctrl_rgn=_tmp1FD,((_tmp404->continue_stmt=_tmp1FE,((_tmp404->break_stmt=_tmp1FF,((_tmp404->fallthru_clause=(const struct _tuple10*)((_tmp405=_region_malloc(_tmp1FD,sizeof(*_tmp405)),((_tmp405->f1=clause,((_tmp405->f2=new_tvs,((_tmp405->f3=_tmp204,_tmp405)))))))),((_tmp404->next_stmt=_tmp201,((_tmp404->try_depth=_tmp202,_tmp404)))))))))))));
# 890
struct Cyc_Tcenv_Fenv*_tmp406;struct Cyc_Tcenv_Fenv*_tmp206=(_tmp406=_region_malloc(r,sizeof(*_tmp406)),((_tmp406->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1F3,((_tmp406->type_vars=(struct Cyc_List_List*)_tmp1F4,((_tmp406->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1F5,((_tmp406->locals=(const struct Cyc_Tcenv_Bindings*)_tmp1F6,((_tmp406->encloser=(struct Cyc_Absyn_Stmt*)_tmp1F7,((_tmp406->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp205,((_tmp406->capability=(void*)_tmp1F9,((_tmp406->curr_rgn=(void*)_tmp1FA,((_tmp406->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1FB,((_tmp406->fnrgn=(struct _RegionHandle*)r,_tmp406)))))))))))))))))))));
# 894
return Cyc_Tcenv_put_fenv(r,te,_tmp206);};};};}
# 897
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp407;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp407="clear_fallthru",_tag_dyneither(_tmp407,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp20C;struct Cyc_Tcenv_Fenv*_tmp20B=fe;_tmp20C=_tmp20B->ctrl_env;
_tmp20C->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 904
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 906
const char*_tmp408;struct Cyc_Tcenv_Fenv*_stmttmp21=
# 908
Cyc_Tcenv_get_fenv(te,((_tmp408="set_in_loop",_tag_dyneither(_tmp408,sizeof(char),12))));
# 906
struct Cyc_Tcenv_SharedFenv*_tmp20F;struct Cyc_List_List*_tmp210;struct Cyc_RgnOrder_RgnPO*_tmp211;const struct Cyc_Tcenv_Bindings*_tmp212;struct Cyc_Absyn_Stmt*_tmp213;struct Cyc_Tcenv_CtrlEnv*_tmp214;void*_tmp215;void*_tmp216;struct Cyc_Tcenv_FenvFlags _tmp217;struct Cyc_Tcenv_Fenv*_tmp20E=_stmttmp21;_tmp20F=_tmp20E->shared;_tmp210=_tmp20E->type_vars;_tmp211=_tmp20E->region_order;_tmp212=_tmp20E->locals;_tmp213=_tmp20E->encloser;_tmp214=_tmp20E->ctrl_env;_tmp215=_tmp20E->capability;_tmp216=_tmp20E->curr_rgn;_tmp217=_tmp20E->flags;{
# 909
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp413;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp412;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp411;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp410;struct Cyc_Tcenv_CtrlEnv*_tmp40F;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp40F=_region_malloc(r,sizeof(*_tmp40F)),((_tmp40F->ctrl_rgn=r,((_tmp40F->continue_stmt=(void*)(
(_tmp411=_region_malloc(r,sizeof(*_tmp411)),((_tmp411[0]=((_tmp410.tag=3,((_tmp410.f1=continue_dest,_tmp410)))),_tmp411)))),((_tmp40F->break_stmt=_tmp214->next_stmt,((_tmp40F->next_stmt=(void*)(
# 913
(_tmp413=_region_malloc(r,sizeof(*_tmp413)),((_tmp413[0]=((_tmp412.tag=3,((_tmp412.f1=continue_dest,_tmp412)))),_tmp413)))),((_tmp40F->fallthru_clause=_tmp214->fallthru_clause,((_tmp40F->try_depth=_tmp214->try_depth,_tmp40F)))))))))))));
# 916
struct Cyc_Tcenv_Fenv*_tmp414;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp414=_region_malloc(r,sizeof(*_tmp414)),((_tmp414->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20F,((_tmp414->type_vars=(struct Cyc_List_List*)_tmp210,((_tmp414->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp211,((_tmp414->locals=(const struct Cyc_Tcenv_Bindings*)_tmp212,((_tmp414->encloser=(struct Cyc_Absyn_Stmt*)_tmp213,((_tmp414->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp414->capability=(void*)_tmp215,((_tmp414->curr_rgn=(void*)_tmp216,((_tmp414->flags=(struct Cyc_Tcenv_FenvFlags)_tmp217,((_tmp414->fnrgn=(struct _RegionHandle*)r,_tmp414)))))))))))))))))))));
# 919
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 922
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp415;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp415="enter_try",_tag_dyneither(_tmp415,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp220;struct Cyc_Tcenv_Fenv*_tmp21F=fe;_tmp220=_tmp21F->ctrl_env;
++ _tmp220->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 928
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp416;struct Cyc_Tcenv_Fenv*_stmttmp22=Cyc_Tcenv_get_fenv(te,((_tmp416="get_try_depth",_tag_dyneither(_tmp416,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp223;struct Cyc_Tcenv_Fenv*_tmp222=_stmttmp22;_tmp223=_tmp222->ctrl_env;
return _tmp223->try_depth;}
# 935
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp417;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp417="set_in_switch",_tag_dyneither(_tmp417,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp226;struct Cyc_Tcenv_Fenv*_tmp225=fe;_tmp226=_tmp225->ctrl_env;
_tmp226->break_stmt=_tmp226->next_stmt;
_tmp226->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 943
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 945
const char*_tmp418;struct Cyc_Tcenv_Fenv _stmttmp23=*
# 947
Cyc_Tcenv_get_fenv(te,((_tmp418="set_next",_tag_dyneither(_tmp418,sizeof(char),9))));
# 945
struct Cyc_Tcenv_SharedFenv*_tmp229;struct Cyc_List_List*_tmp22A;struct Cyc_RgnOrder_RgnPO*_tmp22B;const struct Cyc_Tcenv_Bindings*_tmp22C;struct Cyc_Absyn_Stmt*_tmp22D;struct Cyc_Tcenv_CtrlEnv*_tmp22E;void*_tmp22F;void*_tmp230;struct Cyc_Tcenv_FenvFlags _tmp231;struct Cyc_Tcenv_Fenv _tmp228=_stmttmp23;_tmp229=_tmp228.shared;_tmp22A=_tmp228.type_vars;_tmp22B=_tmp228.region_order;_tmp22C=_tmp228.locals;_tmp22D=_tmp228.encloser;_tmp22E=_tmp228.ctrl_env;_tmp22F=_tmp228.capability;_tmp230=_tmp228.curr_rgn;_tmp231=_tmp228.flags;{
# 948
struct Cyc_Tcenv_CtrlEnv*_tmp419;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp419=_region_malloc(r,sizeof(*_tmp419)),((_tmp419->ctrl_rgn=r,((_tmp419->continue_stmt=_tmp22E->continue_stmt,((_tmp419->break_stmt=_tmp22E->break_stmt,((_tmp419->next_stmt=j,((_tmp419->fallthru_clause=_tmp22E->fallthru_clause,((_tmp419->try_depth=_tmp22E->try_depth,_tmp419)))))))))))));
# 956
struct Cyc_Tcenv_Fenv*_tmp41A;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp41A=_region_malloc(r,sizeof(*_tmp41A)),((_tmp41A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp229,((_tmp41A->type_vars=(struct Cyc_List_List*)_tmp22A,((_tmp41A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22B,((_tmp41A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22C,((_tmp41A->encloser=(struct Cyc_Absyn_Stmt*)_tmp22D,((_tmp41A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp41A->capability=(void*)_tmp22F,((_tmp41A->curr_rgn=(void*)_tmp230,((_tmp41A->flags=(struct Cyc_Tcenv_FenvFlags)_tmp231,((_tmp41A->fnrgn=(struct _RegionHandle*)r,_tmp41A)))))))))))))))))))));
# 959
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 962
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp41B;struct Cyc_Tcenv_Fenv*_stmttmp24=Cyc_Tcenv_get_fenv(te,((_tmp41B="add_label",_tag_dyneither(_tmp41B,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp236;struct Cyc_Tcenv_Fenv*_tmp235=_stmttmp24;_tmp236=_tmp235->shared;{
struct Cyc_Dict_Dict needed=_tmp236->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp236->frgn;
if(sl_opt != 0){
_tmp236->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 970
struct Cyc_List_List*_tmp237=*sl_opt;
s->non_local_preds=_tmp237;
for(0;_tmp237 != 0;_tmp237=_tmp237->tl){
void*_stmttmp25=((struct Cyc_Absyn_Stmt*)_tmp237->hd)->r;void*_tmp238=_stmttmp25;struct Cyc_Absyn_Stmt**_tmp23A;_LLAD: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp239=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp238;if(_tmp239->tag != 8)goto _LLAF;else{_tmp23A=(struct Cyc_Absyn_Stmt**)& _tmp239->f2;}}_LLAE:
*_tmp23A=s;goto _LLAC;_LLAF:;_LLB0:
{const char*_tmp41C;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp41C="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp41C,sizeof(char),42))));}goto _LLAC;_LLAC:;}};}
# 978
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp236->seen_labels,v)){
const char*_tmp420;void*_tmp41F[1];struct Cyc_String_pa_PrintArg_struct _tmp41E;(_tmp41E.tag=0,((_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp41F[0]=& _tmp41E,Cyc_Tcutil_terr(s->loc,((_tmp420="Repeated label: %s",_tag_dyneither(_tmp420,sizeof(char),19))),_tag_dyneither(_tmp41F,sizeof(void*),1)))))));}
_tmp236->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp236->seen_labels,v,s);
return te;};}
# 984
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp421;struct Cyc_Tcenv_Fenv*_stmttmp26=Cyc_Tcenv_get_fenv(te,((_tmp421="all_labels_resolved",_tag_dyneither(_tmp421,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp241;struct Cyc_Tcenv_Fenv*_tmp240=_stmttmp26;_tmp241=_tmp240->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp241->needed_labels);}
# 989
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp422;struct Cyc_Tcenv_Fenv*_stmttmp27=Cyc_Tcenv_get_fenv(te,((_tmp422="get_encloser",_tag_dyneither(_tmp422,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp244;struct Cyc_Tcenv_Fenv*_tmp243=_stmttmp27;_tmp244=_tmp243->encloser;
return _tmp244;}
# 994
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp423;return(_tmp423=_region_malloc(r2,sizeof(*_tmp423)),((_tmp423->ns=te->ns,((_tmp423->ae=te->ae,((_tmp423->le=0,((_tmp423->allow_valueof=1,_tmp423)))))))));}{
struct Cyc_Tcenv_Fenv _stmttmp28=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_RgnOrder_RgnPO*_tmp24A;const struct Cyc_Tcenv_Bindings*_tmp24B;struct Cyc_Absyn_Stmt*_tmp24C;struct Cyc_Tcenv_CtrlEnv*_tmp24D;void*_tmp24E;void*_tmp24F;struct Cyc_Tcenv_FenvFlags _tmp250;struct Cyc_Tcenv_Fenv _tmp247=_stmttmp28;_tmp248=_tmp247.shared;_tmp249=_tmp247.type_vars;_tmp24A=_tmp247.region_order;_tmp24B=_tmp247.locals;_tmp24C=_tmp247.encloser;_tmp24D=_tmp247.ctrl_env;_tmp24E=_tmp247.capability;_tmp24F=_tmp247.curr_rgn;_tmp250=_tmp247.flags;{
struct Cyc_Tcenv_Fenv*_tmp424;struct Cyc_Tcenv_Fenv*_tmp251=(_tmp424=_region_malloc(r2,sizeof(*_tmp424)),((_tmp424->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp248,((_tmp424->type_vars=(struct Cyc_List_List*)_tmp249,((_tmp424->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp24A,((_tmp424->locals=(const struct Cyc_Tcenv_Bindings*)_tmp24B,((_tmp424->encloser=(struct Cyc_Absyn_Stmt*)_tmp24C,((_tmp424->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp24D,((_tmp424->capability=(void*)_tmp24E,((_tmp424->curr_rgn=(void*)_tmp24F,((_tmp424->flags=(struct Cyc_Tcenv_FenvFlags)_tmp250,((_tmp424->fnrgn=(struct _RegionHandle*)r2,_tmp424)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp425;return(_tmp425=_region_malloc(r2,sizeof(*_tmp425)),((_tmp425->ns=te->ns,((_tmp425->ae=te->ae,((_tmp425->le=_tmp251,((_tmp425->allow_valueof=1,_tmp425)))))))));};};}
# 1002
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1004
const char*_tmp426;struct Cyc_Tcenv_Fenv _stmttmp29=*
# 1006
Cyc_Tcenv_get_fenv(te,((_tmp426="set_encloser",_tag_dyneither(_tmp426,sizeof(char),13))));
# 1004
struct Cyc_Tcenv_SharedFenv*_tmp255;struct Cyc_List_List*_tmp256;struct Cyc_RgnOrder_RgnPO*_tmp257;const struct Cyc_Tcenv_Bindings*_tmp258;struct Cyc_Absyn_Stmt*_tmp259;struct Cyc_Tcenv_CtrlEnv*_tmp25A;void*_tmp25B;void*_tmp25C;struct Cyc_Tcenv_FenvFlags _tmp25D;struct Cyc_Tcenv_Fenv _tmp254=_stmttmp29;_tmp255=_tmp254.shared;_tmp256=_tmp254.type_vars;_tmp257=_tmp254.region_order;_tmp258=_tmp254.locals;_tmp259=_tmp254.encloser;_tmp25A=_tmp254.ctrl_env;_tmp25B=_tmp254.capability;_tmp25C=_tmp254.curr_rgn;_tmp25D=_tmp254.flags;{
# 1007
struct Cyc_Tcenv_Fenv*_tmp427;struct Cyc_Tcenv_Fenv*_tmp25E=
(_tmp427=_region_malloc(r,sizeof(*_tmp427)),((_tmp427->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp255,((_tmp427->type_vars=(struct Cyc_List_List*)_tmp256,((_tmp427->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp257,((_tmp427->locals=(const struct Cyc_Tcenv_Bindings*)_tmp258,((_tmp427->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp427->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp25A,((_tmp427->capability=(void*)_tmp25B,((_tmp427->curr_rgn=(void*)_tmp25C,((_tmp427->flags=(struct Cyc_Tcenv_FenvFlags)_tmp25D,((_tmp427->fnrgn=(struct _RegionHandle*)r,_tmp427)))))))))))))))))))));
# 1010
return Cyc_Tcenv_put_fenv(r,te,_tmp25E);};}
# 1013
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1016
struct Cyc_Absyn_Tvar*tv;
{void*_stmttmp2A=Cyc_Tcutil_compress(rgn);void*_tmp261=_stmttmp2A;struct Cyc_Absyn_Tvar*_tmp263;_LLB2: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp262=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp261;if(_tmp262->tag != 2)goto _LLB4;else{_tmp263=_tmp262->f1;}}_LLB3:
 tv=_tmp263;goto _LLB1;_LLB4:;_LLB5:
{const char*_tmp428;tv=((struct Cyc_Absyn_Tvar*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp428="bad add region",_tag_dyneither(_tmp428,sizeof(char),15))));}goto _LLB1;_LLB1:;}{
# 1021
const char*_tmp429;struct Cyc_Tcenv_Fenv _stmttmp2B=*
# 1023
Cyc_Tcenv_get_fenv(te,((_tmp429="add_region",_tag_dyneither(_tmp429,sizeof(char),11))));
# 1021
struct Cyc_Tcenv_SharedFenv*_tmp266;struct Cyc_List_List*_tmp267;struct Cyc_RgnOrder_RgnPO*_tmp268;const struct Cyc_Tcenv_Bindings*_tmp269;struct Cyc_Absyn_Stmt*_tmp26A;struct Cyc_Tcenv_CtrlEnv*_tmp26B;void*_tmp26C;void*_tmp26D;struct Cyc_Tcenv_FenvFlags _tmp26E;struct Cyc_Tcenv_Fenv _tmp265=_stmttmp2B;_tmp266=_tmp265.shared;_tmp267=_tmp265.type_vars;_tmp268=_tmp265.region_order;_tmp269=_tmp265.locals;_tmp26A=_tmp265.encloser;_tmp26B=_tmp265.ctrl_env;_tmp26C=_tmp265.capability;_tmp26D=_tmp265.curr_rgn;_tmp26E=_tmp265.flags;
# 1024
_tmp268=Cyc_RgnOrder_add_youngest(_tmp266->frgn,_tmp268,tv,resetable,opened);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp43C;struct Cyc_List_List*_tmp43B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp43A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp439;struct Cyc_List_List*_tmp438;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp437;_tmp26C=(void*)((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=((_tmp43C.tag=24,((_tmp43C.f1=((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->hd=(void*)((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((_tmp439.tag=23,((_tmp439.f1=rgn,_tmp439)))),_tmp43A)))),((_tmp438->tl=(
(_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->hd=_tmp26C,((_tmp43B->tl=0,_tmp43B)))))),_tmp438)))))),_tmp43C)))),_tmp437))));}{
struct Cyc_Tcenv_Fenv*_tmp43D;struct Cyc_Tcenv_Fenv*_tmp275=
(_tmp43D=_region_malloc(r,sizeof(*_tmp43D)),((_tmp43D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp266,((_tmp43D->type_vars=(struct Cyc_List_List*)_tmp267,((_tmp43D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp268,((_tmp43D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp269,((_tmp43D->encloser=(struct Cyc_Absyn_Stmt*)_tmp26A,((_tmp43D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26B,((_tmp43D->capability=(void*)_tmp26C,((_tmp43D->curr_rgn=(void*)_tmp26D,((_tmp43D->flags=(struct Cyc_Tcenv_FenvFlags)_tmp26E,((_tmp43D->fnrgn=(struct _RegionHandle*)r,_tmp43D)))))))))))))))))))));
# 1030
return Cyc_Tcenv_put_fenv(r,te,_tmp275);};};}
# 1033
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1035
const char*_tmp43E;struct Cyc_Tcenv_Fenv _stmttmp2C=*
# 1037
Cyc_Tcenv_get_fenv(te,((_tmp43E="new_named_block",_tag_dyneither(_tmp43E,sizeof(char),16))));
# 1035
struct Cyc_Tcenv_SharedFenv*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_RgnOrder_RgnPO*_tmp27B;const struct Cyc_Tcenv_Bindings*_tmp27C;struct Cyc_Absyn_Stmt*_tmp27D;struct Cyc_Tcenv_CtrlEnv*_tmp27E;void*_tmp27F;void*_tmp280;struct Cyc_Tcenv_FenvFlags _tmp281;struct Cyc_Tcenv_Fenv _tmp278=_stmttmp2C;_tmp279=_tmp278.shared;_tmp27A=_tmp278.type_vars;_tmp27B=_tmp278.region_order;_tmp27C=_tmp278.locals;_tmp27D=_tmp278.encloser;_tmp27E=_tmp278.ctrl_env;_tmp27F=_tmp278.capability;_tmp280=_tmp278.curr_rgn;_tmp281=_tmp278.flags;{
# 1039
const char*_tmp43F;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp43F="new_block",_tag_dyneither(_tmp43F,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp442;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp441;void*block_typ=(void*)((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441[0]=((_tmp442.tag=2,((_tmp442.f1=block_rgn,_tmp442)))),_tmp441))));
{struct Cyc_List_List*_tmp443;_tmp27A=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->hd=block_rgn,((_tmp443->tl=_tmp27A,_tmp443))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp27A);
_tmp27B=Cyc_RgnOrder_add_youngest(_tmp279->frgn,_tmp27B,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp456;struct Cyc_List_List*_tmp455;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp454;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp453;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp451;_tmp27F=(void*)((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=((_tmp456.tag=24,((_tmp456.f1=((_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452->hd=(void*)((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454[0]=((_tmp453.tag=23,((_tmp453.f1=block_typ,_tmp453)))),_tmp454)))),((_tmp452->tl=((_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455->hd=_tmp27F,((_tmp455->tl=0,_tmp455)))))),_tmp452)))))),_tmp456)))),_tmp451))));}
_tmp280=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp457;struct Cyc_Tcenv_Fenv*_tmp289=
(_tmp457=_region_malloc(r,sizeof(*_tmp457)),((_tmp457->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp279,((_tmp457->type_vars=(struct Cyc_List_List*)_tmp27A,((_tmp457->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp27B,((_tmp457->locals=(const struct Cyc_Tcenv_Bindings*)_tmp27C,((_tmp457->encloser=(struct Cyc_Absyn_Stmt*)_tmp27D,((_tmp457->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp27E,((_tmp457->capability=(void*)_tmp27F,((_tmp457->curr_rgn=(void*)_tmp280,((_tmp457->flags=(struct Cyc_Tcenv_FenvFlags)_tmp281,((_tmp457->fnrgn=(struct _RegionHandle*)r,_tmp457)))))))))))))))))))));
# 1049
return Cyc_Tcenv_put_fenv(r,te,_tmp289);};};}
# 1052
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1054
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1056
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1062
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1066
const char*_tmp458;struct Cyc_Tcenv_Fenv _stmttmp2D=*
# 1068
Cyc_Tcenv_get_fenv(te,((_tmp458="new_outlives_constraints",_tag_dyneither(_tmp458,sizeof(char),25))));
# 1066
struct Cyc_Tcenv_SharedFenv*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_RgnOrder_RgnPO*_tmp293;const struct Cyc_Tcenv_Bindings*_tmp294;struct Cyc_Absyn_Stmt*_tmp295;struct Cyc_Tcenv_CtrlEnv*_tmp296;void*_tmp297;void*_tmp298;struct Cyc_Tcenv_FenvFlags _tmp299;struct Cyc_Tcenv_Fenv _tmp290=_stmttmp2D;_tmp291=_tmp290.shared;_tmp292=_tmp290.type_vars;_tmp293=_tmp290.region_order;_tmp294=_tmp290.locals;_tmp295=_tmp290.encloser;_tmp296=_tmp290.ctrl_env;_tmp297=_tmp290.capability;_tmp298=_tmp290.curr_rgn;_tmp299=_tmp290.flags;
# 1069
for(0;cs != 0;cs=cs->tl){
_tmp293=
Cyc_RgnOrder_add_outlives_constraint(_tmp291->frgn,_tmp293,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1074
struct Cyc_Tcenv_Fenv*_tmp459;struct Cyc_Tcenv_Fenv*_tmp29A=
(_tmp459=_region_malloc(r,sizeof(*_tmp459)),((_tmp459->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp291,((_tmp459->type_vars=(struct Cyc_List_List*)_tmp292,((_tmp459->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp293,((_tmp459->locals=(const struct Cyc_Tcenv_Bindings*)_tmp294,((_tmp459->encloser=(struct Cyc_Absyn_Stmt*)_tmp295,((_tmp459->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp296,((_tmp459->capability=(void*)_tmp297,((_tmp459->curr_rgn=(void*)_tmp298,((_tmp459->flags=(struct Cyc_Tcenv_FenvFlags)_tmp299,((_tmp459->fnrgn=(struct _RegionHandle*)r,_tmp459)))))))))))))))))))));
# 1077
return Cyc_Tcenv_put_fenv(r,te,_tmp29A);};}
# 1080
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1088
void*_tmp29D=Cyc_Tcutil_compress(r1);
void*_tmp29E=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp29F=Cyc_Tcutil_typ_kind(_tmp29D);
struct Cyc_Absyn_Kind*_tmp2A0=Cyc_Tcutil_typ_kind(_tmp29E);
# 1094
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp29F,_tmp2A0);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2A0,_tmp29F);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp460;void*_tmp45F[4];struct Cyc_String_pa_PrintArg_struct _tmp45E;struct Cyc_String_pa_PrintArg_struct _tmp45D;struct Cyc_String_pa_PrintArg_struct _tmp45C;struct Cyc_String_pa_PrintArg_struct _tmp45B;(_tmp45B.tag=0,((_tmp45B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2A0)),((_tmp45C.tag=0,((_tmp45C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp29F)),((_tmp45D.tag=0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29E)),((_tmp45E.tag=0,((_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp29D)),((_tmp45F[0]=& _tmp45E,((_tmp45F[1]=& _tmp45D,((_tmp45F[2]=& _tmp45C,((_tmp45F[3]=& _tmp45B,Cyc_Tcutil_terr(loc,((_tmp460="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp460,sizeof(char),82))),_tag_dyneither(_tmp45F,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1101
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _stmttmp2E=Cyc_Tcutil_swap_kind(_tmp29E,Cyc_Tcutil_kind_to_bound(_tmp29F));struct Cyc_Absyn_Tvar*_tmp2A8;void*_tmp2A9;struct _tuple11 _tmp2A7=_stmttmp2E;_tmp2A8=_tmp2A7.f1;_tmp2A9=_tmp2A7.f2;{
struct _tuple11*_tmp461;*oldtv=((_tmp461=_region_malloc(r,sizeof(*_tmp461)),((_tmp461->f1=_tmp2A8,((_tmp461->f2=_tmp2A9,_tmp461))))));};}else{
# 1105
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _stmttmp2F=Cyc_Tcutil_swap_kind(_tmp29D,Cyc_Tcutil_kind_to_bound(_tmp2A0));struct Cyc_Absyn_Tvar*_tmp2AC;void*_tmp2AD;struct _tuple11 _tmp2AB=_stmttmp2F;_tmp2AC=_tmp2AB.f1;_tmp2AD=_tmp2AB.f2;{
struct _tuple11*_tmp462;*oldtv=((_tmp462=_region_malloc(r,sizeof(*_tmp462)),((_tmp462->f1=_tmp2AC,((_tmp462->f2=_tmp2AD,_tmp462))))));};}}}{
# 1111
struct _RegionHandle*_tmp2AF=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2B0=0;
if((_tmp29D != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29D != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29D != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp465;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp464;void*eff1=(void*)((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464[0]=((_tmp465.tag=23,((_tmp465.f1=_tmp29D,_tmp465)))),_tmp464))));
struct _tuple13*_tmp468;struct Cyc_List_List*_tmp467;_tmp2B0=((_tmp467=_region_malloc(_tmp2AF,sizeof(*_tmp467)),((_tmp467->hd=((_tmp468=_region_malloc(_tmp2AF,sizeof(*_tmp468)),((_tmp468->f1=eff1,((_tmp468->f2=_tmp29E,_tmp468)))))),((_tmp467->tl=_tmp2B0,_tmp467))))));}
# 1117
if((_tmp29E != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp29E != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp29E != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp46B;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp46A;void*eff2=(void*)((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A[0]=((_tmp46B.tag=23,((_tmp46B.f1=_tmp29E,_tmp46B)))),_tmp46A))));
struct _tuple13*_tmp46E;struct Cyc_List_List*_tmp46D;_tmp2B0=((_tmp46D=_region_malloc(_tmp2AF,sizeof(*_tmp46D)),((_tmp46D->hd=((_tmp46E=_region_malloc(_tmp2AF,sizeof(*_tmp46E)),((_tmp46E->f1=eff2,((_tmp46E->f2=_tmp29D,_tmp46E)))))),((_tmp46D->tl=_tmp2B0,_tmp46D))))));}
# 1122
return Cyc_Tcenv_new_outlives_constraints(_tmp2AF,te,_tmp2B0,loc);};}
# 1125
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2B9=te->le;
if(_tmp2B9 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2B9;
void*_tmp2BB;struct Cyc_Tcenv_Fenv*_tmp2BA=fe;_tmp2BB=_tmp2BA->curr_rgn;
return _tmp2BB;};}
# 1136
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp46F;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp46F="check_rgn_accessible",_tag_dyneither(_tmp46F,sizeof(char),21))));
void*_tmp2BD;struct Cyc_RgnOrder_RgnPO*_tmp2BE;struct Cyc_Tcenv_Fenv*_tmp2BC=fe;_tmp2BD=_tmp2BC->capability;_tmp2BE=_tmp2BC->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2BD) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2BD))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp472;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp471;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2BE,(void*)((_tmp471=_cycalloc(sizeof(*_tmp471)),((_tmp471[0]=((_tmp472.tag=23,((_tmp472.f1=rgn,_tmp472)))),_tmp471)))),_tmp2BD))
return;}{
const char*_tmp476;void*_tmp475[1];struct Cyc_String_pa_PrintArg_struct _tmp474;(_tmp474.tag=0,((_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp475[0]=& _tmp474,Cyc_Tcutil_terr(loc,((_tmp476="Expression accesses unavailable region %s",_tag_dyneither(_tmp476,sizeof(char),42))),_tag_dyneither(_tmp475,sizeof(void*),1)))))));};}
# 1148
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp477;struct Cyc_Tcenv_Fenv*_stmttmp30=
Cyc_Tcenv_get_fenv(te,((_tmp477="check_rgn_resetable",_tag_dyneither(_tmp477,sizeof(char),20))));
# 1150
struct Cyc_RgnOrder_RgnPO*_tmp2C6;struct Cyc_Tcenv_Fenv*_tmp2C5=_stmttmp30;_tmp2C6=_tmp2C5->region_order;{
# 1152
void*_stmttmp31=Cyc_Tcutil_compress(rgn);void*_tmp2C7=_stmttmp31;struct Cyc_Absyn_Tvar*_tmp2C9;_LLB7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C7;if(_tmp2C8->tag != 2)goto _LLB9;else{_tmp2C9=_tmp2C8->f1;}}_LLB8:
# 1154
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2C6,_tmp2C9)){
const char*_tmp47B;void*_tmp47A[1];struct Cyc_String_pa_PrintArg_struct _tmp479;(_tmp479.tag=0,((_tmp479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp47A[0]=& _tmp479,Cyc_Tcutil_terr(loc,((_tmp47B="Region %s is not resetable",_tag_dyneither(_tmp47B,sizeof(char),27))),_tag_dyneither(_tmp47A,sizeof(void*),1)))))));}
return;_LLB9:;_LLBA: {
# 1158
const char*_tmp47E;void*_tmp47D;(_tmp47D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp47E="check_rgn_resetable",_tag_dyneither(_tmp47E,sizeof(char),20))),_tag_dyneither(_tmp47D,sizeof(void*),0)));}_LLB6:;};};}
# 1165
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2D0=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1170
if(_tmp2D0 == 0){
void*_tmp2D1=rt_a;_LLBC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D2->tag != 22)goto _LLBE;}_LLBD:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LLBE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D3->tag != 21)goto _LLC0;}_LLBF:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLC0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D1;if(_tmp2D4->tag != 20)goto _LLC2;}_LLC1:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLC2:;_LLC3:
 return 0;_LLBB:;}{
# 1178
struct Cyc_Tcenv_Fenv*fe=_tmp2D0;
struct Cyc_RgnOrder_RgnPO*_tmp2D6;struct Cyc_Tcenv_Fenv*_tmp2D5=fe;_tmp2D6=_tmp2D5->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp481;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp480;int _tmp2D7=Cyc_RgnOrder_effect_outlives(_tmp2D6,(void*)((_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480[0]=((_tmp481.tag=23,((_tmp481.f1=rt_a,_tmp481)))),_tmp480)))),rt_b);
# 1184
return _tmp2D7;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1189
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp482;struct Cyc_Tcenv_Fenv*_stmttmp32=
Cyc_Tcenv_get_fenv(te,((_tmp482="check_effect_accessible",_tag_dyneither(_tmp482,sizeof(char),24))));
# 1190
void*_tmp2DB;struct Cyc_RgnOrder_RgnPO*_tmp2DC;struct Cyc_Tcenv_SharedFenv*_tmp2DD;struct Cyc_Tcenv_Fenv*_tmp2DA=_stmttmp32;_tmp2DB=_tmp2DA->capability;_tmp2DC=_tmp2DA->region_order;_tmp2DD=_tmp2DA->shared;
# 1192
if(Cyc_Tcutil_subset_effect(0,eff,_tmp2DB))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2DC,eff,_tmp2DB))
return;{
struct _RegionHandle*frgn=_tmp2DD->frgn;
struct _tuple14*_tmp485;struct Cyc_List_List*_tmp484;_tmp2DD->delayed_effect_checks=(
(_tmp484=_region_malloc(frgn,sizeof(*_tmp484)),((_tmp484->hd=((_tmp485=_region_malloc(frgn,sizeof(*_tmp485)),((_tmp485->f1=_tmp2DB,((_tmp485->f2=eff,((_tmp485->f3=_tmp2DC,((_tmp485->f4=loc,_tmp485)))))))))),((_tmp484->tl=_tmp2DD->delayed_effect_checks,_tmp484))))));};}
# 1201
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp486;struct Cyc_Tcenv_Fenv*_stmttmp33=
Cyc_Tcenv_get_fenv(te,((_tmp486="check_delayed_effects",_tag_dyneither(_tmp486,sizeof(char),22))));
# 1202
struct Cyc_Tcenv_SharedFenv*_tmp2E2;struct Cyc_Tcenv_Fenv*_tmp2E1=_stmttmp33;_tmp2E2=_tmp2E1->shared;{
# 1204
struct Cyc_List_List*_tmp2E3=_tmp2E2->delayed_effect_checks;
for(0;_tmp2E3 != 0;_tmp2E3=_tmp2E3->tl){
struct _tuple14*_stmttmp34=(struct _tuple14*)_tmp2E3->hd;void*_tmp2E5;void*_tmp2E6;struct Cyc_RgnOrder_RgnPO*_tmp2E7;unsigned int _tmp2E8;struct _tuple14*_tmp2E4=_stmttmp34;_tmp2E5=_tmp2E4->f1;_tmp2E6=_tmp2E4->f2;_tmp2E7=_tmp2E4->f3;_tmp2E8=_tmp2E4->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp2E6,_tmp2E5))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E7,_tmp2E6,_tmp2E5))
continue;{
const char*_tmp48B;void*_tmp48A[2];struct Cyc_String_pa_PrintArg_struct _tmp489;struct Cyc_String_pa_PrintArg_struct _tmp488;(_tmp488.tag=0,((_tmp488.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2E6)),((_tmp489.tag=0,((_tmp489.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2E5)),((_tmp48A[0]=& _tmp489,((_tmp48A[1]=& _tmp488,Cyc_Tcutil_terr(_tmp2E8,((_tmp48B="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp48B,sizeof(char),51))),_tag_dyneither(_tmp48A,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1220
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1222
struct Cyc_Tcenv_Fenv*_tmp2EE=te->le;
if(_tmp2EE == 0){
# 1225
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp48E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp48D;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D[0]=((_tmp48E.tag=23,((_tmp48E.f1=(*((struct _tuple13*)po->hd)).f2,_tmp48E)))),_tmp48D)))),Cyc_Absyn_empty_effect)){
const char*_tmp491;void*_tmp490;(_tmp490=0,Cyc_Tcutil_terr(loc,((_tmp491="the required region ordering is not satisfied here",_tag_dyneither(_tmp491,sizeof(char),51))),_tag_dyneither(_tmp490,sizeof(void*),0)));}}
return;}{
# 1231
struct Cyc_Tcenv_Fenv*_stmttmp35=_tmp2EE;struct Cyc_RgnOrder_RgnPO*_tmp2F4;struct Cyc_Tcenv_SharedFenv*_tmp2F5;struct Cyc_Tcenv_Fenv*_tmp2F3=_stmttmp35;_tmp2F4=_tmp2F3->region_order;_tmp2F5=_tmp2F3->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2F4,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp494;struct Cyc_List_List*_tmp493;_tmp2F5->delayed_constraint_checks=(
(_tmp493=_region_malloc(_tmp2F5->frgn,sizeof(*_tmp493)),((_tmp493->hd=((_tmp494=_region_malloc(_tmp2F5->frgn,sizeof(*_tmp494)),((_tmp494->f1=_tmp2F4,((_tmp494->f2=po,((_tmp494->f3=loc,_tmp494)))))))),((_tmp493->tl=_tmp2F5->delayed_constraint_checks,_tmp493))))));}};}
# 1238
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp495;struct Cyc_Tcenv_Fenv*_stmttmp36=
Cyc_Tcenv_get_fenv(te,((_tmp495="check_delayed_constraints",_tag_dyneither(_tmp495,sizeof(char),26))));
# 1239
struct Cyc_Tcenv_SharedFenv*_tmp2F9;struct Cyc_Tcenv_Fenv*_tmp2F8=_stmttmp36;_tmp2F9=_tmp2F8->shared;{
# 1241
struct Cyc_List_List*_tmp2FA=_tmp2F9->delayed_constraint_checks;
for(0;_tmp2FA != 0;_tmp2FA=_tmp2FA->tl){
struct _tuple15*_stmttmp37=(struct _tuple15*)_tmp2FA->hd;struct Cyc_RgnOrder_RgnPO*_tmp2FC;struct Cyc_List_List*_tmp2FD;unsigned int _tmp2FE;struct _tuple15*_tmp2FB=_stmttmp37;_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;_tmp2FE=_tmp2FB->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp2FC,_tmp2FD,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp498;void*_tmp497;(_tmp497=0,Cyc_Tcutil_terr(_tmp2FE,((_tmp498="the required region ordering is not satisfied here",_tag_dyneither(_tmp498,sizeof(char),51))),_tag_dyneither(_tmp497,sizeof(void*),0)));}}};}
# 1249
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp4A5;const char*_tmp4A4;void*_tmp4A3[1];struct Cyc_String_pa_PrintArg_struct _tmp4A2;struct Cyc_Absyn_Tvar*_tmp4A1;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->name=((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=(struct _dyneither_ptr)((_tmp4A2.tag=0,((_tmp4A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4A3[0]=& _tmp4A2,Cyc_aprintf(((_tmp4A4="`%s",_tag_dyneither(_tmp4A4,sizeof(char),4))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))))),_tmp4A5)))),((_tmp4A1->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4A1->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4A1)))))));
struct Cyc_List_List*_tmp4A6;struct Cyc_List_List*_tmp302=(_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6->hd=rgn0,((_tmp4A6->tl=fd->tvs,_tmp4A6)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp302);{
# 1257
struct Cyc_RgnOrder_RgnPO*_tmp303=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1259
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A8;void*param_rgn=(void*)((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8[0]=((_tmp4A9.tag=2,((_tmp4A9.f1=rgn0,_tmp4A9)))),_tmp4A8))));
struct Cyc_List_List*_tmp304=0;
{struct Cyc_List_List*_tmp305=fd->args;for(0;_tmp305 != 0;_tmp305=_tmp305->tl){
struct Cyc_Absyn_Vardecl _tmp4AF;struct _tuple0*_tmp4AE;struct Cyc_Absyn_Vardecl*_tmp4AD;struct Cyc_Absyn_Vardecl*_tmp306=(_tmp4AD=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4AD[0]=(struct Cyc_Absyn_Vardecl)((_tmp4AF.sc=Cyc_Absyn_Public,((_tmp4AF.name=(
(_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->f1=Cyc_Absyn_Loc_n,((_tmp4AE->f2=(*((struct _tuple8*)_tmp305->hd)).f1,_tmp4AE)))))),((_tmp4AF.tq=(*((struct _tuple8*)_tmp305->hd)).f2,((_tmp4AF.type=(*((struct _tuple8*)_tmp305->hd)).f3,((_tmp4AF.initializer=0,((_tmp4AF.rgn=param_rgn,((_tmp4AF.attributes=0,((_tmp4AF.escapes=0,_tmp4AF)))))))))))))))),_tmp4AD)));
# 1269
{struct Cyc_List_List _tmp4B2;struct Cyc_List_List*_tmp4B1;_tmp304=((_tmp4B1=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4B1[0]=(struct Cyc_List_List)((_tmp4B2.hd=_tmp306,((_tmp4B2.tl=_tmp304,_tmp4B2)))),_tmp4B1))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4B5;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4B4;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp309=(_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4[0]=((_tmp4B5.tag=3,((_tmp4B5.f1=_tmp306,_tmp4B5)))),_tmp4B4)));
struct _dyneither_ptr*_tmp30A=(*((struct _tuple8*)_tmp305->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp4B6;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6)),((_tmp4B6->v=_tmp30A,((_tmp4B6->b=(void*)_tmp309,((_tmp4B6->tl=locals,_tmp4B6))))))));};}}
# 1274
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp38=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp312;struct Cyc_Absyn_Tqual _tmp313;void*_tmp314;int _tmp315;struct Cyc_Absyn_VarargInfo _tmp311=_stmttmp38;_tmp312=_tmp311.name;_tmp313=_tmp311.tq;_tmp314=_tmp311.type;_tmp315=_tmp311.inject;
if(_tmp312 != 0){
void*_tmp316=Cyc_Absyn_dyneither_typ(_tmp314,param_rgn,_tmp313,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp4BC;struct _tuple0*_tmp4BB;struct Cyc_Absyn_Vardecl*_tmp4BA;struct Cyc_Absyn_Vardecl*_tmp317=(_tmp4BA=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4BA[0]=(struct Cyc_Absyn_Vardecl)((_tmp4BC.sc=Cyc_Absyn_Public,((_tmp4BC.name=(
(_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB->f1=Cyc_Absyn_Loc_n,((_tmp4BB->f2=_tmp312,_tmp4BB)))))),((_tmp4BC.tq=
Cyc_Absyn_empty_tqual(0),((_tmp4BC.type=_tmp316,((_tmp4BC.initializer=0,((_tmp4BC.rgn=param_rgn,((_tmp4BC.attributes=0,((_tmp4BC.escapes=0,_tmp4BC)))))))))))))))),_tmp4BA)));
# 1285
{struct Cyc_List_List*_tmp4BD;_tmp304=((_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD->hd=_tmp317,((_tmp4BD->tl=_tmp304,_tmp4BD))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp4C0;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp4BF;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp319=(_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4C0.tag=3,((_tmp4C0.f1=_tmp317,_tmp4C0)))),_tmp4BF)));
struct _dyneither_ptr*_tmp31A=_tmp312;
struct Cyc_Tcenv_Bindings*_tmp4C1;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp4C1=_region_malloc(r,sizeof(*_tmp4C1)),((_tmp4C1->v=_tmp31A,((_tmp4C1->b=(void*)_tmp319,((_tmp4C1->tl=locals,_tmp4C1))))))));};}else{
# 1290
const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,Cyc_Tcutil_terr(loc,((_tmp4C4="missing name for varargs",_tag_dyneither(_tmp4C4,sizeof(char),25))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}}
# 1292
{struct Cyc_Core_Opt _tmp4C7;struct Cyc_Core_Opt*_tmp4C6;fd->param_vardecls=((_tmp4C6=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp4C6[0]=(struct Cyc_Core_Opt)((_tmp4C7.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp304),_tmp4C7)),_tmp4C6))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4E5;struct Cyc_List_List*_tmp4E4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4E3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4E2;struct Cyc_List_List*_tmp4E1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4E0;struct Cyc_Tcenv_CtrlEnv*_tmp4DF;struct Cyc_Tcenv_SharedFenv*_tmp4DE;struct Cyc_Tcenv_Fenv*_tmp4DD;return(_tmp4DD=_region_malloc(r,sizeof(*_tmp4DD)),((_tmp4DD->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE->frgn=r,((_tmp4DE->return_typ=fd->ret_type,((_tmp4DE->seen_labels=
# 1297
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4DE->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp4DE->delayed_effect_checks=0,((_tmp4DE->delayed_constraint_checks=0,_tmp4DE)))))))))))))),((_tmp4DD->type_vars=(struct Cyc_List_List*)_tmp302,((_tmp4DD->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp303,((_tmp4DD->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp4DD->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp4DD->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1306
(_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF->ctrl_rgn=r,((_tmp4DF->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4DF->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp4DF->fallthru_clause=0,((_tmp4DF->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp4DF->try_depth=0,_tmp4DF)))))))))))))),((_tmp4DD->capability=(void*)((void*)(
(_tmp4E5=_cycalloc(sizeof(*_tmp4E5)),((_tmp4E5[0]=((_tmp4E0.tag=24,((_tmp4E0.f1=((_tmp4E4=_cycalloc(sizeof(*_tmp4E4)),((_tmp4E4->hd=(void*)((_tmp4E2=_cycalloc(sizeof(*_tmp4E2)),((_tmp4E2[0]=((_tmp4E3.tag=23,((_tmp4E3.f1=param_rgn,_tmp4E3)))),_tmp4E2)))),((_tmp4E4->tl=(
(_tmp4E1=_cycalloc(sizeof(*_tmp4E1)),((_tmp4E1->hd=(void*)_check_null(fd->effect),((_tmp4E1->tl=0,_tmp4E1)))))),_tmp4E4)))))),_tmp4E0)))),_tmp4E5))))),((_tmp4DD->curr_rgn=(void*)param_rgn,((_tmp4DD->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1310
((_tmp4DD->flags).in_notreadctxt=0,(((_tmp4DD->flags).in_lhs=0,(((_tmp4DD->flags).in_new=0,(((_tmp4DD->flags).abstract_ok=0,_tmp4DD->flags)))))))),((_tmp4DD->fnrgn=(struct _RegionHandle*)r,_tmp4DD)))))))))))))))))))));};};}
# 1315
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp337;struct Cyc_RgnOrder_RgnPO*_tmp338;struct Cyc_List_List*_tmp339;struct Cyc_Tcenv_SharedFenv*_tmp33A;struct _RegionHandle*_tmp33B;struct Cyc_Tcenv_Fenv*_tmp336=old_fenv;_tmp337=_tmp336->locals;_tmp338=_tmp336->region_order;_tmp339=_tmp336->type_vars;_tmp33A=_tmp336->shared;_tmp33B=_tmp336->fnrgn;{
# 1318
struct _RegionHandle*_tmp33C=_tmp33A->frgn;
const struct Cyc_Tcenv_Bindings*_tmp33D=_tmp337;
struct _dyneither_ptr*_tmp4F2;const char*_tmp4F1;void*_tmp4F0[1];struct Cyc_String_pa_PrintArg_struct _tmp4EF;struct Cyc_Absyn_Tvar*_tmp4EE;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4EE=_cycalloc(sizeof(*_tmp4EE)),((_tmp4EE->name=((_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((_tmp4F2[0]=(struct _dyneither_ptr)((_tmp4EF.tag=0,((_tmp4EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4F0[0]=& _tmp4EF,Cyc_aprintf(((_tmp4F1="`%s",_tag_dyneither(_tmp4F1,sizeof(char),4))),_tag_dyneither(_tmp4F0,sizeof(void*),1)))))))),_tmp4F2)))),((_tmp4EE->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4EE->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4EE)))))));
# 1324
{struct Cyc_List_List*_tmp33E=fd->tvs;for(0;_tmp33E != 0;_tmp33E=_tmp33E->tl){
struct Cyc_Absyn_Kind*_stmttmp39=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp33E->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp340;enum Cyc_Absyn_AliasQual _tmp341;struct Cyc_Absyn_Kind*_tmp33F=_stmttmp39;_tmp340=_tmp33F->kind;_tmp341=_tmp33F->aliasqual;
if(_tmp340 == Cyc_Absyn_RgnKind){
if(_tmp341 == Cyc_Absyn_Aliasable)
_tmp338=Cyc_RgnOrder_add_youngest(_tmp33C,_tmp338,(struct Cyc_Absyn_Tvar*)_tmp33E->hd,0,0);else{
# 1330
const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F5="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp4F5,sizeof(char),39))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}}}}
# 1332
_tmp338=Cyc_RgnOrder_add_youngest(_tmp33C,_tmp338,rgn0,0,0);{
struct Cyc_List_List*_tmp4F6;struct Cyc_List_List*_tmp344=(_tmp4F6=_cycalloc(sizeof(*_tmp4F6)),((_tmp4F6->hd=rgn0,((_tmp4F6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp339),_tmp4F6)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp344);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4F9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F8;void*param_rgn=(void*)((_tmp4F8=_cycalloc(sizeof(*_tmp4F8)),((_tmp4F8[0]=((_tmp4F9.tag=2,((_tmp4F9.f1=rgn0,_tmp4F9)))),_tmp4F8))));
struct Cyc_List_List*_tmp345=0;
{struct Cyc_List_List*_tmp346=fd->args;for(0;_tmp346 != 0;_tmp346=_tmp346->tl){
struct Cyc_Absyn_Vardecl _tmp4FF;struct _tuple0*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp4FD;struct Cyc_Absyn_Vardecl*_tmp347=(_tmp4FD=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4FD[0]=(struct Cyc_Absyn_Vardecl)((_tmp4FF.sc=Cyc_Absyn_Public,((_tmp4FF.name=(
(_tmp4FE=_cycalloc(sizeof(*_tmp4FE)),((_tmp4FE->f1=Cyc_Absyn_Loc_n,((_tmp4FE->f2=(*((struct _tuple8*)_tmp346->hd)).f1,_tmp4FE)))))),((_tmp4FF.tq=(*((struct _tuple8*)_tmp346->hd)).f2,((_tmp4FF.type=(*((struct _tuple8*)_tmp346->hd)).f3,((_tmp4FF.initializer=0,((_tmp4FF.rgn=param_rgn,((_tmp4FF.attributes=0,((_tmp4FF.escapes=0,_tmp4FF)))))))))))))))),_tmp4FD)));
# 1345
{struct Cyc_List_List _tmp502;struct Cyc_List_List*_tmp501;_tmp345=((_tmp501=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp501[0]=(struct Cyc_List_List)((_tmp502.hd=_tmp347,((_tmp502.tl=_tmp345,_tmp502)))),_tmp501))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp505;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp504;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34A=(_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504[0]=((_tmp505.tag=3,((_tmp505.f1=_tmp347,_tmp505)))),_tmp504)));
struct _dyneither_ptr*_tmp34B=(*((struct _tuple8*)_tmp346->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp506;_tmp33D=(const struct Cyc_Tcenv_Bindings*)((_tmp506=_region_malloc(_tmp33C,sizeof(*_tmp506)),((_tmp506->v=_tmp34B,((_tmp506->b=(void*)_tmp34A,((_tmp506->tl=_tmp33D,_tmp506))))))));};}}
# 1350
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _stmttmp3A=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp353;struct Cyc_Absyn_Tqual _tmp354;void*_tmp355;int _tmp356;struct Cyc_Absyn_VarargInfo _tmp352=_stmttmp3A;_tmp353=_tmp352.name;_tmp354=_tmp352.tq;_tmp355=_tmp352.type;_tmp356=_tmp352.inject;
if(_tmp353 != 0){
void*_tmp357=Cyc_Absyn_dyneither_typ(_tmp355,param_rgn,_tmp354,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp50C;struct _tuple0*_tmp50B;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Vardecl*_tmp358=(_tmp50A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp50A[0]=(struct Cyc_Absyn_Vardecl)((_tmp50C.sc=Cyc_Absyn_Public,((_tmp50C.name=(
(_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B->f1=Cyc_Absyn_Loc_n,((_tmp50B->f2=_tmp353,_tmp50B)))))),((_tmp50C.tq=
Cyc_Absyn_empty_tqual(0),((_tmp50C.type=_tmp357,((_tmp50C.initializer=0,((_tmp50C.rgn=param_rgn,((_tmp50C.attributes=0,((_tmp50C.escapes=0,_tmp50C)))))))))))))))),_tmp50A)));
# 1361
{struct Cyc_List_List*_tmp50D;_tmp345=((_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D->hd=_tmp358,((_tmp50D->tl=_tmp345,_tmp50D))))));}{
struct _dyneither_ptr*_tmp35A=_tmp353;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp510;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp50F;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp35B=(_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=3,((_tmp510.f1=_tmp358,_tmp510)))),_tmp50F)));
struct Cyc_Tcenv_Bindings*_tmp511;_tmp33D=(const struct Cyc_Tcenv_Bindings*)((_tmp511=_region_malloc(_tmp33C,sizeof(*_tmp511)),((_tmp511->v=_tmp35A,((_tmp511->b=(void*)_tmp35B,((_tmp511->tl=_tmp33D,_tmp511))))))));};}else{
# 1366
const char*_tmp514;void*_tmp513;(_tmp513=0,Cyc_Tcutil_terr(loc,((_tmp514="missing name for varargs",_tag_dyneither(_tmp514,sizeof(char),25))),_tag_dyneither(_tmp513,sizeof(void*),0)));}}
# 1368
{struct Cyc_Core_Opt _tmp517;struct Cyc_Core_Opt*_tmp516;fd->param_vardecls=((_tmp516=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp516[0]=(struct Cyc_Core_Opt)((_tmp517.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp345),_tmp517)),_tmp516))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp535;struct Cyc_List_List*_tmp534;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp533;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp532;struct Cyc_List_List*_tmp531;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp530;struct Cyc_Tcenv_CtrlEnv*_tmp52F;struct Cyc_Tcenv_SharedFenv*_tmp52E;struct Cyc_Tcenv_Fenv*_tmp52D;return(_tmp52D=_region_malloc(_tmp33C,sizeof(*_tmp52D)),((_tmp52D->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp52E=_region_malloc(_tmp33C,sizeof(*_tmp52E)),((_tmp52E->frgn=_tmp33C,((_tmp52E->return_typ=fd->ret_type,((_tmp52E->seen_labels=
# 1373
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp33C,Cyc_strptrcmp),((_tmp52E->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp33C,Cyc_strptrcmp),((_tmp52E->delayed_effect_checks=0,((_tmp52E->delayed_constraint_checks=0,_tmp52E)))))))))))))),((_tmp52D->type_vars=(struct Cyc_List_List*)_tmp344,((_tmp52D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp338,((_tmp52D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp33D,((_tmp52D->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp52D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1382
(_tmp52F=_region_malloc(_tmp33C,sizeof(*_tmp52F)),((_tmp52F->ctrl_rgn=_tmp33C,((_tmp52F->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp52F->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp52F->fallthru_clause=0,((_tmp52F->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp52F->try_depth=0,_tmp52F)))))))))))))),((_tmp52D->capability=(void*)((void*)(
(_tmp535=_cycalloc(sizeof(*_tmp535)),((_tmp535[0]=((_tmp530.tag=24,((_tmp530.f1=((_tmp534=_cycalloc(sizeof(*_tmp534)),((_tmp534->hd=(void*)((_tmp532=_cycalloc(sizeof(*_tmp532)),((_tmp532[0]=((_tmp533.tag=23,((_tmp533.f1=param_rgn,_tmp533)))),_tmp532)))),((_tmp534->tl=(
(_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531->hd=(void*)_check_null(fd->effect),((_tmp531->tl=0,_tmp531)))))),_tmp534)))))),_tmp530)))),_tmp535))))),((_tmp52D->curr_rgn=(void*)param_rgn,((_tmp52D->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1386
((_tmp52D->flags).in_notreadctxt=0,(((_tmp52D->flags).in_lhs=0,(((_tmp52D->flags).in_new=0,(((_tmp52D->flags).abstract_ok=0,_tmp52D->flags)))))))),((_tmp52D->fnrgn=(struct _RegionHandle*)_tmp33B,_tmp52D)))))))))))))))))))));};};};};}
