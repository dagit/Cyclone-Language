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
# 168
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 173
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 182
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 218 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 228
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 230
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 232
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 236
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 242
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 246
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 256
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 261
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 264
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 269
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 274
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 283
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 339
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 342
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
{void*_tmp659;(_tmp659=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp659,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp65A;return(_tmp65A=_cycalloc(sizeof(*_tmp65A)),((_tmp65A->f1=0,((_tmp65A->f2=e,_tmp65A)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp65E;void*_tmp65D[1];struct Cyc_String_pa_PrintArg_struct _tmp65C;(_tmp65C.tag=0,((_tmp65C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp65D[0]=& _tmp65C,Cyc_Tcutil_terr(e1->loc,((_tmp65E="undeclared identifier %s",_tag_dyneither(_tmp65E,sizeof(char),25))),_tag_dyneither(_tmp65D,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp662;void*_tmp661[1];struct Cyc_String_pa_PrintArg_struct _tmp660;(_tmp660.tag=0,((_tmp660.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp661[0]=& _tmp660,Cyc_Tcutil_terr(e->loc,((_tmp662="%s is a constant, not a function",_tag_dyneither(_tmp662,sizeof(char),33))),_tag_dyneither(_tmp661,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp665;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp664;e->r=(void*)((_tmp664=_cycalloc(sizeof(*_tmp664)),((_tmp664[0]=((_tmp665.tag=29,((_tmp665.f1=es,((_tmp665.f2=_tmp10,((_tmp665.f3=_tmp11,_tmp665)))))))),_tmp664))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp668;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp667;e->r=(void*)((_tmp667=_cycalloc(sizeof(*_tmp667)),((_tmp667[0]=((_tmp668.tag=27,((_tmp668.f1=_tmp13->name,((_tmp668.f2=0,((_tmp668.f3=_tmp22,((_tmp668.f4=_tmp13,_tmp668)))))))))),_tmp667))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp66C;void*_tmp66B[1];struct Cyc_String_pa_PrintArg_struct _tmp66A;(_tmp66A.tag=0,((_tmp66A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp66B[0]=& _tmp66A,Cyc_Tcutil_terr(e->loc,((_tmp66C="%s is an enum constructor, not a function",_tag_dyneither(_tmp66C,sizeof(char),42))),_tag_dyneither(_tmp66B,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp670;void*_tmp66F[1];struct Cyc_String_pa_PrintArg_struct _tmp66E;(_tmp66E.tag=0,((_tmp66E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp66F[0]=& _tmp66E,Cyc_Tcutil_terr(e1->loc,((_tmp670="undeclared identifier %s",_tag_dyneither(_tmp670,sizeof(char),25))),_tag_dyneither(_tmp66F,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp673;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp672;e->r=(void*)((_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672[0]=((_tmp673.tag=25,((_tmp673.f1=des,_tmp673)))),_tmp672))));}
return;}{
# 125
void*t=*topt;
void*_tmp33=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp676;void*_tmp675;(_tmp675=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp676="struct type not properly set",_tag_dyneither(_tmp676,sizeof(char),29))),_tag_dyneither(_tmp675,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp679;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp678;e->r=(void*)((_tmp678=_cycalloc(sizeof(*_tmp678)),((_tmp678[0]=((_tmp679.tag=27,((_tmp679.f1=_tmp3B->name,((_tmp679.f2=0,((_tmp679.f3=des,((_tmp679.f4=_tmp3B,_tmp679)))))))))),_tmp678))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp67C;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp67B;e->r=(void*)((_tmp67B=_cycalloc(sizeof(*_tmp67B)),((_tmp67B[0]=((_tmp67C.tag=25,((_tmp67C.f1=des,_tmp67C)))),_tmp67B))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp67F;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp67E;e->r=(void*)((_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E[0]=((_tmp67F.tag=28,((_tmp67F.f1=t,((_tmp67F.f2=des,_tmp67F)))))),_tmp67E))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp682;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp681;e->r=(void*)((_tmp681=_cycalloc(sizeof(*_tmp681)),((_tmp681[0]=((_tmp682.tag=25,((_tmp682.f1=des,_tmp682)))),_tmp681))));}goto _LL17;_LL17:;};}
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
const char*_tmp687;void*_tmp686[2];struct Cyc_String_pa_PrintArg_struct _tmp685;struct Cyc_String_pa_PrintArg_struct _tmp684;(_tmp684.tag=0,((_tmp684.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp685.tag=0,((_tmp685.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp686[0]=& _tmp685,((_tmp686[1]=& _tmp684,Cyc_Tcutil_terr(e->loc,((_tmp687="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp687,sizeof(char),53))),_tag_dyneither(_tmp686,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4A=e->r;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp688;struct _tuple0 _tmp4F=(_tmp688.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp688.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp688)));void*_tmp51;void*_tmp53;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f1;if(_tmp50->tag != 15)goto _LL2D;else{_tmp51=(void*)_tmp50->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f2;if(_tmp52->tag != 15)goto _LL2D;else{_tmp53=(void*)_tmp52->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp68B;struct Cyc_Tcexp_TestEnv _tmp68A;return(_tmp68A.eq=((_tmp68B=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp68B)),((_tmp68B->f1=_tmp51,((_tmp68B->f2=_tmp53,_tmp68B)))))),((_tmp68A.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp68A)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp68C;return(_tmp68C.eq=0,((_tmp68C.isTrue=0,_tmp68C)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp692;struct Cyc_Absyn_PtrInfo _tmp691;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp690;t=(void*)((_tmp690=_cycalloc(sizeof(*_tmp690)),((_tmp690[0]=((_tmp692.tag=5,((_tmp692.f1=((_tmp691.elt_typ=_tmp69,((_tmp691.elt_tq=_tmp6A,((_tmp691.ptr_atts=(
((_tmp691.ptr_atts).rgn=_tmp6B,(((_tmp691.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp691.ptr_atts).bounds=_tmp6D,(((_tmp691.ptr_atts).zero_term=_tmp6E,(((_tmp691.ptr_atts).ptrloc=0,_tmp691.ptr_atts)))))))))),_tmp691)))))),_tmp692)))),_tmp690))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp695;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp694;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp694=_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp695.tag=18,((_tmp695.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp695)))),_tmp694)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp698;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp697;t=(void*)((_tmp697=_cycalloc(sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=19,((_tmp698.f1=(void*)_tmp76,_tmp698)))),_tmp697))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp69B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp69A;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp69A=_cycalloc(sizeof(*_tmp69A)),((_tmp69A[0]=((_tmp69B.tag=1,((_tmp69B.f1=elen,_tmp69B)))),_tmp69A)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6A1;struct Cyc_Absyn_PtrInfo _tmp6A0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69F;return(void*)((_tmp69F=_cycalloc(sizeof(*_tmp69F)),((_tmp69F[0]=((_tmp6A1.tag=5,((_tmp6A1.f1=((_tmp6A0.elt_typ=_tmp84,((_tmp6A0.elt_tq=_tmp85,((_tmp6A0.ptr_atts=(((_tmp6A0.ptr_atts).rgn=_tmp86,(((_tmp6A0.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6A0.ptr_atts).bounds=_tmp88,(((_tmp6A0.ptr_atts).zero_term=_tmp89,(((_tmp6A0.ptr_atts).ptrloc=0,_tmp6A0.ptr_atts)))))))))),_tmp6A0)))))),_tmp6A1)))),_tmp69F))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6B0;struct Cyc_Core_Opt*_tmp6AF;struct Cyc_Core_Opt*_tmp6AE;struct Cyc_Absyn_PtrInfo _tmp6AD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AC;t=(void*)((_tmp6AC=_cycalloc(sizeof(*_tmp6AC)),((_tmp6AC[0]=((_tmp6B0.tag=5,((_tmp6B0.f1=((_tmp6AD.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6AE=_cycalloc(sizeof(*_tmp6AE)),((_tmp6AE->v=_tmp8D,_tmp6AE))))),((_tmp6AD.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6AD.ptr_atts=(
((_tmp6AD.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6AF=_cycalloc(sizeof(*_tmp6AF)),((_tmp6AF->v=_tmp8D,_tmp6AF))))),(((_tmp6AD.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6AD.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6AD.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6AD.ptr_atts).ptrloc=0,_tmp6AD.ptr_atts)))))))))),_tmp6AD)))))),_tmp6B0)))),_tmp6AC))));}
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
const char*_tmp6B4;void*_tmp6B3[1];struct Cyc_String_pa_PrintArg_struct _tmp6B2;void*_tmpB5=(_tmp6B2.tag=0,((_tmp6B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6B3[0]=& _tmp6B2,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6B4="undeclared identifier %s",_tag_dyneither(_tmp6B4,sizeof(char),25))),_tag_dyneither(_tmp6B3,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6B7;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6B6;e->r=(void*)((_tmp6B6=_cycalloc(sizeof(*_tmp6B6)),((_tmp6B6[0]=((_tmp6B7.tag=30,((_tmp6B7.f1=_tmpAA->name,((_tmp6B7.f2=_tmpA9,((_tmp6B7.f3=_tmpAA,_tmp6B7)))))))),_tmp6B6))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6BA;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6B9;void*_tmpBB=(void*)((_tmp6B9=_cycalloc(sizeof(*_tmp6B9)),((_tmp6B9[0]=((_tmp6BA.tag=13,((_tmp6BA.f1=_tmpA9->name,((_tmp6BA.f2=_tmpA9,_tmp6BA)))))),_tmp6B9))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6BD;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6BC;e->r=(void*)((_tmp6BC=_cycalloc(sizeof(*_tmp6BC)),((_tmp6BC[0]=((_tmp6BD.tag=31,((_tmp6BD.f1=_tmpAD->name,((_tmp6BD.f2=_tmpAC,((_tmp6BD.f3=_tmpAD,_tmp6BD)))))))),_tmp6BC))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6C0;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6BF;e->r=(void*)((_tmp6BF=_cycalloc(sizeof(*_tmp6BF)),((_tmp6BF[0]=((_tmp6C0.tag=29,((_tmp6C0.f1=0,((_tmp6C0.f2=_tmpAF,((_tmp6C0.f3=_tmpB0,_tmp6C0)))))))),_tmp6BF))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6C4;void*_tmp6C3[1];struct Cyc_String_pa_PrintArg_struct _tmp6C2;void*_tmpC5=(_tmp6C2.tag=0,((_tmp6C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C3[0]=& _tmp6C2,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C4="bad occurrence of type name %s",_tag_dyneither(_tmp6C4,sizeof(char),31))),_tag_dyneither(_tmp6C3,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6C8;void*_tmp6C7[1];struct Cyc_String_pa_PrintArg_struct _tmp6C6;return(_tmp6C6.tag=0,((_tmp6C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C7[0]=& _tmp6C6,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C8="undeclared identifier %s",_tag_dyneither(_tmp6C8,sizeof(char),25))),_tag_dyneither(_tmp6C7,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6C9;(*q).f1=(((_tmp6C9.Loc_n).val=0,(((_tmp6C9.Loc_n).tag=4,_tmp6C9))));}
if(te->allow_valueof){
void*_tmpCE=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6CC;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6CB;e->r=(void*)((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB[0]=((_tmp6CC.tag=37,((_tmp6CC.f1=_tmpD0,_tmp6CC)))),_tmp6CB))));}
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
const char*_tmp6CF;void*_tmp6CE;(_tmp6CE=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp6CF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6CF,sizeof(char),49))),_tag_dyneither(_tmp6CE,sizeof(void*),0)));}
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
{const char*_tmp6D4;void*_tmp6D3[2];struct Cyc_String_pa_PrintArg_struct _tmp6D2;struct Cyc_String_pa_PrintArg_struct _tmp6D1;(_tmp6D1.tag=0,((_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6D2.tag=0,((_tmp6D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6D3[0]=& _tmp6D2,((_tmp6D3[1]=& _tmp6D1,Cyc_Tcutil_terr(e->loc,((_tmp6D4="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp6D4,sizeof(char),51))),_tag_dyneither(_tmp6D3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6D5;*alias_arg_exps=((_tmp6D5=_cycalloc(sizeof(*_tmp6D5)),((_tmp6D5->hd=(void*)arg_cnt,((_tmp6D5->tl=*alias_arg_exps,_tmp6D5))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6D8;void*_tmp6D7;(_tmp6D7=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6D8="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6D8,sizeof(char),49))),_tag_dyneither(_tmp6D7,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6DB;void*_tmp6DA;(_tmp6DA=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6DB="too few arguments",_tag_dyneither(_tmp6DB,sizeof(char),18))),_tag_dyneither(_tmp6DA,sizeof(void*),0)));}
if(_tmpDD != 0){
const char*_tmp6DE;void*_tmp6DD;(_tmp6DD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6DE="too many arguments",_tag_dyneither(_tmp6DE,sizeof(char),19))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6E2;void*_tmp6E1[1];struct Cyc_String_pa_PrintArg_struct _tmp6E0;(_tmp6E0.tag=0,((_tmp6E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E1[0]=& _tmp6E0,Cyc_Tcutil_terr(loc,((_tmp6E2="expecting arithmetic type but found %s",_tag_dyneither(_tmp6E2,sizeof(char),39))),_tag_dyneither(_tmp6E1,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6E6;void*_tmp6E5[1];struct Cyc_String_pa_PrintArg_struct _tmp6E4;(_tmp6E4.tag=0,((_tmp6E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E5[0]=& _tmp6E4,Cyc_Tcutil_terr(loc,((_tmp6E6="expecting integral or * type but found %s",_tag_dyneither(_tmp6E6,sizeof(char),42))),_tag_dyneither(_tmp6E5,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6EA;void*_tmp6E9[1];struct Cyc_String_pa_PrintArg_struct _tmp6E8;(_tmp6E8.tag=0,((_tmp6E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E9[0]=& _tmp6E8,Cyc_Tcutil_terr(loc,((_tmp6EA="expecting integral type but found %s",_tag_dyneither(_tmp6EA,sizeof(char),37))),_tag_dyneither(_tmp6E9,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF3=t;union Cyc_Absyn_Constraint*_tmpF6;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3;if(_tmpF4->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3;if(_tmpF5->tag != 5)goto _LL9B;else{_tmpF6=((_tmpF5->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_tmpF7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF6);struct Cyc_Absyn_Exp*_tmpFA;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpF8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF8->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF9->tag != 1)goto _LL9D;else{_tmpFA=_tmpF9->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFA)){
const char*_tmp6ED;void*_tmp6EC;(_tmp6EC=0,Cyc_Tcutil_terr(loc,((_tmp6ED="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp6ED,sizeof(char),55))),_tag_dyneither(_tmp6EC,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp6F1;void*_tmp6F0[1];struct Cyc_String_pa_PrintArg_struct _tmp6EF;(_tmp6EF.tag=0,((_tmp6EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F0[0]=& _tmp6EF,Cyc_Tcutil_terr(loc,((_tmp6F1="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp6F1,sizeof(char),47))),_tag_dyneither(_tmp6F0,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp6F4;void*_tmp6F3;(_tmp6F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6F4="Non-unary primop",_tag_dyneither(_tmp6F4,sizeof(char),17))),_tag_dyneither(_tmp6F3,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp6F8;void*_tmp6F7[1];struct Cyc_String_pa_PrintArg_struct _tmp6F6;(_tmp6F6.tag=0,((_tmp6F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp6F7[0]=& _tmp6F6,Cyc_Tcutil_terr(e1->loc,((_tmp6F8="type %s cannot be used here",_tag_dyneither(_tmp6F8,sizeof(char),28))),_tag_dyneither(_tmp6F7,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp6FC;void*_tmp6FB[1];struct Cyc_String_pa_PrintArg_struct _tmp6FA;(_tmp6FA.tag=0,((_tmp6FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp6FB[0]=& _tmp6FA,Cyc_Tcutil_terr(e2->loc,((_tmp6FC="type %s cannot be used here",_tag_dyneither(_tmp6FC,sizeof(char),28))),_tag_dyneither(_tmp6FB,sizeof(void*),1)))))));}
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
const char*_tmp6FF;void*_tmp6FE;(_tmp6FE=0,Cyc_Tcutil_terr(e1->loc,((_tmp6FF="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp6FF,sizeof(char),50))),_tag_dyneither(_tmp6FE,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp702;void*_tmp701;(_tmp701=0,Cyc_Tcutil_terr(e1->loc,((_tmp702="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp702,sizeof(char),54))),_tag_dyneither(_tmp701,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp706;void*_tmp705[1];struct Cyc_String_pa_PrintArg_struct _tmp704;(_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp705[0]=& _tmp704,Cyc_Tcutil_terr(e2->loc,((_tmp706="expecting int but found %s",_tag_dyneither(_tmp706,sizeof(char),27))),_tag_dyneither(_tmp705,sizeof(void*),1)))))));}{
void*_tmp117=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp10E);struct Cyc_Absyn_Exp*_tmp11A;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp118=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp117;if(_tmp118->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp117;if(_tmp119->tag != 1)goto _LLA8;else{_tmp11A=_tmp119->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10F)){
const char*_tmp709;void*_tmp708;(_tmp708=0,Cyc_Tcutil_warn(e1->loc,((_tmp709="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp709,sizeof(char),70))),_tag_dyneither(_tmp708,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp70F;struct Cyc_Absyn_PtrInfo _tmp70E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11D=(_tmp70D=_cycalloc(sizeof(*_tmp70D)),((_tmp70D[0]=((_tmp70F.tag=5,((_tmp70F.f1=((_tmp70E.elt_typ=_tmp10A,((_tmp70E.elt_tq=_tmp10B,((_tmp70E.ptr_atts=(
((_tmp70E.ptr_atts).rgn=_tmp10C,(((_tmp70E.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp70E.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp70E.ptr_atts).zero_term=_tmp10F,(((_tmp70E.ptr_atts).ptrloc=0,_tmp70E.ptr_atts)))))))))),_tmp70E)))))),_tmp70F)))),_tmp70D)));
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
{const char*_tmp714;void*_tmp713[2];struct Cyc_String_pa_PrintArg_struct _tmp712;struct Cyc_String_pa_PrintArg_struct _tmp711;(_tmp711.tag=0,((_tmp711.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp713[0]=& _tmp712,((_tmp713[1]=& _tmp711,Cyc_Tcutil_terr(e1->loc,((_tmp714="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp714,sizeof(char),59))),_tag_dyneither(_tmp713,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp719;void*_tmp718[2];struct Cyc_String_pa_PrintArg_struct _tmp717;struct Cyc_String_pa_PrintArg_struct _tmp716;(_tmp716.tag=0,((_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp717.tag=0,((_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp718[0]=& _tmp717,((_tmp718[1]=& _tmp716,Cyc_Tcutil_terr(e1->loc,((_tmp719="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp719,sizeof(char),59))),_tag_dyneither(_tmp718,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp71C;void*_tmp71B;(_tmp71B=0,Cyc_Tcutil_warn(e1->loc,((_tmp71C="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp71C,sizeof(char),60))),_tag_dyneither(_tmp71B,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp71F;void*_tmp71E;(_tmp71E=0,Cyc_Tcutil_terr(e1->loc,((_tmp71F="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp71F,sizeof(char),50))),_tag_dyneither(_tmp71E,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp722;void*_tmp721;(_tmp721=0,Cyc_Tcutil_terr(e1->loc,((_tmp722="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp722,sizeof(char),54))),_tag_dyneither(_tmp721,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp727;void*_tmp726[2];struct Cyc_String_pa_PrintArg_struct _tmp725;struct Cyc_String_pa_PrintArg_struct _tmp724;(_tmp724.tag=0,((_tmp724.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp726[0]=& _tmp725,((_tmp726[1]=& _tmp724,Cyc_Tcutil_terr(e2->loc,((_tmp727="expecting either %s or int but found %s",_tag_dyneither(_tmp727,sizeof(char),40))),_tag_dyneither(_tmp726,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp72A;void*_tmp729;(_tmp729=0,Cyc_Tcutil_warn(e1->loc,((_tmp72A="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp72A,sizeof(char),60))),_tag_dyneither(_tmp729,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp72D;void*_tmp72C;(_tmp72C=0,Cyc_Tcutil_warn(e1->loc,((_tmp72D="thin pointer subtraction!",_tag_dyneither(_tmp72D,sizeof(char),26))),_tag_dyneither(_tmp72C,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp730;void*_tmp72F;(_tmp72F=0,Cyc_Tcutil_warn(e1->loc,((_tmp730="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp730,sizeof(char),56))),_tag_dyneither(_tmp72F,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_warn(e1->loc,((_tmp733="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp733,sizeof(char),51))),_tag_dyneither(_tmp732,sizeof(void*),0)));}
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
struct _tuple0 _tmp734;struct _tuple0 _tmp13C=(_tmp734.f1=Cyc_Tcutil_compress(t1),((_tmp734.f2=Cyc_Tcutil_compress(t2),_tmp734)));void*_tmp13E;void*_tmp140;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C.f1;if(_tmp13D->tag != 5)goto _LLB0;else{_tmp13E=(_tmp13D->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13C.f2;if(_tmp13F->tag != 5)goto _LLB0;else{_tmp140=(_tmp13F->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp13E,_tmp140))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13C.f1;if(_tmp141->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp142=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13C.f2;if(_tmp142->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp739;void*_tmp738[2];struct Cyc_String_pa_PrintArg_struct _tmp737;struct Cyc_String_pa_PrintArg_struct _tmp736;(_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp737.tag=0,((_tmp737.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp738[0]=& _tmp737,((_tmp738[1]=& _tmp736,Cyc_Tcutil_terr(loc,((_tmp739="comparison not allowed between %s and %s",_tag_dyneither(_tmp739,sizeof(char),41))),_tag_dyneither(_tmp738,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp73A;struct _tuple0 _tmp148=(_tmp73A.f1=t1,((_tmp73A.f2=t2,_tmp73A)));void*_tmp14A;void*_tmp14C;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp149=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp148.f1;if(_tmp149->tag != 15)goto _LLB7;else{_tmp14A=(void*)_tmp149->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp148.f2;if(_tmp14B->tag != 15)goto _LLB7;else{_tmp14C=(void*)_tmp14B->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp73F;void*_tmp73E[2];struct Cyc_String_pa_PrintArg_struct _tmp73D;struct Cyc_String_pa_PrintArg_struct _tmp73C;(_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp73E[0]=& _tmp73D,((_tmp73E[1]=& _tmp73C,Cyc_Tcutil_terr(loc,((_tmp73F="comparison not allowed between %s and %s",_tag_dyneither(_tmp73F,sizeof(char),41))),_tag_dyneither(_tmp73E,sizeof(void*),2)))))))))))));}
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
const char*_tmp742;void*_tmp741;(_tmp741=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp742="bad binary primop",_tag_dyneither(_tmp742,sizeof(char),18))),_tag_dyneither(_tmp741,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
const char*_tmp745;void*_tmp744;return(_tmp744=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp745="primitive operator has 0 arguments",_tag_dyneither(_tmp745,sizeof(char),35))),_tag_dyneither(_tmp744,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp748;void*_tmp747;return(_tmp747=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp748="primitive operator has > 2 arguments",_tag_dyneither(_tmp748,sizeof(char),37))),_tag_dyneither(_tmp747,sizeof(void*),0)));}}
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
{const char*_tmp74C;void*_tmp74B[1];struct Cyc_String_pa_PrintArg_struct _tmp74A;(_tmp74A.tag=0,((_tmp74A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp164->name),((_tmp74B[0]=& _tmp74A,Cyc_Tcutil_terr(loc,((_tmp74C="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp74C,sizeof(char),56))),_tag_dyneither(_tmp74B,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp164->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp158;if(_tmp15D->tag != 4)goto _LLD8;else{if((((_tmp15D->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp15E=((struct _tuple2)(((_tmp15D->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp168=_tmp15E->typs;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;struct _tuple19*_tmp169=(struct _tuple19*)_tmp168->hd;_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;
if(_tmp16A.real_const){
{const char*_tmp750;void*_tmp74F[1];struct Cyc_String_pa_PrintArg_struct _tmp74E;(_tmp74E.tag=0,((_tmp74E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name)),((_tmp74F[0]=& _tmp74E,Cyc_Tcutil_terr(loc,((_tmp750="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp750,sizeof(char),64))),_tag_dyneither(_tmp74F,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16B))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp15F->tag != 8)goto _LLDA;else{_tmp160=(_tmp15F->f1).elt_type;_tmp161=(_tmp15F->f1).tq;}}_LLD9:
# 745
 if(_tmp161.real_const){
{const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_Tcutil_terr(loc,((_tmp753="attempt to over-write a const array",_tag_dyneither(_tmp753,sizeof(char),36))),_tag_dyneither(_tmp752,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp160);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp158;if(_tmp162->tag != 10)goto _LLDC;else{_tmp163=_tmp162->f1;}}_LLDB:
# 751
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct Cyc_Absyn_Tqual _tmp172;void*_tmp173;struct _tuple19*_tmp171=(struct _tuple19*)_tmp163->hd;_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;
if(_tmp172.real_const){
{const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_Tcutil_terr(loc,((_tmp756="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp756,sizeof(char),56))),_tag_dyneither(_tmp755,sizeof(void*),0)));}
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
{const char*_tmp759;void*_tmp758;(_tmp758=0,Cyc_Tcutil_terr(e->loc,((_tmp759="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp759,sizeof(char),47))),_tag_dyneither(_tmp758,sizeof(void*),0)));}
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
const char*_tmp75D;void*_tmp75C[1];struct Cyc_String_pa_PrintArg_struct _tmp75B;(_tmp75B.tag=0,((_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp75C[0]=& _tmp75B,Cyc_Tcutil_terr(e->loc,((_tmp75D="attempt to write a const location: %s",_tag_dyneither(_tmp75D,sizeof(char),38))),_tag_dyneither(_tmp75C,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1BC=_new_region("temp");struct _RegionHandle*temp=& _tmp1BC;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp760;void*_tmp75F;void*_tmp1BF=(_tmp75F=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp760="increment/decrement of non-lvalue",_tag_dyneither(_tmp760,sizeof(char),34))),_tag_dyneither(_tmp75F,sizeof(void*),0)));_npop_handler(0);return _tmp1BF;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp763;void*_tmp762;(_tmp762=0,Cyc_Tcutil_terr(e->loc,((_tmp763="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp763,sizeof(char),50))),_tag_dyneither(_tmp762,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp766;void*_tmp765;(_tmp765=0,Cyc_Tcutil_terr(e->loc,((_tmp766="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp766,sizeof(char),54))),_tag_dyneither(_tmp765,sizeof(void*),0)));}}else{
# 861
const char*_tmp76A;void*_tmp769[1];struct Cyc_String_pa_PrintArg_struct _tmp768;(_tmp768.tag=0,((_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp769[0]=& _tmp768,Cyc_Tcutil_terr(e->loc,((_tmp76A="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp76A,sizeof(char),44))),_tag_dyneither(_tmp769,sizeof(void*),1)))))));}}{
# 863
void*_tmp1C7=t;_npop_handler(0);return _tmp1C7;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _tuple0*_tmp1CA;int _tmp1CB;const char*_tmp76B;struct Cyc_Tcexp_TestEnv _tmp1C9=Cyc_Tcexp_tcTest(te,e1,((_tmp76B="conditional expression",_tag_dyneither(_tmp76B,sizeof(char),23))));_tmp1CA=_tmp1C9.eq;_tmp1CB=_tmp1C9.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_List_List _tmp76C;struct Cyc_List_List _tmp1CC=(_tmp76C.hd=e3,((_tmp76C.tl=0,_tmp76C)));
struct Cyc_List_List _tmp76D;struct Cyc_List_List _tmp1CD=(_tmp76D.hd=e2,((_tmp76D.tl=& _tmp1CC,_tmp76D)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1CD)){
{const char*_tmp772;void*_tmp771[2];struct Cyc_String_pa_PrintArg_struct _tmp770;struct Cyc_String_pa_PrintArg_struct _tmp76F;(_tmp76F.tag=0,((_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp770.tag=0,((_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp771[0]=& _tmp770,((_tmp771[1]=& _tmp76F,Cyc_Tcutil_terr(loc,((_tmp772="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp772,sizeof(char),48))),_tag_dyneither(_tmp771,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 880
return t;};}
# 884
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 886
{const char*_tmp773;Cyc_Tcexp_tcTest(te,e1,((_tmp773="logical-and expression",_tag_dyneither(_tmp773,sizeof(char),23))));}
{const char*_tmp774;Cyc_Tcexp_tcTest(te,e2,((_tmp774="logical-and expression",_tag_dyneither(_tmp774,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 892
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 894
{const char*_tmp775;Cyc_Tcexp_tcTest(te,e1,((_tmp775="logical-or expression",_tag_dyneither(_tmp775,sizeof(char),22))));}
{const char*_tmp776;Cyc_Tcexp_tcTest(te,e2,((_tmp776="logical-or expression",_tag_dyneither(_tmp776,sizeof(char),22))));}
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
{const char*_tmp779;void*_tmp778;(_tmp778=0,Cyc_Tcutil_terr(loc,((_tmp779="cannot assign to an array",_tag_dyneither(_tmp779,sizeof(char),26))),_tag_dyneither(_tmp778,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 919
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(loc,((_tmp77C="type is abstract (can't determine size).",_tag_dyneither(_tmp77C,sizeof(char),41))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
# 923
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp77F;void*_tmp77E;void*_tmp1E1=(_tmp77E=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp77F="assignment to non-lvalue",_tag_dyneither(_tmp77F,sizeof(char),25))),_tag_dyneither(_tmp77E,sizeof(void*),0)));_npop_handler(0);return _tmp1E1;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E2=_new_region("temp");struct _RegionHandle*temp=& _tmp1E2;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp782;void*_tmp781;(_tmp781=0,Cyc_Tcutil_terr(e2->loc,((_tmp782="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp782,sizeof(char),49))),_tag_dyneither(_tmp781,sizeof(void*),0)));}
# 931
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp787;void*_tmp786[2];struct Cyc_String_pa_PrintArg_struct _tmp785;struct Cyc_String_pa_PrintArg_struct _tmp784;void*_tmp1E5=(_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp785.tag=0,((_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp786[0]=& _tmp785,((_tmp786[1]=& _tmp784,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp787="type mismatch: %s != %s",_tag_dyneither(_tmp787,sizeof(char),24))),_tag_dyneither(_tmp786,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1E6=_tmp1E5;_npop_handler(1);return _tmp1E6;};}
# 927
;_pop_region(temp);}else{
# 939
enum Cyc_Absyn_Primop _tmp1EB=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1EC=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EB,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1EC,t1) || Cyc_Tcutil_coerceable(_tmp1EC) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp78C;void*_tmp78B[2];struct Cyc_String_pa_PrintArg_struct _tmp78A;struct Cyc_String_pa_PrintArg_struct _tmp789;void*_tmp1ED=
(_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 945
Cyc_Absynpp_typ2string(t2)),((_tmp78A.tag=0,((_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 944
Cyc_Absynpp_typ2string(t1)),((_tmp78B[0]=& _tmp78A,((_tmp78B[1]=& _tmp789,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78C="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp78C,sizeof(char),82))),_tag_dyneither(_tmp78B,sizeof(void*),2)))))))))))));
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
{const char*_tmp791;void*_tmp790[2];struct Cyc_String_pa_PrintArg_struct _tmp78F;struct Cyc_String_pa_PrintArg_struct _tmp78E;(_tmp78E.tag=0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp78F.tag=0,((_tmp78F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp790[0]=& _tmp78F,((_tmp790[1]=& _tmp78E,Cyc_Tcutil_terr(e->loc,((_tmp791="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp791,sizeof(char),54))),_tag_dyneither(_tmp790,sizeof(void*),2)))))))))))));}
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
# 1035
 if(topt != 0)Cyc_Tcutil_unify(_tmp218,*topt);
# 1037
while(_tmp207 != 0  && _tmp219 != 0){
# 1039
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
void*t2=(*((struct _tuple9*)_tmp219->hd)).f3;
Cyc_Tcexp_tcExp(_tmp20A,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20A,e1,t2,& alias_coercion)){
{const char*_tmp796;void*_tmp795[2];struct Cyc_String_pa_PrintArg_struct _tmp794;struct Cyc_String_pa_PrintArg_struct _tmp793;(_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1047
Cyc_Absynpp_typ2string(t2)),((_tmp794.tag=0,((_tmp794.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp795[0]=& _tmp794,((_tmp795[1]=& _tmp793,Cyc_Tcutil_terr(e1->loc,((_tmp796="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp796,sizeof(char),57))),_tag_dyneither(_tmp795,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1052
if(alias_coercion){
struct Cyc_List_List*_tmp797;*alias_arg_exps=((_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797->hd=(void*)_tmp208,((_tmp797->tl=*alias_arg_exps,_tmp797))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp79A;void*_tmp799;(_tmp799=0,Cyc_Tcutil_terr(e1->loc,((_tmp79A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp79A,sizeof(char),49))),_tag_dyneither(_tmp799,sizeof(void*),0)));}
_tmp207=_tmp207->tl;
_tmp219=_tmp219->tl;
++ _tmp208;}{
# 1063
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp21D;for(0;a != 0;a=a->tl){
void*_tmp225=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp227;int _tmp228;int _tmp229;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp226=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225;if(_tmp226->tag != 19)goto _LL138;else{_tmp227=_tmp226->f1;_tmp228=_tmp226->f2;_tmp229=_tmp226->f3;}}_LL137:
# 1067
{struct _handler_cons _tmp22A;_push_handler(& _tmp22A);{int _tmp22C=0;if(setjmp(_tmp22A.handler))_tmp22C=1;if(!_tmp22C){
# 1069
{struct Cyc_Absyn_Exp*_tmp22D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp228 - 1);
# 1071
struct Cyc_Core_Opt*fmt_args;
if(_tmp229 == 0)
fmt_args=0;else{
# 1075
struct Cyc_Core_Opt*_tmp79B;fmt_args=((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp229 - 1),_tmp79B))));}
args_already_checked=1;{
struct _RegionHandle _tmp22F=_new_region("temp");struct _RegionHandle*temp=& _tmp22F;_push_region(temp);
switch(_tmp227){case Cyc_Absyn_Printf_ft: _LL13A:
# 1080
 Cyc_Tcexp_check_format_args(_tmp20A,_tmp22D,fmt_args,_tmp229 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1083
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1085
 Cyc_Tcexp_check_format_args(_tmp20A,_tmp22D,fmt_args,_tmp229 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1088
break;}
# 1078
;_pop_region(temp);};}
# 1069
;_pop_handler();}else{void*_tmp22B=(void*)_exn_thrown;void*_tmp231=_tmp22B;void*_tmp233;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp232=(struct Cyc_List_Nth_exn_struct*)_tmp231;if(_tmp232->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1092
{const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_Tcutil_terr(loc,((_tmp79E="bad format arguments",_tag_dyneither(_tmp79E,sizeof(char),21))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp233=_tmp231;_LL141:(void)_throw(_tmp233);_LL13D:;}};}
# 1094
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1098
if(_tmp219 != 0){const char*_tmp7A1;void*_tmp7A0;(_tmp7A0=0,Cyc_Tcutil_terr(loc,((_tmp7A1="too few arguments for function",_tag_dyneither(_tmp7A1,sizeof(char),31))),_tag_dyneither(_tmp7A0,sizeof(void*),0)));}else{
# 1100
if((_tmp207 != 0  || _tmp21A) || _tmp21B != 0){
if(_tmp21A)
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
Cyc_Tcexp_tcExp(_tmp20A,0,(struct Cyc_Absyn_Exp*)_tmp207->hd);}else{
if(_tmp21B == 0){
const char*_tmp7A4;void*_tmp7A3;(_tmp7A3=0,Cyc_Tcutil_terr(loc,((_tmp7A4="too many arguments for function",_tag_dyneither(_tmp7A4,sizeof(char),32))),_tag_dyneither(_tmp7A3,sizeof(void*),0)));}else{
# 1107
void*_tmp23B;int _tmp23C;struct Cyc_Absyn_VarargInfo _tmp23A=*_tmp21B;_tmp23B=_tmp23A.type;_tmp23C=_tmp23A.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7A5;struct Cyc_Absyn_VarargCallInfo*_tmp23D=(_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5->num_varargs=0,((_tmp7A5->injectors=0,((_tmp7A5->vai=_tmp21B,_tmp7A5)))))));
# 1111
*vararg_call_info=_tmp23D;
# 1113
if(!_tmp23C)
# 1115
for(0;_tmp207 != 0;(_tmp207=_tmp207->tl,_tmp208 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
++ _tmp23D->num_varargs;
Cyc_Tcexp_tcExp(_tmp20A,& _tmp23B,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20A,e1,_tmp23B,& alias_coercion)){
{const char*_tmp7AA;void*_tmp7A9[2];struct Cyc_String_pa_PrintArg_struct _tmp7A8;struct Cyc_String_pa_PrintArg_struct _tmp7A7;(_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7A8.tag=0,((_tmp7A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23B)),((_tmp7A9[0]=& _tmp7A8,((_tmp7A9[1]=& _tmp7A7,Cyc_Tcutil_terr(loc,((_tmp7AA="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7AA,sizeof(char),49))),_tag_dyneither(_tmp7A9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1125
if(alias_coercion){
struct Cyc_List_List*_tmp7AB;*alias_arg_exps=((_tmp7AB=_cycalloc(sizeof(*_tmp7AB)),((_tmp7AB->hd=(void*)_tmp208,((_tmp7AB->tl=*alias_arg_exps,_tmp7AB))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23B) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7AE;void*_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_terr(e1->loc,((_tmp7AE="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7AE,sizeof(char),49))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}}else{
# 1134
void*_tmp245=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23B));struct Cyc_Absyn_Datatypedecl*_tmp247;struct Cyc_List_List*_tmp248;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp245;if(_tmp246->tag != 3)goto _LL145;else{if((((_tmp246->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp247=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp246->f1).datatype_info).KnownDatatype).val);_tmp248=(_tmp246->f1).targs;}}_LL144: {
# 1138
struct Cyc_Absyn_Datatypedecl*_tmp249=*Cyc_Tcenv_lookup_datatypedecl(_tmp20A,loc,_tmp247->name);
struct Cyc_List_List*fields=0;
if(_tmp249->fields == 0){
const char*_tmp7B2;void*_tmp7B1[1];struct Cyc_String_pa_PrintArg_struct _tmp7B0;(_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23B)),((_tmp7B1[0]=& _tmp7B0,Cyc_Tcutil_terr(loc,((_tmp7B2="can't inject into abstract datatype %s",_tag_dyneither(_tmp7B2,sizeof(char),39))),_tag_dyneither(_tmp7B1,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp249->fields))->v;}
# 1150
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23B),Cyc_Tcenv_curr_rgn(_tmp20A))){
const char*_tmp7B7;void*_tmp7B6[2];struct Cyc_String_pa_PrintArg_struct _tmp7B5;struct Cyc_String_pa_PrintArg_struct _tmp7B4;(_tmp7B4.tag=0,((_tmp7B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20A))),((_tmp7B5.tag=0,((_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23B)),((_tmp7B6[0]=& _tmp7B5,((_tmp7B6[1]=& _tmp7B4,Cyc_Tcutil_terr(loc,((_tmp7B7="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7B7,sizeof(char),49))),_tag_dyneither(_tmp7B6,sizeof(void*),2)))))))))))));}{
# 1154
struct _RegionHandle _tmp251=_new_region("rgn");struct _RegionHandle*rgn=& _tmp251;_push_region(rgn);{
struct Cyc_List_List*_tmp252=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp249->tvs,_tmp248);
for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
++ _tmp23D->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp207->hd;
# 1160
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp20A,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp7BA;void*_tmp7B9;(_tmp7B9=0,Cyc_Tcutil_terr(e1->loc,((_tmp7BA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7BA,sizeof(char),49))),_tag_dyneither(_tmp7B9,sizeof(void*),0)));}}{
# 1166
struct Cyc_Absyn_Datatypefield*_tmp255=Cyc_Tcexp_tcInjection(_tmp20A,e1,Cyc_Tcutil_pointer_elt_type(_tmp23B),rgn,_tmp252,fields);
# 1168
if(_tmp255 != 0){
struct Cyc_List_List*_tmp7BB;_tmp23D->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp23D->injectors,(
(_tmp7BB=_cycalloc(sizeof(*_tmp7BB)),((_tmp7BB->hd=_tmp255,((_tmp7BB->tl=0,_tmp7BB)))))));}};};}}
# 1174
_npop_handler(0);goto _LL142;
# 1154
;_pop_region(rgn);};}_LL145:;_LL146:
# 1175
{const char*_tmp7BE;void*_tmp7BD;(_tmp7BD=0,Cyc_Tcutil_terr(loc,((_tmp7BE="bad inject vararg type",_tag_dyneither(_tmp7BE,sizeof(char),23))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1180
if(*alias_arg_exps == 0){
# 1189 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp20A,loc,(void*)_check_null(_tmp216));
# 1193
Cyc_Tcenv_check_rgn_partial_order(_tmp20A,loc,_tmp21C);}{
# 1195
void*_tmp25A=_tmp218;_npop_handler(0);return _tmp25A;};};_LL133:;_LL134: {
const char*_tmp7C1;void*_tmp7C0;void*_tmp25D=(_tmp7C0=0,Cyc_Tcexp_expr_err(_tmp20A,loc,topt,((_tmp7C1="expected pointer to function",_tag_dyneither(_tmp7C1,sizeof(char),29))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));_npop_handler(0);return _tmp25D;}_LL130:;};_LL12E:;_LL12F: {
# 1198
const char*_tmp7C4;void*_tmp7C3;void*_tmp260=(_tmp7C3=0,Cyc_Tcexp_expr_err(_tmp20A,loc,topt,((_tmp7C4="expected pointer to function",_tag_dyneither(_tmp7C4,sizeof(char),29))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));_npop_handler(0);return _tmp260;}_LL12B:;};}
# 1015 "tcexp.cyc"
;_pop_region(ter);}
# 1204 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp261=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp261,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp261,& bogus)){
const char*_tmp7C9;void*_tmp7C8[2];struct Cyc_String_pa_PrintArg_struct _tmp7C7;struct Cyc_String_pa_PrintArg_struct _tmp7C6;(_tmp7C6.tag=0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7C7.tag=0,((_tmp7C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1209
Cyc_Absynpp_typ2string(_tmp261)),((_tmp7C8[0]=& _tmp7C7,((_tmp7C8[1]=& _tmp7C6,Cyc_Tcutil_terr(loc,((_tmp7C9="expected %s but found %s",_tag_dyneither(_tmp7C9,sizeof(char),25))),_tag_dyneither(_tmp7C8,sizeof(void*),2)))))))))))));}
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
{void*_tmp266=t1;void*_tmp268;struct Cyc_Absyn_Tqual _tmp269;void*_tmp26A;union Cyc_Absyn_Constraint*_tmp26B;union Cyc_Absyn_Constraint*_tmp26C;union Cyc_Absyn_Constraint*_tmp26D;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp266;if(_tmp267->tag != 5)goto _LL14A;else{_tmp268=(_tmp267->f1).elt_typ;_tmp269=(_tmp267->f1).elt_tq;_tmp26A=((_tmp267->f1).ptr_atts).rgn;_tmp26B=((_tmp267->f1).ptr_atts).nullable;_tmp26C=((_tmp267->f1).ptr_atts).bounds;_tmp26D=((_tmp267->f1).ptr_atts).zero_term;}}_LL149:
# 1224
{void*_tmp26E=Cyc_Tcutil_compress(_tmp268);struct Cyc_List_List*_tmp270;void*_tmp271;struct Cyc_Absyn_Tqual _tmp272;void*_tmp273;struct Cyc_List_List*_tmp274;int _tmp275;struct Cyc_Absyn_VarargInfo*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp26F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26E;if(_tmp26F->tag != 9)goto _LL14F;else{_tmp270=(_tmp26F->f1).tvars;_tmp271=(_tmp26F->f1).effect;_tmp272=(_tmp26F->f1).ret_tqual;_tmp273=(_tmp26F->f1).ret_typ;_tmp274=(_tmp26F->f1).args;_tmp275=(_tmp26F->f1).c_varargs;_tmp276=(_tmp26F->f1).cyc_varargs;_tmp277=(_tmp26F->f1).rgn_po;_tmp278=(_tmp26F->f1).attributes;}}_LL14E: {
# 1226
struct _RegionHandle _tmp279=_new_region("temp");struct _RegionHandle*temp=& _tmp279;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1229
for(0;ts != 0  && _tmp270 != 0;(ts=ts->tl,_tmp270=_tmp270->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp270->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp7CC;struct Cyc_List_List*_tmp7CB;instantiation=(
(_tmp7CB=_region_malloc(temp,sizeof(*_tmp7CB)),((_tmp7CB->hd=((_tmp7CC=_region_malloc(temp,sizeof(*_tmp7CC)),((_tmp7CC->f1=(struct Cyc_Absyn_Tvar*)_tmp270->hd,((_tmp7CC->f2=(void*)ts->hd,_tmp7CC)))))),((_tmp7CB->tl=instantiation,_tmp7CB))))));};}
# 1236
if(ts != 0){
const char*_tmp7CF;void*_tmp7CE;void*_tmp27E=
(_tmp7CE=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7CF="too many type variables in instantiation",_tag_dyneither(_tmp7CF,sizeof(char),41))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));_npop_handler(0);return _tmp27E;}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7D5;struct Cyc_Absyn_FnInfo _tmp7D4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7D3;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp7D3=_cycalloc(sizeof(*_tmp7D3)),((_tmp7D3[0]=((_tmp7D5.tag=9,((_tmp7D5.f1=((_tmp7D4.tvars=_tmp270,((_tmp7D4.effect=_tmp271,((_tmp7D4.ret_tqual=_tmp272,((_tmp7D4.ret_typ=_tmp273,((_tmp7D4.args=_tmp274,((_tmp7D4.c_varargs=_tmp275,((_tmp7D4.cyc_varargs=_tmp276,((_tmp7D4.rgn_po=_tmp277,((_tmp7D4.attributes=_tmp278,_tmp7D4)))))))))))))))))),_tmp7D5)))),_tmp7D3)))));
# 1243
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7DB;struct Cyc_Absyn_PtrInfo _tmp7DA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7D9;void*_tmp282=(void*)((_tmp7D9=_cycalloc(sizeof(*_tmp7D9)),((_tmp7D9[0]=((_tmp7DB.tag=5,((_tmp7DB.f1=((_tmp7DA.elt_typ=new_fn_typ,((_tmp7DA.elt_tq=_tmp269,((_tmp7DA.ptr_atts=(((_tmp7DA.ptr_atts).rgn=_tmp26A,(((_tmp7DA.ptr_atts).nullable=_tmp26B,(((_tmp7DA.ptr_atts).bounds=_tmp26C,(((_tmp7DA.ptr_atts).zero_term=_tmp26D,(((_tmp7DA.ptr_atts).ptrloc=0,_tmp7DA.ptr_atts)))))))))),_tmp7DA)))))),_tmp7DB)))),_tmp7D9))));_npop_handler(0);return _tmp282;};}
# 1227
;_pop_region(temp);}_LL14F:;_LL150:
# 1245
 goto _LL14C;_LL14C:;}
# 1247
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1250
const char*_tmp7DF;void*_tmp7DE[1];struct Cyc_String_pa_PrintArg_struct _tmp7DD;return(_tmp7DD.tag=0,((_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7DE[0]=& _tmp7DD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DF="expecting polymorphic type but found %s",_tag_dyneither(_tmp7DF,sizeof(char),40))),_tag_dyneither(_tmp7DE,sizeof(void*),1)))))));};};}
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
const char*_tmp7E4;void*_tmp7E3[2];struct Cyc_String_pa_PrintArg_struct _tmp7E2;struct Cyc_String_pa_PrintArg_struct _tmp7E1;void*_tmp289=(_tmp7E1.tag=0,((_tmp7E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7E2.tag=0,((_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7E3[0]=& _tmp7E2,((_tmp7E3[1]=& _tmp7E1,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp7E4="cannot cast %s to %s",_tag_dyneither(_tmp7E4,sizeof(char),21))),_tag_dyneither(_tmp7E3,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp289;};}}}
# 1282
{struct _tuple0 _tmp7E5;struct _tuple0 _tmp28F=(_tmp7E5.f1=e->r,((_tmp7E5.f2=Cyc_Tcutil_compress(t),_tmp7E5)));int _tmp291;union Cyc_Absyn_Constraint*_tmp293;union Cyc_Absyn_Constraint*_tmp294;union Cyc_Absyn_Constraint*_tmp295;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp290=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp28F.f1;if(_tmp290->tag != 32)goto _LL154;else{_tmp291=(_tmp290->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp292=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28F.f2;if(_tmp292->tag != 5)goto _LL154;else{_tmp293=((_tmp292->f1).ptr_atts).nullable;_tmp294=((_tmp292->f1).ptr_atts).bounds;_tmp295=((_tmp292->f1).ptr_atts).zero_term;}};_LL153:
# 1286
 if((_tmp291  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp295)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp293)){
void*_tmp296=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp294);struct Cyc_Absyn_Exp*_tmp298;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp296;if(_tmp297->tag != 1)goto _LL159;else{_tmp298=_tmp297->f1;}}_LL158:
# 1289
 if((Cyc_Evexp_eval_const_uint_exp(_tmp298)).f1 == 1){
const char*_tmp7E8;void*_tmp7E7;(_tmp7E7=0,Cyc_Tcutil_warn(loc,((_tmp7E8="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp7E8,sizeof(char),60))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}
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
void**_tmp29B=0;
struct Cyc_Absyn_Tqual _tmp29C=Cyc_Absyn_empty_tqual(0);
int _tmp29D=0;
if(topt != 0){
void*_tmp29E=Cyc_Tcutil_compress(*topt);void**_tmp2A0;struct Cyc_Absyn_Tqual _tmp2A1;union Cyc_Absyn_Constraint*_tmp2A2;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29E;if(_tmp29F->tag != 5)goto _LL15E;else{_tmp2A0=(void**)&(_tmp29F->f1).elt_typ;_tmp2A1=(_tmp29F->f1).elt_tq;_tmp2A2=((_tmp29F->f1).ptr_atts).nullable;}}_LL15D:
# 1310
 _tmp29B=_tmp2A0;
_tmp29C=_tmp2A1;
_tmp29D=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2A2);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1324
struct _RegionHandle _tmp2A3=_new_region("r");struct _RegionHandle*r=& _tmp2A3;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp29B,e);
# 1327
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp7EB;void*_tmp7EA;(_tmp7EA=0,Cyc_Tcutil_terr(e->loc,((_tmp7EB="Cannot take the address of an alias-free path",_tag_dyneither(_tmp7EB,sizeof(char),46))),_tag_dyneither(_tmp7EA,sizeof(void*),0)));}
# 1334
{void*_tmp2A6=e->r;struct Cyc_Absyn_Exp*_tmp2A8;struct Cyc_Absyn_Exp*_tmp2A9;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2A7=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A6;if(_tmp2A7->tag != 22)goto _LL163;else{_tmp2A8=_tmp2A7->f1;_tmp2A9=_tmp2A7->f2;}}_LL162:
# 1336
{void*_tmp2AA=Cyc_Tcutil_compress((void*)_check_null(_tmp2A8->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2AB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AA;if(_tmp2AB->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1344
 e0->r=(Cyc_Absyn_add_exp(_tmp2A8,_tmp2A9,0))->r;{
void*_tmp2AC=Cyc_Tcexp_tcPlus(te,_tmp2A8,_tmp2A9);_npop_handler(0);return _tmp2AC;};_LL165:;}
# 1347
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1352
{void*_tmp2AD=e->r;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2AD;if(_tmp2AE->tag != 20)goto _LL16D;else{if(_tmp2AE->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2AF=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2AD;if(_tmp2AF->tag != 21)goto _LL16F;else{if(_tmp2AF->f3 != 1)goto _LL16F;}}_LL16E:
# 1355
{const char*_tmp7EE;void*_tmp7ED;(_tmp7ED=0,Cyc_Tcutil_terr(e->loc,((_tmp7EE="cannot take the address of a @tagged union member",_tag_dyneither(_tmp7EE,sizeof(char),50))),_tag_dyneither(_tmp7ED,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1361
int _tmp2B3;void*_tmp2B4;struct _tuple14 _tmp2B2=Cyc_Tcutil_addressof_props(te,e);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B4,0)){
const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_Tcutil_terr(e->loc,((_tmp7F1="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp7F1,sizeof(char),60))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}{
# 1365
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2B3){
tq.print_const=1;
tq.real_const=1;}{
# 1371
void*t=_tmp29D?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2B4,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2B4,tq,Cyc_Absyn_false_conref);
void*_tmp2B7=t;_npop_handler(0);return _tmp2B7;};};};
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
const char*_tmp7F5;void*_tmp7F4[1];struct Cyc_String_pa_PrintArg_struct _tmp7F3;(_tmp7F3.tag=0,((_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7F4[0]=& _tmp7F3,Cyc_Tcutil_terr(loc,((_tmp7F5="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp7F5,sizeof(char),55))),_tag_dyneither(_tmp7F4,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2BB=Cyc_Tcutil_compress(*topt);void*_tmp2BC;void*_tmp2BE;_LL172: _tmp2BC=_tmp2BB;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2BD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2BC;if(_tmp2BD->tag != 19)goto _LL174;else{_tmp2BE=(void*)_tmp2BD->f1;}};_LL173: {
# 1393
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp7F8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C0=(_tmp7F7=_cycalloc(sizeof(*_tmp7F7)),((_tmp7F7[0]=((_tmp7F8.tag=18,((_tmp7F8.f1=_tmp2BF,_tmp7F8)))),_tmp7F7)));
if(Cyc_Tcutil_unify(_tmp2BE,(void*)_tmp2C0))return _tmp2BC;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1399
return Cyc_Absyn_uint_typ;}
# 1402
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1409
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1411
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp2C3=fs;
void*_tmp2C4=t_orig;
for(0;_tmp2C3 != 0;_tmp2C3=_tmp2C3->tl){
void*_tmp2C5=(void*)_tmp2C3->hd;
void*_tmp2C6=_tmp2C5;struct _dyneither_ptr*_tmp2C8;unsigned int _tmp2CA;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2C7=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2C6;if(_tmp2C7->tag != 0)goto _LL179;else{_tmp2C8=_tmp2C7->f1;}}_LL178: {
# 1419
int bad_type=1;
{void*_tmp2CB=Cyc_Tcutil_compress(_tmp2C4);struct Cyc_Absyn_Aggrdecl**_tmp2CD;struct Cyc_List_List*_tmp2CF;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2CC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CB;if(_tmp2CC->tag != 11)goto _LL17E;else{if((((_tmp2CC->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2CD=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2CC->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1422
 if((*_tmp2CD)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2C8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2CD)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp7FC;void*_tmp7FB[1];struct Cyc_String_pa_PrintArg_struct _tmp7FA;(_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2C8),((_tmp7FB[0]=& _tmp7FA,Cyc_Tcutil_terr(loc,((_tmp7FC="no field of struct/union has name %s",_tag_dyneither(_tmp7FC,sizeof(char),37))),_tag_dyneither(_tmp7FB,sizeof(void*),1)))))));}else{
# 1427
_tmp2C4=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2CE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CB;if(_tmp2CE->tag != 12)goto _LL180;else{_tmp2CF=_tmp2CE->f2;}}_LL17F: {
# 1431
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2C8,_tmp2CF);
if(!((unsigned int)t2)){
const char*_tmp800;void*_tmp7FF[1];struct Cyc_String_pa_PrintArg_struct _tmp7FE;(_tmp7FE.tag=0,((_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2C8),((_tmp7FF[0]=& _tmp7FE,Cyc_Tcutil_terr(loc,((_tmp800="no field of struct/union has name %s",_tag_dyneither(_tmp800,sizeof(char),37))),_tag_dyneither(_tmp7FF,sizeof(void*),1)))))));}else{
# 1435
_tmp2C4=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1440
if(bad_type){
if(_tmp2C3 == fs){
const char*_tmp804;void*_tmp803[1];struct Cyc_String_pa_PrintArg_struct _tmp802;(_tmp802.tag=0,((_tmp802.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C4)),((_tmp803[0]=& _tmp802,Cyc_Tcutil_terr(loc,((_tmp804="%s is not a known struct/union type",_tag_dyneither(_tmp804,sizeof(char),36))),_tag_dyneither(_tmp803,sizeof(void*),1)))))));}else{
# 1444
const char*_tmp808;void*_tmp807[1];struct Cyc_String_pa_PrintArg_struct _tmp806;struct _dyneither_ptr _tmp2D9=(_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp807[0]=& _tmp806,Cyc_aprintf(((_tmp808="(%s)",_tag_dyneither(_tmp808,sizeof(char),5))),_tag_dyneither(_tmp807,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2C3;x=x->tl){
void*_tmp2DA=(void*)((struct Cyc_List_List*)_check_null(x))->hd;struct _dyneither_ptr*_tmp2DC;unsigned int _tmp2DE;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2DB=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2DA;if(_tmp2DB->tag != 0)goto _LL185;else{_tmp2DC=_tmp2DB->f1;}}_LL184:
# 1449
{const char*_tmp80D;void*_tmp80C[2];struct Cyc_String_pa_PrintArg_struct _tmp80B;struct Cyc_String_pa_PrintArg_struct _tmp80A;_tmp2D9=((_tmp80A.tag=0,((_tmp80A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DC),((_tmp80B.tag=0,((_tmp80B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D9),((_tmp80C[0]=& _tmp80B,((_tmp80C[1]=& _tmp80A,Cyc_aprintf(((_tmp80D="%s.%s",_tag_dyneither(_tmp80D,sizeof(char),6))),_tag_dyneither(_tmp80C,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2DD=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2DA;if(_tmp2DD->tag != 1)goto _LL182;else{_tmp2DE=_tmp2DD->f1;}}_LL186:
# 1451
{const char*_tmp812;void*_tmp811[2];struct Cyc_String_pa_PrintArg_struct _tmp810;struct Cyc_Int_pa_PrintArg_struct _tmp80F;_tmp2D9=((_tmp80F.tag=1,((_tmp80F.f1=(unsigned long)((int)_tmp2DE),((_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D9),((_tmp811[0]=& _tmp810,((_tmp811[1]=& _tmp80F,Cyc_aprintf(((_tmp812="%s.%d",_tag_dyneither(_tmp812,sizeof(char),6))),_tag_dyneither(_tmp811,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1453
const char*_tmp817;void*_tmp816[2];struct Cyc_String_pa_PrintArg_struct _tmp815;struct Cyc_String_pa_PrintArg_struct _tmp814;(_tmp814.tag=0,((_tmp814.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C4)),((_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D9),((_tmp816[0]=& _tmp815,((_tmp816[1]=& _tmp814,Cyc_Tcutil_terr(loc,((_tmp817="%s == %s is not a struct/union type",_tag_dyneither(_tmp817,sizeof(char),36))),_tag_dyneither(_tmp816,sizeof(void*),2)))))))))))));};}}
# 1456
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2C9=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2C6;if(_tmp2C9->tag != 1)goto _LL176;else{_tmp2CA=_tmp2C9->f1;}}_LL17A: {
# 1458
int bad_type=1;
{void*_tmp2EE=Cyc_Tcutil_compress(_tmp2C4);struct Cyc_Absyn_Aggrdecl**_tmp2F0;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F4;struct Cyc_Absyn_Datatypefield*_tmp2F6;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EE;if(_tmp2EF->tag != 11)goto _LL18A;else{if((((_tmp2EF->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp2F0=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2EF->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1461
 if((*_tmp2F0)->impl == 0)
goto _LL187;
_tmp2F2=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F0)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EE;if(_tmp2F1->tag != 12)goto _LL18C;else{_tmp2F2=_tmp2F1->f2;}}_LL18B:
# 1465
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F2)<= _tmp2CA){
const char*_tmp81C;void*_tmp81B[2];struct Cyc_Int_pa_PrintArg_struct _tmp81A;struct Cyc_Int_pa_PrintArg_struct _tmp819;(_tmp819.tag=1,((_tmp819.f1=(unsigned long)((int)_tmp2CA),((_tmp81A.tag=1,((_tmp81A.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F2),((_tmp81B[0]=& _tmp81A,((_tmp81B[1]=& _tmp819,Cyc_Tcutil_terr(loc,((_tmp81C="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp81C,sizeof(char),46))),_tag_dyneither(_tmp81B,sizeof(void*),2)))))))))))));}else{
# 1469
_tmp2C4=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F2,(int)_tmp2CA))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2EE;if(_tmp2F3->tag != 10)goto _LL18E;else{_tmp2F4=_tmp2F3->f1;}}_LL18D:
# 1473
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F4)<= _tmp2CA){
const char*_tmp821;void*_tmp820[2];struct Cyc_Int_pa_PrintArg_struct _tmp81F;struct Cyc_Int_pa_PrintArg_struct _tmp81E;(_tmp81E.tag=1,((_tmp81E.f1=(unsigned long)((int)_tmp2CA),((_tmp81F.tag=1,((_tmp81F.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F4),((_tmp820[0]=& _tmp81F,((_tmp820[1]=& _tmp81E,Cyc_Tcutil_terr(loc,((_tmp821="tuple has too few components: %d <= %d",_tag_dyneither(_tmp821,sizeof(char),39))),_tag_dyneither(_tmp820,sizeof(void*),2)))))))))))));}else{
# 1477
_tmp2C4=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F4,(int)_tmp2CA)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2EE;if(_tmp2F5->tag != 4)goto _LL190;else{if((((_tmp2F5->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp2F6=((struct _tuple2)(((_tmp2F5->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1481
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6->typs)< _tmp2CA){
const char*_tmp826;void*_tmp825[2];struct Cyc_Int_pa_PrintArg_struct _tmp824;struct Cyc_Int_pa_PrintArg_struct _tmp823;(_tmp823.tag=1,((_tmp823.f1=(unsigned long)((int)_tmp2CA),((_tmp824.tag=1,((_tmp824.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6->typs),((_tmp825[0]=& _tmp824,((_tmp825[1]=& _tmp823,Cyc_Tcutil_terr(loc,((_tmp826="datatype field has too few components: %d < %d",_tag_dyneither(_tmp826,sizeof(char),47))),_tag_dyneither(_tmp825,sizeof(void*),2)))))))))))));}else{
# 1485
if(_tmp2CA != 0)
_tmp2C4=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F6->typs,(int)(_tmp2CA - 1))).f2;else{
if(_tmp2C3->tl != 0){
const char*_tmp829;void*_tmp828;(_tmp828=0,Cyc_Tcutil_terr(loc,((_tmp829="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp829,sizeof(char),68))),_tag_dyneither(_tmp828,sizeof(void*),0)));}}}
# 1490
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1494
if(bad_type){
const char*_tmp82D;void*_tmp82C[1];struct Cyc_String_pa_PrintArg_struct _tmp82B;(_tmp82B.tag=0,((_tmp82B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C4)),((_tmp82C[0]=& _tmp82B,Cyc_Tcutil_terr(loc,((_tmp82D="%s is not a known type",_tag_dyneither(_tmp82D,sizeof(char),23))),_tag_dyneither(_tmp82C,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1499
return Cyc_Absyn_uint_typ;};}
# 1503
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp308=_new_region("r");struct _RegionHandle*r=& _tmp308;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp309=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp309,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp30A=t;void*_tmp30D;void*_tmp30E;union Cyc_Absyn_Constraint*_tmp30F;union Cyc_Absyn_Constraint*_tmp310;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp30B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30A;if(_tmp30B->tag != 1)goto _LL195;}_LL194: {
# 1510
struct Cyc_List_List*_tmp311=Cyc_Tcenv_lookup_type_vars(_tmp309);
struct Cyc_Core_Opt*_tmp82E;void*_tmp312=Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp82E=_cycalloc(sizeof(*_tmp82E)),((_tmp82E->v=_tmp311,_tmp82E)))));
struct Cyc_Core_Opt*_tmp82F;void*_tmp313=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp82F=_cycalloc(sizeof(*_tmp82F)),((_tmp82F->v=_tmp311,_tmp82F)))));
union Cyc_Absyn_Constraint*_tmp314=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp315=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp830;struct Cyc_Absyn_PtrAtts _tmp316=(_tmp830.rgn=_tmp313,((_tmp830.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp830.bounds=_tmp314,((_tmp830.zero_term=_tmp315,((_tmp830.ptrloc=0,_tmp830)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp836;struct Cyc_Absyn_PtrInfo _tmp835;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp834;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp317=(_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834[0]=((_tmp836.tag=5,((_tmp836.f1=((_tmp835.elt_typ=_tmp312,((_tmp835.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp835.ptr_atts=_tmp316,_tmp835)))))),_tmp836)))),_tmp834)));
Cyc_Tcutil_unify(t,(void*)_tmp317);
_tmp30D=_tmp312;_tmp30E=_tmp313;_tmp30F=_tmp314;_tmp310=_tmp315;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp30C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A;if(_tmp30C->tag != 5)goto _LL197;else{_tmp30D=(_tmp30C->f1).elt_typ;_tmp30E=((_tmp30C->f1).ptr_atts).rgn;_tmp30F=((_tmp30C->f1).ptr_atts).bounds;_tmp310=((_tmp30C->f1).ptr_atts).zero_term;}}_LL196:
# 1520
 Cyc_Tcenv_check_rgn_accessible(_tmp309,loc,_tmp30E);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp30F);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp30D),& Cyc_Tcutil_tmk)){
const char*_tmp839;void*_tmp838;(_tmp838=0,Cyc_Tcutil_terr(loc,((_tmp839="can't dereference abstract pointer type",_tag_dyneither(_tmp839,sizeof(char),40))),_tag_dyneither(_tmp838,sizeof(void*),0)));}{
void*_tmp320=_tmp30D;_npop_handler(0);return _tmp320;};_LL197:;_LL198: {
# 1526
const char*_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_PrintArg_struct _tmp83B;void*_tmp324=(_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp83C[0]=& _tmp83B,Cyc_Tcexp_expr_err(_tmp309,loc,topt,((_tmp83D="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp83D,sizeof(char),39))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))));_npop_handler(0);return _tmp324;}_LL192:;};}
# 1505
;_pop_region(r);}
# 1532
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1536
struct _RegionHandle _tmp325=_new_region("r");struct _RegionHandle*r=& _tmp325;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1540
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp326=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Aggrdecl*_tmp328;struct Cyc_List_List*_tmp329;enum Cyc_Absyn_AggrKind _tmp32B;struct Cyc_List_List*_tmp32C;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp326;if(_tmp327->tag != 11)goto _LL19C;else{if((((_tmp327->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp328=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp327->f1).aggr_info).KnownAggr).val);_tmp329=(_tmp327->f1).targs;}}_LL19B: {
# 1543
struct Cyc_Absyn_Aggrfield*_tmp331=Cyc_Absyn_lookup_decl_field(_tmp328,f);
if(_tmp331 == 0){
const char*_tmp842;void*_tmp841[2];struct Cyc_String_pa_PrintArg_struct _tmp840;struct Cyc_String_pa_PrintArg_struct _tmp83F;void*_tmp336=(_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp328->name)),((_tmp841[0]=& _tmp840,((_tmp841[1]=& _tmp83F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp842="type %s has no %s field",_tag_dyneither(_tmp842,sizeof(char),24))),_tag_dyneither(_tmp841,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp336;}
# 1548
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp328->impl))->tagged)*is_tagged=1;{
void*t2=_tmp331->type;
if(_tmp329 != 0){
struct _RegionHandle _tmp337=_new_region("rgn");struct _RegionHandle*rgn=& _tmp337;_push_region(rgn);
{struct Cyc_List_List*_tmp338=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp328->tvs,_tmp329);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp338,_tmp331->type);}
# 1552
;_pop_region(rgn);}
# 1557
if(((_tmp328->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp328->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
const char*_tmp846;void*_tmp845[1];struct Cyc_String_pa_PrintArg_struct _tmp844;void*_tmp33C=(_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp845[0]=& _tmp844,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp846="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp846,sizeof(char),56))),_tag_dyneither(_tmp845,sizeof(void*),1)))))));_npop_handler(0);return _tmp33C;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp328->impl))->exist_vars != 0){
# 1562
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp84A;void*_tmp849[1];struct Cyc_String_pa_PrintArg_struct _tmp848;void*_tmp340=(_tmp848.tag=0,((_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp849[0]=& _tmp848,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84A="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp84A,sizeof(char),81))),_tag_dyneither(_tmp849,sizeof(void*),1)))))));_npop_handler(0);return _tmp340;}}{
# 1565
void*_tmp341=t2;_npop_handler(0);return _tmp341;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326;if(_tmp32A->tag != 12)goto _LL19E;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;}}_LL19D: {
# 1567
struct Cyc_Absyn_Aggrfield*_tmp342=Cyc_Absyn_lookup_field(_tmp32C,f);
if(_tmp342 == 0){
const char*_tmp84E;void*_tmp84D[1];struct Cyc_String_pa_PrintArg_struct _tmp84C;void*_tmp346=(_tmp84C.tag=0,((_tmp84C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp84D[0]=& _tmp84C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84E="type has no %s field",_tag_dyneither(_tmp84E,sizeof(char),21))),_tag_dyneither(_tmp84D,sizeof(void*),1)))))));_npop_handler(0);return _tmp346;}
if((_tmp32B == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp342->type)){
const char*_tmp852;void*_tmp851[1];struct Cyc_String_pa_PrintArg_struct _tmp850;void*_tmp34A=(_tmp850.tag=0,((_tmp850.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp851[0]=& _tmp850,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp852="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp852,sizeof(char),56))),_tag_dyneither(_tmp851,sizeof(void*),1)))))));_npop_handler(0);return _tmp34A;}{
void*_tmp34B=_tmp342->type;_npop_handler(0);return _tmp34B;};}_LL19E:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326;if(_tmp32D->tag != 8)goto _LL1A0;}{const char*_tmp853;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp853="size",_tag_dyneither(_tmp853,sizeof(char),5))))== 0))goto _LL1A0;};_LL19F: goto _LL1A1;_LL1A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326;if(_tmp32F->tag != 5)goto _LL1A2;}{const char*_tmp854;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp854="size",_tag_dyneither(_tmp854,sizeof(char),5))))== 0))goto _LL1A2;};_LL1A1:
{const char*_tmp858;void*_tmp857[1];struct Cyc_String_pa_PrintArg_struct _tmp856;(_tmp856.tag=0,((_tmp856.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp857[0]=& _tmp856,Cyc_Tcutil_warn(e->loc,((_tmp858="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp858,sizeof(char),54))),_tag_dyneither(_tmp857,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp85E;struct Cyc_List_List*_tmp85D;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp85C;outer_e->r=(void*)((_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85E.tag=2,((_tmp85E.f1=Cyc_Absyn_Numelts,((_tmp85E.f2=((_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D->hd=e,((_tmp85D->tl=0,_tmp85D)))))),_tmp85E)))))),_tmp85C))));}{
void*_tmp352=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp352;};_LL1A2:;_LL1A3: {
# 1579
const char*_tmp85F;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp85F="size",_tag_dyneither(_tmp85F,sizeof(char),5))))== 0){
const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_PrintArg_struct _tmp861;void*_tmp357=(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1582
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp862[0]=& _tmp861,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp863="expecting struct, union, or array, found %s",_tag_dyneither(_tmp863,sizeof(char),44))),_tag_dyneither(_tmp862,sizeof(void*),1)))))));_npop_handler(0);return _tmp357;}else{
# 1584
const char*_tmp867;void*_tmp866[1];struct Cyc_String_pa_PrintArg_struct _tmp865;void*_tmp35B=(_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp866[0]=& _tmp865,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp867="expecting struct or union, found %s",_tag_dyneither(_tmp867,sizeof(char),36))),_tag_dyneither(_tmp866,sizeof(void*),1)))))));_npop_handler(0);return _tmp35B;}}_LL199:;};
# 1536
;_pop_region(r);}
# 1590
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1593
struct _RegionHandle _tmp35C=_new_region("r");struct _RegionHandle*r=& _tmp35C;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1597
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp35D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp35F;void*_tmp360;union Cyc_Absyn_Constraint*_tmp361;union Cyc_Absyn_Constraint*_tmp362;_LL1A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 5)goto _LL1A7;else{_tmp35F=(_tmp35E->f1).elt_typ;_tmp360=((_tmp35E->f1).ptr_atts).rgn;_tmp361=((_tmp35E->f1).ptr_atts).bounds;_tmp362=((_tmp35E->f1).ptr_atts).zero_term;}}_LL1A6:
# 1600
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp361);
{void*_tmp363=Cyc_Tcutil_compress(_tmp35F);struct Cyc_Absyn_Aggrdecl*_tmp365;struct Cyc_List_List*_tmp366;enum Cyc_Absyn_AggrKind _tmp368;struct Cyc_List_List*_tmp369;_LL1AA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp363;if(_tmp364->tag != 11)goto _LL1AC;else{if((((_tmp364->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AC;_tmp365=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp364->f1).aggr_info).KnownAggr).val);_tmp366=(_tmp364->f1).targs;}}_LL1AB: {
# 1603
struct Cyc_Absyn_Aggrfield*_tmp36A=Cyc_Absyn_lookup_decl_field(_tmp365,f);
if(_tmp36A == 0){
const char*_tmp86C;void*_tmp86B[2];struct Cyc_String_pa_PrintArg_struct _tmp86A;struct Cyc_String_pa_PrintArg_struct _tmp869;void*_tmp36F=(_tmp869.tag=0,((_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp86A.tag=0,((_tmp86A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp365->name)),((_tmp86B[0]=& _tmp86A,((_tmp86B[1]=& _tmp869,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86C="type %s has no %s field",_tag_dyneither(_tmp86C,sizeof(char),24))),_tag_dyneither(_tmp86B,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp36F;}
# 1608
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp365->impl))->tagged)*is_tagged=1;{
void*t3=_tmp36A->type;
if(_tmp366 != 0){
struct _RegionHandle _tmp370=_new_region("rgn");struct _RegionHandle*rgn=& _tmp370;_push_region(rgn);
{struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp365->tvs,_tmp366);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp371,_tmp36A->type);}
# 1612
;_pop_region(rgn);}{
# 1616
struct Cyc_Absyn_Kind*_tmp372=Cyc_Tcutil_typ_kind(t3);
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp372)){
# 1619
void*_tmp373=Cyc_Tcutil_compress(t3);_LL1B1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp373;if(_tmp374->tag != 8)goto _LL1B3;}_LL1B2:
 goto _LL1B0;_LL1B3:;_LL1B4: {
# 1622
const char*_tmp870;void*_tmp86F[1];struct Cyc_String_pa_PrintArg_struct _tmp86E;void*_tmp378=(_tmp86E.tag=0,((_tmp86E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp86F[0]=& _tmp86E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp870="cannot get member %s since its type is abstract",_tag_dyneither(_tmp870,sizeof(char),48))),_tag_dyneither(_tmp86F,sizeof(void*),1)))))));_npop_handler(0);return _tmp378;}_LL1B0:;}
# 1625
if(((_tmp365->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp365->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
const char*_tmp874;void*_tmp873[1];struct Cyc_String_pa_PrintArg_struct _tmp872;void*_tmp37C=(_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp873[0]=& _tmp872,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp874="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp874,sizeof(char),56))),_tag_dyneither(_tmp873,sizeof(void*),1)))))));_npop_handler(0);return _tmp37C;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp365->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;void*_tmp380=(_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp877[0]=& _tmp876,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp878="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp878,sizeof(char),72))),_tag_dyneither(_tmp877,sizeof(void*),1)))))));_npop_handler(0);return _tmp380;}}{
# 1633
void*_tmp381=t3;_npop_handler(0);return _tmp381;};};};}_LL1AC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp363;if(_tmp367->tag != 12)goto _LL1AE;else{_tmp368=_tmp367->f1;_tmp369=_tmp367->f2;}}_LL1AD: {
# 1635
struct Cyc_Absyn_Aggrfield*_tmp382=Cyc_Absyn_lookup_field(_tmp369,f);
if(_tmp382 == 0){
const char*_tmp87C;void*_tmp87B[1];struct Cyc_String_pa_PrintArg_struct _tmp87A;void*_tmp386=(_tmp87A.tag=0,((_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87B[0]=& _tmp87A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87C="type has no %s field",_tag_dyneither(_tmp87C,sizeof(char),21))),_tag_dyneither(_tmp87B,sizeof(void*),1)))))));_npop_handler(0);return _tmp386;}
if((_tmp368 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp382->type)){
const char*_tmp880;void*_tmp87F[1];struct Cyc_String_pa_PrintArg_struct _tmp87E;void*_tmp38A=(_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87F[0]=& _tmp87E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp880="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp880,sizeof(char),56))),_tag_dyneither(_tmp87F,sizeof(void*),1)))))));_npop_handler(0);return _tmp38A;}{
void*_tmp38B=_tmp382->type;_npop_handler(0);return _tmp38B;};}_LL1AE:;_LL1AF:
 goto _LL1A9;_LL1A9:;}
# 1643
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}{
# 1646
const char*_tmp884;void*_tmp883[1];struct Cyc_String_pa_PrintArg_struct _tmp882;void*_tmp38F=(_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp883[0]=& _tmp882,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp884="expecting struct or union pointer, found %s",_tag_dyneither(_tmp884,sizeof(char),44))),_tag_dyneither(_tmp883,sizeof(void*),1)))))));_npop_handler(0);return _tmp38F;};
# 1593
;_pop_region(r);}
# 1651
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1653
unsigned int _tmp391;int _tmp392;struct _tuple15 _tmp390=Cyc_Evexp_eval_const_uint_exp(index);_tmp391=_tmp390.f1;_tmp392=_tmp390.f2;
if(!_tmp392){
const char*_tmp887;void*_tmp886;return(_tmp886=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp887="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp887,sizeof(char),47))),_tag_dyneither(_tmp886,sizeof(void*),0)));}{
# 1657
struct _handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(_tmp395.handler))_tmp397=1;if(!_tmp397){
{void*_tmp398=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp391)).f2;_npop_handler(0);return _tmp398;};_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*_tmp39A=_tmp396;void*_tmp39C;_LL1B6: {struct Cyc_List_Nth_exn_struct*_tmp39B=(struct Cyc_List_Nth_exn_struct*)_tmp39A;if(_tmp39B->tag != Cyc_List_Nth)goto _LL1B8;}_LL1B7: {
# 1660
const char*_tmp88C;void*_tmp88B[2];struct Cyc_Int_pa_PrintArg_struct _tmp88A;struct Cyc_Int_pa_PrintArg_struct _tmp889;return(_tmp889.tag=1,((_tmp889.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp88A.tag=1,((_tmp88A.f1=(unsigned long)((int)_tmp391),((_tmp88B[0]=& _tmp88A,((_tmp88B[1]=& _tmp889,Cyc_Tcexp_expr_err(te,loc,0,((_tmp88C="index is %d but tuple has only %d fields",_tag_dyneither(_tmp88C,sizeof(char),41))),_tag_dyneither(_tmp88B,sizeof(void*),2)))))))))))));}_LL1B8: _tmp39C=_tmp39A;_LL1B9:(void)_throw(_tmp39C);_LL1B5:;}};};}
# 1665
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1667
struct _RegionHandle _tmp3A1=_new_region("r");struct _RegionHandle*r=& _tmp3A1;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3A2=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp3A2,0,e1);
Cyc_Tcexp_tcExp(_tmp3A2,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A2,e2)){
const char*_tmp890;void*_tmp88F[1];struct Cyc_String_pa_PrintArg_struct _tmp88E;void*_tmp3A6=(_tmp88E.tag=0,((_tmp88E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp88F[0]=& _tmp88E,Cyc_Tcexp_expr_err(_tmp3A2,e2->loc,topt,((_tmp890="expecting int subscript, found %s",_tag_dyneither(_tmp890,sizeof(char),34))),_tag_dyneither(_tmp88F,sizeof(void*),1)))))));_npop_handler(0);return _tmp3A6;}{
# 1678
void*_tmp3A7=t1;void*_tmp3A9;struct Cyc_Absyn_Tqual _tmp3AA;void*_tmp3AB;union Cyc_Absyn_Constraint*_tmp3AC;union Cyc_Absyn_Constraint*_tmp3AD;struct Cyc_List_List*_tmp3AF;_LL1BB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A7;if(_tmp3A8->tag != 5)goto _LL1BD;else{_tmp3A9=(_tmp3A8->f1).elt_typ;_tmp3AA=(_tmp3A8->f1).elt_tq;_tmp3AB=((_tmp3A8->f1).ptr_atts).rgn;_tmp3AC=((_tmp3A8->f1).ptr_atts).bounds;_tmp3AD=((_tmp3A8->f1).ptr_atts).zero_term;}}_LL1BC:
# 1682
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3AD)){
int emit_warning=1;
{void*_tmp3B0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3AC);struct Cyc_Absyn_Exp*_tmp3B2;_LL1C2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3B0;if(_tmp3B1->tag != 1)goto _LL1C4;else{_tmp3B2=_tmp3B1->f1;}}_LL1C3:
# 1686
 if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3B5;int _tmp3B6;struct _tuple15 _tmp3B4=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;
if(_tmp3B6){
unsigned int _tmp3B8;int _tmp3B9;struct _tuple15 _tmp3B7=Cyc_Evexp_eval_const_uint_exp(_tmp3B2);_tmp3B8=_tmp3B7.f1;_tmp3B9=_tmp3B7.f2;
if(_tmp3B9  && _tmp3B8 > _tmp3B5)emit_warning=0;}}
# 1693
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3B3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B0;if(_tmp3B3->tag != 0)goto _LL1C1;}_LL1C5:
 emit_warning=0;goto _LL1C1;_LL1C1:;}
# 1696
if(emit_warning){
const char*_tmp893;void*_tmp892;(_tmp892=0,Cyc_Tcutil_warn(e2->loc,((_tmp893="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp893,sizeof(char),63))),_tag_dyneither(_tmp892,sizeof(void*),0)));}}else{
# 1700
if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3BD;int _tmp3BE;struct _tuple15 _tmp3BC=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BD=_tmp3BC.f1;_tmp3BE=_tmp3BC.f2;
if(_tmp3BE)
Cyc_Tcutil_check_bound(loc,_tmp3BD,_tmp3AC);}else{
# 1706
if(Cyc_Tcutil_is_bound_one(_tmp3AC) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3AD)){
const char*_tmp896;void*_tmp895;(_tmp895=0,Cyc_Tcutil_warn(e1->loc,((_tmp896="subscript applied to pointer to one object",_tag_dyneither(_tmp896,sizeof(char),43))),_tag_dyneither(_tmp895,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3AC);}}
# 1711
Cyc_Tcenv_check_rgn_accessible(_tmp3A2,loc,_tmp3AB);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3A9),& Cyc_Tcutil_tmk)){
const char*_tmp899;void*_tmp898;(_tmp898=0,Cyc_Tcutil_terr(e1->loc,((_tmp899="can't subscript an abstract pointer",_tag_dyneither(_tmp899,sizeof(char),36))),_tag_dyneither(_tmp898,sizeof(void*),0)));}{
void*_tmp3C3=_tmp3A9;_npop_handler(0);return _tmp3C3;};_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3A7;if(_tmp3AE->tag != 10)goto _LL1BF;else{_tmp3AF=_tmp3AE->f1;}}_LL1BE: {
void*_tmp3C4=Cyc_Tcexp_ithTupleType(_tmp3A2,loc,_tmp3AF,e2);_npop_handler(0);return _tmp3C4;}_LL1BF:;_LL1C0: {
const char*_tmp89D;void*_tmp89C[1];struct Cyc_String_pa_PrintArg_struct _tmp89B;void*_tmp3C8=(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp89C[0]=& _tmp89B,Cyc_Tcexp_expr_err(_tmp3A2,loc,topt,((_tmp89D="subscript applied to %s",_tag_dyneither(_tmp89D,sizeof(char),24))),_tag_dyneither(_tmp89C,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C8;}_LL1BA:;};};}
# 1668
;_pop_region(r);}
# 1722
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp3C9=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3CB;_LL1C7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C9;if(_tmp3CA->tag != 10)goto _LL1C9;else{_tmp3CB=_tmp3CA->f1;}}_LL1C8:
# 1728
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3CB)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1731
goto _LL1C6;
# 1733
for(0;es != 0;(es=es->tl,_tmp3CB=_tmp3CB->tl)){
int bogus=0;
void*_tmp3CC=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3CB))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp3CC,(struct Cyc_Absyn_Exp*)es->hd);
# 1738
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3CB->hd)).f2,& bogus);{
struct _tuple19*_tmp8A0;struct Cyc_List_List*_tmp89F;fields=((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F->hd=((_tmp8A0=_cycalloc(sizeof(*_tmp8A0)),((_tmp8A0->f1=(*((struct _tuple19*)_tmp3CB->hd)).f1,((_tmp8A0->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8A0)))))),((_tmp89F->tl=fields,_tmp89F))))));};}
# 1741
done=1;
goto _LL1C6;_LL1C9:;_LL1CA:
 goto _LL1C6;_LL1C6:;}
# 1745
if(!done)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple19*_tmp8A3;struct Cyc_List_List*_tmp8A2;fields=((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((_tmp8A2->hd=((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((_tmp8A3->f1=Cyc_Absyn_empty_tqual(0),((_tmp8A3->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8A3)))))),((_tmp8A2->tl=fields,_tmp8A2))))));};}{
# 1750
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8A6;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8A5;return(void*)((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5[0]=((_tmp8A6.tag=10,((_tmp8A6.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8A6)))),_tmp8A5))));};}
# 1754
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1757
const char*_tmp8A9;void*_tmp8A8;return(_tmp8A8=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A9="tcCompoundLit",_tag_dyneither(_tmp8A9,sizeof(char),14))),_tag_dyneither(_tmp8A8,sizeof(void*),0)));}
# 1767 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1770
void*res_t2;
struct _RegionHandle _tmp3D5=_new_region("r");struct _RegionHandle*r=& _tmp3D5;_push_region(r);{
int _tmp3D6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8AC;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8AB;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3D7=(_tmp8AB=_cycalloc(sizeof(*_tmp8AB)),((_tmp8AB[0]=((_tmp8AC.tag=0,((_tmp8AC.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3D6),_tmp8AC)))),_tmp8AB)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3D7,loc);
# 1779
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3D8=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3D6 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3D8)){
const char*_tmp8AF;void*_tmp8AE;(_tmp8AE=0,Cyc_Tcutil_terr(_tmp3D8->loc,((_tmp8AF="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8AF,sizeof(char),45))),_tag_dyneither(_tmp8AE,sizeof(void*),0)));}}
# 1784
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1789
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1792
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1794
const char*_tmp8B3;void*_tmp8B2[1];struct Cyc_String_pa_PrintArg_struct _tmp8B1;(_tmp8B1.tag=0,((_tmp8B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8B2[0]=& _tmp8B1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8B3="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8B3,sizeof(char),53))),_tag_dyneither(_tmp8B2,sizeof(void*),1)))))));}}
# 1798
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1803
void*_tmp3E0=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp3E3;_LL1CC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E1=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E0;if(_tmp3E1->tag != 1)goto _LL1CE;}_LL1CD:
# 1805
{const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_terr(loc,((_tmp8B6="only array index designators are supported",_tag_dyneither(_tmp8B6,sizeof(char),43))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3E2=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3E0;if(_tmp3E2->tag != 0)goto _LL1CB;else{_tmp3E3=_tmp3E2->f1;}}_LL1CF:
# 1808
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3E3);{
unsigned int _tmp3E7;int _tmp3E8;struct _tuple15 _tmp3E6=Cyc_Evexp_eval_const_uint_exp(_tmp3E3);_tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;
if(!_tmp3E8){
const char*_tmp8B9;void*_tmp8B8;(_tmp8B8=0,Cyc_Tcutil_terr(_tmp3E3->loc,((_tmp8B9="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8B9,sizeof(char),47))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}else{
if(_tmp3E7 != offset){
const char*_tmp8BE;void*_tmp8BD[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BC;struct Cyc_Int_pa_PrintArg_struct _tmp8BB;(_tmp8BB.tag=1,((_tmp8BB.f1=(unsigned long)((int)_tmp3E7),((_tmp8BC.tag=1,((_tmp8BC.f1=(unsigned long)offset,((_tmp8BD[0]=& _tmp8BC,((_tmp8BD[1]=& _tmp8BB,Cyc_Tcutil_terr(_tmp3E3->loc,((_tmp8BE="expecting index designator %d but found %d",_tag_dyneither(_tmp8BE,sizeof(char),43))),_tag_dyneither(_tmp8BD,sizeof(void*),2)))))))))))));}}
# 1815
goto _LL1CB;};_LL1CB:;}}}{
# 1819
void*_tmp3EF=res_t2;_npop_handler(0);return _tmp3EF;};
# 1771
;_pop_region(r);}
# 1847 "tcexp.cyc"
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1850
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3F0=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3F2;_LL1D1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0;if(_tmp3F1->tag != 19)goto _LL1D3;else{_tmp3F2=(void*)_tmp3F1->f1;}}_LL1D2:
# 1855
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3F3=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3F2,0),0,Cyc_Absyn_No_coercion,0);
_tmp3F3->topt=bound->topt;
# 1860
bound=_tmp3F3;}
# 1862
goto _LL1D0;_LL1D3:;_LL1D4:
# 1864
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp8C2;void*_tmp8C1[1];struct Cyc_String_pa_PrintArg_struct _tmp8C0;(_tmp8C0.tag=0,((_tmp8C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8C1[0]=& _tmp8C0,Cyc_Tcutil_terr(bound->loc,((_tmp8C2="expecting unsigned int, found %s",_tag_dyneither(_tmp8C2,sizeof(char),33))),_tag_dyneither(_tmp8C1,sizeof(void*),1)))))));}_LL1D0:;}
# 1869
if(!(vd->tq).real_const){
const char*_tmp8C5;void*_tmp8C4;(_tmp8C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8C5="comprehension index variable is not declared const!",_tag_dyneither(_tmp8C5,sizeof(char),52))),_tag_dyneither(_tmp8C4,sizeof(void*),0)));}{
# 1872
struct _RegionHandle _tmp3F9=_new_region("r");struct _RegionHandle*r=& _tmp3F9;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1878
struct Cyc_Tcenv_Tenv*_tmp3FA=te2;
void**_tmp3FB=0;
struct Cyc_Absyn_Tqual*_tmp3FC=0;
union Cyc_Absyn_Constraint**_tmp3FD=0;
# 1883
if(topt != 0){
void*_tmp3FE=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp400;void*_tmp402;struct Cyc_Absyn_Tqual _tmp403;struct Cyc_Absyn_Exp*_tmp404;union Cyc_Absyn_Constraint*_tmp405;_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FE;if(_tmp3FF->tag != 5)goto _LL1D8;else{_tmp400=_tmp3FF->f1;}}_LL1D7:
# 1886
{void**_tmp8C6;_tmp3FB=((_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=_tmp400.elt_typ,_tmp8C6))));}
{struct Cyc_Absyn_Tqual*_tmp8C7;_tmp3FC=((_tmp8C7=_region_malloc(r,sizeof(*_tmp8C7)),((_tmp8C7[0]=_tmp400.elt_tq,_tmp8C7))));}
{union Cyc_Absyn_Constraint**_tmp8C8;_tmp3FD=((_tmp8C8=_region_malloc(r,sizeof(*_tmp8C8)),((_tmp8C8[0]=(_tmp400.ptr_atts).zero_term,_tmp8C8))));}
goto _LL1D5;_LL1D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3FE;if(_tmp401->tag != 8)goto _LL1DA;else{_tmp402=(_tmp401->f1).elt_type;_tmp403=(_tmp401->f1).tq;_tmp404=(_tmp401->f1).num_elts;_tmp405=(_tmp401->f1).zero_term;}}_LL1D9:
# 1891
{void**_tmp8C9;_tmp3FB=((_tmp8C9=_region_malloc(r,sizeof(*_tmp8C9)),((_tmp8C9[0]=_tmp402,_tmp8C9))));}
{struct Cyc_Absyn_Tqual*_tmp8CA;_tmp3FC=((_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA[0]=_tmp403,_tmp8CA))));}
{union Cyc_Absyn_Constraint**_tmp8CB;_tmp3FD=((_tmp8CB=_region_malloc(r,sizeof(*_tmp8CB)),((_tmp8CB[0]=_tmp405,_tmp8CB))));}
goto _LL1D5;_LL1DA:;_LL1DB:
# 1896
 goto _LL1D5;_LL1D5:;}{
# 1899
void*t=Cyc_Tcexp_tcExp(_tmp3FA,_tmp3FB,body);
if(_tmp3FA->le == 0){
# 1902
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp8CE;void*_tmp8CD;(_tmp8CD=0,Cyc_Tcutil_terr(bound->loc,((_tmp8CE="bound is not constant",_tag_dyneither(_tmp8CE,sizeof(char),22))),_tag_dyneither(_tmp8CD,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp8D1;void*_tmp8D0;(_tmp8D0=0,Cyc_Tcutil_terr(bound->loc,((_tmp8D1="body is not constant",_tag_dyneither(_tmp8D1,sizeof(char),21))),_tag_dyneither(_tmp8D0,sizeof(void*),0)));}}
# 1907
if(_tmp3FD != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3FD)){
# 1910
struct Cyc_Absyn_Exp*_tmp410=Cyc_Absyn_uint_exp(1,0);_tmp410->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp410,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1915
void*_tmp411=Cyc_Absyn_array_typ(t,_tmp3FC == 0?Cyc_Absyn_empty_tqual(0):*_tmp3FC,bound,
_tmp3FD == 0?Cyc_Absyn_false_conref:*_tmp3FD,0);
void*_tmp412=_tmp411;_npop_handler(0);return _tmp412;};};};}
# 1873
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1931 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1935
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp8D2;adptr=((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=ad,_tmp8D2))));};}else{
# 1941
{struct _handler_cons _tmp414;_push_handler(& _tmp414);{int _tmp416=0;if(setjmp(_tmp414.handler))_tmp416=1;if(!_tmp416){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1941
;_pop_handler();}else{void*_tmp415=(void*)_exn_thrown;void*_tmp418=_tmp415;void*_tmp41A;_LL1DD: {struct Cyc_Dict_Absent_exn_struct*_tmp419=(struct Cyc_Dict_Absent_exn_struct*)_tmp418;if(_tmp419->tag != Cyc_Dict_Absent)goto _LL1DF;}_LL1DE:
# 1944
{const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_PrintArg_struct _tmp8D4;(_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8D5[0]=& _tmp8D4,Cyc_Tcutil_terr(loc,((_tmp8D6="unbound struct/union name %s",_tag_dyneither(_tmp8D6,sizeof(char),29))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1DF: _tmp41A=_tmp418;_LL1E0:(void)_throw(_tmp41A);_LL1DC:;}};}
# 1947
*ad_opt=ad;
*tn=ad->name;}
# 1950
if(ad->impl == 0){
{const char*_tmp8DE;void*_tmp8DD[1];const char*_tmp8DC;const char*_tmp8DB;struct Cyc_String_pa_PrintArg_struct _tmp8DA;(_tmp8DA.tag=0,((_tmp8DA.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8DB="struct",_tag_dyneither(_tmp8DB,sizeof(char),7)):((_tmp8DC="union",_tag_dyneither(_tmp8DC,sizeof(char),6)))),((_tmp8DD[0]=& _tmp8DA,Cyc_Tcutil_terr(loc,((_tmp8DE="can't construct abstract %s",_tag_dyneither(_tmp8DE,sizeof(char),28))),_tag_dyneither(_tmp8DD,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1954
struct _RegionHandle _tmp423=_new_region("rgn");struct _RegionHandle*rgn=& _tmp423;_push_region(rgn);
# 1959
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp424=Cyc_Tcenv_new_status(te);
if(_tmp424 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 1968
te2=Cyc_Tcenv_set_new_status(rgn,_tmp424,te);}{
# 1970
struct _tuple12 _tmp8DF;struct _tuple12 _tmp425=(_tmp8DF.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp8DF.f2=rgn,_tmp8DF)));
struct Cyc_List_List*_tmp426=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp425,ad->tvs);
struct Cyc_List_List*_tmp427=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp425,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp428=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp426);
struct Cyc_List_List*_tmp429=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp427);
struct Cyc_List_List*_tmp42A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp426,_tmp427);
void*res_typ;
# 1981
if(topt != 0){
void*_tmp42B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Aggrdecl**_tmp42D;struct Cyc_List_List*_tmp42E;_LL1E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp42C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp42B;if(_tmp42C->tag != 11)goto _LL1E4;else{if((((_tmp42C->f1).aggr_info).KnownAggr).tag != 2)goto _LL1E4;_tmp42D=(struct Cyc_Absyn_Aggrdecl**)(((_tmp42C->f1).aggr_info).KnownAggr).val;_tmp42E=(_tmp42C->f1).targs;}}_LL1E3:
# 1984
 if(*_tmp42D == *adptr){
{struct Cyc_List_List*_tmp42F=_tmp428;for(0;_tmp42F != 0  && _tmp42E != 0;
(_tmp42F=_tmp42F->tl,_tmp42E=_tmp42E->tl)){
Cyc_Tcutil_unify((void*)_tmp42F->hd,(void*)_tmp42E->hd);}}
# 1989
res_typ=*topt;
goto _LL1E1;}
# 1992
goto _LL1E5;_LL1E4:;_LL1E5: {
# 1994
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8E5;struct Cyc_Absyn_AggrInfo _tmp8E4;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8E3;res_typ=(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E5.tag=11,((_tmp8E5.f1=((_tmp8E4.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8E4.targs=_tmp428,_tmp8E4)))),_tmp8E5)))),_tmp8E3))));}_LL1E1:;}else{
# 1997
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp8EB;struct Cyc_Absyn_AggrInfo _tmp8EA;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8E9;res_typ=(void*)((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EB.tag=11,((_tmp8EB.f1=((_tmp8EA.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp8EA.targs=_tmp428,_tmp8EA)))),_tmp8EB)))),_tmp8E9))));}{
# 2000
struct Cyc_List_List*_tmp436=*ts;
struct Cyc_List_List*_tmp437=_tmp429;
while(_tmp436 != 0  && _tmp437 != 0){
# 2004
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp436->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp436->hd);
Cyc_Tcutil_unify((void*)_tmp436->hd,(void*)_tmp437->hd);
_tmp436=_tmp436->tl;
_tmp437=_tmp437->tl;}
# 2010
if(_tmp436 != 0){
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_terr(loc,((_tmp8EE="too many explicit witness types",_tag_dyneither(_tmp8EE,sizeof(char),32))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}
# 2013
*ts=_tmp429;{
# 2016
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct _tuple20*_tmp43A=(struct _tuple20*)fields->hd;_tmp43B=_tmp43A->f1;_tmp43C=_tmp43A->f2;{
void*_tmp43D=Cyc_Tcutil_rsubstitute(rgn,_tmp42A,_tmp43B->type);
Cyc_Tcexp_tcExpInitializer(te2,& _tmp43D,_tmp43C);
# 2026
if(!Cyc_Tcutil_coerce_arg(te2,_tmp43C,_tmp43D,& bogus)){
{const char*_tmp8FA;void*_tmp8F9[5];struct Cyc_String_pa_PrintArg_struct _tmp8F8;const char*_tmp8F7;const char*_tmp8F6;struct Cyc_String_pa_PrintArg_struct _tmp8F5;struct Cyc_String_pa_PrintArg_struct _tmp8F4;struct Cyc_String_pa_PrintArg_struct _tmp8F3;struct Cyc_String_pa_PrintArg_struct _tmp8F2;(_tmp8F2.tag=0,((_tmp8F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2030
Cyc_Absynpp_typ2string((void*)_check_null(_tmp43C->topt))),((_tmp8F3.tag=0,((_tmp8F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2029
Cyc_Absynpp_typ2string(_tmp43D)),((_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8F5.tag=0,((_tmp8F5.f1=(struct _dyneither_ptr)(
# 2028
ad->kind == Cyc_Absyn_StructA?(_tmp8F6="struct",_tag_dyneither(_tmp8F6,sizeof(char),7)):((_tmp8F7="union",_tag_dyneither(_tmp8F7,sizeof(char),6)))),((_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp43B->name),((_tmp8F9[0]=& _tmp8F8,((_tmp8F9[1]=& _tmp8F5,((_tmp8F9[2]=& _tmp8F4,((_tmp8F9[3]=& _tmp8F3,((_tmp8F9[4]=& _tmp8F2,Cyc_Tcutil_terr(_tmp43C->loc,((_tmp8FA="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp8FA,sizeof(char),40))),_tag_dyneither(_tmp8F9,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2031
Cyc_Tcutil_explain_failure();}};}{
# 2035
struct Cyc_List_List*_tmp447=0;
{struct Cyc_List_List*_tmp448=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp448 != 0;_tmp448=_tmp448->tl){
struct _tuple0*_tmp8FD;struct Cyc_List_List*_tmp8FC;_tmp447=((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC->hd=((_tmp8FD=_cycalloc(sizeof(*_tmp8FD)),((_tmp8FD->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp42A,(*((struct _tuple0*)_tmp448->hd)).f1),((_tmp8FD->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp42A,(*((struct _tuple0*)_tmp448->hd)).f2),_tmp8FD)))))),((_tmp8FC->tl=_tmp447,_tmp8FC))))));}}
# 2040
_tmp447=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp447);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp447);{
void*_tmp44B=res_typ;_npop_handler(0);return _tmp44B;};};};};};}
# 1959
;_pop_region(rgn);};}
# 2046
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2048
struct _RegionHandle _tmp44D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp44D;_push_region(rgn);{
void*_tmp44E=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp450;struct Cyc_List_List*_tmp451;_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44E;if(_tmp44F->tag != 12)goto _LL1E9;else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}_LL1E8:
# 2051
 if(_tmp450 == Cyc_Absyn_UnionA){
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_Tcutil_terr(loc,((_tmp900="expecting struct but found union",_tag_dyneither(_tmp900,sizeof(char),33))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp451);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp455;struct Cyc_Absyn_Exp*_tmp456;struct _tuple20*_tmp454=(struct _tuple20*)fields->hd;_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp455->type,_tmp456);
if(!Cyc_Tcutil_coerce_arg(te,_tmp456,_tmp455->type,& bogus)){
{const char*_tmp906;void*_tmp905[3];struct Cyc_String_pa_PrintArg_struct _tmp904;struct Cyc_String_pa_PrintArg_struct _tmp903;struct Cyc_String_pa_PrintArg_struct _tmp902;(_tmp902.tag=0,((_tmp902.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2062
Cyc_Absynpp_typ2string((void*)_check_null(_tmp456->topt))),((_tmp903.tag=0,((_tmp903.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2061
Cyc_Absynpp_typ2string(_tmp455->type)),((_tmp904.tag=0,((_tmp904.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp455->name),((_tmp905[0]=& _tmp904,((_tmp905[1]=& _tmp903,((_tmp905[2]=& _tmp902,Cyc_Tcutil_terr(_tmp456->loc,((_tmp906="field %s of struct expects type %s != %s",_tag_dyneither(_tmp906,sizeof(char),41))),_tag_dyneither(_tmp905,sizeof(void*),3)))))))))))))))))));}
# 2063
Cyc_Tcutil_explain_failure();}}
# 2066
goto _LL1E6;};_LL1E9:;_LL1EA: {
const char*_tmp909;void*_tmp908;(_tmp908=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp909="tcAnonStruct: wrong type",_tag_dyneither(_tmp909,sizeof(char),25))),_tag_dyneither(_tmp908,sizeof(void*),0)));}_LL1E6:;}{
# 2070
void*_tmp45E=ts;_npop_handler(0);return _tmp45E;};
# 2048
;_pop_region(rgn);}
# 2074
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2077
struct _RegionHandle _tmp45F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp45F;_push_region(rgn);
{struct _tuple12 _tmp90A;struct _tuple12 _tmp460=(_tmp90A.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp90A.f2=rgn,_tmp90A)));
struct Cyc_List_List*_tmp461=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp460,tud->tvs);
struct Cyc_List_List*_tmp462=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp461);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp910;struct Cyc_Absyn_DatatypeFieldInfo _tmp90F;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp90E;void*res=(void*)(
(_tmp90E=_cycalloc(sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp910.tag=4,((_tmp910.f1=((_tmp90F.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp90F.targs=_tmp462,_tmp90F)))),_tmp910)))),_tmp90E))));
# 2085
if(topt != 0){
void*_tmp463=Cyc_Tcutil_compress(*topt);_LL1EC: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp463;if(_tmp464->tag != 4)goto _LL1EE;}_LL1ED:
 Cyc_Tcutil_unify(*topt,res);goto _LL1EB;_LL1EE:;_LL1EF:
 goto _LL1EB;_LL1EB:;}{
# 2091
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;
if(_tmp461 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp461,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp918;void*_tmp917[3];struct Cyc_String_pa_PrintArg_struct _tmp916;struct Cyc_String_pa_PrintArg_struct _tmp915;const char*_tmp914;struct Cyc_String_pa_PrintArg_struct _tmp913;(_tmp913.tag=0,((_tmp913.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2102
e->topt == 0?(struct _dyneither_ptr)((_tmp914="?",_tag_dyneither(_tmp914,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp915.tag=0,((_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2101
Cyc_Absynpp_typ2string(t)),((_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp917[0]=& _tmp916,((_tmp917[1]=& _tmp915,((_tmp917[2]=& _tmp913,Cyc_Tcutil_terr(e->loc,((_tmp918="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp918,sizeof(char),82))),_tag_dyneither(_tmp917,sizeof(void*),3)))))))))))))))))));}
# 2103
Cyc_Tcutil_explain_failure();}}
# 2106
if(es != 0){
const char*_tmp91C;void*_tmp91B[1];struct Cyc_String_pa_PrintArg_struct _tmp91A;void*_tmp46E=(_tmp91A.tag=0,((_tmp91A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2109
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp91B[0]=& _tmp91A,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp91C="too many arguments for datatype constructor %s",_tag_dyneither(_tmp91C,sizeof(char),47))),_tag_dyneither(_tmp91B,sizeof(void*),1)))))));_npop_handler(0);return _tmp46E;}
if(ts != 0){
const char*_tmp920;void*_tmp91F[1];struct Cyc_String_pa_PrintArg_struct _tmp91E;void*_tmp472=(_tmp91E.tag=0,((_tmp91E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp91F[0]=& _tmp91E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp920="too few arguments for datatype constructor %s",_tag_dyneither(_tmp920,sizeof(char),46))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))));_npop_handler(0);return _tmp472;}{
void*_tmp473=res;_npop_handler(0);return _tmp473;};};}
# 2078
;_pop_region(rgn);}
# 2117
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp478=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp47F;void*_tmp483;struct Cyc_List_List*_tmp486;union Cyc_Absyn_AggrInfoU _tmp488;struct Cyc_List_List*_tmp489;struct Cyc_List_List*_tmp48D;_LL1F1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp479=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp478;if(_tmp479->tag != 0)goto _LL1F3;}_LL1F2:
 return 1;_LL1F3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp478;if(_tmp47A->tag != 1)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp478;if(_tmp47B->tag != 2)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp478;if(_tmp47C->tag != 3)goto _LL1F9;}_LL1F8:
 goto _LL1FA;_LL1F9: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp478;if(_tmp47D->tag != 4)goto _LL1FB;}_LL1FA:
 return 0;_LL1FB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp478;if(_tmp47E->tag != 5)goto _LL1FD;else{_tmp47F=((_tmp47E->f1).ptr_atts).nullable;}}_LL1FC:
# 2125
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp47F);_LL1FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp478;if(_tmp480->tag != 6)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp478;if(_tmp481->tag != 7)goto _LL201;}_LL200:
 return 1;_LL201: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp478;if(_tmp482->tag != 8)goto _LL203;else{_tmp483=(_tmp482->f1).elt_type;}}_LL202:
 return Cyc_Tcexp_zeroable_type(_tmp483);_LL203: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp484=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp478;if(_tmp484->tag != 9)goto _LL205;}_LL204:
 return 0;_LL205: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp478;if(_tmp485->tag != 10)goto _LL207;else{_tmp486=_tmp485->f1;}}_LL206:
# 2131
 for(0;(unsigned int)_tmp486;_tmp486=_tmp486->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp486->hd)).f2))return 0;}
return 1;_LL207: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp478;if(_tmp487->tag != 11)goto _LL209;else{_tmp488=(_tmp487->f1).aggr_info;_tmp489=(_tmp487->f1).targs;}}_LL208: {
# 2135
struct Cyc_Absyn_Aggrdecl*_tmp49A=Cyc_Absyn_get_known_aggrdecl(_tmp488);
if(_tmp49A->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49A->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp49B=_new_region("r");struct _RegionHandle*r=& _tmp49B;_push_region(r);
{struct Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp49A->tvs,_tmp489);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49A->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp49C,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp49D=0;_npop_handler(0);return _tmp49D;}}}{
int _tmp49E=1;_npop_handler(0);return _tmp49E;};}
# 2139
;_pop_region(r);};}_LL209: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp478;if(_tmp48A->tag != 13)goto _LL20B;}_LL20A:
# 2144
 return 1;_LL20B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp478;if(_tmp48B->tag != 19)goto _LL20D;}_LL20C:
 return 1;_LL20D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp478;if(_tmp48C->tag != 12)goto _LL20F;else{_tmp48D=_tmp48C->f2;}}_LL20E:
# 2147
 for(0;_tmp48D != 0;_tmp48D=_tmp48D->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp48D->hd)->type))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp478;if(_tmp48E->tag != 14)goto _LL211;}_LL210:
 return 1;_LL211: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp48F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp478;if(_tmp48F->tag != 17)goto _LL213;}_LL212:
 return 0;_LL213: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp478;if(_tmp490->tag != 16)goto _LL215;}_LL214:
 return 0;_LL215: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp491=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp478;if(_tmp491->tag != 15)goto _LL217;}_LL216:
 return 0;_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp478;if(_tmp492->tag != 26)goto _LL219;}_LL218:
# 2155
 goto _LL21A;_LL219: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp478;if(_tmp493->tag != 18)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp478;if(_tmp494->tag != 20)goto _LL21D;}_LL21C:
 goto _LL21E;_LL21D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp495=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp478;if(_tmp495->tag != 21)goto _LL21F;}_LL21E:
 goto _LL220;_LL21F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp478;if(_tmp496->tag != 22)goto _LL221;}_LL220:
 goto _LL222;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp478;if(_tmp497->tag != 23)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp498=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp478;if(_tmp498->tag != 24)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp499=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp478;if(_tmp499->tag != 25)goto _LL1F0;}_LL226: {
const char*_tmp924;void*_tmp923[1];struct Cyc_String_pa_PrintArg_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp923[0]=& _tmp922,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp924="bad type `%s' in zeroable type",_tag_dyneither(_tmp924,sizeof(char),31))),_tag_dyneither(_tmp923,sizeof(void*),1)))))));}_LL1F0:;}
# 2166
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2168
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2170
if(topt != 0){
void*_tmp4A2=Cyc_Tcutil_compress(*topt);void*_tmp4A4;_LL228: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A2;if(_tmp4A3->tag != 5)goto _LL22A;else{_tmp4A4=(_tmp4A3->f1).elt_typ;}}_LL229:
# 2173
 Cyc_Tcutil_unify(_tmp4A4,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL227;_LL22A:;_LL22B:
 goto _LL227;_LL227:;}{
# 2179
const char*_tmp92D;void*_tmp92C[2];const char*_tmp92B;const char*_tmp92A;struct Cyc_String_pa_PrintArg_struct _tmp929;struct Cyc_String_pa_PrintArg_struct _tmp928;(_tmp928.tag=0,((_tmp928.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp929.tag=0,((_tmp929.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp92A="calloc",_tag_dyneither(_tmp92A,sizeof(char),7))):(struct _dyneither_ptr)((_tmp92B="malloc",_tag_dyneither(_tmp92B,sizeof(char),7))))),((_tmp92C[0]=& _tmp929,((_tmp92C[1]=& _tmp928,Cyc_Tcutil_terr(loc,((_tmp92D="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp92D,sizeof(char),60))),_tag_dyneither(_tmp92C,sizeof(void*),2)))))))))))));};}
# 2183
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2185
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL22C:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL22D:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2191
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2196
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2199
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp930;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp92F;void*expected_type=(void*)(
(_tmp92F=_cycalloc(sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp930.tag=15,((_tmp930.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp930)))),_tmp92F))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp4AB=Cyc_Tcutil_compress(handle_type);void*_tmp4AD;_LL230: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4AC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AC->tag != 15)goto _LL232;else{_tmp4AD=(void*)_tmp4AC->f1;}}_LL231:
# 2204
 rgn=_tmp4AD;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22F;_LL232:;_LL233:
# 2208
{const char*_tmp934;void*_tmp933[1];struct Cyc_String_pa_PrintArg_struct _tmp932;(_tmp932.tag=0,((_tmp932.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp933[0]=& _tmp932,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp934="expecting region_t type but found %s",_tag_dyneither(_tmp934,sizeof(char),37))),_tag_dyneither(_tmp933,sizeof(void*),1)))))));}
goto _LL22F;_LL22F:;}else{
# 2215
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2223
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,*e);{
# 2232 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp937;void*_tmp936;(_tmp936=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp937="calloc with empty type",_tag_dyneither(_tmp937,sizeof(char),23))),_tag_dyneither(_tmp936,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2244
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4B5=er;void*_tmp4B7;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*_tmp4BA;_LL235: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4B6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4B5;if(_tmp4B6->tag != 16)goto _LL237;else{_tmp4B7=(void*)_tmp4B6->f1;}}_LL236:
# 2248
 elt_type=_tmp4B7;
{void**_tmp938;*t=((_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=elt_type,_tmp938))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL234;_LL237: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B5;if(_tmp4B8->tag != 2)goto _LL239;else{if(_tmp4B8->f1 != Cyc_Absyn_Times)goto _LL239;if(_tmp4B8->f2 == 0)goto _LL239;_tmp4B9=(struct Cyc_Absyn_Exp*)(_tmp4B8->f2)->hd;if((_tmp4B8->f2)->tl == 0)goto _LL239;_tmp4BA=(struct Cyc_Absyn_Exp*)((_tmp4B8->f2)->tl)->hd;if(((_tmp4B8->f2)->tl)->tl != 0)goto _LL239;}}_LL238:
# 2255
{struct _tuple0 _tmp939;struct _tuple0 _tmp4BD=(_tmp939.f1=_tmp4B9->r,((_tmp939.f2=_tmp4BA->r,_tmp939)));void*_tmp4BF;void*_tmp4C1;_LL23C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4BE=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4BD.f1;if(_tmp4BE->tag != 16)goto _LL23E;else{_tmp4BF=(void*)_tmp4BE->f1;}}_LL23D:
# 2257
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4BF);
elt_type=_tmp4BF;
{void**_tmp93A;*t=((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A[0]=elt_type,_tmp93A))));}
num_elts=_tmp4BA;
one_elt=0;
goto _LL23B;_LL23E: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4BD.f2;if(_tmp4C0->tag != 16)goto _LL240;else{_tmp4C1=(void*)_tmp4C0->f1;}}_LL23F:
# 2264
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C1);
elt_type=_tmp4C1;
{void**_tmp93B;*t=((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B[0]=elt_type,_tmp93B))));}
num_elts=_tmp4B9;
one_elt=0;
goto _LL23B;_LL240:;_LL241:
 goto No_sizeof;_LL23B:;}
# 2272
goto _LL234;_LL239:;_LL23A:
# 2274
 No_sizeof: {
# 2277
struct Cyc_Absyn_Exp*_tmp4C4=*e;
{void*_tmp4C5=_tmp4C4->r;struct Cyc_Absyn_Exp*_tmp4C7;_LL243: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C5;if(_tmp4C6->tag != 13)goto _LL245;else{_tmp4C7=_tmp4C6->f2;}}_LL244:
 _tmp4C4=_tmp4C7;goto _LL242;_LL245:;_LL246:
 goto _LL242;_LL242:;}
# 2282
{void*_tmp4C8=Cyc_Tcutil_compress((void*)_check_null(_tmp4C4->topt));void*_tmp4CA;_LL248: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C8;if(_tmp4C9->tag != 19)goto _LL24A;else{_tmp4CA=(void*)_tmp4C9->f1;}}_LL249:
# 2284
{void*_tmp4CB=Cyc_Tcutil_compress(_tmp4CA);struct Cyc_Absyn_Exp*_tmp4CD;_LL24D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4CB;if(_tmp4CC->tag != 18)goto _LL24F;else{_tmp4CD=_tmp4CC->f1;}}_LL24E:
# 2286
 er=_tmp4CD->r;goto retry_sizeof;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}
# 2289
goto _LL247;_LL24A:;_LL24B:
 goto _LL247;_LL247:;}
# 2292
elt_type=Cyc_Absyn_char_typ;
{void**_tmp93C;*t=((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C[0]=elt_type,_tmp93C))));}
num_elts=*e;
one_elt=0;}
# 2297
goto _LL234;_LL234:;}}
# 2301
*is_fat=!one_elt;
# 2304
{void*_tmp4CF=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4D1;_LL252: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4D0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CF;if(_tmp4D0->tag != 11)goto _LL254;else{if((((_tmp4D0->f1).aggr_info).KnownAggr).tag != 2)goto _LL254;_tmp4D1=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4D0->f1).aggr_info).KnownAggr).val);}}_LL253:
# 2306
 if(_tmp4D1->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D1->impl))->exist_vars != 0){
const char*_tmp93F;void*_tmp93E;(_tmp93E=0,Cyc_Tcutil_terr(loc,((_tmp93F="malloc with existential types not yet implemented",_tag_dyneither(_tmp93F,sizeof(char),50))),_tag_dyneither(_tmp93E,sizeof(void*),0)));}
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}{
# 2313
void*(*_tmp4D4)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4D5=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp4D6=Cyc_Tcutil_compress(*topt);union Cyc_Absyn_Constraint*_tmp4D8;union Cyc_Absyn_Constraint*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4DA;_LL257: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D6;if(_tmp4D7->tag != 5)goto _LL259;else{_tmp4D8=((_tmp4D7->f1).ptr_atts).nullable;_tmp4D9=((_tmp4D7->f1).ptr_atts).bounds;_tmp4DA=((_tmp4D7->f1).ptr_atts).zero_term;}}_LL258:
# 2318
 _tmp4D5=_tmp4DA;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4D8))
