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
# 922
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
# 297
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 324
int Cyc_Tcutil_bits_only(void*t);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 356
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 359
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple15{unsigned int f1;int f2;};
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
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 120
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 127
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 162
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp665;(_tmp665=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp665,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp666;return(_tmp666=_cycalloc(sizeof(*_tmp666)),((_tmp666->f1=0,((_tmp666->f2=e,_tmp666)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_stmttmp0=e1->r;void*_tmp2=_stmttmp0;struct _tuple1*_tmp4;void**_tmp5;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp6;_push_handler(& _tmp6);{int _tmp8=0;if(setjmp(_tmp6.handler))_tmp8=1;if(!_tmp8){
{struct _RegionHandle _tmp9=_new_region("r");struct _RegionHandle*r=& _tmp9;_push_region(r);
{void*_stmttmp1=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpA=_stmttmp1;void*_tmpE;struct Cyc_Absyn_Datatypedecl*_tmp10;struct Cyc_Absyn_Datatypefield*_tmp11;struct Cyc_Absyn_Aggrdecl*_tmp13;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpB->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpB->f1);if(_tmpC->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp66A;void*_tmp669[1];struct Cyc_String_pa_PrintArg_struct _tmp668;(_tmp668.tag=0,((_tmp668.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp669[0]=& _tmp668,Cyc_Tcutil_terr(e1->loc,((_tmp66A="undeclared identifier %s",_tag_dyneither(_tmp66A,sizeof(char),25))),_tag_dyneither(_tmp669,sizeof(void*),1)))))));}{
int _tmp19=0;_npop_handler(1);return _tmp19;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpD->tag != 0)goto _LLA;else{_tmpE=(void*)_tmpD->f1;}}_LL9:
# 86
*_tmp5=_tmpE;{
int _tmp1A=1;_npop_handler(1);return _tmp1A;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA;if(_tmpF->tag != 2)goto _LLC;else{_tmp10=_tmpF->f1;_tmp11=_tmpF->f2;}}_LLB:
# 89
 if(_tmp11->typs == 0){
# 91
{const char*_tmp66E;void*_tmp66D[1];struct Cyc_String_pa_PrintArg_struct _tmp66C;(_tmp66C.tag=0,((_tmp66C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp11->name)),((_tmp66D[0]=& _tmp66C,Cyc_Tcutil_terr(e->loc,((_tmp66E="%s is a constant, not a function",_tag_dyneither(_tmp66E,sizeof(char),33))),_tag_dyneither(_tmp66D,sizeof(void*),1)))))));}{
int _tmp1E=0;_npop_handler(1);return _tmp1E;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp671;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp670;e->r=(void*)((_tmp670=_cycalloc(sizeof(*_tmp670)),((_tmp670[0]=((_tmp671.tag=30,((_tmp671.f1=es,((_tmp671.f2=_tmp10,((_tmp671.f3=_tmp11,_tmp671)))))))),_tmp670))));}{
int _tmp21=1;_npop_handler(1);return _tmp21;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp12=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp12->tag != 1)goto _LLE;else{_tmp13=_tmp12->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp22=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp674;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp673;e->r=(void*)((_tmp673=_cycalloc(sizeof(*_tmp673)),((_tmp673[0]=((_tmp674.tag=28,((_tmp674.f1=_tmp13->name,((_tmp674.f2=0,((_tmp674.f3=_tmp22,((_tmp674.f4=_tmp13,_tmp674)))))))))),_tmp673))));}{
int _tmp25=1;_npop_handler(1);return _tmp25;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp14->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp15=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA;if(_tmp15->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp678;void*_tmp677[1];struct Cyc_String_pa_PrintArg_struct _tmp676;(_tmp676.tag=0,((_tmp676.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp677[0]=& _tmp676,Cyc_Tcutil_terr(e->loc,((_tmp678="%s is an enum constructor, not a function",_tag_dyneither(_tmp678,sizeof(char),42))),_tag_dyneither(_tmp677,sizeof(void*),1)))))));}{
int _tmp29=0;_npop_handler(1);return _tmp29;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp7=(void*)_exn_thrown;void*_tmp2B=_tmp7;void*_tmp2D;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2C=(struct Cyc_Dict_Absent_exn_struct*)_tmp2B;if(_tmp2C->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp67C;void*_tmp67B[1];struct Cyc_String_pa_PrintArg_struct _tmp67A;(_tmp67A.tag=0,((_tmp67A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp67B[0]=& _tmp67A,Cyc_Tcutil_terr(e1->loc,((_tmp67C="undeclared identifier %s",_tag_dyneither(_tmp67C,sizeof(char),25))),_tag_dyneither(_tmp67B,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2D=_tmp2B;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp67F;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp67E;e->r=(void*)((_tmp67E=_cycalloc(sizeof(*_tmp67E)),((_tmp67E[0]=((_tmp67F.tag=25,((_tmp67F.f1=des,_tmp67F)))),_tmp67E))));}
return;}{
# 125
void*t=*topt;
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp33=_stmttmp2;union Cyc_Absyn_AggrInfoU _tmp35;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp34=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33;if(_tmp34->tag != 11)goto _LL1A;else{_tmp35=(_tmp34->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3A=_tmp35;struct Cyc_Absyn_Aggrdecl*_tmp3B;_LL21: if((_tmp3A.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp682;void*_tmp681;(_tmp681=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp682="struct type not properly set",_tag_dyneither(_tmp682,sizeof(char),29))),_tag_dyneither(_tmp681,sizeof(void*),0)));}_LL23: if((_tmp3A.KnownAggr).tag != 2)goto _LL20;_tmp3B=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3A.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp685;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp684;e->r=(void*)((_tmp684=_cycalloc(sizeof(*_tmp684)),((_tmp684[0]=((_tmp685.tag=28,((_tmp685.f1=_tmp3B->name,((_tmp685.f2=0,((_tmp685.f3=des,((_tmp685.f4=_tmp3B,_tmp685)))))))))),_tmp684))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp33;if(_tmp36->tag != 8)goto _LL1C;else{_tmp37=(_tmp36->f1).elt_type;_tmp38=(_tmp36->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp688;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp687;e->r=(void*)((_tmp687=_cycalloc(sizeof(*_tmp687)),((_tmp687[0]=((_tmp688.tag=25,((_tmp688.f1=des,_tmp688)))),_tmp687))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33;if(_tmp39->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp68B;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp68A;e->r=(void*)((_tmp68A=_cycalloc(sizeof(*_tmp68A)),((_tmp68A[0]=((_tmp68B.tag=29,((_tmp68B.f1=t,((_tmp68B.f2=des,_tmp68B)))))),_tmp68A))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp68E;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp68D;e->r=(void*)((_tmp68D=_cycalloc(sizeof(*_tmp68D)),((_tmp68D[0]=((_tmp68E.tag=25,((_tmp68E.f1=des,_tmp68E)))),_tmp68D))));}goto _LL17;_LL17:;};}
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
const char*_tmp693;void*_tmp692[2];struct Cyc_String_pa_PrintArg_struct _tmp691;struct Cyc_String_pa_PrintArg_struct _tmp690;(_tmp690.tag=0,((_tmp690.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp691.tag=0,((_tmp691.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp692[0]=& _tmp691,((_tmp692[1]=& _tmp690,Cyc_Tcutil_terr(e->loc,((_tmp693="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp693,sizeof(char),59))),_tag_dyneither(_tmp692,sizeof(void*),2)))))))))))));}
# 161
{void*_stmttmp3=e->r;void*_tmp4A=_stmttmp3;enum Cyc_Absyn_Primop _tmp4C;struct Cyc_List_List*_tmp4D;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4A;if(_tmp4B->tag != 2)goto _LL28;else{_tmp4C=_tmp4B->f1;_tmp4D=_tmp4B->f2;}}_LL27:
# 163
 if(_tmp4C == Cyc_Absyn_Eq  || _tmp4C == Cyc_Absyn_Neq){
struct _tuple0 _tmp694;struct _tuple0 _stmttmp4=(_tmp694.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D))->hd)->topt),((_tmp694.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp4D))->tl))->hd)->topt),_tmp694)));struct _tuple0 _tmp4E=_stmttmp4;void*_tmp50;void*_tmp52;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f1;if(_tmp4F->tag != 15)goto _LL2D;else{_tmp50=(void*)_tmp4F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp51=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4E.f2;if(_tmp51->tag != 15)goto _LL2D;else{_tmp52=(void*)_tmp51->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp697;struct Cyc_Tcexp_TestEnv _tmp696;return(_tmp696.eq=((_tmp697=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp697)),((_tmp697->f1=_tmp50,((_tmp697->f2=_tmp52,_tmp697)))))),((_tmp696.isTrue=_tmp4C == Cyc_Absyn_Eq,_tmp696)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp698;return(_tmp698.eq=0,((_tmp698.isTrue=0,_tmp698)));};}
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
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp69E;struct Cyc_Absyn_PtrInfo _tmp69D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69C;t=(void*)((_tmp69C=_cycalloc(sizeof(*_tmp69C)),((_tmp69C[0]=((_tmp69E.tag=5,((_tmp69E.f1=((_tmp69D.elt_typ=_tmp69,((_tmp69D.elt_tq=_tmp6A,((_tmp69D.ptr_atts=(
((_tmp69D.ptr_atts).rgn=_tmp6B,(((_tmp69D.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp69D.ptr_atts).bounds=_tmp6D,(((_tmp69D.ptr_atts).zero_term=_tmp6E,(((_tmp69D.ptr_atts).ptrloc=0,_tmp69D.ptr_atts)))))))))),_tmp69D)))))),_tmp69E)))),_tmp69C))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F;if(_tmp6F->tag != 19)goto _LL53;else{_tmp70=(void*)_tmp6F->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6A1;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6A0;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp76=(_tmp6A0=_cycalloc(sizeof(*_tmp6A0)),((_tmp6A0[0]=((_tmp6A1.tag=18,((_tmp6A1.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0),_tmp6A1)))),_tmp6A0)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6A4;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6A3;t=(void*)((_tmp6A3=_cycalloc(sizeof(*_tmp6A3)),((_tmp6A3[0]=((_tmp6A4.tag=19,((_tmp6A4.f1=(void*)_tmp76,_tmp6A4)))),_tmp6A3))));}
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
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp6A7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A6;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7B=(_tmp6A6=_cycalloc(sizeof(*_tmp6A6)),((_tmp6A6[0]=((_tmp6A7.tag=1,((_tmp6A7.f1=elen,_tmp6A7)))),_tmp6A6)));
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
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6AD;struct Cyc_Absyn_PtrInfo _tmp6AC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AB;return(void*)((_tmp6AB=_cycalloc(sizeof(*_tmp6AB)),((_tmp6AB[0]=((_tmp6AD.tag=5,((_tmp6AD.f1=((_tmp6AC.elt_typ=_tmp84,((_tmp6AC.elt_tq=_tmp85,((_tmp6AC.ptr_atts=(((_tmp6AC.ptr_atts).rgn=_tmp86,(((_tmp6AC.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6AC.ptr_atts).bounds=_tmp88,(((_tmp6AC.ptr_atts).zero_term=_tmp89,(((_tmp6AC.ptr_atts).ptrloc=0,_tmp6AC.ptr_atts)))))))))),_tmp6AC)))))),_tmp6AD)))),_tmp6AB))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp8D=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6BC;struct Cyc_Core_Opt*_tmp6BB;struct Cyc_Core_Opt*_tmp6BA;struct Cyc_Absyn_PtrInfo _tmp6B9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B8;t=(void*)((_tmp6B8=_cycalloc(sizeof(*_tmp6B8)),((_tmp6B8[0]=((_tmp6BC.tag=5,((_tmp6BC.f1=((_tmp6B9.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp6BA=_cycalloc(sizeof(*_tmp6BA)),((_tmp6BA->v=_tmp8D,_tmp6BA))))),((_tmp6B9.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp6B9.ptr_atts=(
((_tmp6B9.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp6BB=_cycalloc(sizeof(*_tmp6BB)),((_tmp6BB->v=_tmp8D,_tmp6BB))))),(((_tmp6B9.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6B9.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6B9.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp6B9.ptr_atts).ptrloc=0,_tmp6B9.ptr_atts)))))))))),_tmp6B9)))))),_tmp6BC)))),_tmp6B8))));}
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
const char*_tmp6C0;void*_tmp6BF[1];struct Cyc_String_pa_PrintArg_struct _tmp6BE;void*_tmpB5=(_tmp6BE.tag=0,((_tmp6BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6BF[0]=& _tmp6BE,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6C0="undeclared identifier %s",_tag_dyneither(_tmp6C0,sizeof(char),25))),_tag_dyneither(_tmp6BF,sizeof(void*),1)))))));_npop_handler(1);return _tmpB5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpA6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA6->tag != 0)goto _LL77;else{_tmpA7=(void*)_tmpA6->f1;}}_LL76:
# 348
*b=_tmpA7;{
void*_tmpB6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpB6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpA8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpA8->tag != 3)goto _LL79;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp6C3;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6C2;e->r=(void*)((_tmp6C2=_cycalloc(sizeof(*_tmp6C2)),((_tmp6C2[0]=((_tmp6C3.tag=31,((_tmp6C3.f1=_tmpAA->name,((_tmp6C3.f2=_tmpA9,((_tmp6C3.f3=_tmpAA,_tmp6C3)))))))),_tmp6C2))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp6C6;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6C5;void*_tmpBB=(void*)((_tmp6C5=_cycalloc(sizeof(*_tmp6C5)),((_tmp6C5[0]=((_tmp6C6.tag=13,((_tmp6C6.f1=_tmpA9->name,((_tmp6C6.f2=_tmpA9,_tmp6C6)))))),_tmp6C5))));_npop_handler(1);return _tmpBB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpAB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAB->tag != 4)goto _LL7B;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp6C9;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6C8;e->r=(void*)((_tmp6C8=_cycalloc(sizeof(*_tmp6C8)),((_tmp6C8[0]=((_tmp6C9.tag=32,((_tmp6C9.f1=_tmpAD->name,((_tmp6C9.f2=_tmpAC,((_tmp6C9.f3=_tmpAD,_tmp6C9)))))))),_tmp6C8))));}{
void*_tmpBE=_tmpAC;_npop_handler(1);return _tmpBE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpAE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpAE->tag != 2)goto _LL7D;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6CC;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6CB;e->r=(void*)((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB[0]=((_tmp6CC.tag=30,((_tmp6CC.f1=0,((_tmp6CC.f2=_tmpAF,((_tmp6CC.f3=_tmpB0,_tmp6CC)))))))),_tmp6CB))));}{
void*_tmpC1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpAF,_tmpB0);_npop_handler(1);return _tmpC1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpA3;if(_tmpB1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp6D0;void*_tmp6CF[1];struct Cyc_String_pa_PrintArg_struct _tmp6CE;void*_tmpC5=(_tmp6CE.tag=0,((_tmp6CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6CF[0]=& _tmp6CE,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6D0="bad occurrence of type name %s",_tag_dyneither(_tmp6D0,sizeof(char),31))),_tag_dyneither(_tmp6CF,sizeof(void*),1)))))));_npop_handler(1);return _tmpC5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpA0=(void*)_exn_thrown;void*_tmpC7=_tmpA0;void*_tmpC9;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpC8=(struct Cyc_Dict_Absent_exn_struct*)_tmpC7;if(_tmpC8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp6D4;void*_tmp6D3[1];struct Cyc_String_pa_PrintArg_struct _tmp6D2;return(_tmp6D2.tag=0,((_tmp6D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp6D3[0]=& _tmp6D2,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6D4="undeclared identifier %s",_tag_dyneither(_tmp6D4,sizeof(char),25))),_tag_dyneither(_tmp6D3,sizeof(void*),1)))))));}_LL82: _tmpC9=_tmpC7;_LL83:(void)_throw(_tmpC9);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp95=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL6A;else{_tmp96=_tmp95->f1;}}_LL69:
# 368
*q=*_tmp96->name;
return _tmp96->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp97=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp93;if(_tmp97->tag != 2)goto _LL6C;else{_tmp98=_tmp97->f1;}}_LL6B:
*q=*_tmp98->name;return Cyc_Tcutil_fndecl2typ(_tmp98);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp99=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp93;if(_tmp99->tag != 5)goto _LL6E;else{_tmp9A=_tmp99->f1;}}_LL6D:
 _tmp9C=_tmp9A;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp9B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp93;if(_tmp9B->tag != 4)goto _LL70;else{_tmp9C=_tmp9B->f1;}}_LL6F:
 _tmp9E=_tmp9C;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp9D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp93;if(_tmp9D->tag != 3)goto _LL65;else{_tmp9E=_tmp9D->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp6D5;(*q).f1=(((_tmp6D5.Loc_n).val=0,(((_tmp6D5.Loc_n).tag=4,_tmp6D5))));}
