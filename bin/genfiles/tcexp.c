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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 81
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 214 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 279
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct _dyneither_ptr f1;void*f2;};
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
{void*_tmp62F;(_tmp62F=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp62F,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp630;return(_tmp630=_cycalloc(sizeof(*_tmp630)),((_tmp630->f1=0,((_tmp630->f2=e,_tmp630)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp634;void*_tmp633[1];struct Cyc_String_pa_PrintArg_struct _tmp632;(_tmp632.tag=0,((_tmp632.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp633[0]=& _tmp632,Cyc_Tcutil_terr(e1->loc,((_tmp634="undeclared identifier %s",_tag_dyneither(_tmp634,sizeof(char),25))),_tag_dyneither(_tmp633,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp638;void*_tmp637[1];struct Cyc_String_pa_PrintArg_struct _tmp636;(_tmp636.tag=0,((_tmp636.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp637[0]=& _tmp636,Cyc_Tcutil_terr(e->loc,((_tmp638="%s is a constant, not a function",_tag_dyneither(_tmp638,sizeof(char),33))),_tag_dyneither(_tmp637,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp63B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp63A;e->r=(void*)((_tmp63A=_cycalloc(sizeof(*_tmp63A)),((_tmp63A[0]=((_tmp63B.tag=29,((_tmp63B.f1=es,((_tmp63B.f2=_tmp10,((_tmp63B.f3=_tmp11,_tmp63B)))))))),_tmp63A))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp63E;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp63D;e->r=(void*)((_tmp63D=_cycalloc(sizeof(*_tmp63D)),((_tmp63D[0]=((_tmp63E.tag=27,((_tmp63E.f1=_tmp13->name,((_tmp63E.f2=0,((_tmp63E.f3=_tmp22,((_tmp63E.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp13,_tmp63E)))))))))),_tmp63D))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp642;void*_tmp641[1];struct Cyc_String_pa_PrintArg_struct _tmp640;(_tmp640.tag=0,((_tmp640.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp641[0]=& _tmp640,Cyc_Tcutil_terr(e->loc,((_tmp642="%s is an enum constructor, not a function",_tag_dyneither(_tmp642,sizeof(char),42))),_tag_dyneither(_tmp641,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp646;void*_tmp645[1];struct Cyc_String_pa_PrintArg_struct _tmp644;(_tmp644.tag=0,((_tmp644.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp645[0]=& _tmp644,Cyc_Tcutil_terr(e1->loc,((_tmp646="undeclared identifier %s",_tag_dyneither(_tmp646,sizeof(char),25))),_tag_dyneither(_tmp645,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp649;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp648;e->r=(void*)((_tmp648=_cycalloc(sizeof(*_tmp648)),((_tmp648[0]=((_tmp649.tag=25,((_tmp649.f1=des,_tmp649)))),_tmp648))));}
return;}{
# 125
void*t=*topt;
void*_tmp33=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp64C;void*_tmp64B;(_tmp64B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp64C="struct type not properly set",_tag_dyneither(_tmp64C,sizeof(char),29))),_tag_dyneither(_tmp64B,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp64F;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp64E;e->r=(void*)((_tmp64E=_cycalloc(sizeof(*_tmp64E)),((_tmp64E[0]=((_tmp64F.tag=27,((_tmp64F.f1=_tmp3B->name,((_tmp64F.f2=0,((_tmp64F.f3=des,((_tmp64F.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp3B,_tmp64F)))))))))),_tmp64E))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp652;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp651;e->r=(void*)((_tmp651=_cycalloc(sizeof(*_tmp651)),((_tmp651[0]=((_tmp652.tag=25,((_tmp652.f1=des,_tmp652)))),_tmp651))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp655;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp654;e->r=(void*)((_tmp654=_cycalloc(sizeof(*_tmp654)),((_tmp654[0]=((_tmp655.tag=28,((_tmp655.f1=(void*)t,((_tmp655.f2=des,_tmp655)))))),_tmp654))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp658;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp657;e->r=(void*)((_tmp657=_cycalloc(sizeof(*_tmp657)),((_tmp657[0]=((_tmp658.tag=25,((_tmp658.f1=des,_tmp658)))),_tmp657))));}goto _LL17;_LL17:;};}
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
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp65D;void*_tmp65C[2];struct Cyc_String_pa_PrintArg_struct _tmp65B;struct Cyc_String_pa_PrintArg_struct _tmp65A;(_tmp65A.tag=0,((_tmp65A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp65B.tag=0,((_tmp65B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp65C[0]=& _tmp65B,((_tmp65C[1]=& _tmp65A,Cyc_Tcutil_terr(e->loc,((_tmp65D="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp65D,sizeof(char),53))),_tag_dyneither(_tmp65C,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4A=e->r;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp65E;struct _tuple0 _tmp4F=(_tmp65E.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp65E.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp65E)));void*_tmp51;void*_tmp53;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f1;if(_tmp50->tag != 15)goto _LL2D;else{_tmp51=(void*)_tmp50->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f2;if(_tmp52->tag != 15)goto _LL2D;else{_tmp53=(void*)_tmp52->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp661;struct Cyc_Tcexp_TestEnv _tmp660;return(_tmp660.eq=((_tmp661=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp661)),((_tmp661->f1=_tmp51,((_tmp661->f2=_tmp53,_tmp661)))))),((_tmp660.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp660)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp662;return(_tmp662.eq=0,((_tmp662.isTrue=0,_tmp662)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp66C;struct Cyc_Absyn_PtrAtts _tmp66B;struct Cyc_Absyn_PtrInfo _tmp66A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp669;t=(void*)((_tmp669=_cycalloc(sizeof(*_tmp669)),((_tmp669[0]=((_tmp66C.tag=5,((_tmp66C.f1=((_tmp66A.elt_typ=_tmp69,((_tmp66A.elt_tq=_tmp6A,((_tmp66A.ptr_atts=(
(_tmp66B.rgn=_tmp6B,((_tmp66B.nullable=Cyc_Absyn_true_conref,((_tmp66B.bounds=_tmp6D,((_tmp66B.zero_term=_tmp6E,((_tmp66B.ptrloc=0,_tmp66B)))))))))),_tmp66A)))))),_tmp66C)))),_tmp669))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp66F;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp66E;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp77=(_tmp66E=_cycalloc(sizeof(*_tmp66E)),((_tmp66E[0]=((_tmp66F.tag=18,((_tmp66F.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp66F)))),_tmp66E)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp672;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp671;t=(void*)((_tmp671=_cycalloc(sizeof(*_tmp671)),((_tmp671[0]=((_tmp672.tag=19,((_tmp672.f1=(void*)((void*)_tmp77),_tmp672)))),_tmp671))));}
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
elen->topt=(void*)Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp675;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp674;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7C=(_tmp674=_cycalloc(sizeof(*_tmp674)),((_tmp674[0]=((_tmp675.tag=1,((_tmp675.f1=elen,_tmp675)))),_tmp674)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp7C,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp7D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Tqual _tmp7F;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7D;if(_tmp7E->tag != 8)goto _LL5C;else{_tmp7F=(_tmp7E->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp7F,(struct Cyc_Absyn_Exp*)elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7D;if(_tmp80->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=(void*)t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp7C,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=(void*)t;
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp67F;struct Cyc_Absyn_PtrAtts _tmp67E;struct Cyc_Absyn_PtrInfo _tmp67D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67C;return(void*)((_tmp67C=_cycalloc(sizeof(*_tmp67C)),((_tmp67C[0]=((_tmp67F.tag=5,((_tmp67F.f1=((_tmp67D.elt_typ=_tmp85,((_tmp67D.elt_tq=_tmp86,((_tmp67D.ptr_atts=((_tmp67E.rgn=_tmp87,((_tmp67E.nullable=Cyc_Absyn_true_conref,((_tmp67E.bounds=_tmp89,((_tmp67E.zero_term=_tmp8A,((_tmp67E.ptrloc=0,_tmp67E)))))))))),_tmp67D)))))),_tmp67F)))),_tmp67C))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8F=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp692;struct Cyc_Absyn_PtrAtts _tmp691;struct Cyc_Core_Opt*_tmp690;struct Cyc_Core_Opt*_tmp68F;struct Cyc_Absyn_PtrInfo _tmp68E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68D;t=(void*)((_tmp68D=_cycalloc(sizeof(*_tmp68D)),((_tmp68D[0]=((_tmp692.tag=5,((_tmp692.f1=((_tmp68E.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp68F=_cycalloc(sizeof(*_tmp68F)),((_tmp68F->v=_tmp8F,_tmp68F))))),((_tmp68E.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp68E.ptr_atts=(
(_tmp691.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp690=_cycalloc(sizeof(*_tmp690)),((_tmp690->v=_tmp8F,_tmp690))))),((_tmp691.nullable=Cyc_Absyn_true_conref,((_tmp691.bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp691.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp691.ptrloc=0,_tmp691)))))))))),_tmp68E)))))),_tmp692)))),_tmp68D))));}
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
const char*_tmp696;void*_tmp695[1];struct Cyc_String_pa_PrintArg_struct _tmp694;void*_tmpB8=(_tmp694.tag=0,((_tmp694.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp695[0]=& _tmp694,Cyc_Tcexp_expr_err(te,loc,0,((_tmp696="undeclared identifier %s",_tag_dyneither(_tmp696,sizeof(char),25))),_tag_dyneither(_tmp695,sizeof(void*),1)))))));_npop_handler(1);return _tmpB8;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA9=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpA9->tag != 0)goto _LL77;else{_tmpAA=(void*)_tmpA9->f1;}}_LL76:
# 348
*b=_tmpAA;{
void*_tmpB9=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB9;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAB->tag != 3)goto _LL79;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp699;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp698;e->r=(void*)((_tmp698=_cycalloc(sizeof(*_tmp698)),((_tmp698[0]=((_tmp699.tag=30,((_tmp699.f1=_tmpAD->name,((_tmp699.f2=(struct Cyc_Absyn_Enumdecl*)_tmpAC,((_tmp699.f3=(struct Cyc_Absyn_Enumfield*)_tmpAD,_tmp699)))))))),_tmp698))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp69C;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp69B;void*_tmpBE=(void*)((_tmp69B=_cycalloc(sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=13,((_tmp69C.f1=_tmpAC->name,((_tmp69C.f2=(struct Cyc_Absyn_Enumdecl*)_tmpAC,_tmp69C)))))),_tmp69B))));_npop_handler(1);return _tmpBE;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAE->tag != 4)goto _LL7B;else{_tmpAF=(void*)_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp69F;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp69E;e->r=(void*)((_tmp69E=_cycalloc(sizeof(*_tmp69E)),((_tmp69E[0]=((_tmp69F.tag=31,((_tmp69F.f1=_tmpB0->name,((_tmp69F.f2=(void*)_tmpAF,((_tmp69F.f3=(struct Cyc_Absyn_Enumfield*)_tmpB0,_tmp69F)))))))),_tmp69E))));}{
void*_tmpC1=_tmpAF;_npop_handler(1);return _tmpC1;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB1->tag != 2)goto _LL7D;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6A2;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6A1;e->r=(void*)((_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=29,((_tmp6A2.f1=0,((_tmp6A2.f2=_tmpB2,((_tmp6A2.f3=_tmpB3,_tmp6A2)))))))),_tmp6A1))));}{
void*_tmpC4=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpB2,_tmpB3);_npop_handler(1);return _tmpC4;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB4=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB4->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6A6;void*_tmp6A5[1];struct Cyc_String_pa_PrintArg_struct _tmp6A4;void*_tmpC8=(_tmp6A4.tag=0,((_tmp6A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6A5[0]=& _tmp6A4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6A6="bad occurrence of type name %s",_tag_dyneither(_tmp6A6,sizeof(char),31))),_tag_dyneither(_tmp6A5,sizeof(void*),1)))))));_npop_handler(1);return _tmpC8;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA3=(void*)_exn_thrown;void*_tmpCA=_tmpA3;void*_tmpCC;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpCB=(struct Cyc_Dict_Absent_exn_struct*)_tmpCA;if(_tmpCB->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6AA;void*_tmp6A9[1];struct Cyc_String_pa_PrintArg_struct _tmp6A8;return(_tmp6A8.tag=0,((_tmp6A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6A9[0]=& _tmp6A8,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6AA="undeclared identifier %s",_tag_dyneither(_tmp6AA,sizeof(char),25))),_tag_dyneither(_tmp6A9,sizeof(void*),1)))))));}_LL82: _tmpCC=_tmpCA;_LL83:(void)_throw(_tmpCC);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp98=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96;if(_tmp98->tag != 1)goto _LL6A;else{_tmp99=_tmp98->f1;}}_LL69:
# 368
*q=*_tmp99->name;
return _tmp99->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp9A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp96;if(_tmp9A->tag != 2)goto _LL6C;else{_tmp9B=_tmp9A->f1;}}_LL6B:
*q=*_tmp9B->name;return Cyc_Tcutil_fndecl2typ(_tmp9B);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp9C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96;if(_tmp9C->tag != 5)goto _LL6E;else{_tmp9D=_tmp9C->f1;}}_LL6D:
 _tmp9F=_tmp9D;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96;if(_tmp9E->tag != 4)goto _LL70;else{_tmp9F=_tmp9E->f1;}}_LL6F:
 _tmpA1=_tmp9F;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96;if(_tmpA0->tag != 3)goto _LL65;else{_tmpA1=_tmpA0->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6AB;(*q).f1=(((_tmp6AB.Loc_n).val=0,(((_tmp6AB.Loc_n).tag=4,_tmp6AB))));}
