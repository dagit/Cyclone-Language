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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 234
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 947
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 957
void*Cyc_Absyn_exn_typ();
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 976
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 979
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1067
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1141
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1143
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 100
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 159
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 178
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 192
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 200
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 207
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 55
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 74
int Cyc_Tcutil_coerceable(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 84
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 95
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 145
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 147
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 151
void Cyc_Tcutil_explain_failure();
# 153
int Cyc_Tcutil_unify(void*,void*);
# 158
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 167
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 172
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 181
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 217 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 227
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 229
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 231
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 235
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 241
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 245
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 255
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 260
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 263
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 268
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 273
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 282
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 309
int Cyc_Tcutil_bits_only(void*t);
# 312
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
# 79
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};
# 94
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 140
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 147
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 163 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp63E;(_tmp63E=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp63E,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp63F;return(_tmp63F=_cycalloc(sizeof(*_tmp63F)),((_tmp63F->f1=0,((_tmp63F->f2=e,_tmp63F)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp643;void*_tmp642[1];struct Cyc_String_pa_PrintArg_struct _tmp641;(_tmp641.tag=0,((_tmp641.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp642[0]=& _tmp641,Cyc_Tcutil_terr(e1->loc,((_tmp643="undeclared identifier %s",_tag_dyneither(_tmp643,sizeof(char),25))),_tag_dyneither(_tmp642,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp647;void*_tmp646[1];struct Cyc_String_pa_PrintArg_struct _tmp645;(_tmp645.tag=0,((_tmp645.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp646[0]=& _tmp645,Cyc_Tcutil_terr(e->loc,((_tmp647="%s is a constant, not a function",_tag_dyneither(_tmp647,sizeof(char),33))),_tag_dyneither(_tmp646,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp64A;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp649;e->r=(void*)((_tmp649=_cycalloc(sizeof(*_tmp649)),((_tmp649[0]=((_tmp64A.tag=29,((_tmp64A.f1=es,((_tmp64A.f2=_tmp10,((_tmp64A.f3=_tmp11,_tmp64A)))))))),_tmp649))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp64D;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp64C;e->r=(void*)((_tmp64C=_cycalloc(sizeof(*_tmp64C)),((_tmp64C[0]=((_tmp64D.tag=27,((_tmp64D.f1=_tmp13->name,((_tmp64D.f2=0,((_tmp64D.f3=_tmp22,((_tmp64D.f4=_tmp13,_tmp64D)))))))))),_tmp64C))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp651;void*_tmp650[1];struct Cyc_String_pa_PrintArg_struct _tmp64F;(_tmp64F.tag=0,((_tmp64F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp650[0]=& _tmp64F,Cyc_Tcutil_terr(e->loc,((_tmp651="%s is an enum constructor, not a function",_tag_dyneither(_tmp651,sizeof(char),42))),_tag_dyneither(_tmp650,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp655;void*_tmp654[1];struct Cyc_String_pa_PrintArg_struct _tmp653;(_tmp653.tag=0,((_tmp653.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp654[0]=& _tmp653,Cyc_Tcutil_terr(e1->loc,((_tmp655="undeclared identifier %s",_tag_dyneither(_tmp655,sizeof(char),25))),_tag_dyneither(_tmp654,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp658;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp657;e->r=(void*)((_tmp657=_cycalloc(sizeof(*_tmp657)),((_tmp657[0]=((_tmp658.tag=25,((_tmp658.f1=des,_tmp658)))),_tmp657))));}
return;}{
# 125
void*t=*topt;
void*_tmp33=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp65B;void*_tmp65A;(_tmp65A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp65B="struct type not properly set",_tag_dyneither(_tmp65B,sizeof(char),29))),_tag_dyneither(_tmp65A,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp65E;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp65D;e->r=(void*)((_tmp65D=_cycalloc(sizeof(*_tmp65D)),((_tmp65D[0]=((_tmp65E.tag=27,((_tmp65E.f1=_tmp3B->name,((_tmp65E.f2=0,((_tmp65E.f3=des,((_tmp65E.f4=_tmp3B,_tmp65E)))))))))),_tmp65D))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp661;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp660;e->r=(void*)((_tmp660=_cycalloc(sizeof(*_tmp660)),((_tmp660[0]=((_tmp661.tag=25,((_tmp661.f1=des,_tmp661)))),_tmp660))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp664;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp663;e->r=(void*)((_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663[0]=((_tmp664.tag=28,((_tmp664.f1=t,((_tmp664.f2=des,_tmp664)))))),_tmp663))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp667;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp666;e->r=(void*)((_tmp666=_cycalloc(sizeof(*_tmp666)),((_tmp666[0]=((_tmp667.tag=25,((_tmp667.f1=des,_tmp667)))),_tmp666))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcutil_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp66C;void*_tmp66B[2];struct Cyc_String_pa_PrintArg_struct _tmp66A;struct Cyc_String_pa_PrintArg_struct _tmp669;(_tmp669.tag=0,((_tmp669.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp66A.tag=0,((_tmp66A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp66B[0]=& _tmp66A,((_tmp66B[1]=& _tmp669,Cyc_Tcutil_terr(e->loc,((_tmp66C="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp66C,sizeof(char),53))),_tag_dyneither(_tmp66B,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4A=e->r;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp66D;struct _tuple0 _tmp4F=(_tmp66D.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp66D.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp66D)));void*_tmp51;void*_tmp53;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f1;if(_tmp50->tag != 15)goto _LL2D;else{_tmp51=(void*)_tmp50->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f2;if(_tmp52->tag != 15)goto _LL2D;else{_tmp53=(void*)_tmp52->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp670;struct Cyc_Tcexp_TestEnv _tmp66F;return(_tmp66F.eq=((_tmp670=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp670)),((_tmp670->f1=_tmp51,((_tmp670->f2=_tmp53,_tmp670)))))),((_tmp66F.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp66F)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp671;return(_tmp671.eq=0,((_tmp671.isTrue=0,_tmp671)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp57=*((union Cyc_Absyn_Cnst*)_check_null(c));enum Cyc_Absyn_Sign _tmp58;enum Cyc_Absyn_Sign _tmp59;int _tmp5A;enum Cyc_Absyn_Sign _tmp5B;int _tmp5C;struct _dyneither_ptr _tmp5D;struct _dyneither_ptr _tmp5E;_LL30: if((_tmp57.Char_c).tag != 2)goto _LL32;if(((struct _tuple4)(_tmp57.Char_c).val).f1 != Cyc_Absyn_Signed)goto _LL32;_LL31:
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp57.Char_c).tag != 2)goto _LL34;if(((struct _tuple4)(_tmp57.Char_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp57.Char_c).tag != 2)goto _LL36;if(((struct _tuple4)(_tmp57.Char_c).val).f1 != Cyc_Absyn_None)goto _LL36;_LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp57.Wchar_c).tag != 3)goto _LL38;_LL37:
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp57.Short_c).tag != 4)goto _LL3A;_tmp58=((struct _tuple5)(_tmp57.Short_c).val).f1;_LL39:
# 210
 t=_tmp58 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp57.LongLong_c).tag != 6)goto _LL3C;_tmp59=((struct _tuple7)(_tmp57.LongLong_c).val).f1;_LL3B:
# 212
 t=_tmp59 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp57.Float_c).tag != 7)goto _LL3E;_tmp5A=((struct _tuple8)(_tmp57.Float_c).val).f2;_LL3D:
 t=Cyc_Absyn_float_typ(_tmp5A);goto _LL2F;_LL3E: if((_tmp57.Int_c).tag != 5)goto _LL40;_tmp5B=((struct _tuple6)(_tmp57.Int_c).val).f1;_tmp5C=((struct _tuple6)(_tmp57.Int_c).val).f2;_LL3F:
# 215
 if(topt == 0)
t=_tmp5B == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp5F=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp61;enum Cyc_Absyn_Sign _tmp63;enum Cyc_Absyn_Sign _tmp65;enum Cyc_Absyn_Sign _tmp67;void*_tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;union Cyc_Absyn_Constraint*_tmp6C;union Cyc_Absyn_Constraint*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;void*_tmp70;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F;if(_tmp60->tag != 6)goto _LL49;else{_tmp61=_tmp60->f1;if(_tmp60->f2 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp61){case Cyc_Absyn_Unsigned: _LL55:
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp61,(char)_tmp5C);
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F;if(_tmp62->tag != 6)goto _LL4B;else{_tmp63=_tmp62->f1;if(_tmp62->f2 != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp63 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp63,(short)_tmp5C);
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp64=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F;if(_tmp64->tag != 6)goto _LL4D;else{_tmp65=_tmp64->f1;if(_tmp64->f2 != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp65 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F;if(_tmp66->tag != 6)goto _LL4F;else{_tmp67=_tmp66->f1;if(_tmp66->f2 != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp67 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F;if(_tmp68->tag != 5)goto _LL51;else{_tmp69=(_tmp68->f1).elt_typ;_tmp6A=(_tmp68->f1).elt_tq;_tmp6B=((_tmp68->f1).ptr_atts).rgn;_tmp6C=((_tmp68->f1).ptr_atts).nullable;_tmp6D=((_tmp68->f1).ptr_atts).bounds;_tmp6E=((_tmp68->f1).ptr_atts).zero_term;}}if(!(_tmp5C == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp6C))return*topt;{
struct Cyc_List_List*_tmp71=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp67B;struct Cyc_Absyn_PtrAtts _tmp67A;struct Cyc_Absyn_PtrInfo _tmp679;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp678;t=(void*)((_tmp678=_cycalloc(sizeof(*_tmp678)),((_tmp678[0]=((_tmp67B.tag=5,((_tmp67B.f1=((_tmp679.elt_typ=_tmp69,((_tmp679.elt_tq=_tmp6A,((_tmp679.ptr_atts=(
(_tmp67A.rgn=_tmp6B,((_tmp67A.nullable=Cyc_Absyn_true_conref,((_tmp67A.bounds=_tmp6D,((_tmp67A.zero_term=_tmp6E,((_tmp67A.ptrloc=0,_tmp67A)))))))))),_tmp679)))))),_tmp67B)))),_tmp678))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp67E;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp67D;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp77=(_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=((_tmp67E.tag=18,((_tmp67E.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp67E)))),_tmp67D)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp681;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp680;t=(void*)((_tmp680=_cycalloc(sizeof(*_tmp680)),((_tmp680[0]=((_tmp681.tag=19,((_tmp681.f1=(void*)_tmp77,_tmp681)))),_tmp680))));}
goto _LL46;}_LL53:;_LL54:
# 261
 t=_tmp5B == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}
# 264
goto _LL2F;_LL40: if((_tmp57.String_c).tag != 8)goto _LL42;_tmp5D=(struct _dyneither_ptr)(_tmp57.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp5D,sizeof(char));
_tmp5E=_tmp5D;goto _LL43;_LL42: if((_tmp57.Wstring_c).tag != 9)goto _LL44;_tmp5E=(struct _dyneither_ptr)(_tmp57.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp5E);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp684;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp683;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7C=(_tmp683=_cycalloc(sizeof(*_tmp683)),((_tmp683[0]=((_tmp684.tag=1,((_tmp684.f1=elen,_tmp684)))),_tmp683)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp7C,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp7D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual _tmp7F;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7D;if(_tmp7E->tag != 8)goto _LL5C;else{_tmp7F=(_tmp7E->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp7F,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7D;if(_tmp80->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp7C,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 302
goto _LL59;_LL5E:;_LL5F:
 goto _LL59;_LL59:;}
# 306
return t;};};_LL44: if((_tmp57.Null_c).tag != 1)goto _LL2F;_LL45:
# 308
 if(topt != 0){
void*_tmp83=Cyc_Tcutil_compress(*topt);void*_tmp85;struct Cyc_Absyn_Tqual _tmp86;void*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;union Cyc_Absyn_Constraint*_tmp8A;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp83;if(_tmp84->tag != 5)goto _LL63;else{_tmp85=(_tmp84->f1).elt_typ;_tmp86=(_tmp84->f1).elt_tq;_tmp87=((_tmp84->f1).ptr_atts).rgn;_tmp88=((_tmp84->f1).ptr_atts).nullable;_tmp89=((_tmp84->f1).ptr_atts).bounds;_tmp8A=((_tmp84->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp88))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68E;struct Cyc_Absyn_PtrAtts _tmp68D;struct Cyc_Absyn_PtrInfo _tmp68C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68B;return(void*)((_tmp68B=_cycalloc(sizeof(*_tmp68B)),((_tmp68B[0]=((_tmp68E.tag=5,((_tmp68E.f1=((_tmp68C.elt_typ=_tmp85,((_tmp68C.elt_tq=_tmp86,((_tmp68C.ptr_atts=((_tmp68D.rgn=_tmp87,((_tmp68D.nullable=Cyc_Absyn_true_conref,((_tmp68D.bounds=_tmp89,((_tmp68D.zero_term=_tmp8A,((_tmp68D.ptrloc=0,_tmp68D)))))))))),_tmp68C)))))),_tmp68E)))),_tmp68B))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8F=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6A1;struct Cyc_Absyn_PtrAtts _tmp6A0;struct Cyc_Core_Opt*_tmp69F;struct Cyc_Core_Opt*_tmp69E;struct Cyc_Absyn_PtrInfo _tmp69D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69C;t=(void*)((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C[0]=((_tmp6A1.tag=5,((_tmp6A1.f1=((_tmp69D.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp69E=_cycalloc(sizeof(*_tmp69E)),((_tmp69E->v=_tmp8F,_tmp69E))))),((_tmp69D.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp69D.ptr_atts=(
(_tmp6A0.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp69F=_cycalloc(sizeof(*_tmp69F)),((_tmp69F->v=_tmp8F,_tmp69F))))),((_tmp6A0.nullable=Cyc_Absyn_true_conref,((_tmp6A0.bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6A0.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6A0.ptrloc=0,_tmp6A0)))))))))),_tmp69D)))))),_tmp6A1)))),_tmp69C))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp96=*((void**)_check_null(b));struct Cyc_Absyn_Vardecl*_tmp99;struct Cyc_Absyn_Fndecl*_tmp9B;struct Cyc_Absyn_Vardecl*_tmp9D;struct Cyc_Absyn_Vardecl*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA1;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp96;if(_tmp97->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmpA2;_push_handler(& _tmpA2);{int _tmpA4=0;if(setjmp(_tmpA2.handler))_tmpA4=1;if(!_tmpA4){
{struct _RegionHandle _tmpA5=_new_region("r");struct _RegionHandle*r=& _tmpA5;_push_region(r);
{void*_tmpA6=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpAA;struct Cyc_Absyn_Enumdecl*_tmpAC;struct Cyc_Absyn_Enumfield*_tmpAD;void*_tmpAF;struct Cyc_Absyn_Enumfield*_tmpB0;struct Cyc_Absyn_Datatypedecl*_tmpB2;struct Cyc_Absyn_Datatypefield*_tmpB3;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA7=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpA7->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpA8=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpA7->f1);if(_tmpA8->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp6A5;void*_tmp6A4[1];struct Cyc_String_pa_PrintArg_struct _tmp6A3;void*_tmpB8=(_tmp6A3.tag=0,((_tmp6A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6A4[0]=& _tmp6A3,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6A5="undeclared identifier %s",_tag_dyneither(_tmp6A5,sizeof(char),25))),_tag_dyneither(_tmp6A4,sizeof(void*),1)))))));_npop_handler(1);return _tmpB8;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA9=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpA9->tag != 0)goto _LL77;else{_tmpAA=(void*)_tmpA9->f1;}}_LL76:
# 348
*b=_tmpAA;{
void*_tmpB9=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB9;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAB->tag != 3)goto _LL79;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6A8;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6A7;e->r=(void*)((_tmp6A7=_cycalloc(sizeof(*_tmp6A7)),((_tmp6A7[0]=((_tmp6A8.tag=30,((_tmp6A8.f1=_tmpAD->name,((_tmp6A8.f2=_tmpAC,((_tmp6A8.f3=_tmpAD,_tmp6A8)))))))),_tmp6A7))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6AB;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6AA;void*_tmpBE=(void*)((_tmp6AA=_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=13,((_tmp6AB.f1=_tmpAC->name,((_tmp6AB.f2=_tmpAC,_tmp6AB)))))),_tmp6AA))));_npop_handler(1);return _tmpBE;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAE->tag != 4)goto _LL7B;else{_tmpAF=(void*)_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6AE;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6AD;e->r=(void*)((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=31,((_tmp6AE.f1=_tmpB0->name,((_tmp6AE.f2=_tmpAF,((_tmp6AE.f3=_tmpB0,_tmp6AE)))))))),_tmp6AD))));}{
void*_tmpC1=_tmpAF;_npop_handler(1);return _tmpC1;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB1->tag != 2)goto _LL7D;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6B1;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=29,((_tmp6B1.f1=0,((_tmp6B1.f2=_tmpB2,((_tmp6B1.f3=_tmpB3,_tmp6B1)))))))),_tmp6B0))));}{
void*_tmpC4=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpB2,_tmpB3);_npop_handler(1);return _tmpC4;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB4=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB4->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6B5;void*_tmp6B4[1];struct Cyc_String_pa_PrintArg_struct _tmp6B3;void*_tmpC8=(_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6B4[0]=& _tmp6B3,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6B5="bad occurrence of type name %s",_tag_dyneither(_tmp6B5,sizeof(char),31))),_tag_dyneither(_tmp6B4,sizeof(void*),1)))))));_npop_handler(1);return _tmpC8;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA3=(void*)_exn_thrown;void*_tmpCA=_tmpA3;void*_tmpCC;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpCB=(struct Cyc_Dict_Absent_exn_struct*)_tmpCA;if(_tmpCB->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6B9;void*_tmp6B8[1];struct Cyc_String_pa_PrintArg_struct _tmp6B7;return(_tmp6B7.tag=0,((_tmp6B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6B8[0]=& _tmp6B7,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6B9="undeclared identifier %s",_tag_dyneither(_tmp6B9,sizeof(char),25))),_tag_dyneither(_tmp6B8,sizeof(void*),1)))))));}_LL82: _tmpCC=_tmpCA;_LL83:(void)_throw(_tmpCC);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp98=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96;if(_tmp98->tag != 1)goto _LL6A;else{_tmp99=_tmp98->f1;}}_LL69:
# 368
*q=*_tmp99->name;
return _tmp99->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp9A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp96;if(_tmp9A->tag != 2)goto _LL6C;else{_tmp9B=_tmp9A->f1;}}_LL6B:
*q=*_tmp9B->name;return Cyc_Tcutil_fndecl2typ(_tmp9B);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp9C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96;if(_tmp9C->tag != 5)goto _LL6E;else{_tmp9D=_tmp9C->f1;}}_LL6D:
 _tmp9F=_tmp9D;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96;if(_tmp9E->tag != 4)goto _LL70;else{_tmp9F=_tmp9E->f1;}}_LL6F:
 _tmpA1=_tmp9F;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96;if(_tmpA0->tag != 3)goto _LL65;else{_tmpA1=_tmpA0->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6BA;(*q).f1=(((_tmp6BA.Loc_n).val=0,(((_tmp6BA.Loc_n).tag=4,_tmp6BA))));}
if(te->allow_valueof){
void*_tmpD1=Cyc_Tcutil_compress(_tmpA1->type);void*_tmpD3;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpD1;if(_tmpD2->tag != 19)goto _LL87;else{_tmpD3=(void*)_tmpD2->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6BD;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6BC;e->r=(void*)((_tmp6BC=_cycalloc(sizeof(*_tmp6BC)),((_tmp6BC[0]=((_tmp6BD.tag=37,((_tmp6BD.f1=_tmpD3,_tmp6BD)))),_tmp6BC))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmpA1->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpD6=fmt->r;struct _dyneither_ptr _tmpD8;struct _dyneither_ptr _tmpDB;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD6;if(_tmpD7->tag != 0)goto _LL8C;else{if(((_tmpD7->f1).String_c).tag != 8)goto _LL8C;_tmpD8=(struct _dyneither_ptr)((_tmpD7->f1).String_c).val;}}_LL8B:
 _tmpDB=_tmpD8;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6;if(_tmpD9->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmpD9->f2)->r;if(_tmpDA->tag != 0)goto _LL8E;else{if(((_tmpDA->f1).String_c).tag != 8)goto _LL8E;_tmpDB=(struct _dyneither_ptr)((_tmpDA->f1).String_c).val;}}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDB,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpDC=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpDC->hd);{
struct _RegionHandle _tmpDD=_new_region("temp");struct _RegionHandle*temp=& _tmpDD;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpDC->hd)){
const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->loc,((_tmp6C0="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6C0,sizeof(char),49))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpE0 != 0;
(((desc_types=desc_types->tl,_tmpE0=_tmpE0->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp6C5;void*_tmp6C4[2];struct Cyc_String_pa_PrintArg_struct _tmp6C3;struct Cyc_String_pa_PrintArg_struct _tmp6C2;(_tmp6C2.tag=0,((_tmp6C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6C3.tag=0,((_tmp6C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6C4[0]=& _tmp6C3,((_tmp6C4[1]=& _tmp6C2,Cyc_Tcutil_terr(e->loc,((_tmp6C5="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp6C5,sizeof(char),51))),_tag_dyneither(_tmp6C4,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6C6;*alias_arg_exps=((_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6->hd=(void*)arg_cnt,((_tmp6C6->tl=*alias_arg_exps,_tmp6C6))))));}{
struct _RegionHandle _tmpE6=_new_region("temp");struct _RegionHandle*temp=& _tmpE6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6C9="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6C9,sizeof(char),49))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6CC;void*_tmp6CB;(_tmp6CB=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6CC="too few arguments",_tag_dyneither(_tmp6CC,sizeof(char),18))),_tag_dyneither(_tmp6CB,sizeof(void*),0)));}
if(_tmpE0 != 0){
const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6CF="too many arguments",_tag_dyneither(_tmp6CF,sizeof(char),19))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6D3;void*_tmp6D2[1];struct Cyc_String_pa_PrintArg_struct _tmp6D1;(_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6D2[0]=& _tmp6D1,Cyc_Tcutil_terr(loc,((_tmp6D3="expecting arithmetic type but found %s",_tag_dyneither(_tmp6D3,sizeof(char),39))),_tag_dyneither(_tmp6D2,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6D7;void*_tmp6D6[1];struct Cyc_String_pa_PrintArg_struct _tmp6D5;(_tmp6D5.tag=0,((_tmp6D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6D6[0]=& _tmp6D5,Cyc_Tcutil_terr(loc,((_tmp6D7="expecting integral or * type but found %s",_tag_dyneither(_tmp6D7,sizeof(char),42))),_tag_dyneither(_tmp6D6,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6DB;void*_tmp6DA[1];struct Cyc_String_pa_PrintArg_struct _tmp6D9;(_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6DA[0]=& _tmp6D9,Cyc_Tcutil_terr(loc,((_tmp6DB="expecting integral type but found %s",_tag_dyneither(_tmp6DB,sizeof(char),37))),_tag_dyneither(_tmp6DA,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF6=t;union Cyc_Absyn_Constraint*_tmpF9;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF8->tag != 5)goto _LL9B;else{_tmpF9=((_tmpF8->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_tmpFA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF9);struct Cyc_Absyn_Exp*_tmpFD;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFB->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFC->tag != 1)goto _LL9D;else{_tmpFD=_tmpFC->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFD)){
const char*_tmp6DE;void*_tmp6DD;(_tmp6DD=0,Cyc_Tcutil_terr(loc,((_tmp6DE="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp6DE,sizeof(char),55))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp6E2;void*_tmp6E1[1];struct Cyc_String_pa_PrintArg_struct _tmp6E0;(_tmp6E0.tag=0,((_tmp6E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E1[0]=& _tmp6E0,Cyc_Tcutil_terr(loc,((_tmp6E2="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp6E2,sizeof(char),47))),_tag_dyneither(_tmp6E1,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp6E5;void*_tmp6E4;(_tmp6E4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6E5="Non-unary primop",_tag_dyneither(_tmp6E5,sizeof(char),17))),_tag_dyneither(_tmp6E4,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp6E9;void*_tmp6E8[1];struct Cyc_String_pa_PrintArg_struct _tmp6E7;(_tmp6E7.tag=0,((_tmp6E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp6E8[0]=& _tmp6E7,Cyc_Tcutil_terr(e1->loc,((_tmp6E9="type %s cannot be used here",_tag_dyneither(_tmp6E9,sizeof(char),28))),_tag_dyneither(_tmp6E8,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp6ED;void*_tmp6EC[1];struct Cyc_String_pa_PrintArg_struct _tmp6EB;(_tmp6EB.tag=0,((_tmp6EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp6EC[0]=& _tmp6EB,Cyc_Tcutil_terr(e2->loc,((_tmp6ED="type %s cannot be used here",_tag_dyneither(_tmp6ED,sizeof(char),28))),_tag_dyneither(_tmp6EC,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 495
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 500
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp10B=t1;void*_tmp10D;struct Cyc_Absyn_Tqual _tmp10E;void*_tmp10F;union Cyc_Absyn_Constraint*_tmp110;union Cyc_Absyn_Constraint*_tmp111;union Cyc_Absyn_Constraint*_tmp112;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10B;if(_tmp10C->tag != 5)goto _LLA6;else{_tmp10D=(_tmp10C->f1).elt_typ;_tmp10E=(_tmp10C->f1).elt_tq;_tmp10F=((_tmp10C->f1).ptr_atts).rgn;_tmp110=((_tmp10C->f1).ptr_atts).nullable;_tmp111=((_tmp10C->f1).ptr_atts).bounds;_tmp112=((_tmp10C->f1).ptr_atts).zero_term;}}_LLA5:
# 505
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp10D),& Cyc_Tcutil_tmk)){
const char*_tmp6F0;void*_tmp6EF;(_tmp6EF=0,Cyc_Tcutil_terr(e1->loc,((_tmp6F0="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp6F0,sizeof(char),50))),_tag_dyneither(_tmp6EF,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp6F3;void*_tmp6F2;(_tmp6F2=0,Cyc_Tcutil_terr(e1->loc,((_tmp6F3="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp6F3,sizeof(char),54))),_tag_dyneither(_tmp6F2,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp6F7;void*_tmp6F6[1];struct Cyc_String_pa_PrintArg_struct _tmp6F5;(_tmp6F5.tag=0,((_tmp6F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6F6[0]=& _tmp6F5,Cyc_Tcutil_terr(e2->loc,((_tmp6F7="expecting int but found %s",_tag_dyneither(_tmp6F7,sizeof(char),27))),_tag_dyneither(_tmp6F6,sizeof(void*),1)))))));}{
void*_tmp11A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp111);struct Cyc_Absyn_Exp*_tmp11D;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11B->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11C->tag != 1)goto _LLA8;else{_tmp11D=_tmp11C->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp112)){
const char*_tmp6FA;void*_tmp6F9;(_tmp6F9=0,Cyc_Tcutil_warn(e1->loc,((_tmp6FA="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp6FA,sizeof(char),70))),_tag_dyneither(_tmp6F9,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp704;struct Cyc_Absyn_PtrAtts _tmp703;struct Cyc_Absyn_PtrInfo _tmp702;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp701;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp120=(_tmp701=_cycalloc(sizeof(*_tmp701)),((_tmp701[0]=((_tmp704.tag=5,((_tmp704.f1=((_tmp702.elt_typ=_tmp10D,((_tmp702.elt_tq=_tmp10E,((_tmp702.ptr_atts=(
(_tmp703.rgn=_tmp10F,((_tmp703.nullable=Cyc_Absyn_true_conref,((_tmp703.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp703.zero_term=_tmp112,((_tmp703.ptrloc=0,_tmp703)))))))))),_tmp702)))))),_tmp704)))),_tmp701)));
# 528
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp120,Cyc_Absyn_Other_coercion);
return(void*)_tmp120;};_LLA8:;};_LLA6:;_LLA7:
# 531
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 536
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp709;void*_tmp708[2];struct Cyc_String_pa_PrintArg_struct _tmp707;struct Cyc_String_pa_PrintArg_struct _tmp706;(_tmp706.tag=0,((_tmp706.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp707.tag=0,((_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp708[0]=& _tmp707,((_tmp708[1]=& _tmp706,Cyc_Tcutil_terr(e1->loc,((_tmp709="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp709,sizeof(char),59))),_tag_dyneither(_tmp708,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp70E;void*_tmp70D[2];struct Cyc_String_pa_PrintArg_struct _tmp70C;struct Cyc_String_pa_PrintArg_struct _tmp70B;(_tmp70B.tag=0,((_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp70C.tag=0,((_tmp70C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp70D[0]=& _tmp70C,((_tmp70D[1]=& _tmp70B,Cyc_Tcutil_terr(e1->loc,((_tmp70E="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp70E,sizeof(char),59))),_tag_dyneither(_tmp70D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp711;void*_tmp710;(_tmp710=0,Cyc_Tcutil_warn(e1->loc,((_tmp711="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp711,sizeof(char),60))),_tag_dyneither(_tmp710,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp714;void*_tmp713;(_tmp713=0,Cyc_Tcutil_terr(e1->loc,((_tmp714="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp714,sizeof(char),50))),_tag_dyneither(_tmp713,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_terr(e1->loc,((_tmp717="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp717,sizeof(char),54))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp71C;void*_tmp71B[2];struct Cyc_String_pa_PrintArg_struct _tmp71A;struct Cyc_String_pa_PrintArg_struct _tmp719;(_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp71A.tag=0,((_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp71B[0]=& _tmp71A,((_tmp71B[1]=& _tmp719,Cyc_Tcutil_terr(e2->loc,((_tmp71C="expecting either %s or int but found %s",_tag_dyneither(_tmp71C,sizeof(char),40))),_tag_dyneither(_tmp71B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp71F;void*_tmp71E;(_tmp71E=0,Cyc_Tcutil_warn(e1->loc,((_tmp71F="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp71F,sizeof(char),60))),_tag_dyneither(_tmp71E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp722;void*_tmp721;(_tmp721=0,Cyc_Tcutil_warn(e1->loc,((_tmp722="thin pointer subtraction!",_tag_dyneither(_tmp722,sizeof(char),26))),_tag_dyneither(_tmp721,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp725;void*_tmp724;(_tmp724=0,Cyc_Tcutil_warn(e1->loc,((_tmp725="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp725,sizeof(char),56))),_tag_dyneither(_tmp724,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp728;void*_tmp727;(_tmp727=0,Cyc_Tcutil_warn(e1->loc,((_tmp728="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp728,sizeof(char),51))),_tag_dyneither(_tmp727,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 598
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 601
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 609
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 614
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 627
pointer_cmp: {
struct _tuple0 _tmp729;struct _tuple0 _tmp140=(_tmp729.f1=Cyc_Tcutil_compress(t1),((_tmp729.f2=Cyc_Tcutil_compress(t2),_tmp729)));void*_tmp142;void*_tmp144;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140.f1;if(_tmp141->tag != 5)goto _LLB0;else{_tmp142=(_tmp141->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140.f2;if(_tmp143->tag != 5)goto _LLB0;else{_tmp144=(_tmp143->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp142,_tmp144))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp140.f1;if(_tmp145->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp140.f2;if(_tmp146->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp72E;void*_tmp72D[2];struct Cyc_String_pa_PrintArg_struct _tmp72C;struct Cyc_String_pa_PrintArg_struct _tmp72B;(_tmp72B.tag=0,((_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp72C.tag=0,((_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp72D[0]=& _tmp72C,((_tmp72D[1]=& _tmp72B,Cyc_Tcutil_terr(loc,((_tmp72E="comparison not allowed between %s and %s",_tag_dyneither(_tmp72E,sizeof(char),41))),_tag_dyneither(_tmp72D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 643
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 645
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 651
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp72F;struct _tuple0 _tmp14C=(_tmp72F.f1=t1,((_tmp72F.f2=t2,_tmp72F)));void*_tmp14E;void*_tmp150;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14C.f1;if(_tmp14D->tag != 15)goto _LLB7;else{_tmp14E=(void*)_tmp14D->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14C.f2;if(_tmp14F->tag != 15)goto _LLB7;else{_tmp150=(void*)_tmp14F->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp734;void*_tmp733[2];struct Cyc_String_pa_PrintArg_struct _tmp732;struct Cyc_String_pa_PrintArg_struct _tmp731;(_tmp731.tag=0,((_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp732.tag=0,((_tmp732.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp733[0]=& _tmp732,((_tmp733[1]=& _tmp731,Cyc_Tcutil_terr(loc,((_tmp734="comparison not allowed between %s and %s",_tag_dyneither(_tmp734,sizeof(char),41))),_tag_dyneither(_tmp733,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 667
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 669
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 673
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 676
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 686
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 689
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 694
const char*_tmp737;void*_tmp736;(_tmp736=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp737="bad binary primop",_tag_dyneither(_tmp737,sizeof(char),18))),_tag_dyneither(_tmp736,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
const char*_tmp73A;void*_tmp739;return(_tmp739=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp73A="primitive operator has 0 arguments",_tag_dyneither(_tmp73A,sizeof(char),35))),_tag_dyneither(_tmp739,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp73D;void*_tmp73C;return(_tmp73C=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp73D="primitive operator has > 2 arguments",_tag_dyneither(_tmp73D,sizeof(char),37))),_tag_dyneither(_tmp73C,sizeof(void*),0)));}}
# 716
return t;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 719
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp15B=Cyc_Tcutil_compress(t);
void*_tmp15C=_tmp15B;struct Cyc_Absyn_Aggrdecl*_tmp15E;struct Cyc_List_List*_tmp160;struct Cyc_Absyn_Datatypefield*_tmp162;void*_tmp164;struct Cyc_Absyn_Tqual _tmp165;struct Cyc_List_List*_tmp167;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15C;if(_tmp15D->tag != 11)goto _LLD4;else{if((((_tmp15D->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp15E=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp15D->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 723
 _tmp160=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15E->impl))->fields;goto _LLD5;_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15C;if(_tmp15F->tag != 12)goto _LLD6;else{_tmp160=_tmp15F->f2;}}_LLD5:
# 725
 for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
struct Cyc_Absyn_Aggrfield*_tmp168=(struct Cyc_Absyn_Aggrfield*)_tmp160->hd;
if((_tmp168->tq).real_const){
{const char*_tmp741;void*_tmp740[1];struct Cyc_String_pa_PrintArg_struct _tmp73F;(_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp168->name),((_tmp740[0]=& _tmp73F,Cyc_Tcutil_terr(loc,((_tmp741="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp741,sizeof(char),56))),_tag_dyneither(_tmp740,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp168->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15C;if(_tmp161->tag != 4)goto _LLD8;else{if((((_tmp161->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp162=((struct _tuple2)(((_tmp161->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp16C=_tmp162->typs;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct Cyc_Absyn_Tqual _tmp16E;void*_tmp16F;struct _tuple19*_tmp16D=(struct _tuple19*)_tmp16C->hd;_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
if(_tmp16E.real_const){
{const char*_tmp745;void*_tmp744[1];struct Cyc_String_pa_PrintArg_struct _tmp743;(_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp162->name)),((_tmp744[0]=& _tmp743,Cyc_Tcutil_terr(loc,((_tmp745="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp745,sizeof(char),64))),_tag_dyneither(_tmp744,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16F))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15C;if(_tmp163->tag != 8)goto _LLDA;else{_tmp164=(_tmp163->f1).elt_type;_tmp165=(_tmp163->f1).tq;}}_LLD9:
# 745
 if(_tmp165.real_const){
{const char*_tmp748;void*_tmp747;(_tmp747=0,Cyc_Tcutil_terr(loc,((_tmp748="attempt to over-write a const array",_tag_dyneither(_tmp748,sizeof(char),36))),_tag_dyneither(_tmp747,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp164);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15C;if(_tmp166->tag != 10)goto _LLDC;else{_tmp167=_tmp166->f1;}}_LLDB:
# 751
 for(0;_tmp167 != 0;_tmp167=_tmp167->tl){
struct Cyc_Absyn_Tqual _tmp176;void*_tmp177;struct _tuple19*_tmp175=(struct _tuple19*)_tmp167->hd;_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;
if(_tmp176.real_const){
{const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_terr(loc,((_tmp74B="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp74B,sizeof(char),56))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}
return 0;}
# 757
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp177))return 0;}
# 759
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 767
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 770
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp17A=e->r;struct Cyc_Absyn_Vardecl*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp180;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Vardecl*_tmp186;struct Cyc_Absyn_Exp*_tmp188;struct Cyc_Absyn_Exp*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct _dyneither_ptr*_tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct _dyneither_ptr*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp195;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp17B->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp17C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp17B->f2);if(_tmp17C->tag != 3)goto _LLE1;else{_tmp17D=_tmp17C->f1;}}}_LLE0:
 _tmp180=_tmp17D;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp17E->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17F=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17E->f2);if(_tmp17F->tag != 4)goto _LLE3;else{_tmp180=_tmp17F->f1;}}}_LLE2:
 _tmp183=_tmp180;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp181->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp182=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp181->f2);if(_tmp182->tag != 5)goto _LLE5;else{_tmp183=_tmp182->f1;}}}_LLE4:
 _tmp186=_tmp183;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp184->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp185=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp184->f2);if(_tmp185->tag != 1)goto _LLE7;else{_tmp186=_tmp185->f1;}}}_LLE6:
 if(!(_tmp186->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp187->tag != 22)goto _LLE9;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LLE8:
# 777
{void*_tmp196=Cyc_Tcutil_compress((void*)_check_null(_tmp188->topt));struct Cyc_Absyn_Tqual _tmp198;struct Cyc_Absyn_Tqual _tmp19A;struct Cyc_List_List*_tmp19C;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp196;if(_tmp197->tag != 5)goto _LLF8;else{_tmp198=(_tmp197->f1).elt_tq;}}_LLF7:
 _tmp19A=_tmp198;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp196;if(_tmp199->tag != 8)goto _LLFA;else{_tmp19A=(_tmp199->f1).tq;}}_LLF9:
 if(!_tmp19A.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp196;if(_tmp19B->tag != 10)goto _LLFC;else{_tmp19C=_tmp19B->f1;}}_LLFB: {
# 781
unsigned int _tmp19E;int _tmp19F;struct _tuple15 _tmp19D=Cyc_Evexp_eval_const_uint_exp(_tmp189);_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;
if(!_tmp19F){
{const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_terr(e->loc,((_tmp74E="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp74E,sizeof(char),47))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}
return;}
# 786
{struct _handler_cons _tmp1A2;_push_handler(& _tmp1A2);{int _tmp1A4=0;if(setjmp(_tmp1A2.handler))_tmp1A4=1;if(!_tmp1A4){
{struct Cyc_Absyn_Tqual _tmp1A6;struct _tuple19*_tmp1A5=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp19C,(int)_tmp19E);_tmp1A6=_tmp1A5->f1;
if(!_tmp1A6.real_const){_npop_handler(0);return;}}
# 787
;_pop_handler();}else{void*_tmp1A3=(void*)_exn_thrown;void*_tmp1A8=_tmp1A3;void*_tmp1AA;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1A9=(struct Cyc_List_Nth_exn_struct*)_tmp1A8;if(_tmp1A9->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 789
 return;_LL101: _tmp1AA=_tmp1A8;_LL102:(void)_throw(_tmp1AA);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 793
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp18A->tag != 20)goto _LLEB;else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LLEA:
# 795
{void*_tmp1AB=Cyc_Tcutil_compress((void*)_check_null(_tmp18B->topt));struct Cyc_Absyn_Aggrdecl**_tmp1AD;struct Cyc_List_List*_tmp1AF;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AB;if(_tmp1AC->tag != 11)goto _LL106;else{if((((_tmp1AC->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1AD=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1AC->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 797
struct Cyc_Absyn_Aggrfield*sf=
_tmp1AD == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1AD,_tmp18C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AB;if(_tmp1AE->tag != 12)goto _LL108;else{_tmp1AF=_tmp1AE->f2;}}_LL107: {
# 802
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1AF,_tmp18C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 807
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp18D->tag != 21)goto _LLED;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LLEC:
# 809
{void*_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp18E->topt));void*_tmp1B2;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B1->tag != 5)goto _LL10D;else{_tmp1B2=(_tmp1B1->f1).elt_typ;}}_LL10C:
# 811
{void*_tmp1B3=Cyc_Tcutil_compress(_tmp1B2);struct Cyc_Absyn_Aggrdecl**_tmp1B5;struct Cyc_List_List*_tmp1B7;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1B4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B3;if(_tmp1B4->tag != 11)goto _LL112;else{if((((_tmp1B4->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1B5=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1B4->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 813
struct Cyc_Absyn_Aggrfield*sf=
_tmp1B5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1B5,_tmp18F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B3;if(_tmp1B6->tag != 12)goto _LL114;else{_tmp1B7=_tmp1B6->f2;}}_LL113: {
# 818
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1B7,_tmp18F);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 823
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 826
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp190->tag != 19)goto _LLEF;else{_tmp191=_tmp190->f1;}}_LLEE:
# 828
{void*_tmp1B8=Cyc_Tcutil_compress((void*)_check_null(_tmp191->topt));struct Cyc_Absyn_Tqual _tmp1BA;struct Cyc_Absyn_Tqual _tmp1BC;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B8;if(_tmp1B9->tag != 5)goto _LL119;else{_tmp1BA=(_tmp1B9->f1).elt_tq;}}_LL118:
 _tmp1BC=_tmp1BA;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B8;if(_tmp1BB->tag != 8)goto _LL11B;else{_tmp1BC=(_tmp1BB->f1).tq;}}_LL11A:
 if(!_tmp1BC.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 833
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp192->tag != 11)goto _LLF1;else{_tmp193=_tmp192->f1;}}_LLF0:
 _tmp195=_tmp193;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17A;if(_tmp194->tag != 12)goto _LLF3;else{_tmp195=_tmp194->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp195);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 838
const char*_tmp752;void*_tmp751[1];struct Cyc_String_pa_PrintArg_struct _tmp750;(_tmp750.tag=0,((_tmp750.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp751[0]=& _tmp750,Cyc_Tcutil_terr(e->loc,((_tmp752="attempt to write a const location: %s",_tag_dyneither(_tmp752,sizeof(char),38))),_tag_dyneither(_tmp751,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1C0=_new_region("temp");struct _RegionHandle*temp=& _tmp1C0;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp755;void*_tmp754;void*_tmp1C3=(_tmp754=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp755="increment/decrement of non-lvalue",_tag_dyneither(_tmp755,sizeof(char),34))),_tag_dyneither(_tmp754,sizeof(void*),0)));_npop_handler(0);return _tmp1C3;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_terr(e->loc,((_tmp758="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp758,sizeof(char),50))),_tag_dyneither(_tmp757,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp75B;void*_tmp75A;(_tmp75A=0,Cyc_Tcutil_terr(e->loc,((_tmp75B="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp75B,sizeof(char),54))),_tag_dyneither(_tmp75A,sizeof(void*),0)));}}else{
# 861
const char*_tmp75F;void*_tmp75E[1];struct Cyc_String_pa_PrintArg_struct _tmp75D;(_tmp75D.tag=0,((_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75E[0]=& _tmp75D,Cyc_Tcutil_terr(e->loc,((_tmp75F="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp75F,sizeof(char),44))),_tag_dyneither(_tmp75E,sizeof(void*),1)))))));}}{
# 863
void*_tmp1CB=t;_npop_handler(0);return _tmp1CB;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _tuple0*_tmp1CE;int _tmp1CF;const char*_tmp760;struct Cyc_Tcexp_TestEnv _tmp1CD=Cyc_Tcexp_tcTest(te,e1,((_tmp760="conditional expression",_tag_dyneither(_tmp760,sizeof(char),23))));_tmp1CE=_tmp1CD.eq;_tmp1CF=_tmp1CD.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_List_List _tmp761;struct Cyc_List_List _tmp1D0=(_tmp761.hd=e3,((_tmp761.tl=0,_tmp761)));
struct Cyc_List_List _tmp762;struct Cyc_List_List _tmp1D1=(_tmp762.hd=e2,((_tmp762.tl=& _tmp1D0,_tmp762)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1D1)){
{const char*_tmp767;void*_tmp766[2];struct Cyc_String_pa_PrintArg_struct _tmp765;struct Cyc_String_pa_PrintArg_struct _tmp764;(_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp766[0]=& _tmp765,((_tmp766[1]=& _tmp764,Cyc_Tcutil_terr(loc,((_tmp767="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp767,sizeof(char),48))),_tag_dyneither(_tmp766,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 880
return t;};}
# 884
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 886
{const char*_tmp768;Cyc_Tcexp_tcTest(te,e1,((_tmp768="logical-and expression",_tag_dyneither(_tmp768,sizeof(char),23))));}
{const char*_tmp769;Cyc_Tcexp_tcTest(te,e2,((_tmp769="logical-and expression",_tag_dyneither(_tmp769,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 892
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 894
{const char*_tmp76A;Cyc_Tcexp_tcTest(te,e1,((_tmp76A="logical-or expression",_tag_dyneither(_tmp76A,sizeof(char),22))));}
{const char*_tmp76B;Cyc_Tcexp_tcTest(te,e2,((_tmp76B="logical-or expression",_tag_dyneither(_tmp76B,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 900
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 907
struct _RegionHandle _tmp1DC=_new_region("r");struct _RegionHandle*r=& _tmp1DC;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 910
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 914
{void*_tmp1DD=Cyc_Tcutil_compress(t1);_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DD;if(_tmp1DE->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_terr(loc,((_tmp76E="cannot assign to an array",_tag_dyneither(_tmp76E,sizeof(char),26))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 919
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_Tcutil_terr(loc,((_tmp771="type is abstract (can't determine size).",_tag_dyneither(_tmp771,sizeof(char),41))),_tag_dyneither(_tmp770,sizeof(void*),0)));}
# 923
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp774;void*_tmp773;void*_tmp1E5=(_tmp773=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp774="assignment to non-lvalue",_tag_dyneither(_tmp774,sizeof(char),25))),_tag_dyneither(_tmp773,sizeof(void*),0)));_npop_handler(0);return _tmp1E5;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E6=_new_region("temp");struct _RegionHandle*temp=& _tmp1E6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp777;void*_tmp776;(_tmp776=0,Cyc_Tcutil_terr(e2->loc,((_tmp777="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp777,sizeof(char),49))),_tag_dyneither(_tmp776,sizeof(void*),0)));}
# 931
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp77C;void*_tmp77B[2];struct Cyc_String_pa_PrintArg_struct _tmp77A;struct Cyc_String_pa_PrintArg_struct _tmp779;void*_tmp1E9=(_tmp779.tag=0,((_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp77A.tag=0,((_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp77B[0]=& _tmp77A,((_tmp77B[1]=& _tmp779,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp77C="type mismatch: %s != %s",_tag_dyneither(_tmp77C,sizeof(char),24))),_tag_dyneither(_tmp77B,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1EA=_tmp1E9;_npop_handler(1);return _tmp1EA;};}
# 927
;_pop_region(temp);}else{
# 939
enum Cyc_Absyn_Primop _tmp1EF=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1F0=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EF,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1F0,t1) || Cyc_Tcutil_coerceable(_tmp1F0) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp781;void*_tmp780[2];struct Cyc_String_pa_PrintArg_struct _tmp77F;struct Cyc_String_pa_PrintArg_struct _tmp77E;void*_tmp1F1=
(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 945
Cyc_Absynpp_typ2string(t2)),((_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 944
Cyc_Absynpp_typ2string(t1)),((_tmp780[0]=& _tmp77F,((_tmp780[1]=& _tmp77E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp781="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp781,sizeof(char),82))),_tag_dyneither(_tmp780,sizeof(void*),2)))))))))))));
# 946
Cyc_Tcutil_unify(_tmp1F0,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1F2=_tmp1F1;_npop_handler(0);return _tmp1F2;};}{
# 950
void*_tmp1F7=_tmp1F0;_npop_handler(0);return _tmp1F7;};}{
# 952
void*_tmp1F8=t1;_npop_handler(0);return _tmp1F8;};};};
# 907
;_pop_region(r);}
# 956
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_tcExp(te,topt,e2);
return(void*)_check_null(e2->topt);}
# 963
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 967
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 970
{void*_tmp1F9=Cyc_Tcutil_compress(t1);_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1FA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F9;if(_tmp1FA->tag != 7)goto _LL125;else{if(_tmp1FA->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F9;if(_tmp1FB->tag != 6)goto _LL127;else{if(_tmp1FB->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F9;if(_tmp1FC->tag != 6)goto _LL129;else{if(_tmp1FC->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 977
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp1FE;struct Cyc_List_List*_tmp1FF;unsigned int _tmp200;enum Cyc_Absyn_Scope _tmp201;struct Cyc_Absyn_Datatypefield _tmp1FD=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1FE=_tmp1FD.name;_tmp1FF=_tmp1FD.typs;_tmp200=_tmp1FD.loc;_tmp201=_tmp1FD.sc;
# 980
if(_tmp1FF == 0  || _tmp1FF->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp1FF->hd)).f2);
# 983
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 988
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp203;struct Cyc_List_List*_tmp204;unsigned int _tmp205;enum Cyc_Absyn_Scope _tmp206;struct Cyc_Absyn_Datatypefield _tmp202=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp203=_tmp202.name;_tmp204=_tmp202.typs;_tmp205=_tmp202.loc;_tmp206=_tmp202.sc;
# 991
if(_tmp204 == 0  || _tmp204->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp204->hd)).f2);
# 995
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1000
{const char*_tmp786;void*_tmp785[2];struct Cyc_String_pa_PrintArg_struct _tmp784;struct Cyc_String_pa_PrintArg_struct _tmp783;(_tmp783.tag=0,((_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp785[0]=& _tmp784,((_tmp785[1]=& _tmp783,Cyc_Tcutil_terr(e->loc,((_tmp786="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp786,sizeof(char),54))),_tag_dyneither(_tmp785,sizeof(void*),2)))))))))))));}
return 0;}
# 1006
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1012
struct Cyc_List_List*_tmp20B=args;
int _tmp20C=0;
struct _RegionHandle _tmp20D=_new_region("ter");struct _RegionHandle*ter=& _tmp20D;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp20E=Cyc_Tcenv_new_block(ter,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp20E,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1021
void*_tmp20F=t;void*_tmp211;struct Cyc_Absyn_Tqual _tmp212;void*_tmp213;union Cyc_Absyn_Constraint*_tmp214;union Cyc_Absyn_Constraint*_tmp215;union Cyc_Absyn_Constraint*_tmp216;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20F;if(_tmp210->tag != 5)goto _LL12E;else{_tmp211=(_tmp210->f1).elt_typ;_tmp212=(_tmp210->f1).elt_tq;_tmp213=((_tmp210->f1).ptr_atts).rgn;_tmp214=((_tmp210->f1).ptr_atts).nullable;_tmp215=((_tmp210->f1).ptr_atts).bounds;_tmp216=((_tmp210->f1).ptr_atts).zero_term;}}_LL12D:
# 1026
 Cyc_Tcenv_check_rgn_accessible(_tmp20E,loc,_tmp213);
# 1028
Cyc_Tcutil_check_nonzero_bound(loc,_tmp215);{
void*_tmp217=Cyc_Tcutil_compress(_tmp211);struct Cyc_List_List*_tmp219;void*_tmp21A;struct Cyc_Absyn_Tqual _tmp21B;void*_tmp21C;struct Cyc_List_List*_tmp21D;int _tmp21E;struct Cyc_Absyn_VarargInfo*_tmp21F;struct Cyc_List_List*_tmp220;struct Cyc_List_List*_tmp221;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp217;if(_tmp218->tag != 9)goto _LL133;else{_tmp219=(_tmp218->f1).tvars;_tmp21A=(_tmp218->f1).effect;_tmp21B=(_tmp218->f1).ret_tqual;_tmp21C=(_tmp218->f1).ret_typ;_tmp21D=(_tmp218->f1).args;_tmp21E=(_tmp218->f1).c_varargs;_tmp21F=(_tmp218->f1).cyc_varargs;_tmp220=(_tmp218->f1).rgn_po;_tmp221=(_tmp218->f1).attributes;}}_LL132:
# 1035
 if(topt != 0)Cyc_Tcutil_unify(_tmp21C,*topt);
# 1037
while(_tmp20B != 0  && _tmp21D != 0){
# 1039
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp20B->hd;
void*t2=(*((struct _tuple9*)_tmp21D->hd)).f3;
Cyc_Tcexp_tcExp(_tmp20E,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20E,e1,t2,& alias_coercion)){
{const char*_tmp78B;void*_tmp78A[2];struct Cyc_String_pa_PrintArg_struct _tmp789;struct Cyc_String_pa_PrintArg_struct _tmp788;(_tmp788.tag=0,((_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1047
Cyc_Absynpp_typ2string(t2)),((_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp78A[0]=& _tmp789,((_tmp78A[1]=& _tmp788,Cyc_Tcutil_terr(e1->loc,((_tmp78B="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp78B,sizeof(char),57))),_tag_dyneither(_tmp78A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1052
if(alias_coercion){
struct Cyc_List_List*_tmp78C;*alias_arg_exps=((_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C->hd=(void*)_tmp20C,((_tmp78C->tl=*alias_arg_exps,_tmp78C))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp78F;void*_tmp78E;(_tmp78E=0,Cyc_Tcutil_terr(e1->loc,((_tmp78F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp78F,sizeof(char),49))),_tag_dyneither(_tmp78E,sizeof(void*),0)));}
_tmp20B=_tmp20B->tl;
_tmp21D=_tmp21D->tl;
++ _tmp20C;}{
# 1063
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp221;for(0;a != 0;a=a->tl){
void*_tmp229=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp22B;int _tmp22C;int _tmp22D;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp22A=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp229;if(_tmp22A->tag != 19)goto _LL138;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;_tmp22D=_tmp22A->f3;}}_LL137:
# 1067
{struct _handler_cons _tmp22E;_push_handler(& _tmp22E);{int _tmp230=0;if(setjmp(_tmp22E.handler))_tmp230=1;if(!_tmp230){
# 1069
{struct Cyc_Absyn_Exp*_tmp231=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp22C - 1);
# 1071
struct Cyc_Core_Opt*fmt_args;
if(_tmp22D == 0)
fmt_args=0;else{
# 1075
struct Cyc_Core_Opt*_tmp790;fmt_args=((_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp22D - 1),_tmp790))));}
args_already_checked=1;{
struct _RegionHandle _tmp233=_new_region("temp");struct _RegionHandle*temp=& _tmp233;_push_region(temp);
switch(_tmp22B){case Cyc_Absyn_Printf_ft: _LL13A:
# 1080
 Cyc_Tcexp_check_format_args(_tmp20E,_tmp231,fmt_args,_tmp22D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1083
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1085
 Cyc_Tcexp_check_format_args(_tmp20E,_tmp231,fmt_args,_tmp22D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1088
break;}
# 1078
;_pop_region(temp);};}
# 1069
;_pop_handler();}else{void*_tmp22F=(void*)_exn_thrown;void*_tmp235=_tmp22F;void*_tmp237;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp236=(struct Cyc_List_Nth_exn_struct*)_tmp235;if(_tmp236->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1092
{const char*_tmp793;void*_tmp792;(_tmp792=0,Cyc_Tcutil_terr(loc,((_tmp793="bad format arguments",_tag_dyneither(_tmp793,sizeof(char),21))),_tag_dyneither(_tmp792,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp237=_tmp235;_LL141:(void)_throw(_tmp237);_LL13D:;}};}
# 1094
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1098
if(_tmp21D != 0){const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_terr(loc,((_tmp796="too few arguments for function",_tag_dyneither(_tmp796,sizeof(char),31))),_tag_dyneither(_tmp795,sizeof(void*),0)));}else{
# 1100
if((_tmp20B != 0  || _tmp21E) || _tmp21F != 0){
if(_tmp21E)
for(0;_tmp20B != 0;_tmp20B=_tmp20B->tl){
Cyc_Tcexp_tcExp(_tmp20E,0,(struct Cyc_Absyn_Exp*)_tmp20B->hd);}else{
if(_tmp21F == 0){
const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_terr(loc,((_tmp799="too many arguments for function",_tag_dyneither(_tmp799,sizeof(char),32))),_tag_dyneither(_tmp798,sizeof(void*),0)));}else{
# 1107
void*_tmp23F;int _tmp240;struct Cyc_Absyn_VarargInfo _tmp23E=*_tmp21F;_tmp23F=_tmp23E.type;_tmp240=_tmp23E.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp79A;struct Cyc_Absyn_VarargCallInfo*_tmp241=(_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A->num_varargs=0,((_tmp79A->injectors=0,((_tmp79A->vai=_tmp21F,_tmp79A)))))));
# 1111
*vararg_call_info=_tmp241;
# 1113
if(!_tmp240)
# 1115
for(0;_tmp20B != 0;(_tmp20B=_tmp20B->tl,_tmp20C ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp20B->hd;
++ _tmp241->num_varargs;
Cyc_Tcexp_tcExp(_tmp20E,& _tmp23F,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20E,e1,_tmp23F,& alias_coercion)){
{const char*_tmp79F;void*_tmp79E[2];struct Cyc_String_pa_PrintArg_struct _tmp79D;struct Cyc_String_pa_PrintArg_struct _tmp79C;(_tmp79C.tag=0,((_tmp79C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp79D.tag=0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp79E[0]=& _tmp79D,((_tmp79E[1]=& _tmp79C,Cyc_Tcutil_terr(loc,((_tmp79F="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp79F,sizeof(char),49))),_tag_dyneither(_tmp79E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1125
if(alias_coercion){
struct Cyc_List_List*_tmp7A0;*alias_arg_exps=((_tmp7A0=_cycalloc(sizeof(*_tmp7A0)),((_tmp7A0->hd=(void*)_tmp20C,((_tmp7A0->tl=*alias_arg_exps,_tmp7A0))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23F) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7A3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7A3,sizeof(char),49))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}}else{
# 1134
void*_tmp249=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23F));struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_List_List*_tmp24C;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 3)goto _LL145;else{if((((_tmp24A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp24B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp24A->f1).datatype_info).KnownDatatype).val);_tmp24C=(_tmp24A->f1).targs;}}_LL144: {
# 1138
struct Cyc_Absyn_Datatypedecl*_tmp24D=*Cyc_Tcenv_lookup_datatypedecl(_tmp20E,loc,_tmp24B->name);
struct Cyc_List_List*fields=0;
if(_tmp24D->fields == 0){
const char*_tmp7A7;void*_tmp7A6[1];struct Cyc_String_pa_PrintArg_struct _tmp7A5;(_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7A6[0]=& _tmp7A5,Cyc_Tcutil_terr(loc,((_tmp7A7="can't inject into abstract datatype %s",_tag_dyneither(_tmp7A7,sizeof(char),39))),_tag_dyneither(_tmp7A6,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24D->fields))->v;}
# 1150
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23F),Cyc_Tcenv_curr_rgn(_tmp20E))){
const char*_tmp7AC;void*_tmp7AB[2];struct Cyc_String_pa_PrintArg_struct _tmp7AA;struct Cyc_String_pa_PrintArg_struct _tmp7A9;(_tmp7A9.tag=0,((_tmp7A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20E))),((_tmp7AA.tag=0,((_tmp7AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7AB[0]=& _tmp7AA,((_tmp7AB[1]=& _tmp7A9,Cyc_Tcutil_terr(loc,((_tmp7AC="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7AC,sizeof(char),49))),_tag_dyneither(_tmp7AB,sizeof(void*),2)))))))))))));}{
# 1154
struct _RegionHandle _tmp255=_new_region("rgn");struct _RegionHandle*rgn=& _tmp255;_push_region(rgn);{
struct Cyc_List_List*_tmp256=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp24D->tvs,_tmp24C);
for(0;_tmp20B != 0;_tmp20B=_tmp20B->tl){
++ _tmp241->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp20B->hd;
# 1160
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp20E,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7AF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7AF,sizeof(char),49))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}}{
# 1166
struct Cyc_Absyn_Datatypefield*_tmp259=Cyc_Tcexp_tcInjection(_tmp20E,e1,Cyc_Tcutil_pointer_elt_type(_tmp23F),rgn,_tmp256,fields);
# 1168
if(_tmp259 != 0){
struct Cyc_List_List*_tmp7B0;_tmp241->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp241->injectors,(
(_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0->hd=_tmp259,((_tmp7B0->tl=0,_tmp7B0)))))));}};};}}
# 1174
_npop_handler(0);goto _LL142;
# 1154
;_pop_region(rgn);};}_LL145:;_LL146:
# 1175
{const char*_tmp7B3;void*_tmp7B2;(_tmp7B2=0,Cyc_Tcutil_terr(loc,((_tmp7B3="bad inject vararg type",_tag_dyneither(_tmp7B3,sizeof(char),23))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1180
if(*alias_arg_exps == 0){
# 1189 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp20E,loc,(void*)_check_null(_tmp21A));
# 1193
Cyc_Tcenv_check_rgn_partial_order(_tmp20E,loc,_tmp220);}{
# 1195
void*_tmp25E=_tmp21C;_npop_handler(0);return _tmp25E;};};_LL133:;_LL134: {
const char*_tmp7B6;void*_tmp7B5;void*_tmp261=(_tmp7B5=0,Cyc_Tcexp_expr_err(_tmp20E,loc,topt,((_tmp7B6="expected pointer to function",_tag_dyneither(_tmp7B6,sizeof(char),29))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));_npop_handler(0);return _tmp261;}_LL130:;};_LL12E:;_LL12F: {
# 1198
const char*_tmp7B9;void*_tmp7B8;void*_tmp264=(_tmp7B8=0,Cyc_Tcexp_expr_err(_tmp20E,loc,topt,((_tmp7B9="expected pointer to function",_tag_dyneither(_tmp7B9,sizeof(char),29))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));_npop_handler(0);return _tmp264;}_LL12B:;};}
# 1015 "tcexp.cyc"
;_pop_region(ter);}
# 1204 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp265=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp265,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp265,& bogus)){
const char*_tmp7BE;void*_tmp7BD[2];struct Cyc_String_pa_PrintArg_struct _tmp7BC;struct Cyc_String_pa_PrintArg_struct _tmp7BB;(_tmp7BB.tag=0,((_tmp7BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1209
Cyc_Absynpp_typ2string(_tmp265)),((_tmp7BD[0]=& _tmp7BC,((_tmp7BD[1]=& _tmp7BB,Cyc_Tcutil_terr(loc,((_tmp7BE="expected %s but found %s",_tag_dyneither(_tmp7BE,sizeof(char),25))),_tag_dyneither(_tmp7BD,sizeof(void*),2)))))))))))));}
# 1211
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1216
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1218
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1220
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp26A=t1;void*_tmp26C;struct Cyc_Absyn_Tqual _tmp26D;void*_tmp26E;union Cyc_Absyn_Constraint*_tmp26F;union Cyc_Absyn_Constraint*_tmp270;union Cyc_Absyn_Constraint*_tmp271;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26A;if(_tmp26B->tag != 5)goto _LL14A;else{_tmp26C=(_tmp26B->f1).elt_typ;_tmp26D=(_tmp26B->f1).elt_tq;_tmp26E=((_tmp26B->f1).ptr_atts).rgn;_tmp26F=((_tmp26B->f1).ptr_atts).nullable;_tmp270=((_tmp26B->f1).ptr_atts).bounds;_tmp271=((_tmp26B->f1).ptr_atts).zero_term;}}_LL149:
# 1224
{void*_tmp272=Cyc_Tcutil_compress(_tmp26C);struct Cyc_List_List*_tmp274;void*_tmp275;struct Cyc_Absyn_Tqual _tmp276;void*_tmp277;struct Cyc_List_List*_tmp278;int _tmp279;struct Cyc_Absyn_VarargInfo*_tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27C;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp272;if(_tmp273->tag != 9)goto _LL14F;else{_tmp274=(_tmp273->f1).tvars;_tmp275=(_tmp273->f1).effect;_tmp276=(_tmp273->f1).ret_tqual;_tmp277=(_tmp273->f1).ret_typ;_tmp278=(_tmp273->f1).args;_tmp279=(_tmp273->f1).c_varargs;_tmp27A=(_tmp273->f1).cyc_varargs;_tmp27B=(_tmp273->f1).rgn_po;_tmp27C=(_tmp273->f1).attributes;}}_LL14E: {
# 1226
struct _RegionHandle _tmp27D=_new_region("temp");struct _RegionHandle*temp=& _tmp27D;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1229
for(0;ts != 0  && _tmp274 != 0;(ts=ts->tl,_tmp274=_tmp274->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp274->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp7C1;struct Cyc_List_List*_tmp7C0;instantiation=(
(_tmp7C0=_region_malloc(temp,sizeof(*_tmp7C0)),((_tmp7C0->hd=((_tmp7C1=_region_malloc(temp,sizeof(*_tmp7C1)),((_tmp7C1->f1=(struct Cyc_Absyn_Tvar*)_tmp274->hd,((_tmp7C1->f2=(void*)ts->hd,_tmp7C1)))))),((_tmp7C0->tl=instantiation,_tmp7C0))))));};}
# 1236
if(ts != 0){
const char*_tmp7C4;void*_tmp7C3;void*_tmp282=
(_tmp7C3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C4="too many type variables in instantiation",_tag_dyneither(_tmp7C4,sizeof(char),41))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));_npop_handler(0);return _tmp282;}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7CA;struct Cyc_Absyn_FnInfo _tmp7C9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7C8;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7CA.tag=9,((_tmp7CA.f1=((_tmp7C9.tvars=_tmp274,((_tmp7C9.effect=_tmp275,((_tmp7C9.ret_tqual=_tmp276,((_tmp7C9.ret_typ=_tmp277,((_tmp7C9.args=_tmp278,((_tmp7C9.c_varargs=_tmp279,((_tmp7C9.cyc_varargs=_tmp27A,((_tmp7C9.rgn_po=_tmp27B,((_tmp7C9.attributes=_tmp27C,_tmp7C9)))))))))))))))))),_tmp7CA)))),_tmp7C8)))));
# 1243
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7D4;struct Cyc_Absyn_PtrAtts _tmp7D3;struct Cyc_Absyn_PtrInfo _tmp7D2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D1;void*_tmp287=(void*)((_tmp7D1=_cycalloc(sizeof(*_tmp7D1)),((_tmp7D1[0]=((_tmp7D4.tag=5,((_tmp7D4.f1=((_tmp7D2.elt_typ=new_fn_typ,((_tmp7D2.elt_tq=_tmp26D,((_tmp7D2.ptr_atts=((_tmp7D3.rgn=_tmp26E,((_tmp7D3.nullable=_tmp26F,((_tmp7D3.bounds=_tmp270,((_tmp7D3.zero_term=_tmp271,((_tmp7D3.ptrloc=0,_tmp7D3)))))))))),_tmp7D2)))))),_tmp7D4)))),_tmp7D1))));_npop_handler(0);return _tmp287;};}
# 1227
;_pop_region(temp);}_LL14F:;_LL150:
# 1245
 goto _LL14C;_LL14C:;}
# 1247
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1250
const char*_tmp7D8;void*_tmp7D7[1];struct Cyc_String_pa_PrintArg_struct _tmp7D6;return(_tmp7D6.tag=0,((_tmp7D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7D7[0]=& _tmp7D6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D8="expecting polymorphic type but found %s",_tag_dyneither(_tmp7D8,sizeof(char),40))),_tag_dyneither(_tmp7D7,sizeof(void*),1)))))));};};}
# 1255
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1257
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1260
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1265
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1272
Cyc_Tcutil_unify(t2,t);{
const char*_tmp7DD;void*_tmp7DC[2];struct Cyc_String_pa_PrintArg_struct _tmp7DB;struct Cyc_String_pa_PrintArg_struct _tmp7DA;void*_tmp28E=(_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7DB.tag=0,((_tmp7DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7DC[0]=& _tmp7DB,((_tmp7DC[1]=& _tmp7DA,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp7DD="cannot cast %s to %s",_tag_dyneither(_tmp7DD,sizeof(char),21))),_tag_dyneither(_tmp7DC,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp28E;};}}}
# 1282
{struct _tuple0 _tmp7DE;struct _tuple0 _tmp294=(_tmp7DE.f1=e->r,((_tmp7DE.f2=Cyc_Tcutil_compress(t),_tmp7DE)));int _tmp296;union Cyc_Absyn_Constraint*_tmp298;union Cyc_Absyn_Constraint*_tmp299;union Cyc_Absyn_Constraint*_tmp29A;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp294.f1;if(_tmp295->tag != 32)goto _LL154;else{_tmp296=(_tmp295->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp297=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp294.f2;if(_tmp297->tag != 5)goto _LL154;else{_tmp298=((_tmp297->f1).ptr_atts).nullable;_tmp299=((_tmp297->f1).ptr_atts).bounds;_tmp29A=((_tmp297->f1).ptr_atts).zero_term;}};_LL153:
# 1286
 if((_tmp296  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp29A)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp298)){
void*_tmp29B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp299);struct Cyc_Absyn_Exp*_tmp29D;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B;if(_tmp29C->tag != 1)goto _LL159;else{_tmp29D=_tmp29C->f1;}}_LL158:
# 1289
 if((Cyc_Evexp_eval_const_uint_exp(_tmp29D)).f1 == 1){
const char*_tmp7E1;void*_tmp7E0;(_tmp7E0=0,Cyc_Tcutil_warn(loc,((_tmp7E1="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp7E1,sizeof(char),60))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1295
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1298
return t;};}
# 1302
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1304
void**_tmp2A0=0;
struct Cyc_Absyn_Tqual _tmp2A1=Cyc_Absyn_empty_tqual(0);
int _tmp2A2=0;
if(topt != 0){
void*_tmp2A3=Cyc_Tcutil_compress(*topt);void**_tmp2A5;struct Cyc_Absyn_Tqual _tmp2A6;union Cyc_Absyn_Constraint*_tmp2A7;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A3;if(_tmp2A4->tag != 5)goto _LL15E;else{_tmp2A5=(void**)&(_tmp2A4->f1).elt_typ;_tmp2A6=(_tmp2A4->f1).elt_tq;_tmp2A7=((_tmp2A4->f1).ptr_atts).nullable;}}_LL15D:
# 1310
 _tmp2A0=_tmp2A5;
_tmp2A1=_tmp2A6;
_tmp2A2=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2A7);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1324
struct _RegionHandle _tmp2A8=_new_region("r");struct _RegionHandle*r=& _tmp2A8;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp2A0,e);
# 1327
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp7E4;void*_tmp7E3;(_tmp7E3=0,Cyc_Tcutil_terr(e->loc,((_tmp7E4="Cannot take the address of an alias-free path",_tag_dyneither(_tmp7E4,sizeof(char),46))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}
# 1334
{void*_tmp2AB=e->r;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2AC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2AB;if(_tmp2AC->tag != 22)goto _LL163;else{_tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;}}_LL162:
# 1336
{void*_tmp2AF=Cyc_Tcutil_compress((void*)_check_null(_tmp2AD->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2B0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AF;if(_tmp2B0->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1344
 e0->r=(Cyc_Absyn_add_exp(_tmp2AD,_tmp2AE,0))->r;{
void*_tmp2B1=Cyc_Tcexp_tcPlus(te,_tmp2AD,_tmp2AE);_npop_handler(0);return _tmp2B1;};_LL165:;}
# 1347
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1352
{void*_tmp2B2=e->r;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B2;if(_tmp2B3->tag != 20)goto _LL16D;else{if(_tmp2B3->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B2;if(_tmp2B4->tag != 21)goto _LL16F;else{if(_tmp2B4->f3 != 1)goto _LL16F;}}_LL16E:
# 1355
{const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=0,Cyc_Tcutil_terr(e->loc,((_tmp7E7="cannot take the address of a @tagged union member",_tag_dyneither(_tmp7E7,sizeof(char),50))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1361
int _tmp2B8;void*_tmp2B9;struct _tuple14 _tmp2B7=Cyc_Tcutil_addressof_props(te,e);_tmp2B8=_tmp2B7.f1;_tmp2B9=_tmp2B7.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B9,0)){
const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,Cyc_Tcutil_terr(e->loc,((_tmp7EA="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp7EA,sizeof(char),60))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}{
# 1365
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2B8){
tq.print_const=1;
tq.real_const=1;}{
# 1371
void*t=_tmp2A2?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2B9,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2B9,tq,Cyc_Absyn_false_conref);
void*_tmp2BC=t;_npop_handler(0);return _tmp2BC;};};};
# 1324
;_pop_region(r);};}
# 1378
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1382
return Cyc_Absyn_uint_typ;
# 1384
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp7EE;void*_tmp7ED[1];struct Cyc_String_pa_PrintArg_struct _tmp7EC;(_tmp7EC.tag=0,((_tmp7EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7ED[0]=& _tmp7EC,Cyc_Tcutil_terr(loc,((_tmp7EE="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp7EE,sizeof(char),55))),_tag_dyneither(_tmp7ED,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2C0=Cyc_Tcutil_compress(*topt);void*_tmp2C1;void*_tmp2C3;_LL172: _tmp2C1=_tmp2C0;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2C1;if(_tmp2C2->tag != 19)goto _LL174;else{_tmp2C3=(void*)_tmp2C2->f1;}};_LL173: {
# 1393
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp7F1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F0;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C5=(_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0[0]=((_tmp7F1.tag=18,((_tmp7F1.f1=_tmp2C4,_tmp7F1)))),_tmp7F0)));
if(Cyc_Tcutil_unify(_tmp2C3,(void*)_tmp2C5))return _tmp2C1;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1399
return Cyc_Absyn_uint_typ;}
# 1402
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}
# 1409
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,void*n){
# 1411
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
{void*_tmp2C8=n;struct _dyneither_ptr*_tmp2CA;unsigned int _tmp2CC;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2C9=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2C8;if(_tmp2C9->tag != 0)goto _LL179;else{_tmp2CA=_tmp2C9->f1;}}_LL178: {
# 1415
int bad_type=1;
{void*_tmp2CD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl**_tmp2CF;struct Cyc_List_List*_tmp2D1;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2CE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CD;if(_tmp2CE->tag != 11)goto _LL17E;else{if((((_tmp2CE->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2CF=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2CE->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1418
 if((*_tmp2CF)->impl == 0)
goto _LL17B;
if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2CA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2CF)->impl))->fields)){
const char*_tmp7F5;void*_tmp7F4[1];struct Cyc_String_pa_PrintArg_struct _tmp7F3;(_tmp7F3.tag=0,((_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CA),((_tmp7F4[0]=& _tmp7F3,Cyc_Tcutil_terr(loc,((_tmp7F5="no field of struct/union has name %s",_tag_dyneither(_tmp7F5,sizeof(char),37))),_tag_dyneither(_tmp7F4,sizeof(void*),1)))))));}
bad_type=0;
goto _LL17B;_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2D0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CD;if(_tmp2D0->tag != 12)goto _LL180;else{_tmp2D1=_tmp2D0->f2;}}_LL17F:
# 1425
 if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2CA,_tmp2D1)){
const char*_tmp7F9;void*_tmp7F8[1];struct Cyc_String_pa_PrintArg_struct _tmp7F7;(_tmp7F7.tag=0,((_tmp7F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CA),((_tmp7F8[0]=& _tmp7F7,Cyc_Tcutil_terr(loc,((_tmp7F9="no field of struct/union has name %s",_tag_dyneither(_tmp7F9,sizeof(char),37))),_tag_dyneither(_tmp7F8,sizeof(void*),1)))))));}
bad_type=0;
goto _LL17B;_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1431
if(bad_type){
const char*_tmp7FD;void*_tmp7FC[1];struct Cyc_String_pa_PrintArg_struct _tmp7FB;(_tmp7FB.tag=0,((_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7FC[0]=& _tmp7FB,Cyc_Tcutil_terr(loc,((_tmp7FD="%s is not a known struct/union type",_tag_dyneither(_tmp7FD,sizeof(char),36))),_tag_dyneither(_tmp7FC,sizeof(void*),1)))))));}
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2CB=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2C8;if(_tmp2CB->tag != 1)goto _LL176;else{_tmp2CC=_tmp2CB->f1;}}_LL17A: {
# 1435
int bad_type=1;
{void*_tmp2DB=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl**_tmp2DD;struct Cyc_List_List*_tmp2DF;struct Cyc_List_List*_tmp2E1;struct Cyc_Absyn_Datatypefield*_tmp2E3;_LL183: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DC->tag != 11)goto _LL185;else{if((((_tmp2DC->f1).aggr_info).KnownAggr).tag != 2)goto _LL185;_tmp2DD=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2DC->f1).aggr_info).KnownAggr).val;}}_LL184:
# 1438
 if((*_tmp2DD)->impl == 0)
goto _LL182;
_tmp2DF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2DD)->impl))->fields;goto _LL186;_LL185: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DB;if(_tmp2DE->tag != 12)goto _LL187;else{_tmp2DF=_tmp2DE->f2;}}_LL186:
# 1442
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2DF)<= _tmp2CC){
const char*_tmp802;void*_tmp801[2];struct Cyc_Int_pa_PrintArg_struct _tmp800;struct Cyc_Int_pa_PrintArg_struct _tmp7FF;(_tmp7FF.tag=1,((_tmp7FF.f1=(unsigned long)((int)_tmp2CC),((_tmp800.tag=1,((_tmp800.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2DF),((_tmp801[0]=& _tmp800,((_tmp801[1]=& _tmp7FF,Cyc_Tcutil_terr(loc,((_tmp802="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp802,sizeof(char),46))),_tag_dyneither(_tmp801,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL187: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E0->tag != 10)goto _LL189;else{_tmp2E1=_tmp2E0->f1;}}_LL188:
# 1448
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1)<= _tmp2CC){
const char*_tmp807;void*_tmp806[2];struct Cyc_Int_pa_PrintArg_struct _tmp805;struct Cyc_Int_pa_PrintArg_struct _tmp804;(_tmp804.tag=1,((_tmp804.f1=(unsigned long)((int)_tmp2CC),((_tmp805.tag=1,((_tmp805.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E1),((_tmp806[0]=& _tmp805,((_tmp806[1]=& _tmp804,Cyc_Tcutil_terr(loc,((_tmp807="tuple has too few components: %d <= %d",_tag_dyneither(_tmp807,sizeof(char),39))),_tag_dyneither(_tmp806,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL189: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2DB;if(_tmp2E2->tag != 4)goto _LL18B;else{if((((_tmp2E2->f1).field_info).KnownDatatypefield).tag != 2)goto _LL18B;_tmp2E3=((struct _tuple2)(((_tmp2E2->f1).field_info).KnownDatatypefield).val).f2;}}_LL18A:
# 1454
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E3->typs)< _tmp2CC){
const char*_tmp80C;void*_tmp80B[2];struct Cyc_Int_pa_PrintArg_struct _tmp80A;struct Cyc_Int_pa_PrintArg_struct _tmp809;(_tmp809.tag=1,((_tmp809.f1=(unsigned long)((int)_tmp2CC),((_tmp80A.tag=1,((_tmp80A.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2E3->typs),((_tmp80B[0]=& _tmp80A,((_tmp80B[1]=& _tmp809,Cyc_Tcutil_terr(loc,((_tmp80C="datatype field has too few components: %d < %d",_tag_dyneither(_tmp80C,sizeof(char),47))),_tag_dyneither(_tmp80B,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL18B:;_LL18C:
 goto _LL182;_LL182:;}
# 1461
if(bad_type){
const char*_tmp810;void*_tmp80F[1];struct Cyc_String_pa_PrintArg_struct _tmp80E;(_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp80F[0]=& _tmp80E,Cyc_Tcutil_terr(loc,((_tmp810="%s is not a known type",_tag_dyneither(_tmp810,sizeof(char),23))),_tag_dyneither(_tmp80F,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1465
return Cyc_Absyn_uint_typ;}
# 1469
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp2F3=_new_region("r");struct _RegionHandle*r=& _tmp2F3;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp2F4=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp2F4,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp2F5=t;void*_tmp2F8;void*_tmp2F9;union Cyc_Absyn_Constraint*_tmp2FA;union Cyc_Absyn_Constraint*_tmp2FB;_LL18E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F5;if(_tmp2F6->tag != 1)goto _LL190;}_LL18F: {
# 1476
struct Cyc_List_List*_tmp2FC=Cyc_Tcenv_lookup_type_vars(_tmp2F4);
struct Cyc_Core_Opt*_tmp811;void*_tmp2FD=Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811->v=_tmp2FC,_tmp811)))));
struct Cyc_Core_Opt*_tmp812;void*_tmp2FE=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp812=_cycalloc(sizeof(*_tmp812)),((_tmp812->v=_tmp2FC,_tmp812)))));
union Cyc_Absyn_Constraint*_tmp2FF=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp300=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp813;struct Cyc_Absyn_PtrAtts _tmp301=(_tmp813.rgn=_tmp2FE,((_tmp813.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp813.bounds=_tmp2FF,((_tmp813.zero_term=_tmp300,((_tmp813.ptrloc=0,_tmp813)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp819;struct Cyc_Absyn_PtrInfo _tmp818;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp817;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp302=(_tmp817=_cycalloc(sizeof(*_tmp817)),((_tmp817[0]=((_tmp819.tag=5,((_tmp819.f1=((_tmp818.elt_typ=_tmp2FD,((_tmp818.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp818.ptr_atts=_tmp301,_tmp818)))))),_tmp819)))),_tmp817)));
Cyc_Tcutil_unify(t,(void*)_tmp302);
_tmp2F8=_tmp2FD;_tmp2F9=_tmp2FE;_tmp2FA=_tmp2FF;_tmp2FB=_tmp300;goto _LL191;}_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F5;if(_tmp2F7->tag != 5)goto _LL192;else{_tmp2F8=(_tmp2F7->f1).elt_typ;_tmp2F9=((_tmp2F7->f1).ptr_atts).rgn;_tmp2FA=((_tmp2F7->f1).ptr_atts).bounds;_tmp2FB=((_tmp2F7->f1).ptr_atts).zero_term;}}_LL191:
# 1486
 Cyc_Tcenv_check_rgn_accessible(_tmp2F4,loc,_tmp2F9);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2FA);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2F8),& Cyc_Tcutil_tmk)){
const char*_tmp81C;void*_tmp81B;(_tmp81B=0,Cyc_Tcutil_terr(loc,((_tmp81C="can't dereference abstract pointer type",_tag_dyneither(_tmp81C,sizeof(char),40))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}{
void*_tmp30B=_tmp2F8;_npop_handler(0);return _tmp30B;};_LL192:;_LL193: {
# 1492
const char*_tmp820;void*_tmp81F[1];struct Cyc_String_pa_PrintArg_struct _tmp81E;void*_tmp30F=(_tmp81E.tag=0,((_tmp81E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp81F[0]=& _tmp81E,Cyc_Tcexp_expr_err(_tmp2F4,loc,topt,((_tmp820="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp820,sizeof(char),39))),_tag_dyneither(_tmp81F,sizeof(void*),1)))))));_npop_handler(0);return _tmp30F;}_LL18D:;};}
# 1471
;_pop_region(r);}
# 1498
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1502
struct _RegionHandle _tmp310=_new_region("r");struct _RegionHandle*r=& _tmp310;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1506
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp311=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Aggrdecl*_tmp313;struct Cyc_List_List*_tmp314;enum Cyc_Absyn_AggrKind _tmp316;struct Cyc_List_List*_tmp317;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp312=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp311;if(_tmp312->tag != 11)goto _LL197;else{if((((_tmp312->f1).aggr_info).KnownAggr).tag != 2)goto _LL197;_tmp313=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp312->f1).aggr_info).KnownAggr).val);_tmp314=(_tmp312->f1).targs;}}_LL196: {
# 1509
struct Cyc_Absyn_Aggrfield*_tmp31C=Cyc_Absyn_lookup_decl_field(_tmp313,f);
if(_tmp31C == 0){
const char*_tmp825;void*_tmp824[2];struct Cyc_String_pa_PrintArg_struct _tmp823;struct Cyc_String_pa_PrintArg_struct _tmp822;void*_tmp321=(_tmp822.tag=0,((_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp313->name)),((_tmp824[0]=& _tmp823,((_tmp824[1]=& _tmp822,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp825="type %s has no %s field",_tag_dyneither(_tmp825,sizeof(char),24))),_tag_dyneither(_tmp824,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp321;}
# 1514
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp313->impl))->tagged)*is_tagged=1;{
void*t2=_tmp31C->type;
if(_tmp314 != 0){
struct _RegionHandle _tmp322=_new_region("rgn");struct _RegionHandle*rgn=& _tmp322;_push_region(rgn);
{struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp313->tvs,_tmp314);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp323,_tmp31C->type);}
# 1518
;_pop_region(rgn);}
# 1523
if(((_tmp313->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp313->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
const char*_tmp829;void*_tmp828[1];struct Cyc_String_pa_PrintArg_struct _tmp827;void*_tmp327=(_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp828[0]=& _tmp827,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp829="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp829,sizeof(char),56))),_tag_dyneither(_tmp828,sizeof(void*),1)))))));_npop_handler(0);return _tmp327;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp313->impl))->exist_vars != 0){
# 1528
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp82D;void*_tmp82C[1];struct Cyc_String_pa_PrintArg_struct _tmp82B;void*_tmp32B=(_tmp82B.tag=0,((_tmp82B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp82C[0]=& _tmp82B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp82D="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp82D,sizeof(char),81))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))));_npop_handler(0);return _tmp32B;}}{
# 1531
void*_tmp32C=t2;_npop_handler(0);return _tmp32C;};};}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp311;if(_tmp315->tag != 12)goto _LL199;else{_tmp316=_tmp315->f1;_tmp317=_tmp315->f2;}}_LL198: {
# 1533
struct Cyc_Absyn_Aggrfield*_tmp32D=Cyc_Absyn_lookup_field(_tmp317,f);
if(_tmp32D == 0){
const char*_tmp831;void*_tmp830[1];struct Cyc_String_pa_PrintArg_struct _tmp82F;void*_tmp331=(_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp830[0]=& _tmp82F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp831="type has no %s field",_tag_dyneither(_tmp831,sizeof(char),21))),_tag_dyneither(_tmp830,sizeof(void*),1)))))));_npop_handler(0);return _tmp331;}
if((_tmp316 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp32D->type)){
const char*_tmp835;void*_tmp834[1];struct Cyc_String_pa_PrintArg_struct _tmp833;void*_tmp335=(_tmp833.tag=0,((_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp834[0]=& _tmp833,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp835="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp835,sizeof(char),56))),_tag_dyneither(_tmp834,sizeof(void*),1)))))));_npop_handler(0);return _tmp335;}{
void*_tmp336=_tmp32D->type;_npop_handler(0);return _tmp336;};}_LL199:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp311;if(_tmp318->tag != 8)goto _LL19B;}{const char*_tmp836;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp836="size",_tag_dyneither(_tmp836,sizeof(char),5))))== 0))goto _LL19B;};_LL19A: goto _LL19C;_LL19B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp311;if(_tmp31A->tag != 5)goto _LL19D;}{const char*_tmp837;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp837="size",_tag_dyneither(_tmp837,sizeof(char),5))))== 0))goto _LL19D;};_LL19C:
{const char*_tmp83B;void*_tmp83A[1];struct Cyc_String_pa_PrintArg_struct _tmp839;(_tmp839.tag=0,((_tmp839.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp83A[0]=& _tmp839,Cyc_Tcutil_warn(e->loc,((_tmp83B="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp83B,sizeof(char),54))),_tag_dyneither(_tmp83A,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp841;struct Cyc_List_List*_tmp840;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp83F;outer_e->r=(void*)((_tmp83F=_cycalloc(sizeof(*_tmp83F)),((_tmp83F[0]=((_tmp841.tag=2,((_tmp841.f1=Cyc_Absyn_Numelts,((_tmp841.f2=((_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840->hd=e,((_tmp840->tl=0,_tmp840)))))),_tmp841)))))),_tmp83F))));}{
void*_tmp33D=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp33D;};_LL19D:;_LL19E: {
# 1545
const char*_tmp842;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp842="size",_tag_dyneither(_tmp842,sizeof(char),5))))== 0){
const char*_tmp846;void*_tmp845[1];struct Cyc_String_pa_PrintArg_struct _tmp844;void*_tmp342=(_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1548
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp845[0]=& _tmp844,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp846="expecting struct, union, or array, found %s",_tag_dyneither(_tmp846,sizeof(char),44))),_tag_dyneither(_tmp845,sizeof(void*),1)))))));_npop_handler(0);return _tmp342;}else{
# 1550
const char*_tmp84A;void*_tmp849[1];struct Cyc_String_pa_PrintArg_struct _tmp848;void*_tmp346=(_tmp848.tag=0,((_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp849[0]=& _tmp848,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84A="expecting struct or union, found %s",_tag_dyneither(_tmp84A,sizeof(char),36))),_tag_dyneither(_tmp849,sizeof(void*),1)))))));_npop_handler(0);return _tmp346;}}_LL194:;};
# 1502
;_pop_region(r);}
# 1556
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1559
struct _RegionHandle _tmp347=_new_region("r");struct _RegionHandle*r=& _tmp347;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1563
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp348=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp34A;void*_tmp34B;union Cyc_Absyn_Constraint*_tmp34C;union Cyc_Absyn_Constraint*_tmp34D;_LL1A0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp348;if(_tmp349->tag != 5)goto _LL1A2;else{_tmp34A=(_tmp349->f1).elt_typ;_tmp34B=((_tmp349->f1).ptr_atts).rgn;_tmp34C=((_tmp349->f1).ptr_atts).bounds;_tmp34D=((_tmp349->f1).ptr_atts).zero_term;}}_LL1A1:
# 1566
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp34C);
{void*_tmp34E=Cyc_Tcutil_compress(_tmp34A);struct Cyc_Absyn_Aggrdecl*_tmp350;struct Cyc_List_List*_tmp351;enum Cyc_Absyn_AggrKind _tmp353;struct Cyc_List_List*_tmp354;_LL1A5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp34E;if(_tmp34F->tag != 11)goto _LL1A7;else{if((((_tmp34F->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A7;_tmp350=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp34F->f1).aggr_info).KnownAggr).val);_tmp351=(_tmp34F->f1).targs;}}_LL1A6: {
# 1569
struct Cyc_Absyn_Aggrfield*_tmp355=Cyc_Absyn_lookup_decl_field(_tmp350,f);
if(_tmp355 == 0){
const char*_tmp84F;void*_tmp84E[2];struct Cyc_String_pa_PrintArg_struct _tmp84D;struct Cyc_String_pa_PrintArg_struct _tmp84C;void*_tmp35A=(_tmp84C.tag=0,((_tmp84C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp84D.tag=0,((_tmp84D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp350->name)),((_tmp84E[0]=& _tmp84D,((_tmp84E[1]=& _tmp84C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84F="type %s has no %s field",_tag_dyneither(_tmp84F,sizeof(char),24))),_tag_dyneither(_tmp84E,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp35A;}
# 1574
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp350->impl))->tagged)*is_tagged=1;{
void*t3=_tmp355->type;
if(_tmp351 != 0){
struct _RegionHandle _tmp35B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp35B;_push_region(rgn);
{struct Cyc_List_List*_tmp35C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp350->tvs,_tmp351);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp35C,_tmp355->type);}
# 1578
;_pop_region(rgn);}{
# 1582
struct Cyc_Absyn_Kind*_tmp35D=Cyc_Tcutil_typ_kind(t3);
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp35D)){
# 1585
void*_tmp35E=Cyc_Tcutil_compress(t3);_LL1AC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E;if(_tmp35F->tag != 8)goto _LL1AE;}_LL1AD:
 goto _LL1AB;_LL1AE:;_LL1AF: {
# 1588
const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_PrintArg_struct _tmp851;void*_tmp363=(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp852[0]=& _tmp851,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp853="cannot get member %s since its type is abstract",_tag_dyneither(_tmp853,sizeof(char),48))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));_npop_handler(0);return _tmp363;}_LL1AB:;}
# 1591
if(((_tmp350->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp350->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
const char*_tmp857;void*_tmp856[1];struct Cyc_String_pa_PrintArg_struct _tmp855;void*_tmp367=(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp856[0]=& _tmp855,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp857="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp857,sizeof(char),56))),_tag_dyneither(_tmp856,sizeof(void*),1)))))));_npop_handler(0);return _tmp367;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp350->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp85B;void*_tmp85A[1];struct Cyc_String_pa_PrintArg_struct _tmp859;void*_tmp36B=(_tmp859.tag=0,((_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp85A[0]=& _tmp859,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp85B="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp85B,sizeof(char),72))),_tag_dyneither(_tmp85A,sizeof(void*),1)))))));_npop_handler(0);return _tmp36B;}}{
# 1599
void*_tmp36C=t3;_npop_handler(0);return _tmp36C;};};};}_LL1A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34E;if(_tmp352->tag != 12)goto _LL1A9;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;}}_LL1A8: {
# 1601
struct Cyc_Absyn_Aggrfield*_tmp36D=Cyc_Absyn_lookup_field(_tmp354,f);
if(_tmp36D == 0){
const char*_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_PrintArg_struct _tmp85D;void*_tmp371=(_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp85E[0]=& _tmp85D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp85F="type has no %s field",_tag_dyneither(_tmp85F,sizeof(char),21))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));_npop_handler(0);return _tmp371;}
if((_tmp353 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp36D->type)){
const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_PrintArg_struct _tmp861;void*_tmp375=(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp862[0]=& _tmp861,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp863="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp863,sizeof(char),56))),_tag_dyneither(_tmp862,sizeof(void*),1)))))));_npop_handler(0);return _tmp375;}{
void*_tmp376=_tmp36D->type;_npop_handler(0);return _tmp376;};}_LL1A9:;_LL1AA:
 goto _LL1A4;_LL1A4:;}
# 1609
goto _LL19F;_LL1A2:;_LL1A3:
 goto _LL19F;_LL19F:;}{
# 1612
const char*_tmp867;void*_tmp866[1];struct Cyc_String_pa_PrintArg_struct _tmp865;void*_tmp37A=(_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp866[0]=& _tmp865,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp867="expecting struct or union pointer, found %s",_tag_dyneither(_tmp867,sizeof(char),44))),_tag_dyneither(_tmp866,sizeof(void*),1)))))));_npop_handler(0);return _tmp37A;};
# 1559
;_pop_region(r);}
# 1617
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1619
unsigned int _tmp37C;int _tmp37D;struct _tuple15 _tmp37B=Cyc_Evexp_eval_const_uint_exp(index);_tmp37C=_tmp37B.f1;_tmp37D=_tmp37B.f2;
if(!_tmp37D){
const char*_tmp86A;void*_tmp869;return(_tmp869=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp86A="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp86A,sizeof(char),47))),_tag_dyneither(_tmp869,sizeof(void*),0)));}{
# 1623
struct _handler_cons _tmp380;_push_handler(& _tmp380);{int _tmp382=0;if(setjmp(_tmp380.handler))_tmp382=1;if(!_tmp382){
{void*_tmp383=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp37C)).f2;_npop_handler(0);return _tmp383;};_pop_handler();}else{void*_tmp381=(void*)_exn_thrown;void*_tmp385=_tmp381;void*_tmp387;_LL1B1: {struct Cyc_List_Nth_exn_struct*_tmp386=(struct Cyc_List_Nth_exn_struct*)_tmp385;if(_tmp386->tag != Cyc_List_Nth)goto _LL1B3;}_LL1B2: {
# 1626
const char*_tmp86F;void*_tmp86E[2];struct Cyc_Int_pa_PrintArg_struct _tmp86D;struct Cyc_Int_pa_PrintArg_struct _tmp86C;return(_tmp86C.tag=1,((_tmp86C.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp86D.tag=1,((_tmp86D.f1=(unsigned long)((int)_tmp37C),((_tmp86E[0]=& _tmp86D,((_tmp86E[1]=& _tmp86C,Cyc_Tcexp_expr_err(te,loc,0,((_tmp86F="index is %d but tuple has only %d fields",_tag_dyneither(_tmp86F,sizeof(char),41))),_tag_dyneither(_tmp86E,sizeof(void*),2)))))))))))));}_LL1B3: _tmp387=_tmp385;_LL1B4:(void)_throw(_tmp387);_LL1B0:;}};};}
# 1631
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1633
struct _RegionHandle _tmp38C=_new_region("r");struct _RegionHandle*r=& _tmp38C;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp38D=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp38D,0,e1);
Cyc_Tcexp_tcExp(_tmp38D,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp38D,e2)){
const char*_tmp873;void*_tmp872[1];struct Cyc_String_pa_PrintArg_struct _tmp871;void*_tmp391=(_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp872[0]=& _tmp871,Cyc_Tcexp_expr_err(_tmp38D,e2->loc,topt,((_tmp873="expecting int subscript, found %s",_tag_dyneither(_tmp873,sizeof(char),34))),_tag_dyneither(_tmp872,sizeof(void*),1)))))));_npop_handler(0);return _tmp391;}{
# 1644
void*_tmp392=t1;void*_tmp394;struct Cyc_Absyn_Tqual _tmp395;void*_tmp396;union Cyc_Absyn_Constraint*_tmp397;union Cyc_Absyn_Constraint*_tmp398;struct Cyc_List_List*_tmp39A;_LL1B6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 5)goto _LL1B8;else{_tmp394=(_tmp393->f1).elt_typ;_tmp395=(_tmp393->f1).elt_tq;_tmp396=((_tmp393->f1).ptr_atts).rgn;_tmp397=((_tmp393->f1).ptr_atts).bounds;_tmp398=((_tmp393->f1).ptr_atts).zero_term;}}_LL1B7:
# 1648
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp398)){
int emit_warning=1;
{void*_tmp39B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp397);struct Cyc_Absyn_Exp*_tmp39D;_LL1BD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp39C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp39B;if(_tmp39C->tag != 1)goto _LL1BF;else{_tmp39D=_tmp39C->f1;}}_LL1BE:
# 1652
 if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3A0;int _tmp3A1;struct _tuple15 _tmp39F=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3A0=_tmp39F.f1;_tmp3A1=_tmp39F.f2;
if(_tmp3A1){
unsigned int _tmp3A3;int _tmp3A4;struct _tuple15 _tmp3A2=Cyc_Evexp_eval_const_uint_exp(_tmp39D);_tmp3A3=_tmp3A2.f1;_tmp3A4=_tmp3A2.f2;
if(_tmp3A4  && _tmp3A3 > _tmp3A0)emit_warning=0;}}
# 1659
goto _LL1BC;_LL1BF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp39E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp39B;if(_tmp39E->tag != 0)goto _LL1BC;}_LL1C0:
 emit_warning=0;goto _LL1BC;_LL1BC:;}
# 1662
if(emit_warning){
const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_warn(e2->loc,((_tmp876="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp876,sizeof(char),63))),_tag_dyneither(_tmp875,sizeof(void*),0)));}}else{
# 1666
if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3A8;int _tmp3A9;struct _tuple15 _tmp3A7=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3A8=_tmp3A7.f1;_tmp3A9=_tmp3A7.f2;
if(_tmp3A9)
Cyc_Tcutil_check_bound(loc,_tmp3A8,_tmp397);}else{
# 1672
if(Cyc_Tcutil_is_bound_one(_tmp397) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp398)){
const char*_tmp879;void*_tmp878;(_tmp878=0,Cyc_Tcutil_warn(e1->loc,((_tmp879="subscript applied to pointer to one object",_tag_dyneither(_tmp879,sizeof(char),43))),_tag_dyneither(_tmp878,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp397);}}
# 1677
Cyc_Tcenv_check_rgn_accessible(_tmp38D,loc,_tmp396);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp394),& Cyc_Tcutil_tmk)){
const char*_tmp87C;void*_tmp87B;(_tmp87B=0,Cyc_Tcutil_terr(e1->loc,((_tmp87C="can't subscript an abstract pointer",_tag_dyneither(_tmp87C,sizeof(char),36))),_tag_dyneither(_tmp87B,sizeof(void*),0)));}{
void*_tmp3AE=_tmp394;_npop_handler(0);return _tmp3AE;};_LL1B8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp392;if(_tmp399->tag != 10)goto _LL1BA;else{_tmp39A=_tmp399->f1;}}_LL1B9: {
void*_tmp3AF=Cyc_Tcexp_ithTupleType(_tmp38D,loc,_tmp39A,e2);_npop_handler(0);return _tmp3AF;}_LL1BA:;_LL1BB: {
const char*_tmp880;void*_tmp87F[1];struct Cyc_String_pa_PrintArg_struct _tmp87E;void*_tmp3B3=(_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp87F[0]=& _tmp87E,Cyc_Tcexp_expr_err(_tmp38D,loc,topt,((_tmp880="subscript applied to %s",_tag_dyneither(_tmp880,sizeof(char),24))),_tag_dyneither(_tmp87F,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B3;}_LL1B5:;};};}
# 1634
;_pop_region(r);}
# 1688
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp3B4=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3B6;_LL1C2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B4;if(_tmp3B5->tag != 10)goto _LL1C4;else{_tmp3B6=_tmp3B5->f1;}}_LL1C3:
# 1694
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3B6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1697
goto _LL1C1;
# 1699
for(0;es != 0;(es=es->tl,_tmp3B6=_tmp3B6->tl)){
int bogus=0;
void*_tmp3B7=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3B6))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp3B7,(struct Cyc_Absyn_Exp*)es->hd);
# 1704
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3B6->hd)).f2,& bogus);{
struct _tuple19*_tmp883;struct Cyc_List_List*_tmp882;fields=((_tmp882=_cycalloc(sizeof(*_tmp882)),((_tmp882->hd=((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883->f1=(*((struct _tuple19*)_tmp3B6->hd)).f1,((_tmp883->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp883)))))),((_tmp882->tl=fields,_tmp882))))));};}
# 1707
done=1;
goto _LL1C1;_LL1C4:;_LL1C5:
 goto _LL1C1;_LL1C1:;}
# 1711
if(!done)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple19*_tmp886;struct Cyc_List_List*_tmp885;fields=((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885->hd=((_tmp886=_cycalloc(sizeof(*_tmp886)),((_tmp886->f1=Cyc_Absyn_empty_tqual(0),((_tmp886->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp886)))))),((_tmp885->tl=fields,_tmp885))))));};}{
# 1716
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp889;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp888;return(void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((_tmp889.tag=10,((_tmp889.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp889)))),_tmp888))));};}
# 1720
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1723
const char*_tmp88C;void*_tmp88B;return(_tmp88B=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88C="tcCompoundLit",_tag_dyneither(_tmp88C,sizeof(char),14))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}
# 1733 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1736
void*res_t2;
struct _RegionHandle _tmp3C0=_new_region("r");struct _RegionHandle*r=& _tmp3C0;_push_region(r);{
int _tmp3C1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp88F;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp88E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3C2=(_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp88F.tag=0,((_tmp88F.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3C1),_tmp88F)))),_tmp88E)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3C2,loc);
# 1745
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3C3=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3C1 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3C3)){
const char*_tmp892;void*_tmp891;(_tmp891=0,Cyc_Tcutil_terr(_tmp3C3->loc,((_tmp892="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp892,sizeof(char),45))),_tag_dyneither(_tmp891,sizeof(void*),0)));}}
# 1750
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1755
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1758
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1760
const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_PrintArg_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp895[0]=& _tmp894,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp896="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp896,sizeof(char),53))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}}
# 1764
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1769
void*_tmp3CB=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp3CE;_LL1C7: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3CC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3CB;if(_tmp3CC->tag != 1)goto _LL1C9;}_LL1C8:
# 1771
{const char*_tmp899;void*_tmp898;(_tmp898=0,Cyc_Tcutil_terr(loc,((_tmp899="only array index designators are supported",_tag_dyneither(_tmp899,sizeof(char),43))),_tag_dyneither(_tmp898,sizeof(void*),0)));}
goto _LL1C6;_LL1C9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3CD=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3CB;if(_tmp3CD->tag != 0)goto _LL1C6;else{_tmp3CE=_tmp3CD->f1;}}_LL1CA:
# 1774
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3CE);{
unsigned int _tmp3D2;int _tmp3D3;struct _tuple15 _tmp3D1=Cyc_Evexp_eval_const_uint_exp(_tmp3CE);_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;
if(!_tmp3D3){
const char*_tmp89C;void*_tmp89B;(_tmp89B=0,Cyc_Tcutil_terr(_tmp3CE->loc,((_tmp89C="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp89C,sizeof(char),47))),_tag_dyneither(_tmp89B,sizeof(void*),0)));}else{
if(_tmp3D2 != offset){
const char*_tmp8A1;void*_tmp8A0[2];struct Cyc_Int_pa_PrintArg_struct _tmp89F;struct Cyc_Int_pa_PrintArg_struct _tmp89E;(_tmp89E.tag=1,((_tmp89E.f1=(unsigned long)((int)_tmp3D2),((_tmp89F.tag=1,((_tmp89F.f1=(unsigned long)offset,((_tmp8A0[0]=& _tmp89F,((_tmp8A0[1]=& _tmp89E,Cyc_Tcutil_terr(_tmp3CE->loc,((_tmp8A1="expecting index designator %d but found %d",_tag_dyneither(_tmp8A1,sizeof(char),43))),_tag_dyneither(_tmp8A0,sizeof(void*),2)))))))))))));}}
# 1781
goto _LL1C6;};_LL1C6:;}}}{
# 1785
void*_tmp3DA=res_t2;_npop_handler(0);return _tmp3DA;};
# 1737
;_pop_region(r);}
# 1789
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1792
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3DB=Cyc_Tcutil_compress((void*)_check_null(bound->topt));_LL1CC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3DC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3DB;if(_tmp3DC->tag != 19)goto _LL1CE;}_LL1CD:
 goto _LL1CB;_LL1CE:;_LL1CF:
# 1796
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp8A5;void*_tmp8A4[1];struct Cyc_String_pa_PrintArg_struct _tmp8A3;(_tmp8A3.tag=0,((_tmp8A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8A4[0]=& _tmp8A3,Cyc_Tcutil_terr(bound->loc,((_tmp8A5="expecting unsigned int, found %s",_tag_dyneither(_tmp8A5,sizeof(char),33))),_tag_dyneither(_tmp8A4,sizeof(void*),1)))))));}_LL1CB:;}
# 1801
if(!(vd->tq).real_const){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8A8="comprehension index variable is not declared const!",_tag_dyneither(_tmp8A8,sizeof(char),52))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}{
# 1804
struct _RegionHandle _tmp3E2=_new_region("r");struct _RegionHandle*r=& _tmp3E2;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1810
struct Cyc_Tcenv_Tenv*_tmp3E3=te2;
void**_tmp3E4=0;
struct Cyc_Absyn_Tqual*_tmp3E5=0;
union Cyc_Absyn_Constraint**_tmp3E6=0;
# 1815
if(topt != 0){
void*_tmp3E7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3E9;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;union Cyc_Absyn_Constraint*_tmp3EE;_LL1D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E7;if(_tmp3E8->tag != 5)goto _LL1D3;else{_tmp3E9=_tmp3E8->f1;}}_LL1D2:
# 1818
{void**_tmp8A9;_tmp3E4=((_tmp8A9=_region_malloc(r,sizeof(*_tmp8A9)),((_tmp8A9[0]=_tmp3E9.elt_typ,_tmp8A9))));}
{struct Cyc_Absyn_Tqual*_tmp8AA;_tmp3E5=((_tmp8AA=_region_malloc(r,sizeof(*_tmp8AA)),((_tmp8AA[0]=_tmp3E9.elt_tq,_tmp8AA))));}
{union Cyc_Absyn_Constraint**_tmp8AB;_tmp3E6=((_tmp8AB=_region_malloc(r,sizeof(*_tmp8AB)),((_tmp8AB[0]=(_tmp3E9.ptr_atts).zero_term,_tmp8AB))));}
goto _LL1D0;_LL1D3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E7;if(_tmp3EA->tag != 8)goto _LL1D5;else{_tmp3EB=(_tmp3EA->f1).elt_type;_tmp3EC=(_tmp3EA->f1).tq;_tmp3ED=(_tmp3EA->f1).num_elts;_tmp3EE=(_tmp3EA->f1).zero_term;}}_LL1D4:
# 1823
{void**_tmp8AC;_tmp3E4=((_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC[0]=_tmp3EB,_tmp8AC))));}
{struct Cyc_Absyn_Tqual*_tmp8AD;_tmp3E5=((_tmp8AD=_region_malloc(r,sizeof(*_tmp8AD)),((_tmp8AD[0]=_tmp3EC,_tmp8AD))));}
{union Cyc_Absyn_Constraint**_tmp8AE;_tmp3E6=((_tmp8AE=_region_malloc(r,sizeof(*_tmp8AE)),((_tmp8AE[0]=_tmp3EE,_tmp8AE))));}
goto _LL1D0;_LL1D5:;_LL1D6:
# 1828
 goto _LL1D0;_LL1D0:;}{
# 1831
void*t=Cyc_Tcexp_tcExp(_tmp3E3,_tmp3E4,body);
if(_tmp3E3->le == 0){
# 1834
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp8B1;void*_tmp8B0;(_tmp8B0=0,Cyc_Tcutil_terr(bound->loc,((_tmp8B1="bound is not constant",_tag_dyneither(_tmp8B1,sizeof(char),22))),_tag_dyneither(_tmp8B0,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,Cyc_Tcutil_terr(bound->loc,((_tmp8B4="body is not constant",_tag_dyneither(_tmp8B4,sizeof(char),21))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}}
# 1839
if(_tmp3E6 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3E6)){
# 1842
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Absyn_uint_exp(1,0);_tmp3F9->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp3F9,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1847
void*_tmp3FA=Cyc_Absyn_array_typ(t,_tmp3E5 == 0?Cyc_Absyn_empty_tqual(0):*_tmp3E5,bound,
_tmp3E6 == 0?Cyc_Absyn_false_conref:*_tmp3E6,0);
void*_tmp3FB=_tmp3FA;_npop_handler(0);return _tmp3FB;};};};}
# 1805
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1863 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1867
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp8B5;adptr=((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=ad,_tmp8B5))));};}else{
# 1873
{struct _handler_cons _tmp3FD;_push_handler(& _tmp3FD);{int _tmp3FF=0;if(setjmp(_tmp3FD.handler))_tmp3FF=1;if(!_tmp3FF){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1873
;_pop_handler();}else{void*_tmp3FE=(void*)_exn_thrown;void*_tmp401=_tmp3FE;void*_tmp403;_LL1D8: {struct Cyc_Dict_Absent_exn_struct*_tmp402=(struct Cyc_Dict_Absent_exn_struct*)_tmp401;if(_tmp402->tag != Cyc_Dict_Absent)goto _LL1DA;}_LL1D9:
# 1876
{const char*_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_PrintArg_struct _tmp8B7;(_tmp8B7.tag=0,((_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8B8[0]=& _tmp8B7,Cyc_Tcutil_terr(loc,((_tmp8B9="unbound struct/union name %s",_tag_dyneither(_tmp8B9,sizeof(char),29))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1DA: _tmp403=_tmp401;_LL1DB:(void)_throw(_tmp403);_LL1D7:;}};}
# 1879
*ad_opt=ad;
*tn=ad->name;}
# 1882
if(ad->impl == 0){
{const char*_tmp8C1;void*_tmp8C0[1];const char*_tmp8BF;const char*_tmp8BE;struct Cyc_String_pa_PrintArg_struct _tmp8BD;(_tmp8BD.tag=0,((_tmp8BD.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8BE="struct",_tag_dyneither(_tmp8BE,sizeof(char),7)):((_tmp8BF="union",_tag_dyneither(_tmp8BF,sizeof(char),6)))),((_tmp8C0[0]=& _tmp8BD,Cyc_Tcutil_terr(loc,((_tmp8C1="can't construct abstract %s",_tag_dyneither(_tmp8C1,sizeof(char),28))),_tag_dyneither(_tmp8C0,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1886
struct _RegionHandle _tmp40C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp40C;_push_region(rgn);
# 1888
{struct _tuple12 _tmp8C2;struct _tuple12 _tmp40D=(_tmp8C2.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8C2.f2=rgn,_tmp8C2)));
struct Cyc_List_List*_tmp40E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp40D,ad->tvs);
struct Cyc_List_List*_tmp40F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp40D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp410=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp40E);
struct Cyc_List_List*_tmp411=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp40F);
struct Cyc_List_List*_tmp412=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp40E,_tmp40F);
void*res_typ;
# 1899
if(topt != 0){
void*_tmp413=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Aggrdecl**_tmp415;struct Cyc_List_List*_tmp416;_LL1DD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp413;if(_tmp414->tag != 11)goto _LL1DF;else{if((((_tmp414->f1).aggr_info).KnownAggr).tag != 2)goto _LL1DF;_tmp415=(struct Cyc_Absyn_Aggrdecl**)(((_tmp414->f1).aggr_info).KnownAggr).val;_tmp416=(_tmp414->f1).targs;}}_LL1DE:
# 1902
 if(*_tmp415 == *adptr){
{struct Cyc_List_List*_tmp417=_tmp410;for(0;_tmp417 != 0  && _tmp416 != 0;
(_tmp417=_tmp417->tl,_tmp416=_tmp416->tl)){
Cyc_Tcutil_unify((void*)_tmp417->hd,(void*)_tmp416->hd);}}
# 1907
res_typ=*topt;
goto _LL1DC;}
# 1910
goto _LL1E0;_LL1DF:;_LL1E0: {
# 1912
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8C8;struct Cyc_Absyn_AggrInfo _tmp8C7;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C6;res_typ=(void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C8.tag=11,((_tmp8C8.f1=((_tmp8C7.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8C7.targs=_tmp410,_tmp8C7)))),_tmp8C8)))),_tmp8C6))));}_LL1DC:;}else{
# 1915
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8CE;struct Cyc_Absyn_AggrInfo _tmp8CD;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8CC;res_typ=(void*)((_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CE.tag=11,((_tmp8CE.f1=((_tmp8CD.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8CD.targs=_tmp410,_tmp8CD)))),_tmp8CE)))),_tmp8CC))));}{
# 1918
struct Cyc_List_List*_tmp41E=*ts;
struct Cyc_List_List*_tmp41F=_tmp411;
while(_tmp41E != 0  && _tmp41F != 0){
# 1922
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp41E->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp41E->hd);
Cyc_Tcutil_unify((void*)_tmp41E->hd,(void*)_tmp41F->hd);
_tmp41E=_tmp41E->tl;
_tmp41F=_tmp41F->tl;}
# 1928
if(_tmp41E != 0){
const char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(loc,((_tmp8D1="too many explicit witness types",_tag_dyneither(_tmp8D1,sizeof(char),32))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}
# 1931
*ts=_tmp411;{
# 1934
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp423;struct Cyc_Absyn_Exp*_tmp424;struct _tuple20*_tmp422=(struct _tuple20*)fields->hd;_tmp423=_tmp422->f1;_tmp424=_tmp422->f2;{
void*_tmp425=Cyc_Tcutil_rsubstitute(rgn,_tmp412,_tmp423->type);
Cyc_Tcexp_tcExpInitializer(te,& _tmp425,_tmp424);
if(!Cyc_Tcutil_coerce_arg(te,_tmp424,_tmp425,& bogus)){
{const char*_tmp8DD;void*_tmp8DC[5];struct Cyc_String_pa_PrintArg_struct _tmp8DB;const char*_tmp8DA;const char*_tmp8D9;struct Cyc_String_pa_PrintArg_struct _tmp8D8;struct Cyc_String_pa_PrintArg_struct _tmp8D7;struct Cyc_String_pa_PrintArg_struct _tmp8D6;struct Cyc_String_pa_PrintArg_struct _tmp8D5;(_tmp8D5.tag=0,((_tmp8D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1945
Cyc_Absynpp_typ2string((void*)_check_null(_tmp424->topt))),((_tmp8D6.tag=0,((_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1944
Cyc_Absynpp_typ2string(_tmp425)),((_tmp8D7.tag=0,((_tmp8D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8D8.tag=0,((_tmp8D8.f1=(struct _dyneither_ptr)(
# 1943
ad->kind == Cyc_Absyn_StructA?(_tmp8D9="struct",_tag_dyneither(_tmp8D9,sizeof(char),7)):((_tmp8DA="union",_tag_dyneither(_tmp8DA,sizeof(char),6)))),((_tmp8DB.tag=0,((_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp423->name),((_tmp8DC[0]=& _tmp8DB,((_tmp8DC[1]=& _tmp8D8,((_tmp8DC[2]=& _tmp8D7,((_tmp8DC[3]=& _tmp8D6,((_tmp8DC[4]=& _tmp8D5,Cyc_Tcutil_terr(_tmp424->loc,((_tmp8DD="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp8DD,sizeof(char),40))),_tag_dyneither(_tmp8DC,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 1946
Cyc_Tcutil_explain_failure();}};}{
# 1949
struct Cyc_List_List*_tmp42F=0;
{struct Cyc_List_List*_tmp430=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp430 != 0;_tmp430=_tmp430->tl){
struct _tuple0*_tmp8E0;struct Cyc_List_List*_tmp8DF;_tmp42F=((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF->hd=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp412,(*((struct _tuple0*)_tmp430->hd)).f1),((_tmp8E0->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp412,(*((struct _tuple0*)_tmp430->hd)).f2),_tmp8E0)))))),((_tmp8DF->tl=_tmp42F,_tmp8DF))))));}}
# 1954
_tmp42F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42F);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp42F);{
void*_tmp433=res_typ;_npop_handler(0);return _tmp433;};};};};}
# 1888
;_pop_region(rgn);};}
# 1960
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 1962
struct _RegionHandle _tmp435=_new_region("rgn");struct _RegionHandle*rgn=& _tmp435;_push_region(rgn);{
void*_tmp436=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp438;struct Cyc_List_List*_tmp439;_LL1E2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp437=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp436;if(_tmp437->tag != 12)goto _LL1E4;else{_tmp438=_tmp437->f1;_tmp439=_tmp437->f2;}}_LL1E3:
# 1965
 if(_tmp438 == Cyc_Absyn_UnionA){
const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,Cyc_Tcutil_terr(loc,((_tmp8E3="expecting struct but found union",_tag_dyneither(_tmp8E3,sizeof(char),33))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp439);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct _tuple20*_tmp43C=(struct _tuple20*)fields->hd;_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp43D->type,_tmp43E);
if(!Cyc_Tcutil_coerce_arg(te,_tmp43E,_tmp43D->type,& bogus)){
{const char*_tmp8E9;void*_tmp8E8[3];struct Cyc_String_pa_PrintArg_struct _tmp8E7;struct Cyc_String_pa_PrintArg_struct _tmp8E6;struct Cyc_String_pa_PrintArg_struct _tmp8E5;(_tmp8E5.tag=0,((_tmp8E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1976
Cyc_Absynpp_typ2string((void*)_check_null(_tmp43E->topt))),((_tmp8E6.tag=0,((_tmp8E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1975
Cyc_Absynpp_typ2string(_tmp43D->type)),((_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp43D->name),((_tmp8E8[0]=& _tmp8E7,((_tmp8E8[1]=& _tmp8E6,((_tmp8E8[2]=& _tmp8E5,Cyc_Tcutil_terr(_tmp43E->loc,((_tmp8E9="field %s of struct expects type %s != %s",_tag_dyneither(_tmp8E9,sizeof(char),41))),_tag_dyneither(_tmp8E8,sizeof(void*),3)))))))))))))))))));}
# 1977
Cyc_Tcutil_explain_failure();}}
# 1980
goto _LL1E1;};_LL1E4:;_LL1E5: {
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8EC="tcAnonStruct: wrong type",_tag_dyneither(_tmp8EC,sizeof(char),25))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}_LL1E1:;}{
# 1984
void*_tmp446=ts;_npop_handler(0);return _tmp446;};
# 1962
;_pop_region(rgn);}
# 1988
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1991
struct _RegionHandle _tmp447=_new_region("rgn");struct _RegionHandle*rgn=& _tmp447;_push_region(rgn);
{struct _tuple12 _tmp8ED;struct _tuple12 _tmp448=(_tmp8ED.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8ED.f2=rgn,_tmp8ED)));
struct Cyc_List_List*_tmp449=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp448,tud->tvs);
struct Cyc_List_List*_tmp44A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp449);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp8F3;struct Cyc_Absyn_DatatypeFieldInfo _tmp8F2;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8F1;void*res=(void*)(
(_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F3.tag=4,((_tmp8F3.f1=((_tmp8F2.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8F2.targs=_tmp44A,_tmp8F2)))),_tmp8F3)))),_tmp8F1))));
# 1999
if(topt != 0){
void*_tmp44B=Cyc_Tcutil_compress(*topt);_LL1E7: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp44B;if(_tmp44C->tag != 4)goto _LL1E9;}_LL1E8:
 Cyc_Tcutil_unify(*topt,res);goto _LL1E6;_LL1E9:;_LL1EA:
 goto _LL1E6;_LL1E6:;}{
# 2005
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;
if(_tmp449 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp449,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp8FB;void*_tmp8FA[3];struct Cyc_String_pa_PrintArg_struct _tmp8F9;struct Cyc_String_pa_PrintArg_struct _tmp8F8;const char*_tmp8F7;struct Cyc_String_pa_PrintArg_struct _tmp8F6;(_tmp8F6.tag=0,((_tmp8F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2016
e->topt == 0?(struct _dyneither_ptr)((_tmp8F7="?",_tag_dyneither(_tmp8F7,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2015
Cyc_Absynpp_typ2string(t)),((_tmp8F9.tag=0,((_tmp8F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8FA[0]=& _tmp8F9,((_tmp8FA[1]=& _tmp8F8,((_tmp8FA[2]=& _tmp8F6,Cyc_Tcutil_terr(e->loc,((_tmp8FB="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp8FB,sizeof(char),82))),_tag_dyneither(_tmp8FA,sizeof(void*),3)))))))))))))))))));}
# 2017
Cyc_Tcutil_explain_failure();}}
# 2020
if(es != 0){
const char*_tmp8FF;void*_tmp8FE[1];struct Cyc_String_pa_PrintArg_struct _tmp8FD;void*_tmp456=(_tmp8FD.tag=0,((_tmp8FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2023
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8FE[0]=& _tmp8FD,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp8FF="too many arguments for datatype constructor %s",_tag_dyneither(_tmp8FF,sizeof(char),47))),_tag_dyneither(_tmp8FE,sizeof(void*),1)))))));_npop_handler(0);return _tmp456;}
if(ts != 0){
const char*_tmp903;void*_tmp902[1];struct Cyc_String_pa_PrintArg_struct _tmp901;void*_tmp45A=(_tmp901.tag=0,((_tmp901.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp902[0]=& _tmp901,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp903="too few arguments for datatype constructor %s",_tag_dyneither(_tmp903,sizeof(char),46))),_tag_dyneither(_tmp902,sizeof(void*),1)))))));_npop_handler(0);return _tmp45A;}{
void*_tmp45B=res;_npop_handler(0);return _tmp45B;};};}
# 1992
;_pop_region(rgn);}
# 2031
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp460=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp467;void*_tmp46B;struct Cyc_List_List*_tmp46E;union Cyc_Absyn_AggrInfoU _tmp470;struct Cyc_List_List*_tmp471;struct Cyc_List_List*_tmp475;_LL1EC: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp460;if(_tmp461->tag != 0)goto _LL1EE;}_LL1ED:
 return 1;_LL1EE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp460;if(_tmp462->tag != 1)goto _LL1F0;}_LL1EF:
 goto _LL1F1;_LL1F0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp460;if(_tmp463->tag != 2)goto _LL1F2;}_LL1F1:
 goto _LL1F3;_LL1F2: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp460;if(_tmp464->tag != 3)goto _LL1F4;}_LL1F3:
 goto _LL1F5;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp460;if(_tmp465->tag != 4)goto _LL1F6;}_LL1F5:
 return 0;_LL1F6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp460;if(_tmp466->tag != 5)goto _LL1F8;else{_tmp467=((_tmp466->f1).ptr_atts).nullable;}}_LL1F7:
# 2039
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp467);_LL1F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp460;if(_tmp468->tag != 6)goto _LL1FA;}_LL1F9:
 goto _LL1FB;_LL1FA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp460;if(_tmp469->tag != 7)goto _LL1FC;}_LL1FB:
 return 1;_LL1FC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp460;if(_tmp46A->tag != 8)goto _LL1FE;else{_tmp46B=(_tmp46A->f1).elt_type;}}_LL1FD:
 return Cyc_Tcexp_zeroable_type(_tmp46B);_LL1FE: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp460;if(_tmp46C->tag != 9)goto _LL200;}_LL1FF:
 return 0;_LL200: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp460;if(_tmp46D->tag != 10)goto _LL202;else{_tmp46E=_tmp46D->f1;}}_LL201:
# 2045
 for(0;(unsigned int)_tmp46E;_tmp46E=_tmp46E->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp46E->hd)).f2))return 0;}
return 1;_LL202: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp460;if(_tmp46F->tag != 11)goto _LL204;else{_tmp470=(_tmp46F->f1).aggr_info;_tmp471=(_tmp46F->f1).targs;}}_LL203: {
# 2049
struct Cyc_Absyn_Aggrdecl*_tmp482=Cyc_Absyn_get_known_aggrdecl(_tmp470);
if(_tmp482->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp483=_new_region("r");struct _RegionHandle*r=& _tmp483;_push_region(r);
{struct Cyc_List_List*_tmp484=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp482->tvs,_tmp471);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp484,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp485=0;_npop_handler(0);return _tmp485;}}}{
int _tmp486=1;_npop_handler(0);return _tmp486;};}
# 2053
;_pop_region(r);};}_LL204: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp460;if(_tmp472->tag != 13)goto _LL206;}_LL205:
# 2058
 return 1;_LL206: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp460;if(_tmp473->tag != 19)goto _LL208;}_LL207:
 return 1;_LL208: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp460;if(_tmp474->tag != 12)goto _LL20A;else{_tmp475=_tmp474->f2;}}_LL209:
