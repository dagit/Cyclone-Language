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
# 976
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
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
# 121 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 130
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 147
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 206
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 56
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 59
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 64
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 66
void*Cyc_Tcutil_compress(void*t);
# 75
int Cyc_Tcutil_coerceable(void*);
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 128
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 155
int Cyc_Tcutil_unify(void*,void*);
# 160
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 187
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 235 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 250
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 278
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
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
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
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
{const char*_tmp375;void*_tmp374;(_tmp374=0,Cyc_Tcutil_terr(p->loc,((_tmp375="struct tag used without arguments in pattern",_tag_dyneither(_tmp375,sizeof(char),45))),_tag_dyneither(_tmp374,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp378;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp377;p->r=(void*)((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377[0]=((_tmp378.tag=7,((_tmp378.f1=_tmp1B,((_tmp378.f2=_tmp1C,((_tmp378.f3=0,((_tmp378.f4=0,_tmp378)))))))))),_tmp377))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp37B;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp37A;p->r=(void*)((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((_tmp37A[0]=((_tmp37B.tag=12,((_tmp37B.f1=_tmp1E,((_tmp37B.f2=_tmp1F,_tmp37B)))))),_tmp37A))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp37E;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp37D;p->r=(void*)((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37E.tag=13,((_tmp37E.f1=_tmp21,((_tmp37E.f2=_tmp22,_tmp37E)))))),_tmp37D))));}
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
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp381;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp380;p->r=(void*)((_tmp380=_cycalloc(sizeof(*_tmp380)),((_tmp380[0]=((_tmp381.tag=1,((_tmp381.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp381.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp381)))))),_tmp380))));}
return;_LL22:;_LL23:
{const char*_tmp384;void*_tmp383;(_tmp383=0,Cyc_Tcutil_terr(p->loc,((_tmp384="qualified variable in pattern",_tag_dyneither(_tmp384,sizeof(char),30))),_tag_dyneither(_tmp383,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 85
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_stmttmp2=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);void*_tmp39=_stmttmp2;struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp387;struct Cyc_List_List*_tmp386;_tmp42=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386->hd=((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387->f1=0,((_tmp387->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp387)))))),((_tmp386->tl=_tmp42,_tmp386))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp391;struct Cyc_Absyn_Aggrdecl**_tmp390;struct Cyc_Absyn_AggrInfo*_tmp38F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp38E;p->r=(void*)((_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E[0]=((_tmp391.tag=6,((_tmp391.f1=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F->aggr_info=Cyc_Absyn_KnownAggr(((_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390[0]=_tmp3B,_tmp390))))),((_tmp38F->targs=0,_tmp38F)))))),((_tmp391.f2=0,((_tmp391.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp391.f4=_tmp6,_tmp391)))))))))),_tmp38E))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp394;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp393;p->r=(void*)((_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393[0]=((_tmp394.tag=7,((_tmp394.f1=_tmp3D,((_tmp394.f2=_tmp3E,((_tmp394.f3=_tmp5,((_tmp394.f4=_tmp6,_tmp394)))))))))),_tmp393))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp397;void*_tmp396;(_tmp396=0,Cyc_Tcutil_terr(p->loc,((_tmp397="enum tag used with arguments in pattern",_tag_dyneither(_tmp397,sizeof(char),40))),_tag_dyneither(_tmp396,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp39B;void*_tmp39A[1];struct Cyc_String_pa_PrintArg_struct _tmp399;(_tmp399.tag=0,((_tmp399.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp39A[0]=& _tmp399,Cyc_Tcutil_terr(p->loc,((_tmp39B="%s is not a constructor in pattern",_tag_dyneither(_tmp39B,sizeof(char),35))),_tag_dyneither(_tmp39A,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp39E;void*_tmp39D;(_tmp39D=0,Cyc_Tcutil_terr(p->loc,((_tmp39E="cannot determine pattern type",_tag_dyneither(_tmp39E,sizeof(char),30))),_tag_dyneither(_tmp39D,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3A4;struct Cyc_Absyn_AggrInfo*_tmp3A3;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A2;p->r=(void*)((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2[0]=((_tmp3A4.tag=6,((_tmp3A4.f1=((_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3[0]=_tmp58,_tmp3A3)))),((_tmp3A4.f2=_tmp8,((_tmp3A4.f3=_tmp9,((_tmp3A4.f4=_tmpA,_tmp3A4)))))))))),_tmp3A2))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp3A8;void*_tmp3A7[1];struct Cyc_String_pa_PrintArg_struct _tmp3A6;(_tmp3A6.tag=0,((_tmp3A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3A7[0]=& _tmp3A6,Cyc_Tcutil_terr(p->loc,((_tmp3A8="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3A8,sizeof(char),45))),_tag_dyneither(_tmp3A7,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 126
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp3B0;void*_tmp3AF[1];const char*_tmp3AE;const char*_tmp3AD;struct Cyc_String_pa_PrintArg_struct _tmp3AC;(_tmp3AC.tag=0,((_tmp3AC.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp3AD="struct",_tag_dyneither(_tmp3AD,sizeof(char),7)):((_tmp3AE="union",_tag_dyneither(_tmp3AE,sizeof(char),6)))),((_tmp3AF[0]=& _tmp3AC,Cyc_Tcutil_terr(p->loc,((_tmp3B0="can't destructure an abstract %s",_tag_dyneither(_tmp3B0,sizeof(char),33))),_tag_dyneither(_tmp3AF,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp3B3;void*_tmp3B2;(_tmp3B2=0,Cyc_Tcutil_terr(p->loc,((_tmp3B3="too many existentially bound type variables in pattern",_tag_dyneither(_tmp3B3,sizeof(char),55))),_tag_dyneither(_tmp3B2,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3B9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3B8;struct Cyc_List_List*_tmp3B7;_tmp6C=((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3B8.tag=1,((_tmp3B8.f1=0,_tmp3B8)))),_tmp3B9))))),((_tmp3B7->tl=_tmp6C,_tmp3B7))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3BF;struct Cyc_Absyn_AggrInfo*_tmp3BE;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3BD;p->r=(void*)((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BF.tag=6,((_tmp3BF.f1=((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3BE->targs=_tmpE,_tmp3BE)))))),((_tmp3BF.f2=*_tmpF,((_tmp3BF.f3=_tmp10,((_tmp3BF.f4=_tmp11,_tmp3BF)))))))))),_tmp3BD))));};};}
# 127
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3C2;void*_tmp3C1;(_tmp3C1=0,Cyc_Tcutil_terr(p->loc,((_tmp3C2="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3C2,sizeof(char),43))),_tag_dyneither(_tmp3C1,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3C5;void*_tmp3C4;(_tmp3C4=0,Cyc_Tcutil_terr(p->loc,((_tmp3C5="non-constant expression in case pattern",_tag_dyneither(_tmp3C5,sizeof(char),40))),_tag_dyneither(_tmp3C4,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _stmttmp1=Cyc_Evexp_eval_const_uint_exp(_tmp13);unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=_stmttmp1;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3C8;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3C7;p->r=(void*)((_tmp3C7=_cycalloc_atomic(sizeof(*_tmp3C7)),((_tmp3C7[0]=((_tmp3C8.tag=9,((_tmp3C8.f1=Cyc_Absyn_None,((_tmp3C8.f2=(int)_tmp7C,_tmp3C8)))))),_tmp3C7))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3C9;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9->v=s,_tmp3C9)))));};}
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
struct _tuple16*_tmp3CC;struct Cyc_List_List*_tmp3CB;*v_result_ptr=((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB->hd=((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC->f1=vd,((_tmp3CC->f2=e,_tmp3CC)))))),((_tmp3CB->tl=*v_result_ptr,_tmp3CB))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Tcpat_TcPatResult _tmp86=res1;_tmp87=_tmp86.tvars_and_bounds_opt;_tmp88=_tmp86.patvars;{
struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res2;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;
if(_tmp87 != 0  || _tmp8A != 0){
if(_tmp87 == 0){
struct _tuple1*_tmp3CD;_tmp87=((_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->f1=0,((_tmp3CD->f2=0,_tmp3CD))))));}
if(_tmp8A == 0){
struct _tuple1*_tmp3CE;_tmp8A=((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->f1=0,((_tmp3CE->f2=0,_tmp3CE))))));}{
struct _tuple1*_tmp3D1;struct Cyc_Tcpat_TcPatResult _tmp3D0;return(_tmp3D0.tvars_and_bounds_opt=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f1,(*_tmp8A).f1),((_tmp3D1->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f2,(*_tmp8A).f2),_tmp3D1)))))),((_tmp3D0.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3D0)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp3D2;return(_tmp3D2.tvars_and_bounds_opt=0,((_tmp3D2.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3D2)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3D3;return(_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->loc=loc,((_tmp3D3->topt=0,((_tmp3D3->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3D3)))))));}
# 221
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_stmttmp6=Cyc_Tcutil_compress(t);void*_tmp92=_stmttmp6;void*_tmp94;struct Cyc_Absyn_Tqual _tmp95;struct Cyc_Absyn_Exp*_tmp96;union Cyc_Absyn_Constraint*_tmp97;unsigned int _tmp98;_LL46: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92;if(_tmp93->tag != 8)goto _LL48;else{_tmp94=(_tmp93->f1).elt_type;_tmp95=(_tmp93->f1).tq;_tmp96=(_tmp93->f1).num_elts;_tmp97=(_tmp93->f1).zero_term;_tmp98=(_tmp93->f1).zt_loc;}}_LL47: {
# 224
struct Cyc_Core_Opt*_tmp3D4;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3D4))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3D7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3D6;return Cyc_Absyn_atb_typ(_tmp94,rgn,_tmp95,(void*)((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3D7.tag=1,((_tmp3D7.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp96),_tmp3D7)))),_tmp3D6)))),_tmp97);}_LL48:;_LL49:
 return t;_LL45:;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 232
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 235
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3D8;struct Cyc_Tcpat_TcPatResult res=(_tmp3D8.tvars_and_bounds_opt=0,((_tmp3D8.patvars=0,_tmp3D8)));
# 240
{void*_stmttmp7=p->r;void*_tmp9C=_stmttmp7;struct Cyc_Absyn_Vardecl*_tmp9F;struct Cyc_Absyn_Pat*_tmpA0;struct Cyc_Absyn_Vardecl*_tmpA2;struct Cyc_Absyn_Pat*_tmpA3;struct Cyc_Absyn_Tvar*_tmpA5;struct Cyc_Absyn_Vardecl*_tmpA6;int _tmpAC;struct Cyc_Absyn_Enumdecl*_tmpAE;void*_tmpB0;struct Cyc_Absyn_Pat*_tmpB3;struct Cyc_List_List**_tmpB5;int _tmpB6;struct Cyc_Absyn_Aggrdecl*_tmpB8;struct Cyc_List_List**_tmpB9;struct Cyc_List_List*_tmpBA;struct Cyc_List_List**_tmpBB;int _tmpBC;struct Cyc_Absyn_Datatypedecl*_tmpBE;struct Cyc_Absyn_Datatypefield*_tmpBF;struct Cyc_List_List**_tmpC0;int _tmpC1;_LL4B: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmp9D->tag != 0)goto _LL4D;}_LL4C:
# 243
 if(topt != 0)
t=*topt;else{
# 246
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL4A;_LL4D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9E=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmp9E->tag != 1)goto _LL4F;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LL4E:
# 249
 res=Cyc_Tcpat_tcPatRec(te,_tmpA0,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA0->topt);
# 253
{void*_stmttmpE=Cyc_Tcutil_compress(t);void*_tmpC7=_stmttmpE;_LL76: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7;if(_tmpC8->tag != 8)goto _LL78;}_LL77:
# 255
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp3DB;void*_tmp3DA;(_tmp3DA=0,Cyc_Tcutil_terr(p->loc,((_tmp3DB="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3DB,sizeof(char),58))),_tag_dyneither(_tmp3DA,sizeof(void*),0)));}
goto _LL75;_LL78:;_LL79:
# 259
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3DE;void*_tmp3DD;(_tmp3DD=0,Cyc_Tcutil_terr(p->loc,((_tmp3DE="pattern would point to an abstract member",_tag_dyneither(_tmp3DE,sizeof(char),42))),_tag_dyneither(_tmp3DD,sizeof(void*),0)));}
goto _LL75;_LL75:;}
# 263
{struct Cyc_Absyn_Vardecl**_tmp3DF;Cyc_Tcpat_set_vd(((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF[0]=_tmp9F,_tmp3DF)))),access_exp,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));}
goto _LL4A;_LL4F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA1->tag != 2)goto _LL51;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;}}_LL50:
# 266
 res=Cyc_Tcpat_tcPatRec(te,_tmpA3,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA3->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_Tcutil_terr(p->loc,((_tmp3E2="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3E2,sizeof(char),55))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}
goto _LL4A;}else{
# 273
struct _RegionHandle _tmpD0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD0;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(p->loc,((_tmp3E5="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3E5,sizeof(char),56))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}
# 274
;_pop_region(rgn);}{
# 279
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3EB;struct Cyc_Absyn_PtrInfo _tmp3EA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E9;void*t2=(void*)((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EB.tag=5,((_tmp3EB.f1=((_tmp3EA.elt_typ=t,((_tmp3EA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3EA.ptr_atts=(
((_tmp3EA.ptr_atts).rgn=rgn_pat,(((_tmp3EA.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3EA.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EA.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EA.ptr_atts).ptrloc=0,_tmp3EA.ptr_atts)))))))))),_tmp3EA)))))),_tmp3EB)))),_tmp3E9))));
# 284
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 288
{struct Cyc_Absyn_Vardecl**_tmp3EC;Cyc_Tcpat_set_vd(((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=_tmpA2,_tmp3EC)))),new_access_exp,& res.patvars,t2);}
goto _LL4A;};_LL51: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA4->tag != 3)goto _LL53;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;}}_LL52:
# 294
{struct Cyc_Absyn_Vardecl**_tmp3ED;Cyc_Tcpat_set_vd(((_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED[0]=_tmpA6,_tmp3ED)))),access_exp,& res.patvars,_tmpA6->type);}{
# 298
struct _RegionHandle _tmpD8=_new_region("r2");struct _RegionHandle*r2=& _tmpD8;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3EE[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3EE[0]=_tmpA5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EE,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 301
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp3EF;res.tvars_and_bounds_opt=((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF->f1=0,((_tmp3EF->f2=0,_tmp3EF))))));}
{struct Cyc_List_List*_tmp3F0;(*res.tvars_and_bounds_opt).f1=(
(_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=_tmpA5,((_tmp3F0->tl=(*res.tvars_and_bounds_opt).f1,_tmp3F0))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL4A;
# 298
;_pop_region(r2);};_LL53: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA7->tag != 9)goto _LL55;else{if(_tmpA7->f1 != Cyc_Absyn_Unsigned)goto _LL55;}}_LL54:
# 308
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL4A;_LL55: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA8->tag != 9)goto _LL57;else{if(_tmpA8->f1 != Cyc_Absyn_None)goto _LL57;}}_LL56:
 goto _LL58;_LL57: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA9->tag != 9)goto _LL59;else{if(_tmpA9->f1 != Cyc_Absyn_Signed)goto _LL59;}}_LL58:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL4A;_LL59: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpAA=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAA->tag != 10)goto _LL5B;}_LL5A:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL4A;_LL5B: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAB->tag != 11)goto _LL5D;else{_tmpAC=_tmpAB->f2;}}_LL5C:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAC));goto _LL4A;_LL5D: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAD->tag != 12)goto _LL5F;else{_tmpAE=_tmpAD->f1;}}_LL5E:
