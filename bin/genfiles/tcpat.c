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
# 38 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 44
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
{const char*_tmp372;void*_tmp371;(_tmp371=0,Cyc_Tcutil_terr(p->loc,((_tmp372="struct tag used without arguments in pattern",_tag_dyneither(_tmp372,sizeof(char),45))),_tag_dyneither(_tmp371,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp375;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp374;p->r=(void*)((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374[0]=((_tmp375.tag=7,((_tmp375.f1=_tmp1B,((_tmp375.f2=_tmp1C,((_tmp375.f3=0,((_tmp375.f4=0,_tmp375)))))))))),_tmp374))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp378;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp377;p->r=(void*)((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377[0]=((_tmp378.tag=12,((_tmp378.f1=_tmp1E,((_tmp378.f2=_tmp1F,_tmp378)))))),_tmp377))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp37B;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp37A;p->r=(void*)((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((_tmp37A[0]=((_tmp37B.tag=13,((_tmp37B.f1=_tmp21,((_tmp37B.f2=_tmp22,_tmp37B)))))),_tmp37A))));}
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
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp37E;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp37D;p->r=(void*)((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D[0]=((_tmp37E.tag=1,((_tmp37E.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp37E.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp37E)))))),_tmp37D))));}
return;_LL22:;_LL23:
{const char*_tmp381;void*_tmp380;(_tmp380=0,Cyc_Tcutil_terr(p->loc,((_tmp381="qualified variable in pattern",_tag_dyneither(_tmp381,sizeof(char),30))),_tag_dyneither(_tmp380,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 85
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_stmttmp2=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);void*_tmp39=_stmttmp2;struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp384;struct Cyc_List_List*_tmp383;_tmp42=((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383->hd=((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384->f1=0,((_tmp384->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp384)))))),((_tmp383->tl=_tmp42,_tmp383))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp38E;struct Cyc_Absyn_Aggrdecl**_tmp38D;struct Cyc_Absyn_AggrInfo*_tmp38C;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp38B;p->r=(void*)((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=((_tmp38E.tag=6,((_tmp38E.f1=((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C->aggr_info=Cyc_Absyn_KnownAggr(((_tmp38D=_cycalloc(sizeof(*_tmp38D)),((_tmp38D[0]=_tmp3B,_tmp38D))))),((_tmp38C->targs=0,_tmp38C)))))),((_tmp38E.f2=0,((_tmp38E.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp38E.f4=_tmp6,_tmp38E)))))))))),_tmp38B))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp391;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp390;p->r=(void*)((_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390[0]=((_tmp391.tag=7,((_tmp391.f1=_tmp3D,((_tmp391.f2=_tmp3E,((_tmp391.f3=_tmp5,((_tmp391.f4=_tmp6,_tmp391)))))))))),_tmp390))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp394;void*_tmp393;(_tmp393=0,Cyc_Tcutil_terr(p->loc,((_tmp394="enum tag used with arguments in pattern",_tag_dyneither(_tmp394,sizeof(char),40))),_tag_dyneither(_tmp393,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp398;void*_tmp397[1];struct Cyc_String_pa_PrintArg_struct _tmp396;(_tmp396.tag=0,((_tmp396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp397[0]=& _tmp396,Cyc_Tcutil_terr(p->loc,((_tmp398="%s is not a constructor in pattern",_tag_dyneither(_tmp398,sizeof(char),35))),_tag_dyneither(_tmp397,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp39B;void*_tmp39A;(_tmp39A=0,Cyc_Tcutil_terr(p->loc,((_tmp39B="cannot determine pattern type",_tag_dyneither(_tmp39B,sizeof(char),30))),_tag_dyneither(_tmp39A,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3A1;struct Cyc_Absyn_AggrInfo*_tmp3A0;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp39F;p->r=(void*)((_tmp39F=_cycalloc(sizeof(*_tmp39F)),((_tmp39F[0]=((_tmp3A1.tag=6,((_tmp3A1.f1=((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0[0]=_tmp58,_tmp3A0)))),((_tmp3A1.f2=_tmp8,((_tmp3A1.f3=_tmp9,((_tmp3A1.f4=_tmpA,_tmp3A1)))))))))),_tmp39F))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp3A5;void*_tmp3A4[1];struct Cyc_String_pa_PrintArg_struct _tmp3A3;(_tmp3A3.tag=0,((_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3A4[0]=& _tmp3A3,Cyc_Tcutil_terr(p->loc,((_tmp3A5="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3A5,sizeof(char),45))),_tag_dyneither(_tmp3A4,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 126
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,((struct Cyc_Absyn_Pat*)_check_null(p))->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*((struct Cyc_Absyn_Aggrdecl**)_check_null(_tmp62));
if(_tmp63->impl == 0){
{const char*_tmp3AD;void*_tmp3AC[1];const char*_tmp3AB;const char*_tmp3AA;struct Cyc_String_pa_PrintArg_struct _tmp3A9;(_tmp3A9.tag=0,((_tmp3A9.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp3AA="struct",_tag_dyneither(_tmp3AA,sizeof(char),7)):((_tmp3AB="union",_tag_dyneither(_tmp3AB,sizeof(char),6)))),((_tmp3AC[0]=& _tmp3A9,Cyc_Tcutil_terr(p->loc,((_tmp3AD="can't destructure an abstract %s",_tag_dyneither(_tmp3AD,sizeof(char),33))),_tag_dyneither(_tmp3AC,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp3B0;void*_tmp3AF;(_tmp3AF=0,Cyc_Tcutil_terr(p->loc,((_tmp3B0="too many existentially bound type variables in pattern",_tag_dyneither(_tmp3B0,sizeof(char),55))),_tag_dyneither(_tmp3AF,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3B6;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3B5;struct Cyc_List_List*_tmp3B4;_tmp6C=((_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6[0]=((_tmp3B5.tag=1,((_tmp3B5.f1=0,_tmp3B5)))),_tmp3B6))))),((_tmp3B4->tl=_tmp6C,_tmp3B4))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3BC;struct Cyc_Absyn_AggrInfo*_tmp3BB;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3BA;p->r=(void*)((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA[0]=((_tmp3BC.tag=6,((_tmp3BC.f1=((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3BB->targs=_tmpE,_tmp3BB)))))),((_tmp3BC.f2=*_tmpF,((_tmp3BC.f3=_tmp10,((_tmp3BC.f4=_tmp11,_tmp3BC)))))))))),_tmp3BA))));};};}
# 127
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3BF;void*_tmp3BE;(_tmp3BE=0,Cyc_Tcutil_terr(p->loc,((_tmp3BF="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3BF,sizeof(char),43))),_tag_dyneither(_tmp3BE,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3C2;void*_tmp3C1;(_tmp3C1=0,Cyc_Tcutil_terr(p->loc,((_tmp3C2="non-constant expression in case pattern",_tag_dyneither(_tmp3C2,sizeof(char),40))),_tag_dyneither(_tmp3C1,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _stmttmp1=Cyc_Evexp_eval_const_uint_exp(_tmp13);unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=_stmttmp1;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3C5;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3C4;p->r=(void*)((_tmp3C4=_cycalloc_atomic(sizeof(*_tmp3C4)),((_tmp3C4[0]=((_tmp3C5.tag=9,((_tmp3C5.f1=Cyc_Absyn_None,((_tmp3C5.f2=(int)_tmp7C,_tmp3C5)))))),_tmp3C4))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3C6;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6->v=s,_tmp3C6)))));};}
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
struct _tuple16*_tmp3C9;struct Cyc_List_List*_tmp3C8;*v_result_ptr=((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8->hd=((_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9->f1=vd,((_tmp3C9->f2=e,_tmp3C9)))))),((_tmp3C8->tl=*v_result_ptr,_tmp3C8))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Tcpat_TcPatResult _tmp86=res1;_tmp87=_tmp86.tvars_and_bounds_opt;_tmp88=_tmp86.patvars;{
struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res2;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;
if(_tmp87 != 0  || _tmp8A != 0){
if(_tmp87 == 0){
struct _tuple1*_tmp3CA;_tmp87=((_tmp3CA=_cycalloc(sizeof(*_tmp3CA)),((_tmp3CA->f1=0,((_tmp3CA->f2=0,_tmp3CA))))));}
if(_tmp8A == 0){
struct _tuple1*_tmp3CB;_tmp8A=((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB->f1=0,((_tmp3CB->f2=0,_tmp3CB))))));}{
struct _tuple1*_tmp3CE;struct Cyc_Tcpat_TcPatResult _tmp3CD;return(_tmp3CD.tvars_and_bounds_opt=((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f1,(*_tmp8A).f1),((_tmp3CE->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f2,(*_tmp8A).f2),_tmp3CE)))))),((_tmp3CD.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3CD)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp3CF;return(_tmp3CF.tvars_and_bounds_opt=0,((_tmp3CF.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3CF)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3D0;return(_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->loc=loc,((_tmp3D0->topt=0,((_tmp3D0->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3D0)))))));}
# 221
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_stmttmp6=Cyc_Tcutil_compress(t);void*_tmp92=_stmttmp6;void*_tmp94;struct Cyc_Absyn_Tqual _tmp95;struct Cyc_Absyn_Exp*_tmp96;union Cyc_Absyn_Constraint*_tmp97;unsigned int _tmp98;_LL46: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92;if(_tmp93->tag != 8)goto _LL48;else{_tmp94=(_tmp93->f1).elt_type;_tmp95=(_tmp93->f1).tq;_tmp96=(_tmp93->f1).num_elts;_tmp97=(_tmp93->f1).zero_term;_tmp98=(_tmp93->f1).zt_loc;}}_LL47: {
# 224
struct Cyc_Core_Opt*_tmp3D1;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3D1))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3D4;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3D3;return Cyc_Absyn_atb_typ(_tmp94,rgn,_tmp95,(void*)((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3[0]=((_tmp3D4.tag=1,((_tmp3D4.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp96),_tmp3D4)))),_tmp3D3)))),_tmp97);}_LL48:;_LL49:
 return t;_LL45:;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 232
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 235
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3D5;struct Cyc_Tcpat_TcPatResult res=(_tmp3D5.tvars_and_bounds_opt=0,((_tmp3D5.patvars=0,_tmp3D5)));
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
const char*_tmp3D8;void*_tmp3D7;(_tmp3D7=0,Cyc_Tcutil_terr(p->loc,((_tmp3D8="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3D8,sizeof(char),58))),_tag_dyneither(_tmp3D7,sizeof(void*),0)));}
goto _LL75;_LL78:;_LL79:
# 259
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3DB;void*_tmp3DA;(_tmp3DA=0,Cyc_Tcutil_terr(p->loc,((_tmp3DB="pattern would point to an abstract member",_tag_dyneither(_tmp3DB,sizeof(char),42))),_tag_dyneither(_tmp3DA,sizeof(void*),0)));}
goto _LL75;_LL75:;}
# 263
{struct Cyc_Absyn_Vardecl**_tmp3DC;Cyc_Tcpat_set_vd(((_tmp3DC=_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC[0]=_tmp9F,_tmp3DC)))),access_exp,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));}
goto _LL4A;_LL4F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA1->tag != 2)goto _LL51;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;}}_LL50:
# 266
 res=Cyc_Tcpat_tcPatRec(te,_tmpA3,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA3->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3DF;void*_tmp3DE;(_tmp3DE=0,Cyc_Tcutil_terr(p->loc,((_tmp3DF="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3DF,sizeof(char),55))),_tag_dyneither(_tmp3DE,sizeof(void*),0)));}
goto _LL4A;}else{
# 273
struct _RegionHandle _tmpD0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD0;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_Tcutil_terr(p->loc,((_tmp3E2="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3E2,sizeof(char),56))),_tag_dyneither(_tmp3E1,sizeof(void*),0)));}
# 274
;_pop_region(rgn);}{
# 279
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3E8;struct Cyc_Absyn_PtrInfo _tmp3E7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E6;void*t2=(void*)((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E8.tag=5,((_tmp3E8.f1=((_tmp3E7.elt_typ=t,((_tmp3E7.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3E7.ptr_atts=(
((_tmp3E7.ptr_atts).rgn=rgn_pat,(((_tmp3E7.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3E7.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3E7.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3E7.ptr_atts).ptrloc=0,_tmp3E7.ptr_atts)))))))))),_tmp3E7)))))),_tmp3E8)))),_tmp3E6))));
# 284
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 288
{struct Cyc_Absyn_Vardecl**_tmp3E9;Cyc_Tcpat_set_vd(((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=_tmpA2,_tmp3E9)))),new_access_exp,& res.patvars,t2);}
goto _LL4A;};_LL51: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA4->tag != 3)goto _LL53;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;}}_LL52:
# 294
{struct Cyc_Absyn_Vardecl**_tmp3EA;Cyc_Tcpat_set_vd(((_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA[0]=_tmpA6,_tmp3EA)))),access_exp,& res.patvars,_tmpA6->type);}{
# 298
struct _RegionHandle _tmpD8=_new_region("r2");struct _RegionHandle*r2=& _tmpD8;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3EB[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3EB[0]=_tmpA5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EB,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 301
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp3EC;res.tvars_and_bounds_opt=((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC->f1=0,((_tmp3EC->f2=0,_tmp3EC))))));}
{struct Cyc_List_List*_tmp3ED;(*res.tvars_and_bounds_opt).f1=(
(_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=_tmpA5,((_tmp3ED->tl=(*res.tvars_and_bounds_opt).f1,_tmp3ED))))));}
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
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3F0;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3EF;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3F0.tag=13,((_tmp3F0.f1=_tmpAE->name,((_tmp3F0.f2=_tmpAE,_tmp3F0)))))),_tmp3EF)))));}
goto _LL4A;_LL5F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAF->tag != 13)goto _LL61;else{_tmpB0=(void*)_tmpAF->f1;}}_LL60:
 t=Cyc_Tcpat_num_type(topt,_tmpB0);goto _LL4A;_LL61: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB1->tag != 8)goto _LL63;}_LL62:
# 318
 if(topt != 0){
void*_stmttmpD=Cyc_Tcutil_compress(*topt);void*_tmpDE=_stmttmpD;_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDE;if(_tmpDF->tag != 5)goto _LL7D;}_LL7C:
 t=*topt;goto tcpat_end;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 323
struct Cyc_Core_Opt*_tmpE0=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F4;t=(void*)((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=((_tmp3F6.tag=5,((_tmp3F6.f1=((_tmp3F5.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpE0),((_tmp3F5.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3F5.ptr_atts=(
((_tmp3F5.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpE0),(((_tmp3F5.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp3F5.ptr_atts).bounds=
# 328
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F5.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F5.ptr_atts).ptrloc=0,_tmp3F5.ptr_atts)))))))))),_tmp3F5)))))),_tmp3F6)))),_tmp3F4))));}
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
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp400;struct Cyc_Absyn_Datatypedecl**_tmp3FF;struct Cyc_Absyn_DatatypeInfo _tmp3FE;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FD;void*new_type=(void*)((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=((_tmp400.tag=3,((_tmp400.f1=((_tmp3FE.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF[0]=_tmpEB,_tmp3FF))))),((_tmp3FE.targs=_tmpED,_tmp3FE)))),_tmp400)))),_tmp3FD))));
# 373
_tmpB3->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp406;struct Cyc_Absyn_PtrInfo _tmp405;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp404;t=(void*)((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404[0]=((_tmp406.tag=5,((_tmp406.f1=((_tmp405.elt_typ=new_type,((_tmp405.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp405.ptr_atts=(
((_tmp405.ptr_atts).rgn=ptr_rgn,(((_tmp405.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp405.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp405.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp405.ptr_atts).ptrloc=0,_tmp405.ptr_atts)))))))))),_tmp405)))))),_tmp406)))),_tmp404))));}
# 380
goto _LL84;};_LL87:;_LL88:
# 382
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp40C;struct Cyc_Absyn_PtrInfo _tmp40B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40A;t=(void*)((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A[0]=((_tmp40C.tag=5,((_tmp40C.f1=((_tmp40B.elt_typ=(void*)_check_null(_tmpB3->topt),((_tmp40B.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp40B.ptr_atts=(
((_tmp40B.ptr_atts).rgn=ptr_rgn,(((_tmp40B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40B.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40B.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp40B.ptr_atts).ptrloc=0,_tmp40B.ptr_atts)))))))))),_tmp40B)))))),_tmp40C)))),_tmp40A))));}_LL84:;}
# 390
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpB3->topt;
goto _LL4A;};}_LL65: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB4->tag != 4)goto _LL67;else{_tmpB5=(struct Cyc_List_List**)& _tmpB4->f1;_tmpB6=_tmpB4->f2;}}_LL66: {
# 394
struct Cyc_List_List*_tmpFA=*((struct Cyc_List_List**)_check_null(_tmpB5));
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
struct Cyc_List_List*_tmp40D;wild_ps=((_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp40D->tl=wild_ps,_tmp40D))))));}}
*_tmpB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpFA,wild_ps);
_tmpFA=*_tmpB5;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFA)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD)){
const char*_tmp410;void*_tmp40F;(_tmp40F=0,Cyc_Tcutil_warn(p->loc,((_tmp410="unnecessary ... in tuple pattern",_tag_dyneither(_tmp410,sizeof(char),33))),_tag_dyneither(_tmp40F,sizeof(void*),0)));}}}
# 415
goto _LL8E;_LL91:;_LL92:
# 417
 goto _LL8E;_LL8E:;}else{
# 419
if(_tmpB6){
const char*_tmp413;void*_tmp412;(_tmp412=0,Cyc_Tcutil_terr(p->loc,((_tmp413="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp413,sizeof(char),57))),_tag_dyneither(_tmp412,sizeof(void*),0)));}}
{int i=0;for(0;_tmpFA != 0;(_tmpFA=_tmpFA->tl,i ++)){
void**_tmp105=0;
if(topt_ts != 0){
_tmp105=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 427
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp416;union Cyc_Absyn_Cnst _tmp415;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp415.Int_c).val=((_tmp416.f1=Cyc_Absyn_Unsigned,((_tmp416.f2=i,_tmp416)))),(((_tmp415.Int_c).tag=5,_tmp415)))),0),0);}
# 432
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpFA->hd,_tmp105,rgn_pat,allow_ref_pat,new_access_exp));
# 435
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt;{
struct _tuple17*_tmp419;struct Cyc_List_List*_tmp418;pat_ts=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->f1=Cyc_Absyn_empty_tqual(0),((_tmp419->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt),_tmp419)))))),((_tmp418->tl=pat_ts,_tmp418))))));};};}}
# 438
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp41C;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp41B;t=(void*)((_tmp41B=_cycalloc(sizeof(*_tmp41B)),((_tmp41B[0]=((_tmp41C.tag=10,((_tmp41C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp41C)))),_tmp41B))));}
goto _LL4A;}_LL67: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB7->tag != 6)goto _LL69;else{if(_tmpB7->f1 == 0)goto _LL69;if((((_tmpB7->f1)->aggr_info).KnownAggr).tag != 2)goto _LL69;_tmpB8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB7->f1)->aggr_info).KnownAggr).val);_tmpB9=(struct Cyc_List_List**)&(_tmpB7->f1)->targs;_tmpBA=_tmpB7->f2;_tmpBB=(struct Cyc_List_List**)& _tmpB7->f3;_tmpBC=_tmpB7->f4;}}_LL68: {
# 442
struct Cyc_List_List*_tmp10C=*((struct Cyc_List_List**)_check_null(_tmpBB));
const char*_tmp41E;const char*_tmp41D;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(_tmp41E="struct",_tag_dyneither(_tmp41E,sizeof(char),7)):((_tmp41D="union",_tag_dyneither(_tmp41D,sizeof(char),6)));
if(_tmpB8->impl == 0){
{const char*_tmp422;void*_tmp421[1];struct Cyc_String_pa_PrintArg_struct _tmp420;(_tmp420.tag=0,((_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp421[0]=& _tmp420,Cyc_Tcutil_terr(p->loc,((_tmp422="can't destructure an abstract %s",_tag_dyneither(_tmp422,sizeof(char),33))),_tag_dyneither(_tmp421,sizeof(void*),1)))))));}
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
struct Cyc_List_List*_tmp112=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB8))->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpBA;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp112))->hd;
_tmp112=_tmp112->tl;{
void*_tmp113=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)_check_null(tv))->kind);
void*_tmp114=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)_check_null(uv))->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp115=_tmp114;struct Cyc_Absyn_Kind*_tmp117;struct Cyc_Absyn_Kind*_tmp119;_LL94: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp116->tag != 2)goto _LL96;else{_tmp117=_tmp116->f2;}}_LL95:
 _tmp119=_tmp117;goto _LL97;_LL96: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp118=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp118->tag != 0)goto _LL98;else{_tmp119=_tmp118->f1;}}_LL97:
 k2=_tmp119;goto _LL93;_LL98:;_LL99: {
const char*_tmp425;void*_tmp424;(_tmp424=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp425="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp425,sizeof(char),53))),_tag_dyneither(_tmp424,sizeof(void*),0)));}_LL93:;}
# 476
{void*_tmp11C=_tmp113;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Core_Opt**_tmp120;struct Cyc_Absyn_Kind*_tmp121;struct Cyc_Core_Opt**_tmp123;_LL9B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp11D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL9D;else{_tmp11E=_tmp11D->f1;}}_LL9C:
# 479
 if(!Cyc_Tcutil_kind_leq(k2,_tmp11E))
error=1;
goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11F->tag != 2)goto _LL9F;else{_tmp120=(struct Cyc_Core_Opt**)& _tmp11F->f1;_tmp121=_tmp11F->f2;}}_LL9E:
 _tmp123=_tmp120;goto _LLA0;_LL9F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp122=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp122->tag != 1)goto _LL9A;else{_tmp123=(struct Cyc_Core_Opt**)& _tmp122->f1;}}_LLA0:
{struct Cyc_Core_Opt*_tmp426;*_tmp123=((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426->v=_tmp114,_tmp426))));}goto _LL9A;_LL9A:;}
# 485
if(error){
const char*_tmp42C;void*_tmp42B[3];struct Cyc_String_pa_PrintArg_struct _tmp42A;struct Cyc_String_pa_PrintArg_struct _tmp429;struct Cyc_String_pa_PrintArg_struct _tmp428;(_tmp428.tag=0,((_tmp428.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 489
Cyc_Absynpp_kind2string(k2)),((_tmp429.tag=0,((_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp113)),((_tmp42A.tag=0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp42B[0]=& _tmp42A,((_tmp42B[1]=& _tmp429,((_tmp42B[2]=& _tmp428,Cyc_Tcutil_terr(p->loc,((_tmp42C="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp42C,sizeof(char),60))),_tag_dyneither(_tmp42B,sizeof(void*),3)))))))))))))))))));}{
# 491
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp42F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp42E;void*vartype=(void*)((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E[0]=((_tmp42F.tag=2,((_tmp42F.f1=tv,_tmp42F)))),_tmp42E))));
{struct Cyc_List_List*_tmp430;_tmp111=((_tmp430=_region_malloc(rgn,sizeof(*_tmp430)),((_tmp430->hd=vartype,((_tmp430->tl=_tmp111,_tmp430))))));}
# 495
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp433;struct Cyc_List_List*_tmp432;outlives_constraints=(
(_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432->hd=((_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->f1=Cyc_Absyn_empty_effect,((_tmp433->f2=vartype,_tmp433)))))),((_tmp432->tl=outlives_constraints,_tmp432))))));}else{
# 500
const char*_tmp436;void*_tmp435;(_tmp435=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp436="opened existential had unique or top region kind",_tag_dyneither(_tmp436,sizeof(char),49))),_tag_dyneither(_tmp435,sizeof(void*),0)));}}};};}}
# 504
_tmp111=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp111);{
# 506
struct _RegionHandle _tmp131=_new_region("r2");struct _RegionHandle*r2=& _tmp131;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBA);
# 509
struct Cyc_List_List*_tmp132=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp437;struct _tuple12 _tmp133=(_tmp437.f1=_tmp132,((_tmp437.f2=rgn,_tmp437)));
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp133,((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB8))->tvs);
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB8))->impl))->exist_vars,_tmp111);
struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp134);
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp135);
struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp134,_tmp135);
# 517
if(_tmpBA != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp438;res.tvars_and_bounds_opt=((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->f1=0,((_tmp438->f2=0,_tmp438))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpBA);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp13A=0;
{struct Cyc_List_List*_tmp13B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB8))->impl))->rgn_po;for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
struct _tuple0*_tmp43B;struct Cyc_List_List*_tmp43A;_tmp13A=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->hd=((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f1),((_tmp43B->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f2),_tmp43B)))))),((_tmp43A->tl=_tmp13A,_tmp43A))))));}}
# 529
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13A);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp13A);};}
# 534
*_tmpB9=_tmp136;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp445;struct Cyc_Absyn_Aggrdecl**_tmp444;struct Cyc_Absyn_AggrInfo _tmp443;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp442;t=(void*)((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442[0]=((_tmp445.tag=11,((_tmp445.f1=((_tmp443.aggr_info=Cyc_Absyn_KnownAggr(((_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=_tmpB8,_tmp444))))),((_tmp443.targs=*_tmpB9,_tmp443)))),_tmp445)))),_tmp442))));}
if(_tmpBC  && _tmpB8->kind == Cyc_Absyn_UnionA){
const char*_tmp448;void*_tmp447;(_tmp447=0,Cyc_Tcutil_warn(p->loc,((_tmp448="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp448,sizeof(char),43))),_tag_dyneither(_tmp447,sizeof(void*),0)));}else{
if(_tmpBC){
# 540
int _tmp144=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10C);
int _tmp145=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmpB8))->impl))->fields);
if(_tmp144 < _tmp145){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp145 - _tmp144;++ i){
struct _tuple15*_tmp44B;struct Cyc_List_List*_tmp44A;wild_dps=((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A->hd=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B->f1=0,((_tmp44B->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp44B)))))),((_tmp44A->tl=wild_dps,_tmp44A))))));}}
*_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10C,wild_dps);
_tmp10C=*_tmpBB;}else{
if(_tmp144 == _tmp145){
const char*_tmp44E;void*_tmp44D;(_tmp44D=0,Cyc_Tcutil_warn(p->loc,((_tmp44E="unnecessary ... in struct pattern",_tag_dyneither(_tmp44E,sizeof(char),34))),_tag_dyneither(_tmp44D,sizeof(void*),0)));}}}}{
# 551
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp10C,_tmpB8->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple18*_stmttmp8=(struct _tuple18*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp14B;struct Cyc_Absyn_Pat*_tmp14C;struct _tuple18*_tmp14A=_stmttmp8;_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;{
void*_tmp14D=Cyc_Tcutil_rsubstitute(rgn,_tmp138,((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp14B))->type);
# 557
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp14B->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp14C,& _tmp14D,rgn_pat,allow_ref_pat,new_access_exp));
# 565
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp14C->topt),_tmp14D)){
const char*_tmp455;void*_tmp454[4];struct Cyc_String_pa_PrintArg_struct _tmp453;struct Cyc_String_pa_PrintArg_struct _tmp452;struct Cyc_String_pa_PrintArg_struct _tmp451;struct Cyc_String_pa_PrintArg_struct _tmp450;(_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 568
Cyc_Absynpp_typ2string((void*)_check_null(_tmp14C->topt))),((_tmp451.tag=0,((_tmp451.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 567
Cyc_Absynpp_typ2string(_tmp14D)),((_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp453.tag=0,((_tmp453.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14B->name),((_tmp454[0]=& _tmp453,((_tmp454[1]=& _tmp452,((_tmp454[2]=& _tmp451,((_tmp454[3]=& _tmp450,Cyc_Tcutil_terr(p->loc,((_tmp455="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp455,sizeof(char),45))),_tag_dyneither(_tmp454,sizeof(void*),4)))))))))))))))))))))))));}
# 569
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp14C->topt;};}};}
# 507
;_pop_region(r2);};}
# 573
_npop_handler(0);goto _LL4A;
# 458
;_pop_region(rgn);};}_LL69: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpBD->tag != 7)goto _LL6B;else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBD->f2;_tmpC0=(struct Cyc_List_List**)& _tmpBD->f3;_tmpC1=_tmpBD->f4;}}_LL6A: {
# 576
struct Cyc_List_List*_tmp157=*((struct Cyc_List_List**)_check_null(_tmpC0));
struct _RegionHandle _tmp158=_new_region("rgn");struct _RegionHandle*rgn=& _tmp158;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBF->typs;
# 580
struct Cyc_List_List*_tmp159=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp456;struct _tuple12 _tmp15A=(_tmp456.f1=_tmp159,((_tmp456.f2=rgn,_tmp456)));
struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp15A,((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmpBE))->tvs);
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp15B);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp45C;struct Cyc_Absyn_DatatypeFieldInfo _tmp45B;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp45A;t=(void*)((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp45C.tag=4,((_tmp45C.f1=((_tmp45B.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBE,_tmpBF),((_tmp45B.targs=_tmp15C,_tmp45B)))),_tmp45C)))),_tmp45A))));}
# 586
if(_tmpC1){
# 588
int _tmp160=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp157);
int _tmp161=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp160 < _tmp161){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp161 - _tmp160;++ i){
struct Cyc_List_List*_tmp45D;wild_ps=((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp45D->tl=wild_ps,_tmp45D))))));}}
*_tmpC0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp157,wild_ps);
_tmp157=*_tmpC0;}else{
if(_tmp160 == _tmp161){
const char*_tmp461;void*_tmp460[1];struct Cyc_String_pa_PrintArg_struct _tmp45F;(_tmp45F.tag=0,((_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp460[0]=& _tmp45F,Cyc_Tcutil_warn(p->loc,((_tmp461="unnecessary ... in datatype field %s",_tag_dyneither(_tmp461,sizeof(char),37))),_tag_dyneither(_tmp460,sizeof(void*),1)))))));}}}
# 600
for(0;_tmp157 != 0  && tqts != 0;(_tmp157=_tmp157->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp166=(struct Cyc_Absyn_Pat*)((struct Cyc_List_List*)_check_null(_tmp157))->hd;
# 604
void*_tmp167=Cyc_Tcutil_rsubstitute(rgn,_tmp15B,(*((struct _tuple17*)_check_null((struct _tuple17*)((struct Cyc_List_List*)_check_null(tqts))->hd))).f2);
# 607
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp166,& _tmp167,rgn_pat,allow_ref_pat,0));
# 614
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp166->topt),_tmp167)){
const char*_tmp467;void*_tmp466[3];struct Cyc_String_pa_PrintArg_struct _tmp465;struct Cyc_String_pa_PrintArg_struct _tmp464;struct Cyc_String_pa_PrintArg_struct _tmp463;(_tmp463.tag=0,((_tmp463.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 617
Cyc_Absynpp_typ2string((void*)_check_null(_tmp166->topt))),((_tmp464.tag=0,((_tmp464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 616
Cyc_Absynpp_typ2string(_tmp167)),((_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp466[0]=& _tmp465,((_tmp466[1]=& _tmp464,((_tmp466[2]=& _tmp463,Cyc_Tcutil_terr(_tmp166->loc,((_tmp467="%s expects argument type %s, not %s",_tag_dyneither(_tmp467,sizeof(char),36))),_tag_dyneither(_tmp466,sizeof(void*),3)))))))))))))))))));}}
# 619
if(_tmp157 != 0){
const char*_tmp46B;void*_tmp46A[1];struct Cyc_String_pa_PrintArg_struct _tmp469;(_tmp469.tag=0,((_tmp469.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp46A[0]=& _tmp469,Cyc_Tcutil_terr(p->loc,((_tmp46B="too many arguments for datatype constructor %s",_tag_dyneither(_tmp46B,sizeof(char),47))),_tag_dyneither(_tmp46A,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp46F;void*_tmp46E[1];struct Cyc_String_pa_PrintArg_struct _tmp46D;(_tmp46D.tag=0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp46E[0]=& _tmp46D,Cyc_Tcutil_terr(p->loc,((_tmp46F="too few arguments for datatype constructor %s",_tag_dyneither(_tmp46F,sizeof(char),46))),_tag_dyneither(_tmp46E,sizeof(void*),1)))))));}}
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
# 642
return res;};}
# 645
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 647
struct Cyc_Tcpat_TcPatResult _tmp175=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 649
struct _RegionHandle _tmp176=_new_region("r");struct _RegionHandle*r=& _tmp176;_push_region(r);{
struct _tuple1 _stmttmpF=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp175.patvars);struct Cyc_List_List*_tmp178;struct _tuple1 _tmp177=_stmttmpF;_tmp178=_tmp177.f1;{
struct Cyc_List_List*_tmp179=0;
{struct Cyc_List_List*x=_tmp178;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp470;_tmp179=((_tmp470=_region_malloc(r,sizeof(*_tmp470)),((_tmp470->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp470->tl=_tmp179,_tmp470))))));}}}{
const char*_tmp471;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp179),p->loc,(
(_tmp471="pattern contains a repeated variable",_tag_dyneither(_tmp471,sizeof(char),37))));};};}{
# 657
struct Cyc_Tcpat_TcPatResult _tmp17C=_tmp175;_npop_handler(0);return _tmp17C;};
# 649
;_pop_region(r);}
# 660
enum Cyc_Tcpat_PathState{Cyc_Tcpat_UniquePath  = 0,Cyc_Tcpat_NoAlias  = 1,Cyc_Tcpat_NoAliasOneAlias  = 2,Cyc_Tcpat_AliasPath  = 3};
# 662
struct _dyneither_ptr Cyc_Tcpat_pathState2string(enum Cyc_Tcpat_PathState ps){
switch(ps){case Cyc_Tcpat_UniquePath: _LLA1: {
const char*_tmp472;return(_tmp472="UniquePath",_tag_dyneither(_tmp472,sizeof(char),11));}case Cyc_Tcpat_NoAlias: _LLA2: {
const char*_tmp473;return(_tmp473="NoAlias",_tag_dyneither(_tmp473,sizeof(char),8));}case Cyc_Tcpat_NoAliasOneAlias: _LLA3: {
const char*_tmp474;return(_tmp474="NoAliasOneAlias",_tag_dyneither(_tmp474,sizeof(char),16));}case Cyc_Tcpat_AliasPath: _LLA4: {
const char*_tmp475;return(_tmp475="AliasPath",_tag_dyneither(_tmp475,sizeof(char),10));}}}
# 671
int Cyc_Tcpat_check_unique_ok(enum Cyc_Tcpat_PathState*ps,void*pat_typ,unsigned int loc){
# 673
struct _RegionHandle _tmp181=_new_region("r");struct _RegionHandle*r=& _tmp181;_push_region(r);
{int is_unique_ptr=Cyc_Tcutil_is_noalias_pointer(pat_typ,1);
int is_noalias_ptr_or_aggr=Cyc_Tcutil_is_noalias_pointer_or_aggr(r,pat_typ);
{enum Cyc_Tcpat_PathState _stmttmp10=*ps;switch(_stmttmp10){case Cyc_Tcpat_UniquePath: _LLA6:
# 678
 if(!is_unique_ptr){
if(is_noalias_ptr_or_aggr)*ps=Cyc_Tcpat_NoAlias;else{
*ps=Cyc_Tcpat_AliasPath;}}{
int _tmp182=1;_npop_handler(0);return _tmp182;};case Cyc_Tcpat_NoAlias: _LLA7:
# 683
 if(!is_noalias_ptr_or_aggr){
*ps=Cyc_Tcpat_NoAliasOneAlias;{
int _tmp183=1;_npop_handler(0);return _tmp183;};}
# 687
break;case Cyc_Tcpat_NoAliasOneAlias: _LLA8:
 break;case Cyc_Tcpat_AliasPath: _LLA9:
# 690
 if(is_noalias_ptr_or_aggr)break;{
int _tmp184=1;_npop_handler(0);return _tmp184;};}}
# 693
{const char*_tmp478;void*_tmp477;(_tmp477=0,Cyc_Tcutil_terr(loc,((_tmp478="Pattern dereferences a unique pointer from non-unique path; use swap",_tag_dyneither(_tmp478,sizeof(char),69))),_tag_dyneither(_tmp477,sizeof(void*),0)));}{
# 695
int _tmp187=0;_npop_handler(0);return _tmp187;};}
# 674
;_pop_region(r);}
# 700
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,enum Cyc_Tcpat_PathState pState){
# 703
void*_stmttmp11=p->r;void*_tmp188=_stmttmp11;struct Cyc_Absyn_Pat*_tmp18A;struct Cyc_Absyn_AggrInfo*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp190;struct Cyc_List_List*_tmp192;struct Cyc_Absyn_Pat*_tmp194;struct Cyc_Absyn_Pat*_tmp196;_LLAC: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp189=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp189->tag != 5)goto _LLAE;else{_tmp18A=_tmp189->f1;}}_LLAD:
# 705
 if(!Cyc_Tcpat_check_unique_ok(& pState,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp18A,pState);{
# 708
void*_stmttmp12=(void*)_check_null(p->topt);void*_tmp197=_stmttmp12;void*_tmp199;_LLBB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp197;if(_tmp198->tag != 5)goto _LLBD;else{_tmp199=((_tmp198->f1).ptr_atts).rgn;}}_LLBC:
# 710
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp199);
return;_LLBD:;_LLBE: {
const char*_tmp47B;void*_tmp47A;(_tmp47A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp47B="check_pat_regions: bad pointer type",_tag_dyneither(_tmp47B,sizeof(char),36))),_tag_dyneither(_tmp47A,sizeof(void*),0)));}_LLBA:;};_LLAE: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp18B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp18B->tag != 6)goto _LLB0;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;_tmp18E=_tmp18B->f3;}}_LLAF:
# 715
 for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp18E->hd)).f2,pState);}