# 2061
 for(0;_tmp475 != 0;_tmp475=_tmp475->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp475->hd)->type))return 0;}
return 1;_LL20A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp460;if(_tmp476->tag != 14)goto _LL20C;}_LL20B:
 return 1;_LL20C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp460;if(_tmp477->tag != 17)goto _LL20E;}_LL20D:
 return 0;_LL20E: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp460;if(_tmp478->tag != 16)goto _LL210;}_LL20F:
 return 0;_LL210: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp479=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp460;if(_tmp479->tag != 15)goto _LL212;}_LL211:
 return 0;_LL212: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp460;if(_tmp47A->tag != 26)goto _LL214;}_LL213:
# 2069
 goto _LL215;_LL214: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp460;if(_tmp47B->tag != 18)goto _LL216;}_LL215:
 goto _LL217;_LL216: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp460;if(_tmp47C->tag != 20)goto _LL218;}_LL217:
 goto _LL219;_LL218: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp460;if(_tmp47D->tag != 21)goto _LL21A;}_LL219:
 goto _LL21B;_LL21A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp460;if(_tmp47E->tag != 22)goto _LL21C;}_LL21B:
 goto _LL21D;_LL21C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp460;if(_tmp47F->tag != 23)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp460;if(_tmp480->tag != 24)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp460;if(_tmp481->tag != 25)goto _LL1EB;}_LL221: {
const char*_tmp907;void*_tmp906[1];struct Cyc_String_pa_PrintArg_struct _tmp905;(_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp906[0]=& _tmp905,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp907="bad type `%s' in zeroable type",_tag_dyneither(_tmp907,sizeof(char),31))),_tag_dyneither(_tmp906,sizeof(void*),1)))))));}_LL1EB:;}
# 2080
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2082
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2084
if(topt != 0){
void*_tmp48A=Cyc_Tcutil_compress(*topt);void*_tmp48C;_LL223: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48A;if(_tmp48B->tag != 5)goto _LL225;else{_tmp48C=(_tmp48B->f1).elt_typ;}}_LL224:
# 2087
 Cyc_Tcutil_unify(_tmp48C,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL222;_LL225:;_LL226:
 goto _LL222;_LL222:;}{
# 2093
const char*_tmp910;void*_tmp90F[2];const char*_tmp90E;const char*_tmp90D;struct Cyc_String_pa_PrintArg_struct _tmp90C;struct Cyc_String_pa_PrintArg_struct _tmp90B;(_tmp90B.tag=0,((_tmp90B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp90C.tag=0,((_tmp90C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp90D="calloc",_tag_dyneither(_tmp90D,sizeof(char),7))):(struct _dyneither_ptr)((_tmp90E="malloc",_tag_dyneither(_tmp90E,sizeof(char),7))))),((_tmp90F[0]=& _tmp90C,((_tmp90F[1]=& _tmp90B,Cyc_Tcutil_terr(loc,((_tmp910="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp910,sizeof(char),60))),_tag_dyneither(_tmp90F,sizeof(void*),2)))))))))))));};}
# 2097
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2099
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL227:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL228:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2105
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2110
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2113
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp913;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp912;void*expected_type=(void*)(
(_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=15,((_tmp913.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp913)))),_tmp912))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp493=Cyc_Tcutil_compress(handle_type);void*_tmp495;_LL22B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp493;if(_tmp494->tag != 15)goto _LL22D;else{_tmp495=(void*)_tmp494->f1;}}_LL22C:
# 2118
 rgn=_tmp495;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22A;_LL22D:;_LL22E:
# 2122
{const char*_tmp917;void*_tmp916[1];struct Cyc_String_pa_PrintArg_struct _tmp915;(_tmp915.tag=0,((_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp916[0]=& _tmp915,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp917="expecting region_t type but found %s",_tag_dyneither(_tmp917,sizeof(char),37))),_tag_dyneither(_tmp916,sizeof(void*),1)))))));}
goto _LL22A;_LL22A:;}else{
# 2129
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2137
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,*e);{
# 2146 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp91A;void*_tmp919;(_tmp919=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp91A="calloc with empty type",_tag_dyneither(_tmp91A,sizeof(char),23))),_tag_dyneither(_tmp919,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2158
void*er=(*e)->r;
retry_sizeof: {
void*_tmp49D=er;void*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;_LL230: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp49E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp49E->tag != 16)goto _LL232;else{_tmp49F=(void*)_tmp49E->f1;}}_LL231:
# 2162
 elt_type=_tmp49F;
{void**_tmp91B;*t=((_tmp91B=_cycalloc(sizeof(*_tmp91B)),((_tmp91B[0]=elt_type,_tmp91B))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL22F;_LL232: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4A0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp49D;if(_tmp4A0->tag != 2)goto _LL234;else{if(_tmp4A0->f1 != Cyc_Absyn_Times)goto _LL234;if(_tmp4A0->f2 == 0)goto _LL234;_tmp4A1=(struct Cyc_Absyn_Exp*)(_tmp4A0->f2)->hd;if((_tmp4A0->f2)->tl == 0)goto _LL234;_tmp4A2=(struct Cyc_Absyn_Exp*)((_tmp4A0->f2)->tl)->hd;if(((_tmp4A0->f2)->tl)->tl != 0)goto _LL234;}}_LL233:
# 2169
{struct _tuple0 _tmp91C;struct _tuple0 _tmp4A5=(_tmp91C.f1=_tmp4A1->r,((_tmp91C.f2=_tmp4A2->r,_tmp91C)));void*_tmp4A7;void*_tmp4A9;_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4A6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4A5.f1;if(_tmp4A6->tag != 16)goto _LL239;else{_tmp4A7=(void*)_tmp4A6->f1;}}_LL238:
# 2171
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4A7);
elt_type=_tmp4A7;
{void**_tmp91D;*t=((_tmp91D=_cycalloc(sizeof(*_tmp91D)),((_tmp91D[0]=elt_type,_tmp91D))));}
num_elts=_tmp4A2;
one_elt=0;
goto _LL236;_LL239: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4A8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4A5.f2;if(_tmp4A8->tag != 16)goto _LL23B;else{_tmp4A9=(void*)_tmp4A8->f1;}}_LL23A:
# 2178
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4A9);
elt_type=_tmp4A9;
{void**_tmp91E;*t=((_tmp91E=_cycalloc(sizeof(*_tmp91E)),((_tmp91E[0]=elt_type,_tmp91E))));}
num_elts=_tmp4A1;
one_elt=0;
goto _LL236;_LL23B:;_LL23C:
 goto No_sizeof;_LL236:;}