# 314
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3F3;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2[0]=((_tmp3F3.tag=13,((_tmp3F3.f1=_tmpAE->name,((_tmp3F3.f2=_tmpAE,_tmp3F3)))))),_tmp3F2)))));}
goto _LL4A;_LL5F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAF->tag != 13)goto _LL61;else{_tmpB0=(void*)_tmpAF->f1;}}_LL60:
 t=Cyc_Tcpat_num_type(topt,_tmpB0);goto _LL4A;_LL61: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB1->tag != 8)goto _LL63;}_LL62:
# 318
 if(topt != 0){
void*_stmttmpD=Cyc_Tcutil_compress(*topt);void*_tmpDE=_stmttmpD;_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDE;if(_tmpDF->tag != 5)goto _LL7D;}_LL7C:
 t=*topt;goto tcpat_end;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 323
struct Cyc_Core_Opt*_tmpE0=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3F9;struct Cyc_Absyn_PtrInfo _tmp3F8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F7;t=(void*)((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7[0]=((_tmp3F9.tag=5,((_tmp3F9.f1=((_tmp3F8.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpE0),((_tmp3F8.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3F8.ptr_atts=(
((_tmp3F8.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpE0),(((_tmp3F8.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp3F8.ptr_atts).bounds=
# 328
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F8.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F8.ptr_atts).ptrloc=0,_tmp3F8.ptr_atts)))))))))),_tmp3F8)))))),_tmp3F9)))),_tmp3F7))));}
# 331
goto _LL4A;};_LL63: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB2=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB2->tag != 5)goto _LL65;else{_tmpB3=_tmpB2->f1;}}_LL64: {
# 336
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpE4=0;
int elt_const=0;
if(topt != 0){
void*_stmttmpA=Cyc_Tcutil_compress(*topt);void*_tmpE5=_stmttmpA;void*_tmpE7;struct Cyc_Absyn_Tqual _tmpE8;_LL80: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE5;if(_tmpE6->tag != 5)goto _LL82;else{_tmpE7=(_tmpE6->f1).elt_typ;_tmpE8=(_tmpE6->f1).elt_tq;}}_LL81:
# 342
 inner_typ=_tmpE7;
_tmpE4=& inner_typ;
elt_const=_tmpE8.real_const;
goto _LL7F;_LL82:;_LL83:
 goto _LL7F;_LL7F:;}{
# 351
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB3,_tmpE4,ptr_rgn,1,new_access_exp));
# 360
{void*_stmttmpB=Cyc_Tcutil_compress((void*)_check_null(_tmpB3->topt));void*_tmpE9=_stmttmpB;struct Cyc_Absyn_Datatypedecl*_tmpEB;struct Cyc_Absyn_Datatypefield*_tmpEC;struct Cyc_List_List*_tmpED;_LL85: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE9;if(_tmpEA->tag != 4)goto _LL87;else{if((((_tmpEA->f1).field_info).KnownDatatypefield).tag != 2)goto _LL87;_tmpEB=((struct _tuple3)(((_tmpEA->f1).field_info).KnownDatatypefield).val).f1;_tmpEC=((struct _tuple3)(((_tmpEA->f1).field_info).KnownDatatypefield).val).f2;_tmpED=(_tmpEA->f1).targs;}}_LL86:
# 364
{void*_stmttmpC=Cyc_Tcutil_compress(inner_typ);void*_tmpEE=_stmttmpC;_LL8A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEF=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpEE;if(_tmpEF->tag != 4)goto _LL8C;}_LL8B:
# 366
 goto DONT_PROMOTE;_LL8C:;_LL8D:
 goto _LL89;_LL89:;}{
# 370
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp403;struct Cyc_Absyn_Datatypedecl**_tmp402;struct Cyc_Absyn_DatatypeInfo _tmp401;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp400;void*new_type=(void*)((_tmp400=_cycalloc(sizeof(*_tmp400)),((_tmp400[0]=((_tmp403.tag=3,((_tmp403.f1=((_tmp401.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=_tmpEB,_tmp402))))),((_tmp401.targs=_tmpED,_tmp401)))),_tmp403)))),_tmp400))));
# 373
_tmpB3->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp409;struct Cyc_Absyn_PtrInfo _tmp408;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp407;t=(void*)((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407[0]=((_tmp409.tag=5,((_tmp409.f1=((_tmp408.elt_typ=new_type,((_tmp408.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp408.ptr_atts=(
((_tmp408.ptr_atts).rgn=ptr_rgn,(((_tmp408.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp408.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp408.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp408.ptr_atts).ptrloc=0,_tmp408.ptr_atts)))))))))),_tmp408)))))),_tmp409)))),_tmp407))));}
# 380
goto _LL84;};_LL87:;_LL88:
# 382
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp40F;struct Cyc_Absyn_PtrInfo _tmp40E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40D;t=(void*)((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40F.tag=5,((_tmp40F.f1=((_tmp40E.elt_typ=(void*)_check_null(_tmpB3->topt),((_tmp40E.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp40E.ptr_atts=(
((_tmp40E.ptr_atts).rgn=ptr_rgn,(((_tmp40E.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40E.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40E.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40E.ptr_atts).ptrloc=0,_tmp40E.ptr_atts)))))))))),_tmp40E)))))),_tmp40F)))),_tmp40D))));}_LL84:;}
# 390
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpB3->topt;
goto _LL4A;};}_LL65: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB4->tag != 4)goto _LL67;else{_tmpB5=(struct Cyc_List_List**)& _tmpB4->f1;_tmpB6=_tmpB4->f2;}}_LL66: {
# 394
struct Cyc_List_List*_tmpFA=*_tmpB5;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_stmttmp9=Cyc_Tcutil_compress(*topt);void*_tmpFB=_stmttmp9;struct Cyc_List_List*_tmpFD;_LL8F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpFC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFB;if(_tmpFC->tag != 10)goto _LL91;else{_tmpFD=_tmpFC->f1;}}_LL90:
# 400
 topt_ts=_tmpFD;
