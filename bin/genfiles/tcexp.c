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
# 232
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244
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
# 49
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
# 430
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
# 913
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
# 121
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
# 218
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
# 41
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
# 163
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 182
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 261
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp662;(_tmp662=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp662,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple18*_tmp663;return(_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663->f1=0,((_tmp663->f2=e,_tmp663)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_tmpA=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp667;void*_tmp666[1];struct Cyc_String_pa_PrintArg_struct _tmp665;(_tmp665.tag=0,((_tmp665.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp666[0]=& _tmp665,Cyc_Tcutil_terr(e1->loc,((_tmp667="undeclared identifier %s",_tag_dyneither(_tmp667,sizeof(char),25))),_tag_dyneither(_tmp666,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp66B;void*_tmp66A[1];struct Cyc_String_pa_PrintArg_struct _tmp669;(_tmp669.tag=0,((_tmp669.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp66A[0]=& _tmp669,Cyc_Tcutil_terr(e->loc,((_tmp66B="%s is a constant, not a function",_tag_dyneither(_tmp66B,sizeof(char),33))),_tag_dyneither(_tmp66A,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp66E;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp66D;e->r=(void*)((_tmp66D=_cycalloc(sizeof(*_tmp66D)),((_tmp66D[0]=((_tmp66E.tag=29,((_tmp66E.f1=es,((_tmp66E.f2=_tmp10,((_tmp66E.f3=_tmp11,_tmp66E)))))))),_tmp66D))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp671;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp670;e->r=(void*)((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670[0]=((_tmp671.tag=27,((_tmp671.f1=_tmp13->name,((_tmp671.f2=0,((_tmp671.f3=_tmp22,((_tmp671.f4=_tmp13,_tmp671)))))))))),_tmp670))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp675;void*_tmp674[1];struct Cyc_String_pa_PrintArg_struct _tmp673;(_tmp673.tag=0,((_tmp673.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp674[0]=& _tmp673,Cyc_Tcutil_terr(e->loc,((_tmp675="%s is an enum constructor, not a function",_tag_dyneither(_tmp675,sizeof(char),42))),_tag_dyneither(_tmp674,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp679;void*_tmp678[1];struct Cyc_String_pa_PrintArg_struct _tmp677;(_tmp677.tag=0,((_tmp677.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp678[0]=& _tmp677,Cyc_Tcutil_terr(e1->loc,((_tmp679="undeclared identifier %s",_tag_dyneither(_tmp679,sizeof(char),25))),_tag_dyneither(_tmp678,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp67C;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp67B;e->r=(void*)((_tmp67B=_cycalloc(sizeof(*_tmp67B)),((_tmp67B[0]=((_tmp67C.tag=25,((_tmp67C.f1=des,_tmp67C)))),_tmp67B))));}
return;}{
# 125
void*t=*topt;
void*_tmp33=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp67F;void*_tmp67E;(_tmp67E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp67F="struct type not properly set",_tag_dyneither(_tmp67F,sizeof(char),29))),_tag_dyneither(_tmp67E,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp682;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp681;e->r=(void*)((_tmp681=_cycalloc(sizeof(*_tmp681)),((_tmp681[0]=((_tmp682.tag=27,((_tmp682.f1=_tmp3B->name,((_tmp682.f2=0,((_tmp682.f3=des,((_tmp682.f4=_tmp3B,_tmp682)))))))))),_tmp681))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp685;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp684;e->r=(void*)((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684[0]=((_tmp685.tag=25,((_tmp685.f1=des,_tmp685)))),_tmp684))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp688;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp687;e->r=(void*)((_tmp687=_cycalloc(sizeof(*_tmp687)),((_tmp687[0]=((_tmp688.tag=28,((_tmp688.f1=t,((_tmp688.f2=des,_tmp688)))))),_tmp687))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp68B;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp68A;e->r=(void*)((_tmp68A=_cycalloc(sizeof(*_tmp68A)),((_tmp68A[0]=((_tmp68B.tag=25,((_tmp68B.f1=des,_tmp68B)))),_tmp68A))));}goto _LL17;_LL17:;};}
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
const char*_tmp690;void*_tmp68F[2];struct Cyc_String_pa_PrintArg_struct _tmp68E;struct Cyc_String_pa_PrintArg_struct _tmp68D;(_tmp68D.tag=0,((_tmp68D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp68E.tag=0,((_tmp68E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp68F[0]=& _tmp68E,((_tmp68F[1]=& _tmp68D,Cyc_Tcutil_terr(e->loc,((_tmp690="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp690,sizeof(char),53))),_tag_dyneither(_tmp68F,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4A=e->r;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp691;struct _tuple0 _tmp4F=(_tmp691.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp691.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp691)));void*_tmp51;void*_tmp53;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f1;if(_tmp50->tag != 15)goto _LL2D;else{_tmp51=(void*)_tmp50->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp52=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F.f2;if(_tmp52->tag != 15)goto _LL2D;else{_tmp53=(void*)_tmp52->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp694;struct Cyc_Tcexp_TestEnv _tmp693;return(_tmp693.eq=((_tmp694=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp694)),((_tmp694->f1=_tmp51,((_tmp694->f2=_tmp53,_tmp694)))))),((_tmp693.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp693)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp695;return(_tmp695.eq=0,((_tmp695.isTrue=0,_tmp695)));};}
# 195
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp69F;struct Cyc_Absyn_PtrAtts _tmp69E;struct Cyc_Absyn_PtrInfo _tmp69D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69C;t=(void*)((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C[0]=((_tmp69F.tag=5,((_tmp69F.f1=((_tmp69D.elt_typ=_tmp69,((_tmp69D.elt_tq=_tmp6A,((_tmp69D.ptr_atts=(
(_tmp69E.rgn=_tmp6B,((_tmp69E.nullable=Cyc_Absyn_true_conref,((_tmp69E.bounds=_tmp6D,((_tmp69E.zero_term=_tmp6E,((_tmp69E.ptrloc=0,_tmp69E)))))))))),_tmp69D)))))),_tmp69F)))),_tmp69C))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6A2;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6A1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp77=(_tmp6A1=_cycalloc(sizeof(*_tmp6A1)),((_tmp6A1[0]=((_tmp6A2.tag=18,((_tmp6A2.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp6A2)))),_tmp6A1)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6A5;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6A4;t=(void*)((_tmp6A4=_cycalloc(sizeof(*_tmp6A4)),((_tmp6A4[0]=((_tmp6A5.tag=19,((_tmp6A5.f1=(void*)_tmp77,_tmp6A5)))),_tmp6A4))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6A8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7C=(_tmp6A7=_cycalloc(sizeof(*_tmp6A7)),((_tmp6A7[0]=((_tmp6A8.tag=1,((_tmp6A8.f1=elen,_tmp6A8)))),_tmp6A7)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6B2;struct Cyc_Absyn_PtrAtts _tmp6B1;struct Cyc_Absyn_PtrInfo _tmp6B0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AF;return(void*)((_tmp6AF=_cycalloc(sizeof(*_tmp6AF)),((_tmp6AF[0]=((_tmp6B2.tag=5,((_tmp6B2.f1=((_tmp6B0.elt_typ=_tmp85,((_tmp6B0.elt_tq=_tmp86,((_tmp6B0.ptr_atts=((_tmp6B1.rgn=_tmp87,((_tmp6B1.nullable=Cyc_Absyn_true_conref,((_tmp6B1.bounds=_tmp89,((_tmp6B1.zero_term=_tmp8A,((_tmp6B1.ptrloc=0,_tmp6B1)))))))))),_tmp6B0)))))),_tmp6B2)))),_tmp6AF))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8F=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6C5;struct Cyc_Absyn_PtrAtts _tmp6C4;struct Cyc_Core_Opt*_tmp6C3;struct Cyc_Core_Opt*_tmp6C2;struct Cyc_Absyn_PtrInfo _tmp6C1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C0;t=(void*)((_tmp6C0=_cycalloc(sizeof(*_tmp6C0)),((_tmp6C0[0]=((_tmp6C5.tag=5,((_tmp6C5.f1=((_tmp6C1.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6C2=_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2->v=_tmp8F,_tmp6C2))))),((_tmp6C1.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6C1.ptr_atts=(
(_tmp6C4.rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6C3=_cycalloc(sizeof(*_tmp6C3)),((_tmp6C3->v=_tmp8F,_tmp6C3))))),((_tmp6C4.nullable=Cyc_Absyn_true_conref,((_tmp6C4.bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6C4.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp6C4.ptrloc=0,_tmp6C4)))))))))),_tmp6C1)))))),_tmp6C5)))),_tmp6C0))));}
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
const char*_tmp6C9;void*_tmp6C8[1];struct Cyc_String_pa_PrintArg_struct _tmp6C7;void*_tmpB8=(_tmp6C7.tag=0,((_tmp6C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6C8[0]=& _tmp6C7,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C9="undeclared identifier %s",_tag_dyneither(_tmp6C9,sizeof(char),25))),_tag_dyneither(_tmp6C8,sizeof(void*),1)))))));_npop_handler(1);return _tmpB8;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA9=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpA9->tag != 0)goto _LL77;else{_tmpAA=(void*)_tmpA9->f1;}}_LL76:
# 348
*b=_tmpAA;{
void*_tmpB9=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB9;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAB->tag != 3)goto _LL79;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6CC;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6CB;e->r=(void*)((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB[0]=((_tmp6CC.tag=30,((_tmp6CC.f1=_tmpAD->name,((_tmp6CC.f2=_tmpAC,((_tmp6CC.f3=_tmpAD,_tmp6CC)))))))),_tmp6CB))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6CF;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6CE;void*_tmpBE=(void*)((_tmp6CE=_cycalloc(sizeof(*_tmp6CE)),((_tmp6CE[0]=((_tmp6CF.tag=13,((_tmp6CF.f1=_tmpAC->name,((_tmp6CF.f2=_tmpAC,_tmp6CF)))))),_tmp6CE))));_npop_handler(1);return _tmpBE;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpAE->tag != 4)goto _LL7B;else{_tmpAF=(void*)_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6D2;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6D1;e->r=(void*)((_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1[0]=((_tmp6D2.tag=31,((_tmp6D2.f1=_tmpB0->name,((_tmp6D2.f2=_tmpAF,((_tmp6D2.f3=_tmpB0,_tmp6D2)))))))),_tmp6D1))));}{
void*_tmpC1=_tmpAF;_npop_handler(1);return _tmpC1;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB1->tag != 2)goto _LL7D;else{_tmpB2=_tmpB1->f1;_tmpB3=_tmpB1->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6D5;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6D4;e->r=(void*)((_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4[0]=((_tmp6D5.tag=29,((_tmp6D5.f1=0,((_tmp6D5.f2=_tmpB2,((_tmp6D5.f3=_tmpB3,_tmp6D5)))))))),_tmp6D4))));}{
void*_tmpC4=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpB2,_tmpB3);_npop_handler(1);return _tmpC4;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB4=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA6;if(_tmpB4->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_PrintArg_struct _tmp6D7;void*_tmpC8=(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6D8[0]=& _tmp6D7,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6D9="bad occurrence of type name %s",_tag_dyneither(_tmp6D9,sizeof(char),31))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));_npop_handler(1);return _tmpC8;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA3=(void*)_exn_thrown;void*_tmpCA=_tmpA3;void*_tmpCC;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpCB=(struct Cyc_Dict_Absent_exn_struct*)_tmpCA;if(_tmpCB->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6DD;void*_tmp6DC[1];struct Cyc_String_pa_PrintArg_struct _tmp6DB;return(_tmp6DB.tag=0,((_tmp6DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6DC[0]=& _tmp6DB,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6DD="undeclared identifier %s",_tag_dyneither(_tmp6DD,sizeof(char),25))),_tag_dyneither(_tmp6DC,sizeof(void*),1)))))));}_LL82: _tmpCC=_tmpCA;_LL83:(void)_throw(_tmpCC);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp98=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp96;if(_tmp98->tag != 1)goto _LL6A;else{_tmp99=_tmp98->f1;}}_LL69:
# 368
*q=*_tmp99->name;
return _tmp99->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp9A=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp96;if(_tmp9A->tag != 2)goto _LL6C;else{_tmp9B=_tmp9A->f1;}}_LL6B:
*q=*_tmp9B->name;return Cyc_Tcutil_fndecl2typ(_tmp9B);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp9C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp96;if(_tmp9C->tag != 5)goto _LL6E;else{_tmp9D=_tmp9C->f1;}}_LL6D:
 _tmp9F=_tmp9D;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp96;if(_tmp9E->tag != 4)goto _LL70;else{_tmp9F=_tmp9E->f1;}}_LL6F:
 _tmpA1=_tmp9F;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp96;if(_tmpA0->tag != 3)goto _LL65;else{_tmpA1=_tmpA0->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6DE;(*q).f1=(((_tmp6DE.Loc_n).val=0,(((_tmp6DE.Loc_n).tag=4,_tmp6DE))));}
if(te->allow_valueof){
void*_tmpD1=Cyc_Tcutil_compress(_tmpA1->type);void*_tmpD3;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpD1;if(_tmpD2->tag != 19)goto _LL87;else{_tmpD3=(void*)_tmpD2->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6E1;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6E0;e->r=(void*)((_tmp6E0=_cycalloc(sizeof(*_tmp6E0)),((_tmp6E0[0]=((_tmp6E1.tag=37,((_tmp6E1.f1=_tmpD3,_tmp6E1)))),_tmp6E0))));}
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
const char*_tmp6E4;void*_tmp6E3;(_tmp6E3=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->loc,((_tmp6E4="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6E4,sizeof(char),49))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));}
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
{const char*_tmp6E9;void*_tmp6E8[2];struct Cyc_String_pa_PrintArg_struct _tmp6E7;struct Cyc_String_pa_PrintArg_struct _tmp6E6;(_tmp6E6.tag=0,((_tmp6E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6E7.tag=0,((_tmp6E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6E8[0]=& _tmp6E7,((_tmp6E8[1]=& _tmp6E6,Cyc_Tcutil_terr(e->loc,((_tmp6E9="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp6E9,sizeof(char),51))),_tag_dyneither(_tmp6E8,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6EA;*alias_arg_exps=((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA->hd=(void*)arg_cnt,((_tmp6EA->tl=*alias_arg_exps,_tmp6EA))))));}{
struct _RegionHandle _tmpE6=_new_region("temp");struct _RegionHandle*temp=& _tmpE6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6ED;void*_tmp6EC;(_tmp6EC=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6ED="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6ED,sizeof(char),49))),_tag_dyneither(_tmp6EC,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6F0;void*_tmp6EF;(_tmp6EF=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6F0="too few arguments",_tag_dyneither(_tmp6F0,sizeof(char),18))),_tag_dyneither(_tmp6EF,sizeof(void*),0)));}
if(_tmpE0 != 0){
const char*_tmp6F3;void*_tmp6F2;(_tmp6F2=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,((_tmp6F3="too many arguments",_tag_dyneither(_tmp6F3,sizeof(char),19))),_tag_dyneither(_tmp6F2,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6F7;void*_tmp6F6[1];struct Cyc_String_pa_PrintArg_struct _tmp6F5;(_tmp6F5.tag=0,((_tmp6F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F6[0]=& _tmp6F5,Cyc_Tcutil_terr(loc,((_tmp6F7="expecting arithmetic type but found %s",_tag_dyneither(_tmp6F7,sizeof(char),39))),_tag_dyneither(_tmp6F6,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6FB;void*_tmp6FA[1];struct Cyc_String_pa_PrintArg_struct _tmp6F9;(_tmp6F9.tag=0,((_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6FA[0]=& _tmp6F9,Cyc_Tcutil_terr(loc,((_tmp6FB="expecting integral or * type but found %s",_tag_dyneither(_tmp6FB,sizeof(char),42))),_tag_dyneither(_tmp6FA,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6FF;void*_tmp6FE[1];struct Cyc_String_pa_PrintArg_struct _tmp6FD;(_tmp6FD.tag=0,((_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6FE[0]=& _tmp6FD,Cyc_Tcutil_terr(loc,((_tmp6FF="expecting integral type but found %s",_tag_dyneither(_tmp6FF,sizeof(char),37))),_tag_dyneither(_tmp6FE,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmpF6=t;union Cyc_Absyn_Constraint*_tmpF9;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF8->tag != 5)goto _LL9B;else{_tmpF9=((_tmpF8->f1).ptr_atts).bounds;}}_LL9A:
# 467
{void*_tmpFA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF9);struct Cyc_Absyn_Exp*_tmpFD;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFB->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpFA;if(_tmpFC->tag != 1)goto _LL9D;else{_tmpFD=_tmpFC->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmpFD)){
const char*_tmp702;void*_tmp701;(_tmp701=0,Cyc_Tcutil_terr(loc,((_tmp702="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp702,sizeof(char),55))),_tag_dyneither(_tmp701,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp706;void*_tmp705[1];struct Cyc_String_pa_PrintArg_struct _tmp704;(_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp705[0]=& _tmp704,Cyc_Tcutil_terr(loc,((_tmp706="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp706,sizeof(char),47))),_tag_dyneither(_tmp705,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp709;void*_tmp708;(_tmp708=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp709="Non-unary primop",_tag_dyneither(_tmp709,sizeof(char),17))),_tag_dyneither(_tmp708,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
{const char*_tmp70D;void*_tmp70C[1];struct Cyc_String_pa_PrintArg_struct _tmp70B;(_tmp70B.tag=0,((_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp70C[0]=& _tmp70B,Cyc_Tcutil_terr(e1->loc,((_tmp70D="type %s cannot be used here",_tag_dyneither(_tmp70D,sizeof(char),28))),_tag_dyneither(_tmp70C,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
{const char*_tmp711;void*_tmp710[1];struct Cyc_String_pa_PrintArg_struct _tmp70F;(_tmp70F.tag=0,((_tmp70F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp710[0]=& _tmp70F,Cyc_Tcutil_terr(e2->loc,((_tmp711="type %s cannot be used here",_tag_dyneither(_tmp711,sizeof(char),28))),_tag_dyneither(_tmp710,sizeof(void*),1)))))));}
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
const char*_tmp714;void*_tmp713;(_tmp713=0,Cyc_Tcutil_terr(e1->loc,((_tmp714="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp714,sizeof(char),50))),_tag_dyneither(_tmp713,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp717;void*_tmp716;(_tmp716=0,Cyc_Tcutil_terr(e1->loc,((_tmp717="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp717,sizeof(char),54))),_tag_dyneither(_tmp716,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp71B;void*_tmp71A[1];struct Cyc_String_pa_PrintArg_struct _tmp719;(_tmp719.tag=0,((_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp71A[0]=& _tmp719,Cyc_Tcutil_terr(e2->loc,((_tmp71B="expecting int but found %s",_tag_dyneither(_tmp71B,sizeof(char),27))),_tag_dyneither(_tmp71A,sizeof(void*),1)))))));}{
void*_tmp11A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp111);struct Cyc_Absyn_Exp*_tmp11D;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11B->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp11A;if(_tmp11C->tag != 1)goto _LLA8;else{_tmp11D=_tmp11C->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp112)){
const char*_tmp71E;void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_warn(e1->loc,((_tmp71E="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp71E,sizeof(char),70))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp728;struct Cyc_Absyn_PtrAtts _tmp727;struct Cyc_Absyn_PtrInfo _tmp726;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp725;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp120=(_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725[0]=((_tmp728.tag=5,((_tmp728.f1=((_tmp726.elt_typ=_tmp10D,((_tmp726.elt_tq=_tmp10E,((_tmp726.ptr_atts=(
(_tmp727.rgn=_tmp10F,((_tmp727.nullable=Cyc_Absyn_true_conref,((_tmp727.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp727.zero_term=_tmp112,((_tmp727.ptrloc=0,_tmp727)))))))))),_tmp726)))))),_tmp728)))),_tmp725)));
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
{const char*_tmp72D;void*_tmp72C[2];struct Cyc_String_pa_PrintArg_struct _tmp72B;struct Cyc_String_pa_PrintArg_struct _tmp72A;(_tmp72A.tag=0,((_tmp72A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 546
Cyc_Absynpp_typ2string(t2)),((_tmp72B.tag=0,((_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp72C[0]=& _tmp72B,((_tmp72C[1]=& _tmp72A,Cyc_Tcutil_terr(e1->loc,((_tmp72D="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp72D,sizeof(char),59))),_tag_dyneither(_tmp72C,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 549
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp732;void*_tmp731[2];struct Cyc_String_pa_PrintArg_struct _tmp730;struct Cyc_String_pa_PrintArg_struct _tmp72F;(_tmp72F.tag=0,((_tmp72F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 554
Cyc_Absynpp_typ2string(t2)),((_tmp730.tag=0,((_tmp730.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp731[0]=& _tmp730,((_tmp731[1]=& _tmp72F,Cyc_Tcutil_terr(e1->loc,((_tmp732="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp732,sizeof(char),59))),_tag_dyneither(_tmp731,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 558
{const char*_tmp735;void*_tmp734;(_tmp734=0,Cyc_Tcutil_warn(e1->loc,((_tmp735="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp735,sizeof(char),60))),_tag_dyneither(_tmp734,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 562
return Cyc_Absyn_sint_typ;}else{
# 564
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp738;void*_tmp737;(_tmp737=0,Cyc_Tcutil_terr(e1->loc,((_tmp738="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp738,sizeof(char),50))),_tag_dyneither(_tmp737,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){
const char*_tmp73B;void*_tmp73A;(_tmp73A=0,Cyc_Tcutil_terr(e1->loc,((_tmp73B="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp73B,sizeof(char),54))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp740;void*_tmp73F[2];struct Cyc_String_pa_PrintArg_struct _tmp73E;struct Cyc_String_pa_PrintArg_struct _tmp73D;(_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp73E.tag=0,((_tmp73E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp73F[0]=& _tmp73E,((_tmp73F[1]=& _tmp73D,Cyc_Tcutil_terr(e2->loc,((_tmp740="expecting either %s or int but found %s",_tag_dyneither(_tmp740,sizeof(char),40))),_tag_dyneither(_tmp73F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 573
return t1;}}}
# 577
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp743;void*_tmp742;(_tmp742=0,Cyc_Tcutil_warn(e1->loc,((_tmp743="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp743,sizeof(char),60))),_tag_dyneither(_tmp742,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 587
{const char*_tmp746;void*_tmp745;(_tmp745=0,Cyc_Tcutil_warn(e1->loc,((_tmp746="thin pointer subtraction!",_tag_dyneither(_tmp746,sizeof(char),26))),_tag_dyneither(_tmp745,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 590
{const char*_tmp749;void*_tmp748;(_tmp748=0,Cyc_Tcutil_warn(e1->loc,((_tmp749="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp749,sizeof(char),56))),_tag_dyneither(_tmp748,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 593
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp74C;void*_tmp74B;(_tmp74B=0,Cyc_Tcutil_warn(e1->loc,((_tmp74C="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp74C,sizeof(char),51))),_tag_dyneither(_tmp74B,sizeof(void*),0)));}
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
struct _tuple0 _tmp74D;struct _tuple0 _tmp140=(_tmp74D.f1=Cyc_Tcutil_compress(t1),((_tmp74D.f2=Cyc_Tcutil_compress(t2),_tmp74D)));void*_tmp142;void*_tmp144;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140.f1;if(_tmp141->tag != 5)goto _LLB0;else{_tmp142=(_tmp141->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp140.f2;if(_tmp143->tag != 5)goto _LLB0;else{_tmp144=(_tmp143->f1).elt_typ;}};_LLAF:
# 631
 if(Cyc_Tcutil_unify(_tmp142,_tmp144))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp140.f1;if(_tmp145->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp140.f2;if(_tmp146->tag != 15)goto _LLB2;};_LLB1:
# 633
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 636
{const char*_tmp752;void*_tmp751[2];struct Cyc_String_pa_PrintArg_struct _tmp750;struct Cyc_String_pa_PrintArg_struct _tmp74F;(_tmp74F.tag=0,((_tmp74F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp750.tag=0,((_tmp750.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp751[0]=& _tmp750,((_tmp751[1]=& _tmp74F,Cyc_Tcutil_terr(loc,((_tmp752="comparison not allowed between %s and %s",_tag_dyneither(_tmp752,sizeof(char),41))),_tag_dyneither(_tmp751,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp753;struct _tuple0 _tmp14C=(_tmp753.f1=t1,((_tmp753.f2=t2,_tmp753)));void*_tmp14E;void*_tmp150;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14C.f1;if(_tmp14D->tag != 15)goto _LLB7;else{_tmp14E=(void*)_tmp14D->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14C.f2;if(_tmp14F->tag != 15)goto _LLB7;else{_tmp150=(void*)_tmp14F->f1;}};_LLB6:
# 656
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 659
{const char*_tmp758;void*_tmp757[2];struct Cyc_String_pa_PrintArg_struct _tmp756;struct Cyc_String_pa_PrintArg_struct _tmp755;(_tmp755.tag=0,((_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp757[0]=& _tmp756,((_tmp757[1]=& _tmp755,Cyc_Tcutil_terr(loc,((_tmp758="comparison not allowed between %s and %s",_tag_dyneither(_tmp758,sizeof(char),41))),_tag_dyneither(_tmp757,sizeof(void*),2)))))))))))));}
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
const char*_tmp75B;void*_tmp75A;(_tmp75A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp75B="bad binary primop",_tag_dyneither(_tmp75B,sizeof(char),18))),_tag_dyneither(_tmp75A,sizeof(void*),0)));}}}
# 698
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 706
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
Cyc_Tcexp_tcExpList(te,es);{
void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
const char*_tmp75E;void*_tmp75D;return(_tmp75D=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp75E="primitive operator has 0 arguments",_tag_dyneither(_tmp75E,sizeof(char),35))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp761;void*_tmp760;return(_tmp760=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp761="primitive operator has > 2 arguments",_tag_dyneither(_tmp761,sizeof(char),37))),_tag_dyneither(_tmp760,sizeof(void*),0)));}}
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
{const char*_tmp765;void*_tmp764[1];struct Cyc_String_pa_PrintArg_struct _tmp763;(_tmp763.tag=0,((_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp168->name),((_tmp764[0]=& _tmp763,Cyc_Tcutil_terr(loc,((_tmp765="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp765,sizeof(char),56))),_tag_dyneither(_tmp764,sizeof(void*),1)))))));}
return 0;}
# 731
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp168->type))return 0;}
# 733
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15C;if(_tmp161->tag != 4)goto _LLD8;else{if((((_tmp161->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp162=((struct _tuple2)(((_tmp161->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 735
{struct Cyc_List_List*_tmp16C=_tmp162->typs;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct Cyc_Absyn_Tqual _tmp16E;void*_tmp16F;struct _tuple19*_tmp16D=(struct _tuple19*)_tmp16C->hd;_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
if(_tmp16E.real_const){
{const char*_tmp769;void*_tmp768[1];struct Cyc_String_pa_PrintArg_struct _tmp767;(_tmp767.tag=0,((_tmp767.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp162->name)),((_tmp768[0]=& _tmp767,Cyc_Tcutil_terr(loc,((_tmp769="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp769,sizeof(char),64))),_tag_dyneither(_tmp768,sizeof(void*),1)))))));}
return 0;}
# 741
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16F))return 0;}}
# 743
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15C;if(_tmp163->tag != 8)goto _LLDA;else{_tmp164=(_tmp163->f1).elt_type;_tmp165=(_tmp163->f1).tq;}}_LLD9:
# 745
 if(_tmp165.real_const){
{const char*_tmp76C;void*_tmp76B;(_tmp76B=0,Cyc_Tcutil_terr(loc,((_tmp76C="attempt to over-write a const array",_tag_dyneither(_tmp76C,sizeof(char),36))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}
return 0;}
# 749
return Cyc_Tcexp_check_writable_aggr(loc,_tmp164);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15C;if(_tmp166->tag != 10)goto _LLDC;else{_tmp167=_tmp166->f1;}}_LLDB:
# 751
 for(0;_tmp167 != 0;_tmp167=_tmp167->tl){
struct Cyc_Absyn_Tqual _tmp176;void*_tmp177;struct _tuple19*_tmp175=(struct _tuple19*)_tmp167->hd;_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;
if(_tmp176.real_const){
{const char*_tmp76F;void*_tmp76E;(_tmp76E=0,Cyc_Tcutil_terr(loc,((_tmp76F="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp76F,sizeof(char),56))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}
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
{const char*_tmp772;void*_tmp771;(_tmp771=0,Cyc_Tcutil_terr(e->loc,((_tmp772="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp772,sizeof(char),47))),_tag_dyneither(_tmp771,sizeof(void*),0)));}
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
const char*_tmp776;void*_tmp775[1];struct Cyc_String_pa_PrintArg_struct _tmp774;(_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp775[0]=& _tmp774,Cyc_Tcutil_terr(e->loc,((_tmp776="attempt to write a const location: %s",_tag_dyneither(_tmp776,sizeof(char),38))),_tag_dyneither(_tmp775,sizeof(void*),1)))))));};}
# 841
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 844
struct _RegionHandle _tmp1C0=_new_region("temp");struct _RegionHandle*temp=& _tmp1C0;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 847
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp779;void*_tmp778;void*_tmp1C3=(_tmp778=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp779="increment/decrement of non-lvalue",_tag_dyneither(_tmp779,sizeof(char),34))),_tag_dyneither(_tmp778,sizeof(void*),0)));_npop_handler(0);return _tmp1C3;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 852
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(e->loc,((_tmp77C="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp77C,sizeof(char),50))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp77F;void*_tmp77E;(_tmp77E=0,Cyc_Tcutil_terr(e->loc,((_tmp77F="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp77F,sizeof(char),54))),_tag_dyneither(_tmp77E,sizeof(void*),0)));}}else{
# 861
const char*_tmp783;void*_tmp782[1];struct Cyc_String_pa_PrintArg_struct _tmp781;(_tmp781.tag=0,((_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp782[0]=& _tmp781,Cyc_Tcutil_terr(e->loc,((_tmp783="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp783,sizeof(char),44))),_tag_dyneither(_tmp782,sizeof(void*),1)))))));}}{
# 863
void*_tmp1CB=t;_npop_handler(0);return _tmp1CB;};};
# 844
;_pop_region(temp);}
# 867
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 869
struct _tuple0*_tmp1CE;int _tmp1CF;const char*_tmp784;struct Cyc_Tcexp_TestEnv _tmp1CD=Cyc_Tcexp_tcTest(te,e1,((_tmp784="conditional expression",_tag_dyneither(_tmp784,sizeof(char),23))));_tmp1CE=_tmp1CD.eq;_tmp1CF=_tmp1CD.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_List_List _tmp785;struct Cyc_List_List _tmp1D0=(_tmp785.hd=e3,((_tmp785.tl=0,_tmp785)));
struct Cyc_List_List _tmp786;struct Cyc_List_List _tmp1D1=(_tmp786.hd=e2,((_tmp786.tl=& _tmp1D0,_tmp786)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1D1)){
{const char*_tmp78B;void*_tmp78A[2];struct Cyc_String_pa_PrintArg_struct _tmp789;struct Cyc_String_pa_PrintArg_struct _tmp788;(_tmp788.tag=0,((_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp78A[0]=& _tmp789,((_tmp78A[1]=& _tmp788,Cyc_Tcutil_terr(loc,((_tmp78B="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp78B,sizeof(char),48))),_tag_dyneither(_tmp78A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 880
return t;};}
# 884
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 886
{const char*_tmp78C;Cyc_Tcexp_tcTest(te,e1,((_tmp78C="logical-and expression",_tag_dyneither(_tmp78C,sizeof(char),23))));}
{const char*_tmp78D;Cyc_Tcexp_tcTest(te,e2,((_tmp78D="logical-and expression",_tag_dyneither(_tmp78D,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 892
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 894
{const char*_tmp78E;Cyc_Tcexp_tcTest(te,e1,((_tmp78E="logical-or expression",_tag_dyneither(_tmp78E,sizeof(char),22))));}
{const char*_tmp78F;Cyc_Tcexp_tcTest(te,e2,((_tmp78F="logical-or expression",_tag_dyneither(_tmp78F,sizeof(char),22))));}
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
{const char*_tmp792;void*_tmp791;(_tmp791=0,Cyc_Tcutil_terr(loc,((_tmp792="cannot assign to an array",_tag_dyneither(_tmp792,sizeof(char),26))),_tag_dyneither(_tmp791,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 919
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp795;void*_tmp794;(_tmp794=0,Cyc_Tcutil_terr(loc,((_tmp795="type is abstract (can't determine size).",_tag_dyneither(_tmp795,sizeof(char),41))),_tag_dyneither(_tmp794,sizeof(void*),0)));}
# 923
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp798;void*_tmp797;void*_tmp1E5=(_tmp797=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp798="assignment to non-lvalue",_tag_dyneither(_tmp798,sizeof(char),25))),_tag_dyneither(_tmp797,sizeof(void*),0)));_npop_handler(0);return _tmp1E5;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1E6=_new_region("temp");struct _RegionHandle*temp=& _tmp1E6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp79B;void*_tmp79A;(_tmp79A=0,Cyc_Tcutil_terr(e2->loc,((_tmp79B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp79B,sizeof(char),49))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}
# 931
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7A0;void*_tmp79F[2];struct Cyc_String_pa_PrintArg_struct _tmp79E;struct Cyc_String_pa_PrintArg_struct _tmp79D;void*_tmp1E9=(_tmp79D.tag=0,((_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp79F[0]=& _tmp79E,((_tmp79F[1]=& _tmp79D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7A0="type mismatch: %s != %s",_tag_dyneither(_tmp7A0,sizeof(char),24))),_tag_dyneither(_tmp79F,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1EA=_tmp1E9;_npop_handler(1);return _tmp1EA;};}
# 927
;_pop_region(temp);}else{
# 939
enum Cyc_Absyn_Primop _tmp1EF=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1F0=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1EF,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1F0,t1) || Cyc_Tcutil_coerceable(_tmp1F0) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7A5;void*_tmp7A4[2];struct Cyc_String_pa_PrintArg_struct _tmp7A3;struct Cyc_String_pa_PrintArg_struct _tmp7A2;void*_tmp1F1=
(_tmp7A2.tag=0,((_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 945
Cyc_Absynpp_typ2string(t2)),((_tmp7A3.tag=0,((_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 944
Cyc_Absynpp_typ2string(t1)),((_tmp7A4[0]=& _tmp7A3,((_tmp7A4[1]=& _tmp7A2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7A5="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7A5,sizeof(char),82))),_tag_dyneither(_tmp7A4,sizeof(void*),2)))))))))))));
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
{const char*_tmp7AA;void*_tmp7A9[2];struct Cyc_String_pa_PrintArg_struct _tmp7A8;struct Cyc_String_pa_PrintArg_struct _tmp7A7;(_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7A8.tag=0,((_tmp7A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7A9[0]=& _tmp7A8,((_tmp7A9[1]=& _tmp7A7,Cyc_Tcutil_terr(e->loc,((_tmp7AA="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7AA,sizeof(char),54))),_tag_dyneither(_tmp7A9,sizeof(void*),2)))))))))))));}
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
{const char*_tmp7AF;void*_tmp7AE[2];struct Cyc_String_pa_PrintArg_struct _tmp7AD;struct Cyc_String_pa_PrintArg_struct _tmp7AC;(_tmp7AC.tag=0,((_tmp7AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1047
Cyc_Absynpp_typ2string(t2)),((_tmp7AD.tag=0,((_tmp7AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7AE[0]=& _tmp7AD,((_tmp7AE[1]=& _tmp7AC,Cyc_Tcutil_terr(e1->loc,((_tmp7AF="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp7AF,sizeof(char),57))),_tag_dyneither(_tmp7AE,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1052
if(alias_coercion){
struct Cyc_List_List*_tmp7B0;*alias_arg_exps=((_tmp7B0=_cycalloc(sizeof(*_tmp7B0)),((_tmp7B0->hd=(void*)_tmp20C,((_tmp7B0->tl=*alias_arg_exps,_tmp7B0))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7B3;void*_tmp7B2;(_tmp7B2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7B3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7B3,sizeof(char),49))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}
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
struct Cyc_Core_Opt*_tmp7B4;fmt_args=((_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((_tmp7B4->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp22D - 1),_tmp7B4))));}
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
{const char*_tmp7B7;void*_tmp7B6;(_tmp7B6=0,Cyc_Tcutil_terr(loc,((_tmp7B7="bad format arguments",_tag_dyneither(_tmp7B7,sizeof(char),21))),_tag_dyneither(_tmp7B6,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp237=_tmp235;_LL141:(void)_throw(_tmp237);_LL13D:;}};}
# 1094
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1098
if(_tmp21D != 0){const char*_tmp7BA;void*_tmp7B9;(_tmp7B9=0,Cyc_Tcutil_terr(loc,((_tmp7BA="too few arguments for function",_tag_dyneither(_tmp7BA,sizeof(char),31))),_tag_dyneither(_tmp7B9,sizeof(void*),0)));}else{
# 1100
if((_tmp20B != 0  || _tmp21E) || _tmp21F != 0){
if(_tmp21E)
for(0;_tmp20B != 0;_tmp20B=_tmp20B->tl){
Cyc_Tcexp_tcExp(_tmp20E,0,(struct Cyc_Absyn_Exp*)_tmp20B->hd);}else{
if(_tmp21F == 0){
const char*_tmp7BD;void*_tmp7BC;(_tmp7BC=0,Cyc_Tcutil_terr(loc,((_tmp7BD="too many arguments for function",_tag_dyneither(_tmp7BD,sizeof(char),32))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}else{
# 1107
void*_tmp23F;int _tmp240;struct Cyc_Absyn_VarargInfo _tmp23E=*_tmp21F;_tmp23F=_tmp23E.type;_tmp240=_tmp23E.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7BE;struct Cyc_Absyn_VarargCallInfo*_tmp241=(_tmp7BE=_cycalloc(sizeof(*_tmp7BE)),((_tmp7BE->num_varargs=0,((_tmp7BE->injectors=0,((_tmp7BE->vai=_tmp21F,_tmp7BE)))))));
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
{const char*_tmp7C3;void*_tmp7C2[2];struct Cyc_String_pa_PrintArg_struct _tmp7C1;struct Cyc_String_pa_PrintArg_struct _tmp7C0;(_tmp7C0.tag=0,((_tmp7C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7C1.tag=0,((_tmp7C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7C2[0]=& _tmp7C1,((_tmp7C2[1]=& _tmp7C0,Cyc_Tcutil_terr(loc,((_tmp7C3="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7C3,sizeof(char),49))),_tag_dyneither(_tmp7C2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1125
if(alias_coercion){
struct Cyc_List_List*_tmp7C4;*alias_arg_exps=((_tmp7C4=_cycalloc(sizeof(*_tmp7C4)),((_tmp7C4->hd=(void*)_tmp20C,((_tmp7C4->tl=*alias_arg_exps,_tmp7C4))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp23F) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7C7;void*_tmp7C6;(_tmp7C6=0,Cyc_Tcutil_terr(e1->loc,((_tmp7C7="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7C7,sizeof(char),49))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}}else{
# 1134
void*_tmp249=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp23F));struct Cyc_Absyn_Datatypedecl*_tmp24B;struct Cyc_List_List*_tmp24C;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 3)goto _LL145;else{if((((_tmp24A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp24B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp24A->f1).datatype_info).KnownDatatype).val);_tmp24C=(_tmp24A->f1).targs;}}_LL144: {
# 1138
struct Cyc_Absyn_Datatypedecl*_tmp24D=*Cyc_Tcenv_lookup_datatypedecl(_tmp20E,loc,_tmp24B->name);
struct Cyc_List_List*fields=0;
if(_tmp24D->fields == 0){
const char*_tmp7CB;void*_tmp7CA[1];struct Cyc_String_pa_PrintArg_struct _tmp7C9;(_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7CA[0]=& _tmp7C9,Cyc_Tcutil_terr(loc,((_tmp7CB="can't inject into abstract datatype %s",_tag_dyneither(_tmp7CB,sizeof(char),39))),_tag_dyneither(_tmp7CA,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp24D->fields))->v;}
# 1150
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp23F),Cyc_Tcenv_curr_rgn(_tmp20E))){
const char*_tmp7D0;void*_tmp7CF[2];struct Cyc_String_pa_PrintArg_struct _tmp7CE;struct Cyc_String_pa_PrintArg_struct _tmp7CD;(_tmp7CD.tag=0,((_tmp7CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp20E))),((_tmp7CE.tag=0,((_tmp7CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp23F)),((_tmp7CF[0]=& _tmp7CE,((_tmp7CF[1]=& _tmp7CD,Cyc_Tcutil_terr(loc,((_tmp7D0="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7D0,sizeof(char),49))),_tag_dyneither(_tmp7CF,sizeof(void*),2)))))))))))));}{
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
const char*_tmp7D3;void*_tmp7D2;(_tmp7D2=0,Cyc_Tcutil_terr(e1->loc,((_tmp7D3="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7D3,sizeof(char),49))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}}{
# 1166
struct Cyc_Absyn_Datatypefield*_tmp259=Cyc_Tcexp_tcInjection(_tmp20E,e1,Cyc_Tcutil_pointer_elt_type(_tmp23F),rgn,_tmp256,fields);
# 1168
if(_tmp259 != 0){
struct Cyc_List_List*_tmp7D4;_tmp241->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp241->injectors,(
(_tmp7D4=_cycalloc(sizeof(*_tmp7D4)),((_tmp7D4->hd=_tmp259,((_tmp7D4->tl=0,_tmp7D4)))))));}};};}}
# 1174
_npop_handler(0);goto _LL142;
# 1154
;_pop_region(rgn);};}_LL145:;_LL146:
# 1175
{const char*_tmp7D7;void*_tmp7D6;(_tmp7D6=0,Cyc_Tcutil_terr(loc,((_tmp7D7="bad inject vararg type",_tag_dyneither(_tmp7D7,sizeof(char),23))),_tag_dyneither(_tmp7D6,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1180
if(*alias_arg_exps == 0){
# 1189
Cyc_Tcenv_check_effect_accessible(_tmp20E,loc,(void*)_check_null(_tmp21A));
# 1193
Cyc_Tcenv_check_rgn_partial_order(_tmp20E,loc,_tmp220);}{
# 1195
void*_tmp25E=_tmp21C;_npop_handler(0);return _tmp25E;};};_LL133:;_LL134: {
const char*_tmp7DA;void*_tmp7D9;void*_tmp261=(_tmp7D9=0,Cyc_Tcexp_expr_err(_tmp20E,loc,topt,((_tmp7DA="expected pointer to function",_tag_dyneither(_tmp7DA,sizeof(char),29))),_tag_dyneither(_tmp7D9,sizeof(void*),0)));_npop_handler(0);return _tmp261;}_LL130:;};_LL12E:;_LL12F: {
# 1198
const char*_tmp7DD;void*_tmp7DC;void*_tmp264=(_tmp7DC=0,Cyc_Tcexp_expr_err(_tmp20E,loc,topt,((_tmp7DD="expected pointer to function",_tag_dyneither(_tmp7DD,sizeof(char),29))),_tag_dyneither(_tmp7DC,sizeof(void*),0)));_npop_handler(0);return _tmp264;}_LL12B:;};}
# 1015
;_pop_region(ter);}
# 1204
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp265=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp265,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp265,& bogus)){
const char*_tmp7E2;void*_tmp7E1[2];struct Cyc_String_pa_PrintArg_struct _tmp7E0;struct Cyc_String_pa_PrintArg_struct _tmp7DF;(_tmp7DF.tag=0,((_tmp7DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7E0.tag=0,((_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1209
Cyc_Absynpp_typ2string(_tmp265)),((_tmp7E1[0]=& _tmp7E0,((_tmp7E1[1]=& _tmp7DF,Cyc_Tcutil_terr(loc,((_tmp7E2="expected %s but found %s",_tag_dyneither(_tmp7E2,sizeof(char),25))),_tag_dyneither(_tmp7E1,sizeof(void*),2)))))))))))));}
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
struct _tuple13*_tmp7E5;struct Cyc_List_List*_tmp7E4;instantiation=(
(_tmp7E4=_region_malloc(temp,sizeof(*_tmp7E4)),((_tmp7E4->hd=((_tmp7E5=_region_malloc(temp,sizeof(*_tmp7E5)),((_tmp7E5->f1=(struct Cyc_Absyn_Tvar*)_tmp274->hd,((_tmp7E5->f2=(void*)ts->hd,_tmp7E5)))))),((_tmp7E4->tl=instantiation,_tmp7E4))))));};}
# 1236
if(ts != 0){
const char*_tmp7E8;void*_tmp7E7;void*_tmp282=
(_tmp7E7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E8="too many type variables in instantiation",_tag_dyneither(_tmp7E8,sizeof(char),41))),_tag_dyneither(_tmp7E7,sizeof(void*),0)));_npop_handler(0);return _tmp282;}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp7EE;struct Cyc_Absyn_FnInfo _tmp7ED;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7EC;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp7EC=_cycalloc(sizeof(*_tmp7EC)),((_tmp7EC[0]=((_tmp7EE.tag=9,((_tmp7EE.f1=((_tmp7ED.tvars=_tmp274,((_tmp7ED.effect=_tmp275,((_tmp7ED.ret_tqual=_tmp276,((_tmp7ED.ret_typ=_tmp277,((_tmp7ED.args=_tmp278,((_tmp7ED.c_varargs=_tmp279,((_tmp7ED.cyc_varargs=_tmp27A,((_tmp7ED.rgn_po=_tmp27B,((_tmp7ED.attributes=_tmp27C,_tmp7ED)))))))))))))))))),_tmp7EE)))),_tmp7EC)))));
# 1243
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7F8;struct Cyc_Absyn_PtrAtts _tmp7F7;struct Cyc_Absyn_PtrInfo _tmp7F6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F5;void*_tmp287=(void*)((_tmp7F5=_cycalloc(sizeof(*_tmp7F5)),((_tmp7F5[0]=((_tmp7F8.tag=5,((_tmp7F8.f1=((_tmp7F6.elt_typ=new_fn_typ,((_tmp7F6.elt_tq=_tmp26D,((_tmp7F6.ptr_atts=((_tmp7F7.rgn=_tmp26E,((_tmp7F7.nullable=_tmp26F,((_tmp7F7.bounds=_tmp270,((_tmp7F7.zero_term=_tmp271,((_tmp7F7.ptrloc=0,_tmp7F7)))))))))),_tmp7F6)))))),_tmp7F8)))),_tmp7F5))));_npop_handler(0);return _tmp287;};}
# 1227
;_pop_region(temp);}_LL14F:;_LL150:
# 1245
 goto _LL14C;_LL14C:;}
# 1247
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1250
const char*_tmp7FC;void*_tmp7FB[1];struct Cyc_String_pa_PrintArg_struct _tmp7FA;return(_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7FB[0]=& _tmp7FA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FC="expecting polymorphic type but found %s",_tag_dyneither(_tmp7FC,sizeof(char),40))),_tag_dyneither(_tmp7FB,sizeof(void*),1)))))));};};}
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
const char*_tmp801;void*_tmp800[2];struct Cyc_String_pa_PrintArg_struct _tmp7FF;struct Cyc_String_pa_PrintArg_struct _tmp7FE;void*_tmp28E=(_tmp7FE.tag=0,((_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp7FF.tag=0,((_tmp7FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp800[0]=& _tmp7FF,((_tmp800[1]=& _tmp7FE,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp801="cannot cast %s to %s",_tag_dyneither(_tmp801,sizeof(char),21))),_tag_dyneither(_tmp800,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp28E;};}}}
# 1282
{struct _tuple0 _tmp802;struct _tuple0 _tmp294=(_tmp802.f1=e->r,((_tmp802.f2=Cyc_Tcutil_compress(t),_tmp802)));int _tmp296;union Cyc_Absyn_Constraint*_tmp298;union Cyc_Absyn_Constraint*_tmp299;union Cyc_Absyn_Constraint*_tmp29A;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp294.f1;if(_tmp295->tag != 32)goto _LL154;else{_tmp296=(_tmp295->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp297=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp294.f2;if(_tmp297->tag != 5)goto _LL154;else{_tmp298=((_tmp297->f1).ptr_atts).nullable;_tmp299=((_tmp297->f1).ptr_atts).bounds;_tmp29A=((_tmp297->f1).ptr_atts).zero_term;}};_LL153:
# 1286
 if((_tmp296  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp29A)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp298)){
void*_tmp29B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp299);struct Cyc_Absyn_Exp*_tmp29D;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B;if(_tmp29C->tag != 1)goto _LL159;else{_tmp29D=_tmp29C->f1;}}_LL158:
# 1289
 if((Cyc_Evexp_eval_const_uint_exp(_tmp29D)).f1 == 1){
const char*_tmp805;void*_tmp804;(_tmp804=0,Cyc_Tcutil_warn(loc,((_tmp805="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp805,sizeof(char),60))),_tag_dyneither(_tmp804,sizeof(void*),0)));}
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
const char*_tmp808;void*_tmp807;(_tmp807=0,Cyc_Tcutil_terr(e->loc,((_tmp808="Cannot take the address of an alias-free path",_tag_dyneither(_tmp808,sizeof(char),46))),_tag_dyneither(_tmp807,sizeof(void*),0)));}
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
{const char*_tmp80B;void*_tmp80A;(_tmp80A=0,Cyc_Tcutil_terr(e->loc,((_tmp80B="cannot take the address of a @tagged union member",_tag_dyneither(_tmp80B,sizeof(char),50))),_tag_dyneither(_tmp80A,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1361
int _tmp2B8;void*_tmp2B9;struct _tuple14 _tmp2B7=Cyc_Tcutil_addressof_props(te,e);_tmp2B8=_tmp2B7.f1;_tmp2B9=_tmp2B7.f2;
if(Cyc_Tcutil_is_noalias_region(_tmp2B9,0)){
const char*_tmp80E;void*_tmp80D;(_tmp80D=0,Cyc_Tcutil_terr(e->loc,((_tmp80E="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp80E,sizeof(char),60))),_tag_dyneither(_tmp80D,sizeof(void*),0)));}{
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
const char*_tmp812;void*_tmp811[1];struct Cyc_String_pa_PrintArg_struct _tmp810;(_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp811[0]=& _tmp810,Cyc_Tcutil_terr(loc,((_tmp812="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp812,sizeof(char),55))),_tag_dyneither(_tmp811,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp2C0=Cyc_Tcutil_compress(*topt);void*_tmp2C1;void*_tmp2C3;_LL172: _tmp2C1=_tmp2C0;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2C1;if(_tmp2C2->tag != 19)goto _LL174;else{_tmp2C3=(void*)_tmp2C2->f1;}};_LL173: {
# 1393
struct Cyc_Absyn_Exp*_tmp2C4=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp815;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp814;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C5=(_tmp814=_cycalloc(sizeof(*_tmp814)),((_tmp814[0]=((_tmp815.tag=18,((_tmp815.f1=_tmp2C4,_tmp815)))),_tmp814)));
if(Cyc_Tcutil_unify(_tmp2C3,(void*)_tmp2C5))return _tmp2C1;
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
struct Cyc_List_List*_tmp2C8=fs;
void*_tmp2C9=t_orig;
for(0;_tmp2C8 != 0;_tmp2C8=_tmp2C8->tl){
void*_tmp2CA=(void*)_tmp2C8->hd;
void*_tmp2CB=_tmp2CA;struct _dyneither_ptr*_tmp2CD;unsigned int _tmp2CF;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2CC=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2CB;if(_tmp2CC->tag != 0)goto _LL179;else{_tmp2CD=_tmp2CC->f1;}}_LL178: {
# 1419
int bad_type=1;
{void*_tmp2D0=Cyc_Tcutil_compress(_tmp2C9);struct Cyc_Absyn_Aggrdecl**_tmp2D2;struct Cyc_List_List*_tmp2D4;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2D1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D0;if(_tmp2D1->tag != 11)goto _LL17E;else{if((((_tmp2D1->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp2D2=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2D1->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1422
 if((*_tmp2D2)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2D2)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp819;void*_tmp818[1];struct Cyc_String_pa_PrintArg_struct _tmp817;(_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CD),((_tmp818[0]=& _tmp817,Cyc_Tcutil_terr(loc,((_tmp819="no field of struct/union has name %s",_tag_dyneither(_tmp819,sizeof(char),37))),_tag_dyneither(_tmp818,sizeof(void*),1)))))));}else{
# 1427
_tmp2C9=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D0;if(_tmp2D3->tag != 12)goto _LL180;else{_tmp2D4=_tmp2D3->f2;}}_LL17F: {
# 1431
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2CD,_tmp2D4);
if(!((unsigned int)t2)){
const char*_tmp81D;void*_tmp81C[1];struct Cyc_String_pa_PrintArg_struct _tmp81B;(_tmp81B.tag=0,((_tmp81B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CD),((_tmp81C[0]=& _tmp81B,Cyc_Tcutil_terr(loc,((_tmp81D="no field of struct/union has name %s",_tag_dyneither(_tmp81D,sizeof(char),37))),_tag_dyneither(_tmp81C,sizeof(void*),1)))))));}else{
# 1435
_tmp2C9=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1440
if(bad_type){
if(_tmp2C8 == fs){
const char*_tmp821;void*_tmp820[1];struct Cyc_String_pa_PrintArg_struct _tmp81F;(_tmp81F.tag=0,((_tmp81F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C9)),((_tmp820[0]=& _tmp81F,Cyc_Tcutil_terr(loc,((_tmp821="%s is not a known struct/union type",_tag_dyneither(_tmp821,sizeof(char),36))),_tag_dyneither(_tmp820,sizeof(void*),1)))))));}else{
# 1444
const char*_tmp825;void*_tmp824[1];struct Cyc_String_pa_PrintArg_struct _tmp823;struct _dyneither_ptr _tmp2DE=(_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp824[0]=& _tmp823,Cyc_aprintf(((_tmp825="(%s)",_tag_dyneither(_tmp825,sizeof(char),5))),_tag_dyneither(_tmp824,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2C8;x=x->tl){
void*_tmp2DF=(void*)((struct Cyc_List_List*)_check_null(x))->hd;struct _dyneither_ptr*_tmp2E1;unsigned int _tmp2E3;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2E0=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2DF;if(_tmp2E0->tag != 0)goto _LL185;else{_tmp2E1=_tmp2E0->f1;}}_LL184:
# 1449
{const char*_tmp82A;void*_tmp829[2];struct Cyc_String_pa_PrintArg_struct _tmp828;struct Cyc_String_pa_PrintArg_struct _tmp827;_tmp2DE=((_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E1),((_tmp828.tag=0,((_tmp828.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DE),((_tmp829[0]=& _tmp828,((_tmp829[1]=& _tmp827,Cyc_aprintf(((_tmp82A="%s.%s",_tag_dyneither(_tmp82A,sizeof(char),6))),_tag_dyneither(_tmp829,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2E2=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2DF;if(_tmp2E2->tag != 1)goto _LL182;else{_tmp2E3=_tmp2E2->f1;}}_LL186:
# 1451
{const char*_tmp82F;void*_tmp82E[2];struct Cyc_String_pa_PrintArg_struct _tmp82D;struct Cyc_Int_pa_PrintArg_struct _tmp82C;_tmp2DE=((_tmp82C.tag=1,((_tmp82C.f1=(unsigned long)((int)_tmp2E3),((_tmp82D.tag=0,((_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DE),((_tmp82E[0]=& _tmp82D,((_tmp82E[1]=& _tmp82C,Cyc_aprintf(((_tmp82F="%s.%d",_tag_dyneither(_tmp82F,sizeof(char),6))),_tag_dyneither(_tmp82E,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1453
const char*_tmp834;void*_tmp833[2];struct Cyc_String_pa_PrintArg_struct _tmp832;struct Cyc_String_pa_PrintArg_struct _tmp831;(_tmp831.tag=0,((_tmp831.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C9)),((_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DE),((_tmp833[0]=& _tmp832,((_tmp833[1]=& _tmp831,Cyc_Tcutil_terr(loc,((_tmp834="%s == %s is not a struct/union type",_tag_dyneither(_tmp834,sizeof(char),36))),_tag_dyneither(_tmp833,sizeof(void*),2)))))))))))));};}}
# 1456
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2CE=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2CB;if(_tmp2CE->tag != 1)goto _LL176;else{_tmp2CF=_tmp2CE->f1;}}_LL17A: {
# 1458
int bad_type=1;
{void*_tmp2F3=Cyc_Tcutil_compress(_tmp2C9);struct Cyc_Absyn_Aggrdecl**_tmp2F5;struct Cyc_List_List*_tmp2F7;struct Cyc_List_List*_tmp2F9;struct Cyc_Absyn_Datatypefield*_tmp2FB;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2F4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F3;if(_tmp2F4->tag != 11)goto _LL18A;else{if((((_tmp2F4->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp2F5=(struct Cyc_Absyn_Aggrdecl**)(((_tmp2F4->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1461
 if((*_tmp2F5)->impl == 0)
goto _LL187;
_tmp2F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2F5)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2F3;if(_tmp2F6->tag != 12)goto _LL18C;else{_tmp2F7=_tmp2F6->f2;}}_LL18B:
# 1465
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F7)<= _tmp2CF){
const char*_tmp839;void*_tmp838[2];struct Cyc_Int_pa_PrintArg_struct _tmp837;struct Cyc_Int_pa_PrintArg_struct _tmp836;(_tmp836.tag=1,((_tmp836.f1=(unsigned long)((int)_tmp2CF),((_tmp837.tag=1,((_tmp837.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F7),((_tmp838[0]=& _tmp837,((_tmp838[1]=& _tmp836,Cyc_Tcutil_terr(loc,((_tmp839="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp839,sizeof(char),46))),_tag_dyneither(_tmp838,sizeof(void*),2)))))))))))));}else{
# 1469
_tmp2C9=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F7,(int)_tmp2CF))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F3;if(_tmp2F8->tag != 10)goto _LL18E;else{_tmp2F9=_tmp2F8->f1;}}_LL18D:
# 1473
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F9)<= _tmp2CF){
const char*_tmp83E;void*_tmp83D[2];struct Cyc_Int_pa_PrintArg_struct _tmp83C;struct Cyc_Int_pa_PrintArg_struct _tmp83B;(_tmp83B.tag=1,((_tmp83B.f1=(unsigned long)((int)_tmp2CF),((_tmp83C.tag=1,((_tmp83C.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2F9),((_tmp83D[0]=& _tmp83C,((_tmp83D[1]=& _tmp83B,Cyc_Tcutil_terr(loc,((_tmp83E="tuple has too few components: %d <= %d",_tag_dyneither(_tmp83E,sizeof(char),39))),_tag_dyneither(_tmp83D,sizeof(void*),2)))))))))))));}else{
# 1477
_tmp2C9=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2F9,(int)_tmp2CF)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2F3;if(_tmp2FA->tag != 4)goto _LL190;else{if((((_tmp2FA->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp2FB=((struct _tuple2)(((_tmp2FA->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1481
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FB->typs)< _tmp2CF){
const char*_tmp843;void*_tmp842[2];struct Cyc_Int_pa_PrintArg_struct _tmp841;struct Cyc_Int_pa_PrintArg_struct _tmp840;(_tmp840.tag=1,((_tmp840.f1=(unsigned long)((int)_tmp2CF),((_tmp841.tag=1,((_tmp841.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FB->typs),((_tmp842[0]=& _tmp841,((_tmp842[1]=& _tmp840,Cyc_Tcutil_terr(loc,((_tmp843="datatype field has too few components: %d < %d",_tag_dyneither(_tmp843,sizeof(char),47))),_tag_dyneither(_tmp842,sizeof(void*),2)))))))))))));}else{
# 1485
if(_tmp2CF != 0)
_tmp2C9=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp2FB->typs,(int)(_tmp2CF - 1))).f2;else{
if(_tmp2C8->tl != 0){
const char*_tmp846;void*_tmp845;(_tmp845=0,Cyc_Tcutil_terr(loc,((_tmp846="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp846,sizeof(char),68))),_tag_dyneither(_tmp845,sizeof(void*),0)));}}}
# 1490
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1494
if(bad_type){
const char*_tmp84A;void*_tmp849[1];struct Cyc_String_pa_PrintArg_struct _tmp848;(_tmp848.tag=0,((_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C9)),((_tmp849[0]=& _tmp848,Cyc_Tcutil_terr(loc,((_tmp84A="%s is not a known type",_tag_dyneither(_tmp84A,sizeof(char),23))),_tag_dyneither(_tmp849,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1499
return Cyc_Absyn_uint_typ;};}
# 1503
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp30D=_new_region("r");struct _RegionHandle*r=& _tmp30D;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp30E=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp30E,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp30F=t;void*_tmp312;void*_tmp313;union Cyc_Absyn_Constraint*_tmp314;union Cyc_Absyn_Constraint*_tmp315;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30F;if(_tmp310->tag != 1)goto _LL195;}_LL194: {
# 1510
struct Cyc_List_List*_tmp316=Cyc_Tcenv_lookup_type_vars(_tmp30E);
struct Cyc_Core_Opt*_tmp84B;void*_tmp317=Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp84B=_cycalloc(sizeof(*_tmp84B)),((_tmp84B->v=_tmp316,_tmp84B)))));
struct Cyc_Core_Opt*_tmp84C;void*_tmp318=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C->v=_tmp316,_tmp84C)))));
union Cyc_Absyn_Constraint*_tmp319=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp31A=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp84D;struct Cyc_Absyn_PtrAtts _tmp31B=(_tmp84D.rgn=_tmp318,((_tmp84D.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp84D.bounds=_tmp319,((_tmp84D.zero_term=_tmp31A,((_tmp84D.ptrloc=0,_tmp84D)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp853;struct Cyc_Absyn_PtrInfo _tmp852;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp851;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31C=(_tmp851=_cycalloc(sizeof(*_tmp851)),((_tmp851[0]=((_tmp853.tag=5,((_tmp853.f1=((_tmp852.elt_typ=_tmp317,((_tmp852.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp852.ptr_atts=_tmp31B,_tmp852)))))),_tmp853)))),_tmp851)));
Cyc_Tcutil_unify(t,(void*)_tmp31C);
_tmp312=_tmp317;_tmp313=_tmp318;_tmp314=_tmp319;_tmp315=_tmp31A;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp311=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F;if(_tmp311->tag != 5)goto _LL197;else{_tmp312=(_tmp311->f1).elt_typ;_tmp313=((_tmp311->f1).ptr_atts).rgn;_tmp314=((_tmp311->f1).ptr_atts).bounds;_tmp315=((_tmp311->f1).ptr_atts).zero_term;}}_LL196:
# 1520
 Cyc_Tcenv_check_rgn_accessible(_tmp30E,loc,_tmp313);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp314);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp312),& Cyc_Tcutil_tmk)){
const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_terr(loc,((_tmp856="can't dereference abstract pointer type",_tag_dyneither(_tmp856,sizeof(char),40))),_tag_dyneither(_tmp855,sizeof(void*),0)));}{
void*_tmp325=_tmp312;_npop_handler(0);return _tmp325;};_LL197:;_LL198: {
# 1526
const char*_tmp85A;void*_tmp859[1];struct Cyc_String_pa_PrintArg_struct _tmp858;void*_tmp329=(_tmp858.tag=0,((_tmp858.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp859[0]=& _tmp858,Cyc_Tcexp_expr_err(_tmp30E,loc,topt,((_tmp85A="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp85A,sizeof(char),39))),_tag_dyneither(_tmp859,sizeof(void*),1)))))));_npop_handler(0);return _tmp329;}_LL192:;};}
# 1505
;_pop_region(r);}
# 1532
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1536
struct _RegionHandle _tmp32A=_new_region("r");struct _RegionHandle*r=& _tmp32A;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1540
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp32B=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_Aggrdecl*_tmp32D;struct Cyc_List_List*_tmp32E;enum Cyc_Absyn_AggrKind _tmp330;struct Cyc_List_List*_tmp331;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 11)goto _LL19C;else{if((((_tmp32C->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp32D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp32C->f1).aggr_info).KnownAggr).val);_tmp32E=(_tmp32C->f1).targs;}}_LL19B: {
# 1543
struct Cyc_Absyn_Aggrfield*_tmp336=Cyc_Absyn_lookup_decl_field(_tmp32D,f);
if(_tmp336 == 0){
const char*_tmp85F;void*_tmp85E[2];struct Cyc_String_pa_PrintArg_struct _tmp85D;struct Cyc_String_pa_PrintArg_struct _tmp85C;void*_tmp33B=(_tmp85C.tag=0,((_tmp85C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp32D->name)),((_tmp85E[0]=& _tmp85D,((_tmp85E[1]=& _tmp85C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp85F="type %s has no %s field",_tag_dyneither(_tmp85F,sizeof(char),24))),_tag_dyneither(_tmp85E,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp33B;}
# 1548
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32D->impl))->tagged)*is_tagged=1;{
void*t2=_tmp336->type;
if(_tmp32E != 0){
struct _RegionHandle _tmp33C=_new_region("rgn");struct _RegionHandle*rgn=& _tmp33C;_push_region(rgn);
{struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp32D->tvs,_tmp32E);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp33D,_tmp336->type);}
# 1552
;_pop_region(rgn);}
# 1557
if(((_tmp32D->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32D->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_PrintArg_struct _tmp861;void*_tmp341=(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp862[0]=& _tmp861,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp863="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp863,sizeof(char),56))),_tag_dyneither(_tmp862,sizeof(void*),1)))))));_npop_handler(0);return _tmp341;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp32D->impl))->exist_vars != 0){
# 1562
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp867;void*_tmp866[1];struct Cyc_String_pa_PrintArg_struct _tmp865;void*_tmp345=(_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp866[0]=& _tmp865,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp867="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp867,sizeof(char),81))),_tag_dyneither(_tmp866,sizeof(void*),1)))))));_npop_handler(0);return _tmp345;}}{
# 1565
void*_tmp346=t2;_npop_handler(0);return _tmp346;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32B;if(_tmp32F->tag != 12)goto _LL19E;else{_tmp330=_tmp32F->f1;_tmp331=_tmp32F->f2;}}_LL19D: {
# 1567
struct Cyc_Absyn_Aggrfield*_tmp347=Cyc_Absyn_lookup_field(_tmp331,f);
if(_tmp347 == 0){
const char*_tmp86B;void*_tmp86A[1];struct Cyc_String_pa_PrintArg_struct _tmp869;void*_tmp34B=(_tmp869.tag=0,((_tmp869.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp86A[0]=& _tmp869,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86B="type has no %s field",_tag_dyneither(_tmp86B,sizeof(char),21))),_tag_dyneither(_tmp86A,sizeof(void*),1)))))));_npop_handler(0);return _tmp34B;}
if((_tmp330 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp347->type)){
const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_PrintArg_struct _tmp86D;void*_tmp34F=(_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp86E[0]=& _tmp86D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86F="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp86F,sizeof(char),56))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))));_npop_handler(0);return _tmp34F;}{
void*_tmp350=_tmp347->type;_npop_handler(0);return _tmp350;};}_LL19E:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32B;if(_tmp332->tag != 8)goto _LL1A0;}{const char*_tmp870;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp870="size",_tag_dyneither(_tmp870,sizeof(char),5))))== 0))goto _LL1A0;};_LL19F: goto _LL1A1;_LL1A0:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp32B;if(_tmp334->tag != 5)goto _LL1A2;}{const char*_tmp871;if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp871="size",_tag_dyneither(_tmp871,sizeof(char),5))))== 0))goto _LL1A2;};_LL1A1:
{const char*_tmp875;void*_tmp874[1];struct Cyc_String_pa_PrintArg_struct _tmp873;(_tmp873.tag=0,((_tmp873.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp874[0]=& _tmp873,Cyc_Tcutil_warn(e->loc,((_tmp875="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp875,sizeof(char),54))),_tag_dyneither(_tmp874,sizeof(void*),1)))))));}
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp87B;struct Cyc_List_List*_tmp87A;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp879;outer_e->r=(void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((_tmp87B.tag=2,((_tmp87B.f1=Cyc_Absyn_Numelts,((_tmp87B.f2=((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A->hd=e,((_tmp87A->tl=0,_tmp87A)))))),_tmp87B)))))),_tmp879))));}{
void*_tmp357=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp357;};_LL1A2:;_LL1A3: {
# 1579
const char*_tmp87C;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp87C="size",_tag_dyneither(_tmp87C,sizeof(char),5))))== 0){
const char*_tmp880;void*_tmp87F[1];struct Cyc_String_pa_PrintArg_struct _tmp87E;void*_tmp35C=(_tmp87E.tag=0,((_tmp87E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1582
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp87F[0]=& _tmp87E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp880="expecting struct, union, or array, found %s",_tag_dyneither(_tmp880,sizeof(char),44))),_tag_dyneither(_tmp87F,sizeof(void*),1)))))));_npop_handler(0);return _tmp35C;}else{
# 1584
const char*_tmp884;void*_tmp883[1];struct Cyc_String_pa_PrintArg_struct _tmp882;void*_tmp360=(_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp883[0]=& _tmp882,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp884="expecting struct or union, found %s",_tag_dyneither(_tmp884,sizeof(char),36))),_tag_dyneither(_tmp883,sizeof(void*),1)))))));_npop_handler(0);return _tmp360;}}_LL199:;};
# 1536
;_pop_region(r);}
# 1590
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1593
struct _RegionHandle _tmp361=_new_region("r");struct _RegionHandle*r=& _tmp361;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1597
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp362=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp364;void*_tmp365;union Cyc_Absyn_Constraint*_tmp366;union Cyc_Absyn_Constraint*_tmp367;_LL1A5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362;if(_tmp363->tag != 5)goto _LL1A7;else{_tmp364=(_tmp363->f1).elt_typ;_tmp365=((_tmp363->f1).ptr_atts).rgn;_tmp366=((_tmp363->f1).ptr_atts).bounds;_tmp367=((_tmp363->f1).ptr_atts).zero_term;}}_LL1A6:
# 1600
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp366);
{void*_tmp368=Cyc_Tcutil_compress(_tmp364);struct Cyc_Absyn_Aggrdecl*_tmp36A;struct Cyc_List_List*_tmp36B;enum Cyc_Absyn_AggrKind _tmp36D;struct Cyc_List_List*_tmp36E;_LL1AA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp368;if(_tmp369->tag != 11)goto _LL1AC;else{if((((_tmp369->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AC;_tmp36A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp369->f1).aggr_info).KnownAggr).val);_tmp36B=(_tmp369->f1).targs;}}_LL1AB: {
# 1603
struct Cyc_Absyn_Aggrfield*_tmp36F=Cyc_Absyn_lookup_decl_field(_tmp36A,f);
if(_tmp36F == 0){
const char*_tmp889;void*_tmp888[2];struct Cyc_String_pa_PrintArg_struct _tmp887;struct Cyc_String_pa_PrintArg_struct _tmp886;void*_tmp374=(_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp887.tag=0,((_tmp887.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp36A->name)),((_tmp888[0]=& _tmp887,((_tmp888[1]=& _tmp886,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp889="type %s has no %s field",_tag_dyneither(_tmp889,sizeof(char),24))),_tag_dyneither(_tmp888,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp374;}
# 1608
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36A->impl))->tagged)*is_tagged=1;{
void*t3=_tmp36F->type;
if(_tmp36B != 0){
struct _RegionHandle _tmp375=_new_region("rgn");struct _RegionHandle*rgn=& _tmp375;_push_region(rgn);
{struct Cyc_List_List*_tmp376=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp36A->tvs,_tmp36B);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp376,_tmp36F->type);}
# 1612
;_pop_region(rgn);}{
# 1616
struct Cyc_Absyn_Kind*_tmp377=Cyc_Tcutil_typ_kind(t3);
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp377)){
# 1619
void*_tmp378=Cyc_Tcutil_compress(t3);_LL1B1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp379=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp378;if(_tmp379->tag != 8)goto _LL1B3;}_LL1B2:
 goto _LL1B0;_LL1B3:;_LL1B4: {
# 1622
const char*_tmp88D;void*_tmp88C[1];struct Cyc_String_pa_PrintArg_struct _tmp88B;void*_tmp37D=(_tmp88B.tag=0,((_tmp88B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp88C[0]=& _tmp88B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88D="cannot get member %s since its type is abstract",_tag_dyneither(_tmp88D,sizeof(char),48))),_tag_dyneither(_tmp88C,sizeof(void*),1)))))));_npop_handler(0);return _tmp37D;}_LL1B0:;}
# 1625
if(((_tmp36A->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36A->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
const char*_tmp891;void*_tmp890[1];struct Cyc_String_pa_PrintArg_struct _tmp88F;void*_tmp381=(_tmp88F.tag=0,((_tmp88F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp890[0]=& _tmp88F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp891="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp891,sizeof(char),56))),_tag_dyneither(_tmp890,sizeof(void*),1)))))));_npop_handler(0);return _tmp381;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36A->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp895;void*_tmp894[1];struct Cyc_String_pa_PrintArg_struct _tmp893;void*_tmp385=(_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp894[0]=& _tmp893,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp895="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp895,sizeof(char),72))),_tag_dyneither(_tmp894,sizeof(void*),1)))))));_npop_handler(0);return _tmp385;}}{
# 1633
void*_tmp386=t3;_npop_handler(0);return _tmp386;};};};}_LL1AC: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp368;if(_tmp36C->tag != 12)goto _LL1AE;else{_tmp36D=_tmp36C->f1;_tmp36E=_tmp36C->f2;}}_LL1AD: {
# 1635
struct Cyc_Absyn_Aggrfield*_tmp387=Cyc_Absyn_lookup_field(_tmp36E,f);
if(_tmp387 == 0){
const char*_tmp899;void*_tmp898[1];struct Cyc_String_pa_PrintArg_struct _tmp897;void*_tmp38B=(_tmp897.tag=0,((_tmp897.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp898[0]=& _tmp897,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp899="type has no %s field",_tag_dyneither(_tmp899,sizeof(char),21))),_tag_dyneither(_tmp898,sizeof(void*),1)))))));_npop_handler(0);return _tmp38B;}
if((_tmp36D == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp387->type)){
const char*_tmp89D;void*_tmp89C[1];struct Cyc_String_pa_PrintArg_struct _tmp89B;void*_tmp38F=(_tmp89B.tag=0,((_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp89C[0]=& _tmp89B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89D="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp89D,sizeof(char),56))),_tag_dyneither(_tmp89C,sizeof(void*),1)))))));_npop_handler(0);return _tmp38F;}{
void*_tmp390=_tmp387->type;_npop_handler(0);return _tmp390;};}_LL1AE:;_LL1AF:
 goto _LL1A9;_LL1A9:;}
# 1643
goto _LL1A4;_LL1A7:;_LL1A8:
 goto _LL1A4;_LL1A4:;}{
# 1646
const char*_tmp8A1;void*_tmp8A0[1];struct Cyc_String_pa_PrintArg_struct _tmp89F;void*_tmp394=(_tmp89F.tag=0,((_tmp89F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8A0[0]=& _tmp89F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A1="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8A1,sizeof(char),44))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))));_npop_handler(0);return _tmp394;};
# 1593
;_pop_region(r);}
# 1651
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1653
unsigned int _tmp396;int _tmp397;struct _tuple15 _tmp395=Cyc_Evexp_eval_const_uint_exp(index);_tmp396=_tmp395.f1;_tmp397=_tmp395.f2;
if(!_tmp397){
const char*_tmp8A4;void*_tmp8A3;return(_tmp8A3=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8A4="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8A4,sizeof(char),47))),_tag_dyneither(_tmp8A3,sizeof(void*),0)));}{
# 1657
struct _handler_cons _tmp39A;_push_handler(& _tmp39A);{int _tmp39C=0;if(setjmp(_tmp39A.handler))_tmp39C=1;if(!_tmp39C){
{void*_tmp39D=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp396)).f2;_npop_handler(0);return _tmp39D;};_pop_handler();}else{void*_tmp39B=(void*)_exn_thrown;void*_tmp39F=_tmp39B;void*_tmp3A1;_LL1B6: {struct Cyc_List_Nth_exn_struct*_tmp3A0=(struct Cyc_List_Nth_exn_struct*)_tmp39F;if(_tmp3A0->tag != Cyc_List_Nth)goto _LL1B8;}_LL1B7: {
# 1660
const char*_tmp8A9;void*_tmp8A8[2];struct Cyc_Int_pa_PrintArg_struct _tmp8A7;struct Cyc_Int_pa_PrintArg_struct _tmp8A6;return(_tmp8A6.tag=1,((_tmp8A6.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8A7.tag=1,((_tmp8A7.f1=(unsigned long)((int)_tmp396),((_tmp8A8[0]=& _tmp8A7,((_tmp8A8[1]=& _tmp8A6,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8A9="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8A9,sizeof(char),41))),_tag_dyneither(_tmp8A8,sizeof(void*),2)))))))))))));}_LL1B8: _tmp3A1=_tmp39F;_LL1B9:(void)_throw(_tmp3A1);_LL1B5:;}};};}
# 1665
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1667
struct _RegionHandle _tmp3A6=_new_region("r");struct _RegionHandle*r=& _tmp3A6;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3A7=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp3A7,0,e1);
Cyc_Tcexp_tcExp(_tmp3A7,0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3A7,e2)){
const char*_tmp8AD;void*_tmp8AC[1];struct Cyc_String_pa_PrintArg_struct _tmp8AB;void*_tmp3AB=(_tmp8AB.tag=0,((_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8AC[0]=& _tmp8AB,Cyc_Tcexp_expr_err(_tmp3A7,e2->loc,topt,((_tmp8AD="expecting int subscript, found %s",_tag_dyneither(_tmp8AD,sizeof(char),34))),_tag_dyneither(_tmp8AC,sizeof(void*),1)))))));_npop_handler(0);return _tmp3AB;}{
# 1678
void*_tmp3AC=t1;void*_tmp3AE;struct Cyc_Absyn_Tqual _tmp3AF;void*_tmp3B0;union Cyc_Absyn_Constraint*_tmp3B1;union Cyc_Absyn_Constraint*_tmp3B2;struct Cyc_List_List*_tmp3B4;_LL1BB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC;if(_tmp3AD->tag != 5)goto _LL1BD;else{_tmp3AE=(_tmp3AD->f1).elt_typ;_tmp3AF=(_tmp3AD->f1).elt_tq;_tmp3B0=((_tmp3AD->f1).ptr_atts).rgn;_tmp3B1=((_tmp3AD->f1).ptr_atts).bounds;_tmp3B2=((_tmp3AD->f1).ptr_atts).zero_term;}}_LL1BC:
# 1682
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B2)){
int emit_warning=1;
{void*_tmp3B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3B1);struct Cyc_Absyn_Exp*_tmp3B7;_LL1C2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3B5;if(_tmp3B6->tag != 1)goto _LL1C4;else{_tmp3B7=_tmp3B6->f1;}}_LL1C3:
# 1686
 if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3BA;int _tmp3BB;struct _tuple15 _tmp3B9=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3BA=_tmp3B9.f1;_tmp3BB=_tmp3B9.f2;
if(_tmp3BB){
unsigned int _tmp3BD;int _tmp3BE;struct _tuple15 _tmp3BC=Cyc_Evexp_eval_const_uint_exp(_tmp3B7);_tmp3BD=_tmp3BC.f1;_tmp3BE=_tmp3BC.f2;
if(_tmp3BE  && _tmp3BD > _tmp3BA)emit_warning=0;}}
# 1693
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3B8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B5;if(_tmp3B8->tag != 0)goto _LL1C1;}_LL1C5:
 emit_warning=0;goto _LL1C1;_LL1C1:;}
# 1696
if(emit_warning){
const char*_tmp8B0;void*_tmp8AF;(_tmp8AF=0,Cyc_Tcutil_warn(e2->loc,((_tmp8B0="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8B0,sizeof(char),63))),_tag_dyneither(_tmp8AF,sizeof(void*),0)));}}else{
# 1700
if(Cyc_Tcutil_is_const_exp(e2)){
unsigned int _tmp3C2;int _tmp3C3;struct _tuple15 _tmp3C1=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3C2=_tmp3C1.f1;_tmp3C3=_tmp3C1.f2;
if(_tmp3C3)
Cyc_Tcutil_check_bound(loc,_tmp3C2,_tmp3B1);}else{
# 1706
if(Cyc_Tcutil_is_bound_one(_tmp3B1) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B2)){
const char*_tmp8B3;void*_tmp8B2;(_tmp8B2=0,Cyc_Tcutil_warn(e1->loc,((_tmp8B3="subscript applied to pointer to one object",_tag_dyneither(_tmp8B3,sizeof(char),43))),_tag_dyneither(_tmp8B2,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3B1);}}
# 1711
Cyc_Tcenv_check_rgn_accessible(_tmp3A7,loc,_tmp3B0);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3AE),& Cyc_Tcutil_tmk)){
const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_terr(e1->loc,((_tmp8B6="can't subscript an abstract pointer",_tag_dyneither(_tmp8B6,sizeof(char),36))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}{
void*_tmp3C8=_tmp3AE;_npop_handler(0);return _tmp3C8;};_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3AC;if(_tmp3B3->tag != 10)goto _LL1BF;else{_tmp3B4=_tmp3B3->f1;}}_LL1BE: {
void*_tmp3C9=Cyc_Tcexp_ithTupleType(_tmp3A7,loc,_tmp3B4,e2);_npop_handler(0);return _tmp3C9;}_LL1BF:;_LL1C0: {
const char*_tmp8BA;void*_tmp8B9[1];struct Cyc_String_pa_PrintArg_struct _tmp8B8;void*_tmp3CD=(_tmp8B8.tag=0,((_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8B9[0]=& _tmp8B8,Cyc_Tcexp_expr_err(_tmp3A7,loc,topt,((_tmp8BA="subscript applied to %s",_tag_dyneither(_tmp8BA,sizeof(char),24))),_tag_dyneither(_tmp8B9,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CD;}_LL1BA:;};};}
# 1668
;_pop_region(r);}
# 1722
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp3CE=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp3D0;_LL1C7: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3CF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3CE;if(_tmp3CF->tag != 10)goto _LL1C9;else{_tmp3D0=_tmp3CF->f1;}}_LL1C8:
# 1728
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3D0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1731
goto _LL1C6;
# 1733
for(0;es != 0;(es=es->tl,_tmp3D0=_tmp3D0->tl)){
int bogus=0;
void*_tmp3D1=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp3D0))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp3D1,(struct Cyc_Absyn_Exp*)es->hd);
# 1738
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp3D0->hd)).f2,& bogus);{
struct _tuple19*_tmp8BD;struct Cyc_List_List*_tmp8BC;fields=((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC->hd=((_tmp8BD=_cycalloc(sizeof(*_tmp8BD)),((_tmp8BD->f1=(*((struct _tuple19*)_tmp3D0->hd)).f1,((_tmp8BD->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8BD)))))),((_tmp8BC->tl=fields,_tmp8BC))))));};}
# 1741
done=1;
goto _LL1C6;_LL1C9:;_LL1CA:
 goto _LL1C6;_LL1C6:;}
# 1745
if(!done)
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple19*_tmp8C0;struct Cyc_List_List*_tmp8BF;fields=((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF->hd=((_tmp8C0=_cycalloc(sizeof(*_tmp8C0)),((_tmp8C0->f1=Cyc_Absyn_empty_tqual(0),((_tmp8C0->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8C0)))))),((_tmp8BF->tl=fields,_tmp8BF))))));};}{
# 1750
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8C3;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8C2;return(void*)((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2[0]=((_tmp8C3.tag=10,((_tmp8C3.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8C3)))),_tmp8C2))));};}
# 1754
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1757
const char*_tmp8C6;void*_tmp8C5;return(_tmp8C5=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8C6="tcCompoundLit",_tag_dyneither(_tmp8C6,sizeof(char),14))),_tag_dyneither(_tmp8C5,sizeof(void*),0)));}
# 1767
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1770
void*res_t2;
struct _RegionHandle _tmp3DA=_new_region("r");struct _RegionHandle*r=& _tmp3DA;_push_region(r);{
int _tmp3DB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8C9;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8C8;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3DC=(_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=0,((_tmp8C9.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3DB),_tmp8C9)))),_tmp8C8)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3DC,loc);
# 1779
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3DD=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3DB - 1);
if(!Cyc_Tcutil_is_zero(_tmp3DD)){
const char*_tmp8CC;void*_tmp8CB;(_tmp8CB=0,Cyc_Tcutil_terr(_tmp3DD->loc,((_tmp8CC="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8CC,sizeof(char),45))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}}
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
const char*_tmp8D0;void*_tmp8CF[1];struct Cyc_String_pa_PrintArg_struct _tmp8CE;(_tmp8CE.tag=0,((_tmp8CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8CF[0]=& _tmp8CE,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8D0="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8D0,sizeof(char),53))),_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));}}
# 1798
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1803
void*_tmp3E5=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp3E8;_LL1CC: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3E6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E5;if(_tmp3E6->tag != 1)goto _LL1CE;}_LL1CD:
# 1805
{const char*_tmp8D3;void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(loc,((_tmp8D3="only array index designators are supported",_tag_dyneither(_tmp8D3,sizeof(char),43))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3E7=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3E5;if(_tmp3E7->tag != 0)goto _LL1CB;else{_tmp3E8=_tmp3E7->f1;}}_LL1CF:
# 1808
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3E8);{
unsigned int _tmp3EC;int _tmp3ED;struct _tuple15 _tmp3EB=Cyc_Evexp_eval_const_uint_exp(_tmp3E8);_tmp3EC=_tmp3EB.f1;_tmp3ED=_tmp3EB.f2;
if(!_tmp3ED){
const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,Cyc_Tcutil_terr(_tmp3E8->loc,((_tmp8D6="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8D6,sizeof(char),47))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}else{
if(_tmp3EC != offset){
const char*_tmp8DB;void*_tmp8DA[2];struct Cyc_Int_pa_PrintArg_struct _tmp8D9;struct Cyc_Int_pa_PrintArg_struct _tmp8D8;(_tmp8D8.tag=1,((_tmp8D8.f1=(unsigned long)((int)_tmp3EC),((_tmp8D9.tag=1,((_tmp8D9.f1=(unsigned long)offset,((_tmp8DA[0]=& _tmp8D9,((_tmp8DA[1]=& _tmp8D8,Cyc_Tcutil_terr(_tmp3E8->loc,((_tmp8DB="expecting index designator %d but found %d",_tag_dyneither(_tmp8DB,sizeof(char),43))),_tag_dyneither(_tmp8DA,sizeof(void*),2)))))))))))));}}
# 1815
goto _LL1CB;};_LL1CB:;}}}{
# 1819
void*_tmp3F4=res_t2;_npop_handler(0);return _tmp3F4;};
# 1771
;_pop_region(r);}
# 1847
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1850
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp3F5=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp3F7;_LL1D1: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F5;if(_tmp3F6->tag != 19)goto _LL1D3;else{_tmp3F7=(void*)_tmp3F6->f1;}}_LL1D2:
# 1855
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp3F8=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3F7,0),0,Cyc_Absyn_No_coercion,0);
_tmp3F8->topt=bound->topt;
bound=_tmp3F8;}
# 1860
goto _LL1D0;_LL1D3:;_LL1D4:
# 1862
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp8DF;void*_tmp8DE[1];struct Cyc_String_pa_PrintArg_struct _tmp8DD;(_tmp8DD.tag=0,((_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8DE[0]=& _tmp8DD,Cyc_Tcutil_terr(bound->loc,((_tmp8DF="expecting unsigned int, found %s",_tag_dyneither(_tmp8DF,sizeof(char),33))),_tag_dyneither(_tmp8DE,sizeof(void*),1)))))));}_LL1D0:;}
# 1867
if(!(vd->tq).real_const){
const char*_tmp8E2;void*_tmp8E1;(_tmp8E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8E2="comprehension index variable is not declared const!",_tag_dyneither(_tmp8E2,sizeof(char),52))),_tag_dyneither(_tmp8E1,sizeof(void*),0)));}{
# 1870
struct _RegionHandle _tmp3FE=_new_region("r");struct _RegionHandle*r=& _tmp3FE;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1876
struct Cyc_Tcenv_Tenv*_tmp3FF=te2;
void**_tmp400=0;
struct Cyc_Absyn_Tqual*_tmp401=0;
union Cyc_Absyn_Constraint**_tmp402=0;
# 1881
if(topt != 0){
void*_tmp403=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp405;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;struct Cyc_Absyn_Exp*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;_LL1D6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp403;if(_tmp404->tag != 5)goto _LL1D8;else{_tmp405=_tmp404->f1;}}_LL1D7:
# 1884
{void**_tmp8E3;_tmp400=((_tmp8E3=_region_malloc(r,sizeof(*_tmp8E3)),((_tmp8E3[0]=_tmp405.elt_typ,_tmp8E3))));}
{struct Cyc_Absyn_Tqual*_tmp8E4;_tmp401=((_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=_tmp405.elt_tq,_tmp8E4))));}
{union Cyc_Absyn_Constraint**_tmp8E5;_tmp402=((_tmp8E5=_region_malloc(r,sizeof(*_tmp8E5)),((_tmp8E5[0]=(_tmp405.ptr_atts).zero_term,_tmp8E5))));}
goto _LL1D5;_LL1D8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp403;if(_tmp406->tag != 8)goto _LL1DA;else{_tmp407=(_tmp406->f1).elt_type;_tmp408=(_tmp406->f1).tq;_tmp409=(_tmp406->f1).num_elts;_tmp40A=(_tmp406->f1).zero_term;}}_LL1D9:
# 1889
{void**_tmp8E6;_tmp400=((_tmp8E6=_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6[0]=_tmp407,_tmp8E6))));}
{struct Cyc_Absyn_Tqual*_tmp8E7;_tmp401=((_tmp8E7=_region_malloc(r,sizeof(*_tmp8E7)),((_tmp8E7[0]=_tmp408,_tmp8E7))));}
{union Cyc_Absyn_Constraint**_tmp8E8;_tmp402=((_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8[0]=_tmp40A,_tmp8E8))));}
goto _LL1D5;_LL1DA:;_LL1DB:
# 1894
 goto _LL1D5;_LL1D5:;}{
# 1897
void*t=Cyc_Tcexp_tcExp(_tmp3FF,_tmp400,body);
if(_tmp3FF->le == 0){
# 1900
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_terr(bound->loc,((_tmp8EB="bound is not constant",_tag_dyneither(_tmp8EB,sizeof(char),22))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp8EE;void*_tmp8ED;(_tmp8ED=0,Cyc_Tcutil_terr(bound->loc,((_tmp8EE="body is not constant",_tag_dyneither(_tmp8EE,sizeof(char),21))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}}
# 1905
if(_tmp402 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp402)){
# 1908
struct Cyc_Absyn_Exp*_tmp415=Cyc_Absyn_uint_exp(1,0);_tmp415->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp415,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1913
void*_tmp416=Cyc_Absyn_array_typ(t,_tmp401 == 0?Cyc_Absyn_empty_tqual(0):*_tmp401,bound,
_tmp402 == 0?Cyc_Absyn_false_conref:*_tmp402,0);
void*_tmp417=_tmp416;_npop_handler(0);return _tmp417;};};};}
# 1871
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1929
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1933
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp8EF;adptr=((_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF[0]=ad,_tmp8EF))));};}else{
# 1939
{struct _handler_cons _tmp419;_push_handler(& _tmp419);{int _tmp41B=0;if(setjmp(_tmp419.handler))_tmp41B=1;if(!_tmp41B){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1939
;_pop_handler();}else{void*_tmp41A=(void*)_exn_thrown;void*_tmp41D=_tmp41A;void*_tmp41F;_LL1DD: {struct Cyc_Dict_Absent_exn_struct*_tmp41E=(struct Cyc_Dict_Absent_exn_struct*)_tmp41D;if(_tmp41E->tag != Cyc_Dict_Absent)goto _LL1DF;}_LL1DE:
# 1942
{const char*_tmp8F3;void*_tmp8F2[1];struct Cyc_String_pa_PrintArg_struct _tmp8F1;(_tmp8F1.tag=0,((_tmp8F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp8F2[0]=& _tmp8F1,Cyc_Tcutil_terr(loc,((_tmp8F3="unbound struct/union name %s",_tag_dyneither(_tmp8F3,sizeof(char),29))),_tag_dyneither(_tmp8F2,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1DF: _tmp41F=_tmp41D;_LL1E0:(void)_throw(_tmp41F);_LL1DC:;}};}
# 1945
*ad_opt=ad;
*tn=ad->name;}
# 1948
if(ad->impl == 0){
{const char*_tmp8FB;void*_tmp8FA[1];const char*_tmp8F9;const char*_tmp8F8;struct Cyc_String_pa_PrintArg_struct _tmp8F7;(_tmp8F7.tag=0,((_tmp8F7.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp8F8="struct",_tag_dyneither(_tmp8F8,sizeof(char),7)):((_tmp8F9="union",_tag_dyneither(_tmp8F9,sizeof(char),6)))),((_tmp8FA[0]=& _tmp8F7,Cyc_Tcutil_terr(loc,((_tmp8FB="can't construct abstract %s",_tag_dyneither(_tmp8FB,sizeof(char),28))),_tag_dyneither(_tmp8FA,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1952
struct _RegionHandle _tmp428=_new_region("rgn");struct _RegionHandle*rgn=& _tmp428;_push_region(rgn);
# 1957
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp429=Cyc_Tcenv_new_status(te);
if(_tmp429 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 1962
if(_tmp429 == Cyc_Tcenv_InNewAggr)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_NoneNew,te);else{
# 1966
te2=Cyc_Tcenv_set_new_status(rgn,_tmp429,te);}}{
# 1968
struct _tuple12 _tmp8FC;struct _tuple12 _tmp42A=(_tmp8FC.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp8FC.f2=rgn,_tmp8FC)));
struct Cyc_List_List*_tmp42B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp42A,ad->tvs);
struct Cyc_List_List*_tmp42C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp42A,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp42D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp42B);
struct Cyc_List_List*_tmp42E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp42C);
struct Cyc_List_List*_tmp42F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp42B,_tmp42C);
void*res_typ;
# 1979
if(topt != 0){
void*_tmp430=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_Aggrdecl**_tmp432;struct Cyc_List_List*_tmp433;_LL1E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp431=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp430;if(_tmp431->tag != 11)goto _LL1E4;else{if((((_tmp431->f1).aggr_info).KnownAggr).tag != 2)goto _LL1E4;_tmp432=(struct Cyc_Absyn_Aggrdecl**)(((_tmp431->f1).aggr_info).KnownAggr).val;_tmp433=(_tmp431->f1).targs;}}_LL1E3:
# 1982
 if(*_tmp432 == *adptr){
{struct Cyc_List_List*_tmp434=_tmp42D;for(0;_tmp434 != 0  && _tmp433 != 0;
(_tmp434=_tmp434->tl,_tmp433=_tmp433->tl)){
Cyc_Tcutil_unify((void*)_tmp434->hd,(void*)_tmp433->hd);}}
# 1987
res_typ=*topt;
goto _LL1E1;}
# 1990
goto _LL1E5;_LL1E4:;_LL1E5: {
# 1992
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp902;struct Cyc_Absyn_AggrInfo _tmp901;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp900;res_typ=(void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=((_tmp902.tag=11,((_tmp902.f1=((_tmp901.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp901.targs=_tmp42D,_tmp901)))),_tmp902)))),_tmp900))));}_LL1E1:;}else{
# 1995
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp908;struct Cyc_Absyn_AggrInfo _tmp907;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp906;res_typ=(void*)((_tmp906=_cycalloc(sizeof(*_tmp906)),((_tmp906[0]=((_tmp908.tag=11,((_tmp908.f1=((_tmp907.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp907.targs=_tmp42D,_tmp907)))),_tmp908)))),_tmp906))));}{
# 1998
struct Cyc_List_List*_tmp43B=*ts;
struct Cyc_List_List*_tmp43C=_tmp42E;
while(_tmp43B != 0  && _tmp43C != 0){
# 2002
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp43B->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp43B->hd);
Cyc_Tcutil_unify((void*)_tmp43B->hd,(void*)_tmp43C->hd);
_tmp43B=_tmp43B->tl;
_tmp43C=_tmp43C->tl;}
# 2008
if(_tmp43B != 0){
const char*_tmp90B;void*_tmp90A;(_tmp90A=0,Cyc_Tcutil_terr(loc,((_tmp90B="too many explicit witness types",_tag_dyneither(_tmp90B,sizeof(char),32))),_tag_dyneither(_tmp90A,sizeof(void*),0)));}
# 2011
*ts=_tmp42E;{
# 2014
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp440;struct Cyc_Absyn_Exp*_tmp441;struct _tuple20*_tmp43F=(struct _tuple20*)fields->hd;_tmp440=_tmp43F->f1;_tmp441=_tmp43F->f2;{
void*_tmp442=Cyc_Tcutil_rsubstitute(rgn,_tmp42F,_tmp440->type);
Cyc_Tcexp_tcExpInitializer(te2,& _tmp442,_tmp441);
# 2024
if(!Cyc_Tcutil_coerce_arg(te2,_tmp441,_tmp442,& bogus)){
{const char*_tmp917;void*_tmp916[5];struct Cyc_String_pa_PrintArg_struct _tmp915;const char*_tmp914;const char*_tmp913;struct Cyc_String_pa_PrintArg_struct _tmp912;struct Cyc_String_pa_PrintArg_struct _tmp911;struct Cyc_String_pa_PrintArg_struct _tmp910;struct Cyc_String_pa_PrintArg_struct _tmp90F;(_tmp90F.tag=0,((_tmp90F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2028
Cyc_Absynpp_typ2string((void*)_check_null(_tmp441->topt))),((_tmp910.tag=0,((_tmp910.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2027
Cyc_Absynpp_typ2string(_tmp442)),((_tmp911.tag=0,((_tmp911.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp912.tag=0,((_tmp912.f1=(struct _dyneither_ptr)(
# 2026
ad->kind == Cyc_Absyn_StructA?(_tmp913="struct",_tag_dyneither(_tmp913,sizeof(char),7)):((_tmp914="union",_tag_dyneither(_tmp914,sizeof(char),6)))),((_tmp915.tag=0,((_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp440->name),((_tmp916[0]=& _tmp915,((_tmp916[1]=& _tmp912,((_tmp916[2]=& _tmp911,((_tmp916[3]=& _tmp910,((_tmp916[4]=& _tmp90F,Cyc_Tcutil_terr(_tmp441->loc,((_tmp917="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp917,sizeof(char),40))),_tag_dyneither(_tmp916,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2029
Cyc_Tcutil_explain_failure();}};}{
# 2033
struct Cyc_List_List*_tmp44C=0;
{struct Cyc_List_List*_tmp44D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp44D != 0;_tmp44D=_tmp44D->tl){
struct _tuple0*_tmp91A;struct Cyc_List_List*_tmp919;_tmp44C=((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919->hd=((_tmp91A=_cycalloc(sizeof(*_tmp91A)),((_tmp91A->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp42F,(*((struct _tuple0*)_tmp44D->hd)).f1),((_tmp91A->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp42F,(*((struct _tuple0*)_tmp44D->hd)).f2),_tmp91A)))))),((_tmp919->tl=_tmp44C,_tmp919))))));}}
# 2038
_tmp44C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp44C);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp44C);{
void*_tmp450=res_typ;_npop_handler(0);return _tmp450;};};};};};}
# 1957
;_pop_region(rgn);};}
# 2044
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2046
struct _RegionHandle _tmp452=_new_region("rgn");struct _RegionHandle*rgn=& _tmp452;_push_region(rgn);{
void*_tmp453=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp455;struct Cyc_List_List*_tmp456;_LL1E7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp453;if(_tmp454->tag != 12)goto _LL1E9;else{_tmp455=_tmp454->f1;_tmp456=_tmp454->f2;}}_LL1E8:
# 2049
 if(_tmp455 == Cyc_Absyn_UnionA){
const char*_tmp91D;void*_tmp91C;(_tmp91C=0,Cyc_Tcutil_terr(loc,((_tmp91D="expecting struct but found union",_tag_dyneither(_tmp91D,sizeof(char),33))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp456);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct Cyc_Absyn_Aggrfield*_tmp45A;struct Cyc_Absyn_Exp*_tmp45B;struct _tuple20*_tmp459=(struct _tuple20*)fields->hd;_tmp45A=_tmp459->f1;_tmp45B=_tmp459->f2;
Cyc_Tcexp_tcExpInitializer(te,& _tmp45A->type,_tmp45B);
if(!Cyc_Tcutil_coerce_arg(te,_tmp45B,_tmp45A->type,& bogus)){
{const char*_tmp923;void*_tmp922[3];struct Cyc_String_pa_PrintArg_struct _tmp921;struct Cyc_String_pa_PrintArg_struct _tmp920;struct Cyc_String_pa_PrintArg_struct _tmp91F;(_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2060
Cyc_Absynpp_typ2string((void*)_check_null(_tmp45B->topt))),((_tmp920.tag=0,((_tmp920.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2059
Cyc_Absynpp_typ2string(_tmp45A->type)),((_tmp921.tag=0,((_tmp921.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp45A->name),((_tmp922[0]=& _tmp921,((_tmp922[1]=& _tmp920,((_tmp922[2]=& _tmp91F,Cyc_Tcutil_terr(_tmp45B->loc,((_tmp923="field %s of struct expects type %s != %s",_tag_dyneither(_tmp923,sizeof(char),41))),_tag_dyneither(_tmp922,sizeof(void*),3)))))))))))))))))));}
# 2061
Cyc_Tcutil_explain_failure();}}
# 2064
goto _LL1E6;};_LL1E9:;_LL1EA: {
const char*_tmp926;void*_tmp925;(_tmp925=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp926="tcAnonStruct: wrong type",_tag_dyneither(_tmp926,sizeof(char),25))),_tag_dyneither(_tmp925,sizeof(void*),0)));}_LL1E6:;}{
# 2068
void*_tmp463=ts;_npop_handler(0);return _tmp463;};
# 2046
;_pop_region(rgn);}
# 2072
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2075
struct _RegionHandle _tmp464=_new_region("rgn");struct _RegionHandle*rgn=& _tmp464;_push_region(rgn);
{struct _tuple12 _tmp927;struct _tuple12 _tmp465=(_tmp927.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp927.f2=rgn,_tmp927)));
struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp465,tud->tvs);
struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp466);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp92D;struct Cyc_Absyn_DatatypeFieldInfo _tmp92C;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp92B;void*res=(void*)(
(_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B[0]=((_tmp92D.tag=4,((_tmp92D.f1=((_tmp92C.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp92C.targs=_tmp467,_tmp92C)))),_tmp92D)))),_tmp92B))));
# 2083
if(topt != 0){
void*_tmp468=Cyc_Tcutil_compress(*topt);_LL1EC: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp468;if(_tmp469->tag != 4)goto _LL1EE;}_LL1ED:
 Cyc_Tcutil_unify(*topt,res);goto _LL1EB;_LL1EE:;_LL1EF:
 goto _LL1EB;_LL1EB:;}{
# 2089
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple19*)ts->hd)).f2;
if(_tmp466 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp466,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp935;void*_tmp934[3];struct Cyc_String_pa_PrintArg_struct _tmp933;struct Cyc_String_pa_PrintArg_struct _tmp932;const char*_tmp931;struct Cyc_String_pa_PrintArg_struct _tmp930;(_tmp930.tag=0,((_tmp930.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2100
e->topt == 0?(struct _dyneither_ptr)((_tmp931="?",_tag_dyneither(_tmp931,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp932.tag=0,((_tmp932.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2099
Cyc_Absynpp_typ2string(t)),((_tmp933.tag=0,((_tmp933.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp934[0]=& _tmp933,((_tmp934[1]=& _tmp932,((_tmp934[2]=& _tmp930,Cyc_Tcutil_terr(e->loc,((_tmp935="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp935,sizeof(char),82))),_tag_dyneither(_tmp934,sizeof(void*),3)))))))))))))))))));}
# 2101
Cyc_Tcutil_explain_failure();}}
# 2104
if(es != 0){
const char*_tmp939;void*_tmp938[1];struct Cyc_String_pa_PrintArg_struct _tmp937;void*_tmp473=(_tmp937.tag=0,((_tmp937.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2107
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp938[0]=& _tmp937,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp939="too many arguments for datatype constructor %s",_tag_dyneither(_tmp939,sizeof(char),47))),_tag_dyneither(_tmp938,sizeof(void*),1)))))));_npop_handler(0);return _tmp473;}
if(ts != 0){
const char*_tmp93D;void*_tmp93C[1];struct Cyc_String_pa_PrintArg_struct _tmp93B;void*_tmp477=(_tmp93B.tag=0,((_tmp93B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp93C[0]=& _tmp93B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp93D="too few arguments for datatype constructor %s",_tag_dyneither(_tmp93D,sizeof(char),46))),_tag_dyneither(_tmp93C,sizeof(void*),1)))))));_npop_handler(0);return _tmp477;}{
void*_tmp478=res;_npop_handler(0);return _tmp478;};};}
# 2076
;_pop_region(rgn);}
# 2115
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp47D=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp484;void*_tmp488;struct Cyc_List_List*_tmp48B;union Cyc_Absyn_AggrInfoU _tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_List_List*_tmp492;_LL1F1: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp47D;if(_tmp47E->tag != 0)goto _LL1F3;}_LL1F2:
 return 1;_LL1F3: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp47D;if(_tmp47F->tag != 1)goto _LL1F5;}_LL1F4:
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47D;if(_tmp480->tag != 2)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp47D;if(_tmp481->tag != 3)goto _LL1F9;}_LL1F8:
 goto _LL1FA;_LL1F9: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp47D;if(_tmp482->tag != 4)goto _LL1FB;}_LL1FA:
 return 0;_LL1FB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47D;if(_tmp483->tag != 5)goto _LL1FD;else{_tmp484=((_tmp483->f1).ptr_atts).nullable;}}_LL1FC:
# 2123
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp484);_LL1FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp47D;if(_tmp485->tag != 6)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp486=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp47D;if(_tmp486->tag != 7)goto _LL201;}_LL200:
 return 1;_LL201: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47D;if(_tmp487->tag != 8)goto _LL203;else{_tmp488=(_tmp487->f1).elt_type;}}_LL202:
 return Cyc_Tcexp_zeroable_type(_tmp488);_LL203: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp489=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp47D;if(_tmp489->tag != 9)goto _LL205;}_LL204:
 return 0;_LL205: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47D;if(_tmp48A->tag != 10)goto _LL207;else{_tmp48B=_tmp48A->f1;}}_LL206:
# 2129
 for(0;(unsigned int)_tmp48B;_tmp48B=_tmp48B->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp48B->hd)).f2))return 0;}
return 1;_LL207: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp48C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp47D;if(_tmp48C->tag != 11)goto _LL209;else{_tmp48D=(_tmp48C->f1).aggr_info;_tmp48E=(_tmp48C->f1).targs;}}_LL208: {
# 2133
struct Cyc_Absyn_Aggrdecl*_tmp49F=Cyc_Absyn_get_known_aggrdecl(_tmp48D);
if(_tmp49F->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49F->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp4A0=_new_region("r");struct _RegionHandle*r=& _tmp4A0;_push_region(r);
{struct Cyc_List_List*_tmp4A1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp49F->tvs,_tmp48E);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp49F->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4A1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4A2=0;_npop_handler(0);return _tmp4A2;}}}{
int _tmp4A3=1;_npop_handler(0);return _tmp4A3;};}
# 2137
;_pop_region(r);};}_LL209: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp48F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp47D;if(_tmp48F->tag != 13)goto _LL20B;}_LL20A:
# 2142
 return 1;_LL20B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp47D;if(_tmp490->tag != 19)goto _LL20D;}_LL20C:
 return 1;_LL20D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp491=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp47D;if(_tmp491->tag != 12)goto _LL20F;else{_tmp492=_tmp491->f2;}}_LL20E:
# 2145
 for(0;_tmp492 != 0;_tmp492=_tmp492->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp492->hd)->type))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp47D;if(_tmp493->tag != 14)goto _LL211;}_LL210:
 return 1;_LL211: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp47D;if(_tmp494->tag != 17)goto _LL213;}_LL212:
 return 0;_LL213: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp495=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp47D;if(_tmp495->tag != 16)goto _LL215;}_LL214:
 return 0;_LL215: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp496=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp47D;if(_tmp496->tag != 15)goto _LL217;}_LL216:
 return 0;_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp497=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp47D;if(_tmp497->tag != 26)goto _LL219;}_LL218:
# 2153
 goto _LL21A;_LL219: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp498=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp47D;if(_tmp498->tag != 18)goto _LL21B;}_LL21A:
 goto _LL21C;_LL21B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp499=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp47D;if(_tmp499->tag != 20)goto _LL21D;}_LL21C:
 goto _LL21E;_LL21D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp49A=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp47D;if(_tmp49A->tag != 21)goto _LL21F;}_LL21E:
 goto _LL220;_LL21F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp47D;if(_tmp49B->tag != 22)goto _LL221;}_LL220:
 goto _LL222;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp49C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp47D;if(_tmp49C->tag != 23)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp47D;if(_tmp49D->tag != 24)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp49E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp47D;if(_tmp49E->tag != 25)goto _LL1F0;}_LL226: {
const char*_tmp941;void*_tmp940[1];struct Cyc_String_pa_PrintArg_struct _tmp93F;(_tmp93F.tag=0,((_tmp93F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp940[0]=& _tmp93F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp941="bad type `%s' in zeroable type",_tag_dyneither(_tmp941,sizeof(char),31))),_tag_dyneither(_tmp940,sizeof(void*),1)))))));}_LL1F0:;}
# 2164
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2166
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2168
if(topt != 0){
void*_tmp4A7=Cyc_Tcutil_compress(*topt);void*_tmp4A9;_LL228: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A7;if(_tmp4A8->tag != 5)goto _LL22A;else{_tmp4A9=(_tmp4A8->f1).elt_typ;}}_LL229:
# 2171
 Cyc_Tcutil_unify(_tmp4A9,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL227;_LL22A:;_LL22B:
 goto _LL227;_LL227:;}{
# 2177
const char*_tmp94A;void*_tmp949[2];const char*_tmp948;const char*_tmp947;struct Cyc_String_pa_PrintArg_struct _tmp946;struct Cyc_String_pa_PrintArg_struct _tmp945;(_tmp945.tag=0,((_tmp945.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp946.tag=0,((_tmp946.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp947="calloc",_tag_dyneither(_tmp947,sizeof(char),7))):(struct _dyneither_ptr)((_tmp948="malloc",_tag_dyneither(_tmp948,sizeof(char),7))))),((_tmp949[0]=& _tmp946,((_tmp949[1]=& _tmp945,Cyc_Tcutil_terr(loc,((_tmp94A="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp94A,sizeof(char),60))),_tag_dyneither(_tmp949,sizeof(void*),2)))))))))))));};}
# 2181
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2183
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL22C:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL22D:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2189
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2194
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2197
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp94D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp94C;void*expected_type=(void*)(
(_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94D.tag=15,((_tmp94D.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp94D)))),_tmp94C))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp4B0=Cyc_Tcutil_compress(handle_type);void*_tmp4B2;_LL230: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4B0;if(_tmp4B1->tag != 15)goto _LL232;else{_tmp4B2=(void*)_tmp4B1->f1;}}_LL231:
# 2202
 rgn=_tmp4B2;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22F;_LL232:;_LL233:
# 2206
{const char*_tmp951;void*_tmp950[1];struct Cyc_String_pa_PrintArg_struct _tmp94F;(_tmp94F.tag=0,((_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp950[0]=& _tmp94F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp951="expecting region_t type but found %s",_tag_dyneither(_tmp951,sizeof(char),37))),_tag_dyneither(_tmp950,sizeof(void*),1)))))));}
goto _LL22F;_LL22F:;}else{
# 2213
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2221
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,*e);{
# 2230
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp954;void*_tmp953;(_tmp953=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp954="calloc with empty type",_tag_dyneither(_tmp954,sizeof(char),23))),_tag_dyneither(_tmp953,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2242
void*er=(*e)->r;
retry_sizeof: {
void*_tmp4BA=er;void*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;_LL235: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4BB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4BA;if(_tmp4BB->tag != 16)goto _LL237;else{_tmp4BC=(void*)_tmp4BB->f1;}}_LL236:
# 2246
 elt_type=_tmp4BC;
{void**_tmp955;*t=((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=elt_type,_tmp955))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL234;_LL237: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4BD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4BA;if(_tmp4BD->tag != 2)goto _LL239;else{if(_tmp4BD->f1 != Cyc_Absyn_Times)goto _LL239;if(_tmp4BD->f2 == 0)goto _LL239;_tmp4BE=(struct Cyc_Absyn_Exp*)(_tmp4BD->f2)->hd;if((_tmp4BD->f2)->tl == 0)goto _LL239;_tmp4BF=(struct Cyc_Absyn_Exp*)((_tmp4BD->f2)->tl)->hd;if(((_tmp4BD->f2)->tl)->tl != 0)goto _LL239;}}_LL238:
# 2253
{struct _tuple0 _tmp956;struct _tuple0 _tmp4C2=(_tmp956.f1=_tmp4BE->r,((_tmp956.f2=_tmp4BF->r,_tmp956)));void*_tmp4C4;void*_tmp4C6;_LL23C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4C2.f1;if(_tmp4C3->tag != 16)goto _LL23E;else{_tmp4C4=(void*)_tmp4C3->f1;}}_LL23D:
# 2255
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C4);
elt_type=_tmp4C4;
{void**_tmp957;*t=((_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957[0]=elt_type,_tmp957))));}
num_elts=_tmp4BF;
one_elt=0;
goto _LL23B;_LL23E: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4C5=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4C2.f2;if(_tmp4C5->tag != 16)goto _LL240;else{_tmp4C6=(void*)_tmp4C5->f1;}}_LL23F:
# 2262
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4C6);
elt_type=_tmp4C6;
{void**_tmp958;*t=((_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958[0]=elt_type,_tmp958))));}
num_elts=_tmp4BE;
one_elt=0;
goto _LL23B;_LL240:;_LL241:
 goto No_sizeof;_LL23B:;}
# 2270
goto _LL234;_LL239:;_LL23A:
# 2272
 No_sizeof: {
# 2275
struct Cyc_Absyn_Exp*_tmp4C9=*e;
{void*_tmp4CA=_tmp4C9->r;struct Cyc_Absyn_Exp*_tmp4CC;_LL243: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4CA;if(_tmp4CB->tag != 13)goto _LL245;else{_tmp4CC=_tmp4CB->f2;}}_LL244:
 _tmp4C9=_tmp4CC;goto _LL242;_LL245:;_LL246:
 goto _LL242;_LL242:;}
# 2280
{void*_tmp4CD=Cyc_Tcutil_compress((void*)_check_null(_tmp4C9->topt));void*_tmp4CF;_LL248: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4CD;if(_tmp4CE->tag != 19)goto _LL24A;else{_tmp4CF=(void*)_tmp4CE->f1;}}_LL249:
# 2282
{void*_tmp4D0=Cyc_Tcutil_compress(_tmp4CF);struct Cyc_Absyn_Exp*_tmp4D2;_LL24D: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4D1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4D0;if(_tmp4D1->tag != 18)goto _LL24F;else{_tmp4D2=_tmp4D1->f1;}}_LL24E:
# 2284
 er=_tmp4D2->r;goto retry_sizeof;_LL24F:;_LL250:
 goto _LL24C;_LL24C:;}
# 2287
goto _LL247;_LL24A:;_LL24B:
 goto _LL247;_LL247:;}
# 2290
elt_type=Cyc_Absyn_char_typ;
{void**_tmp959;*t=((_tmp959=_cycalloc(sizeof(*_tmp959)),((_tmp959[0]=elt_type,_tmp959))));}
num_elts=*e;
one_elt=0;}
# 2295
goto _LL234;_LL234:;}}
# 2299
*is_fat=!one_elt;
# 2302
{void*_tmp4D4=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp4D6;_LL252: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4D5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4D4;if(_tmp4D5->tag != 11)goto _LL254;else{if((((_tmp4D5->f1).aggr_info).KnownAggr).tag != 2)goto _LL254;_tmp4D6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp4D5->f1).aggr_info).KnownAggr).val);}}_LL253:
# 2304
 if(_tmp4D6->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4D6->impl))->exist_vars != 0){
const char*_tmp95C;void*_tmp95B;(_tmp95B=0,Cyc_Tcutil_terr(loc,((_tmp95C="malloc with existential types not yet implemented",_tag_dyneither(_tmp95C,sizeof(char),50))),_tag_dyneither(_tmp95B,sizeof(void*),0)));}
goto _LL251;_LL254:;_LL255:
 goto _LL251;_LL251:;}{
# 2311
void*(*_tmp4D9)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp4DA=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp4DB=Cyc_Tcutil_compress(*topt);union Cyc_Absyn_Constraint*_tmp4DD;union Cyc_Absyn_Constraint*_tmp4DE;union Cyc_Absyn_Constraint*_tmp4DF;_LL257: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4DC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DB;if(_tmp4DC->tag != 5)goto _LL259;else{_tmp4DD=((_tmp4DC->f1).ptr_atts).nullable;_tmp4DE=((_tmp4DC->f1).ptr_atts).bounds;_tmp4DF=((_tmp4DC->f1).ptr_atts).zero_term;}}_LL258:
# 2316
 _tmp4DA=_tmp4DF;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DD))
_tmp4D9=Cyc_Absyn_star_typ;
# 2321
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4DF) && !(*is_calloc)){
{const char*_tmp95F;void*_tmp95E;(_tmp95E=0,Cyc_Tcutil_warn(loc,((_tmp95F="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp95F,sizeof(char),55))),_tag_dyneither(_tmp95E,sizeof(void*),0)));}
*is_calloc=1;}
# 2327
{void*_tmp4E2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp4DE);struct Cyc_Absyn_Exp*_tmp4E5;_LL25C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4E3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E2;if(_tmp4E3->tag != 0)goto _LL25E;}_LL25D:
 goto _LL25B;_LL25E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4E4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp4E2;if(_tmp4E4->tag != 1)goto _LL260;else{_tmp4E5=_tmp4E4->f1;}}if(!(!one_elt))goto _LL260;_LL25F: {
# 2330
int _tmp4E6=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp4E6  && Cyc_Evexp_same_const_exp(_tmp4E5,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DE),_tmp4DA);}
# 2336
{void*_tmp4E7=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp4E9;_LL263: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4E7;if(_tmp4E8->tag != 19)goto _LL265;else{_tmp4E9=(void*)_tmp4E8->f1;}}_LL264: {
# 2338
struct Cyc_Absyn_Exp*_tmp4EA=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4E9,0),0,Cyc_Absyn_No_coercion,0);
# 2340
if(Cyc_Evexp_same_const_exp(_tmp4EA,_tmp4E5)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp4DE),_tmp4DA);}
# 2345
goto _LL262;}_LL265:;_LL266:
 goto _LL262;_LL262:;}
# 2348
goto _LL25B;}_LL260:;_LL261:
 goto _LL25B;_LL25B:;}
# 2351
goto _LL256;_LL259:;_LL25A:
 goto _LL256;_LL256:;}
# 2354
if(!one_elt)_tmp4D9=Cyc_Absyn_dyneither_typ;
return _tmp4D9(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp4DA);};};}
# 2359
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2365
struct _RegionHandle _tmp4EB=_new_region("r");struct _RegionHandle*r=& _tmp4EB;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp4EC=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExp(_tmp4EC,0,e1);{
void*_tmp4ED=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(_tmp4EC,& _tmp4ED,e2);};}{
# 2371
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2374
{void*_tmp4EE=Cyc_Tcutil_compress(t1);_LL268: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4EE;if(_tmp4EF->tag != 8)goto _LL26A;}_LL269:
{const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(loc,((_tmp962="cannot assign to an array",_tag_dyneither(_tmp962,sizeof(char),26))),_tag_dyneither(_tmp961,sizeof(void*),0)));}goto _LL267;_LL26A:;_LL26B:
 goto _LL267;_LL267:;}{
# 2379
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp965;void*_tmp964;(_tmp964=0,Cyc_Tcutil_terr(loc,((_tmp965="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp965,sizeof(char),58))),_tag_dyneither(_tmp964,sizeof(void*),0)));}
# 2384
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp968;void*_tmp967;void*_tmp4F6=(_tmp967=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp968="swap non-lvalue",_tag_dyneither(_tmp968,sizeof(char),16))),_tag_dyneither(_tmp967,sizeof(void*),0)));_npop_handler(0);return _tmp4F6;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp96B;void*_tmp96A;void*_tmp4F9=(_tmp96A=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp96B="swap non-lvalue",_tag_dyneither(_tmp96B,sizeof(char),16))),_tag_dyneither(_tmp96A,sizeof(void*),0)));_npop_handler(0);return _tmp4F9;}{
# 2389
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp96E;void*_tmp96D;void*_tmp4FC=(_tmp96D=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp96E="swap value in zeroterm array",_tag_dyneither(_tmp96E,sizeof(char),29))),_tag_dyneither(_tmp96D,sizeof(void*),0)));_npop_handler(0);return _tmp4FC;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp971;void*_tmp970;void*_tmp4FF=(_tmp970=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp971="swap value in zeroterm array",_tag_dyneither(_tmp971,sizeof(char),29))),_tag_dyneither(_tmp970,sizeof(void*),0)));_npop_handler(0);return _tmp4FF;}
# 2396
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp976;void*_tmp975[2];struct Cyc_String_pa_PrintArg_struct _tmp974;struct Cyc_String_pa_PrintArg_struct _tmp973;void*_tmp500=(_tmp973.tag=0,((_tmp973.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp974.tag=0,((_tmp974.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp975[0]=& _tmp974,((_tmp975[1]=& _tmp973,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp976="type mismatch: %s != %s",_tag_dyneither(_tmp976,sizeof(char),24))),_tag_dyneither(_tmp975,sizeof(void*),2)))))))))))));
void*_tmp501=_tmp500;_npop_handler(0);return _tmp501;}{
# 2403
void*_tmp506=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp506;};};};};
# 2365
;_pop_region(r);}
# 2407
int Cyc_Tcexp_in_stmt_exp=0;
# 2409
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp507=_new_region("r");struct _RegionHandle*r=& _tmp507;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2416
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2418
while(1){
void*_tmp508=s->r;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Stmt*_tmp50C;struct Cyc_Absyn_Stmt*_tmp50D;struct Cyc_Absyn_Decl*_tmp50F;struct Cyc_Absyn_Stmt*_tmp510;_LL26D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp509=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp508;if(_tmp509->tag != 1)goto _LL26F;else{_tmp50A=_tmp509->f1;}}_LL26E: {
# 2422
void*_tmp511=(void*)_check_null(_tmp50A->topt);
if(!Cyc_Tcutil_unify(_tmp511,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp97A;void*_tmp979[1];struct Cyc_String_pa_PrintArg_struct _tmp978;(_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp511)),((_tmp979[0]=& _tmp978,Cyc_Tcutil_terr(loc,((_tmp97A="statement expression returns type %s",_tag_dyneither(_tmp97A,sizeof(char),37))),_tag_dyneither(_tmp979,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2428
void*_tmp515=_tmp511;_npop_handler(0);return _tmp515;};}_LL26F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp50B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp508;if(_tmp50B->tag != 2)goto _LL271;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50B->f2;}}_LL270:
 s=_tmp50D;continue;_LL271: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp50E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp508;if(_tmp50E->tag != 12)goto _LL273;else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;}}_LL272:
 s=_tmp510;continue;_LL273:;_LL274: {
# 2432
const char*_tmp97D;void*_tmp97C;void*_tmp518=(_tmp97C=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp97D="statement expression must end with expression",_tag_dyneither(_tmp97D,sizeof(char),46))),_tag_dyneither(_tmp97C,sizeof(void*),0)));_npop_handler(0);return _tmp518;}_LL26C:;}
# 2410
;_pop_region(r);}
# 2437
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
{void*_tmp519=t;struct Cyc_Absyn_Aggrdecl*_tmp51B;_LL276: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp519;if(_tmp51A->tag != 11)goto _LL278;else{if((((_tmp51A->f1).aggr_info).KnownAggr).tag != 2)goto _LL278;_tmp51B=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp51A->f1).aggr_info).KnownAggr).val);}}_LL277:
# 2441
 if((_tmp51B->kind == Cyc_Absyn_UnionA  && _tmp51B->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->tagged)goto _LL275;
goto _LL279;_LL278:;_LL279:
# 2444
{const char*_tmp981;void*_tmp980[1];struct Cyc_String_pa_PrintArg_struct _tmp97F;(_tmp97F.tag=0,((_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp980[0]=& _tmp97F,Cyc_Tcutil_terr(loc,((_tmp981="expecting @tagged union but found %s",_tag_dyneither(_tmp981,sizeof(char),37))),_tag_dyneither(_tmp980,sizeof(void*),1)))))));}
goto _LL275;_LL275:;}
# 2447
return Cyc_Absyn_uint_typ;}
# 2451
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2455
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp51F=_new_region("r");struct _RegionHandle*r=& _tmp51F;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp520=Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te);
if(*rgn_handle != 0){
# 2460
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp984;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp983;void*expected_type=(void*)(
(_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp984.tag=15,((_tmp984.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp520)),_tmp984)))),_tmp983))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp520,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp521=Cyc_Tcutil_compress(handle_type);void*_tmp523;_LL27B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp522=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp521;if(_tmp522->tag != 15)goto _LL27D;else{_tmp523=(void*)_tmp522->f1;}}_LL27C:
# 2465
 rgn=_tmp523;
Cyc_Tcenv_check_rgn_accessible(_tmp520,loc,rgn);
goto _LL27A;_LL27D:;_LL27E:
# 2469
{const char*_tmp988;void*_tmp987[1];struct Cyc_String_pa_PrintArg_struct _tmp986;(_tmp986.tag=0,((_tmp986.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp987[0]=& _tmp986,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp988="expecting region_t type but found %s",_tag_dyneither(_tmp988,sizeof(char),37))),_tag_dyneither(_tmp987,sizeof(void*),1)))))));}
goto _LL27A;_LL27A:;}else{
# 2476
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2484
void*_tmp529=e1->r;struct Cyc_Core_Opt*_tmp52C;struct Cyc_List_List*_tmp52D;struct Cyc_List_List*_tmp52F;_LL280: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp529;if(_tmp52A->tag != 26)goto _LL282;}_LL281: {
# 2488
void*_tmp532=Cyc_Tcexp_tcExpNoPromote(_tmp520,topt,e1);
void*_tmp533=Cyc_Tcutil_compress(_tmp532);void*_tmp535;struct Cyc_Absyn_Tqual _tmp536;struct Cyc_Absyn_Exp*_tmp537;union Cyc_Absyn_Constraint*_tmp538;_LL28D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp533;if(_tmp534->tag != 8)goto _LL28F;else{_tmp535=(_tmp534->f1).elt_type;_tmp536=(_tmp534->f1).tq;_tmp537=(_tmp534->f1).num_elts;_tmp538=(_tmp534->f1).zero_term;}}_LL28E: {
# 2491
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp537);
void*b;
{void*_tmp539=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp53B;_LL292: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp53A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp539;if(_tmp53A->tag != 19)goto _LL294;else{_tmp53B=(void*)_tmp53A->f1;}}_LL293:
# 2495
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp98B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp98A;b=(void*)((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=1,((_tmp98B.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp53B,0),0,Cyc_Absyn_No_coercion,0),_tmp98B)))),_tmp98A))));}
goto _LL291;_LL294:;_LL295:
# 2498
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp98E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp98D;b=(void*)((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98E.tag=1,((_tmp98E.f1=bnd,_tmp98E)))),_tmp98D))));}else{
# 2501
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL291:;}{
# 2503
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp998;struct Cyc_Absyn_PtrAtts _tmp997;struct Cyc_Absyn_PtrInfo _tmp996;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp995;void*res_typ=(void*)(
(_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995[0]=((_tmp998.tag=5,((_tmp998.f1=((_tmp996.elt_typ=_tmp535,((_tmp996.elt_tq=_tmp536,((_tmp996.ptr_atts=(
(_tmp997.rgn=rgn,((_tmp997.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp997.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp997.zero_term=_tmp538,((_tmp997.ptrloc=0,_tmp997)))))))))),_tmp996)))))),_tmp998)))),_tmp995))));
# 2507
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp520,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp520,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2514
void*_tmp540=res_typ;_npop_handler(0);return _tmp540;};};}_LL28F:;_LL290: {
const char*_tmp99B;void*_tmp99A;(_tmp99A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp99B="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp99B,sizeof(char),45))),_tag_dyneither(_tmp99A,sizeof(void*),0)));}_LL28C:;}_LL282: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp529;if(_tmp52B->tag != 34)goto _LL284;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL283:
# 2518
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp99E;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp99D;e1->r=(void*)((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D[0]=((_tmp99E.tag=25,((_tmp99E.f1=_tmp52D,_tmp99E)))),_tmp99D))));}
_tmp52F=_tmp52D;goto _LL285;_LL284: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp529;if(_tmp52E->tag != 25)goto _LL286;else{_tmp52F=_tmp52E->f1;}}_LL285: {
# 2521
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp549=Cyc_Tcutil_compress(*topt);void**_tmp54B;struct Cyc_Absyn_Tqual _tmp54C;union Cyc_Absyn_Constraint*_tmp54D;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549;if(_tmp54A->tag != 5)goto _LL299;else{_tmp54B=(void**)&(_tmp54A->f1).elt_typ;_tmp54C=(_tmp54A->f1).elt_tq;_tmp54D=((_tmp54A->f1).ptr_atts).zero_term;}}_LL298:
# 2527
 elt_typ_opt=_tmp54B;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp54D);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2533
