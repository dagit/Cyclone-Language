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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 276
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 925
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 946
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 948
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 950
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 960
void*Cyc_Absyn_exn_typ();
# 971
extern void*Cyc_Absyn_bounds_one;
# 976
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 979
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 982
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1070
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1085
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1144
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1146
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1163
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
# 135
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 138
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 143
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 147
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 169
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 175
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 188
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 194
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 202
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 206
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 210
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 217
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 56
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 59
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 66
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 75
int Cyc_Tcutil_coerceable(void*);
# 78
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 80
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 86
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 97
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 147
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 149
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 153
void Cyc_Tcutil_explain_failure();
# 155
int Cyc_Tcutil_unify(void*,void*);
# 160
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 182
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 187
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 196
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 232 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 242
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 244
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 246
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 250
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 256
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 258
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 260
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 270
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 275
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 278
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 283
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 288
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 297
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 324
int Cyc_Tcutil_bits_only(void*t);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 356
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 359
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
extern int Cyc_Tcexp_in_stmt_exp;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 50 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 61
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 63
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 68
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 83
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 88
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 95
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 115
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 122
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 157
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 228 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp688;(_tmp688=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp688,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp689;return(_tmp689=_cycalloc(sizeof(*_tmp689)),((_tmp689->f1=0,((_tmp689->f2=e,_tmp689)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_stmttmp0=e1->r;void*_tmp2=_stmttmp0;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_stmttmp1=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpA=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp68D;void*_tmp68C[1];struct Cyc_String_pa_PrintArg_struct _tmp68B;(_tmp68B.tag=0,((_tmp68B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp68C[0]=& _tmp68B,Cyc_Tcutil_terr(e1->loc,((_tmp68D="undeclared identifier %s",_tag_dyneither(_tmp68D,sizeof(char),25))),_tag_dyneither(_tmp68C,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp691;void*_tmp690[1];struct Cyc_String_pa_PrintArg_struct _tmp68F;(_tmp68F.tag=0,((_tmp68F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp690[0]=& _tmp68F,Cyc_Tcutil_terr(e->loc,((_tmp691="%s is a constant, not a function",_tag_dyneither(_tmp691,sizeof(char),33))),_tag_dyneither(_tmp690,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp694;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp693;e->r=(void*)((_tmp693=_cycalloc(sizeof(*_tmp693)),((_tmp693[0]=((_tmp694.tag=30,((_tmp694.f1=es,((_tmp694.f2=_tmp10,((_tmp694.f3=_tmp11,_tmp694)))))))),_tmp693))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp697;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp696;e->r=(void*)((_tmp696=_cycalloc(sizeof(*_tmp696)),((_tmp696[0]=((_tmp697.tag=28,((_tmp697.f1=_tmp13->name,((_tmp697.f2=0,((_tmp697.f3=_tmp22,((_tmp697.f4=_tmp13,_tmp697)))))))))),_tmp696))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp69B;void*_tmp69A[1];struct Cyc_String_pa_PrintArg_struct _tmp699;(_tmp699.tag=0,((_tmp699.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp69A[0]=& _tmp699,Cyc_Tcutil_terr(e->loc,((_tmp69B="%s is an enum constructor, not a function",_tag_dyneither(_tmp69B,sizeof(char),42))),_tag_dyneither(_tmp69A,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp69F;void*_tmp69E[1];struct Cyc_String_pa_PrintArg_struct _tmp69D;(_tmp69D.tag=0,((_tmp69D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp69E[0]=& _tmp69D,Cyc_Tcutil_terr(e1->loc,((_tmp69F="undeclared identifier %s",_tag_dyneither(_tmp69F,sizeof(char),25))),_tag_dyneither(_tmp69E,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6A2;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6A1;e->r=(void*)((_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=25,((_tmp6A2.f1=des,_tmp6A2)))),_tmp6A1))));}
return;}{
# 125
void*t=*topt;
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp33=_stmttmp2;union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6A5;void*_tmp6A4;(_tmp6A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6A5="struct type not properly set",_tag_dyneither(_tmp6A5,sizeof(char),29))),_tag_dyneither(_tmp6A4,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6A8;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6A7;e->r=(void*)((_tmp6A7=_cycalloc(sizeof(*_tmp6A7)),((_tmp6A7[0]=((_tmp6A8.tag=28,((_tmp6A8.f1=_tmp3B->name,((_tmp6A8.f2=0,((_tmp6A8.f3=des,((_tmp6A8.f4=_tmp3B,_tmp6A8)))))))))),_tmp6A7))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6AB;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6AA;e->r=(void*)((_tmp6AA=_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=25,((_tmp6AB.f1=des,_tmp6AB)))),_tmp6AA))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6AE;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6AD;e->r=(void*)((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=29,((_tmp6AE.f1=t,((_tmp6AE.f2=des,_tmp6AE)))))),_tmp6AD))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6B1;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=25,((_tmp6B1.f1=des,_tmp6B1)))),_tmp6B0))));}goto _LL17;_LL17:;};}
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
const char*_tmp6B6;void*_tmp6B5[2];struct Cyc_String_pa_PrintArg_struct _tmp6B4;struct Cyc_String_pa_PrintArg_struct _tmp6B3;(_tmp6B3.tag=0,((_tmp6B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6B5[0]=& _tmp6B4,((_tmp6B5[1]=& _tmp6B3,Cyc_Tcutil_terr(e->loc,((_tmp6B6="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp6B6,sizeof(char),53))),_tag_dyneither(_tmp6B5,sizeof(void*),2)))))))))))));}
# 161
{void*_stmttmp3=e->r;void*_tmp4A=_stmttmp3;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp6B7;struct _tuple0 _stmttmp4=(_tmp6B7.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp6B7.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp6B7)));struct _tuple0 _tmp4E=_stmttmp4;void*_tmp50;void*_tmp52;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f1;if(_tmp4F->tag != 15)goto _LL2D;else{_tmp50=(void*)_tmp4F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f2;if(_tmp51->tag != 15)goto _LL2D;else{_tmp52=(void*)_tmp51->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6BA;struct Cyc_Tcexp_TestEnv _tmp6B9;return(_tmp6B9.eq=((_tmp6BA=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6BA)),((_tmp6BA->f1=_tmp50,((_tmp6BA->f2=_tmp52,_tmp6BA)))))),((_tmp6B9.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp6B9)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6BB;return(_tmp6BB.eq=0,((_tmp6BB.isTrue=0,_tmp6BB)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _stmttmp5=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp57=_stmttmp5;enum Cyc_Absyn_Sign _tmp58;enum Cyc_Absyn_Sign _tmp59;int _tmp5A;enum Cyc_Absyn_Sign _tmp5B;int _tmp5C;struct _dyneither_ptr _tmp5D;struct _dyneither_ptr _tmp5E;_LL30: if((_tmp57.Char_c).tag != 2)goto _LL32;if(((struct _tuple4)(_tmp57.Char_c).val).f1 != Cyc_Absyn_Signed)goto _LL32;_LL31:
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
void*_stmttmp8=Cyc_Tcutil_compress(*topt);void*_tmp5F=_stmttmp8;enum Cyc_Absyn_Sign _tmp61;enum Cyc_Absyn_Sign _tmp63;enum Cyc_Absyn_Sign _tmp65;enum Cyc_Absyn_Sign _tmp67;void*_tmp69;struct Cyc_Absyn_Tqual _tmp6A;void*_tmp6B;union Cyc_Absyn_Constraint*_tmp6C;union Cyc_Absyn_Constraint*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;void*_tmp70;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F;if(_tmp60->tag != 6)goto _LL49;else{_tmp61=_tmp60->f1;if(_tmp60->f2 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6C1;struct Cyc_Absyn_PtrInfo _tmp6C0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BF;t=(void*)((_tmp6BF=_cycalloc(sizeof(*_tmp6BF)),((_tmp6BF[0]=((_tmp6C1.tag=5,((_tmp6C1.f1=((_tmp6C0.elt_typ=_tmp69,((_tmp6C0.elt_tq=_tmp6A,((_tmp6C0.ptr_atts=(
((_tmp6C0.ptr_atts).rgn=_tmp6B,(((_tmp6C0.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6C0.ptr_atts).bounds=_tmp6D,(((_tmp6C0.ptr_atts).zero_term=_tmp6E,(((_tmp6C0.ptr_atts).ptrloc=0,_tmp6C0.ptr_atts)))))))))),_tmp6C0)))))),_tmp6C1)))),_tmp6BF))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6C4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6C3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp6C3=_cycalloc(sizeof(*_tmp6C3)),((_tmp6C3[0]=((_tmp6C4.tag=18,((_tmp6C4.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp6C4)))),_tmp6C3)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6C7;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6C6;t=(void*)((_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=19,((_tmp6C7.f1=(void*)_tmp76,_tmp6C7)))),_tmp6C6))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6CA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6C9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CA.tag=1,((_tmp6CA.f1=elen,_tmp6CA)))),_tmp6C9)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp7B,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_stmttmp7=Cyc_Tcutil_compress(*topt);void*_tmp7C=_stmttmp7;struct Cyc_Absyn_Tqual _tmp7E;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7C;if(_tmp7D->tag != 8)goto _LL5C;else{_tmp7E=(_tmp7D->f1).tq;}}_LL5B:
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
void*_stmttmp6=Cyc_Tcutil_compress(*topt);void*_tmp82=_stmttmp6;void*_tmp84;struct Cyc_Absyn_Tqual _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp82;if(_tmp83->tag != 5)goto _LL63;else{_tmp84=(_tmp83->f1).elt_typ;_tmp85=(_tmp83->f1).elt_tq;_tmp86=((_tmp83->f1).ptr_atts).rgn;_tmp87=((_tmp83->f1).ptr_atts).nullable;_tmp88=((_tmp83->f1).ptr_atts).bounds;_tmp89=((_tmp83->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp87))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6D0;struct Cyc_Absyn_PtrInfo _tmp6CF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CE;return(void*)((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE[0]=((_tmp6D0.tag=5,((_tmp6D0.f1=((_tmp6CF.elt_typ=_tmp84,((_tmp6CF.elt_tq=_tmp85,((_tmp6CF.ptr_atts=(((_tmp6CF.ptr_atts).rgn=_tmp86,(((_tmp6CF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6CF.ptr_atts).bounds=_tmp88,(((_tmp6CF.ptr_atts).zero_term=_tmp89,(((_tmp6CF.ptr_atts).ptrloc=0,_tmp6CF.ptr_atts)))))))))),_tmp6CF)))))),_tmp6D0)))),_tmp6CE))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6DF;struct Cyc_Core_Opt*_tmp6DE;struct Cyc_Core_Opt*_tmp6DD;struct Cyc_Absyn_PtrInfo _tmp6DC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DB;t=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DF.tag=5,((_tmp6DF.f1=((_tmp6DC.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD->v=_tmp8D,_tmp6DD))))),((_tmp6DC.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6DC.ptr_atts=(
((_tmp6DC.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE->v=_tmp8D,_tmp6DE))))),(((_tmp6DC.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6DC.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6DC.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6DC.ptr_atts).ptrloc=0,_tmp6DC.ptr_atts)))))))))),_tmp6DC)))))),_tmp6DF)))),_tmp6DB))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_stmttmp9=*((void**)_check_null(b));void*_tmp93=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp96;struct Cyc_Absyn_Fndecl*_tmp98;struct Cyc_Absyn_Vardecl*_tmp9A;struct Cyc_Absyn_Vardecl*_tmp9C;struct Cyc_Absyn_Vardecl*_tmp9E;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp94=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp93;if(_tmp94->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmp9F;_push_handler(& _tmp9F);{int _tmpA1=0;if(setjmp(_tmp9F.handler))_tmpA1=1;if(!_tmpA1){
{struct _RegionHandle _tmpA2=_new_region("r");struct _RegionHandle*r=& _tmpA2;_push_region(r);
{void*_stmttmpB=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpA3=_stmttmpB;void*_tmpA7;struct Cyc_Absyn_Enumdecl*_tmpA9;struct Cyc_Absyn_Enumfield*_tmpAA;void*_tmpAC;struct Cyc_Absyn_Enumfield*_tmpAD;struct Cyc_Absyn_Datatypedecl*_tmpAF;struct Cyc_Absyn_Datatypefield*_tmpB0;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA4->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpA4->f1);if(_tmpA5->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_PrintArg_struct _tmp6E1;void*_tmpB5=(_tmp6E1.tag=0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6E2[0]=& _tmp6E1,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6E3="undeclared identifier %s",_tag_dyneither(_tmp6E3,sizeof(char),25))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E6;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E5;e->r=(void*)((_tmp6E5=_cycalloc(sizeof(*_tmp6E5)),((_tmp6E5[0]=((_tmp6E6.tag=31,((_tmp6E6.f1=_tmpAA->name,((_tmp6E6.f2=_tmpA9,((_tmp6E6.f3=_tmpAA,_tmp6E6)))))))),_tmp6E5))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6E9;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6E8;void*_tmpBB=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=13,((_tmp6E9.f1=_tmpA9->name,((_tmp6E9.f2=_tmpA9,_tmp6E9)))))),_tmp6E8))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6EC;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6EB;e->r=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=32,((_tmp6EC.f1=_tmpAD->name,((_tmp6EC.f2=_tmpAC,((_tmp6EC.f3=_tmpAD,_tmp6EC)))))))),_tmp6EB))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6EF;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6EE;e->r=(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=30,((_tmp6EF.f1=0,((_tmp6EF.f2=_tmpAF,((_tmp6EF.f3=_tmpB0,_tmp6EF)))))))),_tmp6EE))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6F3;void*_tmp6F2[1];struct Cyc_String_pa_PrintArg_struct _tmp6F1;void*_tmpC5=(_tmp6F1.tag=0,((_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6F2[0]=& _tmp6F1,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6F3="bad occurrence of type name %s",_tag_dyneither(_tmp6F3,sizeof(char),31))),_tag_dyneither(_tmp6F2,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6F7;void*_tmp6F6[1];struct Cyc_String_pa_PrintArg_struct _tmp6F5;return(_tmp6F5.tag=0,((_tmp6F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6F6[0]=& _tmp6F5,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6F7="undeclared identifier %s",_tag_dyneither(_tmp6F7,sizeof(char),25))),_tag_dyneither(_tmp6F6,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6F8;(*q).f1=(((_tmp6F8.Loc_n).val=0,(((_tmp6F8.Loc_n).tag=4,_tmp6F8))));}
if(te->allow_valueof){
void*_stmttmpA=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpCE=_stmttmpA;void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6FB;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6FA;e->r=(void*)((_tmp6FA=_cycalloc(sizeof(*_tmp6FA)),((_tmp6FA[0]=((_tmp6FB.tag=38,((_tmp6FB.f1=_tmpD0,_tmp6FB)))),_tmp6FA))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmp9E->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_stmttmpC=fmt->r;void*_tmpD3=_stmttmpC;struct _dyneither_ptr _tmpD5;struct _dyneither_ptr _tmpD8;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD3;if(_tmpD4->tag != 0)goto _LL8C;else{if(((_tmpD4->f1).String_c).tag != 8)goto _LL8C;_tmpD5=(struct _dyneither_ptr)((_tmpD4->f1).String_c).val;}}_LL8B:
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
const char*_tmp6FE;void*_tmp6FD;(_tmp6FD=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp6FE="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6FE,sizeof(char),49))),_tag_dyneither(_tmp6FD,sizeof(void*),0)));}
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
{const char*_tmp703;void*_tmp702[2];struct Cyc_String_pa_PrintArg_struct _tmp701;struct Cyc_String_pa_PrintArg_struct _tmp700;(_tmp700.tag=0,((_tmp700.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp702[0]=& _tmp701,((_tmp702[1]=& _tmp700,Cyc_Tcutil_terr(e->loc,((_tmp703="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp703,sizeof(char),51))),_tag_dyneither(_tmp702,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp704;*alias_arg_exps=((_tmp704=_cycalloc(sizeof(*_tmp704)),((_tmp704->hd=(void*)arg_cnt,((_tmp704->tl=*alias_arg_exps,_tmp704))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp707;void*_tmp706;(_tmp706=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp707="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp707,sizeof(char),49))),_tag_dyneither(_tmp706,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp70A;void*_tmp709;(_tmp709=0,Cyc_Tcutil_terr(fmt->loc,((_tmp70A="too few arguments",_tag_dyneither(_tmp70A,sizeof(char),18))),_tag_dyneither(_tmp709,sizeof(void*),0)));}
if(_tmpDD != 0){
const char*_tmp70D;void*_tmp70C;(_tmp70C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp70D="too many arguments",_tag_dyneither(_tmp70D,sizeof(char),19))),_tag_dyneither(_tmp70C,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp711;void*_tmp710[1];struct Cyc_String_pa_PrintArg_struct _tmp70F;(_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp710[0]=& _tmp70F,Cyc_Tcutil_terr(loc,((_tmp711="expecting arithmetic type but found %s",_tag_dyneither(_tmp711,sizeof(char),39))),_tag_dyneither(_tmp710,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp715;void*_tmp714[1];struct Cyc_String_pa_PrintArg_struct _tmp713;(_tmp713.tag=0,((_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp714[0]=& _tmp713,Cyc_Tcutil_terr(loc,((_tmp715="expecting integral or * type but found %s",_tag_dyneither(_tmp715,sizeof(char),42))),_tag_dyneither(_tmp714,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp719;void*_tmp718[1];struct Cyc_String_pa_PrintArg_struct _tmp717;(_tmp717.tag=0,((_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp718[0]=& _tmp717,Cyc_Tcutil_terr(loc,((_tmp719="expecting integral type but found %s",_tag_dyneither(_tmp719,sizeof(char),37))),_tag_dyneither(_tmp718,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF3=t;union Cyc_Absyn_Constraint*_tmpF6;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3;if(_tmpF4->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3;if(_tmpF5->tag != 5)goto _LL9B;else{_tmpF6=((_tmpF5->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_stmttmpD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF6);void*_tmpF7=_stmttmpD;struct Cyc_Absyn_Exp*_tmpFA;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpF8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF8->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF9->tag != 1)goto _LL9D;else{_tmpFA=_tmpF9->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFA)){
const char*_tmp71C;void*_tmp71B;(_tmp71B=0,Cyc_Tcutil_terr(loc,((_tmp71C="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp71C,sizeof(char),55))),_tag_dyneither(_tmp71B,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp720;void*_tmp71F[1];struct Cyc_String_pa_PrintArg_struct _tmp71E;(_tmp71E.tag=0,((_tmp71E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp71F[0]=& _tmp71E,Cyc_Tcutil_terr(loc,((_tmp720="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp720,sizeof(char),47))),_tag_dyneither(_tmp71F,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp723;void*_tmp722;(_tmp722=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp723="Non-unary primop",_tag_dyneither(_tmp723,sizeof(char),17))),_tag_dyneither(_tmp722,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp727;void*_tmp726[1];struct Cyc_String_pa_PrintArg_struct _tmp725;(_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp726[0]=& _tmp725,Cyc_Tcutil_terr(e1->loc,((_tmp727="type %s cannot be used here",_tag_dyneither(_tmp727,sizeof(char),28))),_tag_dyneither(_tmp726,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp72B;void*_tmp72A[1];struct Cyc_String_pa_PrintArg_struct _tmp729;(_tmp729.tag=0,((_tmp729.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp72A[0]=& _tmp729,Cyc_Tcutil_terr(e2->loc,((_tmp72B="type %s cannot be used here",_tag_dyneither(_tmp72B,sizeof(char),28))),_tag_dyneither(_tmp72A,sizeof(void*),1)))))));}
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
const char*_tmp72E;void*_tmp72D;(_tmp72D=0,Cyc_Tcutil_terr(e1->loc,((_tmp72E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp72E,sizeof(char),50))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp731;void*_tmp730;(_tmp730=0,Cyc_Tcutil_terr(e1->loc,((_tmp731="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp731,sizeof(char),54))),_tag_dyneither(_tmp730,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp735;void*_tmp734[1];struct Cyc_String_pa_PrintArg_struct _tmp733;(_tmp733.tag=0,((_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp734[0]=& _tmp733,Cyc_Tcutil_terr(e2->loc,((_tmp735="expecting int but found %s",_tag_dyneither(_tmp735,sizeof(char),27))),_tag_dyneither(_tmp734,sizeof(void*),1)))))));}{
void*_stmttmpE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp10E);void*_tmp117=_stmttmpE;struct Cyc_Absyn_Exp*_tmp11A;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp118=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp117;if(_tmp118->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp117;if(_tmp119->tag != 1)goto _LLA8;else{_tmp11A=_tmp119->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10F)){
const char*_tmp738;void*_tmp737;(_tmp737=0,Cyc_Tcutil_warn(e1->loc,((_tmp738="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp738,sizeof(char),70))),_tag_dyneither(_tmp737,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp73E;struct Cyc_Absyn_PtrInfo _tmp73D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11D=(_tmp73C=_cycalloc(sizeof(*_tmp73C)),((_tmp73C[0]=((_tmp73E.tag=5,((_tmp73E.f1=((_tmp73D.elt_typ=_tmp10A,((_tmp73D.elt_tq=_tmp10B,((_tmp73D.ptr_atts=(
((_tmp73D.ptr_atts).rgn=_tmp10C,(((_tmp73D.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp73D.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp73D.ptr_atts).zero_term=_tmp10F,(((_tmp73D.ptr_atts).ptrloc=0,_tmp73D.ptr_atts)))))))))),_tmp73D)))))),_tmp73E)))),_tmp73C)));
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
{const char*_tmp743;void*_tmp742[2];struct Cyc_String_pa_PrintArg_struct _tmp741;struct Cyc_String_pa_PrintArg_struct _tmp740;(_tmp740.tag=0,((_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp741.tag=0,((_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp742[0]=& _tmp741,((_tmp742[1]=& _tmp740,Cyc_Tcutil_terr(e1->loc,((_tmp743="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp743,sizeof(char),59))),_tag_dyneither(_tmp742,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp748;void*_tmp747[2];struct Cyc_String_pa_PrintArg_struct _tmp746;struct Cyc_String_pa_PrintArg_struct _tmp745;(_tmp745.tag=0,((_tmp745.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp747[0]=& _tmp746,((_tmp747[1]=& _tmp745,Cyc_Tcutil_terr(e1->loc,((_tmp748="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp748,sizeof(char),59))),_tag_dyneither(_tmp747,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_warn(e1->loc,((_tmp74B="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp74B,sizeof(char),60))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_terr(e1->loc,((_tmp74E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp74E,sizeof(char),50))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp751;void*_tmp750;(_tmp750=0,Cyc_Tcutil_terr(e1->loc,((_tmp751="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp751,sizeof(char),54))),_tag_dyneither(_tmp750,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp756;void*_tmp755[2];struct Cyc_String_pa_PrintArg_struct _tmp754;struct Cyc_String_pa_PrintArg_struct _tmp753;(_tmp753.tag=0,((_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp754.tag=0,((_tmp754.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp755[0]=& _tmp754,((_tmp755[1]=& _tmp753,Cyc_Tcutil_terr(e2->loc,((_tmp756="expecting either %s or int but found %s",_tag_dyneither(_tmp756,sizeof(char),40))),_tag_dyneither(_tmp755,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp759;void*_tmp758;(_tmp758=0,Cyc_Tcutil_warn(e1->loc,((_tmp759="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp759,sizeof(char),60))),_tag_dyneither(_tmp758,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp75C;void*_tmp75B;(_tmp75B=0,Cyc_Tcutil_warn(e1->loc,((_tmp75C="thin pointer subtraction!",_tag_dyneither(_tmp75C,sizeof(char),26))),_tag_dyneither(_tmp75B,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp75F;void*_tmp75E;(_tmp75E=0,Cyc_Tcutil_warn(e1->loc,((_tmp75F="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp75F,sizeof(char),56))),_tag_dyneither(_tmp75E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp762;void*_tmp761;(_tmp761=0,Cyc_Tcutil_warn(e1->loc,((_tmp762="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp762,sizeof(char),51))),_tag_dyneither(_tmp761,sizeof(void*),0)));}
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
struct _tuple0 _tmp763;struct _tuple0 _stmttmpF=(_tmp763.f1=Cyc_Tcutil_compress(t1),((_tmp763.f2=Cyc_Tcutil_compress(t2),_tmp763)));struct _tuple0 _tmp13B=_stmttmpF;void*_tmp13D;void*_tmp13F;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B.f1;if(_tmp13C->tag != 5)goto _LLB0;else{_tmp13D=(_tmp13C->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B.f2;if(_tmp13E->tag != 5)goto _LLB0;else{_tmp13F=(_tmp13E->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp13D,_tmp13F))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13B.f1;if(_tmp140->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13B.f2;if(_tmp141->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp768;void*_tmp767[2];struct Cyc_String_pa_PrintArg_struct _tmp766;struct Cyc_String_pa_PrintArg_struct _tmp765;(_tmp765.tag=0,((_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp767[0]=& _tmp766,((_tmp767[1]=& _tmp765,Cyc_Tcutil_terr(loc,((_tmp768="comparison not allowed between %s and %s",_tag_dyneither(_tmp768,sizeof(char),41))),_tag_dyneither(_tmp767,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp769;struct _tuple0 _stmttmp10=(_tmp769.f1=t1,((_tmp769.f2=t2,_tmp769)));struct _tuple0 _tmp147=_stmttmp10;void*_tmp149;void*_tmp14B;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp147.f1;if(_tmp148->tag != 15)goto _LLB7;else{_tmp149=(void*)_tmp148->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp147.f2;if(_tmp14A->tag != 15)goto _LLB7;else{_tmp14B=(void*)_tmp14A->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp76E;void*_tmp76D[2];struct Cyc_String_pa_PrintArg_struct _tmp76C;struct Cyc_String_pa_PrintArg_struct _tmp76B;(_tmp76B.tag=0,((_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp76C.tag=0,((_tmp76C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp76D[0]=& _tmp76C,((_tmp76D[1]=& _tmp76B,Cyc_Tcutil_terr(loc,((_tmp76E="comparison not allowed between %s and %s",_tag_dyneither(_tmp76E,sizeof(char),41))),_tag_dyneither(_tmp76D,sizeof(void*),2)))))))))))));}
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
const char*_tmp771;void*_tmp770;(_tmp770=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp771="bad binary primop",_tag_dyneither(_tmp771,sizeof(char),18))),_tag_dyneither(_tmp770,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _stmttmp11=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_stmttmp11){case 0: _LLCC: {
const char*_tmp774;void*_tmp773;return(_tmp773=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp774="primitive operator has 0 arguments",_tag_dyneither(_tmp774,sizeof(char),35))),_tag_dyneither(_tmp773,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp777;void*_tmp776;return(_tmp776=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp777="primitive operator has > 2 arguments",_tag_dyneither(_tmp777,sizeof(char),37))),_tag_dyneither(_tmp776,sizeof(void*),0)));}}}
# 716
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
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
{const char*_tmp77B;void*_tmp77A[1];struct Cyc_String_pa_PrintArg_struct _tmp779;(_tmp779.tag=0,((_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp164->name),((_tmp77A[0]=& _tmp779,Cyc_Tcutil_terr(loc,((_tmp77B="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp77B,sizeof(char),56))),_tag_dyneither(_tmp77A,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp164->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp158;if(_tmp15D->tag != 4)goto _LLD8;else{if((((_tmp15D->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp15E=((struct _tuple2)(((_tmp15D->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp168=_tmp15E->typs;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
struct _tuple18*_stmttmp13=(struct _tuple18*)_tmp168->hd;struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;struct _tuple18*_tmp169=_stmttmp13;_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;
if(_tmp16A.real_const){
{const char*_tmp77F;void*_tmp77E[1];struct Cyc_String_pa_PrintArg_struct _tmp77D;(_tmp77D.tag=0,((_tmp77D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name)),((_tmp77E[0]=& _tmp77D,Cyc_Tcutil_terr(loc,((_tmp77F="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp77F,sizeof(char),64))),_tag_dyneither(_tmp77E,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16B))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp15F->tag != 8)goto _LLDA;else{_tmp160=(_tmp15F->f1).elt_type;_tmp161=(_tmp15F->f1).tq;}}_LLD9:
# 745
 if(_tmp161.real_const){
{const char*_tmp782;void*_tmp781;(_tmp781=0,Cyc_Tcutil_terr(loc,((_tmp782="attempt to over-write a const array",_tag_dyneither(_tmp782,sizeof(char),36))),_tag_dyneither(_tmp781,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp160);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp158;if(_tmp162->tag != 10)goto _LLDC;else{_tmp163=_tmp162->f1;}}_LLDB:
# 751
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct _tuple18*_stmttmp12=(struct _tuple18*)_tmp163->hd;struct Cyc_Absyn_Tqual _tmp172;void*_tmp173;struct _tuple18*_tmp171=_stmttmp12;_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;
if(_tmp172.real_const){
{const char*_tmp785;void*_tmp784;(_tmp784=0,Cyc_Tcutil_terr(loc,((_tmp785="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp785,sizeof(char),56))),_tag_dyneither(_tmp784,sizeof(void*),0)));}
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
{void*_stmttmp14=e->r;void*_tmp176=_stmttmp14;struct Cyc_Absyn_Vardecl*_tmp179;struct Cyc_Absyn_Vardecl*_tmp17C;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Exp*_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*_tmp187;struct _dyneither_ptr*_tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct _dyneither_ptr*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp177->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp178=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp177->f2);if(_tmp178->tag != 3)goto _LLE1;else{_tmp179=_tmp178->f1;}}}_LLE0:
 _tmp17C=_tmp179;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp17A->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp17B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp17A->f2);if(_tmp17B->tag != 4)goto _LLE3;else{_tmp17C=_tmp17B->f1;}}}_LLE2:
 _tmp17F=_tmp17C;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp17D->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp17E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp17D->f2);if(_tmp17E->tag != 5)goto _LLE5;else{_tmp17F=_tmp17E->f1;}}}_LLE4:
 _tmp182=_tmp17F;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp180->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp181=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp180->f2);if(_tmp181->tag != 1)goto _LLE7;else{_tmp182=_tmp181->f1;}}}_LLE6:
 if(!(_tmp182->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp183->tag != 22)goto _LLE9;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;}}_LLE8:
# 777
{void*_stmttmp19=Cyc_Tcutil_compress((void*)_check_null(_tmp184->topt));void*_tmp192=_stmttmp19;struct Cyc_Absyn_Tqual _tmp194;struct Cyc_Absyn_Tqual _tmp196;struct Cyc_List_List*_tmp198;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp192;if(_tmp193->tag != 5)goto _LLF8;else{_tmp194=(_tmp193->f1).elt_tq;}}_LLF7:
 _tmp196=_tmp194;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp195=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192;if(_tmp195->tag != 8)goto _LLFA;else{_tmp196=(_tmp195->f1).tq;}}_LLF9:
 if(!_tmp196.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp192;if(_tmp197->tag != 10)goto _LLFC;else{_tmp198=_tmp197->f1;}}_LLFB: {
# 781
struct _tuple15 _stmttmp1A=Cyc_Evexp_eval_const_uint_exp(_tmp185);unsigned int _tmp19A;int _tmp19B;struct _tuple15 _tmp199=_stmttmp1A;_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;
if(!_tmp19B){
{const char*_tmp788;void*_tmp787;(_tmp787=0,Cyc_Tcutil_terr(e->loc,((_tmp788="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp788,sizeof(char),47))),_tag_dyneither(_tmp787,sizeof(void*),0)));}
return;}
# 786
{struct _handler_cons _tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!_tmp1A0){
{struct _tuple18*_stmttmp1B=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp198,(int)_tmp19A);struct Cyc_Absyn_Tqual _tmp1A2;struct _tuple18*_tmp1A1=_stmttmp1B;_tmp1A2=_tmp1A1->f1;
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
{void*_stmttmp18=Cyc_Tcutil_compress((void*)_check_null(_tmp187->topt));void*_tmp1A7=_stmttmp18;struct Cyc_Absyn_Aggrdecl**_tmp1A9;struct Cyc_List_List*_tmp1AB;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A7;if(_tmp1A8->tag != 11)goto _LL106;else{if((((_tmp1A8->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1A9=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1A8->f1).aggr_info).KnownAggr).val;}}_LL105: {
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
{void*_stmttmp16=Cyc_Tcutil_compress((void*)_check_null(_tmp18A->topt));void*_tmp1AC=_stmttmp16;void*_tmp1AE;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC;if(_tmp1AD->tag != 5)goto _LL10D;else{_tmp1AE=(_tmp1AD->f1).elt_typ;}}_LL10C:
# 811
{void*_stmttmp17=Cyc_Tcutil_compress(_tmp1AE);void*_tmp1AF=_stmttmp17;struct Cyc_Absyn_Aggrdecl**_tmp1B1;struct Cyc_List_List*_tmp1B3;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AF;if(_tmp1B0->tag != 11)goto _LL112;else{if((((_tmp1B0->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1B1=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1B0->f1).aggr_info).KnownAggr).val;}}_LL111: {
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
{void*_stmttmp15=Cyc_Tcutil_compress((void*)_check_null(_tmp18D->topt));void*_tmp1B4=_stmttmp15;struct Cyc_Absyn_Tqual _tmp1B6;struct Cyc_Absyn_Tqual _tmp1B8;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B4;if(_tmp1B5->tag != 5)goto _LL119;else{_tmp1B6=(_tmp1B5->f1).elt_tq;}}_LL118:
 _tmp1B8=_tmp1B6;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4;if(_tmp1B7->tag != 8)goto _LL11B;else{_tmp1B8=(_tmp1B7->f1).tq;}}_LL11A:
 if(!_tmp1B8.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 833
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp18E->tag != 11)goto _LLF1;else{_tmp18F=_tmp18E->f1;}}_LLF0:
 _tmp191=_tmp18F;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp176;if(_tmp190->tag != 12)goto _LLF3;else{_tmp191=_tmp190->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp191);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 838
const char*_tmp78C;void*_tmp78B[1];struct Cyc_String_pa_PrintArg_struct _tmp78A;(_tmp78A.tag=0,((_tmp78A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp78B[0]=& _tmp78A,Cyc_Tcutil_terr(e->loc,((_tmp78C="attempt to write a const location: %s",_tag_dyneither(_tmp78C,sizeof(char),38))),_tag_dyneither(_tmp78B,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1BC=_new_region("temp");struct _RegionHandle*temp=& _tmp1BC;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp78F;void*_tmp78E;void*_tmp1BF=(_tmp78E=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78F="increment/decrement of non-lvalue",_tag_dyneither(_tmp78F,sizeof(char),34))),_tag_dyneither(_tmp78E,sizeof(void*),0)));_npop_handler(0);return _tmp1BF;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp792;void*_tmp791;(_tmp791=0,Cyc_Tcutil_terr(e->loc,((_tmp792="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp792,sizeof(char),50))),_tag_dyneither(_tmp791,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_Tcutil_terr(e->loc,((_tmp795="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp795,sizeof(char),54))),_tag_dyneither(_tmp794,sizeof(void*),0)));}}else{
# 861
const char*_tmp799;void*_tmp798[1];struct Cyc_String_pa_PrintArg_struct _tmp797;(_tmp797.tag=0,((_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp798[0]=& _tmp797,Cyc_Tcutil_terr(e->loc,((_tmp799="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp799,sizeof(char),44))),_tag_dyneither(_tmp798,sizeof(void*),1)))))));}}{
# 863
void*_tmp1C7=t;_npop_handler(0);return _tmp1C7;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _RegionHandle _tmp1C8=_new_region("r");struct _RegionHandle*r=& _tmp1C8;_push_region(r);{
const char*_tmp79A;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp79A="conditional expression",_tag_dyneither(_tmp79A,sizeof(char),23))));}
# 872
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 878
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp79B;struct Cyc_List_List _tmp1CA=(_tmp79B.hd=e3,((_tmp79B.tl=0,_tmp79B)));
struct Cyc_List_List _tmp79C;struct Cyc_List_List _tmp1CB=(_tmp79C.hd=e2,((_tmp79C.tl=& _tmp1CA,_tmp79C)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1CB)){
{const char*_tmp7A1;void*_tmp7A0[2];struct Cyc_String_pa_PrintArg_struct _tmp79F;struct Cyc_String_pa_PrintArg_struct _tmp79E;(_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7A0[0]=& _tmp79F,((_tmp7A0[1]=& _tmp79E,Cyc_Tcutil_terr(loc,((_tmp7A1="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7A1,sizeof(char),48))),_tag_dyneither(_tmp7A0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 886
void*_tmp1D0=t;_npop_handler(0);return _tmp1D0;};};};
# 869
;_pop_region(r);}
# 890
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 892
{const char*_tmp7A2;Cyc_Tcexp_tcTest(te,e1,((_tmp7A2="logical-and expression",_tag_dyneither(_tmp7A2,sizeof(char),23))));}
{const char*_tmp7A3;Cyc_Tcexp_tcTest(te,e2,((_tmp7A3="logical-and expression",_tag_dyneither(_tmp7A3,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 898
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 900
{const char*_tmp7A4;Cyc_Tcexp_tcTest(te,e1,((_tmp7A4="logical-or expression",_tag_dyneither(_tmp7A4,sizeof(char),22))));}
{const char*_tmp7A5;Cyc_Tcexp_tcTest(te,e2,((_tmp7A5="logical-or expression",_tag_dyneither(_tmp7A5,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 906
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 913
struct _RegionHandle _tmp1D7=_new_region("r");struct _RegionHandle*r=& _tmp1D7;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 916
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 920
{void*_stmttmp1C=Cyc_Tcutil_compress(t1);void*_tmp1D8=_stmttmp1C;_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1D9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D8;if(_tmp1D9->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp7A8;void*_tmp7A7;(_tmp7A7=0,Cyc_Tcutil_terr(loc,((_tmp7A8="cannot assign to an array",_tag_dyneither(_tmp7A8,sizeof(char),26))),_tag_dyneither(_tmp7A7,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 925
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7AB;void*_tmp7AA;(_tmp7AA=0,Cyc_Tcutil_terr(loc,((_tmp7AB="type is abstract (can't determine size).",_tag_dyneither(_tmp7AB,sizeof(char),41))),_tag_dyneither(_tmp7AA,sizeof(void*),0)));}
# 929
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp7AE;void*_tmp7AD;void*_tmp1E0=(_tmp7AD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7AE="assignment to non-lvalue",_tag_dyneither(_tmp7AE,sizeof(char),25))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));_npop_handler(0);return _tmp1E0;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_Tcutil_terr(e2->loc,((_tmp7B1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7B1,sizeof(char),49))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));}
# 937
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7B6;void*_tmp7B5[2];struct Cyc_String_pa_PrintArg_struct _tmp7B4;struct Cyc_String_pa_PrintArg_struct _tmp7B3;void*_tmp1E4=(_tmp7B3.tag=0,((_tmp7B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B4.tag=0,((_tmp7B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B5[0]=& _tmp7B4,((_tmp7B5[1]=& _tmp7B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B6="type mismatch: %s != %s",_tag_dyneither(_tmp7B6,sizeof(char),24))),_tag_dyneither(_tmp7B5,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1E5=_tmp1E4;_npop_handler(1);return _tmp1E5;};}
# 933
;_pop_region(temp);}else{
# 945
enum Cyc_Absyn_Primop _tmp1EA=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1EB=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EA,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1EB,t1) || Cyc_Tcutil_coerceable(_tmp1EB) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7BB;void*_tmp7BA[2];struct Cyc_String_pa_PrintArg_struct _tmp7B9;struct Cyc_String_pa_PrintArg_struct _tmp7B8;void*_tmp1EC=
(_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 951
Cyc_Absynpp_typ2string(t2)),((_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 950
Cyc_Absynpp_typ2string(t1)),((_tmp7BA[0]=& _tmp7B9,((_tmp7BA[1]=& _tmp7B8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7BB="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7BB,sizeof(char),82))),_tag_dyneither(_tmp7BA,sizeof(void*),2)))))))))))));
# 952
Cyc_Tcutil_unify(_tmp1EB,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1ED=_tmp1EC;_npop_handler(0);return _tmp1ED;};}{
# 956
void*_tmp1F2=_tmp1EB;_npop_handler(0);return _tmp1F2;};}{
# 958
void*_tmp1F3=t1;_npop_handler(0);return _tmp1F3;};};};
# 913
;_pop_region(r);}
# 962
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp1F4=_new_region("r");struct _RegionHandle*r=& _tmp1F4;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 967
void*_tmp1F5=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp1F5;};
# 963
;_pop_region(r);}
# 971
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 975
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 978
{void*_stmttmp1D=Cyc_Tcutil_compress(t1);void*_tmp1F6=_stmttmp1D;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1F6;if(_tmp1F7->tag != 7)goto _LL125;else{if(_tmp1F7->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F6;if(_tmp1F8->tag != 6)goto _LL127;else{if(_tmp1F8->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1F6;if(_tmp1F9->tag != 6)goto _LL129;else{if(_tmp1F9->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 985
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _stmttmp1E=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp1FB;struct Cyc_List_List*_tmp1FC;unsigned int _tmp1FD;enum Cyc_Absyn_Scope _tmp1FE;struct Cyc_Absyn_Datatypefield _tmp1FA=_stmttmp1E;_tmp1FB=_tmp1FA.name;_tmp1FC=_tmp1FA.typs;_tmp1FD=_tmp1FA.loc;_tmp1FE=_tmp1FA.sc;
# 988
if(_tmp1FC == 0  || _tmp1FC->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp1FC->hd)).f2);
# 991
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _stmttmp1F=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp200;struct Cyc_List_List*_tmp201;unsigned int _tmp202;enum Cyc_Absyn_Scope _tmp203;struct Cyc_Absyn_Datatypefield _tmp1FF=_stmttmp1F;_tmp200=_tmp1FF.name;_tmp201=_tmp1FF.typs;_tmp202=_tmp1FF.loc;_tmp203=_tmp1FF.sc;
# 999
if(_tmp201 == 0  || _tmp201->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp201->hd)).f2);
# 1003
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1008
{const char*_tmp7C0;void*_tmp7BF[2];struct Cyc_String_pa_PrintArg_struct _tmp7BE;struct Cyc_String_pa_PrintArg_struct _tmp7BD;(_tmp7BD.tag=0,((_tmp7BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7BE.tag=0,((_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7BF[0]=& _tmp7BE,((_tmp7BF[1]=& _tmp7BD,Cyc_Tcutil_terr(e->loc,((_tmp7C0="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7C0,sizeof(char),54))),_tag_dyneither(_tmp7BF,sizeof(void*),2)))))))))))));}
return 0;}
# 1014
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1020
struct Cyc_List_List*_tmp208=args;
int _tmp209=0;
struct _RegionHandle _tmp20A=_new_region("ter");struct _RegionHandle*ter=& _tmp20A;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp20B=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp20C=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp20B);
Cyc_Tcexp_tcExp(_tmp20C,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1030
void*_tmp20D=t;void*_tmp20F;struct Cyc_Absyn_Tqual _tmp210;void*_tmp211;union Cyc_Absyn_Constraint*_tmp212;union Cyc_Absyn_Constraint*_tmp213;union Cyc_Absyn_Constraint*_tmp214;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20D;if(_tmp20E->tag != 5)goto _LL12E;else{_tmp20F=(_tmp20E->f1).elt_typ;_tmp210=(_tmp20E->f1).elt_tq;_tmp211=((_tmp20E->f1).ptr_atts).rgn;_tmp212=((_tmp20E->f1).ptr_atts).nullable;_tmp213=((_tmp20E->f1).ptr_atts).bounds;_tmp214=((_tmp20E->f1).ptr_atts).zero_term;}}_LL12D:
# 1035
 Cyc_Tcenv_check_rgn_accessible(_tmp20C,loc,_tmp211);
# 1037
Cyc_Tcutil_check_nonzero_bound(loc,_tmp213);{
void*_stmttmp20=Cyc_Tcutil_compress(_tmp20F);void*_tmp215=_stmttmp20;struct Cyc_List_List*_tmp217;void*_tmp218;struct Cyc_Absyn_Tqual _tmp219;void*_tmp21A;struct Cyc_List_List*_tmp21B;int _tmp21C;struct Cyc_Absyn_VarargInfo*_tmp21D;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21F;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp215;if(_tmp216->tag != 9)goto _LL133;else{_tmp217=(_tmp216->f1).tvars;_tmp218=(_tmp216->f1).effect;_tmp219=(_tmp216->f1).ret_tqual;_tmp21A=(_tmp216->f1).ret_typ;_tmp21B=(_tmp216->f1).args;_tmp21C=(_tmp216->f1).c_varargs;_tmp21D=(_tmp216->f1).cyc_varargs;_tmp21E=(_tmp216->f1).rgn_po;_tmp21F=(_tmp216->f1).attributes;}}_LL132:
# 1041
 if(_tmp217 != 0  || _tmp21E != 0){
const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_Tcutil_terr(e->loc,((_tmp7C3="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp7C3,sizeof(char),79))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}
# 1045
if(topt != 0)Cyc_Tcutil_unify(_tmp21A,*topt);
# 1047
while(_tmp208 != 0  && _tmp21B != 0){
# 1049
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp208->hd;
void*t2=(*((struct _tuple9*)_tmp21B->hd)).f3;
Cyc_Tcexp_tcExp(_tmp20C,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20C,e1,t2,& alias_coercion)){
{const char*_tmp7C8;void*_tmp7C7[2];struct Cyc_String_pa_PrintArg_struct _tmp7C6;struct Cyc_String_pa_PrintArg_struct _tmp7C5;(_tmp7C5.tag=0,((_tmp7C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1057
Cyc_Absynpp_typ2string(t2)),((_tmp7C6.tag=0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7C7[0]=& _tmp7C6,((_tmp7C7[1]=& _tmp7C5,Cyc_Tcutil_terr(e1->loc,((_tmp7C8="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp7C8,sizeof(char),57))),_tag_dyneither(_tmp7C7,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1062
if(alias_coercion){
struct Cyc_List_List*_tmp7C9;*alias_arg_exps=((_tmp7C9=_cycalloc(sizeof(*_tmp7C9)),((_tmp7C9->hd=(void*)_tmp209,((_tmp7C9->tl=*alias_arg_exps,_tmp7C9))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7CC;void*_tmp7CB;(_tmp7CB=0,Cyc_Tcutil_terr(e1->loc,((_tmp7CC="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7CC,sizeof(char),49))),_tag_dyneither(_tmp7CB,sizeof(void*),0)));}
_tmp208=_tmp208->tl;
_tmp21B=_tmp21B->tl;
++ _tmp209;}{
# 1073
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp21F;for(0;a != 0;a=a->tl){
void*_stmttmp21=(void*)a->hd;void*_tmp229=_stmttmp21;enum Cyc_Absyn_Format_Type _tmp22B;int _tmp22C;int _tmp22D;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp22A=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp229;if(_tmp22A->tag != 19)goto _LL138;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;_tmp22D=_tmp22A->f3;}}_LL137:
# 1077
{struct _handler_cons _tmp22E;_push_handler(& _tmp22E);{int _tmp230=0;if(setjmp(_tmp22E.handler))_tmp230=1;if(!_tmp230){
# 1079
{struct Cyc_Absyn_Exp*_tmp231=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp22C - 1);
# 1081
struct Cyc_Core_Opt*fmt_args;
if(_tmp22D == 0)
fmt_args=0;else{
# 1085
struct Cyc_Core_Opt*_tmp7CD;fmt_args=((_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp22D - 1),_tmp7CD))));}
args_already_checked=1;{
struct _RegionHandle _tmp233=_new_region("temp");struct _RegionHandle*temp=& _tmp233;_push_region(temp);
switch(_tmp22B){case Cyc_Absyn_Printf_ft: _LL13A:
# 1090
 Cyc_Tcexp_check_format_args(_tmp20C,_tmp231,fmt_args,_tmp22D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1093
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1095
 Cyc_Tcexp_check_format_args(_tmp20C,_tmp231,fmt_args,_tmp22D - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1098
break;}
# 1088
;_pop_region(temp);};}
# 1079
;_pop_handler();}else{void*_tmp22F=(void*)_exn_thrown;void*_tmp235=_tmp22F;void*_tmp237;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp236=(struct Cyc_List_Nth_exn_struct*)_tmp235;if(_tmp236->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1102
{const char*_tmp7D0;void*_tmp7CF;(_tmp7CF=0,Cyc_Tcutil_terr(loc,((_tmp7D0="bad format arguments",_tag_dyneither(_tmp7D0,sizeof(char),21))),_tag_dyneither(_tmp7CF,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp237=_tmp235;_LL141:(void)_throw(_tmp237);_LL13D:;}};}
# 1104
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1108
if(_tmp21B != 0){const char*_tmp7D3;void*_tmp7D2;(_tmp7D2=0,Cyc_Tcutil_terr(loc,((_tmp7D3="too few arguments for function",_tag_dyneither(_tmp7D3,sizeof(char),31))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}else{
# 1110
if((_tmp208 != 0  || _tmp21C) || _tmp21D != 0){
if(_tmp21C)
for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
Cyc_Tcexp_tcExp(_tmp20C,0,(struct Cyc_Absyn_Exp*)_tmp208->hd);}else{
if(_tmp21D == 0){
const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(loc,((_tmp7D6="too many arguments for function",_tag_dyneither(_tmp7D6,sizeof(char),32))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}else{
# 1117
struct Cyc_Absyn_VarargInfo _stmttmp22=*_tmp21D;void*_tmp23F;int _tmp240;struct Cyc_Absyn_VarargInfo _tmp23E=_stmttmp22;_tmp23F=_tmp23E.type;_tmp240=_tmp23E.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7D7;struct Cyc_Absyn_VarargCallInfo*_tmp241=(_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7->num_varargs=0,((_tmp7D7->injectors=0,((_tmp7D7->vai=_tmp21D,_tmp7D7)))))));
# 1121
*vararg_call_info=_tmp241;
# 1123
if(!_tmp240)
# 1125
for(0;_tmp208 != 0;(_tmp208=_tmp208->tl,_tmp209 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp208->hd;
++ _tmp241->num_varargs;
Cyc_Tcexp_tcExp(_tmp20C,& _tmp23F,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp20C,e1,_tmp23F,& alias_coercion)){
{const char*_tmp7DC;void*_tmp7DB[2];struct Cyc_String_pa_PrintArg_struct _tmp7DA;struct Cyc_String_pa_PrintArg_struct _tmp7D9;(_tmp7D9.tag=0,((_tmp7D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7DB[0]=& _tmp7DA,((_tmp7DB[1]=& _tmp7D9,Cyc_Tcutil_terr(loc,((_tmp7DC="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7DC,sizeof(char),49))),_tag_dyneither(_tmp7DB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1135
if(alias_coercion){
struct Cyc_List_List*_tmp7DD;*alias_arg_exps=((_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD->hd=(void*)_tmp209,((_tmp7DD->tl=*alias_arg_exps,_tmp7DD))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23F) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7E0;void*_tmp7DF;(_tmp7DF=0,Cyc_Tcutil_terr(e1->loc,((_tmp7E0="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7E0,sizeof(char),49))),_tag_dyneither(_tmp7DF,sizeof(void*),0)));}}else{
# 1144
void*_stmttmp23=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23F));void*_tmp249=_stmttmp23;struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_List_List*_tmp24C;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 3)goto _LL145;else{if((((_tmp24A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp24B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp24A->f1).datatype_info).KnownDatatype).val);_tmp24C=(_tmp24A->f1).targs;}}_LL144: {
# 1148
struct Cyc_Absyn_Datatypedecl*_tmp24D=*Cyc_Tcenv_lookup_datatypedecl(_tmp20C,loc,_tmp24B->name);
struct Cyc_List_List*fields=0;
if(_tmp24D->fields == 0){
const char*_tmp7E4;void*_tmp7E3[1];struct Cyc_String_pa_PrintArg_struct _tmp7E2;(_tmp7E2.tag=0,((_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7E3[0]=& _tmp7E2,Cyc_Tcutil_terr(loc,((_tmp7E4="can't inject into abstract datatype %s",_tag_dyneither(_tmp7E4,sizeof(char),39))),_tag_dyneither(_tmp7E3,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24D->fields))->v;}
# 1160
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23F),Cyc_Tcenv_curr_rgn(_tmp20C))){
const char*_tmp7E9;void*_tmp7E8[2];struct Cyc_String_pa_PrintArg_struct _tmp7E7;struct Cyc_String_pa_PrintArg_struct _tmp7E6;(_tmp7E6.tag=0,((_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20C))),((_tmp7E7.tag=0,((_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7E8[0]=& _tmp7E7,((_tmp7E8[1]=& _tmp7E6,Cyc_Tcutil_terr(loc,((_tmp7E9="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7E9,sizeof(char),49))),_tag_dyneither(_tmp7E8,sizeof(void*),2)))))))))))));}{
# 1164
struct _RegionHandle _tmp255=_new_region("rgn");struct _RegionHandle*rgn=& _tmp255;_push_region(rgn);{
struct Cyc_List_List*_tmp256=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp24D->tvs,_tmp24C);
for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
++ _tmp241->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp208->hd;
# 1170
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp20C,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp7EC;void*_tmp7EB;(_tmp7EB=0,Cyc_Tcutil_terr(e1->loc,((_tmp7EC="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7EC,sizeof(char),49))),_tag_dyneither(_tmp7EB,sizeof(void*),0)));}}{
# 1176
struct Cyc_Absyn_Datatypefield*_tmp259=Cyc_Tcexp_tcInjection(_tmp20C,e1,Cyc_Tcutil_pointer_elt_type(_tmp23F),rgn,_tmp256,fields);
# 1178
if(_tmp259 != 0){
struct Cyc_List_List*_tmp7ED;_tmp241->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp241->injectors,(
(_tmp7ED=_cycalloc(sizeof(*_tmp7ED)),((_tmp7ED->hd=_tmp259,((_tmp7ED->tl=0,_tmp7ED)))))));}};};}}
# 1184
_npop_handler(0);goto _LL142;
# 1164
;_pop_region(rgn);};}_LL145:;_LL146:
# 1185
{const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,Cyc_Tcutil_terr(loc,((_tmp7F0="bad inject vararg type",_tag_dyneither(_tmp7F0,sizeof(char),23))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1190
if(*alias_arg_exps == 0)
# 1199 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp20C,loc,(void*)_check_null(_tmp218));{
# 1201
void*_tmp25E=_tmp21A;_npop_handler(0);return _tmp25E;};};_LL133:;_LL134: {
const char*_tmp7F3;void*_tmp7F2;void*_tmp261=(_tmp7F2=0,Cyc_Tcexp_expr_err(_tmp20C,loc,topt,((_tmp7F3="expected pointer to function",_tag_dyneither(_tmp7F3,sizeof(char),29))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));_npop_handler(0);return _tmp261;}_LL130:;};_LL12E:;_LL12F: {
# 1204
const char*_tmp7F6;void*_tmp7F5;void*_tmp264=(_tmp7F5=0,Cyc_Tcexp_expr_err(_tmp20C,loc,topt,((_tmp7F6="expected pointer to function",_tag_dyneither(_tmp7F6,sizeof(char),29))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));_npop_handler(0);return _tmp264;}_LL12B:;};}
# 1023 "tcexp.cyc"
;_pop_region(ter);}
# 1210 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp265=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp265,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp265,& bogus)){
const char*_tmp7FB;void*_tmp7FA[2];struct Cyc_String_pa_PrintArg_struct _tmp7F9;struct Cyc_String_pa_PrintArg_struct _tmp7F8;(_tmp7F8.tag=0,((_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1215
Cyc_Absynpp_typ2string(_tmp265)),((_tmp7FA[0]=& _tmp7F9,((_tmp7FA[1]=& _tmp7F8,Cyc_Tcutil_terr(loc,((_tmp7FB="expected %s but found %s",_tag_dyneither(_tmp7FB,sizeof(char),25))),_tag_dyneither(_tmp7FA,sizeof(void*),2)))))))))))));}
# 1217
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1222
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1224
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1226
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp26A=t1;void*_tmp26C;struct Cyc_Absyn_Tqual _tmp26D;void*_tmp26E;union Cyc_Absyn_Constraint*_tmp26F;union Cyc_Absyn_Constraint*_tmp270;union Cyc_Absyn_Constraint*_tmp271;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26A;if(_tmp26B->tag != 5)goto _LL14A;else{_tmp26C=(_tmp26B->f1).elt_typ;_tmp26D=(_tmp26B->f1).elt_tq;_tmp26E=((_tmp26B->f1).ptr_atts).rgn;_tmp26F=((_tmp26B->f1).ptr_atts).nullable;_tmp270=((_tmp26B->f1).ptr_atts).bounds;_tmp271=((_tmp26B->f1).ptr_atts).zero_term;}}_LL149:
# 1230
{void*_stmttmp24=Cyc_Tcutil_compress(_tmp26C);void*_tmp272=_stmttmp24;struct Cyc_List_List*_tmp274;void*_tmp275;struct Cyc_Absyn_Tqual _tmp276;void*_tmp277;struct Cyc_List_List*_tmp278;int _tmp279;struct Cyc_Absyn_VarargInfo*_tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27C;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp272;if(_tmp273->tag != 9)goto _LL14F;else{_tmp274=(_tmp273->f1).tvars;_tmp275=(_tmp273->f1).effect;_tmp276=(_tmp273->f1).ret_tqual;_tmp277=(_tmp273->f1).ret_typ;_tmp278=(_tmp273->f1).args;_tmp279=(_tmp273->f1).c_varargs;_tmp27A=(_tmp273->f1).cyc_varargs;_tmp27B=(_tmp273->f1).rgn_po;_tmp27C=(_tmp273->f1).attributes;}}_LL14E: {
# 1232
struct _RegionHandle _tmp27D=_new_region("temp");struct _RegionHandle*temp=& _tmp27D;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1235
for(0;ts != 0  && _tmp274 != 0;(ts=ts->tl,_tmp274=_tmp274->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp274->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp7FE;struct Cyc_List_List*_tmp7FD;instantiation=(
(_tmp7FD=_region_malloc(temp,sizeof(*_tmp7FD)),((_tmp7FD->hd=((_tmp7FE=_region_malloc(temp,sizeof(*_tmp7FE)),((_tmp7FE->f1=(struct Cyc_Absyn_Tvar*)_tmp274->hd,((_tmp7FE->f2=(void*)ts->hd,_tmp7FE)))))),((_tmp7FD->tl=instantiation,_tmp7FD))))));};}
# 1242
if(ts != 0){
const char*_tmp801;void*_tmp800;void*_tmp282=
(_tmp800=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp801="too many type variables in instantiation",_tag_dyneither(_tmp801,sizeof(char),41))),_tag_dyneither(_tmp800,sizeof(void*),0)));_npop_handler(0);return _tmp282;}
# 1248
if(_tmp274 == 0){
_tmp27B=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp27B);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp27B);
_tmp27B=0;}{
# 1253
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp807;struct Cyc_Absyn_FnInfo _tmp806;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp805;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp805=_cycalloc(sizeof(*_tmp805)),((_tmp805[0]=((_tmp807.tag=9,((_tmp807.f1=((_tmp806.tvars=_tmp274,((_tmp806.effect=_tmp275,((_tmp806.ret_tqual=_tmp276,((_tmp806.ret_typ=_tmp277,((_tmp806.args=_tmp278,((_tmp806.c_varargs=_tmp279,((_tmp806.cyc_varargs=_tmp27A,((_tmp806.rgn_po=_tmp27B,((_tmp806.attributes=_tmp27C,_tmp806)))))))))))))))))),_tmp807)))),_tmp805)))));
# 1257
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp80D;struct Cyc_Absyn_PtrInfo _tmp80C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80B;void*_tmp286=(void*)((_tmp80B=_cycalloc(sizeof(*_tmp80B)),((_tmp80B[0]=((_tmp80D.tag=5,((_tmp80D.f1=((_tmp80C.elt_typ=new_fn_typ,((_tmp80C.elt_tq=_tmp26D,((_tmp80C.ptr_atts=(((_tmp80C.ptr_atts).rgn=_tmp26E,(((_tmp80C.ptr_atts).nullable=_tmp26F,(((_tmp80C.ptr_atts).bounds=_tmp270,(((_tmp80C.ptr_atts).zero_term=_tmp271,(((_tmp80C.ptr_atts).ptrloc=0,_tmp80C.ptr_atts)))))))))),_tmp80C)))))),_tmp80D)))),_tmp80B))));_npop_handler(0);return _tmp286;};}
# 1233
;_pop_region(temp);}_LL14F:;_LL150:
# 1259
 goto _LL14C;_LL14C:;}
# 1261
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1264
const char*_tmp811;void*_tmp810[1];struct Cyc_String_pa_PrintArg_struct _tmp80F;return(_tmp80F.tag=0,((_tmp80F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp810[0]=& _tmp80F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp811="expecting polymorphic type but found %s",_tag_dyneither(_tmp811,sizeof(char),40))),_tag_dyneither(_tmp810,sizeof(void*),1)))))));};};}
# 1269
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1271
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1275
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1280
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1287
Cyc_Tcutil_unify(t2,t);{
const char*_tmp816;void*_tmp815[2];struct Cyc_String_pa_PrintArg_struct _tmp814;struct Cyc_String_pa_PrintArg_struct _tmp813;void*_tmp28D=(_tmp813.tag=0,((_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp814.tag=0,((_tmp814.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp815[0]=& _tmp814,((_tmp815[1]=& _tmp813,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp816="cannot cast %s to %s",_tag_dyneither(_tmp816,sizeof(char),21))),_tag_dyneither(_tmp815,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp28D;};}}}
# 1297
{struct _tuple0 _tmp817;struct _tuple0 _stmttmp25=(_tmp817.f1=e->r,((_tmp817.f2=Cyc_Tcutil_compress(t),_tmp817)));struct _tuple0 _tmp292=_stmttmp25;int _tmp294;union Cyc_Absyn_Constraint*_tmp296;union Cyc_Absyn_Constraint*_tmp297;union Cyc_Absyn_Constraint*_tmp298;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp293=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp292.f1;if(_tmp293->tag != 33)goto _LL154;else{_tmp294=(_tmp293->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp295=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292.f2;if(_tmp295->tag != 5)goto _LL154;else{_tmp296=((_tmp295->f1).ptr_atts).nullable;_tmp297=((_tmp295->f1).ptr_atts).bounds;_tmp298=((_tmp295->f1).ptr_atts).zero_term;}};_LL153:
# 1301
 if((_tmp294  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp298)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp296)){
void*_stmttmp26=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp297);void*_tmp299=_stmttmp26;struct Cyc_Absyn_Exp*_tmp29B;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp299;if(_tmp29A->tag != 1)goto _LL159;else{_tmp29B=_tmp29A->f1;}}_LL158:
# 1304
 if((Cyc_Evexp_eval_const_uint_exp(_tmp29B)).f1 == 1){
const char*_tmp81A;void*_tmp819;(_tmp819=0,Cyc_Tcutil_warn(loc,((_tmp81A="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp81A,sizeof(char),60))),_tag_dyneither(_tmp819,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1310
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1313
return t;};}
# 1317
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1319
void**_tmp29F=0;
struct Cyc_Absyn_Tqual _tmp2A0=Cyc_Absyn_empty_tqual(0);
int _tmp2A1=0;
if(topt != 0){
void*_stmttmp27=Cyc_Tcutil_compress(*topt);void*_tmp2A2=_stmttmp27;void**_tmp2A4;struct Cyc_Absyn_Tqual _tmp2A5;union Cyc_Absyn_Constraint*_tmp2A6;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A2;if(_tmp2A3->tag != 5)goto _LL15E;else{_tmp2A4=(void**)&(_tmp2A3->f1).elt_typ;_tmp2A5=(_tmp2A3->f1).elt_tq;_tmp2A6=((_tmp2A3->f1).ptr_atts).nullable;}}_LL15D:
# 1325
 _tmp29F=_tmp2A4;
_tmp2A0=_tmp2A5;
_tmp2A1=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2A6);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1339
struct _RegionHandle _tmp2A7=_new_region("r");struct _RegionHandle*r=& _tmp2A7;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp29F,e);
# 1343
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(e->loc,((_tmp81D="Cannot take the address of an alias-free path",_tag_dyneither(_tmp81D,sizeof(char),46))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}
# 1350
{void*_stmttmp28=e->r;void*_tmp2AA=_stmttmp28;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2AA;if(_tmp2AB->tag != 22)goto _LL163;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;}}_LL162:
# 1352
{void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp2AC->topt));void*_tmp2AE=_stmttmp29;_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2AF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2AE;if(_tmp2AF->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1357
 e0->r=(Cyc_Absyn_add_exp(_tmp2AC,_tmp2AD,0))->r;{
void*_tmp2B0=Cyc_Tcexp_tcPlus(te,_tmp2AC,_tmp2AD);_npop_handler(0);return _tmp2B0;};_LL165:;}
# 1360
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1365
{void*_stmttmp2A=e->r;void*_tmp2B1=_stmttmp2A;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2B2=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2B1;if(_tmp2B2->tag != 20)goto _LL16D;else{if(_tmp2B2->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2B1;if(_tmp2B3->tag != 21)goto _LL16F;else{if(_tmp2B3->f3 != 1)goto _LL16F;}}_LL16E:
# 1368
{const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(e->loc,((_tmp820="cannot take the address of a @tagged union member",_tag_dyneither(_tmp820,sizeof(char),50))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1374
struct _tuple14 _stmttmp2B=Cyc_Tcutil_addressof_props(te,e);int _tmp2B7;void*_tmp2B8;struct _tuple14 _tmp2B6=_stmttmp2B;_tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B8,0)){
const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(e->loc,((_tmp823="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp823,sizeof(char),60))),_tag_dyneither(_tmp822,sizeof(void*),0)));}{
# 1378
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2B7){
tq.print_const=1;
tq.real_const=1;}{
# 1384
void*t=_tmp2A1?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2B8,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2B8,tq,Cyc_Absyn_false_conref);
void*_tmp2BB=t;_npop_handler(0);return _tmp2BB;};};};
# 1339
;_pop_region(r);};}
# 1391
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1395
return Cyc_Absyn_uint_typ;
# 1397
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp827;void*_tmp826[1];struct Cyc_String_pa_PrintArg_struct _tmp825;(_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp826[0]=& _tmp825,Cyc_Tcutil_terr(loc,((_tmp827="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp827,sizeof(char),55))),_tag_dyneither(_tmp826,sizeof(void*),1)))))));}
if(topt != 0){
void*_stmttmp2C=Cyc_Tcutil_compress(*topt);void*_tmp2BF=_stmttmp2C;void*_tmp2C0;void*_tmp2C2;_LL172: _tmp2C0=_tmp2BF;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2C0;if(_tmp2C1->tag != 19)goto _LL174;else{_tmp2C2=(void*)_tmp2C1->f1;}};_LL173: {
# 1406
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp82A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp829;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C4=(_tmp829=_cycalloc(sizeof(*_tmp829)),((_tmp829[0]=((_tmp82A.tag=18,((_tmp82A.f1=_tmp2C3,_tmp82A)))),_tmp829)));
if(Cyc_Tcutil_unify(_tmp2C2,(void*)_tmp2C4))return _tmp2C0;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1412
return Cyc_Absyn_uint_typ;}
# 1415
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1422
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1424
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp2C7=fs;
void*_tmp2C8=t_orig;
for(0;_tmp2C7 != 0;_tmp2C7=_tmp2C7->tl){
void*_tmp2C9=(void*)_tmp2C7->hd;
void*_tmp2CA=_tmp2C9;struct _dyneither_ptr*_tmp2CC;unsigned int _tmp2CE;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2CB=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2CA;if(_tmp2CB->tag != 0)goto _LL179;else{_tmp2CC=_tmp2CB->f1;}}_LL178: {
# 1432
int bad_type=1;
{void*_stmttmp2E=Cyc_Tcutil_compress(_tmp2C8);void*_tmp2CF=_stmttmp2E;struct Cyc_Absyn_Aggrdecl**_tmp2D1;struct Cyc_List_List*_tmp2D3;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2D0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2CF;if(_tmp2D0->tag != 11)goto _LL17E;else{if((((_tmp2D0->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2D1=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2D0->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1435
 if((*_tmp2D1)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CC,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2D1)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp82E;void*_tmp82D[1];struct Cyc_String_pa_PrintArg_struct _tmp82C;(_tmp82C.tag=0,((_tmp82C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CC),((_tmp82D[0]=& _tmp82C,Cyc_Tcutil_terr(loc,((_tmp82E="no field of struct/union has name %s",_tag_dyneither(_tmp82E,sizeof(char),37))),_tag_dyneither(_tmp82D,sizeof(void*),1)))))));}else{
# 1440
_tmp2C8=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CF;if(_tmp2D2->tag != 12)goto _LL180;else{_tmp2D3=_tmp2D2->f2;}}_LL17F: {
# 1444
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CC,_tmp2D3);
if(!((unsigned int)t2)){
const char*_tmp832;void*_tmp831[1];struct Cyc_String_pa_PrintArg_struct _tmp830;(_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CC),((_tmp831[0]=& _tmp830,Cyc_Tcutil_terr(loc,((_tmp832="no field of struct/union has name %s",_tag_dyneither(_tmp832,sizeof(char),37))),_tag_dyneither(_tmp831,sizeof(void*),1)))))));}else{
# 1448
_tmp2C8=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1453
if(bad_type){
if(_tmp2C7 == fs){
const char*_tmp836;void*_tmp835[1];struct Cyc_String_pa_PrintArg_struct _tmp834;(_tmp834.tag=0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp835[0]=& _tmp834,Cyc_Tcutil_terr(loc,((_tmp836="%s is not a known struct/union type",_tag_dyneither(_tmp836,sizeof(char),36))),_tag_dyneither(_tmp835,sizeof(void*),1)))))));}else{
# 1457
const char*_tmp83A;void*_tmp839[1];struct Cyc_String_pa_PrintArg_struct _tmp838;struct _dyneither_ptr _tmp2DD=(_tmp838.tag=0,((_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp839[0]=& _tmp838,Cyc_aprintf(((_tmp83A="(%s)",_tag_dyneither(_tmp83A,sizeof(char),5))),_tag_dyneither(_tmp839,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2C7;x=x->tl){
void*_stmttmp2F=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp2DE=_stmttmp2F;struct _dyneither_ptr*_tmp2E0;unsigned int _tmp2E2;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2DF=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2DE;if(_tmp2DF->tag != 0)goto _LL185;else{_tmp2E0=_tmp2DF->f1;}}_LL184:
# 1462
{const char*_tmp83F;void*_tmp83E[2];struct Cyc_String_pa_PrintArg_struct _tmp83D;struct Cyc_String_pa_PrintArg_struct _tmp83C;_tmp2DD=((_tmp83C.tag=0,((_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E0),((_tmp83D.tag=0,((_tmp83D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp83E[0]=& _tmp83D,((_tmp83E[1]=& _tmp83C,Cyc_aprintf(((_tmp83F="%s.%s",_tag_dyneither(_tmp83F,sizeof(char),6))),_tag_dyneither(_tmp83E,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2E1=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2DE;if(_tmp2E1->tag != 1)goto _LL182;else{_tmp2E2=_tmp2E1->f1;}}_LL186:
# 1464
{const char*_tmp844;void*_tmp843[2];struct Cyc_String_pa_PrintArg_struct _tmp842;struct Cyc_Int_pa_PrintArg_struct _tmp841;_tmp2DD=((_tmp841.tag=1,((_tmp841.f1=(unsigned long)((int)_tmp2E2),((_tmp842.tag=0,((_tmp842.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp843[0]=& _tmp842,((_tmp843[1]=& _tmp841,Cyc_aprintf(((_tmp844="%s.%d",_tag_dyneither(_tmp844,sizeof(char),6))),_tag_dyneither(_tmp843,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1466
const char*_tmp849;void*_tmp848[2];struct Cyc_String_pa_PrintArg_struct _tmp847;struct Cyc_String_pa_PrintArg_struct _tmp846;(_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp847.tag=0,((_tmp847.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp848[0]=& _tmp847,((_tmp848[1]=& _tmp846,Cyc_Tcutil_terr(loc,((_tmp849="%s == %s is not a struct/union type",_tag_dyneither(_tmp849,sizeof(char),36))),_tag_dyneither(_tmp848,sizeof(void*),2)))))))))))));};}}
# 1469
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2CD=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2CA;if(_tmp2CD->tag != 1)goto _LL176;else{_tmp2CE=_tmp2CD->f1;}}_LL17A: {
# 1471
int bad_type=1;
{void*_stmttmp2D=Cyc_Tcutil_compress(_tmp2C8);void*_tmp2F2=_stmttmp2D;struct Cyc_Absyn_Aggrdecl**_tmp2F4;struct Cyc_List_List*_tmp2F6;struct Cyc_List_List*_tmp2F8;struct Cyc_Absyn_Datatypefield*_tmp2FA;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F3->tag != 11)goto _LL18A;else{if((((_tmp2F3->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp2F4=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2F3->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1474
 if((*_tmp2F4)->impl == 0)
goto _LL187;
_tmp2F6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F4)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F5->tag != 12)goto _LL18C;else{_tmp2F6=_tmp2F5->f2;}}_LL18B:
# 1478
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6)<= _tmp2CE){
const char*_tmp84E;void*_tmp84D[2];struct Cyc_Int_pa_PrintArg_struct _tmp84C;struct Cyc_Int_pa_PrintArg_struct _tmp84B;(_tmp84B.tag=1,((_tmp84B.f1=(unsigned long)((int)_tmp2CE),((_tmp84C.tag=1,((_tmp84C.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6),((_tmp84D[0]=& _tmp84C,((_tmp84D[1]=& _tmp84B,Cyc_Tcutil_terr(loc,((_tmp84E="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp84E,sizeof(char),46))),_tag_dyneither(_tmp84D,sizeof(void*),2)))))))))))));}else{
# 1482
_tmp2C8=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F6,(int)_tmp2CE))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F7->tag != 10)goto _LL18E;else{_tmp2F8=_tmp2F7->f1;}}_LL18D:
# 1486
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8)<= _tmp2CE){
const char*_tmp853;void*_tmp852[2];struct Cyc_Int_pa_PrintArg_struct _tmp851;struct Cyc_Int_pa_PrintArg_struct _tmp850;(_tmp850.tag=1,((_tmp850.f1=(unsigned long)((int)_tmp2CE),((_tmp851.tag=1,((_tmp851.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8),((_tmp852[0]=& _tmp851,((_tmp852[1]=& _tmp850,Cyc_Tcutil_terr(loc,((_tmp853="tuple has too few components: %d <= %d",_tag_dyneither(_tmp853,sizeof(char),39))),_tag_dyneither(_tmp852,sizeof(void*),2)))))))))))));}else{
# 1490
_tmp2C8=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F8,(int)_tmp2CE)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F9->tag != 4)goto _LL190;else{if((((_tmp2F9->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp2FA=((struct _tuple2)(((_tmp2F9->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1494
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FA->typs)< _tmp2CE){
const char*_tmp858;void*_tmp857[2];struct Cyc_Int_pa_PrintArg_struct _tmp856;struct Cyc_Int_pa_PrintArg_struct _tmp855;(_tmp855.tag=1,((_tmp855.f1=(unsigned long)((int)_tmp2CE),((_tmp856.tag=1,((_tmp856.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FA->typs),((_tmp857[0]=& _tmp856,((_tmp857[1]=& _tmp855,Cyc_Tcutil_terr(loc,((_tmp858="datatype field has too few components: %d < %d",_tag_dyneither(_tmp858,sizeof(char),47))),_tag_dyneither(_tmp857,sizeof(void*),2)))))))))))));}else{
# 1498
if(_tmp2CE != 0)
_tmp2C8=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2FA->typs,(int)(_tmp2CE - 1))).f2;else{
if(_tmp2C7->tl != 0){
const char*_tmp85B;void*_tmp85A;(_tmp85A=0,Cyc_Tcutil_terr(loc,((_tmp85B="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp85B,sizeof(char),68))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}}}
# 1503
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1507
if(bad_type){
const char*_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_PrintArg_struct _tmp85D;(_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp85E[0]=& _tmp85D,Cyc_Tcutil_terr(loc,((_tmp85F="%s is not a known type",_tag_dyneither(_tmp85F,sizeof(char),23))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1512
return Cyc_Absyn_uint_typ;};}
# 1516
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp30C=_new_region("r");struct _RegionHandle*r=& _tmp30C;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp30D=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp30D,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp30E=t;void*_tmp311;void*_tmp312;union Cyc_Absyn_Constraint*_tmp313;union Cyc_Absyn_Constraint*_tmp314;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp30F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30E;if(_tmp30F->tag != 1)goto _LL195;}_LL194: {
# 1523
struct Cyc_List_List*_tmp315=Cyc_Tcenv_lookup_type_vars(_tmp30D);
struct Cyc_Core_Opt*_tmp860;void*_tmp316=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860->v=_tmp315,_tmp860)))));
struct Cyc_Core_Opt*_tmp861;void*_tmp317=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861->v=_tmp315,_tmp861)))));
union Cyc_Absyn_Constraint*_tmp318=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp319=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp862;struct Cyc_Absyn_PtrAtts _tmp31A=(_tmp862.rgn=_tmp317,((_tmp862.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp862.bounds=_tmp318,((_tmp862.zero_term=_tmp319,((_tmp862.ptrloc=0,_tmp862)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp868;struct Cyc_Absyn_PtrInfo _tmp867;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp866;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31B=(_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866[0]=((_tmp868.tag=5,((_tmp868.f1=((_tmp867.elt_typ=_tmp316,((_tmp867.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp867.ptr_atts=_tmp31A,_tmp867)))))),_tmp868)))),_tmp866)));
Cyc_Tcutil_unify(t,(void*)_tmp31B);
_tmp311=_tmp316;_tmp312=_tmp317;_tmp313=_tmp318;_tmp314=_tmp319;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30E;if(_tmp310->tag != 5)goto _LL197;else{_tmp311=(_tmp310->f1).elt_typ;_tmp312=((_tmp310->f1).ptr_atts).rgn;_tmp313=((_tmp310->f1).ptr_atts).bounds;_tmp314=((_tmp310->f1).ptr_atts).zero_term;}}_LL196:
# 1533
 Cyc_Tcenv_check_rgn_accessible(_tmp30D,loc,_tmp312);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp313);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp311),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp30D)){
const char*_tmp86B;void*_tmp86A;(_tmp86A=0,Cyc_Tcutil_terr(loc,((_tmp86B="can't dereference abstract pointer type",_tag_dyneither(_tmp86B,sizeof(char),40))),_tag_dyneither(_tmp86A,sizeof(void*),0)));}{
void*_tmp324=_tmp311;_npop_handler(0);return _tmp324;};_LL197:;_LL198: {
# 1539
const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_PrintArg_struct _tmp86D;void*_tmp328=(_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp86E[0]=& _tmp86D,Cyc_Tcexp_expr_err(_tmp30D,loc,topt,((_tmp86F="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp86F,sizeof(char),39))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));_npop_handler(0);return _tmp328;}_LL192:;};}
# 1518
;_pop_region(r);}
# 1545
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1549
struct _RegionHandle _tmp329=_new_region("r");struct _RegionHandle*r=& _tmp329;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1553
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_stmttmp30=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp32A=_stmttmp30;struct Cyc_Absyn_Aggrdecl*_tmp32C;struct Cyc_List_List*_tmp32D;enum Cyc_Absyn_AggrKind _tmp32F;struct Cyc_List_List*_tmp330;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp32A;if(_tmp32B->tag != 11)goto _LL19C;else{if((((_tmp32B->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp32C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp32B->f1).aggr_info).KnownAggr).val);_tmp32D=(_tmp32B->f1).targs;}}_LL19B: {
# 1556
struct Cyc_Absyn_Aggrfield*_tmp335=Cyc_Absyn_lookup_decl_field(_tmp32C,f);
if(_tmp335 == 0){
const char*_tmp874;void*_tmp873[2];struct Cyc_String_pa_PrintArg_struct _tmp872;struct Cyc_String_pa_PrintArg_struct _tmp871;void*_tmp33A=(_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp32C->name)),((_tmp873[0]=& _tmp872,((_tmp873[1]=& _tmp871,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp874="type %s has no %s field",_tag_dyneither(_tmp874,sizeof(char),24))),_tag_dyneither(_tmp873,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp33A;}
# 1561
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32C->impl))->tagged)*is_tagged=1;{
void*t2=_tmp335->type;
if(_tmp32D != 0){
struct _RegionHandle _tmp33B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33B;_push_region(rgn);
{struct Cyc_List_List*_tmp33C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp32C->tvs,_tmp32D);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp33C,_tmp335->type);}
# 1565
;_pop_region(rgn);}
# 1571
if((((_tmp32C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32C->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp335->requires_clause == 0){
const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;void*_tmp340=(_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp877[0]=& _tmp876,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp878="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp878,sizeof(char),56))),_tag_dyneither(_tmp877,sizeof(void*),1)))))));_npop_handler(0);return _tmp340;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32C->impl))->exist_vars != 0){
# 1576
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp87C;void*_tmp87B[1];struct Cyc_String_pa_PrintArg_struct _tmp87A;void*_tmp344=(_tmp87A.tag=0,((_tmp87A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87B[0]=& _tmp87A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87C="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp87C,sizeof(char),81))),_tag_dyneither(_tmp87B,sizeof(void*),1)))))));_npop_handler(0);return _tmp344;}}{
# 1579
void*_tmp345=t2;_npop_handler(0);return _tmp345;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32A;if(_tmp32E->tag != 12)goto _LL19E;else{_tmp32F=_tmp32E->f1;_tmp330=_tmp32E->f2;}}_LL19D: {
# 1581
struct Cyc_Absyn_Aggrfield*_tmp346=Cyc_Absyn_lookup_field(_tmp330,f);
if(_tmp346 == 0){
const char*_tmp880;void*_tmp87F[1];struct Cyc_String_pa_PrintArg_struct _tmp87E;void*_tmp34A=(_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87F[0]=& _tmp87E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp880="type has no %s field",_tag_dyneither(_tmp880,sizeof(char),21))),_tag_dyneither(_tmp87F,sizeof(void*),1)))))));_npop_handler(0);return _tmp34A;}
# 1586
if(((_tmp32F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp346->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp346->requires_clause == 0){
# 1588
const char*_tmp884;void*_tmp883[1];struct Cyc_String_pa_PrintArg_struct _tmp882;void*_tmp34E=(_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp883[0]=& _tmp882,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp884="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp884,sizeof(char),56))),_tag_dyneither(_tmp883,sizeof(void*),1)))))));_npop_handler(0);return _tmp34E;}{
void*_tmp34F=_tmp346->type;_npop_handler(0);return _tmp34F;};}_LL19E:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32A;if(_tmp331->tag != 8)goto _LL1A0;}{const char*_tmp885;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp885="size",_tag_dyneither(_tmp885,sizeof(char),5))))== 0))goto _LL1A0;};_LL19F: goto _LL1A1;_LL1A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A;if(_tmp333->tag != 5)goto _LL1A2;}{const char*_tmp886;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp886="size",_tag_dyneither(_tmp886,sizeof(char),5))))== 0))goto _LL1A2;};_LL1A1:
{const char*_tmp88A;void*_tmp889[1];struct Cyc_String_pa_PrintArg_struct _tmp888;(_tmp888.tag=0,((_tmp888.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp889[0]=& _tmp888,Cyc_Tcutil_warn(e->loc,((_tmp88A="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp88A,sizeof(char),54))),_tag_dyneither(_tmp889,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp890;struct Cyc_List_List*_tmp88F;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp88E;outer_e->r=(void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp890.tag=2,((_tmp890.f1=Cyc_Absyn_Numelts,((_tmp890.f2=((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F->hd=e,((_tmp88F->tl=0,_tmp88F)))))),_tmp890)))))),_tmp88E))));}{
void*_tmp356=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp356;};_LL1A2:;_LL1A3: {
# 1596
const char*_tmp891;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp891="size",_tag_dyneither(_tmp891,sizeof(char),5))))== 0){
const char*_tmp895;void*_tmp894[1];struct Cyc_String_pa_PrintArg_struct _tmp893;void*_tmp35B=(_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1599
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp894[0]=& _tmp893,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp895="expecting struct, union, or array, found %s",_tag_dyneither(_tmp895,sizeof(char),44))),_tag_dyneither(_tmp894,sizeof(void*),1)))))));_npop_handler(0);return _tmp35B;}else{
# 1601
const char*_tmp899;void*_tmp898[1];struct Cyc_String_pa_PrintArg_struct _tmp897;void*_tmp35F=(_tmp897.tag=0,((_tmp897.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp898[0]=& _tmp897,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp899="expecting struct or union, found %s",_tag_dyneither(_tmp899,sizeof(char),36))),_tag_dyneither(_tmp898,sizeof(void*),1)))))));_npop_handler(0);return _tmp35F;}}_LL199:;};
# 1549
;_pop_region(r);}
# 1607
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1610
struct _RegionHandle _tmp360=_new_region("r");struct _RegionHandle*r=& _tmp360;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1615
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_stmttmp31=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp361=_stmttmp31;void*_tmp363;void*_tmp364;union Cyc_Absyn_Constraint*_tmp365;union Cyc_Absyn_Constraint*_tmp366;_LL1A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp362=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp361;if(_tmp362->tag != 5)goto _LL1A7;else{_tmp363=(_tmp362->f1).elt_typ;_tmp364=((_tmp362->f1).ptr_atts).rgn;_tmp365=((_tmp362->f1).ptr_atts).bounds;_tmp366=((_tmp362->f1).ptr_atts).zero_term;}}_LL1A6:
# 1618
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp365);
{void*_stmttmp32=Cyc_Tcutil_compress(_tmp363);void*_tmp367=_stmttmp32;struct Cyc_Absyn_Aggrdecl*_tmp369;struct Cyc_List_List*_tmp36A;enum Cyc_Absyn_AggrKind _tmp36C;struct Cyc_List_List*_tmp36D;_LL1AA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp367;if(_tmp368->tag != 11)goto _LL1AC;else{if((((_tmp368->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AC;_tmp369=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp368->f1).aggr_info).KnownAggr).val);_tmp36A=(_tmp368->f1).targs;}}_LL1AB: {
# 1621
struct Cyc_Absyn_Aggrfield*_tmp36E=Cyc_Absyn_lookup_decl_field(_tmp369,f);
if(_tmp36E == 0){
const char*_tmp89E;void*_tmp89D[2];struct Cyc_String_pa_PrintArg_struct _tmp89C;struct Cyc_String_pa_PrintArg_struct _tmp89B;void*_tmp373=(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp369->name)),((_tmp89D[0]=& _tmp89C,((_tmp89D[1]=& _tmp89B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89E="type %s has no %s field",_tag_dyneither(_tmp89E,sizeof(char),24))),_tag_dyneither(_tmp89D,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp373;}
# 1626
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->tagged)*is_tagged=1;{
void*t3=_tmp36E->type;
if(_tmp36A != 0){
struct _RegionHandle _tmp374=_new_region("rgn");struct _RegionHandle*rgn=& _tmp374;_push_region(rgn);
{struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp369->tvs,_tmp36A);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp375,_tmp36E->type);}
# 1630
;_pop_region(rgn);}{
# 1634
struct Cyc_Absyn_Kind*_tmp376=Cyc_Tcutil_typ_kind(t3);
# 1637
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp376) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_stmttmp33=Cyc_Tcutil_compress(t3);void*_tmp377=_stmttmp33;_LL1B1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp377;if(_tmp378->tag != 8)goto _LL1B3;}_LL1B2:
 goto _LL1B0;_LL1B3:;_LL1B4: {
# 1641
const char*_tmp8A2;void*_tmp8A1[1];struct Cyc_String_pa_PrintArg_struct _tmp8A0;void*_tmp37C=(_tmp8A0.tag=0,((_tmp8A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A1[0]=& _tmp8A0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A2="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8A2,sizeof(char),48))),_tag_dyneither(_tmp8A1,sizeof(void*),1)))))));_npop_handler(0);return _tmp37C;}_LL1B0:;}
# 1646
if((((_tmp369->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp36E->requires_clause == 0){
# 1649
const char*_tmp8A6;void*_tmp8A5[1];struct Cyc_String_pa_PrintArg_struct _tmp8A4;void*_tmp380=(_tmp8A4.tag=0,((_tmp8A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A5[0]=& _tmp8A4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8A6,sizeof(char),56))),_tag_dyneither(_tmp8A5,sizeof(void*),1)))))));_npop_handler(0);return _tmp380;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8AA;void*_tmp8A9[1];struct Cyc_String_pa_PrintArg_struct _tmp8A8;void*_tmp384=(_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A9[0]=& _tmp8A8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AA="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8AA,sizeof(char),72))),_tag_dyneither(_tmp8A9,sizeof(void*),1)))))));_npop_handler(0);return _tmp384;}}{
# 1656
void*_tmp385=t3;_npop_handler(0);return _tmp385;};};};}_LL1AC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp367;if(_tmp36B->tag != 12)goto _LL1AE;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36B->f2;}}_LL1AD: {
# 1658
struct Cyc_Absyn_Aggrfield*_tmp386=Cyc_Absyn_lookup_field(_tmp36D,f);
if(_tmp386 == 0){
const char*_tmp8AE;void*_tmp8AD[1];struct Cyc_String_pa_PrintArg_struct _tmp8AC;void*_tmp38A=(_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AD[0]=& _tmp8AC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AE="type has no %s field",_tag_dyneither(_tmp8AE,sizeof(char),21))),_tag_dyneither(_tmp8AD,sizeof(void*),1)))))));_npop_handler(0);return _tmp38A;}
# 1663
if((_tmp36C == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp386->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp8B2;void*_tmp8B1[1];struct Cyc_String_pa_PrintArg_struct _tmp8B0;void*_tmp38E=(_tmp8B0.tag=0,((_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B1[0]=& _tmp8B0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B2="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8B2,sizeof(char),56))),_tag_dyneither(_tmp8B1,sizeof(void*),1)))))));_npop_handler(0);return _tmp38E;}{
void*_tmp38F=_tmp386->type;_npop_handler(0);return _tmp38F;};}_LL1AE:;_LL1AF:
 goto _LL1A9;_LL1A9:;}
# 1668
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}{
# 1671
const char*_tmp8B6;void*_tmp8B5[1];struct Cyc_String_pa_PrintArg_struct _tmp8B4;void*_tmp393=(_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8B5[0]=& _tmp8B4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B6="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8B6,sizeof(char),44))),_tag_dyneither(_tmp8B5,sizeof(void*),1)))))));_npop_handler(0);return _tmp393;};
# 1610
;_pop_region(r);}
# 1676
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1678
struct _tuple15 _stmttmp34=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp395;int _tmp396;struct _tuple15 _tmp394=_stmttmp34;_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;
if(!_tmp396){
const char*_tmp8B9;void*_tmp8B8;return(_tmp8B8=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8B9="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8B9,sizeof(char),47))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}{
# 1682
struct _handler_cons _tmp399;_push_handler(& _tmp399);{int _tmp39B=0;if(setjmp(_tmp399.handler))_tmp39B=1;if(!_tmp39B){
{void*_tmp39C=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp395)).f2;_npop_handler(0);return _tmp39C;};_pop_handler();}else{void*_tmp39A=(void*)_exn_thrown;void*_tmp39E=_tmp39A;void*_tmp3A0;_LL1B6: {struct Cyc_List_Nth_exn_struct*_tmp39F=(struct Cyc_List_Nth_exn_struct*)_tmp39E;if(_tmp39F->tag != Cyc_List_Nth)goto _LL1B8;}_LL1B7: {
# 1685
const char*_tmp8BE;void*_tmp8BD[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BC;struct Cyc_Int_pa_PrintArg_struct _tmp8BB;return(_tmp8BB.tag=1,((_tmp8BB.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8BC.tag=1,((_tmp8BC.f1=(unsigned long)((int)_tmp395),((_tmp8BD[0]=& _tmp8BC,((_tmp8BD[1]=& _tmp8BB,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8BE="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8BE,sizeof(char),41))),_tag_dyneither(_tmp8BD,sizeof(void*),2)))))))))))));}_LL1B8: _tmp3A0=_tmp39E;_LL1B9:(void)_throw(_tmp3A0);_LL1B5:;}};};}
# 1690
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1692
struct _RegionHandle _tmp3A5=_new_region("r");struct _RegionHandle*r=& _tmp3A5;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3A6=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3A6),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3A6),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A6,e2)){
const char*_tmp8C2;void*_tmp8C1[1];struct Cyc_String_pa_PrintArg_struct _tmp8C0;void*_tmp3AA=(_tmp8C0.tag=0,((_tmp8C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8C1[0]=& _tmp8C0,Cyc_Tcexp_expr_err(_tmp3A6,e2->loc,topt,((_tmp8C2="expecting int subscript, found %s",_tag_dyneither(_tmp8C2,sizeof(char),34))),_tag_dyneither(_tmp8C1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3AA;}{
# 1703
void*_tmp3AB=t1;void*_tmp3AD;struct Cyc_Absyn_Tqual _tmp3AE;void*_tmp3AF;union Cyc_Absyn_Constraint*_tmp3B0;union Cyc_Absyn_Constraint*_tmp3B1;struct Cyc_List_List*_tmp3B3;_LL1BB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3AC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB;if(_tmp3AC->tag != 5)goto _LL1BD;else{_tmp3AD=(_tmp3AC->f1).elt_typ;_tmp3AE=(_tmp3AC->f1).elt_tq;_tmp3AF=((_tmp3AC->f1).ptr_atts).rgn;_tmp3B0=((_tmp3AC->f1).ptr_atts).bounds;_tmp3B1=((_tmp3AC->f1).ptr_atts).zero_term;}}_LL1BC:
# 1707
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B1)){
int emit_warning=1;
{void*_stmttmp35=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3B0);void*_tmp3B4=_stmttmp35;struct Cyc_Absyn_Exp*_tmp3B6;_LL1C2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3B4;if(_tmp3B5->tag != 1)goto _LL1C4;else{_tmp3B6=_tmp3B5->f1;}}_LL1C3:
# 1711
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp36=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3B9;int _tmp3BA;struct _tuple15 _tmp3B8=_stmttmp36;_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;
if(_tmp3BA){
struct _tuple15 _stmttmp37=Cyc_Evexp_eval_const_uint_exp(_tmp3B6);unsigned int _tmp3BC;int _tmp3BD;struct _tuple15 _tmp3BB=_stmttmp37;_tmp3BC=_tmp3BB.f1;_tmp3BD=_tmp3BB.f2;
if(_tmp3BD  && _tmp3BC > _tmp3B9)emit_warning=0;}}
# 1718
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3B7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B4;if(_tmp3B7->tag != 0)goto _LL1C1;}_LL1C5:
 emit_warning=0;goto _LL1C1;_LL1C1:;}
# 1721
if(emit_warning){
const char*_tmp8C5;void*_tmp8C4;(_tmp8C4=0,Cyc_Tcutil_warn(e2->loc,((_tmp8C5="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8C5,sizeof(char),63))),_tag_dyneither(_tmp8C4,sizeof(void*),0)));}}else{
# 1725
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp38=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3C1;int _tmp3C2;struct _tuple15 _tmp3C0=_stmttmp38;_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;
if(_tmp3C2)
Cyc_Tcutil_check_bound(loc,_tmp3C1,_tmp3B0);}else{
# 1731
if(Cyc_Tcutil_is_bound_one(_tmp3B0) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B1)){
const char*_tmp8C8;void*_tmp8C7;(_tmp8C7=0,Cyc_Tcutil_warn(e1->loc,((_tmp8C8="subscript applied to pointer to one object",_tag_dyneither(_tmp8C8,sizeof(char),43))),_tag_dyneither(_tmp8C7,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3B0);}}
# 1736
Cyc_Tcenv_check_rgn_accessible(_tmp3A6,loc,_tmp3AF);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3AD),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3A6)){
const char*_tmp8CB;void*_tmp8CA;(_tmp8CA=0,Cyc_Tcutil_terr(e1->loc,((_tmp8CB="can't subscript an abstract pointer",_tag_dyneither(_tmp8CB,sizeof(char),36))),_tag_dyneither(_tmp8CA,sizeof(void*),0)));}{
void*_tmp3C7=_tmp3AD;_npop_handler(0);return _tmp3C7;};_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AB;if(_tmp3B2->tag != 10)goto _LL1BF;else{_tmp3B3=_tmp3B2->f1;}}_LL1BE: {
void*_tmp3C8=Cyc_Tcexp_ithTupleType(_tmp3A6,loc,_tmp3B3,e2);_npop_handler(0);return _tmp3C8;}_LL1BF:;_LL1C0: {
const char*_tmp8CF;void*_tmp8CE[1];struct Cyc_String_pa_PrintArg_struct _tmp8CD;void*_tmp3CC=(_tmp8CD.tag=0,((_tmp8CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8CE[0]=& _tmp8CD,Cyc_Tcexp_expr_err(_tmp3A6,loc,topt,((_tmp8CF="subscript applied to %s",_tag_dyneither(_tmp8CF,sizeof(char),24))),_tag_dyneither(_tmp8CE,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CC;}_LL1BA:;};};}
# 1693
;_pop_region(r);}
# 1747
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_stmttmp39=Cyc_Tcutil_compress(*topt);void*_tmp3CD=_stmttmp39;struct Cyc_List_List*_tmp3CF;_LL1C7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3CE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CD;if(_tmp3CE->tag != 10)goto _LL1C9;else{_tmp3CF=_tmp3CE->f1;}}_LL1C8:
# 1753
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3CF)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1756
goto _LL1C6;
# 1758
for(0;es != 0;(es=es->tl,_tmp3CF=_tmp3CF->tl)){
struct _RegionHandle _tmp3D0=_new_region("r");struct _RegionHandle*r=& _tmp3D0;_push_region(r);
{int bogus=0;
void*_tmp3D1=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp3CF))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp3D1,(struct Cyc_Absyn_Exp*)es->hd);
# 1764
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp3CF->hd)).f2,& bogus);{
struct _tuple18*_tmp8D2;struct Cyc_List_List*_tmp8D1;fields=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1->hd=((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2->f1=(*((struct _tuple18*)_tmp3CF->hd)).f1,((_tmp8D2->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8D2)))))),((_tmp8D1->tl=fields,_tmp8D1))))));};}
# 1760
;_pop_region(r);}
# 1768
done=1;
goto _LL1C6;_LL1C9:;_LL1CA:
 goto _LL1C6;_LL1C6:;}
# 1772
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp3D4=_new_region("r");struct _RegionHandle*r=& _tmp3D4;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp8D5;struct Cyc_List_List*_tmp8D4;fields=((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4->hd=((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5->f1=Cyc_Absyn_empty_tqual(0),((_tmp8D5->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8D5)))))),((_tmp8D4->tl=fields,_tmp8D4))))));};
# 1775
;_pop_region(r);}{
# 1779
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8D8;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8D7;return(void*)((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7[0]=((_tmp8D8.tag=10,((_tmp8D8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8D8)))),_tmp8D7))));};}
# 1783
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1786
const char*_tmp8DB;void*_tmp8DA;return(_tmp8DA=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DB="tcCompoundLit",_tag_dyneither(_tmp8DB,sizeof(char),14))),_tag_dyneither(_tmp8DA,sizeof(void*),0)));}
# 1796 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1799
void*res_t2;
struct _RegionHandle _tmp3DB=_new_region("r");struct _RegionHandle*r=& _tmp3DB;_push_region(r);{
int _tmp3DC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8DE;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8DD;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3DD=(_tmp8DD=_cycalloc(sizeof(*_tmp8DD)),((_tmp8DD[0]=((_tmp8DE.tag=0,((_tmp8DE.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3DC),_tmp8DE)))),_tmp8DD)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3DD,loc);
# 1808
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3DE=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3DC - 1);
if(!Cyc_Tcutil_is_zero(_tmp3DE)){
const char*_tmp8E1;void*_tmp8E0;(_tmp8E0=0,Cyc_Tcutil_terr(_tmp3DE->loc,((_tmp8E1="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8E1,sizeof(char),45))),_tag_dyneither(_tmp8E0,sizeof(void*),0)));}}
# 1813
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1818
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1821
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1823
const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;(_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8E4[0]=& _tmp8E3,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8E5="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8E5,sizeof(char),53))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));}}
# 1827
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1832
void*_stmttmp3A=(void*)ds->hd;void*_tmp3E6=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3E9;_LL1CC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E6;if(_tmp3E7->tag != 1)goto _LL1CE;}_LL1CD:
# 1834
{const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,Cyc_Tcutil_terr(loc,((_tmp8E8="only array index designators are supported",_tag_dyneither(_tmp8E8,sizeof(char),43))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3E8=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3E6;if(_tmp3E8->tag != 0)goto _LL1CB;else{_tmp3E9=_tmp3E8->f1;}}_LL1CF:
# 1837
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3E9);{
struct _tuple15 _stmttmp3B=Cyc_Evexp_eval_const_uint_exp(_tmp3E9);unsigned int _tmp3ED;int _tmp3EE;struct _tuple15 _tmp3EC=_stmttmp3B;_tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;
if(!_tmp3EE){
const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_terr(_tmp3E9->loc,((_tmp8EB="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8EB,sizeof(char),47))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}else{
if(_tmp3ED != offset){
const char*_tmp8F0;void*_tmp8EF[2];struct Cyc_Int_pa_PrintArg_struct _tmp8EE;struct Cyc_Int_pa_PrintArg_struct _tmp8ED;(_tmp8ED.tag=1,((_tmp8ED.f1=(unsigned long)((int)_tmp3ED),((_tmp8EE.tag=1,((_tmp8EE.f1=(unsigned long)offset,((_tmp8EF[0]=& _tmp8EE,((_tmp8EF[1]=& _tmp8ED,Cyc_Tcutil_terr(_tmp3E9->loc,((_tmp8F0="expecting index designator %d but found %d",_tag_dyneither(_tmp8F0,sizeof(char),43))),_tag_dyneither(_tmp8EF,sizeof(void*),2)))))))))))));}}
# 1844
goto _LL1CB;};_LL1CB:;}}}{
# 1848
void*_tmp3F5=res_t2;_npop_handler(0);return _tmp3F5;};
# 1800
;_pop_region(r);}
# 1852
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1855
Cyc_Tcexp_tcExp(te,0,bound);
{void*_stmttmp3C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3F6=_stmttmp3C;void*_tmp3F8;_LL1D1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F6;if(_tmp3F7->tag != 19)goto _LL1D3;else{_tmp3F8=(void*)_tmp3F7->f1;}}_LL1D2:
# 1860
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3F8,0),0,Cyc_Absyn_No_coercion,0);
_tmp3F9->topt=bound->topt;
# 1865
bound=_tmp3F9;}
# 1867
goto _LL1D0;_LL1D3:;_LL1D4:
# 1869
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp8F4;void*_tmp8F3[1];struct Cyc_String_pa_PrintArg_struct _tmp8F2;(_tmp8F2.tag=0,((_tmp8F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8F3[0]=& _tmp8F2,Cyc_Tcutil_terr(bound->loc,((_tmp8F4="expecting unsigned int, found %s",_tag_dyneither(_tmp8F4,sizeof(char),33))),_tag_dyneither(_tmp8F3,sizeof(void*),1)))))));}_LL1D0:;}
# 1874
if(!(vd->tq).real_const){
const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8F7="comprehension index variable is not declared const!",_tag_dyneither(_tmp8F7,sizeof(char),52))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}{
# 1877
struct _RegionHandle _tmp3FF=_new_region("r");struct _RegionHandle*r=& _tmp3FF;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1883
struct Cyc_Tcenv_Tenv*_tmp400=te2;
void**_tmp401=0;
struct Cyc_Absyn_Tqual*_tmp402=0;
union Cyc_Absyn_Constraint**_tmp403=0;
# 1888
if(topt != 0){
void*_stmttmp3D=Cyc_Tcutil_compress(*topt);void*_tmp404=_stmttmp3D;struct Cyc_Absyn_PtrInfo _tmp406;void*_tmp408;struct Cyc_Absyn_Tqual _tmp409;struct Cyc_Absyn_Exp*_tmp40A;union Cyc_Absyn_Constraint*_tmp40B;_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp404;if(_tmp405->tag != 5)goto _LL1D8;else{_tmp406=_tmp405->f1;}}_LL1D7:
# 1891
{void**_tmp8F8;_tmp401=((_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8[0]=_tmp406.elt_typ,_tmp8F8))));}
{struct Cyc_Absyn_Tqual*_tmp8F9;_tmp402=((_tmp8F9=_region_malloc(r,sizeof(*_tmp8F9)),((_tmp8F9[0]=_tmp406.elt_tq,_tmp8F9))));}
{union Cyc_Absyn_Constraint**_tmp8FA;_tmp403=((_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA[0]=(_tmp406.ptr_atts).zero_term,_tmp8FA))));}
goto _LL1D5;_LL1D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404;if(_tmp407->tag != 8)goto _LL1DA;else{_tmp408=(_tmp407->f1).elt_type;_tmp409=(_tmp407->f1).tq;_tmp40A=(_tmp407->f1).num_elts;_tmp40B=(_tmp407->f1).zero_term;}}_LL1D9:
# 1896
{void**_tmp8FB;_tmp401=((_tmp8FB=_region_malloc(r,sizeof(*_tmp8FB)),((_tmp8FB[0]=_tmp408,_tmp8FB))));}
{struct Cyc_Absyn_Tqual*_tmp8FC;_tmp402=((_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=_tmp409,_tmp8FC))));}
{union Cyc_Absyn_Constraint**_tmp8FD;_tmp403=((_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD[0]=_tmp40B,_tmp8FD))));}
goto _LL1D5;_LL1DA:;_LL1DB:
# 1901
 goto _LL1D5;_LL1D5:;}{
# 1904
void*t=Cyc_Tcexp_tcExp(_tmp400,_tmp401,body);
if(_tmp400->le == 0){
# 1907
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_Tcutil_terr(bound->loc,((_tmp900="bound is not constant",_tag_dyneither(_tmp900,sizeof(char),22))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_Tcutil_terr(bound->loc,((_tmp903="body is not constant",_tag_dyneither(_tmp903,sizeof(char),21))),_tag_dyneither(_tmp902,sizeof(void*),0)));}}
# 1912
if(_tmp403 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp403)){
# 1915
struct Cyc_Absyn_Exp*_tmp416=Cyc_Absyn_uint_exp(1,0);_tmp416->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp416,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1920
void*_tmp417=Cyc_Absyn_array_typ(t,_tmp402 == 0?Cyc_Absyn_empty_tqual(0):*_tmp402,bound,
_tmp403 == 0?Cyc_Absyn_false_conref:*_tmp403,0);
void*_tmp418=_tmp417;_npop_handler(0);return _tmp418;};};};}
# 1878
;_pop_region(r);};}
# 1927
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1930
Cyc_Tcexp_tcExp(te,0,bound);
{void*_stmttmp3E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp419=_stmttmp3E;void*_tmp41B;_LL1DD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp419;if(_tmp41A->tag != 19)goto _LL1DF;else{_tmp41B=(void*)_tmp41A->f1;}}_LL1DE:
# 1935
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp41B,0),0,Cyc_Absyn_No_coercion,0);
_tmp41C->topt=bound->topt;
# 1940
bound=_tmp41C;}
# 1942
goto _LL1DC;_LL1DF:;_LL1E0:
# 1944
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp907;void*_tmp906[1];struct Cyc_String_pa_PrintArg_struct _tmp905;(_tmp905.tag=0,((_tmp905.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp906[0]=& _tmp905,Cyc_Tcutil_terr(bound->loc,((_tmp907="expecting unsigned int, found %s",_tag_dyneither(_tmp907,sizeof(char),33))),_tag_dyneither(_tmp906,sizeof(void*),1)))))));}_LL1DC:;}{
# 1949
struct _RegionHandle _tmp420=_new_region("r");struct _RegionHandle*r=& _tmp420;_push_region(r);
{void**_tmp421=0;
struct Cyc_Absyn_Tqual*_tmp422=0;
union Cyc_Absyn_Constraint**_tmp423=0;
# 1954
if(topt != 0){
void*_stmttmp3F=Cyc_Tcutil_compress(*topt);void*_tmp424=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp426;void*_tmp428;struct Cyc_Absyn_Tqual _tmp429;struct Cyc_Absyn_Exp*_tmp42A;union Cyc_Absyn_Constraint*_tmp42B;_LL1E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp425=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp424;if(_tmp425->tag != 5)goto _LL1E4;else{_tmp426=_tmp425->f1;}}_LL1E3:
# 1957
{void**_tmp908;_tmp421=((_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=_tmp426.elt_typ,_tmp908))));}
{struct Cyc_Absyn_Tqual*_tmp909;_tmp422=((_tmp909=_region_malloc(r,sizeof(*_tmp909)),((_tmp909[0]=_tmp426.elt_tq,_tmp909))));}
{union Cyc_Absyn_Constraint**_tmp90A;_tmp423=((_tmp90A=_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A[0]=(_tmp426.ptr_atts).zero_term,_tmp90A))));}
goto _LL1E1;_LL1E4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp424;if(_tmp427->tag != 8)goto _LL1E6;else{_tmp428=(_tmp427->f1).elt_type;_tmp429=(_tmp427->f1).tq;_tmp42A=(_tmp427->f1).num_elts;_tmp42B=(_tmp427->f1).zero_term;}}_LL1E5:
# 1962
{void**_tmp90B;_tmp421=((_tmp90B=_region_malloc(r,sizeof(*_tmp90B)),((_tmp90B[0]=_tmp428,_tmp90B))));}
{struct Cyc_Absyn_Tqual*_tmp90C;_tmp422=((_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C[0]=_tmp429,_tmp90C))));}
{union Cyc_Absyn_Constraint**_tmp90D;_tmp423=((_tmp90D=_region_malloc(r,sizeof(*_tmp90D)),((_tmp90D[0]=_tmp42B,_tmp90D))));}
goto _LL1E1;_LL1E6:;_LL1E7:
# 1967
 goto _LL1E1;_LL1E1:;}
# 1970
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp421 != 0)Cyc_Tcutil_unify(*_tmp421,t);
# 1973
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp910;void*_tmp90F;(_tmp90F=0,Cyc_Tcutil_terr(bound->loc,((_tmp910="bound is not constant",_tag_dyneither(_tmp910,sizeof(char),22))),_tag_dyneither(_tmp90F,sizeof(void*),0)));}}
# 1977
if(_tmp423 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp423)){
# 1980
struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_uint_exp(1,0);_tmp434->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp434,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 1984
const char*_tmp913;void*_tmp912;(_tmp912=0,Cyc_Tcutil_terr(loc,((_tmp913="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp913,sizeof(char),74))),_tag_dyneither(_tmp912,sizeof(void*),0)));};}{
# 1987
void*_tmp437=Cyc_Absyn_array_typ(t,_tmp422 == 0?Cyc_Absyn_empty_tqual(0):*_tmp422,bound,
_tmp423 == 0?Cyc_Absyn_false_conref:*_tmp423,0);
void*_tmp438=_tmp437;_npop_handler(0);return _tmp438;};}
# 1950
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2003 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2007
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp914;adptr=((_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=ad,_tmp914))));};}else{
# 2013
{struct _handler_cons _tmp43A;_push_handler(& _tmp43A);{int _tmp43C=0;if(setjmp(_tmp43A.handler))_tmp43C=1;if(!_tmp43C){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2013
;_pop_handler();}else{void*_tmp43B=(void*)_exn_thrown;void*_tmp43E=_tmp43B;void*_tmp440;_LL1E9: {struct Cyc_Dict_Absent_exn_struct*_tmp43F=(struct Cyc_Dict_Absent_exn_struct*)_tmp43E;if(_tmp43F->tag != Cyc_Dict_Absent)goto _LL1EB;}_LL1EA:
# 2016
{const char*_tmp918;void*_tmp917[1];struct Cyc_String_pa_PrintArg_struct _tmp916;(_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp917[0]=& _tmp916,Cyc_Tcutil_terr(loc,((_tmp918="unbound struct/union name %s",_tag_dyneither(_tmp918,sizeof(char),29))),_tag_dyneither(_tmp917,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1EB: _tmp440=_tmp43E;_LL1EC:(void)_throw(_tmp440);_LL1E8:;}};}
# 2019
*ad_opt=ad;
*tn=ad->name;}
# 2022
if(ad->impl == 0){
{const char*_tmp920;void*_tmp91F[1];const char*_tmp91E;const char*_tmp91D;struct Cyc_String_pa_PrintArg_struct _tmp91C;(_tmp91C.tag=0,((_tmp91C.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp91D="struct",_tag_dyneither(_tmp91D,sizeof(char),7)):((_tmp91E="union",_tag_dyneither(_tmp91E,sizeof(char),6)))),((_tmp91F[0]=& _tmp91C,Cyc_Tcutil_terr(loc,((_tmp920="can't construct abstract %s",_tag_dyneither(_tmp920,sizeof(char),28))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2026
struct _RegionHandle _tmp449=_new_region("rgn");struct _RegionHandle*rgn=& _tmp449;_push_region(rgn);
# 2031
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp44A=Cyc_Tcenv_new_status(te);
if(_tmp44A == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2040
te2=Cyc_Tcenv_set_new_status(rgn,_tmp44A,te);}{
# 2042
struct _tuple12 _tmp921;struct _tuple12 _tmp44B=(_tmp921.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp921.f2=rgn,_tmp921)));
struct Cyc_List_List*_tmp44C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp44B,ad->tvs);
struct Cyc_List_List*_tmp44D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp44B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp44E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp44C);
struct Cyc_List_List*_tmp44F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp44D);
struct Cyc_List_List*_tmp450=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp44C,_tmp44D);
void*res_typ;
# 2053
if(topt != 0){
void*_stmttmp40=Cyc_Tcutil_compress(*topt);void*_tmp451=_stmttmp40;struct Cyc_Absyn_Aggrdecl**_tmp453;struct Cyc_List_List*_tmp454;_LL1EE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp451;if(_tmp452->tag != 11)goto _LL1F0;else{if((((_tmp452->f1).aggr_info).KnownAggr).tag != 2)goto _LL1F0;_tmp453=(struct Cyc_Absyn_Aggrdecl**)(((_tmp452->f1).aggr_info).KnownAggr).val;_tmp454=(_tmp452->f1).targs;}}_LL1EF:
# 2056
 if(*_tmp453 == *adptr){
{struct Cyc_List_List*_tmp455=_tmp44E;for(0;_tmp455 != 0  && _tmp454 != 0;
(_tmp455=_tmp455->tl,_tmp454=_tmp454->tl)){
Cyc_Tcutil_unify((void*)_tmp455->hd,(void*)_tmp454->hd);}}
# 2061
res_typ=*topt;
goto _LL1ED;}
# 2064
goto _LL1F1;_LL1F0:;_LL1F1: {
# 2066
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp927;struct Cyc_Absyn_AggrInfo _tmp926;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp925;res_typ=(void*)((_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925[0]=((_tmp927.tag=11,((_tmp927.f1=((_tmp926.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp926.targs=_tmp44E,_tmp926)))),_tmp927)))),_tmp925))));}_LL1ED:;}else{
# 2069
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp92D;struct Cyc_Absyn_AggrInfo _tmp92C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp92B;res_typ=(void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92D.tag=11,((_tmp92D.f1=((_tmp92C.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp92C.targs=_tmp44E,_tmp92C)))),_tmp92D)))),_tmp92B))));}{
# 2072
struct Cyc_List_List*_tmp45C=*ts;
struct Cyc_List_List*_tmp45D=_tmp44F;
while(_tmp45C != 0  && _tmp45D != 0){
# 2076
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp45C->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp45C->hd);
Cyc_Tcutil_unify((void*)_tmp45C->hd,(void*)_tmp45D->hd);
_tmp45C=_tmp45C->tl;
_tmp45D=_tmp45D->tl;}
# 2082
if(_tmp45C != 0){
const char*_tmp930;void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_terr(loc,((_tmp930="too many explicit witness types",_tag_dyneither(_tmp930,sizeof(char),32))),_tag_dyneither(_tmp92F,sizeof(void*),0)));}
# 2085
*ts=_tmp44F;{
# 2088
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp41=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp461;struct Cyc_Absyn_Exp*_tmp462;struct _tuple19*_tmp460=_stmttmp41;_tmp461=_tmp460->f1;_tmp462=_tmp460->f2;{
void*_tmp463=Cyc_Tcutil_rsubstitute(rgn,_tmp450,_tmp461->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp463,_tmp462);
# 2099
if(!Cyc_Tcutil_coerce_arg(te2,_tmp462,_tmp463,& bogus)){
{const char*_tmp93C;void*_tmp93B[5];struct Cyc_String_pa_PrintArg_struct _tmp93A;const char*_tmp939;const char*_tmp938;struct Cyc_String_pa_PrintArg_struct _tmp937;struct Cyc_String_pa_PrintArg_struct _tmp936;struct Cyc_String_pa_PrintArg_struct _tmp935;struct Cyc_String_pa_PrintArg_struct _tmp934;(_tmp934.tag=0,((_tmp934.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2103
Cyc_Absynpp_typ2string((void*)_check_null(_tmp462->topt))),((_tmp935.tag=0,((_tmp935.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2102
Cyc_Absynpp_typ2string(_tmp463)),((_tmp936.tag=0,((_tmp936.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp937.tag=0,((_tmp937.f1=(struct _dyneither_ptr)(
# 2101
ad->kind == Cyc_Absyn_StructA?(_tmp938="struct",_tag_dyneither(_tmp938,sizeof(char),7)):((_tmp939="union",_tag_dyneither(_tmp939,sizeof(char),6)))),((_tmp93A.tag=0,((_tmp93A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp461->name),((_tmp93B[0]=& _tmp93A,((_tmp93B[1]=& _tmp937,((_tmp93B[2]=& _tmp936,((_tmp93B[3]=& _tmp935,((_tmp93B[4]=& _tmp934,Cyc_Tcutil_terr(_tmp462->loc,((_tmp93C="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp93C,sizeof(char),40))),_tag_dyneither(_tmp93B,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2104
Cyc_Tcutil_explain_failure();}};}{
# 2108
struct Cyc_List_List*_tmp46D=0;
{struct Cyc_List_List*_tmp46E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp46E != 0;_tmp46E=_tmp46E->tl){
struct _tuple0*_tmp93F;struct Cyc_List_List*_tmp93E;_tmp46D=((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E->hd=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp450,(*((struct _tuple0*)_tmp46E->hd)).f1),((_tmp93F->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp450,(*((struct _tuple0*)_tmp46E->hd)).f2),_tmp93F)))))),((_tmp93E->tl=_tmp46D,_tmp93E))))));}}
# 2113
_tmp46D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46D);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp46D);{
void*_tmp471=res_typ;_npop_handler(0);return _tmp471;};};};};};}
# 2031
;_pop_region(rgn);};}
# 2119
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2121
struct _RegionHandle _tmp473=_new_region("rgn");struct _RegionHandle*rgn=& _tmp473;_push_region(rgn);{
void*_stmttmp42=Cyc_Tcutil_compress(ts);void*_tmp474=_stmttmp42;enum Cyc_Absyn_AggrKind _tmp476;struct Cyc_List_List*_tmp477;_LL1F3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp474;if(_tmp475->tag != 12)goto _LL1F5;else{_tmp476=_tmp475->f1;_tmp477=_tmp475->f2;}}_LL1F4:
# 2124
 if(_tmp476 == Cyc_Absyn_UnionA){
const char*_tmp942;void*_tmp941;(_tmp941=0,Cyc_Tcutil_terr(loc,((_tmp942="expecting struct but found union",_tag_dyneither(_tmp942,sizeof(char),33))),_tag_dyneither(_tmp941,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp477);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp43=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;struct _tuple19*_tmp47A=_stmttmp43;_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp47B->type,_tmp47C);
# 2133
if(!Cyc_Tcutil_coerce_arg(te,_tmp47C,_tmp47B->type,& bogus)){
{const char*_tmp948;void*_tmp947[3];struct Cyc_String_pa_PrintArg_struct _tmp946;struct Cyc_String_pa_PrintArg_struct _tmp945;struct Cyc_String_pa_PrintArg_struct _tmp944;(_tmp944.tag=0,((_tmp944.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2136
Cyc_Absynpp_typ2string((void*)_check_null(_tmp47C->topt))),((_tmp945.tag=0,((_tmp945.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2135
Cyc_Absynpp_typ2string(_tmp47B->type)),((_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47B->name),((_tmp947[0]=& _tmp946,((_tmp947[1]=& _tmp945,((_tmp947[2]=& _tmp944,Cyc_Tcutil_terr(_tmp47C->loc,((_tmp948="field %s of struct expects type %s != %s",_tag_dyneither(_tmp948,sizeof(char),41))),_tag_dyneither(_tmp947,sizeof(void*),3)))))))))))))))))));}
# 2137
Cyc_Tcutil_explain_failure();}}
# 2140
goto _LL1F2;};_LL1F5:;_LL1F6: {
const char*_tmp94B;void*_tmp94A;(_tmp94A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp94B="tcAnonStruct: wrong type",_tag_dyneither(_tmp94B,sizeof(char),25))),_tag_dyneither(_tmp94A,sizeof(void*),0)));}_LL1F2:;}{
# 2144
void*_tmp484=ts;_npop_handler(0);return _tmp484;};
# 2121
;_pop_region(rgn);}
# 2148
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2151
struct _RegionHandle _tmp485=_new_region("rgn");struct _RegionHandle*rgn=& _tmp485;_push_region(rgn);
{struct _tuple12 _tmp94C;struct _tuple12 _tmp486=(_tmp94C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp94C.f2=rgn,_tmp94C)));
struct Cyc_List_List*_tmp487=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp486,tud->tvs);
struct Cyc_List_List*_tmp488=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp487);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp952;struct Cyc_Absyn_DatatypeFieldInfo _tmp951;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp950;void*res=(void*)(
(_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp952.tag=4,((_tmp952.f1=((_tmp951.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp951.targs=_tmp488,_tmp951)))),_tmp952)))),_tmp950))));
# 2159
if(topt != 0){
void*_stmttmp44=Cyc_Tcutil_compress(*topt);void*_tmp489=_stmttmp44;_LL1F8: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 4)goto _LL1FA;}_LL1F9:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F7;_LL1FA:;_LL1FB:
 goto _LL1F7;_LL1F7:;}{
# 2165
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp487 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp487,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp95A;void*_tmp959[3];struct Cyc_String_pa_PrintArg_struct _tmp958;struct Cyc_String_pa_PrintArg_struct _tmp957;const char*_tmp956;struct Cyc_String_pa_PrintArg_struct _tmp955;(_tmp955.tag=0,((_tmp955.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2176
e->topt == 0?(struct _dyneither_ptr)((_tmp956="?",_tag_dyneither(_tmp956,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp957.tag=0,((_tmp957.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2175
Cyc_Absynpp_typ2string(t)),((_tmp958.tag=0,((_tmp958.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp959[0]=& _tmp958,((_tmp959[1]=& _tmp957,((_tmp959[2]=& _tmp955,Cyc_Tcutil_terr(e->loc,((_tmp95A="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp95A,sizeof(char),82))),_tag_dyneither(_tmp959,sizeof(void*),3)))))))))))))))))));}
# 2177
Cyc_Tcutil_explain_failure();}}
# 2180
if(es != 0){
const char*_tmp95E;void*_tmp95D[1];struct Cyc_String_pa_PrintArg_struct _tmp95C;void*_tmp494=(_tmp95C.tag=0,((_tmp95C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2183
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp95D[0]=& _tmp95C,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp95E="too many arguments for datatype constructor %s",_tag_dyneither(_tmp95E,sizeof(char),47))),_tag_dyneither(_tmp95D,sizeof(void*),1)))))));_npop_handler(0);return _tmp494;}
if(ts != 0){
const char*_tmp962;void*_tmp961[1];struct Cyc_String_pa_PrintArg_struct _tmp960;void*_tmp498=(_tmp960.tag=0,((_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp961[0]=& _tmp960,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp962="too few arguments for datatype constructor %s",_tag_dyneither(_tmp962,sizeof(char),46))),_tag_dyneither(_tmp961,sizeof(void*),1)))))));_npop_handler(0);return _tmp498;}{
void*_tmp499=res;_npop_handler(0);return _tmp499;};};}
# 2152
;_pop_region(rgn);}
# 2191
static int Cyc_Tcexp_zeroable_type(void*t){
void*_stmttmp45=Cyc_Tcutil_compress(t);void*_tmp49E=_stmttmp45;union Cyc_Absyn_Constraint*_tmp4A5;void*_tmp4A9;struct Cyc_List_List*_tmp4AC;union Cyc_Absyn_AggrInfoU _tmp4AE;struct Cyc_List_List*_tmp4AF;struct Cyc_List_List*_tmp4B3;_LL1FD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp49F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp49E;if(_tmp49F->tag != 0)goto _LL1FF;}_LL1FE:
 return 1;_LL1FF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4A0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp49E;if(_tmp4A0->tag != 1)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp49E;if(_tmp4A1->tag != 2)goto _LL203;}_LL202:
 goto _LL204;_LL203: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4A2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp49E;if(_tmp4A2->tag != 3)goto _LL205;}_LL204:
 goto _LL206;_LL205: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp49E;if(_tmp4A3->tag != 4)goto _LL207;}_LL206:
 return 0;_LL207: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49E;if(_tmp4A4->tag != 5)goto _LL209;else{_tmp4A5=((_tmp4A4->f1).ptr_atts).nullable;}}_LL208:
# 2199
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4A5);_LL209: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp49E;if(_tmp4A6->tag != 6)goto _LL20B;}_LL20A:
 goto _LL20C;_LL20B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4A7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp49E;if(_tmp4A7->tag != 7)goto _LL20D;}_LL20C:
 return 1;_LL20D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp49E;if(_tmp4A8->tag != 8)goto _LL20F;else{_tmp4A9=(_tmp4A8->f1).elt_type;}}_LL20E:
 return Cyc_Tcexp_zeroable_type(_tmp4A9);_LL20F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4AA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49E;if(_tmp4AA->tag != 9)goto _LL211;}_LL210:
 return 0;_LL211: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4AB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp49E;if(_tmp4AB->tag != 10)goto _LL213;else{_tmp4AC=_tmp4AB->f1;}}_LL212:
# 2205
 for(0;(unsigned int)_tmp4AC;_tmp4AC=_tmp4AC->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp4AC->hd)).f2))return 0;}
return 1;_LL213: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4AD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49E;if(_tmp4AD->tag != 11)goto _LL215;else{_tmp4AE=(_tmp4AD->f1).aggr_info;_tmp4AF=(_tmp4AD->f1).targs;}}_LL214: {
# 2209
struct Cyc_Absyn_Aggrdecl*_tmp4C0=Cyc_Absyn_get_known_aggrdecl(_tmp4AE);
if(_tmp4C0->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp4C1=_new_region("r");struct _RegionHandle*r=& _tmp4C1;_push_region(r);
{struct Cyc_List_List*_tmp4C2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4C0->tvs,_tmp4AF);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C0->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4C2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4C3=0;_npop_handler(0);return _tmp4C3;}}}{
int _tmp4C4=1;_npop_handler(0);return _tmp4C4;};}
# 2213
;_pop_region(r);};}_LL215: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp49E;if(_tmp4B0->tag != 13)goto _LL217;}_LL216:
# 2218
 return 1;_LL217: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp49E;if(_tmp4B1->tag != 19)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp49E;if(_tmp4B2->tag != 12)goto _LL21B;else{_tmp4B3=_tmp4B2->f2;}}_LL21A:
# 2221
 for(0;_tmp4B3 != 0;_tmp4B3=_tmp4B3->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4B3->hd)->type))return 0;}
return 1;_LL21B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp49E;if(_tmp4B4->tag != 14)goto _LL21D;}_LL21C:
 return 1;_LL21D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp49E;if(_tmp4B5->tag != 17)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4B6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp49E;if(_tmp4B6->tag != 16)goto _LL221;}_LL220:
 return 0;_LL221: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp49E;if(_tmp4B7->tag != 15)goto _LL223;}_LL222:
 return 0;_LL223: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp49E;if(_tmp4B8->tag != 26)goto _LL225;}_LL224:
# 2229
 goto _LL226;_LL225: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp49E;if(_tmp4B9->tag != 18)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp49E;if(_tmp4BA->tag != 20)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4BB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp49E;if(_tmp4BB->tag != 21)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4BC=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp49E;if(_tmp4BC->tag != 22)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp49E;if(_tmp4BD->tag != 23)goto _LL22F;}_LL22E:
 goto _LL230;_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp49E;if(_tmp4BE->tag != 24)goto _LL231;}_LL230:
 goto _LL232;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp49E;if(_tmp4BF->tag != 25)goto _LL1FC;}_LL232: {
const char*_tmp966;void*_tmp965[1];struct Cyc_String_pa_PrintArg_struct _tmp964;(_tmp964.tag=0,((_tmp964.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp965[0]=& _tmp964,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp966="bad type `%s' in zeroable type",_tag_dyneither(_tmp966,sizeof(char),31))),_tag_dyneither(_tmp965,sizeof(void*),1)))))));}_LL1FC:;}
# 2240
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2242
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2244
if(topt != 0){
void*_stmttmp46=Cyc_Tcutil_compress(*topt);void*_tmp4C8=_stmttmp46;void*_tmp4CA;_LL234: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8;if(_tmp4C9->tag != 5)goto _LL236;else{_tmp4CA=(_tmp4C9->f1).elt_typ;}}_LL235:
# 2247
 Cyc_Tcutil_unify(_tmp4CA,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL233;_LL236:;_LL237:
 goto _LL233;_LL233:;}{
# 2253
const char*_tmp96F;void*_tmp96E[2];const char*_tmp96D;const char*_tmp96C;struct Cyc_String_pa_PrintArg_struct _tmp96B;struct Cyc_String_pa_PrintArg_struct _tmp96A;(_tmp96A.tag=0,((_tmp96A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp96B.tag=0,((_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp96C="calloc",_tag_dyneither(_tmp96C,sizeof(char),7))):(struct _dyneither_ptr)((_tmp96D="malloc",_tag_dyneither(_tmp96D,sizeof(char),7))))),((_tmp96E[0]=& _tmp96B,((_tmp96E[1]=& _tmp96A,Cyc_Tcutil_terr(loc,((_tmp96F="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp96F,sizeof(char),60))),_tag_dyneither(_tmp96E,sizeof(void*),2)))))))))))));};}
# 2257
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2259
enum Cyc_Absyn_AliasQual _stmttmp47=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_stmttmp47){case Cyc_Absyn_Unique: _LL238:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL239:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2265
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2270
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2273
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp972;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp971;void*expected_type=(void*)(
(_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=((_tmp972.tag=15,((_tmp972.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp972)))),_tmp971))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_stmttmp48=Cyc_Tcutil_compress(handle_type);void*_tmp4D1=_stmttmp48;void*_tmp4D3;_LL23C: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4D1;if(_tmp4D2->tag != 15)goto _LL23E;else{_tmp4D3=(void*)_tmp4D2->f1;}}_LL23D:
# 2278
 rgn=_tmp4D3;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL23B;_LL23E:;_LL23F:
# 2282
{const char*_tmp976;void*_tmp975[1];struct Cyc_String_pa_PrintArg_struct _tmp974;(_tmp974.tag=0,((_tmp974.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp975[0]=& _tmp974,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp976="expecting region_t type but found %s",_tag_dyneither(_tmp976,sizeof(char),37))),_tag_dyneither(_tmp975,sizeof(void*),1)))))));}
goto _LL23B;_LL23B:;}else{
# 2289
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2296
{struct _RegionHandle _tmp4D9=_new_region("r");struct _RegionHandle*r=& _tmp4D9;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2307 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp979;void*_tmp978;(_tmp978=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp979="calloc with empty type",_tag_dyneither(_tmp979,sizeof(char),23))),_tag_dyneither(_tmp978,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2319
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4DC=er;void*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;_LL241: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4DC;if(_tmp4DD->tag != 16)goto _LL243;else{_tmp4DE=(void*)_tmp4DD->f1;}}_LL242:
# 2323
 elt_type=_tmp4DE;
{void**_tmp97A;*t=((_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A[0]=elt_type,_tmp97A))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL240;_LL243: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4DF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4DC;if(_tmp4DF->tag != 2)goto _LL245;else{if(_tmp4DF->f1 != Cyc_Absyn_Times)goto _LL245;if(_tmp4DF->f2 == 0)goto _LL245;_tmp4E0=(struct Cyc_Absyn_Exp*)(_tmp4DF->f2)->hd;if((_tmp4DF->f2)->tl == 0)goto _LL245;_tmp4E1=(struct Cyc_Absyn_Exp*)((_tmp4DF->f2)->tl)->hd;if(((_tmp4DF->f2)->tl)->tl != 0)goto _LL245;}}_LL244:
# 2330
{struct _tuple0 _tmp97B;struct _tuple0 _stmttmp4C=(_tmp97B.f1=_tmp4E0->r,((_tmp97B.f2=_tmp4E1->r,_tmp97B)));struct _tuple0 _tmp4E3=_stmttmp4C;void*_tmp4E5;void*_tmp4E7;_LL248: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E3.f1;if(_tmp4E4->tag != 16)goto _LL24A;else{_tmp4E5=(void*)_tmp4E4->f1;}}_LL249:
# 2332
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4E5);
elt_type=_tmp4E5;
{void**_tmp97C;*t=((_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C[0]=elt_type,_tmp97C))));}
num_elts=_tmp4E1;
one_elt=0;
goto _LL247;_LL24A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E3.f2;if(_tmp4E6->tag != 16)goto _LL24C;else{_tmp4E7=(void*)_tmp4E6->f1;}}_LL24B:
# 2339
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4E7);
elt_type=_tmp4E7;
{void**_tmp97D;*t=((_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D[0]=elt_type,_tmp97D))));}
num_elts=_tmp4E0;
one_elt=0;
goto _LL247;_LL24C:;_LL24D:
 goto No_sizeof;_LL247:;}
# 2347
goto _LL240;_LL245:;_LL246:
# 2349
 No_sizeof: {
# 2352
struct Cyc_Absyn_Exp*_tmp4EB=*e;
{void*_stmttmp49=_tmp4EB->r;void*_tmp4EC=_stmttmp49;struct Cyc_Absyn_Exp*_tmp4EE;_LL24F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4EC;if(_tmp4ED->tag != 13)goto _LL251;else{_tmp4EE=_tmp4ED->f2;}}_LL250:
 _tmp4EB=_tmp4EE;goto _LL24E;_LL251:;_LL252:
 goto _LL24E;_LL24E:;}
# 2357
{void*_stmttmp4A=Cyc_Tcutil_compress((void*)_check_null(_tmp4EB->topt));void*_tmp4EF=_stmttmp4A;void*_tmp4F1;_LL254: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EF;if(_tmp4F0->tag != 19)goto _LL256;else{_tmp4F1=(void*)_tmp4F0->f1;}}_LL255:
# 2359
{void*_stmttmp4B=Cyc_Tcutil_compress(_tmp4F1);void*_tmp4F2=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp4F4;_LL259: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F2;if(_tmp4F3->tag != 18)goto _LL25B;else{_tmp4F4=_tmp4F3->f1;}}_LL25A:
# 2361
 er=_tmp4F4->r;goto retry_sizeof;_LL25B:;_LL25C:
 goto _LL258;_LL258:;}
# 2364
goto _LL253;_LL256:;_LL257:
 goto _LL253;_LL253:;}
# 2367
elt_type=Cyc_Absyn_char_typ;
{void**_tmp97E;*t=((_tmp97E=_cycalloc(sizeof(*_tmp97E)),((_tmp97E[0]=elt_type,_tmp97E))));}
num_elts=*e;
one_elt=0;}
# 2372
goto _LL240;_LL240:;}}
# 2376
*is_fat=!one_elt;
# 2379
{void*_tmp4F6=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4F8;_LL25E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F6;if(_tmp4F7->tag != 11)goto _LL260;else{if((((_tmp4F7->f1).aggr_info).KnownAggr).tag != 2)goto _LL260;_tmp4F8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4F7->f1).aggr_info).KnownAggr).val);}}_LL25F:
# 2381
 if(_tmp4F8->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F8->impl))->exist_vars != 0){
const char*_tmp981;void*_tmp980;(_tmp980=0,Cyc_Tcutil_terr(loc,((_tmp981="malloc with existential types not yet implemented",_tag_dyneither(_tmp981,sizeof(char),50))),_tag_dyneither(_tmp980,sizeof(void*),0)));}
goto _LL25D;_LL260:;_LL261:
 goto _LL25D;_LL25D:;}{
# 2388
void*(*_tmp4FB)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4FC=Cyc_Absyn_false_conref;
if(topt != 0){
void*_stmttmp4D=Cyc_Tcutil_compress(*topt);void*_tmp4FD=_stmttmp4D;union Cyc_Absyn_Constraint*_tmp4FF;union Cyc_Absyn_Constraint*_tmp500;union Cyc_Absyn_Constraint*_tmp501;_LL263: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FD;if(_tmp4FE->tag != 5)goto _LL265;else{_tmp4FF=((_tmp4FE->f1).ptr_atts).nullable;_tmp500=((_tmp4FE->f1).ptr_atts).bounds;_tmp501=((_tmp4FE->f1).ptr_atts).zero_term;}}_LL264:
# 2393
 _tmp4FC=_tmp501;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4FF))
_tmp4FB=Cyc_Absyn_star_typ;
# 2398
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp501) && !(*is_calloc)){
{const char*_tmp984;void*_tmp983;(_tmp983=0,Cyc_Tcutil_warn(loc,((_tmp984="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp984,sizeof(char),55))),_tag_dyneither(_tmp983,sizeof(void*),0)));}
*is_calloc=1;}
# 2404
{void*_stmttmp4E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp500);void*_tmp504=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp507;_LL268: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp505=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp504;if(_tmp505->tag != 0)goto _LL26A;}_LL269:
 goto _LL267;_LL26A:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp506=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp504;if(_tmp506->tag != 1)goto _LL26C;else{_tmp507=_tmp506->f1;}}if(!(!one_elt))goto _LL26C;_LL26B: {
# 2407
int _tmp508=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp508  && Cyc_Evexp_same_const_exp(_tmp507,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp500),_tmp4FC);}
# 2413
{void*_stmttmp4F=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp509=_stmttmp4F;void*_tmp50B;_LL26F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp509;if(_tmp50A->tag != 19)goto _LL271;else{_tmp50B=(void*)_tmp50A->f1;}}_LL270: {
# 2415
struct Cyc_Absyn_Exp*_tmp50C=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp50B,0),0,Cyc_Absyn_No_coercion,0);
# 2417
if(Cyc_Evexp_same_const_exp(_tmp50C,_tmp507)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp500),_tmp4FC);}
# 2422
goto _LL26E;}_LL271:;_LL272:
 goto _LL26E;_LL26E:;}
# 2425
goto _LL267;}_LL26C:;_LL26D:
 goto _LL267;_LL267:;}
# 2428
goto _LL262;_LL265:;_LL266:
 goto _LL262;_LL262:;}
# 2431
if(!one_elt)_tmp4FB=Cyc_Absyn_dyneither_typ;
return _tmp4FB(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4FC);};};}
# 2436
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2442
struct _RegionHandle _tmp50D=_new_region("r");struct _RegionHandle*r=& _tmp50D;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp50E=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp50E,0,e1);{
void*_tmp50F=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp50E,& _tmp50F,e2);};}{
# 2448
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2451
{void*_stmttmp50=Cyc_Tcutil_compress(t1);void*_tmp510=_stmttmp50;_LL274: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510;if(_tmp511->tag != 8)goto _LL276;}_LL275:
{const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_Tcutil_terr(loc,((_tmp987="cannot assign to an array",_tag_dyneither(_tmp987,sizeof(char),26))),_tag_dyneither(_tmp986,sizeof(void*),0)));}goto _LL273;_LL276:;_LL277:
 goto _LL273;_LL273:;}{
# 2456
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_Tcutil_terr(loc,((_tmp98A="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp98A,sizeof(char),58))),_tag_dyneither(_tmp989,sizeof(void*),0)));}
# 2461
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp98D;void*_tmp98C;void*_tmp518=(_tmp98C=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp98D="swap non-lvalue",_tag_dyneither(_tmp98D,sizeof(char),16))),_tag_dyneither(_tmp98C,sizeof(void*),0)));_npop_handler(0);return _tmp518;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp990;void*_tmp98F;void*_tmp51B=(_tmp98F=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp990="swap non-lvalue",_tag_dyneither(_tmp990,sizeof(char),16))),_tag_dyneither(_tmp98F,sizeof(void*),0)));_npop_handler(0);return _tmp51B;}{
# 2466
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp993;void*_tmp992;void*_tmp51E=(_tmp992=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp993="swap value in zeroterm array",_tag_dyneither(_tmp993,sizeof(char),29))),_tag_dyneither(_tmp992,sizeof(void*),0)));_npop_handler(0);return _tmp51E;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp996;void*_tmp995;void*_tmp521=(_tmp995=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp996="swap value in zeroterm array",_tag_dyneither(_tmp996,sizeof(char),29))),_tag_dyneither(_tmp995,sizeof(void*),0)));_npop_handler(0);return _tmp521;}
# 2473
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp99B;void*_tmp99A[2];struct Cyc_String_pa_PrintArg_struct _tmp999;struct Cyc_String_pa_PrintArg_struct _tmp998;void*_tmp522=(_tmp998.tag=0,((_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp999.tag=0,((_tmp999.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp99A[0]=& _tmp999,((_tmp99A[1]=& _tmp998,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp99B="type mismatch: %s != %s",_tag_dyneither(_tmp99B,sizeof(char),24))),_tag_dyneither(_tmp99A,sizeof(void*),2)))))))))))));
void*_tmp523=_tmp522;_npop_handler(0);return _tmp523;}{
# 2480
void*_tmp528=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp528;};};};};
# 2442
;_pop_region(r);}
# 2484
int Cyc_Tcexp_in_stmt_exp=0;
# 2486
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp529=_new_region("r");struct _RegionHandle*r=& _tmp529;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2493
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2495
while(1){
void*_stmttmp51=s->r;void*_tmp52A=_stmttmp51;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Stmt*_tmp52E;struct Cyc_Absyn_Stmt*_tmp52F;struct Cyc_Absyn_Decl*_tmp531;struct Cyc_Absyn_Stmt*_tmp532;_LL279: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp52B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp52A;if(_tmp52B->tag != 1)goto _LL27B;else{_tmp52C=_tmp52B->f1;}}_LL27A: {
# 2499
void*_tmp533=(void*)_check_null(_tmp52C->topt);
if(!Cyc_Tcutil_unify(_tmp533,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp99F;void*_tmp99E[1];struct Cyc_String_pa_PrintArg_struct _tmp99D;(_tmp99D.tag=0,((_tmp99D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp533)),((_tmp99E[0]=& _tmp99D,Cyc_Tcutil_terr(loc,((_tmp99F="statement expression returns type %s",_tag_dyneither(_tmp99F,sizeof(char),37))),_tag_dyneither(_tmp99E,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2505
void*_tmp537=_tmp533;_npop_handler(0);return _tmp537;};}_LL27B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp52D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp52A;if(_tmp52D->tag != 2)goto _LL27D;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}_LL27C:
 s=_tmp52F;continue;_LL27D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp530=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp52A;if(_tmp530->tag != 12)goto _LL27F;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL27E:
 s=_tmp532;continue;_LL27F:;_LL280: {
# 2509
const char*_tmp9A2;void*_tmp9A1;void*_tmp53A=(_tmp9A1=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9A2="statement expression must end with expression",_tag_dyneither(_tmp9A2,sizeof(char),46))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));_npop_handler(0);return _tmp53A;}_LL278:;}
# 2487
;_pop_region(r);}
# 2514
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp53B=_new_region("r");struct _RegionHandle*r=& _tmp53B;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2519
{void*_tmp53C=t;struct Cyc_Absyn_Aggrdecl*_tmp53E;_LL282: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp53C;if(_tmp53D->tag != 11)goto _LL284;else{if((((_tmp53D->f1).aggr_info).KnownAggr).tag != 2)goto _LL284;_tmp53E=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp53D->f1).aggr_info).KnownAggr).val);}}_LL283:
# 2521
 if((_tmp53E->kind == Cyc_Absyn_UnionA  && _tmp53E->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53E->impl))->tagged)goto _LL281;