# 2186
goto _LL22F;_LL234:;_LL235:
# 2188
 No_sizeof: {
# 2191
struct Cyc_Absyn_Exp*_tmp4AC=*e;
{void*_tmp4AD=_tmp4AC->r;struct Cyc_Absyn_Exp*_tmp4AF;_LL23E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4AE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AD;if(_tmp4AE->tag != 13)goto _LL240;else{_tmp4AF=_tmp4AE->f2;}}_LL23F:
 _tmp4AC=_tmp4AF;goto _LL23D;_LL240:;_LL241:
 goto _LL23D;_LL23D:;}
# 2196
{void*_tmp4B0=Cyc_Tcutil_compress((void*)_check_null(_tmp4AC->topt));void*_tmp4B2;_LL243: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4B0;if(_tmp4B1->tag != 19)goto _LL245;else{_tmp4B2=(void*)_tmp4B1->f1;}}_LL244:
# 2198
{void*_tmp4B3=Cyc_Tcutil_compress(_tmp4B2);struct Cyc_Absyn_Exp*_tmp4B5;_LL248: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4B3;if(_tmp4B4->tag != 18)goto _LL24A;else{_tmp4B5=_tmp4B4->f1;}}_LL249:
# 2200
 er=_tmp4B5->r;goto retry_sizeof;_LL24A:;_LL24B:
 goto _LL247;_LL247:;}
