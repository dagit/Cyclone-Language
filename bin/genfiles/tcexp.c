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
# 253
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 264
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 121 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 133
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 137
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 163
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 169
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 182
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 188
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 196
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 200
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 204
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 211
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
# 77
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 79
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 85
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 93
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 96
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 122
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 146
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 148
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 175
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 184
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 220 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 230
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 232
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 234
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 238
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 246
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 248
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 258
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 263
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 271
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 285
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 312
int Cyc_Tcutil_bits_only(void*t);
# 315
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 341
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 344
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
{void*_tmp65B;(_tmp65B=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp65B,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp65C;return(_tmp65C=_cycalloc(sizeof(*_tmp65C)),((_tmp65C->f1=0,((_tmp65C->f2=e,_tmp65C)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp660;void*_tmp65F[1];struct Cyc_String_pa_PrintArg_struct _tmp65E;(_tmp65E.tag=0,((_tmp65E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp65F[0]=& _tmp65E,Cyc_Tcutil_terr(e1->loc,((_tmp660="undeclared identifier %s",_tag_dyneither(_tmp660,sizeof(char),25))),_tag_dyneither(_tmp65F,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp664;void*_tmp663[1];struct Cyc_String_pa_PrintArg_struct _tmp662;(_tmp662.tag=0,((_tmp662.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp663[0]=& _tmp662,Cyc_Tcutil_terr(e->loc,((_tmp664="%s is a constant, not a function",_tag_dyneither(_tmp664,sizeof(char),33))),_tag_dyneither(_tmp663,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp667;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp666;e->r=(void*)((_tmp666=_cycalloc(sizeof(*_tmp666)),((_tmp666[0]=((_tmp667.tag=29,((_tmp667.f1=es,((_tmp667.f2=_tmp10,((_tmp667.f3=_tmp11,_tmp667)))))))),_tmp666))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp66A;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp669;e->r=(void*)((_tmp669=_cycalloc(sizeof(*_tmp669)),((_tmp669[0]=((_tmp66A.tag=27,((_tmp66A.f1=_tmp13->name,((_tmp66A.f2=0,((_tmp66A.f3=_tmp22,((_tmp66A.f4=_tmp13,_tmp66A)))))))))),_tmp669))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp66E;void*_tmp66D[1];struct Cyc_String_pa_PrintArg_struct _tmp66C;(_tmp66C.tag=0,((_tmp66C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp66D[0]=& _tmp66C,Cyc_Tcutil_terr(e->loc,((_tmp66E="%s is an enum constructor, not a function",_tag_dyneither(_tmp66E,sizeof(char),42))),_tag_dyneither(_tmp66D,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp672;void*_tmp671[1];struct Cyc_String_pa_PrintArg_struct _tmp670;(_tmp670.tag=0,((_tmp670.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp671[0]=& _tmp670,Cyc_Tcutil_terr(e1->loc,((_tmp672="undeclared identifier %s",_tag_dyneither(_tmp672,sizeof(char),25))),_tag_dyneither(_tmp671,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp675;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp674;e->r=(void*)((_tmp674=_cycalloc(sizeof(*_tmp674)),((_tmp674[0]=((_tmp675.tag=25,((_tmp675.f1=des,_tmp675)))),_tmp674))));}
return;}{
# 125
void*t=*topt;
void*_tmp33=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp678;void*_tmp677;(_tmp677=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp678="struct type not properly set",_tag_dyneither(_tmp678,sizeof(char),29))),_tag_dyneither(_tmp677,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp67B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp67A;e->r=(void*)((_tmp67A=_cycalloc(sizeof(*_tmp67A)),((_tmp67A[0]=((_tmp67B.tag=27,((_tmp67B.f1=_tmp3B->name,((_tmp67B.f2=0,((_tmp67B.f3=des,((_tmp67B.f4=_tmp3B,_tmp67B)))))))))),_tmp67A))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp67E;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp67D;e->r=(void*)((_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=((_tmp67E.tag=25,((_tmp67E.f1=des,_tmp67E)))),_tmp67D))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp681;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp680;e->r=(void*)((_tmp680=_cycalloc(sizeof(*_tmp680)),((_tmp680[0]=((_tmp681.tag=28,((_tmp681.f1=t,((_tmp681.f2=des,_tmp681)))))),_tmp680))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp684;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp683;e->r=(void*)((_tmp683=_cycalloc(sizeof(*_tmp683)),((_tmp683[0]=((_tmp684.tag=25,((_tmp684.f1=des,_tmp684)))),_tmp683))));}goto _LL17;_LL17:;};}
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
const char*_tmp689;void*_tmp688[2];struct Cyc_String_pa_PrintArg_struct _tmp687;struct Cyc_String_pa_PrintArg_struct _tmp686;(_tmp686.tag=0,((_tmp686.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp687.tag=0,((_tmp687.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp688[0]=& _tmp687,((_tmp688[1]=& _tmp686,Cyc_Tcutil_terr(e->loc,((_tmp689="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp689,sizeof(char),53))),_tag_dyneither(_tmp688,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4A=e->r;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp68A;struct _tuple0 _tmp4F=(_tmp68A.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp68A.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp68A)));void*_tmp51;void*_tmp53;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f1;if(_tmp50->tag != 15)goto _LL2D;else{_tmp51=(void*)_tmp50->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f2;if(_tmp52->tag != 15)goto _LL2D;else{_tmp53=(void*)_tmp52->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp68D;struct Cyc_Tcexp_TestEnv _tmp68C;return(_tmp68C.eq=((_tmp68D=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp68D)),((_tmp68D->f1=_tmp51,((_tmp68D->f2=_tmp53,_tmp68D)))))),((_tmp68C.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp68C)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp68E;return(_tmp68E.eq=0,((_tmp68E.isTrue=0,_tmp68E)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp694;struct Cyc_Absyn_PtrInfo _tmp693;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp692;t=(void*)((_tmp692=_cycalloc(sizeof(*_tmp692)),((_tmp692[0]=((_tmp694.tag=5,((_tmp694.f1=((_tmp693.elt_typ=_tmp69,((_tmp693.elt_tq=_tmp6A,((_tmp693.ptr_atts=(
((_tmp693.ptr_atts).rgn=_tmp6B,(((_tmp693.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp693.ptr_atts).bounds=_tmp6D,(((_tmp693.ptr_atts).zero_term=_tmp6E,(((_tmp693.ptr_atts).ptrloc=0,_tmp693.ptr_atts)))))))))),_tmp693)))))),_tmp694)))),_tmp692))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp697;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp696;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp696=_cycalloc(sizeof(*_tmp696)),((_tmp696[0]=((_tmp697.tag=18,((_tmp697.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp697)))),_tmp696)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp69A;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp699;t=(void*)((_tmp699=_cycalloc(sizeof(*_tmp699)),((_tmp699[0]=((_tmp69A.tag=19,((_tmp69A.f1=(void*)_tmp76,_tmp69A)))),_tmp699))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp69D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp69C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C[0]=((_tmp69D.tag=1,((_tmp69D.f1=elen,_tmp69D)))),_tmp69C)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp7B,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp7C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual _tmp7E;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7C;if(_tmp7D->tag != 8)goto _LL5C;else{_tmp7E=(_tmp7D->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp7E,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7C;if(_tmp7F->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp7B,Cyc_Absyn_true_conref);
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
void*_tmp82=Cyc_Tcutil_compress(*topt);void*_tmp84;struct Cyc_Absyn_Tqual _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82;if(_tmp83->tag != 5)goto _LL63;else{_tmp84=(_tmp83->f1).elt_typ;_tmp85=(_tmp83->f1).elt_tq;_tmp86=((_tmp83->f1).ptr_atts).rgn;_tmp87=((_tmp83->f1).ptr_atts).nullable;_tmp88=((_tmp83->f1).ptr_atts).bounds;_tmp89=((_tmp83->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp87))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6A3;struct Cyc_Absyn_PtrInfo _tmp6A2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A1;return(void*)((_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A3.tag=5,((_tmp6A3.f1=((_tmp6A2.elt_typ=_tmp84,((_tmp6A2.elt_tq=_tmp85,((_tmp6A2.ptr_atts=(((_tmp6A2.ptr_atts).rgn=_tmp86,(((_tmp6A2.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6A2.ptr_atts).bounds=_tmp88,(((_tmp6A2.ptr_atts).zero_term=_tmp89,(((_tmp6A2.ptr_atts).ptrloc=0,_tmp6A2.ptr_atts)))))))))),_tmp6A2)))))),_tmp6A3)))),_tmp6A1))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6B2;struct Cyc_Core_Opt*_tmp6B1;struct Cyc_Core_Opt*_tmp6B0;struct Cyc_Absyn_PtrInfo _tmp6AF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AE;t=(void*)((_tmp6AE=_cycalloc(sizeof(*_tmp6AE)),((_tmp6AE[0]=((_tmp6B2.tag=5,((_tmp6B2.f1=((_tmp6AF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0->v=_tmp8D,_tmp6B0))))),((_tmp6AF.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6AF.ptr_atts=(
((_tmp6AF.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6B1=_cycalloc(sizeof(*_tmp6B1)),((_tmp6B1->v=_tmp8D,_tmp6B1))))),(((_tmp6AF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6AF.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6AF.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6AF.ptr_atts).ptrloc=0,_tmp6AF.ptr_atts)))))))))),_tmp6AF)))))),_tmp6B2)))),_tmp6AE))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp93=*((void**)_check_null(b));struct Cyc_Absyn_Vardecl*_tmp96;struct Cyc_Absyn_Fndecl*_tmp98;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Vardecl*_tmp9C;struct Cyc_Absyn_Vardecl*_tmp9E;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp94=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp93;if(_tmp94->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmp9F;_push_handler(& _tmp9F);{int _tmpA1=0;if(setjmp(_tmp9F.handler))_tmpA1=1;if(!_tmpA1){
{struct _RegionHandle _tmpA2=_new_region("r");struct _RegionHandle*r=& _tmpA2;_push_region(r);
{void*_tmpA3=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpA7;struct Cyc_Absyn_Enumdecl*_tmpA9;struct Cyc_Absyn_Enumfield*_tmpAA;void*_tmpAC;struct Cyc_Absyn_Enumfield*_tmpAD;struct Cyc_Absyn_Datatypedecl*_tmpAF;struct Cyc_Absyn_Datatypefield*_tmpB0;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpA4->f1);if(_tmpA5->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp6B6;void*_tmp6B5[1];struct Cyc_String_pa_PrintArg_struct _tmp6B4;void*_tmpB5=(_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6B5[0]=& _tmp6B4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6B6="undeclared identifier %s",_tag_dyneither(_tmp6B6,sizeof(char),25))),_tag_dyneither(_tmp6B5,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6B9;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6B8;e->r=(void*)((_tmp6B8=_cycalloc(sizeof(*_tmp6B8)),((_tmp6B8[0]=((_tmp6B9.tag=30,((_tmp6B9.f1=_tmpAA->name,((_tmp6B9.f2=_tmpA9,((_tmp6B9.f3=_tmpAA,_tmp6B9)))))))),_tmp6B8))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6BC;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6BB;void*_tmpBB=(void*)((_tmp6BB=_cycalloc(sizeof(*_tmp6BB)),((_tmp6BB[0]=((_tmp6BC.tag=13,((_tmp6BC.f1=_tmpA9->name,((_tmp6BC.f2=_tmpA9,_tmp6BC)))))),_tmp6BB))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6BF;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6BE;e->r=(void*)((_tmp6BE=_cycalloc(sizeof(*_tmp6BE)),((_tmp6BE[0]=((_tmp6BF.tag=31,((_tmp6BF.f1=_tmpAD->name,((_tmp6BF.f2=_tmpAC,((_tmp6BF.f3=_tmpAD,_tmp6BF)))))))),_tmp6BE))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6C2;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6C1;e->r=(void*)((_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1[0]=((_tmp6C2.tag=29,((_tmp6C2.f1=0,((_tmp6C2.f2=_tmpAF,((_tmp6C2.f3=_tmpB0,_tmp6C2)))))))),_tmp6C1))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6C6;void*_tmp6C5[1];struct Cyc_String_pa_PrintArg_struct _tmp6C4;void*_tmpC5=(_tmp6C4.tag=0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C5[0]=& _tmp6C4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C6="bad occurrence of type name %s",_tag_dyneither(_tmp6C6,sizeof(char),31))),_tag_dyneither(_tmp6C5,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6CA;void*_tmp6C9[1];struct Cyc_String_pa_PrintArg_struct _tmp6C8;return(_tmp6C8.tag=0,((_tmp6C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C9[0]=& _tmp6C8,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6CA="undeclared identifier %s",_tag_dyneither(_tmp6CA,sizeof(char),25))),_tag_dyneither(_tmp6C9,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6CB;(*q).f1=(((_tmp6CB.Loc_n).val=0,(((_tmp6CB.Loc_n).tag=4,_tmp6CB))));}
if(te->allow_valueof){
void*_tmpCE=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6CE;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6CD;e->r=(void*)((_tmp6CD=_cycalloc(sizeof(*_tmp6CD)),((_tmp6CD[0]=((_tmp6CE.tag=37,((_tmp6CE.f1=_tmpD0,_tmp6CE)))),_tmp6CD))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmp9E->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpD3=fmt->r;struct _dyneither_ptr _tmpD5;struct _dyneither_ptr _tmpD8;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD4->tag != 0)goto _LL8C;else{if(((_tmpD4->f1).String_c).tag != 8)goto _LL8C;_tmpD5=(struct _dyneither_ptr)((_tmpD4->f1).String_c).val;}}_LL8B:
 _tmpD8=_tmpD5;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD6->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmpD6->f2)->r;if(_tmpD7->tag != 0)goto _LL8E;else{if(((_tmpD7->f1).String_c).tag != 8)goto _LL8E;_tmpD8=(struct _dyneither_ptr)((_tmpD7->f1).String_c).val;}}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpD8,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpD9=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpD9 != 0;_tmpD9=_tmpD9->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpD9->hd);{
struct _RegionHandle _tmpDA=_new_region("temp");struct _RegionHandle*temp=& _tmpDA;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpD9->hd)){
const char*_tmp6D1;void*_tmp6D0;(_tmp6D0=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp6D1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6D1,sizeof(char),49))),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpDD=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpDD != 0;
(((desc_types=desc_types->tl,_tmpDD=_tmpDD->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpDD->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp6D6;void*_tmp6D5[2];struct Cyc_String_pa_PrintArg_struct _tmp6D4;struct Cyc_String_pa_PrintArg_struct _tmp6D3;(_tmp6D3.tag=0,((_tmp6D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6D4.tag=0,((_tmp6D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6D5[0]=& _tmp6D4,((_tmp6D5[1]=& _tmp6D3,Cyc_Tcutil_terr(e->loc,((_tmp6D6="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp6D6,sizeof(char),51))),_tag_dyneither(_tmp6D5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6D7;*alias_arg_exps=((_tmp6D7=_cycalloc(sizeof(*_tmp6D7)),((_tmp6D7->hd=(void*)arg_cnt,((_tmp6D7->tl=*alias_arg_exps,_tmp6D7))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6DA;void*_tmp6D9;(_tmp6D9=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6DA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6DA,sizeof(char),49))),_tag_dyneither(_tmp6D9,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6DD;void*_tmp6DC;(_tmp6DC=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6DD="too few arguments",_tag_dyneither(_tmp6DD,sizeof(char),18))),_tag_dyneither(_tmp6DC,sizeof(void*),0)));}
if(_tmpDD != 0){
const char*_tmp6E0;void*_tmp6DF;(_tmp6DF=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6E0="too many arguments",_tag_dyneither(_tmp6E0,sizeof(char),19))),_tag_dyneither(_tmp6DF,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6E4;void*_tmp6E3[1];struct Cyc_String_pa_PrintArg_struct _tmp6E2;(_tmp6E2.tag=0,((_tmp6E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E3[0]=& _tmp6E2,Cyc_Tcutil_terr(loc,((_tmp6E4="expecting arithmetic type but found %s",_tag_dyneither(_tmp6E4,sizeof(char),39))),_tag_dyneither(_tmp6E3,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6E8;void*_tmp6E7[1];struct Cyc_String_pa_PrintArg_struct _tmp6E6;(_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E7[0]=& _tmp6E6,Cyc_Tcutil_terr(loc,((_tmp6E8="expecting integral or * type but found %s",_tag_dyneither(_tmp6E8,sizeof(char),42))),_tag_dyneither(_tmp6E7,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6EC;void*_tmp6EB[1];struct Cyc_String_pa_PrintArg_struct _tmp6EA;(_tmp6EA.tag=0,((_tmp6EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6EB[0]=& _tmp6EA,Cyc_Tcutil_terr(loc,((_tmp6EC="expecting integral type but found %s",_tag_dyneither(_tmp6EC,sizeof(char),37))),_tag_dyneither(_tmp6EB,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF3=t;union Cyc_Absyn_Constraint*_tmpF6;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3;if(_tmpF4->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3;if(_tmpF5->tag != 5)goto _LL9B;else{_tmpF6=((_tmpF5->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_tmpF7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF6);struct Cyc_Absyn_Exp*_tmpFA;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpF8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF8->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF9->tag != 1)goto _LL9D;else{_tmpFA=_tmpF9->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFA)){
const char*_tmp6EF;void*_tmp6EE;(_tmp6EE=0,Cyc_Tcutil_terr(loc,((_tmp6EF="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp6EF,sizeof(char),55))),_tag_dyneither(_tmp6EE,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp6F3;void*_tmp6F2[1];struct Cyc_String_pa_PrintArg_struct _tmp6F1;(_tmp6F1.tag=0,((_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F2[0]=& _tmp6F1,Cyc_Tcutil_terr(loc,((_tmp6F3="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp6F3,sizeof(char),47))),_tag_dyneither(_tmp6F2,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp6F6;void*_tmp6F5;(_tmp6F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6F6="Non-unary primop",_tag_dyneither(_tmp6F6,sizeof(char),17))),_tag_dyneither(_tmp6F5,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp6FA;void*_tmp6F9[1];struct Cyc_String_pa_PrintArg_struct _tmp6F8;(_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp6F9[0]=& _tmp6F8,Cyc_Tcutil_terr(e1->loc,((_tmp6FA="type %s cannot be used here",_tag_dyneither(_tmp6FA,sizeof(char),28))),_tag_dyneither(_tmp6F9,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp6FE;void*_tmp6FD[1];struct Cyc_String_pa_PrintArg_struct _tmp6FC;(_tmp6FC.tag=0,((_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp6FD[0]=& _tmp6FC,Cyc_Tcutil_terr(e2->loc,((_tmp6FE="type %s cannot be used here",_tag_dyneither(_tmp6FE,sizeof(char),28))),_tag_dyneither(_tmp6FD,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 495
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 500
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp108=t1;void*_tmp10A;struct Cyc_Absyn_Tqual _tmp10B;void*_tmp10C;union Cyc_Absyn_Constraint*_tmp10D;union Cyc_Absyn_Constraint*_tmp10E;union Cyc_Absyn_Constraint*_tmp10F;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp109=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp108;if(_tmp109->tag != 5)goto _LLA6;else{_tmp10A=(_tmp109->f1).elt_typ;_tmp10B=(_tmp109->f1).elt_tq;_tmp10C=((_tmp109->f1).ptr_atts).rgn;_tmp10D=((_tmp109->f1).ptr_atts).nullable;_tmp10E=((_tmp109->f1).ptr_atts).bounds;_tmp10F=((_tmp109->f1).ptr_atts).zero_term;}}_LLA5:
# 505
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp10A),& Cyc_Tcutil_tmk)){
const char*_tmp701;void*_tmp700;(_tmp700=0,Cyc_Tcutil_terr(e1->loc,((_tmp701="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp701,sizeof(char),50))),_tag_dyneither(_tmp700,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp704;void*_tmp703;(_tmp703=0,Cyc_Tcutil_terr(e1->loc,((_tmp704="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp704,sizeof(char),54))),_tag_dyneither(_tmp703,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp708;void*_tmp707[1];struct Cyc_String_pa_PrintArg_struct _tmp706;(_tmp706.tag=0,((_tmp706.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp707[0]=& _tmp706,Cyc_Tcutil_terr(e2->loc,((_tmp708="expecting int but found %s",_tag_dyneither(_tmp708,sizeof(char),27))),_tag_dyneither(_tmp707,sizeof(void*),1)))))));}{
void*_tmp117=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp10E);struct Cyc_Absyn_Exp*_tmp11A;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp118=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp117;if(_tmp118->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp117;if(_tmp119->tag != 1)goto _LLA8;else{_tmp11A=_tmp119->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10F)){
const char*_tmp70B;void*_tmp70A;(_tmp70A=0,Cyc_Tcutil_warn(e1->loc,((_tmp70B="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp70B,sizeof(char),70))),_tag_dyneither(_tmp70A,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp711;struct Cyc_Absyn_PtrInfo _tmp710;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11D=(_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp711.tag=5,((_tmp711.f1=((_tmp710.elt_typ=_tmp10A,((_tmp710.elt_tq=_tmp10B,((_tmp710.ptr_atts=(
((_tmp710.ptr_atts).rgn=_tmp10C,(((_tmp710.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp710.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp710.ptr_atts).zero_term=_tmp10F,(((_tmp710.ptr_atts).ptrloc=0,_tmp710.ptr_atts)))))))))),_tmp710)))))),_tmp711)))),_tmp70F)));
# 528
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp11D,Cyc_Absyn_Other_coercion);
return(void*)_tmp11D;};_LLA8:;};_LLA6:;_LLA7:
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
{const char*_tmp716;void*_tmp715[2];struct Cyc_String_pa_PrintArg_struct _tmp714;struct Cyc_String_pa_PrintArg_struct _tmp713;(_tmp713.tag=0,((_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp714.tag=0,((_tmp714.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp715[0]=& _tmp714,((_tmp715[1]=& _tmp713,Cyc_Tcutil_terr(e1->loc,((_tmp716="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp716,sizeof(char),59))),_tag_dyneither(_tmp715,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp71B;void*_tmp71A[2];struct Cyc_String_pa_PrintArg_struct _tmp719;struct Cyc_String_pa_PrintArg_struct _tmp718;(_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp71A[0]=& _tmp719,((_tmp71A[1]=& _tmp718,Cyc_Tcutil_terr(e1->loc,((_tmp71B="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp71B,sizeof(char),59))),_tag_dyneither(_tmp71A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp71E;void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_warn(e1->loc,((_tmp71E="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp71E,sizeof(char),60))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp721;void*_tmp720;(_tmp720=0,Cyc_Tcutil_terr(e1->loc,((_tmp721="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp721,sizeof(char),50))),_tag_dyneither(_tmp720,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp724;void*_tmp723;(_tmp723=0,Cyc_Tcutil_terr(e1->loc,((_tmp724="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp724,sizeof(char),54))),_tag_dyneither(_tmp723,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp729;void*_tmp728[2];struct Cyc_String_pa_PrintArg_struct _tmp727;struct Cyc_String_pa_PrintArg_struct _tmp726;(_tmp726.tag=0,((_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp727.tag=0,((_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp728[0]=& _tmp727,((_tmp728[1]=& _tmp726,Cyc_Tcutil_terr(e2->loc,((_tmp729="expecting either %s or int but found %s",_tag_dyneither(_tmp729,sizeof(char),40))),_tag_dyneither(_tmp728,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp72C;void*_tmp72B;(_tmp72B=0,Cyc_Tcutil_warn(e1->loc,((_tmp72C="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp72C,sizeof(char),60))),_tag_dyneither(_tmp72B,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp72F;void*_tmp72E;(_tmp72E=0,Cyc_Tcutil_warn(e1->loc,((_tmp72F="thin pointer subtraction!",_tag_dyneither(_tmp72F,sizeof(char),26))),_tag_dyneither(_tmp72E,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp732;void*_tmp731;(_tmp731=0,Cyc_Tcutil_warn(e1->loc,((_tmp732="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp732,sizeof(char),56))),_tag_dyneither(_tmp731,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp735;void*_tmp734;(_tmp734=0,Cyc_Tcutil_warn(e1->loc,((_tmp735="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp735,sizeof(char),51))),_tag_dyneither(_tmp734,sizeof(void*),0)));}
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
struct _tuple0 _tmp736;struct _tuple0 _tmp13C=(_tmp736.f1=Cyc_Tcutil_compress(t1),((_tmp736.f2=Cyc_Tcutil_compress(t2),_tmp736)));void*_tmp13E;void*_tmp140;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C.f1;if(_tmp13D->tag != 5)goto _LLB0;else{_tmp13E=(_tmp13D->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C.f2;if(_tmp13F->tag != 5)goto _LLB0;else{_tmp140=(_tmp13F->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp13E,_tmp140))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13C.f1;if(_tmp141->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp142=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13C.f2;if(_tmp142->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp73B;void*_tmp73A[2];struct Cyc_String_pa_PrintArg_struct _tmp739;struct Cyc_String_pa_PrintArg_struct _tmp738;(_tmp738.tag=0,((_tmp738.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp739.tag=0,((_tmp739.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp73A[0]=& _tmp739,((_tmp73A[1]=& _tmp738,Cyc_Tcutil_terr(loc,((_tmp73B="comparison not allowed between %s and %s",_tag_dyneither(_tmp73B,sizeof(char),41))),_tag_dyneither(_tmp73A,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp73C;struct _tuple0 _tmp148=(_tmp73C.f1=t1,((_tmp73C.f2=t2,_tmp73C)));void*_tmp14A;void*_tmp14C;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp149=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp148.f1;if(_tmp149->tag != 15)goto _LLB7;else{_tmp14A=(void*)_tmp149->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp148.f2;if(_tmp14B->tag != 15)goto _LLB7;else{_tmp14C=(void*)_tmp14B->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp741;void*_tmp740[2];struct Cyc_String_pa_PrintArg_struct _tmp73F;struct Cyc_String_pa_PrintArg_struct _tmp73E;(_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp73F.tag=0,((_tmp73F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp740[0]=& _tmp73F,((_tmp740[1]=& _tmp73E,Cyc_Tcutil_terr(loc,((_tmp741="comparison not allowed between %s and %s",_tag_dyneither(_tmp741,sizeof(char),41))),_tag_dyneither(_tmp740,sizeof(void*),2)))))))))))));}
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
const char*_tmp744;void*_tmp743;(_tmp743=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp744="bad binary primop",_tag_dyneither(_tmp744,sizeof(char),18))),_tag_dyneither(_tmp743,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
const char*_tmp747;void*_tmp746;return(_tmp746=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp747="primitive operator has 0 arguments",_tag_dyneither(_tmp747,sizeof(char),35))),_tag_dyneither(_tmp746,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp74A;void*_tmp749;return(_tmp749=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp74A="primitive operator has > 2 arguments",_tag_dyneither(_tmp74A,sizeof(char),37))),_tag_dyneither(_tmp749,sizeof(void*),0)));}}
# 716
return t;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 719
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp157=Cyc_Tcutil_compress(t);
void*_tmp158=_tmp157;struct Cyc_Absyn_Aggrdecl*_tmp15A;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Datatypefield*_tmp15E;void*_tmp160;struct Cyc_Absyn_Tqual _tmp161;struct Cyc_List_List*_tmp163;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 11)goto _LLD4;else{if((((_tmp159->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp15A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp159->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 723
 _tmp15C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp15A->impl))->fields;goto _LLD5;_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp158;if(_tmp15B->tag != 12)goto _LLD6;else{_tmp15C=_tmp15B->f2;}}_LLD5:
# 725
 for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){
struct Cyc_Absyn_Aggrfield*_tmp164=(struct Cyc_Absyn_Aggrfield*)_tmp15C->hd;
if((_tmp164->tq).real_const){
{const char*_tmp74E;void*_tmp74D[1];struct Cyc_String_pa_PrintArg_struct _tmp74C;(_tmp74C.tag=0,((_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp164->name),((_tmp74D[0]=& _tmp74C,Cyc_Tcutil_terr(loc,((_tmp74E="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp74E,sizeof(char),56))),_tag_dyneither(_tmp74D,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp164->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp158;if(_tmp15D->tag != 4)goto _LLD8;else{if((((_tmp15D->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp15E=((struct _tuple2)(((_tmp15D->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp168=_tmp15E->typs;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;struct _tuple19*_tmp169=(struct _tuple19*)_tmp168->hd;_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;
if(_tmp16A.real_const){
{const char*_tmp752;void*_tmp751[1];struct Cyc_String_pa_PrintArg_struct _tmp750;(_tmp750.tag=0,((_tmp750.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name)),((_tmp751[0]=& _tmp750,Cyc_Tcutil_terr(loc,((_tmp752="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp752,sizeof(char),64))),_tag_dyneither(_tmp751,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16B))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp15F->tag != 8)goto _LLDA;else{_tmp160=(_tmp15F->f1).elt_type;_tmp161=(_tmp15F->f1).tq;}}_LLD9:
# 745
 if(_tmp161.real_const){
{const char*_tmp755;void*_tmp754;(_tmp754=0,Cyc_Tcutil_terr(loc,((_tmp755="attempt to over-write a const array",_tag_dyneither(_tmp755,sizeof(char),36))),_tag_dyneither(_tmp754,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp160);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp158;if(_tmp162->tag != 10)goto _LLDC;else{_tmp163=_tmp162->f1;}}_LLDB:
# 751
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct Cyc_Absyn_Tqual _tmp172;void*_tmp173;struct _tuple19*_tmp171=(struct _tuple19*)_tmp163->hd;_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;
if(_tmp172.real_const){
{const char*_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_terr(loc,((_tmp758="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp758,sizeof(char),56))),_tag_dyneither(_tmp757,sizeof(void*),0)));}
return 0;}
# 757
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp173))return 0;}
# 759
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 767
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 770
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp176=e->r;struct Cyc_Absyn_Vardecl*_tmp179;struct Cyc_Absyn_Vardecl*_tmp17C;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp177->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp178=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp177->f2);if(_tmp178->tag != 3)goto _LLE1;else{_tmp179=_tmp178->f1;}}}_LLE0:
 _tmp17C=_tmp179;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp17A->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17A->f2);if(_tmp17B->tag != 4)goto _LLE3;else{_tmp17C=_tmp17B->f1;}}}_LLE2:
 _tmp17F=_tmp17C;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp17D->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp17E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp17D->f2);if(_tmp17E->tag != 5)goto _LLE5;else{_tmp17F=_tmp17E->f1;}}}_LLE4:
 _tmp182=_tmp17F;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp180->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp181=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp180->f2);if(_tmp181->tag != 1)goto _LLE7;else{_tmp182=_tmp181->f1;}}}_LLE6:
 if(!(_tmp182->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp183->tag != 22)goto _LLE9;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;}}_LLE8:
# 777
{void*_tmp192=Cyc_Tcutil_compress((void*)_check_null(_tmp184->topt));struct Cyc_Absyn_Tqual _tmp194;struct Cyc_Absyn_Tqual _tmp196;struct Cyc_List_List*_tmp198;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp192;if(_tmp193->tag != 5)goto _LLF8;else{_tmp194=(_tmp193->f1).elt_tq;}}_LLF7:
 _tmp196=_tmp194;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp195=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192;if(_tmp195->tag != 8)goto _LLFA;else{_tmp196=(_tmp195->f1).tq;}}_LLF9:
 if(!_tmp196.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192;if(_tmp197->tag != 10)goto _LLFC;else{_tmp198=_tmp197->f1;}}_LLFB: {
# 781
unsigned int _tmp19A;int _tmp19B;struct _tuple15 _tmp199=Cyc_Evexp_eval_const_uint_exp(_tmp185);_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;
if(!_tmp19B){
{const char*_tmp75B;void*_tmp75A;(_tmp75A=0,Cyc_Tcutil_terr(e->loc,((_tmp75B="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp75B,sizeof(char),47))),_tag_dyneither(_tmp75A,sizeof(void*),0)));}
return;}
# 786
{struct _handler_cons _tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!_tmp1A0){
{struct Cyc_Absyn_Tqual _tmp1A2;struct _tuple19*_tmp1A1=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp198,(int)_tmp19A);_tmp1A2=_tmp1A1->f1;
if(!_tmp1A2.real_const){_npop_handler(0);return;}}
# 787
;_pop_handler();}else{void*_tmp19F=(void*)_exn_thrown;void*_tmp1A4=_tmp19F;void*_tmp1A6;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1A5=(struct Cyc_List_Nth_exn_struct*)_tmp1A4;if(_tmp1A5->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 789
 return;_LL101: _tmp1A6=_tmp1A4;_LL102:(void)_throw(_tmp1A6);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 793
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp186->tag != 20)goto _LLEB;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;}}_LLEA:
# 795
{void*_tmp1A7=Cyc_Tcutil_compress((void*)_check_null(_tmp187->topt));struct Cyc_Absyn_Aggrdecl**_tmp1A9;struct Cyc_List_List*_tmp1AB;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A7;if(_tmp1A8->tag != 11)goto _LL106;else{if((((_tmp1A8->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1A9=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1A8->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 797
struct Cyc_Absyn_Aggrfield*sf=
_tmp1A9 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1A9,_tmp188);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1AA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A7;if(_tmp1AA->tag != 12)goto _LL108;else{_tmp1AB=_tmp1AA->f2;}}_LL107: {
# 802
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1AB,_tmp188);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 807
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp189->tag != 21)goto _LLED;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LLEC:
# 809
{void*_tmp1AC=Cyc_Tcutil_compress((void*)_check_null(_tmp18A->topt));void*_tmp1AE;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC;if(_tmp1AD->tag != 5)goto _LL10D;else{_tmp1AE=(_tmp1AD->f1).elt_typ;}}_LL10C:
# 811
{void*_tmp1AF=Cyc_Tcutil_compress(_tmp1AE);struct Cyc_Absyn_Aggrdecl**_tmp1B1;struct Cyc_List_List*_tmp1B3;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AF;if(_tmp1B0->tag != 11)goto _LL112;else{if((((_tmp1B0->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1B1=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1B0->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 813
struct Cyc_Absyn_Aggrfield*sf=
_tmp1B1 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1B1,_tmp18B);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AF;if(_tmp1B2->tag != 12)goto _LL114;else{_tmp1B3=_tmp1B2->f2;}}_LL113: {
# 818
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1B3,_tmp18B);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 823
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 826
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp18C->tag != 19)goto _LLEF;else{_tmp18D=_tmp18C->f1;}}_LLEE:
# 828
{void*_tmp1B4=Cyc_Tcutil_compress((void*)_check_null(_tmp18D->topt));struct Cyc_Absyn_Tqual _tmp1B6;struct Cyc_Absyn_Tqual _tmp1B8;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B4;if(_tmp1B5->tag != 5)goto _LL119;else{_tmp1B6=(_tmp1B5->f1).elt_tq;}}_LL118:
 _tmp1B8=_tmp1B6;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4;if(_tmp1B7->tag != 8)goto _LL11B;else{_tmp1B8=(_tmp1B7->f1).tq;}}_LL11A:
 if(!_tmp1B8.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 833
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp18E->tag != 11)goto _LLF1;else{_tmp18F=_tmp18E->f1;}}_LLF0:
 _tmp191=_tmp18F;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp190->tag != 12)goto _LLF3;else{_tmp191=_tmp190->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp191);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 838
const char*_tmp75F;void*_tmp75E[1];struct Cyc_String_pa_PrintArg_struct _tmp75D;(_tmp75D.tag=0,((_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp75E[0]=& _tmp75D,Cyc_Tcutil_terr(e->loc,((_tmp75F="attempt to write a const location: %s",_tag_dyneither(_tmp75F,sizeof(char),38))),_tag_dyneither(_tmp75E,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1BC=_new_region("temp");struct _RegionHandle*temp=& _tmp1BC;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp762;void*_tmp761;void*_tmp1BF=(_tmp761=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp762="increment/decrement of non-lvalue",_tag_dyneither(_tmp762,sizeof(char),34))),_tag_dyneither(_tmp761,sizeof(void*),0)));_npop_handler(0);return _tmp1BF;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp765;void*_tmp764;(_tmp764=0,Cyc_Tcutil_terr(e->loc,((_tmp765="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp765,sizeof(char),50))),_tag_dyneither(_tmp764,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp768;void*_tmp767;(_tmp767=0,Cyc_Tcutil_terr(e->loc,((_tmp768="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp768,sizeof(char),54))),_tag_dyneither(_tmp767,sizeof(void*),0)));}}else{
# 861
const char*_tmp76C;void*_tmp76B[1];struct Cyc_String_pa_PrintArg_struct _tmp76A;(_tmp76A.tag=0,((_tmp76A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp76B[0]=& _tmp76A,Cyc_Tcutil_terr(e->loc,((_tmp76C="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp76C,sizeof(char),44))),_tag_dyneither(_tmp76B,sizeof(void*),1)))))));}}{
# 863
void*_tmp1C7=t;_npop_handler(0);return _tmp1C7;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _tuple0*_tmp1CA;int _tmp1CB;const char*_tmp76D;struct Cyc_Tcexp_TestEnv _tmp1C9=Cyc_Tcexp_tcTest(te,e1,((_tmp76D="conditional expression",_tag_dyneither(_tmp76D,sizeof(char),23))));_tmp1CA=_tmp1C9.eq;_tmp1CB=_tmp1C9.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_List_List _tmp76E;struct Cyc_List_List _tmp1CC=(_tmp76E.hd=e3,((_tmp76E.tl=0,_tmp76E)));
struct Cyc_List_List _tmp76F;struct Cyc_List_List _tmp1CD=(_tmp76F.hd=e2,((_tmp76F.tl=& _tmp1CC,_tmp76F)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1CD)){
{const char*_tmp774;void*_tmp773[2];struct Cyc_String_pa_PrintArg_struct _tmp772;struct Cyc_String_pa_PrintArg_struct _tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp772.tag=0,((_tmp772.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp773[0]=& _tmp772,((_tmp773[1]=& _tmp771,Cyc_Tcutil_terr(loc,((_tmp774="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp774,sizeof(char),48))),_tag_dyneither(_tmp773,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 880
return t;};}
# 884
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 886
{const char*_tmp775;Cyc_Tcexp_tcTest(te,e1,((_tmp775="logical-and expression",_tag_dyneither(_tmp775,sizeof(char),23))));}
{const char*_tmp776;Cyc_Tcexp_tcTest(te,e2,((_tmp776="logical-and expression",_tag_dyneither(_tmp776,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 892
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 894
{const char*_tmp777;Cyc_Tcexp_tcTest(te,e1,((_tmp777="logical-or expression",_tag_dyneither(_tmp777,sizeof(char),22))));}
{const char*_tmp778;Cyc_Tcexp_tcTest(te,e2,((_tmp778="logical-or expression",_tag_dyneither(_tmp778,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 900
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 907
struct _RegionHandle _tmp1D8=_new_region("r");struct _RegionHandle*r=& _tmp1D8;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 910
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 914
{void*_tmp1D9=Cyc_Tcutil_compress(t1);_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp77B;void*_tmp77A;(_tmp77A=0,Cyc_Tcutil_terr(loc,((_tmp77B="cannot assign to an array",_tag_dyneither(_tmp77B,sizeof(char),26))),_tag_dyneither(_tmp77A,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 919
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp77E;void*_tmp77D;(_tmp77D=0,Cyc_Tcutil_terr(loc,((_tmp77E="type is abstract (can't determine size).",_tag_dyneither(_tmp77E,sizeof(char),41))),_tag_dyneither(_tmp77D,sizeof(void*),0)));}
# 923
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp781;void*_tmp780;void*_tmp1E1=(_tmp780=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp781="assignment to non-lvalue",_tag_dyneither(_tmp781,sizeof(char),25))),_tag_dyneither(_tmp780,sizeof(void*),0)));_npop_handler(0);return _tmp1E1;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E2=_new_region("temp");struct _RegionHandle*temp=& _tmp1E2;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp784;void*_tmp783;(_tmp783=0,Cyc_Tcutil_terr(e2->loc,((_tmp784="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp784,sizeof(char),49))),_tag_dyneither(_tmp783,sizeof(void*),0)));}
# 931
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp789;void*_tmp788[2];struct Cyc_String_pa_PrintArg_struct _tmp787;struct Cyc_String_pa_PrintArg_struct _tmp786;void*_tmp1E5=(_tmp786.tag=0,((_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp787.tag=0,((_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp788[0]=& _tmp787,((_tmp788[1]=& _tmp786,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp789="type mismatch: %s != %s",_tag_dyneither(_tmp789,sizeof(char),24))),_tag_dyneither(_tmp788,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1E6=_tmp1E5;_npop_handler(1);return _tmp1E6;};}
# 927
;_pop_region(temp);}else{
# 939
enum Cyc_Absyn_Primop _tmp1EB=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1EC=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EB,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1EC,t1) || Cyc_Tcutil_coerceable(_tmp1EC) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp78E;void*_tmp78D[2];struct Cyc_String_pa_PrintArg_struct _tmp78C;struct Cyc_String_pa_PrintArg_struct _tmp78B;void*_tmp1ED=
(_tmp78B.tag=0,((_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 945
Cyc_Absynpp_typ2string(t2)),((_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 944
Cyc_Absynpp_typ2string(t1)),((_tmp78D[0]=& _tmp78C,((_tmp78D[1]=& _tmp78B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78E="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp78E,sizeof(char),82))),_tag_dyneither(_tmp78D,sizeof(void*),2)))))))))))));
# 946
Cyc_Tcutil_unify(_tmp1EC,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1EE=_tmp1ED;_npop_handler(0);return _tmp1EE;};}{
# 950
void*_tmp1F3=_tmp1EC;_npop_handler(0);return _tmp1F3;};}{
# 952
void*_tmp1F4=t1;_npop_handler(0);return _tmp1F4;};};};
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
{void*_tmp1F5=Cyc_Tcutil_compress(t1);_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 7)goto _LL125;else{if(_tmp1F6->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F7->tag != 6)goto _LL127;else{if(_tmp1F7->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F8->tag != 6)goto _LL129;else{if(_tmp1F8->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 977
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp1FA;struct Cyc_List_List*_tmp1FB;unsigned int _tmp1FC;enum Cyc_Absyn_Scope _tmp1FD;struct Cyc_Absyn_Datatypefield _tmp1F9=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1FA=_tmp1F9.name;_tmp1FB=_tmp1F9.typs;_tmp1FC=_tmp1F9.loc;_tmp1FD=_tmp1F9.sc;
# 980
if(_tmp1FB == 0  || _tmp1FB->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp1FB->hd)).f2);
# 983
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 988
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp1FF;struct Cyc_List_List*_tmp200;unsigned int _tmp201;enum Cyc_Absyn_Scope _tmp202;struct Cyc_Absyn_Datatypefield _tmp1FE=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1FF=_tmp1FE.name;_tmp200=_tmp1FE.typs;_tmp201=_tmp1FE.loc;_tmp202=_tmp1FE.sc;
# 991
if(_tmp200 == 0  || _tmp200->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp200->hd)).f2);
# 995
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1000
{const char*_tmp793;void*_tmp792[2];struct Cyc_String_pa_PrintArg_struct _tmp791;struct Cyc_String_pa_PrintArg_struct _tmp790;(_tmp790.tag=0,((_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp791.tag=0,((_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp792[0]=& _tmp791,((_tmp792[1]=& _tmp790,Cyc_Tcutil_terr(e->loc,((_tmp793="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp793,sizeof(char),54))),_tag_dyneither(_tmp792,sizeof(void*),2)))))))))))));}
return 0;}
# 1006
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1012
struct Cyc_List_List*_tmp207=args;
int _tmp208=0;
struct _RegionHandle _tmp209=_new_region("ter");struct _RegionHandle*ter=& _tmp209;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp20A=Cyc_Tcenv_new_block(ter,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp20A,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1021
void*_tmp20B=t;void*_tmp20D;struct Cyc_Absyn_Tqual _tmp20E;void*_tmp20F;union Cyc_Absyn_Constraint*_tmp210;union Cyc_Absyn_Constraint*_tmp211;union Cyc_Absyn_Constraint*_tmp212;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20B;if(_tmp20C->tag != 5)goto _LL12E;else{_tmp20D=(_tmp20C->f1).elt_typ;_tmp20E=(_tmp20C->f1).elt_tq;_tmp20F=((_tmp20C->f1).ptr_atts).rgn;_tmp210=((_tmp20C->f1).ptr_atts).nullable;_tmp211=((_tmp20C->f1).ptr_atts).bounds;_tmp212=((_tmp20C->f1).ptr_atts).zero_term;}}_LL12D:
# 1026
 Cyc_Tcenv_check_rgn_accessible(_tmp20A,loc,_tmp20F);
# 1028
Cyc_Tcutil_check_nonzero_bound(loc,_tmp211);{
void*_tmp213=Cyc_Tcutil_compress(_tmp20D);struct Cyc_List_List*_tmp215;void*_tmp216;struct Cyc_Absyn_Tqual _tmp217;void*_tmp218;struct Cyc_List_List*_tmp219;int _tmp21A;struct Cyc_Absyn_VarargInfo*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21D;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 9)goto _LL133;else{_tmp215=(_tmp214->f1).tvars;_tmp216=(_tmp214->f1).effect;_tmp217=(_tmp214->f1).ret_tqual;_tmp218=(_tmp214->f1).ret_typ;_tmp219=(_tmp214->f1).args;_tmp21A=(_tmp214->f1).c_varargs;_tmp21B=(_tmp214->f1).cyc_varargs;_tmp21C=(_tmp214->f1).rgn_po;_tmp21D=(_tmp214->f1).attributes;}}_LL132:
# 1032
 if(_tmp215 != 0  || _tmp21C != 0){
const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_terr(e->loc,((_tmp796="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp796,sizeof(char),79))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
# 1036
if(topt != 0)Cyc_Tcutil_unify(_tmp218,*topt);
# 1038
while(_tmp207 != 0  && _tmp219 != 0){
# 1040
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
void*t2=(*((struct _tuple9*)_tmp219->hd)).f3;
Cyc_Tcexp_tcExp(_tmp20A,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20A,e1,t2,& alias_coercion)){
{const char*_tmp79B;void*_tmp79A[2];struct Cyc_String_pa_PrintArg_struct _tmp799;struct Cyc_String_pa_PrintArg_struct _tmp798;(_tmp798.tag=0,((_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1048
Cyc_Absynpp_typ2string(t2)),((_tmp799.tag=0,((_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp79A[0]=& _tmp799,((_tmp79A[1]=& _tmp798,Cyc_Tcutil_terr(e1->loc,((_tmp79B="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp79B,sizeof(char),57))),_tag_dyneither(_tmp79A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1053
if(alias_coercion){
struct Cyc_List_List*_tmp79C;*alias_arg_exps=((_tmp79C=_cycalloc(sizeof(*_tmp79C)),((_tmp79C->hd=(void*)_tmp208,((_tmp79C->tl=*alias_arg_exps,_tmp79C))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_Tcutil_terr(e1->loc,((_tmp79F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp79F,sizeof(char),49))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}
_tmp207=_tmp207->tl;
_tmp219=_tmp219->tl;
++ _tmp208;}{
# 1064
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp21D;for(0;a != 0;a=a->tl){
void*_tmp227=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp229;int _tmp22A;int _tmp22B;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp228=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp227;if(_tmp228->tag != 19)goto _LL138;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;_tmp22B=_tmp228->f3;}}_LL137:
# 1068
{struct _handler_cons _tmp22C;_push_handler(& _tmp22C);{int _tmp22E=0;if(setjmp(_tmp22C.handler))_tmp22E=1;if(!_tmp22E){
# 1070
{struct Cyc_Absyn_Exp*_tmp22F=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp22A - 1);
# 1072
struct Cyc_Core_Opt*fmt_args;
if(_tmp22B == 0)
fmt_args=0;else{
# 1076
struct Cyc_Core_Opt*_tmp7A0;fmt_args=((_tmp7A0=_cycalloc(sizeof(*_tmp7A0)),((_tmp7A0->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp22B - 1),_tmp7A0))));}
args_already_checked=1;{
struct _RegionHandle _tmp231=_new_region("temp");struct _RegionHandle*temp=& _tmp231;_push_region(temp);
switch(_tmp229){case Cyc_Absyn_Printf_ft: _LL13A:
# 1081
 Cyc_Tcexp_check_format_args(_tmp20A,_tmp22F,fmt_args,_tmp22B - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1084
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1086
 Cyc_Tcexp_check_format_args(_tmp20A,_tmp22F,fmt_args,_tmp22B - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1089
break;}
# 1079
;_pop_region(temp);};}
# 1070
;_pop_handler();}else{void*_tmp22D=(void*)_exn_thrown;void*_tmp233=_tmp22D;void*_tmp235;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp234=(struct Cyc_List_Nth_exn_struct*)_tmp233;if(_tmp234->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1093
{const char*_tmp7A3;void*_tmp7A2;(_tmp7A2=0,Cyc_Tcutil_terr(loc,((_tmp7A3="bad format arguments",_tag_dyneither(_tmp7A3,sizeof(char),21))),_tag_dyneither(_tmp7A2,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp235=_tmp233;_LL141:(void)_throw(_tmp235);_LL13D:;}};}
# 1095
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1099
if(_tmp219 != 0){const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_Tcutil_terr(loc,((_tmp7A6="too few arguments for function",_tag_dyneither(_tmp7A6,sizeof(char),31))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}else{
# 1101
if((_tmp207 != 0  || _tmp21A) || _tmp21B != 0){
if(_tmp21A)
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
Cyc_Tcexp_tcExp(_tmp20A,0,(struct Cyc_Absyn_Exp*)_tmp207->hd);}else{
if(_tmp21B == 0){
const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_Tcutil_terr(loc,((_tmp7A9="too many arguments for function",_tag_dyneither(_tmp7A9,sizeof(char),32))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}else{
# 1108
void*_tmp23D;int _tmp23E;struct Cyc_Absyn_VarargInfo _tmp23C=*_tmp21B;_tmp23D=_tmp23C.type;_tmp23E=_tmp23C.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7AA;struct Cyc_Absyn_VarargCallInfo*_tmp23F=(_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA->num_varargs=0,((_tmp7AA->injectors=0,((_tmp7AA->vai=_tmp21B,_tmp7AA)))))));
# 1112
*vararg_call_info=_tmp23F;
# 1114
if(!_tmp23E)
# 1116
for(0;_tmp207 != 0;(_tmp207=_tmp207->tl,_tmp208 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
++ _tmp23F->num_varargs;
Cyc_Tcexp_tcExp(_tmp20A,& _tmp23D,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20A,e1,_tmp23D,& alias_coercion)){
{const char*_tmp7AF;void*_tmp7AE[2];struct Cyc_String_pa_PrintArg_struct _tmp7AD;struct Cyc_String_pa_PrintArg_struct _tmp7AC;(_tmp7AC.tag=0,((_tmp7AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7AD.tag=0,((_tmp7AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23D)),((_tmp7AE[0]=& _tmp7AD,((_tmp7AE[1]=& _tmp7AC,Cyc_Tcutil_terr(loc,((_tmp7AF="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7AF,sizeof(char),49))),_tag_dyneither(_tmp7AE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1126
if(alias_coercion){
struct Cyc_List_List*_tmp7B0;*alias_arg_exps=((_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0->hd=(void*)_tmp208,((_tmp7B0->tl=*alias_arg_exps,_tmp7B0))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23D) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7B3;void*_tmp7B2;(_tmp7B2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7B3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7B3,sizeof(char),49))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}}else{
# 1135
void*_tmp247=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23D));struct Cyc_Absyn_Datatypedecl*_tmp249;struct Cyc_List_List*_tmp24A;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp247;if(_tmp248->tag != 3)goto _LL145;else{if((((_tmp248->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp249=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp248->f1).datatype_info).KnownDatatype).val);_tmp24A=(_tmp248->f1).targs;}}_LL144: {
# 1139
struct Cyc_Absyn_Datatypedecl*_tmp24B=*Cyc_Tcenv_lookup_datatypedecl(_tmp20A,loc,_tmp249->name);
struct Cyc_List_List*fields=0;
if(_tmp24B->fields == 0){
const char*_tmp7B7;void*_tmp7B6[1];struct Cyc_String_pa_PrintArg_struct _tmp7B5;(_tmp7B5.tag=0,((_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23D)),((_tmp7B6[0]=& _tmp7B5,Cyc_Tcutil_terr(loc,((_tmp7B7="can't inject into abstract datatype %s",_tag_dyneither(_tmp7B7,sizeof(char),39))),_tag_dyneither(_tmp7B6,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24B->fields))->v;}
# 1151
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23D),Cyc_Tcenv_curr_rgn(_tmp20A))){
const char*_tmp7BC;void*_tmp7BB[2];struct Cyc_String_pa_PrintArg_struct _tmp7BA;struct Cyc_String_pa_PrintArg_struct _tmp7B9;(_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20A))),((_tmp7BA.tag=0,((_tmp7BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23D)),((_tmp7BB[0]=& _tmp7BA,((_tmp7BB[1]=& _tmp7B9,Cyc_Tcutil_terr(loc,((_tmp7BC="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7BC,sizeof(char),49))),_tag_dyneither(_tmp7BB,sizeof(void*),2)))))))))))));}{
# 1155
struct _RegionHandle _tmp253=_new_region("rgn");struct _RegionHandle*rgn=& _tmp253;_push_region(rgn);{
struct Cyc_List_List*_tmp254=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp24B->tvs,_tmp24A);
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
++ _tmp23F->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
# 1161
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp20A,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp7BF;void*_tmp7BE;(_tmp7BE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7BF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7BF,sizeof(char),49))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}}{
# 1167
struct Cyc_Absyn_Datatypefield*_tmp257=Cyc_Tcexp_tcInjection(_tmp20A,e1,Cyc_Tcutil_pointer_elt_type(_tmp23D),rgn,_tmp254,fields);
# 1169
if(_tmp257 != 0){
struct Cyc_List_List*_tmp7C0;_tmp23F->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp23F->injectors,(
(_tmp7C0=_cycalloc(sizeof(*_tmp7C0)),((_tmp7C0->hd=_tmp257,((_tmp7C0->tl=0,_tmp7C0)))))));}};};}}
# 1175
_npop_handler(0);goto _LL142;
# 1155
;_pop_region(rgn);};}_LL145:;_LL146:
# 1176
{const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_Tcutil_terr(loc,((_tmp7C3="bad inject vararg type",_tag_dyneither(_tmp7C3,sizeof(char),23))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1181
if(*alias_arg_exps == 0)
# 1190 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp20A,loc,(void*)_check_null(_tmp216));{
# 1192
void*_tmp25C=_tmp218;_npop_handler(0);return _tmp25C;};};_LL133:;_LL134: {
const char*_tmp7C6;void*_tmp7C5;void*_tmp25F=(_tmp7C5=0,Cyc_Tcexp_expr_err(_tmp20A,loc,topt,((_tmp7C6="expected pointer to function",_tag_dyneither(_tmp7C6,sizeof(char),29))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));_npop_handler(0);return _tmp25F;}_LL130:;};_LL12E:;_LL12F: {
# 1195
const char*_tmp7C9;void*_tmp7C8;void*_tmp262=(_tmp7C8=0,Cyc_Tcexp_expr_err(_tmp20A,loc,topt,((_tmp7C9="expected pointer to function",_tag_dyneither(_tmp7C9,sizeof(char),29))),_tag_dyneither(_tmp7C8,sizeof(void*),0)));_npop_handler(0);return _tmp262;}_LL12B:;};}
# 1015 "tcexp.cyc"
;_pop_region(ter);}
# 1201 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp263=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp263,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp263,& bogus)){
const char*_tmp7CE;void*_tmp7CD[2];struct Cyc_String_pa_PrintArg_struct _tmp7CC;struct Cyc_String_pa_PrintArg_struct _tmp7CB;(_tmp7CB.tag=0,((_tmp7CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7CC.tag=0,((_tmp7CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1206
Cyc_Absynpp_typ2string(_tmp263)),((_tmp7CD[0]=& _tmp7CC,((_tmp7CD[1]=& _tmp7CB,Cyc_Tcutil_terr(loc,((_tmp7CE="expected %s but found %s",_tag_dyneither(_tmp7CE,sizeof(char),25))),_tag_dyneither(_tmp7CD,sizeof(void*),2)))))))))))));}
# 1208
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1213
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1215
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1217
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp268=t1;void*_tmp26A;struct Cyc_Absyn_Tqual _tmp26B;void*_tmp26C;union Cyc_Absyn_Constraint*_tmp26D;union Cyc_Absyn_Constraint*_tmp26E;union Cyc_Absyn_Constraint*_tmp26F;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp268;if(_tmp269->tag != 5)goto _LL14A;else{_tmp26A=(_tmp269->f1).elt_typ;_tmp26B=(_tmp269->f1).elt_tq;_tmp26C=((_tmp269->f1).ptr_atts).rgn;_tmp26D=((_tmp269->f1).ptr_atts).nullable;_tmp26E=((_tmp269->f1).ptr_atts).bounds;_tmp26F=((_tmp269->f1).ptr_atts).zero_term;}}_LL149:
# 1221
{void*_tmp270=Cyc_Tcutil_compress(_tmp26A);struct Cyc_List_List*_tmp272;void*_tmp273;struct Cyc_Absyn_Tqual _tmp274;void*_tmp275;struct Cyc_List_List*_tmp276;int _tmp277;struct Cyc_Absyn_VarargInfo*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp270;if(_tmp271->tag != 9)goto _LL14F;else{_tmp272=(_tmp271->f1).tvars;_tmp273=(_tmp271->f1).effect;_tmp274=(_tmp271->f1).ret_tqual;_tmp275=(_tmp271->f1).ret_typ;_tmp276=(_tmp271->f1).args;_tmp277=(_tmp271->f1).c_varargs;_tmp278=(_tmp271->f1).cyc_varargs;_tmp279=(_tmp271->f1).rgn_po;_tmp27A=(_tmp271->f1).attributes;}}_LL14E: {
# 1223
struct _RegionHandle _tmp27B=_new_region("temp");struct _RegionHandle*temp=& _tmp27B;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1226
for(0;ts != 0  && _tmp272 != 0;(ts=ts->tl,_tmp272=_tmp272->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp272->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp7D1;struct Cyc_List_List*_tmp7D0;instantiation=(
(_tmp7D0=_region_malloc(temp,sizeof(*_tmp7D0)),((_tmp7D0->hd=((_tmp7D1=_region_malloc(temp,sizeof(*_tmp7D1)),((_tmp7D1->f1=(struct Cyc_Absyn_Tvar*)_tmp272->hd,((_tmp7D1->f2=(void*)ts->hd,_tmp7D1)))))),((_tmp7D0->tl=instantiation,_tmp7D0))))));};}
# 1233
if(ts != 0){
const char*_tmp7D4;void*_tmp7D3;void*_tmp280=
(_tmp7D3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D4="too many type variables in instantiation",_tag_dyneither(_tmp7D4,sizeof(char),41))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));_npop_handler(0);return _tmp280;}
# 1239
if(_tmp272 == 0){
_tmp279=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp279);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp279);
_tmp279=0;}{
# 1244
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7DA;struct Cyc_Absyn_FnInfo _tmp7D9;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7D8;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp7D8=_cycalloc(sizeof(*_tmp7D8)),((_tmp7D8[0]=((_tmp7DA.tag=9,((_tmp7DA.f1=((_tmp7D9.tvars=_tmp272,((_tmp7D9.effect=_tmp273,((_tmp7D9.ret_tqual=_tmp274,((_tmp7D9.ret_typ=_tmp275,((_tmp7D9.args=_tmp276,((_tmp7D9.c_varargs=_tmp277,((_tmp7D9.cyc_varargs=_tmp278,((_tmp7D9.rgn_po=_tmp279,((_tmp7D9.attributes=_tmp27A,_tmp7D9)))))))))))))))))),_tmp7DA)))),_tmp7D8)))));
# 1248
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7E0;struct Cyc_Absyn_PtrInfo _tmp7DF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7DE;void*_tmp284=(void*)((_tmp7DE=_cycalloc(sizeof(*_tmp7DE)),((_tmp7DE[0]=((_tmp7E0.tag=5,((_tmp7E0.f1=((_tmp7DF.elt_typ=new_fn_typ,((_tmp7DF.elt_tq=_tmp26B,((_tmp7DF.ptr_atts=(((_tmp7DF.ptr_atts).rgn=_tmp26C,(((_tmp7DF.ptr_atts).nullable=_tmp26D,(((_tmp7DF.ptr_atts).bounds=_tmp26E,(((_tmp7DF.ptr_atts).zero_term=_tmp26F,(((_tmp7DF.ptr_atts).ptrloc=0,_tmp7DF.ptr_atts)))))))))),_tmp7DF)))))),_tmp7E0)))),_tmp7DE))));_npop_handler(0);return _tmp284;};}
# 1224
;_pop_region(temp);}_LL14F:;_LL150:
# 1250
 goto _LL14C;_LL14C:;}
# 1252
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1255
const char*_tmp7E4;void*_tmp7E3[1];struct Cyc_String_pa_PrintArg_struct _tmp7E2;return(_tmp7E2.tag=0,((_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7E3[0]=& _tmp7E2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E4="expecting polymorphic type but found %s",_tag_dyneither(_tmp7E4,sizeof(char),40))),_tag_dyneither(_tmp7E3,sizeof(void*),1)))))));};};}
# 1260
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1262
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1265
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1270
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1277
Cyc_Tcutil_unify(t2,t);{
const char*_tmp7E9;void*_tmp7E8[2];struct Cyc_String_pa_PrintArg_struct _tmp7E7;struct Cyc_String_pa_PrintArg_struct _tmp7E6;void*_tmp28B=(_tmp7E6.tag=0,((_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7E7.tag=0,((_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7E8[0]=& _tmp7E7,((_tmp7E8[1]=& _tmp7E6,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp7E9="cannot cast %s to %s",_tag_dyneither(_tmp7E9,sizeof(char),21))),_tag_dyneither(_tmp7E8,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp28B;};}}}
# 1287
{struct _tuple0 _tmp7EA;struct _tuple0 _tmp291=(_tmp7EA.f1=e->r,((_tmp7EA.f2=Cyc_Tcutil_compress(t),_tmp7EA)));int _tmp293;union Cyc_Absyn_Constraint*_tmp295;union Cyc_Absyn_Constraint*_tmp296;union Cyc_Absyn_Constraint*_tmp297;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp291.f1;if(_tmp292->tag != 32)goto _LL154;else{_tmp293=(_tmp292->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp294=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291.f2;if(_tmp294->tag != 5)goto _LL154;else{_tmp295=((_tmp294->f1).ptr_atts).nullable;_tmp296=((_tmp294->f1).ptr_atts).bounds;_tmp297=((_tmp294->f1).ptr_atts).zero_term;}};_LL153:
# 1291
 if((_tmp293  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp297)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp295)){
void*_tmp298=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp296);struct Cyc_Absyn_Exp*_tmp29A;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp299=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp298;if(_tmp299->tag != 1)goto _LL159;else{_tmp29A=_tmp299->f1;}}_LL158:
# 1294
 if((Cyc_Evexp_eval_const_uint_exp(_tmp29A)).f1 == 1){
const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_Tcutil_warn(loc,((_tmp7ED="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp7ED,sizeof(char),60))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1300
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1303
return t;};}
# 1307
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1309
void**_tmp29D=0;
struct Cyc_Absyn_Tqual _tmp29E=Cyc_Absyn_empty_tqual(0);
int _tmp29F=0;
if(topt != 0){
void*_tmp2A0=Cyc_Tcutil_compress(*topt);void**_tmp2A2;struct Cyc_Absyn_Tqual _tmp2A3;union Cyc_Absyn_Constraint*_tmp2A4;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A0;if(_tmp2A1->tag != 5)goto _LL15E;else{_tmp2A2=(void**)&(_tmp2A1->f1).elt_typ;_tmp2A3=(_tmp2A1->f1).elt_tq;_tmp2A4=((_tmp2A1->f1).ptr_atts).nullable;}}_LL15D:
# 1315
 _tmp29D=_tmp2A2;
_tmp29E=_tmp2A3;
_tmp29F=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2A4);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1329
struct _RegionHandle _tmp2A5=_new_region("r");struct _RegionHandle*r=& _tmp2A5;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp29D,e);
# 1332
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,Cyc_Tcutil_terr(e->loc,((_tmp7F0="Cannot take the address of an alias-free path",_tag_dyneither(_tmp7F0,sizeof(char),46))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}
# 1339
{void*_tmp2A8=e->r;struct Cyc_Absyn_Exp*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2A9=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A8;if(_tmp2A9->tag != 22)goto _LL163;else{_tmp2AA=_tmp2A9->f1;_tmp2AB=_tmp2A9->f2;}}_LL162:
# 1341
{void*_tmp2AC=Cyc_Tcutil_compress((void*)_check_null(_tmp2AA->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2AD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AC;if(_tmp2AD->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1349
 e0->r=(Cyc_Absyn_add_exp(_tmp2AA,_tmp2AB,0))->r;{
void*_tmp2AE=Cyc_Tcexp_tcPlus(te,_tmp2AA,_tmp2AB);_npop_handler(0);return _tmp2AE;};_LL165:;}
# 1352
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1357
{void*_tmp2AF=e->r;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2B0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2AF;if(_tmp2B0->tag != 20)goto _LL16D;else{if(_tmp2B0->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2AF;if(_tmp2B1->tag != 21)goto _LL16F;else{if(_tmp2B1->f3 != 1)goto _LL16F;}}_LL16E:
# 1360
{const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_Tcutil_terr(e->loc,((_tmp7F3="cannot take the address of a @tagged union member",_tag_dyneither(_tmp7F3,sizeof(char),50))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1366
int _tmp2B5;void*_tmp2B6;struct _tuple14 _tmp2B4=Cyc_Tcutil_addressof_props(te,e);_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B6,0)){
const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,Cyc_Tcutil_terr(e->loc,((_tmp7F6="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp7F6,sizeof(char),60))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}{
# 1370
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2B5){
tq.print_const=1;
tq.real_const=1;}{
# 1376
void*t=_tmp29F?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2B6,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2B6,tq,Cyc_Absyn_false_conref);
void*_tmp2B9=t;_npop_handler(0);return _tmp2B9;};};};
# 1329
;_pop_region(r);};}
# 1383
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1387
return Cyc_Absyn_uint_typ;
# 1389
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp7FA;void*_tmp7F9[1];struct Cyc_String_pa_PrintArg_struct _tmp7F8;(_tmp7F8.tag=0,((_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7F9[0]=& _tmp7F8,Cyc_Tcutil_terr(loc,((_tmp7FA="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp7FA,sizeof(char),55))),_tag_dyneither(_tmp7F9,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2BD=Cyc_Tcutil_compress(*topt);void*_tmp2BE;void*_tmp2C0;_LL172: _tmp2BE=_tmp2BD;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2BF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2BE;if(_tmp2BF->tag != 19)goto _LL174;else{_tmp2C0=(void*)_tmp2BF->f1;}};_LL173: {
# 1398
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp7FD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7FC;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C2=(_tmp7FC=_cycalloc(sizeof(*_tmp7FC)),((_tmp7FC[0]=((_tmp7FD.tag=18,((_tmp7FD.f1=_tmp2C1,_tmp7FD)))),_tmp7FC)));
if(Cyc_Tcutil_unify(_tmp2C0,(void*)_tmp2C2))return _tmp2BE;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1404
return Cyc_Absyn_uint_typ;}
# 1407
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1414
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1416
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp2C5=fs;
void*_tmp2C6=t_orig;
for(0;_tmp2C5 != 0;_tmp2C5=_tmp2C5->tl){
void*_tmp2C7=(void*)_tmp2C5->hd;
void*_tmp2C8=_tmp2C7;struct _dyneither_ptr*_tmp2CA;unsigned int _tmp2CC;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2C9=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2C8;if(_tmp2C9->tag != 0)goto _LL179;else{_tmp2CA=_tmp2C9->f1;}}_LL178: {
# 1424
int bad_type=1;
{void*_tmp2CD=Cyc_Tcutil_compress(_tmp2C6);struct Cyc_Absyn_Aggrdecl**_tmp2CF;struct Cyc_List_List*_tmp2D1;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2CE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CD;if(_tmp2CE->tag != 11)goto _LL17E;else{if((((_tmp2CE->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2CF=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2CE->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1427
 if((*_tmp2CF)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2CF)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp801;void*_tmp800[1];struct Cyc_String_pa_PrintArg_struct _tmp7FF;(_tmp7FF.tag=0,((_tmp7FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CA),((_tmp800[0]=& _tmp7FF,Cyc_Tcutil_terr(loc,((_tmp801="no field of struct/union has name %s",_tag_dyneither(_tmp801,sizeof(char),37))),_tag_dyneither(_tmp800,sizeof(void*),1)))))));}else{
# 1432
_tmp2C6=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2D0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CD;if(_tmp2D0->tag != 12)goto _LL180;else{_tmp2D1=_tmp2D0->f2;}}_LL17F: {
# 1436
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CA,_tmp2D1);
if(!((unsigned int)t2)){
const char*_tmp805;void*_tmp804[1];struct Cyc_String_pa_PrintArg_struct _tmp803;(_tmp803.tag=0,((_tmp803.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CA),((_tmp804[0]=& _tmp803,Cyc_Tcutil_terr(loc,((_tmp805="no field of struct/union has name %s",_tag_dyneither(_tmp805,sizeof(char),37))),_tag_dyneither(_tmp804,sizeof(void*),1)))))));}else{
# 1440
_tmp2C6=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1445
if(bad_type){
if(_tmp2C5 == fs){
const char*_tmp809;void*_tmp808[1];struct Cyc_String_pa_PrintArg_struct _tmp807;(_tmp807.tag=0,((_tmp807.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C6)),((_tmp808[0]=& _tmp807,Cyc_Tcutil_terr(loc,((_tmp809="%s is not a known struct/union type",_tag_dyneither(_tmp809,sizeof(char),36))),_tag_dyneither(_tmp808,sizeof(void*),1)))))));}else{
# 1449
const char*_tmp80D;void*_tmp80C[1];struct Cyc_String_pa_PrintArg_struct _tmp80B;struct _dyneither_ptr _tmp2DB=(_tmp80B.tag=0,((_tmp80B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp80C[0]=& _tmp80B,Cyc_aprintf(((_tmp80D="(%s)",_tag_dyneither(_tmp80D,sizeof(char),5))),_tag_dyneither(_tmp80C,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2C5;x=x->tl){
void*_tmp2DC=(void*)((struct Cyc_List_List*)_check_null(x))->hd;struct _dyneither_ptr*_tmp2DE;unsigned int _tmp2E0;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2DD=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2DC;if(_tmp2DD->tag != 0)goto _LL185;else{_tmp2DE=_tmp2DD->f1;}}_LL184:
# 1454
{const char*_tmp812;void*_tmp811[2];struct Cyc_String_pa_PrintArg_struct _tmp810;struct Cyc_String_pa_PrintArg_struct _tmp80F;_tmp2DB=((_tmp80F.tag=0,((_tmp80F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DE),((_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DB),((_tmp811[0]=& _tmp810,((_tmp811[1]=& _tmp80F,Cyc_aprintf(((_tmp812="%s.%s",_tag_dyneither(_tmp812,sizeof(char),6))),_tag_dyneither(_tmp811,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2DF=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2DC;if(_tmp2DF->tag != 1)goto _LL182;else{_tmp2E0=_tmp2DF->f1;}}_LL186:
# 1456
{const char*_tmp817;void*_tmp816[2];struct Cyc_String_pa_PrintArg_struct _tmp815;struct Cyc_Int_pa_PrintArg_struct _tmp814;_tmp2DB=((_tmp814.tag=1,((_tmp814.f1=(unsigned long)((int)_tmp2E0),((_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DB),((_tmp816[0]=& _tmp815,((_tmp816[1]=& _tmp814,Cyc_aprintf(((_tmp817="%s.%d",_tag_dyneither(_tmp817,sizeof(char),6))),_tag_dyneither(_tmp816,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1458
const char*_tmp81C;void*_tmp81B[2];struct Cyc_String_pa_PrintArg_struct _tmp81A;struct Cyc_String_pa_PrintArg_struct _tmp819;(_tmp819.tag=0,((_tmp819.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C6)),((_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DB),((_tmp81B[0]=& _tmp81A,((_tmp81B[1]=& _tmp819,Cyc_Tcutil_terr(loc,((_tmp81C="%s == %s is not a struct/union type",_tag_dyneither(_tmp81C,sizeof(char),36))),_tag_dyneither(_tmp81B,sizeof(void*),2)))))))))))));};}}
# 1461
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2CB=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2C8;if(_tmp2CB->tag != 1)goto _LL176;else{_tmp2CC=_tmp2CB->f1;}}_LL17A: {
# 1463
int bad_type=1;
{void*_tmp2F0=Cyc_Tcutil_compress(_tmp2C6);struct Cyc_Absyn_Aggrdecl**_tmp2F2;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F6;struct Cyc_Absyn_Datatypefield*_tmp2F8;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F1->tag != 11)goto _LL18A;else{if((((_tmp2F1->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp2F2=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2F1->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1466
 if((*_tmp2F2)->impl == 0)
goto _LL187;
_tmp2F4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F2)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F3->tag != 12)goto _LL18C;else{_tmp2F4=_tmp2F3->f2;}}_LL18B:
# 1470
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F4)<= _tmp2CC){
const char*_tmp821;void*_tmp820[2];struct Cyc_Int_pa_PrintArg_struct _tmp81F;struct Cyc_Int_pa_PrintArg_struct _tmp81E;(_tmp81E.tag=1,((_tmp81E.f1=(unsigned long)((int)_tmp2CC),((_tmp81F.tag=1,((_tmp81F.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F4),((_tmp820[0]=& _tmp81F,((_tmp820[1]=& _tmp81E,Cyc_Tcutil_terr(loc,((_tmp821="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp821,sizeof(char),46))),_tag_dyneither(_tmp820,sizeof(void*),2)))))))))))));}else{
# 1474
_tmp2C6=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F4,(int)_tmp2CC))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F5->tag != 10)goto _LL18E;else{_tmp2F6=_tmp2F5->f1;}}_LL18D:
# 1478
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6)<= _tmp2CC){
const char*_tmp826;void*_tmp825[2];struct Cyc_Int_pa_PrintArg_struct _tmp824;struct Cyc_Int_pa_PrintArg_struct _tmp823;(_tmp823.tag=1,((_tmp823.f1=(unsigned long)((int)_tmp2CC),((_tmp824.tag=1,((_tmp824.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6),((_tmp825[0]=& _tmp824,((_tmp825[1]=& _tmp823,Cyc_Tcutil_terr(loc,((_tmp826="tuple has too few components: %d <= %d",_tag_dyneither(_tmp826,sizeof(char),39))),_tag_dyneither(_tmp825,sizeof(void*),2)))))))))))));}else{
# 1482
_tmp2C6=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F6,(int)_tmp2CC)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F7->tag != 4)goto _LL190;else{if((((_tmp2F7->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp2F8=((struct _tuple2)(((_tmp2F7->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1486
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8->typs)< _tmp2CC){
const char*_tmp82B;void*_tmp82A[2];struct Cyc_Int_pa_PrintArg_struct _tmp829;struct Cyc_Int_pa_PrintArg_struct _tmp828;(_tmp828.tag=1,((_tmp828.f1=(unsigned long)((int)_tmp2CC),((_tmp829.tag=1,((_tmp829.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8->typs),((_tmp82A[0]=& _tmp829,((_tmp82A[1]=& _tmp828,Cyc_Tcutil_terr(loc,((_tmp82B="datatype field has too few components: %d < %d",_tag_dyneither(_tmp82B,sizeof(char),47))),_tag_dyneither(_tmp82A,sizeof(void*),2)))))))))))));}else{
# 1490
if(_tmp2CC != 0)
_tmp2C6=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F8->typs,(int)(_tmp2CC - 1))).f2;else{
if(_tmp2C5->tl != 0){
const char*_tmp82E;void*_tmp82D;(_tmp82D=0,Cyc_Tcutil_terr(loc,((_tmp82E="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp82E,sizeof(char),68))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}}}
# 1495
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1499
if(bad_type){
const char*_tmp832;void*_tmp831[1];struct Cyc_String_pa_PrintArg_struct _tmp830;(_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C6)),((_tmp831[0]=& _tmp830,Cyc_Tcutil_terr(loc,((_tmp832="%s is not a known type",_tag_dyneither(_tmp832,sizeof(char),23))),_tag_dyneither(_tmp831,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1504
return Cyc_Absyn_uint_typ;};}
# 1508
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp30A=_new_region("r");struct _RegionHandle*r=& _tmp30A;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp30B=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp30B,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp30C=t;void*_tmp30F;void*_tmp310;union Cyc_Absyn_Constraint*_tmp311;union Cyc_Absyn_Constraint*_tmp312;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp30D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30C;if(_tmp30D->tag != 1)goto _LL195;}_LL194: {
# 1515
struct Cyc_List_List*_tmp313=Cyc_Tcenv_lookup_type_vars(_tmp30B);
struct Cyc_Core_Opt*_tmp833;void*_tmp314=Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp833=_cycalloc(sizeof(*_tmp833)),((_tmp833->v=_tmp313,_tmp833)))));
struct Cyc_Core_Opt*_tmp834;void*_tmp315=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->v=_tmp313,_tmp834)))));
union Cyc_Absyn_Constraint*_tmp316=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp317=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp835;struct Cyc_Absyn_PtrAtts _tmp318=(_tmp835.rgn=_tmp315,((_tmp835.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp835.bounds=_tmp316,((_tmp835.zero_term=_tmp317,((_tmp835.ptrloc=0,_tmp835)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp83B;struct Cyc_Absyn_PtrInfo _tmp83A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp839;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp319=(_tmp839=_cycalloc(sizeof(*_tmp839)),((_tmp839[0]=((_tmp83B.tag=5,((_tmp83B.f1=((_tmp83A.elt_typ=_tmp314,((_tmp83A.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp83A.ptr_atts=_tmp318,_tmp83A)))))),_tmp83B)))),_tmp839)));
Cyc_Tcutil_unify(t,(void*)_tmp319);
_tmp30F=_tmp314;_tmp310=_tmp315;_tmp311=_tmp316;_tmp312=_tmp317;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30C;if(_tmp30E->tag != 5)goto _LL197;else{_tmp30F=(_tmp30E->f1).elt_typ;_tmp310=((_tmp30E->f1).ptr_atts).rgn;_tmp311=((_tmp30E->f1).ptr_atts).bounds;_tmp312=((_tmp30E->f1).ptr_atts).zero_term;}}_LL196:
# 1525
 Cyc_Tcenv_check_rgn_accessible(_tmp30B,loc,_tmp310);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp311);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp30F),& Cyc_Tcutil_tmk)){
const char*_tmp83E;void*_tmp83D;(_tmp83D=0,Cyc_Tcutil_terr(loc,((_tmp83E="can't dereference abstract pointer type",_tag_dyneither(_tmp83E,sizeof(char),40))),_tag_dyneither(_tmp83D,sizeof(void*),0)));}{
void*_tmp322=_tmp30F;_npop_handler(0);return _tmp322;};_LL197:;_LL198: {
# 1531
const char*_tmp842;void*_tmp841[1];struct Cyc_String_pa_PrintArg_struct _tmp840;void*_tmp326=(_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp841[0]=& _tmp840,Cyc_Tcexp_expr_err(_tmp30B,loc,topt,((_tmp842="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp842,sizeof(char),39))),_tag_dyneither(_tmp841,sizeof(void*),1)))))));_npop_handler(0);return _tmp326;}_LL192:;};}
# 1510
;_pop_region(r);}
# 1537
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1541
struct _RegionHandle _tmp327=_new_region("r");struct _RegionHandle*r=& _tmp327;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1545
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp328=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Aggrdecl*_tmp32A;struct Cyc_List_List*_tmp32B;enum Cyc_Absyn_AggrKind _tmp32D;struct Cyc_List_List*_tmp32E;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp328;if(_tmp329->tag != 11)goto _LL19C;else{if((((_tmp329->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp32A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp329->f1).aggr_info).KnownAggr).val);_tmp32B=(_tmp329->f1).targs;}}_LL19B: {
# 1548
struct Cyc_Absyn_Aggrfield*_tmp333=Cyc_Absyn_lookup_decl_field(_tmp32A,f);
if(_tmp333 == 0){
const char*_tmp847;void*_tmp846[2];struct Cyc_String_pa_PrintArg_struct _tmp845;struct Cyc_String_pa_PrintArg_struct _tmp844;void*_tmp338=(_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp32A->name)),((_tmp846[0]=& _tmp845,((_tmp846[1]=& _tmp844,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp847="type %s has no %s field",_tag_dyneither(_tmp847,sizeof(char),24))),_tag_dyneither(_tmp846,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp338;}
# 1553
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32A->impl))->tagged)*is_tagged=1;{
void*t2=_tmp333->type;
if(_tmp32B != 0){
struct _RegionHandle _tmp339=_new_region("rgn");struct _RegionHandle*rgn=& _tmp339;_push_region(rgn);
{struct Cyc_List_List*_tmp33A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp32A->tvs,_tmp32B);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp33A,_tmp333->type);}
# 1557
;_pop_region(rgn);}
# 1562
if(((_tmp32A->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32A->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
const char*_tmp84B;void*_tmp84A[1];struct Cyc_String_pa_PrintArg_struct _tmp849;void*_tmp33E=(_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp84A[0]=& _tmp849,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84B="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp84B,sizeof(char),56))),_tag_dyneither(_tmp84A,sizeof(void*),1)))))));_npop_handler(0);return _tmp33E;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32A->impl))->exist_vars != 0){
# 1567
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp84F;void*_tmp84E[1];struct Cyc_String_pa_PrintArg_struct _tmp84D;void*_tmp342=(_tmp84D.tag=0,((_tmp84D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp84E[0]=& _tmp84D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84F="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp84F,sizeof(char),81))),_tag_dyneither(_tmp84E,sizeof(void*),1)))))));_npop_handler(0);return _tmp342;}}{
# 1570
void*_tmp343=t2;_npop_handler(0);return _tmp343;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp328;if(_tmp32C->tag != 12)goto _LL19E;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;}}_LL19D: {
# 1572
struct Cyc_Absyn_Aggrfield*_tmp344=Cyc_Absyn_lookup_field(_tmp32E,f);
if(_tmp344 == 0){
const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_PrintArg_struct _tmp851;void*_tmp348=(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp852[0]=& _tmp851,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp853="type has no %s field",_tag_dyneither(_tmp853,sizeof(char),21))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));_npop_handler(0);return _tmp348;}
if((_tmp32D == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp344->type)){
const char*_tmp857;void*_tmp856[1];struct Cyc_String_pa_PrintArg_struct _tmp855;void*_tmp34C=(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp856[0]=& _tmp855,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp857="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp857,sizeof(char),56))),_tag_dyneither(_tmp856,sizeof(void*),1)))))));_npop_handler(0);return _tmp34C;}{
void*_tmp34D=_tmp344->type;_npop_handler(0);return _tmp34D;};}_LL19E:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp328;if(_tmp32F->tag != 8)goto _LL1A0;}{const char*_tmp858;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp858="size",_tag_dyneither(_tmp858,sizeof(char),5))))== 0))goto _LL1A0;};_LL19F: goto _LL1A1;_LL1A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp328;if(_tmp331->tag != 5)goto _LL1A2;}{const char*_tmp859;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp859="size",_tag_dyneither(_tmp859,sizeof(char),5))))== 0))goto _LL1A2;};_LL1A1:
{const char*_tmp85D;void*_tmp85C[1];struct Cyc_String_pa_PrintArg_struct _tmp85B;(_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp85C[0]=& _tmp85B,Cyc_Tcutil_warn(e->loc,((_tmp85D="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp85D,sizeof(char),54))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp863;struct Cyc_List_List*_tmp862;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp861;outer_e->r=(void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp863.tag=2,((_tmp863.f1=Cyc_Absyn_Numelts,((_tmp863.f2=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->hd=e,((_tmp862->tl=0,_tmp862)))))),_tmp863)))))),_tmp861))));}{
void*_tmp354=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp354;};_LL1A2:;_LL1A3: {
# 1584
const char*_tmp864;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp864="size",_tag_dyneither(_tmp864,sizeof(char),5))))== 0){
const char*_tmp868;void*_tmp867[1];struct Cyc_String_pa_PrintArg_struct _tmp866;void*_tmp359=(_tmp866.tag=0,((_tmp866.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1587
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp867[0]=& _tmp866,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp868="expecting struct, union, or array, found %s",_tag_dyneither(_tmp868,sizeof(char),44))),_tag_dyneither(_tmp867,sizeof(void*),1)))))));_npop_handler(0);return _tmp359;}else{
# 1589
const char*_tmp86C;void*_tmp86B[1];struct Cyc_String_pa_PrintArg_struct _tmp86A;void*_tmp35D=(_tmp86A.tag=0,((_tmp86A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp86B[0]=& _tmp86A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86C="expecting struct or union, found %s",_tag_dyneither(_tmp86C,sizeof(char),36))),_tag_dyneither(_tmp86B,sizeof(void*),1)))))));_npop_handler(0);return _tmp35D;}}_LL199:;};
# 1541
;_pop_region(r);}
# 1595
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1598
struct _RegionHandle _tmp35E=_new_region("r");struct _RegionHandle*r=& _tmp35E;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1602
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp35F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp361;void*_tmp362;union Cyc_Absyn_Constraint*_tmp363;union Cyc_Absyn_Constraint*_tmp364;_LL1A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35F;if(_tmp360->tag != 5)goto _LL1A7;else{_tmp361=(_tmp360->f1).elt_typ;_tmp362=((_tmp360->f1).ptr_atts).rgn;_tmp363=((_tmp360->f1).ptr_atts).bounds;_tmp364=((_tmp360->f1).ptr_atts).zero_term;}}_LL1A6:
# 1605
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp363);
{void*_tmp365=Cyc_Tcutil_compress(_tmp361);struct Cyc_Absyn_Aggrdecl*_tmp367;struct Cyc_List_List*_tmp368;enum Cyc_Absyn_AggrKind _tmp36A;struct Cyc_List_List*_tmp36B;_LL1AA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp365;if(_tmp366->tag != 11)goto _LL1AC;else{if((((_tmp366->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AC;_tmp367=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp366->f1).aggr_info).KnownAggr).val);_tmp368=(_tmp366->f1).targs;}}_LL1AB: {
# 1608
struct Cyc_Absyn_Aggrfield*_tmp36C=Cyc_Absyn_lookup_decl_field(_tmp367,f);
if(_tmp36C == 0){
const char*_tmp871;void*_tmp870[2];struct Cyc_String_pa_PrintArg_struct _tmp86F;struct Cyc_String_pa_PrintArg_struct _tmp86E;void*_tmp371=(_tmp86E.tag=0,((_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp86F.tag=0,((_tmp86F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp367->name)),((_tmp870[0]=& _tmp86F,((_tmp870[1]=& _tmp86E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp871="type %s has no %s field",_tag_dyneither(_tmp871,sizeof(char),24))),_tag_dyneither(_tmp870,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp371;}
# 1613
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->tagged)*is_tagged=1;{
void*t3=_tmp36C->type;
if(_tmp368 != 0){
struct _RegionHandle _tmp372=_new_region("rgn");struct _RegionHandle*rgn=& _tmp372;_push_region(rgn);
{struct Cyc_List_List*_tmp373=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp367->tvs,_tmp368);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp373,_tmp36C->type);}
# 1617
;_pop_region(rgn);}{
# 1621
struct Cyc_Absyn_Kind*_tmp374=Cyc_Tcutil_typ_kind(t3);
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp374)){
# 1624
void*_tmp375=Cyc_Tcutil_compress(t3);_LL1B1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp375;if(_tmp376->tag != 8)goto _LL1B3;}_LL1B2:
 goto _LL1B0;_LL1B3:;_LL1B4: {
# 1627
const char*_tmp875;void*_tmp874[1];struct Cyc_String_pa_PrintArg_struct _tmp873;void*_tmp37A=(_tmp873.tag=0,((_tmp873.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp874[0]=& _tmp873,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp875="cannot get member %s since its type is abstract",_tag_dyneither(_tmp875,sizeof(char),48))),_tag_dyneither(_tmp874,sizeof(void*),1)))))));_npop_handler(0);return _tmp37A;}_LL1B0:;}
# 1630
if(((_tmp367->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
const char*_tmp879;void*_tmp878[1];struct Cyc_String_pa_PrintArg_struct _tmp877;void*_tmp37E=(_tmp877.tag=0,((_tmp877.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp878[0]=& _tmp877,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp879="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp879,sizeof(char),56))),_tag_dyneither(_tmp878,sizeof(void*),1)))))));_npop_handler(0);return _tmp37E;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp87D;void*_tmp87C[1];struct Cyc_String_pa_PrintArg_struct _tmp87B;void*_tmp382=(_tmp87B.tag=0,((_tmp87B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87C[0]=& _tmp87B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87D="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp87D,sizeof(char),72))),_tag_dyneither(_tmp87C,sizeof(void*),1)))))));_npop_handler(0);return _tmp382;}}{
# 1638
void*_tmp383=t3;_npop_handler(0);return _tmp383;};};};}_LL1AC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp365;if(_tmp369->tag != 12)goto _LL1AE;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp369->f2;}}_LL1AD: {
# 1640
struct Cyc_Absyn_Aggrfield*_tmp384=Cyc_Absyn_lookup_field(_tmp36B,f);
if(_tmp384 == 0){
const char*_tmp881;void*_tmp880[1];struct Cyc_String_pa_PrintArg_struct _tmp87F;void*_tmp388=(_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp880[0]=& _tmp87F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp881="type has no %s field",_tag_dyneither(_tmp881,sizeof(char),21))),_tag_dyneither(_tmp880,sizeof(void*),1)))))));_npop_handler(0);return _tmp388;}
if((_tmp36A == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp384->type)){
const char*_tmp885;void*_tmp884[1];struct Cyc_String_pa_PrintArg_struct _tmp883;void*_tmp38C=(_tmp883.tag=0,((_tmp883.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp884[0]=& _tmp883,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp885="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp885,sizeof(char),56))),_tag_dyneither(_tmp884,sizeof(void*),1)))))));_npop_handler(0);return _tmp38C;}{
void*_tmp38D=_tmp384->type;_npop_handler(0);return _tmp38D;};}_LL1AE:;_LL1AF:
 goto _LL1A9;_LL1A9:;}
# 1648
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}{
# 1651
const char*_tmp889;void*_tmp888[1];struct Cyc_String_pa_PrintArg_struct _tmp887;void*_tmp391=(_tmp887.tag=0,((_tmp887.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp888[0]=& _tmp887,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp889="expecting struct or union pointer, found %s",_tag_dyneither(_tmp889,sizeof(char),44))),_tag_dyneither(_tmp888,sizeof(void*),1)))))));_npop_handler(0);return _tmp391;};
# 1598
;_pop_region(r);}
# 1656
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1658
unsigned int _tmp393;int _tmp394;struct _tuple15 _tmp392=Cyc_Evexp_eval_const_uint_exp(index);_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;
if(!_tmp394){
const char*_tmp88C;void*_tmp88B;return(_tmp88B=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp88C="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp88C,sizeof(char),47))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}{
# 1662
struct _handler_cons _tmp397;_push_handler(& _tmp397);{int _tmp399=0;if(setjmp(_tmp397.handler))_tmp399=1;if(!_tmp399){
{void*_tmp39A=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp393)).f2;_npop_handler(0);return _tmp39A;};_pop_handler();}else{void*_tmp398=(void*)_exn_thrown;void*_tmp39C=_tmp398;void*_tmp39E;_LL1B6: {struct Cyc_List_Nth_exn_struct*_tmp39D=(struct Cyc_List_Nth_exn_struct*)_tmp39C;if(_tmp39D->tag != Cyc_List_Nth)goto _LL1B8;}_LL1B7: {
# 1665
const char*_tmp891;void*_tmp890[2];struct Cyc_Int_pa_PrintArg_struct _tmp88F;struct Cyc_Int_pa_PrintArg_struct _tmp88E;return(_tmp88E.tag=1,((_tmp88E.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp88F.tag=1,((_tmp88F.f1=(unsigned long)((int)_tmp393),((_tmp890[0]=& _tmp88F,((_tmp890[1]=& _tmp88E,Cyc_Tcexp_expr_err(te,loc,0,((_tmp891="index is %d but tuple has only %d fields",_tag_dyneither(_tmp891,sizeof(char),41))),_tag_dyneither(_tmp890,sizeof(void*),2)))))))))))));}_LL1B8: _tmp39E=_tmp39C;_LL1B9:(void)_throw(_tmp39E);_LL1B5:;}};};}
# 1670
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1672
struct _RegionHandle _tmp3A3=_new_region("r");struct _RegionHandle*r=& _tmp3A3;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3A4=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp3A4,0,e1);
Cyc_Tcexp_tcExp(_tmp3A4,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A4,e2)){
const char*_tmp895;void*_tmp894[1];struct Cyc_String_pa_PrintArg_struct _tmp893;void*_tmp3A8=(_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp894[0]=& _tmp893,Cyc_Tcexp_expr_err(_tmp3A4,e2->loc,topt,((_tmp895="expecting int subscript, found %s",_tag_dyneither(_tmp895,sizeof(char),34))),_tag_dyneither(_tmp894,sizeof(void*),1)))))));_npop_handler(0);return _tmp3A8;}{
# 1683
void*_tmp3A9=t1;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AC;void*_tmp3AD;union Cyc_Absyn_Constraint*_tmp3AE;union Cyc_Absyn_Constraint*_tmp3AF;struct Cyc_List_List*_tmp3B1;_LL1BB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3AA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AA->tag != 5)goto _LL1BD;else{_tmp3AB=(_tmp3AA->f1).elt_typ;_tmp3AC=(_tmp3AA->f1).elt_tq;_tmp3AD=((_tmp3AA->f1).ptr_atts).rgn;_tmp3AE=((_tmp3AA->f1).ptr_atts).bounds;_tmp3AF=((_tmp3AA->f1).ptr_atts).zero_term;}}_LL1BC:
# 1687
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3AF)){
int emit_warning=1;
{void*_tmp3B2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3AE);struct Cyc_Absyn_Exp*_tmp3B4;_LL1C2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3B2;if(_tmp3B3->tag != 1)goto _LL1C4;else{_tmp3B4=_tmp3B3->f1;}}_LL1C3:
# 1691
 if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3B7;int _tmp3B8;struct _tuple15 _tmp3B6=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3B7=_tmp3B6.f1;_tmp3B8=_tmp3B6.f2;
if(_tmp3B8){
unsigned int _tmp3BA;int _tmp3BB;struct _tuple15 _tmp3B9=Cyc_Evexp_eval_const_uint_exp(_tmp3B4);_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B9.f2;
if(_tmp3BB  && _tmp3BA > _tmp3B7)emit_warning=0;}}
# 1698
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3B5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B2;if(_tmp3B5->tag != 0)goto _LL1C1;}_LL1C5:
 emit_warning=0;goto _LL1C1;_LL1C1:;}
# 1701
if(emit_warning){
const char*_tmp898;void*_tmp897;(_tmp897=0,Cyc_Tcutil_warn(e2->loc,((_tmp898="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp898,sizeof(char),63))),_tag_dyneither(_tmp897,sizeof(void*),0)));}}else{
# 1705
if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3BF;int _tmp3C0;struct _tuple15 _tmp3BE=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BF=_tmp3BE.f1;_tmp3C0=_tmp3BE.f2;
if(_tmp3C0)
Cyc_Tcutil_check_bound(loc,_tmp3BF,_tmp3AE);}else{
# 1711
if(Cyc_Tcutil_is_bound_one(_tmp3AE) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3AF)){
const char*_tmp89B;void*_tmp89A;(_tmp89A=0,Cyc_Tcutil_warn(e1->loc,((_tmp89B="subscript applied to pointer to one object",_tag_dyneither(_tmp89B,sizeof(char),43))),_tag_dyneither(_tmp89A,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3AE);}}
# 1716
Cyc_Tcenv_check_rgn_accessible(_tmp3A4,loc,_tmp3AD);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3AB),& Cyc_Tcutil_tmk)){
const char*_tmp89E;void*_tmp89D;(_tmp89D=0,Cyc_Tcutil_terr(e1->loc,((_tmp89E="can't subscript an abstract pointer",_tag_dyneither(_tmp89E,sizeof(char),36))),_tag_dyneither(_tmp89D,sizeof(void*),0)));}{
void*_tmp3C5=_tmp3AB;_npop_handler(0);return _tmp3C5;};_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A9;if(_tmp3B0->tag != 10)goto _LL1BF;else{_tmp3B1=_tmp3B0->f1;}}_LL1BE: {
void*_tmp3C6=Cyc_Tcexp_ithTupleType(_tmp3A4,loc,_tmp3B1,e2);_npop_handler(0);return _tmp3C6;}_LL1BF:;_LL1C0: {
const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_PrintArg_struct _tmp8A0;void*_tmp3CA=(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8A1[0]=& _tmp8A0,Cyc_Tcexp_expr_err(_tmp3A4,loc,topt,((_tmp8A2="subscript applied to %s",_tag_dyneither(_tmp8A2,sizeof(char),24))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CA;}_LL1BA:;};};}
# 1673
;_pop_region(r);}
# 1727
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp3CB=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3CD;_LL1C7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3CC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CB;if(_tmp3CC->tag != 10)goto _LL1C9;else{_tmp3CD=_tmp3CC->f1;}}_LL1C8:
# 1733
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3CD)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1736
goto _LL1C6;
# 1738
for(0;es != 0;(es=es->tl,_tmp3CD=_tmp3CD->tl)){
int bogus=0;
void*_tmp3CE=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3CD))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp3CE,(struct Cyc_Absyn_Exp*)es->hd);
# 1743
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3CD->hd)).f2,& bogus);{
struct _tuple19*_tmp8A5;struct Cyc_List_List*_tmp8A4;fields=((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4->hd=((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->f1=(*((struct _tuple19*)_tmp3CD->hd)).f1,((_tmp8A5->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8A5)))))),((_tmp8A4->tl=fields,_tmp8A4))))));};}
# 1746
done=1;
goto _LL1C6;_LL1C9:;_LL1CA:
 goto _LL1C6;_LL1C6:;}
# 1750
if(!done)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple19*_tmp8A8;struct Cyc_List_List*_tmp8A7;fields=((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7->hd=((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8->f1=Cyc_Absyn_empty_tqual(0),((_tmp8A8->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8A8)))))),((_tmp8A7->tl=fields,_tmp8A7))))));};}{
# 1755
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8AB;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8AA;return(void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8AB.tag=10,((_tmp8AB.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8AB)))),_tmp8AA))));};}
# 1759
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1762
const char*_tmp8AE;void*_tmp8AD;return(_tmp8AD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AE="tcCompoundLit",_tag_dyneither(_tmp8AE,sizeof(char),14))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}
# 1772 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1775
void*res_t2;
struct _RegionHandle _tmp3D7=_new_region("r");struct _RegionHandle*r=& _tmp3D7;_push_region(r);{
int _tmp3D8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8B1;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8B0;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3D9=(_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=0,((_tmp8B1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3D8),_tmp8B1)))),_tmp8B0)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3D9,loc);
# 1784
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3DA=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3D8 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3DA)){
const char*_tmp8B4;void*_tmp8B3;(_tmp8B3=0,Cyc_Tcutil_terr(_tmp3DA->loc,((_tmp8B4="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8B4,sizeof(char),45))),_tag_dyneither(_tmp8B3,sizeof(void*),0)));}}
# 1789
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1794
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1797
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1799
const char*_tmp8B8;void*_tmp8B7[1];struct Cyc_String_pa_PrintArg_struct _tmp8B6;(_tmp8B6.tag=0,((_tmp8B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8B7[0]=& _tmp8B6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8B8="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8B8,sizeof(char),53))),_tag_dyneither(_tmp8B7,sizeof(void*),1)))))));}}
# 1803
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1808
void*_tmp3E2=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp3E5;_LL1CC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E2;if(_tmp3E3->tag != 1)goto _LL1CE;}_LL1CD:
# 1810
{const char*_tmp8BB;void*_tmp8BA;(_tmp8BA=0,Cyc_Tcutil_terr(loc,((_tmp8BB="only array index designators are supported",_tag_dyneither(_tmp8BB,sizeof(char),43))),_tag_dyneither(_tmp8BA,sizeof(void*),0)));}
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3E4=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3E2;if(_tmp3E4->tag != 0)goto _LL1CB;else{_tmp3E5=_tmp3E4->f1;}}_LL1CF:
# 1813
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3E5);{
unsigned int _tmp3E9;int _tmp3EA;struct _tuple15 _tmp3E8=Cyc_Evexp_eval_const_uint_exp(_tmp3E5);_tmp3E9=_tmp3E8.f1;_tmp3EA=_tmp3E8.f2;
if(!_tmp3EA){
const char*_tmp8BE;void*_tmp8BD;(_tmp8BD=0,Cyc_Tcutil_terr(_tmp3E5->loc,((_tmp8BE="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8BE,sizeof(char),47))),_tag_dyneither(_tmp8BD,sizeof(void*),0)));}else{
if(_tmp3E9 != offset){
const char*_tmp8C3;void*_tmp8C2[2];struct Cyc_Int_pa_PrintArg_struct _tmp8C1;struct Cyc_Int_pa_PrintArg_struct _tmp8C0;(_tmp8C0.tag=1,((_tmp8C0.f1=(unsigned long)((int)_tmp3E9),((_tmp8C1.tag=1,((_tmp8C1.f1=(unsigned long)offset,((_tmp8C2[0]=& _tmp8C1,((_tmp8C2[1]=& _tmp8C0,Cyc_Tcutil_terr(_tmp3E5->loc,((_tmp8C3="expecting index designator %d but found %d",_tag_dyneither(_tmp8C3,sizeof(char),43))),_tag_dyneither(_tmp8C2,sizeof(void*),2)))))))))))));}}
# 1820
goto _LL1CB;};_LL1CB:;}}}{
# 1824
void*_tmp3F1=res_t2;_npop_handler(0);return _tmp3F1;};
# 1776
;_pop_region(r);}
# 1852 "tcexp.cyc"
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1855
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3F2=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3F4;_LL1D1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F2;if(_tmp3F3->tag != 19)goto _LL1D3;else{_tmp3F4=(void*)_tmp3F3->f1;}}_LL1D2:
# 1860
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3F4,0),0,Cyc_Absyn_No_coercion,0);
_tmp3F5->topt=bound->topt;
# 1865
bound=_tmp3F5;}
# 1867
goto _LL1D0;_LL1D3:;_LL1D4:
# 1869
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp8C7;void*_tmp8C6[1];struct Cyc_String_pa_PrintArg_struct _tmp8C5;(_tmp8C5.tag=0,((_tmp8C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8C6[0]=& _tmp8C5,Cyc_Tcutil_terr(bound->loc,((_tmp8C7="expecting unsigned int, found %s",_tag_dyneither(_tmp8C7,sizeof(char),33))),_tag_dyneither(_tmp8C6,sizeof(void*),1)))))));}_LL1D0:;}
# 1874
if(!(vd->tq).real_const){
const char*_tmp8CA;void*_tmp8C9;(_tmp8C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8CA="comprehension index variable is not declared const!",_tag_dyneither(_tmp8CA,sizeof(char),52))),_tag_dyneither(_tmp8C9,sizeof(void*),0)));}{
# 1877
struct _RegionHandle _tmp3FB=_new_region("r");struct _RegionHandle*r=& _tmp3FB;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1883
struct Cyc_Tcenv_Tenv*_tmp3FC=te2;
void**_tmp3FD=0;
struct Cyc_Absyn_Tqual*_tmp3FE=0;
union Cyc_Absyn_Constraint**_tmp3FF=0;
# 1888
if(topt != 0){
void*_tmp400=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp402;void*_tmp404;struct Cyc_Absyn_Tqual _tmp405;struct Cyc_Absyn_Exp*_tmp406;union Cyc_Absyn_Constraint*_tmp407;_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp400;if(_tmp401->tag != 5)goto _LL1D8;else{_tmp402=_tmp401->f1;}}_LL1D7:
# 1891
{void**_tmp8CB;_tmp3FD=((_tmp8CB=_region_malloc(r,sizeof(*_tmp8CB)),((_tmp8CB[0]=_tmp402.elt_typ,_tmp8CB))));}
{struct Cyc_Absyn_Tqual*_tmp8CC;_tmp3FE=((_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=_tmp402.elt_tq,_tmp8CC))));}
{union Cyc_Absyn_Constraint**_tmp8CD;_tmp3FF=((_tmp8CD=_region_malloc(r,sizeof(*_tmp8CD)),((_tmp8CD[0]=(_tmp402.ptr_atts).zero_term,_tmp8CD))));}
goto _LL1D5;_LL1D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp403=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp400;if(_tmp403->tag != 8)goto _LL1DA;else{_tmp404=(_tmp403->f1).elt_type;_tmp405=(_tmp403->f1).tq;_tmp406=(_tmp403->f1).num_elts;_tmp407=(_tmp403->f1).zero_term;}}_LL1D9:
# 1896
{void**_tmp8CE;_tmp3FD=((_tmp8CE=_region_malloc(r,sizeof(*_tmp8CE)),((_tmp8CE[0]=_tmp404,_tmp8CE))));}
{struct Cyc_Absyn_Tqual*_tmp8CF;_tmp3FE=((_tmp8CF=_region_malloc(r,sizeof(*_tmp8CF)),((_tmp8CF[0]=_tmp405,_tmp8CF))));}
{union Cyc_Absyn_Constraint**_tmp8D0;_tmp3FF=((_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0[0]=_tmp407,_tmp8D0))));}
goto _LL1D5;_LL1DA:;_LL1DB:
# 1901
 goto _LL1D5;_LL1D5:;}{
# 1904
void*t=Cyc_Tcexp_tcExp(_tmp3FC,_tmp3FD,body);
if(_tmp3FC->le == 0){
# 1907
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp8D3;void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(bound->loc,((_tmp8D3="bound is not constant",_tag_dyneither(_tmp8D3,sizeof(char),22))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_terr(bound->loc,((_tmp8D6="body is not constant",_tag_dyneither(_tmp8D6,sizeof(char),21))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}}
# 1912
if(_tmp3FF != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3FF)){
# 1915
struct Cyc_Absyn_Exp*_tmp412=Cyc_Absyn_uint_exp(1,0);_tmp412->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp412,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1920
void*_tmp413=Cyc_Absyn_array_typ(t,_tmp3FE == 0?Cyc_Absyn_empty_tqual(0):*_tmp3FE,bound,
_tmp3FF == 0?Cyc_Absyn_false_conref:*_tmp3FF,0);
void*_tmp414=_tmp413;_npop_handler(0);return _tmp414;};};};}
# 1878
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1936 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1940
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp8D7;adptr=((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=ad,_tmp8D7))));};}else{
# 1946
{struct _handler_cons _tmp416;_push_handler(& _tmp416);{int _tmp418=0;if(setjmp(_tmp416.handler))_tmp418=1;if(!_tmp418){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1946
;_pop_handler();}else{void*_tmp417=(void*)_exn_thrown;void*_tmp41A=_tmp417;void*_tmp41C;_LL1DD: {struct Cyc_Dict_Absent_exn_struct*_tmp41B=(struct Cyc_Dict_Absent_exn_struct*)_tmp41A;if(_tmp41B->tag != Cyc_Dict_Absent)goto _LL1DF;}_LL1DE:
# 1949
{const char*_tmp8DB;void*_tmp8DA[1];struct Cyc_String_pa_PrintArg_struct _tmp8D9;(_tmp8D9.tag=0,((_tmp8D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8DA[0]=& _tmp8D9,Cyc_Tcutil_terr(loc,((_tmp8DB="unbound struct/union name %s",_tag_dyneither(_tmp8DB,sizeof(char),29))),_tag_dyneither(_tmp8DA,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1DF: _tmp41C=_tmp41A;_LL1E0:(void)_throw(_tmp41C);_LL1DC:;}};}
# 1952
*ad_opt=ad;
*tn=ad->name;}
# 1955
if(ad->impl == 0){
{const char*_tmp8E3;void*_tmp8E2[1];const char*_tmp8E1;const char*_tmp8E0;struct Cyc_String_pa_PrintArg_struct _tmp8DF;(_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8E0="struct",_tag_dyneither(_tmp8E0,sizeof(char),7)):((_tmp8E1="union",_tag_dyneither(_tmp8E1,sizeof(char),6)))),((_tmp8E2[0]=& _tmp8DF,Cyc_Tcutil_terr(loc,((_tmp8E3="can't construct abstract %s",_tag_dyneither(_tmp8E3,sizeof(char),28))),_tag_dyneither(_tmp8E2,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1959
struct _RegionHandle _tmp425=_new_region("rgn");struct _RegionHandle*rgn=& _tmp425;_push_region(rgn);
# 1964
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp426=Cyc_Tcenv_new_status(te);
if(_tmp426 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 1973
te2=Cyc_Tcenv_set_new_status(rgn,_tmp426,te);}{
# 1975
struct _tuple12 _tmp8E4;struct _tuple12 _tmp427=(_tmp8E4.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp8E4.f2=rgn,_tmp8E4)));
struct Cyc_List_List*_tmp428=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp427,ad->tvs);
struct Cyc_List_List*_tmp429=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp427,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp42A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp428);
struct Cyc_List_List*_tmp42B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp429);
struct Cyc_List_List*_tmp42C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp428,_tmp429);
void*res_typ;
# 1986
if(topt != 0){
void*_tmp42D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Aggrdecl**_tmp42F;struct Cyc_List_List*_tmp430;_LL1E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp42E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp42D;if(_tmp42E->tag != 11)goto _LL1E4;else{if((((_tmp42E->f1).aggr_info).KnownAggr).tag != 2)goto _LL1E4;_tmp42F=(struct Cyc_Absyn_Aggrdecl**)(((_tmp42E->f1).aggr_info).KnownAggr).val;_tmp430=(_tmp42E->f1).targs;}}_LL1E3:
# 1989
 if(*_tmp42F == *adptr){
{struct Cyc_List_List*_tmp431=_tmp42A;for(0;_tmp431 != 0  && _tmp430 != 0;
(_tmp431=_tmp431->tl,_tmp430=_tmp430->tl)){
Cyc_Tcutil_unify((void*)_tmp431->hd,(void*)_tmp430->hd);}}
# 1994
res_typ=*topt;
goto _LL1E1;}
# 1997
goto _LL1E5;_LL1E4:;_LL1E5: {
# 1999
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8EA;struct Cyc_Absyn_AggrInfo _tmp8E9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8E8;res_typ=(void*)((_tmp8E8=_cycalloc(sizeof(*_tmp8E8)),((_tmp8E8[0]=((_tmp8EA.tag=11,((_tmp8EA.f1=((_tmp8E9.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8E9.targs=_tmp42A,_tmp8E9)))),_tmp8EA)))),_tmp8E8))));}_LL1E1:;}else{
# 2002
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8F0;struct Cyc_Absyn_AggrInfo _tmp8EF;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8EE;res_typ=(void*)((_tmp8EE=_cycalloc(sizeof(*_tmp8EE)),((_tmp8EE[0]=((_tmp8F0.tag=11,((_tmp8F0.f1=((_tmp8EF.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8EF.targs=_tmp42A,_tmp8EF)))),_tmp8F0)))),_tmp8EE))));}{
# 2005
struct Cyc_List_List*_tmp438=*ts;
struct Cyc_List_List*_tmp439=_tmp42B;
while(_tmp438 != 0  && _tmp439 != 0){
# 2009
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp438->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp438->hd);
Cyc_Tcutil_unify((void*)_tmp438->hd,(void*)_tmp439->hd);
_tmp438=_tmp438->tl;
_tmp439=_tmp439->tl;}
# 2015
if(_tmp438 != 0){
const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_Tcutil_terr(loc,((_tmp8F3="too many explicit witness types",_tag_dyneither(_tmp8F3,sizeof(char),32))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
# 2018
*ts=_tmp42B;{
# 2021
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp43D;struct Cyc_Absyn_Exp*_tmp43E;struct _tuple20*_tmp43C=(struct _tuple20*)fields->hd;_tmp43D=_tmp43C->f1;_tmp43E=_tmp43C->f2;{
void*_tmp43F=Cyc_Tcutil_rsubstitute(rgn,_tmp42C,_tmp43D->type);
Cyc_Tcexp_tcExpInitializer(te2,& _tmp43F,_tmp43E);
# 2031
if(!Cyc_Tcutil_coerce_arg(te2,_tmp43E,_tmp43F,& bogus)){
{const char*_tmp8FF;void*_tmp8FE[5];struct Cyc_String_pa_PrintArg_struct _tmp8FD;const char*_tmp8FC;const char*_tmp8FB;struct Cyc_String_pa_PrintArg_struct _tmp8FA;struct Cyc_String_pa_PrintArg_struct _tmp8F9;struct Cyc_String_pa_PrintArg_struct _tmp8F8;struct Cyc_String_pa_PrintArg_struct _tmp8F7;(_tmp8F7.tag=0,((_tmp8F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2035
Cyc_Absynpp_typ2string((void*)_check_null(_tmp43E->topt))),((_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2034
Cyc_Absynpp_typ2string(_tmp43F)),((_tmp8F9.tag=0,((_tmp8F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8FA.tag=0,((_tmp8FA.f1=(struct _dyneither_ptr)(
# 2033
ad->kind == Cyc_Absyn_StructA?(_tmp8FB="struct",_tag_dyneither(_tmp8FB,sizeof(char),7)):((_tmp8FC="union",_tag_dyneither(_tmp8FC,sizeof(char),6)))),((_tmp8FD.tag=0,((_tmp8FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp43D->name),((_tmp8FE[0]=& _tmp8FD,((_tmp8FE[1]=& _tmp8FA,((_tmp8FE[2]=& _tmp8F9,((_tmp8FE[3]=& _tmp8F8,((_tmp8FE[4]=& _tmp8F7,Cyc_Tcutil_terr(_tmp43E->loc,((_tmp8FF="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp8FF,sizeof(char),40))),_tag_dyneither(_tmp8FE,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2036
Cyc_Tcutil_explain_failure();}};}{
# 2040
struct Cyc_List_List*_tmp449=0;
{struct Cyc_List_List*_tmp44A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp44A != 0;_tmp44A=_tmp44A->tl){
struct _tuple0*_tmp902;struct Cyc_List_List*_tmp901;_tmp449=((_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901->hd=((_tmp902=_cycalloc(sizeof(*_tmp902)),((_tmp902->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp42C,(*((struct _tuple0*)_tmp44A->hd)).f1),((_tmp902->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp42C,(*((struct _tuple0*)_tmp44A->hd)).f2),_tmp902)))))),((_tmp901->tl=_tmp449,_tmp901))))));}}
# 2045
_tmp449=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp449);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp449);{
void*_tmp44D=res_typ;_npop_handler(0);return _tmp44D;};};};};};}
# 1964
;_pop_region(rgn);};}
# 2051
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2053
struct _RegionHandle _tmp44F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp44F;_push_region(rgn);{
void*_tmp450=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp452;struct Cyc_List_List*_tmp453;_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp450;if(_tmp451->tag != 12)goto _LL1E9;else{_tmp452=_tmp451->f1;_tmp453=_tmp451->f2;}}_LL1E8:
# 2056
 if(_tmp452 == Cyc_Absyn_UnionA){
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(loc,((_tmp905="expecting struct but found union",_tag_dyneither(_tmp905,sizeof(char),33))),_tag_dyneither(_tmp904,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp453);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp457;struct Cyc_Absyn_Exp*_tmp458;struct _tuple20*_tmp456=(struct _tuple20*)fields->hd;_tmp457=_tmp456->f1;_tmp458=_tmp456->f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp457->type,_tmp458);
if(!Cyc_Tcutil_coerce_arg(te,_tmp458,_tmp457->type,& bogus)){
{const char*_tmp90B;void*_tmp90A[3];struct Cyc_String_pa_PrintArg_struct _tmp909;struct Cyc_String_pa_PrintArg_struct _tmp908;struct Cyc_String_pa_PrintArg_struct _tmp907;(_tmp907.tag=0,((_tmp907.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_typ2string((void*)_check_null(_tmp458->topt))),((_tmp908.tag=0,((_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2066
Cyc_Absynpp_typ2string(_tmp457->type)),((_tmp909.tag=0,((_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp457->name),((_tmp90A[0]=& _tmp909,((_tmp90A[1]=& _tmp908,((_tmp90A[2]=& _tmp907,Cyc_Tcutil_terr(_tmp458->loc,((_tmp90B="field %s of struct expects type %s != %s",_tag_dyneither(_tmp90B,sizeof(char),41))),_tag_dyneither(_tmp90A,sizeof(void*),3)))))))))))))))))));}
# 2068
Cyc_Tcutil_explain_failure();}}
# 2071
goto _LL1E6;};_LL1E9:;_LL1EA: {
const char*_tmp90E;void*_tmp90D;(_tmp90D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp90E="tcAnonStruct: wrong type",_tag_dyneither(_tmp90E,sizeof(char),25))),_tag_dyneither(_tmp90D,sizeof(void*),0)));}_LL1E6:;}{
# 2075
void*_tmp460=ts;_npop_handler(0);return _tmp460;};
# 2053
;_pop_region(rgn);}
# 2079
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2082
struct _RegionHandle _tmp461=_new_region("rgn");struct _RegionHandle*rgn=& _tmp461;_push_region(rgn);
{struct _tuple12 _tmp90F;struct _tuple12 _tmp462=(_tmp90F.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp90F.f2=rgn,_tmp90F)));
struct Cyc_List_List*_tmp463=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp462,tud->tvs);
struct Cyc_List_List*_tmp464=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp463);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp915;struct Cyc_Absyn_DatatypeFieldInfo _tmp914;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp913;void*res=(void*)(
(_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913[0]=((_tmp915.tag=4,((_tmp915.f1=((_tmp914.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp914.targs=_tmp464,_tmp914)))),_tmp915)))),_tmp913))));
# 2090
if(topt != 0){
void*_tmp465=Cyc_Tcutil_compress(*topt);_LL1EC: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp465;if(_tmp466->tag != 4)goto _LL1EE;}_LL1ED:
 Cyc_Tcutil_unify(*topt,res);goto _LL1EB;_LL1EE:;_LL1EF:
 goto _LL1EB;_LL1EB:;}{
# 2096
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;
if(_tmp463 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp463,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp91D;void*_tmp91C[3];struct Cyc_String_pa_PrintArg_struct _tmp91B;struct Cyc_String_pa_PrintArg_struct _tmp91A;const char*_tmp919;struct Cyc_String_pa_PrintArg_struct _tmp918;(_tmp918.tag=0,((_tmp918.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2107
e->topt == 0?(struct _dyneither_ptr)((_tmp919="?",_tag_dyneither(_tmp919,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp91A.tag=0,((_tmp91A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2106
Cyc_Absynpp_typ2string(t)),((_tmp91B.tag=0,((_tmp91B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp91C[0]=& _tmp91B,((_tmp91C[1]=& _tmp91A,((_tmp91C[2]=& _tmp918,Cyc_Tcutil_terr(e->loc,((_tmp91D="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp91D,sizeof(char),82))),_tag_dyneither(_tmp91C,sizeof(void*),3)))))))))))))))))));}
# 2108
Cyc_Tcutil_explain_failure();}}
# 2111
if(es != 0){
const char*_tmp921;void*_tmp920[1];struct Cyc_String_pa_PrintArg_struct _tmp91F;void*_tmp470=(_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2114
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp920[0]=& _tmp91F,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp921="too many arguments for datatype constructor %s",_tag_dyneither(_tmp921,sizeof(char),47))),_tag_dyneither(_tmp920,sizeof(void*),1)))))));_npop_handler(0);return _tmp470;}
if(ts != 0){
const char*_tmp925;void*_tmp924[1];struct Cyc_String_pa_PrintArg_struct _tmp923;void*_tmp474=(_tmp923.tag=0,((_tmp923.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp924[0]=& _tmp923,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp925="too few arguments for datatype constructor %s",_tag_dyneither(_tmp925,sizeof(char),46))),_tag_dyneither(_tmp924,sizeof(void*),1)))))));_npop_handler(0);return _tmp474;}{
void*_tmp475=res;_npop_handler(0);return _tmp475;};};}
# 2083
;_pop_region(rgn);}
# 2122
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp47A=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp481;void*_tmp485;struct Cyc_List_List*_tmp488;union Cyc_Absyn_AggrInfoU _tmp48A;struct Cyc_List_List*_tmp48B;struct Cyc_List_List*_tmp48F;_LL1F1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp47A;if(_tmp47B->tag != 0)goto _LL1F3;}_LL1F2:
 return 1;_LL1F3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp47A;if(_tmp47C->tag != 1)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47A;if(_tmp47D->tag != 2)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47A;if(_tmp47E->tag != 3)goto _LL1F9;}_LL1F8:
 goto _LL1FA;_LL1F9: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47A;if(_tmp47F->tag != 4)goto _LL1FB;}_LL1FA:
 return 0;_LL1FB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47A;if(_tmp480->tag != 5)goto _LL1FD;else{_tmp481=((_tmp480->f1).ptr_atts).nullable;}}_LL1FC:
# 2130
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp481);_LL1FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp47A;if(_tmp482->tag != 6)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp47A;if(_tmp483->tag != 7)goto _LL201;}_LL200:
 return 1;_LL201: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp484=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47A;if(_tmp484->tag != 8)goto _LL203;else{_tmp485=(_tmp484->f1).elt_type;}}_LL202:
 return Cyc_Tcexp_zeroable_type(_tmp485);_LL203: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47A;if(_tmp486->tag != 9)goto _LL205;}_LL204:
 return 0;_LL205: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47A;if(_tmp487->tag != 10)goto _LL207;else{_tmp488=_tmp487->f1;}}_LL206:
# 2136
 for(0;(unsigned int)_tmp488;_tmp488=_tmp488->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp488->hd)).f2))return 0;}
return 1;_LL207: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp489=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47A;if(_tmp489->tag != 11)goto _LL209;else{_tmp48A=(_tmp489->f1).aggr_info;_tmp48B=(_tmp489->f1).targs;}}_LL208: {
# 2140
struct Cyc_Absyn_Aggrdecl*_tmp49C=Cyc_Absyn_get_known_aggrdecl(_tmp48A);
if(_tmp49C->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49C->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp49D=_new_region("r");struct _RegionHandle*r=& _tmp49D;_push_region(r);
{struct Cyc_List_List*_tmp49E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp49C->tvs,_tmp48B);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49C->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp49E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp49F=0;_npop_handler(0);return _tmp49F;}}}{
int _tmp4A0=1;_npop_handler(0);return _tmp4A0;};}
# 2144
;_pop_region(r);};}_LL209: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47A;if(_tmp48C->tag != 13)goto _LL20B;}_LL20A:
# 2149
 return 1;_LL20B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp48D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp47A;if(_tmp48D->tag != 19)goto _LL20D;}_LL20C:
 return 1;_LL20D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47A;if(_tmp48E->tag != 12)goto _LL20F;else{_tmp48F=_tmp48E->f2;}}_LL20E:
# 2152
 for(0;_tmp48F != 0;_tmp48F=_tmp48F->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp48F->hd)->type))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp47A;if(_tmp490->tag != 14)goto _LL211;}_LL210:
 return 1;_LL211: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp491=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47A;if(_tmp491->tag != 17)goto _LL213;}_LL212:
 return 0;_LL213: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47A;if(_tmp492->tag != 16)goto _LL215;}_LL214:
 return 0;_LL215: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp47A;if(_tmp493->tag != 15)goto _LL217;}_LL216:
 return 0;_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47A;if(_tmp494->tag != 26)goto _LL219;}_LL218:
# 2160
 goto _LL21A;_LL219: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp495=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp47A;if(_tmp495->tag != 18)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp47A;if(_tmp496->tag != 20)goto _LL21D;}_LL21C:
 goto _LL21E;_LL21D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp47A;if(_tmp497->tag != 21)goto _LL21F;}_LL21E:
 goto _LL220;_LL21F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp498=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp47A;if(_tmp498->tag != 22)goto _LL221;}_LL220:
 goto _LL222;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp499=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp47A;if(_tmp499->tag != 23)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp47A;if(_tmp49A->tag != 24)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp47A;if(_tmp49B->tag != 25)goto _LL1F0;}_LL226: {
const char*_tmp929;void*_tmp928[1];struct Cyc_String_pa_PrintArg_struct _tmp927;(_tmp927.tag=0,((_tmp927.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp928[0]=& _tmp927,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp929="bad type `%s' in zeroable type",_tag_dyneither(_tmp929,sizeof(char),31))),_tag_dyneither(_tmp928,sizeof(void*),1)))))));}_LL1F0:;}
# 2171
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2173
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2175
if(topt != 0){
void*_tmp4A4=Cyc_Tcutil_compress(*topt);void*_tmp4A6;_LL228: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A4;if(_tmp4A5->tag != 5)goto _LL22A;else{_tmp4A6=(_tmp4A5->f1).elt_typ;}}_LL229:
# 2178
 Cyc_Tcutil_unify(_tmp4A6,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL227;_LL22A:;_LL22B:
 goto _LL227;_LL227:;}{
# 2184
const char*_tmp932;void*_tmp931[2];const char*_tmp930;const char*_tmp92F;struct Cyc_String_pa_PrintArg_struct _tmp92E;struct Cyc_String_pa_PrintArg_struct _tmp92D;(_tmp92D.tag=0,((_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp92E.tag=0,((_tmp92E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp92F="calloc",_tag_dyneither(_tmp92F,sizeof(char),7))):(struct _dyneither_ptr)((_tmp930="malloc",_tag_dyneither(_tmp930,sizeof(char),7))))),((_tmp931[0]=& _tmp92E,((_tmp931[1]=& _tmp92D,Cyc_Tcutil_terr(loc,((_tmp932="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp932,sizeof(char),60))),_tag_dyneither(_tmp931,sizeof(void*),2)))))))))))));};}
# 2188
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2190
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL22C:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL22D:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2196
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2201
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2204
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp935;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp934;void*expected_type=(void*)(
(_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=15,((_tmp935.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp935)))),_tmp934))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp4AD=Cyc_Tcutil_compress(handle_type);void*_tmp4AF;_LL230: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4AD;if(_tmp4AE->tag != 15)goto _LL232;else{_tmp4AF=(void*)_tmp4AE->f1;}}_LL231:
# 2209
 rgn=_tmp4AF;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22F;_LL232:;_LL233:
# 2213
{const char*_tmp939;void*_tmp938[1];struct Cyc_String_pa_PrintArg_struct _tmp937;(_tmp937.tag=0,((_tmp937.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp938[0]=& _tmp937,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp939="expecting region_t type but found %s",_tag_dyneither(_tmp939,sizeof(char),37))),_tag_dyneither(_tmp938,sizeof(void*),1)))))));}
goto _LL22F;_LL22F:;}else{
# 2220
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2228
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,*e);{
# 2237 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp93C;void*_tmp93B;(_tmp93B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp93C="calloc with empty type",_tag_dyneither(_tmp93C,sizeof(char),23))),_tag_dyneither(_tmp93B,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2249
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4B7=er;void*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;_LL235: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4B8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4B7;if(_tmp4B8->tag != 16)goto _LL237;else{_tmp4B9=(void*)_tmp4B8->f1;}}_LL236:
# 2253
 elt_type=_tmp4B9;
{void**_tmp93D;*t=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D[0]=elt_type,_tmp93D))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL234;_LL237: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4BA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B7;if(_tmp4BA->tag != 2)goto _LL239;else{if(_tmp4BA->f1 != Cyc_Absyn_Times)goto _LL239;if(_tmp4BA->f2 == 0)goto _LL239;_tmp4BB=(struct Cyc_Absyn_Exp*)(_tmp4BA->f2)->hd;if((_tmp4BA->f2)->tl == 0)goto _LL239;_tmp4BC=(struct Cyc_Absyn_Exp*)((_tmp4BA->f2)->tl)->hd;if(((_tmp4BA->f2)->tl)->tl != 0)goto _LL239;}}_LL238:
# 2260
{struct _tuple0 _tmp93E;struct _tuple0 _tmp4BF=(_tmp93E.f1=_tmp4BB->r,((_tmp93E.f2=_tmp4BC->r,_tmp93E)));void*_tmp4C1;void*_tmp4C3;_LL23C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4BF.f1;if(_tmp4C0->tag != 16)goto _LL23E;else{_tmp4C1=(void*)_tmp4C0->f1;}}_LL23D:
# 2262
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C1);
elt_type=_tmp4C1;
{void**_tmp93F;*t=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=elt_type,_tmp93F))));}
num_elts=_tmp4BC;
one_elt=0;
goto _LL23B;_LL23E: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4BF.f2;if(_tmp4C2->tag != 16)goto _LL240;else{_tmp4C3=(void*)_tmp4C2->f1;}}_LL23F:
# 2269
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C3);
elt_type=_tmp4C3;
{void**_tmp940;*t=((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940[0]=elt_type,_tmp940))));}
num_elts=_tmp4BB;
one_elt=0;
goto _LL23B;_LL240:;_LL241:
 goto No_sizeof;_LL23B:;}
# 2277
goto _LL234;_LL239:;_LL23A:
# 2279
 No_sizeof: {
# 2282
struct Cyc_Absyn_Exp*_tmp4C6=*e;
{void*_tmp4C7=_tmp4C6->r;struct Cyc_Absyn_Exp*_tmp4C9;_LL243: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C7;if(_tmp4C8->tag != 13)goto _LL245;else{_tmp4C9=_tmp4C8->f2;}}_LL244:
 _tmp4C6=_tmp4C9;goto _LL242;_LL245:;_LL246:
 goto _LL242;_LL242:;}
# 2287
{void*_tmp4CA=Cyc_Tcutil_compress((void*)_check_null(_tmp4C6->topt));void*_tmp4CC;_LL248: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4CA;if(_tmp4CB->tag != 19)goto _LL24A;else{_tmp4CC=(void*)_tmp4CB->f1;}}_LL249:
# 2289
{void*_tmp4CD=Cyc_Tcutil_compress(_tmp4CC);struct Cyc_Absyn_Exp*_tmp4CF;_LL24D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4CD;if(_tmp4CE->tag != 18)goto _LL24F;else{_tmp4CF=_tmp4CE->f1;}}_LL24E:
# 2291
 er=_tmp4CF->r;goto retry_sizeof;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}
# 2294
goto _LL247;_LL24A:;_LL24B:
 goto _LL247;_LL247:;}
# 2297
elt_type=Cyc_Absyn_char_typ;
{void**_tmp941;*t=((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=elt_type,_tmp941))));}
num_elts=*e;
one_elt=0;}
# 2302
goto _LL234;_LL234:;}}
# 2306
*is_fat=!one_elt;
# 2309
{void*_tmp4D1=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4D3;_LL252: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D1;if(_tmp4D2->tag != 11)goto _LL254;else{if((((_tmp4D2->f1).aggr_info).KnownAggr).tag != 2)goto _LL254;_tmp4D3=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4D2->f1).aggr_info).KnownAggr).val);}}_LL253:
# 2311
 if(_tmp4D3->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D3->impl))->exist_vars != 0){
const char*_tmp944;void*_tmp943;(_tmp943=0,Cyc_Tcutil_terr(loc,((_tmp944="malloc with existential types not yet implemented",_tag_dyneither(_tmp944,sizeof(char),50))),_tag_dyneither(_tmp943,sizeof(void*),0)));}
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}{
# 2318
void*(*_tmp4D6)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4D7=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp4D8=Cyc_Tcutil_compress(*topt);union Cyc_Absyn_Constraint*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DC;_LL257: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D8;if(_tmp4D9->tag != 5)goto _LL259;else{_tmp4DA=((_tmp4D9->f1).ptr_atts).nullable;_tmp4DB=((_tmp4D9->f1).ptr_atts).bounds;_tmp4DC=((_tmp4D9->f1).ptr_atts).zero_term;}}_LL258:
# 2323
 _tmp4D7=_tmp4DC;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DA))
_tmp4D6=Cyc_Absyn_star_typ;
# 2328
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DC) && !(*is_calloc)){
{const char*_tmp947;void*_tmp946;(_tmp946=0,Cyc_Tcutil_warn(loc,((_tmp947="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp947,sizeof(char),55))),_tag_dyneither(_tmp946,sizeof(void*),0)));}
*is_calloc=1;}
# 2334
{void*_tmp4DF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4DB);struct Cyc_Absyn_Exp*_tmp4E2;_LL25C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4E0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4DF;if(_tmp4E0->tag != 0)goto _LL25E;}_LL25D:
 goto _LL25B;_LL25E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4E1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4DF;if(_tmp4E1->tag != 1)goto _LL260;else{_tmp4E2=_tmp4E1->f1;}}if(!(!one_elt))goto _LL260;_LL25F: {
# 2337
int _tmp4E3=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4E3  && Cyc_Evexp_same_const_exp(_tmp4E2,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DB),_tmp4D7);}
# 2343
{void*_tmp4E4=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4E6;_LL263: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E4;if(_tmp4E5->tag != 19)goto _LL265;else{_tmp4E6=(void*)_tmp4E5->f1;}}_LL264: {
# 2345
struct Cyc_Absyn_Exp*_tmp4E7=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4E6,0),0,Cyc_Absyn_No_coercion,0);
# 2347
if(Cyc_Evexp_same_const_exp(_tmp4E7,_tmp4E2)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DB),_tmp4D7);}
# 2352
goto _LL262;}_LL265:;_LL266:
 goto _LL262;_LL262:;}
# 2355
goto _LL25B;}_LL260:;_LL261:
 goto _LL25B;_LL25B:;}
