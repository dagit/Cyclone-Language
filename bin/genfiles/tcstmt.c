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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 932
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 960
void*Cyc_Absyn_exn_typ();
# 982
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1179
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 121 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 145
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 155
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 162
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 184
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple11{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 188
const struct _tuple11*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 195
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple12**oldtv,unsigned int loc);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 212
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 224
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 144
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 158
void Cyc_Tcutil_explain_failure();
# 160
int Cyc_Tcutil_unify(void*,void*);
# 163
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 191
struct _tuple12*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 229 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 237
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 288
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 315
int Cyc_Tcutil_new_tvar_id();
# 332
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 353
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 357
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 374
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple13{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple13*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 53
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 57
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 50 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 61
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 63
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 68
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 83
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 88
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 96
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 116
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 123
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 139 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 158
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 229 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 56 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_stmttmp0=e->r;void*_tmp0=_stmttmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp4;_LL1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 4)goto _LL3;else{_tmp2=_tmp1->f1;if(_tmp1->f2 != Cyc_Absyn_PostInc)goto _LL3;}}_LL2:
# 59
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp182;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp181;e->r=(void*)((_tmp181=_cycalloc(sizeof(*_tmp181)),((_tmp181[0]=((_tmp182.tag=4,((_tmp182.f1=_tmp2,((_tmp182.f2=Cyc_Absyn_PreInc,_tmp182)))))),_tmp181))));}goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp3->tag != 4)goto _LL5;else{_tmp4=_tmp3->f1;if(_tmp3->f2 != Cyc_Absyn_PostDec)goto _LL5;}}_LL4:
# 61
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp185;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp184;e->r=(void*)((_tmp184=_cycalloc(sizeof(*_tmp184)),((_tmp184[0]=((_tmp185.tag=4,((_tmp185.f1=_tmp4,((_tmp185.f2=Cyc_Absyn_PreDec,_tmp185)))))),_tmp184))));}goto _LL0;_LL5:;_LL6:
 goto _LL0;_LL0:;}
# 66
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}
# 72
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 74
struct _tuple0*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Tcpat_TcPatResult _tmp9=pat_res;_tmpA=_tmp9.tvars_and_bounds_opt;_tmpB=_tmp9.patvars;{
struct _tuple0 _stmttmp1=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB);struct Cyc_List_List*_tmpD;struct _tuple0 _tmpC=_stmttmp1;_tmpD=_tmpC.f1;{
struct Cyc_List_List*_tmpE=_tmpA == 0?0:(*_tmpA).f1;
struct Cyc_List_List*_tmpF=_tmpA == 0?0:(*_tmpA).f2;
struct _RegionHandle _tmp10=_new_region("r");struct _RegionHandle*r=& _tmp10;_push_region(r);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpE);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpF,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmp11=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp12=_tmpD;for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp12->hd != 0){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp12->hd)));
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp12->hd)))->rgn=_tmp11;}}}
# 90
if(where_opt != 0){
const char*_tmp186;Cyc_Tcexp_tcTest(te2,where_opt,((_tmp186="switch clause guard",_tag_dyneither(_tmp186,sizeof(char),20))));}
# 93
if(_tmpD != 0)
te2=Cyc_Tcenv_set_encloser(r,te2,s);
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 98
if(_tmpD != 0)
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 78
;_pop_region(r);};};}
# 102
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 104
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
struct Cyc_List_List*_tmp187;return(_tmp187=_region_malloc(r,sizeof(*_tmp187)),((_tmp187->hd=f(env,x->hd),((_tmp187->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp187)))));}}
# 111
static void Cyc_Tcstmt_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 114
struct _tuple13*_tmp191;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp190;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp18F;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*assump=(_tmp18E=_cycalloc(sizeof(*_tmp18E)),((_tmp18E->hd=((_tmp191=_cycalloc(sizeof(*_tmp191)),((_tmp191->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp191->f2=(void*)((_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F[0]=((_tmp190.tag=2,((_tmp190.f1=tv,_tmp190)))),_tmp18F)))),_tmp191)))))),((_tmp18E->tl=0,_tmp18E)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 117
Cyc_Tcutil_unchecked_cast(tenv,initializer,new_type,Cyc_Absyn_Other_coercion);{
struct _tuple13 _tmp192;struct _tuple13 _stmttmp2=(_tmp192.f1=Cyc_Tcutil_compress(new_type),((_tmp192.f2=Cyc_Tcutil_compress(new_type),_tmp192)));struct _tuple13 _tmp15=_stmttmp2;struct Cyc_Absyn_PtrInfo _tmp17;struct Cyc_Absyn_PtrInfo _tmp19;_LL8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15.f1;if(_tmp16->tag != 5)goto _LLA;else{_tmp17=_tmp16->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp18=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15.f2;if(_tmp18->tag != 5)goto _LLA;else{_tmp19=_tmp18->f1;}};_LL9:
 goto _LL7;_LLA:;_LLB:
{const char*_tmp195;void*_tmp194;(_tmp194=0,Cyc_Tcutil_terr(loc,((_tmp195="alias requires pointer type",_tag_dyneither(_tmp195,sizeof(char),28))),_tag_dyneither(_tmp194,sizeof(void*),0)));}goto _LL7;_LL7:;};}else{
# 123
const char*_tmp19A;void*_tmp199[2];struct Cyc_String_pa_PrintArg_struct _tmp198;struct Cyc_String_pa_PrintArg_struct _tmp197;(_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp198.tag=0,((_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp199[0]=& _tmp198,((_tmp199[1]=& _tmp197,Cyc_Tcutil_terr(loc,((_tmp19A="cannot alias value of type %s to type %s",_tag_dyneither(_tmp19A,sizeof(char),41))),_tag_dyneither(_tmp199,sizeof(void*),2)))))))))))));}}
# 130
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 133
struct _RegionHandle _tmp25=_new_region("r");struct _RegionHandle*r=& _tmp25;_push_region(r);
{struct _tuple12*tk=0;
struct Cyc_Tcenv_Tenv*_tmp26=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp26,s0,new_block);
if(tk != 0)
((*((struct _tuple12*)_check_null(tk))).f1)->kind=(*((struct _tuple12*)_check_null(tk))).f2;}
# 134
;_pop_region(r);}
# 142
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple1*Cyc_Tcstmt_stmt_temp_var(){
int _tmp27=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _dyneither_ptr*_tmp1A7;const char*_tmp1A6;void*_tmp1A5[1];struct Cyc_Int_pa_PrintArg_struct _tmp1A4;struct _tuple1*_tmp1A3;struct _tuple1*res=(_tmp1A3=_cycalloc(sizeof(*_tmp1A3)),((_tmp1A3->f1=Cyc_Absyn_Loc_n,((_tmp1A3->f2=((_tmp1A7=_cycalloc(sizeof(*_tmp1A7)),((_tmp1A7[0]=(struct _dyneither_ptr)((_tmp1A4.tag=1,((_tmp1A4.f1=(unsigned int)_tmp27,((_tmp1A5[0]=& _tmp1A4,Cyc_aprintf(((_tmp1A6="_stmttmp%X",_tag_dyneither(_tmp1A6,sizeof(char),11))),_tag_dyneither(_tmp1A5,sizeof(void*),1)))))))),_tmp1A7)))),_tmp1A3)))));
return res;}
# 156 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 158
Cyc_Tcstmt_decorate_stmt(te,s0);{
# 160
void*_stmttmp3=s0->r;void*_tmp2D=_stmttmp3;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Stmt*_tmp32;struct Cyc_Absyn_Stmt*_tmp33;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Stmt*_tmp38;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Stmt*_tmp41;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Stmt*_tmp43;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Stmt*_tmp46;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Stmt**_tmp4A;struct Cyc_Absyn_Stmt**_tmp4C;struct _dyneither_ptr*_tmp4E;struct Cyc_Absyn_Stmt**_tmp4F;struct Cyc_List_List*_tmp51;struct Cyc_Absyn_Switch_clause***_tmp52;struct _dyneither_ptr*_tmp54;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Exp**_tmp57;struct Cyc_List_List*_tmp58;struct Cyc_Absyn_Stmt*_tmp5A;struct Cyc_List_List*_tmp5B;struct Cyc_Absyn_Decl*_tmp5D;struct Cyc_Absyn_Stmt*_tmp5E;struct Cyc_Absyn_Exp*_tmp60;_LLD: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp2E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp2E->tag != 0)goto _LLF;}_LLE:
# 162
 return;_LLF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp2F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp2F->tag != 1)goto _LL11;else{_tmp30=_tmp2F->f1;}}_LL10:
# 165
 Cyc_Tcexp_tcExp(te,0,_tmp30);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp30);
return;_LL11: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp31=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp31->tag != 2)goto _LL13;else{_tmp32=_tmp31->f1;_tmp33=_tmp31->f2;}}_LL12: {
# 171
struct _RegionHandle _tmp61=_new_region("r");struct _RegionHandle*r=& _tmp61;_push_region(r);{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp1AA;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1A9;struct Cyc_Tcenv_Tenv*_tmp62=Cyc_Tcenv_set_next(r,te,(void*)((_tmp1A9=_cycalloc(sizeof(*_tmp1A9)),((_tmp1A9[0]=((_tmp1AA.tag=3,((_tmp1AA.f1=_tmp33,_tmp1AA)))),_tmp1A9)))));
Cyc_Tcstmt_tcStmt(_tmp62,_tmp32,1);}
# 175
Cyc_Tcstmt_tcStmt(te,_tmp33,1);
_npop_handler(0);return;
# 171
;_pop_region(r);}_LL13: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp34=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp34->tag != 3)goto _LL15;else{_tmp35=_tmp34->f1;}}_LL14: {
# 196 "tcstmt.cyc"
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp35 == 0){
void*_stmttmpC=Cyc_Tcutil_compress(t);void*_tmp65=_stmttmpC;_LL30: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65;if(_tmp66->tag != 0)goto _LL32;}_LL31:
 goto _LL2F;_LL32: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65;if(_tmp67->tag != 7)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp68->tag != 6)goto _LL36;}_LL35:
# 202
{const char*_tmp1AE;void*_tmp1AD[1];struct Cyc_String_pa_PrintArg_struct _tmp1AC;(_tmp1AC.tag=0,((_tmp1AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1AD[0]=& _tmp1AC,Cyc_Tcutil_warn(s0->loc,((_tmp1AE="should return a value of type %s",_tag_dyneither(_tmp1AE,sizeof(char),33))),_tag_dyneither(_tmp1AD,sizeof(void*),1)))))));}
goto _LL2F;_LL36:;_LL37:
# 205
{const char*_tmp1B2;void*_tmp1B1[1];struct Cyc_String_pa_PrintArg_struct _tmp1B0;(_tmp1B0.tag=0,((_tmp1B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1B1[0]=& _tmp1B0,Cyc_Tcutil_terr(s0->loc,((_tmp1B2="must return a value of type %s",_tag_dyneither(_tmp1B2,sizeof(char),31))),_tag_dyneither(_tmp1B1,sizeof(void*),1)))))));}
goto _LL2F;_LL2F:;}else{
# 210
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp35;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp1B7;void*_tmp1B6[2];struct Cyc_String_pa_PrintArg_struct _tmp1B5;struct Cyc_String_pa_PrintArg_struct _tmp1B4;(_tmp1B4.tag=0,((_tmp1B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1B5.tag=0,((_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp1B6[0]=& _tmp1B5,((_tmp1B6[1]=& _tmp1B4,Cyc_Tcutil_terr(s0->loc,((_tmp1B7="returns value of type %s but requires %s",_tag_dyneither(_tmp1B7,sizeof(char),41))),_tag_dyneither(_tmp1B6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 218
struct _RegionHandle _tmp73=_new_region("temp");struct _RegionHandle*temp=& _tmp73;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp1BA;void*_tmp1B9;(_tmp1B9=0,Cyc_Tcutil_terr(e->loc,((_tmp1BA="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1BA,sizeof(char),49))),_tag_dyneither(_tmp1B9,sizeof(void*),0)));}
# 219
;_pop_region(temp);};}
# 223
return;}_LL15: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp36=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp36->tag != 4)goto _LL17;else{_tmp37=_tmp36->f1;_tmp38=_tmp36->f2;_tmp39=_tmp36->f3;}}_LL16: {
# 226
const char*_tmp1BB;struct Cyc_Tcexp_TestEnv _stmttmpB=Cyc_Tcexp_tcTest(te,_tmp37,((_tmp1BB="if statement",_tag_dyneither(_tmp1BB,sizeof(char),13))));struct _tuple13*_tmp77;int _tmp78;struct Cyc_Tcexp_TestEnv _tmp76=_stmttmpB;_tmp77=_tmp76.eq;_tmp78=_tmp76.isTrue;
{struct _tuple13*_tmp79=_tmp77;void*_tmp7A;void*_tmp7B;_LL39: if(_tmp79 == 0)goto _LL3B;_tmp7A=_tmp79->f1;_tmp7B=_tmp79->f2;_LL3A:
# 229
 if(_tmp78){
Cyc_Tcstmt_tcStmtRefine(te,_tmp38,_tmp7A,_tmp7B,1);
Cyc_Tcstmt_tcStmt(te,_tmp39,1);}else{
# 234
Cyc_Tcstmt_tcStmt(te,_tmp38,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp39,_tmp7A,_tmp7B,1);}
# 237
goto _LL38;_LL3B:;_LL3C:
# 239
 Cyc_Tcstmt_tcStmt(te,_tmp38,1);
Cyc_Tcstmt_tcStmt(te,_tmp39,1);
goto _LL38;_LL38:;}
# 243
return;}_LL17: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp3A->tag != 5)goto _LL19;else{_tmp3B=(_tmp3A->f1).f1;_tmp3C=(_tmp3A->f1).f2;_tmp3D=_tmp3A->f2;}}_LL18:
# 246
 Cyc_Tcstmt_decorate_stmt(te,_tmp3C);
{const char*_tmp1BC;Cyc_Tcexp_tcTest(te,_tmp3B,((_tmp1BC="while loop",_tag_dyneither(_tmp1BC,sizeof(char),11))));}{
struct _RegionHandle _tmp7E=_new_region("r");struct _RegionHandle*r=& _tmp7E;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp3C),_tmp3D,1);
# 251
_npop_handler(0);return;
# 248
;_pop_region(r);};_LL19: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3E=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp3E->tag != 9)goto _LL1B;else{_tmp3F=_tmp3E->f1;_tmp40=(_tmp3E->f2).f1;_tmp41=(_tmp3E->f2).f2;_tmp42=(_tmp3E->f3).f1;_tmp43=(_tmp3E->f3).f2;_tmp44=_tmp3E->f4;}}_LL1A:
# 254
 Cyc_Tcstmt_decorate_stmt(te,_tmp41);
Cyc_Tcstmt_decorate_stmt(te,_tmp43);
Cyc_Tcexp_tcExp(te,0,_tmp3F);
{const char*_tmp1BD;Cyc_Tcexp_tcTest(te,_tmp40,((_tmp1BD="for loop",_tag_dyneither(_tmp1BD,sizeof(char),9))));}{
struct _RegionHandle _tmp80=_new_region("r");struct _RegionHandle*r=& _tmp80;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp81=Cyc_Tcenv_set_in_loop(r,te,_tmp43);
Cyc_Tcstmt_tcStmt(_tmp81,_tmp44,1);
Cyc_Tcexp_tcExp(_tmp81,0,_tmp42);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp42);}
# 264
_npop_handler(0);return;
# 258
;_pop_region(r);};_LL1B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp45=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp45->tag != 14)goto _LL1D;else{_tmp46=_tmp45->f1;_tmp47=(_tmp45->f2).f1;_tmp48=(_tmp45->f2).f2;}}_LL1C:
# 267
 Cyc_Tcstmt_decorate_stmt(te,_tmp48);{
struct _RegionHandle _tmp82=_new_region("r");struct _RegionHandle*r=& _tmp82;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp48),_tmp46,1);
# 271
{const char*_tmp1BE;Cyc_Tcexp_tcTest(te,_tmp47,((_tmp1BE="do loop",_tag_dyneither(_tmp1BE,sizeof(char),8))));}
_npop_handler(0);return;
# 268
;_pop_region(r);};_LL1D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp49=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp49->tag != 6)goto _LL1F;else{_tmp4A=(struct Cyc_Absyn_Stmt**)& _tmp49->f1;}}_LL1E:
# 276
 Cyc_Tcenv_process_break(te,s0,_tmp4A);return;_LL1F: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp4B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp4B->tag != 7)goto _LL21;else{_tmp4C=(struct Cyc_Absyn_Stmt**)& _tmp4B->f1;}}_LL20:
 Cyc_Tcenv_process_continue(te,s0,_tmp4C);return;_LL21: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp4D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp4D->tag != 8)goto _LL23;else{_tmp4E=_tmp4D->f1;_tmp4F=(struct Cyc_Absyn_Stmt**)& _tmp4D->f2;}}_LL22:
 Cyc_Tcenv_process_goto(te,s0,_tmp4E,_tmp4F);return;_LL23: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp50=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp50->tag != 11)goto _LL25;else{_tmp51=_tmp50->f1;_tmp52=(struct Cyc_Absyn_Switch_clause***)& _tmp50->f2;}}_LL24: {
# 281
const struct _tuple11*_tmp84=Cyc_Tcenv_process_fallthru(te,s0,_tmp52);
if(_tmp84 == (const struct _tuple11*)0){
{const char*_tmp1C1;void*_tmp1C0;(_tmp1C0=0,Cyc_Tcutil_terr(s0->loc,((_tmp1C1="fallthru not in a non-last case",_tag_dyneither(_tmp1C1,sizeof(char),32))),_tag_dyneither(_tmp1C0,sizeof(void*),0)));}
return;}{
# 286
struct Cyc_List_List*_tmp87=(*_tmp84).f2;
const struct Cyc_Tcenv_CList*_tmp88=(*_tmp84).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp87);
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp88);
for(0;_tmp89 != 0  && _tmp51 != 0;(_tmp89=_tmp89->tl,_tmp51=_tmp51->tl)){
# 293
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp51->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp51->hd,(void*)_tmp89->hd,& bogus)){
{const char*_tmp1C6;void*_tmp1C5[2];struct Cyc_String_pa_PrintArg_struct _tmp1C4;struct Cyc_String_pa_PrintArg_struct _tmp1C3;(_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 298
Cyc_Absynpp_typ2string((void*)_tmp89->hd)),((_tmp1C4.tag=0,((_tmp1C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp51->hd)->topt))),((_tmp1C5[0]=& _tmp1C4,((_tmp1C5[1]=& _tmp1C3,Cyc_Tcutil_terr(s0->loc,((_tmp1C6="fallthru argument has type %s but pattern variable has type %s",_tag_dyneither(_tmp1C6,sizeof(char),63))),_tag_dyneither(_tmp1C5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 302
struct _RegionHandle _tmp8E=_new_region("temp");struct _RegionHandle*temp=& _tmp8E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp89->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp51->hd)){
const char*_tmp1C9;void*_tmp1C8;(_tmp1C8=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp51->hd)->loc,((_tmp1C9="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1C9,sizeof(char),49))),_tag_dyneither(_tmp1C8,sizeof(void*),0)));}
# 303
;_pop_region(temp);};}
# 308
if(_tmp51 != 0){
const char*_tmp1CC;void*_tmp1CB;(_tmp1CB=0,Cyc_Tcutil_terr(s0->loc,((_tmp1CC="too many arguments in explicit fallthru",_tag_dyneither(_tmp1CC,sizeof(char),40))),_tag_dyneither(_tmp1CB,sizeof(void*),0)));}
if(_tmp89 != 0){
const char*_tmp1CF;void*_tmp1CE;(_tmp1CE=0,Cyc_Tcutil_terr(s0->loc,((_tmp1CF="too few arguments in explicit fallthru",_tag_dyneither(_tmp1CF,sizeof(char),39))),_tag_dyneither(_tmp1CE,sizeof(void*),0)));}
return;};}_LL25: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp53=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp53->tag != 13)goto _LL27;else{_tmp54=_tmp53->f1;_tmp55=_tmp53->f2;}}_LL26: {
# 320
struct _RegionHandle _tmp95=_new_region("r");struct _RegionHandle*r=& _tmp95;_push_region(r);{
struct _dyneither_ptr*_tmp1DC;const char*_tmp1DB;void*_tmp1DA[1];struct Cyc_String_pa_PrintArg_struct _tmp1D9;struct Cyc_Absyn_Tvar*_tmp1D8;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp54,_tmp55),(
(_tmp1D8=_cycalloc(sizeof(*_tmp1D8)),((_tmp1D8->name=((_tmp1DC=_cycalloc(sizeof(*_tmp1DC)),((_tmp1DC[0]=(struct _dyneither_ptr)((_tmp1D9.tag=0,((_tmp1D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp54),((_tmp1DA[0]=& _tmp1D9,Cyc_aprintf(((_tmp1DB="`%s",_tag_dyneither(_tmp1DB,sizeof(char),4))),_tag_dyneither(_tmp1DA,sizeof(void*),1)))))))),_tmp1DC)))),((_tmp1D8->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp1D8->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1D8))))))))),_tmp55,0);}
# 326
_npop_handler(0);return;
# 320
;_pop_region(r);}_LL27: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp56=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp56->tag != 10)goto _LL29;else{_tmp57=(struct Cyc_Absyn_Exp**)& _tmp56->f1;_tmp58=_tmp56->f2;}}_LL28: {
# 332
struct Cyc_Absyn_Exp*_tmp9B=*_tmp57;
if(!Cyc_Tcutil_is_var_exp(_tmp9B)){
struct _tuple1*_tmp9C=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp9D=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Stmt*_tmp9E=Cyc_Absyn_declare_stmt(_tmp9C,Cyc_Absyn_new_evar(0,0),_tmp9B,_tmp9D,s0->loc);
s0->r=_tmp9E->r;
*_tmp57=Cyc_Absyn_var_exp(_tmp9C,_tmp9B->loc);
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 343
Cyc_Tcexp_tcExp(te,0,_tmp9B);{
void*_tmp9F=(void*)_check_null(_tmp9B->topt);
# 348
struct _RegionHandle _tmpA0=_new_region("r");struct _RegionHandle*r=& _tmpA0;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp9F) && !Cyc_Tcutil_is_noalias_path(r,_tmp9B)){
const char*_tmp1DF;void*_tmp1DE;(_tmp1DE=0,Cyc_Tcutil_terr(_tmp9B->loc,((_tmp1DF="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1DF,sizeof(char),49))),_tag_dyneither(_tmp1DE,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmpA3=Cyc_Tcenv_set_in_switch(r,te);
_tmpA3=Cyc_Tcenv_clear_fallthru(r,_tmpA3);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp58);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmpA4=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmpA5=Cyc_Tcpat_tcPat(_tmpA3,_tmpA4,& _tmp9F,_tmp9B);
struct Cyc_List_List*_tmpA6=_tmpA5.tvars_and_bounds_opt == 0?0:(*_tmpA5.tvars_and_bounds_opt).f1;
# 358
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpA4->topt),_tmp9F)){
{const char*_tmp1E4;void*_tmp1E3[2];struct Cyc_String_pa_PrintArg_struct _tmp1E2;struct Cyc_String_pa_PrintArg_struct _tmp1E1;(_tmp1E1.tag=0,((_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpA4->topt))),((_tmp1E2.tag=0,((_tmp1E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp9F)),((_tmp1E3[0]=& _tmp1E2,((_tmp1E3[1]=& _tmp1E1,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((_tmp1E4="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1E4,sizeof(char),44))),_tag_dyneither(_tmp1E3,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{
# 364
Cyc_Tcpat_check_pat_regions(_tmpA3,_tmpA4,_tmpA5.patvars);}
{struct Cyc_Core_Opt*_tmp1E5;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1E5=_cycalloc(sizeof(*_tmp1E5)),((_tmp1E5->v=_tmpA5.patvars,_tmp1E5))));}
# 367
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmpA3,_tmpA5,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 369
if(_tmpA5.tvars_and_bounds_opt != 0  && (*_tmpA5.tvars_and_bounds_opt).f2 != 0)
# 371
_tmpA3=Cyc_Tcenv_clear_fallthru(r,_tmpA3);else{
# 373
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpA5.patvars)).f1);
_tmpAC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpAC);
_tmpA3=Cyc_Tcenv_set_fallthru(r,_tmpA3,_tmpA6,_tmpAC,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
# 379
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp58);
_npop_handler(0);return;
# 348
;_pop_region(r);};}_LL29: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp59=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp59->tag != 15)goto _LL2B;else{_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;}}_LL2A:
# 387
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1E8;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1E7;_tmp5A->r=(Cyc_Absyn_new_stmt((void*)((_tmp1E7=_cycalloc(sizeof(*_tmp1E7)),((_tmp1E7[0]=((_tmp1E8.tag=2,((_tmp1E8.f1=Cyc_Absyn_new_stmt(_tmp5A->r,_tmp5A->loc),((_tmp1E8.f2=Cyc_Absyn_skip_stmt(_tmp5A->loc),_tmp1E8)))))),_tmp1E7)))),_tmp5A->loc))->r;}{
# 391
struct _RegionHandle _tmpAF=_new_region("r");struct _RegionHandle*r=& _tmpAF;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp5A,1);{
# 395
struct _RegionHandle _tmpB0=_new_region("r2");struct _RegionHandle*r2=& _tmpB0;_push_region(r2);{
# 397
struct Cyc_Tcenv_Tenv*_tmpB1=Cyc_Tcenv_set_in_switch(r2,te);
_tmpB1=Cyc_Tcenv_clear_fallthru(r2,_tmpB1);{
struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5B);for(0;_tmpB2 != 0;_tmpB2=_tmpB2->tl){
struct Cyc_Absyn_Pat*_tmpB3=((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->pattern;
void*_tmpB4=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpB5=Cyc_Tcpat_tcPat(_tmpB1,_tmpB3,& _tmpB4,0);
struct Cyc_List_List*_tmpB6=_tmpB5.tvars_and_bounds_opt == 0?0:(*_tmpB5.tvars_and_bounds_opt).f1;
# 405
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpB3->topt),_tmpB4)){
const char*_tmp1EC;void*_tmp1EB[1];struct Cyc_String_pa_PrintArg_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB3->topt))),((_tmp1EB[0]=& _tmp1EA,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->loc,((_tmp1EC="expected datatype exn@ but found %s",_tag_dyneither(_tmp1EC,sizeof(char),36))),_tag_dyneither(_tmp1EB,sizeof(void*),1)))))));}else{
# 409
Cyc_Tcpat_check_pat_regions(_tmpB1,_tmpB3,_tmpB5.patvars);}
{struct Cyc_Core_Opt*_tmp1ED;((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->pat_vars=((_tmp1ED=_cycalloc(sizeof(*_tmp1ED)),((_tmp1ED->v=_tmpB5.patvars,_tmp1ED))));}
# 412
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->loc,_tmpB1,_tmpB5,((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpB2->hd)->where_clause,1);
# 414
if(_tmpB5.tvars_and_bounds_opt != 0  && (*_tmpB5.tvars_and_bounds_opt).f2 != 0)
# 416
_tmpB1=Cyc_Tcenv_clear_fallthru(r2,_tmpB1);else{
# 418
struct Cyc_List_List*_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB5.patvars)).f1);
_tmpBB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBB);
_tmpB1=Cyc_Tcenv_set_fallthru(r2,_tmpB1,_tmpB6,_tmpBB,(struct Cyc_Absyn_Switch_clause*)_tmpB2->hd);}}};}
# 424
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp5B);
_npop_handler(1);return;
# 395
;_pop_region(r2);};
# 391
;_pop_region(r);};_LL2B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp5C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp5C->tag != 12)goto _LL2D;else{_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;}}_LL2C: {
# 428
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpBC=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpBC;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 434
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpBD=te2;
void*_stmttmp4=_tmp5D->r;void*_tmpBE=_stmttmp4;struct Cyc_Absyn_Vardecl*_tmpC0;struct Cyc_Absyn_Pat*_tmpC2;struct Cyc_Core_Opt**_tmpC3;struct Cyc_Absyn_Exp**_tmpC4;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Tvar*_tmpC8;struct Cyc_Absyn_Vardecl*_tmpC9;int _tmpCA;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Tvar*_tmpCD;struct Cyc_Absyn_Vardecl*_tmpCE;struct Cyc_Absyn_Fndecl*_tmpD0;struct _dyneither_ptr*_tmpD2;struct Cyc_List_List*_tmpD3;struct _tuple1*_tmpD5;struct Cyc_List_List*_tmpD6;_LL3E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpBF->tag != 0)goto _LL40;else{_tmpC0=_tmpBF->f1;}}_LL3F: {
# 438
enum Cyc_Absyn_Scope _tmpE0;union Cyc_Absyn_Nmspace _tmpE1;struct _dyneither_ptr*_tmpE2;struct Cyc_Absyn_Tqual _tmpE3;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;void**_tmpE6;struct Cyc_List_List*_tmpE7;struct Cyc_Absyn_Vardecl*_tmpDF=_tmpC0;_tmpE0=_tmpDF->sc;_tmpE1=(_tmpDF->name)->f1;_tmpE2=(_tmpDF->name)->f2;_tmpE3=_tmpDF->tq;_tmpE4=_tmpDF->type;_tmpE5=_tmpDF->initializer;_tmpE6=(void**)& _tmpDF->rgn;_tmpE7=_tmpDF->attributes;{
void*_tmpE8=Cyc_Tcenv_curr_rgn(_tmpBD);
int is_local;
switch(_tmpE0){case Cyc_Absyn_Static: _LL5E:
 goto _LL5F;case Cyc_Absyn_Extern: _LL5F:
 goto _LL60;case Cyc_Absyn_ExternC: _LL60:
# 445
 _tmpC0->escapes=1;
is_local=0;break;case Cyc_Absyn_Abstract: _LL61:
# 448
{const char*_tmp1F0;void*_tmp1EF;(_tmp1EF=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp1F0="bad abstract scope for local variable",_tag_dyneither(_tmp1F0,sizeof(char),38))),_tag_dyneither(_tmp1EF,sizeof(void*),0)));}
goto _LL62;case Cyc_Absyn_Register: _LL62:
 goto _LL63;case Cyc_Absyn_Public: _LL63:
 is_local=1;break;}