if(_tmpB6){
# 403
int _tmpFE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFA);
int _tmpFF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD);
if(_tmpFE < _tmpFF){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpFF - _tmpFE;++ i){
struct Cyc_List_List*_tmp410;wild_ps=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp410->tl=wild_ps,_tmp410))))));}}
*_tmpB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpFA,wild_ps);
_tmpFA=*_tmpB5;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFA)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD)){
const char*_tmp413;void*_tmp412;(_tmp412=0,Cyc_Tcutil_warn(p->loc,((_tmp413="unnecessary ... in tuple pattern",_tag_dyneither(_tmp413,sizeof(char),33))),_tag_dyneither(_tmp412,sizeof(void*),0)));}}}
# 415
goto _LL8E;_LL91:;_LL92:
# 417
 goto _LL8E;_LL8E:;}else{
# 419
if(_tmpB6){
const char*_tmp416;void*_tmp415;(_tmp415=0,Cyc_Tcutil_terr(p->loc,((_tmp416="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp416,sizeof(char),57))),_tag_dyneither(_tmp415,sizeof(void*),0)));}}
{int i=0;for(0;_tmpFA != 0;(_tmpFA=_tmpFA->tl,i ++)){
void**_tmp105=0;
if(topt_ts != 0){
_tmp105=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 427
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp419;union Cyc_Absyn_Cnst _tmp418;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp418.Int_c).val=((_tmp419.f1=Cyc_Absyn_Unsigned,((_tmp419.f2=i,_tmp419)))),(((_tmp418.Int_c).tag=5,_tmp418)))),0),0);}
# 432
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpFA->hd,_tmp105,rgn_pat,allow_ref_pat,new_access_exp));
# 435
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt;{
struct _tuple17*_tmp41C;struct Cyc_List_List*_tmp41B;pat_ts=((_tmp41B=_cycalloc(sizeof(*_tmp41B)),((_tmp41B->hd=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->f1=Cyc_Absyn_empty_tqual(0),((_tmp41C->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt),_tmp41C)))))),((_tmp41B->tl=pat_ts,_tmp41B))))));};};}}
# 438
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp41F;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp41E;t=(void*)((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=((_tmp41F.tag=10,((_tmp41F.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp41F)))),_tmp41E))));}
goto _LL4A;}_LL67: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB7->tag != 6)goto _LL69;else{if(_tmpB7->f1 == 0)goto _LL69;if((((_tmpB7->f1)->aggr_info).KnownAggr).tag != 2)goto _LL69;_tmpB8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB7->f1)->aggr_info).KnownAggr).val);_tmpB9=(struct Cyc_List_List**)&(_tmpB7->f1)->targs;_tmpBA=_tmpB7->f2;_tmpBB=(struct Cyc_List_List**)& _tmpB7->f3;_tmpBC=_tmpB7->f4;}}_LL68: {
# 442
struct Cyc_List_List*_tmp10C=*_tmpBB;
const char*_tmp421;const char*_tmp420;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(_tmp421="struct",_tag_dyneither(_tmp421,sizeof(char),7)):((_tmp420="union",_tag_dyneither(_tmp420,sizeof(char),6)));
if(_tmpB8->impl == 0){
{const char*_tmp425;void*_tmp424[1];struct Cyc_String_pa_PrintArg_struct _tmp423;(_tmp423.tag=0,((_tmp423.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp424[0]=& _tmp423,Cyc_Tcutil_terr(p->loc,((_tmp425="can't destructure an abstract %s",_tag_dyneither(_tmp425,sizeof(char),33))),_tag_dyneither(_tmp424,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL4A;}
# 451
if(_tmpB8->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->tagged)
allow_ref_pat=0;
if(_tmpBA != 0){
# 455
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 458
struct _RegionHandle _tmp110=_new_region("rgn");struct _RegionHandle*rgn=& _tmp110;_push_region(rgn);{
# 460
struct Cyc_List_List*_tmp111=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp112=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpBA;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp112))->hd;
_tmp112=_tmp112->tl;{
void*_tmp113=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp114=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp115=_tmp114;struct Cyc_Absyn_Kind*_tmp117;struct Cyc_Absyn_Kind*_tmp119;_LL94: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp116->tag != 2)goto _LL96;else{_tmp117=_tmp116->f2;}}_LL95:
 _tmp119=_tmp117;goto _LL97;_LL96: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp118=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp118->tag != 0)goto _LL98;else{_tmp119=_tmp118->f1;}}_LL97:
 k2=_tmp119;goto _LL93;_LL98:;_LL99: {
const char*_tmp428;void*_tmp427;(_tmp427=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp428="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp428,sizeof(char),53))),_tag_dyneither(_tmp427,sizeof(void*),0)));}_LL93:;}
# 476
{void*_tmp11C=_tmp113;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Core_Opt**_tmp120;struct Cyc_Absyn_Kind*_tmp121;struct Cyc_Core_Opt**_tmp123;_LL9B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp11D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL9D;else{_tmp11E=_tmp11D->f1;}}_LL9C:
# 479
 if(!Cyc_Tcutil_kind_leq(k2,_tmp11E))
error=1;
goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11F->tag != 2)goto _LL9F;else{_tmp120=(struct Cyc_Core_Opt**)& _tmp11F->f1;_tmp121=_tmp11F->f2;}}_LL9E:
 _tmp123=_tmp120;goto _LLA0;_LL9F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp122=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp122->tag != 1)goto _LL9A;else{_tmp123=(struct Cyc_Core_Opt**)& _tmp122->f1;}}_LLA0:
{struct Cyc_Core_Opt*_tmp429;*_tmp123=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->v=_tmp114,_tmp429))));}goto _LL9A;_LL9A:;}
# 485
if(error){
const char*_tmp42F;void*_tmp42E[3];struct Cyc_String_pa_PrintArg_struct _tmp42D;struct Cyc_String_pa_PrintArg_struct _tmp42C;struct Cyc_String_pa_PrintArg_struct _tmp42B;(_tmp42B.tag=0,((_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 489
Cyc_Absynpp_kind2string(k2)),((_tmp42C.tag=0,((_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp113)),((_tmp42D.tag=0,((_tmp42D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp42E[0]=& _tmp42D,((_tmp42E[1]=& _tmp42C,((_tmp42E[2]=& _tmp42B,Cyc_Tcutil_terr(p->loc,((_tmp42F="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp42F,sizeof(char),60))),_tag_dyneither(_tmp42E,sizeof(void*),3)))))))))))))))))));}{
# 491
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp432;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp431;void*vartype=(void*)((_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431[0]=((_tmp432.tag=2,((_tmp432.f1=tv,_tmp432)))),_tmp431))));
{struct Cyc_List_List*_tmp433;_tmp111=((_tmp433=_region_malloc(rgn,sizeof(*_tmp433)),((_tmp433->hd=vartype,((_tmp433->tl=_tmp111,_tmp433))))));}
# 495
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp436;struct Cyc_List_List*_tmp435;outlives_constraints=(
(_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435->hd=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436->f1=Cyc_Absyn_empty_effect,((_tmp436->f2=vartype,_tmp436)))))),((_tmp435->tl=outlives_constraints,_tmp435))))));}else{
# 500
const char*_tmp439;void*_tmp438;(_tmp438=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp439="opened existential had unique or top region kind",_tag_dyneither(_tmp439,sizeof(char),49))),_tag_dyneither(_tmp438,sizeof(void*),0)));}}};};}}
# 504
_tmp111=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp111);{
# 506
struct _RegionHandle _tmp131=_new_region("r2");struct _RegionHandle*r2=& _tmp131;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBA);
# 509
struct Cyc_List_List*_tmp132=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp43A;struct _tuple12 _tmp133=(_tmp43A.f1=_tmp132,((_tmp43A.f2=rgn,_tmp43A)));
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp133,_tmpB8->tvs);
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->exist_vars,_tmp111);
struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp134);
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp135);
struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp134,_tmp135);
# 517
if(_tmpBA != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp43B;res.tvars_and_bounds_opt=((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->f1=0,((_tmp43B->f2=0,_tmp43B))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpBA);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp13A=0;
{struct Cyc_List_List*_tmp13B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
struct _tuple0*_tmp43E;struct Cyc_List_List*_tmp43D;_tmp13A=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->hd=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f1),((_tmp43E->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f2),_tmp43E)))))),((_tmp43D->tl=_tmp13A,_tmp43D))))));}}
# 529
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13A);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp13A);};}
# 534
*_tmpB9=_tmp136;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp448;struct Cyc_Absyn_Aggrdecl**_tmp447;struct Cyc_Absyn_AggrInfo _tmp446;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp445;t=(void*)((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445[0]=((_tmp448.tag=11,((_tmp448.f1=((_tmp446.aggr_info=Cyc_Absyn_KnownAggr(((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447[0]=_tmpB8,_tmp447))))),((_tmp446.targs=*_tmpB9,_tmp446)))),_tmp448)))),_tmp445))));}
if(_tmpBC  && _tmpB8->kind == Cyc_Absyn_UnionA){
const char*_tmp44B;void*_tmp44A;(_tmp44A=0,Cyc_Tcutil_warn(p->loc,((_tmp44B="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp44B,sizeof(char),43))),_tag_dyneither(_tmp44A,sizeof(void*),0)));}else{
if(_tmpBC){
# 540
int _tmp144=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10C);
int _tmp145=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
if(_tmp144 < _tmp145){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp145 - _tmp144;++ i){
struct _tuple15*_tmp44E;struct Cyc_List_List*_tmp44D;wild_dps=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->hd=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E->f1=0,((_tmp44E->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp44E)))))),((_tmp44D->tl=wild_dps,_tmp44D))))));}}
*_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10C,wild_dps);
_tmp10C=*_tmpBB;}else{
if(_tmp144 == _tmp145){
const char*_tmp451;void*_tmp450;(_tmp450=0,Cyc_Tcutil_warn(p->loc,((_tmp451="unnecessary ... in struct pattern",_tag_dyneither(_tmp451,sizeof(char),34))),_tag_dyneither(_tmp450,sizeof(void*),0)));}}}}{
# 551
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp10C,_tmpB8->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple18*_stmttmp8=(struct _tuple18*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp14B;struct Cyc_Absyn_Pat*_tmp14C;struct _tuple18*_tmp14A=_stmttmp8;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;{
void*_tmp14D=Cyc_Tcutil_rsubstitute(rgn,_tmp138,_tmp14B->type);
# 557
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp14B->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp14C,& _tmp14D,rgn_pat,allow_ref_pat,new_access_exp));
# 565
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp14C->topt),_tmp14D)){
const char*_tmp458;void*_tmp457[4];struct Cyc_String_pa_PrintArg_struct _tmp456;struct Cyc_String_pa_PrintArg_struct _tmp455;struct Cyc_String_pa_PrintArg_struct _tmp454;struct Cyc_String_pa_PrintArg_struct _tmp453;(_tmp453.tag=0,((_tmp453.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 568
Cyc_Absynpp_typ2string((void*)_check_null(_tmp14C->topt))),((_tmp454.tag=0,((_tmp454.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 567
Cyc_Absynpp_typ2string(_tmp14D)),((_tmp455.tag=0,((_tmp455.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp456.tag=0,((_tmp456.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14B->name),((_tmp457[0]=& _tmp456,((_tmp457[1]=& _tmp455,((_tmp457[2]=& _tmp454,((_tmp457[3]=& _tmp453,Cyc_Tcutil_terr(p->loc,((_tmp458="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp458,sizeof(char),45))),_tag_dyneither(_tmp457,sizeof(void*),4)))))))))))))))))))))))));}
# 569
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp14C->topt;};}};}
# 507
;_pop_region(r2);};}
# 573
_npop_handler(0);goto _LL4A;
# 458
;_pop_region(rgn);};}_LL69: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpBD->tag != 7)goto _LL6B;else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBD->f2;_tmpC0=(struct Cyc_List_List**)& _tmpBD->f3;_tmpC1=_tmpBD->f4;}}_LL6A: {
# 576
struct Cyc_List_List*_tmp157=*_tmpC0;
struct _RegionHandle _tmp158=_new_region("rgn");struct _RegionHandle*rgn=& _tmp158;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBF->typs;
# 580
struct Cyc_List_List*_tmp159=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp459;struct _tuple12 _tmp15A=(_tmp459.f1=_tmp159,((_tmp459.f2=rgn,_tmp459)));
struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp15A,_tmpBE->tvs);
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp15B);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp45F;struct Cyc_Absyn_DatatypeFieldInfo _tmp45E;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp45D;t=(void*)((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D[0]=((_tmp45F.tag=4,((_tmp45F.f1=((_tmp45E.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBE,_tmpBF),((_tmp45E.targs=_tmp15C,_tmp45E)))),_tmp45F)))),_tmp45D))));}
# 586
if(_tmpC1){
# 588
int _tmp160=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp157);
int _tmp161=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp160 < _tmp161){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp161 - _tmp160;++ i){
struct Cyc_List_List*_tmp460;wild_ps=((_tmp460=_cycalloc(sizeof(*_tmp460)),((_tmp460->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp460->tl=wild_ps,_tmp460))))));}}
*_tmpC0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp157,wild_ps);
_tmp157=*_tmpC0;}else{
if(_tmp160 == _tmp161){
const char*_tmp464;void*_tmp463[1];struct Cyc_String_pa_PrintArg_struct _tmp462;(_tmp462.tag=0,((_tmp462.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp463[0]=& _tmp462,Cyc_Tcutil_warn(p->loc,((_tmp464="unnecessary ... in datatype field %s",_tag_dyneither(_tmp464,sizeof(char),37))),_tag_dyneither(_tmp463,sizeof(void*),1)))))));}}}
# 600
for(0;_tmp157 != 0  && tqts != 0;(_tmp157=_tmp157->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp166=(struct Cyc_Absyn_Pat*)_tmp157->hd;
# 604
void*_tmp167=Cyc_Tcutil_rsubstitute(rgn,_tmp15B,(*((struct _tuple17*)tqts->hd)).f2);
# 607
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp166,& _tmp167,rgn_pat,allow_ref_pat,0));
# 614
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp166->topt),_tmp167)){
const char*_tmp46A;void*_tmp469[3];struct Cyc_String_pa_PrintArg_struct _tmp468;struct Cyc_String_pa_PrintArg_struct _tmp467;struct Cyc_String_pa_PrintArg_struct _tmp466;(_tmp466.tag=0,((_tmp466.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 617
Cyc_Absynpp_typ2string((void*)_check_null(_tmp166->topt))),((_tmp467.tag=0,((_tmp467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 616
Cyc_Absynpp_typ2string(_tmp167)),((_tmp468.tag=0,((_tmp468.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp469[0]=& _tmp468,((_tmp469[1]=& _tmp467,((_tmp469[2]=& _tmp466,Cyc_Tcutil_terr(_tmp166->loc,((_tmp46A="%s expects argument type %s, not %s",_tag_dyneither(_tmp46A,sizeof(char),36))),_tag_dyneither(_tmp469,sizeof(void*),3)))))))))))))))))));}}
# 619
if(_tmp157 != 0){
const char*_tmp46E;void*_tmp46D[1];struct Cyc_String_pa_PrintArg_struct _tmp46C;(_tmp46C.tag=0,((_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp46D[0]=& _tmp46C,Cyc_Tcutil_terr(p->loc,((_tmp46E="too many arguments for datatype constructor %s",_tag_dyneither(_tmp46E,sizeof(char),47))),_tag_dyneither(_tmp46D,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp472;void*_tmp471[1];struct Cyc_String_pa_PrintArg_struct _tmp470;(_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp471[0]=& _tmp470,Cyc_Tcutil_terr(p->loc,((_tmp472="too few arguments for datatype constructor %s",_tag_dyneither(_tmp472,sizeof(char),46))),_tag_dyneither(_tmp471,sizeof(void*),1)))))));}}
# 626
_npop_handler(0);goto _LL4A;
# 577
;_pop_region(rgn);}_LL6B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC2=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpC2->tag != 6)goto _LL6D;else{if(_tmpC2->f1 != 0)goto _LL6D;}}_LL6C:
# 628
 goto _LL6E;_LL6D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC3=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpC3->tag != 6)goto _LL6F;else{if(_tmpC3->f1 == 0)goto _LL6F;if((((_tmpC3->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6F;}}_LL6E:
 goto _LL70;_LL6F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpC4->tag != 14)goto _LL71;}_LL70:
 goto _LL72;_LL71: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC5=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpC5->tag != 16)goto _LL73;}_LL72:
 goto _LL74;_LL73: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC6=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpC6->tag != 15)goto _LL4A;}_LL74:
# 633
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL4A;_LL4A:;}
# 635
tcpat_end:
 p->topt=t;
return res;};}
# 640
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 642
struct Cyc_Tcpat_TcPatResult _tmp175=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 644
struct _RegionHandle _tmp176=_new_region("r");struct _RegionHandle*r=& _tmp176;_push_region(r);{
struct _tuple1 _stmttmpF=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp175.patvars);struct Cyc_List_List*_tmp178;struct _tuple1 _tmp177=_stmttmpF;_tmp178=_tmp177.f1;{
struct Cyc_List_List*_tmp179=0;
{struct Cyc_List_List*x=_tmp178;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp473;_tmp179=((_tmp473=_region_malloc(r,sizeof(*_tmp473)),((_tmp473->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp473->tl=_tmp179,_tmp473))))));}}}
{const char*_tmp474;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp179),p->loc,(
(_tmp474="pattern contains a repeated variable",_tag_dyneither(_tmp474,sizeof(char),37))));}{
# 654
struct Cyc_List_List*x=_tmp175.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp10=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp17D;struct Cyc_Absyn_Exp**_tmp17E;struct _tuple16*_tmp17C=_stmttmp10;_tmp17D=_tmp17C->f1;_tmp17E=(struct Cyc_Absyn_Exp**)& _tmp17C->f2;
if(*_tmp17E != 0  && *_tmp17E != pat_var_exp)
*_tmp17E=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp17E));}};};}{
# 660
struct Cyc_Tcpat_TcPatResult _tmp17F=_tmp175;_npop_handler(0);return _tmp17F;};
# 644
;_pop_region(r);}
# 663
enum Cyc_Tcpat_PathState{Cyc_Tcpat_UniquePath  = 0,Cyc_Tcpat_NoAlias  = 1,Cyc_Tcpat_NoAliasOneAlias  = 2,Cyc_Tcpat_AliasPath  = 3};
# 665
struct _dyneither_ptr Cyc_Tcpat_pathState2string(enum Cyc_Tcpat_PathState ps){
switch(ps){case Cyc_Tcpat_UniquePath: _LLA1: {
const char*_tmp475;return(_tmp475="UniquePath",_tag_dyneither(_tmp475,sizeof(char),11));}case Cyc_Tcpat_NoAlias: _LLA2: {
const char*_tmp476;return(_tmp476="NoAlias",_tag_dyneither(_tmp476,sizeof(char),8));}case Cyc_Tcpat_NoAliasOneAlias: _LLA3: {
const char*_tmp477;return(_tmp477="NoAliasOneAlias",_tag_dyneither(_tmp477,sizeof(char),16));}case Cyc_Tcpat_AliasPath: _LLA4: {
const char*_tmp478;return(_tmp478="AliasPath",_tag_dyneither(_tmp478,sizeof(char),10));}}}
# 674
int Cyc_Tcpat_check_unique_ok(enum Cyc_Tcpat_PathState*ps,void*pat_typ,unsigned int loc){
# 676
struct _RegionHandle _tmp184=_new_region("r");struct _RegionHandle*r=& _tmp184;_push_region(r);
{int is_unique_ptr=Cyc_Tcutil_is_noalias_pointer(pat_typ,1);
int is_noalias_ptr_or_aggr=Cyc_Tcutil_is_noalias_pointer_or_aggr(r,pat_typ);
{enum Cyc_Tcpat_PathState _stmttmp11=*ps;switch(_stmttmp11){case Cyc_Tcpat_UniquePath: _LLA6:
# 681
 if(!is_unique_ptr){
if(is_noalias_ptr_or_aggr)*ps=Cyc_Tcpat_NoAlias;else{
*ps=Cyc_Tcpat_AliasPath;}}{
int _tmp185=1;_npop_handler(0);return _tmp185;};case Cyc_Tcpat_NoAlias: _LLA7:
# 686
 if(!is_noalias_ptr_or_aggr){
*ps=Cyc_Tcpat_NoAliasOneAlias;{
int _tmp186=1;_npop_handler(0);return _tmp186;};}
# 690
break;case Cyc_Tcpat_NoAliasOneAlias: _LLA8:
 break;case Cyc_Tcpat_AliasPath: _LLA9:
# 693
 if(is_noalias_ptr_or_aggr)break;{
int _tmp187=1;_npop_handler(0);return _tmp187;};}}
# 696
{const char*_tmp47B;void*_tmp47A;(_tmp47A=0,Cyc_Tcutil_terr(loc,((_tmp47B="Pattern dereferences a unique pointer from non-unique path; use swap",_tag_dyneither(_tmp47B,sizeof(char),69))),_tag_dyneither(_tmp47A,sizeof(void*),0)));}{
# 698
int _tmp18A=0;_npop_handler(0);return _tmp18A;};}
# 677
;_pop_region(r);}
# 703
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,enum Cyc_Tcpat_PathState pState){
# 706
void*_stmttmp12=p->r;void*_tmp18B=_stmttmp12;struct Cyc_Absyn_Pat*_tmp18D;struct Cyc_Absyn_AggrInfo*_tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp195;struct Cyc_Absyn_Pat*_tmp197;struct Cyc_Absyn_Pat*_tmp199;_LLAC: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp18C=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp18C->tag != 5)goto _LLAE;else{_tmp18D=_tmp18C->f1;}}_LLAD:
# 708
 if(!Cyc_Tcpat_check_unique_ok(& pState,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp18D,pState);{
# 711
void*_stmttmp13=(void*)_check_null(p->topt);void*_tmp19A=_stmttmp13;void*_tmp19C;_LLBB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 5)goto _LLBD;else{_tmp19C=((_tmp19B->f1).ptr_atts).rgn;}}_LLBC:
# 713
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp19C);
return;_LLBD:;_LLBE: {
const char*_tmp47E;void*_tmp47D;(_tmp47D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp47E="check_pat_regions: bad pointer type",_tag_dyneither(_tmp47E,sizeof(char),36))),_tag_dyneither(_tmp47D,sizeof(void*),0)));}_LLBA:;};_LLAE: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp18E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp18E->tag != 6)goto _LLB0;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;_tmp191=_tmp18E->f3;}}_LLAF:
# 718
 for(0;_tmp191 != 0;_tmp191=_tmp191->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp191->hd)).f2,pState);}