# 2358
goto _LL256;_LL259:;_LL25A:
 goto _LL256;_LL256:;}
# 2361
if(!one_elt)_tmp4D6=Cyc_Absyn_dyneither_typ;
return _tmp4D6(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4D7);};};}
# 2366
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2372
struct _RegionHandle _tmp4E8=_new_region("r");struct _RegionHandle*r=& _tmp4E8;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4E9=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp4E9,0,e1);{
void*_tmp4EA=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp4E9,& _tmp4EA,e2);};}{
# 2378
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2381
{void*_tmp4EB=Cyc_Tcutil_compress(t1);_LL268: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4EB;if(_tmp4EC->tag != 8)goto _LL26A;}_LL269:
{const char*_tmp94A;void*_tmp949;(_tmp949=0,Cyc_Tcutil_terr(loc,((_tmp94A="cannot assign to an array",_tag_dyneither(_tmp94A,sizeof(char),26))),_tag_dyneither(_tmp949,sizeof(void*),0)));}goto _LL267;_LL26A:;_LL26B:
 goto _LL267;_LL267:;}{
# 2386
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp94D;void*_tmp94C;(_tmp94C=0,Cyc_Tcutil_terr(loc,((_tmp94D="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp94D,sizeof(char),58))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}
# 2391
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp950;void*_tmp94F;void*_tmp4F3=(_tmp94F=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp950="swap non-lvalue",_tag_dyneither(_tmp950,sizeof(char),16))),_tag_dyneither(_tmp94F,sizeof(void*),0)));_npop_handler(0);return _tmp4F3;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp953;void*_tmp952;void*_tmp4F6=(_tmp952=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp953="swap non-lvalue",_tag_dyneither(_tmp953,sizeof(char),16))),_tag_dyneither(_tmp952,sizeof(void*),0)));_npop_handler(0);return _tmp4F6;}{
# 2396
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp956;void*_tmp955;void*_tmp4F9=(_tmp955=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp956="swap value in zeroterm array",_tag_dyneither(_tmp956,sizeof(char),29))),_tag_dyneither(_tmp955,sizeof(void*),0)));_npop_handler(0);return _tmp4F9;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp959;void*_tmp958;void*_tmp4FC=(_tmp958=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp959="swap value in zeroterm array",_tag_dyneither(_tmp959,sizeof(char),29))),_tag_dyneither(_tmp958,sizeof(void*),0)));_npop_handler(0);return _tmp4FC;}
# 2403
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp95E;void*_tmp95D[2];struct Cyc_String_pa_PrintArg_struct _tmp95C;struct Cyc_String_pa_PrintArg_struct _tmp95B;void*_tmp4FD=(_tmp95B.tag=0,((_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp95C.tag=0,((_tmp95C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp95D[0]=& _tmp95C,((_tmp95D[1]=& _tmp95B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp95E="type mismatch: %s != %s",_tag_dyneither(_tmp95E,sizeof(char),24))),_tag_dyneither(_tmp95D,sizeof(void*),2)))))))))))));
void*_tmp4FE=_tmp4FD;_npop_handler(0);return _tmp4FE;}{
# 2410
void*_tmp503=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp503;};};};};
# 2372
;_pop_region(r);}
# 2414
int Cyc_Tcexp_in_stmt_exp=0;
# 2416
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp504=_new_region("r");struct _RegionHandle*r=& _tmp504;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2423
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2425
while(1){
void*_tmp505=s->r;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Stmt*_tmp509;struct Cyc_Absyn_Stmt*_tmp50A;struct Cyc_Absyn_Decl*_tmp50C;struct Cyc_Absyn_Stmt*_tmp50D;_LL26D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp506=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp505;if(_tmp506->tag != 1)goto _LL26F;else{_tmp507=_tmp506->f1;}}_LL26E: {
# 2429
void*_tmp50E=(void*)_check_null(_tmp507->topt);
if(!Cyc_Tcutil_unify(_tmp50E,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp962;void*_tmp961[1];struct Cyc_String_pa_PrintArg_struct _tmp960;(_tmp960.tag=0,((_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp50E)),((_tmp961[0]=& _tmp960,Cyc_Tcutil_terr(loc,((_tmp962="statement expression returns type %s",_tag_dyneither(_tmp962,sizeof(char),37))),_tag_dyneither(_tmp961,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2435
void*_tmp512=_tmp50E;_npop_handler(0);return _tmp512;};}_LL26F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp508=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp505;if(_tmp508->tag != 2)goto _LL271;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;}}_LL270:
 s=_tmp50A;continue;_LL271: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp50B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp505;if(_tmp50B->tag != 12)goto _LL273;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50B->f2;}}_LL272:
 s=_tmp50D;continue;_LL273:;_LL274: {
# 2439
const char*_tmp965;void*_tmp964;void*_tmp515=(_tmp964=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp965="statement expression must end with expression",_tag_dyneither(_tmp965,sizeof(char),46))),_tag_dyneither(_tmp964,sizeof(void*),0)));_npop_handler(0);return _tmp515;}_LL26C:;}
# 2417
;_pop_region(r);}
# 2444
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
{void*_tmp516=t;struct Cyc_Absyn_Aggrdecl*_tmp518;_LL276: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp516;if(_tmp517->tag != 11)goto _LL278;else{if((((_tmp517->f1).aggr_info).KnownAggr).tag != 2)goto _LL278;_tmp518=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp517->f1).aggr_info).KnownAggr).val);}}_LL277:
# 2448
 if((_tmp518->kind == Cyc_Absyn_UnionA  && _tmp518->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp518->impl))->tagged)goto _LL275;