goto _LL285;_LL284:;_LL285:
# 2524
{const char*_tmp9A6;void*_tmp9A5[1];struct Cyc_String_pa_PrintArg_struct _tmp9A4;(_tmp9A4.tag=0,((_tmp9A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9A5[0]=& _tmp9A4,Cyc_Tcutil_terr(loc,((_tmp9A6="expecting @tagged union but found %s",_tag_dyneither(_tmp9A6,sizeof(char),37))),_tag_dyneither(_tmp9A5,sizeof(void*),1)))))));}
goto _LL281;_LL281:;}{
# 2527
void*_tmp542=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp542;};
# 2516
;_pop_region(r);}
# 2531
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2535
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp543=_new_region("r");struct _RegionHandle*r=& _tmp543;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp544=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2540
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9A9;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9A8;void*expected_type=(void*)(
(_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=((_tmp9A9.tag=15,((_tmp9A9.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp544)),_tmp9A9)))),_tmp9A8))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp544,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_stmttmp52=Cyc_Tcutil_compress(handle_type);void*_tmp545=_stmttmp52;void*_tmp547;_LL287: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp545;if(_tmp546->tag != 15)goto _LL289;else{_tmp547=(void*)_tmp546->f1;}}_LL288:
# 2545
 rgn=_tmp547;
