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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 244 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
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
# 163
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 364
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
# 50
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
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 945
extern void*Cyc_Absyn_sint_typ;
# 947
void*Cyc_Absyn_float_typ(int);
# 949
extern void*Cyc_Absyn_empty_effect;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1093
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
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
# 67
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
# 124
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 138
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 139
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerceable(void*);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 117
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple11{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
# 217 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 38 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);
# 43
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
# 44
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 45
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
# 46
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
# 53
void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct Cyc_List_List*_tmp5;int _tmp6;struct Cyc_Absyn_AggrInfo*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;struct Cyc_Absyn_AggrInfo*_tmpD;struct Cyc_Absyn_AggrInfo _tmpE;union Cyc_Absyn_AggrInfoU _tmpF;struct _tuple3 _tmp10;enum Cyc_Absyn_AggrKind _tmp11;struct _tuple1*_tmp12;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp14;struct Cyc_List_List**_tmp15;struct Cyc_List_List*_tmp16;int _tmp17;struct Cyc_Absyn_Exp*_tmp19;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;else{_tmp2=_tmp1->f1;}}_LL2:
# 55
{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){
# 56
{struct _RegionHandle _tmp1D=_new_region("r");struct _RegionHandle*r=& _tmp1D;_push_region(r);
# 57
{void*_tmp1E=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2,0);struct Cyc_Absyn_Datatypedecl*_tmp21;struct Cyc_Absyn_Datatypefield*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp24;struct Cyc_Absyn_Enumfield*_tmp25;void*_tmp27;struct Cyc_Absyn_Enumfield*_tmp28;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp1F->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 60
{const char*_tmp3CE;void*_tmp3CD;(_tmp3CD=0,Cyc_Tcutil_terr(p->loc,((_tmp3CE="struct tag used without arguments in pattern",_tag_dyneither(_tmp3CE,sizeof(char),45))),_tag_dyneither(_tmp3CD,sizeof(void*),0)));}
# 61
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3D1;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3D0;p->r=(void*)((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D1.tag=7,((_tmp3D1.f1=_tmp21,((_tmp3D1.f2=_tmp22,((_tmp3D1.f3=0,((_tmp3D1.f4=0,_tmp3D1)))))))))),_tmp3D0))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}_LL13:
# 64
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3D4;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp3D3;p->r=(void*)((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=((_tmp3D4.tag=12,((_tmp3D4.f1=_tmp24,((_tmp3D4.f2=_tmp25,_tmp3D4)))))),_tmp3D3))));}_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp26=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}_LL15:
# 65
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3D7;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp3D6;p->r=(void*)((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=((_tmp3D7.tag=13,((_tmp3D7.f1=(void*)_tmp27,((_tmp3D7.f2=_tmp28,_tmp3D7)))))),_tmp3D6))));}_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp29=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17:
# 66
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp34=(struct Cyc_Dict_Absent_exn_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 69
 goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{
# 71
union Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E: if((_tmp35.Loc_n).tag != 4)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F:
# 72
 goto _LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:
# 74
(*_tmp2).f1=Cyc_Absyn_Loc_n;
# 75
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3DA;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp3D9;p->r=(void*)((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=((_tmp3DA.tag=1,((_tmp3DA.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp3DA.f2=
# 76
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3DA)))))),_tmp3D9))));}
# 77
return;_LL22:;_LL23:
# 78
{const char*_tmp3DD;void*_tmp3DC;(_tmp3DC=0,Cyc_Tcutil_terr(p->loc,((_tmp3DD="qualified variable in pattern",_tag_dyneither(_tmp3DD,sizeof(char),30))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 81
{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){
# 82
{struct _RegionHandle _tmp3F=_new_region("r");struct _RegionHandle*r=& _tmp3F;_push_region(r);
# 83
{void*_tmp40=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);struct Cyc_Absyn_Aggrdecl*_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {
# 85
struct Cyc_List_List*_tmp49=0;
# 86
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
# 87
struct _tuple15*_tmp3E0;struct Cyc_List_List*_tmp3DF;_tmp49=((_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->hd=((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0->f1=0,((_tmp3E0->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp3E0)))))),((_tmp3DF->tl=_tmp49,_tmp3DF))))));}
# 88
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3EA;struct Cyc_Absyn_Aggrdecl**_tmp3E9;struct Cyc_Absyn_AggrInfo*_tmp3E8;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3E7;p->r=(void*)((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7[0]=((_tmp3EA.tag=6,((_tmp3EA.f1=((_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=_tmp42,_tmp3E9))))),((_tmp3E8->targs=0,_tmp3E8)))))),((_tmp3EA.f2=0,((_tmp3EA.f3=
# 89
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp49),((_tmp3EA.f4=_tmp6,_tmp3EA)))))))))),_tmp3E7))));}
# 90
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}_LL28:
# 91
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3ED;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3EC;p->r=(void*)((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3ED.tag=7,((_tmp3ED.f1=_tmp44,((_tmp3ED.f2=_tmp45,((_tmp3ED.f3=_tmp5,((_tmp3ED.f4=_tmp6,_tmp3ED)))))))))),_tmp3EC))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp46=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp46->tag != 3)goto _LL2B;}_LL2A:
# 92
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp47=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp47->tag != 4)goto _LL2D;}_LL2C:
# 94
{const char*_tmp3F0;void*_tmp3EF;(_tmp3EF=0,Cyc_Tcutil_terr(p->loc,((_tmp3F0="enum tag used with arguments in pattern",_tag_dyneither(_tmp3F0,sizeof(char),40))),_tag_dyneither(_tmp3EF,sizeof(void*),0)));}
# 95
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 96
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp48=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}_LL2E:
# 97
 goto _LL24;_LL24:;}
# 83
;_pop_region(r);}
# 82
;_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp56=(struct Cyc_Dict_Absent_exn_struct*)_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 99
 goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp55);_LL2F:;}};}
# 100
{const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_PrintArg_struct _tmp3F2;(_tmp3F2.tag=0,((_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3F3[0]=& _tmp3F2,Cyc_Tcutil_terr(p->loc,((_tmp3F4="%s is not a constructor in pattern",_tag_dyneither(_tmp3F4,sizeof(char),35))),_tag_dyneither(_tmp3F3,sizeof(void*),1)))))));}
# 101
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 102
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=_tmp7->f4;}}_LL6:
# 105
 if(topt == 0){
# 106
const char*_tmp3F7;void*_tmp3F6;(_tmp3F6=0,Cyc_Tcutil_terr(p->loc,((_tmp3F7="cannot determine pattern type",_tag_dyneither(_tmp3F7,sizeof(char),30))),_tag_dyneither(_tmp3F6,sizeof(void*),0)));}{
# 107
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
# 108
{void*_tmp5C=t;struct Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5C;if(_tmp5D->tag != 11)goto _LL37;else{_tmp5E=_tmp5D->f1;}}_LL36:
# 110
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3FD;struct Cyc_Absyn_AggrInfo*_tmp3FC;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3FB;p->r=(void*)((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB[0]=((_tmp3FD.tag=6,((_tmp3FD.f1=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=_tmp5E,_tmp3FC)))),((_tmp3FD.f2=_tmp9,((_tmp3FD.f3=_tmpA,((_tmp3FD.f4=_tmpB,_tmp3FD)))))))))),_tmp3FB))));}
# 111
Cyc_Tcpat_resolve_pat(te,topt,p);
# 112
goto _LL34;_LL37:;_LL38:
# 114
{const char*_tmp401;void*_tmp400[1];struct Cyc_String_pa_PrintArg_struct _tmp3FF;(_tmp3FF.tag=0,((_tmp3FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 115
Cyc_Absynpp_typ2string(t)),((_tmp400[0]=& _tmp3FF,Cyc_Tcutil_terr(p->loc,((_tmp401="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp401,sizeof(char),45))),_tag_dyneither(_tmp400,sizeof(void*),1)))))));}
# 117
goto _LL34;_LL34:;}
# 119
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:
# 122
{struct _handler_cons _tmp65;_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!_tmp67){
# 123
{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmp12);
# 124
struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;
# 125
if(_tmp69->impl == 0){
# 126
{const char*_tmp409;void*_tmp408[1];const char*_tmp407;const char*_tmp406;struct Cyc_String_pa_PrintArg_struct _tmp405;(_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)(
# 127
_tmp69->kind == Cyc_Absyn_StructA?(_tmp406="struct",_tag_dyneither(_tmp406,sizeof(char),7)):((_tmp407="union",_tag_dyneither(_tmp407,sizeof(char),6)))),((_tmp408[0]=& _tmp405,Cyc_Tcutil_terr(p->loc,((_tmp409="can't destructure an abstract %s",_tag_dyneither(_tmp409,sizeof(char),33))),_tag_dyneither(_tmp408,sizeof(void*),1)))))));}
# 128
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 129
_npop_handler(0);return;}{
# 131
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp15);
# 132
if(more_exists < 0){
# 133
{const char*_tmp40C;void*_tmp40B;(_tmp40B=0,Cyc_Tcutil_terr(p->loc,((_tmp40C="too many existentially bound type variables in pattern",_tag_dyneither(_tmp40C,sizeof(char),55))),_tag_dyneither(_tmp40B,sizeof(void*),0)));}{
# 134
struct Cyc_List_List**_tmp71=_tmp15;
# 135
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69->impl))->exist_vars);for(0;n != 0;-- n){
# 136
_tmp71=&((struct Cyc_List_List*)_check_null(*_tmp71))->tl;}}
# 137
*_tmp71=0;};}else{
# 138
if(more_exists > 0){
# 140
struct Cyc_List_List*_tmp72=0;
# 141
for(0;more_exists != 0;-- more_exists){
# 142
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp412;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp411;struct Cyc_List_List*_tmp410;_tmp72=((_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=Cyc_Tcutil_new_tvar((void*)((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=((_tmp411.tag=1,((_tmp411.f1=0,_tmp411)))),_tmp412))))),((_tmp410->tl=_tmp72,_tmp410))))));}
# 143
*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}{
# 145
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp418;struct Cyc_Absyn_AggrInfo*_tmp417;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp416;p->r=(void*)((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416[0]=((_tmp418.tag=6,((_tmp418.f1=((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417->aggr_info=Cyc_Absyn_KnownAggr(_tmp68),((_tmp417->targs=_tmp13,_tmp417)))))),((_tmp418.f2=*_tmp15,((_tmp418.f3=_tmp16,((_tmp418.f4=_tmp17,_tmp418)))))))))),_tmp416))));};};}
# 123
;_pop_handler();}else{void*_tmp66=(void*)_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp7B=(struct Cyc_Dict_Absent_exn_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 148
{const char*_tmp41B;void*_tmp41A;(_tmp41A=0,Cyc_Tcutil_terr(p->loc,((_tmp41B="Non-aggregate name has designator patterns",_tag_dyneither(_tmp41B,sizeof(char),43))),_tag_dyneither(_tmp41A,sizeof(void*),0)));}
# 149
p->r=(void*)& Cyc_Absyn_Wild_p_val;
# 150
goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}
# 152
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp18=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp18->tag != 16)goto _LLB;else{_tmp19=_tmp18->f1;}}_LLA:
# 154
 Cyc_Tcexp_tcExp(te,0,_tmp19);
# 155
if(!Cyc_Tcutil_is_const_exp(_tmp19)){
# 156
{const char*_tmp41E;void*_tmp41D;(_tmp41D=0,Cyc_Tcutil_terr(p->loc,((_tmp41E="non-constant expression in case pattern",_tag_dyneither(_tmp41E,sizeof(char),40))),_tag_dyneither(_tmp41D,sizeof(void*),0)));}
# 157
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 159
unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=Cyc_Evexp_eval_const_uint_exp(_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
# 160
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp421;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp420;p->r=(void*)((_tmp420=_cycalloc_atomic(sizeof(*_tmp420)),((_tmp420[0]=((_tmp421.tag=9,((_tmp421.f1=Cyc_Absyn_None,((_tmp421.f2=(int)_tmp81,_tmp421)))))),_tmp420))));}
# 161
return;};_LLB:;_LLC:
# 162
 return;_LL0:;}
# 166
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
# 167
return(*vd->name).f2;}
# 169
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
# 170
if(topt != 0)
# 171
return*topt;{
# 172
struct Cyc_Core_Opt*_tmp422;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422->v=s,_tmp422)))));};}
# 174
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 178
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
# 179
return*topt;
# 181
{void*_tmp86=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp86;if(_tmp87->tag != 13)goto _LL41;}_LL40:
# 182
 if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42:
# 183
 goto _LL3E;_LL3E:;}