return;_LLB0: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp192=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp192->tag != 7)goto _LLB2;else{_tmp193=_tmp192->f3;}}_LLB1:
# 723
 for(0;_tmp193 != 0;_tmp193=_tmp193->tl){Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp193->hd,Cyc_Tcpat_AliasPath);}
return;_LLB2: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp194=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp194->tag != 4)goto _LLB4;else{_tmp195=_tmp194->f1;}}_LLB3:
# 726
 for(0;_tmp195 != 0;_tmp195=_tmp195->tl){Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp195->hd,pState);}
return;_LLB4: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp196=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp196->tag != 2)goto _LLB6;else{_tmp197=_tmp196->f2;}}_LLB5: {
# 729
enum Cyc_Tcpat_PathState _tmp19F=Cyc_Tcpat_AliasPath;
if(!Cyc_Tcpat_check_unique_ok(& _tmp19F,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp197,Cyc_Tcpat_AliasPath);
return;}_LLB6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp198=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp18B;if(_tmp198->tag != 1)goto _LLB8;else{_tmp199=_tmp198->f2;}}_LLB7: {
# 734
enum Cyc_Tcpat_PathState _tmp1A0=pState;
if(!Cyc_Tcpat_check_unique_ok(& _tmp1A0,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp199,pState);
return;}_LLB8:;_LLB9:
 return;_LLAB:;}
# 741
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
Cyc_Tcpat_check_pat_regions_rec(te,p,Cyc_Tcpat_UniquePath);}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 782 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp47F;return((_tmp47F.Name_v).val=s,(((_tmp47F.Name_v).tag=1,_tmp47F)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp480;return((_tmp480.Int_v).val=i,(((_tmp480.Int_v).tag=2,_tmp480)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 792
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 797
typedef void*Cyc_Tcpat_simple_pat_t;
# 800
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _stmttmp14=c1->name;union Cyc_Tcpat_Name_value _tmp1A3=_stmttmp14;struct _dyneither_ptr _tmp1A4;int _tmp1A5;_LLC0: if((_tmp1A3.Name_v).tag != 1)goto _LLC2;_tmp1A4=(struct _dyneither_ptr)(_tmp1A3.Name_v).val;_LLC1: {
# 803
union Cyc_Tcpat_Name_value _stmttmp16=c2->name;union Cyc_Tcpat_Name_value _tmp1A6=_stmttmp16;struct _dyneither_ptr _tmp1A7;_LLC5: if((_tmp1A6.Name_v).tag != 1)goto _LLC7;_tmp1A7=(struct _dyneither_ptr)(_tmp1A6.Name_v).val;_LLC6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1A4,(struct _dyneither_ptr)_tmp1A7);_LLC7: if((_tmp1A6.Int_v).tag != 2)goto _LLC4;_LLC8:
 return - 1;_LLC4:;}_LLC2: if((_tmp1A3.Int_v).tag != 2)goto _LLBF;_tmp1A5=(int)(_tmp1A3.Int_v).val;_LLC3: {
# 808
union Cyc_Tcpat_Name_value _stmttmp15=c2->name;union Cyc_Tcpat_Name_value _tmp1A8=_stmttmp15;int _tmp1A9;_LLCA: if((_tmp1A8.Name_v).tag != 1)goto _LLCC;_LLCB:
 return 1;_LLCC: if((_tmp1A8.Int_v).tag != 2)goto _LLC9;_tmp1A9=(int)(_tmp1A8.Int_v).val;_LLCD:
 return _tmp1A5 - _tmp1A9;_LLC9:;}_LLBF:;}
# 816
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 820
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 824
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp483;struct Cyc_Tcpat_Con_s*_tmp482;return(_tmp482=_region_malloc(r,sizeof(*_tmp482)),((_tmp482->name=Cyc_Tcpat_Name_v(((_tmp483="NULL",_tag_dyneither(_tmp483,sizeof(char),5)))),((_tmp482->arity=0,((_tmp482->span=& Cyc_Tcpat_two_opt,((_tmp482->orig_pat=p,_tmp482)))))))));}
# 827
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp486;struct Cyc_Tcpat_Con_s*_tmp485;return(_tmp485=_region_malloc(r,sizeof(*_tmp485)),((_tmp485->name=Cyc_Tcpat_Name_v(((_tmp486="&",_tag_dyneither(_tmp486,sizeof(char),2)))),((_tmp485->arity=1,((_tmp485->span=& Cyc_Tcpat_two_opt,((_tmp485->orig_pat=p,_tmp485)))))))));}
# 830
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp489;struct Cyc_Tcpat_Con_s*_tmp488;return(_tmp488=_region_malloc(r,sizeof(*_tmp488)),((_tmp488->name=Cyc_Tcpat_Name_v(((_tmp489="&",_tag_dyneither(_tmp489,sizeof(char),2)))),((_tmp488->arity=1,((_tmp488->span=& Cyc_Tcpat_one_opt,((_tmp488->orig_pat=p,_tmp488)))))))));}
# 833
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp48A;return(_tmp48A=_region_malloc(r,sizeof(*_tmp48A)),((_tmp48A->name=Cyc_Tcpat_Int_v(i),((_tmp48A->arity=0,((_tmp48A->span=0,((_tmp48A->orig_pat=p,_tmp48A)))))))));}
# 836
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp48B;return(_tmp48B=_region_malloc(r,sizeof(*_tmp48B)),((_tmp48B->name=Cyc_Tcpat_Name_v(f),((_tmp48B->arity=0,((_tmp48B->span=0,((_tmp48B->orig_pat=p,_tmp48B)))))))));}
# 839
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp48C;return(_tmp48C=_region_malloc(r,sizeof(*_tmp48C)),((_tmp48C->name=Cyc_Tcpat_Int_v((int)c),((_tmp48C->arity=0,((_tmp48C->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp48C->orig_pat=p,_tmp48C)))))))));}
# 842
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp48F;struct Cyc_Tcpat_Con_s*_tmp48E;return(_tmp48E=_region_malloc(r,sizeof(*_tmp48E)),((_tmp48E->name=Cyc_Tcpat_Name_v(((_tmp48F="$",_tag_dyneither(_tmp48F,sizeof(char),2)))),((_tmp48E->arity=i,((_tmp48E->span=& Cyc_Tcpat_one_opt,((_tmp48E->orig_pat=p,_tmp48E)))))))));}
# 847
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp492;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp491;return(void*)((_tmp491=_region_malloc(r,sizeof(*_tmp491)),((_tmp491[0]=((_tmp492.tag=1,((_tmp492.f1=Cyc_Tcpat_null_con(r,p),((_tmp492.f2=0,_tmp492)))))),_tmp491))));}
# 850
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp495;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp494;return(void*)((_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494[0]=((_tmp495.tag=1,((_tmp495.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp495.f2=0,_tmp495)))))),_tmp494))));}
# 853
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp498;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp497;return(void*)((_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497[0]=((_tmp498.tag=1,((_tmp498.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp498.f2=0,_tmp498)))))),_tmp497))));}
# 856
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp49B;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp49A;return(void*)((_tmp49A=_region_malloc(r,sizeof(*_tmp49A)),((_tmp49A[0]=((_tmp49B.tag=1,((_tmp49B.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp49B.f2=0,_tmp49B)))))),_tmp49A))));}
# 859
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A1;struct Cyc_List_List*_tmp4A0;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp49F;return(void*)((_tmp49F=_region_malloc(r,sizeof(*_tmp49F)),((_tmp49F[0]=((_tmp4A1.tag=1,((_tmp4A1.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4A1.f2=((_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0)),((_tmp4A0->hd=p,((_tmp4A0->tl=0,_tmp4A0)))))),_tmp4A1)))))),_tmp49F))));}
# 862
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A7;struct Cyc_List_List*_tmp4A6;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A5;return(void*)((_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5)),((_tmp4A5[0]=((_tmp4A7.tag=1,((_tmp4A7.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4A7.f2=((_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6->hd=p,((_tmp4A6->tl=0,_tmp4A6)))))),_tmp4A7)))))),_tmp4A5))));}
# 865
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AA;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A9;return(void*)((_tmp4A9=_region_malloc(r,sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AA.tag=1,((_tmp4AA.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4AA.f2=ss,_tmp4AA)))))),_tmp4A9))));}
# 868
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 870
struct Cyc_Tcpat_Con_s*_tmp4AB;struct Cyc_Tcpat_Con_s*c=(_tmp4AB=_region_malloc(r,sizeof(*_tmp4AB)),((_tmp4AB->name=Cyc_Tcpat_Name_v(con_name),((_tmp4AB->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4AB->span=span,((_tmp4AB->orig_pat=p,_tmp4AB)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AE;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4AD;return(void*)((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=1,((_tmp4AE.f1=c,((_tmp4AE.f2=ps,_tmp4AE)))))),_tmp4AD))));}
# 875
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_stmttmp17=p->r;void*_tmp1C8=_stmttmp17;enum Cyc_Absyn_Sign _tmp1CD;int _tmp1CE;char _tmp1D0;struct _dyneither_ptr _tmp1D2;struct Cyc_Absyn_Pat*_tmp1D4;struct Cyc_Absyn_Pat*_tmp1D6;struct Cyc_Absyn_Pat*_tmp1D8;struct Cyc_Absyn_Datatypedecl*_tmp1DA;struct Cyc_Absyn_Datatypefield*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Aggrdecl*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Enumdecl*_tmp1E3;struct Cyc_Absyn_Enumfield*_tmp1E4;void*_tmp1E6;struct Cyc_Absyn_Enumfield*_tmp1E7;_LLCF: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C9=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1C9->tag != 0)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1CA=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1CA->tag != 3)goto _LLD3;}_LLD2:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4B1;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4B0;s=(void*)((_tmp4B0=_region_malloc(r,sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=0,_tmp4B1)),_tmp4B0))));}goto _LLCE;_LLD3: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1CB=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1CB->tag != 8)goto _LLD5;}_LLD4:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLCE;_LLD5: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1CC=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1CC->tag != 9)goto _LLD7;else{_tmp1CD=_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;}}_LLD6:
 s=Cyc_Tcpat_int_pat(r,_tmp1CE,p);goto _LLCE;_LLD7: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1CF=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1CF->tag != 10)goto _LLD9;else{_tmp1D0=_tmp1CF->f1;}}_LLD8:
 s=Cyc_Tcpat_char_pat(r,_tmp1D0,p);goto _LLCE;_LLD9: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1D1->tag != 11)goto _LLDB;else{_tmp1D2=_tmp1D1->f1;}}_LLDA:
 s=Cyc_Tcpat_float_pat(r,_tmp1D2,p);goto _LLCE;_LLDB: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1D3->tag != 1)goto _LLDD;else{_tmp1D4=_tmp1D3->f2;}}_LLDC:
 s=Cyc_Tcpat_compile_pat(r,_tmp1D4);goto _LLCE;_LLDD: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1D5->tag != 2)goto _LLDF;else{_tmp1D6=_tmp1D5->f2;}}_LLDE:
 s=Cyc_Tcpat_compile_pat(r,_tmp1D6);goto _LLCE;_LLDF: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1D7=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1D7->tag != 5)goto _LLE1;else{_tmp1D8=_tmp1D7->f1;}}_LLE0:
# 887
{void*_stmttmp1D=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1EE=_stmttmp1D;union Cyc_Absyn_Constraint*_tmp1F0;_LLF4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EE;if(_tmp1EF->tag != 5)goto _LLF6;else{_tmp1F0=((_tmp1EF->f1).ptr_atts).nullable;}}_LLF5: {
# 889
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1F1=_tmp1F0;union Cyc_Absyn_Constraint*_tmp1F2;int _tmp1F3;_LLF9: if((_tmp1F1->Forward_constr).tag != 2)goto _LLFB;_tmp1F2=(union Cyc_Absyn_Constraint*)(_tmp1F1->Forward_constr).val;_LLFA:
# 894
*_tmp1F0=*_tmp1F2;
continue;_LLFB: if((_tmp1F1->No_constr).tag != 3)goto _LLFD;_LLFC:
# 897
{struct _union_Constraint_Eq_constr*_tmp4B2;(_tmp4B2=& _tmp1F0->Eq_constr,((_tmp4B2->tag=1,_tmp4B2->val=0)));}
is_nullable=0;
still_working=0;
goto _LLF8;_LLFD: if((_tmp1F1->Eq_constr).tag != 1)goto _LLF8;_tmp1F3=(int)(_tmp1F1->Eq_constr).val;_LLFE:
# 902
 is_nullable=_tmp1F3;
still_working=0;
goto _LLF8;_LLF8:;}{
# 907
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1D8);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 911
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLF3;};}_LLF6:;_LLF7: {
const char*_tmp4B5;void*_tmp4B4;(_tmp4B4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B5="expecting pointertype for pattern!",_tag_dyneither(_tmp4B5,sizeof(char),35))),_tag_dyneither(_tmp4B4,sizeof(void*),0)));}_LLF3:;}
# 915
goto _LLCE;_LLE1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1D9->tag != 7)goto _LLE3;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;_tmp1DC=_tmp1D9->f3;}}_LLE2: {
# 917
int*span;
{void*_stmttmp1C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F7=_stmttmp1C;_LL100: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F7;if(_tmp1F8->tag != 3)goto _LL102;}_LL101:
# 920
 if(_tmp1DA->is_extensible)
span=0;else{
# 923
int*_tmp4B6;span=((_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6)),((_tmp4B6[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1DA->fields))->v),_tmp4B6))));}
goto _LLFF;_LL102: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F7;if(_tmp1F9->tag != 4)goto _LL104;}_LL103:
 span=& Cyc_Tcpat_one_opt;goto _LLFF;_LL104:;_LL105:
{const char*_tmp4B9;void*_tmp4B8;span=((_tmp4B8=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B9="void datatype pattern has bad type",_tag_dyneither(_tmp4B9,sizeof(char),35))),_tag_dyneither(_tmp4B8,sizeof(void*),0))));}goto _LLFF;_LLFF:;}
# 928
s=Cyc_Tcpat_con_pat(r,*(*_tmp1DB->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1DC),p);
goto _LLCE;}_LLE3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1DD=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1DD->tag != 4)goto _LLE5;else{_tmp1DE=_tmp1DD->f1;}}_LLE4:
# 932
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1DE),p);goto _LLCE;_LLE5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1DF->tag != 6)goto _LLE7;else{if(_tmp1DF->f1 == 0)goto _LLE7;if((((_tmp1DF->f1)->aggr_info).KnownAggr).tag != 2)goto _LLE7;_tmp1E0=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1DF->f1)->aggr_info).KnownAggr).val);_tmp1E1=_tmp1DF->f3;}}_LLE6:
# 937
 if(_tmp1E0->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E0->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 941
const char*_tmp4BA;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4BA="",_tag_dyneither(_tmp4BA,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1E1;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_stmttmp1B=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Pat*_tmp1FF;struct _tuple15*_tmp1FD=_stmttmp1B;_tmp1FE=_tmp1FD->f1;_tmp1FF=_tmp1FD->f2;{
struct Cyc_List_List*_tmp200=_tmp1FE;struct _dyneither_ptr*_tmp202;_LL107: if(_tmp200 == 0)goto _LL109;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp201=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp200->hd);if(_tmp201->tag != 1)goto _LL109;else{_tmp202=_tmp201->f1;}}if(_tmp200->tl != 0)goto _LL109;_LL108:
# 946
 if(Cyc_strptrcmp(_tmp202,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4BB;ps=((_tmp4BB=_region_malloc(r,sizeof(*_tmp4BB)),((_tmp4BB->hd=Cyc_Tcpat_compile_pat(r,_tmp1FF),((_tmp4BB->tl=ps,_tmp4BB))))));}
found=1;}
# 950
goto _LL106;_LL109:;_LL10A: {
const char*_tmp4BE;void*_tmp4BD;(_tmp4BD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BE="bad designator(s)",_tag_dyneither(_tmp4BE,sizeof(char),18))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}_LL106:;};}}
# 954
if(!found){
const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C1="bad designator",_tag_dyneither(_tmp4C1,sizeof(char),15))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}}}
# 957
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 960
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E0->impl))->tagged){
const char*_tmp4C4;void*_tmp4C3;(_tmp4C3=0,Cyc_Tcutil_terr(p->loc,((_tmp4C4="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4C4,sizeof(char),47))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}{
int*_tmp4C5;int*span=(_tmp4C5=_region_malloc(r,sizeof(*_tmp4C5)),((_tmp4C5[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E0->impl))->fields),_tmp4C5)));
struct Cyc_List_List*_tmp20B=_tmp1E1;struct _dyneither_ptr*_tmp20D;struct Cyc_Absyn_Pat*_tmp20E;_LL10C: if(_tmp20B == 0)goto _LL10E;if(((struct _tuple15*)_tmp20B->hd)->f1 == 0)goto _LL10E;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp20C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp20B->hd)->f1)->hd);if(_tmp20C->tag != 1)goto _LL10E;else{_tmp20D=_tmp20C->f1;}}if((((struct _tuple15*)_tmp20B->hd)->f1)->tl != 0)goto _LL10E;_tmp20E=((struct _tuple15*)_tmp20B->hd)->f2;if(_tmp20B->tl != 0)goto _LL10E;_LL10D:
# 965
{struct Cyc_List_List*_tmp4C6;s=Cyc_Tcpat_con_pat(r,*_tmp20D,span,((_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6)),((_tmp4C6->hd=Cyc_Tcpat_compile_pat(r,_tmp20E),((_tmp4C6->tl=0,_tmp4C6)))))),p);}
goto _LL10B;_LL10E:;_LL10F: {
const char*_tmp4C9;void*_tmp4C8;(_tmp4C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C9="bad union pattern",_tag_dyneither(_tmp4C9,sizeof(char),18))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}_LL10B:;};}
# 970
goto _LLCE;_LLE7: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1E2=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1E2->tag != 12)goto _LLE9;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LLE8:
# 975
{void*_stmttmp1A=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp213=_stmttmp1A;_LL111: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 6)goto _LL113;}_LL112:
# 979
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,0,0,p);
goto _LL110;_LL113:;_LL114: {
# 982
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E3->fields))->v);
{int*_tmp4CA;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,((_tmp4CA=_region_malloc(r,sizeof(*_tmp4CA)),((_tmp4CA[0]=span,_tmp4CA)))),0,p);}
goto _LL110;}_LL110:;}
# 986
goto _LLCE;_LLE9: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1E5=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1E5->tag != 13)goto _LLEB;else{_tmp1E6=(void*)_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;}}_LLEA: {
# 991
struct Cyc_List_List*fields;
{void*_stmttmp18=Cyc_Tcutil_compress(_tmp1E6);void*_tmp216=_stmttmp18;struct Cyc_List_List*_tmp218;_LL116: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 14)goto _LL118;else{_tmp218=_tmp217->f1;}}_LL117:
 fields=_tmp218;goto _LL115;_LL118:;_LL119: {
const char*_tmp4CD;void*_tmp4CC;(_tmp4CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CD="bad type in AnonEnum_p",_tag_dyneither(_tmp4CD,sizeof(char),23))),_tag_dyneither(_tmp4CC,sizeof(void*),0)));}_LL115:;}
# 998
{void*_stmttmp19=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp21B=_stmttmp19;_LL11B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21B;if(_tmp21C->tag != 6)goto _LL11D;}_LL11C:
# 1002
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1E7->name).f2,0,0,p);
goto _LL11A;_LL11D:;_LL11E: {
# 1005
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4CE;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E7->name).f2,((_tmp4CE=_region_malloc(r,sizeof(*_tmp4CE)),((_tmp4CE[0]=span,_tmp4CE)))),0,p);}
goto _LL11A;}_LL11A:;}
# 1009
goto _LLCE;}_LLEB: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1E8=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1E8->tag != 14)goto _LLED;}_LLEC:
 goto _LLEE;_LLED: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E9=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1E9->tag != 15)goto _LLEF;}_LLEE:
 goto _LLF0;_LLEF: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1EA=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1EA->tag != 6)goto _LLF1;}_LLF0:
 goto _LLF2;_LLF1: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1EB=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1C8;if(_tmp1EB->tag != 16)goto _LLCE;}_LLF2: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4D1;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4D0;s=(void*)((_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4D1.tag=0,_tmp4D1)),_tmp4D0))));}_LLCE:;}
# 1015
return s;}
# 1023
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1034
typedef void*Cyc_Tcpat_term_desc_t;
# 1038
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1045
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1054
typedef void*Cyc_Tcpat_decision_t;struct _tuple19{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1059
typedef struct _tuple19*Cyc_Tcpat_work_stack_frame_t;
# 1062
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1064
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1067
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4D4;void*_tmp4D3;(_tmp4D3=0,Cyc_fprintf(Cyc_stderr,((_tmp4D4=" ",_tag_dyneither(_tmp4D4,sizeof(char),2))),_tag_dyneither(_tmp4D3,sizeof(void*),0)));}}
# 1073
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp222=c->name;
union Cyc_Tcpat_Name_value _tmp223=_tmp222;struct _dyneither_ptr _tmp224;int _tmp225;_LL120: if((_tmp223.Name_v).tag != 1)goto _LL122;_tmp224=(struct _dyneither_ptr)(_tmp223.Name_v).val;_LL121:
{const char*_tmp4D8;void*_tmp4D7[1];struct Cyc_String_pa_PrintArg_struct _tmp4D6;(_tmp4D6.tag=0,((_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp224),((_tmp4D7[0]=& _tmp4D6,Cyc_fprintf(Cyc_stderr,((_tmp4D8="%s",_tag_dyneither(_tmp4D8,sizeof(char),3))),_tag_dyneither(_tmp4D7,sizeof(void*),1)))))));}goto _LL11F;_LL122: if((_tmp223.Int_v).tag != 2)goto _LL11F;_tmp225=(int)(_tmp223.Int_v).val;_LL123:
{const char*_tmp4DC;void*_tmp4DB[1];struct Cyc_Int_pa_PrintArg_struct _tmp4DA;(_tmp4DA.tag=1,((_tmp4DA.f1=(unsigned long)_tmp225,((_tmp4DB[0]=& _tmp4DA,Cyc_fprintf(Cyc_stderr,((_tmp4DC="%d",_tag_dyneither(_tmp4DC,sizeof(char),3))),_tag_dyneither(_tmp4DB,sizeof(void*),1)))))));}goto _LL11F;_LL11F:;}
# 1081
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp22C=d;void*_tmp22E;struct Cyc_List_List*_tmp231;struct Cyc_Tcpat_Con_s*_tmp232;void*_tmp233;void*_tmp234;_LL125: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp22D=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp22C;if(_tmp22D->tag != 1)goto _LL127;else{_tmp22E=(void*)_tmp22D->f1;}}_LL126:
# 1084
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,Cyc_fprintf(Cyc_stderr,((_tmp4DF="Success(",_tag_dyneither(_tmp4DF,sizeof(char),9))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}print_rhs(_tmp22E);{const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,Cyc_fprintf(Cyc_stderr,((_tmp4E2=")\n",_tag_dyneither(_tmp4E2,sizeof(char),3))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}
goto _LL124;_LL127: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp22F=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp22C;if(_tmp22F->tag != 0)goto _LL129;}_LL128:
{const char*_tmp4E5;void*_tmp4E4;(_tmp4E4=0,Cyc_fprintf(Cyc_stderr,((_tmp4E5="Failure\n",_tag_dyneither(_tmp4E5,sizeof(char),9))),_tag_dyneither(_tmp4E4,sizeof(void*),0)));}goto _LL124;_LL129: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp230=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp22C;if(_tmp230->tag != 2)goto _LL124;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;_tmp233=(void*)_tmp230->f3;_tmp234=(void*)_tmp230->f4;}}_LL12A:
# 1089
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4E8;void*_tmp4E7;(_tmp4E7=0,Cyc_fprintf(Cyc_stderr,((_tmp4E8="Access[",_tag_dyneither(_tmp4E8,sizeof(char),8))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
{const char*_tmp4EC;void*_tmp4EB[1];struct Cyc_Int_pa_PrintArg_struct _tmp4EA;(_tmp4EA.tag=1,((_tmp4EA.f1=(unsigned long)((int)_tmp231->hd),((_tmp4EB[0]=& _tmp4EA,Cyc_fprintf(Cyc_stderr,((_tmp4EC="%d",_tag_dyneither(_tmp4EC,sizeof(char),3))),_tag_dyneither(_tmp4EB,sizeof(void*),1)))))));}
if(_tmp231->tl != 0){const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,Cyc_fprintf(Cyc_stderr,((_tmp4EF=",",_tag_dyneither(_tmp4EF,sizeof(char),2))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}}
# 1095
{const char*_tmp4F2;void*_tmp4F1;(_tmp4F1=0,Cyc_fprintf(Cyc_stderr,((_tmp4F2="],",_tag_dyneither(_tmp4F2,sizeof(char),3))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp232);
{const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((_tmp4F5=",\n",_tag_dyneither(_tmp4F5,sizeof(char),3))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp233,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp234,print_rhs,tab + 7);_LL124:;}struct _tuple20{int f1;unsigned int f2;};
# 1103
static void Cyc_Tcpat_print_swrhs(struct _tuple20*x){
return;}
# 1111
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp246=td;struct Cyc_Set_Set*_tmp248;_LL12C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp247=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp246;if(_tmp247->tag != 1)goto _LL12E;else{_tmp248=_tmp247->f1;}}_LL12D: {
# 1121
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4F8;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4F7;return(void*)((_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7[0]=((_tmp4F8.tag=1,((_tmp4F8.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp248,c),_tmp4F8)))),_tmp4F7))));}_LL12E: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp249=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp246;if(_tmp249->tag != 0)goto _LL12B;}_LL12F: {
const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FB="add_neg called when td is Positive",_tag_dyneither(_tmp4FB,sizeof(char),35))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}_LL12B:;}
# 1128
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp24E=td;struct Cyc_Tcpat_Con_s*_tmp250;struct Cyc_Set_Set*_tmp252;_LL131: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp24F=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp24E;if(_tmp24F->tag != 0)goto _LL133;else{_tmp250=_tmp24F->f1;}}_LL132:
# 1132
 if(Cyc_Tcpat_compare_con(c,_tmp250)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL133: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp251=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp24E;if(_tmp251->tag != 1)goto _LL130;else{_tmp252=_tmp251->f1;}}_LL134:
# 1136
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp252,c))return Cyc_Tcpat_No;else{
# 1139
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp252)+ 1)
return Cyc_Tcpat_Yes;else{
# 1142
return Cyc_Tcpat_Maybe;}}_LL130:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1150
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1152
struct Cyc_List_List*_tmp253=ctxt;struct Cyc_Tcpat_Con_s*_tmp254;struct Cyc_List_List*_tmp255;struct Cyc_List_List*_tmp256;_LL136: if(_tmp253 != 0)goto _LL138;_LL137:
 return 0;_LL138: if(_tmp253 == 0)goto _LL135;_tmp254=((struct _tuple21*)_tmp253->hd)->f1;_tmp255=((struct _tuple21*)_tmp253->hd)->f2;_tmp256=_tmp253->tl;_LL139: {
# 1155
struct _tuple21*_tmp501;struct Cyc_List_List*_tmp500;struct Cyc_List_List*_tmp4FF;return(_tmp4FF=_region_malloc(r,sizeof(*_tmp4FF)),((_tmp4FF->hd=(
(_tmp501=_region_malloc(r,sizeof(*_tmp501)),((_tmp501->f1=_tmp254,((_tmp501->f2=((_tmp500=_region_malloc(r,sizeof(*_tmp500)),((_tmp500->hd=dsc,((_tmp500->tl=_tmp255,_tmp500)))))),_tmp501)))))),((_tmp4FF->tl=_tmp256,_tmp4FF)))));}_LL135:;}
# 1163
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp25A=ctxt;struct Cyc_Tcpat_Con_s*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;_LL13B: if(_tmp25A != 0)goto _LL13D;_LL13C: {
const char*_tmp504;void*_tmp503;(_tmp503=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp504="norm_context: empty context",_tag_dyneither(_tmp504,sizeof(char),28))),_tag_dyneither(_tmp503,sizeof(void*),0)));}_LL13D: if(_tmp25A == 0)goto _LL13A;_tmp25B=((struct _tuple21*)_tmp25A->hd)->f1;_tmp25C=((struct _tuple21*)_tmp25A->hd)->f2;_tmp25D=_tmp25A->tl;_LL13E: {
# 1167
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp507;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp506;return Cyc_Tcpat_augment(r,_tmp25D,(void*)((_tmp506=_region_malloc(r,sizeof(*_tmp506)),((_tmp506[0]=((_tmp507.tag=0,((_tmp507.f1=_tmp25B,((_tmp507.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp25C),_tmp507)))))),_tmp506)))));}_LL13A:;}
# 1176
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1178
struct _tuple1 _tmp508;struct _tuple1 _stmttmp1E=(_tmp508.f1=ctxt,((_tmp508.f2=work,_tmp508)));struct _tuple1 _tmp262=_stmttmp1E;struct Cyc_Tcpat_Con_s*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp265;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp267;_LL140: if(_tmp262.f1 != 0)goto _LL142;if(_tmp262.f2 != 0)goto _LL142;_LL141:
 return dsc;_LL142: if(_tmp262.f1 != 0)goto _LL144;_LL143:
 goto _LL145;_LL144: if(_tmp262.f2 != 0)goto _LL146;_LL145: {
const char*_tmp50B;void*_tmp50A;(_tmp50A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50B="build_desc: ctxt and work don't match",_tag_dyneither(_tmp50B,sizeof(char),38))),_tag_dyneither(_tmp50A,sizeof(void*),0)));}_LL146: if(_tmp262.f1 == 0)goto _LL13F;_tmp263=((struct _tuple21*)(_tmp262.f1)->hd)->f1;_tmp264=((struct _tuple21*)(_tmp262.f1)->hd)->f2;_tmp265=(_tmp262.f1)->tl;if(_tmp262.f2 == 0)goto _LL13F;_tmp266=((struct _tuple19*)(_tmp262.f2)->hd)->f3;_tmp267=(_tmp262.f2)->tl;_LL147: {
# 1183
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp511;struct Cyc_List_List*_tmp510;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp50F;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp26A=(_tmp50F=_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp511.tag=0,((_tmp511.f1=_tmp263,((_tmp511.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp264),(
(_tmp510=_region_malloc(r,sizeof(*_tmp510)),((_tmp510->hd=dsc,((_tmp510->tl=_tmp266,_tmp510))))))),_tmp511)))))),_tmp50F)));
return Cyc_Tcpat_build_desc(r,_tmp265,(void*)_tmp26A,_tmp267);}_LL13F:;}
# 1189
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1196
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1198
struct Cyc_List_List*_tmp26F=allmrules;void*_tmp270;void*_tmp271;struct Cyc_List_List*_tmp272;_LL149: if(_tmp26F != 0)goto _LL14B;_LL14A: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp514;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp513;return(void*)((_tmp513=_cycalloc(sizeof(*_tmp513)),((_tmp513[0]=((_tmp514.tag=0,((_tmp514.f1=dsc,_tmp514)))),_tmp513))));}_LL14B: if(_tmp26F == 0)goto _LL148;_tmp270=((struct _tuple0*)_tmp26F->hd)->f1;_tmp271=((struct _tuple0*)_tmp26F->hd)->f2;_tmp272=_tmp26F->tl;_LL14C:
# 1201
 return Cyc_Tcpat_match(r,_tmp270,0,dsc,0,0,_tmp271,_tmp272);_LL148:;}
# 1206
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1208
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp517;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp516;return Cyc_Tcpat_or_match(r,(void*)((_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516[0]=((_tmp517.tag=1,((_tmp517.f1=Cyc_Tcpat_empty_con_set(r),_tmp517)))),_tmp516)))),allmrules);}
# 1214
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1218
struct Cyc_List_List*_tmp277=work;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27C;_LL14E: if(_tmp277 != 0)goto _LL150;_LL14F: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp51A;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp519;return(void*)((_tmp519=_region_malloc(r,sizeof(*_tmp519)),((_tmp519[0]=((_tmp51A.tag=1,((_tmp51A.f1=right_hand_side,_tmp51A)))),_tmp519))));}_LL150: if(_tmp277 == 0)goto _LL152;if(((struct _tuple19*)_tmp277->hd)->f1 != 0)goto _LL152;if(((struct _tuple19*)_tmp277->hd)->f2 != 0)goto _LL152;if(((struct _tuple19*)_tmp277->hd)->f3 != 0)goto _LL152;_tmp278=_tmp277->tl;_LL151:
# 1221
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp278,right_hand_side,rules);_LL152: if(_tmp277 == 0)goto _LL14D;_tmp279=((struct _tuple19*)_tmp277->hd)->f1;_tmp27A=((struct _tuple19*)_tmp277->hd)->f2;_tmp27B=((struct _tuple19*)_tmp277->hd)->f3;_tmp27C=_tmp277->tl;_LL153:
# 1223
 if((_tmp279 == 0  || _tmp27A == 0) || _tmp27B == 0){
const char*_tmp51D;void*_tmp51C;(_tmp51C=0,Cyc_Tcutil_impos(((_tmp51D="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp51D,sizeof(char),38))),_tag_dyneither(_tmp51C,sizeof(void*),0)));}{
struct Cyc_List_List*_stmttmp1F=_tmp279;void*_tmp282;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp281=_stmttmp1F;_tmp282=(void*)_tmp281->hd;_tmp283=_tmp281->tl;{
struct Cyc_List_List*_stmttmp20=_tmp27A;struct Cyc_List_List*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp284=_stmttmp20;_tmp285=(struct Cyc_List_List*)_tmp284->hd;_tmp286=_tmp284->tl;{
struct Cyc_List_List*_stmttmp21=_tmp27B;void*_tmp288;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp287=_stmttmp21;_tmp288=(void*)_tmp287->hd;_tmp289=_tmp287->tl;{
struct _tuple19*_tmp51E;struct _tuple19*_tmp28A=(_tmp51E=_region_malloc(r,sizeof(*_tmp51E)),((_tmp51E->f1=_tmp283,((_tmp51E->f2=_tmp286,((_tmp51E->f3=_tmp289,_tmp51E)))))));
struct Cyc_List_List*_tmp51F;return Cyc_Tcpat_match(r,_tmp282,_tmp285,_tmp288,ctx,((_tmp51F=_region_malloc(r,sizeof(*_tmp51F)),((_tmp51F->hd=_tmp28A,((_tmp51F->tl=_tmp27C,_tmp51F)))))),right_hand_side,rules);};};};};_LL14D:;}
# 1235
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1237
void*_tmp28D=dsc;struct Cyc_Set_Set*_tmp28F;struct Cyc_List_List*_tmp291;_LL155: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp28E=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp28D;if(_tmp28E->tag != 1)goto _LL157;else{_tmp28F=_tmp28E->f1;}}_LL156: {
# 1242
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp522;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp521;void*any=(void*)((_tmp521=_region_malloc(r,sizeof(*_tmp521)),((_tmp521[0]=((_tmp522.tag=1,((_tmp522.f1=Cyc_Tcpat_empty_con_set(r),_tmp522)))),_tmp521))));
struct Cyc_List_List*_tmp292=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp523;_tmp292=((_tmp523=_region_malloc(r,sizeof(*_tmp523)),((_tmp523->hd=any,((_tmp523->tl=_tmp292,_tmp523))))));}}
return _tmp292;}_LL157: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp290=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp28D;if(_tmp290->tag != 0)goto _LL154;else{_tmp291=_tmp290->f2;}}_LL158:
 return _tmp291;_LL154:;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1252
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple22*env,int i){
struct _RegionHandle*_tmp297;struct Cyc_List_List*_tmp298;struct _tuple22*_tmp296=env;_tmp297=_tmp296->f1;_tmp298=_tmp296->f2;{
struct Cyc_List_List*_tmp524;return(_tmp524=_region_malloc(_tmp297,sizeof(*_tmp524)),((_tmp524->hd=(void*)(i + 1),((_tmp524->tl=_tmp298,_tmp524)))));};}
# 1256
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1258
struct _tuple22 _tmp525;struct _tuple22 _tmp29A=(_tmp525.f1=r,((_tmp525.f2=obj,_tmp525)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple22*,int),struct _tuple22*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp29A);}
# 1265
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1269
void*_tmp29C=p;struct Cyc_Tcpat_Con_s*_tmp29F;struct Cyc_List_List*_tmp2A0;_LL15A: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp29D=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp29C;if(_tmp29D->tag != 0)goto _LL15C;}_LL15B:
# 1271
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL15C: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp29E=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp29C;if(_tmp29E->tag != 1)goto _LL159;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;}}_LL15D: {
# 1273
enum Cyc_Tcpat_Answer _stmttmp22=Cyc_Tcpat_static_match(_tmp29F,dsc);switch(_stmttmp22){case Cyc_Tcpat_Yes: _LL15E: {
# 1275
struct _tuple21*_tmp528;struct Cyc_List_List*_tmp527;struct Cyc_List_List*ctx2=(_tmp527=_region_malloc(r,sizeof(*_tmp527)),((_tmp527->hd=((_tmp528=_region_malloc(r,sizeof(*_tmp528)),((_tmp528->f1=_tmp29F,((_tmp528->f2=0,_tmp528)))))),((_tmp527->tl=ctx,_tmp527)))));
struct _tuple19*_tmp529;struct _tuple19*work_frame=(_tmp529=_region_malloc(r,sizeof(*_tmp529)),((_tmp529->f1=_tmp2A0,((_tmp529->f2=Cyc_Tcpat_getoargs(r,_tmp29F,obj),((_tmp529->f3=
Cyc_Tcpat_getdargs(r,_tmp29F,dsc),_tmp529)))))));
struct Cyc_List_List*_tmp52A;struct Cyc_List_List*work2=(_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A->hd=work_frame,((_tmp52A->tl=work,_tmp52A)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL15F:
# 1281
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL160: {
# 1283
struct _tuple21*_tmp52D;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*ctx2=(_tmp52C=_region_malloc(r,sizeof(*_tmp52C)),((_tmp52C->hd=((_tmp52D=_region_malloc(r,sizeof(*_tmp52D)),((_tmp52D->f1=_tmp29F,((_tmp52D->f2=0,_tmp52D)))))),((_tmp52C->tl=ctx,_tmp52C)))));
struct _tuple19*_tmp52E;struct _tuple19*work_frame=(_tmp52E=_region_malloc(r,sizeof(*_tmp52E)),((_tmp52E->f1=_tmp2A0,((_tmp52E->f2=Cyc_Tcpat_getoargs(r,_tmp29F,obj),((_tmp52E->f3=
Cyc_Tcpat_getdargs(r,_tmp29F,dsc),_tmp52E)))))));
struct Cyc_List_List*_tmp52F;struct Cyc_List_List*work2=(_tmp52F=_region_malloc(r,sizeof(*_tmp52F)),((_tmp52F->hd=work_frame,((_tmp52F->tl=work,_tmp52F)))));
void*_tmp2A5=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2A6=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp29F),work),rules);
# 1290
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp532;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp531;return(void*)((_tmp531=_region_malloc(r,sizeof(*_tmp531)),((_tmp531[0]=((_tmp532.tag=2,((_tmp532.f1=obj,((_tmp532.f2=_tmp29F,((_tmp532.f3=_tmp2A5,((_tmp532.f4=_tmp2A6,_tmp532)))))))))),_tmp531))));}}}_LL159:;}
# 1300
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1306
void*_tmp2AD=d;void*_tmp2AF;void*_tmp2B1;void*_tmp2B3;void*_tmp2B4;_LL163: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2AE=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2AD;if(_tmp2AE->tag != 0)goto _LL165;else{_tmp2AF=(void*)_tmp2AE->f1;}}_LL164:
# 1308
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2AF);*exhaust_done=1;}
goto _LL162;_LL165: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2B0=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2AD;if(_tmp2B0->tag != 1)goto _LL167;else{_tmp2B1=(void*)_tmp2B0->f1;}}_LL166:
 rhs_appears(env2,_tmp2B1);goto _LL162;_LL167: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2B2=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2AD;if(_tmp2B2->tag != 2)goto _LL162;else{_tmp2B3=(void*)_tmp2B2->f3;_tmp2B4=(void*)_tmp2B2->f4;}}_LL168:
# 1312
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B3,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1314
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B4,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1316
goto _LL162;_LL162:;}struct _tuple23{void*f1;struct _tuple20*f2;};struct _tuple24{struct _RegionHandle*f1;int*f2;};
# 1324
static struct _tuple23*Cyc_Tcpat_get_match(struct _tuple24*env,struct Cyc_Absyn_Switch_clause*swc){
# 1327
struct _RegionHandle*_tmp2B6;int*_tmp2B7;struct _tuple24*_tmp2B5=env;_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2B6,swc->pattern);
struct _tuple20*_tmp533;struct _tuple20*rhs=(_tmp533=_region_malloc(_tmp2B6,sizeof(*_tmp533)),((_tmp533->f1=0,((_tmp533->f2=(swc->pattern)->loc,_tmp533)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp535;sp=Cyc_Tcpat_tuple_pat(_tmp2B6,((_tmp535=_region_malloc(_tmp2B6,sizeof(*_tmp535)),((_tmp535->hd=sp0,((_tmp535->tl=(
(_tmp536=_region_malloc(_tmp2B6,sizeof(*_tmp536)),((_tmp536->hd=Cyc_Tcpat_int_pat(_tmp2B6,*_tmp2B7,0),((_tmp536->tl=0,_tmp536)))))),_tmp535)))))),0);}
*_tmp2B7=*_tmp2B7 + 1;}else{
# 1336
struct Cyc_List_List*_tmp540;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp53F;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp53E;struct Cyc_List_List*_tmp53D;sp=Cyc_Tcpat_tuple_pat(_tmp2B6,((_tmp53D=_region_malloc(_tmp2B6,sizeof(*_tmp53D)),((_tmp53D->hd=sp0,((_tmp53D->tl=((_tmp540=_region_malloc(_tmp2B6,sizeof(*_tmp540)),((_tmp540->hd=(void*)((_tmp53E=_region_malloc(_tmp2B6,sizeof(*_tmp53E)),((_tmp53E[0]=((_tmp53F.tag=0,_tmp53F)),_tmp53E)))),((_tmp540->tl=0,_tmp540)))))),_tmp53D)))))),0);}{
struct _tuple23*_tmp541;return(_tmp541=_region_malloc(_tmp2B6,sizeof(*_tmp541)),((_tmp541->f1=sp,((_tmp541->f2=rhs,_tmp541)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1342
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1344
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2C1=d;struct Cyc_Tcpat_Con_s*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct Cyc_Set_Set*_tmp2C6;_LL16A: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C2=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2C1;if(_tmp2C2->tag != 0)goto _LL16C;else{_tmp2C3=_tmp2C2->f1;_tmp2C4=_tmp2C2->f2;}}_LL16B: {
# 1348
union Cyc_Tcpat_Name_value _tmp2C7=_tmp2C3->name;
struct Cyc_Absyn_Pat*_tmp2C8=_tmp2C3->orig_pat;
if(_tmp2C8 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2C4))->hd);{
# 1353
void*_stmttmp25=_tmp2C8->r;void*_tmp2C9=_stmttmp25;struct Cyc_Absyn_Vardecl*_tmp2CC;struct Cyc_Absyn_Vardecl*_tmp2CE;struct Cyc_Absyn_Tvar*_tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D1;struct Cyc_Absyn_Aggrdecl*_tmp2D5;struct Cyc_List_List*_tmp2D6;struct Cyc_Absyn_Datatypefield*_tmp2D8;int _tmp2DB;char _tmp2DD;struct _dyneither_ptr _tmp2DF;int _tmp2E0;struct Cyc_Absyn_Enumfield*_tmp2E2;struct Cyc_Absyn_Enumfield*_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E6;_LL16F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2CA=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2CA->tag != 0)goto _LL171;}_LL170: {
const char*_tmp542;return(_tmp542="_",_tag_dyneither(_tmp542,sizeof(char),2));}_LL171: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2CB=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2CB->tag != 1)goto _LL173;else{_tmp2CC=_tmp2CB->f1;}}_LL172:
 return Cyc_Absynpp_qvar2string(_tmp2CC->name);_LL173: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2CD=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2CD->tag != 2)goto _LL175;else{_tmp2CE=_tmp2CD->f1;}}_LL174: {
const char*_tmp546;void*_tmp545[1];struct Cyc_String_pa_PrintArg_struct _tmp544;return(struct _dyneither_ptr)((_tmp544.tag=0,((_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2CE->name)),((_tmp545[0]=& _tmp544,Cyc_aprintf(((_tmp546="*%s",_tag_dyneither(_tmp546,sizeof(char),4))),_tag_dyneither(_tmp545,sizeof(void*),1))))))));}_LL175: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2CF=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2CF->tag != 3)goto _LL177;else{_tmp2D0=_tmp2CF->f1;_tmp2D1=_tmp2CF->f2;}}_LL176: {
const char*_tmp54B;void*_tmp54A[2];struct Cyc_String_pa_PrintArg_struct _tmp549;struct Cyc_String_pa_PrintArg_struct _tmp548;return(struct _dyneither_ptr)((_tmp548.tag=0,((_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2D0->name),((_tmp549.tag=0,((_tmp549.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D1->name)),((_tmp54A[0]=& _tmp549,((_tmp54A[1]=& _tmp548,Cyc_aprintf(((_tmp54B="%s<`%s>",_tag_dyneither(_tmp54B,sizeof(char),8))),_tag_dyneither(_tmp54A,sizeof(void*),2))))))))))))));}_LL177: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2D2=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2D2->tag != 4)goto _LL179;}_LL178: {
# 1359
const char*_tmp54F;void*_tmp54E[1];struct Cyc_String_pa_PrintArg_struct _tmp54D;return(struct _dyneither_ptr)((_tmp54D.tag=0,((_tmp54D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C4)),((_tmp54E[0]=& _tmp54D,Cyc_aprintf(((_tmp54F="$(%s)",_tag_dyneither(_tmp54F,sizeof(char),6))),_tag_dyneither(_tmp54E,sizeof(void*),1))))))));}_LL179: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2D3=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2D3->tag != 5)goto _LL17B;}_LL17A: {
# 1361
const char*_tmp553;void*_tmp552[1];struct Cyc_String_pa_PrintArg_struct _tmp551;return(struct _dyneither_ptr)((_tmp551.tag=0,((_tmp551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C4)),((_tmp552[0]=& _tmp551,Cyc_aprintf(((_tmp553="&%s",_tag_dyneither(_tmp553,sizeof(char),4))),_tag_dyneither(_tmp552,sizeof(void*),1))))))));}_LL17B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2D4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2D4->tag != 6)goto _LL17D;else{if(_tmp2D4->f1 == 0)goto _LL17D;if((((_tmp2D4->f1)->aggr_info).KnownAggr).tag != 2)goto _LL17D;_tmp2D5=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2D4->f1)->aggr_info).KnownAggr).val);_tmp2D6=_tmp2D4->f3;}}_LL17C:
# 1363
 if(_tmp2D5->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2F5=_tmp2D6;struct _dyneither_ptr*_tmp2F7;_LL190: if(_tmp2F5 == 0)goto _LL192;if(((struct _tuple15*)_tmp2F5->hd)->f1 == 0)goto _LL192;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2F6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2F5->hd)->f1)->hd);if(_tmp2F6->tag != 1)goto _LL192;else{_tmp2F7=_tmp2F6->f1;}};_LL191: {
# 1366
const char*_tmp559;void*_tmp558[3];struct Cyc_String_pa_PrintArg_struct _tmp557;struct Cyc_String_pa_PrintArg_struct _tmp556;struct Cyc_String_pa_PrintArg_struct _tmp555;return(struct _dyneither_ptr)((_tmp555.tag=0,((_tmp555.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2C4)),((_tmp556.tag=0,((_tmp556.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F7),((_tmp557.tag=0,((_tmp557.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1366
Cyc_Absynpp_qvar2string(_tmp2D5->name)),((_tmp558[0]=& _tmp557,((_tmp558[1]=& _tmp556,((_tmp558[2]=& _tmp555,Cyc_aprintf(((_tmp559="%s{.%s = %s}",_tag_dyneither(_tmp559,sizeof(char),13))),_tag_dyneither(_tmp558,sizeof(void*),3))))))))))))))))))));}_LL192:;_LL193:
# 1368
 goto _LL18F;_LL18F:;}{
# 1371
const char*_tmp55E;void*_tmp55D[2];struct Cyc_String_pa_PrintArg_struct _tmp55C;struct Cyc_String_pa_PrintArg_struct _tmp55B;return(struct _dyneither_ptr)((_tmp55B.tag=0,((_tmp55B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C4)),((_tmp55C.tag=0,((_tmp55C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D5->name)),((_tmp55D[0]=& _tmp55C,((_tmp55D[1]=& _tmp55B,Cyc_aprintf(((_tmp55E="%s{%s}",_tag_dyneither(_tmp55E,sizeof(char),7))),_tag_dyneither(_tmp55D,sizeof(void*),2))))))))))))));};_LL17D: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2D7=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2D7->tag != 7)goto _LL17F;else{_tmp2D8=_tmp2D7->f2;}}_LL17E:
# 1373
 if(_tmp2C4 == 0){
const char*_tmp562;void*_tmp561[1];struct Cyc_String_pa_PrintArg_struct _tmp560;return(struct _dyneither_ptr)((_tmp560.tag=0,((_tmp560.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D8->name)),((_tmp561[0]=& _tmp560,Cyc_aprintf(((_tmp562="%s",_tag_dyneither(_tmp562,sizeof(char),3))),_tag_dyneither(_tmp561,sizeof(void*),1))))))));}else{
# 1376
const char*_tmp567;void*_tmp566[2];struct Cyc_String_pa_PrintArg_struct _tmp565;struct Cyc_String_pa_PrintArg_struct _tmp564;return(struct _dyneither_ptr)((_tmp564.tag=0,((_tmp564.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C4)),((_tmp565.tag=0,((_tmp565.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D8->name)),((_tmp566[0]=& _tmp565,((_tmp566[1]=& _tmp564,Cyc_aprintf(((_tmp567="%s(%s)",_tag_dyneither(_tmp567,sizeof(char),7))),_tag_dyneither(_tmp566,sizeof(void*),2))))))))))))));}_LL17F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2D9=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2D9->tag != 8)goto _LL181;}_LL180: {
const char*_tmp568;return(_tmp568="NULL",_tag_dyneither(_tmp568,sizeof(char),5));}_LL181: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2DA=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2DA->tag != 9)goto _LL183;else{_tmp2DB=_tmp2DA->f2;}}_LL182: {
const char*_tmp56C;void*_tmp56B[1];struct Cyc_Int_pa_PrintArg_struct _tmp56A;return(struct _dyneither_ptr)((_tmp56A.tag=1,((_tmp56A.f1=(unsigned long)_tmp2DB,((_tmp56B[0]=& _tmp56A,Cyc_aprintf(((_tmp56C="%d",_tag_dyneither(_tmp56C,sizeof(char),3))),_tag_dyneither(_tmp56B,sizeof(void*),1))))))));}_LL183: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2DC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2DC->tag != 10)goto _LL185;else{_tmp2DD=_tmp2DC->f1;}}_LL184: {
const char*_tmp570;void*_tmp56F[1];struct Cyc_Int_pa_PrintArg_struct _tmp56E;return(struct _dyneither_ptr)((_tmp56E.tag=1,((_tmp56E.f1=(unsigned long)((int)_tmp2DD),((_tmp56F[0]=& _tmp56E,Cyc_aprintf(((_tmp570="%d",_tag_dyneither(_tmp570,sizeof(char),3))),_tag_dyneither(_tmp56F,sizeof(void*),1))))))));}_LL185: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2DE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2DE->tag != 11)goto _LL187;else{_tmp2DF=_tmp2DE->f1;_tmp2E0=_tmp2DE->f2;}}_LL186:
 return _tmp2DF;_LL187: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2E1=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2E1->tag != 12)goto _LL189;else{_tmp2E2=_tmp2E1->f2;}}_LL188:
 _tmp2E4=_tmp2E2;goto _LL18A;_LL189: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2E3=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2E3->tag != 13)goto _LL18B;else{_tmp2E4=_tmp2E3->f2;}}_LL18A:
 return Cyc_Absynpp_qvar2string(_tmp2E4->name);_LL18B: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2E5=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2C9;if(_tmp2E5->tag != 16)goto _LL18D;else{_tmp2E6=_tmp2E5->f1;}}_LL18C:
 return Cyc_Absynpp_exp2string(_tmp2E6);_LL18D:;_LL18E:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL16E:;};}_LL16C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2C5=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2C1;if(_tmp2C5->tag != 1)goto _LL169;else{_tmp2C6=_tmp2C5->f1;}}_LL16D:
# 1387
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2C6)){const char*_tmp571;return(_tmp571="_",_tag_dyneither(_tmp571,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp310=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2C6);
struct Cyc_Absyn_Pat*_tmp311=_tmp310->orig_pat;
# 1392
if(_tmp311 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_stmttmp23=Cyc_Tcutil_compress((void*)_check_null(_tmp311->topt));void*_tmp312=_stmttmp23;struct Cyc_Absyn_PtrAtts _tmp316;struct Cyc_Absyn_Datatypedecl*_tmp318;struct Cyc_Absyn_Aggrdecl*_tmp31A;_LL195: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 6)goto _LL197;else{if(_tmp313->f2 != Cyc_Absyn_Char_sz)goto _LL197;}}_LL196:
# 1396
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp31B=Cyc_Tcpat_char_con(r,(char)i,_tmp311);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C6,_tmp31B)){
const char*_tmp575;void*_tmp574[1];struct Cyc_Int_pa_PrintArg_struct _tmp573;return(struct _dyneither_ptr)((_tmp573.tag=1,((_tmp573.f1=(unsigned long)i,((_tmp574[0]=& _tmp573,Cyc_aprintf(((_tmp575="%d",_tag_dyneither(_tmp575,sizeof(char),3))),_tag_dyneither(_tmp574,sizeof(void*),1))))))));}}}
# 1401
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL197: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312;if(_tmp314->tag != 6)goto _LL199;}_LL198:
# 1404
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp31F=Cyc_Tcpat_int_con(r,(int)i,_tmp311);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C6,_tmp31F)){
const char*_tmp579;void*_tmp578[1];struct Cyc_Int_pa_PrintArg_struct _tmp577;return(struct _dyneither_ptr)((_tmp577.tag=1,((_tmp577.f1=(unsigned long)((int)i),((_tmp578[0]=& _tmp577,Cyc_aprintf(((_tmp579="%d",_tag_dyneither(_tmp579,sizeof(char),3))),_tag_dyneither(_tmp578,sizeof(void*),1))))))));}}}
# 1409
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL199: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312;if(_tmp315->tag != 5)goto _LL19B;else{_tmp316=(_tmp315->f1).ptr_atts;}}_LL19A: {
# 1411
union Cyc_Absyn_Constraint*_tmp323=_tmp316.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp323);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C6,Cyc_Tcpat_null_con(r,_tmp311))){
const char*_tmp57A;return(_tmp57A="NULL",_tag_dyneither(_tmp57A,sizeof(char),5));}}{
# 1417
const char*_tmp57B;return(_tmp57B="&_",_tag_dyneither(_tmp57B,sizeof(char),3));};}_LL19B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312;if(_tmp317->tag != 3)goto _LL19D;else{if((((_tmp317->f1).datatype_info).KnownDatatype).tag != 2)goto _LL19D;_tmp318=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp317->f1).datatype_info).KnownDatatype).val);}}_LL19C:
# 1419
 if(_tmp318->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp326=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp318->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp326);
for(0;(unsigned int)_tmp326;_tmp326=_tmp326->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp326->hd)->name).f2;
struct Cyc_List_List*_tmp327=((struct Cyc_Absyn_Datatypefield*)_tmp326->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp57C;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C6,((_tmp57C=_cycalloc(sizeof(*_tmp57C)),((_tmp57C->name=Cyc_Tcpat_Name_v(n),((_tmp57C->arity=0,((_tmp57C->span=0,((_tmp57C->orig_pat=_tmp311,_tmp57C)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp327)== 0)
return n;else{
# 1429
const char*_tmp580;void*_tmp57F[1];struct Cyc_String_pa_PrintArg_struct _tmp57E;return(struct _dyneither_ptr)((_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp57F[0]=& _tmp57E,Cyc_aprintf(((_tmp580="%s(...)",_tag_dyneither(_tmp580,sizeof(char),8))),_tag_dyneither(_tmp57F,sizeof(void*),1))))))));}}}
# 1432
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL19D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp319=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312;if(_tmp319->tag != 11)goto _LL19F;else{if((((_tmp319->f1).aggr_info).KnownAggr).tag != 2)goto _LL19F;_tmp31A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp319->f1).aggr_info).KnownAggr).val);}}if(!(_tmp31A->kind == Cyc_Absyn_UnionA))goto _LL19F;_LL19E: {
# 1434
struct Cyc_List_List*_tmp32C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp31A->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32C);
struct _tuple2*_stmttmp24=_tmp31A->name;struct _dyneither_ptr _tmp32E;struct _tuple2*_tmp32D=_stmttmp24;_tmp32E=*_tmp32D->f2;
for(0;(unsigned int)_tmp32C;_tmp32C=_tmp32C->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp32C->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp581;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C6,((_tmp581=_cycalloc(sizeof(*_tmp581)),((_tmp581->name=Cyc_Tcpat_Name_v(n),((_tmp581->arity=0,((_tmp581->span=0,((_tmp581->orig_pat=_tmp311,_tmp581)))))))))))){
const char*_tmp586;void*_tmp585[2];struct Cyc_String_pa_PrintArg_struct _tmp584;struct Cyc_String_pa_PrintArg_struct _tmp583;return(struct _dyneither_ptr)((_tmp583.tag=0,((_tmp583.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp584.tag=0,((_tmp584.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32E),((_tmp585[0]=& _tmp584,((_tmp585[1]=& _tmp583,Cyc_aprintf(((_tmp586="%s{.%s = _}",_tag_dyneither(_tmp586,sizeof(char),12))),_tag_dyneither(_tmp585,sizeof(void*),2))))))))))))));}}
# 1442
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL194:;};};_LL169:;}
# 1447
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp587;return(_tmp587=_cycalloc(sizeof(*_tmp587)),((_tmp587[0]=Cyc_Tcpat_desc2string(r,d),_tmp587)));}
# 1450
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp335=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp336=_tmp335;for(0;_tmp336 != 0;_tmp336=((struct Cyc_List_List*)_check_null(_tmp336))->tl){
if(_tmp336->tl != 0){
{struct Cyc_List_List*_tmp588;_tmp336->tl=((_tmp588=_cycalloc(sizeof(*_tmp588)),((_tmp588->hd=comma,((_tmp588->tl=_tmp336->tl,_tmp588))))));}
_tmp336=_tmp336->tl;}}}
# 1459
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp335);}
# 1462
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp339;_push_handler(& _tmp339);{int _tmp33B=0;if(setjmp(_tmp339.handler))_tmp33B=1;if(!_tmp33B){
{struct _dyneither_ptr _tmp33C=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp58C;void*_tmp58B[1];struct Cyc_String_pa_PrintArg_struct _tmp58A;(_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33C),((_tmp58B[0]=& _tmp58A,Cyc_Tcutil_terr(loc,((_tmp58C="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp58C,sizeof(char),53))),_tag_dyneither(_tmp58B,sizeof(void*),1)))))));}
# 1464
;_pop_handler();}else{void*_tmp33A=(void*)_exn_thrown;void*_tmp341=_tmp33A;void*_tmp343;_LL1A2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp342=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp341;if(_tmp342->tag != Cyc_Tcpat_Desc2string)goto _LL1A4;}_LL1A3:
# 1468
{const char*_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_Tcutil_terr(loc,((_tmp58F="patterns may not be exhaustive.",_tag_dyneither(_tmp58F,sizeof(char),32))),_tag_dyneither(_tmp58E,sizeof(void*),0)));}
goto _LL1A1;_LL1A4: _tmp343=_tmp341;_LL1A5:(void)_throw(_tmp343);_LL1A1:;}};}
# 1472
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple20*rhs){
(*rhs).f1=1;}
# 1476
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1481
struct _RegionHandle _tmp346=_new_region("r");struct _RegionHandle*r=& _tmp346;_push_region(r);
{int _tmp347=0;
struct _tuple24 _tmp590;struct _tuple24 _tmp348=(_tmp590.f1=r,((_tmp590.f2=& _tmp347,_tmp590)));
struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _tuple24*,struct Cyc_Absyn_Switch_clause*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp348,swcs);
void*_tmp34A=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp349);
int _tmp34B=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp34A,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp34B);
# 1489
for(0;_tmp349 != 0;_tmp349=_tmp349->tl){
struct _tuple23*_stmttmp26=(struct _tuple23*)_tmp349->hd;int _tmp34D;unsigned int _tmp34E;struct _tuple23*_tmp34C=_stmttmp26;_tmp34D=(_tmp34C->f2)->f1;_tmp34E=(_tmp34C->f2)->f2;
if(!_tmp34D){
{const char*_tmp593;void*_tmp592;(_tmp592=0,Cyc_Tcutil_terr(_tmp34E,((_tmp593="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp593,sizeof(char),74))),_tag_dyneither(_tmp592,sizeof(void*),0)));}
# 1494
break;}}}
# 1482
;_pop_region(r);}
# 1504
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp352;_push_handler(& _tmp352);{int _tmp354=0;if(setjmp(_tmp352.handler))_tmp354=1;if(!_tmp354){
{struct _dyneither_ptr _tmp355=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp597;void*_tmp596[1];struct Cyc_String_pa_PrintArg_struct _tmp595;(_tmp595.tag=0,((_tmp595.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp355),((_tmp596[0]=& _tmp595,Cyc_Tcutil_warn((*pr).f1,((_tmp597="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp597,sizeof(char),45))),_tag_dyneither(_tmp596,sizeof(void*),1)))))));}
# 1507
;_pop_handler();}else{void*_tmp353=(void*)_exn_thrown;void*_tmp35A=_tmp353;void*_tmp35C;_LL1A7: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp35B=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp35A;if(_tmp35B->tag != Cyc_Tcpat_Desc2string)goto _LL1A9;}_LL1A8:
# 1511
{const char*_tmp59A;void*_tmp599;(_tmp599=0,Cyc_Tcutil_warn((*pr).f1,((_tmp59A="pattern not exhaustive.",_tag_dyneither(_tmp59A,sizeof(char),24))),_tag_dyneither(_tmp599,sizeof(void*),0)));}
goto _LL1A6;_LL1A9: _tmp35C=_tmp35A;_LL1AA:(void)_throw(_tmp35C);_LL1A6:;}};};}
# 1515
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple25{void*f1;int f2;};
# 1518
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp35F=_new_region("r");struct _RegionHandle*r=& _tmp35F;_push_region(r);
{struct _tuple25*_tmp59D;struct Cyc_List_List*_tmp59C;struct Cyc_List_List*_tmp360=(_tmp59C=_region_malloc(r,sizeof(*_tmp59C)),((_tmp59C->hd=((_tmp59D=_region_malloc(r,sizeof(*_tmp59D)),((_tmp59D->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp59D->f2=0,_tmp59D)))))),((_tmp59C->tl=0,_tmp59C)))));
void*_tmp361=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp360);
struct _tuple14 _tmp59E;struct _tuple14 _tmp362=(_tmp59E.f1=loc,((_tmp59E.f2=1,_tmp59E)));
int _tmp363=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp361,Cyc_Tcpat_not_exhaust_warn,& _tmp362,Cyc_Tcpat_dummy_fn,0,& _tmp363);{
# 1526
int _tmp364=_tmp362.f2;_npop_handler(0);return _tmp364;};}
# 1520
;_pop_region(r);}
# 1534
static struct _tuple23*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1536
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple20*_tmp59F;struct _tuple20*rhs=(_tmp59F=_region_malloc(r,sizeof(*_tmp59F)),((_tmp59F->f1=0,((_tmp59F->f2=(swc->pattern)->loc,_tmp59F)))));
struct _tuple23*_tmp5A0;return(_tmp5A0=_region_malloc(r,sizeof(*_tmp5A0)),((_tmp5A0->f1=sp0,((_tmp5A0->f2=rhs,_tmp5A0)))));}
# 1540
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1543
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1545
struct _RegionHandle _tmp36A=_new_region("r");struct _RegionHandle*r=& _tmp36A;_push_region(r);
{struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp36C=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp36B);
int _tmp36D=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp36C,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp36D);
# 1551
for(0;_tmp36B != 0;_tmp36B=_tmp36B->tl){
struct _tuple23*_stmttmp27=(struct _tuple23*)_tmp36B->hd;int _tmp36F;unsigned int _tmp370;struct _tuple23*_tmp36E=_stmttmp27;_tmp36F=(_tmp36E->f2)->f1;_tmp370=(_tmp36E->f2)->f2;
if(!_tmp36F){
{const char*_tmp5A3;void*_tmp5A2;(_tmp5A2=0,Cyc_Tcutil_terr(_tmp370,((_tmp5A3="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5A3,sizeof(char),74))),_tag_dyneither(_tmp5A2,sizeof(void*),0)));}
break;}}}
# 1546
;_pop_region(r);}