_tmp4D4=Cyc_Absyn_star_typ;
# 2323
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DA) && !(*is_calloc)){
{const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_warn(loc,((_tmp942="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp942,sizeof(char),55))),_tag_dyneither(_tmp941,sizeof(void*),0)));}
*is_calloc=1;}
# 2329
{void*_tmp4DD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4D9);struct Cyc_Absyn_Exp*_tmp4E0;_LL25C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4DE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4DD;if(_tmp4DE->tag != 0)goto _LL25E;}_LL25D:
 goto _LL25B;_LL25E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4DF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4DD;if(_tmp4DF->tag != 1)goto _LL260;else{_tmp4E0=_tmp4DF->f1;}}if(!(!one_elt))goto _LL260;_LL25F: {
# 2332
int _tmp4E1=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4E1  && Cyc_Evexp_same_const_exp(_tmp4E0,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4D9),_tmp4D5);}
# 2338
{void*_tmp4E2=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4E4;_LL263: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E2;if(_tmp4E3->tag != 19)goto _LL265;else{_tmp4E4=(void*)_tmp4E3->f1;}}_LL264: {
# 2340
struct Cyc_Absyn_Exp*_tmp4E5=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4E4,0),0,Cyc_Absyn_No_coercion,0);
# 2342
if(Cyc_Evexp_same_const_exp(_tmp4E5,_tmp4E0)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4D9),_tmp4D5);}
# 2347
goto _LL262;}_LL265:;_LL266:
 goto _LL262;_LL262:;}
