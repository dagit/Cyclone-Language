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
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
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
# 89
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 125 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
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
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 173
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 214
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 221
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 80
int Cyc_Tcutil_coerceable(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 136
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 152
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 154
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 158
void Cyc_Tcutil_explain_failure();
# 160
int Cyc_Tcutil_unify(void*,void*);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 192
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 201
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 237 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 247
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 249
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 251
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 255
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 263
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 265
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 275
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 280
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 283
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 302
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 329
int Cyc_Tcutil_bits_only(void*t);
# 332
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 361
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 364
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
{void*_tmp698;(_tmp698=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp698,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp699;return(_tmp699=_cycalloc(sizeof(*_tmp699)),((_tmp699->f1=0,((_tmp699->f2=e,_tmp699)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_stmttmp0=e1->r;void*_tmp2=_stmttmp0;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_stmttmp1=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpA=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp69D;void*_tmp69C[1];struct Cyc_String_pa_PrintArg_struct _tmp69B;(_tmp69B.tag=0,((_tmp69B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp69C[0]=& _tmp69B,Cyc_Tcutil_terr(e1->loc,((_tmp69D="undeclared identifier %s",_tag_dyneither(_tmp69D,sizeof(char),25))),_tag_dyneither(_tmp69C,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp6A1;void*_tmp6A0[1];struct Cyc_String_pa_PrintArg_struct _tmp69F;(_tmp69F.tag=0,((_tmp69F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp6A0[0]=& _tmp69F,Cyc_Tcutil_terr(e->loc,((_tmp6A1="%s is a constant, not a function",_tag_dyneither(_tmp6A1,sizeof(char),33))),_tag_dyneither(_tmp6A0,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6A4;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6A3;e->r=(void*)((_tmp6A3=_cycalloc(sizeof(*_tmp6A3)),((_tmp6A3[0]=((_tmp6A4.tag=30,((_tmp6A4.f1=es,((_tmp6A4.f2=_tmp10,((_tmp6A4.f3=_tmp11,_tmp6A4)))))))),_tmp6A3))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6A7;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6A6;e->r=(void*)((_tmp6A6=_cycalloc(sizeof(*_tmp6A6)),((_tmp6A6[0]=((_tmp6A7.tag=28,((_tmp6A7.f1=_tmp13->name,((_tmp6A7.f2=0,((_tmp6A7.f3=_tmp22,((_tmp6A7.f4=_tmp13,_tmp6A7)))))))))),_tmp6A6))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6AB;void*_tmp6AA[1];struct Cyc_String_pa_PrintArg_struct _tmp6A9;(_tmp6A9.tag=0,((_tmp6A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6AA[0]=& _tmp6A9,Cyc_Tcutil_terr(e->loc,((_tmp6AB="%s is an enum constructor, not a function",_tag_dyneither(_tmp6AB,sizeof(char),42))),_tag_dyneither(_tmp6AA,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6AF;void*_tmp6AE[1];struct Cyc_String_pa_PrintArg_struct _tmp6AD;(_tmp6AD.tag=0,((_tmp6AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6AE[0]=& _tmp6AD,Cyc_Tcutil_terr(e1->loc,((_tmp6AF="undeclared identifier %s",_tag_dyneither(_tmp6AF,sizeof(char),25))),_tag_dyneither(_tmp6AE,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6B2;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6B1;e->r=(void*)((_tmp6B1=_cycalloc(sizeof(*_tmp6B1)),((_tmp6B1[0]=((_tmp6B2.tag=25,((_tmp6B2.f1=des,_tmp6B2)))),_tmp6B1))));}
return;}{
# 125
void*t=*topt;
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp33=_stmttmp2;union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6B5;void*_tmp6B4;(_tmp6B4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6B5="struct type not properly set",_tag_dyneither(_tmp6B5,sizeof(char),29))),_tag_dyneither(_tmp6B4,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6B8;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6B7;e->r=(void*)((_tmp6B7=_cycalloc(sizeof(*_tmp6B7)),((_tmp6B7[0]=((_tmp6B8.tag=28,((_tmp6B8.f1=_tmp3B->name,((_tmp6B8.f2=0,((_tmp6B8.f3=des,((_tmp6B8.f4=_tmp3B,_tmp6B8)))))))))),_tmp6B7))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6BB;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6BA;e->r=(void*)((_tmp6BA=_cycalloc(sizeof(*_tmp6BA)),((_tmp6BA[0]=((_tmp6BB.tag=25,((_tmp6BB.f1=des,_tmp6BB)))),_tmp6BA))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6BE;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6BD;e->r=(void*)((_tmp6BD=_cycalloc(sizeof(*_tmp6BD)),((_tmp6BD[0]=((_tmp6BE.tag=29,((_tmp6BE.f1=t,((_tmp6BE.f2=des,_tmp6BE)))))),_tmp6BD))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6C1;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6C0;e->r=(void*)((_tmp6C0=_cycalloc(sizeof(*_tmp6C0)),((_tmp6C0[0]=((_tmp6C1.tag=25,((_tmp6C1.f1=des,_tmp6C1)))),_tmp6C0))));}goto _LL17;_LL17:;};}
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
const char*_tmp6C6;void*_tmp6C5[2];struct Cyc_String_pa_PrintArg_struct _tmp6C4;struct Cyc_String_pa_PrintArg_struct _tmp6C3;(_tmp6C3.tag=0,((_tmp6C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6C4.tag=0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6C5[0]=& _tmp6C4,((_tmp6C5[1]=& _tmp6C3,Cyc_Tcutil_terr(e->loc,((_tmp6C6="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp6C6,sizeof(char),59))),_tag_dyneither(_tmp6C5,sizeof(void*),2)))))))))))));}
# 161
{void*_stmttmp3=e->r;void*_tmp4A=_stmttmp3;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp6C7;struct _tuple0 _stmttmp4=(_tmp6C7.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp6C7.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp6C7)));struct _tuple0 _tmp4E=_stmttmp4;void*_tmp50;void*_tmp52;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f1;if(_tmp4F->tag != 15)goto _LL2D;else{_tmp50=(void*)_tmp4F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f2;if(_tmp51->tag != 15)goto _LL2D;else{_tmp52=(void*)_tmp51->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6CA;struct Cyc_Tcexp_TestEnv _tmp6C9;return(_tmp6C9.eq=((_tmp6CA=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6CA)),((_tmp6CA->f1=_tmp50,((_tmp6CA->f2=_tmp52,_tmp6CA)))))),((_tmp6C9.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp6C9)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6CB;return(_tmp6CB.eq=0,((_tmp6CB.isTrue=0,_tmp6CB)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6D1;struct Cyc_Absyn_PtrInfo _tmp6D0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CF;t=(void*)((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF[0]=((_tmp6D1.tag=5,((_tmp6D1.f1=((_tmp6D0.elt_typ=_tmp69,((_tmp6D0.elt_tq=_tmp6A,((_tmp6D0.ptr_atts=(
((_tmp6D0.ptr_atts).rgn=_tmp6B,(((_tmp6D0.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6D0.ptr_atts).bounds=_tmp6D,(((_tmp6D0.ptr_atts).zero_term=_tmp6E,(((_tmp6D0.ptr_atts).ptrloc=0,_tmp6D0.ptr_atts)))))))))),_tmp6D0)))))),_tmp6D1)))),_tmp6CF))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6D4;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6D3;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp6D3=_cycalloc(sizeof(*_tmp6D3)),((_tmp6D3[0]=((_tmp6D4.tag=18,((_tmp6D4.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp6D4)))),_tmp6D3)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6D7;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6D6;t=(void*)((_tmp6D6=_cycalloc(sizeof(*_tmp6D6)),((_tmp6D6[0]=((_tmp6D7.tag=19,((_tmp6D7.f1=(void*)_tmp76,_tmp6D7)))),_tmp6D6))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6DA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp6D9=_cycalloc(sizeof(*_tmp6D9)),((_tmp6D9[0]=((_tmp6DA.tag=1,((_tmp6DA.f1=elen,_tmp6DA)))),_tmp6D9)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6E0;struct Cyc_Absyn_PtrInfo _tmp6DF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DE;return(void*)((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6E0.tag=5,((_tmp6E0.f1=((_tmp6DF.elt_typ=_tmp84,((_tmp6DF.elt_tq=_tmp85,((_tmp6DF.ptr_atts=(((_tmp6DF.ptr_atts).rgn=_tmp86,(((_tmp6DF.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6DF.ptr_atts).bounds=_tmp88,(((_tmp6DF.ptr_atts).zero_term=_tmp89,(((_tmp6DF.ptr_atts).ptrloc=0,_tmp6DF.ptr_atts)))))))))),_tmp6DF)))))),_tmp6E0)))),_tmp6DE))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6EF;struct Cyc_Core_Opt*_tmp6EE;struct Cyc_Core_Opt*_tmp6ED;struct Cyc_Absyn_PtrInfo _tmp6EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6EB;t=(void*)((_tmp6EB=_cycalloc(sizeof(*_tmp6EB)),((_tmp6EB[0]=((_tmp6EF.tag=5,((_tmp6EF.f1=((_tmp6EC.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED->v=_tmp8D,_tmp6ED))))),((_tmp6EC.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6EC.ptr_atts=(
((_tmp6EC.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE->v=_tmp8D,_tmp6EE))))),(((_tmp6EC.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6EC.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6EC.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6EC.ptr_atts).ptrloc=0,_tmp6EC.ptr_atts)))))))))),_tmp6EC)))))),_tmp6EF)))),_tmp6EB))));}
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
const char*_tmp6F3;void*_tmp6F2[1];struct Cyc_String_pa_PrintArg_struct _tmp6F1;void*_tmpB5=(_tmp6F1.tag=0,((_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6F2[0]=& _tmp6F1,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6F3="undeclared identifier %s",_tag_dyneither(_tmp6F3,sizeof(char),25))),_tag_dyneither(_tmp6F2,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6F6;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6F5;e->r=(void*)((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5[0]=((_tmp6F6.tag=31,((_tmp6F6.f1=_tmpAA->name,((_tmp6F6.f2=_tmpA9,((_tmp6F6.f3=_tmpAA,_tmp6F6)))))))),_tmp6F5))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6F9;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6F8;void*_tmpBB=(void*)((_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8[0]=((_tmp6F9.tag=13,((_tmp6F9.f1=_tmpA9->name,((_tmp6F9.f2=_tmpA9,_tmp6F9)))))),_tmp6F8))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6FC;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6FB;e->r=(void*)((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB[0]=((_tmp6FC.tag=32,((_tmp6FC.f1=_tmpAD->name,((_tmp6FC.f2=_tmpAC,((_tmp6FC.f3=_tmpAD,_tmp6FC)))))))),_tmp6FB))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6FF;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6FE;e->r=(void*)((_tmp6FE=_cycalloc(sizeof(*_tmp6FE)),((_tmp6FE[0]=((_tmp6FF.tag=30,((_tmp6FF.f1=0,((_tmp6FF.f2=_tmpAF,((_tmp6FF.f3=_tmpB0,_tmp6FF)))))))),_tmp6FE))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp703;void*_tmp702[1];struct Cyc_String_pa_PrintArg_struct _tmp701;void*_tmpC5=(_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp702[0]=& _tmp701,Cyc_Tcexp_expr_err(te,loc,0,((_tmp703="bad occurrence of type name %s",_tag_dyneither(_tmp703,sizeof(char),31))),_tag_dyneither(_tmp702,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp707;void*_tmp706[1];struct Cyc_String_pa_PrintArg_struct _tmp705;return(_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp706[0]=& _tmp705,Cyc_Tcexp_expr_err(te,loc,0,((_tmp707="undeclared identifier %s",_tag_dyneither(_tmp707,sizeof(char),25))),_tag_dyneither(_tmp706,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp708;(*q).f1=(((_tmp708.Loc_n).val=0,(((_tmp708.Loc_n).tag=4,_tmp708))));}
if(te->allow_valueof){
void*_stmttmpA=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpCE=_stmttmpA;void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp70B;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp70A;e->r=(void*)((_tmp70A=_cycalloc(sizeof(*_tmp70A)),((_tmp70A[0]=((_tmp70B.tag=38,((_tmp70B.f1=_tmpD0,_tmp70B)))),_tmp70A))));}
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
const char*_tmp70E;void*_tmp70D;(_tmp70D=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp70E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp70E,sizeof(char),49))),_tag_dyneither(_tmp70D,sizeof(void*),0)));}
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
{const char*_tmp713;void*_tmp712[2];struct Cyc_String_pa_PrintArg_struct _tmp711;struct Cyc_String_pa_PrintArg_struct _tmp710;(_tmp710.tag=0,((_tmp710.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp711.tag=0,((_tmp711.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp712[0]=& _tmp711,((_tmp712[1]=& _tmp710,Cyc_Tcutil_terr(e->loc,((_tmp713="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp713,sizeof(char),48))),_tag_dyneither(_tmp712,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp714;*alias_arg_exps=((_tmp714=_cycalloc(sizeof(*_tmp714)),((_tmp714->hd=(void*)arg_cnt,((_tmp714->tl=*alias_arg_exps,_tmp714))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp717="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp717,sizeof(char),49))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp71A;void*_tmp719;(_tmp719=0,Cyc_Tcutil_terr(fmt->loc,((_tmp71A="too few arguments",_tag_dyneither(_tmp71A,sizeof(char),18))),_tag_dyneither(_tmp719,sizeof(void*),0)));}
if(_tmpDD != 0){
{const char*_tmp71D;void*_tmp71C;(_tmp71C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp71D="too many arguments",_tag_dyneither(_tmp71D,sizeof(char),19))),_tag_dyneither(_tmp71C,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp720;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp71F;(int)_throw((void*)((_tmp71F=_cycalloc_atomic(sizeof(*_tmp71F)),((_tmp71F[0]=((_tmp720.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp720)),_tmp71F)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp724;void*_tmp723[1];struct Cyc_String_pa_PrintArg_struct _tmp722;(_tmp722.tag=0,((_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp723[0]=& _tmp722,Cyc_Tcutil_terr(loc,((_tmp724="expecting numeric type but found %s",_tag_dyneither(_tmp724,sizeof(char),36))),_tag_dyneither(_tmp723,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp728;void*_tmp727[1];struct Cyc_String_pa_PrintArg_struct _tmp726;(_tmp726.tag=0,((_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp727[0]=& _tmp726,Cyc_Tcutil_terr(loc,((_tmp728="expecting integral or * type but found %s",_tag_dyneither(_tmp728,sizeof(char),42))),_tag_dyneither(_tmp727,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp72C;void*_tmp72B[1];struct Cyc_String_pa_PrintArg_struct _tmp72A;(_tmp72A.tag=0,((_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp72B[0]=& _tmp72A,Cyc_Tcutil_terr(loc,((_tmp72C="expecting integral type but found %s",_tag_dyneither(_tmp72C,sizeof(char),37))),_tag_dyneither(_tmp72B,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmpF5=t;union Cyc_Absyn_Constraint*_tmpF8;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF5;if(_tmpF6->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF5;if(_tmpF7->tag != 5)goto _LL9B;else{_tmpF8=((_tmpF7->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_stmttmpD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF8);void*_tmpF9=_stmttmpD;struct Cyc_Absyn_Exp*_tmpFC;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF9;if(_tmpFA->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF9;if(_tmpFB->tag != 1)goto _LL9D;else{_tmpFC=_tmpFB->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmpFC)){
const char*_tmp72F;void*_tmp72E;(_tmp72E=0,Cyc_Tcutil_terr(loc,((_tmp72F="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp72F,sizeof(char),55))),_tag_dyneither(_tmp72E,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp733;void*_tmp732[1];struct Cyc_String_pa_PrintArg_struct _tmp731;(_tmp731.tag=0,((_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp732[0]=& _tmp731,Cyc_Tcutil_terr(loc,((_tmp733="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp733,sizeof(char),47))),_tag_dyneither(_tmp732,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp736;void*_tmp735;(_tmp735=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp736="Non-unary primop",_tag_dyneither(_tmp736,sizeof(char),17))),_tag_dyneither(_tmp735,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp73A;void*_tmp739[1];struct Cyc_String_pa_PrintArg_struct _tmp738;(_tmp738.tag=0,((_tmp738.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp739[0]=& _tmp738,Cyc_Tcutil_terr(e1->loc,((_tmp73A="type %s cannot be used here",_tag_dyneither(_tmp73A,sizeof(char),28))),_tag_dyneither(_tmp739,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp73E;void*_tmp73D[1];struct Cyc_String_pa_PrintArg_struct _tmp73C;(_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp73D[0]=& _tmp73C,Cyc_Tcutil_terr(e2->loc,((_tmp73E="type %s cannot be used here",_tag_dyneither(_tmp73E,sizeof(char),28))),_tag_dyneither(_tmp73D,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 497
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 502
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp10A=t1;void*_tmp10C;struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10E;union Cyc_Absyn_Constraint*_tmp10F;union Cyc_Absyn_Constraint*_tmp110;union Cyc_Absyn_Constraint*_tmp111;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10A;if(_tmp10B->tag != 5)goto _LLA6;else{_tmp10C=(_tmp10B->f1).elt_typ;_tmp10D=(_tmp10B->f1).elt_tq;_tmp10E=((_tmp10B->f1).ptr_atts).rgn;_tmp10F=((_tmp10B->f1).ptr_atts).nullable;_tmp110=((_tmp10B->f1).ptr_atts).bounds;_tmp111=((_tmp10B->f1).ptr_atts).zero_term;}}_LLA5:
# 507
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp10C),& Cyc_Tcutil_tmk)){
const char*_tmp741;void*_tmp740;(_tmp740=0,Cyc_Tcutil_terr(e1->loc,((_tmp741="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp741,sizeof(char),50))),_tag_dyneither(_tmp740,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp744;void*_tmp743;(_tmp743=0,Cyc_Tcutil_terr(e1->loc,((_tmp744="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp744,sizeof(char),54))),_tag_dyneither(_tmp743,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp748;void*_tmp747[1];struct Cyc_String_pa_PrintArg_struct _tmp746;(_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp747[0]=& _tmp746,Cyc_Tcutil_terr(e2->loc,((_tmp748="expecting int but found %s",_tag_dyneither(_tmp748,sizeof(char),27))),_tag_dyneither(_tmp747,sizeof(void*),1)))))));}{
void*_stmttmpE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp110);void*_tmp119=_stmttmpE;struct Cyc_Absyn_Exp*_tmp11C;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp119;if(_tmp11A->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp119;if(_tmp11B->tag != 1)goto _LLA8;else{_tmp11C=_tmp11B->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp111)){
const char*_tmp74B;void*_tmp74A;(_tmp74A=0,Cyc_Tcutil_warn(e1->loc,((_tmp74B="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp74B,sizeof(char),70))),_tag_dyneither(_tmp74A,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp751;struct Cyc_Absyn_PtrInfo _tmp750;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11F=(_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F[0]=((_tmp751.tag=5,((_tmp751.f1=((_tmp750.elt_typ=_tmp10C,((_tmp750.elt_tq=_tmp10D,((_tmp750.ptr_atts=(
((_tmp750.ptr_atts).rgn=_tmp10E,(((_tmp750.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp750.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp750.ptr_atts).zero_term=_tmp111,(((_tmp750.ptr_atts).ptrloc=0,_tmp750.ptr_atts)))))))))),_tmp750)))))),_tmp751)))),_tmp74F)));
# 530
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp11F,Cyc_Absyn_Other_coercion);
return(void*)_tmp11F;};_LLA8:;};_LLA6:;_LLA7:
# 533
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 538
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp756;void*_tmp755[2];struct Cyc_String_pa_PrintArg_struct _tmp754;struct Cyc_String_pa_PrintArg_struct _tmp753;(_tmp753.tag=0,((_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp754.tag=0,((_tmp754.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp755[0]=& _tmp754,((_tmp755[1]=& _tmp753,Cyc_Tcutil_terr(e1->loc,((_tmp756="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp756,sizeof(char),59))),_tag_dyneither(_tmp755,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp75B;void*_tmp75A[2];struct Cyc_String_pa_PrintArg_struct _tmp759;struct Cyc_String_pa_PrintArg_struct _tmp758;(_tmp758.tag=0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp759.tag=0,((_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp75A[0]=& _tmp759,((_tmp75A[1]=& _tmp758,Cyc_Tcutil_terr(e1->loc,((_tmp75B="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp75B,sizeof(char),59))),_tag_dyneither(_tmp75A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_Tcutil_warn(e1->loc,((_tmp75E="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp75E,sizeof(char),60))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_Tcutil_terr(e1->loc,((_tmp761="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp761,sizeof(char),50))),_tag_dyneither(_tmp760,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp764;void*_tmp763;(_tmp763=0,Cyc_Tcutil_terr(e1->loc,((_tmp764="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp764,sizeof(char),54))),_tag_dyneither(_tmp763,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp769;void*_tmp768[2];struct Cyc_String_pa_PrintArg_struct _tmp767;struct Cyc_String_pa_PrintArg_struct _tmp766;(_tmp766.tag=0,((_tmp766.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp767.tag=0,((_tmp767.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp768[0]=& _tmp767,((_tmp768[1]=& _tmp766,Cyc_Tcutil_terr(e2->loc,((_tmp769="expecting either %s or int but found %s",_tag_dyneither(_tmp769,sizeof(char),40))),_tag_dyneither(_tmp768,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp76C;void*_tmp76B;(_tmp76B=0,Cyc_Tcutil_warn(e1->loc,((_tmp76C="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp76C,sizeof(char),60))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp76F;void*_tmp76E;(_tmp76E=0,Cyc_Tcutil_warn(e1->loc,((_tmp76F="thin pointer subtraction!",_tag_dyneither(_tmp76F,sizeof(char),26))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp772;void*_tmp771;(_tmp771=0,Cyc_Tcutil_warn(e1->loc,((_tmp772="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp772,sizeof(char),56))),_tag_dyneither(_tmp771,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp775;void*_tmp774;(_tmp774=0,Cyc_Tcutil_warn(e1->loc,((_tmp775="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp775,sizeof(char),51))),_tag_dyneither(_tmp774,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 600
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 603
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 611
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 616
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 629
pointer_cmp: {
struct _tuple0 _tmp776;struct _tuple0 _stmttmpF=(_tmp776.f1=Cyc_Tcutil_compress(t1),((_tmp776.f2=Cyc_Tcutil_compress(t2),_tmp776)));struct _tuple0 _tmp13D=_stmttmpF;void*_tmp13F;void*_tmp141;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D.f1;if(_tmp13E->tag != 5)goto _LLB0;else{_tmp13F=(_tmp13E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D.f2;if(_tmp140->tag != 5)goto _LLB0;else{_tmp141=(_tmp140->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp13F,_tmp141))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp142=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13D.f1;if(_tmp142->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13D.f2;if(_tmp143->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp77B;void*_tmp77A[2];struct Cyc_String_pa_PrintArg_struct _tmp779;struct Cyc_String_pa_PrintArg_struct _tmp778;(_tmp778.tag=0,((_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp779.tag=0,((_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp77A[0]=& _tmp779,((_tmp77A[1]=& _tmp778,Cyc_Tcutil_terr(loc,((_tmp77B="comparison not allowed between %s and %s",_tag_dyneither(_tmp77B,sizeof(char),41))),_tag_dyneither(_tmp77A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 645
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 647
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 653
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp77C;struct _tuple0 _stmttmp10=(_tmp77C.f1=t1,((_tmp77C.f2=t2,_tmp77C)));struct _tuple0 _tmp149=_stmttmp10;void*_tmp14B;void*_tmp14D;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp149.f1;if(_tmp14A->tag != 15)goto _LLB7;else{_tmp14B=(void*)_tmp14A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp149.f2;if(_tmp14C->tag != 15)goto _LLB7;else{_tmp14D=(void*)_tmp14C->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp781;void*_tmp780[2];struct Cyc_String_pa_PrintArg_struct _tmp77F;struct Cyc_String_pa_PrintArg_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp780[0]=& _tmp77F,((_tmp780[1]=& _tmp77E,Cyc_Tcutil_terr(loc,((_tmp781="comparison not allowed between %s and %s",_tag_dyneither(_tmp781,sizeof(char),41))),_tag_dyneither(_tmp780,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 669
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 671
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 675
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 678
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 688
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 691
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 696
const char*_tmp784;void*_tmp783;(_tmp783=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp784="bad binary primop",_tag_dyneither(_tmp784,sizeof(char),18))),_tag_dyneither(_tmp783,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp155=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp156=Cyc_Tcexp_tcExp(te,topt,_tmp155);
if(!Cyc_Tcutil_is_numeric(_tmp155)){
const char*_tmp788;void*_tmp787[1];struct Cyc_String_pa_PrintArg_struct _tmp786;(_tmp786.tag=0,((_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp156)),((_tmp787[0]=& _tmp786,Cyc_Tcutil_terr(_tmp155->loc,((_tmp788="expecting numeric type but found %s",_tag_dyneither(_tmp788,sizeof(char),36))),_tag_dyneither(_tmp787,sizeof(void*),1)))))));}
return _tmp156;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _stmttmp11=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_stmttmp11){case 0: _LLCC: {
const char*_tmp78B;void*_tmp78A;return(_tmp78A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78B="primitive operator has 0 arguments",_tag_dyneither(_tmp78B,sizeof(char),35))),_tag_dyneither(_tmp78A,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp78E;void*_tmp78D;return(_tmp78D=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78E="primitive operator has > 2 arguments",_tag_dyneither(_tmp78E,sizeof(char),37))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp15E=Cyc_Tcutil_compress(t);
void*_tmp15F=_tmp15E;struct Cyc_Absyn_Aggrdecl*_tmp161;struct Cyc_List_List*_tmp163;struct Cyc_Absyn_Datatypefield*_tmp165;void*_tmp167;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_List_List*_tmp16A;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15F;if(_tmp160->tag != 11)goto _LLD4;else{if((((_tmp160->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp161=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp160->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp161->impl == 0){
{const char*_tmp791;void*_tmp790;(_tmp790=0,Cyc_Tcutil_terr(loc,((_tmp791="attempt to write an abstract aggregate",_tag_dyneither(_tmp791,sizeof(char),39))),_tag_dyneither(_tmp790,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp163=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15F;if(_tmp162->tag != 12)goto _LLD6;else{_tmp163=_tmp162->f2;}}_LLD5:
# 736
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct Cyc_Absyn_Aggrfield*_tmp16D=(struct Cyc_Absyn_Aggrfield*)_tmp163->hd;
if((_tmp16D->tq).real_const){
{const char*_tmp795;void*_tmp794[1];struct Cyc_String_pa_PrintArg_struct _tmp793;(_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp16D->name),((_tmp794[0]=& _tmp793,Cyc_Tcutil_terr(loc,((_tmp795="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp795,sizeof(char),56))),_tag_dyneither(_tmp794,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15F;if(_tmp164->tag != 4)goto _LLD8;else{if((((_tmp164->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp165=((struct _tuple2)(((_tmp164->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp171=_tmp165->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){
struct _tuple18*_stmttmp13=(struct _tuple18*)_tmp171->hd;struct Cyc_Absyn_Tqual _tmp173;void*_tmp174;struct _tuple18*_tmp172=_stmttmp13;_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;
if(_tmp173.real_const){
{const char*_tmp799;void*_tmp798[1];struct Cyc_String_pa_PrintArg_struct _tmp797;(_tmp797.tag=0,((_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp165->name)),((_tmp798[0]=& _tmp797,Cyc_Tcutil_terr(loc,((_tmp799="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp799,sizeof(char),64))),_tag_dyneither(_tmp798,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp174))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15F;if(_tmp166->tag != 8)goto _LLDA;else{_tmp167=(_tmp166->f1).elt_type;_tmp168=(_tmp166->f1).tq;}}_LLD9:
# 756
 if(_tmp168.real_const){
{const char*_tmp79C;void*_tmp79B;(_tmp79B=0,Cyc_Tcutil_terr(loc,((_tmp79C="attempt to over-write a const array",_tag_dyneither(_tmp79C,sizeof(char),36))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp167);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15F;if(_tmp169->tag != 10)goto _LLDC;else{_tmp16A=_tmp169->f1;}}_LLDB:
# 762
 for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct _tuple18*_stmttmp12=(struct _tuple18*)_tmp16A->hd;struct Cyc_Absyn_Tqual _tmp17B;void*_tmp17C;struct _tuple18*_tmp17A=_stmttmp12;_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;
if(_tmp17B.real_const){
{const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_Tcutil_terr(loc,((_tmp79F="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp79F,sizeof(char),56))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}
return 0;}
# 768
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17C))return 0;}
# 770
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 778
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 781
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_stmttmp14=e->r;void*_tmp17F=_stmttmp14;struct Cyc_Absyn_Vardecl*_tmp182;struct Cyc_Absyn_Vardecl*_tmp185;struct Cyc_Absyn_Vardecl*_tmp188;struct Cyc_Absyn_Vardecl*_tmp18B;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18E;struct Cyc_Absyn_Exp*_tmp190;struct _dyneither_ptr*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp19A;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp180->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp181=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp180->f2);if(_tmp181->tag != 3)goto _LLE1;else{_tmp182=_tmp181->f1;}}}_LLE0:
 _tmp185=_tmp182;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp183->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp184=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp183->f2);if(_tmp184->tag != 4)goto _LLE3;else{_tmp185=_tmp184->f1;}}}_LLE2:
 _tmp188=_tmp185;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp186->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp187=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp186->f2);if(_tmp187->tag != 5)goto _LLE5;else{_tmp188=_tmp187->f1;}}}_LLE4:
 _tmp18B=_tmp188;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp189->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp18A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp189->f2);if(_tmp18A->tag != 1)goto _LLE7;else{_tmp18B=_tmp18A->f1;}}}_LLE6:
 if(!(_tmp18B->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18C->tag != 22)goto _LLE9;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LLE8:
# 788
{void*_stmttmp19=Cyc_Tcutil_compress((void*)_check_null(_tmp18D->topt));void*_tmp19B=_stmttmp19;struct Cyc_Absyn_Tqual _tmp19D;struct Cyc_Absyn_Tqual _tmp19F;struct Cyc_List_List*_tmp1A1;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19B;if(_tmp19C->tag != 5)goto _LLF8;else{_tmp19D=(_tmp19C->f1).elt_tq;}}_LLF7:
 _tmp19F=_tmp19D;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19B;if(_tmp19E->tag != 8)goto _LLFA;else{_tmp19F=(_tmp19E->f1).tq;}}_LLF9:
 if(!_tmp19F.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp19B;if(_tmp1A0->tag != 10)goto _LLFC;else{_tmp1A1=_tmp1A0->f1;}}_LLFB: {
# 792
struct _tuple15 _stmttmp1A=Cyc_Evexp_eval_const_uint_exp(_tmp18E);unsigned int _tmp1A3;int _tmp1A4;struct _tuple15 _tmp1A2=_stmttmp1A;_tmp1A3=_tmp1A2.f1;_tmp1A4=_tmp1A2.f2;
if(!_tmp1A4){
{const char*_tmp7A2;void*_tmp7A1;(_tmp7A1=0,Cyc_Tcutil_terr(e->loc,((_tmp7A2="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7A2,sizeof(char),47))),_tag_dyneither(_tmp7A1,sizeof(void*),0)));}
return;}
# 797
{struct _handler_cons _tmp1A7;_push_handler(& _tmp1A7);{int _tmp1A9=0;if(setjmp(_tmp1A7.handler))_tmp1A9=1;if(!_tmp1A9){
{struct _tuple18*_stmttmp1B=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1A1,(int)_tmp1A3);struct Cyc_Absyn_Tqual _tmp1AB;struct _tuple18*_tmp1AA=_stmttmp1B;_tmp1AB=_tmp1AA->f1;
if(!_tmp1AB.real_const){_npop_handler(0);return;}}
# 798
;_pop_handler();}else{void*_tmp1A8=(void*)_exn_thrown;void*_tmp1AD=_tmp1A8;void*_tmp1AF;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1AE=(struct Cyc_List_Nth_exn_struct*)_tmp1AD;if(_tmp1AE->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 800
 return;_LL101: _tmp1AF=_tmp1AD;_LL102:(void)_throw(_tmp1AF);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 804
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp18F->tag != 20)goto _LLEB;else{_tmp190=_tmp18F->f1;_tmp191=_tmp18F->f2;}}_LLEA:
# 806
{void*_stmttmp18=Cyc_Tcutil_compress((void*)_check_null(_tmp190->topt));void*_tmp1B0=_stmttmp18;struct Cyc_Absyn_Aggrdecl**_tmp1B2;struct Cyc_List_List*_tmp1B4;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1B1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B1->tag != 11)goto _LL106;else{if((((_tmp1B1->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1B2=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1B1->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 808
struct Cyc_Absyn_Aggrfield*sf=
_tmp1B2 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1B2,_tmp191);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1B3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B0;if(_tmp1B3->tag != 12)goto _LL108;else{_tmp1B4=_tmp1B3->f2;}}_LL107: {
# 813
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1B4,_tmp191);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 818
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp192->tag != 21)goto _LLED;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LLEC:
# 820
{void*_stmttmp16=Cyc_Tcutil_compress((void*)_check_null(_tmp193->topt));void*_tmp1B5=_stmttmp16;void*_tmp1B7;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B5;if(_tmp1B6->tag != 5)goto _LL10D;else{_tmp1B7=(_tmp1B6->f1).elt_typ;}}_LL10C:
# 822
{void*_stmttmp17=Cyc_Tcutil_compress(_tmp1B7);void*_tmp1B8=_stmttmp17;struct Cyc_Absyn_Aggrdecl**_tmp1BA;struct Cyc_List_List*_tmp1BC;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1B9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B8;if(_tmp1B9->tag != 11)goto _LL112;else{if((((_tmp1B9->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1BA=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1B9->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 824
struct Cyc_Absyn_Aggrfield*sf=
_tmp1BA == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1BA,_tmp194);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B8;if(_tmp1BB->tag != 12)goto _LL114;else{_tmp1BC=_tmp1BB->f2;}}_LL113: {
# 829
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1BC,_tmp194);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 834
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 837
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp195->tag != 19)goto _LLEF;else{_tmp196=_tmp195->f1;}}_LLEE:
# 839
{void*_stmttmp15=Cyc_Tcutil_compress((void*)_check_null(_tmp196->topt));void*_tmp1BD=_stmttmp15;struct Cyc_Absyn_Tqual _tmp1BF;struct Cyc_Absyn_Tqual _tmp1C1;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BD;if(_tmp1BE->tag != 5)goto _LL119;else{_tmp1BF=(_tmp1BE->f1).elt_tq;}}_LL118:
 _tmp1C1=_tmp1BF;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BD;if(_tmp1C0->tag != 8)goto _LL11B;else{_tmp1C1=(_tmp1C0->f1).tq;}}_LL11A:
 if(!_tmp1C1.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 844
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp197->tag != 11)goto _LLF1;else{_tmp198=_tmp197->f1;}}_LLF0:
 _tmp19A=_tmp198;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17F;if(_tmp199->tag != 12)goto _LLF3;else{_tmp19A=_tmp199->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp19A);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 849
const char*_tmp7A6;void*_tmp7A5[1];struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7A5[0]=& _tmp7A4,Cyc_Tcutil_terr(e->loc,((_tmp7A6="attempt to write a const location: %s",_tag_dyneither(_tmp7A6,sizeof(char),38))),_tag_dyneither(_tmp7A5,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1C5=_new_region("temp");struct _RegionHandle*temp=& _tmp1C5;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp7A9;void*_tmp7A8;void*_tmp1C8=(_tmp7A8=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7A9="increment/decrement of non-lvalue",_tag_dyneither(_tmp7A9,sizeof(char),34))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));_npop_handler(0);return _tmp1C8;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_Tcutil_terr(e->loc,((_tmp7AC="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7AC,sizeof(char),50))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp7AF;void*_tmp7AE;(_tmp7AE=0,Cyc_Tcutil_terr(e->loc,((_tmp7AF="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7AF,sizeof(char),54))),_tag_dyneither(_tmp7AE,sizeof(void*),0)));}}else{
# 872
const char*_tmp7B3;void*_tmp7B2[1];struct Cyc_String_pa_PrintArg_struct _tmp7B1;(_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7B2[0]=& _tmp7B1,Cyc_Tcutil_terr(e->loc,((_tmp7B3="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7B3,sizeof(char),44))),_tag_dyneither(_tmp7B2,sizeof(void*),1)))))));}}{
# 874
void*_tmp1D0=t;_npop_handler(0);return _tmp1D0;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1D1=_new_region("r");struct _RegionHandle*r=& _tmp1D1;_push_region(r);{
const char*_tmp7B4;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp7B4="conditional expression",_tag_dyneither(_tmp7B4,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp7B5;struct Cyc_List_List _tmp1D3=(_tmp7B5.hd=e3,((_tmp7B5.tl=0,_tmp7B5)));
struct Cyc_List_List _tmp7B6;struct Cyc_List_List _tmp1D4=(_tmp7B6.hd=e2,((_tmp7B6.tl=& _tmp1D3,_tmp7B6)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1D4)){
{const char*_tmp7BB;void*_tmp7BA[2];struct Cyc_String_pa_PrintArg_struct _tmp7B9;struct Cyc_String_pa_PrintArg_struct _tmp7B8;(_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7BA[0]=& _tmp7B9,((_tmp7BA[1]=& _tmp7B8,Cyc_Tcutil_terr(loc,((_tmp7BB="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7BB,sizeof(char),48))),_tag_dyneither(_tmp7BA,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1D9=t;_npop_handler(0);return _tmp1D9;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp7BC;Cyc_Tcexp_tcTest(te,e1,((_tmp7BC="logical-and expression",_tag_dyneither(_tmp7BC,sizeof(char),23))));}
{const char*_tmp7BD;Cyc_Tcexp_tcTest(te,e2,((_tmp7BD="logical-and expression",_tag_dyneither(_tmp7BD,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp7BE;Cyc_Tcexp_tcTest(te,e1,((_tmp7BE="logical-or expression",_tag_dyneither(_tmp7BE,sizeof(char),22))));}
{const char*_tmp7BF;Cyc_Tcexp_tcTest(te,e2,((_tmp7BF="logical-or expression",_tag_dyneither(_tmp7BF,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 917
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 924
struct _RegionHandle _tmp1E0=_new_region("r");struct _RegionHandle*r=& _tmp1E0;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 927
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 931
{void*_stmttmp1C=Cyc_Tcutil_compress(t1);void*_tmp1E1=_stmttmp1C;_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1E1;if(_tmp1E2->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp7C2;void*_tmp7C1;(_tmp7C1=0,Cyc_Tcutil_terr(loc,((_tmp7C2="cannot assign to an array",_tag_dyneither(_tmp7C2,sizeof(char),26))),_tag_dyneither(_tmp7C1,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7C5;void*_tmp7C4;(_tmp7C4=0,Cyc_Tcutil_terr(loc,((_tmp7C5="type is abstract (can't determine size).",_tag_dyneither(_tmp7C5,sizeof(char),41))),_tag_dyneither(_tmp7C4,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp7C8;void*_tmp7C7;void*_tmp1E9=(_tmp7C7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C8="assignment to non-lvalue",_tag_dyneither(_tmp7C8,sizeof(char),25))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));_npop_handler(0);return _tmp1E9;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1EA=_new_region("temp");struct _RegionHandle*temp=& _tmp1EA;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp7CB;void*_tmp7CA;(_tmp7CA=0,Cyc_Tcutil_terr(e2->loc,((_tmp7CB="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7CB,sizeof(char),49))),_tag_dyneither(_tmp7CA,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7D0;void*_tmp7CF[2];struct Cyc_String_pa_PrintArg_struct _tmp7CE;struct Cyc_String_pa_PrintArg_struct _tmp7CD;void*_tmp1ED=(_tmp7CD.tag=0,((_tmp7CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7CE.tag=0,((_tmp7CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7CF[0]=& _tmp7CE,((_tmp7CF[1]=& _tmp7CD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D0="type mismatch: %s != %s",_tag_dyneither(_tmp7D0,sizeof(char),24))),_tag_dyneither(_tmp7CF,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1EE=_tmp1ED;_npop_handler(1);return _tmp1EE;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp1F3=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1F4=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1F3,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1F4,t1) || Cyc_Tcutil_coerceable(_tmp1F4) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7D5;void*_tmp7D4[2];struct Cyc_String_pa_PrintArg_struct _tmp7D3;struct Cyc_String_pa_PrintArg_struct _tmp7D2;void*_tmp1F5=
(_tmp7D2.tag=0,((_tmp7D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp7D3.tag=0,((_tmp7D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp7D4[0]=& _tmp7D3,((_tmp7D4[1]=& _tmp7D2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D5="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7D5,sizeof(char),82))),_tag_dyneither(_tmp7D4,sizeof(void*),2)))))))))))));
# 963
Cyc_Tcutil_unify(_tmp1F4,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp1F6=_tmp1F5;_npop_handler(0);return _tmp1F6;};}{
# 967
void*_tmp1FB=_tmp1F4;_npop_handler(0);return _tmp1FB;};}{
# 969
void*_tmp1FC=t1;_npop_handler(0);return _tmp1FC;};};};
# 924
;_pop_region(r);}
# 973
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp1FD=_new_region("r");struct _RegionHandle*r=& _tmp1FD;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 978
void*_tmp1FE=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp1FE;};
# 974
;_pop_region(r);}
# 982
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 986
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 989
{void*_stmttmp1D=Cyc_Tcutil_compress(t1);void*_tmp1FF=_stmttmp1D;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1FF;if(_tmp200->tag != 7)goto _LL125;else{if(_tmp200->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp201=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FF;if(_tmp201->tag != 6)goto _LL127;else{if(_tmp201->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FF;if(_tmp202->tag != 6)goto _LL129;else{if(_tmp202->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _stmttmp1E=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp204;struct Cyc_List_List*_tmp205;unsigned int _tmp206;enum Cyc_Absyn_Scope _tmp207;struct Cyc_Absyn_Datatypefield _tmp203=_stmttmp1E;_tmp204=_tmp203.name;_tmp205=_tmp203.typs;_tmp206=_tmp203.loc;_tmp207=_tmp203.sc;
# 999
if(_tmp205 == 0  || _tmp205->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp205->hd)).f2);
# 1002
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1007
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _stmttmp1F=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp209;struct Cyc_List_List*_tmp20A;unsigned int _tmp20B;enum Cyc_Absyn_Scope _tmp20C;struct Cyc_Absyn_Datatypefield _tmp208=_stmttmp1F;_tmp209=_tmp208.name;_tmp20A=_tmp208.typs;_tmp20B=_tmp208.loc;_tmp20C=_tmp208.sc;
# 1010
if(_tmp20A == 0  || _tmp20A->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp20A->hd)).f2);
# 1014
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1019
{const char*_tmp7DA;void*_tmp7D9[2];struct Cyc_String_pa_PrintArg_struct _tmp7D8;struct Cyc_String_pa_PrintArg_struct _tmp7D7;(_tmp7D7.tag=0,((_tmp7D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7D8.tag=0,((_tmp7D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7D9[0]=& _tmp7D8,((_tmp7D9[1]=& _tmp7D7,Cyc_Tcutil_terr(e->loc,((_tmp7DA="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7DA,sizeof(char),54))),_tag_dyneither(_tmp7D9,sizeof(void*),2)))))))))))));}
return 0;}
# 1025
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1031
struct Cyc_List_List*_tmp211=args;
int _tmp212=0;
struct _RegionHandle _tmp213=_new_region("ter");struct _RegionHandle*ter=& _tmp213;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp214=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp215=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp214);
Cyc_Tcexp_tcExp(_tmp215,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1041
void*_tmp216=t;void*_tmp218;struct Cyc_Absyn_Tqual _tmp219;void*_tmp21A;union Cyc_Absyn_Constraint*_tmp21B;union Cyc_Absyn_Constraint*_tmp21C;union Cyc_Absyn_Constraint*_tmp21D;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 5)goto _LL12E;else{_tmp218=(_tmp217->f1).elt_typ;_tmp219=(_tmp217->f1).elt_tq;_tmp21A=((_tmp217->f1).ptr_atts).rgn;_tmp21B=((_tmp217->f1).ptr_atts).nullable;_tmp21C=((_tmp217->f1).ptr_atts).bounds;_tmp21D=((_tmp217->f1).ptr_atts).zero_term;}}_LL12D:
# 1046
 Cyc_Tcenv_check_rgn_accessible(_tmp215,loc,_tmp21A);
# 1048
Cyc_Tcutil_check_nonzero_bound(loc,_tmp21C);{
void*_stmttmp20=Cyc_Tcutil_compress(_tmp218);void*_tmp21E=_stmttmp20;struct Cyc_List_List*_tmp220;void*_tmp221;struct Cyc_Absyn_Tqual _tmp222;void*_tmp223;struct Cyc_List_List*_tmp224;int _tmp225;struct Cyc_Absyn_VarargInfo*_tmp226;struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp228;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21E;if(_tmp21F->tag != 9)goto _LL133;else{_tmp220=(_tmp21F->f1).tvars;_tmp221=(_tmp21F->f1).effect;_tmp222=(_tmp21F->f1).ret_tqual;_tmp223=(_tmp21F->f1).ret_typ;_tmp224=(_tmp21F->f1).args;_tmp225=(_tmp21F->f1).c_varargs;_tmp226=(_tmp21F->f1).cyc_varargs;_tmp227=(_tmp21F->f1).rgn_po;_tmp228=(_tmp21F->f1).attributes;}}_LL132:
# 1052
 if(_tmp220 != 0  || _tmp227 != 0){
const char*_tmp7DD;void*_tmp7DC;(_tmp7DC=0,Cyc_Tcutil_terr(e->loc,((_tmp7DD="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp7DD,sizeof(char),79))),_tag_dyneither(_tmp7DC,sizeof(void*),0)));}
# 1056
if(topt != 0)Cyc_Tcutil_unify(_tmp223,*topt);
# 1058
while(_tmp211 != 0  && _tmp224 != 0){
# 1060
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp211->hd;
void*t2=(*((struct _tuple9*)_tmp224->hd)).f3;
Cyc_Tcexp_tcExp(_tmp215,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp215,e1,t2,& alias_coercion)){
const char*_tmp7DE;struct _dyneither_ptr s0=(_tmp7DE="actual argument has type ",_tag_dyneither(_tmp7DE,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp7DF;struct _dyneither_ptr s2=(_tmp7DF=" but formal has type ",_tag_dyneither(_tmp7DF,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp7ED;struct Cyc_String_pa_PrintArg_struct _tmp7EC;struct Cyc_String_pa_PrintArg_struct _tmp7EB;struct Cyc_String_pa_PrintArg_struct _tmp7EA;void*_tmp7E9[4];const char*_tmp7E8;void*_tmp7E7;(_tmp7E7=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp7ED.tag=0,((_tmp7ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp7EC.tag=0,((_tmp7EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp7EB.tag=0,((_tmp7EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp7EA.tag=0,((_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp7E9[0]=& _tmp7EA,((_tmp7E9[1]=& _tmp7EB,((_tmp7E9[2]=& _tmp7EC,((_tmp7E9[3]=& _tmp7ED,Cyc_aprintf(((_tmp7E8="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp7E8,sizeof(char),15))),_tag_dyneither(_tmp7E9,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));}else{
# 1072
struct Cyc_String_pa_PrintArg_struct _tmp7FB;struct Cyc_String_pa_PrintArg_struct _tmp7FA;struct Cyc_String_pa_PrintArg_struct _tmp7F9;struct Cyc_String_pa_PrintArg_struct _tmp7F8;void*_tmp7F7[4];const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp7FB.tag=0,((_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp7F8.tag=0,((_tmp7F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp7F7[0]=& _tmp7F8,((_tmp7F7[1]=& _tmp7F9,((_tmp7F7[2]=& _tmp7FA,((_tmp7F7[3]=& _tmp7FB,Cyc_aprintf(((_tmp7F6="%s%s%s%s.",_tag_dyneither(_tmp7F6,sizeof(char),10))),_tag_dyneither(_tmp7F7,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1077
if(alias_coercion){
struct Cyc_List_List*_tmp7FC;*alias_arg_exps=((_tmp7FC=_cycalloc(sizeof(*_tmp7FC)),((_tmp7FC->hd=(void*)_tmp212,((_tmp7FC->tl=*alias_arg_exps,_tmp7FC))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7FF;void*_tmp7FE;(_tmp7FE=0,Cyc_Tcutil_terr(e1->loc,((_tmp7FF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7FF,sizeof(char),49))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}
_tmp211=_tmp211->tl;
_tmp224=_tmp224->tl;
++ _tmp212;}{
# 1088
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp228;for(0;a != 0;a=a->tl){
void*_stmttmp21=(void*)a->hd;void*_tmp23E=_stmttmp21;enum Cyc_Absyn_Format_Type _tmp240;int _tmp241;int _tmp242;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp23F=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp23E;if(_tmp23F->tag != 19)goto _LL138;else{_tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;_tmp242=_tmp23F->f3;}}_LL137:
# 1092
{struct _handler_cons _tmp243;_push_handler(& _tmp243);{int _tmp245=0;if(setjmp(_tmp243.handler))_tmp245=1;if(!_tmp245){
# 1094
{struct Cyc_Absyn_Exp*_tmp246=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp241 - 1);
# 1096
struct Cyc_Core_Opt*fmt_args;
if(_tmp242 == 0)
fmt_args=0;else{
# 1100
struct Cyc_Core_Opt*_tmp800;fmt_args=((_tmp800=_cycalloc(sizeof(*_tmp800)),((_tmp800->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp242 - 1),_tmp800))));}
args_already_checked=1;{
struct _RegionHandle _tmp248=_new_region("temp");struct _RegionHandle*temp=& _tmp248;_push_region(temp);
switch(_tmp240){case Cyc_Absyn_Printf_ft: _LL13A:
# 1105
 Cyc_Tcexp_check_format_args(_tmp215,_tmp246,fmt_args,_tmp242 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1108
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1110
 Cyc_Tcexp_check_format_args(_tmp215,_tmp246,fmt_args,_tmp242 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1113
break;}
# 1103
;_pop_region(temp);};}
# 1094
;_pop_handler();}else{void*_tmp244=(void*)_exn_thrown;void*_tmp24A=_tmp244;void*_tmp24C;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp24B=(struct Cyc_List_Nth_exn_struct*)_tmp24A;if(_tmp24B->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1117
{const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(loc,((_tmp803="bad format arguments",_tag_dyneither(_tmp803,sizeof(char),21))),_tag_dyneither(_tmp802,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp24C=_tmp24A;_LL141:(void)_throw(_tmp24C);_LL13D:;}};}
# 1119
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1123
if(_tmp224 != 0){const char*_tmp806;void*_tmp805;(_tmp805=0,Cyc_Tcutil_terr(loc,((_tmp806="too few arguments for function",_tag_dyneither(_tmp806,sizeof(char),31))),_tag_dyneither(_tmp805,sizeof(void*),0)));}else{
# 1125
if((_tmp211 != 0  || _tmp225) || _tmp226 != 0){
if(_tmp225)
for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
Cyc_Tcexp_tcExp(_tmp215,0,(struct Cyc_Absyn_Exp*)_tmp211->hd);}else{
if(_tmp226 == 0){
const char*_tmp809;void*_tmp808;(_tmp808=0,Cyc_Tcutil_terr(loc,((_tmp809="too many arguments for function",_tag_dyneither(_tmp809,sizeof(char),32))),_tag_dyneither(_tmp808,sizeof(void*),0)));}else{
# 1132
struct Cyc_Absyn_VarargInfo _stmttmp22=*_tmp226;void*_tmp254;int _tmp255;struct Cyc_Absyn_VarargInfo _tmp253=_stmttmp22;_tmp254=_tmp253.type;_tmp255=_tmp253.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp80A;struct Cyc_Absyn_VarargCallInfo*_tmp256=(_tmp80A=_cycalloc(sizeof(*_tmp80A)),((_tmp80A->num_varargs=0,((_tmp80A->injectors=0,((_tmp80A->vai=_tmp226,_tmp80A)))))));
# 1136
*vararg_call_info=_tmp256;
# 1138
if(!_tmp255)
# 1140
for(0;_tmp211 != 0;(_tmp211=_tmp211->tl,_tmp212 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp211->hd;
++ _tmp256->num_varargs;
Cyc_Tcexp_tcExp(_tmp215,& _tmp254,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp215,e1,_tmp254,& alias_coercion)){
{const char*_tmp80F;void*_tmp80E[2];struct Cyc_String_pa_PrintArg_struct _tmp80D;struct Cyc_String_pa_PrintArg_struct _tmp80C;(_tmp80C.tag=0,((_tmp80C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp80D.tag=0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp254)),((_tmp80E[0]=& _tmp80D,((_tmp80E[1]=& _tmp80C,Cyc_Tcutil_terr(loc,((_tmp80F="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp80F,sizeof(char),49))),_tag_dyneither(_tmp80E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1150
if(alias_coercion){
struct Cyc_List_List*_tmp810;*alias_arg_exps=((_tmp810=_cycalloc(sizeof(*_tmp810)),((_tmp810->hd=(void*)_tmp212,((_tmp810->tl=*alias_arg_exps,_tmp810))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp254) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp813;void*_tmp812;(_tmp812=0,Cyc_Tcutil_terr(e1->loc,((_tmp813="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp813,sizeof(char),49))),_tag_dyneither(_tmp812,sizeof(void*),0)));}}else{
# 1159
void*_stmttmp23=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp254));void*_tmp25E=_stmttmp23;struct Cyc_Absyn_Datatypedecl*_tmp260;struct Cyc_List_List*_tmp261;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp25E;if(_tmp25F->tag != 3)goto _LL145;else{if((((_tmp25F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp260=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp25F->f1).datatype_info).KnownDatatype).val);_tmp261=(_tmp25F->f1).targs;}}_LL144: {
# 1163
struct Cyc_Absyn_Datatypedecl*_tmp262=*Cyc_Tcenv_lookup_datatypedecl(_tmp215,loc,_tmp260->name);
struct Cyc_List_List*fields=0;
if(_tmp262->fields == 0){
const char*_tmp817;void*_tmp816[1];struct Cyc_String_pa_PrintArg_struct _tmp815;(_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp254)),((_tmp816[0]=& _tmp815,Cyc_Tcutil_terr(loc,((_tmp817="can't inject into abstract datatype %s",_tag_dyneither(_tmp817,sizeof(char),39))),_tag_dyneither(_tmp816,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262->fields))->v;}
# 1175
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp254),Cyc_Tcenv_curr_rgn(_tmp215))){
const char*_tmp81C;void*_tmp81B[2];struct Cyc_String_pa_PrintArg_struct _tmp81A;struct Cyc_String_pa_PrintArg_struct _tmp819;(_tmp819.tag=0,((_tmp819.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp215))),((_tmp81A.tag=0,((_tmp81A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp254)),((_tmp81B[0]=& _tmp81A,((_tmp81B[1]=& _tmp819,Cyc_Tcutil_terr(loc,((_tmp81C="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp81C,sizeof(char),49))),_tag_dyneither(_tmp81B,sizeof(void*),2)))))))))))));}{
# 1179
struct _RegionHandle _tmp26A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp26A;_push_region(rgn);{
struct Cyc_List_List*_tmp26B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp262->tvs,_tmp261);
for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
++ _tmp256->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp211->hd;
# 1185
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp215,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp81F;void*_tmp81E;(_tmp81E=0,Cyc_Tcutil_terr(e1->loc,((_tmp81F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp81F,sizeof(char),49))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}}{
# 1191
struct Cyc_Absyn_Datatypefield*_tmp26E=Cyc_Tcexp_tcInjection(_tmp215,e1,Cyc_Tcutil_pointer_elt_type(_tmp254),rgn,_tmp26B,fields);
# 1193
if(_tmp26E != 0){
struct Cyc_List_List*_tmp820;_tmp256->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp256->injectors,(
(_tmp820=_cycalloc(sizeof(*_tmp820)),((_tmp820->hd=_tmp26E,((_tmp820->tl=0,_tmp820)))))));}};};}}
# 1199
_npop_handler(0);goto _LL142;
# 1179
;_pop_region(rgn);};}_LL145:;_LL146:
# 1200
{const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(loc,((_tmp823="bad inject vararg type",_tag_dyneither(_tmp823,sizeof(char),23))),_tag_dyneither(_tmp822,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1205
if(*alias_arg_exps == 0)
# 1214 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp215,loc,(void*)_check_null(_tmp221));{
# 1216
void*_tmp273=_tmp223;_npop_handler(0);return _tmp273;};};_LL133:;_LL134: {
const char*_tmp827;void*_tmp826[1];struct Cyc_String_pa_PrintArg_struct _tmp825;void*_tmp277=(_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp826[0]=& _tmp825,Cyc_Tcexp_expr_err(_tmp215,loc,topt,((_tmp827="expected pointer to function but found %s",_tag_dyneither(_tmp827,sizeof(char),42))),_tag_dyneither(_tmp826,sizeof(void*),1)))))));_npop_handler(0);return _tmp277;}_LL130:;};_LL12E:;_LL12F: {
# 1219
const char*_tmp82B;void*_tmp82A[1];struct Cyc_String_pa_PrintArg_struct _tmp829;void*_tmp27B=(_tmp829.tag=0,((_tmp829.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp82A[0]=& _tmp829,Cyc_Tcexp_expr_err(_tmp215,loc,topt,((_tmp82B="expected pointer to function but found %s",_tag_dyneither(_tmp82B,sizeof(char),42))),_tag_dyneither(_tmp82A,sizeof(void*),1)))))));_npop_handler(0);return _tmp27B;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1225 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp27C=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp27C,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp27C,& bogus)){
const char*_tmp830;void*_tmp82F[2];struct Cyc_String_pa_PrintArg_struct _tmp82E;struct Cyc_String_pa_PrintArg_struct _tmp82D;(_tmp82D.tag=0,((_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp82E.tag=0,((_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1230
Cyc_Absynpp_typ2string(_tmp27C)),((_tmp82F[0]=& _tmp82E,((_tmp82F[1]=& _tmp82D,Cyc_Tcutil_terr(loc,((_tmp830="expected %s but found %s",_tag_dyneither(_tmp830,sizeof(char),25))),_tag_dyneither(_tmp82F,sizeof(void*),2)))))))))))));}
# 1232
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1237
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1239
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1241
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp281=t1;void*_tmp283;struct Cyc_Absyn_Tqual _tmp284;void*_tmp285;union Cyc_Absyn_Constraint*_tmp286;union Cyc_Absyn_Constraint*_tmp287;union Cyc_Absyn_Constraint*_tmp288;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp282=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp281;if(_tmp282->tag != 5)goto _LL14A;else{_tmp283=(_tmp282->f1).elt_typ;_tmp284=(_tmp282->f1).elt_tq;_tmp285=((_tmp282->f1).ptr_atts).rgn;_tmp286=((_tmp282->f1).ptr_atts).nullable;_tmp287=((_tmp282->f1).ptr_atts).bounds;_tmp288=((_tmp282->f1).ptr_atts).zero_term;}}_LL149:
# 1245
{void*_stmttmp24=Cyc_Tcutil_compress(_tmp283);void*_tmp289=_stmttmp24;struct Cyc_List_List*_tmp28B;void*_tmp28C;struct Cyc_Absyn_Tqual _tmp28D;void*_tmp28E;struct Cyc_List_List*_tmp28F;int _tmp290;struct Cyc_Absyn_VarargInfo*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp293;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp28A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp289;if(_tmp28A->tag != 9)goto _LL14F;else{_tmp28B=(_tmp28A->f1).tvars;_tmp28C=(_tmp28A->f1).effect;_tmp28D=(_tmp28A->f1).ret_tqual;_tmp28E=(_tmp28A->f1).ret_typ;_tmp28F=(_tmp28A->f1).args;_tmp290=(_tmp28A->f1).c_varargs;_tmp291=(_tmp28A->f1).cyc_varargs;_tmp292=(_tmp28A->f1).rgn_po;_tmp293=(_tmp28A->f1).attributes;}}_LL14E: {
# 1247
struct _RegionHandle _tmp294=_new_region("temp");struct _RegionHandle*temp=& _tmp294;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1250
for(0;ts != 0  && _tmp28B != 0;(ts=ts->tl,_tmp28B=_tmp28B->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp28B->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp833;struct Cyc_List_List*_tmp832;instantiation=(
(_tmp832=_region_malloc(temp,sizeof(*_tmp832)),((_tmp832->hd=((_tmp833=_region_malloc(temp,sizeof(*_tmp833)),((_tmp833->f1=(struct Cyc_Absyn_Tvar*)_tmp28B->hd,((_tmp833->f2=(void*)ts->hd,_tmp833)))))),((_tmp832->tl=instantiation,_tmp832))))));};}
# 1257
if(ts != 0){
const char*_tmp836;void*_tmp835;void*_tmp299=
(_tmp835=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp836="too many type variables in instantiation",_tag_dyneither(_tmp836,sizeof(char),41))),_tag_dyneither(_tmp835,sizeof(void*),0)));_npop_handler(0);return _tmp299;}
# 1263
if(_tmp28B == 0){
_tmp292=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp292);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp292);
_tmp292=0;}{
# 1268
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp83C;struct Cyc_Absyn_FnInfo _tmp83B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp83A;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83C.tag=9,((_tmp83C.f1=((_tmp83B.tvars=_tmp28B,((_tmp83B.effect=_tmp28C,((_tmp83B.ret_tqual=_tmp28D,((_tmp83B.ret_typ=_tmp28E,((_tmp83B.args=_tmp28F,((_tmp83B.c_varargs=_tmp290,((_tmp83B.cyc_varargs=_tmp291,((_tmp83B.rgn_po=_tmp292,((_tmp83B.attributes=_tmp293,_tmp83B)))))))))))))))))),_tmp83C)))),_tmp83A)))));
# 1272
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp842;struct Cyc_Absyn_PtrInfo _tmp841;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp840;void*_tmp29D=(void*)((_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((_tmp842.tag=5,((_tmp842.f1=((_tmp841.elt_typ=new_fn_typ,((_tmp841.elt_tq=_tmp284,((_tmp841.ptr_atts=(((_tmp841.ptr_atts).rgn=_tmp285,(((_tmp841.ptr_atts).nullable=_tmp286,(((_tmp841.ptr_atts).bounds=_tmp287,(((_tmp841.ptr_atts).zero_term=_tmp288,(((_tmp841.ptr_atts).ptrloc=0,_tmp841.ptr_atts)))))))))),_tmp841)))))),_tmp842)))),_tmp840))));_npop_handler(0);return _tmp29D;};}
# 1248
;_pop_region(temp);}_LL14F:;_LL150:
# 1274
 goto _LL14C;_LL14C:;}
# 1276
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1279
const char*_tmp846;void*_tmp845[1];struct Cyc_String_pa_PrintArg_struct _tmp844;return(_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp845[0]=& _tmp844,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp846="expecting polymorphic type but found %s",_tag_dyneither(_tmp846,sizeof(char),40))),_tag_dyneither(_tmp845,sizeof(void*),1)))))));};};}
# 1284
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1286
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1290
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1295
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1302
Cyc_Tcutil_unify(t2,t);{
const char*_tmp84B;void*_tmp84A[2];struct Cyc_String_pa_PrintArg_struct _tmp849;struct Cyc_String_pa_PrintArg_struct _tmp848;void*_tmp2A4=(_tmp848.tag=0,((_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp84A[0]=& _tmp849,((_tmp84A[1]=& _tmp848,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp84B="cannot cast %s to %s",_tag_dyneither(_tmp84B,sizeof(char),21))),_tag_dyneither(_tmp84A,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2A4;};}}}
# 1312
{struct _tuple0 _tmp84C;struct _tuple0 _stmttmp25=(_tmp84C.f1=e->r,((_tmp84C.f2=Cyc_Tcutil_compress(t),_tmp84C)));struct _tuple0 _tmp2A9=_stmttmp25;int _tmp2AB;union Cyc_Absyn_Constraint*_tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2A9.f1;if(_tmp2AA->tag != 33)goto _LL154;else{_tmp2AB=(_tmp2AA->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A9.f2;if(_tmp2AC->tag != 5)goto _LL154;else{_tmp2AD=((_tmp2AC->f1).ptr_atts).nullable;_tmp2AE=((_tmp2AC->f1).ptr_atts).bounds;_tmp2AF=((_tmp2AC->f1).ptr_atts).zero_term;}};_LL153:
# 1316
 if((_tmp2AB  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AF)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AD)){
void*_stmttmp26=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2AE);void*_tmp2B0=_stmttmp26;struct Cyc_Absyn_Exp*_tmp2B2;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B0;if(_tmp2B1->tag != 1)goto _LL159;else{_tmp2B2=_tmp2B1->f1;}}_LL158:
# 1319
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2B2)).f1 == 1){
const char*_tmp84F;void*_tmp84E;(_tmp84E=0,Cyc_Tcutil_warn(loc,((_tmp84F="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp84F,sizeof(char),60))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1325
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1328
return t;};}
# 1332
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1334
void**_tmp2B6=0;
struct Cyc_Absyn_Tqual _tmp2B7=Cyc_Absyn_empty_tqual(0);
int _tmp2B8=0;
if(topt != 0){
void*_stmttmp27=Cyc_Tcutil_compress(*topt);void*_tmp2B9=_stmttmp27;void**_tmp2BB;struct Cyc_Absyn_Tqual _tmp2BC;union Cyc_Absyn_Constraint*_tmp2BD;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2BA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9;if(_tmp2BA->tag != 5)goto _LL15E;else{_tmp2BB=(void**)&(_tmp2BA->f1).elt_typ;_tmp2BC=(_tmp2BA->f1).elt_tq;_tmp2BD=((_tmp2BA->f1).ptr_atts).nullable;}}_LL15D:
# 1340
 _tmp2B6=_tmp2BB;
_tmp2B7=_tmp2BC;
_tmp2B8=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2BD);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1354
struct _RegionHandle _tmp2BE=_new_region("r");struct _RegionHandle*r=& _tmp2BE;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp2B6,e);
# 1358
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp852;void*_tmp851;(_tmp851=0,Cyc_Tcutil_terr(e->loc,((_tmp852="Cannot take the address of an alias-free path",_tag_dyneither(_tmp852,sizeof(char),46))),_tag_dyneither(_tmp851,sizeof(void*),0)));}
# 1365
{void*_stmttmp28=e->r;void*_tmp2C1=_stmttmp28;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Exp*_tmp2C4;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2C2=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2C1;if(_tmp2C2->tag != 22)goto _LL163;else{_tmp2C3=_tmp2C2->f1;_tmp2C4=_tmp2C2->f2;}}_LL162:
# 1367
{void*_stmttmp29=Cyc_Tcutil_compress((void*)_check_null(_tmp2C3->topt));void*_tmp2C5=_stmttmp29;_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2C6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2C5;if(_tmp2C6->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1372
 e0->r=(Cyc_Absyn_add_exp(_tmp2C3,_tmp2C4,0))->r;{
void*_tmp2C7=Cyc_Tcexp_tcPlus(te,_tmp2C3,_tmp2C4);_npop_handler(0);return _tmp2C7;};_LL165:;}
# 1375
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1380
{void*_stmttmp2A=e->r;void*_tmp2C8=_stmttmp2A;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2C9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2C8;if(_tmp2C9->tag != 20)goto _LL16D;else{if(_tmp2C9->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2CA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2C8;if(_tmp2CA->tag != 21)goto _LL16F;else{if(_tmp2CA->f3 != 1)goto _LL16F;}}_LL16E:
# 1383
{const char*_tmp855;void*_tmp854;(_tmp854=0,Cyc_Tcutil_terr(e->loc,((_tmp855="cannot take the address of a @tagged union member",_tag_dyneither(_tmp855,sizeof(char),50))),_tag_dyneither(_tmp854,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1389
struct _tuple14 _stmttmp2B=Cyc_Tcutil_addressof_props(te,e);int _tmp2CE;void*_tmp2CF;struct _tuple14 _tmp2CD=_stmttmp2B;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;
# 1391
if(Cyc_Tcutil_is_noalias_region(_tmp2CF,0)){
const char*_tmp858;void*_tmp857;(_tmp857=0,Cyc_Tcutil_terr(e->loc,((_tmp858="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp858,sizeof(char),60))),_tag_dyneither(_tmp857,sizeof(void*),0)));}{
# 1394
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2CE){
tq.print_const=1;
tq.real_const=1;}{
# 1400
void*t=_tmp2B8?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2CF,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2CF,tq,Cyc_Absyn_false_conref);
void*_tmp2D2=t;_npop_handler(0);return _tmp2D2;};};};
# 1354
;_pop_region(r);};}
# 1407
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1411
return Cyc_Absyn_uint_typ;
# 1413
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp85C;void*_tmp85B[1];struct Cyc_String_pa_PrintArg_struct _tmp85A;(_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp85B[0]=& _tmp85A,Cyc_Tcutil_terr(loc,((_tmp85C="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp85C,sizeof(char),55))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))));}
if(topt != 0){
void*_stmttmp2C=Cyc_Tcutil_compress(*topt);void*_tmp2D6=_stmttmp2C;void*_tmp2D7;void*_tmp2D9;_LL172: _tmp2D7=_tmp2D6;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D7;if(_tmp2D8->tag != 19)goto _LL174;else{_tmp2D9=(void*)_tmp2D8->f1;}};_LL173: {
# 1422
struct Cyc_Absyn_Exp*_tmp2DA=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp85F;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp85E;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2DB=(_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp85F.tag=18,((_tmp85F.f1=_tmp2DA,_tmp85F)))),_tmp85E)));
if(Cyc_Tcutil_unify(_tmp2D9,(void*)_tmp2DB))return _tmp2D7;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1428
return Cyc_Absyn_uint_typ;}
# 1431
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1438
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1440
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp2DE=fs;
void*_tmp2DF=t_orig;
for(0;_tmp2DE != 0;_tmp2DE=_tmp2DE->tl){
void*_tmp2E0=(void*)_tmp2DE->hd;
void*_tmp2E1=_tmp2E0;struct _dyneither_ptr*_tmp2E3;unsigned int _tmp2E5;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2E2=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2E1;if(_tmp2E2->tag != 0)goto _LL179;else{_tmp2E3=_tmp2E2->f1;}}_LL178: {
# 1448
int bad_type=1;
{void*_stmttmp2E=Cyc_Tcutil_compress(_tmp2DF);void*_tmp2E6=_stmttmp2E;struct Cyc_Absyn_Aggrdecl**_tmp2E8;struct Cyc_List_List*_tmp2EA;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2E7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E7->tag != 11)goto _LL17E;else{if((((_tmp2E7->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2E8=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2E7->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1451
 if((*_tmp2E8)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2E3,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2E8)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_PrintArg_struct _tmp861;(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E3),((_tmp862[0]=& _tmp861,Cyc_Tcutil_terr(loc,((_tmp863="no field of struct/union has name %s",_tag_dyneither(_tmp863,sizeof(char),37))),_tag_dyneither(_tmp862,sizeof(void*),1)))))));}else{
# 1456
_tmp2DF=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E9->tag != 12)goto _LL180;else{_tmp2EA=_tmp2E9->f2;}}_LL17F: {
# 1460
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2E3,_tmp2EA);
if(!((unsigned int)t2)){
const char*_tmp867;void*_tmp866[1];struct Cyc_String_pa_PrintArg_struct _tmp865;(_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E3),((_tmp866[0]=& _tmp865,Cyc_Tcutil_terr(loc,((_tmp867="no field of struct/union has name %s",_tag_dyneither(_tmp867,sizeof(char),37))),_tag_dyneither(_tmp866,sizeof(void*),1)))))));}else{
# 1464
_tmp2DF=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1469
if(bad_type){
if(_tmp2DE == fs){
const char*_tmp86B;void*_tmp86A[1];struct Cyc_String_pa_PrintArg_struct _tmp869;(_tmp869.tag=0,((_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp86A[0]=& _tmp869,Cyc_Tcutil_terr(loc,((_tmp86B="%s is not a known struct/union type",_tag_dyneither(_tmp86B,sizeof(char),36))),_tag_dyneither(_tmp86A,sizeof(void*),1)))))));}else{
# 1473
const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_PrintArg_struct _tmp86D;struct _dyneither_ptr _tmp2F4=(_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp86E[0]=& _tmp86D,Cyc_aprintf(((_tmp86F="(%s)",_tag_dyneither(_tmp86F,sizeof(char),5))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2DE;x=x->tl){
void*_stmttmp2F=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp2F5=_stmttmp2F;struct _dyneither_ptr*_tmp2F7;unsigned int _tmp2F9;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2F6=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto _LL185;else{_tmp2F7=_tmp2F6->f1;}}_LL184:
# 1478
{const char*_tmp874;void*_tmp873[2];struct Cyc_String_pa_PrintArg_struct _tmp872;struct Cyc_String_pa_PrintArg_struct _tmp871;_tmp2F4=((_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F7),((_tmp872.tag=0,((_tmp872.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp873[0]=& _tmp872,((_tmp873[1]=& _tmp871,Cyc_aprintf(((_tmp874="%s.%s",_tag_dyneither(_tmp874,sizeof(char),6))),_tag_dyneither(_tmp873,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2F8=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F8->tag != 1)goto _LL182;else{_tmp2F9=_tmp2F8->f1;}}_LL186:
# 1480
{const char*_tmp879;void*_tmp878[2];struct Cyc_String_pa_PrintArg_struct _tmp877;struct Cyc_Int_pa_PrintArg_struct _tmp876;_tmp2F4=((_tmp876.tag=1,((_tmp876.f1=(unsigned long)((int)_tmp2F9),((_tmp877.tag=0,((_tmp877.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp878[0]=& _tmp877,((_tmp878[1]=& _tmp876,Cyc_aprintf(((_tmp879="%s.%d",_tag_dyneither(_tmp879,sizeof(char),6))),_tag_dyneither(_tmp878,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1482
const char*_tmp87E;void*_tmp87D[2];struct Cyc_String_pa_PrintArg_struct _tmp87C;struct Cyc_String_pa_PrintArg_struct _tmp87B;(_tmp87B.tag=0,((_tmp87B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp87C.tag=0,((_tmp87C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp87D[0]=& _tmp87C,((_tmp87D[1]=& _tmp87B,Cyc_Tcutil_terr(loc,((_tmp87E="%s == %s is not a struct/union type",_tag_dyneither(_tmp87E,sizeof(char),36))),_tag_dyneither(_tmp87D,sizeof(void*),2)))))))))))));};}}
# 1485
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2E4=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2E1;if(_tmp2E4->tag != 1)goto _LL176;else{_tmp2E5=_tmp2E4->f1;}}_LL17A: {
# 1487
int bad_type=1;
{void*_stmttmp2D=Cyc_Tcutil_compress(_tmp2DF);void*_tmp309=_stmttmp2D;struct Cyc_Absyn_Aggrdecl**_tmp30B;struct Cyc_List_List*_tmp30D;struct Cyc_List_List*_tmp30F;struct Cyc_Absyn_Datatypefield*_tmp311;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp309;if(_tmp30A->tag != 11)goto _LL18A;else{if((((_tmp30A->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp30B=(struct Cyc_Absyn_Aggrdecl**)(((_tmp30A->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1490
 if((*_tmp30B)->impl == 0)
goto _LL187;
_tmp30D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp30B)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp30C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp309;if(_tmp30C->tag != 12)goto _LL18C;else{_tmp30D=_tmp30C->f2;}}_LL18B:
# 1494
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D)<= _tmp2E5){
const char*_tmp883;void*_tmp882[2];struct Cyc_Int_pa_PrintArg_struct _tmp881;struct Cyc_Int_pa_PrintArg_struct _tmp880;(_tmp880.tag=1,((_tmp880.f1=(unsigned long)((int)_tmp2E5),((_tmp881.tag=1,((_tmp881.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D),((_tmp882[0]=& _tmp881,((_tmp882[1]=& _tmp880,Cyc_Tcutil_terr(loc,((_tmp883="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp883,sizeof(char),46))),_tag_dyneither(_tmp882,sizeof(void*),2)))))))))))));}else{
# 1498
_tmp2DF=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp30D,(int)_tmp2E5))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp30E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp309;if(_tmp30E->tag != 10)goto _LL18E;else{_tmp30F=_tmp30E->f1;}}_LL18D:
# 1502
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F)<= _tmp2E5){
const char*_tmp888;void*_tmp887[2];struct Cyc_Int_pa_PrintArg_struct _tmp886;struct Cyc_Int_pa_PrintArg_struct _tmp885;(_tmp885.tag=1,((_tmp885.f1=(unsigned long)((int)_tmp2E5),((_tmp886.tag=1,((_tmp886.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F),((_tmp887[0]=& _tmp886,((_tmp887[1]=& _tmp885,Cyc_Tcutil_terr(loc,((_tmp888="tuple has too few components: %d <= %d",_tag_dyneither(_tmp888,sizeof(char),39))),_tag_dyneither(_tmp887,sizeof(void*),2)))))))))))));}else{
# 1506
_tmp2DF=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp30F,(int)_tmp2E5)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp309;if(_tmp310->tag != 4)goto _LL190;else{if((((_tmp310->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp311=((struct _tuple2)(((_tmp310->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp311->typs)< _tmp2E5){
const char*_tmp88D;void*_tmp88C[2];struct Cyc_Int_pa_PrintArg_struct _tmp88B;struct Cyc_Int_pa_PrintArg_struct _tmp88A;(_tmp88A.tag=1,((_tmp88A.f1=(unsigned long)((int)_tmp2E5),((_tmp88B.tag=1,((_tmp88B.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp311->typs),((_tmp88C[0]=& _tmp88B,((_tmp88C[1]=& _tmp88A,Cyc_Tcutil_terr(loc,((_tmp88D="datatype field has too few components: %d < %d",_tag_dyneither(_tmp88D,sizeof(char),47))),_tag_dyneither(_tmp88C,sizeof(void*),2)))))))))))));}else{
# 1514
if(_tmp2E5 != 0)
_tmp2DF=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp311->typs,(int)(_tmp2E5 - 1))).f2;else{
if(_tmp2DE->tl != 0){
const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(loc,((_tmp890="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp890,sizeof(char),68))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}}}
# 1519
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1523
if(bad_type){
const char*_tmp894;void*_tmp893[1];struct Cyc_String_pa_PrintArg_struct _tmp892;(_tmp892.tag=0,((_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp893[0]=& _tmp892,Cyc_Tcutil_terr(loc,((_tmp894="%s is not a known type",_tag_dyneither(_tmp894,sizeof(char),23))),_tag_dyneither(_tmp893,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1528
return Cyc_Absyn_uint_typ;};}
# 1532
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp323=_new_region("r");struct _RegionHandle*r=& _tmp323;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp324=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp324,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp325=t;void*_tmp328;void*_tmp329;union Cyc_Absyn_Constraint*_tmp32A;union Cyc_Absyn_Constraint*_tmp32B;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 1)goto _LL195;}_LL194: {
# 1539
struct Cyc_List_List*_tmp32C=Cyc_Tcenv_lookup_type_vars(_tmp324);
struct Cyc_Core_Opt*_tmp895;void*_tmp32D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895->v=_tmp32C,_tmp895)))));
struct Cyc_Core_Opt*_tmp896;void*_tmp32E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896->v=_tmp32C,_tmp896)))));
union Cyc_Absyn_Constraint*_tmp32F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp330=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp897;struct Cyc_Absyn_PtrAtts _tmp331=(_tmp897.rgn=_tmp32E,((_tmp897.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp897.bounds=_tmp32F,((_tmp897.zero_term=_tmp330,((_tmp897.ptrloc=0,_tmp897)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp89D;struct Cyc_Absyn_PtrInfo _tmp89C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp89B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp332=(_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((_tmp89D.tag=5,((_tmp89D.f1=((_tmp89C.elt_typ=_tmp32D,((_tmp89C.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp89C.ptr_atts=_tmp331,_tmp89C)))))),_tmp89D)))),_tmp89B)));
Cyc_Tcutil_unify(t,(void*)_tmp332);
_tmp328=_tmp32D;_tmp329=_tmp32E;_tmp32A=_tmp32F;_tmp32B=_tmp330;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325;if(_tmp327->tag != 5)goto _LL197;else{_tmp328=(_tmp327->f1).elt_typ;_tmp329=((_tmp327->f1).ptr_atts).rgn;_tmp32A=((_tmp327->f1).ptr_atts).bounds;_tmp32B=((_tmp327->f1).ptr_atts).zero_term;}}_LL196:
# 1549
 Cyc_Tcenv_check_rgn_accessible(_tmp324,loc,_tmp329);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp32A);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp328),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp324)){
const char*_tmp8A0;void*_tmp89F;(_tmp89F=0,Cyc_Tcutil_terr(loc,((_tmp8A0="can't dereference abstract pointer type",_tag_dyneither(_tmp8A0,sizeof(char),40))),_tag_dyneither(_tmp89F,sizeof(void*),0)));}{
void*_tmp33B=_tmp328;_npop_handler(0);return _tmp33B;};_LL197:;_LL198: {
# 1555
const char*_tmp8A4;void*_tmp8A3[1];struct Cyc_String_pa_PrintArg_struct _tmp8A2;void*_tmp33F=(_tmp8A2.tag=0,((_tmp8A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8A3[0]=& _tmp8A2,Cyc_Tcexp_expr_err(_tmp324,loc,topt,((_tmp8A4="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8A4,sizeof(char),39))),_tag_dyneither(_tmp8A3,sizeof(void*),1)))))));_npop_handler(0);return _tmp33F;}_LL192:;};}
# 1534
;_pop_region(r);}
# 1561
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1565
struct _RegionHandle _tmp340=_new_region("r");struct _RegionHandle*r=& _tmp340;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1569
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_stmttmp30=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp341=_stmttmp30;struct Cyc_Absyn_Aggrdecl*_tmp343;struct Cyc_List_List*_tmp344;enum Cyc_Absyn_AggrKind _tmp346;struct Cyc_List_List*_tmp347;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp341;if(_tmp342->tag != 11)goto _LL19C;else{if((((_tmp342->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp343=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp342->f1).aggr_info).KnownAggr).val);_tmp344=(_tmp342->f1).targs;}}_LL19B: {
# 1572
struct Cyc_Absyn_Aggrfield*_tmp348=Cyc_Absyn_lookup_decl_field(_tmp343,f);
if(_tmp348 == 0){
const char*_tmp8A9;void*_tmp8A8[2];struct Cyc_String_pa_PrintArg_struct _tmp8A7;struct Cyc_String_pa_PrintArg_struct _tmp8A6;void*_tmp34D=(_tmp8A6.tag=0,((_tmp8A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A7.tag=0,((_tmp8A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp343->name)),((_tmp8A8[0]=& _tmp8A7,((_tmp8A8[1]=& _tmp8A6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A9="%s has no %s member",_tag_dyneither(_tmp8A9,sizeof(char),20))),_tag_dyneither(_tmp8A8,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp34D;}
# 1577
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp343->impl))->tagged)*is_tagged=1;{
void*t2=_tmp348->type;
if(_tmp344 != 0){
struct _RegionHandle _tmp34E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp34E;_push_region(rgn);
{struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp343->tvs,_tmp344);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp34F,_tmp348->type);}
# 1581
;_pop_region(rgn);}
# 1587
if((((_tmp343->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp343->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp348->requires_clause == 0){
const char*_tmp8AD;void*_tmp8AC[1];struct Cyc_String_pa_PrintArg_struct _tmp8AB;void*_tmp353=(_tmp8AB.tag=0,((_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AC[0]=& _tmp8AB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AD="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8AD,sizeof(char),56))),_tag_dyneither(_tmp8AC,sizeof(void*),1)))))));_npop_handler(0);return _tmp353;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp343->impl))->exist_vars != 0){
# 1592
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8B1;void*_tmp8B0[1];struct Cyc_String_pa_PrintArg_struct _tmp8AF;void*_tmp357=(_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B0[0]=& _tmp8AF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B1="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8B1,sizeof(char),81))),_tag_dyneither(_tmp8B0,sizeof(void*),1)))))));_npop_handler(0);return _tmp357;}}{
# 1595
void*_tmp358=t2;_npop_handler(0);return _tmp358;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341;if(_tmp345->tag != 12)goto _LL19E;else{_tmp346=_tmp345->f1;_tmp347=_tmp345->f2;}}_LL19D: {
# 1597
struct Cyc_Absyn_Aggrfield*_tmp359=Cyc_Absyn_lookup_field(_tmp347,f);
if(_tmp359 == 0){
const char*_tmp8B5;void*_tmp8B4[1];struct Cyc_String_pa_PrintArg_struct _tmp8B3;void*_tmp35D=(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B4[0]=& _tmp8B3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B5="type has no %s member",_tag_dyneither(_tmp8B5,sizeof(char),22))),_tag_dyneither(_tmp8B4,sizeof(void*),1)))))));_npop_handler(0);return _tmp35D;}
# 1602
if(((_tmp346 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp359->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp359->requires_clause == 0){
# 1604
const char*_tmp8B9;void*_tmp8B8[1];struct Cyc_String_pa_PrintArg_struct _tmp8B7;void*_tmp361=(_tmp8B7.tag=0,((_tmp8B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B8[0]=& _tmp8B7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B9="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8B9,sizeof(char),56))),_tag_dyneither(_tmp8B8,sizeof(void*),1)))))));_npop_handler(0);return _tmp361;}{
void*_tmp362=_tmp359->type;_npop_handler(0);return _tmp362;};}_LL19E:;_LL19F: {
# 1607
const char*_tmp8BD;void*_tmp8BC[1];struct Cyc_String_pa_PrintArg_struct _tmp8BB;void*_tmp366=(_tmp8BB.tag=0,((_tmp8BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8BC[0]=& _tmp8BB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BD="expecting struct or union, found %s",_tag_dyneither(_tmp8BD,sizeof(char),36))),_tag_dyneither(_tmp8BC,sizeof(void*),1)))))));_npop_handler(0);return _tmp366;}_LL199:;};
# 1565
;_pop_region(r);}
# 1613
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1616
struct _RegionHandle _tmp367=_new_region("r");struct _RegionHandle*r=& _tmp367;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1621
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_stmttmp31=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp368=_stmttmp31;void*_tmp36A;void*_tmp36B;union Cyc_Absyn_Constraint*_tmp36C;union Cyc_Absyn_Constraint*_tmp36D;_LL1A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368;if(_tmp369->tag != 5)goto _LL1A3;else{_tmp36A=(_tmp369->f1).elt_typ;_tmp36B=((_tmp369->f1).ptr_atts).rgn;_tmp36C=((_tmp369->f1).ptr_atts).bounds;_tmp36D=((_tmp369->f1).ptr_atts).zero_term;}}_LL1A2:
# 1624
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp36C);
{void*_stmttmp32=Cyc_Tcutil_compress(_tmp36A);void*_tmp36E=_stmttmp32;struct Cyc_Absyn_Aggrdecl*_tmp370;struct Cyc_List_List*_tmp371;enum Cyc_Absyn_AggrKind _tmp373;struct Cyc_List_List*_tmp374;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36E;if(_tmp36F->tag != 11)goto _LL1A8;else{if((((_tmp36F->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A8;_tmp370=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp36F->f1).aggr_info).KnownAggr).val);_tmp371=(_tmp36F->f1).targs;}}_LL1A7: {
# 1627
struct Cyc_Absyn_Aggrfield*_tmp375=Cyc_Absyn_lookup_decl_field(_tmp370,f);
if(_tmp375 == 0){
const char*_tmp8C2;void*_tmp8C1[2];struct Cyc_String_pa_PrintArg_struct _tmp8C0;struct Cyc_String_pa_PrintArg_struct _tmp8BF;void*_tmp37A=(_tmp8BF.tag=0,((_tmp8BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8C0.tag=0,((_tmp8C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp370->name)),((_tmp8C1[0]=& _tmp8C0,((_tmp8C1[1]=& _tmp8BF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C2="type %s has no %s member",_tag_dyneither(_tmp8C2,sizeof(char),25))),_tag_dyneither(_tmp8C1,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp37A;}
# 1632
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->tagged)*is_tagged=1;{
void*t3=_tmp375->type;
if(_tmp371 != 0){
struct _RegionHandle _tmp37B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp37B;_push_region(rgn);
{struct Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp370->tvs,_tmp371);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp37C,_tmp375->type);}
# 1636
;_pop_region(rgn);}{
# 1640
struct Cyc_Absyn_Kind*_tmp37D=Cyc_Tcutil_typ_kind(t3);
# 1643
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp37D) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_stmttmp33=Cyc_Tcutil_compress(t3);void*_tmp37E=_stmttmp33;_LL1AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E;if(_tmp37F->tag != 8)goto _LL1AF;}_LL1AE:
 goto _LL1AC;_LL1AF:;_LL1B0: {
# 1647
const char*_tmp8C6;void*_tmp8C5[1];struct Cyc_String_pa_PrintArg_struct _tmp8C4;void*_tmp383=(_tmp8C4.tag=0,((_tmp8C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8C5[0]=& _tmp8C4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C6="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8C6,sizeof(char),48))),_tag_dyneither(_tmp8C5,sizeof(void*),1)))))));_npop_handler(0);return _tmp383;}_LL1AC:;}
# 1652
if((((_tmp370->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp375->requires_clause == 0){
# 1655
const char*_tmp8CA;void*_tmp8C9[1];struct Cyc_String_pa_PrintArg_struct _tmp8C8;void*_tmp387=(_tmp8C8.tag=0,((_tmp8C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8C9[0]=& _tmp8C8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CA="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8CA,sizeof(char),56))),_tag_dyneither(_tmp8C9,sizeof(void*),1)))))));_npop_handler(0);return _tmp387;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8CE;void*_tmp8CD[1];struct Cyc_String_pa_PrintArg_struct _tmp8CC;void*_tmp38B=(_tmp8CC.tag=0,((_tmp8CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8CD[0]=& _tmp8CC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CE="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8CE,sizeof(char),72))),_tag_dyneither(_tmp8CD,sizeof(void*),1)))))));_npop_handler(0);return _tmp38B;}}{
# 1662
void*_tmp38C=t3;_npop_handler(0);return _tmp38C;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36E;if(_tmp372->tag != 12)goto _LL1AA;else{_tmp373=_tmp372->f1;_tmp374=_tmp372->f2;}}_LL1A9: {
# 1664
struct Cyc_Absyn_Aggrfield*_tmp38D=Cyc_Absyn_lookup_field(_tmp374,f);
if(_tmp38D == 0){
const char*_tmp8D2;void*_tmp8D1[1];struct Cyc_String_pa_PrintArg_struct _tmp8D0;void*_tmp391=(_tmp8D0.tag=0,((_tmp8D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D1[0]=& _tmp8D0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D2="type has no %s field",_tag_dyneither(_tmp8D2,sizeof(char),21))),_tag_dyneither(_tmp8D1,sizeof(void*),1)))))));_npop_handler(0);return _tmp391;}
# 1669
if((_tmp373 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp38D->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_PrintArg_struct _tmp8D4;void*_tmp395=(_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D5[0]=& _tmp8D4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8D6,sizeof(char),56))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))));_npop_handler(0);return _tmp395;}{
void*_tmp396=_tmp38D->type;_npop_handler(0);return _tmp396;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1674
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1677
const char*_tmp8DA;void*_tmp8D9[1];struct Cyc_String_pa_PrintArg_struct _tmp8D8;void*_tmp39A=(_tmp8D8.tag=0,((_tmp8D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8D9[0]=& _tmp8D8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DA="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8DA,sizeof(char),44))),_tag_dyneither(_tmp8D9,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;};
# 1616
;_pop_region(r);}
# 1682
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1684
struct _tuple15 _stmttmp34=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp39C;int _tmp39D;struct _tuple15 _tmp39B=_stmttmp34;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;
if(!_tmp39D){
const char*_tmp8DD;void*_tmp8DC;return(_tmp8DC=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8DD="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8DD,sizeof(char),47))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}{
# 1688
struct _handler_cons _tmp3A0;_push_handler(& _tmp3A0);{int _tmp3A2=0;if(setjmp(_tmp3A0.handler))_tmp3A2=1;if(!_tmp3A2){
{void*_tmp3A3=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp39C)).f2;_npop_handler(0);return _tmp3A3;};_pop_handler();}else{void*_tmp3A1=(void*)_exn_thrown;void*_tmp3A5=_tmp3A1;void*_tmp3A7;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3A6=(struct Cyc_List_Nth_exn_struct*)_tmp3A5;if(_tmp3A6->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1691
const char*_tmp8E2;void*_tmp8E1[2];struct Cyc_Int_pa_PrintArg_struct _tmp8E0;struct Cyc_Int_pa_PrintArg_struct _tmp8DF;return(_tmp8DF.tag=1,((_tmp8DF.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8E0.tag=1,((_tmp8E0.f1=(unsigned long)((int)_tmp39C),((_tmp8E1[0]=& _tmp8E0,((_tmp8E1[1]=& _tmp8DF,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8E2="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8E2,sizeof(char),41))),_tag_dyneither(_tmp8E1,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3A7=_tmp3A5;_LL1B5:(void)_throw(_tmp3A7);_LL1B1:;}};};}
# 1696
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1698
struct _RegionHandle _tmp3AC=_new_region("r");struct _RegionHandle*r=& _tmp3AC;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3AD=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3AD),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3AD),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3AD,e2)){
const char*_tmp8E6;void*_tmp8E5[1];struct Cyc_String_pa_PrintArg_struct _tmp8E4;void*_tmp3B1=(_tmp8E4.tag=0,((_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8E5[0]=& _tmp8E4,Cyc_Tcexp_expr_err(_tmp3AD,e2->loc,topt,((_tmp8E6="expecting int subscript, found %s",_tag_dyneither(_tmp8E6,sizeof(char),34))),_tag_dyneither(_tmp8E5,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B1;}{
# 1709
void*_tmp3B2=t1;void*_tmp3B4;struct Cyc_Absyn_Tqual _tmp3B5;void*_tmp3B6;union Cyc_Absyn_Constraint*_tmp3B7;union Cyc_Absyn_Constraint*_tmp3B8;struct Cyc_List_List*_tmp3BA;_LL1B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 5)goto _LL1B9;else{_tmp3B4=(_tmp3B3->f1).elt_typ;_tmp3B5=(_tmp3B3->f1).elt_tq;_tmp3B6=((_tmp3B3->f1).ptr_atts).rgn;_tmp3B7=((_tmp3B3->f1).ptr_atts).bounds;_tmp3B8=((_tmp3B3->f1).ptr_atts).zero_term;}}_LL1B8:
# 1713
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B8)){
int emit_warning=1;
{void*_stmttmp35=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3B7);void*_tmp3BB=_stmttmp35;struct Cyc_Absyn_Exp*_tmp3BD;_LL1BE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3BC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3BB;if(_tmp3BC->tag != 1)goto _LL1C0;else{_tmp3BD=_tmp3BC->f1;}}_LL1BF:
# 1717
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp36=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3C0;int _tmp3C1;struct _tuple15 _tmp3BF=_stmttmp36;_tmp3C0=_tmp3BF.f1;_tmp3C1=_tmp3BF.f2;
if(_tmp3C1){
struct _tuple15 _stmttmp37=Cyc_Evexp_eval_const_uint_exp(_tmp3BD);unsigned int _tmp3C3;int _tmp3C4;struct _tuple15 _tmp3C2=_stmttmp37;_tmp3C3=_tmp3C2.f1;_tmp3C4=_tmp3C2.f2;
if(_tmp3C4  && _tmp3C3 > _tmp3C0)emit_warning=0;}}
# 1724
goto _LL1BD;_LL1C0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3BE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3BB;if(_tmp3BE->tag != 0)goto _LL1BD;}_LL1C1:
 emit_warning=0;goto _LL1BD;_LL1BD:;}
# 1727
if(emit_warning){
const char*_tmp8E9;void*_tmp8E8;(_tmp8E8=0,Cyc_Tcutil_warn(e2->loc,((_tmp8E9="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8E9,sizeof(char),63))),_tag_dyneither(_tmp8E8,sizeof(void*),0)));}}else{
# 1731
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp38=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3C8;int _tmp3C9;struct _tuple15 _tmp3C7=_stmttmp38;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;
if(_tmp3C9)
Cyc_Tcutil_check_bound(loc,_tmp3C8,_tmp3B7);}else{
# 1737
if(Cyc_Tcutil_is_bound_one(_tmp3B7) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B8)){
const char*_tmp8EC;void*_tmp8EB;(_tmp8EB=0,Cyc_Tcutil_warn(e1->loc,((_tmp8EC="subscript applied to pointer to one object",_tag_dyneither(_tmp8EC,sizeof(char),43))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3B7);}}
# 1742
Cyc_Tcenv_check_rgn_accessible(_tmp3AD,loc,_tmp3B6);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3B4),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3AD)){
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,Cyc_Tcutil_terr(e1->loc,((_tmp8EF="can't subscript an abstract pointer",_tag_dyneither(_tmp8EF,sizeof(char),36))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}{
void*_tmp3CE=_tmp3B4;_npop_handler(0);return _tmp3CE;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B2;if(_tmp3B9->tag != 10)goto _LL1BB;else{_tmp3BA=_tmp3B9->f1;}}_LL1BA: {
void*_tmp3CF=Cyc_Tcexp_ithTupleType(_tmp3AD,loc,_tmp3BA,e2);_npop_handler(0);return _tmp3CF;}_LL1BB:;_LL1BC: {
const char*_tmp8F3;void*_tmp8F2[1];struct Cyc_String_pa_PrintArg_struct _tmp8F1;void*_tmp3D3=(_tmp8F1.tag=0,((_tmp8F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8F2[0]=& _tmp8F1,Cyc_Tcexp_expr_err(_tmp3AD,loc,topt,((_tmp8F3="subscript applied to %s",_tag_dyneither(_tmp8F3,sizeof(char),24))),_tag_dyneither(_tmp8F2,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D3;}_LL1B6:;};};}
# 1699
;_pop_region(r);}
# 1753
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_stmttmp39=Cyc_Tcutil_compress(*topt);void*_tmp3D4=_stmttmp39;struct Cyc_List_List*_tmp3D6;_LL1C3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3D5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3D4;if(_tmp3D5->tag != 10)goto _LL1C5;else{_tmp3D6=_tmp3D5->f1;}}_LL1C4:
# 1759
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3D6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1762
goto _LL1C2;
# 1764
for(0;es != 0;(es=es->tl,_tmp3D6=_tmp3D6->tl)){
struct _RegionHandle _tmp3D7=_new_region("r");struct _RegionHandle*r=& _tmp3D7;_push_region(r);
{int bogus=0;
void*_tmp3D8=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp3D6))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp3D8,(struct Cyc_Absyn_Exp*)es->hd);
# 1770
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp3D6->hd)).f2,& bogus);{
struct _tuple18*_tmp8F6;struct Cyc_List_List*_tmp8F5;fields=((_tmp8F5=_cycalloc(sizeof(*_tmp8F5)),((_tmp8F5->hd=((_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->f1=(*((struct _tuple18*)_tmp3D6->hd)).f1,((_tmp8F6->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8F6)))))),((_tmp8F5->tl=fields,_tmp8F5))))));};}
# 1766
;_pop_region(r);}
# 1774
done=1;
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}
# 1778
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp3DB=_new_region("r");struct _RegionHandle*r=& _tmp3DB;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp8F9;struct Cyc_List_List*_tmp8F8;fields=((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->hd=((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9->f1=Cyc_Absyn_empty_tqual(0),((_tmp8F9->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8F9)))))),((_tmp8F8->tl=fields,_tmp8F8))))));};
# 1781
;_pop_region(r);}{
# 1785
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8FC;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8FB;return(void*)((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=((_tmp8FC.tag=10,((_tmp8FC.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8FC)))),_tmp8FB))));};}
# 1789
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1792
const char*_tmp8FF;void*_tmp8FE;return(_tmp8FE=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FF="tcCompoundLit",_tag_dyneither(_tmp8FF,sizeof(char),14))),_tag_dyneither(_tmp8FE,sizeof(void*),0)));}
# 1802 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1805
void*res_t2;
struct _RegionHandle _tmp3E2=_new_region("r");struct _RegionHandle*r=& _tmp3E2;_push_region(r);{
int _tmp3E3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp902;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp901;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3E4=(_tmp901=_cycalloc(sizeof(*_tmp901)),((_tmp901[0]=((_tmp902.tag=0,((_tmp902.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3E3),_tmp902)))),_tmp901)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3E4,loc);
# 1814
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3E5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3E3 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3E5)){
const char*_tmp905;void*_tmp904;(_tmp904=0,Cyc_Tcutil_terr(_tmp3E5->loc,((_tmp905="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp905,sizeof(char),45))),_tag_dyneither(_tmp904,sizeof(void*),0)));}}
# 1819
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1824
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1827
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1829
const char*_tmp909;void*_tmp908[1];struct Cyc_String_pa_PrintArg_struct _tmp907;(_tmp907.tag=0,((_tmp907.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp908[0]=& _tmp907,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp909="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp909,sizeof(char),53))),_tag_dyneither(_tmp908,sizeof(void*),1)))))));}}
# 1833
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1838
void*_stmttmp3A=(void*)ds->hd;void*_tmp3ED=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3F0;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3EE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3ED;if(_tmp3EE->tag != 1)goto _LL1CA;}_LL1C9:
# 1840
{const char*_tmp90C;void*_tmp90B;(_tmp90B=0,Cyc_Tcutil_terr(loc,((_tmp90C="only array index designators are supported",_tag_dyneither(_tmp90C,sizeof(char),43))),_tag_dyneither(_tmp90B,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3EF=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3ED;if(_tmp3EF->tag != 0)goto _LL1C7;else{_tmp3F0=_tmp3EF->f1;}}_LL1CB:
# 1843
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3F0);{
struct _tuple15 _stmttmp3B=Cyc_Evexp_eval_const_uint_exp(_tmp3F0);unsigned int _tmp3F4;int _tmp3F5;struct _tuple15 _tmp3F3=_stmttmp3B;_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;
if(!_tmp3F5){
const char*_tmp90F;void*_tmp90E;(_tmp90E=0,Cyc_Tcutil_terr(_tmp3F0->loc,((_tmp90F="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp90F,sizeof(char),47))),_tag_dyneither(_tmp90E,sizeof(void*),0)));}else{
if(_tmp3F4 != offset){
const char*_tmp914;void*_tmp913[2];struct Cyc_Int_pa_PrintArg_struct _tmp912;struct Cyc_Int_pa_PrintArg_struct _tmp911;(_tmp911.tag=1,((_tmp911.f1=(unsigned long)((int)_tmp3F4),((_tmp912.tag=1,((_tmp912.f1=(unsigned long)offset,((_tmp913[0]=& _tmp912,((_tmp913[1]=& _tmp911,Cyc_Tcutil_terr(_tmp3F0->loc,((_tmp914="expecting index designator %d but found %d",_tag_dyneither(_tmp914,sizeof(char),43))),_tag_dyneither(_tmp913,sizeof(void*),2)))))))))))));}}
# 1850
goto _LL1C7;};_LL1C7:;}}}{
# 1854
void*_tmp3FC=res_t2;_npop_handler(0);return _tmp3FC;};
# 1806
;_pop_region(r);}
# 1858
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1861
Cyc_Tcexp_tcExp(te,0,bound);
{void*_stmttmp3C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3FD=_stmttmp3C;void*_tmp3FF;_LL1CD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3FD;if(_tmp3FE->tag != 19)goto _LL1CF;else{_tmp3FF=(void*)_tmp3FE->f1;}}_LL1CE:
# 1866
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp400=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3FF,0),0,Cyc_Absyn_No_coercion,0);
_tmp400->topt=bound->topt;
# 1871
bound=_tmp400;}
# 1873
goto _LL1CC;_LL1CF:;_LL1D0:
# 1875
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp918;void*_tmp917[1];struct Cyc_String_pa_PrintArg_struct _tmp916;(_tmp916.tag=0,((_tmp916.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp917[0]=& _tmp916,Cyc_Tcutil_terr(bound->loc,((_tmp918="expecting unsigned int, found %s",_tag_dyneither(_tmp918,sizeof(char),33))),_tag_dyneither(_tmp917,sizeof(void*),1)))))));}_LL1CC:;}
# 1880
if(!(vd->tq).real_const){
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp91B="comprehension index variable is not declared const!",_tag_dyneither(_tmp91B,sizeof(char),52))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}{
# 1883
struct _RegionHandle _tmp406=_new_region("r");struct _RegionHandle*r=& _tmp406;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1889
struct Cyc_Tcenv_Tenv*_tmp407=te2;
void**_tmp408=0;
struct Cyc_Absyn_Tqual*_tmp409=0;
union Cyc_Absyn_Constraint**_tmp40A=0;
# 1894
if(topt != 0){
void*_stmttmp3D=Cyc_Tcutil_compress(*topt);void*_tmp40B=_stmttmp3D;struct Cyc_Absyn_PtrInfo _tmp40D;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp410;struct Cyc_Absyn_Exp*_tmp411;union Cyc_Absyn_Constraint*_tmp412;_LL1D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B;if(_tmp40C->tag != 5)goto _LL1D4;else{_tmp40D=_tmp40C->f1;}}_LL1D3:
# 1897
{void**_tmp91C;_tmp408=((_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=_tmp40D.elt_typ,_tmp91C))));}
{struct Cyc_Absyn_Tqual*_tmp91D;_tmp409=((_tmp91D=_region_malloc(r,sizeof(*_tmp91D)),((_tmp91D[0]=_tmp40D.elt_tq,_tmp91D))));}
{union Cyc_Absyn_Constraint**_tmp91E;_tmp40A=((_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E[0]=(_tmp40D.ptr_atts).zero_term,_tmp91E))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40B;if(_tmp40E->tag != 8)goto _LL1D6;else{_tmp40F=(_tmp40E->f1).elt_type;_tmp410=(_tmp40E->f1).tq;_tmp411=(_tmp40E->f1).num_elts;_tmp412=(_tmp40E->f1).zero_term;}}_LL1D5:
# 1902
{void**_tmp91F;_tmp408=((_tmp91F=_region_malloc(r,sizeof(*_tmp91F)),((_tmp91F[0]=_tmp40F,_tmp91F))));}
{struct Cyc_Absyn_Tqual*_tmp920;_tmp409=((_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=_tmp410,_tmp920))));}
{union Cyc_Absyn_Constraint**_tmp921;_tmp40A=((_tmp921=_region_malloc(r,sizeof(*_tmp921)),((_tmp921[0]=_tmp412,_tmp921))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1907
 goto _LL1D1;_LL1D1:;}{
# 1910
void*t=Cyc_Tcexp_tcExp(_tmp407,_tmp408,body);
# 1912
struct _RegionHandle _tmp419=_new_region("temp");struct _RegionHandle*temp=& _tmp419;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp924;void*_tmp923;(_tmp923=0,Cyc_Tcutil_terr(body->loc,((_tmp924="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp924,sizeof(char),49))),_tag_dyneither(_tmp923,sizeof(void*),0)));}
# 1916
if(_tmp407->le == 0){
# 1918
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_Tcutil_terr(bound->loc,((_tmp927="bound is not constant",_tag_dyneither(_tmp927,sizeof(char),22))),_tag_dyneither(_tmp926,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp92A;void*_tmp929;(_tmp929=0,Cyc_Tcutil_terr(bound->loc,((_tmp92A="body is not constant",_tag_dyneither(_tmp92A,sizeof(char),21))),_tag_dyneither(_tmp929,sizeof(void*),0)));}}
# 1923
if(_tmp40A != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp40A)){
# 1926
struct Cyc_Absyn_Exp*_tmp420=Cyc_Absyn_uint_exp(1,0);_tmp420->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp420,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1930
struct _RegionHandle _tmp421=_new_region("r");struct _RegionHandle*r=& _tmp421;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp92D;void*_tmp92C;(_tmp92C=0,Cyc_Tcutil_terr(body->loc,((_tmp92D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp92D,sizeof(char),49))),_tag_dyneither(_tmp92C,sizeof(void*),0)));}{
# 1936
void*_tmp424=Cyc_Absyn_array_typ(t,_tmp409 == 0?Cyc_Absyn_empty_tqual(0):*_tmp409,bound,
_tmp40A == 0?Cyc_Absyn_false_conref:*_tmp40A,0);
void*_tmp425=_tmp424;_npop_handler(2);return _tmp425;};
# 1930
;_pop_region(r);};
# 1912
;_pop_region(temp);};};}
# 1884
;_pop_region(r);};}
# 1943
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1946
Cyc_Tcexp_tcExp(te,0,bound);
{void*_stmttmp3E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp426=_stmttmp3E;void*_tmp428;_LL1D9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp426;if(_tmp427->tag != 19)goto _LL1DB;else{_tmp428=(void*)_tmp427->f1;}}_LL1DA:
# 1951
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp428,0),0,Cyc_Absyn_No_coercion,0);
_tmp429->topt=bound->topt;
# 1956
bound=_tmp429;}
# 1958
goto _LL1D8;_LL1DB:;_LL1DC:
# 1960
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp931;void*_tmp930[1];struct Cyc_String_pa_PrintArg_struct _tmp92F;(_tmp92F.tag=0,((_tmp92F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp930[0]=& _tmp92F,Cyc_Tcutil_terr(bound->loc,((_tmp931="expecting unsigned int, found %s",_tag_dyneither(_tmp931,sizeof(char),33))),_tag_dyneither(_tmp930,sizeof(void*),1)))))));}_LL1D8:;}{
# 1965
struct _RegionHandle _tmp42D=_new_region("r");struct _RegionHandle*r=& _tmp42D;_push_region(r);
{void**_tmp42E=0;
struct Cyc_Absyn_Tqual*_tmp42F=0;
union Cyc_Absyn_Constraint**_tmp430=0;
# 1970
if(topt != 0){
void*_stmttmp3F=Cyc_Tcutil_compress(*topt);void*_tmp431=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp433;void*_tmp435;struct Cyc_Absyn_Tqual _tmp436;struct Cyc_Absyn_Exp*_tmp437;union Cyc_Absyn_Constraint*_tmp438;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp432=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431;if(_tmp432->tag != 5)goto _LL1E0;else{_tmp433=_tmp432->f1;}}_LL1DF:
# 1973
{void**_tmp932;_tmp42E=((_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=_tmp433.elt_typ,_tmp932))));}
{struct Cyc_Absyn_Tqual*_tmp933;_tmp42F=((_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933[0]=_tmp433.elt_tq,_tmp933))));}
{union Cyc_Absyn_Constraint**_tmp934;_tmp430=((_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=(_tmp433.ptr_atts).zero_term,_tmp934))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp431;if(_tmp434->tag != 8)goto _LL1E2;else{_tmp435=(_tmp434->f1).elt_type;_tmp436=(_tmp434->f1).tq;_tmp437=(_tmp434->f1).num_elts;_tmp438=(_tmp434->f1).zero_term;}}_LL1E1:
# 1978
{void**_tmp935;_tmp42E=((_tmp935=_region_malloc(r,sizeof(*_tmp935)),((_tmp935[0]=_tmp435,_tmp935))));}
{struct Cyc_Absyn_Tqual*_tmp936;_tmp42F=((_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936[0]=_tmp436,_tmp936))));}
{union Cyc_Absyn_Constraint**_tmp937;_tmp430=((_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937[0]=_tmp438,_tmp937))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1983
 goto _LL1DD;_LL1DD:;}
# 1986
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp42E != 0)Cyc_Tcutil_unify(*_tmp42E,t);
# 1989
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp93A;void*_tmp939;(_tmp939=0,Cyc_Tcutil_terr(bound->loc,((_tmp93A="bound is not constant",_tag_dyneither(_tmp93A,sizeof(char),22))),_tag_dyneither(_tmp939,sizeof(void*),0)));}}
# 1993
if(_tmp430 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp430)){
# 1996
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_uint_exp(1,0);_tmp441->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp441,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2000
const char*_tmp93D;void*_tmp93C;(_tmp93C=0,Cyc_Tcutil_terr(loc,((_tmp93D="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp93D,sizeof(char),74))),_tag_dyneither(_tmp93C,sizeof(void*),0)));};}{
# 2003
void*_tmp444=Cyc_Absyn_array_typ(t,_tmp42F == 0?Cyc_Absyn_empty_tqual(0):*_tmp42F,bound,
_tmp430 == 0?Cyc_Absyn_false_conref:*_tmp430,0);
void*_tmp445=_tmp444;_npop_handler(0);return _tmp445;};}
# 1966
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2019 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2023
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp93E;adptr=((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E[0]=ad,_tmp93E))));};}else{
# 2029
{struct _handler_cons _tmp447;_push_handler(& _tmp447);{int _tmp449=0;if(setjmp(_tmp447.handler))_tmp449=1;if(!_tmp449){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2029
;_pop_handler();}else{void*_tmp448=(void*)_exn_thrown;void*_tmp44B=_tmp448;void*_tmp44D;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp44C=(struct Cyc_Dict_Absent_exn_struct*)_tmp44B;if(_tmp44C->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2032
{const char*_tmp942;void*_tmp941[1];struct Cyc_String_pa_PrintArg_struct _tmp940;(_tmp940.tag=0,((_tmp940.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp941[0]=& _tmp940,Cyc_Tcutil_terr(loc,((_tmp942="unbound struct/union name %s",_tag_dyneither(_tmp942,sizeof(char),29))),_tag_dyneither(_tmp941,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp44D=_tmp44B;_LL1E8:(void)_throw(_tmp44D);_LL1E4:;}};}
# 2035
*ad_opt=ad;
*tn=ad->name;}
# 2038
if(ad->impl == 0){
{const char*_tmp94A;void*_tmp949[1];const char*_tmp948;const char*_tmp947;struct Cyc_String_pa_PrintArg_struct _tmp946;(_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp947="struct",_tag_dyneither(_tmp947,sizeof(char),7)):((_tmp948="union",_tag_dyneither(_tmp948,sizeof(char),6)))),((_tmp949[0]=& _tmp946,Cyc_Tcutil_terr(loc,((_tmp94A="can't construct abstract %s",_tag_dyneither(_tmp94A,sizeof(char),28))),_tag_dyneither(_tmp949,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2042
struct _RegionHandle _tmp456=_new_region("rgn");struct _RegionHandle*rgn=& _tmp456;_push_region(rgn);
# 2047
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp457=Cyc_Tcenv_new_status(te);
if(_tmp457 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2056
te2=Cyc_Tcenv_set_new_status(rgn,_tmp457,te);}{
# 2058
struct _tuple12 _tmp94B;struct _tuple12 _tmp458=(_tmp94B.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp94B.f2=rgn,_tmp94B)));
struct Cyc_List_List*_tmp459=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp458,ad->tvs);
struct Cyc_List_List*_tmp45A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp458,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp45B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp459);
struct Cyc_List_List*_tmp45C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp45A);
struct Cyc_List_List*_tmp45D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp459,_tmp45A);
void*res_typ;
# 2069
if(topt != 0){
void*_stmttmp40=Cyc_Tcutil_compress(*topt);void*_tmp45E=_stmttmp40;struct Cyc_Absyn_Aggrdecl**_tmp460;struct Cyc_List_List*_tmp461;_LL1EA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp45F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp45E;if(_tmp45F->tag != 11)goto _LL1EC;else{if((((_tmp45F->f1).aggr_info).KnownAggr).tag != 2)goto _LL1EC;_tmp460=(struct Cyc_Absyn_Aggrdecl**)(((_tmp45F->f1).aggr_info).KnownAggr).val;_tmp461=(_tmp45F->f1).targs;}}_LL1EB:
# 2072
 if(*_tmp460 == *adptr){
{struct Cyc_List_List*_tmp462=_tmp45B;for(0;_tmp462 != 0  && _tmp461 != 0;
(_tmp462=_tmp462->tl,_tmp461=_tmp461->tl)){
Cyc_Tcutil_unify((void*)_tmp462->hd,(void*)_tmp461->hd);}}
# 2077
res_typ=*topt;
goto _LL1E9;}
# 2080
goto _LL1ED;_LL1EC:;_LL1ED: {
# 2082
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp951;struct Cyc_Absyn_AggrInfo _tmp950;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94F;res_typ=(void*)((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F[0]=((_tmp951.tag=11,((_tmp951.f1=((_tmp950.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp950.targs=_tmp45B,_tmp950)))),_tmp951)))),_tmp94F))));}_LL1E9:;}else{
# 2085
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp957;struct Cyc_Absyn_AggrInfo _tmp956;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp955;res_typ=(void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=((_tmp957.tag=11,((_tmp957.f1=((_tmp956.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp956.targs=_tmp45B,_tmp956)))),_tmp957)))),_tmp955))));}{
# 2088
struct Cyc_List_List*_tmp469=*ts;
struct Cyc_List_List*_tmp46A=_tmp45C;
while(_tmp469 != 0  && _tmp46A != 0){
# 2092
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp469->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp469->hd);
Cyc_Tcutil_unify((void*)_tmp469->hd,(void*)_tmp46A->hd);
_tmp469=_tmp469->tl;
_tmp46A=_tmp46A->tl;}
# 2098
if(_tmp469 != 0){
const char*_tmp95A;void*_tmp959;(_tmp959=0,Cyc_Tcutil_terr(loc,((_tmp95A="too many explicit witness types",_tag_dyneither(_tmp95A,sizeof(char),32))),_tag_dyneither(_tmp959,sizeof(void*),0)));}
# 2101
*ts=_tmp45C;{
# 2104
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp41=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp46E;struct Cyc_Absyn_Exp*_tmp46F;struct _tuple19*_tmp46D=_stmttmp41;_tmp46E=_tmp46D->f1;_tmp46F=_tmp46D->f2;{
void*_tmp470=Cyc_Tcutil_rsubstitute(rgn,_tmp45D,_tmp46E->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp470,_tmp46F);
# 2115
if(!Cyc_Tcutil_coerce_arg(te2,_tmp46F,_tmp470,& bogus)){
{const char*_tmp966;void*_tmp965[5];struct Cyc_String_pa_PrintArg_struct _tmp964;const char*_tmp963;const char*_tmp962;struct Cyc_String_pa_PrintArg_struct _tmp961;struct Cyc_String_pa_PrintArg_struct _tmp960;struct Cyc_String_pa_PrintArg_struct _tmp95F;struct Cyc_String_pa_PrintArg_struct _tmp95E;(_tmp95E.tag=0,((_tmp95E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2119
Cyc_Absynpp_typ2string((void*)_check_null(_tmp46F->topt))),((_tmp95F.tag=0,((_tmp95F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2118
Cyc_Absynpp_typ2string(_tmp470)),((_tmp960.tag=0,((_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp961.tag=0,((_tmp961.f1=(struct _dyneither_ptr)(
# 2117
ad->kind == Cyc_Absyn_StructA?(_tmp962="struct",_tag_dyneither(_tmp962,sizeof(char),7)):((_tmp963="union",_tag_dyneither(_tmp963,sizeof(char),6)))),((_tmp964.tag=0,((_tmp964.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46E->name),((_tmp965[0]=& _tmp964,((_tmp965[1]=& _tmp961,((_tmp965[2]=& _tmp960,((_tmp965[3]=& _tmp95F,((_tmp965[4]=& _tmp95E,Cyc_Tcutil_terr(_tmp46F->loc,((_tmp966="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp966,sizeof(char),40))),_tag_dyneither(_tmp965,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2120
Cyc_Tcutil_explain_failure();}};}{
# 2124
struct Cyc_List_List*_tmp47A=0;
{struct Cyc_List_List*_tmp47B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp47B != 0;_tmp47B=_tmp47B->tl){
struct _tuple0*_tmp969;struct Cyc_List_List*_tmp968;_tmp47A=((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->hd=((_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp45D,(*((struct _tuple0*)_tmp47B->hd)).f1),((_tmp969->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp45D,(*((struct _tuple0*)_tmp47B->hd)).f2),_tmp969)))))),((_tmp968->tl=_tmp47A,_tmp968))))));}}
# 2129
_tmp47A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp47A);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp47A);{
void*_tmp47E=res_typ;_npop_handler(0);return _tmp47E;};};};};};}
# 2047
;_pop_region(rgn);};}
# 2136
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2138
struct _RegionHandle _tmp480=_new_region("rgn");struct _RegionHandle*rgn=& _tmp480;_push_region(rgn);{
void*_stmttmp42=Cyc_Tcutil_compress(ts);void*_tmp481=_stmttmp42;enum Cyc_Absyn_AggrKind _tmp483;struct Cyc_List_List*_tmp484;_LL1EF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp481;if(_tmp482->tag != 12)goto _LL1F1;else{_tmp483=_tmp482->f1;_tmp484=_tmp482->f2;}}_LL1F0:
# 2141
 if(_tmp483 == Cyc_Absyn_UnionA){
const char*_tmp96C;void*_tmp96B;(_tmp96B=0,Cyc_Tcutil_terr(loc,((_tmp96C="expecting struct but found union",_tag_dyneither(_tmp96C,sizeof(char),33))),_tag_dyneither(_tmp96B,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp484);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp43=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp488;struct Cyc_Absyn_Exp*_tmp489;struct _tuple19*_tmp487=_stmttmp43;_tmp488=_tmp487->f1;_tmp489=_tmp487->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp488->type,_tmp489);
# 2150
if(!Cyc_Tcutil_coerce_arg(te,_tmp489,_tmp488->type,& bogus)){
{const char*_tmp972;void*_tmp971[3];struct Cyc_String_pa_PrintArg_struct _tmp970;struct Cyc_String_pa_PrintArg_struct _tmp96F;struct Cyc_String_pa_PrintArg_struct _tmp96E;(_tmp96E.tag=0,((_tmp96E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2153
Cyc_Absynpp_typ2string((void*)_check_null(_tmp489->topt))),((_tmp96F.tag=0,((_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2152
Cyc_Absynpp_typ2string(_tmp488->type)),((_tmp970.tag=0,((_tmp970.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp488->name),((_tmp971[0]=& _tmp970,((_tmp971[1]=& _tmp96F,((_tmp971[2]=& _tmp96E,Cyc_Tcutil_terr(_tmp489->loc,((_tmp972="field %s of struct expects type %s != %s",_tag_dyneither(_tmp972,sizeof(char),41))),_tag_dyneither(_tmp971,sizeof(void*),3)))))))))))))))))));}
# 2154
Cyc_Tcutil_explain_failure();}}
# 2157
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp975;void*_tmp974;(_tmp974=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp975="tcAnonStruct: wrong type",_tag_dyneither(_tmp975,sizeof(char),25))),_tag_dyneither(_tmp974,sizeof(void*),0)));}_LL1EE:;}{
# 2161
void*_tmp491=ts;_npop_handler(0);return _tmp491;};
# 2138
;_pop_region(rgn);}
# 2165
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2168
struct _RegionHandle _tmp492=_new_region("rgn");struct _RegionHandle*rgn=& _tmp492;_push_region(rgn);
{struct _tuple12 _tmp976;struct _tuple12 _tmp493=(_tmp976.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp976.f2=rgn,_tmp976)));
struct Cyc_List_List*_tmp494=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp493,tud->tvs);
struct Cyc_List_List*_tmp495=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp494);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp97C;struct Cyc_Absyn_DatatypeFieldInfo _tmp97B;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp97A;void*res=(void*)(
(_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97C.tag=4,((_tmp97C.f1=((_tmp97B.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp97B.targs=_tmp495,_tmp97B)))),_tmp97C)))),_tmp97A))));
# 2176
if(topt != 0){
void*_stmttmp44=Cyc_Tcutil_compress(*topt);void*_tmp496=_stmttmp44;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp496;if(_tmp497->tag != 4)goto _LL1F6;}_LL1F5:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F3;_LL1F6:;_LL1F7:
 goto _LL1F3;_LL1F3:;}{
# 2182
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp494 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp494,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp984;void*_tmp983[3];struct Cyc_String_pa_PrintArg_struct _tmp982;struct Cyc_String_pa_PrintArg_struct _tmp981;const char*_tmp980;struct Cyc_String_pa_PrintArg_struct _tmp97F;(_tmp97F.tag=0,((_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2193
e->topt == 0?(struct _dyneither_ptr)((_tmp980="?",_tag_dyneither(_tmp980,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp981.tag=0,((_tmp981.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2192
Cyc_Absynpp_typ2string(t)),((_tmp982.tag=0,((_tmp982.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp983[0]=& _tmp982,((_tmp983[1]=& _tmp981,((_tmp983[2]=& _tmp97F,Cyc_Tcutil_terr(e->loc,((_tmp984="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp984,sizeof(char),82))),_tag_dyneither(_tmp983,sizeof(void*),3)))))))))))))))))));}
# 2194
Cyc_Tcutil_explain_failure();}}
# 2197
if(es != 0){
const char*_tmp988;void*_tmp987[1];struct Cyc_String_pa_PrintArg_struct _tmp986;void*_tmp4A1=(_tmp986.tag=0,((_tmp986.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp987[0]=& _tmp986,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp988="too many arguments for datatype constructor %s",_tag_dyneither(_tmp988,sizeof(char),47))),_tag_dyneither(_tmp987,sizeof(void*),1)))))));_npop_handler(0);return _tmp4A1;}
if(ts != 0){
const char*_tmp98C;void*_tmp98B[1];struct Cyc_String_pa_PrintArg_struct _tmp98A;void*_tmp4A5=(_tmp98A.tag=0,((_tmp98A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp98B[0]=& _tmp98A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp98C="too few arguments for datatype constructor %s",_tag_dyneither(_tmp98C,sizeof(char),46))),_tag_dyneither(_tmp98B,sizeof(void*),1)))))));_npop_handler(0);return _tmp4A5;}{
void*_tmp4A6=res;_npop_handler(0);return _tmp4A6;};};}
# 2169
;_pop_region(rgn);}
# 2208
static int Cyc_Tcexp_zeroable_type(void*t){
void*_stmttmp45=Cyc_Tcutil_compress(t);void*_tmp4AB=_stmttmp45;union Cyc_Absyn_Constraint*_tmp4B2;void*_tmp4B6;struct Cyc_List_List*_tmp4B9;union Cyc_Absyn_AggrInfoU _tmp4BB;struct Cyc_List_List*_tmp4BC;struct Cyc_List_List*_tmp4C0;_LL1F9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4AC=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AC->tag != 0)goto _LL1FB;}_LL1FA:
 return 1;_LL1FB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4AD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4AB;if(_tmp4AD->tag != 1)goto _LL1FD;}_LL1FC:
 goto _LL1FE;_LL1FD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AE->tag != 2)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4AF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4AB;if(_tmp4AF->tag != 3)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4B0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B0->tag != 4)goto _LL203;}_LL202:
 return 0;_LL203: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B1->tag != 5)goto _LL205;else{_tmp4B2=((_tmp4B1->f1).ptr_atts).nullable;}}_LL204:
# 2216
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4B2);_LL205: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B3->tag != 6)goto _LL207;}_LL206:
 goto _LL208;_LL207: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B4->tag != 7)goto _LL209;}_LL208:
 return 1;_LL209: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B5->tag != 8)goto _LL20B;else{_tmp4B6=(_tmp4B5->f1).elt_type;}}_LL20A:
 return Cyc_Tcexp_zeroable_type(_tmp4B6);_LL20B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B7->tag != 9)goto _LL20D;}_LL20C:
 return 0;_LL20D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4B8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4AB;if(_tmp4B8->tag != 10)goto _LL20F;else{_tmp4B9=_tmp4B8->f1;}}_LL20E:
# 2222
 for(0;(unsigned int)_tmp4B9;_tmp4B9=_tmp4B9->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp4B9->hd)).f2))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4BA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4AB;if(_tmp4BA->tag != 11)goto _LL211;else{_tmp4BB=(_tmp4BA->f1).aggr_info;_tmp4BC=(_tmp4BA->f1).targs;}}_LL210: {
# 2226
struct Cyc_Absyn_Aggrdecl*_tmp4CD=Cyc_Absyn_get_known_aggrdecl(_tmp4BB);
if(_tmp4CD->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CD->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp4CE=_new_region("r");struct _RegionHandle*r=& _tmp4CE;_push_region(r);
{struct Cyc_List_List*_tmp4CF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4CD->tvs,_tmp4BC);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CD->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4CF,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4D0=0;_npop_handler(0);return _tmp4D0;}}}{
int _tmp4D1=1;_npop_handler(0);return _tmp4D1;};}
# 2230
;_pop_region(r);};}_LL211: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4AB;if(_tmp4BD->tag != 13)goto _LL213;}_LL212:
# 2235
 return 1;_LL213: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4AB;if(_tmp4BE->tag != 19)goto _LL215;}_LL214:
 return 1;_LL215: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4AB;if(_tmp4BF->tag != 12)goto _LL217;else{_tmp4C0=_tmp4BF->f2;}}_LL216:
# 2238
 for(0;_tmp4C0 != 0;_tmp4C0=_tmp4C0->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4C0->hd)->type))return 0;}
return 1;_LL217: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C1->tag != 14)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C2->tag != 17)goto _LL21B;}_LL21A:
 return 0;_LL21B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C3->tag != 16)goto _LL21D;}_LL21C:
 return 0;_LL21D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C4->tag != 15)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C5->tag != 26)goto _LL221;}_LL220:
# 2246
 goto _LL222;_LL221: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4AB;if(_tmp4C6->tag != 18)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4C7=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4AB;if(_tmp4C7->tag != 20)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4C8=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4AB;if(_tmp4C8->tag != 21)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4AB;if(_tmp4C9->tag != 22)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4CA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4AB;if(_tmp4CA->tag != 23)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4AB;if(_tmp4CB->tag != 24)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4AB;if(_tmp4CC->tag != 25)goto _LL1F8;}_LL22E: {
const char*_tmp990;void*_tmp98F[1];struct Cyc_String_pa_PrintArg_struct _tmp98E;(_tmp98E.tag=0,((_tmp98E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp98F[0]=& _tmp98E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp990="bad type `%s' in zeroable type",_tag_dyneither(_tmp990,sizeof(char),31))),_tag_dyneither(_tmp98F,sizeof(void*),1)))))));}_LL1F8:;}
# 2257
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2259
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2261
if(topt != 0){
void*_stmttmp46=Cyc_Tcutil_compress(*topt);void*_tmp4D5=_stmttmp46;void*_tmp4D7;_LL230: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D5;if(_tmp4D6->tag != 5)goto _LL232;else{_tmp4D7=(_tmp4D6->f1).elt_typ;}}_LL231:
# 2264
 Cyc_Tcutil_unify(_tmp4D7,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL22F;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 2270
const char*_tmp999;void*_tmp998[2];const char*_tmp997;const char*_tmp996;struct Cyc_String_pa_PrintArg_struct _tmp995;struct Cyc_String_pa_PrintArg_struct _tmp994;(_tmp994.tag=0,((_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp995.tag=0,((_tmp995.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp996="calloc",_tag_dyneither(_tmp996,sizeof(char),7))):(struct _dyneither_ptr)((_tmp997="malloc",_tag_dyneither(_tmp997,sizeof(char),7))))),((_tmp998[0]=& _tmp995,((_tmp998[1]=& _tmp994,Cyc_Tcutil_terr(loc,((_tmp999="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp999,sizeof(char),60))),_tag_dyneither(_tmp998,sizeof(void*),2)))))))))))));};}
# 2274
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2276
enum Cyc_Absyn_AliasQual _stmttmp47=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_stmttmp47){case Cyc_Absyn_Unique: _LL234:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL235:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2282
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2287
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2290
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp99C;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp99B;void*expected_type=(void*)(
(_tmp99B=_cycalloc(sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99C.tag=15,((_tmp99C.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp99C)))),_tmp99B))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_stmttmp48=Cyc_Tcutil_compress(handle_type);void*_tmp4DE=_stmttmp48;void*_tmp4E0;_LL238: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4DF->tag != 15)goto _LL23A;else{_tmp4E0=(void*)_tmp4DF->f1;}}_LL239:
# 2295
 rgn=_tmp4E0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL237;_LL23A:;_LL23B:
# 2299
{const char*_tmp9A0;void*_tmp99F[1];struct Cyc_String_pa_PrintArg_struct _tmp99E;(_tmp99E.tag=0,((_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp99F[0]=& _tmp99E,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9A0="expecting region_t type but found %s",_tag_dyneither(_tmp9A0,sizeof(char),37))),_tag_dyneither(_tmp99F,sizeof(void*),1)))))));}
goto _LL237;_LL237:;}else{
# 2306
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2313
{struct _RegionHandle _tmp4E6=_new_region("r");struct _RegionHandle*r=& _tmp4E6;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2324 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp9A3;void*_tmp9A2;(_tmp9A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A3="calloc with empty type",_tag_dyneither(_tmp9A3,sizeof(char),23))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2336
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4E9=er;void*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EE;_LL23D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4EA=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4E9;if(_tmp4EA->tag != 16)goto _LL23F;else{_tmp4EB=(void*)_tmp4EA->f1;}}_LL23E:
# 2340
 elt_type=_tmp4EB;
{void**_tmp9A4;*t=((_tmp9A4=_cycalloc(sizeof(*_tmp9A4)),((_tmp9A4[0]=elt_type,_tmp9A4))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL23C;_LL23F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E9;if(_tmp4EC->tag != 2)goto _LL241;else{if(_tmp4EC->f1 != Cyc_Absyn_Times)goto _LL241;if(_tmp4EC->f2 == 0)goto _LL241;_tmp4ED=(struct Cyc_Absyn_Exp*)(_tmp4EC->f2)->hd;if((_tmp4EC->f2)->tl == 0)goto _LL241;_tmp4EE=(struct Cyc_Absyn_Exp*)((_tmp4EC->f2)->tl)->hd;if(((_tmp4EC->f2)->tl)->tl != 0)goto _LL241;}}_LL240:
# 2347
{struct _tuple0 _tmp9A5;struct _tuple0 _stmttmp4C=(_tmp9A5.f1=_tmp4ED->r,((_tmp9A5.f2=_tmp4EE->r,_tmp9A5)));struct _tuple0 _tmp4F0=_stmttmp4C;void*_tmp4F2;void*_tmp4F4;_LL244: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F0.f1;if(_tmp4F1->tag != 16)goto _LL246;else{_tmp4F2=(void*)_tmp4F1->f1;}}_LL245:
# 2349
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4F2);
elt_type=_tmp4F2;
{void**_tmp9A6;*t=((_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6[0]=elt_type,_tmp9A6))));}
num_elts=_tmp4EE;
one_elt=0;
goto _LL243;_LL246: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4F3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F0.f2;if(_tmp4F3->tag != 16)goto _LL248;else{_tmp4F4=(void*)_tmp4F3->f1;}}_LL247:
# 2356
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4F4);
elt_type=_tmp4F4;
{void**_tmp9A7;*t=((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=elt_type,_tmp9A7))));}
num_elts=_tmp4ED;
one_elt=0;
goto _LL243;_LL248:;_LL249:
 goto No_sizeof;_LL243:;}
# 2364
goto _LL23C;_LL241:;_LL242:
# 2366
 No_sizeof: {
# 2369
struct Cyc_Absyn_Exp*_tmp4F8=*e;
{void*_stmttmp49=_tmp4F8->r;void*_tmp4F9=_stmttmp49;struct Cyc_Absyn_Exp*_tmp4FB;_LL24B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4F9;if(_tmp4FA->tag != 13)goto _LL24D;else{_tmp4FB=_tmp4FA->f2;}}_LL24C:
 _tmp4F8=_tmp4FB;goto _LL24A;_LL24D:;_LL24E:
 goto _LL24A;_LL24A:;}
# 2374
{void*_stmttmp4A=Cyc_Tcutil_compress((void*)_check_null(_tmp4F8->topt));void*_tmp4FC=_stmttmp4A;void*_tmp4FE;_LL250: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FC;if(_tmp4FD->tag != 19)goto _LL252;else{_tmp4FE=(void*)_tmp4FD->f1;}}_LL251:
# 2376
{void*_stmttmp4B=Cyc_Tcutil_compress(_tmp4FE);void*_tmp4FF=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp501;_LL255: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FF;if(_tmp500->tag != 18)goto _LL257;else{_tmp501=_tmp500->f1;}}_LL256:
# 2378
 er=_tmp501->r;goto retry_sizeof;_LL257:;_LL258:
 goto _LL254;_LL254:;}
# 2381
goto _LL24F;_LL252:;_LL253:
 goto _LL24F;_LL24F:;}
# 2384
elt_type=Cyc_Absyn_char_typ;
{void**_tmp9A8;*t=((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=elt_type,_tmp9A8))));}
num_elts=*e;
one_elt=0;}
# 2389
goto _LL23C;_LL23C:;}}
# 2393
*is_fat=!one_elt;
# 2396
{void*_tmp503=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp505;_LL25A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp504=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp503;if(_tmp504->tag != 11)goto _LL25C;else{if((((_tmp504->f1).aggr_info).KnownAggr).tag != 2)goto _LL25C;_tmp505=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp504->f1).aggr_info).KnownAggr).val);}}_LL25B:
# 2398
 if(_tmp505->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp505->impl))->exist_vars != 0){
const char*_tmp9AB;void*_tmp9AA;(_tmp9AA=0,Cyc_Tcutil_terr(loc,((_tmp9AB="malloc with existential types not yet implemented",_tag_dyneither(_tmp9AB,sizeof(char),50))),_tag_dyneither(_tmp9AA,sizeof(void*),0)));}
goto _LL259;_LL25C:;_LL25D:
 goto _LL259;_LL259:;}{
# 2405
void*(*_tmp508)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp509=Cyc_Absyn_false_conref;
if(topt != 0){
void*_stmttmp4D=Cyc_Tcutil_compress(*topt);void*_tmp50A=_stmttmp4D;union Cyc_Absyn_Constraint*_tmp50C;union Cyc_Absyn_Constraint*_tmp50D;union Cyc_Absyn_Constraint*_tmp50E;_LL25F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp50A;if(_tmp50B->tag != 5)goto _LL261;else{_tmp50C=((_tmp50B->f1).ptr_atts).nullable;_tmp50D=((_tmp50B->f1).ptr_atts).bounds;_tmp50E=((_tmp50B->f1).ptr_atts).zero_term;}}_LL260:
# 2410
 _tmp509=_tmp50E;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp50C))
_tmp508=Cyc_Absyn_star_typ;
# 2415
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp50E) && !(*is_calloc)){
{const char*_tmp9AE;void*_tmp9AD;(_tmp9AD=0,Cyc_Tcutil_warn(loc,((_tmp9AE="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9AE,sizeof(char),55))),_tag_dyneither(_tmp9AD,sizeof(void*),0)));}
*is_calloc=1;}
# 2421
{void*_stmttmp4E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp50D);void*_tmp511=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp514;_LL264: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp512=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp511;if(_tmp512->tag != 0)goto _LL266;}_LL265:
 goto _LL263;_LL266:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp513=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp511;if(_tmp513->tag != 1)goto _LL268;else{_tmp514=_tmp513->f1;}}if(!(!one_elt))goto _LL268;_LL267: {
# 2424
int _tmp515=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp515  && Cyc_Evexp_same_const_exp(_tmp514,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp50D),_tmp509);}
# 2430
{void*_stmttmp4F=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp516=_stmttmp4F;void*_tmp518;_LL26B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp516;if(_tmp517->tag != 19)goto _LL26D;else{_tmp518=(void*)_tmp517->f1;}}_LL26C: {
# 2432
struct Cyc_Absyn_Exp*_tmp519=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp518,0),0,Cyc_Absyn_No_coercion,0);
# 2434
if(Cyc_Evexp_same_const_exp(_tmp519,_tmp514)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp50D),_tmp509);}
# 2439
goto _LL26A;}_LL26D:;_LL26E:
 goto _LL26A;_LL26A:;}
# 2442
goto _LL263;}_LL268:;_LL269:
 goto _LL263;_LL263:;}
# 2445
goto _LL25E;_LL261:;_LL262:
 goto _LL25E;_LL25E:;}
# 2448
if(!one_elt)_tmp508=Cyc_Absyn_dyneither_typ;
return _tmp508(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp509);};};}
# 2453
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2459
struct _RegionHandle _tmp51A=_new_region("r");struct _RegionHandle*r=& _tmp51A;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp51B=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp51B,0,e1);{
void*_tmp51C=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp51B,& _tmp51C,e2);};}{
# 2465
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2468
{void*_stmttmp50=Cyc_Tcutil_compress(t1);void*_tmp51D=_stmttmp50;_LL270: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51D;if(_tmp51E->tag != 8)goto _LL272;}_LL271:
{const char*_tmp9B1;void*_tmp9B0;(_tmp9B0=0,Cyc_Tcutil_terr(loc,((_tmp9B1="cannot assign to an array",_tag_dyneither(_tmp9B1,sizeof(char),26))),_tag_dyneither(_tmp9B0,sizeof(void*),0)));}goto _LL26F;_LL272:;_LL273:
 goto _LL26F;_LL26F:;}{
# 2473
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp9B4;void*_tmp9B3;(_tmp9B3=0,Cyc_Tcutil_terr(loc,((_tmp9B4="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9B4,sizeof(char),58))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}
# 2478
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp9B7;void*_tmp9B6;void*_tmp525=(_tmp9B6=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9B7="swap non-lvalue",_tag_dyneither(_tmp9B7,sizeof(char),16))),_tag_dyneither(_tmp9B6,sizeof(void*),0)));_npop_handler(0);return _tmp525;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp9BA;void*_tmp9B9;void*_tmp528=(_tmp9B9=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9BA="swap non-lvalue",_tag_dyneither(_tmp9BA,sizeof(char),16))),_tag_dyneither(_tmp9B9,sizeof(void*),0)));_npop_handler(0);return _tmp528;}{
# 2483
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9BD;void*_tmp9BC;void*_tmp52B=(_tmp9BC=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9BD="swap value in zeroterm array",_tag_dyneither(_tmp9BD,sizeof(char),29))),_tag_dyneither(_tmp9BC,sizeof(void*),0)));_npop_handler(0);return _tmp52B;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9C0;void*_tmp9BF;void*_tmp52E=(_tmp9BF=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9C0="swap value in zeroterm array",_tag_dyneither(_tmp9C0,sizeof(char),29))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));_npop_handler(0);return _tmp52E;}
# 2490
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9C5;void*_tmp9C4[2];struct Cyc_String_pa_PrintArg_struct _tmp9C3;struct Cyc_String_pa_PrintArg_struct _tmp9C2;void*_tmp52F=(_tmp9C2.tag=0,((_tmp9C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp9C3.tag=0,((_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9C4[0]=& _tmp9C3,((_tmp9C4[1]=& _tmp9C2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9C5="type mismatch: %s != %s",_tag_dyneither(_tmp9C5,sizeof(char),24))),_tag_dyneither(_tmp9C4,sizeof(void*),2)))))))))))));
void*_tmp530=_tmp52F;_npop_handler(0);return _tmp530;}{
# 2497
void*_tmp535=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp535;};};};};
# 2459
;_pop_region(r);}
# 2501
int Cyc_Tcexp_in_stmt_exp=0;
# 2503
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp536=_new_region("r");struct _RegionHandle*r=& _tmp536;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2510
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2512
while(1){
void*_stmttmp51=s->r;void*_tmp537=_stmttmp51;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Stmt*_tmp53B;struct Cyc_Absyn_Stmt*_tmp53C;struct Cyc_Absyn_Decl*_tmp53E;struct Cyc_Absyn_Stmt*_tmp53F;_LL275: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp538=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp537;if(_tmp538->tag != 1)goto _LL277;else{_tmp539=_tmp538->f1;}}_LL276: {
# 2516
void*_tmp540=(void*)_check_null(_tmp539->topt);
if(!Cyc_Tcutil_unify(_tmp540,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp9C9;void*_tmp9C8[1];struct Cyc_String_pa_PrintArg_struct _tmp9C7;(_tmp9C7.tag=0,((_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp540)),((_tmp9C8[0]=& _tmp9C7,Cyc_Tcutil_terr(loc,((_tmp9C9="statement expression returns type %s",_tag_dyneither(_tmp9C9,sizeof(char),37))),_tag_dyneither(_tmp9C8,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2522
void*_tmp544=_tmp540;_npop_handler(0);return _tmp544;};}_LL277: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp53A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp537;if(_tmp53A->tag != 2)goto _LL279;else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;}}_LL278:
 s=_tmp53C;continue;_LL279: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp53D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp537;if(_tmp53D->tag != 12)goto _LL27B;else{_tmp53E=_tmp53D->f1;_tmp53F=_tmp53D->f2;}}_LL27A:
 s=_tmp53F;continue;_LL27B:;_LL27C: {
# 2526
const char*_tmp9CC;void*_tmp9CB;void*_tmp547=(_tmp9CB=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9CC="statement expression must end with expression",_tag_dyneither(_tmp9CC,sizeof(char),46))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));_npop_handler(0);return _tmp547;}_LL274:;}
# 2504
;_pop_region(r);}
# 2531
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp548=_new_region("r");struct _RegionHandle*r=& _tmp548;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2536
{void*_tmp549=t;struct Cyc_Absyn_Aggrdecl*_tmp54B;_LL27E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 11)goto _LL280;else{if((((_tmp54A->f1).aggr_info).KnownAggr).tag != 2)goto _LL280;_tmp54B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp54A->f1).aggr_info).KnownAggr).val);}}_LL27F:
# 2538
 if((_tmp54B->kind == Cyc_Absyn_UnionA  && _tmp54B->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp54B->impl))->tagged)goto _LL27D;
goto _LL281;_LL280:;_LL281:
# 2541
{const char*_tmp9D0;void*_tmp9CF[1];struct Cyc_String_pa_PrintArg_struct _tmp9CE;(_tmp9CE.tag=0,((_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9CF[0]=& _tmp9CE,Cyc_Tcutil_terr(loc,((_tmp9D0="expecting @tagged union but found %s",_tag_dyneither(_tmp9D0,sizeof(char),37))),_tag_dyneither(_tmp9CF,sizeof(void*),1)))))));}
goto _LL27D;_LL27D:;}{
# 2544
void*_tmp54F=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp54F;};
# 2533
;_pop_region(r);}
# 2548
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2552
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp550=_new_region("r");struct _RegionHandle*r=& _tmp550;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp551=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2557
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D3;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9D2;void*expected_type=(void*)(
(_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=15,((_tmp9D3.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp551)),_tmp9D3)))),_tmp9D2))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp551,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_stmttmp52=Cyc_Tcutil_compress(handle_type);void*_tmp552=_stmttmp52;void*_tmp554;_LL283: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp552;if(_tmp553->tag != 15)goto _LL285;else{_tmp554=(void*)_tmp553->f1;}}_LL284:
# 2562
 rgn=_tmp554;
Cyc_Tcenv_check_rgn_accessible(_tmp551,loc,rgn);
goto _LL282;_LL285:;_LL286:
# 2566
{const char*_tmp9D7;void*_tmp9D6[1];struct Cyc_String_pa_PrintArg_struct _tmp9D5;(_tmp9D5.tag=0,((_tmp9D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9D6[0]=& _tmp9D5,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9D7="expecting region_t type but found %s",_tag_dyneither(_tmp9D7,sizeof(char),37))),_tag_dyneither(_tmp9D6,sizeof(void*),1)))))));}
goto _LL282;_LL282:;}else{
# 2573
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2581
void*_stmttmp53=e1->r;void*_tmp55A=_stmttmp53;struct Cyc_Core_Opt*_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp561;_LL288: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp55B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp55B->tag != 26)goto _LL28A;}_LL289:
 goto _LL28B;_LL28A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp55C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp55C->tag != 27)goto _LL28C;}_LL28B: {
# 2586
void*_tmp564=Cyc_Tcexp_tcExpNoPromote(_tmp551,topt,e1);
void*_stmttmp57=Cyc_Tcutil_compress(_tmp564);void*_tmp565=_stmttmp57;void*_tmp567;struct Cyc_Absyn_Tqual _tmp568;struct Cyc_Absyn_Exp*_tmp569;union Cyc_Absyn_Constraint*_tmp56A;_LL297: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp565;if(_tmp566->tag != 8)goto _LL299;else{_tmp567=(_tmp566->f1).elt_type;_tmp568=(_tmp566->f1).tq;_tmp569=(_tmp566->f1).num_elts;_tmp56A=(_tmp566->f1).zero_term;}}_LL298: {
# 2589
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp569);
void*b;
{void*_stmttmp58=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp56B=_stmttmp58;void*_tmp56D;_LL29C: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp56B;if(_tmp56C->tag != 19)goto _LL29E;else{_tmp56D=(void*)_tmp56C->f1;}}_LL29D:
# 2593
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9D9;b=(void*)((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=1,((_tmp9DA.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp56D,0),0,Cyc_Absyn_No_coercion,0),_tmp9DA)))),_tmp9D9))));}
goto _LL29B;_LL29E:;_LL29F:
# 2596
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9DC;b=(void*)((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=1,((_tmp9DD.f1=bnd,_tmp9DD)))),_tmp9DC))));}else{
# 2599
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL29B:;}{
# 2601
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9E3;struct Cyc_Absyn_PtrInfo _tmp9E2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E1;void*res_typ=(void*)(
(_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=((_tmp9E3.tag=5,((_tmp9E3.f1=((_tmp9E2.elt_typ=_tmp567,((_tmp9E2.elt_tq=_tmp568,((_tmp9E2.ptr_atts=(
((_tmp9E2.ptr_atts).rgn=rgn,(((_tmp9E2.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9E2.ptr_atts).bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp9E2.ptr_atts).zero_term=_tmp56A,(((_tmp9E2.ptr_atts).ptrloc=0,_tmp9E2.ptr_atts)))))))))),_tmp9E2)))))),_tmp9E3)))),_tmp9E1))));
# 2605
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2612
void*_tmp572=res_typ;_npop_handler(0);return _tmp572;};};}_LL299:;_LL29A: {
const char*_tmp9E6;void*_tmp9E5;(_tmp9E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9E6="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9E6,sizeof(char),45))),_tag_dyneither(_tmp9E5,sizeof(void*),0)));}_LL296:;}_LL28C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp55D->tag != 35)goto _LL28E;else{_tmp55E=_tmp55D->f1;_tmp55F=_tmp55D->f2;}}_LL28D:
# 2616
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9E9;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9E8;e1->r=(void*)((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=25,((_tmp9E9.f1=_tmp55F,_tmp9E9)))),_tmp9E8))));}
_tmp561=_tmp55F;goto _LL28F;_LL28E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp560->tag != 25)goto _LL290;else{_tmp561=_tmp560->f1;}}_LL28F: {
# 2619
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp55=Cyc_Tcutil_compress(*topt);void*_tmp57A=_stmttmp55;void**_tmp57C;struct Cyc_Absyn_Tqual _tmp57D;union Cyc_Absyn_Constraint*_tmp57E;_LL2A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57A;if(_tmp57B->tag != 5)goto _LL2A3;else{_tmp57C=(void**)&(_tmp57B->f1).elt_typ;_tmp57D=(_tmp57B->f1).elt_tq;_tmp57E=((_tmp57B->f1).ptr_atts).zero_term;}}_LL2A2:
# 2625
 elt_typ_opt=_tmp57C;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp57E);
goto _LL2A0;_LL2A3:;_LL2A4:
 goto _LL2A0;_LL2A0:;}{
# 2631
void*_tmp57F=Cyc_Tcexp_tcArray(_tmp551,e1->loc,elt_typ_opt,0,zero_term,_tmp561);
e1->topt=_tmp57F;{
void*res_typ;
{void*_stmttmp56=Cyc_Tcutil_compress(_tmp57F);void*_tmp580=_stmttmp56;void*_tmp582;struct Cyc_Absyn_Tqual _tmp583;struct Cyc_Absyn_Exp*_tmp584;union Cyc_Absyn_Constraint*_tmp585;_LL2A6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 8)goto _LL2A8;else{_tmp582=(_tmp581->f1).elt_type;_tmp583=(_tmp581->f1).tq;_tmp584=(_tmp581->f1).num_elts;_tmp585=(_tmp581->f1).zero_term;}}_LL2A7:
# 2636
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9FA;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9F9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9F8;struct Cyc_Absyn_PtrInfo _tmp9F7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F6;res_typ=(void*)(
(_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9FA.tag=5,((_tmp9FA.f1=((_tmp9F7.elt_typ=_tmp582,((_tmp9F7.elt_tq=_tmp583,((_tmp9F7.ptr_atts=(
((_tmp9F7.ptr_atts).rgn=rgn,(((_tmp9F7.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9F7.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=1,((_tmp9F9.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp584),_tmp9F9)))),_tmp9F8))))),(((_tmp9F7.ptr_atts).zero_term=_tmp585,(((_tmp9F7.ptr_atts).ptrloc=0,_tmp9F7.ptr_atts)))))))))),_tmp9F7)))))),_tmp9FA)))),_tmp9F6))));}
# 2641
if(topt != 0){
# 2645
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2651
goto _LL2A5;_LL2A8:;_LL2A9: {
const char*_tmp9FD;void*_tmp9FC;(_tmp9FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9FD="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9FD,sizeof(char),50))),_tag_dyneither(_tmp9FC,sizeof(void*),0)));}_LL2A5:;}{
# 2654
void*_tmp58D=res_typ;_npop_handler(0);return _tmp58D;};};};}_LL290: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp562=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp562->tag != 0)goto _LL292;else{if(((_tmp562->f1).String_c).tag != 8)goto _LL292;}}_LL291: {
# 2659
void*_tmp58E=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2661
void*_tmp58F=Cyc_Tcexp_tcExp(_tmp551,& _tmp58E,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA00;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9FF;void*_tmp592=Cyc_Absyn_atb_typ(_tmp58F,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=1,((_tmpA00.f1=Cyc_Absyn_uint_exp(1,0),_tmpA00)))),_tmp9FF)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp592;}_LL292: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp563=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp563->tag != 0)goto _LL294;else{if(((_tmp563->f1).Wstring_c).tag != 9)goto _LL294;}}_LL293: {
# 2666
void*_tmp593=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2668
void*_tmp594=Cyc_Tcexp_tcExp(_tmp551,& _tmp593,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA03;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA02;void*_tmp597=Cyc_Absyn_atb_typ(_tmp594,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA03.tag=1,((_tmpA03.f1=Cyc_Absyn_uint_exp(1,0),_tmpA03)))),_tmpA02)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp597;}_LL294:;_LL295:
# 2675
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_stmttmp54=Cyc_Tcutil_compress(*topt);void*_tmp598=_stmttmp54;void**_tmp59A;struct Cyc_Absyn_Tqual _tmp59B;_LL2AB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598;if(_tmp599->tag != 5)goto _LL2AD;else{_tmp59A=(void**)&(_tmp599->f1).elt_typ;_tmp59B=(_tmp599->f1).elt_tq;}}_LL2AC:
# 2681
 topt2=_tmp59A;goto _LL2AA;_LL2AD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp598;if(_tmp59C->tag != 3)goto _LL2AF;}_LL2AE:
# 2685
 bogus=*topt;
topt2=& bogus;
goto _LL2AA;_LL2AF:;_LL2B0:
 goto _LL2AA;_LL2AA:;}{
# 2691
void*telt=Cyc_Tcexp_tcExp(_tmp551,topt2,e1);
# 2693
struct _RegionHandle _tmp59D=_new_region("temp");struct _RegionHandle*temp=& _tmp59D;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmpA06;void*_tmpA05;(_tmpA05=0,Cyc_Tcutil_terr(e1->loc,((_tmpA06="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA06,sizeof(char),49))),_tag_dyneither(_tmpA05,sizeof(void*),0)));}{
# 2698
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA0C;struct Cyc_Absyn_PtrInfo _tmpA0B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA0A;void*res_typ=(void*)(
(_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0C.tag=5,((_tmpA0C.f1=((_tmpA0B.elt_typ=telt,((_tmpA0B.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA0B.ptr_atts=(
((_tmpA0B.ptr_atts).rgn=rgn,(((_tmpA0B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpA0B.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmpA0B.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmpA0B.ptr_atts).ptrloc=0,_tmpA0B.ptr_atts)))))))))),_tmpA0B)))))),_tmpA0C)))),_tmpA0A))));
# 2703
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2710
void*_tmp5A0=res_typ;_npop_handler(1);return _tmp5A0;};};
# 2693
;_pop_region(temp);};}_LL287:;};}
# 2554
;_pop_region(r);}
# 2717
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp5A4=t;void*_tmp5A6;struct Cyc_Absyn_Tqual _tmp5A7;struct Cyc_Absyn_Exp*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A9;_LL2B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4;if(_tmp5A5->tag != 8)goto _LL2B4;else{_tmp5A6=(_tmp5A5->f1).elt_type;_tmp5A7=(_tmp5A5->f1).tq;_tmp5A8=(_tmp5A5->f1).num_elts;_tmp5A9=(_tmp5A5->f1).zero_term;}}_LL2B3: {
# 2721
struct _tuple14 _stmttmp59=Cyc_Tcutil_addressof_props(te,e);void*_tmp5AB;struct _tuple14 _tmp5AA=_stmttmp59;_tmp5AB=_tmp5AA.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA0F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA0E;void*_tmp5AC=_tmp5A8 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA0F.tag=1,((_tmpA0F.f1=_tmp5A8,_tmpA0F)))),_tmpA0E))));
t=Cyc_Absyn_atb_typ(_tmp5A6,_tmp5AB,_tmp5A7,_tmp5AC,_tmp5A9);
e->topt=t;
return t;};}_LL2B4:;_LL2B5:
# 2727
 return t;_LL2B1:;}