# 185
return numt;}
# 188
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,void*t){
# 189
vd->type=t;
# 190
vd->tq=Cyc_Absyn_empty_tqual(0);{
# 191
struct Cyc_List_List*_tmp423;*v_result_ptr=(struct Cyc_List_List*)((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->hd=vd,((_tmp423->tl=*v_result_ptr,_tmp423))))));};}
# 193
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 195
struct _tuple13*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;{
# 196
struct _tuple13*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;
# 197
if(_tmp8A != 0  || _tmp8D != 0){
# 198
if(_tmp8A == 0){
# 199
struct _tuple13*_tmp424;_tmp8A=((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424->f1=0,((_tmp424->f2=0,_tmp424))))));}
# 200
if(_tmp8D == 0){
# 201
struct _tuple13*_tmp425;_tmp8D=((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->f1=0,((_tmp425->f2=0,_tmp425))))));}{
# 202
struct _tuple13*_tmp428;struct Cyc_Tcpat_TcPatResult _tmp427;return(_tmp427.tvars_and_bounds_opt=((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f1,(*_tmp8D).f1),((_tmp428->f2=
# 203
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8A).f2,(*_tmp8D).f2),_tmp428)))))),((_tmp427.patvars=
# 204
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp427)));};}{
# 206
struct Cyc_Tcpat_TcPatResult _tmp429;return(_tmp429.tvars_and_bounds_opt=0,((_tmp429.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E),_tmp429)));};};}
# 209
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
# 210
struct Cyc_Absyn_Pat*_tmp42A;return(_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A->loc=loc,((_tmp42A->topt=0,((_tmp42A->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp42A)))))));}
# 213
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
# 214
void*_tmp95=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp97;void*_tmp98;struct Cyc_Absyn_Tqual _tmp99;struct Cyc_Absyn_Exp*_tmp9A;union Cyc_Absyn_Constraint*_tmp9B;unsigned int _tmp9C;_LL44: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp95;if(_tmp96->tag != 8)goto _LL46;else{_tmp97=_tmp96->f1;_tmp98=_tmp97.elt_type;_tmp99=_tmp97.tq;_tmp9A=_tmp97.num_elts;_tmp9B=_tmp97.zero_term;_tmp9C=_tmp97.zt_loc;}}_LL45: {
# 216
struct Cyc_Core_Opt*_tmp42B;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,((_tmp42B=_cycalloc(sizeof(*_tmp42B)),((_tmp42B->v=Cyc_Tcenv_lookup_type_vars(te),_tmp42B))))):(void*)rgn_opt;
# 217
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp42E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42D;return Cyc_Absyn_atb_typ(_tmp98,rgn,_tmp99,(void*)((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp42E.tag=1,((_tmp42E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp9A),_tmp42E)))),_tmp42D)))),_tmp9B);}_LL46:;_LL47:
# 218
 return t;_LL43:;}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 222
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat){
# 224
Cyc_Tcpat_resolve_pat(te,topt,p);{
# 225
void*t;
# 226
struct Cyc_Tcpat_TcPatResult _tmp42F;struct Cyc_Tcpat_TcPatResult res=(_tmp42F.tvars_and_bounds_opt=0,((_tmp42F.patvars=0,_tmp42F)));
# 229
{void*_tmpA0=p->r;struct Cyc_Absyn_Vardecl*_tmpA3;struct Cyc_Absyn_Pat*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA6;struct Cyc_Absyn_Pat*_tmpA7;struct Cyc_Absyn_Tvar*_tmpA9;struct Cyc_Absyn_Vardecl*_tmpAA;enum Cyc_Absyn_Sign _tmpAC;enum Cyc_Absyn_Sign _tmpAE;enum Cyc_Absyn_Sign _tmpB0;int _tmpB3;struct Cyc_Absyn_Enumdecl*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Pat*_tmpBA;struct Cyc_List_List*_tmpBC;struct Cyc_List_List**_tmpBD;int _tmpBE;struct Cyc_Absyn_AggrInfo*_tmpC0;struct Cyc_Absyn_AggrInfo _tmpC1;union Cyc_Absyn_AggrInfoU _tmpC2;struct Cyc_Absyn_Aggrdecl**_tmpC3;struct Cyc_Absyn_Aggrdecl*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_List_List**_tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC8;struct Cyc_List_List**_tmpC9;int _tmpCA;struct Cyc_Absyn_Datatypedecl*_tmpCC;struct Cyc_Absyn_Datatypefield*_tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_List_List**_tmpCF;int _tmpD0;struct Cyc_Absyn_AggrInfo*_tmpD2;struct Cyc_Absyn_AggrInfo*_tmpD4;struct Cyc_Absyn_AggrInfo _tmpD5;union Cyc_Absyn_AggrInfoU _tmpD6;struct _tuple3 _tmpD7;_LL49: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmpA1=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpA1->tag != 0)goto _LL4B;}_LL4A:
# 232
 if(topt != 0)
# 233
t=*topt;else{
# 235
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
# 236
goto _LL48;_LL4B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpA2=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpA2->tag != 1)goto _LL4D;else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL4C:
# 238
 res=Cyc_Tcpat_tcPatRec(te,_tmpA4,topt,rgn_pat,allow_ref_pat);
# 239
t=(void*)_check_null(_tmpA4->topt);
# 242
{void*_tmpDB=Cyc_Tcutil_compress(t);_LL74: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDB;if(_tmpDC->tag != 8)goto _LL76;}_LL75:
# 244
 if(rgn_pat == 0  || !allow_ref_pat){
# 245
const char*_tmp432;void*_tmp431;(_tmp431=0,Cyc_Tcutil_terr(p->loc,((_tmp432="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp432,sizeof(char),58))),_tag_dyneither(_tmp431,sizeof(void*),0)));}
# 246
goto _LL73;_LL76:;_LL77:
# 248
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
# 249
const char*_tmp435;void*_tmp434;(_tmp434=0,Cyc_Tcutil_terr(p->loc,((_tmp435="pattern would point to an abstract member",_tag_dyneither(_tmp435,sizeof(char),42))),_tag_dyneither(_tmp434,sizeof(void*),0)));}
# 250
goto _LL73;_LL73:;}
# 252
Cyc_Tcpat_set_vd(_tmpA3,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));
# 253
goto _LL48;_LL4D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA5=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpA5->tag != 2)goto _LL4F;else{_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;}}_LL4E:
# 255
 res=Cyc_Tcpat_tcPatRec(te,_tmpA7,topt,rgn_pat,allow_ref_pat);
# 256
t=(void*)_check_null(_tmpA7->topt);
# 257
if(!allow_ref_pat  || rgn_pat == 0){
# 258
{const char*_tmp438;void*_tmp437;(_tmp437=0,Cyc_Tcutil_terr(p->loc,((_tmp438="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp438,sizeof(char),55))),_tag_dyneither(_tmp437,sizeof(void*),0)));}
# 259
goto _LL48;}else{
# 261
if(Cyc_Tcutil_is_noalias_pointer(t)){
# 262
const char*_tmp43B;void*_tmp43A;(_tmp43A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp43B="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp43B,sizeof(char),56))),_tag_dyneither(_tmp43A,sizeof(void*),0)));}}
# 263
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp445;struct Cyc_Absyn_PtrAtts _tmp444;struct Cyc_Absyn_PtrInfo _tmp443;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp442;Cyc_Tcpat_set_vd(_tmpA6,& res.patvars,(void*)(
# 264
(_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((_tmp445.tag=5,((_tmp445.f1=((_tmp443.elt_typ=t,((_tmp443.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp443.ptr_atts=(
# 265
(_tmp444.rgn=(void*)rgn_pat,((_tmp444.nullable=Cyc_Absyn_false_conref,((_tmp444.bounds=
# 266
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp444.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp444.ptrloc=0,_tmp444)))))))))),_tmp443)))))),_tmp445)))),_tmp442)))));}
# 268
goto _LL48;_LL4F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpA8->tag != 3)goto _LL51;else{_tmpA9=_tmpA8->f1;_tmpAA=_tmpA8->f2;}}_LL50:
# 273
 Cyc_Tcpat_set_vd(_tmpAA,& res.patvars,_tmpAA->type);{
# 277
struct _RegionHandle _tmpE9=_new_region("r2");struct _RegionHandle*r2=& _tmpE9;_push_region(r2);{
# 278
struct Cyc_Absyn_Tvar*_tmp446[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp446[0]=_tmpA9,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp446,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 280
if(res.tvars_and_bounds_opt == 0){
# 281
struct _tuple13*_tmp447;res.tvars_and_bounds_opt=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->f1=0,((_tmp447->f2=0,_tmp447))))));}
# 282
{struct Cyc_List_List*_tmp448;(*res.tvars_and_bounds_opt).f1=(
# 283
(_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->hd=_tmpA9,((_tmp448->tl=(*res.tvars_and_bounds_opt).f1,_tmp448))))));}
# 284
t=Cyc_Absyn_uint_typ;
# 285
_npop_handler(0);goto _LL48;
# 277
;_pop_region(r2);};_LL51: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpAB->tag != 9)goto _LL53;else{_tmpAC=_tmpAB->f1;if(_tmpAC != Cyc_Absyn_Unsigned)goto _LL53;}}_LL52:
# 287
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL48;_LL53: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpAD->tag != 9)goto _LL55;else{_tmpAE=_tmpAD->f1;if(_tmpAE != Cyc_Absyn_None)goto _LL55;}}_LL54:
# 288
 goto _LL56;_LL55: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpAF->tag != 9)goto _LL57;else{_tmpB0=_tmpAF->f1;if(_tmpB0 != Cyc_Absyn_Signed)goto _LL57;}}_LL56:
# 289
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL48;_LL57: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB1->tag != 10)goto _LL59;}_LL58:
# 290
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL48;_LL59: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpB2=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB2->tag != 11)goto _LL5B;else{_tmpB3=_tmpB2->f2;}}_LL5A:
# 291
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpB3));goto _LL48;_LL5B: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB4->tag != 12)goto _LL5D;else{_tmpB5=_tmpB4->f1;}}_LL5C:
# 292
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp44B;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp44A;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=((_tmp44B.tag=13,((_tmp44B.f1=_tmpB5->name,((_tmp44B.f2=(struct Cyc_Absyn_Enumdecl*)_tmpB5,_tmp44B)))))),_tmp44A)))));}goto _LL48;_LL5D: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpB6=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB6->tag != 13)goto _LL5F;else{_tmpB7=(void*)_tmpB6->f1;}}_LL5E:
# 293
 t=Cyc_Tcpat_num_type(topt,_tmpB7);goto _LL48;_LL5F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB8->tag != 8)goto _LL61;}_LL60:
# 295
 if(topt != 0){
# 296
void*_tmpEF=Cyc_Tcutil_compress(*topt);_LL79: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpEF;if(_tmpF0->tag != 5)goto _LL7B;}_LL7A:
# 297
 t=*topt;goto tcpat_end;_LL7B:;_LL7C:
# 298
 goto _LL78;_LL78:;}{
# 300
struct Cyc_Core_Opt*_tmpF1=Cyc_Tcenv_lookup_opt_type_vars(te);
# 301
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp455;struct Cyc_Absyn_PtrAtts _tmp454;struct Cyc_Absyn_PtrInfo _tmp453;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp452;t=(void*)((_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452[0]=((_tmp455.tag=5,((_tmp455.f1=((_tmp453.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ako,_tmpF1),((_tmp453.elt_tq=
# 302
Cyc_Absyn_empty_tqual(0),((_tmp453.ptr_atts=(
# 305
(_tmp454.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,_tmpF1),((_tmp454.nullable=Cyc_Absyn_true_conref,((_tmp454.bounds=
# 307
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp454.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp454.ptrloc=0,_tmp454)))))))))),_tmp453)))))),_tmp455)))),_tmp452))));}
# 310
goto _LL48;};_LL61: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpB9->tag != 5)goto _LL63;else{_tmpBA=_tmpB9->f1;}}_LL62: {
# 316
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
# 317
void**_tmpF6=0;
# 318
int elt_const=0;
# 319
if(topt != 0){
# 320
void*_tmpF7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmpF9;void*_tmpFA;struct Cyc_Absyn_Tqual _tmpFB;_LL7E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF7;if(_tmpF8->tag != 5)goto _LL80;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF9.elt_typ;_tmpFB=_tmpF9.elt_tq;}}_LL7F:
# 322
 inner_typ=_tmpFA;
# 323
_tmpF6=(void**)& inner_typ;
# 324
elt_const=_tmpFB.real_const;
# 325
goto _LL7D;_LL80:;_LL81:
# 326
 goto _LL7D;_LL7D:;}{
# 331
void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
# 332
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpBA,_tmpF6,(void*)ptr_rgn,1));
# 337
{void*_tmpFC=Cyc_Tcutil_compress((void*)_check_null(_tmpBA->topt));struct Cyc_Absyn_DatatypeFieldInfo _tmpFE;union Cyc_Absyn_DatatypeFieldInfoU _tmpFF;struct _tuple2 _tmp100;struct Cyc_Absyn_Datatypedecl*_tmp101;struct Cyc_Absyn_Datatypefield*_tmp102;struct Cyc_List_List*_tmp103;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpFD=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpFC;if(_tmpFD->tag != 4)goto _LL85;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFE.field_info;if((_tmpFF.KnownDatatypefield).tag != 2)goto _LL85;_tmp100=(struct _tuple2)(_tmpFF.KnownDatatypefield).val;_tmp101=_tmp100.f1;_tmp102=_tmp100.f2;_tmp103=_tmpFE.targs;}}_LL84:
# 341
{void*_tmp104=Cyc_Tcutil_compress(inner_typ);_LL88: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp105=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp104;if(_tmp105->tag != 4)goto _LL8A;}_LL89:
# 343
 goto DONT_PROMOTE;_LL8A:;_LL8B:
# 344
 goto _LL87;_LL87:;}{
# 347
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp45F;struct Cyc_Absyn_Datatypedecl**_tmp45E;struct Cyc_Absyn_DatatypeInfo _tmp45D;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp45C;void*new_type=(void*)((_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C[0]=((_tmp45F.tag=3,((_tmp45F.f1=((_tmp45D.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp45E=_cycalloc(sizeof(*_tmp45E)),((_tmp45E[0]=_tmp101,_tmp45E))))),((_tmp45D.targs=_tmp103,_tmp45D)))),_tmp45F)))),_tmp45C))));
# 350
_tmpBA->topt=(void*)new_type;
# 351
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp469;struct Cyc_Absyn_PtrAtts _tmp468;struct Cyc_Absyn_PtrInfo _tmp467;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp466;t=(void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((_tmp469.tag=5,((_tmp469.f1=((_tmp467.elt_typ=new_type,((_tmp467.elt_tq=
# 352
elt_const?Cyc_Absyn_const_tqual(0):
# 353
 Cyc_Absyn_empty_tqual(0),((_tmp467.ptr_atts=(
# 354
(_tmp468.rgn=ptr_rgn,((_tmp468.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp468.bounds=Cyc_Absyn_bounds_one_conref,((_tmp468.zero_term=Cyc_Absyn_false_conref,((_tmp468.ptrloc=0,_tmp468)))))))))),_tmp467)))))),_tmp469)))),_tmp466))));}
# 357
goto _LL82;};_LL85:;_LL86:
# 359
 DONT_PROMOTE: {
# 360
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp473;struct Cyc_Absyn_PtrAtts _tmp472;struct Cyc_Absyn_PtrInfo _tmp471;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp470;t=(void*)((_tmp470=_cycalloc(sizeof(*_tmp470)),((_tmp470[0]=((_tmp473.tag=5,((_tmp473.f1=((_tmp471.elt_typ=(void*)_check_null(_tmpBA->topt),((_tmp471.elt_tq=
# 361
elt_const?Cyc_Absyn_const_tqual(0):
# 362
 Cyc_Absyn_empty_tqual(0),((_tmp471.ptr_atts=(
# 363
(_tmp472.rgn=ptr_rgn,((_tmp472.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp472.bounds=
# 364
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp472.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp472.ptrloc=0,_tmp472)))))))))),_tmp471)))))),_tmp473)))),_tmp470))));}_LL82:;}
# 367
goto _LL48;};}_LL63: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpBB=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpBB->tag != 4)goto _LL65;else{_tmpBC=_tmpBB->f1;_tmpBD=(struct Cyc_List_List**)& _tmpBB->f1;_tmpBE=_tmpBB->f2;}}_LL64: {
# 370
struct Cyc_List_List*_tmp112=*_tmpBD;
# 371
struct Cyc_List_List*pat_ts=0;
# 372
struct Cyc_List_List*topt_ts=0;
# 373
if(topt != 0){
# 374
void*_tmp113=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp115;_LL8D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp114=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp113;if(_tmp114->tag != 10)goto _LL8F;else{_tmp115=_tmp114->f1;}}_LL8E:
# 376
 topt_ts=_tmp115;
# 377
if(_tmpBE){
# 379
int _tmp116=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp112);
# 380
int _tmp117=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp115);
# 381
if(_tmp116 < _tmp117){
# 382
struct Cyc_List_List*wild_ps=0;
# 383
{int i=0;for(0;i < _tmp117 - _tmp116;++ i){
# 384
struct Cyc_List_List*_tmp474;wild_ps=((_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp474->tl=wild_ps,_tmp474))))));}}
# 385
*_tmpBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp112,wild_ps);
# 386
_tmp112=*_tmpBD;}else{
# 387
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp112)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp115)){
# 388
const char*_tmp477;void*_tmp476;(_tmp476=0,Cyc_Tcutil_warn(p->loc,((_tmp477="unnecessary ... in tuple pattern",_tag_dyneither(_tmp477,sizeof(char),33))),_tag_dyneither(_tmp476,sizeof(void*),0)));}}}
# 391
goto _LL8C;_LL8F:;_LL90:
# 393
 goto _LL8C;_LL8C:;}else{
# 395
if(_tmpBE){
# 396
const char*_tmp47A;void*_tmp479;(_tmp479=0,Cyc_Tcutil_terr(p->loc,((_tmp47A="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp47A,sizeof(char),57))),_tag_dyneither(_tmp479,sizeof(void*),0)));}}
# 397
for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
# 398
void**_tmp11D=0;
# 399
if(topt_ts != 0){
# 400
_tmp11D=(void**)&(*((struct _tuple16*)topt_ts->hd)).f2;
# 401
topt_ts=topt_ts->tl;}
# 403
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp112->hd,_tmp11D,rgn_pat,allow_ref_pat));{
# 404
struct _tuple16*_tmp47D;struct Cyc_List_List*_tmp47C;pat_ts=((_tmp47C=_cycalloc(sizeof(*_tmp47C)),((_tmp47C->hd=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->f1=Cyc_Absyn_empty_tqual(0),((_tmp47D->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp112->hd)->topt),_tmp47D)))))),((_tmp47C->tl=pat_ts,_tmp47C))))));};}
# 406
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp480;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp47F;t=(void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp480.tag=10,((_tmp480.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp480)))),_tmp47F))));}
# 407
goto _LL48;}_LL65: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpBF=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpBF->tag != 6)goto _LL67;else{_tmpC0=_tmpBF->f1;if(_tmpC0 == 0)goto _LL67;_tmpC1=*_tmpC0;_tmpC2=_tmpC1.aggr_info;if((_tmpC2.KnownAggr).tag != 2)goto _LL67;_tmpC3=(struct Cyc_Absyn_Aggrdecl**)(_tmpC2.KnownAggr).val;_tmpC4=*_tmpC3;_tmpC5=_tmpC1.targs;_tmpC6=(struct Cyc_List_List**)&(*_tmpBF->f1).targs;_tmpC7=_tmpBF->f2;_tmpC8=_tmpBF->f3;_tmpC9=(struct Cyc_List_List**)& _tmpBF->f3;_tmpCA=_tmpBF->f4;}}_LL66: {
# 410
struct Cyc_List_List*_tmp122=*_tmpC9;
# 411
const char*_tmp482;const char*_tmp481;struct _dyneither_ptr aggr_str=_tmpC4->kind == Cyc_Absyn_StructA?(_tmp482="struct",_tag_dyneither(_tmp482,sizeof(char),7)):((_tmp481="union",_tag_dyneither(_tmp481,sizeof(char),6)));
# 412
if(_tmpC4->impl == 0){
# 413
{const char*_tmp486;void*_tmp485[1];struct Cyc_String_pa_PrintArg_struct _tmp484;(_tmp484.tag=0,((_tmp484.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp485[0]=& _tmp484,Cyc_Tcutil_terr(p->loc,((_tmp486="can't destructure an abstract %s",_tag_dyneither(_tmp486,sizeof(char),33))),_tag_dyneither(_tmp485,sizeof(void*),1)))))));}
# 414
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
# 415
goto _LL48;}
# 418
if(_tmpC4->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->tagged)
# 419
allow_ref_pat=0;
# 420
if(_tmpC7 != 0){
# 422
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
# 423
allow_ref_pat=0;}{
# 425
struct _RegionHandle _tmp126=_new_region("rgn");struct _RegionHandle*rgn=& _tmp126;_push_region(rgn);{
# 427
struct Cyc_List_List*_tmp127=0;
# 428
struct Cyc_List_List*outlives_constraints=0;
# 429
struct Cyc_List_List*_tmp128=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->exist_vars;
# 430
{struct Cyc_List_List*t=_tmpC7;for(0;t != 0;t=t->tl){
# 431
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
# 432
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp128))->hd;
# 433
_tmp128=_tmp128->tl;{
# 434
void*_tmp129=Cyc_Absyn_compress_kb(tv->kind);
# 435
void*_tmp12A=Cyc_Absyn_compress_kb(uv->kind);
# 436
int error=0;
# 437
struct Cyc_Absyn_Kind*k2;
# 438
{void*_tmp12B=_tmp12A;struct Cyc_Absyn_Kind*_tmp12D;struct Cyc_Absyn_Kind*_tmp12F;_LL92: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12B;if(_tmp12C->tag != 2)goto _LL94;else{_tmp12D=_tmp12C->f2;}}_LL93:
# 439
 _tmp12F=_tmp12D;goto _LL95;_LL94: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp12E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp12B;if(_tmp12E->tag != 0)goto _LL96;else{_tmp12F=_tmp12E->f1;}}_LL95:
# 440
 k2=_tmp12F;goto _LL91;_LL96:;_LL97: {
# 441
const char*_tmp489;void*_tmp488;(_tmp488=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp489="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp489,sizeof(char),53))),_tag_dyneither(_tmp488,sizeof(void*),0)));}_LL91:;}
# 443
{void*_tmp132=_tmp129;struct Cyc_Absyn_Kind*_tmp134;struct Cyc_Core_Opt*_tmp136;struct Cyc_Core_Opt**_tmp137;struct Cyc_Absyn_Kind*_tmp138;struct Cyc_Core_Opt*_tmp13A;struct Cyc_Core_Opt**_tmp13B;_LL99: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp133=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp133->tag != 0)goto _LL9B;else{_tmp134=_tmp133->f1;}}_LL9A:
# 446
 if(!Cyc_Tcutil_kind_leq(k2,_tmp134))
# 447
error=1;
# 448
goto _LL98;_LL9B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp135=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp135->tag != 2)goto _LL9D;else{_tmp136=_tmp135->f1;_tmp137=(struct Cyc_Core_Opt**)& _tmp135->f1;_tmp138=_tmp135->f2;}}_LL9C:
# 449
 _tmp13B=_tmp137;goto _LL9E;_LL9D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp139=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp139->tag != 1)goto _LL98;else{_tmp13A=_tmp139->f1;_tmp13B=(struct Cyc_Core_Opt**)& _tmp139->f1;}}_LL9E:
# 450
{struct Cyc_Core_Opt*_tmp48A;*_tmp13B=((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A->v=_tmp12A,_tmp48A))));}goto _LL98;_LL98:;}
# 452
if(error){
# 453
const char*_tmp490;void*_tmp48F[3];struct Cyc_String_pa_PrintArg_struct _tmp48E;struct Cyc_String_pa_PrintArg_struct _tmp48D;struct Cyc_String_pa_PrintArg_struct _tmp48C;(_tmp48C.tag=0,((_tmp48C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 456
Cyc_Absynpp_kind2string(k2)),((_tmp48D.tag=0,((_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp129)),((_tmp48E.tag=0,((_tmp48E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp48F[0]=& _tmp48E,((_tmp48F[1]=& _tmp48D,((_tmp48F[2]=& _tmp48C,Cyc_Tcutil_terr(p->loc,((_tmp490="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp490,sizeof(char),60))),_tag_dyneither(_tmp48F,sizeof(void*),3)))))))))))))))))));}{
# 458
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp493;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp492;void*vartype=(void*)((_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492[0]=((_tmp493.tag=2,((_tmp493.f1=tv,_tmp493)))),_tmp492))));
# 459
{struct Cyc_List_List*_tmp494;_tmp127=((_tmp494=_region_malloc(rgn,sizeof(*_tmp494)),((_tmp494->hd=(void*)vartype,((_tmp494->tl=_tmp127,_tmp494))))));}
# 462
if(k2->kind == Cyc_Absyn_RgnKind){
# 463
if(k2->aliasqual == Cyc_Absyn_Aliasable){
# 464
struct _tuple0*_tmp497;struct Cyc_List_List*_tmp496;outlives_constraints=(
# 465
(_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->hd=((_tmp497=_cycalloc(sizeof(*_tmp497)),((_tmp497->f1=Cyc_Absyn_empty_effect,((_tmp497->f2=vartype,_tmp497)))))),((_tmp496->tl=outlives_constraints,_tmp496))))));}else{
# 467
const char*_tmp49A;void*_tmp499;(_tmp499=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp49A="opened existential had unique or top region kind",_tag_dyneither(_tmp49A,sizeof(char),49))),_tag_dyneither(_tmp499,sizeof(void*),0)));}}};};}}
# 471
_tmp127=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp127);{
# 473
struct _RegionHandle _tmp149=_new_region("r2");struct _RegionHandle*r2=& _tmp149;_push_region(r2);
# 474
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpC7);
# 476
struct Cyc_List_List*_tmp14A=Cyc_Tcenv_lookup_type_vars(te2);
# 477
struct _tuple11 _tmp49B;struct _tuple11 _tmp14B=(_tmp49B.f1=_tmp14A,((_tmp49B.f2=rgn,_tmp49B)));
# 478
struct Cyc_List_List*_tmp14C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp14B,_tmpC4->tvs);
# 479
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->exist_vars,_tmp127);
# 480
struct Cyc_List_List*_tmp14E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp14C);
# 481
struct Cyc_List_List*_tmp14F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp14D);
# 482
struct Cyc_List_List*_tmp150=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp14C,_tmp14D);
# 484
if(_tmpC7 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->rgn_po != 0){
# 485
if(res.tvars_and_bounds_opt == 0){
# 486
struct _tuple13*_tmp49C;res.tvars_and_bounds_opt=((_tmp49C=_cycalloc(sizeof(*_tmp49C)),((_tmp49C->f1=0,((_tmp49C->f2=0,_tmp49C))))));}
# 487
(*res.tvars_and_bounds_opt).f1=
# 488
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpC7);
# 489
(*res.tvars_and_bounds_opt).f2=
# 490
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
# 491
struct Cyc_List_List*_tmp152=0;
# 492
{struct Cyc_List_List*_tmp153=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->rgn_po;for(0;_tmp153 != 0;_tmp153=_tmp153->tl){
# 493
struct _tuple0*_tmp49F;struct Cyc_List_List*_tmp49E;_tmp152=((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E->hd=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp150,(*((struct _tuple0*)_tmp153->hd)).f1),((_tmp49F->f2=
# 494
Cyc_Tcutil_rsubstitute(rgn,_tmp150,(*((struct _tuple0*)_tmp153->hd)).f2),_tmp49F)))))),((_tmp49E->tl=_tmp152,_tmp49E))))));}}
# 496
_tmp152=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp152);
# 497
(*res.tvars_and_bounds_opt).f2=
# 498
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp152);};}
# 501
*_tmpC6=_tmp14E;
# 502
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp4A9;struct Cyc_Absyn_Aggrdecl**_tmp4A8;struct Cyc_Absyn_AggrInfo _tmp4A7;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A6;t=(void*)((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A9.tag=11,((_tmp4A9.f1=((_tmp4A7.aggr_info=Cyc_Absyn_KnownAggr(((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8[0]=_tmpC4,_tmp4A8))))),((_tmp4A7.targs=*_tmpC6,_tmp4A7)))),_tmp4A9)))),_tmp4A6))));}
# 503
if(_tmpCA  && _tmpC4->kind == Cyc_Absyn_UnionA){
# 504
const char*_tmp4AC;void*_tmp4AB;(_tmp4AB=0,Cyc_Tcutil_warn(p->loc,((_tmp4AC="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp4AC,sizeof(char),43))),_tag_dyneither(_tmp4AB,sizeof(void*),0)));}else{
# 505
if(_tmpCA){
# 507
int _tmp15C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp122);
# 508
int _tmp15D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->fields);
# 509
if(_tmp15C < _tmp15D){
# 510
struct Cyc_List_List*wild_dps=0;
# 511
{int i=0;for(0;i < _tmp15D - _tmp15C;++ i){
# 512
struct _tuple15*_tmp4AF;struct Cyc_List_List*_tmp4AE;wild_dps=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->hd=((_tmp4AF=_cycalloc(sizeof(*_tmp4AF)),((_tmp4AF->f1=0,((_tmp4AF->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp4AF)))))),((_tmp4AE->tl=wild_dps,_tmp4AE))))));}}
# 513
*_tmpC9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp122,wild_dps);
# 514
_tmp122=*_tmpC9;}else{
# 515
if(_tmp15C == _tmp15D){
# 516
const char*_tmp4B2;void*_tmp4B1;(_tmp4B1=0,Cyc_Tcutil_warn(p->loc,((_tmp4B2="unnecessary ... in struct pattern",_tag_dyneither(_tmp4B2,sizeof(char),34))),_tag_dyneither(_tmp4B1,sizeof(void*),0)));}}}}{
# 518
struct Cyc_List_List*fields=
# 519
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp122,_tmpC4->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC4->impl))->fields);
# 520
for(0;fields != 0;fields=fields->tl){
# 521
struct _tuple17 _tmp163;struct Cyc_Absyn_Aggrfield*_tmp164;struct Cyc_Absyn_Pat*_tmp165;struct _tuple17*_tmp162=(struct _tuple17*)fields->hd;_tmp163=*_tmp162;_tmp164=_tmp163.f1;_tmp165=_tmp163.f2;{
# 522
void*_tmp166=Cyc_Tcutil_rsubstitute(rgn,_tmp150,_tmp164->type);
# 524
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp165,(void**)& _tmp166,rgn_pat,allow_ref_pat));
# 528
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp165->topt),_tmp166)){
# 529
const char*_tmp4B9;void*_tmp4B8[4];struct Cyc_String_pa_PrintArg_struct _tmp4B7;struct Cyc_String_pa_PrintArg_struct _tmp4B6;struct Cyc_String_pa_PrintArg_struct _tmp4B5;struct Cyc_String_pa_PrintArg_struct _tmp4B4;(_tmp4B4.tag=0,((_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 531
Cyc_Absynpp_typ2string((void*)_check_null(_tmp165->topt))),((_tmp4B5.tag=0,((_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 530
Cyc_Absynpp_typ2string(_tmp166)),((_tmp4B6.tag=0,((_tmp4B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4B7.tag=0,((_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp164->name),((_tmp4B8[0]=& _tmp4B7,((_tmp4B8[1]=& _tmp4B6,((_tmp4B8[2]=& _tmp4B5,((_tmp4B8[3]=& _tmp4B4,Cyc_Tcutil_terr(p->loc,((_tmp4B9="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp4B9,sizeof(char),45))),_tag_dyneither(_tmp4B8,sizeof(void*),4)))))))))))))))))))))))));}};}};}
# 474
;_pop_region(r2);};}
# 535
_npop_handler(0);goto _LL48;
# 425
;_pop_region(rgn);};}_LL67: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpCB=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpCB->tag != 7)goto _LL69;else{_tmpCC=_tmpCB->f1;_tmpCD=_tmpCB->f2;_tmpCE=_tmpCB->f3;_tmpCF=(struct Cyc_List_List**)& _tmpCB->f3;_tmpD0=_tmpCB->f4;}}_LL68: {
# 538
struct Cyc_List_List*_tmp170=*_tmpCF;
# 539
struct _RegionHandle _tmp171=_new_region("rgn");struct _RegionHandle*rgn=& _tmp171;_push_region(rgn);{
# 540
struct Cyc_List_List*tqts=_tmpCD->typs;
# 542
struct Cyc_List_List*_tmp172=Cyc_Tcenv_lookup_type_vars(te);
# 543
struct _tuple11 _tmp4BA;struct _tuple11 _tmp173=(_tmp4BA.f1=_tmp172,((_tmp4BA.f2=rgn,_tmp4BA)));
# 544
struct Cyc_List_List*_tmp174=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp173,_tmpCC->tvs);
# 545
struct Cyc_List_List*_tmp175=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp174);
# 546
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4C0;struct Cyc_Absyn_DatatypeFieldInfo _tmp4BF;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4BE;t=(void*)((_tmp4BE=_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE[0]=((_tmp4C0.tag=4,((_tmp4C0.f1=((_tmp4BF.field_info=Cyc_Absyn_KnownDatatypefield(_tmpCC,_tmpCD),((_tmp4BF.targs=_tmp175,_tmp4BF)))),_tmp4C0)))),_tmp4BE))));}
# 548
if(_tmpD0){
# 550
int _tmp179=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp170);
# 551
int _tmp17A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
# 552
if(_tmp179 < _tmp17A){
# 553
struct Cyc_List_List*wild_ps=0;
# 554
{int i=0;for(0;i < _tmp17A - _tmp179;++ i){
# 555
struct Cyc_List_List*_tmp4C1;wild_ps=((_tmp4C1=_cycalloc(sizeof(*_tmp4C1)),((_tmp4C1->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4C1->tl=wild_ps,_tmp4C1))))));}}
# 556
*_tmpCF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp170,wild_ps);
# 557
_tmp170=*_tmpCF;}else{
# 558
if(_tmp179 == _tmp17A){
# 559
const char*_tmp4C5;void*_tmp4C4[1];struct Cyc_String_pa_PrintArg_struct _tmp4C3;(_tmp4C3.tag=0,((_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 560
Cyc_Absynpp_qvar2string(_tmpCC->name)),((_tmp4C4[0]=& _tmp4C3,Cyc_Tcutil_warn(p->loc,((_tmp4C5="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4C5,sizeof(char),37))),_tag_dyneither(_tmp4C4,sizeof(void*),1)))))));}}}
# 562
for(0;_tmp170 != 0  && tqts != 0;(_tmp170=_tmp170->tl,tqts=tqts->tl)){
# 563
struct Cyc_Absyn_Pat*_tmp17F=(struct Cyc_Absyn_Pat*)_tmp170->hd;
# 566
void*_tmp180=Cyc_Tcutil_rsubstitute(rgn,_tmp174,(*((struct _tuple16*)tqts->hd)).f2);
# 567
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp17F,(void**)& _tmp180,rgn_pat,allow_ref_pat));
# 571
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp17F->topt),_tmp180)){
# 572
const char*_tmp4CB;void*_tmp4CA[3];struct Cyc_String_pa_PrintArg_struct _tmp4C9;struct Cyc_String_pa_PrintArg_struct _tmp4C8;struct Cyc_String_pa_PrintArg_struct _tmp4C7;(_tmp4C7.tag=0,((_tmp4C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 574
Cyc_Absynpp_typ2string((void*)_check_null(_tmp17F->topt))),((_tmp4C8.tag=0,((_tmp4C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 573
Cyc_Absynpp_typ2string(_tmp180)),((_tmp4C9.tag=0,((_tmp4C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpCD->name)),((_tmp4CA[0]=& _tmp4C9,((_tmp4CA[1]=& _tmp4C8,((_tmp4CA[2]=& _tmp4C7,Cyc_Tcutil_terr(_tmp17F->loc,((_tmp4CB="%s expects argument type %s, not %s",_tag_dyneither(_tmp4CB,sizeof(char),36))),_tag_dyneither(_tmp4CA,sizeof(void*),3)))))))))))))))))));}}
# 576
if(_tmp170 != 0){
# 577
const char*_tmp4CF;void*_tmp4CE[1];struct Cyc_String_pa_PrintArg_struct _tmp4CD;(_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 578
Cyc_Absynpp_qvar2string(_tmpCD->name)),((_tmp4CE[0]=& _tmp4CD,Cyc_Tcutil_terr(p->loc,((_tmp4CF="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4CF,sizeof(char),47))),_tag_dyneither(_tmp4CE,sizeof(void*),1)))))));}
# 579
if(tqts != 0){
# 580
const char*_tmp4D3;void*_tmp4D2[1];struct Cyc_String_pa_PrintArg_struct _tmp4D1;(_tmp4D1.tag=0,((_tmp4D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 581
Cyc_Absynpp_qvar2string(_tmpCD->name)),((_tmp4D2[0]=& _tmp4D1,Cyc_Tcutil_terr(p->loc,((_tmp4D3="too few arguments for datatype constructor %s",_tag_dyneither(_tmp4D3,sizeof(char),46))),_tag_dyneither(_tmp4D2,sizeof(void*),1)))))));}}
# 583
_npop_handler(0);goto _LL48;
# 539
;_pop_region(rgn);}_LL69: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpD1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpD1->tag != 6)goto _LL6B;else{_tmpD2=_tmpD1->f1;if(_tmpD2 != 0)goto _LL6B;}}_LL6A:
# 585
 goto _LL6C;_LL6B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpD3=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpD3->tag != 6)goto _LL6D;else{_tmpD4=_tmpD3->f1;if(_tmpD4 == 0)goto _LL6D;_tmpD5=*_tmpD4;_tmpD6=_tmpD5.aggr_info;if((_tmpD6.UnknownAggr).tag != 1)goto _LL6D;_tmpD7=(struct _tuple3)(_tmpD6.UnknownAggr).val;}}_LL6C:
# 586
 goto _LL6E;_LL6D: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpD8=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpD8->tag != 14)goto _LL6F;}_LL6E:
# 587
 goto _LL70;_LL6F: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpD9=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpD9->tag != 16)goto _LL71;}_LL70:
# 588
 goto _LL72;_LL71: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpDA=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmpA0;if(_tmpDA->tag != 15)goto _LL48;}_LL72:
# 590
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL48;_LL48:;}
# 592
tcpat_end:
# 593
 p->topt=(void*)t;
# 594
return res;};}
# 597
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){
# 598
struct Cyc_Tcpat_TcPatResult _tmp18E=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0,0);
# 600
struct _RegionHandle _tmp18F=_new_region("r");struct _RegionHandle*r=& _tmp18F;_push_region(r);{
# 601
const char*_tmp4D4;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp18E.patvars),p->loc,(
# 602
(_tmp4D4="pattern contains a repeated variable",_tag_dyneither(_tmp4D4,sizeof(char),37))));}{
# 604
struct Cyc_Tcpat_TcPatResult _tmp191=_tmp18E;_npop_handler(0);return _tmp191;};
# 600
;_pop_region(r);}
# 607
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
# 608
void*_tmp192=p->r;struct Cyc_Absyn_Pat*_tmp194;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp198;struct Cyc_List_List*_tmp19A;struct Cyc_Absyn_Pat*_tmp19C;struct Cyc_Absyn_Pat*_tmp19E;_LLA0: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp193=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp193->tag != 5)goto _LLA2;else{_tmp194=_tmp193->f1;}}_LLA1:
# 610
 Cyc_Tcpat_check_pat_regions(te,_tmp194);{
# 612
struct _RegionHandle _tmp19F=_new_region("temp");struct _RegionHandle*temp=& _tmp19F;_push_region(temp);
# 613
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(_tmp194->topt))){
# 614
{const char*_tmp4D7;void*_tmp4D6;(_tmp4D6=0,Cyc_Tcutil_terr(p->loc,((_tmp4D7="Pattern dereferences to a non-aliased pointer; use swap",_tag_dyneither(_tmp4D7,sizeof(char),56))),_tag_dyneither(_tmp4D6,sizeof(void*),0)));}
# 615
_npop_handler(0);return;}
# 619
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(p->topt))){
# 620
{const char*_tmp4DA;void*_tmp4D9;(_tmp4D9=0,Cyc_Tcutil_terr(p->loc,((_tmp4DA="Pattern dereferences a non-aliased pointer; use swap",_tag_dyneither(_tmp4DA,sizeof(char),53))),_tag_dyneither(_tmp4D9,sizeof(void*),0)));}
# 621
_npop_handler(0);return;}{
# 624
void*_tmp1A4=(void*)_check_null(p->topt);struct Cyc_Absyn_PtrInfo _tmp1A6;struct Cyc_Absyn_PtrAtts _tmp1A7;void*_tmp1A8;_LLAF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4;if(_tmp1A5->tag != 5)goto _LLB1;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.ptr_atts;_tmp1A8=_tmp1A7.rgn;}}_LLB0:
# 626
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp1A8);
# 627
_npop_handler(0);return;_LLB1:;_LLB2: {
# 628
const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DD="check_pat_regions: bad pointer type",_tag_dyneither(_tmp4DD,sizeof(char),36))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}_LLAE:;};
# 612
;_pop_region(temp);};_LLA2: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp195=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp195->tag != 6)goto _LLA4;else{_tmp196=_tmp195->f3;}}_LLA3:
# 631
 for(0;_tmp196 != 0;_tmp196=_tmp196->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple15*)_tmp196->hd)).f2);}