void*_tmp54E=Cyc_Tcexp_tcArray(_tmp520,e1->loc,elt_typ_opt,0,zero_term,_tmp52F);
e1->topt=_tmp54E;{
void*res_typ;
{void*_tmp54F=Cyc_Tcutil_compress(_tmp54E);void*_tmp551;struct Cyc_Absyn_Tqual _tmp552;struct Cyc_Absyn_Exp*_tmp553;union Cyc_Absyn_Constraint*_tmp554;_LL29C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54F;if(_tmp550->tag != 8)goto _LL29E;else{_tmp551=(_tmp550->f1).elt_type;_tmp552=(_tmp550->f1).tq;_tmp553=(_tmp550->f1).num_elts;_tmp554=(_tmp550->f1).zero_term;}}_LL29D:
# 2538
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9B3;struct Cyc_Absyn_PtrAtts _tmp9B2;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B0;struct Cyc_Absyn_PtrInfo _tmp9AF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9AE;res_typ=(void*)(
(_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE[0]=((_tmp9B3.tag=5,((_tmp9B3.f1=((_tmp9AF.elt_typ=_tmp551,((_tmp9AF.elt_tq=_tmp552,((_tmp9AF.ptr_atts=(
(_tmp9B2.rgn=rgn,((_tmp9B2.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9B2.bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=1,((_tmp9B1.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp553),_tmp9B1)))),_tmp9B0))))),((_tmp9B2.zero_term=_tmp554,((_tmp9B2.ptrloc=0,_tmp9B2)))))))))),_tmp9AF)))))),_tmp9B3)))),_tmp9AE))));}
# 2543
if(topt != 0){
# 2547
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp520,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp520,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2553
goto _LL29B;_LL29E:;_LL29F: {
const char*_tmp9B6;void*_tmp9B5;(_tmp9B5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B6="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9B6,sizeof(char),50))),_tag_dyneither(_tmp9B5,sizeof(void*),0)));}_LL29B:;}{
# 2556
void*_tmp55D=res_typ;_npop_handler(0);return _tmp55D;};};};}_LL286: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp530=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp529;if(_tmp530->tag != 0)goto _LL288;else{if(((_tmp530->f1).String_c).tag != 8)goto _LL288;}}_LL287: {
# 2561
void*_tmp55E=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2563
void*_tmp55F=Cyc_Tcexp_tcExp(_tmp520,& _tmp55E,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B8;void*_tmp562=Cyc_Absyn_atb_typ(_tmp55F,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=1,((_tmp9B9.f1=Cyc_Absyn_uint_exp(1,0),_tmp9B9)))),_tmp9B8)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp562;}_LL288: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp529;if(_tmp531->tag != 0)goto _LL28A;else{if(((_tmp531->f1).Wstring_c).tag != 9)goto _LL28A;}}_LL289: {
# 2568
void*_tmp563=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2570
void*_tmp564=Cyc_Tcexp_tcExp(_tmp520,& _tmp563,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9BC;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9BB;void*_tmp567=Cyc_Absyn_atb_typ(_tmp564,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BC.tag=1,((_tmp9BC.f1=Cyc_Absyn_uint_exp(1,0),_tmp9BC)))),_tmp9BB)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp567;}_LL28A:;_LL28B:
# 2577
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp568=Cyc_Tcutil_compress(*topt);void**_tmp56A;struct Cyc_Absyn_Tqual _tmp56B;_LL2A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp568;if(_tmp569->tag != 5)goto _LL2A3;else{_tmp56A=(void**)&(_tmp569->f1).elt_typ;_tmp56B=(_tmp569->f1).elt_tq;}}_LL2A2:
# 2583
 topt2=_tmp56A;goto _LL2A0;_LL2A3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp568;if(_tmp56C->tag != 3)goto _LL2A5;}_LL2A4:
# 2587
 bogus=*topt;
topt2=& bogus;
goto _LL2A0;_LL2A5:;_LL2A6:
 goto _LL2A0;_LL2A0:;}{
# 2593
void*telt=Cyc_Tcexp_tcExp(_tmp520,topt2,e1);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9C6;struct Cyc_Absyn_PtrAtts _tmp9C5;struct Cyc_Absyn_PtrInfo _tmp9C4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9C3;void*res_typ=(void*)(
(_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C6.tag=5,((_tmp9C6.f1=((_tmp9C4.elt_typ=telt,((_tmp9C4.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9C4.ptr_atts=(
(_tmp9C5.rgn=rgn,((_tmp9C5.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9C5.bounds=Cyc_Absyn_bounds_one_conref,((_tmp9C5.zero_term=Cyc_Absyn_false_conref,((_tmp9C5.ptrloc=0,_tmp9C5)))))))))),_tmp9C4)))))),_tmp9C6)))),_tmp9C3))));
# 2599
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp520,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp520,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2606
void*_tmp56D=res_typ;_npop_handler(0);return _tmp56D;};};}_LL27F:;};}
# 2457
;_pop_region(r);}
# 2613
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
void*_tmp572=t;void*_tmp574;struct Cyc_Absyn_Tqual _tmp575;struct Cyc_Absyn_Exp*_tmp576;union Cyc_Absyn_Constraint*_tmp577;_LL2A8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp572;if(_tmp573->tag != 8)goto _LL2AA;else{_tmp574=(_tmp573->f1).elt_type;_tmp575=(_tmp573->f1).tq;_tmp576=(_tmp573->f1).num_elts;_tmp577=(_tmp573->f1).zero_term;}}_LL2A9: {
# 2617
void*_tmp579;struct _tuple14 _tmp578=Cyc_Tcutil_addressof_props(te,e);_tmp579=_tmp578.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9C9;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9C8;void*_tmp57A=_tmp576 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=1,((_tmp9C9.f1=_tmp576,_tmp9C9)))),_tmp9C8))));
t=Cyc_Absyn_atb_typ(_tmp574,_tmp579,_tmp575,_tmp57A,_tmp577);
e->topt=t;
return t;};}_LL2AA:;_LL2AB:
# 2623
 return t;_LL2A7:;}