goto _LL279;_LL278:;_LL279:
# 2451
{const char*_tmp969;void*_tmp968[1];struct Cyc_String_pa_PrintArg_struct _tmp967;(_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp968[0]=& _tmp967,Cyc_Tcutil_terr(loc,((_tmp969="expecting @tagged union but found %s",_tag_dyneither(_tmp969,sizeof(char),37))),_tag_dyneither(_tmp968,sizeof(void*),1)))))));}
goto _LL275;_LL275:;}
# 2454
return Cyc_Absyn_uint_typ;}
# 2458
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2462
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp51C=_new_region("r");struct _RegionHandle*r=& _tmp51C;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp51D=Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te);
if(*rgn_handle != 0){
# 2467
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp96C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp96B;void*expected_type=(void*)(
(_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96C.tag=15,((_tmp96C.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp51D)),_tmp96C)))),_tmp96B))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp51D,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp51E=Cyc_Tcutil_compress(handle_type);void*_tmp520;_LL27B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp51E;if(_tmp51F->tag != 15)goto _LL27D;else{_tmp520=(void*)_tmp51F->f1;}}_LL27C:
# 2472
 rgn=_tmp520;
Cyc_Tcenv_check_rgn_accessible(_tmp51D,loc,rgn);
goto _LL27A;_LL27D:;_LL27E:
# 2476
{const char*_tmp970;void*_tmp96F[1];struct Cyc_String_pa_PrintArg_struct _tmp96E;(_tmp96E.tag=0,((_tmp96E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp96F[0]=& _tmp96E,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp970="expecting region_t type but found %s",_tag_dyneither(_tmp970,sizeof(char),37))),_tag_dyneither(_tmp96F,sizeof(void*),1)))))));}
goto _LL27A;_LL27A:;}else{
# 2483
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2491
void*_tmp526=e1->r;struct Cyc_Core_Opt*_tmp529;struct Cyc_List_List*_tmp52A;struct Cyc_List_List*_tmp52C;_LL280: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp527=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp526;if(_tmp527->tag != 26)goto _LL282;}_LL281: {
# 2495
void*_tmp52F=Cyc_Tcexp_tcExpNoPromote(_tmp51D,topt,e1);
void*_tmp530=Cyc_Tcutil_compress(_tmp52F);void*_tmp532;struct Cyc_Absyn_Tqual _tmp533;struct Cyc_Absyn_Exp*_tmp534;union Cyc_Absyn_Constraint*_tmp535;_LL28D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp531=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp530;if(_tmp531->tag != 8)goto _LL28F;else{_tmp532=(_tmp531->f1).elt_type;_tmp533=(_tmp531->f1).tq;_tmp534=(_tmp531->f1).num_elts;_tmp535=(_tmp531->f1).zero_term;}}_LL28E: {
# 2498
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp534);
void*b;
{void*_tmp536=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp538;_LL292: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp536;if(_tmp537->tag != 19)goto _LL294;else{_tmp538=(void*)_tmp537->f1;}}_LL293:
# 2502
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp973;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp972;b=(void*)((_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972[0]=((_tmp973.tag=1,((_tmp973.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp538,0),0,Cyc_Absyn_No_coercion,0),_tmp973)))),_tmp972))));}
goto _LL291;_LL294:;_LL295:
# 2505
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp976;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp975;b=(void*)((_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=((_tmp976.tag=1,((_tmp976.f1=bnd,_tmp976)))),_tmp975))));}else{
# 2508
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL291:;}{
# 2510
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp97C;struct Cyc_Absyn_PtrInfo _tmp97B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp97A;void*res_typ=(void*)(
(_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97C.tag=5,((_tmp97C.f1=((_tmp97B.elt_typ=_tmp532,((_tmp97B.elt_tq=_tmp533,((_tmp97B.ptr_atts=(
((_tmp97B.ptr_atts).rgn=rgn,(((_tmp97B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp97B.ptr_atts).bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp97B.ptr_atts).zero_term=_tmp535,(((_tmp97B.ptr_atts).ptrloc=0,_tmp97B.ptr_atts)))))))))),_tmp97B)))))),_tmp97C)))),_tmp97A))));
# 2514
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2521
void*_tmp53D=res_typ;_npop_handler(0);return _tmp53D;};};}_LL28F:;_LL290: {
const char*_tmp97F;void*_tmp97E;(_tmp97E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp97F="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp97F,sizeof(char),45))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}_LL28C:;}_LL282: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp526;if(_tmp528->tag != 34)goto _LL284;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}}_LL283:
# 2525
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp982;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp981;e1->r=(void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp982.tag=25,((_tmp982.f1=_tmp52A,_tmp982)))),_tmp981))));}
_tmp52C=_tmp52A;goto _LL285;_LL284: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp526;if(_tmp52B->tag != 25)goto _LL286;else{_tmp52C=_tmp52B->f1;}}_LL285: {
# 2528
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp545=Cyc_Tcutil_compress(*topt);void**_tmp547;struct Cyc_Absyn_Tqual _tmp548;union Cyc_Absyn_Constraint*_tmp549;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp545;if(_tmp546->tag != 5)goto _LL299;else{_tmp547=(void**)&(_tmp546->f1).elt_typ;_tmp548=(_tmp546->f1).elt_tq;_tmp549=((_tmp546->f1).ptr_atts).zero_term;}}_LL298:
# 2534
 elt_typ_opt=_tmp547;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp549);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2540
