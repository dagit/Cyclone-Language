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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
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
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 241 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
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
void*_tmp0=p->r;void*_tmp1=_tmp0;struct _tuple2*_tmp3;struct _tuple2*_tmp5;struct Cyc_List_List*_tmp6;int _tmp7;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;enum Cyc_Absyn_AggrKind _tmpD;struct _tuple2*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List**_tmp10;struct Cyc_List_List*_tmp11;int _tmp12;struct Cyc_Absyn_Exp*_tmp14;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp2->tag != 14)goto _LL3;else{_tmp3=_tmp2->f1;}}_LL2:
# 55
{struct _handler_cons _tmp15;_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!_tmp17){
{struct _RegionHandle _tmp18=_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);
{void*_tmp19=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp3,0);void*_tmp1A=_tmp19;struct Cyc_Absyn_Datatypedecl*_tmp1D;struct Cyc_Absyn_Datatypefield*_tmp1E;struct Cyc_Absyn_Enumdecl*_tmp20;struct Cyc_Absyn_Enumfield*_tmp21;void*_tmp23;struct Cyc_Absyn_Enumfield*_tmp24;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp1B=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1B->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_Tcutil_terr(p->loc,((_tmp3A5="struct tag used without arguments in pattern",_tag_dyneither(_tmp3A5,sizeof(char),45))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1C->tag != 2)goto _LL12;else{_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3A8;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3A7;p->r=(void*)((_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7[0]=((_tmp3A8.tag=7,((_tmp3A8.f1=_tmp1D,((_tmp3A8.f2=_tmp1E,((_tmp3A8.f3=0,((_tmp3A8.f4=0,_tmp3A8)))))))))),_tmp3A7))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1F->tag != 3)goto _LL14;else{_tmp20=_tmp1F->f1;_tmp21=_tmp1F->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3AB;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp3AA;p->r=(void*)((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA[0]=((_tmp3AB.tag=12,((_tmp3AB.f1=_tmp20,((_tmp3AB.f2=_tmp21,_tmp3AB)))))),_tmp3AA))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp22=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp22->tag != 4)goto _LL16;else{_tmp23=(void*)_tmp22->f1;_tmp24=_tmp22->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3AE;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp3AD;p->r=(void*)((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD[0]=((_tmp3AE.tag=13,((_tmp3AE.f1=_tmp23,((_tmp3AE.f2=_tmp24,_tmp3AE)))))),_tmp3AD))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp25=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp25->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp16=(void*)_exn_thrown;void*_tmp2F=_tmp16;void*_tmp31;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp30=(struct Cyc_Dict_Absent_exn_struct*)_tmp2F;if(_tmp30->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 73
 goto _LL18;_LL1B: _tmp31=_tmp2F;_LL1C:(void)_throw(_tmp31);_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp32=(*_tmp3).f1;union Cyc_Absyn_Nmspace _tmp33=_tmp32;_LL1E: if((_tmp33.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp33.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL22;_LL21:
# 78
(*_tmp3).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3B1;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp3B0;p->r=(void*)((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0[0]=((_tmp3B1.tag=1,((_tmp3B1.f1=Cyc_Absyn_new_vardecl(_tmp3,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp3B1.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3B1)))))),_tmp3B0))));}
return;_LL22:;_LL23:
{const char*_tmp3B4;void*_tmp3B3;(_tmp3B3=0,Cyc_Tcutil_terr(p->loc,((_tmp3B4="qualified variable in pattern",_tag_dyneither(_tmp3B4,sizeof(char),30))),_tag_dyneither(_tmp3B3,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp4->tag != 15)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;_tmp7=_tmp4->f3;}}_LL4:
# 85
{struct _handler_cons _tmp38;_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!_tmp3A){
{struct _RegionHandle _tmp3B=_new_region("r");struct _RegionHandle*r=& _tmp3B;_push_region(r);
{void*_tmp3C=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp5,0);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Aggrdecl*_tmp3F;struct Cyc_Absyn_Datatypedecl*_tmp41;struct Cyc_Absyn_Datatypefield*_tmp42;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3E=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL27;else{_tmp3F=_tmp3E->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp46=0;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct _tuple15*_tmp3B7;struct Cyc_List_List*_tmp3B6;_tmp46=((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6->hd=((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->f1=0,((_tmp3B7->f2=(struct Cyc_Absyn_Pat*)_tmp6->hd,_tmp3B7)))))),((_tmp3B6->tl=_tmp46,_tmp3B6))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3C1;struct Cyc_Absyn_Aggrdecl**_tmp3C0;struct Cyc_Absyn_AggrInfo*_tmp3BF;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3BE;p->r=(void*)((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=((_tmp3C1.tag=6,((_tmp3C1.f1=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0[0]=_tmp3F,_tmp3C0))))),((_tmp3BF->targs=0,_tmp3BF)))))),((_tmp3C1.f2=0,((_tmp3C1.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46),((_tmp3C1.f4=_tmp7,_tmp3C1)))))))))),_tmp3BE))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp40->tag != 2)goto _LL29;else{_tmp41=_tmp40->f1;_tmp42=_tmp40->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3C4;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3C3;p->r=(void*)((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C4.tag=7,((_tmp3C4.f1=_tmp41,((_tmp3C4.f2=_tmp42,((_tmp3C4.f3=_tmp6,((_tmp3C4.f4=_tmp7,_tmp3C4)))))))))),_tmp3C3))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp43=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp43->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp44=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp44->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp3C7;void*_tmp3C6;(_tmp3C6=0,Cyc_Tcutil_terr(p->loc,((_tmp3C7="enum tag used with arguments in pattern",_tag_dyneither(_tmp3C7,sizeof(char),40))),_tag_dyneither(_tmp3C6,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp45=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp45->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp39=(void*)_exn_thrown;void*_tmp52=_tmp39;void*_tmp54;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp53=(struct Cyc_Dict_Absent_exn_struct*)_tmp52;if(_tmp53->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp54=_tmp52;_LL33:(void)_throw(_tmp54);_LL2F:;}};}
{const char*_tmp3CB;void*_tmp3CA[1];struct Cyc_String_pa_PrintArg_struct _tmp3C9;(_tmp3C9.tag=0,((_tmp3C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp3CA[0]=& _tmp3C9,Cyc_Tcutil_terr(p->loc,((_tmp3CB="%s is not a constructor in pattern",_tag_dyneither(_tmp3CB,sizeof(char),35))),_tag_dyneither(_tmp3CA,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp8->tag != 6)goto _LL7;else{if(_tmp8->f1 != 0)goto _LL7;_tmp9=_tmp8->f2;_tmpA=_tmp8->f3;_tmpB=_tmp8->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp3CE;void*_tmp3CD;(_tmp3CD=0,Cyc_Tcutil_terr(p->loc,((_tmp3CE="cannot determine pattern type",_tag_dyneither(_tmp3CE,sizeof(char),30))),_tag_dyneither(_tmp3CD,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp5A=t;struct Cyc_Absyn_AggrInfo _tmp5C;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A;if(_tmp5B->tag != 11)goto _LL37;else{_tmp5C=_tmp5B->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3D4;struct Cyc_Absyn_AggrInfo*_tmp3D3;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3D2;p->r=(void*)((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((_tmp3D4.tag=6,((_tmp3D4.f1=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=_tmp5C,_tmp3D3)))),((_tmp3D4.f2=_tmp9,((_tmp3D4.f3=_tmpA,((_tmp3D4.f4=_tmpB,_tmp3D4)))))))))),_tmp3D2))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_PrintArg_struct _tmp3D6;(_tmp3D6.tag=0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3D7[0]=& _tmp3D6,Cyc_Tcutil_terr(p->loc,((_tmp3D8="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3D8,sizeof(char),45))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmpC->tag != 6)goto _LL9;else{if(_tmpC->f1 == 0)goto _LL9;if((((_tmpC->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpD=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f1;_tmpE=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f2;_tmpF=(_tmpC->f1)->targs;_tmp10=(struct Cyc_List_List**)& _tmpC->f2;_tmp11=_tmpC->f3;_tmp12=_tmpC->f4;}}_LL8:
# 126
{struct _handler_cons _tmp63;_push_handler(& _tmp63);{int _tmp65=0;if(setjmp(_tmp63.handler))_tmp65=1;if(!_tmp65){
{struct Cyc_Absyn_Aggrdecl**_tmp66=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);
struct Cyc_Absyn_Aggrdecl*_tmp67=*_tmp66;
if(_tmp67->impl == 0){
{const char*_tmp3E0;void*_tmp3DF[1];const char*_tmp3DE;const char*_tmp3DD;struct Cyc_String_pa_PrintArg_struct _tmp3DC;(_tmp3DC.tag=0,((_tmp3DC.f1=(struct _dyneither_ptr)(
_tmp67->kind == Cyc_Absyn_StructA?(_tmp3DD="struct",_tag_dyneither(_tmp3DD,sizeof(char),7)):((_tmp3DE="union",_tag_dyneither(_tmp3DE,sizeof(char),6)))),((_tmp3DF[0]=& _tmp3DC,Cyc_Tcutil_terr(p->loc,((_tmp3E0="can't destructure an abstract %s",_tag_dyneither(_tmp3E0,sizeof(char),33))),_tag_dyneither(_tmp3DF,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp10);
if(more_exists < 0){
{const char*_tmp3E3;void*_tmp3E2;(_tmp3E2=0,Cyc_Tcutil_terr(p->loc,((_tmp3E3="too many existentially bound type variables in pattern",_tag_dyneither(_tmp3E3,sizeof(char),55))),_tag_dyneither(_tmp3E2,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6F=_tmp10;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6F=&((struct Cyc_List_List*)_check_null(*_tmp6F))->tl;}}
*_tmp6F=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp70=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3E9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3E8;struct Cyc_List_List*_tmp3E7;_tmp70=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3E8.tag=1,((_tmp3E8.f1=0,_tmp3E8)))),_tmp3E9))))),((_tmp3E7->tl=_tmp70,_tmp3E7))))));}
*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp10,_tmp70);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3EF;struct Cyc_Absyn_AggrInfo*_tmp3EE;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3ED;p->r=(void*)((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED[0]=((_tmp3EF.tag=6,((_tmp3EF.f1=((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE->aggr_info=Cyc_Absyn_KnownAggr(_tmp66),((_tmp3EE->targs=_tmpF,_tmp3EE)))))),((_tmp3EF.f2=*_tmp10,((_tmp3EF.f3=_tmp11,((_tmp3EF.f4=_tmp12,_tmp3EF)))))))))),_tmp3ED))));};};}
# 127
;_pop_handler();}else{void*_tmp64=(void*)_exn_thrown;void*_tmp78=_tmp64;void*_tmp7A;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3F2;void*_tmp3F1;(_tmp3F1=0,Cyc_Tcutil_terr(p->loc,((_tmp3F2="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3F2,sizeof(char),43))),_tag_dyneither(_tmp3F1,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp7A=_tmp78;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp13=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp13->tag != 16)goto _LLB;else{_tmp14=_tmp13->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp14);
if(!Cyc_Tcutil_is_const_exp(_tmp14)){
{const char*_tmp3F5;void*_tmp3F4;(_tmp3F4=0,Cyc_Tcutil_terr(p->loc,((_tmp3F5="non-constant expression in case pattern",_tag_dyneither(_tmp3F5,sizeof(char),40))),_tag_dyneither(_tmp3F4,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp7F=Cyc_Evexp_eval_const_uint_exp(_tmp14);unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=_tmp7F;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3F8;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3F7;p->r=(void*)((_tmp3F7=_cycalloc_atomic(sizeof(*_tmp3F7)),((_tmp3F7[0]=((_tmp3F8.tag=9,((_tmp3F8.f1=Cyc_Absyn_None,((_tmp3F8.f2=(int)_tmp81,_tmp3F8)))))),_tmp3F7))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3F9;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->v=s,_tmp3F9)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp86=Cyc_Tcutil_compress(numt);void*_tmp87=_tmp86;_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp87;if(_tmp88->tag != 13)goto _LL41;}_LL40:
 goto _LL42;_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp87;if(_tmp89->tag != 14)goto _LL43;}_LL42:
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
struct _tuple16*_tmp3FC;struct Cyc_List_List*_tmp3FB;*v_result_ptr=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->f1=vd,((_tmp3FC->f2=e,_tmp3FC)))))),((_tmp3FB->tl=*v_result_ptr,_tmp3FB))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=res1;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;{
struct _tuple1*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_Tcpat_TcPatResult _tmp8F=res2;_tmp90=_tmp8F.tvars_and_bounds_opt;_tmp91=_tmp8F.patvars;
if(_tmp8D != 0  || _tmp90 != 0){
if(_tmp8D == 0){
struct _tuple1*_tmp3FD;_tmp8D=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD->f1=0,((_tmp3FD->f2=0,_tmp3FD))))));}
if(_tmp90 == 0){
struct _tuple1*_tmp3FE;_tmp90=((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE->f1=0,((_tmp3FE->f2=0,_tmp3FE))))));}{
struct _tuple1*_tmp401;struct Cyc_Tcpat_TcPatResult _tmp400;return(_tmp400.tvars_and_bounds_opt=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f1,(*_tmp90).f1),((_tmp401->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f2,(*_tmp90).f2),_tmp401)))))),((_tmp400.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp400)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp402;return(_tmp402.tvars_and_bounds_opt=0,((_tmp402.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp402)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp403;return(_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403->loc=loc,((_tmp403->topt=0,((_tmp403->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp403)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp404;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->v=Cyc_Tcenv_lookup_type_vars(te),_tmp404))))): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 229
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 232
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp405;struct Cyc_Tcpat_TcPatResult res=(_tmp405.tvars_and_bounds_opt=0,((_tmp405.patvars=0,_tmp405)));
# 237
{void*_tmp99=p->r;void*_tmp9A=_tmp99;struct Cyc_Absyn_Vardecl*_tmp9D;struct Cyc_Absyn_Pat*_tmp9E;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Pat*_tmpA1;struct Cyc_Absyn_Tvar*_tmpA3;struct Cyc_Absyn_Vardecl*_tmpA4;int _tmpAA;struct Cyc_Absyn_Enumdecl*_tmpAC;void*_tmpAE;struct Cyc_Absyn_Pat*_tmpB1;struct Cyc_List_List**_tmpB3;int _tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB6;struct Cyc_List_List**_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List**_tmpB9;int _tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBC;struct Cyc_Absyn_Datatypefield*_tmpBD;struct Cyc_List_List**_tmpBE;int _tmpBF;_LL46: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9B->tag != 0)goto _LL48;}_LL47:
# 240
 if(topt != 0)
t=*topt;else{
# 243
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL45;_LL48: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9C->tag != 1)goto _LL4A;else{_tmp9D=_tmp9C->f1;_tmp9E=_tmp9C->f2;}}_LL49:
# 246
 res=Cyc_Tcpat_tcPatRec(te,_tmp9E,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9E->topt);
# 250
{void*_tmpC5=Cyc_Tcutil_compress(t);void*_tmpC6=_tmpC5;_LL71: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC6;if(_tmpC7->tag != 8)goto _LL73;}_LL72:
# 252
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_Tcutil_terr(p->loc,((_tmp408="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp408,sizeof(char),58))),_tag_dyneither(_tmp407,sizeof(void*),0)));}
goto _LL70;_LL73:;_LL74:
# 256
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_Tcutil_terr(p->loc,((_tmp40B="pattern would point to an abstract member",_tag_dyneither(_tmp40B,sizeof(char),42))),_tag_dyneither(_tmp40A,sizeof(void*),0)));}
goto _LL70;_LL70:;}
# 260
{struct Cyc_Absyn_Vardecl**_tmp40C;Cyc_Tcpat_set_vd(((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=_tmp9D,_tmp40C)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL45;_LL4A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp9F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9F->tag != 2)goto _LL4C;else{_tmpA0=_tmp9F->f1;_tmpA1=_tmp9F->f2;}}_LL4B:
# 263
 res=Cyc_Tcpat_tcPatRec(te,_tmpA1,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA1->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp40F;void*_tmp40E;(_tmp40E=0,Cyc_Tcutil_terr(p->loc,((_tmp40F="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp40F,sizeof(char),55))),_tag_dyneither(_tmp40E,sizeof(void*),0)));}
goto _LL45;}else{
# 270
struct _RegionHandle _tmpCF=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCF;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp412;void*_tmp411;(_tmp411=0,Cyc_Tcutil_terr(p->loc,((_tmp412="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp412,sizeof(char),56))),_tag_dyneither(_tmp411,sizeof(void*),0)));}
# 271
;_pop_region(rgn);}{
# 276
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp418;struct Cyc_Absyn_PtrInfo _tmp417;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp416;void*t2=(void*)((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=((_tmp418.tag=5,((_tmp418.f1=((_tmp417.elt_typ=t,((_tmp417.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp417.ptr_atts=(
((_tmp417.ptr_atts).rgn=rgn_pat,(((_tmp417.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp417.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp417.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp417.ptr_atts).ptrloc=0,_tmp417.ptr_atts)))))))))),_tmp417)))))),_tmp418)))),_tmp416))));
# 281
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 285
{struct Cyc_Absyn_Vardecl**_tmp419;Cyc_Tcpat_set_vd(((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419[0]=_tmpA0,_tmp419)))),new_access_exp,& res.patvars,t2);}
goto _LL45;};_LL4C: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA2=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA2->tag != 3)goto _LL4E;else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL4D:
# 291
{struct Cyc_Absyn_Vardecl**_tmp41A;Cyc_Tcpat_set_vd(((_tmp41A=_cycalloc(sizeof(*_tmp41A)),((_tmp41A[0]=_tmpA4,_tmp41A)))),access_exp,& res.patvars,_tmpA4->type);}{
# 295
struct _RegionHandle _tmpD7=_new_region("r2");struct _RegionHandle*r2=& _tmpD7;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp41B[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp41B[0]=_tmpA3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41B,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 298
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp41C;res.tvars_and_bounds_opt=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->f1=0,((_tmp41C->f2=0,_tmp41C))))));}
{struct Cyc_List_List*_tmp41D;(*res.tvars_and_bounds_opt).f1=(
(_tmp41D=_cycalloc(sizeof(*_tmp41D)),((_tmp41D->hd=_tmpA3,((_tmp41D->tl=(*res.tvars_and_bounds_opt).f1,_tmp41D))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL45;
# 295
;_pop_region(r2);};_LL4E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA5->tag != 9)goto _LL50;else{if(_tmpA5->f1 != Cyc_Absyn_Unsigned)goto _LL50;}}_LL4F:
# 305
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL45;_LL50: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA6->tag != 9)goto _LL52;else{if(_tmpA6->f1 != Cyc_Absyn_None)goto _LL52;}}_LL51:
 goto _LL53;_LL52: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA7->tag != 9)goto _LL54;else{if(_tmpA7->f1 != Cyc_Absyn_Signed)goto _LL54;}}_LL53:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL45;_LL54: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA8->tag != 10)goto _LL56;}_LL55:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL45;_LL56: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA9->tag != 11)goto _LL58;else{_tmpAA=_tmpA9->f2;}}_LL57:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAA));goto _LL45;_LL58: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAB->tag != 12)goto _LL5A;else{_tmpAC=_tmpAB->f1;}}_LL59:
# 311
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp420;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp41F;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp420.tag=13,((_tmp420.f1=_tmpAC->name,((_tmp420.f2=_tmpAC,_tmp420)))))),_tmp41F)))));}
goto _LL45;_LL5A: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAD->tag != 13)goto _LL5C;else{_tmpAE=(void*)_tmpAD->f1;}}_LL5B:
 t=Cyc_Tcpat_num_type(topt,_tmpAE);goto _LL45;_LL5C: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAF->tag != 8)goto _LL5E;}_LL5D:
# 315
 if(topt != 0){
void*_tmpDD=Cyc_Tcutil_compress(*topt);void*_tmpDE=_tmpDD;_LL76: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDE;if(_tmpDF->tag != 5)goto _LL78;}_LL77:
 t=*topt;goto tcpat_end;_LL78:;_LL79:
 goto _LL75;_LL75:;}{
# 320
struct Cyc_Core_Opt*_tmpE0=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp426;struct Cyc_Absyn_PtrInfo _tmp425;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp424;t=(void*)((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424[0]=((_tmp426.tag=5,((_tmp426.f1=((_tmp425.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpE0),((_tmp425.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp425.ptr_atts=(
((_tmp425.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpE0),(((_tmp425.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp425.ptr_atts).bounds=
# 325
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp425.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp425.ptr_atts).ptrloc=0,_tmp425.ptr_atts)))))))))),_tmp425)))))),_tmp426)))),_tmp424))));}
# 328
goto _LL45;};_LL5E: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB0->tag != 5)goto _LL60;else{_tmpB1=_tmpB0->f1;}}_LL5F: {
# 333
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpE4=0;
int elt_const=0;
if(topt != 0){
void*_tmpE5=Cyc_Tcutil_compress(*topt);void*_tmpE6=_tmpE5;void*_tmpE8;struct Cyc_Absyn_Tqual _tmpE9;_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE6;if(_tmpE7->tag != 5)goto _LL7D;else{_tmpE8=(_tmpE7->f1).elt_typ;_tmpE9=(_tmpE7->f1).elt_tq;}}_LL7C:
# 339
 inner_typ=_tmpE8;
_tmpE4=& inner_typ;
elt_const=_tmpE9.real_const;
goto _LL7A;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 348
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB1,_tmpE4,ptr_rgn,1,new_access_exp));
# 357
{void*_tmpEA=Cyc_Tcutil_compress((void*)_check_null(_tmpB1->topt));void*_tmpEB=_tmpEA;struct Cyc_Absyn_Datatypedecl*_tmpED;struct Cyc_Absyn_Datatypefield*_tmpEE;struct Cyc_List_List*_tmpEF;_LL80: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEC=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEB;if(_tmpEC->tag != 4)goto _LL82;else{if((((_tmpEC->f1).field_info).KnownDatatypefield).tag != 2)goto _LL82;_tmpED=((struct _tuple3)(((_tmpEC->f1).field_info).KnownDatatypefield).val).f1;_tmpEE=((struct _tuple3)(((_tmpEC->f1).field_info).KnownDatatypefield).val).f2;_tmpEF=(_tmpEC->f1).targs;}}_LL81:
# 361
{void*_tmpF0=Cyc_Tcutil_compress(inner_typ);void*_tmpF1=_tmpF0;_LL85: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF1;if(_tmpF2->tag != 4)goto _LL87;}_LL86:
# 363
 goto DONT_PROMOTE;_LL87:;_LL88:
 goto _LL84;_LL84:;}{
# 367
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp430;struct Cyc_Absyn_Datatypedecl**_tmp42F;struct Cyc_Absyn_DatatypeInfo _tmp42E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp42D;void*new_type=(void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp430.tag=3,((_tmp430.f1=((_tmp42E.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F[0]=_tmpED,_tmp42F))))),((_tmp42E.targs=_tmpEF,_tmp42E)))),_tmp430)))),_tmp42D))));
# 370
_tmpB1->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp436;struct Cyc_Absyn_PtrInfo _tmp435;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp434;t=(void*)((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434[0]=((_tmp436.tag=5,((_tmp436.f1=((_tmp435.elt_typ=new_type,((_tmp435.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp435.ptr_atts=(
((_tmp435.ptr_atts).rgn=ptr_rgn,(((_tmp435.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp435.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp435.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp435.ptr_atts).ptrloc=0,_tmp435.ptr_atts)))))))))),_tmp435)))))),_tmp436)))),_tmp434))));}
# 377
goto _LL7F;};_LL82:;_LL83:
# 379
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp43C;struct Cyc_Absyn_PtrInfo _tmp43B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43A;t=(void*)((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((_tmp43C.tag=5,((_tmp43C.f1=((_tmp43B.elt_typ=(void*)_check_null(_tmpB1->topt),((_tmp43B.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp43B.ptr_atts=(
((_tmp43B.ptr_atts).rgn=ptr_rgn,(((_tmp43B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp43B.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp43B.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp43B.ptr_atts).ptrloc=0,_tmp43B.ptr_atts)))))))))),_tmp43B)))))),_tmp43C)))),_tmp43A))));}_LL7F:;}
# 387
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpB1->topt;
goto _LL45;};}_LL60: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB2=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB2->tag != 4)goto _LL62;else{_tmpB3=(struct Cyc_List_List**)& _tmpB2->f1;_tmpB4=_tmpB2->f2;}}_LL61: {
# 391
struct Cyc_List_List*_tmpFD=*_tmpB3;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpFE=Cyc_Tcutil_compress(*topt);void*_tmpFF=_tmpFE;struct Cyc_List_List*_tmp101;_LL8A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp100=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFF;if(_tmp100->tag != 10)goto _LL8C;else{_tmp101=_tmp100->f1;}}_LL8B:
# 397
 topt_ts=_tmp101;