# 2203
goto _LL242;_LL245:;_LL246:
 goto _LL242;_LL242:;}
# 2206
elt_type=Cyc_Absyn_char_typ;
{void**_tmp91F;*t=((_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=elt_type,_tmp91F))));}
num_elts=*e;
one_elt=0;}
# 2211
goto _LL22F;_LL22F:;}}
# 2215
*is_fat=!one_elt;
# 2218
{void*_tmp4B7=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4B9;_LL24D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B7;if(_tmp4B8->tag != 11)goto _LL24F;else{if((((_tmp4B8->f1).aggr_info).KnownAggr).tag != 2)goto _LL24F;_tmp4B9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4B8->f1).aggr_info).KnownAggr).val);}}_LL24E:
# 2220
 if(_tmp4B9->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4B9->impl))->exist_vars != 0){
const char*_tmp922;void*_tmp921;(_tmp921=0,Cyc_Tcutil_terr(loc,((_tmp922="malloc with existential types not yet implemented",_tag_dyneither(_tmp922,sizeof(char),50))),_tag_dyneither(_tmp921,sizeof(void*),0)));}
goto _LL24C;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}{
# 2227
void*(*_tmp4BC)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4BD=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp4BE=Cyc_Tcutil_compress(*topt);union Cyc_Absyn_Constraint*_tmp4C0;union Cyc_Absyn_Constraint*_tmp4C1;union Cyc_Absyn_Constraint*_tmp4C2;_LL252: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4BE;if(_tmp4BF->tag != 5)goto _LL254;else{_tmp4C0=((_tmp4BF->f1).ptr_atts).nullable;_tmp4C1=((_tmp4BF->f1).ptr_atts).bounds;_tmp4C2=((_tmp4BF->f1).ptr_atts).zero_term;}}_LL253:
# 2232
 _tmp4BD=_tmp4C2;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4C0))