# 2734
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2738
struct _RegionHandle _tmp5AF=_new_region("temp");struct _RegionHandle*temp=& _tmp5AF;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2744
const char*_tmpA12;void*_tmpA11;(_tmpA11=0,Cyc_Tcutil_terr(e->loc,((_tmpA12="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA12,sizeof(char),49))),_tag_dyneither(_tmpA11,sizeof(void*),0)));}{
# 2747
void*_stmttmp5A=e->r;void*_tmp5B2=_stmttmp5A;_LL2B7: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5B3=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B3->tag != 25)goto _LL2B9;}_LL2B8:
 goto _LL2BA;_LL2B9: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5B4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B4->tag != 26)goto _LL2BB;}_LL2BA:
 goto _LL2BC;_LL2BB: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5B5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B5->tag != 27)goto _LL2BD;}_LL2BC:
 goto _LL2BE;_LL2BD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B6->tag != 0)goto _LL2BF;else{if(((_tmp5B6->f1).Wstring_c).tag != 9)goto _LL2BF;}}_LL2BE:
 goto _LL2C0;_LL2BF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B2;if(_tmp5B7->tag != 0)goto _LL2C1;else{if(((_tmp5B7->f1).String_c).tag != 8)goto _LL2C1;}}_LL2C0: {
void*_tmp5B8=t;_npop_handler(0);return _tmp5B8;}_LL2C1:;_LL2C2:
# 2754
 t=Cyc_Tcutil_compress(t);{
void*_tmp5B9=t;void*_tmp5BB;struct Cyc_Absyn_Tqual _tmp5BC;struct Cyc_Absyn_Exp*_tmp5BD;union Cyc_Absyn_Constraint*_tmp5BE;_LL2C4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B9;if(_tmp5BA->tag != 8)goto _LL2C6;else{_tmp5BB=(_tmp5BA->f1).elt_type;_tmp5BC=(_tmp5BA->f1).tq;_tmp5BD=(_tmp5BA->f1).num_elts;_tmp5BE=(_tmp5BA->f1).zero_term;}}_LL2C5: {
# 2757
struct _tuple14 _stmttmp5B=Cyc_Tcutil_addressof_props(te,e);void*_tmp5C0;struct _tuple14 _tmp5BF=_stmttmp5B;_tmp5C0=_tmp5BF.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA15;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA14;void*b=_tmp5BD == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14[0]=((_tmpA15.tag=1,((_tmpA15.f1=_tmp5BD,_tmpA15)))),_tmpA14))));
t=Cyc_Absyn_atb_typ(_tmp5BB,_tmp5C0,_tmp5BC,b,_tmp5BE);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp5C1=t;_npop_handler(0);return _tmp5C1;};};}_LL2C6:;_LL2C7: {
# 2764
void*_tmp5C4=t;_npop_handler(0);return _tmp5C4;}_LL2C3:;};_LL2B6:;};
# 2738
;_pop_region(temp);}
# 2776 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_stmttmp5C=e->r;void*_tmp5C5=_stmttmp5C;struct Cyc_Absyn_Exp*_tmp5C7;_LL2C9: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5C6=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C5;if(_tmp5C6->tag != 11)goto _LL2CB;else{_tmp5C7=_tmp5C6->f1;}}_LL2CA:
# 2780
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5C7);
_tmp5C7->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5C7->topt),0);
e->topt=_tmp5C7->topt;
goto _LL2C8;_LL2CB:;_LL2CC:
# 2786
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2789
{void*_stmttmp5D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5C8=_stmttmp5D;void*_tmp5CA;struct Cyc_Absyn_Tqual _tmp5CB;void*_tmp5CC;union Cyc_Absyn_Constraint*_tmp5CD;union Cyc_Absyn_Constraint*_tmp5CE;union Cyc_Absyn_Constraint*_tmp5CF;_LL2CE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C8;if(_tmp5C9->tag != 5)goto _LL2D0;else{_tmp5CA=(_tmp5C9->f1).elt_typ;_tmp5CB=(_tmp5C9->f1).elt_tq;_tmp5CC=((_tmp5C9->f1).ptr_atts).rgn;_tmp5CD=((_tmp5C9->f1).ptr_atts).nullable;_tmp5CE=((_tmp5C9->f1).ptr_atts).bounds;_tmp5CF=((_tmp5C9->f1).ptr_atts).zero_term;}}_LL2CF:
# 2791
{void*_stmttmp5E=Cyc_Tcutil_compress(_tmp5CA);void*_tmp5D0=_stmttmp5E;struct Cyc_List_List*_tmp5D2;void*_tmp5D3;struct Cyc_Absyn_Tqual _tmp5D4;void*_tmp5D5;struct Cyc_List_List*_tmp5D6;int _tmp5D7;struct Cyc_Absyn_VarargInfo*_tmp5D8;struct Cyc_List_List*_tmp5D9;struct Cyc_List_List*_tmp5DA;_LL2D3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5D1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D0;if(_tmp5D1->tag != 9)goto _LL2D5;else{_tmp5D2=(_tmp5D1->f1).tvars;_tmp5D3=(_tmp5D1->f1).effect;_tmp5D4=(_tmp5D1->f1).ret_tqual;_tmp5D5=(_tmp5D1->f1).ret_typ;_tmp5D6=(_tmp5D1->f1).args;_tmp5D7=(_tmp5D1->f1).c_varargs;_tmp5D8=(_tmp5D1->f1).cyc_varargs;_tmp5D9=(_tmp5D1->f1).rgn_po;_tmp5DA=(_tmp5D1->f1).attributes;}}_LL2D4:
# 2793
 if(_tmp5D2 != 0){
struct _RegionHandle _tmp5DB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5DB;_push_region(rgn);
{struct _tuple12 _tmpA16;struct _tuple12 _tmp5DC=(_tmpA16.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA16.f2=rgn,_tmpA16)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5DC,_tmp5D2);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2802
_tmp5D9=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5D9);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5D9);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA1C;struct Cyc_Absyn_FnInfo _tmpA1B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA1A;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1C.tag=9,((_tmpA1C.f1=((_tmpA1B.tvars=0,((_tmpA1B.effect=_tmp5D3,((_tmpA1B.ret_tqual=_tmp5D4,((_tmpA1B.ret_typ=_tmp5D5,((_tmpA1B.args=_tmp5D6,((_tmpA1B.c_varargs=_tmp5D7,((_tmpA1B.cyc_varargs=_tmp5D8,((_tmpA1B.rgn_po=0,((_tmpA1B.attributes=_tmp5DA,_tmpA1B)))))))))))))))))),_tmpA1C)))),_tmpA1A)))));
# 2808
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA22;struct Cyc_Absyn_PtrInfo _tmpA21;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA20;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DD=(_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20[0]=((_tmpA22.tag=5,((_tmpA22.f1=((_tmpA21.elt_typ=ftyp,((_tmpA21.elt_tq=_tmp5CB,((_tmpA21.ptr_atts=(((_tmpA21.ptr_atts).rgn=_tmp5CC,(((_tmpA21.ptr_atts).nullable=_tmp5CD,(((_tmpA21.ptr_atts).bounds=_tmp5CE,(((_tmpA21.ptr_atts).zero_term=_tmp5CF,(((_tmpA21.ptr_atts).ptrloc=0,_tmpA21.ptr_atts)))))))))),_tmpA21)))))),_tmpA22)))),_tmpA20)));
# 2810
struct Cyc_Absyn_Exp*_tmp5DE=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA25;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA24;e->r=(void*)((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA25.tag=12,((_tmpA25.f1=_tmp5DE,((_tmpA25.f2=ts,_tmpA25)))))),_tmpA24))));}
e->topt=(void*)_tmp5DD;};}
# 2795
;_pop_region(rgn);}
# 2815
goto _LL2D2;_LL2D5:;_LL2D6:
 goto _LL2D2;_LL2D2:;}