if(te->allow_valueof){
void*_tmpD1=Cyc_Tcutil_compress(_tmpA1->type);void*_tmpD3;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpD1;if(_tmpD2->tag != 19)goto _LL87;else{_tmpD3=(void*)_tmpD2->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6AE;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6AD;e->r=(void*)((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=37,((_tmp6AE.f1=(void*)_tmpD3,_tmp6AE)))),_tmp6AD))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmpA1->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpD6=fmt->r;struct _dyneither_ptr _tmpD8;struct _dyneither_ptr _tmpDB;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD6;if(_tmpD7->tag != 0)goto _LL8C;else{if(((_tmpD7->f1).String_c).tag != 8)goto _LL8C;_tmpD8=(struct _dyneither_ptr)((_tmpD7->f1).String_c).val;}}_LL8B:
 _tmpDB=_tmpD8;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD6;if(_tmpD9->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(*_tmpD9->f2).r;if(_tmpDA->tag != 0)goto _LL8E;else{if(((_tmpDA->f1).String_c).tag != 8)goto _LL8E;_tmpDB=(struct _dyneither_ptr)((_tmpDA->f1).String_c).val;}}}_LL8D:
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
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->loc,((_tmp6B1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6B1,sizeof(char),49))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}
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
Cyc_Tcexp_tcExp(te,(void**)& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp6B6;void*_tmp6B5[2];struct Cyc_String_pa_PrintArg_struct _tmp6B4;struct Cyc_String_pa_PrintArg_struct _tmp6B3;(_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6B5[0]=& _tmp6B4,((_tmp6B5[1]=& _tmp6B3,Cyc_Tcutil_terr(e->loc,((_tmp6B6="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp6B6,sizeof(char),51))),_tag_dyneither(_tmp6B5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6B7;*alias_arg_exps=(struct Cyc_List_List*)((_tmp6B7=_cycalloc(sizeof(*_tmp6B7)),((_tmp6B7->hd=(void*)arg_cnt,((_tmp6B7->tl=*alias_arg_exps,_tmp6B7))))));}{
struct _RegionHandle _tmpE6=_new_region("temp");struct _RegionHandle*temp=& _tmpE6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6BA;void*_tmp6B9;(_tmp6B9=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6BA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6BA,sizeof(char),49))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6BD;void*_tmp6BC;(_tmp6BC=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6BD="too few arguments",_tag_dyneither(_tmp6BD,sizeof(char),18))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}
if(_tmpE0 != 0){
const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6C0="too many arguments",_tag_dyneither(_tmp6C0,sizeof(char),19))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6C4;void*_tmp6C3[1];struct Cyc_String_pa_PrintArg_struct _tmp6C2;(_tmp6C2.tag=0,((_tmp6C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6C3[0]=& _tmp6C2,Cyc_Tcutil_terr(loc,((_tmp6C4="expecting arithmetic type but found %s",_tag_dyneither(_tmp6C4,sizeof(char),39))),_tag_dyneither(_tmp6C3,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6C8;void*_tmp6C7[1];struct Cyc_String_pa_PrintArg_struct _tmp6C6;(_tmp6C6.tag=0,((_tmp6C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6C7[0]=& _tmp6C6,Cyc_Tcutil_terr(loc,((_tmp6C8="expecting integral or * type but found %s",_tag_dyneither(_tmp6C8,sizeof(char),42))),_tag_dyneither(_tmp6C7,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6CC;void*_tmp6CB[1];struct Cyc_String_pa_PrintArg_struct _tmp6CA;(_tmp6CA.tag=0,((_tmp6CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6CB[0]=& _tmp6CA,Cyc_Tcutil_terr(loc,((_tmp6CC="expecting integral type but found %s",_tag_dyneither(_tmp6CC,sizeof(char),37))),_tag_dyneither(_tmp6CB,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF6=t;union Cyc_Absyn_Constraint*_tmpF9;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF8->tag != 5)goto _LL9B;else{_tmpF9=((_tmpF8->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_tmpFA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF9);struct Cyc_Absyn_Exp*_tmpFD;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFB->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFC->tag != 1)goto _LL9D;else{_tmpFD=_tmpFC->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFD)){
const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_Tcutil_terr(loc,((_tmp6CF="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp6CF,sizeof(char),55))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp6D3;void*_tmp6D2[1];struct Cyc_String_pa_PrintArg_struct _tmp6D1;(_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6D2[0]=& _tmp6D1,Cyc_Tcutil_terr(loc,((_tmp6D3="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp6D3,sizeof(char),47))),_tag_dyneither(_tmp6D2,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp6D6;void*_tmp6D5;(_tmp6D5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6D6="Non-unary primop",_tag_dyneither(_tmp6D6,sizeof(char),17))),_tag_dyneither(_tmp6D5,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp6DA;void*_tmp6D9[1];struct Cyc_String_pa_PrintArg_struct _tmp6D8;(_tmp6D8.tag=0,((_tmp6D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp6D9[0]=& _tmp6D8,Cyc_Tcutil_terr(e1->loc,((_tmp6DA="type %s cannot be used here",_tag_dyneither(_tmp6DA,sizeof(char),28))),_tag_dyneither(_tmp6D9,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp6DE;void*_tmp6DD[1];struct Cyc_String_pa_PrintArg_struct _tmp6DC;(_tmp6DC.tag=0,((_tmp6DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp6DD[0]=& _tmp6DC,Cyc_Tcutil_terr(e2->loc,((_tmp6DE="type %s cannot be used here",_tag_dyneither(_tmp6DE,sizeof(char),28))),_tag_dyneither(_tmp6DD,sizeof(void*),1)))))));}
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
const char*_tmp6E1;void*_tmp6E0;(_tmp6E0=0,Cyc_Tcutil_terr(e1->loc,((_tmp6E1="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp6E1,sizeof(char),50))),_tag_dyneither(_tmp6E0,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp6E4;void*_tmp6E3;(_tmp6E3=0,Cyc_Tcutil_terr(e1->loc,((_tmp6E4="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp6E4,sizeof(char),54))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp6E8;void*_tmp6E7[1];struct Cyc_String_pa_PrintArg_struct _tmp6E6;(_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp6E7[0]=& _tmp6E6,Cyc_Tcutil_terr(e2->loc,((_tmp6E8="expecting int but found %s",_tag_dyneither(_tmp6E8,sizeof(char),27))),_tag_dyneither(_tmp6E7,sizeof(void*),1)))))));}{
void*_tmp11A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp111);struct Cyc_Absyn_Exp*_tmp11D;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11B->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11C->tag != 1)goto _LLA8;else{_tmp11D=_tmp11C->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp112)){
const char*_tmp6EB;void*_tmp6EA;(_tmp6EA=0,Cyc_Tcutil_warn(e1->loc,((_tmp6EB="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp6EB,sizeof(char),70))),_tag_dyneither(_tmp6EA,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6F5;struct Cyc_Absyn_PtrAtts _tmp6F4;struct Cyc_Absyn_PtrInfo _tmp6F3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp120=(_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((_tmp6F2[0]=((_tmp6F5.tag=5,((_tmp6F5.f1=((_tmp6F3.elt_typ=_tmp10D,((_tmp6F3.elt_tq=_tmp10E,((_tmp6F3.ptr_atts=(
(_tmp6F4.rgn=_tmp10F,((_tmp6F4.nullable=Cyc_Absyn_true_conref,((_tmp6F4.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp6F4.zero_term=_tmp112,((_tmp6F4.ptrloc=0,_tmp6F4)))))))))),_tmp6F3)))))),_tmp6F5)))),_tmp6F2)));
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
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
if(!Cyc_Tcutil_unify(t1,t2)){
{const char*_tmp6FA;void*_tmp6F9[2];struct Cyc_String_pa_PrintArg_struct _tmp6F8;struct Cyc_String_pa_PrintArg_struct _tmp6F7;(_tmp6F7.tag=0,((_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 545
Cyc_Absynpp_typ2string(t2)),((_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp6F9[0]=& _tmp6F8,((_tmp6F9[1]=& _tmp6F7,Cyc_Tcutil_terr(e1->loc,((_tmp6FA="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp6FA,sizeof(char),59))),_tag_dyneither(_tmp6F9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 548
return Cyc_Absyn_sint_typ;}else{
# 550
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp6FD;void*_tmp6FC;(_tmp6FC=0,Cyc_Tcutil_terr(e1->loc,((_tmp6FD="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp6FD,sizeof(char),50))),_tag_dyneither(_tmp6FC,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp700;void*_tmp6FF;(_tmp6FF=0,Cyc_Tcutil_terr(e1->loc,((_tmp700="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp700,sizeof(char),54))),_tag_dyneither(_tmp6FF,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp705;void*_tmp704[2];struct Cyc_String_pa_PrintArg_struct _tmp703;struct Cyc_String_pa_PrintArg_struct _tmp702;(_tmp702.tag=0,((_tmp702.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp704[0]=& _tmp703,((_tmp704[1]=& _tmp702,Cyc_Tcutil_terr(e2->loc,((_tmp705="expecting either %s or int but found %s",_tag_dyneither(_tmp705,sizeof(char),40))),_tag_dyneither(_tmp704,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 559
return t1;}}
# 563
if(Cyc_Tcutil_is_pointer_type(t1))
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
if(Cyc_Tcutil_is_pointer_type(t2))
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 570
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 578
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 583
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 596
pointer_cmp: {
struct _tuple0 _tmp706;struct _tuple0 _tmp132=(_tmp706.f1=Cyc_Tcutil_compress(t1),((_tmp706.f2=Cyc_Tcutil_compress(t2),_tmp706)));void*_tmp134;void*_tmp136;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp133=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132.f1;if(_tmp133->tag != 5)goto _LLB0;else{_tmp134=(_tmp133->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp135=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp132.f2;if(_tmp135->tag != 5)goto _LLB0;else{_tmp136=(_tmp135->f1).elt_typ;}};_LLAF:
# 600
 if(Cyc_Tcutil_unify(_tmp134,_tmp136))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp137=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp132.f1;if(_tmp137->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp138=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp132.f2;if(_tmp138->tag != 15)goto _LLB2;};_LLB1:
# 602
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 605
{const char*_tmp70B;void*_tmp70A[2];struct Cyc_String_pa_PrintArg_struct _tmp709;struct Cyc_String_pa_PrintArg_struct _tmp708;(_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp709.tag=0,((_tmp709.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp70A[0]=& _tmp709,((_tmp70A[1]=& _tmp708,Cyc_Tcutil_terr(loc,((_tmp70B="comparison not allowed between %s and %s",_tag_dyneither(_tmp70B,sizeof(char),41))),_tag_dyneither(_tmp70A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 612
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 614
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 620
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp70C;struct _tuple0 _tmp13E=(_tmp70C.f1=t1,((_tmp70C.f2=t2,_tmp70C)));void*_tmp140;void*_tmp142;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13E.f1;if(_tmp13F->tag != 15)goto _LLB7;else{_tmp140=(void*)_tmp13F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13E.f2;if(_tmp141->tag != 15)goto _LLB7;else{_tmp142=(void*)_tmp141->f1;}};_LLB6:
# 625
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 628
{const char*_tmp711;void*_tmp710[2];struct Cyc_String_pa_PrintArg_struct _tmp70F;struct Cyc_String_pa_PrintArg_struct _tmp70E;(_tmp70E.tag=0,((_tmp70E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp710[0]=& _tmp70F,((_tmp710[1]=& _tmp70E,Cyc_Tcutil_terr(loc,((_tmp711="comparison not allowed between %s and %s",_tag_dyneither(_tmp711,sizeof(char),41))),_tag_dyneither(_tmp710,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 636
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 638
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 642
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 645
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 655
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 658
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 663
const char*_tmp714;void*_tmp713;(_tmp713=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp714="bad binary primop",_tag_dyneither(_tmp714,sizeof(char),18))),_tag_dyneither(_tmp713,sizeof(void*),0)));}}}
# 667
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 675
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
const char*_tmp717;void*_tmp716;return(_tmp716=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp717="primitive operator has 0 arguments",_tag_dyneither(_tmp717,sizeof(char),35))),_tag_dyneither(_tmp716,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp71A;void*_tmp719;return(_tmp719=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp71A="primitive operator has > 2 arguments",_tag_dyneither(_tmp71A,sizeof(char),37))),_tag_dyneither(_tmp719,sizeof(void*),0)));}}
# 685
return t;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 688
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp14D=Cyc_Tcutil_compress(t);
void*_tmp14E=_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp150;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Datatypefield*_tmp154;void*_tmp156;struct Cyc_Absyn_Tqual _tmp157;struct Cyc_List_List*_tmp159;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp14E;if(_tmp14F->tag != 11)goto _LLD4;else{if((((_tmp14F->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp150=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp14F->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 692
 _tmp152=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp150->impl))->fields;goto _LLD5;_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp151=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14E;if(_tmp151->tag != 12)goto _LLD6;else{_tmp152=_tmp151->f2;}}_LLD5:
# 694
 for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
struct Cyc_Absyn_Aggrfield*_tmp15A=(struct Cyc_Absyn_Aggrfield*)_tmp152->hd;
if((_tmp15A->tq).real_const){
{const char*_tmp71E;void*_tmp71D[1];struct Cyc_String_pa_PrintArg_struct _tmp71C;(_tmp71C.tag=0,((_tmp71C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp15A->name),((_tmp71D[0]=& _tmp71C,Cyc_Tcutil_terr(loc,((_tmp71E="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp71E,sizeof(char),56))),_tag_dyneither(_tmp71D,sizeof(void*),1)))))));}
return 0;}
# 700
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp15A->type))return 0;}
# 702
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp14E;if(_tmp153->tag != 4)goto _LLD8;else{if((((_tmp153->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp154=((struct _tuple2)(((_tmp153->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 704
{struct Cyc_List_List*_tmp15E=_tmp154->typs;for(0;_tmp15E != 0;_tmp15E=_tmp15E->tl){
struct Cyc_Absyn_Tqual _tmp160;void*_tmp161;struct _tuple19*_tmp15F=(struct _tuple19*)_tmp15E->hd;_tmp160=(*_tmp15F).f1;_tmp161=(*_tmp15F).f2;
if(_tmp160.real_const){
{const char*_tmp722;void*_tmp721[1];struct Cyc_String_pa_PrintArg_struct _tmp720;(_tmp720.tag=0,((_tmp720.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp154->name)),((_tmp721[0]=& _tmp720,Cyc_Tcutil_terr(loc,((_tmp722="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp722,sizeof(char),64))),_tag_dyneither(_tmp721,sizeof(void*),1)))))));}
return 0;}
# 710
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp161))return 0;}}
# 712
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14E;if(_tmp155->tag != 8)goto _LLDA;else{_tmp156=(_tmp155->f1).elt_type;_tmp157=(_tmp155->f1).tq;}}_LLD9:
# 714
 if(_tmp157.real_const){
{const char*_tmp725;void*_tmp724;(_tmp724=0,Cyc_Tcutil_terr(loc,((_tmp725="attempt to over-write a const array",_tag_dyneither(_tmp725,sizeof(char),36))),_tag_dyneither(_tmp724,sizeof(void*),0)));}
return 0;}
# 718
return Cyc_Tcexp_check_writable_aggr(loc,_tmp156);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14E;if(_tmp158->tag != 10)goto _LLDC;else{_tmp159=_tmp158->f1;}}_LLDB:
# 720
 for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
struct Cyc_Absyn_Tqual _tmp168;void*_tmp169;struct _tuple19*_tmp167=(struct _tuple19*)_tmp159->hd;_tmp168=(*_tmp167).f1;_tmp169=(*_tmp167).f2;
if(_tmp168.real_const){
{const char*_tmp728;void*_tmp727;(_tmp727=0,Cyc_Tcutil_terr(loc,((_tmp728="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp728,sizeof(char),56))),_tag_dyneither(_tmp727,sizeof(void*),0)));}
return 0;}
# 726
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp169))return 0;}
# 728
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 736
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 739
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp16C=e->r;struct Cyc_Absyn_Vardecl*_tmp16F;struct Cyc_Absyn_Vardecl*_tmp172;struct Cyc_Absyn_Vardecl*_tmp175;struct Cyc_Absyn_Vardecl*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;struct _dyneither_ptr*_tmp17E;struct Cyc_Absyn_Exp*_tmp180;struct _dyneither_ptr*_tmp181;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp16D->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16E=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16D->f2);if(_tmp16E->tag != 3)goto _LLE1;else{_tmp16F=_tmp16E->f1;}}}_LLE0:
 _tmp172=_tmp16F;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp170->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp171=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp170->f2);if(_tmp171->tag != 4)goto _LLE3;else{_tmp172=_tmp171->f1;}}}_LLE2:
 _tmp175=_tmp172;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp173->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 5)goto _LLE5;else{_tmp175=_tmp174->f1;}}}_LLE4:
 _tmp178=_tmp175;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp176->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp177=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp176->f2);if(_tmp177->tag != 1)goto _LLE7;else{_tmp178=_tmp177->f1;}}}_LLE6:
 if(!(_tmp178->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp179->tag != 22)goto _LLE9;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;}}_LLE8:
# 746
{void*_tmp188=Cyc_Tcutil_compress((void*)_check_null(_tmp17A->topt));struct Cyc_Absyn_Tqual _tmp18A;struct Cyc_Absyn_Tqual _tmp18C;struct Cyc_List_List*_tmp18E;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188;if(_tmp189->tag != 5)goto _LLF8;else{_tmp18A=(_tmp189->f1).elt_tq;}}_LLF7:
 _tmp18C=_tmp18A;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp188;if(_tmp18B->tag != 8)goto _LLFA;else{_tmp18C=(_tmp18B->f1).tq;}}_LLF9:
 if(!_tmp18C.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp188;if(_tmp18D->tag != 10)goto _LLFC;else{_tmp18E=_tmp18D->f1;}}_LLFB: {
# 750
unsigned int _tmp190;int _tmp191;struct _tuple15 _tmp18F=Cyc_Evexp_eval_const_uint_exp(_tmp17B);_tmp190=_tmp18F.f1;_tmp191=_tmp18F.f2;
if(!_tmp191){
{const char*_tmp72B;void*_tmp72A;(_tmp72A=0,Cyc_Tcutil_terr(e->loc,((_tmp72B="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp72B,sizeof(char),47))),_tag_dyneither(_tmp72A,sizeof(void*),0)));}
return;}
# 755
{struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){
{struct Cyc_Absyn_Tqual _tmp198;struct _tuple19*_tmp197=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp18E,(int)_tmp190);_tmp198=(*_tmp197).f1;
if(!_tmp198.real_const){_npop_handler(0);return;}}
# 756
;_pop_handler();}else{void*_tmp195=(void*)_exn_thrown;void*_tmp19A=_tmp195;void*_tmp19C;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp19B=(struct Cyc_List_Nth_exn_struct*)_tmp19A;if(_tmp19B->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 758
 return;_LL101: _tmp19C=_tmp19A;_LL102:(void)_throw(_tmp19C);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 762
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp17C->tag != 20)goto _LLEB;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LLEA:
# 764
{void*_tmp19D=Cyc_Tcutil_compress((void*)_check_null(_tmp17D->topt));struct Cyc_Absyn_Aggrdecl**_tmp19F;struct Cyc_List_List*_tmp1A1;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp19D;if(_tmp19E->tag != 11)goto _LL106;else{if((((_tmp19E->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp19F=(struct Cyc_Absyn_Aggrdecl**)(((_tmp19E->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 766
struct Cyc_Absyn_Aggrfield*sf=
(struct Cyc_Absyn_Aggrdecl**)_tmp19F == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp19F,_tmp17E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19D;if(_tmp1A0->tag != 12)goto _LL108;else{_tmp1A1=_tmp1A0->f2;}}_LL107: {
# 771
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1A1,_tmp17E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 776
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp17F->tag != 21)goto _LLED;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;}}_LLEC:
# 778
{void*_tmp1A2=Cyc_Tcutil_compress((void*)_check_null(_tmp180->topt));void*_tmp1A4;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A2;if(_tmp1A3->tag != 5)goto _LL10D;else{_tmp1A4=(_tmp1A3->f1).elt_typ;}}_LL10C:
# 780
{void*_tmp1A5=Cyc_Tcutil_compress(_tmp1A4);struct Cyc_Absyn_Aggrdecl**_tmp1A7;struct Cyc_List_List*_tmp1A9;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A5;if(_tmp1A6->tag != 11)goto _LL112;else{if((((_tmp1A6->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1A7=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1A6->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 782
struct Cyc_Absyn_Aggrfield*sf=
(struct Cyc_Absyn_Aggrdecl**)_tmp1A7 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1A7,_tmp181);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A5;if(_tmp1A8->tag != 12)goto _LL114;else{_tmp1A9=_tmp1A8->f2;}}_LL113: {
# 787
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1A9,_tmp181);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 792
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 795
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp182->tag != 19)goto _LLEF;else{_tmp183=_tmp182->f1;}}_LLEE:
# 797
{void*_tmp1AA=Cyc_Tcutil_compress((void*)_check_null(_tmp183->topt));struct Cyc_Absyn_Tqual _tmp1AC;struct Cyc_Absyn_Tqual _tmp1AE;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AA;if(_tmp1AB->tag != 5)goto _LL119;else{_tmp1AC=(_tmp1AB->f1).elt_tq;}}_LL118:
 _tmp1AE=_tmp1AC;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AA;if(_tmp1AD->tag != 8)goto _LL11B;else{_tmp1AE=(_tmp1AD->f1).tq;}}_LL11A:
 if(!_tmp1AE.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 802
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp184->tag != 11)goto _LLF1;else{_tmp185=_tmp184->f1;}}_LLF0:
 _tmp187=_tmp185;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp16C;if(_tmp186->tag != 12)goto _LLF3;else{_tmp187=_tmp186->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp187);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 807
const char*_tmp72F;void*_tmp72E[1];struct Cyc_String_pa_PrintArg_struct _tmp72D;(_tmp72D.tag=0,((_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp72E[0]=& _tmp72D,Cyc_Tcutil_terr(e->loc,((_tmp72F="attempt to write a const location: %s",_tag_dyneither(_tmp72F,sizeof(char),38))),_tag_dyneither(_tmp72E,sizeof(void*),1)))))));};}
# 810
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 813
struct _RegionHandle _tmp1B2=_new_region("temp");struct _RegionHandle*temp=& _tmp1B2;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 816
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp732;void*_tmp731;void*_tmp1B5=(_tmp731=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp732="increment/decrement of non-lvalue",_tag_dyneither(_tmp732,sizeof(char),34))),_tag_dyneither(_tmp731,sizeof(void*),0)));_npop_handler(0);return _tmp1B5;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 821
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp735;void*_tmp734;(_tmp734=0,Cyc_Tcutil_terr(e->loc,((_tmp735="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp735,sizeof(char),50))),_tag_dyneither(_tmp734,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp738;void*_tmp737;(_tmp737=0,Cyc_Tcutil_terr(e->loc,((_tmp738="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp738,sizeof(char),54))),_tag_dyneither(_tmp737,sizeof(void*),0)));}}else{
# 830
const char*_tmp73C;void*_tmp73B[1];struct Cyc_String_pa_PrintArg_struct _tmp73A;(_tmp73A.tag=0,((_tmp73A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp73B[0]=& _tmp73A,Cyc_Tcutil_terr(e->loc,((_tmp73C="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp73C,sizeof(char),44))),_tag_dyneither(_tmp73B,sizeof(void*),1)))))));}}{
# 832
void*_tmp1BD=t;_npop_handler(0);return _tmp1BD;};};
# 813
;_pop_region(temp);}
# 836
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 838
struct _tuple0*_tmp1C0;int _tmp1C1;const char*_tmp73D;struct Cyc_Tcexp_TestEnv _tmp1BF=Cyc_Tcexp_tcTest(te,e1,((_tmp73D="conditional expression",_tag_dyneither(_tmp73D,sizeof(char),23))));_tmp1C0=_tmp1BF.eq;_tmp1C1=_tmp1BF.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_List_List _tmp73E;struct Cyc_List_List _tmp1C2=(_tmp73E.hd=e3,((_tmp73E.tl=0,_tmp73E)));
struct Cyc_List_List _tmp73F;struct Cyc_List_List _tmp1C3=(_tmp73F.hd=e2,((_tmp73F.tl=(struct Cyc_List_List*)& _tmp1C2,_tmp73F)));
if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp1C3)){
{const char*_tmp744;void*_tmp743[2];struct Cyc_String_pa_PrintArg_struct _tmp742;struct Cyc_String_pa_PrintArg_struct _tmp741;(_tmp741.tag=0,((_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp742.tag=0,((_tmp742.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp743[0]=& _tmp742,((_tmp743[1]=& _tmp741,Cyc_Tcutil_terr(loc,((_tmp744="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp744,sizeof(char),48))),_tag_dyneither(_tmp743,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 849
return t;};}
# 853
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 855
{const char*_tmp745;Cyc_Tcexp_tcTest(te,e1,((_tmp745="logical-and expression",_tag_dyneither(_tmp745,sizeof(char),23))));}
{const char*_tmp746;Cyc_Tcexp_tcTest(te,e2,((_tmp746="logical-and expression",_tag_dyneither(_tmp746,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 861
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 863
{const char*_tmp747;Cyc_Tcexp_tcTest(te,e1,((_tmp747="logical-or expression",_tag_dyneither(_tmp747,sizeof(char),22))));}
{const char*_tmp748;Cyc_Tcexp_tcTest(te,e2,((_tmp748="logical-or expression",_tag_dyneither(_tmp748,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 869
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 876
struct _RegionHandle _tmp1CE=_new_region("r");struct _RegionHandle*r=& _tmp1CE;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 879
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,(void**)& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 883
{void*_tmp1CF=Cyc_Tcutil_compress(t1);_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CF;if(_tmp1D0->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_terr(loc,((_tmp74B="cannot assign to an array",_tag_dyneither(_tmp74B,sizeof(char),26))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 888
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_terr(loc,((_tmp74E="type is abstract (can't determine size).",_tag_dyneither(_tmp74E,sizeof(char),41))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}
# 892
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp751;void*_tmp750;void*_tmp1D7=(_tmp750=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp751="assignment to non-lvalue",_tag_dyneither(_tmp751,sizeof(char),25))),_tag_dyneither(_tmp750,sizeof(void*),0)));_npop_handler(0);return _tmp1D7;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1D8=_new_region("temp");struct _RegionHandle*temp=& _tmp1D8;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp754;void*_tmp753;(_tmp753=0,Cyc_Tcutil_terr(e2->loc,((_tmp754="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp754,sizeof(char),49))),_tag_dyneither(_tmp753,sizeof(void*),0)));}
# 900
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp759;void*_tmp758[2];struct Cyc_String_pa_PrintArg_struct _tmp757;struct Cyc_String_pa_PrintArg_struct _tmp756;void*_tmp1DB=(_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp757.tag=0,((_tmp757.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp758[0]=& _tmp757,((_tmp758[1]=& _tmp756,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp759="type mismatch: %s != %s",_tag_dyneither(_tmp759,sizeof(char),24))),_tag_dyneither(_tmp758,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1DC=_tmp1DB;_npop_handler(1);return _tmp1DC;};}
# 896
;_pop_region(temp);}else{
# 908
enum Cyc_Absyn_Primop _tmp1E1=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1E2=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1E1,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1E2,t1) || Cyc_Tcutil_coerceable(_tmp1E2) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp75E;void*_tmp75D[2];struct Cyc_String_pa_PrintArg_struct _tmp75C;struct Cyc_String_pa_PrintArg_struct _tmp75B;void*_tmp1E3=
(_tmp75B.tag=0,((_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 914
Cyc_Absynpp_typ2string(t2)),((_tmp75C.tag=0,((_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 913
Cyc_Absynpp_typ2string(t1)),((_tmp75D[0]=& _tmp75C,((_tmp75D[1]=& _tmp75B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp75E="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp75E,sizeof(char),82))),_tag_dyneither(_tmp75D,sizeof(void*),2)))))))))))));
# 915
Cyc_Tcutil_unify(_tmp1E2,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1E4=_tmp1E3;_npop_handler(0);return _tmp1E4;};}{
# 919
void*_tmp1E9=_tmp1E2;_npop_handler(0);return _tmp1E9;};}{
# 921
void*_tmp1EA=t1;_npop_handler(0);return _tmp1EA;};};};
# 876
;_pop_region(r);}
# 925
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_tcExp(te,topt,e2);
return(void*)_check_null(e2->topt);}
# 932
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 936
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 939
{void*_tmp1EB=Cyc_Tcutil_compress(t1);_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1EB;if(_tmp1EC->tag != 7)goto _LL125;else{if(_tmp1EC->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1EB;if(_tmp1ED->tag != 6)goto _LL127;else{if(_tmp1ED->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1EB;if(_tmp1EE->tag != 6)goto _LL129;else{if(_tmp1EE->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 946
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp1F0;struct Cyc_List_List*_tmp1F1;unsigned int _tmp1F2;enum Cyc_Absyn_Scope _tmp1F3;struct Cyc_Absyn_Datatypefield _tmp1EF=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1F0=_tmp1EF.name;_tmp1F1=_tmp1EF.typs;_tmp1F2=_tmp1EF.loc;_tmp1F3=_tmp1EF.sc;
# 949
if(_tmp1F1 == 0  || _tmp1F1->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp1F1->hd)).f2);
# 952
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 957
for(fields=fs;fields != 0;fields=fields->tl){
struct _tuple1*_tmp1F5;struct Cyc_List_List*_tmp1F6;unsigned int _tmp1F7;enum Cyc_Absyn_Scope _tmp1F8;struct Cyc_Absyn_Datatypefield _tmp1F4=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1F5=_tmp1F4.name;_tmp1F6=_tmp1F4.typs;_tmp1F7=_tmp1F4.loc;_tmp1F8=_tmp1F4.sc;
# 960
if(_tmp1F6 == 0  || _tmp1F6->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp1F6->hd)).f2);
# 964
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 969
{const char*_tmp763;void*_tmp762[2];struct Cyc_String_pa_PrintArg_struct _tmp761;struct Cyc_String_pa_PrintArg_struct _tmp760;(_tmp760.tag=0,((_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp761.tag=0,((_tmp761.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp762[0]=& _tmp761,((_tmp762[1]=& _tmp760,Cyc_Tcutil_terr(e->loc,((_tmp763="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp763,sizeof(char),54))),_tag_dyneither(_tmp762,sizeof(void*),2)))))))))))));}
return 0;}
# 975
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 981
struct Cyc_List_List*_tmp1FD=args;
int _tmp1FE=0;
struct _RegionHandle _tmp1FF=_new_region("ter");struct _RegionHandle*ter=& _tmp1FF;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp200=Cyc_Tcenv_new_block(ter,loc,te_orig);
Cyc_Tcexp_tcExp(_tmp200,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 990
void*_tmp201=t;void*_tmp203;struct Cyc_Absyn_Tqual _tmp204;void*_tmp205;union Cyc_Absyn_Constraint*_tmp206;union Cyc_Absyn_Constraint*_tmp207;union Cyc_Absyn_Constraint*_tmp208;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp201;if(_tmp202->tag != 5)goto _LL12E;else{_tmp203=(_tmp202->f1).elt_typ;_tmp204=(_tmp202->f1).elt_tq;_tmp205=((_tmp202->f1).ptr_atts).rgn;_tmp206=((_tmp202->f1).ptr_atts).nullable;_tmp207=((_tmp202->f1).ptr_atts).bounds;_tmp208=((_tmp202->f1).ptr_atts).zero_term;}}_LL12D:
# 995
 Cyc_Tcenv_check_rgn_accessible(_tmp200,loc,_tmp205);
# 997
Cyc_Tcutil_check_nonzero_bound(loc,_tmp207);{
void*_tmp209=Cyc_Tcutil_compress(_tmp203);struct Cyc_List_List*_tmp20B;void*_tmp20C;struct Cyc_Absyn_Tqual _tmp20D;void*_tmp20E;struct Cyc_List_List*_tmp20F;int _tmp210;struct Cyc_Absyn_VarargInfo*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp213;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp209;if(_tmp20A->tag != 9)goto _LL133;else{_tmp20B=(_tmp20A->f1).tvars;_tmp20C=(_tmp20A->f1).effect;_tmp20D=(_tmp20A->f1).ret_tqual;_tmp20E=(_tmp20A->f1).ret_typ;_tmp20F=(_tmp20A->f1).args;_tmp210=(_tmp20A->f1).c_varargs;_tmp211=(_tmp20A->f1).cyc_varargs;_tmp212=(_tmp20A->f1).rgn_po;_tmp213=(_tmp20A->f1).attributes;}}_LL132:
# 1004
 if(topt != 0)Cyc_Tcutil_unify(_tmp20E,*topt);
# 1006
while(_tmp1FD != 0  && _tmp20F != 0){
# 1008
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1FD->hd;
void*t2=(*((struct _tuple9*)_tmp20F->hd)).f3;
Cyc_Tcexp_tcExp(_tmp200,(void**)& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp200,e1,t2,& alias_coercion)){
{const char*_tmp768;void*_tmp767[2];struct Cyc_String_pa_PrintArg_struct _tmp766;struct Cyc_String_pa_PrintArg_struct _tmp765;(_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1016
Cyc_Absynpp_typ2string(t2)),((_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp767[0]=& _tmp766,((_tmp767[1]=& _tmp765,Cyc_Tcutil_terr(e1->loc,((_tmp768="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp768,sizeof(char),57))),_tag_dyneither(_tmp767,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1021
if(alias_coercion){
struct Cyc_List_List*_tmp769;*alias_arg_exps=(struct Cyc_List_List*)((_tmp769=_cycalloc(sizeof(*_tmp769)),((_tmp769->hd=(void*)_tmp1FE,((_tmp769->tl=*alias_arg_exps,_tmp769))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp76C;void*_tmp76B;(_tmp76B=0,Cyc_Tcutil_terr(e1->loc,((_tmp76C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp76C,sizeof(char),49))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}
_tmp1FD=_tmp1FD->tl;
_tmp20F=_tmp20F->tl;
++ _tmp1FE;}{
# 1032
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp213;for(0;a != 0;a=a->tl){
void*_tmp21B=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp21D;int _tmp21E;int _tmp21F;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp21C=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp21B;if(_tmp21C->tag != 19)goto _LL138;else{_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;_tmp21F=_tmp21C->f3;}}_LL137:
# 1036
{struct _handler_cons _tmp220;_push_handler(& _tmp220);{int _tmp222=0;if(setjmp(_tmp220.handler))_tmp222=1;if(!_tmp222){
# 1038
{struct Cyc_Absyn_Exp*_tmp223=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp21E - 1);
# 1040
struct Cyc_Core_Opt*fmt_args;
if(_tmp21F == 0)
fmt_args=0;else{
# 1044
struct Cyc_Core_Opt*_tmp76D;fmt_args=((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp21F - 1),_tmp76D))));}
args_already_checked=1;{
struct _RegionHandle _tmp225=_new_region("temp");struct _RegionHandle*temp=& _tmp225;_push_region(temp);
switch(_tmp21D){case Cyc_Absyn_Printf_ft: _LL13A:
# 1049
 Cyc_Tcexp_check_format_args(_tmp200,_tmp223,fmt_args,_tmp21F - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1052
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1054
 Cyc_Tcexp_check_format_args(_tmp200,_tmp223,fmt_args,_tmp21F - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1057
break;}
# 1047
;_pop_region(temp);};}
# 1038
;_pop_handler();}else{void*_tmp221=(void*)_exn_thrown;void*_tmp227=_tmp221;void*_tmp229;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp228=(struct Cyc_List_Nth_exn_struct*)_tmp227;if(_tmp228->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1061
{const char*_tmp770;void*_tmp76F;(_tmp76F=0,Cyc_Tcutil_terr(loc,((_tmp770="bad format arguments",_tag_dyneither(_tmp770,sizeof(char),21))),_tag_dyneither(_tmp76F,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp229=_tmp227;_LL141:(void)_throw(_tmp229);_LL13D:;}};}
# 1063
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1067
if(_tmp20F != 0){const char*_tmp773;void*_tmp772;(_tmp772=0,Cyc_Tcutil_terr(loc,((_tmp773="too few arguments for function",_tag_dyneither(_tmp773,sizeof(char),31))),_tag_dyneither(_tmp772,sizeof(void*),0)));}else{
# 1069
if((_tmp1FD != 0  || _tmp210) || _tmp211 != 0){
if(_tmp210)
for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){
Cyc_Tcexp_tcExp(_tmp200,0,(struct Cyc_Absyn_Exp*)_tmp1FD->hd);}else{
if(_tmp211 == 0){
const char*_tmp776;void*_tmp775;(_tmp775=0,Cyc_Tcutil_terr(loc,((_tmp776="too many arguments for function",_tag_dyneither(_tmp776,sizeof(char),32))),_tag_dyneither(_tmp775,sizeof(void*),0)));}else{
# 1076
void*_tmp231;int _tmp232;struct Cyc_Absyn_VarargInfo _tmp230=*_tmp211;_tmp231=_tmp230.type;_tmp232=_tmp230.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp777;struct Cyc_Absyn_VarargCallInfo*_tmp233=(_tmp777=_cycalloc(sizeof(*_tmp777)),((_tmp777->num_varargs=0,((_tmp777->injectors=0,((_tmp777->vai=(struct Cyc_Absyn_VarargInfo*)_tmp211,_tmp777)))))));
# 1080
*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp233;
# 1082
if(!_tmp232)
# 1084
for(0;_tmp1FD != 0;(_tmp1FD=_tmp1FD->tl,_tmp1FE ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1FD->hd;
++ _tmp233->num_varargs;
Cyc_Tcexp_tcExp(_tmp200,(void**)& _tmp231,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp200,e1,_tmp231,& alias_coercion)){
{const char*_tmp77C;void*_tmp77B[2];struct Cyc_String_pa_PrintArg_struct _tmp77A;struct Cyc_String_pa_PrintArg_struct _tmp779;(_tmp779.tag=0,((_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp77A.tag=0,((_tmp77A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp231)),((_tmp77B[0]=& _tmp77A,((_tmp77B[1]=& _tmp779,Cyc_Tcutil_terr(loc,((_tmp77C="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp77C,sizeof(char),49))),_tag_dyneither(_tmp77B,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1094
if(alias_coercion){
struct Cyc_List_List*_tmp77D;*alias_arg_exps=(struct Cyc_List_List*)((_tmp77D=_cycalloc(sizeof(*_tmp77D)),((_tmp77D->hd=(void*)_tmp1FE,((_tmp77D->tl=*alias_arg_exps,_tmp77D))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp231) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp780;void*_tmp77F;(_tmp77F=0,Cyc_Tcutil_terr(e1->loc,((_tmp780="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp780,sizeof(char),49))),_tag_dyneither(_tmp77F,sizeof(void*),0)));}}else{
# 1103
void*_tmp23B=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp231));struct Cyc_Absyn_Datatypedecl*_tmp23D;struct Cyc_List_List*_tmp23E;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23B;if(_tmp23C->tag != 3)goto _LL145;else{if((((_tmp23C->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp23D=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp23C->f1).datatype_info).KnownDatatype).val);_tmp23E=(_tmp23C->f1).targs;}}_LL144: {
# 1107
struct Cyc_Absyn_Datatypedecl*_tmp23F=*Cyc_Tcenv_lookup_datatypedecl(_tmp200,loc,_tmp23D->name);
struct Cyc_List_List*fields=0;
if(_tmp23F->fields == 0){
const char*_tmp784;void*_tmp783[1];struct Cyc_String_pa_PrintArg_struct _tmp782;(_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp231)),((_tmp783[0]=& _tmp782,Cyc_Tcutil_terr(loc,((_tmp784="can't inject into abstract datatype %s",_tag_dyneither(_tmp784,sizeof(char),39))),_tag_dyneither(_tmp783,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23F->fields))->v;}
# 1119
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp231),Cyc_Tcenv_curr_rgn(_tmp200))){
const char*_tmp789;void*_tmp788[2];struct Cyc_String_pa_PrintArg_struct _tmp787;struct Cyc_String_pa_PrintArg_struct _tmp786;(_tmp786.tag=0,((_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp200))),((_tmp787.tag=0,((_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp231)),((_tmp788[0]=& _tmp787,((_tmp788[1]=& _tmp786,Cyc_Tcutil_terr(loc,((_tmp789="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp789,sizeof(char),49))),_tag_dyneither(_tmp788,sizeof(void*),2)))))))))))));}{
# 1123
struct _RegionHandle _tmp247=_new_region("rgn");struct _RegionHandle*rgn=& _tmp247;_push_region(rgn);{
struct Cyc_List_List*_tmp248=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp23F->tvs,_tmp23E);
for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){
++ _tmp233->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1FD->hd;
# 1129
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp200,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_Tcutil_terr(e1->loc,((_tmp78C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp78C,sizeof(char),49))),_tag_dyneither(_tmp78B,sizeof(void*),0)));}}{
# 1135
struct Cyc_Absyn_Datatypefield*_tmp24B=Cyc_Tcexp_tcInjection(_tmp200,e1,Cyc_Tcutil_pointer_elt_type(_tmp231),rgn,_tmp248,fields);
# 1137
if(_tmp24B != 0){
struct Cyc_List_List*_tmp78D;_tmp233->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp233->injectors,(
(_tmp78D=_cycalloc(sizeof(*_tmp78D)),((_tmp78D->hd=(struct Cyc_Absyn_Datatypefield*)_tmp24B,((_tmp78D->tl=0,_tmp78D)))))));}};};}}
# 1143
_npop_handler(0);goto _LL142;
# 1123
;_pop_region(rgn);};}_LL145:;_LL146:
# 1144
{const char*_tmp790;void*_tmp78F;(_tmp78F=0,Cyc_Tcutil_terr(loc,((_tmp790="bad inject vararg type",_tag_dyneither(_tmp790,sizeof(char),23))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1151
Cyc_Tcenv_check_effect_accessible(_tmp200,loc,(void*)_check_null(_tmp20C));
# 1154
Cyc_Tcenv_check_rgn_partial_order(_tmp200,loc,_tmp212);{
void*_tmp250=_tmp20E;_npop_handler(0);return _tmp250;};};_LL133:;_LL134: {
const char*_tmp793;void*_tmp792;void*_tmp253=(_tmp792=0,Cyc_Tcexp_expr_err(_tmp200,loc,topt,((_tmp793="expected pointer to function",_tag_dyneither(_tmp793,sizeof(char),29))),_tag_dyneither(_tmp792,sizeof(void*),0)));_npop_handler(0);return _tmp253;}_LL130:;};_LL12E:;_LL12F: {
# 1158
const char*_tmp796;void*_tmp795;void*_tmp256=(_tmp795=0,Cyc_Tcexp_expr_err(_tmp200,loc,topt,((_tmp796="expected pointer to function",_tag_dyneither(_tmp796,sizeof(char),29))),_tag_dyneither(_tmp795,sizeof(void*),0)));_npop_handler(0);return _tmp256;}_LL12B:;};}
# 984
;_pop_region(ter);}
# 1164
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp257=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,(void**)& _tmp257,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp257,& bogus)){
const char*_tmp79B;void*_tmp79A[2];struct Cyc_String_pa_PrintArg_struct _tmp799;struct Cyc_String_pa_PrintArg_struct _tmp798;(_tmp798.tag=0,((_tmp798.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp799.tag=0,((_tmp799.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1169
Cyc_Absynpp_typ2string(_tmp257)),((_tmp79A[0]=& _tmp799,((_tmp79A[1]=& _tmp798,Cyc_Tcutil_terr(loc,((_tmp79B="expected %s but found %s",_tag_dyneither(_tmp79B,sizeof(char),25))),_tag_dyneither(_tmp79A,sizeof(void*),2)))))))))))));}
# 1171
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1176
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1178
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1180
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp25C=t1;void*_tmp25E;struct Cyc_Absyn_Tqual _tmp25F;void*_tmp260;union Cyc_Absyn_Constraint*_tmp261;union Cyc_Absyn_Constraint*_tmp262;union Cyc_Absyn_Constraint*_tmp263;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25C;if(_tmp25D->tag != 5)goto _LL14A;else{_tmp25E=(_tmp25D->f1).elt_typ;_tmp25F=(_tmp25D->f1).elt_tq;_tmp260=((_tmp25D->f1).ptr_atts).rgn;_tmp261=((_tmp25D->f1).ptr_atts).nullable;_tmp262=((_tmp25D->f1).ptr_atts).bounds;_tmp263=((_tmp25D->f1).ptr_atts).zero_term;}}_LL149:
# 1184
{void*_tmp264=Cyc_Tcutil_compress(_tmp25E);struct Cyc_List_List*_tmp266;void*_tmp267;struct Cyc_Absyn_Tqual _tmp268;void*_tmp269;struct Cyc_List_List*_tmp26A;int _tmp26B;struct Cyc_Absyn_VarargInfo*_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26E;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp264;if(_tmp265->tag != 9)goto _LL14F;else{_tmp266=(_tmp265->f1).tvars;_tmp267=(_tmp265->f1).effect;_tmp268=(_tmp265->f1).ret_tqual;_tmp269=(_tmp265->f1).ret_typ;_tmp26A=(_tmp265->f1).args;_tmp26B=(_tmp265->f1).c_varargs;_tmp26C=(_tmp265->f1).cyc_varargs;_tmp26D=(_tmp265->f1).rgn_po;_tmp26E=(_tmp265->f1).attributes;}}_LL14E: {
# 1186
struct _RegionHandle _tmp26F=_new_region("temp");struct _RegionHandle*temp=& _tmp26F;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1189
for(0;ts != 0  && _tmp266 != 0;(ts=ts->tl,_tmp266=_tmp266->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp266->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp79E;struct Cyc_List_List*_tmp79D;instantiation=(
(_tmp79D=_region_malloc(temp,sizeof(*_tmp79D)),((_tmp79D->hd=((_tmp79E=_region_malloc(temp,sizeof(*_tmp79E)),((_tmp79E->f1=(struct Cyc_Absyn_Tvar*)_tmp266->hd,((_tmp79E->f2=(void*)ts->hd,_tmp79E)))))),((_tmp79D->tl=instantiation,_tmp79D))))));};}
# 1196
if(ts != 0){
const char*_tmp7A1;void*_tmp7A0;void*_tmp274=
(_tmp7A0=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7A1="too many type variables in instantiation",_tag_dyneither(_tmp7A1,sizeof(char),41))),_tag_dyneither(_tmp7A0,sizeof(void*),0)));_npop_handler(0);return _tmp274;}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7A7;struct Cyc_Absyn_FnInfo _tmp7A6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7A5;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A7.tag=9,((_tmp7A7.f1=((_tmp7A6.tvars=_tmp266,((_tmp7A6.effect=_tmp267,((_tmp7A6.ret_tqual=_tmp268,((_tmp7A6.ret_typ=_tmp269,((_tmp7A6.args=_tmp26A,((_tmp7A6.c_varargs=_tmp26B,((_tmp7A6.cyc_varargs=_tmp26C,((_tmp7A6.rgn_po=_tmp26D,((_tmp7A6.attributes=_tmp26E,_tmp7A6)))))))))))))))))),_tmp7A7)))),_tmp7A5)))));
# 1203
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7B1;struct Cyc_Absyn_PtrAtts _tmp7B0;struct Cyc_Absyn_PtrInfo _tmp7AF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7AE;void*_tmp279=(void*)((_tmp7AE=_cycalloc(sizeof(*_tmp7AE)),((_tmp7AE[0]=((_tmp7B1.tag=5,((_tmp7B1.f1=((_tmp7AF.elt_typ=new_fn_typ,((_tmp7AF.elt_tq=_tmp25F,((_tmp7AF.ptr_atts=((_tmp7B0.rgn=_tmp260,((_tmp7B0.nullable=_tmp261,((_tmp7B0.bounds=_tmp262,((_tmp7B0.zero_term=_tmp263,((_tmp7B0.ptrloc=0,_tmp7B0)))))))))),_tmp7AF)))))),_tmp7B1)))),_tmp7AE))));_npop_handler(0);return _tmp279;};}
# 1187
;_pop_region(temp);}_LL14F:;_LL150:
# 1205
 goto _LL14C;_LL14C:;}
# 1207
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1210
const char*_tmp7B5;void*_tmp7B4[1];struct Cyc_String_pa_PrintArg_struct _tmp7B3;return(_tmp7B3.tag=0,((_tmp7B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7B4[0]=& _tmp7B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B5="expecting polymorphic type but found %s",_tag_dyneither(_tmp7B5,sizeof(char),40))),_tag_dyneither(_tmp7B4,sizeof(void*),1)))))));};};}
# 1215
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1217
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1220
Cyc_Tcexp_tcExp(te,(void**)& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1225
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1232
Cyc_Tcutil_unify(t2,t);{
const char*_tmp7BA;void*_tmp7B9[2];struct Cyc_String_pa_PrintArg_struct _tmp7B8;struct Cyc_String_pa_PrintArg_struct _tmp7B7;void*_tmp280=(_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7B9[0]=& _tmp7B8,((_tmp7B9[1]=& _tmp7B7,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp7BA="cannot cast %s to %s",_tag_dyneither(_tmp7BA,sizeof(char),21))),_tag_dyneither(_tmp7B9,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp280;};}}}
# 1242
{struct _tuple0 _tmp7BB;struct _tuple0 _tmp286=(_tmp7BB.f1=e->r,((_tmp7BB.f2=Cyc_Tcutil_compress(t),_tmp7BB)));int _tmp288;union Cyc_Absyn_Constraint*_tmp28A;union Cyc_Absyn_Constraint*_tmp28B;union Cyc_Absyn_Constraint*_tmp28C;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp287=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp286.f1;if(_tmp287->tag != 32)goto _LL154;else{_tmp288=(_tmp287->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp289=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp286.f2;if(_tmp289->tag != 5)goto _LL154;else{_tmp28A=((_tmp289->f1).ptr_atts).nullable;_tmp28B=((_tmp289->f1).ptr_atts).bounds;_tmp28C=((_tmp289->f1).ptr_atts).zero_term;}};_LL153:
# 1246
 if((_tmp288  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp28C)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp28A)){
void*_tmp28D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp28B);struct Cyc_Absyn_Exp*_tmp28F;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp28D;if(_tmp28E->tag != 1)goto _LL159;else{_tmp28F=_tmp28E->f1;}}_LL158:
# 1249
 if((Cyc_Evexp_eval_const_uint_exp(_tmp28F)).f1 == 1){
const char*_tmp7BE;void*_tmp7BD;(_tmp7BD=0,Cyc_Tcutil_warn(loc,((_tmp7BE="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp7BE,sizeof(char),60))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1255
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1258
return t;};}
# 1262
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1264
void**_tmp292=0;
struct Cyc_Absyn_Tqual _tmp293=Cyc_Absyn_empty_tqual(0);
if(topt != 0){
void*_tmp294=Cyc_Tcutil_compress(*topt);void**_tmp296;struct Cyc_Absyn_Tqual _tmp297;union Cyc_Absyn_Constraint*_tmp298;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp295=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp294;if(_tmp295->tag != 5)goto _LL15E;else{_tmp296=(void**)&(_tmp295->f1).elt_typ;_tmp297=(_tmp295->f1).elt_tq;_tmp298=((_tmp295->f1).ptr_atts).zero_term;}}_LL15D:
# 1269
 _tmp292=(void**)_tmp296;
_tmp293=_tmp297;
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1282
struct _RegionHandle _tmp299=_new_region("r");struct _RegionHandle*r=& _tmp299;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp292,e);
# 1285
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp7C1;void*_tmp7C0;(_tmp7C0=0,Cyc_Tcutil_terr(e->loc,((_tmp7C1="Cannot take the address of an alias-free path",_tag_dyneither(_tmp7C1,sizeof(char),46))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}
# 1292
{void*_tmp29C=e->r;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29F;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp29D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp29C;if(_tmp29D->tag != 22)goto _LL163;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}_LL162:
# 1294
{void*_tmp2A0=Cyc_Tcutil_compress((void*)_check_null(_tmp29E->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2A1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2A0;if(_tmp2A1->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1302
 e0->r=(Cyc_Absyn_add_exp(_tmp29E,_tmp29F,0))->r;{
void*_tmp2A2=Cyc_Tcexp_tcPlus(te,_tmp29E,_tmp29F);_npop_handler(0);return _tmp2A2;};_LL165:;}
# 1305
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1310
{void*_tmp2A3=e->r;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2A4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2A3;if(_tmp2A4->tag != 20)goto _LL16D;else{if(_tmp2A4->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2A5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2A3;if(_tmp2A5->tag != 21)goto _LL16F;else{if(_tmp2A5->f3 != 1)goto _LL16F;}}_LL16E:
# 1313
{const char*_tmp7C4;void*_tmp7C3;(_tmp7C3=0,Cyc_Tcutil_terr(e->loc,((_tmp7C4="cannot take the address of a @tagged union member",_tag_dyneither(_tmp7C4,sizeof(char),50))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1319
int _tmp2A9;void*_tmp2AA;struct _tuple14 _tmp2A8=Cyc_Tcutil_addressof_props(te,e);_tmp2A9=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2AA,0)){
const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_terr(e->loc,((_tmp7C7="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp7C7,sizeof(char),60))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}{
# 1323
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2A9){
tq.print_const=1;
tq.real_const=1;}{
# 1329
void*t=Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2AA,tq,Cyc_Absyn_false_conref);
void*_tmp2AD=t;_npop_handler(0);return _tmp2AD;};};};
# 1282
;_pop_region(r);};}
# 1334
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1338
return Cyc_Absyn_uint_typ;
# 1340
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp7CB;void*_tmp7CA[1];struct Cyc_String_pa_PrintArg_struct _tmp7C9;(_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7CA[0]=& _tmp7C9,Cyc_Tcutil_terr(loc,((_tmp7CB="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp7CB,sizeof(char),55))),_tag_dyneither(_tmp7CA,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2B1=Cyc_Tcutil_compress(*topt);void*_tmp2B2;void*_tmp2B4;_LL172: _tmp2B2=_tmp2B1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2B3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B2;if(_tmp2B3->tag != 19)goto _LL174;else{_tmp2B4=(void*)_tmp2B3->f1;}};_LL173: {
# 1349
struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp7CE;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7CD;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2B6=(_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CE.tag=18,((_tmp7CE.f1=_tmp2B5,_tmp7CE)))),_tmp7CD)));
if(Cyc_Tcutil_unify(_tmp2B4,(void*)_tmp2B6))return _tmp2B2;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1355
return Cyc_Absyn_uint_typ;}
# 1358
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}
# 1365
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,void*n){
# 1367
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
{void*_tmp2B9=n;struct _dyneither_ptr*_tmp2BB;unsigned int _tmp2BD;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2BA=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2B9;if(_tmp2BA->tag != 0)goto _LL179;else{_tmp2BB=_tmp2BA->f1;}}_LL178: {
# 1371
int bad_type=1;
{void*_tmp2BE=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl**_tmp2C0;struct Cyc_List_List*_tmp2C2;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2BF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2BE;if(_tmp2BF->tag != 11)goto _LL17E;else{if((((_tmp2BF->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2C0=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2BF->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1374
 if((*_tmp2C0)->impl == 0)
goto _LL17B;
if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2BB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2C0)->impl))->fields)){
const char*_tmp7D2;void*_tmp7D1[1];struct Cyc_String_pa_PrintArg_struct _tmp7D0;(_tmp7D0.tag=0,((_tmp7D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2BB),((_tmp7D1[0]=& _tmp7D0,Cyc_Tcutil_terr(loc,((_tmp7D2="no field of struct/union has name %s",_tag_dyneither(_tmp7D2,sizeof(char),37))),_tag_dyneither(_tmp7D1,sizeof(void*),1)))))));}
bad_type=0;
goto _LL17B;_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BE;if(_tmp2C1->tag != 12)goto _LL180;else{_tmp2C2=_tmp2C1->f2;}}_LL17F:
# 1381
 if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2BB,_tmp2C2)){
const char*_tmp7D6;void*_tmp7D5[1];struct Cyc_String_pa_PrintArg_struct _tmp7D4;(_tmp7D4.tag=0,((_tmp7D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2BB),((_tmp7D5[0]=& _tmp7D4,Cyc_Tcutil_terr(loc,((_tmp7D6="no field of struct/union has name %s",_tag_dyneither(_tmp7D6,sizeof(char),37))),_tag_dyneither(_tmp7D5,sizeof(void*),1)))))));}
bad_type=0;
goto _LL17B;_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1387
if(bad_type){
const char*_tmp7DA;void*_tmp7D9[1];struct Cyc_String_pa_PrintArg_struct _tmp7D8;(_tmp7D8.tag=0,((_tmp7D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7D9[0]=& _tmp7D8,Cyc_Tcutil_terr(loc,((_tmp7DA="%s is not a known struct/union type",_tag_dyneither(_tmp7DA,sizeof(char),36))),_tag_dyneither(_tmp7D9,sizeof(void*),1)))))));}
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2BC=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2B9;if(_tmp2BC->tag != 1)goto _LL176;else{_tmp2BD=_tmp2BC->f1;}}_LL17A: {
# 1391
int bad_type=1;
{void*_tmp2CC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Aggrdecl**_tmp2CE;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Datatypefield*_tmp2D4;_LL183: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2CD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CC;if(_tmp2CD->tag != 11)goto _LL185;else{if((((_tmp2CD->f1).aggr_info).KnownAggr).tag != 2)goto _LL185;_tmp2CE=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2CD->f1).aggr_info).KnownAggr).val;}}_LL184:
# 1394
 if((*_tmp2CE)->impl == 0)
goto _LL182;
_tmp2D0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2CE)->impl))->fields;goto _LL186;_LL185: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2CF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CC;if(_tmp2CF->tag != 12)goto _LL187;else{_tmp2D0=_tmp2CF->f2;}}_LL186:
# 1398
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D0)<= _tmp2BD){
const char*_tmp7DF;void*_tmp7DE[2];struct Cyc_Int_pa_PrintArg_struct _tmp7DD;struct Cyc_Int_pa_PrintArg_struct _tmp7DC;(_tmp7DC.tag=1,((_tmp7DC.f1=(unsigned long)((int)_tmp2BD),((_tmp7DD.tag=1,((_tmp7DD.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D0),((_tmp7DE[0]=& _tmp7DD,((_tmp7DE[1]=& _tmp7DC,Cyc_Tcutil_terr(loc,((_tmp7DF="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp7DF,sizeof(char),46))),_tag_dyneither(_tmp7DE,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL187: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2D1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2CC;if(_tmp2D1->tag != 10)goto _LL189;else{_tmp2D2=_tmp2D1->f1;}}_LL188:
# 1404
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D2)<= _tmp2BD){
const char*_tmp7E4;void*_tmp7E3[2];struct Cyc_Int_pa_PrintArg_struct _tmp7E2;struct Cyc_Int_pa_PrintArg_struct _tmp7E1;(_tmp7E1.tag=1,((_tmp7E1.f1=(unsigned long)((int)_tmp2BD),((_tmp7E2.tag=1,((_tmp7E2.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D2),((_tmp7E3[0]=& _tmp7E2,((_tmp7E3[1]=& _tmp7E1,Cyc_Tcutil_terr(loc,((_tmp7E4="tuple has too few components: %d <= %d",_tag_dyneither(_tmp7E4,sizeof(char),39))),_tag_dyneither(_tmp7E3,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL189: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2CC;if(_tmp2D3->tag != 4)goto _LL18B;else{if((((_tmp2D3->f1).field_info).KnownDatatypefield).tag != 2)goto _LL18B;_tmp2D4=((struct _tuple2)(((_tmp2D3->f1).field_info).KnownDatatypefield).val).f2;}}_LL18A:
# 1410
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D4->typs)< _tmp2BD){
const char*_tmp7E9;void*_tmp7E8[2];struct Cyc_Int_pa_PrintArg_struct _tmp7E7;struct Cyc_Int_pa_PrintArg_struct _tmp7E6;(_tmp7E6.tag=1,((_tmp7E6.f1=(unsigned long)((int)_tmp2BD),((_tmp7E7.tag=1,((_tmp7E7.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2D4->typs),((_tmp7E8[0]=& _tmp7E7,((_tmp7E8[1]=& _tmp7E6,Cyc_Tcutil_terr(loc,((_tmp7E9="datatype field has too few components: %d < %d",_tag_dyneither(_tmp7E9,sizeof(char),47))),_tag_dyneither(_tmp7E8,sizeof(void*),2)))))))))))));}
bad_type=0;
goto _LL182;_LL18B:;_LL18C:
 goto _LL182;_LL182:;}
# 1417
if(bad_type){
const char*_tmp7ED;void*_tmp7EC[1];struct Cyc_String_pa_PrintArg_struct _tmp7EB;(_tmp7EB.tag=0,((_tmp7EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7EC[0]=& _tmp7EB,Cyc_Tcutil_terr(loc,((_tmp7ED="%s is not a known type",_tag_dyneither(_tmp7ED,sizeof(char),23))),_tag_dyneither(_tmp7EC,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1421
return Cyc_Absyn_uint_typ;}
# 1425
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp2E4=_new_region("r");struct _RegionHandle*r=& _tmp2E4;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp2E5=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp2E5,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp2E6=t;void*_tmp2E9;void*_tmp2EA;union Cyc_Absyn_Constraint*_tmp2EB;union Cyc_Absyn_Constraint*_tmp2EC;_LL18E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E6;if(_tmp2E7->tag != 1)goto _LL190;}_LL18F: {
# 1432
struct Cyc_List_List*_tmp2ED=Cyc_Tcenv_lookup_type_vars(_tmp2E5);
struct Cyc_Core_Opt*_tmp7EE;void*_tmp2EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp7EE=_cycalloc(sizeof(*_tmp7EE)),((_tmp7EE->v=_tmp2ED,_tmp7EE)))));
struct Cyc_Core_Opt*_tmp7EF;void*_tmp2EF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp7EF=_cycalloc(sizeof(*_tmp7EF)),((_tmp7EF->v=_tmp2ED,_tmp7EF)))));
union Cyc_Absyn_Constraint*_tmp2F0=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp2F1=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp7F0;struct Cyc_Absyn_PtrAtts _tmp2F2=(_tmp7F0.rgn=_tmp2EF,((_tmp7F0.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp7F0.bounds=_tmp2F0,((_tmp7F0.zero_term=_tmp2F1,((_tmp7F0.ptrloc=0,_tmp7F0)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7F6;struct Cyc_Absyn_PtrInfo _tmp7F5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F3=(_tmp7F4=_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F6.tag=5,((_tmp7F6.f1=((_tmp7F5.elt_typ=_tmp2EE,((_tmp7F5.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp7F5.ptr_atts=_tmp2F2,_tmp7F5)))))),_tmp7F6)))),_tmp7F4)));
Cyc_Tcutil_unify(t,(void*)_tmp2F3);
_tmp2E9=_tmp2EE;_tmp2EA=_tmp2EF;_tmp2EB=_tmp2F0;_tmp2EC=_tmp2F1;goto _LL191;}_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E8->tag != 5)goto _LL192;else{_tmp2E9=(_tmp2E8->f1).elt_typ;_tmp2EA=((_tmp2E8->f1).ptr_atts).rgn;_tmp2EB=((_tmp2E8->f1).ptr_atts).bounds;_tmp2EC=((_tmp2E8->f1).ptr_atts).zero_term;}}_LL191:
# 1442
 Cyc_Tcenv_check_rgn_accessible(_tmp2E5,loc,_tmp2EA);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2EB);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2E9),& Cyc_Tcutil_tmk)){
const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,Cyc_Tcutil_terr(loc,((_tmp7F9="can't dereference abstract pointer type",_tag_dyneither(_tmp7F9,sizeof(char),40))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}{
void*_tmp2FC=_tmp2E9;_npop_handler(0);return _tmp2FC;};_LL192:;_LL193: {
# 1448
const char*_tmp7FD;void*_tmp7FC[1];struct Cyc_String_pa_PrintArg_struct _tmp7FB;void*_tmp300=(_tmp7FB.tag=0,((_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7FC[0]=& _tmp7FB,Cyc_Tcexp_expr_err(_tmp2E5,loc,topt,((_tmp7FD="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp7FD,sizeof(char),39))),_tag_dyneither(_tmp7FC,sizeof(void*),1)))))));_npop_handler(0);return _tmp300;}_LL18D:;};}
# 1427
;_pop_region(r);}
# 1454
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1458
struct _RegionHandle _tmp301=_new_region("r");struct _RegionHandle*r=& _tmp301;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1462
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp302=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Aggrdecl*_tmp304;struct Cyc_List_List*_tmp305;enum Cyc_Absyn_AggrKind _tmp307;struct Cyc_List_List*_tmp308;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp303=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp302;if(_tmp303->tag != 11)goto _LL197;else{if((((_tmp303->f1).aggr_info).KnownAggr).tag != 2)goto _LL197;_tmp304=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp303->f1).aggr_info).KnownAggr).val);_tmp305=(_tmp303->f1).targs;}}_LL196: {
# 1465
struct Cyc_Absyn_Aggrfield*_tmp30D=Cyc_Absyn_lookup_decl_field(_tmp304,f);
if(_tmp30D == 0){
const char*_tmp802;void*_tmp801[2];struct Cyc_String_pa_PrintArg_struct _tmp800;struct Cyc_String_pa_PrintArg_struct _tmp7FF;void*_tmp312=(_tmp7FF.tag=0,((_tmp7FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp800.tag=0,((_tmp800.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp304->name)),((_tmp801[0]=& _tmp800,((_tmp801[1]=& _tmp7FF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp802="type %s has no %s field",_tag_dyneither(_tmp802,sizeof(char),24))),_tag_dyneither(_tmp801,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp312;}
# 1470
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged)*is_tagged=1;{
void*t2=_tmp30D->type;
if(_tmp305 != 0){
struct _RegionHandle _tmp313=_new_region("rgn");struct _RegionHandle*rgn=& _tmp313;_push_region(rgn);
{struct Cyc_List_List*_tmp314=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp304->tvs,_tmp305);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp314,_tmp30D->type);}
# 1474
;_pop_region(rgn);}
# 1479
if(((_tmp304->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
const char*_tmp806;void*_tmp805[1];struct Cyc_String_pa_PrintArg_struct _tmp804;void*_tmp318=(_tmp804.tag=0,((_tmp804.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp805[0]=& _tmp804,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp806="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp806,sizeof(char),56))),_tag_dyneither(_tmp805,sizeof(void*),1)))))));_npop_handler(0);return _tmp318;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->exist_vars != 0){
# 1484
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp80A;void*_tmp809[1];struct Cyc_String_pa_PrintArg_struct _tmp808;void*_tmp31C=(_tmp808.tag=0,((_tmp808.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp809[0]=& _tmp808,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80A="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp80A,sizeof(char),81))),_tag_dyneither(_tmp809,sizeof(void*),1)))))));_npop_handler(0);return _tmp31C;}}{
# 1487
void*_tmp31D=t2;_npop_handler(0);return _tmp31D;};};}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp306=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp302;if(_tmp306->tag != 12)goto _LL199;else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;}}_LL198: {
# 1489
struct Cyc_Absyn_Aggrfield*_tmp31E=Cyc_Absyn_lookup_field(_tmp308,f);
if(_tmp31E == 0){
const char*_tmp80E;void*_tmp80D[1];struct Cyc_String_pa_PrintArg_struct _tmp80C;void*_tmp322=(_tmp80C.tag=0,((_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp80D[0]=& _tmp80C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80E="type has no %s field",_tag_dyneither(_tmp80E,sizeof(char),21))),_tag_dyneither(_tmp80D,sizeof(void*),1)))))));_npop_handler(0);return _tmp322;}
if((_tmp307 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp31E->type)){
const char*_tmp812;void*_tmp811[1];struct Cyc_String_pa_PrintArg_struct _tmp810;void*_tmp326=(_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp811[0]=& _tmp810,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp812="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp812,sizeof(char),56))),_tag_dyneither(_tmp811,sizeof(void*),1)))))));_npop_handler(0);return _tmp326;}{
void*_tmp327=_tmp31E->type;_npop_handler(0);return _tmp327;};}_LL199:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp309=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp302;if(_tmp309->tag != 8)goto _LL19B;}{const char*_tmp813;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp813="size",_tag_dyneither(_tmp813,sizeof(char),5))))== 0))goto _LL19B;};_LL19A: goto _LL19C;_LL19B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302;if(_tmp30B->tag != 5)goto _LL19D;}{const char*_tmp814;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp814="size",_tag_dyneither(_tmp814,sizeof(char),5))))== 0))goto _LL19D;};_LL19C:
{const char*_tmp818;void*_tmp817[1];struct Cyc_String_pa_PrintArg_struct _tmp816;(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp817[0]=& _tmp816,Cyc_Tcutil_warn(e->loc,((_tmp818="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp818,sizeof(char),54))),_tag_dyneither(_tmp817,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp81E;struct Cyc_List_List*_tmp81D;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp81C;outer_e->r=(void*)((_tmp81C=_cycalloc(sizeof(*_tmp81C)),((_tmp81C[0]=((_tmp81E.tag=2,((_tmp81E.f1=Cyc_Absyn_Numelts,((_tmp81E.f2=((_tmp81D=_cycalloc(sizeof(*_tmp81D)),((_tmp81D->hd=e,((_tmp81D->tl=0,_tmp81D)))))),_tmp81E)))))),_tmp81C))));}{
void*_tmp32E=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp32E;};_LL19D:;_LL19E: {
# 1501
const char*_tmp81F;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp81F="size",_tag_dyneither(_tmp81F,sizeof(char),5))))== 0){
const char*_tmp823;void*_tmp822[1];struct Cyc_String_pa_PrintArg_struct _tmp821;void*_tmp333=(_tmp821.tag=0,((_tmp821.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp822[0]=& _tmp821,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp823="expecting struct, union, or array, found %s",_tag_dyneither(_tmp823,sizeof(char),44))),_tag_dyneither(_tmp822,sizeof(void*),1)))))));_npop_handler(0);return _tmp333;}else{
# 1506
const char*_tmp827;void*_tmp826[1];struct Cyc_String_pa_PrintArg_struct _tmp825;void*_tmp337=(_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp826[0]=& _tmp825,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp827="expecting struct or union, found %s",_tag_dyneither(_tmp827,sizeof(char),36))),_tag_dyneither(_tmp826,sizeof(void*),1)))))));_npop_handler(0);return _tmp337;}}_LL194:;};
# 1458
;_pop_region(r);}
# 1512
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1515
struct _RegionHandle _tmp338=_new_region("r");struct _RegionHandle*r=& _tmp338;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1519
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp339=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp33B;void*_tmp33C;union Cyc_Absyn_Constraint*_tmp33D;union Cyc_Absyn_Constraint*_tmp33E;_LL1A0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp339;if(_tmp33A->tag != 5)goto _LL1A2;else{_tmp33B=(_tmp33A->f1).elt_typ;_tmp33C=((_tmp33A->f1).ptr_atts).rgn;_tmp33D=((_tmp33A->f1).ptr_atts).bounds;_tmp33E=((_tmp33A->f1).ptr_atts).zero_term;}}_LL1A1:
# 1522
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp33D);
{void*_tmp33F=Cyc_Tcutil_compress(_tmp33B);struct Cyc_Absyn_Aggrdecl*_tmp341;struct Cyc_List_List*_tmp342;enum Cyc_Absyn_AggrKind _tmp344;struct Cyc_List_List*_tmp345;_LL1A5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33F;if(_tmp340->tag != 11)goto _LL1A7;else{if((((_tmp340->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A7;_tmp341=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp340->f1).aggr_info).KnownAggr).val);_tmp342=(_tmp340->f1).targs;}}_LL1A6: {
# 1525
struct Cyc_Absyn_Aggrfield*_tmp346=Cyc_Absyn_lookup_decl_field(_tmp341,f);
if(_tmp346 == 0){
const char*_tmp82C;void*_tmp82B[2];struct Cyc_String_pa_PrintArg_struct _tmp82A;struct Cyc_String_pa_PrintArg_struct _tmp829;void*_tmp34B=(_tmp829.tag=0,((_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp82A.tag=0,((_tmp82A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp341->name)),((_tmp82B[0]=& _tmp82A,((_tmp82B[1]=& _tmp829,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp82C="type %s has no %s field",_tag_dyneither(_tmp82C,sizeof(char),24))),_tag_dyneither(_tmp82B,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp34B;}
# 1530
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp341->impl))->tagged)*is_tagged=1;{
void*t3=_tmp346->type;
if(_tmp342 != 0){
struct _RegionHandle _tmp34C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34C;_push_region(rgn);
{struct Cyc_List_List*_tmp34D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp341->tvs,_tmp342);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp34D,_tmp346->type);}
# 1534
;_pop_region(rgn);}{
# 1538
struct Cyc_Absyn_Kind*_tmp34E=Cyc_Tcutil_typ_kind(t3);
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp34E)){
# 1541
void*_tmp34F=Cyc_Tcutil_compress(t3);_LL1AC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp350=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34F;if(_tmp350->tag != 8)goto _LL1AE;}_LL1AD:
 goto _LL1AB;_LL1AE:;_LL1AF: {
# 1544
const char*_tmp830;void*_tmp82F[1];struct Cyc_String_pa_PrintArg_struct _tmp82E;void*_tmp354=(_tmp82E.tag=0,((_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp82F[0]=& _tmp82E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp830="cannot get member %s since its type is abstract",_tag_dyneither(_tmp830,sizeof(char),48))),_tag_dyneither(_tmp82F,sizeof(void*),1)))))));_npop_handler(0);return _tmp354;}_LL1AB:;}
# 1547
if(((_tmp341->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp341->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
const char*_tmp834;void*_tmp833[1];struct Cyc_String_pa_PrintArg_struct _tmp832;void*_tmp358=(_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp833[0]=& _tmp832,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp834="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp834,sizeof(char),56))),_tag_dyneither(_tmp833,sizeof(void*),1)))))));_npop_handler(0);return _tmp358;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp341->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp838;void*_tmp837[1];struct Cyc_String_pa_PrintArg_struct _tmp836;void*_tmp35C=(_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp837[0]=& _tmp836,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp838="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp838,sizeof(char),72))),_tag_dyneither(_tmp837,sizeof(void*),1)))))));_npop_handler(0);return _tmp35C;}}{
# 1555
void*_tmp35D=t3;_npop_handler(0);return _tmp35D;};};};}_LL1A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33F;if(_tmp343->tag != 12)goto _LL1A9;else{_tmp344=_tmp343->f1;_tmp345=_tmp343->f2;}}_LL1A8: {
# 1557
struct Cyc_Absyn_Aggrfield*_tmp35E=Cyc_Absyn_lookup_field(_tmp345,f);
if(_tmp35E == 0){
const char*_tmp83C;void*_tmp83B[1];struct Cyc_String_pa_PrintArg_struct _tmp83A;void*_tmp362=(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp83B[0]=& _tmp83A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp83C="type has no %s field",_tag_dyneither(_tmp83C,sizeof(char),21))),_tag_dyneither(_tmp83B,sizeof(void*),1)))))));_npop_handler(0);return _tmp362;}
if((_tmp344 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp35E->type)){
const char*_tmp840;void*_tmp83F[1];struct Cyc_String_pa_PrintArg_struct _tmp83E;void*_tmp366=(_tmp83E.tag=0,((_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp83F[0]=& _tmp83E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp840="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp840,sizeof(char),56))),_tag_dyneither(_tmp83F,sizeof(void*),1)))))));_npop_handler(0);return _tmp366;}{
void*_tmp367=_tmp35E->type;_npop_handler(0);return _tmp367;};}_LL1A9:;_LL1AA:
 goto _LL1A4;_LL1A4:;}
# 1565
goto _LL19F;_LL1A2:;_LL1A3:
 goto _LL19F;_LL19F:;}{
# 1568
const char*_tmp844;void*_tmp843[1];struct Cyc_String_pa_PrintArg_struct _tmp842;void*_tmp36B=(_tmp842.tag=0,((_tmp842.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp843[0]=& _tmp842,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp844="expecting struct or union pointer, found %s",_tag_dyneither(_tmp844,sizeof(char),44))),_tag_dyneither(_tmp843,sizeof(void*),1)))))));_npop_handler(0);return _tmp36B;};
# 1515
;_pop_region(r);}
# 1573
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1575
unsigned int _tmp36D;int _tmp36E;struct _tuple15 _tmp36C=Cyc_Evexp_eval_const_uint_exp(index);_tmp36D=_tmp36C.f1;_tmp36E=_tmp36C.f2;
if(!_tmp36E){
const char*_tmp847;void*_tmp846;return(_tmp846=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp847="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp847,sizeof(char),47))),_tag_dyneither(_tmp846,sizeof(void*),0)));}{
# 1579
struct _handler_cons _tmp371;_push_handler(& _tmp371);{int _tmp373=0;if(setjmp(_tmp371.handler))_tmp373=1;if(!_tmp373){
{void*_tmp374=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp36D)).f2;_npop_handler(0);return _tmp374;};_pop_handler();}else{void*_tmp372=(void*)_exn_thrown;void*_tmp376=_tmp372;void*_tmp378;_LL1B1: {struct Cyc_List_Nth_exn_struct*_tmp377=(struct Cyc_List_Nth_exn_struct*)_tmp376;if(_tmp377->tag != Cyc_List_Nth)goto _LL1B3;}_LL1B2: {
# 1582
const char*_tmp84C;void*_tmp84B[2];struct Cyc_Int_pa_PrintArg_struct _tmp84A;struct Cyc_Int_pa_PrintArg_struct _tmp849;return(_tmp849.tag=1,((_tmp849.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp84A.tag=1,((_tmp84A.f1=(unsigned long)((int)_tmp36D),((_tmp84B[0]=& _tmp84A,((_tmp84B[1]=& _tmp849,Cyc_Tcexp_expr_err(te,loc,0,((_tmp84C="index is %d but tuple has only %d fields",_tag_dyneither(_tmp84C,sizeof(char),41))),_tag_dyneither(_tmp84B,sizeof(void*),2)))))))))))));}_LL1B3: _tmp378=_tmp376;_LL1B4:(void)_throw(_tmp378);_LL1B0:;}};};}
# 1587
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1589
struct _RegionHandle _tmp37D=_new_region("r");struct _RegionHandle*r=& _tmp37D;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp37E=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp37E,0,e1);
Cyc_Tcexp_tcExp(_tmp37E,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp37E,e2)){
const char*_tmp850;void*_tmp84F[1];struct Cyc_String_pa_PrintArg_struct _tmp84E;void*_tmp382=(_tmp84E.tag=0,((_tmp84E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp84F[0]=& _tmp84E,Cyc_Tcexp_expr_err(_tmp37E,e2->loc,topt,((_tmp850="expecting int subscript, found %s",_tag_dyneither(_tmp850,sizeof(char),34))),_tag_dyneither(_tmp84F,sizeof(void*),1)))))));_npop_handler(0);return _tmp382;}{
# 1600
void*_tmp383=t1;void*_tmp385;struct Cyc_Absyn_Tqual _tmp386;void*_tmp387;union Cyc_Absyn_Constraint*_tmp388;union Cyc_Absyn_Constraint*_tmp389;struct Cyc_List_List*_tmp38B;_LL1B6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383;if(_tmp384->tag != 5)goto _LL1B8;else{_tmp385=(_tmp384->f1).elt_typ;_tmp386=(_tmp384->f1).elt_tq;_tmp387=((_tmp384->f1).ptr_atts).rgn;_tmp388=((_tmp384->f1).ptr_atts).bounds;_tmp389=((_tmp384->f1).ptr_atts).zero_term;}}_LL1B7:
# 1604
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp389)){
int emit_warning=1;
{void*_tmp38C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp388);struct Cyc_Absyn_Exp*_tmp38E;_LL1BD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp38D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp38C;if(_tmp38D->tag != 1)goto _LL1BF;else{_tmp38E=_tmp38D->f1;}}_LL1BE:
# 1608
 if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp391;int _tmp392;struct _tuple15 _tmp390=Cyc_Evexp_eval_const_uint_exp(e2);_tmp391=_tmp390.f1;_tmp392=_tmp390.f2;
if(_tmp392){
unsigned int _tmp394;int _tmp395;struct _tuple15 _tmp393=Cyc_Evexp_eval_const_uint_exp(_tmp38E);_tmp394=_tmp393.f1;_tmp395=_tmp393.f2;
if(_tmp395  && _tmp394 > _tmp391)emit_warning=0;}}
# 1615
goto _LL1BC;_LL1BF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp38F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp38C;if(_tmp38F->tag != 0)goto _LL1BC;}_LL1C0:
 emit_warning=0;goto _LL1BC;_LL1BC:;}
# 1618
if(emit_warning){
const char*_tmp853;void*_tmp852;(_tmp852=0,Cyc_Tcutil_warn(e2->loc,((_tmp853="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp853,sizeof(char),63))),_tag_dyneither(_tmp852,sizeof(void*),0)));}}else{
# 1622
if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp399;int _tmp39A;struct _tuple15 _tmp398=Cyc_Evexp_eval_const_uint_exp(e2);_tmp399=_tmp398.f1;_tmp39A=_tmp398.f2;
if(_tmp39A)
Cyc_Tcutil_check_bound(loc,_tmp399,_tmp388);}else{
# 1628
if(Cyc_Tcutil_is_bound_one(_tmp388) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp389)){
const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_warn(e1->loc,((_tmp856="subscript applied to pointer to one object",_tag_dyneither(_tmp856,sizeof(char),43))),_tag_dyneither(_tmp855,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp388);}}
# 1633
Cyc_Tcenv_check_rgn_accessible(_tmp37E,loc,_tmp387);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp385),& Cyc_Tcutil_tmk)){
const char*_tmp859;void*_tmp858;(_tmp858=0,Cyc_Tcutil_terr(e1->loc,((_tmp859="can't subscript an abstract pointer",_tag_dyneither(_tmp859,sizeof(char),36))),_tag_dyneither(_tmp858,sizeof(void*),0)));}{
void*_tmp39F=_tmp385;_npop_handler(0);return _tmp39F;};_LL1B8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp383;if(_tmp38A->tag != 10)goto _LL1BA;else{_tmp38B=_tmp38A->f1;}}_LL1B9: {
void*_tmp3A0=Cyc_Tcexp_ithTupleType(_tmp37E,loc,_tmp38B,e2);_npop_handler(0);return _tmp3A0;}_LL1BA:;_LL1BB: {
const char*_tmp85D;void*_tmp85C[1];struct Cyc_String_pa_PrintArg_struct _tmp85B;void*_tmp3A4=(_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp85C[0]=& _tmp85B,Cyc_Tcexp_expr_err(_tmp37E,loc,topt,((_tmp85D="subscript applied to %s",_tag_dyneither(_tmp85D,sizeof(char),24))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))));_npop_handler(0);return _tmp3A4;}_LL1B5:;};};}
# 1590
;_pop_region(r);}
# 1644
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp3A5=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3A7;_LL1C2: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A5;if(_tmp3A6->tag != 10)goto _LL1C4;else{_tmp3A7=_tmp3A6->f1;}}_LL1C3:
# 1650
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3A7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1653
goto _LL1C1;
# 1655
for(0;es != 0;(es=es->tl,_tmp3A7=_tmp3A7->tl)){
int bogus=0;
void*_tmp3A8=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3A7))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3A8,(struct Cyc_Absyn_Exp*)es->hd);
# 1660
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3A7->hd)).f2,& bogus);{
struct _tuple19*_tmp860;struct Cyc_List_List*_tmp85F;fields=((_tmp85F=_cycalloc(sizeof(*_tmp85F)),((_tmp85F->hd=((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860->f1=(*((struct _tuple19*)_tmp3A7->hd)).f1,((_tmp860->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp860)))))),((_tmp85F->tl=fields,_tmp85F))))));};}
# 1663
done=1;
goto _LL1C1;_LL1C4:;_LL1C5:
 goto _LL1C1;_LL1C1:;}
# 1667
if(!done)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple19*_tmp863;struct Cyc_List_List*_tmp862;fields=((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->hd=((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863->f1=Cyc_Absyn_empty_tqual(0),((_tmp863->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp863)))))),((_tmp862->tl=fields,_tmp862))))));};}{
# 1672
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp866;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp865;return(void*)((_tmp865=_cycalloc(sizeof(*_tmp865)),((_tmp865[0]=((_tmp866.tag=10,((_tmp866.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp866)))),_tmp865))));};}
# 1676
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1679
const char*_tmp869;void*_tmp868;return(_tmp868=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp869="tcCompoundLit",_tag_dyneither(_tmp869,sizeof(char),14))),_tag_dyneither(_tmp868,sizeof(void*),0)));}
# 1689 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1692
void*res_t2;
struct _RegionHandle _tmp3B1=_new_region("r");struct _RegionHandle*r=& _tmp3B1;_push_region(r);{
int _tmp3B2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp86C;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp86B;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3B3=(_tmp86B=_cycalloc(sizeof(*_tmp86B)),((_tmp86B[0]=((_tmp86C.tag=0,((_tmp86C.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3B2),_tmp86C)))),_tmp86B)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3B3,loc);
# 1701
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3B4=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3B2 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3B4)){
const char*_tmp86F;void*_tmp86E;(_tmp86E=0,Cyc_Tcutil_terr(_tmp3B4->loc,((_tmp86F="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp86F,sizeof(char),45))),_tag_dyneither(_tmp86E,sizeof(void*),0)));}}
# 1706
sz_exp->topt=(void*)Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1711
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1714
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1716
const char*_tmp873;void*_tmp872[1];struct Cyc_String_pa_PrintArg_struct _tmp871;(_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp872[0]=& _tmp871,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp873="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp873,sizeof(char),53))),_tag_dyneither(_tmp872,sizeof(void*),1)))))));}}
# 1720
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1725
void*_tmp3BC=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp3BF;_LL1C7: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3BD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3BC;if(_tmp3BD->tag != 1)goto _LL1C9;}_LL1C8:
# 1727
{const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_terr(loc,((_tmp876="only array index designators are supported",_tag_dyneither(_tmp876,sizeof(char),43))),_tag_dyneither(_tmp875,sizeof(void*),0)));}
goto _LL1C6;_LL1C9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3BE=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3BC;if(_tmp3BE->tag != 0)goto _LL1C6;else{_tmp3BF=_tmp3BE->f1;}}_LL1CA:
# 1730
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3BF);{
unsigned int _tmp3C3;int _tmp3C4;struct _tuple15 _tmp3C2=Cyc_Evexp_eval_const_uint_exp(_tmp3BF);_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;
if(!_tmp3C4){
const char*_tmp879;void*_tmp878;(_tmp878=0,Cyc_Tcutil_terr(_tmp3BF->loc,((_tmp879="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp879,sizeof(char),47))),_tag_dyneither(_tmp878,sizeof(void*),0)));}else{
if(_tmp3C3 != offset){
const char*_tmp87E;void*_tmp87D[2];struct Cyc_Int_pa_PrintArg_struct _tmp87C;struct Cyc_Int_pa_PrintArg_struct _tmp87B;(_tmp87B.tag=1,((_tmp87B.f1=(unsigned long)((int)_tmp3C3),((_tmp87C.tag=1,((_tmp87C.f1=(unsigned long)offset,((_tmp87D[0]=& _tmp87C,((_tmp87D[1]=& _tmp87B,Cyc_Tcutil_terr(_tmp3BF->loc,((_tmp87E="expecting index designator %d but found %d",_tag_dyneither(_tmp87E,sizeof(char),43))),_tag_dyneither(_tmp87D,sizeof(void*),2)))))))))))));}}
# 1737
goto _LL1C6;};_LL1C6:;}}}{
# 1741
void*_tmp3CB=res_t2;_npop_handler(0);return _tmp3CB;};
# 1693
;_pop_region(r);}
# 1745
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1748
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3CC=Cyc_Tcutil_compress((void*)_check_null(bound->topt));_LL1CC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3CD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3CC;if(_tmp3CD->tag != 19)goto _LL1CE;}_LL1CD:
 goto _LL1CB;_LL1CE:;_LL1CF:
# 1752
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp882;void*_tmp881[1];struct Cyc_String_pa_PrintArg_struct _tmp880;(_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp881[0]=& _tmp880,Cyc_Tcutil_terr(bound->loc,((_tmp882="expecting unsigned int, found %s",_tag_dyneither(_tmp882,sizeof(char),33))),_tag_dyneither(_tmp881,sizeof(void*),1)))))));}_LL1CB:;}
# 1757
if(!(vd->tq).real_const){
const char*_tmp885;void*_tmp884;(_tmp884=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp885="comprehension index variable is not declared const!",_tag_dyneither(_tmp885,sizeof(char),52))),_tag_dyneither(_tmp884,sizeof(void*),0)));}{
# 1760
struct _RegionHandle _tmp3D3=_new_region("r");struct _RegionHandle*r=& _tmp3D3;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1766
struct Cyc_Tcenv_Tenv*_tmp3D4=te2;
void**_tmp3D5=0;
struct Cyc_Absyn_Tqual*_tmp3D6=0;
union Cyc_Absyn_Constraint**_tmp3D7=0;
# 1771
if(topt != 0){
void*_tmp3D8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3DA;void*_tmp3DC;struct Cyc_Absyn_Tqual _tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;union Cyc_Absyn_Constraint*_tmp3DF;_LL1D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D8;if(_tmp3D9->tag != 5)goto _LL1D3;else{_tmp3DA=_tmp3D9->f1;}}_LL1D2:
# 1774
{void**_tmp886;_tmp3D5=((_tmp886=_region_malloc(r,sizeof(*_tmp886)),((_tmp886[0]=_tmp3DA.elt_typ,_tmp886))));}
{struct Cyc_Absyn_Tqual*_tmp887;_tmp3D6=((_tmp887=_region_malloc(r,sizeof(*_tmp887)),((_tmp887[0]=_tmp3DA.elt_tq,_tmp887))));}
{union Cyc_Absyn_Constraint**_tmp888;_tmp3D7=((_tmp888=_region_malloc(r,sizeof(*_tmp888)),((_tmp888[0]=(_tmp3DA.ptr_atts).zero_term,_tmp888))));}
goto _LL1D0;_LL1D3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3DB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D8;if(_tmp3DB->tag != 8)goto _LL1D5;else{_tmp3DC=(_tmp3DB->f1).elt_type;_tmp3DD=(_tmp3DB->f1).tq;_tmp3DE=(_tmp3DB->f1).num_elts;_tmp3DF=(_tmp3DB->f1).zero_term;}}_LL1D4:
# 1779
{void**_tmp889;_tmp3D5=((_tmp889=_region_malloc(r,sizeof(*_tmp889)),((_tmp889[0]=_tmp3DC,_tmp889))));}
{struct Cyc_Absyn_Tqual*_tmp88A;_tmp3D6=((_tmp88A=_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A[0]=_tmp3DD,_tmp88A))));}
{union Cyc_Absyn_Constraint**_tmp88B;_tmp3D7=((_tmp88B=_region_malloc(r,sizeof(*_tmp88B)),((_tmp88B[0]=_tmp3DF,_tmp88B))));}
goto _LL1D0;_LL1D5:;_LL1D6:
# 1784
 goto _LL1D0;_LL1D0:;}{
# 1787
void*t=Cyc_Tcexp_tcExp(_tmp3D4,_tmp3D5,body);
if(_tmp3D4->le == 0){
# 1790
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp88E;void*_tmp88D;(_tmp88D=0,Cyc_Tcutil_terr(bound->loc,((_tmp88E="bound is not constant",_tag_dyneither(_tmp88E,sizeof(char),22))),_tag_dyneither(_tmp88D,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp891;void*_tmp890;(_tmp890=0,Cyc_Tcutil_terr(bound->loc,((_tmp891="body is not constant",_tag_dyneither(_tmp891,sizeof(char),21))),_tag_dyneither(_tmp890,sizeof(void*),0)));}}
# 1795
if(_tmp3D7 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3D7)){
# 1798
struct Cyc_Absyn_Exp*_tmp3EA=Cyc_Absyn_uint_exp(1,0);_tmp3EA->topt=(void*)Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp3EA,0);bound->topt=(void*)Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1803
void*_tmp3EB=Cyc_Absyn_array_typ(t,_tmp3D6 == 0?Cyc_Absyn_empty_tqual(0):*_tmp3D6,(struct Cyc_Absyn_Exp*)bound,
_tmp3D7 == 0?Cyc_Absyn_false_conref:*_tmp3D7,0);
void*_tmp3EC=_tmp3EB;_npop_handler(0);return _tmp3EC;};};};}
# 1761
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1819 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1823
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp892;adptr=((_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892[0]=ad,_tmp892))));};}else{
# 1829
{struct _handler_cons _tmp3EE;_push_handler(& _tmp3EE);{int _tmp3F0=0;if(setjmp(_tmp3EE.handler))_tmp3F0=1;if(!_tmp3F0){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1829
;_pop_handler();}else{void*_tmp3EF=(void*)_exn_thrown;void*_tmp3F2=_tmp3EF;void*_tmp3F4;_LL1D8: {struct Cyc_Dict_Absent_exn_struct*_tmp3F3=(struct Cyc_Dict_Absent_exn_struct*)_tmp3F2;if(_tmp3F3->tag != Cyc_Dict_Absent)goto _LL1DA;}_LL1D9:
# 1832
{const char*_tmp896;void*_tmp895[1];struct Cyc_String_pa_PrintArg_struct _tmp894;(_tmp894.tag=0,((_tmp894.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp895[0]=& _tmp894,Cyc_Tcutil_terr(loc,((_tmp896="unbound struct/union name %s",_tag_dyneither(_tmp896,sizeof(char),29))),_tag_dyneither(_tmp895,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1DA: _tmp3F4=_tmp3F2;_LL1DB:(void)_throw(_tmp3F4);_LL1D7:;}};}
# 1835
*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;
*tn=ad->name;}
# 1838
if(ad->impl == 0){
{const char*_tmp89E;void*_tmp89D[1];const char*_tmp89C;const char*_tmp89B;struct Cyc_String_pa_PrintArg_struct _tmp89A;(_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp89B="struct",_tag_dyneither(_tmp89B,sizeof(char),7)):((_tmp89C="union",_tag_dyneither(_tmp89C,sizeof(char),6)))),((_tmp89D[0]=& _tmp89A,Cyc_Tcutil_terr(loc,((_tmp89E="can't construct abstract %s",_tag_dyneither(_tmp89E,sizeof(char),28))),_tag_dyneither(_tmp89D,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1842
struct _RegionHandle _tmp3FD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3FD;_push_region(rgn);
# 1844
{struct _tuple12 _tmp89F;struct _tuple12 _tmp3FE=(_tmp89F.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp89F.f2=rgn,_tmp89F)));
struct Cyc_List_List*_tmp3FF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3FE,ad->tvs);
struct Cyc_List_List*_tmp400=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3FE,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp401=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3FF);
struct Cyc_List_List*_tmp402=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp400);
struct Cyc_List_List*_tmp403=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp3FF,_tmp400);
void*res_typ;
# 1855
if(topt != 0){
void*_tmp404=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Aggrdecl**_tmp406;struct Cyc_List_List*_tmp407;_LL1DD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp404;if(_tmp405->tag != 11)goto _LL1DF;else{if((((_tmp405->f1).aggr_info).KnownAggr).tag != 2)goto _LL1DF;_tmp406=(struct Cyc_Absyn_Aggrdecl**)(((_tmp405->f1).aggr_info).KnownAggr).val;_tmp407=(_tmp405->f1).targs;}}_LL1DE:
# 1858
 if(*_tmp406 == *adptr){
{struct Cyc_List_List*_tmp408=_tmp401;for(0;_tmp408 != 0  && _tmp407 != 0;
(_tmp408=_tmp408->tl,_tmp407=_tmp407->tl)){
Cyc_Tcutil_unify((void*)_tmp408->hd,(void*)_tmp407->hd);}}
# 1863
res_typ=*topt;
goto _LL1DC;}
# 1866
goto _LL1E0;_LL1DF:;_LL1E0: {
# 1868
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8A5;struct Cyc_Absyn_AggrInfo _tmp8A4;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8A3;res_typ=(void*)((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3[0]=((_tmp8A5.tag=11,((_tmp8A5.f1=((_tmp8A4.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8A4.targs=_tmp401,_tmp8A4)))),_tmp8A5)))),_tmp8A3))));}_LL1DC:;}else{
# 1871
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8AB;struct Cyc_Absyn_AggrInfo _tmp8AA;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8A9;res_typ=(void*)((_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=((_tmp8AB.tag=11,((_tmp8AB.f1=((_tmp8AA.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8AA.targs=_tmp401,_tmp8AA)))),_tmp8AB)))),_tmp8A9))));}{
# 1874
struct Cyc_List_List*_tmp40F=*ts;
struct Cyc_List_List*_tmp410=_tmp402;
while(_tmp40F != 0  && _tmp410 != 0){
# 1878
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp40F->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp40F->hd);
Cyc_Tcutil_unify((void*)_tmp40F->hd,(void*)_tmp410->hd);
_tmp40F=_tmp40F->tl;
_tmp410=_tmp410->tl;}
# 1884
if(_tmp40F != 0){
const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,Cyc_Tcutil_terr(loc,((_tmp8AE="too many explicit witness types",_tag_dyneither(_tmp8AE,sizeof(char),32))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}
# 1887
*ts=_tmp402;{
# 1890
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp414;struct Cyc_Absyn_Exp*_tmp415;struct _tuple20*_tmp413=(struct _tuple20*)fields->hd;_tmp414=(*_tmp413).f1;_tmp415=(*_tmp413).f2;{
void*_tmp416=Cyc_Tcutil_rsubstitute(rgn,_tmp403,_tmp414->type);
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp416,_tmp415);
if(!Cyc_Tcutil_coerce_arg(te,_tmp415,_tmp416,& bogus)){
{const char*_tmp8BA;void*_tmp8B9[5];struct Cyc_String_pa_PrintArg_struct _tmp8B8;const char*_tmp8B7;const char*_tmp8B6;struct Cyc_String_pa_PrintArg_struct _tmp8B5;struct Cyc_String_pa_PrintArg_struct _tmp8B4;struct Cyc_String_pa_PrintArg_struct _tmp8B3;struct Cyc_String_pa_PrintArg_struct _tmp8B2;(_tmp8B2.tag=0,((_tmp8B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1901
Cyc_Absynpp_typ2string((void*)_check_null(_tmp415->topt))),((_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1900
Cyc_Absynpp_typ2string(_tmp416)),((_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)(
# 1899
ad->kind == Cyc_Absyn_StructA?(_tmp8B6="struct",_tag_dyneither(_tmp8B6,sizeof(char),7)):((_tmp8B7="union",_tag_dyneither(_tmp8B7,sizeof(char),6)))),((_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp414->name),((_tmp8B9[0]=& _tmp8B8,((_tmp8B9[1]=& _tmp8B5,((_tmp8B9[2]=& _tmp8B4,((_tmp8B9[3]=& _tmp8B3,((_tmp8B9[4]=& _tmp8B2,Cyc_Tcutil_terr(_tmp415->loc,((_tmp8BA="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp8BA,sizeof(char),40))),_tag_dyneither(_tmp8B9,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 1902
Cyc_Tcutil_explain_failure();}};}{
# 1905
struct Cyc_List_List*_tmp420=0;
{struct Cyc_List_List*_tmp421=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp421 != 0;_tmp421=_tmp421->tl){
struct _tuple0*_tmp8BD;struct Cyc_List_List*_tmp8BC;_tmp420=((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC->hd=((_tmp8BD=_cycalloc(sizeof(*_tmp8BD)),((_tmp8BD->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp403,(*((struct _tuple0*)_tmp421->hd)).f1),((_tmp8BD->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp403,(*((struct _tuple0*)_tmp421->hd)).f2),_tmp8BD)))))),((_tmp8BC->tl=_tmp420,_tmp8BC))))));}}
# 1910
_tmp420=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp420);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp420);{
void*_tmp424=res_typ;_npop_handler(0);return _tmp424;};};};};}
# 1844
;_pop_region(rgn);};}
# 1916
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 1918
struct _RegionHandle _tmp426=_new_region("rgn");struct _RegionHandle*rgn=& _tmp426;_push_region(rgn);{
void*_tmp427=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp429;struct Cyc_List_List*_tmp42A;_LL1E2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp427;if(_tmp428->tag != 12)goto _LL1E4;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL1E3:
# 1921
 if(_tmp429 == Cyc_Absyn_UnionA){
const char*_tmp8C0;void*_tmp8BF;(_tmp8BF=0,Cyc_Tcutil_terr(loc,((_tmp8C0="expecting struct but found union",_tag_dyneither(_tmp8C0,sizeof(char),33))),_tag_dyneither(_tmp8BF,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp42A);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp42E;struct Cyc_Absyn_Exp*_tmp42F;struct _tuple20*_tmp42D=(struct _tuple20*)fields->hd;_tmp42E=(*_tmp42D).f1;_tmp42F=(*_tmp42D).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp42E->type,_tmp42F);
if(!Cyc_Tcutil_coerce_arg(te,_tmp42F,_tmp42E->type,& bogus)){
{const char*_tmp8C6;void*_tmp8C5[3];struct Cyc_String_pa_PrintArg_struct _tmp8C4;struct Cyc_String_pa_PrintArg_struct _tmp8C3;struct Cyc_String_pa_PrintArg_struct _tmp8C2;(_tmp8C2.tag=0,((_tmp8C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1932
Cyc_Absynpp_typ2string((void*)_check_null(_tmp42F->topt))),((_tmp8C3.tag=0,((_tmp8C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1931
Cyc_Absynpp_typ2string(_tmp42E->type)),((_tmp8C4.tag=0,((_tmp8C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp42E->name),((_tmp8C5[0]=& _tmp8C4,((_tmp8C5[1]=& _tmp8C3,((_tmp8C5[2]=& _tmp8C2,Cyc_Tcutil_terr(_tmp42F->loc,((_tmp8C6="field %s of struct expects type %s != %s",_tag_dyneither(_tmp8C6,sizeof(char),41))),_tag_dyneither(_tmp8C5,sizeof(void*),3)))))))))))))))))));}
# 1933
Cyc_Tcutil_explain_failure();}}
# 1936
goto _LL1E1;};_LL1E4:;_LL1E5: {
const char*_tmp8C9;void*_tmp8C8;(_tmp8C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8C9="tcAnonStruct: wrong type",_tag_dyneither(_tmp8C9,sizeof(char),25))),_tag_dyneither(_tmp8C8,sizeof(void*),0)));}_LL1E1:;}{
# 1940
void*_tmp437=ts;_npop_handler(0);return _tmp437;};
# 1918
;_pop_region(rgn);}
# 1944
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1947
struct _RegionHandle _tmp438=_new_region("rgn");struct _RegionHandle*rgn=& _tmp438;_push_region(rgn);
{struct _tuple12 _tmp8CA;struct _tuple12 _tmp439=(_tmp8CA.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp8CA.f2=rgn,_tmp8CA)));
struct Cyc_List_List*_tmp43A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp439,tud->tvs);
struct Cyc_List_List*_tmp43B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp43A);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp8D0;struct Cyc_Absyn_DatatypeFieldInfo _tmp8CF;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8CE;void*res=(void*)(
(_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8D0.tag=4,((_tmp8D0.f1=((_tmp8CF.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp8CF.targs=_tmp43B,_tmp8CF)))),_tmp8D0)))),_tmp8CE))));
# 1955
if(topt != 0){
void*_tmp43C=Cyc_Tcutil_compress(*topt);_LL1E7: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp43D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp43C;if(_tmp43D->tag != 4)goto _LL1E9;}_LL1E8:
 Cyc_Tcutil_unify(*topt,res);goto _LL1E6;_LL1E9:;_LL1EA:
 goto _LL1E6;_LL1E6:;}{
# 1961
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;
if(_tmp43A != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp43A,t);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp8D8;void*_tmp8D7[3];struct Cyc_String_pa_PrintArg_struct _tmp8D6;struct Cyc_String_pa_PrintArg_struct _tmp8D5;const char*_tmp8D4;struct Cyc_String_pa_PrintArg_struct _tmp8D3;(_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 1972
e->topt == 0?(struct _dyneither_ptr)((_tmp8D4="?",_tag_dyneither(_tmp8D4,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp8D5.tag=0,((_tmp8D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1971
Cyc_Absynpp_typ2string(t)),((_tmp8D6.tag=0,((_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8D7[0]=& _tmp8D6,((_tmp8D7[1]=& _tmp8D5,((_tmp8D7[2]=& _tmp8D3,Cyc_Tcutil_terr(e->loc,((_tmp8D8="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp8D8,sizeof(char),82))),_tag_dyneither(_tmp8D7,sizeof(void*),3)))))))))))))))))));}
# 1973
Cyc_Tcutil_explain_failure();}}
# 1976
if(es != 0){
const char*_tmp8DC;void*_tmp8DB[1];struct Cyc_String_pa_PrintArg_struct _tmp8DA;void*_tmp447=(_tmp8DA.tag=0,((_tmp8DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1979
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8DB[0]=& _tmp8DA,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp8DC="too many arguments for datatype constructor %s",_tag_dyneither(_tmp8DC,sizeof(char),47))),_tag_dyneither(_tmp8DB,sizeof(void*),1)))))));_npop_handler(0);return _tmp447;}
if(ts != 0){
const char*_tmp8E0;void*_tmp8DF[1];struct Cyc_String_pa_PrintArg_struct _tmp8DE;void*_tmp44B=(_tmp8DE.tag=0,((_tmp8DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp8DF[0]=& _tmp8DE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E0="too few arguments for datatype constructor %s",_tag_dyneither(_tmp8E0,sizeof(char),46))),_tag_dyneither(_tmp8DF,sizeof(void*),1)))))));_npop_handler(0);return _tmp44B;}{
void*_tmp44C=res;_npop_handler(0);return _tmp44C;};};}
# 1948
;_pop_region(rgn);}
# 1987
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp451=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp458;void*_tmp45C;struct Cyc_List_List*_tmp45F;union Cyc_Absyn_AggrInfoU _tmp461;struct Cyc_List_List*_tmp462;struct Cyc_List_List*_tmp466;_LL1EC: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp451;if(_tmp452->tag != 0)goto _LL1EE;}_LL1ED:
 return 1;_LL1EE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp451;if(_tmp453->tag != 1)goto _LL1F0;}_LL1EF:
 goto _LL1F1;_LL1F0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp451;if(_tmp454->tag != 2)goto _LL1F2;}_LL1F1:
 goto _LL1F3;_LL1F2: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp451;if(_tmp455->tag != 3)goto _LL1F4;}_LL1F3:
 goto _LL1F5;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp451;if(_tmp456->tag != 4)goto _LL1F6;}_LL1F5:
 return 0;_LL1F6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451;if(_tmp457->tag != 5)goto _LL1F8;else{_tmp458=((_tmp457->f1).ptr_atts).nullable;}}_LL1F7:
# 1995
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp458);_LL1F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp451;if(_tmp459->tag != 6)goto _LL1FA;}_LL1F9:
 goto _LL1FB;_LL1FA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp451;if(_tmp45A->tag != 7)goto _LL1FC;}_LL1FB:
 return 1;_LL1FC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp451;if(_tmp45B->tag != 8)goto _LL1FE;else{_tmp45C=(_tmp45B->f1).elt_type;}}_LL1FD:
 return Cyc_Tcexp_zeroable_type(_tmp45C);_LL1FE: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp451;if(_tmp45D->tag != 9)goto _LL200;}_LL1FF:
 return 0;_LL200: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp451;if(_tmp45E->tag != 10)goto _LL202;else{_tmp45F=_tmp45E->f1;}}_LL201:
# 2001
 for(0;(unsigned int)_tmp45F;_tmp45F=_tmp45F->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp45F->hd)).f2))return 0;}
return 1;_LL202: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp451;if(_tmp460->tag != 11)goto _LL204;else{_tmp461=(_tmp460->f1).aggr_info;_tmp462=(_tmp460->f1).targs;}}_LL203: {
# 2005
struct Cyc_Absyn_Aggrdecl*_tmp473=Cyc_Absyn_get_known_aggrdecl(_tmp461);
if(_tmp473->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp473->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp474=_new_region("r");struct _RegionHandle*r=& _tmp474;_push_region(r);
{struct Cyc_List_List*_tmp475=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp473->tvs,_tmp462);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp473->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp475,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp476=0;_npop_handler(0);return _tmp476;}}}{
int _tmp477=1;_npop_handler(0);return _tmp477;};}
# 2009
;_pop_region(r);};}_LL204: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp451;if(_tmp463->tag != 13)goto _LL206;}_LL205:
# 2014
 return 1;_LL206: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp451;if(_tmp464->tag != 19)goto _LL208;}_LL207:
 return 1;_LL208: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp451;if(_tmp465->tag != 12)goto _LL20A;else{_tmp466=_tmp465->f2;}}_LL209:
# 2017
 for(0;_tmp466 != 0;_tmp466=_tmp466->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp466->hd)->type))return 0;}
return 1;_LL20A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp451;if(_tmp467->tag != 14)goto _LL20C;}_LL20B:
 return 1;_LL20C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp451;if(_tmp468->tag != 17)goto _LL20E;}_LL20D:
 return 0;_LL20E: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp451;if(_tmp469->tag != 16)goto _LL210;}_LL20F:
 return 0;_LL210: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp451;if(_tmp46A->tag != 15)goto _LL212;}_LL211:
 return 0;_LL212: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp451;if(_tmp46B->tag != 26)goto _LL214;}_LL213:
# 2025
 goto _LL215;_LL214: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp451;if(_tmp46C->tag != 18)goto _LL216;}_LL215:
 goto _LL217;_LL216: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp451;if(_tmp46D->tag != 20)goto _LL218;}_LL217:
 goto _LL219;_LL218: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp451;if(_tmp46E->tag != 21)goto _LL21A;}_LL219:
 goto _LL21B;_LL21A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp451;if(_tmp46F->tag != 22)goto _LL21C;}_LL21B:
 goto _LL21D;_LL21C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp451;if(_tmp470->tag != 23)goto _LL21E;}_LL21D:
 goto _LL21F;_LL21E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp451;if(_tmp471->tag != 24)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp451;if(_tmp472->tag != 25)goto _LL1EB;}_LL221: {
const char*_tmp8E4;void*_tmp8E3[1];struct Cyc_String_pa_PrintArg_struct _tmp8E2;(_tmp8E2.tag=0,((_tmp8E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8E3[0]=& _tmp8E2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8E4="bad type `%s' in zeroable type",_tag_dyneither(_tmp8E4,sizeof(char),31))),_tag_dyneither(_tmp8E3,sizeof(void*),1)))))));}_LL1EB:;}
# 2036
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2038
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2040
if(topt != 0){
void*_tmp47B=Cyc_Tcutil_compress(*topt);void*_tmp47D;_LL223: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47B;if(_tmp47C->tag != 5)goto _LL225;else{_tmp47D=(_tmp47C->f1).elt_typ;}}_LL224:
# 2043
 Cyc_Tcutil_unify(_tmp47D,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL222;_LL225:;_LL226:
 goto _LL222;_LL222:;}{
# 2049
const char*_tmp8ED;void*_tmp8EC[2];const char*_tmp8EB;const char*_tmp8EA;struct Cyc_String_pa_PrintArg_struct _tmp8E9;struct Cyc_String_pa_PrintArg_struct _tmp8E8;(_tmp8E8.tag=0,((_tmp8E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp8E9.tag=0,((_tmp8E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp8EA="calloc",_tag_dyneither(_tmp8EA,sizeof(char),7))):(struct _dyneither_ptr)((_tmp8EB="malloc",_tag_dyneither(_tmp8EB,sizeof(char),7))))),((_tmp8EC[0]=& _tmp8E9,((_tmp8EC[1]=& _tmp8E8,Cyc_Tcutil_terr(loc,((_tmp8ED="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp8ED,sizeof(char),60))),_tag_dyneither(_tmp8EC,sizeof(void*),2)))))))))))));};}
# 2053
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2055
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL227:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL228:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2061
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2066
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2069
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp8F0;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8EF;void*expected_type=(void*)(
(_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF[0]=((_tmp8F0.tag=15,((_tmp8F0.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp8F0)))),_tmp8EF))));
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp484=Cyc_Tcutil_compress(handle_type);void*_tmp486;_LL22B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp484;if(_tmp485->tag != 15)goto _LL22D;else{_tmp486=(void*)_tmp485->f1;}}_LL22C:
# 2074
 rgn=_tmp486;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22A;_LL22D:;_LL22E:
# 2078
{const char*_tmp8F4;void*_tmp8F3[1];struct Cyc_String_pa_PrintArg_struct _tmp8F2;(_tmp8F2.tag=0,((_tmp8F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp8F3[0]=& _tmp8F2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp8F4="expecting region_t type but found %s",_tag_dyneither(_tmp8F4,sizeof(char),37))),_tag_dyneither(_tmp8F3,sizeof(void*),1)))))));}
goto _LL22A;_LL22A:;}else{
# 2085
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
# 2093
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{
# 2102 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8F7="calloc with empty type",_tag_dyneither(_tmp8F7,sizeof(char),23))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2114
void*er=(*e)->r;
retry_sizeof: {
void*_tmp48E=er;void*_tmp490;struct Cyc_Absyn_Exp*_tmp492;struct Cyc_Absyn_Exp*_tmp493;_LL230: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp48F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp48F->tag != 16)goto _LL232;else{_tmp490=(void*)_tmp48F->f1;}}_LL231:
# 2118
 elt_type=_tmp490;
{void**_tmp8F8;*t=(void**)((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8[0]=elt_type,_tmp8F8))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL22F;_LL232: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp491=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp48E;if(_tmp491->tag != 2)goto _LL234;else{if(_tmp491->f1 != Cyc_Absyn_Times)goto _LL234;if(_tmp491->f2 == 0)goto _LL234;_tmp492=(struct Cyc_Absyn_Exp*)(*_tmp491->f2).hd;if((*_tmp491->f2).tl == 0)goto _LL234;_tmp493=(struct Cyc_Absyn_Exp*)(*(*_tmp491->f2).tl).hd;if((*(*_tmp491->f2).tl).tl != 0)goto _LL234;}}_LL233:
# 2125
{struct _tuple0 _tmp8F9;struct _tuple0 _tmp496=(_tmp8F9.f1=_tmp492->r,((_tmp8F9.f2=_tmp493->r,_tmp8F9)));void*_tmp498;void*_tmp49A;_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp496.f1;if(_tmp497->tag != 16)goto _LL239;else{_tmp498=(void*)_tmp497->f1;}}_LL238:
# 2127
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp498);
elt_type=_tmp498;
{void**_tmp8FA;*t=(void**)((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=elt_type,_tmp8FA))));}
num_elts=_tmp493;
one_elt=0;
goto _LL236;_LL239: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp499=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp496.f2;if(_tmp499->tag != 16)goto _LL23B;else{_tmp49A=(void*)_tmp499->f1;}}_LL23A:
# 2134
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp49A);
elt_type=_tmp49A;
{void**_tmp8FB;*t=(void**)((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=elt_type,_tmp8FB))));}
num_elts=_tmp492;
one_elt=0;
goto _LL236;_LL23B:;_LL23C:
 goto No_sizeof;_LL236:;}