# 2350
goto _LL25B;}_LL260:;_LL261:
 goto _LL25B;_LL25B:;}
# 2353
goto _LL256;_LL259:;_LL25A:
 goto _LL256;_LL256:;}
# 2356
if(!one_elt)_tmp4D4=Cyc_Absyn_dyneither_typ;
return _tmp4D4(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4D5);};};}
# 2361
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2367
struct _RegionHandle _tmp4E6=_new_region("r");struct _RegionHandle*r=& _tmp4E6;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4E7=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp4E7,0,e1);{
void*_tmp4E8=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp4E7,& _tmp4E8,e2);};}{
# 2373
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2376
{void*_tmp4E9=Cyc_Tcutil_compress(t1);_LL268: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E9;if(_tmp4EA->tag != 8)goto _LL26A;}_LL269:
{const char*_tmp945;void*_tmp944;(_tmp944=0,Cyc_Tcutil_terr(loc,((_tmp945="cannot assign to an array",_tag_dyneither(_tmp945,sizeof(char),26))),_tag_dyneither(_tmp944,sizeof(void*),0)));}goto _LL267;_LL26A:;_LL26B:
 goto _LL267;_LL267:;}{
# 2381
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp948;void*_tmp947;(_tmp947=0,Cyc_Tcutil_terr(loc,((_tmp948="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp948,sizeof(char),58))),_tag_dyneither(_tmp947,sizeof(void*),0)));}
# 2386
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp94B;void*_tmp94A;void*_tmp4F1=(_tmp94A=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp94B="swap non-lvalue",_tag_dyneither(_tmp94B,sizeof(char),16))),_tag_dyneither(_tmp94A,sizeof(void*),0)));_npop_handler(0);return _tmp4F1;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp94E;void*_tmp94D;void*_tmp4F4=(_tmp94D=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp94E="swap non-lvalue",_tag_dyneither(_tmp94E,sizeof(char),16))),_tag_dyneither(_tmp94D,sizeof(void*),0)));_npop_handler(0);return _tmp4F4;}{
# 2391
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp951;void*_tmp950;void*_tmp4F7=(_tmp950=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp951="swap value in zeroterm array",_tag_dyneither(_tmp951,sizeof(char),29))),_tag_dyneither(_tmp950,sizeof(void*),0)));_npop_handler(0);return _tmp4F7;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp954;void*_tmp953;void*_tmp4FA=(_tmp953=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp954="swap value in zeroterm array",_tag_dyneither(_tmp954,sizeof(char),29))),_tag_dyneither(_tmp953,sizeof(void*),0)));_npop_handler(0);return _tmp4FA;}
# 2398
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp959;void*_tmp958[2];struct Cyc_String_pa_PrintArg_struct _tmp957;struct Cyc_String_pa_PrintArg_struct _tmp956;void*_tmp4FB=(_tmp956.tag=0,((_tmp956.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp957.tag=0,((_tmp957.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp958[0]=& _tmp957,((_tmp958[1]=& _tmp956,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp959="type mismatch: %s != %s",_tag_dyneither(_tmp959,sizeof(char),24))),_tag_dyneither(_tmp958,sizeof(void*),2)))))))))))));
void*_tmp4FC=_tmp4FB;_npop_handler(0);return _tmp4FC;}{
# 2405
void*_tmp501=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp501;};};};};
# 2367
;_pop_region(r);}
# 2409
int Cyc_Tcexp_in_stmt_exp=0;
# 2411
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp502=_new_region("r");struct _RegionHandle*r=& _tmp502;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2418
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2420
while(1){
void*_tmp503=s->r;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Stmt*_tmp507;struct Cyc_Absyn_Stmt*_tmp508;struct Cyc_Absyn_Decl*_tmp50A;struct Cyc_Absyn_Stmt*_tmp50B;_LL26D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp504=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp503;if(_tmp504->tag != 1)goto _LL26F;else{_tmp505=_tmp504->f1;}}_LL26E: {
# 2424
void*_tmp50C=(void*)_check_null(_tmp505->topt);
if(!Cyc_Tcutil_unify(_tmp50C,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp95D;void*_tmp95C[1];struct Cyc_String_pa_PrintArg_struct _tmp95B;(_tmp95B.tag=0,((_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp50C)),((_tmp95C[0]=& _tmp95B,Cyc_Tcutil_terr(loc,((_tmp95D="statement expression returns type %s",_tag_dyneither(_tmp95D,sizeof(char),37))),_tag_dyneither(_tmp95C,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2430
void*_tmp510=_tmp50C;_npop_handler(0);return _tmp510;};}_LL26F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp506=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp503;if(_tmp506->tag != 2)goto _LL271;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}_LL270:
 s=_tmp508;continue;_LL271: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp509=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp503;if(_tmp509->tag != 12)goto _LL273;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f2;}}_LL272:
 s=_tmp50B;continue;_LL273:;_LL274: {
# 2434
const char*_tmp960;void*_tmp95F;void*_tmp513=(_tmp95F=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp960="statement expression must end with expression",_tag_dyneither(_tmp960,sizeof(char),46))),_tag_dyneither(_tmp95F,sizeof(void*),0)));_npop_handler(0);return _tmp513;}_LL26C:;}
# 2412
;_pop_region(r);}
# 2439
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
{void*_tmp514=t;struct Cyc_Absyn_Aggrdecl*_tmp516;_LL276: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp514;if(_tmp515->tag != 11)goto _LL278;else{if((((_tmp515->f1).aggr_info).KnownAggr).tag != 2)goto _LL278;_tmp516=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp515->f1).aggr_info).KnownAggr).val);}}_LL277:
# 2443
 if((_tmp516->kind == Cyc_Absyn_UnionA  && _tmp516->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp516->impl))->tagged)goto _LL275;