# 2818
goto _LL2CD;_LL2D0:;_LL2D1:
 goto _LL2CD;_LL2CD:;}
# 2821
goto _LL2C8;_LL2C8:;}
# 2823
return(void*)_check_null(e->topt);}
# 2831
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2833
struct _RegionHandle _tmp5E8=_new_region("r");struct _RegionHandle*r=& _tmp5E8;_push_region(r);
{struct Cyc_List_List*_tmp5E9=0;
# 2836
{void*_stmttmp5F=fn_exp->r;void*_tmp5EA=_stmttmp5F;struct Cyc_List_List*_tmp5EC;_LL2D8: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EB->tag != 9)goto _LL2DA;else{_tmp5EC=_tmp5EB->f2;}}_LL2D9:
# 2838
{void*_stmttmp60=e->r;void*_tmp5ED=_stmttmp60;struct Cyc_List_List*_tmp5EF;_LL2DD: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5ED;if(_tmp5EE->tag != 9)goto _LL2DF;else{_tmp5EF=_tmp5EE->f2;}}_LL2DE: {
# 2840
struct Cyc_List_List*_tmp5F0=alias_arg_exps;
int _tmp5F1=0;
while(_tmp5F0 != 0){
while(_tmp5F1 != (int)_tmp5F0->hd){
if(_tmp5EC == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA2F;struct Cyc_Int_pa_PrintArg_struct _tmpA2E;void*_tmpA2D[2];const char*_tmpA2C;void*_tmpA2B;(_tmpA2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA2F.tag=1,((_tmpA2F.f1=(unsigned long)((int)_tmp5F0->hd),((_tmpA2E.tag=1,((_tmpA2E.f1=(unsigned long)_tmp5F1,((_tmpA2D[0]=& _tmpA2E,((_tmpA2D[1]=& _tmpA2F,Cyc_aprintf(((_tmpA2C="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA2C,sizeof(char),36))),_tag_dyneither(_tmpA2D,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA2B,sizeof(void*),0)));}
# 2847
++ _tmp5F1;
_tmp5EC=_tmp5EC->tl;
_tmp5EF=((struct Cyc_List_List*)_check_null(_tmp5EF))->tl;}{
# 2852
struct _tuple11 _stmttmp61=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5EC))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5EF))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F9;struct _tuple11 _tmp5F7=_stmttmp61;_tmp5F8=_tmp5F7.f1;_tmp5F9=_tmp5F7.f2;
_tmp5EC->hd=(void*)_tmp5F9;
{struct Cyc_List_List*_tmpA30;_tmp5E9=((_tmpA30=_region_malloc(r,sizeof(*_tmpA30)),((_tmpA30->hd=_tmp5F8,((_tmpA30->tl=_tmp5E9,_tmpA30))))));}
_tmp5F0=_tmp5F0->tl;};}
# 2857
goto _LL2DC;}_LL2DF:;_LL2E0: {
const char*_tmpA33;void*_tmpA32;(_tmpA32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA33="not a function call!",_tag_dyneither(_tmpA33,sizeof(char),21))),_tag_dyneither(_tmpA32,sizeof(void*),0)));}_LL2DC:;}
# 2860
goto _LL2D7;_LL2DA:;_LL2DB: {
const char*_tmpA36;void*_tmpA35;(_tmpA35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA36="not a function call!",_tag_dyneither(_tmpA36,sizeof(char),21))),_tag_dyneither(_tmpA35,sizeof(void*),0)));}_LL2D7:;}
# 2865
while(_tmp5E9 != 0){
struct Cyc_Absyn_Decl*_tmp5FF=(struct Cyc_Absyn_Decl*)_tmp5E9->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5FF,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5E9=_tmp5E9->tl;}
# 2872
e->topt=0;
e->r=fn_exp->r;}
# 2834
;_pop_region(r);}
# 2879
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2883
{void*_stmttmp62=e->r;void*_tmp600=_stmttmp62;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_Absyn_Exp*_tmp604;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_VarargCallInfo**_tmp606;int*_tmp607;struct Cyc_Core_Opt*_tmp609;struct Cyc_List_List*_tmp60A;union Cyc_Absyn_Cnst*_tmp60C;struct _tuple1*_tmp60E;void**_tmp60F;enum Cyc_Absyn_Primop _tmp611;struct Cyc_List_List*_tmp612;struct Cyc_Absyn_Exp*_tmp614;enum Cyc_Absyn_Incrementor _tmp615;struct Cyc_Absyn_Exp*_tmp617;struct Cyc_Core_Opt*_tmp618;struct Cyc_Absyn_Exp*_tmp619;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_Absyn_Exp*_tmp61D;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Exp*_tmp620;struct Cyc_Absyn_Exp*_tmp622;struct Cyc_Absyn_Exp*_tmp623;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_List_List*_tmp629;struct Cyc_Absyn_VarargCallInfo**_tmp62A;struct Cyc_Absyn_Exp*_tmp62C;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_List_List*_tmp62F;void*_tmp631;struct Cyc_Absyn_Exp*_tmp632;enum Cyc_Absyn_Coercion*_tmp633;struct Cyc_Absyn_Exp*_tmp635;struct Cyc_Absyn_Exp**_tmp637;struct Cyc_Absyn_Exp*_tmp638;struct Cyc_Absyn_Exp*_tmp63A;void*_tmp63C;void*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp643;struct _dyneither_ptr*_tmp644;int*_tmp645;int*_tmp646;struct Cyc_Absyn_Exp*_tmp648;struct _dyneither_ptr*_tmp649;int*_tmp64A;int*_tmp64B;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_List_List*_tmp650;struct _tuple9*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_List_List*_tmp655;struct Cyc_Absyn_Stmt*_tmp657;struct Cyc_Absyn_Vardecl*_tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;int*_tmp65C;struct Cyc_Absyn_Exp*_tmp65E;void*_tmp65F;int*_tmp660;struct _tuple1**_tmp662;struct Cyc_List_List**_tmp663;struct Cyc_List_List*_tmp664;struct Cyc_Absyn_Aggrdecl**_tmp665;void*_tmp667;struct Cyc_List_List*_tmp668;struct Cyc_List_List*_tmp66A;struct Cyc_Absyn_Datatypedecl*_tmp66B;struct Cyc_Absyn_Datatypefield*_tmp66C;struct _tuple1**_tmp66E;struct Cyc_Absyn_Enumdecl*_tmp66F;struct Cyc_Absyn_Enumfield*_tmp670;struct _tuple1**_tmp672;void*_tmp673;struct Cyc_Absyn_Enumfield*_tmp674;int*_tmp676;struct Cyc_Absyn_Exp**_tmp677;void***_tmp678;struct Cyc_Absyn_Exp**_tmp679;int*_tmp67A;struct Cyc_Absyn_Exp*_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*_tmp67F;struct _dyneither_ptr*_tmp680;void*_tmp682;_LL2E2: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp601=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp601->tag != 11)goto _LL2E4;else{_tmp602=_tmp601->f1;}}_LL2E3:
# 2888
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp602);
return;_LL2E4:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp603=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp603->tag != 9)goto _LL2E6;else{_tmp604=_tmp603->f1;_tmp605=_tmp603->f2;_tmp606=(struct Cyc_Absyn_VarargCallInfo**)& _tmp603->f3;_tmp607=(int*)& _tmp603->f4;}}if(!(!(*_tmp607)))goto _LL2E6;_LL2E5:
# 2894
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp604,_tmp605)){
*_tmp607=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2898
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2901
return;_LL2E6: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp608=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp608->tag != 35)goto _LL2E8;else{_tmp609=_tmp608->f1;_tmp60A=_tmp608->f2;}}_LL2E7:
# 2905
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp60A);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2E8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp60B->tag != 0)goto _LL2EA;else{_tmp60C=(union Cyc_Absyn_Cnst*)& _tmp60B->f1;}}_LL2E9:
# 2910
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp60C,e);goto _LL2E1;_LL2EA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp60D->tag != 1)goto _LL2EC;else{_tmp60E=_tmp60D->f1;_tmp60F=(void**)((void**)& _tmp60D->f2);}}_LL2EB:
# 2912
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp60E,_tmp60F);goto _LL2E1;_LL2EC: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp610->tag != 2)goto _LL2EE;else{_tmp611=_tmp610->f1;_tmp612=_tmp610->f2;}}_LL2ED:
# 2914
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp611,_tmp612);goto _LL2E1;_LL2EE: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp613->tag != 4)goto _LL2F0;else{_tmp614=_tmp613->f1;_tmp615=_tmp613->f2;}}_LL2EF:
# 2916
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp614,_tmp615);goto _LL2E1;_LL2F0: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp616=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp616->tag != 3)goto _LL2F2;else{_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;_tmp619=_tmp616->f3;}}_LL2F1:
# 2918
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp617,_tmp618,_tmp619);goto _LL2E1;_LL2F2: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp61A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp61A->tag != 5)goto _LL2F4;else{_tmp61B=_tmp61A->f1;_tmp61C=_tmp61A->f2;_tmp61D=_tmp61A->f3;}}_LL2F3:
# 2920
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp61B,_tmp61C,_tmp61D);goto _LL2E1;_LL2F4: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp61E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp61E->tag != 6)goto _LL2F6;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61E->f2;}}_LL2F5:
# 2922
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp61F,_tmp620);goto _LL2E1;_LL2F6: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp621=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp621->tag != 7)goto _LL2F8;else{_tmp622=_tmp621->f1;_tmp623=_tmp621->f2;}}_LL2F7:
# 2924
 t=Cyc_Tcexp_tcOr(te,loc,_tmp622,_tmp623);goto _LL2E1;_LL2F8: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp624->tag != 8)goto _LL2FA;else{_tmp625=_tmp624->f1;_tmp626=_tmp624->f2;}}_LL2F9:
# 2926
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp625,_tmp626);goto _LL2E1;_LL2FA: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp627=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp627->tag != 9)goto _LL2FC;else{_tmp628=_tmp627->f1;_tmp629=_tmp627->f2;_tmp62A=(struct Cyc_Absyn_VarargCallInfo**)& _tmp627->f3;}}_LL2FB: {
# 2932
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp684;_push_handler(& _tmp684);{int _tmp686=0;if(setjmp(_tmp684.handler))_tmp686=1;if(!_tmp686){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp685=(void*)_exn_thrown;void*_tmp688=_tmp685;struct _dyneither_ptr _tmp68A;void*_tmp68B;_LL335: {struct Cyc_Core_Failure_exn_struct*_tmp689=(struct Cyc_Core_Failure_exn_struct*)_tmp688;if(_tmp689->tag != Cyc_Core_Failure)goto _LL337;else{_tmp68A=_tmp689->f1;}}_LL336:
# 2939
 ok=0;
fn_exp=e;
goto _LL334;_LL337: _tmp68B=_tmp688;_LL338:(void)_throw(_tmp68B);_LL334:;}};}
# 2943
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp628,_tmp629,_tmp62A,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2950
goto _LL2E1;}_LL2FC: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp62B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp62B->tag != 10)goto _LL2FE;else{_tmp62C=_tmp62B->f1;}}_LL2FD:
# 2952
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp62C);goto _LL2E1;_LL2FE: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp62D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp62D->tag != 12)goto _LL300;else{_tmp62E=_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL2FF:
# 2954
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp62E,_tmp62F);goto _LL2E1;_LL300: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp630->tag != 13)goto _LL302;else{_tmp631=(void*)_tmp630->f1;_tmp632=_tmp630->f2;_tmp633=(enum Cyc_Absyn_Coercion*)& _tmp630->f4;}}_LL301:
# 2956
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp631,_tmp632,_tmp633);goto _LL2E1;_LL302: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp634=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp634->tag != 14)goto _LL304;else{_tmp635=_tmp634->f1;}}_LL303:
# 2958
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp635);goto _LL2E1;_LL304: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp636=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp636->tag != 15)goto _LL306;else{_tmp637=(struct Cyc_Absyn_Exp**)& _tmp636->f1;_tmp638=_tmp636->f2;}}_LL305:
# 2960
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp637,e,_tmp638);goto _LL2E1;_LL306: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp639->tag != 17)goto _LL308;else{_tmp63A=_tmp639->f1;}}_LL307: {
# 2962
void*_tmp68C=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp63A);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp68C);goto _LL2E1;}_LL308: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp63B->tag != 16)goto _LL30A;else{_tmp63C=(void*)_tmp63B->f1;}}_LL309:
# 2965
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp63C);goto _LL2E1;_LL30A: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp63D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp63D->tag != 18)goto _LL30C;else{_tmp63E=(void*)_tmp63D->f1;_tmp63F=_tmp63D->f2;}}_LL30B:
# 2967
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp63E,_tmp63F);goto _LL2E1;_LL30C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp640->tag != 19)goto _LL30E;else{_tmp641=_tmp640->f1;}}_LL30D:
# 2969
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp641);goto _LL2E1;_LL30E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp642->tag != 20)goto _LL310;else{_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=(int*)& _tmp642->f3;_tmp646=(int*)& _tmp642->f4;}}_LL30F:
# 2971
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp643,_tmp644,_tmp645,_tmp646);goto _LL2E1;_LL310: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp647=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp647->tag != 21)goto _LL312;else{_tmp648=_tmp647->f1;_tmp649=_tmp647->f2;_tmp64A=(int*)& _tmp647->f3;_tmp64B=(int*)& _tmp647->f4;}}_LL311:
# 2973
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp648,_tmp649,_tmp64A,_tmp64B);goto _LL2E1;_LL312: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp64C->tag != 22)goto _LL314;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;}}_LL313:
# 2975
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp64D,_tmp64E);goto _LL2E1;_LL314: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp64F=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp64F->tag != 23)goto _LL316;else{_tmp650=_tmp64F->f1;}}_LL315:
# 2977
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp650);goto _LL2E1;_LL316: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp651->tag != 24)goto _LL318;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;}}_LL317:
# 2979
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp652,_tmp653);goto _LL2E1;_LL318: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp654=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp654->tag != 25)goto _LL31A;else{_tmp655=_tmp654->f1;}}_LL319: {
# 2983
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp63=Cyc_Tcutil_compress(*topt);void*_tmp68D=_stmttmp63;void**_tmp68F;struct Cyc_Absyn_Tqual*_tmp690;union Cyc_Absyn_Constraint*_tmp691;_LL33A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68D;if(_tmp68E->tag != 8)goto _LL33C;else{_tmp68F=(void**)&(_tmp68E->f1).elt_type;_tmp690=(struct Cyc_Absyn_Tqual*)&(_tmp68E->f1).tq;_tmp691=(_tmp68E->f1).zero_term;}}_LL33B:
# 2989
 elt_topt=_tmp68F;