_tmp4BC=Cyc_Absyn_star_typ;
# 2237
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4C2) && !(*is_calloc)){
{const char*_tmp925;void*_tmp924;(_tmp924=0,Cyc_Tcutil_warn(loc,((_tmp925="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp925,sizeof(char),55))),_tag_dyneither(_tmp924,sizeof(void*),0)));}
*is_calloc=1;}
# 2243
{void*_tmp4C5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4C1);struct Cyc_Absyn_Exp*_tmp4C8;_LL257: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4C6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4C5;if(_tmp4C6->tag != 0)goto _LL259;}_LL258:
 goto _LL256;_LL259:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4C7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4C5;if(_tmp4C7->tag != 1)goto _LL25B;else{_tmp4C8=_tmp4C7->f1;}}if(!(!one_elt))goto _LL25B;_LL25A: {
# 2246
int _tmp4C9=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4C9  && Cyc_Evexp_same_const_exp(_tmp4C8,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C1),_tmp4BD);}
# 2252
{void*_tmp4CA=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4CC;_LL25E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4CA;if(_tmp4CB->tag != 19)goto _LL260;else{_tmp4CC=(void*)_tmp4CB->f1;}}_LL25F: {
# 2254
struct Cyc_Absyn_Exp*_tmp4CD=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4CC,0),0,Cyc_Absyn_No_coercion,0);
# 2256
if(Cyc_Evexp_same_const_exp(_tmp4CD,_tmp4C8)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4C1),_tmp4BD);}
# 2261
goto _LL25D;}_LL260:;_LL261:
 goto _LL25D;_LL25D:;}
