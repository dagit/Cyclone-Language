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
# 244 "core.h"
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
# 264
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 369
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
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
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 200
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
# 173
struct _tuple12*Cyc_Tcutil_r_make_inst_var(struct _tuple11*,struct Cyc_Absyn_Tvar*);
# 221 "tcutil.h"
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 236
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 264
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 274
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 294
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
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
{const char*_tmp358;void*_tmp357;(_tmp357=0,Cyc_Tcutil_terr(p->loc,((_tmp358="struct tag used without arguments in pattern",_tag_dyneither(_tmp358,sizeof(char),45))),_tag_dyneither(_tmp357,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1A=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1A->tag != 2)goto _LL12;else{_tmp1B=_tmp1A->f1;_tmp1C=_tmp1A->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp35B;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp35A;p->r=(void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp35B.tag=7,((_tmp35B.f1=_tmp1B,((_tmp35B.f2=_tmp1C,((_tmp35B.f3=0,((_tmp35B.f4=0,_tmp35B)))))))))),_tmp35A))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1D=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp1D->tag != 3)goto _LL14;else{_tmp1E=_tmp1D->f1;_tmp1F=_tmp1D->f2;}}_LL13:
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp35E;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp35D;p->r=(void*)((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D[0]=((_tmp35E.tag=12,((_tmp35E.f1=_tmp1E,((_tmp35E.f2=_tmp1F,_tmp35E)))))),_tmp35D))));}_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp20=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp20->tag != 4)goto _LL16;else{_tmp21=(void*)_tmp20->f1;_tmp22=_tmp20->f2;}}_LL15:
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp361;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp360;p->r=(void*)((_tmp360=_cycalloc(sizeof(*_tmp360)),((_tmp360[0]=((_tmp361.tag=13,((_tmp361.f1=_tmp21,((_tmp361.f2=_tmp22,_tmp361)))))),_tmp360))));}_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp23=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp18;if(_tmp23->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp15=(void*)_exn_thrown;void*_tmp2D=_tmp15;void*_tmp2F;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 69
 goto _LL18;_LL1B: _tmp2F=_tmp2D;_LL1C:(void)_throw(_tmp2F);_LL18:;}};}{
# 71
union Cyc_Absyn_Nmspace _tmp30=(*_tmp2).f1;_LL1E: if((_tmp30.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp30.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL22;_LL21:
# 74
(*_tmp2).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp364;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp363;p->r=(void*)((_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363[0]=((_tmp364.tag=1,((_tmp364.f1=Cyc_Absyn_new_vardecl(_tmp2,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp364.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp364)))))),_tmp363))));}
return;_LL22:;_LL23:
{const char*_tmp367;void*_tmp366;(_tmp366=0,Cyc_Tcutil_terr(p->loc,((_tmp367="qualified variable in pattern",_tag_dyneither(_tmp367,sizeof(char),30))),_tag_dyneither(_tmp366,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=_tmp3->f3;}}_LL4:
# 81
{struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler))_tmp37=1;if(!_tmp37){
{struct _RegionHandle _tmp38=_new_region("r");struct _RegionHandle*r=& _tmp38;_push_region(r);
{void*_tmp39=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp4,0);struct Cyc_Absyn_Aggrdecl*_tmp3B;struct Cyc_Absyn_Datatypedecl*_tmp3D;struct Cyc_Absyn_Datatypefield*_tmp3E;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3A=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3A->tag != 1)goto _LL27;else{_tmp3B=_tmp3A->f1;}}_LL26: {
# 85
struct Cyc_List_List*_tmp42=0;
for(0;_tmp5 != 0;_tmp5=_tmp5->tl){
struct _tuple15*_tmp36A;struct Cyc_List_List*_tmp369;_tmp42=((_tmp369=_cycalloc(sizeof(*_tmp369)),((_tmp369->hd=((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A->f1=0,((_tmp36A->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd,_tmp36A)))))),((_tmp369->tl=_tmp42,_tmp369))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp374;struct Cyc_Absyn_Aggrdecl**_tmp373;struct Cyc_Absyn_AggrInfo*_tmp372;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp371;p->r=(void*)((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371[0]=((_tmp374.tag=6,((_tmp374.f1=((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372->aggr_info=Cyc_Absyn_KnownAggr(((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=_tmp3B,_tmp373))))),((_tmp372->targs=0,_tmp372)))))),((_tmp374.f2=0,((_tmp374.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp42),((_tmp374.f4=_tmp6,_tmp374)))))))))),_tmp371))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3C->tag != 2)goto _LL29;else{_tmp3D=_tmp3C->f1;_tmp3E=_tmp3C->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp377;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp376;p->r=(void*)((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp377.tag=7,((_tmp377.f1=_tmp3D,((_tmp377.f2=_tmp3E,((_tmp377.f3=_tmp5,((_tmp377.f4=_tmp6,_tmp377)))))))))),_tmp376))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp3F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp3F->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp40->tag != 4)goto _LL2D;}_LL2C:
# 94
{const char*_tmp37A;void*_tmp379;(_tmp379=0,Cyc_Tcutil_terr(p->loc,((_tmp37A="enum tag used with arguments in pattern",_tag_dyneither(_tmp37A,sizeof(char),40))),_tag_dyneither(_tmp379,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp41=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp39;if(_tmp41->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 83
;_pop_region(r);}
# 82
;_pop_handler();}else{void*_tmp36=(void*)_exn_thrown;void*_tmp4E=_tmp36;void*_tmp50;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp4F=(struct Cyc_Dict_Absent_exn_struct*)_tmp4E;if(_tmp4F->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 99
 goto _LL2F;_LL32: _tmp50=_tmp4E;_LL33:(void)_throw(_tmp50);_LL2F:;}};}
{const char*_tmp37E;void*_tmp37D[1];struct Cyc_String_pa_PrintArg_struct _tmp37C;(_tmp37C.tag=0,((_tmp37C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp37D[0]=& _tmp37C,Cyc_Tcutil_terr(p->loc,((_tmp37E="%s is not a constructor in pattern",_tag_dyneither(_tmp37E,sizeof(char),35))),_tag_dyneither(_tmp37D,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{if(_tmp7->f1 != 0)goto _LL7;_tmp8=_tmp7->f2;_tmp9=_tmp7->f3;_tmpA=_tmp7->f4;}}_LL6:
# 105
 if(topt == 0){
const char*_tmp381;void*_tmp380;(_tmp380=0,Cyc_Tcutil_terr(p->loc,((_tmp381="cannot determine pattern type",_tag_dyneither(_tmp381,sizeof(char),30))),_tag_dyneither(_tmp380,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp56=t;struct Cyc_Absyn_AggrInfo _tmp58;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp57=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56;if(_tmp57->tag != 11)goto _LL37;else{_tmp58=_tmp57->f1;}}_LL36:
# 110
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp387;struct Cyc_Absyn_AggrInfo*_tmp386;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp385;p->r=(void*)((_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385[0]=((_tmp387.tag=6,((_tmp387.f1=((_tmp386=_cycalloc(sizeof(*_tmp386)),((_tmp386[0]=_tmp58,_tmp386)))),((_tmp387.f2=_tmp8,((_tmp387.f3=_tmp9,((_tmp387.f4=_tmpA,_tmp387)))))))))),_tmp385))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 114
{const char*_tmp38B;void*_tmp38A[1];struct Cyc_String_pa_PrintArg_struct _tmp389;(_tmp389.tag=0,((_tmp389.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp38A[0]=& _tmp389,Cyc_Tcutil_terr(p->loc,((_tmp38B="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp38B,sizeof(char),45))),_tag_dyneither(_tmp38A,sizeof(void*),1)))))));}
# 117
goto _LL34;_LL34:;}
# 119
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmpB->tag != 6)goto _LL9;else{if(_tmpB->f1 == 0)goto _LL9;if((((_tmpB->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpC=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f1;_tmpD=((struct _tuple3)(((_tmpB->f1)->aggr_info).UnknownAggr).val).f2;_tmpE=(_tmpB->f1)->targs;_tmpF=(struct Cyc_List_List**)& _tmpB->f2;_tmp10=_tmpB->f3;_tmp11=_tmpB->f4;}}_LL8:
# 122
{struct _handler_cons _tmp5F;_push_handler(& _tmp5F);{int _tmp61=0;if(setjmp(_tmp5F.handler))_tmp61=1;if(!_tmp61){
{struct Cyc_Absyn_Aggrdecl**_tmp62=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpD);
struct Cyc_Absyn_Aggrdecl*_tmp63=*_tmp62;
if(_tmp63->impl == 0){
{const char*_tmp393;void*_tmp392[1];const char*_tmp391;const char*_tmp390;struct Cyc_String_pa_PrintArg_struct _tmp38F;(_tmp38F.tag=0,((_tmp38F.f1=(struct _dyneither_ptr)(
_tmp63->kind == Cyc_Absyn_StructA?(_tmp390="struct",_tag_dyneither(_tmp390,sizeof(char),7)):((_tmp391="union",_tag_dyneither(_tmp391,sizeof(char),6)))),((_tmp392[0]=& _tmp38F,Cyc_Tcutil_terr(p->loc,((_tmp393="can't destructure an abstract %s",_tag_dyneither(_tmp393,sizeof(char),33))),_tag_dyneither(_tmp392,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 131
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmpF);
if(more_exists < 0){
{const char*_tmp396;void*_tmp395;(_tmp395=0,Cyc_Tcutil_terr(p->loc,((_tmp396="too many existentially bound type variables in pattern",_tag_dyneither(_tmp396,sizeof(char),55))),_tag_dyneither(_tmp395,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6B=_tmpF;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp63->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6B=&((struct Cyc_List_List*)_check_null(*_tmp6B))->tl;}}
*_tmp6B=0;};}else{
if(more_exists > 0){
# 140
struct Cyc_List_List*_tmp6C=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp39C;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp39B;struct Cyc_List_List*_tmp39A;_tmp6C=((_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A->hd=Cyc_Tcutil_new_tvar((void*)((_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C[0]=((_tmp39B.tag=1,((_tmp39B.f1=0,_tmp39B)))),_tmp39C))))),((_tmp39A->tl=_tmp6C,_tmp39A))))));}
*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpF,_tmp6C);}}{
# 145
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3A2;struct Cyc_Absyn_AggrInfo*_tmp3A1;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3A0;p->r=(void*)((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0[0]=((_tmp3A2.tag=6,((_tmp3A2.f1=((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1->aggr_info=Cyc_Absyn_KnownAggr(_tmp62),((_tmp3A1->targs=_tmpE,_tmp3A1)))))),((_tmp3A2.f2=*_tmpF,((_tmp3A2.f3=_tmp10,((_tmp3A2.f4=_tmp11,_tmp3A2)))))))))),_tmp3A0))));};};}
# 123
;_pop_handler();}else{void*_tmp60=(void*)_exn_thrown;void*_tmp74=_tmp60;void*_tmp76;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp75=(struct Cyc_Dict_Absent_exn_struct*)_tmp74;if(_tmp75->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 148
{const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_Tcutil_terr(p->loc,((_tmp3A5="Non-aggregate name has designator patterns",_tag_dyneither(_tmp3A5,sizeof(char),43))),_tag_dyneither(_tmp3A4,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp76=_tmp74;_LL3D:(void)_throw(_tmp76);_LL39:;}};}
# 152
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp12=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp0;if(_tmp12->tag != 16)goto _LLB;else{_tmp13=_tmp12->f1;}}_LLA:
# 154
 Cyc_Tcexp_tcExp(te,0,_tmp13);
if(!Cyc_Tcutil_is_const_exp(_tmp13)){
{const char*_tmp3A8;void*_tmp3A7;(_tmp3A7=0,Cyc_Tcutil_terr(p->loc,((_tmp3A8="non-constant expression in case pattern",_tag_dyneither(_tmp3A8,sizeof(char),40))),_tag_dyneither(_tmp3A7,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 159
unsigned int _tmp7C;int _tmp7D;struct _tuple14 _tmp7B=Cyc_Evexp_eval_const_uint_exp(_tmp13);_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3AB;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp3AA;p->r=(void*)((_tmp3AA=_cycalloc_atomic(sizeof(*_tmp3AA)),((_tmp3AA[0]=((_tmp3AB.tag=9,((_tmp3AB.f1=Cyc_Absyn_None,((_tmp3AB.f2=(int)_tmp7C,_tmp3AB)))))),_tmp3AA))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 166
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 169
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp3AC;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->v=s,_tmp3AC)))));};}
# 174
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 178
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 181
{void*_tmp81=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp81;if(_tmp82->tag != 13)goto _LL41;}_LL40:
 if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42:
 goto _LL3E;_LL3E:;}
# 185
return numt;}
# 188
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**v_result_ptr,void*t){
vd->type=t;
vd->tq=Cyc_Absyn_empty_tqual(0);{
struct Cyc_List_List*_tmp3AD;*v_result_ptr=((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD->hd=vd,((_tmp3AD->tl=*v_result_ptr,_tmp3AD))))));};}
# 193
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 195
struct _tuple13*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_Tcpat_TcPatResult _tmp84=res1;_tmp85=_tmp84.tvars_and_bounds_opt;_tmp86=_tmp84.patvars;{
struct _tuple13*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_Tcpat_TcPatResult _tmp87=res2;_tmp88=_tmp87.tvars_and_bounds_opt;_tmp89=_tmp87.patvars;
if(_tmp85 != 0  || _tmp88 != 0){
if(_tmp85 == 0){
struct _tuple13*_tmp3AE;_tmp85=((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->f1=0,((_tmp3AE->f2=0,_tmp3AE))))));}
if(_tmp88 == 0){
struct _tuple13*_tmp3AF;_tmp88=((_tmp3AF=_cycalloc(sizeof(*_tmp3AF)),((_tmp3AF->f1=0,((_tmp3AF->f2=0,_tmp3AF))))));}{
struct _tuple13*_tmp3B2;struct Cyc_Tcpat_TcPatResult _tmp3B1;return(_tmp3B1.tvars_and_bounds_opt=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp85).f1,(*_tmp88).f1),((_tmp3B2->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp85).f2,(*_tmp88).f2),_tmp3B2)))))),((_tmp3B1.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86,_tmp89),_tmp3B1)));};}{
# 206
struct Cyc_Tcpat_TcPatResult _tmp3B3;return(_tmp3B3.tvars_and_bounds_opt=0,((_tmp3B3.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp86,_tmp89),_tmp3B3)));};};}
# 209
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp3B4;return(_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4->loc=loc,((_tmp3B4->topt=0,((_tmp3B4->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp3B4)))))));}
# 213
static void*Cyc_Tcpat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
void*_tmp90=Cyc_Tcutil_compress(t);void*_tmp92;struct Cyc_Absyn_Tqual _tmp93;struct Cyc_Absyn_Exp*_tmp94;union Cyc_Absyn_Constraint*_tmp95;unsigned int _tmp96;_LL44: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90;if(_tmp91->tag != 8)goto _LL46;else{_tmp92=(_tmp91->f1).elt_type;_tmp93=(_tmp91->f1).tq;_tmp94=(_tmp91->f1).num_elts;_tmp95=(_tmp91->f1).zero_term;_tmp96=(_tmp91->f1).zt_loc;}}_LL45: {
# 216
struct Cyc_Core_Opt*_tmp3B5;void*rgn=rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp3B5=_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3B5))))): rgn_opt;
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp3B8;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3B7;return Cyc_Absyn_atb_typ(_tmp92,rgn,_tmp93,(void*)((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((_tmp3B8.tag=1,((_tmp3B8.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp94),_tmp3B8)))),_tmp3B7)))),_tmp95);}_LL46:;_LL47:
 return t;_LL43:;}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple17{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 222
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat){
# 224
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp3B9;struct Cyc_Tcpat_TcPatResult res=(_tmp3B9.tvars_and_bounds_opt=0,((_tmp3B9.patvars=0,_tmp3B9)));
# 229
{void*_tmp9A=p->r;struct Cyc_Absyn_Vardecl*_tmp9D;struct Cyc_Absyn_Pat*_tmp9E;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Pat*_tmpA1;struct Cyc_Absyn_Tvar*_tmpA3;struct Cyc_Absyn_Vardecl*_tmpA4;int _tmpAA;struct Cyc_Absyn_Enumdecl*_tmpAC;void*_tmpAE;struct Cyc_Absyn_Pat*_tmpB1;struct Cyc_List_List**_tmpB3;int _tmpB4;struct Cyc_Absyn_Aggrdecl*_tmpB6;struct Cyc_List_List**_tmpB7;struct Cyc_List_List*_tmpB8;struct Cyc_List_List**_tmpB9;int _tmpBA;struct Cyc_Absyn_Datatypedecl*_tmpBC;struct Cyc_Absyn_Datatypefield*_tmpBD;struct Cyc_List_List**_tmpBE;int _tmpBF;_LL49: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9B->tag != 0)goto _LL4B;}_LL4A:
# 232
 if(topt != 0)