# 2142
goto _LL22F;_LL234:;_LL235:
# 2144
 No_sizeof: {
# 2147
struct Cyc_Absyn_Exp*_tmp49D=*e;
{void*_tmp49E=_tmp49D->r;struct Cyc_Absyn_Exp*_tmp4A0;_LL23E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp49F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49E;if(_tmp49F->tag != 13)goto _LL240;else{_tmp4A0=_tmp49F->f2;}}_LL23F:
 _tmp49D=_tmp4A0;goto _LL23D;_LL240:;_LL241:
 goto _LL23D;_LL23D:;}
# 2152
{void*_tmp4A1=Cyc_Tcutil_compress((void*)_check_null(_tmp49D->topt));void*_tmp4A3;_LL243: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4A2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A2->tag != 19)goto _LL245;else{_tmp4A3=(void*)_tmp4A2->f1;}}_LL244:
# 2154
{void*_tmp4A4=Cyc_Tcutil_compress(_tmp4A3);struct Cyc_Absyn_Exp*_tmp4A6;_LL248: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4A5=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4A4;if(_tmp4A5->tag != 18)goto _LL24A;else{_tmp4A6=_tmp4A5->f1;}}_LL249:
# 2156
 er=_tmp4A6->r;goto retry_sizeof;_LL24A:;_LL24B:
 goto _LL247;_LL247:;}
