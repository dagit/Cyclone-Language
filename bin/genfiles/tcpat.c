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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 381
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 922
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 946
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 948
extern void*Cyc_Absyn_sint_typ;
# 950
void*Cyc_Absyn_float_typ(int);
# 952
extern void*Cyc_Absyn_empty_effect;
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1096
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1105
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 69
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 80
int Cyc_Tcutil_coerceable(void*);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 160
int Cyc_Tcutil_unify(void*,void*);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 192
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 240 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 255
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 280
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 53
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 57
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_stmttmp0=p->r;void*_tmp0=_stmttmp0;struct _tuple2*_tmp2;struct _tuple2*_tmp4;struct Cyc_List_List*_tmp5;int _tmp6;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;int _tmpA;enum Cyc_Absyn_AggrKind _tmpC;struct _tuple2*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List**_tmpF;struct Cyc_List_List*_tmp10;int _tmp11;struct Cyc_Absyn_Exp*_tmp13;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;else{_tmp2=_tmp1->f1;}}_LL2:
# 55
{struct _handler_cons _tmp14;_push_handler(& _tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){
{struct _RegionHandle _tmp17=_new_region("r");struct _RegionHandle*r=& _tmp17;_push_region(r);
{void*_stmttmp3=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2,0);void*_tmp18=_stmttmp3;struct Cyc_Absyn_Datatypedecl*_tmp1B;struct Cyc_Absyn_Datatypefield*_tmp1C;struct Cyc_Absyn_Enumdecl*_tmp1E;struct Cyc_Absyn_Enumfield*_tmp1F;void*_tmp21;struct Cyc_Absyn_Enumfield*_tmp22;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp19=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp19->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp37B;void*_tmp37A;(_tmp37A=0,Cyc_Tcutil_terr(p->loc,((_tmp37B="struct tag used without arguments in pattern",_tag_dyneither(_tmp37B,sizeof(char),45))),_tag_dyneither(_tmp37A,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp37E;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp37D;p->r=(void*)((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37E.tag=7,((_tmp37E.f1=_tmp1B,((_tmp37E.f2=_tmp1C,((_tmp37E.f3=0,((_tmp37E.f4=0,_tmp37E)))))))))),_tmp37D))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp381;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp380;p->r=(void*)((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380[0]=((_tmp381.tag=12,((_tmp381.f1=_tmp1E,((_tmp381.f2=_tmp1F,_tmp381)))))),_tmp380))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp384;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp383;p->r=(void*)((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383[0]=((_tmp384.tag=13,((_tmp384.f1=_tmp21,((_tmp384.f2=_tmp22,_tmp384)))))),_tmp383))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp23=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp23->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp2D=_tmp15;void*_tmp2F;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 73
 goto _LL18;_LL1B: _tmp2F=_tmp2D;_LL1C:(void)_throw(_tmp2F);_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _stmttmp4=(*_tmp2).f1;union Cyc_Absyn_Nmspace _tmp30=_stmttmp4;_LL1E: if((_tmp30.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp30.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL22;_LL21:
# 78
(*_tmp2).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp387;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp386;p->r=(void*)((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386[0]=((_tmp387.tag=1,((_tmp387.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp387.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp387)))))),_tmp386))));}
return;_LL22:;_LL23:
{const char*_tmp38A;void*_tmp389;(_tmp389=0,Cyc_Tcutil_terr(p->loc,((_tmp38A="qualified variable in pattern",_tag_dyneither(_tmp38A,sizeof(char),30))),_tag_dyneither(_tmp389,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 85
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_stmttmp2=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);void*_tmp39=_stmttmp2;struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp38D;struct Cyc_List_List*_tmp38C;_tmp42=((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C->hd=((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D->f1=0,((_tmp38D->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp38D)))))),((_tmp38C->tl=_tmp42,_tmp38C))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp397;struct Cyc_Absyn_Aggrdecl**_tmp396;struct Cyc_Absyn_AggrInfo*_tmp395;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp394;p->r=(void*)((_tmp394=_cycalloc(sizeof(*_tmp394)),((_tmp394[0]=((_tmp397.tag=6,((_tmp397.f1=((_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395->aggr_info=Cyc_Absyn_KnownAggr(((_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396[0]=_tmp3B,_tmp396))))),((_tmp395->targs=0,_tmp395)))))),((_tmp397.f2=0,((_tmp397.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp397.f4=_tmp6,_tmp397)))))))))),_tmp394))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp39A;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp399;p->r=(void*)((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39A.tag=7,((_tmp39A.f1=_tmp3D,((_tmp39A.f2=_tmp3E,((_tmp39A.f3=_tmp5,((_tmp39A.f4=_tmp6,_tmp39A)))))))))),_tmp399))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp39D;void*_tmp39C;(_tmp39C=0,Cyc_Tcutil_terr(p->loc,((_tmp39D="enum tag used with arguments in pattern",_tag_dyneither(_tmp39D,sizeof(char),40))),_tag_dyneither(_tmp39C,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp3A1;void*_tmp3A0[1];struct Cyc_String_pa_PrintArg_struct _tmp39F;(_tmp39F.tag=0,((_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3A0[0]=& _tmp39F,Cyc_Tcutil_terr(p->loc,((_tmp3A1="%s is not a constructor in pattern",_tag_dyneither(_tmp3A1,sizeof(char),35))),_tag_dyneither(_tmp3A0,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp3A4;void*_tmp3A3;(_tmp3A3=0,Cyc_Tcutil_terr(p->loc,((_tmp3A4="cannot determine pattern type",_tag_dyneither(_tmp3A4,sizeof(char),30))),_tag_dyneither(_tmp3A3,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3AA;struct Cyc_Absyn_AggrInfo*_tmp3A9;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A8;p->r=(void*)((_tmp3A8=_cycalloc(sizeof(*_tmp3A8)),((_tmp3A8[0]=((_tmp3AA.tag=6,((_tmp3AA.f1=((_tmp3A9=_cycalloc(sizeof(*_tmp3A9)),((_tmp3A9[0]=_tmp58,_tmp3A9)))),((_tmp3AA.f2=_tmp8,((_tmp3AA.f3=_tmp9,((_tmp3AA.f4=_tmpA,_tmp3AA)))))))))),_tmp3A8))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp3AE;void*_tmp3AD[1];struct Cyc_String_pa_PrintArg_struct _tmp3AC;(_tmp3AC.tag=0,((_tmp3AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3AD[0]=& _tmp3AC,Cyc_Tcutil_terr(p->loc,((_tmp3AE="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3AE,sizeof(char),45))),_tag_dyneither(_tmp3AD,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 126
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp3B6;void*_tmp3B5[1];const char*_tmp3B4;const char*_tmp3B3;struct Cyc_String_pa_PrintArg_struct _tmp3B2;(_tmp3B2.tag=0,((_tmp3B2.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp3B3="struct",_tag_dyneither(_tmp3B3,sizeof(char),7)):((_tmp3B4="union",_tag_dyneither(_tmp3B4,sizeof(char),6)))),((_tmp3B5[0]=& _tmp3B2,Cyc_Tcutil_terr(p->loc,((_tmp3B6="can't destructure an abstract %s",_tag_dyneither(_tmp3B6,sizeof(char),33))),_tag_dyneither(_tmp3B5,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp3B9;void*_tmp3B8;(_tmp3B8=0,Cyc_Tcutil_terr(p->loc,((_tmp3B9="too many existentially bound type variables in pattern",_tag_dyneither(_tmp3B9,sizeof(char),55))),_tag_dyneither(_tmp3B8,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3BF;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3BE;struct Cyc_List_List*_tmp3BD;_tmp6C=((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=((_tmp3BE.tag=1,((_tmp3BE.f1=0,_tmp3BE)))),_tmp3BF))))),((_tmp3BD->tl=_tmp6C,_tmp3BD))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3C5;struct Cyc_Absyn_AggrInfo*_tmp3C4;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3C3;p->r=(void*)((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C5.tag=6,((_tmp3C5.f1=((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3C4->targs=_tmpE,_tmp3C4)))))),((_tmp3C5.f2=*_tmpF,((_tmp3C5.f3=_tmp10,((_tmp3C5.f4=_tmp11,_tmp3C5)))))))))),_tmp3C3))));};};}
# 127
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3C8;void*_tmp3C7;(_tmp3C7=0,Cyc_Tcutil_terr(p->loc,((_tmp3C8="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3C8,sizeof(char),43))),_tag_dyneither(_tmp3C7,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3CB;void*_tmp3CA;(_tmp3CA=0,Cyc_Tcutil_terr(p->loc,((_tmp3CB="non-constant expression in case pattern",_tag_dyneither(_tmp3CB,sizeof(char),40))),_tag_dyneither(_tmp3CA,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _stmttmp1=Cyc_Evexp_eval_const_uint_exp(_tmp13);unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=_stmttmp1;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3CE;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3CD;p->r=(void*)((_tmp3CD=_cycalloc_atomic(sizeof(*_tmp3CD)),((_tmp3CD[0]=((_tmp3CE.tag=9,((_tmp3CE.f1=Cyc_Absyn_None,((_tmp3CE.f2=(int)_tmp7C,_tmp3CE)))))),_tmp3CD))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3CF;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->v=s,_tmp3CF)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_stmttmp5=Cyc_Tcutil_compress(numt);void*_tmp81=_stmttmp5;_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp81;if(_tmp82->tag != 13)goto _LL41;}_LL40:
 goto _LL42;_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp81;if(_tmp83->tag != 14)goto _LL43;}_LL42:
 if(topt != 0)return*topt;goto _LL3E;_LL43:;_LL44:
 goto _LL3E;_LL3E:;}
# 190
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 195
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}{
# 199
struct _tuple16*_tmp3D2;struct Cyc_List_List*_tmp3D1;*v_result_ptr=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->hd=((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->f1=vd,((_tmp3D2->f2=e,_tmp3D2)))))),((_tmp3D1->tl=*v_result_ptr,_tmp3D1))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Tcpat_TcPatResult _tmp86=res1;_tmp87=_tmp86.tvars_and_bounds_opt;_tmp88=_tmp86.patvars;{
struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res2;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;
if(_tmp87 != 0  || _tmp8A != 0){
if(_tmp87 == 0){
struct _tuple1*_tmp3D3;_tmp87=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->f1=0,((_tmp3D3->f2=0,_tmp3D3))))));}
if(_tmp8A == 0){
struct _tuple1*_tmp3D4;_tmp8A=((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4->f1=0,((_tmp3D4->f2=0,_tmp3D4))))));}{
struct _tuple1*_tmp3D7;struct Cyc_Tcpat_TcPatResult _tmp3D6;return(_tmp3D6.tvars_and_bounds_opt=((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f1,(*_tmp8A).f1),((_tmp3D7->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f2,(*_tmp8A).f2),_tmp3D7)))))),((_tmp3D6.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3D6)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp3D8;return(_tmp3D8.tvars_and_bounds_opt=0,((_tmp3D8.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3D8)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3D9;return(_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->loc=loc,((_tmp3D9->topt=0,((_tmp3D9->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3D9)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp3DA;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3DA))))): rgn_opt): t;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 229
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 232
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3DB;struct Cyc_Tcpat_TcPatResult res=(_tmp3DB.tvars_and_bounds_opt=0,((_tmp3DB.patvars=0,_tmp3DB)));
# 237
{void*_stmttmp6=p->r;void*_tmp93=_stmttmp6;struct Cyc_Absyn_Vardecl*_tmp96;struct Cyc_Absyn_Pat*_tmp97;struct Cyc_Absyn_Vardecl*_tmp99;struct Cyc_Absyn_Pat*_tmp9A;struct Cyc_Absyn_Tvar*_tmp9C;struct Cyc_Absyn_Vardecl*_tmp9D;int _tmpA3;struct Cyc_Absyn_Enumdecl*_tmpA5;void*_tmpA7;struct Cyc_Absyn_Pat*_tmpAA;struct Cyc_List_List**_tmpAC;int _tmpAD;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct Cyc_List_List**_tmpB0;struct Cyc_List_List*_tmpB1;struct Cyc_List_List**_tmpB2;int _tmpB3;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_Absyn_Datatypefield*_tmpB6;struct Cyc_List_List**_tmpB7;int _tmpB8;_LL46: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp94=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp94->tag != 0)goto _LL48;}_LL47:
# 240
 if(topt != 0)
t=*topt;else{
# 243
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL45;_LL48: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp95=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp95->tag != 1)goto _LL4A;else{_tmp96=_tmp95->f1;_tmp97=_tmp95->f2;}}_LL49:
# 246
 res=Cyc_Tcpat_tcPatRec(te,_tmp97,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp97->topt);
# 250
{void*_stmttmpD=Cyc_Tcutil_compress(t);void*_tmpBE=_stmttmpD;_LL71: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBE;if(_tmpBF->tag != 8)goto _LL73;}_LL72:
# 252
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp3DE;void*_tmp3DD;(_tmp3DD=0,Cyc_Tcutil_terr(p->loc,((_tmp3DE="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3DE,sizeof(char),58))),_tag_dyneither(_tmp3DD,sizeof(void*),0)));}
goto _LL70;_LL73:;_LL74:
# 256
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3E1;void*_tmp3E0;(_tmp3E0=0,Cyc_Tcutil_terr(p->loc,((_tmp3E1="pattern would point to an abstract member",_tag_dyneither(_tmp3E1,sizeof(char),42))),_tag_dyneither(_tmp3E0,sizeof(void*),0)));}
goto _LL70;_LL70:;}
# 260
{struct Cyc_Absyn_Vardecl**_tmp3E2;Cyc_Tcpat_set_vd(((_tmp3E2=_cycalloc(sizeof(*_tmp3E2)),((_tmp3E2[0]=_tmp96,_tmp3E2)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL45;_LL4A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp98=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp98->tag != 2)goto _LL4C;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}_LL4B:
# 263
 res=Cyc_Tcpat_tcPatRec(te,_tmp9A,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9A->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(p->loc,((_tmp3E5="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3E5,sizeof(char),55))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}
goto _LL45;}else{
# 270
struct _RegionHandle _tmpC7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpC7;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp3E8;void*_tmp3E7;(_tmp3E7=0,Cyc_Tcutil_terr(p->loc,((_tmp3E8="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3E8,sizeof(char),56))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}
# 271
;_pop_region(rgn);}{
# 276
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3EE;struct Cyc_Absyn_PtrInfo _tmp3ED;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3EC;void*t2=(void*)((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3EE.tag=5,((_tmp3EE.f1=((_tmp3ED.elt_typ=t,((_tmp3ED.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3ED.ptr_atts=(
((_tmp3ED.ptr_atts).rgn=rgn_pat,(((_tmp3ED.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3ED.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3ED.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3ED.ptr_atts).ptrloc=0,_tmp3ED.ptr_atts)))))))))),_tmp3ED)))))),_tmp3EE)))),_tmp3EC))));
# 281
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 285
{struct Cyc_Absyn_Vardecl**_tmp3EF;Cyc_Tcpat_set_vd(((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=_tmp99,_tmp3EF)))),new_access_exp,& res.patvars,t2);}
goto _LL45;};_LL4C: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp9B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp9B->tag != 3)goto _LL4E;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL4D:
# 291
{struct Cyc_Absyn_Vardecl**_tmp3F0;Cyc_Tcpat_set_vd(((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0[0]=_tmp9D,_tmp3F0)))),access_exp,& res.patvars,_tmp9D->type);}{
# 295
struct _RegionHandle _tmpCF=_new_region("r2");struct _RegionHandle*r2=& _tmpCF;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3F1[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3F1[0]=_tmp9C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F1,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 298
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp3F2;res.tvars_and_bounds_opt=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2->f1=0,((_tmp3F2->f2=0,_tmp3F2))))));}
{struct Cyc_List_List*_tmp3F3;(*res.tvars_and_bounds_opt).f1=(
(_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3->hd=_tmp9C,((_tmp3F3->tl=(*res.tvars_and_bounds_opt).f1,_tmp3F3))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL45;
# 295
;_pop_region(r2);};_LL4E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp9E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp9E->tag != 9)goto _LL50;else{if(_tmp9E->f1 != Cyc_Absyn_Unsigned)goto _LL50;}}_LL4F:
# 305
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL45;_LL50: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp9F=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmp9F->tag != 9)goto _LL52;else{if(_tmp9F->f1 != Cyc_Absyn_None)goto _LL52;}}_LL51:
 goto _LL53;_LL52: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA0=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA0->tag != 9)goto _LL54;else{if(_tmpA0->f1 != Cyc_Absyn_Signed)goto _LL54;}}_LL53:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL45;_LL54: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpA1=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA1->tag != 10)goto _LL56;}_LL55:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL45;_LL56: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpA2=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA2->tag != 11)goto _LL58;else{_tmpA3=_tmpA2->f2;}}_LL57:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpA3));goto _LL45;_LL58: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpA4=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA4->tag != 12)goto _LL5A;else{_tmpA5=_tmpA4->f1;}}_LL59:
# 311
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3F6;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F5;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3F5=_cycalloc(sizeof(*_tmp3F5)),((_tmp3F5[0]=((_tmp3F6.tag=13,((_tmp3F6.f1=_tmpA5->name,((_tmp3F6.f2=_tmpA5,_tmp3F6)))))),_tmp3F5)))));}
goto _LL45;_LL5A: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA6->tag != 13)goto _LL5C;else{_tmpA7=(void*)_tmpA6->f1;}}_LL5B:
 t=Cyc_Tcpat_num_type(topt,_tmpA7);goto _LL45;_LL5C: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA8->tag != 8)goto _LL5E;}_LL5D:
# 315
 if(topt != 0){
void*_stmttmpC=Cyc_Tcutil_compress(*topt);void*_tmpD5=_stmttmpC;_LL76: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD5;if(_tmpD6->tag != 5)goto _LL78;}_LL77:
 t=*topt;goto tcpat_end;_LL78:;_LL79:
 goto _LL75;_LL75:;}{
# 320
struct Cyc_Core_Opt*_tmpD7=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3FC;struct Cyc_Absyn_PtrInfo _tmp3FB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FA;t=(void*)((_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA[0]=((_tmp3FC.tag=5,((_tmp3FC.f1=((_tmp3FB.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpD7),((_tmp3FB.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3FB.ptr_atts=(
((_tmp3FB.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpD7),(((_tmp3FB.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp3FB.ptr_atts).bounds=
# 325
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3FB.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3FB.ptr_atts).ptrloc=0,_tmp3FB.ptr_atts)))))))))),_tmp3FB)))))),_tmp3FC)))),_tmp3FA))));}
# 328
goto _LL45;};_LL5E: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpA9->tag != 5)goto _LL60;else{_tmpAA=_tmpA9->f1;}}_LL5F: {
# 333
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpDB=0;
int elt_const=0;
if(topt != 0){
void*_stmttmp9=Cyc_Tcutil_compress(*topt);void*_tmpDC=_stmttmp9;void*_tmpDE;struct Cyc_Absyn_Tqual _tmpDF;_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDC;if(_tmpDD->tag != 5)goto _LL7D;else{_tmpDE=(_tmpDD->f1).elt_typ;_tmpDF=(_tmpDD->f1).elt_tq;}}_LL7C:
# 339
 inner_typ=_tmpDE;
_tmpDB=& inner_typ;
elt_const=_tmpDF.real_const;
goto _LL7A;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 348
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpAA,_tmpDB,ptr_rgn,1,new_access_exp));
# 357
{void*_stmttmpA=Cyc_Tcutil_compress((void*)_check_null(_tmpAA->topt));void*_tmpE0=_stmttmpA;struct Cyc_Absyn_Datatypedecl*_tmpE2;struct Cyc_Absyn_Datatypefield*_tmpE3;struct Cyc_List_List*_tmpE4;_LL80: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE0;if(_tmpE1->tag != 4)goto _LL82;else{if((((_tmpE1->f1).field_info).KnownDatatypefield).tag != 2)goto _LL82;_tmpE2=((struct _tuple3)(((_tmpE1->f1).field_info).KnownDatatypefield).val).f1;_tmpE3=((struct _tuple3)(((_tmpE1->f1).field_info).KnownDatatypefield).val).f2;_tmpE4=(_tmpE1->f1).targs;}}_LL81:
# 361
{void*_stmttmpB=Cyc_Tcutil_compress(inner_typ);void*_tmpE5=_stmttmpB;_LL85: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE5;if(_tmpE6->tag != 4)goto _LL87;}_LL86:
# 363
 goto DONT_PROMOTE;_LL87:;_LL88:
 goto _LL84;_LL84:;}{
# 367
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp406;struct Cyc_Absyn_Datatypedecl**_tmp405;struct Cyc_Absyn_DatatypeInfo _tmp404;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp403;void*new_type=(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp406.tag=3,((_tmp406.f1=((_tmp404.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405[0]=_tmpE2,_tmp405))))),((_tmp404.targs=_tmpE4,_tmp404)))),_tmp406)))),_tmp403))));
# 370
_tmpAA->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp40C;struct Cyc_Absyn_PtrInfo _tmp40B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40A;t=(void*)((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A[0]=((_tmp40C.tag=5,((_tmp40C.f1=((_tmp40B.elt_typ=new_type,((_tmp40B.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp40B.ptr_atts=(
((_tmp40B.ptr_atts).rgn=ptr_rgn,(((_tmp40B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40B.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp40B.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp40B.ptr_atts).ptrloc=0,_tmp40B.ptr_atts)))))))))),_tmp40B)))))),_tmp40C)))),_tmp40A))));}
# 377
goto _LL7F;};_LL82:;_LL83:
# 379
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp412;struct Cyc_Absyn_PtrInfo _tmp411;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp410;t=(void*)((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410[0]=((_tmp412.tag=5,((_tmp412.f1=((_tmp411.elt_typ=(void*)_check_null(_tmpAA->topt),((_tmp411.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp411.ptr_atts=(
((_tmp411.ptr_atts).rgn=ptr_rgn,(((_tmp411.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp411.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp411.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp411.ptr_atts).ptrloc=0,_tmp411.ptr_atts)))))))))),_tmp411)))))),_tmp412)))),_tmp410))));}_LL7F:;}
# 387
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpAA->topt;
goto _LL45;};}_LL60: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpAB->tag != 4)goto _LL62;else{_tmpAC=(struct Cyc_List_List**)& _tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL61: {
# 391
struct Cyc_List_List*_tmpF1=*_tmpAC;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_stmttmp8=Cyc_Tcutil_compress(*topt);void*_tmpF2=_stmttmp8;struct Cyc_List_List*_tmpF4;_LL8A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF2;if(_tmpF3->tag != 10)goto _LL8C;else{_tmpF4=_tmpF3->f1;}}_LL8B:
# 397
 topt_ts=_tmpF4;
if(_tmpAD){
# 400
int _tmpF5=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF1);
int _tmpF6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF4);
if(_tmpF5 < _tmpF6){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpF6 - _tmpF5;++ i){
struct Cyc_List_List*_tmp413;wild_ps=((_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp413->tl=wild_ps,_tmp413))))));}}
*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF1,wild_ps);
_tmpF1=*_tmpAC;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF1)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF4)){
const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_Tcutil_warn(p->loc,((_tmp416="unnecessary ... in tuple pattern",_tag_dyneither(_tmp416,sizeof(char),33))),_tag_dyneither(_tmp415,sizeof(void*),0)));}}}
# 412
goto _LL89;_LL8C:;_LL8D:
# 414
 goto _LL89;_LL89:;}else{
# 416
if(_tmpAD){
const char*_tmp419;void*_tmp418;(_tmp418=0,Cyc_Tcutil_terr(p->loc,((_tmp419="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp419,sizeof(char),57))),_tag_dyneither(_tmp418,sizeof(void*),0)));}}
{int i=0;for(0;_tmpF1 != 0;(_tmpF1=_tmpF1->tl,i ++)){
void**_tmpFC=0;
if(topt_ts != 0){
_tmpFC=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 424
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp41C;union Cyc_Absyn_Cnst _tmp41B;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp41B.Int_c).val=((_tmp41C.f1=Cyc_Absyn_Unsigned,((_tmp41C.f2=i,_tmp41C)))),(((_tmp41B.Int_c).tag=5,_tmp41B)))),0),0);}
# 429
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpF1->hd,_tmpFC,rgn_pat,allow_ref_pat,new_access_exp));
# 432
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpF1->hd)->topt;{
struct _tuple17*_tmp41F;struct Cyc_List_List*_tmp41E;pat_ts=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E->hd=((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F->f1=Cyc_Absyn_empty_tqual(0),((_tmp41F->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpF1->hd)->topt),_tmp41F)))))),((_tmp41E->tl=pat_ts,_tmp41E))))));};};}}
# 435
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp422;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp421;t=(void*)((_tmp421=_cycalloc(sizeof(*_tmp421)),((_tmp421[0]=((_tmp422.tag=10,((_tmp422.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp422)))),_tmp421))));}
goto _LL45;}_LL62: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpAE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpAE->tag != 6)goto _LL64;else{if(_tmpAE->f1 == 0)goto _LL64;if((((_tmpAE->f1)->aggr_info).KnownAggr).tag != 2)goto _LL64;_tmpAF=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpAE->f1)->aggr_info).KnownAggr).val);_tmpB0=(struct Cyc_List_List**)&(_tmpAE->f1)->targs;_tmpB1=_tmpAE->f2;_tmpB2=(struct Cyc_List_List**)& _tmpAE->f3;_tmpB3=_tmpAE->f4;}}_LL63: {
# 439
struct Cyc_List_List*_tmp103=*_tmpB2;
const char*_tmp424;const char*_tmp423;struct _dyneither_ptr aggr_str=_tmpAF->kind == Cyc_Absyn_StructA?(_tmp424="struct",_tag_dyneither(_tmp424,sizeof(char),7)):((_tmp423="union",_tag_dyneither(_tmp423,sizeof(char),6)));
if(_tmpAF->impl == 0){
{const char*_tmp428;void*_tmp427[1];struct Cyc_String_pa_PrintArg_struct _tmp426;(_tmp426.tag=0,((_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp427[0]=& _tmp426,Cyc_Tcutil_terr(p->loc,((_tmp428="can't destructure an abstract %s",_tag_dyneither(_tmp428,sizeof(char),33))),_tag_dyneither(_tmp427,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL45;}
# 448
if(_tmpAF->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->tagged)
allow_ref_pat=0;
if(_tmpB1 != 0){
# 454
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 457
struct _RegionHandle _tmp107=_new_region("rgn");struct _RegionHandle*rgn=& _tmp107;_push_region(rgn);{
# 459
struct Cyc_List_List*_tmp108=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp109=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpB1;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp109))->hd;
_tmp109=_tmp109->tl;{
void*_tmp10A=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp10B=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp10C=_tmp10B;struct Cyc_Absyn_Kind*_tmp10E;struct Cyc_Absyn_Kind*_tmp110;_LL8F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10C;if(_tmp10D->tag != 2)goto _LL91;else{_tmp10E=_tmp10D->f2;}}_LL90:
 _tmp110=_tmp10E;goto _LL92;_LL91: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp10F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp10C;if(_tmp10F->tag != 0)goto _LL93;else{_tmp110=_tmp10F->f1;}}_LL92:
 k2=_tmp110;goto _LL8E;_LL93:;_LL94: {
const char*_tmp42B;void*_tmp42A;(_tmp42A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp42B="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp42B,sizeof(char),53))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}_LL8E:;}
# 475
{void*_tmp113=_tmp10A;struct Cyc_Absyn_Kind*_tmp115;struct Cyc_Core_Opt**_tmp117;struct Cyc_Absyn_Kind*_tmp118;struct Cyc_Core_Opt**_tmp11A;_LL96: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp114=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp113;if(_tmp114->tag != 0)goto _LL98;else{_tmp115=_tmp114->f1;}}_LL97:
# 478
 if(!Cyc_Tcutil_kind_leq(k2,_tmp115))
error=1;
goto _LL95;_LL98: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp113;if(_tmp116->tag != 2)goto _LL9A;else{_tmp117=(struct Cyc_Core_Opt**)& _tmp116->f1;_tmp118=_tmp116->f2;}}_LL99:
 _tmp11A=_tmp117;goto _LL9B;_LL9A: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp119=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp113;if(_tmp119->tag != 1)goto _LL95;else{_tmp11A=(struct Cyc_Core_Opt**)& _tmp119->f1;}}_LL9B:
{struct Cyc_Core_Opt*_tmp42C;*_tmp11A=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C->v=_tmp10B,_tmp42C))));}goto _LL95;_LL95:;}
# 484
if(error){
const char*_tmp432;void*_tmp431[3];struct Cyc_String_pa_PrintArg_struct _tmp430;struct Cyc_String_pa_PrintArg_struct _tmp42F;struct Cyc_String_pa_PrintArg_struct _tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 488
Cyc_Absynpp_kind2string(k2)),((_tmp42F.tag=0,((_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp10A)),((_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp431[0]=& _tmp430,((_tmp431[1]=& _tmp42F,((_tmp431[2]=& _tmp42E,Cyc_Tcutil_terr(p->loc,((_tmp432="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp432,sizeof(char),60))),_tag_dyneither(_tmp431,sizeof(void*),3)))))))))))))))))));}{
# 490
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp435;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp434;void*vartype=(void*)((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=((_tmp435.tag=2,((_tmp435.f1=tv,_tmp435)))),_tmp434))));
{struct Cyc_List_List*_tmp436;_tmp108=((_tmp436=_region_malloc(rgn,sizeof(*_tmp436)),((_tmp436->hd=vartype,((_tmp436->tl=_tmp108,_tmp436))))));}
# 494
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp439;struct Cyc_List_List*_tmp438;outlives_constraints=(
(_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->hd=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439->f1=Cyc_Absyn_empty_effect,((_tmp439->f2=vartype,_tmp439)))))),((_tmp438->tl=outlives_constraints,_tmp438))))));}else{
# 499
const char*_tmp43C;void*_tmp43B;(_tmp43B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp43C="opened existential had unique or top region kind",_tag_dyneither(_tmp43C,sizeof(char),49))),_tag_dyneither(_tmp43B,sizeof(void*),0)));}}};};}}
# 503
_tmp108=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp108);{
# 505
struct _RegionHandle _tmp128=_new_region("r2");struct _RegionHandle*r2=& _tmp128;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpB1);
# 508
struct Cyc_List_List*_tmp129=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp43D;struct _tuple12 _tmp12A=(_tmp43D.f1=_tmp129,((_tmp43D.f2=rgn,_tmp43D)));
struct Cyc_List_List*_tmp12B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp12A,_tmpAF->tvs);
struct Cyc_List_List*_tmp12C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->exist_vars,_tmp108);
struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp12B);
struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp12C);
struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp12B,_tmp12C);
# 516
if(_tmpB1 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp43E;res.tvars_and_bounds_opt=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E->f1=0,((_tmp43E->f2=0,_tmp43E))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpB1);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp131=0;
{struct Cyc_List_List*_tmp132=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->rgn_po;for(0;_tmp132 != 0;_tmp132=_tmp132->tl){
struct _tuple0*_tmp441;struct Cyc_List_List*_tmp440;_tmp131=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->hd=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp12F,(*((struct _tuple0*)_tmp132->hd)).f1),((_tmp441->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp12F,(*((struct _tuple0*)_tmp132->hd)).f2),_tmp441)))))),((_tmp440->tl=_tmp131,_tmp440))))));}}
# 528
_tmp131=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp131);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp131);};}
# 533
*_tmpB0=_tmp12D;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp44B;struct Cyc_Absyn_Aggrdecl**_tmp44A;struct Cyc_Absyn_AggrInfo _tmp449;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp448;t=(void*)((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448[0]=((_tmp44B.tag=11,((_tmp44B.f1=((_tmp449.aggr_info=Cyc_Absyn_KnownAggr(((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=_tmpAF,_tmp44A))))),((_tmp449.targs=*_tmpB0,_tmp449)))),_tmp44B)))),_tmp448))));}
if(_tmpB3  && _tmpAF->kind == Cyc_Absyn_UnionA){
const char*_tmp44E;void*_tmp44D;(_tmp44D=0,Cyc_Tcutil_warn(p->loc,((_tmp44E="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp44E,sizeof(char),43))),_tag_dyneither(_tmp44D,sizeof(void*),0)));}else{
if(_tmpB3){
# 539
int _tmp13B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp103);
int _tmp13C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->fields);
if(_tmp13B < _tmp13C){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp13C - _tmp13B;++ i){
struct _tuple15*_tmp451;struct Cyc_List_List*_tmp450;wild_dps=((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450->hd=((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451->f1=0,((_tmp451->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp451)))))),((_tmp450->tl=wild_dps,_tmp450))))));}}
*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp103,wild_dps);
_tmp103=*_tmpB2;}else{
if(_tmp13B == _tmp13C){
const char*_tmp454;void*_tmp453;(_tmp453=0,Cyc_Tcutil_warn(p->loc,((_tmp454="unnecessary ... in struct pattern",_tag_dyneither(_tmp454,sizeof(char),34))),_tag_dyneither(_tmp453,sizeof(void*),0)));}}}}{
# 550
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp103,_tmpAF->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple18*_stmttmp7=(struct _tuple18*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp142;struct Cyc_Absyn_Pat*_tmp143;struct _tuple18*_tmp141=_stmttmp7;_tmp142=_tmp141->f1;_tmp143=_tmp141->f2;{
void*_tmp144=Cyc_Tcutil_rsubstitute(rgn,_tmp12F,_tmp142->type);
# 556
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp142->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp143,& _tmp144,rgn_pat,allow_ref_pat,new_access_exp));
# 564
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp143->topt),_tmp144)){
const char*_tmp45B;void*_tmp45A[4];struct Cyc_String_pa_PrintArg_struct _tmp459;struct Cyc_String_pa_PrintArg_struct _tmp458;struct Cyc_String_pa_PrintArg_struct _tmp457;struct Cyc_String_pa_PrintArg_struct _tmp456;(_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 567
Cyc_Absynpp_typ2string((void*)_check_null(_tmp143->topt))),((_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 566
Cyc_Absynpp_typ2string(_tmp144)),((_tmp458.tag=0,((_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp459.tag=0,((_tmp459.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp142->name),((_tmp45A[0]=& _tmp459,((_tmp45A[1]=& _tmp458,((_tmp45A[2]=& _tmp457,((_tmp45A[3]=& _tmp456,Cyc_Tcutil_terr(p->loc,((_tmp45B="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp45B,sizeof(char),45))),_tag_dyneither(_tmp45A,sizeof(void*),4)))))))))))))))))))))))));}
# 568
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp143->topt;};}};}
# 506
;_pop_region(r2);};}
# 572
_npop_handler(0);goto _LL45;
# 457
;_pop_region(rgn);};}_LL64: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpB4->tag != 7)goto _LL66;else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;_tmpB7=(struct Cyc_List_List**)& _tmpB4->f3;_tmpB8=_tmpB4->f4;}}_LL65: {
# 575
struct Cyc_List_List*_tmp14E=*_tmpB7;
struct _RegionHandle _tmp14F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp14F;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpB6->typs;
# 579
struct Cyc_List_List*_tmp150=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp45C;struct _tuple12 _tmp151=(_tmp45C.f1=_tmp150,((_tmp45C.f2=rgn,_tmp45C)));
struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp151,_tmpB5->tvs);
struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp152);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp462;struct Cyc_Absyn_DatatypeFieldInfo _tmp461;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp460;t=(void*)((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460[0]=((_tmp462.tag=4,((_tmp462.f1=((_tmp461.field_info=Cyc_Absyn_KnownDatatypefield(_tmpB5,_tmpB6),((_tmp461.targs=_tmp153,_tmp461)))),_tmp462)))),_tmp460))));}
# 585
if(_tmpB8){
# 587
int _tmp157=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14E);
int _tmp158=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp157 < _tmp158){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp158 - _tmp157;++ i){
struct Cyc_List_List*_tmp463;wild_ps=((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp463->tl=wild_ps,_tmp463))))));}}
*_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp14E,wild_ps);
_tmp14E=*_tmpB7;}else{
if(_tmp157 == _tmp158){
const char*_tmp467;void*_tmp466[1];struct Cyc_String_pa_PrintArg_struct _tmp465;(_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB5->name)),((_tmp466[0]=& _tmp465,Cyc_Tcutil_warn(p->loc,((_tmp467="unnecessary ... in datatype field %s",_tag_dyneither(_tmp467,sizeof(char),37))),_tag_dyneither(_tmp466,sizeof(void*),1)))))));}}}
# 599
for(0;_tmp14E != 0  && tqts != 0;(_tmp14E=_tmp14E->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp15D=(struct Cyc_Absyn_Pat*)_tmp14E->hd;
# 603
void*_tmp15E=Cyc_Tcutil_rsubstitute(rgn,_tmp152,(*((struct _tuple17*)tqts->hd)).f2);
# 606
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp15D,& _tmp15E,rgn_pat,allow_ref_pat,0));
# 613
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp15D->topt),_tmp15E)){
const char*_tmp46D;void*_tmp46C[3];struct Cyc_String_pa_PrintArg_struct _tmp46B;struct Cyc_String_pa_PrintArg_struct _tmp46A;struct Cyc_String_pa_PrintArg_struct _tmp469;(_tmp469.tag=0,((_tmp469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 616
Cyc_Absynpp_typ2string((void*)_check_null(_tmp15D->topt))),((_tmp46A.tag=0,((_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 615
Cyc_Absynpp_typ2string(_tmp15E)),((_tmp46B.tag=0,((_tmp46B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB6->name)),((_tmp46C[0]=& _tmp46B,((_tmp46C[1]=& _tmp46A,((_tmp46C[2]=& _tmp469,Cyc_Tcutil_terr(_tmp15D->loc,((_tmp46D="%s expects argument type %s, not %s",_tag_dyneither(_tmp46D,sizeof(char),36))),_tag_dyneither(_tmp46C,sizeof(void*),3)))))))))))))))))));}}
# 618
if(_tmp14E != 0){
const char*_tmp471;void*_tmp470[1];struct Cyc_String_pa_PrintArg_struct _tmp46F;(_tmp46F.tag=0,((_tmp46F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB6->name)),((_tmp470[0]=& _tmp46F,Cyc_Tcutil_terr(p->loc,((_tmp471="too many arguments for datatype constructor %s",_tag_dyneither(_tmp471,sizeof(char),47))),_tag_dyneither(_tmp470,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp475;void*_tmp474[1];struct Cyc_String_pa_PrintArg_struct _tmp473;(_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB6->name)),((_tmp474[0]=& _tmp473,Cyc_Tcutil_terr(p->loc,((_tmp475="too few arguments for datatype constructor %s",_tag_dyneither(_tmp475,sizeof(char),46))),_tag_dyneither(_tmp474,sizeof(void*),1)))))));}}
# 625
_npop_handler(0);goto _LL45;
# 576
;_pop_region(rgn);}_LL66: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpB9->tag != 6)goto _LL68;else{if(_tmpB9->f1 != 0)goto _LL68;}}_LL67:
# 627
 goto _LL69;_LL68: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpBA=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpBA->tag != 6)goto _LL6A;else{if(_tmpBA->f1 == 0)goto _LL6A;if((((_tmpBA->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6A;}}_LL69:
 goto _LL6B;_LL6A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpBB=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpBB->tag != 14)goto _LL6C;}_LL6B:
 goto _LL6D;_LL6C: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpBC=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpBC->tag != 16)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpBD=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp93;if(_tmpBD->tag != 15)goto _LL45;}_LL6F:
# 632
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL45;_LL45:;}
# 634
tcpat_end:
 p->topt=t;
return res;};}
# 639
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 641
struct Cyc_Tcpat_TcPatResult _tmp16C=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 643
struct _RegionHandle _tmp16D=_new_region("r");struct _RegionHandle*r=& _tmp16D;_push_region(r);{
struct _tuple1 _stmttmpE=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp16C.patvars);struct Cyc_List_List*_tmp16F;struct _tuple1 _tmp16E=_stmttmpE;_tmp16F=_tmp16E.f1;{
struct Cyc_List_List*_tmp170=0;
{struct Cyc_List_List*x=_tmp16F;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp476;_tmp170=((_tmp476=_region_malloc(r,sizeof(*_tmp476)),((_tmp476->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp476->tl=_tmp170,_tmp476))))));}}}
{const char*_tmp477;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp170),p->loc,(
(_tmp477="pattern contains a repeated variable",_tag_dyneither(_tmp477,sizeof(char),37))));}{
# 653
struct Cyc_List_List*x=_tmp16C.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmpF=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp174;struct Cyc_Absyn_Exp**_tmp175;struct _tuple16*_tmp173=_stmttmpF;_tmp174=_tmp173->f1;_tmp175=(struct Cyc_Absyn_Exp**)& _tmp173->f2;
if(*_tmp175 != 0  && *_tmp175 != pat_var_exp)
*_tmp175=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp175));}};};}{
# 659
struct Cyc_Tcpat_TcPatResult _tmp176=_tmp16C;_npop_handler(0);return _tmp176;};
# 643
;_pop_region(r);}
# 664
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 668
void*_stmttmp10=p->r;void*_tmp177=_stmttmp10;struct Cyc_Absyn_Pat*_tmp179;struct Cyc_Absyn_AggrInfo*_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;struct Cyc_Absyn_Pat*_tmp184;struct Cyc_Absyn_Vardecl*_tmp186;struct Cyc_Absyn_Pat*_tmp187;_LL9D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp178=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp178->tag != 5)goto _LL9F;else{_tmp179=_tmp178->f1;}}_LL9E: {
# 670
void*_stmttmp14=(void*)_check_null(p->topt);void*_tmp188=_stmttmp14;void*_tmp18A;_LLAC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp188;if(_tmp189->tag != 5)goto _LLAE;else{_tmp18A=((_tmp189->f1).ptr_atts).rgn;}}_LLAD:
# 672
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp18A);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp179,Cyc_Tcutil_is_noalias_region(_tmp18A,0),patvars);
return;_LLAE:;_LLAF: {
const char*_tmp47A;void*_tmp479;(_tmp479=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp47A="check_pat_regions: bad pointer type",_tag_dyneither(_tmp47A,sizeof(char),36))),_tag_dyneither(_tmp479,sizeof(void*),0)));}_LLAB:;}_LL9F: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp17A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp17A->tag != 6)goto _LLA1;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;_tmp17D=_tmp17A->f3;}}_LLA0:
# 678
 for(0;_tmp17D != 0;_tmp17D=_tmp17D->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp17D->hd)).f2,did_noalias_deref,patvars);}