elt_tqopt=_tmp690;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp691);
goto _LL339;_LL33C:;_LL33D:
 goto _LL339;_LL339:;}
# 2996
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp655);goto _LL2E1;}_LL31A: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp656=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp656->tag != 36)goto _LL31C;else{_tmp657=_tmp656->f1;}}_LL31B:
# 2998
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp657);goto _LL2E1;_LL31C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp658=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp658->tag != 26)goto _LL31E;else{_tmp659=_tmp658->f1;_tmp65A=_tmp658->f2;_tmp65B=_tmp658->f3;_tmp65C=(int*)& _tmp658->f4;}}_LL31D:
# 3000
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp659,_tmp65A,_tmp65B,_tmp65C);goto _LL2E1;_LL31E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp65D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp65D->tag != 27)goto _LL320;else{_tmp65E=_tmp65D->f1;_tmp65F=(void*)_tmp65D->f2;_tmp660=(int*)& _tmp65D->f3;}}_LL31F:
# 3002
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp65E,_tmp65F,_tmp660);goto _LL2E1;_LL320: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp661->tag != 28)goto _LL322;else{_tmp662=(struct _tuple1**)& _tmp661->f1;_tmp663=(struct Cyc_List_List**)& _tmp661->f2;_tmp664=_tmp661->f3;_tmp665=(struct Cyc_Absyn_Aggrdecl**)& _tmp661->f4;}}_LL321:
# 3004
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp662,_tmp663,_tmp664,_tmp665);goto _LL2E1;_LL322: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp666=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp666->tag != 29)goto _LL324;else{_tmp667=(void*)_tmp666->f1;_tmp668=_tmp666->f2;}}_LL323:
# 3006
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp667,_tmp668);goto _LL2E1;_LL324: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp669=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp669->tag != 30)goto _LL326;else{_tmp66A=_tmp669->f1;_tmp66B=_tmp669->f2;_tmp66C=_tmp669->f3;}}_LL325:
# 3008
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp66A,_tmp66B,_tmp66C);goto _LL2E1;_LL326: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp66D=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp66D->tag != 31)goto _LL328;else{_tmp66E=(struct _tuple1**)& _tmp66D->f1;_tmp66F=_tmp66D->f2;_tmp670=_tmp66D->f3;}}_LL327:
# 3010
*_tmp66E=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp670))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA39;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA38;t=(void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA39.tag=13,((_tmpA39.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp66F))->name,((_tmpA39.f2=_tmp66F,_tmpA39)))))),_tmpA38))));}goto _LL2E1;_LL328: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp671->tag != 32)goto _LL32A;else{_tmp672=(struct _tuple1**)& _tmp671->f1;_tmp673=(void*)_tmp671->f2;_tmp674=_tmp671->f3;}}_LL329:
# 3013
*_tmp672=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp674))->name;
t=_tmp673;goto _LL2E1;_LL32A: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp675=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp675->tag != 33)goto _LL32C;else{_tmp676=(int*)&(_tmp675->f1).is_calloc;_tmp677=(struct Cyc_Absyn_Exp**)&(_tmp675->f1).rgn;_tmp678=(void***)&(_tmp675->f1).elt_type;_tmp679=(struct Cyc_Absyn_Exp**)&(_tmp675->f1).num_elts;_tmp67A=(int*)&(_tmp675->f1).fat_result;}}_LL32B:
# 3016
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp677,_tmp678,_tmp679,_tmp676,_tmp67A);goto _LL2E1;_LL32C: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp67B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp67B->tag != 34)goto _LL32E;else{_tmp67C=_tmp67B->f1;_tmp67D=_tmp67B->f2;}}_LL32D:
# 3018
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp67C,_tmp67D);goto _LL2E1;_LL32E: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp67E=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp67E->tag != 37)goto _LL330;else{_tmp67F=_tmp67E->f1;_tmp680=_tmp67E->f2;}}_LL32F:
# 3020
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp67F,_tmp680);goto _LL2E1;_LL330: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp681=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp681->tag != 38)goto _LL332;else{_tmp682=(void*)_tmp681->f1;}}_LL331:
# 3022
 if(!te->allow_valueof){
const char*_tmpA3C;void*_tmpA3B;(_tmpA3B=0,Cyc_Tcutil_terr(e->loc,((_tmpA3C="valueof(-) can only occur within types",_tag_dyneither(_tmpA3C,sizeof(char),39))),_tag_dyneither(_tmpA3B,sizeof(void*),0)));}
# 3030
t=Cyc_Absyn_sint_typ;
goto _LL2E1;_LL332: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp683=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp600;if(_tmp683->tag != 39)goto _LL2E1;}_LL333:
# 3033
{const char*_tmpA3F;void*_tmpA3E;(_tmpA3E=0,Cyc_Tcutil_terr(e->loc,((_tmpA3F="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA3F,sizeof(char),50))),_tag_dyneither(_tmpA3E,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2E1:;}
# 3036
e->topt=t;}