if(_tmpB4){
# 400
int _tmp102=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD);
int _tmp103=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101);
if(_tmp102 < _tmp103){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp103 - _tmp102;++ i){
struct Cyc_List_List*_tmp43D;wild_ps=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp43D->tl=wild_ps,_tmp43D))))));}}
*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpFD,wild_ps);
_tmpFD=*_tmpB3;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101)){
const char*_tmp440;void*_tmp43F;(_tmp43F=0,Cyc_Tcutil_warn(p->loc,((_tmp440="unnecessary ... in tuple pattern",_tag_dyneither(_tmp440,sizeof(char),33))),_tag_dyneither(_tmp43F,sizeof(void*),0)));}}}
# 412
goto _LL89;_LL8C:;_LL8D:
# 414
 goto _LL89;_LL89:;}else{
# 416
if(_tmpB4){
const char*_tmp443;void*_tmp442;(_tmp442=0,Cyc_Tcutil_terr(p->loc,((_tmp443="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp443,sizeof(char),57))),_tag_dyneither(_tmp442,sizeof(void*),0)));}}
{int i=0;for(0;_tmpFD != 0;(_tmpFD=_tmpFD->tl,i ++)){
void**_tmp109=0;
if(topt_ts != 0){
_tmp109=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 424
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp446;union Cyc_Absyn_Cnst _tmp445;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp445.Int_c).val=((_tmp446.f1=Cyc_Absyn_Unsigned,((_tmp446.f2=i,_tmp446)))),(((_tmp445.Int_c).tag=5,_tmp445)))),0),0);}
# 429
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpFD->hd,_tmp109,rgn_pat,allow_ref_pat,new_access_exp));
# 432
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpFD->hd)->topt;{
struct _tuple17*_tmp449;struct Cyc_List_List*_tmp448;pat_ts=((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->hd=((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449->f1=Cyc_Absyn_empty_tqual(0),((_tmp449->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpFD->hd)->topt),_tmp449)))))),((_tmp448->tl=pat_ts,_tmp448))))));};};}}
# 435
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp44C;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44B;t=(void*)((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=((_tmp44C.tag=10,((_tmp44C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp44C)))),_tmp44B))));}
goto _LL45;}_LL62: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB5->tag != 6)goto _LL64;else{if(_tmpB5->f1 == 0)goto _LL64;if((((_tmpB5->f1)->aggr_info).KnownAggr).tag != 2)goto _LL64;_tmpB6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB5->f1)->aggr_info).KnownAggr).val);_tmpB7=(struct Cyc_List_List**)&(_tmpB5->f1)->targs;_tmpB8=_tmpB5->f2;_tmpB9=(struct Cyc_List_List**)& _tmpB5->f3;_tmpBA=_tmpB5->f4;}}_LL63: {
# 439
struct Cyc_List_List*_tmp110=*_tmpB9;
const char*_tmp44E;const char*_tmp44D;struct _dyneither_ptr aggr_str=_tmpB6->kind == Cyc_Absyn_StructA?(_tmp44E="struct",_tag_dyneither(_tmp44E,sizeof(char),7)):((_tmp44D="union",_tag_dyneither(_tmp44D,sizeof(char),6)));
if(_tmpB6->impl == 0){
{const char*_tmp452;void*_tmp451[1];struct Cyc_String_pa_PrintArg_struct _tmp450;(_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp451[0]=& _tmp450,Cyc_Tcutil_terr(p->loc,((_tmp452="can't destructure an abstract %s",_tag_dyneither(_tmp452,sizeof(char),33))),_tag_dyneither(_tmp451,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL45;}
# 448
if(_tmpB6->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->tagged)
allow_ref_pat=0;
if(_tmpB8 != 0){
# 454
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 457
struct _RegionHandle _tmp114=_new_region("rgn");struct _RegionHandle*rgn=& _tmp114;_push_region(rgn);{
# 459
struct Cyc_List_List*_tmp115=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp116=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpB8;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp116))->hd;
_tmp116=_tmp116->tl;{
void*_tmp117=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp118=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp119=_tmp118;struct Cyc_Absyn_Kind*_tmp11B;struct Cyc_Absyn_Kind*_tmp11D;_LL8F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp119;if(_tmp11A->tag != 2)goto _LL91;else{_tmp11B=_tmp11A->f2;}}_LL90:
 _tmp11D=_tmp11B;goto _LL92;_LL91: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp11C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp119;if(_tmp11C->tag != 0)goto _LL93;else{_tmp11D=_tmp11C->f1;}}_LL92:
 k2=_tmp11D;goto _LL8E;_LL93:;_LL94: {
const char*_tmp455;void*_tmp454;(_tmp454=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp455="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp455,sizeof(char),53))),_tag_dyneither(_tmp454,sizeof(void*),0)));}_LL8E:;}
# 475
{void*_tmp120=_tmp117;struct Cyc_Absyn_Kind*_tmp122;struct Cyc_Core_Opt**_tmp124;struct Cyc_Absyn_Kind*_tmp125;struct Cyc_Core_Opt**_tmp127;_LL96: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp121=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp120;if(_tmp121->tag != 0)goto _LL98;else{_tmp122=_tmp121->f1;}}_LL97:
# 478
 if(!Cyc_Tcutil_kind_leq(k2,_tmp122))
error=1;
goto _LL95;_LL98: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp123=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp120;if(_tmp123->tag != 2)goto _LL9A;else{_tmp124=(struct Cyc_Core_Opt**)& _tmp123->f1;_tmp125=_tmp123->f2;}}_LL99:
 _tmp127=_tmp124;goto _LL9B;_LL9A: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp126=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp120;if(_tmp126->tag != 1)goto _LL95;else{_tmp127=(struct Cyc_Core_Opt**)& _tmp126->f1;}}_LL9B:
{struct Cyc_Core_Opt*_tmp456;*_tmp127=((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456->v=_tmp118,_tmp456))));}goto _LL95;_LL95:;}
# 484
if(error){
const char*_tmp45C;void*_tmp45B[3];struct Cyc_String_pa_PrintArg_struct _tmp45A;struct Cyc_String_pa_PrintArg_struct _tmp459;struct Cyc_String_pa_PrintArg_struct _tmp458;(_tmp458.tag=0,((_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 488
Cyc_Absynpp_kind2string(k2)),((_tmp459.tag=0,((_tmp459.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp117)),((_tmp45A.tag=0,((_tmp45A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp45B[0]=& _tmp45A,((_tmp45B[1]=& _tmp459,((_tmp45B[2]=& _tmp458,Cyc_Tcutil_terr(p->loc,((_tmp45C="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp45C,sizeof(char),60))),_tag_dyneither(_tmp45B,sizeof(void*),3)))))))))))))))))));}{
# 490
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp45F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp45E;void*vartype=(void*)((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp45F.tag=2,((_tmp45F.f1=tv,_tmp45F)))),_tmp45E))));
{struct Cyc_List_List*_tmp460;_tmp115=((_tmp460=_region_malloc(rgn,sizeof(*_tmp460)),((_tmp460->hd=vartype,((_tmp460->tl=_tmp115,_tmp460))))));}
# 494
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp463;struct Cyc_List_List*_tmp462;outlives_constraints=(
(_tmp462=_cycalloc(sizeof(*_tmp462)),((_tmp462->hd=((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463->f1=Cyc_Absyn_empty_effect,((_tmp463->f2=vartype,_tmp463)))))),((_tmp462->tl=outlives_constraints,_tmp462))))));}else{
# 499
const char*_tmp466;void*_tmp465;(_tmp465=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp466="opened existential had unique or top region kind",_tag_dyneither(_tmp466,sizeof(char),49))),_tag_dyneither(_tmp465,sizeof(void*),0)));}}};};}}
# 503
_tmp115=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp115);{
# 505
struct _RegionHandle _tmp135=_new_region("r2");struct _RegionHandle*r2=& _tmp135;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpB8);
# 508
struct Cyc_List_List*_tmp136=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp467;struct _tuple12 _tmp137=(_tmp467.f1=_tmp136,((_tmp467.f2=rgn,_tmp467)));
struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp137,_tmpB6->tvs);
struct Cyc_List_List*_tmp139=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars,_tmp115);
struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp138);
struct Cyc_List_List*_tmp13B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp139);
struct Cyc_List_List*_tmp13C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp138,_tmp139);
# 516
if(_tmpB8 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp468;res.tvars_and_bounds_opt=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468->f1=0,((_tmp468->f2=0,_tmp468))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpB8);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp13E=0;
{struct Cyc_List_List*_tmp13F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){
struct _tuple0*_tmp46B;struct Cyc_List_List*_tmp46A;_tmp13E=((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A->hd=((_tmp46B=_cycalloc(sizeof(*_tmp46B)),((_tmp46B->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp13C,(*((struct _tuple0*)_tmp13F->hd)).f1),((_tmp46B->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp13C,(*((struct _tuple0*)_tmp13F->hd)).f2),_tmp46B)))))),((_tmp46A->tl=_tmp13E,_tmp46A))))));}}
# 528
_tmp13E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13E);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp13E);};}
# 533
*_tmpB7=_tmp13A;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp475;struct Cyc_Absyn_Aggrdecl**_tmp474;struct Cyc_Absyn_AggrInfo _tmp473;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp472;t=(void*)((_tmp472=_cycalloc(sizeof(*_tmp472)),((_tmp472[0]=((_tmp475.tag=11,((_tmp475.f1=((_tmp473.aggr_info=Cyc_Absyn_KnownAggr(((_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[0]=_tmpB6,_tmp474))))),((_tmp473.targs=*_tmpB7,_tmp473)))),_tmp475)))),_tmp472))));}
if(_tmpBA  && _tmpB6->kind == Cyc_Absyn_UnionA){
const char*_tmp478;void*_tmp477;(_tmp477=0,Cyc_Tcutil_warn(p->loc,((_tmp478="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp478,sizeof(char),43))),_tag_dyneither(_tmp477,sizeof(void*),0)));}else{
if(_tmpBA){
# 539
int _tmp148=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp110);
int _tmp149=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
if(_tmp148 < _tmp149){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp149 - _tmp148;++ i){
struct _tuple15*_tmp47B;struct Cyc_List_List*_tmp47A;wild_dps=((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A->hd=((_tmp47B=_cycalloc(sizeof(*_tmp47B)),((_tmp47B->f1=0,((_tmp47B->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp47B)))))),((_tmp47A->tl=wild_dps,_tmp47A))))));}}
*_tmpB9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp110,wild_dps);
_tmp110=*_tmpB9;}else{
if(_tmp148 == _tmp149){
const char*_tmp47E;void*_tmp47D;(_tmp47D=0,Cyc_Tcutil_warn(p->loc,((_tmp47E="unnecessary ... in struct pattern",_tag_dyneither(_tmp47E,sizeof(char),34))),_tag_dyneither(_tmp47D,sizeof(void*),0)));}}}}{
# 550
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp110,_tmpB6->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple18*_tmp14E=(struct _tuple18*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp150;struct Cyc_Absyn_Pat*_tmp151;struct _tuple18*_tmp14F=_tmp14E;_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;{
void*_tmp152=Cyc_Tcutil_rsubstitute(rgn,_tmp13C,_tmp150->type);
# 556
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp150->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp151,& _tmp152,rgn_pat,allow_ref_pat,new_access_exp));
# 564
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp151->topt),_tmp152)){
const char*_tmp485;void*_tmp484[4];struct Cyc_String_pa_PrintArg_struct _tmp483;struct Cyc_String_pa_PrintArg_struct _tmp482;struct Cyc_String_pa_PrintArg_struct _tmp481;struct Cyc_String_pa_PrintArg_struct _tmp480;(_tmp480.tag=0,((_tmp480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 567
Cyc_Absynpp_typ2string((void*)_check_null(_tmp151->topt))),((_tmp481.tag=0,((_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 566
Cyc_Absynpp_typ2string(_tmp152)),((_tmp482.tag=0,((_tmp482.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp483.tag=0,((_tmp483.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp150->name),((_tmp484[0]=& _tmp483,((_tmp484[1]=& _tmp482,((_tmp484[2]=& _tmp481,((_tmp484[3]=& _tmp480,Cyc_Tcutil_terr(p->loc,((_tmp485="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp485,sizeof(char),45))),_tag_dyneither(_tmp484,sizeof(void*),4)))))))))))))))))))))))));}
# 568
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp151->topt;};}};}
# 506
;_pop_region(r2);};}
# 572
_npop_handler(0);goto _LL45;
# 457
;_pop_region(rgn);};}_LL64: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBB=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpBB->tag != 7)goto _LL66;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;_tmpBE=(struct Cyc_List_List**)& _tmpBB->f3;_tmpBF=_tmpBB->f4;}}_LL65: {
# 575
struct Cyc_List_List*_tmp15C=*_tmpBE;
struct _RegionHandle _tmp15D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp15D;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBD->typs;
# 579
struct Cyc_List_List*_tmp15E=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp486;struct _tuple12 _tmp15F=(_tmp486.f1=_tmp15E,((_tmp486.f2=rgn,_tmp486)));
struct Cyc_List_List*_tmp160=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp15F,_tmpBC->tvs);
struct Cyc_List_List*_tmp161=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp160);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp48C;struct Cyc_Absyn_DatatypeFieldInfo _tmp48B;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp48A;t=(void*)((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A[0]=((_tmp48C.tag=4,((_tmp48C.f1=((_tmp48B.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBC,_tmpBD),((_tmp48B.targs=_tmp161,_tmp48B)))),_tmp48C)))),_tmp48A))));}
# 585
if(_tmpBF){
# 587
int _tmp165=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp15C);
int _tmp166=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp165 < _tmp166){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp166 - _tmp165;++ i){
struct Cyc_List_List*_tmp48D;wild_ps=((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp48D->tl=wild_ps,_tmp48D))))));}}
*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp15C,wild_ps);
_tmp15C=*_tmpBE;}else{
if(_tmp165 == _tmp166){
const char*_tmp491;void*_tmp490[1];struct Cyc_String_pa_PrintArg_struct _tmp48F;(_tmp48F.tag=0,((_tmp48F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBC->name)),((_tmp490[0]=& _tmp48F,Cyc_Tcutil_warn(p->loc,((_tmp491="unnecessary ... in datatype field %s",_tag_dyneither(_tmp491,sizeof(char),37))),_tag_dyneither(_tmp490,sizeof(void*),1)))))));}}}
# 599
for(0;_tmp15C != 0  && tqts != 0;(_tmp15C=_tmp15C->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp16B=(struct Cyc_Absyn_Pat*)_tmp15C->hd;
# 603
void*_tmp16C=Cyc_Tcutil_rsubstitute(rgn,_tmp160,(*((struct _tuple17*)tqts->hd)).f2);
# 606
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp16B,& _tmp16C,rgn_pat,allow_ref_pat,0));
# 613
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp16B->topt),_tmp16C)){
const char*_tmp497;void*_tmp496[3];struct Cyc_String_pa_PrintArg_struct _tmp495;struct Cyc_String_pa_PrintArg_struct _tmp494;struct Cyc_String_pa_PrintArg_struct _tmp493;(_tmp493.tag=0,((_tmp493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 616
Cyc_Absynpp_typ2string((void*)_check_null(_tmp16B->topt))),((_tmp494.tag=0,((_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 615
Cyc_Absynpp_typ2string(_tmp16C)),((_tmp495.tag=0,((_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp496[0]=& _tmp495,((_tmp496[1]=& _tmp494,((_tmp496[2]=& _tmp493,Cyc_Tcutil_terr(_tmp16B->loc,((_tmp497="%s expects argument type %s, not %s",_tag_dyneither(_tmp497,sizeof(char),36))),_tag_dyneither(_tmp496,sizeof(void*),3)))))))))))))))))));}}
# 618
if(_tmp15C != 0){
const char*_tmp49B;void*_tmp49A[1];struct Cyc_String_pa_PrintArg_struct _tmp499;(_tmp499.tag=0,((_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp49A[0]=& _tmp499,Cyc_Tcutil_terr(p->loc,((_tmp49B="too many arguments for datatype constructor %s",_tag_dyneither(_tmp49B,sizeof(char),47))),_tag_dyneither(_tmp49A,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp49F;void*_tmp49E[1];struct Cyc_String_pa_PrintArg_struct _tmp49D;(_tmp49D.tag=0,((_tmp49D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp49E[0]=& _tmp49D,Cyc_Tcutil_terr(p->loc,((_tmp49F="too few arguments for datatype constructor %s",_tag_dyneither(_tmp49F,sizeof(char),46))),_tag_dyneither(_tmp49E,sizeof(void*),1)))))));}}
# 625
_npop_handler(0);goto _LL45;
# 576
;_pop_region(rgn);}_LL66: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC0->tag != 6)goto _LL68;else{if(_tmpC0->f1 != 0)goto _LL68;}}_LL67:
# 627
 goto _LL69;_LL68: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC1->tag != 6)goto _LL6A;else{if(_tmpC1->f1 == 0)goto _LL6A;if((((_tmpC1->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6A;}}_LL69:
 goto _LL6B;_LL6A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC2->tag != 14)goto _LL6C;}_LL6B:
 goto _LL6D;_LL6C: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC3->tag != 16)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC4->tag != 15)goto _LL45;}_LL6F:
# 632
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL45;_LL45:;}
# 634
tcpat_end:
 p->topt=t;
