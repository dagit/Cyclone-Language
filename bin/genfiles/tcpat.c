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
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 1096
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1105
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
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
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
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerceable(void*);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 113
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 154
int Cyc_Tcutil_unify(void*,void*);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple11{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 186
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
# 234 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 249
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 277
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 287
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 307
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 326
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct _tuple13{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple13*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 38 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 42
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct Cyc_List_List*_tmp5;int _tmp6;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;int _tmpA;enum Cyc_Absyn_AggrKind _tmpC;struct _tuple1*_tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List**_tmpF;struct Cyc_List_List*_tmp10;int _tmp11;struct Cyc_Absyn_Exp*_tmp13;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;else{_tmp2=_tmp1->f1;}}_LL2:
# 55
{struct _handler_cons _tmp14;_push_handler(& _tmp14);{int _tmp16=0;if(setjmp(_tmp14.handler))_tmp16=1;if(!_tmp16){
{struct _RegionHandle _tmp17=_new_region("r");struct _RegionHandle*r=& _tmp17;_push_region(r);
{void*_tmp18=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2,0);struct Cyc_Absyn_Datatypedecl*_tmp1B;struct Cyc_Absyn_Datatypefield*_tmp1C;struct Cyc_Absyn_Enumdecl*_tmp1E;struct Cyc_Absyn_Enumfield*_tmp1F;void*_tmp21;struct Cyc_Absyn_Enumfield*_tmp22;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp19=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp19->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp35D;void*_tmp35C;(_tmp35C=0,Cyc_Tcutil_terr(p->loc,((_tmp35D="struct tag used without arguments in pattern",_tag_dyneither(_tmp35D,sizeof(char),45))),_tag_dyneither(_tmp35C,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp360;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp35F;p->r=(void*)((_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F[0]=((_tmp360.tag=7,((_tmp360.f1=_tmp1B,((_tmp360.f2=_tmp1C,((_tmp360.f3=0,((_tmp360.f4=0,_tmp360)))))))))),_tmp35F))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp363;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp362;p->r=(void*)((_tmp362=_cycalloc(sizeof(*_tmp362)),((_tmp362[0]=((_tmp363.tag=12,((_tmp363.f1=_tmp1E,((_tmp363.f2=_tmp1F,_tmp363)))))),_tmp362))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp366;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp365;p->r=(void*)((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365[0]=((_tmp366.tag=13,((_tmp366.f1=_tmp21,((_tmp366.f2=_tmp22,_tmp366)))))),_tmp365))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp23=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp23->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp2D=_tmp15;void*_tmp2F;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 73
 goto _LL18;_LL1B: _tmp2F=_tmp2D;_LL1C:(void)_throw(_tmp2F);_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp30=(*_tmp2).f1;_LL1E: if((_tmp30.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp30.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL22;_LL21:
# 78
(*_tmp2).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp369;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp368;p->r=(void*)((_tmp368=_cycalloc(sizeof(*_tmp368)),((_tmp368[0]=((_tmp369.tag=1,((_tmp369.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp369.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp369)))))),_tmp368))));}
return;_LL22:;_LL23:
{const char*_tmp36C;void*_tmp36B;(_tmp36B=0,Cyc_Tcutil_terr(p->loc,((_tmp36C="qualified variable in pattern",_tag_dyneither(_tmp36C,sizeof(char),30))),_tag_dyneither(_tmp36B,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 85
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_tmp39=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp36F;struct Cyc_List_List*_tmp36E;_tmp42=((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E->hd=((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->f1=0,((_tmp36F->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp36F)))))),((_tmp36E->tl=_tmp42,_tmp36E))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp379;struct Cyc_Absyn_Aggrdecl**_tmp378;struct Cyc_Absyn_AggrInfo*_tmp377;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp376;p->r=(void*)((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp379.tag=6,((_tmp379.f1=((_tmp377=_cycalloc(sizeof(*_tmp377)),((_tmp377->aggr_info=Cyc_Absyn_KnownAggr(((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378[0]=_tmp3B,_tmp378))))),((_tmp377->targs=0,_tmp377)))))),((_tmp379.f2=0,((_tmp379.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp379.f4=_tmp6,_tmp379)))))))))),_tmp376))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp37C;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp37B;p->r=(void*)((_tmp37B=_cycalloc(sizeof(*_tmp37B)),((_tmp37B[0]=((_tmp37C.tag=7,((_tmp37C.f1=_tmp3D,((_tmp37C.f2=_tmp3E,((_tmp37C.f3=_tmp5,((_tmp37C.f4=_tmp6,_tmp37C)))))))))),_tmp37B))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp37F;void*_tmp37E;(_tmp37E=0,Cyc_Tcutil_terr(p->loc,((_tmp37F="enum tag used with arguments in pattern",_tag_dyneither(_tmp37F,sizeof(char),40))),_tag_dyneither(_tmp37E,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp383;void*_tmp382[1];struct Cyc_String_pa_PrintArg_struct _tmp381;(_tmp381.tag=0,((_tmp381.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp382[0]=& _tmp381,Cyc_Tcutil_terr(p->loc,((_tmp383="%s is not a constructor in pattern",_tag_dyneither(_tmp383,sizeof(char),35))),_tag_dyneither(_tmp382,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp386;void*_tmp385;(_tmp385=0,Cyc_Tcutil_terr(p->loc,((_tmp386="cannot determine pattern type",_tag_dyneither(_tmp386,sizeof(char),30))),_tag_dyneither(_tmp385,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp38C;struct Cyc_Absyn_AggrInfo*_tmp38B;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp38A;p->r=(void*)((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A[0]=((_tmp38C.tag=6,((_tmp38C.f1=((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B[0]=_tmp58,_tmp38B)))),((_tmp38C.f2=_tmp8,((_tmp38C.f3=_tmp9,((_tmp38C.f4=_tmpA,_tmp38C)))))))))),_tmp38A))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp390;void*_tmp38F[1];struct Cyc_String_pa_PrintArg_struct _tmp38E;(_tmp38E.tag=0,((_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp38F[0]=& _tmp38E,Cyc_Tcutil_terr(p->loc,((_tmp390="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp390,sizeof(char),45))),_tag_dyneither(_tmp38F,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 126
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp398;void*_tmp397[1];const char*_tmp396;const char*_tmp395;struct Cyc_String_pa_PrintArg_struct _tmp394;(_tmp394.tag=0,((_tmp394.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp395="struct",_tag_dyneither(_tmp395,sizeof(char),7)):((_tmp396="union",_tag_dyneither(_tmp396,sizeof(char),6)))),((_tmp397[0]=& _tmp394,Cyc_Tcutil_terr(p->loc,((_tmp398="can't destructure an abstract %s",_tag_dyneither(_tmp398,sizeof(char),33))),_tag_dyneither(_tmp397,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp39B;void*_tmp39A;(_tmp39A=0,Cyc_Tcutil_terr(p->loc,((_tmp39B="too many existentially bound type variables in pattern",_tag_dyneither(_tmp39B,sizeof(char),55))),_tag_dyneither(_tmp39A,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3A1;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3A0;struct Cyc_List_List*_tmp39F;_tmp6C=((_tmp39F=_cycalloc(sizeof(*_tmp39F)),((_tmp39F->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1[0]=((_tmp3A0.tag=1,((_tmp3A0.f1=0,_tmp3A0)))),_tmp3A1))))),((_tmp39F->tl=_tmp6C,_tmp39F))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3A7;struct Cyc_Absyn_AggrInfo*_tmp3A6;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A5;p->r=(void*)((_tmp3A5=_cycalloc(sizeof(*_tmp3A5)),((_tmp3A5[0]=((_tmp3A7.tag=6,((_tmp3A7.f1=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3A6->targs=_tmpE,_tmp3A6)))))),((_tmp3A7.f2=*_tmpF,((_tmp3A7.f3=_tmp10,((_tmp3A7.f4=_tmp11,_tmp3A7)))))))))),_tmp3A5))));};};}
# 127
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3AA;void*_tmp3A9;(_tmp3A9=0,Cyc_Tcutil_terr(p->loc,((_tmp3AA="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3AA,sizeof(char),43))),_tag_dyneither(_tmp3A9,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3AD;void*_tmp3AC;(_tmp3AC=0,Cyc_Tcutil_terr(p->loc,((_tmp3AD="non-constant expression in case pattern",_tag_dyneither(_tmp3AD,sizeof(char),40))),_tag_dyneither(_tmp3AC,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=Cyc_Evexp_eval_const_uint_exp(_tmp13);_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3B0;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3AF;p->r=(void*)((_tmp3AF=_cycalloc_atomic(sizeof(*_tmp3AF)),((_tmp3AF[0]=((_tmp3B0.tag=9,((_tmp3B0.f1=Cyc_Absyn_None,((_tmp3B0.f2=(int)_tmp7C,_tmp3B0)))))),_tmp3AF))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3B1;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1->v=s,_tmp3B1)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp81=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp81;if(_tmp82->tag != 13)goto _LL41;}_LL40:
 goto _LL42;_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp81;if(_tmp83->tag != 14)goto _LL43;}_LL42:
 if(topt != 0)return*topt;goto _LL3E;_LL43:;_LL44:
 goto _LL3E;_LL3E:;}
# 190
return numt;}
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,void*t){
vd->type=t;
vd->tq=Cyc_Absyn_empty_tqual(0);{
struct Cyc_List_List*_tmp3B2;*v_result_ptr=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->hd=vd,((_tmp3B2->tl=*v_result_ptr,_tmp3B2))))));};}
# 198
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 200
struct _tuple13*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_Tcpat_TcPatResult _tmp85=res1;_tmp86=_tmp85.tvars_and_bounds_opt;_tmp87=_tmp85.patvars;{
struct _tuple13*_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_Tcpat_TcPatResult _tmp88=res2;_tmp89=_tmp88.tvars_and_bounds_opt;_tmp8A=_tmp88.patvars;
if(_tmp86 != 0  || _tmp89 != 0){
if(_tmp86 == 0){
struct _tuple13*_tmp3B3;_tmp86=((_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3->f1=0,((_tmp3B3->f2=0,_tmp3B3))))));}
if(_tmp89 == 0){
struct _tuple13*_tmp3B4;_tmp89=((_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4->f1=0,((_tmp3B4->f2=0,_tmp3B4))))));}{
struct _tuple13*_tmp3B7;struct Cyc_Tcpat_TcPatResult _tmp3B6;return(_tmp3B6.tvars_and_bounds_opt=((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp86).f1,(*_tmp89).f1),((_tmp3B7->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp86).f2,(*_tmp89).f2),_tmp3B7)))))),((_tmp3B6.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp87,_tmp8A),_tmp3B6)));};}{
# 211
struct Cyc_Tcpat_TcPatResult _tmp3B8;return(_tmp3B8.tvars_and_bounds_opt=0,((_tmp3B8.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp87,_tmp8A),_tmp3B8)));};};}
# 214
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3B9;return(_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9->loc=loc,((_tmp3B9->topt=0,((_tmp3B9->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3B9)))))));}
# 218
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_tmp91=Cyc_Tcutil_compress(t);void*_tmp93;struct Cyc_Absyn_Tqual _tmp94;struct Cyc_Absyn_Exp*_tmp95;union Cyc_Absyn_Constraint*_tmp96;unsigned int _tmp97;_LL46: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp91;if(_tmp92->tag != 8)goto _LL48;else{_tmp93=(_tmp92->f1).elt_type;_tmp94=(_tmp92->f1).tq;_tmp95=(_tmp92->f1).num_elts;_tmp96=(_tmp92->f1).zero_term;_tmp97=(_tmp92->f1).zt_loc;}}_LL47: {
# 221
struct Cyc_Core_Opt*_tmp3BA;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3BA))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3BD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3BC;return Cyc_Absyn_atb_typ(_tmp93,rgn,_tmp94,(void*)((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC[0]=((_tmp3BD.tag=1,((_tmp3BD.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp95),_tmp3BD)))),_tmp3BC)))),_tmp96);}_LL48:;_LL49:
 return t;_LL45:;}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 227
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat){
# 229
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3BE;struct Cyc_Tcpat_TcPatResult res=(_tmp3BE.tvars_and_bounds_opt=0,((_tmp3BE.patvars=0,_tmp3BE)));
# 234
{void*_tmp9B=p->r;struct Cyc_Absyn_Vardecl*_tmp9E;struct Cyc_Absyn_Pat*_tmp9F;struct Cyc_Absyn_Vardecl*_tmpA1;struct Cyc_Absyn_Pat*_tmpA2;struct Cyc_Absyn_Tvar*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA5;int _tmpAB;struct Cyc_Absyn_Enumdecl*_tmpAD;void*_tmpAF;struct Cyc_Absyn_Pat*_tmpB2;struct Cyc_List_List**_tmpB4;int _tmpB5;struct Cyc_Absyn_Aggrdecl*_tmpB7;struct Cyc_List_List**_tmpB8;struct Cyc_List_List*_tmpB9;struct Cyc_List_List**_tmpBA;int _tmpBB;struct Cyc_Absyn_Datatypedecl*_tmpBD;struct Cyc_Absyn_Datatypefield*_tmpBE;struct Cyc_List_List**_tmpBF;int _tmpC0;_LL4B: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9C->tag != 0)goto _LL4D;}_LL4C:
# 237
 if(topt != 0)
t=*topt;else{
# 240
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL4A;_LL4D: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9D->tag != 1)goto _LL4F;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LL4E:
# 243
 res=Cyc_Tcpat_tcPatRec(te,_tmp9F,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmp9F->topt);