void*_tmp54A=Cyc_Tcexp_tcArray(_tmp51D,e1->loc,elt_typ_opt,0,zero_term,_tmp52C);
e1->topt=_tmp54A;{
void*res_typ;
{void*_tmp54B=Cyc_Tcutil_compress(_tmp54A);void*_tmp54D;struct Cyc_Absyn_Tqual _tmp54E;struct Cyc_Absyn_Exp*_tmp54F;union Cyc_Absyn_Constraint*_tmp550;_LL29C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54B;if(_tmp54C->tag != 8)goto _LL29E;else{_tmp54D=(_tmp54C->f1).elt_type;_tmp54E=(_tmp54C->f1).tq;_tmp54F=(_tmp54C->f1).num_elts;_tmp550=(_tmp54C->f1).zero_term;}}_LL29D:
# 2545
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp993;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp992;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp991;struct Cyc_Absyn_PtrInfo _tmp990;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp98F;res_typ=(void*)(
(_tmp98F=_cycalloc(sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp993.tag=5,((_tmp993.f1=((_tmp990.elt_typ=_tmp54D,((_tmp990.elt_tq=_tmp54E,((_tmp990.ptr_atts=(
((_tmp990.ptr_atts).rgn=rgn,(((_tmp990.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp990.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=((_tmp992.tag=1,((_tmp992.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp54F),_tmp992)))),_tmp991))))),(((_tmp990.ptr_atts).zero_term=_tmp550,(((_tmp990.ptr_atts).ptrloc=0,_tmp990.ptr_atts)))))))))),_tmp990)))))),_tmp993)))),_tmp98F))));}
# 2550
if(topt != 0){
# 2554
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2560
goto _LL29B;_LL29E:;_LL29F: {
const char*_tmp996;void*_tmp995;(_tmp995=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp996="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp996,sizeof(char),50))),_tag_dyneither(_tmp995,sizeof(void*),0)));}_LL29B:;}{
# 2563
void*_tmp558=res_typ;_npop_handler(0);return _tmp558;};};};}_LL286: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp52D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp526;if(_tmp52D->tag != 0)goto _LL288;else{if(((_tmp52D->f1).String_c).tag != 8)goto _LL288;}}_LL287: {
# 2568
void*_tmp559=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2570
void*_tmp55A=Cyc_Tcexp_tcExp(_tmp51D,& _tmp559,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp999;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp998;void*_tmp55D=Cyc_Absyn_atb_typ(_tmp55A,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp998=_cycalloc(sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=1,((_tmp999.f1=Cyc_Absyn_uint_exp(1,0),_tmp999)))),_tmp998)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp55D;}_LL288: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp526;if(_tmp52E->tag != 0)goto _LL28A;else{if(((_tmp52E->f1).Wstring_c).tag != 9)goto _LL28A;}}_LL289: {
# 2575
void*_tmp55E=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2577
void*_tmp55F=Cyc_Tcexp_tcExp(_tmp51D,& _tmp55E,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp99C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp99B;void*_tmp562=Cyc_Absyn_atb_typ(_tmp55F,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99C.tag=1,((_tmp99C.f1=Cyc_Absyn_uint_exp(1,0),_tmp99C)))),_tmp99B)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp562;}_LL28A:;_LL28B:
# 2584
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp563=Cyc_Tcutil_compress(*topt);void**_tmp565;struct Cyc_Absyn_Tqual _tmp566;_LL2A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp563;if(_tmp564->tag != 5)goto _LL2A3;else{_tmp565=(void**)&(_tmp564->f1).elt_typ;_tmp566=(_tmp564->f1).elt_tq;}}_LL2A2:
# 2590
 topt2=_tmp565;goto _LL2A0;_LL2A3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp563;if(_tmp567->tag != 3)goto _LL2A5;}_LL2A4:
# 2594
 bogus=*topt;
topt2=& bogus;
goto _LL2A0;_LL2A5:;_LL2A6:
 goto _LL2A0;_LL2A0:;}{
# 2600
void*telt=Cyc_Tcexp_tcExp(_tmp51D,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9A2;struct Cyc_Absyn_PtrInfo _tmp9A1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9A0;void*res_typ=(void*)(
(_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A2.tag=5,((_tmp9A2.f1=((_tmp9A1.elt_typ=telt,((_tmp9A1.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9A1.ptr_atts=(
((_tmp9A1.ptr_atts).rgn=rgn,(((_tmp9A1.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9A1.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp9A1.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp9A1.ptr_atts).ptrloc=0,_tmp9A1.ptr_atts)))))))))),_tmp9A1)))))),_tmp9A2)))),_tmp9A0))));
# 2606
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51D,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51D,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2613
void*_tmp568=res_typ;_npop_handler(0);return _tmp568;};};}_LL27F:;};}
# 2464
;_pop_region(r);}
# 2620
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp56C=t;void*_tmp56E;struct Cyc_Absyn_Tqual _tmp56F;struct Cyc_Absyn_Exp*_tmp570;union Cyc_Absyn_Constraint*_tmp571;_LL2A8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56C;if(_tmp56D->tag != 8)goto _LL2AA;else{_tmp56E=(_tmp56D->f1).elt_type;_tmp56F=(_tmp56D->f1).tq;_tmp570=(_tmp56D->f1).num_elts;_tmp571=(_tmp56D->f1).zero_term;}}_LL2A9: {
# 2624
void*_tmp573;struct _tuple14 _tmp572=Cyc_Tcutil_addressof_props(te,e);_tmp573=_tmp572.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9A5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9A4;void*_tmp574=_tmp570 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=1,((_tmp9A5.f1=_tmp570,_tmp9A5)))),_tmp9A4))));
t=Cyc_Absyn_atb_typ(_tmp56E,_tmp573,_tmp56F,_tmp574,_tmp571);
e->topt=t;
return t;};}_LL2AA:;_LL2AB:
# 2630
 return t;_LL2A7:;}