# 2630
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2634
struct _RegionHandle _tmp57D=_new_region("temp");struct _RegionHandle*temp=& _tmp57D;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2640
const char*_tmp9CC;void*_tmp9CB;(_tmp9CB=0,Cyc_Tcutil_terr(e->loc,((_tmp9CC="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9CC,sizeof(char),49))),_tag_dyneither(_tmp9CB,sizeof(void*),0)));}{
# 2643
void*_tmp580=e->r;_LL2AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp581=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp580;if(_tmp581->tag != 25)goto _LL2AF;}_LL2AE:
 goto _LL2B0;_LL2AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp582=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp580;if(_tmp582->tag != 26)goto _LL2B1;}_LL2B0:
 goto _LL2B2;_LL2B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp583=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp580;if(_tmp583->tag != 0)goto _LL2B3;else{if(((_tmp583->f1).Wstring_c).tag != 9)goto _LL2B3;}}_LL2B2:
 goto _LL2B4;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp584=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp580;if(_tmp584->tag != 0)goto _LL2B5;else{if(((_tmp584->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4: {
void*_tmp585=t;_npop_handler(0);return _tmp585;}_LL2B5:;_LL2B6:
# 2649
 t=Cyc_Tcutil_compress(t);{
void*_tmp586=t;void*_tmp588;struct Cyc_Absyn_Tqual _tmp589;struct Cyc_Absyn_Exp*_tmp58A;union Cyc_Absyn_Constraint*_tmp58B;_LL2B8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp587=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp586;if(_tmp587->tag != 8)goto _LL2BA;else{_tmp588=(_tmp587->f1).elt_type;_tmp589=(_tmp587->f1).tq;_tmp58A=(_tmp587->f1).num_elts;_tmp58B=(_tmp587->f1).zero_term;}}_LL2B9: {
# 2652
void*_tmp58D;struct _tuple14 _tmp58C=Cyc_Tcutil_addressof_props(te,e);_tmp58D=_tmp58C.f2;{
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9CF;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9CE;void*b=_tmp58A == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
(_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=1,((_tmp9CF.f1=_tmp58A,_tmp9CF)))),_tmp9CE))));
t=Cyc_Absyn_atb_typ(_tmp588,_tmp58D,_tmp589,b,_tmp58B);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
void*_tmp58E=t;_npop_handler(0);return _tmp58E;};};}_LL2BA:;_LL2BB: {
# 2659
void*_tmp591=t;_npop_handler(0);return _tmp591;}_LL2B7:;};_LL2AC:;};
# 2634
;_pop_region(temp);}
# 2671
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp592=e->r;struct Cyc_Absyn_Exp*_tmp594;_LL2BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp592;if(_tmp593->tag != 11)goto _LL2BF;else{_tmp594=_tmp593->f1;}}_LL2BE:
# 2675
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp594);
_tmp594->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp594->topt),0);
e->topt=_tmp594->topt;
goto _LL2BC;_LL2BF:;_LL2C0:
# 2681
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2684
{void*_tmp595=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp597;struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59C;_LL2C2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 5)goto _LL2C4;else{_tmp597=(_tmp596->f1).elt_typ;_tmp598=(_tmp596->f1).elt_tq;_tmp599=((_tmp596->f1).ptr_atts).rgn;_tmp59A=((_tmp596->f1).ptr_atts).nullable;_tmp59B=((_tmp596->f1).ptr_atts).bounds;_tmp59C=((_tmp596->f1).ptr_atts).zero_term;}}_LL2C3:
# 2686
{void*_tmp59D=Cyc_Tcutil_compress(_tmp597);struct Cyc_List_List*_tmp59F;void*_tmp5A0;struct Cyc_Absyn_Tqual _tmp5A1;void*_tmp5A2;struct Cyc_List_List*_tmp5A3;int _tmp5A4;struct Cyc_Absyn_VarargInfo*_tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_List_List*_tmp5A7;_LL2C7: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59D;if(_tmp59E->tag != 9)goto _LL2C9;else{_tmp59F=(_tmp59E->f1).tvars;_tmp5A0=(_tmp59E->f1).effect;_tmp5A1=(_tmp59E->f1).ret_tqual;_tmp5A2=(_tmp59E->f1).ret_typ;_tmp5A3=(_tmp59E->f1).args;_tmp5A4=(_tmp59E->f1).c_varargs;_tmp5A5=(_tmp59E->f1).cyc_varargs;_tmp5A6=(_tmp59E->f1).rgn_po;_tmp5A7=(_tmp59E->f1).attributes;}}_LL2C8:
# 2688
 if(_tmp59F != 0){
struct _RegionHandle _tmp5A8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5A8;_push_region(rgn);
{struct _tuple12 _tmp9D0;struct _tuple12 _tmp5A9=(_tmp9D0.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9D0.f2=rgn,_tmp9D0)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5A9,_tmp59F);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9D6;struct Cyc_Absyn_FnInfo _tmp9D5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9D4;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D6.tag=9,((_tmp9D6.f1=((_tmp9D5.tvars=0,((_tmp9D5.effect=_tmp5A0,((_tmp9D5.ret_tqual=_tmp5A1,((_tmp9D5.ret_typ=_tmp5A2,((_tmp9D5.args=_tmp5A3,((_tmp9D5.c_varargs=_tmp5A4,((_tmp9D5.cyc_varargs=_tmp5A5,((_tmp9D5.rgn_po=_tmp5A6,((_tmp9D5.attributes=_tmp5A7,_tmp9D5)))))))))))))))))),_tmp9D6)))),_tmp9D4)))));
# 2698
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9E0;struct Cyc_Absyn_PtrAtts _tmp9DF;struct Cyc_Absyn_PtrInfo _tmp9DE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9DD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AA=(_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9E0.tag=5,((_tmp9E0.f1=((_tmp9DE.elt_typ=ftyp,((_tmp9DE.elt_tq=_tmp598,((_tmp9DE.ptr_atts=((_tmp9DF.rgn=_tmp599,((_tmp9DF.nullable=_tmp59A,((_tmp9DF.bounds=_tmp59B,((_tmp9DF.zero_term=_tmp59C,((_tmp9DF.ptrloc=0,_tmp9DF)))))))))),_tmp9DE)))))),_tmp9E0)))),_tmp9DD)));
# 2700
struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9E3;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9E2;e->r=(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=12,((_tmp9E3.f1=_tmp5AB,((_tmp9E3.f2=ts,_tmp9E3)))))),_tmp9E2))));}
e->topt=(void*)_tmp5AA;}
# 2690
;_pop_region(rgn);}
# 2705
goto _LL2C6;_LL2C9:;_LL2CA:
 goto _LL2C6;_LL2C6:;}