return;_LLB0: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp18F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp18F->tag != 7)goto _LLB2;else{_tmp190=_tmp18F->f3;}}_LLB1:
# 720
 for(0;_tmp190 != 0;_tmp190=_tmp190->tl){Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp190->hd,Cyc_Tcpat_AliasPath);}
return;_LLB2: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp191=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp191->tag != 4)goto _LLB4;else{_tmp192=_tmp191->f1;}}_LLB3:
# 723
 for(0;_tmp192 != 0;_tmp192=_tmp192->tl){Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp192->hd,pState);}
return;_LLB4: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp193=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp193->tag != 2)goto _LLB6;else{_tmp194=_tmp193->f2;}}_LLB5: {
# 726
enum Cyc_Tcpat_PathState _tmp19C=Cyc_Tcpat_AliasPath;
if(!Cyc_Tcpat_check_unique_ok(& _tmp19C,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp194,Cyc_Tcpat_AliasPath);
return;}_LLB6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp195=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp188;if(_tmp195->tag != 1)goto _LLB8;else{_tmp196=_tmp195->f2;}}_LLB7: {
# 731
enum Cyc_Tcpat_PathState _tmp19D=pState;
if(!Cyc_Tcpat_check_unique_ok(& _tmp19D,(void*)_check_null(p->topt),p->loc))return;
Cyc_Tcpat_check_pat_regions_rec(te,_tmp196,pState);
return;}_LLB8:;_LLB9:
 return;_LLAB:;}