# 632
return;_LLA4: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp197=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp197->tag != 7)goto _LLA6;else{_tmp198=_tmp197->f3;}}_LLA5:
# 634
 _tmp19A=_tmp198;goto _LLA7;_LLA6: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp199=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp199->tag != 4)goto _LLA8;else{_tmp19A=_tmp199->f1;}}_LLA7:
# 636
 for(0;_tmp19A != 0;_tmp19A=_tmp19A->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp19A->hd);}
# 637
return;_LLA8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp19B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp19B->tag != 2)goto _LLAA;else{_tmp19C=_tmp19B->f2;}}_LLA9:
# 638
 _tmp19E=_tmp19C;goto _LLAB;_LLAA: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp19D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp192;if(_tmp19D->tag != 1)goto _LLAC;else{_tmp19E=_tmp19D->f2;}}_LLAB:
# 639
 Cyc_Tcpat_check_pat_regions(te,_tmp19E);
# 640
return;_LLAC:;_LLAD:
# 641
 return;_LL9F:;}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 682 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
# 683
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp4DE;return((_tmp4DE.Name_v).val=s,(((_tmp4DE.Name_v).tag=1,_tmp4DE)));}
# 684
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp4DF;return((_tmp4DF.Int_v).val=i,(((_tmp4DF.Int_v).tag=2,_tmp4DF)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 692
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 697
typedef void*Cyc_Tcpat_simple_pat_t;
# 700
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
# 701
union Cyc_Tcpat_Name_value _tmp1AD=c1->name;struct _dyneither_ptr _tmp1AE;int _tmp1AF;_LLB4: if((_tmp1AD.Name_v).tag != 1)goto _LLB6;_tmp1AE=(struct _dyneither_ptr)(_tmp1AD.Name_v).val;_LLB5: {
# 703
union Cyc_Tcpat_Name_value _tmp1B0=c2->name;struct _dyneither_ptr _tmp1B1;int _tmp1B2;_LLB9: if((_tmp1B0.Name_v).tag != 1)goto _LLBB;_tmp1B1=(struct _dyneither_ptr)(_tmp1B0.Name_v).val;_LLBA:
# 704
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1AE,(struct _dyneither_ptr)_tmp1B1);_LLBB: if((_tmp1B0.Int_v).tag != 2)goto _LLB8;_tmp1B2=(int)(_tmp1B0.Int_v).val;_LLBC:
# 705
 return - 1;_LLB8:;}_LLB6: if((_tmp1AD.Int_v).tag != 2)goto _LLB3;_tmp1AF=(int)(_tmp1AD.Int_v).val;_LLB7: {
# 708
union Cyc_Tcpat_Name_value _tmp1B3=c2->name;struct _dyneither_ptr _tmp1B4;int _tmp1B5;_LLBE: if((_tmp1B3.Name_v).tag != 1)goto _LLC0;_tmp1B4=(struct _dyneither_ptr)(_tmp1B3.Name_v).val;_LLBF:
# 709
 return 1;_LLC0: if((_tmp1B3.Int_v).tag != 2)goto _LLBD;_tmp1B5=(int)(_tmp1B3.Int_v).val;_LLC1:
# 710
 return _tmp1AF - _tmp1B5;_LLBD:;}_LLB3:;}
# 716
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
# 717
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 720
static int Cyc_Tcpat_one_opt=1;
# 721
static int Cyc_Tcpat_two_opt=2;
# 722
static int Cyc_Tcpat_twofiftysix_opt=256;
# 724
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 725
const char*_tmp4E2;struct Cyc_Tcpat_Con_s*_tmp4E1;return(_tmp4E1=_region_malloc(r,sizeof(*_tmp4E1)),((_tmp4E1->name=Cyc_Tcpat_Name_v(((_tmp4E2="NULL",_tag_dyneither(_tmp4E2,sizeof(char),5)))),((_tmp4E1->arity=0,((_tmp4E1->span=(int*)& Cyc_Tcpat_two_opt,((_tmp4E1->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4E1)))))))));}
# 727
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 728
const char*_tmp4E5;struct Cyc_Tcpat_Con_s*_tmp4E4;return(_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4->name=Cyc_Tcpat_Name_v(((_tmp4E5="&",_tag_dyneither(_tmp4E5,sizeof(char),2)))),((_tmp4E4->arity=1,((_tmp4E4->span=(int*)& Cyc_Tcpat_two_opt,((_tmp4E4->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4E4)))))))));}
# 730
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 731
const char*_tmp4E8;struct Cyc_Tcpat_Con_s*_tmp4E7;return(_tmp4E7=_region_malloc(r,sizeof(*_tmp4E7)),((_tmp4E7->name=Cyc_Tcpat_Name_v(((_tmp4E8="&",_tag_dyneither(_tmp4E8,sizeof(char),2)))),((_tmp4E7->arity=1,((_tmp4E7->span=(int*)& Cyc_Tcpat_one_opt,((_tmp4E7->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4E7)))))))));}
# 733
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
# 734
struct Cyc_Tcpat_Con_s*_tmp4E9;return(_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9->name=Cyc_Tcpat_Int_v(i),((_tmp4E9->arity=0,((_tmp4E9->span=0,((_tmp4E9->orig_pat=p,_tmp4E9)))))))));}
# 736
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
# 737
struct Cyc_Tcpat_Con_s*_tmp4EA;return(_tmp4EA=_region_malloc(r,sizeof(*_tmp4EA)),((_tmp4EA->name=Cyc_Tcpat_Name_v(f),((_tmp4EA->arity=0,((_tmp4EA->span=0,((_tmp4EA->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4EA)))))))));}
# 739
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
# 740
struct Cyc_Tcpat_Con_s*_tmp4EB;return(_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB)),((_tmp4EB->name=Cyc_Tcpat_Int_v((int)c),((_tmp4EB->arity=0,((_tmp4EB->span=(int*)& Cyc_Tcpat_twofiftysix_opt,((_tmp4EB->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp4EB)))))))));}
# 742
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
# 743
const char*_tmp4EE;struct Cyc_Tcpat_Con_s*_tmp4ED;return(_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED->name=Cyc_Tcpat_Name_v(((_tmp4EE="$",_tag_dyneither(_tmp4EE,sizeof(char),2)))),((_tmp4ED->arity=i,((_tmp4ED->span=(int*)& Cyc_Tcpat_one_opt,((_tmp4ED->orig_pat=p,_tmp4ED)))))))));}
# 747
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 748
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4F1;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4F0;return(void*)((_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0)),((_tmp4F0[0]=((_tmp4F1.tag=1,((_tmp4F1.f1=Cyc_Tcpat_null_con(r,p),((_tmp4F1.f2=0,_tmp4F1)))))),_tmp4F0))));}
# 750
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
# 751
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4F4;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4F3;return(void*)((_tmp4F3=_region_malloc(r,sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F4.tag=1,((_tmp4F4.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp4F4.f2=0,_tmp4F4)))))),_tmp4F3))));}
# 753
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
# 754
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4F7;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4F6;return(void*)((_tmp4F6=_region_malloc(r,sizeof(*_tmp4F6)),((_tmp4F6[0]=((_tmp4F7.tag=1,((_tmp4F7.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp4F7.f2=0,_tmp4F7)))))),_tmp4F6))));}
# 756
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
# 757
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4FA;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4F9;return(void*)((_tmp4F9=_region_malloc(r,sizeof(*_tmp4F9)),((_tmp4F9[0]=((_tmp4FA.tag=1,((_tmp4FA.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp4FA.f2=0,_tmp4FA)))))),_tmp4F9))));}
# 759
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
# 760
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp500;struct Cyc_List_List*_tmp4FF;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4FE;return(void*)((_tmp4FE=_region_malloc(r,sizeof(*_tmp4FE)),((_tmp4FE[0]=((_tmp500.tag=1,((_tmp500.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp500.f2=((_tmp4FF=_region_malloc(r,sizeof(*_tmp4FF)),((_tmp4FF->hd=p,((_tmp4FF->tl=0,_tmp4FF)))))),_tmp500)))))),_tmp4FE))));}
# 762
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
# 763
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp506;struct Cyc_List_List*_tmp505;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp504;return(void*)((_tmp504=_region_malloc(r,sizeof(*_tmp504)),((_tmp504[0]=((_tmp506.tag=1,((_tmp506.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp506.f2=((_tmp505=_region_malloc(r,sizeof(*_tmp505)),((_tmp505->hd=p,((_tmp505->tl=0,_tmp505)))))),_tmp506)))))),_tmp504))));}
# 765
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
# 766
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp509;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp508;return(void*)((_tmp508=_region_malloc(r,sizeof(*_tmp508)),((_tmp508[0]=((_tmp509.tag=1,((_tmp509.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp509.f2=ss,_tmp509)))))),_tmp508))));}
# 768
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 770
struct Cyc_Tcpat_Con_s*_tmp50A;struct Cyc_Tcpat_Con_s*c=(_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A->name=Cyc_Tcpat_Name_v(con_name),((_tmp50A->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp50A->span=span,((_tmp50A->orig_pat=(struct Cyc_Absyn_Pat*)p,_tmp50A)))))))));
# 771
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50D;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp50C;return(void*)((_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C[0]=((_tmp50D.tag=1,((_tmp50D.f1=c,((_tmp50D.f2=ps,_tmp50D)))))),_tmp50C))));}
# 775
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
# 776
void*s;
# 777
{void*_tmp1D4=p->r;enum Cyc_Absyn_Sign _tmp1D9;int _tmp1DA;char _tmp1DC;struct _dyneither_ptr _tmp1DE;struct Cyc_Absyn_Pat*_tmp1E0;struct Cyc_Absyn_Pat*_tmp1E2;struct Cyc_Absyn_Pat*_tmp1E4;struct Cyc_Absyn_Datatypedecl*_tmp1E6;struct Cyc_Absyn_Datatypefield*_tmp1E7;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_AggrInfo*_tmp1EC;struct Cyc_Absyn_AggrInfo _tmp1ED;union Cyc_Absyn_AggrInfoU _tmp1EE;struct Cyc_Absyn_Aggrdecl**_tmp1EF;struct Cyc_Absyn_Aggrdecl*_tmp1F0;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Enumdecl*_tmp1F3;struct Cyc_Absyn_Enumfield*_tmp1F4;void*_tmp1F6;struct Cyc_Absyn_Enumfield*_tmp1F7;_LLC3: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1D5->tag != 0)goto _LLC5;}_LLC4:
# 778
 goto _LLC6;_LLC5: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1D6->tag != 3)goto _LLC7;}_LLC6:
# 779
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp510;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp50F;s=(void*)((_tmp50F=_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=0,_tmp510)),_tmp50F))));}goto _LLC2;_LLC7: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D7=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1D7->tag != 8)goto _LLC9;}_LLC8:
# 780
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC2;_LLC9: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1D8->tag != 9)goto _LLCB;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;}}_LLCA:
# 781
 s=Cyc_Tcpat_int_pat(r,_tmp1DA,(struct Cyc_Absyn_Pat*)p);goto _LLC2;_LLCB: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1DB=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1DB->tag != 10)goto _LLCD;else{_tmp1DC=_tmp1DB->f1;}}_LLCC:
# 782
 s=Cyc_Tcpat_char_pat(r,_tmp1DC,p);goto _LLC2;_LLCD: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1DD=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1DD->tag != 11)goto _LLCF;else{_tmp1DE=_tmp1DD->f1;}}_LLCE:
# 783
 s=Cyc_Tcpat_float_pat(r,_tmp1DE,p);goto _LLC2;_LLCF: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1DF->tag != 1)goto _LLD1;else{_tmp1E0=_tmp1DF->f2;}}_LLD0:
# 784
 s=Cyc_Tcpat_compile_pat(r,_tmp1E0);goto _LLC2;_LLD1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1E1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1E1->tag != 2)goto _LLD3;else{_tmp1E2=_tmp1E1->f2;}}_LLD2:
# 785
 s=Cyc_Tcpat_compile_pat(r,_tmp1E2);goto _LLC2;_LLD3: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1E3=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1E3->tag != 5)goto _LLD5;else{_tmp1E4=_tmp1E3->f1;}}_LLD4:
# 787
{void*_tmp1FE=Cyc_Tcutil_compress((void*)_check_null(p->topt));struct Cyc_Absyn_PtrInfo _tmp200;struct Cyc_Absyn_PtrAtts _tmp201;union Cyc_Absyn_Constraint*_tmp202;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 5)goto _LLEA;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp200.ptr_atts;_tmp202=_tmp201.nullable;}}_LLE9: {
# 789
int is_nullable=0;
# 790
int still_working=1;
# 791
while(still_working){
# 792
union Cyc_Absyn_Constraint*_tmp203=_tmp202;union Cyc_Absyn_Constraint _tmp204;union Cyc_Absyn_Constraint*_tmp205;union Cyc_Absyn_Constraint _tmp206;int _tmp207;union Cyc_Absyn_Constraint _tmp208;int _tmp209;_LLED: _tmp204=*_tmp203;if((_tmp204.Forward_constr).tag != 2)goto _LLEF;_tmp205=(union Cyc_Absyn_Constraint*)(_tmp204.Forward_constr).val;_LLEE:
# 794
*_tmp202=*_tmp205;
# 795
continue;_LLEF: _tmp206=*_tmp203;if((_tmp206.No_constr).tag != 3)goto _LLF1;_tmp207=(int)(_tmp206.No_constr).val;_LLF0:
# 797
{struct _union_Constraint_Eq_constr*_tmp511;(_tmp511=& _tmp202->Eq_constr,((_tmp511->tag=1,_tmp511->val=0)));}
# 798
is_nullable=0;
# 799
still_working=0;
# 800
goto _LLEC;_LLF1: _tmp208=*_tmp203;if((_tmp208.Eq_constr).tag != 1)goto _LLEC;_tmp209=(int)(_tmp208.Eq_constr).val;_LLF2:
# 802
 is_nullable=(int)_tmp209;
# 803
still_working=0;
# 804
goto _LLEC;_LLEC:;}{
# 807
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1E4);
# 808
if(is_nullable)
# 809
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 811
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
# 812
goto _LLE7;};}_LLEA:;_LLEB: {
# 813
const char*_tmp514;void*_tmp513;(_tmp513=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp514="expecting pointertype for pattern!",_tag_dyneither(_tmp514,sizeof(char),35))),_tag_dyneither(_tmp513,sizeof(void*),0)));}_LLE7:;}
# 815
goto _LLC2;_LLD5: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1E5=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1E5->tag != 7)goto _LLD7;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;_tmp1E8=_tmp1E5->f3;}}_LLD6: {
# 817
int*span;
# 818
{void*_tmp20D=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LLF4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp20D;if(_tmp20E->tag != 3)goto _LLF6;}_LLF5:
# 820
 if(_tmp1E6->is_extensible)
# 821
span=0;else{
# 823
int*_tmp515;span=((_tmp515=_region_malloc(r,sizeof(*_tmp515)),((_tmp515[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E6->fields))->v),_tmp515))));}
# 824
goto _LLF3;_LLF6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp20D;if(_tmp20F->tag != 4)goto _LLF8;}_LLF7:
# 825
 span=(int*)& Cyc_Tcpat_one_opt;goto _LLF3;_LLF8:;_LLF9:
# 826
{const char*_tmp518;void*_tmp517;span=((_tmp517=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp518="void datatype pattern has bad type",_tag_dyneither(_tmp518,sizeof(char),35))),_tag_dyneither(_tmp517,sizeof(void*),0))));}goto _LLF3;_LLF3:;}
# 828
s=Cyc_Tcpat_con_pat(r,*(*_tmp1E7->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1E8),p);
# 829
goto _LLC2;}_LLD7: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1E9=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1E9->tag != 4)goto _LLD9;else{_tmp1EA=_tmp1E9->f1;}}_LLD8:
# 832
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1EA),(struct Cyc_Absyn_Pat*)p);goto _LLC2;_LLD9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1EB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1EB->tag != 6)goto _LLDB;else{_tmp1EC=_tmp1EB->f1;if(_tmp1EC == 0)goto _LLDB;_tmp1ED=*_tmp1EC;_tmp1EE=_tmp1ED.aggr_info;if((_tmp1EE.KnownAggr).tag != 2)goto _LLDB;_tmp1EF=(struct Cyc_Absyn_Aggrdecl**)(_tmp1EE.KnownAggr).val;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1EB->f3;}}_LLDA:
# 837
 if(_tmp1F0->kind == Cyc_Absyn_StructA){
# 838
struct Cyc_List_List*ps=0;
# 839
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F0->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 841
const char*_tmp519;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp519="",_tag_dyneither(_tmp519,sizeof(char),1))))== 0;
# 842
{struct Cyc_List_List*dlps0=_tmp1F1;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
# 843
struct _tuple15 _tmp214;struct Cyc_List_List*_tmp215;struct Cyc_Absyn_Pat*_tmp216;struct _tuple15*_tmp213=(struct _tuple15*)dlps0->hd;_tmp214=*_tmp213;_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;{
# 844
struct Cyc_List_List*_tmp217=_tmp215;struct Cyc_List_List _tmp218;void*_tmp219;struct _dyneither_ptr*_tmp21B;struct Cyc_List_List*_tmp21C;_LLFB: if(_tmp217 == 0)goto _LLFD;_tmp218=*_tmp217;_tmp219=(void*)_tmp218.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp21A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp219;if(_tmp21A->tag != 1)goto _LLFD;else{_tmp21B=_tmp21A->f1;}};_tmp21C=_tmp218.tl;if(_tmp21C != 0)goto _LLFD;_LLFC:
# 846
 if(Cyc_strptrcmp(_tmp21B,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
# 847
{struct Cyc_List_List*_tmp51A;ps=((_tmp51A=_region_malloc(r,sizeof(*_tmp51A)),((_tmp51A->hd=Cyc_Tcpat_compile_pat(r,_tmp216),((_tmp51A->tl=ps,_tmp51A))))));}
# 848
found=1;}
# 850
goto _LLFA;_LLFD:;_LLFE: {
# 851
const char*_tmp51D;void*_tmp51C;(_tmp51C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51D="bad designator(s)",_tag_dyneither(_tmp51D,sizeof(char),18))),_tag_dyneither(_tmp51C,sizeof(void*),0)));}_LLFA:;};}}
# 854
if(!found){
# 855
const char*_tmp520;void*_tmp51F;(_tmp51F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp520="bad designator",_tag_dyneither(_tmp520,sizeof(char),15))),_tag_dyneither(_tmp51F,sizeof(void*),0)));}}}
# 857
s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{
# 860
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F0->impl))->tagged){
# 861
const char*_tmp523;void*_tmp522;(_tmp522=0,Cyc_Tcutil_terr(p->loc,((_tmp523="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp523,sizeof(char),47))),_tag_dyneither(_tmp522,sizeof(void*),0)));}{
# 862
int*_tmp524;int*span=(_tmp524=_region_malloc(r,sizeof(*_tmp524)),((_tmp524[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F0->impl))->fields),_tmp524)));
# 863
struct Cyc_List_List*_tmp225=_tmp1F1;struct Cyc_List_List _tmp226;struct _tuple15*_tmp227;struct _tuple15 _tmp228;struct Cyc_List_List*_tmp229;struct Cyc_List_List _tmp22A;void*_tmp22B;struct _dyneither_ptr*_tmp22D;struct Cyc_List_List*_tmp22E;struct Cyc_Absyn_Pat*_tmp22F;struct Cyc_List_List*_tmp230;_LL100: if(_tmp225 == 0)goto _LL102;_tmp226=*_tmp225;_tmp227=(struct _tuple15*)_tmp226.hd;_tmp228=*_tmp227;_tmp229=_tmp228.f1;if(_tmp229 == 0)goto _LL102;_tmp22A=*_tmp229;_tmp22B=(void*)_tmp22A.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp22C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp22B;if(_tmp22C->tag != 1)goto _LL102;else{_tmp22D=_tmp22C->f1;}};_tmp22E=_tmp22A.tl;if(_tmp22E != 0)goto _LL102;_tmp22F=_tmp228.f2;_tmp230=_tmp226.tl;if(_tmp230 != 0)goto _LL102;_LL101:
# 865
{struct Cyc_List_List*_tmp525;s=Cyc_Tcpat_con_pat(r,*_tmp22D,span,((_tmp525=_region_malloc(r,sizeof(*_tmp525)),((_tmp525->hd=Cyc_Tcpat_compile_pat(r,_tmp22F),((_tmp525->tl=0,_tmp525)))))),p);}
# 866
goto _LLFF;_LL102:;_LL103: {
# 867
const char*_tmp528;void*_tmp527;(_tmp527=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp528="bad union pattern",_tag_dyneither(_tmp528,sizeof(char),18))),_tag_dyneither(_tmp527,sizeof(void*),0)));}_LLFF:;};}
# 870
goto _LLC2;_LLDB: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1F2=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1F2->tag != 12)goto _LLDD;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F2->f2;}}_LLDC: {
# 875
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1F3->fields))->v);
# 876
{int*_tmp529;s=Cyc_Tcpat_con_pat(r,*(*_tmp1F4->name).f2,((_tmp529=_region_malloc(r,sizeof(*_tmp529)),((_tmp529[0]=span,_tmp529)))),0,p);}
# 877
goto _LLC2;}_LLDD: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1F5=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1F5->tag != 13)goto _LLDF;else{_tmp1F6=(void*)_tmp1F5->f1;_tmp1F7=_tmp1F5->f2;}}_LLDE: {
# 882
struct Cyc_List_List*fields;
# 883
{void*_tmp236=Cyc_Tcutil_compress(_tmp1F6);struct Cyc_List_List*_tmp238;_LL105: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp236;if(_tmp237->tag != 14)goto _LL107;else{_tmp238=_tmp237->f1;}}_LL106:
# 884
 fields=_tmp238;goto _LL104;_LL107:;_LL108: {
# 885
const char*_tmp52C;void*_tmp52B;(_tmp52B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp52C="bad type in AnonEnum_p",_tag_dyneither(_tmp52C,sizeof(char),23))),_tag_dyneither(_tmp52B,sizeof(void*),0)));}_LL104:;}{
# 889
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
# 890
{int*_tmp52D;s=Cyc_Tcpat_con_pat(r,*(*_tmp1F7->name).f2,((_tmp52D=_region_malloc(r,sizeof(*_tmp52D)),((_tmp52D[0]=span,_tmp52D)))),0,p);}
# 891
goto _LLC2;};}_LLDF: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1F8=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1F8->tag != 14)goto _LLE1;}_LLE0:
# 893
 goto _LLE2;_LLE1: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1F9=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1F9->tag != 15)goto _LLE3;}_LLE2:
# 894
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1FA=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1FA->tag != 6)goto _LLE5;}_LLE4:
# 895
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1FB=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D4;if(_tmp1FB->tag != 16)goto _LLC2;}_LLE6: {
# 896
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp530;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp52F;s=(void*)((_tmp52F=_region_malloc(r,sizeof(*_tmp52F)),((_tmp52F[0]=((_tmp530.tag=0,_tmp530)),_tmp52F))));}_LLC2:;}
# 898
return s;}
# 906
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 917
typedef void*Cyc_Tcpat_term_desc_t;
# 921
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 928
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 937
typedef void*Cyc_Tcpat_decision_t;struct _tuple18{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 942
typedef struct _tuple18*Cyc_Tcpat_work_stack_frame_t;
# 945
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 947
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
# 948
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 950
static void Cyc_Tcpat_print_tab(int i){
# 951
for(0;i != 0;-- i){
# 952
const char*_tmp533;void*_tmp532;(_tmp532=0,Cyc_fprintf(Cyc_stderr,((_tmp533=" ",_tag_dyneither(_tmp533,sizeof(char),2))),_tag_dyneither(_tmp532,sizeof(void*),0)));}}
# 956
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
# 957
union Cyc_Tcpat_Name_value _tmp240=c->name;
# 958
union Cyc_Tcpat_Name_value _tmp241=_tmp240;struct _dyneither_ptr _tmp242;int _tmp243;_LL10A: if((_tmp241.Name_v).tag != 1)goto _LL10C;_tmp242=(struct _dyneither_ptr)(_tmp241.Name_v).val;_LL10B:
# 959
{const char*_tmp537;void*_tmp536[1];struct Cyc_String_pa_PrintArg_struct _tmp535;(_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp242),((_tmp536[0]=& _tmp535,Cyc_fprintf(Cyc_stderr,((_tmp537="%s",_tag_dyneither(_tmp537,sizeof(char),3))),_tag_dyneither(_tmp536,sizeof(void*),1)))))));}goto _LL109;_LL10C: if((_tmp241.Int_v).tag != 2)goto _LL109;_tmp243=(int)(_tmp241.Int_v).val;_LL10D:
# 960
{const char*_tmp53B;void*_tmp53A[1];struct Cyc_Int_pa_PrintArg_struct _tmp539;(_tmp539.tag=1,((_tmp539.f1=(unsigned long)_tmp243,((_tmp53A[0]=& _tmp539,Cyc_fprintf(Cyc_stderr,((_tmp53B="%d",_tag_dyneither(_tmp53B,sizeof(char),3))),_tag_dyneither(_tmp53A,sizeof(void*),1)))))));}goto _LL109;_LL109:;}
# 964
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
# 965
void*_tmp24A=d;void*_tmp24C;struct Cyc_List_List*_tmp24F;struct Cyc_Tcpat_Con_s*_tmp250;void*_tmp251;void*_tmp252;_LL10F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp24B=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp24A;if(_tmp24B->tag != 1)goto _LL111;else{_tmp24C=(void*)_tmp24B->f1;}}_LL110:
# 967
 Cyc_Tcpat_print_tab(tab);
# 968
{const char*_tmp53E;void*_tmp53D;(_tmp53D=0,Cyc_fprintf(Cyc_stderr,((_tmp53E="Success(",_tag_dyneither(_tmp53E,sizeof(char),9))),_tag_dyneither(_tmp53D,sizeof(void*),0)));}print_rhs(_tmp24C);{const char*_tmp541;void*_tmp540;(_tmp540=0,Cyc_fprintf(Cyc_stderr,((_tmp541=")\n",_tag_dyneither(_tmp541,sizeof(char),3))),_tag_dyneither(_tmp540,sizeof(void*),0)));}
# 969
goto _LL10E;_LL111: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp24D=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp24A;if(_tmp24D->tag != 0)goto _LL113;}_LL112:
# 970
{const char*_tmp544;void*_tmp543;(_tmp543=0,Cyc_fprintf(Cyc_stderr,((_tmp544="Failure\n",_tag_dyneither(_tmp544,sizeof(char),9))),_tag_dyneither(_tmp543,sizeof(void*),0)));}goto _LL10E;_LL113: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp24E=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp24A;if(_tmp24E->tag != 2)goto _LL10E;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;_tmp251=(void*)_tmp24E->f3;_tmp252=(void*)_tmp24E->f4;}}_LL114:
# 972
 Cyc_Tcpat_print_tab(tab);