if(te->allow_valueof){
void*_stmttmpA=Cyc_Tcutil_compress(_tmp9E->type);void*_tmpCE=_stmttmpA;void*_tmpD0;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpCE;if(_tmpCF->tag != 19)goto _LL87;else{_tmpD0=(void*)_tmpCF->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6D8;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6D7;e->r=(void*)((_tmp6D7=_cycalloc(sizeof(*_tmp6D7)),((_tmp6D7[0]=((_tmp6D8.tag=38,((_tmp6D8.f1=_tmpD0,_tmp6D8)))),_tmp6D7))));}
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
const char*_tmp6DB;void*_tmp6DA;(_tmp6DA=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpD9->hd)->loc,((_tmp6DB="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6DB,sizeof(char),49))),_tag_dyneither(_tmp6DA,sizeof(void*),0)));}
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
{const char*_tmp6E0;void*_tmp6DF[2];struct Cyc_String_pa_PrintArg_struct _tmp6DE;struct Cyc_String_pa_PrintArg_struct _tmp6DD;(_tmp6DD.tag=0,((_tmp6DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6DE.tag=0,((_tmp6DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6DF[0]=& _tmp6DE,((_tmp6DF[1]=& _tmp6DD,Cyc_Tcutil_terr(e->loc,((_tmp6E0="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp6E0,sizeof(char),48))),_tag_dyneither(_tmp6DF,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp6E1;*alias_arg_exps=((_tmp6E1=_cycalloc(sizeof(*_tmp6E1)),((_tmp6E1->hd=(void*)arg_cnt,((_tmp6E1->tl=*alias_arg_exps,_tmp6E1))))));}{
struct _RegionHandle _tmpE3=_new_region("temp");struct _RegionHandle*temp=& _tmpE3;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp6E4;void*_tmp6E3;(_tmp6E3=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6E4="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp6E4,sizeof(char),49))),_tag_dyneither(_tmp6E3,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp6E7;void*_tmp6E6;(_tmp6E6=0,Cyc_Tcutil_terr(fmt->loc,((_tmp6E7="too few arguments",_tag_dyneither(_tmp6E7,sizeof(char),18))),_tag_dyneither(_tmp6E6,sizeof(void*),0)));}
if(_tmpDD != 0){
{const char*_tmp6EA;void*_tmp6E9;(_tmp6E9=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDD->hd)->loc,((_tmp6EA="too many arguments",_tag_dyneither(_tmp6EA,sizeof(char),19))),_tag_dyneither(_tmp6E9,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp6ED;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp6EC;(int)_throw((void*)((_tmp6EC=_cycalloc_atomic(sizeof(*_tmp6EC)),((_tmp6EC[0]=((_tmp6ED.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp6ED)),_tmp6EC)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp6F1;void*_tmp6F0[1];struct Cyc_String_pa_PrintArg_struct _tmp6EF;(_tmp6EF.tag=0,((_tmp6EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F0[0]=& _tmp6EF,Cyc_Tcutil_terr(loc,((_tmp6F1="expecting numeric type but found %s",_tag_dyneither(_tmp6F1,sizeof(char),36))),_tag_dyneither(_tmp6F0,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6F5;void*_tmp6F4[1];struct Cyc_String_pa_PrintArg_struct _tmp6F3;(_tmp6F3.tag=0,((_tmp6F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F4[0]=& _tmp6F3,Cyc_Tcutil_terr(loc,((_tmp6F5="expecting integral or * type but found %s",_tag_dyneither(_tmp6F5,sizeof(char),42))),_tag_dyneither(_tmp6F4,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp6F9;void*_tmp6F8[1];struct Cyc_String_pa_PrintArg_struct _tmp6F7;(_tmp6F7.tag=0,((_tmp6F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6F8[0]=& _tmp6F7,Cyc_Tcutil_terr(loc,((_tmp6F9="expecting integral type but found %s",_tag_dyneither(_tmp6F9,sizeof(char),37))),_tag_dyneither(_tmp6F8,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmpF5=t;union Cyc_Absyn_Constraint*_tmpF8;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF5;if(_tmpF6->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF5;if(_tmpF7->tag != 5)goto _LL9B;else{_tmpF8=((_tmpF7->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_stmttmpD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpF8);void*_tmpF9=_stmttmpD;struct Cyc_Absyn_Exp*_tmpFC;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpFA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpF9;if(_tmpFA->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpFB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpF9;if(_tmpFB->tag != 1)goto _LL9D;else{_tmpFC=_tmpFB->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmpFC)){
const char*_tmp6FC;void*_tmp6FB;(_tmp6FB=0,Cyc_Tcutil_terr(loc,((_tmp6FC="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp6FC,sizeof(char),55))),_tag_dyneither(_tmp6FB,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_PrintArg_struct _tmp6FE;(_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6FF[0]=& _tmp6FE,Cyc_Tcutil_terr(loc,((_tmp700="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp700,sizeof(char),47))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp703;void*_tmp702;(_tmp702=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp703="Non-unary primop",_tag_dyneither(_tmp703,sizeof(char),17))),_tag_dyneither(_tmp702,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp707;void*_tmp706[1];struct Cyc_String_pa_PrintArg_struct _tmp705;(_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp706[0]=& _tmp705,Cyc_Tcutil_terr(e1->loc,((_tmp707="type %s cannot be used here",_tag_dyneither(_tmp707,sizeof(char),28))),_tag_dyneither(_tmp706,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp70B;void*_tmp70A[1];struct Cyc_String_pa_PrintArg_struct _tmp709;(_tmp709.tag=0,((_tmp709.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp70A[0]=& _tmp709,Cyc_Tcutil_terr(e2->loc,((_tmp70B="type %s cannot be used here",_tag_dyneither(_tmp70B,sizeof(char),28))),_tag_dyneither(_tmp70A,sizeof(void*),1)))))));}
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
const char*_tmp70E;void*_tmp70D;(_tmp70D=0,Cyc_Tcutil_terr(e1->loc,((_tmp70E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp70E,sizeof(char),50))),_tag_dyneither(_tmp70D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp711;void*_tmp710;(_tmp710=0,Cyc_Tcutil_terr(e1->loc,((_tmp711="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp711,sizeof(char),54))),_tag_dyneither(_tmp710,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp715;void*_tmp714[1];struct Cyc_String_pa_PrintArg_struct _tmp713;(_tmp713.tag=0,((_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp714[0]=& _tmp713,Cyc_Tcutil_terr(e2->loc,((_tmp715="expecting int but found %s",_tag_dyneither(_tmp715,sizeof(char),27))),_tag_dyneither(_tmp714,sizeof(void*),1)))))));}{
void*_stmttmpE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp110);void*_tmp119=_stmttmpE;struct Cyc_Absyn_Exp*_tmp11C;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp119;if(_tmp11A->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp119;if(_tmp11B->tag != 1)goto _LLA8;else{_tmp11C=_tmp11B->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp111)){
const char*_tmp718;void*_tmp717;(_tmp717=0,Cyc_Tcutil_warn(e1->loc,((_tmp718="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp718,sizeof(char),70))),_tag_dyneither(_tmp717,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp71E;struct Cyc_Absyn_PtrInfo _tmp71D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11F=(_tmp71C=_cycalloc(sizeof(*_tmp71C)),((_tmp71C[0]=((_tmp71E.tag=5,((_tmp71E.f1=((_tmp71D.elt_typ=_tmp10C,((_tmp71D.elt_tq=_tmp10D,((_tmp71D.ptr_atts=(
((_tmp71D.ptr_atts).rgn=_tmp10E,(((_tmp71D.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp71D.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp71D.ptr_atts).zero_term=_tmp111,(((_tmp71D.ptr_atts).ptrloc=0,_tmp71D.ptr_atts)))))))))),_tmp71D)))))),_tmp71E)))),_tmp71C)));
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
{const char*_tmp723;void*_tmp722[2];struct Cyc_String_pa_PrintArg_struct _tmp721;struct Cyc_String_pa_PrintArg_struct _tmp720;(_tmp720.tag=0,((_tmp720.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp721.tag=0,((_tmp721.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp722[0]=& _tmp721,((_tmp722[1]=& _tmp720,Cyc_Tcutil_terr(e1->loc,((_tmp723="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp723,sizeof(char),59))),_tag_dyneither(_tmp722,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp728;void*_tmp727[2];struct Cyc_String_pa_PrintArg_struct _tmp726;struct Cyc_String_pa_PrintArg_struct _tmp725;(_tmp725.tag=0,((_tmp725.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp726.tag=0,((_tmp726.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp727[0]=& _tmp726,((_tmp727[1]=& _tmp725,Cyc_Tcutil_terr(e1->loc,((_tmp728="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp728,sizeof(char),59))),_tag_dyneither(_tmp727,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp72B;void*_tmp72A;(_tmp72A=0,Cyc_Tcutil_warn(e1->loc,((_tmp72B="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp72B,sizeof(char),60))),_tag_dyneither(_tmp72A,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp72E;void*_tmp72D;(_tmp72D=0,Cyc_Tcutil_terr(e1->loc,((_tmp72E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp72E,sizeof(char),50))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp731;void*_tmp730;(_tmp730=0,Cyc_Tcutil_terr(e1->loc,((_tmp731="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp731,sizeof(char),54))),_tag_dyneither(_tmp730,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp736;void*_tmp735[2];struct Cyc_String_pa_PrintArg_struct _tmp734;struct Cyc_String_pa_PrintArg_struct _tmp733;(_tmp733.tag=0,((_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp734.tag=0,((_tmp734.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp735[0]=& _tmp734,((_tmp735[1]=& _tmp733,Cyc_Tcutil_terr(e2->loc,((_tmp736="expecting either %s or int but found %s",_tag_dyneither(_tmp736,sizeof(char),40))),_tag_dyneither(_tmp735,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp739;void*_tmp738;(_tmp738=0,Cyc_Tcutil_warn(e1->loc,((_tmp739="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp739,sizeof(char),60))),_tag_dyneither(_tmp738,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp73C;void*_tmp73B;(_tmp73B=0,Cyc_Tcutil_warn(e1->loc,((_tmp73C="thin pointer subtraction!",_tag_dyneither(_tmp73C,sizeof(char),26))),_tag_dyneither(_tmp73B,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp73F;void*_tmp73E;(_tmp73E=0,Cyc_Tcutil_warn(e1->loc,((_tmp73F="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp73F,sizeof(char),56))),_tag_dyneither(_tmp73E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp742;void*_tmp741;(_tmp741=0,Cyc_Tcutil_warn(e1->loc,((_tmp742="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp742,sizeof(char),51))),_tag_dyneither(_tmp741,sizeof(void*),0)));}
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
struct _tuple0 _tmp743;struct _tuple0 _stmttmpF=(_tmp743.f1=Cyc_Tcutil_compress(t1),((_tmp743.f2=Cyc_Tcutil_compress(t2),_tmp743)));struct _tuple0 _tmp13D=_stmttmpF;void*_tmp13F;void*_tmp141;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp13E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D.f1;if(_tmp13E->tag != 5)goto _LLB0;else{_tmp13F=(_tmp13E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13D.f2;if(_tmp140->tag != 5)goto _LLB0;else{_tmp141=(_tmp140->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp13F,_tmp141))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp142=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13D.f1;if(_tmp142->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp13D.f2;if(_tmp143->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp748;void*_tmp747[2];struct Cyc_String_pa_PrintArg_struct _tmp746;struct Cyc_String_pa_PrintArg_struct _tmp745;(_tmp745.tag=0,((_tmp745.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp746.tag=0,((_tmp746.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp747[0]=& _tmp746,((_tmp747[1]=& _tmp745,Cyc_Tcutil_terr(loc,((_tmp748="comparison not allowed between %s and %s",_tag_dyneither(_tmp748,sizeof(char),41))),_tag_dyneither(_tmp747,sizeof(void*),2)))))))))))));}
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
{struct _tuple0 _tmp749;struct _tuple0 _stmttmp10=(_tmp749.f1=t1,((_tmp749.f2=t2,_tmp749)));struct _tuple0 _tmp149=_stmttmp10;void*_tmp14B;void*_tmp14D;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp149.f1;if(_tmp14A->tag != 15)goto _LLB7;else{_tmp14B=(void*)_tmp14A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp149.f2;if(_tmp14C->tag != 15)goto _LLB7;else{_tmp14D=(void*)_tmp14C->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp74E;void*_tmp74D[2];struct Cyc_String_pa_PrintArg_struct _tmp74C;struct Cyc_String_pa_PrintArg_struct _tmp74B;(_tmp74B.tag=0,((_tmp74B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp74C.tag=0,((_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp74D[0]=& _tmp74C,((_tmp74D[1]=& _tmp74B,Cyc_Tcutil_terr(loc,((_tmp74E="comparison not allowed between %s and %s",_tag_dyneither(_tmp74E,sizeof(char),41))),_tag_dyneither(_tmp74D,sizeof(void*),2)))))))))))));}
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
const char*_tmp751;void*_tmp750;(_tmp750=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp751="bad binary primop",_tag_dyneither(_tmp751,sizeof(char),18))),_tag_dyneither(_tmp750,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp155=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp156=Cyc_Tcexp_tcExp(te,topt,_tmp155);
if(!Cyc_Tcutil_is_numeric(_tmp155)){
const char*_tmp755;void*_tmp754[1];struct Cyc_String_pa_PrintArg_struct _tmp753;(_tmp753.tag=0,((_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp156)),((_tmp754[0]=& _tmp753,Cyc_Tcutil_terr(_tmp155->loc,((_tmp755="expecting numeric type but found %s",_tag_dyneither(_tmp755,sizeof(char),36))),_tag_dyneither(_tmp754,sizeof(void*),1)))))));}
return _tmp156;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _stmttmp11=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_stmttmp11){case 0: _LLCC: {
const char*_tmp758;void*_tmp757;return(_tmp757=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp758="primitive operator has 0 arguments",_tag_dyneither(_tmp758,sizeof(char),35))),_tag_dyneither(_tmp757,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp75B;void*_tmp75A;return(_tmp75A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp75B="primitive operator has > 2 arguments",_tag_dyneither(_tmp75B,sizeof(char),37))),_tag_dyneither(_tmp75A,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp15E=Cyc_Tcutil_compress(t);
void*_tmp15F=_tmp15E;struct Cyc_Absyn_Aggrdecl*_tmp161;struct Cyc_List_List*_tmp163;struct Cyc_Absyn_Datatypefield*_tmp165;void*_tmp167;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_List_List*_tmp16A;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15F;if(_tmp160->tag != 11)goto _LLD4;else{if((((_tmp160->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp161=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp160->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp161->impl == 0){
{const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_Tcutil_terr(loc,((_tmp75E="attempt to write an abstract aggregate",_tag_dyneither(_tmp75E,sizeof(char),39))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp163=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15F;if(_tmp162->tag != 12)goto _LLD6;else{_tmp163=_tmp162->f2;}}_LLD5:
# 736
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
struct Cyc_Absyn_Aggrfield*_tmp16D=(struct Cyc_Absyn_Aggrfield*)_tmp163->hd;
if((_tmp16D->tq).real_const){
{const char*_tmp762;void*_tmp761[1];struct Cyc_String_pa_PrintArg_struct _tmp760;(_tmp760.tag=0,((_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp16D->name),((_tmp761[0]=& _tmp760,Cyc_Tcutil_terr(loc,((_tmp762="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp762,sizeof(char),56))),_tag_dyneither(_tmp761,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp16D->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15F;if(_tmp164->tag != 4)goto _LLD8;else{if((((_tmp164->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp165=((struct _tuple2)(((_tmp164->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp171=_tmp165->typs;for(0;_tmp171 != 0;_tmp171=_tmp171->tl){
struct _tuple18*_stmttmp13=(struct _tuple18*)_tmp171->hd;struct Cyc_Absyn_Tqual _tmp173;void*_tmp174;struct _tuple18*_tmp172=_stmttmp13;_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;
if(_tmp173.real_const){
{const char*_tmp766;void*_tmp765[1];struct Cyc_String_pa_PrintArg_struct _tmp764;(_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp165->name)),((_tmp765[0]=& _tmp764,Cyc_Tcutil_terr(loc,((_tmp766="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp766,sizeof(char),64))),_tag_dyneither(_tmp765,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp174))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15F;if(_tmp166->tag != 8)goto _LLDA;else{_tmp167=(_tmp166->f1).elt_type;_tmp168=(_tmp166->f1).tq;}}_LLD9:
# 756
 if(_tmp168.real_const){
{const char*_tmp769;void*_tmp768;(_tmp768=0,Cyc_Tcutil_terr(loc,((_tmp769="attempt to over-write a const array",_tag_dyneither(_tmp769,sizeof(char),36))),_tag_dyneither(_tmp768,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp167);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15F;if(_tmp169->tag != 10)goto _LLDC;else{_tmp16A=_tmp169->f1;}}_LLDB:
# 762
 for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct _tuple18*_stmttmp12=(struct _tuple18*)_tmp16A->hd;struct Cyc_Absyn_Tqual _tmp17B;void*_tmp17C;struct _tuple18*_tmp17A=_stmttmp12;_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;
if(_tmp17B.real_const){
{const char*_tmp76C;void*_tmp76B;(_tmp76B=0,Cyc_Tcutil_terr(loc,((_tmp76C="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp76C,sizeof(char),56))),_tag_dyneither(_tmp76B,sizeof(void*),0)));}
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
{const char*_tmp76F;void*_tmp76E;(_tmp76E=0,Cyc_Tcutil_terr(e->loc,((_tmp76F="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp76F,sizeof(char),47))),_tag_dyneither(_tmp76E,sizeof(void*),0)));}
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
const char*_tmp773;void*_tmp772[1];struct Cyc_String_pa_PrintArg_struct _tmp771;(_tmp771.tag=0,((_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp772[0]=& _tmp771,Cyc_Tcutil_terr(e->loc,((_tmp773="attempt to write a const location: %s",_tag_dyneither(_tmp773,sizeof(char),38))),_tag_dyneither(_tmp772,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1C5=_new_region("temp");struct _RegionHandle*temp=& _tmp1C5;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp776;void*_tmp775;void*_tmp1C8=(_tmp775=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp776="increment/decrement of non-lvalue",_tag_dyneither(_tmp776,sizeof(char),34))),_tag_dyneither(_tmp775,sizeof(void*),0)));_npop_handler(0);return _tmp1C8;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp779;void*_tmp778;(_tmp778=0,Cyc_Tcutil_terr(e->loc,((_tmp779="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp779,sizeof(char),50))),_tag_dyneither(_tmp778,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(e->loc,((_tmp77C="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp77C,sizeof(char),54))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}}else{
# 872
const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_PrintArg_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp77F[0]=& _tmp77E,Cyc_Tcutil_terr(e->loc,((_tmp780="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp780,sizeof(char),44))),_tag_dyneither(_tmp77F,sizeof(void*),1)))))));}}{
# 874
void*_tmp1D0=t;_npop_handler(0);return _tmp1D0;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1D1=_new_region("r");struct _RegionHandle*r=& _tmp1D1;_push_region(r);{
const char*_tmp781;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp781="conditional expression",_tag_dyneither(_tmp781,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp782;struct Cyc_List_List _tmp1D3=(_tmp782.hd=e3,((_tmp782.tl=0,_tmp782)));
struct Cyc_List_List _tmp783;struct Cyc_List_List _tmp1D4=(_tmp783.hd=e2,((_tmp783.tl=& _tmp1D3,_tmp783)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1D4)){
{const char*_tmp788;void*_tmp787[2];struct Cyc_String_pa_PrintArg_struct _tmp786;struct Cyc_String_pa_PrintArg_struct _tmp785;(_tmp785.tag=0,((_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp786.tag=0,((_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp787[0]=& _tmp786,((_tmp787[1]=& _tmp785,Cyc_Tcutil_terr(loc,((_tmp788="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp788,sizeof(char),48))),_tag_dyneither(_tmp787,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1D9=t;_npop_handler(0);return _tmp1D9;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp789;Cyc_Tcexp_tcTest(te,e1,((_tmp789="logical-and expression",_tag_dyneither(_tmp789,sizeof(char),23))));}
{const char*_tmp78A;Cyc_Tcexp_tcTest(te,e2,((_tmp78A="logical-and expression",_tag_dyneither(_tmp78A,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp78B;Cyc_Tcexp_tcTest(te,e1,((_tmp78B="logical-or expression",_tag_dyneither(_tmp78B,sizeof(char),22))));}
{const char*_tmp78C;Cyc_Tcexp_tcTest(te,e2,((_tmp78C="logical-or expression",_tag_dyneither(_tmp78C,sizeof(char),22))));}
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
{const char*_tmp78F;void*_tmp78E;(_tmp78E=0,Cyc_Tcutil_terr(loc,((_tmp78F="cannot assign to an array",_tag_dyneither(_tmp78F,sizeof(char),26))),_tag_dyneither(_tmp78E,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp792;void*_tmp791;(_tmp791=0,Cyc_Tcutil_terr(loc,((_tmp792="type is abstract (can't determine size).",_tag_dyneither(_tmp792,sizeof(char),41))),_tag_dyneither(_tmp791,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp795;void*_tmp794;void*_tmp1E9=(_tmp794=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp795="assignment to non-lvalue",_tag_dyneither(_tmp795,sizeof(char),25))),_tag_dyneither(_tmp794,sizeof(void*),0)));_npop_handler(0);return _tmp1E9;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp1EA=_new_region("temp");struct _RegionHandle*temp=& _tmp1EA;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_terr(e2->loc,((_tmp798="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp798,sizeof(char),49))),_tag_dyneither(_tmp797,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp79D;void*_tmp79C[2];struct Cyc_String_pa_PrintArg_struct _tmp79B;struct Cyc_String_pa_PrintArg_struct _tmp79A;void*_tmp1ED=(_tmp79A.tag=0,((_tmp79A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp79B.tag=0,((_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp79C[0]=& _tmp79B,((_tmp79C[1]=& _tmp79A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp79D="type mismatch: %s != %s",_tag_dyneither(_tmp79D,sizeof(char),24))),_tag_dyneither(_tmp79C,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp1EE=_tmp1ED;_npop_handler(1);return _tmp1EE;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp1F3=(enum Cyc_Absyn_Primop)po->v;
void*_tmp1F4=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1F3,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp1F4,t1) || Cyc_Tcutil_coerceable(_tmp1F4) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp7A2;void*_tmp7A1[2];struct Cyc_String_pa_PrintArg_struct _tmp7A0;struct Cyc_String_pa_PrintArg_struct _tmp79F;void*_tmp1F5=
(_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp7A0.tag=0,((_tmp7A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp7A1[0]=& _tmp7A0,((_tmp7A1[1]=& _tmp79F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7A2="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp7A2,sizeof(char),82))),_tag_dyneither(_tmp7A1,sizeof(void*),2)))))))))))));
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
{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_PrintArg_struct _tmp7A5;struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(e->loc,((_tmp7A7="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp7A7,sizeof(char),54))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
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
const char*_tmp7AA;void*_tmp7A9;(_tmp7A9=0,Cyc_Tcutil_terr(e->loc,((_tmp7AA="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp7AA,sizeof(char),79))),_tag_dyneither(_tmp7A9,sizeof(void*),0)));}
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
const char*_tmp7AB;struct _dyneither_ptr s0=(_tmp7AB="actual argument has type ",_tag_dyneither(_tmp7AB,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp7AC;struct _dyneither_ptr s2=(_tmp7AC=" but formal has type ",_tag_dyneither(_tmp7AC,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp7BA;struct Cyc_String_pa_PrintArg_struct _tmp7B9;struct Cyc_String_pa_PrintArg_struct _tmp7B8;struct Cyc_String_pa_PrintArg_struct _tmp7B7;void*_tmp7B6[4];const char*_tmp7B5;void*_tmp7B4;(_tmp7B4=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp7BA.tag=0,((_tmp7BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp7B8.tag=0,((_tmp7B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp7B6[0]=& _tmp7B7,((_tmp7B6[1]=& _tmp7B8,((_tmp7B6[2]=& _tmp7B9,((_tmp7B6[3]=& _tmp7BA,Cyc_aprintf(((_tmp7B5="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp7B5,sizeof(char),15))),_tag_dyneither(_tmp7B6,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp7B4,sizeof(void*),0)));}else{
# 1072
struct Cyc_String_pa_PrintArg_struct _tmp7C8;struct Cyc_String_pa_PrintArg_struct _tmp7C7;struct Cyc_String_pa_PrintArg_struct _tmp7C6;struct Cyc_String_pa_PrintArg_struct _tmp7C5;void*_tmp7C4[4];const char*_tmp7C3;void*_tmp7C2;(_tmp7C2=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp7C8.tag=0,((_tmp7C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp7C7.tag=0,((_tmp7C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp7C6.tag=0,((_tmp7C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp7C5.tag=0,((_tmp7C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp7C4[0]=& _tmp7C5,((_tmp7C4[1]=& _tmp7C6,((_tmp7C4[2]=& _tmp7C7,((_tmp7C4[3]=& _tmp7C8,Cyc_aprintf(((_tmp7C3="%s%s%s%s.",_tag_dyneither(_tmp7C3,sizeof(char),10))),_tag_dyneither(_tmp7C4,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1077
if(alias_coercion){
struct Cyc_List_List*_tmp7C9;*alias_arg_exps=((_tmp7C9=_cycalloc(sizeof(*_tmp7C9)),((_tmp7C9->hd=(void*)_tmp212,((_tmp7C9->tl=*alias_arg_exps,_tmp7C9))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7CC;void*_tmp7CB;(_tmp7CB=0,Cyc_Tcutil_terr(e1->loc,((_tmp7CC="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7CC,sizeof(char),49))),_tag_dyneither(_tmp7CB,sizeof(void*),0)));}
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
struct Cyc_Core_Opt*_tmp7CD;fmt_args=((_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp242 - 1),_tmp7CD))));}
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
{const char*_tmp7D0;void*_tmp7CF;(_tmp7CF=0,Cyc_Tcutil_terr(loc,((_tmp7D0="bad format arguments",_tag_dyneither(_tmp7D0,sizeof(char),21))),_tag_dyneither(_tmp7CF,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp24C=_tmp24A;_LL141:(void)_throw(_tmp24C);_LL13D:;}};}
# 1119
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1123
if(_tmp224 != 0){const char*_tmp7D3;void*_tmp7D2;(_tmp7D2=0,Cyc_Tcutil_terr(loc,((_tmp7D3="too few arguments for function",_tag_dyneither(_tmp7D3,sizeof(char),31))),_tag_dyneither(_tmp7D2,sizeof(void*),0)));}else{
# 1125
if((_tmp211 != 0  || _tmp225) || _tmp226 != 0){
if(_tmp225)
for(0;_tmp211 != 0;_tmp211=_tmp211->tl){
Cyc_Tcexp_tcExp(_tmp215,0,(struct Cyc_Absyn_Exp*)_tmp211->hd);}else{
if(_tmp226 == 0){
const char*_tmp7D6;void*_tmp7D5;(_tmp7D5=0,Cyc_Tcutil_terr(loc,((_tmp7D6="too many arguments for function",_tag_dyneither(_tmp7D6,sizeof(char),32))),_tag_dyneither(_tmp7D5,sizeof(void*),0)));}else{
# 1132
struct Cyc_Absyn_VarargInfo _stmttmp22=*_tmp226;void*_tmp254;int _tmp255;struct Cyc_Absyn_VarargInfo _tmp253=_stmttmp22;_tmp254=_tmp253.type;_tmp255=_tmp253.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp7D7;struct Cyc_Absyn_VarargCallInfo*_tmp256=(_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7->num_varargs=0,((_tmp7D7->injectors=0,((_tmp7D7->vai=_tmp226,_tmp7D7)))))));
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
{const char*_tmp7DC;void*_tmp7DB[2];struct Cyc_String_pa_PrintArg_struct _tmp7DA;struct Cyc_String_pa_PrintArg_struct _tmp7D9;(_tmp7D9.tag=0,((_tmp7D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp254)),((_tmp7DB[0]=& _tmp7DA,((_tmp7DB[1]=& _tmp7D9,Cyc_Tcutil_terr(loc,((_tmp7DC="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp7DC,sizeof(char),49))),_tag_dyneither(_tmp7DB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1150
if(alias_coercion){
struct Cyc_List_List*_tmp7DD;*alias_arg_exps=((_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD->hd=(void*)_tmp212,((_tmp7DD->tl=*alias_arg_exps,_tmp7DD))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp254) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp7E0;void*_tmp7DF;(_tmp7DF=0,Cyc_Tcutil_terr(e1->loc,((_tmp7E0="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7E0,sizeof(char),49))),_tag_dyneither(_tmp7DF,sizeof(void*),0)));}}else{
# 1159
void*_stmttmp23=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp254));void*_tmp25E=_stmttmp23;struct Cyc_Absyn_Datatypedecl*_tmp260;struct Cyc_List_List*_tmp261;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp25E;if(_tmp25F->tag != 3)goto _LL145;else{if((((_tmp25F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp260=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp25F->f1).datatype_info).KnownDatatype).val);_tmp261=(_tmp25F->f1).targs;}}_LL144: {
# 1163
struct Cyc_Absyn_Datatypedecl*_tmp262=*Cyc_Tcenv_lookup_datatypedecl(_tmp215,loc,_tmp260->name);
struct Cyc_List_List*fields=0;
if(_tmp262->fields == 0){
const char*_tmp7E4;void*_tmp7E3[1];struct Cyc_String_pa_PrintArg_struct _tmp7E2;(_tmp7E2.tag=0,((_tmp7E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp254)),((_tmp7E3[0]=& _tmp7E2,Cyc_Tcutil_terr(loc,((_tmp7E4="can't inject into abstract datatype %s",_tag_dyneither(_tmp7E4,sizeof(char),39))),_tag_dyneither(_tmp7E3,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp262->fields))->v;}
# 1175
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp254),Cyc_Tcenv_curr_rgn(_tmp215))){
const char*_tmp7E9;void*_tmp7E8[2];struct Cyc_String_pa_PrintArg_struct _tmp7E7;struct Cyc_String_pa_PrintArg_struct _tmp7E6;(_tmp7E6.tag=0,((_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp215))),((_tmp7E7.tag=0,((_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp254)),((_tmp7E8[0]=& _tmp7E7,((_tmp7E8[1]=& _tmp7E6,Cyc_Tcutil_terr(loc,((_tmp7E9="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp7E9,sizeof(char),49))),_tag_dyneither(_tmp7E8,sizeof(void*),2)))))))))))));}{
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
const char*_tmp7EC;void*_tmp7EB;(_tmp7EB=0,Cyc_Tcutil_terr(e1->loc,((_tmp7EC="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7EC,sizeof(char),49))),_tag_dyneither(_tmp7EB,sizeof(void*),0)));}}{
# 1191
struct Cyc_Absyn_Datatypefield*_tmp26E=Cyc_Tcexp_tcInjection(_tmp215,e1,Cyc_Tcutil_pointer_elt_type(_tmp254),rgn,_tmp26B,fields);
# 1193
if(_tmp26E != 0){
struct Cyc_List_List*_tmp7ED;_tmp256->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp256->injectors,(
(_tmp7ED=_cycalloc(sizeof(*_tmp7ED)),((_tmp7ED->hd=_tmp26E,((_tmp7ED->tl=0,_tmp7ED)))))));}};};}}
# 1199
_npop_handler(0);goto _LL142;
# 1179
;_pop_region(rgn);};}_LL145:;_LL146:
# 1200
{const char*_tmp7F0;void*_tmp7EF;(_tmp7EF=0,Cyc_Tcutil_terr(loc,((_tmp7F0="bad inject vararg type",_tag_dyneither(_tmp7F0,sizeof(char),23))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1205
if(*alias_arg_exps == 0)
# 1214 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp215,loc,(void*)_check_null(_tmp221));{
# 1216
void*_tmp273=_tmp223;_npop_handler(0);return _tmp273;};};_LL133:;_LL134: {
const char*_tmp7F4;void*_tmp7F3[1];struct Cyc_String_pa_PrintArg_struct _tmp7F2;void*_tmp277=(_tmp7F2.tag=0,((_tmp7F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7F3[0]=& _tmp7F2,Cyc_Tcexp_expr_err(_tmp215,loc,topt,((_tmp7F4="expected pointer to function but found %s",_tag_dyneither(_tmp7F4,sizeof(char),42))),_tag_dyneither(_tmp7F3,sizeof(void*),1)))))));_npop_handler(0);return _tmp277;}_LL130:;};_LL12E:;_LL12F: {
# 1219
const char*_tmp7F8;void*_tmp7F7[1];struct Cyc_String_pa_PrintArg_struct _tmp7F6;void*_tmp27B=(_tmp7F6.tag=0,((_tmp7F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7F7[0]=& _tmp7F6,Cyc_Tcexp_expr_err(_tmp215,loc,topt,((_tmp7F8="expected pointer to function but found %s",_tag_dyneither(_tmp7F8,sizeof(char),42))),_tag_dyneither(_tmp7F7,sizeof(void*),1)))))));_npop_handler(0);return _tmp27B;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1225 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp27C=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp27C,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp27C,& bogus)){
const char*_tmp7FD;void*_tmp7FC[2];struct Cyc_String_pa_PrintArg_struct _tmp7FB;struct Cyc_String_pa_PrintArg_struct _tmp7FA;(_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp7FB.tag=0,((_tmp7FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1230
Cyc_Absynpp_typ2string(_tmp27C)),((_tmp7FC[0]=& _tmp7FB,((_tmp7FC[1]=& _tmp7FA,Cyc_Tcutil_terr(loc,((_tmp7FD="expected %s but found %s",_tag_dyneither(_tmp7FD,sizeof(char),25))),_tag_dyneither(_tmp7FC,sizeof(void*),2)))))))))))));}
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
struct _tuple13*_tmp800;struct Cyc_List_List*_tmp7FF;instantiation=(
(_tmp7FF=_region_malloc(temp,sizeof(*_tmp7FF)),((_tmp7FF->hd=((_tmp800=_region_malloc(temp,sizeof(*_tmp800)),((_tmp800->f1=(struct Cyc_Absyn_Tvar*)_tmp28B->hd,((_tmp800->f2=(void*)ts->hd,_tmp800)))))),((_tmp7FF->tl=instantiation,_tmp7FF))))));};}
# 1257
if(ts != 0){
const char*_tmp803;void*_tmp802;void*_tmp299=
(_tmp802=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp803="too many type variables in instantiation",_tag_dyneither(_tmp803,sizeof(char),41))),_tag_dyneither(_tmp802,sizeof(void*),0)));_npop_handler(0);return _tmp299;}
# 1263
if(_tmp28B == 0){
_tmp292=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp292);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp292);
_tmp292=0;}{
# 1268
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp809;struct Cyc_Absyn_FnInfo _tmp808;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp807;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp807=_cycalloc(sizeof(*_tmp807)),((_tmp807[0]=((_tmp809.tag=9,((_tmp809.f1=((_tmp808.tvars=_tmp28B,((_tmp808.effect=_tmp28C,((_tmp808.ret_tqual=_tmp28D,((_tmp808.ret_typ=_tmp28E,((_tmp808.args=_tmp28F,((_tmp808.c_varargs=_tmp290,((_tmp808.cyc_varargs=_tmp291,((_tmp808.rgn_po=_tmp292,((_tmp808.attributes=_tmp293,_tmp808)))))))))))))))))),_tmp809)))),_tmp807)))));
# 1272
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp80F;struct Cyc_Absyn_PtrInfo _tmp80E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80D;void*_tmp29D=(void*)((_tmp80D=_cycalloc(sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp80F.tag=5,((_tmp80F.f1=((_tmp80E.elt_typ=new_fn_typ,((_tmp80E.elt_tq=_tmp284,((_tmp80E.ptr_atts=(((_tmp80E.ptr_atts).rgn=_tmp285,(((_tmp80E.ptr_atts).nullable=_tmp286,(((_tmp80E.ptr_atts).bounds=_tmp287,(((_tmp80E.ptr_atts).zero_term=_tmp288,(((_tmp80E.ptr_atts).ptrloc=0,_tmp80E.ptr_atts)))))))))),_tmp80E)))))),_tmp80F)))),_tmp80D))));_npop_handler(0);return _tmp29D;};}
# 1248
;_pop_region(temp);}_LL14F:;_LL150:
# 1274
 goto _LL14C;_LL14C:;}
# 1276
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1279
const char*_tmp813;void*_tmp812[1];struct Cyc_String_pa_PrintArg_struct _tmp811;return(_tmp811.tag=0,((_tmp811.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp812[0]=& _tmp811,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp813="expecting polymorphic type but found %s",_tag_dyneither(_tmp813,sizeof(char),40))),_tag_dyneither(_tmp812,sizeof(void*),1)))))));};};}
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
const char*_tmp818;void*_tmp817[2];struct Cyc_String_pa_PrintArg_struct _tmp816;struct Cyc_String_pa_PrintArg_struct _tmp815;void*_tmp2A4=(_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp817[0]=& _tmp816,((_tmp817[1]=& _tmp815,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp818="cannot cast %s to %s",_tag_dyneither(_tmp818,sizeof(char),21))),_tag_dyneither(_tmp817,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2A4;};}}}
# 1312
{struct _tuple0 _tmp819;struct _tuple0 _stmttmp25=(_tmp819.f1=e->r,((_tmp819.f2=Cyc_Tcutil_compress(t),_tmp819)));struct _tuple0 _tmp2A9=_stmttmp25;int _tmp2AB;union Cyc_Absyn_Constraint*_tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2A9.f1;if(_tmp2AA->tag != 33)goto _LL154;else{_tmp2AB=(_tmp2AA->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A9.f2;if(_tmp2AC->tag != 5)goto _LL154;else{_tmp2AD=((_tmp2AC->f1).ptr_atts).nullable;_tmp2AE=((_tmp2AC->f1).ptr_atts).bounds;_tmp2AF=((_tmp2AC->f1).ptr_atts).zero_term;}};_LL153:
# 1316
 if((_tmp2AB  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AF)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2AD)){
void*_stmttmp26=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2AE);void*_tmp2B0=_stmttmp26;struct Cyc_Absyn_Exp*_tmp2B2;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2B1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2B0;if(_tmp2B1->tag != 1)goto _LL159;else{_tmp2B2=_tmp2B1->f1;}}_LL158:
# 1319
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2B2)).f1 == 1){
const char*_tmp81C;void*_tmp81B;(_tmp81B=0,Cyc_Tcutil_warn(loc,((_tmp81C="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp81C,sizeof(char),60))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}
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
const char*_tmp81F;void*_tmp81E;(_tmp81E=0,Cyc_Tcutil_terr(e->loc,((_tmp81F="Cannot take the address of an alias-free path",_tag_dyneither(_tmp81F,sizeof(char),46))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}
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
{const char*_tmp822;void*_tmp821;(_tmp821=0,Cyc_Tcutil_terr(e->loc,((_tmp822="cannot take the address of a @tagged union member",_tag_dyneither(_tmp822,sizeof(char),50))),_tag_dyneither(_tmp821,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1389
struct _tuple14 _stmttmp2B=Cyc_Tcutil_addressof_props(te,e);int _tmp2CE;void*_tmp2CF;struct _tuple14 _tmp2CD=_stmttmp2B;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;
# 1391
if(Cyc_Tcutil_is_noalias_region(_tmp2CF,0)){
const char*_tmp825;void*_tmp824;(_tmp824=0,Cyc_Tcutil_terr(e->loc,((_tmp825="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp825,sizeof(char),60))),_tag_dyneither(_tmp824,sizeof(void*),0)));}{
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
const char*_tmp829;void*_tmp828[1];struct Cyc_String_pa_PrintArg_struct _tmp827;(_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp828[0]=& _tmp827,Cyc_Tcutil_terr(loc,((_tmp829="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp829,sizeof(char),55))),_tag_dyneither(_tmp828,sizeof(void*),1)))))));}
if(topt != 0){
void*_stmttmp2C=Cyc_Tcutil_compress(*topt);void*_tmp2D6=_stmttmp2C;void*_tmp2D7;void*_tmp2D9;_LL172: _tmp2D7=_tmp2D6;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D7;if(_tmp2D8->tag != 19)goto _LL174;else{_tmp2D9=(void*)_tmp2D8->f1;}};_LL173: {
# 1422
struct Cyc_Absyn_Exp*_tmp2DA=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp82C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp82B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2DB=(_tmp82B=_cycalloc(sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp82C.tag=18,((_tmp82C.f1=_tmp2DA,_tmp82C)))),_tmp82B)));
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
const char*_tmp830;void*_tmp82F[1];struct Cyc_String_pa_PrintArg_struct _tmp82E;(_tmp82E.tag=0,((_tmp82E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E3),((_tmp82F[0]=& _tmp82E,Cyc_Tcutil_terr(loc,((_tmp830="no field of struct/union has name %s",_tag_dyneither(_tmp830,sizeof(char),37))),_tag_dyneither(_tmp82F,sizeof(void*),1)))))));}else{
# 1456
_tmp2DF=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2E9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E6;if(_tmp2E9->tag != 12)goto _LL180;else{_tmp2EA=_tmp2E9->f2;}}_LL17F: {
# 1460
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp2E3,_tmp2EA);
if(!((unsigned int)t2)){
const char*_tmp834;void*_tmp833[1];struct Cyc_String_pa_PrintArg_struct _tmp832;(_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2E3),((_tmp833[0]=& _tmp832,Cyc_Tcutil_terr(loc,((_tmp834="no field of struct/union has name %s",_tag_dyneither(_tmp834,sizeof(char),37))),_tag_dyneither(_tmp833,sizeof(void*),1)))))));}else{
# 1464
_tmp2DF=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1469
if(bad_type){
if(_tmp2DE == fs){
const char*_tmp838;void*_tmp837[1];struct Cyc_String_pa_PrintArg_struct _tmp836;(_tmp836.tag=0,((_tmp836.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp837[0]=& _tmp836,Cyc_Tcutil_terr(loc,((_tmp838="%s is not a known struct/union type",_tag_dyneither(_tmp838,sizeof(char),36))),_tag_dyneither(_tmp837,sizeof(void*),1)))))));}else{
# 1473
const char*_tmp83C;void*_tmp83B[1];struct Cyc_String_pa_PrintArg_struct _tmp83A;struct _dyneither_ptr _tmp2F4=(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp83B[0]=& _tmp83A,Cyc_aprintf(((_tmp83C="(%s)",_tag_dyneither(_tmp83C,sizeof(char),5))),_tag_dyneither(_tmp83B,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp2DE;x=x->tl){
void*_stmttmp2F=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp2F5=_stmttmp2F;struct _dyneither_ptr*_tmp2F7;unsigned int _tmp2F9;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2F6=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto _LL185;else{_tmp2F7=_tmp2F6->f1;}}_LL184:
# 1478
{const char*_tmp841;void*_tmp840[2];struct Cyc_String_pa_PrintArg_struct _tmp83F;struct Cyc_String_pa_PrintArg_struct _tmp83E;_tmp2F4=((_tmp83E.tag=0,((_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F7),((_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp840[0]=& _tmp83F,((_tmp840[1]=& _tmp83E,Cyc_aprintf(((_tmp841="%s.%s",_tag_dyneither(_tmp841,sizeof(char),6))),_tag_dyneither(_tmp840,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2F8=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F8->tag != 1)goto _LL182;else{_tmp2F9=_tmp2F8->f1;}}_LL186:
# 1480
{const char*_tmp846;void*_tmp845[2];struct Cyc_String_pa_PrintArg_struct _tmp844;struct Cyc_Int_pa_PrintArg_struct _tmp843;_tmp2F4=((_tmp843.tag=1,((_tmp843.f1=(unsigned long)((int)_tmp2F9),((_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp845[0]=& _tmp844,((_tmp845[1]=& _tmp843,Cyc_aprintf(((_tmp846="%s.%d",_tag_dyneither(_tmp846,sizeof(char),6))),_tag_dyneither(_tmp845,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1482
const char*_tmp84B;void*_tmp84A[2];struct Cyc_String_pa_PrintArg_struct _tmp849;struct Cyc_String_pa_PrintArg_struct _tmp848;(_tmp848.tag=0,((_tmp848.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2F4),((_tmp84A[0]=& _tmp849,((_tmp84A[1]=& _tmp848,Cyc_Tcutil_terr(loc,((_tmp84B="%s == %s is not a struct/union type",_tag_dyneither(_tmp84B,sizeof(char),36))),_tag_dyneither(_tmp84A,sizeof(void*),2)))))))))))));};}}
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
const char*_tmp850;void*_tmp84F[2];struct Cyc_Int_pa_PrintArg_struct _tmp84E;struct Cyc_Int_pa_PrintArg_struct _tmp84D;(_tmp84D.tag=1,((_tmp84D.f1=(unsigned long)((int)_tmp2E5),((_tmp84E.tag=1,((_tmp84E.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30D),((_tmp84F[0]=& _tmp84E,((_tmp84F[1]=& _tmp84D,Cyc_Tcutil_terr(loc,((_tmp850="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp850,sizeof(char),46))),_tag_dyneither(_tmp84F,sizeof(void*),2)))))))))))));}else{
# 1498
_tmp2DF=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp30D,(int)_tmp2E5))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp30E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp309;if(_tmp30E->tag != 10)goto _LL18E;else{_tmp30F=_tmp30E->f1;}}_LL18D:
# 1502
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F)<= _tmp2E5){
const char*_tmp855;void*_tmp854[2];struct Cyc_Int_pa_PrintArg_struct _tmp853;struct Cyc_Int_pa_PrintArg_struct _tmp852;(_tmp852.tag=1,((_tmp852.f1=(unsigned long)((int)_tmp2E5),((_tmp853.tag=1,((_tmp853.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F),((_tmp854[0]=& _tmp853,((_tmp854[1]=& _tmp852,Cyc_Tcutil_terr(loc,((_tmp855="tuple has too few components: %d <= %d",_tag_dyneither(_tmp855,sizeof(char),39))),_tag_dyneither(_tmp854,sizeof(void*),2)))))))))))));}else{
# 1506
_tmp2DF=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp30F,(int)_tmp2E5)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp309;if(_tmp310->tag != 4)goto _LL190;else{if((((_tmp310->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp311=((struct _tuple2)(((_tmp310->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp311->typs)< _tmp2E5){
const char*_tmp85A;void*_tmp859[2];struct Cyc_Int_pa_PrintArg_struct _tmp858;struct Cyc_Int_pa_PrintArg_struct _tmp857;(_tmp857.tag=1,((_tmp857.f1=(unsigned long)((int)_tmp2E5),((_tmp858.tag=1,((_tmp858.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp311->typs),((_tmp859[0]=& _tmp858,((_tmp859[1]=& _tmp857,Cyc_Tcutil_terr(loc,((_tmp85A="datatype field has too few components: %d < %d",_tag_dyneither(_tmp85A,sizeof(char),47))),_tag_dyneither(_tmp859,sizeof(void*),2)))))))))))));}else{
# 1514
if(_tmp2E5 != 0)
_tmp2DF=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp311->typs,(int)(_tmp2E5 - 1))).f2;else{
if(_tmp2DE->tl != 0){
const char*_tmp85D;void*_tmp85C;(_tmp85C=0,Cyc_Tcutil_terr(loc,((_tmp85D="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp85D,sizeof(char),68))),_tag_dyneither(_tmp85C,sizeof(void*),0)));}}}
# 1519
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1523
if(bad_type){
const char*_tmp861;void*_tmp860[1];struct Cyc_String_pa_PrintArg_struct _tmp85F;(_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2DF)),((_tmp860[0]=& _tmp85F,Cyc_Tcutil_terr(loc,((_tmp861="%s is not a known type",_tag_dyneither(_tmp861,sizeof(char),23))),_tag_dyneither(_tmp860,sizeof(void*),1)))))));}
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
struct Cyc_Core_Opt*_tmp862;void*_tmp32D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862->v=_tmp32C,_tmp862)))));
struct Cyc_Core_Opt*_tmp863;void*_tmp32E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863->v=_tmp32C,_tmp863)))));
union Cyc_Absyn_Constraint*_tmp32F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp330=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp864;struct Cyc_Absyn_PtrAtts _tmp331=(_tmp864.rgn=_tmp32E,((_tmp864.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp864.bounds=_tmp32F,((_tmp864.zero_term=_tmp330,((_tmp864.ptrloc=0,_tmp864)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp86A;struct Cyc_Absyn_PtrInfo _tmp869;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp868;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp332=(_tmp868=_cycalloc(sizeof(*_tmp868)),((_tmp868[0]=((_tmp86A.tag=5,((_tmp86A.f1=((_tmp869.elt_typ=_tmp32D,((_tmp869.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp869.ptr_atts=_tmp331,_tmp869)))))),_tmp86A)))),_tmp868)));
Cyc_Tcutil_unify(t,(void*)_tmp332);
_tmp328=_tmp32D;_tmp329=_tmp32E;_tmp32A=_tmp32F;_tmp32B=_tmp330;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325;if(_tmp327->tag != 5)goto _LL197;else{_tmp328=(_tmp327->f1).elt_typ;_tmp329=((_tmp327->f1).ptr_atts).rgn;_tmp32A=((_tmp327->f1).ptr_atts).bounds;_tmp32B=((_tmp327->f1).ptr_atts).zero_term;}}_LL196:
# 1549
 Cyc_Tcenv_check_rgn_accessible(_tmp324,loc,_tmp329);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp32A);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp328),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp324)){
const char*_tmp86D;void*_tmp86C;(_tmp86C=0,Cyc_Tcutil_terr(loc,((_tmp86D="can't dereference abstract pointer type",_tag_dyneither(_tmp86D,sizeof(char),40))),_tag_dyneither(_tmp86C,sizeof(void*),0)));}{
void*_tmp33B=_tmp328;_npop_handler(0);return _tmp33B;};_LL197:;_LL198: {
# 1555
const char*_tmp871;void*_tmp870[1];struct Cyc_String_pa_PrintArg_struct _tmp86F;void*_tmp33F=(_tmp86F.tag=0,((_tmp86F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp870[0]=& _tmp86F,Cyc_Tcexp_expr_err(_tmp324,loc,topt,((_tmp871="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp871,sizeof(char),39))),_tag_dyneither(_tmp870,sizeof(void*),1)))))));_npop_handler(0);return _tmp33F;}_LL192:;};}
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
const char*_tmp876;void*_tmp875[2];struct Cyc_String_pa_PrintArg_struct _tmp874;struct Cyc_String_pa_PrintArg_struct _tmp873;void*_tmp34D=(_tmp873.tag=0,((_tmp873.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp343->name)),((_tmp875[0]=& _tmp874,((_tmp875[1]=& _tmp873,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp876="%s has no %s member",_tag_dyneither(_tmp876,sizeof(char),20))),_tag_dyneither(_tmp875,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp34D;}
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
const char*_tmp87A;void*_tmp879[1];struct Cyc_String_pa_PrintArg_struct _tmp878;void*_tmp353=(_tmp878.tag=0,((_tmp878.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp879[0]=& _tmp878,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87A="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp87A,sizeof(char),56))),_tag_dyneither(_tmp879,sizeof(void*),1)))))));_npop_handler(0);return _tmp353;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp343->impl))->exist_vars != 0){
# 1592
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp87E;void*_tmp87D[1];struct Cyc_String_pa_PrintArg_struct _tmp87C;void*_tmp357=(_tmp87C.tag=0,((_tmp87C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp87D[0]=& _tmp87C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87E="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp87E,sizeof(char),81))),_tag_dyneither(_tmp87D,sizeof(void*),1)))))));_npop_handler(0);return _tmp357;}}{
# 1595
void*_tmp358=t2;_npop_handler(0);return _tmp358;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341;if(_tmp345->tag != 12)goto _LL19E;else{_tmp346=_tmp345->f1;_tmp347=_tmp345->f2;}}_LL19D: {
# 1597
struct Cyc_Absyn_Aggrfield*_tmp359=Cyc_Absyn_lookup_field(_tmp347,f);
if(_tmp359 == 0){
const char*_tmp882;void*_tmp881[1];struct Cyc_String_pa_PrintArg_struct _tmp880;void*_tmp35D=(_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp881[0]=& _tmp880,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp882="type has no %s member",_tag_dyneither(_tmp882,sizeof(char),22))),_tag_dyneither(_tmp881,sizeof(void*),1)))))));_npop_handler(0);return _tmp35D;}
# 1602
if(((_tmp346 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp359->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp359->requires_clause == 0){
# 1604
const char*_tmp886;void*_tmp885[1];struct Cyc_String_pa_PrintArg_struct _tmp884;void*_tmp361=(_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp885[0]=& _tmp884,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp886="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp886,sizeof(char),56))),_tag_dyneither(_tmp885,sizeof(void*),1)))))));_npop_handler(0);return _tmp361;}{
void*_tmp362=_tmp359->type;_npop_handler(0);return _tmp362;};}_LL19E:;_LL19F: {
# 1607
const char*_tmp88A;void*_tmp889[1];struct Cyc_String_pa_PrintArg_struct _tmp888;void*_tmp366=(_tmp888.tag=0,((_tmp888.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp889[0]=& _tmp888,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88A="expecting struct or union, found %s",_tag_dyneither(_tmp88A,sizeof(char),36))),_tag_dyneither(_tmp889,sizeof(void*),1)))))));_npop_handler(0);return _tmp366;}_LL199:;};
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
const char*_tmp88F;void*_tmp88E[2];struct Cyc_String_pa_PrintArg_struct _tmp88D;struct Cyc_String_pa_PrintArg_struct _tmp88C;void*_tmp37A=(_tmp88C.tag=0,((_tmp88C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp88D.tag=0,((_tmp88D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp370->name)),((_tmp88E[0]=& _tmp88D,((_tmp88E[1]=& _tmp88C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp88F="type %s has no %s member",_tag_dyneither(_tmp88F,sizeof(char),25))),_tag_dyneither(_tmp88E,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp37A;}
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
const char*_tmp893;void*_tmp892[1];struct Cyc_String_pa_PrintArg_struct _tmp891;void*_tmp383=(_tmp891.tag=0,((_tmp891.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp892[0]=& _tmp891,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp893="cannot get member %s since its type is abstract",_tag_dyneither(_tmp893,sizeof(char),48))),_tag_dyneither(_tmp892,sizeof(void*),1)))))));_npop_handler(0);return _tmp383;}_LL1AC:;}
# 1652
if((((_tmp370->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp375->requires_clause == 0){
# 1655
const char*_tmp897;void*_tmp896[1];struct Cyc_String_pa_PrintArg_struct _tmp895;void*_tmp387=(_tmp895.tag=0,((_tmp895.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp896[0]=& _tmp895,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp897="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp897,sizeof(char),56))),_tag_dyneither(_tmp896,sizeof(void*),1)))))));_npop_handler(0);return _tmp387;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp370->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp89B;void*_tmp89A[1];struct Cyc_String_pa_PrintArg_struct _tmp899;void*_tmp38B=(_tmp899.tag=0,((_tmp899.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp89A[0]=& _tmp899,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89B="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp89B,sizeof(char),72))),_tag_dyneither(_tmp89A,sizeof(void*),1)))))));_npop_handler(0);return _tmp38B;}}{
# 1662
void*_tmp38C=t3;_npop_handler(0);return _tmp38C;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36E;if(_tmp372->tag != 12)goto _LL1AA;else{_tmp373=_tmp372->f1;_tmp374=_tmp372->f2;}}_LL1A9: {
# 1664
struct Cyc_Absyn_Aggrfield*_tmp38D=Cyc_Absyn_lookup_field(_tmp374,f);
if(_tmp38D == 0){
const char*_tmp89F;void*_tmp89E[1];struct Cyc_String_pa_PrintArg_struct _tmp89D;void*_tmp391=(_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp89E[0]=& _tmp89D,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp89F="type has no %s field",_tag_dyneither(_tmp89F,sizeof(char),21))),_tag_dyneither(_tmp89E,sizeof(void*),1)))))));_npop_handler(0);return _tmp391;}
# 1669
if((_tmp373 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp38D->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp8A3;void*_tmp8A2[1];struct Cyc_String_pa_PrintArg_struct _tmp8A1;void*_tmp395=(_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8A2[0]=& _tmp8A1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A3="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8A3,sizeof(char),56))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))));_npop_handler(0);return _tmp395;}{
void*_tmp396=_tmp38D->type;_npop_handler(0);return _tmp396;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1674
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1677
const char*_tmp8A7;void*_tmp8A6[1];struct Cyc_String_pa_PrintArg_struct _tmp8A5;void*_tmp39A=(_tmp8A5.tag=0,((_tmp8A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8A6[0]=& _tmp8A5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8A7="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8A7,sizeof(char),44))),_tag_dyneither(_tmp8A6,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;};
# 1616
;_pop_region(r);}
# 1682
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1684
struct _tuple15 _stmttmp34=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp39C;int _tmp39D;struct _tuple15 _tmp39B=_stmttmp34;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;
if(!_tmp39D){
const char*_tmp8AA;void*_tmp8A9;return(_tmp8A9=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8AA="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8AA,sizeof(char),47))),_tag_dyneither(_tmp8A9,sizeof(void*),0)));}{
# 1688
struct _handler_cons _tmp3A0;_push_handler(& _tmp3A0);{int _tmp3A2=0;if(setjmp(_tmp3A0.handler))_tmp3A2=1;if(!_tmp3A2){
{void*_tmp3A3=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp39C)).f2;_npop_handler(0);return _tmp3A3;};_pop_handler();}else{void*_tmp3A1=(void*)_exn_thrown;void*_tmp3A5=_tmp3A1;void*_tmp3A7;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3A6=(struct Cyc_List_Nth_exn_struct*)_tmp3A5;if(_tmp3A6->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1691
const char*_tmp8AF;void*_tmp8AE[2];struct Cyc_Int_pa_PrintArg_struct _tmp8AD;struct Cyc_Int_pa_PrintArg_struct _tmp8AC;return(_tmp8AC.tag=1,((_tmp8AC.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8AD.tag=1,((_tmp8AD.f1=(unsigned long)((int)_tmp39C),((_tmp8AE[0]=& _tmp8AD,((_tmp8AE[1]=& _tmp8AC,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8AF="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8AF,sizeof(char),41))),_tag_dyneither(_tmp8AE,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3A7=_tmp3A5;_LL1B5:(void)_throw(_tmp3A7);_LL1B1:;}};};}
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
const char*_tmp8B3;void*_tmp8B2[1];struct Cyc_String_pa_PrintArg_struct _tmp8B1;void*_tmp3B1=(_tmp8B1.tag=0,((_tmp8B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp8B2[0]=& _tmp8B1,Cyc_Tcexp_expr_err(_tmp3AD,e2->loc,topt,((_tmp8B3="expecting int subscript, found %s",_tag_dyneither(_tmp8B3,sizeof(char),34))),_tag_dyneither(_tmp8B2,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B1;}{
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
const char*_tmp8B6;void*_tmp8B5;(_tmp8B5=0,Cyc_Tcutil_warn(e2->loc,((_tmp8B6="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp8B6,sizeof(char),63))),_tag_dyneither(_tmp8B5,sizeof(void*),0)));}}else{
# 1731
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _stmttmp38=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3C8;int _tmp3C9;struct _tuple15 _tmp3C7=_stmttmp38;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;
if(_tmp3C9)
Cyc_Tcutil_check_bound(loc,_tmp3C8,_tmp3B7);}else{
# 1737
if(Cyc_Tcutil_is_bound_one(_tmp3B7) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3B8)){
const char*_tmp8B9;void*_tmp8B8;(_tmp8B8=0,Cyc_Tcutil_warn(e1->loc,((_tmp8B9="subscript applied to pointer to one object",_tag_dyneither(_tmp8B9,sizeof(char),43))),_tag_dyneither(_tmp8B8,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3B7);}}
# 1742
Cyc_Tcenv_check_rgn_accessible(_tmp3AD,loc,_tmp3B6);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3B4),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3AD)){
const char*_tmp8BC;void*_tmp8BB;(_tmp8BB=0,Cyc_Tcutil_terr(e1->loc,((_tmp8BC="can't subscript an abstract pointer",_tag_dyneither(_tmp8BC,sizeof(char),36))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}{
void*_tmp3CE=_tmp3B4;_npop_handler(0);return _tmp3CE;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B2;if(_tmp3B9->tag != 10)goto _LL1BB;else{_tmp3BA=_tmp3B9->f1;}}_LL1BA: {
void*_tmp3CF=Cyc_Tcexp_ithTupleType(_tmp3AD,loc,_tmp3BA,e2);_npop_handler(0);return _tmp3CF;}_LL1BB:;_LL1BC: {
const char*_tmp8C0;void*_tmp8BF[1];struct Cyc_String_pa_PrintArg_struct _tmp8BE;void*_tmp3D3=(_tmp8BE.tag=0,((_tmp8BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp8BF[0]=& _tmp8BE,Cyc_Tcexp_expr_err(_tmp3AD,loc,topt,((_tmp8C0="subscript applied to %s",_tag_dyneither(_tmp8C0,sizeof(char),24))),_tag_dyneither(_tmp8BF,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D3;}_LL1B6:;};};}
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
struct _tuple18*_tmp8C3;struct Cyc_List_List*_tmp8C2;fields=((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2->hd=((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3->f1=(*((struct _tuple18*)_tmp3D6->hd)).f1,((_tmp8C3->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8C3)))))),((_tmp8C2->tl=fields,_tmp8C2))))));};}
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
struct _tuple18*_tmp8C6;struct Cyc_List_List*_tmp8C5;fields=((_tmp8C5=_cycalloc(sizeof(*_tmp8C5)),((_tmp8C5->hd=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6->f1=Cyc_Absyn_empty_tqual(0),((_tmp8C6->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp8C6)))))),((_tmp8C5->tl=fields,_tmp8C5))))));};
# 1781
;_pop_region(r);}{
# 1785
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp8C9;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8C8;return(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=10,((_tmp8C9.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp8C9)))),_tmp8C8))));};}
# 1789
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1792
const char*_tmp8CC;void*_tmp8CB;return(_tmp8CB=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8CC="tcCompoundLit",_tag_dyneither(_tmp8CC,sizeof(char),14))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}
# 1802 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1805
void*res_t2;
struct _RegionHandle _tmp3E2=_new_region("r");struct _RegionHandle*r=& _tmp3E2;_push_region(r);{
int _tmp3E3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp8CF;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8CE;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp3E4=(_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CF.tag=0,((_tmp8CF.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp3E3),_tmp8CF)))),_tmp8CE)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp3E4,loc);
# 1814
if(zero_term){
struct Cyc_Absyn_Exp*_tmp3E5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp3E3 - 1);
if(!Cyc_Tcutil_is_zero(_tmp3E5)){
const char*_tmp8D2;void*_tmp8D1;(_tmp8D1=0,Cyc_Tcutil_terr(_tmp3E5->loc,((_tmp8D2="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp8D2,sizeof(char),45))),_tag_dyneither(_tmp8D1,sizeof(void*),0)));}}
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
const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_PrintArg_struct _tmp8D4;(_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp8D5[0]=& _tmp8D4,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp8D6="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp8D6,sizeof(char),53))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))));}}
# 1833
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1838
void*_stmttmp3A=(void*)ds->hd;void*_tmp3ED=_stmttmp3A;struct Cyc_Absyn_Exp*_tmp3F0;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3EE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3ED;if(_tmp3EE->tag != 1)goto _LL1CA;}_LL1C9:
# 1840
{const char*_tmp8D9;void*_tmp8D8;(_tmp8D8=0,Cyc_Tcutil_terr(loc,((_tmp8D9="only array index designators are supported",_tag_dyneither(_tmp8D9,sizeof(char),43))),_tag_dyneither(_tmp8D8,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3EF=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3ED;if(_tmp3EF->tag != 0)goto _LL1C7;else{_tmp3F0=_tmp3EF->f1;}}_LL1CB:
# 1843
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp3F0);{
struct _tuple15 _stmttmp3B=Cyc_Evexp_eval_const_uint_exp(_tmp3F0);unsigned int _tmp3F4;int _tmp3F5;struct _tuple15 _tmp3F3=_stmttmp3B;_tmp3F4=_tmp3F3.f1;_tmp3F5=_tmp3F3.f2;
if(!_tmp3F5){
const char*_tmp8DC;void*_tmp8DB;(_tmp8DB=0,Cyc_Tcutil_terr(_tmp3F0->loc,((_tmp8DC="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp8DC,sizeof(char),47))),_tag_dyneither(_tmp8DB,sizeof(void*),0)));}else{
if(_tmp3F4 != offset){
const char*_tmp8E1;void*_tmp8E0[2];struct Cyc_Int_pa_PrintArg_struct _tmp8DF;struct Cyc_Int_pa_PrintArg_struct _tmp8DE;(_tmp8DE.tag=1,((_tmp8DE.f1=(unsigned long)((int)_tmp3F4),((_tmp8DF.tag=1,((_tmp8DF.f1=(unsigned long)offset,((_tmp8E0[0]=& _tmp8DF,((_tmp8E0[1]=& _tmp8DE,Cyc_Tcutil_terr(_tmp3F0->loc,((_tmp8E1="expecting index designator %d but found %d",_tag_dyneither(_tmp8E1,sizeof(char),43))),_tag_dyneither(_tmp8E0,sizeof(void*),2)))))))))))));}}
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
const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;(_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8E4[0]=& _tmp8E3,Cyc_Tcutil_terr(bound->loc,((_tmp8E5="expecting unsigned int, found %s",_tag_dyneither(_tmp8E5,sizeof(char),33))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));}_LL1CC:;}
# 1880
if(!(vd->tq).real_const){
const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp8E8="comprehension index variable is not declared const!",_tag_dyneither(_tmp8E8,sizeof(char),52))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}{
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
{void**_tmp8E9;_tmp408=((_tmp8E9=_region_malloc(r,sizeof(*_tmp8E9)),((_tmp8E9[0]=_tmp40D.elt_typ,_tmp8E9))));}
{struct Cyc_Absyn_Tqual*_tmp8EA;_tmp409=((_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=_tmp40D.elt_tq,_tmp8EA))));}
{union Cyc_Absyn_Constraint**_tmp8EB;_tmp40A=((_tmp8EB=_region_malloc(r,sizeof(*_tmp8EB)),((_tmp8EB[0]=(_tmp40D.ptr_atts).zero_term,_tmp8EB))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40B;if(_tmp40E->tag != 8)goto _LL1D6;else{_tmp40F=(_tmp40E->f1).elt_type;_tmp410=(_tmp40E->f1).tq;_tmp411=(_tmp40E->f1).num_elts;_tmp412=(_tmp40E->f1).zero_term;}}_LL1D5:
# 1902
{void**_tmp8EC;_tmp408=((_tmp8EC=_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC[0]=_tmp40F,_tmp8EC))));}
{struct Cyc_Absyn_Tqual*_tmp8ED;_tmp409=((_tmp8ED=_region_malloc(r,sizeof(*_tmp8ED)),((_tmp8ED[0]=_tmp410,_tmp8ED))));}
{union Cyc_Absyn_Constraint**_tmp8EE;_tmp40A=((_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE[0]=_tmp412,_tmp8EE))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1907
 goto _LL1D1;_LL1D1:;}{
# 1910
void*t=Cyc_Tcexp_tcExp(_tmp407,_tmp408,body);
# 1912
struct _RegionHandle _tmp419=_new_region("temp");struct _RegionHandle*temp=& _tmp419;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,Cyc_Tcutil_terr(body->loc,((_tmp8F1="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp8F1,sizeof(char),49))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}
# 1916
if(_tmp407->le == 0){
# 1918
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp8F4;void*_tmp8F3;(_tmp8F3=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F4="bound is not constant",_tag_dyneither(_tmp8F4,sizeof(char),22))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp8F7;void*_tmp8F6;(_tmp8F6=0,Cyc_Tcutil_terr(bound->loc,((_tmp8F7="body is not constant",_tag_dyneither(_tmp8F7,sizeof(char),21))),_tag_dyneither(_tmp8F6,sizeof(void*),0)));}}
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
const char*_tmp8FA;void*_tmp8F9;(_tmp8F9=0,Cyc_Tcutil_terr(body->loc,((_tmp8FA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp8FA,sizeof(char),49))),_tag_dyneither(_tmp8F9,sizeof(void*),0)));}{
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
const char*_tmp8FE;void*_tmp8FD[1];struct Cyc_String_pa_PrintArg_struct _tmp8FC;(_tmp8FC.tag=0,((_tmp8FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp8FD[0]=& _tmp8FC,Cyc_Tcutil_terr(bound->loc,((_tmp8FE="expecting unsigned int, found %s",_tag_dyneither(_tmp8FE,sizeof(char),33))),_tag_dyneither(_tmp8FD,sizeof(void*),1)))))));}_LL1D8:;}{
# 1965
struct _RegionHandle _tmp42D=_new_region("r");struct _RegionHandle*r=& _tmp42D;_push_region(r);
{void**_tmp42E=0;
struct Cyc_Absyn_Tqual*_tmp42F=0;
union Cyc_Absyn_Constraint**_tmp430=0;
# 1970
if(topt != 0){
void*_stmttmp3F=Cyc_Tcutil_compress(*topt);void*_tmp431=_stmttmp3F;struct Cyc_Absyn_PtrInfo _tmp433;void*_tmp435;struct Cyc_Absyn_Tqual _tmp436;struct Cyc_Absyn_Exp*_tmp437;union Cyc_Absyn_Constraint*_tmp438;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp432=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp431;if(_tmp432->tag != 5)goto _LL1E0;else{_tmp433=_tmp432->f1;}}_LL1DF:
# 1973
{void**_tmp8FF;_tmp42E=((_tmp8FF=_region_malloc(r,sizeof(*_tmp8FF)),((_tmp8FF[0]=_tmp433.elt_typ,_tmp8FF))));}
{struct Cyc_Absyn_Tqual*_tmp900;_tmp42F=((_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900[0]=_tmp433.elt_tq,_tmp900))));}
{union Cyc_Absyn_Constraint**_tmp901;_tmp430=((_tmp901=_region_malloc(r,sizeof(*_tmp901)),((_tmp901[0]=(_tmp433.ptr_atts).zero_term,_tmp901))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp431;if(_tmp434->tag != 8)goto _LL1E2;else{_tmp435=(_tmp434->f1).elt_type;_tmp436=(_tmp434->f1).tq;_tmp437=(_tmp434->f1).num_elts;_tmp438=(_tmp434->f1).zero_term;}}_LL1E1:
# 1978
{void**_tmp902;_tmp42E=((_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=_tmp435,_tmp902))));}
{struct Cyc_Absyn_Tqual*_tmp903;_tmp42F=((_tmp903=_region_malloc(r,sizeof(*_tmp903)),((_tmp903[0]=_tmp436,_tmp903))));}
{union Cyc_Absyn_Constraint**_tmp904;_tmp430=((_tmp904=_region_malloc(r,sizeof(*_tmp904)),((_tmp904[0]=_tmp438,_tmp904))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1983
 goto _LL1DD;_LL1DD:;}
# 1986
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp42E != 0)Cyc_Tcutil_unify(*_tmp42E,t);
# 1989
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp907;void*_tmp906;(_tmp906=0,Cyc_Tcutil_terr(bound->loc,((_tmp907="bound is not constant",_tag_dyneither(_tmp907,sizeof(char),22))),_tag_dyneither(_tmp906,sizeof(void*),0)));}}
# 1993
if(_tmp430 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp430)){
# 1996
struct Cyc_Absyn_Exp*_tmp441=Cyc_Absyn_uint_exp(1,0);_tmp441->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp441,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2000
const char*_tmp90A;void*_tmp909;(_tmp909=0,Cyc_Tcutil_terr(loc,((_tmp90A="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp90A,sizeof(char),74))),_tag_dyneither(_tmp909,sizeof(void*),0)));};}{
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
struct Cyc_Absyn_Aggrdecl**_tmp90B;adptr=((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=ad,_tmp90B))));};}else{
# 2029
{struct _handler_cons _tmp447;_push_handler(& _tmp447);{int _tmp449=0;if(setjmp(_tmp447.handler))_tmp449=1;if(!_tmp449){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2029
;_pop_handler();}else{void*_tmp448=(void*)_exn_thrown;void*_tmp44B=_tmp448;void*_tmp44D;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp44C=(struct Cyc_Dict_Absent_exn_struct*)_tmp44B;if(_tmp44C->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2032
{const char*_tmp90F;void*_tmp90E[1];struct Cyc_String_pa_PrintArg_struct _tmp90D;(_tmp90D.tag=0,((_tmp90D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp90E[0]=& _tmp90D,Cyc_Tcutil_terr(loc,((_tmp90F="unbound struct/union name %s",_tag_dyneither(_tmp90F,sizeof(char),29))),_tag_dyneither(_tmp90E,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp44D=_tmp44B;_LL1E8:(void)_throw(_tmp44D);_LL1E4:;}};}
# 2035
*ad_opt=ad;
*tn=ad->name;}
# 2038
if(ad->impl == 0){
{const char*_tmp917;void*_tmp916[1];const char*_tmp915;const char*_tmp914;struct Cyc_String_pa_PrintArg_struct _tmp913;(_tmp913.tag=0,((_tmp913.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp914="struct",_tag_dyneither(_tmp914,sizeof(char),7)):((_tmp915="union",_tag_dyneither(_tmp915,sizeof(char),6)))),((_tmp916[0]=& _tmp913,Cyc_Tcutil_terr(loc,((_tmp917="can't construct abstract %s",_tag_dyneither(_tmp917,sizeof(char),28))),_tag_dyneither(_tmp916,sizeof(void*),1)))))));}
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
struct _tuple12 _tmp918;struct _tuple12 _tmp458=(_tmp918.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp918.f2=rgn,_tmp918)));
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
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91E;struct Cyc_Absyn_AggrInfo _tmp91D;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp91C;res_typ=(void*)((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91E.tag=11,((_tmp91E.f1=((_tmp91D.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp91D.targs=_tmp45B,_tmp91D)))),_tmp91E)))),_tmp91C))));}_LL1E9:;}else{
# 2085
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp924;struct Cyc_Absyn_AggrInfo _tmp923;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp922;res_typ=(void*)((_tmp922=_cycalloc(sizeof(*_tmp922)),((_tmp922[0]=((_tmp924.tag=11,((_tmp924.f1=((_tmp923.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp923.targs=_tmp45B,_tmp923)))),_tmp924)))),_tmp922))));}{
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
const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_Tcutil_terr(loc,((_tmp927="too many explicit witness types",_tag_dyneither(_tmp927,sizeof(char),32))),_tag_dyneither(_tmp926,sizeof(void*),0)));}
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
{const char*_tmp933;void*_tmp932[5];struct Cyc_String_pa_PrintArg_struct _tmp931;const char*_tmp930;const char*_tmp92F;struct Cyc_String_pa_PrintArg_struct _tmp92E;struct Cyc_String_pa_PrintArg_struct _tmp92D;struct Cyc_String_pa_PrintArg_struct _tmp92C;struct Cyc_String_pa_PrintArg_struct _tmp92B;(_tmp92B.tag=0,((_tmp92B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2119
Cyc_Absynpp_typ2string((void*)_check_null(_tmp46F->topt))),((_tmp92C.tag=0,((_tmp92C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2118
Cyc_Absynpp_typ2string(_tmp470)),((_tmp92D.tag=0,((_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp92E.tag=0,((_tmp92E.f1=(struct _dyneither_ptr)(
# 2117
ad->kind == Cyc_Absyn_StructA?(_tmp92F="struct",_tag_dyneither(_tmp92F,sizeof(char),7)):((_tmp930="union",_tag_dyneither(_tmp930,sizeof(char),6)))),((_tmp931.tag=0,((_tmp931.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46E->name),((_tmp932[0]=& _tmp931,((_tmp932[1]=& _tmp92E,((_tmp932[2]=& _tmp92D,((_tmp932[3]=& _tmp92C,((_tmp932[4]=& _tmp92B,Cyc_Tcutil_terr(_tmp46F->loc,((_tmp933="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp933,sizeof(char),40))),_tag_dyneither(_tmp932,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2120
Cyc_Tcutil_explain_failure();}};}{
# 2124
struct Cyc_List_List*_tmp47A=0;
{struct Cyc_List_List*_tmp47B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp47B != 0;_tmp47B=_tmp47B->tl){
struct _tuple0*_tmp936;struct Cyc_List_List*_tmp935;_tmp47A=((_tmp935=_cycalloc(sizeof(*_tmp935)),((_tmp935->hd=((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp45D,(*((struct _tuple0*)_tmp47B->hd)).f1),((_tmp936->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp45D,(*((struct _tuple0*)_tmp47B->hd)).f2),_tmp936)))))),((_tmp935->tl=_tmp47A,_tmp935))))));}}
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
const char*_tmp939;void*_tmp938;(_tmp938=0,Cyc_Tcutil_terr(loc,((_tmp939="expecting struct but found union",_tag_dyneither(_tmp939,sizeof(char),33))),_tag_dyneither(_tmp938,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp484);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_stmttmp43=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp488;struct Cyc_Absyn_Exp*_tmp489;struct _tuple19*_tmp487=_stmttmp43;_tmp488=_tmp487->f1;_tmp489=_tmp487->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp488->type,_tmp489);
# 2150
if(!Cyc_Tcutil_coerce_arg(te,_tmp489,_tmp488->type,& bogus)){
{const char*_tmp93F;void*_tmp93E[3];struct Cyc_String_pa_PrintArg_struct _tmp93D;struct Cyc_String_pa_PrintArg_struct _tmp93C;struct Cyc_String_pa_PrintArg_struct _tmp93B;(_tmp93B.tag=0,((_tmp93B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2153
Cyc_Absynpp_typ2string((void*)_check_null(_tmp489->topt))),((_tmp93C.tag=0,((_tmp93C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2152
Cyc_Absynpp_typ2string(_tmp488->type)),((_tmp93D.tag=0,((_tmp93D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp488->name),((_tmp93E[0]=& _tmp93D,((_tmp93E[1]=& _tmp93C,((_tmp93E[2]=& _tmp93B,Cyc_Tcutil_terr(_tmp489->loc,((_tmp93F="field %s of struct expects type %s != %s",_tag_dyneither(_tmp93F,sizeof(char),41))),_tag_dyneither(_tmp93E,sizeof(void*),3)))))))))))))))))));}
# 2154
Cyc_Tcutil_explain_failure();}}
# 2157
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp942;void*_tmp941;(_tmp941=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp942="tcAnonStruct: wrong type",_tag_dyneither(_tmp942,sizeof(char),25))),_tag_dyneither(_tmp941,sizeof(void*),0)));}_LL1EE:;}{
# 2161
void*_tmp491=ts;_npop_handler(0);return _tmp491;};
# 2138
;_pop_region(rgn);}
# 2165
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2168
struct _RegionHandle _tmp492=_new_region("rgn");struct _RegionHandle*rgn=& _tmp492;_push_region(rgn);
{struct _tuple12 _tmp943;struct _tuple12 _tmp493=(_tmp943.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp943.f2=rgn,_tmp943)));
struct Cyc_List_List*_tmp494=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp493,tud->tvs);
struct Cyc_List_List*_tmp495=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp494);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp949;struct Cyc_Absyn_DatatypeFieldInfo _tmp948;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp947;void*res=(void*)(
(_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947[0]=((_tmp949.tag=4,((_tmp949.f1=((_tmp948.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp948.targs=_tmp495,_tmp948)))),_tmp949)))),_tmp947))));
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
{const char*_tmp951;void*_tmp950[3];struct Cyc_String_pa_PrintArg_struct _tmp94F;struct Cyc_String_pa_PrintArg_struct _tmp94E;const char*_tmp94D;struct Cyc_String_pa_PrintArg_struct _tmp94C;(_tmp94C.tag=0,((_tmp94C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2193
e->topt == 0?(struct _dyneither_ptr)((_tmp94D="?",_tag_dyneither(_tmp94D,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp94E.tag=0,((_tmp94E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2192
Cyc_Absynpp_typ2string(t)),((_tmp94F.tag=0,((_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp950[0]=& _tmp94F,((_tmp950[1]=& _tmp94E,((_tmp950[2]=& _tmp94C,Cyc_Tcutil_terr(e->loc,((_tmp951="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp951,sizeof(char),82))),_tag_dyneither(_tmp950,sizeof(void*),3)))))))))))))))))));}
# 2194
Cyc_Tcutil_explain_failure();}}
# 2197
if(es != 0){
const char*_tmp955;void*_tmp954[1];struct Cyc_String_pa_PrintArg_struct _tmp953;void*_tmp4A1=(_tmp953.tag=0,((_tmp953.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp954[0]=& _tmp953,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp955="too many arguments for datatype constructor %s",_tag_dyneither(_tmp955,sizeof(char),47))),_tag_dyneither(_tmp954,sizeof(void*),1)))))));_npop_handler(0);return _tmp4A1;}
if(ts != 0){
const char*_tmp959;void*_tmp958[1];struct Cyc_String_pa_PrintArg_struct _tmp957;void*_tmp4A5=(_tmp957.tag=0,((_tmp957.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp958[0]=& _tmp957,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp959="too few arguments for datatype constructor %s",_tag_dyneither(_tmp959,sizeof(char),46))),_tag_dyneither(_tmp958,sizeof(void*),1)))))));_npop_handler(0);return _tmp4A5;}{
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
const char*_tmp95D;void*_tmp95C[1];struct Cyc_String_pa_PrintArg_struct _tmp95B;(_tmp95B.tag=0,((_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp95C[0]=& _tmp95B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp95D="bad type `%s' in zeroable type",_tag_dyneither(_tmp95D,sizeof(char),31))),_tag_dyneither(_tmp95C,sizeof(void*),1)))))));}_LL1F8:;}
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
const char*_tmp966;void*_tmp965[2];const char*_tmp964;const char*_tmp963;struct Cyc_String_pa_PrintArg_struct _tmp962;struct Cyc_String_pa_PrintArg_struct _tmp961;(_tmp961.tag=0,((_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp963="calloc",_tag_dyneither(_tmp963,sizeof(char),7))):(struct _dyneither_ptr)((_tmp964="malloc",_tag_dyneither(_tmp964,sizeof(char),7))))),((_tmp965[0]=& _tmp962,((_tmp965[1]=& _tmp961,Cyc_Tcutil_terr(loc,((_tmp966="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp966,sizeof(char),60))),_tag_dyneither(_tmp965,sizeof(void*),2)))))))))))));};}
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
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp969;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp968;void*expected_type=(void*)(
(_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=15,((_tmp969.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp969)))),_tmp968))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_stmttmp48=Cyc_Tcutil_compress(handle_type);void*_tmp4DE=_stmttmp48;void*_tmp4E0;_LL238: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4DF->tag != 15)goto _LL23A;else{_tmp4E0=(void*)_tmp4DF->f1;}}_LL239:
# 2295
 rgn=_tmp4E0;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL237;_LL23A:;_LL23B:
# 2299
{const char*_tmp96D;void*_tmp96C[1];struct Cyc_String_pa_PrintArg_struct _tmp96B;(_tmp96B.tag=0,((_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp96C[0]=& _tmp96B,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp96D="expecting region_t type but found %s",_tag_dyneither(_tmp96D,sizeof(char),37))),_tag_dyneither(_tmp96C,sizeof(void*),1)))))));}
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
if(*t == 0){const char*_tmp970;void*_tmp96F;(_tmp96F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp970="calloc with empty type",_tag_dyneither(_tmp970,sizeof(char),23))),_tag_dyneither(_tmp96F,sizeof(void*),0)));}
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
{void**_tmp971;*t=((_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971[0]=elt_type,_tmp971))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL23C;_LL23F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E9;if(_tmp4EC->tag != 2)goto _LL241;else{if(_tmp4EC->f1 != Cyc_Absyn_Times)goto _LL241;if(_tmp4EC->f2 == 0)goto _LL241;_tmp4ED=(struct Cyc_Absyn_Exp*)(_tmp4EC->f2)->hd;if((_tmp4EC->f2)->tl == 0)goto _LL241;_tmp4EE=(struct Cyc_Absyn_Exp*)((_tmp4EC->f2)->tl)->hd;if(((_tmp4EC->f2)->tl)->tl != 0)goto _LL241;}}_LL240:
# 2347
{struct _tuple0 _tmp972;struct _tuple0 _stmttmp4C=(_tmp972.f1=_tmp4ED->r,((_tmp972.f2=_tmp4EE->r,_tmp972)));struct _tuple0 _tmp4F0=_stmttmp4C;void*_tmp4F2;void*_tmp4F4;_LL244: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F0.f1;if(_tmp4F1->tag != 16)goto _LL246;else{_tmp4F2=(void*)_tmp4F1->f1;}}_LL245:
# 2349
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4F2);
elt_type=_tmp4F2;
{void**_tmp973;*t=((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973[0]=elt_type,_tmp973))));}
num_elts=_tmp4EE;
one_elt=0;
goto _LL243;_LL246: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4F3=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F0.f2;if(_tmp4F3->tag != 16)goto _LL248;else{_tmp4F4=(void*)_tmp4F3->f1;}}_LL247:
# 2356
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp4F4);
elt_type=_tmp4F4;
{void**_tmp974;*t=((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974[0]=elt_type,_tmp974))));}
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
{void**_tmp975;*t=((_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=elt_type,_tmp975))));}
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
const char*_tmp978;void*_tmp977;(_tmp977=0,Cyc_Tcutil_terr(loc,((_tmp978="malloc with existential types not yet implemented",_tag_dyneither(_tmp978,sizeof(char),50))),_tag_dyneither(_tmp977,sizeof(void*),0)));}
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
{const char*_tmp97B;void*_tmp97A;(_tmp97A=0,Cyc_Tcutil_warn(loc,((_tmp97B="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp97B,sizeof(char),55))),_tag_dyneither(_tmp97A,sizeof(void*),0)));}
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
{const char*_tmp97E;void*_tmp97D;(_tmp97D=0,Cyc_Tcutil_terr(loc,((_tmp97E="cannot assign to an array",_tag_dyneither(_tmp97E,sizeof(char),26))),_tag_dyneither(_tmp97D,sizeof(void*),0)));}goto _LL26F;_LL272:;_LL273:
 goto _LL26F;_LL26F:;}{
# 2473
int ign_1=0;
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
const char*_tmp981;void*_tmp980;(_tmp980=0,Cyc_Tcutil_terr(loc,((_tmp981="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp981,sizeof(char),58))),_tag_dyneither(_tmp980,sizeof(void*),0)));}
# 2478
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp984;void*_tmp983;void*_tmp525=(_tmp983=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp984="swap non-lvalue",_tag_dyneither(_tmp984,sizeof(char),16))),_tag_dyneither(_tmp983,sizeof(void*),0)));_npop_handler(0);return _tmp525;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp987;void*_tmp986;void*_tmp528=(_tmp986=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp987="swap non-lvalue",_tag_dyneither(_tmp987,sizeof(char),16))),_tag_dyneither(_tmp986,sizeof(void*),0)));_npop_handler(0);return _tmp528;}{
# 2483
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp98A;void*_tmp989;void*_tmp52B=(_tmp989=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp98A="swap value in zeroterm array",_tag_dyneither(_tmp98A,sizeof(char),29))),_tag_dyneither(_tmp989,sizeof(void*),0)));_npop_handler(0);return _tmp52B;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp98D;void*_tmp98C;void*_tmp52E=(_tmp98C=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp98D="swap value in zeroterm array",_tag_dyneither(_tmp98D,sizeof(char),29))),_tag_dyneither(_tmp98C,sizeof(void*),0)));_npop_handler(0);return _tmp52E;}
# 2490
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp992;void*_tmp991[2];struct Cyc_String_pa_PrintArg_struct _tmp990;struct Cyc_String_pa_PrintArg_struct _tmp98F;void*_tmp52F=(_tmp98F.tag=0,((_tmp98F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp990.tag=0,((_tmp990.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp991[0]=& _tmp990,((_tmp991[1]=& _tmp98F,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp992="type mismatch: %s != %s",_tag_dyneither(_tmp992,sizeof(char),24))),_tag_dyneither(_tmp991,sizeof(void*),2)))))))))))));
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
{const char*_tmp996;void*_tmp995[1];struct Cyc_String_pa_PrintArg_struct _tmp994;(_tmp994.tag=0,((_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp540)),((_tmp995[0]=& _tmp994,Cyc_Tcutil_terr(loc,((_tmp996="statement expression returns type %s",_tag_dyneither(_tmp996,sizeof(char),37))),_tag_dyneither(_tmp995,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2522
void*_tmp544=_tmp540;_npop_handler(0);return _tmp544;};}_LL277: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp53A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp537;if(_tmp53A->tag != 2)goto _LL279;else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;}}_LL278:
 s=_tmp53C;continue;_LL279: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp53D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp537;if(_tmp53D->tag != 12)goto _LL27B;else{_tmp53E=_tmp53D->f1;_tmp53F=_tmp53D->f2;}}_LL27A:
 s=_tmp53F;continue;_LL27B:;_LL27C: {
# 2526
const char*_tmp999;void*_tmp998;void*_tmp547=(_tmp998=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp999="statement expression must end with expression",_tag_dyneither(_tmp999,sizeof(char),46))),_tag_dyneither(_tmp998,sizeof(void*),0)));_npop_handler(0);return _tmp547;}_LL274:;}
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
{const char*_tmp99D;void*_tmp99C[1];struct Cyc_String_pa_PrintArg_struct _tmp99B;(_tmp99B.tag=0,((_tmp99B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp99C[0]=& _tmp99B,Cyc_Tcutil_terr(loc,((_tmp99D="expecting @tagged union but found %s",_tag_dyneither(_tmp99D,sizeof(char),37))),_tag_dyneither(_tmp99C,sizeof(void*),1)))))));}
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
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9A0;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp99F;void*expected_type=(void*)(
(_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F[0]=((_tmp9A0.tag=15,((_tmp9A0.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp551)),_tmp9A0)))),_tmp99F))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp551,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_stmttmp52=Cyc_Tcutil_compress(handle_type);void*_tmp552=_stmttmp52;void*_tmp554;_LL283: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp552;if(_tmp553->tag != 15)goto _LL285;else{_tmp554=(void*)_tmp553->f1;}}_LL284:
# 2562
 rgn=_tmp554;
Cyc_Tcenv_check_rgn_accessible(_tmp551,loc,rgn);
goto _LL282;_LL285:;_LL286:
# 2566
{const char*_tmp9A4;void*_tmp9A3[1];struct Cyc_String_pa_PrintArg_struct _tmp9A2;(_tmp9A2.tag=0,((_tmp9A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9A3[0]=& _tmp9A2,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9A4="expecting region_t type but found %s",_tag_dyneither(_tmp9A4,sizeof(char),37))),_tag_dyneither(_tmp9A3,sizeof(void*),1)))))));}
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
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp551,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp9A7;void*_tmp9A6;(_tmp9A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A7="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9A7,sizeof(char),45))),_tag_dyneither(_tmp9A6,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2597
void*_tmp566=res_typ;_npop_handler(0);return _tmp566;};}_LL28C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp55D->tag != 35)goto _LL28E;else{_tmp55E=_tmp55D->f1;_tmp55F=_tmp55D->f2;}}_LL28D:
# 2599
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9AA;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9A9;e1->r=(void*)((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9[0]=((_tmp9AA.tag=25,((_tmp9AA.f1=_tmp55F,_tmp9AA)))),_tmp9A9))));}
_tmp561=_tmp55F;goto _LL28F;_LL28E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp560->tag != 25)goto _LL290;else{_tmp561=_tmp560->f1;}}_LL28F: {
# 2602
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp55=Cyc_Tcutil_compress(*topt);void*_tmp569=_stmttmp55;void**_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;union Cyc_Absyn_Constraint*_tmp56D;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp569;if(_tmp56A->tag != 5)goto _LL299;else{_tmp56B=(void**)&(_tmp56A->f1).elt_typ;_tmp56C=(_tmp56A->f1).elt_tq;_tmp56D=((_tmp56A->f1).ptr_atts).zero_term;}}_LL298:
# 2608
 elt_typ_opt=_tmp56B;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56D);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2614
void*res_typ=Cyc_Tcexp_tcArray(_tmp551,e1->loc,elt_typ_opt,0,zero_term,_tmp561);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmp9AD;void*_tmp9AC;(_tmp9AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AD="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmp9AD,sizeof(char),50))),_tag_dyneither(_tmp9AC,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2623
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2629
void*_tmp570=res_typ;_npop_handler(0);return _tmp570;};};}_LL290: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp562=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp562->tag != 0)goto _LL292;else{if(((_tmp562->f1).String_c).tag != 8)goto _LL292;}}_LL291: {
# 2634
void*_tmp571=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2636
void*_tmp572=Cyc_Tcexp_tcExp(_tmp551,& _tmp571,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9AF;void*_tmp575=Cyc_Absyn_atb_typ(_tmp572,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF[0]=((_tmp9B0.tag=1,((_tmp9B0.f1=Cyc_Absyn_uint_exp(1,0),_tmp9B0)))),_tmp9AF)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp575;}_LL292: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp563=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp55A;if(_tmp563->tag != 0)goto _LL294;else{if(((_tmp563->f1).Wstring_c).tag != 9)goto _LL294;}}_LL293: {
# 2641
void*_tmp576=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2643
void*_tmp577=Cyc_Tcexp_tcExp(_tmp551,& _tmp576,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9B3;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9B2;void*_tmp57A=Cyc_Absyn_atb_typ(_tmp577,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B3.tag=1,((_tmp9B3.f1=Cyc_Absyn_uint_exp(1,0),_tmp9B3)))),_tmp9B2)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp57A;}_LL294:;_LL295:
# 2650
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_stmttmp54=Cyc_Tcutil_compress(*topt);void*_tmp57B=_stmttmp54;void**_tmp57D;struct Cyc_Absyn_Tqual _tmp57E;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B;if(_tmp57C->tag != 5)goto _LL29E;else{_tmp57D=(void**)&(_tmp57C->f1).elt_typ;_tmp57E=(_tmp57C->f1).elt_tq;}}_LL29D:
# 2656
 topt2=_tmp57D;goto _LL29B;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp57F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp57B;if(_tmp57F->tag != 3)goto _LL2A0;}_LL29F:
# 2660
 bogus=*topt;
topt2=& bogus;
goto _LL29B;_LL2A0:;_LL2A1:
 goto _LL29B;_LL29B:;}{
# 2666
void*telt=Cyc_Tcexp_tcExp(_tmp551,topt2,e1);
# 2668
struct _RegionHandle _tmp580=_new_region("temp");struct _RegionHandle*temp=& _tmp580;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmp9B6;void*_tmp9B5;(_tmp9B5=0,Cyc_Tcutil_terr(e1->loc,((_tmp9B6="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9B6,sizeof(char),49))),_tag_dyneither(_tmp9B5,sizeof(void*),0)));}{
# 2673
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9BC;struct Cyc_Absyn_PtrInfo _tmp9BB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9BA;void*res_typ=(void*)(
(_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA[0]=((_tmp9BC.tag=5,((_tmp9BC.f1=((_tmp9BB.elt_typ=telt,((_tmp9BB.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp9BB.ptr_atts=(
((_tmp9BB.ptr_atts).rgn=rgn,(((_tmp9BB.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp9BB.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp9BB.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp9BB.ptr_atts).ptrloc=0,_tmp9BB.ptr_atts)))))))))),_tmp9BB)))))),_tmp9BC)))),_tmp9BA))));
# 2678
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp551,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp551,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2685
void*_tmp583=res_typ;_npop_handler(1);return _tmp583;};};
# 2668
;_pop_region(temp);};}_LL287:;};}
# 2554
;_pop_region(r);}
# 2692
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2702
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2706
struct _RegionHandle _tmp587=_new_region("temp");struct _RegionHandle*temp=& _tmp587;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2712
const char*_tmp9BF;void*_tmp9BE;(_tmp9BE=0,Cyc_Tcutil_terr(e->loc,((_tmp9BF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp9BF,sizeof(char),49))),_tag_dyneither(_tmp9BE,sizeof(void*),0)));}{
# 2715
void*_stmttmp56=e->r;void*_tmp58A=_stmttmp56;_LL2A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp58B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58B->tag != 25)goto _LL2A5;}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp58C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58C->tag != 26)goto _LL2A7;}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58D->tag != 27)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp58E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58E->tag != 0)goto _LL2AB;else{if(((_tmp58E->f1).Wstring_c).tag != 9)goto _LL2AB;}}_LL2AA:
 goto _LL2AC;_LL2AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp58F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp58A;if(_tmp58F->tag != 0)goto _LL2AD;else{if(((_tmp58F->f1).String_c).tag != 8)goto _LL2AD;}}_LL2AC: {
void*_tmp590=t;_npop_handler(0);return _tmp590;}_LL2AD:;_LL2AE:
# 2722
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2725
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2728
void*_tmp591=t;_npop_handler(0);return _tmp591;};_LL2A2:;};
# 2706
;_pop_region(temp);}
# 2739 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_stmttmp57=e->r;void*_tmp592=_stmttmp57;struct Cyc_Absyn_Exp*_tmp594;_LL2B0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp592;if(_tmp593->tag != 11)goto _LL2B2;else{_tmp594=_tmp593->f1;}}_LL2B1:
# 2743
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp594);
_tmp594->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp594->topt),0);
e->topt=_tmp594->topt;
goto _LL2AF;_LL2B2:;_LL2B3:
# 2749
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2752
{void*_stmttmp58=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp595=_stmttmp58;void*_tmp597;struct Cyc_Absyn_Tqual _tmp598;void*_tmp599;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59C;_LL2B5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 5)goto _LL2B7;else{_tmp597=(_tmp596->f1).elt_typ;_tmp598=(_tmp596->f1).elt_tq;_tmp599=((_tmp596->f1).ptr_atts).rgn;_tmp59A=((_tmp596->f1).ptr_atts).nullable;_tmp59B=((_tmp596->f1).ptr_atts).bounds;_tmp59C=((_tmp596->f1).ptr_atts).zero_term;}}_LL2B6:
# 2754
{void*_stmttmp59=Cyc_Tcutil_compress(_tmp597);void*_tmp59D=_stmttmp59;struct Cyc_List_List*_tmp59F;void*_tmp5A0;struct Cyc_Absyn_Tqual _tmp5A1;void*_tmp5A2;struct Cyc_List_List*_tmp5A3;int _tmp5A4;struct Cyc_Absyn_VarargInfo*_tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_List_List*_tmp5A7;_LL2BA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59D;if(_tmp59E->tag != 9)goto _LL2BC;else{_tmp59F=(_tmp59E->f1).tvars;_tmp5A0=(_tmp59E->f1).effect;_tmp5A1=(_tmp59E->f1).ret_tqual;_tmp5A2=(_tmp59E->f1).ret_typ;_tmp5A3=(_tmp59E->f1).args;_tmp5A4=(_tmp59E->f1).c_varargs;_tmp5A5=(_tmp59E->f1).cyc_varargs;_tmp5A6=(_tmp59E->f1).rgn_po;_tmp5A7=(_tmp59E->f1).attributes;}}_LL2BB:
# 2756
 if(_tmp59F != 0){
struct _RegionHandle _tmp5A8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5A8;_push_region(rgn);
{struct _tuple12 _tmp9C0;struct _tuple12 _tmp5A9=(_tmp9C0.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9C0.f2=rgn,_tmp9C0)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp5A9,_tmp59F);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2765
_tmp5A6=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5A6);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp5A6);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9C6;struct Cyc_Absyn_FnInfo _tmp9C5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9C4;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp9C4=_cycalloc(sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C6.tag=9,((_tmp9C6.f1=((_tmp9C5.tvars=0,((_tmp9C5.effect=_tmp5A0,((_tmp9C5.ret_tqual=_tmp5A1,((_tmp9C5.ret_typ=_tmp5A2,((_tmp9C5.args=_tmp5A3,((_tmp9C5.c_varargs=_tmp5A4,((_tmp9C5.cyc_varargs=_tmp5A5,((_tmp9C5.rgn_po=0,((_tmp9C5.attributes=_tmp5A7,_tmp9C5)))))))))))))))))),_tmp9C6)))),_tmp9C4)))));
# 2771
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9CC;struct Cyc_Absyn_PtrInfo _tmp9CB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9CA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5AA=(_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CC.tag=5,((_tmp9CC.f1=((_tmp9CB.elt_typ=ftyp,((_tmp9CB.elt_tq=_tmp598,((_tmp9CB.ptr_atts=(((_tmp9CB.ptr_atts).rgn=_tmp599,(((_tmp9CB.ptr_atts).nullable=_tmp59A,(((_tmp9CB.ptr_atts).bounds=_tmp59B,(((_tmp9CB.ptr_atts).zero_term=_tmp59C,(((_tmp9CB.ptr_atts).ptrloc=0,_tmp9CB.ptr_atts)))))))))),_tmp9CB)))))),_tmp9CC)))),_tmp9CA)));
# 2773
struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp9CF;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9CE;e->r=(void*)((_tmp9CE=_cycalloc(sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=12,((_tmp9CF.f1=_tmp5AB,((_tmp9CF.f2=ts,_tmp9CF)))))),_tmp9CE))));}
e->topt=(void*)_tmp5AA;};}
# 2758
;_pop_region(rgn);}
# 2778
goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 2781
goto _LL2B4;_LL2B7:;_LL2B8:
 goto _LL2B4;_LL2B4:;}
# 2784
goto _LL2AF;_LL2AF:;}
# 2786
return(void*)_check_null(e->topt);}
# 2794
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2796
struct _RegionHandle _tmp5B5=_new_region("r");struct _RegionHandle*r=& _tmp5B5;_push_region(r);
{struct Cyc_List_List*_tmp5B6=0;
# 2799
{void*_stmttmp5A=fn_exp->r;void*_tmp5B7=_stmttmp5A;struct Cyc_List_List*_tmp5B9;_LL2BF: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5B8=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5B7;if(_tmp5B8->tag != 9)goto _LL2C1;else{_tmp5B9=_tmp5B8->f2;}}_LL2C0:
# 2801
{void*_stmttmp5B=e->r;void*_tmp5BA=_stmttmp5B;struct Cyc_List_List*_tmp5BC;_LL2C4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5BB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5BA;if(_tmp5BB->tag != 9)goto _LL2C6;else{_tmp5BC=_tmp5BB->f2;}}_LL2C5: {
# 2803
struct Cyc_List_List*_tmp5BD=alias_arg_exps;
int _tmp5BE=0;
while(_tmp5BD != 0){
while(_tmp5BE != (int)_tmp5BD->hd){
if(_tmp5B9 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmp9D9;struct Cyc_Int_pa_PrintArg_struct _tmp9D8;void*_tmp9D7[2];const char*_tmp9D6;void*_tmp9D5;(_tmp9D5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp9D9.tag=1,((_tmp9D9.f1=(unsigned long)((int)_tmp5BD->hd),((_tmp9D8.tag=1,((_tmp9D8.f1=(unsigned long)_tmp5BE,((_tmp9D7[0]=& _tmp9D8,((_tmp9D7[1]=& _tmp9D9,Cyc_aprintf(((_tmp9D6="bad count %d/%d for alias coercion!",_tag_dyneither(_tmp9D6,sizeof(char),36))),_tag_dyneither(_tmp9D7,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmp9D5,sizeof(void*),0)));}
# 2810
++ _tmp5BE;
_tmp5B9=_tmp5B9->tl;
_tmp5BC=((struct Cyc_List_List*)_check_null(_tmp5BC))->tl;}{
# 2815
struct _tuple11 _stmttmp5C=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5B9))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5BC))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;struct _tuple11 _tmp5C4=_stmttmp5C;_tmp5C5=_tmp5C4.f1;_tmp5C6=_tmp5C4.f2;
_tmp5B9->hd=(void*)_tmp5C6;
{struct Cyc_List_List*_tmp9DA;_tmp5B6=((_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA->hd=_tmp5C5,((_tmp9DA->tl=_tmp5B6,_tmp9DA))))));}
_tmp5BD=_tmp5BD->tl;};}
# 2820
goto _LL2C3;}_LL2C6:;_LL2C7: {
const char*_tmp9DD;void*_tmp9DC;(_tmp9DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9DD="not a function call!",_tag_dyneither(_tmp9DD,sizeof(char),21))),_tag_dyneither(_tmp9DC,sizeof(void*),0)));}_LL2C3:;}
# 2823
goto _LL2BE;_LL2C1:;_LL2C2: {
const char*_tmp9E0;void*_tmp9DF;(_tmp9DF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9E0="not a function call!",_tag_dyneither(_tmp9E0,sizeof(char),21))),_tag_dyneither(_tmp9DF,sizeof(void*),0)));}_LL2BE:;}
# 2828
while(_tmp5B6 != 0){
struct Cyc_Absyn_Decl*_tmp5CC=(struct Cyc_Absyn_Decl*)_tmp5B6->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp5CC,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp5B6=_tmp5B6->tl;}
# 2835
e->topt=0;
e->r=fn_exp->r;}
# 2797
;_pop_region(r);}
# 2842
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2846
{void*_stmttmp5D=e->r;void*_tmp5CD=_stmttmp5D;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_List_List*_tmp5D2;struct Cyc_Absyn_VarargCallInfo**_tmp5D3;int*_tmp5D4;struct Cyc_Core_Opt*_tmp5D6;struct Cyc_List_List*_tmp5D7;union Cyc_Absyn_Cnst*_tmp5D9;struct _tuple1*_tmp5DB;void**_tmp5DC;enum Cyc_Absyn_Primop _tmp5DE;struct Cyc_List_List*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5E1;enum Cyc_Absyn_Incrementor _tmp5E2;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Core_Opt*_tmp5E5;struct Cyc_Absyn_Exp*_tmp5E6;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5EC;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_List_List*_tmp5F6;struct Cyc_Absyn_VarargCallInfo**_tmp5F7;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_List_List*_tmp5FC;void*_tmp5FE;struct Cyc_Absyn_Exp*_tmp5FF;enum Cyc_Absyn_Coercion*_tmp600;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_Absyn_Exp**_tmp604;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp*_tmp607;void*_tmp609;void*_tmp60B;struct Cyc_List_List*_tmp60C;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_Exp*_tmp610;struct _dyneither_ptr*_tmp611;int*_tmp612;int*_tmp613;struct Cyc_Absyn_Exp*_tmp615;struct _dyneither_ptr*_tmp616;int*_tmp617;int*_tmp618;struct Cyc_Absyn_Exp*_tmp61A;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_List_List*_tmp61D;struct _tuple9*_tmp61F;struct Cyc_List_List*_tmp620;struct Cyc_List_List*_tmp622;struct Cyc_Absyn_Stmt*_tmp624;struct Cyc_Absyn_Vardecl*_tmp626;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Exp*_tmp628;int*_tmp629;struct Cyc_Absyn_Exp*_tmp62B;void*_tmp62C;int*_tmp62D;struct _tuple1**_tmp62F;struct Cyc_List_List**_tmp630;struct Cyc_List_List*_tmp631;struct Cyc_Absyn_Aggrdecl**_tmp632;void*_tmp634;struct Cyc_List_List*_tmp635;struct Cyc_List_List*_tmp637;struct Cyc_Absyn_Datatypedecl*_tmp638;struct Cyc_Absyn_Datatypefield*_tmp639;struct _tuple1**_tmp63B;struct Cyc_Absyn_Enumdecl*_tmp63C;struct Cyc_Absyn_Enumfield*_tmp63D;struct _tuple1**_tmp63F;void*_tmp640;struct Cyc_Absyn_Enumfield*_tmp641;int*_tmp643;struct Cyc_Absyn_Exp**_tmp644;void***_tmp645;struct Cyc_Absyn_Exp**_tmp646;int*_tmp647;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64C;struct _dyneither_ptr*_tmp64D;void*_tmp64F;_LL2C9: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5CE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5CE->tag != 11)goto _LL2CB;else{_tmp5CF=_tmp5CE->f1;}}_LL2CA:
# 2851
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp5CF);
return;_LL2CB:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5D0=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D0->tag != 9)goto _LL2CD;else{_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;_tmp5D3=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5D0->f3;_tmp5D4=(int*)& _tmp5D0->f4;}}if(!(!(*_tmp5D4)))goto _LL2CD;_LL2CC:
# 2857
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp5D1,_tmp5D2)){
*_tmp5D4=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2861
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2864
return;_LL2CD: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5D5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D5->tag != 35)goto _LL2CF;else{_tmp5D6=_tmp5D5->f1;_tmp5D7=_tmp5D5->f2;}}_LL2CE:
# 2868
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp5D7);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2CF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5D8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5D8->tag != 0)goto _LL2D1;else{_tmp5D9=(union Cyc_Absyn_Cnst*)& _tmp5D8->f1;}}_LL2D0:
# 2873
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp5D9,e);goto _LL2C8;_LL2D1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp5DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5DA->tag != 1)goto _LL2D3;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=(void**)((void**)& _tmp5DA->f2);}}_LL2D2:
# 2875
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp5DB,_tmp5DC);goto _LL2C8;_LL2D3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp5DD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5DD->tag != 2)goto _LL2D5;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;}}_LL2D4:
# 2877
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp5DE,_tmp5DF);goto _LL2C8;_LL2D5: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp5E0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5E0->tag != 4)goto _LL2D7;else{_tmp5E1=_tmp5E0->f1;_tmp5E2=_tmp5E0->f2;}}_LL2D6:
# 2879
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp5E1,_tmp5E2);goto _LL2C8;_LL2D7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5E3->tag != 3)goto _LL2D9;else{_tmp5E4=_tmp5E3->f1;_tmp5E5=_tmp5E3->f2;_tmp5E6=_tmp5E3->f3;}}_LL2D8:
# 2881
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp5E4,_tmp5E5,_tmp5E6);goto _LL2C8;_LL2D9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5E7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5E7->tag != 5)goto _LL2DB;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;_tmp5EA=_tmp5E7->f3;}}_LL2DA:
# 2883
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp5E8,_tmp5E9,_tmp5EA);goto _LL2C8;_LL2DB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5EB->tag != 6)goto _LL2DD;else{_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;}}_LL2DC:
# 2885
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp5EC,_tmp5ED);goto _LL2C8;_LL2DD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5EE->tag != 7)goto _LL2DF;else{_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;}}_LL2DE:
# 2887
 t=Cyc_Tcexp_tcOr(te,loc,_tmp5EF,_tmp5F0);goto _LL2C8;_LL2DF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp5F1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5F1->tag != 8)goto _LL2E1;else{_tmp5F2=_tmp5F1->f1;_tmp5F3=_tmp5F1->f2;}}_LL2E0:
# 2889
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp5F2,_tmp5F3);goto _LL2C8;_LL2E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5F4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5F4->tag != 9)goto _LL2E3;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;_tmp5F7=(struct Cyc_Absyn_VarargCallInfo**)& _tmp5F4->f3;}}_LL2E2: {
# 2895
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp651;_push_handler(& _tmp651);{int _tmp653=0;if(setjmp(_tmp651.handler))_tmp653=1;if(!_tmp653){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp652=(void*)_exn_thrown;void*_tmp655=_tmp652;struct _dyneither_ptr _tmp657;void*_tmp658;_LL31C: {struct Cyc_Core_Failure_exn_struct*_tmp656=(struct Cyc_Core_Failure_exn_struct*)_tmp655;if(_tmp656->tag != Cyc_Core_Failure)goto _LL31E;else{_tmp657=_tmp656->f1;}}_LL31D:
# 2902
 ok=0;
fn_exp=e;
goto _LL31B;_LL31E: _tmp658=_tmp655;_LL31F:(void)_throw(_tmp658);_LL31B:;}};}
# 2906
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp5F5,_tmp5F6,_tmp5F7,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2913
goto _LL2C8;}_LL2E3: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp5F8=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5F8->tag != 10)goto _LL2E5;else{_tmp5F9=_tmp5F8->f1;}}_LL2E4:
# 2915
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp5F9);goto _LL2C8;_LL2E5: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp5FA=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5FA->tag != 12)goto _LL2E7;else{_tmp5FB=_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}_LL2E6:
# 2917
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp5FB,_tmp5FC);goto _LL2C8;_LL2E7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5FD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp5FD->tag != 13)goto _LL2E9;else{_tmp5FE=(void*)_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;_tmp600=(enum Cyc_Absyn_Coercion*)& _tmp5FD->f4;}}_LL2E8:
# 2919
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp5FE,_tmp5FF,_tmp600);goto _LL2C8;_LL2E9: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp601=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp601->tag != 14)goto _LL2EB;else{_tmp602=_tmp601->f1;}}_LL2EA:
# 2921
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp602);goto _LL2C8;_LL2EB: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp603=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp603->tag != 15)goto _LL2ED;else{_tmp604=(struct Cyc_Absyn_Exp**)& _tmp603->f1;_tmp605=_tmp603->f2;}}_LL2EC:
# 2923
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp604,e,_tmp605);goto _LL2C8;_LL2ED: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp606=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp606->tag != 17)goto _LL2EF;else{_tmp607=_tmp606->f1;}}_LL2EE: {
# 2925
void*_tmp659=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp607);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp659);goto _LL2C8;}_LL2EF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp608=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp608->tag != 16)goto _LL2F1;else{_tmp609=(void*)_tmp608->f1;}}_LL2F0:
# 2928
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp609);goto _LL2C8;_LL2F1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp60A->tag != 18)goto _LL2F3;else{_tmp60B=(void*)_tmp60A->f1;_tmp60C=_tmp60A->f2;}}_LL2F2:
# 2930
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp60B,_tmp60C);goto _LL2C8;_LL2F3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp60D->tag != 19)goto _LL2F5;else{_tmp60E=_tmp60D->f1;}}_LL2F4:
# 2932
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp60E);goto _LL2C8;_LL2F5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp60F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp60F->tag != 20)goto _LL2F7;else{_tmp610=_tmp60F->f1;_tmp611=_tmp60F->f2;_tmp612=(int*)& _tmp60F->f3;_tmp613=(int*)& _tmp60F->f4;}}_LL2F6:
# 2934
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp610,_tmp611,_tmp612,_tmp613);goto _LL2C8;_LL2F7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp614->tag != 21)goto _LL2F9;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;_tmp617=(int*)& _tmp614->f3;_tmp618=(int*)& _tmp614->f4;}}_LL2F8:
# 2936
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp615,_tmp616,_tmp617,_tmp618);goto _LL2C8;_LL2F9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp619=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp619->tag != 22)goto _LL2FB;else{_tmp61A=_tmp619->f1;_tmp61B=_tmp619->f2;}}_LL2FA:
# 2938
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp61A,_tmp61B);goto _LL2C8;_LL2FB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp61C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp61C->tag != 23)goto _LL2FD;else{_tmp61D=_tmp61C->f1;}}_LL2FC:
# 2940
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp61D);goto _LL2C8;_LL2FD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp61E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp61E->tag != 24)goto _LL2FF;else{_tmp61F=_tmp61E->f1;_tmp620=_tmp61E->f2;}}_LL2FE:
# 2942
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp61F,_tmp620);goto _LL2C8;_LL2FF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp621=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp621->tag != 25)goto _LL301;else{_tmp622=_tmp621->f1;}}_LL300: {
# 2946
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_stmttmp5E=Cyc_Tcutil_compress(*topt);void*_tmp65A=_stmttmp5E;void**_tmp65C;struct Cyc_Absyn_Tqual*_tmp65D;union Cyc_Absyn_Constraint*_tmp65E;_LL321: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65A;if(_tmp65B->tag != 8)goto _LL323;else{_tmp65C=(void**)&(_tmp65B->f1).elt_type;_tmp65D=(struct Cyc_Absyn_Tqual*)&(_tmp65B->f1).tq;_tmp65E=(_tmp65B->f1).zero_term;}}_LL322:
# 2952
 elt_topt=_tmp65C;
