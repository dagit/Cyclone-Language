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
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 96
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 116
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 123
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 139 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 158
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 229 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp68B;(_tmp68B=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp68B,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp68C;return(_tmp68C=_cycalloc(sizeof(*_tmp68C)),((_tmp68C->f1=0,((_tmp68C->f2=e,_tmp68C)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_stmttmp0=e1->r;void*_tmp2=_stmttmp0;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_stmttmp1=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpA=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp690;void*_tmp68F[1];struct Cyc_String_pa_PrintArg_struct _tmp68E;(_tmp68E.tag=0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp68F[0]=& _tmp68E,Cyc_Tcutil_terr(e1->loc,((_tmp690="undeclared identifier %s",_tag_dyneither(_tmp690,sizeof(char),25))),_tag_dyneither(_tmp68F,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp694;void*_tmp693[1];struct Cyc_String_pa_PrintArg_struct _tmp692;(_tmp692.tag=0,((_tmp692.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp693[0]=& _tmp692,Cyc_Tcutil_terr(e->loc,((_tmp694="%s is a constant, not a function",_tag_dyneither(_tmp694,sizeof(char),33))),_tag_dyneither(_tmp693,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp697;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp696;e->r=(void*)((_tmp696=_cycalloc(sizeof(*_tmp696)),((_tmp696[0]=((_tmp697.tag=30,((_tmp697.f1=es,((_tmp697.f2=_tmp10,((_tmp697.f3=_tmp11,_tmp697)))))))),_tmp696))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp69A;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp699;e->r=(void*)((_tmp699=_cycalloc(sizeof(*_tmp699)),((_tmp699[0]=((_tmp69A.tag=28,((_tmp69A.f1=_tmp13->name,((_tmp69A.f2=0,((_tmp69A.f3=_tmp22,((_tmp69A.f4=_tmp13,_tmp69A)))))))))),_tmp699))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp69E;void*_tmp69D[1];struct Cyc_String_pa_PrintArg_struct _tmp69C;(_tmp69C.tag=0,((_tmp69C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp69D[0]=& _tmp69C,Cyc_Tcutil_terr(e->loc,((_tmp69E="%s is an enum constructor, not a function",_tag_dyneither(_tmp69E,sizeof(char),42))),_tag_dyneither(_tmp69D,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6A2;void*_tmp6A1[1];struct Cyc_String_pa_PrintArg_struct _tmp6A0;(_tmp6A0.tag=0,((_tmp6A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6A1[0]=& _tmp6A0,Cyc_Tcutil_terr(e1->loc,((_tmp6A2="undeclared identifier %s",_tag_dyneither(_tmp6A2,sizeof(char),25))),_tag_dyneither(_tmp6A1,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6A5;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6A4;e->r=(void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=25,((_tmp6A5.f1=des,_tmp6A5)))),_tmp6A4))));}
return;}{
# 125
void*t=*topt;
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp33=_stmttmp2;union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6A8;void*_tmp6A7;(_tmp6A7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6A8="struct type not properly set",_tag_dyneither(_tmp6A8,sizeof(char),29))),_tag_dyneither(_tmp6A7,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6AB;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6AA;e->r=(void*)((_tmp6AA=_cycalloc(sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=28,((_tmp6AB.f1=_tmp3B->name,((_tmp6AB.f2=0,((_tmp6AB.f3=des,((_tmp6AB.f4=_tmp3B,_tmp6AB)))))))))),_tmp6AA))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6AE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6AD;e->r=(void*)((_tmp6AD=_cycalloc(sizeof(*_tmp6AD)),((_tmp6AD[0]=((_tmp6AE.tag=25,((_tmp6AE.f1=des,_tmp6AE)))),_tmp6AD))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6B1;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6B0;e->r=(void*)((_tmp6B0=_cycalloc(sizeof(*_tmp6B0)),((_tmp6B0[0]=((_tmp6B1.tag=29,((_tmp6B1.f1=t,((_tmp6B1.f2=des,_tmp6B1)))))),_tmp6B0))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6B4;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6B3;e->r=(void*)((_tmp6B3=_cycalloc(sizeof(*_tmp6B3)),((_tmp6B3[0]=((_tmp6B4.tag=25,((_tmp6B4.f1=des,_tmp6B4)))),_tmp6B3))));}goto _LL17;_LL17:;};}
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
const char*_tmp6B9;void*_tmp6B8[2];struct Cyc_String_pa_PrintArg_struct _tmp6B7;struct Cyc_String_pa_PrintArg_struct _tmp6B6;(_tmp6B6.tag=0,((_tmp6B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6B7.tag=0,((_tmp6B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6B8[0]=& _tmp6B7,((_tmp6B8[1]=& _tmp6B6,Cyc_Tcutil_terr(e->loc,((_tmp6B9="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp6B9,sizeof(char),53))),_tag_dyneither(_tmp6B8,sizeof(void*),2)))))))))))));}
# 161
{void*_stmttmp3=e->r;void*_tmp4A=_stmttmp3;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp6BA;struct _tuple0 _stmttmp4=(_tmp6BA.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp6BA.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp6BA)));struct _tuple0 _tmp4E=_stmttmp4;void*_tmp50;void*_tmp52;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f1;if(_tmp4F->tag != 15)goto _LL2D;else{_tmp50=(void*)_tmp4F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f2;if(_tmp51->tag != 15)goto _LL2D;else{_tmp52=(void*)_tmp51->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6BD;struct Cyc_Tcexp_TestEnv _tmp6BC;return(_tmp6BC.eq=((_tmp6BD=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6BD)),((_tmp6BD->f1=_tmp50,((_tmp6BD->f2=_tmp52,_tmp6BD)))))),((_tmp6BC.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp6BC)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6BE;return(_tmp6BE.eq=0,((_tmp6BE.isTrue=0,_tmp6BE)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6C4;struct Cyc_Absyn_PtrInfo _tmp6C3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C2;t=(void*)((_tmp6C2=_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2[0]=((_tmp6C4.tag=5,((_tmp6C4.f1=((_tmp6C3.elt_typ=_tmp69,((_tmp6C3.elt_tq=_tmp6A,((_tmp6C3.ptr_atts=(
((_tmp6C3.ptr_atts).rgn=_tmp6B,(((_tmp6C3.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6C3.ptr_atts).bounds=_tmp6D,(((_tmp6C3.ptr_atts).zero_term=_tmp6E,(((_tmp6C3.ptr_atts).ptrloc=0,_tmp6C3.ptr_atts)))))))))),_tmp6C3)))))),_tmp6C4)))),_tmp6C2))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6C7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6C6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=18,((_tmp6C7.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp6C7)))),_tmp6C6)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6CA;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6C9;t=(void*)((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9[0]=((_tmp6CA.tag=19,((_tmp6CA.f1=(void*)_tmp76,_tmp6CA)))),_tmp6C9))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6CD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC[0]=((_tmp6CD.tag=1,((_tmp6CD.f1=elen,_tmp6CD)))),_tmp6CC)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6D3;struct Cyc_Absyn_PtrInfo _tmp6D2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6D1;return(void*)((_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1[0]=((_tmp6D3.tag=5,((_tmp6D3.f1=((_tmp6D2.elt_typ=_tmp84,((_tmp6D2.elt_tq=_tmp85,((_tmp6D2.ptr_atts=(((_tmp6D2.ptr_atts).rgn=_tmp86,(((_tmp6D2.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6D2.ptr_atts).bounds=_tmp88,(((_tmp6D2.ptr_atts).zero_term=_tmp89,(((_tmp6D2.ptr_atts).ptrloc=0,_tmp6D2.ptr_atts)))))))))),_tmp6D2)))))),_tmp6D3)))),_tmp6D1))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6E2;struct Cyc_Core_Opt*_tmp6E1;struct Cyc_Core_Opt*_tmp6E0;struct Cyc_Absyn_PtrInfo _tmp6DF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DE;t=(void*)((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6E2.tag=5,((_tmp6E2.f1=((_tmp6DF.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0->v=_tmp8D,_tmp6E0))))),((_tmp6DF.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6DF.ptr_atts=(
((_tmp6DF.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1->v=_tmp8D,_tmp6E1))))),(((_tmp6DF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6DF.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6DF.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6DF.ptr_atts).ptrloc=0,_tmp6DF.ptr_atts)))))))))),_tmp6DF)))))),_tmp6E2)))),_tmp6DE))));}
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
const char*_tmp6E6;void*_tmp6E5[1];struct Cyc_String_pa_PrintArg_struct _tmp6E4;void*_tmpB5=(_tmp6E4.tag=0,((_tmp6E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6E5[0]=& _tmp6E4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6E6="undeclared identifier %s",_tag_dyneither(_tmp6E6,sizeof(char),25))),_tag_dyneither(_tmp6E5,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6E9;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6E8;e->r=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((_tmp6E9.tag=31,((_tmp6E9.f1=_tmpAA->name,((_tmp6E9.f2=_tmpA9,((_tmp6E9.f3=_tmpAA,_tmp6E9)))))))),_tmp6E8))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6EC;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6EB;void*_tmpBB=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EC.tag=13,((_tmp6EC.f1=_tmpA9->name,((_tmp6EC.f2=_tmpA9,_tmp6EC)))))),_tmp6EB))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6EF;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6EE;e->r=(void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((_tmp6EF.tag=32,((_tmp6EF.f1=_tmpAD->name,((_tmp6EF.f2=_tmpAC,((_tmp6EF.f3=_tmpAD,_tmp6EF)))))))),_tmp6EE))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6F2;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6F1;e->r=(void*)((_tmp6F1=_cycalloc(sizeof(*_tmp6F1)),((_tmp6F1[0]=((_tmp6F2.tag=30,((_tmp6F2.f1=0,((_tmp6F2.f2=_tmpAF,((_tmp6F2.f3=_tmpB0,_tmp6F2)))))))),_tmp6F1))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6F6;void*_tmp6F5[1];struct Cyc_String_pa_PrintArg_struct _tmp6F4;void*_tmpC5=(_tmp6F4.tag=0,((_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6F5[0]=& _tmp6F4,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6F6="bad occurrence of type name %s",_tag_dyneither(_tmp6F6,sizeof(char),31))),_tag_dyneither(_tmp6F5,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6FA;void*_tmp6F9[1];struct Cyc_String_pa_PrintArg_struct _tmp6F8;return(_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6F9[0]=& _tmp6F8,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6FA="undeclared identifier %s",_tag_dyneither(_tmp6FA,sizeof(char),25))),_tag_dyneither(_tmp6F9,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6FB;(*q).f1=(((_tmp6FB.Loc_n).val=0,(((_tmp6FB.Loc_n).tag=4,_tmp6FB))));}
if(te->allow_valueof){
void*_stmttmpA=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpCE=_stmttmpA;void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6FE;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6FD;e->r=(void*)((_tmp6FD=_cycalloc(sizeof(*_tmp6FD)),((_tmp6FD[0]=((_tmp6FE.tag=38,((_tmp6FE.f1=_tmpD0,_tmp6FE)))),_tmp6FD))));}
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
const char*_tmp701;void*_tmp700;(_tmp700=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp701="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp701,sizeof(char),49))),_tag_dyneither(_tmp700,sizeof(void*),0)));}
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
{const char*_tmp706;void*_tmp705[2];struct Cyc_String_pa_PrintArg_struct _tmp704;struct Cyc_String_pa_PrintArg_struct _tmp703;(_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp705[0]=& _tmp704,((_tmp705[1]=& _tmp703,Cyc_Tcutil_terr(e->loc,((_tmp706="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp706,sizeof(char),51))),_tag_dyneither(_tmp705,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp707;*alias_arg_exps=((_tmp707=_cycalloc(sizeof(*_tmp707)),((_tmp707->hd=(void*)arg_cnt,((_tmp707->tl=*alias_arg_exps,_tmp707))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp70A;void*_tmp709;(_tmp709=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp70A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp70A,sizeof(char),49))),_tag_dyneither(_tmp709,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp70D;void*_tmp70C;(_tmp70C=0,Cyc_Tcutil_terr(fmt->loc,((_tmp70D="too few arguments",_tag_dyneither(_tmp70D,sizeof(char),18))),_tag_dyneither(_tmp70C,sizeof(void*),0)));}
if(_tmpDD != 0){
const char*_tmp710;void*_tmp70F;(_tmp70F=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp710="too many arguments",_tag_dyneither(_tmp710,sizeof(char),19))),_tag_dyneither(_tmp70F,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp714;void*_tmp713[1];struct Cyc_String_pa_PrintArg_struct _tmp712;(_tmp712.tag=0,((_tmp712.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp713[0]=& _tmp712,Cyc_Tcutil_terr(loc,((_tmp714="expecting arithmetic type but found %s",_tag_dyneither(_tmp714,sizeof(char),39))),_tag_dyneither(_tmp713,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp718;void*_tmp717[1];struct Cyc_String_pa_PrintArg_struct _tmp716;(_tmp716.tag=0,((_tmp716.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp717[0]=& _tmp716,Cyc_Tcutil_terr(loc,((_tmp718="expecting integral or * type but found %s",_tag_dyneither(_tmp718,sizeof(char),42))),_tag_dyneither(_tmp717,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp71C;void*_tmp71B[1];struct Cyc_String_pa_PrintArg_struct _tmp71A;(_tmp71A.tag=0,((_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp71B[0]=& _tmp71A,Cyc_Tcutil_terr(loc,((_tmp71C="expecting integral type but found %s",_tag_dyneither(_tmp71C,sizeof(char),37))),_tag_dyneither(_tmp71B,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF3=t;union Cyc_Absyn_Constraint*_tmpF6;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3;if(_tmpF4->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF3;if(_tmpF5->tag != 5)goto _LL9B;else{_tmpF6=((_tmpF5->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_stmttmpD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF6);void*_tmpF7=_stmttmpD;struct Cyc_Absyn_Exp*_tmpFA;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpF8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF8->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpF9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF7;if(_tmpF9->tag != 1)goto _LL9D;else{_tmpFA=_tmpF9->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFA)){
const char*_tmp71F;void*_tmp71E;(_tmp71E=0,Cyc_Tcutil_terr(loc,((_tmp71F="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp71F,sizeof(char),55))),_tag_dyneither(_tmp71E,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp723;void*_tmp722[1];struct Cyc_String_pa_PrintArg_struct _tmp721;(_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp722[0]=& _tmp721,Cyc_Tcutil_terr(loc,((_tmp723="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp723,sizeof(char),47))),_tag_dyneither(_tmp722,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp726;void*_tmp725;(_tmp725=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp726="Non-unary primop",_tag_dyneither(_tmp726,sizeof(char),17))),_tag_dyneither(_tmp725,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp72A;void*_tmp729[1];struct Cyc_String_pa_PrintArg_struct _tmp728;(_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp729[0]=& _tmp728,Cyc_Tcutil_terr(e1->loc,((_tmp72A="type %s cannot be used here",_tag_dyneither(_tmp72A,sizeof(char),28))),_tag_dyneither(_tmp729,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp72E;void*_tmp72D[1];struct Cyc_String_pa_PrintArg_struct _tmp72C;(_tmp72C.tag=0,((_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp72D[0]=& _tmp72C,Cyc_Tcutil_terr(e2->loc,((_tmp72E="type %s cannot be used here",_tag_dyneither(_tmp72E,sizeof(char),28))),_tag_dyneither(_tmp72D,sizeof(void*),1)))))));}
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
const char*_tmp731;void*_tmp730;(_tmp730=0,Cyc_Tcutil_terr(e1->loc,((_tmp731="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp731,sizeof(char),50))),_tag_dyneither(_tmp730,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp734;void*_tmp733;(_tmp733=0,Cyc_Tcutil_terr(e1->loc,((_tmp734="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp734,sizeof(char),54))),_tag_dyneither(_tmp733,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp738;void*_tmp737[1];struct Cyc_String_pa_PrintArg_struct _tmp736;(_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp737[0]=& _tmp736,Cyc_Tcutil_terr(e2->loc,((_tmp738="expecting int but found %s",_tag_dyneither(_tmp738,sizeof(char),27))),_tag_dyneither(_tmp737,sizeof(void*),1)))))));}{
void*_stmttmpE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp10E);void*_tmp117=_stmttmpE;struct Cyc_Absyn_Exp*_tmp11A;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp118=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp117;if(_tmp118->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp119=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp117;if(_tmp119->tag != 1)goto _LLA8;else{_tmp11A=_tmp119->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10F)){
const char*_tmp73B;void*_tmp73A;(_tmp73A=0,Cyc_Tcutil_warn(e1->loc,((_tmp73B="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp73B,sizeof(char),70))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp741;struct Cyc_Absyn_PtrInfo _tmp740;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11D=(_tmp73F=_cycalloc(sizeof(*_tmp73F)),((_tmp73F[0]=((_tmp741.tag=5,((_tmp741.f1=((_tmp740.elt_typ=_tmp10A,((_tmp740.elt_tq=_tmp10B,((_tmp740.ptr_atts=(
((_tmp740.ptr_atts).rgn=_tmp10C,(((_tmp740.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp740.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp740.ptr_atts).zero_term=_tmp10F,(((_tmp740.ptr_atts).ptrloc=0,_tmp740.ptr_atts)))))))))),_tmp740)))))),_tmp741)))),_tmp73F)));
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
{const char*_tmp746;void*_tmp745[2];struct Cyc_String_pa_PrintArg_struct _tmp744;struct Cyc_String_pa_PrintArg_struct _tmp743;(_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp744.tag=0,((_tmp744.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp745[0]=& _tmp744,((_tmp745[1]=& _tmp743,Cyc_Tcutil_terr(e1->loc,((_tmp746="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp746,sizeof(char),59))),_tag_dyneither(_tmp745,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp74B;void*_tmp74A[2];struct Cyc_String_pa_PrintArg_struct _tmp749;struct Cyc_String_pa_PrintArg_struct _tmp748;(_tmp748.tag=0,((_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp74A[0]=& _tmp749,((_tmp74A[1]=& _tmp748,Cyc_Tcutil_terr(e1->loc,((_tmp74B="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp74B,sizeof(char),59))),_tag_dyneither(_tmp74A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp74E;void*_tmp74D;(_tmp74D=0,Cyc_Tcutil_warn(e1->loc,((_tmp74E="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp74E,sizeof(char),60))),_tag_dyneither(_tmp74D,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp751;void*_tmp750;(_tmp750=0,Cyc_Tcutil_terr(e1->loc,((_tmp751="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp751,sizeof(char),50))),_tag_dyneither(_tmp750,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp754;void*_tmp753;(_tmp753=0,Cyc_Tcutil_terr(e1->loc,((_tmp754="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp754,sizeof(char),54))),_tag_dyneither(_tmp753,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp759;void*_tmp758[2];struct Cyc_String_pa_PrintArg_struct _tmp757;struct Cyc_String_pa_PrintArg_struct _tmp756;(_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp757.tag=0,((_tmp757.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp758[0]=& _tmp757,((_tmp758[1]=& _tmp756,Cyc_Tcutil_terr(e2->loc,((_tmp759="expecting either %s or int but found %s",_tag_dyneither(_tmp759,sizeof(char),40))),_tag_dyneither(_tmp758,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp75C;void*_tmp75B;(_tmp75B=0,Cyc_Tcutil_warn(e1->loc,((_tmp75C="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp75C,sizeof(char),60))),_tag_dyneither(_tmp75B,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp75F;void*_tmp75E;(_tmp75E=0,Cyc_Tcutil_warn(e1->loc,((_tmp75F="thin pointer subtraction!",_tag_dyneither(_tmp75F,sizeof(char),26))),_tag_dyneither(_tmp75E,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp762;void*_tmp761;(_tmp761=0,Cyc_Tcutil_warn(e1->loc,((_tmp762="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp762,sizeof(char),56))),_tag_dyneither(_tmp761,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp765;void*_tmp764;(_tmp764=0,Cyc_Tcutil_warn(e1->loc,((_tmp765="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp765,sizeof(char),51))),_tag_dyneither(_tmp764,sizeof(void*),0)));}
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
struct _tuple0 _tmp766;struct _tuple0 _stmttmpF=(_tmp766.f1=Cyc_Tcutil_compress(t1),((_tmp766.f2=Cyc_Tcutil_compress(t2),_tmp766)));struct _tuple0 _tmp13B=_stmttmpF;void*_tmp13D;void*_tmp13F;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B.f1;if(_tmp13C->tag != 5)goto _LLB0;else{_tmp13D=(_tmp13C->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13B.f2;if(_tmp13E->tag != 5)goto _LLB0;else{_tmp13F=(_tmp13E->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp13D,_tmp13F))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13B.f1;if(_tmp140->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13B.f2;if(_tmp141->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp76B;void*_tmp76A[2];struct Cyc_String_pa_PrintArg_struct _tmp769;struct Cyc_String_pa_PrintArg_struct _tmp768;(_tmp768.tag=0,((_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp76A[0]=& _tmp769,((_tmp76A[1]=& _tmp768,Cyc_Tcutil_terr(loc,((_tmp76B="comparison not allowed between %s and %s",_tag_dyneither(_tmp76B,sizeof(char),41))),_tag_dyneither(_tmp76A,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp76C;struct _tuple0 _stmttmp10=(_tmp76C.f1=t1,((_tmp76C.f2=t2,_tmp76C)));struct _tuple0 _tmp147=_stmttmp10;void*_tmp149;void*_tmp14B;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp147.f1;if(_tmp148->tag != 15)goto _LLB7;else{_tmp149=(void*)_tmp148->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp147.f2;if(_tmp14A->tag != 15)goto _LLB7;else{_tmp14B=(void*)_tmp14A->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp771;void*_tmp770[2];struct Cyc_String_pa_PrintArg_struct _tmp76F;struct Cyc_String_pa_PrintArg_struct _tmp76E;(_tmp76E.tag=0,((_tmp76E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp76F.tag=0,((_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp770[0]=& _tmp76F,((_tmp770[1]=& _tmp76E,Cyc_Tcutil_terr(loc,((_tmp771="comparison not allowed between %s and %s",_tag_dyneither(_tmp771,sizeof(char),41))),_tag_dyneither(_tmp770,sizeof(void*),2)))))))))))));}
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
const char*_tmp774;void*_tmp773;(_tmp773=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp774="bad binary primop",_tag_dyneither(_tmp774,sizeof(char),18))),_tag_dyneither(_tmp773,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _stmttmp11=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_stmttmp11){case 0: _LLCC: {
const char*_tmp777;void*_tmp776;return(_tmp776=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp777="primitive operator has 0 arguments",_tag_dyneither(_tmp777,sizeof(char),35))),_tag_dyneither(_tmp776,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp77A;void*_tmp779;return(_tmp779=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp77A="primitive operator has > 2 arguments",_tag_dyneither(_tmp77A,sizeof(char),37))),_tag_dyneither(_tmp779,sizeof(void*),0)));}}}
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
{const char*_tmp77E;void*_tmp77D[1];struct Cyc_String_pa_PrintArg_struct _tmp77C;(_tmp77C.tag=0,((_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp164->name),((_tmp77D[0]=& _tmp77C,Cyc_Tcutil_terr(loc,((_tmp77E="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp77E,sizeof(char),56))),_tag_dyneither(_tmp77D,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp164->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp158;if(_tmp15D->tag != 4)goto _LLD8;else{if((((_tmp15D->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp15E=((struct _tuple2)(((_tmp15D->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp168=_tmp15E->typs;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
struct _tuple18*_stmttmp13=(struct _tuple18*)_tmp168->hd;struct Cyc_Absyn_Tqual _tmp16A;void*_tmp16B;struct _tuple18*_tmp169=_stmttmp13;_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;
if(_tmp16A.real_const){
{const char*_tmp782;void*_tmp781[1];struct Cyc_String_pa_PrintArg_struct _tmp780;(_tmp780.tag=0,((_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp15E->name)),((_tmp781[0]=& _tmp780,Cyc_Tcutil_terr(loc,((_tmp782="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp782,sizeof(char),64))),_tag_dyneither(_tmp781,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16B))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp15F->tag != 8)goto _LLDA;else{_tmp160=(_tmp15F->f1).elt_type;_tmp161=(_tmp15F->f1).tq;}}_LLD9:
# 745
 if(_tmp161.real_const){
{const char*_tmp785;void*_tmp784;(_tmp784=0,Cyc_Tcutil_terr(loc,((_tmp785="attempt to over-write a const array",_tag_dyneither(_tmp785,sizeof(char),36))),_tag_dyneither(_tmp784,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp160);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp158;if(_tmp162->tag != 10)goto _LLDC;else{_tmp163=_tmp162->f1;}}_LLDB:
# 751
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct _tuple18*_stmttmp12=(struct _tuple18*)_tmp163->hd;struct Cyc_Absyn_Tqual _tmp172;void*_tmp173;struct _tuple18*_tmp171=_stmttmp12;_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;
if(_tmp172.real_const){
{const char*_tmp788;void*_tmp787;(_tmp787=0,Cyc_Tcutil_terr(loc,((_tmp788="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp788,sizeof(char),56))),_tag_dyneither(_tmp787,sizeof(void*),0)));}
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
{const char*_tmp78B;void*_tmp78A;(_tmp78A=0,Cyc_Tcutil_terr(e->loc,((_tmp78B="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp78B,sizeof(char),47))),_tag_dyneither(_tmp78A,sizeof(void*),0)));}
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
const char*_tmp78F;void*_tmp78E[1];struct Cyc_String_pa_PrintArg_struct _tmp78D;(_tmp78D.tag=0,((_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp78E[0]=& _tmp78D,Cyc_Tcutil_terr(e->loc,((_tmp78F="attempt to write a const location: %s",_tag_dyneither(_tmp78F,sizeof(char),38))),_tag_dyneither(_tmp78E,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1BC=_new_region("temp");struct _RegionHandle*temp=& _tmp1BC;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp792;void*_tmp791;void*_tmp1BF=(_tmp791=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp792="increment/decrement of non-lvalue",_tag_dyneither(_tmp792,sizeof(char),34))),_tag_dyneither(_tmp791,sizeof(void*),0)));_npop_handler(0);return _tmp1BF;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_Tcutil_terr(e->loc,((_tmp795="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp795,sizeof(char),50))),_tag_dyneither(_tmp794,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_terr(e->loc,((_tmp798="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp798,sizeof(char),54))),_tag_dyneither(_tmp797,sizeof(void*),0)));}}else{
# 861
const char*_tmp79C;void*_tmp79B[1];struct Cyc_String_pa_PrintArg_struct _tmp79A;(_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp79B[0]=& _tmp79A,Cyc_Tcutil_terr(e->loc,((_tmp79C="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp79C,sizeof(char),44))),_tag_dyneither(_tmp79B,sizeof(void*),1)))))));}}{
# 863
void*_tmp1C7=t;_npop_handler(0);return _tmp1C7;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _RegionHandle _tmp1C8=_new_region("r");struct _RegionHandle*r=& _tmp1C8;_push_region(r);{
const char*_tmp79D;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp79D="conditional expression",_tag_dyneither(_tmp79D,sizeof(char),23))));}
# 872
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 878
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp79E;struct Cyc_List_List _tmp1CA=(_tmp79E.hd=e3,((_tmp79E.tl=0,_tmp79E)));
struct Cyc_List_List _tmp79F;struct Cyc_List_List _tmp1CB=(_tmp79F.hd=e2,((_tmp79F.tl=& _tmp1CA,_tmp79F)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1CB)){
{const char*_tmp7A4;void*_tmp7A3[2];struct Cyc_String_pa_PrintArg_struct _tmp7A2;struct Cyc_String_pa_PrintArg_struct _tmp7A1;(_tmp7A1.tag=0,((_tmp7A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7A3[0]=& _tmp7A2,((_tmp7A3[1]=& _tmp7A1,Cyc_Tcutil_terr(loc,((_tmp7A4="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7A4,sizeof(char),48))),_tag_dyneither(_tmp7A3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 886
void*_tmp1D0=t;_npop_handler(0);return _tmp1D0;};};};
# 869
;_pop_region(r);}
# 890
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 892
{const char*_tmp7A5;Cyc_Tcexp_tcTest(te,e1,((_tmp7A5="logical-and expression",_tag_dyneither(_tmp7A5,sizeof(char),23))));}
{const char*_tmp7A6;Cyc_Tcexp_tcTest(te,e2,((_tmp7A6="logical-and expression",_tag_dyneither(_tmp7A6,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 898
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 900
{const char*_tmp7A7;Cyc_Tcexp_tcTest(te,e1,((_tmp7A7="logical-or expression",_tag_dyneither(_tmp7A7,sizeof(char),22))));}
{const char*_tmp7A8;Cyc_Tcexp_tcTest(te,e2,((_tmp7A8="logical-or expression",_tag_dyneither(_tmp7A8,sizeof(char),22))));}
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
{const char*_tmp7AB;void*_tmp7AA;(_tmp7AA=0,Cyc_Tcutil_terr(loc,((_tmp7AB="cannot assign to an array",_tag_dyneither(_tmp7AB,sizeof(char),26))),_tag_dyneither(_tmp7AA,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 925
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7AE;void*_tmp7AD;(_tmp7AD=0,Cyc_Tcutil_terr(loc,((_tmp7AE="type is abstract (can't determine size).",_tag_dyneither(_tmp7AE,sizeof(char),41))),_tag_dyneither(_tmp7AD,sizeof(void*),0)));}
# 929
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp7B1;void*_tmp7B0;void*_tmp1E0=(_tmp7B0=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B1="assignment to non-lvalue",_tag_dyneither(_tmp7B1,sizeof(char),25))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));_npop_handler(0);return _tmp1E0;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_Tcutil_terr(e2->loc,((_tmp7B4="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7B4,sizeof(char),49))),_tag_dyneither(_tmp7B3,sizeof(void*),0)));}
# 937
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7B9;void*_tmp7B8[2];struct Cyc_String_pa_PrintArg_struct _tmp7B7;struct Cyc_String_pa_PrintArg_struct _tmp7B6;void*_tmp1E4=(_tmp7B6.tag=0,((_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B8[0]=& _tmp7B7,((_tmp7B8[1]=& _tmp7B6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B9="type mismatch: %s != %s",_tag_dyneither(_tmp7B9,sizeof(char),24))),_tag_dyneither(_tmp7B8,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1E5=_tmp1E4;_npop_handler(1);return _tmp1E5;};}
# 933
;_pop_region(temp);}else{
# 945
enum Cyc_Absyn_Primop _tmp1EA=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1EB=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EA,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1EB,t1) || Cyc_Tcutil_coerceable(_tmp1EB) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7BE;void*_tmp7BD[2];struct Cyc_String_pa_PrintArg_struct _tmp7BC;struct Cyc_String_pa_PrintArg_struct _tmp7BB;void*_tmp1EC=
(_tmp7BB.tag=0,((_tmp7BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 951
Cyc_Absynpp_typ2string(t2)),((_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 950
Cyc_Absynpp_typ2string(t1)),((_tmp7BD[0]=& _tmp7BC,((_tmp7BD[1]=& _tmp7BB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7BE="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7BE,sizeof(char),82))),_tag_dyneither(_tmp7BD,sizeof(void*),2)))))))))))));
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
{const char*_tmp7C3;void*_tmp7C2[2];struct Cyc_String_pa_PrintArg_struct _tmp7C1;struct Cyc_String_pa_PrintArg_struct _tmp7C0;(_tmp7C0.tag=0,((_tmp7C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7C1.tag=0,((_tmp7C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7C2[0]=& _tmp7C1,((_tmp7C2[1]=& _tmp7C0,Cyc_Tcutil_terr(e->loc,((_tmp7C3="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7C3,sizeof(char),54))),_tag_dyneither(_tmp7C2,sizeof(void*),2)))))))))))));}
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
const char*_tmp7C6;void*_tmp7C5;(_tmp7C5=0,Cyc_Tcutil_terr(e->loc,((_tmp7C6="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp7C6,sizeof(char),79))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));}
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
{const char*_tmp7CB;void*_tmp7CA[2];struct Cyc_String_pa_PrintArg_struct _tmp7C9;struct Cyc_String_pa_PrintArg_struct _tmp7C8;(_tmp7C8.tag=0,((_tmp7C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1057
Cyc_Absynpp_typ2string(t2)),((_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7CA[0]=& _tmp7C9,((_tmp7CA[1]=& _tmp7C8,Cyc_Tcutil_terr(e1->loc,((_tmp7CB="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp7CB,sizeof(char),57))),_tag_dyneither(_tmp7CA,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1062
if(alias_coercion){
struct Cyc_List_List*_tmp7CC;*alias_arg_exps=((_tmp7CC=_cycalloc(sizeof(*_tmp7CC)),((_tmp7CC->hd=(void*)_tmp209,((_tmp7CC->tl=*alias_arg_exps,_tmp7CC))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7CF;void*_tmp7CE;(_tmp7CE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7CF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7CF,sizeof(char),49))),_tag_dyneither(_tmp7CE,sizeof(void*),0)));}
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
struct Cyc_Core_Opt*_tmp7D0;fmt_args=((_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp22D - 1),_tmp7D0))));}
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
{const char*_tmp7D3;void*_tmp7D2;(_tmp7D2=0,Cyc_Tcutil_terr(loc,((_tmp7D3="bad format arguments",_tag_dyneither(_tmp7D3,sizeof(char),21))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp237=_tmp235;_LL141:(void)_throw(_tmp237);_LL13D:;}};}
# 1104
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1108
if(_tmp21B != 0){const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(loc,((_tmp7D6="too few arguments for function",_tag_dyneither(_tmp7D6,sizeof(char),31))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}else{
# 1110
if((_tmp208 != 0  || _tmp21C) || _tmp21D != 0){
if(_tmp21C)
for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
Cyc_Tcexp_tcExp(_tmp20C,0,(struct Cyc_Absyn_Exp*)_tmp208->hd);}else{
if(_tmp21D == 0){
const char*_tmp7D9;void*_tmp7D8;(_tmp7D8=0,Cyc_Tcutil_terr(loc,((_tmp7D9="too many arguments for function",_tag_dyneither(_tmp7D9,sizeof(char),32))),_tag_dyneither(_tmp7D8,sizeof(void*),0)));}else{
# 1117
struct Cyc_Absyn_VarargInfo _stmttmp22=*_tmp21D;void*_tmp23F;int _tmp240;struct Cyc_Absyn_VarargInfo _tmp23E=_stmttmp22;_tmp23F=_tmp23E.type;_tmp240=_tmp23E.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7DA;struct Cyc_Absyn_VarargCallInfo*_tmp241=(_tmp7DA=_cycalloc(sizeof(*_tmp7DA)),((_tmp7DA->num_varargs=0,((_tmp7DA->injectors=0,((_tmp7DA->vai=_tmp21D,_tmp7DA)))))));
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
{const char*_tmp7DF;void*_tmp7DE[2];struct Cyc_String_pa_PrintArg_struct _tmp7DD;struct Cyc_String_pa_PrintArg_struct _tmp7DC;(_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7DD.tag=0,((_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7DE[0]=& _tmp7DD,((_tmp7DE[1]=& _tmp7DC,Cyc_Tcutil_terr(loc,((_tmp7DF="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7DF,sizeof(char),49))),_tag_dyneither(_tmp7DE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1135
if(alias_coercion){
struct Cyc_List_List*_tmp7E0;*alias_arg_exps=((_tmp7E0=_cycalloc(sizeof(*_tmp7E0)),((_tmp7E0->hd=(void*)_tmp209,((_tmp7E0->tl=*alias_arg_exps,_tmp7E0))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23F) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7E3;void*_tmp7E2;(_tmp7E2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7E3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7E3,sizeof(char),49))),_tag_dyneither(_tmp7E2,sizeof(void*),0)));}}else{
# 1144
void*_stmttmp23=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23F));void*_tmp249=_stmttmp23;struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_List_List*_tmp24C;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 3)goto _LL145;else{if((((_tmp24A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp24B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp24A->f1).datatype_info).KnownDatatype).val);_tmp24C=(_tmp24A->f1).targs;}}_LL144: {
# 1148
struct Cyc_Absyn_Datatypedecl*_tmp24D=*Cyc_Tcenv_lookup_datatypedecl(_tmp20C,loc,_tmp24B->name);
struct Cyc_List_List*fields=0;
if(_tmp24D->fields == 0){
const char*_tmp7E7;void*_tmp7E6[1];struct Cyc_String_pa_PrintArg_struct _tmp7E5;(_tmp7E5.tag=0,((_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7E6[0]=& _tmp7E5,Cyc_Tcutil_terr(loc,((_tmp7E7="can't inject into abstract datatype %s",_tag_dyneither(_tmp7E7,sizeof(char),39))),_tag_dyneither(_tmp7E6,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24D->fields))->v;}
# 1160
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23F),Cyc_Tcenv_curr_rgn(_tmp20C))){
const char*_tmp7EC;void*_tmp7EB[2];struct Cyc_String_pa_PrintArg_struct _tmp7EA;struct Cyc_String_pa_PrintArg_struct _tmp7E9;(_tmp7E9.tag=0,((_tmp7E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20C))),((_tmp7EA.tag=0,((_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7EB[0]=& _tmp7EA,((_tmp7EB[1]=& _tmp7E9,Cyc_Tcutil_terr(loc,((_tmp7EC="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7EC,sizeof(char),49))),_tag_dyneither(_tmp7EB,sizeof(void*),2)))))))))))));}{
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
const char*_tmp7EF;void*_tmp7EE;(_tmp7EE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7EF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7EF,sizeof(char),49))),_tag_dyneither(_tmp7EE,sizeof(void*),0)));}}{
# 1176
struct Cyc_Absyn_Datatypefield*_tmp259=Cyc_Tcexp_tcInjection(_tmp20C,e1,Cyc_Tcutil_pointer_elt_type(_tmp23F),rgn,_tmp256,fields);
# 1178
if(_tmp259 != 0){
struct Cyc_List_List*_tmp7F0;_tmp241->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp241->injectors,(
(_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0->hd=_tmp259,((_tmp7F0->tl=0,_tmp7F0)))))));}};};}}
# 1184
_npop_handler(0);goto _LL142;
# 1164
;_pop_region(rgn);};}_LL145:;_LL146:
# 1185
{const char*_tmp7F3;void*_tmp7F2;(_tmp7F2=0,Cyc_Tcutil_terr(loc,((_tmp7F3="bad inject vararg type",_tag_dyneither(_tmp7F3,sizeof(char),23))),_tag_dyneither(_tmp7F2,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1190
if(*alias_arg_exps == 0)
# 1199 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp20C,loc,(void*)_check_null(_tmp218));{
# 1201
void*_tmp25E=_tmp21A;_npop_handler(0);return _tmp25E;};};_LL133:;_LL134: {
const char*_tmp7F6;void*_tmp7F5;void*_tmp261=(_tmp7F5=0,Cyc_Tcexp_expr_err(_tmp20C,loc,topt,((_tmp7F6="expected pointer to function",_tag_dyneither(_tmp7F6,sizeof(char),29))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));_npop_handler(0);return _tmp261;}_LL130:;};_LL12E:;_LL12F: {
# 1204
const char*_tmp7F9;void*_tmp7F8;void*_tmp264=(_tmp7F8=0,Cyc_Tcexp_expr_err(_tmp20C,loc,topt,((_tmp7F9="expected pointer to function",_tag_dyneither(_tmp7F9,sizeof(char),29))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));_npop_handler(0);return _tmp264;}_LL12B:;};}
# 1023 "tcexp.cyc"
;_pop_region(ter);}
# 1210 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp265=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp265,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp265,& bogus)){
const char*_tmp7FE;void*_tmp7FD[2];struct Cyc_String_pa_PrintArg_struct _tmp7FC;struct Cyc_String_pa_PrintArg_struct _tmp7FB;(_tmp7FB.tag=0,((_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7FC.tag=0,((_tmp7FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1215
Cyc_Absynpp_typ2string(_tmp265)),((_tmp7FD[0]=& _tmp7FC,((_tmp7FD[1]=& _tmp7FB,Cyc_Tcutil_terr(loc,((_tmp7FE="expected %s but found %s",_tag_dyneither(_tmp7FE,sizeof(char),25))),_tag_dyneither(_tmp7FD,sizeof(void*),2)))))))))))));}
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
struct _tuple13*_tmp801;struct Cyc_List_List*_tmp800;instantiation=(
(_tmp800=_region_malloc(temp,sizeof(*_tmp800)),((_tmp800->hd=((_tmp801=_region_malloc(temp,sizeof(*_tmp801)),((_tmp801->f1=(struct Cyc_Absyn_Tvar*)_tmp274->hd,((_tmp801->f2=(void*)ts->hd,_tmp801)))))),((_tmp800->tl=instantiation,_tmp800))))));};}
# 1242
if(ts != 0){
const char*_tmp804;void*_tmp803;void*_tmp282=
(_tmp803=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp804="too many type variables in instantiation",_tag_dyneither(_tmp804,sizeof(char),41))),_tag_dyneither(_tmp803,sizeof(void*),0)));_npop_handler(0);return _tmp282;}
# 1248
if(_tmp274 == 0){
_tmp27B=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp27B);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp27B);
_tmp27B=0;}{
# 1253
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp80A;struct Cyc_Absyn_FnInfo _tmp809;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp808;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp808=_cycalloc(sizeof(*_tmp808)),((_tmp808[0]=((_tmp80A.tag=9,((_tmp80A.f1=((_tmp809.tvars=_tmp274,((_tmp809.effect=_tmp275,((_tmp809.ret_tqual=_tmp276,((_tmp809.ret_typ=_tmp277,((_tmp809.args=_tmp278,((_tmp809.c_varargs=_tmp279,((_tmp809.cyc_varargs=_tmp27A,((_tmp809.rgn_po=_tmp27B,((_tmp809.attributes=_tmp27C,_tmp809)))))))))))))))))),_tmp80A)))),_tmp808)))));
# 1257
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp810;struct Cyc_Absyn_PtrInfo _tmp80F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80E;void*_tmp286=(void*)((_tmp80E=_cycalloc(sizeof(*_tmp80E)),((_tmp80E[0]=((_tmp810.tag=5,((_tmp810.f1=((_tmp80F.elt_typ=new_fn_typ,((_tmp80F.elt_tq=_tmp26D,((_tmp80F.ptr_atts=(((_tmp80F.ptr_atts).rgn=_tmp26E,(((_tmp80F.ptr_atts).nullable=_tmp26F,(((_tmp80F.ptr_atts).bounds=_tmp270,(((_tmp80F.ptr_atts).zero_term=_tmp271,(((_tmp80F.ptr_atts).ptrloc=0,_tmp80F.ptr_atts)))))))))),_tmp80F)))))),_tmp810)))),_tmp80E))));_npop_handler(0);return _tmp286;};}
# 1233
;_pop_region(temp);}_LL14F:;_LL150:
# 1259
 goto _LL14C;_LL14C:;}
# 1261
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1264
const char*_tmp814;void*_tmp813[1];struct Cyc_String_pa_PrintArg_struct _tmp812;return(_tmp812.tag=0,((_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp813[0]=& _tmp812,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp814="expecting polymorphic type but found %s",_tag_dyneither(_tmp814,sizeof(char),40))),_tag_dyneither(_tmp813,sizeof(void*),1)))))));};};}
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
const char*_tmp819;void*_tmp818[2];struct Cyc_String_pa_PrintArg_struct _tmp817;struct Cyc_String_pa_PrintArg_struct _tmp816;void*_tmp28D=(_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp818[0]=& _tmp817,((_tmp818[1]=& _tmp816,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp819="cannot cast %s to %s",_tag_dyneither(_tmp819,sizeof(char),21))),_tag_dyneither(_tmp818,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp28D;};}}}
# 1297
{struct _tuple0 _tmp81A;struct _tuple0 _stmttmp25=(_tmp81A.f1=e->r,((_tmp81A.f2=Cyc_Tcutil_compress(t),_tmp81A)));struct _tuple0 _tmp292=_stmttmp25;int _tmp294;union Cyc_Absyn_Constraint*_tmp296;union Cyc_Absyn_Constraint*_tmp297;union Cyc_Absyn_Constraint*_tmp298;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp293=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp292.f1;if(_tmp293->tag != 33)goto _LL154;else{_tmp294=(_tmp293->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp295=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292.f2;if(_tmp295->tag != 5)goto _LL154;else{_tmp296=((_tmp295->f1).ptr_atts).nullable;_tmp297=((_tmp295->f1).ptr_atts).bounds;_tmp298=((_tmp295->f1).ptr_atts).zero_term;}};_LL153:
# 1301
 if((_tmp294  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp298)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp296)){
void*_stmttmp26=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp297);void*_tmp299=_stmttmp26;struct Cyc_Absyn_Exp*_tmp29B;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp299;if(_tmp29A->tag != 1)goto _LL159;else{_tmp29B=_tmp29A->f1;}}_LL158:
# 1304
 if((Cyc_Evexp_eval_const_uint_exp(_tmp29B)).f1 == 1){
const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_warn(loc,((_tmp81D="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp81D,sizeof(char),60))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}
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
const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(e->loc,((_tmp820="Cannot take the address of an alias-free path",_tag_dyneither(_tmp820,sizeof(char),46))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}
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
{const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(e->loc,((_tmp823="cannot take the address of a @tagged union member",_tag_dyneither(_tmp823,sizeof(char),50))),_tag_dyneither(_tmp822,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1374
struct _tuple14 _stmttmp2B=Cyc_Tcutil_addressof_props(te,e);int _tmp2B7;void*_tmp2B8;struct _tuple14 _tmp2B6=_stmttmp2B;_tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B8,0)){
const char*_tmp826;void*_tmp825;(_tmp825=0,Cyc_Tcutil_terr(e->loc,((_tmp826="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp826,sizeof(char),60))),_tag_dyneither(_tmp825,sizeof(void*),0)));}{
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
const char*_tmp82A;void*_tmp829[1];struct Cyc_String_pa_PrintArg_struct _tmp828;(_tmp828.tag=0,((_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp829[0]=& _tmp828,Cyc_Tcutil_terr(loc,((_tmp82A="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp82A,sizeof(char),55))),_tag_dyneither(_tmp829,sizeof(void*),1)))))));}
if(topt != 0){
void*_stmttmp2C=Cyc_Tcutil_compress(*topt);void*_tmp2BF=_stmttmp2C;void*_tmp2C0;void*_tmp2C2;_LL172: _tmp2C0=_tmp2BF;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2C0;if(_tmp2C1->tag != 19)goto _LL174;else{_tmp2C2=(void*)_tmp2C1->f1;}};_LL173: {
# 1406
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp82D;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp82C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C4=(_tmp82C=_cycalloc(sizeof(*_tmp82C)),((_tmp82C[0]=((_tmp82D.tag=18,((_tmp82D.f1=_tmp2C3,_tmp82D)))),_tmp82C)));
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
const char*_tmp831;void*_tmp830[1];struct Cyc_String_pa_PrintArg_struct _tmp82F;(_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CC),((_tmp830[0]=& _tmp82F,Cyc_Tcutil_terr(loc,((_tmp831="no field of struct/union has name %s",_tag_dyneither(_tmp831,sizeof(char),37))),_tag_dyneither(_tmp830,sizeof(void*),1)))))));}else{
# 1440
_tmp2C8=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2CF;if(_tmp2D2->tag != 12)goto _LL180;else{_tmp2D3=_tmp2D2->f2;}}_LL17F: {
# 1444
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CC,_tmp2D3);
if(!((unsigned int)t2)){
const char*_tmp835;void*_tmp834[1];struct Cyc_String_pa_PrintArg_struct _tmp833;(_tmp833.tag=0,((_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CC),((_tmp834[0]=& _tmp833,Cyc_Tcutil_terr(loc,((_tmp835="no field of struct/union has name %s",_tag_dyneither(_tmp835,sizeof(char),37))),_tag_dyneither(_tmp834,sizeof(void*),1)))))));}else{
# 1448
_tmp2C8=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1453
if(bad_type){
if(_tmp2C7 == fs){
const char*_tmp839;void*_tmp838[1];struct Cyc_String_pa_PrintArg_struct _tmp837;(_tmp837.tag=0,((_tmp837.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp838[0]=& _tmp837,Cyc_Tcutil_terr(loc,((_tmp839="%s is not a known struct/union type",_tag_dyneither(_tmp839,sizeof(char),36))),_tag_dyneither(_tmp838,sizeof(void*),1)))))));}else{
# 1457
const char*_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_PrintArg_struct _tmp83B;struct _dyneither_ptr _tmp2DD=(_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp83C[0]=& _tmp83B,Cyc_aprintf(((_tmp83D="(%s)",_tag_dyneither(_tmp83D,sizeof(char),5))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2C7;x=x->tl){
void*_stmttmp2F=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp2DE=_stmttmp2F;struct _dyneither_ptr*_tmp2E0;unsigned int _tmp2E2;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2DF=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2DE;if(_tmp2DF->tag != 0)goto _LL185;else{_tmp2E0=_tmp2DF->f1;}}_LL184:
# 1462
{const char*_tmp842;void*_tmp841[2];struct Cyc_String_pa_PrintArg_struct _tmp840;struct Cyc_String_pa_PrintArg_struct _tmp83F;_tmp2DD=((_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E0),((_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp841[0]=& _tmp840,((_tmp841[1]=& _tmp83F,Cyc_aprintf(((_tmp842="%s.%s",_tag_dyneither(_tmp842,sizeof(char),6))),_tag_dyneither(_tmp841,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2E1=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2DE;if(_tmp2E1->tag != 1)goto _LL182;else{_tmp2E2=_tmp2E1->f1;}}_LL186:
# 1464
{const char*_tmp847;void*_tmp846[2];struct Cyc_String_pa_PrintArg_struct _tmp845;struct Cyc_Int_pa_PrintArg_struct _tmp844;_tmp2DD=((_tmp844.tag=1,((_tmp844.f1=(unsigned long)((int)_tmp2E2),((_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp846[0]=& _tmp845,((_tmp846[1]=& _tmp844,Cyc_aprintf(((_tmp847="%s.%d",_tag_dyneither(_tmp847,sizeof(char),6))),_tag_dyneither(_tmp846,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1466
const char*_tmp84C;void*_tmp84B[2];struct Cyc_String_pa_PrintArg_struct _tmp84A;struct Cyc_String_pa_PrintArg_struct _tmp849;(_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp84A.tag=0,((_tmp84A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DD),((_tmp84B[0]=& _tmp84A,((_tmp84B[1]=& _tmp849,Cyc_Tcutil_terr(loc,((_tmp84C="%s == %s is not a struct/union type",_tag_dyneither(_tmp84C,sizeof(char),36))),_tag_dyneither(_tmp84B,sizeof(void*),2)))))))))))));};}}
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
const char*_tmp851;void*_tmp850[2];struct Cyc_Int_pa_PrintArg_struct _tmp84F;struct Cyc_Int_pa_PrintArg_struct _tmp84E;(_tmp84E.tag=1,((_tmp84E.f1=(unsigned long)((int)_tmp2CE),((_tmp84F.tag=1,((_tmp84F.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F6),((_tmp850[0]=& _tmp84F,((_tmp850[1]=& _tmp84E,Cyc_Tcutil_terr(loc,((_tmp851="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp851,sizeof(char),46))),_tag_dyneither(_tmp850,sizeof(void*),2)))))))))))));}else{
# 1482
_tmp2C8=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F6,(int)_tmp2CE))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F7->tag != 10)goto _LL18E;else{_tmp2F8=_tmp2F7->f1;}}_LL18D:
# 1486
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8)<= _tmp2CE){
const char*_tmp856;void*_tmp855[2];struct Cyc_Int_pa_PrintArg_struct _tmp854;struct Cyc_Int_pa_PrintArg_struct _tmp853;(_tmp853.tag=1,((_tmp853.f1=(unsigned long)((int)_tmp2CE),((_tmp854.tag=1,((_tmp854.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F8),((_tmp855[0]=& _tmp854,((_tmp855[1]=& _tmp853,Cyc_Tcutil_terr(loc,((_tmp856="tuple has too few components: %d <= %d",_tag_dyneither(_tmp856,sizeof(char),39))),_tag_dyneither(_tmp855,sizeof(void*),2)))))))))))));}else{
# 1490
_tmp2C8=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F8,(int)_tmp2CE)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2F2;if(_tmp2F9->tag != 4)goto _LL190;else{if((((_tmp2F9->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp2FA=((struct _tuple2)(((_tmp2F9->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1494
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FA->typs)< _tmp2CE){
const char*_tmp85B;void*_tmp85A[2];struct Cyc_Int_pa_PrintArg_struct _tmp859;struct Cyc_Int_pa_PrintArg_struct _tmp858;(_tmp858.tag=1,((_tmp858.f1=(unsigned long)((int)_tmp2CE),((_tmp859.tag=1,((_tmp859.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FA->typs),((_tmp85A[0]=& _tmp859,((_tmp85A[1]=& _tmp858,Cyc_Tcutil_terr(loc,((_tmp85B="datatype field has too few components: %d < %d",_tag_dyneither(_tmp85B,sizeof(char),47))),_tag_dyneither(_tmp85A,sizeof(void*),2)))))))))))));}else{
# 1498
if(_tmp2CE != 0)
_tmp2C8=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2FA->typs,(int)(_tmp2CE - 1))).f2;else{
if(_tmp2C7->tl != 0){
const char*_tmp85E;void*_tmp85D;(_tmp85D=0,Cyc_Tcutil_terr(loc,((_tmp85E="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp85E,sizeof(char),68))),_tag_dyneither(_tmp85D,sizeof(void*),0)));}}}
# 1503
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1507
if(bad_type){
const char*_tmp862;void*_tmp861[1];struct Cyc_String_pa_PrintArg_struct _tmp860;(_tmp860.tag=0,((_tmp860.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C8)),((_tmp861[0]=& _tmp860,Cyc_Tcutil_terr(loc,((_tmp862="%s is not a known type",_tag_dyneither(_tmp862,sizeof(char),23))),_tag_dyneither(_tmp861,sizeof(void*),1)))))));}
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
struct Cyc_Core_Opt*_tmp863;void*_tmp316=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863->v=_tmp315,_tmp863)))));
struct Cyc_Core_Opt*_tmp864;void*_tmp317=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp864=_cycalloc(sizeof(*_tmp864)),((_tmp864->v=_tmp315,_tmp864)))));
union Cyc_Absyn_Constraint*_tmp318=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp319=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp865;struct Cyc_Absyn_PtrAtts _tmp31A=(_tmp865.rgn=_tmp317,((_tmp865.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp865.bounds=_tmp318,((_tmp865.zero_term=_tmp319,((_tmp865.ptrloc=0,_tmp865)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp86B;struct Cyc_Absyn_PtrInfo _tmp86A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp869;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31B=(_tmp869=_cycalloc(sizeof(*_tmp869)),((_tmp869[0]=((_tmp86B.tag=5,((_tmp86B.f1=((_tmp86A.elt_typ=_tmp316,((_tmp86A.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp86A.ptr_atts=_tmp31A,_tmp86A)))))),_tmp86B)))),_tmp869)));
Cyc_Tcutil_unify(t,(void*)_tmp31B);
_tmp311=_tmp316;_tmp312=_tmp317;_tmp313=_tmp318;_tmp314=_tmp319;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30E;if(_tmp310->tag != 5)goto _LL197;else{_tmp311=(_tmp310->f1).elt_typ;_tmp312=((_tmp310->f1).ptr_atts).rgn;_tmp313=((_tmp310->f1).ptr_atts).bounds;_tmp314=((_tmp310->f1).ptr_atts).zero_term;}}_LL196:
# 1533
 Cyc_Tcenv_check_rgn_accessible(_tmp30D,loc,_tmp312);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp313);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp311),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp30D)){
const char*_tmp86E;void*_tmp86D;(_tmp86D=0,Cyc_Tcutil_terr(loc,((_tmp86E="can't dereference abstract pointer type",_tag_dyneither(_tmp86E,sizeof(char),40))),_tag_dyneither(_tmp86D,sizeof(void*),0)));}{
void*_tmp324=_tmp311;_npop_handler(0);return _tmp324;};_LL197:;_LL198: {
# 1539
const char*_tmp872;void*_tmp871[1];struct Cyc_String_pa_PrintArg_struct _tmp870;void*_tmp328=(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp871[0]=& _tmp870,Cyc_Tcexp_expr_err(_tmp30D,loc,topt,((_tmp872="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp872,sizeof(char),39))),_tag_dyneither(_tmp871,sizeof(void*),1)))))));_npop_handler(0);return _tmp328;}_LL192:;};}
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
const char*_tmp877;void*_tmp876[2];struct Cyc_String_pa_PrintArg_struct _tmp875;struct Cyc_String_pa_PrintArg_struct _tmp874;void*_tmp33A=(_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp32C->name)),((_tmp876[0]=& _tmp875,((_tmp876[1]=& _tmp874,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp877="type %s has no %s field",_tag_dyneither(_tmp877,sizeof(char),24))),_tag_dyneither(_tmp876,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp33A;}
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
const char*_tmp87B;void*_tmp87A[1];struct Cyc_String_pa_PrintArg_struct _tmp879;void*_tmp340=(_tmp879.tag=0,((_tmp879.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87A[0]=& _tmp879,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87B="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp87B,sizeof(char),56))),_tag_dyneither(_tmp87A,sizeof(void*),1)))))));_npop_handler(0);return _tmp340;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32C->impl))->exist_vars != 0){
# 1576
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_PrintArg_struct _tmp87D;void*_tmp344=(_tmp87D.tag=0,((_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87E[0]=& _tmp87D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87F="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp87F,sizeof(char),81))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))));_npop_handler(0);return _tmp344;}}{
# 1579
void*_tmp345=t2;_npop_handler(0);return _tmp345;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32A;if(_tmp32E->tag != 12)goto _LL19E;else{_tmp32F=_tmp32E->f1;_tmp330=_tmp32E->f2;}}_LL19D: {
# 1581
struct Cyc_Absyn_Aggrfield*_tmp346=Cyc_Absyn_lookup_field(_tmp330,f);
if(_tmp346 == 0){
const char*_tmp883;void*_tmp882[1];struct Cyc_String_pa_PrintArg_struct _tmp881;void*_tmp34A=(_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp882[0]=& _tmp881,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp883="type has no %s field",_tag_dyneither(_tmp883,sizeof(char),21))),_tag_dyneither(_tmp882,sizeof(void*),1)))))));_npop_handler(0);return _tmp34A;}
# 1586
if(((_tmp32F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp346->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp346->requires_clause == 0){
# 1588
const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_PrintArg_struct _tmp885;void*_tmp34E=(_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp886[0]=& _tmp885,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp887="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp887,sizeof(char),56))),_tag_dyneither(_tmp886,sizeof(void*),1)))))));_npop_handler(0);return _tmp34E;}{
void*_tmp34F=_tmp346->type;_npop_handler(0);return _tmp34F;};}_LL19E:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32A;if(_tmp331->tag != 8)goto _LL1A0;}{const char*_tmp888;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp888="size",_tag_dyneither(_tmp888,sizeof(char),5))))== 0))goto _LL1A0;};_LL19F: goto _LL1A1;_LL1A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32A;if(_tmp333->tag != 5)goto _LL1A2;}{const char*_tmp889;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp889="size",_tag_dyneither(_tmp889,sizeof(char),5))))== 0))goto _LL1A2;};_LL1A1:
{const char*_tmp88D;void*_tmp88C[1];struct Cyc_String_pa_PrintArg_struct _tmp88B;(_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp88C[0]=& _tmp88B,Cyc_Tcutil_warn(e->loc,((_tmp88D="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp88D,sizeof(char),54))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp893;struct Cyc_List_List*_tmp892;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp891;outer_e->r=(void*)((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891[0]=((_tmp893.tag=2,((_tmp893.f1=Cyc_Absyn_Numelts,((_tmp893.f2=((_tmp892=_cycalloc(sizeof(*_tmp892)),((_tmp892->hd=e,((_tmp892->tl=0,_tmp892)))))),_tmp893)))))),_tmp891))));}{
void*_tmp356=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp356;};_LL1A2:;_LL1A3: {
# 1596
const char*_tmp894;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp894="size",_tag_dyneither(_tmp894,sizeof(char),5))))== 0){
const char*_tmp898;void*_tmp897[1];struct Cyc_String_pa_PrintArg_struct _tmp896;void*_tmp35B=(_tmp896.tag=0,((_tmp896.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1599
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp897[0]=& _tmp896,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp898="expecting struct, union, or array, found %s",_tag_dyneither(_tmp898,sizeof(char),44))),_tag_dyneither(_tmp897,sizeof(void*),1)))))));_npop_handler(0);return _tmp35B;}else{
# 1601
const char*_tmp89C;void*_tmp89B[1];struct Cyc_String_pa_PrintArg_struct _tmp89A;void*_tmp35F=(_tmp89A.tag=0,((_tmp89A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp89B[0]=& _tmp89A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89C="expecting struct or union, found %s",_tag_dyneither(_tmp89C,sizeof(char),36))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))));_npop_handler(0);return _tmp35F;}}_LL199:;};
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
const char*_tmp8A1;void*_tmp8A0[2];struct Cyc_String_pa_PrintArg_struct _tmp89F;struct Cyc_String_pa_PrintArg_struct _tmp89E;void*_tmp373=(_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp89F.tag=0,((_tmp89F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp369->name)),((_tmp8A0[0]=& _tmp89F,((_tmp8A0[1]=& _tmp89E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A1="type %s has no %s field",_tag_dyneither(_tmp8A1,sizeof(char),24))),_tag_dyneither(_tmp8A0,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp373;}
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
const char*_tmp8A5;void*_tmp8A4[1];struct Cyc_String_pa_PrintArg_struct _tmp8A3;void*_tmp37C=(_tmp8A3.tag=0,((_tmp8A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A4[0]=& _tmp8A3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A5="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8A5,sizeof(char),48))),_tag_dyneither(_tmp8A4,sizeof(void*),1)))))));_npop_handler(0);return _tmp37C;}_LL1B0:;}
# 1646
if((((_tmp369->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp36E->requires_clause == 0){
# 1649
const char*_tmp8A9;void*_tmp8A8[1];struct Cyc_String_pa_PrintArg_struct _tmp8A7;void*_tmp380=(_tmp8A7.tag=0,((_tmp8A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A8[0]=& _tmp8A7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A9="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8A9,sizeof(char),56))),_tag_dyneither(_tmp8A8,sizeof(void*),1)))))));_npop_handler(0);return _tmp380;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp369->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8AD;void*_tmp8AC[1];struct Cyc_String_pa_PrintArg_struct _tmp8AB;void*_tmp384=(_tmp8AB.tag=0,((_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AC[0]=& _tmp8AB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AD="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8AD,sizeof(char),72))),_tag_dyneither(_tmp8AC,sizeof(void*),1)))))));_npop_handler(0);return _tmp384;}}{
# 1656
void*_tmp385=t3;_npop_handler(0);return _tmp385;};};};}_LL1AC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp367;if(_tmp36B->tag != 12)goto _LL1AE;else{_tmp36C=_tmp36B->f1;_tmp36D=_tmp36B->f2;}}_LL1AD: {
# 1658
struct Cyc_Absyn_Aggrfield*_tmp386=Cyc_Absyn_lookup_field(_tmp36D,f);
if(_tmp386 == 0){
const char*_tmp8B1;void*_tmp8B0[1];struct Cyc_String_pa_PrintArg_struct _tmp8AF;void*_tmp38A=(_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B0[0]=& _tmp8AF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B1="type has no %s field",_tag_dyneither(_tmp8B1,sizeof(char),21))),_tag_dyneither(_tmp8B0,sizeof(void*),1)))))));_npop_handler(0);return _tmp38A;}
# 1663
if((_tmp36C == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp386->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp8B5;void*_tmp8B4[1];struct Cyc_String_pa_PrintArg_struct _tmp8B3;void*_tmp38E=(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B4[0]=& _tmp8B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B5="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8B5,sizeof(char),56))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))));_npop_handler(0);return _tmp38E;}{
void*_tmp38F=_tmp386->type;_npop_handler(0);return _tmp38F;};}_LL1AE:;_LL1AF:
 goto _LL1A9;_LL1A9:;}
# 1668
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}{
# 1671
const char*_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_PrintArg_struct _tmp8B7;void*_tmp393=(_tmp8B7.tag=0,((_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8B8[0]=& _tmp8B7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B9="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8B9,sizeof(char),44))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));_npop_handler(0);return _tmp393;};
# 1610
;_pop_region(r);}
# 1676
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1678
struct _tuple15 _stmttmp34=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp395;int _tmp396;struct _tuple15 _tmp394=_stmttmp34;_tmp395=_tmp394.f1;_tmp396=_tmp394.f2;
if(!_tmp396){
const char*_tmp8BC;void*_tmp8BB;return(_tmp8BB=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8BC="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8BC,sizeof(char),47))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}{
# 1682
struct _handler_cons _tmp399;_push_handler(& _tmp399);{int _tmp39B=0;if(setjmp(_tmp399.handler))_tmp39B=1;if(!_tmp39B){
{void*_tmp39C=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp395)).f2;_npop_handler(0);return _tmp39C;};_pop_handler();}else{void*_tmp39A=(void*)_exn_thrown;void*_tmp39E=_tmp39A;void*_tmp3A0;_LL1B6: {struct Cyc_List_Nth_exn_struct*_tmp39F=(struct Cyc_List_Nth_exn_struct*)_tmp39E;if(_tmp39F->tag != Cyc_List_Nth)goto _LL1B8;}_LL1B7: {
# 1685
const char*_tmp8C1;void*_tmp8C0[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BF;struct Cyc_Int_pa_PrintArg_struct _tmp8BE;return(_tmp8BE.tag=1,((_tmp8BE.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8BF.tag=1,((_tmp8BF.f1=(unsigned long)((int)_tmp395),((_tmp8C0[0]=& _tmp8BF,((_tmp8C0[1]=& _tmp8BE,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8C1="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8C1,sizeof(char),41))),_tag_dyneither(_tmp8C0,sizeof(void*),2)))))))))))));}_LL1B8: _tmp3A0=_tmp39E;_LL1B9:(void)_throw(_tmp3A0);_LL1B5:;}};};}
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
const char*_tmp8C5;void*_tmp8C4[1];struct Cyc_String_pa_PrintArg_struct _tmp8C3;void*_tmp3AA=(_tmp8C3.tag=0,((_tmp8C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8C4[0]=& _tmp8C3,Cyc_Tcexp_expr_err(_tmp3A6,e2->loc,topt,((_tmp8C5="expecting int subscript, found %s",_tag_dyneither(_tmp8C5,sizeof(char),34))),_tag_dyneither(_tmp8C4,sizeof(void*),1)))))));_npop_handler(0);return _tmp3AA;}{
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
const char*_tmp8C8;void*_tmp8C7;(_tmp8C7=0,Cyc_Tcutil_warn(e2->loc,((_tmp8C8="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8C8,sizeof(char),63))),_tag_dyneither(_tmp8C7,sizeof(void*),0)));}}else{
# 1725
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp38=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3C1;int _tmp3C2;struct _tuple15 _tmp3C0=_stmttmp38;_tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;
if(_tmp3C2)
Cyc_Tcutil_check_bound(loc,_tmp3C1,_tmp3B0);}else{
# 1731
if(Cyc_Tcutil_is_bound_one(_tmp3B0) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B1)){
const char*_tmp8CB;void*_tmp8CA;(_tmp8CA=0,Cyc_Tcutil_warn(e1->loc,((_tmp8CB="subscript applied to pointer to one object",_tag_dyneither(_tmp8CB,sizeof(char),43))),_tag_dyneither(_tmp8CA,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3B0);}}
# 1736
Cyc_Tcenv_check_rgn_accessible(_tmp3A6,loc,_tmp3AF);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3AD),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3A6)){
const char*_tmp8CE;void*_tmp8CD;(_tmp8CD=0,Cyc_Tcutil_terr(e1->loc,((_tmp8CE="can't subscript an abstract pointer",_tag_dyneither(_tmp8CE,sizeof(char),36))),_tag_dyneither(_tmp8CD,sizeof(void*),0)));}{
void*_tmp3C7=_tmp3AD;_npop_handler(0);return _tmp3C7;};_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AB;if(_tmp3B2->tag != 10)goto _LL1BF;else{_tmp3B3=_tmp3B2->f1;}}_LL1BE: {
void*_tmp3C8=Cyc_Tcexp_ithTupleType(_tmp3A6,loc,_tmp3B3,e2);_npop_handler(0);return _tmp3C8;}_LL1BF:;_LL1C0: {
const char*_tmp8D2;void*_tmp8D1[1];struct Cyc_String_pa_PrintArg_struct _tmp8D0;void*_tmp3CC=(_tmp8D0.tag=0,((_tmp8D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8D1[0]=& _tmp8D0,Cyc_Tcexp_expr_err(_tmp3A6,loc,topt,((_tmp8D2="subscript applied to %s",_tag_dyneither(_tmp8D2,sizeof(char),24))),_tag_dyneither(_tmp8D1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CC;}_LL1BA:;};};}
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
struct _tuple18*_tmp8D5;struct Cyc_List_List*_tmp8D4;fields=((_tmp8D4=_cycalloc(sizeof(*_tmp8D4)),((_tmp8D4->hd=((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5->f1=(*((struct _tuple18*)_tmp3CF->hd)).f1,((_tmp8D5->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8D5)))))),((_tmp8D4->tl=fields,_tmp8D4))))));};}
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
struct _tuple18*_tmp8D8;struct Cyc_List_List*_tmp8D7;fields=((_tmp8D7=_cycalloc(sizeof(*_tmp8D7)),((_tmp8D7->hd=((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8->f1=Cyc_Absyn_empty_tqual(0),((_tmp8D8->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8D8)))))),((_tmp8D7->tl=fields,_tmp8D7))))));};
# 1775
;_pop_region(r);}{
# 1779
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8DB;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8DA;return(void*)((_tmp8DA=_cycalloc(sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=10,((_tmp8DB.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8DB)))),_tmp8DA))));};}
# 1783
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1786
const char*_tmp8DE;void*_tmp8DD;return(_tmp8DD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DE="tcCompoundLit",_tag_dyneither(_tmp8DE,sizeof(char),14))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}
# 1796 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1799
void*res_t2;
struct _RegionHandle _tmp3DB=_new_region("r");struct _RegionHandle*r=& _tmp3DB;_push_region(r);{
int _tmp3DC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8E1;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8E0;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3DD=(_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=0,((_tmp8E1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3DC),_tmp8E1)))),_tmp8E0)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3DD,loc);
# 1808
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3DE=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3DC - 1);
if(!Cyc_Tcutil_is_zero(_tmp3DE)){
const char*_tmp8E4;void*_tmp8E3;(_tmp8E3=0,Cyc_Tcutil_terr(_tmp3DE->loc,((_tmp8E4="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8E4,sizeof(char),45))),_tag_dyneither(_tmp8E3,sizeof(void*),0)));}}
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
const char*_tmp8E8;void*_tmp8E7[1];struct Cyc_String_pa_PrintArg_struct _tmp8E6;(_tmp8E6.tag=0,((_tmp8E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8E7[0]=& _tmp8E6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8E8="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8E8,sizeof(char),53))),_tag_dyneither(_tmp8E7,sizeof(void*),1)))))));}}
# 1827
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1832
void*_stmttmp3A=(void*)ds->hd;void*_tmp3E6=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3E9;_LL1CC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E6;if(_tmp3E7->tag != 1)goto _LL1CE;}_LL1CD:
# 1834
{const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_terr(loc,((_tmp8EB="only array index designators are supported",_tag_dyneither(_tmp8EB,sizeof(char),43))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3E8=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3E6;if(_tmp3E8->tag != 0)goto _LL1CB;else{_tmp3E9=_tmp3E8->f1;}}_LL1CF:
# 1837
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3E9);{
struct _tuple15 _stmttmp3B=Cyc_Evexp_eval_const_uint_exp(_tmp3E9);unsigned int _tmp3ED;int _tmp3EE;struct _tuple15 _tmp3EC=_stmttmp3B;_tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;
if(!_tmp3EE){
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_terr(_tmp3E9->loc,((_tmp8EE="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8EE,sizeof(char),47))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}else{
if(_tmp3ED != offset){
const char*_tmp8F3;void*_tmp8F2[2];struct Cyc_Int_pa_PrintArg_struct _tmp8F1;struct Cyc_Int_pa_PrintArg_struct _tmp8F0;(_tmp8F0.tag=1,((_tmp8F0.f1=(unsigned long)((int)_tmp3ED),((_tmp8F1.tag=1,((_tmp8F1.f1=(unsigned long)offset,((_tmp8F2[0]=& _tmp8F1,((_tmp8F2[1]=& _tmp8F0,Cyc_Tcutil_terr(_tmp3E9->loc,((_tmp8F3="expecting index designator %d but found %d",_tag_dyneither(_tmp8F3,sizeof(char),43))),_tag_dyneither(_tmp8F2,sizeof(void*),2)))))))))))));}}
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
const char*_tmp8F7;void*_tmp8F6[1];struct Cyc_String_pa_PrintArg_struct _tmp8F5;(_tmp8F5.tag=0,((_tmp8F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8F6[0]=& _tmp8F5,Cyc_Tcutil_terr(bound->loc,((_tmp8F7="expecting unsigned int, found %s",_tag_dyneither(_tmp8F7,sizeof(char),33))),_tag_dyneither(_tmp8F6,sizeof(void*),1)))))));}_LL1D0:;}
# 1874
if(!(vd->tq).real_const){
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8FA="comprehension index variable is not declared const!",_tag_dyneither(_tmp8FA,sizeof(char),52))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}{
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
{void**_tmp8FB;_tmp401=((_tmp8FB=_region_malloc(r,sizeof(*_tmp8FB)),((_tmp8FB[0]=_tmp406.elt_typ,_tmp8FB))));}
{struct Cyc_Absyn_Tqual*_tmp8FC;_tmp402=((_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=_tmp406.elt_tq,_tmp8FC))));}
{union Cyc_Absyn_Constraint**_tmp8FD;_tmp403=((_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD[0]=(_tmp406.ptr_atts).zero_term,_tmp8FD))));}
goto _LL1D5;_LL1D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp404;if(_tmp407->tag != 8)goto _LL1DA;else{_tmp408=(_tmp407->f1).elt_type;_tmp409=(_tmp407->f1).tq;_tmp40A=(_tmp407->f1).num_elts;_tmp40B=(_tmp407->f1).zero_term;}}_LL1D9:
# 1896
{void**_tmp8FE;_tmp401=((_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE[0]=_tmp408,_tmp8FE))));}
{struct Cyc_Absyn_Tqual*_tmp8FF;_tmp402=((_tmp8FF=_region_malloc(r,sizeof(*_tmp8FF)),((_tmp8FF[0]=_tmp409,_tmp8FF))));}
{union Cyc_Absyn_Constraint**_tmp900;_tmp403=((_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900[0]=_tmp40B,_tmp900))));}
goto _LL1D5;_LL1DA:;_LL1DB:
# 1901
 goto _LL1D5;_LL1D5:;}{
# 1904
void*t=Cyc_Tcexp_tcExp(_tmp400,_tmp401,body);
if(_tmp400->le == 0){
# 1907
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_Tcutil_terr(bound->loc,((_tmp903="bound is not constant",_tag_dyneither(_tmp903,sizeof(char),22))),_tag_dyneither(_tmp902,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp906;void*_tmp905;(_tmp905=0,Cyc_Tcutil_terr(bound->loc,((_tmp906="body is not constant",_tag_dyneither(_tmp906,sizeof(char),21))),_tag_dyneither(_tmp905,sizeof(void*),0)));}}
# 1912
if(_tmp403 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp403)){
# 1915
struct Cyc_Absyn_Exp*_tmp416=Cyc_Absyn_uint_exp(1,0);_tmp416->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp416,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1919
struct _RegionHandle _tmp417=_new_region("r");struct _RegionHandle*r=& _tmp417;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp909;void*_tmp908;(_tmp908=0,Cyc_Tcutil_terr(body->loc,((_tmp909="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp909,sizeof(char),49))),_tag_dyneither(_tmp908,sizeof(void*),0)));}{
# 1925
void*_tmp41A=Cyc_Absyn_array_typ(t,_tmp402 == 0?Cyc_Absyn_empty_tqual(0):*_tmp402,bound,
_tmp403 == 0?Cyc_Absyn_false_conref:*_tmp403,0);
void*_tmp41B=_tmp41A;_npop_handler(1);return _tmp41B;};
# 1919
;_pop_region(r);};};};}
# 1878
;_pop_region(r);};}
# 1932
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1935
Cyc_Tcexp_tcExp(te,0,bound);
{void*_stmttmp3E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp41C=_stmttmp3E;void*_tmp41E;_LL1DD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp41C;if(_tmp41D->tag != 19)goto _LL1DF;else{_tmp41E=(void*)_tmp41D->f1;}}_LL1DE:
# 1940
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp41E,0),0,Cyc_Absyn_No_coercion,0);
_tmp41F->topt=bound->topt;
# 1945
bound=_tmp41F;}
# 1947
goto _LL1DC;_LL1DF:;_LL1E0:
# 1949
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp90D;void*_tmp90C[1];struct Cyc_String_pa_PrintArg_struct _tmp90B;(_tmp90B.tag=0,((_tmp90B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp90C[0]=& _tmp90B,Cyc_Tcutil_terr(bound->loc,((_tmp90D="expecting unsigned int, found %s",_tag_dyneither(_tmp90D,sizeof(char),33))),_tag_dyneither(_tmp90C,sizeof(void*),1)))))));}_LL1DC:;}{
# 1954
struct _RegionHandle _tmp423=_new_region("r");struct _RegionHandle*r=& _tmp423;_push_region(r);
{void**_tmp424=0;
struct Cyc_Absyn_Tqual*_tmp425=0;
union Cyc_Absyn_Constraint**_tmp426=0;
# 1959
if(topt != 0){
void*_stmttmp3F=Cyc_Tcutil_compress(*topt);void*_tmp427=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp429;void*_tmp42B;struct Cyc_Absyn_Tqual _tmp42C;struct Cyc_Absyn_Exp*_tmp42D;union Cyc_Absyn_Constraint*_tmp42E;_LL1E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp428=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp427;if(_tmp428->tag != 5)goto _LL1E4;else{_tmp429=_tmp428->f1;}}_LL1E3:
# 1962
{void**_tmp90E;_tmp424=((_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=_tmp429.elt_typ,_tmp90E))));}
{struct Cyc_Absyn_Tqual*_tmp90F;_tmp425=((_tmp90F=_region_malloc(r,sizeof(*_tmp90F)),((_tmp90F[0]=_tmp429.elt_tq,_tmp90F))));}
{union Cyc_Absyn_Constraint**_tmp910;_tmp426=((_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910[0]=(_tmp429.ptr_atts).zero_term,_tmp910))));}
goto _LL1E1;_LL1E4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp427;if(_tmp42A->tag != 8)goto _LL1E6;else{_tmp42B=(_tmp42A->f1).elt_type;_tmp42C=(_tmp42A->f1).tq;_tmp42D=(_tmp42A->f1).num_elts;_tmp42E=(_tmp42A->f1).zero_term;}}_LL1E5:
# 1967
{void**_tmp911;_tmp424=((_tmp911=_region_malloc(r,sizeof(*_tmp911)),((_tmp911[0]=_tmp42B,_tmp911))));}
{struct Cyc_Absyn_Tqual*_tmp912;_tmp425=((_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912[0]=_tmp42C,_tmp912))));}
{union Cyc_Absyn_Constraint**_tmp913;_tmp426=((_tmp913=_region_malloc(r,sizeof(*_tmp913)),((_tmp913[0]=_tmp42E,_tmp913))));}
goto _LL1E1;_LL1E6:;_LL1E7:
# 1972
 goto _LL1E1;_LL1E1:;}
# 1975
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp424 != 0)Cyc_Tcutil_unify(*_tmp424,t);
# 1978
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp916;void*_tmp915;(_tmp915=0,Cyc_Tcutil_terr(bound->loc,((_tmp916="bound is not constant",_tag_dyneither(_tmp916,sizeof(char),22))),_tag_dyneither(_tmp915,sizeof(void*),0)));}}
# 1982
if(_tmp426 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp426)){
# 1985
struct Cyc_Absyn_Exp*_tmp437=Cyc_Absyn_uint_exp(1,0);_tmp437->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp437,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 1989
const char*_tmp919;void*_tmp918;(_tmp918=0,Cyc_Tcutil_terr(loc,((_tmp919="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp919,sizeof(char),74))),_tag_dyneither(_tmp918,sizeof(void*),0)));};}{
# 1992
void*_tmp43A=Cyc_Absyn_array_typ(t,_tmp425 == 0?Cyc_Absyn_empty_tqual(0):*_tmp425,bound,
_tmp426 == 0?Cyc_Absyn_false_conref:*_tmp426,0);
void*_tmp43B=_tmp43A;_npop_handler(0);return _tmp43B;};}
# 1955
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2008 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2012
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp91A;adptr=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=ad,_tmp91A))));};}else{
# 2018
{struct _handler_cons _tmp43D;_push_handler(& _tmp43D);{int _tmp43F=0;if(setjmp(_tmp43D.handler))_tmp43F=1;if(!_tmp43F){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2018
;_pop_handler();}else{void*_tmp43E=(void*)_exn_thrown;void*_tmp441=_tmp43E;void*_tmp443;_LL1E9: {struct Cyc_Dict_Absent_exn_struct*_tmp442=(struct Cyc_Dict_Absent_exn_struct*)_tmp441;if(_tmp442->tag != Cyc_Dict_Absent)goto _LL1EB;}_LL1EA:
# 2021
{const char*_tmp91E;void*_tmp91D[1];struct Cyc_String_pa_PrintArg_struct _tmp91C;(_tmp91C.tag=0,((_tmp91C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp91D[0]=& _tmp91C,Cyc_Tcutil_terr(loc,((_tmp91E="unbound struct/union name %s",_tag_dyneither(_tmp91E,sizeof(char),29))),_tag_dyneither(_tmp91D,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1EB: _tmp443=_tmp441;_LL1EC:(void)_throw(_tmp443);_LL1E8:;}};}
# 2024
*ad_opt=ad;
*tn=ad->name;}
# 2027
if(ad->impl == 0){
{const char*_tmp926;void*_tmp925[1];const char*_tmp924;const char*_tmp923;struct Cyc_String_pa_PrintArg_struct _tmp922;(_tmp922.tag=0,((_tmp922.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp923="struct",_tag_dyneither(_tmp923,sizeof(char),7)):((_tmp924="union",_tag_dyneither(_tmp924,sizeof(char),6)))),((_tmp925[0]=& _tmp922,Cyc_Tcutil_terr(loc,((_tmp926="can't construct abstract %s",_tag_dyneither(_tmp926,sizeof(char),28))),_tag_dyneither(_tmp925,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2031
struct _RegionHandle _tmp44C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp44C;_push_region(rgn);
# 2036
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp44D=Cyc_Tcenv_new_status(te);
if(_tmp44D == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2045
te2=Cyc_Tcenv_set_new_status(rgn,_tmp44D,te);}{
# 2047
struct _tuple12 _tmp927;struct _tuple12 _tmp44E=(_tmp927.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp927.f2=rgn,_tmp927)));
struct Cyc_List_List*_tmp44F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp44E,ad->tvs);
struct Cyc_List_List*_tmp450=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp44E,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp451=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp44F);
struct Cyc_List_List*_tmp452=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp450);
struct Cyc_List_List*_tmp453=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp44F,_tmp450);
void*res_typ;
# 2058
if(topt != 0){
void*_stmttmp40=Cyc_Tcutil_compress(*topt);void*_tmp454=_stmttmp40;struct Cyc_Absyn_Aggrdecl**_tmp456;struct Cyc_List_List*_tmp457;_LL1EE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp454;if(_tmp455->tag != 11)goto _LL1F0;else{if((((_tmp455->f1).aggr_info).KnownAggr).tag != 2)goto _LL1F0;_tmp456=(struct Cyc_Absyn_Aggrdecl**)(((_tmp455->f1).aggr_info).KnownAggr).val;_tmp457=(_tmp455->f1).targs;}}_LL1EF:
# 2061
 if(*_tmp456 == *adptr){
{struct Cyc_List_List*_tmp458=_tmp451;for(0;_tmp458 != 0  && _tmp457 != 0;
(_tmp458=_tmp458->tl,_tmp457=_tmp457->tl)){
Cyc_Tcutil_unify((void*)_tmp458->hd,(void*)_tmp457->hd);}}
# 2066
res_typ=*topt;
goto _LL1ED;}
# 2069
goto _LL1F1;_LL1F0:;_LL1F1: {
# 2071
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp92D;struct Cyc_Absyn_AggrInfo _tmp92C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp92B;res_typ=(void*)((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92D.tag=11,((_tmp92D.f1=((_tmp92C.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp92C.targs=_tmp451,_tmp92C)))),_tmp92D)))),_tmp92B))));}_LL1ED:;}else{
# 2074
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp933;struct Cyc_Absyn_AggrInfo _tmp932;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp931;res_typ=(void*)((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931[0]=((_tmp933.tag=11,((_tmp933.f1=((_tmp932.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp932.targs=_tmp451,_tmp932)))),_tmp933)))),_tmp931))));}{
# 2077
struct Cyc_List_List*_tmp45F=*ts;
struct Cyc_List_List*_tmp460=_tmp452;
while(_tmp45F != 0  && _tmp460 != 0){
# 2081
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp45F->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp45F->hd);
Cyc_Tcutil_unify((void*)_tmp45F->hd,(void*)_tmp460->hd);
_tmp45F=_tmp45F->tl;
_tmp460=_tmp460->tl;}
# 2087
if(_tmp45F != 0){
const char*_tmp936;void*_tmp935;(_tmp935=0,Cyc_Tcutil_terr(loc,((_tmp936="too many explicit witness types",_tag_dyneither(_tmp936,sizeof(char),32))),_tag_dyneither(_tmp935,sizeof(void*),0)));}
# 2090
*ts=_tmp452;{
# 2093
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp41=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp464;struct Cyc_Absyn_Exp*_tmp465;struct _tuple19*_tmp463=_stmttmp41;_tmp464=_tmp463->f1;_tmp465=_tmp463->f2;{
void*_tmp466=Cyc_Tcutil_rsubstitute(rgn,_tmp453,_tmp464->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp466,_tmp465);
# 2104
if(!Cyc_Tcutil_coerce_arg(te2,_tmp465,_tmp466,& bogus)){
{const char*_tmp942;void*_tmp941[5];struct Cyc_String_pa_PrintArg_struct _tmp940;const char*_tmp93F;const char*_tmp93E;struct Cyc_String_pa_PrintArg_struct _tmp93D;struct Cyc_String_pa_PrintArg_struct _tmp93C;struct Cyc_String_pa_PrintArg_struct _tmp93B;struct Cyc_String_pa_PrintArg_struct _tmp93A;(_tmp93A.tag=0,((_tmp93A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2108
Cyc_Absynpp_typ2string((void*)_check_null(_tmp465->topt))),((_tmp93B.tag=0,((_tmp93B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2107
Cyc_Absynpp_typ2string(_tmp466)),((_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp93D.tag=0,((_tmp93D.f1=(struct _dyneither_ptr)(
# 2106
ad->kind == Cyc_Absyn_StructA?(_tmp93E="struct",_tag_dyneither(_tmp93E,sizeof(char),7)):((_tmp93F="union",_tag_dyneither(_tmp93F,sizeof(char),6)))),((_tmp940.tag=0,((_tmp940.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp464->name),((_tmp941[0]=& _tmp940,((_tmp941[1]=& _tmp93D,((_tmp941[2]=& _tmp93C,((_tmp941[3]=& _tmp93B,((_tmp941[4]=& _tmp93A,Cyc_Tcutil_terr(_tmp465->loc,((_tmp942="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp942,sizeof(char),40))),_tag_dyneither(_tmp941,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2109
Cyc_Tcutil_explain_failure();}};}{
# 2113
struct Cyc_List_List*_tmp470=0;
{struct Cyc_List_List*_tmp471=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp471 != 0;_tmp471=_tmp471->tl){
struct _tuple0*_tmp945;struct Cyc_List_List*_tmp944;_tmp470=((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944->hd=((_tmp945=_cycalloc(sizeof(*_tmp945)),((_tmp945->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp453,(*((struct _tuple0*)_tmp471->hd)).f1),((_tmp945->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp453,(*((struct _tuple0*)_tmp471->hd)).f2),_tmp945)))))),((_tmp944->tl=_tmp470,_tmp944))))));}}
# 2118
_tmp470=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp470);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp470);{
void*_tmp474=res_typ;_npop_handler(0);return _tmp474;};};};};};}
# 2036
;_pop_region(rgn);};}
# 2124
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2126
struct _RegionHandle _tmp476=_new_region("rgn");struct _RegionHandle*rgn=& _tmp476;_push_region(rgn);{
void*_stmttmp42=Cyc_Tcutil_compress(ts);void*_tmp477=_stmttmp42;enum Cyc_Absyn_AggrKind _tmp479;struct Cyc_List_List*_tmp47A;_LL1F3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp477;if(_tmp478->tag != 12)goto _LL1F5;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;}}_LL1F4:
# 2129
 if(_tmp479 == Cyc_Absyn_UnionA){
const char*_tmp948;void*_tmp947;(_tmp947=0,Cyc_Tcutil_terr(loc,((_tmp948="expecting struct but found union",_tag_dyneither(_tmp948,sizeof(char),33))),_tag_dyneither(_tmp947,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp47A);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp43=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct _tuple19*_tmp47D=_stmttmp43;_tmp47E=_tmp47D->f1;_tmp47F=_tmp47D->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp47E->type,_tmp47F);
# 2138
if(!Cyc_Tcutil_coerce_arg(te,_tmp47F,_tmp47E->type,& bogus)){
{const char*_tmp94E;void*_tmp94D[3];struct Cyc_String_pa_PrintArg_struct _tmp94C;struct Cyc_String_pa_PrintArg_struct _tmp94B;struct Cyc_String_pa_PrintArg_struct _tmp94A;(_tmp94A.tag=0,((_tmp94A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2141
Cyc_Absynpp_typ2string((void*)_check_null(_tmp47F->topt))),((_tmp94B.tag=0,((_tmp94B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2140
Cyc_Absynpp_typ2string(_tmp47E->type)),((_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47E->name),((_tmp94D[0]=& _tmp94C,((_tmp94D[1]=& _tmp94B,((_tmp94D[2]=& _tmp94A,Cyc_Tcutil_terr(_tmp47F->loc,((_tmp94E="field %s of struct expects type %s != %s",_tag_dyneither(_tmp94E,sizeof(char),41))),_tag_dyneither(_tmp94D,sizeof(void*),3)))))))))))))))))));}
# 2142
Cyc_Tcutil_explain_failure();}}
# 2145
goto _LL1F2;};_LL1F5:;_LL1F6: {
const char*_tmp951;void*_tmp950;(_tmp950=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp951="tcAnonStruct: wrong type",_tag_dyneither(_tmp951,sizeof(char),25))),_tag_dyneither(_tmp950,sizeof(void*),0)));}_LL1F2:;}{
# 2149
void*_tmp487=ts;_npop_handler(0);return _tmp487;};
# 2126
;_pop_region(rgn);}
# 2153
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2156
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);
{struct _tuple12 _tmp952;struct _tuple12 _tmp489=(_tmp952.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp952.f2=rgn,_tmp952)));
struct Cyc_List_List*_tmp48A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp489,tud->tvs);
struct Cyc_List_List*_tmp48B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp48A);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp958;struct Cyc_Absyn_DatatypeFieldInfo _tmp957;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp956;void*res=(void*)(
(_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956[0]=((_tmp958.tag=4,((_tmp958.f1=((_tmp957.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp957.targs=_tmp48B,_tmp957)))),_tmp958)))),_tmp956))));
# 2164
if(topt != 0){
void*_stmttmp44=Cyc_Tcutil_compress(*topt);void*_tmp48C=_stmttmp44;_LL1F8: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp48D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp48C;if(_tmp48D->tag != 4)goto _LL1FA;}_LL1F9:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F7;_LL1FA:;_LL1FB:
 goto _LL1F7;_LL1F7:;}{
# 2170
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp48A != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp48A,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp960;void*_tmp95F[3];struct Cyc_String_pa_PrintArg_struct _tmp95E;struct Cyc_String_pa_PrintArg_struct _tmp95D;const char*_tmp95C;struct Cyc_String_pa_PrintArg_struct _tmp95B;(_tmp95B.tag=0,((_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2181
e->topt == 0?(struct _dyneither_ptr)((_tmp95C="?",_tag_dyneither(_tmp95C,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp95D.tag=0,((_tmp95D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2180
Cyc_Absynpp_typ2string(t)),((_tmp95E.tag=0,((_tmp95E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp95F[0]=& _tmp95E,((_tmp95F[1]=& _tmp95D,((_tmp95F[2]=& _tmp95B,Cyc_Tcutil_terr(e->loc,((_tmp960="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp960,sizeof(char),82))),_tag_dyneither(_tmp95F,sizeof(void*),3)))))))))))))))))));}
# 2182
Cyc_Tcutil_explain_failure();}}
# 2185
if(es != 0){
const char*_tmp964;void*_tmp963[1];struct Cyc_String_pa_PrintArg_struct _tmp962;void*_tmp497=(_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2188
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp963[0]=& _tmp962,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp964="too many arguments for datatype constructor %s",_tag_dyneither(_tmp964,sizeof(char),47))),_tag_dyneither(_tmp963,sizeof(void*),1)))))));_npop_handler(0);return _tmp497;}
if(ts != 0){
const char*_tmp968;void*_tmp967[1];struct Cyc_String_pa_PrintArg_struct _tmp966;void*_tmp49B=(_tmp966.tag=0,((_tmp966.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp967[0]=& _tmp966,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp968="too few arguments for datatype constructor %s",_tag_dyneither(_tmp968,sizeof(char),46))),_tag_dyneither(_tmp967,sizeof(void*),1)))))));_npop_handler(0);return _tmp49B;}{
void*_tmp49C=res;_npop_handler(0);return _tmp49C;};};}
# 2157
;_pop_region(rgn);}
# 2196
static int Cyc_Tcexp_zeroable_type(void*t){
void*_stmttmp45=Cyc_Tcutil_compress(t);void*_tmp4A1=_stmttmp45;union Cyc_Absyn_Constraint*_tmp4A8;void*_tmp4AC;struct Cyc_List_List*_tmp4AF;union Cyc_Absyn_AggrInfoU _tmp4B1;struct Cyc_List_List*_tmp4B2;struct Cyc_List_List*_tmp4B6;_LL1FD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4A2=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A2->tag != 0)goto _LL1FF;}_LL1FE:
 return 1;_LL1FF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4A1;if(_tmp4A3->tag != 1)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A4->tag != 2)goto _LL203;}_LL202:
 goto _LL204;_LL203: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4A5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A5->tag != 3)goto _LL205;}_LL204:
 goto _LL206;_LL205: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A6->tag != 4)goto _LL207;}_LL206:
 return 0;_LL207: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A7->tag != 5)goto _LL209;else{_tmp4A8=((_tmp4A7->f1).ptr_atts).nullable;}}_LL208:
# 2204
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4A8);_LL209: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4A1;if(_tmp4A9->tag != 6)goto _LL20B;}_LL20A:
 goto _LL20C;_LL20B: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4AA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4A1;if(_tmp4AA->tag != 7)goto _LL20D;}_LL20C:
 return 1;_LL20D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4AB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A1;if(_tmp4AB->tag != 8)goto _LL20F;else{_tmp4AC=(_tmp4AB->f1).elt_type;}}_LL20E:
 return Cyc_Tcexp_zeroable_type(_tmp4AC);_LL20F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4AD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A1;if(_tmp4AD->tag != 9)goto _LL211;}_LL210:
 return 0;_LL211: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4A1;if(_tmp4AE->tag != 10)goto _LL213;else{_tmp4AF=_tmp4AE->f1;}}_LL212:
# 2210
 for(0;(unsigned int)_tmp4AF;_tmp4AF=_tmp4AF->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp4AF->hd)).f2))return 0;}
return 1;_LL213: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B0->tag != 11)goto _LL215;else{_tmp4B1=(_tmp4B0->f1).aggr_info;_tmp4B2=(_tmp4B0->f1).targs;}}_LL214: {
# 2214
struct Cyc_Absyn_Aggrdecl*_tmp4C3=Cyc_Absyn_get_known_aggrdecl(_tmp4B1);
if(_tmp4C3->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C3->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp4C4=_new_region("r");struct _RegionHandle*r=& _tmp4C4;_push_region(r);
{struct Cyc_List_List*_tmp4C5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4C3->tvs,_tmp4B2);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C3->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4C5,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4C6=0;_npop_handler(0);return _tmp4C6;}}}{
int _tmp4C7=1;_npop_handler(0);return _tmp4C7;};}
# 2218
;_pop_region(r);};}_LL215: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B3->tag != 13)goto _LL217;}_LL216:
# 2223
 return 1;_LL217: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B4->tag != 19)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B5->tag != 12)goto _LL21B;else{_tmp4B6=_tmp4B5->f2;}}_LL21A:
# 2226
 for(0;_tmp4B6 != 0;_tmp4B6=_tmp4B6->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4B6->hd)->type))return 0;}
return 1;_LL21B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B7->tag != 14)goto _LL21D;}_LL21C:
 return 1;_LL21D: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B8->tag != 17)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4A1;if(_tmp4B9->tag != 16)goto _LL221;}_LL220:
 return 0;_LL221: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4A1;if(_tmp4BA->tag != 15)goto _LL223;}_LL222:
 return 0;_LL223: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4BB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4A1;if(_tmp4BB->tag != 26)goto _LL225;}_LL224:
# 2234
 goto _LL226;_LL225: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4BC=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4A1;if(_tmp4BC->tag != 18)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4A1;if(_tmp4BD->tag != 20)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4A1;if(_tmp4BE->tag != 21)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4A1;if(_tmp4BF->tag != 22)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4A1;if(_tmp4C0->tag != 23)goto _LL22F;}_LL22E:
 goto _LL230;_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4A1;if(_tmp4C1->tag != 24)goto _LL231;}_LL230:
 goto _LL232;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4A1;if(_tmp4C2->tag != 25)goto _LL1FC;}_LL232: {
const char*_tmp96C;void*_tmp96B[1];struct Cyc_String_pa_PrintArg_struct _tmp96A;(_tmp96A.tag=0,((_tmp96A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp96B[0]=& _tmp96A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp96C="bad type `%s' in zeroable type",_tag_dyneither(_tmp96C,sizeof(char),31))),_tag_dyneither(_tmp96B,sizeof(void*),1)))))));}_LL1FC:;}
# 2245
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2247
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2249
if(topt != 0){
void*_stmttmp46=Cyc_Tcutil_compress(*topt);void*_tmp4CB=_stmttmp46;void*_tmp4CD;_LL234: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CB;if(_tmp4CC->tag != 5)goto _LL236;else{_tmp4CD=(_tmp4CC->f1).elt_typ;}}_LL235:
# 2252
 Cyc_Tcutil_unify(_tmp4CD,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL233;_LL236:;_LL237:
 goto _LL233;_LL233:;}{
# 2258
const char*_tmp975;void*_tmp974[2];const char*_tmp973;const char*_tmp972;struct Cyc_String_pa_PrintArg_struct _tmp971;struct Cyc_String_pa_PrintArg_struct _tmp970;(_tmp970.tag=0,((_tmp970.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp971.tag=0,((_tmp971.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp972="calloc",_tag_dyneither(_tmp972,sizeof(char),7))):(struct _dyneither_ptr)((_tmp973="malloc",_tag_dyneither(_tmp973,sizeof(char),7))))),((_tmp974[0]=& _tmp971,((_tmp974[1]=& _tmp970,Cyc_Tcutil_terr(loc,((_tmp975="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp975,sizeof(char),60))),_tag_dyneither(_tmp974,sizeof(void*),2)))))))))))));};}
# 2262
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2264
enum Cyc_Absyn_AliasQual _stmttmp47=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_stmttmp47){case Cyc_Absyn_Unique: _LL238:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL239:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2270
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2275
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2278
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp978;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp977;void*expected_type=(void*)(
(_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp978.tag=15,((_tmp978.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp978)))),_tmp977))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_stmttmp48=Cyc_Tcutil_compress(handle_type);void*_tmp4D4=_stmttmp48;void*_tmp4D6;_LL23C: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4D5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4D4;if(_tmp4D5->tag != 15)goto _LL23E;else{_tmp4D6=(void*)_tmp4D5->f1;}}_LL23D:
# 2283
 rgn=_tmp4D6;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL23B;_LL23E:;_LL23F:
# 2287
{const char*_tmp97C;void*_tmp97B[1];struct Cyc_String_pa_PrintArg_struct _tmp97A;(_tmp97A.tag=0,((_tmp97A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp97B[0]=& _tmp97A,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp97C="expecting region_t type but found %s",_tag_dyneither(_tmp97C,sizeof(char),37))),_tag_dyneither(_tmp97B,sizeof(void*),1)))))));}
goto _LL23B;_LL23B:;}else{
# 2294
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2301
{struct _RegionHandle _tmp4DC=_new_region("r");struct _RegionHandle*r=& _tmp4DC;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2312 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp97F;void*_tmp97E;(_tmp97E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp97F="calloc with empty type",_tag_dyneither(_tmp97F,sizeof(char),23))),_tag_dyneither(_tmp97E,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2324
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4DF=er;void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;_LL241: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4DF;if(_tmp4E0->tag != 16)goto _LL243;else{_tmp4E1=(void*)_tmp4E0->f1;}}_LL242:
# 2328
 elt_type=_tmp4E1;
{void**_tmp980;*t=((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=elt_type,_tmp980))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL240;_LL243: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4E2=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4DF;if(_tmp4E2->tag != 2)goto _LL245;else{if(_tmp4E2->f1 != Cyc_Absyn_Times)goto _LL245;if(_tmp4E2->f2 == 0)goto _LL245;_tmp4E3=(struct Cyc_Absyn_Exp*)(_tmp4E2->f2)->hd;if((_tmp4E2->f2)->tl == 0)goto _LL245;_tmp4E4=(struct Cyc_Absyn_Exp*)((_tmp4E2->f2)->tl)->hd;if(((_tmp4E2->f2)->tl)->tl != 0)goto _LL245;}}_LL244:
# 2335
{struct _tuple0 _tmp981;struct _tuple0 _stmttmp4C=(_tmp981.f1=_tmp4E3->r,((_tmp981.f2=_tmp4E4->r,_tmp981)));struct _tuple0 _tmp4E6=_stmttmp4C;void*_tmp4E8;void*_tmp4EA;_LL248: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E6.f1;if(_tmp4E7->tag != 16)goto _LL24A;else{_tmp4E8=(void*)_tmp4E7->f1;}}_LL249:
# 2337
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4E8);
elt_type=_tmp4E8;
{void**_tmp982;*t=((_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982[0]=elt_type,_tmp982))));}
num_elts=_tmp4E4;
one_elt=0;
goto _LL247;_LL24A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4E9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E6.f2;if(_tmp4E9->tag != 16)goto _LL24C;else{_tmp4EA=(void*)_tmp4E9->f1;}}_LL24B:
# 2344
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4EA);
elt_type=_tmp4EA;
{void**_tmp983;*t=((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=elt_type,_tmp983))));}
num_elts=_tmp4E3;
one_elt=0;
goto _LL247;_LL24C:;_LL24D:
 goto No_sizeof;_LL247:;}
# 2352
goto _LL240;_LL245:;_LL246:
# 2354
 No_sizeof: {
# 2357
struct Cyc_Absyn_Exp*_tmp4EE=*e;
{void*_stmttmp49=_tmp4EE->r;void*_tmp4EF=_stmttmp49;struct Cyc_Absyn_Exp*_tmp4F1;_LL24F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4EF;if(_tmp4F0->tag != 13)goto _LL251;else{_tmp4F1=_tmp4F0->f2;}}_LL250:
 _tmp4EE=_tmp4F1;goto _LL24E;_LL251:;_LL252:
 goto _LL24E;_LL24E:;}
# 2362
{void*_stmttmp4A=Cyc_Tcutil_compress((void*)_check_null(_tmp4EE->topt));void*_tmp4F2=_stmttmp4A;void*_tmp4F4;_LL254: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F2;if(_tmp4F3->tag != 19)goto _LL256;else{_tmp4F4=(void*)_tmp4F3->f1;}}_LL255:
# 2364
{void*_stmttmp4B=Cyc_Tcutil_compress(_tmp4F4);void*_tmp4F5=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp4F7;_LL259: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F5;if(_tmp4F6->tag != 18)goto _LL25B;else{_tmp4F7=_tmp4F6->f1;}}_LL25A:
# 2366
 er=_tmp4F7->r;goto retry_sizeof;_LL25B:;_LL25C:
 goto _LL258;_LL258:;}
# 2369
goto _LL253;_LL256:;_LL257:
 goto _LL253;_LL253:;}
# 2372
elt_type=Cyc_Absyn_char_typ;
{void**_tmp984;*t=((_tmp984=_cycalloc(sizeof(*_tmp984)),((_tmp984[0]=elt_type,_tmp984))));}
num_elts=*e;
one_elt=0;}
# 2377
goto _LL240;_LL240:;}}
# 2381
*is_fat=!one_elt;
# 2384
{void*_tmp4F9=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4FB;_LL25E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FA->tag != 11)goto _LL260;else{if((((_tmp4FA->f1).aggr_info).KnownAggr).tag != 2)goto _LL260;_tmp4FB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4FA->f1).aggr_info).KnownAggr).val);}}_LL25F:
# 2386
 if(_tmp4FB->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4FB->impl))->exist_vars != 0){
const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_Tcutil_terr(loc,((_tmp987="malloc with existential types not yet implemented",_tag_dyneither(_tmp987,sizeof(char),50))),_tag_dyneither(_tmp986,sizeof(void*),0)));}
goto _LL25D;_LL260:;_LL261:
 goto _LL25D;_LL25D:;}{
# 2393
void*(*_tmp4FE)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4FF=Cyc_Absyn_false_conref;
if(topt != 0){
void*_stmttmp4D=Cyc_Tcutil_compress(*topt);void*_tmp500=_stmttmp4D;union Cyc_Absyn_Constraint*_tmp502;union Cyc_Absyn_Constraint*_tmp503;union Cyc_Absyn_Constraint*_tmp504;_LL263: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp500;if(_tmp501->tag != 5)goto _LL265;else{_tmp502=((_tmp501->f1).ptr_atts).nullable;_tmp503=((_tmp501->f1).ptr_atts).bounds;_tmp504=((_tmp501->f1).ptr_atts).zero_term;}}_LL264:
# 2398
 _tmp4FF=_tmp504;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp502))