# 2708
goto _LL2C1;_LL2C4:;_LL2C5:
 goto _LL2C1;_LL2C1:;}
# 2711
goto _LL2BC;_LL2BC:;}
# 2713
return(void*)_check_null(e->topt);}
# 2721
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2723
struct _RegionHandle _tmp5B6=_new_region("r");struct _RegionHandle*r=& _tmp5B6;_push_region(r);
{struct Cyc_List_List*_tmp5B7=0;
# 2726
{void*_tmp5B8=fn_exp->r;struct Cyc_List_List*_tmp5BA;_LL2CC: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B9=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5B8;if(_tmp5B9->tag != 9)goto _LL2CE;else{_tmp5BA=_tmp5B9->f2;}}_LL2CD:
# 2728
{void*_tmp5BB=e->r;struct Cyc_List_List*_tmp5BD;_LL2D1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5BC=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5BB;if(_tmp5BC->tag != 9)goto _LL2D3;else{_tmp5BD=_tmp5BC->f2;}}_LL2D2: {
# 2730
struct Cyc_List_List*_tmp5BE=alias_arg_exps;
int _tmp5BF=0;
while(_tmp5BE != 0){
while(_tmp5BF != (int)_tmp5BE->hd){
if(_tmp5BA == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp9ED;struct Cyc_Int_pa_PrintArg_struct _tmp9EC;void*_tmp9EB[2];const char*_tmp9EA;void*_tmp9E9;(_tmp9E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp9ED.tag=1,((_tmp9ED.f1=(unsigned long)((int)_tmp5BE->hd),((_tmp9EC.tag=1,((_tmp9EC.f1=(unsigned long)_tmp5BF,((_tmp9EB[0]=& _tmp9EC,((_tmp9EB[1]=& _tmp9ED,Cyc_aprintf(((_tmp9EA="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9EA,sizeof(char),36))),_tag_dyneither(_tmp9EB,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9E9,sizeof(void*),0)));}
# 2737
++ _tmp5BF;
_tmp5BA=_tmp5BA->tl;
_tmp5BD=((struct Cyc_List_List*)_check_null(_tmp5BD))->tl;}{
# 2742
struct Cyc_Absyn_Decl*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C7;struct _tuple11 _tmp5C5=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BA))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BD))->hd)->topt)));_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;
_tmp5BA->hd=(void*)_tmp5C7;
{struct Cyc_List_List*_tmp9EE;_tmp5B7=((_tmp9EE=_region_malloc(r,sizeof(*_tmp9EE)),((_tmp9EE->hd=_tmp5C6,((_tmp9EE->tl=_tmp5B7,_tmp9EE))))));}
_tmp5BE=_tmp5BE->tl;};}
# 2747
goto _LL2D0;}_LL2D3:;_LL2D4: {
const char*_tmp9F1;void*_tmp9F0;(_tmp9F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9F1="not a function call!",_tag_dyneither(_tmp9F1,sizeof(char),21))),_tag_dyneither(_tmp9F0,sizeof(void*),0)));}_LL2D0:;}
# 2750
goto _LL2CB;_LL2CE:;_LL2CF: {
const char*_tmp9F4;void*_tmp9F3;(_tmp9F3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9F4="not a function call!",_tag_dyneither(_tmp9F4,sizeof(char),21))),_tag_dyneither(_tmp9F3,sizeof(void*),0)));}_LL2CB:;}
# 2755
while(_tmp5B7 != 0){
struct Cyc_Absyn_Decl*_tmp5CD=(struct Cyc_Absyn_Decl*)_tmp5B7->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5CD,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5B7=_tmp5B7->tl;}
# 2762
e->topt=0;
e->r=fn_exp->r;}
# 2724
;_pop_region(r);}
# 2769
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2773
{void*_tmp5CE=e->r;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_List_List*_tmp5D3;struct Cyc_Absyn_VarargCallInfo**_tmp5D4;int*_tmp5D5;struct Cyc_Core_Opt*_tmp5D7;struct Cyc_List_List*_tmp5D8;union Cyc_Absyn_Cnst*_tmp5DA;struct _tuple1*_tmp5DC;void**_tmp5DD;enum Cyc_Absyn_Primop _tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E2;enum Cyc_Absyn_Incrementor _tmp5E3;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Core_Opt*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E7;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5EE;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_Absyn_VarargCallInfo**_tmp5F8;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_List_List*_tmp5FD;void*_tmp5FF;struct Cyc_Absyn_Exp*_tmp600;enum Cyc_Absyn_Coercion*_tmp601;struct Cyc_Absyn_Exp*_tmp603;struct Cyc_Absyn_Exp**_tmp605;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_Exp*_tmp608;void*_tmp60A;void*_tmp60C;struct Cyc_List_List*_tmp60D;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp611;struct _dyneither_ptr*_tmp612;int*_tmp613;int*_tmp614;struct Cyc_Absyn_Exp*_tmp616;struct _dyneither_ptr*_tmp617;int*_tmp618;int*_tmp619;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Exp*_tmp61C;struct Cyc_List_List*_tmp61E;struct _tuple9*_tmp620;struct Cyc_List_List*_tmp621;struct Cyc_List_List*_tmp623;struct Cyc_Absyn_Stmt*_tmp625;struct Cyc_Absyn_Vardecl*_tmp627;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Exp*_tmp629;int*_tmp62A;struct _tuple1**_tmp62C;struct Cyc_List_List**_tmp62D;struct Cyc_List_List*_tmp62E;struct Cyc_Absyn_Aggrdecl**_tmp62F;void*_tmp631;struct Cyc_List_List*_tmp632;struct Cyc_List_List*_tmp634;struct Cyc_Absyn_Datatypedecl*_tmp635;struct Cyc_Absyn_Datatypefield*_tmp636;struct _tuple1**_tmp638;struct Cyc_Absyn_Enumdecl*_tmp639;struct Cyc_Absyn_Enumfield*_tmp63A;struct _tuple1**_tmp63C;void*_tmp63D;struct Cyc_Absyn_Enumfield*_tmp63E;int*_tmp640;struct Cyc_Absyn_Exp**_tmp641;void***_tmp642;struct Cyc_Absyn_Exp**_tmp643;int*_tmp644;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp649;struct _dyneither_ptr*_tmp64A;void*_tmp64C;_LL2D6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5CF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5CF->tag != 11)goto _LL2D8;else{_tmp5D0=_tmp5CF->f1;}}_LL2D7:
# 2778
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5D0);
return;_LL2D8:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5D1=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5D1->tag != 9)goto _LL2DA;else{_tmp5D2=_tmp5D1->f1;_tmp5D3=_tmp5D1->f2;_tmp5D4=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5D1->f3;_tmp5D5=(int*)& _tmp5D1->f4;}}if(!(!(*_tmp5D5)))goto _LL2DA;_LL2D9:
# 2784
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5D2,_tmp5D3)){
*_tmp5D5=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2788
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2791
return;_LL2DA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5D6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5D6->tag != 34)goto _LL2DC;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=_tmp5D6->f2;}}_LL2DB:
# 2795
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5D8);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2DC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5D9->tag != 0)goto _LL2DE;else{_tmp5DA=(union Cyc_Absyn_Cnst*)& _tmp5D9->f1;}}_LL2DD:
# 2800
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5DA,e);goto _LL2D5;_LL2DE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5DB->tag != 1)goto _LL2E0;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=(void**)((void**)& _tmp5DB->f2);}}_LL2DF:
# 2802
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5DC,_tmp5DD);goto _LL2D5;_LL2E0: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5DE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5DE->tag != 2)goto _LL2E2;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;}}_LL2E1:
# 2804
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5DF,_tmp5E0);goto _LL2D5;_LL2E2: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5E1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5E1->tag != 4)goto _LL2E4;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;}}_LL2E3:
# 2806
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5E2,_tmp5E3);goto _LL2D5;_LL2E4: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5E4=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5E4->tag != 3)goto _LL2E6;else{_tmp5E5=_tmp5E4->f1;_tmp5E6=_tmp5E4->f2;_tmp5E7=_tmp5E4->f3;}}_LL2E5:
# 2808
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5E5,_tmp5E6,_tmp5E7);goto _LL2D5;_LL2E6: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5E8=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5E8->tag != 5)goto _LL2E8;else{_tmp5E9=_tmp5E8->f1;_tmp5EA=_tmp5E8->f2;_tmp5EB=_tmp5E8->f3;}}_LL2E7:
# 2810
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5E9,_tmp5EA,_tmp5EB);goto _LL2D5;_LL2E8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5EC->tag != 6)goto _LL2EA;else{_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;}}_LL2E9:
# 2812
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5ED,_tmp5EE);goto _LL2D5;_LL2EA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5EF=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5EF->tag != 7)goto _LL2EC;else{_tmp5F0=_tmp5EF->f1;_tmp5F1=_tmp5EF->f2;}}_LL2EB:
# 2814
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5F0,_tmp5F1);goto _LL2D5;_LL2EC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5F2=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5F2->tag != 8)goto _LL2EE;else{_tmp5F3=_tmp5F2->f1;_tmp5F4=_tmp5F2->f2;}}_LL2ED:
# 2816
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5F3,_tmp5F4);goto _LL2D5;_LL2EE: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5F5->tag != 9)goto _LL2F0;else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5F5->f3;}}_LL2EF: {
# 2822
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp64E;_push_handler(& _tmp64E);{int _tmp650=0;if(setjmp(_tmp64E.handler))_tmp650=1;if(!_tmp650){
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp64F=(void*)_exn_thrown;void*_tmp652=_tmp64F;struct _dyneither_ptr _tmp654;void*_tmp655;_LL327: {struct Cyc_Core_Failure_exn_struct*_tmp653=(struct Cyc_Core_Failure_exn_struct*)_tmp652;if(_tmp653->tag != Cyc_Core_Failure)goto _LL329;else{_tmp654=_tmp653->f1;}}_LL328:
# 2829
 ok=0;
fn_exp=e;
goto _LL326;_LL329: _tmp655=_tmp652;_LL32A:(void)_throw(_tmp655);_LL326:;}};}
# 2833
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5F6,_tmp5F7,_tmp5F8,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2840
goto _LL2D5;}_LL2F0: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5F9=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5F9->tag != 10)goto _LL2F2;else{_tmp5FA=_tmp5F9->f1;}}_LL2F1:
# 2842
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5FA);goto _LL2D5;_LL2F2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5FB->tag != 12)goto _LL2F4;else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;}}_LL2F3:
# 2844
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5FC,_tmp5FD);goto _LL2D5;_LL2F4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5FE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5FE->tag != 13)goto _LL2F6;else{_tmp5FF=(void*)_tmp5FE->f1;_tmp600=_tmp5FE->f2;_tmp601=(enum Cyc_Absyn_Coercion*)& _tmp5FE->f4;}}_LL2F5:
# 2846
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5FF,_tmp600,_tmp601);goto _LL2D5;_LL2F6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp602->tag != 14)goto _LL2F8;else{_tmp603=_tmp602->f1;}}_LL2F7:
# 2848
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp603);goto _LL2D5;_LL2F8: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp604=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp604->tag != 15)goto _LL2FA;else{_tmp605=(struct Cyc_Absyn_Exp**)& _tmp604->f1;_tmp606=_tmp604->f2;}}_LL2F9:
# 2850
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp605,e,_tmp606);goto _LL2D5;_LL2FA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp607=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp607->tag != 17)goto _LL2FC;else{_tmp608=_tmp607->f1;}}_LL2FB: {
# 2852
void*_tmp656=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp608);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp656);goto _LL2D5;}_LL2FC: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp609=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp609->tag != 16)goto _LL2FE;else{_tmp60A=(void*)_tmp609->f1;}}_LL2FD:
# 2855
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp60A);goto _LL2D5;_LL2FE: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp60B->tag != 18)goto _LL300;else{_tmp60C=(void*)_tmp60B->f1;_tmp60D=_tmp60B->f2;}}_LL2FF:
# 2857
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp60C,_tmp60D);goto _LL2D5;_LL300: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp60E->tag != 19)goto _LL302;else{_tmp60F=_tmp60E->f1;}}_LL301:
# 2859
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp60F);goto _LL2D5;_LL302: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp610->tag != 20)goto _LL304;else{_tmp611=_tmp610->f1;_tmp612=_tmp610->f2;_tmp613=(int*)& _tmp610->f3;_tmp614=(int*)& _tmp610->f4;}}_LL303:
# 2861
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp611,_tmp612,_tmp613,_tmp614);goto _LL2D5;_LL304: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp615=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp615->tag != 21)goto _LL306;else{_tmp616=_tmp615->f1;_tmp617=_tmp615->f2;_tmp618=(int*)& _tmp615->f3;_tmp619=(int*)& _tmp615->f4;}}_LL305:
# 2863
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp616,_tmp617,_tmp618,_tmp619);goto _LL2D5;_LL306: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp61A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp61A->tag != 22)goto _LL308;else{_tmp61B=_tmp61A->f1;_tmp61C=_tmp61A->f2;}}_LL307:
# 2865
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp61B,_tmp61C);goto _LL2D5;_LL308: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp61D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp61D->tag != 23)goto _LL30A;else{_tmp61E=_tmp61D->f1;}}_LL309:
# 2867
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp61E);goto _LL2D5;_LL30A: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp61F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp61F->tag != 24)goto _LL30C;else{_tmp620=_tmp61F->f1;_tmp621=_tmp61F->f2;}}_LL30B:
# 2869
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp620,_tmp621);goto _LL2D5;_LL30C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp622=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp622->tag != 25)goto _LL30E;else{_tmp623=_tmp622->f1;}}_LL30D: {
# 2873
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp657=Cyc_Tcutil_compress(*topt);void**_tmp659;struct Cyc_Absyn_Tqual*_tmp65A;union Cyc_Absyn_Constraint*_tmp65B;_LL32C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp658=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp657;if(_tmp658->tag != 8)goto _LL32E;else{_tmp659=(void**)&(_tmp658->f1).elt_type;_tmp65A=(struct Cyc_Absyn_Tqual*)&(_tmp658->f1).tq;_tmp65B=(_tmp658->f1).zero_term;}}_LL32D:
# 2879
 elt_topt=_tmp659;