goto _LL279;_LL278:;_LL279:
# 2446
{const char*_tmp964;void*_tmp963[1];struct Cyc_String_pa_PrintArg_struct _tmp962;(_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp963[0]=& _tmp962,Cyc_Tcutil_terr(loc,((_tmp964="expecting @tagged union but found %s",_tag_dyneither(_tmp964,sizeof(char),37))),_tag_dyneither(_tmp963,sizeof(void*),1)))))));}
goto _LL275;_LL275:;}
# 2449
return Cyc_Absyn_uint_typ;}
# 2453
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2457
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp51A=_new_region("r");struct _RegionHandle*r=& _tmp51A;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp51B=Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te);
if(*rgn_handle != 0){
# 2462
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp967;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp966;void*expected_type=(void*)(
(_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp967.tag=15,((_tmp967.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp51B)),_tmp967)))),_tmp966))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp51B,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp51C=Cyc_Tcutil_compress(handle_type);void*_tmp51E;_LL27B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp51C;if(_tmp51D->tag != 15)goto _LL27D;else{_tmp51E=(void*)_tmp51D->f1;}}_LL27C:
# 2467
 rgn=_tmp51E;
Cyc_Tcenv_check_rgn_accessible(_tmp51B,loc,rgn);
goto _LL27A;_LL27D:;_LL27E:
# 2471
{const char*_tmp96B;void*_tmp96A[1];struct Cyc_String_pa_PrintArg_struct _tmp969;(_tmp969.tag=0,((_tmp969.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp96A[0]=& _tmp969,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp96B="expecting region_t type but found %s",_tag_dyneither(_tmp96B,sizeof(char),37))),_tag_dyneither(_tmp96A,sizeof(void*),1)))))));}
goto _LL27A;_LL27A:;}else{
# 2478
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2486
void*_tmp524=e1->r;struct Cyc_Core_Opt*_tmp527;struct Cyc_List_List*_tmp528;struct Cyc_List_List*_tmp52A;_LL280: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp524;if(_tmp525->tag != 26)goto _LL282;}_LL281: {
# 2490
void*_tmp52D=Cyc_Tcexp_tcExpNoPromote(_tmp51B,topt,e1);
void*_tmp52E=Cyc_Tcutil_compress(_tmp52D);void*_tmp530;struct Cyc_Absyn_Tqual _tmp531;struct Cyc_Absyn_Exp*_tmp532;union Cyc_Absyn_Constraint*_tmp533;_LL28D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp52F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52E;if(_tmp52F->tag != 8)goto _LL28F;else{_tmp530=(_tmp52F->f1).elt_type;_tmp531=(_tmp52F->f1).tq;_tmp532=(_tmp52F->f1).num_elts;_tmp533=(_tmp52F->f1).zero_term;}}_LL28E: {
# 2493
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp532);
void*b;
{void*_tmp534=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp536;_LL292: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp534;if(_tmp535->tag != 19)goto _LL294;else{_tmp536=(void*)_tmp535->f1;}}_LL293:
# 2497
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp96E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp96D;b=(void*)((_tmp96D=_cycalloc(sizeof(*_tmp96D)),((_tmp96D[0]=((_tmp96E.tag=1,((_tmp96E.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp536,0),0,Cyc_Absyn_No_coercion,0),_tmp96E)))),_tmp96D))));}
goto _LL291;_LL294:;_LL295:
# 2500
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp971;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp970;b=(void*)((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=((_tmp971.tag=1,((_tmp971.f1=bnd,_tmp971)))),_tmp970))));}else{
# 2503
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL291:;}{
# 2505
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp977;struct Cyc_Absyn_PtrInfo _tmp976;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp975;void*res_typ=(void*)(
(_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=((_tmp977.tag=5,((_tmp977.f1=((_tmp976.elt_typ=_tmp530,((_tmp976.elt_tq=_tmp531,((_tmp976.ptr_atts=(
((_tmp976.ptr_atts).rgn=rgn,(((_tmp976.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp976.ptr_atts).bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp976.ptr_atts).zero_term=_tmp533,(((_tmp976.ptr_atts).ptrloc=0,_tmp976.ptr_atts)))))))))),_tmp976)))))),_tmp977)))),_tmp975))));
# 2509
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51B,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51B,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2516
void*_tmp53B=res_typ;_npop_handler(0);return _tmp53B;};};}_LL28F:;_LL290: {
const char*_tmp97A;void*_tmp979;(_tmp979=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp97A="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp97A,sizeof(char),45))),_tag_dyneither(_tmp979,sizeof(void*),0)));}_LL28C:;}_LL282: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp524;if(_tmp526->tag != 34)goto _LL284;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;}}_LL283:
# 2520
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp97D;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp97C;e1->r=(void*)((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=25,((_tmp97D.f1=_tmp528,_tmp97D)))),_tmp97C))));}
_tmp52A=_tmp528;goto _LL285;_LL284: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp529=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp524;if(_tmp529->tag != 25)goto _LL286;else{_tmp52A=_tmp529->f1;}}_LL285: {
# 2523
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp543=Cyc_Tcutil_compress(*topt);void**_tmp545;struct Cyc_Absyn_Tqual _tmp546;union Cyc_Absyn_Constraint*_tmp547;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp543;if(_tmp544->tag != 5)goto _LL299;else{_tmp545=(void**)&(_tmp544->f1).elt_typ;_tmp546=(_tmp544->f1).elt_tq;_tmp547=((_tmp544->f1).ptr_atts).zero_term;}}_LL298:
# 2529
 elt_typ_opt=_tmp545;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp547);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2535