Cyc_Tcenv_check_rgn_accessible(_tmp544,loc,rgn);
goto _LL286;_LL289:;_LL28A:
# 2549
{const char*_tmp9AD;void*_tmp9AC[1];struct Cyc_String_pa_PrintArg_struct _tmp9AB;(_tmp9AB.tag=0,((_tmp9AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9AC[0]=& _tmp9AB,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9AD="expecting region_t type but found %s",_tag_dyneither(_tmp9AD,sizeof(char),37))),_tag_dyneither(_tmp9AC,sizeof(void*),1)))))));}
goto _LL286;_LL286:;}else{
# 2556
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2564
void*_stmttmp53=e1->r;void*_tmp54D=_stmttmp53;struct Cyc_Core_Opt*_tmp551;struct Cyc_List_List*_tmp552;struct Cyc_List_List*_tmp554;_LL28C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp54E->tag != 26)goto _LL28E;}_LL28D:
 goto _LL28F;_LL28E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp54F->tag != 27)goto _LL290;}_LL28F: {
# 2569
void*_tmp557=Cyc_Tcexp_tcExpNoPromote(_tmp544,topt,e1);
void*_stmttmp57=Cyc_Tcutil_compress(_tmp557);void*_tmp558=_stmttmp57;void*_tmp55A;struct Cyc_Absyn_Tqual _tmp55B;struct Cyc_Absyn_Exp*_tmp55C;union Cyc_Absyn_Constraint*_tmp55D;_LL29B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp558;if(_tmp559->tag != 8)goto _LL29D;else{_tmp55A=(_tmp559->f1).elt_type;_tmp55B=(_tmp559->f1).tq;_tmp55C=(_tmp559->f1).num_elts;_tmp55D=(_tmp559->f1).zero_term;}}_LL29C: {
# 2572
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp55C);
void*b;
{void*_stmttmp58=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp55E=_stmttmp58;void*_tmp560;_LL2A0: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp55E;if(_tmp55F->tag != 19)goto _LL2A2;else{_tmp560=(void*)_tmp55F->f1;}}_LL2A1:
# 2576
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9AF;b=(void*)((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF[0]=((_tmp9B0.tag=1,((_tmp9B0.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp560,0),0,Cyc_Absyn_No_coercion,0),_tmp9B0)))),_tmp9AF))));}
goto _LL29F;_LL2A2:;_LL2A3:
# 2579
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B3;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B2;b=(void*)((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B3.tag=1,((_tmp9B3.f1=bnd,_tmp9B3)))),_tmp9B2))));}else{
# 2582
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL29F:;}{
# 2584
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9B9;struct Cyc_Absyn_PtrInfo _tmp9B8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B7;void*res_typ=(void*)(
(_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7[0]=((_tmp9B9.tag=5,((_tmp9B9.f1=((_tmp9B8.elt_typ=_tmp55A,((_tmp9B8.elt_tq=_tmp55B,((_tmp9B8.ptr_atts=(
((_tmp9B8.ptr_atts).rgn=rgn,(((_tmp9B8.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9B8.ptr_atts).bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp9B8.ptr_atts).zero_term=_tmp55D,(((_tmp9B8.ptr_atts).ptrloc=0,_tmp9B8.ptr_atts)))))))))),_tmp9B8)))))),_tmp9B9)))),_tmp9B7))));
# 2588
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp544,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp544,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2595
void*_tmp565=res_typ;_npop_handler(0);return _tmp565;};};}_LL29D:;_LL29E: {
const char*_tmp9BC;void*_tmp9BB;(_tmp9BB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9BC="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9BC,sizeof(char),45))),_tag_dyneither(_tmp9BB,sizeof(void*),0)));}_LL29A:;}_LL290: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp550->tag != 35)goto _LL292;else{_tmp551=_tmp550->f1;_tmp552=_tmp550->f2;}}_LL291:
# 2599
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9BF;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9BE;e1->r=(void*)((_tmp9BE=_cycalloc(sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=25,((_tmp9BF.f1=_tmp552,_tmp9BF)))),_tmp9BE))));}
_tmp554=_tmp552;goto _LL293;_LL292: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp553->tag != 25)goto _LL294;else{_tmp554=_tmp553->f1;}}_LL293: {
# 2602
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp55=Cyc_Tcutil_compress(*topt);void*_tmp56D=_stmttmp55;void**_tmp56F;struct Cyc_Absyn_Tqual _tmp570;union Cyc_Absyn_Constraint*_tmp571;_LL2A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56D;if(_tmp56E->tag != 5)goto _LL2A7;else{_tmp56F=(void**)&(_tmp56E->f1).elt_typ;_tmp570=(_tmp56E->f1).elt_tq;_tmp571=((_tmp56E->f1).ptr_atts).zero_term;}}_LL2A6:
# 2608
 elt_typ_opt=_tmp56F;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp571);