# 247
{void*_tmpC6=Cyc_Tcutil_compress(t);_LL76: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC6;if(_tmpC7->tag != 8)goto _LL78;}_LL77:
# 249
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp3C1;void*_tmp3C0;(_tmp3C0=0,Cyc_Tcutil_terr(p->loc,((_tmp3C1="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3C1,sizeof(char),58))),_tag_dyneither(_tmp3C0,sizeof(void*),0)));}
goto _LL75;_LL78:;_LL79:
# 253
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3C4;void*_tmp3C3;(_tmp3C3=0,Cyc_Tcutil_terr(p->loc,((_tmp3C4="pattern would point to an abstract member",_tag_dyneither(_tmp3C4,sizeof(char),42))),_tag_dyneither(_tmp3C3,sizeof(void*),0)));}
goto _LL75;_LL75:;}
# 257
Cyc_Tcpat_set_vd(_tmp9E,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));
goto _LL4A;_LL4F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA0=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA0->tag != 2)goto _LL51;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;}}_LL50:
# 260
 res=Cyc_Tcpat_tcPatRec(te,_tmpA2,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmpA2->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3C7;void*_tmp3C6;(_tmp3C6=0,Cyc_Tcutil_terr(p->loc,((_tmp3C7="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3C7,sizeof(char),55))),_tag_dyneither(_tmp3C6,sizeof(void*),0)));}
goto _LL4A;}else{
# 266
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp3CA;void*_tmp3C9;(_tmp3C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3CA="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3CA,sizeof(char),56))),_tag_dyneither(_tmp3C9,sizeof(void*),0)));}}
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3D0;struct Cyc_Absyn_PtrInfo _tmp3CF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3CE;Cyc_Tcpat_set_vd(_tmpA1,& res.patvars,(void*)(
(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=((_tmp3D0.tag=5,((_tmp3D0.f1=((_tmp3CF.elt_typ=t,((_tmp3CF.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3CF.ptr_atts=(
((_tmp3CF.ptr_atts).rgn=rgn_pat,(((_tmp3CF.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3CF.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3CF.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3CF.ptr_atts).ptrloc=0,_tmp3CF.ptr_atts)))))))))),_tmp3CF)))))),_tmp3D0)))),_tmp3CE)))));}
# 273
goto _LL4A;_LL51: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA3=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA3->tag != 3)goto _LL53;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;}}_LL52:
# 278
 Cyc_Tcpat_set_vd(_tmpA5,& res.patvars,_tmpA5->type);{
# 282
struct _RegionHandle _tmpD3=_new_region("r2");struct _RegionHandle*r2=& _tmpD3;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3D1[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3D1[0]=_tmpA4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3D1,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 285
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp3D2;res.tvars_and_bounds_opt=((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->f1=0,((_tmp3D2->f2=0,_tmp3D2))))));}
{struct Cyc_List_List*_tmp3D3;(*res.tvars_and_bounds_opt).f1=(
(_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->hd=_tmpA4,((_tmp3D3->tl=(*res.tvars_and_bounds_opt).f1,_tmp3D3))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL4A;
# 282
;_pop_region(r2);};_LL53: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA6->tag != 9)goto _LL55;else{if(_tmpA6->f1 != Cyc_Absyn_Unsigned)goto _LL55;}}_LL54:
# 292
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL4A;_LL55: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA7->tag != 9)goto _LL57;else{if(_tmpA7->f1 != Cyc_Absyn_None)goto _LL57;}}_LL56:
 goto _LL58;_LL57: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA8->tag != 9)goto _LL59;else{if(_tmpA8->f1 != Cyc_Absyn_Signed)goto _LL59;}}_LL58:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL4A;_LL59: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA9->tag != 10)goto _LL5B;}_LL5A:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL4A;_LL5B: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpAA=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAA->tag != 11)goto _LL5D;else{_tmpAB=_tmpAA->f2;}}_LL5C:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAB));goto _LL4A;_LL5D: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAC=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAC->tag != 12)goto _LL5F;else{_tmpAD=_tmpAC->f1;}}_LL5E:
# 298
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3D6;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3D5;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=((_tmp3D6.tag=13,((_tmp3D6.f1=_tmpAD->name,((_tmp3D6.f2=_tmpAD,_tmp3D6)))))),_tmp3D5)))));}
goto _LL4A;_LL5F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAE=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAE->tag != 13)goto _LL61;else{_tmpAF=(void*)_tmpAE->f1;}}_LL60:
 t=Cyc_Tcpat_num_type(topt,_tmpAF);goto _LL4A;_LL61: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB0=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB0->tag != 8)goto _LL63;}_LL62:
# 302
 if(topt != 0){
void*_tmpD9=Cyc_Tcutil_compress(*topt);_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9;if(_tmpDA->tag != 5)goto _LL7D;}_LL7C:
 t=*topt;goto tcpat_end;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 307
struct Cyc_Core_Opt*_tmpDB=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3DC;struct Cyc_Absyn_PtrInfo _tmp3DB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3DA;t=(void*)((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA[0]=((_tmp3DC.tag=5,((_tmp3DC.f1=((_tmp3DB.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpDB),((_tmp3DB.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3DB.ptr_atts=(
# 312
((_tmp3DB.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDB),(((_tmp3DB.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp3DB.ptr_atts).bounds=
# 314
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3DB.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3DB.ptr_atts).ptrloc=0,_tmp3DB.ptr_atts)))))))))),_tmp3DB)))))),_tmp3DC)))),_tmp3DA))));}
# 317
goto _LL4A;};_LL63: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB1->tag != 5)goto _LL65;else{_tmpB2=_tmpB1->f1;}}_LL64: {
# 323
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpDF=0;
int elt_const=0;
if(topt != 0){
void*_tmpE0=Cyc_Tcutil_compress(*topt);void*_tmpE2;struct Cyc_Absyn_Tqual _tmpE3;_LL80: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE0;if(_tmpE1->tag != 5)goto _LL82;else{_tmpE2=(_tmpE1->f1).elt_typ;_tmpE3=(_tmpE1->f1).elt_tq;}}_LL81:
# 329
 inner_typ=_tmpE2;
_tmpDF=& inner_typ;
elt_const=_tmpE3.real_const;
goto _LL7F;_LL82:;_LL83:
 goto _LL7F;_LL7F:;}{
# 338
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB2,_tmpDF,ptr_rgn,1));
# 344
{void*_tmpE4=Cyc_Tcutil_compress((void*)_check_null(_tmpB2->topt));struct Cyc_Absyn_Datatypedecl*_tmpE6;struct Cyc_Absyn_Datatypefield*_tmpE7;struct Cyc_List_List*_tmpE8;_LL85: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE4;if(_tmpE5->tag != 4)goto _LL87;else{if((((_tmpE5->f1).field_info).KnownDatatypefield).tag != 2)goto _LL87;_tmpE6=((struct _tuple2)(((_tmpE5->f1).field_info).KnownDatatypefield).val).f1;_tmpE7=((struct _tuple2)(((_tmpE5->f1).field_info).KnownDatatypefield).val).f2;_tmpE8=(_tmpE5->f1).targs;}}_LL86:
# 348
{void*_tmpE9=Cyc_Tcutil_compress(inner_typ);_LL8A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE9;if(_tmpEA->tag != 4)goto _LL8C;}_LL8B:
# 350
 goto DONT_PROMOTE;_LL8C:;_LL8D:
 goto _LL89;_LL89:;}{
# 354
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp3E6;struct Cyc_Absyn_Datatypedecl**_tmp3E5;struct Cyc_Absyn_DatatypeInfo _tmp3E4;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3E3;void*new_type=(void*)((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=((_tmp3E6.tag=3,((_tmp3E6.f1=((_tmp3E4.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5[0]=_tmpE6,_tmp3E5))))),((_tmp3E4.targs=_tmpE8,_tmp3E4)))),_tmp3E6)))),_tmp3E3))));
# 357
_tmpB2->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3EC;struct Cyc_Absyn_PtrInfo _tmp3EB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3EA;t=(void*)((_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA[0]=((_tmp3EC.tag=5,((_tmp3EC.f1=((_tmp3EB.elt_typ=new_type,((_tmp3EB.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3EB.ptr_atts=(
((_tmp3EB.ptr_atts).rgn=ptr_rgn,(((_tmp3EB.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EB.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp3EB.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp3EB.ptr_atts).ptrloc=0,_tmp3EB.ptr_atts)))))))))),_tmp3EB)))))),_tmp3EC)))),_tmp3EA))));}
# 364
goto _LL84;};_LL87:;_LL88:
# 366
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3F2;struct Cyc_Absyn_PtrInfo _tmp3F1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0;t=(void*)((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0[0]=((_tmp3F2.tag=5,((_tmp3F2.f1=((_tmp3F1.elt_typ=(void*)_check_null(_tmpB2->topt),((_tmp3F1.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3F1.ptr_atts=(
((_tmp3F1.ptr_atts).rgn=ptr_rgn,(((_tmp3F1.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F1.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F1.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F1.ptr_atts).ptrloc=0,_tmp3F1.ptr_atts)))))))))),_tmp3F1)))))),_tmp3F2)))),_tmp3F0))));}_LL84:;}
# 374
goto _LL4A;};}_LL65: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB3=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB3->tag != 4)goto _LL67;else{_tmpB4=(struct Cyc_List_List**)& _tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL66: {
# 377
struct Cyc_List_List*_tmpF5=*_tmpB4;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpF6=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpF8;_LL8F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 10)goto _LL91;else{_tmpF8=_tmpF7->f1;}}_LL90:
# 383
 topt_ts=_tmpF8;