void*_tmp548=Cyc_Tcexp_tcArray(_tmp51B,e1->loc,elt_typ_opt,0,zero_term,_tmp52A);
e1->topt=_tmp548;{
void*res_typ;
{void*_tmp549=Cyc_Tcutil_compress(_tmp548);void*_tmp54B;struct Cyc_Absyn_Tqual _tmp54C;struct Cyc_Absyn_Exp*_tmp54D;union Cyc_Absyn_Constraint*_tmp54E;_LL29C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 8)goto _LL29E;else{_tmp54B=(_tmp54A->f1).elt_type;_tmp54C=(_tmp54A->f1).tq;_tmp54D=(_tmp54A->f1).num_elts;_tmp54E=(_tmp54A->f1).zero_term;}}_LL29D:
# 2540
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp98E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp98D;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp98C;struct Cyc_Absyn_PtrInfo _tmp98B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp98A;res_typ=(void*)(
(_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98E.tag=5,((_tmp98E.f1=((_tmp98B.elt_typ=_tmp54B,((_tmp98B.elt_tq=_tmp54C,((_tmp98B.ptr_atts=(
((_tmp98B.ptr_atts).rgn=rgn,(((_tmp98B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp98B.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=1,((_tmp98D.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp54D),_tmp98D)))),_tmp98C))))),(((_tmp98B.ptr_atts).zero_term=_tmp54E,(((_tmp98B.ptr_atts).ptrloc=0,_tmp98B.ptr_atts)))))))))),_tmp98B)))))),_tmp98E)))),_tmp98A))));}
# 2545
if(topt != 0){
# 2549
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51B,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51B,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2555
goto _LL29B;_LL29E:;_LL29F: {
const char*_tmp991;void*_tmp990;(_tmp990=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp991="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp991,sizeof(char),50))),_tag_dyneither(_tmp990,sizeof(void*),0)));}_LL29B:;}{
# 2558
void*_tmp556=res_typ;_npop_handler(0);return _tmp556;};};};}_LL286: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp524;if(_tmp52B->tag != 0)goto _LL288;else{if(((_tmp52B->f1).String_c).tag != 8)goto _LL288;}}_LL287: {
# 2563
void*_tmp557=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2565
void*_tmp558=Cyc_Tcexp_tcExp(_tmp51B,& _tmp557,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp994;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp993;void*_tmp55B=Cyc_Absyn_atb_typ(_tmp558,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993[0]=((_tmp994.tag=1,((_tmp994.f1=Cyc_Absyn_uint_exp(1,0),_tmp994)))),_tmp993)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp55B;}_LL288: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp524;if(_tmp52C->tag != 0)goto _LL28A;else{if(((_tmp52C->f1).Wstring_c).tag != 9)goto _LL28A;}}_LL289: {
# 2570
void*_tmp55C=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2572
void*_tmp55D=Cyc_Tcexp_tcExp(_tmp51B,& _tmp55C,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp997;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp996;void*_tmp560=Cyc_Absyn_atb_typ(_tmp55D,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996[0]=((_tmp997.tag=1,((_tmp997.f1=Cyc_Absyn_uint_exp(1,0),_tmp997)))),_tmp996)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp560;}_LL28A:;_LL28B:
# 2579
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp561=Cyc_Tcutil_compress(*topt);void**_tmp563;struct Cyc_Absyn_Tqual _tmp564;_LL2A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag != 5)goto _LL2A3;else{_tmp563=(void**)&(_tmp562->f1).elt_typ;_tmp564=(_tmp562->f1).elt_tq;}}_LL2A2:
# 2585
 topt2=_tmp563;goto _LL2A0;_LL2A3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp561;if(_tmp565->tag != 3)goto _LL2A5;}_LL2A4:
# 2589
 bogus=*topt;
topt2=& bogus;
goto _LL2A0;_LL2A5:;_LL2A6:
 goto _LL2A0;_LL2A0:;}{
# 2595
void*telt=Cyc_Tcexp_tcExp(_tmp51B,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp99D;struct Cyc_Absyn_PtrInfo _tmp99C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp99B;void*res_typ=(void*)(
(_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99D.tag=5,((_tmp99D.f1=((_tmp99C.elt_typ=telt,((_tmp99C.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp99C.ptr_atts=(
((_tmp99C.ptr_atts).rgn=rgn,(((_tmp99C.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp99C.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp99C.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp99C.ptr_atts).ptrloc=0,_tmp99C.ptr_atts)))))))))),_tmp99C)))))),_tmp99D)))),_tmp99B))));
# 2601
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp51B,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp51B,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2608
void*_tmp566=res_typ;_npop_handler(0);return _tmp566;};};}_LL27F:;};}
# 2459
;_pop_region(r);}
# 2615
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp56A=t;void*_tmp56C;struct Cyc_Absyn_Tqual _tmp56D;struct Cyc_Absyn_Exp*_tmp56E;union Cyc_Absyn_Constraint*_tmp56F;_LL2A8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56A;if(_tmp56B->tag != 8)goto _LL2AA;else{_tmp56C=(_tmp56B->f1).elt_type;_tmp56D=(_tmp56B->f1).tq;_tmp56E=(_tmp56B->f1).num_elts;_tmp56F=(_tmp56B->f1).zero_term;}}_LL2A9: {
# 2619
void*_tmp571;struct _tuple14 _tmp570=Cyc_Tcutil_addressof_props(te,e);_tmp571=_tmp570.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9A0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp99F;void*_tmp572=_tmp56E == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F[0]=((_tmp9A0.tag=1,((_tmp9A0.f1=_tmp56E,_tmp9A0)))),_tmp99F))));
t=Cyc_Absyn_atb_typ(_tmp56C,_tmp571,_tmp56D,_tmp572,_tmp56F);
e->topt=t;
return t;};}_LL2AA:;_LL2AB:
# 2625
 return t;_LL2A7:;}