goto _LL2A4;_LL2A7:;_LL2A8:
 goto _LL2A4;_LL2A4:;}{
# 2614
void*_tmp572=Cyc_Tcexp_tcArray(_tmp544,e1->loc,elt_typ_opt,0,zero_term,_tmp554);
e1->topt=_tmp572;{
void*res_typ;
{void*_stmttmp56=Cyc_Tcutil_compress(_tmp572);void*_tmp573=_stmttmp56;void*_tmp575;struct Cyc_Absyn_Tqual _tmp576;struct Cyc_Absyn_Exp*_tmp577;union Cyc_Absyn_Constraint*_tmp578;_LL2AA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp573;if(_tmp574->tag != 8)goto _LL2AC;else{_tmp575=(_tmp574->f1).elt_type;_tmp576=(_tmp574->f1).tq;_tmp577=(_tmp574->f1).num_elts;_tmp578=(_tmp574->f1).zero_term;}}_LL2AB:
# 2619
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9D0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9CF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9CE;struct Cyc_Absyn_PtrInfo _tmp9CD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9CC;res_typ=(void*)(
(_tmp9CC=_cycalloc(sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9D0.tag=5,((_tmp9D0.f1=((_tmp9CD.elt_typ=_tmp575,((_tmp9CD.elt_tq=_tmp576,((_tmp9CD.ptr_atts=(
((_tmp9CD.ptr_atts).rgn=rgn,(((_tmp9CD.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9CD.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=1,((_tmp9CF.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp577),_tmp9CF)))),_tmp9CE))))),(((_tmp9CD.ptr_atts).zero_term=_tmp578,(((_tmp9CD.ptr_atts).ptrloc=0,_tmp9CD.ptr_atts)))))))))),_tmp9CD)))))),_tmp9D0)))),_tmp9CC))));}
# 2624
if(topt != 0){
# 2628
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp544,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp544,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2634
goto _LL2A9;_LL2AC:;_LL2AD: {
const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9D3="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9D3,sizeof(char),50))),_tag_dyneither(_tmp9D2,sizeof(void*),0)));}_LL2A9:;}{
# 2637
void*_tmp580=res_typ;_npop_handler(0);return _tmp580;};};};}_LL294: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp555=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp555->tag != 0)goto _LL296;else{if(((_tmp555->f1).String_c).tag != 8)goto _LL296;}}_LL295: {
# 2642
void*_tmp581=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2644
void*_tmp582=Cyc_Tcexp_tcExp(_tmp544,& _tmp581,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9D6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D5;void*_tmp585=Cyc_Absyn_atb_typ(_tmp582,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5[0]=((_tmp9D6.tag=1,((_tmp9D6.f1=Cyc_Absyn_uint_exp(1,0),_tmp9D6)))),_tmp9D5)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp585;}_LL296: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp556=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp54D;if(_tmp556->tag != 0)goto _LL298;else{if(((_tmp556->f1).Wstring_c).tag != 9)goto _LL298;}}_LL297: {
# 2649
void*_tmp586=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2651
void*_tmp587=Cyc_Tcexp_tcExp(_tmp544,& _tmp586,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9D9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D8;void*_tmp58A=Cyc_Absyn_atb_typ(_tmp587,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9D8=_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8[0]=((_tmp9D9.tag=1,((_tmp9D9.f1=Cyc_Absyn_uint_exp(1,0),_tmp9D9)))),_tmp9D8)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp58A;}_LL298:;_LL299:
# 2658
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_stmttmp54=Cyc_Tcutil_compress(*topt);void*_tmp58B=_stmttmp54;void**_tmp58D;struct Cyc_Absyn_Tqual _tmp58E;_LL2AF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58B;if(_tmp58C->tag != 5)goto _LL2B1;else{_tmp58D=(void**)&(_tmp58C->f1).elt_typ;_tmp58E=(_tmp58C->f1).elt_tq;}}_LL2B0:
# 2664
 topt2=_tmp58D;goto _LL2AE;_LL2B1: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp58B;if(_tmp58F->tag != 3)goto _LL2B3;}_LL2B2:
# 2668
 bogus=*topt;
topt2=& bogus;
goto _LL2AE;_LL2B3:;_LL2B4:
 goto _LL2AE;_LL2AE:;}{
# 2674
void*telt=Cyc_Tcexp_tcExp(_tmp544,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9DF;struct Cyc_Absyn_PtrInfo _tmp9DE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9DD;void*res_typ=(void*)(
(_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DF.tag=5,((_tmp9DF.f1=((_tmp9DE.elt_typ=telt,((_tmp9DE.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9DE.ptr_atts=(
((_tmp9DE.ptr_atts).rgn=rgn,(((_tmp9DE.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9DE.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp9DE.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp9DE.ptr_atts).ptrloc=0,_tmp9DE.ptr_atts)))))))))),_tmp9DE)))))),_tmp9DF)))),_tmp9DD))));
# 2680
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp544,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp544,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2687
void*_tmp590=res_typ;_npop_handler(0);return _tmp590;};};}_LL28B:;};}
# 2537
;_pop_region(r);}
# 2694
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp594=t;void*_tmp596;struct Cyc_Absyn_Tqual _tmp597;struct Cyc_Absyn_Exp*_tmp598;union Cyc_Absyn_Constraint*_tmp599;_LL2B6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp595=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp594;if(_tmp595->tag != 8)goto _LL2B8;else{_tmp596=(_tmp595->f1).elt_type;_tmp597=(_tmp595->f1).tq;_tmp598=(_tmp595->f1).num_elts;_tmp599=(_tmp595->f1).zero_term;}}_LL2B7: {
# 2698
struct _tuple14 _stmttmp59=Cyc_Tcutil_addressof_props(te,e);void*_tmp59B;struct _tuple14 _tmp59A=_stmttmp59;_tmp59B=_tmp59A.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9E2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E1;void*_tmp59C=_tmp598 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=((_tmp9E2.tag=1,((_tmp9E2.f1=_tmp598,_tmp9E2)))),_tmp9E1))));
t=Cyc_Absyn_atb_typ(_tmp596,_tmp59B,_tmp597,_tmp59C,_tmp599);
e->topt=t;
return t;};}_LL2B8:;_LL2B9:
# 2704
 return t;_LL2B5:;}