_tmp4FE=Cyc_Absyn_star_typ;
# 2403
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp504) && !(*is_calloc)){
{const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_Tcutil_warn(loc,((_tmp98A="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp98A,sizeof(char),55))),_tag_dyneither(_tmp989,sizeof(void*),0)));}
*is_calloc=1;}
# 2409
{void*_stmttmp4E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp503);void*_tmp507=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp50A;_LL268: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp508=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp507;if(_tmp508->tag != 0)goto _LL26A;}_LL269:
 goto _LL267;_LL26A:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp509=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp507;if(_tmp509->tag != 1)goto _LL26C;else{_tmp50A=_tmp509->f1;}}if(!(!one_elt))goto _LL26C;_LL26B: {
# 2412
int _tmp50B=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp50B  && Cyc_Evexp_same_const_exp(_tmp50A,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp503),_tmp4FF);}
# 2418
{void*_stmttmp4F=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp50C=_stmttmp4F;void*_tmp50E;_LL26F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp50C;if(_tmp50D->tag != 19)goto _LL271;else{_tmp50E=(void*)_tmp50D->f1;}}_LL270: {
# 2420
struct Cyc_Absyn_Exp*_tmp50F=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp50E,0),0,Cyc_Absyn_No_coercion,0);
# 2422
if(Cyc_Evexp_same_const_exp(_tmp50F,_tmp50A)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp503),_tmp4FF);}
# 2427
goto _LL26E;}_LL271:;_LL272:
 goto _LL26E;_LL26E:;}