t=*topt;else{
# 235
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL48;_LL4B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9C->tag != 1)goto _LL4D;else{_tmp9D=_tmp9C->f1;_tmp9E=_tmp9C->f2;}}_LL4C:
# 238
 res=Cyc_Tcpat_tcPatRec(te,_tmp9E,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmp9E->topt);
# 242
{void*_tmpC5=Cyc_Tcutil_compress(t);_LL74: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC5;if(_tmpC6->tag != 8)goto _LL76;}_LL75:
# 244
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp3BC;void*_tmp3BB;(_tmp3BB=0,Cyc_Tcutil_terr(p->loc,((_tmp3BC="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp3BC,sizeof(char),58))),_tag_dyneither(_tmp3BB,sizeof(void*),0)));}
goto _LL73;_LL76:;_LL77:
# 248
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp3BF;void*_tmp3BE;(_tmp3BE=0,Cyc_Tcutil_terr(p->loc,((_tmp3BF="pattern would point to an abstract member",_tag_dyneither(_tmp3BF,sizeof(char),42))),_tag_dyneither(_tmp3BE,sizeof(void*),0)));}
goto _LL73;_LL73:;}
# 252
Cyc_Tcpat_set_vd(_tmp9D,& res.patvars,Cyc_Tcpat_promote_array(te,t,rgn_pat));
goto _LL48;_LL4D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp9F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmp9F->tag != 2)goto _LL4F;else{_tmpA0=_tmp9F->f1;_tmpA1=_tmp9F->f2;}}_LL4E:
# 255
 res=Cyc_Tcpat_tcPatRec(te,_tmpA1,topt,rgn_pat,allow_ref_pat);
t=(void*)_check_null(_tmpA1->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp3C2;void*_tmp3C1;(_tmp3C1=0,Cyc_Tcutil_terr(p->loc,((_tmp3C2="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp3C2,sizeof(char),55))),_tag_dyneither(_tmp3C1,sizeof(void*),0)));}
goto _LL48;}else{
# 261
if(Cyc_Tcutil_is_noalias_pointer(t)){
const char*_tmp3C5;void*_tmp3C4;(_tmp3C4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3C5="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp3C5,sizeof(char),56))),_tag_dyneither(_tmp3C4,sizeof(void*),0)));}}
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3CB;struct Cyc_Absyn_PtrInfo _tmp3CA;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3C9;Cyc_Tcpat_set_vd(_tmpA0,& res.patvars,(void*)(
(_tmp3C9=_cycalloc(sizeof(*_tmp3C9)),((_tmp3C9[0]=((_tmp3CB.tag=5,((_tmp3CB.f1=((_tmp3CA.elt_typ=t,((_tmp3CA.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp3CA.ptr_atts=(
((_tmp3CA.ptr_atts).rgn=rgn_pat,(((_tmp3CA.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp3CA.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3CA.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3CA.ptr_atts).ptrloc=0,_tmp3CA.ptr_atts)))))))))),_tmp3CA)))))),_tmp3CB)))),_tmp3C9)))));}
# 268
goto _LL48;_LL4F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA2=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA2->tag != 3)goto _LL51;else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL50:
# 273
 Cyc_Tcpat_set_vd(_tmpA4,& res.patvars,_tmpA4->type);{
# 277
struct _RegionHandle _tmpD2=_new_region("r2");struct _RegionHandle*r2=& _tmpD2;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp3CC[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp3CC[0]=_tmpA3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3CC,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 280
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp3CD;res.tvars_and_bounds_opt=((_tmp3CD=_cycalloc(sizeof(*_tmp3CD)),((_tmp3CD->f1=0,((_tmp3CD->f2=0,_tmp3CD))))));}
{struct Cyc_List_List*_tmp3CE;(*res.tvars_and_bounds_opt).f1=(
(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->hd=_tmpA3,((_tmp3CE->tl=(*res.tvars_and_bounds_opt).f1,_tmp3CE))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL48;
# 277
;_pop_region(r2);};_LL51: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA5->tag != 9)goto _LL53;else{if(_tmpA5->f1 != Cyc_Absyn_Unsigned)goto _LL53;}}_LL52:
# 287
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL48;_LL53: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA6->tag != 9)goto _LL55;else{if(_tmpA6->f1 != Cyc_Absyn_None)goto _LL55;}}_LL54:
 goto _LL56;_LL55: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA7=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA7->tag != 9)goto _LL57;else{if(_tmpA7->f1 != Cyc_Absyn_Signed)goto _LL57;}}_LL56:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL48;_LL57: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpA8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA8->tag != 10)goto _LL59;}_LL58:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL48;_LL59: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpA9->tag != 11)goto _LL5B;else{_tmpAA=_tmpA9->f2;}}_LL5A:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAA));goto _LL48;_LL5B: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAB->tag != 12)goto _LL5D;else{_tmpAC=_tmpAB->f1;}}_LL5C:
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp3D1;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3D0;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3D1.tag=13,((_tmp3D1.f1=_tmpAC->name,((_tmp3D1.f2=_tmpAC,_tmp3D1)))))),_tmp3D0)))));}goto _LL48;_LL5D: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAD->tag != 13)goto _LL5F;else{_tmpAE=(void*)_tmpAD->f1;}}_LL5E:
 t=Cyc_Tcpat_num_type(topt,_tmpAE);goto _LL48;_LL5F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpAF->tag != 8)goto _LL61;}_LL60:
# 295
 if(topt != 0){
void*_tmpD8=Cyc_Tcutil_compress(*topt);_LL79: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD8;if(_tmpD9->tag != 5)goto _LL7B;}_LL7A:
 t=*topt;goto tcpat_end;_LL7B:;_LL7C:
 goto _LL78;_LL78:;}{
# 300
struct Cyc_Core_Opt*_tmpDA=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3D7;struct Cyc_Absyn_PtrInfo _tmp3D6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D5;t=(void*)((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=((_tmp3D7.tag=5,((_tmp3D7.f1=((_tmp3D6.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpDA),((_tmp3D6.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp3D6.ptr_atts=(
# 305
((_tmp3D6.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,_tmpDA),(((_tmp3D6.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp3D6.ptr_atts).bounds=
# 307
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3D6.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3D6.ptr_atts).ptrloc=0,_tmp3D6.ptr_atts)))))))))),_tmp3D6)))))),_tmp3D7)))),_tmp3D5))));}
# 310
goto _LL48;};_LL61: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB0->tag != 5)goto _LL63;else{_tmpB1=_tmpB0->f1;}}_LL62: {
# 316
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpDE=0;
int elt_const=0;
if(topt != 0){
void*_tmpDF=Cyc_Tcutil_compress(*topt);void*_tmpE1;struct Cyc_Absyn_Tqual _tmpE2;_LL7E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDF;if(_tmpE0->tag != 5)goto _LL80;else{_tmpE1=(_tmpE0->f1).elt_typ;_tmpE2=(_tmpE0->f1).elt_tq;}}_LL7F:
# 322
 inner_typ=_tmpE1;
_tmpDE=& inner_typ;
elt_const=_tmpE2.real_const;
goto _LL7D;_LL80:;_LL81:
 goto _LL7D;_LL7D:;}{
# 331
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB1,_tmpDE,ptr_rgn,1));
# 337
{void*_tmpE3=Cyc_Tcutil_compress((void*)_check_null(_tmpB1->topt));struct Cyc_Absyn_Datatypedecl*_tmpE5;struct Cyc_Absyn_Datatypefield*_tmpE6;struct Cyc_List_List*_tmpE7;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE3;if(_tmpE4->tag != 4)goto _LL85;else{if((((_tmpE4->f1).field_info).KnownDatatypefield).tag != 2)goto _LL85;_tmpE5=((struct _tuple2)(((_tmpE4->f1).field_info).KnownDatatypefield).val).f1;_tmpE6=((struct _tuple2)(((_tmpE4->f1).field_info).KnownDatatypefield).val).f2;_tmpE7=(_tmpE4->f1).targs;}}_LL84:
# 341
{void*_tmpE8=Cyc_Tcutil_compress(inner_typ);_LL88: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpE9=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpE8;if(_tmpE9->tag != 4)goto _LL8A;}_LL89:
# 343
 goto DONT_PROMOTE;_LL8A:;_LL8B:
 goto _LL87;_LL87:;}{
# 347
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp3E1;struct Cyc_Absyn_Datatypedecl**_tmp3E0;struct Cyc_Absyn_DatatypeInfo _tmp3DF;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3DE;void*new_type=(void*)((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3E1.tag=3,((_tmp3E1.f1=((_tmp3DF.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp3E0=_cycalloc(sizeof(*_tmp3E0)),((_tmp3E0[0]=_tmpE5,_tmp3E0))))),((_tmp3DF.targs=_tmpE7,_tmp3DF)))),_tmp3E1)))),_tmp3DE))));
# 350
_tmpB1->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3E7;struct Cyc_Absyn_PtrInfo _tmp3E6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E5;t=(void*)((_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((_tmp3E5[0]=((_tmp3E7.tag=5,((_tmp3E7.f1=((_tmp3E6.elt_typ=new_type,((_tmp3E6.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3E6.ptr_atts=(
((_tmp3E6.ptr_atts).rgn=ptr_rgn,(((_tmp3E6.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3E6.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp3E6.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp3E6.ptr_atts).ptrloc=0,_tmp3E6.ptr_atts)))))))))),_tmp3E6)))))),_tmp3E7)))),_tmp3E5))));}
# 357
goto _LL82;};_LL85:;_LL86:
# 359
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp3ED;struct Cyc_Absyn_PtrInfo _tmp3EC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3EB;t=(void*)((_tmp3EB=_cycalloc(sizeof(*_tmp3EB)),((_tmp3EB[0]=((_tmp3ED.tag=5,((_tmp3ED.f1=((_tmp3EC.elt_typ=(void*)_check_null(_tmpB1->topt),((_tmp3EC.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp3EC.ptr_atts=(
((_tmp3EC.ptr_atts).rgn=ptr_rgn,(((_tmp3EC.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EC.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EC.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp3EC.ptr_atts).ptrloc=0,_tmp3EC.ptr_atts)))))))))),_tmp3EC)))))),_tmp3ED)))),_tmp3EB))));}_LL82:;}
# 367
goto _LL48;};}_LL63: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB2=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB2->tag != 4)goto _LL65;else{_tmpB3=(struct Cyc_List_List**)& _tmpB2->f1;_tmpB4=_tmpB2->f2;}}_LL64: {
# 370
struct Cyc_List_List*_tmpF4=*_tmpB3;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmpF5=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpF7;_LL8D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF5;if(_tmpF6->tag != 10)goto _LL8F;else{_tmpF7=_tmpF6->f1;}}_LL8E:
# 376
 topt_ts=_tmpF7;