# 973
{const char*_tmp547;void*_tmp546;(_tmp546=0,Cyc_fprintf(Cyc_stderr,((_tmp547="Access[",_tag_dyneither(_tmp547,sizeof(char),8))),_tag_dyneither(_tmp546,sizeof(void*),0)));}
# 974
for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
# 975
{const char*_tmp54B;void*_tmp54A[1];struct Cyc_Int_pa_PrintArg_struct _tmp549;(_tmp549.tag=1,((_tmp549.f1=(unsigned long)((int)_tmp24F->hd),((_tmp54A[0]=& _tmp549,Cyc_fprintf(Cyc_stderr,((_tmp54B="%d",_tag_dyneither(_tmp54B,sizeof(char),3))),_tag_dyneither(_tmp54A,sizeof(void*),1)))))));}
# 976
if(_tmp24F->tl != 0){const char*_tmp54E;void*_tmp54D;(_tmp54D=0,Cyc_fprintf(Cyc_stderr,((_tmp54E=",",_tag_dyneither(_tmp54E,sizeof(char),2))),_tag_dyneither(_tmp54D,sizeof(void*),0)));}}
# 978
{const char*_tmp551;void*_tmp550;(_tmp550=0,Cyc_fprintf(Cyc_stderr,((_tmp551="],",_tag_dyneither(_tmp551,sizeof(char),3))),_tag_dyneither(_tmp550,sizeof(void*),0)));}
# 979
Cyc_Tcpat_print_con(_tmp250);
# 980
{const char*_tmp554;void*_tmp553;(_tmp553=0,Cyc_fprintf(Cyc_stderr,((_tmp554=",\n",_tag_dyneither(_tmp554,sizeof(char),3))),_tag_dyneither(_tmp553,sizeof(void*),0)));}
# 981
Cyc_Tcpat_print_dec_tree(_tmp251,print_rhs,tab + 7);
# 982
Cyc_Tcpat_print_dec_tree(_tmp252,print_rhs,tab + 7);_LL10E:;}struct _tuple19{int f1;unsigned int f2;};
# 986
static void Cyc_Tcpat_print_swrhs(struct _tuple19*x){
# 987
return;}
# 994
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
# 995
void*_tmp264=td;struct Cyc_Set_Set*_tmp266;_LL116: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp265=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp264;if(_tmp265->tag != 1)goto _LL118;else{_tmp266=_tmp265->f1;}}_LL117: {
# 1004
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp557;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp556;return(void*)((_tmp556=_region_malloc(r,sizeof(*_tmp556)),((_tmp556[0]=((_tmp557.tag=1,((_tmp557.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp266,c),_tmp557)))),_tmp556))));}_LL118: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp267=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp264;if(_tmp267->tag != 0)goto _LL115;}_LL119: {
# 1005
const char*_tmp55A;void*_tmp559;(_tmp559=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp55A="add_neg called when td is Positive",_tag_dyneither(_tmp55A,sizeof(char),35))),_tag_dyneither(_tmp559,sizeof(void*),0)));}_LL115:;}
# 1011
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
# 1012
void*_tmp26C=td;struct Cyc_Tcpat_Con_s*_tmp26E;struct Cyc_Set_Set*_tmp270;_LL11B: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp26D=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp26C;if(_tmp26D->tag != 0)goto _LL11D;else{_tmp26E=_tmp26D->f1;}}_LL11C:
# 1015
 if(Cyc_Tcpat_compare_con(c,_tmp26E)== 0)return Cyc_Tcpat_Yes;else{
# 1016
return Cyc_Tcpat_No;}_LL11D: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp26F=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp26C;if(_tmp26F->tag != 1)goto _LL11A;else{_tmp270=_tmp26F->f1;}}_LL11E:
# 1019
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp270,c))return Cyc_Tcpat_No;else{
# 1022
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp270)+ 1)
# 1023
return Cyc_Tcpat_Yes;else{
# 1025
return Cyc_Tcpat_Maybe;}}_LL11A:;}struct _tuple20{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1033
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1035
struct Cyc_List_List*_tmp271=ctxt;struct Cyc_List_List _tmp272;struct _tuple20*_tmp273;struct _tuple20 _tmp274;struct Cyc_Tcpat_Con_s*_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;_LL120: if(_tmp271 != 0)goto _LL122;_LL121:
# 1036
 return 0;_LL122: if(_tmp271 == 0)goto _LL11F;_tmp272=*_tmp271;_tmp273=(struct _tuple20*)_tmp272.hd;_tmp274=*_tmp273;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;_tmp277=_tmp272.tl;_LL123: {
# 1038
struct _tuple20*_tmp560;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp55E;return(_tmp55E=_region_malloc(r,sizeof(*_tmp55E)),((_tmp55E->hd=(
# 1039
(_tmp560=_region_malloc(r,sizeof(*_tmp560)),((_tmp560->f1=_tmp275,((_tmp560->f2=(struct Cyc_List_List*)((_tmp55F=_region_malloc(r,sizeof(*_tmp55F)),((_tmp55F->hd=dsc,((_tmp55F->tl=_tmp276,_tmp55F)))))),_tmp560)))))),((_tmp55E->tl=_tmp277,_tmp55E)))));}_LL11F:;}
# 1046
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
# 1047
struct Cyc_List_List*_tmp27B=ctxt;struct Cyc_List_List _tmp27C;struct _tuple20*_tmp27D;struct _tuple20 _tmp27E;struct Cyc_Tcpat_Con_s*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp281;_LL125: if(_tmp27B != 0)goto _LL127;_LL126: {
# 1048
const char*_tmp563;void*_tmp562;(_tmp562=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp563="norm_context: empty context",_tag_dyneither(_tmp563,sizeof(char),28))),_tag_dyneither(_tmp562,sizeof(void*),0)));}_LL127: if(_tmp27B == 0)goto _LL124;_tmp27C=*_tmp27B;_tmp27D=(struct _tuple20*)_tmp27C.hd;_tmp27E=*_tmp27D;_tmp27F=_tmp27E.f1;_tmp280=_tmp27E.f2;_tmp281=_tmp27C.tl;_LL128: {
# 1050
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp566;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp565;return Cyc_Tcpat_augment(r,_tmp281,(void*)((_tmp565=_region_malloc(r,sizeof(*_tmp565)),((_tmp565[0]=((_tmp566.tag=0,((_tmp566.f1=_tmp27F,((_tmp566.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp280),_tmp566)))))),_tmp565)))));}_LL124:;}
# 1059
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1061
struct _tuple13 _tmp567;struct _tuple13 _tmp287=(_tmp567.f1=ctxt,((_tmp567.f2=work,_tmp567)));struct Cyc_List_List*_tmp288;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_List_List*_tmp28C;struct Cyc_List_List _tmp28D;struct _tuple20*_tmp28E;struct _tuple20 _tmp28F;struct Cyc_Tcpat_Con_s*_tmp290;struct Cyc_List_List*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp293;struct Cyc_List_List _tmp294;struct _tuple18*_tmp295;struct _tuple18 _tmp296;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp298;_LL12A: _tmp288=_tmp287.f1;if(_tmp288 != 0)goto _LL12C;_tmp289=_tmp287.f2;if(_tmp289 != 0)goto _LL12C;_LL12B:
# 1062
 return dsc;_LL12C: _tmp28A=_tmp287.f1;if(_tmp28A != 0)goto _LL12E;_LL12D:
# 1063
 goto _LL12F;_LL12E: _tmp28B=_tmp287.f2;if(_tmp28B != 0)goto _LL130;_LL12F: {
# 1064
const char*_tmp56A;void*_tmp569;(_tmp569=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp56A="build_desc: ctxt and work don't match",_tag_dyneither(_tmp56A,sizeof(char),38))),_tag_dyneither(_tmp569,sizeof(void*),0)));}_LL130: _tmp28C=_tmp287.f1;if(_tmp28C == 0)goto _LL129;_tmp28D=*_tmp28C;_tmp28E=(struct _tuple20*)_tmp28D.hd;_tmp28F=*_tmp28E;_tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_tmp292=_tmp28D.tl;_tmp293=_tmp287.f2;if(_tmp293 == 0)goto _LL129;_tmp294=*_tmp293;_tmp295=(struct _tuple18*)_tmp294.hd;_tmp296=*_tmp295;_tmp297=_tmp296.f3;_tmp298=_tmp294.tl;_LL131: {
# 1066
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp570;struct Cyc_List_List*_tmp56F;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp56E;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp29B=(_tmp56E=_region_malloc(r,sizeof(*_tmp56E)),((_tmp56E[0]=((_tmp570.tag=0,((_tmp570.f1=_tmp290,((_tmp570.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp291),(
# 1067
(_tmp56F=_region_malloc(r,sizeof(*_tmp56F)),((_tmp56F->hd=dsc,((_tmp56F->tl=_tmp297,_tmp56F))))))),_tmp570)))))),_tmp56E)));
# 1068
return Cyc_Tcpat_build_desc(r,_tmp292,(void*)_tmp29B,_tmp298);}_LL129:;}
# 1072
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1079
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1081
struct Cyc_List_List*_tmp29F=allmrules;struct Cyc_List_List _tmp2A0;struct _tuple0*_tmp2A1;struct _tuple0 _tmp2A2;void*_tmp2A3;void*_tmp2A4;struct Cyc_List_List*_tmp2A5;_LL133: if(_tmp29F != 0)goto _LL135;_LL134: {
# 1082
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp573;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp572;return(void*)((_tmp572=_cycalloc(sizeof(*_tmp572)),((_tmp572[0]=((_tmp573.tag=0,((_tmp573.f1=(void*)dsc,_tmp573)))),_tmp572))));}_LL135: if(_tmp29F == 0)goto _LL132;_tmp2A0=*_tmp29F;_tmp2A1=(struct _tuple0*)_tmp2A0.hd;_tmp2A2=*_tmp2A1;_tmp2A3=_tmp2A2.f1;_tmp2A4=_tmp2A2.f2;_tmp2A5=_tmp2A0.tl;_LL136:
# 1084
 return Cyc_Tcpat_match(r,_tmp2A3,0,dsc,0,0,_tmp2A4,_tmp2A5);_LL132:;}
# 1089
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1091
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp576;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp575;return Cyc_Tcpat_or_match(r,(void*)((_tmp575=_region_malloc(r,sizeof(*_tmp575)),((_tmp575[0]=((_tmp576.tag=1,((_tmp576.f1=Cyc_Tcpat_empty_con_set(r),_tmp576)))),_tmp575)))),allmrules);}
# 1097
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1101
struct Cyc_List_List*_tmp2AA=work;struct Cyc_List_List _tmp2AB;struct _tuple18*_tmp2AC;struct _tuple18 _tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_List_List _tmp2B2;struct _tuple18*_tmp2B3;struct _tuple18 _tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B8;_LL138: if(_tmp2AA != 0)goto _LL13A;_LL139: {
# 1102
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp579;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp578;return(void*)((_tmp578=_region_malloc(r,sizeof(*_tmp578)),((_tmp578[0]=((_tmp579.tag=1,((_tmp579.f1=(void*)right_hand_side,_tmp579)))),_tmp578))));}_LL13A: if(_tmp2AA == 0)goto _LL13C;_tmp2AB=*_tmp2AA;_tmp2AC=(struct _tuple18*)_tmp2AB.hd;_tmp2AD=*_tmp2AC;_tmp2AE=_tmp2AD.f1;if(_tmp2AE != 0)goto _LL13C;_tmp2AF=_tmp2AD.f2;if(_tmp2AF != 0)goto _LL13C;_tmp2B0=_tmp2AD.f3;if(_tmp2B0 != 0)goto _LL13C;_tmp2B1=_tmp2AB.tl;_LL13B:
# 1104
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp2B1,right_hand_side,rules);_LL13C: if(_tmp2AA == 0)goto _LL137;_tmp2B2=*_tmp2AA;_tmp2B3=(struct _tuple18*)_tmp2B2.hd;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B4.f1;_tmp2B6=_tmp2B4.f2;_tmp2B7=_tmp2B4.f3;_tmp2B8=_tmp2B2.tl;_LL13D:
# 1106
 if((_tmp2B5 == 0  || _tmp2B6 == 0) || _tmp2B7 == 0){
# 1107
const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_Tcutil_impos(((_tmp57C="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp57C,sizeof(char),38))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}{
# 1108
struct Cyc_List_List _tmp2BE;void*_tmp2BF;struct Cyc_List_List*_tmp2C0;struct Cyc_List_List*_tmp2BD=(struct Cyc_List_List*)_tmp2B5;_tmp2BE=*_tmp2BD;_tmp2BF=(void*)_tmp2BE.hd;_tmp2C0=_tmp2BE.tl;{
# 1109
struct Cyc_List_List _tmp2C2;struct Cyc_List_List*_tmp2C3;struct Cyc_List_List*_tmp2C4;struct Cyc_List_List*_tmp2C1=(struct Cyc_List_List*)_tmp2B6;_tmp2C2=*_tmp2C1;_tmp2C3=(struct Cyc_List_List*)_tmp2C2.hd;_tmp2C4=_tmp2C2.tl;{
# 1110
struct Cyc_List_List _tmp2C6;void*_tmp2C7;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2C5=(struct Cyc_List_List*)_tmp2B7;_tmp2C6=*_tmp2C5;_tmp2C7=(void*)_tmp2C6.hd;_tmp2C8=_tmp2C6.tl;{
# 1111
struct _tuple18*_tmp57D;struct _tuple18*_tmp2C9=(_tmp57D=_region_malloc(r,sizeof(*_tmp57D)),((_tmp57D->f1=_tmp2C0,((_tmp57D->f2=_tmp2C4,((_tmp57D->f3=_tmp2C8,_tmp57D)))))));
# 1112
struct Cyc_List_List*_tmp57E;return Cyc_Tcpat_match(r,_tmp2BF,_tmp2C3,_tmp2C7,ctx,((_tmp57E=_region_malloc(r,sizeof(*_tmp57E)),((_tmp57E->hd=_tmp2C9,((_tmp57E->tl=_tmp2B8,_tmp57E)))))),right_hand_side,rules);};};};};_LL137:;}
# 1118
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1120
void*_tmp2CC=dsc;struct Cyc_Set_Set*_tmp2CE;struct Cyc_List_List*_tmp2D0;_LL13F: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2CD=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2CC;if(_tmp2CD->tag != 1)goto _LL141;else{_tmp2CE=_tmp2CD->f1;}}_LL140: {
# 1125
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp581;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp580;void*any=(void*)((_tmp580=_region_malloc(r,sizeof(*_tmp580)),((_tmp580[0]=((_tmp581.tag=1,((_tmp581.f1=Cyc_Tcpat_empty_con_set(r),_tmp581)))),_tmp580))));
# 1126
struct Cyc_List_List*_tmp2D1=0;
# 1127
{int i=0;for(0;i < pcon->arity;++ i){
# 1128
struct Cyc_List_List*_tmp582;_tmp2D1=((_tmp582=_region_malloc(r,sizeof(*_tmp582)),((_tmp582->hd=any,((_tmp582->tl=_tmp2D1,_tmp582))))));}}
# 1129
return _tmp2D1;}_LL141: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2CF=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2CC;if(_tmp2CF->tag != 0)goto _LL13E;else{_tmp2D0=_tmp2CF->f2;}}_LL142:
# 1130
 return _tmp2D0;_LL13E:;}struct _tuple21{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1135
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple21*env,int i){
# 1136
struct _tuple21 _tmp2D6;struct _RegionHandle*_tmp2D7;struct Cyc_List_List*_tmp2D8;struct _tuple21*_tmp2D5=env;_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{
# 1137
struct Cyc_List_List*_tmp583;return(_tmp583=_region_malloc(_tmp2D7,sizeof(*_tmp583)),((_tmp583->hd=(void*)(i + 1),((_tmp583->tl=_tmp2D8,_tmp583)))));};}
# 1139
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1141
struct _tuple21 _tmp584;struct _tuple21 _tmp2DA=(_tmp584.f1=r,((_tmp584.f2=obj,_tmp584)));
# 1142
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp2DA);}
# 1148
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1152
void*_tmp2DC=p;struct Cyc_Tcpat_Con_s*_tmp2DF;struct Cyc_List_List*_tmp2E0;_LL144: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2DD=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2DC;if(_tmp2DD->tag != 0)goto _LL146;}_LL145:
# 1154
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL146: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2DE=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2DC;if(_tmp2DE->tag != 1)goto _LL143;else{_tmp2DF=_tmp2DE->f1;_tmp2E0=_tmp2DE->f2;}}_LL147:
# 1156
 switch(Cyc_Tcpat_static_match(_tmp2DF,dsc)){case Cyc_Tcpat_Yes: _LL148: {
# 1158
struct _tuple20*_tmp587;struct Cyc_List_List*_tmp586;struct Cyc_List_List*ctx2=(_tmp586=_region_malloc(r,sizeof(*_tmp586)),((_tmp586->hd=((_tmp587=_region_malloc(r,sizeof(*_tmp587)),((_tmp587->f1=_tmp2DF,((_tmp587->f2=0,_tmp587)))))),((_tmp586->tl=ctx,_tmp586)))));
# 1159
struct _tuple18*_tmp588;struct _tuple18*work_frame=(_tmp588=_region_malloc(r,sizeof(*_tmp588)),((_tmp588->f1=_tmp2E0,((_tmp588->f2=Cyc_Tcpat_getoargs(r,_tmp2DF,obj),((_tmp588->f3=
# 1160
Cyc_Tcpat_getdargs(r,_tmp2DF,dsc),_tmp588)))))));
# 1161
struct Cyc_List_List*_tmp589;struct Cyc_List_List*work2=(_tmp589=_region_malloc(r,sizeof(*_tmp589)),((_tmp589->hd=work_frame,((_tmp589->tl=work,_tmp589)))));
# 1162
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL149:
# 1164
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL14A: {
# 1166
struct _tuple20*_tmp58C;struct Cyc_List_List*_tmp58B;struct Cyc_List_List*ctx2=(_tmp58B=_region_malloc(r,sizeof(*_tmp58B)),((_tmp58B->hd=((_tmp58C=_region_malloc(r,sizeof(*_tmp58C)),((_tmp58C->f1=_tmp2DF,((_tmp58C->f2=0,_tmp58C)))))),((_tmp58B->tl=ctx,_tmp58B)))));
# 1167
struct _tuple18*_tmp58D;struct _tuple18*work_frame=(_tmp58D=_region_malloc(r,sizeof(*_tmp58D)),((_tmp58D->f1=_tmp2E0,((_tmp58D->f2=Cyc_Tcpat_getoargs(r,_tmp2DF,obj),((_tmp58D->f3=
# 1168
Cyc_Tcpat_getdargs(r,_tmp2DF,dsc),_tmp58D)))))));
# 1169
struct Cyc_List_List*_tmp58E;struct Cyc_List_List*work2=(_tmp58E=_region_malloc(r,sizeof(*_tmp58E)),((_tmp58E->hd=work_frame,((_tmp58E->tl=work,_tmp58E)))));
# 1170
void*_tmp2E5=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
# 1171
void*_tmp2E6=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2DF),work),rules);
# 1173
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp591;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp590;return(void*)((_tmp590=_region_malloc(r,sizeof(*_tmp590)),((_tmp590[0]=((_tmp591.tag=2,((_tmp591.f1=obj,((_tmp591.f2=_tmp2DF,((_tmp591.f3=(void*)_tmp2E5,((_tmp591.f4=(void*)_tmp2E6,_tmp591)))))))))),_tmp590))));}}_LL143:;}
# 1183
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1189
void*_tmp2ED=d;void*_tmp2EF;void*_tmp2F1;void*_tmp2F3;void*_tmp2F4;_LL14D: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2EE=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2ED;if(_tmp2EE->tag != 0)goto _LL14F;else{_tmp2EF=(void*)_tmp2EE->f1;}}_LL14E:
# 1191
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2EF);*exhaust_done=1;}
# 1192
goto _LL14C;_LL14F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2F0=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2ED;if(_tmp2F0->tag != 1)goto _LL151;else{_tmp2F1=(void*)_tmp2F0->f1;}}_LL150:
# 1193
 rhs_appears(env2,_tmp2F1);goto _LL14C;_LL151: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2F2=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2ED;if(_tmp2F2->tag != 2)goto _LL14C;else{_tmp2F3=(void*)_tmp2F2->f3;_tmp2F4=(void*)_tmp2F2->f4;}}_LL152:
# 1195
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2F3,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1197
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2F4,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1199
goto _LL14C;_LL14C:;}struct _tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{struct _RegionHandle*f1;int*f2;};
# 1207
static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*swc){
# 1210
struct _tuple23 _tmp2F6;struct _RegionHandle*_tmp2F7;int*_tmp2F8;struct _tuple23*_tmp2F5=env;_tmp2F6=*_tmp2F5;_tmp2F7=_tmp2F6.f1;_tmp2F8=_tmp2F6.f2;{
# 1211
void*sp0=Cyc_Tcpat_compile_pat(_tmp2F7,swc->pattern);
# 1212
struct _tuple19*_tmp592;struct _tuple19*rhs=(_tmp592=_region_malloc(_tmp2F7,sizeof(*_tmp592)),((_tmp592->f1=0,((_tmp592->f2=(swc->pattern)->loc,_tmp592)))));
# 1213
void*sp;
# 1214
if(swc->where_clause != 0){
# 1215
{struct Cyc_List_List*_tmp595;struct Cyc_List_List*_tmp594;sp=Cyc_Tcpat_tuple_pat(_tmp2F7,((_tmp594=_region_malloc(_tmp2F7,sizeof(*_tmp594)),((_tmp594->hd=sp0,((_tmp594->tl=(
# 1216
(_tmp595=_region_malloc(_tmp2F7,sizeof(*_tmp595)),((_tmp595->hd=Cyc_Tcpat_int_pat(_tmp2F7,*_tmp2F8,0),((_tmp595->tl=0,_tmp595)))))),_tmp594)))))),0);}
# 1217
*_tmp2F8=*_tmp2F8 + 1;}else{
# 1219
struct Cyc_List_List*_tmp59F;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp59E;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp59D;struct Cyc_List_List*_tmp59C;sp=Cyc_Tcpat_tuple_pat(_tmp2F7,((_tmp59C=_region_malloc(_tmp2F7,sizeof(*_tmp59C)),((_tmp59C->hd=sp0,((_tmp59C->tl=((_tmp59F=_region_malloc(_tmp2F7,sizeof(*_tmp59F)),((_tmp59F->hd=(void*)((_tmp59D=_region_malloc(_tmp2F7,sizeof(*_tmp59D)),((_tmp59D[0]=((_tmp59E.tag=0,_tmp59E)),_tmp59D)))),((_tmp59F->tl=0,_tmp59F)))))),_tmp59C)))))),0);}{
# 1220
struct _tuple22*_tmp5A0;return(_tmp5A0=_region_malloc(_tmp2F7,sizeof(*_tmp5A0)),((_tmp5A0->f1=sp,((_tmp5A0->f2=rhs,_tmp5A0)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1225
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1227
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
# 1228
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
# 1229
void*_tmp302=d;struct Cyc_Tcpat_Con_s*_tmp304;struct Cyc_List_List*_tmp305;struct Cyc_Set_Set*_tmp307;_LL154: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp303=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL156;else{_tmp304=_tmp303->f1;_tmp305=_tmp303->f2;}}_LL155: {
# 1231
union Cyc_Tcpat_Name_value _tmp308=_tmp304->name;
# 1232
struct Cyc_Absyn_Pat*_tmp309=_tmp304->orig_pat;
# 1233
if(_tmp309 == 0)
# 1234
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp305))->hd);{
# 1236
void*_tmp30A=_tmp309->r;struct Cyc_Absyn_Vardecl*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;struct Cyc_Absyn_Tvar*_tmp311;struct Cyc_Absyn_Vardecl*_tmp312;struct Cyc_Absyn_AggrInfo*_tmp316;struct Cyc_Absyn_AggrInfo _tmp317;union Cyc_Absyn_AggrInfoU _tmp318;struct Cyc_Absyn_Aggrdecl**_tmp319;struct Cyc_Absyn_Aggrdecl*_tmp31A;struct Cyc_List_List*_tmp31B;struct Cyc_Absyn_Datatypefield*_tmp31D;int _tmp320;char _tmp322;struct _dyneither_ptr _tmp324;int _tmp325;struct Cyc_Absyn_Enumfield*_tmp327;struct Cyc_Absyn_Enumfield*_tmp329;struct Cyc_Absyn_Exp*_tmp32B;_LL159: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp30B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp30B->tag != 0)goto _LL15B;}_LL15A: {
# 1237
const char*_tmp5A1;return(_tmp5A1="_",_tag_dyneither(_tmp5A1,sizeof(char),2));}_LL15B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp30C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp30C->tag != 1)goto _LL15D;else{_tmp30D=_tmp30C->f1;}}_LL15C:
# 1238
 return Cyc_Absynpp_qvar2string(_tmp30D->name);_LL15D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp30E=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp30E->tag != 2)goto _LL15F;else{_tmp30F=_tmp30E->f1;}}_LL15E: {
# 1239
const char*_tmp5A5;void*_tmp5A4[1];struct Cyc_String_pa_PrintArg_struct _tmp5A3;return(struct _dyneither_ptr)((_tmp5A3.tag=0,((_tmp5A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp30F->name)),((_tmp5A4[0]=& _tmp5A3,Cyc_aprintf(((_tmp5A5="*%s",_tag_dyneither(_tmp5A5,sizeof(char),4))),_tag_dyneither(_tmp5A4,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp310=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp310->tag != 3)goto _LL161;else{_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;}}_LL160: {
# 1240
const char*_tmp5AA;void*_tmp5A9[2];struct Cyc_String_pa_PrintArg_struct _tmp5A8;struct Cyc_String_pa_PrintArg_struct _tmp5A7;return(struct _dyneither_ptr)((_tmp5A7.tag=0,((_tmp5A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp311->name),((_tmp5A8.tag=0,((_tmp5A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp312->name)),((_tmp5A9[0]=& _tmp5A8,((_tmp5A9[1]=& _tmp5A7,Cyc_aprintf(((_tmp5AA="%s<`%s>",_tag_dyneither(_tmp5AA,sizeof(char),8))),_tag_dyneither(_tmp5A9,sizeof(void*),2))))))))))))));}_LL161: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp313=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp313->tag != 4)goto _LL163;}_LL162: {
# 1242
const char*_tmp5AE;void*_tmp5AD[1];struct Cyc_String_pa_PrintArg_struct _tmp5AC;return(struct _dyneither_ptr)((_tmp5AC.tag=0,((_tmp5AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp305)),((_tmp5AD[0]=& _tmp5AC,Cyc_aprintf(((_tmp5AE="$(%s)",_tag_dyneither(_tmp5AE,sizeof(char),6))),_tag_dyneither(_tmp5AD,sizeof(void*),1))))))));}_LL163: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp314=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp314->tag != 5)goto _LL165;}_LL164: {
# 1244
const char*_tmp5B2;void*_tmp5B1[1];struct Cyc_String_pa_PrintArg_struct _tmp5B0;return(struct _dyneither_ptr)((_tmp5B0.tag=0,((_tmp5B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp305)),((_tmp5B1[0]=& _tmp5B0,Cyc_aprintf(((_tmp5B2="&%s",_tag_dyneither(_tmp5B2,sizeof(char),4))),_tag_dyneither(_tmp5B1,sizeof(void*),1))))))));}_LL165: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp315=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp315->tag != 6)goto _LL167;else{_tmp316=_tmp315->f1;if(_tmp316 == 0)goto _LL167;_tmp317=*_tmp316;_tmp318=_tmp317.aggr_info;if((_tmp318.KnownAggr).tag != 2)goto _LL167;_tmp319=(struct Cyc_Absyn_Aggrdecl**)(_tmp318.KnownAggr).val;_tmp31A=*_tmp319;_tmp31B=_tmp315->f3;}}_LL166:
# 1246
 if(_tmp31A->kind == Cyc_Absyn_UnionA){
# 1247
struct Cyc_List_List*_tmp33A=_tmp31B;struct Cyc_List_List _tmp33B;struct _tuple15*_tmp33C;struct _tuple15 _tmp33D;struct Cyc_List_List*_tmp33E;struct Cyc_List_List _tmp33F;void*_tmp340;struct _dyneither_ptr*_tmp342;_LL17A: if(_tmp33A == 0)goto _LL17C;_tmp33B=*_tmp33A;_tmp33C=(struct _tuple15*)_tmp33B.hd;_tmp33D=*_tmp33C;_tmp33E=_tmp33D.f1;if(_tmp33E == 0)goto _LL17C;_tmp33F=*_tmp33E;_tmp340=(void*)_tmp33F.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp341=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp340;if(_tmp341->tag != 1)goto _LL17C;else{_tmp342=_tmp341->f1;}};_LL17B: {
# 1249
const char*_tmp5B8;void*_tmp5B7[3];struct Cyc_String_pa_PrintArg_struct _tmp5B6;struct Cyc_String_pa_PrintArg_struct _tmp5B5;struct Cyc_String_pa_PrintArg_struct _tmp5B4;return(struct _dyneither_ptr)((_tmp5B4.tag=0,((_tmp5B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1250
Cyc_Tcpat_descs2string(r,_tmp305)),((_tmp5B5.tag=0,((_tmp5B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp342),((_tmp5B6.tag=0,((_tmp5B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1249
Cyc_Absynpp_qvar2string(_tmp31A->name)),((_tmp5B7[0]=& _tmp5B6,((_tmp5B7[1]=& _tmp5B5,((_tmp5B7[2]=& _tmp5B4,Cyc_aprintf(((_tmp5B8="%s{.%s = %s}",_tag_dyneither(_tmp5B8,sizeof(char),13))),_tag_dyneither(_tmp5B7,sizeof(void*),3))))))))))))))))))));}_LL17C:;_LL17D:
# 1251
 goto _LL179;_LL179:;}{
# 1254
const char*_tmp5BD;void*_tmp5BC[2];struct Cyc_String_pa_PrintArg_struct _tmp5BB;struct Cyc_String_pa_PrintArg_struct _tmp5BA;return(struct _dyneither_ptr)((_tmp5BA.tag=0,((_tmp5BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp305)),((_tmp5BB.tag=0,((_tmp5BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31A->name)),((_tmp5BC[0]=& _tmp5BB,((_tmp5BC[1]=& _tmp5BA,Cyc_aprintf(((_tmp5BD="%s{%s}",_tag_dyneither(_tmp5BD,sizeof(char),7))),_tag_dyneither(_tmp5BC,sizeof(void*),2))))))))))))));};_LL167: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp31C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp31C->tag != 7)goto _LL169;else{_tmp31D=_tmp31C->f2;}}_LL168:
# 1256
 if(_tmp305 == 0){
# 1257
const char*_tmp5C1;void*_tmp5C0[1];struct Cyc_String_pa_PrintArg_struct _tmp5BF;return(struct _dyneither_ptr)((_tmp5BF.tag=0,((_tmp5BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31D->name)),((_tmp5C0[0]=& _tmp5BF,Cyc_aprintf(((_tmp5C1="%s",_tag_dyneither(_tmp5C1,sizeof(char),3))),_tag_dyneither(_tmp5C0,sizeof(void*),1))))))));}else{
# 1259
const char*_tmp5C6;void*_tmp5C5[2];struct Cyc_String_pa_PrintArg_struct _tmp5C4;struct Cyc_String_pa_PrintArg_struct _tmp5C3;return(struct _dyneither_ptr)((_tmp5C3.tag=0,((_tmp5C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp305)),((_tmp5C4.tag=0,((_tmp5C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp31D->name)),((_tmp5C5[0]=& _tmp5C4,((_tmp5C5[1]=& _tmp5C3,Cyc_aprintf(((_tmp5C6="%s(%s)",_tag_dyneither(_tmp5C6,sizeof(char),7))),_tag_dyneither(_tmp5C5,sizeof(void*),2))))))))))))));}_LL169: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp31E=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp31E->tag != 8)goto _LL16B;}_LL16A: {
# 1260
const char*_tmp5C7;return(_tmp5C7="NULL",_tag_dyneither(_tmp5C7,sizeof(char),5));}_LL16B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp31F=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp31F->tag != 9)goto _LL16D;else{_tmp320=_tmp31F->f2;}}_LL16C: {
# 1261
const char*_tmp5CB;void*_tmp5CA[1];struct Cyc_Int_pa_PrintArg_struct _tmp5C9;return(struct _dyneither_ptr)((_tmp5C9.tag=1,((_tmp5C9.f1=(unsigned long)_tmp320,((_tmp5CA[0]=& _tmp5C9,Cyc_aprintf(((_tmp5CB="%d",_tag_dyneither(_tmp5CB,sizeof(char),3))),_tag_dyneither(_tmp5CA,sizeof(void*),1))))))));}_LL16D: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp321=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp321->tag != 10)goto _LL16F;else{_tmp322=_tmp321->f1;}}_LL16E: {
# 1262
const char*_tmp5CF;void*_tmp5CE[1];struct Cyc_Int_pa_PrintArg_struct _tmp5CD;return(struct _dyneither_ptr)((_tmp5CD.tag=1,((_tmp5CD.f1=(unsigned long)((int)_tmp322),((_tmp5CE[0]=& _tmp5CD,Cyc_aprintf(((_tmp5CF="%d",_tag_dyneither(_tmp5CF,sizeof(char),3))),_tag_dyneither(_tmp5CE,sizeof(void*),1))))))));}_LL16F: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp323=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp323->tag != 11)goto _LL171;else{_tmp324=_tmp323->f1;_tmp325=_tmp323->f2;}}_LL170:
# 1263
 return _tmp324;_LL171: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp326=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp326->tag != 12)goto _LL173;else{_tmp327=_tmp326->f2;}}_LL172:
# 1264
 _tmp329=_tmp327;goto _LL174;_LL173: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp328=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp328->tag != 13)goto _LL175;else{_tmp329=_tmp328->f2;}}_LL174:
# 1265
 return Cyc_Absynpp_qvar2string(_tmp329->name);_LL175: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp32A=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp30A;if(_tmp32A->tag != 16)goto _LL177;else{_tmp32B=_tmp32A->f1;}}_LL176:
# 1266
 return Cyc_Absynpp_exp2string(_tmp32B);_LL177:;_LL178:
# 1267
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL158:;};}_LL156: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp306=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp302;if(_tmp306->tag != 1)goto _LL153;else{_tmp307=_tmp306->f1;}}_LL157:
# 1270
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp307)){const char*_tmp5D0;return(_tmp5D0="_",_tag_dyneither(_tmp5D0,sizeof(char),2));}{
# 1271
struct Cyc_Tcpat_Con_s*_tmp35B=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp307);
# 1272
struct Cyc_Absyn_Pat*_tmp35C=_tmp35B->orig_pat;
# 1275
if(_tmp35C == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
# 1276
void*_tmp35D=Cyc_Tcutil_compress((void*)_check_null(_tmp35C->topt));enum Cyc_Absyn_Size_of _tmp35F;struct Cyc_Absyn_PtrInfo _tmp362;struct Cyc_Absyn_PtrAtts _tmp363;struct Cyc_Absyn_DatatypeInfo _tmp365;union Cyc_Absyn_DatatypeInfoU _tmp366;struct Cyc_Absyn_Datatypedecl**_tmp367;struct Cyc_Absyn_Datatypedecl*_tmp368;struct Cyc_Absyn_AggrInfo _tmp36A;union Cyc_Absyn_AggrInfoU _tmp36B;struct Cyc_Absyn_Aggrdecl**_tmp36C;struct Cyc_Absyn_Aggrdecl*_tmp36D;_LL17F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 6)goto _LL181;else{_tmp35F=_tmp35E->f2;if(_tmp35F != Cyc_Absyn_Char_sz)goto _LL181;}}_LL180:
# 1279
{int i=0;for(0;i < 256;++ i){
# 1280
struct Cyc_Tcpat_Con_s*_tmp36E=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)_tmp35C);
# 1281
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp307,_tmp36E)){
# 1282
const char*_tmp5D4;void*_tmp5D3[1];struct Cyc_Int_pa_PrintArg_struct _tmp5D2;return(struct _dyneither_ptr)((_tmp5D2.tag=1,((_tmp5D2.f1=(unsigned long)i,((_tmp5D3[0]=& _tmp5D2,Cyc_aprintf(((_tmp5D4="%d",_tag_dyneither(_tmp5D4,sizeof(char),3))),_tag_dyneither(_tmp5D3,sizeof(void*),1))))))));}}}
# 1284
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL181: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp35D;if(_tmp360->tag != 6)goto _LL183;}_LL182:
# 1287
{unsigned int i=0;for(0;i < 0 - 1;++ i){
# 1288
struct Cyc_Tcpat_Con_s*_tmp372=Cyc_Tcpat_int_con(r,(int)i,_tmp35C);
# 1289
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp307,_tmp372)){
# 1290
const char*_tmp5D8;void*_tmp5D7[1];struct Cyc_Int_pa_PrintArg_struct _tmp5D6;return(struct _dyneither_ptr)((_tmp5D6.tag=1,((_tmp5D6.f1=(unsigned long)((int)i),((_tmp5D7[0]=& _tmp5D6,Cyc_aprintf(((_tmp5D8="%d",_tag_dyneither(_tmp5D8,sizeof(char),3))),_tag_dyneither(_tmp5D7,sizeof(void*),1))))))));}}}
# 1292
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL183: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D;if(_tmp361->tag != 5)goto _LL185;else{_tmp362=_tmp361->f1;_tmp363=_tmp362.ptr_atts;}}_LL184: {
# 1294
union Cyc_Absyn_Constraint*_tmp376=_tmp363.nullable;
# 1295
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp376);
# 1296
if(is_nullable){
# 1297
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp307,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp35C))){
# 1298
const char*_tmp5D9;return(_tmp5D9="NULL",_tag_dyneither(_tmp5D9,sizeof(char),5));}}{
# 1300
const char*_tmp5DA;return(_tmp5DA="&_",_tag_dyneither(_tmp5DA,sizeof(char),3));};}_LL185: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp364=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35D;if(_tmp364->tag != 3)goto _LL187;else{_tmp365=_tmp364->f1;_tmp366=_tmp365.datatype_info;if((_tmp366.KnownDatatype).tag != 2)goto _LL187;_tmp367=(struct Cyc_Absyn_Datatypedecl**)(_tmp366.KnownDatatype).val;_tmp368=*_tmp367;}}_LL186:
# 1302
 if(_tmp368->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
# 1303
struct Cyc_List_List*_tmp379=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp368->fields))->v;
# 1304
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp379);
# 1305
for(0;(unsigned int)_tmp379;_tmp379=_tmp379->tl){
# 1306
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp379->hd)->name).f2;
# 1307
struct Cyc_List_List*_tmp37A=((struct Cyc_Absyn_Datatypefield*)_tmp379->hd)->typs;
# 1308
struct Cyc_Tcpat_Con_s*_tmp5DB;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp307,(struct Cyc_Tcpat_Con_s*)((_tmp5DB=_cycalloc(sizeof(*_tmp5DB)),((_tmp5DB->name=Cyc_Tcpat_Name_v(n),((_tmp5DB->arity=0,((_tmp5DB->span=0,((_tmp5DB->orig_pat=_tmp35C,_tmp5DB)))))))))))){
# 1309
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37A)== 0)
# 1310
return n;else{
# 1312
const char*_tmp5DF;void*_tmp5DE[1];struct Cyc_String_pa_PrintArg_struct _tmp5DD;return(struct _dyneither_ptr)((_tmp5DD.tag=0,((_tmp5DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5DE[0]=& _tmp5DD,Cyc_aprintf(((_tmp5DF="%s(...)",_tag_dyneither(_tmp5DF,sizeof(char),8))),_tag_dyneither(_tmp5DE,sizeof(void*),1))))))));}}}
# 1315
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL187:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp35D;if(_tmp369->tag != 11)goto _LL189;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp36A.aggr_info;if((_tmp36B.KnownAggr).tag != 2)goto _LL189;_tmp36C=(struct Cyc_Absyn_Aggrdecl**)(_tmp36B.KnownAggr).val;_tmp36D=*_tmp36C;}}if(!(_tmp36D->kind == Cyc_Absyn_UnionA))goto _LL189;_LL188: {
# 1317
struct Cyc_List_List*_tmp37F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp36D->impl))->fields;
# 1318
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37F);
# 1319
struct _tuple1 _tmp381;struct _dyneither_ptr*_tmp382;struct _dyneither_ptr _tmp383;struct _tuple1*_tmp380=_tmp36D->name;_tmp381=*_tmp380;_tmp382=_tmp381.f2;_tmp383=*_tmp382;
# 1320
for(0;(unsigned int)_tmp37F;_tmp37F=_tmp37F->tl){
# 1321
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp37F->hd)->name;
# 1322
struct Cyc_Tcpat_Con_s*_tmp5E0;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp307,(struct Cyc_Tcpat_Con_s*)((_tmp5E0=_cycalloc(sizeof(*_tmp5E0)),((_tmp5E0->name=Cyc_Tcpat_Name_v(n),((_tmp5E0->arity=0,((_tmp5E0->span=0,((_tmp5E0->orig_pat=_tmp35C,_tmp5E0)))))))))))){
# 1323
const char*_tmp5E5;void*_tmp5E4[2];struct Cyc_String_pa_PrintArg_struct _tmp5E3;struct Cyc_String_pa_PrintArg_struct _tmp5E2;return(struct _dyneither_ptr)((_tmp5E2.tag=0,((_tmp5E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp5E3.tag=0,((_tmp5E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp383),((_tmp5E4[0]=& _tmp5E3,((_tmp5E4[1]=& _tmp5E2,Cyc_aprintf(((_tmp5E5="%s{.%s = _}",_tag_dyneither(_tmp5E5,sizeof(char),12))),_tag_dyneither(_tmp5E4,sizeof(void*),2))))))))))))));}}
# 1325
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL189:;_LL18A:
# 1326
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL17E:;};};_LL153:;}
# 1330
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
# 1331
struct _dyneither_ptr*_tmp5E6;return(_tmp5E6=_cycalloc(sizeof(*_tmp5E6)),((_tmp5E6[0]=Cyc_Tcpat_desc2string(r,d),_tmp5E6)));}
# 1333
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
# 1334
struct Cyc_List_List*_tmp38A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
# 1335
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
# 1336
{struct Cyc_List_List*_tmp38B=_tmp38A;for(0;_tmp38B != 0;_tmp38B=((struct Cyc_List_List*)_check_null(_tmp38B))->tl){
# 1337
if(_tmp38B->tl != 0){
# 1338
{struct Cyc_List_List*_tmp5E7;_tmp38B->tl=(struct Cyc_List_List*)((_tmp5E7=_cycalloc(sizeof(*_tmp5E7)),((_tmp5E7->hd=comma,((_tmp5E7->tl=_tmp38B->tl,_tmp5E7))))));}
# 1339
_tmp38B=_tmp38B->tl;}}}
# 1342
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp38A);}
# 1345
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
# 1346
struct _handler_cons _tmp38E;_push_handler(& _tmp38E);{int _tmp390=0;if(setjmp(_tmp38E.handler))_tmp390=1;if(!_tmp390){
# 1347
{struct _dyneither_ptr _tmp391=Cyc_Tcpat_desc2string(r,desc);
# 1348
const char*_tmp5EB;void*_tmp5EA[1];struct Cyc_String_pa_PrintArg_struct _tmp5E9;(_tmp5E9.tag=0,((_tmp5E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp391),((_tmp5EA[0]=& _tmp5E9,Cyc_Tcutil_terr(loc,((_tmp5EB="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5EB,sizeof(char),53))),_tag_dyneither(_tmp5EA,sizeof(void*),1)))))));}
# 1347
;_pop_handler();}else{void*_tmp38F=(void*)_exn_thrown;void*_tmp396=_tmp38F;_LL18C: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp397=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp396;if(_tmp397->tag != Cyc_Tcpat_Desc2string)goto _LL18E;}_LL18D:
# 1351
{const char*_tmp5EE;void*_tmp5ED;(_tmp5ED=0,Cyc_Tcutil_terr(loc,((_tmp5EE="patterns may not be exhaustive.",_tag_dyneither(_tmp5EE,sizeof(char),32))),_tag_dyneither(_tmp5ED,sizeof(void*),0)));}
# 1352
goto _LL18B;_LL18E:;_LL18F:(void)_throw(_tmp396);_LL18B:;}};}
# 1355
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple19*rhs){
# 1356
(*rhs).f1=1;}
# 1359
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1364
struct _RegionHandle _tmp39A=_new_region("r");struct _RegionHandle*r=& _tmp39A;_push_region(r);
# 1365
{int _tmp39B=0;
# 1366
struct _tuple23 _tmp5EF;struct _tuple23 _tmp39C=(_tmp5EF.f1=r,((_tmp5EF.f2=& _tmp39B,_tmp5EF)));
# 1367
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp39C,swcs);
# 1368
void*_tmp39E=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp39D);
# 1369
int _tmp39F=0;
# 1370
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp39E,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp39F);
# 1372
for(0;_tmp39D != 0;_tmp39D=_tmp39D->tl){
# 1373
struct _tuple22 _tmp3A1;struct _tuple19*_tmp3A2;struct _tuple19 _tmp3A3;int _tmp3A4;unsigned int _tmp3A5;struct _tuple22*_tmp3A0=(struct _tuple22*)_tmp39D->hd;_tmp3A1=*_tmp3A0;_tmp3A2=_tmp3A1.f2;_tmp3A3=*_tmp3A2;_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;
# 1374
if(!_tmp3A4){
# 1375
{const char*_tmp5F2;void*_tmp5F1;(_tmp5F1=0,Cyc_Tcutil_terr(_tmp3A5,((_tmp5F2="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5F2,sizeof(char),74))),_tag_dyneither(_tmp5F1,sizeof(void*),0)));}
# 1377
break;}}}
# 1365
;_pop_region(r);}
# 1387
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
# 1388
(*pr).f2=0;{
# 1389
struct _handler_cons _tmp3A9;_push_handler(& _tmp3A9);{int _tmp3AB=0;if(setjmp(_tmp3A9.handler))_tmp3AB=1;if(!_tmp3AB){
# 1390
{struct _dyneither_ptr _tmp3AC=Cyc_Tcpat_desc2string(r,desc);
# 1391
const char*_tmp5F6;void*_tmp5F5[1];struct Cyc_String_pa_PrintArg_struct _tmp5F4;(_tmp5F4.tag=0,((_tmp5F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AC),((_tmp5F5[0]=& _tmp5F4,Cyc_Tcutil_warn((*pr).f1,((_tmp5F6="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5F6,sizeof(char),45))),_tag_dyneither(_tmp5F5,sizeof(void*),1)))))));}
# 1390
;_pop_handler();}else{void*_tmp3AA=(void*)_exn_thrown;void*_tmp3B1=_tmp3AA;_LL191: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3B2=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3B1;if(_tmp3B2->tag != Cyc_Tcpat_Desc2string)goto _LL193;}_LL192:
# 1394
{const char*_tmp5F9;void*_tmp5F8;(_tmp5F8=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5F9="pattern not exhaustive.",_tag_dyneither(_tmp5F9,sizeof(char),24))),_tag_dyneither(_tmp5F8,sizeof(void*),0)));}
# 1395
goto _LL190;_LL193:;_LL194:(void)_throw(_tmp3B1);_LL190:;}};};}
# 1398
static void Cyc_Tcpat_dummy_fn(int i,int j){
# 1399
return;}struct _tuple24{void*f1;int f2;};
# 1401
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
# 1402
struct _RegionHandle _tmp3B5=_new_region("r");struct _RegionHandle*r=& _tmp3B5;_push_region(r);
# 1403
{struct _tuple24*_tmp5FC;struct Cyc_List_List*_tmp5FB;struct Cyc_List_List*_tmp3B6=(_tmp5FB=_region_malloc(r,sizeof(*_tmp5FB)),((_tmp5FB->hd=((_tmp5FC=_region_malloc(r,sizeof(*_tmp5FC)),((_tmp5FC->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp5FC->f2=0,_tmp5FC)))))),((_tmp5FB->tl=0,_tmp5FB)))));
# 1404
void*_tmp3B7=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3B6);
# 1405
struct _tuple14 _tmp5FD;struct _tuple14 _tmp3B8=(_tmp5FD.f1=loc,((_tmp5FD.f2=1,_tmp5FD)));
# 1406
int _tmp3B9=0;
# 1407
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3B7,Cyc_Tcpat_not_exhaust_warn,& _tmp3B8,Cyc_Tcpat_dummy_fn,0,& _tmp3B9);{
# 1409
int _tmp3BA=_tmp3B8.f2;_npop_handler(0);return _tmp3BA;};}
# 1403
;_pop_region(r);}
# 1417
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1419
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
# 1420
struct _tuple19*_tmp5FE;struct _tuple19*rhs=(_tmp5FE=_region_malloc(r,sizeof(*_tmp5FE)),((_tmp5FE->f1=0,((_tmp5FE->f2=(swc->pattern)->loc,_tmp5FE)))));
# 1421
struct _tuple22*_tmp5FF;return(_tmp5FF=_region_malloc(r,sizeof(*_tmp5FF)),((_tmp5FF->f1=sp0,((_tmp5FF->f2=rhs,_tmp5FF)))));}
# 1423
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1426
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1428
struct _RegionHandle _tmp3C0=_new_region("r");struct _RegionHandle*r=& _tmp3C0;_push_region(r);
# 1429
{struct Cyc_List_List*_tmp3C1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
# 1430
void*_tmp3C2=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3C1);
# 1431
int _tmp3C3=0;
# 1432
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3C2,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3C3);
# 1434
for(0;_tmp3C1 != 0;_tmp3C1=_tmp3C1->tl){
# 1435
struct _tuple22 _tmp3C5;struct _tuple19*_tmp3C6;struct _tuple19 _tmp3C7;int _tmp3C8;unsigned int _tmp3C9;struct _tuple22*_tmp3C4=(struct _tuple22*)_tmp3C1->hd;_tmp3C5=*_tmp3C4;_tmp3C6=_tmp3C5.f2;_tmp3C7=*_tmp3C6;_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;
# 1436
if(!_tmp3C8){
# 1437
{const char*_tmp602;void*_tmp601;(_tmp601=0,Cyc_Tcutil_terr(_tmp3C9,((_tmp602="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp602,sizeof(char),74))),_tag_dyneither(_tmp601,sizeof(void*),0)));}
# 1438
break;}}}
# 1429
;_pop_region(r);}