# 2430
goto _LL267;}_LL26C:;_LL26D:
 goto _LL267;_LL267:;}
# 2433
goto _LL262;_LL265:;_LL266:
 goto _LL262;_LL262:;}
# 2436
if(!one_elt)_tmp4FE=Cyc_Absyn_dyneither_typ;
return _tmp4FE(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4FF);};};}
# 2441
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2447
struct _RegionHandle _tmp510=_new_region("r");struct _RegionHandle*r=& _tmp510;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp511=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp511,0,e1);{
void*_tmp512=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp511,& _tmp512,e2);};}{
# 2453
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2456
{void*_stmttmp50=Cyc_Tcutil_compress(t1);void*_tmp513=_stmttmp50;_LL274: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp513;if(_tmp514->tag != 8)goto _LL276;}_LL275:
{const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_Tcutil_terr(loc,((_tmp98D="cannot assign to an array",_tag_dyneither(_tmp98D,sizeof(char),26))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}goto _LL273;_LL276:;_LL277:
 goto _LL273;_LL273:;}{
# 2461
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp990;void*_tmp98F;(_tmp98F=0,Cyc_Tcutil_terr(loc,((_tmp990="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp990,sizeof(char),58))),_tag_dyneither(_tmp98F,sizeof(void*),0)));}
# 2466
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp993;void*_tmp992;void*_tmp51B=(_tmp992=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp993="swap non-lvalue",_tag_dyneither(_tmp993,sizeof(char),16))),_tag_dyneither(_tmp992,sizeof(void*),0)));_npop_handler(0);return _tmp51B;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp996;void*_tmp995;void*_tmp51E=(_tmp995=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp996="swap non-lvalue",_tag_dyneither(_tmp996,sizeof(char),16))),_tag_dyneither(_tmp995,sizeof(void*),0)));_npop_handler(0);return _tmp51E;}{
# 2471
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp999;void*_tmp998;void*_tmp521=(_tmp998=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp999="swap value in zeroterm array",_tag_dyneither(_tmp999,sizeof(char),29))),_tag_dyneither(_tmp998,sizeof(void*),0)));_npop_handler(0);return _tmp521;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp99C;void*_tmp99B;void*_tmp524=(_tmp99B=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp99C="swap value in zeroterm array",_tag_dyneither(_tmp99C,sizeof(char),29))),_tag_dyneither(_tmp99B,sizeof(void*),0)));_npop_handler(0);return _tmp524;}
# 2478
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9A1;void*_tmp9A0[2];struct Cyc_String_pa_PrintArg_struct _tmp99F;struct Cyc_String_pa_PrintArg_struct _tmp99E;void*_tmp525=(_tmp99E.tag=0,((_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp99F.tag=0,((_tmp99F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9A0[0]=& _tmp99F,((_tmp9A0[1]=& _tmp99E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9A1="type mismatch: %s != %s",_tag_dyneither(_tmp9A1,sizeof(char),24))),_tag_dyneither(_tmp9A0,sizeof(void*),2)))))))))))));
void*_tmp526=_tmp525;_npop_handler(0);return _tmp526;}{
# 2485
void*_tmp52B=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp52B;};};};};
# 2447
;_pop_region(r);}
# 2489
int Cyc_Tcexp_in_stmt_exp=0;
# 2491
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp52C=_new_region("r");struct _RegionHandle*r=& _tmp52C;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2498
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2500
while(1){
void*_stmttmp51=s->r;void*_tmp52D=_stmttmp51;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Stmt*_tmp531;struct Cyc_Absyn_Stmt*_tmp532;struct Cyc_Absyn_Decl*_tmp534;struct Cyc_Absyn_Stmt*_tmp535;_LL279: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp52E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp52D;if(_tmp52E->tag != 1)goto _LL27B;else{_tmp52F=_tmp52E->f1;}}_LL27A: {
# 2504
void*_tmp536=(void*)_check_null(_tmp52F->topt);
if(!Cyc_Tcutil_unify(_tmp536,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp9A5;void*_tmp9A4[1];struct Cyc_String_pa_PrintArg_struct _tmp9A3;(_tmp9A3.tag=0,((_tmp9A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp536)),((_tmp9A4[0]=& _tmp9A3,Cyc_Tcutil_terr(loc,((_tmp9A5="statement expression returns type %s",_tag_dyneither(_tmp9A5,sizeof(char),37))),_tag_dyneither(_tmp9A4,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2510
void*_tmp53A=_tmp536;_npop_handler(0);return _tmp53A;};}_LL27B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp530=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp52D;if(_tmp530->tag != 2)goto _LL27D;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL27C:
 s=_tmp532;continue;_LL27D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp533=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp52D;if(_tmp533->tag != 12)goto _LL27F;else{_tmp534=_tmp533->f1;_tmp535=_tmp533->f2;}}_LL27E:
 s=_tmp535;continue;_LL27F:;_LL280: {
# 2514
const char*_tmp9A8;void*_tmp9A7;void*_tmp53D=(_tmp9A7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9A8="statement expression must end with expression",_tag_dyneither(_tmp9A8,sizeof(char),46))),_tag_dyneither(_tmp9A7,sizeof(void*),0)));_npop_handler(0);return _tmp53D;}_LL278:;}
# 2492
;_pop_region(r);}
# 2519
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp53E=_new_region("r");struct _RegionHandle*r=& _tmp53E;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2524
{void*_tmp53F=t;struct Cyc_Absyn_Aggrdecl*_tmp541;_LL282: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag != 11)goto _LL284;else{if((((_tmp540->f1).aggr_info).KnownAggr).tag != 2)goto _LL284;_tmp541=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp540->f1).aggr_info).KnownAggr).val);}}_LL283:
# 2526
 if((_tmp541->kind == Cyc_Absyn_UnionA  && _tmp541->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp541->impl))->tagged)goto _LL281;