elt_tqopt=_tmp65A;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp65B);
goto _LL32B;_LL32E:;_LL32F:
 goto _LL32B;_LL32B:;}
# 2886
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp623);goto _LL2D5;}_LL30E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp624=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp624->tag != 35)goto _LL310;else{_tmp625=_tmp624->f1;}}_LL30F:
# 2888
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp625);goto _LL2D5;_LL310: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp626=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp626->tag != 26)goto _LL312;else{_tmp627=_tmp626->f1;_tmp628=_tmp626->f2;_tmp629=_tmp626->f3;_tmp62A=(int*)& _tmp626->f4;}}_LL311:
# 2890
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp627,_tmp628,_tmp629,_tmp62A);goto _LL2D5;_LL312: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp62B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp62B->tag != 27)goto _LL314;else{_tmp62C=(struct _tuple1**)& _tmp62B->f1;_tmp62D=(struct Cyc_List_List**)& _tmp62B->f2;_tmp62E=_tmp62B->f3;_tmp62F=(struct Cyc_Absyn_Aggrdecl**)& _tmp62B->f4;}}_LL313:
# 2892
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp62C,_tmp62D,_tmp62E,_tmp62F);goto _LL2D5;_LL314: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp630->tag != 28)goto _LL316;else{_tmp631=(void*)_tmp630->f1;_tmp632=_tmp630->f2;}}_LL315:
# 2894
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp631,_tmp632);goto _LL2D5;_LL316: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp633=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp633->tag != 29)goto _LL318;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;_tmp636=_tmp633->f3;}}_LL317:
# 2896
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp634,_tmp635,_tmp636);goto _LL2D5;_LL318: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp637=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp637->tag != 30)goto _LL31A;else{_tmp638=(struct _tuple1**)& _tmp637->f1;_tmp639=_tmp637->f2;_tmp63A=_tmp637->f3;}}_LL319:
# 2898
*_tmp638=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp63A))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9F7;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9F6;t=(void*)((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9F7.tag=13,((_tmp9F7.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp639))->name,((_tmp9F7.f2=_tmp639,_tmp9F7)))))),_tmp9F6))));}goto _LL2D5;_LL31A: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp63B->tag != 31)goto _LL31C;else{_tmp63C=(struct _tuple1**)& _tmp63B->f1;_tmp63D=(void*)_tmp63B->f2;_tmp63E=_tmp63B->f3;}}_LL31B:
# 2901
*_tmp63C=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp63E))->name;
t=_tmp63D;goto _LL2D5;_LL31C: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp63F->tag != 32)goto _LL31E;else{_tmp640=(int*)&(_tmp63F->f1).is_calloc;_tmp641=(struct Cyc_Absyn_Exp**)&(_tmp63F->f1).rgn;_tmp642=(void***)&(_tmp63F->f1).elt_type;_tmp643=(struct Cyc_Absyn_Exp**)&(_tmp63F->f1).num_elts;_tmp644=(int*)&(_tmp63F->f1).fat_result;}}_LL31D:
# 2904
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp641,_tmp642,_tmp643,_tmp640,_tmp644);goto _LL2D5;_LL31E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp645->tag != 33)goto _LL320;else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;}}_LL31F:
# 2906
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp646,_tmp647);goto _LL2D5;_LL320: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp648->tag != 36)goto _LL322;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;}}_LL321:
# 2908
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp649,_tmp64A);goto _LL2D5;_LL322: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp64B->tag != 37)goto _LL324;else{_tmp64C=(void*)_tmp64B->f1;}}_LL323:
# 2910
 if(!te->allow_valueof){
const char*_tmp9FA;void*_tmp9F9;(_tmp9F9=0,Cyc_Tcutil_terr(e->loc,((_tmp9FA="valueof(-) can only occur within types",_tag_dyneither(_tmp9FA,sizeof(char),39))),_tag_dyneither(_tmp9F9,sizeof(void*),0)));}
# 2918
t=Cyc_Absyn_sint_typ;
goto _LL2D5;_LL324: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp64D->tag != 38)goto _LL2D5;}_LL325:
# 2921
{const char*_tmp9FD;void*_tmp9FC;(_tmp9FC=0,Cyc_Tcutil_terr(e->loc,((_tmp9FD="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9FD,sizeof(char),50))),_tag_dyneither(_tmp9FC,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2D5:;}
# 2924
e->topt=t;}