# 454
*_tmpE6=is_local?_tmpE8:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpEB=_tmpE1;_LL66: if((_tmpEB.Loc_n).tag != 4)goto _LL68;_LL67:
 goto _LL65;_LL68: if((_tmpEB.Rel_n).tag != 1)goto _LL6A;if((struct Cyc_List_List*)(_tmpEB.Rel_n).val != 0)goto _LL6A;_LL69:
# 458
(*_tmpC0->name).f1=Cyc_Absyn_Loc_n;
goto _LL65;_LL6A: if((_tmpEB.C_n).tag != 3)goto _LL6C;_LL6B:
# 462
 goto _LL6D;_LL6C: if((_tmpEB.Abs_n).tag != 2)goto _LL6E;_LL6D:
 goto _LL6F;_LL6E:;_LL6F:
# 465
{const char*_tmp1F3;void*_tmp1F2;(_tmp1F2=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp1F3="cannot declare a qualified local variable",_tag_dyneither(_tmp1F3,sizeof(char),42))),_tag_dyneither(_tmp1F2,sizeof(void*),0)));}
(*_tmpC0->name).f1=Cyc_Absyn_Loc_n;
goto _LL65;_LL65:;}
# 470
{void*_stmttmp9=Cyc_Tcutil_compress(_tmpE4);void*_tmpEE=_stmttmp9;void*_tmpF1;struct Cyc_Absyn_Tqual _tmpF2;union Cyc_Absyn_Constraint*_tmpF3;unsigned int _tmpF4;_LL71:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpEE;if(_tmpEF->tag != 9)goto _LL73;}if(!is_local)goto _LL73;_LL72:
# 473
 _tmpC0->escapes=1;