# 2637
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2641
struct _RegionHandle _tmp577=_new_region("temp");struct _RegionHandle*temp=& _tmp577;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2647
const char*_tmp9A8;void*_tmp9A7;(_tmp9A7=0,Cyc_Tcutil_terr(e->loc,((_tmp9A8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9A8,sizeof(char),49))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));}{
# 2650
void*_tmp57A=e->r;_LL2AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp57B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp57A;if(_tmp57B->tag != 25)goto _LL2AF;}_LL2AE:
 goto _LL2B0;_LL2AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp57A;if(_tmp57C->tag != 26)goto _LL2B1;}_LL2B0:
 goto _LL2B2;_LL2B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57A;if(_tmp57D->tag != 0)goto _LL2B3;else{if(((_tmp57D->f1).Wstring_c).tag != 9)goto _LL2B3;}}_LL2B2:
 goto _LL2B4;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57A;if(_tmp57E->tag != 0)goto _LL2B5;else{if(((_tmp57E->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4: {
void*_tmp57F=t;_npop_handler(0);return _tmp57F;}_LL2B5:;_LL2B6:
# 2656
 t=Cyc_Tcutil_compress(t);{
void*_tmp580=t;void*_tmp582;struct Cyc_Absyn_Tqual _tmp583;struct Cyc_Absyn_Exp*_tmp584;union Cyc_Absyn_Constraint*_tmp585;_LL2B8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 8)goto _LL2BA;else{_tmp582=(_tmp581->f1).elt_type;_tmp583=(_tmp581->f1).tq;_tmp584=(_tmp581->f1).num_elts;_tmp585=(_tmp581->f1).zero_term;}}_LL2B9: {
# 2659
void*_tmp587;struct _tuple14 _tmp586=Cyc_Tcutil_addressof_props(te,e);_tmp587=_tmp586.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9AB;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9AA;void*b=_tmp584 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp9AA=_cycalloc(sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=1,((_tmp9AB.f1=_tmp584,_tmp9AB)))),_tmp9AA))));
t=Cyc_Absyn_atb_typ(_tmp582,_tmp587,_tmp583,b,_tmp585);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp588=t;_npop_handler(0);return _tmp588;};};}_LL2BA:;_LL2BB: {
# 2666
void*_tmp58B=t;_npop_handler(0);return _tmp58B;}_LL2B7:;};_LL2AC:;};
# 2641
;_pop_region(temp);}
# 2678 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp58C=e->r;struct Cyc_Absyn_Exp*_tmp58E;_LL2BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp58C;if(_tmp58D->tag != 11)goto _LL2BF;else{_tmp58E=_tmp58D->f1;}}_LL2BE:
# 2682
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp58E);
_tmp58E->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp58E->topt),0);
e->topt=_tmp58E->topt;
goto _LL2BC;_LL2BF:;_LL2C0:
# 2688
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2691
{void*_tmp58F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp591;struct Cyc_Absyn_Tqual _tmp592;void*_tmp593;union Cyc_Absyn_Constraint*_tmp594;union Cyc_Absyn_Constraint*_tmp595;union Cyc_Absyn_Constraint*_tmp596;_LL2C2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp590=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58F;if(_tmp590->tag != 5)goto _LL2C4;else{_tmp591=(_tmp590->f1).elt_typ;_tmp592=(_tmp590->f1).elt_tq;_tmp593=((_tmp590->f1).ptr_atts).rgn;_tmp594=((_tmp590->f1).ptr_atts).nullable;_tmp595=((_tmp590->f1).ptr_atts).bounds;_tmp596=((_tmp590->f1).ptr_atts).zero_term;}}_LL2C3:
# 2693
{void*_tmp597=Cyc_Tcutil_compress(_tmp591);struct Cyc_List_List*_tmp599;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp59B;void*_tmp59C;struct Cyc_List_List*_tmp59D;int _tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp5A1;_LL2C7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp597;if(_tmp598->tag != 9)goto _LL2C9;else{_tmp599=(_tmp598->f1).tvars;_tmp59A=(_tmp598->f1).effect;_tmp59B=(_tmp598->f1).ret_tqual;_tmp59C=(_tmp598->f1).ret_typ;_tmp59D=(_tmp598->f1).args;_tmp59E=(_tmp598->f1).c_varargs;_tmp59F=(_tmp598->f1).cyc_varargs;_tmp5A0=(_tmp598->f1).rgn_po;_tmp5A1=(_tmp598->f1).attributes;}}_LL2C8:
# 2695
 if(_tmp599 != 0){
struct _RegionHandle _tmp5A2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5A2;_push_region(rgn);
{struct _tuple12 _tmp9AC;struct _tuple12 _tmp5A3=(_tmp9AC.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9AC.f2=rgn,_tmp9AC)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5A3,_tmp599);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2704
_tmp5A0=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5A0);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5A0);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9B2;struct Cyc_Absyn_FnInfo _tmp9B1;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9B0;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B2.tag=9,((_tmp9B2.f1=((_tmp9B1.tvars=0,((_tmp9B1.effect=_tmp59A,((_tmp9B1.ret_tqual=_tmp59B,((_tmp9B1.ret_typ=_tmp59C,((_tmp9B1.args=_tmp59D,((_tmp9B1.c_varargs=_tmp59E,((_tmp9B1.cyc_varargs=_tmp59F,((_tmp9B1.rgn_po=0,((_tmp9B1.attributes=_tmp5A1,_tmp9B1)))))))))))))))))),_tmp9B2)))),_tmp9B0)))));
# 2710
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9B8;struct Cyc_Absyn_PtrInfo _tmp9B7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A4=(_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B8.tag=5,((_tmp9B8.f1=((_tmp9B7.elt_typ=ftyp,((_tmp9B7.elt_tq=_tmp592,((_tmp9B7.ptr_atts=(((_tmp9B7.ptr_atts).rgn=_tmp593,(((_tmp9B7.ptr_atts).nullable=_tmp594,(((_tmp9B7.ptr_atts).bounds=_tmp595,(((_tmp9B7.ptr_atts).zero_term=_tmp596,(((_tmp9B7.ptr_atts).ptrloc=0,_tmp9B7.ptr_atts)))))))))),_tmp9B7)))))),_tmp9B8)))),_tmp9B6)));
# 2712
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9BB;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9BA;e->r=(void*)((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BB.tag=12,((_tmp9BB.f1=_tmp5A5,((_tmp9BB.f2=ts,_tmp9BB)))))),_tmp9BA))));}
e->topt=(void*)_tmp5A4;};}
# 2697
;_pop_region(rgn);}
# 2717
goto _LL2C6;_LL2C9:;_LL2CA:
 goto _LL2C6;_LL2C6:;}