# 2711
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2715
struct _RegionHandle _tmp59F=_new_region("temp");struct _RegionHandle*temp=& _tmp59F;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2721
const char*_tmp9E5;void*_tmp9E4;(_tmp9E4=0,Cyc_Tcutil_terr(e->loc,((_tmp9E5="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9E5,sizeof(char),49))),_tag_dyneither(_tmp9E4,sizeof(void*),0)));}{
# 2724
void*_stmttmp5A=e->r;void*_tmp5A2=_stmttmp5A;_LL2BB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5A3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A2;if(_tmp5A3->tag != 25)goto _LL2BD;}_LL2BC:
 goto _LL2BE;_LL2BD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5A4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A2;if(_tmp5A4->tag != 26)goto _LL2BF;}_LL2BE:
 goto _LL2C0;_LL2BF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5A5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A2;if(_tmp5A5->tag != 27)goto _LL2C1;}_LL2C0:
 goto _LL2C2;_LL2C1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A2;if(_tmp5A6->tag != 0)goto _LL2C3;else{if(((_tmp5A6->f1).Wstring_c).tag != 9)goto _LL2C3;}}_LL2C2:
 goto _LL2C4;_LL2C3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A2;if(_tmp5A7->tag != 0)goto _LL2C5;else{if(((_tmp5A7->f1).String_c).tag != 8)goto _LL2C5;}}_LL2C4: {
void*_tmp5A8=t;_npop_handler(0);return _tmp5A8;}_LL2C5:;_LL2C6:
# 2731
 t=Cyc_Tcutil_compress(t);{
void*_tmp5A9=t;void*_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AC;struct Cyc_Absyn_Exp*_tmp5AD;union Cyc_Absyn_Constraint*_tmp5AE;_LL2C8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A9;if(_tmp5AA->tag != 8)goto _LL2CA;else{_tmp5AB=(_tmp5AA->f1).elt_type;_tmp5AC=(_tmp5AA->f1).tq;_tmp5AD=(_tmp5AA->f1).num_elts;_tmp5AE=(_tmp5AA->f1).zero_term;}}_LL2C9: {
# 2734
struct _tuple14 _stmttmp5B=Cyc_Tcutil_addressof_props(te,e);void*_tmp5B0;struct _tuple14 _tmp5AF=_stmttmp5B;_tmp5B0=_tmp5AF.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9E8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E7;void*b=_tmp5AD == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=1,((_tmp9E8.f1=_tmp5AD,_tmp9E8)))),_tmp9E7))));
t=Cyc_Absyn_atb_typ(_tmp5AB,_tmp5B0,_tmp5AC,b,_tmp5AE);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp5B1=t;_npop_handler(0);return _tmp5B1;};};}_LL2CA:;_LL2CB: {
# 2741
void*_tmp5B4=t;_npop_handler(0);return _tmp5B4;}_LL2C7:;};_LL2BA:;};
# 2715
;_pop_region(temp);}
# 2753 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_stmttmp5C=e->r;void*_tmp5B5=_stmttmp5C;struct Cyc_Absyn_Exp*_tmp5B7;_LL2CD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5B6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5B5;if(_tmp5B6->tag != 11)goto _LL2CF;else{_tmp5B7=_tmp5B6->f1;}}_LL2CE:
# 2757
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5B7);
_tmp5B7->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5B7->topt),0);
e->topt=_tmp5B7->topt;
goto _LL2CC;_LL2CF:;_LL2D0:
# 2763
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2766
{void*_stmttmp5D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5B8=_stmttmp5D;void*_tmp5BA;struct Cyc_Absyn_Tqual _tmp5BB;void*_tmp5BC;union Cyc_Absyn_Constraint*_tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;union Cyc_Absyn_Constraint*_tmp5BF;_LL2D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B8;if(_tmp5B9->tag != 5)goto _LL2D4;else{_tmp5BA=(_tmp5B9->f1).elt_typ;_tmp5BB=(_tmp5B9->f1).elt_tq;_tmp5BC=((_tmp5B9->f1).ptr_atts).rgn;_tmp5BD=((_tmp5B9->f1).ptr_atts).nullable;_tmp5BE=((_tmp5B9->f1).ptr_atts).bounds;_tmp5BF=((_tmp5B9->f1).ptr_atts).zero_term;}}_LL2D3:
# 2768
{void*_stmttmp5E=Cyc_Tcutil_compress(_tmp5BA);void*_tmp5C0=_stmttmp5E;struct Cyc_List_List*_tmp5C2;void*_tmp5C3;struct Cyc_Absyn_Tqual _tmp5C4;void*_tmp5C5;struct Cyc_List_List*_tmp5C6;int _tmp5C7;struct Cyc_Absyn_VarargInfo*_tmp5C8;struct Cyc_List_List*_tmp5C9;struct Cyc_List_List*_tmp5CA;_LL2D7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5C1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5C0;if(_tmp5C1->tag != 9)goto _LL2D9;else{_tmp5C2=(_tmp5C1->f1).tvars;_tmp5C3=(_tmp5C1->f1).effect;_tmp5C4=(_tmp5C1->f1).ret_tqual;_tmp5C5=(_tmp5C1->f1).ret_typ;_tmp5C6=(_tmp5C1->f1).args;_tmp5C7=(_tmp5C1->f1).c_varargs;_tmp5C8=(_tmp5C1->f1).cyc_varargs;_tmp5C9=(_tmp5C1->f1).rgn_po;_tmp5CA=(_tmp5C1->f1).attributes;}}_LL2D8:
# 2770
 if(_tmp5C2 != 0){
struct _RegionHandle _tmp5CB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5CB;_push_region(rgn);
{struct _tuple12 _tmp9E9;struct _tuple12 _tmp5CC=(_tmp9E9.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9E9.f2=rgn,_tmp9E9)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5CC,_tmp5C2);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2779
_tmp5C9=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5C9);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5C9);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9EF;struct Cyc_Absyn_FnInfo _tmp9EE;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9ED;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EF.tag=9,((_tmp9EF.f1=((_tmp9EE.tvars=0,((_tmp9EE.effect=_tmp5C3,((_tmp9EE.ret_tqual=_tmp5C4,((_tmp9EE.ret_typ=_tmp5C5,((_tmp9EE.args=_tmp5C6,((_tmp9EE.c_varargs=_tmp5C7,((_tmp9EE.cyc_varargs=_tmp5C8,((_tmp9EE.rgn_po=0,((_tmp9EE.attributes=_tmp5CA,_tmp9EE)))))))))))))))))),_tmp9EF)))),_tmp9ED)))));
# 2785
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9F5;struct Cyc_Absyn_PtrInfo _tmp9F4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5CD=(_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F5.tag=5,((_tmp9F5.f1=((_tmp9F4.elt_typ=ftyp,((_tmp9F4.elt_tq=_tmp5BB,((_tmp9F4.ptr_atts=(((_tmp9F4.ptr_atts).rgn=_tmp5BC,(((_tmp9F4.ptr_atts).nullable=_tmp5BD,(((_tmp9F4.ptr_atts).bounds=_tmp5BE,(((_tmp9F4.ptr_atts).zero_term=_tmp5BF,(((_tmp9F4.ptr_atts).ptrloc=0,_tmp9F4.ptr_atts)))))))))),_tmp9F4)))))),_tmp9F5)))),_tmp9F3)));
# 2787
struct Cyc_Absyn_Exp*_tmp5CE=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9F8;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9F7;e->r=(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=12,((_tmp9F8.f1=_tmp5CE,((_tmp9F8.f2=ts,_tmp9F8)))))),_tmp9F7))));}
e->topt=(void*)_tmp5CD;};}
# 2772
;_pop_region(rgn);}
# 2792
goto _LL2D6;_LL2D9:;_LL2DA:
 goto _LL2D6;_LL2D6:;}