# 2264
goto _LL256;}_LL25B:;_LL25C:
 goto _LL256;_LL256:;}
# 2267
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}
# 2270
if(!one_elt)_tmp4BC=Cyc_Absyn_dyneither_typ;
return _tmp4BC(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4BD);};};}
# 2275
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2281
struct _RegionHandle _tmp4CE=_new_region("r");struct _RegionHandle*r=& _tmp4CE;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4CF=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp4CF,0,e1);{
void*_tmp4D0=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp4CF,& _tmp4D0,e2);};}{
# 2287
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2290
{void*_tmp4D1=Cyc_Tcutil_compress(t1);_LL263: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D1;if(_tmp4D2->tag != 8)goto _LL265;}_LL264:
{const char*_tmp928;void*_tmp927;(_tmp927=0,Cyc_Tcutil_terr(loc,((_tmp928="cannot assign to an array",_tag_dyneither(_tmp928,sizeof(char),26))),_tag_dyneither(_tmp927,sizeof(void*),0)));}goto _LL262;_LL265:;_LL266:
 goto _LL262;_LL262:;}{
# 2295
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp92B;void*_tmp92A;(_tmp92A=0,Cyc_Tcutil_terr(loc,((_tmp92B="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp92B,sizeof(char),58))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}
# 2300
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp92E;void*_tmp92D;void*_tmp4D9=(_tmp92D=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp92E="swap non-lvalue",_tag_dyneither(_tmp92E,sizeof(char),16))),_tag_dyneither(_tmp92D,sizeof(void*),0)));_npop_handler(0);return _tmp4D9;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp931;void*_tmp930;void*_tmp4DC=(_tmp930=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp931="swap non-lvalue",_tag_dyneither(_tmp931,sizeof(char),16))),_tag_dyneither(_tmp930,sizeof(void*),0)));_npop_handler(0);return _tmp4DC;}{
# 2305
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp934;void*_tmp933;void*_tmp4DF=(_tmp933=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp934="swap value in zeroterm array",_tag_dyneither(_tmp934,sizeof(char),29))),_tag_dyneither(_tmp933,sizeof(void*),0)));_npop_handler(0);return _tmp4DF;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp937;void*_tmp936;void*_tmp4E2=(_tmp936=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp937="swap value in zeroterm array",_tag_dyneither(_tmp937,sizeof(char),29))),_tag_dyneither(_tmp936,sizeof(void*),0)));_npop_handler(0);return _tmp4E2;}
# 2312
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp93C;void*_tmp93B[2];struct Cyc_String_pa_PrintArg_struct _tmp93A;struct Cyc_String_pa_PrintArg_struct _tmp939;void*_tmp4E3=(_tmp939.tag=0,((_tmp939.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp93A.tag=0,((_tmp93A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp93B[0]=& _tmp93A,((_tmp93B[1]=& _tmp939,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp93C="type mismatch: %s != %s",_tag_dyneither(_tmp93C,sizeof(char),24))),_tag_dyneither(_tmp93B,sizeof(void*),2)))))))))))));
void*_tmp4E4=_tmp4E3;_npop_handler(0);return _tmp4E4;}{
# 2319
void*_tmp4E9=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp4E9;};};};};
# 2281
;_pop_region(r);}
# 2323
int Cyc_Tcexp_in_stmt_exp=0;
# 2325
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp4EA=_new_region("r");struct _RegionHandle*r=& _tmp4EA;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2332
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2334
while(1){
void*_tmp4EB=s->r;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Stmt*_tmp4EF;struct Cyc_Absyn_Stmt*_tmp4F0;struct Cyc_Absyn_Decl*_tmp4F2;struct Cyc_Absyn_Stmt*_tmp4F3;_LL268: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp4EC=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4EB;if(_tmp4EC->tag != 1)goto _LL26A;else{_tmp4ED=_tmp4EC->f1;}}_LL269: {
# 2338
void*_tmp4F4=(void*)_check_null(_tmp4ED->topt);
if(!Cyc_Tcutil_unify(_tmp4F4,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp940;void*_tmp93F[1];struct Cyc_String_pa_PrintArg_struct _tmp93E;(_tmp93E.tag=0,((_tmp93E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp4F4)),((_tmp93F[0]=& _tmp93E,Cyc_Tcutil_terr(loc,((_tmp940="statement expression returns type %s",_tag_dyneither(_tmp940,sizeof(char),37))),_tag_dyneither(_tmp93F,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2344
void*_tmp4F8=_tmp4F4;_npop_handler(0);return _tmp4F8;};}_LL26A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp4EE=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4EB;if(_tmp4EE->tag != 2)goto _LL26C;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}}_LL26B:
 s=_tmp4F0;continue;_LL26C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp4F1=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4EB;if(_tmp4F1->tag != 12)goto _LL26E;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;}}_LL26D:
 s=_tmp4F3;continue;_LL26E:;_LL26F: {
# 2348
const char*_tmp943;void*_tmp942;void*_tmp4FB=(_tmp942=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp943="statement expression must end with expression",_tag_dyneither(_tmp943,sizeof(char),46))),_tag_dyneither(_tmp942,sizeof(void*),0)));_npop_handler(0);return _tmp4FB;}_LL267:;}
# 2326
;_pop_region(r);}
# 2353
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
{void*_tmp4FC=t;struct Cyc_Absyn_Aggrdecl*_tmp4FE;_LL271: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FC;if(_tmp4FD->tag != 11)goto _LL273;else{if((((_tmp4FD->f1).aggr_info).KnownAggr).tag != 2)goto _LL273;_tmp4FE=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4FD->f1).aggr_info).KnownAggr).val);}}_LL272:
# 2357
 if((_tmp4FE->kind == Cyc_Absyn_UnionA  && _tmp4FE->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FE->impl))->tagged)goto _LL270;