if(_tmpB4){
# 379
int _tmpF8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF4);
int _tmpF9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF7);
if(_tmpF8 < _tmpF9){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpF9 - _tmpF8;++ i){
struct Cyc_List_List*_tmp3EE;wild_ps=((_tmp3EE=_cycalloc(sizeof(*_tmp3EE)),((_tmp3EE->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp3EE->tl=wild_ps,_tmp3EE))))));}}
*_tmpB3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpF4,wild_ps);
_tmpF4=*_tmpB3;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF4)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF7)){
const char*_tmp3F1;void*_tmp3F0;(_tmp3F0=0,Cyc_Tcutil_warn(p->loc,((_tmp3F1="unnecessary ... in tuple pattern",_tag_dyneither(_tmp3F1,sizeof(char),33))),_tag_dyneither(_tmp3F0,sizeof(void*),0)));}}}
# 391
goto _LL8C;_LL8F:;_LL90:
# 393
 goto _LL8C;_LL8C:;}else{
# 395
if(_tmpB4){
const char*_tmp3F4;void*_tmp3F3;(_tmp3F3=0,Cyc_Tcutil_terr(p->loc,((_tmp3F4="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp3F4,sizeof(char),57))),_tag_dyneither(_tmp3F3,sizeof(void*),0)));}}
for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
void**_tmpFF=0;
if(topt_ts != 0){
_tmpFF=&(*((struct _tuple16*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}
# 403
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpF4->hd,_tmpFF,rgn_pat,allow_ref_pat));{
struct _tuple16*_tmp3F7;struct Cyc_List_List*_tmp3F6;pat_ts=((_tmp3F6=_cycalloc(sizeof(*_tmp3F6)),((_tmp3F6->hd=((_tmp3F7=_cycalloc(sizeof(*_tmp3F7)),((_tmp3F7->f1=Cyc_Absyn_empty_tqual(0),((_tmp3F7->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmpF4->hd)->topt),_tmp3F7)))))),((_tmp3F6->tl=pat_ts,_tmp3F6))))));};}
# 406
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp3FA;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F9;t=(void*)((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3FA.tag=10,((_tmp3FA.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp3FA)))),_tmp3F9))));}
goto _LL48;}_LL65: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpB5->tag != 6)goto _LL67;else{if(_tmpB5->f1 == 0)goto _LL67;if((((_tmpB5->f1)->aggr_info).KnownAggr).tag != 2)goto _LL67;_tmpB6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB5->f1)->aggr_info).KnownAggr).val);_tmpB7=(struct Cyc_List_List**)&(_tmpB5->f1)->targs;_tmpB8=_tmpB5->f2;_tmpB9=(struct Cyc_List_List**)& _tmpB5->f3;_tmpBA=_tmpB5->f4;}}_LL66: {
# 410
struct Cyc_List_List*_tmp104=*_tmpB9;
const char*_tmp3FC;const char*_tmp3FB;struct _dyneither_ptr aggr_str=_tmpB6->kind == Cyc_Absyn_StructA?(_tmp3FC="struct",_tag_dyneither(_tmp3FC,sizeof(char),7)):((_tmp3FB="union",_tag_dyneither(_tmp3FB,sizeof(char),6)));
if(_tmpB6->impl == 0){
{const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_PrintArg_struct _tmp3FE;(_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp3FF[0]=& _tmp3FE,Cyc_Tcutil_terr(p->loc,((_tmp400="can't destructure an abstract %s",_tag_dyneither(_tmp400,sizeof(char),33))),_tag_dyneither(_tmp3FF,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL48;}
# 418
if(_tmpB6->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->tagged)
allow_ref_pat=0;
if(_tmpB8 != 0){
# 422
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 425
struct _RegionHandle _tmp108=_new_region("rgn");struct _RegionHandle*rgn=& _tmp108;_push_region(rgn);{
# 427
struct Cyc_List_List*_tmp109=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp10A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpB8;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp10A))->hd;
_tmp10A=_tmp10A->tl;{
void*_tmp10B=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp10C=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp10D=_tmp10C;struct Cyc_Absyn_Kind*_tmp10F;struct Cyc_Absyn_Kind*_tmp111;_LL92: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp10E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp10D;if(_tmp10E->tag != 2)goto _LL94;else{_tmp10F=_tmp10E->f2;}}_LL93:
 _tmp111=_tmp10F;goto _LL95;_LL94: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp110=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp10D;if(_tmp110->tag != 0)goto _LL96;else{_tmp111=_tmp110->f1;}}_LL95:
 k2=_tmp111;goto _LL91;_LL96:;_LL97: {
const char*_tmp403;void*_tmp402;(_tmp402=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp403="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp403,sizeof(char),53))),_tag_dyneither(_tmp402,sizeof(void*),0)));}_LL91:;}
# 443
{void*_tmp114=_tmp10B;struct Cyc_Absyn_Kind*_tmp116;struct Cyc_Core_Opt**_tmp118;struct Cyc_Absyn_Kind*_tmp119;struct Cyc_Core_Opt**_tmp11B;_LL99: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp115=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp114;if(_tmp115->tag != 0)goto _LL9B;else{_tmp116=_tmp115->f1;}}_LL9A:
# 446
 if(!Cyc_Tcutil_kind_leq(k2,_tmp116))
error=1;
goto _LL98;_LL9B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp117=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp114;if(_tmp117->tag != 2)goto _LL9D;else{_tmp118=(struct Cyc_Core_Opt**)& _tmp117->f1;_tmp119=_tmp117->f2;}}_LL9C:
 _tmp11B=_tmp118;goto _LL9E;_LL9D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp11A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp114;if(_tmp11A->tag != 1)goto _LL98;else{_tmp11B=(struct Cyc_Core_Opt**)& _tmp11A->f1;}}_LL9E:
{struct Cyc_Core_Opt*_tmp404;*_tmp11B=((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404->v=_tmp10C,_tmp404))));}goto _LL98;_LL98:;}
# 452
if(error){
const char*_tmp40A;void*_tmp409[3];struct Cyc_String_pa_PrintArg_struct _tmp408;struct Cyc_String_pa_PrintArg_struct _tmp407;struct Cyc_String_pa_PrintArg_struct _tmp406;(_tmp406.tag=0,((_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 456
Cyc_Absynpp_kind2string(k2)),((_tmp407.tag=0,((_tmp407.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp10B)),((_tmp408.tag=0,((_tmp408.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp409[0]=& _tmp408,((_tmp409[1]=& _tmp407,((_tmp409[2]=& _tmp406,Cyc_Tcutil_terr(p->loc,((_tmp40A="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp40A,sizeof(char),60))),_tag_dyneither(_tmp409,sizeof(void*),3)))))))))))))))))));}{
# 458
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp40D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp40C;void*vartype=(void*)((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=((_tmp40D.tag=2,((_tmp40D.f1=tv,_tmp40D)))),_tmp40C))));
{struct Cyc_List_List*_tmp40E;_tmp109=((_tmp40E=_region_malloc(rgn,sizeof(*_tmp40E)),((_tmp40E->hd=vartype,((_tmp40E->tl=_tmp109,_tmp40E))))));}
# 462
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp411;struct Cyc_List_List*_tmp410;outlives_constraints=(
(_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=((_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411->f1=Cyc_Absyn_empty_effect,((_tmp411->f2=vartype,_tmp411)))))),((_tmp410->tl=outlives_constraints,_tmp410))))));}else{
# 467
const char*_tmp414;void*_tmp413;(_tmp413=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp414="opened existential had unique or top region kind",_tag_dyneither(_tmp414,sizeof(char),49))),_tag_dyneither(_tmp413,sizeof(void*),0)));}}};};}}
# 471
_tmp109=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp109);{
# 473
struct _RegionHandle _tmp129=_new_region("r2");struct _RegionHandle*r2=& _tmp129;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpB8);
# 476
struct Cyc_List_List*_tmp12A=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple11 _tmp415;struct _tuple11 _tmp12B=(_tmp415.f1=_tmp12A,((_tmp415.f2=rgn,_tmp415)));
struct Cyc_List_List*_tmp12C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp12B,_tmpB6->tvs);
struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->exist_vars,_tmp109);
struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp12C);
struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp12D);
struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp12C,_tmp12D);
# 484
if(_tmpB8 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple13*_tmp416;res.tvars_and_bounds_opt=((_tmp416=_cycalloc(sizeof(*_tmp416)),((_tmp416->f1=0,((_tmp416->f2=0,_tmp416))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpB8);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp132=0;
{struct Cyc_List_List*_tmp133=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->rgn_po;for(0;_tmp133 != 0;_tmp133=_tmp133->tl){
struct _tuple0*_tmp419;struct Cyc_List_List*_tmp418;_tmp132=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp130,(*((struct _tuple0*)_tmp133->hd)).f1),((_tmp419->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp130,(*((struct _tuple0*)_tmp133->hd)).f2),_tmp419)))))),((_tmp418->tl=_tmp132,_tmp418))))));}}
# 496
_tmp132=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp132);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp132);};}
# 501
*_tmpB7=_tmp12E;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp423;struct Cyc_Absyn_Aggrdecl**_tmp422;struct Cyc_Absyn_AggrInfo _tmp421;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp420;t=(void*)((_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=((_tmp423.tag=11,((_tmp423.f1=((_tmp421.aggr_info=Cyc_Absyn_KnownAggr(((_tmp422=_cycalloc(sizeof(*_tmp422)),((_tmp422[0]=_tmpB6,_tmp422))))),((_tmp421.targs=*_tmpB7,_tmp421)))),_tmp423)))),_tmp420))));}
if(_tmpBA  && _tmpB6->kind == Cyc_Absyn_UnionA){
const char*_tmp426;void*_tmp425;(_tmp425=0,Cyc_Tcutil_warn(p->loc,((_tmp426="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp426,sizeof(char),43))),_tag_dyneither(_tmp425,sizeof(void*),0)));}else{
if(_tmpBA){
# 507
int _tmp13C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp104);
int _tmp13D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
if(_tmp13C < _tmp13D){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp13D - _tmp13C;++ i){
struct _tuple15*_tmp429;struct Cyc_List_List*_tmp428;wild_dps=((_tmp428=_cycalloc(sizeof(*_tmp428)),((_tmp428->hd=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->f1=0,((_tmp429->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp429)))))),((_tmp428->tl=wild_dps,_tmp428))))));}}
*_tmpB9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp104,wild_dps);
_tmp104=*_tmpB9;}else{
if(_tmp13C == _tmp13D){
const char*_tmp42C;void*_tmp42B;(_tmp42B=0,Cyc_Tcutil_warn(p->loc,((_tmp42C="unnecessary ... in struct pattern",_tag_dyneither(_tmp42C,sizeof(char),34))),_tag_dyneither(_tmp42B,sizeof(void*),0)));}}}}{
# 518
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp104,_tmpB6->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB6->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp143;struct Cyc_Absyn_Pat*_tmp144;struct _tuple17*_tmp142=(struct _tuple17*)fields->hd;_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;{
void*_tmp145=Cyc_Tcutil_rsubstitute(rgn,_tmp130,_tmp143->type);
# 524
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp144,& _tmp145,rgn_pat,allow_ref_pat));
# 528
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp144->topt),_tmp145)){
const char*_tmp433;void*_tmp432[4];struct Cyc_String_pa_PrintArg_struct _tmp431;struct Cyc_String_pa_PrintArg_struct _tmp430;struct Cyc_String_pa_PrintArg_struct _tmp42F;struct Cyc_String_pa_PrintArg_struct _tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 531
Cyc_Absynpp_typ2string((void*)_check_null(_tmp144->topt))),((_tmp42F.tag=0,((_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 530
Cyc_Absynpp_typ2string(_tmp145)),((_tmp430.tag=0,((_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp431.tag=0,((_tmp431.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp143->name),((_tmp432[0]=& _tmp431,((_tmp432[1]=& _tmp430,((_tmp432[2]=& _tmp42F,((_tmp432[3]=& _tmp42E,Cyc_Tcutil_terr(p->loc,((_tmp433="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp433,sizeof(char),45))),_tag_dyneither(_tmp432,sizeof(void*),4)))))))))))))))))))))))));}};}};}
# 474
;_pop_region(r2);};}
# 535
_npop_handler(0);goto _LL48;
# 425
;_pop_region(rgn);};}_LL67: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBB=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpBB->tag != 7)goto _LL69;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;_tmpBE=(struct Cyc_List_List**)& _tmpBB->f3;_tmpBF=_tmpBB->f4;}}_LL68: {
# 538
struct Cyc_List_List*_tmp14F=*_tmpBE;
struct _RegionHandle _tmp150=_new_region("rgn");struct _RegionHandle*rgn=& _tmp150;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpBD->typs;
# 542
struct Cyc_List_List*_tmp151=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple11 _tmp434;struct _tuple11 _tmp152=(_tmp434.f1=_tmp151,((_tmp434.f2=rgn,_tmp434)));
struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple12*(*f)(struct _tuple11*,struct Cyc_Absyn_Tvar*),struct _tuple11*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp152,_tmpBC->tvs);
struct Cyc_List_List*_tmp154=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple12*))Cyc_Core_snd,_tmp153);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp43A;struct Cyc_Absyn_DatatypeFieldInfo _tmp439;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp438;t=(void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp43A.tag=4,((_tmp43A.f1=((_tmp439.field_info=Cyc_Absyn_KnownDatatypefield(_tmpBC,_tmpBD),((_tmp439.targs=_tmp154,_tmp439)))),_tmp43A)))),_tmp438))));}
# 548
if(_tmpBF){
# 550
int _tmp158=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14F);
int _tmp159=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp158 < _tmp159){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp159 - _tmp158;++ i){
struct Cyc_List_List*_tmp43B;wild_ps=((_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp43B->tl=wild_ps,_tmp43B))))));}}
*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp14F,wild_ps);
_tmp14F=*_tmpBE;}else{
if(_tmp158 == _tmp159){
const char*_tmp43F;void*_tmp43E[1];struct Cyc_String_pa_PrintArg_struct _tmp43D;(_tmp43D.tag=0,((_tmp43D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBC->name)),((_tmp43E[0]=& _tmp43D,Cyc_Tcutil_warn(p->loc,((_tmp43F="unnecessary ... in datatype field %s",_tag_dyneither(_tmp43F,sizeof(char),37))),_tag_dyneither(_tmp43E,sizeof(void*),1)))))));}}}
# 562
for(0;_tmp14F != 0  && tqts != 0;(_tmp14F=_tmp14F->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp15E=(struct Cyc_Absyn_Pat*)_tmp14F->hd;
# 566
void*_tmp15F=Cyc_Tcutil_rsubstitute(rgn,_tmp153,(*((struct _tuple16*)tqts->hd)).f2);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp15E,& _tmp15F,rgn_pat,allow_ref_pat));
# 571
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp15E->topt),_tmp15F)){
const char*_tmp445;void*_tmp444[3];struct Cyc_String_pa_PrintArg_struct _tmp443;struct Cyc_String_pa_PrintArg_struct _tmp442;struct Cyc_String_pa_PrintArg_struct _tmp441;(_tmp441.tag=0,((_tmp441.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 574
Cyc_Absynpp_typ2string((void*)_check_null(_tmp15E->topt))),((_tmp442.tag=0,((_tmp442.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 573
Cyc_Absynpp_typ2string(_tmp15F)),((_tmp443.tag=0,((_tmp443.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp444[0]=& _tmp443,((_tmp444[1]=& _tmp442,((_tmp444[2]=& _tmp441,Cyc_Tcutil_terr(_tmp15E->loc,((_tmp445="%s expects argument type %s, not %s",_tag_dyneither(_tmp445,sizeof(char),36))),_tag_dyneither(_tmp444,sizeof(void*),3)))))))))))))))))));}}
# 576
if(_tmp14F != 0){
const char*_tmp449;void*_tmp448[1];struct Cyc_String_pa_PrintArg_struct _tmp447;(_tmp447.tag=0,((_tmp447.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp448[0]=& _tmp447,Cyc_Tcutil_terr(p->loc,((_tmp449="too many arguments for datatype constructor %s",_tag_dyneither(_tmp449,sizeof(char),47))),_tag_dyneither(_tmp448,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp44D;void*_tmp44C[1];struct Cyc_String_pa_PrintArg_struct _tmp44B;(_tmp44B.tag=0,((_tmp44B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBD->name)),((_tmp44C[0]=& _tmp44B,Cyc_Tcutil_terr(p->loc,((_tmp44D="too few arguments for datatype constructor %s",_tag_dyneither(_tmp44D,sizeof(char),46))),_tag_dyneither(_tmp44C,sizeof(void*),1)))))));}}
# 583
_npop_handler(0);goto _LL48;
# 539
;_pop_region(rgn);}_LL69: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC0->tag != 6)goto _LL6B;else{if(_tmpC0->f1 != 0)goto _LL6B;}}_LL6A:
# 585
 goto _LL6C;_LL6B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC1->tag != 6)goto _LL6D;else{if(_tmpC1->f1 == 0)goto _LL6D;if((((_tmpC1->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6D;}}_LL6C:
 goto _LL6E;_LL6D: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC2->tag != 14)goto _LL6F;}_LL6E:
 goto _LL70;_LL6F: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC3=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC3->tag != 16)goto _LL71;}_LL70:
 goto _LL72;_LL71: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9A;if(_tmpC4->tag != 15)goto _LL48;}_LL72:
# 590
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL48;_LL48:;}
# 592
tcpat_end:
 p->topt=t;
return res;};}
# 597
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){
struct Cyc_Tcpat_TcPatResult _tmp16D=Cyc_Tcpat_tcPatRec(te,p,topt,0,0);
# 600
struct _RegionHandle _tmp16E=_new_region("r");struct _RegionHandle*r=& _tmp16E;_push_region(r);{
const char*_tmp44E;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp16D.patvars),p->loc,(
(_tmp44E="pattern contains a repeated variable",_tag_dyneither(_tmp44E,sizeof(char),37))));}{
# 604
struct Cyc_Tcpat_TcPatResult _tmp170=_tmp16D;_npop_handler(0);return _tmp170;};
# 600
;_pop_region(r);}
# 607
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
void*_tmp171=p->r;struct Cyc_Absyn_Pat*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp177;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Pat*_tmp17B;struct Cyc_Absyn_Pat*_tmp17D;_LLA0: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp172=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp172->tag != 5)goto _LLA2;else{_tmp173=_tmp172->f1;}}_LLA1:
# 610
 Cyc_Tcpat_check_pat_regions(te,_tmp173);{
# 612
struct _RegionHandle _tmp17E=_new_region("temp");struct _RegionHandle*temp=& _tmp17E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(_tmp173->topt))){
{const char*_tmp451;void*_tmp450;(_tmp450=0,Cyc_Tcutil_terr(p->loc,((_tmp451="Pattern dereferences to a non-aliased pointer; use swap",_tag_dyneither(_tmp451,sizeof(char),56))),_tag_dyneither(_tmp450,sizeof(void*),0)));}
_npop_handler(0);return;}
# 619
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(p->topt))){
{const char*_tmp454;void*_tmp453;(_tmp453=0,Cyc_Tcutil_terr(p->loc,((_tmp454="Pattern dereferences a non-aliased pointer; use swap",_tag_dyneither(_tmp454,sizeof(char),53))),_tag_dyneither(_tmp453,sizeof(void*),0)));}
_npop_handler(0);return;}{
# 624
void*_tmp183=(void*)_check_null(p->topt);void*_tmp185;_LLAF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp183;if(_tmp184->tag != 5)goto _LLB1;else{_tmp185=((_tmp184->f1).ptr_atts).rgn;}}_LLB0:
# 626
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp185);
_npop_handler(0);return;_LLB1:;_LLB2: {
const char*_tmp457;void*_tmp456;(_tmp456=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp457="check_pat_regions: bad pointer type",_tag_dyneither(_tmp457,sizeof(char),36))),_tag_dyneither(_tmp456,sizeof(void*),0)));}_LLAE:;};
# 612
;_pop_region(temp);};_LLA2: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp174=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp174->tag != 6)goto _LLA4;else{_tmp175=_tmp174->f3;}}_LLA3:
# 631
 for(0;_tmp175 != 0;_tmp175=_tmp175->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct _tuple15*)_tmp175->hd)).f2);}
return;_LLA4: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp176=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp176->tag != 7)goto _LLA6;else{_tmp177=_tmp176->f3;}}_LLA5:
# 634
 _tmp179=_tmp177;goto _LLA7;_LLA6: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp178=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp178->tag != 4)goto _LLA8;else{_tmp179=_tmp178->f1;}}_LLA7:
# 636
 for(0;_tmp179 != 0;_tmp179=_tmp179->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)_tmp179->hd);}
return;_LLA8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp17A=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp17A->tag != 2)goto _LLAA;else{_tmp17B=_tmp17A->f2;}}_LLA9:
 _tmp17D=_tmp17B;goto _LLAB;_LLAA: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp17C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp171;if(_tmp17C->tag != 1)goto _LLAC;else{_tmp17D=_tmp17C->f2;}}_LLAB:
 Cyc_Tcpat_check_pat_regions(te,_tmp17D);
return;_LLAC:;_LLAD:
 return;_LL9F:;}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 682 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp458;return((_tmp458.Name_v).val=s,(((_tmp458.Name_v).tag=1,_tmp458)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp459;return((_tmp459.Int_v).val=i,(((_tmp459.Int_v).tag=2,_tmp459)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 692
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 697
typedef void*Cyc_Tcpat_simple_pat_t;
# 700
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp18A=c1->name;struct _dyneither_ptr _tmp18B;int _tmp18C;_LLB4: if((_tmp18A.Name_v).tag != 1)goto _LLB6;_tmp18B=(struct _dyneither_ptr)(_tmp18A.Name_v).val;_LLB5: {
# 703
union Cyc_Tcpat_Name_value _tmp18D=c2->name;struct _dyneither_ptr _tmp18E;_LLB9: if((_tmp18D.Name_v).tag != 1)goto _LLBB;_tmp18E=(struct _dyneither_ptr)(_tmp18D.Name_v).val;_LLBA:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp18B,(struct _dyneither_ptr)_tmp18E);_LLBB: if((_tmp18D.Int_v).tag != 2)goto _LLB8;_LLBC:
 return - 1;_LLB8:;}_LLB6: if((_tmp18A.Int_v).tag != 2)goto _LLB3;_tmp18C=(int)(_tmp18A.Int_v).val;_LLB7: {
# 708
union Cyc_Tcpat_Name_value _tmp18F=c2->name;int _tmp190;_LLBE: if((_tmp18F.Name_v).tag != 1)goto _LLC0;_LLBF:
 return 1;_LLC0: if((_tmp18F.Int_v).tag != 2)goto _LLBD;_tmp190=(int)(_tmp18F.Int_v).val;_LLC1:
 return _tmp18C - _tmp190;_LLBD:;}_LLB3:;}
# 716
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 720
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 724
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp45C;struct Cyc_Tcpat_Con_s*_tmp45B;return(_tmp45B=_region_malloc(r,sizeof(*_tmp45B)),((_tmp45B->name=Cyc_Tcpat_Name_v(((_tmp45C="NULL",_tag_dyneither(_tmp45C,sizeof(char),5)))),((_tmp45B->arity=0,((_tmp45B->span=& Cyc_Tcpat_two_opt,((_tmp45B->orig_pat=p,_tmp45B)))))))));}
# 727
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp45F;struct Cyc_Tcpat_Con_s*_tmp45E;return(_tmp45E=_region_malloc(r,sizeof(*_tmp45E)),((_tmp45E->name=Cyc_Tcpat_Name_v(((_tmp45F="&",_tag_dyneither(_tmp45F,sizeof(char),2)))),((_tmp45E->arity=1,((_tmp45E->span=& Cyc_Tcpat_two_opt,((_tmp45E->orig_pat=p,_tmp45E)))))))));}
# 730
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp462;struct Cyc_Tcpat_Con_s*_tmp461;return(_tmp461=_region_malloc(r,sizeof(*_tmp461)),((_tmp461->name=Cyc_Tcpat_Name_v(((_tmp462="&",_tag_dyneither(_tmp462,sizeof(char),2)))),((_tmp461->arity=1,((_tmp461->span=& Cyc_Tcpat_one_opt,((_tmp461->orig_pat=p,_tmp461)))))))));}
# 733
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp463;return(_tmp463=_region_malloc(r,sizeof(*_tmp463)),((_tmp463->name=Cyc_Tcpat_Int_v(i),((_tmp463->arity=0,((_tmp463->span=0,((_tmp463->orig_pat=p,_tmp463)))))))));}
# 736
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp464;return(_tmp464=_region_malloc(r,sizeof(*_tmp464)),((_tmp464->name=Cyc_Tcpat_Name_v(f),((_tmp464->arity=0,((_tmp464->span=0,((_tmp464->orig_pat=p,_tmp464)))))))));}
# 739
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp465;return(_tmp465=_region_malloc(r,sizeof(*_tmp465)),((_tmp465->name=Cyc_Tcpat_Int_v((int)c),((_tmp465->arity=0,((_tmp465->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp465->orig_pat=p,_tmp465)))))))));}
# 742
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp468;struct Cyc_Tcpat_Con_s*_tmp467;return(_tmp467=_region_malloc(r,sizeof(*_tmp467)),((_tmp467->name=Cyc_Tcpat_Name_v(((_tmp468="$",_tag_dyneither(_tmp468,sizeof(char),2)))),((_tmp467->arity=i,((_tmp467->span=& Cyc_Tcpat_one_opt,((_tmp467->orig_pat=p,_tmp467)))))))));}
# 747
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp46B;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp46A;return(void*)((_tmp46A=_region_malloc(r,sizeof(*_tmp46A)),((_tmp46A[0]=((_tmp46B.tag=1,((_tmp46B.f1=Cyc_Tcpat_null_con(r,p),((_tmp46B.f2=0,_tmp46B)))))),_tmp46A))));}
# 750
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp46E;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp46D;return(void*)((_tmp46D=_region_malloc(r,sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp46E.tag=1,((_tmp46E.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp46E.f2=0,_tmp46E)))))),_tmp46D))));}
# 753
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp471;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp470;return(void*)((_tmp470=_region_malloc(r,sizeof(*_tmp470)),((_tmp470[0]=((_tmp471.tag=1,((_tmp471.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp471.f2=0,_tmp471)))))),_tmp470))));}
# 756
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp474;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp473;return(void*)((_tmp473=_region_malloc(r,sizeof(*_tmp473)),((_tmp473[0]=((_tmp474.tag=1,((_tmp474.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp474.f2=0,_tmp474)))))),_tmp473))));}
# 759
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp47A;struct Cyc_List_List*_tmp479;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp478;return(void*)((_tmp478=_region_malloc(r,sizeof(*_tmp478)),((_tmp478[0]=((_tmp47A.tag=1,((_tmp47A.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp47A.f2=((_tmp479=_region_malloc(r,sizeof(*_tmp479)),((_tmp479->hd=p,((_tmp479->tl=0,_tmp479)))))),_tmp47A)))))),_tmp478))));}
# 762
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp480;struct Cyc_List_List*_tmp47F;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp47E;return(void*)((_tmp47E=_region_malloc(r,sizeof(*_tmp47E)),((_tmp47E[0]=((_tmp480.tag=1,((_tmp480.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp480.f2=((_tmp47F=_region_malloc(r,sizeof(*_tmp47F)),((_tmp47F->hd=p,((_tmp47F->tl=0,_tmp47F)))))),_tmp480)))))),_tmp47E))));}
# 765
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp483;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp482;return(void*)((_tmp482=_region_malloc(r,sizeof(*_tmp482)),((_tmp482[0]=((_tmp483.tag=1,((_tmp483.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp483.f2=ss,_tmp483)))))),_tmp482))));}
# 768
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 770
struct Cyc_Tcpat_Con_s*_tmp484;struct Cyc_Tcpat_Con_s*c=(_tmp484=_region_malloc(r,sizeof(*_tmp484)),((_tmp484->name=Cyc_Tcpat_Name_v(con_name),((_tmp484->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp484->span=span,((_tmp484->orig_pat=p,_tmp484)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp487;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp486;return(void*)((_tmp486=_region_malloc(r,sizeof(*_tmp486)),((_tmp486[0]=((_tmp487.tag=1,((_tmp487.f1=c,((_tmp487.f2=ps,_tmp487)))))),_tmp486))));}
# 775
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1AF=p->r;enum Cyc_Absyn_Sign _tmp1B4;int _tmp1B5;char _tmp1B7;struct _dyneither_ptr _tmp1B9;struct Cyc_Absyn_Pat*_tmp1BB;struct Cyc_Absyn_Pat*_tmp1BD;struct Cyc_Absyn_Pat*_tmp1BF;struct Cyc_Absyn_Datatypedecl*_tmp1C1;struct Cyc_Absyn_Datatypefield*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C5;struct Cyc_Absyn_Aggrdecl*_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Enumdecl*_tmp1CA;struct Cyc_Absyn_Enumfield*_tmp1CB;void*_tmp1CD;struct Cyc_Absyn_Enumfield*_tmp1CE;_LLC3: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1B0=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B0->tag != 0)goto _LLC5;}_LLC4:
 goto _LLC6;_LLC5: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp1B1=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B1->tag != 3)goto _LLC7;}_LLC6:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp48A;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp489;s=(void*)((_tmp489=_region_malloc(r,sizeof(*_tmp489)),((_tmp489[0]=((_tmp48A.tag=0,_tmp48A)),_tmp489))));}goto _LLC2;_LLC7: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1B2=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B2->tag != 8)goto _LLC9;}_LLC8:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLC2;_LLC9: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1B3=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B3->tag != 9)goto _LLCB;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B3->f2;}}_LLCA:
 s=Cyc_Tcpat_int_pat(r,_tmp1B5,p);goto _LLC2;_LLCB: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1B6=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B6->tag != 10)goto _LLCD;else{_tmp1B7=_tmp1B6->f1;}}_LLCC:
 s=Cyc_Tcpat_char_pat(r,_tmp1B7,p);goto _LLC2;_LLCD: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1B8=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1B8->tag != 11)goto _LLCF;else{_tmp1B9=_tmp1B8->f1;}}_LLCE:
 s=Cyc_Tcpat_float_pat(r,_tmp1B9,p);goto _LLC2;_LLCF: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1BA=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1BA->tag != 1)goto _LLD1;else{_tmp1BB=_tmp1BA->f2;}}_LLD0:
 s=Cyc_Tcpat_compile_pat(r,_tmp1BB);goto _LLC2;_LLD1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1BC=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1BC->tag != 2)goto _LLD3;else{_tmp1BD=_tmp1BC->f2;}}_LLD2:
 s=Cyc_Tcpat_compile_pat(r,_tmp1BD);goto _LLC2;_LLD3: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1BE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1BE->tag != 5)goto _LLD5;else{_tmp1BF=_tmp1BE->f1;}}_LLD4:
# 787
{void*_tmp1D5=Cyc_Tcutil_compress((void*)_check_null(p->topt));union Cyc_Absyn_Constraint*_tmp1D7;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D5;if(_tmp1D6->tag != 5)goto _LLEA;else{_tmp1D7=((_tmp1D6->f1).ptr_atts).nullable;}}_LLE9: {
# 789
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1D8=_tmp1D7;union Cyc_Absyn_Constraint*_tmp1D9;int _tmp1DA;_LLED: if((_tmp1D8->Forward_constr).tag != 2)goto _LLEF;_tmp1D9=(union Cyc_Absyn_Constraint*)(_tmp1D8->Forward_constr).val;_LLEE:
# 794
*_tmp1D7=*_tmp1D9;
continue;_LLEF: if((_tmp1D8->No_constr).tag != 3)goto _LLF1;_LLF0:
# 797
{struct _union_Constraint_Eq_constr*_tmp48B;(_tmp48B=& _tmp1D7->Eq_constr,((_tmp48B->tag=1,_tmp48B->val=0)));}
is_nullable=0;
still_working=0;
goto _LLEC;_LLF1: if((_tmp1D8->Eq_constr).tag != 1)goto _LLEC;_tmp1DA=(int)(_tmp1D8->Eq_constr).val;_LLF2:
# 802
 is_nullable=_tmp1DA;
still_working=0;
goto _LLEC;_LLEC:;}{
# 807
void*ss=Cyc_Tcpat_compile_pat(r,_tmp1BF);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 811
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLE7;};}_LLEA:;_LLEB: {
const char*_tmp48E;void*_tmp48D;(_tmp48D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp48E="expecting pointertype for pattern!",_tag_dyneither(_tmp48E,sizeof(char),35))),_tag_dyneither(_tmp48D,sizeof(void*),0)));}_LLE7:;}
# 815
goto _LLC2;_LLD5: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1C0=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1C0->tag != 7)goto _LLD7;else{_tmp1C1=_tmp1C0->f1;_tmp1C2=_tmp1C0->f2;_tmp1C3=_tmp1C0->f3;}}_LLD6: {
# 817
int*span;
{void*_tmp1DE=Cyc_Tcutil_compress((void*)_check_null(p->topt));_LLF4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1DF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1DE;if(_tmp1DF->tag != 3)goto _LLF6;}_LLF5:
# 820
 if(_tmp1C1->is_extensible)
span=0;else{
# 823
int*_tmp48F;span=((_tmp48F=_region_malloc(r,sizeof(*_tmp48F)),((_tmp48F[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->fields))->v),_tmp48F))));}
goto _LLF3;_LLF6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1DE;if(_tmp1E0->tag != 4)goto _LLF8;}_LLF7:
 span=& Cyc_Tcpat_one_opt;goto _LLF3;_LLF8:;_LLF9:
{const char*_tmp492;void*_tmp491;span=((_tmp491=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp492="void datatype pattern has bad type",_tag_dyneither(_tmp492,sizeof(char),35))),_tag_dyneither(_tmp491,sizeof(void*),0))));}goto _LLF3;_LLF3:;}
# 828
s=Cyc_Tcpat_con_pat(r,*(*_tmp1C2->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C3),p);
goto _LLC2;}_LLD7: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1C4->tag != 4)goto _LLD9;else{_tmp1C5=_tmp1C4->f1;}}_LLD8:
# 832
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1C5),p);goto _LLC2;_LLD9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1C6=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1C6->tag != 6)goto _LLDB;else{if(_tmp1C6->f1 == 0)goto _LLDB;if((((_tmp1C6->f1)->aggr_info).KnownAggr).tag != 2)goto _LLDB;_tmp1C7=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp1C6->f1)->aggr_info).KnownAggr).val);_tmp1C8=_tmp1C6->f3;}}_LLDA:
# 837
 if(_tmp1C7->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C7->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 841
const char*_tmp493;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp493="",_tag_dyneither(_tmp493,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp1C8;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Pat*_tmp1E6;struct _tuple15*_tmp1E4=(struct _tuple15*)dlps0->hd;_tmp1E5=_tmp1E4->f1;_tmp1E6=_tmp1E4->f2;{
struct Cyc_List_List*_tmp1E7=_tmp1E5;struct _dyneither_ptr*_tmp1E9;_LLFB: if(_tmp1E7 == 0)goto _LLFD;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1E8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp1E7->hd);if(_tmp1E8->tag != 1)goto _LLFD;else{_tmp1E9=_tmp1E8->f1;}}if(_tmp1E7->tl != 0)goto _LLFD;_LLFC:
# 846
 if(Cyc_strptrcmp(_tmp1E9,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp494;ps=((_tmp494=_region_malloc(r,sizeof(*_tmp494)),((_tmp494->hd=Cyc_Tcpat_compile_pat(r,_tmp1E6),((_tmp494->tl=ps,_tmp494))))));}
found=1;}
# 850
goto _LLFA;_LLFD:;_LLFE: {
const char*_tmp497;void*_tmp496;(_tmp496=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp497="bad designator(s)",_tag_dyneither(_tmp497,sizeof(char),18))),_tag_dyneither(_tmp496,sizeof(void*),0)));}_LLFA:;};}}
# 854
if(!found){
const char*_tmp49A;void*_tmp499;(_tmp499=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp49A="bad designator",_tag_dyneither(_tmp49A,sizeof(char),15))),_tag_dyneither(_tmp499,sizeof(void*),0)));}}}
# 857
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 860
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C7->impl))->tagged){
const char*_tmp49D;void*_tmp49C;(_tmp49C=0,Cyc_Tcutil_terr(p->loc,((_tmp49D="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp49D,sizeof(char),47))),_tag_dyneither(_tmp49C,sizeof(void*),0)));}{
int*_tmp49E;int*span=(_tmp49E=_region_malloc(r,sizeof(*_tmp49E)),((_tmp49E[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1C7->impl))->fields),_tmp49E)));
struct Cyc_List_List*_tmp1F2=_tmp1C8;struct _dyneither_ptr*_tmp1F4;struct Cyc_Absyn_Pat*_tmp1F5;_LL100: if(_tmp1F2 == 0)goto _LL102;if(((struct _tuple15*)_tmp1F2->hd)->f1 == 0)goto _LL102;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1F3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp1F2->hd)->f1)->hd);if(_tmp1F3->tag != 1)goto _LL102;else{_tmp1F4=_tmp1F3->f1;}}if((((struct _tuple15*)_tmp1F2->hd)->f1)->tl != 0)goto _LL102;_tmp1F5=((struct _tuple15*)_tmp1F2->hd)->f2;if(_tmp1F2->tl != 0)goto _LL102;_LL101:
# 865
{struct Cyc_List_List*_tmp49F;s=Cyc_Tcpat_con_pat(r,*_tmp1F4,span,((_tmp49F=_region_malloc(r,sizeof(*_tmp49F)),((_tmp49F->hd=Cyc_Tcpat_compile_pat(r,_tmp1F5),((_tmp49F->tl=0,_tmp49F)))))),p);}
goto _LLFF;_LL102:;_LL103: {
const char*_tmp4A2;void*_tmp4A1;(_tmp4A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A2="bad union pattern",_tag_dyneither(_tmp4A2,sizeof(char),18))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}_LLFF:;};}
# 870
goto _LLC2;_LLDB: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp1C9=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1C9->tag != 12)goto _LLDD;else{_tmp1CA=_tmp1C9->f1;_tmp1CB=_tmp1C9->f2;}}_LLDC: {
# 875
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CA->fields))->v);
{int*_tmp4A3;s=Cyc_Tcpat_con_pat(r,*(*_tmp1CB->name).f2,((_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3[0]=span,_tmp4A3)))),0,p);}
goto _LLC2;}_LLDD: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp1CC=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1CC->tag != 13)goto _LLDF;else{_tmp1CD=(void*)_tmp1CC->f1;_tmp1CE=_tmp1CC->f2;}}_LLDE: {
# 882
struct Cyc_List_List*fields;
{void*_tmp1FB=Cyc_Tcutil_compress(_tmp1CD);struct Cyc_List_List*_tmp1FD;_LL105: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1FC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FB;if(_tmp1FC->tag != 14)goto _LL107;else{_tmp1FD=_tmp1FC->f1;}}_LL106:
 fields=_tmp1FD;goto _LL104;_LL107:;_LL108: {
const char*_tmp4A6;void*_tmp4A5;(_tmp4A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A6="bad type in AnonEnum_p",_tag_dyneither(_tmp4A6,sizeof(char),23))),_tag_dyneither(_tmp4A5,sizeof(void*),0)));}_LL104:;}{
# 889
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp4A7;s=Cyc_Tcpat_con_pat(r,*(*_tmp1CE->name).f2,((_tmp4A7=_region_malloc(r,sizeof(*_tmp4A7)),((_tmp4A7[0]=span,_tmp4A7)))),0,p);}
goto _LLC2;};}_LLDF: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1CF=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1CF->tag != 14)goto _LLE1;}_LLE0:
# 893
 goto _LLE2;_LLE1: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1D0=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1D0->tag != 15)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1D1->tag != 6)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1D2=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1AF;if(_tmp1D2->tag != 16)goto _LLC2;}_LLE6: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp4AA;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp4A9;s=(void*)((_tmp4A9=_region_malloc(r,sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AA.tag=0,_tmp4AA)),_tmp4A9))));}_LLC2:;}
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
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 950
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp4AD;void*_tmp4AC;(_tmp4AC=0,Cyc_fprintf(Cyc_stderr,((_tmp4AD=" ",_tag_dyneither(_tmp4AD,sizeof(char),2))),_tag_dyneither(_tmp4AC,sizeof(void*),0)));}}
# 956
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp205=c->name;
union Cyc_Tcpat_Name_value _tmp206=_tmp205;struct _dyneither_ptr _tmp207;int _tmp208;_LL10A: if((_tmp206.Name_v).tag != 1)goto _LL10C;_tmp207=(struct _dyneither_ptr)(_tmp206.Name_v).val;_LL10B:
{const char*_tmp4B1;void*_tmp4B0[1];struct Cyc_String_pa_PrintArg_struct _tmp4AF;(_tmp4AF.tag=0,((_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp207),((_tmp4B0[0]=& _tmp4AF,Cyc_fprintf(Cyc_stderr,((_tmp4B1="%s",_tag_dyneither(_tmp4B1,sizeof(char),3))),_tag_dyneither(_tmp4B0,sizeof(void*),1)))))));}goto _LL109;_LL10C: if((_tmp206.Int_v).tag != 2)goto _LL109;_tmp208=(int)(_tmp206.Int_v).val;_LL10D:
{const char*_tmp4B5;void*_tmp4B4[1];struct Cyc_Int_pa_PrintArg_struct _tmp4B3;(_tmp4B3.tag=1,((_tmp4B3.f1=(unsigned long)_tmp208,((_tmp4B4[0]=& _tmp4B3,Cyc_fprintf(Cyc_stderr,((_tmp4B5="%d",_tag_dyneither(_tmp4B5,sizeof(char),3))),_tag_dyneither(_tmp4B4,sizeof(void*),1)))))));}goto _LL109;_LL109:;}
# 964
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp20F=d;void*_tmp211;struct Cyc_List_List*_tmp214;struct Cyc_Tcpat_Con_s*_tmp215;void*_tmp216;void*_tmp217;_LL10F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp210=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp20F;if(_tmp210->tag != 1)goto _LL111;else{_tmp211=(void*)_tmp210->f1;}}_LL110:
# 967
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4B8;void*_tmp4B7;(_tmp4B7=0,Cyc_fprintf(Cyc_stderr,((_tmp4B8="Success(",_tag_dyneither(_tmp4B8,sizeof(char),9))),_tag_dyneither(_tmp4B7,sizeof(void*),0)));}print_rhs(_tmp211);{const char*_tmp4BB;void*_tmp4BA;(_tmp4BA=0,Cyc_fprintf(Cyc_stderr,((_tmp4BB=")\n",_tag_dyneither(_tmp4BB,sizeof(char),3))),_tag_dyneither(_tmp4BA,sizeof(void*),0)));}
goto _LL10E;_LL111: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp212=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp20F;if(_tmp212->tag != 0)goto _LL113;}_LL112:
{const char*_tmp4BE;void*_tmp4BD;(_tmp4BD=0,Cyc_fprintf(Cyc_stderr,((_tmp4BE="Failure\n",_tag_dyneither(_tmp4BE,sizeof(char),9))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}goto _LL10E;_LL113: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp213=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp20F;if(_tmp213->tag != 2)goto _LL10E;else{_tmp214=_tmp213->f1;_tmp215=_tmp213->f2;_tmp216=(void*)_tmp213->f3;_tmp217=(void*)_tmp213->f4;}}_LL114:
# 972
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp4C1;void*_tmp4C0;(_tmp4C0=0,Cyc_fprintf(Cyc_stderr,((_tmp4C1="Access[",_tag_dyneither(_tmp4C1,sizeof(char),8))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}
for(0;_tmp214 != 0;_tmp214=_tmp214->tl){
{const char*_tmp4C5;void*_tmp4C4[1];struct Cyc_Int_pa_PrintArg_struct _tmp4C3;(_tmp4C3.tag=1,((_tmp4C3.f1=(unsigned long)((int)_tmp214->hd),((_tmp4C4[0]=& _tmp4C3,Cyc_fprintf(Cyc_stderr,((_tmp4C5="%d",_tag_dyneither(_tmp4C5,sizeof(char),3))),_tag_dyneither(_tmp4C4,sizeof(void*),1)))))));}
if(_tmp214->tl != 0){const char*_tmp4C8;void*_tmp4C7;(_tmp4C7=0,Cyc_fprintf(Cyc_stderr,((_tmp4C8=",",_tag_dyneither(_tmp4C8,sizeof(char),2))),_tag_dyneither(_tmp4C7,sizeof(void*),0)));}}
# 978
{const char*_tmp4CB;void*_tmp4CA;(_tmp4CA=0,Cyc_fprintf(Cyc_stderr,((_tmp4CB="],",_tag_dyneither(_tmp4CB,sizeof(char),3))),_tag_dyneither(_tmp4CA,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp215);
{const char*_tmp4CE;void*_tmp4CD;(_tmp4CD=0,Cyc_fprintf(Cyc_stderr,((_tmp4CE=",\n",_tag_dyneither(_tmp4CE,sizeof(char),3))),_tag_dyneither(_tmp4CD,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp216,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp217,print_rhs,tab + 7);_LL10E:;}struct _tuple19{int f1;unsigned int f2;};
# 986
static void Cyc_Tcpat_print_swrhs(struct _tuple19*x){
return;}
# 994
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp229=td;struct Cyc_Set_Set*_tmp22B;_LL116: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp22A=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp229;if(_tmp22A->tag != 1)goto _LL118;else{_tmp22B=_tmp22A->f1;}}_LL117: {
# 1004
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4D1;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4D0;return(void*)((_tmp4D0=_region_malloc(r,sizeof(*_tmp4D0)),((_tmp4D0[0]=((_tmp4D1.tag=1,((_tmp4D1.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp22B,c),_tmp4D1)))),_tmp4D0))));}_LL118: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp22C=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp229;if(_tmp22C->tag != 0)goto _LL115;}_LL119: {
const char*_tmp4D4;void*_tmp4D3;(_tmp4D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4D4="add_neg called when td is Positive",_tag_dyneither(_tmp4D4,sizeof(char),35))),_tag_dyneither(_tmp4D3,sizeof(void*),0)));}_LL115:;}
# 1011
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp231=td;struct Cyc_Tcpat_Con_s*_tmp233;struct Cyc_Set_Set*_tmp235;_LL11B: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp232=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp231;if(_tmp232->tag != 0)goto _LL11D;else{_tmp233=_tmp232->f1;}}_LL11C:
# 1015
 if(Cyc_Tcpat_compare_con(c,_tmp233)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL11D: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp234=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp231;if(_tmp234->tag != 1)goto _LL11A;else{_tmp235=_tmp234->f1;}}_LL11E:
# 1019
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp235,c))return Cyc_Tcpat_No;else{
# 1022
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp235)+ 1)
return Cyc_Tcpat_Yes;else{
# 1025
return Cyc_Tcpat_Maybe;}}_LL11A:;}struct _tuple20{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1033
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1035
struct Cyc_List_List*_tmp236=ctxt;struct Cyc_Tcpat_Con_s*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_List_List*_tmp239;_LL120: if(_tmp236 != 0)goto _LL122;_LL121:
 return 0;_LL122: if(_tmp236 == 0)goto _LL11F;_tmp237=((struct _tuple20*)_tmp236->hd)->f1;_tmp238=((struct _tuple20*)_tmp236->hd)->f2;_tmp239=_tmp236->tl;_LL123: {
# 1038
struct _tuple20*_tmp4DA;struct Cyc_List_List*_tmp4D9;struct Cyc_List_List*_tmp4D8;return(_tmp4D8=_region_malloc(r,sizeof(*_tmp4D8)),((_tmp4D8->hd=(
(_tmp4DA=_region_malloc(r,sizeof(*_tmp4DA)),((_tmp4DA->f1=_tmp237,((_tmp4DA->f2=((_tmp4D9=_region_malloc(r,sizeof(*_tmp4D9)),((_tmp4D9->hd=dsc,((_tmp4D9->tl=_tmp238,_tmp4D9)))))),_tmp4DA)))))),((_tmp4D8->tl=_tmp239,_tmp4D8)))));}_LL11F:;}
# 1046
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp23D=ctxt;struct Cyc_Tcpat_Con_s*_tmp23E;struct Cyc_List_List*_tmp23F;struct Cyc_List_List*_tmp240;_LL125: if(_tmp23D != 0)goto _LL127;_LL126: {
const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4DD="norm_context: empty context",_tag_dyneither(_tmp4DD,sizeof(char),28))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}_LL127: if(_tmp23D == 0)goto _LL124;_tmp23E=((struct _tuple20*)_tmp23D->hd)->f1;_tmp23F=((struct _tuple20*)_tmp23D->hd)->f2;_tmp240=_tmp23D->tl;_LL128: {
# 1050
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4E0;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4DF;return Cyc_Tcpat_augment(r,_tmp240,(void*)((_tmp4DF=_region_malloc(r,sizeof(*_tmp4DF)),((_tmp4DF[0]=((_tmp4E0.tag=0,((_tmp4E0.f1=_tmp23E,((_tmp4E0.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp23F),_tmp4E0)))))),_tmp4DF)))));}_LL124:;}
# 1059
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1061
struct _tuple13 _tmp4E1;struct _tuple13 _tmp246=(_tmp4E1.f1=ctxt,((_tmp4E1.f2=work,_tmp4E1)));struct Cyc_Tcpat_Con_s*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_List_List*_tmp24A;struct Cyc_List_List*_tmp24B;_LL12A: if(_tmp246.f1 != 0)goto _LL12C;if(_tmp246.f2 != 0)goto _LL12C;_LL12B:
 return dsc;_LL12C: if(_tmp246.f1 != 0)goto _LL12E;_LL12D:
 goto _LL12F;_LL12E: if(_tmp246.f2 != 0)goto _LL130;_LL12F: {
const char*_tmp4E4;void*_tmp4E3;(_tmp4E3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4E4="build_desc: ctxt and work don't match",_tag_dyneither(_tmp4E4,sizeof(char),38))),_tag_dyneither(_tmp4E3,sizeof(void*),0)));}_LL130: if(_tmp246.f1 == 0)goto _LL129;_tmp247=((struct _tuple20*)(_tmp246.f1)->hd)->f1;_tmp248=((struct _tuple20*)(_tmp246.f1)->hd)->f2;_tmp249=(_tmp246.f1)->tl;if(_tmp246.f2 == 0)goto _LL129;_tmp24A=((struct _tuple18*)(_tmp246.f2)->hd)->f3;_tmp24B=(_tmp246.f2)->tl;_LL131: {
# 1066
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp4EA;struct Cyc_List_List*_tmp4E9;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp4E8;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp24E=(_tmp4E8=_region_malloc(r,sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4EA.tag=0,((_tmp4EA.f1=_tmp247,((_tmp4EA.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp248),(
(_tmp4E9=_region_malloc(r,sizeof(*_tmp4E9)),((_tmp4E9->hd=dsc,((_tmp4E9->tl=_tmp24A,_tmp4E9))))))),_tmp4EA)))))),_tmp4E8)));
return Cyc_Tcpat_build_desc(r,_tmp249,(void*)_tmp24E,_tmp24B);}_LL129:;}
# 1072
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1079
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1081
struct Cyc_List_List*_tmp252=allmrules;void*_tmp253;void*_tmp254;struct Cyc_List_List*_tmp255;_LL133: if(_tmp252 != 0)goto _LL135;_LL134: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp4ED;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp4EC;return(void*)((_tmp4EC=_cycalloc(sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4ED.tag=0,((_tmp4ED.f1=dsc,_tmp4ED)))),_tmp4EC))));}_LL135: if(_tmp252 == 0)goto _LL132;_tmp253=((struct _tuple0*)_tmp252->hd)->f1;_tmp254=((struct _tuple0*)_tmp252->hd)->f2;_tmp255=_tmp252->tl;_LL136:
# 1084
 return Cyc_Tcpat_match(r,_tmp253,0,dsc,0,0,_tmp254,_tmp255);_LL132:;}