if(_tmpB5){
# 386
int _tmpF9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF5);
int _tmpFA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF8);
if(_tmpF9 < _tmpFA){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpFA - _tmpF9;++ i){
struct Cyc_List_List*_tmp3F3;wild_ps=((_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp3F3->tl=wild_ps,_tmp3F3))))));}}
*_tmpB4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF5,wild_ps);
_tmpF5=*_tmpB4;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF5)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF8)){
const char*_tmp3F6;void*_tmp3F5;(_tmp3F5=0,Cyc_Tcutil_warn(p->loc,((_tmp3F6="unnecessary ... in tuple pattern",_tag_dyneither(_tmp3F6,sizeof(char),33))),_tag_dyneither(_tmp3F5,sizeof(void*),0)));}}}
# 398
goto _LL8E;_LL91:;_LL92:
# 400
 goto _LL8E;_LL8E:;}else{
# 402
if(_tmpB5){
const char*_tmp3F9;void*_tmp3F8;(_tmp3F8=0,Cyc_Tcutil_terr(p->loc,((_tmp3F9="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp3F9,sizeof(char),57))),_tag_dyneither(_tmp3F8,sizeof(void*),0)));}}
for(0;_tmpF5 != 0;_tmpF5=_tmpF5->tl){
void**_tmp100=0;
if(topt_ts != 0){
_tmp100=&(*((struct _tuple16*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}
# 410
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpF5->hd,_tmp100,rgn_pat,allow_ref_pat));{
struct _tuple16*_tmp3FC;struct Cyc_List_List*_tmp3FB;pat_ts=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->f1=Cyc_Absyn_empty_tqual(0),((_tmp3FC->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpF5->hd)->topt),_tmp3FC)))))),((_tmp3FB->tl=pat_ts,_tmp3FB))))));};}
# 413
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp3FF;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3FE;t=(void*)((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE[0]=((_tmp3FF.tag=10,((_tmp3FF.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp3FF)))),_tmp3FE))));}
goto _LL4A;}_LL67: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB6=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB6->tag != 6)goto _LL69;else{if(_tmpB6->f1 == 0)goto _LL69;if((((_tmpB6->f1)->aggr_info).KnownAggr).tag != 2)goto _LL69;_tmpB7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB6->f1)->aggr_info).KnownAggr).val);_tmpB8=(struct Cyc_List_List**)&(_tmpB6->f1)->targs;_tmpB9=_tmpB6->f2;_tmpBA=(struct Cyc_List_List**)& _tmpB6->f3;_tmpBB=_tmpB6->f4;}}_LL68: {
# 417
struct Cyc_List_List*_tmp105=*_tmpBA;
const char*_tmp401;const char*_tmp400;struct _dyneither_ptr aggr_str=_tmpB7->kind == Cyc_Absyn_StructA?(_tmp401="struct",_tag_dyneither(_tmp401,sizeof(char),7)):((_tmp400="union",_tag_dyneither(_tmp400,sizeof(char),6)));
if(_tmpB7->impl == 0){
{const char*_tmp405;void*_tmp404[1];struct Cyc_String_pa_PrintArg_struct _tmp403;(_tmp403.tag=0,((_tmp403.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp404[0]=& _tmp403,Cyc_Tcutil_terr(p->loc,((_tmp405="can't destructure an abstract %s",_tag_dyneither(_tmp405,sizeof(char),33))),_tag_dyneither(_tmp404,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL4A;}
# 425
if(_tmpB7->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->tagged)
allow_ref_pat=0;
if(_tmpB9 != 0){
# 429
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 432
struct _RegionHandle _tmp109=_new_region("rgn");struct _RegionHandle*rgn=& _tmp109;_push_region(rgn);{
# 434
struct Cyc_List_List*_tmp10A=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp10B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpB9;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp10B))->hd;
_tmp10B=_tmp10B->tl;{
void*_tmp10C=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp10D=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp10E=_tmp10D;struct Cyc_Absyn_Kind*_tmp110;struct Cyc_Absyn_Kind*_tmp112;_LL94: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp10F->tag != 2)goto _LL96;else{_tmp110=_tmp10F->f2;}}_LL95:
 _tmp112=_tmp110;goto _LL97;_LL96: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp111=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp10E;if(_tmp111->tag != 0)goto _LL98;else{_tmp112=_tmp111->f1;}}_LL97:
 k2=_tmp112;goto _LL93;_LL98:;_LL99: {
const char*_tmp408;void*_tmp407;(_tmp407=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp408="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp408,sizeof(char),53))),_tag_dyneither(_tmp407,sizeof(void*),0)));}_LL93:;}
# 450
{void*_tmp115=_tmp10C;struct Cyc_Absyn_Kind*_tmp117;struct Cyc_Core_Opt**_tmp119;struct Cyc_Absyn_Kind*_tmp11A;struct Cyc_Core_Opt**_tmp11C;_LL9B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp116=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp116->tag != 0)goto _LL9D;else{_tmp117=_tmp116->f1;}}_LL9C:
# 453
 if(!Cyc_Tcutil_kind_leq(k2,_tmp117))
error=1;
goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp118=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp118->tag != 2)goto _LL9F;else{_tmp119=(struct Cyc_Core_Opt**)& _tmp118->f1;_tmp11A=_tmp118->f2;}}_LL9E:
 _tmp11C=_tmp119;goto _LLA0;_LL9F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp11B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp115;if(_tmp11B->tag != 1)goto _LL9A;else{_tmp11C=(struct Cyc_Core_Opt**)& _tmp11B->f1;}}_LLA0:
{struct Cyc_Core_Opt*_tmp409;*_tmp11C=((_tmp409=_cycalloc(sizeof(*_tmp409)),((_tmp409->v=_tmp10D,_tmp409))));}goto _LL9A;_LL9A:;}
# 459
if(error){
const char*_tmp40F;void*_tmp40E[3];struct Cyc_String_pa_PrintArg_struct _tmp40D;struct Cyc_String_pa_PrintArg_struct _tmp40C;struct Cyc_String_pa_PrintArg_struct _tmp40B;(_tmp40B.tag=0,((_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 463
Cyc_Absynpp_kind2string(k2)),((_tmp40C.tag=0,((_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp10C)),((_tmp40D.tag=0,((_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp40E[0]=& _tmp40D,((_tmp40E[1]=& _tmp40C,((_tmp40E[2]=& _tmp40B,Cyc_Tcutil_terr(p->loc,((_tmp40F="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp40F,sizeof(char),60))),_tag_dyneither(_tmp40E,sizeof(void*),3)))))))))))))))))));}{
# 465
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp412;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp411;void*vartype=(void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp412.tag=2,((_tmp412.f1=tv,_tmp412)))),_tmp411))));
{struct Cyc_List_List*_tmp413;_tmp10A=((_tmp413=_region_malloc(rgn,sizeof(*_tmp413)),((_tmp413->hd=vartype,((_tmp413->tl=_tmp10A,_tmp413))))));}
# 469
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp416;struct Cyc_List_List*_tmp415;outlives_constraints=(
(_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415->hd=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416->f1=Cyc_Absyn_empty_effect,((_tmp416->f2=vartype,_tmp416)))))),((_tmp415->tl=outlives_constraints,_tmp415))))));}else{
# 474
const char*_tmp419;void*_tmp418;(_tmp418=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp419="opened existential had unique or top region kind",_tag_dyneither(_tmp419,sizeof(char),49))),_tag_dyneither(_tmp418,sizeof(void*),0)));}}};};}}
# 478
_tmp10A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10A);{
# 480
struct _RegionHandle _tmp12A=_new_region("r2");struct _RegionHandle*r2=& _tmp12A;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpB9);
# 483
struct Cyc_List_List*_tmp12B=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple11 _tmp41A;struct _tuple11 _tmp12C=(_tmp41A.f1=_tmp12B,((_tmp41A.f2=rgn,_tmp41A)));
struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp12C,_tmpB7->tvs);
struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->exist_vars,_tmp10A);
struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp12D);
struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp12E);
struct Cyc_List_List*_tmp131=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp12D,_tmp12E);
# 491
if(_tmpB9 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp41B;res.tvars_and_bounds_opt=((_tmp41B=_cycalloc(sizeof(*_tmp41B)),((_tmp41B->f1=0,((_tmp41B->f2=0,_tmp41B))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpB9);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp133=0;
{struct Cyc_List_List*_tmp134=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->rgn_po;for(0;_tmp134 != 0;_tmp134=_tmp134->tl){
struct _tuple0*_tmp41E;struct Cyc_List_List*_tmp41D;_tmp133=((_tmp41D=_cycalloc(sizeof(*_tmp41D)),((_tmp41D->hd=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp131,(*((struct _tuple0*)_tmp134->hd)).f1),((_tmp41E->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp131,(*((struct _tuple0*)_tmp134->hd)).f2),_tmp41E)))))),((_tmp41D->tl=_tmp133,_tmp41D))))));}}
# 503
_tmp133=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp133);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp133);};}
# 508
*_tmpB8=_tmp12F;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp428;struct Cyc_Absyn_Aggrdecl**_tmp427;struct Cyc_Absyn_AggrInfo _tmp426;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp425;t=(void*)((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425[0]=((_tmp428.tag=11,((_tmp428.f1=((_tmp426.aggr_info=Cyc_Absyn_KnownAggr(((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427[0]=_tmpB7,_tmp427))))),((_tmp426.targs=*_tmpB8,_tmp426)))),_tmp428)))),_tmp425))));}
if(_tmpBB  && _tmpB7->kind == Cyc_Absyn_UnionA){
const char*_tmp42B;void*_tmp42A;(_tmp42A=0,Cyc_Tcutil_warn(p->loc,((_tmp42B="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp42B,sizeof(char),43))),_tag_dyneither(_tmp42A,sizeof(void*),0)));}else{
if(_tmpBB){
# 514
int _tmp13D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp105);
int _tmp13E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->fields);
if(_tmp13D < _tmp13E){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp13E - _tmp13D;++ i){
struct _tuple15*_tmp42E;struct Cyc_List_List*_tmp42D;wild_dps=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->hd=((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->f1=0,((_tmp42E->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp42E)))))),((_tmp42D->tl=wild_dps,_tmp42D))))));}}
*_tmpBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp105,wild_dps);
_tmp105=*_tmpBA;}else{
if(_tmp13D == _tmp13E){
const char*_tmp431;void*_tmp430;(_tmp430=0,Cyc_Tcutil_warn(p->loc,((_tmp431="unnecessary ... in struct pattern",_tag_dyneither(_tmp431,sizeof(char),34))),_tag_dyneither(_tmp430,sizeof(void*),0)));}}}}{
# 525
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp105,_tmpB7->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB7->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp144;struct Cyc_Absyn_Pat*_tmp145;struct _tuple17*_tmp143=(struct _tuple17*)fields->hd;_tmp144=_tmp143->f1;_tmp145=_tmp143->f2;{
void*_tmp146=Cyc_Tcutil_rsubstitute(rgn,_tmp131,_tmp144->type);
# 531
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp145,& _tmp146,rgn_pat,allow_ref_pat));
# 535
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp145->topt),_tmp146)){
const char*_tmp438;void*_tmp437[4];struct Cyc_String_pa_PrintArg_struct _tmp436;struct Cyc_String_pa_PrintArg_struct _tmp435;struct Cyc_String_pa_PrintArg_struct _tmp434;struct Cyc_String_pa_PrintArg_struct _tmp433;(_tmp433.tag=0,((_tmp433.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 538
Cyc_Absynpp_typ2string((void*)_check_null(_tmp145->topt))),((_tmp434.tag=0,((_tmp434.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 537
Cyc_Absynpp_typ2string(_tmp146)),((_tmp435.tag=0,((_tmp435.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp436.tag=0,((_tmp436.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp144->name),((_tmp437[0]=& _tmp436,((_tmp437[1]=& _tmp435,((_tmp437[2]=& _tmp434,((_tmp437[3]=& _tmp433,Cyc_Tcutil_terr(p->loc,((_tmp438="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp438,sizeof(char),45))),_tag_dyneither(_tmp437,sizeof(void*),4)))))))))))))))))))))))));}};}};}
# 481
;_pop_region(r2);};}
# 542
_npop_handler(0);goto _LL4A;
# 432
;_pop_region(rgn);};}_LL69: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBC=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpBC->tag != 7)goto _LL6B;else{_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=(struct Cyc_List_List**)& _tmpBC->f3;_tmpC0=_tmpBC->f4;}}_LL6A: {
# 545
struct Cyc_List_List*_tmp150=*_tmpBF;
struct _RegionHandle _tmp151=_new_region("rgn");struct _RegionHandle*rgn=& _tmp151;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBE->typs;
# 549
struct Cyc_List_List*_tmp152=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple11 _tmp439;struct _tuple11 _tmp153=(_tmp439.f1=_tmp152,((_tmp439.f2=rgn,_tmp439)));
struct Cyc_List_List*_tmp154=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp153,_tmpBD->tvs);
struct Cyc_List_List*_tmp155=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp154);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp43F;struct Cyc_Absyn_DatatypeFieldInfo _tmp43E;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp43D;t=(void*)((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D[0]=((_tmp43F.tag=4,((_tmp43F.f1=((_tmp43E.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBD,_tmpBE),((_tmp43E.targs=_tmp155,_tmp43E)))),_tmp43F)))),_tmp43D))));}
# 555
if(_tmpC0){
# 557
int _tmp159=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp150);
int _tmp15A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp159 < _tmp15A){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp15A - _tmp159;++ i){
struct Cyc_List_List*_tmp440;wild_ps=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp440->tl=wild_ps,_tmp440))))));}}
*_tmpBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp150,wild_ps);
_tmp150=*_tmpBF;}else{
if(_tmp159 == _tmp15A){
const char*_tmp444;void*_tmp443[1];struct Cyc_String_pa_PrintArg_struct _tmp442;(_tmp442.tag=0,((_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp443[0]=& _tmp442,Cyc_Tcutil_warn(p->loc,((_tmp444="unnecessary ... in datatype field %s",_tag_dyneither(_tmp444,sizeof(char),37))),_tag_dyneither(_tmp443,sizeof(void*),1)))))));}}}
# 569
for(0;_tmp150 != 0  && tqts != 0;(_tmp150=_tmp150->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp15F=(struct Cyc_Absyn_Pat*)_tmp150->hd;
# 573
void*_tmp160=Cyc_Tcutil_rsubstitute(rgn,_tmp154,(*((struct _tuple16*)tqts->hd)).f2);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp15F,& _tmp160,rgn_pat,allow_ref_pat));
# 578
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp15F->topt),_tmp160)){
const char*_tmp44A;void*_tmp449[3];struct Cyc_String_pa_PrintArg_struct _tmp448;struct Cyc_String_pa_PrintArg_struct _tmp447;struct Cyc_String_pa_PrintArg_struct _tmp446;(_tmp446.tag=0,((_tmp446.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 581
Cyc_Absynpp_typ2string((void*)_check_null(_tmp15F->topt))),((_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 580
Cyc_Absynpp_typ2string(_tmp160)),((_tmp448.tag=0,((_tmp448.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp449[0]=& _tmp448,((_tmp449[1]=& _tmp447,((_tmp449[2]=& _tmp446,Cyc_Tcutil_terr(_tmp15F->loc,((_tmp44A="%s expects argument type %s, not %s",_tag_dyneither(_tmp44A,sizeof(char),36))),_tag_dyneither(_tmp449,sizeof(void*),3)))))))))))))))))));}}
# 583
if(_tmp150 != 0){
const char*_tmp44E;void*_tmp44D[1];struct Cyc_String_pa_PrintArg_struct _tmp44C;(_tmp44C.tag=0,((_tmp44C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp44D[0]=& _tmp44C,Cyc_Tcutil_terr(p->loc,((_tmp44E="too many arguments for datatype constructor %s",_tag_dyneither(_tmp44E,sizeof(char),47))),_tag_dyneither(_tmp44D,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp452;void*_tmp451[1];struct Cyc_String_pa_PrintArg_struct _tmp450;(_tmp450.tag=0,((_tmp450.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp451[0]=& _tmp450,Cyc_Tcutil_terr(p->loc,((_tmp452="too few arguments for datatype constructor %s",_tag_dyneither(_tmp452,sizeof(char),46))),_tag_dyneither(_tmp451,sizeof(void*),1)))))));}}
# 590
_npop_handler(0);goto _LL4A;
# 546
;_pop_region(rgn);}_LL6B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC1->tag != 6)goto _LL6D;else{if(_tmpC1->f1 != 0)goto _LL6D;}}_LL6C:
# 592
 goto _LL6E;_LL6D: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC2=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC2->tag != 6)goto _LL6F;else{if(_tmpC2->f1 == 0)goto _LL6F;if((((_tmpC2->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6F;}}_LL6E:
 goto _LL70;_LL6F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC3=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC3->tag != 14)goto _LL71;}_LL70:
 goto _LL72;_LL71: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC4->tag != 16)goto _LL73;}_LL72:
 goto _LL74;_LL73: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC5=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC5->tag != 15)goto _LL4A;}_LL74:
# 597
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL4A;_LL4A:;}
# 599
tcpat_end:
 p->topt=t;