# 2159
goto _LL242;_LL245:;_LL246:
 goto _LL242;_LL242:;}
# 2162
elt_type=Cyc_Absyn_char_typ;
{void**_tmp8FC;*t=(void**)((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC[0]=elt_type,_tmp8FC))));}
num_elts=*e;
one_elt=0;}
# 2167
goto _LL22F;_LL22F:;}}
# 2171
*is_fat=!one_elt;
# 2174
{void*_tmp4A8=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4AA;_LL24D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A8;if(_tmp4A9->tag != 11)goto _LL24F;else{if((((_tmp4A9->f1).aggr_info).KnownAggr).tag != 2)goto _LL24F;_tmp4AA=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4A9->f1).aggr_info).KnownAggr).val);}}_LL24E:
# 2176
 if(_tmp4AA->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4AA->impl))->exist_vars != 0){
const char*_tmp8FF;void*_tmp8FE;(_tmp8FE=0,Cyc_Tcutil_terr(loc,((_tmp8FF="malloc with existential types not yet implemented",_tag_dyneither(_tmp8FF,sizeof(char),50))),_tag_dyneither(_tmp8FE,sizeof(void*),0)));}
goto _LL24C;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}{
# 2183
void*(*_tmp4AD)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4AE=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp4AF=Cyc_Tcutil_compress(*topt);union Cyc_Absyn_Constraint*_tmp4B1;union Cyc_Absyn_Constraint*_tmp4B2;union Cyc_Absyn_Constraint*_tmp4B3;_LL252: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AF;if(_tmp4B0->tag != 5)goto _LL254;else{_tmp4B1=((_tmp4B0->f1).ptr_atts).nullable;_tmp4B2=((_tmp4B0->f1).ptr_atts).bounds;_tmp4B3=((_tmp4B0->f1).ptr_atts).zero_term;}}_LL253:
# 2188
 _tmp4AE=_tmp4B3;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B1))