# 2632
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2636
struct _RegionHandle _tmp575=_new_region("temp");struct _RegionHandle*temp=& _tmp575;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2642
const char*_tmp9A3;void*_tmp9A2;(_tmp9A2=0,Cyc_Tcutil_terr(e->loc,((_tmp9A3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9A3,sizeof(char),49))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}{
# 2645
void*_tmp578=e->r;_LL2AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp579=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp578;if(_tmp579->tag != 25)goto _LL2AF;}_LL2AE:
 goto _LL2B0;_LL2AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp578;if(_tmp57A->tag != 26)goto _LL2B1;}_LL2B0:
 goto _LL2B2;_LL2B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp578;if(_tmp57B->tag != 0)goto _LL2B3;else{if(((_tmp57B->f1).Wstring_c).tag != 9)goto _LL2B3;}}_LL2B2:
 goto _LL2B4;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp578;if(_tmp57C->tag != 0)goto _LL2B5;else{if(((_tmp57C->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4: {
void*_tmp57D=t;_npop_handler(0);return _tmp57D;}_LL2B5:;_LL2B6:
# 2651
 t=Cyc_Tcutil_compress(t);{
void*_tmp57E=t;void*_tmp580;struct Cyc_Absyn_Tqual _tmp581;struct Cyc_Absyn_Exp*_tmp582;union Cyc_Absyn_Constraint*_tmp583;_LL2B8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp57F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57E;if(_tmp57F->tag != 8)goto _LL2BA;else{_tmp580=(_tmp57F->f1).elt_type;_tmp581=(_tmp57F->f1).tq;_tmp582=(_tmp57F->f1).num_elts;_tmp583=(_tmp57F->f1).zero_term;}}_LL2B9: {
# 2654
void*_tmp585;struct _tuple14 _tmp584=Cyc_Tcutil_addressof_props(te,e);_tmp585=_tmp584.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9A6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9A5;void*b=_tmp582 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=1,((_tmp9A6.f1=_tmp582,_tmp9A6)))),_tmp9A5))));
t=Cyc_Absyn_atb_typ(_tmp580,_tmp585,_tmp581,b,_tmp583);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp586=t;_npop_handler(0);return _tmp586;};};}_LL2BA:;_LL2BB: {
# 2661
void*_tmp589=t;_npop_handler(0);return _tmp589;}_LL2B7:;};_LL2AC:;};
# 2636
;_pop_region(temp);}
# 2673 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp58A=e->r;struct Cyc_Absyn_Exp*_tmp58C;_LL2BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp58B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58B->tag != 11)goto _LL2BF;else{_tmp58C=_tmp58B->f1;}}_LL2BE:
# 2677
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp58C);
_tmp58C->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp58C->topt),0);
e->topt=_tmp58C->topt;
goto _LL2BC;_LL2BF:;_LL2C0:
# 2683
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2686
{void*_tmp58D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp58F;struct Cyc_Absyn_Tqual _tmp590;void*_tmp591;union Cyc_Absyn_Constraint*_tmp592;union Cyc_Absyn_Constraint*_tmp593;union Cyc_Absyn_Constraint*_tmp594;_LL2C2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58D;if(_tmp58E->tag != 5)goto _LL2C4;else{_tmp58F=(_tmp58E->f1).elt_typ;_tmp590=(_tmp58E->f1).elt_tq;_tmp591=((_tmp58E->f1).ptr_atts).rgn;_tmp592=((_tmp58E->f1).ptr_atts).nullable;_tmp593=((_tmp58E->f1).ptr_atts).bounds;_tmp594=((_tmp58E->f1).ptr_atts).zero_term;}}_LL2C3:
# 2688
{void*_tmp595=Cyc_Tcutil_compress(_tmp58F);struct Cyc_List_List*_tmp597;void*_tmp598;struct Cyc_Absyn_Tqual _tmp599;void*_tmp59A;struct Cyc_List_List*_tmp59B;int _tmp59C;struct Cyc_Absyn_VarargInfo*_tmp59D;struct Cyc_List_List*_tmp59E;struct Cyc_List_List*_tmp59F;_LL2C7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 9)goto _LL2C9;else{_tmp597=(_tmp596->f1).tvars;_tmp598=(_tmp596->f1).effect;_tmp599=(_tmp596->f1).ret_tqual;_tmp59A=(_tmp596->f1).ret_typ;_tmp59B=(_tmp596->f1).args;_tmp59C=(_tmp596->f1).c_varargs;_tmp59D=(_tmp596->f1).cyc_varargs;_tmp59E=(_tmp596->f1).rgn_po;_tmp59F=(_tmp596->f1).attributes;}}_LL2C8:
# 2690
 if(_tmp597 != 0){
struct _RegionHandle _tmp5A0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5A0;_push_region(rgn);
{struct _tuple12 _tmp9A7;struct _tuple12 _tmp5A1=(_tmp9A7.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9A7.f2=rgn,_tmp9A7)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5A1,_tmp597);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9AD;struct Cyc_Absyn_FnInfo _tmp9AC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9AB;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AD.tag=9,((_tmp9AD.f1=((_tmp9AC.tvars=0,((_tmp9AC.effect=_tmp598,((_tmp9AC.ret_tqual=_tmp599,((_tmp9AC.ret_typ=_tmp59A,((_tmp9AC.args=_tmp59B,((_tmp9AC.c_varargs=_tmp59C,((_tmp9AC.cyc_varargs=_tmp59D,((_tmp9AC.rgn_po=_tmp59E,((_tmp9AC.attributes=_tmp59F,_tmp9AC)))))))))))))))))),_tmp9AD)))),_tmp9AB)))));
# 2700
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9B3;struct Cyc_Absyn_PtrInfo _tmp9B2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A2=(_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B3.tag=5,((_tmp9B3.f1=((_tmp9B2.elt_typ=ftyp,((_tmp9B2.elt_tq=_tmp590,((_tmp9B2.ptr_atts=(((_tmp9B2.ptr_atts).rgn=_tmp591,(((_tmp9B2.ptr_atts).nullable=_tmp592,(((_tmp9B2.ptr_atts).bounds=_tmp593,(((_tmp9B2.ptr_atts).zero_term=_tmp594,(((_tmp9B2.ptr_atts).ptrloc=0,_tmp9B2.ptr_atts)))))))))),_tmp9B2)))))),_tmp9B3)))),_tmp9B1)));
# 2702
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9B6;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9B5;e->r=(void*)((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=12,((_tmp9B6.f1=_tmp5A3,((_tmp9B6.f2=ts,_tmp9B6)))))),_tmp9B5))));}
e->topt=(void*)_tmp5A2;}
# 2692
;_pop_region(rgn);}
# 2707
goto _LL2C6;_LL2C9:;_LL2CA:
 goto _LL2C6;_LL2C6:;}
