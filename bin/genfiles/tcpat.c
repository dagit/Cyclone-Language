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
# 100
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
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
# 283
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
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
{const char*_tmp37F;void*_tmp37E;(_tmp37E=0,Cyc_Tcutil_terr(p->loc,((_tmp37F="struct tag used without arguments in pattern",_tag_dyneither(_tmp37F,sizeof(char),45))),_tag_dyneither(_tmp37E,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp382;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp381;p->r=(void*)((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381[0]=((_tmp382.tag=7,((_tmp382.f1=_tmp1B,((_tmp382.f2=_tmp1C,((_tmp382.f3=0,((_tmp382.f4=0,_tmp382)))))))))),_tmp381))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp385;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp384;p->r=(void*)((_tmp384=_cycalloc(sizeof(*_tmp384)),((_tmp384[0]=((_tmp385.tag=12,((_tmp385.f1=_tmp1E,((_tmp385.f2=_tmp1F,_tmp385)))))),_tmp384))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp388;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp387;p->r=(void*)((_tmp387=_cycalloc(sizeof(*_tmp387)),((_tmp387[0]=((_tmp388.tag=13,((_tmp388.f1=_tmp21,((_tmp388.f2=_tmp22,_tmp388)))))),_tmp387))));}
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
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp38B;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp38A;p->r=(void*)((_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A[0]=((_tmp38B.tag=1,((_tmp38B.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp38B.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp38B)))))),_tmp38A))));}
return;_LL22:;_LL23:
{const char*_tmp38E;void*_tmp38D;(_tmp38D=0,Cyc_Tcutil_terr(p->loc,((_tmp38E="qualified variable in pattern",_tag_dyneither(_tmp38E,sizeof(char),30))),_tag_dyneither(_tmp38D,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 85
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_stmttmp2=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);void*_tmp39=_stmttmp2;struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp391;struct Cyc_List_List*_tmp390;_tmp42=((_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390->hd=((_tmp391=_cycalloc(sizeof(*_tmp391)),((_tmp391->f1=0,((_tmp391->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp391)))))),((_tmp390->tl=_tmp42,_tmp390))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp39B;struct Cyc_Absyn_Aggrdecl**_tmp39A;struct Cyc_Absyn_AggrInfo*_tmp399;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp398;p->r=(void*)((_tmp398=_cycalloc(sizeof(*_tmp398)),((_tmp398[0]=((_tmp39B.tag=6,((_tmp39B.f1=((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399->aggr_info=Cyc_Absyn_KnownAggr(((_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A[0]=_tmp3B,_tmp39A))))),((_tmp399->targs=0,_tmp399)))))),((_tmp39B.f2=0,((_tmp39B.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp39B.f4=_tmp6,_tmp39B)))))))))),_tmp398))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp39E;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp39D;p->r=(void*)((_tmp39D=_cycalloc(sizeof(*_tmp39D)),((_tmp39D[0]=((_tmp39E.tag=7,((_tmp39E.f1=_tmp3D,((_tmp39E.f2=_tmp3E,((_tmp39E.f3=_tmp5,((_tmp39E.f4=_tmp6,_tmp39E)))))))))),_tmp39D))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp3A1;void*_tmp3A0;(_tmp3A0=0,Cyc_Tcutil_terr(p->loc,((_tmp3A1="enum tag used with arguments in pattern",_tag_dyneither(_tmp3A1,sizeof(char),40))),_tag_dyneither(_tmp3A0,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp3A5;void*_tmp3A4[1];struct Cyc_String_pa_PrintArg_struct _tmp3A3;(_tmp3A3.tag=0,((_tmp3A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp3A4[0]=& _tmp3A3,Cyc_Tcutil_terr(p->loc,((_tmp3A5="%s is not a constructor in pattern",_tag_dyneither(_tmp3A5,sizeof(char),35))),_tag_dyneither(_tmp3A4,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_terr(p->loc,((_tmp3A8="cannot determine pattern type",_tag_dyneither(_tmp3A8,sizeof(char),30))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3AE;struct Cyc_Absyn_AggrInfo*_tmp3AD;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3AC;p->r=(void*)((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC[0]=((_tmp3AE.tag=6,((_tmp3AE.f1=((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD[0]=_tmp58,_tmp3AD)))),((_tmp3AE.f2=_tmp8,((_tmp3AE.f3=_tmp9,((_tmp3AE.f4=_tmpA,_tmp3AE)))))))))),_tmp3AC))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp3B2;void*_tmp3B1[1];struct Cyc_String_pa_PrintArg_struct _tmp3B0;(_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp3B1[0]=& _tmp3B0,Cyc_Tcutil_terr(p->loc,((_tmp3B2="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp3B2,sizeof(char),45))),_tag_dyneither(_tmp3B1,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple4)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 126
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp3BA;void*_tmp3B9[1];const char*_tmp3B8;const char*_tmp3B7;struct Cyc_String_pa_PrintArg_struct _tmp3B6;(_tmp3B6.tag=0,((_tmp3B6.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp3B7="struct",_tag_dyneither(_tmp3B7,sizeof(char),7)):((_tmp3B8="union",_tag_dyneither(_tmp3B8,sizeof(char),6)))),((_tmp3B9[0]=& _tmp3B6,Cyc_Tcutil_terr(p->loc,((_tmp3BA="can't destructure an abstract %s",_tag_dyneither(_tmp3BA,sizeof(char),33))),_tag_dyneither(_tmp3B9,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp3BD;void*_tmp3BC;(_tmp3BC=0,Cyc_Tcutil_terr(p->loc,((_tmp3BD="too many existentially bound type variables in pattern",_tag_dyneither(_tmp3BD,sizeof(char),55))),_tag_dyneither(_tmp3BC,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3C3;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3C2;struct Cyc_List_List*_tmp3C1;_tmp6C=((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1->hd=Cyc_Tcutil_new_tvar((void*)((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C2.tag=1,((_tmp3C2.f1=0,_tmp3C2)))),_tmp3C3))))),((_tmp3C1->tl=_tmp6C,_tmp3C1))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3C9;struct Cyc_Absyn_AggrInfo*_tmp3C8;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3C7;p->r=(void*)((_tmp3C7=_cycalloc(sizeof(*_tmp3C7)),((_tmp3C7[0]=((_tmp3C9.tag=6,((_tmp3C9.f1=((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((_tmp3C8->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3C8->targs=_tmpE,_tmp3C8)))))),((_tmp3C9.f2=*_tmpF,((_tmp3C9.f3=_tmp10,((_tmp3C9.f4=_tmp11,_tmp3C9)))))))))),_tmp3C7))));};};}
# 127
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp3CC;void*_tmp3CB;(_tmp3CB=0,Cyc_Tcutil_terr(p->loc,((_tmp3CC="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3CC,sizeof(char),43))),_tag_dyneither(_tmp3CB,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3CF;void*_tmp3CE;(_tmp3CE=0,Cyc_Tcutil_terr(p->loc,((_tmp3CF="non-constant expression in case pattern",_tag_dyneither(_tmp3CF,sizeof(char),40))),_tag_dyneither(_tmp3CE,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _stmttmp1=Cyc_Evexp_eval_const_uint_exp(_tmp13);unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=_stmttmp1;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3D2;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3D1;p->r=(void*)((_tmp3D1=_cycalloc_atomic(sizeof(*_tmp3D1)),((_tmp3D1[0]=((_tmp3D2.tag=9,((_tmp3D2.f1=Cyc_Absyn_None,((_tmp3D2.f2=(int)_tmp7C,_tmp3D2)))))),_tmp3D1))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3D3;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->v=s,_tmp3D3)))));};}
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
struct _tuple16*_tmp3D6;struct Cyc_List_List*_tmp3D5;*v_result_ptr=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->hd=((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6->f1=vd,((_tmp3D6->f2=e,_tmp3D6)))))),((_tmp3D5->tl=*v_result_ptr,_tmp3D5))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_Tcpat_TcPatResult _tmp86=res1;_tmp87=_tmp86.tvars_and_bounds_opt;_tmp88=_tmp86.patvars;{
struct _tuple1*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult _tmp89=res2;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;
if(_tmp87 != 0  || _tmp8A != 0){
if(_tmp87 == 0){
struct _tuple1*_tmp3D7;_tmp87=((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7->f1=0,((_tmp3D7->f2=0,_tmp3D7))))));}
if(_tmp8A == 0){
struct _tuple1*_tmp3D8;_tmp8A=((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8->f1=0,((_tmp3D8->f2=0,_tmp3D8))))));}{
struct _tuple1*_tmp3DB;struct Cyc_Tcpat_TcPatResult _tmp3DA;return(_tmp3DA.tvars_and_bounds_opt=((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f1,(*_tmp8A).f1),((_tmp3DB->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp87).f2,(*_tmp8A).f2),_tmp3DB)))))),((_tmp3DA.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3DA)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp3DC;return(_tmp3DC.tvars_and_bounds_opt=0,((_tmp3DC.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88,_tmp8B),_tmp3DC)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3DD;return(_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->loc=loc,((_tmp3DD->topt=0,((_tmp3DD->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3DD)))))));}
# 221
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_stmttmp6=Cyc_Tcutil_compress(t);void*_tmp92=_stmttmp6;void*_tmp94;struct Cyc_Absyn_Tqual _tmp95;struct Cyc_Absyn_Exp*_tmp96;union Cyc_Absyn_Constraint*_tmp97;unsigned int _tmp98;_LL46: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92;if(_tmp93->tag != 8)goto _LL48;else{_tmp94=(_tmp93->f1).elt_type;_tmp95=(_tmp93->f1).tq;_tmp96=(_tmp93->f1).num_elts;_tmp97=(_tmp93->f1).zero_term;_tmp98=(_tmp93->f1).zt_loc;}}_LL47: {
# 224
struct Cyc_Core_Opt*_tmp3DE;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3DE))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3E1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3E0;return Cyc_Absyn_atb_typ(_tmp94,rgn,_tmp95,(void*)((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0[0]=((_tmp3E1.tag=1,((_tmp3E1.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp96),_tmp3E1)))),_tmp3E0)))),_tmp97);}_LL48:;_LL49:
 return t;_LL45:;}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 232
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 235
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3E2;struct Cyc_Tcpat_TcPatResult res=(_tmp3E2.tvars_and_bounds_opt=0,((_tmp3E2.patvars=0,_tmp3E2)));
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
const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(p->loc,((_tmp3E5="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3E5,sizeof(char),58))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}
goto _LL75;_LL78:;_LL79:
# 259
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3E8;void*_tmp3E7;(_tmp3E7=0,Cyc_Tcutil_terr(p->loc,((_tmp3E8="pattern would point to an abstract member",_tag_dyneither(_tmp3E8,sizeof(char),42))),_tag_dyneither(_tmp3E7,sizeof(void*),0)));}
goto _LL75;_LL75:;}
# 263
{struct Cyc_Absyn_Vardecl**_tmp3E9;Cyc_Tcpat_set_vd(((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=_tmp9F,_tmp3E9)))),access_exp,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));}
goto _LL4A;_LL4F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA1->tag != 2)goto _LL51;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;}}_LL50:
# 266
 res=Cyc_Tcpat_tcPatRec(te,_tmpA3,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA3->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_terr(p->loc,((_tmp3EC="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3EC,sizeof(char),55))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}
goto _LL4A;}else{
# 273
struct _RegionHandle _tmpD0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD0;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_Tcutil_terr(p->loc,((_tmp3EF="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3EF,sizeof(char),56))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}
# 274
;_pop_region(rgn);}{
# 279
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3F5;struct Cyc_Absyn_PtrInfo _tmp3F4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F3;void*t2=(void*)((_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3[0]=((_tmp3F5.tag=5,((_tmp3F5.f1=((_tmp3F4.elt_typ=t,((_tmp3F4.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3F4.ptr_atts=(
((_tmp3F4.ptr_atts).rgn=rgn_pat,(((_tmp3F4.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3F4.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F4.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3F4.ptr_atts).ptrloc=0,_tmp3F4.ptr_atts)))))))))),_tmp3F4)))))),_tmp3F5)))),_tmp3F3))));
# 284
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 288
{struct Cyc_Absyn_Vardecl**_tmp3F6;Cyc_Tcpat_set_vd(((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6[0]=_tmpA2,_tmp3F6)))),new_access_exp,& res.patvars,t2);}
goto _LL4A;};_LL51: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpA4->tag != 3)goto _LL53;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;}}_LL52:
# 294
{struct Cyc_Absyn_Vardecl**_tmp3F7;Cyc_Tcpat_set_vd(((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7[0]=_tmpA6,_tmp3F7)))),access_exp,& res.patvars,_tmpA6->type);}{
# 298
struct _RegionHandle _tmpD8=_new_region("r2");struct _RegionHandle*r2=& _tmpD8;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3F8[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3F8[0]=_tmpA5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3F8,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 301
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp3F9;res.tvars_and_bounds_opt=((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->f1=0,((_tmp3F9->f2=0,_tmp3F9))))));}
{struct Cyc_List_List*_tmp3FA;(*res.tvars_and_bounds_opt).f1=(
(_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA->hd=_tmpA5,((_tmp3FA->tl=(*res.tvars_and_bounds_opt).f1,_tmp3FA))))));}
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
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3FD;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3FC;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=((_tmp3FD.tag=13,((_tmp3FD.f1=_tmpAE->name,((_tmp3FD.f2=_tmpAE,_tmp3FD)))))),_tmp3FC)))));}
goto _LL4A;_LL5F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpAF->tag != 13)goto _LL61;else{_tmpB0=(void*)_tmpAF->f1;}}_LL60:
 t=Cyc_Tcpat_num_type(topt,_tmpB0);goto _LL4A;_LL61: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB1->tag != 8)goto _LL63;}_LL62:
# 318
 if(topt != 0){
void*_stmttmpD=Cyc_Tcutil_compress(*topt);void*_tmpDE=_stmttmpD;_LL7B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDE;if(_tmpDF->tag != 5)goto _LL7D;}_LL7C:
 t=*topt;goto tcpat_end;_LL7D:;_LL7E:
 goto _LL7A;_LL7A:;}{
# 323
struct Cyc_Core_Opt*_tmpE0=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp403;struct Cyc_Absyn_PtrInfo _tmp402;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp401;t=(void*)((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=((_tmp403.tag=5,((_tmp403.f1=((_tmp402.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpE0),((_tmp402.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp402.ptr_atts=(
((_tmp402.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpE0),(((_tmp402.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp402.ptr_atts).bounds=
# 328
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp402.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp402.ptr_atts).ptrloc=0,_tmp402.ptr_atts)))))))))),_tmp402)))))),_tmp403)))),_tmp401))));}
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
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp40D;struct Cyc_Absyn_Datatypedecl**_tmp40C;struct Cyc_Absyn_DatatypeInfo _tmp40B;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp40A;void*new_type=(void*)((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A[0]=((_tmp40D.tag=3,((_tmp40D.f1=((_tmp40B.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=_tmpEB,_tmp40C))))),((_tmp40B.targs=_tmpED,_tmp40B)))),_tmp40D)))),_tmp40A))));
# 373
_tmpB3->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp413;struct Cyc_Absyn_PtrInfo _tmp412;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp411;t=(void*)((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=((_tmp413.tag=5,((_tmp413.f1=((_tmp412.elt_typ=new_type,((_tmp412.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp412.ptr_atts=(
((_tmp412.ptr_atts).rgn=ptr_rgn,(((_tmp412.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp412.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp412.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp412.ptr_atts).ptrloc=0,_tmp412.ptr_atts)))))))))),_tmp412)))))),_tmp413)))),_tmp411))));}
# 380
goto _LL84;};_LL87:;_LL88:
# 382
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp419;struct Cyc_Absyn_PtrInfo _tmp418;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp417;t=(void*)((_tmp417=_cycalloc(sizeof(*_tmp417)),((_tmp417[0]=((_tmp419.tag=5,((_tmp419.f1=((_tmp418.elt_typ=(void*)_check_null(_tmpB3->topt),((_tmp418.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp418.ptr_atts=(
((_tmp418.ptr_atts).rgn=ptr_rgn,(((_tmp418.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp418.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp418.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp418.ptr_atts).ptrloc=0,_tmp418.ptr_atts)))))))))),_tmp418)))))),_tmp419)))),_tmp417))));}_LL84:;}
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
struct Cyc_List_List*_tmp41A;wild_ps=((_tmp41A=_cycalloc(sizeof(*_tmp41A)),((_tmp41A->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp41A->tl=wild_ps,_tmp41A))))));}}
*_tmpB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpFA,wild_ps);
_tmpFA=*_tmpB5;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFA)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpFD)){
const char*_tmp41D;void*_tmp41C;(_tmp41C=0,Cyc_Tcutil_warn(p->loc,((_tmp41D="unnecessary ... in tuple pattern",_tag_dyneither(_tmp41D,sizeof(char),33))),_tag_dyneither(_tmp41C,sizeof(void*),0)));}}}
# 415
goto _LL8E;_LL91:;_LL92:
# 417
 goto _LL8E;_LL8E:;}else{
# 419
if(_tmpB6){
const char*_tmp420;void*_tmp41F;(_tmp41F=0,Cyc_Tcutil_terr(p->loc,((_tmp420="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp420,sizeof(char),57))),_tag_dyneither(_tmp41F,sizeof(void*),0)));}}
{int i=0;for(0;_tmpFA != 0;(_tmpFA=_tmpFA->tl,i ++)){
void**_tmp105=0;
if(topt_ts != 0){
_tmp105=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 427
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp423;union Cyc_Absyn_Cnst _tmp422;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp422.Int_c).val=((_tmp423.f1=Cyc_Absyn_Unsigned,((_tmp423.f2=i,_tmp423)))),(((_tmp422.Int_c).tag=5,_tmp422)))),0),0);}
# 432
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpFA->hd,_tmp105,rgn_pat,allow_ref_pat,new_access_exp));
# 435
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt;{
struct _tuple17*_tmp426;struct Cyc_List_List*_tmp425;pat_ts=((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425->hd=((_tmp426=_cycalloc(sizeof(*_tmp426)),((_tmp426->f1=Cyc_Absyn_empty_tqual(0),((_tmp426->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpFA->hd)->topt),_tmp426)))))),((_tmp425->tl=pat_ts,_tmp425))))));};};}}
# 438
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp429;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp428;t=(void*)((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428[0]=((_tmp429.tag=10,((_tmp429.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp429)))),_tmp428))));}
goto _LL4A;}_LL67: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9C;if(_tmpB7->tag != 6)goto _LL69;else{if(_tmpB7->f1 == 0)goto _LL69;if((((_tmpB7->f1)->aggr_info).KnownAggr).tag != 2)goto _LL69;_tmpB8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB7->f1)->aggr_info).KnownAggr).val);_tmpB9=(struct Cyc_List_List**)&(_tmpB7->f1)->targs;_tmpBA=_tmpB7->f2;_tmpBB=(struct Cyc_List_List**)& _tmpB7->f3;_tmpBC=_tmpB7->f4;}}_LL68: {
# 442
struct Cyc_List_List*_tmp10C=*_tmpBB;
const char*_tmp42B;const char*_tmp42A;struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?(_tmp42B="struct",_tag_dyneither(_tmp42B,sizeof(char),7)):((_tmp42A="union",_tag_dyneither(_tmp42A,sizeof(char),6)));
if(_tmpB8->impl == 0){
{const char*_tmp42F;void*_tmp42E[1];struct Cyc_String_pa_PrintArg_struct _tmp42D;(_tmp42D.tag=0,((_tmp42D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp42E[0]=& _tmp42D,Cyc_Tcutil_terr(p->loc,((_tmp42F="can't destructure an abstract %s",_tag_dyneither(_tmp42F,sizeof(char),33))),_tag_dyneither(_tmp42E,sizeof(void*),1)))))));}
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
const char*_tmp432;void*_tmp431;(_tmp431=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp432="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp432,sizeof(char),53))),_tag_dyneither(_tmp431,sizeof(void*),0)));}_LL93:;}
# 476
{void*_tmp11C=_tmp113;struct Cyc_Absyn_Kind*_tmp11E;struct Cyc_Core_Opt**_tmp120;struct Cyc_Absyn_Kind*_tmp121;struct Cyc_Core_Opt**_tmp123;_LL9B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp11D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL9D;else{_tmp11E=_tmp11D->f1;}}_LL9C:
# 479
 if(!Cyc_Tcutil_kind_leq(k2,_tmp11E))
error=1;
goto _LL9A;_LL9D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp11F->tag != 2)goto _LL9F;else{_tmp120=(struct Cyc_Core_Opt**)& _tmp11F->f1;_tmp121=_tmp11F->f2;}}_LL9E:
 _tmp123=_tmp120;goto _LLA0;_LL9F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp122=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11C;if(_tmp122->tag != 1)goto _LL9A;else{_tmp123=(struct Cyc_Core_Opt**)& _tmp122->f1;}}_LLA0:
{struct Cyc_Core_Opt*_tmp433;*_tmp123=((_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433->v=_tmp114,_tmp433))));}goto _LL9A;_LL9A:;}
# 485
if(error){
const char*_tmp439;void*_tmp438[3];struct Cyc_String_pa_PrintArg_struct _tmp437;struct Cyc_String_pa_PrintArg_struct _tmp436;struct Cyc_String_pa_PrintArg_struct _tmp435;(_tmp435.tag=0,((_tmp435.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 489
Cyc_Absynpp_kind2string(k2)),((_tmp436.tag=0,((_tmp436.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp113)),((_tmp437.tag=0,((_tmp437.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp438[0]=& _tmp437,((_tmp438[1]=& _tmp436,((_tmp438[2]=& _tmp435,Cyc_Tcutil_terr(p->loc,((_tmp439="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp439,sizeof(char),60))),_tag_dyneither(_tmp438,sizeof(void*),3)))))))))))))))))));}{
# 491
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp43C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43B;void*vartype=(void*)((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=((_tmp43C.tag=2,((_tmp43C.f1=tv,_tmp43C)))),_tmp43B))));
{struct Cyc_List_List*_tmp43D;_tmp111=((_tmp43D=_region_malloc(rgn,sizeof(*_tmp43D)),((_tmp43D->hd=vartype,((_tmp43D->tl=_tmp111,_tmp43D))))));}
# 495
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp440;struct Cyc_List_List*_tmp43F;outlives_constraints=(
(_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F->hd=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->f1=Cyc_Absyn_empty_effect,((_tmp440->f2=vartype,_tmp440)))))),((_tmp43F->tl=outlives_constraints,_tmp43F))))));}else{
# 500
const char*_tmp443;void*_tmp442;(_tmp442=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp443="opened existential had unique or top region kind",_tag_dyneither(_tmp443,sizeof(char),49))),_tag_dyneither(_tmp442,sizeof(void*),0)));}}};};}}
# 504
_tmp111=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp111);{
# 506
struct _RegionHandle _tmp131=_new_region("r2");struct _RegionHandle*r2=& _tmp131;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBA);
# 509
struct Cyc_List_List*_tmp132=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp444;struct _tuple12 _tmp133=(_tmp444.f1=_tmp132,((_tmp444.f2=rgn,_tmp444)));
struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp133,_tmpB8->tvs);
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->exist_vars,_tmp111);
struct Cyc_List_List*_tmp136=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp134);
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp135);
struct Cyc_List_List*_tmp138=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp134,_tmp135);
# 517
if(_tmpBA != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp445;res.tvars_and_bounds_opt=((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445->f1=0,((_tmp445->f2=0,_tmp445))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpBA);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp13A=0;
{struct Cyc_List_List*_tmp13B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;_tmp13B != 0;_tmp13B=_tmp13B->tl){
struct _tuple0*_tmp448;struct Cyc_List_List*_tmp447;_tmp13A=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447->hd=((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f1),((_tmp448->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp138,(*((struct _tuple0*)_tmp13B->hd)).f2),_tmp448)))))),((_tmp447->tl=_tmp13A,_tmp447))))));}}
# 529
_tmp13A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13A);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp13A);};}
# 534
*_tmpB9=_tmp136;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp452;struct Cyc_Absyn_Aggrdecl**_tmp451;struct Cyc_Absyn_AggrInfo _tmp450;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp44F;t=(void*)((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=((_tmp452.tag=11,((_tmp452.f1=((_tmp450.aggr_info=Cyc_Absyn_KnownAggr(((_tmp451=_cycalloc(sizeof(*_tmp451)),((_tmp451[0]=_tmpB8,_tmp451))))),((_tmp450.targs=*_tmpB9,_tmp450)))),_tmp452)))),_tmp44F))));}
if(_tmpBC  && _tmpB8->kind == Cyc_Absyn_UnionA){
const char*_tmp455;void*_tmp454;(_tmp454=0,Cyc_Tcutil_warn(p->loc,((_tmp455="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp455,sizeof(char),43))),_tag_dyneither(_tmp454,sizeof(void*),0)));}else{
if(_tmpBC){
# 540
int _tmp144=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10C);
int _tmp145=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
if(_tmp144 < _tmp145){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp145 - _tmp144;++ i){
struct _tuple15*_tmp458;struct Cyc_List_List*_tmp457;wild_dps=((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457->hd=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->f1=0,((_tmp458->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp458)))))),((_tmp457->tl=wild_dps,_tmp457))))));}}
*_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10C,wild_dps);
_tmp10C=*_tmpBB;}else{
if(_tmp144 == _tmp145){
const char*_tmp45B;void*_tmp45A;(_tmp45A=0,Cyc_Tcutil_warn(p->loc,((_tmp45B="unnecessary ... in struct pattern",_tag_dyneither(_tmp45B,sizeof(char),34))),_tag_dyneither(_tmp45A,sizeof(void*),0)));}}}}{
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
const char*_tmp462;void*_tmp461[4];struct Cyc_String_pa_PrintArg_struct _tmp460;struct Cyc_String_pa_PrintArg_struct _tmp45F;struct Cyc_String_pa_PrintArg_struct _tmp45E;struct Cyc_String_pa_PrintArg_struct _tmp45D;(_tmp45D.tag=0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 568
Cyc_Absynpp_typ2string((void*)_check_null(_tmp14C->topt))),((_tmp45E.tag=0,((_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 567
Cyc_Absynpp_typ2string(_tmp14D)),((_tmp45F.tag=0,((_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp460.tag=0,((_tmp460.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp14B->name),((_tmp461[0]=& _tmp460,((_tmp461[1]=& _tmp45F,((_tmp461[2]=& _tmp45E,((_tmp461[3]=& _tmp45D,Cyc_Tcutil_terr(p->loc,((_tmp462="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp462,sizeof(char),45))),_tag_dyneither(_tmp461,sizeof(void*),4)))))))))))))))))))))))));}
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
struct _tuple12 _tmp463;struct _tuple12 _tmp15A=(_tmp463.f1=_tmp159,((_tmp463.f2=rgn,_tmp463)));
struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp15A,_tmpBE->tvs);
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp15B);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp469;struct Cyc_Absyn_DatatypeFieldInfo _tmp468;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp467;t=(void*)((_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467[0]=((_tmp469.tag=4,((_tmp469.f1=((_tmp468.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBE,_tmpBF),((_tmp468.targs=_tmp15C,_tmp468)))),_tmp469)))),_tmp467))));}
# 586
if(_tmpC1){
# 588
int _tmp160=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp157);
int _tmp161=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp160 < _tmp161){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp161 - _tmp160;++ i){
struct Cyc_List_List*_tmp46A;wild_ps=((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp46A->tl=wild_ps,_tmp46A))))));}}
*_tmpC0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp157,wild_ps);
_tmp157=*_tmpC0;}else{
if(_tmp160 == _tmp161){
const char*_tmp46E;void*_tmp46D[1];struct Cyc_String_pa_PrintArg_struct _tmp46C;(_tmp46C.tag=0,((_tmp46C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBE->name)),((_tmp46D[0]=& _tmp46C,Cyc_Tcutil_warn(p->loc,((_tmp46E="unnecessary ... in datatype field %s",_tag_dyneither(_tmp46E,sizeof(char),37))),_tag_dyneither(_tmp46D,sizeof(void*),1)))))));}}}
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
const char*_tmp474;void*_tmp473[3];struct Cyc_String_pa_PrintArg_struct _tmp472;struct Cyc_String_pa_PrintArg_struct _tmp471;struct Cyc_String_pa_PrintArg_struct _tmp470;(_tmp470.tag=0,((_tmp470.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 617
Cyc_Absynpp_typ2string((void*)_check_null(_tmp166->topt))),((_tmp471.tag=0,((_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 616
Cyc_Absynpp_typ2string(_tmp167)),((_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp473[0]=& _tmp472,((_tmp473[1]=& _tmp471,((_tmp473[2]=& _tmp470,Cyc_Tcutil_terr(_tmp166->loc,((_tmp474="%s expects argument type %s, not %s",_tag_dyneither(_tmp474,sizeof(char),36))),_tag_dyneither(_tmp473,sizeof(void*),3)))))))))))))))))));}}
# 619
if(_tmp157 != 0){
const char*_tmp478;void*_tmp477[1];struct Cyc_String_pa_PrintArg_struct _tmp476;(_tmp476.tag=0,((_tmp476.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp477[0]=& _tmp476,Cyc_Tcutil_terr(p->loc,((_tmp478="too many arguments for datatype constructor %s",_tag_dyneither(_tmp478,sizeof(char),47))),_tag_dyneither(_tmp477,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp47C;void*_tmp47B[1];struct Cyc_String_pa_PrintArg_struct _tmp47A;(_tmp47A.tag=0,((_tmp47A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBF->name)),((_tmp47B[0]=& _tmp47A,Cyc_Tcutil_terr(p->loc,((_tmp47C="too few arguments for datatype constructor %s",_tag_dyneither(_tmp47C,sizeof(char),46))),_tag_dyneither(_tmp47B,sizeof(void*),1)))))));}}
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
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp47D;_tmp179=((_tmp47D=_region_malloc(r,sizeof(*_tmp47D)),((_tmp47D->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp47D->tl=_tmp179,_tmp47D))))));}}}
{const char*_tmp47E;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp179),p->loc,(
(_tmp47E="pattern contains a repeated variable",_tag_dyneither(_tmp47E,sizeof(char),37))));}{
# 654
struct Cyc_List_List*x=_tmp175.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp10=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp17D;struct Cyc_Absyn_Exp**_tmp17E;struct _tuple16*_tmp17C=_stmttmp10;_tmp17D=_tmp17C->f1;_tmp17E=(struct Cyc_Absyn_Exp**)& _tmp17C->f2;
if(*_tmp17E != 0  && *_tmp17E != pat_var_exp)
*_tmp17E=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp17E));}};};}{
# 660
struct Cyc_Tcpat_TcPatResult _tmp17F=_tmp175;_npop_handler(0);return _tmp17F;};
# 644
;_pop_region(r);}
# 665
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 667
void*_stmttmp11=p->r;void*_tmp180=_stmttmp11;struct Cyc_Absyn_Pat*_tmp182;struct Cyc_Absyn_AggrInfo*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp18A;struct Cyc_Absyn_Vardecl*_tmp18C;struct Cyc_Absyn_Pat*_tmp18D;struct Cyc_Absyn_Pat*_tmp18F;_LLA2: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp181=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp181->tag != 5)goto _LLA4;else{_tmp182=_tmp181->f1;}}_LLA3:
# 669
 Cyc_Tcpat_check_pat_regions_rec(te,_tmp182,patvars);{
void*_stmttmp14=(void*)_check_null(p->topt);void*_tmp190=_stmttmp14;void*_tmp192;_LLB1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp190;if(_tmp191->tag != 5)goto _LLB3;else{_tmp192=((_tmp191->f1).ptr_atts).rgn;}}_LLB2:
# 672
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp192);
return;_LLB3:;_LLB4: {
const char*_tmp481;void*_tmp480;(_tmp480=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp481="check_pat_regions: bad pointer type",_tag_dyneither(_tmp481,sizeof(char),36))),_tag_dyneither(_tmp480,sizeof(void*),0)));}_LLB0:;};_LLA4: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp183=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp183->tag != 6)goto _LLA6;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;_tmp186=_tmp183->f3;}}_LLA5:
# 677
 for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp186->hd)).f2,patvars);}