goto _LL274;_LL273:;_LL274:
# 2360
{const char*_tmp947;void*_tmp946[1];struct Cyc_String_pa_PrintArg_struct _tmp945;(_tmp945.tag=0,((_tmp945.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp946[0]=& _tmp945,Cyc_Tcutil_terr(loc,((_tmp947="expecting @tagged union but found %s",_tag_dyneither(_tmp947,sizeof(char),37))),_tag_dyneither(_tmp946,sizeof(void*),1)))))));}
goto _LL270;_LL270:;}
# 2363
return Cyc_Absyn_uint_typ;}
# 2367
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2371
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2373
if(*rgn_handle != 0){
# 2375
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp94A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp949;void*expected_type=(void*)(
(_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949[0]=((_tmp94A.tag=15,((_tmp94A.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp94A)))),_tmp949))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp502=Cyc_Tcutil_compress(handle_type);void*_tmp504;_LL276: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp502;if(_tmp503->tag != 15)goto _LL278;else{_tmp504=(void*)_tmp503->f1;}}_LL277:
# 2380
 rgn=_tmp504;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL275;_LL278:;_LL279:
# 2384
{const char*_tmp94E;void*_tmp94D[1];struct Cyc_String_pa_PrintArg_struct _tmp94C;(_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp94D[0]=& _tmp94C,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp94E="expecting region_t type but found %s",_tag_dyneither(_tmp94E,sizeof(char),37))),_tag_dyneither(_tmp94D,sizeof(void*),1)))))));}
goto _LL275;_LL275:;}else{
# 2391
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2399
void*_tmp50A=e1->r;struct Cyc_Core_Opt*_tmp50D;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp510;_LL27B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp50A;if(_tmp50B->tag != 26)goto _LL27D;}_LL27C: {
# 2402
void*_tmp513=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
void*_tmp514=Cyc_Tcutil_compress(_tmp513);void*_tmp516;struct Cyc_Absyn_Tqual _tmp517;struct Cyc_Absyn_Exp*_tmp518;union Cyc_Absyn_Constraint*_tmp519;_LL288: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp514;if(_tmp515->tag != 8)goto _LL28A;else{_tmp516=(_tmp515->f1).elt_type;_tmp517=(_tmp515->f1).tq;_tmp518=(_tmp515->f1).num_elts;_tmp519=(_tmp515->f1).zero_term;}}_LL289: {
# 2405
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp518);
void*b;
{void*_tmp51A=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp51C;_LL28D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp51A;if(_tmp51B->tag != 19)goto _LL28F;else{_tmp51C=(void*)_tmp51B->f1;}}_LL28E:
# 2409
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp951;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp950;b=(void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=1,((_tmp951.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp51C,0),0,Cyc_Absyn_No_coercion,0),_tmp951)))),_tmp950))));}
goto _LL28C;_LL28F:;_LL290:
# 2412
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp954;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp953;b=(void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=1,((_tmp954.f1=bnd,_tmp954)))),_tmp953))));}else{
# 2415
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL28C:;}{
# 2417
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp95E;struct Cyc_Absyn_PtrAtts _tmp95D;struct Cyc_Absyn_PtrInfo _tmp95C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp95B;void*res_typ=(void*)(
(_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95E.tag=5,((_tmp95E.f1=((_tmp95C.elt_typ=_tmp516,((_tmp95C.elt_tq=_tmp517,((_tmp95C.ptr_atts=(
(_tmp95D.rgn=rgn,((_tmp95D.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp95D.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp95D.zero_term=_tmp519,((_tmp95D.ptrloc=0,_tmp95D)))))))))),_tmp95C)))))),_tmp95E)))),_tmp95B))));
# 2421
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2428
return res_typ;};}_LL28A:;_LL28B: {
const char*_tmp961;void*_tmp960;(_tmp960=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp961="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp961,sizeof(char),45))),_tag_dyneither(_tmp960,sizeof(void*),0)));}_LL287:;}_LL27D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp50A;if(_tmp50C->tag != 34)goto _LL27F;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;}}_LL27E:
# 2432
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp964;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp963;e1->r=(void*)((_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963[0]=((_tmp964.tag=25,((_tmp964.f1=_tmp50E,_tmp964)))),_tmp963))));}
_tmp510=_tmp50E;goto _LL280;_LL27F: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp50A;if(_tmp50F->tag != 25)goto _LL281;else{_tmp510=_tmp50F->f1;}}_LL280: {
# 2435
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp529=Cyc_Tcutil_compress(*topt);void**_tmp52B;struct Cyc_Absyn_Tqual _tmp52C;union Cyc_Absyn_Constraint*_tmp52D;_LL292: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp529;if(_tmp52A->tag != 5)goto _LL294;else{_tmp52B=(void**)&(_tmp52A->f1).elt_typ;_tmp52C=(_tmp52A->f1).elt_tq;_tmp52D=((_tmp52A->f1).ptr_atts).zero_term;}}_LL293:
# 2441
 elt_typ_opt=_tmp52B;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52D);
goto _LL291;_LL294:;_LL295:
 goto _LL291;_LL291:;}{
# 2447
void*_tmp52E=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,_tmp510);
e1->topt=_tmp52E;{
void*res_typ;
{void*_tmp52F=Cyc_Tcutil_compress(_tmp52E);void*_tmp531;struct Cyc_Absyn_Tqual _tmp532;struct Cyc_Absyn_Exp*_tmp533;union Cyc_Absyn_Constraint*_tmp534;_LL297: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 8)goto _LL299;else{_tmp531=(_tmp530->f1).elt_type;_tmp532=(_tmp530->f1).tq;_tmp533=(_tmp530->f1).num_elts;_tmp534=(_tmp530->f1).zero_term;}}_LL298:
# 2452
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp979;struct Cyc_Absyn_PtrAtts _tmp978;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp977;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp976;struct Cyc_Absyn_PtrInfo _tmp975;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp974;res_typ=(void*)(
(_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=((_tmp979.tag=5,((_tmp979.f1=((_tmp975.elt_typ=_tmp531,((_tmp975.elt_tq=_tmp532,((_tmp975.ptr_atts=(
(_tmp978.rgn=rgn,((_tmp978.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp978.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976[0]=((_tmp977.tag=1,((_tmp977.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp533),_tmp977)))),_tmp976))))),((_tmp978.zero_term=_tmp534,((_tmp978.ptrloc=0,_tmp978)))))))))),_tmp975)))))),_tmp979)))),_tmp974))));}
# 2457
if(topt != 0){
# 2461
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2467
goto _LL296;_LL299:;_LL29A: {
const char*_tmp97C;void*_tmp97B;(_tmp97B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp97C="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp97C,sizeof(char),50))),_tag_dyneither(_tmp97B,sizeof(void*),0)));}_LL296:;}
# 2470
return res_typ;};};}_LL281: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50A;if(_tmp511->tag != 0)goto _LL283;else{if(((_tmp511->f1).String_c).tag != 8)goto _LL283;}}_LL282: {
# 2475
void*_tmp53D=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2477
void*_tmp53E=Cyc_Tcexp_tcExp(te,& _tmp53D,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp97F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp97E;return Cyc_Absyn_atb_typ(_tmp53E,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=((_tmp97F.tag=1,((_tmp97F.f1=Cyc_Absyn_uint_exp(1,0),_tmp97F)))),_tmp97E)))),Cyc_Absyn_false_conref);}_LL283: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp50A;if(_tmp512->tag != 0)goto _LL285;else{if(((_tmp512->f1).Wstring_c).tag != 9)goto _LL285;}}_LL284: {
# 2482
void*_tmp541=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2484
void*_tmp542=Cyc_Tcexp_tcExp(te,& _tmp541,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp982;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp981;return Cyc_Absyn_atb_typ(_tmp542,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp982.tag=1,((_tmp982.f1=Cyc_Absyn_uint_exp(1,0),_tmp982)))),_tmp981)))),Cyc_Absyn_false_conref);}_LL285:;_LL286:
# 2491
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp545=Cyc_Tcutil_compress(*topt);void**_tmp547;struct Cyc_Absyn_Tqual _tmp548;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp545;if(_tmp546->tag != 5)goto _LL29E;else{_tmp547=(void**)&(_tmp546->f1).elt_typ;_tmp548=(_tmp546->f1).elt_tq;}}_LL29D:
# 2497
 topt2=_tmp547;goto _LL29B;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp545;if(_tmp549->tag != 3)goto _LL2A0;}_LL29F:
# 2501
 bogus=*topt;
topt2=& bogus;
goto _LL29B;_LL2A0:;_LL2A1:
 goto _LL29B;_LL29B:;}{
# 2507
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp98C;struct Cyc_Absyn_PtrAtts _tmp98B;struct Cyc_Absyn_PtrInfo _tmp98A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp989;void*res_typ=(void*)(
(_tmp989=_cycalloc(sizeof(*_tmp989)),((_tmp989[0]=((_tmp98C.tag=5,((_tmp98C.f1=((_tmp98A.elt_typ=telt,((_tmp98A.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp98A.ptr_atts=(
(_tmp98B.rgn=rgn,((_tmp98B.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp98B.bounds=Cyc_Absyn_bounds_one_conref,((_tmp98B.zero_term=Cyc_Absyn_false_conref,((_tmp98B.ptrloc=0,_tmp98B)))))))))),_tmp98A)))))),_tmp98C)))),_tmp989))));
# 2513
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2520
return res_typ;};}_LL27A:;};}
# 2526
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp54E=t;void*_tmp550;struct Cyc_Absyn_Tqual _tmp551;struct Cyc_Absyn_Exp*_tmp552;union Cyc_Absyn_Constraint*_tmp553;_LL2A3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54E;if(_tmp54F->tag != 8)goto _LL2A5;else{_tmp550=(_tmp54F->f1).elt_type;_tmp551=(_tmp54F->f1).tq;_tmp552=(_tmp54F->f1).num_elts;_tmp553=(_tmp54F->f1).zero_term;}}_LL2A4: {
# 2530
void*_tmp555;struct _tuple14 _tmp554=Cyc_Tcutil_addressof_props(te,e);_tmp555=_tmp554.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp98F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp98E;void*_tmp556=_tmp552 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=1,((_tmp98F.f1=_tmp552,_tmp98F)))),_tmp98E))));
t=Cyc_Absyn_atb_typ(_tmp550,_tmp555,_tmp551,_tmp556,_tmp553);
e->topt=t;
return t;};}_LL2A5:;_LL2A6:
# 2536
 return t;_LL2A2:;}