# 2710
goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}
# 2713
goto _LL2BC;_LL2BC:;}
# 2715
return(void*)_check_null(e->topt);}
# 2723
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2725
struct _RegionHandle _tmp5AD=_new_region("r");struct _RegionHandle*r=& _tmp5AD;_push_region(r);
{struct Cyc_List_List*_tmp5AE=0;
# 2728
{void*_tmp5AF=fn_exp->r;struct Cyc_List_List*_tmp5B1;_LL2CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B0=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5AF;if(_tmp5B0->tag != 9)goto _LL2CE;else{_tmp5B1=_tmp5B0->f2;}}_LL2CD:
# 2730
{void*_tmp5B2=e->r;struct Cyc_List_List*_tmp5B4;_LL2D1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B3->tag != 9)goto _LL2D3;else{_tmp5B4=_tmp5B3->f2;}}_LL2D2: {
# 2732
struct Cyc_List_List*_tmp5B5=alias_arg_exps;
int _tmp5B6=0;
while(_tmp5B5 != 0){
while(_tmp5B6 != (int)_tmp5B5->hd){
if(_tmp5B1 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp9C0;struct Cyc_Int_pa_PrintArg_struct _tmp9BF;void*_tmp9BE[2];const char*_tmp9BD;void*_tmp9BC;(_tmp9BC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp9C0.tag=1,((_tmp9C0.f1=(unsigned long)((int)_tmp5B5->hd),((_tmp9BF.tag=1,((_tmp9BF.f1=(unsigned long)_tmp5B6,((_tmp9BE[0]=& _tmp9BF,((_tmp9BE[1]=& _tmp9C0,Cyc_aprintf(((_tmp9BD="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9BD,sizeof(char),36))),_tag_dyneither(_tmp9BE,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));}
# 2739
++ _tmp5B6;
_tmp5B1=_tmp5B1->tl;
_tmp5B4=((struct Cyc_List_List*)_check_null(_tmp5B4))->tl;}{
# 2744
struct Cyc_Absyn_Decl*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BE;struct _tuple11 _tmp5BC=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B1))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B4))->hd)->topt)));_tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;
_tmp5B1->hd=(void*)_tmp5BE;
{struct Cyc_List_List*_tmp9C1;_tmp5AE=((_tmp9C1=_region_malloc(r,sizeof(*_tmp9C1)),((_tmp9C1->hd=_tmp5BD,((_tmp9C1->tl=_tmp5AE,_tmp9C1))))));}
_tmp5B5=_tmp5B5->tl;};}
# 2749
goto _LL2D0;}_LL2D3:;_LL2D4: {
const char*_tmp9C4;void*_tmp9C3;(_tmp9C3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C4="not a function call!",_tag_dyneither(_tmp9C4,sizeof(char),21))),_tag_dyneither(_tmp9C3,sizeof(void*),0)));}_LL2D0:;}
# 2752
goto _LL2CB;_LL2CE:;_LL2CF: {
const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C7="not a function call!",_tag_dyneither(_tmp9C7,sizeof(char),21))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}_LL2CB:;}
# 2757
while(_tmp5AE != 0){
struct Cyc_Absyn_Decl*_tmp5C4=(struct Cyc_Absyn_Decl*)_tmp5AE->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5C4,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5AE=_tmp5AE->tl;}
# 2764
e->topt=0;
e->r=fn_exp->r;}
# 2726
;_pop_region(r);}
# 2771
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2775
{void*_tmp5C5=e->r;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_List_List*_tmp5CA;struct Cyc_Absyn_VarargCallInfo**_tmp5CB;int*_tmp5CC;struct Cyc_Core_Opt*_tmp5CE;struct Cyc_List_List*_tmp5CF;union Cyc_Absyn_Cnst*_tmp5D1;struct _tuple1*_tmp5D3;void**_tmp5D4;enum Cyc_Absyn_Primop _tmp5D6;struct Cyc_List_List*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;enum Cyc_Absyn_Incrementor _tmp5DA;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Core_Opt*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_List_List*_tmp5EE;struct Cyc_Absyn_VarargCallInfo**_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_List_List*_tmp5F4;void*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F7;enum Cyc_Absyn_Coercion*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp**_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FF;void*_tmp601;void*_tmp603;struct Cyc_List_List*_tmp604;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Exp*_tmp608;struct _dyneither_ptr*_tmp609;int*_tmp60A;int*_tmp60B;struct Cyc_Absyn_Exp*_tmp60D;struct _dyneither_ptr*_tmp60E;int*_tmp60F;int*_tmp610;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_List_List*_tmp615;struct _tuple9*_tmp617;struct Cyc_List_List*_tmp618;struct Cyc_List_List*_tmp61A;struct Cyc_Absyn_Stmt*_tmp61C;struct Cyc_Absyn_Vardecl*_tmp61E;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Exp*_tmp620;int*_tmp621;struct _tuple1**_tmp623;struct Cyc_List_List**_tmp624;struct Cyc_List_List*_tmp625;struct Cyc_Absyn_Aggrdecl**_tmp626;void*_tmp628;struct Cyc_List_List*_tmp629;struct Cyc_List_List*_tmp62B;struct Cyc_Absyn_Datatypedecl*_tmp62C;struct Cyc_Absyn_Datatypefield*_tmp62D;struct _tuple1**_tmp62F;struct Cyc_Absyn_Enumdecl*_tmp630;struct Cyc_Absyn_Enumfield*_tmp631;struct _tuple1**_tmp633;void*_tmp634;struct Cyc_Absyn_Enumfield*_tmp635;int*_tmp637;struct Cyc_Absyn_Exp**_tmp638;void***_tmp639;struct Cyc_Absyn_Exp**_tmp63A;int*_tmp63B;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp640;struct _dyneither_ptr*_tmp641;void*_tmp643;_LL2D6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5C6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5C6->tag != 11)goto _LL2D8;else{_tmp5C7=_tmp5C6->f1;}}_LL2D7:
# 2780
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5C7);
return;_LL2D8:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5C8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5C8->tag != 9)goto _LL2DA;else{_tmp5C9=_tmp5C8->f1;_tmp5CA=_tmp5C8->f2;_tmp5CB=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5C8->f3;_tmp5CC=(int*)& _tmp5C8->f4;}}if(!(!(*_tmp5CC)))goto _LL2DA;_LL2D9:
# 2786
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5C9,_tmp5CA)){
*_tmp5CC=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2790
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2793
return;_LL2DA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5CD=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5CD->tag != 34)goto _LL2DC;else{_tmp5CE=_tmp5CD->f1;_tmp5CF=_tmp5CD->f2;}}_LL2DB:
# 2797
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5CF);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5D0->tag != 0)goto _LL2DE;else{_tmp5D1=(union Cyc_Absyn_Cnst*)& _tmp5D0->f1;}}_LL2DD:
# 2802
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5D1,e);goto _LL2D5;_LL2DE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5D2->tag != 1)goto _LL2E0;else{_tmp5D3=_tmp5D2->f1;_tmp5D4=(void**)((void**)& _tmp5D2->f2);}}_LL2DF:
# 2804
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5D3,_tmp5D4);goto _LL2D5;_LL2E0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5D5=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5D5->tag != 2)goto _LL2E2;else{_tmp5D6=_tmp5D5->f1;_tmp5D7=_tmp5D5->f2;}}_LL2E1:
# 2806
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5D6,_tmp5D7);goto _LL2D5;_LL2E2: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5D8=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5D8->tag != 4)goto _LL2E4;else{_tmp5D9=_tmp5D8->f1;_tmp5DA=_tmp5D8->f2;}}_LL2E3:
# 2808
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5D9,_tmp5DA);goto _LL2D5;_LL2E4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5DB->tag != 3)goto _LL2E6;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;_tmp5DE=_tmp5DB->f3;}}_LL2E5:
# 2810
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5DC,_tmp5DD,_tmp5DE);goto _LL2D5;_LL2E6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5DF=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5DF->tag != 5)goto _LL2E8;else{_tmp5E0=_tmp5DF->f1;_tmp5E1=_tmp5DF->f2;_tmp5E2=_tmp5DF->f3;}}_LL2E7:
# 2812
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5E0,_tmp5E1,_tmp5E2);goto _LL2D5;_LL2E8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5E3->tag != 6)goto _LL2EA;else{_tmp5E4=_tmp5E3->f1;_tmp5E5=_tmp5E3->f2;}}_LL2E9:
# 2814
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5E4,_tmp5E5);goto _LL2D5;_LL2EA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5E6=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5E6->tag != 7)goto _LL2EC;else{_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;}}_LL2EB:
# 2816
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5E7,_tmp5E8);goto _LL2D5;_LL2EC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5E9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5E9->tag != 8)goto _LL2EE;else{_tmp5EA=_tmp5E9->f1;_tmp5EB=_tmp5E9->f2;}}_LL2ED:
# 2818
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5EA,_tmp5EB);goto _LL2D5;_LL2EE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5EC->tag != 9)goto _LL2F0;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;_tmp5EF=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5EC->f3;}}_LL2EF: {
# 2824
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp645;_push_handler(& _tmp645);{int _tmp647=0;if(setjmp(_tmp645.handler))_tmp647=1;if(!_tmp647){
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp646=(void*)_exn_thrown;void*_tmp649=_tmp646;struct _dyneither_ptr _tmp64B;void*_tmp64C;_LL327: {struct Cyc_Core_Failure_exn_struct*_tmp64A=(struct Cyc_Core_Failure_exn_struct*)_tmp649;if(_tmp64A->tag != Cyc_Core_Failure)goto _LL329;else{_tmp64B=_tmp64A->f1;}}_LL328:
# 2831
 ok=0;
fn_exp=e;
goto _LL326;_LL329: _tmp64C=_tmp649;_LL32A:(void)_throw(_tmp64C);_LL326:;}};}
# 2835
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5ED,_tmp5EE,_tmp5EF,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2842
goto _LL2D5;}_LL2F0: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5F0=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5F0->tag != 10)goto _LL2F2;else{_tmp5F1=_tmp5F0->f1;}}_LL2F1:
# 2844
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5F1);goto _LL2D5;_LL2F2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5F2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5F2->tag != 12)goto _LL2F4;else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;}}_LL2F3:
# 2846
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5F3,_tmp5F4);goto _LL2D5;_LL2F4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5F5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5F5->tag != 13)goto _LL2F6;else{_tmp5F6=(void*)_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=(enum Cyc_Absyn_Coercion*)& _tmp5F5->f4;}}_LL2F5:
# 2848
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5F6,_tmp5F7,_tmp5F8);goto _LL2D5;_LL2F6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp5F9=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5F9->tag != 14)goto _LL2F8;else{_tmp5FA=_tmp5F9->f1;}}_LL2F7:
# 2850
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp5FA);goto _LL2D5;_LL2F8: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5FB->tag != 15)goto _LL2FA;else{_tmp5FC=(struct Cyc_Absyn_Exp**)& _tmp5FB->f1;_tmp5FD=_tmp5FB->f2;}}_LL2F9:
# 2852
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5FC,e,_tmp5FD);goto _LL2D5;_LL2FA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp5FE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5FE->tag != 17)goto _LL2FC;else{_tmp5FF=_tmp5FE->f1;}}_LL2FB: {
# 2854
void*_tmp64D=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5FF);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp64D);goto _LL2D5;}_LL2FC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp600->tag != 16)goto _LL2FE;else{_tmp601=(void*)_tmp600->f1;}}_LL2FD:
# 2857
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp601);goto _LL2D5;_LL2FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp602->tag != 18)goto _LL300;else{_tmp603=(void*)_tmp602->f1;_tmp604=_tmp602->f2;}}_LL2FF:
# 2859
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp603,_tmp604);goto _LL2D5;_LL300: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp605=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp605->tag != 19)goto _LL302;else{_tmp606=_tmp605->f1;}}_LL301:
# 2861
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp606);goto _LL2D5;_LL302: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp607=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp607->tag != 20)goto _LL304;else{_tmp608=_tmp607->f1;_tmp609=_tmp607->f2;_tmp60A=(int*)& _tmp607->f3;_tmp60B=(int*)& _tmp607->f4;}}_LL303:
# 2863
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp608,_tmp609,_tmp60A,_tmp60B);goto _LL2D5;_LL304: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp60C->tag != 21)goto _LL306;else{_tmp60D=_tmp60C->f1;_tmp60E=_tmp60C->f2;_tmp60F=(int*)& _tmp60C->f3;_tmp610=(int*)& _tmp60C->f4;}}_LL305:
# 2865
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp60D,_tmp60E,_tmp60F,_tmp610);goto _LL2D5;_LL306: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp611->tag != 22)goto _LL308;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;}}_LL307:
# 2867
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp612,_tmp613);goto _LL2D5;_LL308: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp614->tag != 23)goto _LL30A;else{_tmp615=_tmp614->f1;}}_LL309:
# 2869
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp615);goto _LL2D5;_LL30A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp616=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp616->tag != 24)goto _LL30C;else{_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;}}_LL30B:
# 2871
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp617,_tmp618);goto _LL2D5;_LL30C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp619=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp619->tag != 25)goto _LL30E;else{_tmp61A=_tmp619->f1;}}_LL30D: {
# 2875
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp64E=Cyc_Tcutil_compress(*topt);void**_tmp650;struct Cyc_Absyn_Tqual*_tmp651;union Cyc_Absyn_Constraint*_tmp652;_LL32C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp64E;if(_tmp64F->tag != 8)goto _LL32E;else{_tmp650=(void**)&(_tmp64F->f1).elt_type;_tmp651=(struct Cyc_Absyn_Tqual*)&(_tmp64F->f1).tq;_tmp652=(_tmp64F->f1).zero_term;}}_LL32D:
# 2881
 elt_topt=_tmp650;
elt_tqopt=_tmp651;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp652);
goto _LL32B;_LL32E:;_LL32F:
 goto _LL32B;_LL32B:;}
# 2888
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp61A);goto _LL2D5;}_LL30E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp61B=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp61B->tag != 35)goto _LL310;else{_tmp61C=_tmp61B->f1;}}_LL30F:
# 2890
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp61C);goto _LL2D5;_LL310: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp61D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp61D->tag != 26)goto _LL312;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;_tmp620=_tmp61D->f3;_tmp621=(int*)& _tmp61D->f4;}}_LL311:
# 2892
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp61E,_tmp61F,_tmp620,_tmp621);goto _LL2D5;_LL312: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp622=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp622->tag != 27)goto _LL314;else{_tmp623=(struct _tuple1**)& _tmp622->f1;_tmp624=(struct Cyc_List_List**)& _tmp622->f2;_tmp625=_tmp622->f3;_tmp626=(struct Cyc_Absyn_Aggrdecl**)& _tmp622->f4;}}_LL313:
# 2894
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp623,_tmp624,_tmp625,_tmp626);goto _LL2D5;_LL314: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp627=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp627->tag != 28)goto _LL316;else{_tmp628=(void*)_tmp627->f1;_tmp629=_tmp627->f2;}}_LL315:
# 2896
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp628,_tmp629);goto _LL2D5;_LL316: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp62A=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp62A->tag != 29)goto _LL318;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62A->f2;_tmp62D=_tmp62A->f3;}}_LL317:
# 2898
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp62B,_tmp62C,_tmp62D);goto _LL2D5;_LL318: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp62E->tag != 30)goto _LL31A;else{_tmp62F=(struct _tuple1**)& _tmp62E->f1;_tmp630=_tmp62E->f2;_tmp631=_tmp62E->f3;}}_LL319:
# 2900
*_tmp62F=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp631))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9CA;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9C9;t=(void*)((_tmp9C9=_cycalloc(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=13,((_tmp9CA.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp630))->name,((_tmp9CA.f2=_tmp630,_tmp9CA)))))),_tmp9C9))));}goto _LL2D5;_LL31A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp632=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp632->tag != 31)goto _LL31C;else{_tmp633=(struct _tuple1**)& _tmp632->f1;_tmp634=(void*)_tmp632->f2;_tmp635=_tmp632->f3;}}_LL31B:
# 2903
*_tmp633=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp635))->name;
t=_tmp634;goto _LL2D5;_LL31C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp636=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp636->tag != 32)goto _LL31E;else{_tmp637=(int*)&(_tmp636->f1).is_calloc;_tmp638=(struct Cyc_Absyn_Exp**)&(_tmp636->f1).rgn;_tmp639=(void***)&(_tmp636->f1).elt_type;_tmp63A=(struct Cyc_Absyn_Exp**)&(_tmp636->f1).num_elts;_tmp63B=(int*)&(_tmp636->f1).fat_result;}}_LL31D:
# 2906
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp638,_tmp639,_tmp63A,_tmp637,_tmp63B);goto _LL2D5;_LL31E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp63C=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp63C->tag != 33)goto _LL320;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;}}_LL31F:
# 2908
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp63D,_tmp63E);goto _LL2D5;_LL320: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp63F->tag != 36)goto _LL322;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL321:
# 2910
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp640,_tmp641);goto _LL2D5;_LL322: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp642->tag != 37)goto _LL324;else{_tmp643=(void*)_tmp642->f1;}}_LL323:
# 2912
 if(!te->allow_valueof){
const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,Cyc_Tcutil_terr(e->loc,((_tmp9CD="valueof(-) can only occur within types",_tag_dyneither(_tmp9CD,sizeof(char),39))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}
# 2920
t=Cyc_Absyn_sint_typ;
goto _LL2D5;_LL324: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp644->tag != 38)goto _LL2D5;}_LL325:
# 2923
{const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,Cyc_Tcutil_terr(e->loc,((_tmp9D0="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9D0,sizeof(char),50))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2D5:;}
# 2926
e->topt=t;}