goto _LL285;_LL284:;_LL285:
# 2529
{const char*_tmp9AC;void*_tmp9AB[1];struct Cyc_String_pa_PrintArg_struct _tmp9AA;(_tmp9AA.tag=0,((_tmp9AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9AB[0]=& _tmp9AA,Cyc_Tcutil_terr(loc,((_tmp9AC="expecting @tagged union but found %s",_tag_dyneither(_tmp9AC,sizeof(char),37))),_tag_dyneither(_tmp9AB,sizeof(void*),1)))))));}
goto _LL281;_LL281:;}{
# 2532
void*_tmp545=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp545;};
# 2521
;_pop_region(r);}
# 2536
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2540
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp546=_new_region("r");struct _RegionHandle*r=& _tmp546;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp547=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2545
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9AF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9AE;void*expected_type=(void*)(
(_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9AF.tag=15,((_tmp9AF.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp547)),_tmp9AF)))),_tmp9AE))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp547,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_stmttmp52=Cyc_Tcutil_compress(handle_type);void*_tmp548=_stmttmp52;void*_tmp54A;_LL287: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp548;if(_tmp549->tag != 15)goto _LL289;else{_tmp54A=(void*)_tmp549->f1;}}_LL288:
# 2550
 rgn=_tmp54A;
Cyc_Tcenv_check_rgn_accessible(_tmp547,loc,rgn);
goto _LL286;_LL289:;_LL28A:
# 2554
{const char*_tmp9B3;void*_tmp9B2[1];struct Cyc_String_pa_PrintArg_struct _tmp9B1;(_tmp9B1.tag=0,((_tmp9B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9B2[0]=& _tmp9B1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9B3="expecting region_t type but found %s",_tag_dyneither(_tmp9B3,sizeof(char),37))),_tag_dyneither(_tmp9B2,sizeof(void*),1)))))));}
goto _LL286;_LL286:;}else{
# 2561
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2569
void*_stmttmp53=e1->r;void*_tmp550=_stmttmp53;struct Cyc_Core_Opt*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_List_List*_tmp557;_LL28C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp551->tag != 26)goto _LL28E;}_LL28D:
 goto _LL28F;_LL28E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp552=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp552->tag != 27)goto _LL290;}_LL28F: {
# 2574
void*_tmp55A=Cyc_Tcexp_tcExpNoPromote(_tmp547,topt,e1);
void*_stmttmp57=Cyc_Tcutil_compress(_tmp55A);void*_tmp55B=_stmttmp57;void*_tmp55D;struct Cyc_Absyn_Tqual _tmp55E;struct Cyc_Absyn_Exp*_tmp55F;union Cyc_Absyn_Constraint*_tmp560;_LL29B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55B;if(_tmp55C->tag != 8)goto _LL29D;else{_tmp55D=(_tmp55C->f1).elt_type;_tmp55E=(_tmp55C->f1).tq;_tmp55F=(_tmp55C->f1).num_elts;_tmp560=(_tmp55C->f1).zero_term;}}_LL29C: {
# 2577
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp55F);
void*b;
{void*_stmttmp58=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp561=_stmttmp58;void*_tmp563;_LL2A0: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag != 19)goto _LL2A2;else{_tmp563=(void*)_tmp562->f1;}}_LL2A1:
# 2581
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B5;b=(void*)((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B6.tag=1,((_tmp9B6.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp563,0),0,Cyc_Absyn_No_coercion,0),_tmp9B6)))),_tmp9B5))));}
goto _LL29F;_LL2A2:;_LL2A3:
# 2584
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B8;b=(void*)((_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=1,((_tmp9B9.f1=bnd,_tmp9B9)))),_tmp9B8))));}else{
# 2587
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL29F:;}{
# 2589
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9BF;struct Cyc_Absyn_PtrInfo _tmp9BE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9BD;void*res_typ=(void*)(
(_tmp9BD=_cycalloc(sizeof(*_tmp9BD)),((_tmp9BD[0]=((_tmp9BF.tag=5,((_tmp9BF.f1=((_tmp9BE.elt_typ=_tmp55D,((_tmp9BE.elt_tq=_tmp55E,((_tmp9BE.ptr_atts=(
((_tmp9BE.ptr_atts).rgn=rgn,(((_tmp9BE.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9BE.ptr_atts).bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp9BE.ptr_atts).zero_term=_tmp560,(((_tmp9BE.ptr_atts).ptrloc=0,_tmp9BE.ptr_atts)))))))))),_tmp9BE)))))),_tmp9BF)))),_tmp9BD))));
# 2593
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp547,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp547,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2600
void*_tmp568=res_typ;_npop_handler(0);return _tmp568;};};}_LL29D:;_LL29E: {
const char*_tmp9C2;void*_tmp9C1;(_tmp9C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C2="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9C2,sizeof(char),45))),_tag_dyneither(_tmp9C1,sizeof(void*),0)));}_LL29A:;}_LL290: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp553->tag != 35)goto _LL292;else{_tmp554=_tmp553->f1;_tmp555=_tmp553->f2;}}_LL291:
# 2604
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9C5;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9C4;e1->r=(void*)((_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=25,((_tmp9C5.f1=_tmp555,_tmp9C5)))),_tmp9C4))));}
_tmp557=_tmp555;goto _LL293;_LL292: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp556=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp556->tag != 25)goto _LL294;else{_tmp557=_tmp556->f1;}}_LL293: {
# 2607
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp55=Cyc_Tcutil_compress(*topt);void*_tmp570=_stmttmp55;void**_tmp572;struct Cyc_Absyn_Tqual _tmp573;union Cyc_Absyn_Constraint*_tmp574;_LL2A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570;if(_tmp571->tag != 5)goto _LL2A7;else{_tmp572=(void**)&(_tmp571->f1).elt_typ;_tmp573=(_tmp571->f1).elt_tq;_tmp574=((_tmp571->f1).ptr_atts).zero_term;}}_LL2A6:
# 2613
 elt_typ_opt=_tmp572;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp574);