return;_LLA6: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp187=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp187->tag != 7)goto _LLA8;else{_tmp188=_tmp187->f3;}}_LLA7:
# 681
 _tmp18A=_tmp188;goto _LLA9;_LLA8: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp189=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp189->tag != 4)goto _LLAA;else{_tmp18A=_tmp189->f1;}}_LLA9:
# 683
 for(0;_tmp18A != 0;_tmp18A=_tmp18A->tl){Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp18A->hd,patvars);}
return;_LLAA: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp18B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp18B->tag != 2)goto _LLAC;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;}}_LLAB:
# 686
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp12=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct _tuple16*_tmp195=_stmttmp12;_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;
# 692
if((_tmp196 != 0  && *_tmp196 == _tmp18C) && _tmp197 != 0){
{void*_stmttmp13=_tmp197->r;void*_tmp198=_stmttmp13;struct Cyc_Absyn_Exp*_tmp19A;_LLB6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp199=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp198;if(_tmp199->tag != 14)goto _LLB8;else{_tmp19A=_tmp199->f1;}}_LLB7:
# 695
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp19A->topt))){
# 697
const char*_tmp484;void*_tmp483;(_tmp483=0,Cyc_Tcutil_terr(p->loc,((_tmp484="Reference pattern not allowed on unique pointers",_tag_dyneither(_tmp484,sizeof(char),49))),_tag_dyneither(_tmp483,sizeof(void*),0)));}
goto _LLB5;_LLB8:;_LLB9: {
# 700
const char*_tmp487;void*_tmp486;(_tmp486=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp487="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp487,sizeof(char),51))),_tag_dyneither(_tmp486,sizeof(void*),0)));}_LLB5:;}
# 702
break;}}}
# 705
_tmp18F=_tmp18D;goto _LLAD;_LLAC: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp18E=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp180;if(_tmp18E->tag != 1)goto _LLAE;else{_tmp18F=_tmp18E->f2;}}_LLAD:
 Cyc_Tcpat_check_pat_regions_rec(te,_tmp18F,patvars);return;_LLAE:;_LLAF:
 return;_LLA1:;}