# 738
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
Cyc_Tcpat_check_pat_regions_rec(te,p,Cyc_Tcpat_UniquePath);}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 779 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp47C;return((_tmp47C.Name_v).val=s,(((_tmp47C.Name_v).tag=1,_tmp47C)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp47D;return((_tmp47D.Int_v).val=i,(((_tmp47D.Int_v).tag=2,_tmp47D)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 789
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 794
typedef void*Cyc_Tcpat_simple_pat_t;
# 797
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _stmttmp13=c1->name;union Cyc_Tcpat_Name_value _tmp1A0=_stmttmp13;struct _dyneither_ptr _tmp1A1;int _tmp1A2;_LLC0: if((_tmp1A0.Name_v).tag != 1)goto _LLC2;_tmp1A1=(struct _dyneither_ptr)(_tmp1A0.Name_v).val;_LLC1: {
# 800
union Cyc_Tcpat_Name_value _stmttmp15=c2->name;union Cyc_Tcpat_Name_value _tmp1A3=_stmttmp15;struct _dyneither_ptr _tmp1A4;_LLC5: if((_tmp1A3.Name_v).tag != 1)goto _LLC7;_tmp1A4=(struct _dyneither_ptr)(_tmp1A3.Name_v).val;_LLC6:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1A1,(struct _dyneither_ptr)_tmp1A4);_LLC7: if((_tmp1A3.Int_v).tag != 2)goto _LLC4;_LLC8:
 return - 1;_LLC4:;}_LLC2: if((_tmp1A0.Int_v).tag != 2)goto _LLBF;_tmp1A2=(int)(_tmp1A0.Int_v).val;_LLC3: {
# 805
union Cyc_Tcpat_Name_value _stmttmp14=c2->name;union Cyc_Tcpat_Name_value _tmp1A5=_stmttmp14;int _tmp1A6;_LLCA: if((_tmp1A5.Name_v).tag != 1)goto _LLCC;_LLCB:
 return 1;_LLCC: if((_tmp1A5.Int_v).tag != 2)goto _LLC9;_tmp1A6=(int)(_tmp1A5.Int_v).val;_LLCD:
 return _tmp1A2 - _tmp1A6;_LLC9:;}_LLBF:;}
# 813
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 817
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 821
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp480;struct Cyc_Tcpat_Con_s*_tmp47F;return(_tmp47F=_region_malloc(r,sizeof(*_tmp47F)),((_tmp47F->name=Cyc_Tcpat_Name_v(((_tmp480="NULL",_tag_dyneither(_tmp480,sizeof(char),5)))),((_tmp47F->arity=0,((_tmp47F->span=& Cyc_Tcpat_two_opt,((_tmp47F->orig_pat=p,_tmp47F)))))))));}
# 824
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp483;struct Cyc_Tcpat_Con_s*_tmp482;return(_tmp482=_region_malloc(r,sizeof(*_tmp482)),((_tmp482->name=Cyc_Tcpat_Name_v(((_tmp483="&",_tag_dyneither(_tmp483,sizeof(char),2)))),((_tmp482->arity=1,((_tmp482->span=& Cyc_Tcpat_two_opt,((_tmp482->orig_pat=p,_tmp482)))))))));}
# 827
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp486;struct Cyc_Tcpat_Con_s*_tmp485;return(_tmp485=_region_malloc(r,sizeof(*_tmp485)),((_tmp485->name=Cyc_Tcpat_Name_v(((_tmp486="&",_tag_dyneither(_tmp486,sizeof(char),2)))),((_tmp485->arity=1,((_tmp485->span=& Cyc_Tcpat_one_opt,((_tmp485->orig_pat=p,_tmp485)))))))));}
# 830
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp487;return(_tmp487=_region_malloc(r,sizeof(*_tmp487)),((_tmp487->name=Cyc_Tcpat_Int_v(i),((_tmp487->arity=0,((_tmp487->span=0,((_tmp487->orig_pat=p,_tmp487)))))))));}
# 833
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp488;return(_tmp488=_region_malloc(r,sizeof(*_tmp488)),((_tmp488->name=Cyc_Tcpat_Name_v(f),((_tmp488->arity=0,((_tmp488->span=0,((_tmp488->orig_pat=p,_tmp488)))))))));}
# 836
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp489;return(_tmp489=_region_malloc(r,sizeof(*_tmp489)),((_tmp489->name=Cyc_Tcpat_Int_v((int)c),((_tmp489->arity=0,((_tmp489->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp489->orig_pat=p,_tmp489)))))))));}
# 839
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp48C;struct Cyc_Tcpat_Con_s*_tmp48B;return(_tmp48B=_region_malloc(r,sizeof(*_tmp48B)),((_tmp48B->name=Cyc_Tcpat_Name_v(((_tmp48C="$",_tag_dyneither(_tmp48C,sizeof(char),2)))),((_tmp48B->arity=i,((_tmp48B->span=& Cyc_Tcpat_one_opt,((_tmp48B->orig_pat=p,_tmp48B)))))))));}
# 844
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp48F;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp48E;return(void*)((_tmp48E=_region_malloc(r,sizeof(*_tmp48E)),((_tmp48E[0]=((_tmp48F.tag=1,((_tmp48F.f1=Cyc_Tcpat_null_con(r,p),((_tmp48F.f2=0,_tmp48F)))))),_tmp48E))));}
# 847
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp492;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp491;return(void*)((_tmp491=_region_malloc(r,sizeof(*_tmp491)),((_tmp491[0]=((_tmp492.tag=1,((_tmp492.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp492.f2=0,_tmp492)))))),_tmp491))));}
# 850
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp495;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp494;return(void*)((_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494[0]=((_tmp495.tag=1,((_tmp495.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp495.f2=0,_tmp495)))))),_tmp494))));}
# 853
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp498;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp497;return(void*)((_tmp497=_region_malloc(r,sizeof(*_tmp497)),((_tmp497[0]=((_tmp498.tag=1,((_tmp498.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp498.f2=0,_tmp498)))))),_tmp497))));}
# 856
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp49E;struct Cyc_List_List*_tmp49D;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp49C;return(void*)((_tmp49C=_region_malloc(r,sizeof(*_tmp49C)),((_tmp49C[0]=((_tmp49E.tag=1,((_tmp49E.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp49E.f2=((_tmp49D=_region_malloc(r,sizeof(*_tmp49D)),((_tmp49D->hd=p,((_tmp49D->tl=0,_tmp49D)))))),_tmp49E)))))),_tmp49C))));}
# 859
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A4;struct Cyc_List_List*_tmp4A3;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A2;return(void*)((_tmp4A2=_region_malloc(r,sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A4.tag=1,((_tmp4A4.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4A4.f2=((_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3->hd=p,((_tmp4A3->tl=0,_tmp4A3)))))),_tmp4A4)))))),_tmp4A2))));}
# 862
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A7;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A6;return(void*)((_tmp4A6=_region_malloc(r,sizeof(*_tmp4A6)),((_tmp4A6[0]=((_tmp4A7.tag=1,((_tmp4A7.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4A7.f2=ss,_tmp4A7)))))),_tmp4A6))));}
# 865
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 867
struct Cyc_Tcpat_Con_s*_tmp4A8;struct Cyc_Tcpat_Con_s*c=(_tmp4A8=_region_malloc(r,sizeof(*_tmp4A8)),((_tmp4A8->name=Cyc_Tcpat_Name_v(con_name),((_tmp4A8->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4A8->span=span,((_tmp4A8->orig_pat=p,_tmp4A8)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AB;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4AA;return(void*)((_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA)),((_tmp4AA[0]=((_tmp4AB.tag=1,((_tmp4AB.f1=c,((_tmp4AB.f2=ps,_tmp4AB)))))),_tmp4AA))));}
# 872
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_stmttmp16=p->r;void*_tmp1C5=_stmttmp16;enum Cyc_Absyn_Sign _tmp1CA;int _tmp1CB;char _tmp1CD;struct _dyneither_ptr _tmp1CF;struct Cyc_Absyn_Pat*_tmp1D1;struct Cyc_Absyn_Pat*_tmp1D3;struct Cyc_Absyn_Pat*_tmp1D5;struct Cyc_Absyn_Datatypedecl*_tmp1D7;struct Cyc_Absyn_Datatypefield*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Aggrdecl*_tmp1DD;struct Cyc_List_List*_tmp1DE;struct Cyc_Absyn_Enumdecl*_tmp1E0;struct Cyc_Absyn_Enumfield*_tmp1E1;void*_tmp1E3;struct Cyc_Absyn_Enumfield*_tmp1E4;_LLCF: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1C6=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1C6->tag != 0)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1C7->tag != 3)goto _LLD3;}_LLD2:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4AE;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4AD;s=(void*)((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=0,_tmp4AE)),_tmp4AD))));}goto _LLCE;_LLD3: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1C8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1C8->tag != 8)goto _LLD5;}_LLD4:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLCE;_LLD5: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1C9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1C9->tag != 9)goto _LLD7;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}_LLD6:
 s=Cyc_Tcpat_int_pat(r,_tmp1CB,p);goto _LLCE;_LLD7: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1CC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1CC->tag != 10)goto _LLD9;else{_tmp1CD=_tmp1CC->f1;}}_LLD8:
 s=Cyc_Tcpat_char_pat(r,_tmp1CD,p);goto _LLCE;_LLD9: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1CE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1CE->tag != 11)goto _LLDB;else{_tmp1CF=_tmp1CE->f1;}}_LLDA:
 s=Cyc_Tcpat_float_pat(r,_tmp1CF,p);goto _LLCE;_LLDB: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1D0->tag != 1)goto _LLDD;else{_tmp1D1=_tmp1D0->f2;}}_LLDC:
 s=Cyc_Tcpat_compile_pat(r,_tmp1D1);goto _LLCE;_LLDD: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1D2->tag != 2)goto _LLDF;else{_tmp1D3=_tmp1D2->f2;}}_LLDE:
 s=Cyc_Tcpat_compile_pat(r,_tmp1D3);goto _LLCE;_LLDF: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1D4=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1D4->tag != 5)goto _LLE1;else{_tmp1D5=_tmp1D4->f1;}}_LLE0:
# 884
{void*_stmttmp1C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1EB=_stmttmp1C;union Cyc_Absyn_Constraint*_tmp1ED;_LLF4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EB;if(_tmp1EC->tag != 5)goto _LLF6;else{_tmp1ED=((_tmp1EC->f1).ptr_atts).nullable;}}_LLF5: {
# 886
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1EE=_tmp1ED;union Cyc_Absyn_Constraint*_tmp1EF;int _tmp1F0;_LLF9: if((_tmp1EE->Forward_constr).tag != 2)goto _LLFB;_tmp1EF=(union Cyc_Absyn_Constraint*)(_tmp1EE->Forward_constr).val;_LLFA:
# 891
*_tmp1ED=*_tmp1EF;
continue;_LLFB: if((_tmp1EE->No_constr).tag != 3)goto _LLFD;_LLFC:
# 894
{struct _union_Constraint_Eq_constr*_tmp4AF;(_tmp4AF=& _tmp1ED->Eq_constr,((_tmp4AF->tag=1,_tmp4AF->val=0)));}
is_nullable=0;
still_working=0;
goto _LLF8;_LLFD: if((_tmp1EE->Eq_constr).tag != 1)goto _LLF8;_tmp1F0=(int)(_tmp1EE->Eq_constr).val;_LLFE:
# 899
 is_nullable=_tmp1F0;
still_working=0;
goto _LLF8;_LLF8:;}{
# 904
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1D5);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 908
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLF3;};}_LLF6:;_LLF7: {
const char*_tmp4B2;void*_tmp4B1;(_tmp4B1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B2="expecting pointertype for pattern!",_tag_dyneither(_tmp4B2,sizeof(char),35))),_tag_dyneither(_tmp4B1,sizeof(void*),0)));}_LLF3:;}
# 912
goto _LLCE;_LLE1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1D6->tag != 7)goto _LLE3;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;_tmp1D9=_tmp1D6->f3;}}_LLE2: {
# 914
int*span;
{void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F4=_stmttmp1B;_LL100: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1F5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F4;if(_tmp1F5->tag != 3)goto _LL102;}_LL101:
# 917
 if(_tmp1D7->is_extensible)
span=0;else{
# 920
int*_tmp4B3;span=((_tmp4B3=_region_malloc(r,sizeof(*_tmp4B3)),((_tmp4B3[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D7->fields))->v),_tmp4B3))));}
goto _LLFF;_LL102: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F4;if(_tmp1F6->tag != 4)goto _LL104;}_LL103:
 span=& Cyc_Tcpat_one_opt;goto _LLFF;_LL104:;_LL105:
{const char*_tmp4B6;void*_tmp4B5;span=((_tmp4B5=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B6="void datatype pattern has bad type",_tag_dyneither(_tmp4B6,sizeof(char),35))),_tag_dyneither(_tmp4B5,sizeof(void*),0))));}goto _LLFF;_LLFF:;}
# 925
s=Cyc_Tcpat_con_pat(r,*(*_tmp1D8->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1D9),p);
goto _LLCE;}_LLE3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1DA=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1DA->tag != 4)goto _LLE5;else{_tmp1DB=_tmp1DA->f1;}}_LLE4:
# 929
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1DB),p);goto _LLCE;_LLE5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1DC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1DC->tag != 6)goto _LLE7;else{if(_tmp1DC->f1 == 0)goto _LLE7;if((((_tmp1DC->f1)->aggr_info).KnownAggr).tag != 2)goto _LLE7;_tmp1DD=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1DC->f1)->aggr_info).KnownAggr).val);_tmp1DE=_tmp1DC->f3;}}_LLE6:
# 934
 if(_tmp1DD->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 938
const char*_tmp4B7;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4B7="",_tag_dyneither(_tmp4B7,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1DE;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_stmttmp1A=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp1FB;struct Cyc_Absyn_Pat*_tmp1FC;struct _tuple15*_tmp1FA=_stmttmp1A;_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;{
struct Cyc_List_List*_tmp1FD=_tmp1FB;struct _dyneither_ptr*_tmp1FF;_LL107: if(_tmp1FD == 0)goto _LL109;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1FE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp1FD->hd);if(_tmp1FE->tag != 1)goto _LL109;else{_tmp1FF=_tmp1FE->f1;}}if(_tmp1FD->tl != 0)goto _LL109;_LL108:
# 943
 if(Cyc_strptrcmp(_tmp1FF,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4B8;ps=((_tmp4B8=_region_malloc(r,sizeof(*_tmp4B8)),((_tmp4B8->hd=Cyc_Tcpat_compile_pat(r,_tmp1FC),((_tmp4B8->tl=ps,_tmp4B8))))));}
found=1;}
# 947
goto _LL106;_LL109:;_LL10A: {
const char*_tmp4BB;void*_tmp4BA;(_tmp4BA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BB="bad designator(s)",_tag_dyneither(_tmp4BB,sizeof(char),18))),_tag_dyneither(_tmp4BA,sizeof(void*),0)));}_LL106:;};}}
# 951
if(!found){
const char*_tmp4BE;void*_tmp4BD;(_tmp4BD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4BE="bad designator",_tag_dyneither(_tmp4BE,sizeof(char),15))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}}}
# 954
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 957
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->tagged){
const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,Cyc_Tcutil_terr(p->loc,((_tmp4C1="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4C1,sizeof(char),47))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}{
int*_tmp4C2;int*span=(_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DD->impl))->fields),_tmp4C2)));
struct Cyc_List_List*_tmp208=_tmp1DE;struct _dyneither_ptr*_tmp20A;struct Cyc_Absyn_Pat*_tmp20B;_LL10C: if(_tmp208 == 0)goto _LL10E;if(((struct _tuple15*)_tmp208->hd)->f1 == 0)goto _LL10E;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp209=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp208->hd)->f1)->hd);if(_tmp209->tag != 1)goto _LL10E;else{_tmp20A=_tmp209->f1;}}if((((struct _tuple15*)_tmp208->hd)->f1)->tl != 0)goto _LL10E;_tmp20B=((struct _tuple15*)_tmp208->hd)->f2;if(_tmp208->tl != 0)goto _LL10E;_LL10D:
# 962
{struct Cyc_List_List*_tmp4C3;s=Cyc_Tcpat_con_pat(r,*_tmp20A,span,((_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3)),((_tmp4C3->hd=Cyc_Tcpat_compile_pat(r,_tmp20B),((_tmp4C3->tl=0,_tmp4C3)))))),p);}
goto _LL10B;_LL10E:;_LL10F: {
const char*_tmp4C6;void*_tmp4C5;(_tmp4C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4C6="bad union pattern",_tag_dyneither(_tmp4C6,sizeof(char),18))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}_LL10B:;};}
# 967
goto _LLCE;_LLE7: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1DF->tag != 12)goto _LLE9;else{_tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}}_LLE8:
# 972
{void*_stmttmp19=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp210=_stmttmp19;_LL111: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp210;if(_tmp211->tag != 6)goto _LL113;}_LL112:
# 976
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1E1->name).f2,0,0,p);
goto _LL110;_LL113:;_LL114: {
# 979
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E0->fields))->v);
{int*_tmp4C7;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E1->name).f2,((_tmp4C7=_region_malloc(r,sizeof(*_tmp4C7)),((_tmp4C7[0]=span,_tmp4C7)))),0,p);}
goto _LL110;}_LL110:;}
# 983
goto _LLCE;_LLE9: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1E2=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1E2->tag != 13)goto _LLEB;else{_tmp1E3=(void*)_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;}}_LLEA: {
# 988
struct Cyc_List_List*fields;
{void*_stmttmp17=Cyc_Tcutil_compress(_tmp1E3);void*_tmp213=_stmttmp17;struct Cyc_List_List*_tmp215;_LL116: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 14)goto _LL118;else{_tmp215=_tmp214->f1;}}_LL117:
 fields=_tmp215;goto _LL115;_LL118:;_LL119: {
const char*_tmp4CA;void*_tmp4C9;(_tmp4C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CA="bad type in AnonEnum_p",_tag_dyneither(_tmp4CA,sizeof(char),23))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}_LL115:;}
# 995
{void*_stmttmp18=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp218=_stmttmp18;_LL11B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 6)goto _LL11D;}_LL11C:
# 999
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,0,0,p);
goto _LL11A;_LL11D:;_LL11E: {
# 1002
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4CB;s=Cyc_Tcpat_con_pat(r,*(*_tmp1E4->name).f2,((_tmp4CB=_region_malloc(r,sizeof(*_tmp4CB)),((_tmp4CB[0]=span,_tmp4CB)))),0,p);}
goto _LL11A;}_LL11A:;}
# 1006
goto _LLCE;}_LLEB: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1E5=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1E5->tag != 14)goto _LLED;}_LLEC:
 goto _LLEE;_LLED: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E6=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1E6->tag != 15)goto _LLEF;}_LLEE:
 goto _LLF0;_LLEF: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1E7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1E7->tag != 6)goto _LLF1;}_LLF0:
 goto _LLF2;_LLF1: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1E8=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1C5;if(_tmp1E8->tag != 16)goto _LLCE;}_LLF2: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4CE;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4CD;s=(void*)((_tmp4CD=_region_malloc(r,sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CE.tag=0,_tmp4CE)),_tmp4CD))));}_LLCE:;}
# 1012
return s;}
# 1020
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1031
typedef void*Cyc_Tcpat_term_desc_t;
# 1035
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1042
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1051
typedef void*Cyc_Tcpat_decision_t;struct _tuple19{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1056
typedef struct _tuple19*Cyc_Tcpat_work_stack_frame_t;
# 1059
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1061
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1064
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4D1;void*_tmp4D0;(_tmp4D0=0,Cyc_fprintf(Cyc_stderr,((_tmp4D1=" ",_tag_dyneither(_tmp4D1,sizeof(char),2))),_tag_dyneither(_tmp4D0,sizeof(void*),0)));}}
# 1070
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp21F=((struct Cyc_Tcpat_Con_s*)_check_null(c))->name;
union Cyc_Tcpat_Name_value _tmp220=_tmp21F;struct _dyneither_ptr _tmp221;int _tmp222;_LL120: if((_tmp220.Name_v).tag != 1)goto _LL122;_tmp221=(struct _dyneither_ptr)(_tmp220.Name_v).val;_LL121:
{const char*_tmp4D5;void*_tmp4D4[1];struct Cyc_String_pa_PrintArg_struct _tmp4D3;(_tmp4D3.tag=0,((_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp221),((_tmp4D4[0]=& _tmp4D3,Cyc_fprintf(Cyc_stderr,((_tmp4D5="%s",_tag_dyneither(_tmp4D5,sizeof(char),3))),_tag_dyneither(_tmp4D4,sizeof(void*),1)))))));}goto _LL11F;_LL122: if((_tmp220.Int_v).tag != 2)goto _LL11F;_tmp222=(int)(_tmp220.Int_v).val;_LL123:
{const char*_tmp4D9;void*_tmp4D8[1];struct Cyc_Int_pa_PrintArg_struct _tmp4D7;(_tmp4D7.tag=1,((_tmp4D7.f1=(unsigned long)_tmp222,((_tmp4D8[0]=& _tmp4D7,Cyc_fprintf(Cyc_stderr,((_tmp4D9="%d",_tag_dyneither(_tmp4D9,sizeof(char),3))),_tag_dyneither(_tmp4D8,sizeof(void*),1)))))));}goto _LL11F;_LL11F:;}
# 1078
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp229=d;void*_tmp22B;struct Cyc_List_List*_tmp22E;struct Cyc_Tcpat_Con_s*_tmp22F;void*_tmp230;void*_tmp231;_LL125: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp22A=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp229;if(_tmp22A->tag != 1)goto _LL127;else{_tmp22B=(void*)_tmp22A->f1;}}_LL126:
# 1081
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4DC;void*_tmp4DB;(_tmp4DB=0,Cyc_fprintf(Cyc_stderr,((_tmp4DC="Success(",_tag_dyneither(_tmp4DC,sizeof(char),9))),_tag_dyneither(_tmp4DB,sizeof(void*),0)));}print_rhs(_tmp22B);{const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,Cyc_fprintf(Cyc_stderr,((_tmp4DF=")\n",_tag_dyneither(_tmp4DF,sizeof(char),3))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
goto _LL124;_LL127: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp22C=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp229;if(_tmp22C->tag != 0)goto _LL129;}_LL128:
{const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,Cyc_fprintf(Cyc_stderr,((_tmp4E2="Failure\n",_tag_dyneither(_tmp4E2,sizeof(char),9))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}goto _LL124;_LL129: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp22D=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp229;if(_tmp22D->tag != 2)goto _LL124;else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22D->f2;_tmp230=(void*)_tmp22D->f3;_tmp231=(void*)_tmp22D->f4;}}_LL12A:
# 1086
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4E5;void*_tmp4E4;(_tmp4E4=0,Cyc_fprintf(Cyc_stderr,((_tmp4E5="Access[",_tag_dyneither(_tmp4E5,sizeof(char),8))),_tag_dyneither(_tmp4E4,sizeof(void*),0)));}
for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
{const char*_tmp4E9;void*_tmp4E8[1];struct Cyc_Int_pa_PrintArg_struct _tmp4E7;(_tmp4E7.tag=1,((_tmp4E7.f1=(unsigned long)((int)_tmp22E->hd),((_tmp4E8[0]=& _tmp4E7,Cyc_fprintf(Cyc_stderr,((_tmp4E9="%d",_tag_dyneither(_tmp4E9,sizeof(char),3))),_tag_dyneither(_tmp4E8,sizeof(void*),1)))))));}
if(_tmp22E->tl != 0){const char*_tmp4EC;void*_tmp4EB;(_tmp4EB=0,Cyc_fprintf(Cyc_stderr,((_tmp4EC=",",_tag_dyneither(_tmp4EC,sizeof(char),2))),_tag_dyneither(_tmp4EB,sizeof(void*),0)));}}
# 1092
{const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,Cyc_fprintf(Cyc_stderr,((_tmp4EF="],",_tag_dyneither(_tmp4EF,sizeof(char),3))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp22F);
{const char*_tmp4F2;void*_tmp4F1;(_tmp4F1=0,Cyc_fprintf(Cyc_stderr,((_tmp4F2=",\n",_tag_dyneither(_tmp4F2,sizeof(char),3))),_tag_dyneither(_tmp4F1,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp230,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp231,print_rhs,tab + 7);_LL124:;}struct _tuple20{int f1;unsigned int f2;};
# 1100
static void Cyc_Tcpat_print_swrhs(struct _tuple20*x){
return;}
# 1108
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp243=td;struct Cyc_Set_Set*_tmp245;_LL12C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp244=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp243;if(_tmp244->tag != 1)goto _LL12E;else{_tmp245=_tmp244->f1;}}_LL12D: {
# 1118
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4F5;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4F4;return(void*)((_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F5.tag=1,((_tmp4F5.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp245,c),_tmp4F5)))),_tmp4F4))));}_LL12E: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp246=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp243;if(_tmp246->tag != 0)goto _LL12B;}_LL12F: {
const char*_tmp4F8;void*_tmp4F7;(_tmp4F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F8="add_neg called when td is Positive",_tag_dyneither(_tmp4F8,sizeof(char),35))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}_LL12B:;}
# 1125
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp24B=td;struct Cyc_Tcpat_Con_s*_tmp24D;struct Cyc_Set_Set*_tmp24F;_LL131: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp24C=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp24B;if(_tmp24C->tag != 0)goto _LL133;else{_tmp24D=_tmp24C->f1;}}_LL132:
# 1129
 if(Cyc_Tcpat_compare_con(c,_tmp24D)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL133: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp24E=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp24B;if(_tmp24E->tag != 1)goto _LL130;else{_tmp24F=_tmp24E->f1;}}_LL134:
# 1133
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp24F,c))return Cyc_Tcpat_No;else{
# 1136
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp24F)+ 1)
return Cyc_Tcpat_Yes;else{
# 1139
return Cyc_Tcpat_Maybe;}}_LL130:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1147
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1149
struct Cyc_List_List*_tmp250=ctxt;struct Cyc_Tcpat_Con_s*_tmp251;struct Cyc_List_List*_tmp252;struct Cyc_List_List*_tmp253;_LL136: if(_tmp250 != 0)goto _LL138;_LL137:
 return 0;_LL138: if(_tmp250 == 0)goto _LL135;_tmp251=((struct _tuple21*)_tmp250->hd)->f1;_tmp252=((struct _tuple21*)_tmp250->hd)->f2;_tmp253=_tmp250->tl;_LL139: {
# 1152
struct _tuple21*_tmp4FE;struct Cyc_List_List*_tmp4FD;struct Cyc_List_List*_tmp4FC;return(_tmp4FC=_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC->hd=(
(_tmp4FE=_region_malloc(r,sizeof(*_tmp4FE)),((_tmp4FE->f1=_tmp251,((_tmp4FE->f2=((_tmp4FD=_region_malloc(r,sizeof(*_tmp4FD)),((_tmp4FD->hd=dsc,((_tmp4FD->tl=_tmp252,_tmp4FD)))))),_tmp4FE)))))),((_tmp4FC->tl=_tmp253,_tmp4FC)))));}_LL135:;}
# 1160
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp257=ctxt;struct Cyc_Tcpat_Con_s*_tmp258;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp25A;_LL13B: if(_tmp257 != 0)goto _LL13D;_LL13C: {
const char*_tmp501;void*_tmp500;(_tmp500=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp501="norm_context: empty context",_tag_dyneither(_tmp501,sizeof(char),28))),_tag_dyneither(_tmp500,sizeof(void*),0)));}_LL13D: if(_tmp257 == 0)goto _LL13A;_tmp258=((struct _tuple21*)_tmp257->hd)->f1;_tmp259=((struct _tuple21*)_tmp257->hd)->f2;_tmp25A=_tmp257->tl;_LL13E: {
# 1164
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp504;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp503;return Cyc_Tcpat_augment(r,_tmp25A,(void*)((_tmp503=_region_malloc(r,sizeof(*_tmp503)),((_tmp503[0]=((_tmp504.tag=0,((_tmp504.f1=_tmp258,((_tmp504.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp259),_tmp504)))))),_tmp503)))));}_LL13A:;}
# 1173
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1175
struct _tuple1 _tmp505;struct _tuple1 _stmttmp1D=(_tmp505.f1=ctxt,((_tmp505.f2=work,_tmp505)));struct _tuple1 _tmp25F=_stmttmp1D;struct Cyc_Tcpat_Con_s*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp264;_LL140: if(_tmp25F.f1 != 0)goto _LL142;if(_tmp25F.f2 != 0)goto _LL142;_LL141:
 return dsc;_LL142: if(_tmp25F.f1 != 0)goto _LL144;_LL143:
 goto _LL145;_LL144: if(_tmp25F.f2 != 0)goto _LL146;_LL145: {
const char*_tmp508;void*_tmp507;(_tmp507=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp508="build_desc: ctxt and work don't match",_tag_dyneither(_tmp508,sizeof(char),38))),_tag_dyneither(_tmp507,sizeof(void*),0)));}_LL146: if(_tmp25F.f1 == 0)goto _LL13F;_tmp260=((struct _tuple21*)(_tmp25F.f1)->hd)->f1;_tmp261=((struct _tuple21*)(_tmp25F.f1)->hd)->f2;_tmp262=(_tmp25F.f1)->tl;if(_tmp25F.f2 == 0)goto _LL13F;_tmp263=((struct _tuple19*)(_tmp25F.f2)->hd)->f3;_tmp264=(_tmp25F.f2)->tl;_LL147: {
# 1180
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp50C;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp267=(_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C[0]=((_tmp50E.tag=0,((_tmp50E.f1=_tmp260,((_tmp50E.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp261),(
(_tmp50D=_region_malloc(r,sizeof(*_tmp50D)),((_tmp50D->hd=dsc,((_tmp50D->tl=_tmp263,_tmp50D))))))),_tmp50E)))))),_tmp50C)));
return Cyc_Tcpat_build_desc(r,_tmp262,(void*)_tmp267,_tmp264);}_LL13F:;}
# 1186
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1193
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1195
struct Cyc_List_List*_tmp26C=allmrules;void*_tmp26D;void*_tmp26E;struct Cyc_List_List*_tmp26F;_LL149: if(_tmp26C != 0)goto _LL14B;_LL14A: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp511;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp510;return(void*)((_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510[0]=((_tmp511.tag=0,((_tmp511.f1=dsc,_tmp511)))),_tmp510))));}_LL14B: if(_tmp26C == 0)goto _LL148;_tmp26D=((struct _tuple0*)_tmp26C->hd)->f1;_tmp26E=((struct _tuple0*)_tmp26C->hd)->f2;_tmp26F=_tmp26C->tl;_LL14C:
# 1198
 return Cyc_Tcpat_match(r,_tmp26D,0,dsc,0,0,_tmp26E,_tmp26F);_LL148:;}
# 1203
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1205
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp514;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp513;return Cyc_Tcpat_or_match(r,(void*)((_tmp513=_region_malloc(r,sizeof(*_tmp513)),((_tmp513[0]=((_tmp514.tag=1,((_tmp514.f1=Cyc_Tcpat_empty_con_set(r),_tmp514)))),_tmp513)))),allmrules);}
# 1211
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1215
struct Cyc_List_List*_tmp274=work;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp279;_LL14E: if(_tmp274 != 0)goto _LL150;_LL14F: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp517;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp516;return(void*)((_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516[0]=((_tmp517.tag=1,((_tmp517.f1=right_hand_side,_tmp517)))),_tmp516))));}_LL150: if(_tmp274 == 0)goto _LL152;if(((struct _tuple19*)_tmp274->hd)->f1 != 0)goto _LL152;if(((struct _tuple19*)_tmp274->hd)->f2 != 0)goto _LL152;if(((struct _tuple19*)_tmp274->hd)->f3 != 0)goto _LL152;_tmp275=_tmp274->tl;_LL151:
# 1218
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp275,right_hand_side,rules);_LL152: if(_tmp274 == 0)goto _LL14D;_tmp276=((struct _tuple19*)_tmp274->hd)->f1;_tmp277=((struct _tuple19*)_tmp274->hd)->f2;_tmp278=((struct _tuple19*)_tmp274->hd)->f3;_tmp279=_tmp274->tl;_LL153:
# 1220
 if((_tmp276 == 0  || _tmp277 == 0) || _tmp278 == 0){
const char*_tmp51A;void*_tmp519;(_tmp519=0,Cyc_Tcutil_impos(((_tmp51A="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp51A,sizeof(char),38))),_tag_dyneither(_tmp519,sizeof(void*),0)));}{
struct Cyc_List_List*_stmttmp1E=_tmp276;void*_tmp27F;struct Cyc_List_List*_tmp280;struct Cyc_List_List*_tmp27E=_stmttmp1E;_tmp27F=(void*)_tmp27E->hd;_tmp280=_tmp27E->tl;{
struct Cyc_List_List*_stmttmp1F=_tmp277;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp281=_stmttmp1F;_tmp282=(struct Cyc_List_List*)_tmp281->hd;_tmp283=_tmp281->tl;{
struct Cyc_List_List*_stmttmp20=_tmp278;void*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp284=_stmttmp20;_tmp285=(void*)_tmp284->hd;_tmp286=_tmp284->tl;{
struct _tuple19*_tmp51B;struct _tuple19*_tmp287=(_tmp51B=_region_malloc(r,sizeof(*_tmp51B)),((_tmp51B->f1=_tmp280,((_tmp51B->f2=_tmp283,((_tmp51B->f3=_tmp286,_tmp51B)))))));
struct Cyc_List_List*_tmp51C;return Cyc_Tcpat_match(r,_tmp27F,_tmp282,_tmp285,ctx,((_tmp51C=_region_malloc(r,sizeof(*_tmp51C)),((_tmp51C->hd=_tmp287,((_tmp51C->tl=_tmp279,_tmp51C)))))),right_hand_side,rules);};};};};_LL14D:;}
# 1232
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1234
void*_tmp28A=dsc;struct Cyc_Set_Set*_tmp28C;struct Cyc_List_List*_tmp28E;_LL155: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp28B=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp28A;if(_tmp28B->tag != 1)goto _LL157;else{_tmp28C=_tmp28B->f1;}}_LL156: {
# 1239
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp51F;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp51E;void*any=(void*)((_tmp51E=_region_malloc(r,sizeof(*_tmp51E)),((_tmp51E[0]=((_tmp51F.tag=1,((_tmp51F.f1=Cyc_Tcpat_empty_con_set(r),_tmp51F)))),_tmp51E))));
struct Cyc_List_List*_tmp28F=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp520;_tmp28F=((_tmp520=_region_malloc(r,sizeof(*_tmp520)),((_tmp520->hd=any,((_tmp520->tl=_tmp28F,_tmp520))))));}}
return _tmp28F;}_LL157: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp28D=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp28A;if(_tmp28D->tag != 0)goto _LL154;else{_tmp28E=_tmp28D->f2;}}_LL158:
 return _tmp28E;_LL154:;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1249
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple22*env,int i){
struct _RegionHandle*_tmp294;struct Cyc_List_List*_tmp295;struct _tuple22*_tmp293=env;_tmp294=_tmp293->f1;_tmp295=_tmp293->f2;{
struct Cyc_List_List*_tmp521;return(_tmp521=_region_malloc(_tmp294,sizeof(*_tmp521)),((_tmp521->hd=(void*)(i + 1),((_tmp521->tl=_tmp295,_tmp521)))));};}
# 1253
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1255
struct _tuple22 _tmp522;struct _tuple22 _tmp297=(_tmp522.f1=r,((_tmp522.f2=obj,_tmp522)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple22*,int),struct _tuple22*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp297);}
# 1262
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1266
void*_tmp299=p;struct Cyc_Tcpat_Con_s*_tmp29C;struct Cyc_List_List*_tmp29D;_LL15A: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp29A=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp299;if(_tmp29A->tag != 0)goto _LL15C;}_LL15B:
# 1268
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL15C: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp29B=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp299;if(_tmp29B->tag != 1)goto _LL159;else{_tmp29C=_tmp29B->f1;_tmp29D=_tmp29B->f2;}}_LL15D: {
# 1270
enum Cyc_Tcpat_Answer _stmttmp21=Cyc_Tcpat_static_match(_tmp29C,dsc);switch(_stmttmp21){case Cyc_Tcpat_Yes: _LL15E: {
# 1272
struct _tuple21*_tmp525;struct Cyc_List_List*_tmp524;struct Cyc_List_List*ctx2=(_tmp524=_region_malloc(r,sizeof(*_tmp524)),((_tmp524->hd=((_tmp525=_region_malloc(r,sizeof(*_tmp525)),((_tmp525->f1=_tmp29C,((_tmp525->f2=0,_tmp525)))))),((_tmp524->tl=ctx,_tmp524)))));
struct _tuple19*_tmp526;struct _tuple19*work_frame=(_tmp526=_region_malloc(r,sizeof(*_tmp526)),((_tmp526->f1=_tmp29D,((_tmp526->f2=Cyc_Tcpat_getoargs(r,_tmp29C,obj),((_tmp526->f3=
Cyc_Tcpat_getdargs(r,_tmp29C,dsc),_tmp526)))))));
struct Cyc_List_List*_tmp527;struct Cyc_List_List*work2=(_tmp527=_region_malloc(r,sizeof(*_tmp527)),((_tmp527->hd=work_frame,((_tmp527->tl=work,_tmp527)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL15F:
# 1278
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL160: {
# 1280
struct _tuple21*_tmp52A;struct Cyc_List_List*_tmp529;struct Cyc_List_List*ctx2=(_tmp529=_region_malloc(r,sizeof(*_tmp529)),((_tmp529->hd=((_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A->f1=_tmp29C,((_tmp52A->f2=0,_tmp52A)))))),((_tmp529->tl=ctx,_tmp529)))));
struct _tuple19*_tmp52B;struct _tuple19*work_frame=(_tmp52B=_region_malloc(r,sizeof(*_tmp52B)),((_tmp52B->f1=_tmp29D,((_tmp52B->f2=Cyc_Tcpat_getoargs(r,_tmp29C,obj),((_tmp52B->f3=
Cyc_Tcpat_getdargs(r,_tmp29C,dsc),_tmp52B)))))));
struct Cyc_List_List*_tmp52C;struct Cyc_List_List*work2=(_tmp52C=_region_malloc(r,sizeof(*_tmp52C)),((_tmp52C->hd=work_frame,((_tmp52C->tl=work,_tmp52C)))));
void*_tmp2A2=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2A3=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp29C),work),rules);
# 1287
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp52F;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp52E;return(void*)((_tmp52E=_region_malloc(r,sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp52F.tag=2,((_tmp52F.f1=obj,((_tmp52F.f2=_tmp29C,((_tmp52F.f3=_tmp2A2,((_tmp52F.f4=_tmp2A3,_tmp52F)))))))))),_tmp52E))));}}}_LL159:;}
# 1297
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1303
void*_tmp2AA=d;void*_tmp2AC;void*_tmp2AE;void*_tmp2B0;void*_tmp2B1;_LL163: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2AB=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2AA;if(_tmp2AB->tag != 0)goto _LL165;else{_tmp2AC=(void*)_tmp2AB->f1;}}_LL164:
# 1305
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2AC);*exhaust_done=1;}
goto _LL162;_LL165: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2AD=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2AA;if(_tmp2AD->tag != 1)goto _LL167;else{_tmp2AE=(void*)_tmp2AD->f1;}}_LL166:
 rhs_appears(env2,_tmp2AE);goto _LL162;_LL167: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2AF=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2AA;if(_tmp2AF->tag != 2)goto _LL162;else{_tmp2B0=(void*)_tmp2AF->f3;_tmp2B1=(void*)_tmp2AF->f4;}}_LL168:
# 1309
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B0,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1311
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B1,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1313
goto _LL162;_LL162:;}struct _tuple23{void*f1;struct _tuple20*f2;};struct _tuple24{struct _RegionHandle*f1;int*f2;};
# 1321
static struct _tuple23*Cyc_Tcpat_get_match(struct _tuple24*env,struct Cyc_Absyn_Switch_clause*swc){
# 1324
struct _RegionHandle*_tmp2B3;int*_tmp2B4;struct _tuple24*_tmp2B2=env;_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2B3,swc->pattern);
struct _tuple20*_tmp530;struct _tuple20*rhs=(_tmp530=_region_malloc(_tmp2B3,sizeof(*_tmp530)),((_tmp530->f1=0,((_tmp530->f2=(swc->pattern)->loc,_tmp530)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp533;struct Cyc_List_List*_tmp532;sp=Cyc_Tcpat_tuple_pat(_tmp2B3,((_tmp532=_region_malloc(_tmp2B3,sizeof(*_tmp532)),((_tmp532->hd=sp0,((_tmp532->tl=(
(_tmp533=_region_malloc(_tmp2B3,sizeof(*_tmp533)),((_tmp533->hd=Cyc_Tcpat_int_pat(_tmp2B3,*_tmp2B4,0),((_tmp533->tl=0,_tmp533)))))),_tmp532)))))),0);}
*_tmp2B4=*_tmp2B4 + 1;}else{
# 1333
struct Cyc_List_List*_tmp53D;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp53C;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp53B;struct Cyc_List_List*_tmp53A;sp=Cyc_Tcpat_tuple_pat(_tmp2B3,((_tmp53A=_region_malloc(_tmp2B3,sizeof(*_tmp53A)),((_tmp53A->hd=sp0,((_tmp53A->tl=((_tmp53D=_region_malloc(_tmp2B3,sizeof(*_tmp53D)),((_tmp53D->hd=(void*)((_tmp53B=_region_malloc(_tmp2B3,sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53C.tag=0,_tmp53C)),_tmp53B)))),((_tmp53D->tl=0,_tmp53D)))))),_tmp53A)))))),0);}{
struct _tuple23*_tmp53E;return(_tmp53E=_region_malloc(_tmp2B3,sizeof(*_tmp53E)),((_tmp53E->f1=sp,((_tmp53E->f2=rhs,_tmp53E)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1339
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1341
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2BE=d;struct Cyc_Tcpat_Con_s*_tmp2C0;struct Cyc_List_List*_tmp2C1;struct Cyc_Set_Set*_tmp2C3;_LL16A: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BF=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2BE;if(_tmp2BF->tag != 0)goto _LL16C;else{_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2BF->f2;}}_LL16B: {
# 1345
union Cyc_Tcpat_Name_value _tmp2C4=((struct Cyc_Tcpat_Con_s*)_check_null(_tmp2C0))->name;
struct Cyc_Absyn_Pat*_tmp2C5=((struct Cyc_Tcpat_Con_s*)_check_null(_tmp2C0))->orig_pat;
if(_tmp2C5 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2C1))->hd);{
# 1350
void*_stmttmp24=_tmp2C5->r;void*_tmp2C6=_stmttmp24;struct Cyc_Absyn_Vardecl*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CB;struct Cyc_Absyn_Tvar*_tmp2CD;struct Cyc_Absyn_Vardecl*_tmp2CE;struct Cyc_Absyn_Aggrdecl*_tmp2D2;struct Cyc_List_List*_tmp2D3;struct Cyc_Absyn_Datatypefield*_tmp2D5;int _tmp2D8;char _tmp2DA;struct _dyneither_ptr _tmp2DC;int _tmp2DD;struct Cyc_Absyn_Enumfield*_tmp2DF;struct Cyc_Absyn_Enumfield*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E3;_LL16F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2C7=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2C7->tag != 0)goto _LL171;}_LL170: {
const char*_tmp53F;return(_tmp53F="_",_tag_dyneither(_tmp53F,sizeof(char),2));}_LL171: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2C8=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2C8->tag != 1)goto _LL173;else{_tmp2C9=_tmp2C8->f1;}}_LL172:
 return Cyc_Absynpp_qvar2string(_tmp2C9->name);_LL173: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2CA=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2CA->tag != 2)goto _LL175;else{_tmp2CB=_tmp2CA->f1;}}_LL174: {
const char*_tmp543;void*_tmp542[1];struct Cyc_String_pa_PrintArg_struct _tmp541;return(struct _dyneither_ptr)((_tmp541.tag=0,((_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2CB->name)),((_tmp542[0]=& _tmp541,Cyc_aprintf(((_tmp543="*%s",_tag_dyneither(_tmp543,sizeof(char),4))),_tag_dyneither(_tmp542,sizeof(void*),1))))))));}_LL175: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2CC=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2CC->tag != 3)goto _LL177;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;}}_LL176: {
const char*_tmp548;void*_tmp547[2];struct Cyc_String_pa_PrintArg_struct _tmp546;struct Cyc_String_pa_PrintArg_struct _tmp545;return(struct _dyneither_ptr)((_tmp545.tag=0,((_tmp545.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CD->name),((_tmp546.tag=0,((_tmp546.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2CE->name)),((_tmp547[0]=& _tmp546,((_tmp547[1]=& _tmp545,Cyc_aprintf(((_tmp548="%s<`%s>",_tag_dyneither(_tmp548,sizeof(char),8))),_tag_dyneither(_tmp547,sizeof(void*),2))))))))))))));}_LL177: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2CF=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2CF->tag != 4)goto _LL179;}_LL178: {
# 1356
const char*_tmp54C;void*_tmp54B[1];struct Cyc_String_pa_PrintArg_struct _tmp54A;return(struct _dyneither_ptr)((_tmp54A.tag=0,((_tmp54A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C1)),((_tmp54B[0]=& _tmp54A,Cyc_aprintf(((_tmp54C="$(%s)",_tag_dyneither(_tmp54C,sizeof(char),6))),_tag_dyneither(_tmp54B,sizeof(void*),1))))))));}_LL179: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2D0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D0->tag != 5)goto _LL17B;}_LL17A: {
# 1358
const char*_tmp550;void*_tmp54F[1];struct Cyc_String_pa_PrintArg_struct _tmp54E;return(struct _dyneither_ptr)((_tmp54E.tag=0,((_tmp54E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C1)),((_tmp54F[0]=& _tmp54E,Cyc_aprintf(((_tmp550="&%s",_tag_dyneither(_tmp550,sizeof(char),4))),_tag_dyneither(_tmp54F,sizeof(void*),1))))))));}_LL17B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2D1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D1->tag != 6)goto _LL17D;else{if(_tmp2D1->f1 == 0)goto _LL17D;if((((_tmp2D1->f1)->aggr_info).KnownAggr).tag != 2)goto _LL17D;_tmp2D2=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2D1->f1)->aggr_info).KnownAggr).val);_tmp2D3=_tmp2D1->f3;}}_LL17C:
# 1360
 if(_tmp2D2->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2F2=_tmp2D3;struct _dyneither_ptr*_tmp2F4;_LL190: if(_tmp2F2 == 0)goto _LL192;if(((struct _tuple15*)_tmp2F2->hd)->f1 == 0)goto _LL192;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2F3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2F2->hd)->f1)->hd);if(_tmp2F3->tag != 1)goto _LL192;else{_tmp2F4=_tmp2F3->f1;}};_LL191: {
# 1363
const char*_tmp556;void*_tmp555[3];struct Cyc_String_pa_PrintArg_struct _tmp554;struct Cyc_String_pa_PrintArg_struct _tmp553;struct Cyc_String_pa_PrintArg_struct _tmp552;return(struct _dyneither_ptr)((_tmp552.tag=0,((_tmp552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2C1)),((_tmp553.tag=0,((_tmp553.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2F4),((_tmp554.tag=0,((_tmp554.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1363
Cyc_Absynpp_qvar2string(_tmp2D2->name)),((_tmp555[0]=& _tmp554,((_tmp555[1]=& _tmp553,((_tmp555[2]=& _tmp552,Cyc_aprintf(((_tmp556="%s{.%s = %s}",_tag_dyneither(_tmp556,sizeof(char),13))),_tag_dyneither(_tmp555,sizeof(void*),3))))))))))))))))))));}_LL192:;_LL193:
# 1365
 goto _LL18F;_LL18F:;}{
# 1368
const char*_tmp55B;void*_tmp55A[2];struct Cyc_String_pa_PrintArg_struct _tmp559;struct Cyc_String_pa_PrintArg_struct _tmp558;return(struct _dyneither_ptr)((_tmp558.tag=0,((_tmp558.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C1)),((_tmp559.tag=0,((_tmp559.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D2->name)),((_tmp55A[0]=& _tmp559,((_tmp55A[1]=& _tmp558,Cyc_aprintf(((_tmp55B="%s{%s}",_tag_dyneither(_tmp55B,sizeof(char),7))),_tag_dyneither(_tmp55A,sizeof(void*),2))))))))))))));};_LL17D: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2D4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D4->tag != 7)goto _LL17F;else{_tmp2D5=_tmp2D4->f2;}}_LL17E:
# 1370
 if(_tmp2C1 == 0){
const char*_tmp55F;void*_tmp55E[1];struct Cyc_String_pa_PrintArg_struct _tmp55D;return(struct _dyneither_ptr)((_tmp55D.tag=0,((_tmp55D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D5->name)),((_tmp55E[0]=& _tmp55D,Cyc_aprintf(((_tmp55F="%s",_tag_dyneither(_tmp55F,sizeof(char),3))),_tag_dyneither(_tmp55E,sizeof(void*),1))))))));}else{
# 1373
const char*_tmp564;void*_tmp563[2];struct Cyc_String_pa_PrintArg_struct _tmp562;struct Cyc_String_pa_PrintArg_struct _tmp561;return(struct _dyneither_ptr)((_tmp561.tag=0,((_tmp561.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2C1)),((_tmp562.tag=0,((_tmp562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D5->name)),((_tmp563[0]=& _tmp562,((_tmp563[1]=& _tmp561,Cyc_aprintf(((_tmp564="%s(%s)",_tag_dyneither(_tmp564,sizeof(char),7))),_tag_dyneither(_tmp563,sizeof(void*),2))))))))))))));}_LL17F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2D6=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D6->tag != 8)goto _LL181;}_LL180: {
const char*_tmp565;return(_tmp565="NULL",_tag_dyneither(_tmp565,sizeof(char),5));}_LL181: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2D7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D7->tag != 9)goto _LL183;else{_tmp2D8=_tmp2D7->f2;}}_LL182: {
const char*_tmp569;void*_tmp568[1];struct Cyc_Int_pa_PrintArg_struct _tmp567;return(struct _dyneither_ptr)((_tmp567.tag=1,((_tmp567.f1=(unsigned long)_tmp2D8,((_tmp568[0]=& _tmp567,Cyc_aprintf(((_tmp569="%d",_tag_dyneither(_tmp569,sizeof(char),3))),_tag_dyneither(_tmp568,sizeof(void*),1))))))));}_LL183: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2D9=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2D9->tag != 10)goto _LL185;else{_tmp2DA=_tmp2D9->f1;}}_LL184: {
const char*_tmp56D;void*_tmp56C[1];struct Cyc_Int_pa_PrintArg_struct _tmp56B;return(struct _dyneither_ptr)((_tmp56B.tag=1,((_tmp56B.f1=(unsigned long)((int)_tmp2DA),((_tmp56C[0]=& _tmp56B,Cyc_aprintf(((_tmp56D="%d",_tag_dyneither(_tmp56D,sizeof(char),3))),_tag_dyneither(_tmp56C,sizeof(void*),1))))))));}_LL185: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2DB=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2DB->tag != 11)goto _LL187;else{_tmp2DC=_tmp2DB->f1;_tmp2DD=_tmp2DB->f2;}}_LL186:
 return _tmp2DC;_LL187: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2DE=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2DE->tag != 12)goto _LL189;else{_tmp2DF=_tmp2DE->f2;}}_LL188:
 _tmp2E1=_tmp2DF;goto _LL18A;_LL189: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2E0=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2E0->tag != 13)goto _LL18B;else{_tmp2E1=_tmp2E0->f2;}}_LL18A:
 return Cyc_Absynpp_qvar2string(_tmp2E1->name);_LL18B: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2E2=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2C6;if(_tmp2E2->tag != 16)goto _LL18D;else{_tmp2E3=_tmp2E2->f1;}}_LL18C:
 return Cyc_Absynpp_exp2string(_tmp2E3);_LL18D:;_LL18E:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL16E:;};}_LL16C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2C2=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2BE;if(_tmp2C2->tag != 1)goto _LL169;else{_tmp2C3=_tmp2C2->f1;}}_LL16D:
# 1384
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2C3)){const char*_tmp56E;return(_tmp56E="_",_tag_dyneither(_tmp56E,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp30D=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2C3);
struct Cyc_Absyn_Pat*_tmp30E=((struct Cyc_Tcpat_Con_s*)_check_null(_tmp30D))->orig_pat;
# 1389
if(_tmp30E == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_stmttmp22=Cyc_Tcutil_compress((void*)_check_null(_tmp30E->topt));void*_tmp30F=_stmttmp22;struct Cyc_Absyn_PtrAtts _tmp313;struct Cyc_Absyn_Datatypedecl*_tmp315;struct Cyc_Absyn_Aggrdecl*_tmp317;_LL195: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp30F;if(_tmp310->tag != 6)goto _LL197;else{if(_tmp310->f2 != Cyc_Absyn_Char_sz)goto _LL197;}}_LL196:
# 1393
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp318=Cyc_Tcpat_char_con(r,(char)i,(struct Cyc_Absyn_Pat*)_check_null(_tmp30E));
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C3,_tmp318)){
const char*_tmp572;void*_tmp571[1];struct Cyc_Int_pa_PrintArg_struct _tmp570;return(struct _dyneither_ptr)((_tmp570.tag=1,((_tmp570.f1=(unsigned long)i,((_tmp571[0]=& _tmp570,Cyc_aprintf(((_tmp572="%d",_tag_dyneither(_tmp572,sizeof(char),3))),_tag_dyneither(_tmp571,sizeof(void*),1))))))));}}}
# 1398
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL197: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp311=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp30F;if(_tmp311->tag != 6)goto _LL199;}_LL198:
# 1401
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp31C=Cyc_Tcpat_int_con(r,(int)i,_tmp30E);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C3,_tmp31C)){
const char*_tmp576;void*_tmp575[1];struct Cyc_Int_pa_PrintArg_struct _tmp574;return(struct _dyneither_ptr)((_tmp574.tag=1,((_tmp574.f1=(unsigned long)((int)i),((_tmp575[0]=& _tmp574,Cyc_aprintf(((_tmp576="%d",_tag_dyneither(_tmp576,sizeof(char),3))),_tag_dyneither(_tmp575,sizeof(void*),1))))))));}}}
# 1406
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL199: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp312=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30F;if(_tmp312->tag != 5)goto _LL19B;else{_tmp313=(_tmp312->f1).ptr_atts;}}_LL19A: {
# 1408
union Cyc_Absyn_Constraint*_tmp320=_tmp313.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp320);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C3,Cyc_Tcpat_null_con(r,_tmp30E))){
const char*_tmp577;return(_tmp577="NULL",_tag_dyneither(_tmp577,sizeof(char),5));}}{
# 1414
const char*_tmp578;return(_tmp578="&_",_tag_dyneither(_tmp578,sizeof(char),3));};}_LL19B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp30F;if(_tmp314->tag != 3)goto _LL19D;else{if((((_tmp314->f1).datatype_info).KnownDatatype).tag != 2)goto _LL19D;_tmp315=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp314->f1).datatype_info).KnownDatatype).val);}}_LL19C:
# 1416
 if(_tmp315->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp323=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Datatypedecl*)_check_null(_tmp315))->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp323);
for(0;(unsigned int)_tmp323;_tmp323=_tmp323->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp323->hd)->name).f2;
struct Cyc_List_List*_tmp324=((struct Cyc_Absyn_Datatypefield*)_check_null((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp323))->hd))->typs;
struct Cyc_Tcpat_Con_s*_tmp579;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C3,((_tmp579=_cycalloc(sizeof(*_tmp579)),((_tmp579->name=Cyc_Tcpat_Name_v(n),((_tmp579->arity=0,((_tmp579->span=0,((_tmp579->orig_pat=_tmp30E,_tmp579)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp324)== 0)
return n;else{
# 1426
const char*_tmp57D;void*_tmp57C[1];struct Cyc_String_pa_PrintArg_struct _tmp57B;return(struct _dyneither_ptr)((_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp57C[0]=& _tmp57B,Cyc_aprintf(((_tmp57D="%s(...)",_tag_dyneither(_tmp57D,sizeof(char),8))),_tag_dyneither(_tmp57C,sizeof(void*),1))))))));}}}
# 1429
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL19D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp30F;if(_tmp316->tag != 11)goto _LL19F;else{if((((_tmp316->f1).aggr_info).KnownAggr).tag != 2)goto _LL19F;_tmp317=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp316->f1).aggr_info).KnownAggr).val);}}if(!(_tmp317->kind == Cyc_Absyn_UnionA))goto _LL19F;_LL19E: {
# 1431
struct Cyc_List_List*_tmp329=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp317))->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp329);
struct _tuple2*_stmttmp23=_tmp317->name;struct _dyneither_ptr _tmp32B;struct _tuple2*_tmp32A=_stmttmp23;_tmp32B=*_tmp32A->f2;
for(0;(unsigned int)_tmp329;_tmp329=_tmp329->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp329->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp57E;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2C3,((_tmp57E=_cycalloc(sizeof(*_tmp57E)),((_tmp57E->name=Cyc_Tcpat_Name_v(n),((_tmp57E->arity=0,((_tmp57E->span=0,((_tmp57E->orig_pat=_tmp30E,_tmp57E)))))))))))){
const char*_tmp583;void*_tmp582[2];struct Cyc_String_pa_PrintArg_struct _tmp581;struct Cyc_String_pa_PrintArg_struct _tmp580;return(struct _dyneither_ptr)((_tmp580.tag=0,((_tmp580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp581.tag=0,((_tmp581.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32B),((_tmp582[0]=& _tmp581,((_tmp582[1]=& _tmp580,Cyc_aprintf(((_tmp583="%s{.%s = _}",_tag_dyneither(_tmp583,sizeof(char),12))),_tag_dyneither(_tmp582,sizeof(void*),2))))))))))))));}}
# 1439
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL194:;};};_LL169:;}
# 1444
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp584;return(_tmp584=_cycalloc(sizeof(*_tmp584)),((_tmp584[0]=Cyc_Tcpat_desc2string(r,d),_tmp584)));}
# 1447
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp332=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp333=_tmp332;for(0;_tmp333 != 0;_tmp333=((struct Cyc_List_List*)_check_null(_tmp333))->tl){
if(_tmp333->tl != 0){
{struct Cyc_List_List*_tmp585;_tmp333->tl=((_tmp585=_cycalloc(sizeof(*_tmp585)),((_tmp585->hd=comma,((_tmp585->tl=_tmp333->tl,_tmp585))))));}
_tmp333=_tmp333->tl;}}}
# 1456
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp332);}
# 1459
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp336;_push_handler(& _tmp336);{int _tmp338=0;if(setjmp(_tmp336.handler))_tmp338=1;if(!_tmp338){
{struct _dyneither_ptr _tmp339=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp589;void*_tmp588[1];struct Cyc_String_pa_PrintArg_struct _tmp587;(_tmp587.tag=0,((_tmp587.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp339),((_tmp588[0]=& _tmp587,Cyc_Tcutil_terr(loc,((_tmp589="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp589,sizeof(char),53))),_tag_dyneither(_tmp588,sizeof(void*),1)))))));}
# 1461
;_pop_handler();}else{void*_tmp337=(void*)_exn_thrown;void*_tmp33E=_tmp337;void*_tmp340;_LL1A2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp33F=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp33E;if(_tmp33F->tag != Cyc_Tcpat_Desc2string)goto _LL1A4;}_LL1A3:
# 1465
{const char*_tmp58C;void*_tmp58B;(_tmp58B=0,Cyc_Tcutil_terr(loc,((_tmp58C="patterns may not be exhaustive.",_tag_dyneither(_tmp58C,sizeof(char),32))),_tag_dyneither(_tmp58B,sizeof(void*),0)));}
goto _LL1A1;_LL1A4: _tmp340=_tmp33E;_LL1A5:(void)_throw(_tmp340);_LL1A1:;}};}
# 1469
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple20*rhs){
(*rhs).f1=1;}
# 1473
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1478
struct _RegionHandle _tmp343=_new_region("r");struct _RegionHandle*r=& _tmp343;_push_region(r);
{int _tmp344=0;
struct _tuple24 _tmp58D;struct _tuple24 _tmp345=(_tmp58D.f1=r,((_tmp58D.f2=& _tmp344,_tmp58D)));
struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _tuple24*,struct Cyc_Absyn_Switch_clause*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp345,swcs);
void*_tmp347=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp346);
int _tmp348=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp347,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp348);
# 1486
for(0;_tmp346 != 0;_tmp346=_tmp346->tl){
struct _tuple23*_stmttmp25=(struct _tuple23*)_tmp346->hd;int _tmp34A;unsigned int _tmp34B;struct _tuple23*_tmp349=_stmttmp25;_tmp34A=(_tmp349->f2)->f1;_tmp34B=(_tmp349->f2)->f2;
if(!_tmp34A){
{const char*_tmp590;void*_tmp58F;(_tmp58F=0,Cyc_Tcutil_terr(_tmp34B,((_tmp590="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp590,sizeof(char),74))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}
# 1491
break;}}}
# 1479
;_pop_region(r);}
# 1501
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp34F;_push_handler(& _tmp34F);{int _tmp351=0;if(setjmp(_tmp34F.handler))_tmp351=1;if(!_tmp351){
{struct _dyneither_ptr _tmp352=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp594;void*_tmp593[1];struct Cyc_String_pa_PrintArg_struct _tmp592;(_tmp592.tag=0,((_tmp592.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352),((_tmp593[0]=& _tmp592,Cyc_Tcutil_warn((*pr).f1,((_tmp594="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp594,sizeof(char),45))),_tag_dyneither(_tmp593,sizeof(void*),1)))))));}
# 1504
;_pop_handler();}else{void*_tmp350=(void*)_exn_thrown;void*_tmp357=_tmp350;void*_tmp359;_LL1A7: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp358=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp357;if(_tmp358->tag != Cyc_Tcpat_Desc2string)goto _LL1A9;}_LL1A8:
# 1508
{const char*_tmp597;void*_tmp596;(_tmp596=0,Cyc_Tcutil_warn((*pr).f1,((_tmp597="pattern not exhaustive.",_tag_dyneither(_tmp597,sizeof(char),24))),_tag_dyneither(_tmp596,sizeof(void*),0)));}
goto _LL1A6;_LL1A9: _tmp359=_tmp357;_LL1AA:(void)_throw(_tmp359);_LL1A6:;}};};}
# 1512
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple25{void*f1;int f2;};
# 1515
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp35C=_new_region("r");struct _RegionHandle*r=& _tmp35C;_push_region(r);
{struct _tuple25*_tmp59A;struct Cyc_List_List*_tmp599;struct Cyc_List_List*_tmp35D=(_tmp599=_region_malloc(r,sizeof(*_tmp599)),((_tmp599->hd=((_tmp59A=_region_malloc(r,sizeof(*_tmp59A)),((_tmp59A->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp59A->f2=0,_tmp59A)))))),((_tmp599->tl=0,_tmp599)))));
void*_tmp35E=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp35D);
struct _tuple14 _tmp59B;struct _tuple14 _tmp35F=(_tmp59B.f1=loc,((_tmp59B.f2=1,_tmp59B)));
int _tmp360=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp35E,Cyc_Tcpat_not_exhaust_warn,& _tmp35F,Cyc_Tcpat_dummy_fn,0,& _tmp360);{
# 1523
int _tmp361=_tmp35F.f2;_npop_handler(0);return _tmp361;};}
# 1517
;_pop_region(r);}
# 1531
static struct _tuple23*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1533
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple20*_tmp59C;struct _tuple20*rhs=(_tmp59C=_region_malloc(r,sizeof(*_tmp59C)),((_tmp59C->f1=0,((_tmp59C->f2=(swc->pattern)->loc,_tmp59C)))));
struct _tuple23*_tmp59D;return(_tmp59D=_region_malloc(r,sizeof(*_tmp59D)),((_tmp59D->f1=sp0,((_tmp59D->f2=rhs,_tmp59D)))));}
# 1537
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1540
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1542
struct _RegionHandle _tmp367=_new_region("r");struct _RegionHandle*r=& _tmp367;_push_region(r);
{struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp369=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp368);
int _tmp36A=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp369,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp36A);
# 1548
for(0;_tmp368 != 0;_tmp368=_tmp368->tl){
struct _tuple23*_stmttmp26=(struct _tuple23*)_tmp368->hd;int _tmp36C;unsigned int _tmp36D;struct _tuple23*_tmp36B=_stmttmp26;_tmp36C=(_tmp36B->f2)->f1;_tmp36D=(_tmp36B->f2)->f2;
if(!_tmp36C){
{const char*_tmp5A0;void*_tmp59F;(_tmp59F=0,Cyc_Tcutil_terr(_tmp36D,((_tmp5A0="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5A0,sizeof(char),74))),_tag_dyneither(_tmp59F,sizeof(void*),0)));}
break;}}}
# 1543
;_pop_region(r);}