goto _LL2A4;_LL2A7:;_LL2A8:
 goto _LL2A4;_LL2A4:;}{
# 2619
void*_tmp575=Cyc_Tcexp_tcArray(_tmp547,e1->loc,elt_typ_opt,0,zero_term,_tmp557);
e1->topt=_tmp575;{
void*res_typ;
{void*_stmttmp56=Cyc_Tcutil_compress(_tmp575);void*_tmp576=_stmttmp56;void*_tmp578;struct Cyc_Absyn_Tqual _tmp579;struct Cyc_Absyn_Exp*_tmp57A;union Cyc_Absyn_Constraint*_tmp57B;_LL2AA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp576;if(_tmp577->tag != 8)goto _LL2AC;else{_tmp578=(_tmp577->f1).elt_type;_tmp579=(_tmp577->f1).tq;_tmp57A=(_tmp577->f1).num_elts;_tmp57B=(_tmp577->f1).zero_term;}}_LL2AB:
# 2624
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9D6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9D5;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D4;struct Cyc_Absyn_PtrInfo _tmp9D3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9D2;res_typ=(void*)(
(_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D6.tag=5,((_tmp9D6.f1=((_tmp9D3.elt_typ=_tmp578,((_tmp9D3.elt_tq=_tmp579,((_tmp9D3.ptr_atts=(
((_tmp9D3.ptr_atts).rgn=rgn,(((_tmp9D3.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9D3.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=1,((_tmp9D5.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp57A),_tmp9D5)))),_tmp9D4))))),(((_tmp9D3.ptr_atts).zero_term=_tmp57B,(((_tmp9D3.ptr_atts).ptrloc=0,_tmp9D3.ptr_atts)))))))))),_tmp9D3)))))),_tmp9D6)))),_tmp9D2))));}
# 2629
if(topt != 0){
# 2633
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp547,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp547,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2639
goto _LL2A9;_LL2AC:;_LL2AD: {
const char*_tmp9D9;void*_tmp9D8;(_tmp9D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9D9="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9D9,sizeof(char),50))),_tag_dyneither(_tmp9D8,sizeof(void*),0)));}_LL2A9:;}{
# 2642
void*_tmp583=res_typ;_npop_handler(0);return _tmp583;};};};}_LL294: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp558=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp558->tag != 0)goto _LL296;else{if(((_tmp558->f1).String_c).tag != 8)goto _LL296;}}_LL295: {
# 2647
void*_tmp584=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2649
void*_tmp585=Cyc_Tcexp_tcExp(_tmp547,& _tmp584,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9DB;void*_tmp588=Cyc_Absyn_atb_typ(_tmp585,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB[0]=((_tmp9DC.tag=1,((_tmp9DC.f1=Cyc_Absyn_uint_exp(1,0),_tmp9DC)))),_tmp9DB)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp588;}_LL296: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp559=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp550;if(_tmp559->tag != 0)goto _LL298;else{if(((_tmp559->f1).Wstring_c).tag != 9)goto _LL298;}}_LL297: {
# 2654
void*_tmp589=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2656
void*_tmp58A=Cyc_Tcexp_tcExp(_tmp547,& _tmp589,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9DE;void*_tmp58D=Cyc_Absyn_atb_typ(_tmp58A,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=((_tmp9DF.tag=1,((_tmp9DF.f1=Cyc_Absyn_uint_exp(1,0),_tmp9DF)))),_tmp9DE)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp58D;}_LL298:;_LL299:
# 2663
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_stmttmp54=Cyc_Tcutil_compress(*topt);void*_tmp58E=_stmttmp54;void**_tmp590;struct Cyc_Absyn_Tqual _tmp591;_LL2AF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E;if(_tmp58F->tag != 5)goto _LL2B1;else{_tmp590=(void**)&(_tmp58F->f1).elt_typ;_tmp591=(_tmp58F->f1).elt_tq;}}_LL2B0:
# 2669
 topt2=_tmp590;goto _LL2AE;_LL2B1: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp592=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp58E;if(_tmp592->tag != 3)goto _LL2B3;}_LL2B2:
# 2673
 bogus=*topt;
topt2=& bogus;
goto _LL2AE;_LL2B3:;_LL2B4:
 goto _LL2AE;_LL2AE:;}{
# 2679
void*telt=Cyc_Tcexp_tcExp(_tmp547,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9E5;struct Cyc_Absyn_PtrInfo _tmp9E4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E3;void*res_typ=(void*)(
(_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E5.tag=5,((_tmp9E5.f1=((_tmp9E4.elt_typ=telt,((_tmp9E4.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9E4.ptr_atts=(
((_tmp9E4.ptr_atts).rgn=rgn,(((_tmp9E4.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9E4.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp9E4.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp9E4.ptr_atts).ptrloc=0,_tmp9E4.ptr_atts)))))))))),_tmp9E4)))))),_tmp9E5)))),_tmp9E3))));
# 2685
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp547,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp547,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2692
void*_tmp593=res_typ;_npop_handler(0);return _tmp593;};};}_LL28B:;};}
# 2542
;_pop_region(r);}
# 2699
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp597=t;void*_tmp599;struct Cyc_Absyn_Tqual _tmp59A;struct Cyc_Absyn_Exp*_tmp59B;union Cyc_Absyn_Constraint*_tmp59C;_LL2B6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp597;if(_tmp598->tag != 8)goto _LL2B8;else{_tmp599=(_tmp598->f1).elt_type;_tmp59A=(_tmp598->f1).tq;_tmp59B=(_tmp598->f1).num_elts;_tmp59C=(_tmp598->f1).zero_term;}}_LL2B7: {
# 2703
struct _tuple14 _stmttmp59=Cyc_Tcutil_addressof_props(te,e);void*_tmp59E;struct _tuple14 _tmp59D=_stmttmp59;_tmp59E=_tmp59D.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9E8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9E7;void*_tmp59F=_tmp59B == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=1,((_tmp9E8.f1=_tmp59B,_tmp9E8)))),_tmp9E7))));
t=Cyc_Absyn_atb_typ(_tmp599,_tmp59E,_tmp59A,_tmp59F,_tmp59C);
e->topt=t;
return t;};}_LL2B8:;_LL2B9:
# 2709
 return t;_LL2B5:;}