return res;};}
# 639
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 641
struct Cyc_Tcpat_TcPatResult _tmp17A=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 643
struct _RegionHandle _tmp17B=_new_region("r");struct _RegionHandle*r=& _tmp17B;_push_region(r);{
struct _tuple1 _tmp17C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp17A.patvars);struct Cyc_List_List*_tmp17E;struct _tuple1 _tmp17D=_tmp17C;_tmp17E=_tmp17D.f1;{
struct Cyc_List_List*_tmp17F=0;
{struct Cyc_List_List*x=_tmp17E;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp4A0;_tmp17F=((_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0)),((_tmp4A0->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp4A0->tl=_tmp17F,_tmp4A0))))));}}}
{const char*_tmp4A1;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp17F),p->loc,(
(_tmp4A1="pattern contains a repeated variable",_tag_dyneither(_tmp4A1,sizeof(char),37))));}{
# 653
struct Cyc_List_List*x=_tmp17A.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp182=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp184;struct Cyc_Absyn_Exp**_tmp185;struct _tuple16*_tmp183=_tmp182;_tmp184=_tmp183->f1;_tmp185=(struct Cyc_Absyn_Exp**)& _tmp183->f2;
if(*_tmp185 != 0  && *_tmp185 != pat_var_exp)
*_tmp185=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp185));}};};}{
# 659
struct Cyc_Tcpat_TcPatResult _tmp186=_tmp17A;_npop_handler(0);return _tmp186;};
# 643
;_pop_region(r);}
# 664
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 668
void*_tmp187=p->r;void*_tmp188=_tmp187;struct Cyc_Absyn_Pat*_tmp18A;struct Cyc_Absyn_AggrInfo*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp190;struct Cyc_List_List*_tmp192;struct Cyc_Absyn_Vardecl*_tmp194;struct Cyc_Absyn_Pat*_tmp195;struct Cyc_Absyn_Vardecl*_tmp197;struct Cyc_Absyn_Pat*_tmp198;_LL9D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp189=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp189->tag != 5)goto _LL9F;else{_tmp18A=_tmp189->f1;}}_LL9E: {
# 670
void*_tmp199=(void*)_check_null(p->topt);void*_tmp19A=_tmp199;void*_tmp19C;_LLAC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 5)goto _LLAE;else{_tmp19C=((_tmp19B->f1).ptr_atts).rgn;}}_LLAD:
# 672
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp19C);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp18A,Cyc_Tcutil_is_noalias_region(_tmp19C,0),patvars);
return;_LLAE:;_LLAF: {
const char*_tmp4A4;void*_tmp4A3;(_tmp4A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A4="check_pat_regions: bad pointer type",_tag_dyneither(_tmp4A4,sizeof(char),36))),_tag_dyneither(_tmp4A3,sizeof(void*),0)));}_LLAB:;}_LL9F: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp18B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp18B->tag != 6)goto _LLA1;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;_tmp18E=_tmp18B->f3;}}_LLA0:
# 678
 for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp18E->hd)).f2,did_noalias_deref,patvars);}
return;_LLA1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp18F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp18F->tag != 7)goto _LLA3;else{_tmp190=_tmp18F->f3;}}_LLA2:
# 682
 did_noalias_deref=0;_tmp192=_tmp190;goto _LLA4;_LLA3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp191=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp191->tag != 4)goto _LLA5;else{_tmp192=_tmp191->f1;}}_LLA4:
# 684
 for(0;_tmp192 != 0;_tmp192=_tmp192->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp192->hd,did_noalias_deref,patvars);}
return;_LLA5: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp193=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp193->tag != 2)goto _LLA7;else{_tmp194=_tmp193->f1;_tmp195=_tmp193->f2;}}_LLA6:
# 688
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp19F=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;struct _tuple16*_tmp1A0=_tmp19F;_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;
# 694
if((_tmp1A1 != 0  && *_tmp1A1 == _tmp194) && _tmp1A2 != 0){
{void*_tmp1A3=_tmp1A2->r;void*_tmp1A4=_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A6;_LLB1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1A4;if(_tmp1A5->tag != 14)goto _LLB3;else{_tmp1A6=_tmp1A5->f1;}}_LLB2:
# 697
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp1A6->topt))){
# 699
const char*_tmp4A7;void*_tmp4A6;(_tmp4A6=0,Cyc_Tcutil_terr(p->loc,((_tmp4A7="Reference pattern not allowed on unique pointers",_tag_dyneither(_tmp4A7,sizeof(char),49))),_tag_dyneither(_tmp4A6,sizeof(void*),0)));}
goto _LLB0;_LLB3:;_LLB4: {
# 702
const char*_tmp4AA;void*_tmp4A9;(_tmp4A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AA="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp4AA,sizeof(char),51))),_tag_dyneither(_tmp4A9,sizeof(void*),0)));}_LLB0:;}
# 704
break;}}}
# 707
Cyc_Tcpat_check_pat_regions_rec(te,_tmp195,did_noalias_deref,patvars);
return;_LLA7: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp196=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp196->tag != 1)goto _LLA9;else{_tmp197=_tmp196->f1;_tmp198=_tmp196->f2;}}_LLA8:
# 710
{void*_tmp1AB=p->topt;void*_tmp1AC=_tmp1AB;_LLB6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC;if(_tmp1AD == 0)goto _LLB8;else{if(_tmp1AD->tag != 8)goto _LLB8;}}_LLB7:
# 712
 if(did_noalias_deref){
{const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,Cyc_Tcutil_terr(p->loc,((_tmp4AD="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp4AD,sizeof(char),56))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}
return;}
# 716
goto _LLB5;_LLB8:;_LLB9:
 goto _LLB5;_LLB5:;}
# 719
Cyc_Tcpat_check_pat_regions_rec(te,_tmp198,did_noalias_deref,patvars);
return;_LLA9:;_LLAA:
 return;_LL9C:;}
# 736 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 738
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1B0=_new_region("r");struct _RegionHandle*r=& _tmp1B0;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1B1=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct _tuple16*_tmp1B2=_tmp1B1;_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;
if(_tmp1B4 != 0){
struct Cyc_Absyn_Exp*_tmp1B5=_tmp1B4;
# 746
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1B5->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1B5)){
# 749
const char*_tmp4BA;void*_tmp4B9[1];const char*_tmp4B8;struct Cyc_String_pa_PrintArg_struct _tmp4B7;void*_tmp4B6[1];const char*_tmp4B5;struct Cyc_String_pa_PrintArg_struct _tmp4B4;(_tmp4B4.tag=0,((_tmp4B4.f1=(struct _dyneither_ptr)(
# 751
_tmp1B3 != 0?(struct _dyneither_ptr)(
(_tmp4B7.tag=0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1B3)->name)),((_tmp4B6[0]=& _tmp4B7,Cyc_aprintf(((_tmp4B5="for variable %s",_tag_dyneither(_tmp4B5,sizeof(char),16))),_tag_dyneither(_tmp4B6,sizeof(void*),1)))))))):(
(_tmp4B8="",_tag_dyneither(_tmp4B8,sizeof(char),1)))),((_tmp4B9[0]=& _tmp4B4,Cyc_Tcutil_terr(p->loc,((_tmp4BA="Pattern %s dereferences a unique pointer from a non-unique path",_tag_dyneither(_tmp4BA,sizeof(char),64))),_tag_dyneither(_tmp4B9,sizeof(void*),1)))))));}}}}
# 740
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 797 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp4BB;return((_tmp4BB.Name_v).val=s,(((_tmp4BB.Name_v).tag=1,_tmp4BB)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp4BC;return((_tmp4BC.Int_v).val=i,(((_tmp4BC.Int_v).tag=2,_tmp4BC)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 807
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 812
typedef void*Cyc_Tcpat_simple_pat_t;
# 815
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1BF=c1->name;union Cyc_Tcpat_Name_value _tmp1C0=_tmp1BF;struct _dyneither_ptr _tmp1C1;int _tmp1C2;_LLBB: if((_tmp1C0.Name_v).tag != 1)goto _LLBD;_tmp1C1=(struct _dyneither_ptr)(_tmp1C0.Name_v).val;_LLBC: {
# 818
union Cyc_Tcpat_Name_value _tmp1C3=c2->name;union Cyc_Tcpat_Name_value _tmp1C4=_tmp1C3;struct _dyneither_ptr _tmp1C5;_LLC0: if((_tmp1C4.Name_v).tag != 1)goto _LLC2;_tmp1C5=(struct _dyneither_ptr)(_tmp1C4.Name_v).val;_LLC1:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1C1,(struct _dyneither_ptr)_tmp1C5);_LLC2: if((_tmp1C4.Int_v).tag != 2)goto _LLBF;_LLC3:
 return - 1;_LLBF:;}_LLBD: if((_tmp1C0.Int_v).tag != 2)goto _LLBA;_tmp1C2=(int)(_tmp1C0.Int_v).val;_LLBE: {
# 823
union Cyc_Tcpat_Name_value _tmp1C6=c2->name;union Cyc_Tcpat_Name_value _tmp1C7=_tmp1C6;int _tmp1C8;_LLC5: if((_tmp1C7.Name_v).tag != 1)goto _LLC7;_LLC6:
 return 1;_LLC7: if((_tmp1C7.Int_v).tag != 2)goto _LLC4;_tmp1C8=(int)(_tmp1C7.Int_v).val;_LLC8:
 return _tmp1C2 - _tmp1C8;_LLC4:;}_LLBA:;}
# 831
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 835
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 839
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp4BF;struct Cyc_Tcpat_Con_s*_tmp4BE;return(_tmp4BE=_region_malloc(r,sizeof(*_tmp4BE)),((_tmp4BE->name=Cyc_Tcpat_Name_v(((_tmp4BF="NULL",_tag_dyneither(_tmp4BF,sizeof(char),5)))),((_tmp4BE->arity=0,((_tmp4BE->span=& Cyc_Tcpat_two_opt,((_tmp4BE->orig_pat=p,_tmp4BE)))))))));}
# 842
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp4C2;struct Cyc_Tcpat_Con_s*_tmp4C1;return(_tmp4C1=_region_malloc(r,sizeof(*_tmp4C1)),((_tmp4C1->name=Cyc_Tcpat_Name_v(((_tmp4C2="&",_tag_dyneither(_tmp4C2,sizeof(char),2)))),((_tmp4C1->arity=1,((_tmp4C1->span=& Cyc_Tcpat_two_opt,((_tmp4C1->orig_pat=p,_tmp4C1)))))))));}
# 845
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp4C5;struct Cyc_Tcpat_Con_s*_tmp4C4;return(_tmp4C4=_region_malloc(r,sizeof(*_tmp4C4)),((_tmp4C4->name=Cyc_Tcpat_Name_v(((_tmp4C5="&",_tag_dyneither(_tmp4C5,sizeof(char),2)))),((_tmp4C4->arity=1,((_tmp4C4->span=& Cyc_Tcpat_one_opt,((_tmp4C4->orig_pat=p,_tmp4C4)))))))));}
# 848
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4C6;return(_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6)),((_tmp4C6->name=Cyc_Tcpat_Int_v(i),((_tmp4C6->arity=0,((_tmp4C6->span=0,((_tmp4C6->orig_pat=p,_tmp4C6)))))))));}
# 851
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4C7;return(_tmp4C7=_region_malloc(r,sizeof(*_tmp4C7)),((_tmp4C7->name=Cyc_Tcpat_Name_v(f),((_tmp4C7->arity=0,((_tmp4C7->span=0,((_tmp4C7->orig_pat=p,_tmp4C7)))))))));}
# 854
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4C8;return(_tmp4C8=_region_malloc(r,sizeof(*_tmp4C8)),((_tmp4C8->name=Cyc_Tcpat_Int_v((int)c),((_tmp4C8->arity=0,((_tmp4C8->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp4C8->orig_pat=p,_tmp4C8)))))))));}
# 857
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp4CB;struct Cyc_Tcpat_Con_s*_tmp4CA;return(_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA)),((_tmp4CA->name=Cyc_Tcpat_Name_v(((_tmp4CB="$",_tag_dyneither(_tmp4CB,sizeof(char),2)))),((_tmp4CA->arity=i,((_tmp4CA->span=& Cyc_Tcpat_one_opt,((_tmp4CA->orig_pat=p,_tmp4CA)))))))));}
# 862
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4CE;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4CD;return(void*)((_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CE.tag=1,((_tmp4CE.f1=Cyc_Tcpat_null_con(r,p),((_tmp4CE.f2=0,_tmp4CE)))))),_tmp4CD))));}
# 865
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4D1;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4D0;return(void*)((_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4D1.tag=1,((_tmp4D1.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp4D1.f2=0,_tmp4D1)))))),_tmp4D0))));}
# 868
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4D4;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4D3;return(void*)((_tmp4D3=_region_malloc(r,sizeof(*_tmp4D3)),((_tmp4D3[0]=((_tmp4D4.tag=1,((_tmp4D4.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp4D4.f2=0,_tmp4D4)))))),_tmp4D3))));}
# 871
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4D7;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4D6;return(void*)((_tmp4D6=_region_malloc(r,sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D7.tag=1,((_tmp4D7.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp4D7.f2=0,_tmp4D7)))))),_tmp4D6))));}
# 874
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4DD;struct Cyc_List_List*_tmp4DC;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4DB;return(void*)((_tmp4DB=_region_malloc(r,sizeof(*_tmp4DB)),((_tmp4DB[0]=((_tmp4DD.tag=1,((_tmp4DD.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4DD.f2=((_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC->hd=p,((_tmp4DC->tl=0,_tmp4DC)))))),_tmp4DD)))))),_tmp4DB))));}
# 877
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4E3;struct Cyc_List_List*_tmp4E2;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4E1;return(void*)((_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1[0]=((_tmp4E3.tag=1,((_tmp4E3.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4E3.f2=((_tmp4E2=_region_malloc(r,sizeof(*_tmp4E2)),((_tmp4E2->hd=p,((_tmp4E2->tl=0,_tmp4E2)))))),_tmp4E3)))))),_tmp4E1))));}
# 880
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4E6;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4E5;return(void*)((_tmp4E5=_region_malloc(r,sizeof(*_tmp4E5)),((_tmp4E5[0]=((_tmp4E6.tag=1,((_tmp4E6.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4E6.f2=ss,_tmp4E6)))))),_tmp4E5))));}
# 883
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 885
struct Cyc_Tcpat_Con_s*_tmp4E7;struct Cyc_Tcpat_Con_s*c=(_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7)),((_tmp4E7->name=Cyc_Tcpat_Name_v(con_name),((_tmp4E7->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4E7->span=span,((_tmp4E7->orig_pat=p,_tmp4E7)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4EA;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4E9;return(void*)((_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9[0]=((_tmp4EA.tag=1,((_tmp4EA.f1=c,((_tmp4EA.f2=ps,_tmp4EA)))))),_tmp4E9))));}
# 890
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1E7=p->r;void*_tmp1E8=_tmp1E7;enum Cyc_Absyn_Sign _tmp1ED;int _tmp1EE;char _tmp1F0;struct _dyneither_ptr _tmp1F2;struct Cyc_Absyn_Pat*_tmp1F4;struct Cyc_Absyn_Pat*_tmp1F6;struct Cyc_Absyn_Pat*_tmp1F8;struct Cyc_Absyn_Datatypedecl*_tmp1FA;struct Cyc_Absyn_Datatypefield*_tmp1FB;struct Cyc_List_List*_tmp1FC;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Aggrdecl*_tmp200;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_Enumdecl*_tmp203;struct Cyc_Absyn_Enumfield*_tmp204;void*_tmp206;struct Cyc_Absyn_Enumfield*_tmp207;_LLCA: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1E9=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1E9->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1EA=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EA->tag != 3)goto _LLCE;}_LLCD:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4ED;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4EC;s=(void*)((_tmp4EC=_region_malloc(r,sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4ED.tag=0,_tmp4ED)),_tmp4EC))));}goto _LLC9;_LLCE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1EB=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EB->tag != 8)goto _LLD0;}_LLCF:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC9;_LLD0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1EC=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EC->tag != 9)goto _LLD2;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LLD1:
 s=Cyc_Tcpat_int_pat(r,_tmp1EE,p);goto _LLC9;_LLD2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1EF=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1EF->tag != 10)goto _LLD4;else{_tmp1F0=_tmp1EF->f1;}}_LLD3:
 s=Cyc_Tcpat_char_pat(r,_tmp1F0,p);goto _LLC9;_LLD4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1F1=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F1->tag != 11)goto _LLD6;else{_tmp1F2=_tmp1F1->f1;}}_LLD5:
 s=Cyc_Tcpat_float_pat(r,_tmp1F2,p);goto _LLC9;_LLD6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1F3=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F3->tag != 1)goto _LLD8;else{_tmp1F4=_tmp1F3->f2;}}_LLD7:
 s=Cyc_Tcpat_compile_pat(r,_tmp1F4);goto _LLC9;_LLD8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1F5=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F5->tag != 2)goto _LLDA;else{_tmp1F6=_tmp1F5->f2;}}_LLD9:
 s=Cyc_Tcpat_compile_pat(r,_tmp1F6);goto _LLC9;_LLDA: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1F7=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F7->tag != 5)goto _LLDC;else{_tmp1F8=_tmp1F7->f1;}}_LLDB:
# 902
{void*_tmp20E=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp20F=_tmp20E;union Cyc_Absyn_Constraint*_tmp211;_LLEF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20F;if(_tmp210->tag != 5)goto _LLF1;else{_tmp211=((_tmp210->f1).ptr_atts).nullable;}}_LLF0: {
# 904
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp212=_tmp211;union Cyc_Absyn_Constraint*_tmp213;int _tmp214;_LLF4: if((_tmp212->Forward_constr).tag != 2)goto _LLF6;_tmp213=(union Cyc_Absyn_Constraint*)(_tmp212->Forward_constr).val;_LLF5:
# 909
*_tmp211=*_tmp213;
continue;_LLF6: if((_tmp212->No_constr).tag != 3)goto _LLF8;_LLF7:
# 912
{struct _union_Constraint_Eq_constr*_tmp4EE;(_tmp4EE=& _tmp211->Eq_constr,((_tmp4EE->tag=1,_tmp4EE->val=0)));}
is_nullable=0;
still_working=0;
goto _LLF3;_LLF8: if((_tmp212->Eq_constr).tag != 1)goto _LLF3;_tmp214=(int)(_tmp212->Eq_constr).val;_LLF9:
# 917
 is_nullable=_tmp214;
still_working=0;
goto _LLF3;_LLF3:;}{
# 922
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1F8);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 926
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLEE;};}_LLF1:;_LLF2: {
const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F1="expecting pointertype for pattern!",_tag_dyneither(_tmp4F1,sizeof(char),35))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}_LLEE:;}
# 930
goto _LLC9;_LLDC: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1F9=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1F9->tag != 7)goto _LLDE;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=_tmp1F9->f2;_tmp1FC=_tmp1F9->f3;}}_LLDD: {
# 932
int*span;
{void*_tmp218=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp219=_tmp218;_LLFB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp219;if(_tmp21A->tag != 3)goto _LLFD;}_LLFC:
# 935
 if(_tmp1FA->is_extensible)
span=0;else{
# 938
int*_tmp4F2;span=((_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2)),((_tmp4F2[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1FA->fields))->v),_tmp4F2))));}
goto _LLFA;_LLFD: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp219;if(_tmp21B->tag != 4)goto _LLFF;}_LLFE:
 span=& Cyc_Tcpat_one_opt;goto _LLFA;_LLFF:;_LL100:
{const char*_tmp4F5;void*_tmp4F4;span=((_tmp4F4=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F5="void datatype pattern has bad type",_tag_dyneither(_tmp4F5,sizeof(char),35))),_tag_dyneither(_tmp4F4,sizeof(void*),0))));}goto _LLFA;_LLFA:;}
# 943
s=Cyc_Tcpat_con_pat(r,*(*_tmp1FB->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1FC),p);
goto _LLC9;}_LLDE: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1FD=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1FD->tag != 4)goto _LLE0;else{_tmp1FE=_tmp1FD->f1;}}_LLDF:
# 947
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1FE),p);goto _LLC9;_LLE0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1FF=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp1FF->tag != 6)goto _LLE2;else{if(_tmp1FF->f1 == 0)goto _LLE2;if((((_tmp1FF->f1)->aggr_info).KnownAggr).tag != 2)goto _LLE2;_tmp200=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1FF->f1)->aggr_info).KnownAggr).val);_tmp201=_tmp1FF->f3;}}_LLE1:
# 952
 if(_tmp200->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp200->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 956
const char*_tmp4F6;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4F6="",_tag_dyneither(_tmp4F6,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp201;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp21F=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Pat*_tmp222;struct _tuple15*_tmp220=_tmp21F;_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;{
struct Cyc_List_List*_tmp223=_tmp221;struct _dyneither_ptr*_tmp225;_LL102: if(_tmp223 == 0)goto _LL104;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp224=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp223->hd);if(_tmp224->tag != 1)goto _LL104;else{_tmp225=_tmp224->f1;}}if(_tmp223->tl != 0)goto _LL104;_LL103:
# 961
 if(Cyc_strptrcmp(_tmp225,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4F7;ps=((_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7->hd=Cyc_Tcpat_compile_pat(r,_tmp222),((_tmp4F7->tl=ps,_tmp4F7))))));}
found=1;}
# 965
goto _LL101;_LL104:;_LL105: {
const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FA="bad designator(s)",_tag_dyneither(_tmp4FA,sizeof(char),18))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}_LL101:;};}}
# 969
if(!found){
const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FD="bad designator",_tag_dyneither(_tmp4FD,sizeof(char),15))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}}}
# 972
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 975
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp200->impl))->tagged){
const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_Tcutil_terr(p->loc,((_tmp500="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp500,sizeof(char),47))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}{
int*_tmp501;int*span=(_tmp501=_region_malloc(r,sizeof(*_tmp501)),((_tmp501[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp200->impl))->fields),_tmp501)));
struct Cyc_List_List*_tmp22E=_tmp201;struct _dyneither_ptr*_tmp230;struct Cyc_Absyn_Pat*_tmp231;_LL107: if(_tmp22E == 0)goto _LL109;if(((struct _tuple15*)_tmp22E->hd)->f1 == 0)goto _LL109;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp22F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp22E->hd)->f1)->hd);if(_tmp22F->tag != 1)goto _LL109;else{_tmp230=_tmp22F->f1;}}if((((struct _tuple15*)_tmp22E->hd)->f1)->tl != 0)goto _LL109;_tmp231=((struct _tuple15*)_tmp22E->hd)->f2;if(_tmp22E->tl != 0)goto _LL109;_LL108:
# 980
{struct Cyc_List_List*_tmp502;s=Cyc_Tcpat_con_pat(r,*_tmp230,span,((_tmp502=_region_malloc(r,sizeof(*_tmp502)),((_tmp502->hd=Cyc_Tcpat_compile_pat(r,_tmp231),((_tmp502->tl=0,_tmp502)))))),p);}
goto _LL106;_LL109:;_LL10A: {
const char*_tmp505;void*_tmp504;(_tmp504=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp505="bad union pattern",_tag_dyneither(_tmp505,sizeof(char),18))),_tag_dyneither(_tmp504,sizeof(void*),0)));}_LL106:;};}
# 985
goto _LLC9;_LLE2: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp202=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp202->tag != 12)goto _LLE4;else{_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;}}_LLE3:
# 990
{void*_tmp236=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp237=_tmp236;_LL10C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp237;if(_tmp238->tag != 6)goto _LL10E;}_LL10D:
# 994
 s=Cyc_Tcpat_con_pat(r,*(*_tmp204->name).f2,0,0,p);
goto _LL10B;_LL10E:;_LL10F: {
# 997
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp203->fields))->v);
{int*_tmp506;s=Cyc_Tcpat_con_pat(r,*(*_tmp204->name).f2,((_tmp506=_region_malloc(r,sizeof(*_tmp506)),((_tmp506[0]=span,_tmp506)))),0,p);}
goto _LL10B;}_LL10B:;}
# 1001
goto _LLC9;_LLE4: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp205=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp205->tag != 13)goto _LLE6;else{_tmp206=(void*)_tmp205->f1;_tmp207=_tmp205->f2;}}_LLE5: {
# 1006
struct Cyc_List_List*fields;
{void*_tmp23A=Cyc_Tcutil_compress(_tmp206);void*_tmp23B=_tmp23A;struct Cyc_List_List*_tmp23D;_LL111: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp23B;if(_tmp23C->tag != 14)goto _LL113;else{_tmp23D=_tmp23C->f1;}}_LL112:
 fields=_tmp23D;goto _LL110;_LL113:;_LL114: {
const char*_tmp509;void*_tmp508;(_tmp508=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp509="bad type in AnonEnum_p",_tag_dyneither(_tmp509,sizeof(char),23))),_tag_dyneither(_tmp508,sizeof(void*),0)));}_LL110:;}
# 1013
{void*_tmp240=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp241=_tmp240;_LL116: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp241;if(_tmp242->tag != 6)goto _LL118;}_LL117:
# 1017
 s=Cyc_Tcpat_con_pat(r,*(*_tmp207->name).f2,0,0,p);
goto _LL115;_LL118:;_LL119: {
# 1020
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp50A;s=Cyc_Tcpat_con_pat(r,*(*_tmp207->name).f2,((_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A[0]=span,_tmp50A)))),0,p);}
goto _LL115;}_LL115:;}
# 1024
goto _LLC9;}_LLE6: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp208=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp208->tag != 14)goto _LLE8;}_LLE7:
 goto _LLE9;_LLE8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp209=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp209->tag != 15)goto _LLEA;}_LLE9:
 goto _LLEB;_LLEA: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp20A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp20A->tag != 6)goto _LLEC;}_LLEB:
 goto _LLED;_LLEC: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp20B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1E8;if(_tmp20B->tag != 16)goto _LLC9;}_LLED: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp50D;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp50C;s=(void*)((_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C[0]=((_tmp50D.tag=0,_tmp50D)),_tmp50C))));}_LLC9:;}
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
const char*_tmp510;void*_tmp50F;(_tmp50F=0,Cyc_fprintf(Cyc_stderr,((_tmp510=" ",_tag_dyneither(_tmp510,sizeof(char),2))),_tag_dyneither(_tmp50F,sizeof(void*),0)));}}
# 1088
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp248=c->name;
union Cyc_Tcpat_Name_value _tmp249=_tmp248;struct _dyneither_ptr _tmp24A;int _tmp24B;_LL11B: if((_tmp249.Name_v).tag != 1)goto _LL11D;_tmp24A=(struct _dyneither_ptr)(_tmp249.Name_v).val;_LL11C:
{const char*_tmp514;void*_tmp513[1];struct Cyc_String_pa_PrintArg_struct _tmp512;(_tmp512.tag=0,((_tmp512.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp24A),((_tmp513[0]=& _tmp512,Cyc_fprintf(Cyc_stderr,((_tmp514="%s",_tag_dyneither(_tmp514,sizeof(char),3))),_tag_dyneither(_tmp513,sizeof(void*),1)))))));}goto _LL11A;_LL11D: if((_tmp249.Int_v).tag != 2)goto _LL11A;_tmp24B=(int)(_tmp249.Int_v).val;_LL11E:
{const char*_tmp518;void*_tmp517[1];struct Cyc_Int_pa_PrintArg_struct _tmp516;(_tmp516.tag=1,((_tmp516.f1=(unsigned long)_tmp24B,((_tmp517[0]=& _tmp516,Cyc_fprintf(Cyc_stderr,((_tmp518="%d",_tag_dyneither(_tmp518,sizeof(char),3))),_tag_dyneither(_tmp517,sizeof(void*),1)))))));}goto _LL11A;_LL11A:;}
# 1096
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp252=d;void*_tmp254;struct Cyc_List_List*_tmp257;struct Cyc_Tcpat_Con_s*_tmp258;void*_tmp259;void*_tmp25A;_LL120: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp253=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp252;if(_tmp253->tag != 1)goto _LL122;else{_tmp254=(void*)_tmp253->f1;}}_LL121:
# 1099
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp51B;void*_tmp51A;(_tmp51A=0,Cyc_fprintf(Cyc_stderr,((_tmp51B="Success(",_tag_dyneither(_tmp51B,sizeof(char),9))),_tag_dyneither(_tmp51A,sizeof(void*),0)));}print_rhs(_tmp254);{const char*_tmp51E;void*_tmp51D;(_tmp51D=0,Cyc_fprintf(Cyc_stderr,((_tmp51E=")\n",_tag_dyneither(_tmp51E,sizeof(char),3))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}
goto _LL11F;_LL122: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp255=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp252;if(_tmp255->tag != 0)goto _LL124;}_LL123:
{const char*_tmp521;void*_tmp520;(_tmp520=0,Cyc_fprintf(Cyc_stderr,((_tmp521="Failure\n",_tag_dyneither(_tmp521,sizeof(char),9))),_tag_dyneither(_tmp520,sizeof(void*),0)));}goto _LL11F;_LL124: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp256=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp252;if(_tmp256->tag != 2)goto _LL11F;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;_tmp259=(void*)_tmp256->f3;_tmp25A=(void*)_tmp256->f4;}}_LL125:
# 1104
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp524;void*_tmp523;(_tmp523=0,Cyc_fprintf(Cyc_stderr,((_tmp524="Access[",_tag_dyneither(_tmp524,sizeof(char),8))),_tag_dyneither(_tmp523,sizeof(void*),0)));}
for(0;_tmp257 != 0;_tmp257=_tmp257->tl){
{const char*_tmp528;void*_tmp527[1];struct Cyc_Int_pa_PrintArg_struct _tmp526;(_tmp526.tag=1,((_tmp526.f1=(unsigned long)((int)_tmp257->hd),((_tmp527[0]=& _tmp526,Cyc_fprintf(Cyc_stderr,((_tmp528="%d",_tag_dyneither(_tmp528,sizeof(char),3))),_tag_dyneither(_tmp527,sizeof(void*),1)))))));}
if(_tmp257->tl != 0){const char*_tmp52B;void*_tmp52A;(_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B=",",_tag_dyneither(_tmp52B,sizeof(char),2))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}}
# 1110
{const char*_tmp52E;void*_tmp52D;(_tmp52D=0,Cyc_fprintf(Cyc_stderr,((_tmp52E="],",_tag_dyneither(_tmp52E,sizeof(char),3))),_tag_dyneither(_tmp52D,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp258);
{const char*_tmp531;void*_tmp530;(_tmp530=0,Cyc_fprintf(Cyc_stderr,((_tmp531=",\n",_tag_dyneither(_tmp531,sizeof(char),3))),_tag_dyneither(_tmp530,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp259,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp25A,print_rhs,tab + 7);_LL11F:;}struct _tuple20{int f1;unsigned int f2;};
# 1118
static void Cyc_Tcpat_print_swrhs(struct _tuple20*x){
return;}
# 1126
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp26C=td;struct Cyc_Set_Set*_tmp26E;_LL127: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp26D=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp26C;if(_tmp26D->tag != 1)goto _LL129;else{_tmp26E=_tmp26D->f1;}}_LL128: {
# 1136
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp534;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp533;return(void*)((_tmp533=_region_malloc(r,sizeof(*_tmp533)),((_tmp533[0]=((_tmp534.tag=1,((_tmp534.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp26E,c),_tmp534)))),_tmp533))));}_LL129: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp26F=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp26C;if(_tmp26F->tag != 0)goto _LL126;}_LL12A: {
const char*_tmp537;void*_tmp536;(_tmp536=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp537="add_neg called when td is Positive",_tag_dyneither(_tmp537,sizeof(char),35))),_tag_dyneither(_tmp536,sizeof(void*),0)));}_LL126:;}
# 1143
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp274=td;struct Cyc_Tcpat_Con_s*_tmp276;struct Cyc_Set_Set*_tmp278;_LL12C: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp275=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp274;if(_tmp275->tag != 0)goto _LL12E;else{_tmp276=_tmp275->f1;}}_LL12D:
# 1147
 if(Cyc_Tcpat_compare_con(c,_tmp276)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL12E: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp277=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp274;if(_tmp277->tag != 1)goto _LL12B;else{_tmp278=_tmp277->f1;}}_LL12F:
# 1151
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp278,c))return Cyc_Tcpat_No;else{
# 1154
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp278)+ 1)
return Cyc_Tcpat_Yes;else{
# 1157
return Cyc_Tcpat_Maybe;}}_LL12B:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1165
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1167
struct Cyc_List_List*_tmp279=ctxt;struct Cyc_Tcpat_Con_s*_tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27C;_LL131: if(_tmp279 != 0)goto _LL133;_LL132:
 return 0;_LL133: if(_tmp279 == 0)goto _LL130;_tmp27A=((struct _tuple21*)_tmp279->hd)->f1;_tmp27B=((struct _tuple21*)_tmp279->hd)->f2;_tmp27C=_tmp279->tl;_LL134: {
# 1170
struct _tuple21*_tmp53D;struct Cyc_List_List*_tmp53C;struct Cyc_List_List*_tmp53B;return(_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B->hd=(
(_tmp53D=_region_malloc(r,sizeof(*_tmp53D)),((_tmp53D->f1=_tmp27A,((_tmp53D->f2=((_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C->hd=dsc,((_tmp53C->tl=_tmp27B,_tmp53C)))))),_tmp53D)))))),((_tmp53B->tl=_tmp27C,_tmp53B)))));}_LL130:;}
# 1178
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp280=ctxt;struct Cyc_Tcpat_Con_s*_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp283;_LL136: if(_tmp280 != 0)goto _LL138;_LL137: {
const char*_tmp540;void*_tmp53F;(_tmp53F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp540="norm_context: empty context",_tag_dyneither(_tmp540,sizeof(char),28))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}_LL138: if(_tmp280 == 0)goto _LL135;_tmp281=((struct _tuple21*)_tmp280->hd)->f1;_tmp282=((struct _tuple21*)_tmp280->hd)->f2;_tmp283=_tmp280->tl;_LL139: {
# 1182
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp543;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp542;return Cyc_Tcpat_augment(r,_tmp283,(void*)((_tmp542=_region_malloc(r,sizeof(*_tmp542)),((_tmp542[0]=((_tmp543.tag=0,((_tmp543.f1=_tmp281,((_tmp543.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp282),_tmp543)))))),_tmp542)))));}_LL135:;}
# 1191
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1193
struct _tuple1 _tmp544;struct _tuple1 _tmp288=(_tmp544.f1=ctxt,((_tmp544.f2=work,_tmp544)));struct _tuple1 _tmp289=_tmp288;struct Cyc_Tcpat_Con_s*_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_List_List*_tmp28C;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28E;_LL13B: if(_tmp289.f1 != 0)goto _LL13D;if(_tmp289.f2 != 0)goto _LL13D;_LL13C:
 return dsc;_LL13D: if(_tmp289.f1 != 0)goto _LL13F;_LL13E:
 goto _LL140;_LL13F: if(_tmp289.f2 != 0)goto _LL141;_LL140: {
const char*_tmp547;void*_tmp546;(_tmp546=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp547="build_desc: ctxt and work don't match",_tag_dyneither(_tmp547,sizeof(char),38))),_tag_dyneither(_tmp546,sizeof(void*),0)));}_LL141: if(_tmp289.f1 == 0)goto _LL13A;_tmp28A=((struct _tuple21*)(_tmp289.f1)->hd)->f1;_tmp28B=((struct _tuple21*)(_tmp289.f1)->hd)->f2;_tmp28C=(_tmp289.f1)->tl;if(_tmp289.f2 == 0)goto _LL13A;_tmp28D=((struct _tuple19*)(_tmp289.f2)->hd)->f3;_tmp28E=(_tmp289.f2)->tl;_LL142: {
# 1198
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp54D;struct Cyc_List_List*_tmp54C;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp54B;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp291=(_tmp54B=_region_malloc(r,sizeof(*_tmp54B)),((_tmp54B[0]=((_tmp54D.tag=0,((_tmp54D.f1=_tmp28A,((_tmp54D.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp28B),(
(_tmp54C=_region_malloc(r,sizeof(*_tmp54C)),((_tmp54C->hd=dsc,((_tmp54C->tl=_tmp28D,_tmp54C))))))),_tmp54D)))))),_tmp54B)));
return Cyc_Tcpat_build_desc(r,_tmp28C,(void*)_tmp291,_tmp28E);}_LL13A:;}
# 1204
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1211
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1213
struct Cyc_List_List*_tmp296=allmrules;void*_tmp297;void*_tmp298;struct Cyc_List_List*_tmp299;_LL144: if(_tmp296 != 0)goto _LL146;_LL145: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp550;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp54F;return(void*)((_tmp54F=_cycalloc(sizeof(*_tmp54F)),((_tmp54F[0]=((_tmp550.tag=0,((_tmp550.f1=dsc,_tmp550)))),_tmp54F))));}_LL146: if(_tmp296 == 0)goto _LL143;_tmp297=((struct _tuple0*)_tmp296->hd)->f1;_tmp298=((struct _tuple0*)_tmp296->hd)->f2;_tmp299=_tmp296->tl;_LL147:
# 1216
 return Cyc_Tcpat_match(r,_tmp297,0,dsc,0,0,_tmp298,_tmp299);_LL143:;}
# 1221
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1223
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp553;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp552;return Cyc_Tcpat_or_match(r,(void*)((_tmp552=_region_malloc(r,sizeof(*_tmp552)),((_tmp552[0]=((_tmp553.tag=1,((_tmp553.f1=Cyc_Tcpat_empty_con_set(r),_tmp553)))),_tmp552)))),allmrules);}
# 1229
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1233
struct Cyc_List_List*_tmp29E=work;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*_tmp2A3;_LL149: if(_tmp29E != 0)goto _LL14B;_LL14A: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp556;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp555;return(void*)((_tmp555=_region_malloc(r,sizeof(*_tmp555)),((_tmp555[0]=((_tmp556.tag=1,((_tmp556.f1=right_hand_side,_tmp556)))),_tmp555))));}_LL14B: if(_tmp29E == 0)goto _LL14D;if(((struct _tuple19*)_tmp29E->hd)->f1 != 0)goto _LL14D;if(((struct _tuple19*)_tmp29E->hd)->f2 != 0)goto _LL14D;if(((struct _tuple19*)_tmp29E->hd)->f3 != 0)goto _LL14D;_tmp29F=_tmp29E->tl;_LL14C:
# 1236
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp29F,right_hand_side,rules);_LL14D: if(_tmp29E == 0)goto _LL148;_tmp2A0=((struct _tuple19*)_tmp29E->hd)->f1;_tmp2A1=((struct _tuple19*)_tmp29E->hd)->f2;_tmp2A2=((struct _tuple19*)_tmp29E->hd)->f3;_tmp2A3=_tmp29E->tl;_LL14E:
# 1238
 if((_tmp2A0 == 0  || _tmp2A1 == 0) || _tmp2A2 == 0){
const char*_tmp559;void*_tmp558;(_tmp558=0,Cyc_Tcutil_impos(((_tmp559="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp559,sizeof(char),38))),_tag_dyneither(_tmp558,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2A8=_tmp2A0;void*_tmp2AA;struct Cyc_List_List*_tmp2AB;struct Cyc_List_List*_tmp2A9=_tmp2A8;_tmp2AA=(void*)_tmp2A9->hd;_tmp2AB=_tmp2A9->tl;{
struct Cyc_List_List*_tmp2AC=_tmp2A1;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_List_List*_tmp2AD=_tmp2AC;_tmp2AE=(struct Cyc_List_List*)_tmp2AD->hd;_tmp2AF=_tmp2AD->tl;{
struct Cyc_List_List*_tmp2B0=_tmp2A2;void*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B1=_tmp2B0;_tmp2B2=(void*)_tmp2B1->hd;_tmp2B3=_tmp2B1->tl;{
struct _tuple19*_tmp55A;struct _tuple19*_tmp2B4=(_tmp55A=_region_malloc(r,sizeof(*_tmp55A)),((_tmp55A->f1=_tmp2AB,((_tmp55A->f2=_tmp2AF,((_tmp55A->f3=_tmp2B3,_tmp55A)))))));
struct Cyc_List_List*_tmp55B;return Cyc_Tcpat_match(r,_tmp2AA,_tmp2AE,_tmp2B2,ctx,((_tmp55B=_region_malloc(r,sizeof(*_tmp55B)),((_tmp55B->hd=_tmp2B4,((_tmp55B->tl=_tmp2A3,_tmp55B)))))),right_hand_side,rules);};};};};_LL148:;}
# 1250
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1252
void*_tmp2B7=dsc;struct Cyc_Set_Set*_tmp2B9;struct Cyc_List_List*_tmp2BB;_LL150: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2B8=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B7;if(_tmp2B8->tag != 1)goto _LL152;else{_tmp2B9=_tmp2B8->f1;}}_LL151: {
# 1257
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp55E;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp55D;void*any=(void*)((_tmp55D=_region_malloc(r,sizeof(*_tmp55D)),((_tmp55D[0]=((_tmp55E.tag=1,((_tmp55E.f1=Cyc_Tcpat_empty_con_set(r),_tmp55E)))),_tmp55D))));
struct Cyc_List_List*_tmp2BC=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp55F;_tmp2BC=((_tmp55F=_region_malloc(r,sizeof(*_tmp55F)),((_tmp55F->hd=any,((_tmp55F->tl=_tmp2BC,_tmp55F))))));}}
return _tmp2BC;}_LL152: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BA=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B7;if(_tmp2BA->tag != 0)goto _LL14F;else{_tmp2BB=_tmp2BA->f2;}}_LL153:
 return _tmp2BB;_LL14F:;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1267
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple22*env,int i){
struct _RegionHandle*_tmp2C1;struct Cyc_List_List*_tmp2C2;struct _tuple22*_tmp2C0=env;_tmp2C1=_tmp2C0->f1;_tmp2C2=_tmp2C0->f2;{
struct Cyc_List_List*_tmp560;return(_tmp560=_region_malloc(_tmp2C1,sizeof(*_tmp560)),((_tmp560->hd=(void*)(i + 1),((_tmp560->tl=_tmp2C2,_tmp560)))));};}
# 1271
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1273
struct _tuple22 _tmp561;struct _tuple22 _tmp2C4=(_tmp561.f1=r,((_tmp561.f2=obj,_tmp561)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple22*,int),struct _tuple22*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp2C4);}
# 1280
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1284
void*_tmp2C6=p;struct Cyc_Tcpat_Con_s*_tmp2C9;struct Cyc_List_List*_tmp2CA;_LL155: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2C7=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2C6;if(_tmp2C7->tag != 0)goto _LL157;}_LL156:
# 1286
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL157: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2C8=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2C6;if(_tmp2C8->tag != 1)goto _LL154;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;}}_LL158: {
# 1288
enum Cyc_Tcpat_Answer _tmp2CB=Cyc_Tcpat_static_match(_tmp2C9,dsc);switch(_tmp2CB){case Cyc_Tcpat_Yes: _LL159: {
# 1290
struct _tuple21*_tmp564;struct Cyc_List_List*_tmp563;struct Cyc_List_List*ctx2=(_tmp563=_region_malloc(r,sizeof(*_tmp563)),((_tmp563->hd=((_tmp564=_region_malloc(r,sizeof(*_tmp564)),((_tmp564->f1=_tmp2C9,((_tmp564->f2=0,_tmp564)))))),((_tmp563->tl=ctx,_tmp563)))));
struct _tuple19*_tmp565;struct _tuple19*work_frame=(_tmp565=_region_malloc(r,sizeof(*_tmp565)),((_tmp565->f1=_tmp2CA,((_tmp565->f2=Cyc_Tcpat_getoargs(r,_tmp2C9,obj),((_tmp565->f3=
Cyc_Tcpat_getdargs(r,_tmp2C9,dsc),_tmp565)))))));
struct Cyc_List_List*_tmp566;struct Cyc_List_List*work2=(_tmp566=_region_malloc(r,sizeof(*_tmp566)),((_tmp566->hd=work_frame,((_tmp566->tl=work,_tmp566)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL15A:
# 1296
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL15B: {
# 1298
struct _tuple21*_tmp569;struct Cyc_List_List*_tmp568;struct Cyc_List_List*ctx2=(_tmp568=_region_malloc(r,sizeof(*_tmp568)),((_tmp568->hd=((_tmp569=_region_malloc(r,sizeof(*_tmp569)),((_tmp569->f1=_tmp2C9,((_tmp569->f2=0,_tmp569)))))),((_tmp568->tl=ctx,_tmp568)))));
struct _tuple19*_tmp56A;struct _tuple19*work_frame=(_tmp56A=_region_malloc(r,sizeof(*_tmp56A)),((_tmp56A->f1=_tmp2CA,((_tmp56A->f2=Cyc_Tcpat_getoargs(r,_tmp2C9,obj),((_tmp56A->f3=
Cyc_Tcpat_getdargs(r,_tmp2C9,dsc),_tmp56A)))))));
struct Cyc_List_List*_tmp56B;struct Cyc_List_List*work2=(_tmp56B=_region_malloc(r,sizeof(*_tmp56B)),((_tmp56B->hd=work_frame,((_tmp56B->tl=work,_tmp56B)))));
void*_tmp2D0=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2D1=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2C9),work),rules);
# 1305
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp56E;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp56D;return(void*)((_tmp56D=_region_malloc(r,sizeof(*_tmp56D)),((_tmp56D[0]=((_tmp56E.tag=2,((_tmp56E.f1=obj,((_tmp56E.f2=_tmp2C9,((_tmp56E.f3=_tmp2D0,((_tmp56E.f4=_tmp2D1,_tmp56E)))))))))),_tmp56D))));}}}_LL154:;}
# 1315
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1321
void*_tmp2D8=d;void*_tmp2DA;void*_tmp2DC;void*_tmp2DE;void*_tmp2DF;_LL15E: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2D9=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2D8;if(_tmp2D9->tag != 0)goto _LL160;else{_tmp2DA=(void*)_tmp2D9->f1;}}_LL15F:
# 1323
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2DA);*exhaust_done=1;}
goto _LL15D;_LL160: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2DB=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2D8;if(_tmp2DB->tag != 1)goto _LL162;else{_tmp2DC=(void*)_tmp2DB->f1;}}_LL161:
 rhs_appears(env2,_tmp2DC);goto _LL15D;_LL162: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2DD=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2D8;if(_tmp2DD->tag != 2)goto _LL15D;else{_tmp2DE=(void*)_tmp2DD->f3;_tmp2DF=(void*)_tmp2DD->f4;}}_LL163:
# 1327
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2DE,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1329
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2DF,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1331
goto _LL15D;_LL15D:;}struct _tuple23{void*f1;struct _tuple20*f2;};struct _tuple24{struct _RegionHandle*f1;int*f2;};
# 1339
static struct _tuple23*Cyc_Tcpat_get_match(struct _tuple24*env,struct Cyc_Absyn_Switch_clause*swc){
# 1342
struct _RegionHandle*_tmp2E1;int*_tmp2E2;struct _tuple24*_tmp2E0=env;_tmp2E1=_tmp2E0->f1;_tmp2E2=_tmp2E0->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2E1,swc->pattern);
struct _tuple20*_tmp56F;struct _tuple20*rhs=(_tmp56F=_region_malloc(_tmp2E1,sizeof(*_tmp56F)),((_tmp56F->f1=0,((_tmp56F->f2=(swc->pattern)->loc,_tmp56F)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp572;struct Cyc_List_List*_tmp571;sp=Cyc_Tcpat_tuple_pat(_tmp2E1,((_tmp571=_region_malloc(_tmp2E1,sizeof(*_tmp571)),((_tmp571->hd=sp0,((_tmp571->tl=(
(_tmp572=_region_malloc(_tmp2E1,sizeof(*_tmp572)),((_tmp572->hd=Cyc_Tcpat_int_pat(_tmp2E1,*_tmp2E2,0),((_tmp572->tl=0,_tmp572)))))),_tmp571)))))),0);}
*_tmp2E2=*_tmp2E2 + 1;}else{
# 1351
struct Cyc_List_List*_tmp57C;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp57B;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp57A;struct Cyc_List_List*_tmp579;sp=Cyc_Tcpat_tuple_pat(_tmp2E1,((_tmp579=_region_malloc(_tmp2E1,sizeof(*_tmp579)),((_tmp579->hd=sp0,((_tmp579->tl=((_tmp57C=_region_malloc(_tmp2E1,sizeof(*_tmp57C)),((_tmp57C->hd=(void*)((_tmp57A=_region_malloc(_tmp2E1,sizeof(*_tmp57A)),((_tmp57A[0]=((_tmp57B.tag=0,_tmp57B)),_tmp57A)))),((_tmp57C->tl=0,_tmp57C)))))),_tmp579)))))),0);}{
struct _tuple23*_tmp57D;return(_tmp57D=_region_malloc(_tmp2E1,sizeof(*_tmp57D)),((_tmp57D->f1=sp,((_tmp57D->f2=rhs,_tmp57D)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1357
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1359
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2EC=d;struct Cyc_Tcpat_Con_s*_tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_Set_Set*_tmp2F1;_LL165: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2ED=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2EC;if(_tmp2ED->tag != 0)goto _LL167;else{_tmp2EE=_tmp2ED->f1;_tmp2EF=_tmp2ED->f2;}}_LL166: {
# 1363
union Cyc_Tcpat_Name_value _tmp2F2=_tmp2EE->name;
struct Cyc_Absyn_Pat*_tmp2F3=_tmp2EE->orig_pat;
if(_tmp2F3 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2EF))->hd);{
# 1368
void*_tmp2F4=_tmp2F3->r;void*_tmp2F5=_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F8;struct Cyc_Absyn_Vardecl*_tmp2FA;struct Cyc_Absyn_Tvar*_tmp2FC;struct Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Aggrdecl*_tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Absyn_Datatypefield*_tmp304;int _tmp307;char _tmp309;struct _dyneither_ptr _tmp30B;int _tmp30C;struct Cyc_Absyn_Enumfield*_tmp30E;struct Cyc_Absyn_Enumfield*_tmp310;struct Cyc_Absyn_Exp*_tmp312;_LL16A: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2F6=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto _LL16C;}_LL16B: {
const char*_tmp57E;return(_tmp57E="_",_tag_dyneither(_tmp57E,sizeof(char),2));}_LL16C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2F7=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2F7->tag != 1)goto _LL16E;else{_tmp2F8=_tmp2F7->f1;}}_LL16D:
 return Cyc_Absynpp_qvar2string(_tmp2F8->name);_LL16E: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2F9=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2F9->tag != 2)goto _LL170;else{_tmp2FA=_tmp2F9->f1;}}_LL16F: {
const char*_tmp582;void*_tmp581[1];struct Cyc_String_pa_PrintArg_struct _tmp580;return(struct _dyneither_ptr)((_tmp580.tag=0,((_tmp580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FA->name)),((_tmp581[0]=& _tmp580,Cyc_aprintf(((_tmp582="*%s",_tag_dyneither(_tmp582,sizeof(char),4))),_tag_dyneither(_tmp581,sizeof(void*),1))))))));}_LL170: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2FB=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2FB->tag != 3)goto _LL172;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;}}_LL171: {
const char*_tmp587;void*_tmp586[2];struct Cyc_String_pa_PrintArg_struct _tmp585;struct Cyc_String_pa_PrintArg_struct _tmp584;return(struct _dyneither_ptr)((_tmp584.tag=0,((_tmp584.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2FC->name),((_tmp585.tag=0,((_tmp585.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2FD->name)),((_tmp586[0]=& _tmp585,((_tmp586[1]=& _tmp584,Cyc_aprintf(((_tmp587="%s<`%s>",_tag_dyneither(_tmp587,sizeof(char),8))),_tag_dyneither(_tmp586,sizeof(void*),2))))))))))))));}_LL172: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2FE=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2FE->tag != 4)goto _LL174;}_LL173: {
# 1374
const char*_tmp58B;void*_tmp58A[1];struct Cyc_String_pa_PrintArg_struct _tmp589;return(struct _dyneither_ptr)((_tmp589.tag=0,((_tmp589.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2EF)),((_tmp58A[0]=& _tmp589,Cyc_aprintf(((_tmp58B="$(%s)",_tag_dyneither(_tmp58B,sizeof(char),6))),_tag_dyneither(_tmp58A,sizeof(void*),1))))))));}_LL174: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2FF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp2FF->tag != 5)goto _LL176;}_LL175: {
# 1376
const char*_tmp58F;void*_tmp58E[1];struct Cyc_String_pa_PrintArg_struct _tmp58D;return(struct _dyneither_ptr)((_tmp58D.tag=0,((_tmp58D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2EF)),((_tmp58E[0]=& _tmp58D,Cyc_aprintf(((_tmp58F="&%s",_tag_dyneither(_tmp58F,sizeof(char),4))),_tag_dyneither(_tmp58E,sizeof(void*),1))))))));}_LL176: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp300=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp300->tag != 6)goto _LL178;else{if(_tmp300->f1 == 0)goto _LL178;if((((_tmp300->f1)->aggr_info).KnownAggr).tag != 2)goto _LL178;_tmp301=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp300->f1)->aggr_info).KnownAggr).val);_tmp302=_tmp300->f3;}}_LL177:
# 1378
 if(_tmp301->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp321=_tmp302;struct _dyneither_ptr*_tmp323;_LL18B: if(_tmp321 == 0)goto _LL18D;if(((struct _tuple15*)_tmp321->hd)->f1 == 0)goto _LL18D;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp322=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp321->hd)->f1)->hd);if(_tmp322->tag != 1)goto _LL18D;else{_tmp323=_tmp322->f1;}};_LL18C: {
# 1381
const char*_tmp595;void*_tmp594[3];struct Cyc_String_pa_PrintArg_struct _tmp593;struct Cyc_String_pa_PrintArg_struct _tmp592;struct Cyc_String_pa_PrintArg_struct _tmp591;return(struct _dyneither_ptr)((_tmp591.tag=0,((_tmp591.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2EF)),((_tmp592.tag=0,((_tmp592.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp323),((_tmp593.tag=0,((_tmp593.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1381
Cyc_Absynpp_qvar2string(_tmp301->name)),((_tmp594[0]=& _tmp593,((_tmp594[1]=& _tmp592,((_tmp594[2]=& _tmp591,Cyc_aprintf(((_tmp595="%s{.%s = %s}",_tag_dyneither(_tmp595,sizeof(char),13))),_tag_dyneither(_tmp594,sizeof(void*),3))))))))))))))))))));}_LL18D:;_LL18E:
# 1383
 goto _LL18A;_LL18A:;}{
# 1386
const char*_tmp59A;void*_tmp599[2];struct Cyc_String_pa_PrintArg_struct _tmp598;struct Cyc_String_pa_PrintArg_struct _tmp597;return(struct _dyneither_ptr)((_tmp597.tag=0,((_tmp597.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2EF)),((_tmp598.tag=0,((_tmp598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp301->name)),((_tmp599[0]=& _tmp598,((_tmp599[1]=& _tmp597,Cyc_aprintf(((_tmp59A="%s{%s}",_tag_dyneither(_tmp59A,sizeof(char),7))),_tag_dyneither(_tmp599,sizeof(void*),2))))))))))))));};_LL178: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp303=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp303->tag != 7)goto _LL17A;else{_tmp304=_tmp303->f2;}}_LL179:
# 1388
 if(_tmp2EF == 0){
const char*_tmp59E;void*_tmp59D[1];struct Cyc_String_pa_PrintArg_struct _tmp59C;return(struct _dyneither_ptr)((_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp304->name)),((_tmp59D[0]=& _tmp59C,Cyc_aprintf(((_tmp59E="%s",_tag_dyneither(_tmp59E,sizeof(char),3))),_tag_dyneither(_tmp59D,sizeof(void*),1))))))));}else{
# 1391
const char*_tmp5A3;void*_tmp5A2[2];struct Cyc_String_pa_PrintArg_struct _tmp5A1;struct Cyc_String_pa_PrintArg_struct _tmp5A0;return(struct _dyneither_ptr)((_tmp5A0.tag=0,((_tmp5A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2EF)),((_tmp5A1.tag=0,((_tmp5A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp304->name)),((_tmp5A2[0]=& _tmp5A1,((_tmp5A2[1]=& _tmp5A0,Cyc_aprintf(((_tmp5A3="%s(%s)",_tag_dyneither(_tmp5A3,sizeof(char),7))),_tag_dyneither(_tmp5A2,sizeof(void*),2))))))))))))));}_LL17A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp305=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp305->tag != 8)goto _LL17C;}_LL17B: {
const char*_tmp5A4;return(_tmp5A4="NULL",_tag_dyneither(_tmp5A4,sizeof(char),5));}_LL17C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp306=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp306->tag != 9)goto _LL17E;else{_tmp307=_tmp306->f2;}}_LL17D: {
const char*_tmp5A8;void*_tmp5A7[1];struct Cyc_Int_pa_PrintArg_struct _tmp5A6;return(struct _dyneither_ptr)((_tmp5A6.tag=1,((_tmp5A6.f1=(unsigned long)_tmp307,((_tmp5A7[0]=& _tmp5A6,Cyc_aprintf(((_tmp5A8="%d",_tag_dyneither(_tmp5A8,sizeof(char),3))),_tag_dyneither(_tmp5A7,sizeof(void*),1))))))));}_LL17E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp308=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp308->tag != 10)goto _LL180;else{_tmp309=_tmp308->f1;}}_LL17F: {
const char*_tmp5AC;void*_tmp5AB[1];struct Cyc_Int_pa_PrintArg_struct _tmp5AA;return(struct _dyneither_ptr)((_tmp5AA.tag=1,((_tmp5AA.f1=(unsigned long)((int)_tmp309),((_tmp5AB[0]=& _tmp5AA,Cyc_aprintf(((_tmp5AC="%d",_tag_dyneither(_tmp5AC,sizeof(char),3))),_tag_dyneither(_tmp5AB,sizeof(void*),1))))))));}_LL180: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp30A=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp30A->tag != 11)goto _LL182;else{_tmp30B=_tmp30A->f1;_tmp30C=_tmp30A->f2;}}_LL181:
 return _tmp30B;_LL182: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp30D=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp30D->tag != 12)goto _LL184;else{_tmp30E=_tmp30D->f2;}}_LL183:
 _tmp310=_tmp30E;goto _LL185;_LL184: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp30F=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp30F->tag != 13)goto _LL186;else{_tmp310=_tmp30F->f2;}}_LL185:
 return Cyc_Absynpp_qvar2string(_tmp310->name);_LL186: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp311=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2F5;if(_tmp311->tag != 16)goto _LL188;else{_tmp312=_tmp311->f1;}}_LL187:
 return Cyc_Absynpp_exp2string(_tmp312);_LL188:;_LL189:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL169:;};}_LL167: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F0=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2EC;if(_tmp2F0->tag != 1)goto _LL164;else{_tmp2F1=_tmp2F0->f1;}}_LL168:
# 1402
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2F1)){const char*_tmp5AD;return(_tmp5AD="_",_tag_dyneither(_tmp5AD,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp33C=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2F1);
struct Cyc_Absyn_Pat*_tmp33D=_tmp33C->orig_pat;
# 1407
if(_tmp33D == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp33E=Cyc_Tcutil_compress((void*)_check_null(_tmp33D->topt));void*_tmp33F=_tmp33E;struct Cyc_Absyn_PtrAtts _tmp343;struct Cyc_Absyn_Datatypedecl*_tmp345;struct Cyc_Absyn_Aggrdecl*_tmp347;_LL190: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp33F;if(_tmp340->tag != 6)goto _LL192;else{if(_tmp340->f2 != Cyc_Absyn_Char_sz)goto _LL192;}}_LL191:
# 1411
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp348=Cyc_Tcpat_char_con(r,(char)i,_tmp33D);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F1,_tmp348)){
const char*_tmp5B1;void*_tmp5B0[1];struct Cyc_Int_pa_PrintArg_struct _tmp5AF;return(struct _dyneither_ptr)((_tmp5AF.tag=1,((_tmp5AF.f1=(unsigned long)i,((_tmp5B0[0]=& _tmp5AF,Cyc_aprintf(((_tmp5B1="%d",_tag_dyneither(_tmp5B1,sizeof(char),3))),_tag_dyneither(_tmp5B0,sizeof(void*),1))))))));}}}
# 1416
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL192: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp33F;if(_tmp341->tag != 6)goto _LL194;}_LL193:
# 1419
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp34C=Cyc_Tcpat_int_con(r,(int)i,_tmp33D);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F1,_tmp34C)){
const char*_tmp5B5;void*_tmp5B4[1];struct Cyc_Int_pa_PrintArg_struct _tmp5B3;return(struct _dyneither_ptr)((_tmp5B3.tag=1,((_tmp5B3.f1=(unsigned long)((int)i),((_tmp5B4[0]=& _tmp5B3,Cyc_aprintf(((_tmp5B5="%d",_tag_dyneither(_tmp5B5,sizeof(char),3))),_tag_dyneither(_tmp5B4,sizeof(void*),1))))))));}}}
# 1424
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL194: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp33F;if(_tmp342->tag != 5)goto _LL196;else{_tmp343=(_tmp342->f1).ptr_atts;}}_LL195: {
# 1426
union Cyc_Absyn_Constraint*_tmp350=_tmp343.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp350);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F1,Cyc_Tcpat_null_con(r,_tmp33D))){
const char*_tmp5B6;return(_tmp5B6="NULL",_tag_dyneither(_tmp5B6,sizeof(char),5));}}{
# 1432
const char*_tmp5B7;return(_tmp5B7="&_",_tag_dyneither(_tmp5B7,sizeof(char),3));};}_LL196: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp33F;if(_tmp344->tag != 3)goto _LL198;else{if((((_tmp344->f1).datatype_info).KnownDatatype).tag != 2)goto _LL198;_tmp345=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp344->f1).datatype_info).KnownDatatype).val);}}_LL197:
# 1434
 if(_tmp345->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp353=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp345->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp353);
for(0;(unsigned int)_tmp353;_tmp353=_tmp353->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp353->hd)->name).f2;
struct Cyc_List_List*_tmp354=((struct Cyc_Absyn_Datatypefield*)_tmp353->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp5B8;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F1,((_tmp5B8=_cycalloc(sizeof(*_tmp5B8)),((_tmp5B8->name=Cyc_Tcpat_Name_v(n),((_tmp5B8->arity=0,((_tmp5B8->span=0,((_tmp5B8->orig_pat=_tmp33D,_tmp5B8)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp354)== 0)
return n;else{
# 1444
const char*_tmp5BC;void*_tmp5BB[1];struct Cyc_String_pa_PrintArg_struct _tmp5BA;return(struct _dyneither_ptr)((_tmp5BA.tag=0,((_tmp5BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5BB[0]=& _tmp5BA,Cyc_aprintf(((_tmp5BC="%s(...)",_tag_dyneither(_tmp5BC,sizeof(char),8))),_tag_dyneither(_tmp5BB,sizeof(void*),1))))))));}}}
# 1447
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL198:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp33F;if(_tmp346->tag != 11)goto _LL19A;else{if((((_tmp346->f1).aggr_info).KnownAggr).tag != 2)goto _LL19A;_tmp347=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp346->f1).aggr_info).KnownAggr).val);}}if(!(_tmp347->kind == Cyc_Absyn_UnionA))goto _LL19A;_LL199: {
# 1449
struct Cyc_List_List*_tmp359=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp347->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp359);
struct _tuple2*_tmp35A=_tmp347->name;struct _dyneither_ptr _tmp35C;struct _tuple2*_tmp35B=_tmp35A;_tmp35C=*_tmp35B->f2;
for(0;(unsigned int)_tmp359;_tmp359=_tmp359->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp359->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp5BD;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2F1,((_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((_tmp5BD->name=Cyc_Tcpat_Name_v(n),((_tmp5BD->arity=0,((_tmp5BD->span=0,((_tmp5BD->orig_pat=_tmp33D,_tmp5BD)))))))))))){
const char*_tmp5C2;void*_tmp5C1[2];struct Cyc_String_pa_PrintArg_struct _tmp5C0;struct Cyc_String_pa_PrintArg_struct _tmp5BF;return(struct _dyneither_ptr)((_tmp5BF.tag=0,((_tmp5BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5C0.tag=0,((_tmp5C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35C),((_tmp5C1[0]=& _tmp5C0,((_tmp5C1[1]=& _tmp5BF,Cyc_aprintf(((_tmp5C2="%s{.%s = _}",_tag_dyneither(_tmp5C2,sizeof(char),12))),_tag_dyneither(_tmp5C1,sizeof(void*),2))))))))))))));}}
# 1457
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18F:;};};_LL164:;}
# 1462
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp5C3;return(_tmp5C3=_cycalloc(sizeof(*_tmp5C3)),((_tmp5C3[0]=Cyc_Tcpat_desc2string(r,d),_tmp5C3)));}
# 1465
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp364=_tmp363;for(0;_tmp364 != 0;_tmp364=((struct Cyc_List_List*)_check_null(_tmp364))->tl){
if(_tmp364->tl != 0){
{struct Cyc_List_List*_tmp5C4;_tmp364->tl=((_tmp5C4=_cycalloc(sizeof(*_tmp5C4)),((_tmp5C4->hd=comma,((_tmp5C4->tl=_tmp364->tl,_tmp5C4))))));}
_tmp364=_tmp364->tl;}}}
# 1474
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp363);}
# 1477
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp367;_push_handler(& _tmp367);{int _tmp369=0;if(setjmp(_tmp367.handler))_tmp369=1;if(!_tmp369){
{struct _dyneither_ptr _tmp36A=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5C8;void*_tmp5C7[1];struct Cyc_String_pa_PrintArg_struct _tmp5C6;(_tmp5C6.tag=0,((_tmp5C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36A),((_tmp5C7[0]=& _tmp5C6,Cyc_Tcutil_terr(loc,((_tmp5C8="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5C8,sizeof(char),53))),_tag_dyneither(_tmp5C7,sizeof(void*),1)))))));}
# 1479
;_pop_handler();}else{void*_tmp368=(void*)_exn_thrown;void*_tmp36F=_tmp368;void*_tmp371;_LL19D: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp370=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp36F;if(_tmp370->tag != Cyc_Tcpat_Desc2string)goto _LL19F;}_LL19E:
# 1483
{const char*_tmp5CB;void*_tmp5CA;(_tmp5CA=0,Cyc_Tcutil_terr(loc,((_tmp5CB="patterns may not be exhaustive.",_tag_dyneither(_tmp5CB,sizeof(char),32))),_tag_dyneither(_tmp5CA,sizeof(void*),0)));}
goto _LL19C;_LL19F: _tmp371=_tmp36F;_LL1A0:(void)_throw(_tmp371);_LL19C:;}};}
# 1487
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple20*rhs){
(*rhs).f1=1;}
# 1491
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1496
struct _RegionHandle _tmp374=_new_region("r");struct _RegionHandle*r=& _tmp374;_push_region(r);
{int _tmp375=0;
struct _tuple24 _tmp5CC;struct _tuple24 _tmp376=(_tmp5CC.f1=r,((_tmp5CC.f2=& _tmp375,_tmp5CC)));
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _tuple24*,struct Cyc_Absyn_Switch_clause*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp376,swcs);
void*_tmp378=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp377);
int _tmp379=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp378,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp379);
# 1504
for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
struct _tuple23*_tmp37A=(struct _tuple23*)_tmp377->hd;int _tmp37C;unsigned int _tmp37D;struct _tuple23*_tmp37B=_tmp37A;_tmp37C=(_tmp37B->f2)->f1;_tmp37D=(_tmp37B->f2)->f2;
if(!_tmp37C){
{const char*_tmp5CF;void*_tmp5CE;(_tmp5CE=0,Cyc_Tcutil_terr(_tmp37D,((_tmp5CF="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5CF,sizeof(char),74))),_tag_dyneither(_tmp5CE,sizeof(void*),0)));}
# 1509
break;}}}
# 1497
;_pop_region(r);}
# 1519
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp381;_push_handler(& _tmp381);{int _tmp383=0;if(setjmp(_tmp381.handler))_tmp383=1;if(!_tmp383){
{struct _dyneither_ptr _tmp384=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5D3;void*_tmp5D2[1];struct Cyc_String_pa_PrintArg_struct _tmp5D1;(_tmp5D1.tag=0,((_tmp5D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp384),((_tmp5D2[0]=& _tmp5D1,Cyc_Tcutil_warn((*pr).f1,((_tmp5D3="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5D3,sizeof(char),45))),_tag_dyneither(_tmp5D2,sizeof(void*),1)))))));}
# 1522
;_pop_handler();}else{void*_tmp382=(void*)_exn_thrown;void*_tmp389=_tmp382;void*_tmp38B;_LL1A2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp38A=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp389;if(_tmp38A->tag != Cyc_Tcpat_Desc2string)goto _LL1A4;}_LL1A3:
# 1526
{const char*_tmp5D6;void*_tmp5D5;(_tmp5D5=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5D6="pattern not exhaustive.",_tag_dyneither(_tmp5D6,sizeof(char),24))),_tag_dyneither(_tmp5D5,sizeof(void*),0)));}
goto _LL1A1;_LL1A4: _tmp38B=_tmp389;_LL1A5:(void)_throw(_tmp38B);_LL1A1:;}};};}
# 1530
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple25{void*f1;int f2;};
# 1533
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp38E=_new_region("r");struct _RegionHandle*r=& _tmp38E;_push_region(r);
{struct _tuple25*_tmp5D9;struct Cyc_List_List*_tmp5D8;struct Cyc_List_List*_tmp38F=(_tmp5D8=_region_malloc(r,sizeof(*_tmp5D8)),((_tmp5D8->hd=((_tmp5D9=_region_malloc(r,sizeof(*_tmp5D9)),((_tmp5D9->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp5D9->f2=0,_tmp5D9)))))),((_tmp5D8->tl=0,_tmp5D8)))));
void*_tmp390=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp38F);
struct _tuple14 _tmp5DA;struct _tuple14 _tmp391=(_tmp5DA.f1=loc,((_tmp5DA.f2=1,_tmp5DA)));
int _tmp392=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp390,Cyc_Tcpat_not_exhaust_warn,& _tmp391,Cyc_Tcpat_dummy_fn,0,& _tmp392);{
# 1541
int _tmp393=_tmp391.f2;_npop_handler(0);return _tmp393;};}
# 1535
;_pop_region(r);}
# 1549
static struct _tuple23*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1551
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple20*_tmp5DB;struct _tuple20*rhs=(_tmp5DB=_region_malloc(r,sizeof(*_tmp5DB)),((_tmp5DB->f1=0,((_tmp5DB->f2=(swc->pattern)->loc,_tmp5DB)))));
struct _tuple23*_tmp5DC;return(_tmp5DC=_region_malloc(r,sizeof(*_tmp5DC)),((_tmp5DC->f1=sp0,((_tmp5DC->f2=rhs,_tmp5DC)))));}
# 1555
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1558
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1560
struct _RegionHandle _tmp399=_new_region("r");struct _RegionHandle*r=& _tmp399;_push_region(r);
{struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp39B=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp39A);
int _tmp39C=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp39B,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp39C);
# 1566
for(0;_tmp39A != 0;_tmp39A=_tmp39A->tl){
struct _tuple23*_tmp39D=(struct _tuple23*)_tmp39A->hd;int _tmp39F;unsigned int _tmp3A0;struct _tuple23*_tmp39E=_tmp39D;_tmp39F=(_tmp39E->f2)->f1;_tmp3A0=(_tmp39E->f2)->f2;
if(!_tmp39F){
{const char*_tmp5DF;void*_tmp5DE;(_tmp5DE=0,Cyc_Tcutil_terr(_tmp3A0,((_tmp5DF="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5DF,sizeof(char),74))),_tag_dyneither(_tmp5DE,sizeof(void*),0)));}
break;}}}
# 1561
;_pop_region(r);}