return;_LLA1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp17E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp17E->tag != 7)goto _LLA3;else{_tmp17F=_tmp17E->f3;}}_LLA2:
# 682
 did_noalias_deref=0;_tmp181=_tmp17F;goto _LLA4;_LLA3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp180=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp180->tag != 4)goto _LLA5;else{_tmp181=_tmp180->f1;}}_LLA4:
# 684
 for(0;_tmp181 != 0;_tmp181=_tmp181->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp181->hd,did_noalias_deref,patvars);}
return;_LLA5: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp182=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp182->tag != 2)goto _LLA7;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLA6:
# 688
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp12=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp18E;struct Cyc_Absyn_Exp*_tmp18F;struct _tuple16*_tmp18D=_stmttmp12;_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;
# 694
if((_tmp18E != 0  && *_tmp18E == _tmp183) && _tmp18F != 0){
{void*_stmttmp13=_tmp18F->r;void*_tmp190=_stmttmp13;struct Cyc_Absyn_Exp*_tmp192;_LLB1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp190;if(_tmp191->tag != 14)goto _LLB3;else{_tmp192=_tmp191->f1;}}_LLB2:
# 697
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp192->topt))){
# 699
const char*_tmp47D;void*_tmp47C;(_tmp47C=0,Cyc_Tcutil_terr(p->loc,((_tmp47D="Reference pattern not allowed on unique pointers",_tag_dyneither(_tmp47D,sizeof(char),49))),_tag_dyneither(_tmp47C,sizeof(void*),0)));}
goto _LLB0;_LLB3:;_LLB4: {
# 702
const char*_tmp480;void*_tmp47F;(_tmp47F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp480="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp480,sizeof(char),51))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}_LLB0:;}
# 704
break;}}}
# 707
Cyc_Tcpat_check_pat_regions_rec(te,_tmp184,did_noalias_deref,patvars);
return;_LLA7: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp185=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp177;if(_tmp185->tag != 1)goto _LLA9;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;}}_LLA8:
# 710
{void*_stmttmp11=p->topt;void*_tmp197=_stmttmp11;_LLB6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197;if(_tmp198 == 0)goto _LLB8;else{if(_tmp198->tag != 8)goto _LLB8;}}_LLB7:
# 712
 if(did_noalias_deref){
{const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_Tcutil_terr(p->loc,((_tmp483="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp483,sizeof(char),56))),_tag_dyneither(_tmp482,sizeof(void*),0)));}
return;}
# 716
goto _LLB5;_LLB8:;_LLB9:
 goto _LLB5;_LLB5:;}
# 719
Cyc_Tcpat_check_pat_regions_rec(te,_tmp187,did_noalias_deref,patvars);
return;_LLA9:;_LLAA:
 return;_LL9C:;}
# 736 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 738
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp19B=_new_region("r");struct _RegionHandle*r=& _tmp19B;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp15=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp19D;struct Cyc_Absyn_Exp*_tmp19E;struct _tuple16*_tmp19C=_stmttmp15;_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;
if(_tmp19E != 0){
struct Cyc_Absyn_Exp*_tmp19F=_tmp19E;
# 746
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp19F->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp19F)){
# 749
const char*_tmp490;void*_tmp48F[1];const char*_tmp48E;struct Cyc_String_pa_PrintArg_struct _tmp48D;void*_tmp48C[1];const char*_tmp48B;struct Cyc_String_pa_PrintArg_struct _tmp48A;(_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)(
# 751
_tmp19D != 0?(struct _dyneither_ptr)(
(_tmp48D.tag=0,((_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp19D)->name)),((_tmp48C[0]=& _tmp48D,Cyc_aprintf(((_tmp48B="for variable %s",_tag_dyneither(_tmp48B,sizeof(char),16))),_tag_dyneither(_tmp48C,sizeof(void*),1)))))))):(
(_tmp48E="",_tag_dyneither(_tmp48E,sizeof(char),1)))),((_tmp48F[0]=& _tmp48A,Cyc_Tcutil_terr(p->loc,((_tmp490="Pattern %s dereferences a unique pointer from a non-unique path",_tag_dyneither(_tmp490,sizeof(char),64))),_tag_dyneither(_tmp48F,sizeof(void*),1)))))));}}}}
# 740
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 797 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp491;return((_tmp491.Name_v).val=s,(((_tmp491.Name_v).tag=1,_tmp491)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp492;return((_tmp492.Int_v).val=i,(((_tmp492.Int_v).tag=2,_tmp492)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 807
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 812
typedef void*Cyc_Tcpat_simple_pat_t;
# 815
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _stmttmp16=c1->name;union Cyc_Tcpat_Name_value _tmp1A9=_stmttmp16;struct _dyneither_ptr _tmp1AA;int _tmp1AB;_LLBB: if((_tmp1A9.Name_v).tag != 1)goto _LLBD;_tmp1AA=(struct _dyneither_ptr)(_tmp1A9.Name_v).val;_LLBC: {
# 818
union Cyc_Tcpat_Name_value _stmttmp18=c2->name;union Cyc_Tcpat_Name_value _tmp1AC=_stmttmp18;struct _dyneither_ptr _tmp1AD;_LLC0: if((_tmp1AC.Name_v).tag != 1)goto _LLC2;_tmp1AD=(struct _dyneither_ptr)(_tmp1AC.Name_v).val;_LLC1:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1AA,(struct _dyneither_ptr)_tmp1AD);_LLC2: if((_tmp1AC.Int_v).tag != 2)goto _LLBF;_LLC3:
 return - 1;_LLBF:;}_LLBD: if((_tmp1A9.Int_v).tag != 2)goto _LLBA;_tmp1AB=(int)(_tmp1A9.Int_v).val;_LLBE: {
# 823
union Cyc_Tcpat_Name_value _stmttmp17=c2->name;union Cyc_Tcpat_Name_value _tmp1AE=_stmttmp17;int _tmp1AF;_LLC5: if((_tmp1AE.Name_v).tag != 1)goto _LLC7;_LLC6:
 return 1;_LLC7: if((_tmp1AE.Int_v).tag != 2)goto _LLC4;_tmp1AF=(int)(_tmp1AE.Int_v).val;_LLC8:
 return _tmp1AB - _tmp1AF;_LLC4:;}_LLBA:;}
# 831
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 835
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 839
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp495;struct Cyc_Tcpat_Con_s*_tmp494;return(_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494->name=Cyc_Tcpat_Name_v(((_tmp495="NULL",_tag_dyneither(_tmp495,sizeof(char),5)))),((_tmp494->arity=0,((_tmp494->span=& Cyc_Tcpat_two_opt,((_tmp494->orig_pat=p,_tmp494)))))))));}
# 842
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp498;struct Cyc_Tcpat_Con_s*_tmp497;return(_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497->name=Cyc_Tcpat_Name_v(((_tmp498="&",_tag_dyneither(_tmp498,sizeof(char),2)))),((_tmp497->arity=1,((_tmp497->span=& Cyc_Tcpat_two_opt,((_tmp497->orig_pat=p,_tmp497)))))))));}
# 845
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp49B;struct Cyc_Tcpat_Con_s*_tmp49A;return(_tmp49A=_region_malloc(r,sizeof(*_tmp49A)),((_tmp49A->name=Cyc_Tcpat_Name_v(((_tmp49B="&",_tag_dyneither(_tmp49B,sizeof(char),2)))),((_tmp49A->arity=1,((_tmp49A->span=& Cyc_Tcpat_one_opt,((_tmp49A->orig_pat=p,_tmp49A)))))))));}
# 848
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp49C;return(_tmp49C=_region_malloc(r,sizeof(*_tmp49C)),((_tmp49C->name=Cyc_Tcpat_Int_v(i),((_tmp49C->arity=0,((_tmp49C->span=0,((_tmp49C->orig_pat=p,_tmp49C)))))))));}
# 851
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp49D;return(_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D->name=Cyc_Tcpat_Name_v(f),((_tmp49D->arity=0,((_tmp49D->span=0,((_tmp49D->orig_pat=p,_tmp49D)))))))));}
# 854
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp49E;return(_tmp49E=_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E->name=Cyc_Tcpat_Int_v((int)c),((_tmp49E->arity=0,((_tmp49E->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp49E->orig_pat=p,_tmp49E)))))))));}
# 857
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp4A1;struct Cyc_Tcpat_Con_s*_tmp4A0;return(_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0)),((_tmp4A0->name=Cyc_Tcpat_Name_v(((_tmp4A1="$",_tag_dyneither(_tmp4A1,sizeof(char),2)))),((_tmp4A0->arity=i,((_tmp4A0->span=& Cyc_Tcpat_one_opt,((_tmp4A0->orig_pat=p,_tmp4A0)))))))));}
# 862
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A4;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A3;return(void*)((_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((_tmp4A4.tag=1,((_tmp4A4.f1=Cyc_Tcpat_null_con(r,p),((_tmp4A4.f2=0,_tmp4A4)))))),_tmp4A3))));}
# 865
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A7;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A6;return(void*)((_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A7.tag=1,((_tmp4A7.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp4A7.f2=0,_tmp4A7)))))),_tmp4A6))));}
# 868
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AA;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A9;return(void*)((_tmp4A9=_region_malloc(r,sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AA.tag=1,((_tmp4AA.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp4AA.f2=0,_tmp4AA)))))),_tmp4A9))));}
# 871
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AD;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4AC;return(void*)((_tmp4AC=_region_malloc(r,sizeof(*_tmp4AC)),((_tmp4AC[0]=((_tmp4AD.tag=1,((_tmp4AD.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp4AD.f2=0,_tmp4AD)))))),_tmp4AC))));}
# 874
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4B3;struct Cyc_List_List*_tmp4B2;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4B1;return(void*)((_tmp4B1=_region_malloc(r,sizeof(*_tmp4B1)),((_tmp4B1[0]=((_tmp4B3.tag=1,((_tmp4B3.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4B3.f2=((_tmp4B2=_region_malloc(r,sizeof(*_tmp4B2)),((_tmp4B2->hd=p,((_tmp4B2->tl=0,_tmp4B2)))))),_tmp4B3)))))),_tmp4B1))));}
# 877
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4B9;struct Cyc_List_List*_tmp4B8;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4B7;return(void*)((_tmp4B7=_region_malloc(r,sizeof(*_tmp4B7)),((_tmp4B7[0]=((_tmp4B9.tag=1,((_tmp4B9.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4B9.f2=((_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8)),((_tmp4B8->hd=p,((_tmp4B8->tl=0,_tmp4B8)))))),_tmp4B9)))))),_tmp4B7))));}
# 880
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4BC;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4BB;return(void*)((_tmp4BB=_region_malloc(r,sizeof(*_tmp4BB)),((_tmp4BB[0]=((_tmp4BC.tag=1,((_tmp4BC.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4BC.f2=ss,_tmp4BC)))))),_tmp4BB))));}
# 883
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 885
struct Cyc_Tcpat_Con_s*_tmp4BD;struct Cyc_Tcpat_Con_s*c=(_tmp4BD=_region_malloc(r,sizeof(*_tmp4BD)),((_tmp4BD->name=Cyc_Tcpat_Name_v(con_name),((_tmp4BD->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4BD->span=span,((_tmp4BD->orig_pat=p,_tmp4BD)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4C0;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4BF;return(void*)((_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4C0.tag=1,((_tmp4C0.f1=c,((_tmp4C0.f2=ps,_tmp4C0)))))),_tmp4BF))));}
# 890
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_stmttmp19=p->r;void*_tmp1CE=_stmttmp19;enum Cyc_Absyn_Sign _tmp1D3;int _tmp1D4;char _tmp1D6;struct _dyneither_ptr _tmp1D8;struct Cyc_Absyn_Pat*_tmp1DA;struct Cyc_Absyn_Pat*_tmp1DC;struct Cyc_Absyn_Pat*_tmp1DE;struct Cyc_Absyn_Datatypedecl*_tmp1E0;struct Cyc_Absyn_Datatypefield*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E4;struct Cyc_Absyn_Aggrdecl*_tmp1E6;struct Cyc_List_List*_tmp1E7;struct Cyc_Absyn_Enumdecl*_tmp1E9;struct Cyc_Absyn_Enumfield*_tmp1EA;void*_tmp1EC;struct Cyc_Absyn_Enumfield*_tmp1ED;_LLCA: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1CF=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1CF->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D0->tag != 3)goto _LLCE;}_LLCD:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4C3;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4C2;s=(void*)((_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2[0]=((_tmp4C3.tag=0,_tmp4C3)),_tmp4C2))));}goto _LLC9;_LLCE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D1->tag != 8)goto _LLD0;}_LLCF:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC9;_LLD0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D2=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D2->tag != 9)goto _LLD2;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}_LLD1:
 s=Cyc_Tcpat_int_pat(r,_tmp1D4,p);goto _LLC9;_LLD2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D5->tag != 10)goto _LLD4;else{_tmp1D6=_tmp1D5->f1;}}_LLD3:
 s=Cyc_Tcpat_char_pat(r,_tmp1D6,p);goto _LLC9;_LLD4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1D7=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D7->tag != 11)goto _LLD6;else{_tmp1D8=_tmp1D7->f1;}}_LLD5:
 s=Cyc_Tcpat_float_pat(r,_tmp1D8,p);goto _LLC9;_LLD6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1D9->tag != 1)goto _LLD8;else{_tmp1DA=_tmp1D9->f2;}}_LLD7:
 s=Cyc_Tcpat_compile_pat(r,_tmp1DA);goto _LLC9;_LLD8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1DB=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1DB->tag != 2)goto _LLDA;else{_tmp1DC=_tmp1DB->f2;}}_LLD9:
 s=Cyc_Tcpat_compile_pat(r,_tmp1DC);goto _LLC9;_LLDA: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1DD=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1DD->tag != 5)goto _LLDC;else{_tmp1DE=_tmp1DD->f1;}}_LLDB:
# 902
{void*_stmttmp1F=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F4=_stmttmp1F;union Cyc_Absyn_Constraint*_tmp1F6;_LLEF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1F5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F4;if(_tmp1F5->tag != 5)goto _LLF1;else{_tmp1F6=((_tmp1F5->f1).ptr_atts).nullable;}}_LLF0: {
# 904
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1F7=_tmp1F6;union Cyc_Absyn_Constraint*_tmp1F8;int _tmp1F9;_LLF4: if((_tmp1F7->Forward_constr).tag != 2)goto _LLF6;_tmp1F8=(union Cyc_Absyn_Constraint*)(_tmp1F7->Forward_constr).val;_LLF5:
# 909
*_tmp1F6=*_tmp1F8;
continue;_LLF6: if((_tmp1F7->No_constr).tag != 3)goto _LLF8;_LLF7:
# 912
{struct _union_Constraint_Eq_constr*_tmp4C4;(_tmp4C4=& _tmp1F6->Eq_constr,((_tmp4C4->tag=1,_tmp4C4->val=0)));}
is_nullable=0;
still_working=0;
goto _LLF3;_LLF8: if((_tmp1F7->Eq_constr).tag != 1)goto _LLF3;_tmp1F9=(int)(_tmp1F7->Eq_constr).val;_LLF9:
# 917
 is_nullable=_tmp1F9;
still_working=0;
goto _LLF3;_LLF3:;}{
# 922
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1DE);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 926
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLEE;};}_LLF1:;_LLF2: {
const char*_tmp4C7;void*_tmp4C6;(_tmp4C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C7="expecting pointertype for pattern!",_tag_dyneither(_tmp4C7,sizeof(char),35))),_tag_dyneither(_tmp4C6,sizeof(void*),0)));}_LLEE:;}
# 930
goto _LLC9;_LLDC: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1DF->tag != 7)goto _LLDE;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;_tmp1E2=_tmp1DF->f3;}}_LLDD: {
# 932
int*span;
{void*_stmttmp1E=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1FD=_stmttmp1E;_LLFB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1FD;if(_tmp1FE->tag != 3)goto _LLFD;}_LLFC:
# 935
 if(_tmp1E0->is_extensible)
span=0;else{
# 938
int*_tmp4C8;span=((_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8)),((_tmp4C8[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v),_tmp4C8))));}
goto _LLFA;_LLFD: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FD;if(_tmp1FF->tag != 4)goto _LLFF;}_LLFE:
 span=& Cyc_Tcpat_one_opt;goto _LLFA;_LLFF:;_LL100:
{const char*_tmp4CB;void*_tmp4CA;span=((_tmp4CA=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CB="void datatype pattern has bad type",_tag_dyneither(_tmp4CB,sizeof(char),35))),_tag_dyneither(_tmp4CA,sizeof(void*),0))));}goto _LLFA;_LLFA:;}
# 943
s=Cyc_Tcpat_con_pat(r,*(*_tmp1E1->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1E2),p);
goto _LLC9;}_LLDE: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1E3=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1E3->tag != 4)goto _LLE0;else{_tmp1E4=_tmp1E3->f1;}}_LLDF:
# 947
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1E4),p);goto _LLC9;_LLE0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1E5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1E5->tag != 6)goto _LLE2;else{if(_tmp1E5->f1 == 0)goto _LLE2;if((((_tmp1E5->f1)->aggr_info).KnownAggr).tag != 2)goto _LLE2;_tmp1E6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1E5->f1)->aggr_info).KnownAggr).val);_tmp1E7=_tmp1E5->f3;}}_LLE1:
# 952
 if(_tmp1E6->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E6->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 956
const char*_tmp4CC;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4CC="",_tag_dyneither(_tmp4CC,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1E7;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_stmttmp1D=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Pat*_tmp205;struct _tuple15*_tmp203=_stmttmp1D;_tmp204=_tmp203->f1;_tmp205=_tmp203->f2;{
struct Cyc_List_List*_tmp206=_tmp204;struct _dyneither_ptr*_tmp208;_LL102: if(_tmp206 == 0)goto _LL104;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp207=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp206->hd);if(_tmp207->tag != 1)goto _LL104;else{_tmp208=_tmp207->f1;}}if(_tmp206->tl != 0)goto _LL104;_LL103:
# 961
 if(Cyc_strptrcmp(_tmp208,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4CD;ps=((_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD)),((_tmp4CD->hd=Cyc_Tcpat_compile_pat(r,_tmp205),((_tmp4CD->tl=ps,_tmp4CD))))));}
found=1;}
# 965
goto _LL101;_LL104:;_LL105: {
const char*_tmp4D0;void*_tmp4CF;(_tmp4CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D0="bad designator(s)",_tag_dyneither(_tmp4D0,sizeof(char),18))),_tag_dyneither(_tmp4CF,sizeof(void*),0)));}_LL101:;};}}
# 969
if(!found){
const char*_tmp4D3;void*_tmp4D2;(_tmp4D2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D3="bad designator",_tag_dyneither(_tmp4D3,sizeof(char),15))),_tag_dyneither(_tmp4D2,sizeof(void*),0)));}}}
# 972
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 975
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E6->impl))->tagged){
const char*_tmp4D6;void*_tmp4D5;(_tmp4D5=0,Cyc_Tcutil_terr(p->loc,((_tmp4D6="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4D6,sizeof(char),47))),_tag_dyneither(_tmp4D5,sizeof(void*),0)));}{
int*_tmp4D7;int*span=(_tmp4D7=_region_malloc(r,sizeof(*_tmp4D7)),((_tmp4D7[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E6->impl))->fields),_tmp4D7)));
struct Cyc_List_List*_tmp211=_tmp1E7;struct _dyneither_ptr*_tmp213;struct Cyc_Absyn_Pat*_tmp214;_LL107: if(_tmp211 == 0)goto _LL109;if(((struct _tuple15*)_tmp211->hd)->f1 == 0)goto _LL109;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp212=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp211->hd)->f1)->hd);if(_tmp212->tag != 1)goto _LL109;else{_tmp213=_tmp212->f1;}}if((((struct _tuple15*)_tmp211->hd)->f1)->tl != 0)goto _LL109;_tmp214=((struct _tuple15*)_tmp211->hd)->f2;if(_tmp211->tl != 0)goto _LL109;_LL108:
# 980
{struct Cyc_List_List*_tmp4D8;s=Cyc_Tcpat_con_pat(r,*_tmp213,span,((_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8)),((_tmp4D8->hd=Cyc_Tcpat_compile_pat(r,_tmp214),((_tmp4D8->tl=0,_tmp4D8)))))),p);}
goto _LL106;_LL109:;_LL10A: {
const char*_tmp4DB;void*_tmp4DA;(_tmp4DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DB="bad union pattern",_tag_dyneither(_tmp4DB,sizeof(char),18))),_tag_dyneither(_tmp4DA,sizeof(void*),0)));}_LL106:;};}
# 985
goto _LLC9;_LLE2: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1E8=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1E8->tag != 12)goto _LLE4;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;}}_LLE3:
# 990
{void*_stmttmp1C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp219=_stmttmp1C;_LL10C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp219;if(_tmp21A->tag != 6)goto _LL10E;}_LL10D:
# 994
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1EA->name).f2,0,0,p);
goto _LL10B;_LL10E:;_LL10F: {
# 997
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E9->fields))->v);
{int*_tmp4DC;s=Cyc_Tcpat_con_pat(r,*(*_tmp1EA->name).f2,((_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC[0]=span,_tmp4DC)))),0,p);}
goto _LL10B;}_LL10B:;}
# 1001
goto _LLC9;_LLE4: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1EB=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1EB->tag != 13)goto _LLE6;else{_tmp1EC=(void*)_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;}}_LLE5: {
# 1006
struct Cyc_List_List*fields;
{void*_stmttmp1A=Cyc_Tcutil_compress(_tmp1EC);void*_tmp21C=_stmttmp1A;struct Cyc_List_List*_tmp21E;_LL111: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp21C;if(_tmp21D->tag != 14)goto _LL113;else{_tmp21E=_tmp21D->f1;}}_LL112:
 fields=_tmp21E;goto _LL110;_LL113:;_LL114: {
const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DF="bad type in AnonEnum_p",_tag_dyneither(_tmp4DF,sizeof(char),23))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}_LL110:;}
# 1013
{void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp221=_stmttmp1B;_LL116: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp221;if(_tmp222->tag != 6)goto _LL118;}_LL117:
# 1017
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1ED->name).f2,0,0,p);
goto _LL115;_LL118:;_LL119: {
# 1020
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4E0;s=Cyc_Tcpat_con_pat(r,*(*_tmp1ED->name).f2,((_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0)),((_tmp4E0[0]=span,_tmp4E0)))),0,p);}
goto _LL115;}_LL115:;}
# 1024
goto _LLC9;}_LLE6: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1EE=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1EE->tag != 14)goto _LLE8;}_LLE7:
 goto _LLE9;_LLE8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1EF=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1EF->tag != 15)goto _LLEA;}_LLE9:
 goto _LLEB;_LLEA: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1F0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1F0->tag != 6)goto _LLEC;}_LLEB:
 goto _LLED;_LLEC: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1F1=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1CE;if(_tmp1F1->tag != 16)goto _LLC9;}_LLED: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4E3;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4E2;s=(void*)((_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2)),((_tmp4E2[0]=((_tmp4E3.tag=0,_tmp4E3)),_tmp4E2))));}_LLC9:;}