# 1089
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1091
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4F0;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4EF;return Cyc_Tcpat_or_match(r,(void*)((_tmp4EF=_region_malloc(r,sizeof(*_tmp4EF)),((_tmp4EF[0]=((_tmp4F0.tag=1,((_tmp4F0.f1=Cyc_Tcpat_empty_con_set(r),_tmp4F0)))),_tmp4EF)))),allmrules);}
# 1097
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1101
struct Cyc_List_List*_tmp25A=work;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp25F;_LL138: if(_tmp25A != 0)goto _LL13A;_LL139: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp4F3;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp4F2;return(void*)((_tmp4F2=_region_malloc(r,sizeof(*_tmp4F2)),((_tmp4F2[0]=((_tmp4F3.tag=1,((_tmp4F3.f1=right_hand_side,_tmp4F3)))),_tmp4F2))));}_LL13A: if(_tmp25A == 0)goto _LL13C;if(((struct _tuple18*)_tmp25A->hd)->f1 != 0)goto _LL13C;if(((struct _tuple18*)_tmp25A->hd)->f2 != 0)goto _LL13C;if(((struct _tuple18*)_tmp25A->hd)->f3 != 0)goto _LL13C;_tmp25B=_tmp25A->tl;_LL13B:
# 1104
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp25B,right_hand_side,rules);_LL13C: if(_tmp25A == 0)goto _LL137;_tmp25C=((struct _tuple18*)_tmp25A->hd)->f1;_tmp25D=((struct _tuple18*)_tmp25A->hd)->f2;_tmp25E=((struct _tuple18*)_tmp25A->hd)->f3;_tmp25F=_tmp25A->tl;_LL13D:
# 1106
 if((_tmp25C == 0  || _tmp25D == 0) || _tmp25E == 0){
const char*_tmp4F6;void*_tmp4F5;(_tmp4F5=0,Cyc_Tcutil_impos(((_tmp4F6="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp4F6,sizeof(char),38))),_tag_dyneither(_tmp4F5,sizeof(void*),0)));}{
void*_tmp265;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp264=_tmp25C;_tmp265=(void*)_tmp264->hd;_tmp266=_tmp264->tl;{
struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_List_List*_tmp267=_tmp25D;_tmp268=(struct Cyc_List_List*)_tmp267->hd;_tmp269=_tmp267->tl;{
void*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26A=_tmp25E;_tmp26B=(void*)_tmp26A->hd;_tmp26C=_tmp26A->tl;{
struct _tuple18*_tmp4F7;struct _tuple18*_tmp26D=(_tmp4F7=_region_malloc(r,sizeof(*_tmp4F7)),((_tmp4F7->f1=_tmp266,((_tmp4F7->f2=_tmp269,((_tmp4F7->f3=_tmp26C,_tmp4F7)))))));
struct Cyc_List_List*_tmp4F8;return Cyc_Tcpat_match(r,_tmp265,_tmp268,_tmp26B,ctx,((_tmp4F8=_region_malloc(r,sizeof(*_tmp4F8)),((_tmp4F8->hd=_tmp26D,((_tmp4F8->tl=_tmp25F,_tmp4F8)))))),right_hand_side,rules);};};};};_LL137:;}
# 1118
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1120
void*_tmp270=dsc;struct Cyc_Set_Set*_tmp272;struct Cyc_List_List*_tmp274;_LL13F: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp271=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp270;if(_tmp271->tag != 1)goto _LL141;else{_tmp272=_tmp271->f1;}}_LL140: {
# 1125
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp4FB;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp4FA;void*any=(void*)((_tmp4FA=_region_malloc(r,sizeof(*_tmp4FA)),((_tmp4FA[0]=((_tmp4FB.tag=1,((_tmp4FB.f1=Cyc_Tcpat_empty_con_set(r),_tmp4FB)))),_tmp4FA))));
struct Cyc_List_List*_tmp275=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp4FC;_tmp275=((_tmp4FC=_region_malloc(r,sizeof(*_tmp4FC)),((_tmp4FC->hd=any,((_tmp4FC->tl=_tmp275,_tmp4FC))))));}}
return _tmp275;}_LL141: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp273=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp270;if(_tmp273->tag != 0)goto _LL13E;else{_tmp274=_tmp273->f2;}}_LL142:
 return _tmp274;_LL13E:;}struct _tuple21{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1135
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple21*env,int i){
struct _RegionHandle*_tmp27A;struct Cyc_List_List*_tmp27B;struct _tuple21*_tmp279=env;_tmp27A=_tmp279->f1;_tmp27B=_tmp279->f2;{
struct Cyc_List_List*_tmp4FD;return(_tmp4FD=_region_malloc(_tmp27A,sizeof(*_tmp4FD)),((_tmp4FD->hd=(void*)(i + 1),((_tmp4FD->tl=_tmp27B,_tmp4FD)))));};}
# 1139
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1141
struct _tuple21 _tmp4FE;struct _tuple21 _tmp27D=(_tmp4FE.f1=r,((_tmp4FE.f2=obj,_tmp4FE)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple21*,int),struct _tuple21*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp27D);}
# 1148
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1152
void*_tmp27F=p;struct Cyc_Tcpat_Con_s*_tmp282;struct Cyc_List_List*_tmp283;_LL144: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp280=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp27F;if(_tmp280->tag != 0)goto _LL146;}_LL145:
# 1154
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL146: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp281=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp27F;if(_tmp281->tag != 1)goto _LL143;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL147:
# 1156
 switch(Cyc_Tcpat_static_match(_tmp282,dsc)){case Cyc_Tcpat_Yes: _LL148: {
# 1158
struct _tuple20*_tmp501;struct Cyc_List_List*_tmp500;struct Cyc_List_List*ctx2=(_tmp500=_region_malloc(r,sizeof(*_tmp500)),((_tmp500->hd=((_tmp501=_region_malloc(r,sizeof(*_tmp501)),((_tmp501->f1=_tmp282,((_tmp501->f2=0,_tmp501)))))),((_tmp500->tl=ctx,_tmp500)))));
struct _tuple18*_tmp502;struct _tuple18*work_frame=(_tmp502=_region_malloc(r,sizeof(*_tmp502)),((_tmp502->f1=_tmp283,((_tmp502->f2=Cyc_Tcpat_getoargs(r,_tmp282,obj),((_tmp502->f3=
Cyc_Tcpat_getdargs(r,_tmp282,dsc),_tmp502)))))));
struct Cyc_List_List*_tmp503;struct Cyc_List_List*work2=(_tmp503=_region_malloc(r,sizeof(*_tmp503)),((_tmp503->hd=work_frame,((_tmp503->tl=work,_tmp503)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL149:
# 1164
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL14A: {
# 1166
struct _tuple20*_tmp506;struct Cyc_List_List*_tmp505;struct Cyc_List_List*ctx2=(_tmp505=_region_malloc(r,sizeof(*_tmp505)),((_tmp505->hd=((_tmp506=_region_malloc(r,sizeof(*_tmp506)),((_tmp506->f1=_tmp282,((_tmp506->f2=0,_tmp506)))))),((_tmp505->tl=ctx,_tmp505)))));
struct _tuple18*_tmp507;struct _tuple18*work_frame=(_tmp507=_region_malloc(r,sizeof(*_tmp507)),((_tmp507->f1=_tmp283,((_tmp507->f2=Cyc_Tcpat_getoargs(r,_tmp282,obj),((_tmp507->f3=
Cyc_Tcpat_getdargs(r,_tmp282,dsc),_tmp507)))))));
struct Cyc_List_List*_tmp508;struct Cyc_List_List*work2=(_tmp508=_region_malloc(r,sizeof(*_tmp508)),((_tmp508->hd=work_frame,((_tmp508->tl=work,_tmp508)))));
void*_tmp288=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp289=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp282),work),rules);
# 1173
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp50B;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp50A;return(void*)((_tmp50A=_region_malloc(r,sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp50B.tag=2,((_tmp50B.f1=obj,((_tmp50B.f2=_tmp282,((_tmp50B.f3=_tmp288,((_tmp50B.f4=_tmp289,_tmp50B)))))))))),_tmp50A))));}}_LL143:;}
# 1183
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1189
void*_tmp290=d;void*_tmp292;void*_tmp294;void*_tmp296;void*_tmp297;_LL14D: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp291=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp290;if(_tmp291->tag != 0)goto _LL14F;else{_tmp292=(void*)_tmp291->f1;}}_LL14E:
# 1191
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp292);*exhaust_done=1;}
goto _LL14C;_LL14F: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp293=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp290;if(_tmp293->tag != 1)goto _LL151;else{_tmp294=(void*)_tmp293->f1;}}_LL150:
 rhs_appears(env2,_tmp294);goto _LL14C;_LL151: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp295=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp290;if(_tmp295->tag != 2)goto _LL14C;else{_tmp296=(void*)_tmp295->f3;_tmp297=(void*)_tmp295->f4;}}_LL152:
# 1195
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp296,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1197
Cyc_Tcpat_check_exhaust_overlap(r,_tmp297,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1199
goto _LL14C;_LL14C:;}struct _tuple22{void*f1;struct _tuple19*f2;};struct _tuple23{struct _RegionHandle*f1;int*f2;};
# 1207
static struct _tuple22*Cyc_Tcpat_get_match(struct _tuple23*env,struct Cyc_Absyn_Switch_clause*swc){
# 1210
struct _RegionHandle*_tmp299;int*_tmp29A;struct _tuple23*_tmp298=env;_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp299,swc->pattern);
struct _tuple19*_tmp50C;struct _tuple19*rhs=(_tmp50C=_region_malloc(_tmp299,sizeof(*_tmp50C)),((_tmp50C->f1=0,((_tmp50C->f2=(swc->pattern)->loc,_tmp50C)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp50E;sp=Cyc_Tcpat_tuple_pat(_tmp299,((_tmp50E=_region_malloc(_tmp299,sizeof(*_tmp50E)),((_tmp50E->hd=sp0,((_tmp50E->tl=(
(_tmp50F=_region_malloc(_tmp299,sizeof(*_tmp50F)),((_tmp50F->hd=Cyc_Tcpat_int_pat(_tmp299,*_tmp29A,0),((_tmp50F->tl=0,_tmp50F)))))),_tmp50E)))))),0);}
*_tmp29A=*_tmp29A + 1;}else{
# 1219
struct Cyc_List_List*_tmp519;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp518;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp517;struct Cyc_List_List*_tmp516;sp=Cyc_Tcpat_tuple_pat(_tmp299,((_tmp516=_region_malloc(_tmp299,sizeof(*_tmp516)),((_tmp516->hd=sp0,((_tmp516->tl=((_tmp519=_region_malloc(_tmp299,sizeof(*_tmp519)),((_tmp519->hd=(void*)((_tmp517=_region_malloc(_tmp299,sizeof(*_tmp517)),((_tmp517[0]=((_tmp518.tag=0,_tmp518)),_tmp517)))),((_tmp519->tl=0,_tmp519)))))),_tmp516)))))),0);}{
struct _tuple22*_tmp51A;return(_tmp51A=_region_malloc(_tmp299,sizeof(*_tmp51A)),((_tmp51A->f1=sp,((_tmp51A->f2=rhs,_tmp51A)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1225
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1227
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp2A4=d;struct Cyc_Tcpat_Con_s*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_Set_Set*_tmp2A9;_LL154: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A5=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A4;if(_tmp2A5->tag != 0)goto _LL156;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;}}_LL155: {
# 1231
union Cyc_Tcpat_Name_value _tmp2AA=_tmp2A6->name;
struct Cyc_Absyn_Pat*_tmp2AB=_tmp2A6->orig_pat;
if(_tmp2AB == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp2A7))->hd);{
# 1236
void*_tmp2AC=_tmp2AB->r;struct Cyc_Absyn_Vardecl*_tmp2AF;struct Cyc_Absyn_Vardecl*_tmp2B1;struct Cyc_Absyn_Tvar*_tmp2B3;struct Cyc_Absyn_Vardecl*_tmp2B4;struct Cyc_Absyn_Aggrdecl*_tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_Datatypefield*_tmp2BB;int _tmp2BE;char _tmp2C0;struct _dyneither_ptr _tmp2C2;int _tmp2C3;struct Cyc_Absyn_Enumfield*_tmp2C5;struct Cyc_Absyn_Enumfield*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C9;_LL159: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2AD=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2AD->tag != 0)goto _LL15B;}_LL15A: {
const char*_tmp51B;return(_tmp51B="_",_tag_dyneither(_tmp51B,sizeof(char),2));}_LL15B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2AE=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2AE->tag != 1)goto _LL15D;else{_tmp2AF=_tmp2AE->f1;}}_LL15C:
 return Cyc_Absynpp_qvar2string(_tmp2AF->name);_LL15D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2B0=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2B0->tag != 2)goto _LL15F;else{_tmp2B1=_tmp2B0->f1;}}_LL15E: {
const char*_tmp51F;void*_tmp51E[1];struct Cyc_String_pa_PrintArg_struct _tmp51D;return(struct _dyneither_ptr)((_tmp51D.tag=0,((_tmp51D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B1->name)),((_tmp51E[0]=& _tmp51D,Cyc_aprintf(((_tmp51F="*%s",_tag_dyneither(_tmp51F,sizeof(char),4))),_tag_dyneither(_tmp51E,sizeof(void*),1))))))));}_LL15F: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2B2=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2B2->tag != 3)goto _LL161;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;}}_LL160: {
const char*_tmp524;void*_tmp523[2];struct Cyc_String_pa_PrintArg_struct _tmp522;struct Cyc_String_pa_PrintArg_struct _tmp521;return(struct _dyneither_ptr)((_tmp521.tag=0,((_tmp521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B3->name),((_tmp522.tag=0,((_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B4->name)),((_tmp523[0]=& _tmp522,((_tmp523[1]=& _tmp521,Cyc_aprintf(((_tmp524="%s<`%s>",_tag_dyneither(_tmp524,sizeof(char),8))),_tag_dyneither(_tmp523,sizeof(void*),2))))))))))))));}_LL161: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2B5=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2B5->tag != 4)goto _LL163;}_LL162: {
# 1242
const char*_tmp528;void*_tmp527[1];struct Cyc_String_pa_PrintArg_struct _tmp526;return(struct _dyneither_ptr)((_tmp526.tag=0,((_tmp526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2A7)),((_tmp527[0]=& _tmp526,Cyc_aprintf(((_tmp528="$(%s)",_tag_dyneither(_tmp528,sizeof(char),6))),_tag_dyneither(_tmp527,sizeof(void*),1))))))));}_LL163: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2B6=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2B6->tag != 5)goto _LL165;}_LL164: {
# 1244
const char*_tmp52C;void*_tmp52B[1];struct Cyc_String_pa_PrintArg_struct _tmp52A;return(struct _dyneither_ptr)((_tmp52A.tag=0,((_tmp52A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2A7)),((_tmp52B[0]=& _tmp52A,Cyc_aprintf(((_tmp52C="&%s",_tag_dyneither(_tmp52C,sizeof(char),4))),_tag_dyneither(_tmp52B,sizeof(void*),1))))))));}_LL165: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2B7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2B7->tag != 6)goto _LL167;else{if(_tmp2B7->f1 == 0)goto _LL167;if((((_tmp2B7->f1)->aggr_info).KnownAggr).tag != 2)goto _LL167;_tmp2B8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2B7->f1)->aggr_info).KnownAggr).val);_tmp2B9=_tmp2B7->f3;}}_LL166:
# 1246
 if(_tmp2B8->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp2D8=_tmp2B9;struct _dyneither_ptr*_tmp2DA;_LL17A: if(_tmp2D8 == 0)goto _LL17C;if(((struct _tuple15*)_tmp2D8->hd)->f1 == 0)goto _LL17C;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp2D9=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp2D8->hd)->f1)->hd);if(_tmp2D9->tag != 1)goto _LL17C;else{_tmp2DA=_tmp2D9->f1;}};_LL17B: {
# 1249
const char*_tmp532;void*_tmp531[3];struct Cyc_String_pa_PrintArg_struct _tmp530;struct Cyc_String_pa_PrintArg_struct _tmp52F;struct Cyc_String_pa_PrintArg_struct _tmp52E;return(struct _dyneither_ptr)((_tmp52E.tag=0,((_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp2A7)),((_tmp52F.tag=0,((_tmp52F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DA),((_tmp530.tag=0,((_tmp530.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1249
Cyc_Absynpp_qvar2string(_tmp2B8->name)),((_tmp531[0]=& _tmp530,((_tmp531[1]=& _tmp52F,((_tmp531[2]=& _tmp52E,Cyc_aprintf(((_tmp532="%s{.%s = %s}",_tag_dyneither(_tmp532,sizeof(char),13))),_tag_dyneither(_tmp531,sizeof(void*),3))))))))))))))))))));}_LL17C:;_LL17D:
# 1251
 goto _LL179;_LL179:;}{
# 1254
const char*_tmp537;void*_tmp536[2];struct Cyc_String_pa_PrintArg_struct _tmp535;struct Cyc_String_pa_PrintArg_struct _tmp534;return(struct _dyneither_ptr)((_tmp534.tag=0,((_tmp534.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2A7)),((_tmp535.tag=0,((_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B8->name)),((_tmp536[0]=& _tmp535,((_tmp536[1]=& _tmp534,Cyc_aprintf(((_tmp537="%s{%s}",_tag_dyneither(_tmp537,sizeof(char),7))),_tag_dyneither(_tmp536,sizeof(void*),2))))))))))))));};_LL167: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2BA=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2BA->tag != 7)goto _LL169;else{_tmp2BB=_tmp2BA->f2;}}_LL168:
# 1256
 if(_tmp2A7 == 0){
const char*_tmp53B;void*_tmp53A[1];struct Cyc_String_pa_PrintArg_struct _tmp539;return(struct _dyneither_ptr)((_tmp539.tag=0,((_tmp539.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BB->name)),((_tmp53A[0]=& _tmp539,Cyc_aprintf(((_tmp53B="%s",_tag_dyneither(_tmp53B,sizeof(char),3))),_tag_dyneither(_tmp53A,sizeof(void*),1))))))));}else{
# 1259
const char*_tmp540;void*_tmp53F[2];struct Cyc_String_pa_PrintArg_struct _tmp53E;struct Cyc_String_pa_PrintArg_struct _tmp53D;return(struct _dyneither_ptr)((_tmp53D.tag=0,((_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2A7)),((_tmp53E.tag=0,((_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2BB->name)),((_tmp53F[0]=& _tmp53E,((_tmp53F[1]=& _tmp53D,Cyc_aprintf(((_tmp540="%s(%s)",_tag_dyneither(_tmp540,sizeof(char),7))),_tag_dyneither(_tmp53F,sizeof(void*),2))))))))))))));}_LL169: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp2BC=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2BC->tag != 8)goto _LL16B;}_LL16A: {
const char*_tmp541;return(_tmp541="NULL",_tag_dyneither(_tmp541,sizeof(char),5));}_LL16B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2BD=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2BD->tag != 9)goto _LL16D;else{_tmp2BE=_tmp2BD->f2;}}_LL16C: {
const char*_tmp545;void*_tmp544[1];struct Cyc_Int_pa_PrintArg_struct _tmp543;return(struct _dyneither_ptr)((_tmp543.tag=1,((_tmp543.f1=(unsigned long)_tmp2BE,((_tmp544[0]=& _tmp543,Cyc_aprintf(((_tmp545="%d",_tag_dyneither(_tmp545,sizeof(char),3))),_tag_dyneither(_tmp544,sizeof(void*),1))))))));}_LL16D: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2BF=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2BF->tag != 10)goto _LL16F;else{_tmp2C0=_tmp2BF->f1;}}_LL16E: {
const char*_tmp549;void*_tmp548[1];struct Cyc_Int_pa_PrintArg_struct _tmp547;return(struct _dyneither_ptr)((_tmp547.tag=1,((_tmp547.f1=(unsigned long)((int)_tmp2C0),((_tmp548[0]=& _tmp547,Cyc_aprintf(((_tmp549="%d",_tag_dyneither(_tmp549,sizeof(char),3))),_tag_dyneither(_tmp548,sizeof(void*),1))))))));}_LL16F: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2C1=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2C1->tag != 11)goto _LL171;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C1->f2;}}_LL170:
 return _tmp2C2;_LL171: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2C4=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2C4->tag != 12)goto _LL173;else{_tmp2C5=_tmp2C4->f2;}}_LL172:
 _tmp2C7=_tmp2C5;goto _LL174;_LL173: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2C6=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2C6->tag != 13)goto _LL175;else{_tmp2C7=_tmp2C6->f2;}}_LL174:
 return Cyc_Absynpp_qvar2string(_tmp2C7->name);_LL175: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2C8=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp2AC;if(_tmp2C8->tag != 16)goto _LL177;else{_tmp2C9=_tmp2C8->f1;}}_LL176:
 return Cyc_Absynpp_exp2string(_tmp2C9);_LL177:;_LL178:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL158:;};}_LL156: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A8=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A4;if(_tmp2A8->tag != 1)goto _LL153;else{_tmp2A9=_tmp2A8->f1;}}_LL157:
# 1270
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2A9)){const char*_tmp54A;return(_tmp54A="_",_tag_dyneither(_tmp54A,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp2F3=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2A9);
struct Cyc_Absyn_Pat*_tmp2F4=_tmp2F3->orig_pat;
# 1275
if(_tmp2F4 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp2F5=Cyc_Tcutil_compress((void*)_check_null(_tmp2F4->topt));struct Cyc_Absyn_PtrAtts _tmp2F9;struct Cyc_Absyn_Datatypedecl*_tmp2FB;struct Cyc_Absyn_Aggrdecl*_tmp2FD;_LL17F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2F5;if(_tmp2F6->tag != 6)goto _LL181;else{if(_tmp2F6->f2 != Cyc_Absyn_Char_sz)goto _LL181;}}_LL180:
# 1279
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp2FE=Cyc_Tcpat_char_con(r,(char)i,_tmp2F4);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,_tmp2FE)){
const char*_tmp54E;void*_tmp54D[1];struct Cyc_Int_pa_PrintArg_struct _tmp54C;return(struct _dyneither_ptr)((_tmp54C.tag=1,((_tmp54C.f1=(unsigned long)i,((_tmp54D[0]=& _tmp54C,Cyc_aprintf(((_tmp54E="%d",_tag_dyneither(_tmp54E,sizeof(char),3))),_tag_dyneither(_tmp54D,sizeof(void*),1))))))));}}}
# 1284
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL181: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2F5;if(_tmp2F7->tag != 6)goto _LL183;}_LL182:
# 1287
{unsigned int i=0;for(0;i < 0 - 1;++ i){
struct Cyc_Tcpat_Con_s*_tmp302=Cyc_Tcpat_int_con(r,(int)i,_tmp2F4);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,_tmp302)){
const char*_tmp552;void*_tmp551[1];struct Cyc_Int_pa_PrintArg_struct _tmp550;return(struct _dyneither_ptr)((_tmp550.tag=1,((_tmp550.f1=(unsigned long)((int)i),((_tmp551[0]=& _tmp550,Cyc_aprintf(((_tmp552="%d",_tag_dyneither(_tmp552,sizeof(char),3))),_tag_dyneither(_tmp551,sizeof(void*),1))))))));}}}
# 1292
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL183: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F5;if(_tmp2F8->tag != 5)goto _LL185;else{_tmp2F9=(_tmp2F8->f1).ptr_atts;}}_LL184: {
# 1294
union Cyc_Absyn_Constraint*_tmp306=_tmp2F9.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp306);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,Cyc_Tcpat_null_con(r,_tmp2F4))){
const char*_tmp553;return(_tmp553="NULL",_tag_dyneither(_tmp553,sizeof(char),5));}}{
# 1300
const char*_tmp554;return(_tmp554="&_",_tag_dyneither(_tmp554,sizeof(char),3));};}_LL185: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2F5;if(_tmp2FA->tag != 3)goto _LL187;else{if((((_tmp2FA->f1).datatype_info).KnownDatatype).tag != 2)goto _LL187;_tmp2FB=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp2FA->f1).datatype_info).KnownDatatype).val);}}_LL186:
# 1302
 if(_tmp2FB->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp309=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2FB->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp309);
for(0;(unsigned int)_tmp309;_tmp309=_tmp309->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp309->hd)->name).f2;
struct Cyc_List_List*_tmp30A=((struct Cyc_Absyn_Datatypefield*)_tmp309->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp555;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,((_tmp555=_cycalloc(sizeof(*_tmp555)),((_tmp555->name=Cyc_Tcpat_Name_v(n),((_tmp555->arity=0,((_tmp555->span=0,((_tmp555->orig_pat=_tmp2F4,_tmp555)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30A)== 0)
return n;else{
# 1312
const char*_tmp559;void*_tmp558[1];struct Cyc_String_pa_PrintArg_struct _tmp557;return(struct _dyneither_ptr)((_tmp557.tag=0,((_tmp557.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp558[0]=& _tmp557,Cyc_aprintf(((_tmp559="%s(...)",_tag_dyneither(_tmp559,sizeof(char),8))),_tag_dyneither(_tmp558,sizeof(void*),1))))))));}}}
# 1315
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL187:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F5;if(_tmp2FC->tag != 11)goto _LL189;else{if((((_tmp2FC->f1).aggr_info).KnownAggr).tag != 2)goto _LL189;_tmp2FD=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp2FC->f1).aggr_info).KnownAggr).val);}}if(!(_tmp2FD->kind == Cyc_Absyn_UnionA))goto _LL189;_LL188: {
# 1317
struct Cyc_List_List*_tmp30F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2FD->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp30F);
struct _dyneither_ptr _tmp311;struct _tuple1*_tmp310=_tmp2FD->name;_tmp311=*_tmp310->f2;
for(0;(unsigned int)_tmp30F;_tmp30F=_tmp30F->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp30F->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp55A;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,((_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A->name=Cyc_Tcpat_Name_v(n),((_tmp55A->arity=0,((_tmp55A->span=0,((_tmp55A->orig_pat=_tmp2F4,_tmp55A)))))))))))){
const char*_tmp55F;void*_tmp55E[2];struct Cyc_String_pa_PrintArg_struct _tmp55D;struct Cyc_String_pa_PrintArg_struct _tmp55C;return(struct _dyneither_ptr)((_tmp55C.tag=0,((_tmp55C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp55D.tag=0,((_tmp55D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp311),((_tmp55E[0]=& _tmp55D,((_tmp55E[1]=& _tmp55C,Cyc_aprintf(((_tmp55F="%s{.%s = _}",_tag_dyneither(_tmp55F,sizeof(char),12))),_tag_dyneither(_tmp55E,sizeof(void*),2))))))))))))));}}
# 1325
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL189:;_LL18A:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL17E:;};};_LL153:;}
# 1330
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp560;return(_tmp560=_cycalloc(sizeof(*_tmp560)),((_tmp560[0]=Cyc_Tcpat_desc2string(r,d),_tmp560)));}
# 1333
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp318=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp319=_tmp318;for(0;_tmp319 != 0;_tmp319=((struct Cyc_List_List*)_check_null(_tmp319))->tl){
if(_tmp319->tl != 0){
{struct Cyc_List_List*_tmp561;_tmp319->tl=((_tmp561=_cycalloc(sizeof(*_tmp561)),((_tmp561->hd=comma,((_tmp561->tl=_tmp319->tl,_tmp561))))));}
_tmp319=_tmp319->tl;}}}
# 1342
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp318);}
# 1345
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp31C;_push_handler(& _tmp31C);{int _tmp31E=0;if(setjmp(_tmp31C.handler))_tmp31E=1;if(!_tmp31E){
{struct _dyneither_ptr _tmp31F=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp565;void*_tmp564[1];struct Cyc_String_pa_PrintArg_struct _tmp563;(_tmp563.tag=0,((_tmp563.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31F),((_tmp564[0]=& _tmp563,Cyc_Tcutil_terr(loc,((_tmp565="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp565,sizeof(char),53))),_tag_dyneither(_tmp564,sizeof(void*),1)))))));}
# 1347
;_pop_handler();}else{void*_tmp31D=(void*)_exn_thrown;void*_tmp324=_tmp31D;void*_tmp326;_LL18C: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp325=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp324;if(_tmp325->tag != Cyc_Tcpat_Desc2string)goto _LL18E;}_LL18D:
# 1351
{const char*_tmp568;void*_tmp567;(_tmp567=0,Cyc_Tcutil_terr(loc,((_tmp568="patterns may not be exhaustive.",_tag_dyneither(_tmp568,sizeof(char),32))),_tag_dyneither(_tmp567,sizeof(void*),0)));}
goto _LL18B;_LL18E: _tmp326=_tmp324;_LL18F:(void)_throw(_tmp326);_LL18B:;}};}
# 1355
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple19*rhs){
(*rhs).f1=1;}
# 1359
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1364
struct _RegionHandle _tmp329=_new_region("r");struct _RegionHandle*r=& _tmp329;_push_region(r);
{int _tmp32A=0;
struct _tuple23 _tmp569;struct _tuple23 _tmp32B=(_tmp569.f1=r,((_tmp569.f2=& _tmp32A,_tmp569)));
struct Cyc_List_List*_tmp32C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _tuple23*,struct Cyc_Absyn_Switch_clause*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp32B,swcs);
void*_tmp32D=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp32C);
int _tmp32E=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp32D,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp32E);
# 1372
for(0;_tmp32C != 0;_tmp32C=_tmp32C->tl){
int _tmp330;unsigned int _tmp331;struct _tuple22*_tmp32F=(struct _tuple22*)_tmp32C->hd;_tmp330=(_tmp32F->f2)->f1;_tmp331=(_tmp32F->f2)->f2;
if(!_tmp330){
{const char*_tmp56C;void*_tmp56B;(_tmp56B=0,Cyc_Tcutil_terr(_tmp331,((_tmp56C="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp56C,sizeof(char),74))),_tag_dyneither(_tmp56B,sizeof(void*),0)));}
# 1377
break;}}}
# 1365
;_pop_region(r);}
# 1387
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp335;_push_handler(& _tmp335);{int _tmp337=0;if(setjmp(_tmp335.handler))_tmp337=1;if(!_tmp337){
{struct _dyneither_ptr _tmp338=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp570;void*_tmp56F[1];struct Cyc_String_pa_PrintArg_struct _tmp56E;(_tmp56E.tag=0,((_tmp56E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp338),((_tmp56F[0]=& _tmp56E,Cyc_Tcutil_warn((*pr).f1,((_tmp570="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp570,sizeof(char),45))),_tag_dyneither(_tmp56F,sizeof(void*),1)))))));}
# 1390
;_pop_handler();}else{void*_tmp336=(void*)_exn_thrown;void*_tmp33D=_tmp336;void*_tmp33F;_LL191: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp33E=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp33D;if(_tmp33E->tag != Cyc_Tcpat_Desc2string)goto _LL193;}_LL192:
# 1394
{const char*_tmp573;void*_tmp572;(_tmp572=0,Cyc_Tcutil_warn((*pr).f1,((_tmp573="pattern not exhaustive.",_tag_dyneither(_tmp573,sizeof(char),24))),_tag_dyneither(_tmp572,sizeof(void*),0)));}
goto _LL190;_LL193: _tmp33F=_tmp33D;_LL194:(void)_throw(_tmp33F);_LL190:;}};};}
# 1398
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple24{void*f1;int f2;};
# 1401
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp342=_new_region("r");struct _RegionHandle*r=& _tmp342;_push_region(r);
{struct _tuple24*_tmp576;struct Cyc_List_List*_tmp575;struct Cyc_List_List*_tmp343=(_tmp575=_region_malloc(r,sizeof(*_tmp575)),((_tmp575->hd=((_tmp576=_region_malloc(r,sizeof(*_tmp576)),((_tmp576->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp576->f2=0,_tmp576)))))),((_tmp575->tl=0,_tmp575)))));
void*_tmp344=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp343);
struct _tuple14 _tmp577;struct _tuple14 _tmp345=(_tmp577.f1=loc,((_tmp577.f2=1,_tmp577)));
int _tmp346=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp344,Cyc_Tcpat_not_exhaust_warn,& _tmp345,Cyc_Tcpat_dummy_fn,0,& _tmp346);{
# 1409
int _tmp347=_tmp345.f2;_npop_handler(0);return _tmp347;};}
# 1403
;_pop_region(r);}
# 1417
static struct _tuple22*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1419
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple19*_tmp578;struct _tuple19*rhs=(_tmp578=_region_malloc(r,sizeof(*_tmp578)),((_tmp578->f1=0,((_tmp578->f2=(swc->pattern)->loc,_tmp578)))));
struct _tuple22*_tmp579;return(_tmp579=_region_malloc(r,sizeof(*_tmp579)),((_tmp579->f1=sp0,((_tmp579->f2=rhs,_tmp579)))));}
# 1423
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1426
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1428
struct _RegionHandle _tmp34D=_new_region("r");struct _RegionHandle*r=& _tmp34D;_push_region(r);
{struct Cyc_List_List*_tmp34E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple22*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp34F=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp34E);
int _tmp350=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple19*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp34F,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp350);
# 1434
for(0;_tmp34E != 0;_tmp34E=_tmp34E->tl){
int _tmp352;unsigned int _tmp353;struct _tuple22*_tmp351=(struct _tuple22*)_tmp34E->hd;_tmp352=(_tmp351->f2)->f1;_tmp353=(_tmp351->f2)->f2;
if(!_tmp352){
{const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_Tcutil_terr(_tmp353,((_tmp57C="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp57C,sizeof(char),74))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}
break;}}}
# 1429
;_pop_region(r);}