# 2795
goto _LL2D1;_LL2D4:;_LL2D5:
 goto _LL2D1;_LL2D1:;}
# 2798
goto _LL2CC;_LL2CC:;}
# 2800
return(void*)_check_null(e->topt);}
# 2808
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2810
struct _RegionHandle _tmp5D8=_new_region("r");struct _RegionHandle*r=& _tmp5D8;_push_region(r);
{struct Cyc_List_List*_tmp5D9=0;
# 2813
{void*_stmttmp5F=fn_exp->r;void*_tmp5DA=_stmttmp5F;struct Cyc_List_List*_tmp5DC;_LL2DC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5DA;if(_tmp5DB->tag != 9)goto _LL2DE;else{_tmp5DC=_tmp5DB->f2;}}_LL2DD:
# 2815
{void*_stmttmp60=e->r;void*_tmp5DD=_stmttmp60;struct Cyc_List_List*_tmp5DF;_LL2E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5DE->tag != 9)goto _LL2E3;else{_tmp5DF=_tmp5DE->f2;}}_LL2E2: {
# 2817
struct Cyc_List_List*_tmp5E0=alias_arg_exps;
int _tmp5E1=0;
while(_tmp5E0 != 0){
while(_tmp5E1 != (int)_tmp5E0->hd){
if(_tmp5DC == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA02;struct Cyc_Int_pa_PrintArg_struct _tmpA01;void*_tmpA00[2];const char*_tmp9FF;void*_tmp9FE;(_tmp9FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA02.tag=1,((_tmpA02.f1=(unsigned long)((int)_tmp5E0->hd),((_tmpA01.tag=1,((_tmpA01.f1=(unsigned long)_tmp5E1,((_tmpA00[0]=& _tmpA01,((_tmpA00[1]=& _tmpA02,Cyc_aprintf(((_tmp9FF="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9FF,sizeof(char),36))),_tag_dyneither(_tmpA00,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9FE,sizeof(void*),0)));}
# 2824
++ _tmp5E1;
_tmp5DC=_tmp5DC->tl;
_tmp5DF=((struct Cyc_List_List*)_check_null(_tmp5DF))->tl;}{
# 2829
struct _tuple11 _stmttmp61=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5DC))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5DF))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct _tuple11 _tmp5E7=_stmttmp61;_tmp5E8=_tmp5E7.f1;_tmp5E9=_tmp5E7.f2;
_tmp5DC->hd=(void*)_tmp5E9;
{struct Cyc_List_List*_tmpA03;_tmp5D9=((_tmpA03=_region_malloc(r,sizeof(*_tmpA03)),((_tmpA03->hd=_tmp5E8,((_tmpA03->tl=_tmp5D9,_tmpA03))))));}
_tmp5E0=_tmp5E0->tl;};}
# 2834
goto _LL2E0;}_LL2E3:;_LL2E4: {
const char*_tmpA06;void*_tmpA05;(_tmpA05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA06="not a function call!",_tag_dyneither(_tmpA06,sizeof(char),21))),_tag_dyneither(_tmpA05,sizeof(void*),0)));}_LL2E0:;}
# 2837
goto _LL2DB;_LL2DE:;_LL2DF: {
const char*_tmpA09;void*_tmpA08;(_tmpA08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA09="not a function call!",_tag_dyneither(_tmpA09,sizeof(char),21))),_tag_dyneither(_tmpA08,sizeof(void*),0)));}_LL2DB:;}
# 2842
while(_tmp5D9 != 0){
struct Cyc_Absyn_Decl*_tmp5EF=(struct Cyc_Absyn_Decl*)_tmp5D9->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5EF,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5D9=_tmp5D9->tl;}
# 2849
e->topt=0;
e->r=fn_exp->r;}
# 2811
;_pop_region(r);}
# 2856
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2860
{void*_stmttmp62=e->r;void*_tmp5F0=_stmttmp62;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_List_List*_tmp5F5;struct Cyc_Absyn_VarargCallInfo**_tmp5F6;int*_tmp5F7;struct Cyc_Core_Opt*_tmp5F9;struct Cyc_List_List*_tmp5FA;union Cyc_Absyn_Cnst*_tmp5FC;struct _tuple1*_tmp5FE;void**_tmp5FF;enum Cyc_Absyn_Primop _tmp601;struct Cyc_List_List*_tmp602;struct Cyc_Absyn_Exp*_tmp604;enum Cyc_Absyn_Incrementor _tmp605;struct Cyc_Absyn_Exp*_tmp607;struct Cyc_Core_Opt*_tmp608;struct Cyc_Absyn_Exp*_tmp609;struct Cyc_Absyn_Exp*_tmp60B;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_Absyn_Exp*_tmp616;struct Cyc_Absyn_Exp*_tmp618;struct Cyc_List_List*_tmp619;struct Cyc_Absyn_VarargCallInfo**_tmp61A;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Exp*_tmp61E;struct Cyc_List_List*_tmp61F;void*_tmp621;struct Cyc_Absyn_Exp*_tmp622;enum Cyc_Absyn_Coercion*_tmp623;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Exp**_tmp627;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Exp*_tmp62A;void*_tmp62C;void*_tmp62E;struct Cyc_List_List*_tmp62F;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Exp*_tmp633;struct _dyneither_ptr*_tmp634;int*_tmp635;int*_tmp636;struct Cyc_Absyn_Exp*_tmp638;struct _dyneither_ptr*_tmp639;int*_tmp63A;int*_tmp63B;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_List_List*_tmp640;struct _tuple9*_tmp642;struct Cyc_List_List*_tmp643;struct Cyc_List_List*_tmp645;struct Cyc_Absyn_Stmt*_tmp647;struct Cyc_Absyn_Vardecl*_tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64B;int*_tmp64C;struct Cyc_Absyn_Exp*_tmp64E;void*_tmp64F;int*_tmp650;struct _tuple1**_tmp652;struct Cyc_List_List**_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Aggrdecl**_tmp655;void*_tmp657;struct Cyc_List_List*_tmp658;struct Cyc_List_List*_tmp65A;struct Cyc_Absyn_Datatypedecl*_tmp65B;struct Cyc_Absyn_Datatypefield*_tmp65C;struct _tuple1**_tmp65E;struct Cyc_Absyn_Enumdecl*_tmp65F;struct Cyc_Absyn_Enumfield*_tmp660;struct _tuple1**_tmp662;void*_tmp663;struct Cyc_Absyn_Enumfield*_tmp664;int*_tmp666;struct Cyc_Absyn_Exp**_tmp667;void***_tmp668;struct Cyc_Absyn_Exp**_tmp669;int*_tmp66A;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66D;struct Cyc_Absyn_Exp*_tmp66F;struct _dyneither_ptr*_tmp670;void*_tmp672;_LL2E6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5F1->tag != 11)goto _LL2E8;else{_tmp5F2=_tmp5F1->f1;}}_LL2E7:
# 2865
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5F2);
return;_LL2E8:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5F3->tag != 9)goto _LL2EA;else{_tmp5F4=_tmp5F3->f1;_tmp5F5=_tmp5F3->f2;_tmp5F6=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5F3->f3;_tmp5F7=(int*)& _tmp5F3->f4;}}if(!(!(*_tmp5F7)))goto _LL2EA;_LL2E9:
# 2871
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5F4,_tmp5F5)){
*_tmp5F7=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2875
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2878
return;_LL2EA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5F8=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5F8->tag != 35)goto _LL2EC;else{_tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F8->f2;}}_LL2EB:
# 2882
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5FA);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2EC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5FB->tag != 0)goto _LL2EE;else{_tmp5FC=(union Cyc_Absyn_Cnst*)& _tmp5FB->f1;}}_LL2ED:
# 2887
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5FC,e);goto _LL2E5;_LL2EE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5FD->tag != 1)goto _LL2F0;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=(void**)((void**)& _tmp5FD->f2);}}_LL2EF:
# 2889
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5FE,_tmp5FF);goto _LL2E5;_LL2F0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp600->tag != 2)goto _LL2F2;else{_tmp601=_tmp600->f1;_tmp602=_tmp600->f2;}}_LL2F1:
# 2891
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp601,_tmp602);goto _LL2E5;_LL2F2: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp603=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp603->tag != 4)goto _LL2F4;else{_tmp604=_tmp603->f1;_tmp605=_tmp603->f2;}}_LL2F3:
# 2893
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp604,_tmp605);goto _LL2E5;_LL2F4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp606=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp606->tag != 3)goto _LL2F6;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;_tmp609=_tmp606->f3;}}_LL2F5:
# 2895
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp607,_tmp608,_tmp609);goto _LL2E5;_LL2F6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp60A->tag != 5)goto _LL2F8;else{_tmp60B=_tmp60A->f1;_tmp60C=_tmp60A->f2;_tmp60D=_tmp60A->f3;}}_LL2F7:
# 2897
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp60B,_tmp60C,_tmp60D);goto _LL2E5;_LL2F8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp60E->tag != 6)goto _LL2FA;else{_tmp60F=_tmp60E->f1;_tmp610=_tmp60E->f2;}}_LL2F9:
# 2899
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp60F,_tmp610);goto _LL2E5;_LL2FA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp611->tag != 7)goto _LL2FC;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;}}_LL2FB:
# 2901
 t=Cyc_Tcexp_tcOr(te,loc,_tmp612,_tmp613);goto _LL2E5;_LL2FC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp614->tag != 8)goto _LL2FE;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;}}_LL2FD:
# 2903
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp615,_tmp616);goto _LL2E5;_LL2FE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp617->tag != 9)goto _LL300;else{_tmp618=_tmp617->f1;_tmp619=_tmp617->f2;_tmp61A=(struct Cyc_Absyn_VarargCallInfo**)& _tmp617->f3;}}_LL2FF: {
# 2909
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp674;_push_handler(& _tmp674);{int _tmp676=0;if(setjmp(_tmp674.handler))_tmp676=1;if(!_tmp676){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp675=(void*)_exn_thrown;void*_tmp678=_tmp675;struct _dyneither_ptr _tmp67A;void*_tmp67B;_LL339: {struct Cyc_Core_Failure_exn_struct*_tmp679=(struct Cyc_Core_Failure_exn_struct*)_tmp678;if(_tmp679->tag != Cyc_Core_Failure)goto _LL33B;else{_tmp67A=_tmp679->f1;}}_LL33A:
# 2916
 ok=0;
fn_exp=e;
goto _LL338;_LL33B: _tmp67B=_tmp678;_LL33C:(void)_throw(_tmp67B);_LL338:;}};}
# 2920
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp618,_tmp619,_tmp61A,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2927
goto _LL2E5;}_LL300: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp61B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp61B->tag != 10)goto _LL302;else{_tmp61C=_tmp61B->f1;}}_LL301:
# 2929
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp61C);goto _LL2E5;_LL302: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp61D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp61D->tag != 12)goto _LL304;else{_tmp61E=_tmp61D->f1;_tmp61F=_tmp61D->f2;}}_LL303:
# 2931
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp61E,_tmp61F);goto _LL2E5;_LL304: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp620=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp620->tag != 13)goto _LL306;else{_tmp621=(void*)_tmp620->f1;_tmp622=_tmp620->f2;_tmp623=(enum Cyc_Absyn_Coercion*)& _tmp620->f4;}}_LL305:
# 2933
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp621,_tmp622,_tmp623);goto _LL2E5;_LL306: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp624->tag != 14)goto _LL308;else{_tmp625=_tmp624->f1;}}_LL307:
# 2935
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp625);goto _LL2E5;_LL308: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp626=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp626->tag != 15)goto _LL30A;else{_tmp627=(struct Cyc_Absyn_Exp**)& _tmp626->f1;_tmp628=_tmp626->f2;}}_LL309:
# 2937
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp627,e,_tmp628);goto _LL2E5;_LL30A: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp629=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp629->tag != 17)goto _LL30C;else{_tmp62A=_tmp629->f1;}}_LL30B: {
# 2939
void*_tmp67C=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp62A);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp67C);goto _LL2E5;}_LL30C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp62B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp62B->tag != 16)goto _LL30E;else{_tmp62C=(void*)_tmp62B->f1;}}_LL30D:
# 2942
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp62C);goto _LL2E5;_LL30E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp62D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp62D->tag != 18)goto _LL310;else{_tmp62E=(void*)_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL30F:
# 2944
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp62E,_tmp62F);goto _LL2E5;_LL310: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp630->tag != 19)goto _LL312;else{_tmp631=_tmp630->f1;}}_LL311:
# 2946
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp631);goto _LL2E5;_LL312: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp632=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp632->tag != 20)goto _LL314;else{_tmp633=_tmp632->f1;_tmp634=_tmp632->f2;_tmp635=(int*)& _tmp632->f3;_tmp636=(int*)& _tmp632->f4;}}_LL313:
# 2948
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp633,_tmp634,_tmp635,_tmp636);goto _LL2E5;_LL314: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp637=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp637->tag != 21)goto _LL316;else{_tmp638=_tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=(int*)& _tmp637->f3;_tmp63B=(int*)& _tmp637->f4;}}_LL315:
# 2950
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp638,_tmp639,_tmp63A,_tmp63B);goto _LL2E5;_LL316: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp63C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp63C->tag != 22)goto _LL318;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;}}_LL317:
# 2952
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp63D,_tmp63E);goto _LL2E5;_LL318: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp63F->tag != 23)goto _LL31A;else{_tmp640=_tmp63F->f1;}}_LL319:
# 2954
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp640);goto _LL2E5;_LL31A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp641=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp641->tag != 24)goto _LL31C;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;}}_LL31B:
# 2956
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp642,_tmp643);goto _LL2E5;_LL31C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp644->tag != 25)goto _LL31E;else{_tmp645=_tmp644->f1;}}_LL31D: {
# 2960
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp63=Cyc_Tcutil_compress(*topt);void*_tmp67D=_stmttmp63;void**_tmp67F;struct Cyc_Absyn_Tqual*_tmp680;union Cyc_Absyn_Constraint*_tmp681;_LL33E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67D;if(_tmp67E->tag != 8)goto _LL340;else{_tmp67F=(void**)&(_tmp67E->f1).elt_type;_tmp680=(struct Cyc_Absyn_Tqual*)&(_tmp67E->f1).tq;_tmp681=(_tmp67E->f1).zero_term;}}_LL33F:
# 2966
 elt_topt=_tmp67F;
elt_tqopt=_tmp680;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp681);
goto _LL33D;_LL340:;_LL341:
 goto _LL33D;_LL33D:;}
# 2973
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp645);goto _LL2E5;}_LL31E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp646=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp646->tag != 36)goto _LL320;else{_tmp647=_tmp646->f1;}}_LL31F:
# 2975
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp647);goto _LL2E5;_LL320: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp648->tag != 26)goto _LL322;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;_tmp64B=_tmp648->f3;_tmp64C=(int*)& _tmp648->f4;}}_LL321:
# 2977
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp649,_tmp64A,_tmp64B,_tmp64C);goto _LL2E5;_LL322: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp64D->tag != 27)goto _LL324;else{_tmp64E=_tmp64D->f1;_tmp64F=(void*)_tmp64D->f2;_tmp650=(int*)& _tmp64D->f3;}}_LL323:
# 2979
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp64E,_tmp64F,_tmp650);goto _LL2E5;_LL324: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp651->tag != 28)goto _LL326;else{_tmp652=(struct _tuple1**)& _tmp651->f1;_tmp653=(struct Cyc_List_List**)& _tmp651->f2;_tmp654=_tmp651->f3;_tmp655=(struct Cyc_Absyn_Aggrdecl**)& _tmp651->f4;}}_LL325:
# 2981
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp652,_tmp653,_tmp654,_tmp655);goto _LL2E5;_LL326: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp656=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp656->tag != 29)goto _LL328;else{_tmp657=(void*)_tmp656->f1;_tmp658=_tmp656->f2;}}_LL327:
# 2983
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp657,_tmp658);goto _LL2E5;_LL328: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp659=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp659->tag != 30)goto _LL32A;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;_tmp65C=_tmp659->f3;}}_LL329:
# 2985
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp65A,_tmp65B,_tmp65C);goto _LL2E5;_LL32A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp65D=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp65D->tag != 31)goto _LL32C;else{_tmp65E=(struct _tuple1**)& _tmp65D->f1;_tmp65F=_tmp65D->f2;_tmp660=_tmp65D->f3;}}_LL32B:
# 2987
*_tmp65E=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp660))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA0C;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA0B;t=(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=13,((_tmpA0C.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp65F))->name,((_tmpA0C.f2=_tmp65F,_tmpA0C)))))),_tmpA0B))));}goto _LL2E5;_LL32C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp661->tag != 32)goto _LL32E;else{_tmp662=(struct _tuple1**)& _tmp661->f1;_tmp663=(void*)_tmp661->f2;_tmp664=_tmp661->f3;}}_LL32D:
# 2990
*_tmp662=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp664))->name;
t=_tmp663;goto _LL2E5;_LL32E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp665=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp665->tag != 33)goto _LL330;else{_tmp666=(int*)&(_tmp665->f1).is_calloc;_tmp667=(struct Cyc_Absyn_Exp**)&(_tmp665->f1).rgn;_tmp668=(void***)&(_tmp665->f1).elt_type;_tmp669=(struct Cyc_Absyn_Exp**)&(_tmp665->f1).num_elts;_tmp66A=(int*)&(_tmp665->f1).fat_result;}}_LL32F:
# 2993
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp667,_tmp668,_tmp669,_tmp666,_tmp66A);goto _LL2E5;_LL330: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp66B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp66B->tag != 34)goto _LL332;else{_tmp66C=_tmp66B->f1;_tmp66D=_tmp66B->f2;}}_LL331:
# 2995
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp66C,_tmp66D);goto _LL2E5;_LL332: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp66E->tag != 37)goto _LL334;else{_tmp66F=_tmp66E->f1;_tmp670=_tmp66E->f2;}}_LL333:
# 2997
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp66F,_tmp670);goto _LL2E5;_LL334: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp671->tag != 38)goto _LL336;else{_tmp672=(void*)_tmp671->f1;}}_LL335:
# 2999
 if(!te->allow_valueof){
const char*_tmpA0F;void*_tmpA0E;(_tmpA0E=0,Cyc_Tcutil_terr(e->loc,((_tmpA0F="valueof(-) can only occur within types",_tag_dyneither(_tmpA0F,sizeof(char),39))),_tag_dyneither(_tmpA0E,sizeof(void*),0)));}
# 3007
t=Cyc_Absyn_sint_typ;
goto _LL2E5;_LL336: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp673->tag != 39)goto _LL2E5;}_LL337:
# 3010
{const char*_tmpA12;void*_tmpA11;(_tmpA11=0,Cyc_Tcutil_terr(e->loc,((_tmpA12="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA12,sizeof(char),50))),_tag_dyneither(_tmpA11,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2E5:;}
# 3013
e->topt=t;}