# 722 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 724
Cyc_Tcpat_check_pat_regions_rec(te,p,patvars);{
struct _RegionHandle _tmp19F=_new_region("r");struct _RegionHandle*r=& _tmp19F;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_stmttmp15=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;struct _tuple16*_tmp1A0=_stmttmp15;_tmp1A1=_tmp1A0->f1;_tmp1A2=_tmp1A0->f2;
if(_tmp1A2 != 0){
struct Cyc_Absyn_Exp*_tmp1A3=_tmp1A2;
# 732
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1A3->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1A3)){
# 735
const char*_tmp494;void*_tmp493[1];const char*_tmp492;struct Cyc_String_pa_PrintArg_struct _tmp491;void*_tmp490[1];const char*_tmp48F;struct Cyc_String_pa_PrintArg_struct _tmp48E;(_tmp48E.tag=0,((_tmp48E.f1=(struct _dyneither_ptr)(
# 737
_tmp1A1 != 0?(struct _dyneither_ptr)(
(_tmp491.tag=0,((_tmp491.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1A1)->name)),((_tmp490[0]=& _tmp491,Cyc_aprintf(((_tmp48F="for variable %s",_tag_dyneither(_tmp48F,sizeof(char),16))),_tag_dyneither(_tmp490,sizeof(void*),1)))))))):(
(_tmp492="",_tag_dyneither(_tmp492,sizeof(char),1)))),((_tmp493[0]=& _tmp48E,Cyc_Tcutil_terr(p->loc,((_tmp494="Pattern %s dereferences a unique pointer from a non-unique path",_tag_dyneither(_tmp494,sizeof(char),64))),_tag_dyneither(_tmp493,sizeof(void*),1)))))));}}}}
# 726
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 783 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp495;return((_tmp495.Name_v).val=s,(((_tmp495.Name_v).tag=1,_tmp495)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp496;return((_tmp496.Int_v).val=i,(((_tmp496.Int_v).tag=2,_tmp496)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 793
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 798
typedef void*Cyc_Tcpat_simple_pat_t;
# 801
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _stmttmp16=c1->name;union Cyc_Tcpat_Name_value _tmp1AD=_stmttmp16;struct _dyneither_ptr _tmp1AE;int _tmp1AF;_LLBB: if((_tmp1AD.Name_v).tag != 1)goto _LLBD;_tmp1AE=(struct _dyneither_ptr)(_tmp1AD.Name_v).val;_LLBC: {
# 804
union Cyc_Tcpat_Name_value _stmttmp18=c2->name;union Cyc_Tcpat_Name_value _tmp1B0=_stmttmp18;struct _dyneither_ptr _tmp1B1;_LLC0: if((_tmp1B0.Name_v).tag != 1)goto _LLC2;_tmp1B1=(struct _dyneither_ptr)(_tmp1B0.Name_v).val;_LLC1:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1AE,(struct _dyneither_ptr)_tmp1B1);_LLC2: if((_tmp1B0.Int_v).tag != 2)goto _LLBF;_LLC3:
 return - 1;_LLBF:;}_LLBD: if((_tmp1AD.Int_v).tag != 2)goto _LLBA;_tmp1AF=(int)(_tmp1AD.Int_v).val;_LLBE: {
# 809
union Cyc_Tcpat_Name_value _stmttmp17=c2->name;union Cyc_Tcpat_Name_value _tmp1B2=_stmttmp17;int _tmp1B3;_LLC5: if((_tmp1B2.Name_v).tag != 1)goto _LLC7;_LLC6:
 return 1;_LLC7: if((_tmp1B2.Int_v).tag != 2)goto _LLC4;_tmp1B3=(int)(_tmp1B2.Int_v).val;_LLC8:
 return _tmp1AF - _tmp1B3;_LLC4:;}_LLBA:;}
# 817
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 821
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 825
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp499;struct Cyc_Tcpat_Con_s*_tmp498;return(_tmp498=_region_malloc(r,sizeof(*_tmp498)),((_tmp498->name=Cyc_Tcpat_Name_v(((_tmp499="NULL",_tag_dyneither(_tmp499,sizeof(char),5)))),((_tmp498->arity=0,((_tmp498->span=& Cyc_Tcpat_two_opt,((_tmp498->orig_pat=p,_tmp498)))))))));}
# 828
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp49C;struct Cyc_Tcpat_Con_s*_tmp49B;return(_tmp49B=_region_malloc(r,sizeof(*_tmp49B)),((_tmp49B->name=Cyc_Tcpat_Name_v(((_tmp49C="&",_tag_dyneither(_tmp49C,sizeof(char),2)))),((_tmp49B->arity=1,((_tmp49B->span=& Cyc_Tcpat_two_opt,((_tmp49B->orig_pat=p,_tmp49B)))))))));}
# 831
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp49F;struct Cyc_Tcpat_Con_s*_tmp49E;return(_tmp49E=_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E->name=Cyc_Tcpat_Name_v(((_tmp49F="&",_tag_dyneither(_tmp49F,sizeof(char),2)))),((_tmp49E->arity=1,((_tmp49E->span=& Cyc_Tcpat_one_opt,((_tmp49E->orig_pat=p,_tmp49E)))))))));}
# 834
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4A0;return(_tmp4A0=_region_malloc(r,sizeof(*_tmp4A0)),((_tmp4A0->name=Cyc_Tcpat_Int_v(i),((_tmp4A0->arity=0,((_tmp4A0->span=0,((_tmp4A0->orig_pat=p,_tmp4A0)))))))));}
# 837
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4A1;return(_tmp4A1=_region_malloc(r,sizeof(*_tmp4A1)),((_tmp4A1->name=Cyc_Tcpat_Name_v(f),((_tmp4A1->arity=0,((_tmp4A1->span=0,((_tmp4A1->orig_pat=p,_tmp4A1)))))))));}
# 840
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp4A2;return(_tmp4A2=_region_malloc(r,sizeof(*_tmp4A2)),((_tmp4A2->name=Cyc_Tcpat_Int_v((int)c),((_tmp4A2->arity=0,((_tmp4A2->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp4A2->orig_pat=p,_tmp4A2)))))))));}
# 843
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp4A5;struct Cyc_Tcpat_Con_s*_tmp4A4;return(_tmp4A4=_region_malloc(r,sizeof(*_tmp4A4)),((_tmp4A4->name=Cyc_Tcpat_Name_v(((_tmp4A5="$",_tag_dyneither(_tmp4A5,sizeof(char),2)))),((_tmp4A4->arity=i,((_tmp4A4->span=& Cyc_Tcpat_one_opt,((_tmp4A4->orig_pat=p,_tmp4A4)))))))));}
# 848
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4A8;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4A7;return(void*)((_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7)),((_tmp4A7[0]=((_tmp4A8.tag=1,((_tmp4A8.f1=Cyc_Tcpat_null_con(r,p),((_tmp4A8.f2=0,_tmp4A8)))))),_tmp4A7))));}
# 851
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AB;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4AA;return(void*)((_tmp4AA=_region_malloc(r,sizeof(*_tmp4AA)),((_tmp4AA[0]=((_tmp4AB.tag=1,((_tmp4AB.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp4AB.f2=0,_tmp4AB)))))),_tmp4AA))));}
# 854
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4AE;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4AD;return(void*)((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD[0]=((_tmp4AE.tag=1,((_tmp4AE.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp4AE.f2=0,_tmp4AE)))))),_tmp4AD))));}
# 857
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4B1;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4B0;return(void*)((_tmp4B0=_region_malloc(r,sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=1,((_tmp4B1.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp4B1.f2=0,_tmp4B1)))))),_tmp4B0))));}
# 860
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4B7;struct Cyc_List_List*_tmp4B6;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4B5;return(void*)((_tmp4B5=_region_malloc(r,sizeof(*_tmp4B5)),((_tmp4B5[0]=((_tmp4B7.tag=1,((_tmp4B7.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp4B7.f2=((_tmp4B6=_region_malloc(r,sizeof(*_tmp4B6)),((_tmp4B6->hd=p,((_tmp4B6->tl=0,_tmp4B6)))))),_tmp4B7)))))),_tmp4B5))));}
# 863
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4BD;struct Cyc_List_List*_tmp4BC;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4BB;return(void*)((_tmp4BB=_region_malloc(r,sizeof(*_tmp4BB)),((_tmp4BB[0]=((_tmp4BD.tag=1,((_tmp4BD.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp4BD.f2=((_tmp4BC=_region_malloc(r,sizeof(*_tmp4BC)),((_tmp4BC->hd=p,((_tmp4BC->tl=0,_tmp4BC)))))),_tmp4BD)))))),_tmp4BB))));}
# 866
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4C0;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4BF;return(void*)((_tmp4BF=_region_malloc(r,sizeof(*_tmp4BF)),((_tmp4BF[0]=((_tmp4C0.tag=1,((_tmp4C0.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp4C0.f2=ss,_tmp4C0)))))),_tmp4BF))));}
# 869
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 871
struct Cyc_Tcpat_Con_s*_tmp4C1;struct Cyc_Tcpat_Con_s*c=(_tmp4C1=_region_malloc(r,sizeof(*_tmp4C1)),((_tmp4C1->name=Cyc_Tcpat_Name_v(con_name),((_tmp4C1->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp4C1->span=span,((_tmp4C1->orig_pat=p,_tmp4C1)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp4C4;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp4C3;return(void*)((_tmp4C3=_region_malloc(r,sizeof(*_tmp4C3)),((_tmp4C3[0]=((_tmp4C4.tag=1,((_tmp4C4.f1=c,((_tmp4C4.f2=ps,_tmp4C4)))))),_tmp4C3))));}
# 876
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_stmttmp19=p->r;void*_tmp1D2=_stmttmp19;enum Cyc_Absyn_Sign _tmp1D7;int _tmp1D8;char _tmp1DA;struct _dyneither_ptr _tmp1DC;struct Cyc_Absyn_Pat*_tmp1DE;struct Cyc_Absyn_Pat*_tmp1E0;struct Cyc_Absyn_Pat*_tmp1E2;struct Cyc_Absyn_Datatypedecl*_tmp1E4;struct Cyc_Absyn_Datatypefield*_tmp1E5;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E8;struct Cyc_Absyn_Aggrdecl*_tmp1EA;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Enumdecl*_tmp1ED;struct Cyc_Absyn_Enumfield*_tmp1EE;void*_tmp1F0;struct Cyc_Absyn_Enumfield*_tmp1F1;_LLCA: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1D3->tag != 0)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1D4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1D4->tag != 3)goto _LLCE;}_LLCD:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4C7;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4C6;s=(void*)((_tmp4C6=_region_malloc(r,sizeof(*_tmp4C6)),((_tmp4C6[0]=((_tmp4C7.tag=0,_tmp4C7)),_tmp4C6))));}goto _LLC9;_LLCE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1D5->tag != 8)goto _LLD0;}_LLCF:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC9;_LLD0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1D6->tag != 9)goto _LLD2;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LLD1:
 s=Cyc_Tcpat_int_pat(r,_tmp1D8,p);goto _LLC9;_LLD2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1D9->tag != 10)goto _LLD4;else{_tmp1DA=_tmp1D9->f1;}}_LLD3:
 s=Cyc_Tcpat_char_pat(r,_tmp1DA,p);goto _LLC9;_LLD4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1DB=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1DB->tag != 11)goto _LLD6;else{_tmp1DC=_tmp1DB->f1;}}_LLD5:
 s=Cyc_Tcpat_float_pat(r,_tmp1DC,p);goto _LLC9;_LLD6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1DD=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1DD->tag != 1)goto _LLD8;else{_tmp1DE=_tmp1DD->f2;}}_LLD7:
 s=Cyc_Tcpat_compile_pat(r,_tmp1DE);goto _LLC9;_LLD8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1DF=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1DF->tag != 2)goto _LLDA;else{_tmp1E0=_tmp1DF->f2;}}_LLD9:
 s=Cyc_Tcpat_compile_pat(r,_tmp1E0);goto _LLC9;_LLDA: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1E1=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1E1->tag != 5)goto _LLDC;else{_tmp1E2=_tmp1E1->f1;}}_LLDB:
# 888
{void*_stmttmp1F=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F8=_stmttmp1F;union Cyc_Absyn_Constraint*_tmp1FA;_LLEF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F8;if(_tmp1F9->tag != 5)goto _LLF1;else{_tmp1FA=((_tmp1F9->f1).ptr_atts).nullable;}}_LLF0: {
# 890
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1FB=_tmp1FA;union Cyc_Absyn_Constraint*_tmp1FC;int _tmp1FD;_LLF4: if((_tmp1FB->Forward_constr).tag != 2)goto _LLF6;_tmp1FC=(union Cyc_Absyn_Constraint*)(_tmp1FB->Forward_constr).val;_LLF5:
# 895
*_tmp1FA=*_tmp1FC;
continue;_LLF6: if((_tmp1FB->No_constr).tag != 3)goto _LLF8;_LLF7:
# 898
{struct _union_Constraint_Eq_constr*_tmp4C8;(_tmp4C8=& _tmp1FA->Eq_constr,((_tmp4C8->tag=1,_tmp4C8->val=0)));}
is_nullable=0;
still_working=0;
goto _LLF3;_LLF8: if((_tmp1FB->Eq_constr).tag != 1)goto _LLF3;_tmp1FD=(int)(_tmp1FB->Eq_constr).val;_LLF9:
# 903
 is_nullable=_tmp1FD;
still_working=0;
goto _LLF3;_LLF3:;}{
# 908
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1E2);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 912
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLEE;};}_LLF1:;_LLF2: {
const char*_tmp4CB;void*_tmp4CA;(_tmp4CA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CB="expecting pointertype for pattern!",_tag_dyneither(_tmp4CB,sizeof(char),35))),_tag_dyneither(_tmp4CA,sizeof(void*),0)));}_LLEE:;}
# 916
goto _LLC9;_LLDC: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1E3=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1E3->tag != 7)goto _LLDE;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;_tmp1E6=_tmp1E3->f3;}}_LLDD: {
# 918
int*span;
{void*_stmttmp1E=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp201=_stmttmp1E;_LLFB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp201;if(_tmp202->tag != 3)goto _LLFD;}_LLFC:
# 921
 if(_tmp1E4->is_extensible)
span=0;else{
# 924
int*_tmp4CC;span=((_tmp4CC=_region_malloc(r,sizeof(*_tmp4CC)),((_tmp4CC[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E4->fields))->v),_tmp4CC))));}
goto _LLFA;_LLFD: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp201;if(_tmp203->tag != 4)goto _LLFF;}_LLFE:
 span=& Cyc_Tcpat_one_opt;goto _LLFA;_LLFF:;_LL100:
{const char*_tmp4CF;void*_tmp4CE;span=((_tmp4CE=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CF="void datatype pattern has bad type",_tag_dyneither(_tmp4CF,sizeof(char),35))),_tag_dyneither(_tmp4CE,sizeof(void*),0))));}goto _LLFA;_LLFA:;}
# 929
s=Cyc_Tcpat_con_pat(r,*(*_tmp1E5->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1E6),p);
goto _LLC9;}_LLDE: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1E7=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1E7->tag != 4)goto _LLE0;else{_tmp1E8=_tmp1E7->f1;}}_LLDF:
# 933
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1E8),p);goto _LLC9;_LLE0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1E9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1E9->tag != 6)goto _LLE2;else{if(_tmp1E9->f1 == 0)goto _LLE2;if((((_tmp1E9->f1)->aggr_info).KnownAggr).tag != 2)goto _LLE2;_tmp1EA=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1E9->f1)->aggr_info).KnownAggr).val);_tmp1EB=_tmp1E9->f3;}}_LLE1:
# 938
 if(_tmp1EA->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EA->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 942
const char*_tmp4D0;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp4D0="",_tag_dyneither(_tmp4D0,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1EB;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_stmttmp1D=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp208;struct Cyc_Absyn_Pat*_tmp209;struct _tuple15*_tmp207=_stmttmp1D;_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;{
struct Cyc_List_List*_tmp20A=_tmp208;struct _dyneither_ptr*_tmp20C;_LL102: if(_tmp20A == 0)goto _LL104;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp20B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp20A->hd);if(_tmp20B->tag != 1)goto _LL104;else{_tmp20C=_tmp20B->f1;}}if(_tmp20A->tl != 0)goto _LL104;_LL103:
# 947
 if(Cyc_strptrcmp(_tmp20C,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp4D1;ps=((_tmp4D1=_region_malloc(r,sizeof(*_tmp4D1)),((_tmp4D1->hd=Cyc_Tcpat_compile_pat(r,_tmp209),((_tmp4D1->tl=ps,_tmp4D1))))));}
found=1;}
# 951
goto _LL101;_LL104:;_LL105: {
const char*_tmp4D4;void*_tmp4D3;(_tmp4D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D4="bad designator(s)",_tag_dyneither(_tmp4D4,sizeof(char),18))),_tag_dyneither(_tmp4D3,sizeof(void*),0)));}_LL101:;};}}
# 955
if(!found){
const char*_tmp4D7;void*_tmp4D6;(_tmp4D6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D7="bad designator",_tag_dyneither(_tmp4D7,sizeof(char),15))),_tag_dyneither(_tmp4D6,sizeof(void*),0)));}}}
# 958
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 961
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EA->impl))->tagged){
const char*_tmp4DA;void*_tmp4D9;(_tmp4D9=0,Cyc_Tcutil_terr(p->loc,((_tmp4DA="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp4DA,sizeof(char),47))),_tag_dyneither(_tmp4D9,sizeof(void*),0)));}{
int*_tmp4DB;int*span=(_tmp4DB=_region_malloc(r,sizeof(*_tmp4DB)),((_tmp4DB[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1EA->impl))->fields),_tmp4DB)));
struct Cyc_List_List*_tmp215=_tmp1EB;struct _dyneither_ptr*_tmp217;struct Cyc_Absyn_Pat*_tmp218;_LL107: if(_tmp215 == 0)goto _LL109;if(((struct _tuple15*)_tmp215->hd)->f1 == 0)goto _LL109;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp216=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp215->hd)->f1)->hd);if(_tmp216->tag != 1)goto _LL109;else{_tmp217=_tmp216->f1;}}if((((struct _tuple15*)_tmp215->hd)->f1)->tl != 0)goto _LL109;_tmp218=((struct _tuple15*)_tmp215->hd)->f2;if(_tmp215->tl != 0)goto _LL109;_LL108:
# 966
{struct Cyc_List_List*_tmp4DC;s=Cyc_Tcpat_con_pat(r,*_tmp217,span,((_tmp4DC=_region_malloc(r,sizeof(*_tmp4DC)),((_tmp4DC->hd=Cyc_Tcpat_compile_pat(r,_tmp218),((_tmp4DC->tl=0,_tmp4DC)))))),p);}
goto _LL106;_LL109:;_LL10A: {
const char*_tmp4DF;void*_tmp4DE;(_tmp4DE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DF="bad union pattern",_tag_dyneither(_tmp4DF,sizeof(char),18))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}_LL106:;};}
# 971
goto _LLC9;_LLE2: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1EC=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1EC->tag != 12)goto _LLE4;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;}}_LLE3:
# 976
{void*_stmttmp1C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp21D=_stmttmp1C;_LL10C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 6)goto _LL10E;}_LL10D:
# 980
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1EE->name).f2,0,0,p);
goto _LL10B;_LL10E:;_LL10F: {
# 983
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1ED->fields))->v);
{int*_tmp4E0;s=Cyc_Tcpat_con_pat(r,*(*_tmp1EE->name).f2,((_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0)),((_tmp4E0[0]=span,_tmp4E0)))),0,p);}
goto _LL10B;}_LL10B:;}
# 987
goto _LLC9;_LLE4: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1EF=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1EF->tag != 13)goto _LLE6;else{_tmp1F0=(void*)_tmp1EF->f1;_tmp1F1=_tmp1EF->f2;}}_LLE5: {
# 992
struct Cyc_List_List*fields;
{void*_stmttmp1A=Cyc_Tcutil_compress(_tmp1F0);void*_tmp220=_stmttmp1A;struct Cyc_List_List*_tmp222;_LL111: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp220;if(_tmp221->tag != 14)goto _LL113;else{_tmp222=_tmp221->f1;}}_LL112:
 fields=_tmp222;goto _LL110;_LL113:;_LL114: {
const char*_tmp4E3;void*_tmp4E2;(_tmp4E2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E3="bad type in AnonEnum_p",_tag_dyneither(_tmp4E3,sizeof(char),23))),_tag_dyneither(_tmp4E2,sizeof(void*),0)));}_LL110:;}
# 999
{void*_stmttmp1B=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp225=_stmttmp1B;_LL116: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp225;if(_tmp226->tag != 6)goto _LL118;}_LL117:
# 1003
 s=Cyc_Tcpat_con_pat(r,*(*_tmp1F1->name).f2,0,0,p);