_tmpE0=Cyc_Absyn_Extern;
is_local=0;
goto _LL70;_LL73:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpEE;if(_tmpF0->tag != 8)goto _LL75;else{_tmpF1=(_tmpF0->f1).elt_type;_tmpF2=(_tmpF0->f1).tq;if((_tmpF0->f1).num_elts != 0)goto _LL75;_tmpF3=(_tmpF0->f1).zero_term;_tmpF4=(_tmpF0->f1).zt_loc;}}if(!(_tmpC0->initializer != 0))goto _LL75;_LL74:
# 478
{void*_stmttmpA=((struct Cyc_Absyn_Exp*)_check_null(_tmpC0->initializer))->r;void*_tmpF5=_stmttmpA;struct _dyneither_ptr _tmpF7;struct _dyneither_ptr _tmpF9;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmp101;_LL78: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpF6->tag != 0)goto _LL7A;else{if(((_tmpF6->f1).String_c).tag != 8)goto _LL7A;_tmpF7=(struct _dyneither_ptr)((_tmpF6->f1).String_c).val;}}_LL79:
# 480
 _tmpE4=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpF1,_tmpF2,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpF7,sizeof(char)),0),_tmpF3,_tmpF4));
goto _LL77;_LL7A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpF8->tag != 0)goto _LL7C;else{if(((_tmpF8->f1).Wstring_c).tag != 9)goto _LL7C;_tmpF9=(struct _dyneither_ptr)((_tmpF8->f1).Wstring_c).val;}}_LL7B:
# 483
 _tmpE4=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpF1,_tmpF2,Cyc_Absyn_uint_exp(1,0),_tmpF3,_tmpF4));
goto _LL77;_LL7C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpFA->tag != 26)goto _LL7E;else{_tmpFB=_tmpFA->f2;}}_LL7D:
 _tmpFD=_tmpFB;goto _LL7F;_LL7E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpFC->tag != 27)goto _LL80;else{_tmpFD=_tmpFC->f1;}}_LL7F:
# 488
 _tmpE4=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpF1,_tmpF2,_tmpFD,_tmpF3,_tmpF4));
goto _LL77;_LL80: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmpFE->tag != 35)goto _LL82;else{_tmpFF=_tmpFE->f2;}}_LL81:
 _tmp101=_tmpFF;goto _LL83;_LL82: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF5;if(_tmp100->tag != 25)goto _LL84;else{_tmp101=_tmp100->f1;}}_LL83:
# 492
 _tmpE4=(_tmpC0->type=Cyc_Absyn_array_typ(_tmpF1,_tmpF2,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101),0),_tmpF3,_tmpF4));
# 494
goto _LL77;_LL84:;_LL85:
 goto _LL77;_LL77:;}
# 497
goto _LL70;_LL75:;_LL76:
 goto _LL70;_LL70:;}{
# 504
struct Cyc_List_List*_tmp102=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpBD);
int _tmp103=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpBD,_tmp102,& Cyc_Tcutil_tmk,_tmp103,_tmpE4);
(_tmpC0->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpC0->tq).print_const,_tmpE4);{
# 509
struct Cyc_Tcenv_Tenv*_tmp104=Cyc_Tcenv_add_local_var(decl_rgn,_tmp5E->loc,_tmpBD,_tmpC0);
if(_tmpE0 == Cyc_Absyn_Extern  || _tmpE0 == Cyc_Absyn_ExternC){
const char*_tmp1F6;void*_tmp1F5;(_tmp1F5=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp1F6="extern declarations are not yet supported within functions",_tag_dyneither(_tmp1F6,sizeof(char),59))),_tag_dyneither(_tmp1F5,sizeof(void*),0)));}
if(_tmpE5 != 0){
Cyc_Tcexp_tcExpInitializer(_tmp104,& _tmpE4,_tmpE5);
# 515
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpE5)){
const char*_tmp1F9;void*_tmp1F8;(_tmp1F8=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp1F9="initializer for static variable needs to be a constant expression",_tag_dyneither(_tmp1F9,sizeof(char),66))),_tag_dyneither(_tmp1F8,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(_tmp104,_tmpE5,_tmpE4)){
struct _dyneither_ptr _tmp109=*_tmpE2;
struct _dyneither_ptr _tmp10A=Cyc_Absynpp_typ2string(_tmpE4);
struct _dyneither_ptr _tmp10B=Cyc_Absynpp_typ2string((void*)_check_null(_tmpE5->topt));
if(((_get_dyneither_size(_tmp109,sizeof(char))+ _get_dyneither_size(_tmp10A,sizeof(char)))+ _get_dyneither_size(_tmp10B,sizeof(char)))+ 50 < 80){
const char*_tmp1FF;void*_tmp1FE[3];struct Cyc_String_pa_PrintArg_struct _tmp1FD;struct Cyc_String_pa_PrintArg_struct _tmp1FC;struct Cyc_String_pa_PrintArg_struct _tmp1FB;(_tmp1FB.tag=0,((_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10B),((_tmp1FC.tag=0,((_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp1FD.tag=0,((_tmp1FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((_tmp1FE[0]=& _tmp1FD,((_tmp1FE[1]=& _tmp1FC,((_tmp1FE[2]=& _tmp1FB,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp1FF="%s was declared with type %s but initialized with type %s.",_tag_dyneither(_tmp1FF,sizeof(char),59))),_tag_dyneither(_tmp1FE,sizeof(void*),3)))))))))))))))))));}else{
# 524
if((_get_dyneither_size(_tmp109,sizeof(char))+ _get_dyneither_size(_tmp10A,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmp10B,sizeof(char))+ 25 < 80){
const char*_tmp205;void*_tmp204[3];struct Cyc_String_pa_PrintArg_struct _tmp203;struct Cyc_String_pa_PrintArg_struct _tmp202;struct Cyc_String_pa_PrintArg_struct _tmp201;(_tmp201.tag=0,((_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10B),((_tmp202.tag=0,((_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp203.tag=0,((_tmp203.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((_tmp204[0]=& _tmp203,((_tmp204[1]=& _tmp202,((_tmp204[2]=& _tmp201,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp205="%s was declared with type %s\n but initialized with type %s.",_tag_dyneither(_tmp205,sizeof(char),60))),_tag_dyneither(_tmp204,sizeof(void*),3)))))))))))))))))));}else{
# 529
const char*_tmp20B;void*_tmp20A[3];struct Cyc_String_pa_PrintArg_struct _tmp209;struct Cyc_String_pa_PrintArg_struct _tmp208;struct Cyc_String_pa_PrintArg_struct _tmp207;(_tmp207.tag=0,((_tmp207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10B),((_tmp208.tag=0,((_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10A),((_tmp209.tag=0,((_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp109),((_tmp20A[0]=& _tmp209,((_tmp20A[1]=& _tmp208,((_tmp20A[2]=& _tmp207,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp20B="%s declared with type \n%s\n but initialized with type \n%s.",_tag_dyneither(_tmp20B,sizeof(char),58))),_tag_dyneither(_tmp20A,sizeof(void*),3)))))))))))))))))));}}
# 531
Cyc_Tcutil_unify(_tmpE4,(void*)_check_null(_tmpE5->topt));
Cyc_Tcutil_explain_failure();}}
# 535
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp104,s0),_tmp5E,0);
_npop_handler(0);return;};};};}_LL40: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpC1=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpC1->tag != 2)goto _LL42;else{_tmpC2=_tmpC1->f1;_tmpC3=(struct Cyc_Core_Opt**)& _tmpC1->f2;_tmpC4=(struct Cyc_Absyn_Exp**)& _tmpC1->f3;}}_LL41: {
# 543
struct Cyc_Absyn_Exp*_tmp11B=*_tmpC4;
{void*_stmttmp8=_tmpC2->r;void*_tmp11C=_stmttmp8;_LL87: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp11D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp11C;if(_tmp11D->tag != 1)goto _LL89;}_LL88:
 goto _LL8A;_LL89: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp11E=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp11C;if(_tmp11E->tag != 14)goto _LL8B;}_LL8A:
 goto _LL86;_LL8B:;_LL8C:
# 548
 if(!Cyc_Tcutil_is_var_exp(_tmp11B)){
struct _tuple1*_tmp11F=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp120=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Stmt*_tmp121=Cyc_Absyn_declare_stmt(_tmp11F,Cyc_Absyn_new_evar(0,0),_tmp11B,_tmp120,s0->loc);
s0->r=_tmp121->r;
*_tmpC4=Cyc_Absyn_var_exp(_tmp11F,_tmp11B->loc);
Cyc_Tcstmt_tcStmt(_tmpBD,s0,new_block);
_npop_handler(0);return;}_LL86:;}
# 558
Cyc_Tcexp_tcExpInitializer(_tmpBD,0,_tmp11B);{
# 560
void*pat_type=(void*)_check_null(_tmp11B->topt);
struct Cyc_Tcpat_TcPatResult _tmp122=Cyc_Tcpat_tcPat(_tmpBD,_tmpC2,& pat_type,_tmp11B);
{struct Cyc_Core_Opt*_tmp20C;*_tmpC3=((_tmp20C=_cycalloc(sizeof(*_tmp20C)),((_tmp20C->v=_tmp122.patvars,_tmp20C))));}
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpC2->topt),(void*)_check_null(_tmp11B->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpBD,_tmp11B,(void*)_check_null(_tmpC2->topt))){
{const char*_tmp211;void*_tmp210[2];struct Cyc_String_pa_PrintArg_struct _tmp20F;struct Cyc_String_pa_PrintArg_struct _tmp20E;(_tmp20E.tag=0,((_tmp20E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp11B->topt))),((_tmp20F.tag=0,((_tmp20F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpC2->topt))),((_tmp210[0]=& _tmp20F,((_tmp210[1]=& _tmp20E,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp211="pattern type %s does not match definition type %s",_tag_dyneither(_tmp211,sizeof(char),50))),_tag_dyneither(_tmp210,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpC2->topt),(void*)_check_null(_tmp11B->topt));
Cyc_Tcutil_explain_failure();}else{
# 571
Cyc_Tcpat_check_pat_regions(_tmpBD,_tmpC2,_tmp122.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpC2->loc,_tmpBD,_tmpC2);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpBD,_tmp122,_tmp5E,0,0);
_npop_handler(0);return;};}_LL42: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpC5=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpC5->tag != 3)goto _LL44;else{_tmpC6=_tmpC5->f1;}}_LL43: {
# 577
void*_tmp128=Cyc_Tcenv_curr_rgn(_tmpBD);
struct Cyc_Tcenv_Tenv*_tmp129=_tmpBD;
for(0;_tmpC6 != 0;_tmpC6=_tmpC6->tl){
struct Cyc_Absyn_Vardecl*_tmp12A=(struct Cyc_Absyn_Vardecl*)_tmpC6->hd;
union Cyc_Absyn_Nmspace _tmp12C;void*_tmp12D;void**_tmp12E;struct Cyc_Absyn_Vardecl*_tmp12B=_tmp12A;_tmp12C=(_tmp12B->name)->f1;_tmp12D=_tmp12B->type;_tmp12E=(void**)& _tmp12B->rgn;
*_tmp12E=_tmp128;
{union Cyc_Absyn_Nmspace _tmp12F=_tmp12C;struct Cyc_List_List*_tmp130;_LL8E: if((_tmp12F.Loc_n).tag != 4)goto _LL90;_LL8F:
 goto _LL8D;_LL90: if((_tmp12F.Rel_n).tag != 1)goto _LL92;if((struct Cyc_List_List*)(_tmp12F.Rel_n).val != 0)goto _LL92;_LL91:
# 586
(*_tmp12A->name).f1=Cyc_Absyn_Loc_n;
goto _LL8D;_LL92: if((_tmp12F.Abs_n).tag != 2)goto _LL94;_tmp130=(struct Cyc_List_List*)(_tmp12F.Abs_n).val;_LL93: {
const char*_tmp214;void*_tmp213;(int)_throw(((_tmp213=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp214="tcstmt: Abs_n declaration",_tag_dyneither(_tmp214,sizeof(char),26))),_tag_dyneither(_tmp213,sizeof(void*),0)))));}_LL94:;_LL95:
# 592
{const char*_tmp217;void*_tmp216;(_tmp216=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp217="cannot declare a qualified local variable",_tag_dyneither(_tmp217,sizeof(char),42))),_tag_dyneither(_tmp216,sizeof(void*),0)));}
goto _LL8D;_LL8D:;}
# 595
Cyc_Tcutil_check_type(s0->loc,_tmp129,Cyc_Tcenv_lookup_type_vars(_tmp129),& Cyc_Tcutil_tmk,1,_tmp12D);
_tmp129=Cyc_Tcenv_add_local_var(decl_rgn,_tmp5E->loc,_tmp129,_tmp12A);}
# 598
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp129,s0),_tmp5E,0);
_npop_handler(0);return;}_LL44: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpC7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpC7->tag != 4)goto _LL46;else{_tmpC8=_tmpC7->f1;_tmpC9=_tmpC7->f2;_tmpCA=_tmpC7->f3;_tmpCB=_tmpC7->f4;}}_LL45:
# 604
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp21A;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp219;_tmp5E->r=(Cyc_Absyn_new_stmt((void*)((_tmp219=_cycalloc(sizeof(*_tmp219)),((_tmp219[0]=((_tmp21A.tag=2,((_tmp21A.f1=Cyc_Absyn_new_stmt(_tmp5E->r,_tmp5E->loc),((_tmp21A.f2=Cyc_Absyn_skip_stmt(_tmp5E->loc),_tmp21A)))))),_tmp219)))),_tmp5E->loc))->r;}{
# 606
struct Cyc_Tcenv_Tenv*_tmp137=_tmpBD;
void**_tmp139;void**_tmp13A;struct Cyc_Absyn_Vardecl*_tmp138=_tmpC9;_tmp139=(void**)& _tmp138->type;_tmp13A=(void**)& _tmp138->rgn;{
void*_tmp13B=Cyc_Tcenv_curr_rgn(_tmpBD);
*_tmp13A=_tmp13B;{
void*rgn_typ;
if(_tmpCB != 0){
struct Cyc_Absyn_Exp*open_exp=_tmpCB;
struct Cyc_List_List*_tmp13C=Cyc_Tcenv_lookup_type_vars(_tmpBD);
{struct Cyc_Core_Opt*_tmp21B;rgn_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp21B=_cycalloc(sizeof(*_tmp21B)),((_tmp21B->v=_tmp13C,_tmp21B)))));}{
struct Cyc_Core_Opt*_tmp21C;void*rgn2_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp21C=_cycalloc(sizeof(*_tmp21C)),((_tmp21C->v=_tmp13C,_tmp21C)))));
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp21F;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp21E;void*expected=(void*)((_tmp21E=_cycalloc(sizeof(*_tmp21E)),((_tmp21E[0]=((_tmp21F.tag=16,((_tmp21F.f1=rgn_typ,((_tmp21F.f2=rgn2_typ,_tmp21F)))))),_tmp21E))));
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp222;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp221;*_tmp139=(void*)((_tmp221=_cycalloc(sizeof(*_tmp221)),((_tmp221[0]=((_tmp222.tag=15,((_tmp222.f1=rgn_typ,_tmp222)))),_tmp221))));}
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmpBD,& expected,open_exp),expected)){
{const char*_tmp227;void*_tmp226[2];struct Cyc_String_pa_PrintArg_struct _tmp225;struct Cyc_String_pa_PrintArg_struct _tmp224;(_tmp224.tag=0,((_tmp224.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 621
Cyc_Absynpp_typ2string((void*)_check_null(open_exp->topt))),((_tmp225.tag=0,((_tmp225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 620
Cyc_Absynpp_typ2string(expected)),((_tmp226[0]=& _tmp225,((_tmp226[1]=& _tmp224,Cyc_Tcutil_terr(open_exp->loc,((_tmp227="expecting %s but found %s",_tag_dyneither(_tmp227,sizeof(char),26))),_tag_dyneither(_tmp226,sizeof(void*),2)))))))))))));}{
# 622
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp22A;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp229;(int)_throw((void*)((_tmp229=_cycalloc_atomic(sizeof(*_tmp229)),((_tmp229[0]=((_tmp22A.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp22A)),_tmp229)))));};}
# 625
Cyc_Tcenv_check_rgn_accessible(_tmpBD,open_exp->loc,rgn2_typ);
# 627
_tmp137=Cyc_Tcenv_add_region(decl_rgn,_tmp137,rgn_typ,0,1);};}else{
# 629
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp22D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp22C;rgn_typ=(void*)((_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C[0]=((_tmp22D.tag=2,((_tmp22D.f1=_tmpC8,_tmp22D)))),_tmp22C))));}
{struct Cyc_List_List*_tmp22E;_tmp137=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp137,((_tmp22E=_cycalloc(sizeof(*_tmp22E)),((_tmp22E->hd=_tmpC8,((_tmp22E->tl=0,_tmp22E)))))));}
_tmp137=Cyc_Tcenv_add_region(decl_rgn,_tmp137,rgn_typ,_tmpCA,1);}
# 633
Cyc_Tcutil_check_type(s0->loc,_tmpBD,Cyc_Tcenv_lookup_type_vars(_tmp137),& Cyc_Tcutil_bk,1,*_tmp139);
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp231;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp230;if(!Cyc_Tcutil_unify(*_tmp139,(void*)((_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230[0]=((_tmp231.tag=15,((_tmp231.f1=rgn_typ,_tmp231)))),_tmp230)))))){
const char*_tmp234;void*_tmp233;(_tmp233=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp234="region stmt: type of region handle is wrong!",_tag_dyneither(_tmp234,sizeof(char),45))),_tag_dyneither(_tmp233,sizeof(void*),0)));}}
# 638
if(!Cyc_Absyn_no_regions)
_tmp137=Cyc_Tcenv_enter_try(decl_rgn,_tmp137);
# 641
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp5E->loc,_tmp137,_tmpC9),s0),_tmp5E,0);
_npop_handler(0);return;};};};_LL46: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmpCC=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpCC->tag != 5)goto _LL48;else{_tmpCD=_tmpCC->f1;_tmpCE=_tmpCC->f2;}}_LL47: {
# 646
struct _RegionHandle _tmp150=_new_region("r");struct _RegionHandle*r=& _tmp150;_push_region(r);{
# 649
struct Cyc_Absyn_Tvar*_tmp235[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,s0->loc,_tmpBD,((_tmp235[0]=_tmpCD,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp235,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp238;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp237;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp237=_cycalloc(sizeof(*_tmp237)),((_tmp237[0]=((_tmp238.tag=2,((_tmp238.f1=_tmpCD,_tmp238)))),_tmp237)))),0,1);}
Cyc_Tcutil_check_type(s0->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,_tmpCE->type);{
struct Cyc_Absyn_Exp*_tmp153=_tmpCE->initializer;
if(_tmp153 == 0){
{const char*_tmp23B;void*_tmp23A;(_tmp23A=0,Cyc_Tcutil_terr(s0->loc,((_tmp23B="alias declaration requires initializer",_tag_dyneither(_tmp23B,sizeof(char),39))),_tag_dyneither(_tmp23A,sizeof(void*),0)));}
_npop_handler(1);return;}
# 657
if(_tmpCE->sc != Cyc_Absyn_Public){
const char*_tmp23E;void*_tmp23D;(_tmp23D=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp23E="bad scope for alias declaration",_tag_dyneither(_tmp23E,sizeof(char),32))),_tag_dyneither(_tmp23D,sizeof(void*),0)));}
_tmpCE->rgn=Cyc_Tcenv_curr_rgn(_tmpBD);
{union Cyc_Absyn_Nmspace _stmttmp7=(*_tmpCE->name).f1;union Cyc_Absyn_Nmspace _tmp158=_stmttmp7;_LL97: if((_tmp158.Loc_n).tag != 4)goto _LL99;_LL98:
 goto _LL96;_LL99: if((_tmp158.Rel_n).tag != 1)goto _LL9B;if((struct Cyc_List_List*)(_tmp158.Rel_n).val != 0)goto _LL9B;_LL9A:
(*_tmpCE->name).f1=Cyc_Absyn_Loc_n;goto _LL96;_LL9B:;_LL9C:
{const char*_tmp241;void*_tmp240;(_tmp240=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp241="cannot declare a qualified local variable",_tag_dyneither(_tmp241,sizeof(char),42))),_tag_dyneither(_tmp240,sizeof(void*),0)));}
(*_tmpCE->name).f1=Cyc_Absyn_Loc_n;goto _LL96;_LL96:;}
# 666
(_tmpCE->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpCE->tq).print_const,_tmpCE->type);
# 669
te2=Cyc_Tcenv_add_local_var(r,s0->loc,te2,_tmpCE);
te2=Cyc_Tcenv_set_encloser(r,te2,_tmp5E);
# 675
Cyc_Tcexp_tcExpInitializer(_tmpBD,0,_tmp153);
# 679
Cyc_Tcstmt_check_alias_coercion(te2,_tmp5D->loc,(void*)_check_null(_tmp153->topt),_tmpCD,_tmpCE->type,_tmp153);
# 682
Cyc_Tcstmt_tcStmt(te2,_tmp5E,0);};}
# 684
Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(_tmpBD));
_npop_handler(1);return;
# 646
;_pop_region(r);}_LL48: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpCF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpCF->tag != 1)goto _LL4A;else{_tmpD0=_tmpCF->f1;}}_LL49: {
# 689
void*_tmp15C=Cyc_Tcenv_curr_rgn(_tmpBD);
if(_tmpD0->sc != Cyc_Absyn_Public){
const char*_tmp244;void*_tmp243;(_tmp243=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp244="bad storage class for inner function",_tag_dyneither(_tmp244,sizeof(char),37))),_tag_dyneither(_tmp243,sizeof(void*),0)));}
# 693
{union Cyc_Absyn_Nmspace _stmttmp5=(*_tmpD0->name).f1;union Cyc_Absyn_Nmspace _tmp15F=_stmttmp5;struct Cyc_List_List*_tmp160;_LL9E: if((_tmp15F.Rel_n).tag != 1)goto _LLA0;if((struct Cyc_List_List*)(_tmp15F.Rel_n).val != 0)goto _LLA0;_LL9F:
 goto _LL9D;_LLA0: if((_tmp15F.Abs_n).tag != 2)goto _LLA2;_tmp160=(struct Cyc_List_List*)(_tmp15F.Abs_n).val;_LLA1: {
const char*_tmp247;void*_tmp246;(_tmp246=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp247="tc: Abs_n in tcStmt var decl",_tag_dyneither(_tmp247,sizeof(char),29))),_tag_dyneither(_tmp246,sizeof(void*),0)));}_LLA2:;_LLA3: {
const char*_tmp24A;void*_tmp249;(_tmp249=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp24A="explicit namespace not allowed on inner function declaration",_tag_dyneither(_tmp24A,sizeof(char),61))),_tag_dyneither(_tmp249,sizeof(void*),0)));}_LL9D:;}
# 698
(*_tmpD0->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp5D->loc,_tmpBD,_tmpD0);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpD0);
_tmpD0->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpD0->attributes);
# 703
{struct Cyc_List_List*atts=_tmpD0->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_stmttmp6=(void*)atts->hd;void*_tmp165=_stmttmp6;_LLA5: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp166=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp165;if(_tmp166->tag != 7)goto _LLA7;}_LLA6:
 goto _LLA8;_LLA7: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp167=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp165;if(_tmp167->tag != 6)goto _LLA9;}_LLA8:
{const char*_tmp24E;void*_tmp24D[1];struct Cyc_String_pa_PrintArg_struct _tmp24C;(_tmp24C.tag=0,((_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp24D[0]=& _tmp24C,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp24E="bad attribute %s for function",_tag_dyneither(_tmp24E,sizeof(char),30))),_tag_dyneither(_tmp24D,sizeof(void*),1)))))));}
goto _LLA4;_LLA9:;_LLAA:
 goto _LLA4;_LLA4:;}}{
# 711
struct Cyc_Absyn_Vardecl*_tmp24F;struct Cyc_Absyn_Vardecl*vd=(_tmp24F=_cycalloc(sizeof(*_tmp24F)),((_tmp24F->sc=_tmpD0->sc,((_tmp24F->name=_tmpD0->name,((_tmp24F->tq=Cyc_Absyn_const_tqual(0),((_tmp24F->type=
Cyc_Absyn_at_typ(t,_tmp15C,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp24F->initializer=0,((_tmp24F->rgn=_tmp15C,((_tmp24F->attributes=0,((_tmp24F->escapes=0,_tmp24F)))))))))))))))));
# 715
_tmpD0->fn_vardecl=vd;
_tmpBD=Cyc_Tcenv_add_local_var(decl_rgn,_tmp5D->loc,_tmpBD,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpBD->le);
_tmpBD->le=Cyc_Tcenv_nested_fenv(_tmp5D->loc,old_fenv,_tmpD0);
Cyc_Tcstmt_tcStmt(_tmpBD,_tmpD0->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpBD);
Cyc_Tcenv_check_delayed_constraints(_tmpBD);
if(!Cyc_Tcenv_all_labels_resolved(_tmpBD)){
const char*_tmp252;void*_tmp251;(_tmp251=0,Cyc_Tcutil_terr(_tmp5D->loc,((_tmp252="function has goto statements to undefined labels",_tag_dyneither(_tmp252,sizeof(char),49))),_tag_dyneither(_tmp251,sizeof(void*),0)));}
_tmpBD->le=old_fenv;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpBD,s0),_tmp5E,0);
_npop_handler(0);return;};};};}_LL4A: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpD1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpD1->tag != 10)goto _LL4C;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;}}_LL4B:
{const char*_tmp253;unimp_msg_part=((_tmp253="namespace",_tag_dyneither(_tmp253,sizeof(char),10)));}goto _LL3D;_LL4C: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpD4=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpD4->tag != 11)goto _LL4E;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;}}_LL4D:
{const char*_tmp254;unimp_msg_part=((_tmp254="using",_tag_dyneither(_tmp254,sizeof(char),6)));}goto _LL3D;_LL4E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD7=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpD7->tag != 6)goto _LL50;}_LL4F:
{const char*_tmp255;unimp_msg_part=((_tmp255="type",_tag_dyneither(_tmp255,sizeof(char),5)));}goto _LL3D;_LL50: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpD8=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpD8->tag != 7)goto _LL52;}_LL51:
{const char*_tmp256;unimp_msg_part=((_tmp256="datatype",_tag_dyneither(_tmp256,sizeof(char),9)));}goto _LL3D;_LL52: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpD9=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpD9->tag != 8)goto _LL54;}_LL53:
{const char*_tmp257;unimp_msg_part=((_tmp257="enum",_tag_dyneither(_tmp257,sizeof(char),5)));}goto _LL3D;_LL54: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpDA=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpDA->tag != 9)goto _LL56;}_LL55:
{const char*_tmp258;unimp_msg_part=((_tmp258="typedef",_tag_dyneither(_tmp258,sizeof(char),8)));}goto _LL3D;_LL56: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpDB=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpDB->tag != 12)goto _LL58;}_LL57:
{const char*_tmp259;unimp_msg_part=((_tmp259="extern \"C\"",_tag_dyneither(_tmp259,sizeof(char),11)));}goto _LL3D;_LL58: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpDC=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpDC->tag != 13)goto _LL5A;}_LL59:
# 735
{const char*_tmp25A;unimp_msg_part=((_tmp25A="extern \"C include\"",_tag_dyneither(_tmp25A,sizeof(char),19)));}goto _LL3D;_LL5A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpDD=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpDD->tag != 14)goto _LL5C;}_LL5B:
{const char*_tmp25B;unimp_msg_part=((_tmp25B="__cyclone_port_on__",_tag_dyneither(_tmp25B,sizeof(char),20)));}goto _LL3D;_LL5C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpDE=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpBE;if(_tmpDE->tag != 15)goto _LL3D;}_LL5D:
{const char*_tmp25C;unimp_msg_part=((_tmp25C="__cyclone_port_off__",_tag_dyneither(_tmp25C,sizeof(char),21)));}goto _LL3D;_LL3D:;};}{
# 740
const char*_tmp260;void*_tmp25F[1];struct Cyc_String_pa_PrintArg_struct _tmp25E;(int)_throw(((_tmp25E.tag=0,((_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp25F[0]=& _tmp25E,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp260="tcStmt: nested %s declarations unimplemented",_tag_dyneither(_tmp260,sizeof(char),45))),_tag_dyneither(_tmp25F,sizeof(void*),1)))))))));};
# 429
;_pop_region(decl_rgn);}_LL2D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp5F=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp2D;if(_tmp5F->tag != 16)goto _LLC;else{_tmp60=_tmp5F->f1;}}_LL2E: {
# 744
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp263;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp262;void*etype=(void*)((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262[0]=((_tmp263.tag=15,((_tmp263.f1=rgn_type,_tmp263)))),_tmp262))));
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp60),etype)){
const char*_tmp267;void*_tmp266[1];struct Cyc_String_pa_PrintArg_struct _tmp265;(_tmp265.tag=0,((_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp60->topt))),((_tmp266[0]=& _tmp265,Cyc_Tcutil_terr(_tmp60->loc,((_tmp267="expecting region_t but found %s",_tag_dyneither(_tmp267,sizeof(char),32))),_tag_dyneither(_tmp266,sizeof(void*),1)))))));}
# 749
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LLC:;};}