# 2720
goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}
# 2723
goto _LL2BC;_LL2BC:;}
# 2725
return(void*)_check_null(e->topt);}
# 2733
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2735
struct _RegionHandle _tmp5AF=_new_region("r");struct _RegionHandle*r=& _tmp5AF;_push_region(r);
{struct Cyc_List_List*_tmp5B0=0;
# 2738
{void*_tmp5B1=fn_exp->r;struct Cyc_List_List*_tmp5B3;_LL2CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B2=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5B1;if(_tmp5B2->tag != 9)goto _LL2CE;else{_tmp5B3=_tmp5B2->f2;}}_LL2CD:
# 2740
{void*_tmp5B4=e->r;struct Cyc_List_List*_tmp5B6;_LL2D1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5B4;if(_tmp5B5->tag != 9)goto _LL2D3;else{_tmp5B6=_tmp5B5->f2;}}_LL2D2: {
# 2742
struct Cyc_List_List*_tmp5B7=alias_arg_exps;
int _tmp5B8=0;
while(_tmp5B7 != 0){
while(_tmp5B8 != (int)_tmp5B7->hd){
if(_tmp5B3 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp9C5;struct Cyc_Int_pa_PrintArg_struct _tmp9C4;void*_tmp9C3[2];const char*_tmp9C2;void*_tmp9C1;(_tmp9C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp9C5.tag=1,((_tmp9C5.f1=(unsigned long)((int)_tmp5B7->hd),((_tmp9C4.tag=1,((_tmp9C4.f1=(unsigned long)_tmp5B8,((_tmp9C3[0]=& _tmp9C4,((_tmp9C3[1]=& _tmp9C5,Cyc_aprintf(((_tmp9C2="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9C2,sizeof(char),36))),_tag_dyneither(_tmp9C3,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9C1,sizeof(void*),0)));}
# 2749
++ _tmp5B8;
_tmp5B3=_tmp5B3->tl;
_tmp5B6=((struct Cyc_List_List*)_check_null(_tmp5B6))->tl;}{
# 2754
struct Cyc_Absyn_Decl*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C0;struct _tuple11 _tmp5BE=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B3))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B6))->hd)->topt)));_tmp5BF=_tmp5BE.f1;_tmp5C0=_tmp5BE.f2;
_tmp5B3->hd=(void*)_tmp5C0;
{struct Cyc_List_List*_tmp9C6;_tmp5B0=((_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->hd=_tmp5BF,((_tmp9C6->tl=_tmp5B0,_tmp9C6))))));}
_tmp5B7=_tmp5B7->tl;};}
# 2759
goto _LL2D0;}_LL2D3:;_LL2D4: {
const char*_tmp9C9;void*_tmp9C8;(_tmp9C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C9="not a function call!",_tag_dyneither(_tmp9C9,sizeof(char),21))),_tag_dyneither(_tmp9C8,sizeof(void*),0)));}_LL2D0:;}
# 2762
goto _LL2CB;_LL2CE:;_LL2CF: {
const char*_tmp9CC;void*_tmp9CB;(_tmp9CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9CC="not a function call!",_tag_dyneither(_tmp9CC,sizeof(char),21))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));}_LL2CB:;}
# 2767
while(_tmp5B0 != 0){
struct Cyc_Absyn_Decl*_tmp5C6=(struct Cyc_Absyn_Decl*)_tmp5B0->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5C6,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5B0=_tmp5B0->tl;}
# 2774
e->topt=0;
e->r=fn_exp->r;}
# 2736
;_pop_region(r);}
# 2781
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2785
{void*_tmp5C7=e->r;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_Absyn_VarargCallInfo**_tmp5CD;int*_tmp5CE;struct Cyc_Core_Opt*_tmp5D0;struct Cyc_List_List*_tmp5D1;union Cyc_Absyn_Cnst*_tmp5D3;struct _tuple1*_tmp5D5;void**_tmp5D6;enum Cyc_Absyn_Primop _tmp5D8;struct Cyc_List_List*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DB;enum Cyc_Absyn_Incrementor _tmp5DC;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Core_Opt*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_List_List*_tmp5F0;struct Cyc_Absyn_VarargCallInfo**_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_List_List*_tmp5F6;void*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F9;enum Cyc_Absyn_Coercion*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_Exp**_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FF;struct Cyc_Absyn_Exp*_tmp601;void*_tmp603;void*_tmp605;struct Cyc_List_List*_tmp606;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Exp*_tmp60A;struct _dyneither_ptr*_tmp60B;int*_tmp60C;int*_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;struct _dyneither_ptr*_tmp610;int*_tmp611;int*_tmp612;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_List_List*_tmp617;struct _tuple9*_tmp619;struct Cyc_List_List*_tmp61A;struct Cyc_List_List*_tmp61C;struct Cyc_Absyn_Stmt*_tmp61E;struct Cyc_Absyn_Vardecl*_tmp620;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_Absyn_Exp*_tmp622;int*_tmp623;struct _tuple1**_tmp625;struct Cyc_List_List**_tmp626;struct Cyc_List_List*_tmp627;struct Cyc_Absyn_Aggrdecl**_tmp628;void*_tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_List_List*_tmp62D;struct Cyc_Absyn_Datatypedecl*_tmp62E;struct Cyc_Absyn_Datatypefield*_tmp62F;struct _tuple1**_tmp631;struct Cyc_Absyn_Enumdecl*_tmp632;struct Cyc_Absyn_Enumfield*_tmp633;struct _tuple1**_tmp635;void*_tmp636;struct Cyc_Absyn_Enumfield*_tmp637;int*_tmp639;struct Cyc_Absyn_Exp**_tmp63A;void***_tmp63B;struct Cyc_Absyn_Exp**_tmp63C;int*_tmp63D;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp642;struct _dyneither_ptr*_tmp643;void*_tmp645;_LL2D6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5C8->tag != 11)goto _LL2D8;else{_tmp5C9=_tmp5C8->f1;}}_LL2D7:
# 2790
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5C9);
return;_LL2D8:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5CA=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5CA->tag != 9)goto _LL2DA;else{_tmp5CB=_tmp5CA->f1;_tmp5CC=_tmp5CA->f2;_tmp5CD=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5CA->f3;_tmp5CE=(int*)& _tmp5CA->f4;}}if(!(!(*_tmp5CE)))goto _LL2DA;_LL2D9:
# 2796
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5CB,_tmp5CC)){
*_tmp5CE=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2800
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2803
return;_LL2DA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5CF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5CF->tag != 34)goto _LL2DC;else{_tmp5D0=_tmp5CF->f1;_tmp5D1=_tmp5CF->f2;}}_LL2DB:
# 2807
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5D1);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5D2->tag != 0)goto _LL2DE;else{_tmp5D3=(union Cyc_Absyn_Cnst*)& _tmp5D2->f1;}}_LL2DD:
# 2812
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5D3,e);goto _LL2D5;_LL2DE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5D4->tag != 1)goto _LL2E0;else{_tmp5D5=_tmp5D4->f1;_tmp5D6=(void**)((void**)& _tmp5D4->f2);}}_LL2DF:
# 2814
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5D5,_tmp5D6);goto _LL2D5;_LL2E0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5D7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5D7->tag != 2)goto _LL2E2;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=_tmp5D7->f2;}}_LL2E1:
# 2816
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5D8,_tmp5D9);goto _LL2D5;_LL2E2: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5DA=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5DA->tag != 4)goto _LL2E4;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;}}_LL2E3:
# 2818
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5DB,_tmp5DC);goto _LL2D5;_LL2E4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5DD=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5DD->tag != 3)goto _LL2E6;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;_tmp5E0=_tmp5DD->f3;}}_LL2E5:
# 2820
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5DE,_tmp5DF,_tmp5E0);goto _LL2D5;_LL2E6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5E1=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5E1->tag != 5)goto _LL2E8;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;_tmp5E4=_tmp5E1->f3;}}_LL2E7:
# 2822
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5E2,_tmp5E3,_tmp5E4);goto _LL2D5;_LL2E8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5E5=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5E5->tag != 6)goto _LL2EA;else{_tmp5E6=_tmp5E5->f1;_tmp5E7=_tmp5E5->f2;}}_LL2E9:
# 2824
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5E6,_tmp5E7);goto _LL2D5;_LL2EA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5E8=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5E8->tag != 7)goto _LL2EC;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E8->f2;}}_LL2EB:
# 2826
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5E9,_tmp5EA);goto _LL2D5;_LL2EC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5EB->tag != 8)goto _LL2EE;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;}}_LL2ED:
# 2828
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5EC,_tmp5ED);goto _LL2D5;_LL2EE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5EE->tag != 9)goto _LL2F0;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;_tmp5F1=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5EE->f3;}}_LL2EF: {
# 2834
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp647;_push_handler(& _tmp647);{int _tmp649=0;if(setjmp(_tmp647.handler))_tmp649=1;if(!_tmp649){
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp648=(void*)_exn_thrown;void*_tmp64B=_tmp648;struct _dyneither_ptr _tmp64D;void*_tmp64E;_LL327: {struct Cyc_Core_Failure_exn_struct*_tmp64C=(struct Cyc_Core_Failure_exn_struct*)_tmp64B;if(_tmp64C->tag != Cyc_Core_Failure)goto _LL329;else{_tmp64D=_tmp64C->f1;}}_LL328:
# 2841
 ok=0;
fn_exp=e;
goto _LL326;_LL329: _tmp64E=_tmp64B;_LL32A:(void)_throw(_tmp64E);_LL326:;}};}
# 2845
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5EF,_tmp5F0,_tmp5F1,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2852
goto _LL2D5;}_LL2F0: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5F2=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5F2->tag != 10)goto _LL2F2;else{_tmp5F3=_tmp5F2->f1;}}_LL2F1:
# 2854
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5F3);goto _LL2D5;_LL2F2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5F4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5F4->tag != 12)goto _LL2F4;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;}}_LL2F3:
# 2856
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5F5,_tmp5F6);goto _LL2D5;_LL2F4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5F7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5F7->tag != 13)goto _LL2F6;else{_tmp5F8=(void*)_tmp5F7->f1;_tmp5F9=_tmp5F7->f2;_tmp5FA=(enum Cyc_Absyn_Coercion*)& _tmp5F7->f4;}}_LL2F5:
# 2858
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5F8,_tmp5F9,_tmp5FA);goto _LL2D5;_LL2F6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5FB->tag != 14)goto _LL2F8;else{_tmp5FC=_tmp5FB->f1;}}_LL2F7:
# 2860
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp5FC);goto _LL2D5;_LL2F8: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp5FD->tag != 15)goto _LL2FA;else{_tmp5FE=(struct Cyc_Absyn_Exp**)& _tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}_LL2F9:
# 2862
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5FE,e,_tmp5FF);goto _LL2D5;_LL2FA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp600->tag != 17)goto _LL2FC;else{_tmp601=_tmp600->f1;}}_LL2FB: {
# 2864
void*_tmp64F=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp601);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp64F);goto _LL2D5;}_LL2FC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp602->tag != 16)goto _LL2FE;else{_tmp603=(void*)_tmp602->f1;}}_LL2FD:
# 2867
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp603);goto _LL2D5;_LL2FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp604=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp604->tag != 18)goto _LL300;else{_tmp605=(void*)_tmp604->f1;_tmp606=_tmp604->f2;}}_LL2FF:
# 2869
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp605,_tmp606);goto _LL2D5;_LL300: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp607=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp607->tag != 19)goto _LL302;else{_tmp608=_tmp607->f1;}}_LL301:
# 2871
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp608);goto _LL2D5;_LL302: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp609=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp609->tag != 20)goto _LL304;else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=(int*)& _tmp609->f3;_tmp60D=(int*)& _tmp609->f4;}}_LL303:
# 2873
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp60A,_tmp60B,_tmp60C,_tmp60D);goto _LL2D5;_LL304: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp60E->tag != 21)goto _LL306;else{_tmp60F=_tmp60E->f1;_tmp610=_tmp60E->f2;_tmp611=(int*)& _tmp60E->f3;_tmp612=(int*)& _tmp60E->f4;}}_LL305:
# 2875
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp60F,_tmp610,_tmp611,_tmp612);goto _LL2D5;_LL306: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp613->tag != 22)goto _LL308;else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;}}_LL307:
# 2877
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp614,_tmp615);goto _LL2D5;_LL308: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp616=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp616->tag != 23)goto _LL30A;else{_tmp617=_tmp616->f1;}}_LL309:
# 2879
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp617);goto _LL2D5;_LL30A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp618=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp618->tag != 24)goto _LL30C;else{_tmp619=_tmp618->f1;_tmp61A=_tmp618->f2;}}_LL30B:
# 2881
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp619,_tmp61A);goto _LL2D5;_LL30C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp61B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp61B->tag != 25)goto _LL30E;else{_tmp61C=_tmp61B->f1;}}_LL30D: {
# 2885
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp650=Cyc_Tcutil_compress(*topt);void**_tmp652;struct Cyc_Absyn_Tqual*_tmp653;union Cyc_Absyn_Constraint*_tmp654;_LL32C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp651=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp650;if(_tmp651->tag != 8)goto _LL32E;else{_tmp652=(void**)&(_tmp651->f1).elt_type;_tmp653=(struct Cyc_Absyn_Tqual*)&(_tmp651->f1).tq;_tmp654=(_tmp651->f1).zero_term;}}_LL32D:
# 2891
 elt_topt=_tmp652;