elt_tqopt=_tmp65D;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp65E);
goto _LL320;_LL323:;_LL324:
 goto _LL320;_LL320:;}
# 2959
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp622);goto _LL2C8;}_LL301: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp623=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp623->tag != 36)goto _LL303;else{_tmp624=_tmp623->f1;}}_LL302:
# 2961
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp624);goto _LL2C8;_LL303: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp625=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp625->tag != 26)goto _LL305;else{_tmp626=_tmp625->f1;_tmp627=_tmp625->f2;_tmp628=_tmp625->f3;_tmp629=(int*)& _tmp625->f4;}}_LL304:
# 2963
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp626,_tmp627,_tmp628,_tmp629);goto _LL2C8;_LL305: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp62A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp62A->tag != 27)goto _LL307;else{_tmp62B=_tmp62A->f1;_tmp62C=(void*)_tmp62A->f2;_tmp62D=(int*)& _tmp62A->f3;}}_LL306:
# 2965
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp62B,_tmp62C,_tmp62D);goto _LL2C8;_LL307: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp62E->tag != 28)goto _LL309;else{_tmp62F=(struct _tuple1**)& _tmp62E->f1;_tmp630=(struct Cyc_List_List**)& _tmp62E->f2;_tmp631=_tmp62E->f3;_tmp632=(struct Cyc_Absyn_Aggrdecl**)& _tmp62E->f4;}}_LL308:
# 2967
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp62F,_tmp630,_tmp631,_tmp632);goto _LL2C8;_LL309: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp633=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp633->tag != 29)goto _LL30B;else{_tmp634=(void*)_tmp633->f1;_tmp635=_tmp633->f2;}}_LL30A:
# 2969
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp634,_tmp635);goto _LL2C8;_LL30B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp636=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp636->tag != 30)goto _LL30D;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;_tmp639=_tmp636->f3;}}_LL30C:
# 2971
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp637,_tmp638,_tmp639);goto _LL2C8;_LL30D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp63A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp63A->tag != 31)goto _LL30F;else{_tmp63B=(struct _tuple1**)& _tmp63A->f1;_tmp63C=_tmp63A->f2;_tmp63D=_tmp63A->f3;}}_LL30E:
# 2973
*_tmp63B=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp63D))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9E3;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9E2;t=(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=13,((_tmp9E3.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp63C))->name,((_tmp9E3.f2=_tmp63C,_tmp9E3)))))),_tmp9E2))));}goto _LL2C8;_LL30F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp63E=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp63E->tag != 32)goto _LL311;else{_tmp63F=(struct _tuple1**)& _tmp63E->f1;_tmp640=(void*)_tmp63E->f2;_tmp641=_tmp63E->f3;}}_LL310:
# 2976
*_tmp63F=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp641))->name;
t=_tmp640;goto _LL2C8;_LL311: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp642->tag != 33)goto _LL313;else{_tmp643=(int*)&(_tmp642->f1).is_calloc;_tmp644=(struct Cyc_Absyn_Exp**)&(_tmp642->f1).rgn;_tmp645=(void***)&(_tmp642->f1).elt_type;_tmp646=(struct Cyc_Absyn_Exp**)&(_tmp642->f1).num_elts;_tmp647=(int*)&(_tmp642->f1).fat_result;}}_LL312:
# 2979
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp644,_tmp645,_tmp646,_tmp643,_tmp647);goto _LL2C8;_LL313: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp648->tag != 34)goto _LL315;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;}}_LL314:
# 2981
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp649,_tmp64A);goto _LL2C8;_LL315: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp64B->tag != 37)goto _LL317;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL316:
# 2983
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp64C,_tmp64D);goto _LL2C8;_LL317: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp64E->tag != 38)goto _LL319;else{_tmp64F=(void*)_tmp64E->f1;}}_LL318:
# 2985
 if(!te->allow_valueof){
const char*_tmp9E6;void*_tmp9E5;(_tmp9E5=0,Cyc_Tcutil_terr(e->loc,((_tmp9E6="valueof(-) can only occur within types",_tag_dyneither(_tmp9E6,sizeof(char),39))),_tag_dyneither(_tmp9E5,sizeof(void*),0)));}
# 2993
t=Cyc_Absyn_sint_typ;
goto _LL2C8;_LL319: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp5CD;if(_tmp650->tag != 39)goto _LL2C8;}_LL31A:
# 2996
{const char*_tmp9E9;void*_tmp9E8;(_tmp9E8=0,Cyc_Tcutil_terr(e->loc,((_tmp9E9="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmp9E9,sizeof(char),50))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2C8:;}
# 2999
e->topt=t;}