_tmp4AD=Cyc_Absyn_star_typ;
# 2193
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B3) && !(*is_calloc)){
{const char*_tmp902;void*_tmp901;(_tmp901=0,Cyc_Tcutil_warn(loc,((_tmp902="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp902,sizeof(char),55))),_tag_dyneither(_tmp901,sizeof(void*),0)));}
*is_calloc=1;}
# 2199
{void*_tmp4B6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4B2);struct Cyc_Absyn_Exp*_tmp4B9;_LL257: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4B7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4B6;if(_tmp4B7->tag != 0)goto _LL259;}_LL258:
 goto _LL256;_LL259:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4B8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4B6;if(_tmp4B8->tag != 1)goto _LL25B;else{_tmp4B9=_tmp4B8->f1;}}if(!(!one_elt))goto _LL25B;_LL25A: {
# 2202
int _tmp4BA=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4BA  && Cyc_Evexp_same_const_exp(_tmp4B9,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4B2),_tmp4AE);}
# 2208
{void*_tmp4BB=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4BD;_LL25E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4BC=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4BB;if(_tmp4BC->tag != 19)goto _LL260;else{_tmp4BD=(void*)_tmp4BC->f1;}}_LL25F: {
# 2210
struct Cyc_Absyn_Exp*_tmp4BE=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4BD,0),0,Cyc_Absyn_No_coercion,0);
# 2212
if(Cyc_Evexp_same_const_exp(_tmp4BE,_tmp4B9)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4B2),_tmp4AE);}
# 2217
goto _LL25D;}_LL260:;_LL261:
 goto _LL25D;_LL25D:;}
# 2220
goto _LL256;}_LL25B:;_LL25C:
 goto _LL256;_LL256:;}
# 2223
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}
# 2226
if(!one_elt)_tmp4AD=Cyc_Absyn_dyneither_typ;
return _tmp4AD(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4AE);};};}
# 2231
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2237
struct _RegionHandle _tmp4BF=_new_region("r");struct _RegionHandle*r=& _tmp4BF;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4C0=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp4C0,0,e1);{
void*_tmp4C1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp4C0,(void**)& _tmp4C1,e2);};}{
# 2243
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2246
{void*_tmp4C2=Cyc_Tcutil_compress(t1);_LL263: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C2;if(_tmp4C3->tag != 8)goto _LL265;}_LL264:
{const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(loc,((_tmp905="cannot assign to an array",_tag_dyneither(_tmp905,sizeof(char),26))),_tag_dyneither(_tmp904,sizeof(void*),0)));}goto _LL262;_LL265:;_LL266:
 goto _LL262;_LL262:;}{
# 2251
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp908;void*_tmp907;(_tmp907=0,Cyc_Tcutil_terr(loc,((_tmp908="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp908,sizeof(char),58))),_tag_dyneither(_tmp907,sizeof(void*),0)));}
# 2256
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp90B;void*_tmp90A;void*_tmp4CA=(_tmp90A=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp90B="swap non-lvalue",_tag_dyneither(_tmp90B,sizeof(char),16))),_tag_dyneither(_tmp90A,sizeof(void*),0)));_npop_handler(0);return _tmp4CA;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp90E;void*_tmp90D;void*_tmp4CD=(_tmp90D=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp90E="swap non-lvalue",_tag_dyneither(_tmp90E,sizeof(char),16))),_tag_dyneither(_tmp90D,sizeof(void*),0)));_npop_handler(0);return _tmp4CD;}{
# 2261
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp911;void*_tmp910;void*_tmp4D0=(_tmp910=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp911="swap value in zeroterm array",_tag_dyneither(_tmp911,sizeof(char),29))),_tag_dyneither(_tmp910,sizeof(void*),0)));_npop_handler(0);return _tmp4D0;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp914;void*_tmp913;void*_tmp4D3=(_tmp913=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp914="swap value in zeroterm array",_tag_dyneither(_tmp914,sizeof(char),29))),_tag_dyneither(_tmp913,sizeof(void*),0)));_npop_handler(0);return _tmp4D3;}
# 2268
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp919;void*_tmp918[2];struct Cyc_String_pa_PrintArg_struct _tmp917;struct Cyc_String_pa_PrintArg_struct _tmp916;void*_tmp4D4=(_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp917.tag=0,((_tmp917.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp918[0]=& _tmp917,((_tmp918[1]=& _tmp916,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp919="type mismatch: %s != %s",_tag_dyneither(_tmp919,sizeof(char),24))),_tag_dyneither(_tmp918,sizeof(void*),2)))))))))))));
void*_tmp4D5=_tmp4D4;_npop_handler(0);return _tmp4D5;}{
# 2275
void*_tmp4DA=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp4DA;};};};};
# 2237
;_pop_region(r);}
# 2279
int Cyc_Tcexp_in_stmt_exp=0;
# 2281
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp4DB=_new_region("r");struct _RegionHandle*r=& _tmp4DB;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2288
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2290
while(1){
void*_tmp4DC=s->r;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Stmt*_tmp4E0;struct Cyc_Absyn_Stmt*_tmp4E1;struct Cyc_Absyn_Decl*_tmp4E3;struct Cyc_Absyn_Stmt*_tmp4E4;_LL268: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp4DD=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp4DC;if(_tmp4DD->tag != 1)goto _LL26A;else{_tmp4DE=_tmp4DD->f1;}}_LL269: {
# 2294
void*_tmp4E5=(void*)_check_null(_tmp4DE->topt);
if(!Cyc_Tcutil_unify(_tmp4E5,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp91D;void*_tmp91C[1];struct Cyc_String_pa_PrintArg_struct _tmp91B;(_tmp91B.tag=0,((_tmp91B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp4E5)),((_tmp91C[0]=& _tmp91B,Cyc_Tcutil_terr(loc,((_tmp91D="statement expression returns type %s",_tag_dyneither(_tmp91D,sizeof(char),37))),_tag_dyneither(_tmp91C,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2300
void*_tmp4E9=_tmp4E5;_npop_handler(0);return _tmp4E9;};}_LL26A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp4DF=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp4DC;if(_tmp4DF->tag != 2)goto _LL26C;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f2;}}_LL26B:
 s=_tmp4E1;continue;_LL26C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp4E2=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp4DC;if(_tmp4E2->tag != 12)goto _LL26E;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}}_LL26D:
 s=_tmp4E4;continue;_LL26E:;_LL26F: {
# 2304
const char*_tmp920;void*_tmp91F;void*_tmp4EC=(_tmp91F=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp920="statement expression must end with expression",_tag_dyneither(_tmp920,sizeof(char),46))),_tag_dyneither(_tmp91F,sizeof(void*),0)));_npop_handler(0);return _tmp4EC;}_LL267:;}
# 2282
;_pop_region(r);}
# 2309
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
{void*_tmp4ED=t;struct Cyc_Absyn_Aggrdecl*_tmp4EF;_LL271: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4ED;if(_tmp4EE->tag != 11)goto _LL273;else{if((((_tmp4EE->f1).aggr_info).KnownAggr).tag != 2)goto _LL273;_tmp4EF=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4EE->f1).aggr_info).KnownAggr).val);}}_LL272:
# 2313
 if((_tmp4EF->kind == Cyc_Absyn_UnionA  && _tmp4EF->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EF->impl))->tagged)goto _LL270;
goto _LL274;_LL273:;_LL274:
# 2316
{const char*_tmp924;void*_tmp923[1];struct Cyc_String_pa_PrintArg_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp923[0]=& _tmp922,Cyc_Tcutil_terr(loc,((_tmp924="expecting @tagged union but found %s",_tag_dyneither(_tmp924,sizeof(char),37))),_tag_dyneither(_tmp923,sizeof(void*),1)))))));}
goto _LL270;_LL270:;}
# 2319
return Cyc_Absyn_uint_typ;}
# 2323
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2327
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2329
if(*rgn_handle != 0){
# 2331
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp927;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp926;void*expected_type=(void*)(
(_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=15,((_tmp927.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp927)))),_tmp926))));
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp4F3=Cyc_Tcutil_compress(handle_type);void*_tmp4F5;_LL276: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F3;if(_tmp4F4->tag != 15)goto _LL278;else{_tmp4F5=(void*)_tmp4F4->f1;}}_LL277:
# 2336
 rgn=_tmp4F5;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL275;_LL278:;_LL279:
# 2340
{const char*_tmp92B;void*_tmp92A[1];struct Cyc_String_pa_PrintArg_struct _tmp929;(_tmp929.tag=0,((_tmp929.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp92A[0]=& _tmp929,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp92B="expecting region_t type but found %s",_tag_dyneither(_tmp92B,sizeof(char),37))),_tag_dyneither(_tmp92A,sizeof(void*),1)))))));}
goto _LL275;_LL275:;}else{
# 2347
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
# 2355
void*_tmp4FB=e1->r;struct Cyc_Core_Opt*_tmp4FE;struct Cyc_List_List*_tmp4FF;struct Cyc_List_List*_tmp501;_LL27B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp4FC->tag != 26)goto _LL27D;}_LL27C: {
# 2358
void*_tmp504=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
void*_tmp505=Cyc_Tcutil_compress(_tmp504);void*_tmp507;struct Cyc_Absyn_Tqual _tmp508;struct Cyc_Absyn_Exp*_tmp509;union Cyc_Absyn_Constraint*_tmp50A;_LL288: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp505;if(_tmp506->tag != 8)goto _LL28A;else{_tmp507=(_tmp506->f1).elt_type;_tmp508=(_tmp506->f1).tq;_tmp509=(_tmp506->f1).num_elts;_tmp50A=(_tmp506->f1).zero_term;}}_LL289: {
# 2361
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp509);
void*b;
{void*_tmp50B=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp50D;_LL28D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp50B;if(_tmp50C->tag != 19)goto _LL28F;else{_tmp50D=(void*)_tmp50C->f1;}}_LL28E:
# 2365
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp92E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp92D;b=(void*)((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=1,((_tmp92E.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp50D,0),0,Cyc_Absyn_No_coercion,0),_tmp92E)))),_tmp92D))));}
goto _LL28C;_LL28F:;_LL290:
# 2368
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp931;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp930;b=(void*)((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930[0]=((_tmp931.tag=1,((_tmp931.f1=bnd,_tmp931)))),_tmp930))));}else{
# 2371
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL28C:;}{
# 2373
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp93B;struct Cyc_Absyn_PtrAtts _tmp93A;struct Cyc_Absyn_PtrInfo _tmp939;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp938;void*res_typ=(void*)(
(_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp93B.tag=5,((_tmp93B.f1=((_tmp939.elt_typ=_tmp507,((_tmp939.elt_tq=_tmp508,((_tmp939.ptr_atts=(
(_tmp93A.rgn=rgn,((_tmp93A.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp93A.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp93A.zero_term=_tmp50A,((_tmp93A.ptrloc=0,_tmp93A)))))))))),_tmp939)))))),_tmp93B)))),_tmp938))));
# 2377
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=(void*)res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2384
return res_typ;};}_LL28A:;_LL28B: {
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp93E="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp93E,sizeof(char),45))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}_LL287:;}_LL27D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp4FD->tag != 34)goto _LL27F;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FD->f2;}}_LL27E:
# 2388
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp941;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp940;e1->r=(void*)((_tmp940=_cycalloc(sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=25,((_tmp941.f1=_tmp4FF,_tmp941)))),_tmp940))));}
_tmp501=_tmp4FF;goto _LL280;_LL27F: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp500->tag != 25)goto _LL281;else{_tmp501=_tmp500->f1;}}_LL280: {
# 2391
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp51A=Cyc_Tcutil_compress(*topt);void**_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;union Cyc_Absyn_Constraint*_tmp51E;_LL292: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51A;if(_tmp51B->tag != 5)goto _LL294;else{_tmp51C=(void**)&(_tmp51B->f1).elt_typ;_tmp51D=(_tmp51B->f1).elt_tq;_tmp51E=((_tmp51B->f1).ptr_atts).zero_term;}}_LL293:
# 2397
 elt_typ_opt=(void**)_tmp51C;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51E);
goto _LL291;_LL294:;_LL295:
 goto _LL291;_LL291:;}{
# 2403
void*_tmp51F=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,_tmp501);
e1->topt=(void*)_tmp51F;{
void*res_typ;
{void*_tmp520=Cyc_Tcutil_compress(_tmp51F);void*_tmp522;struct Cyc_Absyn_Tqual _tmp523;struct Cyc_Absyn_Exp*_tmp524;union Cyc_Absyn_Constraint*_tmp525;_LL297: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp520;if(_tmp521->tag != 8)goto _LL299;else{_tmp522=(_tmp521->f1).elt_type;_tmp523=(_tmp521->f1).tq;_tmp524=(_tmp521->f1).num_elts;_tmp525=(_tmp521->f1).zero_term;}}_LL298:
# 2408
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp956;struct Cyc_Absyn_PtrAtts _tmp955;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp954;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp953;struct Cyc_Absyn_PtrInfo _tmp952;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp951;res_typ=(void*)(
(_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=((_tmp956.tag=5,((_tmp956.f1=((_tmp952.elt_typ=_tmp522,((_tmp952.elt_tq=_tmp523,((_tmp952.ptr_atts=(
(_tmp955.rgn=rgn,((_tmp955.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp955.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp953=_cycalloc(sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=1,((_tmp954.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp524),_tmp954)))),_tmp953))))),((_tmp955.zero_term=_tmp525,((_tmp955.ptrloc=0,_tmp955)))))))))),_tmp952)))))),_tmp956)))),_tmp951))));}
# 2413
if(topt != 0){
# 2417
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=(void*)res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2423
goto _LL296;_LL299:;_LL29A: {
const char*_tmp959;void*_tmp958;(_tmp958=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp959="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp959,sizeof(char),50))),_tag_dyneither(_tmp958,sizeof(void*),0)));}_LL296:;}
# 2426
return res_typ;};};}_LL281: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp502->tag != 0)goto _LL283;else{if(((_tmp502->f1).String_c).tag != 8)goto _LL283;}}_LL282: {
# 2431
void*_tmp52E=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2433
void*_tmp52F=Cyc_Tcexp_tcExp(te,(void**)& _tmp52E,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp95C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp95B;return Cyc_Absyn_atb_typ(_tmp52F,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp95B=_cycalloc(sizeof(*_tmp95B)),((_tmp95B[0]=((_tmp95C.tag=1,((_tmp95C.f1=Cyc_Absyn_uint_exp(1,0),_tmp95C)))),_tmp95B)))),Cyc_Absyn_false_conref);}_LL283: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp503=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4FB;if(_tmp503->tag != 0)goto _LL285;else{if(((_tmp503->f1).Wstring_c).tag != 9)goto _LL285;}}_LL284: {
# 2438
void*_tmp532=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2440
void*_tmp533=Cyc_Tcexp_tcExp(te,(void**)& _tmp532,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp95F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp95E;return Cyc_Absyn_atb_typ(_tmp533,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp95E=_cycalloc(sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=1,((_tmp95F.f1=Cyc_Absyn_uint_exp(1,0),_tmp95F)))),_tmp95E)))),Cyc_Absyn_false_conref);}_LL285:;_LL286:
# 2447
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp536=Cyc_Tcutil_compress(*topt);void**_tmp538;struct Cyc_Absyn_Tqual _tmp539;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp536;if(_tmp537->tag != 5)goto _LL29E;else{_tmp538=(void**)&(_tmp537->f1).elt_typ;_tmp539=(_tmp537->f1).elt_tq;}}_LL29D:
# 2453
 topt2=(void**)_tmp538;goto _LL29B;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp536;if(_tmp53A->tag != 3)goto _LL2A0;}_LL29F:
# 2457
 bogus=*topt;
topt2=(void**)& bogus;
goto _LL29B;_LL2A0:;_LL2A1:
 goto _LL29B;_LL29B:;}{
# 2463
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp969;struct Cyc_Absyn_PtrAtts _tmp968;struct Cyc_Absyn_PtrInfo _tmp967;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp966;void*res_typ=(void*)(
(_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp969.tag=5,((_tmp969.f1=((_tmp967.elt_typ=telt,((_tmp967.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp967.ptr_atts=(
(_tmp968.rgn=rgn,((_tmp968.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp968.bounds=Cyc_Absyn_bounds_one_conref,((_tmp968.zero_term=Cyc_Absyn_false_conref,((_tmp968.ptrloc=0,_tmp968)))))))))),_tmp967)))))),_tmp969)))),_tmp966))));
# 2469
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
e->topt=(void*)res_typ;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2476
return res_typ;};}_LL27A:;};}
# 2482
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp53F=t;void*_tmp541;struct Cyc_Absyn_Tqual _tmp542;struct Cyc_Absyn_Exp*_tmp543;union Cyc_Absyn_Constraint*_tmp544;_LL2A3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag != 8)goto _LL2A5;else{_tmp541=(_tmp540->f1).elt_type;_tmp542=(_tmp540->f1).tq;_tmp543=(_tmp540->f1).num_elts;_tmp544=(_tmp540->f1).zero_term;}}_LL2A4: {
# 2486
void*_tmp546;struct _tuple14 _tmp545=Cyc_Tcutil_addressof_props(te,e);_tmp546=_tmp545.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp96C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp96B;void*_tmp547=_tmp543 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp96B=_cycalloc(sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96C.tag=1,((_tmp96C.f1=(struct Cyc_Absyn_Exp*)_tmp543,_tmp96C)))),_tmp96B))));
t=Cyc_Absyn_atb_typ(_tmp541,_tmp546,_tmp542,_tmp547,_tmp544);
e->topt=(void*)t;
return t;};}_LL2A5:;_LL2A6:
# 2492
 return t;_LL2A2:;}