# 2543
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2547
struct _RegionHandle _tmp559=_new_region("temp");struct _RegionHandle*temp=& _tmp559;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2553
const char*_tmp992;void*_tmp991;(_tmp991=0,Cyc_Tcutil_terr(e->loc,((_tmp992="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp992,sizeof(char),49))),_tag_dyneither(_tmp991,sizeof(void*),0)));}{
# 2556
void*_tmp55C=e->r;_LL2A8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp55C;if(_tmp55D->tag != 25)goto _LL2AA;}_LL2A9:
 goto _LL2AB;_LL2AA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp55E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp55C;if(_tmp55E->tag != 26)goto _LL2AC;}_LL2AB:
 goto _LL2AD;_LL2AC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55C;if(_tmp55F->tag != 0)goto _LL2AE;else{if(((_tmp55F->f1).Wstring_c).tag != 9)goto _LL2AE;}}_LL2AD:
 goto _LL2AF;_LL2AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55C;if(_tmp560->tag != 0)goto _LL2B0;else{if(((_tmp560->f1).String_c).tag != 8)goto _LL2B0;}}_LL2AF: {
void*_tmp561=t;_npop_handler(0);return _tmp561;}_LL2B0:;_LL2B1:
# 2562
 t=Cyc_Tcutil_compress(t);{
void*_tmp562=t;void*_tmp564;struct Cyc_Absyn_Tqual _tmp565;struct Cyc_Absyn_Exp*_tmp566;union Cyc_Absyn_Constraint*_tmp567;_LL2B3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp562;if(_tmp563->tag != 8)goto _LL2B5;else{_tmp564=(_tmp563->f1).elt_type;_tmp565=(_tmp563->f1).tq;_tmp566=(_tmp563->f1).num_elts;_tmp567=(_tmp563->f1).zero_term;}}_LL2B4: {
# 2565
void*_tmp569;struct _tuple14 _tmp568=Cyc_Tcutil_addressof_props(te,e);_tmp569=_tmp568.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp995;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp994;void*b=_tmp566 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994[0]=((_tmp995.tag=1,((_tmp995.f1=_tmp566,_tmp995)))),_tmp994))));
t=Cyc_Absyn_atb_typ(_tmp564,_tmp569,_tmp565,b,_tmp567);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp56A=t;_npop_handler(0);return _tmp56A;};};}_LL2B5:;_LL2B6: {
# 2572
void*_tmp56D=t;_npop_handler(0);return _tmp56D;}_LL2B2:;};_LL2A7:;};
# 2547
;_pop_region(temp);}
# 2584 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp56E=e->r;struct Cyc_Absyn_Exp*_tmp570;_LL2B8: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp56F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp56E;if(_tmp56F->tag != 11)goto _LL2BA;else{_tmp570=_tmp56F->f1;}}_LL2B9:
# 2588
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp570);
_tmp570->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp570->topt),0);
e->topt=_tmp570->topt;
goto _LL2B7;_LL2BA:;_LL2BB:
# 2594
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2597
{void*_tmp571=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp573;struct Cyc_Absyn_Tqual _tmp574;void*_tmp575;union Cyc_Absyn_Constraint*_tmp576;union Cyc_Absyn_Constraint*_tmp577;union Cyc_Absyn_Constraint*_tmp578;_LL2BD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp571;if(_tmp572->tag != 5)goto _LL2BF;else{_tmp573=(_tmp572->f1).elt_typ;_tmp574=(_tmp572->f1).elt_tq;_tmp575=((_tmp572->f1).ptr_atts).rgn;_tmp576=((_tmp572->f1).ptr_atts).nullable;_tmp577=((_tmp572->f1).ptr_atts).bounds;_tmp578=((_tmp572->f1).ptr_atts).zero_term;}}_LL2BE:
# 2599
{void*_tmp579=Cyc_Tcutil_compress(_tmp573);struct Cyc_List_List*_tmp57B;void*_tmp57C;struct Cyc_Absyn_Tqual _tmp57D;void*_tmp57E;struct Cyc_List_List*_tmp57F;int _tmp580;struct Cyc_Absyn_VarargInfo*_tmp581;struct Cyc_List_List*_tmp582;struct Cyc_List_List*_tmp583;_LL2C2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp579;if(_tmp57A->tag != 9)goto _LL2C4;else{_tmp57B=(_tmp57A->f1).tvars;_tmp57C=(_tmp57A->f1).effect;_tmp57D=(_tmp57A->f1).ret_tqual;_tmp57E=(_tmp57A->f1).ret_typ;_tmp57F=(_tmp57A->f1).args;_tmp580=(_tmp57A->f1).c_varargs;_tmp581=(_tmp57A->f1).cyc_varargs;_tmp582=(_tmp57A->f1).rgn_po;_tmp583=(_tmp57A->f1).attributes;}}_LL2C3:
# 2601
 if(_tmp57B != 0){
struct _RegionHandle _tmp584=_new_region("rgn");struct _RegionHandle*rgn=& _tmp584;_push_region(rgn);
{struct _tuple12 _tmp996;struct _tuple12 _tmp585=(_tmp996.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp996.f2=rgn,_tmp996)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp585,_tmp57B);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp99C;struct Cyc_Absyn_FnInfo _tmp99B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp99A;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp99A=_cycalloc(sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99C.tag=9,((_tmp99C.f1=((_tmp99B.tvars=0,((_tmp99B.effect=_tmp57C,((_tmp99B.ret_tqual=_tmp57D,((_tmp99B.ret_typ=_tmp57E,((_tmp99B.args=_tmp57F,((_tmp99B.c_varargs=_tmp580,((_tmp99B.cyc_varargs=_tmp581,((_tmp99B.rgn_po=_tmp582,((_tmp99B.attributes=_tmp583,_tmp99B)))))))))))))))))),_tmp99C)))),_tmp99A)))));
# 2611
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9A6;struct Cyc_Absyn_PtrAtts _tmp9A5;struct Cyc_Absyn_PtrInfo _tmp9A4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9A3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp586=(_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3[0]=((_tmp9A6.tag=5,((_tmp9A6.f1=((_tmp9A4.elt_typ=ftyp,((_tmp9A4.elt_tq=_tmp574,((_tmp9A4.ptr_atts=((_tmp9A5.rgn=_tmp575,((_tmp9A5.nullable=_tmp576,((_tmp9A5.bounds=_tmp577,((_tmp9A5.zero_term=_tmp578,((_tmp9A5.ptrloc=0,_tmp9A5)))))))))),_tmp9A4)))))),_tmp9A6)))),_tmp9A3)));
# 2613
struct Cyc_Absyn_Exp*_tmp587=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9A9;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9A8;e->r=(void*)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=12,((_tmp9A9.f1=_tmp587,((_tmp9A9.f2=ts,_tmp9A9)))))),_tmp9A8))));}
e->topt=(void*)_tmp586;}
# 2603
;_pop_region(rgn);}
# 2618
goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}
# 2621
goto _LL2BC;_LL2BF:;_LL2C0:
 goto _LL2BC;_LL2BC:;}
# 2624
goto _LL2B7;_LL2B7:;}
# 2626
return(void*)_check_null(e->topt);}
# 2634
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2636
struct _RegionHandle _tmp592=_new_region("r");struct _RegionHandle*r=& _tmp592;_push_region(r);
{struct Cyc_List_List*_tmp593=0;
# 2639
{void*_tmp594=fn_exp->r;struct Cyc_List_List*_tmp596;_LL2C7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp595=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp594;if(_tmp595->tag != 9)goto _LL2C9;else{_tmp596=_tmp595->f2;}}_LL2C8:
# 2641
{void*_tmp597=e->r;struct Cyc_List_List*_tmp599;_LL2CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp598=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp597;if(_tmp598->tag != 9)goto _LL2CE;else{_tmp599=_tmp598->f2;}}_LL2CD: {
# 2643
struct Cyc_List_List*_tmp59A=alias_arg_exps;
int _tmp59B=0;
while(_tmp59A != 0){
while(_tmp59B != (int)_tmp59A->hd){
if(_tmp596 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp9B3;struct Cyc_Int_pa_PrintArg_struct _tmp9B2;void*_tmp9B1[2];const char*_tmp9B0;void*_tmp9AF;(_tmp9AF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp9B3.tag=1,((_tmp9B3.f1=(unsigned long)((int)_tmp59A->hd),((_tmp9B2.tag=1,((_tmp9B2.f1=(unsigned long)_tmp59B,((_tmp9B1[0]=& _tmp9B2,((_tmp9B1[1]=& _tmp9B3,Cyc_aprintf(((_tmp9B0="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9B0,sizeof(char),36))),_tag_dyneither(_tmp9B1,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9AF,sizeof(void*),0)));}
# 2650
++ _tmp59B;
_tmp596=_tmp596->tl;
_tmp599=((struct Cyc_List_List*)_check_null(_tmp599))->tl;}{
# 2655
struct Cyc_Absyn_Decl*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;struct _tuple11 _tmp5A1=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp596))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp599))->hd)->topt)));_tmp5A2=_tmp5A1.f1;_tmp5A3=_tmp5A1.f2;
_tmp596->hd=(void*)_tmp5A3;
{struct Cyc_List_List*_tmp9B4;_tmp593=((_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->hd=_tmp5A2,((_tmp9B4->tl=_tmp593,_tmp9B4))))));}
_tmp59A=_tmp59A->tl;};}
# 2660
goto _LL2CB;}_LL2CE:;_LL2CF: {
const char*_tmp9B7;void*_tmp9B6;(_tmp9B6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B7="not a function call!",_tag_dyneither(_tmp9B7,sizeof(char),21))),_tag_dyneither(_tmp9B6,sizeof(void*),0)));}_LL2CB:;}
# 2663
goto _LL2C6;_LL2C9:;_LL2CA: {
const char*_tmp9BA;void*_tmp9B9;(_tmp9B9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9BA="not a function call!",_tag_dyneither(_tmp9BA,sizeof(char),21))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));}_LL2C6:;}
# 2668
while(_tmp593 != 0){
struct Cyc_Absyn_Decl*_tmp5A9=(struct Cyc_Absyn_Decl*)_tmp593->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5A9,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp593=_tmp593->tl;}
# 2675
e->topt=0;
e->r=fn_exp->r;}
# 2637
;_pop_region(r);}
# 2682
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2686
{void*_tmp5AA=e->r;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_VarargCallInfo**_tmp5B0;int*_tmp5B1;struct Cyc_Core_Opt*_tmp5B3;struct Cyc_List_List*_tmp5B4;union Cyc_Absyn_Cnst*_tmp5B6;struct _tuple1*_tmp5B8;void**_tmp5B9;enum Cyc_Absyn_Primop _tmp5BB;struct Cyc_List_List*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BE;enum Cyc_Absyn_Incrementor _tmp5BF;struct Cyc_Absyn_Exp*_tmp5C1;struct Cyc_Core_Opt*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_List_List*_tmp5D3;struct Cyc_Absyn_VarargCallInfo**_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_List_List*_tmp5D9;void*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DC;enum Cyc_Absyn_Coercion*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Exp**_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E4;void*_tmp5E6;void*_tmp5E8;void*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;struct _dyneither_ptr*_tmp5EE;int*_tmp5EF;int*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F2;struct _dyneither_ptr*_tmp5F3;int*_tmp5F4;int*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F8;struct Cyc_List_List*_tmp5FA;struct _tuple9*_tmp5FC;struct Cyc_List_List*_tmp5FD;struct Cyc_List_List*_tmp5FF;struct Cyc_Absyn_Stmt*_tmp601;struct Cyc_Absyn_Vardecl*_tmp603;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_Absyn_Exp*_tmp605;int*_tmp606;struct _tuple1**_tmp608;struct Cyc_List_List**_tmp609;struct Cyc_List_List*_tmp60A;struct Cyc_Absyn_Aggrdecl**_tmp60B;void*_tmp60D;struct Cyc_List_List*_tmp60E;struct Cyc_List_List*_tmp610;struct Cyc_Absyn_Datatypedecl*_tmp611;struct Cyc_Absyn_Datatypefield*_tmp612;struct _tuple1**_tmp614;struct Cyc_Absyn_Enumdecl*_tmp615;struct Cyc_Absyn_Enumfield*_tmp616;struct _tuple1**_tmp618;void*_tmp619;struct Cyc_Absyn_Enumfield*_tmp61A;int*_tmp61C;struct Cyc_Absyn_Exp**_tmp61D;void***_tmp61E;struct Cyc_Absyn_Exp**_tmp61F;int*_tmp620;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Exp*_tmp623;struct Cyc_Absyn_Exp*_tmp625;struct _dyneither_ptr*_tmp626;void*_tmp628;_LL2D1: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5AB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5AB->tag != 11)goto _LL2D3;else{_tmp5AC=_tmp5AB->f1;}}_LL2D2:
# 2691
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5AC);
return;_LL2D3:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5AD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5AD->tag != 9)goto _LL2D5;else{_tmp5AE=_tmp5AD->f1;_tmp5AF=_tmp5AD->f2;_tmp5B0=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5AD->f3;_tmp5B1=(int*)& _tmp5AD->f4;}}if(!(!(*_tmp5B1)))goto _LL2D5;_LL2D4:
# 2697
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5AE,_tmp5AF)){
*_tmp5B1=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2701
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2704
return;_LL2D5: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5B2=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5B2->tag != 34)goto _LL2D7;else{_tmp5B3=_tmp5B2->f1;_tmp5B4=_tmp5B2->f2;}}_LL2D6:
# 2708
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5B4);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2D7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5B5->tag != 0)goto _LL2D9;else{_tmp5B6=(union Cyc_Absyn_Cnst*)& _tmp5B5->f1;}}_LL2D8:
# 2713
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5B6,e);goto _LL2D0;_LL2D9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5B7->tag != 1)goto _LL2DB;else{_tmp5B8=_tmp5B7->f1;_tmp5B9=(void**)((void**)& _tmp5B7->f2);}}_LL2DA:
# 2715
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5B8,_tmp5B9);goto _LL2D0;_LL2DB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5BA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5BA->tag != 2)goto _LL2DD;else{_tmp5BB=_tmp5BA->f1;_tmp5BC=_tmp5BA->f2;}}_LL2DC:
# 2717
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5BB,_tmp5BC);goto _LL2D0;_LL2DD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5BD=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5BD->tag != 4)goto _LL2DF;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;}}_LL2DE:
# 2719
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5BE,_tmp5BF);goto _LL2D0;_LL2DF: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5C0=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5C0->tag != 3)goto _LL2E1;else{_tmp5C1=_tmp5C0->f1;_tmp5C2=_tmp5C0->f2;_tmp5C3=_tmp5C0->f3;}}_LL2E0:
# 2721
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5C1,_tmp5C2,_tmp5C3);goto _LL2D0;_LL2E1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5C4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5C4->tag != 5)goto _LL2E3;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;_tmp5C7=_tmp5C4->f3;}}_LL2E2:
# 2723
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5C5,_tmp5C6,_tmp5C7);goto _LL2D0;_LL2E3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5C8->tag != 6)goto _LL2E5;else{_tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;}}_LL2E4:
# 2725
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5C9,_tmp5CA);goto _LL2D0;_LL2E5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5CB=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5CB->tag != 7)goto _LL2E7;else{_tmp5CC=_tmp5CB->f1;_tmp5CD=_tmp5CB->f2;}}_LL2E6:
# 2727
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5CC,_tmp5CD);goto _LL2D0;_LL2E7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5CE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5CE->tag != 8)goto _LL2E9;else{_tmp5CF=_tmp5CE->f1;_tmp5D0=_tmp5CE->f2;}}_LL2E8:
# 2729
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5CF,_tmp5D0);goto _LL2D0;_LL2E9: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5D1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5D1->tag != 9)goto _LL2EB;else{_tmp5D2=_tmp5D1->f1;_tmp5D3=_tmp5D1->f2;_tmp5D4=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5D1->f3;}}_LL2EA: {
# 2735
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp62A;_push_handler(& _tmp62A);{int _tmp62C=0;if(setjmp(_tmp62A.handler))_tmp62C=1;if(!_tmp62C){
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp62B=(void*)_exn_thrown;void*_tmp62E=_tmp62B;struct _dyneither_ptr _tmp630;void*_tmp631;_LL322: {struct Cyc_Core_Failure_exn_struct*_tmp62F=(struct Cyc_Core_Failure_exn_struct*)_tmp62E;if(_tmp62F->tag != Cyc_Core_Failure)goto _LL324;else{_tmp630=_tmp62F->f1;}}_LL323:
# 2742
 ok=0;
fn_exp=e;
goto _LL321;_LL324: _tmp631=_tmp62E;_LL325:(void)_throw(_tmp631);_LL321:;}};}
# 2746
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5D2,_tmp5D3,_tmp5D4,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2753
goto _LL2D0;}_LL2EB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5D5=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5D5->tag != 10)goto _LL2ED;else{_tmp5D6=_tmp5D5->f1;}}_LL2EC:
# 2755
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5D6);goto _LL2D0;_LL2ED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5D7=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5D7->tag != 12)goto _LL2EF;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;}}_LL2EE:
# 2757
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5D8,_tmp5D9);goto _LL2D0;_LL2EF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5DA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5DA->tag != 13)goto _LL2F1;else{_tmp5DB=(void*)_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;_tmp5DD=(enum Cyc_Absyn_Coercion*)& _tmp5DA->f4;}}_LL2F0:
# 2759
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5DB,_tmp5DC,_tmp5DD);goto _LL2D0;_LL2F1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5DE->tag != 14)goto _LL2F3;else{_tmp5DF=_tmp5DE->f1;}}_LL2F2:
# 2761
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp5DF);goto _LL2D0;_LL2F3: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5E0=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5E0->tag != 15)goto _LL2F5;else{_tmp5E1=(struct Cyc_Absyn_Exp**)& _tmp5E0->f1;_tmp5E2=_tmp5E0->f2;}}_LL2F4:
# 2763
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5E1,e,_tmp5E2);goto _LL2D0;_LL2F5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5E3->tag != 17)goto _LL2F7;else{_tmp5E4=_tmp5E3->f1;}}_LL2F6: {
# 2765
void*_tmp632=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5E4);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp632);goto _LL2D0;}_LL2F7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5E5=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5E5->tag != 16)goto _LL2F9;else{_tmp5E6=(void*)_tmp5E5->f1;}}_LL2F8:
# 2768
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5E6);goto _LL2D0;_LL2F9: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp5E7=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5E7->tag != 18)goto _LL2FB;else{_tmp5E8=(void*)_tmp5E7->f1;_tmp5E9=(void*)_tmp5E7->f2;}}_LL2FA:
# 2770
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5E8,_tmp5E9);goto _LL2D0;_LL2FB: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5EA->tag != 19)goto _LL2FD;else{_tmp5EB=_tmp5EA->f1;}}_LL2FC:
# 2772
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp5EB);goto _LL2D0;_LL2FD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5EC->tag != 20)goto _LL2FF;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;_tmp5EF=(int*)& _tmp5EC->f3;_tmp5F0=(int*)& _tmp5EC->f4;}}_LL2FE:
# 2774
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp5ED,_tmp5EE,_tmp5EF,_tmp5F0);goto _LL2D0;_LL2FF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp5F1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5F1->tag != 21)goto _LL301;else{_tmp5F2=_tmp5F1->f1;_tmp5F3=_tmp5F1->f2;_tmp5F4=(int*)& _tmp5F1->f3;_tmp5F5=(int*)& _tmp5F1->f4;}}_LL300:
# 2776
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp5F2,_tmp5F3,_tmp5F4,_tmp5F5);goto _LL2D0;_LL301: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp5F6=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5F6->tag != 22)goto _LL303;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;}}_LL302:
# 2778
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp5F7,_tmp5F8);goto _LL2D0;_LL303: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5F9=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5F9->tag != 23)goto _LL305;else{_tmp5FA=_tmp5F9->f1;}}_LL304:
# 2780
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5FA);goto _LL2D0;_LL305: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5FB->tag != 24)goto _LL307;else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;}}_LL306:
# 2782
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5FC,_tmp5FD);goto _LL2D0;_LL307: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5FE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp5FE->tag != 25)goto _LL309;else{_tmp5FF=_tmp5FE->f1;}}_LL308: {
# 2786
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp633=Cyc_Tcutil_compress(*topt);void**_tmp635;struct Cyc_Absyn_Tqual*_tmp636;union Cyc_Absyn_Constraint*_tmp637;_LL327: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp634=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp633;if(_tmp634->tag != 8)goto _LL329;else{_tmp635=(void**)&(_tmp634->f1).elt_type;_tmp636=(struct Cyc_Absyn_Tqual*)&(_tmp634->f1).tq;_tmp637=(_tmp634->f1).zero_term;}}_LL328:
# 2792
 elt_topt=_tmp635;
elt_tqopt=_tmp636;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp637);
goto _LL326;_LL329:;_LL32A:
 goto _LL326;_LL326:;}
# 2799
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp5FF);goto _LL2D0;}_LL309: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp600->tag != 35)goto _LL30B;else{_tmp601=_tmp600->f1;}}_LL30A:
# 2801
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp601);goto _LL2D0;_LL30B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp602->tag != 26)goto _LL30D;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;_tmp605=_tmp602->f3;_tmp606=(int*)& _tmp602->f4;}}_LL30C:
# 2803
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp603,_tmp604,_tmp605,_tmp606);goto _LL2D0;_LL30D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp607=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp607->tag != 27)goto _LL30F;else{_tmp608=(struct _tuple1**)& _tmp607->f1;_tmp609=(struct Cyc_List_List**)& _tmp607->f2;_tmp60A=_tmp607->f3;_tmp60B=(struct Cyc_Absyn_Aggrdecl**)& _tmp607->f4;}}_LL30E:
# 2805
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp608,_tmp609,_tmp60A,_tmp60B);goto _LL2D0;_LL30F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp60C->tag != 28)goto _LL311;else{_tmp60D=(void*)_tmp60C->f1;_tmp60E=_tmp60C->f2;}}_LL310:
# 2807
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp60D,_tmp60E);goto _LL2D0;_LL311: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp60F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp60F->tag != 29)goto _LL313;else{_tmp610=_tmp60F->f1;_tmp611=_tmp60F->f2;_tmp612=_tmp60F->f3;}}_LL312:
# 2809
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp610,_tmp611,_tmp612);goto _LL2D0;_LL313: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp613->tag != 30)goto _LL315;else{_tmp614=(struct _tuple1**)& _tmp613->f1;_tmp615=_tmp613->f2;_tmp616=_tmp613->f3;}}_LL314:
# 2811
*_tmp614=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp616))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9BD;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9BC;t=(void*)((_tmp9BC=_cycalloc(sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=13,((_tmp9BD.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp615))->name,((_tmp9BD.f2=_tmp615,_tmp9BD)))))),_tmp9BC))));}goto _LL2D0;_LL315: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp617->tag != 31)goto _LL317;else{_tmp618=(struct _tuple1**)& _tmp617->f1;_tmp619=(void*)_tmp617->f2;_tmp61A=_tmp617->f3;}}_LL316:
# 2814
*_tmp618=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp61A))->name;
t=_tmp619;goto _LL2D0;_LL317: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp61B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp61B->tag != 32)goto _LL319;else{_tmp61C=(int*)&(_tmp61B->f1).is_calloc;_tmp61D=(struct Cyc_Absyn_Exp**)&(_tmp61B->f1).rgn;_tmp61E=(void***)&(_tmp61B->f1).elt_type;_tmp61F=(struct Cyc_Absyn_Exp**)&(_tmp61B->f1).num_elts;_tmp620=(int*)&(_tmp61B->f1).fat_result;}}_LL318:
# 2817
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp61D,_tmp61E,_tmp61F,_tmp61C,_tmp620);goto _LL2D0;_LL319: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp621=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp621->tag != 33)goto _LL31B;else{_tmp622=_tmp621->f1;_tmp623=_tmp621->f2;}}_LL31A:
# 2819
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp622,_tmp623);goto _LL2D0;_LL31B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp624->tag != 36)goto _LL31D;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;}}_LL31C:
# 2821
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp625,_tmp626);goto _LL2D0;_LL31D: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp627=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp627->tag != 37)goto _LL31F;else{_tmp628=(void*)_tmp627->f1;}}_LL31E:
# 2823
 if(!te->allow_valueof){
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,Cyc_Tcutil_terr(e->loc,((_tmp9C0="valueof(-) can only occur within types",_tag_dyneither(_tmp9C0,sizeof(char),39))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}
# 2831
t=Cyc_Absyn_sint_typ;
goto _LL2D0;_LL31F: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp629=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5AA;if(_tmp629->tag != 38)goto _LL2D0;}_LL320:
# 2834
{const char*_tmp9C3;void*_tmp9C2;(_tmp9C2=0,Cyc_Tcutil_terr(e->loc,((_tmp9C3="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9C3,sizeof(char),50))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2D0:;}
# 2837
e->topt=t;}