# 2716
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2720
struct _RegionHandle _tmp5A2=_new_region("temp");struct _RegionHandle*temp=& _tmp5A2;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2726
const char*_tmp9EB;void*_tmp9EA;(_tmp9EA=0,Cyc_Tcutil_terr(e->loc,((_tmp9EB="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9EB,sizeof(char),49))),_tag_dyneither(_tmp9EA,sizeof(void*),0)));}{
# 2729
void*_stmttmp5A=e->r;void*_tmp5A5=_stmttmp5A;_LL2BB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5A6=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A5;if(_tmp5A6->tag != 25)goto _LL2BD;}_LL2BC:
 goto _LL2BE;_LL2BD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5A7=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A5;if(_tmp5A7->tag != 26)goto _LL2BF;}_LL2BE:
 goto _LL2C0;_LL2BF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5A8=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A5;if(_tmp5A8->tag != 27)goto _LL2C1;}_LL2C0:
 goto _LL2C2;_LL2C1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A5;if(_tmp5A9->tag != 0)goto _LL2C3;else{if(((_tmp5A9->f1).Wstring_c).tag != 9)goto _LL2C3;}}_LL2C2:
 goto _LL2C4;_LL2C3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5AA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A5;if(_tmp5AA->tag != 0)goto _LL2C5;else{if(((_tmp5AA->f1).String_c).tag != 8)goto _LL2C5;}}_LL2C4: {
void*_tmp5AB=t;_npop_handler(0);return _tmp5AB;}_LL2C5:;_LL2C6:
# 2736
 t=Cyc_Tcutil_compress(t);{
void*_tmp5AC=t;void*_tmp5AE;struct Cyc_Absyn_Tqual _tmp5AF;struct Cyc_Absyn_Exp*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5B1;_LL2C8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AC;if(_tmp5AD->tag != 8)goto _LL2CA;else{_tmp5AE=(_tmp5AD->f1).elt_type;_tmp5AF=(_tmp5AD->f1).tq;_tmp5B0=(_tmp5AD->f1).num_elts;_tmp5B1=(_tmp5AD->f1).zero_term;}}_LL2C9: {
# 2739
struct _tuple14 _stmttmp5B=Cyc_Tcutil_addressof_props(te,e);void*_tmp5B3;struct _tuple14 _tmp5B2=_stmttmp5B;_tmp5B3=_tmp5B2.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9EE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9ED;void*b=_tmp5B0 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EE.tag=1,((_tmp9EE.f1=_tmp5B0,_tmp9EE)))),_tmp9ED))));
t=Cyc_Absyn_atb_typ(_tmp5AE,_tmp5B3,_tmp5AF,b,_tmp5B1);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp5B4=t;_npop_handler(0);return _tmp5B4;};};}_LL2CA:;_LL2CB: {
# 2746
void*_tmp5B7=t;_npop_handler(0);return _tmp5B7;}_LL2C7:;};_LL2BA:;};
# 2720
;_pop_region(temp);}
# 2758 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_stmttmp5C=e->r;void*_tmp5B8=_stmttmp5C;struct Cyc_Absyn_Exp*_tmp5BA;_LL2CD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5B9=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5B8;if(_tmp5B9->tag != 11)goto _LL2CF;else{_tmp5BA=_tmp5B9->f1;}}_LL2CE:
# 2762
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5BA);
_tmp5BA->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5BA->topt),0);
e->topt=_tmp5BA->topt;
goto _LL2CC;_LL2CF:;_LL2D0:
# 2768
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2771
{void*_stmttmp5D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5BB=_stmttmp5D;void*_tmp5BD;struct Cyc_Absyn_Tqual _tmp5BE;void*_tmp5BF;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5C1;union Cyc_Absyn_Constraint*_tmp5C2;_LL2D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB;if(_tmp5BC->tag != 5)goto _LL2D4;else{_tmp5BD=(_tmp5BC->f1).elt_typ;_tmp5BE=(_tmp5BC->f1).elt_tq;_tmp5BF=((_tmp5BC->f1).ptr_atts).rgn;_tmp5C0=((_tmp5BC->f1).ptr_atts).nullable;_tmp5C1=((_tmp5BC->f1).ptr_atts).bounds;_tmp5C2=((_tmp5BC->f1).ptr_atts).zero_term;}}_LL2D3:
# 2773
{void*_stmttmp5E=Cyc_Tcutil_compress(_tmp5BD);void*_tmp5C3=_stmttmp5E;struct Cyc_List_List*_tmp5C5;void*_tmp5C6;struct Cyc_Absyn_Tqual _tmp5C7;void*_tmp5C8;struct Cyc_List_List*_tmp5C9;int _tmp5CA;struct Cyc_Absyn_VarargInfo*_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_List_List*_tmp5CD;_LL2D7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5C4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5C3;if(_tmp5C4->tag != 9)goto _LL2D9;else{_tmp5C5=(_tmp5C4->f1).tvars;_tmp5C6=(_tmp5C4->f1).effect;_tmp5C7=(_tmp5C4->f1).ret_tqual;_tmp5C8=(_tmp5C4->f1).ret_typ;_tmp5C9=(_tmp5C4->f1).args;_tmp5CA=(_tmp5C4->f1).c_varargs;_tmp5CB=(_tmp5C4->f1).cyc_varargs;_tmp5CC=(_tmp5C4->f1).rgn_po;_tmp5CD=(_tmp5C4->f1).attributes;}}_LL2D8:
# 2775
 if(_tmp5C5 != 0){
struct _RegionHandle _tmp5CE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5CE;_push_region(rgn);
{struct _tuple12 _tmp9EF;struct _tuple12 _tmp5CF=(_tmp9EF.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9EF.f2=rgn,_tmp9EF)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5CF,_tmp5C5);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2784
_tmp5CC=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5CC);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5CC);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9F5;struct Cyc_Absyn_FnInfo _tmp9F4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F3;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F5.tag=9,((_tmp9F5.f1=((_tmp9F4.tvars=0,((_tmp9F4.effect=_tmp5C6,((_tmp9F4.ret_tqual=_tmp5C7,((_tmp9F4.ret_typ=_tmp5C8,((_tmp9F4.args=_tmp5C9,((_tmp9F4.c_varargs=_tmp5CA,((_tmp9F4.cyc_varargs=_tmp5CB,((_tmp9F4.rgn_po=0,((_tmp9F4.attributes=_tmp5CD,_tmp9F4)))))))))))))))))),_tmp9F5)))),_tmp9F3)))));
# 2790
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9FB;struct Cyc_Absyn_PtrInfo _tmp9FA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D0=(_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9[0]=((_tmp9FB.tag=5,((_tmp9FB.f1=((_tmp9FA.elt_typ=ftyp,((_tmp9FA.elt_tq=_tmp5BE,((_tmp9FA.ptr_atts=(((_tmp9FA.ptr_atts).rgn=_tmp5BF,(((_tmp9FA.ptr_atts).nullable=_tmp5C0,(((_tmp9FA.ptr_atts).bounds=_tmp5C1,(((_tmp9FA.ptr_atts).zero_term=_tmp5C2,(((_tmp9FA.ptr_atts).ptrloc=0,_tmp9FA.ptr_atts)))))))))),_tmp9FA)))))),_tmp9FB)))),_tmp9F9)));
# 2792
struct Cyc_Absyn_Exp*_tmp5D1=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9FE;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9FD;e->r=(void*)((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD[0]=((_tmp9FE.tag=12,((_tmp9FE.f1=_tmp5D1,((_tmp9FE.f2=ts,_tmp9FE)))))),_tmp9FD))));}
e->topt=(void*)_tmp5D0;};}
# 2777
;_pop_region(rgn);}
# 2797
goto _LL2D6;_LL2D9:;_LL2DA:
 goto _LL2D6;_LL2D6:;}