return res;};}
# 604
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){
struct Cyc_Tcpat_TcPatResult _tmp16E=Cyc_Tcpat_tcPatRec(te,p,topt,0,0);
# 607
struct _RegionHandle _tmp16F=_new_region("r");struct _RegionHandle*r=& _tmp16F;_push_region(r);{
const char*_tmp453;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp16E.patvars),p->loc,(
(_tmp453="pattern contains a repeated variable",_tag_dyneither(_tmp453,sizeof(char),37))));}{
# 611
struct Cyc_Tcpat_TcPatResult _tmp171=_tmp16E;_npop_handler(0);return _tmp171;};
# 607
;_pop_region(r);}
# 614
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
void*_tmp172=p->r;struct Cyc_Absyn_Pat*_tmp174;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Pat*_tmp17C;struct Cyc_Absyn_Pat*_tmp17E;_LLA2: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp173=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp173->tag != 5)goto _LLA4;else{_tmp174=_tmp173->f1;}}_LLA3:
# 617
 Cyc_Tcpat_check_pat_regions(te,_tmp174);{
# 619
struct _RegionHandle _tmp17F=_new_region("temp");struct _RegionHandle*temp=& _tmp17F;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(_tmp174->topt))){
{const char*_tmp456;void*_tmp455;(_tmp455=0,Cyc_Tcutil_terr(p->loc,((_tmp456="Pattern dereferences to a non-aliased pointer; use swap",_tag_dyneither(_tmp456,sizeof(char),56))),_tag_dyneither(_tmp455,sizeof(void*),0)));}
_npop_handler(0);return;}
# 626
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(p->topt))){
{const char*_tmp459;void*_tmp458;(_tmp458=0,Cyc_Tcutil_terr(p->loc,((_tmp459="Pattern dereferences a non-aliased pointer; use swap",_tag_dyneither(_tmp459,sizeof(char),53))),_tag_dyneither(_tmp458,sizeof(void*),0)));}
_npop_handler(0);return;}{
# 631
void*_tmp184=(void*)_check_null(p->topt);void*_tmp186;_LLB1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184;if(_tmp185->tag != 5)goto _LLB3;else{_tmp186=((_tmp185->f1).ptr_atts).rgn;}}_LLB2:
# 633
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp186);
_npop_handler(0);return;_LLB3:;_LLB4: {
const char*_tmp45C;void*_tmp45B;(_tmp45B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp45C="check_pat_regions: bad pointer type",_tag_dyneither(_tmp45C,sizeof(char),36))),_tag_dyneither(_tmp45B,sizeof(void*),0)));}_LLB0:;};
# 619
;_pop_region(temp);};_LLA4: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp175=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp175->tag != 6)goto _LLA6;else{_tmp176=_tmp175->f3;}}_LLA5:
# 638
 for(0;_tmp176 != 0;_tmp176=_tmp176->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple15*)_tmp176->hd)).f2);}
return;_LLA6: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp177=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp177->tag != 7)goto _LLA8;else{_tmp178=_tmp177->f3;}}_LLA7:
# 641
 _tmp17A=_tmp178;goto _LLA9;_LLA8: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp179=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp179->tag != 4)goto _LLAA;else{_tmp17A=_tmp179->f1;}}_LLA9:
# 643
 for(0;_tmp17A != 0;_tmp17A=_tmp17A->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp17A->hd);}
return;_LLAA: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp17B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp17B->tag != 2)goto _LLAC;else{_tmp17C=_tmp17B->f2;}}_LLAB:
 _tmp17E=_tmp17C;goto _LLAD;_LLAC: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp17D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp172;if(_tmp17D->tag != 1)goto _LLAE;else{_tmp17E=_tmp17D->f2;}}_LLAD:
 Cyc_Tcpat_check_pat_regions(te,_tmp17E);
return;_LLAE:;_LLAF:
 return;_LLA1:;}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 689 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp45D;return((_tmp45D.Name_v).val=s,(((_tmp45D.Name_v).tag=1,_tmp45D)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp45E;return((_tmp45E.Int_v).val=i,(((_tmp45E.Int_v).tag=2,_tmp45E)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 699
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 704
typedef void*Cyc_Tcpat_simple_pat_t;
# 707
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp18B=c1->name;struct _dyneither_ptr _tmp18C;int _tmp18D;_LLB6: if((_tmp18B.Name_v).tag != 1)goto _LLB8;_tmp18C=(struct _dyneither_ptr)(_tmp18B.Name_v).val;_LLB7: {
# 710
union Cyc_Tcpat_Name_value _tmp18E=c2->name;struct _dyneither_ptr _tmp18F;_LLBB: if((_tmp18E.Name_v).tag != 1)goto _LLBD;_tmp18F=(struct _dyneither_ptr)(_tmp18E.Name_v).val;_LLBC:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp18C,(struct _dyneither_ptr)_tmp18F);_LLBD: if((_tmp18E.Int_v).tag != 2)goto _LLBA;_LLBE:
 return - 1;_LLBA:;}_LLB8: if((_tmp18B.Int_v).tag != 2)goto _LLB5;_tmp18D=(int)(_tmp18B.Int_v).val;_LLB9: {
# 715
union Cyc_Tcpat_Name_value _tmp190=c2->name;int _tmp191;_LLC0: if((_tmp190.Name_v).tag != 1)goto _LLC2;_LLC1:
 return 1;_LLC2: if((_tmp190.Int_v).tag != 2)goto _LLBF;_tmp191=(int)(_tmp190.Int_v).val;_LLC3:
 return _tmp18D - _tmp191;_LLBF:;}_LLB5:;}
# 723
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 727
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 731
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp461;struct Cyc_Tcpat_Con_s*_tmp460;return(_tmp460=_region_malloc(r,sizeof(*_tmp460)),((_tmp460->name=Cyc_Tcpat_Name_v(((_tmp461="NULL",_tag_dyneither(_tmp461,sizeof(char),5)))),((_tmp460->arity=0,((_tmp460->span=& Cyc_Tcpat_two_opt,((_tmp460->orig_pat=p,_tmp460)))))))));}
# 734
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp464;struct Cyc_Tcpat_Con_s*_tmp463;return(_tmp463=_region_malloc(r,sizeof(*_tmp463)),((_tmp463->name=Cyc_Tcpat_Name_v(((_tmp464="&",_tag_dyneither(_tmp464,sizeof(char),2)))),((_tmp463->arity=1,((_tmp463->span=& Cyc_Tcpat_two_opt,((_tmp463->orig_pat=p,_tmp463)))))))));}
# 737
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp467;struct Cyc_Tcpat_Con_s*_tmp466;return(_tmp466=_region_malloc(r,sizeof(*_tmp466)),((_tmp466->name=Cyc_Tcpat_Name_v(((_tmp467="&",_tag_dyneither(_tmp467,sizeof(char),2)))),((_tmp466->arity=1,((_tmp466->span=& Cyc_Tcpat_one_opt,((_tmp466->orig_pat=p,_tmp466)))))))));}
# 740
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp468;return(_tmp468=_region_malloc(r,sizeof(*_tmp468)),((_tmp468->name=Cyc_Tcpat_Int_v(i),((_tmp468->arity=0,((_tmp468->span=0,((_tmp468->orig_pat=p,_tmp468)))))))));}
# 743
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp469;return(_tmp469=_region_malloc(r,sizeof(*_tmp469)),((_tmp469->name=Cyc_Tcpat_Name_v(f),((_tmp469->arity=0,((_tmp469->span=0,((_tmp469->orig_pat=p,_tmp469)))))))));}
# 746
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp46A;return(_tmp46A=_region_malloc(r,sizeof(*_tmp46A)),((_tmp46A->name=Cyc_Tcpat_Int_v((int)c),((_tmp46A->arity=0,((_tmp46A->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp46A->orig_pat=p,_tmp46A)))))))));}
# 749
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp46D;struct Cyc_Tcpat_Con_s*_tmp46C;return(_tmp46C=_region_malloc(r,sizeof(*_tmp46C)),((_tmp46C->name=Cyc_Tcpat_Name_v(((_tmp46D="$",_tag_dyneither(_tmp46D,sizeof(char),2)))),((_tmp46C->arity=i,((_tmp46C->span=& Cyc_Tcpat_one_opt,((_tmp46C->orig_pat=p,_tmp46C)))))))));}
# 754
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp470;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp46F;return(void*)((_tmp46F=_region_malloc(r,sizeof(*_tmp46F)),((_tmp46F[0]=((_tmp470.tag=1,((_tmp470.f1=Cyc_Tcpat_null_con(r,p),((_tmp470.f2=0,_tmp470)))))),_tmp46F))));}
# 757
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp473;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp472;return(void*)((_tmp472=_region_malloc(r,sizeof(*_tmp472)),((_tmp472[0]=((_tmp473.tag=1,((_tmp473.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp473.f2=0,_tmp473)))))),_tmp472))));}
# 760
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp476;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp475;return(void*)((_tmp475=_region_malloc(r,sizeof(*_tmp475)),((_tmp475[0]=((_tmp476.tag=1,((_tmp476.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp476.f2=0,_tmp476)))))),_tmp475))));}
# 763
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp479;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp478;return(void*)((_tmp478=_region_malloc(r,sizeof(*_tmp478)),((_tmp478[0]=((_tmp479.tag=1,((_tmp479.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp479.f2=0,_tmp479)))))),_tmp478))));}
# 766
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp47F;struct Cyc_List_List*_tmp47E;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp47D;return(void*)((_tmp47D=_region_malloc(r,sizeof(*_tmp47D)),((_tmp47D[0]=((_tmp47F.tag=1,((_tmp47F.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp47F.f2=((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E->hd=p,((_tmp47E->tl=0,_tmp47E)))))),_tmp47F)))))),_tmp47D))));}
# 769
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp485;struct Cyc_List_List*_tmp484;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp483;return(void*)((_tmp483=_region_malloc(r,sizeof(*_tmp483)),((_tmp483[0]=((_tmp485.tag=1,((_tmp485.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp485.f2=((_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484->hd=p,((_tmp484->tl=0,_tmp484)))))),_tmp485)))))),_tmp483))));}
# 772
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp488;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp487;return(void*)((_tmp487=_region_malloc(r,sizeof(*_tmp487)),((_tmp487[0]=((_tmp488.tag=1,((_tmp488.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp488.f2=ss,_tmp488)))))),_tmp487))));}
# 775
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 777
struct Cyc_Tcpat_Con_s*_tmp489;struct Cyc_Tcpat_Con_s*c=(_tmp489=_region_malloc(r,sizeof(*_tmp489)),((_tmp489->name=Cyc_Tcpat_Name_v(con_name),((_tmp489->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp489->span=span,((_tmp489->orig_pat=p,_tmp489)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp48C;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp48B;return(void*)((_tmp48B=_region_malloc(r,sizeof(*_tmp48B)),((_tmp48B[0]=((_tmp48C.tag=1,((_tmp48C.f1=c,((_tmp48C.f2=ps,_tmp48C)))))),_tmp48B))));}
# 782
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1B0=p->r;enum Cyc_Absyn_Sign _tmp1B5;int _tmp1B6;char _tmp1B8;struct _dyneither_ptr _tmp1BA;struct Cyc_Absyn_Pat*_tmp1BC;struct Cyc_Absyn_Pat*_tmp1BE;struct Cyc_Absyn_Pat*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1C2;struct Cyc_Absyn_Datatypefield*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Aggrdecl*_tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_Enumdecl*_tmp1CB;struct Cyc_Absyn_Enumfield*_tmp1CC;void*_tmp1CE;struct Cyc_Absyn_Enumfield*_tmp1CF;_LLC5: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1B1=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B1->tag != 0)goto _LLC7;}_LLC6:
 goto _LLC8;_LLC7: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1B2=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B2->tag != 3)goto _LLC9;}_LLC8:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp48F;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp48E;s=(void*)((_tmp48E=_region_malloc(r,sizeof(*_tmp48E)),((_tmp48E[0]=((_tmp48F.tag=0,_tmp48F)),_tmp48E))));}goto _LLC4;_LLC9: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1B3=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B3->tag != 8)goto _LLCB;}_LLCA:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC4;_LLCB: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B4=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B4->tag != 9)goto _LLCD;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B4->f2;}}_LLCC:
 s=Cyc_Tcpat_int_pat(r,_tmp1B6,p);goto _LLC4;_LLCD: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1B7=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B7->tag != 10)goto _LLCF;else{_tmp1B8=_tmp1B7->f1;}}_LLCE:
 s=Cyc_Tcpat_char_pat(r,_tmp1B8,p);goto _LLC4;_LLCF: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1B9=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1B9->tag != 11)goto _LLD1;else{_tmp1BA=_tmp1B9->f1;}}_LLD0:
 s=Cyc_Tcpat_float_pat(r,_tmp1BA,p);goto _LLC4;_LLD1: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1BB=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1BB->tag != 1)goto _LLD3;else{_tmp1BC=_tmp1BB->f2;}}_LLD2:
 s=Cyc_Tcpat_compile_pat(r,_tmp1BC);goto _LLC4;_LLD3: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1BD=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1BD->tag != 2)goto _LLD5;else{_tmp1BE=_tmp1BD->f2;}}_LLD4:
 s=Cyc_Tcpat_compile_pat(r,_tmp1BE);goto _LLC4;_LLD5: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1BF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1BF->tag != 5)goto _LLD7;else{_tmp1C0=_tmp1BF->f1;}}_LLD6:
# 794
{void*_tmp1D6=Cyc_Tcutil_compress((void*)_check_null(p->topt));union Cyc_Absyn_Constraint*_tmp1D8;_LLEA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D6;if(_tmp1D7->tag != 5)goto _LLEC;else{_tmp1D8=((_tmp1D7->f1).ptr_atts).nullable;}}_LLEB: {
# 796
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1D9=_tmp1D8;union Cyc_Absyn_Constraint*_tmp1DA;int _tmp1DB;_LLEF: if((_tmp1D9->Forward_constr).tag != 2)goto _LLF1;_tmp1DA=(union Cyc_Absyn_Constraint*)(_tmp1D9->Forward_constr).val;_LLF0:
# 801
*_tmp1D8=*_tmp1DA;
continue;_LLF1: if((_tmp1D9->No_constr).tag != 3)goto _LLF3;_LLF2:
# 804
{struct _union_Constraint_Eq_constr*_tmp490;(_tmp490=& _tmp1D8->Eq_constr,((_tmp490->tag=1,_tmp490->val=0)));}
is_nullable=0;
still_working=0;
goto _LLEE;_LLF3: if((_tmp1D9->Eq_constr).tag != 1)goto _LLEE;_tmp1DB=(int)(_tmp1D9->Eq_constr).val;_LLF4:
# 809
 is_nullable=_tmp1DB;
still_working=0;
goto _LLEE;_LLEE:;}{
# 814
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1C0);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 818
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLE9;};}_LLEC:;_LLED: {
const char*_tmp493;void*_tmp492;(_tmp492=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp493="expecting pointertype for pattern!",_tag_dyneither(_tmp493,sizeof(char),35))),_tag_dyneither(_tmp492,sizeof(void*),0)));}_LLE9:;}
# 822
goto _LLC4;_LLD7: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1C1=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1C1->tag != 7)goto _LLD9;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;_tmp1C4=_tmp1C1->f3;}}_LLD8: {
# 824
int*span;
{void*_tmp1DF=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LLF6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DF;if(_tmp1E0->tag != 3)goto _LLF8;}_LLF7:
# 827
 if(_tmp1C2->is_extensible)
span=0;else{
# 830
int*_tmp494;span=((_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C2->fields))->v),_tmp494))));}
goto _LLF5;_LLF8: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1E1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DF;if(_tmp1E1->tag != 4)goto _LLFA;}_LLF9:
 span=& Cyc_Tcpat_one_opt;goto _LLF5;_LLFA:;_LLFB:
{const char*_tmp497;void*_tmp496;span=((_tmp496=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp497="void datatype pattern has bad type",_tag_dyneither(_tmp497,sizeof(char),35))),_tag_dyneither(_tmp496,sizeof(void*),0))));}goto _LLF5;_LLF5:;}
# 835
s=Cyc_Tcpat_con_pat(r,*(*_tmp1C3->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C4),p);
goto _LLC4;}_LLD9: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1C5=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1C5->tag != 4)goto _LLDB;else{_tmp1C6=_tmp1C5->f1;}}_LLDA:
# 839
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C6),p);goto _LLC4;_LLDB: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1C7->tag != 6)goto _LLDD;else{if(_tmp1C7->f1 == 0)goto _LLDD;if((((_tmp1C7->f1)->aggr_info).KnownAggr).tag != 2)goto _LLDD;_tmp1C8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1C7->f1)->aggr_info).KnownAggr).val);_tmp1C9=_tmp1C7->f3;}}_LLDC:
# 844
 if(_tmp1C8->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C8->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 848
const char*_tmp498;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp498="",_tag_dyneither(_tmp498,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1C9;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct Cyc_List_List*_tmp1E6;struct Cyc_Absyn_Pat*_tmp1E7;struct _tuple15*_tmp1E5=(struct _tuple15*)dlps0->hd;_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;{
struct Cyc_List_List*_tmp1E8=_tmp1E6;struct _dyneither_ptr*_tmp1EA;_LLFD: if(_tmp1E8 == 0)goto _LLFF;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1E9=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp1E8->hd);if(_tmp1E9->tag != 1)goto _LLFF;else{_tmp1EA=_tmp1E9->f1;}}if(_tmp1E8->tl != 0)goto _LLFF;_LLFE:
# 853
 if(Cyc_strptrcmp(_tmp1EA,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp499;ps=((_tmp499=_region_malloc(r,sizeof(*_tmp499)),((_tmp499->hd=Cyc_Tcpat_compile_pat(r,_tmp1E7),((_tmp499->tl=ps,_tmp499))))));}
found=1;}
# 857
goto _LLFC;_LLFF:;_LL100: {
const char*_tmp49C;void*_tmp49B;(_tmp49B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp49C="bad designator(s)",_tag_dyneither(_tmp49C,sizeof(char),18))),_tag_dyneither(_tmp49B,sizeof(void*),0)));}_LLFC:;};}}
# 861
if(!found){
const char*_tmp49F;void*_tmp49E;(_tmp49E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp49F="bad designator",_tag_dyneither(_tmp49F,sizeof(char),15))),_tag_dyneither(_tmp49E,sizeof(void*),0)));}}}
# 864
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 867
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C8->impl))->tagged){
const char*_tmp4A2;void*_tmp4A1;(_tmp4A1=0,Cyc_Tcutil_terr(p->loc,((_tmp4A2="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4A2,sizeof(char),47))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}{
int*_tmp4A3;int*span=(_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C8->impl))->fields),_tmp4A3)));
struct Cyc_List_List*_tmp1F3=_tmp1C9;struct _dyneither_ptr*_tmp1F5;struct Cyc_Absyn_Pat*_tmp1F6;_LL102: if(_tmp1F3 == 0)goto _LL104;if(((struct _tuple15*)_tmp1F3->hd)->f1 == 0)goto _LL104;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1F4=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp1F3->hd)->f1)->hd);if(_tmp1F4->tag != 1)goto _LL104;else{_tmp1F5=_tmp1F4->f1;}}if((((struct _tuple15*)_tmp1F3->hd)->f1)->tl != 0)goto _LL104;_tmp1F6=((struct _tuple15*)_tmp1F3->hd)->f2;if(_tmp1F3->tl != 0)goto _LL104;_LL103:
# 872
{struct Cyc_List_List*_tmp4A4;s=Cyc_Tcpat_con_pat(r,*_tmp1F5,span,((_tmp4A4=_region_malloc(r,sizeof(*_tmp4A4)),((_tmp4A4->hd=Cyc_Tcpat_compile_pat(r,_tmp1F6),((_tmp4A4->tl=0,_tmp4A4)))))),p);}
goto _LL101;_LL104:;_LL105: {
const char*_tmp4A7;void*_tmp4A6;(_tmp4A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A7="bad union pattern",_tag_dyneither(_tmp4A7,sizeof(char),18))),_tag_dyneither(_tmp4A6,sizeof(void*),0)));}_LL101:;};}
# 877
goto _LLC4;_LLDD: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1CA=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1CA->tag != 12)goto _LLDF;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}_LLDE:
# 882
{void*_tmp1FB=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LL107: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FB;if(_tmp1FC->tag != 6)goto _LL109;}_LL108:
# 886
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1CC->name).f2,0,0,p);
goto _LL106;_LL109:;_LL10A: {
# 889
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v);
{int*_tmp4A8;s=Cyc_Tcpat_con_pat(r,*(*_tmp1CC->name).f2,((_tmp4A8=_region_malloc(r,sizeof(*_tmp4A8)),((_tmp4A8[0]=span,_tmp4A8)))),0,p);}
goto _LL106;}_LL106:;}
# 893
goto _LLC4;_LLDF: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1CD=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1CD->tag != 13)goto _LLE1;else{_tmp1CE=(void*)_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;}}_LLE0: {
# 898
struct Cyc_List_List*fields;
{void*_tmp1FE=Cyc_Tcutil_compress(_tmp1CE);struct Cyc_List_List*_tmp200;_LL10C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 14)goto _LL10E;else{_tmp200=_tmp1FF->f1;}}_LL10D:
 fields=_tmp200;goto _LL10B;_LL10E:;_LL10F: {
const char*_tmp4AB;void*_tmp4AA;(_tmp4AA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4AB="bad type in AnonEnum_p",_tag_dyneither(_tmp4AB,sizeof(char),23))),_tag_dyneither(_tmp4AA,sizeof(void*),0)));}_LL10B:;}
# 905
{void*_tmp203=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LL111: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp203;if(_tmp204->tag != 6)goto _LL113;}_LL112:
# 909
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1CF->name).f2,0,0,p);
goto _LL110;_LL113:;_LL114: {
# 912
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4AC;s=Cyc_Tcpat_con_pat(r,*(*_tmp1CF->name).f2,((_tmp4AC=_region_malloc(r,sizeof(*_tmp4AC)),((_tmp4AC[0]=span,_tmp4AC)))),0,p);}
goto _LL110;}_LL110:;}
# 916
goto _LLC4;}_LLE1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1D0->tag != 14)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1D1->tag != 15)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1D2=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1D2->tag != 6)goto _LLE7;}_LLE6:
 goto _LLE8;_LLE7: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1B0;if(_tmp1D3->tag != 16)goto _LLC4;}_LLE8: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4AF;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4AE;s=(void*)((_tmp4AE=_region_malloc(r,sizeof(*_tmp4AE)),((_tmp4AE[0]=((_tmp4AF.tag=0,_tmp4AF)),_tmp4AE))));}_LLC4:;}
# 922
return s;}
# 930
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 941
typedef void*Cyc_Tcpat_term_desc_t;
# 945
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 952
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 961
typedef void*Cyc_Tcpat_decision_t;struct _tuple18{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 966
typedef struct _tuple18*Cyc_Tcpat_work_stack_frame_t;
# 969
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 971
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 974
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4B2;void*_tmp4B1;(_tmp4B1=0,Cyc_fprintf(Cyc_stderr,((_tmp4B2=" ",_tag_dyneither(_tmp4B2,sizeof(char),2))),_tag_dyneither(_tmp4B1,sizeof(void*),0)));}}
# 980
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp20A=c->name;
union Cyc_Tcpat_Name_value _tmp20B=_tmp20A;struct _dyneither_ptr _tmp20C;int _tmp20D;_LL116: if((_tmp20B.Name_v).tag != 1)goto _LL118;_tmp20C=(struct _dyneither_ptr)(_tmp20B.Name_v).val;_LL117:
{const char*_tmp4B6;void*_tmp4B5[1];struct Cyc_String_pa_PrintArg_struct _tmp4B4;(_tmp4B4.tag=0,((_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20C),((_tmp4B5[0]=& _tmp4B4,Cyc_fprintf(Cyc_stderr,((_tmp4B6="%s",_tag_dyneither(_tmp4B6,sizeof(char),3))),_tag_dyneither(_tmp4B5,sizeof(void*),1)))))));}goto _LL115;_LL118: if((_tmp20B.Int_v).tag != 2)goto _LL115;_tmp20D=(int)(_tmp20B.Int_v).val;_LL119:
{const char*_tmp4BA;void*_tmp4B9[1];struct Cyc_Int_pa_PrintArg_struct _tmp4B8;(_tmp4B8.tag=1,((_tmp4B8.f1=(unsigned long)_tmp20D,((_tmp4B9[0]=& _tmp4B8,Cyc_fprintf(Cyc_stderr,((_tmp4BA="%d",_tag_dyneither(_tmp4BA,sizeof(char),3))),_tag_dyneither(_tmp4B9,sizeof(void*),1)))))));}goto _LL115;_LL115:;}
# 988
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp214=d;void*_tmp216;struct Cyc_List_List*_tmp219;struct Cyc_Tcpat_Con_s*_tmp21A;void*_tmp21B;void*_tmp21C;_LL11B: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp215=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp214;if(_tmp215->tag != 1)goto _LL11D;else{_tmp216=(void*)_tmp215->f1;}}_LL11C:
# 991
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4BD;void*_tmp4BC;(_tmp4BC=0,Cyc_fprintf(Cyc_stderr,((_tmp4BD="Success(",_tag_dyneither(_tmp4BD,sizeof(char),9))),_tag_dyneither(_tmp4BC,sizeof(void*),0)));}print_rhs(_tmp216);{const char*_tmp4C0;void*_tmp4BF;(_tmp4BF=0,Cyc_fprintf(Cyc_stderr,((_tmp4C0=")\n",_tag_dyneither(_tmp4C0,sizeof(char),3))),_tag_dyneither(_tmp4BF,sizeof(void*),0)));}
goto _LL11A;_LL11D: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp217=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp214;if(_tmp217->tag != 0)goto _LL11F;}_LL11E:
{const char*_tmp4C3;void*_tmp4C2;(_tmp4C2=0,Cyc_fprintf(Cyc_stderr,((_tmp4C3="Failure\n",_tag_dyneither(_tmp4C3,sizeof(char),9))),_tag_dyneither(_tmp4C2,sizeof(void*),0)));}goto _LL11A;_LL11F: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp218=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp214;if(_tmp218->tag != 2)goto _LL11A;else{_tmp219=_tmp218->f1;_tmp21A=_tmp218->f2;_tmp21B=(void*)_tmp218->f3;_tmp21C=(void*)_tmp218->f4;}}_LL120:
# 996
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4C6;void*_tmp4C5;(_tmp4C5=0,Cyc_fprintf(Cyc_stderr,((_tmp4C6="Access[",_tag_dyneither(_tmp4C6,sizeof(char),8))),_tag_dyneither(_tmp4C5,sizeof(void*),0)));}
for(0;_tmp219 != 0;_tmp219=_tmp219->tl){
{const char*_tmp4CA;void*_tmp4C9[1];struct Cyc_Int_pa_PrintArg_struct _tmp4C8;(_tmp4C8.tag=1,((_tmp4C8.f1=(unsigned long)((int)_tmp219->hd),((_tmp4C9[0]=& _tmp4C8,Cyc_fprintf(Cyc_stderr,((_tmp4CA="%d",_tag_dyneither(_tmp4CA,sizeof(char),3))),_tag_dyneither(_tmp4C9,sizeof(void*),1)))))));}
if(_tmp219->tl != 0){const char*_tmp4CD;void*_tmp4CC;(_tmp4CC=0,Cyc_fprintf(Cyc_stderr,((_tmp4CD=",",_tag_dyneither(_tmp4CD,sizeof(char),2))),_tag_dyneither(_tmp4CC,sizeof(void*),0)));}}
# 1002
{const char*_tmp4D0;void*_tmp4CF;(_tmp4CF=0,Cyc_fprintf(Cyc_stderr,((_tmp4D0="],",_tag_dyneither(_tmp4D0,sizeof(char),3))),_tag_dyneither(_tmp4CF,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp21A);
{const char*_tmp4D3;void*_tmp4D2;(_tmp4D2=0,Cyc_fprintf(Cyc_stderr,((_tmp4D3=",\n",_tag_dyneither(_tmp4D3,sizeof(char),3))),_tag_dyneither(_tmp4D2,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp21B,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp21C,print_rhs,tab + 7);_LL11A:;}struct _tuple19{int f1;unsigned int f2;};
# 1010
static void Cyc_Tcpat_print_swrhs(struct _tuple19*x){
return;}
# 1018
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp22E=td;struct Cyc_Set_Set*_tmp230;_LL122: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp22F=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp22E;if(_tmp22F->tag != 1)goto _LL124;else{_tmp230=_tmp22F->f1;}}_LL123: {
# 1028
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4D6;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4D5;return(void*)((_tmp4D5=_region_malloc(r,sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D6.tag=1,((_tmp4D6.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp230,c),_tmp4D6)))),_tmp4D5))));}_LL124: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp231=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp22E;if(_tmp231->tag != 0)goto _LL121;}_LL125: {
const char*_tmp4D9;void*_tmp4D8;(_tmp4D8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D9="add_neg called when td is Positive",_tag_dyneither(_tmp4D9,sizeof(char),35))),_tag_dyneither(_tmp4D8,sizeof(void*),0)));}_LL121:;}
# 1035
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp236=td;struct Cyc_Tcpat_Con_s*_tmp238;struct Cyc_Set_Set*_tmp23A;_LL127: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp237=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp236;if(_tmp237->tag != 0)goto _LL129;else{_tmp238=_tmp237->f1;}}_LL128:
# 1039
 if(Cyc_Tcpat_compare_con(c,_tmp238)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL129: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp239=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp236;if(_tmp239->tag != 1)goto _LL126;else{_tmp23A=_tmp239->f1;}}_LL12A:
# 1043
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp23A,c))return Cyc_Tcpat_No;else{
# 1046
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp23A)+ 1)
return Cyc_Tcpat_Yes;else{
# 1049
return Cyc_Tcpat_Maybe;}}_LL126:;}struct _tuple20{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1057
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1059
struct Cyc_List_List*_tmp23B=ctxt;struct Cyc_Tcpat_Con_s*_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23E;_LL12C: if(_tmp23B != 0)goto _LL12E;_LL12D:
 return 0;_LL12E: if(_tmp23B == 0)goto _LL12B;_tmp23C=((struct _tuple20*)_tmp23B->hd)->f1;_tmp23D=((struct _tuple20*)_tmp23B->hd)->f2;_tmp23E=_tmp23B->tl;_LL12F: {
# 1062
struct _tuple20*_tmp4DF;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4DD;return(_tmp4DD=_region_malloc(r,sizeof(*_tmp4DD)),((_tmp4DD->hd=(
(_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF->f1=_tmp23C,((_tmp4DF->f2=((_tmp4DE=_region_malloc(r,sizeof(*_tmp4DE)),((_tmp4DE->hd=dsc,((_tmp4DE->tl=_tmp23D,_tmp4DE)))))),_tmp4DF)))))),((_tmp4DD->tl=_tmp23E,_tmp4DD)))));}_LL12B:;}
# 1070
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp242=ctxt;struct Cyc_Tcpat_Con_s*_tmp243;struct Cyc_List_List*_tmp244;struct Cyc_List_List*_tmp245;_LL131: if(_tmp242 != 0)goto _LL133;_LL132: {
const char*_tmp4E2;void*_tmp4E1;(_tmp4E1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E2="norm_context: empty context",_tag_dyneither(_tmp4E2,sizeof(char),28))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}_LL133: if(_tmp242 == 0)goto _LL130;_tmp243=((struct _tuple20*)_tmp242->hd)->f1;_tmp244=((struct _tuple20*)_tmp242->hd)->f2;_tmp245=_tmp242->tl;_LL134: {
# 1074
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4E5;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4E4;return Cyc_Tcpat_augment(r,_tmp245,(void*)((_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4[0]=((_tmp4E5.tag=0,((_tmp4E5.f1=_tmp243,((_tmp4E5.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp244),_tmp4E5)))))),_tmp4E4)))));}_LL130:;}
# 1083
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1085
struct _tuple13 _tmp4E6;struct _tuple13 _tmp24B=(_tmp4E6.f1=ctxt,((_tmp4E6.f2=work,_tmp4E6)));struct Cyc_Tcpat_Con_s*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp250;_LL136: if(_tmp24B.f1 != 0)goto _LL138;if(_tmp24B.f2 != 0)goto _LL138;_LL137:
 return dsc;_LL138: if(_tmp24B.f1 != 0)goto _LL13A;_LL139:
 goto _LL13B;_LL13A: if(_tmp24B.f2 != 0)goto _LL13C;_LL13B: {
const char*_tmp4E9;void*_tmp4E8;(_tmp4E8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E9="build_desc: ctxt and work don't match",_tag_dyneither(_tmp4E9,sizeof(char),38))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));}_LL13C: if(_tmp24B.f1 == 0)goto _LL135;_tmp24C=((struct _tuple20*)(_tmp24B.f1)->hd)->f1;_tmp24D=((struct _tuple20*)(_tmp24B.f1)->hd)->f2;_tmp24E=(_tmp24B.f1)->tl;if(_tmp24B.f2 == 0)goto _LL135;_tmp24F=((struct _tuple18*)(_tmp24B.f2)->hd)->f3;_tmp250=(_tmp24B.f2)->tl;_LL13D: {
# 1090
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4EF;struct Cyc_List_List*_tmp4EE;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4ED;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp253=(_tmp4ED=_region_malloc(r,sizeof(*_tmp4ED)),((_tmp4ED[0]=((_tmp4EF.tag=0,((_tmp4EF.f1=_tmp24C,((_tmp4EF.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp24D),(
(_tmp4EE=_region_malloc(r,sizeof(*_tmp4EE)),((_tmp4EE->hd=dsc,((_tmp4EE->tl=_tmp24F,_tmp4EE))))))),_tmp4EF)))))),_tmp4ED)));
return Cyc_Tcpat_build_desc(r,_tmp24E,(void*)_tmp253,_tmp250);}_LL135:;}
# 1096
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1103
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1105
struct Cyc_List_List*_tmp257=allmrules;void*_tmp258;void*_tmp259;struct Cyc_List_List*_tmp25A;_LL13F: if(_tmp257 != 0)goto _LL141;_LL140: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp4F2;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp4F1;return(void*)((_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F2.tag=0,((_tmp4F2.f1=dsc,_tmp4F2)))),_tmp4F1))));}_LL141: if(_tmp257 == 0)goto _LL13E;_tmp258=((struct _tuple0*)_tmp257->hd)->f1;_tmp259=((struct _tuple0*)_tmp257->hd)->f2;_tmp25A=_tmp257->tl;_LL142:
# 1108
 return Cyc_Tcpat_match(r,_tmp258,0,dsc,0,0,_tmp259,_tmp25A);_LL13E:;}
# 1113
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1115
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4F5;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4F4;return Cyc_Tcpat_or_match(r,(void*)((_tmp4F4=_region_malloc(r,sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F5.tag=1,((_tmp4F5.f1=Cyc_Tcpat_empty_con_set(r),_tmp4F5)))),_tmp4F4)))),allmrules);}
# 1121
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1125
struct Cyc_List_List*_tmp25F=work;struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp264;_LL144: if(_tmp25F != 0)goto _LL146;_LL145: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp4F8;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp4F7;return(void*)((_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7[0]=((_tmp4F8.tag=1,((_tmp4F8.f1=right_hand_side,_tmp4F8)))),_tmp4F7))));}_LL146: if(_tmp25F == 0)goto _LL148;if(((struct _tuple18*)_tmp25F->hd)->f1 != 0)goto _LL148;if(((struct _tuple18*)_tmp25F->hd)->f2 != 0)goto _LL148;if(((struct _tuple18*)_tmp25F->hd)->f3 != 0)goto _LL148;_tmp260=_tmp25F->tl;_LL147:
# 1128
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp260,right_hand_side,rules);_LL148: if(_tmp25F == 0)goto _LL143;_tmp261=((struct _tuple18*)_tmp25F->hd)->f1;_tmp262=((struct _tuple18*)_tmp25F->hd)->f2;_tmp263=((struct _tuple18*)_tmp25F->hd)->f3;_tmp264=_tmp25F->tl;_LL149:
# 1130
 if((_tmp261 == 0  || _tmp262 == 0) || _tmp263 == 0){
const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,Cyc_Tcutil_impos(((_tmp4FB="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp4FB,sizeof(char),38))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}{
void*_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp269=_tmp261;_tmp26A=(void*)_tmp269->hd;_tmp26B=_tmp269->tl;{
struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp26C=_tmp262;_tmp26D=(struct Cyc_List_List*)_tmp26C->hd;_tmp26E=_tmp26C->tl;{
void*_tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List*_tmp26F=_tmp263;_tmp270=(void*)_tmp26F->hd;_tmp271=_tmp26F->tl;{
struct _tuple18*_tmp4FC;struct _tuple18*_tmp272=(_tmp4FC=_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC->f1=_tmp26B,((_tmp4FC->f2=_tmp26E,((_tmp4FC->f3=_tmp271,_tmp4FC)))))));
struct Cyc_List_List*_tmp4FD;return Cyc_Tcpat_match(r,_tmp26A,_tmp26D,_tmp270,ctx,((_tmp4FD=_region_malloc(r,sizeof(*_tmp4FD)),((_tmp4FD->hd=_tmp272,((_tmp4FD->tl=_tmp264,_tmp4FD)))))),right_hand_side,rules);};};};};_LL143:;}
# 1142
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1144
void*_tmp275=dsc;struct Cyc_Set_Set*_tmp277;struct Cyc_List_List*_tmp279;_LL14B: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp276=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp275;if(_tmp276->tag != 1)goto _LL14D;else{_tmp277=_tmp276->f1;}}_LL14C: {
# 1149
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp500;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4FF;void*any=(void*)((_tmp4FF=_region_malloc(r,sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp500.tag=1,((_tmp500.f1=Cyc_Tcpat_empty_con_set(r),_tmp500)))),_tmp4FF))));
struct Cyc_List_List*_tmp27A=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp501;_tmp27A=((_tmp501=_region_malloc(r,sizeof(*_tmp501)),((_tmp501->hd=any,((_tmp501->tl=_tmp27A,_tmp501))))));}}
return _tmp27A;}_LL14D: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp278=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp275;if(_tmp278->tag != 0)goto _LL14A;else{_tmp279=_tmp278->f2;}}_LL14E:
 return _tmp279;_LL14A:;}struct _tuple21{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1159
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple21*env,int i){
struct _RegionHandle*_tmp27F;struct Cyc_List_List*_tmp280;struct _tuple21*_tmp27E=env;_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;{
struct Cyc_List_List*_tmp502;return(_tmp502=_region_malloc(_tmp27F,sizeof(*_tmp502)),((_tmp502->hd=(void*)(i + 1),((_tmp502->tl=_tmp280,_tmp502)))));};}
# 1163
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1165
struct _tuple21 _tmp503;struct _tuple21 _tmp282=(_tmp503.f1=r,((_tmp503.f2=obj,_tmp503)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp282);}
# 1172
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1176
void*_tmp284=p;struct Cyc_Tcpat_Con_s*_tmp287;struct Cyc_List_List*_tmp288;_LL150: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp285=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp284;if(_tmp285->tag != 0)goto _LL152;}_LL151:
# 1178
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL152: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp286=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp284;if(_tmp286->tag != 1)goto _LL14F;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}_LL153:
# 1180
 switch(Cyc_Tcpat_static_match(_tmp287,dsc)){case Cyc_Tcpat_Yes: _LL154: {
# 1182
struct _tuple20*_tmp506;struct Cyc_List_List*_tmp505;struct Cyc_List_List*ctx2=(_tmp505=_region_malloc(r,sizeof(*_tmp505)),((_tmp505->hd=((_tmp506=_region_malloc(r,sizeof(*_tmp506)),((_tmp506->f1=_tmp287,((_tmp506->f2=0,_tmp506)))))),((_tmp505->tl=ctx,_tmp505)))));
struct _tuple18*_tmp507;struct _tuple18*work_frame=(_tmp507=_region_malloc(r,sizeof(*_tmp507)),((_tmp507->f1=_tmp288,((_tmp507->f2=Cyc_Tcpat_getoargs(r,_tmp287,obj),((_tmp507->f3=
Cyc_Tcpat_getdargs(r,_tmp287,dsc),_tmp507)))))));
struct Cyc_List_List*_tmp508;struct Cyc_List_List*work2=(_tmp508=_region_malloc(r,sizeof(*_tmp508)),((_tmp508->hd=work_frame,((_tmp508->tl=work,_tmp508)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL155:
# 1188
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL156: {
# 1190
struct _tuple20*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*ctx2=(_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A->hd=((_tmp50B=_region_malloc(r,sizeof(*_tmp50B)),((_tmp50B->f1=_tmp287,((_tmp50B->f2=0,_tmp50B)))))),((_tmp50A->tl=ctx,_tmp50A)))));
struct _tuple18*_tmp50C;struct _tuple18*work_frame=(_tmp50C=_region_malloc(r,sizeof(*_tmp50C)),((_tmp50C->f1=_tmp288,((_tmp50C->f2=Cyc_Tcpat_getoargs(r,_tmp287,obj),((_tmp50C->f3=
Cyc_Tcpat_getdargs(r,_tmp287,dsc),_tmp50C)))))));
struct Cyc_List_List*_tmp50D;struct Cyc_List_List*work2=(_tmp50D=_region_malloc(r,sizeof(*_tmp50D)),((_tmp50D->hd=work_frame,((_tmp50D->tl=work,_tmp50D)))));
void*_tmp28D=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp28E=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp287),work),rules);
# 1197
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp510;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp50F;return(void*)((_tmp50F=_region_malloc(r,sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp510.tag=2,((_tmp510.f1=obj,((_tmp510.f2=_tmp287,((_tmp510.f3=_tmp28D,((_tmp510.f4=_tmp28E,_tmp510)))))))))),_tmp50F))));}}_LL14F:;}
# 1207
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1213
void*_tmp295=d;void*_tmp297;void*_tmp299;void*_tmp29B;void*_tmp29C;_LL159: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp296=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp295;if(_tmp296->tag != 0)goto _LL15B;else{_tmp297=(void*)_tmp296->f1;}}_LL15A:
# 1215
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp297);*exhaust_done=1;}
goto _LL158;_LL15B: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp298=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp295;if(_tmp298->tag != 1)goto _LL15D;else{_tmp299=(void*)_tmp298->f1;}}_LL15C:
 rhs_appears(env2,_tmp299);goto _LL158;_LL15D: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp29A=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp295;if(_tmp29A->tag != 2)goto _LL158;else{_tmp29B=(void*)_tmp29A->f3;_tmp29C=(void*)_tmp29A->f4;}}_LL15E:
# 1219
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp29B,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1221
Cyc_Tcpat_check_exhaust_overlap(r,_tmp29C,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1223
goto _LL158;_LL158:;}struct _tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{struct _RegionHandle*f1;int*f2;};
# 1231
static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*swc){
# 1234
struct _RegionHandle*_tmp29E;int*_tmp29F;struct _tuple23*_tmp29D=env;_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp29E,swc->pattern);
struct _tuple19*_tmp511;struct _tuple19*rhs=(_tmp511=_region_malloc(_tmp29E,sizeof(*_tmp511)),((_tmp511->f1=0,((_tmp511->f2=(swc->pattern)->loc,_tmp511)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp514;struct Cyc_List_List*_tmp513;sp=Cyc_Tcpat_tuple_pat(_tmp29E,((_tmp513=_region_malloc(_tmp29E,sizeof(*_tmp513)),((_tmp513->hd=sp0,((_tmp513->tl=(
(_tmp514=_region_malloc(_tmp29E,sizeof(*_tmp514)),((_tmp514->hd=Cyc_Tcpat_int_pat(_tmp29E,*_tmp29F,0),((_tmp514->tl=0,_tmp514)))))),_tmp513)))))),0);}
*_tmp29F=*_tmp29F + 1;}else{
# 1243
struct Cyc_List_List*_tmp51E;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp51D;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp51C;struct Cyc_List_List*_tmp51B;sp=Cyc_Tcpat_tuple_pat(_tmp29E,((_tmp51B=_region_malloc(_tmp29E,sizeof(*_tmp51B)),((_tmp51B->hd=sp0,((_tmp51B->tl=((_tmp51E=_region_malloc(_tmp29E,sizeof(*_tmp51E)),((_tmp51E->hd=(void*)((_tmp51C=_region_malloc(_tmp29E,sizeof(*_tmp51C)),((_tmp51C[0]=((_tmp51D.tag=0,_tmp51D)),_tmp51C)))),((_tmp51E->tl=0,_tmp51E)))))),_tmp51B)))))),0);}{
struct _tuple22*_tmp51F;return(_tmp51F=_region_malloc(_tmp29E,sizeof(*_tmp51F)),((_tmp51F->f1=sp,((_tmp51F->f2=rhs,_tmp51F)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1249
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1251
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2A9=d;struct Cyc_Tcpat_Con_s*_tmp2AB;struct Cyc_List_List*_tmp2AC;struct Cyc_Set_Set*_tmp2AE;_LL160: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2AA=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A9;if(_tmp2AA->tag != 0)goto _LL162;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AA->f2;}}_LL161: {
# 1255
union Cyc_Tcpat_Name_value _tmp2AF=_tmp2AB->name;
struct Cyc_Absyn_Pat*_tmp2B0=_tmp2AB->orig_pat;
if(_tmp2B0 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2AC))->hd);{
# 1260
void*_tmp2B1=_tmp2B0->r;struct Cyc_Absyn_Vardecl*_tmp2B4;struct Cyc_Absyn_Vardecl*_tmp2B6;struct Cyc_Absyn_Tvar*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B9;struct Cyc_Absyn_Aggrdecl*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Datatypefield*_tmp2C0;int _tmp2C3;char _tmp2C5;struct _dyneither_ptr _tmp2C7;int _tmp2C8;struct Cyc_Absyn_Enumfield*_tmp2CA;struct Cyc_Absyn_Enumfield*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CE;_LL165: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2B2=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2B2->tag != 0)goto _LL167;}_LL166: {
const char*_tmp520;return(_tmp520="_",_tag_dyneither(_tmp520,sizeof(char),2));}_LL167: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2B3=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2B3->tag != 1)goto _LL169;else{_tmp2B4=_tmp2B3->f1;}}_LL168:
 return Cyc_Absynpp_qvar2string(_tmp2B4->name);_LL169: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2B5=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2B5->tag != 2)goto _LL16B;else{_tmp2B6=_tmp2B5->f1;}}_LL16A: {
const char*_tmp524;void*_tmp523[1];struct Cyc_String_pa_PrintArg_struct _tmp522;return(struct _dyneither_ptr)((_tmp522.tag=0,((_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B6->name)),((_tmp523[0]=& _tmp522,Cyc_aprintf(((_tmp524="*%s",_tag_dyneither(_tmp524,sizeof(char),4))),_tag_dyneither(_tmp523,sizeof(void*),1))))))));}_LL16B: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2B7=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2B7->tag != 3)goto _LL16D;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;}}_LL16C: {
const char*_tmp529;void*_tmp528[2];struct Cyc_String_pa_PrintArg_struct _tmp527;struct Cyc_String_pa_PrintArg_struct _tmp526;return(struct _dyneither_ptr)((_tmp526.tag=0,((_tmp526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B8->name),((_tmp527.tag=0,((_tmp527.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B9->name)),((_tmp528[0]=& _tmp527,((_tmp528[1]=& _tmp526,Cyc_aprintf(((_tmp529="%s<`%s>",_tag_dyneither(_tmp529,sizeof(char),8))),_tag_dyneither(_tmp528,sizeof(void*),2))))))))))))));}_LL16D: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2BA=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2BA->tag != 4)goto _LL16F;}_LL16E: {
# 1266
const char*_tmp52D;void*_tmp52C[1];struct Cyc_String_pa_PrintArg_struct _tmp52B;return(struct _dyneither_ptr)((_tmp52B.tag=0,((_tmp52B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AC)),((_tmp52C[0]=& _tmp52B,Cyc_aprintf(((_tmp52D="$(%s)",_tag_dyneither(_tmp52D,sizeof(char),6))),_tag_dyneither(_tmp52C,sizeof(void*),1))))))));}_LL16F: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2BB=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2BB->tag != 5)goto _LL171;}_LL170: {
# 1268
const char*_tmp531;void*_tmp530[1];struct Cyc_String_pa_PrintArg_struct _tmp52F;return(struct _dyneither_ptr)((_tmp52F.tag=0,((_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AC)),((_tmp530[0]=& _tmp52F,Cyc_aprintf(((_tmp531="&%s",_tag_dyneither(_tmp531,sizeof(char),4))),_tag_dyneither(_tmp530,sizeof(void*),1))))))));}_LL171: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2BC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2BC->tag != 6)goto _LL173;else{if(_tmp2BC->f1 == 0)goto _LL173;if((((_tmp2BC->f1)->aggr_info).KnownAggr).tag != 2)goto _LL173;_tmp2BD=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2BC->f1)->aggr_info).KnownAggr).val);_tmp2BE=_tmp2BC->f3;}}_LL172:
# 1270
 if(_tmp2BD->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2DD=_tmp2BE;struct _dyneither_ptr*_tmp2DF;_LL186: if(_tmp2DD == 0)goto _LL188;if(((struct _tuple15*)_tmp2DD->hd)->f1 == 0)goto _LL188;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2DE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2DD->hd)->f1)->hd);if(_tmp2DE->tag != 1)goto _LL188;else{_tmp2DF=_tmp2DE->f1;}};_LL187: {
# 1273
const char*_tmp537;void*_tmp536[3];struct Cyc_String_pa_PrintArg_struct _tmp535;struct Cyc_String_pa_PrintArg_struct _tmp534;struct Cyc_String_pa_PrintArg_struct _tmp533;return(struct _dyneither_ptr)((_tmp533.tag=0,((_tmp533.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2AC)),((_tmp534.tag=0,((_tmp534.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DF),((_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1273
Cyc_Absynpp_qvar2string(_tmp2BD->name)),((_tmp536[0]=& _tmp535,((_tmp536[1]=& _tmp534,((_tmp536[2]=& _tmp533,Cyc_aprintf(((_tmp537="%s{.%s = %s}",_tag_dyneither(_tmp537,sizeof(char),13))),_tag_dyneither(_tmp536,sizeof(void*),3))))))))))))))))))));}_LL188:;_LL189:
# 1275
 goto _LL185;_LL185:;}{
# 1278
const char*_tmp53C;void*_tmp53B[2];struct Cyc_String_pa_PrintArg_struct _tmp53A;struct Cyc_String_pa_PrintArg_struct _tmp539;return(struct _dyneither_ptr)((_tmp539.tag=0,((_tmp539.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AC)),((_tmp53A.tag=0,((_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BD->name)),((_tmp53B[0]=& _tmp53A,((_tmp53B[1]=& _tmp539,Cyc_aprintf(((_tmp53C="%s{%s}",_tag_dyneither(_tmp53C,sizeof(char),7))),_tag_dyneither(_tmp53B,sizeof(void*),2))))))))))))));};_LL173: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2BF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2BF->tag != 7)goto _LL175;else{_tmp2C0=_tmp2BF->f2;}}_LL174:
# 1280
 if(_tmp2AC == 0){
const char*_tmp540;void*_tmp53F[1];struct Cyc_String_pa_PrintArg_struct _tmp53E;return(struct _dyneither_ptr)((_tmp53E.tag=0,((_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C0->name)),((_tmp53F[0]=& _tmp53E,Cyc_aprintf(((_tmp540="%s",_tag_dyneither(_tmp540,sizeof(char),3))),_tag_dyneither(_tmp53F,sizeof(void*),1))))))));}else{
# 1283
const char*_tmp545;void*_tmp544[2];struct Cyc_String_pa_PrintArg_struct _tmp543;struct Cyc_String_pa_PrintArg_struct _tmp542;return(struct _dyneither_ptr)((_tmp542.tag=0,((_tmp542.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2AC)),((_tmp543.tag=0,((_tmp543.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2C0->name)),((_tmp544[0]=& _tmp543,((_tmp544[1]=& _tmp542,Cyc_aprintf(((_tmp545="%s(%s)",_tag_dyneither(_tmp545,sizeof(char),7))),_tag_dyneither(_tmp544,sizeof(void*),2))))))))))))));}_LL175: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2C1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2C1->tag != 8)goto _LL177;}_LL176: {
const char*_tmp546;return(_tmp546="NULL",_tag_dyneither(_tmp546,sizeof(char),5));}_LL177: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2C2=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2C2->tag != 9)goto _LL179;else{_tmp2C3=_tmp2C2->f2;}}_LL178: {
const char*_tmp54A;void*_tmp549[1];struct Cyc_Int_pa_PrintArg_struct _tmp548;return(struct _dyneither_ptr)((_tmp548.tag=1,((_tmp548.f1=(unsigned long)_tmp2C3,((_tmp549[0]=& _tmp548,Cyc_aprintf(((_tmp54A="%d",_tag_dyneither(_tmp54A,sizeof(char),3))),_tag_dyneither(_tmp549,sizeof(void*),1))))))));}_LL179: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2C4=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2C4->tag != 10)goto _LL17B;else{_tmp2C5=_tmp2C4->f1;}}_LL17A: {
const char*_tmp54E;void*_tmp54D[1];struct Cyc_Int_pa_PrintArg_struct _tmp54C;return(struct _dyneither_ptr)((_tmp54C.tag=1,((_tmp54C.f1=(unsigned long)((int)_tmp2C5),((_tmp54D[0]=& _tmp54C,Cyc_aprintf(((_tmp54E="%d",_tag_dyneither(_tmp54E,sizeof(char),3))),_tag_dyneither(_tmp54D,sizeof(void*),1))))))));}_LL17B: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2C6=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2C6->tag != 11)goto _LL17D;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f2;}}_LL17C:
 return _tmp2C7;_LL17D: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2C9=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2C9->tag != 12)goto _LL17F;else{_tmp2CA=_tmp2C9->f2;}}_LL17E:
 _tmp2CC=_tmp2CA;goto _LL180;_LL17F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2CB=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2CB->tag != 13)goto _LL181;else{_tmp2CC=_tmp2CB->f2;}}_LL180:
 return Cyc_Absynpp_qvar2string(_tmp2CC->name);_LL181: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2CD=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2CD->tag != 16)goto _LL183;else{_tmp2CE=_tmp2CD->f1;}}_LL182:
 return Cyc_Absynpp_exp2string(_tmp2CE);_LL183:;_LL184:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL164:;};}_LL162: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2AD=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A9;if(_tmp2AD->tag != 1)goto _LL15F;else{_tmp2AE=_tmp2AD->f1;}}_LL163:
# 1294
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2AE)){const char*_tmp54F;return(_tmp54F="_",_tag_dyneither(_tmp54F,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp2F8=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2AE);
struct Cyc_Absyn_Pat*_tmp2F9=_tmp2F8->orig_pat;
# 1299
if(_tmp2F9 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp2FA=Cyc_Tcutil_compress((void*)_check_null(_tmp2F9->topt));struct Cyc_Absyn_PtrAtts _tmp2FE;struct Cyc_Absyn_Datatypedecl*_tmp300;struct Cyc_Absyn_Aggrdecl*_tmp302;_LL18B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2FA;if(_tmp2FB->tag != 6)goto _LL18D;else{if(_tmp2FB->f2 != Cyc_Absyn_Char_sz)goto _LL18D;}}_LL18C:
# 1303
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp303=Cyc_Tcpat_char_con(r,(char)i,_tmp2F9);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AE,_tmp303)){
const char*_tmp553;void*_tmp552[1];struct Cyc_Int_pa_PrintArg_struct _tmp551;return(struct _dyneither_ptr)((_tmp551.tag=1,((_tmp551.f1=(unsigned long)i,((_tmp552[0]=& _tmp551,Cyc_aprintf(((_tmp553="%d",_tag_dyneither(_tmp553,sizeof(char),3))),_tag_dyneither(_tmp552,sizeof(void*),1))))))));}}}
# 1308
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2FA;if(_tmp2FC->tag != 6)goto _LL18F;}_LL18E:
# 1311
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp307=Cyc_Tcpat_int_con(r,(int)i,_tmp2F9);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AE,_tmp307)){
const char*_tmp557;void*_tmp556[1];struct Cyc_Int_pa_PrintArg_struct _tmp555;return(struct _dyneither_ptr)((_tmp555.tag=1,((_tmp555.f1=(unsigned long)((int)i),((_tmp556[0]=& _tmp555,Cyc_aprintf(((_tmp557="%d",_tag_dyneither(_tmp557,sizeof(char),3))),_tag_dyneither(_tmp556,sizeof(void*),1))))))));}}}
# 1316
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA;if(_tmp2FD->tag != 5)goto _LL191;else{_tmp2FE=(_tmp2FD->f1).ptr_atts;}}_LL190: {
# 1318
union Cyc_Absyn_Constraint*_tmp30B=_tmp2FE.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp30B);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AE,Cyc_Tcpat_null_con(r,_tmp2F9))){
const char*_tmp558;return(_tmp558="NULL",_tag_dyneither(_tmp558,sizeof(char),5));}}{
# 1324
const char*_tmp559;return(_tmp559="&_",_tag_dyneither(_tmp559,sizeof(char),3));};}_LL191: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2FF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2FA;if(_tmp2FF->tag != 3)goto _LL193;else{if((((_tmp2FF->f1).datatype_info).KnownDatatype).tag != 2)goto _LL193;_tmp300=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp2FF->f1).datatype_info).KnownDatatype).val);}}_LL192:
# 1326
 if(_tmp300->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp30E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp300->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30E);
for(0;(unsigned int)_tmp30E;_tmp30E=_tmp30E->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp30E->hd)->name).f2;
struct Cyc_List_List*_tmp30F=((struct Cyc_Absyn_Datatypefield*)_tmp30E->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp55A;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AE,((_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A->name=Cyc_Tcpat_Name_v(n),((_tmp55A->arity=0,((_tmp55A->span=0,((_tmp55A->orig_pat=_tmp2F9,_tmp55A)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F)== 0)
return n;else{
# 1336
const char*_tmp55E;void*_tmp55D[1];struct Cyc_String_pa_PrintArg_struct _tmp55C;return(struct _dyneither_ptr)((_tmp55C.tag=0,((_tmp55C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp55D[0]=& _tmp55C,Cyc_aprintf(((_tmp55E="%s(...)",_tag_dyneither(_tmp55E,sizeof(char),8))),_tag_dyneither(_tmp55D,sizeof(void*),1))))))));}}}
# 1339
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL193:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp301=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2FA;if(_tmp301->tag != 11)goto _LL195;else{if((((_tmp301->f1).aggr_info).KnownAggr).tag != 2)goto _LL195;_tmp302=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp301->f1).aggr_info).KnownAggr).val);}}if(!(_tmp302->kind == Cyc_Absyn_UnionA))goto _LL195;_LL194: {
# 1341
struct Cyc_List_List*_tmp314=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp314);
struct _dyneither_ptr _tmp316;struct _tuple1*_tmp315=_tmp302->name;_tmp316=*_tmp315->f2;
for(0;(unsigned int)_tmp314;_tmp314=_tmp314->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp314->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp55F;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2AE,((_tmp55F=_cycalloc(sizeof(*_tmp55F)),((_tmp55F->name=Cyc_Tcpat_Name_v(n),((_tmp55F->arity=0,((_tmp55F->span=0,((_tmp55F->orig_pat=_tmp2F9,_tmp55F)))))))))))){
const char*_tmp564;void*_tmp563[2];struct Cyc_String_pa_PrintArg_struct _tmp562;struct Cyc_String_pa_PrintArg_struct _tmp561;return(struct _dyneither_ptr)((_tmp561.tag=0,((_tmp561.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp562.tag=0,((_tmp562.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp316),((_tmp563[0]=& _tmp562,((_tmp563[1]=& _tmp561,Cyc_aprintf(((_tmp564="%s{.%s = _}",_tag_dyneither(_tmp564,sizeof(char),12))),_tag_dyneither(_tmp563,sizeof(void*),2))))))))))))));}}
# 1349
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL195:;_LL196:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18A:;};};_LL15F:;}
# 1354
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp565;return(_tmp565=_cycalloc(sizeof(*_tmp565)),((_tmp565[0]=Cyc_Tcpat_desc2string(r,d),_tmp565)));}
# 1357
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp31D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp31E=_tmp31D;for(0;_tmp31E != 0;_tmp31E=((struct Cyc_List_List*)_check_null(_tmp31E))->tl){
if(_tmp31E->tl != 0){
{struct Cyc_List_List*_tmp566;_tmp31E->tl=((_tmp566=_cycalloc(sizeof(*_tmp566)),((_tmp566->hd=comma,((_tmp566->tl=_tmp31E->tl,_tmp566))))));}
_tmp31E=_tmp31E->tl;}}}
# 1366
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp31D);}
# 1369
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp321;_push_handler(& _tmp321);{int _tmp323=0;if(setjmp(_tmp321.handler))_tmp323=1;if(!_tmp323){
{struct _dyneither_ptr _tmp324=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp56A;void*_tmp569[1];struct Cyc_String_pa_PrintArg_struct _tmp568;(_tmp568.tag=0,((_tmp568.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp324),((_tmp569[0]=& _tmp568,Cyc_Tcutil_terr(loc,((_tmp56A="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp56A,sizeof(char),53))),_tag_dyneither(_tmp569,sizeof(void*),1)))))));}
# 1371
;_pop_handler();}else{void*_tmp322=(void*)_exn_thrown;void*_tmp329=_tmp322;void*_tmp32B;_LL198: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp32A=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp329;if(_tmp32A->tag != Cyc_Tcpat_Desc2string)goto _LL19A;}_LL199:
# 1375
{const char*_tmp56D;void*_tmp56C;(_tmp56C=0,Cyc_Tcutil_terr(loc,((_tmp56D="patterns may not be exhaustive.",_tag_dyneither(_tmp56D,sizeof(char),32))),_tag_dyneither(_tmp56C,sizeof(void*),0)));}
goto _LL197;_LL19A: _tmp32B=_tmp329;_LL19B:(void)_throw(_tmp32B);_LL197:;}};}
# 1379
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple19*rhs){
(*rhs).f1=1;}
# 1383
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1388
struct _RegionHandle _tmp32E=_new_region("r");struct _RegionHandle*r=& _tmp32E;_push_region(r);
{int _tmp32F=0;
struct _tuple23 _tmp56E;struct _tuple23 _tmp330=(_tmp56E.f1=r,((_tmp56E.f2=& _tmp32F,_tmp56E)));
struct Cyc_List_List*_tmp331=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp330,swcs);
void*_tmp332=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp331);
int _tmp333=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp332,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp333);
# 1396
for(0;_tmp331 != 0;_tmp331=_tmp331->tl){
int _tmp335;unsigned int _tmp336;struct _tuple22*_tmp334=(struct _tuple22*)_tmp331->hd;_tmp335=(_tmp334->f2)->f1;_tmp336=(_tmp334->f2)->f2;
if(!_tmp335){
{const char*_tmp571;void*_tmp570;(_tmp570=0,Cyc_Tcutil_terr(_tmp336,((_tmp571="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp571,sizeof(char),74))),_tag_dyneither(_tmp570,sizeof(void*),0)));}
# 1401
break;}}}
# 1389
;_pop_region(r);}
# 1411
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp33A;_push_handler(& _tmp33A);{int _tmp33C=0;if(setjmp(_tmp33A.handler))_tmp33C=1;if(!_tmp33C){
{struct _dyneither_ptr _tmp33D=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp575;void*_tmp574[1];struct Cyc_String_pa_PrintArg_struct _tmp573;(_tmp573.tag=0,((_tmp573.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp33D),((_tmp574[0]=& _tmp573,Cyc_Tcutil_warn((*pr).f1,((_tmp575="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp575,sizeof(char),45))),_tag_dyneither(_tmp574,sizeof(void*),1)))))));}
# 1414
;_pop_handler();}else{void*_tmp33B=(void*)_exn_thrown;void*_tmp342=_tmp33B;void*_tmp344;_LL19D: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp343=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp342;if(_tmp343->tag != Cyc_Tcpat_Desc2string)goto _LL19F;}_LL19E:
# 1418
{const char*_tmp578;void*_tmp577;(_tmp577=0,Cyc_Tcutil_warn((*pr).f1,((_tmp578="pattern not exhaustive.",_tag_dyneither(_tmp578,sizeof(char),24))),_tag_dyneither(_tmp577,sizeof(void*),0)));}
goto _LL19C;_LL19F: _tmp344=_tmp342;_LL1A0:(void)_throw(_tmp344);_LL19C:;}};};}
# 1422
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple24{void*f1;int f2;};
# 1425
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp347=_new_region("r");struct _RegionHandle*r=& _tmp347;_push_region(r);
{struct _tuple24*_tmp57B;struct Cyc_List_List*_tmp57A;struct Cyc_List_List*_tmp348=(_tmp57A=_region_malloc(r,sizeof(*_tmp57A)),((_tmp57A->hd=((_tmp57B=_region_malloc(r,sizeof(*_tmp57B)),((_tmp57B->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp57B->f2=0,_tmp57B)))))),((_tmp57A->tl=0,_tmp57A)))));
void*_tmp349=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp348);
struct _tuple14 _tmp57C;struct _tuple14 _tmp34A=(_tmp57C.f1=loc,((_tmp57C.f2=1,_tmp57C)));
int _tmp34B=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp349,Cyc_Tcpat_not_exhaust_warn,& _tmp34A,Cyc_Tcpat_dummy_fn,0,& _tmp34B);{
# 1433
int _tmp34C=_tmp34A.f2;_npop_handler(0);return _tmp34C;};}
# 1427
;_pop_region(r);}
# 1441
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1443
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple19*_tmp57D;struct _tuple19*rhs=(_tmp57D=_region_malloc(r,sizeof(*_tmp57D)),((_tmp57D->f1=0,((_tmp57D->f2=(swc->pattern)->loc,_tmp57D)))));
struct _tuple22*_tmp57E;return(_tmp57E=_region_malloc(r,sizeof(*_tmp57E)),((_tmp57E->f1=sp0,((_tmp57E->f2=rhs,_tmp57E)))));}
# 1447
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1450
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1452
struct _RegionHandle _tmp352=_new_region("r");struct _RegionHandle*r=& _tmp352;_push_region(r);
{struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp354=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp353);
int _tmp355=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp354,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp355);
# 1458
for(0;_tmp353 != 0;_tmp353=_tmp353->tl){
int _tmp357;unsigned int _tmp358;struct _tuple22*_tmp356=(struct _tuple22*)_tmp353->hd;_tmp357=(_tmp356->f2)->f1;_tmp358=(_tmp356->f2)->f2;
if(!_tmp357){
{const char*_tmp581;void*_tmp580;(_tmp580=0,Cyc_Tcutil_terr(_tmp358,((_tmp581="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp581,sizeof(char),74))),_tag_dyneither(_tmp580,sizeof(void*),0)));}
break;}}}
# 1453
;_pop_region(r);}