# 1030
return s;}
# 1038
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1049
typedef void*Cyc_Tcpat_term_desc_t;
# 1053
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1060
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1069
typedef void*Cyc_Tcpat_decision_t;struct _tuple19{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1074
typedef struct _tuple19*Cyc_Tcpat_work_stack_frame_t;
# 1077
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1079
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1082
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4E6;void*_tmp4E5;(_tmp4E5=0,Cyc_fprintf(Cyc_stderr,((_tmp4E6=" ",_tag_dyneither(_tmp4E6,sizeof(char),2))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}}
# 1088
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp228=c->name;
union Cyc_Tcpat_Name_value _tmp229=_tmp228;struct _dyneither_ptr _tmp22A;int _tmp22B;_LL11B: if((_tmp229.Name_v).tag != 1)goto _LL11D;_tmp22A=(struct _dyneither_ptr)(_tmp229.Name_v).val;_LL11C:
{const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_String_pa_PrintArg_struct _tmp4E8;(_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22A),((_tmp4E9[0]=& _tmp4E8,Cyc_fprintf(Cyc_stderr,((_tmp4EA="%s",_tag_dyneither(_tmp4EA,sizeof(char),3))),_tag_dyneither(_tmp4E9,sizeof(void*),1)))))));}goto _LL11A;_LL11D: if((_tmp229.Int_v).tag != 2)goto _LL11A;_tmp22B=(int)(_tmp229.Int_v).val;_LL11E:
{const char*_tmp4EE;void*_tmp4ED[1];struct Cyc_Int_pa_PrintArg_struct _tmp4EC;(_tmp4EC.tag=1,((_tmp4EC.f1=(unsigned long)_tmp22B,((_tmp4ED[0]=& _tmp4EC,Cyc_fprintf(Cyc_stderr,((_tmp4EE="%d",_tag_dyneither(_tmp4EE,sizeof(char),3))),_tag_dyneither(_tmp4ED,sizeof(void*),1)))))));}goto _LL11A;_LL11A:;}
# 1096
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp232=d;void*_tmp234;struct Cyc_List_List*_tmp237;struct Cyc_Tcpat_Con_s*_tmp238;void*_tmp239;void*_tmp23A;_LL120: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp233=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp232;if(_tmp233->tag != 1)goto _LL122;else{_tmp234=(void*)_tmp233->f1;}}_LL121:
# 1099
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,Cyc_fprintf(Cyc_stderr,((_tmp4F1="Success(",_tag_dyneither(_tmp4F1,sizeof(char),9))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}print_rhs(_tmp234);{const char*_tmp4F4;void*_tmp4F3;(_tmp4F3=0,Cyc_fprintf(Cyc_stderr,((_tmp4F4=")\n",_tag_dyneither(_tmp4F4,sizeof(char),3))),_tag_dyneither(_tmp4F3,sizeof(void*),0)));}
goto _LL11F;_LL122: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp235=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp232;if(_tmp235->tag != 0)goto _LL124;}_LL123:
{const char*_tmp4F7;void*_tmp4F6;(_tmp4F6=0,Cyc_fprintf(Cyc_stderr,((_tmp4F7="Failure\n",_tag_dyneither(_tmp4F7,sizeof(char),9))),_tag_dyneither(_tmp4F6,sizeof(void*),0)));}goto _LL11F;_LL124: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp236=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp232;if(_tmp236->tag != 2)goto _LL11F;else{_tmp237=_tmp236->f1;_tmp238=_tmp236->f2;_tmp239=(void*)_tmp236->f3;_tmp23A=(void*)_tmp236->f4;}}_LL125:
# 1104
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_fprintf(Cyc_stderr,((_tmp4FA="Access[",_tag_dyneither(_tmp4FA,sizeof(char),8))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
for(0;_tmp237 != 0;_tmp237=_tmp237->tl){
{const char*_tmp4FE;void*_tmp4FD[1];struct Cyc_Int_pa_PrintArg_struct _tmp4FC;(_tmp4FC.tag=1,((_tmp4FC.f1=(unsigned long)((int)_tmp237->hd),((_tmp4FD[0]=& _tmp4FC,Cyc_fprintf(Cyc_stderr,((_tmp4FE="%d",_tag_dyneither(_tmp4FE,sizeof(char),3))),_tag_dyneither(_tmp4FD,sizeof(void*),1)))))));}
if(_tmp237->tl != 0){const char*_tmp501;void*_tmp500;(_tmp500=0,Cyc_fprintf(Cyc_stderr,((_tmp501=",",_tag_dyneither(_tmp501,sizeof(char),2))),_tag_dyneither(_tmp500,sizeof(void*),0)));}}
# 1110
{const char*_tmp504;void*_tmp503;(_tmp503=0,Cyc_fprintf(Cyc_stderr,((_tmp504="],",_tag_dyneither(_tmp504,sizeof(char),3))),_tag_dyneither(_tmp503,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp238);
{const char*_tmp507;void*_tmp506;(_tmp506=0,Cyc_fprintf(Cyc_stderr,((_tmp507=",\n",_tag_dyneither(_tmp507,sizeof(char),3))),_tag_dyneither(_tmp506,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp239,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp23A,print_rhs,tab + 7);_LL11F:;}struct _tuple20{int f1;unsigned int f2;};
# 1118
static void Cyc_Tcpat_print_swrhs(struct _tuple20*x){
return;}
# 1126
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp24C=td;struct Cyc_Set_Set*_tmp24E;_LL127: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp24D=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp24C;if(_tmp24D->tag != 1)goto _LL129;else{_tmp24E=_tmp24D->f1;}}_LL128: {
# 1136
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp50A;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp509;return(void*)((_tmp509=_region_malloc(r,sizeof(*_tmp509)),((_tmp509[0]=((_tmp50A.tag=1,((_tmp50A.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp24E,c),_tmp50A)))),_tmp509))));}_LL129: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp24F=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp24C;if(_tmp24F->tag != 0)goto _LL126;}_LL12A: {
const char*_tmp50D;void*_tmp50C;(_tmp50C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50D="add_neg called when td is Positive",_tag_dyneither(_tmp50D,sizeof(char),35))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}_LL126:;}
# 1143
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp254=td;struct Cyc_Tcpat_Con_s*_tmp256;struct Cyc_Set_Set*_tmp258;_LL12C: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp255=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp254;if(_tmp255->tag != 0)goto _LL12E;else{_tmp256=_tmp255->f1;}}_LL12D:
# 1147
 if(Cyc_Tcpat_compare_con(c,_tmp256)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL12E: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp257=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp254;if(_tmp257->tag != 1)goto _LL12B;else{_tmp258=_tmp257->f1;}}_LL12F:
# 1151
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp258,c))return Cyc_Tcpat_No;else{
# 1154
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp258)+ 1)
return Cyc_Tcpat_Yes;else{
# 1157
return Cyc_Tcpat_Maybe;}}_LL12B:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1165
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1167
struct Cyc_List_List*_tmp259=ctxt;struct Cyc_Tcpat_Con_s*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;_LL131: if(_tmp259 != 0)goto _LL133;_LL132:
 return 0;_LL133: if(_tmp259 == 0)goto _LL130;_tmp25A=((struct _tuple21*)_tmp259->hd)->f1;_tmp25B=((struct _tuple21*)_tmp259->hd)->f2;_tmp25C=_tmp259->tl;_LL134: {
# 1170
struct _tuple21*_tmp513;struct Cyc_List_List*_tmp512;struct Cyc_List_List*_tmp511;return(_tmp511=_region_malloc(r,sizeof(*_tmp511)),((_tmp511->hd=(
(_tmp513=_region_malloc(r,sizeof(*_tmp513)),((_tmp513->f1=_tmp25A,((_tmp513->f2=((_tmp512=_region_malloc(r,sizeof(*_tmp512)),((_tmp512->hd=dsc,((_tmp512->tl=_tmp25B,_tmp512)))))),_tmp513)))))),((_tmp511->tl=_tmp25C,_tmp511)))));}_LL130:;}
# 1178
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp260=ctxt;struct Cyc_Tcpat_Con_s*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp263;_LL136: if(_tmp260 != 0)goto _LL138;_LL137: {
const char*_tmp516;void*_tmp515;(_tmp515=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp516="norm_context: empty context",_tag_dyneither(_tmp516,sizeof(char),28))),_tag_dyneither(_tmp515,sizeof(void*),0)));}_LL138: if(_tmp260 == 0)goto _LL135;_tmp261=((struct _tuple21*)_tmp260->hd)->f1;_tmp262=((struct _tuple21*)_tmp260->hd)->f2;_tmp263=_tmp260->tl;_LL139: {
# 1182
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp519;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp518;return Cyc_Tcpat_augment(r,_tmp263,(void*)((_tmp518=_region_malloc(r,sizeof(*_tmp518)),((_tmp518[0]=((_tmp519.tag=0,((_tmp519.f1=_tmp261,((_tmp519.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp262),_tmp519)))))),_tmp518)))));}_LL135:;}
# 1191
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1193
struct _tuple1 _tmp51A;struct _tuple1 _stmttmp20=(_tmp51A.f1=ctxt,((_tmp51A.f2=work,_tmp51A)));struct _tuple1 _tmp268=_stmttmp20;struct Cyc_Tcpat_Con_s*_tmp269;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26D;_LL13B: if(_tmp268.f1 != 0)goto _LL13D;if(_tmp268.f2 != 0)goto _LL13D;_LL13C:
 return dsc;_LL13D: if(_tmp268.f1 != 0)goto _LL13F;_LL13E:
 goto _LL140;_LL13F: if(_tmp268.f2 != 0)goto _LL141;_LL140: {
const char*_tmp51D;void*_tmp51C;(_tmp51C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51D="build_desc: ctxt and work don't match",_tag_dyneither(_tmp51D,sizeof(char),38))),_tag_dyneither(_tmp51C,sizeof(void*),0)));}_LL141: if(_tmp268.f1 == 0)goto _LL13A;_tmp269=((struct _tuple21*)(_tmp268.f1)->hd)->f1;_tmp26A=((struct _tuple21*)(_tmp268.f1)->hd)->f2;_tmp26B=(_tmp268.f1)->tl;if(_tmp268.f2 == 0)goto _LL13A;_tmp26C=((struct _tuple19*)(_tmp268.f2)->hd)->f3;_tmp26D=(_tmp268.f2)->tl;_LL142: {
# 1198
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp521;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp270=(_tmp521=_region_malloc(r,sizeof(*_tmp521)),((_tmp521[0]=((_tmp523.tag=0,((_tmp523.f1=_tmp269,((_tmp523.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp26A),(
(_tmp522=_region_malloc(r,sizeof(*_tmp522)),((_tmp522->hd=dsc,((_tmp522->tl=_tmp26C,_tmp522))))))),_tmp523)))))),_tmp521)));
return Cyc_Tcpat_build_desc(r,_tmp26B,(void*)_tmp270,_tmp26D);}_LL13A:;}
# 1204
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1211
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1213
struct Cyc_List_List*_tmp275=allmrules;void*_tmp276;void*_tmp277;struct Cyc_List_List*_tmp278;_LL144: if(_tmp275 != 0)goto _LL146;_LL145: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp526;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp525;return(void*)((_tmp525=_cycalloc(sizeof(*_tmp525)),((_tmp525[0]=((_tmp526.tag=0,((_tmp526.f1=dsc,_tmp526)))),_tmp525))));}_LL146: if(_tmp275 == 0)goto _LL143;_tmp276=((struct _tuple0*)_tmp275->hd)->f1;_tmp277=((struct _tuple0*)_tmp275->hd)->f2;_tmp278=_tmp275->tl;_LL147:
# 1216
 return Cyc_Tcpat_match(r,_tmp276,0,dsc,0,0,_tmp277,_tmp278);_LL143:;}
# 1221
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1223
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp529;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp528;return Cyc_Tcpat_or_match(r,(void*)((_tmp528=_region_malloc(r,sizeof(*_tmp528)),((_tmp528[0]=((_tmp529.tag=1,((_tmp529.f1=Cyc_Tcpat_empty_con_set(r),_tmp529)))),_tmp528)))),allmrules);}
# 1229
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1233
struct Cyc_List_List*_tmp27D=work;struct Cyc_List_List*_tmp27E;struct Cyc_List_List*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp281;struct Cyc_List_List*_tmp282;_LL149: if(_tmp27D != 0)goto _LL14B;_LL14A: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp52C;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp52B;return(void*)((_tmp52B=_region_malloc(r,sizeof(*_tmp52B)),((_tmp52B[0]=((_tmp52C.tag=1,((_tmp52C.f1=right_hand_side,_tmp52C)))),_tmp52B))));}_LL14B: if(_tmp27D == 0)goto _LL14D;if(((struct _tuple19*)_tmp27D->hd)->f1 != 0)goto _LL14D;if(((struct _tuple19*)_tmp27D->hd)->f2 != 0)goto _LL14D;if(((struct _tuple19*)_tmp27D->hd)->f3 != 0)goto _LL14D;_tmp27E=_tmp27D->tl;_LL14C:
# 1236
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp27E,right_hand_side,rules);_LL14D: if(_tmp27D == 0)goto _LL148;_tmp27F=((struct _tuple19*)_tmp27D->hd)->f1;_tmp280=((struct _tuple19*)_tmp27D->hd)->f2;_tmp281=((struct _tuple19*)_tmp27D->hd)->f3;_tmp282=_tmp27D->tl;_LL14E:
# 1238
 if((_tmp27F == 0  || _tmp280 == 0) || _tmp281 == 0){
const char*_tmp52F;void*_tmp52E;(_tmp52E=0,Cyc_Tcutil_impos(((_tmp52F="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp52F,sizeof(char),38))),_tag_dyneither(_tmp52E,sizeof(void*),0)));}{
struct Cyc_List_List*_stmttmp21=_tmp27F;void*_tmp288;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp287=_stmttmp21;_tmp288=(void*)_tmp287->hd;_tmp289=_tmp287->tl;{
struct Cyc_List_List*_stmttmp22=_tmp280;struct Cyc_List_List*_tmp28B;struct Cyc_List_List*_tmp28C;struct Cyc_List_List*_tmp28A=_stmttmp22;_tmp28B=(struct Cyc_List_List*)_tmp28A->hd;_tmp28C=_tmp28A->tl;{
struct Cyc_List_List*_stmttmp23=_tmp281;void*_tmp28E;struct Cyc_List_List*_tmp28F;struct Cyc_List_List*_tmp28D=_stmttmp23;_tmp28E=(void*)_tmp28D->hd;_tmp28F=_tmp28D->tl;{
struct _tuple19*_tmp530;struct _tuple19*_tmp290=(_tmp530=_region_malloc(r,sizeof(*_tmp530)),((_tmp530->f1=_tmp289,((_tmp530->f2=_tmp28C,((_tmp530->f3=_tmp28F,_tmp530)))))));
struct Cyc_List_List*_tmp531;return Cyc_Tcpat_match(r,_tmp288,_tmp28B,_tmp28E,ctx,((_tmp531=_region_malloc(r,sizeof(*_tmp531)),((_tmp531->hd=_tmp290,((_tmp531->tl=_tmp282,_tmp531)))))),right_hand_side,rules);};};};};_LL148:;}
# 1250
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1252
void*_tmp293=dsc;struct Cyc_Set_Set*_tmp295;struct Cyc_List_List*_tmp297;_LL150: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp294=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp293;if(_tmp294->tag != 1)goto _LL152;else{_tmp295=_tmp294->f1;}}_LL151: {
# 1257
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp534;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp533;void*any=(void*)((_tmp533=_region_malloc(r,sizeof(*_tmp533)),((_tmp533[0]=((_tmp534.tag=1,((_tmp534.f1=Cyc_Tcpat_empty_con_set(r),_tmp534)))),_tmp533))));
struct Cyc_List_List*_tmp298=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp535;_tmp298=((_tmp535=_region_malloc(r,sizeof(*_tmp535)),((_tmp535->hd=any,((_tmp535->tl=_tmp298,_tmp535))))));}}
return _tmp298;}_LL152: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp296=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp293;if(_tmp296->tag != 0)goto _LL14F;else{_tmp297=_tmp296->f2;}}_LL153:
 return _tmp297;_LL14F:;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1267
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple22*env,int i){
struct _RegionHandle*_tmp29D;struct Cyc_List_List*_tmp29E;struct _tuple22*_tmp29C=env;_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;{
struct Cyc_List_List*_tmp536;return(_tmp536=_region_malloc(_tmp29D,sizeof(*_tmp536)),((_tmp536->hd=(void*)(i + 1),((_tmp536->tl=_tmp29E,_tmp536)))));};}
# 1271
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1273
struct _tuple22 _tmp537;struct _tuple22 _tmp2A0=(_tmp537.f1=r,((_tmp537.f2=obj,_tmp537)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple22*,int),struct _tuple22*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp2A0);}
# 1280
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1284
void*_tmp2A2=p;struct Cyc_Tcpat_Con_s*_tmp2A5;struct Cyc_List_List*_tmp2A6;_LL155: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2A3=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2A2;if(_tmp2A3->tag != 0)goto _LL157;}_LL156:
# 1286
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL157: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2A4=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2A2;if(_tmp2A4->tag != 1)goto _LL154;else{_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A4->f2;}}_LL158: {
# 1288
enum Cyc_Tcpat_Answer _stmttmp24=Cyc_Tcpat_static_match(_tmp2A5,dsc);switch(_stmttmp24){case Cyc_Tcpat_Yes: _LL159: {
# 1290
struct _tuple21*_tmp53A;struct Cyc_List_List*_tmp539;struct Cyc_List_List*ctx2=(_tmp539=_region_malloc(r,sizeof(*_tmp539)),((_tmp539->hd=((_tmp53A=_region_malloc(r,sizeof(*_tmp53A)),((_tmp53A->f1=_tmp2A5,((_tmp53A->f2=0,_tmp53A)))))),((_tmp539->tl=ctx,_tmp539)))));
struct _tuple19*_tmp53B;struct _tuple19*work_frame=(_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B->f1=_tmp2A6,((_tmp53B->f2=Cyc_Tcpat_getoargs(r,_tmp2A5,obj),((_tmp53B->f3=
Cyc_Tcpat_getdargs(r,_tmp2A5,dsc),_tmp53B)))))));
struct Cyc_List_List*_tmp53C;struct Cyc_List_List*work2=(_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C->hd=work_frame,((_tmp53C->tl=work,_tmp53C)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL15A:
# 1296
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL15B: {
# 1298
struct _tuple21*_tmp53F;struct Cyc_List_List*_tmp53E;struct Cyc_List_List*ctx2=(_tmp53E=_region_malloc(r,sizeof(*_tmp53E)),((_tmp53E->hd=((_tmp53F=_region_malloc(r,sizeof(*_tmp53F)),((_tmp53F->f1=_tmp2A5,((_tmp53F->f2=0,_tmp53F)))))),((_tmp53E->tl=ctx,_tmp53E)))));
struct _tuple19*_tmp540;struct _tuple19*work_frame=(_tmp540=_region_malloc(r,sizeof(*_tmp540)),((_tmp540->f1=_tmp2A6,((_tmp540->f2=Cyc_Tcpat_getoargs(r,_tmp2A5,obj),((_tmp540->f3=
Cyc_Tcpat_getdargs(r,_tmp2A5,dsc),_tmp540)))))));
struct Cyc_List_List*_tmp541;struct Cyc_List_List*work2=(_tmp541=_region_malloc(r,sizeof(*_tmp541)),((_tmp541->hd=work_frame,((_tmp541->tl=work,_tmp541)))));
void*_tmp2AB=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2AC=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2A5),work),rules);
# 1305
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp544;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp543;return(void*)((_tmp543=_region_malloc(r,sizeof(*_tmp543)),((_tmp543[0]=((_tmp544.tag=2,((_tmp544.f1=obj,((_tmp544.f2=_tmp2A5,((_tmp544.f3=_tmp2AB,((_tmp544.f4=_tmp2AC,_tmp544)))))))))),_tmp543))));}}}_LL154:;}
# 1315
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1321
void*_tmp2B3=d;void*_tmp2B5;void*_tmp2B7;void*_tmp2B9;void*_tmp2BA;_LL15E: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2B4=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2B3;if(_tmp2B4->tag != 0)goto _LL160;else{_tmp2B5=(void*)_tmp2B4->f1;}}_LL15F:
# 1323
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2B5);*exhaust_done=1;}
goto _LL15D;_LL160: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2B6=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2B3;if(_tmp2B6->tag != 1)goto _LL162;else{_tmp2B7=(void*)_tmp2B6->f1;}}_LL161:
 rhs_appears(env2,_tmp2B7);goto _LL15D;_LL162: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2B8=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2B3;if(_tmp2B8->tag != 2)goto _LL15D;else{_tmp2B9=(void*)_tmp2B8->f3;_tmp2BA=(void*)_tmp2B8->f4;}}_LL163:
# 1327
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B9,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1329
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2BA,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1331
goto _LL15D;_LL15D:;}struct _tuple23{void*f1;struct _tuple20*f2;};struct _tuple24{struct _RegionHandle*f1;int*f2;};
# 1339
static struct _tuple23*Cyc_Tcpat_get_match(struct _tuple24*env,struct Cyc_Absyn_Switch_clause*swc){
# 1342
struct _RegionHandle*_tmp2BC;int*_tmp2BD;struct _tuple24*_tmp2BB=env;_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2BC,swc->pattern);
struct _tuple20*_tmp545;struct _tuple20*rhs=(_tmp545=_region_malloc(_tmp2BC,sizeof(*_tmp545)),((_tmp545->f1=0,((_tmp545->f2=(swc->pattern)->loc,_tmp545)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp548;struct Cyc_List_List*_tmp547;sp=Cyc_Tcpat_tuple_pat(_tmp2BC,((_tmp547=_region_malloc(_tmp2BC,sizeof(*_tmp547)),((_tmp547->hd=sp0,((_tmp547->tl=(
(_tmp548=_region_malloc(_tmp2BC,sizeof(*_tmp548)),((_tmp548->hd=Cyc_Tcpat_int_pat(_tmp2BC,*_tmp2BD,0),((_tmp548->tl=0,_tmp548)))))),_tmp547)))))),0);}
*_tmp2BD=*_tmp2BD + 1;}else{
# 1351
struct Cyc_List_List*_tmp552;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp551;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp550;struct Cyc_List_List*_tmp54F;sp=Cyc_Tcpat_tuple_pat(_tmp2BC,((_tmp54F=_region_malloc(_tmp2BC,sizeof(*_tmp54F)),((_tmp54F->hd=sp0,((_tmp54F->tl=((_tmp552=_region_malloc(_tmp2BC,sizeof(*_tmp552)),((_tmp552->hd=(void*)((_tmp550=_region_malloc(_tmp2BC,sizeof(*_tmp550)),((_tmp550[0]=((_tmp551.tag=0,_tmp551)),_tmp550)))),((_tmp552->tl=0,_tmp552)))))),_tmp54F)))))),0);}{
struct _tuple23*_tmp553;return(_tmp553=_region_malloc(_tmp2BC,sizeof(*_tmp553)),((_tmp553->f1=sp,((_tmp553->f2=rhs,_tmp553)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1357
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1359
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2C7=d;struct Cyc_Tcpat_Con_s*_tmp2C9;struct Cyc_List_List*_tmp2CA;struct Cyc_Set_Set*_tmp2CC;_LL165: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C8=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C7;if(_tmp2C8->tag != 0)goto _LL167;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;}}_LL166: {
# 1363
union Cyc_Tcpat_Name_value _tmp2CD=_tmp2C9->name;
struct Cyc_Absyn_Pat*_tmp2CE=_tmp2C9->orig_pat;
if(_tmp2CE == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2CA))->hd);{
# 1368
void*_stmttmp27=_tmp2CE->r;void*_tmp2CF=_stmttmp27;struct Cyc_Absyn_Vardecl*_tmp2D2;struct Cyc_Absyn_Vardecl*_tmp2D4;struct Cyc_Absyn_Tvar*_tmp2D6;struct Cyc_Absyn_Vardecl*_tmp2D7;struct Cyc_Absyn_Aggrdecl*_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_Absyn_Datatypefield*_tmp2DE;int _tmp2E1;char _tmp2E3;struct _dyneither_ptr _tmp2E5;int _tmp2E6;struct Cyc_Absyn_Enumfield*_tmp2E8;struct Cyc_Absyn_Enumfield*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EC;_LL16A: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2D0=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D0->tag != 0)goto _LL16C;}_LL16B: {
const char*_tmp554;return(_tmp554="_",_tag_dyneither(_tmp554,sizeof(char),2));}_LL16C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2D1=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D1->tag != 1)goto _LL16E;else{_tmp2D2=_tmp2D1->f1;}}_LL16D:
 return Cyc_Absynpp_qvar2string(_tmp2D2->name);_LL16E: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2D3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D3->tag != 2)goto _LL170;else{_tmp2D4=_tmp2D3->f1;}}_LL16F: {
const char*_tmp558;void*_tmp557[1];struct Cyc_String_pa_PrintArg_struct _tmp556;return(struct _dyneither_ptr)((_tmp556.tag=0,((_tmp556.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D4->name)),((_tmp557[0]=& _tmp556,Cyc_aprintf(((_tmp558="*%s",_tag_dyneither(_tmp558,sizeof(char),4))),_tag_dyneither(_tmp557,sizeof(void*),1))))))));}_LL170: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2D5=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D5->tag != 3)goto _LL172;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}_LL171: {
const char*_tmp55D;void*_tmp55C[2];struct Cyc_String_pa_PrintArg_struct _tmp55B;struct Cyc_String_pa_PrintArg_struct _tmp55A;return(struct _dyneither_ptr)((_tmp55A.tag=0,((_tmp55A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2D6->name),((_tmp55B.tag=0,((_tmp55B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D7->name)),((_tmp55C[0]=& _tmp55B,((_tmp55C[1]=& _tmp55A,Cyc_aprintf(((_tmp55D="%s<`%s>",_tag_dyneither(_tmp55D,sizeof(char),8))),_tag_dyneither(_tmp55C,sizeof(void*),2))))))))))))));}_LL172: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2D8=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D8->tag != 4)goto _LL174;}_LL173: {
# 1374
const char*_tmp561;void*_tmp560[1];struct Cyc_String_pa_PrintArg_struct _tmp55F;return(struct _dyneither_ptr)((_tmp55F.tag=0,((_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp560[0]=& _tmp55F,Cyc_aprintf(((_tmp561="$(%s)",_tag_dyneither(_tmp561,sizeof(char),6))),_tag_dyneither(_tmp560,sizeof(void*),1))))))));}_LL174: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2D9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2D9->tag != 5)goto _LL176;}_LL175: {
# 1376
const char*_tmp565;void*_tmp564[1];struct Cyc_String_pa_PrintArg_struct _tmp563;return(struct _dyneither_ptr)((_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp564[0]=& _tmp563,Cyc_aprintf(((_tmp565="&%s",_tag_dyneither(_tmp565,sizeof(char),4))),_tag_dyneither(_tmp564,sizeof(void*),1))))))));}_LL176: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2DA=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2DA->tag != 6)goto _LL178;else{if(_tmp2DA->f1 == 0)goto _LL178;if((((_tmp2DA->f1)->aggr_info).KnownAggr).tag != 2)goto _LL178;_tmp2DB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2DA->f1)->aggr_info).KnownAggr).val);_tmp2DC=_tmp2DA->f3;}}_LL177:
# 1378
 if(_tmp2DB->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2FB=_tmp2DC;struct _dyneither_ptr*_tmp2FD;_LL18B: if(_tmp2FB == 0)goto _LL18D;if(((struct _tuple15*)_tmp2FB->hd)->f1 == 0)goto _LL18D;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2FC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2FB->hd)->f1)->hd);if(_tmp2FC->tag != 1)goto _LL18D;else{_tmp2FD=_tmp2FC->f1;}};_LL18C: {
# 1381
const char*_tmp56B;void*_tmp56A[3];struct Cyc_String_pa_PrintArg_struct _tmp569;struct Cyc_String_pa_PrintArg_struct _tmp568;struct Cyc_String_pa_PrintArg_struct _tmp567;return(struct _dyneither_ptr)((_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp568.tag=0,((_tmp568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2FD),((_tmp569.tag=0,((_tmp569.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1381
Cyc_Absynpp_qvar2string(_tmp2DB->name)),((_tmp56A[0]=& _tmp569,((_tmp56A[1]=& _tmp568,((_tmp56A[2]=& _tmp567,Cyc_aprintf(((_tmp56B="%s{.%s = %s}",_tag_dyneither(_tmp56B,sizeof(char),13))),_tag_dyneither(_tmp56A,sizeof(void*),3))))))))))))))))))));}_LL18D:;_LL18E:
# 1383
 goto _LL18A;_LL18A:;}{
# 1386
const char*_tmp570;void*_tmp56F[2];struct Cyc_String_pa_PrintArg_struct _tmp56E;struct Cyc_String_pa_PrintArg_struct _tmp56D;return(struct _dyneither_ptr)((_tmp56D.tag=0,((_tmp56D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp56E.tag=0,((_tmp56E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DB->name)),((_tmp56F[0]=& _tmp56E,((_tmp56F[1]=& _tmp56D,Cyc_aprintf(((_tmp570="%s{%s}",_tag_dyneither(_tmp570,sizeof(char),7))),_tag_dyneither(_tmp56F,sizeof(void*),2))))))))))))));};_LL178: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2DD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2DD->tag != 7)goto _LL17A;else{_tmp2DE=_tmp2DD->f2;}}_LL179:
# 1388
 if(_tmp2CA == 0){
const char*_tmp574;void*_tmp573[1];struct Cyc_String_pa_PrintArg_struct _tmp572;return(struct _dyneither_ptr)((_tmp572.tag=0,((_tmp572.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DE->name)),((_tmp573[0]=& _tmp572,Cyc_aprintf(((_tmp574="%s",_tag_dyneither(_tmp574,sizeof(char),3))),_tag_dyneither(_tmp573,sizeof(void*),1))))))));}else{
# 1391
const char*_tmp579;void*_tmp578[2];struct Cyc_String_pa_PrintArg_struct _tmp577;struct Cyc_String_pa_PrintArg_struct _tmp576;return(struct _dyneither_ptr)((_tmp576.tag=0,((_tmp576.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA)),((_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DE->name)),((_tmp578[0]=& _tmp577,((_tmp578[1]=& _tmp576,Cyc_aprintf(((_tmp579="%s(%s)",_tag_dyneither(_tmp579,sizeof(char),7))),_tag_dyneither(_tmp578,sizeof(void*),2))))))))))))));}_LL17A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2DF=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2DF->tag != 8)goto _LL17C;}_LL17B: {
const char*_tmp57A;return(_tmp57A="NULL",_tag_dyneither(_tmp57A,sizeof(char),5));}_LL17C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2E0=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2E0->tag != 9)goto _LL17E;else{_tmp2E1=_tmp2E0->f2;}}_LL17D: {
const char*_tmp57E;void*_tmp57D[1];struct Cyc_Int_pa_PrintArg_struct _tmp57C;return(struct _dyneither_ptr)((_tmp57C.tag=1,((_tmp57C.f1=(unsigned long)_tmp2E1,((_tmp57D[0]=& _tmp57C,Cyc_aprintf(((_tmp57E="%d",_tag_dyneither(_tmp57E,sizeof(char),3))),_tag_dyneither(_tmp57D,sizeof(void*),1))))))));}_LL17E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2E2=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2E2->tag != 10)goto _LL180;else{_tmp2E3=_tmp2E2->f1;}}_LL17F: {
const char*_tmp582;void*_tmp581[1];struct Cyc_Int_pa_PrintArg_struct _tmp580;return(struct _dyneither_ptr)((_tmp580.tag=1,((_tmp580.f1=(unsigned long)((int)_tmp2E3),((_tmp581[0]=& _tmp580,Cyc_aprintf(((_tmp582="%d",_tag_dyneither(_tmp582,sizeof(char),3))),_tag_dyneither(_tmp581,sizeof(void*),1))))))));}_LL180: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2E4=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2E4->tag != 11)goto _LL182;else{_tmp2E5=_tmp2E4->f1;_tmp2E6=_tmp2E4->f2;}}_LL181:
 return _tmp2E5;_LL182: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2E7=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2E7->tag != 12)goto _LL184;else{_tmp2E8=_tmp2E7->f2;}}_LL183:
 _tmp2EA=_tmp2E8;goto _LL185;_LL184: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2E9=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2E9->tag != 13)goto _LL186;else{_tmp2EA=_tmp2E9->f2;}}_LL185:
 return Cyc_Absynpp_qvar2string(_tmp2EA->name);_LL186: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2EB=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2CF;if(_tmp2EB->tag != 16)goto _LL188;else{_tmp2EC=_tmp2EB->f1;}}_LL187:
 return Cyc_Absynpp_exp2string(_tmp2EC);_LL188:;_LL189:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL169:;};}_LL167: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2CB=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C7;if(_tmp2CB->tag != 1)goto _LL164;else{_tmp2CC=_tmp2CB->f1;}}_LL168:
# 1402
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2CC)){const char*_tmp583;return(_tmp583="_",_tag_dyneither(_tmp583,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp316=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2CC);
struct Cyc_Absyn_Pat*_tmp317=_tmp316->orig_pat;
# 1407
if(_tmp317 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_stmttmp25=Cyc_Tcutil_compress((void*)_check_null(_tmp317->topt));void*_tmp318=_stmttmp25;struct Cyc_Absyn_PtrAtts _tmp31C;struct Cyc_Absyn_Datatypedecl*_tmp31E;struct Cyc_Absyn_Aggrdecl*_tmp320;_LL190: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp319=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318;if(_tmp319->tag != 6)goto _LL192;else{if(_tmp319->f2 != Cyc_Absyn_Char_sz)goto _LL192;}}_LL191:
# 1411
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp321=Cyc_Tcpat_char_con(r,(char)i,_tmp317);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,_tmp321)){
const char*_tmp587;void*_tmp586[1];struct Cyc_Int_pa_PrintArg_struct _tmp585;return(struct _dyneither_ptr)((_tmp585.tag=1,((_tmp585.f1=(unsigned long)i,((_tmp586[0]=& _tmp585,Cyc_aprintf(((_tmp587="%d",_tag_dyneither(_tmp587,sizeof(char),3))),_tag_dyneither(_tmp586,sizeof(void*),1))))))));}}}
# 1416
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL192: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318;if(_tmp31A->tag != 6)goto _LL194;}_LL193:
# 1419
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp325=Cyc_Tcpat_int_con(r,(int)i,_tmp317);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,_tmp325)){
const char*_tmp58B;void*_tmp58A[1];struct Cyc_Int_pa_PrintArg_struct _tmp589;return(struct _dyneither_ptr)((_tmp589.tag=1,((_tmp589.f1=(unsigned long)((int)i),((_tmp58A[0]=& _tmp589,Cyc_aprintf(((_tmp58B="%d",_tag_dyneither(_tmp58B,sizeof(char),3))),_tag_dyneither(_tmp58A,sizeof(void*),1))))))));}}}
# 1424
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL194: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318;if(_tmp31B->tag != 5)goto _LL196;else{_tmp31C=(_tmp31B->f1).ptr_atts;}}_LL195: {
# 1426
union Cyc_Absyn_Constraint*_tmp329=_tmp31C.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp329);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,Cyc_Tcpat_null_con(r,_tmp317))){
const char*_tmp58C;return(_tmp58C="NULL",_tag_dyneither(_tmp58C,sizeof(char),5));}}{
# 1432
const char*_tmp58D;return(_tmp58D="&_",_tag_dyneither(_tmp58D,sizeof(char),3));};}_LL196: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318;if(_tmp31D->tag != 3)goto _LL198;else{if((((_tmp31D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL198;_tmp31E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp31D->f1).datatype_info).KnownDatatype).val);}}_LL197:
# 1434
 if(_tmp31E->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp32C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp31E->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32C);
for(0;(unsigned int)_tmp32C;_tmp32C=_tmp32C->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp32C->hd)->name).f2;
struct Cyc_List_List*_tmp32D=((struct Cyc_Absyn_Datatypefield*)_tmp32C->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp58E;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,((_tmp58E=_cycalloc(sizeof(*_tmp58E)),((_tmp58E->name=Cyc_Tcpat_Name_v(n),((_tmp58E->arity=0,((_tmp58E->span=0,((_tmp58E->orig_pat=_tmp317,_tmp58E)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32D)== 0)
return n;else{
# 1444
const char*_tmp592;void*_tmp591[1];struct Cyc_String_pa_PrintArg_struct _tmp590;return(struct _dyneither_ptr)((_tmp590.tag=0,((_tmp590.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp591[0]=& _tmp590,Cyc_aprintf(((_tmp592="%s(...)",_tag_dyneither(_tmp592,sizeof(char),8))),_tag_dyneither(_tmp591,sizeof(void*),1))))))));}}}
# 1447
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL198:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp318;if(_tmp31F->tag != 11)goto _LL19A;else{if((((_tmp31F->f1).aggr_info).KnownAggr).tag != 2)goto _LL19A;_tmp320=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp31F->f1).aggr_info).KnownAggr).val);}}if(!(_tmp320->kind == Cyc_Absyn_UnionA))goto _LL19A;_LL199: {
# 1449
struct Cyc_List_List*_tmp332=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp320->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332);
struct _tuple2*_stmttmp26=_tmp320->name;struct _dyneither_ptr _tmp334;struct _tuple2*_tmp333=_stmttmp26;_tmp334=*_tmp333->f2;
for(0;(unsigned int)_tmp332;_tmp332=_tmp332->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp332->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp593;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,((_tmp593=_cycalloc(sizeof(*_tmp593)),((_tmp593->name=Cyc_Tcpat_Name_v(n),((_tmp593->arity=0,((_tmp593->span=0,((_tmp593->orig_pat=_tmp317,_tmp593)))))))))))){
const char*_tmp598;void*_tmp597[2];struct Cyc_String_pa_PrintArg_struct _tmp596;struct Cyc_String_pa_PrintArg_struct _tmp595;return(struct _dyneither_ptr)((_tmp595.tag=0,((_tmp595.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp334),((_tmp597[0]=& _tmp596,((_tmp597[1]=& _tmp595,Cyc_aprintf(((_tmp598="%s{.%s = _}",_tag_dyneither(_tmp598,sizeof(char),12))),_tag_dyneither(_tmp597,sizeof(void*),2))))))))))))));}}
# 1457
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18F:;};};_LL164:;}
# 1462
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp599;return(_tmp599=_cycalloc(sizeof(*_tmp599)),((_tmp599[0]=Cyc_Tcpat_desc2string(r,d),_tmp599)));}
# 1465
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp33B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp33C=_tmp33B;for(0;_tmp33C != 0;_tmp33C=((struct Cyc_List_List*)_check_null(_tmp33C))->tl){
if(_tmp33C->tl != 0){
{struct Cyc_List_List*_tmp59A;_tmp33C->tl=((_tmp59A=_cycalloc(sizeof(*_tmp59A)),((_tmp59A->hd=comma,((_tmp59A->tl=_tmp33C->tl,_tmp59A))))));}
_tmp33C=_tmp33C->tl;}}}
# 1474
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp33B);}
# 1477
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp33F;_push_handler(& _tmp33F);{int _tmp341=0;if(setjmp(_tmp33F.handler))_tmp341=1;if(!_tmp341){
{struct _dyneither_ptr _tmp342=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp59E;void*_tmp59D[1];struct Cyc_String_pa_PrintArg_struct _tmp59C;(_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp342),((_tmp59D[0]=& _tmp59C,Cyc_Tcutil_terr(loc,((_tmp59E="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp59E,sizeof(char),53))),_tag_dyneither(_tmp59D,sizeof(void*),1)))))));}
# 1479
;_pop_handler();}else{void*_tmp340=(void*)_exn_thrown;void*_tmp347=_tmp340;void*_tmp349;_LL19D: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp348=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp347;if(_tmp348->tag != Cyc_Tcpat_Desc2string)goto _LL19F;}_LL19E:
# 1483
{const char*_tmp5A1;void*_tmp5A0;(_tmp5A0=0,Cyc_Tcutil_terr(loc,((_tmp5A1="patterns may not be exhaustive.",_tag_dyneither(_tmp5A1,sizeof(char),32))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}
goto _LL19C;_LL19F: _tmp349=_tmp347;_LL1A0:(void)_throw(_tmp349);_LL19C:;}};}
# 1487
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple20*rhs){
(*rhs).f1=1;}
# 1491
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1496
struct _RegionHandle _tmp34C=_new_region("r");struct _RegionHandle*r=& _tmp34C;_push_region(r);
{int _tmp34D=0;
struct _tuple24 _tmp5A2;struct _tuple24 _tmp34E=(_tmp5A2.f1=r,((_tmp5A2.f2=& _tmp34D,_tmp5A2)));
struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _tuple24*,struct Cyc_Absyn_Switch_clause*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp34E,swcs);
void*_tmp350=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp34F);
int _tmp351=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp350,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp351);
# 1504
for(0;_tmp34F != 0;_tmp34F=_tmp34F->tl){
struct _tuple23*_stmttmp28=(struct _tuple23*)_tmp34F->hd;int _tmp353;unsigned int _tmp354;struct _tuple23*_tmp352=_stmttmp28;_tmp353=(_tmp352->f2)->f1;_tmp354=(_tmp352->f2)->f2;
if(!_tmp353){
{const char*_tmp5A5;void*_tmp5A4;(_tmp5A4=0,Cyc_Tcutil_terr(_tmp354,((_tmp5A5="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5A5,sizeof(char),74))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}
# 1509
break;}}}
# 1497
;_pop_region(r);}
# 1519
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp358;_push_handler(& _tmp358);{int _tmp35A=0;if(setjmp(_tmp358.handler))_tmp35A=1;if(!_tmp35A){
{struct _dyneither_ptr _tmp35B=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5A9;void*_tmp5A8[1];struct Cyc_String_pa_PrintArg_struct _tmp5A7;(_tmp5A7.tag=0,((_tmp5A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35B),((_tmp5A8[0]=& _tmp5A7,Cyc_Tcutil_warn((*pr).f1,((_tmp5A9="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5A9,sizeof(char),45))),_tag_dyneither(_tmp5A8,sizeof(void*),1)))))));}
# 1522
;_pop_handler();}else{void*_tmp359=(void*)_exn_thrown;void*_tmp360=_tmp359;void*_tmp362;_LL1A2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp361=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp360;if(_tmp361->tag != Cyc_Tcpat_Desc2string)goto _LL1A4;}_LL1A3:
# 1526
{const char*_tmp5AC;void*_tmp5AB;(_tmp5AB=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5AC="pattern not exhaustive.",_tag_dyneither(_tmp5AC,sizeof(char),24))),_tag_dyneither(_tmp5AB,sizeof(void*),0)));}
goto _LL1A1;_LL1A4: _tmp362=_tmp360;_LL1A5:(void)_throw(_tmp362);_LL1A1:;}};};}
# 1530
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple25{void*f1;int f2;};
# 1533
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp365=_new_region("r");struct _RegionHandle*r=& _tmp365;_push_region(r);
{struct _tuple25*_tmp5AF;struct Cyc_List_List*_tmp5AE;struct Cyc_List_List*_tmp366=(_tmp5AE=_region_malloc(r,sizeof(*_tmp5AE)),((_tmp5AE->hd=((_tmp5AF=_region_malloc(r,sizeof(*_tmp5AF)),((_tmp5AF->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp5AF->f2=0,_tmp5AF)))))),((_tmp5AE->tl=0,_tmp5AE)))));
void*_tmp367=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp366);
struct _tuple14 _tmp5B0;struct _tuple14 _tmp368=(_tmp5B0.f1=loc,((_tmp5B0.f2=1,_tmp5B0)));
int _tmp369=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp367,Cyc_Tcpat_not_exhaust_warn,& _tmp368,Cyc_Tcpat_dummy_fn,0,& _tmp369);{
# 1541
int _tmp36A=_tmp368.f2;_npop_handler(0);return _tmp36A;};}
# 1535
;_pop_region(r);}
# 1549
static struct _tuple23*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1551
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple20*_tmp5B1;struct _tuple20*rhs=(_tmp5B1=_region_malloc(r,sizeof(*_tmp5B1)),((_tmp5B1->f1=0,((_tmp5B1->f2=(swc->pattern)->loc,_tmp5B1)))));
struct _tuple23*_tmp5B2;return(_tmp5B2=_region_malloc(r,sizeof(*_tmp5B2)),((_tmp5B2->f1=sp0,((_tmp5B2->f2=rhs,_tmp5B2)))));}
# 1555
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1558
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1560
struct _RegionHandle _tmp370=_new_region("r");struct _RegionHandle*r=& _tmp370;_push_region(r);
{struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp372=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp371);
int _tmp373=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp372,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp373);
# 1566
for(0;_tmp371 != 0;_tmp371=_tmp371->tl){
struct _tuple23*_stmttmp29=(struct _tuple23*)_tmp371->hd;int _tmp375;unsigned int _tmp376;struct _tuple23*_tmp374=_stmttmp29;_tmp375=(_tmp374->f2)->f1;_tmp376=(_tmp374->f2)->f2;
if(!_tmp375){
{const char*_tmp5B5;void*_tmp5B4;(_tmp5B4=0,Cyc_Tcutil_terr(_tmp376,((_tmp5B5="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5B5,sizeof(char),74))),_tag_dyneither(_tmp5B4,sizeof(void*),0)));}
break;}}}
# 1561
;_pop_region(r);}