# 2499
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2503
struct _RegionHandle _tmp54A=_new_region("temp");struct _RegionHandle*temp=& _tmp54A;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2509
const char*_tmp96F;void*_tmp96E;(_tmp96E=0,Cyc_Tcutil_terr(e->loc,((_tmp96F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp96F,sizeof(char),49))),_tag_dyneither(_tmp96E,sizeof(void*),0)));}{
# 2512
void*_tmp54D=e->r;_LL2A8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp54E->tag != 25)goto _LL2AA;}_LL2A9:
 goto _LL2AB;_LL2AA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp54F->tag != 26)goto _LL2AC;}_LL2AB:
 goto _LL2AD;_LL2AC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp550->tag != 0)goto _LL2AE;else{if(((_tmp550->f1).Wstring_c).tag != 9)goto _LL2AE;}}_LL2AD:
 goto _LL2AF;_LL2AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp551->tag != 0)goto _LL2B0;else{if(((_tmp551->f1).String_c).tag != 8)goto _LL2B0;}}_LL2AF: {
void*_tmp552=t;_npop_handler(0);return _tmp552;}_LL2B0:;_LL2B1:
# 2518
 t=Cyc_Tcutil_compress(t);{
void*_tmp553=t;void*_tmp555;struct Cyc_Absyn_Tqual _tmp556;struct Cyc_Absyn_Exp*_tmp557;union Cyc_Absyn_Constraint*_tmp558;_LL2B3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp553;if(_tmp554->tag != 8)goto _LL2B5;else{_tmp555=(_tmp554->f1).elt_type;_tmp556=(_tmp554->f1).tq;_tmp557=(_tmp554->f1).num_elts;_tmp558=(_tmp554->f1).zero_term;}}_LL2B4: {
# 2521
void*_tmp55A;struct _tuple14 _tmp559=Cyc_Tcutil_addressof_props(te,e);_tmp55A=_tmp559.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp972;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp971;void*b=_tmp557 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((_tmp972.tag=1,((_tmp972.f1=(struct Cyc_Absyn_Exp*)_tmp557,_tmp972)))),_tmp971))));
t=Cyc_Absyn_atb_typ(_tmp555,_tmp55A,_tmp556,b,_tmp558);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp55B=t;_npop_handler(0);return _tmp55B;};};}_LL2B5:;_LL2B6: {
# 2528
void*_tmp55E=t;_npop_handler(0);return _tmp55E;}_LL2B2:;};_LL2A7:;};
# 2503
;_pop_region(temp);}
# 2540 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp55F=e->r;struct Cyc_Absyn_Exp*_tmp561;_LL2B8: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp55F;if(_tmp560->tag != 11)goto _LL2BA;else{_tmp561=_tmp560->f1;}}_LL2B9:
# 2544
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp561);
_tmp561->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(_tmp561->topt),0);
e->topt=_tmp561->topt;
goto _LL2B7;_LL2BA:;_LL2BB:
# 2550
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2553
{void*_tmp562=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp564;struct Cyc_Absyn_Tqual _tmp565;void*_tmp566;union Cyc_Absyn_Constraint*_tmp567;union Cyc_Absyn_Constraint*_tmp568;union Cyc_Absyn_Constraint*_tmp569;_LL2BD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp562;if(_tmp563->tag != 5)goto _LL2BF;else{_tmp564=(_tmp563->f1).elt_typ;_tmp565=(_tmp563->f1).elt_tq;_tmp566=((_tmp563->f1).ptr_atts).rgn;_tmp567=((_tmp563->f1).ptr_atts).nullable;_tmp568=((_tmp563->f1).ptr_atts).bounds;_tmp569=((_tmp563->f1).ptr_atts).zero_term;}}_LL2BE:
# 2555
{void*_tmp56A=Cyc_Tcutil_compress(_tmp564);struct Cyc_List_List*_tmp56C;void*_tmp56D;struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;struct Cyc_List_List*_tmp570;int _tmp571;struct Cyc_Absyn_VarargInfo*_tmp572;struct Cyc_List_List*_tmp573;struct Cyc_List_List*_tmp574;_LL2C2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp56A;if(_tmp56B->tag != 9)goto _LL2C4;else{_tmp56C=(_tmp56B->f1).tvars;_tmp56D=(_tmp56B->f1).effect;_tmp56E=(_tmp56B->f1).ret_tqual;_tmp56F=(_tmp56B->f1).ret_typ;_tmp570=(_tmp56B->f1).args;_tmp571=(_tmp56B->f1).c_varargs;_tmp572=(_tmp56B->f1).cyc_varargs;_tmp573=(_tmp56B->f1).rgn_po;_tmp574=(_tmp56B->f1).attributes;}}_LL2C3:
# 2557
 if(_tmp56C != 0){
struct _RegionHandle _tmp575=_new_region("rgn");struct _RegionHandle*rgn=& _tmp575;_push_region(rgn);
{struct _tuple12 _tmp973;struct _tuple12 _tmp576=(_tmp973.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp973.f2=rgn,_tmp973)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp576,_tmp56C);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp979;struct Cyc_Absyn_FnInfo _tmp978;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp977;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp979.tag=9,((_tmp979.f1=((_tmp978.tvars=0,((_tmp978.effect=_tmp56D,((_tmp978.ret_tqual=_tmp56E,((_tmp978.ret_typ=_tmp56F,((_tmp978.args=_tmp570,((_tmp978.c_varargs=_tmp571,((_tmp978.cyc_varargs=_tmp572,((_tmp978.rgn_po=_tmp573,((_tmp978.attributes=_tmp574,_tmp978)))))))))))))))))),_tmp979)))),_tmp977)))));
# 2567
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp983;struct Cyc_Absyn_PtrAtts _tmp982;struct Cyc_Absyn_PtrInfo _tmp981;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp980;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp577=(_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=((_tmp983.tag=5,((_tmp983.f1=((_tmp981.elt_typ=ftyp,((_tmp981.elt_tq=_tmp565,((_tmp981.ptr_atts=((_tmp982.rgn=_tmp566,((_tmp982.nullable=_tmp567,((_tmp982.bounds=_tmp568,((_tmp982.zero_term=_tmp569,((_tmp982.ptrloc=0,_tmp982)))))))))),_tmp981)))))),_tmp983)))),_tmp980)));
# 2569
struct Cyc_Absyn_Exp*_tmp578=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp986;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp985;e->r=(void*)((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985[0]=((_tmp986.tag=12,((_tmp986.f1=_tmp578,((_tmp986.f2=ts,_tmp986)))))),_tmp985))));}
e->topt=(void*)_tmp577;}
# 2559
;_pop_region(rgn);}
# 2574
goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}
# 2577
goto _LL2BC;_LL2BF:;_LL2C0:
 goto _LL2BC;_LL2BC:;}
# 2580
goto _LL2B7;_LL2B7:;}
# 2582
return(void*)_check_null(e->topt);}
# 2590
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2592
struct _RegionHandle _tmp583=_new_region("r");struct _RegionHandle*r=& _tmp583;_push_region(r);
{struct Cyc_List_List*_tmp584=0;
# 2595
{void*_tmp585=fn_exp->r;struct Cyc_List_List*_tmp587;_LL2C7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp586=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp585;if(_tmp586->tag != 9)goto _LL2C9;else{_tmp587=_tmp586->f2;}}_LL2C8:
# 2597
{void*_tmp588=e->r;struct Cyc_List_List*_tmp58A;_LL2CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp589=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp588;if(_tmp589->tag != 9)goto _LL2CE;else{_tmp58A=_tmp589->f2;}}_LL2CD: {
# 2599
struct Cyc_List_List*_tmp58B=alias_arg_exps;
int _tmp58C=0;
while(_tmp58B != 0){
while(_tmp58C != (int)_tmp58B->hd){
if(_tmp587 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp990;struct Cyc_Int_pa_PrintArg_struct _tmp98F;void*_tmp98E[2];const char*_tmp98D;void*_tmp98C;(_tmp98C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp990.tag=1,((_tmp990.f1=(unsigned long)((int)_tmp58B->hd),((_tmp98F.tag=1,((_tmp98F.f1=(unsigned long)_tmp58C,((_tmp98E[0]=& _tmp98F,((_tmp98E[1]=& _tmp990,Cyc_aprintf(((_tmp98D="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp98D,sizeof(char),36))),_tag_dyneither(_tmp98E,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
# 2606
++ _tmp58C;
_tmp587=_tmp587->tl;
_tmp58A=((struct Cyc_List_List*)_check_null(_tmp58A))->tl;}{
# 2611
struct Cyc_Absyn_Decl*_tmp593;struct Cyc_Absyn_Exp*_tmp594;struct _tuple11 _tmp592=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp587))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp58A))->hd)->topt)));_tmp593=_tmp592.f1;_tmp594=_tmp592.f2;
_tmp587->hd=(void*)_tmp594;
{struct Cyc_List_List*_tmp991;_tmp584=((_tmp991=_region_malloc(r,sizeof(*_tmp991)),((_tmp991->hd=_tmp593,((_tmp991->tl=_tmp584,_tmp991))))));}
_tmp58B=_tmp58B->tl;};}
# 2616
goto _LL2CB;}_LL2CE:;_LL2CF: {
const char*_tmp994;void*_tmp993;(_tmp993=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp994="not a function call!",_tag_dyneither(_tmp994,sizeof(char),21))),_tag_dyneither(_tmp993,sizeof(void*),0)));}_LL2CB:;}
# 2619
goto _LL2C6;_LL2C9:;_LL2CA: {
const char*_tmp997;void*_tmp996;(_tmp996=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp997="not a function call!",_tag_dyneither(_tmp997,sizeof(char),21))),_tag_dyneither(_tmp996,sizeof(void*),0)));}_LL2C6:;}
# 2624
while(_tmp584 != 0){
struct Cyc_Absyn_Decl*_tmp59A=(struct Cyc_Absyn_Decl*)_tmp584->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp59A,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp584=_tmp584->tl;}
# 2631
e->topt=0;
e->r=fn_exp->r;}
# 2593
;_pop_region(r);}
# 2637
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2641
{void*_tmp59B=e->r;struct Cyc_Absyn_Exp*_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_Absyn_VarargCallInfo**_tmp5A1;int*_tmp5A2;struct Cyc_Core_Opt*_tmp5A4;struct Cyc_List_List*_tmp5A5;union Cyc_Absyn_Cnst*_tmp5A7;struct _tuple1*_tmp5A9;void**_tmp5AA;enum Cyc_Absyn_Primop _tmp5AC;struct Cyc_List_List*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AF;enum Cyc_Absyn_Incrementor _tmp5B0;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Core_Opt*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5C1;struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_VarargCallInfo**_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_List_List*_tmp5CA;void*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;enum Cyc_Absyn_Coercion*_tmp5CE;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp**_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D5;void*_tmp5D7;void*_tmp5D9;void*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DE;struct _dyneither_ptr*_tmp5DF;int*_tmp5E0;int*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E3;struct _dyneither_ptr*_tmp5E4;int*_tmp5E5;int*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_List_List*_tmp5EB;struct _tuple9*_tmp5ED;struct Cyc_List_List*_tmp5EE;struct Cyc_List_List*_tmp5F0;struct Cyc_Absyn_Stmt*_tmp5F2;struct Cyc_Absyn_Vardecl*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;int*_tmp5F7;struct _tuple1**_tmp5F9;struct Cyc_List_List**_tmp5FA;struct Cyc_List_List*_tmp5FB;struct Cyc_Absyn_Aggrdecl**_tmp5FC;void*_tmp5FE;struct Cyc_List_List*_tmp5FF;struct Cyc_List_List*_tmp601;struct Cyc_Absyn_Datatypedecl*_tmp602;struct Cyc_Absyn_Datatypefield*_tmp603;struct _tuple1**_tmp605;struct Cyc_Absyn_Enumdecl*_tmp606;struct Cyc_Absyn_Enumfield*_tmp607;struct _tuple1**_tmp609;void*_tmp60A;struct Cyc_Absyn_Enumfield*_tmp60B;int*_tmp60D;struct Cyc_Absyn_Exp**_tmp60E;void***_tmp60F;struct Cyc_Absyn_Exp**_tmp610;int*_tmp611;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Exp*_tmp614;struct Cyc_Absyn_Exp*_tmp616;struct _dyneither_ptr*_tmp617;void*_tmp619;_LL2D1: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp59C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp59C->tag != 11)goto _LL2D3;else{_tmp59D=_tmp59C->f1;}}_LL2D2:
# 2646
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp59D);
return;_LL2D3:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp59E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp59E->tag != 9)goto _LL2D5;else{_tmp59F=_tmp59E->f1;_tmp5A0=_tmp59E->f2;_tmp5A1=(struct Cyc_Absyn_VarargCallInfo**)& _tmp59E->f3;_tmp5A2=(int*)& _tmp59E->f4;}}if(!(!(*_tmp5A2)))goto _LL2D5;_LL2D4:
# 2652
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp59F,_tmp5A0)){
*_tmp5A2=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2656
if(topt != 0)e->topt=(void*)*topt;else{
e->topt=(void*)Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2659
return;_LL2D5: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5A3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5A3->tag != 34)goto _LL2D7;else{_tmp5A4=_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;}}_LL2D6:
# 2663
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5A5);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2D7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5A6->tag != 0)goto _LL2D9;else{_tmp5A7=(union Cyc_Absyn_Cnst*)& _tmp5A6->f1;}}_LL2D8:
# 2668
 t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)_tmp5A7,e);goto _LL2D0;_LL2D9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5A8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5A8->tag != 1)goto _LL2DB;else{_tmp5A9=_tmp5A8->f1;_tmp5AA=(void**)((void**)& _tmp5A8->f2);}}_LL2DA:
# 2670
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5A9,(void**)_tmp5AA);goto _LL2D0;_LL2DB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5AB=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5AB->tag != 2)goto _LL2DD;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=_tmp5AB->f2;}}_LL2DC:
# 2672
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5AC,_tmp5AD);goto _LL2D0;_LL2DD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5AE=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5AE->tag != 4)goto _LL2DF;else{_tmp5AF=_tmp5AE->f1;_tmp5B0=_tmp5AE->f2;}}_LL2DE:
# 2674
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5AF,_tmp5B0);goto _LL2D0;_LL2DF: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5B1=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5B1->tag != 3)goto _LL2E1;else{_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;_tmp5B4=_tmp5B1->f3;}}_LL2E0:
# 2676
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5B2,_tmp5B3,_tmp5B4);goto _LL2D0;_LL2E1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5B5->tag != 5)goto _LL2E3;else{_tmp5B6=_tmp5B5->f1;_tmp5B7=_tmp5B5->f2;_tmp5B8=_tmp5B5->f3;}}_LL2E2:
# 2678
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5B6,_tmp5B7,_tmp5B8);goto _LL2D0;_LL2E3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5B9=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5B9->tag != 6)goto _LL2E5;else{_tmp5BA=_tmp5B9->f1;_tmp5BB=_tmp5B9->f2;}}_LL2E4:
# 2680
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5BA,_tmp5BB);goto _LL2D0;_LL2E5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5BC=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5BC->tag != 7)goto _LL2E7;else{_tmp5BD=_tmp5BC->f1;_tmp5BE=_tmp5BC->f2;}}_LL2E6:
# 2682
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5BD,_tmp5BE);goto _LL2D0;_LL2E7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5BF->tag != 8)goto _LL2E9;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=_tmp5BF->f2;}}_LL2E8:
# 2684
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5C0,_tmp5C1);goto _LL2D0;_LL2E9: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5C2=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5C2->tag != 9)goto _LL2EB;else{_tmp5C3=_tmp5C2->f1;_tmp5C4=_tmp5C2->f2;_tmp5C5=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5C2->f3;}}_LL2EA: {
# 2690
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp61B;_push_handler(& _tmp61B);{int _tmp61D=0;if(setjmp(_tmp61B.handler))_tmp61D=1;if(!_tmp61D){
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp61C=(void*)_exn_thrown;void*_tmp61F=_tmp61C;struct _dyneither_ptr _tmp621;void*_tmp622;_LL322: {struct Cyc_Core_Failure_exn_struct*_tmp620=(struct Cyc_Core_Failure_exn_struct*)_tmp61F;if(_tmp620->tag != Cyc_Core_Failure)goto _LL324;else{_tmp621=_tmp620->f1;}}_LL323:
# 2697
 ok=0;
fn_exp=e;
goto _LL321;_LL324: _tmp622=_tmp61F;_LL325:(void)_throw(_tmp622);_LL321:;}};}
# 2701
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5C3,_tmp5C4,_tmp5C5,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2708
goto _LL2D0;}_LL2EB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5C6=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5C6->tag != 10)goto _LL2ED;else{_tmp5C7=_tmp5C6->f1;}}_LL2EC:
# 2710
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5C7);goto _LL2D0;_LL2ED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5C8->tag != 12)goto _LL2EF;else{_tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;}}_LL2EE:
# 2712
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5C9,_tmp5CA);goto _LL2D0;_LL2EF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5CB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5CB->tag != 13)goto _LL2F1;else{_tmp5CC=(void*)_tmp5CB->f1;_tmp5CD=_tmp5CB->f2;_tmp5CE=(enum Cyc_Absyn_Coercion*)& _tmp5CB->f4;}}_LL2F0:
# 2714
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5CC,_tmp5CD,(enum Cyc_Absyn_Coercion*)_tmp5CE);goto _LL2D0;_LL2F1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp5CF=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5CF->tag != 14)goto _LL2F3;else{_tmp5D0=_tmp5CF->f1;}}_LL2F2:
# 2716
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp5D0);goto _LL2D0;_LL2F3: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5D1=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5D1->tag != 15)goto _LL2F5;else{_tmp5D2=(struct Cyc_Absyn_Exp**)& _tmp5D1->f1;_tmp5D3=_tmp5D1->f2;}}_LL2F4:
# 2718
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5D2,e,_tmp5D3);goto _LL2D0;_LL2F5: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5D4=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5D4->tag != 17)goto _LL2F7;else{_tmp5D5=_tmp5D4->f1;}}_LL2F6: {
# 2720
void*_tmp623=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5D5);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp623);goto _LL2D0;}_LL2F7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp5D6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5D6->tag != 16)goto _LL2F9;else{_tmp5D7=(void*)_tmp5D6->f1;}}_LL2F8:
# 2723
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5D7);goto _LL2D0;_LL2F9: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp5D8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5D8->tag != 18)goto _LL2FB;else{_tmp5D9=(void*)_tmp5D8->f1;_tmp5DA=(void*)_tmp5D8->f2;}}_LL2FA:
# 2725
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5D9,_tmp5DA);goto _LL2D0;_LL2FB: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5DB->tag != 19)goto _LL2FD;else{_tmp5DC=_tmp5DB->f1;}}_LL2FC:
# 2727
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp5DC);goto _LL2D0;_LL2FD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp5DD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5DD->tag != 20)goto _LL2FF;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;_tmp5E0=(int*)& _tmp5DD->f3;_tmp5E1=(int*)& _tmp5DD->f4;}}_LL2FE:
# 2729
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp5DE,_tmp5DF,_tmp5E0,_tmp5E1);goto _LL2D0;_LL2FF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp5E2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5E2->tag != 21)goto _LL301;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;_tmp5E5=(int*)& _tmp5E2->f3;_tmp5E6=(int*)& _tmp5E2->f4;}}_LL300:
# 2731
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp5E3,_tmp5E4,_tmp5E5,_tmp5E6);goto _LL2D0;_LL301: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp5E7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5E7->tag != 22)goto _LL303;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;}}_LL302:
# 2733
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp5E8,_tmp5E9);goto _LL2D0;_LL303: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5EA->tag != 23)goto _LL305;else{_tmp5EB=_tmp5EA->f1;}}_LL304:
# 2735
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5EB);goto _LL2D0;_LL305: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5EC->tag != 24)goto _LL307;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;}}_LL306:
# 2737
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5ED,_tmp5EE);goto _LL2D0;_LL307: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5EF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5EF->tag != 25)goto _LL309;else{_tmp5F0=_tmp5EF->f1;}}_LL308: {
# 2741
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp624=Cyc_Tcutil_compress(*topt);void**_tmp626;struct Cyc_Absyn_Tqual*_tmp627;union Cyc_Absyn_Constraint*_tmp628;_LL327: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp625=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp624;if(_tmp625->tag != 8)goto _LL329;else{_tmp626=(void**)&(_tmp625->f1).elt_type;_tmp627=(struct Cyc_Absyn_Tqual*)&(_tmp625->f1).tq;_tmp628=(_tmp625->f1).zero_term;}}_LL328:
# 2747
 elt_topt=(void**)_tmp626;
elt_tqopt=(struct Cyc_Absyn_Tqual*)_tmp627;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp628);
goto _LL326;_LL329:;_LL32A:
 goto _LL326;_LL326:;}
# 2754
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp5F0);goto _LL2D0;}_LL309: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp5F1=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5F1->tag != 35)goto _LL30B;else{_tmp5F2=_tmp5F1->f1;}}_LL30A:
# 2756
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp5F2);goto _LL2D0;_LL30B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5F3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5F3->tag != 26)goto _LL30D;else{_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;_tmp5F6=_tmp5F3->f3;_tmp5F7=(int*)& _tmp5F3->f4;}}_LL30C:
# 2758
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp5F4,_tmp5F5,_tmp5F6,_tmp5F7);goto _LL2D0;_LL30D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5F8=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5F8->tag != 27)goto _LL30F;else{_tmp5F9=(struct _tuple1**)& _tmp5F8->f1;_tmp5FA=(struct Cyc_List_List**)& _tmp5F8->f2;_tmp5FB=_tmp5F8->f3;_tmp5FC=(struct Cyc_Absyn_Aggrdecl**)& _tmp5F8->f4;}}_LL30E:
# 2760
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp5F9,_tmp5FA,_tmp5FB,_tmp5FC);goto _LL2D0;_LL30F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp5FD->tag != 28)goto _LL311;else{_tmp5FE=(void*)_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}_LL310:
# 2762
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp5FE,_tmp5FF);goto _LL2D0;_LL311: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp600->tag != 29)goto _LL313;else{_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;_tmp603=_tmp600->f3;}}_LL312:
# 2764
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp601,_tmp602,_tmp603);goto _LL2D0;_LL313: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp604=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp604->tag != 30)goto _LL315;else{_tmp605=(struct _tuple1**)& _tmp604->f1;_tmp606=_tmp604->f2;_tmp607=_tmp604->f3;}}_LL314:
# 2766
*_tmp605=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp607))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp99A;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp999;t=(void*)((_tmp999=_cycalloc(sizeof(*_tmp999)),((_tmp999[0]=((_tmp99A.tag=13,((_tmp99A.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp606))->name,((_tmp99A.f2=_tmp606,_tmp99A)))))),_tmp999))));}goto _LL2D0;_LL315: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp608=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp608->tag != 31)goto _LL317;else{_tmp609=(struct _tuple1**)& _tmp608->f1;_tmp60A=(void*)_tmp608->f2;_tmp60B=_tmp608->f3;}}_LL316:
# 2769
*_tmp609=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp60B))->name;
t=_tmp60A;goto _LL2D0;_LL317: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp60C->tag != 32)goto _LL319;else{_tmp60D=(int*)&(_tmp60C->f1).is_calloc;_tmp60E=(struct Cyc_Absyn_Exp**)&(_tmp60C->f1).rgn;_tmp60F=(void***)&(_tmp60C->f1).elt_type;_tmp610=(struct Cyc_Absyn_Exp**)&(_tmp60C->f1).num_elts;_tmp611=(int*)&(_tmp60C->f1).fat_result;}}_LL318:
# 2772
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp60E,_tmp60F,_tmp610,_tmp60D,_tmp611);goto _LL2D0;_LL319: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp612->tag != 33)goto _LL31B;else{_tmp613=_tmp612->f1;_tmp614=_tmp612->f2;}}_LL31A:
# 2774
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp613,_tmp614);goto _LL2D0;_LL31B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp615=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp615->tag != 36)goto _LL31D;else{_tmp616=_tmp615->f1;_tmp617=_tmp615->f2;}}_LL31C:
# 2776
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp616,_tmp617);goto _LL2D0;_LL31D: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp618=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp618->tag != 37)goto _LL31F;else{_tmp619=(void*)_tmp618->f1;}}_LL31E:
# 2778
 if(!te->allow_valueof){
const char*_tmp99D;void*_tmp99C;(_tmp99C=0,Cyc_Tcutil_terr(e->loc,((_tmp99D="valueof(-) can only occur within types",_tag_dyneither(_tmp99D,sizeof(char),39))),_tag_dyneither(_tmp99C,sizeof(void*),0)));}
# 2786
t=Cyc_Absyn_sint_typ;
goto _LL2D0;_LL31F: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp61A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp59B;if(_tmp61A->tag != 38)goto _LL2D0;}_LL320:
# 2789
{const char*_tmp9A0;void*_tmp99F;(_tmp99F=0,Cyc_Tcutil_terr(e->loc,((_tmp9A0="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9A0,sizeof(char),50))),_tag_dyneither(_tmp99F,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2D0:;}
# 2792
e->topt=(void*)t;}