goto _LL115;_LL118:;_LL119: {
# 1006
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4E4;s=Cyc_Tcpat_con_pat(r,*(*_tmp1F1->name).f2,((_tmp4E4=_region_malloc(r,sizeof(*_tmp4E4)),((_tmp4E4[0]=span,_tmp4E4)))),0,p);}
goto _LL115;}_LL115:;}
# 1010
goto _LLC9;}_LLE6: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1F2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1F2->tag != 14)goto _LLE8;}_LLE7:
 goto _LLE9;_LLE8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1F3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1F3->tag != 15)goto _LLEA;}_LLE9:
 goto _LLEB;_LLEA: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1F4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1F4->tag != 6)goto _LLEC;}_LLEB:
 goto _LLED;_LLEC: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1F5=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D2;if(_tmp1F5->tag != 16)goto _LLC9;}_LLED: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4E7;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4E6;s=(void*)((_tmp4E6=_region_malloc(r,sizeof(*_tmp4E6)),((_tmp4E6[0]=((_tmp4E7.tag=0,_tmp4E7)),_tmp4E6))));}_LLC9:;}
# 1016
return s;}
# 1024
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1035
typedef void*Cyc_Tcpat_term_desc_t;
# 1039
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1046
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1055
typedef void*Cyc_Tcpat_decision_t;struct _tuple19{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1060
typedef struct _tuple19*Cyc_Tcpat_work_stack_frame_t;
# 1063
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1065
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1068
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4EA;void*_tmp4E9;(_tmp4E9=0,Cyc_fprintf(Cyc_stderr,((_tmp4EA=" ",_tag_dyneither(_tmp4EA,sizeof(char),2))),_tag_dyneither(_tmp4E9,sizeof(void*),0)));}}
# 1074
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp22C=c->name;
union Cyc_Tcpat_Name_value _tmp22D=_tmp22C;struct _dyneither_ptr _tmp22E;int _tmp22F;_LL11B: if((_tmp22D.Name_v).tag != 1)goto _LL11D;_tmp22E=(struct _dyneither_ptr)(_tmp22D.Name_v).val;_LL11C:
{const char*_tmp4EE;void*_tmp4ED[1];struct Cyc_String_pa_PrintArg_struct _tmp4EC;(_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22E),((_tmp4ED[0]=& _tmp4EC,Cyc_fprintf(Cyc_stderr,((_tmp4EE="%s",_tag_dyneither(_tmp4EE,sizeof(char),3))),_tag_dyneither(_tmp4ED,sizeof(void*),1)))))));}goto _LL11A;_LL11D: if((_tmp22D.Int_v).tag != 2)goto _LL11A;_tmp22F=(int)(_tmp22D.Int_v).val;_LL11E:
{const char*_tmp4F2;void*_tmp4F1[1];struct Cyc_Int_pa_PrintArg_struct _tmp4F0;(_tmp4F0.tag=1,((_tmp4F0.f1=(unsigned long)_tmp22F,((_tmp4F1[0]=& _tmp4F0,Cyc_fprintf(Cyc_stderr,((_tmp4F2="%d",_tag_dyneither(_tmp4F2,sizeof(char),3))),_tag_dyneither(_tmp4F1,sizeof(void*),1)))))));}goto _LL11A;_LL11A:;}
# 1082
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp236=d;void*_tmp238;struct Cyc_List_List*_tmp23B;struct Cyc_Tcpat_Con_s*_tmp23C;void*_tmp23D;void*_tmp23E;_LL120: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp237=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp236;if(_tmp237->tag != 1)goto _LL122;else{_tmp238=(void*)_tmp237->f1;}}_LL121:
# 1085
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((_tmp4F5="Success(",_tag_dyneither(_tmp4F5,sizeof(char),9))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}print_rhs(_tmp238);{const char*_tmp4F8;void*_tmp4F7;(_tmp4F7=0,Cyc_fprintf(Cyc_stderr,((_tmp4F8=")\n",_tag_dyneither(_tmp4F8,sizeof(char),3))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}
goto _LL11F;_LL122: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp239=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp236;if(_tmp239->tag != 0)goto _LL124;}_LL123:
{const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,Cyc_fprintf(Cyc_stderr,((_tmp4FB="Failure\n",_tag_dyneither(_tmp4FB,sizeof(char),9))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}goto _LL11F;_LL124: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp23A=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp236;if(_tmp23A->tag != 2)goto _LL11F;else{_tmp23B=_tmp23A->f1;_tmp23C=_tmp23A->f2;_tmp23D=(void*)_tmp23A->f3;_tmp23E=(void*)_tmp23A->f4;}}_LL125:
# 1090
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4FE;void*_tmp4FD;(_tmp4FD=0,Cyc_fprintf(Cyc_stderr,((_tmp4FE="Access[",_tag_dyneither(_tmp4FE,sizeof(char),8))),_tag_dyneither(_tmp4FD,sizeof(void*),0)));}
for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
{const char*_tmp502;void*_tmp501[1];struct Cyc_Int_pa_PrintArg_struct _tmp500;(_tmp500.tag=1,((_tmp500.f1=(unsigned long)((int)_tmp23B->hd),((_tmp501[0]=& _tmp500,Cyc_fprintf(Cyc_stderr,((_tmp502="%d",_tag_dyneither(_tmp502,sizeof(char),3))),_tag_dyneither(_tmp501,sizeof(void*),1)))))));}
if(_tmp23B->tl != 0){const char*_tmp505;void*_tmp504;(_tmp504=0,Cyc_fprintf(Cyc_stderr,((_tmp505=",",_tag_dyneither(_tmp505,sizeof(char),2))),_tag_dyneither(_tmp504,sizeof(void*),0)));}}
# 1096
{const char*_tmp508;void*_tmp507;(_tmp507=0,Cyc_fprintf(Cyc_stderr,((_tmp508="],",_tag_dyneither(_tmp508,sizeof(char),3))),_tag_dyneither(_tmp507,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp23C);
{const char*_tmp50B;void*_tmp50A;(_tmp50A=0,Cyc_fprintf(Cyc_stderr,((_tmp50B=",\n",_tag_dyneither(_tmp50B,sizeof(char),3))),_tag_dyneither(_tmp50A,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp23D,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp23E,print_rhs,tab + 7);_LL11F:;}struct _tuple20{int f1;unsigned int f2;};
# 1104
static void Cyc_Tcpat_print_swrhs(struct _tuple20*x){
return;}
# 1112
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp250=td;struct Cyc_Set_Set*_tmp252;_LL127: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp251=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp250;if(_tmp251->tag != 1)goto _LL129;else{_tmp252=_tmp251->f1;}}_LL128: {
# 1122
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp50E;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp50D;return(void*)((_tmp50D=_region_malloc(r,sizeof(*_tmp50D)),((_tmp50D[0]=((_tmp50E.tag=1,((_tmp50E.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp252,c),_tmp50E)))),_tmp50D))));}_LL129: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp253=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp250;if(_tmp253->tag != 0)goto _LL126;}_LL12A: {
const char*_tmp511;void*_tmp510;(_tmp510=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp511="add_neg called when td is Positive",_tag_dyneither(_tmp511,sizeof(char),35))),_tag_dyneither(_tmp510,sizeof(void*),0)));}_LL126:;}
# 1129
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp258=td;struct Cyc_Tcpat_Con_s*_tmp25A;struct Cyc_Set_Set*_tmp25C;_LL12C: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp259=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp258;if(_tmp259->tag != 0)goto _LL12E;else{_tmp25A=_tmp259->f1;}}_LL12D:
# 1133
 if(Cyc_Tcpat_compare_con(c,_tmp25A)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL12E: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp25B=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp258;if(_tmp25B->tag != 1)goto _LL12B;else{_tmp25C=_tmp25B->f1;}}_LL12F:
# 1137
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp25C,c))return Cyc_Tcpat_No;else{
# 1140
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp25C)+ 1)
return Cyc_Tcpat_Yes;else{
# 1143
return Cyc_Tcpat_Maybe;}}_LL12B:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1151
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1153
struct Cyc_List_List*_tmp25D=ctxt;struct Cyc_Tcpat_Con_s*_tmp25E;struct Cyc_List_List*_tmp25F;struct Cyc_List_List*_tmp260;_LL131: if(_tmp25D != 0)goto _LL133;_LL132:
 return 0;_LL133: if(_tmp25D == 0)goto _LL130;_tmp25E=((struct _tuple21*)_tmp25D->hd)->f1;_tmp25F=((struct _tuple21*)_tmp25D->hd)->f2;_tmp260=_tmp25D->tl;_LL134: {
# 1156
struct _tuple21*_tmp517;struct Cyc_List_List*_tmp516;struct Cyc_List_List*_tmp515;return(_tmp515=_region_malloc(r,sizeof(*_tmp515)),((_tmp515->hd=(
(_tmp517=_region_malloc(r,sizeof(*_tmp517)),((_tmp517->f1=_tmp25E,((_tmp517->f2=((_tmp516=_region_malloc(r,sizeof(*_tmp516)),((_tmp516->hd=dsc,((_tmp516->tl=_tmp25F,_tmp516)))))),_tmp517)))))),((_tmp515->tl=_tmp260,_tmp515)))));}_LL130:;}
# 1164
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp264=ctxt;struct Cyc_Tcpat_Con_s*_tmp265;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp267;_LL136: if(_tmp264 != 0)goto _LL138;_LL137: {
const char*_tmp51A;void*_tmp519;(_tmp519=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51A="norm_context: empty context",_tag_dyneither(_tmp51A,sizeof(char),28))),_tag_dyneither(_tmp519,sizeof(void*),0)));}_LL138: if(_tmp264 == 0)goto _LL135;_tmp265=((struct _tuple21*)_tmp264->hd)->f1;_tmp266=((struct _tuple21*)_tmp264->hd)->f2;_tmp267=_tmp264->tl;_LL139: {
# 1168
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp51D;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp51C;return Cyc_Tcpat_augment(r,_tmp267,(void*)((_tmp51C=_region_malloc(r,sizeof(*_tmp51C)),((_tmp51C[0]=((_tmp51D.tag=0,((_tmp51D.f1=_tmp265,((_tmp51D.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp266),_tmp51D)))))),_tmp51C)))));}_LL135:;}
# 1177
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1179
struct _tuple1 _tmp51E;struct _tuple1 _stmttmp20=(_tmp51E.f1=ctxt,((_tmp51E.f2=work,_tmp51E)));struct _tuple1 _tmp26C=_stmttmp20;struct Cyc_Tcpat_Con_s*_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp271;_LL13B: if(_tmp26C.f1 != 0)goto _LL13D;if(_tmp26C.f2 != 0)goto _LL13D;_LL13C:
 return dsc;_LL13D: if(_tmp26C.f1 != 0)goto _LL13F;_LL13E:
 goto _LL140;_LL13F: if(_tmp26C.f2 != 0)goto _LL141;_LL140: {
const char*_tmp521;void*_tmp520;(_tmp520=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp521="build_desc: ctxt and work don't match",_tag_dyneither(_tmp521,sizeof(char),38))),_tag_dyneither(_tmp520,sizeof(void*),0)));}_LL141: if(_tmp26C.f1 == 0)goto _LL13A;_tmp26D=((struct _tuple21*)(_tmp26C.f1)->hd)->f1;_tmp26E=((struct _tuple21*)(_tmp26C.f1)->hd)->f2;_tmp26F=(_tmp26C.f1)->tl;if(_tmp26C.f2 == 0)goto _LL13A;_tmp270=((struct _tuple19*)(_tmp26C.f2)->hd)->f3;_tmp271=(_tmp26C.f2)->tl;_LL142: {
# 1184
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp527;struct Cyc_List_List*_tmp526;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp525;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp274=(_tmp525=_region_malloc(r,sizeof(*_tmp525)),((_tmp525[0]=((_tmp527.tag=0,((_tmp527.f1=_tmp26D,((_tmp527.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp26E),(
(_tmp526=_region_malloc(r,sizeof(*_tmp526)),((_tmp526->hd=dsc,((_tmp526->tl=_tmp270,_tmp526))))))),_tmp527)))))),_tmp525)));
return Cyc_Tcpat_build_desc(r,_tmp26F,(void*)_tmp274,_tmp271);}_LL13A:;}
# 1190
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1197
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1199
struct Cyc_List_List*_tmp279=allmrules;void*_tmp27A;void*_tmp27B;struct Cyc_List_List*_tmp27C;_LL144: if(_tmp279 != 0)goto _LL146;_LL145: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp52A;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp529;return(void*)((_tmp529=_cycalloc(sizeof(*_tmp529)),((_tmp529[0]=((_tmp52A.tag=0,((_tmp52A.f1=dsc,_tmp52A)))),_tmp529))));}_LL146: if(_tmp279 == 0)goto _LL143;_tmp27A=((struct _tuple0*)_tmp279->hd)->f1;_tmp27B=((struct _tuple0*)_tmp279->hd)->f2;_tmp27C=_tmp279->tl;_LL147:
# 1202
 return Cyc_Tcpat_match(r,_tmp27A,0,dsc,0,0,_tmp27B,_tmp27C);_LL143:;}
# 1207
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1209
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp52D;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp52C;return Cyc_Tcpat_or_match(r,(void*)((_tmp52C=_region_malloc(r,sizeof(*_tmp52C)),((_tmp52C[0]=((_tmp52D.tag=1,((_tmp52D.f1=Cyc_Tcpat_empty_con_set(r),_tmp52D)))),_tmp52C)))),allmrules);}
# 1215
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1219
struct Cyc_List_List*_tmp281=work;struct Cyc_List_List*_tmp282;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_List_List*_tmp286;_LL149: if(_tmp281 != 0)goto _LL14B;_LL14A: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp530;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp52F;return(void*)((_tmp52F=_region_malloc(r,sizeof(*_tmp52F)),((_tmp52F[0]=((_tmp530.tag=1,((_tmp530.f1=right_hand_side,_tmp530)))),_tmp52F))));}_LL14B: if(_tmp281 == 0)goto _LL14D;if(((struct _tuple19*)_tmp281->hd)->f1 != 0)goto _LL14D;if(((struct _tuple19*)_tmp281->hd)->f2 != 0)goto _LL14D;if(((struct _tuple19*)_tmp281->hd)->f3 != 0)goto _LL14D;_tmp282=_tmp281->tl;_LL14C:
# 1222
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp282,right_hand_side,rules);_LL14D: if(_tmp281 == 0)goto _LL148;_tmp283=((struct _tuple19*)_tmp281->hd)->f1;_tmp284=((struct _tuple19*)_tmp281->hd)->f2;_tmp285=((struct _tuple19*)_tmp281->hd)->f3;_tmp286=_tmp281->tl;_LL14E:
# 1224
 if((_tmp283 == 0  || _tmp284 == 0) || _tmp285 == 0){
const char*_tmp533;void*_tmp532;(_tmp532=0,Cyc_Tcutil_impos(((_tmp533="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp533,sizeof(char),38))),_tag_dyneither(_tmp532,sizeof(void*),0)));}{
struct Cyc_List_List*_stmttmp21=_tmp283;void*_tmp28C;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28B=_stmttmp21;_tmp28C=(void*)_tmp28B->hd;_tmp28D=_tmp28B->tl;{
struct Cyc_List_List*_stmttmp22=_tmp284;struct Cyc_List_List*_tmp28F;struct Cyc_List_List*_tmp290;struct Cyc_List_List*_tmp28E=_stmttmp22;_tmp28F=(struct Cyc_List_List*)_tmp28E->hd;_tmp290=_tmp28E->tl;{
struct Cyc_List_List*_stmttmp23=_tmp285;void*_tmp292;struct Cyc_List_List*_tmp293;struct Cyc_List_List*_tmp291=_stmttmp23;_tmp292=(void*)_tmp291->hd;_tmp293=_tmp291->tl;{
struct _tuple19*_tmp534;struct _tuple19*_tmp294=(_tmp534=_region_malloc(r,sizeof(*_tmp534)),((_tmp534->f1=_tmp28D,((_tmp534->f2=_tmp290,((_tmp534->f3=_tmp293,_tmp534)))))));
struct Cyc_List_List*_tmp535;return Cyc_Tcpat_match(r,_tmp28C,_tmp28F,_tmp292,ctx,((_tmp535=_region_malloc(r,sizeof(*_tmp535)),((_tmp535->hd=_tmp294,((_tmp535->tl=_tmp286,_tmp535)))))),right_hand_side,rules);};};};};_LL148:;}
# 1236
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1238
void*_tmp297=dsc;struct Cyc_Set_Set*_tmp299;struct Cyc_List_List*_tmp29B;_LL150: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp298=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp297;if(_tmp298->tag != 1)goto _LL152;else{_tmp299=_tmp298->f1;}}_LL151: {
# 1243
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp538;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp537;void*any=(void*)((_tmp537=_region_malloc(r,sizeof(*_tmp537)),((_tmp537[0]=((_tmp538.tag=1,((_tmp538.f1=Cyc_Tcpat_empty_con_set(r),_tmp538)))),_tmp537))));
struct Cyc_List_List*_tmp29C=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp539;_tmp29C=((_tmp539=_region_malloc(r,sizeof(*_tmp539)),((_tmp539->hd=any,((_tmp539->tl=_tmp29C,_tmp539))))));}}
return _tmp29C;}_LL152: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp29A=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp297;if(_tmp29A->tag != 0)goto _LL14F;else{_tmp29B=_tmp29A->f2;}}_LL153:
 return _tmp29B;_LL14F:;}struct _tuple22{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1253
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple22*env,int i){
struct _RegionHandle*_tmp2A1;struct Cyc_List_List*_tmp2A2;struct _tuple22*_tmp2A0=env;_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;{
struct Cyc_List_List*_tmp53A;return(_tmp53A=_region_malloc(_tmp2A1,sizeof(*_tmp53A)),((_tmp53A->hd=(void*)(i + 1),((_tmp53A->tl=_tmp2A2,_tmp53A)))));};}
# 1257
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1259
struct _tuple22 _tmp53B;struct _tuple22 _tmp2A4=(_tmp53B.f1=r,((_tmp53B.f2=obj,_tmp53B)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple22*,int),struct _tuple22*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp2A4);}
# 1266
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1270
void*_tmp2A6=p;struct Cyc_Tcpat_Con_s*_tmp2A9;struct Cyc_List_List*_tmp2AA;_LL155: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2A7=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2A6;if(_tmp2A7->tag != 0)goto _LL157;}_LL156:
# 1272
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL157: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2A8=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2A6;if(_tmp2A8->tag != 1)goto _LL154;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f2;}}_LL158: {
# 1274
enum Cyc_Tcpat_Answer _stmttmp24=Cyc_Tcpat_static_match(_tmp2A9,dsc);switch(_stmttmp24){case Cyc_Tcpat_Yes: _LL159: {
# 1276
struct _tuple21*_tmp53E;struct Cyc_List_List*_tmp53D;struct Cyc_List_List*ctx2=(_tmp53D=_region_malloc(r,sizeof(*_tmp53D)),((_tmp53D->hd=((_tmp53E=_region_malloc(r,sizeof(*_tmp53E)),((_tmp53E->f1=_tmp2A9,((_tmp53E->f2=0,_tmp53E)))))),((_tmp53D->tl=ctx,_tmp53D)))));
struct _tuple19*_tmp53F;struct _tuple19*work_frame=(_tmp53F=_region_malloc(r,sizeof(*_tmp53F)),((_tmp53F->f1=_tmp2AA,((_tmp53F->f2=Cyc_Tcpat_getoargs(r,_tmp2A9,obj),((_tmp53F->f3=
Cyc_Tcpat_getdargs(r,_tmp2A9,dsc),_tmp53F)))))));
struct Cyc_List_List*_tmp540;struct Cyc_List_List*work2=(_tmp540=_region_malloc(r,sizeof(*_tmp540)),((_tmp540->hd=work_frame,((_tmp540->tl=work,_tmp540)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL15A:
# 1282
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL15B: {
# 1284
struct _tuple21*_tmp543;struct Cyc_List_List*_tmp542;struct Cyc_List_List*ctx2=(_tmp542=_region_malloc(r,sizeof(*_tmp542)),((_tmp542->hd=((_tmp543=_region_malloc(r,sizeof(*_tmp543)),((_tmp543->f1=_tmp2A9,((_tmp543->f2=0,_tmp543)))))),((_tmp542->tl=ctx,_tmp542)))));
struct _tuple19*_tmp544;struct _tuple19*work_frame=(_tmp544=_region_malloc(r,sizeof(*_tmp544)),((_tmp544->f1=_tmp2AA,((_tmp544->f2=Cyc_Tcpat_getoargs(r,_tmp2A9,obj),((_tmp544->f3=
Cyc_Tcpat_getdargs(r,_tmp2A9,dsc),_tmp544)))))));
struct Cyc_List_List*_tmp545;struct Cyc_List_List*work2=(_tmp545=_region_malloc(r,sizeof(*_tmp545)),((_tmp545->hd=work_frame,((_tmp545->tl=work,_tmp545)))));
void*_tmp2AF=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2B0=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2A9),work),rules);
# 1291
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp548;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp547;return(void*)((_tmp547=_region_malloc(r,sizeof(*_tmp547)),((_tmp547[0]=((_tmp548.tag=2,((_tmp548.f1=obj,((_tmp548.f2=_tmp2A9,((_tmp548.f3=_tmp2AF,((_tmp548.f4=_tmp2B0,_tmp548)))))))))),_tmp547))));}}}_LL154:;}
# 1301
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1307
void*_tmp2B7=d;void*_tmp2B9;void*_tmp2BB;void*_tmp2BD;void*_tmp2BE;_LL15E: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2B8=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp2B7;if(_tmp2B8->tag != 0)goto _LL160;else{_tmp2B9=(void*)_tmp2B8->f1;}}_LL15F:
# 1309
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp2B9);*exhaust_done=1;}
goto _LL15D;_LL160: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2BA=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp2B7;if(_tmp2BA->tag != 1)goto _LL162;else{_tmp2BB=(void*)_tmp2BA->f1;}}_LL161:
 rhs_appears(env2,_tmp2BB);goto _LL15D;_LL162: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp2BC=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp2B7;if(_tmp2BC->tag != 2)goto _LL15D;else{_tmp2BD=(void*)_tmp2BC->f3;_tmp2BE=(void*)_tmp2BC->f4;}}_LL163:
# 1313
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp2BD,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1315
Cyc_Tcpat_check_exhaust_overlap(r,_tmp2BE,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1317
goto _LL15D;_LL15D:;}struct _tuple23{void*f1;struct _tuple20*f2;};struct _tuple24{struct _RegionHandle*f1;int*f2;};
# 1325
static struct _tuple23*Cyc_Tcpat_get_match(struct _tuple24*env,struct Cyc_Absyn_Switch_clause*swc){
# 1328
struct _RegionHandle*_tmp2C0;int*_tmp2C1;struct _tuple24*_tmp2BF=env;_tmp2C0=_tmp2BF->f1;_tmp2C1=_tmp2BF->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2C0,swc->pattern);
struct _tuple20*_tmp549;struct _tuple20*rhs=(_tmp549=_region_malloc(_tmp2C0,sizeof(*_tmp549)),((_tmp549->f1=0,((_tmp549->f2=(swc->pattern)->loc,_tmp549)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54B;sp=Cyc_Tcpat_tuple_pat(_tmp2C0,((_tmp54B=_region_malloc(_tmp2C0,sizeof(*_tmp54B)),((_tmp54B->hd=sp0,((_tmp54B->tl=(
(_tmp54C=_region_malloc(_tmp2C0,sizeof(*_tmp54C)),((_tmp54C->hd=Cyc_Tcpat_int_pat(_tmp2C0,*_tmp2C1,0),((_tmp54C->tl=0,_tmp54C)))))),_tmp54B)))))),0);}
*_tmp2C1=*_tmp2C1 + 1;}else{
# 1337
struct Cyc_List_List*_tmp556;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp555;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp554;struct Cyc_List_List*_tmp553;sp=Cyc_Tcpat_tuple_pat(_tmp2C0,((_tmp553=_region_malloc(_tmp2C0,sizeof(*_tmp553)),((_tmp553->hd=sp0,((_tmp553->tl=((_tmp556=_region_malloc(_tmp2C0,sizeof(*_tmp556)),((_tmp556->hd=(void*)((_tmp554=_region_malloc(_tmp2C0,sizeof(*_tmp554)),((_tmp554[0]=((_tmp555.tag=0,_tmp555)),_tmp554)))),((_tmp556->tl=0,_tmp556)))))),_tmp553)))))),0);}{
struct _tuple23*_tmp557;return(_tmp557=_region_malloc(_tmp2C0,sizeof(*_tmp557)),((_tmp557->f1=sp,((_tmp557->f2=rhs,_tmp557)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1343
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1345
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2CB=d;struct Cyc_Tcpat_Con_s*_tmp2CD;struct Cyc_List_List*_tmp2CE;struct Cyc_Set_Set*_tmp2D0;_LL165: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2CC=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2CB;if(_tmp2CC->tag != 0)goto _LL167;else{_tmp2CD=_tmp2CC->f1;_tmp2CE=_tmp2CC->f2;}}_LL166: {
# 1349
union Cyc_Tcpat_Name_value _tmp2D1=_tmp2CD->name;
struct Cyc_Absyn_Pat*_tmp2D2=_tmp2CD->orig_pat;
if(_tmp2D2 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2CE))->hd);{
# 1354
void*_stmttmp27=_tmp2D2->r;void*_tmp2D3=_stmttmp27;struct Cyc_Absyn_Vardecl*_tmp2D6;struct Cyc_Absyn_Vardecl*_tmp2D8;struct Cyc_Absyn_Tvar*_tmp2DA;struct Cyc_Absyn_Vardecl*_tmp2DB;struct Cyc_Absyn_Aggrdecl*_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_Absyn_Datatypefield*_tmp2E2;int _tmp2E5;char _tmp2E7;struct _dyneither_ptr _tmp2E9;int _tmp2EA;struct Cyc_Absyn_Enumfield*_tmp2EC;struct Cyc_Absyn_Enumfield*_tmp2EE;struct Cyc_Absyn_Exp*_tmp2F0;_LL16A: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2D4=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2D4->tag != 0)goto _LL16C;}_LL16B: {
const char*_tmp558;return(_tmp558="_",_tag_dyneither(_tmp558,sizeof(char),2));}_LL16C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2D5=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2D5->tag != 1)goto _LL16E;else{_tmp2D6=_tmp2D5->f1;}}_LL16D:
 return Cyc_Absynpp_qvar2string(_tmp2D6->name);_LL16E: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2D7=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2D7->tag != 2)goto _LL170;else{_tmp2D8=_tmp2D7->f1;}}_LL16F: {
const char*_tmp55C;void*_tmp55B[1];struct Cyc_String_pa_PrintArg_struct _tmp55A;return(struct _dyneither_ptr)((_tmp55A.tag=0,((_tmp55A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D8->name)),((_tmp55B[0]=& _tmp55A,Cyc_aprintf(((_tmp55C="*%s",_tag_dyneither(_tmp55C,sizeof(char),4))),_tag_dyneither(_tmp55B,sizeof(void*),1))))))));}_LL170: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2D9=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2D9->tag != 3)goto _LL172;else{_tmp2DA=_tmp2D9->f1;_tmp2DB=_tmp2D9->f2;}}_LL171: {
const char*_tmp561;void*_tmp560[2];struct Cyc_String_pa_PrintArg_struct _tmp55F;struct Cyc_String_pa_PrintArg_struct _tmp55E;return(struct _dyneither_ptr)((_tmp55E.tag=0,((_tmp55E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DA->name),((_tmp55F.tag=0,((_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DB->name)),((_tmp560[0]=& _tmp55F,((_tmp560[1]=& _tmp55E,Cyc_aprintf(((_tmp561="%s<`%s>",_tag_dyneither(_tmp561,sizeof(char),8))),_tag_dyneither(_tmp560,sizeof(void*),2))))))))))))));}_LL172: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2DC=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2DC->tag != 4)goto _LL174;}_LL173: {
# 1360
const char*_tmp565;void*_tmp564[1];struct Cyc_String_pa_PrintArg_struct _tmp563;return(struct _dyneither_ptr)((_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CE)),((_tmp564[0]=& _tmp563,Cyc_aprintf(((_tmp565="$(%s)",_tag_dyneither(_tmp565,sizeof(char),6))),_tag_dyneither(_tmp564,sizeof(void*),1))))))));}_LL174: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2DD=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2DD->tag != 5)goto _LL176;}_LL175: {
# 1362
const char*_tmp569;void*_tmp568[1];struct Cyc_String_pa_PrintArg_struct _tmp567;return(struct _dyneither_ptr)((_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CE)),((_tmp568[0]=& _tmp567,Cyc_aprintf(((_tmp569="&%s",_tag_dyneither(_tmp569,sizeof(char),4))),_tag_dyneither(_tmp568,sizeof(void*),1))))))));}_LL176: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2DE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2DE->tag != 6)goto _LL178;else{if(_tmp2DE->f1 == 0)goto _LL178;if((((_tmp2DE->f1)->aggr_info).KnownAggr).tag != 2)goto _LL178;_tmp2DF=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2DE->f1)->aggr_info).KnownAggr).val);_tmp2E0=_tmp2DE->f3;}}_LL177:
# 1364
 if(_tmp2DF->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2FF=_tmp2E0;struct _dyneither_ptr*_tmp301;_LL18B: if(_tmp2FF == 0)goto _LL18D;if(((struct _tuple15*)_tmp2FF->hd)->f1 == 0)goto _LL18D;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp300=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2FF->hd)->f1)->hd);if(_tmp300->tag != 1)goto _LL18D;else{_tmp301=_tmp300->f1;}};_LL18C: {
# 1367
const char*_tmp56F;void*_tmp56E[3];struct Cyc_String_pa_PrintArg_struct _tmp56D;struct Cyc_String_pa_PrintArg_struct _tmp56C;struct Cyc_String_pa_PrintArg_struct _tmp56B;return(struct _dyneither_ptr)((_tmp56B.tag=0,((_tmp56B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2CE)),((_tmp56C.tag=0,((_tmp56C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp301),((_tmp56D.tag=0,((_tmp56D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1367
Cyc_Absynpp_qvar2string(_tmp2DF->name)),((_tmp56E[0]=& _tmp56D,((_tmp56E[1]=& _tmp56C,((_tmp56E[2]=& _tmp56B,Cyc_aprintf(((_tmp56F="%s{.%s = %s}",_tag_dyneither(_tmp56F,sizeof(char),13))),_tag_dyneither(_tmp56E,sizeof(void*),3))))))))))))))))))));}_LL18D:;_LL18E:
# 1369
 goto _LL18A;_LL18A:;}{
# 1372
const char*_tmp574;void*_tmp573[2];struct Cyc_String_pa_PrintArg_struct _tmp572;struct Cyc_String_pa_PrintArg_struct _tmp571;return(struct _dyneither_ptr)((_tmp571.tag=0,((_tmp571.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CE)),((_tmp572.tag=0,((_tmp572.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2DF->name)),((_tmp573[0]=& _tmp572,((_tmp573[1]=& _tmp571,Cyc_aprintf(((_tmp574="%s{%s}",_tag_dyneither(_tmp574,sizeof(char),7))),_tag_dyneither(_tmp573,sizeof(void*),2))))))))))))));};_LL178: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2E1=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2E1->tag != 7)goto _LL17A;else{_tmp2E2=_tmp2E1->f2;}}_LL179:
# 1374
 if(_tmp2CE == 0){
const char*_tmp578;void*_tmp577[1];struct Cyc_String_pa_PrintArg_struct _tmp576;return(struct _dyneither_ptr)((_tmp576.tag=0,((_tmp576.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2E2->name)),((_tmp577[0]=& _tmp576,Cyc_aprintf(((_tmp578="%s",_tag_dyneither(_tmp578,sizeof(char),3))),_tag_dyneither(_tmp577,sizeof(void*),1))))))));}else{
# 1377
const char*_tmp57D;void*_tmp57C[2];struct Cyc_String_pa_PrintArg_struct _tmp57B;struct Cyc_String_pa_PrintArg_struct _tmp57A;return(struct _dyneither_ptr)((_tmp57A.tag=0,((_tmp57A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CE)),((_tmp57B.tag=0,((_tmp57B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2E2->name)),((_tmp57C[0]=& _tmp57B,((_tmp57C[1]=& _tmp57A,Cyc_aprintf(((_tmp57D="%s(%s)",_tag_dyneither(_tmp57D,sizeof(char),7))),_tag_dyneither(_tmp57C,sizeof(void*),2))))))))))))));}_LL17A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2E3=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2E3->tag != 8)goto _LL17C;}_LL17B: {
const char*_tmp57E;return(_tmp57E="NULL",_tag_dyneither(_tmp57E,sizeof(char),5));}_LL17C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2E4=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2E4->tag != 9)goto _LL17E;else{_tmp2E5=_tmp2E4->f2;}}_LL17D: {
const char*_tmp582;void*_tmp581[1];struct Cyc_Int_pa_PrintArg_struct _tmp580;return(struct _dyneither_ptr)((_tmp580.tag=1,((_tmp580.f1=(unsigned long)_tmp2E5,((_tmp581[0]=& _tmp580,Cyc_aprintf(((_tmp582="%d",_tag_dyneither(_tmp582,sizeof(char),3))),_tag_dyneither(_tmp581,sizeof(void*),1))))))));}_LL17E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2E6=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2E6->tag != 10)goto _LL180;else{_tmp2E7=_tmp2E6->f1;}}_LL17F: {
const char*_tmp586;void*_tmp585[1];struct Cyc_Int_pa_PrintArg_struct _tmp584;return(struct _dyneither_ptr)((_tmp584.tag=1,((_tmp584.f1=(unsigned long)((int)_tmp2E7),((_tmp585[0]=& _tmp584,Cyc_aprintf(((_tmp586="%d",_tag_dyneither(_tmp586,sizeof(char),3))),_tag_dyneither(_tmp585,sizeof(void*),1))))))));}_LL180: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2E8=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2E8->tag != 11)goto _LL182;else{_tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E8->f2;}}_LL181:
 return _tmp2E9;_LL182: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2EB=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2EB->tag != 12)goto _LL184;else{_tmp2EC=_tmp2EB->f2;}}_LL183:
 _tmp2EE=_tmp2EC;goto _LL185;_LL184: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2ED=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2ED->tag != 13)goto _LL186;else{_tmp2EE=_tmp2ED->f2;}}_LL185:
 return Cyc_Absynpp_qvar2string(_tmp2EE->name);_LL186: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2EF=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2D3;if(_tmp2EF->tag != 16)goto _LL188;else{_tmp2F0=_tmp2EF->f1;}}_LL187:
 return Cyc_Absynpp_exp2string(_tmp2F0);_LL188:;_LL189:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL169:;};}_LL167: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2CF=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2CB;if(_tmp2CF->tag != 1)goto _LL164;else{_tmp2D0=_tmp2CF->f1;}}_LL168:
# 1388
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2D0)){const char*_tmp587;return(_tmp587="_",_tag_dyneither(_tmp587,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp31A=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2D0);
struct Cyc_Absyn_Pat*_tmp31B=_tmp31A->orig_pat;
# 1393
if(_tmp31B == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_stmttmp25=Cyc_Tcutil_compress((void*)_check_null(_tmp31B->topt));void*_tmp31C=_stmttmp25;struct Cyc_Absyn_PtrAtts _tmp320;struct Cyc_Absyn_Datatypedecl*_tmp322;struct Cyc_Absyn_Aggrdecl*_tmp324;_LL190: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp31C;if(_tmp31D->tag != 6)goto _LL192;else{if(_tmp31D->f2 != Cyc_Absyn_Char_sz)goto _LL192;}}_LL191:
# 1397
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp325=Cyc_Tcpat_char_con(r,(char)i,_tmp31B);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2D0,_tmp325)){
const char*_tmp58B;void*_tmp58A[1];struct Cyc_Int_pa_PrintArg_struct _tmp589;return(struct _dyneither_ptr)((_tmp589.tag=1,((_tmp589.f1=(unsigned long)i,((_tmp58A[0]=& _tmp589,Cyc_aprintf(((_tmp58B="%d",_tag_dyneither(_tmp58B,sizeof(char),3))),_tag_dyneither(_tmp58A,sizeof(void*),1))))))));}}}
# 1402
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL192: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp31E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp31C;if(_tmp31E->tag != 6)goto _LL194;}_LL193:
# 1405
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp329=Cyc_Tcpat_int_con(r,(int)i,_tmp31B);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2D0,_tmp329)){
const char*_tmp58F;void*_tmp58E[1];struct Cyc_Int_pa_PrintArg_struct _tmp58D;return(struct _dyneither_ptr)((_tmp58D.tag=1,((_tmp58D.f1=(unsigned long)((int)i),((_tmp58E[0]=& _tmp58D,Cyc_aprintf(((_tmp58F="%d",_tag_dyneither(_tmp58F,sizeof(char),3))),_tag_dyneither(_tmp58E,sizeof(void*),1))))))));}}}
# 1410
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL194: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C;if(_tmp31F->tag != 5)goto _LL196;else{_tmp320=(_tmp31F->f1).ptr_atts;}}_LL195: {
# 1412
union Cyc_Absyn_Constraint*_tmp32D=_tmp320.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32D);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2D0,Cyc_Tcpat_null_con(r,_tmp31B))){
const char*_tmp590;return(_tmp590="NULL",_tag_dyneither(_tmp590,sizeof(char),5));}}{
# 1418
const char*_tmp591;return(_tmp591="&_",_tag_dyneither(_tmp591,sizeof(char),3));};}_LL196: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp31C;if(_tmp321->tag != 3)goto _LL198;else{if((((_tmp321->f1).datatype_info).KnownDatatype).tag != 2)goto _LL198;_tmp322=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp321->f1).datatype_info).KnownDatatype).val);}}_LL197:
# 1420
 if(_tmp322->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp330=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp322->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp330);
for(0;(unsigned int)_tmp330;_tmp330=_tmp330->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp330->hd)->name).f2;
struct Cyc_List_List*_tmp331=((struct Cyc_Absyn_Datatypefield*)_tmp330->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp592;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2D0,((_tmp592=_cycalloc(sizeof(*_tmp592)),((_tmp592->name=Cyc_Tcpat_Name_v(n),((_tmp592->arity=0,((_tmp592->span=0,((_tmp592->orig_pat=_tmp31B,_tmp592)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp331)== 0)
return n;else{
# 1430
const char*_tmp596;void*_tmp595[1];struct Cyc_String_pa_PrintArg_struct _tmp594;return(struct _dyneither_ptr)((_tmp594.tag=0,((_tmp594.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp595[0]=& _tmp594,Cyc_aprintf(((_tmp596="%s(...)",_tag_dyneither(_tmp596,sizeof(char),8))),_tag_dyneither(_tmp595,sizeof(void*),1))))))));}}}
# 1433
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL198:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C;if(_tmp323->tag != 11)goto _LL19A;else{if((((_tmp323->f1).aggr_info).KnownAggr).tag != 2)goto _LL19A;_tmp324=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp323->f1).aggr_info).KnownAggr).val);}}if(!(_tmp324->kind == Cyc_Absyn_UnionA))goto _LL19A;_LL199: {
# 1435
struct Cyc_List_List*_tmp336=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp324->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp336);
struct _tuple2*_stmttmp26=_tmp324->name;struct _dyneither_ptr _tmp338;struct _tuple2*_tmp337=_stmttmp26;_tmp338=*_tmp337->f2;
for(0;(unsigned int)_tmp336;_tmp336=_tmp336->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp336->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp597;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2D0,((_tmp597=_cycalloc(sizeof(*_tmp597)),((_tmp597->name=Cyc_Tcpat_Name_v(n),((_tmp597->arity=0,((_tmp597->span=0,((_tmp597->orig_pat=_tmp31B,_tmp597)))))))))))){
const char*_tmp59C;void*_tmp59B[2];struct Cyc_String_pa_PrintArg_struct _tmp59A;struct Cyc_String_pa_PrintArg_struct _tmp599;return(struct _dyneither_ptr)((_tmp599.tag=0,((_tmp599.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp59A.tag=0,((_tmp59A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp338),((_tmp59B[0]=& _tmp59A,((_tmp59B[1]=& _tmp599,Cyc_aprintf(((_tmp59C="%s{.%s = _}",_tag_dyneither(_tmp59C,sizeof(char),12))),_tag_dyneither(_tmp59B,sizeof(void*),2))))))))))))));}}
# 1443
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL18F:;};};_LL164:;}
# 1448
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp59D;return(_tmp59D=_cycalloc(sizeof(*_tmp59D)),((_tmp59D[0]=Cyc_Tcpat_desc2string(r,d),_tmp59D)));}
# 1451
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp33F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp340=_tmp33F;for(0;_tmp340 != 0;_tmp340=((struct Cyc_List_List*)_check_null(_tmp340))->tl){
if(_tmp340->tl != 0){
{struct Cyc_List_List*_tmp59E;_tmp340->tl=((_tmp59E=_cycalloc(sizeof(*_tmp59E)),((_tmp59E->hd=comma,((_tmp59E->tl=_tmp340->tl,_tmp59E))))));}
_tmp340=_tmp340->tl;}}}
# 1460
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp33F);}
# 1463
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp343;_push_handler(& _tmp343);{int _tmp345=0;if(setjmp(_tmp343.handler))_tmp345=1;if(!_tmp345){
{struct _dyneither_ptr _tmp346=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5A2;void*_tmp5A1[1];struct Cyc_String_pa_PrintArg_struct _tmp5A0;(_tmp5A0.tag=0,((_tmp5A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp346),((_tmp5A1[0]=& _tmp5A0,Cyc_Tcutil_terr(loc,((_tmp5A2="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5A2,sizeof(char),53))),_tag_dyneither(_tmp5A1,sizeof(void*),1)))))));}
# 1465
;_pop_handler();}else{void*_tmp344=(void*)_exn_thrown;void*_tmp34B=_tmp344;void*_tmp34D;_LL19D: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp34C=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp34B;if(_tmp34C->tag != Cyc_Tcpat_Desc2string)goto _LL19F;}_LL19E:
# 1469
{const char*_tmp5A5;void*_tmp5A4;(_tmp5A4=0,Cyc_Tcutil_terr(loc,((_tmp5A5="patterns may not be exhaustive.",_tag_dyneither(_tmp5A5,sizeof(char),32))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}
goto _LL19C;_LL19F: _tmp34D=_tmp34B;_LL1A0:(void)_throw(_tmp34D);_LL19C:;}};}
# 1473
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple20*rhs){
(*rhs).f1=1;}
# 1477
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1482
struct _RegionHandle _tmp350=_new_region("r");struct _RegionHandle*r=& _tmp350;_push_region(r);
{int _tmp351=0;
struct _tuple24 _tmp5A6;struct _tuple24 _tmp352=(_tmp5A6.f1=r,((_tmp5A6.f2=& _tmp351,_tmp5A6)));
struct Cyc_List_List*_tmp353=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _tuple24*,struct Cyc_Absyn_Switch_clause*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp352,swcs);
void*_tmp354=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp353);
int _tmp355=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp354,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp355);
# 1490
for(0;_tmp353 != 0;_tmp353=_tmp353->tl){
struct _tuple23*_stmttmp28=(struct _tuple23*)_tmp353->hd;int _tmp357;unsigned int _tmp358;struct _tuple23*_tmp356=_stmttmp28;_tmp357=(_tmp356->f2)->f1;_tmp358=(_tmp356->f2)->f2;
if(!_tmp357){
{const char*_tmp5A9;void*_tmp5A8;(_tmp5A8=0,Cyc_Tcutil_terr(_tmp358,((_tmp5A9="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5A9,sizeof(char),74))),_tag_dyneither(_tmp5A8,sizeof(void*),0)));}
# 1495
break;}}}
# 1483
;_pop_region(r);}
# 1505
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp35C;_push_handler(& _tmp35C);{int _tmp35E=0;if(setjmp(_tmp35C.handler))_tmp35E=1;if(!_tmp35E){
{struct _dyneither_ptr _tmp35F=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp5AD;void*_tmp5AC[1];struct Cyc_String_pa_PrintArg_struct _tmp5AB;(_tmp5AB.tag=0,((_tmp5AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35F),((_tmp5AC[0]=& _tmp5AB,Cyc_Tcutil_warn((*pr).f1,((_tmp5AD="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp5AD,sizeof(char),45))),_tag_dyneither(_tmp5AC,sizeof(void*),1)))))));}
# 1508
;_pop_handler();}else{void*_tmp35D=(void*)_exn_thrown;void*_tmp364=_tmp35D;void*_tmp366;_LL1A2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp365=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp364;if(_tmp365->tag != Cyc_Tcpat_Desc2string)goto _LL1A4;}_LL1A3:
# 1512
{const char*_tmp5B0;void*_tmp5AF;(_tmp5AF=0,Cyc_Tcutil_warn((*pr).f1,((_tmp5B0="pattern not exhaustive.",_tag_dyneither(_tmp5B0,sizeof(char),24))),_tag_dyneither(_tmp5AF,sizeof(void*),0)));}
goto _LL1A1;_LL1A4: _tmp366=_tmp364;_LL1A5:(void)_throw(_tmp366);_LL1A1:;}};};}
# 1516
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple25{void*f1;int f2;};
# 1519
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp369=_new_region("r");struct _RegionHandle*r=& _tmp369;_push_region(r);
{struct _tuple25*_tmp5B3;struct Cyc_List_List*_tmp5B2;struct Cyc_List_List*_tmp36A=(_tmp5B2=_region_malloc(r,sizeof(*_tmp5B2)),((_tmp5B2->hd=((_tmp5B3=_region_malloc(r,sizeof(*_tmp5B3)),((_tmp5B3->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp5B3->f2=0,_tmp5B3)))))),((_tmp5B2->tl=0,_tmp5B2)))));
void*_tmp36B=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp36A);
struct _tuple14 _tmp5B4;struct _tuple14 _tmp36C=(_tmp5B4.f1=loc,((_tmp5B4.f2=1,_tmp5B4)));
int _tmp36D=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp36B,Cyc_Tcpat_not_exhaust_warn,& _tmp36C,Cyc_Tcpat_dummy_fn,0,& _tmp36D);{
# 1527
int _tmp36E=_tmp36C.f2;_npop_handler(0);return _tmp36E;};}
# 1521
;_pop_region(r);}
# 1535
static struct _tuple23*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1537
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple20*_tmp5B5;struct _tuple20*rhs=(_tmp5B5=_region_malloc(r,sizeof(*_tmp5B5)),((_tmp5B5->f1=0,((_tmp5B5->f2=(swc->pattern)->loc,_tmp5B5)))));
struct _tuple23*_tmp5B6;return(_tmp5B6=_region_malloc(r,sizeof(*_tmp5B6)),((_tmp5B6->f1=sp0,((_tmp5B6->f2=rhs,_tmp5B6)))));}
# 1541
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1544
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1546
struct _RegionHandle _tmp374=_new_region("r");struct _RegionHandle*r=& _tmp374;_push_region(r);
{struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple23*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp376=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp375);
int _tmp377=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp376,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp377);
# 1552
for(0;_tmp375 != 0;_tmp375=_tmp375->tl){
struct _tuple23*_stmttmp29=(struct _tuple23*)_tmp375->hd;int _tmp379;unsigned int _tmp37A;struct _tuple23*_tmp378=_stmttmp29;_tmp379=(_tmp378->f2)->f1;_tmp37A=(_tmp378->f2)->f2;
if(!_tmp379){
{const char*_tmp5B9;void*_tmp5B8;(_tmp5B8=0,Cyc_Tcutil_terr(_tmp37A,((_tmp5B9="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp5B9,sizeof(char),74))),_tag_dyneither(_tmp5B8,sizeof(void*),0)));}
break;}}}
# 1547
;_pop_region(r);}