elt_tqopt=_tmp653;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp654);
goto _LL32B;_LL32E:;_LL32F:
 goto _LL32B;_LL32B:;}
# 2898
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp61C);goto _LL2D5;}_LL30E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp61D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp61D->tag != 35)goto _LL310;else{_tmp61E=_tmp61D->f1;}}_LL30F:
# 2900
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp61E);goto _LL2D5;_LL310: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp61F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp61F->tag != 26)goto _LL312;else{_tmp620=_tmp61F->f1;_tmp621=_tmp61F->f2;_tmp622=_tmp61F->f3;_tmp623=(int*)& _tmp61F->f4;}}_LL311:
# 2902
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp620,_tmp621,_tmp622,_tmp623);goto _LL2D5;_LL312: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp624->tag != 27)goto _LL314;else{_tmp625=(struct _tuple1**)& _tmp624->f1;_tmp626=(struct Cyc_List_List**)& _tmp624->f2;_tmp627=_tmp624->f3;_tmp628=(struct Cyc_Absyn_Aggrdecl**)& _tmp624->f4;}}_LL313:
# 2904
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp625,_tmp626,_tmp627,_tmp628);goto _LL2D5;_LL314: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp629=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp629->tag != 28)goto _LL316;else{_tmp62A=(void*)_tmp629->f1;_tmp62B=_tmp629->f2;}}_LL315:
# 2906
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp62A,_tmp62B);goto _LL2D5;_LL316: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp62C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp62C->tag != 29)goto _LL318;else{_tmp62D=_tmp62C->f1;_tmp62E=_tmp62C->f2;_tmp62F=_tmp62C->f3;}}_LL317:
# 2908
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp62D,_tmp62E,_tmp62F);goto _LL2D5;_LL318: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp630->tag != 30)goto _LL31A;else{_tmp631=(struct _tuple1**)& _tmp630->f1;_tmp632=_tmp630->f2;_tmp633=_tmp630->f3;}}_LL319:
# 2910
*_tmp631=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp633))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9CF;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9CE;t=(void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=13,((_tmp9CF.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp632))->name,((_tmp9CF.f2=_tmp632,_tmp9CF)))))),_tmp9CE))));}goto _LL2D5;_LL31A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp634=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp634->tag != 31)goto _LL31C;else{_tmp635=(struct _tuple1**)& _tmp634->f1;_tmp636=(void*)_tmp634->f2;_tmp637=_tmp634->f3;}}_LL31B:
# 2913
*_tmp635=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp637))->name;
t=_tmp636;goto _LL2D5;_LL31C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp638=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp638->tag != 32)goto _LL31E;else{_tmp639=(int*)&(_tmp638->f1).is_calloc;_tmp63A=(struct Cyc_Absyn_Exp**)&(_tmp638->f1).rgn;_tmp63B=(void***)&(_tmp638->f1).elt_type;_tmp63C=(struct Cyc_Absyn_Exp**)&(_tmp638->f1).num_elts;_tmp63D=(int*)&(_tmp638->f1).fat_result;}}_LL31D:
# 2916
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp63A,_tmp63B,_tmp63C,_tmp639,_tmp63D);goto _LL2D5;_LL31E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp63E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp63E->tag != 33)goto _LL320;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;}}_LL31F:
# 2918
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp63F,_tmp640);goto _LL2D5;_LL320: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp641=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp641->tag != 36)goto _LL322;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;}}_LL321:
# 2920
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp642,_tmp643);goto _LL2D5;_LL322: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp644->tag != 37)goto _LL324;else{_tmp645=(void*)_tmp644->f1;}}_LL323:
# 2922
 if(!te->allow_valueof){
const char*_tmp9D2;void*_tmp9D1;(_tmp9D1=0,Cyc_Tcutil_terr(e->loc,((_tmp9D2="valueof(-) can only occur within types",_tag_dyneither(_tmp9D2,sizeof(char),39))),_tag_dyneither(_tmp9D1,sizeof(void*),0)));}
# 2930
t=Cyc_Absyn_sint_typ;
goto _LL2D5;_LL324: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp646=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5C7;if(_tmp646->tag != 38)goto _LL2D5;}_LL325:
# 2933
{const char*_tmp9D5;void*_tmp9D4;(_tmp9D4=0,Cyc_Tcutil_terr(e->loc,((_tmp9D5="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9D5,sizeof(char),50))),_tag_dyneither(_tmp9D4,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2D5:;}
# 2936
e->topt=t;}