# 2800
goto _LL2D1;_LL2D4:;_LL2D5:
 goto _LL2D1;_LL2D1:;}
# 2803
goto _LL2CC;_LL2CC:;}
# 2805
return(void*)_check_null(e->topt);}
# 2813
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2815
struct _RegionHandle _tmp5DB=_new_region("r");struct _RegionHandle*r=& _tmp5DB;_push_region(r);
{struct Cyc_List_List*_tmp5DC=0;
# 2818
{void*_stmttmp5F=fn_exp->r;void*_tmp5DD=_stmttmp5F;struct Cyc_List_List*_tmp5DF;_LL2DC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5DD;if(_tmp5DE->tag != 9)goto _LL2DE;else{_tmp5DF=_tmp5DE->f2;}}_LL2DD:
# 2820
{void*_stmttmp60=e->r;void*_tmp5E0=_stmttmp60;struct Cyc_List_List*_tmp5E2;_LL2E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5E1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5E0;if(_tmp5E1->tag != 9)goto _LL2E3;else{_tmp5E2=_tmp5E1->f2;}}_LL2E2: {
# 2822
struct Cyc_List_List*_tmp5E3=alias_arg_exps;
int _tmp5E4=0;
while(_tmp5E3 != 0){
while(_tmp5E4 != (int)_tmp5E3->hd){
if(_tmp5DF == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA08;struct Cyc_Int_pa_PrintArg_struct _tmpA07;void*_tmpA06[2];const char*_tmpA05;void*_tmpA04;(_tmpA04=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA08.tag=1,((_tmpA08.f1=(unsigned long)((int)_tmp5E3->hd),((_tmpA07.tag=1,((_tmpA07.f1=(unsigned long)_tmp5E4,((_tmpA06[0]=& _tmpA07,((_tmpA06[1]=& _tmpA08,Cyc_aprintf(((_tmpA05="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA05,sizeof(char),36))),_tag_dyneither(_tmpA06,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA04,sizeof(void*),0)));}
# 2829
++ _tmp5E4;
_tmp5DF=_tmp5DF->tl;
_tmp5E2=((struct Cyc_List_List*)_check_null(_tmp5E2))->tl;}{
# 2834
struct _tuple11 _stmttmp61=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5DF))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5E2))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EC;struct _tuple11 _tmp5EA=_stmttmp61;_tmp5EB=_tmp5EA.f1;_tmp5EC=_tmp5EA.f2;
_tmp5DF->hd=(void*)_tmp5EC;
{struct Cyc_List_List*_tmpA09;_tmp5DC=((_tmpA09=_region_malloc(r,sizeof(*_tmpA09)),((_tmpA09->hd=_tmp5EB,((_tmpA09->tl=_tmp5DC,_tmpA09))))));}
_tmp5E3=_tmp5E3->tl;};}
# 2839
goto _LL2E0;}_LL2E3:;_LL2E4: {
const char*_tmpA0C;void*_tmpA0B;(_tmpA0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA0C="not a function call!",_tag_dyneither(_tmpA0C,sizeof(char),21))),_tag_dyneither(_tmpA0B,sizeof(void*),0)));}_LL2E0:;}
# 2842
goto _LL2DB;_LL2DE:;_LL2DF: {
const char*_tmpA0F;void*_tmpA0E;(_tmpA0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA0F="not a function call!",_tag_dyneither(_tmpA0F,sizeof(char),21))),_tag_dyneither(_tmpA0E,sizeof(void*),0)));}_LL2DB:;}
# 2847
while(_tmp5DC != 0){
struct Cyc_Absyn_Decl*_tmp5F2=(struct Cyc_Absyn_Decl*)_tmp5DC->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5F2,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5DC=_tmp5DC->tl;}
# 2854
e->topt=0;
e->r=fn_exp->r;}
# 2816
;_pop_region(r);}
# 2861
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2865
{void*_stmttmp62=e->r;void*_tmp5F3=_stmttmp62;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct Cyc_Absyn_VarargCallInfo**_tmp5F9;int*_tmp5FA;struct Cyc_Core_Opt*_tmp5FC;struct Cyc_List_List*_tmp5FD;union Cyc_Absyn_Cnst*_tmp5FF;struct _tuple1*_tmp601;void**_tmp602;enum Cyc_Absyn_Primop _tmp604;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_Exp*_tmp607;enum Cyc_Absyn_Incrementor _tmp608;struct Cyc_Absyn_Exp*_tmp60A;struct Cyc_Core_Opt*_tmp60B;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;struct Cyc_Absyn_Exp*_tmp612;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Exp*_tmp615;struct Cyc_Absyn_Exp*_tmp616;struct Cyc_Absyn_Exp*_tmp618;struct Cyc_Absyn_Exp*_tmp619;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_List_List*_tmp61C;struct Cyc_Absyn_VarargCallInfo**_tmp61D;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Exp*_tmp621;struct Cyc_List_List*_tmp622;void*_tmp624;struct Cyc_Absyn_Exp*_tmp625;enum Cyc_Absyn_Coercion*_tmp626;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Exp**_tmp62A;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Exp*_tmp62D;void*_tmp62F;void*_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_Absyn_Exp*_tmp636;struct _dyneither_ptr*_tmp637;int*_tmp638;int*_tmp639;struct Cyc_Absyn_Exp*_tmp63B;struct _dyneither_ptr*_tmp63C;int*_tmp63D;int*_tmp63E;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_List_List*_tmp643;struct _tuple9*_tmp645;struct Cyc_List_List*_tmp646;struct Cyc_List_List*_tmp648;struct Cyc_Absyn_Stmt*_tmp64A;struct Cyc_Absyn_Vardecl*_tmp64C;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64E;int*_tmp64F;struct Cyc_Absyn_Exp*_tmp651;void*_tmp652;int*_tmp653;struct _tuple1**_tmp655;struct Cyc_List_List**_tmp656;struct Cyc_List_List*_tmp657;struct Cyc_Absyn_Aggrdecl**_tmp658;void*_tmp65A;struct Cyc_List_List*_tmp65B;struct Cyc_List_List*_tmp65D;struct Cyc_Absyn_Datatypedecl*_tmp65E;struct Cyc_Absyn_Datatypefield*_tmp65F;struct _tuple1**_tmp661;struct Cyc_Absyn_Enumdecl*_tmp662;struct Cyc_Absyn_Enumfield*_tmp663;struct _tuple1**_tmp665;void*_tmp666;struct Cyc_Absyn_Enumfield*_tmp667;int*_tmp669;struct Cyc_Absyn_Exp**_tmp66A;void***_tmp66B;struct Cyc_Absyn_Exp**_tmp66C;int*_tmp66D;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp672;struct _dyneither_ptr*_tmp673;void*_tmp675;_LL2E6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5F4->tag != 11)goto _LL2E8;else{_tmp5F5=_tmp5F4->f1;}}_LL2E7:
# 2870
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5F5);
return;_LL2E8:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F6=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5F6->tag != 9)goto _LL2EA;else{_tmp5F7=_tmp5F6->f1;_tmp5F8=_tmp5F6->f2;_tmp5F9=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5F6->f3;_tmp5FA=(int*)& _tmp5F6->f4;}}if(!(!(*_tmp5FA)))goto _LL2EA;_LL2E9:
# 2876
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5F7,_tmp5F8)){
*_tmp5FA=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2880
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2883
return;_LL2EA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5FB->tag != 35)goto _LL2EC;else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;}}_LL2EB:
# 2887
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5FD);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2EC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5FE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp5FE->tag != 0)goto _LL2EE;else{_tmp5FF=(union Cyc_Absyn_Cnst*)& _tmp5FE->f1;}}_LL2ED:
# 2892
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5FF,e);goto _LL2E5;_LL2EE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp600=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp600->tag != 1)goto _LL2F0;else{_tmp601=_tmp600->f1;_tmp602=(void**)((void**)& _tmp600->f2);}}_LL2EF:
# 2894
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp601,_tmp602);goto _LL2E5;_LL2F0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp603=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp603->tag != 2)goto _LL2F2;else{_tmp604=_tmp603->f1;_tmp605=_tmp603->f2;}}_LL2F1:
# 2896
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp604,_tmp605);goto _LL2E5;_LL2F2: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp606=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp606->tag != 4)goto _LL2F4;else{_tmp607=_tmp606->f1;_tmp608=_tmp606->f2;}}_LL2F3:
# 2898
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp607,_tmp608);goto _LL2E5;_LL2F4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp609=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp609->tag != 3)goto _LL2F6;else{_tmp60A=_tmp609->f1;_tmp60B=_tmp609->f2;_tmp60C=_tmp609->f3;}}_LL2F5:
# 2900
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp60A,_tmp60B,_tmp60C);goto _LL2E5;_LL2F6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp60D->tag != 5)goto _LL2F8;else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60D->f2;_tmp610=_tmp60D->f3;}}_LL2F7:
# 2902
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp60E,_tmp60F,_tmp610);goto _LL2E5;_LL2F8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp611->tag != 6)goto _LL2FA;else{_tmp612=_tmp611->f1;_tmp613=_tmp611->f2;}}_LL2F9:
# 2904
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp612,_tmp613);goto _LL2E5;_LL2FA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp614->tag != 7)goto _LL2FC;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;}}_LL2FB:
# 2906
 t=Cyc_Tcexp_tcOr(te,loc,_tmp615,_tmp616);goto _LL2E5;_LL2FC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp617->tag != 8)goto _LL2FE;else{_tmp618=_tmp617->f1;_tmp619=_tmp617->f2;}}_LL2FD:
# 2908
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp618,_tmp619);goto _LL2E5;_LL2FE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp61A->tag != 9)goto _LL300;else{_tmp61B=_tmp61A->f1;_tmp61C=_tmp61A->f2;_tmp61D=(struct Cyc_Absyn_VarargCallInfo**)& _tmp61A->f3;}}_LL2FF: {
# 2914
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp677;_push_handler(& _tmp677);{int _tmp679=0;if(setjmp(_tmp677.handler))_tmp679=1;if(!_tmp679){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp678=(void*)_exn_thrown;void*_tmp67B=_tmp678;struct _dyneither_ptr _tmp67D;void*_tmp67E;_LL339: {struct Cyc_Core_Failure_exn_struct*_tmp67C=(struct Cyc_Core_Failure_exn_struct*)_tmp67B;if(_tmp67C->tag != Cyc_Core_Failure)goto _LL33B;else{_tmp67D=_tmp67C->f1;}}_LL33A:
# 2921
 ok=0;
fn_exp=e;
goto _LL338;_LL33B: _tmp67E=_tmp67B;_LL33C:(void)_throw(_tmp67E);_LL338:;}};}
# 2925
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp61B,_tmp61C,_tmp61D,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2932
goto _LL2E5;}_LL300: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp61E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp61E->tag != 10)goto _LL302;else{_tmp61F=_tmp61E->f1;}}_LL301:
# 2934
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp61F);goto _LL2E5;_LL302: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp620=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp620->tag != 12)goto _LL304;else{_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;}}_LL303:
# 2936
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp621,_tmp622);goto _LL2E5;_LL304: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp623=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp623->tag != 13)goto _LL306;else{_tmp624=(void*)_tmp623->f1;_tmp625=_tmp623->f2;_tmp626=(enum Cyc_Absyn_Coercion*)& _tmp623->f4;}}_LL305:
# 2938
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp624,_tmp625,_tmp626);goto _LL2E5;_LL306: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp627=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp627->tag != 14)goto _LL308;else{_tmp628=_tmp627->f1;}}_LL307:
# 2940
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp628);goto _LL2E5;_LL308: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp629=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp629->tag != 15)goto _LL30A;else{_tmp62A=(struct Cyc_Absyn_Exp**)& _tmp629->f1;_tmp62B=_tmp629->f2;}}_LL309:
# 2942
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp62A,e,_tmp62B);goto _LL2E5;_LL30A: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp62C=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp62C->tag != 17)goto _LL30C;else{_tmp62D=_tmp62C->f1;}}_LL30B: {
# 2944
void*_tmp67F=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp62D);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp67F);goto _LL2E5;}_LL30C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp62E->tag != 16)goto _LL30E;else{_tmp62F=(void*)_tmp62E->f1;}}_LL30D:
# 2947
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp62F);goto _LL2E5;_LL30E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp630->tag != 18)goto _LL310;else{_tmp631=(void*)_tmp630->f1;_tmp632=_tmp630->f2;}}_LL30F:
# 2949
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp631,_tmp632);goto _LL2E5;_LL310: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp633=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp633->tag != 19)goto _LL312;else{_tmp634=_tmp633->f1;}}_LL311:
# 2951
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp634);goto _LL2E5;_LL312: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp635=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp635->tag != 20)goto _LL314;else{_tmp636=_tmp635->f1;_tmp637=_tmp635->f2;_tmp638=(int*)& _tmp635->f3;_tmp639=(int*)& _tmp635->f4;}}_LL313:
# 2953
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp636,_tmp637,_tmp638,_tmp639);goto _LL2E5;_LL314: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp63A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp63A->tag != 21)goto _LL316;else{_tmp63B=_tmp63A->f1;_tmp63C=_tmp63A->f2;_tmp63D=(int*)& _tmp63A->f3;_tmp63E=(int*)& _tmp63A->f4;}}_LL315:
# 2955
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp63B,_tmp63C,_tmp63D,_tmp63E);goto _LL2E5;_LL316: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp63F->tag != 22)goto _LL318;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL317:
# 2957
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp640,_tmp641);goto _LL2E5;_LL318: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp642->tag != 23)goto _LL31A;else{_tmp643=_tmp642->f1;}}_LL319:
# 2959
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp643);goto _LL2E5;_LL31A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp644->tag != 24)goto _LL31C;else{_tmp645=_tmp644->f1;_tmp646=_tmp644->f2;}}_LL31B:
# 2961
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp645,_tmp646);goto _LL2E5;_LL31C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp647=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp647->tag != 25)goto _LL31E;else{_tmp648=_tmp647->f1;}}_LL31D: {
# 2965
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp63=Cyc_Tcutil_compress(*topt);void*_tmp680=_stmttmp63;void**_tmp682;struct Cyc_Absyn_Tqual*_tmp683;union Cyc_Absyn_Constraint*_tmp684;_LL33E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp681=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp680;if(_tmp681->tag != 8)goto _LL340;else{_tmp682=(void**)&(_tmp681->f1).elt_type;_tmp683=(struct Cyc_Absyn_Tqual*)&(_tmp681->f1).tq;_tmp684=(_tmp681->f1).zero_term;}}_LL33F:
# 2971
 elt_topt=_tmp682;
elt_tqopt=_tmp683;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp684);
goto _LL33D;_LL340:;_LL341:
 goto _LL33D;_LL33D:;}
# 2978
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp648);goto _LL2E5;}_LL31E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp649->tag != 36)goto _LL320;else{_tmp64A=_tmp649->f1;}}_LL31F:
# 2980
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp64A);goto _LL2E5;_LL320: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp64B->tag != 26)goto _LL322;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;_tmp64E=_tmp64B->f3;_tmp64F=(int*)& _tmp64B->f4;}}_LL321:
# 2982
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp64C,_tmp64D,_tmp64E,_tmp64F);goto _LL2E5;_LL322: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp650->tag != 27)goto _LL324;else{_tmp651=_tmp650->f1;_tmp652=(void*)_tmp650->f2;_tmp653=(int*)& _tmp650->f3;}}_LL323:
# 2984
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp651,_tmp652,_tmp653);goto _LL2E5;_LL324: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp654=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp654->tag != 28)goto _LL326;else{_tmp655=(struct _tuple1**)& _tmp654->f1;_tmp656=(struct Cyc_List_List**)& _tmp654->f2;_tmp657=_tmp654->f3;_tmp658=(struct Cyc_Absyn_Aggrdecl**)& _tmp654->f4;}}_LL325:
# 2986
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp655,_tmp656,_tmp657,_tmp658);goto _LL2E5;_LL326: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp659=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp659->tag != 29)goto _LL328;else{_tmp65A=(void*)_tmp659->f1;_tmp65B=_tmp659->f2;}}_LL327:
# 2988
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp65A,_tmp65B);goto _LL2E5;_LL328: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp65C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp65C->tag != 30)goto _LL32A;else{_tmp65D=_tmp65C->f1;_tmp65E=_tmp65C->f2;_tmp65F=_tmp65C->f3;}}_LL329:
# 2990
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp65D,_tmp65E,_tmp65F);goto _LL2E5;_LL32A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp660=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp660->tag != 31)goto _LL32C;else{_tmp661=(struct _tuple1**)& _tmp660->f1;_tmp662=_tmp660->f2;_tmp663=_tmp660->f3;}}_LL32B:
# 2992
*_tmp661=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp663))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA12;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA11;t=(void*)((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA12.tag=13,((_tmpA12.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp662))->name,((_tmpA12.f2=_tmp662,_tmpA12)))))),_tmpA11))));}goto _LL2E5;_LL32C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp664->tag != 32)goto _LL32E;else{_tmp665=(struct _tuple1**)& _tmp664->f1;_tmp666=(void*)_tmp664->f2;_tmp667=_tmp664->f3;}}_LL32D:
# 2995
*_tmp665=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp667))->name;
t=_tmp666;goto _LL2E5;_LL32E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp668=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp668->tag != 33)goto _LL330;else{_tmp669=(int*)&(_tmp668->f1).is_calloc;_tmp66A=(struct Cyc_Absyn_Exp**)&(_tmp668->f1).rgn;_tmp66B=(void***)&(_tmp668->f1).elt_type;_tmp66C=(struct Cyc_Absyn_Exp**)&(_tmp668->f1).num_elts;_tmp66D=(int*)&(_tmp668->f1).fat_result;}}_LL32F:
# 2998
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp66A,_tmp66B,_tmp66C,_tmp669,_tmp66D);goto _LL2E5;_LL330: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp66E->tag != 34)goto _LL332;else{_tmp66F=_tmp66E->f1;_tmp670=_tmp66E->f2;}}_LL331:
# 3000
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp66F,_tmp670);goto _LL2E5;_LL332: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp671->tag != 37)goto _LL334;else{_tmp672=_tmp671->f1;_tmp673=_tmp671->f2;}}_LL333:
# 3002
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp672,_tmp673);goto _LL2E5;_LL334: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp674=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp674->tag != 38)goto _LL336;else{_tmp675=(void*)_tmp674->f1;}}_LL335:
# 3004
 if(!te->allow_valueof){
const char*_tmpA15;void*_tmpA14;(_tmpA14=0,Cyc_Tcutil_terr(e->loc,((_tmpA15="valueof(-) can only occur within types",_tag_dyneither(_tmpA15,sizeof(char),39))),_tag_dyneither(_tmpA14,sizeof(void*),0)));}
# 3012
t=Cyc_Absyn_sint_typ;
goto _LL2E5;_LL336: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp676=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5F3;if(_tmp676->tag != 39)goto _LL2E5;}_LL337:
# 3015
{const char*_tmpA18;void*_tmpA17;(_tmpA17=0,Cyc_Tcutil_terr(e->loc,((_tmpA18="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA18,sizeof(char),50))),_tag_dyneither(_tmpA17,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2E5:;}
# 3018
e->topt=t;}
